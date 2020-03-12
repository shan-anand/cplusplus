//////////////////////////////////////////////////////
//
// connection.cpp
//
//////////////////////////////////////////////////////

/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@brief HTTP library implementation in C++
===============================================================================
MIT License

Copyright (c) 2017 Shanmuga (Anand) Gunasekaran

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
===============================================================================
LICENSE: END
*/

#include "http/http.hpp"
#include "common/convert.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/select.h>
#include <arpa/inet.h>

//OpenSSL includes
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

using namespace std;
using namespace sid;
using namespace sid::http;

extern const SSL_METHOD* g_clientMethod; 
extern const SSL_METHOD* g_serverMethod; 

#define IO_READ  1
#define IO_WRITE 2
#define IO_BOTH  3

#define __SSL_free(s) if ( s ) { SSL_free(s); s = nullptr; }
#define __SSL_CTX_free(s) if ( s ) { SSL_CTX_free(s); s = nullptr; }

struct io_exec_output
{
  int retVal;
  bool timedOut;
  bool ioNotReady;
  io_exec_output(int _r = 0) : retVal(_r), timedOut(false), ioNotReady(false) {}
};
using IOLoopCallback = std::function<int(bool&)> ;

/**
 * @class http_connection
 * @brief Class definition for a HTTP connection.
 *
 * This class cannot be instantiated directly. Use connection::create(connection_type::http) to create an instance of it.
 */
class http_connection : public sid::http::connection
{
  using super = sid::http::connection;
public:
  http_connection();
  virtual ~http_connection();
  ////////////////////////////////////////////////////////////////////////////
  // Virtual functions - These functions are overwritten from the base class
  const connection_type type() const override { return connection_type::http; }
  bool open(const std::string& _server, const unsigned short& _port = 0) override;
  bool open(int _sockfd) override;
  bool is_open() const override { return m_socket > 0; }
  bool close() override;
  ssize_t write(const void* _buffer, size_t _count) override;
  ssize_t read(void* _buffer, size_t _count) override;
  connection_description description() const override;
  ////////////////////////////////////////////////////////////////////////////

protected:
  bool do_set_non_blocking(int fd);
  io_exec_output io_exec(IOLoopCallback& fnIOCallback, int ioType, int default_retVal = 0);
  bool isReadyForIO(int ioType, bool* pOperationTimedOut = nullptr) const;
  
protected:
  int m_socket; //! Socket to the server
};

/**
 * @class https_connection
 * @brief Class definition for a HTTPS connection.
 *
 * This class cannot be instantiated directly. Use connection::create(connection_type::https) to create an instance of it.
 */
class https_connection : public http_connection
{
  using super = http_connection;
public:
  https_connection();
  virtual ~https_connection();
  ////////////////////////////////////////////////////////////////////////////
  // Virtual functions - These functions are overwritten from the base class
  const connection_type type() const override { return connection_type::https; }
  bool open(const std::string& _server, const unsigned short& _port = 0) override;
  bool open(int _sockfd) override;
  bool is_open() const override { return super::is_open(); }
  bool close() override;
  ssize_t write(const void* _buffer, size_t _count) override;
  ssize_t read(void* _buffer, size_t _count) override;
  connection_description description() const override;
  //! Accept - SSL-specific
  void accept() override;
  ////////////////////////////////////////////////////////////////////////////

private:
  void attach_ssl();

private:
  SSL_CTX* m_sslctx;
  SSL*     m_ssl;
};

//////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of abstract class connection
//
//////////////////////////////////////////////////////////////////////////////////////
//! Default constructor
connection::connection() : 
  m_server(""),
  m_error(""),
  m_port(0),
  m_retryable(false),
  m_ioTimeout(DEFAULT_IO_TIMEOUT_SECS)
{
}

//! Destructor
connection::~connection()
{
  // Nothing to destroy here. Cleanup done in derived class.
}

/**
 * @fn connection_ptr create(const connection_type& _type);
 * @brief Creates a connection object based on the connection type specified. In case of error it throws a sid::exception.
 *
 * @param type [in] Type of connection (HTTP or HTTPS)
 *
 * @return Smart pointer to the connection object. It is guaranteed not to return a null pointer.
 */
