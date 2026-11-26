#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>

class Properties {
private:
    std::unordered_map<std::string, std::string> data;

public:
    void load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Nie udalo sie otworzyc pliku properties: " << filename << "\n";
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            auto delimiterPos = line.find('=');
            if (delimiterPos != std::string::npos) {
                std::string key = line.substr(0, delimiterPos);
                std::string value = line.substr(delimiterPos + 1);
                data[key] = value;
            }
        }
    }

    std::string get(const std::string& name) {
        auto it = data.find(name);
        if (it != data.end()) return it->second;
        return "";
    }

    std::string get(const std::string& key, const std::string& def) {
        auto it = data.find(key);
        if (it != data.end()) return it->second;
        return def;
    }
};
