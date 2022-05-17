// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once
#include <string_view>
#include <utility>

#define FURI_CPP_NAMESPACE furi::capi
#include "furi.h"

namespace furi
{
// optional string view
// has an operator bool which makes the difference between "" and nullptr
class opt_string_view
{
    capi::furi_sv m_sv;
public:
    opt_string_view() noexcept
        : m_sv({nullptr, nullptr}) {}
    opt_string_view(const char* cstring) noexcept // intentionally implicit
        : m_sv(capi::furi_make_sv_from_string(cstring)) {}
    opt_string_view(std::string_view str) noexcept // intentionally implicit
        : m_sv(capi::furi_make_sv(str.data(), str.data() + str.size())) {}
    explicit opt_string_view(const capi::furi_sv& sv) : m_sv(sv) {}
    opt_string_view(const char* begin, const char* end) noexcept
        : m_sv(capi::furi_make_sv(begin, end)) {}
    template <typename CI> // contiguous iterator
    opt_string_view(CI begin, CI end) noexcept
        : m_sv(capi::furi_make_sv(&(*begin), &(*end))) {}

    const char* begin() const noexcept { return m_sv.begin; }
    const char* end() const noexcept { return m_sv.end; }
    const char* data() const noexcept { return begin(); }
    size_t length() const noexcept { return capi::furi_sv_length(m_sv); }
    bool empty() const noexcept { return capi::furi_sv_is_empty(m_sv); }
    bool null() const noexcept { return capi::furi_sv_is_null(m_sv); }
    explicit operator bool() const noexcept { return !null(); }
    std::string_view std_sv() const noexcept { return std::string_view(m_sv.begin, length()); }
    operator std::string_view() const noexcept { return std_sv(); } // intentionally implicit
    const capi::furi_sv& c_sv() const { return m_sv; }

    bool operator==(opt_string_view b) const noexcept { return capi::furi_sv_cmp(m_sv, b.m_sv) == 0; }
    bool operator!=(opt_string_view b) const noexcept { return capi::furi_sv_cmp(m_sv, b.m_sv) != 0; }

    // rely on implicit casts for these
    friend bool operator==(const char* a, opt_string_view b) noexcept { return b == a; }
    friend bool operator!=(const char* a, opt_string_view b) noexcept { return b != a; }
};

struct uri_split
{
    opt_string_view scheme;
    opt_string_view authority;
    opt_string_view path;
    opt_string_view query;
    opt_string_view fragment;

    static uri_split from_capi(const capi::furi_uri_split& cs) noexcept
    {
        return {
            opt_string_view(cs.scheme),
            opt_string_view(cs.authority),
            opt_string_view(cs.path),
            opt_string_view(cs.query),
            opt_string_view(cs.fragment),
        };
    }

    static uri_split from_uri(opt_string_view u) noexcept
    {
        return from_capi(capi::furi_split_uri(u.c_sv()));
    }

    static opt_string_view get_scheme_from_uri(opt_string_view u) noexcept
    {
        return opt_string_view(capi::furi_get_scheme_from_uri(u.c_sv()));
    }

    static opt_string_view get_authority_from_uri(opt_string_view u) noexcept
    {
        return opt_string_view(capi::furi_get_authority_from_uri(u.c_sv()));
    }

    static opt_string_view get_path_from_uri(opt_string_view u) noexcept
    {
        return opt_string_view(capi::furi_get_path_from_uri(u.c_sv()));
    }

    static opt_string_view get_query_from_uri(opt_string_view u) noexcept
    {
        return opt_string_view(capi::furi_get_query_from_uri(u.c_sv()));
    }

    static opt_string_view get_fragment_from_uri(opt_string_view u) noexcept
    {
        return opt_string_view(capi::furi_get_fragment_from_uri(u.c_sv()));
    }
};

struct authority_split
{
    opt_string_view userinfo;
    opt_string_view host;
    opt_string_view port;

    static authority_split from_capi(const capi::furi_authority_split& as) noexcept
    {
        return {
            opt_string_view(as.userinfo),
            opt_string_view(as.host),
            opt_string_view(as.port),
        };
    }

    static authority_split from_authority(opt_string_view a) noexcept
    {
        return from_capi(capi::furi_split_authority(a.c_sv()));
    }

