#ifndef __COMMON_STR_H__
#define __COMMON_STR_H__

#include <common/def.h>
#include <common/util.h>

#define DEFAULT_STRING_LEN      64
typedef char *                  str_t;

typedef struct {
    unsigned int capacity;
    unsigned int length;
    char buf[0];
} strbuf_t;


str_t str_new();
str_t str_new__len(int len);
str_t str_new__cstr(char *cstr);
str_t str_new__cstr2(char *cstr, int size);
str_t str_new__fmt(int maxsize, const char *fmt, ...);

str_t str_clone(str_t str);
void str_release(str_t str);
int str_len(str_t str);
int str_capacity(str_t str);
int str_memsize(str_t str);

void str_erase(str_t str);
str_t str_trim(str_t str);
int str_compare(str_t str, str_t str1);
int str_equal(str_t str, str_t str1);

char str_at(str_t str, int index);
int str_char(str_t str, char c);
int str_substr(str_t str, char *sub);
bool str_start_with(str_t str, char *sub);
bool str_end_with(str_t str, char *sub);

int str_count(str_t str, char *sub);
str_t *str_split(str_t str, char *separator);
void str_scan(str_t str, const char *fmt, ...);

// void str_set_len(str_t str, int len);
str_t str_resize(str_t str, int size);                      //realloc
str_t str_realloc(str_t str, int size);                     //realloc

str_t _str_append(str_t str, char *cstr, ...);              //realloc
#define str_append(str, cstr, ...)          _str_append(str, cstr, ##__VA_ARGS__, NULL)

str_t str_prepend(str_t str, char *cstr);                   //realloc
str_t str_insert(str_t str, int index, char *cstr);         //realloc

str_t str_replace(str_t str, char *sub, char *rep);         //realloc


#endif//__COMMON_STR_H__
