//////////////////////////////////////////////////////
//
// server.cpp
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
#include <strings.h>

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

#include <openssl/ssl.h>

using namespace std;
using namespace sid;
using namespace sid::http;

extern const SSL_METHOD* g_clientMethod; 
extern const SSL_METHOD* g_serverMethod; 

/*
To create a self-signed certificate with no password

sudo openssl req -nodes -x509 -days 3650 -sha256 -newkey rsa:4096 -subj "/C=IN/ST=KA/L=Bengaluru/O=self/OU=DevOps/CN=self/emailAddress=self@none" -keyout sid_self.pem -out sid_self.cert

*/

//! Default constructor
server::server() :
  m_type(),
  m_family(),
  m_sslClientCert(),
  m_port(0),
  m_socket(-1),
  m_isRunning(false),
  m_exitLoop(false),
  m_exception()
{
  http::library_init();
}

server_ptr server::create(const connection_type& _type, const connection_family& _family/* = connection_family::none*/)
{
  return server::p_create(_type, ssl::client_certificate(), _family);
}

server_ptr server::create(const ssl::client_certificate& _sslClientCert, const connection_family& _family/* = connection_family::none*/)
{
  return server::p_create(connection_type::https, _sslClientCert, _family);
}

server_ptr server::p_create(const connection_type& _type, const ssl::client_certificate& _sslClientCert, const connection_family& _family)
{
  server_ptr server;

  try
  {
    // Create an object based on the connection type
    server = new http::server();
    // Fill the object
    server->m_type          = _type;
    server->m_family        = _family;
    server->m_sslClientCert = _sslClientCert;
  }
  catch ( http::server* p )
  {
    if ( p ) delete p;
    throw sid::exception("Unable to create server smart pointer object");
  }
  catch ( const sid::exception& ) { /* Rethrow sid exception */ throw; }
  catch (...)
  {
    throw sid::exception("An unhandled exception occurred while trying to create a server object");
  }

  // If the server object is empty, the object was not created successfully. So, throw an exception.
  if ( !server )
    throw sid::exception("Unable to create server object");

  // Return the server object. Guarantees that the object is NOT a null pointer
  return server;
}

void server::stop()
{
  m_exitLoop = false;
}

bool server::run(uint16_t _port, FNProcessCallback& _fnProcessCallback, FNExitCallback& _fnExitCallback)
{
  bool bStatus = false;
  struct sockaddr_in6 serv_addr6, cli_addr6;

  try
  {
    m_port = (_port != 0)? _port : (m_type == http::connection_type::http)? DEFAULT_PORT_HTTP : DEFAULT_PORT_HTTPS;
    m_socket = ::socket(AF_INET6, SOCK_STREAM, 0);
    if ( m_socket < 0 )
      throw sid::exception("Error creating server socket: " + http::errno_str(errno));

    int reuse_port = 1;
    ::setsockopt(m_socket, SOL_SOCKET, SO_REUSEPORT, &reuse_port, sizeof(reuse_port));

    // Initialize server socket address
    bzero((char *) &serv_addr6, sizeof(serv_addr6));
    serv_addr6.sin6_family = AF_INET6;
    serv_addr6.sin6_addr = in6addr_any;
    serv_addr6.sin6_port = htons(m_port);

    // bind the host address, port to the socket
    if ( ::bind(m_socket, (struct sockaddr *) &serv_addr6, sizeof(serv_addr6)) < 0 )
      throw sid::exception("Error binding server socket: " + http::errno_str(errno));

    // Set server socket to non-blocking
    int flags = ::fcntl(m_socket, F_GETFD);
    flags |= O_NONBLOCK;
    if ( -1 == ::fcntl(m_socket, F_SETFD, flags) )
      throw sid::exception("Unable to set socket to non-blocking");

    if ( ::listen(m_socket, SOMAXCONN) < 0 )
      throw sid::exception("Error listening for connections: " + http::errno_str(errno));

    m_exitLoop = false;
    m_isRunning = true;

    // Certificate to use for https
    ssl::certificate sslCert;
    sslCert.type = ssl::certificate_type::client;
    sslCert.client = m_sslClientCert;

    pollfd fds = { m_socket, POLLIN, 0 };
    socklen_t clientLen = sizeof(cli_addr6);
    int client_fd = -1;
    for ( m_exitLoop = false; ! (m_exitLoop = _fnExitCallback()); )
    {
      int pollRes = ::poll(&fds, 1, 10);
      if ( pollRes == 0 ) continue;

      if ( pollRes == -1 )
        throw sid::exception("Polling failed: " + http::errno_str(errno));

      client_fd = ::accept(m_socket, (struct sockaddr *)&cli_addr6, &clientLen);
      if ( client_fd < 0 )
        throw sid::exception("Error accepting socket: " + http::errno_str(errno));
      //cout << "Request received" << endl;

      try
      {
	//sslCert.client.privateKeyType = 0;
	http::connection_ptr client;
	if ( m_type == http::connection_type::http )
	  client = http::connection::create(m_type);
	else
	  client = http::connection::create(sslCert);

	client->open(client_fd);
	client_fd = -1;
	// This is SSL-specific
	client->accept();
	// Call the client callback function to process the request
	_fnProcessCallback(client);
      }
      catch (...)
      {
	if ( client_fd > 0 )
	{
	  //::shutdown(client_fd, SHUT_RDWR);
	  ::close(client_fd);
	}
        client_fd = -1;
      }
    } // loop
    cout << "Exiting server loop" << endl;
    bStatus = true;
  }
  catch (const sid::exception& e)
  {
    m_exception = e;
  }
  catch (...)
  {
    m_exception = sid::exception("server::run: An Unhandled exception occurred");
  }

  // Close the server socket
  if ( m_socket != -1 )
  {
    ::shutdown(m_socket, SHUT_RDWR);
    ::close(m_socket);
    m_socket = -1;
  }
  
  // Indicate that we stopped running
  m_isRunning = false;

  return bStatus;
}

/*
http::server_ptr http_server = http::server::create(http::connection_type::http);
http_server->run(5080, continue_callback, client_callback);
http_server.clear();

http::server_ptr https_server = http::server::create(http::connection_type::https);
https_server->run(5443, continue_callback, client_callback);
https_server.clear();

FNClientCallback client_callback = [](http::connection_ptr client)->void
  {
    // start a new thread???
    http::request request;
    request.recv(conn);

    http::response response;
    response.send(conn);
  };

FNContinueCallback continue_callback = [&]()->bool
  {
    return true;
  };
*/
