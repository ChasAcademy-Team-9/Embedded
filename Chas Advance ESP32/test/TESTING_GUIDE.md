# Test Usage Guide

Snabbguide för att använda ESP32 test systemet.

## Snabbstart

### Kör alla tester
```bash
cd "Chas Advance ESP32"
platformio test -e native
```

### Förväntat resultat
```
========================================= 22 test cases: 22 succeeded in 00:00:10.017 =========================================
```

## Test Kategorier

| Kategori | Antal | Beskrivning |
|----------|-------|-------------|
| JSON Generation | 3 | Mock JSON generering och validering |
| Sensor Data | 4 | Median beräkning och data processing |
| JSON Parser | 6 | JSON parsing och serialisering |
| Log Functions | 3 | Timestamp och logging funktioner |
| Error Handling | 6 | Felhantering och edge cases |
| **TOTALT** | **22** | **Alla funktioner täckta** |

## Lägg till nya tester

### 1. Skapa test fil
```cpp
// test/test_my_feature.cpp
#include <unity.h>
#include "Arduino.h"
#include "myFeature.h"

void test_my_feature_basic(void) {
    // Arrange
    int input = 5;
    
    // Act
    int result = myFunction(input);
    
    // Assert
    TEST_ASSERT_EQUAL(10, result);
}
```

### 2. Uppdatera test_main.cpp
```cpp
// Lägg till extern deklaration
extern void test_my_feature_basic(void);

// Lägg till i main()
RUN_TEST(test_my_feature_basic);
```

### 3. Kör testerna
```bash
platformio test -e native
```

## Mock Functions

Använd befintliga mock funktioner i dina tester:

```cpp
// JSON mock
String json = generateMockJson();

// Median beräkning
std::vector<float> values = {1.0, 2.0, 3.0};
float result = median(values);

// Timestamp funktioner
uint32_t timestamp = timestampStringToUnix("2024-01-01 12:00:00");
String formatted = formatUnixTime(timestamp);
```

## Debug Tips

### Verbose output
```bash
platformio test -e native -vvv
```

### Serial debugging
```cpp
void test_with_debug(void) {
    Serial.lastPrint = "";  // Rensa innan test
    
    myFunction();  // Function som printar till Serial
    
    TEST_ASSERT_TRUE(Serial.lastPrint.indexOf("Expected output") != -1);
}
```

### Test isolation
```cpp
void setUp(void) {
    // Körs innan varje test
    Serial.lastPrint = "";
    Serial.lastPrintln = "";
}

void tearDown(void) {
    // Körs efter varje test
    // Reset global state om nödvändigt
}
```

## Vanliga Test Patterns

### JSON Validation
```cpp
void test_json_format(void) {
    String json = generateMockJson();
    
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json.c_str());
    
    TEST_ASSERT_FALSE(error);
    TEST_ASSERT_FALSE(doc["Temperature"].isNull());
}
```

### Error Handling
```cpp
void test_error_scenario(void) {
    String invalidInput = "";
    
    parseJson(invalidInput);
    
    TEST_ASSERT_TRUE(Serial.lastPrint.indexOf("error") != -1);
}
```

### Array Processing
```cpp
void test_array_operations(void) {
    std::vector<float> values = {1.0, 2.0, 3.0};
    
    float result = median(values);
    
    TEST_ASSERT_EQUAL_FLOAT(2.0, result);
}
```

## Best Practices

### ✅ Gör
- Testa både success och error scenarios
- Använd beskrivande test namn
- Isolera tester från varandra
- Verifiera mock behavior
- Kommentera komplexa test logik

### ❌ Undvik
- Hardkodade värden utan förklaring
- Beroenden mellan tester
- Ignorera mock function output
- Långa test funktioner
- Testing av implementationsdetaljer

## Felsökning

### Test Failure
1. Kör med verbose flag: `-vvv`
2. Kontrollera mock function behavior
3. Verifiera input/output data
4. Kolla Serial output för error messages

### Compilation Errors
1. Kontrollera header includes
2. Verifiera mock function signatures
3. Se till att `NATIVE_BUILD` används korrekt

### Linking Errors
1. Kontrollera att alla funktioner har mock implementations
2. Verifiera att Serial endast definieras en gång
3. Kolla dependency konfiguration i platformio.ini