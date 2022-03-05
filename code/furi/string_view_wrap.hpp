// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once
#include <string_view>

namespace furi
{

class string_view_wrap
{
    std::string_view m_str;
public:
    string_view_wrap() noexcept = default;
    explicit string_view_wrap(std::string_view str) noexcept : m_str(str) {}

    std::string_view sv() const noexcept { return m_str; }
    std::string_view& sv() noexcept { return m_str; }

    bool empty() const noexcept { return m_str.empty(); }
    explicit operator bool() const { return empty(); }

    const char* data() const noexcept { return m_str.data(); }
    size_t length() const noexcept { return m_str.length(); }

    bool operator==(const string_view_wrap& other) const noexcept { return m_str == other.m_str; }
    bool operator!=(const string_view_wrap& other) const noexcept { return m_str != other.m_str; }
};

}
