#include "kx_i2c_hal.h"

#include "soc/i2c_reg.h"
#include "soc/i2c_struct.h"
#include "soc/gpio_struct.h"
#include "hal/i2c_ll.h"
#include "hal/gpio_ll.h"
#include "soc/gpio_sig_map.h"
#include "esp_private/periph_ctrl.h"
#include "esp_rom_gpio.h"

#include <stdint.h>

#define FI2C_SCLK 40000000U

// globale variable to hold the instance
static KxI2C_ChannelConfig_t g_i2c_instances[KX_I2C_INSTANCE_MAX] = {0};

typedef struct
{
    uint8_t sda_in;
    uint8_t scl_in;
    uint8_t sda_out;
    uint8_t scl_out;

} KxI2C_Signal;

typedef union
{
    struct
    {
        uint32_t byte_num : 8;
        uint32_t ack_en : 1;
        uint32_t ack_exp : 1;
        uint32_t ack_val : 1;
        uint32_t op_code : 3;
        uint32_t reserved14 : 17;
        uint32_t done : 1;
    };

    uint32_t val;

} kx_i2c_hal_command_reg_t;

static KxI2C_Signal i2c_signal_map[KX_I2C_INSTANCE_MAX] = {
    {
        .scl_in = I2CEXT0_SCL_IN_IDX,
        .scl_out = I2CEXT0_SCL_OUT_IDX,
        .sda_in = I2CEXT0_SDA_IN_IDX,
        .sda_out = I2CEXT0_SDA_OUT_IDX,
    },
    {
        .scl_in = I2CEXT1_SCL_IN_IDX,
        .scl_out = I2CEXT1_SCL_OUT_IDX,
        .sda_in = I2CEXT1_SDA_IN_IDX,
        .sda_out = I2CEXT1_SDA_OUT_IDX,
    },
};

Kx_ErrorCode kx_i2c_init()
{
    // init the clock for the i2c peripheral
    // init values for the global state array
    for (uint8_t t_idx = 0; t_idx < KX_I2C_INSTANCE_MAX; t_idx++)
    {
        g_i2c_instances[t_idx].is_initialized = false;
        g_i2c_instances[t_idx].is_used = false;
        g_i2c_instances[t_idx].instance = t_idx;
    }
    return KX_HAL_OK;
}
Kx_ErrorCode kx_i2c_deinit()
{
    return KX_HAL_OK;
}

Kx_ErrorCode i2c_alloc_instance(KxI2C_Handle_t *handle)
{

    // validate the handle not null

    // loop and get an free instance
    uint8_t free_idx = KX_I2C_INSTANCE_MAX;
    for (uint8_t s_idx = 0; s_idx < KX_I2C_INSTANCE_MAX; s_idx++)
    {
        if (g_i2c_instances[s_idx].is_used == false)
        {
            free_idx = s_idx;
            break;
        }
    }

    if (free_idx == KX_I2C_INSTANCE_MAX)
    {
        return KX_HAL_ERR_FAIL;
    }

    i2c_port_t t_port = (i2c_port_t)g_i2c_instances[free_idx].instance;

    PERIPH_RCC_ATOMIC()
    {
        i2c_ll_enable_bus_clock(t_port, true);
        i2c_ll_reset_register(t_port);
    }

    i2c_dev_t *t_dev = I2C_LL_GET_HW(t_port);
    i2c_ll_set_source_clk(t_dev, I2C_CLK_SRC_XTAL);
    i2c_ll_enable_controller_clock(t_dev, true);

    g_i2c_instances[free_idx].is_used = true;
    g_i2c_instances[free_idx].is_initialized = true;

    return KX_HAL_OK;
}

Kx_ErrorCode KxI2C_free_instance(KxI2C_Handle_t handle)
{
    return KX_HAL_OK;
}

Kx_ErrorCode KxI2C_set_device_mode(KxI2C_Handle_t handle, KxI2C_Mode_t mode)
{
    if (KX_I2C_MODE_MASTER == mode)
    {
        i2c_ll_master_init(I2C_LL_GET_HW(handle->instance));
    }
    else if (KX_I2C_MODE_SLAVE == mode)
    {
        i2c_ll_slave_init(I2C_LL_GET_HW(handle->instance));
    }

    i2c_ll_update(I2C_LL_GET_HW(handle->instance));

    return KX_HAL_OK;
}

