#include "task_uart.h"

void send_parking_data_task(void)
{
    // 发送车位状态给主机
    UART2_SendData_Master(SLAVE_ADDR, ParkData.park_state); // 从机地址0x01，发送当前车位状态
}
