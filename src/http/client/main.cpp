#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <fstream>
#include <common/convert.hpp>
#include <common/hash.hpp>

#include "main.h"
#include "aws_auth.h"
#include <cstring>
#include <cstdlib>

using namespace sid;
using namespace sid::http;

using namespace std;

struct CommonParams
{
  CommonParams() : method(http::method_type::get) { ip.v4 = ip.v6 = 0; }
  std::string      url;
  http::method     method;
  http::version    version;
  http::headers    headers;
  std::string      data;
  std::string      infile;
  std::string      outfile;
  std::string      userName;
  std::string      password;
  struct
  {
    uint8_t v4 : 1;
    uint8_t v6 : 1;
  }ip;
  void setUser(const std::string& value)
    {
      size_t pos = value.find(':');
      if ( pos == std::string::npos )
        throw sid::exception("Invalid syntax for user");
      this->userName = trim(value.substr(0, pos));
      this->password = trim(value.substr(pos+1));
      if ( this->userName.empty() || this->password.empty() )
        throw sid::exception("Username or password cannot be empty");
    }
};

struct AWSParams
{
  int         version;
  std::string bucket;
  std::string id;
  std::string key;
  AWSParams() { version = 2; }
};

struct AzureParams
{
  std::string container;
  std::string account;
  std::string key;
};

struct Global
{
  std::string scriptName;
  bool        verbose;
  bool        blocking;
  uint32_t    timeout;
  ClassKeyValues ckv;

  Class        ctype;
  CommonParams http;
  AWSParams    aws;
  AzureParams  azure;

  Global()
    {
      verbose = false;
      blocking = true;
      timeout = 0;
      ctype = Class::none;
    }
} global;

enum ParamType {
  PT_none =  -1,
  PT_class,
  PT_url,
  PT_method,
  PT_version,
  PT_ipv4,
  PT_ipv6,
  PT_header,
  PT_user,
  PT_data,
  PT_infile,
  PT_outfile,
  PT_blocking,
  PT_timeout,
  PT_verbose
};

enum AWSPType {
  PT_aws_version,
  PT_aws_bucket,
  PT_aws_id,
  PT_aws_key
};

enum AzurePType {
    PT_azure_container,
    PT_azure_account,
    PT_azure_key
};

struct ParamCount : std::map<int, int>
{
  bool exists(const int type) const { return this->find(type) != this->end(); }
  int getCount(const int type) const
    {
      ParamCount::const_iterator it = this->find(type);
      return ( it == this->end() )? 0 : it->second;
    }
  int incCount(const int type)
    {
      ParamCount::iterator it = this->find(type);
      if ( it == this->end() )
      {
        (*this)[type] = 0;
        it = this->find(type);
      }
      it->second++;
      return it->second;
    }
};

ValidParams global_valid_params[] =
{
  {"--class",    "-c", PT_class,    OPTIONAL_SINGLE_NON_EMPTY},
  {"--url",      "-l", PT_url,      REQUIRED_SINGLE_NON_EMPTY},
  {"--method",   "-m", PT_method,   OPTIONAL_SINGLE_NON_EMPTY},
  {"--version",  "-v", PT_version,  OPTIONAL_SINGLE_NON_EMPTY},
  {"--ipv4",     "-4", PT_ipv4,     OPTIONAL_SINGLE_NO_DATA},
  {"--ipv6",     "-6", PT_ipv6,     OPTIONAL_SINGLE_NO_DATA},
  {"--header",   "-h", PT_header,   OPTIONAL_MULTIPLE_NON_EMPTY},
  {"--user",     "-u", PT_user,     OPTIONAL_SINGLE_NON_EMPTY},
  {"--data",     "-d", PT_data,     REQUIRED_SINGLE_NON_EMPTY},
  {"--infile",   "-i", PT_infile,   REQUIRED_SINGLE_NON_EMPTY},
  {"--outfile",  "-o", PT_outfile,  REQUIRED_SINGLE_NON_EMPTY},
  {"--blocking", "-b", PT_blocking, OPTIONAL_SINGLE_NON_EMPTY},
  {"--timeout",  "-t", PT_timeout,  OPTIONAL_SINGLE_NON_EMPTY},
  {"--verbose",  "-v", PT_verbose,  OPTIONAL_SINGLE_NO_DATA},
  {NULL,         NULL, PT_none,     0}
};