connection_ptr connection::create(const connection_type& _type, const connection_family& _family/* = connection_family::none*/)
{
  return connection::p_create(_type, ssl::certificate(), _family);
}

/**
 * @fn connection_ptr create(const ssl::certificate& _sslCert, const connection_family& _family);
 * @brief Creates a connection object based on the connection type specified. In case of error it throws a sid::exception.
 *
 * @param _sslCert [in] SSL Certificate to be used for HTTPS connection
 * @param _family [in] Connection family to use (ipv4 or ipv6 or any)
 *
 * @return Smart pointer to the connection object. It is guaranteed not to return a null pointer.
 */
connection_ptr connection::create(const ssl::certificate& _sslCert, const connection_family& _family/* = connection_family::none*/)
{
  return connection::p_create(connection_type::https, _sslCert, _family);
}

/**
 * @fn connection_ptr create(const ssl::certificate& _sslCert, const connection_family& _family);
 * @brief Creates a connection object based on the connection type specified. In case of error it throws a sid::exception.
 *
 * @param _type [in] Type of connection (HTTP or HTTPS)
 * @param _sslCert [in] SSL Certificate to be used for HTTPS connection
 * @param _family [in] Connection family to use (ipv4 or ipv6 or any)
 *
 * @return Smart pointer to the connection object. It is guaranteed not to return a null pointer.
 */
