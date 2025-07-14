#ifndef NWPLUGINSTRUCT_NW005_DATA_STRUCT_PPP_MODEM_H
#define NWPLUGINSTRUCT_NW005_DATA_STRUCT_PPP_MODEM_H


#include "../../_NWPlugin_Helper.h"
#ifdef USES_NW005


struct NW005_data_struct_PPP_modem : public NWPluginData_base {

  NW005_data_struct_PPP_modem();
  ~NW005_data_struct_PPP_modem();

  void testWrite();

  void testRead();

};


#endif // ifdef USES_NW005

#endif // ifndef NWPLUGINSTRUCT_NW005_DATA_STRUCT_PPP_MODEM_H
