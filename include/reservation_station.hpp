#pragma once
#include <algorithm>
#include <memory>
#include <sstream>

#include "instructions.h"
#include "issue_slot.h"
#include "logger.h"
#include "register_file.h"
#include "rob.h"

template <unsigned size>
class ReservationStation {
    IssueSlot buffer[size];

public:
    ReservationStation();
    [[nodiscard]] bool hasEmptySlot() const;
    void insertInstruction(const Instruction &inst,
                           unsigned robIdx,
                           RegisterFile *regFile,
                           const ReorderBuffer &reorderBuffer);
    void wakeup(const ROBStatusWritePort &x);
    [[nodiscard]] bool canIssue() const;
    IssueSlot issue();
    void flush();
    void compressBuffer(unsigned k, IssueSlot entry);
};

template <unsigned size>
ReservationStation<size>::ReservationStation() {
    for (auto &slot : buffer) {
        slot.busy = false;
    }
}

template <unsigned size>
bool ReservationStation<size>::hasEmptySlot() const {
    return std::any_of(buffer, buffer + size, [](const IssueSlot &slot) {
        return !slot.busy;
    });
}

template <unsigned size>
void ReservationStation<size>::insertInstruction(
    [[maybe_unused]] const Instruction &inst,
    [[maybe_unused]] unsigned robIdx,
    [[maybe_unused]] RegisterFile *const regFile,
    [[maybe_unused]] const ReorderBuffer &reorderBuffer) {
    for (IssueSlot &slot : buffer) {
        //slot busy，返回
        if (slot.busy) continue;
        // DONE COMPLETELY: Dispatch instruction to this slot
        //Logger::Error("Dispatching instruction is not implemented");
        //std::__throw_runtime_error("Dispatching instruction is not implemented");
        //若有slot空闲，插入,设置两个寄存器读取端口
        //slot busy，返回
        unsigned reg1 = inst.getRs1();
        unsigned robIdx1 = regFile->getBusyIndex(reg1);
		if (regFile->isBusy(reg1)) {
			if (reorderBuffer.checkReady(robIdx1)) {
                slot.updateReadPort(true,false,0,reorderBuffer.read(robIdx1));
			}
			else {
                slot.updateReadPort(true,true,robIdx1,0);
			}		
		}
		else {
            slot.updateReadPort(true,false,0,regFile->read(reg1));
		}
		unsigned reg2 = inst.getRs2();
		unsigned robIdx2 = regFile->getBusyIndex(reg2);
		if (regFile->isBusy(reg2)) {
			if (reorderBuffer.checkReady(robIdx2)) {
                slot.updateReadPort(false,false,0,reorderBuffer.read(robIdx2));
			}
			else {
                slot.updateReadPort(false,true,robIdx2,0);
			}
		}
		else {
            slot.updateReadPort(false,false,0,regFile->read(reg2));
		}
        slot.busy = true;
        slot.inst = inst;
        slot.robIdx = robIdx;
		return;
    }
    Logger::Error("ReservationStation::insertInstruction no empty slots!");
    std::__throw_runtime_error("ReservationStation::insertInstruction no empty slots!");
    return;

}

template <unsigned size>
void ReservationStation<size>::wakeup(
    [[maybe_unused]] const ROBStatusWritePort &x) {
    // : Wakeup instructions according to ROB Write
    //Logger::Error("Wakeup not implemented");
    //std::__throw_runtime_error("Wakeup not implemented");
    //查看每个 slot 的寄存器读取端口是否已经唤醒;如果未唤醒，比对信息，尝试唤醒指令
    for(IssueSlot &slot : buffer){
        if(slot.busy){
            if(slot.readPort1.waitForWakeup && slot.readPort1.robIdx == x.robIdx){
                slot.updateReadPort(true,false,x.robIdx,x.result);
            }
            if(slot.readPort2.waitForWakeup && slot.readPort2.robIdx == x.robIdx){
                slot.updateReadPort(false,false,x.robIdx,x.result);
            }
        }
    }
    return;
}

template <unsigned size>
bool ReservationStation<size>::canIssue() const {
    // DONE COMPLETELY: Decide whether an issueSlot is ready to issue.
    // Warning: Store instructions must be issued in order!!
    // Warning: Store instructions must be issued in order!!
    bool LSU_valid = buffer[0].busy && getFUType(buffer[0].inst) == FUType::LSU;
    for (const IssueSlot &slot : buffer) {
        Instruction inst = slot.inst;
        bool is_load = inst == RV32I::LB || inst == RV32I::LH || inst == RV32I::LHU || inst == RV32I::LW || inst == RV32I::LBU;
        bool is_store = inst == RV32I::SB || inst == RV32I::SH || inst == RV32I::SW;
        bool slot_valid = slot.busy && !slot.readPort1.waitForWakeup && !slot.readPort2.waitForWakeup;
        if (is_load||is_store) {
            if (slot_valid) {
                return true;
            }
        }
        if(LSU_valid) continue;
        if (slot_valid) {
            return true;
        }
    }
    return false;
}

template <unsigned size>
IssueSlot ReservationStation<size>::issue() {
    // DONE COMPLETELY: Issue a ready issue slot and remove it from reservation station.
    // Warning: Store instructions must be issued in order!!
    //Logger::Error("No available slots for issuing");
    //std::__throw_runtime_error("No available slots for issuing");

    bool LSU_valid = buffer[0].busy && getFUType(buffer[0].inst) == FUType::LSU;
	if(LSU_valid) {
		bool initial = false;
		IssueSlot entry;
		unsigned k = 0;
		for (k = 0; k < size; k++) {
			IssueSlot slot = buffer[k];
			Instruction inst = slot.inst;
			bool is_load = inst == RV32I::LB || inst == RV32I::LH || inst == RV32I::LHU || inst == RV32I::LW || inst == RV32I::LBU;
            bool is_store = inst == RV32I::SB || inst == RV32I::SH || inst == RV32I::SW;
            bool slot_valid = slot.busy && !slot.readPort1.waitForWakeup && !slot.readPort2.waitForWakeup;
			if (is_load) {
				if (slot_valid) {
					entry = slot;
					slot.busy = false;
					compressBuffer(k, entry);
                    return entry;
				}
			}
			if(is_store) {
				if (initial == false) {
					initial = true;
					if (slot_valid) {
						entry = slot;
						slot.busy = false;
						compressBuffer(k, entry);
                        return entry; 
					}
				}
			}
		}
        return IssueSlot{};
	}
	else {
		IssueSlot entry;
		unsigned k = 0;
		for (k = 0; k < size; k++) {
			IssueSlot slot = buffer[k];
            bool slot_valid = slot.busy && !slot.readPort1.waitForWakeup && !slot.readPort2.waitForWakeup;
			if (slot_valid) {
				entry = slot;
				slot.busy = false;
                compressBuffer(k, entry);
                return entry;
			}
		}
        return IssueSlot{};
	}
}




template <unsigned size>
void ReservationStation<size>::flush() {
    for (auto &slot : buffer) {
        slot.busy = false;
    }
}


template <unsigned size>
void ReservationStation<size>::compressBuffer(unsigned pos, IssueSlot entry) {
    unsigned idx = pos;
    while (idx < size - 1) {
        buffer[idx] = std::move(buffer[idx + 1]);
        ++idx;
    }
    buffer[size - 1] = std::move(entry);
    buffer[size - 1].busy = false;
}