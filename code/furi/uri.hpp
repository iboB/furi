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

    const std::string& str() const noexcept { return m_str; }
    std::string& str() noexcept { return m_str; }

    std::string_view sv() const noexcept { return m_str; }
    const char* c_str() const noexcept { return m_str.c_str(); }
    size_t length() const noexcept { return m_str.length(); }
    uri_view uv() const noexcept { return uri_view(sv()); }

    // Intentionally not explicit.
    // This conversion is almost free. We want it to happen implicitly
    operator uri_view() const noexcept { return uv(); }

    std::string_view scheme() const noexcept { return uv().scheme(); }
    std::string_view authority() const noexcept { return uv().authority(); }
    std::string_view userinfo() const noexcept { return uv().userinfo(); }
    std::string_view host() const noexcept { return uv().host(); }
    std::string_view port() const noexcept { return uv().port(); }
    std::string_view path() const noexcept { return uv().path(); }
    std::string_view query() const noexcept { return uv().query(); }
    std::string_view fragment() const noexcept { return uv().fragment(); }
};

}
