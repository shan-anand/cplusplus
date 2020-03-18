/*
LICENSE: BEGIN
===============================================================================
@author Shanmuga (Anand) Gunasekaran
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file io_buffer.hpp
@brief Buffer manipulation for I/O. Converts to and from network byte order.
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
 * @file  io_buffer.hpp
 * @brief io_buffer definition. Used for all IO operations over the network.
 */
#ifndef _SID_UTIL_IO_BUFFER_HPP_
#define _SID_UTIL_IO_BUFFER_HPP_

#include <string>
#include "exception.hpp"

//! New type definition unsigned char
using uchar8_t = unsigned char;
using uchar8_p = uchar8_t*;

namespace sid {

/**
 * @struct io_buffer
 * @brief Buffer used for input/output operation
 */
struct io_buffer : public std::basic_string<uchar8_t>
{
  //! Default constructor
  io_buffer() : m_zero_pos(0) {}
  //! Constructor to create a buffer with a pre-allocated size, filled with zeros
  io_buffer(size_t n) : std::basic_string<uchar8_t>(n, 0), m_zero_pos(0) {}

  //! Make a clone of the object (Deep copy)
  io_buffer clone() const;

  void clear() { std::basic_string<uchar8_t>::clear(); m_zero_pos = 0; }

  //! Set the current absolute position treated as 0
  size_t get_zero_pos() const { return m_zero_pos; }

  //! Set the current absolute position, treated as 0 by rd_data() and wr_data() functions. Returns the old position.
  size_t set_zero_pos(const size_t new_pos) { size_t old_pos = m_zero_pos; m_zero_pos = new_pos; return old_pos; }

  //! Get the read buffer's length from the zero'th position
  size_t rd_length() const { return m_zero_pos > this->length()? 0 : (this->length() - m_zero_pos); }

  //! Get the read buffer pointer at starting point
  const uchar8_p rd_data() const { return (const_cast<uchar8_p>(this->data()) + m_zero_pos); }
  /**
   * @brief Get the read buffer pointer at the given byte position.
   *        It does not check for bounds. The caller is reponsible for checking it.
   */
  const uchar8_p rd_data(const size_t byte) const { return rd_data() + byte; }

  //! Get the write buffer's length from the zero'th position
  size_t wr_length() const { return rd_length(); }

  //! Get the write buffer pointer at starting point.
  uchar8_p wr_data() { return &((*this)[m_zero_pos]); }
  /**
   * @brief Get the write buffer pointer at the given byte position.
   *        It does not check for bounds. The caller is reponsible for checking it.
   */
  uchar8_p wr_data(const size_t byte) { return wr_data() + byte; }

  //! Get 8-bit data as uint8_t at the given byte position
  uint8_t get_8(const size_t byte) const;
  //! Get 16-bit data as uint16_t at the given byte position
  uint16_t get_16(const size_t byte) const;
  //! Get 24-bit data as uint32_t at the given byte position
  uint32_t get_24(const size_t byte) const;
  //! Get 32-bit data as uint32_t at the given byte position
  uint32_t get_32(const size_t byte) const;
  //! Get 48-bit data as uint64_t at the given byte position
  uint64_t get_48(const size_t byte) const;
  //! Get 48-bit data as uint64_t at the given byte position
  uint64_t get_64(const size_t byte) const;
  //! Get n-byte string starting from the given byte position
  std::string get_string(const size_t byte, const size_t n) const;
  //! Get 1-bit value as boolen value at the given byte and bit position
  bool get_bool(const size_t byte, const uint8_t bitPos) const;
  //! Get n-bits (1-8 bits) data as uint8_t at the given byte position, with starting bit position and number of bits (n)
  uint8_t get_8(const size_t byte, const uint8_t bitStart, const uint8_t nBits) const;

  //! Set 8-bit data from uint8_t at the given byte position
  bool set_8(const size_t byte, const uint8_t v);
  //! Set 16-bit data from uint16_t at the given byte position
  bool set_16(const size_t byte, const uint16_t v);
  //! Set 24-bit least significant bits data from uint32_t at the given byte position
  bool set_24(const size_t byte, const uint32_t v);
  //! Set 32-bit data from uint32_t at the given byte position
  bool set_32(const size_t byte, const uint32_t v);
  //! Set 48-bit least significant bits data from uint64_t at the given byte position
  bool set_48(const size_t byte, const uint64_t v);
  //! Set 64-bit data from uint64_t at the given byte position
  bool set_64(const size_t byte, const uint64_t v);
  //! Set string starting at the given byte position
  bool set_string(const size_t byte, const std::string& v);
  //! Set n-byte string starting at the given byte position
  bool set_string(const size_t byte, const char* v, size_t nBytes);
  //! Set boolean as a 1-bit value at the given byte and bit position
  bool set_bool(const size_t byte, const uint8_t bitPos, bool v);
  //! Set n-bits (1-8 bits) data in uint8_t at the given byte position, with starting bit position and number of bits (n)
  bool set_8(const size_t byte, const uint8_t bitStart, const uint8_t nBits, uint8_t v);

private:
  // Internal functions to support get and set operations
  uint8_t p_get_8(const uchar8_p p) const;
  uint16_t p_get_16(const uchar8_p p) const;
  uint32_t p_get_24(const uchar8_p p) const;
  uint32_t p_get_32(const uchar8_p p) const;
  uint64_t p_get_48(const uchar8_p p) const;
  uint64_t p_get_64(const uchar8_p p) const;

  bool p_set_8(uchar8_p p, const uint8_t v);
  bool p_set_16(uchar8_p p, const uint16_t v);
  bool p_set_24(uchar8_p p, const uint32_t v);
  bool p_set_32(uchar8_p p, const uint32_t v);
  bool p_set_48(uchar8_p p, const uint64_t v);
  bool p_set_64(uchar8_p p, const uint64_t v);

  //! Current position (Marked as zero for all the above functions)
  size_t m_zero_pos;
};

} // namespace sid

#endif // _SID_UTIL_IO_BUFFER_HPP_
