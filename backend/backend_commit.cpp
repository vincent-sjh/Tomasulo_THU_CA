#include <stdexcept>

#include "logger.h"
#include "processor.h"

/**
 * @brief 处理前端流出的指令
 *
 * @param inst 前端将要流出的指令（在流出buffer里面）
 * @return true 后端接受该指令
 * @return false 后端拒绝该指令
 */
bool Backend::dispatchInstruction([[maybe_unused]] const Instruction &inst) {
    if (!rob.canPush()) return false;

    // TODO: Check rob and reservation station is available for push.
    // NOTE: use getFUType to get instruction's target FU
    // NOTE: FUType::NONE only goes into ROB but not Reservation Stations
    Logger::Error("Instruction dispatch not implemented!");
    std::__throw_runtime_error("Instruction dispatch not implemented!");

    return false;
}

/**
 * @brief 后端完成指令提交
 *
 * @param entry 被提交的 ROBEntry
 * @param frontend 前端，用于调用前端接口
 * @return true 提交了 EXTRA::EXIT
 * @return false 其他情况
 */
bool Backend::commitInstruction([[maybe_unused]] const ROBEntry &entry,
                                [[maybe_unused]] Frontend &frontend) {
    using namespace RV32I;

    std::stringstream ss;
    ss << entry.inst;

    Logger::Info("Committing instruction %s: ", ss.str().c_str());
    Logger::Info("ROB index: %u", rob.getPopPtr());
    Logger::Info("rd: %s, value = %u",
                 xreg_name[entry.inst.getRd()].c_str(),
                 entry.state.result);

    // Exceptions are handled here
    // You don't need to change this exception handling code.

    if (entry.state.exception) {
        Logger::Error(
            "Committing an instruction with exception, fault address = %08x",
            entry.state.result);
        Logger::Error("Committing instruction %s: ", ss.str().c_str());
        Logger::Error("ROB index: %u", rob.getPopPtr());
        throw std::runtime_error("Exception encountered");
    }

    // TODO: Commit instructions here.
    // Return true when committing EXTRA::EXIT
    // NOTE: Be careful about Store Buffer!
    // NOTE: Re-executing load instructions when it is invalidated in load
    // buffer.
    // NOTE: Be careful about flush!

    // Optional TODO: Update your BTB when necessary

    Logger::Error("Committing area in backend.cpp is not implemented!");
    std::__throw_runtime_error(
        "Committing area in backend.cpp is not implemented!");
    return false;
}
