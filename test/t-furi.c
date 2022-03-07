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
    TEST_ASSERT(furi_string_view_is_empty(e));
    TEST_ASSERT(furi_string_view_length(e) == 0);
    TEST_ASSERT(furi_string_view_cmp(e, e) == 0);
    TEST_ASSERT(furi_string_view_starts_with(e, ""));
    TEST_ASSERT_FALSE(furi_string_view_starts_with(e, "x"));
    TEST_ASSERT_NULL(furi_string_view_find_first(e, 'x'));
    TEST_ASSERT_NULL(furi_string_view_find_last(e, 'x'));

    furi_string_view e2 = furi_string_view_from_string("");
    TEST_ASSERT_NOT_NULL(e2.begin);
    TEST_ASSERT_NOT_NULL(e2.end);
    TEST_ASSERT_FALSE(furi_string_view_is_null(e2));
    TEST_ASSERT(furi_string_view_is_empty(e2));
    TEST_ASSERT(furi_string_view_length(e2) == 0);
    TEST_ASSERT(furi_string_view_cmp(e2, e2) == 0);
    TEST_ASSERT(furi_string_view_cmp(e, e2) == 0);
    TEST_ASSERT(furi_string_view_cmp(e2, e) == 0);
    TEST_ASSERT(furi_string_view_starts_with(e2, ""));
    TEST_ASSERT_FALSE(furi_string_view_starts_with(e2, "x"));
    TEST_ASSERT_NULL(furi_string_view_find_first(e2, 'x'));
    TEST_ASSERT_NULL(furi_string_view_find_last(e2, 'x'));

    {
        furi_string_view abc = furi_string_view_from_string("abc");
        TEST_ASSERT_NOT_NULL(abc.begin);
        TEST_ASSERT_NOT_NULL(abc.end);
        TEST_ASSERT_FALSE(furi_string_view_is_null(abc));
        TEST_ASSERT_FALSE(furi_string_view_is_empty(abc));
        TEST_ASSERT(furi_string_view_length(abc) == 3);
        TEST_ASSERT(furi_string_view_cmp(abc, abc) == 0);
        TEST_ASSERT(furi_string_view_cmp(e, abc) == -1);
        TEST_ASSERT(furi_string_view_cmp(abc, e) == 1);
        TEST_ASSERT(furi_string_view_cmp(e2, abc) == -1);
        TEST_ASSERT(furi_string_view_cmp(abc, e2) == 1);
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
    TEST_ASSERT_SV_EQUAL(s.scheme, furi_get_scheme_from_uri(uri));
    TEST_ASSERT_EXPECT_SV(authority, s.authority);
    TEST_ASSERT(!authority == !s.authority.begin);
    TEST_ASSERT_SV_EQUAL(s.authority, furi_get_authority_from_uri(uri));
    TEST_ASSERT_EXPECT_SV(path, s.path);
    TEST_ASSERT(!path == !s.path.begin);
    TEST_ASSERT_SV_EQUAL(s.path, furi_get_path_from_uri(uri));
    TEST_ASSERT_EXPECT_SV(query, s.query);
    TEST_ASSERT(!query == !s.query.begin);
    TEST_ASSERT_SV_EQUAL(s.query, furi_get_query_from_uri(uri));
    TEST_ASSERT_EXPECT_SV(fragment, s.fragment);
    TEST_ASSERT(!fragment == !s.fragment.begin);
    TEST_ASSERT_SV_EQUAL(s.fragment, furi_get_fragment_from_uri(uri));
}

void no_crash_uri_split_test(const char* struri)
{
    // test invalid splits
    // we don't care about the particular results here
    // we only care that these don't crash or cause sanitizer issues problems
    furi_string_view uri = furi_string_view_from_string(struri);
    furi_split_uri(uri);
    furi_get_scheme_from_uri(uri);
    furi_get_authority_from_uri(uri);
    furi_get_path_from_uri(uri);
    furi_get_query_from_uri(uri);
    furi_get_fragment_from_uri(uri);
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

    no_crash_uri_split_test(":xx:#");
    no_crash_uri_split_test("////");
    no_crash_uri_split_test("#sad");
    no_crash_uri_split_test("#:bad");
    no_crash_uri_split_test(":?:#mad");
    no_crash_uri_split_test("????");
}

