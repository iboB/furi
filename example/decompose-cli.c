// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#include <furi/furi.h>
#include <stdio.h>

void print_sv(furi_sv sv) {
    if (furi_sv_is_null(sv)) {
        printf("<none>");
    }
    else {
        printf("\"%.*s\"", (int)furi_sv_length(sv), sv.begin);
    }
}

void print_elem(const char* title, furi_sv sv) {
    printf("%s: ", title);
    print_sv(sv);
    puts("");
}

void decompose(const char* uri) {
    puts(uri);
    furi_uri_split split = furi_split_uri(furi_make_sv_from_string(uri));

    print_elem("scheme   ", split.scheme);
    print_elem("authority", split.authority);
    if (!furi_sv_is_empty(split.authority)) {
        furi_authority_split asplit = furi_split_authority(split.authority);
        print_elem("  userinfo", asplit.userinfo);
        print_elem("  host    ", asplit.host);
        print_elem("  port    ", asplit.port);
    }
    print_elem("path     ", split.path);
    if (!furi_sv_is_empty(split.path)) {
        printf("  elements: ");
        for (furi_path_iter it = furi_make_path_iter_begin(split.path); !furi_path_iter_is_done(it); furi_path_iter_next(&it)) {
            print_sv(furi_path_iter_get_value(it));
            printf(" ");
        }
        puts("");
    }
    print_elem("query    ", split.query);
    if (!furi_sv_is_empty(split.query)) {
        printf("  elements: ");
        for (furi_query_iter it = furi_make_query_iter_begin(split.query); !furi_query_iter_is_done(it); furi_query_iter_next(&it)) {
            furi_query_iter_value value = furi_query_iter_get_value(it);
            print_sv(value.key);
            printf("=");
            print_sv(value.value);
            printf(" ");
        }
        puts("");
    }
    print_elem("fragment ", split.fragment);
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        puts("No URIs provided. Using an example one.");
        decompose("https://example.com/over/there?name=ferret");
        return 0;
    }

    for (int i=1; i<argc; ++i) {
        decompose(argv[i]);
    }

    return 0;
}
