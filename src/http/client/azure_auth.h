/////////////////////////////////////////////////////////////////////////////////
//
// @file azure_auth.h
// @bried Define data structures that are visible only to this folder
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef _AZURE_AUTH_H_
#define _AZURE_AUTH_H_

#include <string>
#include <map>
#include <Http/Http.h>

#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

typedef std::map<std::string, std::string> StringMap;

using namespace Gratis;

class NormalizedHeaders : public StringMap
{
public:
  static NormalizedHeaders get(const Http::HeaderList& inHeaders);
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

namespace Azure
{
  std::string getRegion(const std::string& host);
  std::string getApiVersion();

  struct SignatureOutput
  {
    int         azureVersion;
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
    Http::HeaderList headerList;
    Http::Method method;
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
    void getSignature(SignatureOutput& out, const int azureApiVersion);
    void getSignature_v2(SignatureOutput& out);
    void getSignature_v2a(SignatureOutput& out);
    void getSignature_v4(SignatureOutput& out);
    bool getCanonicalData(int azureVersion, CanonicalData& out);

  private:
    std::string private_fixAndGetHost(std::string* hostOnly = nullptr);
  };
}

struct Digest
{
  Digest() {}
  Digest(const Digest& obj) { *this = obj; }
  Digest& operator=(const Digest& obj) { m_data = obj.m_data; m_type = obj.m_type; return *this; }

  bool empty() const { return m_data.empty(); }
  void clear() { m_data.clear(); m_type.clear(); }

  const std::string& data() const { return m_data; }
  const std::string& type() const { return m_type; }

  void set(const std::string& data, const std::string& type)
    { m_data = data; m_type = type; }
  void set(const unsigned char* pdata, size_t dataLen, const std::string& type)
    { m_data.assign((const char*) pdata, dataLen); m_type = type; }


  std::string toHexString() const;
  std::string toBase64() const;

private:
  std::string m_data;
  std::string m_type;
};

struct MDAlgorithm
{
  MDAlgorithm(int nid) : m_nid(nid) {}

  std::string name() const;

  Digest getHash(const uint8_t* data, uint64_t dataLen);
  Digest getHash(const std::string& data)
    { return getHash((const uint8_t*) data.c_str(), data.length()); }

  Digest getHMAC(const uint8_t* key, uint64_t keyLen, const uint8_t* data, uint64_t dataLen);
  Digest getHMAC(const std::string& key, const std::string& data)
    { return getHMAC((const uint8_t*) key.c_str(), key.length(), (const uint8_t*) data.c_str(), data.length()); };

protected:
  int m_nid;
};

#define define_MDAlgorithm(class_name, nid) struct class_name : public MDAlgorithm { class_name() : MDAlgorithm(nid) {} }

#endif // _AZURE_AUTH_H_
