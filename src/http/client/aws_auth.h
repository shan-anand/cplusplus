/////////////////////////////////////////////////////////////////////////////////
//
// @file aws_auth.h
// @bried Define data structures that are visible only to this folder
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef _AWS_AUTH_H_
#define _AWS_AUTH_H_

#include <string>
#include <map>
#include <http/http.hpp>

typedef std::map<std::string, std::string> StringMap;

using namespace sid;

class NormalizedHeaders : public StringMap
{
public:
  static NormalizedHeaders get(const http::headers& inHeaders);
};

namespace local
{
  bool HttpDate_fromString(const std::string& csDate, struct tm& tm);
  std::string HttpDate_toString(const struct tm& tm);
  std::string HttpDate_toString(const time_t& tt);
  std::string XAMZDate_toString(const struct tm& tm);
  std::string XAMZDate_toString(const time_t& tt);
  std::string HttpDate_to_XAMZDate(const std::string& httpDate);
  std::string uriEncode(const std::string& uri, bool encodeSlash);
}

namespace AWS
{
  std::string getRegion(const std::string& host);
  std::string getApiVersion();

  struct SignatureOutput
  {
    int         awsVersion;
    bool        success;
    std::string scope;
    std::string canonicalRequest;
    std::string signingKeyHex;
    std::string stringToSign;
    std::string signature;
    std::string authStr;

  public:
    SignatureOutput() { clear(); }
    void clear();
    std::string toString() const;
  };

  struct SignatureInput
  {
    http::headers headers;
    http::method method;
    std::string  resource;
    std::string  OID;
    std::string  bucketName;
    std::string  accessKeyId;
    std::string  secret;
    const uint8_t* data;
    uint64_t       dataLen;

    struct CanonicalData
    {
      std::string uri;
      std::string queryString;
      std::string headers;
      std::string signedHeaders;
      void clear() { uri.clear(); queryString.clear(); headers.clear(); signedHeaders.clear(); }
    };

  public:
    SignatureInput();
    void getSignature(SignatureOutput& out, const int awsApiVersion);
    void getSignature_v2(SignatureOutput& out);
    void getSignature_v2a(SignatureOutput& out);
    void getSignature_v4(SignatureOutput& out);
    bool getCanonicalData(int awsVersion, CanonicalData& out);

  private:
    std::string private_fixAndGetHost(std::string* hostOnly = nullptr);
  };
}

#endif // _AWS_AUTH_H_
