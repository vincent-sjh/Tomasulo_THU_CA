#include "cache-exp.h"

ReplaceType GetCacheReplaceType([[maybe_unused]] ProcessorAbstract *p) {
    // The replacement policy will be either LRU or FIFO
    return ReplaceType::RANDOM;
}
