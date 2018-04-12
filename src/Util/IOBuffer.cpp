/*
LICENSE: BEGIN
===============================================================================
@author Shanmuga (Anand) Gunasekaran
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file IOBuffer.cpp
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
 * @file  IOBuffer.cpp
 * @brief Implementation of I/O Buffer that is used for all I/O operations over the network.
 */
#include <Util/IOBuffer.h>
#include <string.h>
#include <algorithm>

using namespace Gratis::Util;

//! Determine which byte order we're using
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define _IS_LITTLE_ENDIAN_
#elif __BYTE_ORDER == __BIG_ENDIAN
#define _IS_BIG_ENDIAN_
#endif

#include <arpa/inet.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of Util::IOBuffer_t
//

//! Make a clone of the object (Deep copy)
IOBuffer_t IOBuffer_t::clone() const
{
  IOBuffer_t out(this->length());
  out.assign(this->data(), this->length());
  //memcpy(out.wr_data(), this->data(), this->length());
  return out;
}

uint8_t IOBuffer_t::get_8(const size_t byte) const
{
  return p_get_8(this->rd_data(byte));
}
uint16_t IOBuffer_t::get_16(const size_t byte) const
{
  return p_get_16(this->rd_data(byte));
}

uint32_t IOBuffer_t::get_24(const size_t byte) const
{
  return p_get_24(this->rd_data(byte));
}

uint32_t IOBuffer_t::get_32(const size_t byte) const
{
  return p_get_32(this->rd_data(byte));
}

uint64_t IOBuffer_t::get_48(const size_t byte) const
{
  return p_get_48(this->rd_data(byte));
}

uint64_t IOBuffer_t::get_64(const size_t byte) const
{
  return p_get_64(this->rd_data(byte));
}

std::string IOBuffer_t::get_string(const size_t byte, const size_t n) const
{
  return std::string(reinterpret_cast<const char*>(this->rd_data(byte)), n);
}

bool IOBuffer_t::get_bool(const size_t byte, const uint8_t bitPos) const
{
  return ( get_8(byte, bitPos, 1) != 0 );
}

uint8_t IOBuffer_t::get_8(const size_t byte, const uint8_t bitStart, const uint8_t nBits) const
{
  if ( (bitStart + nBits) > 8 )
    throw std::string("bitStart + nBits cannot be more than 8");

  uint8_t v = get_8(byte);
  v <<= (8-(bitStart + nBits));
  v >>= (8-nBits);
  return v;
}

/**
 * @brief Implementation of set_x() functions
 */
bool IOBuffer_t::set_8(const size_t byte, const uint8_t v)
{
  return p_set_8(this->wr_data(byte), v);
}

bool IOBuffer_t::set_16(const size_t byte, const uint16_t v)
{
  return p_set_16(this->wr_data(byte), v);
}

bool IOBuffer_t::set_24(const size_t byte, const uint32_t v)
{
  return p_set_24(this->wr_data(byte), v);
}

bool IOBuffer_t::set_32(const size_t byte, const uint32_t v)
{
  return p_set_32(this->wr_data(byte), v);
}

bool IOBuffer_t::set_48(const size_t byte, const uint64_t v)
{
  return p_set_48(this->wr_data(byte), v);
}

bool IOBuffer_t::set_64(const size_t byte, const uint64_t v)
{
  return p_set_64(this->wr_data(byte), v);
}

bool IOBuffer_t::set_string(const size_t byte, const std::string& v)
{
  memcpy(this->wr_data(byte), v.c_str(), v.length());
  return true;
}

bool IOBuffer_t::set_string(const size_t byte, const char* v, size_t nBytes)
{
  if ( nBytes == std::string::npos )
    nBytes = v? strlen(v) : 0;
  if ( nBytes > 0 )
  {
    if ( v )
      memcpy(this->wr_data(byte), v, nBytes);
    else
      memset(this->wr_data(byte), 0, nBytes);
  }
  return true;
}

bool IOBuffer_t::set_bool(const size_t byte, const uint8_t bitPos, bool v)
{
  return set_8(byte, bitPos, 1, (v? 1 : 0));
}

