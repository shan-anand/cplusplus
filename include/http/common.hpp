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
 * @file common.hpp
 * @brief Defines the common http functions
 */
#ifndef _SID_HTTP_COMMON_
#define _SID_HTTP_COMMON_

#define CRLF "\r\n"

#include <string>
#include <iostream>
#include <ctime>

using namespace std;

namespace sid {
namespace http {

bool library_init();
void library_cleanup();

void set_verbose(bool _turnOn);
bool is_verbose();

std::string errno_str(const int _errno);
bool get_line(const std::string& _input, size_t& pos1, std::string& _output);

std::string date_to_str(const struct tm& _tm);
std::string date_to_str(const time_t& _tt);
bool date_from_str(const std::string& _input, struct tm& _tm);
bool date_from_str(const std::string& _input, time_t& _tt);

std::string url_encode(const std::string& _input);
std::string url_decode(const std::string& _input);

} // namespace http
} // namespace sid

#endif // _SID_HTTP_COMMON_

