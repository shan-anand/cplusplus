#ifndef _SID_SIMPLE_TYPES_HPP_
#define _SID_SIMPLE_TYPES_HPP_

#include <string>
#include <functional>

namespace sid {

typedef std::function<bool()> FNContinueCallback;

//! basic_cred data structure
struct basic_cred
{
  std::string userName, password;

  //! Default constructor
  basic_cred() { clear(); }
  //! Copy constructor
  basic_cred(const basic_cred&) = default;
  //! All-arg constructor
  basic_cred(const std::string& _userName, const std::string& _password) :
    userName(_userName), password(_password) {}
  //! Virtual destructor
  virtual ~basic_cred() {}

  //! Clears the members of the structure
  void clear() { userName.clear(); password.clear(); }
  //! Checks whether either of the members is empty
  bool empty() const { return userName.empty() || password.empty(); }

  //! Equality / Unequality functions
  inline bool equals(const basic_cred& _obj) const 
    { return userName == _obj.userName && password == _obj.password; }
  bool operator==(const basic_cred& _obj) const { return equals(_obj); }
  bool operator!=(const basic_cred& _obj) const { return !equals(_obj); }

  //! set both username and password and return the object
  basic_cred& set(const std::string& _userName, const std::string& _password)
    { this->userName = _userName; this->password = _password; return *this; }

  //! set both username and password that is separated by a delimiter
  bool set(const std::string& _userPassword, const char _delimiter = ':')
    {
      size_t pos = _userPassword.find(_delimiter);
      if ( pos == std::string::npos ) return false;
      this->userName = _userPassword.substr(0, pos);
      this->password = _userPassword.substr(pos+1);
      return true;
    }
};

} // namespace sid

#endif // _SID_SIMPLE_TYPES_HPP_
