#include "../Globals/SPI1.h"

#ifdef ESP32
SPIClass SPI1(HSPI);
#endif // ifdef ESP32
