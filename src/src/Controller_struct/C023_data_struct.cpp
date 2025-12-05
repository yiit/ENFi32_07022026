#include "../Controller_struct/C023_data_struct.h"

#ifdef USES_C023


# include <ESPeasySerial.h>


C023_data_struct::C023_data_struct() :
  C023_easySerial(nullptr) {}

C023_data_struct::~C023_data_struct() {
  reset();
}

void C023_data_struct::reset() {
  if (C023_easySerial != nullptr) {
    delete C023_easySerial;
    C023_easySerial = nullptr;
  }
  free_string(cacheDevAddr);
  free_string(cacheHWEUI);
  free_string(cacheSysVer);
}

bool C023_data_struct::init(const uint8_t port, const int8_t serial_rx, const int8_t serial_tx, unsigned long baudrate,
                            bool joinIsOTAA, taskIndex_t sampleSet_Initiator, int8_t reset_pin) {
  if ((serial_rx < 0) || (serial_tx < 0)) {
    // Both pins are needed, or else no serial possible
    return false;
  }

  // FIXME TD-er: Prevent unneeded OTAA joins.
  // See: https://www.thethingsnetwork.org/forum/t/how-often-should-a-node-do-an-otaa-join-and-is-otaa-better-than-abp/11192/47?u=td-er


  sampleSetInitiator = sampleSet_Initiator;

  if (isInitialized()) {
    // Check to see if serial parameters have changed.
    bool notChanged = true;
    notChanged &= C023_easySerial->getRxPin() == serial_rx;
    notChanged &= C023_easySerial->getTxPin() == serial_tx;
    notChanged &= C023_easySerial->getBaudRate() == static_cast<int>(baudrate);

    if (notChanged) { return true; }
  }
  reset();
  _resetPin = reset_pin;
  _baudrate = baudrate;

  // FIXME TD-er: Make force SW serial a proper setting.
  if (C023_easySerial != nullptr) {
    delete C023_easySerial;
  }

  C023_easySerial = new (std::nothrow) ESPeasySerial(static_cast<ESPEasySerialPort>(port), serial_rx, serial_tx, false, 64);

  if (C023_easySerial != nullptr) {
    C023_easySerial->begin(baudrate);

    //    C023_easySerial->println(F("ATZ")); // Reset LA66
    delay(1000);
    C023_easySerial->println(F("AT+CFG")); // AT+CFG: Print all configurations

  }
  return isInitialized();
}

bool C023_data_struct::hasJoined() const {
  if (!isInitialized()) { return false; }
  return true; // myLora->hasJoined();
}

bool C023_data_struct::useOTAA() const {
  if (!isInitialized()) { return true; }
  bool res = true; // = myLora->useOTAA();

  C023_logError(F("useOTA()"));
  return res;
}

bool C023_data_struct::command_finished() const {
  return true; // myLora->command_finished();
}

bool C023_data_struct::txUncnfBytes(const uint8_t *data, uint8_t size, uint8_t port) {
  bool res = true; // myLora->txBytes(data, size, port) != RN2xx3_datatypes::TX_return_type::TX_FAIL;

  C023_logError(F("txUncnfBytes()"));
  return res;
}

bool C023_data_struct::txHexBytes(const String& data, uint8_t port) {
  if (!isInitialized()) { return false; }
  bool   res      = true;
  String sendData = data;
  sendData.replace(F(" "), F(""));
  sendData.trim();

  // "AT+SENDB=0,2,4,11223344"
  // confirm status,Fport,payload length,payload(HEX)

  C023_easySerial->println(
    strformat(
      F("AT+SENDB=%d,%d,%d,%s"),
      0,                     // confirm status
      port,                  // Fport
      sendData.length() / 2, // payload length
      sendData.c_str()));    // payload(HEX)

  C023_logError(F("txHexBytes()"));
  return res;
}

bool C023_data_struct::txUncnf(const String& data, uint8_t port) {
  bool res = true; // myLora->tx(data, port) != RN2xx3_datatypes::TX_return_type::TX_FAIL;

  C023_logError(F("txUncnf()"));
  return res;
}

