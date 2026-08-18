#ifndef PID
#define PID 

#include "Init.h"

float PID_Caculate(PIDType *pid);


typedef struct {
    float SetVal;       
    float CurVal;       
    float KP, KI, KD;    //系数
    float Err[3];       
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


