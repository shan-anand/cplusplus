/**
 * @file util.cpp
 * @brief Class implementations of Message digest algorithms
 */

/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@brief List of common functions in C++
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

#include <iostream>
#include "common/convert.hpp"
#include "common/util.hpp"

#include <fcntl.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;
using namespace sid::util;

/////////////////////////////////////////////////////////////////////////////////
//
// Implementation of util
//
command::command() : retVal(0), response(), error()
{
}

command::command(const sid::exception& _e)
{
  *this = _e;
}

command& command::operator=(const sid::exception& _e)
{
  this->retVal = _e.code();
  if ( this->retVal == 0 )
    this->response = _e.message();
  else
    this->error = _e.message();
  return *this;
}

/*static*/
command command::execute(const std::string& _cmd)
{
  std::vector<std::string> params;
  sid::split(params, _cmd, ' ', SPLIT_SKIP_EMPTY);
  const std::string cmd = params[0];
  params.erase(params.begin());
  return command::execute(cmd, params);
}

/*static*/
command command::execute(const std::string& _cmd, const std::vector<std::string>& _params)
{
  command cmdOut;

  try
  {
    int pipefd_stdout[2] = {-1, -1};
    int pipefd_stderr[2] = {-1, -1};

    if ( ::pipe(pipefd_stdout) == -1 )
      throw sid::exception("Cannot open file for stdout");
    if ( ::pipe(pipefd_stderr) == -1 )
      throw sid::exception("Cannot open file for stderr");

    int pid = ::fork();
    if ( pid < 0 )
      throw sid::exception(errno, "Failed to fork");

    if ( pid == 0 )
    {
      // Close read end of the pipes
      ::close(pipefd_stdout[0]);
      ::close(pipefd_stderr[0]);
      // Duplicate the fd
      ::dup2(pipefd_stdout[1], 1);
      ::dup2(pipefd_stderr[1], 2);
      // call exec
      const char* cparams[_params.size()+2] = {0};
      cparams[0] = _cmd.c_str();
      for ( size_t i = 0; i < _params.size(); i++ )
        cparams[i+1] = _params[i+1].c_str();
      if ( -1 == ::execve(cparams[0], (char* const*) cparams, (char* const*) nullptr) )
        throw sid::exception(errno, sid::to_errno_str(errno, "Failed to run execve()"));
    }

    // Close write end of the pipes
    ::close(pipefd_stdout[1]);
    ::close(pipefd_stderr[1]);

    char buffer[4096] = {0};
    ::memset(buffer, 0, sizeof(buffer));
    ssize_t n = 0;
    while ( 0 < (n = ::read(pipefd_stdout[0], buffer, sizeof(buffer)-1)) )
    {
      cmdOut.response += buffer;
      ::memset(buffer, 0, sizeof(buffer));
    }
    //cout << cmdOut.response << endl;

    ::memset(buffer, 0, sizeof(buffer));
    while ( 0 < (n = ::read(pipefd_stderr[0], buffer, sizeof(buffer)-1)) )
    {
      cmdOut.error += buffer;
      ::memset(buffer, 0, sizeof(buffer));
    }
    //cout << cmdOut.error << "====" << endl;

    // wait for completion of the child
    //::waitpid(pid, &cmdOut.retVal, WEXITSTATUS);
    ::waitpid(pid, &cmdOut.retVal, 0);
  }
  catch (const sid::exception& _e)
  {
    cmdOut = _e;
  }
  catch (...)
  {
    cmdOut = sid::exception(-1, "An unhandled exception occurred while running the command " + _cmd);
  }

  return cmdOut;
}
