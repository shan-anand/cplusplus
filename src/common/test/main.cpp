#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <stdlib.h>
#include "common/uuid.hpp"
#include "common/json.hpp"

using namespace std;
using namespace sid;

void parser_test()
{
  const std::string jsonStr = "{\"key\": \"v\\\"alue1\", \"mname\": null, \"num1\": -34234.23456, \"num2\": 7.012e1, \"numbers\": [100, -100, 12.34, -34.02, -9.223372037e18, 1.844674407e19]}";
  cout << jsonStr << endl;
  json::value jroot = json::value::get(jsonStr);
  cout << jroot["key"].as_str() << endl;
  //cout.precision(std::numeric_limits< long double >::max_digits10);
  cout << jroot["num1"].as_str() << endl;
  cout << jroot["num2"].as_str() << endl;
  const json::value& jnumbers = jroot["numbers"];
  for ( size_t i = 0; i < jnumbers.size(); i++ )
    cout << jnumbers[i].as_str() << " ";
  cout << endl<< endl;
  cout << jroot.to_str() << endl << endl;
}

int main(int argc, char* argv[])
{
  try
  {
    parser_test();
    cout << "sizeof(json::value) = " << sizeof(json::value) << endl;
    json::value jroot1, jroot2, jroot3, jroot4, jroot5, jroot6;
    jroot1 = "Shan";
    jroot2 = -100;
    jroot3 = false;
    jroot4["first"] = jroot1;
    jroot4["last"] = "Anand";
    jroot4["yob"] = 1975;
    jroot5["name"] = jroot4;
    jroot6.append(100);
    jroot6.append(-200);
    jroot6.append(300);
    jroot4.clear();
    cout << jroot1.as_str() << endl;
    jroot1.clear();
    cout << jroot2.as_str() << ", " << jroot2.get_uint64() << endl;
    cout << jroot3.as_str() << endl;
    if ( jroot5.has_key("name") )
    {
      const json::value& jname = jroot5["name"];
      cout << jname["first"].as_str() << " " << jname["last"].as_str() << " year=" << jname["yob"].as_str() << endl;
    }
    else
      cout << "key not found" << endl;
    cout << "Array count: " << jroot6.size() << " val-2: " << jroot6[1].as_str() << endl;
    //cout << jroot4.as_str() << endl;
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
