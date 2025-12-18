#include "stemma.h"

static i2c_master_bus_handle_t s_bus_handle = NULL;
static i2c_master_dev_handle_t s_dev_handle = NULL;

/**
* Reads a register from STEMMA sensor
*
* 1. send command
* ______________________________________________________________________
* | start | slave_addr + wr_bit + ack | base & func address + ack | stop |
* --------|---------------------------|-------------------------  |------|
*
* 2. read data
* ___________________________________________________________________________________
* | start | slave_addr + rd_bit + ack | read data_len byte + ack(last nack)  | stop |
* --------|---------------------------|--------------------------------------|------|
*
* @param[in] dev_handle I2C master bus device handle
* @param[in] low_addr STEMMA base address
* @param[in] high_addr STEMMA function address
* @param[in, out] data data buffer for reading from STEMMA sensor
* @param[in] len size of read buffer
*
* @return
*     - ESP_OK Success
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
*     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
*     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
*/
static esp_err_t stemma_register_read(i2c_master_dev_handle_t dev_handle, uint8_t low_addr, uint8_t high_addr, uint8_t *data, size_t len)
{
    esp_err_t ret;
    uint8_t write_buf[] = {low_addr, high_addr};
    
    ret = i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    if (ret != ESP_OK) return ret;
    
    vTaskDelay(pdMS_TO_TICKS(25));
    
    ret = i2c_master_receive(dev_handle, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    return ret;
}

esp_err_t stemma_init(void)
{
    if (s_bus_handle && s_dev_handle) return ESP_OK;
    
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    esp_err_t err = i2c_new_master_bus(&bus_config, &s_bus_handle);
    if (err != ESP_OK) return err;
    
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = STEMMA_SENSOR_ADDR,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };
    err = i2c_master_bus_add_device(s_bus_handle, &dev_config, &s_dev_handle);
    return err;
}

esp_err_t stemma_read_moisture(uint16_t *moisture)
{
    if (!s_dev_handle) return ESP_ERR_INVALID_STATE;
    
    // read the data from stemma via I2C bus
    uint8_t data[2];
    esp_err_t ret = stemma_register_read(s_dev_handle, STEMMA_MOISTURE_BASE_REG, STEMMA_MOISTURE_FUNC_REG, data, sizeof(data));
    if (ret != ESP_OK) return ret;
    
    *moisture = (data[0] << 8) | data[1];
    return ESP_OK;
}

esp_err_t stemma_deinit(void)
{
    if (s_dev_handle) {
        i2c_master_bus_rm_device(s_dev_handle);
        s_dev_handle = NULL;
    }
    if (s_bus_handle) {
        i2c_del_master_bus(s_bus_handle);
        s_bus_handle = NULL;
    }
    return ESP_OK;
}
