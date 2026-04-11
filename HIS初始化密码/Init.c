#include "Init.h"

// ===================== 全局变量定义（与Typedef.hpp声明对应）=====================
Loginf* userroot = NULL;
Doctor doctors[MAX_DOCTORS];
Patient patients[MAX_PATIENTS];
Department depts[MAX_DEPARTMENTS];
Bed beds[MAX_BEDS];
Medicine meds[MAX_MEDICINE];
RegisterRecord regs[MAX_REG];
Case cases[MAX_CASE];
DoctorDuty docDuty[MAX_DEPARTMENTS * DUTY_DAYS];

int docCnt = 0, patCnt = 0, regCnt = 0, caseCnt = 0, medCnt = 0, dutyCnt = 0;
// 固定常量（需求定义）
const char ADMIN_ID[ID_LENGTH] = "admin";
const char ADMIN_PWD[PASSWORD_LENGTH] = "123456";
const int ADMIN_ROLE = 3;
const float FEE_TODAY = 20.0f;   // 当日挂号费20元
const float FEE_APPOINT = 50.0f; // 预约挂号费50元

// 检测并创建空文件：文件不存在则创建，存在则跳过
int CheckAndCreateFile(const char* filename) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, filename, "r");
    if (err != 0 || fp == NULL) {
        err = fopen_s(&fp, filename, "w");
        if (err != 0) {
            printf("创建文件【%s】失败！\n", filename);
            return -1;
        }
        printf("创建空文件【%s】成功！\n", filename);
        if (strcmp(filename, "BedData.txt") == 0) {
            // 初始化床位数据：每科室10床，默认全空闲
            for (int i = 0; i < MAX_BEDS; i++) {
                beds[i].bedId = i + 1;
                beds[i].deptId = (i / 10) + 1; // 1-10:内科，11-20:外科...41-50:骨科
                beds[i].isFree = 1; // 默认全空闲
                //fprintf(fp, "%d %d %d\n", beds[i].bedId, beds[i].deptId, beds[i].isFree);//从整个项目看，读写格式
                fprintf(fp, "%d %d %d %s %d-%d-%d\n",
                    beds[i].bedId, beds[i].deptId, beds[i].isFree,
                    beds[i].patientId, beds[i].inDate.year,
                    beds[i].inDate.month, beds[i].inDate.day);
				printf("####初始化床位数据：%d号床（%d）\n", beds[i].bedId, beds[i].isFree);
			}
            system("pause");
        }
    }
    fclose(fp);
    return 0;
}

// 获取当前电脑系统时间：全系统时间基准，无偏差
Date GetCurrentDate(void) {
    time_t now = time(NULL);
    struct tm t;
    localtime_s(&t, &now); // 安全版，适配VS
    Date date;
    date.year = t.tm_year + 1900;
    date.month = t.tm_mon + 1;
    date.day = t.tm_mday;
    return date;
}

// 计算日期对应的周几：1=周一，2=周二...7=周日
int GetWeekDay(Date date) {
    struct tm t = { 0 };
    t.tm_year = date.year - 1900;
    t.tm_mon = date.month - 1;
    t.tm_mday = date.day;
    mktime(&t); // 转换为时间戳
    int wday = t.tm_wday;
    return wday == 0 ? 7 : wday; // tm_wday:0=周日→转为7，1-6=周一至周六
}

// 校验预约日期：必须是未来7天内，且非过去日期
int CheckAppointDate(Date now, Date appoint) {
    // 转换为时间戳比较
    struct tm t_now = { 0 }, t_app = { 0 };
    t_now.tm_year = now.year - 1900;
    t_now.tm_mon = now.month - 1;
    t_now.tm_mday = now.day;
    t_app.tm_year = appoint.year - 1900;
    t_app.tm_mon = appoint.month - 1;
    t_app.tm_mday = appoint.day;
    time_t ts_now = mktime(&t_now);
    time_t ts_app = mktime(&t_app);
    if (ts_app < ts_now) return 0; // 过去日期，不合法
    // 计算天数差（≤7天为合法）
    int days = (ts_app - ts_now) / (24 * 3600);
    return days <= 7 ? 1 : 0;
}

