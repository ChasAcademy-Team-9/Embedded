# Arduino Unit Tests

## Översikt

Denna mapp innehåller enhetstester för Arduino-funktionerna i projektet. Testerna använder PlatformIO:s inbyggda Unity-ramverk för att säkerställa att varje del av koden fungerar som avsett.

## Testfiler

### test_thresholds.cpp
Testar `getThresholdsForMode()`-funktionen som returnerar olika temperatur- och fuktighetströsklar baserat på driftsläge (ROOM_TEMP, COOLER, FREEZER).

**Tester:**
- `test_room_temp_thresholds()` - Verifierar tröskelvärden för rumstemperatur (18-25°C, 30-70% fuktighet)
- `test_cooler_thresholds()` - Verifierar tröskelvärden för kylskåp (2-6°C, 30-70% fuktighet)
- `test_freezer_thresholds()` - Verifierar tröskelvärden för frys (-30 till -18°C, 30-70% fuktighet)

### test_sensor_data.cpp
Testar `checkThresholds()`-funktionen som validerar sensoravläsningar mot konfigurerade tröskelvärden och sätter felmarkeringar när trösklar överskrids.

**Tester:**
- `test_valid_sensor_data()` - Verifierar att giltiga värden passerar validering
- `test_temperature_too_low()` - Verifierar detektering av för låg temperatur
- `test_temperature_too_high()` - Verifierar detektering av för hög temperatur
- `test_humidity_too_low()` - Verifierar detektering av för låg fuktighet
- `test_humidity_too_high()` - Verifierar detektering av för hög fuktighet
- `test_temperature_at_minimum()` - Verifierar gränsvärden vid exakt minimum
- `test_temperature_at_maximum()` - Verifierar gränsvärden vid exakt maximum

### test_batch_handler.cpp
Testar `calculateMedian()`-funktionen som beräknar median från en vektor av sensoravläsningar och exkluderar avläsningar med SENSOR_FAIL-fel.

**Tester:**
- `test_median_odd_number_of_readings()` - Beräknar median med udda antal avläsningar
- `test_median_even_number_of_readings()` - Beräknar median med jämnt antal avläsningar
- `test_median_excludes_sensor_failures()` - Verifierar att SENSOR_FAIL-avläsningar exkluderas
- `test_median_single_reading()` - Hanterar en enskild avläsning
- `test_median_all_sensor_failures()` - Hanterar när alla avläsningar har SENSOR_FAIL
- `test_median_unsorted_data()` - Verifierar korrekt median oavsett inmatningsordning

### test_mock_sensor.cpp
Testar `generateMockData()`-funktionen som genererar simulerade sensoravläsningar för testning.

**Tester:**
- `test_mock_data_valid_ranges()` - Verifierar att giltiga värden är inom förväntade intervall (22-28°C, 40-60%)
- `test_mock_data_error_flag_consistency()` - Verifierar att felflaggan sätts korrekt
- `test_mock_data_modifies_outputs()` - Säkerställer att funktionen faktiskt skriver till utdatavariabler

## Hur man kör testerna

### Köra alla tester
```bash
pio test
```

### Köra specifika tester
```bash
pio test -f test_thresholds
pio test -f test_sensor_data
pio test -f test_batch_handler
pio test -f test_mock_sensor
```

### Köra tester med verbose output
```bash
pio test -v
```

### Köra tester för specifik miljö
```bash
pio test -e uno_r4_wifi
```

## Förväntade resultat

När testerna körs framgångsrikt ska du se output liknande:

```
test/test_thresholds.cpp:XX:test_room_temp_thresholds    [PASSED]
test/test_thresholds.cpp:XX:test_cooler_thresholds       [PASSED]
test/test_thresholds.cpp:XX:test_freezer_thresholds      [PASSED]
-----------------------
3 Tests 0 Failures 0 Ignored
OK
```

## Kravacceptans / Definition of Done

✅ Testerna kan köras via `pio test` utan fel  
✅ Testerna returnerar "PASS" i konsolen  
✅ Kommentarer i testfilerna beskriver syftet med testet  
✅ Ingen befintlig kod har påverkats negativt  

## Testramverk

Testerna använder **Unity** som är PlatformIOs standard testramverk för embedded-system. Unity tillhandahåller assertions som:

- `TEST_ASSERT_EQUAL(expected, actual)` - Jämför två värden
- `TEST_ASSERT_EQUAL_FLOAT(expected, actual)` - Jämför flyttalsvärden
- `TEST_ASSERT_TRUE(condition)` - Verifierar att villkor är sant
- `TEST_ASSERT_FALSE(condition)` - Verifierar att villkor är falskt
- `TEST_ASSERT_NULL(pointer)` - Verifierar att pekare är NULL

## Felsökning

Om tester misslyckas:

1. Kontrollera att alla beroenden är installerade: `pio lib install`
2. Bygg projektet först: `pio run`
3. Kör tester med verbose output: `pio test -v`
4. Kontrollera att hårdvaran är korrekt ansluten (om tester körs på fysisk enhet)

## Bidra med nya tester

När du lägger till nya funktioner:

1. Skapa en ny testfil i `/test/` mappen med prefix `test_`
2. Inkludera nödvändiga headers (`Arduino.h`, `unity.h`, och dina egna headers)
3. Skriv testfunktioner med prefix `test_`
4. Dokumentera vad varje test gör med kommentarer
5. Lägg till tester i `setup()`-funktionen med `RUN_TEST()`
6. Kör `pio test` för att verifiera att testerna fungerar

## Mer information

- [PlatformIO Unit Testing](https://docs.platformio.org/en/latest/advanced/unit-testing/index.html)
- [Unity Test Framework](http://www.throwtheswitch.org/unity)
