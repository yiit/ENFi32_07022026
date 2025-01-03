#ifndef HELPERS_HTTPRESPONSEPARSER_H
#define HELPERS_HTTPRESPONSEPARSER_H

#include "../../ESPEasy_common.h"

// Function declarations
void eventFromResponse(const String& host,
                       const int   & httpCode,
                       const String& uri,
                       HTTPClient  & http);


#endif // OTHER_H
