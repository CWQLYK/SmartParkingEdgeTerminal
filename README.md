# SmartParkingEdgeTerminal — 边缘计算智能车位检测系统

基于 STM32 + ESP32 + MQTT + Flask 的端到端 IoT 智能停车解决方案，覆盖**传感器驱动 → 边缘 AI 推理 → 网关传输 → 云平台 → Web 可视化**的完整数据管道。

---

## 系统架构

```
┌──────────────────────────────────────────────────────┐
│                   Edge Sensing Layer                  │
│  ┌─────────────────────────────────────────────────┐ │
│  │           STM32F103C8T6 (Cortex-M3, 72MHz)      │ │
│  │                                                 │ │
│  │  HC-SR04 Ultrasonic ──→  Two-Stage Filter      │ │
│  │  (Trig/Echo时序驱动)     (限幅 + 去极值滑动平均)  │ │
│  │                              │                  │ │
│  │                              ▼                  │ │
│  │                    Edge AI Inference            │ │
│  │              Dense(10→16)→ReLU                  │ │
│  │              Dense(16→8)→ReLU                   │ │
│  │              Dense(8→1)→Sigmoid                 │ │
│  │              纯C推理引擎 · 321参数               │ │
│  │              Flash 1.3KB · RAM <2KB             │ │
│  │                         │                      │ │
│  │              ┌──────────▼──────────┐            │ │
│  │              │  AI+Threshold Fusion │            │ │
│  │              │  三级置信度决策策略   │            │ │
│  │              └──────────┬──────────┘            │ │
│  │                         │                      │ │
│  │              10-Sample Debounce                 │ │
│  │                         │                      │ │
│  │              Custom 5-Byte Frame                │ │
│  │              (AA|ADDR|STATE|CKSUM|55)           │ │
│  │                         │                      │ │
│  │         FreeRTOS: 4 Tasks (Sensor|Park|UART|LP) │ │
│  └─────────────────────────┼──────────────────────┘ │
└────────────────────────────┼────────────────────────┘
                             │ UART 115200 bps 8N1
                             │
┌────────────────────────────┼────────────────────────┐
│                   Edge Gateway Layer                 │
│  ┌─────────────────────────▼──────────────────────┐ │
│  │          ESP32-S3 (Xtensa LX7 Dual-Core)       │ │
│  │                                                 │ │
│  │  Frame Sync Parser (triple validation)          │ │
│  │  State Change Detection                         │ │
│  │  WiFi Auto-Reconnect                            │ │
│  │  MQTT Client (PubSubClient)                     │ │
│  │                                                 │ │
│  │    FreeRTOS: 2 Tasks (comm_task|cloud_task)     │ │
│  └─────────────────────────┬──────────────────────┘ │
└────────────────────────────┼────────────────────────┘
                             │ MQTT Publish
                             │ Topic: park/status
                             │ QoS 0, JSON Payload
                             ▼
                  ┌──────────────────┐
                  │  EMQX Public MQTT │
                  │  broker.emqx.io   │
                  │     :1883        │
                  └────────┬─────────┘
                           │ MQTT Subscribe
                           ▼
┌─────────────────────────────────────────────────────┐
│                  Cloud & Frontend                    │
│  ┌───────────────────────────────────────────────┐  │
│  │          Flask Server (Python 3)               │  │
│  │                                                │  │
│  │  paho-mqtt → JSON Parse → SQLite Storage      │  │
│  │       │                                       │  │
│  │       ├── GET /api/park/status   (RESTful)    │  │
│  │       ├── GET /api/park/history               │  │
│  │       └── GET /              (Dashboard SPA)  │  │
│  └───────────────────────┬───────────────────────┘  │
│                          │                          │
│  ┌───────────────────────▼───────────────────────┐  │
│  │         Web Dashboard (Zero-Dependency)        │  │
│  │    Dark Theme · Real-Time Cards · History      │  │
│  │    AJAX Polling @ 3s · Color-Coded Status      │  │
│  └───────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────┘
```

