// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once

// some functions copied from
// https://github.com/iboB/itlib/blob/master/include/itlib/strutil.hpp
namespace furi::util
{

template <typename CI> // contiguous iterator
inline constexpr std::string_view make_string_view(CI begin, CI end)
{
    return std::string_view(&(*begin), end - begin);
}

// overload which is likely to come up most often
inline constexpr std::string_view make_string_view(const char* begin, const char* end) noexcept
{
    return std::string_view(begin, end - begin);
}

inline bool starts_with(std::string_view str, std::string_view prefix) noexcept
{
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

inline bool ends_with(std::string_view str, std::string_view suffix) noexcept
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

}
