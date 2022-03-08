// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once
#include <cstring>
#include <cassert>
#include <string_view>

namespace furi
{
namespace capi
{
// hide the c api in a namespace
#include "cfuri.inl"
}

// optional string view
// has an operator bool which makes the difference between "" and nullptr
class opt_string_view
{
    capi::furi_sv m_sv;
public:
    opt_string_view() noexcept
        : m_sv({nullptr, nullptr}) {}
    opt_string_view(const char* cstring) noexcept // intentionally implicit
        : m_sv(capi::furi_make_sv_from_string(cstring)) {}
    opt_string_view(std::string_view str) noexcept // intentionally implicit
        : m_sv(capi::furi_make_sv(str.data(), str.data() + str.size())) {}
    opt_string_view(const char* begin, const char* end) noexcept
        : m_sv(capi::furi_make_sv(begin, end)) {}
    template <typename CI> // contiguous iterator
    opt_string_view(CI begin, CI end) noexcept
        : m_sv(capi::furi_make_sv(&(*begin), &(*end))) {}

    const char* begin() const noexcept { return m_sv.begin; }
    const char* end() const noexcept { return m_sv.end; }
    const char* data() const noexcept { return begin(); }
    size_t length() const noexcept { return capi::furi_sv_length(m_sv); }
    bool empty() const noexcept { return capi::furi_sv_is_empty(m_sv); }
    bool null() const noexcept { return capi::furi_sv_is_null(m_sv); }
    explicit operator bool() const noexcept { return !null(); }
    std::string_view std_sv() const noexcept { return std::string_view(m_sv.begin, length()); }
    operator std::string_view() const noexcept { return std_sv(); } // intentionally implicit
    const capi::furi_sv& c_sv() const { return m_sv; }

    bool operator==(opt_string_view b) const noexcept { return capi::furi_sv_cmp(m_sv, b.m_sv) == 0; }
    bool operator!=(opt_string_view b) const noexcept { return capi::furi_sv_cmp(m_sv, b.m_sv) != 0; }

    // rely on implicit casts for these
    friend bool operator==(const char* a, opt_string_view b) noexcept { return b == a; }
    friend bool operator!=(const char* a, opt_string_view b) noexcept { return b != a; }
};

}
