#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <signal.h>
#include <thread>
#include <atomic>
#include <http/http.hpp>
#include "common/uuid.hpp"
#include "common/optional.hpp"

#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <openssl/ssl.h>

using namespace std;
using namespace sid;

const char gszPrompt[] = "http_server> ";
using Args = std::vector<std::string>;
bool makeArgs(const std::string& csArgs, Args& args);

void input_thread();
void server_thread();
bool run_command(const std::string& cmdLine);

bool s_set_non_blocking(bool _isEnable)
{
  int flags = ::fcntl(STDIN_FILENO, F_GETFL);
  if ( flags == -1 ) return false;
  if ( _isEnable )
    flags |= O_NONBLOCK;
  else
    flags &= ~O_NONBLOCK;
  return (::fcntl(STDIN_FILENO, F_SETFL, flags) == 0);
}

using ProcessThreadMap = std::map<uint64_t, std::thread>;

struct Global
{
  std::string           scriptName;
  bool                  exit;
  std::atomic<uint64_t> totalProcessed;
  ProcessThreadMap      processMap;

  http::connection_type type() const { return m_type; }
  void set_type(http::connection_type _type) { m_type = _type; }
  uint16_t port() const { return m_port > 0? m_port : m_type == http::connection_type::http? 5080 : 5443; }
  void set_port(uint16_t _port) { m_port = _port; }

  Global() : scriptName(), totalProcessed(0), processMap(), m_type(http::connection_type::http), m_port(0) {}

private:
  http::connection_type m_type;
  uint16_t m_port;
};

static Global global;
static void parseCommandLine(const std::vector<std::string>& args);

void exit_handler(int signal)
{
  cout << "Graceful shutdown" << endl;
  global.exit = true;
  //exit(0);
  return;
}

int main(int argc, char* argv[])
{
  signal(SIGINT, exit_handler);
  signal(SIGQUIT, exit_handler);
  signal(SIGABRT, exit_handler);

  try
  {
    //sid::hash::init();
    global.scriptName = argv[0];

    std::vector<std::string> args;
    for ( int i = 1; i < argc; i++ )
      args.push_back(argv[i]);

    parseCommandLine(args);

    //s_set_non_blocking(true);
    std::thread in_thread([=](){input_thread();});
    in_thread.detach();

    std::thread svr_thread([=](){server_thread();});
    cout << "Waiting for " << (global.type() == http::connection_type::http? "HTTP" : "HTTPS") << " connections at port " << global.port() << endl;
    svr_thread.join();
    //s_set_non_blocking(false);
    //in_thread.join();
  }
  catch (const std::string& err)
  {
    cerr << err << endl;
  }
  catch (const std::exception& e)
  {
    cerr << e.what() << endl;
  }
  catch (...)
  {
    cerr << "An unhandled exception occurred" << endl;
  }
  return 0;
}

void input_thread()
{
  std::string cmdLine;
  bool bContinue = true;

  #define HISTORY_FILE "~/.http_server_history"
  using_history();
  read_history(HISTORY_FILE);

  auto is_ready_for_read = [&]()->bool
    {
      struct timespec ts = {1000, 0};
      pollfd poll_fd = {STDIN_FILENO, POLLRDHUP, 0};
      poll_fd.events |= (POLLIN | POLLPRI);
      int ret = ppoll(&poll_fd, 1, &ts, nullptr);
      if ( ret == -1 )
	throw sid::exception(http::errno_str(errno));

      if ( ret > 0 )
      {
	const int& revents = poll_fd.revents;
	if ( (revents & POLLIN) )
	  return true;
	if ( (revents & POLLERR) )
	  throw sid::exception("Error polling for data");
	if ( (revents & POLLHUP) )
	  throw sid::exception("Error polling for data as the peer closed the connection");
	if ( (revents & POLLNVAL) )
	  throw sid::exception("Error polling for data due to invalid request");
      }
      return false;
    };

  for ( bool bShowPrompt = true; bContinue && !global.exit; )
  {
    if ( bShowPrompt ) { cout << gszPrompt; cout.flush(); bShowPrompt = false; }
    if ( !is_ready_for_read() ) continue;
    char* pszCmdLine = readline(nullptr);
    //char* pszCmdLine = readline(gszPrompt);
    cmdLine = pszCmdLine? pszCmdLine:"exit";
    if ( pszCmdLine ) free(pszCmdLine);
    append_history(1, HISTORY_FILE);
    bContinue = run_command(cmdLine);
    bShowPrompt = true;
  }
  global.exit = true;
}

