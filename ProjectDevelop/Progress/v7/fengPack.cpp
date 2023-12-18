/**
* @file fengPack.cpp
* @author Makaka
* @date 2023-12-14
* @brief 封包
*/
#include "scsnDataHead.h"



int main(){
    DataPack dataToSend;
    dataToSend.setPackType(0b1);
    dataToSend.setDataSheetIdentificationNum(0b111);

    void *payload = malloc(10);
    int a = 100;
    int *ptr = &a;
    char str = 'a';
    // memcpy(payload, ptr, sizeof(int));
    // memcpy(payload + sizeof(int), &str, sizeof(char));

    // data.payload = payload;
    // std::cout << *(int *)data.payload << std::endl;
    // std::cout << *((char *)data.payload + sizeof(int)) << std::endl;

    std::cout << "大小" << sizeof(dataToSend) << std::endl;
    return 0;
}