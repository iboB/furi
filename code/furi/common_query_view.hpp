// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once
#include "string_view_wrap.hpp"
#include "util/common_query_iterator.hpp"

namespace furi
{

class common_query_view : public string_view_wrap
{
public:
    using string_view_wrap::string_view_wrap;

    using const_iterator = util::common_query_iterator;
    const_iterator begin() const { return const_iterator::make_begin(sv()); }
    const_iterator end() const { return const_iterator::make_end(sv()); }
};

}
