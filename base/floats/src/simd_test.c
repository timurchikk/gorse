#include "munit.h"
#include "math.h"

const size_t kVectorLength = 31;

/* AVX and FMA */

void _mm256_mul_const_add_to(float *a, float *b, float *c, int64_t n);
void _mm256_mul_const_to(float *a, float *b, float *c, int64_t n);
void _mm256_mul_const(float *a, float *b, int64_t n);
void _mm256_mul_to(float *a, float *b, float *c, int64_t n);
void _mm256_dot(float *a, float *b, int64_t n, float *ret);

/* Neon */

void vmul_const_add_to(float *a, float *b, float *c, int64_t n);
void vmul_const_to(float *a, float *b, float *c, int64_t n);
void vmul_const(float *a, float *b, int64_t n);
void vmul_to(float *a, float *b, float *c, int64_t n);
void vdot(float *a, float *b, int64_t n, float *ret);

/* no simd */

void mul_const_add_to(float *a, float *b, float *c, int64_t n)
{
  for (int64_t i = 0; i < n; i++)
  {
    c[i] += a[i] * (*b);
  }
}

void mul_const_to(float *a, float *b, float *c, int64_t n)
{
  for (int64_t i = 0; i < n; i++)
  {
    c[i] = a[i] * (*b);
  }
}

void mul_const(float *a, float *b, int64_t n)
{
  for (int64_t i = 0; i < n; i++)
  {
    a[i] *= *b;
  }
}

void mul_to(float *a, float *b, float *c, int64_t n)
{
  for (int64_t i = 0; i < n; i++)
  {
    c[i] = a[i] * b[i];
  }
}

void dot(float *a, float *b, int64_t n, float *ret)
{
  *ret = 0;
  for (int64_t i = 0; i < n; i++)
  {
    *ret += a[i] * b[i];
  }
}

int rand_float(float *a, int64_t n)
{
  for (int i = 0; i < n; i++)
  {
    a[i] = munit_rand_double();
  }
}

MunitResult mm256_mul_const_add_to_test(const MunitParameter params[], void *user_data_or_fixture)
{
  float a[kVectorLength], expect[kVectorLength], actual[kVectorLength];
  rand_float(a, kVectorLength);
  rand_float(expect, kVectorLength);
  memcpy(expect, actual, sizeof(float) * kVectorLength);
  float b = munit_rand_double();

  mul_const_add_to(a, &b, expect, kVectorLength);
  _mm256_mul_const_add_to(a, &b, actual, kVectorLength);
  munit_assert_floats_equal(kVectorLength, expect, actual);
  return MUNIT_OK;
}

MunitResult mm256_mul_const_to_test(const MunitParameter params[], void *user_data_or_fixture)
{
  float a[kVectorLength], expect[kVectorLength], actual[kVectorLength];
  rand_float(a, kVectorLength);
  float b = munit_rand_double();

  mul_const_to(a, &b, expect, kVectorLength);
  _mm256_mul_const_to(a, &b, actual, kVectorLength);
  munit_assert_floats_equal(kVectorLength, expect, actual);
  return MUNIT_OK;
}

MunitResult mm256_mul_const_test(const MunitParameter params[], void *user_data_or_fixture)
{
  float expect[kVectorLength], actual[kVectorLength];
  rand_float(expect, kVectorLength);
  memcpy(expect, actual, sizeof(float) * kVectorLength);
  float b = munit_rand_double();

  mul_const(expect, &b, kVectorLength);
  _mm256_mul_const(actual, &b, kVectorLength);
  munit_assert_floats_equal(kVectorLength, expect, actual);
  return MUNIT_OK;
}

MunitResult mm256_mul_to_test(const MunitParameter params[], void *user_data_or_fixture)
{
  float a[kVectorLength], b[kVectorLength], expect[kVectorLength], actual[kVectorLength];
  rand_float(a, kVectorLength);
  rand_float(b, kVectorLength);

  mul_to(a, b, expect, kVectorLength);
  _mm256_mul_to(a, b, actual, kVectorLength);
  munit_assert_floats_equal(kVectorLength, expect, actual);
  return MUNIT_OK;
}

MunitResult mm256_dot_test(const MunitParameter params[], void *user_data_or_fixture)
{
  float a[kVectorLength], b[kVectorLength], expect, actual;
  rand_float(a, kVectorLength);
  rand_float(b, kVectorLength);

  dot(a, b, kVectorLength, &expect);
  _mm256_dot(a, b, kVectorLength, &actual);
  munit_assert_float_equal(expect, actual, 6);
  return MUNIT_OK;
}

MunitTest tests[] = {
    {"mul_const_add_to", mm256_mul_const_add_to_test, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"mul_const_to", mm256_mul_const_to_test, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"mul_const", mm256_mul_const_test, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"mul_to", mm256_mul_to_test, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"dot", mm256_dot_test, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

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
