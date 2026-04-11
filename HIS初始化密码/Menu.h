#pragma once
#include "Typedef.h"
#include "Init.h"
#include "Tool.h"

// 患者功能菜单（全功能：挂号/查记录/住院等）
void PatientMenu(Loginf* user);
// 医生功能菜单（全功能：写病例/开药/分配床位等）
void DoctorMenu(Loginf* user);
// 管理员功能菜单（全功能：查药品/改库存/检索等）
void AdminMenu(Loginf* user);
// 退出菜单确认
int ExitConfirm(void);

// 患者子功能
void PatientAppointReg(Loginf* user);  // 7天预约挂号
void PatientTodayReg(Loginf* user);   // 当日挂号
void PatientCheckReg(Loginf* user);   // 查看已挂号
void PatientCheckBill(Loginf* user);  // 查看账单
void PatientCheckInfo(Loginf* user);  // 查看个人信息
void PatientCheckCase(Loginf* user);  // 查看病情/病例
void PatientHospital(Loginf* user);   // 住院业务

// 医生子功能
void DoctorCheckInfo(Loginf* user);   // 查看个人信息
void DoctorCheckPatient(Loginf* user);// 查看病人列表
void DoctorFindPatient(Loginf* user); // 查询病人信息
void DoctorWriteCase(Loginf* user);   // 写/修改病例
void DoctorAssignBed(Loginf* user);   // 床位分配/释放

// 管理员子功能
void AdminCheckMed(Loginf* user);     // 查看药品列表
void AdminEditStock(Loginf* user);    // 修改药品库存
void AdminSearchMed(Loginf* user);    // 药品检索（名称/别名）
void AdminCheckStock(Loginf* user);   // 查看库存预警（库存<20）