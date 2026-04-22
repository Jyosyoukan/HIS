#include "Init.h"
#include "Tool.h"

/* ========== 【修改】全局变量定义：链表头指针 ========== */
Loginf* userroot = NULL;

DoctorNode* doctorHead = NULL;
PatientNode* patientHead = NULL;
Department depts[MAX_DEPARTMENTS];
BedNode* bedHead = NULL;
MedicineNode* medicineHead = NULL;
RegNode* regHead = NULL;
CaseNode* caseHead = NULL;
DutyNode* dutyHead = NULL;
ExamNode* examHead = NULL;

int docCnt = 0, patCnt = 0, regCnt = 0, caseCnt = 0, medCnt = 0, dutyCnt = 0, examCnt = 0;

const char ADMIN_ID[ID_LENGTH] = "admin";
const char ADMIN_PWD[PASSWORD_LENGTH] = "123456";
const int ADMIN_ROLE = 3;
const float FEE_TODAY = 20.0f;
const float FEE_APPOINT = 50.0f;

//释放所有链表内存
void FreeAllLists(void) {
    // 释放登录链表
    Loginf* lp = userroot;
    while (lp) { Loginf* tmp = lp; lp = lp->next; free(tmp); }
    userroot = NULL;
    // 释放医生链表
    DoctorNode* dp = doctorHead;
    while (dp) { DoctorNode* tmp = dp; dp = dp->next; free(tmp); }
    doctorHead = NULL;
    // 释放患者链表
    PatientNode* pp = patientHead;
    while (pp) { PatientNode* tmp = pp; pp = pp->next; free(tmp); }
    patientHead = NULL;
    // 释放床位链表
    BedNode* bp = bedHead;
    while (bp) { BedNode* tmp = bp; bp = bp->next; free(tmp); }
    bedHead = NULL;
    // 释放药品链表
    MedicineNode* mp = medicineHead;
    while (mp) { MedicineNode* tmp = mp; mp = mp->next; free(tmp); }
    medicineHead = NULL;
    // 释放挂号链表
    RegNode* rp = regHead;
    while (rp) { RegNode* tmp = rp; rp = rp->next; free(tmp); }
    regHead = NULL;
    // 释放病例链表
    CaseNode* cp = caseHead;
    while (cp) { CaseNode* tmp = cp; cp = cp->next; free(tmp); }
    caseHead = NULL;
    // 释放值班链表
    DutyNode* dup = dutyHead;
    while (dup) { DutyNode* tmp = dup; dup = dup->next; free(tmp); }
    dutyHead = NULL;
    // 释放检查链表
    ExamNode* ep = examHead;
    while (ep) { ExamNode* tmp = ep; ep = ep->next; free(tmp); }
    examHead = NULL;
    // 重置计数器
    docCnt = patCnt = regCnt = caseCnt = medCnt = dutyCnt = examCnt = 0;
}

// 检测并创建空文件
int CheckAndCreateFile(const char* filename) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, filename, "r");
    if (err != 0 || fp == NULL) {
        err = fopen_s(&fp, filename, "w");
        if (err != 0) {
            printf("创建文件【%s】失败！\n", filename);
            return -1;
        }
      //  printf("创建空文件【%s】成功！\n", filename);
        if (strcmp(filename, "BedData.txt") == 0) {
           
            BedNode* bedTail = NULL;
            int bedId = 1;
            // 5个科室，每个科室8张床（2个双人间 + 4个单人间）
            for (int d = NEIKE; d <= GUKE; d++) {
                // 2个双人间（每间2床）
                for (int room = 1; room <= 2; room++) {
                    for (int bed = 1; bed <= 2; bed++) {
                        BedNode* newBed = (BedNode*)malloc(sizeof(BedNode));
                        newBed->bedId = bedId;
                        newBed->deptId = d;
                        newBed->isFree = 1;
                        newBed->patientId[0] = '\0';
                        memset(&newBed->inDate, 0, sizeof(Date));
                        newBed->roomType = 0;
                        newBed->roomId = room;
                        newBed->bedIndex = bed;
                        newBed->next = NULL;
                        if (bedHead == NULL) bedHead = bedTail = newBed;
                        else { bedTail->next = newBed; bedTail = newBed; }
                        bedId++;
                    }
                }
                // 4个单人间
                for (int room = 3; room <= 6; room++) {
                    BedNode* newBed = (BedNode*)malloc(sizeof(BedNode));
                    newBed->bedId = bedId;
                    newBed->deptId = d;
                    newBed->isFree = 1;
                    newBed->patientId[0] = '\0';
                    memset(&newBed->inDate, 0, sizeof(Date));
                    newBed->roomType = 1;
                    newBed->roomId = room;
                    newBed->bedIndex = 1;
                    newBed->next = NULL;
                    if (bedHead == NULL) bedHead = bedTail = newBed;
                    else { bedTail->next = newBed; bedTail = newBed; }
                    bedId++;
                }
            }
            // 5个ICU病房
            for (int i = 1; i <= 5; i++) {
                BedNode* newBed = (BedNode*)malloc(sizeof(BedNode));
                newBed->bedId = bedId;
                newBed->deptId = 0;
                newBed->isFree = 1;
                newBed->patientId[0] = '\0';
                memset(&newBed->inDate, 0, sizeof(Date));
                newBed->roomType = 2;
                newBed->roomId = i;
                newBed->bedIndex = 1;
                newBed->next = NULL;
                if (bedHead == NULL) bedHead = bedTail = newBed;
                else { bedTail->next = newBed; bedTail = newBed; }
                bedId++;
            }
            // 写入文件
            BedNode* p = bedHead;
            while (p != NULL) {
                fprintf(fp, "%d %d %d %s %d-%d-%d %d %d %d\n",
                    p->bedId, p->deptId, p->isFree,
                    p->patientId[0] ? p->patientId : "-",
                    p->inDate.year, p->inDate.month, p->inDate.day,
                    p->roomType, p->roomId, p->bedIndex);
                p = p->next;
            }
        }
    }
    if (fp) fclose(fp);
    return 0;
}

