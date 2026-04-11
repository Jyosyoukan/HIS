#include "Menu.h"

// 退出菜单确认
int ExitConfirm(void) {
    printf("是否确认退出当前菜单？（Y/N）：");
    char ch = _getch();
    if (ch == 'Y' || ch == 'y') {
        system("cls");
        return 1;
    }
    else {
        system("cls");
        return 0;
    }
}

// ===================== 患者子功能实现 =====================
// 7天预约挂号（强绑值班表+系统时间）
void PatientAppointReg(Loginf* user) {
    system("cls");
    printf("===================== 7天预约挂号 =====================\n");
    Date now = GetCurrentDate();
    int nowWeek = GetWeekDay(now);
    char nowDateStr[20] = { 0 };
    PrintDate(now, nowDateStr);
    printf("当前系统时间：%s（周%d）| 可预约未来7天内医生\n", nowDateStr, nowWeek);
    printf("=======================================================\n");

    int deptSel = 0;
    printf("请选择科室（1-内科 2-外科 3-妇产科 4-儿科 5-骨科）：");
    scanf_s("%d", &deptSel);
    if (deptSel < 1 || deptSel > 5) {
        printf("科室选择错误！\n");
        PauseAndClear();
        return;
    }
    DeptType deptId = (DeptType)deptSel;

    // 列出该科室未来7天的值班安排（含今天）
    printf("该科室未来7天值班安排：\n");
    printf("┌───────────────┬────────────────┬────────────────┬────────────────────────────────────┐\n");
    printf("│     时间      │     姓名       │      职称      │               擅长                 │\n");
    printf("├───────────────┼────────────────┼────────────────┼────────────────────────────────────┤\n");

    for (int off = 0; off < 7; off++) {
        int dayOfWeek = (nowWeek + off - 1) % 7 + 1; // 1..7
        char* dutyDocId = FindDutyDoctor(deptId, dayOfWeek);
        int docIdx = dutyDocId ? FindDoctorById(dutyDocId) : -1;

        // ✅ 修复烫烫烫核心：初始化 + 标准 sprintf_s 三参数
        char timeStr[30] = { 0 };
        if (off == 0) sprintf_s(timeStr, sizeof(timeStr), "周%d(今天)", dayOfWeek);
        else if (off == 1) sprintf_s(timeStr, sizeof(timeStr), "周%d(明天)", dayOfWeek);
        else if (off == 2) sprintf_s(timeStr, sizeof(timeStr), "周%d(后天)", dayOfWeek);
        else sprintf_s(timeStr, sizeof(timeStr), "周%d", dayOfWeek);

        // 表格输出
        if (docIdx >= 0) {
            printf("│ %-13s │ %-14s │ %-14s │ %-34s │\n",
                timeStr,
                doctors[docIdx].name,
                doctors[docIdx].title,
                doctors[docIdx].specialty);
        }
        else {
            printf("│ %-13s │     无安排     │                │                                    │\n", timeStr);
        }
    }

    printf("└───────────────┴────────────────┴────────────────┴────────────────────────────────────┘\n");
    int weekSel = 0;
    printf("请选择预约周几（1-周一 ...7-周日，不可选过去日期）：");
    scanf_s("%d", &weekSel);
    if (weekSel < 1 || weekSel > 7) {
        printf("周几选择错误！\n");
        PauseAndClear();
        return;
    }

    char* docId = FindDutyDoctor(deptId, weekSel);
    if (docId == NULL) {
        printf("该科室该日无值班医生！\n");
        PauseAndClear();
        return;
    }
    int docIdx = FindDoctorById(docId);
    if (docIdx == -1) {
        printf("值班医生信息不存在！\n");
        PauseAndClear();
        return;
    }

    printf("=======================================================\n");
    printf("【%s】周%d值班医生信息\n", depts[deptId - 1].deptName, weekSel);
    printf("姓名：%s | 职称：%s\n", doctors[docIdx].name, doctors[docIdx].title);
    printf("擅长：%s | 简介：%s\n", doctors[docIdx].specialty, doctors[docIdx].info);
    printf("预约挂号费：%.1f元\n", FEE_APPOINT);
    printf("=======================================================\n");

    printf("是否确认预约该医生？（Y/N）：");
    char ch = _getch();
    if (ch != 'Y' && ch != 'y') {
        printf("取消预约！\n");
        PauseAndClear();
        return;
    }

    Date appointDate = now;
    appointDate.day += (weekSel - nowWeek + 7) % 7;
    if (!CheckAppointDate(now, appointDate)) {
        printf("预约日期不合法（超出7天）！\n");
        PauseAndClear();
        return;
    }

    regs[regCnt].regId = regCnt + 1;
    strcpy_s(regs[regCnt].patientId, ID_LENGTH, user->idcard);
    regs[regCnt].deptId = deptId;
    strcpy_s(regs[regCnt].doctorId, ID_LENGTH, docId);
    regs[regCnt].weekDay = weekSel;
    regs[regCnt].fee = FEE_APPOINT;
    regs[regCnt].isToday = 0;
    regs[regCnt].regDate = now;
    regs[regCnt].appointDate = appointDate;
    regCnt++;

    SaveRegToFile();
    char appointDateStr[20] = { 0 };
    PrintDate(appointDate, appointDateStr);
    printf("\n===================== 预约成功！ =====================\n");
    printf("挂号编号：%d | 预约日期：%s\n", regs[regCnt - 1].regId, appointDateStr);
    printf("科室：%s | 医生：%s\n", depts[deptId - 1].deptName, doctors[docIdx].name);
    printf("费用：%.1f元 | 请按时就诊！\n", FEE_APPOINT);
    PauseAndClear();
}

