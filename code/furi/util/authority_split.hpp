// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once
#include "strutil.hpp"

namespace furi::util
{

struct authority_split
{
    std::string_view userinfo;
    std::string_view host;
    std::string_view port;
};

struct userinfo_split
{
    std::string_view username;
    std::string_view password;
};

authority_split split_authority(std::string_view a) noexcept
{
    authority_split ret;

    auto pos = a.find_first_of('@');
    if (pos != std::string_view::npos)
    {
        ret.userinfo = a.substr(0, pos);
        a = a.substr(pos + 1); // slice userinfo off
    }

    if (a.empty()) return {}; // invalid uri

    // check for ipv6
    if (a[0] == '[')
    {
        auto pos = a.find_first_of(']');
        if (pos == std::string_view::npos) return {}; // invalid uri
        ++pos; // include the ']'
        ret.host = a.substr(0, pos);
        a.remove_prefix(pos);
        if (a.empty()) return ret;
        if (a[0] != ':') return {}; // invalid uri
        ret.port = a.substr(1);
        return ret;
    }

    // no ipv6
    // continue looking for port

    // premptively set host
    // will update if needed
    ret.host = a;

    for (auto p = a.begin(); p != a.end(); ++p)
    {
        if (*p == ':')
        {
            // found port
            ret.host = strutil::make_string_view(a.begin(), p);
            ret.port = strutil::make_string_view(p + 1, a.end());
            break;
        }
    }

    return ret;
}

userinfo_split split_userinfo(std::string_view ui) noexcept
{
    userinfo_split ret = {ui, {}}; // preemptively set user to entire string
    auto pos = ui.find_first_of(':');
    if (pos == std::string_view::npos) return ret;

    ret.username = ui.substr(0, pos);
    ret.password = ui.substr(pos + 1);
    return ret;
}

// individual getters

inline std::string_view get_userinfo_from_authority(std::string_view a) noexcept
{
    auto pos = a.find_first_of('@');
    if (pos == std::string_view::npos) return {};
    return a.substr(0, pos);
}

inline std::string_view get_username_from_userinfo(std::string_view ui) noexcept
{
    auto pos = ui.find_first_of(':');
    return ui.substr(0, pos);
}

inline std::string_view get_password_from_userinfo(std::string_view ui) noexcept
{
    auto pos = ui.find_first_of(':');
    if (pos == std::string_view::npos) return {};
    return ui.substr(pos + 1);
}

inline std::string_view get_host_from_authority(std::string_view a) noexcept
{
    auto pos = a.find_first_of('@');
    if (pos == std::string_view::npos) pos = 0;
    else ++pos; // cut the @ symbol as well
    a.remove_prefix(pos);
    pos = a.find_first_of(']');
    if (pos != std::string_view::npos)
    {
        return a.substr(0, pos+1);
    }
    pos = a.find_first_of(':');
    return a.substr(0, pos);
}

inline std::string_view get_port_from_authority(std::string_view a) noexcept
{
    auto upos = a.find_first_of('@');
    if (upos == std::string_view::npos) upos = 0;
    auto v6pos = a.find_first_of(']', upos); // skip ipv6
    if (v6pos == std::string_view::npos) v6pos = upos;
    auto ppos = a.find_first_of(':', v6pos);
    if (ppos == std::string_view::npos) return {};
    return a.substr(ppos + 1);
}

}
