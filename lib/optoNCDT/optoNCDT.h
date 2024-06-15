#ifndef optoNCDT_h
#define optoNCDT_h

#include "Arduino.h"

class optoNCDT
{
public:
    optoNCDT();
    void begin(uint32_t baud, uint16_t format);
    void setMeasuringRange(int range);
    float optoMeas();
    String optoCmd(String cmd);

private:
    bool serTimeout(uint8_t *byte);
    int MR;
};

#endif