connection_ptr connection::p_create(const connection_type& _type, const ssl::certificate& _sslCert, const connection_family& _family)
{
  connection_ptr conn;
  http::connection* ptr = nullptr;

  try
  {
    // Create an object based on the connection type
    if ( _type == connection_type::http )
      ptr = new http_connection();
    else if ( _type == connection_type::https )
      ptr = new https_connection();
    else
      throw sid::exception("Cannot create connection object as an invalid connection type was specifed.");

    // Copy the created object to the smart pointer
    conn = ptr;
    conn->m_family = _family;
    conn->m_sslCert = _sslCert;
  }
  catch ( http::connection* p )
  {
    if ( p ) delete p;
    throw sid::exception(std::string("Unable to create smart pointer object for ") + ( (_type == connection_type::http)? "HTTP":"HTTPS") + std::string(" connection"));
  }
  catch ( const sid::exception& ) { /* Rethrow sid exception */ throw; }
  catch (...)
  {
    if ( ptr ) { delete ptr; ptr = nullptr; }
    throw sid::exception(std::string("An unhandled exception occurred while trying to create a ") + ( (_type == connection_type::http)? "HTTP":"HTTPS") + std::string(" connection"));
  }

  // If the connection object is empty, the object was not created successfully. So, throw an exception.
  if ( !conn )
    throw sid::exception(std::string("Unable to create ") + ( (_type == connection_type::http)? "HTTP":"HTTPS") + std::string(" connection object"));

  // Return the connection object. Guarantees that the object is NOT a null pointer
  return conn;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of http_connection class
//
//////////////////////////////////////////////////////////////////////////////////////
http_connection::http_connection() : super(), m_socket(-1)
{
}

http_connection::~http_connection()
{
  close();
}

bool http_connection::do_set_non_blocking(int fd)
{
  int flags = ::fcntl(fd, F_GETFL);
  if ( flags == -1 ) return false;
  flags |= O_NONBLOCK;
  return (::fcntl(fd, F_SETFL, flags) == 0);
}

io_exec_output http_connection::io_exec(IOLoopCallback& fnIOCallback, int ioType, int default_retVal/* = 0*/)
{
  io_exec_output out(default_retVal);

  for ( bool bContinue = true; bContinue; )
  {
    bContinue = false;
    out.timedOut = false;
    if ( ! isReadyForIO(ioType, &out.timedOut) )
    {
      // if operation timedout and if the socket was set for blocking, continue to loop again
      if ( out.timedOut && is_blocking() )
      {
	bContinue = true;
	continue;
      }
      out.retVal = 0;
      out.ioNotReady = true;
      break;
    }

    out.retVal = fnIOCallback(/*out*/bContinue);
  } // for loop

  if ( out.timedOut )
    throw sid::exception("The operation timedout after " + sid::to_str(this->get_timeout()) + " seconds");

  return out;
}

bool http_connection::isReadyForIO(int _ioType, bool* _pOperationTimedOut) const
{
  if ( _pOperationTimedOut ) *_pOperationTimedOut = false;

  bool isReady = false;

  auto check_using_poll = [&]()->void
    {
      struct timespec ts = {this->get_timeout(), 0};
      pollfd poll_fd = {m_socket, POLLRDHUP, 0};
      if ( _ioType & IO_READ )
	poll_fd.events |= (POLLIN | POLLPRI);
      if ( _ioType & IO_WRITE )
	poll_fd.events |= POLLOUT;
      int ret = ppoll(&poll_fd, 1, &ts, nullptr);
      if ( ret == -1 )
	throw sid::exception(http::errno_str(errno));

      if ( ret > 0 )
      {
	const int& revents = poll_fd.revents;
	if ( (revents & POLLERR) )
	  throw sid::exception("Error polling for data");
	if ( (revents & POLLHUP) || (revents & POLLRDHUP) )
	  throw sid::exception("Error polling for data as the peer closed the connection");
	if ( (revents & POLLNVAL) )
	  throw sid::exception("Error polling for data due to invalid request");
	if ( (_ioType & IO_READ) && (revents & POLLIN) )
	  isReady = true;
	else if ( (_ioType & IO_WRITE) && (revents & POLLOUT) )
	  isReady = true;
      }
    };

  auto check_using_select = [&]()->void
    {
      fd_set r_fds, w_fds, e_fds;
      FD_ZERO(&r_fds);
      FD_ZERO(&w_fds);
      FD_ZERO(&e_fds);

      fd_set* pr_fds = nullptr;
      fd_set* pw_fds = nullptr;

      if ( _ioType & IO_READ )
      {
	pr_fds = &r_fds;
	FD_SET(m_socket, pr_fds);
      }
      if ( _ioType & IO_WRITE )
      {
	pw_fds = &w_fds;
	FD_SET(m_socket, pw_fds);
      }
      FD_SET(m_socket, &e_fds);

      struct timeval tv = {0};
      tv.tv_sec = this->get_timeout();
      int ret = select(m_socket+1, pr_fds, pw_fds, &e_fds, &tv);
      if ( ret == -1 )
	throw sid::exception(http::errno_str(errno));

      if ( ret > 0 )
      {
	if ( pr_fds && FD_ISSET(m_socket, pr_fds ) )
	  isReady = true;
	else if ( pw_fds && FD_ISSET(m_socket, pw_fds ) )
	  isReady = true;
	// If an error occurred, return false
	else if ( FD_ISSET(m_socket, &e_fds ) )
	  throw sid::exception("Error polling for data in select()");
      }
    };

  // Check for availability of I/O
  check_using_poll();

  if ( !isReady )
  {
    // possibly a timeout error
    if ( _pOperationTimedOut ) *_pOperationTimedOut = true;
  }
  return isReady;
}

bool http_connection::close()
{
  if ( m_socket != -1 )
  {
    ::close(m_socket);
    m_socket = -1;
    m_server.clear();
    m_port = 0;
    return true;
  }
  return false;
}

bool http_connection::open(const std::string& _server, const unsigned short& _port)
{
  bool isSuccess = false;
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  std::string csPort;
  unsigned short httpPort = ( _port )? _port : DEFAULT_PORT_HTTP;

  try
  {
    m_error.clear();

    if ( this->is_open() )
      throw sid::exception("Connection is already established. Close the connection before opening it.");

    if ( _server.empty() )
      throw sid::exception("Server name cannot be empty");

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = (m_family == connection_family::ip_v4)? AF_INET :
                      (m_family == connection_family::ip_v6)? AF_INET6 : AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM; // Stream socket
    hints.ai_flags = 0;
    hints.ai_protocol = 0;           // Any protocol

    csPort = sid::to_str(httpPort);
    int s = getaddrinfo(_server.c_str(), csPort.c_str(), &hints, &result);
    if ( s != 0 )
      throw sid::exception(std::string("getaddrinfo() failed with gai_error(") + sid::to_str(s) + ") " + gai_strerror(s));

    /* getaddrinfo() returns a list of address structures.
       Try each address until we successfully connect(2).
       If socket(2) (or connect(2)) fails, we (close the socket
       and) try the next address. */

    char szName[256] = {0};
    bool found = false;
    int iErrNo = 0;
    for ( rp = result; rp && !found; rp = rp->ai_next )
    {
      int sfd = -1;

      switch ( rp->ai_addr->sa_family )
      {
      case AF_INET:
	{
	  sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
	  if ( sfd == -1 ) break;
	  struct sockaddr_in* saddr = (struct sockaddr_in*) rp->ai_addr;
	  // set the server and socket descriptor
	  memset(szName, 0, sizeof(szName));
	  inet_ntop(AF_INET, &(saddr->sin_addr), szName, INET_ADDRSTRLEN);
	  //std::string svr = inet_ntoa(saddr->sin_addr);
	  if ( connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1 )
	  {
	    // set the server and socket descriptor
	    m_server = szName;
	    m_socket = sfd;
	    m_family = connection_family::ip_v4;
	    found = true;
	  }
	  else
	    iErrNo = errno;

	  if ( !found )
	    ::close(sfd);
	}
	break;
      case AF_INET6:
	{
	  sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
	  if ( sfd == -1 ) break;
	  struct sockaddr_in6* saddr6 = (struct sockaddr_in6 *) rp->ai_addr;
	  // set the server and socket descriptor
	  memset(szName, 0, sizeof(szName));
	  inet_ntop(AF_INET6, &(saddr6->sin6_addr), szName, INET6_ADDRSTRLEN);
	  if ( connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1 )
	  {
	    // set the server and socket descriptor
	    m_server = szName;
	    m_socket = sfd;
	    m_family = connection_family::ip_v6;
	    found = true;
	  }
	  else
	    iErrNo = errno;

	  if ( !found )
	    ::close(sfd);
	}
	break;
      } // switch
    } // for
    freeaddrinfo(result);

    if ( ! found )
      throw sid::exception(std::string("Could not connect to server ") + _server + " at port " + csPort + " over " + szName + ". " + http::errno_str(iErrNo));

    // set the port member
    m_port = httpPort;

    // set the socket to non-blocking mode internally
    do_set_non_blocking(m_socket);

    // set the return status to true
    isSuccess = true;
  }
  catch ( const sid::exception& e )
  {
    m_error = __func__ + std::string(": ") + e.what();
  }
  catch (...)
  {
    m_error = __func__ + std::string(": Unhandled exception occurred");
  }

  return isSuccess;
}

bool http_connection::open(int _sockfd)
{
  bool isSuccess = false;

  try
  {
    m_error.clear();

    if ( this->is_open() )
      throw sid::exception("Connection is already established. Close the connection before opening it.");

    if ( _sockfd <= 0 )
      throw sid::exception("Invalid socket descriptor");

    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);
    char ipstr[INET6_ADDRSTRLEN+1];

    memset(&addr, 0, sizeof(addr));
    memset(ipstr, 0, sizeof(ipstr));

    if ( -1 == getpeername(_sockfd, (struct sockaddr*) &addr, &len) )
      throw sid::exception(http::errno_str(errno));

    switch ( addr.ss_family )
    {
    case AF_INET:
      {
	struct sockaddr_in* saddr = (struct sockaddr_in *) &addr;
	inet_ntop(AF_INET, &(saddr->sin_addr), ipstr, sizeof(ipstr));
	// set the server and port members
	m_socket = _sockfd;
	m_port = ntohs(saddr->sin_port);
	m_server = ipstr;
	m_family = connection_family::ip_v4;
      }
      break;
    case AF_INET6:
      {
	struct sockaddr_in6* saddr6 = (struct sockaddr_in6 *) &addr;
	inet_ntop(AF_INET6, &(saddr6->sin6_addr), ipstr, sizeof(ipstr));
	// set the server and port members
	m_socket = _sockfd;
	m_port = ntohs(saddr6->sin6_port);
	m_server = ipstr;
	m_family = connection_family::ip_v6;
      }
      break;
    default:
      throw sid::exception("Invalid address family (" + sid::to_str(addr.ss_family) + ") returned for getpeername()");
    }
    // set the socket to non-blocking mode internally
    do_set_non_blocking(m_socket);
    // set the return status to true
    isSuccess = true;
  }
  catch ( const sid::exception& e )
  {
    m_error = __func__ + std::string(": ") + e.what();
  }
  catch (...)
  {
    m_error = __func__ + std::string(": Unhandled exception occurred");
  }

  return isSuccess;
}