void generate_password(char* pwd, int lastIdx, int firstIdx, const char* idcard) {
    char* lastNames[] = { "张", "李", "王", "刘", "陈", "杨", "赵", "黄", "周", "吴", "朱", "胡", "林", "马", "郭" };
    char* firstNames[] = { "明", "华", "伟", "芳", "丽", "军", "杰", "娟", "涛", "敏", "强", "燕", "浩", "婷", "伟" };
    // lastNamesPinyin 对应 lastNames 的全拼
    char* lastNamesPinyin[] = { "zhang", "li", "wang", "liu", "chen", "yang", "zhao", "huang", "zhou", "wu", "zhu", "hu", "lin", "ma", "guo" };
    // firstNamesPinyinInitial 对应 firstNames 的首字母
    char* firstNamesPinyinInitial[] = { "m", "h", "w", "f", "l", "j", "j", "j", "t", "m", "q", "y", "h", "t", "w" };

    const char* pinyinLast = lastNamesPinyin[lastIdx];
    // 获取名的拼音首字母
    const char* pinyinFirstInitial = firstNamesPinyinInitial[firstIdx];
    // 获取身份证后六位（18位身份证，后六位从下标12开始）
    const char* idLast6 = &idcard[12];

    // 组合成密码: 姓全拼 + 名首字母 + 身份证后六位
    sprintf_s(pwd, PASSWORD_LENGTH, "%s%s%s", pinyinLast, pinyinFirstInitial, idLast6);
}

// 生成30名医生测试数据：随机身份证/姓名/职称/科室
int GenDoctorData(void) {
    
    char* titles[] = { "主任医师", "副主任医师", "主治医师", "住院医师" };
    DeptType deptIds[] = { NEIKE, WAIKE, FUCHANKE, ERKE, GUKE };
    char* deptNames[] = { "内科", "外科", "妇产科", "儿科", "骨科" };
    char* specs[] = { "心血管疾病/呼吸内科", "普外科/骨科创伤", "妇科炎症/产科护理", "小儿感冒/儿科消化", "骨折/关节外科" };

    char* lastNames[] = { "张", "李", "王", "刘", "陈", "杨", "赵", "黄", "周", "吴", "朱", "胡", "林", "马", "郭" };
    char* firstNames[] = { "明", "华", "伟", "芳", "丽", "军", "杰", "娟", "涛", "敏", "强", "燕", "浩", "婷", "伟" };
    // lastNamesPinyin 对应 lastNames 的全拼
    char* lastNamesPinyin[] = { "zhang", "li", "wang", "liu", "chen", "yang", "zhao", "huang", "zhou", "wu", "zhu", "hu", "lin", "ma", "guo" };
    // firstNamesPinyinInitial 对应 firstNames 的首字母
    char* firstNamesPinyinInitial[] = { "m", "h", "w", "f", "l", "j", "j", "j", "t", "m", "q", "y", "h", "t", "w" };

    srand((unsigned)time(NULL));

    for (int i = 0; i < MAX_DOCTORS; i++) {
        // 1. 生成18位身份证号
        char id[ID_LENGTH] = "0";
        for (int j = 0; j < 17; j++) id[j] = '0' + rand() % 10;
        if (!rand())
            id[17] = 'X';
        else
            id[17] = rand() % 10 + '0';
        id[ID_LENGTH - 1] = '\0'; // 注意：字符串结尾应该是 ID_LENGTH - 1，不是 ID_LENGTH - 2
        strcpy_s(doctors[i].idcard, ID_LENGTH, id);

        // 2. 生成随机姓名并记录下标
        int lIdx = rand() % 15;
        int fIdx = rand() % 15;
        char name[NAME_LENGTH];
        strcpy_s(name, NAME_LENGTH, lastNames[lIdx]);
        strcat_s(name, NAME_LENGTH, firstNames[fIdx]);
        strcpy_s(doctors[i].name, NAME_LENGTH, name);
        printf("%s\n", doctors[i].name); // 加个换行打印更清晰

        // 3. 随机分配科室/职称/擅长
        doctors[i].deptId = (i / 6) + 1;
        strcpy_s(doctors[i].title, NAME_LENGTH, titles[rand() % 4]);
        strcpy_s(doctors[i].specialty, NAME_LENGTH, specs[doctors[i].deptId - 1]);

        // 4. 医生简介
        char info[MAX_INFO];
        sprintf_s(info, MAX_INFO, "从事%s临床工作%d年，擅长%s的诊断与治疗，临床经验丰富。",
            deptNames[doctors[i].deptId - 1], 5 + rand() % 25, doctors[i].specialty);
        strcpy_s(doctors[i].info, MAX_INFO, info);

        // 5. 同步到登录链表
        Loginf* newNode = (Loginf*)malloc(sizeof(Loginf));
        strcpy_s(newNode->idcard, ID_LENGTH, id);

        // --- 使用生成密码函数 ---
        char temp_pwd[PASSWORD_LENGTH];
        generate_password(temp_pwd, lIdx, fIdx, id);
        strcpy_s(newNode->password, PASSWORD_LENGTH, temp_pwd);
        newNode->role = 2;
        newNode->next = NULL;
        // 尾插法加入链表
        if (userroot == NULL) userroot = newNode;
        else {
            Loginf* p = userroot;
            while (p->next != NULL) p = p->next;
            p->next = newNode;
        }
    }
    docCnt = MAX_DOCTORS;
    printf("成功生成%d名医生数据（每科室6人）！\n", MAX_DOCTORS);
    SaveDoctorToFile(); // 持久化到DoctorData.txt
    SaveLoginfToFile();
    return 0;
}