ValidParams global_aws_params[] =
{
  {P_aws_version, NULL, PT_aws_version, OPTIONAL_SINGLE_NON_EMPTY},
  {P_aws_bucket,  NULL, PT_aws_bucket,  REQUIRED_SINGLE_NON_EMPTY},
  {P_aws_id,      NULL, PT_aws_id,      REQUIRED_SINGLE_NON_EMPTY},
  {P_aws_key,     NULL, PT_aws_key,     REQUIRED_SINGLE_NON_EMPTY},
  {NULL,          NULL, -1,        0}
};

ValidParams global_azure_params[] =
{
  {P_azure_container, NULL, PT_azure_container, REQUIRED_SINGLE_NON_EMPTY},
  {P_azure_account,   NULL, PT_azure_account,   REQUIRED_SINGLE_NON_EMPTY},
  {P_azure_key,       NULL, PT_azure_key,       REQUIRED_SINGLE_NON_EMPTY},
  {NULL,              NULL, -1,        0}
};

struct ClassMap
{
  const char* name;
  Class       type;
};

ClassMap global_class_map[] =
{
  {"aws",   Class::aws},
  {"atmos", Class::atmos},
  {"azure", Class::azure},
  {"none",  Class::none}
};

void showUsage(const std::string& csErr = "")
{
  if ( !csErr.empty() )
    cout << "Error: " << csErr << endl << endl;

  cout << "Usage: " << global.scriptName << " [options] " << endl;
  cout << "  [options]" << endl;
  cout << "    --verbose" << endl;
  cout << "    -l --url=(http|https)://<url>" << endl;
  cout << "    -m --method=GET|POST|PUT|DELETE|HEAD (Optional: Defaults to GET)" << endl;
  cout << "    -v --version=1.1|1.0 (Optional: Defaults to 1.1)" << endl;
  cout << "    -4 --ipv4 (default)" << endl;
  cout << "    -6 --ipv6" << endl;
  cout << "    -h --header=name:value" << endl;
  cout << "    -u --user=<user>:<password>" << endl;
  cout << "    -c --class=(none|aws|atmos|azure) (Optional: Defaults to none)" << endl;
  cout << "    -d --data=..." << endl;
  cout << "    -i --infile=data-input-file" << endl;
  cout << "    -o --outfile=data-output-file" << endl;
  cout << "       --blocking=true|false (Optional: Defaults to true)" << endl;
  cout << "       --timeout=SECONDS (Optional: Defaults to " << DEFAULT_IO_TIMEOUT_SECS << ")" << endl;
  cout << "           Note: --timeout is applicable only for non-blocking mode, when --blocking=false" << endl;
  cout << "  [AWS options]" << endl;
  cout << "       --aws-bucket=<AmazonS3 Bucket Name>" << endl;
  cout << "       --aws-id=<AmazonS3 Access ID>" << endl;
  cout << "       --aws-key=<AmazonS3 Secret Key>" << endl;
  cout << "       --aws-version=(2|4) (Optional: Defaults to 2)" << endl;
  cout << "  [Azure options]" << endl;
  cout << "       --azure-container=<Azure Container Name>" << endl;
  cout << "       --azure-account=<Azure Account Name>" << endl;
  cout << "       --azure-key=<Azure Access Key>" << endl;
  cout << endl;
  exit(-1);
}

Class getClassType(const std::string& className)
{
  for ( size_t i = 0; i < sizeof(global_class_map)/sizeof(global_class_map[0]); i++ )
  {
    if ( className == global_class_map[i].name )
      return global_class_map[i].type;
  }
  throw sid::exception("Invalid class \"" + className + "\"");
}

std::string getClassName(const Class& ctype)
{
  for ( size_t i = 0; i < sizeof(global_class_map)/sizeof(global_class_map[0]); i++ )
  {
    if ( ctype == global_class_map[i].type )
      return std::string(global_class_map[i].name);
  }
  throw sid::exception("Invalid class type encountered");
}

int getParamType(const Param& param, const ValidParams* p_vp, ParamCount& params_count)
{
  ParamType paramType = PT_none;

  for ( ; p_vp->key != NULL; p_vp++ )
  {
    if ( param.key == p_vp->key || (p_vp->alias && param.key == p_vp->alias) )
    {
      paramType = (ParamType) p_vp->id;
      if ( params_count.find(paramType) == params_count.end() )
        params_count[paramType] = 0;

      Flags flags; flags.value = p_vp->flags;
      if ( flags.single && params_count.getCount(paramType) > 1 )
        throw sid::exception(param.key + " cannot be repeated");
      else if ( flags.nonEmpty && (param.hasData && param.value.empty()) )
        throw sid::exception(param.key + " cannot be empty");
      else if ( flags.noData && param.hasData )
        throw sid::exception(param.key + " cannot have data. Specify without '=' sign");

      params_count.incCount(paramType);
      break;
    }
  }

  return paramType;
}

