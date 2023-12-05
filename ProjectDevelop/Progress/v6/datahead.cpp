/**
* @file datahead.cpp
* @author Makaka
* @date 2023-12-05
* @brief 结构体的函数与定义
*/
#include <iostream>

struct DataPackHeader
{
    uint32_t firstLine; // 包类型(7b) + 信道(3b) + 优先级(6b) + 模块ID(4b) + 包长度(12b)
    uint32_t secondLine; // 包偏移量(12b) + 包序列号(20b)
    uint16_t dataSheetIdentificationNum; // 数据片标识号
    uint16_t targetId; // 目标ID
    uint16_t sourceId; // 源ID
    uint16_t nextHopId; // 下一跳ID
    uint16_t singleHopSourceId; // 单跳源ID
    uint16_t checkSum; // 校验和
    uint32_t validTime; // 有效时间
    uint8_t lastLine;  // 集群ID + 备用字段
};

struct DataPack {
    DataPackHeader header; // 包头
    uint32_t* payload; // 数据
};

DataPack makaka;

uint32_t getPackType(const DataPack& makaka) {
    uint32_t mask = 0b1111111; 
    uint32_t result = makaka.header.firstLine >> 25; 
    return result & mask; 
}
void setPackType(DataPack& makaka, uint32_t value) {
    uint32_t mask = 0b1111111 << 25; 
    makaka.header.firstLine = (makaka.header.firstLine & ~mask) | ((value & 0b1111111) << 25); 
}

uint32_t getChannel(const DataPack& makaka) {
    uint32_t mask = 0b111; 
    uint32_t result = makaka.header.firstLine >> 22; 
    return result & mask; 
}
void setChannel(DataPack& makaka, uint32_t value) {
    uint32_t mask = 0b111 << 22;  
    makaka.header.firstLine = (makaka.header.firstLine & ~mask) | ((value & 0b111) << 22);  
}

uint32_t getPacketPriority(const DataPack& makaka) {
    uint32_t mask = 0b111111;  
    uint32_t result = makaka.header.firstLine >> 16;  
    return result & mask; 
}
void setPacketPriority(DataPack& makaka, uint32_t value) {
    uint32_t mask = 0b111111 << 16;  
    makaka.header.firstLine = (makaka.header.firstLine & ~mask) | ((value & 0b111111) << 16);  
}

uint32_t getModuleId(const DataPack& makaka) {
    uint32_t mask = 0b1111; 
    uint32_t result = makaka.header.firstLine >> 12;  
    return result & mask;  
}
void setModuleId(DataPack& makaka, uint32_t value) {
    uint32_t mask = 0b1111 << 12; 
    makaka.header.firstLine = (makaka.header.firstLine & ~mask) | ((value & 0b1111) << 12); 
}

uint32_t getPackLength(const DataPack& makaka) {
    uint32_t mask = 0b111111111111; 
    uint32_t result = makaka.header.firstLine;  
    return result & mask; 
}
void setPackLength(DataPack& makaka, uint32_t value) {
    uint32_t mask = 0b111111111111;  
    makaka.header.firstLine = (makaka.header.firstLine & ~mask) | (value & 0b111111111111); 
}

uint32_t getPackOffset(const DataPack& makaka) {
    uint32_t mask = 0b111111111111; 
    uint32_t result = makaka.header.secondLine >> 20; 
    return result & mask; 
}
void setPackOffset(DataPack& makaka, uint32_t value) {
    uint32_t mask = 0b111111111111 << 20; 
    makaka.header.secondLine = (makaka.header.secondLine & ~mask) | ((value & 0b111111111111) << 20);
}

uint32_t getPackSequenceNum(const DataPack& makaka) {
    uint32_t mask = 0b11111111111111111111; 
    uint32_t result = makaka.header.secondLine; 
    return result & mask; 
}
void setPackSequenceNum(DataPack& makaka, uint32_t value) {
    uint32_t mask = 0b11111111111111111111; 
    makaka.header.secondLine = (makaka.header.secondLine & ~mask) | (value & 0b11111111111111111111); 
}

uint16_t getDataSheetIdentificationNum(const DataPack& makaka) {
    return makaka.header.dataSheetIdentificationNum;
}
void setDataSheetIdentificationNum(DataPack& makaka, uint16_t value) {
    makaka.header.dataSheetIdentificationNum = value;
}

uint16_t getTargetId(const DataPack& makaka) {
    return makaka.header.targetId;
}
void setTargetId(DataPack& makaka, uint16_t value) {
    makaka.header.targetId = value;
}

uint16_t getSourceId(const DataPack& makaka) {
    return makaka.header.sourceId;
}
void setSourceId(DataPack& makaka, uint16_t value) {
    makaka.header.sourceId = value;
}

uint16_t getNextHopId(const DataPack& makaka) {
    return makaka.header.nextHopId;
}
void setNextHopId(DataPack& makaka, uint16_t value) {
    makaka.header.nextHopId = value;
}

uint16_t getSingleHopSourceId(const DataPack& makaka) {
    return makaka.header.singleHopSourceId;
}
void setSingleHopSourceId(DataPack& makaka, uint16_t value) {
    makaka.header.singleHopSourceId = value;
}

uint16_t getCheckSum(const DataPack& makaka) {
    return makaka.header.checkSum;
}
void setCheckSum(DataPack& makaka, uint16_t value) {
    makaka.header.checkSum = value;
}

uint32_t getValidTime(const DataPack& makaka) {
    return makaka.header.validTime;
}
void setValidTime(DataPack& makaka, uint32_t value) {
    makaka.header.validTime = value;
}

uint32_t getClusterId(const DataPack& makaka) {
    uint32_t mask = 0b11111111;  
    uint32_t result = makaka.header.lastLine >> 8; 
    return result & mask;  
}
void setClusterId(DataPack& makaka, uint32_t value) {
    uint32_t mask = 0b11111111 << 8;  
    makaka.header.lastLine = (makaka.header.lastLine & ~mask) | ((value & 0b11111111) << 8); 
}

int main(){
    uint32_t firstLine = 0b11111111000111111000111111111111;
    uint32_t secondLine = 0b11111111111100000000000000000000;
    DataPack ma;
    ma.header.firstLine = firstLine;
    ma.header.secondLine = secondLine;
    
    std::cout << "PackType:" << getPackType(ma) << std::endl;
    std::cout << "Channel:" << getChannel(ma) << std::endl;
    std::cout << "PacketPriority:" << getPacketPriority(ma) << std::endl;
    std::cout << "ModuleId:" << getModuleId(ma) << std::endl;
    std::cout << "PackLength:" << getPackLength(ma) << std::endl;
    std::cout << "PackOffset:" << getPackOffset(ma) << std::endl;
    std::cout << "PackSequenceNum:" << getPackSequenceNum(ma) << std::endl;
    std::cout << "DataSheetIdentificationNum:" << getDataSheetIdentificationNum(ma) << std::endl;
    std::cout << "TargetId:" << getTargetId(ma) << std::endl;
    std::cout << "SourceId:" << getSourceId(ma) << std::endl;
    std::cout << "NextHopId:" << getNextHopId(ma) << std::endl;
    std::cout << "SingleHopSourceId:" << getSingleHopSourceId(ma) << std::endl;
    std::cout << "CheckSum:" << getCheckSum(ma) << std::endl;
    std::cout << "ValidTime:" << getValidTime(ma) << std::endl;
    std::cout << "ClusterId:" << getClusterId(ma) << std::endl;
    
    return 0;
}