ssize_t http_connection::write(const void* _buffer, size_t _count)
{
  IOLoopCallback write_callback = [&](bool& bContinue)->int
    {
      int retVal = ::write(m_socket, _buffer, _count);
      if ( retVal < 0 )
      {
	if ( errno == EAGAIN || errno == EWOULDBLOCK )
	{
	  bContinue = true;
	  //cout << "write: EWOULDBLOCK returned. Continuing the loop" << endl;
	}
	else if ( errno != 0 )
	  throw sid::exception("Write failed with error: " + http::errno_str(errno));
      }
      return retVal;
    };

  io_exec_output out = io_exec(write_callback, IO_WRITE, 0);
  return out.retVal;
}

ssize_t http_connection::read(void* _buffer, size_t _count)
{
  IOLoopCallback read_callback = [&](bool& bContinue)->int
    {
      errno = 0;
      int retVal = ::read(m_socket, _buffer, _count);
      if ( retVal < 0 )
      {
	if ( errno == EAGAIN || errno == EWOULDBLOCK )
	{
	  bContinue = true;
	  //cout << "read: EWOULDBLOCK returned. Continuing the loop" << endl;
	}
	else if ( errno != 0 )
	  throw sid::exception("Read failed with error: " + http::errno_str(errno));
      }
      return retVal;
    };

  io_exec_output out = io_exec(read_callback, IO_READ, 0);
  return out.retVal;
}

