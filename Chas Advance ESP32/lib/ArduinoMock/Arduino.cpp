/**
 * @file Arduino.cpp
 * @brief Implementation of Arduino mock
 */

#include "Arduino.h"

// Actual definition of the global Serial instance.
// This matches the 'extern SerialMock Serial;' declaration in test_main.cpp (line 11).
SerialMock Serial;