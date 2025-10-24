# Arduino Testing Guide

This project includes separate environments for production and testing to ensure clean configuration management.

## Environments

### Production Environment: `uno_r4_wifi`
- Used for normal Arduino deployment
- Clean configuration without test dependencies
- Includes main.cpp for regular operation

```bash
# Build for production
platformio run -e uno_r4_wifi

# Upload to production
platformio run -e uno_r4_wifi --target upload
```

### Test Environment: `uno_r4_wifi_test`
- Dedicated environment for unit testing
- Excludes main.cpp to avoid conflicts with test runner
- Includes test-specific build flags and configuration
- Uses Unity testing framework

```bash
# Run all tests
platformio test -e uno_r4_wifi_test

# Run specific test
platformio test -e uno_r4_wifi_test --filter test_median_*
```

## Test Structure

- **test/test_main.cpp**: Main test runner with setup()
- **test/test_*.cpp**: Individual test modules
- **Global Logger**: Single logger instance shared across all tests

## Current Test Coverage

- ✅ Batch handler functionality (6 tests)
- ✅ Mock sensor data generation (3 tests) 
- ✅ Sensor data validation (7 tests)
- ✅ Temperature threshold checking (3 tests)
- ✅ Flash data functionality (5 tests)

**Total: 24 tests, all passing**

## Best Practices

1. **Use dedicated test environment** for testing to avoid mixing configurations
2. **Share global logger instance** to prevent conflicts between test modules
3. **Clean up flash memory** after each test to ensure isolation
4. **Test on actual hardware** for realistic validation