std::string to_str(const connection_family& family)
{
  return (family == connection_family::ip_v4)? "ip_v4" : (family == connection_family::ip_v6? "ip_v6" : "ip_any");
}

connection_description http_connection::description() const
{
  connection_description desc;

  desc.isConnected = ( m_socket > 0 );
  desc.server = m_server;
  desc.port = m_port;
  desc.type = this->type();
  desc.family = m_family;
  desc.isBlocking = this->is_blocking();
  desc.nonBlockingTimeout = this->get_timeout();

  return desc;
}
//////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of https_connection class
//
//////////////////////////////////////////////////////////////////////////////////////
https_connection::https_connection() : super()
{
  http::library_init();
  m_sslctx = nullptr;
  m_ssl = nullptr;
}

https_connection::~https_connection()
{
  close();
}

bool https_connection::close()
{
  __SSL_free(m_ssl);
  __SSL_CTX_free(m_sslctx);
  super::close();
  return true;
}

static std::string s_ssl_error_string()
{
  char szError[1024] = {0};
  unsigned long e = ERR_get_error();
  ERR_error_string_n(e, szError, sizeof(szError)-1);
  return std::string(szError);
}

void https_connection::attach_ssl()
{
  try
  {
    // Ensure m_ssl and m_sslctx are cleared before starting
    __SSL_free(m_ssl);
    __SSL_CTX_free(m_sslctx);

    m_sslctx = SSL_CTX_new ( (SSL_METHOD *) g_clientMethod );
    if ( !m_sslctx )
      throw sid::exception("Unable to create new SSL context");

    // Clear the error queue
    ERR_clear_error();

    int ret = 0;
    // Set the certificate if provided
    const ssl::certificate& cert = this->certificate();;
    switch ( cert.type )
    {
    case ssl::certificate_type::none:
      break;
    case ssl::certificate_type::client:
      if ( cert.client.chainFile.empty() && cert.client.privateKeyFile.empty() )
	throw sid::exception("Client certificate error: Chain file and private key file are both empty");

      ret = SSL_CTX_use_certificate_chain_file(
              m_sslctx,
	      cert.client.chainFile.empty()? nullptr : cert.client.chainFile.c_str()
            );
      if ( ret != 1 )
	throw sid::exception("SSL Ceritificate chain file error: " + s_ssl_error_string());

      /*
      ret = SSL_CTX_use_certificate_file(
              m_sslctx,
	      cert.client.certFile.empty()? nullptr : cert.client.certFile.c_str(),
	      cert.client.privateKeyType
            );
      if ( ret != 1 )
	throw sid::exception("SSL Ceritificate file error: " + s_ssl_error_string());
      */

      ret = SSL_CTX_use_PrivateKey_file(
	      m_sslctx,
	      cert.client.privateKeyFile.empty()? nullptr : cert.client.privateKeyFile.c_str(),
	      cert.client.privateKeyType
            );
      if ( ret != 1 )
	throw sid::exception("SSL Private key file error: " + s_ssl_error_string());

      break;
    case ssl::certificate_type::server:
      if ( cert.server.caFile.empty() && cert.server.caPath.empty() )
	throw sid::exception("Server certificate error: CA file and directory are both empty");

      ret = SSL_CTX_load_verify_locations(
	      m_sslctx,
	      cert.server.caFile.empty()? nullptr : cert.server.caFile.c_str(),
	      cert.server.caPath.empty()? nullptr : cert.server.caPath.c_str()
            );
      if ( ret != 1 )
	throw sid::exception("SSL server certificate error: " + s_ssl_error_string());

      SSL_CTX_set_verify(m_sslctx, SSL_VERIFY_PEER, NULL);
      break;
    }

    m_ssl = SSL_new(m_sslctx);
    if ( !m_ssl )
      throw sid::exception("Unable to create new SSL object");

    if ( 0 == SSL_set_fd(m_ssl, m_socket) )
      throw sid::exception("Unable to set socket on SSL");

    IOLoopCallback ssl_connect_callback = [&](bool& bContinue)->int
      {
	int retVal = SSL_connect(m_ssl);
	if ( retVal == 1 )
	  ;
	else if ( retVal < 0 )
	{
	  int sslErr = SSL_get_error(m_ssl, retVal);
	  if ( sslErr == SSL_ERROR_WANT_READ || sslErr == SSL_ERROR_WANT_WRITE )
	    bContinue = true;
	  else if ( sslErr != 0 )
	    throw sid::exception("SSL handshake was unsuccessful. retVal=" + sid::to_str(retVal) + ", sslErr=" + sid::to_str(sslErr) + ", errno=" + http::errno_str(errno));
	}
	else
	  throw sid::exception("SSL handshake was unsuccessful");
	return retVal;
      };

    io_exec(ssl_connect_callback, IO_READ|IO_WRITE);
  }
  catch (...)
  {
    // clear the context
    __SSL_free(m_ssl);
    __SSL_CTX_free(m_sslctx);
    // rethrow the exception
    throw;
  }
}

