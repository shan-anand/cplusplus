#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <stdlib.h>
#include "common/uuid.hpp"
#include "common/json.hpp"

using namespace std;
using namespace sid;

void file_test()
{
}

void parser_test(std::string jsonStr)
{
  if ( jsonStr.empty() )
    jsonStr = "{\"key\": \"v\\\"alue1\", \"mname\": null, \"num1\": -34234.23456, \"num2\": 7.012e1, \"numbers\": [100, -100, 12.34, -34.02, -9.223372037e18, 1.844674407e19]}";

  cout << jsonStr << endl;
  json::value jroot = json::value::get(jsonStr);
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
  cout << jroot.to_str() << endl << endl;
}

int main(int argc, char* argv[])
{
  try
  {
    json::value jroot;
    json::value& jperson = jroot["person"];
    json::value& jname = jperson["name"];
    jname["first"] = "Shan";
    jname["last"] = "Anand";
    jname["middle"] = nullptr;
    jperson["male"] = true;
    jperson["year"] = 1975;
    json::value& jtest = jroot["test"];
    jtest["int"] = -3423;
    jtest["uint"] = 3423;
    jtest["double-1"] = 23432.32L;
    jtest["double-2"] = -3432e16;
    jtest["str-1"] = "v\nal\"u\\e";
    jtest["str-2"] = "unicode-\u0B85";
    jtest["str-3"] = json::value::get("{\"k1\":\"\\\\u0B85\"}");
    json::value& jarray = jtest["array"];
    jarray.append(100);
    jarray.append(-200);
    jarray.append(300);
    jarray.append(-400);
    jtest["array2"] = json::value(json::element::array);
    jtest["object2"] = json::value(json::element::object);

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
    cout << "Array count: " << jarray.size() << " val-2: " << jarray[1].as_str() << endl;
    //cout << "str-1: " << jtest["str-1"].as_str() << " : " << jtest["str-1"].to_str() << endl;
    //cout << "str-2: " << jtest["str-2"].as_str() << " : " << jtest["str-2"].to_str() << endl;
    //cout << "str-3: " << jtest["str-3"]["k1"].as_str() << " : " << jtest["str-3"]["k1"].to_str() << endl;
    cout << "Year: " << jperson["year"].as_str() << ", " << jperson["year"].get_uint64() << endl;
    if ( jperson.has_key("name") )
    {
      const json::value& jname = jperson["name"];
      cout << jname["first"].as_str() << " " << jname["last"].as_str() << endl;
    }
    else
      cout << "key not found" << endl;
    //cout << jroot.to_str() << endl;
    parser_test(jroot.to_str(json::format::pretty));
    //json::pretty_formatter formatter(' ', 0);
    //parser_test(jroot.to_str(formatter));
    parser_test("[\t\n{  \n \t }\n\t]");
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
