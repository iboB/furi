// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once
#include "string_view_wrap.hpp"
#include "util/split.hpp"
#include "util/authority_split.hpp"

namespace furi
{

class uri_view : public string_view_wrap
{
public:
    using string_view_wrap::string_view_wrap;

    std::string_view scheme() const noexcept { return util::get_scheme_from_uri(sv()); }
    std::string_view authority() const noexcept { return util::get_authority_from_uri(sv()); }
    std::string_view userinfo() const noexcept { return util::get_userinfo_from_authority(util::get_authority_from_uri(sv())); }
    std::string_view host() const noexcept { return util::get_host_from_authority(util::get_authority_from_uri(sv())); }
    std::string_view port() const noexcept { return util::get_port_from_authority(util::get_authority_from_uri(sv())); }
    std::string_view path() const noexcept { return util::get_path_from_uri(sv()); }
    std::string_view query() const noexcept { return util::get_query_from_uri(sv()); }
    std::string_view fragment() const noexcept { return util::get_fragment_from_uri(sv()); }
};

}
