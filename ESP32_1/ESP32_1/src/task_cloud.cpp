#include "task_cloud.h"
#include "user_wifi.h"
#include <WiFi.h>
#include <PubSubClient.h>

volatile uint8_t g_park_state         = 0;
volatile bool    g_park_state_updated = false;

static WiFiClient   wifi_client;
static PubSubClient mqtt_client(wifi_client);

// 连接到 MQTT Broker
static bool mqtt_connect(void)
{
    DEBUG_SERIAL.printf("[MQTT] 连接 Broker %s:%d ...\r\n", MQTT_BROKER_IP, MQTT_BROKER_PORT);
    mqtt_client.setServer(MQTT_BROKER_IP, MQTT_BROKER_PORT);

    // 用 ESP32 MAC 地址作为客户端ID，避免冲突
    char client_id[32];
    snprintf(client_id, sizeof(client_id), "ESP32-Park-%04X", (uint16_t)WiFi.macAddress()[5]);

    if (mqtt_client.connect(client_id)) {
        DEBUG_SERIAL.printf("[MQTT] 连接成功 (client: %s)\r\n", client_id);
        return true;
    }
    DEBUG_SERIAL.printf("[MQTT] 连接失败, state=%d\r\n", mqtt_client.state());
    return false;
}

void cloud_upload_task(void *pvParameters)
{
    while (1) {
        // 检查WiFi
        if (WiFi.status() != WL_CONNECTED) {
            DEBUG_SERIAL.printf("[Cloud] WiFi未连接，尝试重连...\r\n");
            WiFi.reconnect();
            vTaskDelay(pdMS_TO_TICKS(10000));
            continue;
        }

        // 检查MQTT连接
        if (!mqtt_client.connected()) {
            if (!mqtt_connect()) {
                vTaskDelay(pdMS_TO_TICKS(5000));
                continue;
            }
        }

        // 等待有新的车位数据
        if (!g_park_state_updated) {
            mqtt_client.loop(); // 保持MQTT心跳
            vTaskDelay(pdMS_TO_TICKS(500));
            continue;
        }

        uint8_t state = g_park_state;
        g_park_state_updated = false;

        // 构造JSON
        char json_buf[64];
        snprintf(json_buf, sizeof(json_buf),
                 "{\"park_id\":1,\"state\":%d}", state);

        DEBUG_SERIAL.printf("[MQTT] Publish topic=%s: %s\r\n", MQTT_TOPIC, json_buf);

        if (mqtt_client.publish(MQTT_TOPIC, json_buf)) {
            DEBUG_SERIAL.printf("[MQTT] 发送成功\r\n");
        } else {
            DEBUG_SERIAL.printf("[MQTT] 发送失败\r\n");
            g_park_state_updated = true; // 失败恢复标记，下次重试
        }

        mqtt_client.loop();
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
