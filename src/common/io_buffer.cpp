/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file io_buffer.cpp
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
 * @file  io_buffer.cpp
 * @brief Implementation of I/O Buffer that is used for all I/O operations over the network.
 */
#include <common/io_buffer.hpp>
#include <string.h>
#include <algorithm>

using namespace sid;

//! Determine which byte order we're using
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define _IS_LITTLE_ENDIAN_
#elif __BYTE_ORDER == __BIG_ENDIAN
#define _IS_BIG_ENDIAN_
#endif

#include <arpa/inet.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of sid::io_buffer
//

//! Make a clone of the object (Deep copy)
io_buffer io_buffer::clone() const
{
  io_buffer out(this->length());
  out.assign(this->data(), this->length());
  //::memcpy(out.wr_data(), this->data(), this->length());
  return out;
}

uint8_t io_buffer::get_8(const size_t _byte) const
{
  return p_get_8(this->rd_data(_byte));
}
uint16_t io_buffer::get_16(const size_t _byte) const
{
  return p_get_16(this->rd_data(_byte));
}

uint32_t io_buffer::get_24(const size_t _byte) const
{
  return p_get_24(this->rd_data(_byte));
}

uint32_t io_buffer::get_32(const size_t _byte) const
{
  return p_get_32(this->rd_data(_byte));
}

uint64_t io_buffer::get_48(const size_t _byte) const
{
  return p_get_48(this->rd_data(_byte));
}

uint64_t io_buffer::get_64(const size_t _byte) const
{
  return p_get_64(this->rd_data(_byte));
}

std::string io_buffer::get_string(const size_t _byte, const size_t _n) const
{
  return std::string(reinterpret_cast<const char*>(this->rd_data(_byte)), _n);
}

bool io_buffer::get_bool(const size_t _byte, const uint8_t _bitPos) const
{
  return ( get_8(_byte, _bitPos, 1) != 0 );
}

uint8_t io_buffer::get_8(const size_t _byte, const uint8_t _bitStart, const uint8_t _nBits) const
{
  if ( (_bitStart + _nBits) > 8 )
    throw sid::exception("bitStart + nBits cannot be more than 8");

  uint8_t v = get_8(_byte);
  v <<= (8-(_bitStart + _nBits));
  v >>= (8-_nBits);
  return v;
}

/**
 * @brief Implementation of set_x() functions
 */
bool io_buffer::set_8(const size_t _byte, const uint8_t _v)
{
  return p_set_8(this->wr_data(_byte), _v);
}

bool io_buffer::set_16(const size_t _byte, const uint16_t _v)
{
  return p_set_16(this->wr_data(_byte), _v);
}

bool io_buffer::set_24(const size_t _byte, const uint32_t _v)
{
  return p_set_24(this->wr_data(_byte), _v);
}

bool io_buffer::set_32(const size_t _byte, const uint32_t _v)
{
  return p_set_32(this->wr_data(_byte), _v);
}

bool io_buffer::set_48(const size_t _byte, const uint64_t _v)
{
  return p_set_48(this->wr_data(_byte), _v);
}

bool io_buffer::set_64(const size_t _byte, const uint64_t _v)
{
  return p_set_64(this->wr_data(_byte), _v);
}

bool io_buffer::set_string(const size_t _byte, const std::string& _v)
{
  ::memcpy(this->wr_data(_byte), _v.c_str(), _v.length());
  return true;
}

bool io_buffer::set_string(const size_t _byte, const char* _v, size_t _nBytes)
{
  if ( _nBytes == std::string::npos )
    _nBytes = _v? ::strlen(_v) : 0;
  if ( _nBytes > 0 )
  {
    if ( _v )
      ::memcpy(this->wr_data(_byte), _v, _nBytes);
    else
      ::memset(this->wr_data(_byte), 0, _nBytes);
  }
  return true;
}

bool io_buffer::set_bool(const size_t _byte, const uint8_t _bitPos, bool _v)
{
  return set_8(_byte, _bitPos, 1, (_v? 1 : 0));
}

