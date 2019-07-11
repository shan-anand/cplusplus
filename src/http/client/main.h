#ifndef _HTTP_MAIN_H_
#define _HTTP_MAIN_H_

#include <vector>
#include <string>
#include <map>
#include <http/http.hpp>

using namespace sid;

#define P_Invalid       "INVALID"
#define P_PREFIX_AWS    "--aws-"
#define P_PREFIX_ATMOS  "--atmos-"
#define P_PREFIX_AZURE  "--azure-"
#define P_PREFIX_GENERIC "<none>"

enum class Class { none, aws, atmos, azure };

#define P_aws_version P_PREFIX_AWS"version"
#define P_aws_bucket  P_PREFIX_AWS"bucket"
#define P_aws_id      P_PREFIX_AWS"id"
#define P_aws_key     P_PREFIX_AWS"key"

#define P_azure_container  P_PREFIX_AZURE"container"
#define P_azure_account    P_PREFIX_AZURE"account"
#define P_azure_key        P_PREFIX_AZURE"key"

#define P_NONE      0
#define P_OPTIONAL  1
#define P_SINGLE    2
#define P_NON_EMPTY 4
#define P_NO_DATA   8

#define REQUIRED_SINGLE   (P_SINGLE)
#define OPTIONAL_SINGLE   (P_OPTIONAL | P_SINGLE)
#define REQUIRED_SINGLE_NON_EMPTY   (REQUIRED_SINGLE | P_NON_EMPTY)
#define OPTIONAL_SINGLE_NON_EMPTY   (OPTIONAL_SINGLE | P_NON_EMPTY)
#define REQUIRED_SINGLE_NO_DATA   (REQUIRED_SINGLE | P_NO_DATA)
#define OPTIONAL_SINGLE_NO_DATA   (OPTIONAL_SINGLE | P_NO_DATA)

#define REQUIRED_MULTIPLE (P_NONE)
#define OPTIONAL_MULTIPLE (P_OPTIONAL | P_NONE)
#define REQUIRED_MULTIPLE_NON_EMPTY (REQUIRED_MULTIPLE | P_NON_EMPTY)
#define OPTIONAL_MULTIPLE_NON_EMPTY (OPTIONAL_MULTIPLE | P_NON_EMPTY)

struct ValidParams
{
  const char* key;
  const char* alias;
  int   id;
  int   flags;
};

union Flags
{
  struct
  {
    int optional : 1;
    int single   : 1;
    int nonEmpty : 1;
    int noData   : 1;
  };
  int value;
};

struct Param
{
  Param() { hasData = false; id = -1; }
  std::string key, value;
  bool hasData;
  int  id;
};

struct PValue
{
  PValue() { hasData = false; id = -1; }
  std::string value;
  bool        hasData;
  int  id;
};

typedef std::vector<PValue> PValues;
typedef std::map<std::string, PValues> KeyValues;
typedef std::map<Class, KeyValues> ClassKeyValues;

#endif // _HTTP_MAIN_H_
