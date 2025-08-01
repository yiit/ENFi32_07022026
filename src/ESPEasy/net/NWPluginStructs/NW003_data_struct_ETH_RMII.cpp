#include "../NWPluginStructs/NW003_data_struct_ETH_RMII.h"

#ifdef USES_NW003

# include "../../../src/Globals/Settings.h"

# include "../../../src/Helpers/ESPEasy_time_calc.h"
# include "../../../src/Helpers/LongTermOnOffTimer.h"
# include "../../../src/Helpers/StringConverter.h"


namespace ESPEasy {
namespace net {
namespace eth {

static LongTermOnOffTimer _startStopStats;
static LongTermOnOffTimer _connectedStats;
static LongTermOnOffTimer _gotIPStats;
# if FEATURE_USE_IPV6
static LongTermOnOffTimer _gotIP6Stats;
# endif
static IPAddress _dns_cache[2]{};

NW003_data_struct_ETH_RMII::NW003_data_struct_ETH_RMII(networkIndex_t networkIndex)
  : NWPluginData_base(nwpluginID_t(1), networkIndex)
{
  _connectedStats.clear();
  _gotIPStats.clear();
  _gotIP6Stats.clear();

  nw_event_id = Network.onEvent(NW003_data_struct_ETH_RMII::onEvent);
}

NW003_data_struct_ETH_RMII::~NW003_data_struct_ETH_RMII()
{
  if (nw_event_id != 0) {
    Network.removeEvent(nw_event_id);
  }
  nw_event_id = 0;
}

void NW003_data_struct_ETH_RMII::webform_load(EventStruct *event) {}

void NW003_data_struct_ETH_RMII::webform_save(EventStruct *event) {}

bool NW003_data_struct_ETH_RMII::webform_getPort(String& str)     { return true; }

bool NW003_data_struct_ETH_RMII::init(EventStruct *event)         { return true; }

bool NW003_data_struct_ETH_RMII::exit(EventStruct *event) {

  return true;
}

bool NW003_data_struct_ETH_RMII::handle_priority_route_changed()
{
  bool res{};

  if (ETH.isDefault()) {
    // Check to see if we may need to restore any cached DNS server
    for (size_t i = 0; i < NR_ELEMENTS(_dns_cache); ++i) {
      auto tmp = ETH.dnsIP(i);

      if ((_dns_cache[i] != INADDR_NONE) && (_dns_cache[i] != tmp)) {
        ETH.dnsIP(i, _dns_cache[i]);
        res = true;
      }
    }
  }
  return res;
}

void NW003_data_struct_ETH_RMII::onEvent(arduino_event_id_t   event,
                                         arduino_event_info_t info)
{
  switch (event)
  {
    case ARDUINO_EVENT_ETH_START:
      _startStopStats.setOn();
      addLog(LOG_LEVEL_INFO, F("ETH_START"));
      break;
    case ARDUINO_EVENT_ETH_STOP:
      _startStopStats.setOff();
      addLog(LOG_LEVEL_INFO, F("ETH_STOP"));
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      _connectedStats.setOn();
      addLog(LOG_LEVEL_INFO, F("ETH_CONNECTED"));
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      _connectedStats.setOff();
      addLog(LOG_LEVEL_INFO, F("ETH_DISCONNECTED"));
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:

      for (size_t i = 0; i < NR_ELEMENTS(_dns_cache); ++i) {
        auto tmp = ETH.dnsIP(i);

        if (tmp != INADDR_NONE) {
          _dns_cache[i] = tmp;
          addLog(LOG_LEVEL_INFO, strformat(F("DNS Cache %d set to %s"), i, tmp.toString(true).c_str()));
        }

      }
      _gotIPStats.setOn();
      addLog(LOG_LEVEL_INFO, F("ETH_GOT_IP"));
      break;
    case ARDUINO_EVENT_ETH_GOT_IP6:
      _gotIP6Stats.setOn();
      addLog(LOG_LEVEL_INFO, F("ETH_GOT_IP6"));
      break;
    case ARDUINO_EVENT_ETH_LOST_IP:
      _gotIPStats.setOff();
      _gotIP6Stats.setOff();

      addLog(LOG_LEVEL_INFO, F("ETH_LOST_IP"));
      break;

    default: break;
  }
}

} // namespace eth
} // namespace net
} // namespace ESPEasy

#endif // ifdef USES_NW003
