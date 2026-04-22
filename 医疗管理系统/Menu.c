#include "Menu.h"
#include "Tool.h"
#include"windows.h"
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

    printf("该科室未来7天值班安排：\n");
    printf("┌───────────────┬────────────────┬────────────────┬────────────────────────────────────┐\n");
    printf("│     时间      │     姓名       │      职称      │               擅长                 │\n");
    printf("├───────────────┼────────────────┼────────────────┼────────────────────────────────────┤\n");

    for (int off = 1; off <= 7; off++) {
        int dayOfWeek = (nowWeek + off - 1) % 7 + 1;
        char* dutyDocId = FindDutyDoctor(deptId, dayOfWeek);
        DoctorNode* doc = dutyDocId ? FindDoctorById(dutyDocId) : NULL;

        char timeStr[30] = { 0 };
        if (off == 1) sprintf_s(timeStr, sizeof(timeStr), "周%d(明天)", dayOfWeek);
        else if (off == 2) sprintf_s(timeStr, sizeof(timeStr), "周%d(后天)", dayOfWeek);
        else sprintf_s(timeStr, sizeof(timeStr), "周%d", dayOfWeek);

        if (doc != NULL) {
            printf("│ %-13s │ %-14s │ %-14s │ %-34s │\n",
                timeStr, doc->name, doc->title, doc->specialty);
        }
        else {
            printf("│ %-13s │     无安排     │                │                                    │\n", timeStr);
        }
    }
    printf("└───────────────┴────────────────┴────────────────┴────────────────────────────────────┘\n");

    int weekSel = 0;
    printf("请选择预约周几（1-周一 ...7-周日，不可选今日及过去日期）：");
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
    DoctorNode* doc = FindDoctorById(docId);
    if (doc == NULL) {
        printf("值班医生信息不存在！\n");
        PauseAndClear();
        return;
    }

    printf("=======================================================\n");
    printf("【%s】周%d值班医生信息\n", depts[deptId - 1].deptName, weekSel);
    printf("姓名：%s | 职称：%s\n", doc->name, doc->title);
    printf("擅长：%s \n简介：%s\n", doc->specialty, doc->info);
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
    if (!(weekSel - nowWeek)) appointDate.day += 7;
    if (!CheckAppointDate(now, appointDate)) {
        printf("预约日期不合法（超出7天）！\n");
        PauseAndClear();
        return;
    }


    RegNode* newReg = (RegNode*)malloc(sizeof(RegNode));
    memset(newReg, 0, sizeof(RegNode));
    newReg->regId = regCnt + 1;
    strcpy_s(newReg->patientId, ID_LENGTH, user->idcard);
    newReg->deptId = deptId;
    strcpy_s(newReg->doctorId, ID_LENGTH, docId);
    newReg->weekDay = weekSel;
    newReg->fee = FEE_APPOINT;
    newReg->isToday = 0;
    newReg->regDate = now;
    newReg->appointDate = appointDate;
    newReg->next = NULL;

    if (regHead == NULL) regHead = newReg;
    else {
        RegNode* tail = regHead;
        while (tail->next) tail = tail->next;
        tail->next = newReg;
    }
    regCnt++;
    SaveRegToFile();

    char appointDateStr[20] = { 0 };
    PrintDate(appointDate, appointDateStr);
    printf("\n===================== 预约成功！ =====================\n");
    printf("预约日期：%s\n", appointDateStr);
    printf("科室：%s | 医生：%s\n", depts[deptId - 1].deptName, doc->name);
    printf("费用：%.1f元 | 请按时就诊！\n", FEE_APPOINT);
    PauseAndClear();
}

