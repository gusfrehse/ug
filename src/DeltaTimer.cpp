#include "DeltaTimer.h"

#include <chrono>
#include <unordered_map>

std::unordered_map<size_t, float> times;

std::chrono::steady_clock timer;

DeltaTimer::DeltaTimer(size_t hash)
: mStartTime(timer.now()), mHash(hash) {
    if (!times.contains(hash)) {
        times[hash] = 16.6;
    }
}

DeltaTimer::~DeltaTimer() {
    auto end = timer.now();

    times[mHash] = std::chrono::duration_cast<std::chrono::microseconds>(end - mStartTime).count() / 1000.0f;
}

float DeltaTimer::get() {
    return times[mHash];
}
