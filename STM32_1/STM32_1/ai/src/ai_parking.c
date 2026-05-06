#include "ai_parking.h"

// 模型权重（由 extract_model.py 自动生成）
#include "model_w1.h"  // Dense(10→16): 160 floats
#include "model_b1.h"  // Bias: 16 floats
#include "model_w2.h"  // Dense(16→8): 128 floats
#include "model_b2.h"  // Bias: 8 floats
#include "model_w3.h"  // Dense(8→1): 8 floats
#include "model_b3.h"  // Bias: 1 float

#define INPUT_DIM  10
#define HIDDEN1_DIM 16
#define HIDDEN2_DIM 8

static inline float relu(float x) { return (x > 0.0f) ? x : 0.0f; }

static inline float sigmoid(float x)
{
    // 数值稳定的 sigmoid 实现
    if (x >= 0) {
        return 1.0f / (1.0f + expf(-x));
    } else {
        float ex = expf(x);
        return ex / (1.0f + ex);
    }
}

/**
 * @brief 全连接层 + ReLU 激活
 * @param out 输出缓冲区
 * @param w 权重矩阵 [out_dim × in_dim]
 * @param b 偏置 [out_dim]
 */
static void dense_relu(float *out, const float *in,
                       const float *w, const float *b,
                       int in_dim, int out_dim)
{
    for (int i = 0; i < out_dim; i++) {
        float sum = b[i];
        for (int j = 0; j < in_dim; j++) {
            sum += w[i * in_dim + j] * in[j];
        }
        out[i] = relu(sum);
    }
}

/**
 * @brief 全连接层（无激活）
 */
static float dense_linear(const float *in,
                          const float *w, const float *b,
                          int in_dim)
{
    float sum = b[0];
    for (int j = 0; j < in_dim; j++) {
        sum += w[j] * in[j];
    }
    return sum;
}

float ai_parking_predict(float *features)
{
    // 归一化输入
    float x[INPUT_DIM];
    for (int i = 0; i < INPUT_DIM; i++) {
        x[i] = features[i] / NORM_SCALE;
    }

    // Layer 1: Dense(10→16) + ReLU
    float h1[HIDDEN1_DIM];
    dense_relu(h1, x, model_w1, model_b1, INPUT_DIM, HIDDEN1_DIM);

    // Layer 2: Dense(16→8) + ReLU
    float h2[HIDDEN2_DIM];
    dense_relu(h2, h1, model_w2, model_b2, HIDDEN1_DIM, HIDDEN2_DIM);

    // Layer 3: Dense(8→1) + Sigmoid
    float z = dense_linear(h2, model_w3, model_b3, HIDDEN2_DIM);
    return sigmoid(z);
}

void AI_Parking_Init(void)
{
    // 权重已编译为常量，无需初始化
}