---

## 技术栈

| 层级 | 技术 | 说明 |
|------|------|------|
| **边缘感知** | STM32F103C8T6 + HC-SR04 | 超声波测距 (2cm–400cm)，HAL 库驱动 |
| **信号处理** | 限幅滤波 + 去极值滑动平均 | 窗口=10，抑制脉冲噪声与高斯噪声 |
| **边缘 AI** | 纯 C 手写推理引擎 | 3层 DNN (10→16→8→1)，321 参数，无框架依赖 |
| **RTOS** | FreeRTOS (CMSIS-RTOS v1 / ESP-IDF) | STM32 4任务 + ESP32 2任务，优先级调度 |
| **通信协议** | UART 自定义 5 字节帧 (主机模式) | 帧头/帧尾/校验和三重校验 + 字节级流同步 |
| **消息中间件** | MQTT 3.1.1 (EMQX Public Broker) | QoS 0，JSON 负载，变化触发上传 |
| **网关** | ESP32-S3 + PubSubClient | WiFi 自动重连，MQTT 断线重连 |
| **云平台** | Python Flask + paho-mqtt + SQLite | RESTful API，MQTT 订阅 + HTTP 双通道 |
| **前端** | 原生 HTML/CSS/JS (零依赖) | 暗色主题，3秒 AJAX 轮询，自适应布局 |
| **AI 训练** | TensorFlow/Keras (离线) | 合成数据训练，权重自动导出为 C 头文件 |

---

## 数据流

```
HC-SR04 超声波测距
    │
    ▼
限幅滤波 (抑制跳变噪声, Δmax=50cm)
    │
    ▼
去极值滑动平均滤波 (窗口=10, 去掉最大最小值后取平均)
    │
    ├──────────────────────────┐
    ▼                          ▼
10帧环形缓冲              阈值判定
    │                    (≤50cm → 占用)
    ▼                          │
Edge AI 推理 (3层DNN)          │
    │                          │
    ▼                          │
┌───────────────────────────────┐
│   AI + 阈值 三级置信度融合决策   │
│   ≥90%: AI 直接生效            │
│   70-90%: AI与阈值一致时采信    │
│   <70%: 退化到阈值兜底          │
└──────────────┬────────────────┘
               ▼
        10 次防抖确认
               │
               ▼
      5字节帧封装 (AA|01|STATE|CKSUM|55)
               │
               ▼
        UART2 TX → ESP32 RX
               │
               ▼
    ESP32 帧同步解析 (三重校验 + 字节流同步)
               │
               ▼
        状态变化检测 (仅在变化时触发上传)
               │
               ▼
        MQTT Publish → park/status
               │
               ▼
    Flask 云平台 (paho-mqtt Subscribe)
               │
               ▼
        SQLite 持久化存储
               │
               ▼
    RESTful API → Web 仪表盘 (3秒轮询刷新)
```

---

## 目录结构