void PatientTodayReg(Loginf* user) {
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
    if (deptSel < 1 || deptSel > 5) {
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
    DoctorNode* doc = FindDoctorById(docId);
    if (doc == NULL) {
        printf("值班医生信息不存在！\n");
        PauseAndClear();
        return;
    }

    printf("======================================================\n");
    printf("【%s】今日值班医生：%s（%s）\n", depts[deptId - 1].deptName, doc->name, doc->title);
    printf("擅长：%s | 挂号费：%.1f元\n", doc->specialty, FEE_TODAY);

    printf("是否确认当日挂号？（Y/N）：");
    char ch = _getch();
    if (ch != 'Y' && ch != 'y') {
        printf("取消挂号！\n");
        PauseAndClear();
        return;
    }

    // 创建挂号记录节点并加入链表
    RegNode* newReg = (RegNode*)malloc(sizeof(RegNode));
    memset(newReg, 0, sizeof(RegNode));
    newReg->regId = regCnt + 1;
    strcpy_s(newReg->patientId, ID_LENGTH, user->idcard);
    newReg->deptId = deptId;
    strcpy_s(newReg->doctorId, ID_LENGTH, docId);
    newReg->weekDay = nowWeek;
    newReg->fee = FEE_TODAY;
    newReg->isToday = 1;
    newReg->regDate = now;
    newReg->appointDate = now;
    newReg->next = NULL;

    if (regHead == NULL) regHead = newReg;
    else {
        RegNode* tail = regHead;
        while (tail->next) tail = tail->next;
        tail->next = newReg;
    }
    regCnt++;
    SaveRegToFile();

    printf("\n===================== 当日挂号成功！ =====================\n");
    printf("挂号编号：%d | 就诊科室：%s\n", newReg->regId, depts[deptId - 1].deptName);
    printf("就诊医生：%s | 费用：%.1f元\n", doc->name, FEE_TODAY);
    PauseAndClear();
}

void PatientCheckReg(Loginf* user) {
    system("cls");
    Date now = GetCurrentDate();
    char nowDateStr[20] = { 0 };
    PrintDate(now, nowDateStr);


    int hasReg = 0;
    RegNode* p = regHead;
    while (p != NULL) {
        if (strcmp(p->patientId, user->idcard) == 0 && (p->isToday == 0 || p->isToday == 1)) {
            hasReg++;
        }
        p = p->next;
    }
    printf("当前系统时间：%s | 共查询到%d条挂号记录\n", nowDateStr, hasReg);
    printf("=======================================================\n");
    printf("编号\t类型\t科室\t\t医生\t\t日期\t\t费用\n");
    printf("-------------------------------------------------------\n");
    p = regHead;
    hasReg = 0;
    while (p != NULL) {
        if (strcmp(p->patientId, user->idcard) == 0 && (p->isToday == 0 || p->isToday == 1)) {
            hasReg++;
            char regType[10];
            strcpy_s(regType, 10, p->isToday ? "当日" : "预约");
            char deptName[NAME_LENGTH];
            strcpy_s(deptName, NAME_LENGTH, depts[p->deptId - 1].deptName);
            DoctorNode* doc = FindDoctorById(p->doctorId);
            char docName[NAME_LENGTH] = "停诊";
            if (doc) strcpy_s(docName, NAME_LENGTH, doc->name);
            char appDateStr[20];
            PrintDate(p->appointDate, appDateStr);
            printf("%d\t%s\t%s\t\t%s\t\t%s\t%.1f\n",
                hasReg, regType, deptName, docName, appDateStr, p->fee);
        }
        p = p->next;
    }
    if (!hasReg) printf("暂无挂号记录！\n");
    printf("=======================================================\n");
    PauseAndClear();
}

void PatientCheckBill(Loginf* user) {
    system("cls");
    printf("===================== 我的费用账单 =====================\n");
    int regNum = 0;
    float totalFee = 0.0f;
    RegNode* p = regHead;
    while (p != NULL) {
        if (strcmp(p->patientId, user->idcard) == 0) {
            totalFee += p->fee;
            regNum++;
        }
        p = p->next;
    }
    printf("累计消费次数：%d次\n", regNum);
    printf("累计消费金额：%.1f元\n", totalFee);
    printf("=======================================================\n");
    printf("费用明细：\n");
    printf("编号\t日期\t\t   类型\t      费用\n");
    printf("-------------------------------------------------------\n");

    p = regHead;
    regNum = 0;
    while (p != NULL) {
        
        if (strcmp(p->patientId, user->idcard) == 0) {
            regNum++;
            char appDateStr[20];
            PrintDate(p->appointDate, appDateStr);
            char regType[20] = "其他消费";
            if (p->isToday == 1) strcpy_s(regType, 20, "当日挂号");
            else if (p->isToday == 0) strcpy_s(regType, 20, "预约挂号");
            else if (p->isToday == 2) strcpy_s(regType, 20, "开具药品");
            else if (p->isToday == 3) strcpy_s(regType, 20, "开具检查");
            else if (p->isToday == 4) strcpy_s(regType, 20, "住院费用");
            printf("%d\t%s\t%s\t%.1f\n", regNum, appDateStr, regType, p->fee);
        }
        p = p->next;
    }
    if (regNum == 0) printf("暂无费用记录！\n");
    printf("=======================================================\n");
    PauseAndClear();
}

void PatientCheckInfo(Loginf* user) {
    system("cls");
    printf("===================== 我的个人信息 =====================\n");
    PatientNode* pat = FindPatientById(user->idcard);
    if (pat == NULL) {
        printf("患者信息不存在！\n");
        PauseAndClear();
        return;
    }
    printf("身份证号：%s\n", pat->idcard);
    printf("患者姓名：%s\n", pat->name);
    printf("性别：%s\n", pat->gender == 'M' ? "男" : "女");
    printf("住院状态：%s\n", pat->isHospitalized ? "已住院" : "未住院");
    if (pat->isHospitalized) {
        // 查找床位信息
        BedNode* bed = bedHead;
        while (bed && bed->bedId != pat->bedId) bed = bed->next;
        if (bed) {
            char* roomTypeStr;
            if (bed->roomType == 0) roomTypeStr = "双人间";
            else if (bed->roomType == 1) roomTypeStr = "单人间";
            else roomTypeStr = "ICU";
     
            printf("住院床位：%d号（%s，%s）\n",
                pat->bedId,
                bed->deptId == 0 ? "重症监护室" : depts[bed->deptId - 1].deptName,
                roomTypeStr);
            char inDateStr[20];
            PrintDate(pat->inDate, inDateStr);
            printf("入院日期：%s\n", inDateStr);
        }
    }
    printf("简易病历：%s\n", pat->medicalRecord[0] ? pat->medicalRecord : "暂无病历");
    printf("=======================================================\n");
    PauseAndClear();
}

void PatientCheckCase(Loginf* user) {
    system("cls");
    printf("===================== 我的病情/病例 =====================\n");
    int hasCase = 0;
    CaseNode* p = caseHead;
    while (p != NULL) {
        if (strcmp(p->patientId, user->idcard) == 0) {
            hasCase++;
            DoctorNode* doc = FindDoctorById(p->doctorId);
            char docName[NAME_LENGTH] = "停诊";
            if (doc) strcpy_s(docName, NAME_LENGTH, doc->name);
            char diagDateStr[20], inDateStr[20], outDateStr[20];
            PrintDate(p->diagDate, diagDateStr);
            PrintDate(p->inDate, inDateStr);
            PrintDate(p->outDate, outDateStr);

            printf("病例编号：%d | 诊断医生：%s\n", hasCase, docName);
            printf("诊断日期：%s | 病情描述：%s\n", diagDateStr, p->illness);
            printf("住院状态：%s\n", p->bedId == 0 ? "无需住院" : "已安排住院");
            if (p->bedId != 0) {
                BedNode* bed = bedHead;
                while (bed && bed->bedId != p->bedId) bed = bed->next;
                if (bed) {
                    char* roomTypeStr;
                    if (bed->roomType == 0) roomTypeStr = "双人间";
                    else if (bed->roomType == 1) roomTypeStr = "单人间";
                    else roomTypeStr = "ICU";
                    printf("住院床位：%d号（%s，%s） | 入院日期：%s | 出院日期：%s\n",
                        p->bedId,
                        bed->deptId == 0 ? "ICU" : depts[bed->deptId - 1].deptName,
                        roomTypeStr,
                        inDateStr,
                        p->outDate.year == 0 ? "未出院" : outDateStr);
                }
            }
            printf("开具药品：");
            int hasMed = 0;
            for (int j = 0; j < 5; j++) {
                if (p->medId[j] != 0 && p->medNum[j] != 0) {
                    hasMed = 1;
                    MedicineNode* med = medicineHead;
                    while (med && med->medId != p->medId[j]) med = med->next;
                    if (med) printf("%s（%d盒）、", med->medName, p->medNum[j]);
                    else printf("未知药品（%d盒）、", p->medNum[j]);
                }
            }
            if (!hasMed) printf("暂无药品");
            else printf("\b\b  ");
            printf("\n=======================================================\n");
        }
        p = p->next;
    }
    if (!hasCase) printf("暂无病例记录！\n");
    PauseAndClear();
}

void PatientHospital(Loginf* user) {
    PatientNode* pat = FindPatientById(user->idcard);
    if (pat == NULL) {
        printf("患者信息不存在！\n");
        PauseAndClear();
        return;
    }
    system("cls");
    printf("===================== 住院业务办理 =====================\n");
    if (!pat->isHospitalized) {
        printf("当前状态：未住院 | 各科室空闲床位信息：\n");
        printf("-------------------------------------------------------\n");
        for (int d = NEIKE; d <= GUKE; d++) {
            printf("%s：总床位%d张（双人间4床+单人间4床） | 空闲%d张\n",
                depts[d - 1].deptName, depts[d - 1].totalBed, depts[d - 1].freeBed);
        }
        int icuFree = 0;
        BedNode* bed = bedHead;
        while (bed) {
            if (bed->roomType == 2 && bed->isFree) icuFree++;
            bed = bed->next;
        }
        printf("全院共用ICU病房：%d张空闲\n", icuFree);
        printf("=======================================================\n");
        printf("住院需由医生开具住院证明后分配床位，不可自行办理！\n");
    }
    else {
        printf("当前状态：已住院\n");
        BedNode* bed = bedHead;
        while (bed && bed->bedId != pat->bedId) bed = bed->next;
        if (bed) {
            char* roomTypeStr;
            if (bed->roomType == 0) roomTypeStr = "双人间";
            else if (bed->roomType == 1) roomTypeStr = "单人间";
            else roomTypeStr = "ICU";
            printf("住院床位：%d号（%s，%s）\n",
                pat->bedId,
                bed->deptId == 0 ? "ICU" : depts[bed->deptId - 1].deptName,
                roomTypeStr);
            char inDateStr[20];
            PrintDate(pat->inDate, inDateStr);
            printf("入院日期：%s\n", inDateStr);
        }
        printf("=======================================================\n");
        printf("是否办理出院？（Y/N）：");
        char ch = _getch();
        if (ch == 'Y' || ch == 'y') {
            pat->isHospitalized = 0;
            pat->outDate = GetCurrentDate();
            FreeBed(pat->bedId);
            CaseNode* c = caseHead;
            while (c) {
                if (strcmp(c->patientId, user->idcard) == 0 && c->bedId == pat->bedId) {
                    c->outDate = GetCurrentDate();
                }
                c = c->next;
            }
            SavePatientToFile();
            SaveCaseToFile();
            printf("出院办理成功！\n");
        }
        else {
            printf("取消出院办理！\n");
        }
    }
    PauseAndClear();
}

void PatientMenu(Loginf* user) {
    int choice = 0;
    while (1) {
        system("cls");
        PrintHospitalBanner();
        printf("===================== [患者功能菜单] =====================\n");
        printf("当前用户：%s（患者）\n", user->idcard);
        printf("1. 7天预约挂号    2. 当日挂号    3. 查看已挂号\n");
        printf("4. 查看费用账单    5. 个人信息    6. 病情/病例查看\n");
        printf("7. 住院业务办理    8. 修改密码    0. 退出菜单\n");  
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
        case 8: ChangePassword(user); break;         
        case 0: if (ExitConfirm()) return; break;
        default: printf("输入错误！请输入0-8的数字！\n"); PauseAndClear();
        }
    }
}

// ===================== 医生子功能实现 =====================

void DoctorCheckInfo(Loginf* user) {
    DoctorNode* doc = FindDoctorById(user->idcard);
    system("cls");
    printf("===================== 我的医生信息 =====================\n");
    if (doc == NULL) {
        printf("医生信息不存在！\n");
        PauseAndClear();
        return;
    }
    printf("身份证号：%s\n", doc->idcard);
    printf("医生姓名：%s\n", doc->name);
    printf("所属科室：%s\n", depts[doc->deptId - 1].deptName);
    printf("职称：%s\n", doc->title);
    printf("擅长领域：%s\n", doc->specialty);
    printf("医生简介：%s\n", doc->info);
    printf("=======================================================\n");
    printf("我的值班安排：\n");
    DutyNode* duty = dutyHead;
    while (duty) {
        if (strcmp(duty->doctorId, user->idcard) == 0) {
            printf("【%s】周%d值班\n", depts[duty->deptId - 1].deptName, duty->weekDay);
        }
        duty = duty->next;
    }
    printf("=======================================================\n");
    PauseAndClear();
}

