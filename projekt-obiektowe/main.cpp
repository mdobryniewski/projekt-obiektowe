#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <regex>
#include <map>

#include "Logger.h"
#include "Properties.h"
#include "PHSensor.h"
#include "NO3Sensor.h"
#include "TempSensor.h"
#include "Meter.h"

#ifdef _WIN32
#include "getopt.h"
#else
#include <unistd.h>
#include <getopt.h>
#endif

using namespace std;

void extractAndGroupPhoneNumbers() {
    std::string text = "W celu potwierdzenia rezerwacji prosimy o kontakt z dzialem obslugi pod numerem +48 225551234. Jesli Twoja sprawa dotyczy platnosci, mozesz zadzwonic takze pod +48 225551235, albo skontaktowac sie z naszym oddzialem w Krakowie - numer +48 126789321. W Czechach dziala biuro regionalne dostepne pod numerem +42 312345678, a dzial techniczny ma osobna linie: +42 312349876. W Niemczech infolinia dziala calodobowo - +49 301112223 lub +49 309998877. Jesli jestes z Wegier, wybierz numer +36 178945612. Dodatkowo, w sprawach serwisowych dostepny jest dyzurny technik pod +36 178945699. W przypadku zgloszen miedzynarodowych prosimy dzwonic na centrale europejska +48 225550000.";

    std::map<std::string, std::vector<std::string>> groupedNumbers;
    std::regex phoneRegex(R"((\+\d{2})\s(\d+))");

    auto words_begin = std::sregex_iterator(text.begin(), text.end(), phoneRegex);
    auto words_end = std::sregex_iterator();

    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        std::string prefix = match[1].str();
        std::string number = match[2].str();
        groupedNumbers[prefix].push_back(number);
    }

    std::cout << "\n--- Zgrupowane numery telefonow (Regex) ---\n";
    for (const auto& pair : groupedNumbers) {
        std::cout << "Kierunkowy: " << pair.first << "\n";
        for (const auto& num : pair.second) {
            std::cout << "  - " << num << "\n";
        }
    }
}

std::pair<double, double> parseRangeRegex(const std::string& rangeStr, double defMin, double defMax) {
    std::regex rangeRegex(R"((-?\d+\.?\d*)\s*-\s*(-?\d+\.?\d*))");
    std::smatch match;

    if (std::regex_search(rangeStr, match, rangeRegex) && match.size() > 2) {
        try {
            return { std::stod(match[1].str()), std::stod(match[2].str()) };
        }
        catch (...) {
        }
    }
    return { defMin, defMax };
}

void printHelp() {
    std::cout << "Sensors v0.1 by Pawel Prokop\n";
    std::cout << "Usage: ./sensor OPTS\n";
    std::cout << "Options:\n";
    std::cout << "  -c FILE                     Configuration file\n";
    std::cout << "  -h                          This help message\n";
    std::cout << "  -l --log-level LOG_LEVEL    Logging level\n";
}

LogLevel parseLogLevel(const std::string& levelStr) {
    if (levelStr == "TRACE") return LogLevel::TRACE;
    if (levelStr == "DEBUG") return LogLevel::DEBUG;
    if (levelStr == "INFO") return LogLevel::INFO;
    if (levelStr == "WARN") return LogLevel::WARN;
    if (levelStr == "ERROR") return LogLevel::ERROR;
    return LogLevel::INFO;
}

int main(int argc, char* argv[]) {
    std::string configFile = "sensor.conf";
    bool logOverride = false;
    LogLevel cmdLogLevel = LogLevel::INFO;

    struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"loglevel", required_argument, 0, 'l'},
        {"log-level", required_argument, 0, 'l'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "c:hl:", long_options, &option_index)) != -1) {
        switch (opt) {
        case 'c': configFile = optarg; break;
        case 'h': printHelp(); return 0;
        case 'l':
            cmdLogLevel = parseLogLevel(optarg);
            logOverride = true;
            break;
        default: printHelp(); return 1;
        }
    }

    Properties props;
    props.load(configFile);

    LogLevel finalLogLevel = logOverride ? cmdLogLevel : parseLogLevel(props.get("min_log_level", "INFO"));

    auto phRange = parseRangeRegex(props.get("ph_range", "4.0-9.0"), 4.0, 9.0);
    double phMin = phRange.first;
    double phMax = phRange.second;

    auto no3Range = parseRangeRegex(props.get("no3_range", "0.0-300.0"), 0.0, 300.0);
    double no3Min = no3Range.first;
    double no3Max = no3Range.second;

    auto tempRange = parseRangeRegex(props.get("temp_range", "0.0-40.0"), 0.0, 40.0);
    double tempMin = tempRange.first;
    double tempMax = tempRange.second;

    std::string logFileName = props.get("log_file", "");

    // Konfiguracja Singletona Logger
    Logger::instance().setMinLevel(finalLogLevel);
    if (!logFileName.empty()) {
        Logger::instance().setLogFile(logFileName);
    }

    Logger::instance().info("Sensors start");

    extractAndGroupPhoneNumbers();

    std::cout << "\n--- Odczyt danych PHSensor ---\n";
    PHSensor ph(phMin, phMax);
    ph.read_data("ph_data.txt");
    ph.displayLast();

    std::cout << "\n--- Odczyt danych NO3Sensor ---\n";
    NO3Sensor no3(no3Min, no3Max);
    no3.read_data("no3_data.txt");
    no3.displayLast();

    std::cout << "\n--- Odczyt danych TempSensor ---\n";
    ifstream plik("temp_data.txt");
    vector<TempSensor> sensory;

    if (!plik.is_open()) {
        Logger::instance().error("Blad otwarcia pliku temp_data.txt!");
    }
    else {
        string czas, tempStr;
        while (getline(plik, czas, ';')) {
            getline(plik, tempStr);
            if (tempStr.empty()) continue;

            try {
                TempSensor temp(czas, tempMin, tempMax);
                temp.setTemperatura(stod(tempStr));
                sensory.push_back(std::move(temp));
            }
            catch (...) {
            }
        }
        plik.close();

        cout << "Wczytano " << sensory.size() << " sensorow" << endl;
        cout << "Lp.   | Nazwa sensora             | Temperatura |" << endl;
        cout << "------+---------------------------+-------------+" << endl;
        for (size_t i = 0; i < sensory.size(); i++) {
            cout << left << setw(5) << (i + 1) << " | "
                << setw(25) << sensory[i].nazwa << " | "
                << right << setw(11) << fixed << setprecision(1) << sensory[i].temperatura << " |" << endl;
        }
    }

    std::cout << "\n--- Inicjalizacja Meter ---\n";
    TempSensor sampleTemp = sensory.empty() ? TempSensor("Brak", tempMin, tempMax) : sensory[0];

    Meter mainMeter(std::move(ph), sampleTemp, std::move(no3));
    Logger::instance().info("Miernik zlozony (Kompozycja zakonczona sukcesem).");

    return 0;
}