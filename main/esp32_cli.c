#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/uart.h"
#define LOG_TAG "cli"
#include "cli_log.h"
#include "cmd_parser.h"
#include "cli_app.h"
#include "esp_system.h"

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
    }

    buf[idx] = '\0'; // null terminate
    printf("\r\n");
    return idx; // return length of input
}

// crate a basic set of cmd function pairs
// greeting banner
void cmd_hello(char *args)
{
    printf("Hello, %s!\r\n", args);
}

void cmd_print_banner(char *args)
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
void cmd_reboot()
{

    esp_restart();
}

cmdEntry_t g_cmd_table[] = {
    {"hello", cmd_hello,""},
    {"print_banner", cmd_print_banner,""},
    {"help", cmd_help,""},
    {"create", cmd_create,""},
    {"use", cmd_use,""},
    {"reboot", cmd_reboot,""},
    {"", NULL,""},
};

void app_main(void)
{
    // UART0 already initialized by IDF, just install driver if needed
    uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);

    LOG_INFO("ESP32 CLI started");
    cmd_print_banner();
    char input[128] = {0};
    app_init(); // initialize app commands and state
    while (1)
    {
        printf("\r\n> ");
        fflush(stdout);
        readline(input, sizeof(input));

        cmd_dispatch(input, g_cmd_table, sizeof(g_cmd_table) / sizeof(cmdEntry_t));

        // clear input buffers
        memset(input, 0, sizeof(input));
    }
}
