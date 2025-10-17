#include "../Helpers/StringProvider.h"

#if FEATURE_ETHERNET
# include <ETH.h>
#endif // if FEATURE_ETHERNET

#include "../../ESPEasy-Globals.h"

#include "../CustomBuild/CompiletimeDefines.h"

#include "../../ESPEasy/net/ESPEasyNetwork.h"
#include "../../ESPEasy/net/Helpers/NWAccessControl.h"
#include "../../ESPEasy/net/wifi/ESPEasyWifi.h"

#if FEATURE_ETHERNET
# include "../../ESPEasy/net/eth/ESPEasyEth.h"
#endif

#include "../Globals/Device.h"
#include "../Globals/ESPEasy_Console.h"
#include "../Globals/ESPEasy_Scheduler.h"
#include "../Globals/ESPEasy_time.h"
#include "../../ESPEasy/net/Globals/ESPEasyWiFiEvent.h"

#include "../../ESPEasy/net/Globals/NetworkState.h"
#include "../Globals/RTC.h"
#include "../Globals/SecuritySettings.h"
#include "../Globals/Settings.h"
#include "../../ESPEasy/net/Globals/WiFi_AP_Candidates.h"

#include "../Helpers/Convert.h"
#include "../Helpers/ESPEasy_Storage.h"
#include "../Helpers/Hardware_device_info.h"
#include "../Helpers/Hardware_temperature_sensor.h"
#include "../Helpers/Memory.h"
#include "../Helpers/Misc.h"
#include "../Helpers/Networking.h"
#include "../Helpers/OTA.h"
#include "../Helpers/Scheduler.h"
#include "../Helpers/StringConverter.h"
#include "../Helpers/StringGenerator_System.h"
#include "../Helpers/StringGenerator_WiFi.h"

#include "../WebServer/JSON.h"
#include "../WebServer/AccessControl.h"

#ifdef ESP32
# include <soc/rtc.h>
#endif

