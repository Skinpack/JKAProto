#pragma once
#include <chrono>

namespace JKA {
    using ByteType = char;
    static_assert(sizeof(ByteType) == sizeof(char));

    using Clock = std::chrono::system_clock;
    using TimePoint = Clock::time_point;
}

#if __has_cpp_attribute(likely)
#define JKA_LIKELY [[likely]]
#else
#define JKA_LIKELY
#endif

#if __has_cpp_attribute(unlikely)
#define JKA_UNLIKELY [[unlikely]]
#else
#define JKA_UNLIKELY
#endif

