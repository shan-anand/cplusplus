/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file json.cpp
@brief Json schema handling using c++
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
 * @file  json.cpp
 * @brief Implementation of json parser and handler
 */
#include <common/json.hpp>
#include <common/convert.hpp>
#include <common/optional.hpp>
#include <fstream>
#include <stack>
#include <iomanip>
#include <ctime>
#include <unistd.h>

using namespace sid;
using namespace sid::json;

namespace local
{
void fill_required(
  std::string_set&            _required,
  const value&                _jarray,
  const schema::property_vec& _properties
  );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of schema::property
//
///////////////////////////////////////////////////////////////////////////////////////////////////
schema::property::property()
{
}

void schema::property::clear()
{
  key.clear();
  description.clear();
  type.clear();
  // For numbers
  minimum.clear(0);
  exclusiveMinimum.clear(0);
  maximum.clear(0);
  exclusiveMaximum.clear(0);
  multipleOf.clear(0);
  // For strings
  minLength.clear(0);
  maxLength.clear(0);
  pattern.clear();
  // For arrays
  minItems.clear(0);
  maxItems.clear(0);
  uniqueItems.clear(false);
  minContains.clear(0);
  maxContains.clear(0);
  // For objects
  minProperties.clear(0);
  maxProperties.clear(0);
  required.clear();
  properties.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of schema
//
///////////////////////////////////////////////////////////////////////////////////////////////////
schema::schema()
{
  clear();
}

void schema::clear()
{
  this->_schema = "https://json-schema.org/draft/2020-12/schema";
  //_schema.clear();
  _id.clear();
  title.clear();
  description.clear();
  type.clear();
  properties.clear();
  required.clear();
}

/*static*/
schema schema::parse_file(const std::string& _schemaFile)
{
  std::ifstream in;
  in.open(_schemaFile);
  if ( ! in.is_open() )
    throw sid::exception("Failed to open schema file: ") + _schemaFile;
  char buf[8096] = {0};
  std::string jsonStr;
  while ( ! in.eof() )
  {
    ::memset(buf, 0, sizeof(buf));
    in.read(buf, sizeof(buf)-1);
    if ( in.bad() )
      throw sid::exception(sid::to_errno_str("Failed to read schema file"));
    jsonStr += buf;
  }
  return parse(jsonStr);
}

/*static*/
schema schema::parse(const std::string& _schemaData)
{
  value jroot;
  value::parse(jroot, _schemaData);
  return parse(jroot);
}

/*static*/
schema schema::parse(const value& _jroot)
{
  schema schema;
  value jval;
  if ( _jroot.has_key("$schema", jval) && !jval.is_null() )
    schema._schema = jval.get_str();
  if ( _jroot.has_key("$id", jval) && !jval.is_null() )
    schema._id = jval.get_str();
  if ( _jroot.has_key("title", jval) && !jval.is_null() )
    schema.title = jval.get_str();
  if ( _jroot.has_key("description", jval) && !jval.is_null() )
    schema.description = jval.get_str();

  if ( !_jroot.has_key("type", jval) )
    throw sid::exception("type missing in schema");
  else
    schema.type.add(jval);
  // Top level type must be an object or an array
  {
    schema_types type = schema.type;
    type.remove(schema_type::object);
    type.remove(schema_type::array);
    if ( !type.empty() )
      throw sid::exception("Top-level schema type must be an object or an array");
  }

  const bool hasProperties = _jroot.has_key("properties", jval);
  if ( schema.type.exists(schema_type::object) )
  {
    if ( ! hasProperties )
      throw sid::exception("properties missing in schema");
    schema.properties.set(jval);
  }
  else if ( hasProperties )
    throw sid::exception("properties is applicable only for object type schema");

  if ( _jroot.has_key("required", jval) )
  {
    if ( ! schema.type.exists(schema_type::object) )
      throw sid::exception("required is applicable only for object type schema");
    local::fill_required(schema.required, jval, schema.properties);
  }
  return schema;
}

void schema::property_vec::set(const value& _jproperties)
{
  if ( ! _jproperties.is_object() )
    throw sid::exception("properties must be an object");

  for ( const std::string& key : _jproperties.get_keys() )
  {
    schema::property property;
    property.set(_jproperties, key);
    this->push_back(property);
  }
}

void schema::property::set(const value& _jproperties, const std::string& _key)
{
  const value& jproperty = _jproperties[_key];
  value jval;

  this->key = _key;
  if ( ! jproperty.has_key("type", jval) )
    throw sid::exception("property type missing for ") + this->key;
  this->type.add(jval);
  if ( jproperty.has_key("description", jval) && !jval.is_null() )
    this->description = jval.get_str();

  if ( this->type.exists(schema_type::number) || this->type.exists(schema_type::integer) )
  {
    if ( jproperty.has_key("minimum", jval) )
    {
      if ( ! jval.is_decimal() )
	throw sid::exception("minimum must be a decimal value");
      this->minimum = jval.get_int64();
    }
    if ( jproperty.has_key("exclusiveMinimum", jval) )
    {
      if ( ! jval.is_decimal() )
	throw sid::exception("exclusiveMinimum must be a decimal value");
      this->minimum = jval.get_int64();
    }
    if ( jproperty.has_key("maximum", jval) )
    {
      if ( ! jval.is_decimal() )
	throw sid::exception("maximum must be a decimal value");
      this->minimum = jval.get_int64();
    }
    if ( jproperty.has_key("exclusiveMaximum", jval) )
    {
      if ( ! jval.is_decimal() )
	throw sid::exception("exclusiveMaximum must be a decimal value");
      this->minimum = jval.get_int64();
    }
    if ( jproperty.has_key("multipleOf", jval) )
    {
      if ( ! jval.is_decimal() )
	throw sid::exception("multipleOf must be a decimal value");
      this->minimum = jval.get_int64();
    }
  }
  if ( this->type.exists(schema_type::string) )
  {
    if ( jproperty.has_key("minLength", jval) )
    {
      if ( ! jval.is_unsigned() )
	throw sid::exception("minLength must be an unsigned value");
      this->minLength = jval.get_uint64();
    }
    if ( jproperty.has_key("maxLength", jval) )
    {
      if ( ! jval.is_unsigned() )
	throw sid::exception("maxLength must be an unsigned value");
      this->maxLength = jval.get_uint64();
    }
    if ( jproperty.has_key("pattern", jval) )
    {
      if ( ! jval.is_string() )
	throw sid::exception("pattern must be a string");
      this->pattern = jval.get_str();
    }
  }
  if ( this->type.exists(schema_type::array) )
  {
    if ( jproperty.has_key("minItems", jval) )
    {
      if ( ! jval.is_unsigned() )
	throw sid::exception("minItems must be an unsigned value");
      this->minItems = jval.get_uint64();
    }
    if ( jproperty.has_key("maxItems", jval) )
    {
      if ( ! jval.is_unsigned() )
	throw sid::exception("maxItems must be an unsigned value");
      this->maxItems = jval.get_uint64();
    }
    if ( jproperty.has_key("uniqueItems", jval) )
    {
      if ( ! jval.is_unsigned() )
	throw sid::exception("uniqueItems must be a boolean value");
      this->uniqueItems = jval.get_bool();
    }
    if ( jproperty.has_key("minContains", jval) )
    {
      if ( ! jval.is_unsigned() )
	throw sid::exception("minContains must be an unsigned value");
      this->minContains = jval.get_uint64();
    }
    if ( jproperty.has_key("maxContains", jval) )
    {
      if ( ! jval.is_unsigned() )
	throw sid::exception("maxContains must be an unsigned value");
      this->maxContains = jval.get_uint64();
    }
  }
  if ( this->type.exists(schema_type::object) )
  {
    if ( jproperty.has_key("minProperties", jval) )
    {
      if ( ! jval.is_unsigned() )
	throw sid::exception("minProperties must be an unsigned value");
      this->minProperties = jval.get_uint64();
    }
    if ( jproperty.has_key("maxProperties", jval) )
    {
      if ( ! jval.is_unsigned() )
	throw sid::exception("maxProperties must be an unsigned value");
      this->maxProperties = jval.get_uint64();
    }
  }
  if ( jproperty.has_key("properties", jval) )
  {
    if ( ! this->type.exists(schema_type::object) )
      throw sid::exception("properties is applicable only for object types. Key: ") + this->key;
    this->properties.set(jval);
  }
  if ( jproperty.has_key("required", jval) )
  {
    if ( ! this->type.exists(schema_type::object) )
      throw sid::exception("required is applicable only for object types for key ") + this->key;
    local::fill_required(this->required, jval, this->properties);
  }
}

std::string schema::to_str() const
{
  return to_json().to_str(format_type::pretty);
}

value schema::to_json() const
{
  value jroot;
  if ( ! this->_schema.empty() )
    jroot["$schema"] = this->_schema;
  if ( ! this->_id.empty() )
    jroot["$id"] = this->_id;
  if ( ! this->title.empty() )
    jroot["title"] = this->title;
  if ( ! this->description.empty() )
    jroot["description"] = this->description;
  if ( this->type.empty() )
    throw sid::exception("Schema type not set");
  jroot["type"] = this->type.to_json();
  if ( ! this->properties.empty() )
    jroot["properties"] = this->properties.to_json();
  for ( const std::string& req :this->required )
    jroot["required"].append(req);
  
  return jroot;
}

std::string schema::property_vec::to_str() const
{
  return to_json().to_str(format_type::pretty);
}

value schema::property_vec::to_json() const
{
  value jroot;

  for ( const schema::property& property : *this )
    jroot[property.key] = property.to_json();
  return jroot;
}

std::string schema::property::to_str() const
{
  return to_json().to_str(format_type::pretty);
}

value schema::property::to_json() const
{
  value jroot;
  
  if ( ! this->description.empty() )
    jroot["description"] = this->description;
  if ( this->type.empty() )
    throw sid::exception("Property type not set");
  jroot["type"] = this->type.to_json();

  if ( this->type.exists(schema_type::number) || this->type.exists(schema_type::integer) )
  {
    if ( this->minimum.exists() )
      jroot["minimum"] = this->minimum();
    if ( this->exclusiveMinimum.exists() )
      jroot["exclusiveMinimum"] = this->exclusiveMinimum();
    if ( this->maximum.exists() )
      jroot["maximum"] = this->maximum();
    if ( this->exclusiveMaximum.exists() )
      jroot["exclusiveMaximum"] = this->exclusiveMaximum();
    if ( this->multipleOf.exists() )
      jroot["multipleOf"] = this->maximum();
  }
  if ( this->type.exists(schema_type::string) )
  {
    if ( this->minLength.exists() )
      jroot["minLength"] = this->minLength();
    if ( this->maxLength.exists() )
      jroot["maxLength"] = this->maxLength();
    if ( ! this->pattern.empty() )
      jroot["pattern"] = this->pattern;
  }
  if ( this->type.exists(schema_type::array) )
  {
    if ( this->minItems.exists() )
      jroot["minItems"] = this->minItems();
    if ( this->maxItems.exists() )
      jroot["maxItems"] = this->maxItems();
    if ( this->uniqueItems.exists() )
      jroot["uniqueItems"] = this->uniqueItems();
    if ( this->minContains.exists() )
      jroot["minContains"] = this->minContains();
    if ( this->maxContains.exists() )
      jroot["maxContains"] = this->maxContains();
  }
  if ( this->type.exists(schema_type::object) )
  {
    if ( this->minProperties.exists() )
      jroot["minProperties"] = this->minProperties();
    if ( this->maxProperties.exists() )
      jroot["maxProperties"] = this->maxProperties();
    if ( ! this->properties.empty() )
      jroot["jproperties"] = this->properties.to_json();
    for ( const std::string& req : this->required )
      jroot["required"].append(req);
  }
  return jroot;
}

value schema_types::to_json() const
{
  value jroot;
  if ( this->size() == 1 )
    jroot = (*(this->begin())).name();
  else
  {
    for ( const schema_type& type : *this )
      jroot.append(type.name());
  }
  return jroot;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of schema_type
//
///////////////////////////////////////////////////////////////////////////////////////////////////
struct json_schema_type_map
{
  schema_type::ID type;
  const char*           name;
}
  static gSchemaTypeMap[] =
  {
    {schema_type::null,    "null"},
    {schema_type::object,  "object"},
    {schema_type::array,   "array"},
    {schema_type::string,  "string"},
    {schema_type::boolean, "boolean"},
    {schema_type::number,  "number"},
    {schema_type::integer, "integer"}
  };

/*static*/
bool schema_type::get(const std::string& _name, /*out*/ schema_type& _type)
{
  for ( const auto& entry : gSchemaTypeMap )
  {
    if ( _name == entry.name )
    {
      _type = entry.type;
      return true;
    }
  }
  return false;
}

/*static*/
schema_type schema_type::get(const std::string& _name)
{
  schema_type type;
  if ( !get(_name, /*out*/ type) )
    throw sid::exception("Invalid schema type [" + _name + "] encountered");
  return type;
}

std::string schema_type::name() const
{
  std::string name;
  for ( const auto& entry : gSchemaTypeMap )
  {
    if ( m_id == entry.type )
    {
      name = entry.name;
      break;
    }
  }
  return name;
}

void schema_types::add(const value& _value)
{
  if ( _value.is_string() )
    this->insert(schema_type::get(_value.get_str()));
  else if ( _value.is_array() )
  {
    // Each array element must be a string
    for ( size_t i = 0; i < _value.size(); i++ )
    {
      const value& jval = _value[i];
      if ( !jval.is_string() )
	throw sid::exception("type parameter must be strings within the array");
      schema_type type = schema_type::get(jval.get_str());
      if ( this->exists(type) )
	throw sid::exception("type parameters must be unique within the array");
      this->insert(type);
    }
  }
  else
    throw sid::exception("type parameter must be string or an array of unique string");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of local namespace
//
///////////////////////////////////////////////////////////////////////////////////////////////////
void local::fill_required(
  std::string_set&            _required,
  const value&                _jarray,
  const schema::property_vec& _properties
  )
{
  if ( ! _jarray.is_array() )
    throw sid::exception("required must be an array of strings");

  for ( size_t i = 0; i < _jarray.size(); i++ )
  {
    const value& jval = _jarray[i];
    if ( !jval.is_string() )
      throw sid::exception("required parameter must be strings within the array");

    const std::string key = jval.get_str();
    if ( _required.find(key) == _required.end() )
    {
      bool found = false;
      for ( const schema::property& property : _properties )
      {
	if ( key == property.key )
	{
	  found = true;
	  break;
	}
      }
      if ( ! found )
	throw sid::exception("key (" + key + ") marked as required is not found in properties");
      _required.insert(key);
    }
  }
}
