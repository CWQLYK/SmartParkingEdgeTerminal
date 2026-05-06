from flask import Flask, request, jsonify, render_template
import sqlite3
import threading
import json
from datetime import datetime
import os

import paho.mqtt.client as mqtt

app = Flask(__name__)
DB_PATH = os.path.join(os.path.dirname(__file__), "parking.db")

# MQTT Broker 配置（和 ESP32 task_cloud.h 中的保持一致）
MQTT_BROKER   = "broker.emqx.io"
MQTT_PORT     = 1883
MQTT_TOPIC    = "park/status"

STATE_NAMES = {0: "未知", 1: "空闲", 2: "占用"}

# ==================== 数据库 ====================

def init_db():
    conn = sqlite3.connect(DB_PATH)
    c = conn.cursor()
    c.execute("""CREATE TABLE IF NOT EXISTS park_records (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        park_id INTEGER NOT NULL,
        state INTEGER NOT NULL,
        timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
    )""")
    c.execute("""CREATE TABLE IF NOT EXISTS park_current (
        park_id INTEGER PRIMARY KEY,
        state INTEGER NOT NULL DEFAULT 0,
        updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
    )""")
    c.execute("INSERT OR IGNORE INTO park_current (park_id, state) VALUES (1, 0)")
    conn.commit()
    conn.close()

def save_park_data(park_id, state):
    conn = sqlite3.connect(DB_PATH)
    c = conn.cursor()
    c.execute("INSERT INTO park_records (park_id, state) VALUES (?, ?)", (park_id, state))
    c.execute("""INSERT INTO park_current (park_id, state, updated_at)
                 VALUES (?, ?, ?)
                 ON CONFLICT(park_id) DO UPDATE SET state=?, updated_at=?""",
              (park_id, state, datetime.now(), state, datetime.now()))
    conn.commit()
    conn.close()
    print(f"[MQTT收到] 车位{park_id}: {STATE_NAMES.get(state, '?')}")

# ==================== MQTT 订阅 ====================

def on_connect(client, userdata, flags, reason_code, props=None):
    if reason_code == 0:
        print(f"[MQTT] 已连接 Broker {MQTT_BROKER}")
        client.subscribe(MQTT_TOPIC)
        print(f"[MQTT] 已订阅主题: {MQTT_TOPIC}")
    else:
        print(f"[MQTT] 连接失败, code={reason_code}")

def on_message(client, userdata, msg):
    try:
        payload = msg.payload.decode("utf-8")
        print(f"[MQTT] 收到 {msg.topic}: {payload}")
        data = json.loads(payload)
        park_id = data.get("park_id")
        state   = data.get("state")
        if park_id is not None and state in (0, 1, 2):
            save_park_data(park_id, state)
    except Exception as e:
        print(f"[MQTT] 数据解析失败: {e}")

def start_mqtt():
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.on_connect = on_connect
    client.on_message = on_message

    # 断线自动重连
    client.reconnect_delay_set(min_delay=1, max_delay=30)

    print(f"[MQTT] 正在连接 {MQTT_BROKER}:{MQTT_PORT} ...")
    client.connect_async(MQTT_BROKER, MQTT_PORT, keepalive=60)
    client.loop_start()  # 后台线程自动处理网络IO
    return client

# ==================== Flask API ====================

@app.route("/api/park", methods=["POST"])
def receive_park_data():
    """HTTP 方式上传车位数据（兼容保留）"""
    data = request.get_json()
    if not data or "park_id" not in data or "state" not in data:
        return jsonify({"error": "缺少 park_id 或 state"}), 400

    park_id = data["park_id"]
    state = data["state"]

    if state not in (0, 1, 2):
        return jsonify({"error": "state 必须为 0/1/2"}), 400

    save_park_data(park_id, state)
    return jsonify({"status": "ok"}), 200

@app.route("/api/park/status", methods=["GET"])
def get_park_status():
    conn = sqlite3.connect(DB_PATH)
    c = conn.cursor()
    c.execute("SELECT park_id, state, updated_at FROM park_current ORDER BY park_id")
    rows = c.fetchall()
    conn.close()

    result = []
    for r in rows:
        result.append({
            "park_id": r[0],
            "state": r[1],
            "state_name": STATE_NAMES.get(r[1], "?"),
            "updated_at": r[2]
        })
    return jsonify(result)

@app.route("/api/park/history", methods=["GET"])
def get_park_history():
    limit = request.args.get("limit", 50, type=int)
    conn = sqlite3.connect(DB_PATH)
    c = conn.cursor()
    c.execute("SELECT park_id, state, timestamp FROM park_records ORDER BY id DESC LIMIT ?", (limit,))
    rows = c.fetchall()
    conn.close()

    result = []
    for r in rows:
        result.append({
            "park_id": r[0],
            "state": r[1],
            "state_name": STATE_NAMES.get(r[1], "?"),
            "timestamp": r[2]
        })
    return jsonify(result)

@app.route("/")
def index():
    return render_template("index.html")

# ==================== 主程序 ====================

if __name__ == "__main__":
    init_db()

    print("\n" + "=" * 50)
    print("  智能车位检测系统 - MQTT云端服务器")
    print("=" * 50)
    print(f"  MQTT Broker: {MQTT_BROKER}:{MQTT_PORT}")
    print(f"  订阅主题:    {MQTT_TOPIC}")
    print(f"  Web仪表盘:   http://localhost:5000")
    print("=" * 50 + "\n")

    mqtt_client = start_mqtt()
    app.run(host="0.0.0.0", port=5000, debug=False)