bool https_connection::open(const std::string& _server, const unsigned short& _port)
{
  bool isSuccess = false;
  unsigned short httpsPort = ( _port )? _port : DEFAULT_PORT_HTTPS;

  try
  {
    if ( ! super::open(_server, httpsPort) )
      return false;

    attach_ssl();

    // set the return status to true
    isSuccess = true;
  }
  catch ( const sid::exception& e )
  {
    close();
    m_error = __func__ + std::string(": ") + e.what();
  }
  catch (...)
  {
    close();
    m_error = __func__ + std::string(": Unhandled exception occurred");
  }

  return isSuccess;
}

bool https_connection::open(int _sockfd)
{
  bool isSuccess = false;

  try
  {
    if ( ! super::open(_sockfd) )
      return false;

    attach_ssl();

    // set the return status to true
    isSuccess = true;
  }
  catch ( const sid::exception& e )
  {
    m_error = __func__ + std::string(": ") + e.what();
  }
  catch (...)
  {
    m_error = __func__ + std::string(": Unhandled exception occurred");
  }

  return isSuccess;
}


ssize_t https_connection::write(const void* _buffer, size_t _count)
{
  IOLoopCallback ssl_write_callback = [&](bool& bContinue)->int
    {
      int retVal = SSL_write(m_ssl, _buffer, _count);
      if ( retVal <= 0 )
      {
	int sslErr = SSL_get_error(m_ssl, retVal);
	if ( sslErr == SSL_ERROR_WANT_READ || sslErr == SSL_ERROR_WANT_WRITE )
	{
	  bContinue = true;
	  //cout << "SSL_write: SSL_ERROR_WANT_WRITE returned. Continuing the loop" << endl;
	}
	else if ( sslErr != 0 )
	  throw sid::exception("Write failed. retVal=" + sid::to_str(retVal) + ", sslErr=" + sid::to_str(sslErr));
      }
      return retVal;
    };

  io_exec_output out = io_exec(ssl_write_callback, IO_WRITE, 0);
  return out.retVal;
}

