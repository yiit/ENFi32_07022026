#include "../eth/ETH_NWPluginData_static_runtime.h"

#if FEATURE_ETHERNET


# include "../../../src/Helpers/StringConverter.h"


namespace ESPEasy {
namespace net {
namespace eth {

struct ETH_stats_and_cache_t {
  ETH_stats_and_cache_t(uint8_t eth_index, networkIndex_t networkIndex) :
    _networkIndex(networkIndex),
    _eth_index(eth_index),
    _eth(eth_index),
    _stats_and_cache(&_eth, concat(F("eth"), eth_index)) {
    _stats_and_cache.clear(networkIndex);
    if (_eth.getStatusBits() & ESP_NETIF_STARTED_BIT) {
      // Bit has been set, so it might be for this interface
      _stats_and_cache.mark_start();
    }
  }

  ~ETH_stats_and_cache_t() {
    _eth.end();
    _stats_and_cache.processEvent_and_clear();
  }

  void mark_start(esp_eth_handle_t eth_started)
  {
    if (_eth.handle() == eth_started) {
      _stats_and_cache.mark_start();
      _eth.enableIPv6(_stats_and_cache._enableIPv6);
    }
  }

  void mark_stop(esp_eth_handle_t eth_stopped)
  {
    if (_eth.handle() == eth_stopped) {
      _stats_and_cache.mark_stop();
    }
  }

  void mark_got_IP(ip_event_got_ip_t *event)
  {
    if (event && (_eth.netif() == event->esp_netif))
    {
      _stats_and_cache.mark_got_IP();
    }
  }

# if FEATURE_USE_IPV6

  void mark_got_IPv6(ip_event_got_ip6_t *event)
  {
    if (event && (_eth.netif() == event->esp_netif))
    {
      _stats_and_cache.mark_got_IPv6(event);
    }
  }

# endif // if FEATURE_USE_IPV6

  void mark_lost_IP(ip_event_got_ip_t *event)
  {
    if (event && (_eth.netif() == event->esp_netif)) {
      _stats_and_cache.mark_lost_IP();
    }
  }

  void mark_connected(esp_eth_handle_t eth_connected)
  {
    if (_eth.handle() == eth_connected) {
      _stats_and_cache.mark_connected();
    }
  }

  void mark_disconnected(esp_eth_handle_t eth_disconnected)
  {
    if (_eth.handle() == eth_disconnected) {
      _stats_and_cache.mark_disconnected();
    }
  }

  const networkIndex_t _networkIndex;

