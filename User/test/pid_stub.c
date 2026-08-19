/**
 * @file pid_stub.c
 * @brief PID 相关函数的 stub 实现
 */

#include "unity_config.h"

void PID_Init(PIDType *pid, float kp, float ki, float kd, uint8_t mode)
{
    if (pid == NULL) {
        return;
    }

    pid->KP = kp;
    pid->KI = ki;
    pid->KD = kd;
    pid->mode = mode;
    pid->SetVal = 0.0f;
    pid->CurVal = 0.0f;
    pid->SumError = 0.0f;
    pid->output = 0.0f;

    for (int i = 0; i < 3; i++) {
        pid->err[i] = 0.0f;
    }
}

void PID_Reset(PIDType *pid)
{
    if (pid == NULL) {
        return;
    }

    pid->SetVal = 0.0f;
    pid->CurVal = 0.0f;
    pid->SumError = 0.0f;
    pid->output = 0.0f;

    for (int i = 0; i < 3; i++) {
        pid->err[i] = 0.0f;
    }
}

float PID_Caculate(PIDType *pid)
{
    if (pid == NULL) {
        return 0.0f;
    }

    pid->err[2] = pid->err[1];
    pid->err[1] = pid->err[0];
    pid->err[0] = pid->SetVal - pid->CurVal;

    float output = pid->KP * (pid->err[0] - pid->err[1])
                 + pid->KI * pid->err[0]
                 + pid->KD * (pid->err[0] - 2.0f * pid->err[1] + pid->err[2]);

    pid->output = output;
    return output;
}
