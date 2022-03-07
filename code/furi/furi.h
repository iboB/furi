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

#if defined(__cplusplus)
#define FURI_EMPTY_VAL {}
extern "C" {
#else
#define FURI_EMPTY_VAL {0}
#endif

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
    if (!str) return (furi_string_view)FURI_EMPTY_VAL;
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
    // avoid memcmp with null
    if (a.begin == b.begin) return 0;
    size_t alen = furi_string_view_length(a);
    if (!b.begin) return !!alen;
    size_t blen = furi_string_view_length(b);
    if (!a.begin) return -!!blen;

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
    if (svlen == 0 && plen == 0) return true; // avoid memcmp with null
    return memcmp(sv.begin, prefix, plen) == 0;
}

inline const char* furi_string_view_find_first(furi_string_view sv, char q)
{
    size_t len = furi_string_view_length(sv);
    if (!len) return NULL; // avoid memchr with null
    return (const char*)memchr(sv.begin, q, len);
}

inline const char* furi_string_view_find_last(furi_string_view sv, char q)
{
    size_t len = furi_string_view_length(sv);

#if defined(_MSC_VER)
    while (len--)
    {
        if (sv.begin[len] == q) return sv.begin + len;
    }
    return NULL;
#else
    if (!len) return NULL; // avoid memchr with null
    return (const char*)memrchr(sv.begin, q, len);
#endif
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
    furi_uri_split ret = FURI_EMPTY_VAL;

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
                // authority found
                u.begin += 2; // slice prefix off
                const char* f = furi_string_view_find_first(u, '/'); // skip till end of authority
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
            break;
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

    // if p is not u.end here this means we found a query and broke out of the loop above
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

// indivitual getters

inline furi_string_view get_scheme_from_uri(furi_string_view u)
{
    for (const char* p = u.begin; p != u.end; ++p)
    {
        if (*p == ':') return furi_make_string_view(u.begin, p);
        if (*p == '/') return (furi_string_view)FURI_EMPTY_VAL; // encounter path separator => no scheme (no point in searching more)
    }
    return (furi_string_view)FURI_EMPTY_VAL;
}

inline furi_string_view get_authority_from_uri(furi_string_view u)
{
    furi_string_view s = get_scheme_from_uri(u);
    if (!furi_string_view_is_null(s))
    {
        u.begin += furi_string_view_length(s) + 1; // slice off scheme
    }

    if (!furi_string_view_starts_with(u, "//"))
    {
        return (furi_string_view)FURI_EMPTY_VAL;
    }

    u.begin += 2; // slice off prefix
    for (const char* p = u.begin; p != u.end; ++p)
    {
        if (*p == '/') return furi_make_string_view(u.begin, p);
    }
    return u; // uri has authority and nothing else
}

inline furi_string_view get_path_from_uri(furi_string_view u)
{
    furi_string_view s = get_scheme_from_uri(u);
    furi_string_view a = get_authority_from_uri(u);

    if (s.begin != a.begin)
    {
        // the only way to have scheme and authority be equal is if both are null
        // in such case we don't touch u
        // otherwise we slice off what we have
        // first try with authority and if not slice off scheme AND colon
        u.begin = a.begin ? a.end : s.end + 1;
    }

    for (const char* p = u.begin; p != u.end; ++p)
    {
        if (*p == '?' || *p == '#') return furi_make_string_view(u.begin, p);
    }

    return u;
}

inline furi_string_view get_query_from_uri(furi_string_view u)
{
    const char* p = furi_string_view_find_first(u, '?');
    if (!p) return (furi_string_view)FURI_EMPTY_VAL; // no query
    u.begin = p + 1;
    p = furi_string_view_find_first(u, '#');
    if (p) u.end = p;
    return u;
}

inline furi_string_view get_fragment_from_uri(furi_string_view u)
{
    const char* p = furi_string_view_find_last(u, '#');
    if (!p) return (furi_string_view)FURI_EMPTY_VAL;
    u.begin = p + 1;
    return u;
}

#if defined(__cplusplus)
} // extern "C"
#endif
