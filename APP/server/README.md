# 智能车位检测系统 - MQTT 云端服务器

## 架构

```
STM32 → UART → ESP32 → WiFi → MQTT Publish (park/status)
                                     ↓
                              MQTT Broker (Mosquitto)
                                     ↓
                              Flask 后端订阅 → SQLite → 网页仪表盘
```

## 快速启动

### 第一步：安装 MQTT Broker (Mosquitto)

Windows 下载地址：https://mosquitto.org/download/

安装后 Mosquitto 会自动作为服务运行，默认监听 `localhost:1883`。

验证是否安装成功：
```bash
mosquitto -v
```

### 第二步：安装 Python 依赖

```bash
pip install -r requirements.txt
```

### 第三步：启动服务器

```bash
python app.py
```

启动后：
- Web 仪表盘：http://localhost:5000
- Flask 后端自动连接本地 Mosquitto 并订阅 `park/status` 主题

## ESP32 配置

在以下文件中修改配置：

1. `ESP32_1/ESP32_1/include/user_wifi.h` — WiFi SSID 和密码
2. `ESP32_1/ESP32_1/include/task_cloud.h` — `MQTT_BROKER_IP` 改为你电脑的 IP 地址

## API 说明

| 方法 | 路径 | 说明 |
|------|------|------|
| POST | `/api/park` | (兼容保留) HTTP 上传车位数据 |
| GET  | `/api/park/status` | 获取当前车位状态 |
| GET  | `/api/park/history?limit=50` | 获取历史记录 |

## MQTT 主题

| 主题 | 方向 | 说明 |
|------|------|------|
| `park/status` | ESP32 → Broker → Flask | 车位状态数据 |

消息格式：`{"park_id":1,"state":0}` — state: 0=未知 1=空闲 2=占用

## 命令行测试

```bash
# 用 mosquitto_pub 模拟 ESP32 发送数据
mosquitto_pub -t "park/status" -m "{\"park_id\":1,\"state\":1}"   # 空闲
mosquitto_pub -t "park/status" -m "{\"park_id\":1,\"state\":2}"   # 占用

# 查询状态
curl http://localhost:5000/api/park/status
```
