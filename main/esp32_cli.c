#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/uart.h"
#define LOG_TAG "cli"
#include "cli_log.h"
#include "cmd_parser.h"
#include "cli_app.h"
#include "esp_system.h"
#include "esp_err.h"
#include "esp_chip_info.h"
#include "esp_timer.h"
#include "driver/temperature_sensor.h"

// create and array to hold command history
char command_history[10][128];
uint8_t command_history_index = 0;

int readline_scanf(const char *fmt, ...)
{
    char buffer[128];
    char ch;
    uint8_t uch;
    memset(buffer, 0, sizeof(buffer));

    while (1)
    {
        int len = uart_read_bytes(UART_NUM_0, &uch, 1, pdMS_TO_TICKS(50));
        if (len <= 0)
        {
            vTaskDelay(pdMS_TO_TICKS(10)); // yield while idle
            continue;
        }

        ch = (char)uch;

        if (ch == '\n' || ch == '\r')
        {
            break;
        }
        else if ((ch == '\b' || ch == 127) && strlen(buffer) > 0)
        {
            // 127 = DEL, sent by some terminals for backspace
            buffer[strlen(buffer) - 1] = '\0';
            printf("\b \b");
            fflush(stdout);
        }
        // handle command history
        else if (ch == '[')
        {
            // read the next two characters to determine the arrow key
            char seq[2];
            uart_read_bytes(UART_NUM_0, (uint8_t *)seq, 2, pdMS_TO_TICKS(50));
            if (seq[0] == 'A') // Up arrow
            {
                // handle up arrow (previous command)
                printf("\r\n[Up Arrow Pressed]\r\n");
            }
            else if (seq[0] == 'B') // Down arrow
            {
                // handle down arrow (next command)
                printf("\r\n[Down Arrow Pressed]\r\n");
            }
        }
        else if (ch >= 0x20 && ch < 0x7F)
        {
            // printable ASCII only
            size_t len = strlen(buffer);
            if (len < sizeof(buffer) - 1)
            {
                buffer[len] = ch;
                printf("%c", ch);
                fflush(stdout);
            }
        }
    }

    printf("\r\n");

    va_list args;
    va_start(args, fmt);
    int ret = vsscanf(buffer, fmt, args);
    va_end(args);
    return ret; // return match count like sscanf does
}

int readline(char *buf, size_t max_len)
{
    char ch;
    uint8_t uch;
    size_t idx = 0;
    uint8_t currr_hist_index = command_history_index;

    while (idx < max_len - 1) // leave space for null terminator
    {
        int len = uart_read_bytes(UART_NUM_0, &uch, 1, pdMS_TO_TICKS(50));
        if (len <= 0)
        {
            vTaskDelay(pdMS_TO_TICKS(10)); // yield while idle
            continue;
        }

        ch = (char)uch;

        if (ch == '\n' || ch == '\r')
        {
            break;
        }
        else if ((ch == '\b' || ch == 127) && idx > 0)
        {
            // 127 = DEL, sent by some terminals for backspace
            idx--;
            buf[idx] = '\0';
            printf("\b \b");
            fflush(stdout);
        }
        else if (ch >= 0x20 && ch < 0x7F)
        {
            // printable ASCII only
            buf[idx++] = ch;
            printf("%c", ch);
            fflush(stdout);
        }
        // handle command history
        else if (ch == 0x1B)
        {
            // read the next two characters to determine the arrow key
            char seq[2];
            uart_read_bytes(UART_NUM_0, (uint8_t *)seq, 2, pdMS_TO_TICKS(50));
            if (seq[0] == '[') // Up arrow
            {
                if (seq[1] == 'A') // Up arrow
                {
                    // handle up arrow (previous command)
                    printf("\r\n[Up Arrow Pressed]\r\n");
                }
                else if (seq[1] == 'B') // Down arrow
                {
                    // handle down arrow (next command)
                    printf("\r\n[Down Arrow Pressed]\r\n");
                }
            }
        }
        // detect tab
        else if (ch == '\t')
        {
            printf("\r\n[Tab Pressed]\r\n");
        }
    }

    buf[idx] = '\0'; // null terminate
    printf("\r\n");
    // add command to history
    if (idx > 0)
    {
        strncpy(command_history[command_history_index], buf, strlen(buf) + 1);
        command_history_index = (command_history_index + 1) % 10; // wrap around
    }
    return idx; // return length of input
}

// crate a basic set of cmd function pairs
// greeting banner
void cmd_hello(void* handle,char *args)
{
    printf("Hello, %s!\r\n", args);
}

