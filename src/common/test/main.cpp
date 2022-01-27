#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <limits>
#include <stdlib.h>
#include "common/optional.hpp"
#include "common/uuid.hpp"
#include "common/json.hpp"
#include "common/convert.hpp"
#include "common/uuid.hpp"
#include "common/regex.hpp"
#include "common/simple_types.hpp"

using namespace std;
using namespace sid;


std::string get_file_contents(const std::string& filePath)
{
  std::ifstream in;
  in.open(filePath);
  if ( ! in.is_open() )
    throw sid::exception("Failed to open file: " + filePath);
  char buf[8096] = {0};
  std::string jsonStr;
  while ( ! in.eof() )
  {
    ::memset(buf, 0, sizeof(buf));
    in.read(buf, sizeof(buf)-1);
    if ( in.bad() )
      throw sid::exception(sid::to_errno_str("Failed to read data"));
    jsonStr += buf;
  }
  return jsonStr;
}

void parser_test(std::string jsonStr)
{
  if ( jsonStr.empty() )
    jsonStr = "{\"key\": \"v\\\"alue1\", \"mname\": null, \"num1\": -34234.23456, \"num2\": 7.012e1, \"numbers\": [100, -100, 12.34, -34.02, -9.223372037e18, 1.844674407e19]}";

  cout << jsonStr << endl;
  json::value jroot;
  json::value::parse(jroot, jsonStr);
  /*
  cout << jroot["key"].as_str() << endl;
  //cout.precision(std::numeric_limits< long double >::max_digits10);
  cout << jroot["num1"].as_str() << endl;
  cout << jroot["num2"].as_str() << endl;
  const json::value& jnumbers = jroot["numbers"];
  for ( size_t i = 0; i < jnumbers.size(); i++ )
    cout << jnumbers[i].as_str() << " ";
  */
  cout << endl<< endl;
  //jroot.write(cout, json::format_type::pretty);
  cout << jroot.to_str(json::format_type::pretty) << endl << endl;
}

template <typename T>
std::string to_type()
{
  /*
  using _num_type = typename std::conditional<
    std::is_same<long double, T>::value, long double, typename std::conditional<
      std::is_same<double, T>::value, double, typename std::conditional<
        std::is_same<float, T>::value, float, typename std::conditional<std::is_unsigned<T>::value, unsigned long long int, long long int>>::type>::type>::type;
  */
  using _num_type = typename std::conditional< std::is_same<long double, T>::value, long double,
		        typename std::conditional< std::is_same<double, T>::value, double,
		            typename std::conditional< std::is_same<float, T>::value, float,
		                typename std::conditional< std::is_unsigned<T>::value, unsigned long long int, long long int >::type
		            >::type
		        >::type
		    >::type;

  //using _num_type = typename std::conditional<std::is_unsigned<T>::value, unsigned long long int, long long int>::type;
  return typeid(_num_type).name();
  if ( std::is_same<long double, T>::value )
    return "is_longdouble";
  else if ( std::is_same<double, T>::value )
    return "is_double";
  else if ( std::is_same<float, T>::value )
    return "is_float";
  else if ( std::is_unsigned<T>::value )
    return "is_unsigned";
  else if ( std::is_signed<T>::value )
    return "is_signed";
  return "unknown";
};

