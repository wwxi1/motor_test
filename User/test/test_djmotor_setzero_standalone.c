/**
 * @file test_djmotor_setzero_standalone.c
 * @brief DJmotor_SetZero 函数的独立单元测试（内联 Unity 框架）
 *
 * 此文件包含了完整的测试代码，无需外部依赖即可编译运行
 * 使用方法: gcc test_djmotor_setzero_standalone.c -o test -lm && ./test
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

/* ================================================================
 * 内联 Unity 测试框架
 * ================================================================ */
#define UNITY_VERSION "2.5.0"
#define UNITY_FLOAT_PRECISION 0.001f
#define UNITY_EXCLUDE_SETJMP

typedef struct {
    unsigned int test_count;
    unsigned int failures;
    const char *current_test;
    const char *current_file;
    unsigned int current_line;
} Unity_t;

static Unity_t Unity;

#define TEST_FAIL_MESSAGE(msg) do { \
    Unity.failures++; \
    printf("FAIL: %s (%s:%u)\n", msg, __FILE__, (unsigned int)__LINE__); \
} while(0)

#define TEST_FAIL_EMPTY() TEST_FAIL_MESSAGE("FAIL")

#define TEST_ASSERT(condition) do { \
    if (!(condition)) { \
        TEST_FAIL_MESSAGE("Expression was false: " #condition); \
    } \
} while(0)

#define TEST_ASSERT_TRUE(condition) TEST_ASSERT(condition)
#define TEST_ASSERT_FALSE(condition) TEST_ASSERT(!(condition))

#define TEST_ASSERT_EQUAL(expected, actual) do { \
    if ((expected) != (actual)) { \
        Unity.failures++; \
        printf("FAIL: Expected %d, Actual %d (%s:%u)\n", \
               (int)(expected), (int)(actual), __FILE__, (unsigned int)__LINE__); \
    } \
} while(0)

#define TEST_ASSERT_EQUAL_INT(expected, actual) TEST_ASSERT_EQUAL(expected, actual)
#define TEST_ASSERT_EQUAL_INT32(expected, actual) TEST_ASSERT_EQUAL(expected, actual)
#define TEST_ASSERT_EQUAL_INT16(expected, actual) TEST_ASSERT_EQUAL(expected, actual)
#define TEST_ASSERT_EQUAL_INT8(expected, actual) TEST_ASSERT_EQUAL(expected, actual)
#define TEST_ASSERT_EQUAL_UINT(expected, actual) TEST_ASSERT_EQUAL(expected, actual)

#define TEST_ASSERT_EQUAL_FLOAT(expected, actual) do { \
    float _exp = (float)(expected); \
    float _act = (float)(actual); \
    float _diff = fabsf(_exp - _act); \
    if (_diff > UNITY_FLOAT_PRECISION) { \
        Unity.failures++; \
        printf("FAIL: Expected %.6f, Actual %.6f (diff %.6f > %.6f) (%s:%u)\n", \
               _exp, _act, _diff, UNITY_FLOAT_PRECISION, __FILE__, (unsigned int)__LINE__); \
    } \
} while(0)

#define TEST_ASSERT_EQUAL_MEMORY(expected, actual, size) do { \
    if (memcmp((expected), (actual), (size)) != 0) { \
        Unity.failures++; \
        printf("FAIL: Memory mismatch at %s:%u\n", __FILE__, (unsigned int)__LINE__); \
    } \
} while(0)

#define TEST_ASSERT_NULL(pointer) do { \
    if ((pointer) != NULL) { \
        Unity.failures++; \
        printf("FAIL: Expected NULL, got %p at %s:%u\n", \
               (void*)(pointer), __FILE__, (unsigned int)__LINE__); \
    } \
} while(0)

#define TEST_ASSERT_NOT_NULL(pointer) do { \
    if ((pointer) == NULL) { \
        Unity.failures++; \
        printf("FAIL: Expected non-NULL at %s:%u\n", __FILE__, (unsigned int)__LINE__); \
    } \
} while(0)

#define TEST_ASSERT_FAIL() TEST_FAIL_EMPTY()

#define TEST_IGNORE_MESSAGE(msg) printf("IGNORE: %s\n", msg)
#define TEST_IGNORE() return

#define TEST_CASE()