// 生成医生值班表：5科室×7天，每日每科室1名医生（从科室所属6名医生中随机选）
int GenDoctorDuty(void) {
    dutyCnt = 0;
    char* deptNames[] = { "内科", "外科", "妇产科", "儿科", "骨科" };
    for (int d = NEIKE; d <= GUKE; d++) { // 遍历5个科室
        for (int w = 1; w <= DUTY_DAYS; w++) { // 遍历7天（1-7）
            docDuty[dutyCnt].deptId = d;
            docDuty[dutyCnt].weekDay = w;
            // 从科室所属6名医生中随机选1名值班（如内科：0-5号医生）
            int docStart = (d - 1) * 6; // 科室医生起始索引
            int randDoc = docStart + rand() % 6;
            strcpy_s(docDuty[dutyCnt].doctorId, ID_LENGTH, doctors[randDoc].idcard);
            dutyCnt++;
            printf("【%s】周%d值班医生：%s（ID：%s）\n",
                deptNames[d - 1], w, doctors[randDoc].name, doctors[randDoc].idcard);
        }
    }
    printf("成功生成医生值班表（5科室×7天，每日每科室1人）！\n");
    SaveDoctorDutyToFile(); // 持久化到DoctorDuty.txt
    return 0;
}

// 生成30类药品：5科室×6类，按科室绑定，随机生成名称/别名/价格/库存
int GenMedicineData(void) {
    medCnt = 0;
    // 各科室药品名库（每科室6个，按需求）
    char* medNames[5][MED_PER_DEPT] = {
        {"阿司匹林", "布洛芬", "头孢克肟", "氨溴索", "奥美拉唑", "硝苯地平"}, // 内科6类
        {"碘伏", "纱布", "云南白药", "阿莫西林", "破伤风疫苗", "红花油"},     // 外科6类
        {"益母草", "黄体酮", "头孢曲松", "碘伏", "妇科千金片", "叶酸"},       // 妇产科6类
        {"小儿氨酚黄那敏", "妈咪爱", "布洛芬混悬液", "头孢克洛", "蒙脱石散", "小儿豉翘"}, // 儿科6类
        {"钙片", "布洛芬", "云南白药", "接骨七厘片", "活血止痛膏", "氨糖"}    // 骨科6类
    };
    // 药品别名库（与药品名对应）
    char* medAliases[5][MED_PER_DEPT] = {
        {"乙酰水杨酸", "芬必得", "世福素", "沐舒坦", "洛赛克", "心痛定"}, // 内科别名
        {"碘酊", "医用纱布", "云白", "阿莫仙", "破免", "红药"},           // 外科别名
        {"益母膏", "孕酮", "菌必治", "碘酊", "千金片", "维生素B9"},       // 妇产科别名
        {"小儿氨酚", "益生菌", "美林", "希刻劳", "思密达", "豉翘"},       // 儿科别名
        {"碳酸钙", "芬必得", "云白", "接骨片", "止痛膏", "氨基葡萄糖"}    // 骨科别名
    };
    char* deptNames[] = { "内科", "外科", "妇产科", "儿科", "骨科" };

    srand((unsigned)time(NULL));
    for (int d = NEIKE; d <= GUKE; d++) { // 遍历5个科室
        for (int m = 0; m < MED_PER_DEPT; m++) { // 遍历每科室6类药品
            meds[medCnt].medId = medCnt + 1; // 药品编号1-30
            // 药品名称/别名（从库中取）
            strcpy_s(meds[medCnt].medName, NAME_LENGTH, medNames[d - 1][m]);
            strcpy_s(meds[medCnt].medAlias, MAX_MED_ALIAS, medAliases[d - 1][m]);
            // 随机药品类型（1-处方药，2-OTC）
            meds[medCnt].medType = rand() % 2 + 1;
            meds[medCnt].bindDept = d; // 严格绑定当前科室
            // 随机单价（1-100元，保留1位小数）
            meds[medCnt].price = (float)(rand() % 990 + 10) / 10.0f;
            meds[medCnt].stock = 100; // 初始库存100
            // 生产日期（近1年，随机）
            Date now = GetCurrentDate();
            meds[medCnt].prodDate.year = now.year - (rand() % 2);
            meds[medCnt].prodDate.month = rand() % 12 + 1;
            meds[medCnt].prodDate.day = rand() % 28 + 1;
            medCnt++;
        }
    }
    printf("成功生成%d类药品（5科室×6类，按科室绑定）！\n", MAX_MEDICINE);
    SaveMedicineToFile(); // 持久化到MedData.txt
    return 0;
}

