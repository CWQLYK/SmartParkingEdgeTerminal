# ESP32 边缘网关

基于 ESP32-S3 + FreeRTOS + PlatformIO 的 MQTT 网关，接收 STM32 车位数据并上传到云平台。

## 硬件

| 组件 | 引脚 |
|------|------|
| MCU | ESP32-S3-DevKitM-1 |
| 通信串口 | UART2 (RX=GPIO16, TX=GPIO17) → STM32 |
| 调试串口 | UART0 (USB) |
| WiFi | 2.4GHz 内置 |

## 软件架构

```
ESP32-S3 (FreeRTOS)
├── comm_task    接收STM32帧 → 帧同步解析 → 变化检测
├── cloud_task   状态变化时 → WiFi → MQTT发布
└── WiFi管理    自动连接 + 断线重连
```

## FreeRTOS 任务

| 任务 | 优先级 | 栈 | 周期 | 功能 |
|------|--------|-----|------|------|
| `comm_task` | 1 | 2KB | 500ms | 轮询 UART2，帧同步解析，变化检测 |
| `cloud_task` | 2 | 4KB | 500ms | WiFi/MQTT 保活，状态变化时发布 |

## 模块说明

### UART 接收 (`user_usart.cpp`)

从 STM32 接收 5 字节帧，通过帧头 `0xAA` 同步，验证帧尾 `0x55` + 校验和。

### 帧解析 (`task_usart.cpp`)

```cpp
// 逐字节搜索帧头 0xAA → 读4字节 → 验证帧尾 + 校验和
// 三重校验通过 → 状态变化时标记 g_park_state_updated = true
```

- 帧头 `0xAA` 同步，跳过杂散字节（解决串口噪声导致的帧偏移问题）
- 双重校验：帧尾 `0x55` + 校验和 `(addr + state) % 256`
- 状态变化检测：只有 `state != last_state` 才触发上传

### MQTT 上传 (`task_cloud.cpp`)

```
PubSubClient → broker.emqx.io:1883
Topic: park/status
Payload: {"park_id":1,"state":0}
QoS: 0 (最多一次)
```

- WiFi MAC 地址作为 ClientID，避免多设备冲突
- WiFi 断线自动重连
- MQTT 断线自动重连

## 配置

编译前修改以下宏定义：

| 文件 | 宏 | 说明 |
|------|-----|------|
| `user_wifi.h` | `WIFI_SSID` / `WIFI_PASSWORD` | WiFi 名称和密码 |
| `task_cloud.h` | `MQTT_BROKER_IP` | MQTT Broker 地址 |

## 编译与烧录

```bash
# PlatformIO
pio run --target upload

# 查看调试输出
pio device monitor
```

调试输出示例：
```
[WiFi] 连接成功！IP: 192.168.1.100
[MQTT] 连接成功 (client: ESP32-Park-XX)
[UART] 地址=0x01, 状态=1
[UART] 状态变化，触发上传: 1
[MQTT] Publish topic=park/status: {"park_id":1,"state":1}
[MQTT] 发送成功
```
