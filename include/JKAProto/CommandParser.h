#pragma once

#include <string>
#include <string_view>
#include <sstream>
#include <vector>

namespace JKA::CommandParser
{
    using TypeType = uint32_t;

    constexpr TypeType bitFlag(TypeType index)
    {
        return 1ull << index;
    }

    enum class ArgType : TypeType {
        Str = bitFlag(0),
        Int = bitFlag(1),
        Float = bitFlag(2),
        Bool = bitFlag(3),
        BoolExt = bitFlag(4),
    };

    // Note: type conversions here is messy
    // and follows the std::from_chars()/std::strtof() rules;
    // In general it should be safe to assume that an argument with
    // isX() == true would be treated as getX() value in original JKA engine as well
    class Argument {
    public:
        Argument();
        Argument(std::string_view string);
        Argument(const Argument &) = default;
        Argument(Argument &&) = default;
        Argument & operator=(const Argument &) = default;
        Argument & operator=(Argument &&) = default;
        ~Argument() = default;

        const std::string & getStr() const;
        int64_t getInt64() const;
        int32_t getInt32() const;
        float getFloat() const;
        bool getBool() const;
        bool getBoolExt() const;

        template<ArgType Type>
        bool is() const noexcept
        {
            return (type & static_cast<TypeType>(Type)) != 0;
        }

        bool is(ArgType targetType) const noexcept
        {
            return (type & static_cast<TypeType>(targetType)) != 0;
        }

        bool isStr() const;
        bool isInt() const;
        bool isFloat() const;

        // Follows the original JKA bool conversion rules:
        // argument is true when and only when it has non-zero int value
        bool isBool() const; 
        // Extended boolean parser: treats true/yes/on and false/no/off (case-insensitive) as proper booleans
        bool isBoolExt() const;

        bool operator==(const Argument & other) const;
        bool operator==(std::string_view str) const;
        bool operator<(const Argument & other) const;

        friend std::ostream & operator<<(std::ostream & stream, const Argument & arg);
    private:
        void setInt(int64_t val);
        void setFloat(float val);
        void setBool(bool val);

        TypeType type = 0;
        std::string data = "";
        std::string dataLower = "";
        int64_t dataInt = 0;
        float dataFloat = 0.0f;
        bool dataBool = false;
        bool dataBoolExt = false;

        void setTypeFlag(ArgType flag);
        void clearTypeFlag(ArgType flag);

        // parseAs*() functions should be called in exactly this order,
        // e.g. parseAsBool() assumes that both AsInt() and AsFloat() were already called
        void parseAsInt();
        void parseAsFloat();
        void parseAsBool();
        void parseAsBoolExt();
    };

    template<typename Iter>
    std::string concatArgs(Iter begin, Iter end, bool escape)
    {
        std::ostringstream ss;
        if (begin != end) {
            ss << begin++->getStr();
            for (auto it = begin; it != end; ++it) {
                auto itStr = it->getStr();
                ss << ' ';
                if (escape && itStr.find(' ') != itStr.npos) {
                    ss << '"' << itStr << '"';
                } else {
                    ss << itStr;
                }
            }
        }
        return ss.str();
    }

    struct Command {
        std::string name = "";
        std::vector<Argument> args{};

        std::string concat(size_t startIdx = 0, size_t endIdx = std::string::npos) const
        {
            auto endIter = (endIdx == std::string::npos) ? (args.end()) : (args.begin() + endIdx);
            return concatArgs(args.begin() + startIdx, endIter, /*escape*/ false);
        }

        // Concats args while placing those with spaces in it in double quotes
        std::string concatEscape(size_t startIdx = 0, size_t endIdx = std::string::npos) const
        {
            auto endIter = (endIdx == std::string::npos) ? (args.end()) : (args.begin() + endIdx);
            return concatArgs(args.begin() + startIdx, endIter, /*escape*/ true);
        }
    };

    Command parseCommand(std::string_view cmd, std::string_view sepChars = " \r\n");
}
