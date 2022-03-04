// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once
#include "util/split.hpp"
#include "util/authority_split.hpp"

namespace furi
{

class uri_view
{
    std::string_view m_str;
public:
    uri_view() noexcept = default;
    explicit uri_view(std::string_view str) noexcept : m_str(str) {}

    bool empty() const noexcept { return m_str.empty(); }
    std::string_view sv() const noexcept { return m_str; }
    const char* data() const noexcept { return m_str.data(); }
    size_t length() const noexcept { return m_str.length(); }

    std::string_view scheme() const noexcept { return util::get_scheme_from_uri(m_str); }
    std::string_view authority() const noexcept { return util::get_authority_from_uri(m_str); }
    std::string_view userinfo() const noexcept { return util::get_userinfo_from_authority(util::get_authority_from_uri(m_str)); }
    std::string_view host() const noexcept { return util::get_host_from_authority(util::get_authority_from_uri(m_str)); }
    std::string_view port() const noexcept { return util::get_port_from_authority(util::get_authority_from_uri(m_str)); }
    std::string_view path() const noexcept { return util::get_path_from_uri(m_str); }
    std::string_view query() const noexcept { return util::get_query_from_uri(m_str); }
    std::string_view fragment() const noexcept { return util::get_fragment_from_uri(m_str); }
};

}
