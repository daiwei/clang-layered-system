#include <common/str.h>
#include <criterion/criterion.h>


static strbuf_t *strbuf(str_t str)
{
    return (strbuf_t *)(str - sizeof(strbuf_t));
}


TestSuite(suite_str);

// Test(suite_str, str_new) {
//     str_t str = str_new();
//
//     cr_assert_neq(NULL, str);
//     cr_assert_neq(NULL, strbuf(str));
//     cr_assert_eq(DEFAULT_STRING_LEN + 1, strbuf(str)->capacity);
// }
//
// Test(suite_str, str_new__cstr) {
//     str_t str = str_new__cstr("test");
//     cr_assert_str_eq(str, "test");
//     cr_assert_eq(DEFAULT_STRING_LEN + 1, strbuf(str)->capacity);
// }
//
// Test(suite_str, str_new__cstr2) {
//     str_t str = str_new__cstr2("test", 3);
//     cr_assert_str_eq(str, "tes");
//     cr_assert_eq(DEFAULT_STRING_LEN + 1, strbuf(str)->capacity);
// }
//
// Test(suite_str, str_new__fmt) {
//     // str_t str = str_new__fmt(DEFAULT_STRING_LEN + 10, "%s%f", "1234", 5.0);
//     // cr_assert_str_eq(str, "12345.0");
//     // cr_assert_eq(DEFAULT_STRING_LEN + 10, strbuf(str)->capacity);
//
//     static const char *fmt = "$FRCMD,%s,_SendMessage, ,%f,%c,%f,%c,%f,%d,%d,%s,%s,%d";
//     str_t str = str_new__fmt(1024, fmt, "025001",
//                         10.0, 'c', 10.0, 'c',
//                         10.0, 10, 10,
//                         "datebuf", "timebuf", 0);
//
//     log_debug("%s", str);
// }
//
// Test(suite_str, str_clone) {
//     str_t str = str_new__cstr("test");
//     str_t str1 = str_clone(str);
//     cr_assert_neq(str, str1);
//     cr_assert_neq(strbuf(str), strbuf(str1));
//     cr_assert_str_eq(str1, "test");
// }
//
// Test(suite_str, str_len) {
//     str_t str = str_new__cstr("test");
//     cr_assert_eq(str_len(str), 4);
// }
//
// Test(suite_str, str_capacity) {
//     str_t str = str_new__cstr("test");
//     cr_assert_eq(str_capacity(str), DEFAULT_STRING_LEN + 1);
// }

// Test(suite_str, str_trim) {
//     str_t str = str_new__cstr(" testfsdfasfsdf ");
//     str = str_trim(str);
//     log_debug("%s", str);
//     cr_assert_str_eq(str, "testfsdfasfsdf");
// }

// Test(suite_str, str_erase) {
//     str_t str = str_new__cstr("test");
//     str_erase(str);
//     cr_assert_eq(str_len(str), 0);
//     cr_assert_str_eq(str, "");
// }
//
// Test(suite_str, str_compare) {
//     str_t str00 = str_new__cstr("test");
//     str_t str01 = str_new__cstr("test");
//     cr_assert_eq(str_compare(str00, str01), 0);
//
//     str_t str10 = str_new__cstr("test");
//     str_t str11 = str_new__cstr("tes");
//     cr_assert(str_compare(str10, str11) > 0);
//
//     str_t str20 = str_new__cstr("tast");
//     str_t str21 = str_new__cstr("test");
//     cr_assert(str_compare(str20, str21) < 0);
// }
//
// Test(suite_str, str_at) {
//     str_t str = str_new__cstr("test");
//     cr_assert_eq(str_at(str, 2), 's');
// }
//
// Test(suite_str, str_char) {
//     str_t str = str_new__cstr("test");
//     cr_assert_eq(str_char(str, 's'), 2);
// }
//
// Test(suite_str, str_substr) {
//     str_t str = str_new__cstr("test");
//     int idx = str_substr(str, "es");
//     cr_assert_eq(idx, 1);
// }
//
// Test(suite_str, str_start_with) {
//     str_t str = str_new__cstr("test");
//     cr_assert(str_start_with(str, "tes"));
// }
//
// Test(suite_str, str_end_with) {
//     str_t str = str_new__cstr("test");
//     cr_assert(str_end_with(str, "st"));
// }
//
// Test(suite_str, str_count) {
//     str_t str = str_new__cstr("This guide presents information about BSP Layers, "
//             "defines a structure for components so that BSPs follow a commonly understood layout");
//     int num = str_count(str, "BSP");
//     cr_assert_eq(num, 2);
// }
//
// Test(suite_str, str_split) {
//     str_t str = str_new__cstr("This guide presents information about BSP Layers, "
//             "defines a structure for components so that BSPs follow a commonly understood layout");
//     str_t *ret = str_split(str, "BSP");
//     log_debug("%s", ret[0]);
//     log_debug("%s", ret[1]);
//     log_debug("%s", ret[2]);
//     cr_assert_eq(ret[3], NULL);
// }
//
// Test(suite_str, str_scan) {
//     str_t str = str_new__cstr("test,123");
//     char s[10] = {0};
//     int i = 0;
//     str_scan(str, "%[^,],%d", s, &i);
//     cr_assert_str_eq(s, "test");
//     cr_assert_eq(i, 123);
// }
//
// Test(suite_str, str_resize) {
//     str_t str = str_new__cstr("test");
//     str = str_resize(str, 10);
//     log_debug("%s", str);
//     cr_assert_str_eq(str, "test");
//     cr_assert_eq(DEFAULT_STRING_LEN + 1, strbuf(str)->capacity);
//
//     str = str_resize(str, DEFAULT_STRING_LEN + 10);
//     log_debug("%s", str);
//     cr_assert_str_eq(str, "test");
//     cr_assert_eq(DEFAULT_STRING_LEN + 11, strbuf(str)->capacity);
// }
//
// Test(suite_str, str_append) {
//     str_t str = str_new__cstr("test");
//     str = str_append(str, "_new", "_cmd");
//     log_debug("%s", str);
//     cr_assert_str_eq(str, "test_new_cmd");
// }
//
// Test(suite_str, str_prepend) {
//     str_t str = str_new__cstr("test");
//     str = str_prepend(str, "new_");
//     cr_assert_str_eq(str, "new_test");
// }
//
// Test(suite_str, str_insert) {
//     str_t str0 = str_new__cstr("test");
//     str0 =str_insert(str0, 1, "#new#");
//     cr_assert_str_eq(str0, "t#new#est");
//
//     str_t str1 = str_new__cstr("test");
//     str1 = str_insert(str1, 0, "#new#");
//     cr_assert_str_eq(str1, "#new#test");
//
//     str_t str2 = str_new__cstr("test");
//     str2 = str_insert(str2, 4, "#new#");
//     cr_assert_str_eq(str2, "test#new#");
//
//     str_t str3 = str_new__cstr("test");
//     cr_assert_eq(str_insert(str3, 10, "#new#"), NULL);
// }
//
// Test(suite_str, str_replace) {
//     str_t str = str_new__cstr("This guide presents information about BSP Layers, "
//             "defines a structure for components so that BSPs follow a commonly understood layout");
//     str = str_replace(str, "BSP", "TEST");
//     log_debug("%s", str);
//     // cr_assert_eq(1, 1);
// }