#define RUN_TEST(func) do { \
    printf("  %s ... ", #func); \
    Unity.current_test = #func; \
    Unity.current_line = 0; \
    func(); \
    if (Unity.failures == 0 || (Unity.failures > 0 && strstr(#func, "_fail") != NULL)) { \
        if (strstr(#func, "_fail") == NULL) { \
            printf("OK\n"); \
        } else { \
            printf("PASS (expected failure)\n"); \
            Unity.failures--; \
        } \
    } \
    Unity.test_count++; \
} while(0)

void UnityBegin(const char *filename) {
    memset(&Unity, 0, sizeof(Unity));
    printf("\n=============================%s==========================\n", filename);
    printf("Unity Test Framework v%s\n", UNITY_VERSION);
}

int UnityEnd(void) {
    printf("=========================%u tests, %u failures==========================\n\n",
           Unity.test_count, Unity.failures);
    if (Unity.failures > 0) {
        return 1;
    }
    return 0;
}

/* ================================================================
 * 测试目标代码
 * ================================================================ */
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

typedef enum {
    DJ_Disable = 0,
    DJ_RPM = 1,
    DJ_Position = 2,
    DJ_Zero = 3,
    DJ_Current = 4,
} DJmotor_mode_t;

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

typedef struct {
    uint16_t PulsePerRound;
    float Gear_ratio;
    float Reduction_ratio;
    uint32_t ParamID;
    int16_t CurrentLimit_raw;
} DJmotorParam;

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

typedef struct {
    volatile bool IsSetZero;
    volatile bool ZeroFlag;
    volatile bool StuckFlag;
    volatile bool Overtimeflag;
} DJmotorStatus;

typedef struct {
    int32_t pulseLock;
    uint32_t zeroCnt;
    uint32_t GapCnt;
} DJmotorArgum;

typedef struct {
    volatile uint32_t lastRxTime;
    volatile uint32_t stuckCount;
    volatile uint32_t timeoutCount;
} DJmotorError;

typedef struct {
    float SetVal;
    float CurVal;
    float KP, KI, KD;
    float err[3];
    float SumError;
    float output;
    uint8_t mode;
} PIDType;

typedef struct {
    float pos_Kp, pos_Ki, pos_Kd;
    float vel_Kp, vel_Ki, vel_Kd;
} DJmotorPID;

typedef struct {
    uint8_t ID;
    volatile bool Begin;
    volatile DJmotor_mode_t MODE_Set;
    volatile DJmotor_mode_t MODE_Cur;
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

/* 被测函数 */
void DJmotor_SetZero(DJMotorPointer motor)
{
    motor->valNow.PulseTotal = 0;
    motor->valNow.angle_deg = 0.0f;
    motor->valPre = motor->valNow;
}

/* ================================================================
 * 测试夹具
 * ================================================================ */
static DJMotor testMotor;

static void initMotorToKnownState(DJMotor *motor)
{
    memset(motor, 0, sizeof(DJMotor));

    motor->ID = 1;
    motor->Begin = true;
    motor->MODE_Set = DJ_RPM;
    motor->MODE_Cur = DJ_RPM;

    motor->valSet.current_raw = 100;
    motor->valSet.angle_deg = 45.5f;
    motor->valSet.speed_rpm = 500;
    motor->valSet.current_A = 2.5f;
    motor->valSet.PulseRead = 200;
    motor->valSet.PulseGap = 50;
    motor->valSet.PulseTotal = 12345;
    motor->valSet.temperature_C = 30;

    motor->valNow.current_raw = 150;
    motor->valNow.angle_deg = 90.0f;
    motor->valNow.speed_rpm = 800;
    motor->valNow.current_A = 3.5f;
    motor->valNow.PulseRead = 400;
    motor->valNow.PulseGap = 100;
    motor->valNow.PulseTotal = 67890;
    motor->valNow.temperature_C = 40;

    motor->valPre.current_raw = 200;
    motor->valPre.angle_deg = 180.0f;
    motor->valPre.speed_rpm = 1000;
    motor->valPre.current_A = 4.5f;
    motor->valPre.PulseRead = 600;
    motor->valPre.PulseGap = 150;
    motor->valPre.PulseTotal = 11111;
    motor->valPre.temperature_C = 50;
}

/* ================================================================
 * 测试用例
 * ================================================================ */

/**
 * 测试用例 1: 基本功能测试
 * 测试将非零值设置为零
 */
void test_DJmotor_SetZero_BasicFunction(void)
{
    initMotorToKnownState(&testMotor);

    DJmotor_SetZero(&testMotor);

    TEST_ASSERT_EQUAL_INT32(0, testMotor.valNow.PulseTotal);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valPre.PulseTotal);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valPre.angle_deg);

    TEST_ASSERT_EQUAL_INT16(testMotor.valNow.current_raw, testMotor.valPre.current_raw);
    TEST_ASSERT_EQUAL_INT16(testMotor.valNow.speed_rpm, testMotor.valPre.speed_rpm);
    TEST_ASSERT_EQUAL_FLOAT(testMotor.valNow.current_A, testMotor.valPre.current_A);
    TEST_ASSERT_EQUAL_INT16(testMotor.valNow.PulseRead, testMotor.valPre.PulseRead);
    TEST_ASSERT_EQUAL_INT16(testMotor.valNow.PulseGap, testMotor.valPre.PulseGap);
    TEST_ASSERT_EQUAL_INT8(testMotor.valNow.temperature_C, testMotor.valPre.temperature_C);
}

/**
 * 测试用例 2: PulseTotal 为最大值 (INT32_MAX)
 */
void test_DJmotor_SetZero_PulseTotal_MaxValue(void)
{
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = INT32_MAX;
    testMotor.valNow.angle_deg = 100.0f;
    testMotor.valPre.PulseTotal = 0;

    DJmotor_SetZero(&testMotor);

    TEST_ASSERT_EQUAL_INT32(0, testMotor.valNow.PulseTotal);
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valPre.PulseTotal);
}

/**
 * 测试用例 3: PulseTotal 为最小值 (INT32_MIN)
 */
void test_DJmotor_SetZero_PulseTotal_MinValue(void)
{
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = INT32_MIN;
    testMotor.valNow.angle_deg = -100.0f;
    testMotor.valPre.PulseTotal = 100;

    DJmotor_SetZero(&testMotor);

    TEST_ASSERT_EQUAL_INT32(0, testMotor.valNow.PulseTotal);
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valPre.PulseTotal);
}

