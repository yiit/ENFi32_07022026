#include "../Helpers/ESPEasy_key_value_store_import_export.h"

#if FEATURE_ESPEASY_KEY_VALUE_STORE
# include "../Helpers/_ESPEasy_key_value_store.h"
# include "../Helpers/KeyValueWriter.h"
# include "../Helpers/StringConverter.h"

ESPEasy_key_value_store_import_export::ESPEasy_key_value_store_import_export(
    ESPEasy_key_value_store*store,
    getLabelStringFunction  fnc,
    uint32_t maxKey)
: _store(store), _getLabelStringFunction(fnc), _maxKey(maxKey) {}


ESPEasy_key_value_store_import_export::ESPEasy_key_value_store_import_export(
  ESPEasy_key_value_store*store,
  getLabelStringFunction  fnc,
  validKeyFunction        validKey)
  : _store(store), _getLabelStringFunction(fnc), _validKey(validKey) {}

bool ESPEasy_key_value_store_import_export::write(
  uint32_t       key,
  KeyValueWriter*writer) const
{
  if ((_store == nullptr) || (_getLabelStringFunction == nullptr) || (writer == nullptr)) {
    return false;
  }
  KVS_StorageType::Enum storageType = KVS_StorageType::Enum::not_set;

  auto label = _getLabelStringFunction(key, false, storageType);

  if (!_store->hasKey(storageType, key)) {
    return false;
  }

  switch (storageType)
  {
    case KVS_StorageType::Enum::not_set:
    case KVS_StorageType::Enum::MAX_Type:
      break;
    case KVS_StorageType::Enum::binary:
      // TODO TD-er: Implement
      break;
    case KVS_StorageType::Enum::bool_type:
    case KVS_StorageType::Enum::bool_true:
    case KVS_StorageType::Enum::bool_false:
    {
      bool value{};

      if (_store->getValue(key, value)) {
        writer->write({ label, value });
        return true;
      }
      break;
    }
    case KVS_StorageType::Enum::float_type:
    {
      float value{};

      if (_store->getValue(key, value)) {
        writer->write({ label, value });
        return true;
      }
      break;
    }
    case KVS_StorageType::Enum::double_type:
    {
      double value{};

      if (_store->getValue(key, value)) {
        writer->write({ label, value });
        return true;
      }
      break;
    }
    default:
    {
      String value;

      if (_store->getValueAsString(key, value)) {
        writer->write({ label, value });
        return true;
      }
      break;
    }
  }
  return false;
}

String ESPEasy_key_value_store_import_export::read(const String& json)
{
  if (_store == nullptr) {
    return F("No Store Set");
  }

  if (!_store->isEmpty()) {
    return F("Store not empty");
  }

  String tmp(json);
  tmp.trim();

  if (tmp.startsWith("{") && tmp.endsWith("}")) {
    tmp = tmp.substring(1, tmp.length() - 2);
  }

  int index = 1;

  do
  {
    String keyValueStr = parseStringKeepCase(tmp, index);

    if (!keyValueStr.isEmpty()) {
      ++index;

      // KeyValueStr now contains something like this:
      // "key": value
      // "key": "value"

      String   key_str = parseStringKeepCase(keyValueStr, 1, ':');
      String   value   = parseStringKeepCase(keyValueStr, 2, ':');
      uint32_t key{};
      KVS_StorageType::Enum storageType;

      if (!findKey(key_str, key, storageType)) {
        return strformat(
          F("Unknown key: '%s' with value: '%s'"),
          key_str.c_str(),
          value.c_str());
      }
      _store->setValue(storageType, key, value);
    } else { index = -1; }

  } while (index > 0);

  _store->dump();
  return EMPTY_STRING;
}

bool ESPEasy_key_value_store_import_export::findKey(
  const String         & key_str,
  uint32_t             & key,
  KVS_StorageType::Enum& storageType) const
{
  if ((_getLabelStringFunction == nullptr)) {
    return false;
  }
  key = 0;

  while (validKey(key)) {
    storageType = KVS_StorageType::Enum::not_set;

    auto label = _getLabelStringFunction(key, false, storageType);

    if (key_str.equalsIgnoreCase(label)) {
      return true;
    }
    ++key;
  }
  return false;
}

bool ESPEasy_key_value_store_import_export::validKey(uint32_t key) const
{
  if (_validKey) {
    return _validKey(key);
  }
  return key <= _maxKey && _maxKey > 0;
}

#endif // if FEATURE_ESPEASY_KEY_VALUE_STORE
