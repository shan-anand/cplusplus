//////////////////////////////////////////////////////
//
// www_authenticate.cpp
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

#include "http/http.hpp"
#include "common/convert.hpp"
#include "common/hash.hpp"
#include <sstream>

using namespace std;
using namespace sid;
using namespace sid::http;

bool www_authenticate::exists(const std::string& _key, std::string& _value) const
{
  try
  {
    _value = this->info.at(_key);
    return true;
  }
  catch (...) {}
  return false;
}

std::string www_authenticate::get_auth_string(const http::request& _request)
{
  std::ostringstream out;

  if ( this->type == "Basic" )
  {
    out << this->type << " " << sid::base64::encode(_request.userName + ":" + _request.password);
  }
  else if ( this->type == "Digest" )
  {
    std::string realm, nc, nonce, cnonce, opaque, temp;
    enum class Algorithm { none, MD5, MD5_sess };
    enum class QOP { none, auth, auth_int };
    Algorithm alg = Algorithm::none;
    QOP qop = QOP::none;

    exists("realm", /*out*/ realm);
    if ( !exists("nc", /*out*/ nc) || nc.empty() )
      nc = "00000001";
    exists("nonce", /*out*/ nonce);
    if ( !exists("cnonce", /*out*/ cnonce) || cnonce.empty() )
      cnonce = "82973294";
    exists("opaque", /*out*/ opaque);
    if ( exists("algorithm", /*out*/ temp) )
    {
      if ( temp == "MD5-sess" )
        alg = Algorithm::MD5_sess;
      else if ( temp == "MD5" )
        alg = Algorithm::MD5;
    }

    if ( exists("qop", /*out*/ temp) )
    {
      if ( temp == "auth-int" )
        qop = QOP::auth_int;
      else if ( temp == "auth" )
        qop = QOP::auth;
    }

    std::string qopStr;
    switch ( qop )
    {
    case QOP::auth_int: qopStr = "auth-int"; break;
    default: qopStr = "auth"; break;
    }

    sid::hash::md5 md5;
    std::string HA1, HA2, response;
    HA1 = sid::to_lower(md5.get_hash(_request.userName + ":" + realm + ":" + _request.password).to_hex_str());
    if ( alg == Algorithm::MD5_sess )
      HA1 = sid::to_lower(md5.get_hash(HA1 + ":" + nonce + ":" + cnonce).to_hex_str());

    if ( qop == QOP::auth || qop == QOP::none )
      HA2 = sid::to_lower(md5.get_hash(_request.method.to_str() + ":" + _request.uri).to_hex_str());
    else if ( qop == QOP::auth_int )
    {
      std::string contentMD5 = sid::to_lower(md5.get_hash(_request.content()).to_hex_str());
      HA2 = sid::to_lower(md5.get_hash(_request.method.to_str() + ":" + _request.uri + ":" + contentMD5).to_hex_str());
    }

    if ( qop == QOP::auth || qop == QOP::auth_int )
      response = sid::to_lower(md5.get_hash(HA1 + ":" + nonce + ":" + nc + ":" + cnonce + ":" + qopStr + ":" + HA2).to_hex_str());
    else
      response = sid::to_lower(md5.get_hash(HA1 + ":" + nonce + ":" + HA2).to_hex_str());

    out << this->type << " "
        << "username=\"" << _request.userName << "\","
        << "realm=\"" << realm << "\","
        << "nonce=\"" << nonce << "\","
        << "uri=\"" << _request.uri << "\","
        << "qop=\"" << qopStr << "\","
        << "nc=\"" << nc << "\","
        << "cnonce=\"" << cnonce << "\","
        << "response=\"" << response << "\","
        << "opaque=\"" << opaque << "\""
      ;
  }
  std::string authStr = out.str();

  return authStr;
}

void www_authenticate_list::set(const std::string& _wwwAuthStr)
{
  this->clear();
  bool endOfData = false;

  www_authenticate wwwAuth;
  for ( size_t pos = 0; pos != std::string::npos && !endOfData; )
  {
    size_t wpos = _wwwAuthStr.find_first_of(" =", pos);
    if ( wpos == std::string::npos )
    {
      if ( !wwwAuth.empty() )
      {
        this->push_back(wwwAuth);
        wwwAuth.clear();
      }

      wwwAuth.type = sid::trim(_wwwAuthStr.substr(pos));

      if ( !wwwAuth.empty() )
      {
        this->push_back(wwwAuth);
        wwwAuth.clear();
      }
      pos = wpos;
    }
    else if ( _wwwAuthStr[wpos] == ' ' )
    {
      wwwAuth.clear();
      wwwAuth.type = _wwwAuthStr.substr(pos, wpos-pos);
      pos = wpos+1;
    }
    else // if = is present
    {
      if ( wwwAuth.type.empty() )
        throw std::string("Wrong WWW-Authentication: ") + _wwwAuthStr + " at " + sid::to_str(wpos) + ", type not found";

      std::string key = sid::trim(_wwwAuthStr.substr(pos, wpos-pos));
      pos = wpos+1;
      if ( _wwwAuthStr[pos] == ' ' ) ++pos;
      bool hasQuote = ( _wwwAuthStr[pos] == '\"' );
      if ( hasQuote ) ++pos;
      wpos = pos;
      bool properLoop = false;
      bool endQuote = false;
      bool isEscape = false;
      size_t lpos = wpos;

      //cout << "KEY: " << key << ", wpos: " << sid::to_str(wpos) << ((char) _wwwAuthStr[wpos])<< endl;
      for ( char ch = 0; true; wpos++ )
      {
        ch = _wwwAuthStr[wpos];
        endOfData = ( ch == '\0' );
        if ( endQuote )
        {
          if ( ch != ' ' && ch != ',' && !endOfData )
            throw std::string("Wrong WWW-Authentication: ") + _wwwAuthStr + " at " + sid::to_str(wpos);
        }

        if ( !hasQuote )
        {
          if ( ch == ',' || endOfData )
          {
            lpos = wpos - 1;
            properLoop = true;
            break;
          }
        }
        else
        {
          if ( (ch == ',' || endOfData) && endQuote )
          {
            properLoop = true;
            break;
          }
          if ( ch == '\"' && !isEscape )
          {
            lpos = wpos - 1;
            endQuote = true;
            //cout << "endQuote reached" << endl;
          }
        }
        isEscape = ( ch == '\\' );
        if ( endOfData ) break;
      }
      //cout << key << " endQuote: " << sid::to_str(endQuote) << ", properLoop: " << sid::to_str(properLoop) << endl;
      if ( !properLoop )
        throw std::string("Wrong WWW-Authentication: ") + _wwwAuthStr + ", improper loop at " + sid::to_str(wpos);
      std::string value = sid::trim(_wwwAuthStr.substr(pos, lpos-pos+1));
      wwwAuth.info[key] = value;
      pos = wpos+1;
    }
  }
  if ( !wwwAuth.empty() )
  {
    this->push_back(wwwAuth);
    wwwAuth.clear();
  }
}