KeyValueStruct getKeyValue(LabelType::Enum label, bool extendedValue)
{
  switch (label)
  {
    case LabelType::UNIT_NR:
    {
      KeyValueStruct kv(F("Unit Number"), Settings.Unit);
      return kv;
    }
    #if FEATURE_ZEROFILLED_UNITNUMBER
    case LabelType::UNIT_NR_0:
    {
      // Fixed 3-digit unitnumber
      KeyValueStruct kv(F("Unit Number 0-filled"), formatIntLeadingZeroes(Settings.Unit, 3));
      return kv;
    }
    #endif // FEATURE_ZEROFILLED_UNITNUMBER
    case LabelType::UNIT_NAME:
    {
      // Only return the set name, no appended unit.
      KeyValueStruct kv(F("Unit Name"), Settings.getName());
      return kv;
    }
    case LabelType::HOST_NAME:
    {
      KeyValueStruct kv(F("Hostname"), ESPEasy::net::NetworkGetHostname());
      return kv;
    }

    case LabelType::LOCAL_TIME:

      if (node_time.systemTimePresent())
      {
        KeyValueStruct kv(F("Local Time"), node_time.getDateTimeString('-', ':', ' '));
        return kv;
      } else if (extendedValue) {
        KeyValueStruct kv(F("Local Time"), F("<font color='red'>No system time source</font>"));
        return kv;
      }
      break;
    case LabelType::TIME_SOURCE:

      if (node_time.systemTimePresent())
      {
        String timeSource_str = toString(node_time.getTimeSource());

        if (((node_time.getTimeSource() == timeSource_t::ESPEASY_p2p_UDP) ||
             (node_time.getTimeSource() == timeSource_t::ESP_now_peer)) &&
            (node_time.timeSource_p2p_unit != 0))
        {
          timeSource_str = strformat(F("%s (%u)"), timeSource_str.c_str(), node_time.timeSource_p2p_unit);
        }

        KeyValueStruct kv(F("Time Source"), timeSource_str);
        return kv;
      }
      break;
    case LabelType::TIME_WANDER:

      if (node_time.systemTimePresent())
      {
        KeyValueStruct kv(F("Time Wander"), node_time.timeWander, 3);
        kv.setUnit(F("ppm"));
        return kv;
      }
      break;
    #if FEATURE_EXT_RTC
    case LabelType::EXT_RTC_UTC_TIME:
    {
      if (Settings.ExtTimeSource() == ExtTimeSource_e::None) { break; }
      String rtcTime = F("Not Set");

      // Try to read the stored time in the ext. time source to allow to check if it is working properly.
      uint32_t unixtime;

      if (node_time.ExtRTC_get(unixtime)) {
        struct tm RTC_time;
        breakTime(unixtime, RTC_time);
        rtcTime = formatDateTimeString(RTC_time);
      }
      KeyValueStruct kv(F("UTC time stored in RTC chip"), rtcTime);
      return kv;
    }
    #endif // if FEATURE_EXT_RTC
    case LabelType::UPTIME:
    {
      if (extendedValue) {
        KeyValueStruct kv(F("Uptime"), minutesToDayHourMinute(getUptimeMinutes()));
        return kv;
      } else {
        KeyValueStruct kv(F("Uptime"), getUptimeMinutes());
        return kv;
      }
    }
    case LabelType::LOAD_PCT:

      if (wdcounter > 0)
      {
        if (extendedValue) {
          KeyValueStruct kv(F("Load"), strformat(
                              F("%.2f [%%] (LC=%d)"),
                              getCPUload(),
                              getLoopCountPerSec()));
          return kv;
        }
        KeyValueStruct kv(F("Load"), getCPUload(), 2);
        kv.setUnit(F("%"));
        return kv;
      }
      break;
    case LabelType::LOOP_COUNT:
    {
      if (extendedValue) { break; }
      KeyValueStruct kv(F("Load LC"), getLoopCountPerSec());
      return kv;
    }
    case LabelType::CPU_ECO_MODE:
    {
      KeyValueStruct kv(F("CPU Eco Mode"), Settings.EcoPowerMode());
      return kv;
    }
#if FEATURE_SET_WIFI_TX_PWR
    case LabelType::WIFI_TX_MAX_PWR:
    {
      KeyValueStruct kv(F("Max WiFi TX Power"), Settings.getWiFi_TX_power(), 2);
      kv.setUnit(F("dBm"));
      return kv;
    }
    case LabelType::WIFI_CUR_TX_PWR:
    {
      KeyValueStruct kv(F("Current WiFi TX Power"), ESPEasy::net::wifi::GetWiFiTXpower(), 2);
      kv.setUnit(F("dBm"));
      return kv;
    }
    case LabelType::WIFI_SENS_MARGIN:
    {
      KeyValueStruct kv(F("WiFi Sensitivity Margin"), Settings.WiFi_sensitivity_margin);
      kv.setUnit(F("dB"));
      return kv;
    }
    case LabelType::WIFI_SEND_AT_MAX_TX_PWR:
    {
      KeyValueStruct kv(F("Send With Max TX Power"), Settings.UseMaxTXpowerForSending());
      return kv;
    }
#endif // if FEATURE_SET_WIFI_TX_PWR
    case LabelType::WIFI_NR_EXTRA_SCANS:
    {
      KeyValueStruct kv(F("Extra WiFi scan loops"), Settings.NumberExtraWiFiScans);
      return kv;
    }
    case LabelType::WIFI_USE_LAST_CONN_FROM_RTC:
    {
      KeyValueStruct kv(F("Use Last Connected AP from RTC"), Settings.UseLastWiFiFromRTC());
      return kv;
    }

    case LabelType::FREE_MEM:
    {
#ifndef BUILD_NO_RAM_TRACKER

      if (extendedValue) {
        KeyValueStruct kv(F("Free RAM"),
                          strformat(
                            F("%d [byte] (%d - %s)"),
                            FreeMem(),
                            lowestRAM,
                            lowestRAMfunction.c_str()));
        return kv;
      }
#endif // ifndef BUILD_NO_RAM_TRACKER

      KeyValueStruct kv(F("Free RAM"), FreeMem());
      kv.setUnit(F("byte"));
      return kv;
    }
    case LabelType::FREE_STACK:
    {
#ifndef BUILD_NO_RAM_TRACKER

      if (extendedValue) {
        KeyValueStruct kv(F("Free Stack"),
                          strformat(
                            F("%d [byte] (%d - %s)"),
                            getCurrentFreeStack(),
                            lowestFreeStack,
                            lowestFreeStackfunction.c_str()));

        return kv;
      }
#endif // ifndef BUILD_NO_RAM_TRACKER
      KeyValueStruct kv(F("Free Stack"), getCurrentFreeStack());
      kv.setUnit(F("byte"));

      return kv;
    }
#ifdef USE_SECOND_HEAP
    case LabelType::FREE_HEAP_IRAM:
    {
      KeyValueStruct kv(F("Free 2nd Heap"), FreeMem2ndHeap());
      kv.setUnit(F("byte"));
      return kv;
    }
#endif // ifdef USE_SECOND_HEAP

#if defined(CORE_POST_2_5_0) || defined(ESP32)
  # ifndef LIMIT_BUILD_SIZE
    case LabelType::HEAP_MAX_FREE_BLOCK:
    {
      KeyValueStruct kv(F("Heap Max Free Block"),
#  ifdef ESP32
                        ESP.getMaxAllocHeap()
#  else
                        ESP.getMaxFreeBlockSize()
#  endif // ifdef ESP32
                        );
      kv.setUnit(F("byte"));
      return kv;
    }
  # endif // ifndef LIMIT_BUILD_SIZE
#endif // if defined(CORE_POST_2_5_0) || defined(ESP32)
#if defined(CORE_POST_2_5_0)
  # ifndef LIMIT_BUILD_SIZE
    case LabelType::HEAP_FRAGMENTATION:
    {
      KeyValueStruct kv(F("Heap Fragmentation"), ESP.getHeapFragmentation());
      kv.setUnit(F("%"));
      return kv;
    }
  # endif // ifndef LIMIT_BUILD_SIZE
#endif // if defined(CORE_POST_2_5_0)

#ifdef ESP32
    case LabelType::HEAP_SIZE:
    {
      KeyValueStruct kv(F("Heap Size"), ESP.getHeapSize());
      kv.setUnit(F("byte"));
      return kv;
    }
    case LabelType::HEAP_MIN_FREE:
    {
      KeyValueStruct kv(F("Heap Min Free"), ESP.getMinFreeHeap());
      kv.setUnit(F("byte"));
      return kv;
    }
    # ifdef BOARD_HAS_PSRAM
    case LabelType::PSRAM_SIZE:
    {
      if (!UsePSRAM()) { break; }
      KeyValueStruct kv(F("PSRAM Size"), ESP.getPsramSize());
      kv.setUnit(F("byte"));
      return kv;
    }
    case LabelType::PSRAM_FREE:
    {
      if (!UsePSRAM()) { break; }
      KeyValueStruct kv(F("PSRAM Free"), ESP.getFreePsram());
      kv.setUnit(F("byte"));
      return kv;
    }
    case LabelType::PSRAM_MIN_FREE:
    {
      if (!UsePSRAM()) { break; }
      KeyValueStruct kv(F("PSRAM Min Free"), ESP.getMinFreePsram());
      kv.setUnit(F("byte"));
      return kv;
    }
    case LabelType::PSRAM_MAX_FREE_BLOCK:
    {
      if (!UsePSRAM()) { break; }
      KeyValueStruct kv(F("PSRAM Max Free Block"), ESP.getMaxAllocPsram());
      kv.setUnit(F("byte"));
      return kv;
    }
    # endif // BOARD_HAS_PSRAM
#endif // ifdef ESP32

    case LabelType::JSON_BOOL_QUOTES:
    {
      KeyValueStruct kv(F("JSON bool output without quotes"), Settings.JSONBoolWithoutQuotes());
      return kv;
    }
#if FEATURE_TIMING_STATS
    case LabelType::ENABLE_TIMING_STATISTICS:
    {
      KeyValueStruct kv(F("Collect Timing Statistics"), Settings.EnableTimingStats());
      return kv;
    }
#endif // if FEATURE_TIMING_STATS
    case LabelType::ENABLE_RULES_CACHING:
    {
      KeyValueStruct kv(F("Enable Rules Cache"), Settings.EnableRulesCaching());
      return kv;
    }
    case LabelType::ENABLE_SERIAL_PORT_CONSOLE:
    {
      KeyValueStruct kv(F("Enable Serial Port Console"), !!Settings.UseSerial);
      return kv;
    }
    case LabelType::CONSOLE_SERIAL_PORT:
    {
      KeyValueStruct kv(F("Console Serial Port"), ESPEasy_Console.getPortDescription());
      return kv;
    }
#if USES_ESPEASY_CONSOLE_FALLBACK_PORT
    case LabelType::CONSOLE_FALLBACK_TO_SERIAL0:
    {
      KeyValueStruct kv(F("Fallback to Serial 0"), Settings.console_serial0_fallback);
      return kv;
    }
    case LabelType::CONSOLE_FALLBACK_PORT:
    {
      KeyValueStruct kv(F("Console Fallback Port"), ESPEasy_Console.getFallbackPortDescription());
      return kv;
    }
#endif // if USES_ESPEASY_CONSOLE_FALLBACK_PORT

    //    case LabelType::ENABLE_RULES_EVENT_REORDER: {
    // KeyValueStruct kv(F("Optimize Rules Cache Event Order"), Settings.EnableRulesEventReorder());
    // return kv;
    // } // TD-er: Disabled for now
    case LabelType::TASKVALUESET_ALL_PLUGINS:
    {
      KeyValueStruct kv(F("Allow TaskValueSet on all plugins"), Settings.AllowTaskValueSetAllPlugins());
      return kv;
    }
    case LabelType::ALLOW_OTA_UNLIMITED:
    {
      KeyValueStruct kv(F("Allow OTA without size-check"), Settings.AllowOTAUnlimited());
      return kv;
    }
#if FEATURE_CLEAR_I2C_STUCK
    case LabelType::ENABLE_CLEAR_HUNG_I2C_BUS:
    {
      KeyValueStruct kv(F("Try clear I2C bus when stuck"), Settings.EnableClearHangingI2Cbus());
      return kv;
    }
#endif // if FEATURE_CLEAR_I2C_STUCK
    #if FEATURE_I2C_DEVICE_CHECK
    case LabelType::ENABLE_I2C_DEVICE_CHECK:
    {
      KeyValueStruct kv(F("Check I2C devices when enabled"), Settings.CheckI2Cdevice());
      return kv;
    }
    #endif // if FEATURE_I2C_DEVICE_CHECK
#ifndef BUILD_NO_RAM_TRACKER
    case LabelType::ENABLE_RAM_TRACKING:
    {
      KeyValueStruct kv(F("Enable RAM Tracker"), Settings.EnableRAMTracking());
      return kv;
    }
#endif // ifndef BUILD_NO_RAM_TRACKER
#if FEATURE_AUTO_DARK_MODE
    case LabelType::ENABLE_AUTO_DARK_MODE:
    {
      KeyValueStruct kv(F("Web light/dark mode"), Settings.getCssMode());
      return kv;
    }
#endif // FEATURE_AUTO_DARK_MODE
#if FEATURE_RULES_EASY_COLOR_CODE
    case LabelType::DISABLE_RULES_AUTOCOMPLETE:
    {
      KeyValueStruct kv(F("Disable Rules auto-completion"), Settings.DisableRulesCodeCompletion());
      return kv;
    }
#endif // if FEATURE_RULES_EASY_COLOR_CODE
#if FEATURE_TARSTREAM_SUPPORT
    case LabelType::DISABLE_SAVE_CONFIG_AS_TAR:
    {
      KeyValueStruct kv(F("Disable Save Config as .tar"), Settings.DisableSaveConfigAsTar());
      return kv;
    }
#endif // if FEATURE_TARSTREAM_SUPPORT
#if FEATURE_TASKVALUE_UNIT_OF_MEASURE
    case LabelType::SHOW_UOM_ON_DEVICES_PAGE:
    {
      KeyValueStruct kv(F("Show Unit of Measure"), Settings.ShowUnitOfMeasureOnDevicesPage());
      return kv;
    }
    #endif // if FEATURE_TASKVALUE_UNIT_OF_MEASURE
    #if FEATURE_MQTT_CONNECT_BACKGROUND
    case LabelType::MQTT_CONNECT_IN_BACKGROUND:
    {
      KeyValueStruct kv(F("MQTT Connect in background"), Settings.MQTTConnectInBackground());
      return kv;
    }
    #endif // if FEATURE_MQTT_CONNECT_BACKGROUND

#if CONFIG_SOC_WIFI_SUPPORT_5G
    case LabelType::WIFI_BAND_MODE:
    {
      KeyValueStruct kv(F("WiFi Band Mode"), ESPEasy::net::wifi::getWifiBandModeString(Settings.WiFi_band_mode()));
      return kv;
    }
#endif // if CONFIG_SOC_WIFI_SUPPORT_5G

    case LabelType::BOOT_TYPE:
    {
      if (extendedValue) {
        KeyValueStruct kv(
          F("Boot"),
          concat(
            getLastBootCauseString(),
            strformat(F(" (%d)"), RTC.bootCounter)));
        return kv;
      }
      KeyValueStruct kv(F("Last Boot Cause"), getLastBootCauseString());
      return kv;
    }
    case LabelType::BOOT_COUNT:
    {
      KeyValueStruct kv(F("Boot Count"), RTC.bootCounter);
      return kv;
    }
    case LabelType::DEEP_SLEEP_ALTERNATIVE_CALL:
    {
      KeyValueStruct kv(F("Deep Sleep Alternative"), Settings.UseAlternativeDeepSleep());
      return kv;
    }
    case LabelType::RESET_REASON:
    {
      KeyValueStruct kv(F("Reset Reason"), getResetReasonString());
      return kv;
    }
    case LabelType::LAST_TASK_BEFORE_REBOOT:
    {
      KeyValueStruct kv(F("Last Action before Reboot"), ESPEasy_Scheduler::decodeSchedulerId(lastMixedSchedulerId_beforereboot));
      return kv;
    }
    case LabelType::SW_WD_COUNT:
    {
      KeyValueStruct kv(F("SW WD count"), sw_watchdog_callback_count);
      return kv;
    }

    case LabelType::WIFI_CONNECTION:
    {
      KeyValueStruct kv(F("WiFi Connection") /*, value*/);
      return kv;
    }
    case LabelType::WIFI_RSSI:
    {
      if (extendedValue) {
        KeyValueStruct kv(F("RSSI"), strformat(
                            F("%d [dBm] (%s)"),
                            WiFi.RSSI(),
                            WiFi.SSID().c_str()));
        return kv;
      }
      KeyValueStruct kv(F("RSSI"), WiFi.RSSI());
      kv.setUnit(F("dBm"));
      return kv;
    }
    case LabelType::IP_CONFIG:
    {
      KeyValueStruct kv(F("IP Config"), useStaticIP() ? F("static") : F("DHCP"));
      kv.setID(F("dhcp"));
      return kv;
    }
#if FEATURE_USE_IPV6
    case LabelType::IP6_LOCAL:

      if (Settings.EnableIPv6()) {
        KeyValueStruct kv(F("IPv6 link local"), formatIP(ESPEasy::net::NetworkLocalIP6(), true));
        return kv;
      }
      break;
    case LabelType::IP6_GLOBAL:

      if (Settings.EnableIPv6()) {
        KeyValueStruct kv(F("IPv6 global"), formatIP(ESPEasy::net::NetworkGlobalIP6()));
        return kv;
      }
      break;

      // case LabelType::IP6_ALL_ADDRESSES:      {
      // KeyValueStruct kv(F("IPv6 all addresses"));
      // IP6Addresses_t addresses = NetworkAllIPv6();
      // for (auto it = addresses.begin(); it != addresses.end(); ++it)
      // {
      //   kv.appendValue(it->toString());
      // }
      // return kv;
      // }
#endif // if FEATURE_USE_IPV6
    case LabelType::IP_ADDRESS:
    {
      KeyValueStruct kv(F("IP Address"), formatIP(ESPEasy::net::NetworkLocalIP()));
      kv.setID(F("ip"));
      return kv;
    }
    case LabelType::IP_SUBNET:
    {
      KeyValueStruct kv(F("IP Subnet"), formatIP(ESPEasy::net::NetworkSubnetMask()));
      kv.setID(F("subnet"));
      return kv;
    }
    case LabelType::IP_ADDRESS_SUBNET:
    {
      KeyValueStruct kv(F("IP / Subnet"), strformat(
                          F("%s / %s"),
                          getValue(LabelType::IP_ADDRESS).c_str(),
                          getValue(LabelType::IP_SUBNET).c_str()));
      return kv;
    }
    case LabelType::GATEWAY:
    {
      KeyValueStruct kv(F("Gateway"), formatIP(ESPEasy::net::NetworkGatewayIP()));
      kv.setID(F("gw"));
      return kv;
    }
    case LabelType::CLIENT_IP:
    {
      KeyValueStruct kv(F("Client IP"), formatIP(web_server.client().remoteIP(), true));
      return kv;
    }
    #if FEATURE_MDNS
    case LabelType::M_DNS:
    {
      const String url = NetworkGetHostname() + F(".local");

      if (extendedValue) {
        KeyValueStruct kv(F("mDNS"),
                          strformat(
                            F("<a href='http://%s'>%s</a>"),
                            url.c_str(),
                            url.c_str()));
        return kv;
      }
      KeyValueStruct kv(F("mDNS"), url);
      return kv;
    }
    #endif // if FEATURE_MDNS
    case LabelType::DNS:
    {
      if (!extendedValue) { break; }
      KeyValueStruct kv(F("DNS"), strformat(F("%s / %s"),
                                            getValue(LabelType::DNS_1).c_str(),
                                            getValue(LabelType::DNS_2).c_str()));
      return kv;
    }
    case LabelType::DNS_1:
    {
      if (extendedValue) { break; }
      KeyValueStruct kv(F("DNS 1"), formatIP(ESPEasy::net::NetworkDnsIP(0)));
      kv.setID(F("dns1"));
      return kv;
    }
    case LabelType::DNS_2:
    {
      if (extendedValue) { break; }
      KeyValueStruct kv(F("DNS 2"), formatIP(ESPEasy::net::NetworkDnsIP(1)));
      kv.setID(F("dns2"));
      return kv;
    }
    case LabelType::ALLOWED_IP_RANGE:
    {
      KeyValueStruct kv(F("Allowed IP Range"), ESPEasy::net::describeAllowedIPrange());
      kv.setID(F("allowed_range"));
      return kv;
    }
    case LabelType::STA_MAC:
    {
      KeyValueStruct kv(F("STA MAC"), ESPEasy::net::WifiSTAmacAddress().toString());
      return kv;
    }
    case LabelType::AP_MAC:
    {
      KeyValueStruct kv(F("AP MAC"), ESPEasy::net::WifiSoftAPmacAddress().toString());
      return kv;
    }
    case LabelType::SSID:
    {
      KeyValueStruct kv(F("SSID"), WiFi.SSID());
      return kv;
    }
    case LabelType::BSSID:
    {
      KeyValueStruct kv(F("BSSID"), WiFi.BSSIDstr());
      return kv;
    }
    case LabelType::CHANNEL:
    {
      KeyValueStruct kv(F("Channel"), WiFi.channel());
      return kv;
    }
    case LabelType::ENCRYPTION_TYPE_STA:
    {
      KeyValueStruct kv(F("Encryption Type"), getWiFi_encryptionType());
      kv.setID(F("encryption"));
      return kv;
    }
    case LabelType::CONNECTED:
    {
      KeyValueStruct kv(F("Connected"), format_msec_duration(ESPEasy::net::NetworkConnectDuration_ms()));
      return kv;
    }
    case LabelType::CONNECTED_MSEC:
    {
      KeyValueStruct kv(F("Connected msec"), ESPEasy::net::NetworkConnectDuration_ms());
      return kv;
    }
    case LabelType::LAST_DISCONNECT_REASON:
    {
      KeyValueStruct kv(F("Last Disconnect Reason"), getWiFi_disconnectReason());
      return kv;
    }
    case LabelType::LAST_DISC_REASON_STR:
    {
      KeyValueStruct kv(F("Last Disconnect Reason str"), getWiFi_disconnectReason_str());
      return kv;
    }
    case LabelType::NUMBER_RECONNECTS:
    {
      KeyValueStruct kv(F("Number Reconnects"), ESPEasy::net::NetworkConnectCount());
      return kv;
    }
    case LabelType::WIFI_STORED_SSID1:
    {
      KeyValueStruct kv(F("Configured SSID1"), SecuritySettings.WifiSSID);
      return kv;
    }
    case LabelType::WIFI_STORED_SSID2:
    {
      KeyValueStruct kv(F("Configured SSID2"), SecuritySettings.WifiSSID2);
      return kv;
    }


    case LabelType::FORCE_WIFI_BG:
    {
      KeyValueStruct kv(F("Force WiFi B/G"), Settings.ForceWiFi_bg_mode());
      return kv;
    }
    case LabelType::RESTART_WIFI_LOST_CONN:
    {
      KeyValueStruct kv(F("Restart WiFi Lost Conn"), Settings.WiFiRestart_connection_lost());
      return kv;
    }
    case LabelType::FORCE_WIFI_NOSLEEP:
    {
      KeyValueStruct kv(F("Force WiFi No Sleep"), Settings.WifiNoneSleep());
      return kv;
    }
    case LabelType::PERIODICAL_GRAT_ARP:
    {
      KeyValueStruct kv(F("Periodical send Gratuitous ARP"), Settings.gratuitousARP());
      return kv;
    }
    case LabelType::CONNECTION_FAIL_THRESH:
    {
      KeyValueStruct kv(F("Connection Failure Threshold"), Settings.ConnectionFailuresThreshold);
      return kv;
    }
#ifndef ESP32
    case LabelType::WAIT_WIFI_CONNECT:
    {
      KeyValueStruct kv(F("Extra Wait WiFi Connect"), Settings.WaitWiFiConnect());
      return kv;
    }
#endif // ifndef ESP32
    case LabelType::CONNECT_HIDDEN_SSID:
    {
      KeyValueStruct kv(F("Include Hidden SSID"), Settings.IncludeHiddenSSID());
      return kv;
    }
#ifdef ESP32
    case LabelType::WIFI_PASSIVE_SCAN:
    {
      KeyValueStruct kv(F("Passive WiFi Scan"), Settings.PassiveWiFiScan());
      return kv;
    }
#endif // ifdef ESP32
    case LabelType::HIDDEN_SSID_SLOW_CONNECT:
    {
      KeyValueStruct kv(F("Hidden SSID Slow Connect"), Settings.HiddenSSID_SlowConnectPerBSSID());
      return kv;
    }
    case LabelType::SDK_WIFI_AUTORECONNECT:
    {
      KeyValueStruct kv(F("Enable SDK WiFi Auto Reconnect"), Settings.SDK_WiFi_autoreconnect());
      return kv;
    }
#if FEATURE_USE_IPV6
    case LabelType::ENABLE_IPV6:
    {
      KeyValueStruct kv(F("Enable IPv6"), Settings.EnableIPv6());
      return kv;
    }
#endif // if FEATURE_USE_IPV6


    case LabelType::BUILD_DESC:
    {
      String descr = getSystemBuildString();

      if (extendedValue) {
        descr += ' ';
        descr += F(BUILD_NOTES);
      }
      KeyValueStruct kv(F("Build"), descr);
      return kv;
    }
    case LabelType::BUILD_ORIGIN:
    {
      KeyValueStruct kv(F("Build Origin"), get_build_origin());
      return kv;
    }
    case LabelType::GIT_BUILD:
    {
      String res(F(BUILD_GIT));

      if (res.isEmpty()) { res = get_git_head(); }
      KeyValueStruct kv(F("Git Build"), res);
      return kv;
    }
    case LabelType::SYSTEM_LIBRARIES:
    {
      KeyValueStruct kv(F("System Libraries"), getSystemLibraryString());
      return kv;
    }
#ifdef ESP32
    case LabelType::ESP_IDF_SDK_VERSION:
    {
      KeyValueStruct kv(F("ESP-IDF Version"), strformat(
                          F("%d.%d.%d"),
                          ESP_IDF_VERSION_MAJOR,
                          ESP_IDF_VERSION_MINOR,
                          ESP_IDF_VERSION_PATCH));
      return kv;
    }
#endif // ifdef ESP32
    case LabelType::PLUGIN_COUNT:
    {
      KeyValueStruct kv(F("Plugin Count"), getDeviceCount() + 1);
      return kv;
    }
    case LabelType::PLUGIN_DESCRIPTION:
    {
      KeyValueStruct kv(F("Plugin Description"), getPluginDescriptionString());
      return kv;
    }
    case LabelType::BUILD_TIME:
    {
      KeyValueStruct kv(F("Build Time"), String(get_build_date()) + ' ' + get_build_time());
      return kv;
    }
    case LabelType::BINARY_FILENAME:
    {
      KeyValueStruct kv(F("Binary Filename"), get_binary_filename());
      return kv;
    }
    case LabelType::BUILD_PLATFORM:
    {
      KeyValueStruct kv(F("Build Platform"), get_build_platform());
      return kv;
    }
    case LabelType::GIT_HEAD:
    {
      KeyValueStruct kv(F("Git HEAD"), get_git_head());
      return kv;
    }
    #ifdef CONFIGURATION_CODE
    case LabelType::CONFIGURATION_CODE_LBL:
    {
      KeyValueStruct kv(F("Configuration code"), getConfigurationCode());
      return kv;
    }
    #endif // ifdef CONFIGURATION_CODE

    case LabelType::I2C_BUS_STATE:
    {
      KeyValueStruct kv(F("I2C Bus State"), toString(I2C_state));
      return kv;
    }
    case LabelType::I2C_BUS_CLEARED_COUNT:
    {
      KeyValueStruct kv(F("I2C bus cleared count"), I2C_bus_cleared_count);
      return kv;
    }

    case LabelType::SYSLOG_LOG_LEVEL:
    {
      KeyValueStruct kv(F("Syslog Log Level"), getLogLevelDisplayString(Settings.SyslogLevel));
      return kv;
    }
    case LabelType::SERIAL_LOG_LEVEL:
    {
      KeyValueStruct kv(F("Serial Log Level"), getLogLevelDisplayString(getSerialLogLevel()));
      return kv;
    }
    case LabelType::WEB_LOG_LEVEL:
    {
      KeyValueStruct kv(F("Web Log Level"), getLogLevelDisplayString(getWebLogLevel()));
      return kv;
    }
  #if FEATURE_SD
    case LabelType::SD_LOG_LEVEL:
    {
      KeyValueStruct kv(F("SD Log Level"), getLogLevelDisplayString(Settings.SDLogLevel));
      return kv;
    }
  #endif // if FEATURE_SD

    case LabelType::ESP_CHIP_ID:
    {
      KeyValueStruct kv(F("ESP Chip ID"), formatToHex(getChipId(), 6));
      return kv;
    }
    case LabelType::ESP_CHIP_FREQ:
    {
      KeyValueStruct kv(F("ESP Chip Frequency"), ESP.getCpuFreqMHz());
      kv.setUnit(F("MHz"));
      return kv;
    }
#ifdef ESP32
    case LabelType::ESP_CHIP_XTAL_FREQ:
    {
      KeyValueStruct kv(F("ESP Crystal Frequency"), getXtalFrequencyMHz());
      kv.setUnit(F("MHz"));
      return kv;
    }
    case LabelType::ESP_CHIP_APB_FREQ:
    {
      KeyValueStruct kv(F("ESP APB Frequency"), rtc_clk_apb_freq_get() / 1000000);
      kv.setUnit(F("MHz"));
      return kv;
    }
#endif // ifdef ESP32
    case LabelType::ESP_CHIP_MODEL:
    {
      KeyValueStruct kv(F("ESP Chip Model"), getChipModel());
      return kv;
    }
    case LabelType::ESP_CHIP_REVISION:
    {
      KeyValueStruct kv(F("ESP Chip Revision"), getChipRevision());
      return kv;
    }
    case LabelType::ESP_CHIP_CORES:
    {
      KeyValueStruct kv(F("ESP Chip Cores"), getChipCores());
      return kv;
    }

    case LabelType::BOARD_NAME:
    {
      KeyValueStruct kv(F("ESP Board Name"), get_board_name());
      return kv;
    }

    case LabelType::FLASH_CHIP_ID:
    {
      auto flashChipId = getFlashChipId();

      if (flashChipId == 0) { break; }
      KeyValueStruct kv(F("Flash Chip ID"), formatToHex(flashChipId, 6));
      return kv;
    }
    case LabelType::FLASH_CHIP_VENDOR:
    {
      const uint32_t flashChipId = getFlashChipId();

      if (flashChipId == 0) { break; }
      String id = formatToHex(flashChipId & 0xFF, 2);

      if (extendedValue && flashChipVendorPuya()) {
        id += concat(F(" (PUYA"), puyaSupport() ? F(", supported") : F(HTML_SYMBOL_WARNING)) + ')';
      }

      KeyValueStruct kv(F("Flash Chip Vendor"), id);
      return kv;
    }
    case LabelType::FLASH_CHIP_MODEL:
    {
      const uint32_t flashChipId = getFlashChipId();
      const uint32_t flashDevice = (flashChipId & 0xFF00) | ((flashChipId >> 16) & 0xFF);

      String model(formatToHex(flashDevice, 4));
          #ifdef ESP32

      if (extendedValue && getChipFeatures().embeddedFlash) {
        model += F(" (Embedded)");
      }
    #endif // ifdef ESP32
      KeyValueStruct kv(F("Flash Chip Model"), model);
      return kv;
    }
    case LabelType::FLASH_CHIP_REAL_SIZE:
    {
      KeyValueStruct kv(F("Flash Chip Real Size"), getFlashRealSizeInBytes() >> 10);
      kv.setUnit(F("kB"));
      return kv;
    }
    case LabelType::FLASH_CHIP_SPEED:
    {
      KeyValueStruct kv(F("Flash Chip Speed"), getFlashChipSpeed() / 1000000);
      kv.setUnit(F("MHz"));
      return kv;
    }
    case LabelType::FLASH_IDE_SIZE:
    {
      KeyValueStruct kv(F("Flash IDE Size"), ESP.getFlashChipSize() >> 10);
      kv.setUnit(F("kB"));
      return kv;
    }
    case LabelType::FLASH_IDE_SPEED:
    {
      KeyValueStruct kv(F("Flash IDE Speed"), getFlashChipSpeed() / 1000000);
      kv.setUnit(F("MHz"));
      return kv;
    }
    case LabelType::FLASH_IDE_MODE:
    {
      KeyValueStruct kv(F("Flash IDE Mode"), getFlashChipMode());
      kv.setID(F("mode"));
      return kv;
    }
    case LabelType::FLASH_WRITE_COUNT:
    {
      if (extendedValue) {
        KeyValueStruct kv(
          F("Flash Writes"),
          strformat(
            F("%d daily / %d cold boot"),
            RTC.flashDayCounter,
            static_cast<int>(RTC.flashCounter)));
        return kv;
      }
      KeyValueStruct kv(F("Flash Writes"), RTC.flashCounter);
      return kv;
    }
    case LabelType::SKETCH_SIZE:
    {
      String str;

      if (extendedValue) {
        uint32_t maxSketchSize;
        bool     use2step;
        OTA_possible(maxSketchSize, use2step);
        str += strformat(
          F("%d [kB] (%d kB not used)"),
          (getSketchSize() >> 10),
          (maxSketchSize - getSketchSize()) >> 10);
      } else {
        str = (getSketchSize() >> 10);
      }

      KeyValueStruct kv(F("Sketch Size"), str);
      kv.setID(F("sketch_size"));

      if (!extendedValue) {
        kv.setUnit(F("kB"));
      }
      return kv;
    }
    case LabelType::SKETCH_FREE:
    {
      KeyValueStruct kv(F("Sketch Free"), getFreeSketchSpace() >> 10);
      kv.setID(F("sketch_free"));
      kv.setUnit(F("kB"));
      return kv;
    }
    case LabelType::FS_SIZE:
    {
      String size;

      if (extendedValue) {
        size = strformat(
          F("%d [kB] (%d kB free)"),
          SpiffsTotalBytes() / 1024,
          SpiffsFreeSpace() / 1024);
      }
      else {
        size = (SpiffsTotalBytes() >> 10);
      }

      KeyValueStruct kv(
        #ifdef USE_LITTLEFS
        F("Little FS Size"),
        #else
        F("SPIFFS Size"),
        #endif // ifdef USE_LITTLEFS
        SpiffsTotalBytes() >> 10);
      kv.setID(F("fs_size"));

      if (!extendedValue) {
        kv.setUnit(F("kB"));
      }
      return kv;
    }
    case LabelType::FS_FREE:
    {
      KeyValueStruct kv(
        #ifdef USE_LITTLEFS
        F("Little FS Free"),
        #else
        F("SPIFFS Free"),
        #endif // ifdef USE_LITTLEFS
        SpiffsFreeSpace() >> 10);
      kv.setID(F("fs_free"));
      kv.setUnit(F("kB"));
      return kv;
    }
    case LabelType::MAX_OTA_SKETCH_SIZE:
    {
      uint32_t maxSketchSize;
      bool     use2step;
      OTA_possible(maxSketchSize, use2step);

      KeyValueStruct kv(F("Max. OTA Sketch Size"), strformat(
                          F("%d [kB] (%d bytes)"),
                          maxSketchSize / 1024,
                          maxSketchSize));
      return kv;
    }
#ifdef ESP8266
    case LabelType::OTA_2STEP:
    {
      uint32_t maxSketchSize;
      bool     use2step;
      OTA_possible(maxSketchSize, use2step);

      KeyValueStruct kv(F("OTA 2-step Needed"), use2step);
      return kv;
    }
    case LabelType::OTA_POSSIBLE:
    {
      uint32_t maxSketchSize;
      bool     use2step;
    # if defined(ESP8266)
      bool otaEnabled =
    # endif // if defined(ESP8266)
      OTA_possible(maxSketchSize, use2step);
      KeyValueStruct kv(F("OTA possible"), otaEnabled);
      return kv;
    }
#endif // ifdef ESP8266
    #if FEATURE_INTERNAL_TEMPERATURE
    case LabelType::INTERNAL_TEMPERATURE:
    {
      KeyValueStruct kv(F("Internal Temperature"), getInternalTemperature(), 1);
      kv.setUnit(F("&deg;C"));
      return kv;
    }
    #endif // if FEATURE_INTERNAL_TEMPERATURE
#if FEATURE_ETHERNET
    case LabelType::ETH_MAC:
    {
      KeyValueStruct kv(F("Eth MAC"), ESPEasy::net::NetworkMacAddress().toString());
      return kv;
    }
    case LabelType::ETH_DUPLEX:
    {
      KeyValueStruct kv(F("Eth Mode"), ESPEasy::net::EthLinkUp() ?
                        (ESPEasy::net::EthFullDuplex() ? F("Full Duplex") : F("Half Duplex")) : F("Link Down"));
      kv.setID(F("ethduplex"));
      return kv;
    }
    case LabelType::ETH_SPEED:
    {
      KeyValueStruct kv(F("Eth Speed"), getEthSpeed());
      kv.setID(F("ethspeed"));
      kv.setUnit(F("Mbps"));
      return kv;
    }
    case LabelType::ETH_STATE:
    {
      KeyValueStruct kv(F("Eth State"), ESPEasy::net::EthLinkUp() ? F("Link Up") : F("Link Down"));
      kv.setID(F("ethstate"));
      return kv;
    }
    case LabelType::ETH_SPEED_STATE:
    {
      if (active_network_medium != ESPEasy::net::NetworkMedium_t::Ethernet) { break; }
      KeyValueStruct kv(F("Eth Speed State"), getEthLinkSpeedState());
      kv.setID(F("ethspeedstate"));
      return kv;
    }
    case LabelType::ETH_CONNECTED:
    {
      KeyValueStruct kv(F("Eth connected"), ESPEasy::net::eth::ETHConnected() ? F("CONNECTED") : F("DISCONNECTED"));
      kv.setID(F("ethconnected"));
      return kv;
    }
    case LabelType::ETH_CHIP:
    {
      KeyValueStruct kv(F("Eth chip"), toString(Settings.ETH_Phy_Type));
      kv.setID(F("ethchip"));
      return kv;
    }
#endif // if FEATURE_ETHERNET
#if FEATURE_ETHERNET || defined(USES_ESPEASY_NOW)
    case LabelType::ETH_WIFI_MODE:
    {
      KeyValueStruct kv(F("Network Type"), toString(active_network_medium));
      kv.setID(F("ethwifimode"));
      return kv;
    }
#endif // if FEATURE_ETHERNET || defined(USES_ESPEASY_NOW)
    case LabelType::SUNRISE:
    {
      KeyValueStruct kv(F("Sunrise"), node_time.getSunriseTimeString(':'));
      return kv;
    }
    case LabelType::SUNSET:
    {
      KeyValueStruct kv(F("Sunset"), node_time.getSunsetTimeString(':'));
      return kv;
    }
    case LabelType::SUNRISE_S:
    {
      KeyValueStruct kv(F("Sunrise sec."), node_time.sunRise.tm_hour * 3600 + node_time.sunRise.tm_min * 60 +
                        node_time.sunRise.tm_sec);
      return kv;
    }
    case LabelType::SUNSET_S:
    {
      KeyValueStruct kv(F("Sunset sec."), node_time.sunSet.tm_hour * 3600 + node_time.sunSet.tm_min * 60 + node_time.sunSet.tm_sec);
      return kv;
    }
    case LabelType::SUNRISE_M:
    {
      KeyValueStruct kv(F("Sunrise min."), node_time.sunRise.tm_hour * 60 + node_time.sunRise.tm_min);
      return kv;
    }
    case LabelType::SUNSET_M:
    {
      KeyValueStruct kv(F("Sunset min."), node_time.sunSet.tm_hour * 60 + node_time.sunSet.tm_min);
      return kv;
    }
    case LabelType::ISNTP:
    {
      KeyValueStruct kv(F("Use NTP"), Settings.UseNTP());
      return kv;
    }
    case LabelType::UPTIME_MS:
    {
      KeyValueStruct kv(F("Uptime (ms)"), getMicros64() / 1000);
      return kv;
    }
    case LabelType::TIMEZONE_OFFSET:
    {
      KeyValueStruct kv(F("Timezone Offset"), Settings.TimeZone);
      return kv;
    }
    case LabelType::LATITUDE:
    {
      KeyValueStruct kv(F("Latitude"), Settings.Latitude, 6);
      return kv;
    }
    case LabelType::LONGITUDE:
    {
      KeyValueStruct kv(F("Longitude"), Settings.Longitude, 6);
      return kv;
    }

    case LabelType::MAX_LABEL:
      break;
  }
  return KeyValueStruct();
}

