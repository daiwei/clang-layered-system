#include <common/list.h>
#include <common/sys.h>
#include <common/log.h>

list_t *list_new()
{
    list_t *list = calloc(1, sizeof(list_t));
    return list;
}

int list_size(list_t *list)
{
    if (!list) {
        log_debug("arg is null");
        return -1;
    }

    return list->size;
}

static list_node_t **list_cursor(list_t *list, int index)
{
    if (!list) {
        log_debug("arg is null");
        return NULL;
    }

    list_node_t **cursor;

    if (index >= 0) {
        cursor = &(list->head);
        for (int i = 0; i < index; i++) {
            if (*cursor == NULL) {
                log_debug("error, out-of range. index: %d", index);
                return NULL;
            }
            cursor = &((*cursor)->next);
        }
    } else {
        cursor = &(list->tail);
        for (int i = -1; i > index; i--) {
            if (*cursor == NULL) {
                log_debug("error, out-of range. index: %d", index);
                return NULL;
            }
            cursor = &((*cursor)->prev);
        }
    }

    return cursor;
}

void list_foreach(list_t *list, list_node_handler_t func)
{
    if (!list) {
        log_debug("arg is null");
        return;
    }

    log_debug("list_foreach[%p], size[%d]:", list, list->size);
    for (list_node_t *node = list->head; node; node = node->next) {
        func(node->data);
    }
    log_debug("list_foreach: end");
}

void list_insert(list_t *list, int index, void *data)
{
    if (!list) {
        log_debug("arg is null");
        return;
    }

    list_node_t **cursor = list_cursor(list, index);
    if (!cursor) {
        return;
    }

    list_node_t *tmp = calloc(1, sizeof(list_node_t));
    tmp->data = data;

    if (*cursor) {
        // log_debug("cursor point to a node");
        if (index >= 0) {
            tmp->prev = (*cursor)->prev;
            tmp->next = (*cursor);
            (*cursor)->prev = tmp;
            (*cursor) = tmp;
        } else {
            tmp->next = (*cursor)->next;
            tmp->prev = (*cursor);
            (*cursor)->next = tmp;
            (*cursor) = tmp;
        }
    } else {
        // log_debug("cursor point to NULL");
        (*cursor) = tmp;
        if (index >= 0) {
            if (list->tail) {
                tmp->prev = list->tail;
                list->tail->next = tmp;
            }
            list->tail = tmp;
        } else {
            if (list->head) {
                tmp->next = list->head;
                list->head->prev = tmp;
            }
            list->head = tmp;
        }
    }

    // list_node_dump(list->head);

    list->size++;
    if (list->size > 1000) {
        log_debug("after inster, list size: %d (*log only size > 1000*)", list->size);
    }
}

void list_insert__clone(list_t *list, int index, void *data, int size)
{
    void *tmp = calloc(1, size);
    memcpy(tmp, data, size);
    list_insert(list, index, tmp);
}

list_node_t *list_get(list_t *list, int index)
{
    if (!list) {
        log_debug("arg is null");
        return NULL;
    }

    list_node_t **cursor = list_cursor(list, index);
    if (!cursor) {
        return NULL;
    }

    return *cursor;
}

list_node_t *list_get__data(list_t *list, void *data, list_node_compare_t compare)
{
    list_iter_t *iter = list_iter(list);
    while (list_iter_has_next(iter)) {
        list_node_t *node = list_iter_next(iter);
        if (compare(node->data, data) == 0) {
            list_iter_free(iter);
            return node;
        }
    }
    list_iter_free(iter);

    return NULL;
}

list_node_t *list_remove(list_t *list, int index)
{
    if (!list) {
        log_debug("arg is null");
        return NULL;
    }

    list_node_t **cursor = list_cursor(list, index);
    if (!cursor) {
        return NULL;
    }

    if (!*cursor) {
        return NULL;
    }

    list_node_t *ret = (*cursor);
    if (index >= 0) {
        if ((*cursor)->next) {
            (*cursor)->next->prev = (*cursor)->prev;
            (*cursor) = (*cursor)->next;
        } else {
            list->tail = (*cursor)->prev;
            (*cursor)->prev->next = NULL;
        }
    } else {
        if ((*cursor)->prev) {
            (*cursor)->prev->next = (*cursor)->next;
            (*cursor) = (*cursor)->prev;
        } else {
            list->head = (*cursor)->next;
            if ((*cursor)->next) (*cursor)->next->prev = NULL;
        }
    }
    list->size--;

    return ret;
}

list_node_t *list_remove__node(list_t *list, list_node_t *node)
{
    //useless
}

list_node_t *list_remove__data(list_t *list, void *data, list_node_compare_t compare)
{
    list_node_t *node = NULL;
    list_iter_t *iter = list_iter(list);
    while (list_iter_has_next(iter)) {
        node = list_iter_next(iter);
        if (compare(node->data, data) == 0) {
            list_iter_remove(iter);
            break;
        }
    }
    list_iter_free(iter);
    return node;
}

list_t *list_concat(list_t *list1, list_t *list2)
{
    if (!list1 || !list2) {
        log_debug("arg is null");
        return NULL;
    }

    list1->tail->next = list2->head;
    list2->head->prev = list1->tail->next;
    list1->tail = list2->tail;

    list_free(list2);

    return list1;
}

void list_free(list_t *list)
{
    if (!list) {
        log_debug("arg is null");
        return;
    }

    list_iter_t *iter = list_iter(list);
    while (list_iter_has_next(iter)) {
        list_node_free(list_iter_next(iter));
        list_iter_remove(iter);
    }
    list_iter_free(iter);

    free(list);
}

void list_node_dump(list_node_t *node)
{
    log_debug("list node[%p]:", node);
    if (node) {
        log_debug("  data: %p", node->data);
        log_debug("  next: %p", node->next);
        log_debug("  prev: %p", node->prev);
    }
}

void list_node_free(list_node_t *node)
{
    if (!node) {
        log_debug("arg is null");
        return;
    }
    // if `node->data` points to constant, will chrash
    free(node->data);
    node->data = NULL;
    free(node);
}


/**
 * iterator for list
 */
list_iter_t *list_iter(list_t *list)
{
    if (!list) {
        log_debug("arg is null");
        return NULL;
    }

    list_iter_t *iter = calloc(1, sizeof(list_iter_t));
    iter->list = list;
    iter->neg_idx = -1 * list_size(list) - 1;
    return iter;
}

bool list_iter_has_next(list_iter_t *iter)
{
    if (!iter || !iter->list || !list_size(iter->list)) {
        return false;
    }

    list_node_t *node = list_get(iter->list, iter->neg_idx);
    if (!node) {
        return true;
    }

    return node->next ? true : false;
}

list_node_t *list_iter_next(list_iter_t *iter)
{
    if (!iter) {
        log_debug("arg is null");
        return NULL;
    }

    iter->neg_idx++;
    return list_get(iter->list, iter->neg_idx);
}

void list_iter_prepend(list_iter_t *iter, void *data)
{
    list_insert(iter->list, iter->neg_idx - 1, data);
}

void list_iter_append(list_iter_t *iter, void *data)
{
    list_insert(iter->list, iter->neg_idx, data);
}

void list_iter_remove(list_iter_t *iter)
{
    if (!iter) {
        log_debug("arg is null");
        return;
    }

    list_remove(iter->list, iter->neg_idx);
}

void list_iter_free(list_iter_t *iter)
{
    free(iter);
}
