/**
 * @file local.h
 * @brief Definition of local macros and functions
 */

#include <sstream>
#include <iomanip>

namespace local
{
template <typename T> std::string toString(const T& v, bool inHex = false)
{
  std::ostringstream out;
  if ( !inHex )
    out << v;
  else
  {
    out << "0x" << std::setfill('0') << std::setw(2) << std::hex;
    if ( sizeof(v) != 1 )
      out << v;
    else
      out << (int) v;
  }
  return out.str();
}
} // namespace local
