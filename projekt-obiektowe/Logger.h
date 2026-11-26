#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>

enum class LogLevel {
    TRACE = 0, DEBUG, INFO, WARN, ERROR
};

class Logger {
private:
    std::ostream* out_stream;
    std::shared_ptr<std::ofstream> file_stream;
    LogLevel minLevel;

    // Prywatny konstruktor (Singleton)
    Logger(std::ostream& stream = std::cout, LogLevel level = LogLevel::INFO)
        : out_stream(&stream), minLevel(level) {
    }

    std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        struct tm timeinfo;
#ifdef _WIN32
        localtime_s(&timeinfo, &now_c);
#else
        localtime_r(&now_c, &timeinfo);
#endif

        char buffer[30];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);

        std::ostringstream oss;
        oss << buffer << "." << std::setfill('0') << std::setw(3) << now_ms.count();
        return oss.str();
    }

    void writeLog(LogLevel level, const std::string& prefix, const std::string& msg) {
        if (static_cast<int>(level) >= static_cast<int>(minLevel)) {
            std::string timeStr = getCurrentTime();
            std::string logLine = timeStr + " [" + prefix + "] " + msg + "\n";

            *out_stream << logLine;

            if (file_stream && file_stream->is_open()) {
                *file_stream << logLine;
                file_stream->flush();
            }
        }
    }

public:
    // Zablokowanie kopiowania i przenoszenia
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    // Jedyny punkt dostêpu do instancji
    static Logger& instance() {
        static Logger inst;
        return inst;
    }

    void setStream(std::ostream& stream) {
        out_stream = &stream;
    }

    void setLogFile(const std::string& filename) {
        if (!filename.empty()) {
            file_stream = std::make_shared<std::ofstream>(filename, std::ios_base::app);
        }
    }

    void setMinLevel(LogLevel level) {
        minLevel = level;
    }

    void trace(const std::string& msg) { writeLog(LogLevel::TRACE, "TRACE", msg); }
    void debug(const std::string& msg) { writeLog(LogLevel::DEBUG, "DEBUG", msg); }
    void info(const std::string& msg) { writeLog(LogLevel::INFO, "INFO", msg); }
    void warn(const std::string& msg) { writeLog(LogLevel::WARN, "WARN", msg); }
    void error(const std::string& msg) { writeLog(LogLevel::ERROR, "ERROR", msg); }
};