```
SmartParkingEdgeTerminal/
│
├── STM32_1/                          # STM32 边缘检测终端
│   └── STM32_1/
│       ├── Core/                     # STM32CubeMX 生成 (HAL + FreeRTOS 配置)
│       │   ├── Inc/                  #   main.h, FreeRTOSConfig.h, gpio.h, usart.h...
│       │   └── Src/                  #   main.c, freertos.c, stm32f1xx_it.c...
│       ├── Hardware/                 # 外设驱动层
│       │   ├── inc/                  #   HC_SR04.h, led.h, uart_comm.h
│       │   └── src/                  #   HC_SR04.c, led.c, uart_comm.c
│       ├── Algorithm/                # 算法层
│       │   ├── inc/                  #   filter.h, park_state.h, tinyml_infer.h
│       │   └── src/                  #   filter.c (组合滤波), park_state.c (融合决策)
│       ├── Protocol/                 # 通信协议层
│       │   ├── inc/                  #   protocol_master.h
│       │   └── src/                  #   protocol_master.c (5字节帧封装)
│       ├── App/                      # FreeRTOS 应用任务层
│       │   ├── inc/                  #   task_sensor.h, task_park.h, task_uart.h, task_lowpower.h
│       │   └── src/                  #   各任务入口函数
│       ├── ai/                       # Edge AI 模块
│       │   ├── inc/                  #   ai_parking.h, 模型权重头文件 (model_w*.h, model_b*.h)
│       │   ├── src/                  #   ai_parking.c (纯C手写推理引擎)
│       │   └── tool/                 #   模型训练 & 导出脚本
│       │       ├── train_model.py    #     训练脚本 (Keras)
│       │       ├── extract_model.py  #     权重提取 → C头文件
│       │       └── *.csv             #     训练数据样本
│       ├── Drivers/                  # STM32Cube HAL & CMSIS
│       ├── Middlewares/              # FreeRTOS 内核 (v10.3.1)
│       └── MDK-ARM/                  # Keil MDK 工程文件
│           └── STM32_1.uvprojx       #   Keil 项目入口
│
├── ESP32_1/                          # ESP32 边缘网关
│   └── ESP32_1/
│       ├── platformio.ini            #   PlatformIO 构建配置
│       ├── src/
│       │   ├── main.cpp              #    入口 & 初始化
│       │   ├── user_freertos.cpp     #    FreeRTOS 任务创建
│       │   ├── user_usart.cpp        #    UART2 初始化
│       │   ├── user_wifi.cpp         #    WiFi 连接管理
│       │   ├── task_usart.cpp        #    UART 帧接收 & 解析 & 变化检测
│       │   └── task_cloud.cpp        #    MQTT 连接 & 发布
│       └── include/                  #   头文件
│
└── APP/                              # 云平台
    └── server/
        ├── app.py                    #   Flask 主程序 (MQTT订阅 + RESTful API)
        ├── requirements.txt          #   flask, paho-mqtt
        └── templates/
            └── index.html            #   Web 仪表盘 (暗色主题 · 零依赖)
```

---

## 关键设计

### 1. 边缘 AI 推理引擎

在仅有 **64KB Flash / 20KB RAM** 的 STM32F103C8T6 上实现手写纯 C 神经网络推理：

| 属性 | 值 |
|------|-----|
| 架构 | 全连接: 10 → 16 (ReLU) → 8 (ReLU) → 1 (Sigmoid) |
| 参数量 | 321 floats (160+16+128+8+8+1) |
| 模型存储 | ~1.3 KB Flash (C 常量数组) |
| 运行时内存 | < 2 KB RAM |
| 依赖 | 仅 `math.h` (expf)，无 TFLite Micro / CMSIS-NN |
| 数值稳定性 | 分段 Sigmoid 实现，防止 exp 溢出 |

权重通过 Python 离线训练后自动导出为 C 头文件，编译时直接链接，零运行时加载开销。

### 2. AI + 阈值三级置信度融合

| AI 置信度 | 条件 | 策略 |
|-----------|------|------|
| ≥ 90% | AI 高置信度 | 直接采信 AI 结果 |
| 70%–90% | AI 与阈值一致 | 采信 AI 结果 |
| 70%–90% | AI 与阈值分歧 | 退化到阈值判定（兜底） |
| < 70% 或未就绪 | 低置信度 / 预热中 | 退化到阈值判定 |

上电前 10 个采样周期为 AI 预热期，期间完全由阈值判定保障可用性。

### 3. 组合滤波算法

- **限幅滤波**：相邻采样变化 > 50cm 时判定为脉冲噪声，丢弃并保持上次有效值
- **去极值滑动平均**：窗口=10，去掉最大和最小值后计算剩余 8 个值的平均值
- 两级串联，先抑制突发噪声再平滑随机波动

