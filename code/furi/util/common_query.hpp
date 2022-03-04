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

inline constexpr char common_query_div = '&';
inline constexpr char common_query_kv_sep = '=';

// Visit query items for queries in the form of k1=v1&k2=v2...
// Will invoke f for each pair of key value
// f must be invokable like void f(std::string_view key, std::string_view value)
template <typename F>
void visit_common_query_items(std::string_view q, F f)
{
    std::string_view k, v;
    auto cur = &k;

    auto p = q.begin();

    auto update_iteration = [&]() {
        *cur = strutil::make_string_view(q.begin(), p);
        q = strutil::make_string_view(p + 1, q.end()); // slice off
        p = q.begin(); // redundant but we appease MSSTL here (no mixing of iterators)
    };

    while (p != q.end())
    {
        if (*p == common_query_kv_sep)
        {
            update_iteration();
            cur = &v;
        }
        else if (*p == common_query_div)
        {
            update_iteration();
            f(k, v);
            cur = &k;
        }
        else
        {
            ++p;
        }
    }

    // fill-in remainder
    *cur = q;
    f(k, v);
}

}
