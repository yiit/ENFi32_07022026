#include "../DataStructs/NWPluginData_static_runtime.h"

#ifdef ESP8266

# include "../../../src/Helpers/StringConverter.h"

namespace ESPEasy {
namespace net {

NWPluginData_static_runtime::NWPluginData_static_runtime(bool isSTA, const char * eventInterfaceName)
  : _isSTA(isSTA), _eventInterfaceName(eventInterfaceName) {}

bool NWPluginData_static_runtime::started() const
{
  // FIXME TD-er: Does this work reliable on ESP8266?
  return _startStopStats.isOn();
}

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

bool NWPluginData_static_runtime::hasIP() const
{
  return _gotIPStats.isOn();
}

void NWPluginData_static_runtime::mark_start()
{
  _startStopStats.setOn();
  # ifndef BUILD_NO_DEBUG
  addLog(LOG_LEVEL_INFO, _isSTA ? F("STA: Started") : F("AP: Started"));
  # endif
}

void NWPluginData_static_runtime::mark_stop()
{
  _startStopStats.setOff();
  # ifndef BUILD_NO_DEBUG
  addLog(LOG_LEVEL_INFO, _isSTA ? F("STA: Stopped") : F("AP: Stopped"));
  # endif
}

void NWPluginData_static_runtime::mark_got_IP()
{
  // Set OnOffTimer to off so we can also count how often we het new IP
  _gotIPStats.forceSet(true);
  # ifndef BUILD_NO_DEBUG
  addLog(LOG_LEVEL_INFO, _isSTA ? F("STA: Got IP") : F("AP: Got IP"));
  # endif
}

void NWPluginData_static_runtime::mark_lost_IP()
{
  _gotIPStats.setOff();
  # ifndef BUILD_NO_DEBUG
  addLog(LOG_LEVEL_INFO, _isSTA ? F("STA: Lost IP") : F("AP: Lost IP"));
  # endif
}

void NWPluginData_static_runtime::mark_begin_establish_connection()
{
  _establishConnectStats.forceSet(true);

}

void NWPluginData_static_runtime::mark_connected()
{
# ifndef BUILD_NO_DEBUG
  const bool logDuration = _establishConnectStats.isOn();
# endif
  _establishConnectStats.setOff();
  _connectedStats.setOn();
}

void NWPluginData_static_runtime::log_connected()
{
# ifndef BUILD_NO_DEBUG

  if (loglevelActiveFor(LOG_LEVEL_INFO)) {
    if (_establishConnectStats.getCycleCount()) {
      addLog(LOG_LEVEL_INFO, concat(
               F("STA: Connected, took: "),
               format_msec_duration_HMS(
                 _establishConnectStats.getLastOnDuration_ms())));
    } else {
      addLog(LOG_LEVEL_INFO, F("STA: Connected"));
    }
  }
# endif // ifndef BUILD_NO_DEBUG
}

void NWPluginData_static_runtime::mark_disconnected()
{
  _connectedStats.setOff();
# ifndef BUILD_NO_DEBUG
# endif
}

void NWPluginData_static_runtime::log_disconnected()
{
# ifndef BUILD_NO_DEBUG

  if (_isSTA && loglevelActiveFor(LOG_LEVEL_INFO)) {
    addLog(LOG_LEVEL_INFO, concat(
             F("STA: Disconnected. Connected for: "),
             format_msec_duration_HMS(_connectedStats.getLastOnDuration_ms())));
  }
# endif // ifndef BUILD_NO_DEBUG
}

} // namespace net
} // namespace ESPEasy

#endif // ifdef ESP8266
