// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include <furi/uri.hpp>
#include <furi/util/split.hpp>
#include <furi/common_query_view.hpp>
#include <furi/path_view.hpp>

#include <iostream>

using namespace std;
using namespace furi;

int main()
{
    uri u("https://john.doe@www.example.com:123/forum/questions/aa?tag=networking&order=newest#top");
    //uri_view u("foo://xx/bar");

    auto split = util::split_uri(u.sv());
    auto asplit = util::split_authority(split.authority);

    cout << u.scheme() << '\n';
    cout << split.scheme << '\n';
    cout << u.authority() << '\n';
    cout << split.authority << '\n';
    cout << u.userinfo() << '\n';
    cout << asplit.userinfo << '\n';
    cout << u.host() << '\n';
    cout << asplit.host << '\n';
    cout << u.port() << '\n';
    cout << asplit.port << '\n';
    cout << u.path() << '\n';
    cout << split.path << '\n';
    cout << u.query() << '\n';
    cout << split.query << '\n';
    cout << u.fragment() << '\n';
    cout << split.fragment << '\n';

    cout << "\n\n";
    common_query_view query(split.query);
    for (auto p : query)
    {
        cout << p.first << " : " << p.second << '\n';
    }

    cout << "\n\n";
    path_view path(split.path);
    for (auto e : path)
    {
        cout << "/ " << e << '\n';
    }

    return 0;
}
