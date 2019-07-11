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

/**
 * @file status.hpp
 * @brief Defines all the return HTTP status codes.
 *
 * A class that defines all the return HTTP status codes like OK, Continue, NotFound etc.
 */
#ifndef _SID_HTTP_STATUS_CODE_H_
#define _SID_HTTP_STATUS_CODE_H_

#include <string>
#include <common/exception.hpp>

namespace sid {
namespace http {

//! Return HTTP status IDs
enum class status_code : uint16_t
{
  Continue = 100,
  SwitchingProtocols = 101,
  Processing = 102,

  OK = 200,
  Created = 201,
  Accepted = 202,
  NonAuthoritativeInformation = 203,
  NoContent = 204,
  ResetContent = 205,
  PartialContent = 206,
  MultiStatus = 207,
  AlreadyReported = 208,
  IMUsed = 226,

  MultipleChoices = 300,
  MovedPermanently = 301,
  Found = 302,
  SeeOther = 303,
  NotModified = 304,
  UseProxy = 305,
  Unused306 = 306,
  TemporaryRedirect = 307,
  PermanentRedirect = 308,

  BadRequest = 400,
  Unauthorized = 401,
  PaymentRequired = 402,
  Forbidden = 403,
  NotFound = 404,
  MethodNotAllowed = 405,
  NotAcceptable = 406,
  ProxyAuthenticationRequired = 407,
  RequestTimeout = 408,
  Conflict = 409,
  Gone = 410,
  LengthRequired = 411,
  PreconditionFailed = 412,
  RequestEntityTooLarge = 413,
  RequestURITooLong = 414,
  UnsupportedMediaType = 415,
  RequestedRangeNotSatisfiable = 416,
  ExpectationFailed = 417,
  ImATeaPot= 418,
  MisdirectedRequest = 421,
  UnprocessableEntity = 422,
  Locked = 423,
  FailedDependency = 424,
  UpgradeRequired = 426,
  PreconditionRequired = 428,
  TooManyRequests = 429,
  RequestHeaderFieldsTooLarge = 431,
  UnavailableForLegalReasons = 451,

  InternalServerError = 500,
  NotImplemented = 501,
  BadGateway = 502,
  ServiceUnavailable = 503,
  GatewayTimeout = 504,
  HTTPVersionNotSupported = 505,
  VariantAlsoNegotiates = 506,
  InsufficientStorage = 507,
  LoopDetected = 508,
  NotExtended = 510,
  NetworkAuthenticationRequired = 511,

  /* UNOFFICIAL CODES */
  // RESTful third-party service codes
  Checkpoint = 103,
  MethodFailure = 420,
  EnhanceYourCalm = 420,
  BlockedbyWindowsParentalControls = 450,
  InvalidToken = 498,
  TokenRequired = 499,
  Requesthasbeenforbiddenbyantivirus = 499,
  BandwidthLimitExceeded = 509,
  Siteisfrozen = 530,

  // Internet Information Services
  LoginTimeout = 440,
  RetryWith = 449,
  Redirect = 451,

  // nginx
  NoResponse = 444,
  SSLCertificateError = 495,
  SSLCertificateRequired = 496,
  HTTPRequestSenttoHTTPSPort = 497,
  ClientClosedRequest = 499
};

/**
 * @class status
 * @brief Definition for all return HTTP status codes.
 */
class status
{
public:
  struct redirect_info
  {
    bool isPermanent;
    bool canChangeMethod;
    redirect_info() : isPermanent(false), canChangeMethod(false) {}
    redirect_info(bool _isPermanent, bool _canChangeMethod) : isPermanent(_isPermanent), canChangeMethod(_canChangeMethod) {}
    bool set(bool _isPermanent, bool _canChangeMethod) { isPermanent = _isPermanent; canChangeMethod = _canChangeMethod; return true; }
  };
public:
  //! Get the status code
  const status_code& code() const { return m_code; }

  //! Get/Set the status code
  status_code& code() { return m_code; }

  //! Return the complete message (<id> <message>)
  std::string to_str() const;

  //! Get the string name of the status object
  const std::string& message() const;

  //! Check whether the current status is a redirect request
  bool is_redirect(redirect_info* predirectInfo = nullptr) const;

  //! Get the status object using the given input.
  static status get(const std::string& input);

public:
  //! Default constructor
  status();

  //! Copy constructor
  status(const status&) = default;

  //! Constructor using status code
  status(const status_code& _code);

  //! Clear the object so that it can be reused again
  void clear();

  //! Copy operators
  status& operator=(const status&) = default;
  status& operator=(const status_code& _code);

private:
  status_code m_code; //! status code
};

} // namespace http
} // namespace sid

#endif // _SID_HTTP_STATUS_CODE_H_