Class getArgClass(const std::string& arg)
{
  if ( ::strncmp(arg.c_str(), "--", 2) != 0 && ::strncmp(arg.c_str(), "--", 1) != 0 )
    throw sid::exception("Expecting a parameter. Encountered: " + arg);
  else if ( ::strncmp(arg.c_str(), P_PREFIX_AWS, ::strlen(P_PREFIX_AWS)) == 0 )
    return Class::aws;
  else if ( ::strncmp(arg.c_str(), P_PREFIX_ATMOS, ::strlen(P_PREFIX_ATMOS)) == 0 )
    return Class::atmos;
  else if ( ::strncmp(arg.c_str(), P_PREFIX_AZURE, ::strlen(P_PREFIX_AZURE)) == 0 )
    return Class::azure;

  return Class::none;
}

void addToClassKeyValue(const Class& ctype, const Param& param)
{
  PValue pval;
  pval.hasData = param.hasData;
  pval.value = param.value;

  global.ckv[ctype][param.key].push_back(pval);
}


void validateAwsParams()
{
  ParamCount params_count;
  Param param;
  std::string csError;

  for ( const auto& k : global.ckv[global.ctype] )
  {
    param.key = k.first;
    for ( const auto& v : k.second )
    {
      param.value = v.value;
      param.hasData = v.hasData;
      switch ( getParamType(param, (const ValidParams*) &global_aws_params, params_count) )
      {
      case PT_none: throw sid::exception("Invalid parameter " + param.key);
      case PT_aws_version:
        if ( ! sid::to_num(param.value, global.aws.version, &csError) )
          throw sid::exception(param.key + ": " + csError);
        if ( global.aws.version != 2 && global.aws.version != 4 )
          throw sid::exception(param.key + ": Invalid version specified");
        break;
      case PT_aws_bucket: global.aws.bucket = param.value; break;
      case PT_aws_id:     global.aws.id = param.value; break;
      case PT_aws_key:    global.aws.key = param.value; break;
      }
    }
  }

  if ( ! params_count.exists(PT_aws_bucket) )
    throw sid::exception("Missing --aws-bucket");
  if ( ! params_count.exists(PT_aws_id) )
    throw sid::exception("Missing --aws-id");
  if ( ! params_count.exists(PT_aws_key) )
    throw sid::exception("Missing --aws-key");
}

void validateAtmosParams()
{
  throw sid::exception("Not implemented");
}

void validateAzureParams()
{
  ParamCount params_count;
  Param param;
  std::string csError;

  for ( const auto& k : global.ckv[global.ctype] )
  {
    param.key = k.first;
    for ( const auto& v : k.second )
    {
      param.value = v.value;
      param.hasData = v.hasData;
      switch ( getParamType(param, (const ValidParams*) &global_azure_params, params_count) )
      {
      case PT_none: throw sid::exception("Invalid parameter " + param.key);
      case PT_azure_container: global.azure.container = param.value; break;
      case PT_azure_account:   global.azure.account = param.value; break;
      case PT_azure_key:       global.azure.key = param.value; break;
      }
    }
  }

  if ( ! params_count.exists(PT_azure_container) )
    throw sid::exception("Missing --azure-container");
  if ( ! params_count.exists(PT_azure_account) )
    throw sid::exception("Missing --azure-account");
  if ( ! params_count.exists(PT_azure_key) )
    throw sid::exception("Missing --azure-key");
}

void validateClassKeyValues(const Class& ctype)
{
  std::set<Class> cset;
  for ( const auto& v : global.ckv )
    cset.insert(v.first);

  cset.erase(ctype);
  if ( ! cset.empty() )
  {
    std::ostringstream out;
    out << "Cannot set parameters for";
    for ( const auto& s : cset )
      out << " \"" << getClassName(s) << "\"";
    out << " when the class type is \"" << getClassName(ctype) << "\"";
    throw sid::exception(out.str());
  }

  switch (ctype)
  {
  case Class::none:   return; // nothing to do
  case Class::aws:    validateAwsParams(); break;
  case Class::atmos:  validateAtmosParams(); break;
  case Class::azure:  validateAzureParams(); break;
  }
  return;
}