bool io_buffer::set_8(const size_t _byte, const uint8_t _bitStart, const uint8_t _nBits, uint8_t _v)
{
  if ( (_bitStart + _nBits) > 8 )
    throw sid::exception("bitStart + nBits cannot be more than 8");

  _v <<= (8-_nBits);
  _v >>= (8-(_bitStart + _nBits));
  *this->wr_data(_byte) |= _v;
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of private functions in sid::io_buffer
//
uint8_t io_buffer::p_get_8(const uchar8_p _p) const
{
  return *((uint8_t*)_p);
}

uint16_t io_buffer::p_get_16(const uchar8_p _p) const
{
  return ntohs(*((uint16_t*)_p));
}

uint32_t io_buffer::p_get_24(const uchar8_p _p) const
{
#if defined(_IS_LITTLE_ENDIAN_)
  return (((uint32_t) p_get_8(_p)) << 16) | p_get_16(_p+1);
#elif defined(_IS_BIG_ENDIAN_)
  return (((uint32_t) p_get_16(_p+1)) << 8) | p_get_8(_p);
#endif
}

uint32_t io_buffer::p_get_32(const uchar8_p _p) const
{
  return ntohl(*((uint32_t*)_p));
}

uint64_t io_buffer::p_get_48(const uchar8_p _p) const
{
#if defined(_IS_LITTLE_ENDIAN_)
  return (((uint64_t) p_get_16(_p)) << 32) | p_get_32(_p+2);
#elif defined(_IS_BIG_ENDIAN_)
  return (((uint64_t) p_get_32(_p+2)) << 16) | p_get_16(_p);
#endif
}

uint64_t io_buffer::p_get_64(const uchar8_p _p) const
{
#if defined(_IS_LITTLE_ENDIAN_)
  return (((uint64_t) p_get_32(_p)) << 32) | p_get_32(_p+4);
#elif defined(_IS_BIG_ENDIAN_)
  return (((uint64_t) p_get_32(_p+4)) << 32) | p_get_32(_p);
#endif
}

bool io_buffer::p_set_8(uchar8_p _p, const uint8_t _v)
{
  *_p = _v;
  return true;
}

bool io_buffer::p_set_16(uchar8_p _p, const uint16_t _v)
{
  *((uint16_t*)_p) = ::htons(_v);
  return true;
}

bool io_buffer::p_set_24(uchar8_p _p, const uint32_t _v)
{
#if defined(_IS_LITTLE_ENDIAN_)
  p_set_8(_p, (uint8_t) ((_v >> 16) & 0xFF));
  p_set_16(_p+1, (uint16_t) (_v & 0xFFFF));
  return true;
#elif defined(_IS_BIG_ENDIAN_)
  p_set_16(_p, (uint16_t) (_v & 0xFFFF));
  p_set_8(_p+2(uint8_t) ((_v >> 16) & 0xFF));
  return true;
#endif
}

bool io_buffer::p_set_32(uchar8_p _p, const uint32_t _v)
{
  *((uint32_t*)_p) = ::htonl(_v);
  return true;
}

bool io_buffer::p_set_48(uchar8_p _p, const uint64_t _v)
{
#if defined(_IS_LITTLE_ENDIAN_)
  p_set_16(_p, (uint16_t) ((_v >> 32) & 0xFFFF));
  p_set_32(_p+2, (uint32_t) (_v & 0xFFFFFFFF));
  return true;
#elif defined(_IS_BIG_ENDIAN_)
  p_set_32(_p, (uint32_t) (_v & 0xFFFFFFFF));
  p_set_16(_p+4, (uint16_t) ((_v >> 32) & 0xFFFF));
  return true;
#endif
}

bool io_buffer::p_set_64(uchar8_p _p, const uint64_t _v)
{
#if defined(_IS_LITTLE_ENDIAN_)
  p_set_32(_p, (uint32_t) ((_v >> 32) & 0xFFFFFFFF));
  p_set_32(_p+4, (uint32_t) (_v & 0xFFFFFFFF));
  return true;
#elif defined(_IS_BIG_ENDIAN_)
  p_set_32(_p, (uint32_t) (_v & 0xFFFFFFFF));
  p_set_32(_p+4(uint32_t) ((_v >> 32) & 0xFFFFFFFF));
  return true;
#endif
}