void DoctorCheckPatient(Loginf* user) {
    DoctorNode* doc = FindDoctorById(user->idcard);
    if (doc == NULL) return;

    system("cls");

    // 简洁风格分割线
    printf("╔══════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                【 我 的 病 人 列 表 】                                               ║\n");
    printf("║ 当前医生：%10s  %10s                                                                     ║\n", doc->name, depts[doc->deptId - 1].deptName);
    printf("╚══════════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n");

    // 收集所有符合条件的挂号记录指针
    RegNode* regArray[MAX_REG];  // 假设最大挂号数不超过 MAX_REG
    int count = 0;
    RegNode* reg = regHead;
    Date date = GetCurrentDate();
   
    while (reg) {
        if (strcmp(reg->doctorId, user->idcard) == 0 && (reg->isToday == 1 || reg->isToday == 0) && (reg->appointDate.year * 10000 +reg->appointDate.month * 100 +reg->appointDate.day >= date.year * 10000 +date.month * 100 + date.day)) {
            if (count < MAX_REG) {
                regArray[count++] = reg;
            }
        }
        reg = reg->next;
    }

    // 按预约日期升序，同日期按挂号ID升序（体现挂号顺序）
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - 1 - i; j++) {
            RegNode* a = regArray[j];
            RegNode* b = regArray[j + 1];
            // 比较预约日期
            if (a->appointDate.year != b->appointDate.year) {
                if (a->appointDate.year > b->appointDate.year) {
                    RegNode* tmp = regArray[j];
                    regArray[j] = regArray[j + 1];
                    regArray[j + 1] = tmp;
                }
            }
            else if (a->appointDate.month != b->appointDate.month) {
                if (a->appointDate.month > b->appointDate.month) {
                    RegNode* tmp = regArray[j];
                    regArray[j] = regArray[j + 1];
                    regArray[j + 1] = tmp;
                }
            }
            else if (a->appointDate.day != b->appointDate.day) {
                if (a->appointDate.day > b->appointDate.day) {
                    RegNode* tmp = regArray[j];
                    regArray[j] = regArray[j + 1];
                    regArray[j + 1] = tmp;
                }
            }
            else {
                // 同日期按挂号ID升序
                if (a->regId > b->regId) {
                    RegNode* tmp = regArray[j];
                    regArray[j] = regArray[j + 1];
                    regArray[j + 1] = tmp;
                }
            }
        }
    }
    // 表格区域
    printf("╔══════════════╦══════════════╦══════════════════════╦══════════════╦═══════════════╗\n");
    printf("║ %-12s │ %-12s │ %-20s │ %-12s │ %-12s  ║\n",
        "挂号编号", "患者姓名", "身份证号", "挂号类型", "预约日期");
    printf("╠══════════════╬══════════════╬══════════════════════╬══════════════╬═══════════════╣\n");

    if (count == 0) {
        printf("║ %-74s ║\n", "暂无挂您号的患者！");
    }
    else {
        for (int i = 0; i < count; i++) {
            RegNode* r = regArray[i];
            PatientNode* pat = FindPatientById(r->patientId);
            char patName[NAME_LENGTH] = "未知";
            if (pat) strcpy_s(patName, NAME_LENGTH, pat->name);
            char regType[10];
            strcpy_s(regType, 10, r->isToday ? "当日" : "预约");
            char appointDateStr[20];
            PrintDate(r->appointDate, appointDateStr);

            printf("║ %-12d │ %-12s │ %-20s │ %-12s │ %-12s ║\n",
                i + 1, patName, r->patientId, regType, appointDateStr);
        }
    }

    printf("╚══════════════╩══════════════╩══════════════════════╩══════════════╩═══════════════╝\n");

    PauseAndClear();
}

void DoctorFindPatient(Loginf* user) {
    char patId[ID_LENGTH] = { 0 };
    system("cls");
    printf("===================== 查询病人信息 =====================\n");
    printf("请输入患者身份证号：");
    scanf_s("%s", patId, (unsigned)_countof(patId));
    PatientNode* pat = FindPatientById(patId);
    if (pat == NULL) {
        printf("该患者信息不存在！\n");
        PauseAndClear();
        return;
    }
    printf("=======================================================\n");
    printf("患者身份证：%s\n", pat->idcard);
    printf("患者姓名：%s\n", pat->name);
    printf("性别：%s\n", pat->gender == 'M' ? "男" : "女");
    printf("住院状态：%s\n", pat->isHospitalized ? "已住院" : "未住院");
    if (pat->isHospitalized) {
        BedNode* bed = bedHead;
        while (bed && bed->bedId != pat->bedId) bed = bed->next;
        if (bed) {
            printf("住院床位：%d号（%s）\n", pat->bedId,
                bed->deptId == 0 ? "ICU" : depts[bed->deptId - 1].deptName);
            char inDateStr[20];
            PrintDate(pat->inDate, inDateStr);
            printf("入院日期：%s\n", inDateStr);
        }
    }
    printf("=======================================================\n");
    printf("该患者挂号记录：\n");
    int hasReg = 0;
    RegNode* reg = regHead;
    while (reg) {
        if (strcmp(reg->patientId, patId) == 0 && (reg->isToday == 1 || reg->isToday == 0)) {
            hasReg++;
            DoctorNode* doc = FindDoctorById(reg->doctorId);
            char docName[NAME_LENGTH] = "停诊";
            if (doc) strcpy_s(docName, NAME_LENGTH, doc->name);
            char regDateStr[20];
            PrintDate(reg->regDate, regDateStr);
            printf("编号：%d | 医生：%s | 日期：%s |\n",
                hasReg, docName, regDateStr);
        }
        reg = reg->next;
    }
    if (!hasReg) printf("暂无挂号记录！\n");
    printf("=======================================================\n");
    printf("该患者病例记录：\n");
    int hasCase = 0;
    CaseNode* c = caseHead;
    while (c) {
        if (strcmp(c->patientId, patId) == 0) {
            hasCase++;
            char diagDateStr[20];
            PrintDate(c->diagDate, diagDateStr);
            if (c->bedId == 0) {
                printf("编号：%d | 诊断日期：%s | 病情：%s | 未住院\n",
                    hasCase, diagDateStr, c->illness);
            }
            else {
                printf("编号：%d | 诊断日期：%s | 病情：%s | 床位：%d\n",
                    hasCase, diagDateStr, c->illness, c->bedId);
            }
        }
        c = c->next;
    }
    if (!hasCase) printf("暂无病例记录！\n");
    printf("=======================================================\n");
    PauseAndClear();
}