  const uint8_t               _eth_index;
  ETHClass                    _eth;
  NWPluginData_static_runtime _stats_and_cache;

};

DEF_UP(ETH_stats_and_cache_t);


UP_ETH_stats_and_cache_t ETH_stats_and_cache[3]{};

ETH_stats_and_cache_t* getStatsAndCache(networkIndex_t networkIndex)
{
  for (size_t i = 0; i < NR_ELEMENTS(ETH_stats_and_cache); ++i) {
    if (ETH_stats_and_cache[i] && (ETH_stats_and_cache[i]->_networkIndex == networkIndex)) {
      return ETH_stats_and_cache[i].get();
    }
  }
  return nullptr;
}

ETH_NWPluginData_static_runtime::ETH_NWPluginData_static_runtime()
{
  nw_event_id = Network.onEvent(ETH_NWPluginData_static_runtime::onEvent);
}

ETH_NWPluginData_static_runtime::~ETH_NWPluginData_static_runtime()
{
  if (nw_event_id != 0) {
    Network.removeEvent(nw_event_id);
  }
  nw_event_id = 0;
}

ETHClass * ETH_NWPluginData_static_runtime::getInterface(networkIndex_t networkIndex)
{
  auto data = getStatsAndCache(networkIndex);

  if (data) { return &data->_eth; }

  return nullptr;
}

NWPluginData_static_runtime * ETH_NWPluginData_static_runtime::getNWPluginData_static_runtime(networkIndex_t networkIndex)
{
  auto data = getStatsAndCache(networkIndex);

  if (data) { return &data->_stats_and_cache; }

  return nullptr;
}

ETHClass * ETH_NWPluginData_static_runtime::init(networkIndex_t networkIndex)
{
  auto data = getStatsAndCache(networkIndex);

  if (data) {
    // TODO TD-er: What to do here? This should not occur
    return &data->_eth;
  }

  for (size_t i = 0; i < NR_ELEMENTS(ETH_stats_and_cache); ++i) {
    if (!ETH_stats_and_cache[i]) {
      // Found an empty slot
        if (nw_event_id != 0) {
    Network.removeEvent(nw_event_id);
  }

      ETH_stats_and_cache[i].reset(new (std::nothrow) ETH_stats_and_cache_t(i, networkIndex));
      nw_event_id = Network.onEvent(ETH_NWPluginData_static_runtime::onEvent);

      if (ETH_stats_and_cache[i]) { return &ETH_stats_and_cache[i]->_eth; }
      return nullptr;
    }
  }
  return nullptr;
}

void ETH_NWPluginData_static_runtime::exit(networkIndex_t networkIndex)
{
  for (size_t i = 0; i < NR_ELEMENTS(ETH_stats_and_cache); ++i) {
    if (ETH_stats_and_cache[i] && (ETH_stats_and_cache[i]->_networkIndex == networkIndex)) {
      ETH_stats_and_cache[i].reset(nullptr);
      return;
    }
  }
}

void ETH_NWPluginData_static_runtime::onEvent(
  arduino_event_id_t   event,
  arduino_event_info_t info)
{
  switch (event)
  {
    case ARDUINO_EVENT_ETH_START:

      for (size_t i = 0; i < NR_ELEMENTS(ETH_stats_and_cache); ++i) {
        if (ETH_stats_and_cache[i]) { ETH_stats_and_cache[i]->mark_start(info.eth_started); }
      }
      break;
    case ARDUINO_EVENT_ETH_STOP:

      for (size_t i = 0; i < NR_ELEMENTS(ETH_stats_and_cache); ++i) {
        if (ETH_stats_and_cache[i]) { ETH_stats_and_cache[i]->mark_stop(info.eth_stopped); }
      }
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:

      for (size_t i = 0; i < NR_ELEMENTS(ETH_stats_and_cache); ++i) {
        if (ETH_stats_and_cache[i]) { ETH_stats_and_cache[i]->mark_connected(info.eth_connected); }
      }
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:

      for (size_t i = 0; i < NR_ELEMENTS(ETH_stats_and_cache); ++i) {
        if (ETH_stats_and_cache[i]) { ETH_stats_and_cache[i]->mark_disconnected(info.eth_disconnected); }
      }

      break;
    case ARDUINO_EVENT_ETH_GOT_IP:

      for (size_t i = 0; i < NR_ELEMENTS(ETH_stats_and_cache); ++i) {
        if (ETH_stats_and_cache[i]) { ETH_stats_and_cache[i]->mark_got_IP(&info.got_ip); }
      }
      break;
# if FEATURE_USE_IPV6
    case ARDUINO_EVENT_ETH_GOT_IP6:

      for (size_t i = 0; i < NR_ELEMENTS(ETH_stats_and_cache); ++i) {
        if (ETH_stats_and_cache[i]) { ETH_stats_and_cache[i]->mark_got_IPv6(&info.got_ip6); }
      }
      break;
# endif // if FEATURE_USE_IPV6
    case ARDUINO_EVENT_ETH_LOST_IP:

      for (size_t i = 0; i < NR_ELEMENTS(ETH_stats_and_cache); ++i) {
        if (ETH_stats_and_cache[i]) { ETH_stats_and_cache[i]->mark_lost_IP(&info.lost_ip); }
      }
      break;

    default: break;
  }
}

} // namespace eth
} // namespace net
} // namespace ESPEasy


#endif // if FEATURE_ETHERNET
