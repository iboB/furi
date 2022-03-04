// furi
// Copyright (c) 2022 Borislav Stanimirov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#include <furi/uri.hpp>
#include <furi/util/split.hpp>

#include <iostream>

using namespace std;
using namespace furi;

int main()
{
    uri u("https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top");
    //uri_view u("foo://xx/bar");

    cout << u.scheme() << '\n';
    cout << u.authority() << '\n';
    cout << u.userinfo() << '\n';
    cout << u.host() << '\n';
    cout << u.port() << '\n';
    cout << u.path() << '\n';
    cout << u.query() << '\n';
    cout << u.fragment() << '\n';

    return 0;
}
