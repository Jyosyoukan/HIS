#pragma once
#include "Typedef.h"
#include "Init.h"
#include "Tool.h"

void PatientMenu(Loginf* user);
void DoctorMenu(Loginf* user);
void AdminMenu(Loginf* user);
int ExitConfirm(void);

// 患者子功能
void PatientAppointReg(Loginf* user);
void PatientTodayReg(Loginf* user);
void PatientCheckReg(Loginf* user);
void PatientCheckBill(Loginf* user);
void PatientCheckInfo(Loginf* user);
void PatientCheckCase(Loginf* user);
void PatientHospital(Loginf* user);

// 医生子功能
void DoctorCheckInfo(Loginf* user);
void DoctorCheckPatient(Loginf* user);
void DoctorFindPatient(Loginf* user);
void DoctorWriteCase(Loginf* user);
void DoctorAssignBed(Loginf* user);

// 管理员子功能
void AdminCheckMed(Loginf* user);
void AdminEditStock(Loginf* user);
void AdminSearchMed(Loginf* user);
void AdminCheckStock(Loginf* user);
void AdminDeleteDoctor(Loginf* user);
void AdminViewAllReg(Loginf* user);
void AdminFinanceReport(Loginf* user);
// 关于本院

void About(void);