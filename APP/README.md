# 智能停车云平台

Python Flask 后端 + MQTT 订阅 + Web 仪表盘。

## 架构

```
broker.emqx.io:1883 (公共MQTT Broker)
        │
        │ subscribe "park/status"
        ▼
    Flask 后端 (app.py)
        │
        ├── paho-mqtt 订阅线程 → 解析JSON → SQLite
        │
        └── Web API ────→ 浏览器仪表盘 (index.html)
             GET /api/park/status
             GET /api/park/history
```

## 技术栈

| 层 | 技术 |
|----|------|
| 后端框架 | Flask 3.x |
| MQTT客户端 | paho-mqtt 2.x |
| 数据库 | SQLite3 |
| 前端 | 原生 HTML/CSS/JS，无框架 |
| MQTT Broker | EMQX Cloud (broker.emqx.io) |

## 目录结构

```
APP/server/
├── app.py              # Flask 主程序 (API + MQTT订阅)
├── templates/
│   └── index.html      # 仪表盘页面
├── parking.db          # SQLite 数据库 (自动创建)
├── requirements.txt    # Python 依赖
└── README.md
```

## 数据库

### park_current — 当前状态

| 列 | 类型 | 说明 |
|----|------|------|
| park_id | INTEGER PK | 车位编号 |
| state | INTEGER | 0=未知 1=空闲 2=占用 |
| updated_at | DATETIME | 最后更新时间 |

### park_records — 历史记录

| 列 | 类型 | 说明 |
|----|------|------|
| id | INTEGER PK | 自增主键 |
| park_id | INTEGER | 车位编号 |
| state | INTEGER | 状态值 |
| timestamp | DATETIME | 记录时间 |

## API

| 方法 | 路径 | 说明 |
|------|------|------|
| POST | `/api/park` | 接收车位数据 (兼容HTTP方式) |
| GET | `/api/park/status` | 获取当前所有车位状态 |
| GET | `/api/park/history?limit=N` | 获取最近N条历史记录 |
| GET | `/` | 仪表盘页面 |

## 快速启动

```bash
cd APP/server
pip install -r requirements.txt
python app.py
```

浏览器打开 http://localhost:5000。

## MQTT 消息格式

Topic: `park/status`

```json
{"park_id": 1, "state": 0}
```

| 字段 | 类型 | 说明 |
|------|------|------|
| park_id | int | 车位编号 (当前固定1) |
| state | int | 0=未知, 1=空闲, 2=占用 |

## 测试

```bash
# 模拟ESP32发送MQTT消息 (需安装mosquitto客户端)
mosquitto_pub -h broker.emqx.io -t "park/status" -m "{\"park_id\":1,\"state\":2}"

# 查询API
curl http://localhost:5000/api/park/status
```
