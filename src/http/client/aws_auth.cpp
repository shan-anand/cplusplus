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
  case 2:     getSignature_v2a(out);  break;
  default:
    throw std::string("Unsupported awsApiVersion ") + sid::to_str(awsApiVersion);
    break;
  }
  cout << out.toString() << endl;
  ; // LOG_PRINTF(INFO, "SignatureInput::%s: Signature Output\n%s\n", __func__, out.toString().c_str());
}

void SignatureInput::getSignature_v2(SignatureOutput& out)
{/*
  bool bStatus = false;

    // @see http://docs.aws.amazon.com/AmazonS3/latest/dev/RESTAuthentication.html

    // normalizedHeaders will contain all headers - including "date",
    // "content-type", "range" (if it exists), all "x-amz-*", and other headers;
    // each header is stripped of leading ' ', and trailing ' ' or ':'
  CloudAdapter::Http_headers_t normalizedHeaders = CloudAdapter::getNormalizedHeaders(mapRequest);
  string hashStr;
  hashStr = method.to_str() + "\n";
    CloudAdapter::appendPositionalHeader(hashStr, normalizedHeaders, "content-md5");
    CloudAdapter::appendPositionalHeader(hashStr, normalizedHeaders, "content-type");
    if (normalizedHeaders.find("date") != normalizedHeaders.end())
        CloudAdapter::appendPositionalHeader(hashStr, normalizedHeaders, "date");
    else
    {
        ; // LOG_PRINTF(ERROR, "AmazonS3Adapter::%s OID %s: unable to find 'date' header; failed to attach signature\n", __func__, OID.c_str());
        mapRequest.insert(pair<string,string>("Authorization:","UNABLE TO CALCULATE"));
        return;
    }
    // Add canonicalized headers
    CloudAdapter::Http_headers_iter_t iter;
    for (iter=normalizedHeaders.begin();iter!=normalizedHeaders.end();iter++)
    {
        string tag = iter->first;
        string val = iter->second;
        // TODO: If any duplicates, combine val's with "," no whitespace
        if (tag.find("x-amz-") == 0)
            hashStr = hashStr + tag + ":" + val + string("\n");
    }
    // should always begin with /<bucket_name>/<URI_path> with
    // <URI_path> up to first arg; it should following args and
    // their values, if any, if they match those shown below;
    // the args should be sorted
    // examples:
    //   /mypath?prefix=helloworld    ==> do not append the arg
    //   /mypath?acl                  ==> append '?acl'
    //   /mypath?versionid=2          ==> append '?versionid=2'
    //   /mypath?versionid=2&prefix=2 ==> append '?prefix=2&versionid=2'
    //
    // http://docs.amazonwebservices.com/AmazonS3/2006-03-01/dev/index.html
    
    // split URI into path - up to first '?', if any - first arg, and the rest
    // discard the rest
    string path;
    string::size_type delim = resource.find('?');
    if (delim != string::npos)
    {
        path = resource.substr(0, delim);
        if (delim == resource.length() - 1)
        {
            // the '?' is the last character in the string 
          ; // LOG_PRINTF(WARNING, "AmazonS3Adapter::%s: OID %s resource %s - last char is '?'; unexpected; no args (delim %u)\n", 
                     __func__, OID.c_str(), resource.c_str(), (uint32_t) delim);
        }
    }
    else
    {
        path = resource;
    }
    // only include first arg if it's one of "?versioning", "?location",
    // "?acl", "?torrent", or "?versionid"
    // TODO: these are "subresources'; are there other subresources?
    // The full set of subresources:
    //   acl, location, logging, notification, partNumber, policy,
    //   requestPayment, torrent, uploadId, uploads, versionId,
    //   versioning, versions and website
    // The subresources must be stored.
    // They must include and values.
    // http://docs.amazonwebservices.com/AmazonS3/latest/dev/
    string subresourceArg;
    delim = resource.find('?');
    if (delim != string::npos)
    {
        map<string,string> subresources;
        path = resource.substr(0, delim);
        if (delim == resource.length() - 1)
        {
            // the '?' is the last character in the string 
            ; // LOG_PRINTF(WARNING, "AmazonS3Adapter::%s: OID %s resource %s - last char is '?'; unexpected; no args (delim %u)\n", __func__, OID.c_str(), resource.c_str(), (uint32_t) delim);
        }
        else
        {
            // we're looking for <arg>=<val>[&<arg>=<val>]*
            // start with what's behind the first '?'
            string remaining = resource.substr(delim + 1);
            while (! remaining.empty())
            {
                string arg; string val;
                delim = remaining.find('&');
                if (delim != string::npos)
                {
                    // TODO: bad strings that end with '&'
                    arg = remaining.substr(0, delim);
                    remaining = remaining.substr(delim + 1);
                }
                else 
                {
                    arg = remaining;
                    remaining = "";
                }
                // split arg on '='
                delim = arg.find('=');
                if (delim != string::npos)
                {
                    if (delim == arg.length() - 1)
                    {
                        // the '=' is the last character in the string 
                        ; // LOG_PRINTF(WARNING, "AmazonS3Adapter::%s: OID %s resource %s arg '%s' - last char is '='; unexpected; no value\n", __func__, OID.c_str(), resource.c_str(), arg.c_str());
                    }
                    else
                    {
                        val = arg.substr(delim + 1);
                        arg = arg.substr(0, delim);
                    }
                }
                if (arg == "acl" ||
                    arg == "location" ||
                    arg == "logging" ||
                    arg == "notificcation" || 
                    arg == "partNumber" || 
                    arg == "policy" || 
                    arg == "requestPayment" || 
                    arg == "torrent" ||
                    arg == "uploadId" ||
                    arg == "uploads" ||
                    arg == "versionId" || 
                    arg == "versioning" || 
                    arg == "versions" || 
                    arg == "website")
                {
                    subresources.insert(make_pair(arg, val));
                }
            }
        }

        ostringstream subresourceStr;
        map<string,string>::iterator subIter;
        for (subIter=subresources.begin();subIter!=subresources.end();subIter++)
        {
            if (! subresourceStr.str().empty())
                subresourceStr << "&";
            if (! subIter->second.empty())
                subresourceStr << subIter->first << "=" << subIter->second;
            else
                subresourceStr << subIter->first;
        }
        subresourceArg = subresourceStr.str();
    }
    if (! subresourceArg.empty())
        ; // LOG_PRINTF(DEBUG_2, "AmazonS3Adapter::%s: resource '%s' subresource '%s'\n", __func__, resource.c_str(), subresourceArg.c_str());

    // now construct canonicalized resource
    string canonicalizedResource;

    string leadingBucket = string("/") + bucketName + string("/");
    if (path.find(leadingBucket) != 0)
    {
        // prepend /<bucket_name>
        canonicalizedResource = string("/") + bucketName;
        if (path.find("/") != 0)
            canonicalizedResource = canonicalizedResource + string("/");
        canonicalizedResource = canonicalizedResource + path;
    }
    else
        canonicalizedResource = path;
    if (! subresourceArg.empty())
        canonicalizedResource = canonicalizedResource + string("?") + subresourceArg;

    hashStr = hashStr + canonicalizedResource;
    ; // LOG_PRINTF(DEBUG_4, "AmazonS3Adapter::%s: canonicalized hash string\n%s\n", __func__, hashStr.c_str());

    ; // LOG_PRINTF(DEBUG_4, "AmazonS3Adapter::%s OID %s: StringToSign:\n%s\n", __func__, OID.c_str(), hashStr.c_str());

    // Signing the request
    std::string signature;
    sid::hash::sha1 sha1;
    sid::hash::digest digest = sha1.get_hmac(this->secret, hashStr);
    if ( ! digest.empty() )
    {
      ; // LOG_PRINTF(DEBUG_4, "AmazonS3Adapter::%s: raw digest: '0x%s'\n", __func__, digest.to_hex_str().c_str());

      signature = digest.to_base64();
      ; // LOG_PRINTF(DEBUG_4, "AmazonS3Adapter%s: signature = %s\n", __func__, signature.c_str());
    }
    else
    {
      ; // LOG_PRINTF(ERROR, "AmazonS3Adapter::%s: failed to calculate hash\n", __func__);
      signature = std::string("HMAC error");
    }
    ///////////////////////////////////////////////////
    // Generate the "Authorization" header
    // variables: authStr
    std::string authStr = "AWS " + accessKeyId + ":" + signature;
    bStatus = true;

    out.awsVersion = 2;
    out.success = bStatus;
    out.stringToSign = hashStr;
    out.signature = signature;
    out.authStr = authStr;
 */
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

void SignatureInput::getSignature_v2a(SignatureOutput& out)
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

    ; // LOG_PRINTF(DEBUG_4, "AmazonS3Adapter::%s OID %s: StringToSign:\n%s\n", __func__, OID.c_str(), stringToSign.c_str());

    ///////////////////////////////////////////////////
    // Generate the signature
    // variables: signature
    sid::hash::sha1 sha1;
    sid::hash::digest digest = sha1.get_hmac(this->secret, stringToSign);
    if ( digest.empty() ) throw std::string("Failed to create the signature");
    signature = digest.to_base64();

    ///////////////////////////////////////////////////
    // Generate the "Authorization" header
    // variables: authStr
    authStr = "AWS " + accessKeyId + ":" + signature;
    bStatus = true;
  }
  catch (const std::string& csErr)
  {
    ; // LOG_PRINTF(ERROR, "AmazonS3Adapter::%s OID %s: %s\n", __func__, OID.c_str(), csErr.c_str());
    authStr = "UNABLE TO CALCULATE";
  }
  catch (...)
  {
    ; // LOG_PRINTF(ERROR, "AmazonS3Adapter::%s OID %s: %s\n", __func__, OID.c_str(), "An Unhanlded exception occurred");
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
    ; // LOG_PRINTF(INFO, "Anand-3: Canonical_Request=\n%s\n\nString_To_Sign=\n%s\n", canonicalRequest.c_str(), stringToSign.c_str());
    digest = sha256.get_hmac("AWS4"+this->secret, dateStr);
    if ( digest.empty() ) throw std::string("Failed to create the date key");
    digest = sha256.get_hmac(digest.data(), aws_region);
    if ( digest.empty() ) throw std::string("Failed to create the date region key");
    digest = sha256.get_hmac(digest.data(), aws_service);
    if ( digest.empty() ) throw std::string("Failed to create the date region service key");
    digest = sha256.get_hmac(digest.data(), aws_request);
    if ( digest.empty() ) throw std::string("Failed to create the signing key");
    signingKey = digest.data();

    ///////////////////////////////////////////////////
    // Generate the signature
    // variables: signature
    digest = sha256.get_hmac(signingKey, stringToSign);
    if ( digest.empty() ) throw std::string("Failed to create the signature");
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
  catch (const std::string& csErr)
  {
    ; // LOG_PRINTF(ERROR, "AmazonS3Adapter::%s OID %s: %s\n", __func__, OID.c_str(), csErr.c_str());
    authStr = "UNABLE TO CALCULATE";
  }
  catch (...)
  {
    ; // LOG_PRINTF(ERROR, "AmazonS3Adapter::%s OID %s: %s\n", __func__, OID.c_str(), "An Unhanlded exception occurred");
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
      ; // LOG_PRINTF(WARNING, "AmazonS3Adapter::%s: resource %s - last char is '?'; unexpected; no args (delim %u)\n", __func__, tResource.c_str(), (uint32_t) pos);
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
