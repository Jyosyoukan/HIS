#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <ctype.h>

#define ID_LENGTH 20
#define PASSWORD_LENGTH 16
#define NAME_LENGTH 200
#define MAX_INFO 150
#define MAX_MED_ALIAS 20
#define DUTY_DAYS 7
#define MED_PER_DEPT 6
#define BED_FEE_DOUBLE 80.0f    
#define BED_FEE_SINGLE 150.0f   
#define BED_FEE_ICU 360.0f      
#define MAX_PATIENTS 105
#define MAX_DOCTORS 30
#define MAX_DEPARTMENTS 5
#define MAX_BEDS 45
#define MAX_MEDICINE 30
#define MAX_REG 200
#define MAX_CASE 200
#define MAX_STOCK 500
#define MAX_EXAM 100
#define CHACKCODE "HIS"

typedef enum {
    NEIKE = 1,
    WAIKE,
    FUCHANKE,
    ERKE,
    GUKE
} DeptType;

typedef enum {
    PRESCRIPTION = 1,
    OTC = 2
} MedType;

typedef struct {
    int year;
    int month;
    int day;
} Date;

/* ========== 【修改】所有数据结构添加 next 指针，改为链表节点 ========== */

// 登录信息链表节点
typedef struct Loginf {
    char idcard[ID_LENGTH];
    char password[PASSWORD_LENGTH];
    int role;
    struct Loginf* next;
} Loginf;

// 科室信息
typedef struct {
    DeptType deptId;
    char deptName[NAME_LENGTH];
    int totalBed;
    int freeBed;
} Department;

// 床位链表节点
typedef struct BedNode {
    int bedId;
    DeptType deptId;
    int isFree;
    char patientId[ID_LENGTH];
    Date inDate;
    int roomType;   // 0:双人间, 1:单人间, 2:ICU
    int roomId;     // 病房号
    int bedIndex;   // 床号
    struct BedNode* next;
} BedNode;

// 药品链表节点
typedef struct MedicineNode {
    int medId;
    char medName[NAME_LENGTH];
    char medAlias[MAX_MED_ALIAS];
    MedType medType;
    DeptType bindDept;
    float price;
    int stock;
    Date prodDate;
    struct MedicineNode* next;
} MedicineNode;

// 医生链表节点
typedef struct DoctorNode {
    char idcard[ID_LENGTH];
    char name[NAME_LENGTH];
    DeptType deptId;
    char title[NAME_LENGTH];
    char info[MAX_INFO];
    char specialty[NAME_LENGTH];
    struct DoctorNode* next;
} DoctorNode;

// 患者链表节点
typedef struct PatientNode {
    char idcard[ID_LENGTH];
    char name[NAME_LENGTH];
    char gender;
    int isHospitalized;
    int bedId;
    Date inDate;
    Date outDate;
    char medicalRecord[MAX_INFO];
    struct PatientNode* next;
} PatientNode;

// 挂号记录链表节点
typedef struct RegNode {
    int regId;
    char patientId[ID_LENGTH];
    DeptType deptId;
    char doctorId[ID_LENGTH];
    int weekDay;
    float fee;
    int isToday;
    Date regDate;
    Date appointDate;
    struct RegNode* next;
} RegNode;

// 检查记录链表节点
typedef struct ExamNode {
    int examId;
    char patientId[ID_LENGTH];
    char doctorId[ID_LENGTH];
    char examType[NAME_LENGTH];
    float cost;
    Date examDate;
    int status; // 0=已开具(待完成) 1=已完成
    char result[MAX_INFO];
    struct ExamNode* next;
} ExamNode;

// 病例链表节点
typedef struct CaseNode {
    int caseId;
    char patientId[ID_LENGTH];
    char doctorId[ID_LENGTH];
    char illness[MAX_INFO];
    int medId[5];
    int medNum[5];
    int bedId;
    Date diagDate;
    Date inDate;
    Date outDate;
    struct CaseNode* next;
} CaseNode;

// 医生值班链表节点
typedef struct DutyNode {
    DeptType deptId;
    int weekDay;
    char doctorId[ID_LENGTH];
    struct DutyNode* next;
} DutyNode;

/* ========== 【修改】全局变量：数组 → 链表头指针 ========== */
extern Loginf* userroot;            // 登录链表头
extern DoctorNode* doctorHead;  // 医生链表头
extern PatientNode* patientHead;        // 患者链表头
extern Department depts[MAX_DEPARTMENTS]; // 科室保留数组
extern BedNode* bedHead;            // 床位链表头
extern MedicineNode* medicineHead;      // 药品链表头
extern RegNode* regHead;        // 挂号记录链表头
extern CaseNode* caseHead;          // 病例链表头
extern DutyNode* dutyHead;    // 值班链表头
extern ExamNode* examHead;  // 检查记录链表头

// 计数器（可选，方便获取总数）
extern int docCnt, patCnt, regCnt, caseCnt, medCnt, dutyCnt, examCnt;

extern const char ADMIN_ID[];  // 管理员账号（身份证号）
extern const char ADMIN_PWD[];  //管理员密码
extern const int ADMIN_ROLE;   //管理员角色值
extern const float FEE_TODAY;     // 当日挂号费
extern const float FEE_APPOINT;    // 预约挂号费