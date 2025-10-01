#pragma once

#include "../../ESPEasy_common.h"

#include "../Helpers/StringProvider.h"

#include <vector>

struct ValueStruct
{
  ValueStruct() {}

  explicit ValueStruct(bool val);

  ValueStruct(int val);
  ValueStruct(uint32_t val);
  explicit ValueStruct(uint64_t val);
  explicit ValueStruct(int64_t val);
  ValueStruct(const __FlashStringHelper *val);

  template<typename T>
  ValueStruct(const T& val) : str(val) {}

  String str;
  bool   isBoolean{};
  bool   isInt{};

};

struct KeyValueStruct
{

  KeyValueStruct() {}

  KeyValueStruct(const __FlashStringHelper *key);
  KeyValueStruct(const String& key);


  KeyValueStruct(const __FlashStringHelper *key,
                 ValueStruct                value);

  KeyValueStruct(const String& key,
                 ValueStruct   value);


  KeyValueStruct(const __FlashStringHelper *key,
                 ValueStruct                value,
                 const __FlashStringHelper *unit);

  KeyValueStruct(const __FlashStringHelper *key,
                 ValueStruct                value,
                 const String             & unit);

  KeyValueStruct(const String& key,
                 ValueStruct   value,
                 const String& unit);

  KeyValueStruct(LabelType::Enum label);

  void appendValue(const ValueStruct& value);

  void appendValue(ValueStruct&& value);


  String _key;
  String _id;
  String _unit;

  std::vector<ValueStruct>_values;

  // output as pre-formatted monospaced
  bool _value_pre{};


};

class KeyValueWriter
{
public:

  virtual ~KeyValueWriter() {}

  virtual void clear();

  virtual void write(const KeyValueStruct& kv) = 0;

  void writeLabels(const LabelType::Enum labels[]);

}; // class KeyValueWriter
