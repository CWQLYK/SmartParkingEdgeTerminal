# ======================
# 车位检测AI训练（最终完美版）
# 输入：10个超声波滤波距离
# 输出：0=无车 1=有车
# 适配STM32F103 + X-CUBE-AI
# ======================
import tensorflow as tf
import pandas as pd
import numpy as np

# ============ 配置文件 ============
NO_CAR_CSV = "no_car.csv"
HAS_CAR_CSV = "has_car.csv"
# ==================================

# 读取数据
df_no_car = pd.read_csv(NO_CAR_CSV, header=None)
df_has_car = pd.read_csv(HAS_CAR_CSV, header=None)

# 1. 只取前10列 → 解决你的列数不匹配
# 2. 转 float32 → 解决量化报错
x0 = df_no_car.values[:, :10].astype(np.float32)
x1 = df_has_car.values[:, :10].astype(np.float32)

# 标签
y0 = np.zeros(len(x0), dtype=np.float32)
y1 = np.ones(len(x1), dtype=np.float32)

# 合并
X = np.vstack([x0, x1])
y = np.hstack([y0, y1])

# 打乱
idx = np.random.permutation(len(X))
X = X[idx]
y = y[idx]

# ============ 构建小模型 ============
model = tf.keras.Sequential([
    tf.keras.layers.Input(shape=(10,)),
    tf.keras.layers.Dense(16, activation='relu'),
    tf.keras.layers.Dense(8, activation='relu'),
    tf.keras.layers.Dense(1, activation='sigmoid')
])

model.compile(
    optimizer='adam',
    loss='binary_crossentropy',
    metrics=['accuracy']
)

print("开始训练...")
model.fit(X, y, epochs=50, batch_size=4, validation_split=0.1)

# ============ 量化导出（修复版） ============
converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.DEFAULT]

# 量化数据集（float32）
def representative_data_gen():
    for _ in range(100):
        i = np.random.randint(0, len(X))
        yield [X[i:i+1]]

converter.representative_dataset = representative_data_gen
converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
converter.inference_input_type = tf.float32
converter.inference_output_type = tf.float32

# 导出模型
tflite_model = converter.convert()
with open("park_model.tflite", "wb") as f:
    f.write(tflite_model)

print("\n✅ 成功！")
print("✅ 模型已生成：park_model.tflite")
print("✅ 输入：10个超声波距离（float32）")
print("✅ 可直接用于 STM32 X-CUBE-AI")