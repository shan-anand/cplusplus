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

#define IO_READ  1
#define IO_WRITE 2
#define IO_BOTH  3

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
  bool open(const std::string& server, const unsigned short& port = 0) override;
  bool open(int sockfd) override;
  bool is_open() const override { return m_socket > 0; }
  bool close() override;
  ssize_t write(const void* buffer, size_t count) override;
  ssize_t read(void* buffer, size_t count) override;
  bool set_non_blocking(bool bEnable) final;
  bool is_non_blocking() const final;
  bool is_blocking() const final;
  ////////////////////////////////////////////////////////////////////////////

protected:
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
  bool open(const std::string& server, const unsigned short& port = 0) override;
  bool open(int sockfd) override;
  bool is_open() const override { return http_connection::is_open(); }
  bool close() override;
  ssize_t write(const void* buffer, size_t count) override;
  ssize_t read(void* buffer, size_t count) override;
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
  m_nonBlockingTimeout(DEFAULT_NONBLOCKING_TIMEOUT)
{
}

//! Destructor
connection::~connection()
{
  // Nothing to destroy here. Cleanup done in derived class.
}

/**
 * @fn connection_ptr create(const connection_type& _type) throw (std::string);
 * @brief Creates a connection object based on the connection type specified. In case of error it throws a string exception.
 *
 * @param type [in] Type of connection (HTTP or HTTPS)
 *
 * @return Smart pointer to the connection object. It is guaranteed not to return a null pointer.
 */
connection_ptr connection::create(const connection_type& _type) throw (sid::exception)
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
  }
  catch ( http::connection* p )
  {
    if ( p ) delete p;
    throw sid::exception(std::string("Unable to create smart pointer object for ") + ( (_type == connection_type::http)? "HTTP":"HTTPS") + std::string(" connection"));
  }
  catch ( const sid::exception& ) { /* Rethrow string exception */ throw; }
  catch (...)
  {
    if ( ptr ) { delete ptr; ptr = nullptr; }
    throw sid::exception(std::string("An unhandled exception occurred while trying to create a ") + ( (_type == connection_type::http)? "HTTP":"HTTPS") + std::string(" connection"));
  }

  // If the connection object is empty, the object was not created successfully. So, throw an exception.
  if ( conn.empty() )
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

bool http_connection::open(const std::string& server, const unsigned short& port)
{
  bool bStatus = false;
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  std::string csPort;
  unsigned short httpPort = ( port )? port : DEFAULT_PORT_HTTP;

  try
  {
    m_error.clear();

    if ( this->is_open() )
      throw sid::exception("Connection is already established. Close the connection before opening it.");

    if ( server.empty() )
      throw sid::exception("Server name cannot be empty");

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;     // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // Stream socket
    hints.ai_flags = 0;
    hints.ai_protocol = 0;           // Any protocol

    csPort = sid::to_str(httpPort);
    int s = getaddrinfo(server.c_str(), csPort.c_str(), &hints, &result);
    if ( s != 0 )
      throw sid::exception(std::string("getaddrinfo: ") + gai_strerror(s));

    /* getaddrinfo() returns a list of address structures.
       Try each address until we successfully connect(2).
       If socket(2) (or connect(2)) fails, we (close the socket
       and) try the next address. */

    for ( rp = result; rp; rp = rp->ai_next )
    {
      int sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
      if (sfd == -1) continue;

      if ( connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1 )
      {
        struct sockaddr_in* saddr = (struct sockaddr_in*) rp->ai_addr;
        // set the server and socket descriptor
        m_server = inet_ntoa(saddr->sin_addr);
        m_socket = sfd;
        break;
      }
      ::close(sfd);
    }
    freeaddrinfo(result);
    if ( ! rp )
      throw sid::exception(std::string("Could not connect to server ") + server + " at port " + csPort);

    // set the port member
    m_port = httpPort;

    // set the return status to true
    bStatus = true;
  }
  catch ( const sid::exception& e )
  {
    m_error = __func__ + std::string(": ") + e.what();
  }
  catch (...)
  {
    m_error = __func__ + std::string(": Unhandled exception occurred");
  }

  return bStatus;
}

