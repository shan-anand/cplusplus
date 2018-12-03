/*
LICENSE: BEGIN
===============================================================================
@author Shanmuga (Anand) Gunasekaran
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@brief Implementation of a generic Smart pointer class
===============================================================================
MIT License

Copyright (c) 2017 Shanmuga (Anand) Gunasekaran

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
===============================================================================
LICENSE: END
*/

/**
 * @file smart_ptr.hpp
 * @brief New and improved implementation of Smart pointer class for handling
 *        deletion of pointers automatically.
 *        This version is a thread-safe implmentation of the smart pointer class.
 *        However, the internal pointer itself is not thread-safe. It should be
 *        taken care of by the implementer of that datatype.
 *
 *        1) All structs and classes MUST be derived from smart_ref in-order to use smart_ptr.
 *           Any attempt to use it without deriving from smart_ref will result
 *           in a compiler error.
 *        2) Basic datatypes like int, char, bool etc. can be used directly.
 *           See example (1) for details.
 *        3) Any union or pre-defined struct/classes must be encapsulated in
 *           smart_ref_container before using it in smart_ptr.
 *           See example (2) for details.
 *
 * @example
 *       (1) sid::smart_ptr<int> p1 = new int;
 *           {
 *             // Increments the reference count on the smart pointer
 *             sid::smart_ptr<int> p2 = p1;
 *             // As we leave this scope the reference count is decremented by one
 *           }
 *           // In the above example when p1 goes out of scope, it will be
 *           // destroyed automatically using smart_ptr_delete()
 *
 *           sid::smart_ptr<int, smart_ptr_free> x1 = (int) malloc(sizeof(int));
 *           // In the above example when x1 goes out of scope, it will be
 *           // destroyed automatically using smart_ptr_free()
 *
 *       (2) typedef sid::smart_ptr< sid::smart_ref_container<struct tm> > struct_tmPtr;
 *           time_t now = time(NULL);
 *           struct_tmPtr tm1 = sid::smart_ref_container<struct tm>::create_smart_ptr(new struct tm);
 *           localtime_r(&now, tm1.ptr()->ptr()); // NOTE .ptr()->ptr()
 *           {
 *             struct_tmPtr tm2 = tm1; // reference count incremented
 *           } // reference count decremented
 *           cout << tm1.ptr()->ptr()->tm_sec << endl;
 *
 * @note THE FOLLOWING SCENARIOS MUST BE AVOIDED for non-struct/class smart_ptr:
 *       ======================================================================
 *       1) sid::smart_ptr<int> p1 = new int;
 *          sid::smart_ptr<int> p2 = p1.ptr(); // DANGEROUS. p2 will create a new internal reference, for the same pointer as p1
 *                                          Both p1 and p2 will try destroying the same pointer, the second delete will crash.
 *
 */
#ifndef _SID_SMART_PTR_
#define _SID_SMART_PTR_

#include <type_traits>
#include <typeinfo>
#include <atomic>
#include "exception.hpp"

#include <iostream>
using namespace std;

