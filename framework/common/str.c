#include <common/str.h>
#include <common/log.h>

static strbuf_t *strbuf(str_t str)
{
    return (strbuf_t *)(str - sizeof(strbuf_t));
}

str_t str_new()
{
    return str_new__len(DEFAULT_STRING_LEN);
}

str_t str_new__len(int len)
{
    strbuf_t *sb = NULL;
    if (len < DEFAULT_STRING_LEN) {
        len = DEFAULT_STRING_LEN;
    }

    sb = calloc(1, sizeof(strbuf_t) + len + 1);
    sb->capacity = len;
    sb->length = 0;
    // sb->buf[len] = '\0';

    return sb->buf;
}

str_t str_new__cstr(char *cstr)
{
    int cslen = strlen(cstr);
    str_t str = str_new__len(cslen);
    memcpy(str, cstr, cslen);
    str = str_resize(str, cslen);
    return str;
}

str_t str_new__cstr2(char *cstr, int len)
{
    int size = strlen(cstr);
    if (len < size) {
        size = len;
    }

    str_t str = str_new__len(size);
    memcpy(str, cstr, size);
    str = str_resize(str, size);

    return str;
}

str_t str_new__fmt(int maxlen, const char *fmt, ...)
{
    va_list argv;
    va_start(argv, fmt);

    str_t str = str_new__len(maxlen);
    vsnprintf(str, maxlen, fmt, argv);

    va_end(argv);

    strbuf(str)->length = strlen(str);
    return str;
}

str_t str_clone(str_t str)
{
    strbuf_t *sb = calloc(1, str_memsize(str));
    memcpy(sb, strbuf(str), str_memsize(str));
    return sb->buf;
}

void str_release(str_t str)
{
    free(strbuf(str));
}

int str_len(str_t str)
{
    return strbuf(str)->length;
}

int str_capacity(str_t str)
{
    return strbuf(str)->capacity;
}

int str_memsize(str_t str)
{
    return sizeof(strbuf_t) + str_capacity(str) + 1;
}

void str_erase(str_t str)
{
    strbuf(str)->length = 0;
    str[0] = '\0';
}

str_t str_trim(str_t str)
{
    #define TRIM_CHECK(c)       ((c)=='\0'||(c)=='\t'||(c)=='\n'||(c)=='\x0B'||(c)=='\r'||(c)==' ')

    char *pos = str;
    while (TRIM_CHECK(*pos)) {
        pos++;
    }

    str_t nwstr = str_new__len(str_len(str));
    char *nwpos = nwstr;
    while (!TRIM_CHECK(*pos)) {
        *nwpos++ = *pos++;
    }

    str_release(str);

    nwstr = str_resize(nwstr, strlen(nwstr));
    return nwstr;
}

int str_compare(str_t str, str_t str1)
{
    return memcmp(str, str1, str_len(str) + 1);
}

int str_equal(str_t str, str_t str1)
{
    return str_compare(str, str1) == 0;
}

char str_at(str_t str, int index)
{
    return *(str + index);
}

int str_char(str_t str, char c)
{
    return strchr(str, c) - str;
}

int str_substr(str_t str, char *sub)
{
    return strstr(str, sub) - str;
}

bool str_start_with(str_t str, char *sub)
{
    int len = strlen(sub);
    return memcmp(str, sub, len) == 0 ? true : false;
}

bool str_end_with(str_t str, char *sub)
{
    int len  = str_len(str);
    int len1 = strlen(sub);
    return memcmp(str + len - len1, sub, len1) == 0 ? true : false;
}

int str_count(str_t str, char *sub)
{
    if (!str || !sub) return -1;

    char *pos = str;
    int len = strlen(sub);
    if (len == 0) return 0;

    int i = 0;
    while ((pos = strstr(pos, sub))) {
      pos += len;
      i++;
    }

    return i;
}

