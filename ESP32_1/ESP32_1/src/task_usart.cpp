#include "task_usart.h"

void user_usart_task(void)
{
    // 逐字节读取，找帧头 0xAA
    while (COMM_SERIAL.available() > 0) {
        uint8_t b = COMM_SERIAL.read();

        if (b != FRAME_HEAD) {
            // 不是帧头，丢弃
            continue;
        }

        // 找到帧头，等待剩余4字节到达（最多等5ms）
        unsigned long deadline = millis() + 5;
        while (COMM_SERIAL.available() < 4) {
            if (millis() > deadline) break;
        }
        if (COMM_SERIAL.available() < 4) continue;

        uint8_t buf[4];
        for (int i = 0; i < 4; i++) {
            buf[i] = COMM_SERIAL.read();
        }

        // 验证帧尾
        if (buf[3] != FRAME_TAIL) continue;

        uint8_t addr  = buf[0];
        uint8_t state = buf[1];
        uint8_t cksum = buf[2];

        // 验证校验和: (addr + state) % 256
        if (((addr + state) & 0xFF) != cksum) continue;

        DEBUG_SERIAL.printf("[UART] 地址=0x%02X, 状态=%d\r\n", addr, state);

        // 只有状态变化时才通知云端上传
        if (state != g_park_state) {
            g_park_state = state;
            g_park_state_updated = true;
            DEBUG_SERIAL.printf("[UART] 状态变化，触发上传: %d\r\n", state);
        }
    }
}
