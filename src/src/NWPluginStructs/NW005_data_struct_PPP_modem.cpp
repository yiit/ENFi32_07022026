#include "../NWPluginStructs/NW005_data_struct_PPP_modem.h"

#ifdef USES_NW005

# include "../Helpers/StringConverter.h"
# include "../Helpers/_Plugin_Helper_serial.h"

# include "../WebServer/HTML_wrappers.h"
# include "../WebServer/Markup.h"
# include "../WebServer/Markup_Forms.h"
# include "../WebServer/ESPEasy_key_value_store_webform.h"

# include <ESPEasySerialPort.h>
# include <PPP.h>

//# include <esp_modem_api.h>
//#include <esp_modem_c_api_types.h>

// Keys as used in the Key-value-store
# define NW005_KEY_SERIAL_PORT          1
# define NW005_KEY_PIN_RX               2
# define NW005_KEY_PIN_TX               3
# define NW005_KEY_PIN_RTS              4
# define NW005_KEY_PIN_CTS              5
# define NW005_KEY_PIN_RESET            6
# define NW005_KEY_PIN_RESET_ACTIVE_LOW 7
# define NW005_KEY_PIN_RESET_DELAY      8
# define NW005_KEY_BAUDRATE             9
# define NW005_KEY_FLOWCTRL             10
# define NW005_KEY_MODEM_MODEL          11
# define NW005_KEY_APN                  12
# define NW005_KEY_PIN                  13

const __FlashStringHelper* NW005_getLabelString(uint32_t key, bool displayString, ESPEasy_key_value_store::StorageType& storageType)
{
  storageType = ESPEasy_key_value_store::StorageType::int8_type;

  switch (key)
  {
    case NW005_KEY_SERIAL_PORT:
      return displayString ? F("Serial Port") : F("serPort");
    case NW005_KEY_PIN_RX: return F("RX");
    case NW005_KEY_PIN_TX: return F("TX");
    case NW005_KEY_PIN_RTS: return F("RTS");
    case NW005_KEY_PIN_CTS: return F("CTS");
    case NW005_KEY_PIN_RESET: return displayString ? F("Reset") : F("rst");
    case NW005_KEY_PIN_RESET_ACTIVE_LOW:
      storageType = ESPEasy_key_value_store::StorageType::bool_type;
      return displayString ? F("Reset Active Low") : F("rst_act_low");
    case NW005_KEY_PIN_RESET_DELAY:
      storageType = ESPEasy_key_value_store::StorageType::uint16_type;
      return displayString ? F("Reset Delay") : F("rst_delay");
    case NW005_KEY_BAUDRATE:
      storageType = ESPEasy_key_value_store::StorageType::uint32_type;
      return displayString ? F("Baud rate") : F("baudrate");
    case NW005_KEY_FLOWCTRL:
      return displayString ? F("Flow Control") : F("flowctrl");
    case NW005_KEY_MODEM_MODEL: return displayString ? F("Modem Model") : F("mmodel");
    case NW005_KEY_APN:
      storageType = ESPEasy_key_value_store::StorageType::string_type;
      return displayString ? F("Access Point Name (APN)") : F("apn");
    case NW005_KEY_PIN:
      storageType = ESPEasy_key_value_store::StorageType::string_type;
      return displayString ? F("SIM Card Pin") : F("pin");

  }
  return F("");
}

NW005_data_struct_PPP_modem::NW005_data_struct_PPP_modem(networkIndex_t networkIndex)
  : NWPluginData_base(nwpluginID_t(5), networkIndex)
{}

WebFormItemParams NW005_makeWebFormItemParams(uint32_t key) {
  ESPEasy_key_value_store::StorageType storageType;
  const __FlashStringHelper*label = NW005_getLabelString(key, true, storageType);
  const __FlashStringHelper*id    = NW005_getLabelString(key, false, storageType);

  return WebFormItemParams(label, id, storageType, key);
}

NW005_data_struct_PPP_modem::~NW005_data_struct_PPP_modem() {
  PPP.end();
  _modem_initialized = false;
}

enum class NW005_modem_model {
  generic = 1,
  SIM7600 = 2,
  SIM7070 = 3,
  SIM7000 = 4,
  BG96    = 5,
  SIM800  = 6

};