bool http_connection::open(int sockfd)
{
  bool bStatus = false;

  try
  {
    m_error.clear();

    if ( this->is_open() )
      throw sid::exception("Connection is already established. Close the connection before opening it.");

    if ( sockfd <= 0 )
      throw sid::exception("Invalid socket descriptor");

    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);
    char ipstr[INET6_ADDRSTRLEN+1];

    memset(&addr, 0, sizeof(addr));
    memset(ipstr, 0, sizeof(ipstr));

    if ( -1 == getpeername(sockfd, (struct sockaddr*) &addr, &len) )
      throw http::errno_str(errno);

    if ( addr.ss_family == AF_INET)
    {
      struct sockaddr_in *s = (struct sockaddr_in *) &addr;
      inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof(ipstr));

      // set the server and port members
      m_socket = sockfd;
      m_port = ntohs(s->sin_port);
      m_server = ipstr;
    }

    // set the return status to true
    bStatus = true;
  }
  catch ( const std::string& csErr )
  {
    m_error = __func__ + std::string(": ") + csErr;
  }
  catch (...)
  {
    m_error = __func__ + std::string(": Unhandled exception occurred");
  }

  return bStatus;
}

ssize_t http_connection::write(const void* buffer, size_t count)
{
  bool operationTimedOut = false;
  if ( ! isReadyForIO(IO_WRITE, &operationTimedOut) )
    return 0;

  bool isActualNonBlocking = is_non_blocking();
  if ( isActualNonBlocking )
    set_non_blocking(false);
  ssize_t written = ::write(m_socket, buffer, count);
  if ( isActualNonBlocking )
    set_non_blocking(true);
  return written;
}

ssize_t http_connection::read(void* buffer, size_t count)
{
  bool operationTimedOut = false;
  if ( ! isReadyForIO(IO_READ, &operationTimedOut) )
    return 0;

  bool isActualNonBlocking = is_non_blocking();
  if ( isActualNonBlocking )
    set_non_blocking(false);
  ssize_t read = ::read(m_socket, buffer, count);
  if ( isActualNonBlocking )
    set_non_blocking(true);
  return read;
}

bool http_connection::is_non_blocking() const
{
  int flags = ::fcntl(m_socket, F_GETFL);
  if ( flags == -1 ) return false;
  return ( flags & O_NONBLOCK );
}

inline bool http_connection::is_blocking() const
{
  return !is_non_blocking();
}

bool http_connection::set_non_blocking(bool bEnable)
{
  int flags = ::fcntl(m_socket, F_GETFL);
  if ( flags == -1 ) return false;
  
  if ( bEnable )
    flags |= O_NONBLOCK;
  else
    flags &= ~O_NONBLOCK;

  return (::fcntl(m_socket, F_SETFL, flags) == 0);
}

