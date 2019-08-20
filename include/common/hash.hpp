/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@brief C++ Wrapper for various hash functions like MD<xxx>, SHA<xxx>
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
 * @file hash.hpp
 * @brief Class implementations of Message digest algorithms
 */

#ifndef _SID_HASH_H_
#define _SID_HASH_H_

#include <string>
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "exception.hpp"

namespace sid {
namespace hash {

void init();

/**
 * @class digest
 * @class Message digest result
 */
class digest
{
public:
  digest() {}
  digest(const digest& _obj) = default;
  digest& operator=(const digest& _obj) = default;

  bool empty() const { return m_data.empty(); }
  void clear() { m_data.clear(); m_type.clear(); }

  const std::string& data() const { return m_data; }
  const std::string& type() const { return m_type; }

  void set(const std::string& _data, const std::string& _type)
    { m_data = _data; m_type = _type; }
  void set(const unsigned char* _pdata, size_t _dataLen, const std::string& _type)
    { m_data.assign((const char*) _pdata, _dataLen); m_type = _type; }


  std::string to_hex_str() const;
  std::string to_base64() const;

private:
  std::string m_data;
  std::string m_type;
};

/**
 * @class md_algorithm
 * @brief Message digest Algorithm
 */
class md_algorithm
{
public:
  md_algorithm(int _nid) : m_nid(_nid) {}

  std::string name() const;

  digest get_hash(const uint8_t* _data, uint64_t _dataLen);
  digest get_hash(const std::string& _data)
    { return get_hash((const uint8_t*) _data.c_str(), _data.length()); }

  digest get_hmac(const uint8_t* _key, uint64_t _keyLen, const uint8_t* _data, uint64_t _dataLen);
  digest get_hmac(const std::string& _key, const std::string& _data)
    { return get_hmac((const uint8_t*) _key.c_str(), _key.length(), (const uint8_t*) _data.c_str(), _data.length()); };

protected:
  int m_nid;
};

#define define_md_algorithm(class_name, nid) \
  class class_name : public md_algorithm \
  { public: class_name() : md_algorithm(nid) {} }

//! @class sha256
define_md_algorithm(sha256, NID_sha256);
//! @class sha1
define_md_algorithm(sha1, NID_sha1);
//! @class md5
define_md_algorithm(md5, NID_md5);

} // namespace hash
} // namespace sid

#endif // _SID_HASH_H_
