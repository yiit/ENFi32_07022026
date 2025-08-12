#include "../Helpers/SPI_Helper.h"

void SPIInterfaceSelector(String  label,
                          String  id,
                          uint8_t choice,
                          bool disabled) {
  const uint8_t spiMaxBusCount = (getSPIBusCount() > 1
                                  ? ((Settings.isSPI_valid(1) ? 1 : 0)
                                     )
                                  : 0) + (Settings.isSPI_valid(0) ? 1 : 0);

  if (spiMaxBusCount > 1) {
    static uint8_t spiBusCount = 0;
    static int     spiBusNumbers[3];

    if (spiBusCount != spiMaxBusCount) {
      spiBusCount                = 0;
      spiBusNumbers[spiBusCount] = 0;
      ++spiBusCount;

      if ((getSPIBusCount() > 1) && Settings.isSPI_valid(1)) {
        spiBusNumbers[spiBusCount] = 1;
        ++spiBusCount;
      }
    }
    FormSelectorOptions selector(spiBusCount,
                                 spiBusNumbers);
    selector.default_index = 0;
    selector.enabled = !disabled;
    selector.addFormSelector(label, id, choice);
  }
}