### 4. 自定义通信协议

```
┌──────┬──────────┬──────────┬──────────┬──────┐
│ 0xAA │  SLAVE   │  STATE   │ CHECKSUM │ 0x55 │
│ 帧头  │ 从机地址  │ 车位状态  │ 校验和    │ 帧尾  │
│  1B  │   1B     │   1B     │   1B     │  1B  │
└──────┴──────────┴──────────┴──────────┴──────┘
```

- STATE: 0=未知, 1=空闲, 2=占用
- CHECKSUM = (SLAVE + STATE) % 256
- ESP32 端三重校验：帧头 + 帧尾 + 校验和，外加 5ms 字节超时

### 5. 状态变化触发上传

ESP32 仅在检测到车位状态 **发生实际变化** 时才触发 MQTT 发布，持续不变时只维持心跳，大幅减少网络流量和云端写入。

---

## 快速开始

### 环境要求

| 组件 | 依赖 |
|------|------|
| STM32 | Keil MDK v5, ARM Compiler 5/6, ST-Link/J-Link |
| ESP32 | PlatformIO, USB 串口驱动 |
| 云平台 | Python 3.8+, pip |

### 1. 启动云平台

```bash
cd APP/server
pip install -r requirements.txt
python app.py
# 打开浏览器访问 http://localhost:5000
```

### 2. 编译 & 烧录 ESP32

修改 `ESP32_1/ESP32_1/include/user_wifi.h` 中的 WiFi SSID 和密码：

```c
#define WIFI_SSID     "你的WiFi名称"
#define WIFI_PASSWORD "你的WiFi密码"
```

```bash
cd ESP32_1/ESP32_1
pio run --target upload && pio device monitor
```

### 3. 编译 & 烧录 STM32

用 Keil MDK 打开 `STM32_1/STM32_1/MDK-ARM/STM32_1.uvprojx`，编译后通过 ST-Link 烧录。

### 4. 硬件连接

```
STM32 USART2 TX (PA2)  ────  ESP32 GPIO16 (UART2 RX)
STM32 USART2 RX (PA3)  ────  ESP32 GPIO17 (UART2 TX)
STM32 GND              ────  ESP32 GND
HC-SR04 Trig ────STM32 PA7,  Echo ────STM32 PA6
LED0 ────STM32 PA0,  LED1 ────STM32 PA1
```

---

## 性能指标

| 指标 | 数值 |
|------|------|
| STM32 传感器采样周期 | 100 ms |
| 滤波延迟 | < 1 ms |
| AI 单次推理耗时 | < 5 ms (72MHz Cortex-M3) |
| 状态确认延迟 (含防抖) | ≤ 1 s (10 × 100ms) |
| UART 上报间隔 | 500 ms |
| MQTT 端到端延迟 | < 2 s (含 WiFi + Broker + Python) |
| Web 仪表盘刷新间隔 | 3 s |
| AI 模型大小 | 1.3 KB |
| STM32 总 Flash 占用 | ~30 KB (含 HAL + FreeRTOS + AI) |
| ESP32 总 Flash 占用 | ~800 KB (含 Arduino + WiFi 栈) |

---

## 扩展方向

- **多车位支持**：在协议帧中扩展车位 ID 字段，STM32 端增加多路传感器分时采集
- **OTA 升级**：ESP32 端通过 MQTT 下发 STM32 固件，利用 STM32 内置 Bootloader 更新
- **持续学习**：云端积累真实车位数据，周期性重训练模型并通过 OTA 更新权重
- **低功耗优化**：Sleep 模式 + 定时唤醒采集（已预留 `lowPowerTask` 框架）
- **边缘 TTS 播报**：ESP32 端集成语音合成，空闲时主动播报引导车辆
- **移动端 APP**：通过 RESTful API 开发配套手机应用

---

## License

MIT

---

*Author: 咸 · 2026*
