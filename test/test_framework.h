#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stddef.h>
#include <string.h>

typedef void (*test_fn)(void);

typedef struct {
    const char* name;
    test_fn fn;
} test_case;

typedef struct {
    test_case cases[128];
    size_t count;
} test_suite;

void test_suite_init(test_suite* suite);
void test_suite_add(test_suite* suite, const char* name, test_fn fn);
int  test_suite_run(test_suite* suite);
void test_mark_failure(const char* file, int line, const char* expr, const char* message, ...);

#define ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            test_mark_failure(__FILE__, __LINE__, #expr, "expected expression to be true"); \
            return; \
        } \
    } while (0)

#define ASSERT_FALSE(expr) \
    do { \
        if ((expr)) { \
            test_mark_failure(__FILE__, __LINE__, #expr, "expected expression to be false"); \
            return; \
        } \
    } while (0)

#define ASSERT_EQ_INT(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            test_mark_failure(__FILE__, __LINE__, #actual, "expected %d, got %d", (int)(expected), (int)(actual)); \
            return; \
        } \
    } while (0)

#define ASSERT_EQ_SIZE(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            test_mark_failure(__FILE__, __LINE__, #actual, "expected %zu, got %zu", (size_t)(expected), (size_t)(actual)); \
            return; \
        } \
    } while (0)

#define ASSERT_PTR_EQ(expected, actual) \
    do { \
        if ((void*)(expected) != (void*)(actual)) { \
            test_mark_failure(__FILE__, __LINE__, #actual, "expected pointer %p, got %p", (void*)(expected), (void*)(actual)); \
            return; \
        } \
    } while (0)

#define ASSERT_MEM_EQ(expected, actual, len) \
    do { \
        if (memcmp((expected), (actual), (len)) != 0) { \
            test_mark_failure(__FILE__, __LINE__, #actual, "memory comparison failed"); \
            return; \
        } \
    } while (0)

#define ASSERT_STR_EQ(expected, actual) \
    do { \
        if (strcmp((expected), (actual)) != 0) { \
            test_mark_failure(__FILE__, __LINE__, #actual, "expected \"%s\", got \"%s\"", (expected), (actual)); \
            return; \
        } \
    } while (0)

#endif /* TEST_FRAMEWORK_H */
