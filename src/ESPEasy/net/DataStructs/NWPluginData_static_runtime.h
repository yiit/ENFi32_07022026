#pragma once

#include "../../../src/Helpers/LongTermOnOffTimer.h"
#include <IPAddress.h>
#ifdef ESP32
# include <NetworkInterface.h>
#endif

namespace ESPEasy {
namespace net {

struct NWPluginData_static_runtime {
#ifdef ESP32
  NWPluginData_static_runtime(NetworkInterface *netif) : _netif(netif) {}

#else // ifdef ESP32
  NWPluginData_static_runtime(bool isSTA) : _isSTA(isSTA) {}

#endif // ifdef ESP32

  void clear();

  bool connected() const;

  bool isDefaultRoute() const;


  LongTermOnOffTimer _startStopStats{};
  LongTermOnOffTimer _connectedStats{};
  LongTermOnOffTimer _gotIPStats{};
#if FEATURE_USE_IPV6
  LongTermOnOffTimer _gotIP6Stats{};
#endif
#ifdef ESP32
  IPAddress         _dns_cache[2]{};
  int               _route_prio = -1;
  NetworkInterface *_netif{};
#endif // ifdef ESP32
#ifdef ESP8266
  const bool _isSTA;
#endif

};


} // namespace net
} // namespace ESPEasy
