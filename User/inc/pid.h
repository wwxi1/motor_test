#ifndef PID
#define PID 

#include <stdint.h>

typedef struct {
    float SetVal;       
    float CurVal;       
    float KP, KI, KD;    //系数
    float err[3]; 
    float SumError;      
    float output;        
    uint8_t mode;    
} PIDType;

typedef struct {
    float pos_Kp, pos_Ki, pos_Kd;
    float vel_Kp, vel_Ki, vel_Kd;
} DJmotorPID;



float PID_Caculate(PIDType *pid);
void  PID_Init(PIDType *pid, float kp, float ki, float kd, uint8_t mode);
void  PID_Reset(PIDType *pid);




#endif


