/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@brief C++ Wrapper for UUID generation and handling
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
 * @file uuid.hpp
 * @brief uuid class declaration
 *        Handling Univerally Unique Identifier
 */

#ifndef _SID_UUID_H_
#define _SID_UUID_H_

#include <uuid/uuid.h>
#include <string>

namespace sid {

/**
 * @class uuid
 */
class uuid
{
public:
  enum class type { def = 0, time, random };
  enum class case_type { def = 0, lower, upper };

public:
  //! Default constructor
  uuid();

  //! Copy constructor
  uuid(const uuid& _obj);

  //! Virtual destructor
  virtual ~uuid();

  //! Clears the object and makes it empty
  void clear();

  //! Checks whether the object is empty or not
  bool empty() const;

  //! Checks for equality of 2 objects
  bool operator ==(const uuid& _obj) const;

  //! Checks for inequality of 2 objects
  bool operator !=(const uuid& _obj) const;

  //! Copy operator. Copies obj to the current object
  uuid& operator =(const uuid& _obj);

  //! Generates a new UUID
  bool generate();

  //! Generates a new UUID according to the specified type
  bool generate(const uuid::type& _eType);

  //! Parses the given string and sets the UUID
  bool parse(const std::string& _csUuid);

  //! Converts the object to the string according to the case specified in the current locale
  std::string to_str() const;

  //! Converts the object to the string according to the case specified as parameter
  std::string to_str(const uuid::case_type& _eCaseType) const;

private:
  uuid_t m_data;
};

} // sid

#endif // _SID_UUID_H_
