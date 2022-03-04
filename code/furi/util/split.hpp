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

split make_split(std::string_view u)
{
    split ret;

    auto beg = u.begin();
    auto end = beg;

    // scheme
    while (end != u.end())
    {
        if (*end == ':')
        {
            // found scheme
            ret.scheme = strutil::make_string_view(beg, end);
            ++end;
            beg = end;
            break;
        }

        if (*end == '/')
        {
            // there is no scheme. It was a path all along
            ++end;
            break;
        }
    }

    if (end == u.end())
    {
        // we've reached the end of the string
        // so this is just a path or a scheme and an empty path
        ret.path = strutil::make_string_view(beg, end);
        return ret;
    }


    return ret;
}

}
