#include "park_state.h"
#include "ai_parking.h"
#include <stdio.h>

static uint8_t debounce_cnt = 0;
static uint8_t sample_count = 0;

void ParkState_Init(void)
{
    ParkData.park_state = PARK_STATE_FREE;
    debounce_cnt = 0;
    sample_count  = 0;
}

void ParkState_Update(float filter_dist)
{
    if (sample_count < 10) sample_count++;

    // 1. AI inference
    float ai_prob = 0.5f;
    uint8_t ai_ready = 0;

    if (sample_count >= 10) {
        ai_prob = ai_parking_predict(Filter_Distances);
        ai_ready = 1;
    }

    // 2. Threshold fallback
    uint8_t thresh_state = (filter_dist <= PARK_DIST_THRESHOLD)
                         ? PARK_STATE_OCCUPIED : PARK_STATE_FREE;

    // 3. Fusion decision
    uint8_t new_state;
    const char *reason = "THRESH";

    if (ai_ready && (ai_prob >= AI_CONFIDENCE_HIGH || ai_prob <= (1.0f - AI_CONFIDENCE_HIGH))) {
        uint8_t ai_state = (ai_prob >= 0.5f) ? PARK_STATE_OCCUPIED : PARK_STATE_FREE;
        if (ai_state == thresh_state) {
            new_state = ai_state;
            reason = "AI_HIGH";
        } else {
            new_state = thresh_state;
            reason = "AI_HIGH_DIVERGE";
        }
    } else if (ai_ready && (ai_prob >= AI_CONFIDENCE_MEDIUM || ai_prob <= (1.0f - AI_CONFIDENCE_MEDIUM))) {
        uint8_t ai_state = (ai_prob >= 0.5f) ? PARK_STATE_OCCUPIED : PARK_STATE_FREE;
        if (ai_state == thresh_state) {
            new_state = ai_state;
            reason = "AI_MED_MATCH";
        } else {
            new_state = thresh_state;
            reason = "THRESH_DIVERGE";
        }
    } else {
        new_state = thresh_state;
        reason = ai_ready ? "THRESH_LOWCONF" : "THRESH_WARMUP";
    }

    // 4. Debounce
    if (new_state != ParkData.park_state) {
        debounce_cnt++;
        if (debounce_cnt >= PARK_DEBOUNCE_COUNT) {
            ParkData.park_state = new_state;
            debounce_cnt = 0;
        }
    } else {
        debounce_cnt = 0;
    }

    // 5. Debug output
    printf("[AI] prob=%.3f thresh=%s final=%s reason=%s\r\n",
           ai_prob,
           thresh_state == PARK_STATE_OCCUPIED ? "OCC" : "FREE",
           ParkData.park_state == PARK_STATE_OCCUPIED ? "OCC" : "FREE",
           reason);
}