void parseCommandLine(std::vector<std::string>& args, std::string& location, http::request& request, http::response& response)
{
  if ( args.empty() )
  {
    showUsage();
    exit(0);
  }
  request.method = http::method_type::get;
  request.version = http::version_id::v11;

  ParamCount params_count;
  
  bool timeoutSet = false;
  std::string timeoutValue;

  for ( size_t i = 0; i < args.size(); i++ )
  {
    Param param;
    std::string arg = args[i];

    Class arg_ctype = getArgClass(arg);
    
    param.id = i;
    size_t pos = arg.find('=');
    if ( pos == std::string::npos )
      param.key = arg;
    else
    {
      param.hasData = true;
      param.key = arg.substr(0, pos);
      param.value = arg.substr(pos+1);
    }

    if ( arg_ctype != Class::none )
      addToClassKeyValue(arg_ctype, param);
    else
    {
      switch ( getParamType(param, (const ValidParams*) &global_valid_params, params_count) )
      {
      case PT_none:     throw sid::exception("Invalid parameter " + param.key);
      case PT_url:      global.http.url = param.value;   break;
      case PT_method:   global.http.method = http::method::get(param.value); break;
      case PT_version:  global.http.version = http::version::get(param.value); break;
      case PT_ipv4:     global.http.ip.v4 = 1; break;
      case PT_ipv6:     global.http.ip.v6 = 1; break;
      case PT_header:   global.http.headers.add(param.value); break;
      case PT_data:     global.http.data = param.value; break;
      case PT_user:     global.http.setUser(param.value); break;
      case PT_infile:   global.http.infile = param.value;  break;
      case PT_outfile:  global.http.outfile = param.value; break;
      case PT_class:    global.ctype = getClassType(param.value); break;
      case PT_blocking: global.blocking = sid::to_bool(param.value); break;
      case PT_timeout:  timeoutSet = true; timeoutValue = param.value; break;
      case PT_verbose:  global.verbose = true; http::set_verbose(global.verbose); break;
      }
    }
  }
  if ( global.http.ip.v4 == 0 && global.http.ip.v6 == 0 )
    global.http.ip.v4 = global.http.ip.v6 = 1;

  if ( timeoutSet )
  {
    if ( global.blocking )
      cout << "Warning: --timeout is applicable only for non-blocking mode. Ignoring this parameter." << endl;
    else
    {
      std::string csError;
      if ( !sid::to_num(timeoutValue, global.timeout, &csError) )
        throw sid::exception("--timeout: " + csError);
      if ( global.timeout == 0 )
        throw sid::exception("Timeout value must be greater than 0");
    }
  }

  // validate required and mutually exclusive parameters
  if ( ! params_count.exists(PT_url) )
    throw sid::exception("Missing --url");
  if ( params_count.exists(PT_data) && params_count.exists(PT_infile) )
    throw sid::exception("--data(-d) and --infile(-i) cannot be specified together");

  // validate class-specific parameters
  validateClassKeyValues(global.ctype);

  if ( ! global.http.infile.empty() )
  {
    std::ifstream ifs(global.http.infile.c_str(), std::ifstream::in | std::ifstream::binary);
    if ( ! ifs.is_open() )
      throw sid::exception("Unable to open input file");
    std::ostringstream out;
    out << ifs.rdbuf();
    // read the file contents and put it in content
    global.http.data = out.str();
    ifs.close();
  }
}

void attach_aws_signature(http::headers& headers, const http::method& method, const std::string& resource)
{
  AWS::SignatureInput input;
  AWS::SignatureOutput output;

  // Remove "Authorization" header if it exists
  headers.remove_all("Authorization");

  // Fill the input object
  input.headers = headers;
  input.method = method;
  input.resource = resource;
  input.bucketName = global.aws.bucket;
  input.accessKeyId = global.aws.id;
  input.secret = global.aws.key;
  input.data = reinterpret_cast<const uint8_t*>(global.http.data.c_str());
  input.dataLen = global.http.data.length();

  // Generate the signature
  input.getSignature(output, global.aws.version);

  //if ( global.verbose )
  //  cerr << "Signature calculation" << endl << output.toString() << endl;

  // Replace all the headers from input
  headers.add(input.headers, http::header_action::replace);
  // Attach the signature
  headers("Authorization", output.authStr);
}

void attach_azure_signature(http::headers& headers, const http::method& method, const std::string& resource)
{
}

