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

authority_split split_authority(std::string_view a)
{
    authority_split ret;
    auto p = a.begin();

    for (; p != a.end(); ++p)
    {
        if (*p == '@')
        {
            // found user info
            ret.userinfo = strutil::make_string_view(a.begin(), p);
            a = strutil::make_string_view(p + 1, a.end()); // slice userinfo off
            p = a.begin(); // somewhat redundant but we appease MSSTL here (no mixing of iterators)
            break;
        }
    }

    if (a.empty()) return {}; // invalid uri

    // check for ipv6
    if (a[0] == '[')
    {
        auto pos = a.find_first_of(']');
        if (pos == std::string_view::npos) return {}; // invalid uri
        ret.host = a.substr(1, pos - 1);
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

    for (; p != a.end(); ++p)
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

// individual getters

inline std::string_view get_userinfo_from_authority(std::string_view a)
{
    auto pos = a.find_first_of('@');
    if (pos == std::string_view::npos) return {};
    return a.substr(0, pos);
}

inline std::string_view get_host_from_authority(std::string_view a)
{
    auto pos = a.find_first_of('@');
    if (pos == std::string_view::npos) pos = 0;
    else ++pos; // cut the @ symbol as well
    a.remove_prefix(pos);
    pos = a.find_first_of(']');
    if (pos != std::string_view::npos)
    {
        // ipv6 encountered
        a.remove_prefix(1); // remove opening '['
        if (a.empty()) return {}; // invalid uri
        return a.substr(0, pos - 1);
    }
    pos = a.find_first_of(':');
    return a.substr(0, pos);
}

inline std::string_view get_port_from_authority(std::string_view a)
{
    auto pos = a.find_first_of('@');
    if (pos == std::string_view::npos) pos = 0;
    pos = a.find_first_of(']', pos); // skip ipv6
    if (pos == std::string_view::npos) pos = 0;
    pos = a.find_first_of(':', pos);
    if (pos == std::string_view::npos) return {};
    return a.substr(pos + 1);
}

}
