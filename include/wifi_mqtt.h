#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include "esp_err.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "mqtt_client.h"
#include "freertos/event_groups.h"

#include "config.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#define ESP_MAXIMUM_RETRY 5

/**
* Configures ESP32 in Wifi Station Mode.
* Connects to specified Wifi network.
* Logs whether connection is successful.
* 
* @return esp_err_t ESP error code
*/
esp_err_t wifi_init_station(void);

/**
* Initializes and starts ESP32 as a MQTT client to specified MQTT broker.
*
* @return esp_err_t ESP error code
*/
esp_err_t mqtt_app_start(void);

/**
* Details whether MQTT client is connected or disconnected to MQTT broker
* 
* @return returns 0 if MQTT client is disconnected, otherwise 1
*/
int mqtt_is_connected(void);

/**
* Publishes message with specified topic from MQTT client to broker.
*
* @param[in] topic topic string
* @param[in] payload payload string
* @param[in] qos QoS of publish message
* @param[in] retain retain flag
* @return -1 if publish failed, -2 in case of full outbox, otherwise message_id of publish message
*/
int mqtt_publish(const char *topic, const char *payload, int qos, int retain);

#endif // WIFI_MQTT_H