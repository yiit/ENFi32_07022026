#include "../Helpers/ESPEasy_key_value_store_import_export.h"

#if FEATURE_ESPEASY_KEY_VALUE_STORE
# include "../Helpers/_ESPEasy_key_value_store.h"
# include "../Helpers/KeyValueWriter.h"
# include "../Helpers/StringConverter.h"


ESPEasy_key_value_store_import_export::ESPEasy_key_value_store_import_export(
  ESPEasy_key_value_store*store,
  LabelStringFunction     fnc,
  NextKeyFunction         nextKey)
  : _store(store), _LabelStringFunction(fnc), _nextKey(nextKey) {}

bool ESPEasy_key_value_store_import_export::write(
  uint32_t       key,
  KeyValueWriter*writer) const
{
  if ((_store == nullptr) || (_LabelStringFunction == nullptr) || (writer == nullptr)) {
    return false;
  }
  KVS_StorageType::Enum storageType = KVS_StorageType::Enum::not_set;

  auto label = _LabelStringFunction(key, false, storageType);

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

  String tmp_json(json);

  String key_str;
  String value;

  while (getNextKeyValue(tmp_json, key_str, value)) {
    uint32_t key{};
    KVS_StorageType::Enum storageType;

    if (!findKey(key_str, key, storageType)) {
      key_str.toLowerCase();
      _parsedJSON[key_str] = value;
      /*
         return strformat(
         F("Unknown key: '%s' with value: '%s'"),
         key_str.c_str(),
         value.c_str());
       */
    } else {
      _store->setValue(storageType, key, value);
    }
  }

  _store->dump();
  return EMPTY_STRING;
}

bool ESPEasy_key_value_store_import_export::getNextKeyValue(String& json, String& key, String& value)
{
  json.trim();

  if (json.startsWith("{") && json.endsWith("}")) {
    json = json.substring(1, json.length() - 2);
  }
  json.trim();

  String keyValueStr = parseStringKeepCase(json, 1);

  if (keyValueStr.isEmpty()) {
    return false;
  }

  // KeyValueStr now contains something like this:
  // "key": value
  // "key": "value"

  key   = parseStringKeepCase(keyValueStr, 1, ':');
  value = parseStringKeepCase(keyValueStr, 2, ':');

  addLog(LOG_LEVEL_INFO, strformat(F("KVS : key:'%s' value:'%s'"), key.c_str(), value.c_str() ));

  // Strip found item off
  json = json.substring(keyValueStr.length());
  json.trim();

  if (json.startsWith(",")) {
    json = json.substring(1);
  }
  return true;
}

bool ESPEasy_key_value_store_import_export::findKey(
  const String         & key_str,
  uint32_t             & key,
  KVS_StorageType::Enum& storageType) const
{
  if ((_LabelStringFunction == nullptr) || (_nextKey == nullptr)) {
    return false;
  }
  key = _nextKey(-1);

  while (key >= 0) {
    storageType = KVS_StorageType::Enum::not_set;

    auto label = _LabelStringFunction(key, false, storageType);

    if (key_str.equalsIgnoreCase(label)) {
      return true;
    }
    key = _nextKey(key);
  }
  return false;
}

bool ESPEasy_key_value_store_import_export::getParsedJSON(const String& key, String& value) const
{
  String key_lc = key;
  key_lc.toLowerCase();
  auto it = _parsedJSON.find(key_lc);
  if (it == _parsedJSON.end()) return false;
  value = it->second;
  return true;  
}

#endif // if FEATURE_ESPEASY_KEY_VALUE_STORE
