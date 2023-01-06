#ifndef DeltaTimer_h_INCLUDED
#define DeltaTimer_h_INCLUDED

#include <chrono>

#define DT(name) DeltaTimer name(__LINE__)

class DeltaTimer {
private:
    std::chrono::time_point<std::chrono::steady_clock> mStartTime;
    size_t mHash;

public:
    DeltaTimer(size_t hash);
    DeltaTimer(DeltaTimer&) = delete;
    ~DeltaTimer();

    float get();
};

#endif // DeltaTimer_h_INCLUDED
