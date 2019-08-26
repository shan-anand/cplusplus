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
 * @file headers.hpp
 * @brief Defines data type for HTTP header and headers entries.
 */
#ifndef _SID_HTTP_HEADERS_H_
#define _SID_HTTP_HEADERS_H_

#include <string>
#include <vector>
#include <common/simple_types.hpp>
#include <common/exception.hpp>
#include <common/optional.hpp>

namespace sid {
namespace http {

//! content encoding
enum class content_encoding : uint8_t { identity, gzip, compress, deflate, br };
using content_encoding_opt = sid::optional<content_encoding>;

//! Transfer encoding method
enum class transfer_encoding : uint8_t { none = 0, chunked, compress, deflate, gzip, identity };
using transfer_encoding_opt = sid::optional<transfer_encoding>;

enum class header_connection : uint8_t { close = 0, keep_alive };
using header_connection_opt = sid::optional<header_connection>;

using uint64_opt = sid::optional<uint64_t>;

//! header action
enum class header_action : uint8_t { replace, skip };

/**
 * @class header
 * @brief Defines one HTTP header entry with key/value pair.
 */
class header
{
public:
  //! Default constructor
  header();

  //! Constructor using key/value pair
  header(const std::string& _key, const std::string& _value);

  //! Copy constructor
  header(const header&) = default;

  //! Copy operator
  header& operator=(const header&) = default;

  //! Return the key/value pair as string
  std::string to_str() const;

  //! Get the header object using the given key/value string data.
  static header get(const std::string& _data);

public:
  std::string key, value;
};


/**
 * @class headers
 * @brief Defines a vector that holds one or more HTTP header entries.
 */
class headers : public std::vector<header>
{
  using super = std::vector<header>;
public:
  //! Default constructor
  headers();

  /**
   * @fn header& operator()(const header& obj);
   * @brief Add/Replace the header entry to the list.
   *
   * @param obj [in] header object to the added to the list.
   * @param _action [in] Merge action for existing entries (replace or skip)
   *
   * @return A reference to the header object that was just added.
   */
  header& operator()(const header& obj, const header_action& _action = header_action::replace);

  /**
   * @fn header& operator()(const std::string& key, const std::string& _value);
   * @brief Add/Replace the header entry to the list using the given key/value pair.
   *
   * @param _key [in] header key.
   * @param _value [in] Value associated with the key.
   * @param _action [in] Merge action for existing entries (replace or skip)
   *
   * @return A reference to the header object that was just added.
   */
  header& operator()(const std::string& _key, const std::string& _value, const header_action& _action = header_action::replace);

  /**
   * @fn header& add(const std::string& data);
   * @brief Add a new header entry to the list using the given data that is in key:value format.
   *
   * @param data[in] header value in key:value format.
   *
   * @return A reference to the header object that was just added.
   */
  header& add(const std::string& data);

  /**
   * @fn header& add(const std::string& _key, const std::string& _value);
   * @brief Add a new header entry to the list using the given key/value pair.
   *
   * @param _key [in] header key.
   * @param _value [in] Value associated with the key.
   *
   * @return A reference to the header object that was just added.
   */
  header& add(const std::string& _key, const std::string& _value);

  /**
   * @fn void add(const headers& headers, const header_action& _action);
   * @brief Add the incoming header list to the existing list using AddType flag.
   *
   * @param headers [in] Incoming header list that needs to be merged.
   * @param _action [in] Merge action for existing entries (replace or skip)
   *
   * @return A reference to the header object that was just added.
   */
  void add(const headers& headers, const header_action& _action = header_action::replace);

  /**
   * @fn bool exists(const std::string& _key, std::string* _pValue);
   * @brief Checks whether the requested key exists or not.
   *
   * @param _key [in] header key.
   * @param _pValue [out] Optional pointer to fill the value if the key exists
   *
   * @return true if the key exists, false otherwise
   */
  bool exists(const std::string& _key, std::string* _pValue = nullptr) const;

  /**
   * @fn std::string get(const std::string& _key, bool* _pisFound);
   * @brief Get the requested value for the key from the headers. If the key doesn't exist it returns an empty string.
   *
   * @param _key [in] header key.
   * @param _pisFound [out] If the header key is found it is set to true, otherwise it is set to false.
   *
   * @return The value associated with the key if found, otherwise it returns empty string. _pisFound variable is set to false in that case.
   */
  std::string get(const std::string& _key, bool* _pisFound = nullptr) const;

  /**
   * @fn std::vector<std::string> get_all(const std::string& _key) const;
   * @brief Get all the values for the requested key from the headers. If the key doesn't exist it returns an empty vector.
   *
   * @param _key [in] header key.
   *
   * @return The values associated with one or more key if found, otherwise it returns empty vector.
   */
  std::vector<std::string> get_all(const std::string& _key) const;

  /**
   * @fn size_t removeAll(const std::string& _key);
   * @brief Removes all the requested keys from the headers and returns the number of keys removed.
   *
   * @param _key [in] header key.
   *
   * @return The number of keys removed from the list
   */
  size_t remove_all(const std::string& _key);

  //! Return a CRLF separated list of header key/value pair as string.
  std::string to_str() const;

  // Get the "Content-Length" header. Returns 0 if it is not found. Check for *_pisFound for existence
  uint64_t content_length(bool* _pisFound = nullptr) const;

  //! Get "Content-Encoding" header
  http::content_encoding content_encoding(bool* _pisFound = nullptr) const;

  //! Get "Transfer-Encoding" header
  http::transfer_encoding transfer_encoding(bool* _pisFound = nullptr) const;

  //! Get "Connection" header
  http::header_connection connection(bool* _pisFound = nullptr) const;

protected:
  /**
   * @fn headers::iterator find(const std::string& _key) const;
   * @brief Check whether the key exists in the list.
   *
   * @param _key [in] header key.
   *
   * @return The iterator object.
   */
  headers::iterator find(const std::string& _key);
  headers::const_iterator find(const std::string& _key) const;
};

} // namespace http
} // namespace sid

#endif // _SID_HTTP_HEADERS_H_
