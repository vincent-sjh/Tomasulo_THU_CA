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
    if (!rob.canPush()) {
        Logger::Info("ROB is full, cannot dispatch instruction.");
        return false;
    }

    // DONE COMPLETELY: Check rob and reservation station is available for push.
    // NOTE: use getFUType to get instruction's target FU
    // NOTE: FUType::NONE only goes into ROB but not Reservation Stations

    //Logger::Error("Instruction dispatch not implemented!");
    //std::__throw_runtime_error("Instruction dispatch not implemented!");
    switch (getFUType(inst)) {
        case FUType::ALU: {
            if (rsALU.hasEmptySlot()) {
            unsigned int robIdx = rob.push(inst, false);
            rsALU.insertInstruction(inst, robIdx, regFile, rob);
            regFile->markBusy(inst.getRd(), robIdx);
            return true;
            } else {
                //Logger::Error("ALU reservation station is full, cannot dispatch instruction.");
                return false;
            }
        }
        case FUType::BRU: {
            if (rsBRU.hasEmptySlot()) {
                unsigned int robIdx = rob.push(inst, false);
                rsBRU.insertInstruction(inst, robIdx, regFile, rob);
                regFile->markBusy(inst.getRd(), robIdx);
                return true;
            } else {
                Logger::Error("BRU reservation station is full, cannot dispatch instruction.");
                return false;
            }
        }
        case FUType::DIV: {
            if (rsDIV.hasEmptySlot()) {
                unsigned int robIdx = rob.push(inst, false);
                rsDIV.insertInstruction(inst, robIdx, regFile, rob);
                regFile->markBusy(inst.getRd(), robIdx);
                return true;
            } else {
                Logger::Error("DIV reservation station is full, cannot dispatch instruction.");
                return false;
            }
        }
        case FUType::MUL: {
            if (rsMUL.hasEmptySlot()) {
                unsigned int robIdx = rob.push(inst, false);
                rsMUL.insertInstruction(inst, robIdx, regFile, rob);
                regFile->markBusy(inst.getRd(), robIdx);
                return true;
            } else {
                Logger::Error("MUL reservation station is full, cannot dispatch instruction.");
                return false;
            }
        }
        case FUType::LSU: {
            if (rsLSU.hasEmptySlot()) {
                unsigned int robIdx = rob.push(inst, false);
                rsLSU.insertInstruction(inst, robIdx, regFile, rob);
                regFile->markBusy(inst.getRd(), robIdx);
                return true;
            } else {
                //Logger::Error("LSU reservation station is full, cannot dispatch instruction.");
                return false;
            }
        }
        case FUType::NONE: {
            unsigned int robIdx = rob.push(inst, true);
            regFile->markBusy(inst.getRd(), robIdx);
            return true;
        }
        default: {
            Logger::Error("Unhandled FUType: %d", static_cast<int>(getFUType(inst)));
            std::__throw_runtime_error("Unhandled FUType in dispatchInstruction!");
            return false;
        }
    }
    // 这里是一个错误的情况，应该不会到达这里
    Logger::Error("Dispatch instruction failed!");
    std::__throw_runtime_error("Dispatch instruction failed!");
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
    // using namespace RV32I;

    // std::stringstream ss;
    // ss << entry.inst;

    // Logger::Info("Committing instruction %s: ", ss.str().c_str());
    // Logger::Info("ROB index: %u", rob.getPopPtr());
    // Logger::Info("rd: %s, value = %u",
    //              xreg_name[entry.inst.getRd()].c_str(),
    //              entry.state.result);

    // Exceptions are handled here
    // You don't need to change this exception handling code.

    // if (entry.state.exception) {
    //     Logger::Error(
    //         "Committing an instruction with exception, fault address = %08x",
    //         entry.state.result);
    //     Logger::Error("Committing instruction %s: ", ss.str().c_str());
    //     Logger::Error("ROB index: %u", rob.getPopPtr());
    //     throw std::runtime_error("Exception encountered");
    // }

    // DONE COMPLETELY: Commit instructions here.
    // Return true when committing EXTRA::EXIT
    // NOTE: Be careful about Store Buffer!
    // NOTE: Re-executing load instructions when it is invalidated in load
    // buffer.
    // NOTE: Be careful about flush!

    // TODO: Update your BTB when necessary

    //Logger::Error("Committing area in backend.cpp is not implemented!");
    //std::__throw_runtime_error("Committing area in backend.cpp is not implemented!");
    using namespace RV32I;

    FUType ftype = getFUType(entry.inst);
    switch (ftype) {
    case FUType::ALU:
        break;
    case FUType::DIV:
        break;
    case FUType::MUL:
        break;
    case FUType::BRU:{
        if (entry.inst == BEQ || entry.inst == BNE || entry.inst == BLT ||
            entry.inst == BGE || entry.inst == BLTU || entry.inst == BGEU) {
            BpuUpdateData data{};
            data.pc = entry.inst.pc;
            data.isCall = false;
            data.isReturn = false;
            data.isBranch = true;
            data.branchTaken = entry.state.actualTaken;
            data.jumpTarget = entry.state.jumpTarget;
            frontend.bpuBackendUpdate(data);
        }
        break;
    }
    case FUType::LSU:{
        //store
        bool is_store = entry.inst == RV32I::SB || entry.inst == RV32I::SH || entry.inst == RV32I::SW;
        bool is_load = entry.inst == RV32I::LB || entry.inst == RV32I::LH ||
            entry.inst == RV32I::LBU || entry.inst == RV32I::LHU || entry.inst == RV32I::LW;
        if (is_store) { 
            // Store
            StoreBufferSlot stSlot = storeBuffer.front();
            bool status = writeMemoryHierarchy(stSlot.storeAddress, stSlot.storeData, 0xF);
            if (!status) {
                return false;
            } else {
                storeBuffer.pop();
            }
        }
        //load
        if (is_load) {
            // Load
            LoadBufferSlot ldSlot = loadBuffer.pop(rob.getPopPtr());
            if (ldSlot.invalidate) {
                frontend.jump(entry.inst.pc);
                flush();
                return false;
            }
        }
        break;
    }
    case FUType::NONE:
        break;
    default:
        Logger::Error("Unhandled FUType: %d", static_cast<int>(ftype));
        //std::__throw_runtime_error("Unhandled FUType in commitInstruction!");
        break;
    }
    regFile->write(entry.inst.getRd(), entry.state.result, rob.getPopPtr());
    rob.pop();
    if (entry.state.mispredict) {
        frontend.jump(entry.state.actualTaken ? entry.state.jumpTarget : entry.inst.pc + 4);
        flush();
    }
    return (entry.inst == EXTRA::EXIT)? true : false;
}
