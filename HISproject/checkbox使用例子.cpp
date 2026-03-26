#include <stdio.h>
#include<malloc.h>
#include<string.h>
// 假设你的节点定义如下（实际请按你的定义）
typedef struct node {
    int data;
    char name[20];          // 示例中需要名字，若无此字段可删除或修改打印内容
    struct node* next;
} node;




/**
 * 在控制台打印链表的每一项（带索引），并等待用户输入数字选择
 * @param root 链表头节点指针
 * @return 用户选择的节点指针，若链表为空或输入无效则返回 NULL
 */
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


main(){
	node* arr[15];
	node* tmp=NULL;
	char s[20];
	scanf("%s",s); 
	for(int i=0;i<10;i++){
	arr[i]=(node*)malloc(sizeof(node));	
	if(tmp!=NULL)tmp->next=arr[i];
	strcpy(arr[i]->name,s);
	arr[i]->next=NULL;
	tmp=arr[i];
	}
	checkbox(arr[0]);
	return 0; 

} 