    static opt_string_view get_userinfo_from_authority(opt_string_view a) noexcept
    {
        return opt_string_view(capi::furi_get_userinfo_from_authority(a.c_sv()));
    }

    static opt_string_view get_host_from_authority(opt_string_view a) noexcept
    {
        return opt_string_view(capi::furi_get_host_from_authority(a.c_sv()));
    }

    static opt_string_view get_port_from_authority(opt_string_view a) noexcept
    {
        return opt_string_view(capi::furi_get_port_from_authority(a.c_sv()));
    }
};

struct userinfo_split
{
    opt_string_view username;
    opt_string_view password;

    static userinfo_split from_capi(const capi::furi_userinfo_split& uis) noexcept
    {
        return {
            opt_string_view(uis.username),
            opt_string_view(uis.password),
        };
    }

    static userinfo_split from_userinfo(opt_string_view ui) noexcept
    {
        return from_capi(capi::furi_split_userinfo(ui.c_sv()));
    }

    static opt_string_view get_username_from_userinfo(opt_string_view ui) noexcept
    {
        return opt_string_view(capi::furi_get_username_from_userinfo(ui.c_sv()));
    }

    static opt_string_view get_password_from_userinfo(opt_string_view ui) noexcept
    {
        return opt_string_view(capi::furi_get_password_from_userinfo(ui.c_sv()));
    }
};

class path_iterator
{
    capi::furi_path_iter m_pi;
public:
    path_iterator() : m_pi({nullptr, nullptr, nullptr}) {}
    explicit path_iterator(const capi::furi_path_iter& pi) : m_pi(pi) {};

    static path_iterator begin_of(opt_string_view path) noexcept
    {
        return path_iterator(capi::furi_make_path_iter_begin(path.c_sv()));
    }

    static path_iterator end_of(opt_string_view path) noexcept
    {
        return path_iterator(capi::furi_make_path_iter_end(path.c_sv()));
    }

    void operator++() noexcept
    {
        capi::furi_path_iter_next(&m_pi);
    }

    opt_string_view operator*() const noexcept
    {
        return opt_string_view(capi::furi_path_iter_get_value(m_pi));
    }

    bool operator==(const path_iterator& other) const noexcept
    {
        return capi::furi_path_iter_equal(m_pi, other.m_pi);
    }

    bool operator!=(const path_iterator& other) const noexcept
    {
        return !capi::furi_path_iter_equal(m_pi, other.m_pi);
    }
};

struct path_view : public opt_string_view
{
public:
    using opt_string_view::opt_string_view;
    using const_iterator = path_iterator;
    const_iterator begin() const noexcept { return const_iterator::begin_of(*this); }
    const_iterator end() const noexcept { return const_iterator::end_of(*this); }
};

using query_item = std::pair<opt_string_view, opt_string_view>;

class query_iterator
{
    capi::furi_query_iter m_qi;
public:
    query_iterator() : m_qi({nullptr, nullptr, nullptr, nullptr}) {}
    explicit query_iterator(const capi::furi_query_iter& pi) : m_qi(pi) {};

    static query_iterator begin_of(opt_string_view path) noexcept
    {
        return query_iterator(capi::furi_make_query_iter_begin(path.c_sv()));
    }

    static query_iterator end_of(opt_string_view path) noexcept
    {
        return query_iterator(capi::furi_make_query_iter_end(path.c_sv()));
    }

    void operator++() noexcept
    {
        capi::furi_query_iter_next(&m_qi);
    }

    query_item operator*() const noexcept
    {
        auto c = furi_query_iter_get_value(m_qi);
        return {
            opt_string_view(c.key),
            opt_string_view(c.value),
        };
    }

    bool operator==(const query_iterator& other) const noexcept
    {
        return capi::furi_query_iter_equal(m_qi, other.m_qi);
    }

    bool operator!=(const query_iterator& other) const noexcept
    {
        return !capi::furi_query_iter_equal(m_qi, other.m_qi);
    }
};

struct query_view : public opt_string_view
{
public:
    using opt_string_view::opt_string_view;
    using const_iterator = query_iterator;
    const_iterator begin() const noexcept { return const_iterator::begin_of(*this); }
    const_iterator end() const noexcept { return const_iterator::end_of(*this); }
};

}