bool http_connection::isReadyForIO(int ioType, bool* pOperationTimedOut) const
{
  if ( pOperationTimedOut ) *pOperationTimedOut = false;

  if ( is_blocking() )
    return true;

  fd_set r_fds, w_fds, e_fds;
  FD_ZERO(&r_fds);
  FD_ZERO(&w_fds);
  FD_ZERO(&e_fds);

  fd_set* pr_fds = NULL;
  fd_set* pw_fds = NULL;

  if ( ioType & IO_READ )
  {
    pr_fds = &r_fds;
    FD_SET(m_socket, pr_fds);
  }
  if ( ioType & IO_WRITE )
  {
    pw_fds = &w_fds;
    FD_SET(m_socket, pw_fds);
  }
  FD_SET(m_socket, &e_fds);

  struct timeval tv = {0};
  tv.tv_sec = m_nonBlockingTimeout;
  int ret = select(m_socket+1, pr_fds, pw_fds, &e_fds, &tv);
  if ( ret == -1 )
    throw http::errno_str(errno);

  if ( ret > 0 )
  {
    if ( pr_fds && FD_ISSET(m_socket, pr_fds ) )
      return true;
    if ( pw_fds && FD_ISSET(m_socket, pw_fds ) )
      return true;
    // If an error occurred, return false
    if ( FD_ISSET(m_socket, &e_fds ) )
      return false;
  }

  // possibly a timeout error
  if ( pOperationTimedOut ) *pOperationTimedOut = true;
  return false;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of https_connection class
//
//////////////////////////////////////////////////////////////////////////////////////
static const SSL_METHOD* g_clientMethod; 
static const SSL_METHOD* g_serverMethod; 
static bool g_bInitSuccessful = false;

bool http_library_init()
{
  static bool firstTime = true;
  if ( !firstTime ) return g_bInitSuccessful;

  SSL_library_init();

  SSL_load_error_strings();
  ERR_load_BIO_strings();
  SSLeay_add_ssl_algorithms();
    
  // Set up OpenSSL to enable all algorithms, ciphers and digests
  OpenSSL_add_all_algorithms();
  OpenSSL_add_all_ciphers();
  OpenSSL_add_all_digests();

  g_serverMethod = SSLv23_server_method();
  if ( ! g_serverMethod ) 
  {
    cerr << "No serverMethod" << endl; // XXX
    return g_bInitSuccessful;
  }

  g_clientMethod = SSLv23_client_method();
  if ( ! g_clientMethod  ) 
  {
    cerr << "No clientMethod" << endl; // XXX
    return g_bInitSuccessful;
  }

  //SetupSSLCallbacks(); // for multithreading 

  g_bInitSuccessful = true;
  return g_bInitSuccessful;
}

void http_library_cleanup()
{
  if ( !g_bInitSuccessful ) return;

#ifdef RSA_SSL_C
  SSL_library_cleanup();
#endif
}

https_connection::https_connection() : super()
{
  http_library_init();
  m_sslctx = nullptr;
  m_ssl = nullptr;
}

https_connection::~https_connection()
{
  close();
}

bool https_connection::close()
{
  if ( m_ssl )
  {
    SSL_free(m_ssl);
    m_ssl = nullptr;
  }
  if ( m_sslctx )
  {
    SSL_CTX_free(m_sslctx);
    m_sslctx = nullptr;
  }
  http_connection::close();
  return true;
}

void https_connection::attach_ssl()
{
  m_sslctx = SSL_CTX_new ( (SSL_METHOD *) g_clientMethod );
  if ( !m_sslctx )
    throw sid::exception("Unable to create new SSL context");

  m_ssl = SSL_new(m_sslctx);
  if ( !m_ssl )
    throw sid::exception("Unable to create new SSL object");

  if ( 0 == SSL_set_fd(m_ssl, m_socket) )
    throw sid::exception("Unable to set socket on SSL");

  if ( 1 != SSL_connect(m_ssl) )
    throw sid::exception("SSL handshake was unsuccessful");
}

bool https_connection::open(const std::string& server, const unsigned short& port)
{
  bool bStatus = false;
  unsigned short httpsPort = ( port )? port : DEFAULT_PORT_HTTPS;

  try
  {
    if ( ! http_connection::open(server, httpsPort) )
      return false;

    attach_ssl();

    // set the return status to true
    bStatus = true;
  }
  catch (const std::string& csErr)
  {
    close();
    m_error = __func__ + std::string(": ") + csErr;
  }
  catch (...)
  {
    close();
    m_error = __func__ + std::string(": Unhandled exception occurred");
  }

  return bStatus;
}

bool https_connection::open(int sockfd)
{
  bool bStatus = false;

  try
  {
    if ( ! http_connection::open(sockfd) )
      return false;

    attach_ssl();

    // set the return status to true
    bStatus = true;
  }
  catch (const std::string& csErr)
  {
    m_error = __func__ + std::string(": ") + csErr;
  }
  catch (...)
  {
    m_error = __func__ + std::string(": Unhandled exception occurred");
  }

  return bStatus;
}

ssize_t https_connection::write(const void* buffer, size_t count)
{
  bool operationTimedOut = false;
  if ( ! isReadyForIO(IO_WRITE, &operationTimedOut) )
    return 0;

  bool isActualNonBlocking = is_non_blocking();
  if ( isActualNonBlocking )
    set_non_blocking(false);
  ssize_t written = SSL_write(m_ssl, buffer, count);
  if ( isActualNonBlocking )
    set_non_blocking(true);
  return written;
/*
  if ( is_blocking() )
    return SSL_write(m_ssl, buffer, count);

  const size_t defaultWriteSize = 32*1024;
  size_t bytesToWrite;
  ssize_t written = 0, wrt = 0;
  const unsigned char* buf = (const unsigned char*) buffer;
  while ( count != (size_t) written )
  {
    bool operationTimedOut = false;
    if ( ! isReadyForIO(IO_WRITE, &operationTimedOut) )
      return 0;

    size_t remainingBytes = count - written;
    if ( remainingBytes <= defaultWriteSize )
      bytesToWrite = remainingBytes;
    else
      bytesToWrite = defaultWriteSize;

    wrt = SSL_write(m_ssl, buf, bytesToWrite);
    if ( wrt == -1 )
    {
      if ( errno == EAGAIN || errno == EWOULDBLOCK )
        continue;
      written = wrt;
      break;
    }
    written += wrt;
    buf += wrt;
    cout << written << endl;
  }
  return written;
*/
}

ssize_t https_connection::read(void* buffer, size_t count)
{
  bool operationTimedOut = false;
  if ( ! isReadyForIO(IO_READ, &operationTimedOut) )
    return 0;

  bool isActualNonBlocking = is_non_blocking();
  if ( isActualNonBlocking )
    set_non_blocking(false);
  ssize_t read = SSL_read(m_ssl, buffer, count);
  if ( isActualNonBlocking )
    set_non_blocking(true);
  return read;
}