Date GetCurrentDate(void) {
    time_t now = time(NULL);
    struct tm t;
    localtime_s(&t, &now);
    Date date;
    date.year = t.tm_year + 1900;
    date.month = t.tm_mon + 1;
    date.day = t.tm_mday;
    return date;
}

int GetWeekDay(Date date) {
    struct tm t = { 0 };
    t.tm_year = date.year - 1900;
    t.tm_mon = date.month - 1;
    t.tm_mday = date.day;
    mktime(&t);
    int wday = t.tm_wday;
    return wday == 0 ? 7 : wday;
}

int CheckAppointDate(Date now, Date appoint) {
    struct tm t_now = { 0 }, t_app = { 0 };
    t_now.tm_year = now.year - 1900;
    t_now.tm_mon = now.month - 1;
    t_now.tm_mday = now.day;
    t_app.tm_year = appoint.year - 1900;
    t_app.tm_mon = appoint.month - 1;
    t_app.tm_mday = appoint.day;
    time_t ts_now = mktime(&t_now);
    time_t ts_app = mktime(&t_app);
    if (ts_app < ts_now) return 0;
    int days = (int)((ts_app - ts_now) / (24 * 3600));
    return days <= 7 ? 1 : 0;
}

void generate_password(char* pwd, int lastIdx, int firstIdx, const char* idcard) {
    char* lastNames[] = { "张", "李", "王", "刘", "陈", "杨", "赵", "黄", "周", "吴", "朱", "胡", "林", "马", "郭" };
    char* firstNames[] = { "明", "华", "伟", "芳", "丽", "军", "杰", "娟", "涛", "敏", "强", "燕", "浩", "婷", "伟" };
    char* lastNamesPinyin[] = { "zhang", "li", "wang", "liu", "chen", "yang", "zhao", "huang", "zhou", "wu", "zhu", "hu", "lin", "ma", "guo" };
    char* firstNamesPinyinInitial[] = { "m", "h", "w", "f", "l", "j", "j", "j", "t", "m", "q", "y", "h", "t", "w" };
    const char* pinyinLast = lastNamesPinyin[lastIdx];
    const char* pinyinFirstInitial = firstNamesPinyinInitial[firstIdx];
    const char* idLast6 = &idcard[12];
    sprintf_s(pwd, PASSWORD_LENGTH, "%s%s%s", pinyinLast, pinyinFirstInitial, idLast6);
}

