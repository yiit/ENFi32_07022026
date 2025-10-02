#include "../Helpers/KeyValueWriter.h"

#include "../Helpers/StringConverter_Numerical.h"

ValueStruct::ValueStruct(bool val) : str(val), isBoolean(true) {}

ValueStruct::ValueStruct(int val) : str(val), isInt(true) {}

ValueStruct::ValueStruct(uint32_t val) : str(val), isInt(true) {}

ValueStruct::ValueStruct(uint64_t val) : str(ull2String(val)), isInt(true) {}

ValueStruct::ValueStruct(int64_t val) : str(ll2String(val)), isInt(true) {}

ValueStruct::ValueStruct(const __FlashStringHelper *val) : str(val) {}


KeyValueStruct::KeyValueStruct(const __FlashStringHelper *key) : _key(key) {}

KeyValueStruct::KeyValueStruct(const String& key) : _key(key) {}

KeyValueStruct::KeyValueStruct(const __FlashStringHelper *key,
                               ValueStruct                value)
  : _key(key) {
  _values.emplace_back(std::move(value));
}

KeyValueStruct::KeyValueStruct(const String& key,
                               ValueStruct   value)
  : _key(key) {
  _values.emplace_back(std::move(value));
}

KeyValueStruct::KeyValueStruct(const __FlashStringHelper *key,
                               ValueStruct                value,
                               const __FlashStringHelper *unit)
  : _key(key), _unit(unit) {
  _values.emplace_back(std::move(value));
}

KeyValueStruct::KeyValueStruct(const __FlashStringHelper *key,
                               ValueStruct                value,
                               const String             & unit)
  : _key(key), _unit(unit) {
  _values.emplace_back(std::move(value));
}

KeyValueStruct::KeyValueStruct(const String& key,
                               ValueStruct   value,
                               const String& unit)
  : _key(key), _unit(unit) {
  _values.emplace_back(std::move(value));
}

KeyValueStruct::KeyValueStruct(LabelType::Enum label)
  : _key(getLabel(label)), _unit(getFormUnit(label)) {
  _values.emplace_back(getValue(label));
}

void KeyValueStruct::appendValue(const ValueStruct& value) { _values.emplace_back(std::move(value)); }

void KeyValueStruct::appendValue(ValueStruct&& value)      { _values.emplace_back(std::move(value)); }

void KeyValueWriter::clear()                               {}

void KeyValueWriter::writeLabels(const LabelType::Enum labels[])
{
  size_t i            = 0;
  LabelType::Enum cur = static_cast<const LabelType::Enum>(pgm_read_byte(labels + i));

  while (cur != LabelType::MAX_LABEL) {
    write(cur);
    cur = static_cast<const LabelType::Enum>(pgm_read_byte(labels + i + 1));
    ++i;
  }
}

int KeyValueWriter::getLevel() const
{
  if (_parent == nullptr) { return 0; }
  return _parent->getLevel() + 1;
}
