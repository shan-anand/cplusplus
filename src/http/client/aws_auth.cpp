/////////////////////////////////////////////////////////////////////////////////
//
// @file aws_auth.cpp
// @bried Implementation of data structures that are visible only to this folder
//
/////////////////////////////////////////////////////////////////////////////////

#include "aws_auth.h"

#include <fstream>
#include <cstdlib>
#include <set>
#include <common/convert.hpp>
#include <common/hash.hpp>

using namespace std;
using namespace AWS;

/////////////////////////////////////////////////////////////////////////////////
//
// Implementation of SignatureOutput
//
void SignatureOutput::clear()
{
  awsVersion = 0;
  success = false;
  scope.clear();
  canonicalRequest.clear();
  signingKeyHex.clear();
  stringToSign.clear();
  signature.clear();
  authStr.clear();
}

std::string SignatureOutput::toString() const
{
  std::ostringstream out;
  if ( awsVersion == 2 )
  {
    out << "StringToSign: " << endl  << stringToSign << endl
        << "Signature: " << signature << endl
        << "Authenticate: " << authStr;
  }
  else if ( awsVersion == 4 )
  {
    out << "Scope: " << scope << endl
        << "CanonicalRequest: " << endl << canonicalRequest << endl
        << "StringToSign: " << endl << stringToSign << endl
        << "SigningKey_Hex: " << signingKeyHex << endl
        << "Signature: " << signature << endl
        << "Authenticate: " << authStr;
  }
  else
  {
    out << "INVALID AWS VERSION: " << awsVersion;
  }
  return out.str();
}

/////////////////////////////////////////////////////////////////////////////////
//
// Implementation of SignatureInput
//
SignatureInput::SignatureInput()
{
  data = NULL;
  dataLen = 0;
}

void SignatureInput::getSignature(SignatureOutput& out, const int awsApiVersion)
{
  switch ( awsApiVersion )
  {
  case 4:     getSignature_v4(out);   break;
  case 2:     getSignature_v2(out);  break;
  default:
    throw sid::exception("Unsupported awsApiVersion " + sid::to_str(awsApiVersion));
    break;
  }
  cout << out.toString() << endl;
}

std::string SignatureInput::private_fixAndGetHost(std::string* hostOnly)
{
  bool updateHost = true;
  std::string hostHeader = headers.get("host");
  size_t pos = hostHeader.find('.');
  if ( pos != std::string::npos )
  {
    std::string first = hostHeader.substr(0, pos);
    if ( first == bucketName )
    {
      updateHost = false;
      if ( hostOnly )
        *hostOnly = hostHeader.substr(pos+1);
    }
  }
  if ( updateHost )
  {
    if ( hostOnly )
      *hostOnly = hostHeader;
    headers("Host", bucketName + "." + hostHeader, http::header_action::replace);
  }
  hostHeader = headers.get("host");
  return hostHeader;
}

void SignatureInput::getSignature_v2(SignatureOutput& out)
{
   /**
     CanonicalizedResource = [ "/" + Bucket ] +
                             <HTTP-Request-URI, from the protocol name up to the query string> +
                             [ subresource, if present. For example "?acl", "?location", "?logging", or "?torrent"];

     CanonicalizedAmzHeaders = <described below>

     StringToSign = HTTP-Verb + "\n" +
                    Content-MD5 + "\n" +
                    Content-Type + "\n" +
                    Date + "\n" +
                    CanonicalizedAmzHeaders +
                    CanonicalizedResource;

     Signature = Base64( HMAC-SHA1( YourSecretAccessKeyID, UTF-8-Encoding-Of( StringToSign ) ) );

     Authorization = "AWS" + " " + AWSAccessKeyId + ":" + Signature;
   */

  bool bStatus = false;
  std::string stringToSign;
  std::string authStr;
  CanonicalData canonicalData;
  std::string signature;
  std::string content_md5, content_type, timeStamp;
  http::headers::const_iterator it;
  NormalizedHeaders normalizedHeaders;

  try
  {
    std::string hostHeader = private_fixAndGetHost();
    content_md5 = headers.get("content-md5");
    content_type = headers.get("content-type");
    timeStamp = headers.get("date");
    if ( timeStamp.empty() )
    {
      timeStamp = local::HttpDate_toString(time(NULL));
      headers("Date", timeStamp);
    }

    getCanonicalData(2, canonicalData);

    stringToSign = method.to_str() + "\n" +
                   content_md5 + "\n" +
                   content_type + "\n" +
                   timeStamp + "\n" +
                   canonicalData.headers +
                   canonicalData.uri;

    ///////////////////////////////////////////////////
    // Generate the signature
    // variables: signature
    sid::hash::sha1 sha1;
    sid::hash::digest digest = sha1.get_hmac(this->secret, stringToSign);
    if ( digest.empty() ) throw sid::exception("Failed to create the signature");
    signature = digest.to_base64();

    ///////////////////////////////////////////////////
    // Generate the "Authorization" header
    // variables: authStr
    authStr = "AWS " + accessKeyId + ":" + signature;
    bStatus = true;
  }
  catch (const sid::exception& e)
  {
    authStr = "UNABLE TO CALCULATE";
  }
  catch (...)
  {
    authStr = "UNABLE TO CALCULATE";
  }

  out.awsVersion = 2;
  out.success = bStatus;
  out.stringToSign = stringToSign;
  out.signature = signature;
  out.authStr = authStr;
}

