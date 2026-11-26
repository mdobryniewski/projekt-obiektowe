#pragma once
#include <vector>
#include <string>
#include "Logger.h"

class Sensor {
private:
    bool isValid(double value) const;

protected:
    std::vector<double> measurements;
    double* samples;
    int sampleCount;
    double minValue;
    double maxValue;

    void pushMeasurement(double val);

public:
    Sensor(double minV, double maxV);
    Sensor(const Sensor& other);
    Sensor(Sensor&& other) noexcept;
    virtual ~Sensor() = 0;

    Sensor& operator=(const Sensor& other);
    Sensor& operator=(Sensor&& other) noexcept;

    virtual std::string getName() const = 0;

    void read_data(const std::string& meas_file, const std::string& samp_file = "");
    void displayLast() const;

    // Pomocniczy getter przydatny w testach jednostkowych
    const std::vector<double>& getMeasurements() const { return measurements; }
};