// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include <doctest/doctest.h>

#include <furi/util/split.hpp>

TEST_SUITE_BEGIN("furi");

using namespace furi::util;

void stest(std::string_view uri, const split ex)
{
    auto s = split_uri(uri);
    CHECK(ex.scheme == s.scheme);
    CHECK(s.scheme == get_scheme_from_uri(uri));
    CHECK(ex.authority == s.authority);
    CHECK(s.authority == get_authority_from_uri(uri));
    CHECK(ex.path == s.path);
    CHECK(s.path == get_path_from_uri(uri));
    CHECK(ex.query == s.query);
    CHECK(s.query == get_query_from_uri(uri));
    CHECK(ex.fragment == s.fragment);
    CHECK(s.fragment == get_fragment_from_uri(uri));
}

TEST_CASE("split")
{
    stest("", {});
    stest("asdf", {{}, {}, {"asdf"}, {}, {}});
    stest("xx:asdf", {{"xx"}, {}, {"asdf"}, {}, {}});
    stest("a-b://asdf", {{"a-b"}, {"asdf"}, {}, {}, {}});
    stest("a-b://asdf/", {{"a-b"}, {"asdf"}, {"/"}, {}, {}});
    stest("sys:x/y/z?q=4&qq=m", {{"sys"}, {""}, {"x/y/z"}, {"q=4&qq=m"}, {}});
    stest("sys:x/y/z?zz#", {{"sys"}, {""}, {"x/y/z"}, {"zz"}, {"#"}});
    stest("mailto:g@gg.com", {{"mailto"}, {""}, {"g@gg.com"}, {""}, {""}});
    stest("http://x.com:43/abc?xyz#top", {{"http"}, {"x.com:43"}, {"/abc"}, {"xyz"}, {"#top"}});
    stest("https://[2001:db8::ff00:42:8329]:43/xxx", {{"https"}, {"[2001:db8::ff00:42:8329]:43"}, {"/xxx"}, {}, {}});
}

void no_crash_test(std::string_view uri)
{
    split_uri(uri);
    get_scheme_from_uri(uri);
    get_authority_from_uri(uri);
    get_path_from_uri(uri);
    get_query_from_uri(uri);
    get_fragment_from_uri(uri);
}

TEST_CASE("invalid split")
{
    // test invalid splits
    // we don't care about the particular results here
    // we only care that these don't crash
    no_crash_test(":xx:#");
    no_crash_test("////");
    no_crash_test("#sad");
    no_crash_test("#:bad");
    no_crash_test(":?:#mad");
    no_crash_test("????");
}