void SignatureInput::getSignature_v4(SignatureOutput& out)
{
  /**
     @see: http://docs.aws.amazon.com/AmazonS3/latest/API/sig-v4-header-based-auth.html

     TimeStamp:
     ==========
     It is the UTC time in ISO8601 Format: <YYYYMMDD>T<hhmmss>Z
     Eg: 20151203T000000Z

     Scope:
     ======
     <date>/<aws-region>/<aws-service>/aws4_request
          <date> value is specified using YYYYMMDD format.
          <aws-service> value is s3 when sending request to Amazon S3.
     Eg: 20151203/us-east-1/s3/aws4_request

     Canonical_Headers:
     ==================
     Lowercase(<HeaderName1>)+":"+sid::trim(value)+"\n"
     Lowercase(<HeaderName2>)+":"+sid::trim(value)+"\n"
     Lowercase(<HeaderName3>)+":"+sid::trim(value)+"\n"
     Lowercase(<HeaderName4>)+":"+sid::trim(value)+"\n"

     Signed_Headers:
     ===============
     Lowercase(<HeaderName1>)+";"+Lowercase(<HeaderName2>)+";"+Lowercase(<HeaderName3>)+ ...

     Canonical_Request:
     ==================
     HTTP_Verb + "\n" +
     Canonical_URI + "\n" +
     Canonical_Query_String + "\n" +
     Canonical_Headers + "\n" +
     Signed_Headers + "\n" +
     Hashed_Payload

     String_To_Sign:
     ===============
     "AWS4-HMAC-SHA256" + "\n" +
     <TimeStamp> + "\n" +
     <Scope> + "\n" +
     HEX(SHA256Hash(<Canonical_Request>))

     Signing_Key:
     ============
     DateKey              = HMAC-SHA256("AWS4"+"<SecretAccessKey>", "<YYYYDDMM>")
     DateRegionKey        = HMAC-SHA256(<DateKey>, "<aws-region>")
     DateRegionServiceKey = HMAC-SHA256(<DateRegionKey>, "<aws-service>")
     Signing_Key          = HMAC-SHA256(<DateRegionServiceKey>, "aws4_request")

     Signature:
     ==========
     HMAC-SHA256(<Signing_Key>, <String_To_Sign>)

     Authorization_Header:
     =====================
     "AWS4-HMAC-SHA256 Credential=<your-access-key-id>/<Scope>,SignedHeaders=<Signed_Headers>,Signature=<Signature>
   */

  bool bStatus = false;
  const std::string aws_service = "s3";
  const std::string aws_request = "aws4_request";
  std::string host, aws_region;
  std::string timeStamp, dateStr;
  std::string scope;
  std::string canonicalRequest;
  std::string stringToSign;
  std::string signingKey;
  std::string signature;
  std::string authorizationHeader;
  std::string authStr;
  std::string hashedPayload;
  std::string p_awsHmacKey;
  sid::hash::sha256 sha256;
  sid::hash::digest digest;
  CanonicalData canonicalData;
  std::string objectKey;
  std::string hostOnly;
  std::map<std::string, std::string>::const_iterator it;

  try
  {
    p_awsHmacKey = std::string("AWS4-HMAC-") + sha256.name();
    std::string hostHeader = private_fixAndGetHost(&hostOnly);
    aws_region = AWS::getRegion(hostOnly);

    // Get has value of payload
    hashedPayload = sid::to_lower(sha256.get_hash(data, dataLen).to_hex_str());

    headers("x-amz-content-sha256", hashedPayload);

    timeStamp = headers.get("x-amz-date");
    if ( timeStamp.empty() )
    {
      timeStamp = headers.get("date");
      // HTTP date field exists, convert it to XAMZDate format
      if ( !timeStamp.empty() )
        timeStamp = local::HttpDate_to_XAMZDate(timeStamp);
      // If the timestamp is empty, get the current date
      if ( timeStamp.empty() )
        timeStamp = local::XAMZDate_toString(time(NULL));
      // Add x-amz-date to the header list
      headers("x-amz-date", timeStamp);
    }

    size_t pos = timeStamp.find('T');
    if ( pos != std::string::npos )
      dateStr = timeStamp.substr(0, pos);

    scope = dateStr;
    if ( !aws_region.empty() )
      scope += "/" + aws_region;
    scope += "/" + aws_service + "/" + aws_request;

    ///////////////////////////////////////////////////
    // Generate Canonical Request
    // variables: canonicalRequest
    getCanonicalData(4, canonicalData);

    canonicalRequest = method.to_str() + "\n" +
                       canonicalData.uri + "\n" +
                       canonicalData.queryString + "\n" +
                       canonicalData.headers + "\n" +
                       canonicalData.signedHeaders + "\n" +
                       hashedPayload;

    ///////////////////////////////////////////////////
    // Generate the string to sign
    // variables: stringToSign
    stringToSign = p_awsHmacKey + "\n" +
                   timeStamp + "\n" +
                   scope + "\n" +
                   sid::to_lower(sha256.get_hash(canonicalRequest).to_hex_str());

    ///////////////////////////////////////////////////
    // Generate the signing key
    // variables: signingKey
    digest = sha256.get_hmac("AWS4"+this->secret, dateStr);
    if ( digest.empty() ) throw sid::exception("Failed to create the date key");
    digest = sha256.get_hmac(digest.data(), aws_region);
    if ( digest.empty() ) throw sid::exception("Failed to create the date region key");
    digest = sha256.get_hmac(digest.data(), aws_service);
    if ( digest.empty() ) throw sid::exception("Failed to create the date region service key");
    digest = sha256.get_hmac(digest.data(), aws_request);
    if ( digest.empty() ) throw sid::exception("Failed to create the signing key");
    signingKey = digest.data();

    ///////////////////////////////////////////////////
    // Generate the signature
    // variables: signature
    digest = sha256.get_hmac(signingKey, stringToSign);
    if ( digest.empty() ) throw sid::exception("Failed to create the signature");
    signature = sid::to_lower(digest.to_hex_str());

    ///////////////////////////////////////////////////
    // Generate the "Authorization" header
    // variables: authStr
    authStr = p_awsHmacKey + " " +
      "Credential=" + accessKeyId + "/" + scope + "," +
      "SignedHeaders=" + canonicalData.signedHeaders + "," +
      "Signature=" + signature;

    bStatus = true;
  }
  catch (const sid::exception& e)
  {
    authStr = "UNABLE TO CALCULATE";
  }
  catch (...)
  {
    authStr = "UNABLE TO CALCULATE";
  }

  out.awsVersion = 4;
  out.success = bStatus;
  out.scope = scope;
  out.canonicalRequest = canonicalRequest;
  sid::bytes_to_hex(signingKey, out.signingKeyHex);
  out.stringToSign = stringToSign;
  out.signature = signature;
  out.authStr = authStr;
}


