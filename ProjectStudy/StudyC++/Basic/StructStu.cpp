#include <cstring>
#include <iostream>
using namespace std;

struct StructStu
{
    /* data */
    char name[10];
    int age;
};

void swap(int& x, int& y){  //
    int temp;
    temp = x; /* 保存地址 x 的值 */
    x = y;    /* 把 y 赋值给 x */
    y = temp; /* 把 x 赋值给 y  */
}

int main(){
    // StructStu Stu1;
    // strcpy(Stu1.name, "makaka");
    // Stu1.age = 11;
    // cout << "Name :  " << Stu1.name << "Age : " << Stu1.age << endl;

    // char site[7] = {'R', 'U', 'N', 'O', 'O', 'B', '\0'};
    // cout << sizeof(site) << endl;

    // int  *ptr = NULL;
    // cout << "ptr 的值是 " << ptr ;
    // return 0;

    int a = 1;
    int b = 2;
    swap(a, b);
    cout << "a : " << a << " b : " << b;
}


