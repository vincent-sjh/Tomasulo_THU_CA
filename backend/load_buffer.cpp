#include "load_buffer.h"

#include <algorithm>
#include <stdexcept>

#include "defines.h"
#include "logger.h"

LoadBuffer::LoadBuffer() {
    std::for_each(buffer, buffer + ROB_SIZE, [](LoadBufferSlot &slot) {
        slot.valid = false;
    });
}

/**
 * @brief LoadBuffer 推入新项目
 *
 * @param addr load 地址
 * @param robIdx rob 编号
 */
void LoadBuffer::push(unsigned addr, unsigned robIdx) {
    buffer[robIdx].loadAddress = addr;
    buffer[robIdx].robIdx = robIdx;
    buffer[robIdx].valid = true;
    buffer[robIdx].invalidate = false;
    Logger::Info("Load buffer push:");
    Logger::Info("Index: %u", robIdx);
    Logger::Info("Address: %08x, robIdx: %u\n", addr, robIdx);
}

/**
 * @brief LoadBuffer 弹出项目
 *
 * @param robIdx rob 地址
 * @return LoadBufferSlot 返回对应项目
 */
LoadBufferSlot LoadBuffer::pop(unsigned robIdx) {
    Logger::Info("Load buffer pop");
    buffer[robIdx].valid = false;
    return buffer[robIdx];
}

/**
 * @brief 刷新 LoadBuffer 状态
 *
 */
void LoadBuffer::flush() {
    std::for_each(buffer, buffer + ROB_SIZE, [](LoadBufferSlot &slot) {
        slot.valid = false;
    });
}

/**
 * @brief 查询并无效化 load buffer 的对应项目
 *
 * @param addr store 地址
 * @param robIdx store 指令的 rob 编号
 * @param robPopPtr 当前 rob 的 pop 指针
 */
void LoadBuffer::check([[maybe_unused]] unsigned addr,
                       [[maybe_unused]] unsigned robIdx,
                       [[maybe_unused]] unsigned robPopPtr,
                       [[maybe_unused]] unsigned robPushPtr) {
    // DONE COMPLETELY: 完成 Load Buffer 的检验逻辑，寻找顺序错误的 load 指令
    //throw std::runtime_error("Load Buffer Check not implemented");
    for (unsigned i = 0; i < ROB_SIZE; ++i) {
        LoadBufferSlot& slot = buffer[i];
        unsigned load_robIdx = slot.robIdx;
        bool rob_valid = ((load_robIdx > robIdx && robIdx >= robPopPtr) ||
        (load_robIdx < robPopPtr && robIdx >= robPopPtr) ||
        (load_robIdx > robIdx && load_robIdx < robPopPtr));
        if (slot.valid && rob_valid) {
            slot.invalidate = true;
            Logger::Info("Invalidate load buffer:");
            Logger::Info("Index: %u", slot.robIdx);
            Logger::Info("Address: %08x, robIdx: %u\n", slot.loadAddress, slot.robIdx);
        }
    }
}
