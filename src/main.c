#include <stdio.h>
#include "nvs_flash.h"

#include "stemma.h"
#include "wifi_mqtt.h"

static const char *TAG = "mqtt soil sensor";

void app_main(void)
{
    ESP_LOGI(TAG, "starting...");
    
    // initializes NVS for storing Wifi settings; if there's an issue, erases & reinitializes
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    ESP_LOGI(TAG, "starting stemma sensor");
    ESP_ERROR_CHECK(stemma_init());
    
    ESP_LOGI(TAG, "starting wifi station");
    ESP_ERROR_CHECK(wifi_init_station());
    
    ESP_LOGI(TAG, "starting MQTT client");
    ESP_ERROR_CHECK(mqtt_app_start());;
    
    while (1) {
        uint16_t moisture_data;
        esp_err_t r = stemma_read_moisture(&moisture_data);
        if (r == ESP_OK) {
            ESP_LOGI(TAG, "Moisture Level = %d", moisture_data);
            char moisture_str[16];
            snprintf(moisture_str, sizeof(moisture_str), "%d", moisture_data);
            if (mqtt_is_connected()) {
                int msg_id = mqtt_publish("/topic/soil-moisture", moisture_str, 0, 0);
                if (msg_id > -1) {
                    ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
                } else {
                    ESP_LOGI(TAG, "sent publish unsuccessful; client unhealthy");
                }
            }
        } else {
            ESP_LOGE(TAG, "stemma sensor read failed: %d", r);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}