// ===================== 数据持久化接口（内存→TXT）=====================
int SaveDoctorToFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "DoctorData.txt", "w");
    if (!fp) { printf("保存医生数据失败！\n"); return -1; }
    for (int i = 0; i < docCnt; i++) {
        fprintf(fp, "%s %s %d %s %s %s\n",
            doctors[i].idcard, doctors[i].name, doctors[i].deptId,
            doctors[i].title, doctors[i].specialty, doctors[i].info);
    }
    fclose(fp);
    return 0;
}

int SaveDoctorDutyToFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "DoctorDuty.txt", "w");
    if (err != 0) { printf("保存值班表失败！\n");return -1; }
    for (int i = 0; i < dutyCnt; i++) {
        fprintf(fp, "%d %d %s\n",
            docDuty[i].deptId, docDuty[i].weekDay, docDuty[i].doctorId);
    }
    fclose(fp);
    return 0;
}

int SaveMedicineToFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "MedData.txt", "w");
    if (err != 0) { printf("保存药品数据失败！\n");return -1; }
    for (int i = 0; i < medCnt; i++) {
        fprintf(fp, "%d %s %s %d %d %.1f %d %d-%d-%d\n",
            meds[i].medId, meds[i].medName, meds[i].medAlias,
            meds[i].medType, meds[i].bindDept, meds[i].price,
            meds[i].stock, meds[i].prodDate.year,
            meds[i].prodDate.month, meds[i].prodDate.day);
    }
    fclose(fp);
    return 0;
}

int SaveLoginfToFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "LoginRes.txt", "w");
    if (err != 0) { printf("保存登录数据失败！\n");  return -1; }
    fprintf(fp, "%s %s %d\n", ADMIN_ID, ADMIN_PWD, ADMIN_ROLE); // 管理员
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
    if (err != 0) { printf("保存患者数据失败！\n"); return -1; }
    for (int i = 0; i < patCnt; i++) {
        fprintf(fp, "%s %s %c %d %d %d-%d-%d %d-%d-%d %s\n",
            patients[i].idcard, patients[i].name, patients[i].gender,
            patients[i].isHospitalized, patients[i].bedId,
            patients[i].inDate.year, patients[i].inDate.month, patients[i].inDate.day,
            patients[i].outDate.year, patients[i].outDate.month, patients[i].outDate.day,
            patients[i].medicalRecord);
    }
    fclose(fp);
    return 0;
}

int SaveBedToFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "BedData.txt", "w");
    if (err != 0 || fp == NULL) { printf("保存床位数据失败！\n"); return -1; }
    for (int i = 0; i < MAX_BEDS; i++) {
        // 写入时用 "-" 表示空 patientId，保持与创建时一致
        fprintf(fp, "%d %d %d %s %d-%d-%d\n",
                beds[i].bedId, beds[i].deptId, beds[i].isFree,
                beds[i].patientId[0] ? beds[i].patientId : "-",
                beds[i].inDate.year, beds[i].inDate.month, beds[i].inDate.day);
        printf("床位数据：%02d号床(%s)%s\n", beds[i].bedId,
               depts[beds[i].deptId - 1].deptName, beds[i].isFree ? "空闲" : "占用");
    }
    fclose(fp);
    return 0;
}

int SaveRegToFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "RegRecord.txt", "w");
    if (err != 0) { printf("保存挂号记录失败！\n");return -1; }
    for (int i = 0; i < regCnt; i++) {
        fprintf(fp, "%d %s %d %s %d %.1f %d %d-%d-%d %d-%d-%d\n",
            regs[i].regId, regs[i].patientId, regs[i].deptId,
            regs[i].doctorId, regs[i].weekDay, regs[i].fee,
            regs[i].isToday, regs[i].regDate.year,
            regs[i].regDate.month, regs[i].regDate.day,
            regs[i].appointDate.year, regs[i].appointDate.month,
            regs[i].appointDate.day);
    }
    fclose(fp);
    return 0;
}

int SaveCaseToFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "CaseData.txt", "r+");
    if (err != 0) { printf("保存病例数据失败！\n");  return -1; }
    for (int i = 0; i < caseCnt; i++) {
        fprintf(fp, "%d %s %s %s %d %d %d %d %d %d %d %d-%d-%d %d-%d-%d %d-%d-%d\n",
            cases[i].caseId, cases[i].patientId, cases[i].doctorId,
            cases[i].illness, cases[i].medId[0], cases[i].medId[1],
            cases[i].medId[2], cases[i].medId[3], cases[i].medId[4],
            cases[i].bedId, cases[i].diagDate.year, cases[i].diagDate.month,
            cases[i].diagDate.day, cases[i].inDate.year, cases[i].inDate.month,
            cases[i].inDate.day, cases[i].outDate.year, cases[i].outDate.month,
            cases[i].outDate.day);
        // 写入药品数量
        //fprintf(fp, "%d %d %d %d %d\n",
        //    cases[i].medNum[0], cases[i].medNum[1],
        //    cases[i].medNum[2], cases[i].medNum[3], cases[i].medNum[4]);//重复了，
    }
    fclose(fp);
    return 0;
}