void DoctorWriteCase(Loginf* user) {
    char patId[ID_LENGTH] = { 0 };
    system("cls");
    printf("===================== 写/修改患者病例 =====================\n");
    printf("请输入患者身份证号：");
    scanf_s("%s", patId, (unsigned)_countof(patId));
    PatientNode* pat = FindPatientById(patId);
    DoctorNode* doc = FindDoctorById(user->idcard);
    if (pat == NULL || doc == NULL) {
        printf("患者信息不存在！\n");
        PauseAndClear();
        return;
    }
    printf("当前患者：%s\n", pat->name);
    Date now = GetCurrentDate();
    char nowDateStr[20];
    PrintDate(now, nowDateStr);

    CaseNode* c = NULL;
    //while (c->next!=NULL) {
    //  //  if (strcmp(c->patientId, patId) == 0 && strcmp(c->doctorId, user->idcard) == 0) break;
    //    c = c->next;
    //}


    PatientNode* p = patientHead;
    while (p)
    {
        if (strcmp(p->idcard, patId) == 0) break;
        p = p->next;
    }

    if (c == NULL) {
        c = (CaseNode*)malloc(sizeof(CaseNode));
        memset(c, 0, sizeof(CaseNode));
        c->caseId = caseCnt + 1;
        strcpy_s(c->patientId, ID_LENGTH, patId);
        strcpy_s(c->doctorId, ID_LENGTH, user->idcard);
        c->diagDate = now;
        c->next = NULL;
        if (caseHead == NULL) caseHead = c;
        else {
            CaseNode* tail = caseHead;
            while (tail->next) tail = tail->next;
            tail->next = c;
        }
        caseCnt++;
    }

    printf("\n=======================================================\n");
    printf("当前诊断日期：%s\n", nowDateStr);
    c->illness[0] = 0;
    while (getchar() != '\n');
    while (c->illness[0] == 0) {
        printf("请输入患者病情描述：");

        gets_s(c->illness, MAX_INFO);
        if (c->illness[0] != 0) break;
        else
        {
            printf("病情描述不能为空！\n\n");
        }
    }

    strcpy_s(p->medicalRecord, MAX_INFO, c->illness);

    char ch;
    printf("\n=======================================================\n");
    while (1) {
        printf("是否为患者开具药品？（Y/N）：\n");
        scanf_s(" %c", &ch, 1);
        if (ch == 'Y' || ch == 'y' || ch == 'N' || ch == 'n') break;
        else {
            while (getchar() != '\n');
            printf("输入错误！请输入Y或N！\n");
        }
    }
    if (ch == 'Y' || ch == 'y') {
        printf("\n当前科室：%s | 可开具本科室绑定药品（6类）\n", depts[doc->deptId - 1].deptName);
        printf("本科室药品列表：\n");
        printf("%-6s %-18s %-12s %-8s %-8s %-6s\n", "编号", "名称", "别名", "类型", "单价", "库存");
        printf("-------------------------------------------------------\n");
        MedicineNode* med = medicineHead;
        while (med) {
            if (med->bindDept == doc->deptId) {
                char medType[10];
                strcpy_s(medType, 10, med->medType == PRESCRIPTION ? "Rx" : "OTC");
                printf("%-6d %-18s %-12s %-10s %-8.2lf %-6d\n",
                    med->medId, med->medName, med->medAlias, medType, med->price, med->stock);
            }
            med = med->next;
        }
        for (int i = 0; i < 5; i++)
        {
            c->medId[i] = 0;
            c->medNum[i] = 0;
        }
        int medNum = 0;
        while (medNum < 5) {
            printf("请输入药品编号（输入0结束）：");
            int medIdSel;
            // 循环直到正确输入整数
            while (scanf_s("%d", &medIdSel) != 1) {
                // 清除输入流中的错误标志和缓冲区
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF) {}
                printf("输入无效，请输入数字编号！\n");
                printf("请输入药品编号（输入0结束）：");
            }
            if (medIdSel == 0) break;

            MedicineNode* target = medicineHead;
            while (target && target->medId != medIdSel) target = target->next;
            if (target == NULL || target->bindDept != doc->deptId) {
                printf("药品编号错误/非本科室药品！\n");
                continue;
            }

            printf("请输入药品数量（盒）：");
            int numSel;
            while (scanf_s("%d", &numSel) != 1) {
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF) {}
                printf("输入无效，请输入正整数！\n");
                printf("请输入药品数量（盒）：");
            }
            if (numSel <= 0 || !ReduceMedStock(medIdSel, numSel)) {
                printf("数量错误/库存不足！\n");
                continue;
            }

            c->medId[medNum] = medIdSel;
            c->medNum[medNum] = numSel;
            medNum++;
            printf("成功开具%s %d盒！\n", target->medName, numSel);

            RegNode* newReg = (RegNode*)malloc(sizeof(RegNode));
            memset(newReg, 0, sizeof(RegNode));
            newReg->regId = regCnt + 1;
            strcpy_s(newReg->patientId, ID_LENGTH, patId);
            newReg->deptId = target->bindDept;
            strcpy_s(newReg->doctorId, ID_LENGTH, user->idcard);
            newReg->fee = target->price * numSel;
            newReg->isToday = 2;
            newReg->regDate = now;
            newReg->appointDate = now;
            newReg->next = NULL;
            if (regHead == NULL) regHead = newReg;
            else {
                RegNode* tail = regHead;
                while (tail->next) tail = tail->next;
                tail->next = newReg;
            }
            regCnt++;
            SaveRegToFile();
        }
    }

    printf("\n=======================================================\n");
    while (1) {
        printf("是否为患者开具检查？（Y/N）：\n");
        scanf_s(" %c", &ch, 1);
        if (ch == 'Y' || ch == 'y' || ch == 'N' || ch == 'n') break;
        else {
            while (getchar() != '\n');
            printf("输入错误！请输入Y或N！\n");
        }
    }
    if (ch == 'Y' || ch == 'y') {
        const char* examTypes[] = { "血液化验", "X光", "B超", "CT", "心电图" };
        const float examCosts[] = { 50.0f, 100.0f, 80.0f, 500.0f, 60.0f };
        const int examTypeCount = sizeof(examTypes) / sizeof(examTypes[0]);

        while (1) {
            printf("\n可选检查类型：\n");
            for (int i = 0; i < examTypeCount; i++) {
                printf("%d. %s （%.2f元）\n", i + 1, examTypes[i], examCosts[i]);
            }
            printf("0. 结束开具检查\n");
            printf("请输入检查编号：");
            int sel;
            if (scanf_s("%d", &sel) != 1) {
                while (getchar() != '\n');
                printf("检查编号错误!\n");
                continue;
            }
            if (sel == 0) break;
            if (sel < 1 || sel > examTypeCount) { printf("检查编号错误!\n"); continue; }

            ExamNode* newExam = (ExamNode*)malloc(sizeof(ExamNode));
            memset(newExam, 0, sizeof(ExamNode));
            newExam->examId = examCnt + 1;
            strcpy_s(newExam->patientId, ID_LENGTH, patId);
            strcpy_s(newExam->doctorId, ID_LENGTH, user->idcard);
            strcpy_s(newExam->examType, NAME_LENGTH, examTypes[sel - 1]);
            newExam->cost = examCosts[sel - 1];
            newExam->examDate = now;
            newExam->status = 0;
            newExam->next = NULL;
            if (examHead == NULL) examHead = newExam;
            else {
                ExamNode* tail = examHead;
                while (tail->next) tail = tail->next;
                tail->next = newExam;
            }
            examCnt++;
            SaveExamToFile();

            RegNode* newReg = (RegNode*)malloc(sizeof(RegNode));
            memset(newReg, 0, sizeof(RegNode));
            newReg->regId = regCnt + 1;
            strcpy_s(newReg->patientId, ID_LENGTH, patId);
            newReg->deptId = doc->deptId;
            strcpy_s(newReg->doctorId, ID_LENGTH, user->idcard);
            newReg->fee = newExam->cost;
            newReg->isToday = 3;
            newReg->regDate = now;
            newReg->appointDate = now;
            newReg->next = NULL;
            if (regHead == NULL) regHead = newReg;
            else {
                RegNode* tail = regHead;
                while (tail->next) tail = tail->next;
                tail->next = newReg;
            }
            regCnt++;
            SaveRegToFile();
            printf("成功开具检查！");
        }
    }






    printf("\n=======================================================\n");
    while (1) {
        printf("是否为患者安排住院？（Y/N）：\n");
        scanf_s(" %c", &ch, 1);
        if (ch == 'Y' || ch == 'y' || ch == 'N' || ch == 'n') break;
        else {
            while (getchar() != '\n');
            printf("输入错误！请输入Y或N！\n");
        }
    }
    if (ch == 'Y' || ch == 'y') {
        int freeBed = FindFreeBed(doc->deptId);
        if (freeBed == -1) {
            printf("本科室无空闲床位！无法安排住院！\n");
        }
        else {
            if (pat->isHospitalized == 1)
            {
                printf("该患者已住院，请先释放所在病床！");
                system("pause");
                return;
            }
            printf("\n┌────────────┬────────────┬──────────────────┐\n");
            printf("│  床位号    │    状态    │    房间类型      │\n");
            printf("├────────────┼────────────┼──────────────────┤\n");
            BedNode* bed = bedHead;
            int foundAny = 0;
            while (bed) {
                if (bed->deptId == doc->deptId) {
                    char* roomTypeStr;
                    if (bed->roomType == 0) roomTypeStr = "双人间";
                    else if (bed->roomType == 1) roomTypeStr = "单人间";
                    else roomTypeStr = "ICU";
                    if (bed->isFree) {
                        printf("│  %-9d │ \033[1;32m空闲 \033[0m      │ %-16s │\n", bed->bedId, roomTypeStr);
                        foundAny = 1;
                    }
                    else {
                        printf("│  %-9d │ \033[1;31m占用 \033[0m      │ %-16s │\n", bed->bedId, roomTypeStr);
                    }
                }
                bed = bed->next;
            }
            printf("└────────────┴────────────┴──────────────────┘\n");

            if (!foundAny) {
                printf("提示：扫描到没有空闲床位。\n");
            }
            else {
                int selBed;
                printf("请输入要分配的床位号（输入0取消）：");
                while (1) {
                    if (scanf_s("%d", &selBed) != 1) {
                        while (getchar() != '\n');
                        printf("输入错误，请重新输入：");
                        continue;
                    }
                    if (selBed == 0) { printf("取消住院安排！\n"); break; }
                    BedNode* targetBed = bedHead;
                    while (targetBed && targetBed->bedId != selBed) targetBed = targetBed->next;
                    if (targetBed == NULL || targetBed->deptId != doc->deptId || !targetBed->isFree) {
                        printf("床位号错误或已被占用，请重新输入：");
                        continue;
                    }
                    c->bedId = selBed;
                    c->inDate = now;
                    pat->isHospitalized = 1;
                    pat->bedId = selBed;
                    pat->inDate = now;
                    targetBed->isFree = 0;
                    strcpy_s(targetBed->patientId, ID_LENGTH, patId);
                    targetBed->inDate = now;
                    depts[doc->deptId - 1].freeBed--;

                    RegNode* newReg = (RegNode*)malloc(sizeof(RegNode));
                    memset(newReg, 0, sizeof(RegNode));
                    newReg->regId = regCnt + 1;
                    strcpy_s(newReg->patientId, ID_LENGTH, patId);
                    newReg->deptId = doc->deptId;
                    strcpy_s(newReg->doctorId, ID_LENGTH, user->idcard);
                    // 根据床位类型设置住院费用
                    float bedFee = 0;
                    if (targetBed->roomType == 0) bedFee = BED_FEE_DOUBLE;      // 双人间 80元
                    else if (targetBed->roomType == 1) bedFee = BED_FEE_SINGLE; // 单人间 150元
                    else bedFee = BED_FEE_ICU;                                   // ICU 360元
                    newReg->fee = bedFee;
                    newReg->isToday = 4;
                    newReg->regDate = now;
                    newReg->appointDate = now;
                    newReg->next = NULL;
                    if (regHead == NULL) regHead = newReg;
                    else {
                        RegNode* tail = regHead;
                        while (tail->next) tail = tail->next;
                        tail->next = newReg;
                    }
                    regCnt++;
                    SaveRegToFile();
                    printf("成功为患者安排住院！床位号：%d，费用：%.1f元\n", selBed, bedFee);
                    break;
                }
            }
            SaveCaseToFile();
            SavePatientToFile();
            SaveBedToFile();
            printf("病例写/修改成功！\n");
            PauseAndClear();
            return;
        }
    }
    SaveCaseToFile();
    SavePatientToFile();
    printf("病例写/修改成功！\n");
    PauseAndClear();
}
void DoctorAssignBed(Loginf* user) {
    DoctorNode* doc = FindDoctorById(user->idcard);
    if (doc == NULL) {
        printf("医生信息不存在！\n");
        PauseAndClear();
        return;
    }
    system("cls");
    printf("===================== 床位分配/释放 =====================\n");
    printf("当前科室：%s | 总床位%d张 | 空闲%d张\n",
        depts[doc->deptId - 1].deptName,
        depts[doc->deptId - 1].totalBed,
        depts[doc->deptId - 1].freeBed);
    printProgressBar(depts[doc->deptId - 1].freeBed, depts[doc->deptId - 1].totalBed);
 

    printf("┌────────────┬────────────┬──────────────────┬──────────────┐\n");
    printf("│  床位号    │    状态    │    房间类型      │   所属科室   │\n");
    printf("├────────────┼────────────┼──────────────────┼──────────────┤\n");

    BedNode* bed = bedHead;
    int hasReleaseable = 0;
    while (bed) {
        // 只显示本科室床位或ICU床位
        if (bed->deptId == doc->deptId || bed->deptId == 0) {
            hasReleaseable = 1;
            char* roomTypeStr;
            if (bed->roomType == 0) roomTypeStr = "双人间";
            else if (bed->roomType == 1) roomTypeStr = "单人间";
            else roomTypeStr = "ICU";
            char* deptNameStr = (bed->deptId == 0) ? "ICU" : depts[bed->deptId - 1].deptName;
            if (bed->isFree) {
                printf("│  %-9d │ \033[1;32m空闲 \033[0m      │ %-16s │ %-12s │\n", bed->bedId, roomTypeStr, deptNameStr);
            }
            else {
                printf("│  %-9d │ \033[1;31m占用 \033[0m      │ %-16s │ %-12s │\n", bed->bedId, roomTypeStr, deptNameStr);
            }
        }
        bed = bed->next;
    }
    printf("└────────────┴────────────┴──────────────────┴──────────────┘\n");
    int choice;
    printf("请选择操作（1-分配普通床位 2-释放床位 3-分配ICU床位 0-取消）：");
    scanf_s("%d", &choice);

    if (choice == 1) {
        char patId[ID_LENGTH];
        printf("请输入患者身份证号：");
        scanf_s("%s", patId, (unsigned)_countof(patId));
        PatientNode* pat = FindPatientById(patId);
        if (pat == NULL || pat->isHospitalized) {
            printf("患者信息不存在或已住院！\n");
            PauseAndClear();
            return;
        }
        int freeBed = FindFreeBed(doc->deptId);
        if (freeBed == -1) {
            printf("本科室无空闲床位！\n");
            PauseAndClear();
            return;
        }
        BedNode* targetBed = bedHead;
        while (targetBed && targetBed->bedId != freeBed) targetBed = targetBed->next;
        if (targetBed) {
            Date now = GetCurrentDate();
            pat->isHospitalized = 1;
            pat->bedId = freeBed;
            pat->inDate = now;
            targetBed->isFree = 0;
            strcpy_s(targetBed->patientId, ID_LENGTH, patId);
            targetBed->inDate = now;
            depts[doc->deptId - 1].freeBed--;
            SavePatientToFile();
            SaveBedToFile();

            RegNode* newReg = (RegNode*)malloc(sizeof(RegNode));
            memset(newReg, 0, sizeof(RegNode));
            newReg->regId = regCnt + 1;
            strcpy_s(newReg->patientId, ID_LENGTH, patId);
            newReg->deptId = doc->deptId;
            strcpy_s(newReg->doctorId, ID_LENGTH, user->idcard);
            // 根据床位类型设置住院费用
            float bedFee = 0;
            if (targetBed->roomType == 0) bedFee = BED_FEE_DOUBLE;      // 双人间 80元
            else if (targetBed->roomType == 1) bedFee = BED_FEE_SINGLE; // 单人间 150元
            else bedFee = BED_FEE_ICU;                                   // ICU 360元
            newReg->fee = bedFee;
            newReg->isToday = 4;
            newReg->regDate = now;
            newReg->appointDate = now;
            newReg->next = NULL;
            if (regHead == NULL) regHead = newReg;
            else {
                RegNode* tail = regHead;
                while (tail->next) tail = tail->next;
                tail->next = newReg;
            }
            regCnt++;
            SaveRegToFile();
            printf("床位分配成功！患者：%s | 床位号：%d | 费用：%.1f元\n", pat->name, freeBed, bedFee);
        }
    }
    else if (choice == 2) {
        system("cls");
        printf("===================== 释放床位 =====================\n");
        printf("可释放床位范围：本科室床位 + ICU病房床位\n");
        printf("┌────────────┬────────────┬──────────────────┬──────────────┐\n");
        printf("│  床位号    │    状态    │    房间类型      │   所属科室   │\n");
        printf("├────────────┼────────────┼──────────────────┼──────────────┤\n");

        BedNode* bed = bedHead;
        int hasReleaseable = 0;
        while (bed) {
            // 只显示本科室床位或ICU床位
            if (bed->deptId == doc->deptId || bed->deptId == 0) {
                hasReleaseable = 1;
                char* roomTypeStr;
                if (bed->roomType == 0) roomTypeStr = "双人间";
                else if (bed->roomType == 1) roomTypeStr = "单人间";
                else roomTypeStr = "ICU";
                char* deptNameStr = (bed->deptId == 0) ? "ICU" : depts[bed->deptId - 1].deptName;
                if (bed->isFree) {
                    printf("│  %-9d │ \033[1;32m空闲 \033[0m      │ %-16s │ %-12s │\n", bed->bedId, roomTypeStr, deptNameStr);
                }
                else {
                    printf("│  %-9d │ \033[1;31m占用 \033[0m      │ %-16s │ %-12s │\n", bed->bedId, roomTypeStr, deptNameStr);
                }
            }
            bed = bed->next;
        }
        printf("└────────────┴────────────┴──────────────────┴──────────────┘\n");
        if (!hasReleaseable) {
            printf("当前没有可释放的床位（本科室及ICU均无记录）！\n");
            PauseAndClear();
            return;
        }

        int bedSel;
        printf("请输入要释放的床位号（输入0取消）：");
        scanf_s("%d", &bedSel);
        if (bedSel == 0) {
            printf("取消释放操作！\n");
            PauseAndClear();
            return;
        }

        BedNode* targetBed = bedHead;
        while (targetBed && targetBed->bedId != bedSel) targetBed = targetBed->next;
        if (targetBed == NULL || targetBed->isFree) {
            PrintError("床位号错误或已空闲！\n");
            PauseAndClear();
            return;
        }
        FreeBed(bedSel);
        PatientNode* pat = patientHead;
        while (pat) {
            if (pat->bedId == bedSel) {
                pat->isHospitalized = 0;
                pat->outDate = GetCurrentDate();
                pat->bedId = 0;
                break;
            }
            pat = pat->next;
        }
        SavePatientToFile();
        printf("床位释放成功！\n");
    }
    else if (choice == 3) {
        int icuBed = FindFreeIcuBed();
        if (icuBed == -1) {
            printf("暂无空闲ICU床位！\n");
            PauseAndClear();
            return;
        }
        char patId[ID_LENGTH];
        printf("请输入患者身份证号：");
        scanf_s("%s", patId, (unsigned)_countof(patId));
        PatientNode* pat = FindPatientById(patId);
        if (pat == NULL || pat->isHospitalized) {
            printf("患者信息不存在或已住院！\n");
            PauseAndClear();
            return;
        }
        BedNode* targetBed = bedHead;
        while (targetBed && targetBed->bedId != icuBed) targetBed = targetBed->next;
        if (targetBed) {
            Date now = GetCurrentDate();
            pat->isHospitalized = 1;
            pat->bedId = icuBed;
            pat->inDate = now;
            targetBed->isFree = 0;
            strcpy_s(targetBed->patientId, ID_LENGTH, patId);
            targetBed->inDate = now;
            SavePatientToFile();
            SaveBedToFile();

            RegNode* newReg = (RegNode*)malloc(sizeof(RegNode));
            memset(newReg, 0, sizeof(RegNode));
            newReg->regId = regCnt + 1;
            strcpy_s(newReg->patientId, ID_LENGTH, patId);
            newReg->deptId = doc->deptId;
            strcpy_s(newReg->doctorId, ID_LENGTH, user->idcard);
            newReg->fee = BED_FEE_ICU;  // ICU费用 360元
            newReg->isToday = 4;
            newReg->regDate = now;
            newReg->appointDate = now;
            newReg->next = NULL;
            if (regHead == NULL) regHead = newReg;
            else {
                RegNode* tail = regHead;
                while (tail->next) tail = tail->next;
                tail->next = newReg;
            }
            regCnt++;
            SaveRegToFile();
            printf("ICU床位分配成功！床位号：%d，费用：%.1f元\n", icuBed, BED_FEE_ICU);
        }
    }
    else if (choice == 0);
    else
    {
        printf("输入错误!");
        }
    PauseAndClear();
}

