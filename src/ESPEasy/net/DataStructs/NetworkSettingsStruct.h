#pragma once

/*********************************************************************************************\
* NetworkSettingsStruct definition
\*********************************************************************************************/
#include "../../../ESPEasy_common.h"

#include <memory> // For std::unique_ptr
#include <new>    // for std::nothrow

#include "../../../src/Globals/Plugins.h"
#include "../../../src/Helpers/Memory.h"

namespace ESPEasy {
namespace net {


struct NetworkSettingsStruct
{
  // ********************************************************************************
  //   IDs of network settings, used to generate web forms
  // ********************************************************************************
  enum VarType {
    NETWORK_IP,
    NETWORK_PORT,
    NETWORK_USER,
    NETWORK_PASS,
    NETWORK_TIMEOUT,


    // Keep this as last, is used to loop over all parameters
    NETWORK_ENABLED

  };


  NetworkSettingsStruct();

  void reset();

  void validate();


  uint8_t      IP[4];
  unsigned int Port;
  unsigned int ClientTimeout;

private:

};

DEF_UP(NetworkSettingsStruct);
#define MakeNetworkSettings(T) void *calloc_ptr = special_calloc(1, sizeof(NetworkSettingsStruct)); \
        UP_NetworkSettingsStruct T(new (calloc_ptr)  NetworkSettingsStruct());

// Check to see if MakeNetworkSettings was successful
#define AllocatedNetworkSettings() (NetworkSettings.get() != nullptr)

} // namespace net
} // namespace ESPEasy
