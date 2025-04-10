#include "cache-exp.h"
#include "processor.h"
#include "runner.h"

unsigned MeasureCacheSize([[maybe_unused]] ProcessorAbstract *p) {

    // TODO: Measure the size of the cache in the given processor
    // The size will be ranged from 512 bytes to 4096 bytes(or 4 KB), and must be a power of 2
    // Return the accurate value

    return 1024;
}
