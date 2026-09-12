# KaruviX ESP32 CLI

KaruviX is an ESP32-S3 firmware project that exposes a UART-driven command-line interface for creating and controlling hardware interface instances. The project is built around a registry-based module system so new interfaces can be added without changing the core CLI flow.

## What this project does

The firmware starts a serial console and accepts commands over UART. The CLI supports a small set of core commands as well as interface-specific commands for hardware modules.

Current interface modules include:

- GPIO
- PWM
- I2C

The interface model is intentionally extensible: each module registers itself in the central interface registry, and instances are created with names at runtime using the create / use pattern.

## Current status

This repository is an active prototype and development platform. The core command loop, registry, and several hardware interfaces are implemented and usable, but the project is still evolving and not yet a complete production-grade hardware toolkit.

Implemented areas:

- UART command loop and command dispatch
- Core CLI commands such as help, sysinfo, time, reboot, and cls
- Registry-based interface management
- GPIO interface with pin, direction, pull, read/write/toggle support
- PWM interface registration and basic command handling
- I2C interface registration and basic transaction helpers
- ESP-IDF build integration

## Hardware and software requirements

- Target: ESP32-S3
- Framework: ESP-IDF
- Toolchain: ESP-IDF installed and configured in your shell environment
- Serial terminal for UART interaction (for example, minicom, PuTTY, or the ESP-IDF monitor)

## Build and flash

From the project root:

```bash
idf.py set-target esp32s3
idf.py build
```

Then flash and monitor the device:

```bash
idf.py -p <YOUR_PORT> flash monitor
```

If you only want to flash without opening the monitor:

```bash
idf.py -p <YOUR_PORT> flash
```

## Command model

The CLI uses a simple runtime pattern:

```text
create <interface> <name>
use <name> <command> [args]
```

Examples:

```text
help
help gpio
create gpio led
use led set_pin IO_5
use led set_dir output
use led get_config
use led set
```

The main commands are registered in the application layer:

- print_banner
- time
- sysinfo
- panic
- all
- help
- create
- use
- reboot
- cls / clear

## Supported interface examples

### GPIO

```text
create gpio io1
use io1 set_pin IO_5
use io1 set_dir output
use io1 set_pull pull_up
use io1 set
use io1 read
use io1 toggle
use io1 get_config
```

GPIO commands include:

- set_pin <IO_x>
- get_config
- set_dir <input|output>
- set_pull <no_pull|pull_up|pull_down>
- set
- clear
- read
- toggle

### PWM

```text
create pwm pwm0
use pwm0 set_pin 5
use pwm0 set_freq_hz 1000
use pwm0 set_duty_cycle 50
use pwm0 start
```

PWM commands include:

- set_pin <IO_x>
- set_freq_hz <freq_hz>
- set_freq_Mhz <freq_Mhz>
- get_freq_hz
- set_duty_cycle <0-100>
- get_duty_cycle
- start
- stop

### I2C

```text
create i2c bus1
use bus1 alloc_instance 0
use bus1 set_sda 17
use bus1 set_scl 18
use bus1 set_speed standard
use bus1 set_mode master
use bus1 scan
```

I2C commands include:

- alloc_instance [instance]
- set_sda <IO_x>
- set_scl <IO_x>
- set_addr <addr>
- set_speed <standard|fast|fast_plus|high>
- set_mode <master|slave>
- read <slave_addr> <length>
- write <addr> <byte0> [byte1 ...]
- scan
- probe <slave_addr>

## Repository layout

- [main](main) - Firmware entry point and UART loop
- [cli_app](cli_app) - Core CLI application commands and registration logic
- [cli_gpio](cli_gpio) - GPIO interface implementation and command table
- [cli_pwm](cli_pwm) - PWM interface implementation and command table
- [cli_i2c](cli_i2c) - I2C interface implementation and command table
- [cmd_parser](cmd_parser) - Minimal command parsing and dispatch helpers
- [interface_registry](interface_registry) - Central registry for interface lookup and registration
- [kx_hal](kx_hal) - HAL-level support layer for hardware abstractions
- [test](test) - Test scripts and command samples
- [Release](Release) - Packaging and release helper scripts

## Testing and automation

The project includes a simple serial automation script for sending commands from a file:

```bash
python .\send_cmd.py --port COM12 --baud 115200 --interval 1 --file test_cmds.txt
```

The file in [test](test) is intended as a quick smoke-test for the UART CLI flow.

## Contributing

Contributions are welcome. Please read [CONTRIBUTING.md](CONTRIBUTING.md) before opening issues or pull requests.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