void DoctorMenu(Loginf* user) {
    int choice = 0;
    while (1) {
        system("cls");
        PrintHospitalBanner();
        SetColor(10);
        printf("===================== 医生功能菜单 =====================\n");
        DoctorNode* doc = FindDoctorById(user->idcard);
        if (doc) {
            printf("当前用户：%s（%s医生）\n", user->idcard, depts[doc->deptId - 1].deptName);
        }
        printf("1. 个人信息查询    2. 我的病人列表    3. 查找病人信息\n");
        printf("4. 写/修改病例     5. 床位分配/释放    6. 修改密码\n"); 
        printf("0. 退出菜单\n");
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
        case 6: ChangePassword(user); break;
        case 0: if (ExitConfirm()) return; break;
        default: printf("输入错误！请输入0-6的数字！\n"); PauseAndClear();
        }
    }
}

// ===================== 管理员子功能实现 =====================

void AdminCheckMed(Loginf* user) {
    system("cls");

    printf("╔══════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                    药 品 列 表                                                       ║\n");
    printf("╚══════════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n");

    printf("╔════════════╦══════════════════════╦══════════════════════╦════════════╦════════╦════════╦════════╗\n");
    printf("║ 编号       │ 药品名称             │ 别名                 │ 科室       │ 类型   │ 单价   │ 库存   ║\n");
    printf("╠════════════╬══════════════════════╬══════════════════════╬════════════╬════════╬════════╬════════╣\n");

    MedicineNode* p = medicineHead;
    int hasData = 0;
    int count = 0;

    while (p) {
        hasData = 1;
        count++;

        char deptName[NAME_LENGTH];
        strcpy_s(deptName, NAME_LENGTH, depts[p->bindDept - 1].deptName);

        char medType[10];
        if (p->medType == PRESCRIPTION) {
            strcpy_s(medType, 10, "Rx");
        }
        else {
            strcpy_s(medType, 10, "OTC");
        }

        printf("║ %-10d │ %-20s │ %-20s │ %-10s │ %-6s │ %-6.1f │ %-6d ║\n",
            p->medId,
            p->medName,
            p->medAlias,
            deptName,
            medType,
            p->price,
            p->stock
        );

        p = p->next;
    }

    if (!hasData) {
        printf("║ %-96s ║\n", "暂无药品信息！");
    }

    printf("╚════════════╩══════════════════════╩══════════════════════╩════════════╩════════╩════════╩════════╝\n");
    printf("总计药品：%d 种\n", count);

    PauseAndClear();
}