String getInternalLabel(LabelType::Enum label, char replaceSpace) {
  auto kv = getKeyValue(label);

  return kv.getID();
}

String getLabel(LabelType::Enum label) {
  auto kv = getKeyValue(label);

  if (kv._key.isEmpty()) {
    return F("MissingString");
  }
  return kv._key;
}

String getValue(LabelType::Enum label) {
  auto kv = getKeyValue(label);

  if (kv._values.size() && kv._values[0]) { return kv._values[0]->toString(); }
  return EMPTY_STRING;
}

#if FEATURE_ETHERNET

String getEthSpeed() {
  if (ESPEasy::net::EthLinkUp()) {
    return strformat(F("%d [Mbps]"), ESPEasy::net::EthLinkSpeed());
  }
  return getValue(LabelType::ETH_STATE);
}

String getEthLinkSpeedState() {
  if (ESPEasy::net::EthLinkUp()) {
    return strformat(F("%s %s %s"),
                     getValue(LabelType::ETH_STATE).c_str(),
                     getValue(LabelType::ETH_DUPLEX).c_str(),
                     getEthSpeed().c_str());
  }
  return getValue(LabelType::ETH_STATE);
}

#endif // if FEATURE_ETHERNET

String getExtendedValue(LabelType::Enum label) {
  switch (label)
  {
    case LabelType::UPTIME:
    {
      return minutesToDayHourMinute(getUptimeMinutes());
    }

    default:
      break;
  }
  return EMPTY_STRING;
}

