#include "Sensor.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <utility>

Sensor::~Sensor() = default;

bool Sensor::isValid(double value) const {
    return (value >= minValue && value <= maxValue);
}

void Sensor::pushMeasurement(double val) {
    if (isValid(val)) {
        measurements.push_back(val);
    }
    else {
        Logger::instance().warn(getName() + ": Pominieto wartosc poza zakresem (" + std::to_string(val) + ")");
    }
}

Sensor::Sensor(double minV, double maxV)
    : samples(nullptr), sampleCount(0), minValue(minV), maxValue(maxV) {
}

Sensor::Sensor(const Sensor& other)
    : measurements(other.measurements), sampleCount(other.sampleCount), minValue(other.minValue), maxValue(other.maxValue) {
    if (sampleCount > 0 && other.samples != nullptr) {
        samples = std::make_unique<double[]>(sampleCount);
        std::copy(other.samples.get(), other.samples.get() + sampleCount, samples.get());
    }
}

Sensor::Sensor(Sensor&& other) noexcept
    : measurements(std::move(other.measurements)),
    samples(std::move(other.samples)),
    sampleCount(other.sampleCount),
    minValue(other.minValue),
    maxValue(other.maxValue) {
    other.sampleCount = 0;
}

Sensor& Sensor::operator=(const Sensor& other) {
    if (this != &other) {
        measurements = other.measurements;
        sampleCount = other.sampleCount;
        minValue = other.minValue;
        maxValue = other.maxValue;
        if (sampleCount > 0 && other.samples != nullptr) {
            samples = std::make_unique<double[]>(sampleCount);
            std::copy(other.samples.get(), other.samples.get() + sampleCount, samples.get());
        }
        else {
            samples.reset();
        }
    }
    return *this;
}

Sensor& Sensor::operator=(Sensor&& other) noexcept {
    if (this != &other) {
        measurements = std::move(other.measurements);
        samples = std::move(other.samples);
        sampleCount = other.sampleCount;
        minValue = other.minValue;
        maxValue = other.maxValue;

        other.sampleCount = 0;
    }
    return *this;
}

void Sensor::read_data(const std::string& meas_file, const std::string& samp_file) {
    std::ifstream f_meas(meas_file);
    if (f_meas.is_open()) {
        std::string line;
        while (std::getline(f_meas, line)) {
            if (line.empty()) continue;

            size_t pos = line.find(';');
            std::string valStr = (pos != std::string::npos) ? line.substr(pos + 1) : line;

            try {
                double val = std::stod(valStr);
                pushMeasurement(val);
            }
            catch (...) {
            }
        }
        Logger::instance().info(getName() + ": Poprawnie wczytano pomiary z " + meas_file);
    }
    else {
        Logger::instance().error(getName() + ": Nie udalo sie otworzyc pliku " + meas_file);
    }

    if (!samp_file.empty()) {
        std::ifstream f_samp(samp_file);
        std::vector<double> tempSamples;
        if (f_samp.is_open()) {
            double val;
            while (f_samp >> val) tempSamples.push_back(val);
            Logger::instance().info(getName() + ": Poprawnie wczytano probki kalibracyjne.");

            sampleCount = static_cast<int>(tempSamples.size());
            if (sampleCount > 0) {
                samples = std::make_unique<double[]>(sampleCount);
                std::copy(tempSamples.begin(), tempSamples.end(), samples.get());
            }
            else {
                samples.reset();
            }
        }
        else {
            Logger::instance().error(getName() + ": Nie udalo sie otworzyc pliku " + samp_file);
        }
    }
}

void Sensor::displayLast() const {
    if (!measurements.empty()) {
        std::cout << "Ostatni pomiar " << getName() << ": " << measurements.back() << "\n";
    }
    else {
        std::cout << "Brak pomiarow " << getName() << ".\n";
    }
}