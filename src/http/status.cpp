//////////////////////////////////////////////////////
//
// status.cpp
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

#include "http/status.hpp"
#include <sstream>
#include <stdlib.h>
#include <common/convert.hpp>

using namespace sid;
using namespace sid::http;

struct MapStatus { status_code code; std::string description; };

static MapStatus gMapStatus[] =
  {
    /*100*/ {status_code::Continue, "Continue"},
    /*101*/ {status_code::SwitchingProtocols, "Switching Protocols"},
    /*102*/ {status_code::Processing, "Processing"},

    /*200*/ {status_code::OK, "OK"},
    /*201*/ {status_code::Created, "Created"},
    /*202*/ {status_code::Accepted, "Accepted"},
    /*203*/ {status_code::NonAuthoritativeInformation, "Non-Authoritative Information"},
    /*204*/ {status_code::NoContent, "No Content"},
    /*205*/ {status_code::ResetContent, "Reset Content"},
    /*206*/ {status_code::PartialContent, "Partial Content"},
    /*207*/ {status_code::MultiStatus, "Multi-Status"},
    /*208*/ {status_code::AlreadyReported, "AlreadyReported"},
    /*226*/ {status_code::IMUsed, "IM Used"},

    /*300*/ {status_code::MultipleChoices, "Multiple Choices"},
    /*301*/ {status_code::MovedPermanently, "Moved Permanently"},
    /*302*/ {status_code::Found, "Found"},
    /*303*/ {status_code::SeeOther, "See Other"},
    /*304*/ {status_code::NotModified, "Not Modified"},
    /*305*/ {status_code::UseProxy, "Use Proxy"},
    /*306*/ {status_code::Unused306, "(Unused)"},
    /*307*/ {status_code::TemporaryRedirect, "Temporary Redirect"},
    /*308*/ {status_code::PermanentRedirect, "Permanent Redirect"},

    /*400*/ {status_code::BadRequest, "Bad Request"},
    /*401*/ {status_code::Unauthorized, "Unauthorized"},
    /*402*/ {status_code::PaymentRequired, "Payment Required"},
    /*403*/ {status_code::Forbidden, "Forbidden"},
    /*404*/ {status_code::NotFound, "Not Found"},
    /*405*/ {status_code::MethodNotAllowed, "Method Not Allowed"},
    /*406*/ {status_code::NotAcceptable, "Not Acceptable"},
    /*407*/ {status_code::ProxyAuthenticationRequired, "Proxy Authentication Required"},
    /*408*/ {status_code::RequestTimeout, "Request Timeout"},
    /*409*/ {status_code::Conflict, "Conflict"},
    /*410*/ {status_code::Gone, "Gone"},
    /*411*/ {status_code::LengthRequired, "Length Required"},
    /*412*/ {status_code::PreconditionFailed, "Precondition Failed"},
    /*413*/ {status_code::RequestEntityTooLarge, "Request Entity Too Large"},
    /*414*/ {status_code::RequestURITooLong, "Request-URI Too Long"},
    /*415*/ {status_code::UnsupportedMediaType, "Unsupported Media Type"},
    /*416*/ {status_code::RequestedRangeNotSatisfiable, "Requested Range Not Satisfiable"},
    /*417*/ {status_code::ExpectationFailed, "Expectation Failed"},
    /*418*/ {status_code::ImATeaPot, "I'm a Tea Pot"},
    /*421*/ {status_code::MisdirectedRequest, "Misdirected Request"},
    /*422*/ {status_code::UnprocessableEntity, "Unprocessable Entity"},
    /*423*/ {status_code::Locked, "Locked"},
    /*424*/ {status_code::FailedDependency, "Failed Dependency"},
    /*426*/ {status_code::UpgradeRequired, "Upgrade Required"},
    /*428*/ {status_code::PreconditionRequired, "Precondition Required"},
    /*429*/ {status_code::TooManyRequests, "Too Many Requests"},
    /*431*/ {status_code::RequestHeaderFieldsTooLarge, "Request Header Fields Too Large"},
    /*451*/ {status_code::UnavailableForLegalReasons, "Unavailable For Legal Reasons"},

    /*500*/ {status_code::InternalServerError, "Internal Server Error"},
    /*501*/ {status_code::NotImplemented, "Not Implemented"},
    /*502*/ {status_code::BadGateway, "Bad Gateway"},
    /*503*/ {status_code::ServiceUnavailable, "Service Unavailable"},
    /*504*/ {status_code::GatewayTimeout, "Gateway Timeout"},
    /*505*/ {status_code::HTTPVersionNotSupported, "HTTP Version Not Supported"},
    /*506*/ {status_code::VariantAlsoNegotiates, "Variant Also Negotiates"},
    /*507*/ {status_code::InsufficientStorage, "Insufficient Storage"},
    /*508*/ {status_code::LoopDetected, "Loop Detected"},
    /*510*/ {status_code::NotExtended, "Not Extended"},
    /*511*/ {status_code::NetworkAuthenticationRequired, "Network Authentication Required"}
  };


status::status()
{
  clear();
}

status::status(const status_code& _code)
{
  *this = _code;
}

//! Clear the object so that it can be reused again
void status::clear()
{
  m_code = status_code::OK; // Default the status code to OK
}

http::status& status::operator=(const status_code& _code)
{
  m_code = _code;
  return *this;
}

const std::string& status::message() const
{
  size_t cnt = sizeof(gMapStatus)/sizeof(gMapStatus[0]);
  for ( size_t i = 0; i < cnt; i++ )
    if ( m_code == gMapStatus[i].code ) return gMapStatus[i].description;
  throw sid::exception("Invalid Status code");
}

//! Check whether the current status is a redirect request
bool status::is_redirect(redirect_info* predirectInfo) const
{
  bool status = false;
  redirect_info redirectInfo;

  switch ( m_code )
  {
  case status_code::MovedPermanently:  /*301*/
    status = redirectInfo.set(true /*permanent*/, false /*cannot change method*/);
    break;
  case status_code::PermanentRedirect: /*308*/ // HTTP method should not change
    status = redirectInfo.set(true /*permanent*/, true /*can change method*/);
    break;
  case status_code::Found:             /*302*/
    status = redirectInfo.set(false /*temporary*/, false /*cannot change method*/);
    break;
  case status_code::TemporaryRedirect: /*307*/ // HTTP method should not change
    status = redirectInfo.set(false /*temporary*/, true /*can change method*/);
    break;
  default: break;
  }

  if ( predirectInfo ) *predirectInfo = redirectInfo;
  return status;
}

std::string status::to_str() const
{
  std::ostringstream out;
  out << static_cast<int>(m_code) << " " << this->message();
  return out.str();
}

/*static*/
http::status status::get(const std::string& _input)
{
  http::status status;
  size_t pos = _input.find(' ');
  if ( pos == std::string::npos ) throw sid::exception("Invalid status format");
  std::string codeStr = _input.substr(0, pos);
  int code = sid::to_num<int>(codeStr);
  if ( code == 0 ) throw sid::exception("Invalid HTTP status code");
  status = (status_code) code;

  bool isFound = false;
  size_t cnt = sizeof(gMapStatus)/sizeof(gMapStatus[0]);
  for ( size_t i = 0; i < cnt; i++ )
    if ( status.code() == gMapStatus[i].code ) { isFound = true; break; }

  if ( !isFound ) throw sid::exception(code, std::string("Invalid HTTP status code: ") + codeStr);

  return status;
}
