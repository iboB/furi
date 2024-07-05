// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#pragma once
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#if defined(__cplusplus)
#   define FURI_EMPTY_VAL {}
#   define FURI_EMPTY_T(T) {}
#   if defined FURI_CPP_NAMESPACE
        namespace FURI_CPP_NAMESPACE {
#   else
        extern "C" {
#   endif
#define FURI_INLINE inline
#else
#define FURI_EMPTY_VAL {0}
#define FURI_EMPTY_T(T) (T){0}
#define FURI_INLINE static inline
#endif

///////////////////////////////////////////////////////////////////////////////
// string view
typedef struct furi_sv
{
    const char* begin; // start of string
    const char* end; // one after the last character of the string
} furi_sv;

FURI_INLINE furi_sv furi_make_sv(const char* begin, const char* end)
{
    furi_sv ret = {begin, end};
    return ret;
}

FURI_INLINE furi_sv furi_make_sv_from_string(const char* str)
{
    if (!str) return FURI_EMPTY_T(furi_sv);
    return furi_make_sv(str, str + strlen(str));
}

FURI_INLINE bool furi_sv_is_null(furi_sv sv)
{
    return !sv.begin;
}

FURI_INLINE size_t furi_sv_length(furi_sv sv)
{
    return sv.end - sv.begin;
}

FURI_INLINE bool furi_sv_is_empty(furi_sv sv)
{
    return sv.begin == sv.end;
}

FURI_INLINE int furi_sv_cmp(furi_sv a, furi_sv b)
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


FURI_INLINE bool furi_sv_starts_with(furi_sv sv, const char* prefix)
{
    size_t svlen = furi_sv_length(sv);
    size_t plen = strlen(prefix);
    if (plen > svlen) return false; // prefix longer than string
    if (svlen == 0 && plen == 0) return true; // avoid memcmp with null
    return memcmp(sv.begin, prefix, plen) == 0;
}

FURI_INLINE const char* furi_sv_find_first(furi_sv sv, char q)
{
    size_t len = furi_sv_length(sv);
    if (!len) return NULL; // avoid memchr with null
    return (const char*)memchr(sv.begin, q, len);
}

FURI_INLINE const char* furi_sv_find_last(furi_sv sv, char q)
{
    size_t len = furi_sv_length(sv);
    while (len--)
    {
        if (sv.begin[len] == q) return sv.begin + len;
    }
    return NULL;

    // future:
    // if (!len) return NULL; // avoid memchr with null
    // return (const char*)memrchr(sv.begin, q, len);
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


FURI_INLINE furi_uri_split furi_split_uri(furi_sv u)
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
                    // nothing more than authority
                    ret.authority = u;
                    return ret;
                }

                ret.authority = furi_make_sv(u.begin, f);
                u.begin = f; // slice off authority
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

    // here p is somewhere in u, where u has scheme and authority removed (if present)

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

// individual getters

FURI_INLINE furi_sv furi_get_scheme_from_uri(furi_sv u)
{
    for (const char* p = u.begin; p != u.end; ++p)
    {
        if (*p == ':') return furi_make_sv(u.begin, p);
        if (*p == '/') return FURI_EMPTY_T(furi_sv); // encounter path separator => no scheme (no point in searching more)
    }
    return FURI_EMPTY_T(furi_sv);
}

FURI_INLINE furi_sv furi_get_authority_from_uri(furi_sv u)
{
    furi_sv s = furi_get_scheme_from_uri(u);
    if (!furi_sv_is_null(s))
    {
        u.begin = s.end + 1; // slice off scheme
    }

    if (!furi_sv_starts_with(u, "//"))
    {
        return FURI_EMPTY_T(furi_sv);
    }

    u.begin += 2; // slice off prefix
    for (const char* p = u.begin; p != u.end; ++p)
    {
        if (*p == '/') return furi_make_sv(u.begin, p);
    }
    return u; // uri has authority and nothing else
}

FURI_INLINE furi_sv furi_get_path_from_uri(furi_sv u)
{
    furi_sv s = furi_get_scheme_from_uri(u);
    furi_sv a = furi_get_authority_from_uri(u);

    if (s.begin != a.begin)
    {
        // the only way to have scheme and authority be equal is if both are null
        // in such case we don't touch u
        // otherwise we slice off what we have
        // first try with authority and if not slice off scheme AND colon
        if (a.end == u.end) return FURI_EMPTY_T(furi_sv); // nothing more than authority

        if (a.begin) {
            // we have authority
            u.begin = a.end;
        }
        else {
            // only scheme
            u.begin = s.end + 1; // slice off colon
        }
    }

    for (const char* p = u.begin; p != u.end; ++p)
    {
        // break if we encounter query or fragment
        if (*p == '?' || *p == '#') return furi_make_sv(u.begin, p);
    }

    return u;
}

FURI_INLINE furi_sv furi_get_query_from_uri(furi_sv u)
{
    const char* p = furi_sv_find_first(u, '?');
    if (!p) return FURI_EMPTY_T(furi_sv); // no query
    u.begin = p + 1;
    p = furi_sv_find_first(u, '#');
    if (p) u.end = p;
    return u;
}

FURI_INLINE furi_sv furi_get_fragment_from_uri(furi_sv u)
{
    const char* p = furi_sv_find_last(u, '#');
    if (!p) return FURI_EMPTY_T(furi_sv);
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

FURI_INLINE furi_authority_split furi_split_authority(furi_sv a)
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
        if (!bf) return FURI_EMPTY_T(furi_authority_split); // invalid uri
        ++bf; // include the ']'
        ret.host.end = bf;
        a.begin = bf; // slice off
        if (furi_sv_is_empty(a)) return ret; // valid. no port
        if (a.begin[0] != ':') return FURI_EMPTY_T(furi_authority_split); // invalid uri
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

FURI_INLINE furi_sv furi_get_userinfo_from_authority(furi_sv a)
{
    const char* p = furi_sv_find_first(a, '@');
    if (!p) return FURI_EMPTY_T(furi_sv);
    a.end = p;
    return a;
}

FURI_INLINE furi_sv furi_get_host_from_authority(furi_sv a)
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

FURI_INLINE furi_sv furi_get_port_from_authority(furi_sv a)
{
    const char* p = furi_sv_find_first(a, '@'); // skip userinfo if any
    if (p) a.begin = p + 1;
    p = furi_sv_find_first(a, ']'); // skip ipv6 if any
    if (p) a.begin = p + 1;
    p = furi_sv_find_first(a, ':'); // find port separator
    if (!p) return FURI_EMPTY_T(furi_sv);
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


FURI_INLINE furi_userinfo_split furi_split_userinfo(furi_sv ui)
{
    furi_userinfo_split ret = {ui, FURI_EMPTY_VAL}; // preemptively set user to entire string
    const char* p = furi_sv_find_first(ui, ':');
    if (!p) return ret;

    ret.username.end = p;
    ret.password = furi_make_sv(p + 1, ui.end);
    return ret;
}

FURI_INLINE furi_sv furi_get_username_from_userinfo(furi_sv ui)
{
    const char* p = furi_sv_find_first(ui, ':');
    if (!p) return ui;
    ui.end = p;
    return ui;
}

FURI_INLINE furi_sv furi_get_password_from_userinfo(furi_sv ui)
{
    const char* p = furi_sv_find_first(ui, ':');
    if (!p) return FURI_EMPTY_T(furi_sv);
    ui.begin = p + 1;
    return ui;
}

///////////////////////////////////////////////////////////////////////////////
// path iterator
typedef struct furi_path_iter
{
    const char* begin;
    const char* p;
    const char* range_end;
} furi_path_iter;

FURI_INLINE furi_path_iter furi_make_path_iter_end(const furi_sv path)
{
    furi_path_iter ret = { path.end, path.end, path.end };
    return ret;
}

FURI_INLINE void furi_path_iter_next(furi_path_iter* pi)
{
    pi->begin = pi->p;
    ++pi->p; // overflow to skip last separator

    for (; pi->p < pi->range_end; ++pi->p)
    {
        if (*pi->p == '/') break;
    }
}

FURI_INLINE furi_path_iter furi_make_path_iter_begin(const furi_sv path)
{
    if (furi_sv_is_null(path)) return furi_make_path_iter_end(path);
    furi_path_iter r = { path.begin, path.begin, path.end };
    if (path.begin[0] != '/') --r.p; // hacky redirect for paths which don't begin with /
    furi_path_iter_next(&r);
    return r;
}

FURI_INLINE bool furi_path_iter_is_done(const furi_path_iter pi)
{
    return pi.begin == pi.range_end;
}

FURI_INLINE furi_sv furi_path_iter_get_value(const furi_path_iter pi)
{
    assert(pi.p <= pi.range_end); // out-of bounds check

    // at this point begin points to one before the actual item
    return furi_make_sv(pi.begin + 1, pi.p);
}

FURI_INLINE bool furi_path_iter_equal(const furi_path_iter a, const furi_path_iter b)
{
    return a.begin == b.begin;
}

///////////////////////////////////////////////////////////////////////////////
// query iterator
#define FURI_QUERY_KV_SEP '='
#define FURI_QUERY_ITEM_SEP '&'

typedef struct furi_query_iter
{
    const char* begin;
    const char* p;
    const char* range_end;
    const char* kv_sep_pos;
} furi_query_iter;

FURI_INLINE furi_query_iter furi_make_query_iter_end(const furi_sv query)
{
    furi_query_iter ret = {query.end, query.end, query.end, NULL};
    return ret;
}

FURI_INLINE void furi_query_iter_next(furi_query_iter* qi)
{
    qi->kv_sep_pos = NULL;
    qi->begin = qi->p;
    ++qi->p; // overflow to skip last separator

    for (; qi->p < qi->range_end; ++qi->p)
    {
        if (*qi->p == FURI_QUERY_KV_SEP) qi->kv_sep_pos = qi->p;
        if (*qi->p == FURI_QUERY_ITEM_SEP) break;
    }
}

FURI_INLINE furi_query_iter furi_make_query_iter_begin(const furi_sv query)
{
    if (furi_sv_is_empty(query)) return furi_make_query_iter_end(query);
    furi_query_iter r = {query.begin, query.begin, query.end, NULL};
    --r.p; // hacky redirect for lack of initial item separator
    furi_query_iter_next(&r);
    return r;
}

FURI_INLINE bool furi_query_iter_is_done(const furi_query_iter qi)
{
    return qi.begin == qi.range_end;
}

typedef struct furi_query_iter_value
{
    furi_sv key;
    furi_sv value;
} furi_query_iter_value;

FURI_INLINE furi_query_iter_value furi_query_iter_get_value(const furi_query_iter qi)
{
    assert(qi.p <= qi.range_end); // out-of bounds check

    furi_query_iter_value ret = FURI_EMPTY_VAL;
    if (qi.kv_sep_pos)
    {
        ret.key = furi_make_sv(qi.begin + 1, qi.kv_sep_pos);
        ret.value = furi_make_sv(qi.kv_sep_pos + 1, qi.p);
    }
    else
    {
        // no key value separator, just a key
        ret.key = furi_make_sv(qi.begin + 1, qi.p);
    }

    return ret;
}

FURI_INLINE bool furi_query_iter_equal(const furi_query_iter a, const furi_query_iter b)
{
    return a.begin == b.begin;
}

#if defined(__cplusplus)
// dual purpose closing brace
// if FURI_CPP_NAMESPACE is defined this closes the namespace
// otherwise it closes extern "C"
}
#endif
