#pragma once
#include <vector>
#include <string>

class Console
{
private:
    std::vector<std::string> logs;

    // Private constructor to prevent instantiation
    Console() {}

public:
    // Destructor
    ~Console() {}

    // Function to add a log to the console
    void AddLog(const std::string& log) {
        logs.push_back(log);
    }

    // Function to clear all logs from the console
    void ClearLogs() {
        logs.clear();
    }

    // Function to get all logs
    const std::vector<std::string>& GetLogs() const {
        return logs;
    }

    // Static function to get the instance of Console
    static Console& getInstance() {
        static Console instance; // Guaranteed to be destroyed, instantiated on first use
        return instance;
    }
};