#include "protocol_master.h"

/**
 * @brief 封装通信帧（从机地址+车位状态）
 * @param slave_addr 从机地址
 * @param state 车位状态
 * @param frame_buf 输出帧缓存（需至少5字节）
 * @retval 帧长度（固定5）
 */
uint8_t Simple_Protocol_Pack(uint8_t slave_addr, uint8_t state, uint8_t *frame_buf)
{
    if (frame_buf == NULL)
    {
        return 0; // 无效参数
    }

    // 构建帧
    frame_buf[0] = FRAME_HEAD;                 // 帧头
    frame_buf[1] = slave_addr;                 // 从机地址
    frame_buf[2] = state;                      // 车位状态
    frame_buf[3] = (slave_addr + state) % CHECKSUM_MOD; // 校验位（简单求和取模）
    frame_buf[4] = FRAME_TAIL;                 // 帧尾

    return FRAME_LEN; // 返回帧长度
}
