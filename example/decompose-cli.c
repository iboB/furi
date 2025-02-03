// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#include <furi/furi.h>
#include <stdio.h>

void print_elem(const char* title, furi_sv sv) {
    printf("%s: ", title);
    if (furi_sv_is_null(sv)) {
        puts("<none>");
    }
    else {
        printf("\"%.*s\"\n", (int)furi_sv_length(sv), sv.begin);
    }
}

void decompose(const char* uri) {
    puts(uri);
    furi_uri_split split = furi_split_uri(furi_make_sv_from_string(uri));

    print_elem("scheme   ", split.scheme);
    print_elem("authority", split.authority);
    print_elem("path     ", split.path);
    print_elem("query    ", split.query);
    print_elem("fragment ", split.fragment);
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        puts("No URIs provided. Using an example one.");
        decompose("https://example.com/over/there?name=ferret");
        return 0;
    }

    for (int i=1; i<argv; ++i) {
        decompose(argv[i]);
    }

    return 0;
}
