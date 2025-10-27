/**
 * @file Arduino.h
 * @brief Minimal Arduino mock for native unit testing
 *
 * This mock provides only the essential types and functions needed
 * for our ESP32 source code to compile in native environment.
 * Header-only implementation for testing simplicity.
 */

#pragma once

#include <string>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cstddef>

#ifdef NATIVE_BUILD
#include <vector>
#include <algorithm>
#include <ArduinoJson.h>
#endif

// Basic Arduino types
typedef uint8_t byte;
typedef bool boolean;

// Forward declarations
struct SensorData;

// Arduino String class mock using std::string
class String
{
private:
    std::string data;

public:
    // Constructors
    String() : data("") {}
    String(const char *str) : data(str ? str : "") {}
    String(const std::string &str) : data(str) {}
    String(int value)
    {
        char buffer[32];
        sprintf(buffer, "%d", value);
        data = buffer;
    }
    String(float value)
    {
        char buffer[32];
        sprintf(buffer, "%.2f", value);
        data = buffer;
    }
    String(double value)
    {
        char buffer[32];
        sprintf(buffer, "%.2f", value);
        data = buffer;
    }
    String(float value, int digits)
    {
        char format[16];
        sprintf(format, "%%.%df", digits);
        char buffer[32];
        sprintf(buffer, format, value);
        data = buffer;
    }

    // Essential operations
    const char *c_str() const { return data.c_str(); }
    size_t length() const { return data.length(); }
    bool isEmpty() const { return data.empty(); }

    // String search methods
    int indexOf(const String &str) const
    {
        size_t pos = data.find(str.data);
        return (pos != std::string::npos) ? (int)pos : -1;
    }
    int indexOf(const char *str) const
    {
        size_t pos = data.find(str);
        return (pos != std::string::npos) ? (int)pos : -1;
    }

    // For ArduinoJson compatibility (Stream interface)
    int read()
    {
        static size_t index = 0;
        if (index >= data.length())
        {
            index = 0;
            return -1;
        }
        return data[index++];
    }

    // Operators
    String &operator=(const String &other)
    {
        data = other.data;
        return *this;
    }
    String &operator=(const char *str)
    {
        data = (str ? str : "");
        return *this;
    }
    String operator+(const String &other) const
    {
        return String(data + other.data);
    }
    String &operator+=(const String &other)
    {
        data += other.data;
        return *this;
    }
    bool operator==(const String &other) const
    {
        return data == other.data;
    }
    bool operator!=(const String &other) const
    {
        return data != other.data;
    }

    // Conversion operators
    operator const char *() const { return c_str(); }
    operator std::string() const { return data; }
};

// Mock Serial for compilation and testing
struct SerialMock
{
    String lastPrint;
    String lastPrintln;

    void begin(int baud) {}
    void print(const String &str) { lastPrint = str; }
    void println(const String &str) { lastPrintln = str; }
    void print(const char *str) { lastPrint = String(str); }
    void println(const char *str) { lastPrintln = String(str); }
};

// Global Serial instance - defined in test_main.cpp to avoid multiple definitions
extern SerialMock Serial;

// Basic Arduino functions
inline int random(int min, int max)
{
    static bool seeded = false;
    if (!seeded)
    {
        srand(time(nullptr));
        seeded = true;
    }
    return min + rand() % (max - min);
}

inline void delay(int milliseconds)
{
    // Mock: do nothing in tests
}

#ifdef NATIVE_BUILD
// Mock function implementations for testing ESP32 code in native environment

inline String generateMockJson()
{
    return String("{\"timestamp\":\"2025-01-01 12:00:00\",\"temperature\":25.0,\"humidity\":50.0,\"error\":false}");
}

inline float median(std::vector<float> &values)
{
    if (values.empty())
        return 0.0;

    // Remove error values (-99.0, -1.0)
    std::vector<float> filtered;
    for (float val : values)
    {
        if (val != -99.0 && val != -1.0)
        {
            filtered.push_back(val);
        }
    }

    if (filtered.empty())
        return 0.0;

    std::sort(filtered.begin(), filtered.end());
    size_t size = filtered.size();

    if (size % 2 == 0)
    {
        return (filtered[size / 2 - 1] + filtered[size / 2]) / 2.0;
    }
    else
    {
        return filtered[size / 2];
    }
}

inline String serializeBatchToJson(const std::vector<SensorData> &batch)
{
    if (batch.empty())
    {
        return String("[]");
    }

    // For testing, return a simple JSON array with one sensor
    return String("[{\"SensorId\":1,\"Temperature\":22.5,\"Humidity\":45.0,\"Error\":false}]");
}

inline uint32_t timestampStringToUnix(const String &tsStr)
{
    // Simple mock: return a fixed timestamp for testing
    return 1704110400; // 2024-01-01 12:00:00 UTC
}

inline String formatUnixTime(uint32_t ts)
{
    // Simple mock: return a formatted string
    return String("2024-01-01 12:00:00");
}

inline void logSensorData(String timestamp, float temperature, float humidity, int errorType)
{
    // Mock: do nothing, just for compilation
}

inline void logEvent(String timestamp, String eventType, String description, String status)
{
    // Mock: do nothing, just for compilation
}

inline void parseJson(String json)
{
    // Mock implementation that tests for JSON parsing errors
    if (json.isEmpty() || json.indexOf("{") == -1 || json.indexOf("}") == -1 ||
        json.indexOf("\"Temperature\":\"not_a_number\"") != -1 ||
        json.indexOf("\"Humidity\":}") != -1 ||
        json.indexOf("{invalid json") != -1)
    {
        Serial.print("JSON parse error: ");
        Serial.println("Invalid JSON format");
        return;
    }

    // For testing, we simulate successful parsing
    // In real implementation, this would parse and call logSensorData
}

inline void parseJsonArray(JsonArray &arr, const String &timestamp)
{
    // Mock implementation for array parsing
    for (JsonVariant obj : arr)
    {
        // Simulate processing each object
        String mockJsonStr = String("{\"timestamp\":\"") + timestamp + String("\",\"Temperature\":25.0,\"Humidity\":50.0}");
        parseJson(mockJsonStr);
    }
}

#endif