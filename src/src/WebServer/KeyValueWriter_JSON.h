#pragma once

#include "../Helpers/KeyValueWriter.h"

class KeyValueWriter_JSON : public KeyValueWriter
{
public:

  using KeyValueWriter::writeLabels;

  KeyValueWriter_JSON() {}

  KeyValueWriter_JSON(const String& header);
  KeyValueWriter_JSON(const __FlashStringHelper *header);

  virtual ~KeyValueWriter_JSON();

  virtual void clear() override;

  virtual void write(const KeyValueStruct& kv);

private:

  void writeValue(const ValueStruct& val);

  bool _hasHeader{};


  bool _isFirst = true;
}; // class KeyValueWriter_JSON
