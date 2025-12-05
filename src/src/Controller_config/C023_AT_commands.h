#pragma once

#include <Arduino.h>

#include "../DataStructs/KeyValueStruct.h"

struct C023_timestamped_value {
    C023_timestamped_value(String&& val) :
    timestamp(millis()),
    value(std::move(val)) {}

    bool expired() const;
    
    uint32_t timestamp{};
    String value;
};

class C023_AT_commands
{
public:

  // Received types. 
  // These commands can be used to get and/or set a value.
  enum class AT_cmd {
    UUID,         //
    VER,          // 2.4 AT+VER: Image Version and Frequency Band
    APPEUI,       // 3.1 AT+APPEUI: Application EUI
    APPKEY,       // 3.2 AT+APPKEY: Application Key
    APPSKEY,      // 3.3 AT+APPSKEY: Application Session Key
    DADDR,        // 3.4 AT+DADDR: Device Address
    DEUI,         // 3.5 AT+DEUI: Device EUI
    NWKID,        // 3.6 AT+NWKID: Network ID(You can enter this command change only after successful network connection)
    NWKSKEY,      // 3.7 AT+NWKSKEY: Network Session Key
    CFM,          // 4.1 AT+CFM: Confirm Mode
    NJM,          // 4.3 AT+NJM: LoRa® Network Join Mode
    NJS,          // 4.4 AT+NJS: LoRa® Network Join Status
    RECV,         // 4.5 AT+RECV: Print Last Received Data in Raw Format
    RECVB,        // 4.6 AT+RECVB: Print Last Received Data in Binary Format
    ADR,          // 5.1 AT+ADR: Adaptive Rate
    CLASS,        // 5.2 AT+CLASS: LoRa® Class(Currently only support class A, class C)
    DCS,          // 5.3 AT+DCS: Duty Cycle Setting
    DR,           // 5.4 AT+DR: Data Rate (Can Only be Modified after ADR
    FCD,          // 5.5 AT+FCD: Frame Counter Downlink
    FCU,          // 5.6 AT+FCU: Frame Counter Uplink
    JN1DL,        // 5.7 AT+JN1DL: Join Accept Delay1
    JN2DL,        // 5.8 AT+JN2DL: Join Accept Delay2
    PNM,          // 5.9 AT+PNM: Public Network Mode
    RX1DL,        // 5.10 AT+RX1DL: Receive Delay1
    RX2DL,        // 5.11 AT+RX2DL: Receive Delay2
    RX2DR,        // 5.12 AT+RX2DR: Rx2 Window Data Rate
    RX2FQ,        // 5.13 AT+RX2FQ: Rx2 Window Frequency
    TXP,          // 5.14 AT+TXP: Transmit Power
    RSSI,         // 5.15 AT+RSSI: RSSI of the Last Received Packet
    SNR,          // 5.16 AT+SNR: SNR of the Last Received Packet
    PORT,         // 5.17 AT+PORT: Application Port
    CHS,          // 5.18 AT+ CHS: Single Channel Mode
    SLEEP,        // 5.20 AT+SLEEP: Set sleep mode
    BAT,          // 5.22 AT+BAT: Get the current battery voltage in Mv
    RJTDC,        // 5.23 AT+RJTDC: Get or set the ReJoin data transmission interval in min
    RPL,          // 5.24 AT+RPL: Get or set response level
    TIMESTAMP,    // 5.25 AT+TIMESTAMP: Get or Set UNIX timestamp in second
    LEAPSEC,      // 5.26 AT+LEAPSEC: Get or Set Leap Second
    SYNCMOD,      // 5.27 AT+SYNCMOD: Get or Set time synchronization method
    SYNCTDC,      // 5.28 AT+SYNCTDC: Get or Set time synchronization interval in day
    DDETECT,      // 5.29 AT+DDETECT: Get or set the downlink detection
    SETMAXNBTRANS,// 5.30 AT+SETMAXNBTRANS: Get or set the max nbtrans in LinkADR
    Unknown
  };

  static const __FlashStringHelper * toFlashString(AT_cmd at_cmd);

  static String toString(AT_cmd at_cmd);
  static const __FlashStringHelper * toDisplayString(AT_cmd at_cmd);

  static AT_cmd determineReceivedDataType(const String& receivedData);

  static AT_cmd decode(const String& receivedData, String& value);

  static KeyValueStruct getKeyValue(AT_cmd at_cmd, const String& value, bool extendedValue);


}; // class C023_AT_commands