// ===================== 本地数据加载（TXT→内存）=====================
int LoadDoctorDutyFromFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "DoctorDuty.txt", "r");
    if (err != 0) {
        return -1;
    }
    dutyCnt = 0;
    while (fscanf_s(fp, "%d %d %s", &docDuty[dutyCnt].deptId,
        &docDuty[dutyCnt].weekDay, docDuty[dutyCnt].doctorId, (unsigned)_countof(docDuty[dutyCnt].doctorId)) != EOF) {
        dutyCnt++;
    }
    fclose(fp);
    if (dutyCnt == 0) {
        fclose(fp); return -1;
    }
   // system("pause");
    return 0;
}

int LoadMedicineFromFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "MedData.txt", "r");
    if (!fp) {  return -1; }
    medCnt = 0;
    while (fscanf_s(fp, "%d %s %s %d %d %f %d %d-%d-%d", &meds[medCnt].medId,
        meds[medCnt].medName, (unsigned)_countof(meds[medCnt].medName),
        meds[medCnt].medAlias, (unsigned)_countof(meds[medCnt].medAlias),
        &meds[medCnt].medType, &meds[medCnt].bindDept, &meds[medCnt].price,
        &meds[medCnt].stock, &meds[medCnt].prodDate.year,
        &meds[medCnt].prodDate.month, &meds[medCnt].prodDate.day) != EOF) {
        medCnt++;
    }
    if (medCnt == 0) { fclose(fp);return -1; }
    fclose(fp);
    return 0;
}

int LoadLoginfFromFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "LoginRes.txt", "r");
    if (err != 0) {  return -1; }
    userroot = NULL;

    Loginf* tail = NULL;
    char id[ID_LENGTH], pwd[PASSWORD_LENGTH];
    int role;
    while (fscanf_s(fp, "%s %s %d", id, (unsigned)_countof(id),
        pwd, (unsigned)_countof(pwd), &role) != EOF) {
        if (strcmp(id, ADMIN_ID) == 0) continue; // 管理员不加入链表
        Loginf* newNode = (Loginf*)malloc(sizeof(Loginf));
        strcpy_s(newNode->idcard, ID_LENGTH, id);
        strcpy_s(newNode->password, PASSWORD_LENGTH, pwd);
        newNode->role = role;
        newNode->next = NULL;

        if (userroot == NULL) {userroot = newNode; tail = newNode; }

        else { tail->next = newNode; tail = newNode; }
    }
    fclose(fp);
    return 0;
}

int LoadDoctorFromFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "DoctorData.txt", "r");
    if (!fp) return -1;

    docCnt = 0;
    while (fscanf_s(fp, "%s %s %d %s %s %s",
        doctors[docCnt].idcard, (unsigned)_countof(doctors[docCnt].idcard),
        doctors[docCnt].name, (unsigned)_countof(doctors[docCnt].name),
        &doctors[docCnt].deptId,
        doctors[docCnt].title, (unsigned)_countof(doctors[docCnt].title),
        doctors[docCnt].specialty, (unsigned)_countof(doctors[docCnt].specialty),
        doctors[docCnt].info, (unsigned)_countof(doctors[docCnt].info)) != EOF) {
        docCnt++;
    }

    fclose(fp);
    if (docCnt == 0) return -1;
    return 0;
}

int LoadPatientFromFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "PatientData.txt", "r");
    if (err != 0) {  return -1; }
    patCnt = 0;
    while (fscanf_s(fp, "%s %s %c %d %d %d-%d-%d %d-%d-%d",
        patients[patCnt].idcard, (unsigned)_countof(patients[patCnt].idcard),
        patients[patCnt].name, (unsigned)_countof(patients[patCnt].name),
        &patients[patCnt].gender, &patients[patCnt].isHospitalized,
        &patients[patCnt].bedId, &patients[patCnt].inDate.year,
        &patients[patCnt].inDate.month, &patients[patCnt].inDate.day,
        &patients[patCnt].outDate.year, &patients[patCnt].outDate.month,
        &patients[patCnt].outDate.day, patients[patCnt].medicalRecord,
        (unsigned)_countof(patients[patCnt].medicalRecord)) != EOF) {
        patCnt++;
    }
    fclose(fp);
    return 0;
}

int LoadBedFromFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "BedData.txt", "r");
    if (err != 0 || fp == NULL) {  return -1; }

    for (int i = 0; i < MAX_BEDS; i++) {
        char patientBuf[ID_LENGTH];
        int items = fscanf_s(fp, "%d %d %d %s %d-%d-%d",
                             &beds[i].bedId, &beds[i].deptId, &beds[i].isFree,
                             patientBuf, (unsigned)_countof(patientBuf),
                             &beds[i].inDate.year, &beds[i].inDate.month, &beds[i].inDate.day);
        // 如果读取失败或文件末尾/格式不完整，做安全初始化
        if (items == EOF || items < 7) {
            beds[i].bedId = i + 1;
            beds[i].deptId = (i / 10) + 1;
            beds[i].isFree = 1;
            beds[i].patientId[0] = '\0';
            beds[i].inDate.year = beds[i].inDate.month = beds[i].inDate.day = 0;
            continue;
        }
        // 约定 "-" 表示空 patientId
        if (strcmp(patientBuf, "-") == 0) beds[i].patientId[0] = '\0';
        else strcpy_s(beds[i].patientId, ID_LENGTH, patientBuf);

        printf("加载床位数据：床位ID=%d 科室ID=%d 是否空闲=%d\n",
               beds[i].bedId, beds[i].deptId, beds[i].isFree);
    }

    fclose(fp);
    return 0;
}

int LoadRegFromFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "RegRecord.txt", "r");
    if (err != 0) {  return -1; }
    regCnt = 0;
    while (fscanf_s(fp, "%d %s %d %s %d %f %d %d-%d-%d %d-%d-%d", &regs[regCnt].regId,
        regs[regCnt].patientId, (unsigned)_countof(regs[regCnt].patientId),
        &regs[regCnt].deptId, regs[regCnt].doctorId,
        (unsigned)_countof(regs[regCnt].doctorId), &regs[regCnt].weekDay,
        &regs[regCnt].fee, &regs[regCnt].isToday, &regs[regCnt].regDate.year,
        &regs[regCnt].regDate.month, &regs[regCnt].regDate.day,
        &regs[regCnt].appointDate.year, &regs[regCnt].appointDate.month,
        &regs[regCnt].appointDate.day) != EOF) {
        regCnt++;
    }
    fclose(fp);
    return 0;
}

int LoadCaseFromFile(void) {
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "CaseData.txt", "r");
    if (err != 0) {  return -1; }
    caseCnt = 0;//错了，应该是字符串的病例，
    while (fscanf_s(fp, "%d %s %s %s %d %d %d %d %d %d-%d-%d %d-%d-%d %d-%d-%d",
        &cases[caseCnt].caseId, cases[caseCnt].patientId,
        (unsigned)_countof(cases[caseCnt].patientId),
        cases[caseCnt].doctorId, (unsigned)_countof(cases[caseCnt].doctorId),
        cases[caseCnt].illness, (unsigned)_countof(cases[caseCnt].illness),
        &cases[caseCnt].medId[0], &cases[caseCnt].medId[1],
        &cases[caseCnt].medId[2], &cases[caseCnt].medId[3],
        &cases[caseCnt].medId[4], &cases[caseCnt].bedId,
        &cases[caseCnt].diagDate.year, &cases[caseCnt].diagDate.month,
        &cases[caseCnt].diagDate.day, &cases[caseCnt].inDate.year,
        &cases[caseCnt].inDate.month, &cases[caseCnt].inDate.day,
        &cases[caseCnt].outDate.year, &cases[caseCnt].outDate.month,
        &cases[caseCnt].outDate.day) != EOF) {
        // 读取药品数量
        //fscanf_s(fp, "%d %d %d %d %d", &cases[caseCnt].medNum[0],
        //    &cases[caseCnt].medNum[1], &cases[caseCnt].medNum[2],
        //    &cases[caseCnt].medNum[3], &cases[caseCnt].medNum[4]);
        //caseCnt++;
        //看一看读到什么
        printf("加载病例数据：病例ID=%d 患者ID=%s 医生ID=%s 疾病描述=%s\n",
               cases[caseCnt].caseId, cases[caseCnt].patientId,
			cases[caseCnt].doctorId, cases[caseCnt].illness);
    }
    fclose(fp);
    return 0;
}

