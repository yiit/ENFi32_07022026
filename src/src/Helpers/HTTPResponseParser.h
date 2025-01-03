#ifndef HELPERS_HTTPRESPONSEPARSER_H
#define HELPERS_HTTPRESPONSEPARSER_H

#include "../../ESPEasy_common.h"
#include "../../_Plugin_Helper.h"

#ifdef PLUGIN_BUILD_MINIMAL_OTA
# include <ESP8266HTTPClient.h>
#endif // ifdef ESP8266


// Function declarations
void eventFromResponse(const String& host,
                       const int   & httpCode,
                       const String& uri,
                       HTTPClient  & http);

#endif // ifndef HELPERS_HTTPRESPONSEPARSER_H
