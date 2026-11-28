#include "pch.h"
#include <fstream>
#include <sstream>
#include "../projekt-obiektowe/PHSensor.h"
#include "../projekt-obiektowe/NO3Sensor.h"
#include "../projekt-obiektowe/TempSensor.h"
#include "../projekt-obiektowe/Meter.h"
#include "../projekt-obiektowe/Sensor.cpp"

TEST(PHSensorTest, NameAndReadValidAndInvalidData) {
    PHSensor ph(4.0, 9.0);
    EXPECT_EQ(ph.getName(), "PHSensor");
    ph.displayLast();

    std::ofstream out("test_ph.txt");
    out << "2026-11-01 12:00:00;7.2\n";
    out << "2026-11-01 12:01:00;15.0\n";
    out << "bledna_linia\n";
    out.close();

    ph.read_data("test_ph.txt");
    ASSERT_EQ(ph.getMeasurements().size(), 1);
    EXPECT_DOUBLE_EQ(ph.getMeasurements()[0], 7.2);
    ph.displayLast();

    ph.read_data("nie_istnieje.txt", "nie_istnieje_samp.txt");
}

TEST(NO3SensorTest, CalibrationSamplesAndRuleOfFive) {
    std::ofstream sOut("test_samp.txt");
    sOut << "10.0\n20.0\n";
    sOut.close();

    NO3Sensor no3(0.0, 300.0);
    EXPECT_EQ(no3.getName(), "NO3Sensor");
    no3.read_data("nie_istnieje.txt", "test_samp.txt");

    NO3Sensor copyNo3(no3);
    NO3Sensor assignedNo3;
    assignedNo3 = copyNo3;

    NO3Sensor movedNo3(std::move(copyNo3));
    NO3Sensor moveAssignedNo3;
    moveAssignedNo3 = std::move(assignedNo3);
}

TEST(TempSensorTest, SetTemperaturaValidAndInvalid) {
    TempSensor temp("Salon", 0.0, 40.0);
    EXPECT_EQ(temp.getName(), "TempSensor (Salon)");

    temp.setTemperatura(22.5);
    EXPECT_DOUBLE_EQ(temp.temperatura, 22.5);

    temp.setTemperatura(100.0);
    EXPECT_DOUBLE_EQ(temp.temperatura, 22.5);
}

TEST(SensorStreamTest, OutputOperatorPrintsName) {
    PHSensor ph(4.0, 9.0);
    std::ostringstream oss;
    oss << ph;
    EXPECT_EQ(oss.str(), "PHSensor");
}

TEST(MeterTest, OperatorsAddFindRemoveAndStream) {
    Meter m;
    m += std::make_unique<PHSensor>(4.0, 9.0);
    m += std::make_unique<TempSensor>("T1", 0.0, 40.0);
    m += std::make_unique<NO3Sensor>(0.0, 300.0);

    EXPECT_EQ(m.getSensorCount(), 3);

    // Test operator[]
    Sensor* foundPh = m["PHSensor"];
    ASSERT_NE(foundPh, nullptr);
    EXPECT_EQ(foundPh->getName(), "PHSensor");
    EXPECT_EQ(m["Nieistniejacy"], nullptr);

    // Test operator<<
    std::ostringstream oss;
    oss << m;
    EXPECT_EQ(oss.str(), "PHSensor\nTempSensor (T1)\nNO3Sensor\n");

    // Test operator-=
    m -= "TempSensor (T1)";
    EXPECT_EQ(m.getSensorCount(), 2);
    EXPECT_EQ(m["TempSensor (T1)"], nullptr);
}