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
 * @file content.hpp
 * @brief Defines the HTTP content object.
 */
#ifndef _SID_HTTP_CONTENT_H_
#define _SID_HTTP_CONTENT_H_

#include <string>
#include <fstream>

namespace sid {
namespace http {

/**
 * @class content
 * @brief Definition of content object used in request and response.
 */
class content
{
public:
  //! Default constructor
  content();

  //! Copy constructor
  content(const content& obj);

  //! Virtual destructor
  virtual ~content();

  //! Copy operator
  content& operator=(const content& obj);

  /**
   * @fn void clear();
   * @brief Clear the object so that it can be reused again. Clears all the properties and makes it like a new object.
   *
   * @param _isFullReset [in] If set to true the object is reset to a fresh state using string data.
   *                          If set to false the object will retain the data state. If it uses a file, the file will be truncated.
   */
  void clear(bool _isFullReset = false);

  /**
   * @fn void set_data(const std::string& _data, size_t _len = std::string::npos);
   * @brief Set the contents as data. The current object is cleared before doing this operation.
   *
   * @param _data Payload
   * @param _len  if std::string::npos it takes the whole string, otherwise it restricts the length.
   *
   */
  void set_data(const std::string& _data, size_t _len = std::string::npos);

  /**
   * @fn void set_file(const std::string& _filePath, bool _doTruncateFile = false);
   * @brief Set the contents as a file in the file system. The current object is cleared before doing this operation.
   *        If there is an error trying to open the file a sid::exception is thrown.
   */
  void set_file(const std::string& _filePath, bool _doTruncateFile = false);

  /**
   * @fn std::string to_str() const;
   * @brief Return the content as a string.
   */
  std::string to_str() const;

  /**
   * @fn void append(const std::string& _data, size_t _pos = 0, size_t _len = std::string::npos);
   * @brief Appends data to the end.
   *
   * @param _data Data to be appended
   * @param _pos Position in _data from which string needs to be used. 
   * @param _len  if std::string::npos it takes the entire string from the given position, otherwise it restricts the length.
   *
   * @note This also sets the m_length member.
   */
  void append(const std::string& _data, size_t _pos = 0, size_t _len = std::string::npos);

  //! Checks whether the content is empty or has data
  bool empty() const { return (m_length == 0); }

  //! Returns the length of the content
  size_t length() const { return m_length; }

  //! Checks whether the content is in a file 
  bool is_file() const { return m_dataIsFilePath; }

  //! Checks whether the content is raw string
  bool is_string() const { return !m_dataIsFilePath; }

  //! Gets the file path if the content is a file, otherwise it returns an empty string
  std::string file_path() const { return m_dataIsFilePath? m_data : std::string(); }

private:
  bool         m_dataIsFilePath; //! Indicates whether the m_data variable is a file path or not
  std::string  m_data;           //! Actual data or full path to the file that has data
  size_t       m_length;         //! Length of the data
  std::fstream m_file;           //! File stream (used when file path is used)
};

} // namespace http
} // namespace sid

#endif // _SID_HTTP_CONTENT_H_