namespace sid {

/**
 * @class smart_ref
 * @brief Any class or struct planning use smart_ptr should derive from smart_ref class
 *        for better handling of smart pointer
 */
class smart_ref
{
public:
  smart_ref() :  __refcount__value(0) {}
  smart_ref(const smart_ref&) : __refcount__value(0) {}
  smart_ref& operator=(const smart_ref&) { return *this; }
  virtual ~smart_ref() {}
  // Allow smart_ptr to access the private members
  template <class U, void (*pfnDelete)(U*)> friend class smart_ptr;
private:
  mutable std::atomic<int> __refcount__value;
};

//! Destroy the pointer using c++ delete operator
template <typename T> void smart_ptr_delete(T* _ptr) { if ( _ptr ) delete _ptr; }
//! Destroy the pointer using c++ delete[] operator
template <typename T> void smart_ptr_delete_array(T* _ptr) { if ( _ptr ) delete[] _ptr; }
//! Destroy the pointer using c++ free operator
template <typename T> void smart_ptr_free(T* _ptr) { if ( _ptr ) free(_ptr); }

/**
 * @brief A template class for handling smart pointers.
 *        Users only have to allocate pointers using "new".
 *        Deletion is taken care of automatically.
 */
template <typename T, void (*pfnDelete)(T*) = smart_ptr_delete> class smart_ptr
{
public:
  //! Any struct or class MUST be derived from smart_ref to be used as a smart_ptr
  static_assert(!std::is_class<T>::value || std::is_base_of<smart_ref, T>::value, "T must inherit from smart_ref");
  /**
   * @brief Prevent using union as a smart_ptr because it cannot be derived from a structure
   *        However, the main reason for preventing it is the danger of assigning the
   *        "this" pointer to a smart_ptr within any function in the union.
   */
  static_assert(!std::is_union<T>::value, "We do not allow creating smart pointer with union to prevent mis-usage of this pointer");

  //! Default constructor
  smart_ptr() : m_data(nullptr) {}
  //! One-arg constructor. Starts a fresh object with reference count 1. May throw T* exception
  smart_ptr(T* _ptr) : m_data(nullptr) { p_assign(_ptr); }
  //! Copy constructor (Increments the reference count)
  smart_ptr(const smart_ptr& _obj) : m_data(nullptr) { p_assign(_obj); }
  //! Virtual destructor. Releases the smart pointer by decrementing the reference count.
  virtual ~smart_ptr() { p_release(); }

  //! Returns the pointer to the object
  T* ptr() const { return p_get(m_data); }
  //! Checks whether the smart pointer is empty or not
  bool empty() const { return !p_get(m_data); }
  //! Clears the smart pointer by decrementing the reference count.
  void clear() { p_release(); m_data = nullptr; }

  // scope resolution operators ( throws a sid::exception() error if it is a null pointer)
  const T* operator ->() const throw (sid::exception) { throw_if_empty(); return p_get(m_data); }
  T* operator ->() throw (sid::exception) { throw_if_empty(); return p_get(m_data); }
  const T& operator *() const throw (sid::exception) { throw_if_empty(); return *(p_get(m_data)); }
  T& operator *() throw (sid::exception) { throw_if_empty(); return *(p_get(m_data)); }

  // condition checking operators
  bool operator ==(T* _ptr) const { return (this->ptr() == _ptr); }
  bool operator ==(const smart_ptr& _obj) const { return (m_data == _obj.m_data); }
  bool operator !=(T* _ptr) const { return (this->ptr() != _ptr); }
  bool operator !=(const smart_ptr& _obj) const { return (m_data != _obj.m_data); }
  bool operator !() const { return (!m_data || !p_get(m_data)); }
  operator void*() const { return this->ptr(); }
  int ref_count() const { return get_ref_count(); }

  // assignment operators
  //! Starts a fresh object with reference count 1. May throw T* exception
  smart_ptr& operator =(T* _ptr) { return p_assign(_ptr); }
  smart_ptr& operator =(const smart_ptr& _obj) { return p_assign(_obj); }

//////////////////////////
//
// private members
//
//////////////////////////
private:
  struct smart_ref_helper
  {
  public:
    static smart_ref* allocate(T* _ptr)
    {
      //cout << "Calling smart_ref_helper allocate" << endl;
      return dynamic_cast<smart_ref*>(_ptr);
    }
    static void deallocate(smart_ref* _pData)
      {
        if ( _pData )
        {
          // Get T* and delete, or can we delete pData directly?
          T* p = get(_pData);
          if ( p ) (*pfnDelete)(p);
        }
      }
    static T* get(smart_ref* _pData)
      {
        if ( !_pData ) return nullptr;
        T* p = dynamic_cast<T*>(_pData);
        if ( !p )
        {
          ///////////////////////////////////////////////////////////////////////////////////
          //
          //                    ***** THIS IS A SPECIAL CASE ******
          //
          ////////////////////////////////////////////////////////////////////////////////////
          // This can happen when the upcasting class inherits smart_ref as private/protected.
          // In this case, a dynamic_cast would return nullptr.
          // So, we cast it using static_cast first, and then do a dynamic_cast.
          // ***** If something is wrong, we need to revisit this code. *****
          ////////////////////////////////////////////////////////////////////////////////////
          p = dynamic_cast<T*>(static_cast<T*>(_pData));
          //cout << "Calling smart_ref_helper get: [" << _pData << "] " << data << endl;
        }
        return p;
      }
  };

  struct smart_ref_ex : public smart_ref
  {
  private:
    T* ptr;
    smart_ref_ex(T* _p) : ptr(_p) {}

  public:
    static smart_ref* allocate(T* _ptr)
      {
        //cout << "Calling smart_ref_ex allocate" << endl;
        smart_ref_ex* pDataEx = nullptr;
        if ( _ptr )
        {
          // allocate memory for handling smart pointer
          pDataEx = new smart_ref_ex(_ptr);
          // if memory allocation failed, throw "ptr" as an exception
          // the caller must use catch() and perform necessary cleanup action on ptr
          if ( pDataEx == nullptr ) throw _ptr;
        }
        return dynamic_cast<smart_ref*>(pDataEx);
      }
    static void deallocate(smart_ref* _pData)
      {
        if ( _pData )
        {
          T* p = get(_pData);
          if ( p ) (*pfnDelete)(p);
          delete _pData;
        }
      }
    static T* get(smart_ref* _pData)
      {
        if ( !_pData ) return nullptr;
        smart_ref_ex* pDataEx = dynamic_cast<smart_ref_ex*>(_pData);
        return ( pDataEx )? pDataEx->ptr : nullptr;
      }
  };

#define IS_ALREADY_SMARTREF (std::is_base_of<smart_ref, T>::value != 0)
  typedef typename std::conditional<IS_ALREADY_SMARTREF, smart_ref_helper, smart_ref_ex>::type smart_data;
  smart_ref* m_data; // internal smart pointer object

private:
  int inc_ref_count() { return (!m_data)?  0 : ++m_data->__refcount__value; }
  int dec_ref_count() { return (!m_data)? -1 : --m_data->__refcount__value; }
  int get_ref_count() const { return (!m_data)? 0 : m_data->__refcount__value.load(); }
  void throw_if_empty() const throw (sid::exception) { if ( empty() ) throw sid::exception("Cannot reference null pointer"); }

  T* p_get(smart_ref* _pData) const { return smart_data::get(_pData); }

  smart_ptr& p_assign(T* _ptr)
  {
    // Perform assignment only when both pointers are different
    if ( this->ptr() != _ptr )
    {
      p_release();
      m_data = smart_data::allocate(_ptr);
      inc_ref_count();
    }
    return *this;
  }

  smart_ptr& p_assign(const smart_ptr& _obj)
  {
    // Perform assignment only when both pointers are different
    if ( m_data != _obj.m_data )
    {
      p_release();
      m_data = _obj.m_data;
      inc_ref_count();
    }
    return *this;
  }

  /**
   * @brief releases the smart pointer object
   */
  void p_release()
  {
    // 1) decrement the reference count
    // 2) if the reference count is zero, delete both the pointers
    if ( m_data != nullptr && dec_ref_count() == 0 )
    {
      smart_data::deallocate(m_data);
      m_data = nullptr;
    }
  }
};

/**
 * @class smart_ref_container
 * @brief Used as smart_ref container class to support unions and c-defined structs
 */
template <typename T, void (*pfnDelete)(T*) = smart_ptr_delete> class smart_ref_container : public smart_ref
{
public:
  //! The only way to create this object
  static smart_ptr< smart_ref_container<T, pfnDelete> > create_smart_ptr(T* _p) throw (T*)
  {
    smart_ptr< smart_ref_container<T, pfnDelete> > ptr;
    if ( _p )
    {
      try { ptr = new smart_ref_container(_p); } catch (...) { throw _p; }
      if ( ptr.empty() ) throw _p;
    }
    return ptr;
  }
  //! Destructor
  ~smart_ref_container() { if ( m_p ) (*pfnDelete)(m_p); }
  //! Checks for empty
  bool empty() const { return (m_p == nullptr); }
  //! Returns the pointer associated with this class
  T* ptr() const { return m_p; }

private:
  T* m_p;

private:
  //! Private constructor
  smart_ref_container(T* _p) : m_p(_p) {}
  smart_ref_container(const smart_ref_container&); // Copy construction is not allowed
  smart_ref_container& operator=(const smart_ref_container&); // Copy operation is not allowed
};

typedef smart_ptr<smart_ref> smart_refPtr;

} // namespace sid

#endif // _SID_SMART_PTR_
