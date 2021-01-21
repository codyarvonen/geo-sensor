#ifndef PM_H
#define PM_H

#include <Arduino.h>

class PM {
  public:
    int power = 9; //Pin number for MOSFET providing power to the PM Sensor

    double measure25();
    double measure10();
};

#endif //PM_H
