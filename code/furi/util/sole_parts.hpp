// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once
#include "strutil.hpp"

// Functions which get individual components of an uri.
// They are optimized to do that.
// There are other methods for uri parsing if you need multiple components at a time.
namespace furi::util
{

std::string_view get_scheme(std::string_view u)
{
    for (auto& c : u)
    {
        if (c == ':') return strutil::make_string_view(u.data(), &c);
        if (c == '/') return {}; // encounter path separator => no scheme (no point in searching more)
    }
    return {};
}

std::string_view get_authority(std::string_view u)
{
    auto s = get_scheme(u);
    auto sans_scheme = s.empty() ? u : u.substr(s.length() + 1);
    if (!strutil::starts_with(sans_scheme, "//"))
    {
        // no authority
        // (note that we still return an address; get_path() relies on this)
        return std::string_view(sans_scheme.data(), 0);
    }

    sans_scheme.remove_prefix(2);
    for (auto& c : sans_scheme)
    {
        if (c == '/') return strutil::make_string_view(sans_scheme.data(), &c);
    }
    return sans_scheme; // uri has authority and nothing else
}

std::string_view get_userinfo(std::string_view u)
{
    auto a = get_authority(u);
    if (a.empty()) return {};
    auto pos = a.find_first_of('@');
    if (pos == std::string_view::npos) return {};
    return a.substr(0, pos);
}

std::string_view get_host(std::string_view u)
{
    auto a = get_authority(u);
    if (a.empty()) return {};
    auto pos = a.find_first_of('@');
    if (pos == std::string_view::npos) pos = 0;
    else ++pos; // cut the @ symbol as well
    a.remove_prefix(pos);
    pos = a.find_first_of(':');
    return a.substr(0, pos);
}

std::string_view get_port(std::string_view u)
{
    auto a = get_authority(u);
    if (a.empty()) return {};
    auto pos = a.find_first_of('@');
    if (pos == std::string_view::npos) pos = 0;
    pos = a.find_first_of(':', pos);
    if (pos == std::string_view::npos) return {};
    return a.substr(pos + 1);
}

std::string_view get_path(std::string_view u)
{
    auto a = get_authority(u);
    auto post_authority = strutil::make_string_view(a.data() + a.length(), u.data() + u.length());
    for (auto& c : post_authority)
    {
        if (c == '?' || c == '#') return strutil::make_string_view(post_authority.data(), &c);
    }
    return post_authority;
}

std::string_view get_query(std::string_view u)
{
    auto pos = u.find_first_of('?');
    if (pos == std::string_view::npos) return {}; // no query
    auto post_path = u.substr(pos + 1);
    return post_path.substr(0, post_path.find_first_of('#'));
}

// Note that the fragment is returned with the hash prefix.
// Thus one can differentiate between empty but existing fragment or no fragment at all
std::string_view get_fragment(std::string_view u)
{
    auto pos = u.find_last_of('#');
    if (pos == std::string_view::npos) return {};
    return u.substr(pos);
}

}
