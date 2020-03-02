#include "http_server.hpp"
#include <string>
#include <string.h> // for strerror
#include <iostream>
#include <sstream>
#include <errno.h>
#include <stdlib.h>
#include <http/http.hpp>
#include <stdio.h>
#include "common/uuid.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

using namespace std;
using namespace sid;

bool gbExitLoop = false;
static void* client_thread(void* arg);

struct ClientData
{
  int socket;
};

bool handle_map::add(const std::string& handle)
{
  m_map.insert(std::pair<std::string, time_t>(handle, time(NULL)));
  return true;
}

bool handle_map::remove(const std::string& handle)
{
  return (m_map.erase(handle) > 0);
}

bool handle_map::exists(const std::string& handle, bool* pbTimedOut)
{
  bool bExists = true;
  if ( pbTimedOut ) *pbTimedOut = false;

  std::map<std::string, time_t>::iterator it = m_map.find(handle);
  if ( it != m_map.end() )
  {
    time_t currentTime = time(NULL);
    if ( difftime(currentTime, it->second) < 10*60 )
    {
      bExists = true;
      it->second = currentTime;
    }
    else if ( pbTimedOut )
      *pbTimedOut = true;
  }
  return bExists;
}

http_server& http_server::get()
{
  static http_server server;
  return server;
}

//! Returns the error string associated with the given error number.
std::string http_server::errStr(const int iError)
{
  char buff[1024] = {0};

  // call the library function with NULL parameter to get the length of the error string.
  strerror_r(iError, buff, sizeof(buff)-1);
  return std::string(buff);
}

http_server::http_server()
{
  m_socket = -1;
  m_port = 5080;
  totalProcessed = 0;
}

void http_server::stop()
{
  gbExitLoop = true;
  //shutdown(m_socket, SHUT_RDWR);
  //::close(m_socket);
  //m_socket = -1;
}

void http_server::run(unsigned short port)
{
  struct sockaddr_in6 serv_addr6, cli_addr6;
  int clientSocket = -1;
  ClientData* client = NULL;

  try
  {
    m_port = port;
    m_socket = socket(AF_INET6, SOCK_STREAM, 0);
    if ( m_socket < 0 )
      throw std::string("Error creating socket: ")+errStr(errno);

    // Initialize server socket address
    bzero((char *) &serv_addr6, sizeof(serv_addr6));
    serv_addr6.sin6_family = AF_INET6;
    serv_addr6.sin6_addr = in6addr_any;
    serv_addr6.sin6_port = htons(m_port);

    // bind the host address, port to the socket
    if ( bind(m_socket, (struct sockaddr *) &serv_addr6, sizeof(serv_addr6)) < 0 )
      throw std::string("Error binding socket to host: ")+errStr(errno);

    int flags = fcntl(m_socket, F_GETFD);
    flags |= O_NONBLOCK;
    if ( -1 == fcntl(m_socket, F_SETFD, flags) )
      throw std::string("Unable to set socket to non-blocking");

    if ( listen(m_socket, SOMAXCONN) < 0 )
      throw std::string("Error listening for connections: ")+errStr(errno);

    pollfd fds = { m_socket, POLLIN, 0 };
    socklen_t clientLen = sizeof(cli_addr6);
    while ( !gbExitLoop )
    {
      int pollRes = poll(&fds, 1, 10);
      if ( pollRes == 0 ) continue;

      if ( pollRes == -1 )
        throw std::string("Polling failed: ")+errStr(errno);

      clientSocket = accept(m_socket, (struct sockaddr *)&cli_addr6, &clientLen);
      if ( clientSocket < 0 )
        throw std::string("Error accepting socket: ")+errStr(errno);
      cout << "Request received" << endl;

      try
      {
        // start a new thread to process this request
        pthread_t clientThreadId = 0;
        client = new ClientData();
        if ( ! client )
          throw std::string("Sorry, unable to handle the request");
        client->socket = clientSocket;
        int status = pthread_create(&clientThreadId, NULL, client_thread, (void*) client);
        if ( status ) throw std::string("Failed to create input thread");
        client = NULL;
        this->clientSet.add(clientThreadId);
        totalProcessed++;
        pthread_detach(clientThreadId);
        cout << "Request delegated to thread " << clientThreadId << endl;
      }
      catch (...)
      {
        ::close(clientSocket);
        clientSocket = -1;
        if ( client ) { delete client; client = NULL; }
      }
      cout << "About to loop: " << sid::to_str(gbExitLoop) << endl;
    } // loop
    useconds_t remaining = 5 * 1000 * 1000; // total waiting time of 5 seconds
    useconds_t waitTime = 5000; // 5 milli-second sleep everytime
    // Even though we exited server loop we need to wait until all client requests are sent
    for ( ; ! this->clientSet.empty() && remaining >= waitTime; remaining -= waitTime )
      usleep(waitTime);
    cout << "Exiting server loop" << endl;
    shutdown(m_socket, SHUT_RDWR);
    ::close(m_socket);
    m_socket = -1;
  }
  catch (const std::string& csErr)
  {
    cerr << "http_server::run: " << csErr << endl;
  }
  catch (...)
  {
    cerr << "http_server::run: Unhandled exception occurred" << endl;
  }
}

bool isEqualNoCase(const std::string& csVal1, const char* pszVal2)
{
  return ( pszVal2 )? (strcasecmp(csVal1.c_str(), pszVal2) == 0): false;
}