void process_client(http::connection_ptr conn, const uint64_t currentProcessId)
{
  try
  {
    if ( global.exit )
      throw sid::exception("Exiting process " + sid::to_str(currentProcessId) + " before reading request");

    http::request request;
    if ( ! request.recv(conn) )
      throw sid::exception("Failed to receive request: " + request.error);

    cout << "============================================" << endl;
    cout << request.to_str() << endl << endl;

    bool sleepBlock = false;
    std::string sleepStr;
    if ( request.headers.exists("x-sid-server-sleep", &sleepStr) || (sleepBlock = request.headers.exists("x-sid-server-sleep-block", &sleepStr)) )
    {
      uint64_t sleepTime = 0;
      if ( sid::to_num(sleepStr, /*out*/ sleepTime) && sleepTime > 0 )
      {
	// Restrict sleep time to a maximum of 1 minute
	if ( sleepTime > 60 )
	  sleepTime = 60;
	auto sleep_for_microsecs = [&](uint64_t microSeconds, auto break_callback)->bool
	  {
	    const uint64_t defInterval = 100'000; // Default is 100 milli-seconds
	    while ( microSeconds > 0 )
	    {
	      if ( break_callback() ) return false;
	      uint64_t interval = (microSeconds > defInterval)? defInterval : microSeconds;
	      usleep(interval);
	      microSeconds -= interval;
	    }
	    return true;
	  };
	auto sleep_for_millisecs = [&](uint64_t milliSeconds, auto break_callback)->bool { return sleep_for_microsecs(milliSeconds * 1000, break_callback); };
	auto sleep_for_secs = [&](uint64_t seconds, auto break_callback)->bool { return sleep_for_microsecs(seconds * 1'000'000, break_callback); };
	// Sleep for a few seconds
	cout << "Sleeping for " << sleepTime << " second(s)" << endl;
	sleep_for_secs(sleepTime, [&](){ return sleepBlock? false : global.exit; });
      }
    }

    if ( request.method == http::method_type::post )
    {
      if ( request.content().to_str() == "exit" )
      {
	global.exit = true;
	throw sid::exception("Exit command received from the client");
      }
    }

    if ( global.exit )
      throw sid::exception("Exiting process " + sid::to_str(currentProcessId) + " before sending response");

    if ( request.headers.exists("x-sid-server-kill") )
      kill(getpid(), SIGKILL);

    // Construct the response object
    http::response response;
    response.status = http::status_code::OK;
    response.version = http::version_id::v11;
    response.headers("Date", http::date_to_str(::time(nullptr)));
    response.headers("Content-Type", "text/xml");
    response.headers.add("X-Server", "Anand's Server");
    response.content.set_data("<ProcessCount>" + sid::to_str(currentProcessId) + "</ProcessCount>");
    response.headers("Content-Length", sid::to_str(response.content.length()));

    // Send the response
    if ( ! response.send(conn) )
      throw sid::exception(response.error);
    cout << response.content.to_str() << endl;
  }
  catch (const sid::exception& e)
  {
    cerr << "process_callback: " << e.what() << endl;
  }
  catch (...)
  {
    cerr << "process_callback: An unhandled exception occurred" << endl;
  }
  global.processMap.erase(global.processMap.find(currentProcessId));
}

