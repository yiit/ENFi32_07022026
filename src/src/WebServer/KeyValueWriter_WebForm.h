#pragma once

#include "../Helpers/KeyValueWriter.h"

class KeyValueWriter_WebForm : public KeyValueWriter
{
public:

  using KeyValueWriter::writeLabels;

  KeyValueWriter_WebForm() {}

  KeyValueWriter_WebForm(const String& header);
  KeyValueWriter_WebForm(const __FlashStringHelper *header);


  virtual ~KeyValueWriter_WebForm() {}


  virtual void write(const KeyValueStruct& kv);


}; // class KeyValueWriter_WebForm
