#include "../WebServer/KeyValueWriter_JSON.h"

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
      indent();
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
      indent();

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
  indent();
  addHtml('"');
  addHtml(kv._key);
  addHtml('"', ':');

  const size_t nrValues = kv._values.size();

  if (nrValues < 2) {
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
      indent();
      writeValue(kv._values[i]);
    }
    indent();
    addHtml(']', '\n');
  }
}

void KeyValueWriter_JSON::writeValue(const ValueStruct& val)
{
  if (val.isInt) {
    addHtml(val.str);
  } else if (val.isBoolean) {
    addHtml(val.str.equals("0") ? F("false") : F("true"));
  } else {
    addHtml(to_json_value(val.str));
  }
}

void KeyValueWriter_JSON::indent() const
{
  const int level = getLevel();

  for (int i = 0; i < level; ++i) {
    addHtml('\t');
  }
}
