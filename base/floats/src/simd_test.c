#include "munit.h"

void _mm256_mul_const_add_to(float *a, float *b, float *c, int64_t n);
void _mm256_mul_const_to(float *a, float *b, float *c, int64_t n);
void _mm256_mul_const(float *a, float *b, int64_t n);
void _mm256_mul_to(float *a, float *b, float *c, int64_t n);
void _mm256_dot(float *a, float *b, int64_t n, float *ret);

void vmul_const_add_to(float *a, float *b, float *c, int64_t n);
void vmul_const_to(float *a, float *b, float *c, int64_t n);
void vmul_const(float *a, float *b, int64_t n);
void vmul_to(float *a, float *b, float *c, int64_t n);
void vdot(float *a, float *b, int64_t n, float *ret);

void dot(float *a, float *b, int64_t n, float *ret)
{
  *ret = 0;
  for (int64_t i = 0; i < n; i++)
  {
    *ret += a[i] * b[i];
  }
}

const size_t kLength = 31;

int rand_float(float *a, int64_t n)
{
  for (int i = 0; i < n; i++)
  {
    a[i] = munit_rand_int_range(1, 10);
  }
}

MunitResult mm256_dot_test(const MunitParameter params[], void *user_data_or_fixture)
{
  float a[kLength], b[kLength], expect, actual;
  rand_float(a, kLength);
  rand_float(b, kLength);
  dot(a, b, kLength, &expect);
  _mm256_dot(a, b, kLength, &actual);
  munit_assert_double_equal(expect, actual, 8);
  return MUNIT_OK;
}

MunitTest tests[] = {
    {
        "dot",                  /* name */
        mm256_dot_test,         /* test */
        NULL,                   /* setup */
        NULL,                   /* tear_down */
        MUNIT_TEST_OPTION_NONE, /* options */
        NULL                    /* parameters */
    },
    /* Mark the end of the array with an entry where the test
     * function is NULL */
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {
    "mm256_",               /* name */
    tests,                  /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

int main(int argc, char const *argv[])
{
  return munit_suite_main(&suite, NULL, argc, argv);
}
