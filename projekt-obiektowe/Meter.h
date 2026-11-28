#pragma once
#include <vector>
#include <memory>
#include <string>
#include <ostream>
#include <algorithm>
#include "Sensor.h"

class Meter {
private:
    std::vector<std::unique_ptr<Sensor>> sensors;

public:
    Meter() = default;

    // Dodawanie sensora do kolekcji
    Meter& operator+=(std::unique_ptr<Sensor> sensor) {
        if (sensor) {
            sensors.push_back(std::move(sensor));
        }
        return *this;
    }

    // Usuwanie sensora po nazwie
    Meter& operator-=(const std::string& name) {
        sensors.erase(
            std::remove_if(sensors.begin(), sensors.end(),
                [&name](const std::unique_ptr<Sensor>& s) {
                    return s && s->getName() == name;
                }),
            sensors.end()
        );
        return *this;
    }

    // Wyszukiwanie sensora po nazwie (zwraca nullptr, jeœli nie znaleziono)
    Sensor* operator[](const std::string& name) const {
        for (const auto& s : sensors) {
            if (s && s->getName() == name) {
                return s.get();
            }
        }
        return nullptr;
    }

    // Wypisywanie wszystkich sensorów (ka¿dy w nowej linii)
    friend std::ostream& operator<<(std::ostream& os, const Meter& meter) {
        for (const auto& s : meter.sensors) {
            if (s) {
                os << *s << "\n";
            }
        }
        return os;
    }

    size_t getSensorCount() const {
        return sensors.size();
    }
};