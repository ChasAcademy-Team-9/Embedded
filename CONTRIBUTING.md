# Contributing to Chas Advance IoT System

Thank you for your interest in contributing to the Chas Advance IoT System! This document provides guidelines and information for contributors.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Process](#development-process)
- [Coding Standards](#coding-standards)
- [Testing Requirements](#testing-requirements)
- [Pull Request Process](#pull-request-process)
- [Issue Reporting](#issue-reporting)

## Code of Conduct

This project follows a code of conduct to ensure a welcoming environment for all contributors:

- Be respectful and inclusive
- Focus on constructive feedback
- Help others learn and grow
- Maintain professionalism in all interactions

## Getting Started

### Prerequisites

- **PlatformIO Core**: `pip install platformio`
- **VS Code** with PlatformIO extension (recommended)
- **Git** for version control
- **Arduino Uno R4 WiFi** and **ESP32-S3** hardware for testing

### Development Setup

1. **Fork and clone the repository**:
   ```bash
   git clone https://github.com/YOUR_USERNAME/Embedded.git
   cd "Chas Advance"
   ```

2. **Create secrets files**:
   ```bash
   cp "Chas Advance Arduino/include/ARDUINOSECRETS.h.example" "Chas Advance Arduino/include/ARDUINOSECRETS.h"
   cp "Chas Advance ESP32/include/ESPSECRETS.h.example" "Chas Advance ESP32/include/ESPSECRETS.h"
   # Edit both files with your WiFi credentials
   ```

3. **Verify setup by running tests**:
   ```bash
   # Arduino tests (requires hardware)
   cd "Chas Advance Arduino"
   platformio test -e uno_r4_wifi_test
   
   # ESP32 tests (native - no hardware required)
   cd "../Chas Advance ESP32"
   platformio test -e native
   ```

## Development Process

### Branch Naming Convention

- **Feature branches**: `feature/description-of-feature`
- **Bug fixes**: `bugfix/description-of-bug`
- **Hotfixes**: `hotfix/critical-issue`
- **Documentation**: `docs/description-of-changes`

### Workflow

1. Create a feature branch from `main`
2. Make your changes with appropriate tests
3. Run all tests to ensure nothing breaks
4. Update documentation if needed
5. Create a Pull Request with clear description
6. Address review feedback
7. Merge after approval

## Coding Standards

### C++ Guidelines

- **Standard**: Use C++11 features for compatibility
- **Naming**: Use camelCase for variables and functions, PascalCase for classes
- **Comments**: Document complex logic and public interfaces
- **Headers**: Include appropriate header guards
- **Memory**: Avoid memory leaks, use RAII principles

### File Organization

- **Source files**: Place in `src/` directory
- **Headers**: Place in `include/` directory  
- **Tests**: Place in `test/` directory with `test_` prefix
- **Documentation**: Update relevant README files

### Arduino Specific

- **Pin assignments**: Document all pin usage
- **Libraries**: Use stable, well-maintained libraries
- **Memory usage**: Be mindful of limited RAM/Flash
- **Power management**: Consider power efficiency

### ESP32 Specific

- **Task management**: Use FreeRTOS tasks appropriately
- **WiFi handling**: Implement robust connection management
- **JSON processing**: Validate all incoming data
- **Error handling**: Handle network and parsing errors gracefully

## Testing Requirements

### Unit Tests

- **Coverage**: Add tests for all new functionality
- **Arduino tests**: 24 tests covering core functionality
- **ESP32 tests**: 22 native tests for cross-platform compatibility
- **Test isolation**: Each test should be independent

### Test Environments

- **Arduino**: `uno_r4_wifi_test` - runs on hardware
- **ESP32**: `native` - runs without hardware using mocks

### Running Tests

```bash
# Run all Arduino tests
cd "Chas Advance Arduino"
platformio test -e uno_r4_wifi_test

# Run all ESP32 tests  
cd "Chas Advance ESP32"
platformio test -e native

# Run specific test file
platformio test -e native -f test_json_parser
```

### Test Requirements for PRs

- All existing tests must pass
- New functionality must include tests
- Test coverage should not decrease
- Mock functions should simulate real behavior

## Pull Request Process

### Before Creating a PR

1. **Update your branch** with latest `main`:
   ```bash
   git checkout main
   git pull upstream main
   git checkout your-feature-branch
   git rebase main
   ```

2. **Run complete test suite**:
   ```bash
   # Test both projects
   cd "Chas Advance Arduino" && platformio test -e uno_r4_wifi_test
   cd "../Chas Advance ESP32" && platformio test -e native
   ```

3. **Verify builds** work:
   ```bash
   cd "Chas Advance Arduino" && platformio run -e uno_r4_wifi
   cd "../Chas Advance ESP32" && platformio run -e esp32-s3-devkitc-1
   ```

### PR Template

Use the provided [PULL_REQUEST_TEMPLATE.md](PULL_REQUEST_TEMPLATE.md) and include:

- **Clear description** of changes and motivation
- **Testing performed** - which tests were run
- **Hardware tested** - which devices were used
- **Breaking changes** - if any
- **Documentation updates** - if needed

### Review Process

- At least one team member must review
- All tests must pass
- Code must follow style guidelines
- Documentation must be updated if needed
- No merge conflicts

## Issue Reporting

### Bug Reports

Include the following information:

- **Hardware**: Arduino/ESP32 model and version
- **Environment**: PlatformIO version, OS
- **Steps to reproduce**: Minimal example
- **Expected behavior**: What should happen
- **Actual behavior**: What actually happens
- **Serial output**: Include relevant logs

### Feature Requests

- **Use case**: Why is this needed?
- **Proposed solution**: How should it work?
- **Alternatives considered**: Other approaches
- **Implementation notes**: Technical considerations

### Labels

- `bug`: Something isn't working
- `enhancement`: New feature or request
- `documentation`: Improvements or additions to docs
- `good first issue`: Good for newcomers
- `help wanted`: Extra attention is needed

## Project Architecture

### Separation of Concerns

- **Arduino**: Sensor reading, data validation, WiFi client
- **ESP32**: Access Point, HTTP server, data processing
- **Shared**: Common data structures and protocols

### Dependencies

- Keep external dependencies minimal
- Use well-maintained, stable libraries
- Document all dependencies in platformio.ini
- Consider licensing compatibility

### Testing Philosophy

- **Unit tests**: Test individual functions in isolation
- **Integration tests**: Test component interactions
- **Hardware tests**: Validate on actual devices
- **Mock tests**: Enable testing without hardware

## Questions and Support

- **GitHub Issues**: For bugs and feature requests
- **Discussions**: For questions and general discussion
- **Team Contact**: For course-related questions

Thank you for contributing to the Chas Advance IoT System!