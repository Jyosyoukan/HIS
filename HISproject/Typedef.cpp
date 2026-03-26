/*
全局函数全在这里定义，方便修改和维护
任何文件都应该#include这个头文件
*/
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include<windows.h>
#include"Typedef.hpp"
// --- 全局数据存储 (例如：使用数组，实际课设可以用文件加载/保存) ---
//extern Patient patients[MAX_PATIENTS];
//extern Doctor doctors[MAX_DOCTORS];
//extern Department departments[MAX_DEPT];
//extern Medicine medicines[MAX_MEDICINE];
//extern Bed beds[TOTAL_BEDS];
//extern MedicalRecord records[MAX_RECORDS]; // 假设最多只有住院患者的病历（改用 MAX_RECORDS）
//extern Registration registrations[MAX_PATIENTS * 5]; // 假设一个病人最多挂5次号
//// ... 可能还需要计数器
//extern int patient_count;
//extern int doctor_count;
//extern int medicine_count;
//extern int record_count;
//extern int registration_count;
//extern int bed_count; // 初始化时所有床位可用



Patient* checkbox(Patient* root) {
    if (root == NULL) {
        printf("链表为空，无法选择。\n");
        return NULL;
    }
    // 1. 遍历链表，记录所有节点指针（假设节点数不超过 100，可根据需要调整）
    Patient* nodes[100];
    int count = 0;
    Patient* cur = root;
    while (cur != NULL && count < 100) {
        nodes[count] = cur;
        // 打印索引和节点信息（这里打印 name，若无 name 可改为 cur->data）
        printf("[%d] %s\n", count, cur->name);
        count++;
        cur = cur->next;
    }

    // 2. 获取用户输入
    int choice;
    printf("用户（提示）； ");
    // 使用 scanf_s 读取数字（若编译器不支持，可改为 scanf）
    if (scanf_s("%d", &choice) != 1) {
        // 输入不是数字，清空缓冲区
        while (getchar() != '\n');
        printf("输入无效，请重新运行。\n");
        return NULL;
    }

    // 3. 检查范围
    if (choice < 0 || choice >= count) {
        printf("选择超出范围（0 ~ %d）。\n", count - 1);
        return NULL;
    }

    // 4. 返回选中的节点指针
    return nodes[choice];
}