ssize_t https_connection::read(void* _buffer, size_t _count)
{
  IOLoopCallback ssl_read_callback = [&](bool& bContinue)->int
    {
      int retVal = SSL_read(m_ssl, _buffer, _count);
      if ( retVal <= 0 )
      {
	int sslErr = SSL_get_error(m_ssl, retVal);
	if ( sslErr == SSL_ERROR_WANT_READ || sslErr == SSL_ERROR_WANT_WRITE )
	{
	  bContinue = true;
	  //cout << "SSL_read: SSL_ERROR_WANT_READ returned. Continuing the loop" << endl;
	}
	else if ( sslErr != 0 )
	  throw sid::exception("Read failed. retVal=" + sid::to_str(retVal) + ", sslErr=" + sid::to_str(sslErr));
      }
      return retVal;
    };

  io_exec_output out = io_exec(ssl_read_callback, IO_READ, 0);
  return out.retVal;
}

connection_description https_connection::description() const
{
  connection_description desc = super::description();

  // More info from https://www.openssl.org/docs/man1.1.0/man3/SSL_CIPHER_get_name.html
  if ( m_ssl )
  {
    char szDesc[256] = {0};
    SSL_CIPHER_description(SSL_get_current_cipher(m_ssl), szDesc, sizeof(szDesc)-1);
    desc.ssl.isAvailable = true;
    desc.ssl.info = szDesc;
  }

  return desc;
}

void https_connection::accept()
{
  try
  {
    IOLoopCallback ssl_accept_callback = [&](bool& bContinue)->int
      {
	int retVal = SSL_accept(m_ssl);
	if ( retVal == 1 )
	  ;
	else if ( retVal < 0 )
	{
	  int sslErr = SSL_get_error(m_ssl, retVal);
	  if ( sslErr == SSL_ERROR_WANT_READ || sslErr == SSL_ERROR_WANT_WRITE )
	  {
	    //cout << __func__ << ": retVal=" << retVal << ", sslErr=" << sslErr << endl;
	    bContinue = true;
	  }
	  else
	    throw sid::exception("SSL accept was unsuccessful. retVal=" + sid::to_str(retVal) + ", sslErr=" + sid::to_str(sslErr));
	}
	else
	  throw sid::exception("SSL accept was unsuccessful");
	return retVal;
      };

    io_exec(ssl_accept_callback, IO_READ|IO_WRITE);
  }
  catch ( const sid::exception& e ) { cout << "Accept Error: " << e.what() << endl; /* Rethrow sid exception */ throw; }
  catch (...)
  {
    throw sid::exception(std::string("An unhandled exception occurred while trying to accept the SSL connection "));
  }
}

//////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of connection_description::ssl_info structure
//
//////////////////////////////////////////////////////////////////////////////////////
//! Default constructor
connection_description::ssl_info::ssl_info()
{
  clear();
}

connection_description::ssl_info::~ssl_info()
{
}

void connection_description::ssl_info::clear()
{
  isAvailable = false;
  info.clear();
}

std::string connection_description::ssl_info::to_str() const
{
  std::ostringstream desc;

  if ( !isAvailable )
    desc << "Not available";
  else
    desc << info;

  return desc.str();
}

//////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of connection_description structure
//
//////////////////////////////////////////////////////////////////////////////////////
//! Default constructor
connection_description::connection_description()
{
  clear();
}

connection_description::~connection_description()
{
}

void connection_description::clear()
{
  isConnected = false;
  server.clear();
  port = 0;
  type = connection_type::http;
  family = connection_family::none;
  isBlocking = false;
  nonBlockingTimeout = 0;
  ssl.clear();
}

std::string connection_description::to_str() const
{
  std::ostringstream desc;

  desc << (type == connection_type::http? "http" : "https");
  if ( isConnected )
  {
    desc << (isBlocking? " blocking":" non-blocking");
    if ( !isBlocking )
      desc << " (timeout " << nonBlockingTimeout << " secs)";
    desc << " connected to " << server << " on port " << port
	 << ", family " << ::to_str(family);
  }
  else
    desc << " not connected, family " << ::to_str(family);

  if ( type == connection_type::https )
    desc << " SSL Info: " << ssl.to_str();

  return desc.str();
}
