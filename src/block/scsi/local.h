/**
 * @file local.h
 * @brief Definition of local macros and functions
 */

#include <sstream>
#include <iomanip>

namespace local
{
template <typename T> std::string to_str(const T& _v, bool _inHex = false)
{
  std::ostringstream out;
  if ( ! _inHex )
    out << _v;
  else
  {
    out << "0x" << std::setfill('0') << std::setw(2) << std::hex;
    if ( sizeof(_v) != 1 )
      out << _v;
    else
      out << (int) _v;
  }
  return out.str();
}
} // namespace local
