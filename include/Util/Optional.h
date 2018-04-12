/*
LICENSE: BEGIN
===============================================================================
@author Shanmuga (Anand) Gunasekaran
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file  Optional.h
@brief Template class for having optional parameters
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
 * @file  Optional.h
 * @brief Template class for handling optional parameters
 */
#ifndef _GRATIS_OPTIONAL_H_
#define _GRATIS_OPTIONAL_H_

 namespace Gratis {

/**
 * @class Optional
 * @brief Template class for having optional parameters in a structure or in a function
 */
template <class T> struct Optional
{
private:
  T    m_value;
  bool m_exists;
public:
  //! Default constructor
  Optional() { clear(); }
  //! Copy constructor
  Optional(const Optional& obj) { *this = obj; }
  //! Constructor with template type
  Optional(const T& v) { *this = v; }

  //! Set the object exists
  void set() { m_exists = true; }
  //! Clear the object
  void clear() { m_exists = false; }
  //! Clear the object with a given value
  void clear(const T& v) { m_exists = false; m_value = v; }
  //! Assignment operators
  const Optional& operator=(const Optional& obj) { m_value = obj.m_value; m_exists = obj.m_exists; return *this; }
  const Optional& operator=(const T& v) { m_value = v; m_exists = true; return *this; }
  //! Value getter
  const T& operator()() const { return m_value; }
  T& operator()() { return m_value; }
  const T& operator()(const T& defaultIfNotExists) const { return m_exists? m_value : defaultIfNotExists; }
  //! Check for existence
  bool exists(T* p = nullptr) const { if ( m_exists && p ) { *p = m_value;} return m_exists; }
};

} // namespace Gratis

#endif // _GRATIS_OPTIONAL_H_
