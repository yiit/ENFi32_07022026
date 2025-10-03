#pragma once

#include "../../ESPEasy_common.h"

#include "../Helpers/StringProvider.h"

#include <vector>

struct ValueStruct
{
  enum class ValueType {
    Auto,
    String,
    Float,
    Double,
    Int,
    Bool

  };


  ValueStruct() {}

  explicit ValueStruct(const uint64_t& val,
                       ValueType       vType = ValueType::Int);
  explicit ValueStruct(const int64_t& val,
                       ValueType      vType = ValueType::Int);
  ValueStruct(const String& val,
              ValueType     vType = ValueType::Auto);
  ValueStruct(const __FlashStringHelper *val,
              ValueType                  vType = ValueType::Auto);
  ValueStruct(String && val,
              ValueType vType = ValueType::Auto);


  String str;
  ValueType valueType = ValueType::Auto;

};

struct KeyValueStruct
{

  KeyValueStruct() {}

   KeyValueStruct(const __FlashStringHelper *key);
   KeyValueStruct(const String& key);

  /*
     KeyValueStruct(const __FlashStringHelper *key,
                   ValueStruct             && value);

     KeyValueStruct(const String& key,
                   ValueStruct&& value);
   */

  template<typename T>
  KeyValueStruct(const String         & key,
                 const T              & val,
                 ValueStruct::ValueType vType = ValueStruct::ValueType::Auto)
    : _key(key) {
    _values.emplace_back(String(val), vType);
  }

  KeyValueStruct(const String         & key,
                 bool                   val,
                 ValueStruct::ValueType vType = ValueStruct::ValueType::Bool);

  KeyValueStruct(const String         & key,
                 int                    val,
                 ValueStruct::ValueType vType = ValueStruct::ValueType::Int);

  KeyValueStruct(const String         & key,
                 const float&           val,
                 int nrDecimals = 4,
                 ValueStruct::ValueType vType = ValueStruct::ValueType::Float);

  KeyValueStruct(const String         & key,
                 const double&          val,
                 int nrDecimals = 4,
                 ValueStruct::ValueType vType = ValueStruct::ValueType::Double);


  KeyValueStruct(const __FlashStringHelper *key,
                 const String             & val,
                 ValueStruct::ValueType     vType = ValueStruct::ValueType::Auto);

  KeyValueStruct(const __FlashStringHelper *key,
                 const __FlashStringHelper *val,
                 ValueStruct::ValueType     vType = ValueStruct::ValueType::Auto);

  KeyValueStruct(const String         & key,
                 const String         & val,
                 ValueStruct::ValueType vType = ValueStruct::ValueType::Auto);

  KeyValueStruct(const __FlashStringHelper *key,
                 String              && val,
                 ValueStruct::ValueType vType = ValueStruct::ValueType::Auto);


  KeyValueStruct(const String         & key,
                 String              && val,
                 ValueStruct::ValueType vType = ValueStruct::ValueType::Auto);

  KeyValueStruct(LabelType::Enum label);

  void setUnit(const String& unit);
  void setUnit(const __FlashStringHelper *unit);

  void appendValue(const ValueStruct& value);

  void appendValue(ValueStruct&& value);


  String _key;
  String _id;
  String _unit;

  std::vector<ValueStruct>_values;

  // output as pre-formatted monospaced
  bool _value_pre{};
  bool _isArray{};

};

class KeyValueWriter
{
public:

  KeyValueWriter(bool emptyHeader = false) : _hasHeader(emptyHeader) {}

protected:

  KeyValueWriter(KeyValueWriter*parent) : _parent(parent) {}

  KeyValueWriter(bool emptyHeader, KeyValueWriter*parent) : _parent(parent), _hasHeader(emptyHeader) {}

  KeyValueWriter(const String& header, KeyValueWriter*parent) : _header(header), _parent(parent) {}

public:

  virtual ~KeyValueWriter() {}

  virtual void setHeader(const String& header) {
    _header    = header;
    _hasHeader = true;
  }

  virtual void clear();

  // Mark a write, typically called from a child calling its parent it is about to write
  virtual void write() = 0;

  virtual void write(const KeyValueStruct& kv) = 0;

  void         writeLabels(const LabelType::Enum labels[]);

  //  virtual void setParent(KeyValueWriter*parent) { _parent = parent; }

  virtual int  getLevel() const;

  virtual void setIsArray()   { _isArray = true; }

  virtual void indent() const {}

protected:

  String _header;

  KeyValueWriter *_parent{};

  bool _hasHeader = true;

  bool _isEmpty = true;

  bool _isArray{};


}; // class KeyValueWriter
