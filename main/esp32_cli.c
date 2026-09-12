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



// create and array to hold command history
char command_history[10][128];
uint8_t command_history_index = 0;

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



void app_main(void)
{
    // UART0 already initialized by IDF, just install driver if needed
    uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);

    LOG_INFO("ESP32 CLI started");
    cmd_print_banner(NULL, NULL);
    char input[128] = {0};
    app_init(); // initialize app commands and state
    int8_t status;
    while (1)
    {
        printf("\r\n> ");
        fflush(stdout);
        readline(input, sizeof(input));

        main_cmd_dispatch(input);

        // clear input buffers
        memset(input, 0, sizeof(input));
    }
}
