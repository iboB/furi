// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once
#include <string.h>
#include <stdbool.h>

///////////////////////////////////////////////////////////////////////////////
// string view
typedef struct furi_string_view
{
    const char* begin; // start of string
    const char* end; // one after the last character of the string
} furi_string_view;

inline furi_string_view furi_make_string_view(const char* begin, const char* end)
{
    furi_string_view ret = {begin, end};
    return ret;
}

inline furi_string_view furi_string_view_from_string(const char* str)
{
    return furi_make_string_view(str, str + strlen(str));
}

inline bool furi_string_view_is_null(furi_string_view sv)
{
    return !sv.begin;
}

inline size_t furi_string_view_length(furi_string_view sv)
{
    return sv.end - sv.begin;
}

inline int furi_string_view_cmp(furi_string_view a, furi_string_view b)
{
    size_t alen = furi_string_view_length(a);
    size_t blen = furi_string_view_length(b);
    if (alen == blen)
    {
        return memcmp(a.begin, b.begin, alen);
    }
    else if (alen < blen)
    {
        int cmp = memcmp(a.begin, b.begin, alen);
        return cmp == 0 ? -1 : cmp;
    }
    else
    {
        int cmp = memcmp(a.begin, b.begin, blen);
        return cmp == 0 ? 1 : cmp;
    }
}


inline bool furi_string_view_starts_with(furi_string_view sv, const char* prefix)
{
    size_t svlen = furi_string_view_length(sv);
    size_t plen = strlen(prefix);
    if (plen > svlen) return false; // prefix longer than string
    return memcmp(sv.begin, prefix, plen) == 0;
}

inline const char* furi_string_view_find_first(furi_string_view sv, char q)
{
    return (const char*)memchr(sv.begin, q, furi_string_view_length(sv));
}

///////////////////////////////////////////////////////////////////////////////
// uri split

typedef struct furi_uri_split
{
    furi_string_view scheme;
    furi_string_view authority;
    furi_string_view path;
    furi_string_view query;
    furi_string_view fragment;
} furi_uri_split;

inline furi_uri_split furi_split_uri(furi_string_view u)
{
    furi_uri_split ret = {0};

    const char* p = u.begin;
    for (; p != u.end; ++p)
    {
        if (*p == ':')
        {
            // found scheme
            ret.scheme = furi_make_string_view(u.begin, p);
            u = furi_make_string_view(p + 1, u.end); // slice scheme off

            // since we have a scheme, we can also search for authority
            // note with no scheme, there is no possibility to have authority
            if (furi_string_view_starts_with(u, "//"))
            {
                const char* f;
                // authority found
                u.begin += 2;
                f = furi_string_view_find_first(u, '/'); // skip till end of authority
                if (!f)
                {
                    // nothing more than authroity
                    ret.authority = u;
                    return ret;
                }

                ret.authority = furi_make_string_view(u.begin, f);
                u.begin = f; // slice authority off
            }

            p = u.begin; // redirect p in u
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
    for (; p != u.end; ++p)
    {
        if (*p == '?')
        {
            // found a query
            ret.path = furi_make_string_view(u.begin, p); // update path
            u.begin = p + 1; // slice path off
            ret.query = u; // preemptively set query to rest
        }
        else if (*p == '#')
        {
            // found a fragment
            // at this point this definitely means that there is no query
            ret.path = furi_make_string_view(u.begin, p); // update path
            ret.fragment = furi_make_string_view(p + 1, u.end);
            return ret; // nothing more to search
        }
    }

    // if p is not u.end() here this means we found a query and broke out of the loop above
    // it also means that u is now the query alone and p is still in u
    for (; p != u.end; ++p)
    {
        if (*p == '#')
        {
            ret.query = furi_make_string_view(u.begin, p); // update query
            ret.fragment = furi_make_string_view(p + 1, u.end);
            break;
        }
    }

    return ret;
}