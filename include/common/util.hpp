/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@brief List of utility functions in C++
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

#ifndef _SID_UTIL_H_
#define _SID_UTIL_H_

#include <string>
#include <vector>
#include "exception.hpp"

namespace sid {
namespace util {

struct command
{
  int         retVal;   //! Return value of the command
  std::string response; //! Successful response
  std::string error;    //! Error response

  //! Execute the given command
  static command execute(const std::string& _cmd);
  static command execute(const std::string& _cmd, const std::vector<std::string>& _params);

  //! Copy constructor
  command(const command&) = default;
  //! Copy operator
  command& operator=(const command&) = default;

private:
  //! Default constructor
  command();
  //! Constructor using sid::exception
  command(const sid::exception& _e);
  //! Copy operator using sid::exception
  command& operator=(const sid::exception& _e);
};

} // namespace util
} // namespace sid

#endif // _SID_UTIL_H_
