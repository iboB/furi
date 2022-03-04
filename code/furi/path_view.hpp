// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once
#include "util/path_iterator.hpp"

namespace furi
{

class path_view
{
    std::string_view m_str;
public:
    path_view() noexcept = default;
    explicit path_view(std::string_view str) noexcept : m_str(str) {}

    bool empty() const noexcept { return m_str.empty(); }
    std::string_view sv() const noexcept { return m_str; }
    const char* data() const noexcept { return m_str.data(); }
    size_t length() const noexcept { return m_str.length(); }

    using const_iterator = util::path_iterator;
    const_iterator begin() const { return const_iterator::make_begin(m_str); }
    const_iterator end() const { return const_iterator::make_end(m_str); }
};

}