const __FlashStringHelper* toString(NW005_modem_model NW005_modemmodel)
{
  switch (NW005_modemmodel)
  {
    case NW005_modem_model::generic: return F("Generic");
    case NW005_modem_model::SIM7600: return F("SIM7600");
    case NW005_modem_model::SIM7070: return F("SIM7070");
    case NW005_modem_model::SIM7000: return F("SIM7000");
    case NW005_modem_model::BG96:    return F("BG96");
    case NW005_modem_model::SIM800:  return F("SIM800");
  }
  return F("unknown");
}

ppp_modem_model_t to_ppp_modem_model_t(NW005_modem_model NW005_modemmodel)
{
  switch (NW005_modemmodel)
  {
    case NW005_modem_model::generic: return PPP_MODEM_GENERIC;
    case NW005_modem_model::SIM7600: return PPP_MODEM_SIM7600;
    case NW005_modem_model::SIM7070: return PPP_MODEM_SIM7070;
    case NW005_modem_model::SIM7000: return PPP_MODEM_SIM7000;
    case NW005_modem_model::BG96:    return PPP_MODEM_BG96;
    case NW005_modem_model::SIM800:  return PPP_MODEM_SIM800;
  }
  return PPP_MODEM_GENERIC;
}

void NW005_data_struct_PPP_modem::webform_load(struct EventStruct *event)
{
  _load();

  addFormSubHeader(F("Device Settings"));
  {
    const int ids[] = {
      static_cast<int>(NW005_modem_model::generic),
      static_cast<int>(NW005_modem_model::SIM7600),
      static_cast<int>(NW005_modem_model::SIM7070),
      static_cast<int>(NW005_modem_model::SIM7000),
      static_cast<int>(NW005_modem_model::BG96),
      static_cast<int>(NW005_modem_model::SIM800)
    };

    const __FlashStringHelper*options[] = {
      toString(NW005_modem_model::generic),
      toString(NW005_modem_model::SIM7600),
      toString(NW005_modem_model::SIM7070),
      toString(NW005_modem_model::SIM7000),
      toString(NW005_modem_model::BG96),
      toString(NW005_modem_model::SIM800)
    };

    FormSelectorOptions selector(NR_ELEMENTS(ids), options, ids);
    showFormSelector(*_kvs, selector, NW005_makeWebFormItemParams(NW005_KEY_MODEM_MODEL));
  }

  {
    // TODO TD-er: We cannot use ESPEasySerialPort here as PPPClass needs to handle the pins using periman
    const int ids[] = {
      static_cast<int>(ESPEasySerialPort::serial0)
# if USABLE_SOC_UART_NUM > 1
      , static_cast<int>(ESPEasySerialPort::serial1)
# endif
# if USABLE_SOC_UART_NUM > 2
      , static_cast<int>(ESPEasySerialPort::serial2)
# endif
# if USABLE_SOC_UART_NUM > 3
      , static_cast<int>(ESPEasySerialPort::serial3)
# endif
# if USABLE_SOC_UART_NUM > 4
      , static_cast<int>(ESPEasySerialPort::serial4)
# endif
# if USABLE_SOC_UART_NUM > 5
      , static_cast<int>(ESPEasySerialPort::serial5)
# endif
    };

    constexpr int NR_ESPEASY_SERIAL_TYPES = NR_ELEMENTS(ids);
    const __FlashStringHelper*options[]   = {
      serialHelper_getSerialTypeLabel(ESPEasySerialPort::serial0)
# if USABLE_SOC_UART_NUM > 1
      , serialHelper_getSerialTypeLabel(ESPEasySerialPort::serial1)
# endif
# if USABLE_SOC_UART_NUM > 2
      , serialHelper_getSerialTypeLabel(ESPEasySerialPort::serial2)
# endif
# if USABLE_SOC_UART_NUM > 3
      , serialHelper_getSerialTypeLabel(ESPEasySerialPort::serial3)
# endif
# if USABLE_SOC_UART_NUM > 4
      , serialHelper_getSerialTypeLabel(ESPEasySerialPort::serial4)
# endif
# if USABLE_SOC_UART_NUM > 5
      , serialHelper_getSerialTypeLabel(ESPEasySerialPort::serial5)
# endif
    };

    FormSelectorOptions selector(NR_ELEMENTS(ids), options, ids);
    showFormSelector(*_kvs, selector, NW005_makeWebFormItemParams(NW005_KEY_SERIAL_PORT));
  }

  for (int i = NW005_KEY_PIN_RX; i <= NW005_KEY_PIN_RESET; ++i)
  {
    ESPEasy_key_value_store::StorageType storageType;
    PinSelectPurpose purpose      = PinSelectPurpose::Generic;
    String label                  = NW005_getLabelString(i, true, storageType);
    const __FlashStringHelper *id = NW005_getLabelString(i, false, storageType);

    switch (i)
    {
      case NW005_KEY_PIN_RX:
        purpose = PinSelectPurpose::Serial_input;
        label   = formatGpioName_serialRX(false);
        break;
      case NW005_KEY_PIN_TX:
        purpose = PinSelectPurpose::Serial_output;
        label   = formatGpioName_serialTX(false);
        break;
      case NW005_KEY_PIN_CTS:
        purpose = PinSelectPurpose::Generic_input;
        label   = formatGpioName(
          label, gpio_direction::gpio_input, true);
        break;
      case NW005_KEY_PIN_RTS:
        purpose = PinSelectPurpose::Generic_output;
        label   = formatGpioName(
          label, gpio_direction::gpio_output, true);
        break;
      case NW005_KEY_PIN_RESET:
        purpose = PinSelectPurpose::Generic_output;
        label   = formatGpioName(
          label, gpio_direction::gpio_output, true);
        break;
    }

    int8_t pin = -1;
    _kvs->getValue(i, pin);
    addFormPinSelect(purpose, label, id, pin);
  }
  showWebformItem(
    *_kvs,
    NW005_makeWebFormItemParams(NW005_KEY_PIN_RESET_ACTIVE_LOW));
  {
    auto params = NW005_makeWebFormItemParams(NW005_KEY_PIN_RESET_DELAY);
    params._max             = 2000;
    params._defaultIntValue = 200;
    showWebformItem(*_kvs, params);
    addUnit(F("ms"));

  }
  {
    const int ids[] = {
      ESP_MODEM_FLOW_CONTROL_NONE,
      ESP_MODEM_FLOW_CONTROL_SW,
      ESP_MODEM_FLOW_CONTROL_HW
    };
    const __FlashStringHelper*options[] = {
      F("None"),
      F("Software"),
      F("Hardware")
    };

    FormSelectorOptions selector(NR_ELEMENTS(ids), options, ids);
    showFormSelector(*_kvs, selector, NW005_makeWebFormItemParams(NW005_KEY_FLOWCTRL));
    addFormNote(F("Only set flow control if RTS and CTS are used"));
  }
  {
    auto params = NW005_makeWebFormItemParams(NW005_KEY_BAUDRATE);
    params._max             = 10000000;
    params._defaultIntValue = 115200;
    showWebformItem(*_kvs, params);
  }

  addFormSubHeader(F("Connection"));
  {
    auto params = NW005_makeWebFormItemParams(NW005_KEY_APN);
    params._maxLength = 64;
    showWebformItem(*_kvs, params);
  }
  {
    auto params = NW005_makeWebFormItemParams(NW005_KEY_PIN);
    params._maxLength = 4;
    showWebformItem(*_kvs, params);
    addFormNote(F("Only numerical digits"));
  }

  if (!_modem_initialized) { return; }

  addFormSubHeader(F("Modem State"));
  addRowLabel(F("Modem Model"));
  addHtml_pre(PPP.moduleName());
  addRowLabel(F("IMEI"));
  addHtml_pre(PPP.IMEI());

  addRowLabel(F("IMSI"));
  addHtml_pre(PPP.IMSI());

  if (PPP.attached()) {
    addRowLabel(F("Operator Name"));
    addHtml_pre(PPP.operatorName());
  }

  addRowLabel(F("RSSI"));
  addHtmlInt(PPP.RSSI());

  addRowLabel(F("BER"));
  addHtmlInt(PPP.BER());

  addRowLabel(F("Radio State"));
  addHtml((PPP.radioState() == 0) ? F("Minimal") : F("Full"));

  int networkMode = PPP.networkMode();

  if (networkMode >= 0) {
    addRowLabel(F("Network Mode"));

    // TODO TD-er: Must 'decode' the mode into GSM, LTE, etc.
    addHtmlInt(networkMode);
  }

  int batVolt = PPP.batteryVoltage();

  if (batVolt >= 0) {
    addRowLabel(F("Battery Voltage"));
    addHtmlInt(batVolt);
  }

  int batLevel = PPP.batteryLevel();

  if (batLevel >= 0) {
    addRowLabel(F("Battery Level"));
    addHtmlInt(batLevel);
  }

  int batStatus = PPP.batteryStatus();

  if (batStatus >= 0) {
    addRowLabel(F("Battery Status"));
    addHtmlInt(batStatus);
  }

  // TODO TD-er: Disabled for now, missing PdpContext
  /*
  esp_modem_dce_t *handle = PPP.handle();

  if (handle) {
    int state{};

    if (esp_modem_get_network_registration_state(handle, state) == ESP_OK) {
      addRowLabel(F("Network Registration State"));
      const __FlashStringHelper*str = F("Unknown");

      switch (state)
      {
        case 0: str = F("Not registered, MT is not currently searching an operator to register to");
          break;
        case 1: str = F("Registered, home network");
          break;
        case 2: str = F("Not registered, but MT is currently trying to attach or searching an operator to register to");
          break;
        case 3: str = F("Registration denied");
          break;
        case 4: str = F("Unknown");
          break;
        case 5: str = F("Registered, Roaming");
          break;
        case 6: str = F("Registered, for SMS only, home network (NB-IoT only)");
          break;
        case 7: str = F("Registered, for SMS only, roaming (NB-IoT only)");
          break;
        case 8: str = F("Attached for emergency bearer services only");
          break;
        case 9: str = F("Registered for CSFB not preferred, home network");
          break;
        case 10: str = F("Registered for CSFB not preferred, roaming");
          break;
      }
      addHtml(str);
    }
  }
*/

}

