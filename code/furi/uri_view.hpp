// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once
#include <string_view>

#include "impl/strutil.hpp"

namespace furi
{

class uri_view
{
    std::string_view m_str;
public:
    uri_view() = default;
    explicit uri_view(std::string_view str) : m_str(str) {}

    std::string_view scheme() const
    {
        for (auto& c : m_str)
        {
            if (c == ':') return util::make_string_view(m_str.data(), &c);
            if (c == '/') return {}; // encounter path separator => no scheme (no point in searching more)
        }
        return {};
    }

    std::string_view authority() const
    {
        auto s = scheme();
        auto sans_scheme = s.empty() ? m_str : m_str.substr(scheme().length() + 1);
        if (!util::starts_with(sans_scheme, "//"))
        {
            // no authority
            // (note that we still return an address; path() relies on this)
            return std::string_view(sans_scheme.data(), 0);
        }

        sans_scheme.remove_prefix(2);
        for (auto& c : sans_scheme)
        {
            if (c == '/') return util::make_string_view(sans_scheme.data(), &c);
        }
        return sans_scheme; // uri has authority and nothing else
    }

    std::string_view userinfo() const
    {
        auto a = authority();
        if (a.empty()) return {};
        auto pos = a.find_first_of('@');
        if (pos == std::string_view::npos) return {};
        return a.substr(0, pos);
    }

    std::string_view host() const
    {
        auto a = authority();
        if (a.empty()) return {};
        auto pos = a.find_first_of('@');
        if (pos == std::string_view::npos) pos = 0;
        else ++pos; // cut the @ symbol as well
        a.remove_prefix(pos);
        pos = a.find_first_of(':');
        return a.substr(0, pos);
    }

    std::string_view port() const
    {
        auto a = authority();
        if (a.empty()) return {};
        auto pos = a.find_first_of('@');
        if (pos == std::string_view::npos) pos = 0;
        pos = a.find_first_of(':', pos);
        if (pos == std::string_view::npos) return {};
        return a.substr(pos + 1);
    }

    std::string_view path() const
    {
        auto a = authority();
        auto post_authority = util::make_string_view(a.data() + a.length(), m_str.data() + m_str.length());
        for (auto& c : post_authority)
        {
            if (c == '?' || c == '#') return util::make_string_view(post_authority.data(), &c);
        }
        return post_authority;
    }

    std::string_view query() const
    {
        auto pos = m_str.find_first_of('?');
        if (pos == std::string_view::npos) return {}; // no query
        auto post_path = m_str.substr(pos + 1);
        return post_path.substr(0, post_path.find_first_of('#'));
    }

    // note that the fragment is returned with the hash prefix
    // thus one can differentiate between empty but existing fragment or no fragment at all
    std::string_view fragment() const
    {
        auto pos = m_str.find_last_of('#');
        if (pos == std::string_view::npos) return {};
        return m_str.substr(pos);
    }
};

}