bool SignatureInput::getCanonicalData(int awsVersion, CanonicalData& out)
{
  out.clear();
  std::string tResource = resource;

  do
  {
    if ( awsVersion == 2 )
    {
      std::string bucket_prefix = "/" + bucketName + "/";
      if ( tResource.find(bucket_prefix) == std::string::npos )
      {
        if ( ! tResource.empty() && tResource[0] == '/' )
          tResource = tResource.substr(1);
        tResource = bucket_prefix + tResource;
      }
    }

    string::size_type pos = tResource.find('?');
    if ( pos == string::npos )
    {
      out.uri = tResource;
      break;
    }

    out.uri = tResource.substr(0, pos);
    if ( pos == tResource.length() - 1)
    {
      // the '?' is the last character in the string 
      break;
    }

    // We have query string. Process them
    std::map< std::string, std::vector<std::string> > qsmap;
    size_t p1 = pos+1;
    size_t p2 = 0;
    std::string qs, key, value;

    do
    {
      p2 = tResource.find('&', p1);
      qs = ( p2 == std::string::npos )? tResource.substr(p1) : tResource.substr(p1, p2-p1);
      if ( p2 != std::string::npos )
        p1 = p2+1;

      size_t x1 = qs.find('=');
      if ( x1 == std::string::npos )
      {
        key = sid::trim(qs);
        value.clear();
      }
      else
      {
        key = sid::trim(qs.substr(0, x1));
        value = sid::trim(qs.substr(x1+1));
      }
      qsmap[key].push_back(value);
    }
    while ( p2 != std::string::npos );

    if ( awsVersion == 2 )
    {
      std::set<std::string> sr;
      sr.insert("acl");
      sr.insert("lifecycle");
      sr.insert("location");
      sr.insert("logging");
      sr.insert("notification");
      sr.insert("partNumber");
      sr.insert("policy");
      sr.insert("requestPayment");
      sr.insert("torrent");
      sr.insert("uploadId");
      sr.insert("uploads");
      sr.insert("versionId");
      sr.insert("versioning");
      sr.insert("versions");
      sr.insert("website");

      std::string sub_resource;
      std::map< std::string, std::vector<std::string> > srmap;
      std::map< std::string, std::vector<std::string> >::const_iterator it;
      for ( it = qsmap.begin(); it != qsmap.end(); it++ )
      {
        const std::string& key = it->first;
        if ( sr.end() == sr.find(key) )
          continue;
        const std::vector<std::string>& values = it->second;
        for ( std::vector<std::string>::const_iterator itv = values.begin(); itv != values.end(); itv++ )
        {
          const std::string& value = *itv;
          if ( ! sub_resource.empty() ) sub_resource += "&";
          sub_resource += key;
          if ( ! value.empty() )
          {
            sub_resource += "=";
            sub_resource += value;
          }
        }
      }
      if ( ! sub_resource.empty() )
        out.uri += ("?" + sub_resource);
    }

    // get all the query string values in a sorted fashion
    std::map< std::string, std::vector<std::string> >::const_iterator it;
    for ( it = qsmap.begin(); it != qsmap.end(); it++ )
    {
      const std::string& key = it->first;
      const std::vector<std::string>& values = it->second;

      for ( std::vector<std::string>::const_iterator itv = values.begin(); itv != values.end(); itv++ )
      {
        const std::string& value = *itv;
        if ( ! out.queryString.empty() ) out.queryString += "&";
        out.queryString += local::uriEncode(key, true);
        out.queryString += "=";
        if ( ! value.empty() )
          out.queryString += local::uriEncode(value, true);
      }
    }
  }
  while ( false );

  ///////////////////////////////////////////////////
  // Generate Canonical Headers and Signed Headers
  // variables: headers, signedHeaders
  NormalizedHeaders normalizedHeaders = NormalizedHeaders::get(headers);
  NormalizedHeaders::const_iterator it;

  if ( awsVersion == 4 )
  {
    // Encode the URI
    //out.uri = local::uriEncode(out.uri, false);

    if ( normalizedHeaders.end() != (it = normalizedHeaders.find("x-amz-date")) && 
         normalizedHeaders.end() != (it = normalizedHeaders.find("date")) )
      normalizedHeaders.erase(it);
  }
                                  
  for ( it = normalizedHeaders.begin(); it != normalizedHeaders.end(); it++ )
  {
    std::string tag = it->first;
    std::string val = it->second;
    bool include = false;

    if ( awsVersion == 2 )
    {
      // For AWS version 2 we consider only amz headers
      include = ( tag.find("x-amz-") == 0 );
    }
    else
    {
      /*
        Note: For the purpose of calculating an authorization signature, 
        only the host and any x-amz-* headers are required; however, 
        in order to prevent data tampering, you should consider including 
        all the headers in the signature calculation.
      */
      include = ( tag == "host" || tag.find("x-amz-") == 0 );
    }

    if ( include )
    {
      // TODO: If any duplicates, combine val's with "," no whitespace
      out.headers += tag + ":" + val + "\n";

      if ( !out.signedHeaders.empty() ) out.signedHeaders += ";";
      out.signedHeaders += tag;
    }
  }

  return true;
}

