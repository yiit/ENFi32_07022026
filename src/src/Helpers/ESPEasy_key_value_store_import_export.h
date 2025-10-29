#pragma once
#include "../../ESPEasy_common.h"

#if FEATURE_ESPEASY_KEY_VALUE_STORE
# include "../DataTypes/ESPEasy_key_value_store_StorageType.h"

#include <map>

class ESPEasy_key_value_store;
class KeyValueWriter;

class ESPEasy_key_value_store_import_export
{
public:

  using LabelStringFunction = const __FlashStringHelper * (*)(uint32_t, bool, KVS_StorageType::Enum&);


  // When queried with a key of -1, it will return the first key index
  // Return next key, or -2 when no next key exists.
  using NextKeyFunction       = int32_t (*)(int32_t);

  ESPEasy_key_value_store_import_export(
    ESPEasy_key_value_store*store,
    LabelStringFunction  fnc,
    NextKeyFunction        nextKey);

  bool   write(uint32_t       key,
               KeyValueWriter*writer) const;
  String read(const String& json);

  static bool getNextKeyValue(String& json, String& key, String& value);

  bool getParsedJSON(const String& key, String& value) const;

private:

  std::map<String, String> _parsedJSON;

  bool findKey(
    const String         & key_str,
    uint32_t             & key,
    KVS_StorageType::Enum& storageType) const;

  ESPEasy_key_value_store*_store{};
  LabelStringFunction _LabelStringFunction{};
  NextKeyFunction _nextKey{};

}; // class ESPEasy_key_value_store_import_export

#endif // if FEATURE_ESPEASY_KEY_VALUE_STORE