/* ========== GenDoctorData 改为链表尾插 ========== */
int GenDoctorData(void) {
    char* titles[] = { "主任医师", "副主任医师", "主治医师", "住院医师" };
    DeptType deptIds[] = { NEIKE, WAIKE, FUCHANKE, ERKE, GUKE };
    char* deptNames[] = { "内科", "外科", "妇产科", "儿科", "骨科" };
    char* specs[] = { "心血管疾病/呼吸内科", "普外科/骨科创伤", "妇科炎症/产科护理", "小儿感冒/儿科消化", "骨折/关节外科" };
    char* lastNames[] = { "张", "李", "王", "刘", "陈", "杨", "赵", "黄", "周", "吴", "朱", "胡", "林", "马", "郭" };
    char* firstNames[] = { "明", "华", "伟", "芳", "丽", "军", "杰", "娟", "涛", "敏", "强", "燕", "浩", "婷", "伟" };
    char* lastNamesPinyin[] = { "zhang", "li", "wang", "liu", "chen", "yang", "zhao", "huang", "zhou", "wu", "zhu", "hu", "lin", "ma", "guo" };
    char* firstNamesPinyinInitial[] = { "m", "h", "w", "f", "l", "j", "j", "j", "t", "m", "q", "y", "h", "t", "w" };

    srand((unsigned)time(NULL));
    DoctorNode* tail = doctorHead;
    while (tail && tail->next) tail = tail->next; // 找到链表尾

    for (int i = 0; i < MAX_DOCTORS; i++) {
        char id[ID_LENGTH] = "0";
        for (int j = 0; j < 17; j++) id[j] = '0' + rand() % 10;
        id[17] = (rand() % 10 == 0) ? 'X' : (rand() % 10 + '0');
        id[ID_LENGTH - 1] = '\0';

        int lIdx = rand() % 15;
        int fIdx = rand() % 15;
        char name[NAME_LENGTH];
        strcpy_s(name, NAME_LENGTH, lastNames[lIdx]);
        strcat_s(name, NAME_LENGTH, firstNames[fIdx]);

        DoctorNode* newDoc = (DoctorNode*)malloc(sizeof(DoctorNode));
        memset(newDoc, 0, sizeof(DoctorNode));
        strcpy_s(newDoc->idcard, ID_LENGTH, id);
        strcpy_s(newDoc->name, NAME_LENGTH, name);
        newDoc->deptId = (i / 6) + 1;
        strcpy_s(newDoc->title, NAME_LENGTH, titles[rand() % 4]);
        strcpy_s(newDoc->specialty, NAME_LENGTH, specs[newDoc->deptId - 1]);
        char info[MAX_INFO];
        sprintf_s(info, MAX_INFO, "从事%s临床工作%d年，擅长%s的诊断与治疗，临床经验丰富。",
            deptNames[newDoc->deptId - 1], 5 + rand() % 25, newDoc->specialty);
        strcpy_s(newDoc->info, MAX_INFO, info);
        newDoc->next = NULL;

        // 链表尾插
        if (doctorHead == NULL) doctorHead = tail = newDoc;
        else { tail->next = newDoc; tail = newDoc; }

        // 同步到登录链表
        Loginf* newNode = (Loginf*)malloc(sizeof(Loginf));
        strcpy_s(newNode->idcard, ID_LENGTH, id);
        char temp_pwd[PASSWORD_LENGTH];
        generate_password(temp_pwd, lIdx, fIdx, id);
        strcpy_s(newNode->password, PASSWORD_LENGTH, temp_pwd);
        newNode->role = 2;
        newNode->next = NULL;
        if (userroot == NULL) userroot = newNode;
        else {
            Loginf* p = userroot;
            while (p->next != NULL) p = p->next;
            p->next = newNode;
        }
    }
    docCnt = MAX_DOCTORS;
   // printf("成功生成%d名医生数据（每科室6人）！\n", MAX_DOCTORS);
    SaveDoctorToFile();
    SaveLoginfToFile();
    return 0;
}

/* ========== GenDoctorDuty 改为链表 ========== */
int GenDoctorDuty(void) {
    // 先清空原有值班链表
    DutyNode* p = dutyHead;
    while (p) { DutyNode* tmp = p; p = p->next; free(tmp); }
    dutyHead = NULL;
    dutyCnt = 0;

    char* deptNames[] = { "内科", "外科", "妇产科", "儿科", "骨科" };
    DutyNode* tail = NULL;
    for (int d = NEIKE; d <= GUKE; d++) {
        for (int w = 1; w <= DUTY_DAYS; w++) {
            DutyNode* newDuty = (DutyNode*)malloc(sizeof(DutyNode));
            newDuty->deptId = d;
            newDuty->weekDay = w;
            // 从医生链表中查找对应科室的医生
            DoctorNode* doc = doctorHead;
            int count = 0;
            while (doc) {
                if (doc->deptId == d) count++;
                doc = doc->next;
            }
            if (count == 0) {
                strcpy_s(newDuty->doctorId, ID_LENGTH, "-");
            }
            else {
                int randIdx = rand() % count;
                doc = doctorHead;
                int idx = 0;
                while (doc) {
                    if (doc->deptId == d) {
                        if (idx == randIdx) break;
                        idx++;
                    }
                    doc = doc->next;
                }
                strcpy_s(newDuty->doctorId, ID_LENGTH, doc->idcard);
            }
            newDuty->next = NULL;
            if (dutyHead == NULL) dutyHead = tail = newDuty;
            else { tail->next = newDuty; tail = newDuty; }
            dutyCnt++;
            printf("【%s】周%d值班医生：%s\n", deptNames[d - 1], w, newDuty->doctorId);
        }
    }
  //  printf("成功生成医生值班表（5科室×7天）！\n");
    SaveDoctorDutyToFile();
    return 0;
}