/////////////////////////////////////////////////////////////////////////////////
//
// Implementation of local functions
//
#define DEFAULT_AWS_REGION "us-east-1"

#define HTTP_DATE_FORMAT "%a, %d %b %Y %H:%M:%S %z"
#define HTTP_DATE_FORMAT2 "%a, %d %b %Y %H:%M:%S %Z"
#define XAMZ_DATE_FORMAT "%Y%m%dT%H%M%SZ"

bool local::HttpDate_fromString(const std::string& csDate, struct tm& tm)
{
  memset(&tm, 0, sizeof(tm));
  return ( NULL != (strptime(csDate.c_str(), HTTP_DATE_FORMAT, &tm)) );
}

std::string local::HttpDate_toString(const struct tm& tm)
{
  char szDate[256] = {0};
  size_t len = strftime(szDate, sizeof(szDate)-1, HTTP_DATE_FORMAT, &tm);
  szDate[len] = '\0';
  return std::string(szDate);
}

std::string local::HttpDate_toString(const time_t& tt)
{
  struct tm tm = {0};
  gmtime_r(&tt, /*out*/ &tm);
  return HttpDate_toString(tm);
}

std::string local::XAMZDate_toString(const struct tm& tm)
{
  char szDate[256] = {0};
  size_t len = strftime(szDate, sizeof(szDate)-1, XAMZ_DATE_FORMAT, &tm);
  szDate[len] = '\0';
  return std::string(szDate);
}

