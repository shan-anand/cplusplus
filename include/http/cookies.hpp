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
 * @file cookies.hpp
 * @brief Defines data type for HTTP Cookie entry.
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include "connection.hpp"
#include "request.hpp"
#include "response.hpp"
#include <mutex>

namespace sid::http {

//! Cookie expiration type
enum class cookie_expiration : uint8_t { none = 0, expire, max_age };

/**
 * @class cookie
 * @brief Defines one HTTP cookie entry with all entries
 */
class cookie
{
public:
  //! Default Constructor
  cookie() { clear(); }
  cookie(const cookie& obj) = default;
  virtual ~cookie() {}
  cookie& operator=(const cookie&) = default;

  bool set(const std::string& _value);
  void clear();
  std::string to_str(bool _forRequest = true) const;
  bool is_expired() const;
  bool equals(const std::string& _name) const;

public:
  struct name_value {
    std::string name, value;
    bool empty() const { return name.empty(); }
    void clear() { name.clear(); value.clear(); }
    std::string to_str() const { return empty()? "" : (name + "=" + value); }
  };
  struct expiration_info
  {
    cookie_expiration type;
    union
    {
      time_t       time;
      unsigned int max_age;
   };
  };

  name_value       entry;
  bool             is_secure;
  bool             is_http_only;
  std::string      domain;
  std::string      path;
  expiration_info  expiration;
  time_t           time_received;
};

/**
 * @class cookies
 * @brief Defines a list of HTTP cookies
 */
class cookies : public std::vector<cookie>
{
  using super = std::vector<cookie>;
public:
  size_t add(response& _response) const;
  size_t add(request& _request, const connection_ptr _conn) const;
  cookies::const_iterator find(const std::string& _name) const;
  cookies::iterator find(const std::string& _name);
  bool remove(const std::string& name, cookie* _pCookie = nullptr);

  //! Get all the cookies from the response headers
  static cookies get_response_cookies(const headers& _headers, bool _forceGetAll = false);
  static cookies get_session_cookies(const std::string& _host);
  static void set_session_cookie(const std::string& _host, const cookie& _cookie);
  static bool remove_session_cookie(const std::string& _host, const std::string& _name, cookie* _pCookie = nullptr);
  static void clear_session_cookies(const std::string& _host);
};

/**
 * @class cookies_map
 * @brief Contains a list of cookie per server
 *        It is a thread-safe class
 */
class cookies_map : public std::map<std::string, cookies>
{
  using super = std::map<std::string, cookies>;
public:
  cookies get(const std::string& _host) const;
  void set(const std::string& _host, const cookie& _cookie);
  bool remove(const std::string& _host, const std::string& _name, cookie* _pCookie = nullptr);
  void clear(const std::string& _host);
private:
  mutable std::recursive_mutex m_mutex;
};

} // namespace sid::http
