/**
 * @file unity_config.h
 * @brief Unity 测试框架配置和必要的类型定义
 *
 * 这是为嵌入式 C 代码单元测试创建的头文件 stub
 * 用于模拟目标硬件环境
 */

#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include <float.h>

/* 定义 volatile 宏 (某些编译器可能没有定义) */
#ifndef volatile
#define volatile
#endif

/* 模拟 STM32 的 CAN 数据类型 */
typedef struct {
    uint32_t StdId;
    uint32_t ExtId;
    uint8_t IDE;
    uint8_t RTR;
    uint8_t DLC;
} CAN_RxHeaderTypeDef;

/* 模拟 PIDType 结构体 */
typedef struct {
    float SetVal;
    float CurVal;
    float KP, KI, KD;
    float err[3];
    float SumError;
    float output;
    uint8_t mode;
} PIDType;

/* 模拟 DJmotorPID 结构体 */
typedef struct {
    float pos_Kp, pos_Ki, pos_Kd;
    float vel_Kp, vel_Ki, vel_Kd;
} DJmotorPID;

/* 模拟 DJmotorVal 结构体 */
typedef struct {
    volatile int16_t current_raw;
    volatile float angle_deg;
    volatile int16_t speed_rpm;
    volatile float current_A;
    volatile int16_t PulseRead;
    volatile int16_t PulseGap;
    volatile int32_t PulseTotal;
    volatile int8_t temperature_C;
} DJmotorVal;

/* 模拟 DJmotorParam 结构体 */
typedef struct {
    uint16_t PulsePerRound;
    float Gear_ratio;
    float Reduction_ratio;
    uint32_t ParamID;
    int16_t CurrentLimit_raw;
} DJmotorParam;

/* 模拟 DJmotorLimit 结构体 */
typedef struct {
    bool RPMLimitFlag;
    bool PosAngleLimitFlag;
    bool PosRPMFlag;
    bool CurrentLimitFlag;
    float MaxAngle_deg;
    float MinAngle_deg;
    int16_t SpeedRPMLimit;
    int32_t PosRPMLimit;
    int16_t ZeroRPMLimit;
    int16_t ZeroCurrentLimit_raw;
    bool IsLooseStuck;
} DJmotorLimit;

/* 模拟 DJmotorStatus 结构体 */
typedef struct {
    volatile bool IsSetZero;
    volatile bool ZeroFlag;
    volatile bool StuckFlag;
    volatile bool Overtimeflag;
} DJmotorStatus;

/* 模拟 DJmotorArgum 结构体 */
typedef struct {
    int32_t pulseLock;
    uint32_t zeroCnt;
    uint32_t GapCnt;
} DJmotorArgum;

/* 模拟 DJmotorError 结构体 */
typedef struct {
    volatile uint32_t lastRxTime;
    volatile uint32_t stuckCount;
    volatile uint32_t timeoutCount;
} DJmotorError;

/* 模拟 DJMotor 结构体 */
typedef struct {
    uint8_t ID;
    volatile bool Begin;
    volatile uint8_t MODE_Set;
    volatile uint8_t MODE_Cur;
    DJmotorParam param;
    DJmotorVal valSet;
    DJmotorVal valNow;
    DJmotorVal valPre;
    DJmotorStatus statusFlag;
    DJmotorLimit limit;
    DJmotorArgum argum;
    DJmotorError error;
    PIDType posPID;
    PIDType velPID;
} DJMotor, *DJMotorPointer;

/* 电机模式枚举 */
typedef enum {
    DJ_Disable = 0,
    DJ_RPM = 1,
    DJ_Position = 2,
    DJ_Zero = 3,
    DJ_Current = 4,
} DJmotor_mode_t;

/* PID 函数声明 */
void PID_Init(PIDType *pid, float kp, float ki, float kd, uint8_t mode);

#endif /* UNITY_CONFIG_H */