void regex_test1(const std::string& _infoStr)
{
  try
  {
    if ( ::strncmp(_infoStr.c_str(), "iscsi://", 8) != 0 )
      throw sid::exception("Invalid syntax");

    auto get_cred = [&](const std::string& key, const std::string& in)->basic_cred
      {
        basic_cred cred;
        std::string inEx = in;
        if ( in.empty() )
          ; // No chap credentials are given. It's ok. Don't throw an exception
        else if ( in[0] != '#' )
        {
          // <USERNAME>:<PASSWORD>
          if ( ! cred.set(in, ':') )
            throw sid::exception(key + ": Invalid syntax");
        }
        else if ( in.length() > 1 && in[1] != '#' )
        {
          // #BASE64(<USERNAME>:<PASSWORD>)
          inEx = in.substr(1);
          inEx = sid::base64::decode(inEx);
          if ( ! cred.set(inEx, ':') )
            throw sid::exception(key + ": Invalid syntax");
        }
        else
        {
          // ##BASE64(<USERNAME>):BASE64(<PASSWORD>)
          inEx = in.substr(2);
          if ( ! cred.set(in, ':') )
            throw sid::exception(key + ": Invalid syntax");
          cred.userName = sid::base64::decode(cred.userName);
          cred.password = sid::base64::decode(cred.password);
        }
        return cred;
      };

    auto get_lun = [&](const std::string& key, const std::string& in)->int
      {
        int lun = 0;
        if ( !sid::to_num(in, lun) )
          throw sid::exception(key + ": Invalid value");
        if ( lun < 0 )
          throw sid::exception(key + " cannot be negative");
        return lun;
      };

    std::vector<std::string> outVec;
    sid::split(outVec, _infoStr.substr(8), '/', SPLIT_TRIM);
    cout << "Portal: " << outVec[0] << endl;
    std::string key, value;
    basic_cred chap, mchap;
    int lun = -1;
    std::set<std::string> keys;
    for ( size_t i = 1; i < outVec.size(); i++ )
    {
      const std::string& s = outVec[i];
      if ( s.empty() || s[0] != '@' )
        throw sid::exception("Invalid syntax at position " + sid::to_str(i));
      size_t pos = s.find('=');
      if ( pos == std::string::npos )
        throw sid::exception("Invalid syntax: " + s);
      key = s.substr(0, pos);
      if ( key != "@iqn" && keys.find(key) != keys.end() )
        throw sid::exception(key + " cannot be repeated");
      keys.insert(key);
      value = s.substr(pos+1);
      if ( key == "@iqn" )
        cout << "IQN: " << value << endl;
      else if ( key == "@lun" )
        lun = get_lun(key, value);
      else if ( key == "@chap" )
        chap = get_cred(key, value);
      else if ( key == "@mchap" )
        mchap = get_cred(key, value);
      else
        throw sid::exception("Invalid key " + key);
    }
  }
  catch (const sid::exception& e)
  {
    cout << e.what() << endl;
  }
  catch (const std::string& s)
  {
    cout << s << endl;
  }
  catch (...)
  {
    cout << __func__ << ": Unhandled exception" << endl;
  }
}

void regex_test(const std::string& _infoStr)
{
  try
  {
    sid::regex regEx("^iscsi://([^/]+)(/@((iqn)|(lun)|(chap)|(mchap))=([^/]*))*$");
    /*
      1) Portal
      2) -- All additional @key=value pairs
      3) -- Param key
      4) iqn
      5) lun
      6) chap
      7) mchap
      8) value
    */
    sid::regex::result out;
    if ( !regEx.exec(_infoStr.c_str(), /*out*/ out) )
      throw sid::exception(std::string("Invalid device info [") + _infoStr + "]: " + regEx.error());

    for ( size_t i = 0; i < out.size(); i++ )
      cout << i << ") " << out[i] << endl;
  }
  catch (const sid::exception& e)
  {
    cout << e.what() << endl;
  }
  catch (const std::string& s)
  {
    cout << s << endl;
  }
  catch (...)
  {
    cout << __func__ << ": Unhandled exception" << endl;
  }
}

void json_schema_test(const std::string& schemaFile)
{
  json::schema schema = json::schema::parse_file(schemaFile);
  json::value jroot;
  cout << schema.to_str() << endl;
  return;
}

