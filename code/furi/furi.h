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
typedef struct furi_sv
{
    const char* begin; // start of string
    const char* end; // one after the last character of the string
} furi_sv;

inline furi_sv furi_make_sv(const char* begin, const char* end)
{
    furi_sv ret = {begin, end};
    return ret;
}

inline furi_sv furi_sv_from_string(const char* str)
{
    if (!str) return (furi_sv)FURI_EMPTY_VAL;
    return furi_make_sv(str, str + strlen(str));
}

inline bool furi_sv_is_null(furi_sv sv)
{
    return !sv.begin;
}

inline size_t furi_sv_length(furi_sv sv)
{
    return sv.end - sv.begin;
}

inline bool furi_sv_is_empty(furi_sv sv)
{
    return sv.begin == sv.end;
}

inline int furi_sv_cmp(furi_sv a, furi_sv b)
{
    // avoid memcmp with null
    if (a.begin == b.begin) return 0;
    size_t alen = furi_sv_length(a);
    if (!b.begin) return !!alen;
    size_t blen = furi_sv_length(b);
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


inline bool furi_sv_starts_with(furi_sv sv, const char* prefix)
{
    size_t svlen = furi_sv_length(sv);
    size_t plen = strlen(prefix);
    if (plen > svlen) return false; // prefix longer than string
    if (svlen == 0 && plen == 0) return true; // avoid memcmp with null
    return memcmp(sv.begin, prefix, plen) == 0;
}

inline const char* furi_sv_find_first(furi_sv sv, char q)
{
    size_t len = furi_sv_length(sv);
    if (!len) return NULL; // avoid memchr with null
    return (const char*)memchr(sv.begin, q, len);
}

inline const char* furi_sv_find_last(furi_sv sv, char q)
{
    size_t len = furi_sv_length(sv);

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
    furi_sv scheme;
    furi_sv authority;
    furi_sv path;
    furi_sv query;
    furi_sv fragment;
} furi_uri_split;

inline furi_uri_split furi_split_uri(furi_sv u)
{
    furi_uri_split ret = FURI_EMPTY_VAL;

    const char* p = u.begin;
    for (; p != u.end; ++p)
    {
        if (*p == ':')
        {
            // found scheme
            ret.scheme = furi_make_sv(u.begin, p);
            u = furi_make_sv(p + 1, u.end); // slice scheme off

            // since we have a scheme, we can also search for authority
            // note with no scheme, there is no possibility to have authority
            if (furi_sv_starts_with(u, "//"))
            {
                // authority found
                u.begin += 2; // slice prefix off
                const char* f = furi_sv_find_first(u, '/'); // skip till end of authority
                if (!f)
                {
                    // nothing more than authroity
                    ret.authority = u;
                    return ret;
                }

                ret.authority = furi_make_sv(u.begin, f);
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
            ret.path = furi_make_sv(u.begin, p); // update path
            u.begin = p + 1; // slice path off
            ret.query = u; // preemptively set query to rest
            break;
        }
        else if (*p == '#')
        {
            // found a fragment
            // at this point this definitely means that there is no query
            ret.path = furi_make_sv(u.begin, p); // update path
            ret.fragment = furi_make_sv(p + 1, u.end);
            return ret; // nothing more to search
        }
    }

    // if p is not u.end here this means we found a query and broke out of the loop above
    // it also means that u is now the query alone and p is still in u
    for (; p != u.end; ++p)
    {
        if (*p == '#')
        {
            ret.query = furi_make_sv(u.begin, p); // update query
            ret.fragment = furi_make_sv(p + 1, u.end);
            break;
        }
    }

    return ret;
}

// indivitual getters

inline furi_sv furi_get_scheme_from_uri(furi_sv u)
{
    for (const char* p = u.begin; p != u.end; ++p)
    {
        if (*p == ':') return furi_make_sv(u.begin, p);
        if (*p == '/') return (furi_sv)FURI_EMPTY_VAL; // encounter path separator => no scheme (no point in searching more)
    }
    return (furi_sv)FURI_EMPTY_VAL;
}

inline furi_sv furi_get_authority_from_uri(furi_sv u)
{
    furi_sv s = furi_get_scheme_from_uri(u);
    if (!furi_sv_is_null(s))
    {
        u.begin = s.end + 1; // slice off scheme
    }

    if (!furi_sv_starts_with(u, "//"))
    {
        return (furi_sv)FURI_EMPTY_VAL;
    }

    u.begin += 2; // slice off prefix
    for (const char* p = u.begin; p != u.end; ++p)
    {
        if (*p == '/') return furi_make_sv(u.begin, p);
    }
    return u; // uri has authority and nothing else
}

inline furi_sv furi_get_path_from_uri(furi_sv u)
{
    furi_sv s = furi_get_scheme_from_uri(u);
    furi_sv a = furi_get_authority_from_uri(u);

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
        if (*p == '?' || *p == '#') return furi_make_sv(u.begin, p);
    }

    return u;
}

inline furi_sv furi_get_query_from_uri(furi_sv u)
{
    const char* p = furi_sv_find_first(u, '?');
    if (!p) return (furi_sv)FURI_EMPTY_VAL; // no query
    u.begin = p + 1;
    p = furi_sv_find_first(u, '#');
    if (p) u.end = p;
    return u;
}

inline furi_sv furi_get_fragment_from_uri(furi_sv u)
{
    const char* p = furi_sv_find_last(u, '#');
    if (!p) return (furi_sv)FURI_EMPTY_VAL;
    u.begin = p + 1;
    return u;
}

///////////////////////////////////////////////////////////////////////////////
// authority split

typedef struct furi_authority_split
{
    furi_sv userinfo;
    furi_sv host;
    furi_sv port;
} furi_authority_split;

inline furi_authority_split furi_split_authority(furi_sv a)
{
    furi_authority_split ret = FURI_EMPTY_VAL;

    const char* f = furi_sv_find_first(a, '@');
    if (f)
    {
        ret.userinfo = furi_make_sv(a.begin, f);
        a.begin = f + 1; // slice userinfo off
    }

    // premptively set host
    // will update if needed
    ret.host = a;

    if (furi_sv_is_empty(a)) return ret; // empty host

    // check for ipv6
    if (a.begin[0] == '[')
    {
        const char* bf = furi_sv_find_first(a, ']');
        if (!bf) return (furi_authority_split)FURI_EMPTY_VAL; // invalid uri
        ++bf; // include the ']'
        ret.host.end = bf;
        a.begin = bf; // slice off
        if (furi_sv_is_empty(a)) return ret; // valid. no port
        if (a.begin[0] != ':') return (furi_authority_split)FURI_EMPTY_VAL; // invalid uri
        ret.port = furi_make_sv(a.begin + 1, a.end);
        return ret;
    }

    // no ipv6
    // continue looking for port

    for (const char* p = a.begin; p != a.end; ++p)
    {
        if (*p == ':')
        {
            // found port
            ret.host = furi_make_sv(a.begin, p);
            ret.port = furi_make_sv(p + 1, a.end);
            break;
        }
    }

    return ret;
}

// individual getters

inline furi_sv furi_get_userinfo_from_authority(furi_sv a)
{
    const char* p = furi_sv_find_first(a, '@');
    if (!p) return (furi_sv)FURI_EMPTY_VAL;
    a.end = p;
    return a;
}

inline furi_sv furi_get_host_from_authority(furi_sv a)
{
    const char* p = furi_sv_find_first(a, '@');
    if (p) a.begin = p + 1; // cut the @ symbol as well
    p = furi_sv_find_first(a, ']');; // ipv6 check
    if (p)
    {
        a.end = p + 1;
        return a;
    }
    p = furi_sv_find_first(a, ':');
    if (p) a.end = p;
    return a;
}

inline furi_sv furi_get_port_from_authority(furi_sv a)
{
    const char* p = furi_sv_find_first(a, '@');
    if (p) a.begin = p + 1;
    p = furi_sv_find_first(a, ']');
    if (p) a.begin = p + 1;
    p = furi_sv_find_first(a, ':');
    if (!p) return (furi_sv)FURI_EMPTY_VAL;
    a.begin = p + 1;
    return a;
}

///////////////////////////////////////////////////////////////////////////////
// userinfo split

typedef struct furi_userinfo_split
{
    furi_sv username;
    furi_sv password;
} furi_userinfo_split;

furi_userinfo_split furi_split_userinfo(furi_sv ui)
{
    furi_userinfo_split ret = {ui, FURI_EMPTY_VAL}; // preemptively set user to entire string
    const char* p = furi_sv_find_first(ui, ':');
    if (!p) return ret;

    ret.username.end = p;
    ret.password = furi_make_sv(p + 1, ui.end);
    return ret;
}

inline furi_sv furi_get_username_from_userinfo(furi_sv ui)
{
    const char* p = furi_sv_find_first(ui, ':');
    if (!p) return ui;
    ui.end = p;
    return ui;
}

inline furi_sv furi_get_password_from_userinfo(furi_sv ui)
{
    const char* p = furi_sv_find_first(ui, ':');
    if (!p) return (furi_sv)FURI_EMPTY_VAL;
    ui.begin = p + 1;
    return ui;
}

#if defined(__cplusplus)
} // extern "C"
#endif