void test_authority_split(const char* strauthority,
    const char* userinfo,
    const char* host,
    const char* port)
{
    furi_string_view a = furi_string_view_from_string(strauthority);
    furi_authority_split s = furi_split_authority(a);
    TEST_ASSERT_EXPECT_SV(userinfo, s.userinfo);
    TEST_ASSERT(!userinfo == !s.userinfo.begin);
    TEST_ASSERT_SV_EQUAL(s.userinfo, furi_get_userinfo_from_authority(a));
    TEST_ASSERT_EXPECT_SV(host, s.host);
    TEST_ASSERT(!host == !s.host.begin);
    TEST_ASSERT_SV_EQUAL(s.host, furi_get_host_from_authority(a));
    TEST_ASSERT_EXPECT_SV(port, s.port);
    TEST_ASSERT(!port == !s.port.begin);
    TEST_ASSERT_SV_EQUAL(s.port, furi_get_port_from_authority(a));
}

void no_crash_authority_split_test(const char* strauthority)
{
    furi_string_view a = furi_string_view_from_string(strauthority);
    furi_split_authority(a);
    furi_get_userinfo_from_authority(a);
    furi_get_host_from_authority(a);
    furi_get_port_from_authority(a);
}

void authority_split(void)
{
    test_authority_split("", NULL, "", NULL);
    test_authority_split("xxx", NULL, "xxx", NULL);
    test_authority_split("alice@abc", "alice", "abc", NULL);
    test_authority_split("bob:pass@123.33", "bob:pass", "123.33", NULL);
    test_authority_split("foo.com:34", NULL, "foo.com", "34");
    test_authority_split("alice:pass@foo.com:44", "alice:pass", "foo.com", "44");

    // ipv6
    test_authority_split("[::1]", NULL, "[::1]", NULL);
    test_authority_split("bob:pass@[2001:db8::ff00:42:8329]", "bob:pass", "[2001:db8::ff00:42:8329]", NULL);
    test_authority_split("[::ff]:222", NULL, "[::ff]", "222");
    test_authority_split("x@[::2]:5", "x", "[::2]", "5");
    test_authority_split("x:y@[::b]:222", "x:y", "[::b]", "222");

    no_crash_authority_split_test("]");
    no_crash_authority_split_test("[");
    no_crash_authority_split_test(":::");
    no_crash_authority_split_test("@");
    no_crash_authority_split_test("@]");
    no_crash_authority_split_test("@[");
    no_crash_authority_split_test("][");
    no_crash_authority_split_test("[::]xx");
}

void test_userinfo_split(const char* strui, const char* username, const char* password)
{
    furi_string_view ui = furi_string_view_from_string(strui);
    furi_userinfo_split s = furi_split_userinfo(ui);
    TEST_ASSERT_EXPECT_SV(username, s.username);
    TEST_ASSERT(!username == !s.username.begin);
    TEST_ASSERT_SV_EQUAL(s.username, furi_get_username_from_userinfo(ui));
    TEST_ASSERT_EXPECT_SV(password, s.password);
    TEST_ASSERT(!password == !s.password.begin);
    TEST_ASSERT_SV_EQUAL(s.password, furi_get_password_from_userinfo(ui));
}

void useinfo_split(void)
{
    test_userinfo_split("user", "user", NULL);
    test_userinfo_split("user:", "user", "");
    test_userinfo_split("user:pass", "user", "pass");
    test_userinfo_split(":pass", "", "pass");
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(string_view);
    RUN_TEST(uri_split);
    RUN_TEST(authority_split);
    RUN_TEST(useinfo_split);
    return UNITY_END();
}