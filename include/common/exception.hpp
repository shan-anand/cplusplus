/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@brief Exception object thrown by all the functions in the project
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
#ifndef _SID_EXCEPTION_H_
#define _SID_EXCEPTION_H_

#include <string>
#include <exception>

namespace sid {

/**
 * @class exception
 * @brief exception class used by the library
 */
class exception : public std::exception
{
public:
  using super = std::exception;
  exception() noexcept { m_code = 0; }
  exception(const std::string& _msg) noexcept : m_code(-1), m_msg(_msg) {}
  exception(int _code, const std::string& _msg) noexcept : m_code(_code), m_msg(_msg) {}
  //! Default copy constructor
  exception(const exception&) = default;
  virtual ~exception() {}
  //! Default copy operator
  exception& operator= (const exception&) = default;
  
  const char* what() const noexcept override { return m_msg.c_str(); }
  const std::string& message() const noexcept { return m_msg; }

  int code() const noexcept { return m_code; }
  bool success() const noexcept { return m_code == 0; }
  bool failure() const noexcept { return m_code != 0; }

  void clear() { m_code = 0; m_msg.clear(); }

  void set(const std::string& _msg) { m_code = -1; m_msg = _msg; }
  void set(int _code, const std::string& _msg) { m_code = _code; m_msg = _msg; }

protected:
  int         m_code;
  std::string m_msg;
};

} // namespace sid

#endif // _SID_EXCEPTION_H_
