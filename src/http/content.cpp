//////////////////////////////////////////////////////
//
// content.cpp
//
//////////////////////////////////////////////////////

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

#include "http/content.hpp"
#include "common/convert.hpp"
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace sid;
using namespace sid::http;

//! Default constructor
content::content() : m_dataIsFilePath(false), m_data(), m_length(0)
{
}

//! Copy constructor
content::content(const content& obj)
{
  *this = obj;
}

//! Virtual destructor
content::~content()
{
}

//! Copy constructor
content& content::operator=(const content& obj)
{
  if ( this == &obj ) return *this;
  this->clear(true);
  if ( obj.is_string() )
    this->set_data(obj.m_data);
  else
    this->set_file(obj.m_data);
  this->m_length = obj.m_length;
  return *this;
}

/**
 * @fn void clear();
 * @brief Clear the object so that it can be reused again. Clears all the properties and makes it like a new object.
 *
 * @param _isFullReset [in] If set to true the object is reset to a fresh state using string data.
 *                          If set to false the object will retain the data state. If it uses a file, the file will be truncated.
 */
void content::clear(bool _isFullReset/* = false*/)
{
  if ( _isFullReset )
  {
    this->m_dataIsFilePath = false;
    this->m_data.clear();
    this->m_length = 0;
    if ( this->m_file.is_open() )
      this->m_file.close();
  }
  else
  {
    if ( !this->m_dataIsFilePath )
    {
      this->m_data.clear();
      this->m_length = 0;
    }
    else
    {
      // Set the file pointers to the beginning of the file
      m_file.seekg(0);
      m_file.seekp(0);
      // Truncate the file using the file path
      if ( -1 == ::truncate(m_data.c_str(), 0) )
	throw sid::exception("Unable to truncate file " + m_data);
      this->m_length = 0;
    }  
  }
}

/**
 * @fn void set_data(const std::string& _data, size_t _len = std::string::npos);
 * @brief Set the contents as data. The current object is cleared before doing this operation.
 *
 * @param _data Payload
 * @param _len  if std::string::npos it takes the whole string, otherwise it restricts the length.
 *
 */
void content::set_data(const std::string& _data, size_t _len/* = std::string::npos*/)
{
  // Clear the current object
  clear();

  // Set the member variables
  m_dataIsFilePath = false;
  if ( _len == std::string::npos )
    m_data = _data;
  else
    m_data = _data.substr(0, _len);
  m_length = m_data.length();
}

/**
 * @fn void set_file(const std::string& _filePath, bool _doTruncateFile = false);
 * @brief Set the contents as a file in the file system. The current object is cleared before doing this operation.
 *        If there is an error trying to open the file a sid::exception is thrown.
 */
void content::set_file(const std::string& _filePath, bool _doTruncateFile/* = false*/)
{
  // Clear the current object
  clear();
 
  struct stat st = {0};
  std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out;
  if ( _doTruncateFile )
    mode |= std::ios_base::trunc;
  else
    mode |= std::ios_base::app;

  m_file.open(_filePath, mode);
  if ( !m_file.is_open() )
    throw sid::exception("Failed to open the file: " + _filePath);

  ::stat(_filePath.c_str(), &st);

  // Set the member variables
  m_dataIsFilePath = true;
  m_data = _filePath;
  m_length = st.st_size;
}

/**
 * @fn std::string to_str() const;
 * @brief Return the complete HTTP content as a string.
 */
std::string content::to_str() const
{
  if ( this->is_string() )
    return m_data;

  // Read the contents of the file and return it as a string
  return std::string();
}

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
void content::append(const std::string& _data, size_t _pos/* = 0*/, size_t _len/* = std::string::npos*/)
{
  if ( this->is_string() )
  {
    m_data.append(_data, _pos, _len);
    m_length = m_data.length();
    return;
  }
  if ( _pos < _data.length() )
  {
    if ( _len == std::string::npos || _len > (_data.length() - _pos) )
      _len = _data.length() - _pos;
    m_file.seekp(0, std::ios_base::end);
    m_file.write(_data.c_str()+_pos, _len);
    m_length += _len;
  }
}