int main(int argc, char* argv[])
{
  ::srand(::time(nullptr));
  try
  {
    if ( argc < 2 )
      throw std::string("Need atleast one argument");

    std::string out = base64::encode(argv[1]);
    cout << out.length() << ": " << out << endl;
    out = base64::decode(out);
    cout << out.length() << ": " << out << endl;
    return 0;
    //json_schema_test(argv[1]);
    //return 0;
    //regex_test1(argv[1]);
    //return 0;

    //json::value j1 = "string";
    //json::value j2 = j1;
    //return 0;
    /*
    long double d;
    std::string csErr;
    if ( ! sid::to_num("  e10.343E6", d, &csErr) )
      throw csErr;
    cout << "ld: " << d << endl;
    cout << "double: " << sid::to_num<double>("10.343E6") << endl;
    cout << "float: " << sid::to_num<float>("  1.999999999999999999") << endl;
    cout << "long double: " << sid::to_num<long double>("  1.999999999999999999") << endl;
    cout << "long double: " << to_type<long double>() << endl;
    cout << "double: " << to_type<double>() << endl;
    cout << "float: " << to_type<float>() << endl;
    cout << "uint64_t: " << to_type<uint64_t>() << endl;
    cout << "int64_t: " << to_type<int64_t>() << endl;
    cout << "bool: " << to_type<bool>() << endl;
    cout << typeid(char).name() << endl;
    return 0;
*/
    cout << "sizeof(json::value) = " << sizeof(json::value) << endl;
    cout << "sizeof(json::schema) = " << sizeof(json::schema) << endl;
    cout << "sizeof(json::schema::property) = " << sizeof(json::schema::property) << endl;
    json::parser_control ctrl;
    json::parser_stats stats;
    json::value jroot;
    sid::optional<json::format> outputFmt;
    if ( argc > 1 )
    {
      std::string jsonStr = get_file_contents(argv[1]);
      std::string param, key, value;
      for ( int i = 2; i < argc; i++ )
      {
        param = argv[i];
        if ( param[0] != '-' )
          throw sid::exception("Invalid parameter at position " + sid::to_str(i));
        size_t pos = param.find('=');
        if ( pos == std::string::npos )
        {
          key = param;
          value.clear();
        }
        else
        {
          key = param.substr(0, pos);
          value = param.substr(pos+1);
        }
        if ( key == "--dup" || key == "--duplicate" )
        {
          if ( value == "accept" )
            ctrl.dupKey = json::parser_control::dup_key::accept;
          else if ( value == "ignore" )
            ctrl.dupKey = json::parser_control::dup_key::ignore;
          else if ( value == "append" )
            ctrl.dupKey = json::parser_control::dup_key::append;
          else if ( value == "reject" )
            ctrl.dupKey = json::parser_control::dup_key::reject;
          else if ( ! value.empty() )
            throw sid::exception("Can only be accept|ignore|append|reject");
        }
        else if ( key == "--allow-flex-keys" || key == "--allow-flexible-keys" )
          ctrl.mode.allowFlexibleKeys = 1;
        else if ( key == "--allow-flex-strings" || key == "--allow-flexible-strings" )
          ctrl.mode.allowFlexibleStrings = 1;
        else if ( key == "--allow-nocase" || key == "--allow-nocase-values" )
          ctrl.mode.allowNocaseValues = 1;
        else if ( key == "--show-output" )
        {
          if ( ! value.empty() && value != "false" )
            outputFmt = json::format::get(value);
        }
        else
          throw sid::exception("Invalid key: " + key);
      }
      json::value::parse(jroot, stats, jsonStr, ctrl);
      cout << stats.to_str() << endl;
      //cout << jsonStr << endl;
      if ( outputFmt.exists() )
        cout << jroot.to_str(outputFmt()) << endl;

      //cout << jroot.to_str(json::format_type::pretty) << endl;
      //json::value j1 = jroot;
      //cout << stats.to_str() << endl;
      //cout << "*** Before clear() " << endl;
      //jroot.clear();
      //cout << "*** After clear() " << endl;
      return 0;
    }
    else
    {
      json::value& jname = jroot["name"];
      jname["id"] = 1;
      jname["first"] = "Shan";
      jname["last"] = "Anand";
      json::value jmeta = json::value_type::object;
      jmeta["storage_group_id"] = "1";
      jmeta["policy_id"] = nullptr;
      jmeta["written_size"] = 32423423;
      jmeta["pi"] = 3.14159L;
      //jroot["meta"] = jmeta.to_str();
      jroot["meta"] = jmeta;
      //std::string jsonStr = jroot.to_str(json::format(json::format_type::pretty, true));

      bool flexibleKey = true;
      std::string jsonStr = jroot.to_str(json::format(json::format_type::compact, flexibleKey));
      cout << jsonStr << endl;
 
      cout << "=====================================================" << endl;
      json::value jsecond;

      json::parser_control ctrl;
      ctrl.mode.allowFlexibleKeys = flexibleKey? 1 : 0;
      json::value::parse(jsecond, jsonStr, ctrl);
      jsonStr = jsecond.to_str(json::format_type::pretty);
      cout << jsonStr << endl;
      return 0;

      for ( size_t i = 0; i < 600000; i++ )
      {
        json::value& jperson = jroot.append();
        json::value& jname = jperson["name"];
        jname["id"] = i;
        jname["first"] = "Shan";
        jname["last"] = "Anand";
        jname["middle"] = nullptr;
        jperson["male"] = true;
        jperson["year"] = 1975;
        json::value& jtest = jperson;//["test"];
        jtest["int"] = -3423;
        jtest["uint"] = 3423;
        jtest["double-1"] = 23432.32L;
        jtest["double-2"] = -3432e16;
        jtest["str-1"] = "v\nal\"u\\e";
        jtest["str-2"] = "unicode-\u0B85";
        //jtest["str-3"] = json::value::get("{\"k1\":\"\\\\u0B85\"}");
        json::value& jarray = jtest["array"];
        jarray.append(100);
        jarray.append(-200);
        jarray.append(300);
        jarray.append(-400);
        jtest["empty_array"] = json::value(json::value_type::array);
        jtest["empty_object"] = json::value(json::value_type::object);
        json::value& jaoa = jtest["array_of_arrays"] = json::value(json::value_type::array);
        json::value& jmetadata = jperson["metadata"];
        for ( size_t j = 0; j < (i%5)+1; j++ )
        {
          json::value& ja = jaoa.append();
          ja.append() = uuid::create().to_str();
          ja.append() = sid::to_str(::rand());

          json::value& jentry = jmetadata.append();
          jentry["key"] = "key-" + sid::to_str(j);
          jentry["value"] = "value-" + sid::to_str(j);
        }
      }
      cout << jroot.to_str(json::format_type::pretty) << endl;
      return 0;
    }

    struct New1
    {
      std::string   m_data;
      json::value_type m_type;
    }new1;
    struct New3
    {
      union union_data
      {
        long double d __attribute__((packed));
        uint64_t    uval;
        bool b;
        std::string str;
        std::vector<New3> vstr;
        std::map<std::string, New3>* mstr;
        union_data() {}
        ~union_data() {}
      };
      union_data   m_data;
      json::value_type m_type;
    }new3;

    cout << "sizeof(json::value) = " << sizeof(json::value) << endl;
    cout << "sizeof(long double) = " << sizeof(long double) << endl;
    cout << "sizeof(std::string) = " << sizeof(std::string) << endl;
    cout << "sizeof(uint64_t) = " << sizeof(uint64_t) << endl;
    cout << "sizeof(std::map<>*) = " << sizeof(std::map<std::string, json::value>*) << endl;
    cout << "sizeof(std::vector<>) = " << sizeof(std::vector<json::value>) << endl;
    cout << "sizeof(int*) = " << sizeof(int*) << endl;
    //cout << "sizeof(json::value::union_data) = " << sizeof(json::value::union_data) << endl;
    cout << "sizeof(json::value_type) = " << sizeof(json::value_type) << endl;
    cout << "sizeof(New1) = " << sizeof(new1.m_data) << " + " << sizeof(new1.m_type) << " = " << sizeof(new1) << endl;
    cout << "sizeof(New3) = " << sizeof(new3.m_data) << " + " << sizeof(new3.m_type) << " = " << sizeof(new3) << endl;
    cout << endl;
    //cout << "str-1: " << jtest["str-1"].as_str() << " : " << jtest["str-1"].to_str() << endl;
    //cout << "str-2: " << jtest["str-2"].as_str() << " : " << jtest["str-2"].to_str() << endl;
    //cout << "str-3: " << jtest["str-3"]["k1"].as_str() << " : " << jtest["str-3"]["k1"].to_str() << endl;
    //cout << jroot.to_str() << endl;
    parser_test(jroot.to_str());
    //json::format format(' ', 0);
    //parser_test(jroot.to_str(format));
    //parser_test("[\t\n{  \n \t }\n\t]");
  }
  catch (const std::string& err)
  {
    cerr << err << endl;
  }
  catch (const std::exception& e)
  {
    cerr << e.what() << endl;
  }
  catch (...)
  {
    cerr << "An unhandled exception occurred" << endl;
  }
  return 0;
}