/**
 * 测试用例 4: PulseTotal 为 -1 (负数边界)
 */
void test_DJmotor_SetZero_PulseTotal_NegativeOne(void)
{
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = -1;
    testMotor.valNow.angle_deg = 0.0f;

    DJmotor_SetZero(&testMotor);

    TEST_ASSERT_EQUAL_INT32(0, testMotor.valNow.PulseTotal);
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valPre.PulseTotal);
}

/**
 * 测试用例 5: angle_deg 为正浮点最大值
 */
void test_DJmotor_SetZero_AngleDeg_PositiveMax(void)
{
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = 99999;
    testMotor.valNow.angle_deg = 1e10f;

    DJmotor_SetZero(&testMotor);

    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valPre.angle_deg);
}

/**
 * 测试用例 6: angle_deg 为负浮点最小值
 */
void test_DJmotor_SetZero_AngleDeg_NegativeMin(void)
{
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = -99999;
    testMotor.valNow.angle_deg = -1e10f;

    DJmotor_SetZero(&testMotor);

    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valPre.angle_deg);
}

/**
 * 测试用例 7: angle_deg 为负小数
 */
void test_DJmotor_SetZero_AngleDeg_NegativeFloat(void)
{
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = 100;
    testMotor.valNow.angle_deg = -180.5f;

    DJmotor_SetZero(&testMotor);

    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valPre.angle_deg);
}

/**
 * 测试用例 8: angle_deg 已经是 0.0f
 */
void test_DJmotor_SetZero_AngleDeg_AlreadyZero(void)
{
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = 100;
    testMotor.valNow.angle_deg = 0.0f;

    DJmotor_SetZero(&testMotor);

    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valPre.angle_deg);
}

/**
 * 测试用例 9: PulseTotal 已经是 0
 */
void test_DJmotor_SetZero_PulseTotal_AlreadyZero(void)
{
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = 0;
    testMotor.valNow.angle_deg = 90.0f;

    DJmotor_SetZero(&testMotor);

    TEST_ASSERT_EQUAL_INT32(0, testMotor.valNow.PulseTotal);
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valPre.PulseTotal);
}

