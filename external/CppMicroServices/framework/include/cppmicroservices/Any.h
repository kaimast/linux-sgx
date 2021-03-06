/*=============================================================================

  Library: CppMicroServices


Copyright Kevlin Henney, 2000, 2001, 2002. All rights reserved.
Extracted from Boost 1.46.1 and adapted for CppMicroServices.

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

=========================================================================*/

#ifndef CPPMICROSERVICES_ANY_H
#define CPPMICROSERVICES_ANY_H

#include "cppmicroservices/FrameworkConfig.h"

#include <algorithm>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <typeinfo>
#include <vector>

/**

\defgroup gr_any Any

\brief The Any class and related functions.

*/

namespace cppmicroservices {

class Any;

US_Framework_EXPORT std::ostream& any_value_to_string(std::ostream& os,
                                                      const Any& any);

US_Framework_EXPORT std::ostream& any_value_to_json(std::ostream& os,
                                                    const Any& val);
US_Framework_EXPORT std::ostream& any_value_to_json(std::ostream& os,
                                                    const std::string& val);
US_Framework_EXPORT std::ostream& any_value_to_json(std::ostream& os, bool val);

template<typename ValueType>
ValueType* any_cast(Any* operand);

template<class T>
std::ostream& any_value_to_string(std::ostream& os, const T& val)
{
  os << val;
  return os;
}

template<class T>
std::ostream& any_value_to_json(std::ostream& os, const T& val)
{
  return os << val;
}

/**
 * \internal
 */
template<typename Iterator>
std::ostream& container_to_string(std::ostream& os, Iterator i1, Iterator i2)
{
  os << "[";
  const Iterator begin = i1;
  for (; i1 != i2; ++i1) {
    if (i1 == begin) {
      any_value_to_string(os, *i1);
    } else {
      os << ",";
      any_value_to_string(os, *i1);
    }
  }
  os << "]";
  return os;
}

/**
 * \internal
 */
template<typename Iterator>
std::ostream& container_to_json(std::ostream& os, Iterator i1, Iterator i2)
{
  os << "[";
  const Iterator begin = i1;
  for (; i1 != i2; ++i1) {
    if (i1 == begin) {
      any_value_to_json(os, *i1);
    } else {
      os << ",";
      any_value_to_json(os, *i1);
    }
  }
  os << "]";
  return os;
}

template<class E>
std::ostream& any_value_to_string(std::ostream& os, const std::vector<E>& vec)
{
  return container_to_string(os, vec.begin(), vec.end());
}

template<class E>
std::ostream& any_value_to_json(std::ostream& os, const std::vector<E>& vec)
{
  return container_to_json(os, vec.begin(), vec.end());
}

template<class E>
std::ostream& any_value_to_string(std::ostream& os, const std::list<E>& l)
{
  return container_to_string(os, l.begin(), l.end());
}

template<class E>
std::ostream& any_value_to_json(std::ostream& os, const std::list<E>& l)
{
  return container_to_json(os, l.begin(), l.end());
}

template<class E>
std::ostream& any_value_to_string(std::ostream& os, const std::set<E>& s)
{
  return container_to_string(os, s.begin(), s.end());
}

template<class E>
std::ostream& any_value_to_json(std::ostream& os, const std::set<E>& s)
{
  return container_to_json(os, s.begin(), s.end());
}

template<class M>
std::ostream& any_value_to_string(std::ostream& os, const std::map<M, Any>& m);

template<class K, class V>
std::ostream& any_value_to_string(std::ostream& os, const std::map<K, V>& m);

template<class M>
std::ostream& any_value_to_json(std::ostream& os, const std::map<M, Any>& m);

template<class K, class V>
std::ostream& any_value_to_json(std::ostream& os, const std::map<K, V>& m);

/**
 * \ingroup gr_any
 *
 * An Any class represents a general type and is capable of storing any type, supporting type-safe extraction
 * of the internally stored data.
 *
 * Code taken from the Boost 1.46.1 library. Original copyright by Kevlin Henney. Modified for CppMicroServices.
 */
class US_Framework_EXPORT Any
{
public:
  /**
   * Creates an empty any type.
   */
  Any() {}

  /**
   * Creates an Any which stores the init parameter inside.
   *
   * \param value The content of the Any
   *
   * Example:
   * \code
   * Any a(13);
   * Any a(string("12345"));
   * \endcode
   */
  template<typename ValueType>
  Any(const ValueType& value)
    : _content(new Holder<ValueType>(value))
  {}