// 加载所有本地数据
int LoadAllData(void) {
    printf("===================== 开始加载本地数据 =====================\n");
    // 优先加载医生/值班表/药品（核心依赖）
    if (LoadDoctorFromFile() == -1) { GenDoctorData();  } // 文件空则生成

    if (LoadDoctorDutyFromFile() == -1) GenDoctorDuty();
    
    if (LoadMedicineFromFile() == -1) GenMedicineData();
    
    //system("pause");

    // 加载其余数据
    LoadLoginfFromFile();
    LoadPatientFromFile();
    LoadBedFromFile();
    LoadRegFromFile();
    LoadCaseFromFile();
    // 同步科室空闲床位数
    for (int d = NEIKE; d <= GUKE; d++) {
        int free = 0;
        for (int i = 0; i < MAX_BEDS; i++) {
            if (beds[i].deptId == d && beds[i].isFree == 1) free++;
        }
        depts[d - 1].freeBed = free;
    }
    printf("数据加载完成：%d医生/%d药品/%d患者/%d挂号/%d病例\n",
        docCnt, medCnt, patCnt, regCnt, caseCnt);
    Sleep(2000);
    system("cls");
    return 0;
}

// 系统初始化核心函数
int InitSystem(void) {
    system("chcp 936"); // UTF-8编码，支持中文
    system("cls");
    printf("===================== 医院HIS管理系统初始化 =====================\n");
    // 1. 检测并创建9个TXT文件
    char* files[] = { "LoginRes.txt", "DoctorData.txt", "PatientData.txt",
                      "DeptData.txt", "BedData.txt", "MedData.txt",
                      "RegRecord.txt", "CaseData.txt", "DoctorDuty.txt" };
    int fileNum = sizeof(files) / sizeof(char*);
    for (int i = 0; i < fileNum; i++) CheckAndCreateFile(files[i]);
    // 2. 打印当前电脑系统时间
    Date now = GetCurrentDate();
    printf("当前电脑系统时间：%d年%d月%d日 周%d\n",
        now.year, now.month, now.day, GetWeekDay(now));
    // 3. 初始化科室/床位（5科室×10床位）
    char* deptNames[] = { "内科", "外科", "妇产科", "儿科", "骨科" };
    for (int i = 0; i < MAX_DEPARTMENTS; i++) {
        depts[i].deptId = i + 1;
        strcpy_s(depts[i].deptName, NAME_LENGTH, deptNames[i]);
        depts[i].totalBed = 10;
        depts[i].freeBed = 10;
    }
  //  for (int i = 0; i < MAX_BEDS; i++) {
  //      beds[i].bedId = i + 1;
  //      beds[i].deptId = (i / 10) + 1; // 1-10内科，11-20外科...41-50骨科
		//beds[i].isFree = 1;//代码错误，因为在LoadBedFromFile函数中，床位数据是从文件加载的，如果文件中没有数据或者数据不完整，那么床位的状态可能会被错误地设置为非空闲（isFree = 0）。因此，在初始化床位时，应该确保所有床位的状态都被正确地设置为初始状态，即空闲（isFree = 1）。以下是修正后的代码：

  //      beds[i].patientId[0] = '\0';
  //      beds[i].inDate.year = beds[i].inDate.month = beds[i].inDate.day = 0;
  //  }

    //SaveBedToFile(); // 初始化床位持久化，但不应该在这里，因为


    //GenbedData(); 应该写在
    printf("科室/床位初始化完成：5科室，各10床位（总计50）\n");
    printf("===================== 系统初始化成功 =====================\n");
    Sleep(2000);
    system("cls");
    return 0;
}