/**
 * 测试用例 10: 验证 valNow 其他字段保持不变
 */
void test_DJmotor_SetZero_OtherFieldsUnchanged(void)
{
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.current_raw = 200;
    testMotor.valNow.angle_deg = 90.0f;
    testMotor.valNow.speed_rpm = 1500;
    testMotor.valNow.current_A = 5.5f;
    testMotor.valNow.PulseRead = 4095;
    testMotor.valNow.PulseGap = 50;
    testMotor.valNow.PulseTotal = 50000;
    testMotor.valNow.temperature_C = 60;

    int16_t orig_current_raw = testMotor.valNow.current_raw;
    int16_t orig_speed_rpm = testMotor.valNow.speed_rpm;
    float orig_current_A = testMotor.valNow.current_A;
    int16_t orig_PulseRead = testMotor.valNow.PulseRead;
    int16_t orig_PulseGap = testMotor.valNow.PulseGap;
    int8_t orig_temperature = testMotor.valNow.temperature_C;

    DJmotor_SetZero(&testMotor);

    TEST_ASSERT_EQUAL_INT16(orig_current_raw, testMotor.valNow.current_raw);
    TEST_ASSERT_EQUAL_INT16(orig_speed_rpm, testMotor.valNow.speed_rpm);
    TEST_ASSERT_EQUAL_FLOAT(orig_current_A, testMotor.valNow.current_A);
    TEST_ASSERT_EQUAL_INT16(orig_PulseRead, testMotor.valNow.PulseRead);
    TEST_ASSERT_EQUAL_INT16(orig_PulseGap, testMotor.valNow.PulseGap);
    TEST_ASSERT_EQUAL_INT8(orig_temperature, testMotor.valNow.temperature_C);

    TEST_ASSERT_EQUAL_INT16(orig_current_raw, testMotor.valPre.current_raw);
    TEST_ASSERT_EQUAL_INT16(orig_speed_rpm, testMotor.valPre.speed_rpm);
    TEST_ASSERT_EQUAL_FLOAT(orig_current_A, testMotor.valPre.current_A);
    TEST_ASSERT_EQUAL_INT16(orig_PulseRead, testMotor.valPre.PulseRead);
    TEST_ASSERT_EQUAL_INT16(orig_PulseGap, testMotor.valPre.PulseGap);
    TEST_ASSERT_EQUAL_INT8(orig_temperature, testMotor.valPre.temperature_C);
}

/**
 * 测试用例 11: 验证 valPre 完整复制
 */
void test_DJmotor_SetZero_ValPreCopyComplete(void)
{
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.current_raw = 999;
    testMotor.valNow.angle_deg = 360.0f;
    testMotor.valNow.speed_rpm = 8192;
    testMotor.valNow.current_A = 10.123f;
    testMotor.valNow.PulseRead = 8191;
    testMotor.valNow.PulseGap = 1;
    testMotor.valNow.PulseTotal = 123456;
    testMotor.valNow.temperature_C = 85;

    DJmotor_SetZero(&testMotor);

    TEST_ASSERT_EQUAL_INT16(testMotor.valNow.current_raw, testMotor.valPre.current_raw);
    TEST_ASSERT_EQUAL_FLOAT(testMotor.valNow.angle_deg, testMotor.valPre.angle_deg);
    TEST_ASSERT_EQUAL_INT16(testMotor.valNow.speed_rpm, testMotor.valPre.speed_rpm);
    TEST_ASSERT_EQUAL_FLOAT(testMotor.valNow.current_A, testMotor.valPre.current_A);
    TEST_ASSERT_EQUAL_INT16(testMotor.valNow.PulseRead, testMotor.valPre.PulseRead);
    TEST_ASSERT_EQUAL_INT16(testMotor.valNow.PulseGap, testMotor.valPre.PulseGap);
    TEST_ASSERT_EQUAL_INT32(testMotor.valNow.PulseTotal, testMotor.valPre.PulseTotal);
    TEST_ASSERT_EQUAL_INT8(testMotor.valNow.temperature_C, testMotor.valPre.temperature_C);
}

/**
 * 测试用例 12: 连续调用两次
 */
void test_DJmotor_SetZero_ConsecutiveCalls(void)
{
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = 100;
    testMotor.valNow.angle_deg = 45.0f;

    DJmotor_SetZero(&testMotor);
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valNow.PulseTotal);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);

    DJmotor_SetZero(&testMotor);
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valNow.PulseTotal);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valPre.PulseTotal);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valPre.angle_deg);
}

