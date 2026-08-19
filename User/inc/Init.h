#ifndef INIT
#define INIT

#include "main.h"
#include "pid.h"
#include <stdbool.h> 


#define USE_DJ 1
#define USE_DJNUM 4 
#define M2006_NUM 4
#define M3508_NUM 0
#define M2006_RATIO (36.0f / 1.0f) 
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


typedef enum
{
    DJ_Disable = 0,  //关
    DJ_RPM = 1,      //速度模式
    DJ_Position = 2, //位置模式
    DJ_Zero = 3,     //寻零模式
    DJ_Current = 4,  //电流/扭矩模式
} DJmotor_mode_t;


typedef struct
{
    volatile int16_t current_raw;  // 直接设置电流
    volatile float angle_deg;      // 输出角度,degree
    volatile int16_t speed_rpm;    // valSet:输出轴 rpm;valNow:转子 rpm(原始反馈)
    volatile float current_A;      // 反馈电流,A
    volatile int16_t PulseRead;    // raw encoder pulse
    volatile int16_t PulseGap;     // pulse delta
    volatile int32_t PulseTotal;   // accumulated pulse
    volatile int8_t temperature_C; // ℃
} DJmotorVal;

typedef struct
{
    uint16_t PulsePerRound;   //  8191
    float Gear_ratio;         // mechanism ratio
    float Reduction_ratio;    // motor reducer ratio
    uint32_t ParamID;         // CAN receive ID base
    int16_t CurrentLimit_raw; // output current limit, raw
} DJmotorParam;

typedef struct
{
    bool RPMLimitFlag;
    bool PosAngleLimitFlag;
    bool PosRPMFlag;
    bool CurrentLimitFlag;
    float MaxAngle_deg;           // degree
    float MinAngle_deg;           // degree
    int16_t SpeedRPMLimit;        // rpm
    int32_t PosRPMLimit;          // rpm
    int16_t ZeroRPMLimit;         // rpm
    int16_t ZeroCurrentLimit_raw; // raw
    bool IsLooseStuck;
} DJmotorLimit;


//电机状态
typedef struct {
    volatile bool IsSetZero;    
    volatile bool ZeroFlag;      
    volatile bool StuckFlag;    
    volatile bool Overtimeflag;
} DJmotorStatus;

// 寻零计数器 
typedef struct {
    int32_t pulseLock;  
    uint32_t zeroCnt;    
    uint32_t GapCnt;     
} DJmotorArgum;

//故障检测计数器 
typedef struct {
    volatile uint32_t lastRxTime;  
    volatile uint32_t stuckCount;    
    volatile uint32_t timeoutCount;
} DJmotorError;
typedef struct
{
    uint8_t ID;
    volatile bool Begin;              // true 运行 MODE;false 失能
    volatile DJmotor_mode_t MODE_Set; // DJ_Disable 即失能(发0电流)
    volatile DJmotor_mode_t MODE_Cur; // 实际运行模式,任务层可读

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



#if USE_DJ
extern DJMotor DJmotor[USE_DJNUM];

void DJmotor_Init(void);
void DJmotor_Func(void);
void DJmotor_Receive(CAN_RxHeaderTypeDef Rxheader, uint8_t *Rx_data);
void DJmotor_PID_Reload(DJMotorPointer motor, DJmotorPID pid_reload);

 
#endif


void DJmotor_Init(void);
void EncodeS16Data(const int16_t *src, uint8_t *dst);
void DJmotor_SetZero(DJMotorPointer motor);
void start_test(uint8_t x);



#endif
