/**
 * ********************************************************
 * LIST STRUCTURE
 * ********************************************************
 *
 *   --next-->
 *          0        1        2        3
 *   head ---> +-+ ---> +-+ ---> +-+ ---> NULL
 *             |0|      |1|      |2|
 *   NULL <--- +-+ <--- +-+ <--- +-+ <--- tail
 *        ^ -4  ^   -3       -2       -1
 *        | ^   |                    <--prev--
 *        | |   +node:list_node_t*
 *        | +index:int
 *        +cursor:list_node_t**
 *
 */
#ifndef __COMMON_LIST_H__
#define __COMMON_LIST_H__

#include <common/def.h>
#include <common/util.h>

typedef struct list_node_t {
    void *data;
    struct list_node_t *prev;
    struct list_node_t *next;
} list_node_t;

typedef struct list_t {
    struct list_node_t *head;
    struct list_node_t *tail;
    int size;
} list_t;

typedef struct list_iter_t {
    struct list_t *list;
    int neg_idx;
} list_iter_t;


typedef int (*list_node_compare_t)(void *, void *);
typedef int (*list_node_handler_t)(void *);

list_t *list_new();
int list_size(list_t *list);
void list_foreach(list_t *list, list_node_handler_t func);

void list_insert(list_t *list, int index, void *data);
void list_insert__clone(list_t *list, int index, void *data, int size);
list_node_t *list_get(list_t *list, int index);
list_node_t *list_get__data(list_t *list, void *data, list_node_compare_t compare);
list_node_t *list_remove(list_t *list, int index);
list_node_t *list_remove__node(list_t *list, list_node_t *node);
list_node_t *list_remove__data(list_t *list, void *data, list_node_compare_t compare);

list_t *list_concat(list_t *list1, list_t *list2);

void list_free(list_t *list);

void list_node_dump(list_node_t *node);
void list_node_free(list_node_t *node);

#define list_lpush(l, d)            list_insert(l,  0, d)
#define list_rpush(l, d)            list_insert(l, -1, d)
#define list_lpush__clone(l, d, s)  list_insert__clone(l,  0, d, s)
#define list_rpush__clone(l, d, s)  list_insert__clone(l, -1, d, s)
#define list_lpop(l)                list_remove(l,  0)
#define list_rpop(l)                list_remove(l, -1)

list_iter_t *list_iter(list_t *list);
list_node_t *list_iter_next(list_iter_t *iter);
void list_iter_remove(list_iter_t *iter);
bool list_iter_has_next(list_iter_t *iter);
void list_iter_free(list_iter_t *iter);

#endif//__COMMON_LIST_H__
