/**
 * @file Init_test.h
 * @brief Init.h 的测试版本，用于单元测试环境
 *
 * 此文件在测试环境中替代原始的 Init.h
 * 定义了 DJMotor 相关的类型和函数
 */

#ifndef INIT_TEST_H
#define INIT_TEST_H

#include "unity_config.h"
#include <stdbool.h>

/* 宏定义 - 与原始 Init.h 保持一致 */
#define USE_DJ 1
#define USE_DJNUM 1
#define M2006_NUM 1
#define M3508_NUM 0
#define M2006_RATIO (36.0f / 19.0f)
#define M3508_RATIO  (3591.0f / 187.0f)
#define Zero_Distance  5
#define PIDPOS 0
#define PIDINC 1

#define ABS(x) ((x) > 0 ? (x) : -(x))
#define Clamp(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#define ClampPeak(val, peak)   Clamp((val), -(peak), (peak))
#define GetSign(x) ((x) > 0 ? 1 : ((x) < 0 ? -1 : 0))
#define ChangeDataByte(a, b)   do { \
    uint8_t _t = *(a);  *(a) = *(b);  *(b) = _t; \
} while(0)

/* DJMotor 全局变量声明 */
extern DJMotor DJmotor[USE_DJNUM];

/* 函数声明 */
void DJmotor_Init(void);
void DJmotor_Func(void);
void DJmotor_Receive(CAN_RxHeaderTypeDef Rxheader, uint8_t *Rx_data);
void DJmotor_PID_Reload(DJMotorPointer motor, DJmotorPID pid_reload);
void EncodeS16Data(const int16_t *src, uint8_t *dst);
void DJmotor_SetZero(DJMotorPointer motor);

#endif /* INIT_TEST_H */