/* ========== GenMedicineData 改为链表 ========== */
int GenMedicineData(void) {
    MedicineNode* p = medicineHead;
    while (p) { MedicineNode* tmp = p; p = p->next; free(tmp); }
    medicineHead = NULL;
    medCnt = 0;

    char* medNames[5][MED_PER_DEPT] = {
        {"阿司匹林", "布洛芬", "头孢克肟", "氨溴索", "奥美拉唑", "硝苯地平"},
        {"碘伏", "纱布", "云南白药", "阿莫西林", "破伤风疫苗", "红花油"},
        {"益母草", "黄体酮", "头孢曲松", "碘伏", "妇科千金片", "叶酸"},
        {"小儿氨酚黄那敏", "妈咪爱", "布洛芬混悬液", "头孢克洛", "蒙脱石散", "小儿豉翘"},
        {"钙片", "布洛芬", "云南白药", "接骨七厘片", "活血止痛膏", "氨糖"}
    };
    char* medAliases[5][MED_PER_DEPT] = {
        {"乙酰水杨酸", "芬必得", "世福素", "沐舒坦", "洛赛克", "心痛定"},
        {"碘酊", "医用纱布", "云白", "阿莫仙", "破免", "红药"},
        {"益母膏", "孕酮", "菌必治", "碘酊", "千金片", "维生素B9"},
        {"小儿氨酚", "益生菌", "美林", "希刻劳", "思密达", "豉翘"},
        {"碳酸钙", "芬必得", "云白", "接骨片", "止痛膏", "氨基葡萄糖"}
    };
    srand((unsigned)time(NULL));
    MedicineNode* tail = NULL;
    int medId = 1;
    for (int d = NEIKE; d <= GUKE; d++) {
        for (int m = 0; m < MED_PER_DEPT; m++) {
            MedicineNode* newMed = (MedicineNode*)malloc(sizeof(MedicineNode));
            memset(newMed, 0, sizeof(MedicineNode));
            newMed->medId = medId++;
            strcpy_s(newMed->medName, NAME_LENGTH, medNames[d - 1][m]);
            strcpy_s(newMed->medAlias, MAX_MED_ALIAS, medAliases[d - 1][m]);
            newMed->medType = rand() % 2 + 1;
            newMed->bindDept = d;
            newMed->price = (float)(rand() % 990 + 10) / 10.0f;
            newMed->stock = 100;
            Date now = GetCurrentDate();
            newMed->prodDate.year = now.year - (rand() % 2);
            newMed->prodDate.month = rand() % 12 + 1;
            newMed->prodDate.day = rand() % 28 + 1;
            newMed->next = NULL;
            if (medicineHead == NULL) medicineHead = tail = newMed;
            else { tail->next = newMed; tail = newMed; }
            medCnt++;
        }
    }
    SaveMedicineToFile();
    return 0;
}

/* ========== GenExamData 改为链表 ========== */
int GenExamData(void) {
    ExamNode* p = examHead;
    while (p) { ExamNode* tmp = p; p = p->next; free(tmp); }
    examHead = NULL;
    examCnt = 0;

    const char* examTypes[] = { "血液化验", "X光", "B超", "CT", "心电图", "尿常规", "肝功", "肾功", "血糖", "凝血" };
    const float examCosts[] = { 50.0f, 120.0f, 150.0f, 800.0f, 80.0f, 30.0f, 60.0f, 70.0f, 40.0f, 55.0f };
    const int examTypeCount = sizeof(examTypes) / sizeof(examTypes[0]);
    Date now = GetCurrentDate();
    ExamNode* tail = NULL;
    for (int i = 0; i < examTypeCount; i++) {
        ExamNode* newExam = (ExamNode*)malloc(sizeof(ExamNode));
        memset(newExam, 0, sizeof(ExamNode));
        newExam->examId = i + 1;
        strcpy_s(newExam->patientId, ID_LENGTH, "-");
        strcpy_s(newExam->doctorId, ID_LENGTH, "-");
        strcpy_s(newExam->examType, NAME_LENGTH, examTypes[i]);
        newExam->cost = examCosts[i];
        newExam->examDate = now;
        newExam->status = 0;
        newExam->result[0] = '\0';
        newExam->next = NULL;
        if (examHead == NULL) examHead = tail = newExam;
        else { tail->next = newExam; tail = newExam; }
        examCnt++;
    }
    SaveExamToFile();
    return 0;
}

/* ========== Save 系列函数：遍历链表写入 ========== */
int SaveDoctorToFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "DoctorData.txt", "w");
    if (!fp) { printf("保存医生数据失败！\n"); return -1; }
    DoctorNode* p = doctorHead;
    while (p != NULL) {
        fprintf(fp, "%s %s %d %s %s %s\n",
            p->idcard, p->name, p->deptId,
            p->title, p->specialty, p->info);
        p = p->next;
    }
    fclose(fp);
    return 0;
}

int SaveDoctorDutyToFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "DoctorDuty.txt", "w");
    if (err != 0) return -1;
    DutyNode* p = dutyHead;
    while (p != NULL) {
        fprintf(fp, "%d %d %s\n", p->deptId, p->weekDay, p->doctorId);
        p = p->next;
    }
    fclose(fp);
    return 0;
}

int SaveMedicineToFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "MedData.txt", "w");
    if (err != 0) return -1;
    MedicineNode* p = medicineHead;
    while (p != NULL) {
        fprintf(fp, "%d %s %s %d %d %.1f %d %d-%d-%d\n",
            p->medId, p->medName, p->medAlias,
            p->medType, p->bindDept, p->price,
            p->stock, p->prodDate.year,
            p->prodDate.month, p->prodDate.day);
        p = p->next;
    }
    fclose(fp);
    return 0;
}

int SaveLoginfToFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "LoginRes.txt", "w");
    if (err != 0) return -1;
    fprintf(fp, "%s %s %d\n", ADMIN_ID, ADMIN_PWD, ADMIN_ROLE);
    Loginf* p = userroot;
    while (p != NULL) {
        fprintf(fp, "%s %s %d\n", p->idcard, p->password, p->role);
        p = p->next;
    }
    fclose(fp);
    return 0;
}

int SavePatientToFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "PatientData.txt", "w");
    if (err != 0) return -1;
    PatientNode* p = patientHead;
    p->medicalRecord[MAX_INFO - 1] = '\0';
    while (p != NULL) {
        fprintf(fp, "%s %s %c %d %d %d-%d-%d %d-%d-%d %s@\n",
            p->idcard, p->name, p->gender,
            p->isHospitalized, p->bedId,
            p->inDate.year, p->inDate.month, p->inDate.day,
            p->outDate.year, p->outDate.month, p->outDate.day,
            p->medicalRecord);
        p = p->next;
    }
    fclose(fp);
    return 0;
}

int SaveBedToFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "BedData.txt", "w");
    if (err != 0 || fp == NULL) return -1;
    BedNode* p = bedHead;
    while (p != NULL) {
        fprintf(fp, "%d %d %d %s %d-%d-%d %d %d %d\n",
            p->bedId, p->deptId, p->isFree,
            p->patientId[0] ? p->patientId : "-",
            p->inDate.year, p->inDate.month, p->inDate.day,
            p->roomType, p->roomId, p->bedIndex);
        p = p->next;
    }
    fclose(fp);
    return 0;
}

int SaveRegToFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "RegRecord.txt", "w");
    if (err != 0) return -1;
    RegNode* p = regHead;
    while (p != NULL) {
        fprintf(fp, "%d %s %d %s %d %.1f %d %d-%d-%d %d-%d-%d\n",
            p->regId, p->patientId, p->deptId,
            p->doctorId, p->weekDay, p->fee,
            p->isToday, p->regDate.year,
            p->regDate.month, p->regDate.day,
            p->appointDate.year, p->appointDate.month,
            p->appointDate.day);
        p = p->next;
    }
    fclose(fp);
    return 0;
}

int SaveCaseToFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "CaseData.txt", "w");
    if (err != 0) return -1;
    CaseNode* p = caseHead;
	if (p->illness[0] == '\0') p->illness[0] = '-',p->illness[1] = '\0';
    while (p != NULL) {
        fprintf(fp, "%d %s %s %s@ %d %d %d %d %d %d %d %d %d %d %d %d-%d-%d %d-%d-%d %d-%d-%d\n",
            p->caseId, p->patientId, p->doctorId,
            p->illness, p->medId[0], p->medId[1],
            p->medId[2], p->medId[3], p->medId[4],p->medNum[0],p->medNum[1],p->medNum[2],p->medNum[3],p->medNum[4],
            p->bedId, p->diagDate.year, p->diagDate.month,
            p->diagDate.day, p->inDate.year, p->inDate.month,
            p->inDate.day, p->outDate.year, p->outDate.month,
            p->outDate.day);
        p = p->next;
    }
    fclose(fp);
    return 0;
}

int SaveExamToFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "ExamData.txt", "w");
    if (err != 0 || fp == NULL) return -1;
    ExamNode* p = examHead;
    while (p != NULL) {
        fprintf(fp, "%d %s %s %s %.2f %d-%d-%d %d %s\n",
            p->examId,
            p->patientId,
            p->doctorId,
            p->examType,
            p->cost,
            p->examDate.year, p->examDate.month, p->examDate.day,
            p->status,
            p->result[0] ? p->result : "-");
        p = p->next;
    }
    fclose(fp);
    return 0;
}

