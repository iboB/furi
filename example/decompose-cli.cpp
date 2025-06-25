// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#include <furi/furi.hpp>
#include <iostream>

void print_elem(const char* title, const furi::opt_string_view& sv) {
    std::cout << title << ": ";
    if (sv.null()) {
        std::cout << "<none>\n";
    }
    else {
        std::cout << "\"" << sv << "\"\n";
    }
}

void decompose(const std::string& uri) {
    std::cout << uri << '\n';
    auto split = furi::uri_split::from_uri(uri);

    print_elem("scheme   ", split.scheme);
    print_elem("authority", split.authority);
    if (!split.authority.empty()) {
        auto asplit = furi::authority_split::from_authority(split.authority);
        print_elem("  userinfo", asplit.userinfo);
        print_elem("  host    ", asplit.host);
        print_elem("  port    ", asplit.port);
    }
    print_elem("path     ", split.path);
    print_elem("query    ", split.query);
    print_elem("fragment ", split.fragment);
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "No URIs provided. Using an example one.\n";
        decompose("https://example.com/over/there?name=ferret");
        return 0;
    }

    for (int i = 1; i < argc; ++i) {
        decompose(argv[i]);
    }

    return 0;
}