void cmd_print_banner(void* handle,char *args)
{
    (void)args;
    printf("\r\n");
    printf("  ██╗  ██╗ █████╗ ██████╗ ██╗   ██╗██╗   ██╗██╗    ██╗  ██╗\r\n");
    printf("  ██║ ██╔╝██╔══██╗██╔══██╗██║   ██║██║   ██║██║    ╚██╗██╔╝\r\n");
    printf("  █████╔╝ ███████║██████╔╝██║   ██║██║   ██║██║     ╚███╔╝ \r\n");
    printf("  ██╔═██╗ ██╔══██║██╔══██╗██║   ██║╚██╗ ██╔╝██║     ██╔██╗ \r\n");
    printf("  ██║  ██╗██║  ██║██║  ██║╚██████╔╝ ╚████╔╝ ██║    ██╔╝ ██╗\r\n");
    printf("  ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝   ╚═══╝  ╚═╝    ╚═╝  ╚═╝\r\n");
    printf("\r\n");
    printf("  Open Source Engineer's Toolkit  |  ESP32-S3\r\n");
    printf("  v0.1.0  |  https://github.com/elizzer/Karuvi_X.git\r\n");
    printf("\r\n");
}

void cmd_time(void* handle,char *args)
{
    int64_t us = esp_timer_get_time();
    int64_t seconds = us / 1000000;
    int64_t micros = us % 1000000;
    printf("Uptime: %lld.%06lld seconds\r\n", (long long)seconds, (long long)micros);
}

void cmd_sysinfo(void* handle,char *args)
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    printf("Chip model: %s\r\n", chip_info.model == CHIP_ESP32S3 ? "ESP32-S3" : "Unknown");
    printf("Cores: %d\r\n", chip_info.cores);
    printf("Revision: %d\r\n", chip_info.revision);
    printf("Features bitmap: 0x%08" PRIx32 "\r\n", chip_info.features);
    printf("Free heap: %" PRIu32 " bytes\r\n", esp_get_free_heap_size());
    printf("Minimum free heap: %" PRIu32 " bytes\r\n", esp_get_minimum_free_heap_size());
}

void cmd_temp(void* handle,char *args)
{
    (void)args;

    temperature_sensor_config_t temp_cfg = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 100);
    temperature_sensor_handle_t temp_handle = NULL;
    esp_err_t err = temperature_sensor_install(&temp_cfg, &temp_handle);
    if (err != ESP_OK)
    {
        printf("Temperature sensor install failed: %s\r\n", esp_err_to_name(err));
        return;
    }

    err = temperature_sensor_enable(temp_handle);
    if (err != ESP_OK)
    {
        printf("Temperature sensor enable failed: %s\r\n", esp_err_to_name(err));
        temperature_sensor_uninstall(temp_handle);
        return;
    }

    float temperature = 0.0f;
    err = temperature_sensor_get_celsius(temp_handle, &temperature);
    if (err == ESP_OK)
    {
        const char *note = temperature > 70.0f ? "(running hot, consider a fan)" : "";
        printf("Chip temperature: %.2f°C %s\r\n", temperature, note);
    }
    else
    {
        printf("Temperature read failed: %s\r\n", esp_err_to_name(err));
    }

    temperature_sensor_disable(temp_handle);
    temperature_sensor_uninstall(temp_handle);
}
void cmd_panic(void* handle,char *args)
{
    (void)args;
    printf("Breaking me on purpose...\r\n");
    abort();
}

void cmd_all(void* handle,char *args)
{
    (void)args;
    cmd_time("",NULL);
    cmd_sysinfo("",NULL);
}

void cmd_reboot(void* handle,char *args)
{
    (void)args;
    esp_restart();
}

void cmd_console_clear(void* handle,char *args)
{
    // \033[2J clears the entire screen, \033[H moves cursor to home (0,0)
    printf("\033[2J\033[H");

}

cmdEntry_t g_cmd_table[] = {
    {"hello", cmd_hello, ""},
    {"print_banner", cmd_print_banner, ""},
    {"time", cmd_time, ""},
    {"sysinfo", cmd_sysinfo, ""},
    {"temp", cmd_temp, ""},
    {"panic", cmd_panic, ""},
    {"all", cmd_all, ""},
    {"help", cmd_help, ""},
    {"create", cmd_create, ""},
    {"use", cmd_use, ""},
    {"reboot", cmd_reboot, ""},
    {"cls", cmd_console_clear, ""},
    {"clear", cmd_console_clear, ""},
    {"", NULL, ""},
};

void app_main(void)
{
    // UART0 already initialized by IDF, just install driver if needed
    uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);

    LOG_INFO("ESP32 CLI started");
    cmd_print_banner(NULL,NULL);
    char input[128] = {0};
    app_init(); // initialize app commands and state
    int8_t status;
    while (1)
    {
        printf("\r\n> ");
        fflush(stdout);
        readline(input, sizeof(input));

        status = cmd_dispatch(input, g_cmd_table, sizeof(g_cmd_table) / sizeof(cmdEntry_t));
        if(status == -1){
            LOG_ERR("Unknown command :%s",input);
        }

        // clear input buffers
        memset(input, 0, sizeof(input));
    }
}
