#include "user_wifi.h"
#include "user_usart.h"

void wifi_init(void)
{
    DEBUG_SERIAL.printf("[WiFi] 正在连接 %s ...\r\n", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 40) {
        vTaskDelay(pdMS_TO_TICKS(500));
        DEBUG_SERIAL.print(".");
        retry++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        DEBUG_SERIAL.printf("\r\n[WiFi] 连接成功！IP: %s\r\n", WiFi.localIP().toString().c_str());
    } else {
        DEBUG_SERIAL.printf("\r\n[WiFi] 连接失败，将继续重试\r\n");
    }
}