void* client_thread(void* arg)
{
  ClientData* client = (ClientData*) arg;
  http::connection_ptr conn;

  try
  {
    conn = http::connection::create(http::connection_type::http, http::connection_family::none);
    conn->open(client->socket);
    client->socket  = -1;
    http_server::processRequest(conn);
    //http_server::processRequest(client->socket);
  }
  catch (const sid::exception& e)
  {
    cerr << __func__ << ": " << e.what() << endl;
  }
  catch (const std::string& csErr)
  {
    cerr << __func__ << ": " << csErr << endl;
  }
  catch (...)
  {
    cerr << __func__ << ": An unhandled exception occurred" << endl;
  }

  if ( client )
  {
    if ( client->socket < 0 )
      ::close(client->socket);
    delete client;
  }
  http_server::get().clientSet.remove(pthread_self());
  cout << "Exiting thread " << pthread_self() << endl;
  return NULL;
}

std::string getResponseData(const std::string& csRes, bool bStatus)
{
  sid::uuid uuid;

  uuid.generate();

  std::ostringstream out;
  out << "<?xml version=\"1.0\"?>\r\n"
      << "<?win_agent?>\r\n"
      << "<RESPONSE AUTH_HANDLE=\"" << uuid.to_str() << "\">\r\n"
      << "<RESULT SUCCESS=\"" << (bStatus? "True":"False") << "\" VALUE=\"" << csRes << "\" />\r\n"
      << "</RESPONSE>\r\n"
    ;
  return out.str();
}

std::string getCurrentTime()
{
  char szNow[256] = {0};
  time_t now = time(NULL);
  struct tm *tmp = localtime(&now);
  if ( tmp )
    strftime(szNow, sizeof(szNow)-1, "%a, %d %b %Y %X %Z", tmp);

  return std::string(szNow);
}

std::string getResponse(const http::status& status, const std::string& csRes)
{
  http::response response;

  response.status = status;
  response.version = http::version_id::v11;
  response.headers("Date", getCurrentTime());
  response.headers("Content-Type", "text/xml");
  response.content.set_data(csRes);
  // determine the content length and add to the request
  response.headers("Content-Length", sid::to_str(response.content.length()));

  return response.to_str();
}

void http_server::processRequest(http::connection_ptr conn)
{
  cout << "Using connection pointer" << endl;
  http::request request;

  try
  {
    if ( ! request.recv(conn) )
      throw sid::exception("Failed to receivce request: " + request.error);

    cout << "============================================" << endl;
    cout << request.to_str() << endl << endl;
    if ( request.method == http::method_type::post )
    {
      if ( request.content().to_str() == "exit" )
      {
	gbExitLoop = true;
	cout << "Exit command received from the client" << endl;
	return;
      }
    }

    int sleepTime = 0;
    std::string sleepStr;
    if ( request.headers.exists("x-sid-server-sleep", &sleepStr) )
    {
      sleepTime = sid::to_num<int>(sleepStr);
    }
    if ( sleepTime > 0 )
    {
      // Sleep for a few seconds
      cout << "Sleeping for " << sleepTime << " second(s)" << endl;
      sleep(sleepTime);
    }

    // Construct the response object
    http::response response;
    response.status = http::status_code::OK;
    response.version = http::version_id::v11;
    response.headers("Date", getCurrentTime());
    response.headers("Content-Type", "text/xml");
    response.headers.add("X-Server", "Anand's Server");
    response.content.set_data("<Response></Response>");
    response.headers("Content-Length", sid::to_str(response.content.length()));
    // Send the response
    response.send(conn);

    if ( gbExitLoop )
      throw sid::exception("Server closing connection. Please try again later");
  }
  catch (const sid::exception& e)
  {
    cerr << __func__ << ": " << e.what() << endl;
  }
  catch (...)
  {
    cout << __func__ << ": An unhandled exception occurred" << endl;
  }
}

void http_server::processRequest(int clientSocket)
{
  char buffer[32*1024] = {0};
  std::string csRequest;
  int dataLen = 0;
  http::request request;

  try
  {
    bool bReceivedAll = false;
    do
    {
      bzero(buffer, sizeof(buffer));
      dataLen = read(clientSocket, buffer, sizeof(buffer)-1);
      if ( dataLen < 0 )
        throw std::string("Unable to read data: ")+http_server::errStr(errno);
      else if ( dataLen > 0 )
      {
	buffer[dataLen] = '\0';
	csRequest += buffer;
	bReceivedAll = ( (size_t) dataLen < sizeof(buffer)-1 );
      }
      else
	bReceivedAll = true;

      if ( bReceivedAll )
      {
	http::request request;
	request.set(csRequest);
	if ( request.method == http::method_type::post )
	{
	  if ( request.content().to_str() == "exit" )
	  {
	    gbExitLoop = true;
	    throw std::string("Exit command received from the client");
	  }
	}
	cout << "Request: " << request.to_str() << endl;
	csRequest.clear();
	sleep(5);
	std::string csResponse = getResponse(http::status_code::OK, "<Response />");
	::write(clientSocket, csResponse.c_str(), csResponse.length());
	break;
      }
    }
    while ( !gbExitLoop && !bReceivedAll );

    if ( gbExitLoop )
      throw std::string("Server closing connection. Please try again later");
  }
  catch (const http::status& status)
  {
    cout << status.to_str() << endl;
  }
  catch (const std::string& csErr)
  {
    cout << csErr << endl;
  }
  catch (...)
  {
    cout << "An unhandled exception occurred" << endl;
  }
}
