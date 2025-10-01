#include "../WebServer/KeyValueWriter_WebForm.h"


#include "../WebServer/HTML_wrappers.h"
#include "../WebServer/Markup.h"

KeyValueWriter_WebForm::KeyValueWriter_WebForm(const String& header)
{
  addFormSubHeader(header);
}

KeyValueWriter_WebForm::KeyValueWriter_WebForm(const __FlashStringHelper *header)
{
  addFormSubHeader(header);
}

void KeyValueWriter_WebForm::write(const KeyValueStruct& kv)
{
  addRowLabel(kv._key, kv._id);
  const size_t nrValues   = kv._values.size();
  const bool   format_pre = nrValues > 1 || kv._value_pre;

  if (format_pre) { addHtml(F("<pre>")); }

  for (size_t i = 0; i < nrValues; ++i) {
    if (i != 0) {
      addHtml(F("<br>"));
    }
    addHtml(kv._values[i].str);
  }

  if (format_pre) { addHtml(F("</pre>")); }

  if (nrValues == 1) {
    addUnit(kv._unit);
  }
}