bool C023_data_struct::setSF(uint8_t sf) {
  if (!isInitialized()) { return false; }
  bool res = true; // myLora->setSF(sf);

  C023_logError(F("setSF()"));
  return res;
}

bool C023_data_struct::setAdaptiveDataRate(bool enabled) {
  if (!isInitialized()) { return false; }
  bool res = true; // myLora->setAdaptiveDataRate(enabled);

  C023_logError(F("setAdaptiveDataRate()"));
  return res;
}

bool C023_data_struct::initOTAA(const String& AppEUI, const String& AppKey, const String& DevEUI) {
  //  if (myLora == nullptr) { return false; }
  bool success = true; // myLora->initOTAA(AppEUI, AppKey, DevEUI);

  free_string(cacheDevAddr);

  C023_logError(F("initOTAA()"));
  updateCacheOnInit();
  return success;
}

bool C023_data_struct::initABP(const String& addr, const String& AppSKey, const String& NwkSKey) {
  //  if (myLora == nullptr) { return false; }
  bool success = true; // myLora->initABP(addr, AppSKey, NwkSKey);

  cacheDevAddr = addr;

  C023_logError(F("initABP()"));
  updateCacheOnInit();
  return success;
}

String C023_data_struct::sendRawCommand(const String& command) {
  if (!isInitialized()) { return EMPTY_STRING; }

  if (loglevelActiveFor(LOG_LEVEL_INFO)) {
    String log = F("sendRawCommand: ");
    log += command;
    addLogMove(LOG_LEVEL_INFO, log);
  }
  String res; // = myLora->sendRawCommand(command);

  C023_logError(F("sendRawCommand()"));
  return res;
}

int C023_data_struct::getVbat() {
  if (!isInitialized()) { return -1; }
  return -1; // myLora->getVbat();
}

String C023_data_struct::peekLastError() {
  if (!isInitialized()) { return EMPTY_STRING; }
  return EMPTY_STRING; // myLora->peekLastError();
}

String C023_data_struct::getLastError() {
  if (!isInitialized()) { return EMPTY_STRING; }
  return EMPTY_STRING; // myLora->getLastError();
}

String C023_data_struct::getDataRate() {
  if (!isInitialized()) { return EMPTY_STRING; }
  String res; // = myLora->getDataRate();

  C023_logError(F("getDataRate()"));
  return res;
}

int C023_data_struct::getRSSI() {
  if (!isInitialized()) { return 0; }
  return 0; // myLora->getRSSI();
}

uint32_t C023_data_struct::getRawStatus() {
  if (!isInitialized()) { return 0; }
  return 0; // myLora->getStatus().getRawStatus();
}

bool C023_data_struct::getFrameCounters(uint32_t& dnctr, uint32_t& upctr) {
  if (!isInitialized()) { return false; }
  bool res = true; // myLora->getFrameCounters(dnctr, upctr);

  C023_logError(F("getFrameCounters()"));
  return res;
}

bool C023_data_struct::setFrameCounters(uint32_t dnctr, uint32_t upctr) {
  if (!isInitialized()) { return false; }
  bool res = true; // myLora->setFrameCounters(dnctr, upctr);

  C023_logError(F("setFrameCounters()"));
  return res;
}

// Cached data, only changing occasionally.

String C023_data_struct::getDevaddr() {
  if (cacheDevAddr.isEmpty())
  {
    updateCacheOnInit();
  }
  return cacheDevAddr;
}

String C023_data_struct::hweui() {
  if (cacheHWEUI.isEmpty()) {
    if (isInitialized()) {
      // cacheHWEUI = myLora->hweui();
    }
  }
  return cacheHWEUI;
}

String C023_data_struct::sysver() {
  if (cacheSysVer.isEmpty()) {
    if (isInitialized()) {
      // cacheSysVer = myLora->sysver();
    }
  }
  return cacheSysVer;
}

uint8_t C023_data_struct::getSampleSetCount() const {
  return sampleSetCounter;
}