  /**
   * Copy constructor, works with empty Anys and initialized Any values.
   *
   * \param other The Any to copy
   */
  Any(const Any& other)
    : _content(other._content ? other._content->Clone() : 0)
  {}

  /**
   * Move constructor.
   *
   * @param other The Any to move
   */
  Any(Any&& other)
    : _content(std::move(other._content))
  {}

  /**
   * Swaps the content of the two Anys.
   *
   * \param rhs The Any to swap this Any with.
   */
  Any& Swap(Any& rhs)
  {
    std::swap(_content, rhs._content);
    return *this;
  }

  /**
   * Compares this Any with another value.
   * If the internal type of this any and of \c val do not
   * match, the comparison always returns false.
   *
   * \param val The value to compare to.
   * \returns \c true if this Any contains value \c val, \c false otherwise.
   */
  template<typename ValueType>
  bool operator==(const ValueType& val) const
  {
    if (Type() != typeid(ValueType))
      return false;
    return *any_cast<const ValueType>(this) == val;
  }

  /**
   * Compares this Any with another value for inequality.
   *
   * This is the same as
   * \code
   * !this->operator==(val)
   * \endcode
   *
   * \param val The value to compare to.
   * \returns \c true if this Any does not contain value \c val, \c false otherwise.
   */
  template<typename ValueType>
  bool operator!=(const ValueType& val) const
  {
    return !operator==(val);
  }

  /**
   * Assignment operator for all types != Any.
   *
   * \param rhs The value which should be assigned to this Any.
   *
   * Example:
   * \code
   * Any a = 13;
   * Any a = string("12345");
   * \endcode
   */
  template<typename ValueType>
  Any& operator=(const ValueType& rhs)
  {
    Any(rhs).Swap(*this);
    return *this;
  }

  /**
   * Assignment operator for Any.
   *
   * \param rhs The Any which should be assigned to this Any.
   */
  Any& operator=(const Any& rhs)
  {
    Any(rhs).Swap(*this);
    return *this;
  }

  /**
   * Move assignment operator for Any.
   *
   * \param rhs The Any which should be moved into this Any.
   * \return A reference to this Any.
   */
  Any& operator=(Any&& rhs)
  {
    _content = std::move(rhs._content);
    return *this;
  }

  /**
   * returns true if the Any is empty
   */
  bool Empty() const { return !_content; }

  /**
   * Returns a string representation for the content if it is not empty.
   *
   * Custom types should either provide a <code>std::ostream& operator<<(std::ostream& os, const CustomType& ct)</code>
   * function or specialize the any_value_to_string template function for meaningful output.
   *
   * \throws std::logic_error if the Any is empty.
   */
  std::string ToString() const;

  /**
   * Returns a string representation for the content. If the Any is
   * empty, an empty string is returned.
   *
   * Custom types should either provide a <code>std::ostream& operator<<(std::ostream& os, const CustomType& ct)</code>
   * function or specialize the any_value_to_string template function for meaningful output.
   */
  std::string ToStringNoExcept() const;

  /**
   * Returns a JSON representation for the content.
   *
   * Custom types should specialize the any_value_to_json template function for meaningful output.
   */
  std::string ToJSON() const { return Empty() ? "null" : _content->ToJSON(); }

  /**
   * Returns the type information of the stored content.
   * If the Any is empty typeid(void) is returned.
   * It is suggested to always query an Any for its type info before trying to extract
   * data via an any_cast/ref_any_cast.
   */
  const std::type_info& Type() const
  {
    return _content ? _content->Type() : typeid(void);
  }

private:
  class Placeholder
  {
  public:
    virtual ~Placeholder() {}

    virtual std::string ToString() const = 0;
    virtual std::string ToJSON() const = 0;

    virtual const std::type_info& Type() const = 0;
    virtual std::unique_ptr<Placeholder> Clone() const = 0;
  };

  template<typename ValueType>
  class Holder : public Placeholder
  {
  public:
    Holder(const ValueType& value)
      : _held(value)
    {}

    virtual std::string ToString() const
    {
      std::stringstream ss;
      any_value_to_string(ss, _held);
      return ss.str();
    }

    virtual std::string ToJSON() const
    {
      std::stringstream ss;
      any_value_to_json(ss, _held);
      return ss.str();
    }