void AdminEditStock(Loginf* user) {
    system("cls");
    printf("===================== 药品库存修改 =====================\n");
    printf("当前操作：药品入库/出库 | 输入正数=入库，负数=出库\n");
    printf("=======================================================\n");
    int medIdSel;
    printf("请输入药品编号：");
    scanf_s("%d", &medIdSel);
    MedicineNode* p = medicineHead;
    while (p && p->medId != medIdSel) p = p->next;
    if (p == NULL) {
        PrintError("药品编号错误！\n");
        PauseAndClear();
        return;
    }
    printf("=======================================================\n");
    printf("药品信息：\n编号：%d | 名称：%s | 别名：%s | 科室：%s\n",
        p->medId, p->medName, p->medAlias, depts[p->bindDept - 1].deptName);
    printf("当前库存：%d盒 | 单价：%.1f元\n", p->stock, p->price);
    printf("=======================================================\n");
    int numSel;
    printf("请输入库存修改数量（正数入库/负数出库）：");
    scanf_s("%d", &numSel);
    if (numSel == 0 || (numSel < 0 && p->stock + numSel < 0) || p->stock + numSel > MAX_STOCK) {
        printf("修改数量无效或库存不足/超限！\n");
        PauseAndClear();
        return;
    }
    p->stock += numSel;
    SaveMedicineToFile();
    printf("库存修改成功！最新库存：%d盒\n", p->stock);
    PauseAndClear();
}

void AdminSearchMed(Loginf* user) {
    system("cls");
    printf("╔═════════════════════════════════════药 品 模 糊 检 索 ═══════════════════════════════════════════╗\n");
    char key[NAME_LENGTH];
    printf("请输入检索关键字：");
    // 清空缓冲区（因为可能有之前的换行符残留）
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
    // 使用 fgets 代替 gets_s
    if (fgets(key, sizeof(key), stdin) == NULL) {
        PrintError("读取输入失败！");
        PauseAndClear();
        return;
    }
    // 去掉末尾换行符
    size_t len = strlen(key);
    if (len > 0 && key[len - 1] == '\n') {
        key[len - 1] = '\0';
        len--;
    }
    // 检查是否为空输入
    if (len == 0) {
        PrintError("检索关键字不能为空！");
        PauseAndClear();
        return;
    }

    MedicineNode* res[MAX_MEDICINE];
    int resNum = FindMedicineByKey(key, res);
        printf("\n检索关键字：%s | 共找到 %d 类匹配药品\n\n", key, resNum);

        if (resNum == 0) {
            PauseAndClear();
            return;
        }

        printf("╔══════════════════════╦══════════════════════╦══════════════════════╦════════════╦═══════╦═══════╦═══════╗\n");
        printf("║ %-20s │ %-20s │ %-20s │ %-10s │ %-5s │ %-5s │ %-5s ║\n",
            "编号", "药品名称", "别名", "科室", "类型", "单价", "库存");
        printf("╠══════════════════════╬══════════════════════╬══════════════════════╬════════════╬═══════╬═══════╬═══════╣\n");

        for (int i = 0; i < resNum; i++) {
            MedicineNode* m = res[i];
            char deptName[NAME_LENGTH];
            strcpy_s(deptName, NAME_LENGTH, depts[m->bindDept - 1].deptName);
            char medType[10];
            strcpy_s(medType, 10, m->medType == PRESCRIPTION ? "Rx" : "OTC");

            printf("║ %-20d │ %-20s │ %-20s │ %-10s │ %-5s │ %-5.1f │ %-5d ║\n",
                m->medId, m->medName, m->medAlias, deptName, medType, m->price, m->stock);
        }

        printf("╚══════════════════════╩══════════════════════╩══════════════════════╩════════════╩═══════╩═══════╩═══════╝\n");
        PauseAndClear();
    

}

