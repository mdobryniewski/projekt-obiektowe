#pragma once
#include "Sensor.h"

class NO3Sensor : public Sensor {
public:
    NO3Sensor(double minV = 0.0, double maxV = 300.0)
        : Sensor(minV, maxV) {
    }

    std::string getName() const override { return "NO3Sensor"; }
};