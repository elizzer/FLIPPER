# KaruviX

*Tool for eXploration*

A lightweight ESP32-S3 firmware project that exposes a UART-driven
command-line interface for creating and controlling hardware interface
instances. The current prototype focuses on GPIO support and is designed
to grow into a modular, extensible toolkit for embedded experimentation
and hardware debugging. This repository is intended to be the first step
of a larger project that will be built upon once the CLI application
reaches a useful and stable stage.

## Why this exists

Most hardware debugging and prototyping tools — logic analyzers, protocol
interfaces, GPIO/bus testers, power profilers — are either expensive,
closed-source, or require installing vendor-specific desktop software.
That's a real barrier for anyone learning or working with embedded
systems on a budget, especially without access to a full bench of lab
equipment.

This project explores a different approach: put the toolkit on a cheap,
widely available microcontroller (ESP32-S3) and expose it entirely over
a serial terminal. No GUI, no drivers, no OS-specific app — just a UART
connection and any terminal emulator.

A screen and onboard GUI are a natural extension of this idea, letting
the toolkit run standalone without a host PC. That isn't a functional
requirement right now, but the architecture is modular by design, so
a display/GUI layer can be added later without reworking the core.

The bigger goal is a general-purpose, extensible hardware toolkit rather
than a single-purpose tool. The same registry-based interface model that
handles GPIO today is meant to support custom modules for things like:

- Power profiling / current monitoring
- Wi-Fi debugging and Wi-Fi stub/simulation tooling
- IR transmit/receive (e.g. as a universal remote)
- Data acquisition
- Additional protocol interfaces (I2C, SPI, UART, and beyond)

Each of these can be built as a self-contained interface module and
registered into the same CLI, without touching the core firmware. This
repository is the foundation that makes that extensibility possible —
released early and open because the core architecture is useful on its
own, and because collaboration from this stage will shape how well it
generalizes to those future modules.


## Current status

**Working:**
- UART-based command loop in the firmware entry point
- Command parsing and dispatch
- A registry-based interface model
- A GPIO interface implementation for basic pin configuration

**Evolving:**
- More complete command coverage
- Better validation and error handling
- More hardware-facing implementations
- Documentation and examples for real-world use

## Hardware and software requirements

- Target hardware: ESP32-S3
- Framework: ESP-IDF
- Build toolchain: ESP-IDF toolchain installed and configured

## Quick start

1. Install ESP-IDF and make sure the environment is configured.
2. Open the project directory.
3. Build the firmware:

```bash
idf.py set-target esp32s3
idf.py build
```

4. Flash it to your board:

```bash
idf.py -p <YOUR_PORT> flash monitor
```

## Example commands

Once the firmware is running, you can try commands like:

```text
hello world
create gpio io1
use io1 set_pin IO_5
use io1 set_dir output
use io1 get_config
```

## Project structure

- [main](main) - Firmware entry point and UART loop
- [cli_app](cli_app) - Application-level CLI commands
- [cli_gpio](cli_gpio) - GPIO interface implementation
- [cmd_parser](cmd_parser) - Simple command parsing logic
- [interface_registry](interface_registry) - Interface registration and lookup
- [DESIGN_DOCUMENT.md](DESIGN_DOCUMENT.md) - Architecture notes and design context

## Roadmap

Potential next steps for the project include:
- Additional interfaces such as I2C, SPI, and UART
- Better validation for command arguments
- Improved help text and examples
- Stability improvements and cleanup for public use
- A richer demo workflow for hardware testing

## Contributing

Contributions are welcome. Please read [CONTRIBUTING.md](CONTRIBUTING.md) before opening issues or pull requests.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
