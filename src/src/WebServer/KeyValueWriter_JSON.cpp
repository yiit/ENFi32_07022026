#include "../WebServer/KeyValueWriter_JSON.h"

#include "../Globals/Settings.h"
#include "../Helpers/StringConverter.h"

#include "../WebServer/HTML_wrappers.h"

KeyValueWriter_JSON::KeyValueWriter_JSON(bool emptyHeader)
  : KeyValueWriter(emptyHeader)
{}

KeyValueWriter_JSON::KeyValueWriter_JSON(KeyValueWriter_JSON*parent)
  : KeyValueWriter(parent)
{}

KeyValueWriter_JSON::KeyValueWriter_JSON(bool emptyHeader, KeyValueWriter_JSON*parent)
  : KeyValueWriter(emptyHeader, parent)
{}

KeyValueWriter_JSON::KeyValueWriter_JSON(const String& header)
  : KeyValueWriter(header, nullptr)
{}

KeyValueWriter_JSON::KeyValueWriter_JSON(const __FlashStringHelper *header)
  : KeyValueWriter(String(header), nullptr)
{}


KeyValueWriter_JSON::KeyValueWriter_JSON(const String& header, KeyValueWriter_JSON*parent)
  : KeyValueWriter(header, parent)
{}

KeyValueWriter_JSON::KeyValueWriter_JSON(const __FlashStringHelper *header, KeyValueWriter_JSON*parent)
  : KeyValueWriter(String(header), parent)
{}

KeyValueWriter_JSON::~KeyValueWriter_JSON()
{
  if (!_isEmpty) {
    addHtml('\n');

    if (_hasHeader) {
#ifdef USE_KVW_JSON_INDENT
      indent();
#endif

      addHtml(_isArray ? ']' : '}');
    }
  }
}

void KeyValueWriter_JSON::setHeader(const String& header)
{
  _header    = header;
  _hasHeader = true;
}

void KeyValueWriter_JSON::clear() { _isEmpty = true; }

void KeyValueWriter_JSON::write()
{
  if (_isEmpty) {
    if (_parent != nullptr) { _parent->write(); }

    if (_hasHeader) {
#ifdef USE_KVW_JSON_INDENT
      indent();
#endif

      if (_header.isEmpty()) {
        addHtml('{', '\n');
      } else {
        addHtml(strformat(
                  F("\"%s\":%c\n"),
                  _header.c_str(),
                  _isArray ? '[' : '{'));
      }
    }
    _isEmpty = false;
  } else {
    addHtml(',', '\n');
  }
}

void KeyValueWriter_JSON::write(const KeyValueStruct& kv)
{
  write();
#ifdef USE_KVW_JSON_INDENT
  indent();
#endif

  if (kv._key.length()) {
    addHtml('"');
    addHtml(kv._key);
    addHtml('"', ':');
  }

  const size_t nrValues = kv._values.size();

  if (!kv._isArray) {
    // Either 1 value or empty value
    if (nrValues == 0) {
      addHtml('"', '"');
    }
    else {
      writeValue(kv._values[0]);
    }
  } else {
    // Multiple values, so we must wrap it in []
    addHtml('[', '\n');

    for (size_t i = 0; i < nrValues; ++i) {
      if (i != 0) {
        addHtml(',', '\n');
      }
#ifdef USE_KVW_JSON_INDENT
      indent();
      addHtml('\t');
#endif // ifdef USE_KVW_JSON_INDENT

      writeValue(kv._values[i]);
    }
    addHtml(']');
  }
}

void KeyValueWriter_JSON::writeValue(const ValueStruct& val)
{
  switch (val.valueType)
  {
    case ValueStruct::ValueType::Float:
    case ValueStruct::ValueType::Double:
    case ValueStruct::ValueType::Int:
      addHtml(val.str);
      return;
    case ValueStruct::ValueType::Bool:

      if (Settings.JSONBoolWithoutQuotes())
      {
        addHtml(val.str.equals("0") ? F("false") : F("true"));
        return;
      }

    // Fall through
    case ValueStruct::ValueType::Auto:
    case ValueStruct::ValueType::String:
      break;
  }
  addHtml(to_json_value(val.str));
}

#ifdef USE_KVW_JSON_INDENT

void KeyValueWriter_JSON::indent() const
{
  if (_parent != nullptr) {
    addHtml('\t');
    _parent->indent();
  }
}

#endif // ifdef USE_KVW_JSON_INDENT
