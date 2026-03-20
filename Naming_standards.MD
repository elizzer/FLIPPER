# Naming Standards

---

## Files

| Type            | Convention           | Example                  |
|:----------------|:---------------------|:-------------------------|
| Source / Header | `module_name.c/.h`   | `event_manager.c`        |
| Driver          | `drv_type.c/.h`      | `drv_uart.h`             |
| Board support   | `bsp_boardname.c/.h` | `bsp_discovery.h`        |
| Config          | `board_config.c/.h`  | `board_stm32f4_config.c` |

---

## Types

| Type             | Convention        | Example              |
|:-----------------|:------------------|:---------------------|
| Struct           | `PascalCase_t`    | `DriverDescriptor_t` |
| Enum             | `PascalCase_t`    | `EventType_t`        |
| Typedef          | `PascalCase_t`    | `ResourceConfig_t`   |
| Function pointer | `PascalCase_fn`   | `DriverInit_fn`      |
| Enum members     | `PREFIX_ALL_CAPS` | `EVENT_TYPE_KEYPAD`  |

---

## Variables

| Type                | Convention     | Example          |
|:--------------------|:---------------|:-----------------|
| Local               | `snake_case`   | `queue_handle`   |
| Static module level | `s_snake_case` | `s_registry`     |
| Global (avoid)      | `g_snake_case` | `g_system_state` |
| Constant            | `k_snake_case` | `k_board_config` |

---

## Functions

| Type             | Convention          | Example                 |
|:-----------------|:--------------------|:------------------------|
| Public API       | `module_verb()`     | `event_init()`          |
| Private / static | `verb_noun()`       | `lookup_driver()`       |
| Task             | `noun_task()`       | `comms_task()`          |
| ISR              | `MODULE_IRQHandler` | `UART1_IRQHandler`      |
| Callback         | `module_on_event()` | `uart_on_rx_complete()` |

---

## Macros

| Type            | Convention         | Example                 |
|:----------------|:-------------------|:------------------------|
| General         | `ALL_CAPS_SNAKE`   | `MAX_RESOURCE_COUNT`    |
| Module specific | `MODULE_FLAG_NAME` | `UART_FLAG_DMA_ENABLED` |
| Header guard    | `MODULE_NAME_H`    | `EVENT_MANAGER_H`       |

---

## FreeRTOS Handles

| Type        | Convention         | Example             |
|:------------|:-------------------|:--------------------|
| Task        | `noun_task_handle` | `comms_task_handle` |
| Queue       | `noun_queue`       | `sensor_queue`      |
| Mutex       | `noun_mutex`       | `i2c_mutex`         |
| Semaphore   | `noun_sem`         | `data_ready_sem`    |
| Event group | `noun_events`      | `system_events`     |
| Timer       | `noun_timer`       | `watchdog_timer`    |

---

## Golden Rules

- No abbreviations unless universal — `uart`, `spi`, `gpio`, `dma` are acceptable; `rm`, `em` are not
- Module prefix always comes first — `event_init()` not `init_event()`
- `_t` suffix for types only — never on variables or functions
- Verbs before nouns in all function names
