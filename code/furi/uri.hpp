// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once
#include "uri_view.hpp"

#include <string>

namespace furi
{

class uri
{
    std::string m_str;
public:
    uri() noexcept = default;
    explicit uri(const char* cstr) : m_str(cstr) {}
    explicit uri(std::string_view sv) : m_str(sv) {}
    explicit uri(const std::string& str) : m_str(str) {}
    explicit uri(std::string&& str) : m_str(std::move(str)) {}
    explicit uri(uri_view uv) : m_str(uv.sv()) {}

    bool empty() const noexcept { return m_str.empty(); }
    std::string_view sv() const noexcept { return m_str; }
    const std::string& str() const noexcept { return m_str; }
    const char* c_str() const noexcept { return m_str.c_str(); }
    size_t length() const noexcept { return m_str.length(); }

    // Intentionally not explicit.
    // This conversion is almost free. We want it to happen implicitly
    operator uri_view() const noexcept { return uri_view(sv()); }

    std::string_view scheme() const noexcept { return util::get_scheme(m_str); }
    std::string_view authority() const noexcept { return util::get_authority(m_str); }
    std::string_view userinfo() const noexcept { return util::get_userinfo(m_str); }
    std::string_view host() const noexcept { return util::get_host(m_str); }
    std::string_view port() const noexcept { return util::get_port(m_str); }
    std::string_view path() const noexcept { return util::get_path(m_str); }
    std::string_view query() const noexcept { return util::get_query(m_str); }
    std::string_view fragment() const noexcept { return util::get_fragment(m_str); }
};

}