std::string local::XAMZDate_toString(const time_t& tt)
{
  struct tm tm = {0};
  gmtime_r(&tt, /*out*/ &tm);
  return XAMZDate_toString(tm);
}

std::string local::HttpDate_to_XAMZDate(const std::string& httpDate)
{
  std::string xamzDate;
  struct tm tm = {0};
  if ( HttpDate_fromString(httpDate, /*out*/ tm) )
    xamzDate = XAMZDate_toString(tm);
  return xamzDate;
}

NormalizedHeaders NormalizedHeaders::get(const http::headers& inHeaders)
{
  NormalizedHeaders inMap;
  for ( const http::header& header : inHeaders )
  {
    std::string key = sid::to_lower(header.key);
    std::string val, value;
    size_t p2, p1 = 0;
    if ( inMap.find(key) != inMap.end() )
      value = inMap[key];
    do
    {
      p2 = header.value.find(p1, ';');
      if ( p2 == std::string::npos )
        val = sid::trim(header.value.substr(p1));
      else
      {
        val = sid::trim(header.value.substr(p1, p2-p1));
        p1 = p2+1;
      }
      if ( ! value.empty() )
        value += ";";
      value += val;
    }
    while ( p2 != std::string::npos );
    inMap[key] = value;
  }

  return inMap;
}

std::string AWS::getRegion(const std::string& host)
{
  std::string aws_region;
  if ( host == "s3.amazonaws.com" || host == "s3-external-1.amazonaws.com" )
    aws_region = DEFAULT_AWS_REGION;
  else if ( host.find("s3.") == 0 || host.find("s3-") == 0 )
  {
    size_t pos = host.find('.', 3); // 3 is the length of "s3." and "s3-"
    if ( pos == std::string::npos )
      aws_region = host.substr(3);
    else
      aws_region = host.substr(3, pos-3);
  }
  else
    aws_region = "region";

  //if ( aws_region.empty() )
  //  aws_region = DEFAULT_AWS_REGION;

  return aws_region;
}

std::string AWS::getApiVersion()
{
  std::string version;
  std::ifstream ifs;
  ifs.open("/opt/rainfinity/filemanagement/conf/.AWS_API_VERSION", std::ifstream::in);
  if ( ifs.is_open() )
  {
    char value[32];
    memset(value, 0, sizeof(value));
    ifs.getline(value, sizeof(value)-1);
    version = sid::trim(value);
  }
  return version;
}

std::string local::uriEncode(const std::string& uri, bool encodeSlash)
{
  std::string encodedUri;

  for ( size_t i = 0; i < uri.length(); i++ )
  {
    const char ch = uri[i];
    if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')
        || (ch >= '0' && ch <= '9')
        || ch == '_' || ch == '-' || ch == '~' || ch == '.'
        || (ch == '/' && !encodeSlash) )
    {
      encodedUri.push_back(ch);
    }
    else
    {
      std::string csHex = sid::to_str((unsigned char) ch, num_base::hex);
      if ( csHex.length() == 1 ) csHex = "0" + csHex;
      encodedUri.append("%"+csHex);
    }
  }

  return encodedUri;
}
