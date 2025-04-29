#include "processor.h"
#include "with_predict.h"
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define BUFFER_DEPTH 2
FrontendWithPredict::FrontendWithPredict(const std::vector<unsigned> &inst)
    : Frontend(inst) {
    for (auto &entry : btb) {
        entry.valid = false;
    }
}

/**
 * @brief 获取指令的分支预测结果，分支预测时需要
 *
 * @param pc 指令的pc
 * @return BranchPredictBundle 分支预测的结构
 */
BranchPredictBundle FrontendWithPredict::bpuFrontendUpdate(unsigned int pc) {
    //DONE COMPLETELY
    // branch predictions
    // 取最低10位作为index
    // 因为btb大小为1024个条目
    auto index = pc & 0x3FF;
    bool btb_match = btb[index].valid && (btb[index].pc == pc);
    bool taken = (btb[index].state & (1<<(BUFFER_DEPTH-1))) && btb_match;
    return taken? BranchPredictBundle{.predictJump = true, .predictTarget = btb[index].target} : BranchPredictBundle{.predictJump = false, .predictTarget = pc + 4};
}

/**
 * @brief 用于计算NextPC，分支预测时需要
 *
 * @param pc
 * @return unsigned
 */
unsigned FrontendWithPredict::calculateNextPC(unsigned pc) const {
    //DONE COMPLETELY
    // branch predictions
    auto index = pc & 0x3FF;
    bool btb_match = btb[index].valid && (btb[index].pc == pc);
    bool taken = (btb[index].state & (1<<(BUFFER_DEPTH-1))) && btb_match;
    return taken? btb[index].target : pc + 4;
}

/**
 * @brief 用于后端提交指令时更新分支预测器状态，分支预测时需要
 *
 * @param x
 */
void FrontendWithPredict::bpuBackendUpdate(const BpuUpdateData &x) {
    //DONE COMPLETELY
    // Optional branch predictions
        auto index = x.pc & 0x3FF;
        bool btb_match = btb[index].valid && (btb[index].pc == x.pc);
        if (btb_match) {
            btb[index].state = (x.branchTaken) ? min(btb[index].state+1, (1<<BUFFER_DEPTH)-1) : max(btb[index].state-1, 0);
            btb[index].target = x.jumpTarget;
        }else {
            btb[index].state = x.branchTaken ? (1<<(BUFFER_DEPTH-1)) : (1<<(BUFFER_DEPTH-1))-1;
            btb[index].valid = true;
            btb[index].pc = x.pc;
            btb[index].target = x.jumpTarget;
            btb[index].counter = 0;
        }
}

/**
 * @brief 重置前端状态
 *
 * @param inst
 * @param entry
 */
void FrontendWithPredict::reset(const std::vector<unsigned int> &inst,
                                unsigned int entry) {
    // DONE COMPLETELY
    Frontend::reset(inst, entry);
    for (auto &entry : btb) {
        entry.valid = false;
    }
}
