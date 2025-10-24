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

### Run all tests (recommended)
```bash
platformio test -e uno_r4_wifi_test
```

This command will:
1. Use the dedicated test environment (avoids production config conflicts)
2. Compile the test files
3. Upload tests to the connected Arduino board
4. Execute the tests on the hardware
5. Report results back to the console

### Legacy command (uses default environment)
```bash
platformio test
```

### Run specific test file
```bash
platformio test -e uno_r4_wifi_test -f test_thresholds
platformio test -e uno_r4_wifi_test -f test_sensor_data
platformio test -e uno_r4_wifi_test -f test_batch_handler
platformio test -e uno_r4_wifi_test -f test_mock_sensor
platformio test -e uno_r4_wifi_test -f test_flash_functionality
```

### Run tests with verbose output
```bash
platformio test -e uno_r4_wifi_test -v
```

### Check production build (separate from tests)
```bash
platformio run -e uno_r4_wifi
```

## Expected Output

When all tests pass successfully, you should see output similar to:

```
Testing...
If you don't see any output for the first 10 secs, please reset board (press reset button)

test\test_main.cpp:67: test_median_odd_number_of_readings       [PASSED]
test\test_main.cpp:68: test_median_even_number_of_readings      [PASSED]
test\test_main.cpp:69: test_median_excludes_sensor_failures     [PASSED]
test\test_main.cpp:70: test_median_single_reading       [PASSED]
test\test_main.cpp:71: test_median_all_sensor_failures  [PASSED]
test\test_main.cpp:72: test_median_unsorted_data        [PASSED]
test\test_main.cpp:75: test_mock_data_valid_ranges      [PASSED]
test\test_main.cpp:76: test_mock_data_error_flag_consistency    [PASSED]
test\test_main.cpp:77: test_mock_data_modifies_outputs  [PASSED]
test\test_main.cpp:80: test_valid_sensor_data   [PASSED]
test\test_main.cpp:81: test_temperature_too_low [PASSED]
test\test_main.cpp:82: test_temperature_too_high        [PASSED]
test\test_main.cpp:83: test_humidity_too_low    [PASSED]
test\test_main.cpp:84: test_humidity_too_high   [PASSED]
test\test_main.cpp:85: test_temperature_at_minimum      [PASSED]
test\test_main.cpp:86: test_temperature_at_maximum      [PASSED]
test\test_main.cpp:89: test_room_temp_thresholds        [PASSED]
test\test_main.cpp:90: test_cooler_thresholds   [PASSED]
test\test_main.cpp:91: test_freezer_thresholds  [PASSED]
test\test_main.cpp:94: test_get_flash_data_as_batch_empty       [PASSED]
test\test_main.cpp:95: test_get_flash_data_as_batch_with_data   [PASSED]
test\test_main.cpp:96: test_send_flash_data_when_empty  [PASSED]
test\test_main.cpp:97: test_flash_data_parsing_malformed_entries        [PASSED]
test\test_main.cpp:98: test_flash_data_preserves_sensor_id      [PASSED]
--------------------------------------------- uno_r4_wifi_test:* [PASSED] Took 39.77 seconds --------------------------------------------- 

================================================================= SUMMARY ================================================================= 
Environment       Test    Status    Duration
----------------  ------  --------  ------------
uno_r4_wifi_test  *       PASSED    00:00:39.770
=============================================== 24 test cases: 24 succeeded in 00:00:39.770 ===============================================
```

## Test Summary

Total: **24 unit tests** covering 5 Arduino function groups:

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

5. **test_flash_functionality.cpp** (5 tests)
   - Tests for `getFlashDataAsBatch()` and `sendFlashDataIfAvailable()`
   - Validates flash data storage and retrieval
   - Tests WiFi reconnection flash data sending

## Troubleshooting

### Tests don't start
- Ensure Arduino board is properly connected
- Press the reset button on the Arduino when prompted
- Check USB connection and drivers

### Compilation errors
- Run `platformio lib install` to ensure all dependencies are installed
- Run `platformio run -e uno_r4_wifi` to verify the main project builds correctly
- Make sure you're using the correct test environment: `-e uno_r4_wifi_test`

### Environment Configuration
- **uno_r4_wifi**: Production environment (clean build)
- **uno_r4_wifi_test**: Test environment (includes test dependencies)

### Platform installation issues
- The first time you run tests, PlatformIO may need to install the platform
- This requires an internet connection and may take several minutes
- If installation fails, try: `platformio platform install renesas-ra`

## Environment Architecture

This project uses **separate environments** for production and testing:

- **Production (uno_r4_wifi)**: Clean environment for deployment
  - Includes main.cpp for normal operation
  - No test dependencies or configuration
  
- **Testing (uno_r4_wifi_test)**: Dedicated test environment
  - Excludes main.cpp to avoid conflicts with test runner
  - Includes Unity test framework and test-specific build flags
  - Uses global logger instance to prevent conflicts

## Notes

- Tests run directly on the Arduino hardware (uno_r4_wifi board)
- Unity test framework is included with PlatformIO, no separate installation needed
- Tests are located in the `test/` directory
- Each test file is self-contained and can be run independently
- Flash data tests clean up after themselves to ensure test isolation
- Global logger instance prevents conflicts between test modules
