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
#define NAME_LENGTH 20
#define MAX_INFO 150
#define MAX_MED_ALIAS 20
#define DUTY_DAYS 7
#define MED_PER_DEPT 6

#define MAX_PATIENTS 105
#define MAX_DOCTORS 30
#define MAX_DEPARTMENTS 5
#define MAX_BEDS 50
#define MAX_MEDICINE 30
#define MAX_REG 200
#define MAX_CASE 200
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

typedef struct {
    DeptType deptId;
    int weekDay;
    char doctorId[ID_LENGTH];
} DoctorDuty;

typedef struct Loginf {
    char idcard[ID_LENGTH];
    char password[PASSWORD_LENGTH];
    int role;
    struct Loginf* next;
} Loginf;

typedef struct {
    DeptType deptId;
    char deptName[NAME_LENGTH];
    int totalBed;
    int freeBed;
} Department;

typedef struct {
    int bedId;
    DeptType deptId;
    int isFree;
    char patientId[ID_LENGTH];
    Date inDate;
    
} Bed;

typedef struct {
    int medId;
    char medName[NAME_LENGTH];
    char medAlias[MAX_MED_ALIAS];
    MedType medType;
    DeptType bindDept;
    float price;
    int stock;
    Date prodDate;
} Medicine;

typedef struct {
    char idcard[ID_LENGTH];
    char name[NAME_LENGTH];
    DeptType deptId;
    char title[NAME_LENGTH];
    char info[MAX_INFO];
    char specialty[NAME_LENGTH];
} Doctor;

typedef struct {
    char idcard[ID_LENGTH];
    char name[NAME_LENGTH];
    char gender;
    int isHospitalized;
    int bedId;
    Date inDate;
    Date outDate;
    char medicalRecord[MAX_INFO];
} Patient;

typedef struct {
    int regId;
    char patientId[ID_LENGTH];
    DeptType deptId;
    char doctorId[ID_LENGTH];
    int weekDay;
    float fee;
    int isToday;
    Date regDate;
    Date appointDate;
} RegisterRecord;

typedef struct {
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
} Case;

extern Loginf* userroot;
extern Doctor doctors[MAX_DOCTORS];
extern Patient patients[MAX_PATIENTS];
extern Department depts[MAX_DEPARTMENTS];
extern Bed beds[MAX_BEDS];
extern Medicine meds[MAX_MEDICINE];
extern RegisterRecord regs[MAX_REG];
extern Case cases[MAX_CASE];
extern DoctorDuty docDuty[MAX_DEPARTMENTS * DUTY_DAYS];

extern int docCnt, patCnt, regCnt, caseCnt, medCnt, dutyCnt;
extern const char ADMIN_ID[];
extern const char ADMIN_PWD[];
extern const int ADMIN_ROLE;
extern const float FEE_TODAY;
extern const float FEE_APPOINT;