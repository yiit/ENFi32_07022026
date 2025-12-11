#include "../Controller_struct/C023_data_struct.h"

#ifdef USES_C023


# include <ESPeasySerial.h>


C023_data_struct::C023_data_struct() :
  _easySerial(nullptr) {}

C023_data_struct::~C023_data_struct() {
  if (_easySerial != nullptr) {
    _easySerial->end();
    delete _easySerial;
    _easySerial = nullptr;
  }
}

void C023_data_struct::reset() {
  if (_easySerial != nullptr) {
    _easySerial->end();
    delete _easySerial;
    _easySerial = nullptr;
  }
  _cachedValues.clear();
  _queuedQueries.clear();
  clearQueryPending();
}

bool C023_data_struct::init(
  const C023_ConfigStruct& config,
  taskIndex_t              sampleSet_Initiator)
{
  const uint8_t port      = config.serialPort;
  const int8_t  serial_rx = config.rxpin;
  const int8_t  serial_tx = config.txpin;
  unsigned long baudrate  = config.baudrate;
  bool   joinIsOTAA       = (config.joinmethod == C023_USE_OTAA);
  int8_t reset_pin        = config.resetpin;

  _eventFormatStructure = config.getEventFormat();

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
    notChanged &= _easySerial->getRxPin() == serial_rx;
    notChanged &= _easySerial->getTxPin() == serial_tx;
    notChanged &= _easySerial->getBaudRate() == static_cast<int>(baudrate);

    if (notChanged) { return true; }
  }
  reset();
  _resetPin = reset_pin;
  _baudrate = baudrate;
  _isClassA = config.getClass() == C023_ConfigStruct::LoRaWANclass_e::A;

  // FIXME TD-er: Make force SW serial a proper setting.
  if (_easySerial != nullptr) {
    delete _easySerial;
  }
    // When calling "AT+CFG", we may get quite a lot of data at once at a relatively low baud rate.
    // This requires quite a lot of calls to read it, so it is much more likely to have some other call inbetween taking way longer than 20
    // msec and thus we will miss some data

  _easySerial = new (std::nothrow) ESPeasySerial(static_cast<ESPEasySerialPort>(port), serial_rx, serial_tx, false, 1024);

  if (_easySerial != nullptr) {
    _easySerial->begin(baudrate);

    _easySerial->println(F("ATZ"));    // Reset LoRa
    delay(1000);

    _easySerial->println(F("AT+CFG")); // AT+CFG: Print all configurations
  }
  return isInitialized();
}

bool C023_data_struct::hasJoined() {
  if (!isInitialized()) { return false; }
  return getInt(C023_AT_commands::AT_cmd::NJS, 0) != 0;
}

bool C023_data_struct::useOTAA() {
  if (!isInitialized()) { return true; }
  return getInt(C023_AT_commands::AT_cmd::NJM, 1) == 1;
}

bool C023_data_struct::command_finished() const {
  return true; // myLora->command_finished();
}

bool C023_data_struct::txUncnfBytes(const uint8_t *data, uint8_t size, uint8_t port) {
  bool res = true; // myLora->txBytes(data, size, port) != RN2xx3_datatypes::TX_return_type::TX_FAIL;

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

  _easySerial->println(
    strformat(
      F("AT+SENDB=%d,%d,%d,%s"),
      0,                     // confirm status
      port,                  // Fport
      sendData.length() / 2, // payload length
      sendData.c_str()));    // payload(HEX)

  // TODO TD-er: Must wait for either "OK" or "AT_BUSY_ERROR"
  // This 'busy error' may occur in case the previous send is
  // not completed, because of the duty cycle restriction,
  // or because RX windows are not completed

  return res;
}

bool C023_data_struct::txUncnf(const String& data, uint8_t port) {
  bool res = true; // myLora->tx(data, port) != RN2xx3_datatypes::TX_return_type::TX_FAIL;

  return res;
}

bool C023_data_struct::setSF(uint8_t sf) {
  if (!isInitialized()) { return false; }
  bool res = true; // myLora->setSF(sf);
  return res;
}

bool C023_data_struct::setAdaptiveDataRate(bool enabled) {
  if (!isInitialized()) { return false; }
  bool res = true; // myLora->setAdaptiveDataRate(enabled);
  return res;
}

bool C023_data_struct::initOTAA(const String& AppEUI, const String& AppKey, const String& DevEUI) {
  //  if (myLora == nullptr) { return false; }
  bool success = true; // myLora->initOTAA(AppEUI, AppKey, DevEUI);

  return success;
}

bool C023_data_struct::initABP(const String& addr, const String& AppSKey, const String& NwkSKey) {
  //  if (myLora == nullptr) { return false; }
  bool success = true; // myLora->initABP(addr, AppSKey, NwkSKey);

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
  return res;
}

int    C023_data_struct::getVbat() { return getInt(C023_AT_commands::AT_cmd::BAT, -1); }

