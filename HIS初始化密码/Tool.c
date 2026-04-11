#include "Tool.h"

// 根据身份证找医生
int FindDoctorById(const char* idcard) {
    for (int i = 0; i < docCnt; i++) {
        if (strcmp(doctors[i].idcard, idcard) == 0) return i;
    }
    return -1;
}

// 根据科室+周几找值班医生ID
char* FindDutyDoctor(DeptType deptId, int weekDay) {
    for (int i = 0; i < dutyCnt; i++) {
        if (docDuty[i].deptId == deptId && docDuty[i].weekDay == weekDay) {
            return docDuty[i].doctorId;
        }
    }
    return NULL;
}

// 药品模糊检索（名称/别名，不区分大小写）
int FindMedicineByKey(const char* key, int res[]) {
    int idx = 0;
    char keyLow[NAME_LENGTH], medNameLow[NAME_LENGTH], medAliasLow[MAX_MED_ALIAS];
    // 关键字转小写
    strcpy_s(keyLow, NAME_LENGTH, key);
    for (int i = 0; keyLow[i] != '\0'; i++) keyLow[i] = tolower(keyLow[i]);
    // 遍历药品
    for (int i = 0; i < medCnt; i++) {
        // 药品名转小写
        strcpy_s(medNameLow, NAME_LENGTH, meds[i].medName);
        for (int j = 0; medNameLow[j] != '\0'; j++) medNameLow[j] = tolower(medNameLow[j]);
        // 药品别名转小写
        strcpy_s(medAliasLow, MAX_MED_ALIAS, meds[i].medAlias);
        for (int j = 0; medAliasLow[j] != '\0'; j++) medAliasLow[j] = tolower(medAliasLow[j]);
        // 包含关键字则加入结果
        if (strstr(medNameLow, keyLow) != NULL || strstr(medAliasLow, keyLow) != NULL) {
            res[idx++] = i;
        }
    }
    res[idx] = -1; // 结束标记
    return idx;
}

// 根据身份证找患者
int FindPatientById(const char* idcard) {
    for (int i = 0; i < patCnt; i++) {
        if (strcmp(patients[i].idcard, idcard) == 0) return i;
    }
    return -1;
}

// 查找科室第一个空闲床位
int FindFreeBed(DeptType deptId) {
    for (int i = 0; i < MAX_BEDS; i++) {
        if (beds[i].deptId == deptId && beds[i].isFree == 1) {
            return beds[i].bedId;
        }
    }
    return -1;
}

// 释放床位
int FreeBed(int bedId) {
    if (bedId < 1 || bedId > MAX_BEDS) return 0;
    beds[bedId - 1].isFree = 1;
    beds[bedId - 1].patientId[0] = '\0';
    beds[bedId - 1].inDate.year = beds[bedId - 1].inDate.month = beds[bedId - 1].inDate.day = 0;
    // 更新科室空闲床位数
    DeptType dept = beds[bedId - 1].deptId;
    depts[dept - 1].freeBed++;
    SaveBedToFile(); // 持久化
    return 1;
}

// 扣减药品库存
int ReduceMedStock(int medId, int num) {
    if (medId < 1 || medId > medCnt || num <= 0) return 0;
    if (meds[medId - 1].stock < num) return 0; // 库存不足
    meds[medId - 1].stock -= num;
    SaveMedicineToFile(); // 实时持久化
    return 1;
}

// 格式化打印日期
void PrintDate(Date date, char* str) {
    sprintf_s(str, 20, "%d年%d月%d日", date.year, date.month, date.day);
}

// 清屏+等待按键
void PauseAndClear(void) {
    printf("\n按任意键继续...");
    _getch();
    system("cls");
}
// ===================== 界面美化函数实现 =====================
void SetColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void ResetColor(void) {
    SetColor(7);
}

void PrintHospitalBanner(void) {
    SetColor(11); // 亮青色
    printf("\n");
    printf("  ██╗  ██╗ ██████╗ ███████╗██████╗ ██╗████████╗ █████╗ ██╗\n");
    printf("  ██║  ██║██╔═══██╗██╔════╝██╔══██╗██║╚══██╔══╝██╔══██╗██║\n");
    printf("  ███████║██║   ██║███████╗██████╔╝██║   ██║   ███████║██║\n");
    printf("  ██╔══██║██║   ██║╚════██║██╔═══╝ ██║   ██║   ██╔══██║██║\n");
    printf("  ██║  ██║╚██████╔╝███████║██║     ██║   ██║   ██║  ██║███████╗\n");
    printf("  ╚═╝  ╚═╝ ╚═════╝ ╚══════╝╚═╝     ╚═╝   ╚═╝   ╚═╝  ╚═╝╚══════╝\n");
    printf("\n");
    SetColor(14); // 亮黄色
    printf("              ===========================================\n");
    printf("              ||      HIS 医院综合管理系统  v1.0       ||\n");
    printf("              ===========================================\n");
    ResetColor();
    printf("\n");
}

void PrintTitleBox(const char* title) {
    int len = (int)strlen(title);
    SetColor(10);
    printf("+");
    for (int i = 0; i < len + 4; i++) printf("-");
    printf("+\n");
    printf("|  %s  |\n", title);
    printf("+");
    for (int i = 0; i < len + 4; i++) printf("-");
    printf("+\n");
    ResetColor();
}

void PrintMenuBorder(void) {
    SetColor(9);
    printf("+");
    for (int i = 0; i < 62; i++) printf("-");
    printf("+\n");
    ResetColor();
}

void PrintMenuBottom(void) {
    SetColor(9);
    printf("+");
    for (int i = 0; i < 62; i++) printf("-");
    printf("+\n");
    ResetColor();
}

void PrintLoadingAnimation(int seconds) {
    char spin[4] = { '|', '/', '-', '\\' };
    printf("\n  ");
    for (int i = 0; i < seconds * 5; i++) {
        printf("\b%c", spin[i % 4]);
        Sleep(200);
    }
    printf("\b \n");
}

void PrintSuccess(const char* msg) {
    SetColor(10);
    printf("[√] %s\n", msg);
    ResetColor();
}

void PrintError(const char* msg) {
    SetColor(12);
    printf("[×] %s\n", msg);
    ResetColor();
}

void PrintInfo(const char* msg) {
    SetColor(14);
    printf("[i] %s\n", msg);
    ResetColor();
}