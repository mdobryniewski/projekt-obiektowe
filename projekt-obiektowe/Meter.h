#pragma once
#include "PHSensor.h"
#include "TempSensor.h"
#include "NO3Sensor.h"
#include <utility>

class Meter {
private:
    PHSensor phSensor;
    TempSensor tempSensor;
    NO3Sensor no3Sensor;

public:
    Meter(PHSensor ph, TempSensor temp, NO3Sensor no3)
        : phSensor(std::move(ph)), tempSensor(std::move(temp)), no3Sensor(std::move(no3)) {
    }
};