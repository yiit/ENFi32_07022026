#include "../DataStructs/NWPluginData_static_runtime.h"

#ifdef ESP8266

namespace ESPEasy {
namespace net {

bool NWPluginData_static_runtime::connected() const
{
  if (_isSTA) { return WiFi.status() == WL_CONNECTED; }
  return false;
}

bool NWPluginData_static_runtime::isDefaultRoute() const
{
  // FIXME TD-er: Should I just return connected() here?
  return _isSTA;
}

} // namespace net
} // namespace ESPEasy

#endif // ifdef ESP8266
