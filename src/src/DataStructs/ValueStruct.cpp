#include "../DataStructs/ValueStruct.h"

#include "../Helpers/Memory.h"
#include "../Helpers/PrintToString.h"
#include "../Helpers/StringConverter_Numerical.h"

// ********************************************************************************
// ValueStruct
// ********************************************************************************
ValueStruct::~ValueStruct()
{
  if (static_cast<ValueStruct::ValueType>(valueType) == ValueStruct::ValueType::String)
  {
    if (str_val != nullptr) {
      free(str_val);
    }
  }
}

ValueStruct::ValueStruct(ValueStruct&& rhs)
{
  memcpy(bytes_metadata, rhs.bytes_metadata, sizeof(bytes_metadata));
  memcpy(bytes_val,      rhs.bytes_val,      sizeof(bytes_val));
  memset(rhs.bytes_metadata, 0, sizeof(bytes_metadata));
  memset(rhs.bytes_val,      0, sizeof(bytes_val));
}

ValueStruct::ValueStruct(const bool& val) :
  valueType((uint64_t)ValueStruct::ValueType::Bool),
  size(1),
  u64_val(val ? 1ull : 0ull)
{}

ValueStruct::ValueStruct(int val) :
  valueType((uint64_t)ValueStruct::ValueType::Int),
  size(sizeof(val) * 8),
  i64_val(val)
{}

#if defined(ESP32) && !defined(__riscv)
ValueStruct::ValueStruct(int32_t val) :
  valueType((uint64_t)ValueStruct::ValueType::Int),
  size(sizeof(val) * 8),
  i64_val(val)
{}

#endif // if defined(ESP32) && !defined(__riscv)
ValueStruct::ValueStruct(uint32_t val) :
  valueType((uint64_t)ValueStruct::ValueType::UInt),
  size(sizeof(val) * 8),
  u64_val(val)
{}

#if defined(ESP32) && !defined(__riscv)
ValueStruct::ValueStruct(size_t val) :
  valueType((uint64_t)ValueStruct::ValueType::UInt),
  size(sizeof(val) * 8),
  u64_val(val)
{}

#endif // if defined(ESP32) && !defined(__riscv)
ValueStruct::ValueStruct(const uint64_t& val) :
  valueType((uint64_t)ValueStruct::ValueType::UInt),
  size(sizeof(val) * 8),
  u64_val(val)
{}

ValueStruct::ValueStruct(const int64_t& val) :
  valueType((uint64_t)ValueStruct::ValueType::Int),
  size(sizeof(val) * 8),
  i64_val(val)
{}

ValueStruct::ValueStruct(const float& val,
                         uint8_t      nrDecimals,
                         bool         trimTrailingZeros) :
  nrDecimals((uint64_t)nrDecimals),
  trimTrailingZeros((uint64_t)trimTrailingZeros),
  valueType((uint64_t)ValueStruct::ValueType::Float),
  size(sizeof(val) * 8),
  f_val(val)
{}

ValueStruct::ValueStruct(const double& val,
                         uint8_t       nrDecimals,
                         bool          trimTrailingZeros) :
  nrDecimals((uint64_t)nrDecimals),
  trimTrailingZeros((uint64_t)trimTrailingZeros),
  valueType((uint64_t)ValueStruct::ValueType::Double),
  size(sizeof(val) * 8),
  d_val(val)
{}

ValueStruct::ValueStruct(const char*val) :
  valueType((uint64_t)ValueStruct::ValueType::String),
  size(val ? strlen_P((const char *)(val)) : 0),
  str_val(nullptr)
{
  if (size) {
    str_val = special_calloc(1, size + 1);

    if (str_val) {
      memcpy_P(str_val, val, size);
    }
  }
}

ValueStruct::ValueStruct(const String& val) :
  valueType((uint64_t)ValueStruct::ValueType::String),
  size(val.length()),
  str_val(nullptr)
{
  if (size) {
    str_val = special_calloc(1, size + 1);

    if (str_val) {
      memcpy_P(str_val, val.begin(), size);
    }
  }
}

ValueStruct::ValueStruct(String&& val) :
  valueType((uint64_t)ValueStruct::ValueType::String),
  size(val.length()),
  str_val(nullptr)
{
  // We can't move the allocated memory from 'message'.
  // Just use move so we make sure the memory is de-allocated after this call.
  const String str = std::move(val);

  if (size) {
    str_val = special_calloc(1, size + 1);

    if (str_val) {
      memcpy_P(str_val, str.begin(), size);
    }
  }
}

ValueStruct::ValueStruct(const __FlashStringHelper *val) :
  valueType((uint64_t)ValueStruct::ValueType::FlashString),
  size(val ? strlen_P((const char *)(val)) : 0),
  str_val((void *)(val))
{}

String ValueStruct::toString() const
{
  PrintToString p;

  print(p);
  String res(p.getMove());
  return res;
}

String ValueStruct::toString(ValueType& valueType) const
{
  valueType = getValueType();
  return toString();
}

size_t ValueStruct::print(Print& out) const
{
  ValueStruct::ValueType v;

  return print(out, v);
}

size_t ValueStruct::print(Print& out, ValueType& valueType) const
{
  valueType = getValueType();

  switch (valueType)
  {
    case ValueStruct::ValueType::Bool:
    {
      const bool v = u64_val != 0;
      return out.print(u64_val == 0 ? 0 : 1);
    }
    case ValueStruct::ValueType::String:
    {
      if (str_val == nullptr) { return 0; }
      return out.write((const uint8_t *)str_val, size);
    }
    case ValueStruct::ValueType::FlashString:
    {
      if (str_val == nullptr) { return 0; }
      return out.print((const __FlashStringHelper *)str_val);
    }
    case ValueStruct::ValueType::Float:
    {
      String res;

      if (!toValidString(res, f_val, nrDecimals, trimTrailingZeros))
      {
        valueType = ValueStruct::ValueType::String;
      }
      return out.print(res);
    }
    case ValueStruct::ValueType::Double:
    {
      String res;

      if (!doubleToValidString(res, d_val, nrDecimals, trimTrailingZeros))
      {
        valueType = ValueStruct::ValueType::String;
      }
      return out.print(res);
    }
    case ValueStruct::ValueType::Int:
    {
      if (size == 8) {
        return out.print(ll2String(i64_val));
      }
      auto v = static_cast<int32_t>(i64_val);
      return out.print(v);
    }
    case ValueStruct::ValueType::UInt:
    {
      if (size == 8) {
        return out.print(ull2String(u64_val));
      }
      auto v = static_cast<uint32_t>(u64_val);
      return out.print(v);
    }
    case ValueStruct::ValueType::Unset:  break;
  }
  return 0;
}

bool ValueStruct::isEmpty() const
{
  switch (getValueType())
  {
    case ValueStruct::ValueType::String:
    case ValueStruct::ValueType::FlashString:
      return size == 0;
    case ValueStruct::ValueType::Unset:
      return true;
    default: break;
  }
  return false;
}