void AdminCheckStock(Loginf* user) {
    system("cls");

    printf("╔══════════════════════╦══════════════════════╦══════════════════════╦════════════╦═══════╦═══════╗\n");
    printf("║ %-20s │ %-20s │ %-20s │ %-10s │ %-5s │ %-5s ║\n",
        "编号", "药品名称", "别名", "科室", "库存", "补货");
    printf("╠══════════════════════╬══════════════════════╬══════════════════════╬════════════╬═══════╬═══════╣\n");

    int hasWarn = 0;
    MedicineNode* p = medicineHead;

    while (p) {
        if (p->stock < 20) {
            hasWarn = 1;

            char deptName[NAME_LENGTH];
            strcpy_s(deptName, NAME_LENGTH, depts[p->bindDept - 1].deptName);

            printf("║ %-20d │ %-20s │ %-20s │ %-10s │ %-5d │ %-5d ║\n",
                p->medId,
                p->medName,
                p->medAlias,
                deptName,
                p->stock,
                20 - p->stock
            );
        }
        p = p->next;
    }

    if (!hasWarn) {
        printf("║ %-92s ║\n", "暂无库存预警药品！");
    }

    printf("╚══════════════════════╩══════════════════════╩══════════════════════╩════════════╩═══════╩═══════╝\n");

    PauseAndClear();
    
}
void AdminArrangeDuty() {
    system("cls");
    printf("===================== 医生排班管理 =====================\n");
    char docId[ID_LENGTH];
    printf("请输入医生身份证号：");
    scanf_s("%s", docId, (unsigned)_countof(docId));
    DoctorNode* doc = FindDoctorById(docId);
    if (!doc) {
        printf("医生不存在！\n");
        PauseAndClear();
        return;
    }
    printf("医生：%s（%s）\n", doc->name, depts[doc->deptId - 1].deptName);
    printf("当前排班：\n");
    DutyNode* d = dutyHead;
    int has = 0;
    while (d) {
        if (strcmp(d->doctorId, docId) == 0) {
            printf("科室：%s 周%d\n", depts[d->deptId - 1].deptName, d->weekDay);
            has = 1;
        }
        d = d->next;
    }
    if (!has) printf("无\n");

    while (1) {
        printf("\n1. 新增/修改排班  2. 删除排班  0. 返回\n请选择：");
        int op = 0;
        scanf_s("%d", &op);
        if (op == 0) break;
        if (op == 1) {
            int dept, week;
            dept = depts[doc->deptId - 1].deptId;
            printf("输入周几（1-7）："); scanf_s("%d", &week);
            // 检查是否已存在
            DutyNode* found = NULL;
            for (d = dutyHead; d; d = d->next) {
                if (strcmp(d->doctorId, docId) == 0 && d->deptId == dept && d->weekDay == week) {
                    found = d; break;
                }
            }
            if (!found) {
                if (dept >= 1 && dept <= 5 && week >= 1 && week <= 7) {
                    DutyNode* newDuty = (DutyNode*)malloc(sizeof(DutyNode));
                    memset(newDuty, 0, sizeof(DutyNode));
                    strcpy_s(newDuty->doctorId, ID_LENGTH, docId);
                    newDuty->deptId = dept;
                    newDuty->weekDay = week;
                    newDuty->next = dutyHead;
                    dutyHead = newDuty;
                    dutyCnt++;
                    printf("新增排班成功！\n");
                }
                else printf("输入错误！");
            }
            else {
                printf("已存在该排班，无需重复添加。\n");
            }
        }
        else if (op == 2) {
            int dept, week;
            dept = depts[doc->deptId - 1].deptId;
            printf("输入要删除的周几（1-7）："); scanf_s("%d", &week);
            DutyNode* prev = NULL, * cur = dutyHead;
            while (cur) {
                if (strcmp(cur->doctorId, docId) == 0 && cur->deptId == dept && cur->weekDay == week) {
                    if (prev) prev->next = cur->next;
                    else dutyHead = cur->next;
                    free(cur);
                    dutyCnt--;
                    printf("排班已删除！\n");
                    break;
                }
                prev = cur; cur = cur->next;
            }
            if (!cur) printf("未找到该排班。\n");
        }
        SaveDoctorDutyToFile();
    }
    PauseAndClear();
}




void AdminDeleteDoctor(Loginf* user) {
    system("cls");
    printf("===================== 删除/排班医生 =====================\n");
    printf("1. 查看所有医生信息\n2. 删除指定医生（按身份证号）\n3. 医生排班管理\n0. 取消\n");
    printf("======================================================\n");
    int choice;
    scanf_s("%d", &choice);
    if (choice == 0) return;
    if (choice == 1) {
        system("cls");
        printf("===================== 全部医生信息 =====================\n");
        printf("共计%d名医生\n", docCnt);
        printf("═══════════════════════════════════════════════════════════════════════════\n");
        DoctorNode* p = doctorHead;
        while (p) {
            printf(" 姓名：%s |身份证：%s | 科室：%s | 职称：%s\n",
                p->name, p->idcard,  depts[p->deptId - 1].deptName, p->title);
            printf("擅长：%s | 简介：%s\n", p->specialty, p->info);
            printf("═══════════════════════════════════════════════════════════════════════════\n");
            p = p->next;
        }
        PauseAndClear();
        return;
    }
    if (choice == 2) {
        char docId[ID_LENGTH];
        printf("请输入要删除的医生身份证号：");
        scanf_s("%s", docId, (unsigned)_countof(docId));
        DoctorNode* prev = NULL;
        DoctorNode* p = doctorHead;
        while (p && strcmp(p->idcard, docId) != 0) { prev = p; p = p->next; }
        if (p == NULL) { printf("医生不存在！\n"); PauseAndClear(); return; }
        printf("将删除医生：%s，确认？(Y/N)：", p->name);
        char ch = _getch();
        if (ch != 'Y' && ch != 'y') return;
        // 删除值班记录
        DutyNode* dprev = NULL;
        DutyNode* dp = dutyHead;
        while (dp) {
            if (strcmp(dp->doctorId, docId) == 0) {
                if (dprev) dprev->next = dp->next;
                else dutyHead = dp->next;
                DutyNode* tmp = dp;
                dp = dp->next;
                free(tmp);
                dutyCnt--;
            }
            else { dprev = dp; dp = dp->next; }
        }
        // 删除登录信息
        Loginf* lprev = NULL;
        Loginf* lp = userroot;
        while (lp) {
            if (strcmp(lp->idcard, docId) == 0) {
                if (lprev) lprev->next = lp->next;
                else userroot = lp->next;
                free(lp);
                break;
            }
            lprev = lp;
            lp = lp->next;
        }
        // 删除医生节点
        if (prev) prev->next = p->next;
        else doctorHead = p->next;
        free(p);
        docCnt--;
        SaveDoctorToFile();
        SaveDoctorDutyToFile();
        SaveLoginfToFile();
        printf("删除成功！\n");
        PauseAndClear();
        return;
    }
    if (choice == 3) {
        // 直接调用排班管理
        AdminArrangeDuty();
        return;
    }
    printf("无效选项！\n");
    PauseAndClear();
}

void AdminViewAllReg(Loginf* user) {
    system("cls");
    printf("===================== 全部缴费记录 =====================\n");
    Date now = GetCurrentDate();
    char nowStr[20];
    PrintDate(now, nowStr);
    printf("当前日期：%s\n", nowStr);
    printf("---------------------------------------------------------\n");

    if (regHead == NULL) {
        printf("暂无挂号记录。\n");
        PauseAndClear();
        return;
    }

    printf("%-8s %-20s %-10s %-10s %-8s %-12s %-8s\n",
        "编号", "患者身份证", "科室", "医生", "类型", "挂号日期", "费用");
    printf("---------------------------------------------------------\n");

    RegNode* p = regHead;
    int cnt = 0;
    float total = 0.0f;
    while (p) {
        cnt++;
        char deptName[NAME_LENGTH] = "";
        if (p->deptId >= NEIKE && p->deptId <= GUKE)
            strcpy_s(deptName, NAME_LENGTH, depts[p->deptId - 1].deptName);
        else
            strcpy_s(deptName, NAME_LENGTH, "未知");

        DoctorNode* doc = FindDoctorById(p->doctorId);
        char docName[NAME_LENGTH] = "未知";
        if (doc) strcpy_s(docName, NAME_LENGTH, doc->name);

        char typeStr[10] = "";
        if (p->isToday == 0) strcpy_s(typeStr, 10, "预约");
        else if (p->isToday == 1) strcpy_s(typeStr, 10, "当日");
        else if (p->isToday == 2) strcpy_s(typeStr, 10, "药品");
        else if (p->isToday == 3) strcpy_s(typeStr, 10, "检查");
        else if (p->isToday == 4) strcpy_s(typeStr, 10, "住院");
        else strcpy_s(typeStr, 10, "其他");

        char dateStr[20];
        PrintDate(p->regDate, dateStr);

        printf("%-8d %-20s %-10s %-10s %-8s %-12s %-8.1f\n",
            p->regId, p->patientId, deptName, docName, typeStr, dateStr, p->fee);
        total += p->fee;
        p = p->next;
    }

    printf("---------------------------------------------------------\n");
    printf("总计：%d 条记录，总金额 %.1f 元\n", cnt, total);
    PauseAndClear();
}

