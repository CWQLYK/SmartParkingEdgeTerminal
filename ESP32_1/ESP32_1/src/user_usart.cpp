#include "user_usart.h"

void uart_init_all(void)
{
    DEBUG_SERIAL.begin(DEBUG_SERIAL_BAUD);
    COMM_SERIAL.begin(COMM_SERIAL_BAUD, SERIAL_8N1, COMM_RX_PIN, COMM_TX_PIN);

    while (!DEBUG_SERIAL) ;
    while (!COMM_SERIAL) ;

    DEBUG_SERIAL.printf("双串口初始化完成！\r\n");
    DEBUG_SERIAL.printf("调试串口：USB打印\r\n");
    DEBUG_SERIAL.printf("通信串口：RX=16, TX=17\r\n");
}

void comm_send(uint8_t *data, uint16_t len)
{
    if (len == 0 || data == NULL) return;
    COMM_SERIAL.write(data, len);
}
