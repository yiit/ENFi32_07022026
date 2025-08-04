#include "../DataStructs/NWPluginData_static_runtime.h"

#ifdef ESP32

namespace ESPEasy {
namespace net {

bool NWPluginData_static_runtime::connected() const
{
  if (!_netif) { return false; }
  return _netif->connected();
}

bool NWPluginData_static_runtime::isDefaultRoute() const
{
  if (!_netif) { return false; }
  return _netif->isDefault();
}

} // namespace net
} // namespace ESPEasy

#endif // ifdef ESP32
