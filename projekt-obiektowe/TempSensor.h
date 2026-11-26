#pragma once
#include "Sensor.h"
#include <string>

class TempSensor : public Sensor {
public:
    std::string nazwa;
    double temperatura = 0.0;

    TempSensor(std::string n = "Unknown", double minV = 0.0, double maxV = 40.0)
        : Sensor(minV, maxV), nazwa(n) {
    }

    std::string getName() const override { return "TempSensor (" + nazwa + ")"; }

    void setTemperatura(double t) {
        pushMeasurement(t);
        temperatura = measurements.empty() ? 0.0 : measurements.back();
    }
};