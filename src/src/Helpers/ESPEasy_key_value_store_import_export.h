#pragma once
#include "../../ESPEasy_common.h"

#if FEATURE_ESPEASY_KEY_VALUE_STORE
# include "../DataTypes/ESPEasy_key_value_store_StorageType.h"

class ESPEasy_key_value_store;
class KeyValueWriter;

class ESPEasy_key_value_store_import_export
{
public:

  using getLabelStringFunction = const __FlashStringHelper * (*)(uint32_t, bool, KVS_StorageType::Enum&);
  using validKeyFunction       = bool (*)(uint32_t);

  ESPEasy_key_value_store_import_export(
    ESPEasy_key_value_store*store,
    getLabelStringFunction  fnc,
    uint32_t maxKey);


  ESPEasy_key_value_store_import_export(
    ESPEasy_key_value_store*store,
    getLabelStringFunction  fnc,
    validKeyFunction        validKey);

  bool   write(uint32_t       key,
               KeyValueWriter*writer) const;
  String read(const String& json);

private:

  bool findKey(
    const String         & key_str,
    uint32_t             & key,
    KVS_StorageType::Enum& storageType) const;

  bool validKey(uint32_t key) const;

  ESPEasy_key_value_store*_store{};
  getLabelStringFunction _getLabelStringFunction{};
  validKeyFunction _validKey{};

  uint32_t _maxKey{};

}; // class ESPEasy_key_value_store_import_export

#endif // if FEATURE_ESPEASY_KEY_VALUE_STORE
