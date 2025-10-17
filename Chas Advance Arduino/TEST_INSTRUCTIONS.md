# How to Run Arduino Unit Tests

## Prerequisites

1. Install PlatformIO Core:
   ```bash
   pip install platformio
   ```

2. Make sure you are in the Arduino project directory:
   ```bash
   cd "Chas Advance Arduino"
   ```

## Running Tests

### Run all tests
```bash
pio test
```

This command will:
1. Compile the test files
2. Upload tests to the connected Arduino board (if available)
3. Execute the tests on the hardware
4. Report results back to the console

### Run specific test file
```bash
pio test -f test_thresholds
pio test -f test_sensor_data
pio test -f test_batch_handler
pio test -f test_mock_sensor
```

### Run tests with verbose output
```bash
pio test -v
```

### Run tests for specific environment
```bash
pio test -e uno_r4_wifi
```

## Expected Output

When all tests pass successfully, you should see output similar to:

```
Testing...
If you don't see any output for the first 10 secs, please reset board (press reset button)

test/test_thresholds.cpp:23:test_room_temp_thresholds    [PASSED]
test/test_thresholds.cpp:34:test_cooler_thresholds       [PASSED]
test/test_thresholds.cpp:45:test_freezer_thresholds      [PASSED]
-----------------------
3 Tests 0 Failures 0 Ignored
OK

test/test_sensor_data.cpp:21:test_valid_sensor_data           [PASSED]
test/test_sensor_data.cpp:34:test_temperature_too_low         [PASSED]
test/test_sensor_data.cpp:47:test_temperature_too_high        [PASSED]
test/test_sensor_data.cpp:60:test_humidity_too_low            [PASSED]
test/test_sensor_data.cpp:73:test_humidity_too_high           [PASSED]
test/test_sensor_data.cpp:86:test_temperature_at_minimum      [PASSED]
test/test_sensor_data.cpp:97:test_temperature_at_maximum      [PASSED]
-----------------------
7 Tests 0 Failures 0 Ignored
OK

test/test_batch_handler.cpp:23:test_median_odd_number_of_readings       [PASSED]
test/test_batch_handler.cpp:37:test_median_even_number_of_readings      [PASSED]
test/test_batch_handler.cpp:52:test_median_excludes_sensor_failures     [PASSED]
test/test_batch_handler.cpp:68:test_median_single_reading               [PASSED]
test/test_batch_handler.cpp:79:test_median_all_sensor_failures          [PASSED]
test/test_batch_handler.cpp:95:test_median_unsorted_data                [PASSED]
-----------------------
6 Tests 0 Failures 0 Ignored
OK

test/test_mock_sensor.cpp:23:test_mock_data_valid_ranges              [PASSED]
test/test_mock_sensor.cpp:48:test_mock_data_error_flag_consistency    [PASSED]
test/test_mock_sensor.cpp:67:test_mock_data_modifies_outputs          [PASSED]
-----------------------
3 Tests 0 Failures 0 Ignored
OK
```

## Test Summary

Total: **19 unit tests** covering 4 Arduino functions:

1. **test_thresholds.cpp** (3 tests)
   - Tests for `getThresholdsForMode()` function
   - Validates threshold values for ROOM_TEMP, COOLER, and FREEZER modes

2. **test_sensor_data.cpp** (7 tests)
   - Tests for `checkThresholds()` function
   - Validates sensor data against configured thresholds
   - Tests edge cases and boundary conditions

3. **test_batch_handler.cpp** (6 tests)
   - Tests for `calculateMedian()` function
   - Validates median calculation with various scenarios
   - Tests handling of sensor failures

4. **test_mock_sensor.cpp** (3 tests)
   - Tests for `generateMockData()` function
   - Validates mock data generation for testing purposes

## Troubleshooting

### Tests don't start
- Ensure Arduino board is properly connected
- Press the reset button on the Arduino when prompted
- Check USB connection and drivers

### Compilation errors
- Run `pio lib install` to ensure all dependencies are installed
- Run `pio run` to verify the main project builds correctly

### Platform installation issues
- The first time you run tests, PlatformIO may need to install the platform
- This requires an internet connection and may take several minutes
- If installation fails, try: `pio platform install renesas-ra`

## Notes

- Tests run directly on the Arduino hardware (uno_r4_wifi board)
- Unity test framework is included with PlatformIO, no separate installation needed
- Tests are located in the `test/` directory
- Each test file is self-contained and can be run independently
