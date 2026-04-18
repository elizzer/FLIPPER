#include <stdio.h>

#include "spi_hal.h"
#include "hw_include.h"
#include <string.h>

uint8_t tx_data[2*1024]; // Example command to read JEDEC ID
uint8_t rx_data[2*1024]; // Example command to read JEDEC ID

void hal_spi_test(void){
    SpiHalBusConfig_t bus_config = {
        .spi_instance = SPI_NUM_3   ,
        .mosi_io_num = IO_NUM_23,
        .miso_io_num = IO_NUM_19,
        .sclk_io_num = IO_NUM_18,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 2*1024, // 2 KB
    };

    SpiHalBusHandle_t bus_handle;
    esp_err_t ret = spi_hal_bus_init(&bus_handle, &bus_config);
    if (ret != ESP_OK) {
        printf("Failed to initialize SPI bus\n");
        return;
    }

    SpiHalDeviceConfig_t device_config = {
        .cs_io_num = IO_NUM_5,
        .clk_freq_hz = 1*1000*1000,  // 1 MHz
    };

    SpiHalDeviceHandle_t device_handle;
    ret = spi_hal_add_device(&bus_handle, &device_config, &device_handle);
    if (ret != ESP_OK) {
        printf("Failed to add SPI device\n");
        spi_hal_bus_deinit(&bus_handle);
        return;
    }



    for (size_t i = 0; i < 2*1024; i++)
    {
        /* code */
        tx_data[i] = i % 256;
        rx_data[i] = 0;
    }
    

    SpiHalTransaction_t transaction = {
        .tx_buffer = tx_data,
        .rx_buffer = rx_data,
        .tx_length = sizeof(tx_data) * 8, // Length in bits
        .rx_length = 2*1024 * 8, // Length in bits
    };
    ret = spi_hal_transfer(&device_handle, &transaction);
    if (ret != ESP_OK) {
        printf("SPI transfer failed\n");
    } else {
        printf("SPI transfer succeeded\n");
    }

    //compare data
    
    memcmp(tx_data, rx_data, 2*1024) == 0 ? printf("Data match!\n") : printf("Data mismatch!\n");

    printf("ESP HAL SPI Test\n");
}