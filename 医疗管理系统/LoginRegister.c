#include "LoginRegister.h"
#include "Init.h"
#include "Tool.h"
int IsIdExist(const char* idcard) {
    if (strcmp(idcard, ADMIN_ID) == 0) return 1;
    Loginf* p = userroot;
    while (p != NULL) {
        if (strcmp(p->idcard, idcard) == 0) return 1;
        p = p->next;
    }
    return 0;
}

Loginf* CheckLogin(const char* idcard, const char* password) {
    if (strcmp(idcard, ADMIN_ID) == 0 && strcmp(password, ADMIN_PWD) == 0) {
        Loginf* admin = (Loginf*)malloc(sizeof(Loginf));
        strcpy_s(admin->idcard, ID_LENGTH, ADMIN_ID);
        strcpy_s(admin->password, PASSWORD_LENGTH, ADMIN_PWD);
        admin->role = ADMIN_ROLE;
        admin->next = NULL;
        return admin;
    }
    Loginf* p = userroot;
    while (p != NULL) {
        if (strcmp(p->idcard, idcard) == 0 && strcmp(p->password, password) == 0) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

/* ========== 注册时添加到链表 ========== */
int Register(void) {
    system("cls");
    PrintTitleBox("新用户注册");
    PrintInfo("请选择注册身份：1-患者  2-医生  3-管理员  (0 取消)");
    int roleChoice = -1;
    while (1) {
        printf("输入选项：");
        if (scanf_s("%d", &roleChoice) != 1) {
            int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}
            PrintError("输入无效，请重试。");
            continue;
        }
        if (roleChoice == 0) { system("cls"); return 0; }
        if (roleChoice >= 1 && roleChoice <= 3) break;
        PrintError("请选择 1/2/3 或 0 取消。");
    }
    while (getchar() != '\n');
    if (roleChoice != 1) {
        PrintInfo("请输入企业统一验证码：");
        char checkcodeinput[20];
        fgets(checkcodeinput, (unsigned)_countof(checkcodeinput), stdin);
        checkcodeinput[strcspn(checkcodeinput, "\n")] = '\0'; // 去掉换行符
        if (strcmp(checkcodeinput, CHACKCODE) != 0) {
            PrintError("验证码错误！注册失败！");
            Sleep(1500);
            system("cls");
            return 0;
        }
    }

    Loginf newUser;
    memset(&newUser, 0, sizeof(newUser));
    while (1) {
        printf("请输入18位身份证号：");
        fgets(newUser.idcard, (unsigned)_countof(newUser.idcard), stdin);
        newUser.idcard[strcspn(newUser.idcard, "\n")] = '\0'; // 去掉换行符
        if (strlen(newUser.idcard) != 18) {
            PrintError("身份证号必须为18位！(按0返回）");
            if (_getch() == '0') {
                system("cls");
                return 0;
            }
            continue;
        }

     
        int letterCount = 0;
        
        for (int i = 0; i < 18; i++) {
            if (    newUser.idcard[i]   >= '0'  && newUser.idcard[i] <= '9'  ) letterCount++;
        }
        if (letterCount < 16) {
            PrintError("身份证号中字母不能超过2个！(按0返回）");
            if (_getch() == '0') {
                system("cls");
                return 0;
            }
            continue;
        }
        
        if (IsIdExist(newUser.idcard)) {
            PrintError("该身份证号已注册！请直接登录或使用其它身份证号。");
            Sleep(1500);
            if (_getch() == '0') {
                system("cls");
                return 0;
            }
            return 0;
        }
        break;
    }
    while (1) {
        printf("请输入密码（6-16位）：");
        fgets(newUser.password, (unsigned)_countof(newUser.password), stdin);
        newUser.password[strcspn(newUser.password, "\n")] = '\0'; // 去掉换行符
        if (strlen(newUser.password) < 6 || strlen(newUser.password) > 16) {
            PrintError("密码长度必须6-16位！按任意键重新输入(按0返回)");
            if (_getch() == '0') {
                system("cls");
                return 0;
            }
            continue;
        }
        break;
    }
    while (1) {
        printf("请再次输入密码（6-16位）：");
        char confirmpassword[PASSWORD_LENGTH];
        fgets(confirmpassword, (unsigned)_countof(confirmpassword), stdin);
        confirmpassword[strcspn(confirmpassword, "\n")] = '\0'; // 去掉换行符
        if (strcmp(confirmpassword,newUser.password)!=0) {
            PrintError("两次密码不一致！按任意键重新输入(按0返回)");
            if (_getch() == '0') {
                system("cls");
                return 0;
            }
            continue;
        }
        break;
    }
    if (roleChoice == 1) {
        PatientNode* newPat = (PatientNode*)malloc(sizeof(PatientNode));
        memset(newPat, 0, sizeof(PatientNode));
       
        

        while (1) {
            printf("请输入患者姓名：");
            fgets(newPat->name, (unsigned)_countof(newPat->name), stdin);
            newPat->name[strcspn(newPat->name, "\n")] = '\0'; // 去掉换行符
            if (newPat->name[0] == '\0') {
                PrintError("姓名格式非法！按任意键重新输入(按0返回)");
                if (_getch() == '0') {
                    system("cls");
                    return 0;
                }
                continue;
            }
            break;
        }

        while (1) {
            printf("请输入性别（M-男 F-女）：");
            scanf_s(" %c", &newPat->gender, 1);
            if (newPat->gender == 'M' || newPat->gender == 'F' || newPat->gender == 'm' || newPat->gender == 'f') {
                newPat->gender = (char)toupper(newPat->gender);
                break;
            }
            PrintError("性别输入错误！");
        }
        strcpy_s(newPat->idcard, ID_LENGTH, newUser.idcard);
        newPat->isHospitalized = 0;
        newPat->bedId = 0;
        newPat->next = NULL;
		newPat->medicalRecord[0] = '\0';//缓存无@
        // 尾插
        if (patientHead == NULL) patientHead = newPat;
        else {
            PatientNode* tail = patientHead;
            while (tail->next) tail = tail->next;
            tail->next = newPat;
        }
        patCnt++;
        newUser.role = 1;
    }
    else if (roleChoice == 2) {
        DoctorNode* newDoc = (DoctorNode*)malloc(sizeof(DoctorNode));
        memset(newDoc, 0, sizeof(DoctorNode));
        strcpy_s(newDoc->idcard, ID_LENGTH, newUser.idcard);
        printf("请输入医生姓名：");
        scanf_s("%s", newDoc->name, (unsigned)_countof(newDoc->name));
        PrintInfo("请选择科室：1-内科 2-外科 3-妇产科 4-儿科 5-骨科");
        int dept = 0;
        while (1) {
            printf("输入科室编号(1-5)：");
            if (scanf_s("%d", &dept) != 1) { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} PrintError("输入无效。"); continue; }
            if (dept >= 1 && dept <= 5) break;
            PrintError("请输入1~5。");
        }
        newDoc->deptId = (DeptType)dept;
        printf("请输入职称：");
        scanf_s("%s", newDoc->title, (unsigned)_countof(newDoc->title));
        printf("请输入擅长：");
        scanf_s("%s", newDoc->specialty, (unsigned)_countof(newDoc->specialty));
        sprintf_s(newDoc->info, MAX_INFO, "%s医生，擅长%s。", newDoc->name, newDoc->specialty);
        newDoc->next = NULL;
        if (doctorHead == NULL) doctorHead = newDoc;
        else {
            DoctorNode* tail = doctorHead;
            while (tail->next) tail = tail->next;
            tail->next = newDoc;
        }
        docCnt++;
        newUser.role = 2;
    }
    else {
        newUser.role = 3;
    }

    // 添加到登录链表
    Loginf* newNode = (Loginf*)malloc(sizeof(Loginf));
    memcpy(newNode, &newUser, sizeof(Loginf));
    newNode->next = NULL;
    if (userroot == NULL) userroot = newNode;
    else {
        Loginf* tail = userroot;
        while (tail->next) tail = tail->next;
        tail->next = newNode;
    }

    if (roleChoice == 1) SavePatientToFile();
    if (roleChoice == 2) SaveDoctorToFile();
    SaveLoginfToFile();

    PrintSuccess("注册成功！");
    SetColor(11);
    printf("身份：%s | 身份证：%s\n",
        roleChoice == 1 ? "患者" : roleChoice == 2 ? "医生" : "管理员",
        newUser.idcard);
    ResetColor();
    PrintInfo("请牢记密码！");
    Sleep(2000);
    system("cls");
    return 1;
}

Loginf* Login(void) {
    system("cls");
    char idcard[ID_LENGTH], password[PASSWORD_LENGTH];
    while (1) {
        PrintHospitalBanner();
        PrintTitleBox("系统登录");
        PrintInfo("【管理员】管理并查询医院业务，包括医生，药品，缴费记录");
        PrintInfo("【医生】为患者诊断病情，开具药品，检查，住院建议");
        PrintInfo("【患者】请登录已注册的账号");
        PrintMenuBorder();
        printf("请输入身份证号/管理员账号：");
        scanf_s("%s", idcard, (unsigned)_countof(idcard));
        printf("请输入密码：");
        scanf_s("%s", password, (unsigned)_countof(password));

        Loginf* user = CheckLogin(idcard, password);
        if (user != NULL) {
            MessageBeep(MB_OK);
            PrintSuccess("登录成功！");
            Sleep(1200);
            system("cls");
            return user;
        }
        else {
            MessageBeep(MB_ICONERROR);
            PrintError("身份证号/密码错误！");
            PrintInfo("按任意键重新输入...(或按0返回主菜单)");
            if (_getch() == '0') {
                system("cls");
                return NULL;
            }
            system("cls");
        }
    }
}