Kx_ErrorCode KxI2C_set_speed(KxI2C_Handle_t handle, KxI2C_Speed_t speed)
{

    i2c_dev_t *t_dev = I2C_LL_GET_HW(handle->instance);

    i2c_hal_clk_config_t clk_cfg = {0};
    // timeout is not enabled for now
    if (KX_I2C_SPEED_STANDARD == speed)
    {

        uint32_t sclk = FI2C_SCLK;
        uint32_t bus_freq = 100000;
        uint32_t clkm_div = (sclk / (bus_freq * 1024)) + 1;
        uint32_t half_cycle = (sclk / bus_freq) / 2;
        if (KX_I2C_MODE_MASTER == handle->device_mode)
        {

            clk_cfg.clkm_div = clkm_div;
            clk_cfg.scl_low = half_cycle;
            clk_cfg.scl_wait_high = half_cycle / 4;
            clk_cfg.scl_high = half_cycle * (3 / 4);
            clk_cfg.setup = half_cycle;
            clk_cfg.hold = half_cycle;
            clk_cfg.sda_hold = half_cycle / 4;
            clk_cfg.sda_sample = half_cycle / 2;
            clk_cfg.tout = 10;
        }
        else if (KX_I2C_MODE_SLAVE == handle->device_mode)
        {
            clk_cfg.sda_hold = half_cycle / 4;
            clk_cfg.sda_sample = half_cycle / 2;
        }
        else
        {
            return KX_HAL_ERR_FAIL;
        }
    }
    i2c_ll_master_set_bus_timing(t_dev, &clk_cfg);
    return KX_HAL_OK;
}

Kx_ErrorCode KxI2C_set_sda(KxI2C_Handle_t handle, Kx_IO sda)
{

    if (handle == NULL)
    {
        return KX_HAL_ERR_FAIL;
    }

    gpio_dev_t *gpio = GPIO_LL_GET_HW(0);
    // set the pin to gpio
    gpio_ll_func_sel(gpio, sda, PIN_FUNC_GPIO);
    gpio_ll_output_enable(gpio, sda);
    gpio_ll_od_enable(gpio, sda);

    gpio_ll_input_enable(gpio, sda);
    gpio_ll_pullup_en(gpio, sda);
    gpio_ll_pulldown_dis(gpio, sda); // make sure pulldown isn't also on

    // map the i2c SDA in and out signals
    esp_rom_gpio_connect_in_signal(sda, i2c_signal_map[handle->instance].sda_in, false);
    esp_rom_gpio_connect_out_signal(sda, i2c_signal_map[handle->instance].sda_out, false, false);
    return KX_HAL_OK;
}

Kx_ErrorCode KxI2C_set_scl(KxI2C_Handle_t handle, Kx_IO scl)
{
    if (handle == NULL)
    {
        return KX_HAL_ERR_FAIL;
    }

    gpio_dev_t *gpio = GPIO_LL_GET_HW(0);
    // set the pin to gpio
    gpio_ll_func_sel(gpio, scl, PIN_FUNC_GPIO);
    gpio_ll_output_enable(gpio, scl);
    gpio_ll_od_enable(gpio, scl);

    gpio_ll_input_enable(gpio, scl);
    gpio_ll_pullup_en(gpio, scl);
    gpio_ll_pulldown_dis(gpio, scl); // make sure pulldown isn't also on

    // map the i2c scl in and out signals
    esp_rom_gpio_connect_in_signal(scl, i2c_signal_map[handle->instance].scl_in, false);
    esp_rom_gpio_connect_out_signal(scl, i2c_signal_map[handle->instance].scl_out, false, false);
    return KX_HAL_OK;
}

Kx_ErrorCode KxI2C_set_slave_addr(KxI2C_Handle_t handle, KxI2C_AddrMode_t addr_mode, uint16_t addr)
{
    return KX_HAL_OK;
}

Kx_ErrorCode KxI2C_probe(KxI2C_Handle_t handle, uint16_t addr)
{
    return KX_HAL_OK;
}

Kx_ErrorCode KxI2C_master_read(KxI2C_Handle_t handle, uint16_t s_addr, uint8_t *data, size_t length)
{
    return KX_HAL_OK;
}
Kx_ErrorCode KxI2C_master_write(KxI2C_Handle_t handle, uint16_t s_addr, uint8_t *data, size_t length)
{
    // put the data into the fifo and start the transfer
    i2c_dev_t *t_dev = I2C_LL_GET_HW(handle->instance);

    // cleare the fifo before writing
    t_dev->fifo_conf.tx_fifo_rst = 1;
    // for every write the data is written from the start
    memcpy(t_dev->txfifo_mem, data, length);

    // now fill the command registers with the write commands
    kx_i2c_hal_command_reg_t cmd ;

    cmd.val = 0;
    cmd.op_code = 6; // 
    t_dev->comd[0].val = cmd.val;
    
    //write
    cmd.val = 0;
    cmd.op_code = 1; 
    cmd.byte_num = length > 32 ? 32 : length;
    t_dev->comd[1].val = cmd.val;
    
    //stop
    cmd.val = 0;
    cmd.op_code = 2;
    t_dev->comd[2].val = cmd.val;
    
    i2c_ll_master_trans_start(t_dev);
    
    return KX_HAL_OK;
}