int makeHttpCall(const std::vector<std::string>& args)
{
  int status = -1;
  std::ostringstream out;
  http::client cmd;
  std::string location;

  try
  {
    cmd.request.method = global.http.method;
    cmd.request.version = http::version_id::v11;

    std::vector<std::string> newArgs = args;
    parseCommandLine(newArgs, location, cmd.request, cmd.response);

    location = global.http.url;
    cmd.request.method = global.http.method;
    cmd.request.version = global.http.version;
    cmd.request.headers.add("Accept: */*");
    cmd.request.headers.add("Accept-Encoding: identity");
    cmd.request.headers.add(global.http.headers, http::header_action::replace);
    if ( global.ctype == Class::none )
    {
      cmd.request.userName = global.http.userName;
      cmd.request.password = global.http.password;
    }
    if ( (cmd.request.method == http::method_type::post || cmd.request.method == http::method_type::put ) &&
         ! global.http.data.empty() )
      cmd.request.set_content(global.http.data);

    if ( !global.http.outfile.empty() )
    {
      cmd.response.content.set_file(global.http.outfile, true);
    }
    http::cookies cookies;

    http::url url;
    if ( ! url.set(location) )
      throw sid::exception(url.error);

    /*
    out << (url.type == http::connection_type::http? "http":"https");
    out << " " << url.server ;
    if ( url.port == 0 )
      out << " at default port";
    else
      out << " at port " << url.port;
    out << " on resource " << url.resource << endl;
    */
    cookies = http::cookies::get_session_cookies(url.server);

    http::connection_family family = connection_family::none;
    if ( global.http.ip.v6 && global.http.ip.v4 )
      family = http::connection_family::none;
    else if ( global.http.ip.v6 )
      family = http::connection_family::ip_v6;
    else if ( global.http.ip.v4 )
      family = http::connection_family::ip_v4;
    cmd.conn = http::connection::create(url.type, family);
    if ( ! cmd.conn->open(url.server, url.port) )
      throw sid::exception(cmd.conn->error());

    cmd.conn->set_blocking(global.blocking, global.timeout);

    out << "******* " << cmd.conn->description().to_str() << endl;

    if ( global.verbose )
      cerr << out.str();

    cmd.request.uri = url.resource;
    cmd.request.headers("Host", url.server);

    cookies.add(cmd.request, cmd.conn);

    // A lambda function for redirect callback
    FNRedirectCallback signature_calc = [&](http::request& request)
      {
        switch ( global.ctype )
        {
        case Class::aws:
          attach_aws_signature(request.headers, request.method, request.uri);
          break;
        case Class::azure:
          attach_azure_signature(request.headers, request.method, request.uri);
          break;
        default: break;
        }
      };

    switch ( global.ctype )
    {
    case Class::aws:
      if ( cmd.request.method == http::method_type::put || cmd.request.method == http::method_type::post )
      {
        cmd.request.headers.add("Content-Type", "application/octet-stream");
        cmd.request.headers.add("x-amz-meta-fma-attr", "TEST");
      }
      attach_aws_signature(cmd.request.headers, cmd.request.method, url.resource);
      break;
    case Class::azure:
      attach_azure_signature(cmd.request.headers, cmd.request.method, url.resource);
      break;
    default: break;
    }

    //cmd.response.content_is_file_path = true;
    //cmd.response.content = "/home/gunash/projects/RESTClient/out.html";
    cmd.response.clear();
    if ( ! cmd.run(signature_calc, true) )
      throw cmd.exception();

    if ( global.verbose )
    {
      http::cookies s_cookies = http::cookies::get_session_cookies(url.server);
      if ( ! s_cookies.empty() )
      {
        for ( const http::cookie& cookie : s_cookies )
          cerr << cookie.to_str(false) << endl;
        cerr << endl;
      }
    }

    bool bShowContent = (cmd.response.headers.content_encoding() == http::content_encoding::identity);
    if ( ! global.verbose )
    {
      if ( (int) cmd.response.status.code() >= 400 )
        cout << "Error: " << cmd.response.status.to_str() << endl;
      else
      {
        if ( bShowContent )
          cout << cmd.response.content.to_str() << endl;
        else
          cout << "<COMPRESSED CONTENT NOT DISPLAYED>" << endl;
      }
    }
    else
    {
      if ( bShowContent )
        cout << cmd.response.content.to_str() << endl;
      else
        cout << "<COMPRESSED CONTENT NOT DISPLAYED>" << endl;
    }
    status = 0;
  }
  catch (const sid::exception& e)
  {
    cerr << e.what() << endl;
    //cout << cmd.response.to_str() << endl;
  }
  //http_library_cleanup();
  return status;
}

int main(int argc, char* argv[])
{
  int status = -1;

  try
  {
    sid::hash::init();
    global.scriptName = argv[0];

    std::vector<std::string> args;
    for ( int i = 1; i < argc; i++ )
      args.push_back(argv[i]);

    status = makeHttpCall(args);
    // status = makeHttpCall(args);
    // status = makeHttpCall(args);
    // status = makeHttpCall(args);
  }
  catch (const sid::exception& e)
  {
    cerr << "Error captured in main: " << e.what() << endl;
  }
  catch (...)
  {
    cerr << "Error captured in main: An unhandled exception occurred" << endl;
  }
  return status;
}