String getFormNote(LabelType::Enum label)
{
  // Keep flash string till the end of the function, to reduce build size
  // Otherwise lots of calls to String() constructor are included.
  const __FlashStringHelper *flash_str = F("");

  switch (label)
  {
#ifndef MINIMAL_OTA
    case LabelType::CONNECT_HIDDEN_SSID:
      flash_str = F("Must be checked to connect to a hidden SSID");
      break;
# ifdef ESP32
    case LabelType::WIFI_PASSIVE_SCAN:
      flash_str = F("Passive scan listens for WiFi beacons, Active scan probes for AP. Passive scan is typically faster.");
      break;
# endif // ifdef ESP32
    case LabelType::HIDDEN_SSID_SLOW_CONNECT:
      flash_str = F("Required for some AP brands like Mikrotik to connect to hidden SSID");
      break;
# if FEATURE_USE_IPV6
    case LabelType::ENABLE_IPV6:
      flash_str = F("Toggling IPv6 requires reboot");
      break;
# endif // if FEATURE_USE_IPV6
# ifndef NO_HTTP_UPDATER
    case LabelType::ALLOW_OTA_UNLIMITED:
      flash_str = F("When enabled, OTA updating can overwrite the filesystem and settings!<br>Requires reboot to activate");
      break;
# endif // ifndef NO_HTTP_UPDATER
# if FEATURE_RULES_EASY_COLOR_CODE
    case LabelType::DISABLE_RULES_AUTOCOMPLETE:
      flash_str = F("Also disables Rules syntax highlighting!");
      break;
# endif // if FEATURE_RULES_EASY_COLOR_CODE

    case LabelType::FORCE_WIFI_NOSLEEP:
      flash_str = F("Change WiFi sleep settings requires reboot to activate");
      break;

    case LabelType::CPU_ECO_MODE:
      flash_str = F("Node may miss receiving packets with Eco mode enabled");
      break;

    case LabelType::WIFI_NR_EXTRA_SCANS:
      flash_str = F("Number of extra times to scan all channels to have higher chance of finding the desired AP");
      break;
# ifndef ESP32
    case LabelType::WAIT_WIFI_CONNECT:
      flash_str = F("Wait for 1000 msec right after connecting to WiFi.<BR>May improve success on some APs like Fritz!Box");
      break;
# endif // ifndef ESP32

#endif // ifndef MINIMAL_OTA

#if FEATURE_SET_WIFI_TX_PWR
    case LabelType::WIFI_TX_MAX_PWR:
    case LabelType::WIFI_SENS_MARGIN:
    {
      float maxTXpwr;
      float sensitivity = ESPEasy::net::wifi::GetRSSIthreshold(maxTXpwr);

      if (LabelType::WIFI_TX_MAX_PWR == label) {
        return strformat(
          F("Current max: %.2f dBm"), maxTXpwr);
      }
      return strformat(
        F("Adjust TX power to target the AP with (sensitivity + margin) dBm signal strength. Current sensitivity: %.2f dBm"),
        sensitivity);
    }
#endif // if FEATURE_SET_WIFI_TX_PWR

    default:
      return EMPTY_STRING;
  }

  return flash_str;
}

String getFormUnit(LabelType::Enum label)
{
  auto kv = getKeyValue(label);

  return kv._unit;
}
