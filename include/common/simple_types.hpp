/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@brief Declaration of most commonly used data types
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

/**
 * @file  simple_types.hpp
 * @brief Declaration of most commonly used data types
 */

 #pragma once

#include <string>
#include <functional>

namespace sid {

typedef std::function<bool()> FNContinueCallback;

//! basic_cred data structure
struct basic_cred
{
  std::string userName, password;

  //! Default constructor
  basic_cred() { clear(); }
  //! Copy constructor
  basic_cred(const basic_cred&) = default;
  //! All-arg constructor
  basic_cred(const std::string& _userName, const std::string& _password) :
    userName(_userName), password(_password) {}
  //! Virtual destructor
  virtual ~basic_cred() {}

  //! Clears the members of the structure
  inline void clear() { userName.clear(); password.clear(); }
  //! Checks whether either of the members is empty
  inline bool empty() const { return userName.empty() || password.empty(); }

  //! Equality / Unequality functions
  inline bool equals(const basic_cred& _obj) const 
    { return userName == _obj.userName && password == _obj.password; }
  inline bool operator==(const basic_cred& _obj) const { return equals(_obj); }
  inline bool operator!=(const basic_cred& _obj) const { return !equals(_obj); }

  //! set both username and password and return the object
  inline basic_cred& set(const std::string& _userName, const std::string& _password)
    { this->userName = _userName; this->password = _password; return *this; }

  //! set both username and password that is separated by a delimiter
  inline bool set(const std::string& _userPassword, const char _delimiter = ':')
    {
      size_t pos = _userPassword.find(_delimiter);
      if ( pos == std::string::npos ) return false;
      this->userName = _userPassword.substr(0, pos);
      this->password = _userPassword.substr(pos+1);
      return true;
    }
};

} // namespace sid