void NW005_data_struct_PPP_modem::webform_save(struct EventStruct *event)
{
  const uint32_t keys[] {
    NW005_KEY_SERIAL_PORT,
    NW005_KEY_PIN_RX,
    NW005_KEY_PIN_TX,
    NW005_KEY_PIN_RTS,
    NW005_KEY_PIN_CTS,
    NW005_KEY_PIN_RESET,
    NW005_KEY_PIN_RESET_ACTIVE_LOW,
    NW005_KEY_PIN_RESET_DELAY,
    NW005_KEY_BAUDRATE,
    NW005_KEY_FLOWCTRL,
    NW005_KEY_MODEM_MODEL,
    NW005_KEY_APN,
    NW005_KEY_PIN
  };


  for (int i = 0; i < NR_ELEMENTS(keys); ++i)
  {
    ESPEasy_key_value_store::StorageType storageType;
    const __FlashStringHelper *id = NW005_getLabelString(keys[i], false, storageType);
    storeWebformItem(*_kvs, keys[i], storageType, id);
  }
  _store();
}

bool NW005_data_struct_PPP_modem::init(struct EventStruct *event)
{
  if (!_KVS_initialized()) {
    addLog(LOG_LEVEL_ERROR, F("PPP: Could not initialize storage"));
    return false;
  }

  if (!_load()) {
    addLog(LOG_LEVEL_ERROR, F("PPP: Could not load settings"));
    return false;
  }

  PPP.setResetPin(
    _kvs->getValueAsInt_or_default(NW005_KEY_PIN_RESET, -1),
    _kvs->getValueAsInt_or_default(NW005_KEY_PIN_RESET_ACTIVE_LOW, 0),
    _kvs->getValueAsInt_or_default(NW005_KEY_PIN_RESET_DELAY, 200));

  const int rtsPin               = _kvs->getValueAsInt_or_default(NW005_KEY_PIN_RTS, -1);
  const int ctsPin               = _kvs->getValueAsInt_or_default(NW005_KEY_PIN_CTS, -1);
  esp_modem_flow_ctrl_t flow_ctl = static_cast<esp_modem_flow_ctrl_t>(_kvs->getValueAsInt_or_default(NW005_KEY_FLOWCTRL,
                                                                                                     ESP_MODEM_FLOW_CONTROL_NONE));

  if ((rtsPin < 0) || (ctsPin < 0)) {
    if (flow_ctl != ESP_MODEM_FLOW_CONTROL_NONE) {
      addLog(LOG_LEVEL_INFO, F("PPP: Disable flow control as RTS/CTS are not set"));
      flow_ctl = ESP_MODEM_FLOW_CONTROL_NONE;
    }
  }

  if (!PPP.setPins(
        _kvs->getValueAsInt_or_default(NW005_KEY_PIN_TX, -1),
        _kvs->getValueAsInt_or_default(NW005_KEY_PIN_RX, -1),
        rtsPin,
        ctsPin,
        flow_ctl))
  {
    addLog(LOG_LEVEL_ERROR, F("PPP: Could not set pins"));
    return false;
  }
  {
    String apn;
    _kvs->getValue(NW005_KEY_APN, apn);
    PPP.setApn(apn.c_str());
  }
  {
    String pin;
    _kvs->getValue(NW005_KEY_PIN, pin);

    if (pin.length() >= 4) {
      PPP.setPin(pin.c_str());
    }
  }

  ppp_modem_model_t model = to_ppp_modem_model_t(static_cast<NW005_modem_model>(
                                                   _kvs->getValueAsInt_or_default(NW005_KEY_MODEM_MODEL, PPP_MODEM_GENERIC)));
  int serialPort = 1;
  {
    int8_t serial = 1;

    if (_kvs->getValue(NW005_KEY_SERIAL_PORT, serial)) {
      switch (static_cast<ESPEasySerialPort>(serial))
      {
        case ESPEasySerialPort::serial0: serialPort = 0;
          break;
# if USABLE_SOC_UART_NUM > 1
        case ESPEasySerialPort::serial1: serialPort = 1;
          break;
# endif // if USABLE_SOC_UART_NUM > 1
# if USABLE_SOC_UART_NUM > 2
        case ESPEasySerialPort::serial2: serialPort = 2;
          break;
# endif // if USABLE_SOC_UART_NUM > 2
# if USABLE_SOC_UART_NUM > 3
        case ESPEasySerialPort::serial3: serialPort = 3;
          break;
# endif // if USABLE_SOC_UART_NUM > 3
# if USABLE_SOC_UART_NUM > 4
        case ESPEasySerialPort::serial4: serialPort = 4;
          break;
# endif // if USABLE_SOC_UART_NUM > 4
# if USABLE_SOC_UART_NUM > 5
        case ESPEasySerialPort::serial5: serialPort = 5;
          break;
# endif // if USABLE_SOC_UART_NUM > 5
        default: break;
      }
    }
  }
  uint32_t baud_rate = _kvs->getValueAsInt_or_default(NW005_KEY_BAUDRATE, 115200);

  if (!PPP.begin(model, serialPort, baud_rate)) {
    addLog(LOG_LEVEL_ERROR, F("PPP: Error PPP.begin"));
    return false;
  }

  addLog(LOG_LEVEL_INFO, strformat(F("PPP: Module Name: %s, IMEI: %s"),
                                   PPP.moduleName().c_str(),
                                   PPP.IMEI().c_str()));

  _modem_initialized = true;
  return true;
}

