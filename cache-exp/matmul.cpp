#include <vector>

#include "cache-exp.h"
#include "processor.h"
#include "runner.h"
#include "with_cache.h"

#define eps 1e-6

bool MeasureMatmulWithCache(ProcessorWithCache *p) {
    // NOTE: DO NOT MODIFY THIS TEST FILE!!

    [[maybe_unused]] unsigned testTime[2];
    unsigned long totalMemoryTime[2];
    unsigned long totalCacheHitTime[2];

    testTime[0] = executeWithCache(p,
                                   totalMemoryTime[0],
                                   totalCacheHitTime[0],
                                   "./test/sample_cache_matmul",
                                   0);

    std::vector<unsigned> answer;

    answer.reserve(256);
    for (int i = 0; i < 256; i++) {
        answer.push_back(p->readMem(0x80400800 + 4 * i));
    }

    testTime[1] = executeWithCache(p,
                                   totalMemoryTime[1],
                                   totalCacheHitTime[1],
                                   "./test/baseline_matmul",
                                   0);

    for (int i = 0; i < 256; i++) {
        unsigned result = p->readMem(0x80400800 + 4 * i);
        if (result != answer[i]) {
            Logger::Error("Incorrect matrix multiplication optimization.");
            return false;
        }
    }

    double beforeOpt = 1.0 * totalCacheHitTime[1] / totalMemoryTime[1];
    double afterOpt = 1.0 * totalCacheHitTime[0] / totalMemoryTime[0];

    Logger::Warn(
        "Before optimization, cache hit rate = %.3lf, finished in %u cycles",
        beforeOpt,
        testTime[1]);
    Logger::Warn(
        "After optimization, cache hit rate = %.3lf, finished in %u cycles",
        afterOpt,
        testTime[0]);

    return afterOpt > 0.91 - eps && testTime[0] < 1.2 * testTime[1];
}
