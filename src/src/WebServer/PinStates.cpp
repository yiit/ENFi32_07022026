#include "../WebServer/PinStates.h"

#include "../WebServer/ESPEasy_WebServer.h"
#include "../WebServer/HTML_wrappers.h"
#include "../WebServer/Markup.h"

#include "../DataStructs/PinMode.h"
#include "../Globals/GlobalMapPortStatus.h"
#include "../Helpers/PortStatus.h"

#ifdef ESP32
# include <esp32-hal-periman.h>
#endif // ifdef ESP32

#ifdef WEBSERVER_NEW_UI


// ********************************************************************************
// Web Interface pin state list
// ********************************************************************************
void handle_pinstates_json() {
  # ifndef BUILD_NO_RAM_TRACKER
  checkRAM(F("handle_pinstates"));
  # endif // ifndef BUILD_NO_RAM_TRACKER

  if (!isLoggedIn()) { return; }
  navMenuIndex = MENU_INDEX_TOOLS;
  TXBuffer.startJsonStream();

  bool first = true;
  addHtml('[');

  for (auto it = globalMapPortStatus.begin(); it != globalMapPortStatus.end(); ++it)
  {
    if (!first) {
      addHtml(',');
    } else {
      first = false;
    }
    addHtml('{');


    const uint16_t plugin = getPluginFromKey(it->first);
    const uint16_t port   = getPortFromKey(it->first);

    stream_next_json_object_value(F("plugin"),  plugin);
    stream_next_json_object_value(F("port"),    port);
    stream_next_json_object_value(F("state"),   it->second.state);
    stream_next_json_object_value(F("task"),    it->second.task);
    stream_next_json_object_value(F("monitor"), it->second.monitor);
    stream_next_json_object_value(F("command"), it->second.command);
    stream_last_json_object_value(F("init"), it->second.init);
  }

  addHtml(']');

  TXBuffer.endStream();
}

#endif // WEBSERVER_NEW_UI

#ifdef WEBSERVER_PINSTATES

void handle_pinstates() {
  # ifndef BUILD_NO_RAM_TRACKER
  checkRAM(F("handle_pinstates"));
  # endif // ifndef BUILD_NO_RAM_TRACKER

  if (!isLoggedIn()) { return; }
  navMenuIndex = MENU_INDEX_TOOLS;
  TXBuffer.startStream();
  sendHeadandTail_stdtemplate(_HEAD);

  # ifdef ESP32
  addFormSubHeader(F("Pin states"));
  # endif // ifdef ESP32

  html_table_class_multirow();
  html_TR();
  html_table_header(F("Plugin"), F("RTDPlugin/_Plugin.html#list-of-official-plugins"), 0);
  html_table_header(F("GPIO"));
  html_table_header(F("Mode"));
  html_table_header(F("Value/State"));
  html_table_header(F("Task"));
  html_table_header(F("Monitor"));
  html_table_header(F("Command"));
  html_table_header(F("Init"));

  for (auto it = globalMapPortStatus.begin(); it != globalMapPortStatus.end(); ++it)
  {
    html_TR_TD();
    const pluginID_t plugin = getPluginFromKey(it->first);
    const uint16_t   port   = getPortFromKey(it->first);
    addHtml(plugin.toDisplayString());
    html_TD();
    addHtmlInt(port);
    html_TD();
    addHtml(getPinModeString(it->second.mode));
    html_TD();
    addHtmlInt(it->second.getValue());
    html_TD();
    addHtmlInt(it->second.task);
    html_TD();
    addHtmlInt(it->second.monitor);
    html_TD();
    addHtmlInt(it->second.command);
    html_TD();
    addHtmlInt(it->second.init);
  }

  html_end_table();

# ifdef ESP32

  addFormSubHeader(F("Peripheral Bus Type per GPIO"));
  html_table_class_multirow();
  html_TR();
#  if defined(BOARD_HAS_PIN_REMAP)
  html_table_header(F("Dnnn|GPIO"));
#  else // if defined(BOARD_HAS_PIN_REMAP)
  html_table_header(F("GPIO"));
#  endif // if defined(BOARD_HAS_PIN_REMAP)
  html_table_header(F("Bus Type"));
  html_table_header(F("Bus Num"));
  html_table_header(F("Bus Channel"));

  for (int i = 0; i <= MAX_GPIO; ++i) {
    if (!perimanPinIsValid(i)) {
      continue; // invalid pin
    }
    peripheral_bus_type_t type = perimanGetPinBusType(i);

    if (type == ESP32_BUS_TYPE_INIT) {
      continue; // unused pin
    }


#  if defined(BOARD_HAS_PIN_REMAP)
    int dpin = gpioNumberToDigitalPin(i);

    if (dpin < 0) {
      continue; // pin is not exported
    } else {
      html_TR_TD();
      addHtml(strformat(F("D%-3d|%4u"), dpin, i));
    }
#  else // if defined(BOARD_HAS_PIN_REMAP)
    html_TR_TD();
    addHtml(strformat(F("%4u"), i));
#  endif // if defined(BOARD_HAS_PIN_REMAP)
    const char *extra_type = perimanGetPinBusExtraType(i);
    html_TD();

    if (extra_type) {
      addHtml(strformat(F("%s"), extra_type));
    } else {
      addHtml(strformat(F("%s"), perimanGetTypeName(type)));
    }
    int8_t bus_number = perimanGetPinBusNum(i);
    html_TD();

    if (bus_number != -1) {
      addHtmlInt(bus_number);
    }
    int8_t bus_channel = perimanGetPinBusChannel(i);
    html_TD();

    if (bus_channel != -1) {
      addHtmlInt(bus_channel);
    }
  }

  html_end_table();
# endif // ifdef ESP32
  sendHeadandTail_stdtemplate(_TAIL);
  TXBuffer.endStream();
}

#endif // ifdef WEBSERVER_PINSTATES