void AdminFinanceReport(Loginf* user) {
    system("cls");
    printf("===================== 医院财务收入总表 =====================\n");

    if (regHead == NULL) {
        printf("暂无任何收入记录。\n");
        PauseAndClear();
        return;
    }

    // 定义按天统计的结构
#define MAX_DAILY 365
    typedef struct {
        Date date;
        float regFee;       // 挂号费（当日+预约）
        float medFee;       // 药品费
        float examFee;      // 检查费
        float bedFee;       // 住院费
        float total;
    } DailyStat;

    DailyStat stats[MAX_DAILY];
    int dayCount = 0;

    // 遍历所有挂号记录（所有收入都在 regHead 中）
    RegNode* p = regHead;
    while (p) {
        // 查找该日期是否已存在于 stats 中
        int found = -1;
        for (int i = 0; i < dayCount; i++) {
            if (stats[i].date.year == p->regDate.year &&
                stats[i].date.month == p->regDate.month &&
                stats[i].date.day == p->regDate.day) {
                found = i;
                break;
            }
        }
        if (found == -1) {
            // 新增一天
            if (dayCount >= MAX_DAILY) break;
            found = dayCount++;
            stats[found].date = p->regDate;
            stats[found].regFee = 0.0f;
            stats[found].medFee = 0.0f;
            stats[found].examFee = 0.0f;
            stats[found].bedFee = 0.0f;
            stats[found].total = 0.0f;
        }

        // 根据类型累加
        float fee = p->fee;
        if (p->isToday == 0 || p->isToday == 1) {
            stats[found].regFee += fee;
        }
        else if (p->isToday == 2) {
            stats[found].medFee += fee;
        }
        else if (p->isToday == 3) {
            stats[found].examFee += fee;
        }
        else if (p->isToday == 4) {
            stats[found].bedFee += fee;
        }
        stats[found].total += fee;
        p = p->next;
    }

    // 按日期排序（冒泡简单排序）
    for (int i = 0; i < dayCount - 1; i++) {
        for (int j = 0; j < dayCount - 1 - i; j++) {
            // 比较日期先后
            if (stats[j].date.year > stats[j + 1].date.year ||
                (stats[j].date.year == stats[j + 1].date.year && stats[j].date.month > stats[j + 1].date.month) ||
                (stats[j].date.year == stats[j + 1].date.year && stats[j].date.month == stats[j + 1].date.month && stats[j].date.day > stats[j + 1].date.day)) {
                DailyStat tmp = stats[j];
                stats[j] = stats[j + 1];
                stats[j + 1] = tmp;
            }
        }
    }

    // 输出日报表
    printf("日期          挂号收入    药品收入    检查收入    住院收入    当日合计\n");
    printf("--------------------------------------------------------------------\n");
    float totalReg = 0, totalMed = 0, totalExam = 0, totalBed = 0, grandTotal = 0;
    for (int i = 0; i < dayCount; i++) {
        // 跳过收入全为0的日期
        if (stats[i].total == 0) continue;

        char dateStr[20];
        PrintDate(stats[i].date, dateStr);
        printf("%-12s %10.1f %10.1f %10.1f %10.1f %10.1f\n",
            dateStr, stats[i].regFee, stats[i].medFee, stats[i].examFee, stats[i].bedFee, stats[i].total);

        totalReg += stats[i].regFee;
        totalMed += stats[i].medFee;
        totalExam += stats[i].examFee;
        totalBed += stats[i].bedFee;
        grandTotal += stats[i].total;
    }

    printf("--------------------------------------------------------------------\n");
    printf("分类合计：    %10.1f %10.1f %10.1f %10.1f %10.1f\n",
        totalReg, totalMed, totalExam, totalBed, grandTotal);
    printf("\n===================== 收入汇总（总计） =====================\n");
    printf("挂号总收入   ：%.2f 元\n", totalReg);
    printf("药品总收入   ：%.2f 元\n", totalMed);
    printf("检查总收入   ：%.2f 元\n", totalExam);
    printf("住院总收入   ：%.2f 元\n", totalBed);
    printf("----------------------------------------\n");
    printf("医院总收入   ：%.2f 元\n", grandTotal);

    PauseAndClear();
}


void AdminMenu(Loginf* user) {
    int choice = 0;
    while (1) {
        system("cls");
        PrintHospitalBanner();
        SetColor(14);
        printf("======================= 管理员菜单 =======================\n");
        printf("当前用户：%s（管理员）\n", user->idcard);
        printf("1. 编辑医生         2. 查看全科室药品    3. 修改药品库存\n");
        printf("4. 药品模糊检索     5. 库存预警查询      6. 全部缴费记录\n"); 
		printf("7. 财务收入总表     8. 修改密码          0. 退出菜单\n");
        printf("========================================================\n");
        printf("请输入操作选项：");
        if (scanf_s("%d", &choice) != 1) {
            while (getchar() != '\n');
            PrintError("输入错误！\n");
            PauseAndClear();
            continue;
        }
        switch (choice) {
        case 1: AdminDeleteDoctor(user); break;
        case 2: AdminCheckMed(user); break;
        case 3: AdminEditStock(user); break;
        case 4: AdminSearchMed(user); break;
        case 5: AdminCheckStock(user); break;
        case 6: AdminViewAllReg(user); break;
        case 7: AdminFinanceReport(user); break;
        case 8: ChangePassword(user); break;         // 调用修改密码
        case 0: if (ExitConfirm()) return; break;
        default: PrintError("输入错误！\n"); PauseAndClear();
        }
    }
}

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define RESET   "\033[0m"
#define NOTE_DO  660
#define NOTE_RE  587
#define NOTE_MI  659
#define NOTE_FA  698
#define NOTE_SO  784
#define NOTE_LA  880
#define NOTE_SI  988
//about
void type_print(const char* text, const char* color, int delay_ms) {
    printf("%s", color);
    fflush(stdout);
    for (const char* p = text; *p; p++) {
        putchar(*p);
        Beep(NOTE_DO, 10);
        fflush(stdout);
        Sleep(delay_ms); // 毫秒级延迟
    }
    printf(RESET);
}
void About() {
    system("cls");
    printf("\n");
    type_print("=================================================\n", BLUE, 5);
    Sleep(1);
    type_print("           计算机学院医院 简介                  \n", MAGENTA, 8);
    Sleep(1);
    type_print("=================================================\n", BLUE, 5);
    Sleep(1);

    // 地址信息
    printf("\n");
    type_print("  地址：前进大街2700号 王湘浩楼对面 \n", GREEN, 6);
    Sleep(1);
    printf("\n");

    // 正文逐字打印
    type_print("  计算机学院医院是学院直属专业医疗机构，\n", WHITE, 4);
    Sleep(1);
    type_print("  专注为全院师生提供安全、便捷、贴心的医疗服务。\n", WHITE, 4);
    Sleep(1);

    printf("\n");
    type_print("  医院现有职工共三十余名，汇聚多名资深主治医师，\n", YELLOW, 4);
    Sleep(1);
    type_print("  医疗团队经验丰富，诊疗专业可靠。\n", YELLOW, 4);
    Sleep(1);

    printf("\n");
    type_print("  全院共设5个特色科室，每个科室配备5间诊疗用房，\n", CYAN, 4);
    Sleep(1);
    type_print("  所有科室均支持单人间、双人间灵活使用，\n", CYAN, 4);
    Sleep(1);
    type_print("  环境私密舒适，特别适合家属陪伴就医。\n", CYAN, 4);
    Sleep(1);
    printf("\n");
    type_print("  医院可正常开具30大类常用药品，满足日常诊疗需求，\n", GREEN, 4);
    Sleep(1);
    type_print("  宽敞舒适的诊疗空间可同时容纳多名患者就诊，\n", GREEN, 4);
    Sleep(1);
    type_print("  全力守护全院师生的身体健康。\n", GREEN, 4);
    Sleep(1);
    printf("\n");
    type_print("=================================================\n", BLUE, 5);
    Sleep(1);
    type_print("           用心服务 · 守护健康                   \n", RED, 8);
    Sleep(1);
    type_print("=================================================\n", BLUE, 5);
    printf("\n");
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBeep(MB_ICONEXCLAMATION);
    Sleep(100); MessageBeep(MB_ICONEXCLAMATION);
	system("pause");


}