String C023_data_struct::peekLastError() {
  if (!isInitialized()) { return EMPTY_STRING; }
  return EMPTY_STRING; // myLora->peekLastError();
}

String C023_data_struct::getLastError() {
  if (!isInitialized()) { return EMPTY_STRING; }
  return EMPTY_STRING; // myLora->getLastError();
}

String C023_data_struct::getDataRate() {
  const int dr_int = getInt(C023_AT_commands::AT_cmd::DR, -1);

  if (dr_int == -1) { return F("-"); }
  C023_ConfigStruct::LoRaWAN_DR dr =
    static_cast<C023_ConfigStruct::LoRaWAN_DR>(getInt(C023_AT_commands::AT_cmd::DR, 0));
  return strformat(F("%d: %s"), dr_int, FsP(C023_ConfigStruct::toString(dr)));
}

int      C023_data_struct::getRSSI() { return getInt(C023_AT_commands::AT_cmd::RSSI, 0); }

uint32_t C023_data_struct::getRawStatus() {
  if (!isInitialized()) { return 0; }
  return 0; // myLora->getStatus().getRawStatus();
}

bool C023_data_struct::getFrameCounters(uint32_t& dnctr, uint32_t& upctr) {
  if (!isInitialized()) { return false; }
  dnctr = getInt(C023_AT_commands::AT_cmd::FCD, 0);
  upctr = getInt(C023_AT_commands::AT_cmd::FCU, 0);
  return true;
}

bool C023_data_struct::setFrameCounters(uint32_t dnctr, uint32_t upctr) {
  if (!isInitialized()) { return false; }
  bool res = true; // myLora->setFrameCounters(dnctr, upctr);

  return res;
}

// Cached data, only changing occasionally.

String  C023_data_struct::getDevaddr() { return get(C023_AT_commands::AT_cmd::DADDR); }

String  C023_data_struct::hweui()      { return get(C023_AT_commands::AT_cmd::DEUI); }

String  C023_data_struct::sysver()     { return get(C023_AT_commands::AT_cmd::VER); }

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
    while (_easySerial->available()) {
      const int ret = _easySerial->read();

      if (ret >= 0) {
        const char c = static_cast<char>(ret);

        switch (c)
        {
          case '\n':
          case '\r':
          {
            // End of line
            if (!_fromLA66.isEmpty()) {
              addLog(LOG_LEVEL_INFO, concat(F("LoRa recv: "), _fromLA66));

              // TODO TD-er: Process received data
              processReceived(_fromLA66);
            }

            _fromLA66.clear();
            break;
          }
          default:
            _fromLA66 += c;
            break;
        }
      }
    }

    sendNextQueuedQuery();
  }
}

bool C023_data_struct::writeCachedValues(KeyValueWriter*writer, C023_AT_commands::AT_cmd start, C023_AT_commands::AT_cmd end)
{
  if (writer == nullptr) { return false; }

  for (size_t i = static_cast<size_t>(start); i < static_cast<size_t>(end); ++i) {
    const C023_AT_commands::AT_cmd cmd = static_cast<C023_AT_commands::AT_cmd>(i);

    const String value = get(cmd);

    if (!value.isEmpty()) {
      auto kv = C023_AT_commands::getKeyValue(cmd, value, true /*!writer->dataOnlyOutput()*/);
      writer->write(kv);
    }
  }
  return true;
}

String C023_data_struct::get(C023_AT_commands::AT_cmd at_cmd)
{
  if (isInitialized() && (at_cmd != C023_AT_commands::AT_cmd::Unknown)) {
    auto it = _cachedValues.find(static_cast<size_t>(at_cmd));

    if (it != _cachedValues.end()) {
      if (it->second.expired()) {
        sendQuery(at_cmd);
      }
      return it->second.value;
    }
    sendQuery(at_cmd);
  }
  return EMPTY_STRING;
}

