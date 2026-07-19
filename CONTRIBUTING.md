# Contributing

Thanks for your interest in contributing to this project.

## How to contribute

1. Fork the repository.
2. Create a feature branch for your change.
3. Make your changes and keep them focused.
4. Update documentation when behavior or setup changes.
5. Open a pull request with a clear summary.

## Adding a new interface module

New interfaces (I2C, SPI, power profiling, IR, etc.) should follow the
same pattern as the existing GPIO interface — see `cli_gpio` for
reference and `interface_registry` for how modules are registered.
Keep each interface self-contained in its own directory.

## Suggested areas to help with

- New interface implementations such as I2C, SPI, or UART
- Command validation and error handling
- Documentation and example usage
- Bug fixes and stability improvements
- Hardware testing on different ESP32 boards

## Before submitting

- Keep changes minimal and easy to review
- Follow the existing C style in the project
- Mention any hardware limitations or assumptions
- Include screenshots or logs when relevant

## Reporting issues

Please use GitHub Issues to report bugs or request features. Include:
- What you expected to happen
- What actually happened
- Your hardware setup and ESP-IDF version
- Relevant logs or command output
