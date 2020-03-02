#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <signal.h>
#include <thread>
#include "common/optional.hpp"
#include "http_server.hpp"

#include <readline/readline.h>
#include <readline/history.h>

using namespace std;
using namespace sid;

extern bool gbExitLoop;
const char gszPrompt[] = "http_server> ";
using Args = std::vector<std::string>;
bool makeArgs(const std::string& csArgs, Args& args);

void exit_handler(int signal)
{
  cout << "Graceful shutdown" << endl;
  gbExitLoop = true;
  //exit(0);
  return;
}

void input_thread();
void server_thread();
bool run_command(const std::string& cmdLine);

struct Global
{
  std::string scriptName;
  http::connection_type type() const { return m_type; }
  void set_type(http::connection_type _type) { m_type = _type; }
  uint16_t port() const { return m_port > 0? m_port : m_type == http::connection_type::http? 5080 : 5443; }
  void set_port(uint16_t _port) { m_port = _port; }

  Global() : scriptName(), m_type(http::connection_type::http), m_port(0) {}

private:
  http::connection_type m_type;
  uint16_t m_port;
};

static Global global;
static void parseCommandLine(const std::vector<std::string>& args);

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

    std::thread in_thread([=](){input_thread();});
    in_thread.detach();

    std::thread svr_thread([=](){server_thread();});
    cout << "Waiting for " << (global.type() == http::connection_type::http? "HTTP" : "HTTPS") << " connections at port " << global.port() << endl;
    svr_thread.join();
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

  #define HISTORY_FILE "~/.httpServer_history"
  using_history();
  read_history(HISTORY_FILE);

  while ( bContinue )
  {
    char* pszCmdLine = readline(gszPrompt);
    cmdLine = pszCmdLine? pszCmdLine:"exit";
    if ( pszCmdLine ) free(pszCmdLine);
    append_history(1, HISTORY_FILE);
    bContinue = run_command(cmdLine);
  }
  //http_server::get().stop();
  gbExitLoop = true;
}

std::string s_getCurrentTime()
{
  char szNow[256] = {0};
  time_t now = time(NULL);
  struct tm *tmp = localtime(&now);
  if ( tmp )
    strftime(szNow, sizeof(szNow)-1, "%a, %d %b %Y %X %Z", tmp);

  return std::string(szNow);
}

void server_thread()
{
  try
  {
    http::FNExitCallback exit_callback = []() { return gbExitLoop; };

    http::FNProcessCallback process_callback = [](http::connection_ptr conn)
      {
	// start a new thread???
	try
	{
	  http::request request;
	  if ( ! request.recv(conn) )
	    throw sid::exception("Failed to receive request: " + request.error);

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

	  if ( request.headers.exists("x-sid-server-kill") )
	    kill(getpid(), SIGKILL);

	  // Construct the response object
	  http::response response;
	  response.status = http::status_code::OK;
	  response.version = http::version_id::v11;
	  response.headers("Date", s_getCurrentTime());
	  response.headers("Content-Type", "text/xml");
	  response.headers.add("X-Server", "Anand's Server");
	  response.content.set_data("<Response></Response>");
	  response.headers("Content-Length", sid::to_str(response.content.length()));
	  // Send the response
	  response.send(conn);
	}
	catch (const sid::exception& e)
	{
	  cerr << "process_callback: " << e.what() << endl;
	}
	catch (...)
        {
	  cout << "process_callback: An unhandled exception occurred" << endl;
	}
      };

    http::server_ptr server = http::server::create(global.type());
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
    cout << "Total processed: " << http_server::get().totalProcessed << endl;
    cout << "Processing " << http_server::get().clientSet.size() << " client requests" << endl;
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
