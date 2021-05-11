#pragma once
#include <array>
#include <string>
#include <string_view>
#include <type_traits>

#include "../SharedDefs.h"
#include "../utility/Span.h"
#include "../jka/JKAConstants.h"
#include "FragmentBuffer.h"

namespace JKA::Protocol {
    class State {
    public:
        State() noexcept = default;
        explicit State(int32_t challenge_) noexcept : challenge(challenge_) {}
        State(const State & other) = default;
        State(State && other) noexcept : State()
        {
            swap(*this, other);
        }

        State & operator=(State other) noexcept
        {
            swap(*this, other);
            return *this;
        }

        template<typename StateTA, typename StateTB>
        friend
            std::enable_if_t<
                std::is_same_v<std::remove_reference_t<StateTA>, State> &&
                std::is_same_v<std::remove_reference_t<StateTB>, State>
            >
        swap(StateTA && a, StateTB && b) noexcept
        {
            using std::swap;
            swap(a.challenge, b.challenge);
            swap(a.reliableCommands, b.reliableCommands);
            swap(a.incomingSequence, b.incomingSequence);
            swap(a.fragmentBuffer, b.fragmentBuffer);
        }

        void reset(int32_t newChallenge) noexcept
        {
            swap(*this, State(newChallenge));
        }

        int32_t getChallenge() const noexcept
        {
            return challenge;
        }

        std::string & reliableCommand(size_t idx) &
        {
            return reliableCommands[reliableCommandsIdx(idx)];
        }

        const std::string & reliableCommand(size_t idx) const &
        {
            return reliableCommands[reliableCommandsIdx(idx)];
        }

        std::optional<FragmentBuffer::FragmentType>
        processFragment(Utility::Span<const ByteType> fragment,
                        int32_t thisFragmentStart,
                        int32_t thisFragmentSequence)
        {
            return fragmentBuffer.processFragment(std::move(fragment),
                                                  thisFragmentStart,
                                                  thisFragmentSequence);
        }

        constexpr int32_t getIncomingSequence() const noexcept
        {
            return incomingSequence;
        }

        void updateIncomingSequence(int32_t newSequence) noexcept
        {
            incomingSequence = newSequence;
        }

    private:
        size_t reliableCommandsIdx(size_t rawIdx) const noexcept
        {
            return rawIdx % reliableCommands.size();
        }

        int32_t challenge{};
        std::array<std::string, MAX_RELIABLE_COMMANDS> reliableCommands{};
        int32_t incomingSequence{};
        FragmentBuffer fragmentBuffer;
    };
}