bool IOBuffer_t::set_8(const size_t byte, const uint8_t bitStart, const uint8_t nBits, uint8_t v)
{
  if ( (bitStart + nBits) > 8 )
    throw std::string("bitStart + nBits cannot be more than 8");

  v <<= (8-nBits);
  v >>= (8-(bitStart + nBits));
  *this->wr_data(byte) |= v;
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of private functions in Util::IOBuffer_t
//
uint8_t IOBuffer_t::p_get_8(const uchar8_p p) const
{
  return *((uint8_t*)p);
}

uint16_t IOBuffer_t::p_get_16(const uchar8_p p) const
{
  return ntohs(*((uint16_t*)p));
}

uint32_t IOBuffer_t::p_get_24(const uchar8_p p) const
{
#if defined(_IS_LITTLE_ENDIAN_)
  return (((uint32_t) p_get_8(p)) << 16) | p_get_16(p+1);
#elif defined(_IS_BIG_ENDIAN_)
  return (((uint32_t) p_get_16(p+1)) << 8) | p_get_8(p);
#endif
}

uint32_t IOBuffer_t::p_get_32(const uchar8_p p) const
{
  return ntohl(*((uint32_t*)p));
}

uint64_t IOBuffer_t::p_get_48(const uchar8_p p) const
{
#if defined(_IS_LITTLE_ENDIAN_)
  return (((uint64_t) p_get_16(p)) << 32) | p_get_32(p+2);
#elif defined(_IS_BIG_ENDIAN_)
  return (((uint64_t) p_get_32(p+2)) << 16) | p_get_16(p);
#endif
}

uint64_t IOBuffer_t::p_get_64(const uchar8_p p) const
{
#if defined(_IS_LITTLE_ENDIAN_)
  return (((uint64_t) p_get_32(p)) << 32) | p_get_32(p+4);
#elif defined(_IS_BIG_ENDIAN_)
  return (((uint64_t) p_get_32(p+4)) << 32) | p_get_32(p);
#endif
}

bool IOBuffer_t::p_set_8(uchar8_p p, const uint8_t v)
{
  *p = v;
  return true;
}

bool IOBuffer_t::p_set_16(uchar8_p p, const uint16_t v)
{
  *((uint16_t*)p) = htons(v);
  return true;
}

bool IOBuffer_t::p_set_24(uchar8_p p, const uint32_t v)
{
#if defined(_IS_LITTLE_ENDIAN_)
  p_set_8(p, (uint8_t) ((v >> 16) & 0xFF));
  p_set_16(p+1, (uint16_t) (v & 0xFFFF));
  return true;
#elif defined(_IS_BIG_ENDIAN_)
  p_set_16(p, (uint16_t) (v & 0xFFFF));
  p_set_8(p+2(uint8_t) ((v >> 16) & 0xFF));
  return true;
#endif
}

bool IOBuffer_t::p_set_32(uchar8_p p, const uint32_t v)
{
  *((uint32_t*)p) = htonl(v);
  return true;
}

bool IOBuffer_t::p_set_48(uchar8_p p, const uint64_t v)
{
#if defined(_IS_LITTLE_ENDIAN_)
  p_set_16(p, (uint16_t) ((v >> 32) & 0xFFFF));
  p_set_32(p+2, (uint32_t) (v & 0xFFFFFFFF));
  return true;
#elif defined(_IS_BIG_ENDIAN_)
  p_set_32(p, (uint32_t) (v & 0xFFFFFFFF));
  p_set_16(p+4, (uint16_t) ((v >> 32) & 0xFFFF));
  return true;
#endif
}

bool IOBuffer_t::p_set_64(uchar8_p p, const uint64_t v)
{
#if defined(_IS_LITTLE_ENDIAN_)
  p_set_32(p, (uint32_t) ((v >> 32) & 0xFFFFFFFF));
  p_set_32(p+4, (uint32_t) (v & 0xFFFFFFFF));
  return true;
#elif defined(_IS_BIG_ENDIAN_)
  p_set_32(p, (uint32_t) (v & 0xFFFFFFFF));
  p_set_32(p+4(uint32_t) ((v >> 32) & 0xFFFFFFFF));
  return true;
#endif
}
