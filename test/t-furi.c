#include <unity.h>

#include <furi/furi.h>

void setUp(void) {}
void tearDown(void) {}

#define TEST_ASSERT_SV_EQUAL(a, b) TEST_ASSERT(furi_string_view_cmp(a, b) == 0)
#define TEST_ASSERT_EXPECT_SV(expected, sv) TEST_ASSERT_SV_EQUAL(furi_string_view_from_string(expected), sv)

void string_view(void)
{
    furi_string_view e = {0};
    TEST_ASSERT_NULL(e.begin);
    TEST_ASSERT_NULL(e.end);
    TEST_ASSERT(furi_string_view_is_null(e));
    TEST_ASSERT(furi_string_view_length(e) == 0);
    TEST_ASSERT(furi_string_view_cmp(e, e) == 0);
    TEST_ASSERT(furi_string_view_starts_with(e, ""));
    TEST_ASSERT_FALSE(furi_string_view_starts_with(e, "x"));
    TEST_ASSERT_NULL(furi_string_view_find_first(e, 'x'));

    {
        furi_string_view abc = furi_string_view_from_string("abc");
        TEST_ASSERT_FALSE(furi_string_view_is_null(abc));
        TEST_ASSERT(furi_string_view_length(abc) == 3);
        TEST_ASSERT(furi_string_view_cmp(abc, abc) == 0);
        TEST_ASSERT(furi_string_view_cmp(e, abc) == -1);
        TEST_ASSERT(furi_string_view_cmp(abc, e) == 1);
        TEST_ASSERT(furi_string_view_starts_with(abc, "a"));
        TEST_ASSERT(furi_string_view_starts_with(abc, "ab"));
        TEST_ASSERT(furi_string_view_starts_with(abc, "abc"));
        TEST_ASSERT_FALSE(furi_string_view_starts_with(abc, "abcd"));
        TEST_ASSERT_FALSE(furi_string_view_starts_with(abc, "x"));
        TEST_ASSERT_NULL(furi_string_view_find_first(abc, 'x'));
        TEST_ASSERT_NULL(furi_string_view_find_last(abc, 'x'));
        const char* p = furi_string_view_find_first(abc, 'b');
        TEST_ASSERT_NOT_NULL(p);
        const char* p2 = furi_string_view_find_last(abc, 'b');
        TEST_ASSERT_EQUAL_PTR(p, p2);
        furi_string_view bc = furi_make_string_view(p, abc.end);
        TEST_ASSERT_EXPECT_SV("bc", bc);
    }

    {
        furi_string_view foof = furi_string_view_from_string("foof");
        TEST_ASSERT_NULL(furi_string_view_find_first(foof, 'x'));
        const char* p = furi_string_view_find_first(foof, 'o');
        TEST_ASSERT_NOT_NULL(p);
        furi_string_view oof = furi_make_string_view(p, foof.end);
        TEST_ASSERT_EXPECT_SV("oof", oof);
        const char* p2 = furi_string_view_find_last(foof, 'o');
        TEST_ASSERT_EQUAL_PTR(p + 1, p2);
        furi_string_view fo = furi_make_string_view(foof.begin, p2);
        TEST_ASSERT_EXPECT_SV("fo", fo);
    }

}

void test_uri_split(const char* struri,
    const char* scheme,
    const char* authority,
    const char* path,
    const char* query,
    const char* fragment)
{
    furi_string_view uri = furi_string_view_from_string(struri);
    furi_uri_split s = furi_split_uri(uri);
    TEST_ASSERT_EXPECT_SV(scheme, s.scheme);
    TEST_ASSERT(!scheme == !s.scheme.begin);
    TEST_ASSERT_SV_EQUAL(s.scheme, get_scheme_from_uri(uri));
    TEST_ASSERT_EXPECT_SV(authority, s.authority);
    TEST_ASSERT(!authority == !s.authority.begin);
    TEST_ASSERT_SV_EQUAL(s.authority, get_authority_from_uri(uri));
    TEST_ASSERT_EXPECT_SV(path, s.path);
    TEST_ASSERT(!path == !s.path.begin);
    TEST_ASSERT_SV_EQUAL(s.path, get_path_from_uri(uri));
    TEST_ASSERT_EXPECT_SV(query, s.query);
    TEST_ASSERT(!query == !s.query.begin);
    TEST_ASSERT_SV_EQUAL(s.query, get_query_from_uri(uri));
    TEST_ASSERT_EXPECT_SV(fragment, s.fragment);
    TEST_ASSERT(!fragment == !s.fragment.begin);
    TEST_ASSERT_SV_EQUAL(s.fragment, get_fragment_from_uri(uri));
}

void uri_split(void)
{
    test_uri_split("", NULL, NULL, "", NULL, NULL);
    test_uri_split("asdf", NULL, NULL, "asdf", NULL, NULL);
    test_uri_split("x/y", NULL, NULL, "x/y", NULL, NULL);
    test_uri_split("asdf/foo.bar", NULL, NULL, "asdf/foo.bar", NULL, NULL);
    test_uri_split("/usr/bin/xyz", NULL, NULL, "/usr/bin/xyz", NULL, NULL);
    test_uri_split("xx:asdf", "xx", NULL, "asdf", NULL, NULL);
    test_uri_split("a-b://asdf", "a-b", "asdf", NULL, NULL, NULL);
    test_uri_split("a-b://asdf/", "a-b", "asdf", "/", NULL, NULL);
    test_uri_split("sys:x/y/z?q=4&qq=m", "sys", NULL, "x/y/z", "q=4&qq=m", NULL);
    test_uri_split("sys:x/y/z?zz#", "sys", NULL, "x/y/z", "zz", "");
    test_uri_split("mailto:g@gg.com", "mailto", NULL, "g@gg.com", NULL, NULL);
    test_uri_split("http://x.com:43/abc?xyz#top", "http", "x.com:43", "/abc", "xyz", "top");
    test_uri_split("https://[2001:db8::ff00:42:8329]:43/xxx", "https", "[2001:db8::ff00:42:8329]:43", "/xxx", NULL, NULL);
    test_uri_split("file:///home/user/f.txt", "file", "", "/home/user/f.txt", NULL, NULL);
    test_uri_split("file://localhost/home/user/f.txt", "file", "localhost", "/home/user/f.txt", NULL, NULL);
}

void no_crash_uri_split_test(const char* struri)
{
    furi_string_view uri = furi_string_view_from_string(struri);
    furi_split_uri(uri);
    get_scheme_from_uri(uri);
    get_authority_from_uri(uri);
    get_path_from_uri(uri);
    get_query_from_uri(uri);
    get_fragment_from_uri(uri);
}

void invalid_uri_split(void)
{
    // test invalid splits
    // we don't care about the particular results here
    // we only care that these don't crash or cause sanitizer issues problems
    no_crash_uri_split_test(":xx:#");
    no_crash_uri_split_test("////");
    no_crash_uri_split_test("#sad");
    no_crash_uri_split_test("#:bad");
    no_crash_uri_split_test(":?:#mad");
    no_crash_uri_split_test("????");
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(string_view);
    RUN_TEST(uri_split);
    RUN_TEST(invalid_uri_split);
    return UNITY_END();
}