str_t *str_split(str_t str, char *separator)
{
    if (!str || !separator) return NULL;

    str_t *ret = NULL;
    int seplen = strlen(separator);

    int num = str_count(str, separator);
    ret = calloc(1, sizeof(str_t) * (num + 2));

    char *start = str;
    char *end = NULL;
    for (int i = 0; i <= num; i++) {
        end = strstr(start, separator);
        if (!end) {
            end = str + str_len(str);
        }

        ret[i] = str_new__cstr2(start, end - start);
        start = end + seplen;
    }

    return ret;
}

void str_scan(str_t str, const char *fmt, ...)
{
    va_list argv;
    va_start(argv, fmt);
    vsscanf(str, fmt, argv);
    va_end(argv);
}

// void str_set_len(str_t str, int len)
// {
//     if (len < 0) {
//         len = strlen(str);
//     }
//     if (len > strbuf(str)->capacity) {
//         len = strbuf(str)->capacity;
//     }
//
//     str[len] = '\0';
//     strbuf(str)->length = len;
// }

str_t str_resize(str_t str, int size)
{
    if (size < 0) {
        return str;
    } else if (size > str_capacity(str)) {
        str = str_realloc(str, size);
    }

    str[size] = '\0';
    strbuf(str)->length = size;

    return str;
}

str_t str_realloc(str_t str, int size)
{
    if (str_capacity(str) < size) {
        str_t nwstr = str_new__len(size);
        memcpy(nwstr, str, str_len(str));
        nwstr[str_len(str)] = '\0';
        strbuf(nwstr)->length = str_len(str);

        str_release(str);
        str = nwstr;
        log_info("warnning: str has been realloced, new address: %p", nwstr);
    }

    return str;
}

str_t str_copy(str_t str, char *cstr)
{
    int cslen = strlen(cstr);

    str = str_resize(str, cslen);
    memcpy(str, cstr, cslen);
    // str_set_len(str, cslen);

    return str;
}

str_t _str_append(str_t str, char *cstr, ...)
{
    int len = str_len(str);
    int end = len;

    va_list argv;

    va_start(argv, cstr);
    for (char *cs = cstr; cs; cs = va_arg(argv, char *)) {
        len += strlen(cs);
    }
    va_end(argv);

    str = str_resize(str, len);

    va_start(argv, cstr);
    for (char *cs = cstr; cs; cs = va_arg(argv, char *)) {
        strcpy(str + end, cs);
        end += strlen(cs);
    }
    va_end(argv);

    // str_set_len(str, len);
    return str;
}

str_t str_prepend(str_t str, char *cstr)
{
    int s1 = str_len(str);
    int s2 = strlen(cstr);

    str_t nwstr = str_new__len(s1 + s2);
    memcpy(nwstr, cstr, s2);
    memcpy(nwstr + s2, str, s1);
    nwstr = str_resize(nwstr, s1 + s2);
    str_release(str);

    return nwstr;
}

str_t str_insert(str_t str, int index, char *cstr)
{
    if (index > str_len(str)) {
        log_debug("error: index > size of str");
        return NULL;
    }

    int len = str_len(str);
    int cslen = strlen(cstr);

    str_t nwstr = str_new__len(len + cslen);
    memcpy(nwstr, str, index);
    memcpy(nwstr + index, cstr, cslen);
    memcpy(nwstr + index + cslen, str + index, len - index);
    nwstr = str_resize(nwstr, len + cslen);
    str_release(str);

    return nwstr;
}

str_t str_replace(str_t str, char *sub, char *replace)
{
    int num = str_count(str, sub);
    str_t *splits = str_split(str, sub);
    int sublen = strlen(sub);
    int replen = strlen(replace);

    int size = str_len(str) + (replen - sublen) * num;

    str = str_resize(str, size);

    int pos = 0;
    str_t slice = splits[0];
    for (int i = 0; slice; slice = splits[++i]) {
        memcpy(str + pos, slice, str_len(slice));
        pos += str_len(slice);

        if (pos < size) {
            memcpy(str + pos, replace, replen);
            pos += replen;
        }

        str_release(slice);
    }

    // str_set_len(str, size);
    free(splits);
    return str;
}
