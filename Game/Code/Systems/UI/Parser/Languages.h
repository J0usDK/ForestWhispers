#pragma once

enum class ELanguage : uint8_t
{
    English,
    Count
};

namespace LocalNames
{
    inline constexpr std::string_view English = "EnglishLocal.json";

    inline constexpr std::string_view Table[] =
    {
        English
    };
}