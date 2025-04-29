#pragma once
#include "instructions.h"

struct RegReadBundle {
    bool waitForWakeup;
    unsigned robIdx;
    unsigned value;
};

struct IssueSlot {
    Instruction inst{};
    RegReadBundle readPort1{}, readPort2{};
    unsigned robIdx = 0u;
    bool busy = false;

    void updateReadPort(bool selectPort1, bool waitForWakeup, unsigned newRobIdx, unsigned newValue) {
        if (selectPort1) {
            readPort1.waitForWakeup = waitForWakeup;
            readPort1.robIdx = newRobIdx;
            readPort1.value = newValue;
        } else {
            readPort2.waitForWakeup = waitForWakeup;
            readPort2.robIdx = newRobIdx;
            readPort2.value = newValue;
        }
        return;    
    }
};

