// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include <doctest/doctest.h>

#include <furi/util/authority_split.hpp>

TEST_SUITE_BEGIN("furi");

using namespace furi::util;

void atest(std::string_view a, const authority_split ex)
{
    auto s = split_authority(a);
    CHECK(ex.userinfo == s.userinfo);
    CHECK(s.userinfo == get_userinfo_from_authority(a));
    CHECK(ex.host == s.host);
    CHECK(s.host == get_host_from_authority(a));
    CHECK(ex.port == s.port);
    CHECK(s.port == get_port_from_authority(a));
}

TEST_CASE("authority split")
{
    atest("", {});
    atest("xxx", {{}, {"xxx"}, {}});
    atest("alice@abc", {{"alice"}, {"abc"}, {}});
    atest("bob:pass@123.33", {{"bob:pass"}, {"123.33"}, {}});
    atest("foo.com:34", {{}, {"foo.com"}, {"34"}});
    atest("alice:pass@foo.com:44", {{"alice:pass"}, {"foo.com"}, {"44"}});
}

TEST_CASE("authority split ipv6")
{
    atest("[::1]", {{}, {"[::1]"}, {}});
    atest("bob:pass@[2001:db8::ff00:42:8329]", {{"bob:pass"}, {"[2001:db8::ff00:42:8329]"}, {}});
    atest("[::ff]:222", {{}, {"[::ff]"}, {"222"}});
    atest("x@[::2]:5", {{"x"}, {"[::2]"}, {"5"}});
    atest("x:y@[::b]:222", {{"x:y"}, {"[::b]"}, {"222"}});
}

void uitest(std::string_view ui, userinfo_split ex)
{
    auto s = split_userinfo(ui);
    CHECK(ex.username == s.username);
    CHECK(s.username == get_username_from_userinfo(ui));
    CHECK(ex.password == s.password);
    CHECK(s.password == get_password_from_userinfo(ui));
}

TEST_CASE("userinfo split")
{
    uitest("user", {"user", ""});
    uitest("user:", {"user", ""});
    uitest("user:pass", {"user", "pass"});
    uitest(":pass", {"", "pass"});
}

void no_crash_test(std::string_view a)
{
    split_authority(a);
    get_userinfo_from_authority(a);
    get_host_from_authority(a);
    get_port_from_authority(a);
}

TEST_CASE("invalid split")
{
    // test invalid splits
    // we don't care about the particular results here
    // we only care that these don't crash
    no_crash_test("]");
    no_crash_test("[");
    no_crash_test(":::");
    no_crash_test("@");
    no_crash_test("@]");
    no_crash_test("@[");
    no_crash_test("][");
    no_crash_test("[::]xx");
}
