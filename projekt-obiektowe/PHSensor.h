#pragma once
#include "Sensor.h"

class PHSensor : public Sensor {
public:
    PHSensor(double minV = 4.0, double maxV = 9.0)
        : Sensor(minV, maxV) {
    }

    std::string getName() const override { return "PHSensor"; }
};