bool NW005_data_struct_PPP_modem::exit(struct EventStruct *event)
{
  PPP.end();

  _modem_initialized = false;
  return true;
}

struct testStruct {
  String   foo1 = F("test123");
  int64_t  foo2 = -123;
  uint32_t foo3 = 123;


};

void NW005_data_struct_PPP_modem::testWrite()
{
  if (!_KVS_initialized()) { return; }

  for (int i = 0; i < 30; ++i) {
    testStruct _test{ .foo1 = concat(F("str_"), (3 * (i) + 1)), .foo2 = -1l * (3 * (i) + 2), .foo3 = 3 * (i) + 3 };
    _kvs->setValue(3 * (i) + 1, _test.foo1);
    _kvs->setValue(3 * (i) + 2, _test.foo2);
    _kvs->setValue(3 * (i) + 3, _test.foo3);
  }

  _kvs->dump();


  //  _kvs->clear();
  _store();

  _load();

  _kvs->dump();

}

void NW005_data_struct_PPP_modem::testRead()
{
  if (!_KVS_initialized()) { return; }
  _load();

  for (uint32_t i = 1; i <= 30; ++i) {
    String val;
    _kvs->getValue(i, val);
    addLog(LOG_LEVEL_INFO, strformat(F("KVS, foo%d: %s"), i, val.c_str()));
  }

}

#endif // ifdef USES_NW005