// 当日挂号
void PatientTodayReg(Loginf* user)      {
    system("cls");
    printf("===================== 当日挂号 =====================\n");
    Date now = GetCurrentDate();
    int nowWeek = GetWeekDay(now);
    char nowDateStr[20] = { 0 };
    PrintDate(now, nowDateStr);
    printf("当前系统时间：%s（周%d）| 当日挂号费：%.1f元\n", nowDateStr, nowWeek, FEE_TODAY);
    printf("======================================================\n");

    int deptSel = 0;
    printf("请选择科室（1-内科 2-外科 3-妇产科 4-儿科 5-骨科）：");
    scanf_s("%d", &deptSel);
    if (deptSel < 1 || deptSel>5) {
        printf("科室选择错误！\n");
        PauseAndClear();
        return;
    }
    DeptType deptId = (DeptType)deptSel;

    char* docId = FindDutyDoctor(deptId, nowWeek);
    if (docId == NULL) {
        printf("该科室今日无值班医生！\n");
        PauseAndClear();
        return;
    }
    int docIdx = FindDoctorById(docId);
    if (docIdx == -1) {
        printf("值班医生信息不存在！\n");
        PauseAndClear();
        return;
    }

    printf("======================================================\n");
    printf("【%s】今日值班医生：%s（%s）\n", depts[deptId - 1].deptName, doctors[docIdx].name, doctors[docIdx].title);
    printf("擅长：%s | 挂号费：%.1f元\n", doctors[docIdx].specialty, FEE_TODAY);

    printf("是否确认当日挂号？（Y/N）：");
    char ch = _getch();
    if (ch != 'Y' && ch != 'y') {
        printf("取消挂号！\n");
        PauseAndClear();
        return;
    }

    regs[regCnt].regId = regCnt + 1;
    strcpy_s(regs[regCnt].patientId, ID_LENGTH, user->idcard);
    regs[regCnt].deptId = deptId;
    strcpy_s(regs[regCnt].doctorId, ID_LENGTH, docId);
    regs[regCnt].weekDay = nowWeek;
    regs[regCnt].fee = FEE_TODAY;
    regs[regCnt].isToday = 1;
    regs[regCnt].regDate = now;
    regs[regCnt].appointDate = now;
    regCnt++;
    SaveRegToFile();

    printf("\n===================== 当日挂号成功！ =====================\n");
    printf("挂号编号：%d | 就诊科室：%s\n", regs[regCnt - 1].regId, depts[deptId - 1].deptName);
    printf("就诊医生：%s | 费用：%.1f元\n", doctors[docIdx].name, FEE_TODAY);
    PauseAndClear();
}

// 查看已挂号记录
void PatientCheckReg(Loginf* user) {
    int i = 0, hasReg = 0, docIdx = 0;
    char regType[10] = { 0 };
    char deptName[NAME_LENGTH] = { 0 };
    char docName[NAME_LENGTH] = { 0 };
    char appDateStr[20] = { 0 };
    Date now;

    system("cls");
    now = GetCurrentDate();
    char nowDateStr[20] = { 0 };
    PrintDate(now, nowDateStr);
    printf("当前系统时间：%s | 共查询到%d条挂号记录\n", nowDateStr, regCnt);
    printf("=======================================================\n");
    printf("编号\t类型\t科室\t\t医生\t\t日期\t\t费用\n");
    printf("-------------------------------------------------------\n");

    for (i = 0; i < regCnt; i++) {
        if (strcmp(regs[i].patientId, user->idcard) == 0) {
            hasReg = 1;
            if (regs[i].isToday) {
                strcpy_s(regType, 10, "当日");
            }
            else {
                strcpy_s(regType, 10, "预约");
            }
            strcpy_s(deptName, NAME_LENGTH, depts[regs[i].deptId - 1].deptName);
            docIdx = FindDoctorById(regs[i].doctorId);
            if (docIdx == -1) {
                strcpy_s(docName, NAME_LENGTH, "未知");
            }
            else {
                strcpy_s(docName, NAME_LENGTH, doctors[docIdx].name);
            }
            PrintDate(regs[i].appointDate, appDateStr);
            printf("%d\t%s\t%s\t\t%s\t\t%s\t%.1f\n",
                regs[i].regId, regType, deptName, docName, appDateStr, regs[i].fee);
        }
    }
    if (!hasReg)
        printf("暂无挂号记录！\n");
    printf("=======================================================\n");
    PauseAndClear();
}

// 查看账单（挂号费用汇总）
void PatientCheckBill(Loginf* user) {
    int i = 0, regNum = 0;
    float totalFee = 0.0f;
    char regType[10] = { 0 };
    char appDateStr[20] = { 0 };

    system("cls");
    printf("===================== 我的费用账单 =====================\n");
    for (i = 0; i < regCnt; i++) {
        if (strcmp(regs[i].patientId, user->idcard) == 0) {
            totalFee += regs[i].fee;
            regNum++;
        }
    }
    printf("累计挂号次数：%d次\n", regNum);
    printf("累计消费金额：%.1f元\n", totalFee);
    printf("=======================================================\n");
    printf("费用明细：\n");
    printf("编号\t日期\t\t类型\t费用\n");
    printf("-------------------------------------------------------\n");

    for (i = 0; i < regCnt; i++) {
        if (strcmp(regs[i].patientId, user->idcard) == 0) {
            PrintDate(regs[i].appointDate, appDateStr);
            if (regs[i].isToday) {
                strcpy_s(regType, 10, "当日挂号");
            }
            else {
                strcpy_s(regType, 10, "预约挂号");
            }
            printf("%d\t%s\t%s\t%.1f\n", regs[i].regId, appDateStr, regType, regs[i].fee);
        }
    }
    if (regNum == 0)
        printf("暂无费用记录！\n");
    printf("=======================================================\n");
    PauseAndClear();
}

