/*
 * @file http_server.h
 * @brief Header file for a simple HTTP server
 */
#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include "http/http.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <stdio.h>
#include <time.h>
#include <map>
#include <set>

namespace sid {

class handle_map
{
public:
  bool add(const std::string& handle);
  bool remove(const std::string& handle);
  bool exists(const std::string& handle, bool* pbTimedOut);

private:
  std::map<std::string, time_t> m_map;
};

class client_set : public std::set<pthread_t>
{
public:
  bool add(pthread_t threadId) { this->insert(threadId); return true; }
  bool remove(pthread_t threadId) { return ( this->erase(threadId) > 0 ); }
};

class http_server
{
public:
  void run(unsigned short port = 80);
  void stop();

  static http_server& get();

  handle_map handleMap;
  client_set clientSet;
  size_t     totalProcessed;

public:
  static std::string errStr(const int iError);
  static void processRequest(http::connection_ptr conn);
  static void processRequest(int clientSocket);

private:
  http_server();
  int m_socket;
  unsigned int m_port;
};

} // namespace sid

#endif // _HTTP_SERVER_H_
