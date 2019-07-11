/**
 * @file Hash.cpp
 * @brief Class implementations of Message digest algorithms
 */

/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@brief List of common functions in C++
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

#include "common/convert.hpp"
#include "common/hash.hpp"

using namespace std;
using namespace sid::hash;

void sid::hash::init()
{
  OpenSSL_add_all_digests();
}

/////////////////////////////////////////////////////////////////////////////////
//
// Implementation of digest
//
std::string digest::to_hex_str() const
{
  std::string out;
  sid::bytes_to_hex(m_data, out);
  return out;
}

std::string digest::to_base64() const
{
  return sid::base64::encode(m_data);
}

/////////////////////////////////////////////////////////////////////////////////
//
// Implementation of md_algorithm
//
static const uint8_t* uint8_t_empty = (const uint8_t*) "";

std::string md_algorithm::name() const
{
  return std::string(OBJ_nid2sn(m_nid));
}

digest md_algorithm::get_hash(const uint8_t* _data, uint64_t _dataLen) throw (sid::exception)
{
  digest md_digest;

  const EVP_MD* md = EVP_get_digestbynid(m_nid);
  if ( !md )
    throw sid::exception(std::string("Failed to get digest for nid ") + sid::to_str(m_nid) + ". Ensure sid::hash::init() is called before using");

  EVP_MD_CTX* ctx = EVP_MD_CTX_create();
  EVP_MD_CTX_init(ctx);
  if ( 1 == EVP_DigestInit_ex(ctx, md, nullptr) )
  {
    unsigned char out[EVP_MAX_MD_SIZE] = {0};
    unsigned int out_len = EVP_MD_size(md);

    int s1 = EVP_DigestUpdate(ctx, (void*) (_data? _data:uint8_t_empty), _dataLen);
    int s2 = EVP_DigestFinal_ex(ctx, out, &out_len);
    if ( s1 == 1 && s2 == 1 )
      md_digest.set(out, out_len, EVP_MD_name(md));
  }
  EVP_MD_CTX_destroy(ctx);

  return md_digest;
}

digest md_algorithm::get_hmac(const uint8_t* _key, uint64_t _keyLen, const uint8_t* _data, uint64_t _dataLen) throw (sid::exception)
{
  digest md_digest;

  const EVP_MD* md = EVP_get_digestbynid(m_nid);
  if ( !md )
    throw sid::exception(std::string("Failed to get digest for nid ") + sid::to_str(m_nid) + ". Ensure sid::hash::init() is called before using");

  unsigned char out[EVP_MAX_MD_SIZE];
  unsigned int out_len = EVP_MD_size(md);

  unsigned char *result = HMAC(md,
                               (unsigned char*) (_key? _key:uint8_t_empty), _keyLen,
                               (unsigned char*) (_data? _data:uint8_t_empty), _dataLen,
                               out, &out_len);
  if ( result && out_len > 0 )
  {
    std::string type = std::string("HMAC-") + EVP_MD_name(md);
    md_digest.set(out, out_len, type);
  }

  return md_digest;
}
