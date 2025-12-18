#ifndef STEMMA_H
#define STEMMA_H

#include "esp_err.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>

#define STEMMA_SENSOR_ADDR          0x36     /*!< slave address for stemma sensor */
#define STEMMA_MOISTURE_BASE_REG    0x0F     /*!< base register for stemma soil moisture sensor */
#define STEMMA_MOISTURE_FUNC_REG    0x10     /*!< function register for stemma soil moisture sensor */
#define I2C_MASTER_SCL_IO           22       /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           21       /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_FREQ_HZ          100000   /*!< I2C master clock frequency */
#define I2C_MASTER_TIMEOUT_MS       1000     /*!< I2C Master Timeout */

/**
* Initializes and configures ESP32 to be master & stemma sensor to be slave on I2C bus
* 
* @return ESP error code
*/
esp_err_t stemma_init(void);

/**
* Takes soil moisture measurement from stemma
* 
* @param[in,out] moisture int pointer to hold soil moisture measurement
* @return ESP error code
*/
esp_err_t stemma_read_moisture(uint16_t *moisture);

/**
* Deletes I2C bus
* 
* @return ESP error code
*/
esp_err_t stemma_deinit(void);

#endif // STEMMA_H
