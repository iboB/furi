// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include <doctest/doctest.h>
#include <furi/furi.hpp>

#include <vector>

using namespace furi;

// the c api is extensively tested
// this test suite has only basic tests to check that the C++ wrapper has done its job

TEST_SUITE_BEGIN("furi");

void std_sv_test(opt_string_view e)
{
    std::string_view stdsv = e;
    CHECK(stdsv == e);
    CHECK(e == stdsv);
    CHECK(stdsv.length() == e.length());
    CHECK((void*)stdsv.data() == e.data()); // cast to void* to deal with potential DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
}

TEST_CASE("string view")
{
    opt_string_view e;
    CHECK(!e);
    CHECK(e.empty());
    CHECK(e.length() == 0);
    CHECK(e == e);
    CHECK(e == "");
    CHECK("" == e);
    CHECK("xx" != e);
    CHECK(e != "yy");
    CHECK(e.data() == nullptr);
    std_sv_test(e);

    opt_string_view e2 = "";
    CHECK(!!e2);
    CHECK(e2.empty());
    CHECK(e2.length() == 0);
    CHECK(e2 == e2);
    CHECK(e2 == e);
    CHECK(e == e2);
    CHECK(e2 == "");
    CHECK("" == e2);
    CHECK("xx" != e2);
    CHECK(e2 != "yy");
    CHECK(e2.data() != nullptr);
    std_sv_test(e2);

    std::string_view svabc = "abc";
    opt_string_view abc = svabc;
    CHECK(!!abc);
    CHECK(!abc.empty());
    CHECK(abc.length() == 3);
    CHECK(abc == abc);
    CHECK(abc != e);
    CHECK(e2 != abc);
    CHECK(abc == "abc");
    CHECK("abc" == abc);
    CHECK(abc != "xyz");
    CHECK(abc.data() != nullptr);
    std_sv_test(abc);
}

TEST_CASE("uri_split")
{
    std::string_view uri = "http://x.com:43/abc?xyz#top";
    auto s = uri_split::from_uri(uri);
    CHECK(s.scheme == "http");
    CHECK(s.scheme == uri_split::get_scheme_from_uri(uri));
    CHECK(s.authority == "x.com:43");
    CHECK(s.authority == uri_split::get_authority_from_uri(uri));
    CHECK(s.path == "abc");
    CHECK(s.path == uri_split::get_path_from_uri(uri));
    CHECK(s.query == "xyz");
    CHECK(s.query == uri_split::get_query_from_uri(uri));
    CHECK(s.fragment == "top");
    CHECK(s.fragment == uri_split::get_fragment_from_uri(uri));

    s = uri_split::from_uri("");
    CHECK_FALSE(s.scheme);
    CHECK_FALSE(s.authority);
    CHECK(bool(s.path));
    CHECK(s.path.empty());
    CHECK_FALSE(s.query);
    CHECK_FALSE(s.fragment);
}

TEST_CASE("authority_split")
{
    auto authority = "alice:pass@foo.com:44";
    auto s = authority_split::from_authority(authority);
    CHECK(s.userinfo == "alice:pass");
    CHECK(s.userinfo == authority_split::get_userinfo_from_authority(authority));
    CHECK(s.host == "foo.com");
    CHECK(s.host == authority_split::get_host_from_authority(authority));
    CHECK(s.port == "44");
    CHECK(s.port == authority_split::get_port_from_authority(authority));

    s = authority_split::from_authority("");
    CHECK_FALSE(s.userinfo);
    CHECK(bool(s.host));
    CHECK(s.host.empty());
    CHECK_FALSE(s.port);
}

TEST_CASE("userinfo_split")
{
    auto ui = "user:pass";
    auto s = userinfo_split::from_userinfo(ui);
    CHECK(s.username == "user");
    CHECK(s.username == userinfo_split::get_username_from_userinfo(ui));
    CHECK(s.password == "pass");
    CHECK(s.password == userinfo_split::get_password_from_userinfo(ui));

    CHECK(userinfo_split::get_username_from_userinfo("user") == "user");
    CHECK_FALSE(userinfo_split::get_password_from_userinfo("user"));
}

TEST_CASE("path_view/iter")
{
    std::vector<std::string_view> vec = {"foo", "bar", "baz", ""};

    path_view pv = "/foo/bar/baz/";
    std::vector<std::string_view> check;
    for (auto e : pv)
    {
        check.push_back(e);
    }

    CHECK(vec == check);
}

TEST_CASE("query_view/iter")
{
    std::vector<query_item> vec = {{"xy", "23"}, {"f", {}}, {"q", "z"}};

    query_view qv = "xy=23&f&q=z";
    std::vector<query_item> check;
    for (auto e : qv)
    {
        check.push_back(e);
    }

    CHECK(vec == check);
}
