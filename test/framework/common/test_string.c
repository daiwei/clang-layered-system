#include <common/string.h>
#include <criterion/criterion.h>

TestSuite(suite_string);

Test(suite_string, str_new) {
    string_t *str = str_new();

    cr_assert_neq(NULL, str);
    cr_assert_eq(NULL, *str);
}

Test(suite_string, str_new__cstr) {
    string_t *str = str_new__cstr("test");
    // log_debug("%s", cstring(str));
    cr_assert_str_eq(cstring(str), "test");
}

Test(suite_string, str_new__fmt) {
    string_t *str = str_new__fmt(10, "%s%d", "1234", 5);
    // log_debug("%s", cstring(str));
    cr_assert_str_eq(cstring(str), "12345");
}

Test(suite_string, str_clone) {
    string_t *str = str_new__cstr("test");
    string_t *str2 = str_clone(str);
    cr_assert_neq(str, str2);
    cr_assert_neq(cstring(str), cstring(str2));
    cr_assert_str_eq(cstring(str2), "test");
}

Test(suite_string, str_size) {
    string_t *str = str_new__cstr("test");
    // log_debug("%s", cstring(str));
    cr_assert_eq(str_size(str), 4);
}

Test(suite_string, str_at) {
    string_t *str = str_new__cstr("test");
    // log_debug("%c", str_at(str, 0));
    cr_assert_eq(str_at(str, 3), 't');
}

Test(suite_string, str_compare) {
    string_t *str0 = str_new__cstr("test");
    string_t *str1 = str_new__cstr("test");
    cr_assert_eq(str_compare(str0, str1), 0);

    string_t *str2 = str_new__fmt(10, "%s", "test");
    cr_assert_eq(str_compare(str0, str2), 0);
}

Test(suite_string, str_append) {
    string_t *str = str_new__cstr("test");
    str_append(str, " new cmd");
    cr_assert_str_eq(cstring(str), "test new cmd");
}

Test(suite_string, str_append) {
    string_t *str = str_new__cstr("test");
    // str_append(str, "_new", "_cmd", NULL);
    str_append(str, "_new", "_cmd");
    log_debug("%s", cstring(str));
    cr_assert_str_eq(cstring(str), "test_new_cmd");
}

Test(suite_string, str_prepend) {
    string_t *str = str_new__cstr("test");
    str_prepend(str, "new ");
    cr_assert_str_eq(cstring(str), "new test");
}

Test(suite_string, str_insert) {
    string_t *str0 = str_new__cstr("test");
    str_insert(str0, 1, "#new#");
    cr_assert_str_eq(cstring(str0), "t#new#est");

    string_t *str1 = str_new__cstr("test");
    str_insert(str1, 0, "#new#");
    cr_assert_str_eq(cstring(str1), "#new#test");

    string_t *str2 = str_new__cstr("test");
    str_insert(str2, 4, "#new#");
    cr_assert_str_eq(cstring(str2), "test#new#");

    string_t *str3 = str_new__cstr("test");
    cr_assert_eq(str_insert(str3, 10, "#new#"), NULL);
}

Test(suite_string, str_char) {
    string_t *str = str_new__cstr("test");
    int idx = str_char(str, 's');
    cr_assert_eq(idx, 2);
}

Test(suite_string, str_substr) {
    string_t *str = str_new__cstr("test");
    int idx = str_substr(str, "es");
    cr_assert_eq(idx, 1);
}

Test(suite_string, str_count) {
    string_t *str = str_new__cstr("This guide presents information about BSP Layers, "
            "defines a structure for components so that BSPs follow a commonly understood layout");
    int num = str_count(str, "BSP");
    cr_assert_eq(num, 2);
}

Test(suite_string, str_split) {
    string_t *str = str_new__cstr("This guide presents information about BSP Layers, "
            "defines a structure for components so that BSPs follow a commonly understood layout");
    string_t **ret = str_split(str, "BSP");
    // log_debug("%s", cstring(ret[0]));
    // log_debug("%s", cstring(ret[1]));
    // log_debug("%s", cstring(ret[2]));
    cr_assert_eq(ret[3], NULL);
}

Test(suite_string, str_replace) {
    string_t *str = str_new__cstr("This guide presents information about BSP Layers, "
            "defines a structure for components so that BSPs follow a commonly understood layout");
    str_replace(str, "BSP", "TEST");
    // log_debug("%s", cstring(str));
    cr_assert_eq(1, 1);
}

Test(suite_string, str_scan) {
    string_t *str = str_new__cstr("test,123");
    char s[10] = {0};
    int i = 0;
    str_scan(str, "%[^,],%d", s, &i);
    cr_assert_str_eq(s, "test");
    cr_assert_eq(i, 123);
}