void server_thread()
{
  try
  {
    http::FNExitCallback exit_callback = []() { return global.exit; };

    http::FNProcessCallback process_callback = [](http::connection_ptr conn)
      {
	const uint64_t currentProcessId = ++global.totalProcessed;
	// start a new thread
	global.processMap[currentProcessId] = std::thread([=](){process_client(conn, currentProcessId);});
	global.processMap[currentProcessId].detach();
	//process_client(conn, currentProcessId);
      };

    /*
    ssl::client_certificate sslClientCert;
    if ( global.type() == http::connection_type::https )
    {
      sslClientCert.chainFile = "/etc/ssl/certs/sid_https_server_cert.pem";
      sslClientCert.privateKeyFile = "/etc/ssl/private/sid_https_server_key.pem";
      sslClientCert.privateKeyType = SSL_FILETYPE_PEM;
    }
    */

    http::server_ptr server = http::server::create(global.type());
    /*
    http::server_ptr server;
    if ( global.type() == http::connection_type::http )
      server = http::server::create(global.type());
    else
      server = http::server::create(sslClientCert);
    */

    if ( !server->run(global.port(), process_callback, exit_callback) )
      throw server->exception();
  }
  catch (const sid::exception& e)
  {
    cerr << e.what() << endl;
  }
  catch (...)
  {
    cerr << __func__ << ": An unhandled exception occurred" << endl;
  }
  global.exit = true;
  if ( ! global.processMap.empty() )
  {
    // Wait for completion of all the detached process threads
    cout << "Process threads waiting to be completed: " << global.processMap.size() << endl;
    while ( !global.processMap.empty() )
      usleep(10'000);
  }
  cout << __func__ << ": Exiting" << endl;
}

bool run_command(const std::string& cmdLine)
{
  if ( cmdLine.empty() ) return true;

  std::string cmd;
  Args args;
  // add history entry for manipulating it
  add_history(cmdLine.c_str());

  if ( cmdLine == "exit" || cmdLine == "quit" )
    return false;
  if ( cmdLine == "history" )
  {
    HIST_ENTRY** ppHistory = history_list();
    if ( !ppHistory ) return true;
    for ( int i = 0; ppHistory[i] != NULL; i++ )
    {
      cout << i+1 << "  " << ppHistory[i]->line << endl;
    }
    return true;
  }

  size_t pos = cmdLine.find(' ');
  if ( pos != std::string::npos )
  {
    cmd = cmdLine.substr(0, pos);
    if ( !makeArgs(cmdLine.substr(pos+1), args) )
      return true;
  }
  else
    cmd = cmdLine;

  if ( cmd == "status" )
  {
    if ( ! args.empty() )
    {
      cerr << cmd << " does not take arguments" << endl;
      return true;
    }
    cout << "Waiting for " << (global.type() == http::connection_type::http? "HTTP" : "HTTPS") << " connections at port " << global.port() << endl;
    cout << "Total Processed: " << global.totalProcessed << endl;
  }
  return true;
}

bool makeArgs(const std::string& csArgs, Args& args)
{
  size_t p1, p2;
  for ( p1 = p2 = 0; (p2 = csArgs.find(' ', p1)) != std::string::npos; p1=p2+1 )
  {
    std::string csArg = csArgs.substr(p1, p2-p1);
    if ( !csArgs.empty() )
      args.push_back(csArg);
  }
  std::string csArg = csArgs.substr(p1);
  if ( !csArgs.empty() )
    args.push_back(csArg);

  return true;
}

void parseCommandLine(const std::vector<std::string>& args)
{
  struct Param
  {
    Param() { hasData = false; id = -1; }
    std::string key, value;
    bool hasData;
    int  id;
  };

  struct Cmd
  {
    sid::optional<http::connection_type> type;
    sid::optional<uint16_t> port;
    Cmd() { clear(); }
    void clear() { type.clear(global.type()); port.clear(global.port()); }
  } cmd;

  for ( size_t i = 0; i < args.size(); i++ )
  {
    Param param;
    std::string arg = args[i];

    param.id = i;
    size_t pos = arg.find('=');
    if ( pos == std::string::npos )
      param.key = arg;
    else
    {
      param.hasData = true;
      param.key = arg.substr(0, pos);
      param.value = arg.substr(pos+1);
    }
    if ( param.key == "--help" )
    {
      cout << "Usage: " << endl;
      cout << global.scriptName << " [--type=http|https] [--port=<port_number>]" << endl;
      exit(0);
    }
    else if ( param.key == "--type" )
    {
      if ( cmd.type.exists() )
	sid::exception(param.key + " cannot be repeated");
      if ( !param.hasData )
	sid::exception(param.key + " must have data");
      if ( param.value == "http" )
	cmd.type = http::connection_type::http;
      else if ( param.value == "https" )
	cmd.type = http::connection_type::https;
      else
	sid::exception(param.key + " must be http|https");
    }
    else if ( param.key == "--port" )
    {
      if ( cmd.port.exists() )
	sid::exception(param.key + " cannot be repeated");
      if ( !param.hasData )
	sid::exception(param.key + " must have data");
      uint16_t port = 0;
      std::string errStr;
      if ( !sid::to_num(param.value, /*out*/ port, &errStr) )
	throw sid::exception(param.key + " error: " + errStr);
      if ( port == 0 )
	throw sid::exception(param.key + " cannot be 0");
      cmd.port = port;
    }
    else
      throw sid::exception("Invalid command line parameter: " + param.key);
  } // end of for loop

  // set the global variables
  if ( cmd.type.exists() )
    global.set_type(cmd.type());
  if ( cmd.port.exists() )
    global.set_port(cmd.port());
}
