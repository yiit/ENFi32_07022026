#include "../WebServer/KeyValueWriter_JSON.h"

#include "../Helpers/StringConverter.h"

#include "../WebServer/HTML_wrappers.h"

KeyValueWriter_JSON::KeyValueWriter_JSON(const String& header)
  : _hasHeader(true)
{
  addHtml(strformat(F("\"%s\":{\n"), header.c_str()));
}

KeyValueWriter_JSON::KeyValueWriter_JSON(const __FlashStringHelper *header)
  : _hasHeader(true)
{
  addHtml(strformat(F("\"%s\":{\n"), FsP(header)));
}

KeyValueWriter_JSON::~KeyValueWriter_JSON()
{
  if (_hasHeader) { addHtml('}', '\n'); }
}

void KeyValueWriter_JSON::clear() { _isFirst = true; }

void KeyValueWriter_JSON::write(const KeyValueStruct& kv)
{
  if (!_isFirst) {
    addHtml(',', '\n');
  }
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
      writeValue(kv._values[i]);
    }
    addHtml(']', '\n');
  }

  _isFirst = false;
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
