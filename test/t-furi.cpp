// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include <doctest/doctest.h>
#include <furi/furi.hpp>

using namespace furi;

TEST_SUITE_BEGIN("furi");

void std_sv_test(opt_string_view e)
{
    std::string_view stdsv = e;
    CHECK(stdsv == e);
    CHECK(stdsv.length() == e.length());
    CHECK(stdsv.data() == e.data());
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

    opt_string_view abc = "abc";
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