/**
 * 测试用例 13: angle_deg 为正无穷大
 */
void test_DJmotor_SetZero_AngleDeg_Infinity(void)
{
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = 100;
    testMotor.valNow.angle_deg = INFINITY;

    DJmotor_SetZero(&testMotor);

    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valPre.angle_deg);
}

/**
 * 测试用例 14: angle_deg 为负无穷大
 */
void test_DJmotor_SetZero_AngleDeg_NegativeInfinity(void)
{
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = 100;
    testMotor.valNow.angle_deg = -INFINITY;

    DJmotor_SetZero(&testMotor);

    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valPre.angle_deg);
}

/**
 * 测试用例 15: PulseTotal 接近最大值
 */
void test_DJmotor_SetZero_PulseTotal_CloseToMax(void)
{
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = INT32_MAX - 1;
    testMotor.valNow.angle_deg = 270.0f;

    DJmotor_SetZero(&testMotor);

    TEST_ASSERT_EQUAL_INT32(0, testMotor.valNow.PulseTotal);
    TEST_ASSERT_EQUAL_INT32(0, testMotor.valPre.PulseTotal);
}

/**
 * 测试用例 16: 极小正浮点值
 */
void test_DJmotor_SetZero_AngleDeg_VerySmallPositive(void)
{
    memset(&testMotor, 0, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = 1;
    testMotor.valNow.angle_deg = 1e-30f;

    DJmotor_SetZero(&testMotor);

    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valPre.angle_deg);
}

/**
 * 测试用例 17: 结构体内存布局验证
 */
void test_DJmotor_SetZero_StructMemoryLayout(void)
{
    memset(&testMotor, 0xAA, sizeof(DJMotor));
    testMotor.valNow.PulseTotal = 123456;
    testMotor.valNow.angle_deg = 90.0f;
    testMotor.valPre.PulseTotal = 0;
    testMotor.valPre.angle_deg = 0.0f;

    DJmotor_SetZero(&testMotor);

    TEST_ASSERT_EQUAL_INT32(0, testMotor.valNow.PulseTotal);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, testMotor.valNow.angle_deg);
}

/* ================================================================
 * 主函数
 * ================================================================ */
int main(void)
{
    UnityBegin("DJmotor_SetZero Unit Tests");

    printf("\n--- Basic Function Tests ---\n");
    RUN_TEST(test_DJmotor_SetZero_BasicFunction);

    printf("\n--- Boundary Value Tests (PulseTotal) ---\n");
    RUN_TEST(test_DJmotor_SetZero_PulseTotal_MaxValue);
    RUN_TEST(test_DJmotor_SetZero_PulseTotal_MinValue);
    RUN_TEST(test_DJmotor_SetZero_PulseTotal_NegativeOne);
    RUN_TEST(test_DJmotor_SetZero_PulseTotal_AlreadyZero);
    RUN_TEST(test_DJmotor_SetZero_PulseTotal_CloseToMax);

    printf("\n--- Boundary Value Tests (angle_deg) ---\n");
    RUN_TEST(test_DJmotor_SetZero_AngleDeg_PositiveMax);
    RUN_TEST(test_DJmotor_SetZero_AngleDeg_NegativeMin);
    RUN_TEST(test_DJmotor_SetZero_AngleDeg_NegativeFloat);
    RUN_TEST(test_DJmotor_SetZero_AngleDeg_AlreadyZero);
    RUN_TEST(test_DJmotor_SetZero_AngleDeg_Infinity);
    RUN_TEST(test_DJmotor_SetZero_AngleDeg_NegativeInfinity);
    RUN_TEST(test_DJmotor_SetZero_AngleDeg_VerySmallPositive);

    printf("\n--- Data Integrity Tests ---\n");
    RUN_TEST(test_DJmotor_SetZero_OtherFieldsUnchanged);
    RUN_TEST(test_DJmotor_SetZero_ValPreCopyComplete);

    printf("\n--- Stress Tests ---\n");
    RUN_TEST(test_DJmotor_SetZero_ConsecutiveCalls);
    RUN_TEST(test_DJmotor_SetZero_StructMemoryLayout);

    return UnityEnd();
}
