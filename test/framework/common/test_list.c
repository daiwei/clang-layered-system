#include <common/list.h>
#include <criterion/criterion.h>

TestSuite(suite_list);

int log_data(void *data)
{
    log_debug("%s", (char*)data);
}

// Test(suite_list, test_init) {
//     list_t *list = list_new();
//
//     cr_assert_neq(NULL, list);
//     cr_assert_eq(0, list->size);
// }
//
// Test(suite_list, test_insert_0) {
//     list_t *list = list_new();
//     list_insert(list, 0, "node0");
//
//     cr_assert_eq(1, list->size);
//     cr_assert_str_eq("node0", (char*)list_get(list, 0)->data);
//     cr_assert_str_eq("node0", (char*)list_get(list, -1)->data);
// }
//
// Test(suite_list, test_insert_neg_1) {
//     list_t *list = list_new();
//     list_insert(list, -1, "node0");
//
//     cr_assert_eq(1, list->size);
//     cr_assert_str_eq("node0", (char*)list_get(list, 0)->data);
//     cr_assert_str_eq("node0", (char*)list_get(list, -1)->data);
// }
//
// Test(suite_list, test_remove_neg_1) {
//     list_t *list = list_new();
//     list_insert(list, 0, "node0");
//     cr_assert_eq(1, list->size);
//     cr_assert_str_eq("node0", (char*)list_get(list, 0)->data);
//     cr_assert_str_eq("node0", (char*)list_get(list, -1)->data);
//
//     list_remove(list, -1);
//     cr_assert_eq(0, list->size);
// }
//
// Test(suite_list, test_append) {
//     list_t *list = list_new();
//     list_rpush(list, "node0");
//     list_rpush(list, "node1");
//     list_rpush(list, "node2");
//     list_foreach(list, log_data);
//
//     cr_assert_eq(3, list->size);
//     cr_assert_str_eq("node0", (char*)list_get(list, 0)->data);
//     cr_assert_str_eq("node2", (char*)list_get(list, 2)->data);
// }
//
// Test(suite_list, test_prepend) {
//     list_t *list = list_new();
//     list_lpush(list, "node0");
//     list_lpush(list, "node1");
//     list_lpush(list, "node2");
//     list_foreach(list, log_data);
//
//     cr_assert_eq(3, list->size);
//     cr_assert_str_eq("node2", (char*)list_get(list, 0)->data);
//     cr_assert_str_eq("node0", (char*)list_get(list, 2)->data);
// }
//
// Test(suite_list, test_pop) {
//     list_t *list = list_new();
//     list_lpush(list, "node0");
//     list_lpush(list, "node1");
//     list_lpush(list, "node2");
//     list_lpush(list, "node3");
//     list_foreach(list, log_data);
//
//     list_rpop(list);
//     list_foreach(list, log_data);
//
//     list_lpop(list);
//     list_foreach(list, log_data);
//
//     cr_assert_eq(2, list->size);
//     cr_assert_str_eq("node1", (char*)list_get(list, 1)->data);
//     cr_assert_str_eq("node2", (char*)list_get(list, 0)->data);
// }

static int compare(void *data1, void *data2)
{
    return strcmp((char*)data1, (char*)data2);
}

Test(suite_list, test_get_by_data) {
    list_t *list = list_new();
    // list_rpush(list, "node0");
    // list_rpush(list, "node1");
    // list_rpush(list, "node2");
    // list_foreach(list, log_data);
    //
    list_node_t *node = list_get__data(list, "node1", compare);
    cr_assert_eq(NULL, node);
}

// Test(suite_list, test_iter) {
//     list_t *list = list_new();
//     list_rpush(list, "node0");
//     list_rpush(list, "node1");
//     list_rpush(list, "node2");
//     list_rpush(list, "node3");
//     list_foreach(list, log_data);
//
//     list_iter_t *iter = list_iter(list);
//     cr_assert_eq(4, list->size);
//
//     cr_assert(list_iter_has_next(iter));
//     list_node_t *node0 = list_iter_next(iter);
//     cr_assert_str_eq("node0", (char*)node0->data);
//     cr_assert_eq(4, list->size);
//
//     cr_assert(list_iter_has_next(iter));
//     list_node_t *node1 = list_iter_next(iter);
//     cr_assert_str_eq("node1", (char*)node1->data);
//     cr_assert_eq(4, list->size);
//
//     logger__;
//     list_iter_remove(iter);
//     cr_assert_eq(3, list->size);
//
//     logger__;
//     cr_assert(list_iter_has_next(iter));
//     list_node_t *node2 = list_iter_next(iter);
//     cr_assert_str_eq("node2", (char*)node2->data);
//     cr_assert_eq(3, list->size);
//
//     list_foreach(list, log_data);
//     logger__;
// }
//
// Test(suite_list, test_iter_traverse) {
//     list_t *list = list_new();
//     list_rpush(list, "node0");
//     list_rpush(list, "node1");
//     list_rpush(list, "node2");
//     list_rpush(list, "node3");
//     list_foreach(list, log_data);
//
//     int size = list_size(list);
//     list_iter_t *iter = list_iter(list);
//     while (list_iter_has_next(iter)) {
//         list_node_t *node = list_iter_next(iter);
//         log_debug("============ %s", (char*)node->data);
//         list_iter_remove(iter);
//         list_foreach(list, log_data);
//         cr_assert_eq(--size, list_size(list));
//     }
//     list_iter_free(iter);
// }
