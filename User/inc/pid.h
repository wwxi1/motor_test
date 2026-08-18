#ifndef PID
#define PID 

#include "Init.h"


typedef struct {
    float SetVal;       
    float CurVal;       
    float Kp, Ki, Kd;    //系数
    float Error;        
    float LastError;    //上次误差 e(k-1) 
    float PrevError;    // 上上次误差 e(k-2)
    float SumError;    
    float Out;        
    PIDMode_t Mode;    
} PIDType;

typedef struct {
    float pos_Kp, pos_Ki, pos_Kd;
    float vel_Kp, vel_Ki, vel_Kd;
} DJmotorPID;
float PID_Caculate(PIDType *pid);



#endif


