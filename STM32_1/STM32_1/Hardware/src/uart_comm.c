#include "uart_comm.h"
#include "string.h"

// 全局接受缓存
uint8_t uart2_rx_buf[UART2_RX_BUF_SIZE];
uint16_t uart2_rx_len = 0;
// 发送数据帧数组
uint8_t frame_buf[FRAME_LEN];

// USART2通信初始化
void UART2_Comm_Init(void)
{
    // 清空缓存
    memset(uart2_rx_buf, 0, UART2_RX_BUF_SIZE);
    uart2_rx_len = 0;

    // 开启USART2接收中断
    HAL_UART_Receive_IT(&huart2, uart2_rx_buf, UART2_RX_SINGLE_SIZE);
}

// 发送数据（阻塞式，保证数据完整发送）
void UART2_SendData(uint8_t *data, uint16_t len)
{
    if (len == 0 || data == NULL)
    {
        return; // 无数据可发送
    }

    // 禁用接受中断，避免发送过程中接收数据干扰
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);

    // 发送数据
    HAL_UART_Transmit(&huart2, data, len, UART2_TX_TIMEOUT);

    // 重新开启接受中断
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
}

// USART2接收中断回调（需在stm32f1xx_it.c中调用）
void UART2_RxCpltCallback(void)
{
    uart2_rx_len++;

    // 缓存满重置
    if (uart2_rx_len >= UART2_RX_BUF_SIZE)
    {
        uart2_rx_len = 0;
    }

    // 重新开启接收中断，继续接收下一字节数据
    HAL_UART_Receive_IT(&huart2, &uart2_rx_buf[uart2_rx_len], 1);
}

/**
 * @brief 发送车位状态给主机
 * @param slave_addr 自身从机地址
 * @param state 车位状态
 */
void UART2_SendData_Master(uint8_t slave_addr, uint8_t state)
{
    // 封装数据帧
    uint8_t frame_len = Simple_Protocol_Pack(slave_addr, state, frame_buf);
    if (frame_len > 0)
    {
        // 发送数据帧
        UART2_SendData(frame_buf, frame_len);
    }
}
