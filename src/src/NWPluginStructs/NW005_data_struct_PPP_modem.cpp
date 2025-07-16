#include "../NWPluginStructs/NW005_data_struct_PPP_modem.h"

#ifdef USES_NW005

# include "../Helpers/StringConverter.h"
# include "../Helpers/_Plugin_Helper_serial.h"

# include "../WebServer/Markup.h"
# include "../WebServer/Markup_Forms.h"
# include "../WebServer/ESPEasy_key_value_store_webform.h"

# include <ESPEasySerialPort.h>
# include <PPP.h>

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
  }
  {
    auto params = NW005_makeWebFormItemParams(NW005_KEY_BAUDRATE);
    params._max = 10000000;
    showWebformItem(*_kvs, params);

  }

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
    NW005_KEY_MODEM_MODEL
  };


  for (int i = 0; i < NR_ELEMENTS(keys); ++i)
  {
    ESPEasy_key_value_store::StorageType storageType;
    const __FlashStringHelper *id = NW005_getLabelString(keys[i], false, storageType);
    _kvs->setValue(storageType, keys[i], webArg(id));
  }
  _store();
}

bool NW005_data_struct_PPP_modem::init(struct EventStruct *event)
{
  if (!_KVS_initialized()) { return false; }

  if (!_load()) { return false; }
  int8_t tx, rx, rts, cts, rst = -1;
  _kvs->getValue(NW005_KEY_PIN_TX,    tx);
  _kvs->getValue(NW005_KEY_PIN_RX,    rx);
  _kvs->getValue(NW005_KEY_PIN_RTS,   rts);
  _kvs->getValue(NW005_KEY_PIN_CTS,   cts);
  _kvs->getValue(NW005_KEY_PIN_RESET, rst);

  PPP.setResetPin(rst);

  if (!PPP.setPins(tx, rx, rts, cts)) { return false; }

  ppp_modem_model_t model = PPP_MODEM_GENERIC;
  {
    int8_t modemModel = PPP_MODEM_GENERIC;

    if (_kvs->getValue(NW005_KEY_MODEM_MODEL, modemModel)) {
      ppp_modem_model_t model = to_ppp_modem_model_t(static_cast<NW005_modem_model>(modemModel));
    }
  }
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
  uint32_t baud_rate = 115200;
  _kvs->getValue(NW005_KEY_BAUDRATE, baud_rate);

  if (!PPP.begin(model, serialPort, baud_rate)) { return false; }


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
