// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include <doctest/doctest.h>

#include <furi/util/common_query_iterator.hpp>

#include <vector>

TEST_SUITE_BEGIN("furi");

using namespace furi::util;

void check(std::string_view path , const std::vector<common_query_iterator::value_type> parts)
{
    auto ex = parts.begin();
    for (auto i = common_query_iterator::make_begin(path); i; ++i, ++ex)
    {
        auto val = *i;
        CHECK(ex->first == val.first);
        CHECK(ex->second == val.second);
    }
    CHECK(ex == parts.end());
}

TEST_CASE("common query iterator")
{
    check("", {});
    check("abc", {{"abc", ""}});
    check("abc=123", {{"abc", "123"}});
    check("xy=23&q=z&f", {{"xy", "23"}, {"q", "z"}, {"f", ""}});
    check("f&xy=23&q=z", {{"f", ""}, {"xy", "23"}, {"q", "z"}});
    check("xy=23&f&q=z", {{"xy", "23"}, {"f", ""}, {"q", "z"}});
    check("a&b&c", {{"a", ""}, {"b", ""}, {"c", ""}});
}
