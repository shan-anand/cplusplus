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

/**
 * @file www_authenticate.hpp
 * @brief Defines the HTTP response object.
 */

#pragma once

#include <string>
#include <vector>
#include <map>

namespace sid::http {

/**
 * @struct www_authenticate
 * @brief HTTP www_authenticate header object and parser
 */
struct www_authenticate
{
  using auth_pair = std::map<std::string, std::string>;
  std::string type;
  auth_pair   info;
  void clear() { type.clear(); info.clear(); }
  bool empty() const { return type.empty(); }
  bool exists(const std::string& _key, std::string& _value) const;
  std::string get_auth_string(const http::request& _request);
};

/**
 * @struct www_authenticate_list
 * @brief HTTP www_authenticate header object and parser
 */
struct www_authenticate_list : public std::vector<www_authenticate>
{
  using super = std::vector<www_authenticate>;
public:
  void set(const std::string& _wwwAuthStr);
};


} // namespace sid::http
