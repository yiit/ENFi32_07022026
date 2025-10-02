#pragma once

#include "../Helpers/KeyValueWriter.h"

class KeyValueWriter_JSON : public KeyValueWriter
{
public:

  using KeyValueWriter::writeLabels;

  KeyValueWriter_JSON(bool emptyHeader = false);

  KeyValueWriter_JSON(KeyValueWriter_JSON*parent);

  KeyValueWriter_JSON(bool                emptyHeader,
                      KeyValueWriter_JSON*parent);

  KeyValueWriter_JSON(const String& header);
  KeyValueWriter_JSON(const __FlashStringHelper *header);

  KeyValueWriter_JSON(const String      & header,
                      KeyValueWriter_JSON*parent);
  KeyValueWriter_JSON(const __FlashStringHelper *header,
                      KeyValueWriter_JSON       *parent);

  virtual ~KeyValueWriter_JSON();

  virtual void setHeader(const String& header);

  void         setIsArray() { _isArray = true; }

  virtual void clear() override;

  virtual void write();

  virtual void write(const KeyValueStruct& kv);

private:

  void writeValue(const ValueStruct& val);

  void indent() const;

  bool _isArray{};

}; // class KeyValueWriter_JSON
