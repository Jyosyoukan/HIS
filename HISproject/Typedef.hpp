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
#define MAX_DOCTORS 50  //最大医生数
#define MAX_DEPARTMENTS 5 //最大科室数
#define MAX_APPOINTMENTS 200 //最大预约数
#define MAX_RECORDS 200 //最大病历数
#define MAX_MEDICINE 200 //最大处方数
#define MAX_MEDICINE_NAME 50 //药品名字长度



typedef struct Date{
	int year;
	int month;
	int day;
}Date;
typedef struct Time {
	int hour;
	int minute;
	int second;
}Time;
typedef struct Patient{//链表实现
	char id[ID_LENGTH];
    char name[NAME_LENGTH];

	int gender;//0女1男

	Patient* next;
}Patient;


node* checkbox(node* root) {
    if (root == NULL) {
        printf("链表为空，无法选择。\n");
        return NULL;
    }
    // 1. 遍历链表，记录所有节点指针（假设节点数不超过 100，可根据需要调整）
    node* nodes[100];
    int count = 0;
    node* cur = root;
    while (cur != NULL && count < 100) {
        nodes[count] = cur;
        // 打印索引和节点信息（这里打印 name，若无 name 可改为 cur->data）
        printf("[%d] %s\n", count, cur->name);
        count++;
        cur = cur->next;
    }

    // 2. 获取用户输入
    int choice;
    printf("用户（提示）：");
    // 使用 scanf_s 读取数字（若编译器不支持，可改为 scanf）
    if (scanf("%d", &choice) != 1) {
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
