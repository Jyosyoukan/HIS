/*
数据类型全在这里定义，方便修改和维护
任何文件都应该#include这个头文件
*/
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define MAX_PATIENTS 105 //最大病人数
#define NAME_LENGTH 25   //名字长度
#define ID_LENGTH 25     //ID长度
#define PASSWORD_LENGTH  10 //最长密码长度
#define MAX_DOCTORS 50  //最大医生数
#define MAX_DEPARTMENTS 5 //最大科室数
#define MAX_APPOINTMENTS 200 //最大预约数
#define MAX_RECORDS 200 //最大病历数
#define MAX_MEDICINE 200 //最大处方数
#define MAX_MEDICINE_NAME 50 //药品名字长度
#define TOTAL_BEDS 100 //总床位数
#define MAX_DEPT 5 // 科室数量
#define MAX_DEPT_NAME_LEN 20 // 科室名称长度

typedef struct Date {
    int year;
    int month;
    int day;
}Date;
typedef struct Time {
    int hour;
    int minute;
    int second;
}Time;






// 科室结构体
typedef struct {
    int id;
    char name[MAX_DEPT_NAME_LEN];
    int total_beds;       // 该科室总床位数
    int available_beds;   // 该科室可用床位数
} Department;

// 医生结构体
typedef struct {
    char id_card[ID_LENGTH]; // 身份证号
    char name[NAME_LENGTH];
    char password[PASSWORD_LENGTH];
    int dept_id;         // 所属科室ID
    char specialty[NAME_LENGTH]; // 专业 （改为使用已定义的 NAME_LENGTH）
    char title[20];      // 职称
    int role;            // 1:病人, 2:医生, 3:管理员
} Doctor;

// 患者（病人）结构体
typedef struct patient {
    char id_card[ID_LENGTH]; // 身份证号
    char name[NAME_LENGTH];
    char password[PASSWORD_LENGTH];
    char gender[5];      // 性别
    // char birthday[10]; // 可选
    int role;            // 1:病人, 2:医生, 3:管理员

    // 挂号信息（可以动态分配或链接）
    // int appointment_count; // 记录预约了多少次
    // Appointment appointments[MAX_APPOINTMENTS_PER_PATIENT]; // 简化起见，可能用链表或单次挂号记录

    // 住院信息
    int is_hospitalized; // 0:否, 1:是
    int bed_id;          // 住院床位ID (对应全局床位数组的索引)
    char admission_date[20]; // 入院日期
    char discharge_date[20]; // 出院日期

    // 病历信息（可以链接到单独的病历结构体）
    // int medical_record_id; 
	struct patient* next; // 链表指针
} Patient;

typedef struct Admin {
    char id_card[ID_LENGTH];
}Admin;
// 病历记录结构体
typedef struct {
    char patient_id_card[ID_LENGTH]; // 关联的病人身份证号（改为使用 ID_LENGTH）
    char doctor_id_card[ID_LENGTH];  // 开具病历的医生身份证号（改为使用 ID_LENGTH）
    char diagnosis_date[20];          // 诊断日期
    char symptoms[200];               // 症状描述
    char prescription[500];           // 医生开具的药品（文本描述）
    // int prescription_count; // 药品数量
    // Medicine prescribed_medicines[MAX_MEDICINES_PER_PRESCRIPTION]; // 简化起见，文本描述
    char treatment_plan[300];         // 治疗方案/检查建议
    char admission_start_date[20];    // 住院开始日期
    char admission_end_date[20];      // 住院结束日期
} MedicalRecord;

// 药品结构体
typedef struct {
    char id[ID_LENGTH];
    char name[NAME_LENGTH];
    char alias[NAME_LENGTH]; // 别名/NAME_LENGTH
    float price;
    int stock;              // 库存
    int is_rx;              // 1:处方药, 0:非处方药
    int bound_dept_id;      // 绑定的科室ID (-1 表示不绑定)
    // char production_date[20]; // 生产日期（题目中提到，但常用作过期日期）
    char expiry_date[20];   // 过期日期
} Medicine;

// 床位结构体
typedef struct {
    int id;                 // 床位唯一ID
    int dept_id;            // 所属科室ID
    int room_id;            // 房间号（同一科室下）
    char patient_id_card[ID_LENGTH]; // 占用床位的病人身份证号 (空字符串表示空闲)
    char admission_date[20];          // 入院日期
    char discharge_date[20];          // 出院日期
} Bed;

// 挂号记录结构体 (简化处理，可以存储在Patient结构体或单独的文件中)
typedef struct {
    char patient_id_card[ID_LENGTH];
    char doctor_id_card[ID_LENGTH];
    int dept_id;
    char appointment_day[10]; // 周几
    char appointment_time[20]; // 具体时间
    char registration_date[20]; // 挂号日期
    float fee;
    int status; // 0:待就诊, 1:已就诊, 2:已取消
} Registration;



Patient* checkbox(Patient* root);