uint8_t C023_data_struct::getSampleSetCount(taskIndex_t taskIndex) {
  if (sampleSetInitiator == taskIndex)
  {
    ++sampleSetCounter;
  }
  return sampleSetCounter;
}

float C023_data_struct::getLoRaAirTime(uint8_t pl) const {
  if (isInitialized()) {
    return 0.0f; // myLora->getLoRaAirTime(pl + 13); // We have a LoRaWAN header of 13 bytes.
  }
  return -1.0f;
}

void C023_data_struct::async_loop() {
  if (isInitialized()) {
    /*
       rn2xx3_handler::RN_state state = myLora->async_loop();

       if (rn2xx3_handler::RN_state::must_perform_init == state) {
       if (myLora->get_busy_count() > 10) {
        if (validGpio(_resetPin)) {
          pinMode(_resetPin, OUTPUT);
          digitalWrite(_resetPin, LOW);
          delay(50);
          digitalWrite(_resetPin, HIGH);
          delay(200);
        }
        autobaud_success = false;

        //          triggerAutobaud();
       }
       }
     */
    while (C023_easySerial->available()) {
      const int ret = C023_easySerial->read();

      if (ret < 0) { return; }
      const char c = static_cast<char>(ret);

      switch (c)
      {
        case '\n':
        case '\r':
        {
          // End of line
          if (!_fromLA66.isEmpty()) {
            addLog(LOG_LEVEL_INFO, concat(F("LA66 recv: "), _fromLA66));

            // TODO TD-er: Process received data
            processReceived(_fromLA66);
          }

          _fromLA66.clear();
          return;
        }
        default:
          _fromLA66 += c;
          break;
      }
    }
  }
}

bool C023_data_struct::writeCachedValues(KeyValueWriter*writer)
{
  if (writer == nullptr) { return false; }

  for (size_t i = 0; i < static_cast<size_t>(C023_AT_commands::AT_cmd::Unknown); ++i) {
    const C023_AT_commands::AT_cmd cmd = static_cast<C023_AT_commands::AT_cmd>(i);

    String value = get(cmd);

    if (!value.isEmpty()) {
      auto kv = C023_AT_commands::getKeyValue(cmd, value, true /*!writer->dataOnlyOutput()*/);
      writer->write(kv);
    }
  }
  return true;
}

String C023_data_struct::get(C023_AT_commands::AT_cmd at_cmd)
{
  if (at_cmd != C023_AT_commands::AT_cmd::Unknown) {
    auto it = _cachedValues.find(static_cast<size_t>(at_cmd));

    if (it != _cachedValues.end()) {
      if (!it->second.expired()) {
        return it->second.value;
      }
      _cachedValues.erase(it);
    }
  }
  sendQuery(at_cmd);
  return EMPTY_STRING;
}

