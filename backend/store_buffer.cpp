#include "store_buffer.h"

#include <stdexcept>

#include "logger.h"

StoreBuffer::StoreBuffer() {
    pushPtr = popPtr = 0;
    for (auto &slot : buffer) {
        slot.valid = false;
    }
}

/**
 * @brief 向ROB推入新项
 *
 * @param addr store地址
 * @param value store值
 */
void StoreBuffer::push(unsigned addr, unsigned value, unsigned robIdx) {
    buffer[pushPtr].storeAddress = addr;
    buffer[pushPtr].storeData = value;
    buffer[pushPtr].robIdx = robIdx;
    buffer[pushPtr].valid = true;
    Logger::Info("Store buffer push:");
    Logger::Info("Index: %u", pushPtr);
    Logger::Info("Address: %08x, value: %u\n", addr, value);
    pushPtr++;
    if (pushPtr == ROB_SIZE) {
        pushPtr -= ROB_SIZE;
    }
}

/**
 * @brief 从StoreBuffer中弹出一项，需保证Store Buffer中有项目
 *
 * @return StoreBufferSlot 弹出的StoreBuffer项目
 */
StoreBufferSlot StoreBuffer::pop() {
    auto ret = buffer[popPtr];
    Logger::Info("Store buffer pop:");
    Logger::Info("Index: %u", popPtr);
    Logger::Info("Address: %08x, value: %u\n", ret.storeAddress, ret.storeData);
    buffer[popPtr].valid = false;
    popPtr++;
    if (popPtr == ROB_SIZE) {
        popPtr -= ROB_SIZE;
    }
    return ret;
}

/**
 * @brief 返回 Store Buffer 队列首项
 *
 * @return StoreBufferSlot
 */
StoreBufferSlot StoreBuffer::front() { return buffer[popPtr]; }

/**
 * @brief 清空Store Buffer状态
 *
 */
void StoreBuffer::flush() {
    pushPtr = popPtr = 0;
    for (auto &slot : buffer) {
        slot.valid = false;
    }
}

/**
 * @brief 查询StoreBuffer中的内容
 *
 * @param addr 地址
 * @return std::optional<unsigned> 如果在Store
 * Buffer中命中，返回对应值，否则返回std::nullopt
 */
unsigned StoreBuffer::lastptr(unsigned currentptr){
    unsigned p = currentptr? currentptr - 1 : ROB_SIZE - 1;
    return p;
}
std::optional<unsigned> StoreBuffer::query(
    [[maybe_unused]] unsigned addr,
    [[maybe_unused]] unsigned robIdx,
    [[maybe_unused]] unsigned robPopPtr) {
    // DONE COMPLETELY: 完成 Store Buffer 的查询逻辑
    //throw std::runtime_error("Store Buffer query not implemented.");
    unsigned ptr = lastptr(pushPtr);
    unsigned mask = 0xFFFFFFFC;
    while (true) {
        bool addr_valid = buffer[ptr].valid;
        bool mask_valid = (buffer[ptr].storeAddress & mask) == (addr & mask);
        if(addr_valid && mask_valid) {
            return std::make_optional(buffer[ptr].storeData);
        }
        if (ptr == popPtr) {
            break;
        }
        ptr = lastptr(ptr);      
    }
    return std::nullopt;
}
