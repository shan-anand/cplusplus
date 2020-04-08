#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <limits>
#include <stdlib.h>
#include "common/uuid.hpp"
#include "common/json.hpp"
#include "common/convert.hpp"

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
  //jroot.write(cout, json::format::pretty);
  cout << jroot.to_str(json::format::pretty) << endl << endl;
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

int main(int argc, char* argv[])
{
  try
  {
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
    json::value jroot;
    if ( argc > 1 )
    {
      std::string jsonStr = get_file_contents(argv[1]);
      json::parser_stats stats;
      //jroot = std::move(json::value::get(jsonStr, stats));
      json::value::parse(jroot, stats, jsonStr);
      cout << stats.to_str() << endl;
      //json::value j1 = jroot;
      //cout << stats.to_str() << endl;
      return 0;
    }
    else
    {
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
	//jtest["array2"] = json::value(json::element::array);
	//jtest["object2"] = json::value(json::element::object);
	/*
	json::value& jmetadata = jperson["metadata"];
	for ( size_t j = 0; j < (i%5)+1; j++ )
	{
	  json::value& jentry = jmetadata.append();
	  jentry["key"] = "key-" + sid::to_str(j);
	  jentry["value"] = "value-" + sid::to_str(j);
	}
	*/
      }
      cout << jroot.to_str(json::format::pretty) << endl;
      return 0;
    }

    struct New1
    {
      std::string   m_data;
      json::element m_type;
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
      json::element m_type;
    }new3;

    cout << "sizeof(json::value) = " << sizeof(json::value) << endl;
    cout << "sizeof(long double) = " << sizeof(long double) << endl;
    cout << "sizeof(std::string) = " << sizeof(std::string) << endl;
    cout << "sizeof(uint64_t) = " << sizeof(uint64_t) << endl;
    cout << "sizeof(std::map<>*) = " << sizeof(std::map<std::string, json::value>*) << endl;
    cout << "sizeof(std::vector<>) = " << sizeof(std::vector<json::value>) << endl;
    cout << "sizeof(int*) = " << sizeof(int*) << endl;
    //cout << "sizeof(json::value::union_data) = " << sizeof(json::value::union_data) << endl;
    cout << "sizeof(json::element) = " << sizeof(json::element) << endl;
    cout << "sizeof(New1) = " << sizeof(new1.m_data) << " + " << sizeof(new1.m_type) << " = " << sizeof(new1) << endl;
    cout << "sizeof(New3) = " << sizeof(new3.m_data) << " + " << sizeof(new3.m_type) << " = " << sizeof(new3) << endl;
    cout << endl;
    //cout << "str-1: " << jtest["str-1"].as_str() << " : " << jtest["str-1"].to_str() << endl;
    //cout << "str-2: " << jtest["str-2"].as_str() << " : " << jtest["str-2"].to_str() << endl;
    //cout << "str-3: " << jtest["str-3"]["k1"].as_str() << " : " << jtest["str-3"]["k1"].to_str() << endl;
    //cout << jroot.to_str() << endl;
    parser_test(jroot.to_str());
    //json::pretty_formatter formatter(' ', 0);
    //parser_test(jroot.to_str(formatter));
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