bool C023_data_struct::processReceived(const String& receivedData)
{
  String value;
  const C023_AT_commands::AT_cmd at_cmd = C023_AT_commands::decode(receivedData, value);

  if (at_cmd == C023_AT_commands::AT_cmd::Unknown) {
    if (receivedData.equals(F("txDone"))) {
      sendQuery(C023_AT_commands::AT_cmd::FCU);
    } else if (receivedData.equals(F("rxDone"))) {
      sendQuery(C023_AT_commands::AT_cmd::FCD);
      sendQuery(C023_AT_commands::AT_cmd::SNR);
    } else if (receivedData.equals(F("rxTimeout"))) {
      // Just skip this one, no data received
    } else if (receivedData.startsWith(F("Rssi"))) {
      String value = getValueFromReceivedData(receivedData);

      if (!value.isEmpty()) {
        _cachedValues.emplace(static_cast<size_t>(C023_AT_commands::AT_cmd::RSSI), std::move(value));
      }
    } else if (receivedData.indexOf(F("AT+RECVB=?")) != -1) {
      sendQuery(C023_AT_commands::AT_cmd::RECVB);
    } else if (receivedData.startsWith(F("*****")) ||
               receivedData.startsWith(F("TX on")) ||
               receivedData.startsWith(F("RX on")))
    {
      // Ignore these lines for now.
      // Maybe those "***** UpLinkCounter= 51 *****" could be parsed
    }

    else if (
      receivedData.equals(F("AT_ERROR")) ||               // Generic error
      receivedData.equals(F("AT_PARAM_ERROR")) ||         // A parameter of the command is wrong
      receivedData.equals(F("AT_BUSY_ERROR")) ||          // the LoRa® network is busy, so the command could not completed
      receivedData.equals(F("AT_TEST_PARAM_OVERFLOW")) || // the parameter is too long
      receivedData.equals(F("AT_NO_NETWORK_JOINED")) ||   // the LoRa® network has not been joined yet
      receivedData.equals(F("AT_RX_ERROR")))              // error detection during the reception of the command
    {
      if (_queuedQueries.empty()) {
        addLog(LOG_LEVEL_ERROR, strformat(
                 F("LA66   : %s"),
                 receivedData.c_str()));
      } else {
        addLog(LOG_LEVEL_ERROR, strformat(
                 F("LA66   : %s while processing %s"),
                 receivedData.c_str(),
                 C023_AT_commands::toString(static_cast<C023_AT_commands::AT_cmd>(_queuedQueries.front())).c_str()));
        _queuedQueries.pop_front();
      }
    }

    else if (receivedData.equals(F("OK"))) {
//      if (!_queuedQueries.empty()) {
//        _queuedQueries.pop_front();
//      }
    } else {
      if (!_queuedQueries.empty()) {
        String tmp(receivedData);

        //        C023_timestamped_value ts_value(std::move(tmp));
        //        _cachedValues[_queuedQueries.front()] = ts_value;
        auto it = _cachedValues.find(_queuedQueries.front());

        if (it != _cachedValues.end()) {
          _cachedValues.erase(it);
        }
        _cachedValues.emplace(_queuedQueries.front(), std::move(tmp));
        addLog(LOG_LEVEL_INFO, strformat(
                 F("LA66 : Process Query: %s -> %s"),
                 C023_AT_commands::toString(static_cast<C023_AT_commands::AT_cmd>(_queuedQueries.front())).c_str(),
                 receivedData.c_str()));

        _queuedQueries.pop_front();
      }
    }

    return false;
  }

  //  addLog(LOG_LEVEL_INFO, concat(C023_AT_commands::toString(at_cmd), receivedData));

  _cachedValues.emplace(static_cast<size_t>(at_cmd), std::move(value));

  //  _cachedValues[static_cast<size_t>(at_cmd)] = C023_timestamped_value(std::move(value));

  return true;
}

void C023_data_struct::sendQuery(C023_AT_commands::AT_cmd at_cmd)
{
  if (C023_easySerial) {
    _queuedQueries.push_back(static_cast<size_t>(at_cmd));
    const String query = concat(C023_AT_commands::toString(at_cmd), F("=?"));
    addLog(LOG_LEVEL_INFO, concat(F("LA66 : Queried "), query));
    C023_easySerial->println(query);

    // TODO: Wait for result and store in cached value.
  }
}

String C023_data_struct::getValueFromReceivedData(const String& receivedData)
{
  const int pos = receivedData.indexOf('=');

  if (pos == -1) { return EMPTY_STRING; }
  String res = receivedData.substring(pos + 1);
  res.trim();
  return res;
}

void C023_data_struct::C023_logError(const __FlashStringHelper *command) const {
  if (loglevelActiveFor(LOG_LEVEL_INFO)) {
    String error; // = myLora->peekLastError();

    //    String error = myLora->getLastError();

    if (error.length() > 0) {
      String log = F("RN2483: ");
      log += command;
      log += F(": ");
      log += error;
      addLogMove(LOG_LEVEL_INFO, log);
    }
  }
}

void C023_data_struct::updateCacheOnInit() {
  if (isInitialized()) {
    if (cacheDevAddr.isEmpty() /* && myLora->getStatus().Joined*/)
    {
      // cacheDevAddr = myLora->sendRawCommand(F("mac get devaddr"));

      if (cacheDevAddr == F("00000000")) {
        free_string(cacheDevAddr);
      }
    }
  }
}

#endif // ifdef USES_C023
