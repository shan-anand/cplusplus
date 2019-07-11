#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include "http_server.hpp"

#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

const uint16_t gServerPort = 5080;

const char gszPrompt[] = "http_server> ";
typedef std::vector<std::string> Args;
bool makeArgs(const std::string& csArgs, Args& args);

void exit_handler(int signal)
{
  cout << "Graceful shutdown" << endl;
  http_server::get().stop();
  //exit(0);
  return;
}

static pthread_t serverThreadId = 0;
static pthread_t inputThreadId = 0;
void* input_thread(void* arg);
void* server_thread(void* arg);
bool run_command(const std::string& cmdLine);

int main(int argc, char* argv[])
{
  signal(SIGINT, exit_handler);
  signal(SIGQUIT, exit_handler);
  signal(SIGABRT, exit_handler);

  int status;

  try
  {
    status = pthread_create(&inputThreadId, NULL, input_thread, NULL);
    if (status) throw std::string("Failed to create input thread");
    pthread_detach(inputThreadId);

    status = pthread_create(&serverThreadId, NULL, server_thread, NULL);
    if (status) throw std::string("Failed to create server thread");
    cout << "Server: Waiting for connections at port " << gServerPort << endl;
    pthread_join(serverThreadId, NULL);
  }
  catch (const std::string& err)
  {
    cerr << err << endl;
  }
  catch (...)
  {
    cerr << "An unhandled exception occurred" << endl;
  }
  return 0;
}

void* input_thread(void* arg)
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
  http_server::get().stop();

  return NULL;
}

void* server_thread(void* arg)
{
  http_server::get().run(gServerPort);
  return NULL;
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
    cout << "Server: Waiting for connections at port " << gServerPort << endl;
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