// 查看个人信息
void PatientCheckInfo(Loginf* user) {
    int patIdx = FindPatientById(user->idcard);

    system("cls");
    printf("===================== 我的个人信息 =====================\n");
    if (patIdx == -1) {
        printf("患者信息不存在！\n");
        PauseAndClear();
        return;
    }
    printf("身份证号：%s\n", patients[patIdx].idcard);
    printf("患者姓名：%s\n", patients[patIdx].name);
    printf("性别：%s\n", patients[patIdx].gender == 'M' ? "男" : "女");
    printf("住院状态：%s\n", patients[patIdx].isHospitalized ? "已住院" : "未住院");
    if (patients[patIdx].isHospitalized) {
        printf("住院床位：%d号（%s）\n", patients[patIdx].bedId, depts[beds[patients[patIdx].bedId - 1].deptId - 1].deptName);
        char inDateStr[20] = { 0 };
        PrintDate(patients[patIdx].inDate, inDateStr);
        printf("入院日期：%s\n", inDateStr);
    }
    printf("简易病历：%s\n", patients[patIdx].medicalRecord[0] == '\0' ? "暂无病历" : patients[patIdx].medicalRecord);
    printf("=======================================================\n");
    PauseAndClear();
}

// 查看病情/病例
void PatientCheckCase(Loginf* user) {
    int i = 0, hasCase = 0, docIdx = 0, j = 0, hasMed = 0;
    char docName[NAME_LENGTH] = { 0 };
    char diagDateStr[20] = { 0 };
    char inDateStr[20] = { 0 };
    char outDateStr[20] = { 0 };

    system("cls");
    printf("===================== 我的病情/病例 =====================\n");
    for (i = 0; i < caseCnt; i++) {
        if (strcmp(cases[i].patientId, user->idcard) == 0) {
            hasCase = 1;
            docIdx = FindDoctorById(cases[i].doctorId);
            if (docIdx == -1) {
                strcpy_s(docName, NAME_LENGTH, "未知");
            }
            else {
                strcpy_s(docName, NAME_LENGTH, doctors[docIdx].name);
            }
            PrintDate(cases[i].diagDate, diagDateStr);
            PrintDate(cases[i].inDate, inDateStr);
            PrintDate(cases[i].outDate, outDateStr);

            printf("病例编号：%d | 诊断医生：%s\n", cases[i].caseId, docName);
            printf("诊断日期：%s | 病情描述：%s\n", diagDateStr, cases[i].illness);
            printf("住院状态：%s\n", cases[i].bedId == 0 ? "无需住院" : "已安排住院");
            if (cases[i].bedId != 0) {
                printf("住院床位：%d号 | 入院日期：%s | 出院日期：%s\n",
                    cases[i].bedId, inDateStr, cases[i].outDate.year == 0 ? "未出院" : outDateStr);
            }
            printf("开具药品：");
            hasMed = 0;
            for (j = 0; j < 5; j++) {
                if (cases[i].medId[j] != 0 && cases[i].medNum[j] != 0) {
                    hasMed = 1;
                    printf("%s（%d盒）、", meds[cases[i].medId[j] - 1].medName, cases[i].medNum[j]);
                }
            }
            if (!hasMed)
                printf("暂无药品");
            else
                printf("\b\b  ");
            printf("\n=======================================================\n");
        }
    }
    if (!hasCase)
        printf("暂无病例记录！\n");
    PauseAndClear();
}

// 住院业务（查看床位/办理出院）
void PatientHospital(Loginf* user) {
    int patIdx = FindPatientById(user->idcard);
    int d = 0;
    char ch = 0;
    char inDateStr[20] = { 0 };

    system("cls");
    printf("===================== 住院业务办理 =====================\n");
    if (patIdx == -1) {
        printf("患者信息不存在！\n");
        PauseAndClear();
        return;
    }

    if (!patients[patIdx].isHospitalized) {
        printf("当前状态：未住院 | 各科室空闲床位信息：\n");
        printf("-------------------------------------------------------\n");
        for (d = NEIKE; d <= GUKE; d++) {
            printf("%s：总床位%d张 | 空闲%d张\n",
                depts[d - 1].deptName, depts[d - 1].totalBed, depts[d - 1].freeBed);
        }
        printf("=======================================================\n");
        printf("住院需由医生开具住院证明后分配床位，不可自行办理！\n");
    }
    else {
        printf("当前状态：已住院\n");
        printf("住院床位：%d号（%s）\n", patients[patIdx].bedId, depts[beds[patients[patIdx].bedId - 1].deptId - 1].deptName);
        PrintDate(patients[patIdx].inDate, inDateStr);
        printf("入院日期：%s\n", inDateStr);
        printf("=======================================================\n");
        printf("是否办理出院？（Y/N）：");
        ch = _getch();
        if (ch == 'Y' || ch == 'y') {
            patients[patIdx].isHospitalized = 0;
            patients[patIdx].outDate = GetCurrentDate();
            FreeBed(patients[patIdx].bedId);
            for (int i = 0; i < caseCnt; i++) {
                if (strcmp(cases[i].patientId, user->idcard) == 0 && cases[i].bedId == patients[patIdx].bedId) {
                    cases[i].outDate = GetCurrentDate();
                }
            }
            SavePatientToFile();
            SaveCaseToFile();
            printf("出院办理成功！\n");
            printf("出院日期：%s\n", inDateStr);
        }
        else {
            printf("取消出院办理！\n");
        }
    }
    PauseAndClear();
}