    virtual const std::type_info& Type() const { return typeid(ValueType); }

    virtual std::unique_ptr<Placeholder> Clone() const
    {
      return std::unique_ptr<Placeholder>(new Holder(_held));
    }

    ValueType _held;

  private: // intentionally left unimplemented
    Holder& operator=(const Holder&);
  };

private:
  template<typename ValueType>
  friend ValueType* any_cast(Any*);

  template<typename ValueType>
  friend ValueType* unsafe_any_cast(Any*);

  std::unique_ptr<Placeholder> _content;
};

/**
 * \ingroup gr_any
 *
 * The BadAnyCastException class is thrown in case
 * of casting an Any instance
 */
class BadAnyCastException : public std::bad_cast
{
public:
  BadAnyCastException(const std::string& msg = "")
    : std::bad_cast()
    , _msg(msg)
  {}

  ~BadAnyCastException() throw() {}

  virtual const char* what() const throw()
  {
    if (_msg.empty())
      return "cppmicroservices::BadAnyCastException: "
             "failed conversion using cppmicroservices::any_cast";
    else
      return _msg.c_str();
  }

private:
  std::string _msg;
};

namespace detail {
/**
 *
 * A utility function used to throw a BadAnyCastException object
 * containing an exception message containing the source and target type names.
 *
 * \param funcName The throwing function's name.
 * \param anyTypeName A string representing the Any object's underlying type.
 * \throws cppmicroservices::BadAnyCastException
 */
US_Framework_EXPORT void ThrowBadAnyCastException(const std::string& funcName,
                                                  const std::type_info& source,
                                                  const std::type_info& target);
}

/**
 * \ingroup gr_any
 *
 * any_cast operator used to extract the ValueType from an Any*. Will return a pointer
 * to the stored value.
 *
 * Example Usage:
 * \code
 * MyType* pTmp = any_cast<MyType*>(pAny)
 * \endcode
 * Will return nullptr if the cast fails, i.e. types don't match.
 */
template<typename ValueType>
ValueType* any_cast(Any* operand)
{
  return operand && operand->Type() == typeid(ValueType)
           ? &static_cast<Any::Holder<ValueType>*>(operand->_content.get())
                ->_held
           : nullptr;
}

/**
 * \ingroup gr_any
 *
 * any_cast operator used to extract a const ValueType pointer from an const Any*. Will return a const pointer
 * to the stored value.
 *
 * Example Usage:
 * \code
 * const MyType* pTmp = any_cast<MyType*>(pAny)
 * \endcode
 * Will return nullptr if the cast fails, i.e. types don't match.
 */
template<typename ValueType>
const ValueType* any_cast(const Any* operand)
{
  return any_cast<ValueType>(const_cast<Any*>(operand));
}

/**
 * \ingroup gr_any
 *
 * any_cast operator used to extract a copy of the ValueType from an const Any&.
 *
 * Example Usage:
 * \code
 * MyType tmp = any_cast<MyType>(anAny)
 * \endcode
 *
 * \throws BadAnyCastException if the cast fails.
 *
 * Dont use an any_cast in combination with references, i.e. MyType& tmp = ... or const MyType& = ...
 * Some compilers will accept this code although a copy is returned. Use the ref_any_cast in
 * these cases.
 */
template<typename ValueType>
ValueType any_cast(const Any& operand)
{
  ValueType* result = any_cast<ValueType>(const_cast<Any*>(&operand));
  if (!result) {
    detail::ThrowBadAnyCastException(
      std::string("any_cast"), operand.Type(), typeid(ValueType));
  }
  return *result;
}

/**
 * \ingroup gr_any
 *
 * any_cast operator used to extract a copy of the ValueType from an Any&.
 *
 * Example Usage:
 * \code
 * MyType tmp = any_cast<MyType>(anAny)
 * \endcode
 *
 * \throws BadAnyCastException if the cast fails.
 *
 * Dont use an any_cast in combination with references, i.e. MyType& tmp = ... or const MyType& tmp = ...
 * Some compilers will accept this code although a copy is returned. Use the ref_any_cast in
 * these cases.
 */
template<typename ValueType>
ValueType any_cast(Any& operand)
{
  ValueType* result = any_cast<ValueType>(&operand);
  if (!result) {
    detail::ThrowBadAnyCastException(
      std::string("any_cast"), operand.Type(), typeid(ValueType));
  }
  return *result;
}

/**
 * \ingroup gr_any
 *
 * ref_any_cast operator used to return a const reference to the internal data.
 *
 * Example Usage:
 * \code
 * const MyType& tmp = ref_any_cast<MyType>(anAny);
 * \endcode
 *
 * \throws BadAnyCastException if the cast fails.
 */
template<typename ValueType>
const ValueType& ref_any_cast(const Any& operand)
{
  ValueType* result = any_cast<ValueType>(const_cast<Any*>(&operand));
  if (!result) {
    detail::ThrowBadAnyCastException(
      std::string("ref_any_cast"), operand.Type(), typeid(ValueType));
  }
  return *result;
}

/**
 * \ingroup gr_any
 *
 * ref_any_cast operator used to return a reference to the internal data.
 *
 * Example Usage:
 * \code
 * MyType& tmp = ref_any_cast<MyType>(anAny);
 * \endcode
 *
 * \throws BadAnyCastException if the cast fails.
 */
template<typename ValueType>
ValueType& ref_any_cast(Any& operand)
{
  ValueType* result = any_cast<ValueType>(&operand);
  if (!result) {
    detail::ThrowBadAnyCastException(
      std::string("ref_any_cast"), operand.Type(), typeid(ValueType));
  }
  return *result;
}

/**
 * \internal
 *
 * The "unsafe" versions of any_cast are not part of the
 * public interface and may be removed at any time. They are
 * required where we know what type is stored in the any and can't
 * use typeid() comparison, e.g., when our types may travel across
 * different shared libraries.
 */
template<typename ValueType>
ValueType* unsafe_any_cast(Any* operand)
{
  return &static_cast<Any::Holder<ValueType>*>(operand->_content.get())->_held;
}

/**
 * \internal
 *
 * The "unsafe" versions of any_cast are not part of the
 * public interface and may be removed at any time. They are
 * required where we know what type is stored in the any and can't
 * use typeid() comparison, e.g., when our types may travel across
 * different shared libraries.
 */
template<typename ValueType>
const ValueType* unsafe_any_cast(const Any* operand)
{
  return any_cast<ValueType>(const_cast<Any*>(operand));
}

template<class K>
std::ostream& any_value_to_string(std::ostream& os, const std::map<K, Any>& m)
{
  os << "{";
  typedef typename std::map<K, Any>::const_iterator Iterator;
  Iterator i1 = m.begin();
  const Iterator begin = i1;
  const Iterator end = m.end();
  for (; i1 != end; ++i1) {
    if (i1 == begin)
      os << i1->first << " : " << i1->second.ToString();
    else
      os << ", " << i1->first << " : " << i1->second.ToString();
  }
  os << "}";
  return os;
}

template<class K, class V>
std::ostream& any_value_to_string(std::ostream& os, const std::map<K, V>& m)
{
  os << "{";
  typedef typename std::map<K, V>::const_iterator Iterator;
  Iterator i1 = m.begin();
  const Iterator begin = i1;
  const Iterator end = m.end();
  for (; i1 != end; ++i1) {
    if (i1 == begin)
      os << i1->first << " : " << i1->second;
    else
      os << ", " << i1->first << " : " << i1->second;
  }
  os << "}";
  return os;
}

template<class K>
std::ostream& any_value_to_json(std::ostream& os, const std::map<K, Any>& m)
{
  os << "{";
  typedef typename std::map<K, Any>::const_iterator Iterator;
  Iterator i1 = m.begin();
  const Iterator begin = i1;
  const Iterator end = m.end();
  for (; i1 != end; ++i1) {
    if (i1 == begin)
      os << "\"" << i1->first << "\" : " << i1->second.ToJSON();
    else
      os << ", "
         << "\"" << i1->first << "\" : " << i1->second.ToJSON();
  }
  os << "}";
  return os;
}

template<class K, class V>
std::ostream& any_value_to_json(std::ostream& os, const std::map<K, V>& m)
{
  os << "{";
  typedef typename std::map<K, V>::const_iterator Iterator;
  Iterator i1 = m.begin();
  const Iterator begin = i1;
  const Iterator end = m.end();
  for (; i1 != end; ++i1) {
    if (i1 == begin)
      os << "\"" << i1->first << "\" : " << i1->second;
    else
      os << ", "
         << "\"" << i1->first << "\" : " << i1->second;
  }
  os << "}";
  return os;
}
}

#endif // CPPMICROSERVICES_ANY_H