/* ========== Load 系列函数：从文件读取并构建链表 ========== */
int LoadExamFromFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "ExamData.txt", "r");
    if (err != 0 || fp == NULL) return -1;
    ExamNode* p = examHead;
    while (p) { ExamNode* tmp = p; p = p->next; free(tmp); }
    examHead = NULL;
    examCnt = 0;
    ExamNode* tail = NULL;
    char resultBuf[MAX_INFO];
    ExamNode temp;
    while (fscanf_s(fp, "%d %s %s %s %f %d-%d-%d %d %s",
        &temp.examId,
        temp.patientId, (unsigned)_countof(temp.patientId),
        temp.doctorId, (unsigned)_countof(temp.doctorId),
        temp.examType, (unsigned)_countof(temp.examType),
        &temp.cost,
        &temp.examDate.year, &temp.examDate.month, &temp.examDate.day,
        &temp.status,
        resultBuf, (unsigned)_countof(resultBuf)) != EOF) {
        ExamNode* newNode = (ExamNode*)malloc(sizeof(ExamNode));
        memcpy(newNode, &temp, sizeof(ExamNode));
        if (strcmp(resultBuf, "-") == 0) newNode->result[0] = '\0';
        else strcpy_s(newNode->result, MAX_INFO, resultBuf);
        newNode->next = NULL;
        if (examHead == NULL) examHead = tail = newNode;
        else { tail->next = newNode; tail = newNode; }
        examCnt++;
    }
    fclose(fp);
    return examCnt > 0 ? 0 : -1;
}

int LoadDoctorDutyFromFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "DoctorDuty.txt", "r");
    if (err != 0) return -1;
    DutyNode* p = dutyHead;
    while (p) { DutyNode* tmp = p; p = p->next; free(tmp); }
    dutyHead = NULL;
    dutyCnt = 0;
    DutyNode* tail = NULL;
    DutyNode temp;
    while (fscanf_s(fp, "%d %d %s", &temp.deptId, &temp.weekDay,
        temp.doctorId, (unsigned)_countof(temp.doctorId)) != EOF) {
        DutyNode* newNode = (DutyNode*)malloc(sizeof(DutyNode));
        memcpy(newNode, &temp, sizeof(DutyNode));
        newNode->next = NULL;
        if (dutyHead == NULL) dutyHead = tail = newNode;
        else { tail->next = newNode; tail = newNode; }
        dutyCnt++;
    }
    fclose(fp);
    return dutyCnt > 0 ? 0 : -1;
}

int LoadMedicineFromFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "MedData.txt", "r");
    if (!fp) return -1;
    MedicineNode* p = medicineHead;
    while (p) { MedicineNode* tmp = p; p = p->next; free(tmp); }
    medicineHead = NULL;
    medCnt = 0;
    MedicineNode* tail = NULL;
    MedicineNode temp;
    while (fscanf_s(fp, "%d %s %s %d %d %f %d %d-%d-%d",
        &temp.medId,
        temp.medName, (unsigned)_countof(temp.medName),
        temp.medAlias, (unsigned)_countof(temp.medAlias),
        &temp.medType, &temp.bindDept, &temp.price,
        &temp.stock, &temp.prodDate.year,
        &temp.prodDate.month, &temp.prodDate.day) != EOF) {
        MedicineNode* newNode = (MedicineNode*)malloc(sizeof(MedicineNode));
        memcpy(newNode, &temp, sizeof(MedicineNode));
        newNode->next = NULL;
        if (medicineHead == NULL) medicineHead = tail = newNode;
        else { tail->next = newNode; tail = newNode; }
        medCnt++;
    }
    fclose(fp);
    return medCnt > 0 ? 0 : -1;
}

int LoadLoginfFromFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "LoginRes.txt", "r");
    if (err != 0) return -1;
    Loginf* p = userroot;
    while (p) { Loginf* tmp = p; p = p->next; free(tmp); }
    userroot = NULL;
    Loginf* tail = NULL;
    char id[ID_LENGTH], pwd[PASSWORD_LENGTH];
    int role;
    while (fscanf_s(fp, "%s %s %d", id, (unsigned)_countof(id),
        pwd, (unsigned)_countof(pwd), &role) != EOF) {
        if (strcmp(id, ADMIN_ID) == 0) continue;
        Loginf* newNode = (Loginf*)malloc(sizeof(Loginf));
        strcpy_s(newNode->idcard, ID_LENGTH, id);
        strcpy_s(newNode->password, PASSWORD_LENGTH, pwd);
        newNode->role = role;
        newNode->next = NULL;
        if (userroot == NULL) userroot = tail = newNode;
        else { tail->next = newNode; tail = newNode; }
    }
    fclose(fp);
    return 0;
}

int LoadDoctorFromFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "DoctorData.txt", "r");
    if (!fp) return -1;
    DoctorNode* p = doctorHead;
    while (p) { DoctorNode* tmp = p; p = p->next; free(tmp); }
    doctorHead = NULL;
    docCnt = 0;
    DoctorNode* tail = NULL;
    DoctorNode temp;
    while (fscanf_s(fp, "%s %s %d %s %s %s",
        temp.idcard, (unsigned)_countof(temp.idcard),
        temp.name, (unsigned)_countof(temp.name),
        &temp.deptId,
        temp.title, (unsigned)_countof(temp.title),
        temp.specialty, (unsigned)_countof(temp.specialty),
        temp.info, (unsigned)_countof(temp.info)) != EOF) {
        DoctorNode* newNode = (DoctorNode*)malloc(sizeof(DoctorNode));
        memcpy(newNode, &temp, sizeof(DoctorNode));
        newNode->next = NULL;
        if (doctorHead == NULL) doctorHead = tail = newNode;
        else { tail->next = newNode; tail = newNode; }
        docCnt++;
    }
    fclose(fp);
    return docCnt > 0 ? 0 : -1;
}

int LoadPatientFromFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "PatientData.txt", "r");
    if (err != 0 || fp == NULL) return -1;

    // 清空现有链表
    PatientNode* p = patientHead;
    while (p) { PatientNode* tmp = p; p = p->next; free(tmp); }
    patientHead = NULL;
    patCnt = 0;

    char line[1024];
    PatientNode* tail = NULL;

    while (fgets(line, sizeof(line), fp)) {
        PatientNode* newNode = (PatientNode*)calloc(1, sizeof(PatientNode));
        if (!newNode) break;

        char medicalBuf[MAX_INFO * 2] = { 0 }; // 临时存放病历（防止溢出）

        // 按格式解析：身份证 姓名 性别 住院状态 床位号 年-月-日 年-月-日 病历@
        int parsed = sscanf_s(line, "%19s %199s %c %d %d %d-%d-%d %d-%d-%d %[^@]@",
            newNode->idcard, (unsigned)_countof(newNode->idcard),
            newNode->name, (unsigned)_countof(newNode->name),
            &newNode->gender, 1,
            &newNode->isHospitalized, &newNode->bedId,
            &newNode->inDate.year, &newNode->inDate.month, &newNode->inDate.day,
            &newNode->outDate.year, &newNode->outDate.month, &newNode->outDate.day,
            medicalBuf, (unsigned)_countof(medicalBuf));

        // 若病历字段解析失败（文件损坏），强制置空并清理残留
        if (parsed < 12) {
            // 未读到病历部分，手动将病历置空
            newNode->medicalRecord[0] = '\0';
            // 跳过本行剩余内容，避免影响下一行
        }
        else {
            // 安全复制病历，确保截断并终止
            strncpy_s(newNode->medicalRecord, MAX_INFO, medicalBuf, MAX_INFO - 1);
            newNode->medicalRecord[MAX_INFO - 1] = '\0';
        }

        newNode->next = NULL;
        if (patientHead == NULL) {
            patientHead = tail = newNode;
        }
        else {
            tail->next = newNode;
            tail = newNode;
        }
        patCnt++;
    }
    fclose(fp);
    return patCnt > 0 ? 0 : -1;
}

int LoadBedFromFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "BedData.txt", "r");
    if (err != 0 || fp == NULL) return -1;
    BedNode* p = bedHead;
    while (p) { BedNode* tmp = p; p = p->next; free(tmp); }
    bedHead = NULL;
    BedNode* tail = NULL;
    char patientBuf[ID_LENGTH];
    BedNode temp;
    while (fscanf_s(fp, "%d %d %d %s %d-%d-%d %d %d %d",
        &temp.bedId, &temp.deptId, &temp.isFree,
        patientBuf, (unsigned)_countof(patientBuf),
        &temp.inDate.year, &temp.inDate.month, &temp.inDate.day,
        &temp.roomType, &temp.roomId, &temp.bedIndex) != EOF) {
        BedNode* newNode = (BedNode*)malloc(sizeof(BedNode));
        memcpy(newNode, &temp, sizeof(BedNode));
        if (strcmp(patientBuf, "-") == 0) newNode->patientId[0] = '\0';
        else strcpy_s(newNode->patientId, ID_LENGTH, patientBuf);
        newNode->next = NULL;
        if (bedHead == NULL) bedHead = tail = newNode;
        else { tail->next = newNode; tail = newNode; }
    }
    fclose(fp);
    return 0;
}

int LoadRegFromFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "RegRecord.txt", "r");
    if (err != 0) return -1;
    RegNode* p = regHead;
    while (p) { RegNode* tmp = p; p = p->next; free(tmp); }
    regHead = NULL;
    regCnt = 0;
    RegNode* tail = NULL;
    RegNode temp;
    while (fscanf_s(fp, "%d %s %d %s %d %f %d %d-%d-%d %d-%d-%d",
        &temp.regId,
        temp.patientId, (unsigned)_countof(temp.patientId),
        &temp.deptId, temp.doctorId,
        (unsigned)_countof(temp.doctorId), &temp.weekDay,
        &temp.fee, &temp.isToday, &temp.regDate.year,
        &temp.regDate.month, &temp.regDate.day,
        &temp.appointDate.year, &temp.appointDate.month,
        &temp.appointDate.day) != EOF) {
        RegNode* newNode = (RegNode*)malloc(sizeof(RegNode));
        memcpy(newNode, &temp, sizeof(RegNode));
        newNode->next = NULL;
        if (regHead == NULL) regHead = tail = newNode;
        else { tail->next = newNode; tail = newNode; }
        regCnt++;
    }
    fclose(fp);
    return 0;
}

int LoadCaseFromFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "CaseData.txt", "r");
    if (err != 0) return -1;
    CaseNode* p = caseHead;
    while (p) { CaseNode* tmp = p; p = p->next; free(tmp); }
    caseHead = NULL;
    caseCnt = 0;
    CaseNode* tail = NULL;
    CaseNode temp;
    while (fscanf_s(fp, "%d %s %s %[^@]@ %d %d %d %d %d %d %d %d %d %d %d %d-%d-%d %d-%d-%d %d-%d-%d",
        &temp.caseId,
        temp.patientId, (unsigned)_countof(temp.patientId),
        temp.doctorId, (unsigned)_countof(temp.doctorId),
        temp.illness, (unsigned)_countof(temp.illness),
        &temp.medId[0], &temp.medId[1], &temp.medId[2], &temp.medId[3], &temp.medId[4],
        &temp.medNum[0], &temp.medNum[1], &temp.medNum[2], &temp.medNum[3], &temp.medNum[4],
        &temp.bedId,
        &temp.diagDate.year, &temp.diagDate.month, &temp.diagDate.day,
        &temp.inDate.year, &temp.inDate.month, &temp.inDate.day,
        &temp.outDate.year, &temp.outDate.month, &temp.outDate.day) != EOF) {
        CaseNode* newNode = (CaseNode*)malloc(sizeof(CaseNode));
        memcpy(newNode, &temp, sizeof(CaseNode));
        newNode->next = NULL;
        if (caseHead == NULL) caseHead = tail = newNode;
        else { tail->next = newNode; tail = newNode; }
        caseCnt++;
        
    }
	
    fclose(fp);
    return 0;
}

// 加载所有本地数据
int LoadAllData(void) {
    printf("===================== 开始加载本地数据 =====================\n");
    if (LoadDoctorFromFile() == -1) GenDoctorData();
    if (LoadDoctorDutyFromFile() == -1) GenDoctorDuty();
    if (LoadMedicineFromFile() == -1) GenMedicineData();
    if (LoadExamFromFile() == -1) GenExamData();
    LoadLoginfFromFile();
    LoadPatientFromFile();
    LoadBedFromFile();
    LoadRegFromFile();
    LoadCaseFromFile();
    system("echo \r&cls");
    // 同步科室空闲床位数
    for (int d = NEIKE; d <= GUKE; d++) {
        int free = 0;
        BedNode* p = bedHead;
        while (p) {
            if (p->deptId == d && p->isFree == 1) free++;
            p = p->next;
        }
        depts[d - 1].freeBed = free;
        printProgressBar(d, GUKE);
        system("echo \r&cls");
    }
   // printf("数据加载完成：%d医生/%d药品/%d患者/%d挂号/%d病例\n",
   //     docCnt, medCnt, patCnt, regCnt, caseCnt);
//Sleep(100);
    system("cls");
    return 0;
}


void DisableResize() {
    HWND hwnd = GetConsoleWindow();
    SetWindowLong(hwnd, GWL_STYLE,
        GetWindowLong(hwnd, GWL_STYLE) & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX);
}
// 系统初始化核心函数
int InitSystem(void) {
    system("chcp 936");
    system("cls");
    DisableResize();
    printf("===================== 医院HIS管理系统初始化 =====================\n");
    char* files[] = { "LoginRes.txt", "DoctorData.txt", "PatientData.txt",
                       "BedData.txt", "MedData.txt",
                      "RegRecord.txt", "CaseData.txt", "DoctorDuty.txt", "ExamData.txt" };
    int fileNum = sizeof(files) / sizeof(char*);
    for (int i = 0; i < fileNum; i++) CheckAndCreateFile(files[i]);
    Date now = GetCurrentDate();
    printf("当前电脑系统时间：%d年%d月%d日 周%d\n",
        now.year, now.month, now.day, GetWeekDay(now));
    char* deptNames[] = { "内科", "外科", "妇产科", "儿科", "骨科" };
    system("echo \r&cls");
    for (int i = 0; i < MAX_DEPARTMENTS; i++) {
        depts[i].deptId = i + 1;
        strcpy_s(depts[i].deptName, NAME_LENGTH, deptNames[i]);
        depts[i].totalBed = 8;
        depts[i].freeBed = 8;
        printProgressBar(i + 1, MAX_DEPARTMENTS);
        system("echo \r&cls");
    }
    printf("科室/床位初始化完成：5科室，各8床位\n");
    printf("===================== 系统初始化成功 =====================\n");
    Sleep(100);
    system("cls");
    return 0;
}