# STM32 边缘检测终端

基于 STM32F103C8T6 + FreeRTOS + HC-SR04 超声波传感器的车位状态检测系统。

## 硬件

| 组件 | 型号/引脚 |
|------|-----------|
| MCU | STM32F103C8T6 |
| 超声波模块 | HC-SR04 (Trig=PA7, Echo=PA6) |
| 状态指示灯 | LED0=PA0, LED1=PA1 |
| 通信串口 | USART2 → ESP32 |

## 软件架构

```
STM32F103 (FreeRTOS)
├── sensorTask    获取超声波数据 → 组合滤波
├── parkTask      判断车位状态 → LED指示
├── UARTTask      封装5字节帧 → 发送ESP32
└── lowPowerTask  低功耗管理
```

## 模块说明

### Algorithm — 数据处理层

| 模块 | 文件 | 功能 |
|------|------|------|
| 组合滤波 | `filter.c` | 限幅滤波(max delta=50cm) + 去极值滑动平均(窗口=10) |
| 状态判定 | `park_state.c` | 距离 ≤ 50cm → 占用，＞50cm → 空闲，连续10次确认防抖 |

### Hardware — 硬件驱动层

| 模块 | 文件 | 功能 |
|------|------|------|
| HC-SR04 | `HC_SR04.c` | 超声波 Trig/Echo 驱动，返回距离(cm) |
| UART通信 | `uart_comm.c` | USART2中断接收 + 阻塞发送 |
| LED | `led.c` | 空闲=LED0亮，占用=LED1亮 |

### Protocol — 通信协议

5字节帧格式（主机模式，单向发送）：

```
[0] 0xAA        帧头
[1] slave_addr  从机地址 (0x01)
[2] state       车位状态 (0=未知 1=空闲 2=占用)
[3] checksum    (addr + state) % 256
[4] 0x55        帧尾
```

`protocol_master.c` 中的 `Simple_Protocol_Pack()` 负责封装。

### App — 应用任务层

| 任务 | 周期 | 功能 |
|------|------|------|
| `get_sensor_data_task` | ~200ms | 获取超声波原始数据 → 组合滤波 |
| `park_state_task` | ~200ms | 滤波数据 → 阈值判定 → 防抖 → LED |
| `send_parking_data_task` | ~200ms | 封装车位状态帧 → USART2 发送 ESP32 |

### AI — TinyML 边缘推理

| 模块 | 文件 | 功能 |
|------|------|------|
| 推理引擎 | `ai_parking.c` | 纯 C 手写推理（无框架依赖） |
| 权重头文件 | `model_w1~w3.h, model_b1~b3.h` | TFLite 模型提取的权重 |
| 模型提取工具 | `ai/tool/extract_model.py` | 从 TFLite 导出 C 数组 |

**模型架构：** 3层全连接神经网络 (10→16→8→1)
- 输入：最近10次滤波距离值（单位cm，归一化 /100）
- 输出：车位占用概率（0~1，>0.5=占用）
- 参数：321个 float，~1.3KB Flash，推理时 <2KB RAM
- 依赖：仅需 `math.h` 的 `expf()`，无需 TFLite Micro 或任何外部框架

**决策融合策略：**
```
AI 高置信度(>90%) → 直接采纳 AI 结果
AI 中置信度(>70%) + 与阈值一致 → 采纳
AI 低置信度 / 分歧 → 退化为 50cm 阈值判定
```
所有结果均经过 10 次连续确认防抖，消除状态抖动。

## 编译与烧录

- IDE: Keil MDK-ARM
- 工程文件: `MDK-ARM/STM32_1.uvprojx`
- 烧录工具: ST-Link / J-Link
