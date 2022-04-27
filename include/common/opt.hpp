/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
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
 * @file  opt.hpp
 * @brief Template class for handling optional parameters
 */

#ifndef _SID_OPT_HPP_
#define _SID_OPT_HPP_

namespace sid {

/**
 * @class opt
 * @brief Template class for having optional parameters in a structure or in a function
 */
template <typename T> struct opt
{
private:
  T    m_value;
  bool m_exists;
public:
  //! Default constructor
  opt() { clear(); }
  //! Copy constructor
  opt(const opt& _obj) { *this = _obj; }
  //! Constructor with template type
  opt(const T& _v) { *this = _v; }

  //! Set the object exists
  void set() { m_exists = true; }
  //! Clear the object
  void clear() { m_exists = false; }
  //! Clear the object with a given value
  void clear(const T& _v) { m_exists = false; m_value = _v; }
  //! Assignment operators
  const opt& operator=(const opt& _obj) { m_value = _obj.m_value; m_exists = _obj.m_exists; return *this; }
  const opt& operator=(const T& _v) { m_value = _v; m_exists = true; return *this; }
  //! Value getter
  const T& operator()() const { return m_value; }
  T& operator()() { return m_value; }
  //! Check for exists
  operator bool() const { return m_exists; }
  //! Return the default value if it doesn't exist
  const T& operator()(const T& _defaultIfNotExists) const { return m_exists? m_value : _defaultIfNotExists; }
  //! Check for existence
  bool exists(T* _p = nullptr) const { if ( m_exists && _p ) { *_p = m_value;} return m_exists; }
};

} // namespace sid

#endif // _SID_OPT_HPP_