int C023_data_struct::getInt(C023_AT_commands::AT_cmd at_cmd, int errorvalue)
{
  String value = get(at_cmd);

  if (value.isEmpty()) {
    return errorvalue;
  }
  return value.toInt();
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
    } else if (receivedData.indexOf(F("ADR Message")) != -1) {
      sendQuery(C023_AT_commands::AT_cmd::ADR);
      sendQuery(C023_AT_commands::AT_cmd::DR);
    } else if (receivedData.equals(F("JOINED"))) {
      _easySerial->println(concat(F("AT+CLASS="), _isClassA ? 'A' : 'C'));

      // Enable Sync system time via LoRaWAN MAC Command (DeviceTimeReq), LoRaWAN server must support v1.0.3 protocol to reply this command.
      _easySerial->println(F("AT+SYNCMOD=1"));

      _easySerial->println(F("AT+CFG")); // AT+CFG: Print all configurations
      //      sendQuery(C023_AT_commands::AT_cmd::NJM);
      //      sendQuery(C023_AT_commands::AT_cmd::NJS);
      eventQueue.add(F("LoRa#joined"));
    } else if (receivedData.equals(F("rxTimeout"))) {
      // Just skip this one, no data received
    } else if (receivedData.startsWith(F("Rssi"))) {
      cacheValue(C023_AT_commands::AT_cmd::RSSI, getValueFromReceivedData(receivedData));
    } else if (receivedData.indexOf(F("DevEui")) != -1) {
      cacheValue(C023_AT_commands::AT_cmd::DEUI, getValueFromReceivedData(receivedData));
    } else if (receivedData.indexOf(F("AT+RECVB=?")) != -1) {
      sendQuery(C023_AT_commands::AT_cmd::RECVB, true);
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
      if (!queryPending()) {
        addLog(LOG_LEVEL_ERROR, strformat(
                 F("LoRa   : %s"),
                 receivedData.c_str()));
      } else {
        addLog(LOG_LEVEL_ERROR, strformat(
                 F("LoRa   : %s while processing %s"),
                 receivedData.c_str(),
                 C023_AT_commands::toString(_queryPending).c_str()));
        clearQueryPending();
      }
    }

    else if (receivedData.equals(F("OK"))) {
      // Just ignore
    } else {
      processPendingQuery(receivedData);
    }

    return false;
  }
  cacheValue(at_cmd, std::move(value));

  return true;
}

bool C023_data_struct::processPendingQuery(const String& receivedData)
{
  if (!queryPending()) {
    return false;
  }

  if (_queryPending == C023_AT_commands::AT_cmd::RECVB) {
    int port{};
    String value = getValueFromReceivedBinaryData(port, receivedData);

    if ((port > 0) && (value.length() != 0)) {
      switch (_eventFormatStructure)
      {
        case C023_ConfigStruct::EventFormatStructure_e::PortNr_in_eventPar:
          eventQueue.addMove(strformat(F("LoRa#received%d=%s"), port, value.c_str()));
          break;
        case C023_ConfigStruct::EventFormatStructure_e::PortNr_as_first_eventvalue:
          eventQueue.addMove(strformat(F("LoRa#received=%d,%s"), port, value.c_str()));
          break;
        case C023_ConfigStruct::EventFormatStructure_e::PortNr_both_eventPar_eventvalue:
          eventQueue.addMove(strformat(F("LoRa#received%d=%d,%s"), port, port, value.c_str()));
          break;
      }
    }

    cacheValue(_queryPending, strformat(
                 F("%s -> %d : %s"), receivedData.c_str(), port, value.c_str()));
  } else {
    cacheValue(_queryPending, receivedData);
  }
  addLog(LOG_LEVEL_INFO, strformat(
           F("LoRa : Process Query: %s -> %s"),
           C023_AT_commands::toString(_queryPending).c_str(),
           receivedData.c_str()));

  clearQueryPending();
  return true;

}

void C023_data_struct::sendQuery(C023_AT_commands::AT_cmd at_cmd, bool prioritize)
{
  if (_easySerial) {
    if (prioritize) {
      _queuedQueries.push_front(static_cast<size_t>(at_cmd));
    } else {
      _queuedQueries.push_back(static_cast<size_t>(at_cmd));
    }

    if (loglevelActiveFor(LOG_LEVEL_INFO)) {
      const String query = concat(C023_AT_commands::toString(at_cmd), F("=?"));
      addLog(LOG_LEVEL_INFO, concat(F("LoRa : Add to queue: "), query));
    }
  }
}

void C023_data_struct::sendNextQueuedQuery()
{
  if (!queryPending() && !_queuedQueries.empty()) {
    _queryPending = static_cast<C023_AT_commands::AT_cmd>(_queuedQueries.front());
    _queuedQueries.pop_front();
    const String query = concat(C023_AT_commands::toString(_queryPending), F("=?"));
    addLog(LOG_LEVEL_INFO, concat(F("LoRa : Queried "), query));

    _easySerial->println(query);
    _querySent = millis();
  }
}

void C023_data_struct::cacheValue(C023_AT_commands::AT_cmd at_cmd, const String& value)
{
  String tmp(value);

  cacheValue(at_cmd, std::move(tmp));
}

void C023_data_struct::cacheValue(C023_AT_commands::AT_cmd at_cmd, String&& value)
{
  if (value.isEmpty()) { return; }
  const size_t key = static_cast<size_t>(at_cmd);
  auto it          = _cachedValues.find(key);

  if (it != _cachedValues.end()) {
    it->second.value = std::move(value);
    it->second.timestamp = millis();
  } else {
    _cachedValues.emplace(key, std::move(value));
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

String C023_data_struct::getValueFromReceivedBinaryData(int& port, const String& receivedData)
{
  port = -1;
  int pos = receivedData.indexOf(':');

  if (pos == -1) { return EMPTY_STRING; }
  port = receivedData.substring(0, pos).toInt();

  addLog(LOG_LEVEL_INFO, concat(
           F("LoRa fromHex: "), receivedData.substring(pos + 1)));

  return stringFromHexArray(receivedData.substring(pos + 1));
}

#endif // ifdef USES_C023
