// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once
#include "util/sole_parts.hpp"

namespace furi
{

class uri_view
{
    std::string_view m_str;
public:
    uri_view() = default;
    explicit uri_view(std::string_view str) : m_str(str) {}

    bool empty() const { return m_str.empty(); }
    std::string_view sv() const { return m_str; }

    std::string_view scheme() const { return util::get_scheme(m_str); }
    std::string_view authority() const { return util::get_authority(m_str); }
    std::string_view userinfo() const { return util::get_userinfo(m_str); }
    std::string_view host() const { return util::get_host(m_str); }
    std::string_view port() const { return util::get_port(m_str); }
    std::string_view path() const { return util::get_path(m_str); }
    std::string_view query() const { return util::get_query(m_str); }
    std::string_view fragment() const { return util::get_fragment(m_str); }
};

}