// 患者主菜单
void PatientMenu(Loginf* user) {
    int choice = 0;
    while (1) {
        system("cls");
        PrintHospitalBanner();
        SetColor(13); // 亮紫色
        printf("===================== 患者功能菜单 =====================\n");
        printf("当前用户：%s（患者）\n", user->idcard);
        printf("1. 7天预约挂号    2. 当日挂号    3. 查看已挂号\n");
        printf("4. 查看费用账单    5. 个人信息    6. 病情/病例查看\n");
        printf("7. 住院业务办理    0. 退出菜单\n");
        printf("=======================================================\n");
        printf("请输入操作选项：");
        if (scanf_s("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("输入错误！请输入数字！\n");
            PauseAndClear();
            continue;
        }
        switch (choice) {
        case 1: PatientAppointReg(user); break;
        case 2: PatientTodayReg(user); break;
        case 3: PatientCheckReg(user); break;
        case 4: PatientCheckBill(user); break;
        case 5: PatientCheckInfo(user); break;
        case 6: PatientCheckCase(user); break;
        case 7: PatientHospital(user); break;
        case 0: if (ExitConfirm()) return; break;
        default: printf("输入错误！请输入0-7的数字！\n"); PauseAndClear();
        }
    }
}

// ===================== 医生子功能实现 =====================
// 查看医生个人信息
void DoctorCheckInfo(Loginf* user) {
    int docIdx = FindDoctorById(user->idcard);
    int i = 0;

    system("cls");
    printf("===================== 我的医生信息 =====================\n");
    if (docIdx == -1) {
        printf("医生信息不存在！\n");
        PauseAndClear();
        return;
    }
    printf("身份证号：%s\n", doctors[docIdx].idcard);
    printf("医生姓名：%s\n", doctors[docIdx].name);
    printf("所属科室：%s\n", depts[doctors[docIdx].deptId - 1].deptName);
    printf("职称：%s\n", doctors[docIdx].title);
    printf("擅长领域：%s\n", doctors[docIdx].specialty);
    printf("医生简介：%s\n", doctors[docIdx].info);
    printf("=======================================================\n");
    printf("我的值班安排：\n");
    for (i = 0; i < dutyCnt; i++) {
        if (strcmp(docDuty[i].doctorId, user->idcard) == 0) {
            printf("【%s】周%d值班\n", depts[docDuty[i].deptId - 1].deptName, docDuty[i].weekDay);
        }
    }
    printf("=======================================================\n");
    PauseAndClear();
}

// 查看病人列表（挂自己号的患者）
void DoctorCheckPatient(Loginf* user) {
    int i = 0, hasPatient = 0, patIdx = 0;
    char regType[10] = { 0 };
    char patName[NAME_LENGTH] = { 0 };
    char regDateStr[20] = { 0 };
    int docIdx = FindDoctorById(user->idcard);

    system("cls");
    printf("===================== 我的病人列表 =====================\n");
    printf("当前医生：%s（%s）\n", doctors[docIdx].name, depts[doctors[docIdx].deptId - 1].deptName);
    printf("=======================================================\n");
    printf("挂号编号\t患者姓名\t身份证号\t\t挂号类型\t挂号日期\n");
    printf("-------------------------------------------------------\n");

    for (i = 0; i < regCnt; i++) {
        if (strcmp(regs[i].doctorId, user->idcard) == 0) {
            hasPatient = 1;
            patIdx = FindPatientById(regs[i].patientId);
            if (patIdx == -1) {
                strcpy_s(patName, NAME_LENGTH, "未知");
            }
            else {
                strcpy_s(patName, NAME_LENGTH, patients[patIdx].name);
            }
            if (regs[i].isToday) {
                strcpy_s(regType, 10, "当日");
            }
            else {
                strcpy_s(regType, 10, "预约");
            }
            PrintDate(regs[i].regDate, regDateStr);
            printf("%d\t\t%s\t\t%s\t%s\t%s\n",
                regs[i].regId, patName, regs[i].patientId, regType, regDateStr);
        }
    }
    if (!hasPatient)
        printf("暂无挂您号的患者！\n");
    printf("=======================================================\n");
    PauseAndClear();
}

// 查询病人详细信息
void DoctorFindPatient(Loginf* user) {
    char patId[ID_LENGTH] = { 0 };
    int patIdx = 0;
    int i = 0, hasReg = 0, hasCase = 0;
    int docIdx = 0;
    char docName[NAME_LENGTH] = { 0 };
    char regDateStr[20] = { 0 };
    char diagDateStr[20] = { 0 };

    system("cls");
    printf("===================== 查询病人信息 =====================\n");
    printf("请输入患者身份证号：");
    scanf_s("%s", patId, (unsigned)_countof(patId));
    patIdx = FindPatientById(patId);
    if (patIdx == -1) {
        printf("该患者信息不存在！\n");
        PauseAndClear();
        return;
    }

    printf("=======================================================\n");
    printf("患者身份证：%s\n", patients[patIdx].idcard);
    printf("患者姓名：%s\n", patients[patIdx].name);
    printf("性别：%s\n", patients[patIdx].gender == 'M' ? "男" : "女");
    printf("住院状态：%s\n", patients[patIdx].isHospitalized ? "已住院" : "未住院");
    if (patients[patIdx].isHospitalized) {
        printf("住院床位：%d号（%s）\n", patients[patIdx].bedId, depts[beds[patients[patIdx].bedId - 1].deptId - 1].deptName);
        char inDateStr[20] = { 0 };
        PrintDate(patients[patIdx].inDate, inDateStr);
        printf("入院日期：%s\n", inDateStr);
    }
    printf("=======================================================\n");
    printf("该患者挂号记录：\n");
    hasReg = 0;
    for (i = 0; i < regCnt; i++) {
        if (strcmp(regs[i].patientId, patId) == 0) {
            hasReg = 1;
            docIdx = FindDoctorById(regs[i].doctorId);
            if (docIdx == -1) {
                strcpy_s(docName, NAME_LENGTH, "未知");
            }
            else {
                strcpy_s(docName, NAME_LENGTH, doctors[docIdx].name);
            }
            PrintDate(regs[i].regDate, regDateStr);
            printf("编号：%d | 医生：%s | 日期：%s | 费用：%.1f\n",
                regs[i].regId, docName, regDateStr, regs[i].fee);
        }
    }
    if (!hasReg)
        printf("暂无挂号记录！\n");
    printf("=======================================================\n");
    printf("该患者病例记录：\n");
    hasCase = 0;
    for (i = 0; i < caseCnt; i++) {
        if (strcmp(cases[i].patientId, patId) == 0) {
            hasCase = 1;
            PrintDate(cases[i].diagDate, diagDateStr);
            printf("编号：%d | 诊断日期：%s | 病情：%s | 床位：%d\n",
                cases[i].caseId, diagDateStr, cases[i].illness, cases[i].bedId);
        }
    }
    if (!hasCase)
        printf("暂无病例记录！\n");
    printf("=======================================================\n");
    PauseAndClear();
}

// 写/修改病例（核心：开药+分配床位）
void DoctorWriteCase(Loginf* user) {
    char patId[ID_LENGTH] = { 0 };
    int patIdx = 0, docIdx = 0;
    Date now;
    char nowDateStr[20] = { 0 };
    int caseIdx = -1, i = 0;
    char ch = 0;
    int medIdxArr[MED_PER_DEPT] = { 0 };
    int idx = 0, medNum = 0;
    int medIdSel = 0, numSel = 0;
    int freeBed = 0;

    system("cls");
    printf("===================== 写/修改患者病例 =====================\n");
    printf("请输入患者身份证号：");
    scanf_s("%s", patId, (unsigned)_countof(patId));
    patIdx = FindPatientById(patId);
    docIdx = FindDoctorById(user->idcard);
    if (patIdx == -1 || docIdx == -1) {
        printf("患者/医生信息不存在！\n");
        PauseAndClear();
        return;
    }
    now = GetCurrentDate();
    PrintDate(now, nowDateStr);

    for (i = 0; i < caseCnt; i++) {
        if (strcmp(cases[i].patientId, patId) == 0 && strcmp(cases[i].doctorId, user->idcard) == 0) {
            caseIdx = i;
            break;
        }
    }
    if (caseIdx == -1) {
        caseIdx = caseCnt;
        cases[caseIdx].caseId = caseCnt + 1;
        strcpy_s(cases[caseIdx].patientId, ID_LENGTH, patId);
        strcpy_s(cases[caseIdx].doctorId, ID_LENGTH, user->idcard);
        cases[caseIdx].diagDate = now;
        for (i = 0; i < 5; i++) {
            cases[caseIdx].medId[i] = 0;
            cases[caseIdx].medNum[i] = 0;
        }
        cases[caseIdx].bedId = 0;
        caseCnt++;
    }

    printf("=======================================================\n");
    printf("当前诊断日期：%s\n", nowDateStr);
    printf("请输入患者病情描述：");
    while (getchar() != '\n');
    gets_s(cases[caseIdx].illness, MAX_INFO);

    printf("=======================================================\n");
    printf("是否为患者开具药品？（Y/N）：");
    ch = _getch();
    if (ch == 'Y' || ch == 'y') {
        printf("\n当前科室：%s | 可开具本科室绑定药品（6类）\n", depts[doctors[docIdx].deptId - 1].deptName);
        printf("本科室药品列表：\n");
        printf("%-6s %-18s %-12s %-8s %-8s %-6s\n",
            "编号", "名称", "别名",  "类型", "单价", "库存");
        printf("-------------------------------------------------------\n");
        idx = 0;
        for (i = 0; i < medCnt; i++) {
            if (meds[i].bindDept == doctors[docIdx].deptId) {
                medIdxArr[idx++] = i;
                char medType[10] = { 0 };
                if (meds[i].medType == PRESCRIPTION) {
                    strcpy_s(medType, 10, "处方药");
                }
                else {
                    strcpy_s(medType, 10, "OTC");
                }
                printf("%-6d %-18s %-12s %-10s %-8.2lf %-6d\n",
					meds[i].medId, meds[i].medName, meds[i].medAlias, medType, meds[i].price,meds[i].stock);//错在这里meds[i].bindDept应该是depts[meds[i].bindDept - 1].deptName，但为了简化代码就不显示科室名称了
            
            }
        }

        medNum = 0;
        while (1) {
            if (medNum >= 5) {
                printf("最多开具5种药品！\n");
                break;
            }
            printf("请输入药品编号（输入0结束）：");
            scanf_s("%d", &medIdSel);
            if (medIdSel == 0) break;

            int isDeptMed = 0;
            for (i = 0; i < idx; i++) {
                if (meds[medIdxArr[i]].medId == medIdSel)
                    isDeptMed = 1;
            }
            if (medIdSel < 1 || medIdSel > medCnt || !isDeptMed) {
                printf("药品编号错误/非本科室药品！\n");
                continue;
            }

            printf("请输入药品数量（盒）：");
            scanf_s("%d", &numSel);
            if (numSel <= 0 || !ReduceMedStock(medIdSel, numSel)) {
                printf("数量错误/库存不足！\n");
                continue;
            }

            cases[caseIdx].medId[medNum] = medIdSel;
            cases[caseIdx].medNum[medNum] = numSel;
            medNum++;
            printf("成功开具%s %d盒！\n", meds[medIdSel - 1].medName, numSel);
        }
    }

    printf("=======================================================\n");
    printf("是否为患者安排住院？（Y/N）：");
    ch = _getch();
    if (ch == 'Y' || ch == 'y') {
        freeBed = FindFreeBed(doctors[docIdx].deptId);
        if (freeBed == -1) {
            printf("本科室无空闲床位！无法安排住院！\n");
        }
        else {
            cases[caseIdx].bedId = freeBed;
            cases[caseIdx].inDate = now;
            cases[caseIdx].outDate.year = 0;
            patients[patIdx].isHospitalized = 1;
            patients[patIdx].bedId = freeBed;
            patients[patIdx].inDate = now;
            beds[freeBed - 1].isFree = 0;
            strcpy_s(beds[freeBed - 1].patientId, ID_LENGTH, patId);
            beds[freeBed - 1].inDate = now;
            depts[doctors[docIdx].deptId - 1].freeBed--;
            printf("成功为患者安排住院！\n");
            printf("住院科室：%s | 床位号：%d号\n", depts[doctors[docIdx].deptId - 1].deptName, freeBed);
        }
    }

    SaveCaseToFile();
    SavePatientToFile();
    SaveBedToFile();
    printf("=======================================================\n");
    printf("病例写/修改成功！已持久化到本地！\n");
    PauseAndClear();
}

// 床位分配/释放（单独操作）
void DoctorAssignBed(Loginf* user) {
    int docIdx = FindDoctorById(user->idcard);
    int choice = 0;
    char patId[ID_LENGTH] = { 0 };
    int patIdx = 0;
    int freeBed = 0;
    int bedSel = 0;
    int i = 0;

    system("cls");
    printf("===================== 床位分配/释放 =====================\n");
    if (docIdx == -1) {
        printf("医生信息不存在！\n");
        PauseAndClear();
        return;
    }
    printf("当前科室：%s | 总床位%d张 | 空闲%d张\n",
        depts[doctors[docIdx].deptId - 1].deptName,
        depts[doctors[docIdx].deptId - 1].totalBed,
        depts[doctors[docIdx].deptId - 1].freeBed);
    printf("=======================================================\n");
    printf("请选择操作（1-分配床位 2-释放床位 0-取消）：");
    scanf_s("%d", &choice);

    if (choice == 1) {
        printf("请输入患者身份证号：");
        scanf_s("%s", patId, (unsigned)_countof(patId));
        patIdx = FindPatientById(patId);
        if (patIdx == -1) {
            printf("患者信息不存在！\n");
            PauseAndClear();
            return;
        }
        if (patients[patIdx].isHospitalized) {
            printf("该患者已住院！无需重复分配！\n");
            PauseAndClear();
            return;
        }
        freeBed = FindFreeBed(doctors[docIdx].deptId);
        if (freeBed == -1) {
            printf("本科室无空闲床位！\n");
            PauseAndClear();
            return;
        }

        patients[patIdx].isHospitalized = 1;
        patients[patIdx].bedId = freeBed;
        patients[patIdx].inDate = GetCurrentDate();
        beds[freeBed - 1].isFree = 0;
        strcpy_s(beds[freeBed - 1].patientId, ID_LENGTH, patId);
        beds[freeBed - 1].inDate = GetCurrentDate();
        depts[doctors[docIdx].deptId - 1].freeBed--;
        SavePatientToFile();
        SaveBedToFile();
        printf("床位分配成功！\n");
        printf("患者：%s | 床位号：%d号（%s）\n",
            patients[patIdx].name, freeBed, depts[doctors[docIdx].deptId - 1].deptName);
    }
    else if (choice == 2) {
        printf("请输入要释放的床位号：");
        scanf_s("%d", &bedSel);
        if (bedSel < 1 || bedSel > MAX_BEDS || beds[bedSel - 1].deptId != doctors[docIdx].deptId) {
            printf("床位号错误/非本科室床位！\n");
            PauseAndClear();
            return;
        }
        if (beds[bedSel - 1].isFree) {
            printf("该床位已空闲！无需释放！\n");
            PauseAndClear();
            return;
        }

        FreeBed(bedSel);
        for (i = 0; i < patCnt; i++) {
            if (patients[i].bedId == bedSel) {
                patients[i].isHospitalized = 0;
                patients[i].outDate = GetCurrentDate();
                patients[i].bedId = 0;
                break;
            }
        }
        SavePatientToFile();
        printf("床位释放成功！\n");
        printf("床位号：%d号（%s）已转为空闲！\n", bedSel, depts[doctors[docIdx].deptId - 1].deptName);
    }
    PauseAndClear();
}

// 医生主菜单
void DoctorMenu(Loginf* user) {
    int choice = 0;
    while (1) {
        system("cls");
        PrintHospitalBanner();
        SetColor(10);
        printf("===================== 医生功能菜单 =====================\n");
        printf("当前用户：%s（%s医生）\n", user->idcard, depts[doctors[FindDoctorById(user->idcard)].deptId - 1].deptName);
        printf("1. 个人信息查询    2. 我的病人列表    3. 查找病人信息\n");
        printf("4. 写/修改病例     5. 床位分配/释放    0. 退出菜单\n");
        printf("=======================================================\n");
        printf("请输入操作选项：");
        if (scanf_s("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("输入错误！请输入数字！\n");
            PauseAndClear();
            continue;
        }
        switch (choice) {
        case 1: DoctorCheckInfo(user); break;
        case 2: DoctorCheckPatient(user); break;
        case 3: DoctorFindPatient(user); break;
        case 4: DoctorWriteCase(user); break;
        case 5: DoctorAssignBed(user); break;
        case 0: if (ExitConfirm()) return; break;
        default: printf("输入错误！请输入0-5的数字！\n"); PauseAndClear();
        }
    }
}

// ===================== 管理员子功能实现 =====================
// 查看所有药品列表
void AdminCheckMed(Loginf* user) {
    int i = 0;
    char deptName[NAME_LENGTH] = { 0 };
    char medType[10] = { 0 };
    char prodDateStr[20] = { 0 };

    system("cls");
    printf("===================== 全科室药品列表 =====================\n");
    printf("总计%d类药品 | 5科室×6类（按科室绑定）\n", medCnt);
    printf("=========================================================\n");
    printf("%-6s %-18s %-12s %-8s %-8s %-8s %-6s %-12s\n",
        "编号", "名称", "别名", "科室", "类型", "单价", "库存", "生产日期");
    printf("---------------------------------------------------------\n");
    for (i = 0; i < medCnt; i++) {
        strcpy_s(deptName, NAME_LENGTH, depts[meds[i].bindDept - 1].deptName);
        if (meds[i].medType == PRESCRIPTION) {
            strcpy_s(medType, 10, "处方药");
        }
        else {
            strcpy_s(medType, 10, "OTC");
        }
        PrintDate(meds[i].prodDate, prodDateStr);
        // 对齐版本的 printf
        printf("%-6d %-18s %-12s %-8s %-8s %-8.1f %-6d %-12s\n",
            meds[i].medId,
            meds[i].medName,
            meds[i].medAlias,
            deptName,
            medType,
            meds[i].price,
            meds[i].stock,
            prodDateStr);
    }
    printf("=========================================================\n");
    PauseAndClear();
}

// 修改药品库存（入库/出库）
void AdminEditStock(Loginf* user) {
    int medIdSel = 0;
    int numSel = 0;

    system("cls");
    printf("===================== 药品库存修改 =====================\n");
    printf("当前操作：药品入库/出库 | 输入正数=入库，负数=出库\n");
    printf("=======================================================\n");
    printf("请输入药品编号：");
    scanf_s("%d", &medIdSel);
    if (medIdSel < 1 || medIdSel > medCnt) {
        printf("药品编号错误！\n");
        PauseAndClear();
        return;
    }

    printf("=======================================================\n");
    printf("药品信息：\n");
    printf("编号：%d | 名称：%s | 别名：%s | 科室：%s\n",
        meds[medIdSel - 1].medId, meds[medIdSel - 1].medName,
        meds[medIdSel - 1].medAlias, depts[meds[medIdSel - 1].bindDept - 1].deptName);
    printf("当前库存：%d盒 | 单价：%.1f元\n", meds[medIdSel - 1].stock, meds[medIdSel - 1].price);

    printf("=======================================================\n");
    printf("请输入库存修改数量（正数入库/负数出库）：");
    scanf_s("%d", &numSel);
    if (numSel == 0) {
        printf("修改数量不能为0！\n");
        PauseAndClear();
        return;
    }
    if (numSel < 0 && meds[medIdSel - 1].stock + numSel < 0) {
        printf("出库数量超过当前库存！库存不足！\n");
        PauseAndClear();
        return;
    }

    meds[medIdSel - 1].stock += numSel;
    SaveMedicineToFile();
    printf("=======================================================\n");
    printf("库存修改成功！\n");
    printf("操作类型：%s | 修改数量：%d | 最新库存：%d盒\n",
        numSel > 0 ? "入库" : "出库", abs(numSel), meds[medIdSel - 1].stock);
    PauseAndClear();
}


// 管理员：编辑医生
void AdminDeleteDoctor(Loginf* user) {
    char docId[ID_LENGTH] = { 0 };
    int idx = 0, i = 0, j = 0;
    char ch = 0;
    int choice = -1;

    system("cls");
    printf("===================== 删除医生 =====================\n");
    printf("1. 查看所有医生信息\n");
    printf("2. 删除指定医生（按身份证号）\n");
    printf("0. 取消\n");
    printf("===================================================\n");
    printf("请输入操作选项：");
    if (scanf_s("%d", &choice) != 1) {
        while (getchar() != '\n');
        printf("输入错误！请输入数字选项！\n");
        PauseAndClear();
        return;
    }

    if (choice == 0) {
        printf("已取消操作。\n");
        PauseAndClear();
        return;
    }

    if (choice == 1) {
        system("cls");
        printf("===================== 全部医生信息 =====================\n");
        printf("共计%d名医生\n", docCnt);
        printf("=======================================================\n");
        // 表头，使用 NAME_LENGTH 保证列宽一致
        printf("%-*s %-*s %-*s %-*s %-*s\n",
            NAME_LENGTH, "身份证号",
            NAME_LENGTH, "姓名",
            8, "科室",
            NAME_LENGTH, "职称",
            NAME_LENGTH, "擅长");
        printf("-------------------------------------------------------\n");
        for (i = 0; i < docCnt; i++) {
            char deptName[NAME_LENGTH] = { 0 };
            strcpy_s(deptName, NAME_LENGTH, depts[doctors[i].deptId - 1].deptName);
            printf("%-*s %-*s %-*s %-*s %-*s\n",
                NAME_LENGTH, doctors[i].idcard,
                NAME_LENGTH, doctors[i].name,
                8, deptName,
                NAME_LENGTH, doctors[i].title,
                NAME_LENGTH, doctors[i].specialty);
            // 简介单独一行，保证可读性
            printf("简介：%s\n", doctors[i].info);
            printf("-------------------------------------------------------\n");
        }
        PauseAndClear();
        return;
    }

    if (choice == 2) {
        system("cls");
        printf("===================== 删除医生 =====================\n");
        printf("请输入要删除的医生身份证号：");
        scanf_s("%s", docId, (unsigned)_countof(docId));

        idx = FindDoctorById(docId);
        if (idx == -1) {
            printf("该医生不存在！\n");
            PauseAndClear();
            return;
        }

        // 显示医生信息以确认
        printf("将删除以下医生：\n");
        printf("身份证：%s\n", doctors[idx].idcard);
        printf("姓名：%s | 科室：%s | 职称：%s\n",
            doctors[idx].name, depts[doctors[idx].deptId - 1].deptName, doctors[idx].title);
        printf("是否确实要删除该医生？（Y/N）：");
        ch = _getch();
        if (ch != 'Y' && ch != 'y') {
            printf("已取消删除。\n");
            PauseAndClear();
            return;
        }

        // 删除该医生的值班安排（紧凑数组）
        for (i = 0; i < dutyCnt; ) {
            if (strcmp(docDuty[i].doctorId, docId) == 0) {
                for (j = i; j < dutyCnt - 1; j++) {
                    docDuty[j] = docDuty[j + 1];
                }
                dutyCnt--;
                // 不递增 i，检查被移入当前位置的新元素
            }
            else {
                i++;
            }
        }

        // 删除医生记录（紧凑数组）
        for (i = idx; i < docCnt - 1; i++) {
            doctors[i] = doctors[i + 1];
        }
        docCnt--;

        // 清除挂号/病例中对该医生的引用（保留记录但取消医生关联）
        for (i = 0; i < regCnt; i++) {
            if (strcmp(regs[i].doctorId, docId) == 0) {
                regs[i].doctorId[0] = '\0';
            }
        }
        for (i = 0; i < caseCnt; i++) {
            if (strcmp(cases[i].doctorId, docId) == 0) {
                cases[i].doctorId[0] = '\0';
            }
        }

        // 从登录链表中删除对应的登录信息（Loginf）
        if (userroot != NULL) {
            Loginf* prev = NULL;
            Loginf* p = userroot;
            int removed = 0;
            while (p != NULL) {
                if (strcmp(p->idcard, docId) == 0) {
                    // 找到，断链并释放节点
                    if (prev == NULL) {
                        userroot = p->next;
                    }
                    else {
                        prev->next = p->next;
                    }
                    free(p);
                    removed = 1;
                    break;
                }
                prev = p;
                p = p->next;
            }
            if (removed) {
                // 持久化登录信息（删除后保存）
                SaveLoginfToFile();
            }
        }

        // 持久化其他数据
        SaveDoctorToFile();
        SaveDoctorDutyToFile();
        SaveRegToFile();
        SaveCaseToFile();

        printf("删除成功！\n");
        PauseAndClear();
        return;
    }

    // 非法选项
    printf("输入错误！请输入0/1/2。\n");
    PauseAndClear();
}

// 药品检索（名称/别名，模糊匹配）
void AdminSearchMed(Loginf* user) {
    char key[NAME_LENGTH] = { 0 };
    int res[MAX_MEDICINE] = { 0 };
    int resNum = 0;
    int i = 0;
    char deptName[NAME_LENGTH] = { 0 };
    char medType[10] = { 0 };

    system("cls");
    printf("===================== 药品模糊检索 =====================\n");
    printf("支持：药品名称/别名检索（不区分大小写）\n");
    printf("=======================================================\n");
    printf("请输入检索关键字：");
    while (getchar() != '\n');
    gets_s(key, NAME_LENGTH);
    resNum = FindMedicineByKey(key, res);

    printf("=======================================================\n");
    printf("检索关键字：%s | 共找到%d类匹配药品\n", key, resNum);
    if (resNum == 0) {
        PauseAndClear();
        return;
    }
    printf("编号\t名称\t\t别名\t\t科室\t\t类型\t\t单价\t库存\n");
    printf("-------------------------------------------------------\n");
    for (i = 0; i < resNum; i++) {
        strcpy_s(deptName, NAME_LENGTH, depts[meds[res[i]].bindDept - 1].deptName);
        if (meds[res[i]].medType == PRESCRIPTION) {
            strcpy_s(medType, 10, "处方药");
        }
        else {
            strcpy_s(medType, 10, "OTC");
        }
        printf("%d\t%s\t\t%s\t\t%s\t\t%s\t\t%.1f\t%d\n",
            meds[res[i]].medId, meds[res[i]].medName, meds[res[i]].medAlias,
            deptName, medType, meds[res[i]].price, meds[res[i]].stock);
    }
    printf("=======================================================\n");
    PauseAndClear();
}

// 查看库存预警（库存<20盒的药品）
void AdminCheckStock(Loginf* user) {
    int i = 0, hasWarn = 0;
    int need = 0;
    char deptName[NAME_LENGTH] = { 0 };

    system("cls");
    printf("===================== 药品库存预警 =====================\n");
    printf("预警阈值：库存<20盒 | 以下为需补货药品\n");
    printf("=======================================================\n");
    printf("编号\t名称\t\t别名\t\t科室\t\t当前库存\t需补货\n");
    printf("-------------------------------------------------------\n");
    for (i = 0; i < medCnt; i++) {
        if (meds[i].stock < 20) {
            hasWarn = 1;
            need = 20 - meds[i].stock;
            strcpy_s(deptName, NAME_LENGTH, depts[meds[i].bindDept - 1].deptName);
            printf("%d\t%s\t\t%s\t\t%s\t\t%d\t\t%d\n",
                meds[i].medId, meds[i].medName, meds[i].medAlias,
                deptName, meds[i].stock, need);
        }
    }
    if (!hasWarn)
        printf("暂无库存预警药品！所有药品库存充足！\n");
    printf("=======================================================\n");
    PauseAndClear();
}

// 管理员主菜单

void AdminMenu(Loginf* user) {
    int choice = 0;
    while (1) {
        system("cls");
        PrintHospitalBanner();
        SetColor(14); 
        printf("===================== 药品管理员菜单 =====================\n");
        printf("当前用户：%s（管理员）\n", user->idcard);
        printf("1. 编辑医生    2. 查看全科室药品    3. 修改药品库存\n");
        printf("4. 药品模糊检索    5. 库存预警查询      0. 退出菜单\n");
        printf("========================================================\n");
        printf("请输入操作选项：");
        if (scanf_s("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("输入错误！请输入数字！\n");
            PauseAndClear();
            continue;
        }
        switch (choice) {
        case 1: AdminDeleteDoctor(user); break;
        case 2: AdminCheckMed(user); break;
        case 3: AdminEditStock(user); break;
        case 4: AdminSearchMed(user); break;
        case 5: AdminCheckStock(user); break;
        case 0: if (ExitConfirm()) return; break;
        default: printf("输入错误！请输入0-5的数字！\n"); PauseAndClear();
        }
    }
}