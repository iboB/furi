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

struct split
{
    std::string_view scheme;
    std::string_view authority;
    std::string_view path;
    std::string_view query;
    std::string_view fragment;
};

// get all parts at once
inline split split_uri(std::string_view u) noexcept
{
    split ret;

    auto p = u.begin();

    // scheme
    for (; p != u.end(); ++p)
    {
        if (*p == ':')
        {
            // found scheme
            ret.scheme = strutil::make_string_view(u.begin(), p);
            u = strutil::make_string_view(p + 1, u.end()); // slice scheme off

            // since we have a scheme, we can also search for authority
            // note with no scheme, there is no possibility to have authority
            if (strutil::starts_with(u, "//"))
            {
                // authority found
                u.remove_prefix(2);
                auto pos = u.find_first_of('/'); // skip till end of authority
                if (pos == std::string_view::npos)
                {
                    // nothing more than authroity
                    ret.authority = u;
                    return ret;
                }

                ret.authority = u.substr(0, pos);
                u.remove_prefix(pos); // slice authority off
            }

            p = u.begin(); // redirect p in u
            break;
        }

        if (*p == '/')
        {
            // there is no scheme. It was a path all along
            ++p;
            break;
        }
    }

    // here p is somewhere in u, where u has scheme and authroity removed (if present)

    // preemptively set path to entire string
    // will update if needed
    ret.path = u;

    // find end of path
    for (; p != u.end(); ++p)
    {
        if (*p == '?')
        {
            // found a query
            ret.path = strutil::make_string_view(u.begin(), p); // update path
            u = strutil::make_string_view(p + 1, u.end()); // slice path off
            ret.query = u; // preemptively set query to rest
            p = u.begin(); // somewhat redundant but we appease MSSTL here (no mixing of iterators)
            break;
        }

        if (*p == '#')
        {
            // found a fragment
            // at this point this definitely means that there is no query
            ret.path = strutil::make_string_view(u.begin(), p); // update path
            ret.fragment = strutil::make_string_view(p + 1, u.end());
            return ret; // nothing more to search
        }
    }

    // if p is not u.end() here this means we found a query and broke out of the loop above
    // it also means that u is now the query alone and p is still in u
    for (; p != u.end(); ++p)
    {
        if (*p == '#')
        {
            ret.query = strutil::make_string_view(u.begin(), p); // update query
            ret.fragment = strutil::make_string_view(p + 1, u.end());
            break;
        }
    }

    return ret;
}

// indivitual getters

inline std::string_view get_scheme_from_uri(std::string_view u) noexcept
{
    for (auto& c : u)
    {
        if (c == ':') return strutil::make_string_view(u.data(), &c);
        if (c == '/') return {}; // encounter path separator => no scheme (no point in searching more)
    }
    return {};
}

inline std::string_view get_authority_from_uri(std::string_view u) noexcept
{
    auto s = get_scheme_from_uri(u);
    auto sans_scheme = s.empty() ? u : u.substr(s.length() + 1);
    if (!strutil::starts_with(sans_scheme, "//"))
    {
        // no authority
        // note that we still return an address
        // get_path_from_uri() relies on this
        return std::string_view(sans_scheme.data(), 0);
    }

    sans_scheme.remove_prefix(2);
    for (auto& c : sans_scheme)
    {
        if (c == '/') return strutil::make_string_view(sans_scheme.data(), &c);
    }
    return sans_scheme; // uri has authority and nothing else
}

inline std::string_view get_path_from_uri(std::string_view u) noexcept
{
    auto a = get_authority_from_uri(u);
    auto post_authority = strutil::make_string_view(a.data() + a.length(), u.data() + u.length());
    for (auto& c : post_authority)
    {
        if (c == '?' || c == '#') return strutil::make_string_view(post_authority.data(), &c);
    }
    return post_authority;
}

inline std::string_view get_query_from_uri(std::string_view u) noexcept
{
    auto pos = u.find_first_of('?');
    if (pos == std::string_view::npos) return {}; // no query
    auto post_path = u.substr(pos + 1);
    return post_path.substr(0, post_path.find_first_of('#'));
}

inline std::string_view get_fragment_from_uri(std::string_view u) noexcept
{
    auto pos = u.find_last_of('#');
    if (pos == std::string_view::npos) return {};
    return u.substr(pos + 1);
}

}
