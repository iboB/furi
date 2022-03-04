// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once
#include "strutil.hpp"
#include <utility>
#include <cassert>

namespace furi::util
{

inline constexpr char common_query_item_separator = '&';
inline constexpr char common_query_keyval_separator = '=';

class common_query_iterator
{
    const char* begin;
    const char* p;
    const char* range_end;
    const char* kv_sep_pos = nullptr;

    common_query_iterator() noexcept = default;
public:
    static common_query_iterator make_begin(std::string_view q) noexcept
    {
        common_query_iterator r;
        if (q.empty()) return make_end(q);
        r.begin = q.data();
        r.p = q.data();
        r.range_end = q.data() + q.size();
        --r.p; // hacky redirect for lack of initial item separator
        r.next();
        return r;
    }

    static common_query_iterator make_end(std::string_view q) noexcept
    {
        common_query_iterator r;
        r.begin = r.p = r.range_end = q.data() + q.size();
        return r;
    }

    void next() noexcept
    {
        kv_sep_pos = nullptr;
        begin = p;
        ++p; // overflow to skip last separator

        for (; p < range_end; ++p)
        {
            if (*p == common_query_keyval_separator) kv_sep_pos = p;
            if (*p == common_query_item_separator) break;
        }
    }

    void operator++() noexcept { next(); }

    explicit operator bool() const noexcept { return begin != range_end; }

    using value_type = std::pair<std::string_view, std::string_view>;

    value_type operator*() const noexcept
    {
        assert(p <= range_end); // out-of bounds check

        auto kend = kv_sep_pos, vbegin = kv_sep_pos;
        if (!kv_sep_pos)
        {
            kend = p;
            vbegin = p;
        }
        else
        {
            vbegin += 1;
        }

        return std::make_pair(
            strutil::make_string_view(begin + 1, kend),
            strutil::make_string_view(vbegin, p)
        );
    }

    bool operator==(const common_query_iterator& other) const noexcept { return begin == other.begin; }
    bool operator!=(const common_query_iterator& other) const noexcept { return begin != other.begin; }
};

}
