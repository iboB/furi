// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include <doctest/doctest.h>

#include <furi/util/path_iterator.hpp>

#include <vector>

TEST_SUITE_BEGIN("furi");

using namespace furi::util;

void check(std::string_view path , const std::vector<std::string_view> parts)
{
    auto ex = parts.begin();
    for (auto i = path_iterator::make_begin(path); i; ++i, ++ex)
    {
        CHECK(*ex == *i);
    }
    CHECK(ex == parts.end());
}

TEST_CASE("path iterator")
{
    check("", {});
    check("foo", {"foo"});
    check("/foo", {"foo"});
    check("foo/", {"foo", ""});
    check("/foo/", {"foo", ""});
    check("foo/bar/baz", {"foo", "bar", "baz"});
    check("/foo/bar/baz", {"foo", "bar", "baz"});
    check("/foo/bar/baz/", {"foo", "bar", "baz", ""});
    check("foo/bar/baz/", {"foo", "bar", "baz", ""});
}
