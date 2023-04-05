#include "ut.h"

static bool initialized = false;
static bool cleaned_up = false;

typedef struct {
  UtObject object;
  uint32_t value;
} TestObject;

static void test_object_init(UtObject *object) { initialized = true; }

static char *test_object_to_string(UtObject *object) {
  TestObject *self = (TestObject *)object;
  return ut_cstring_new_printf("<TestObject>(%u)", self->value);
}

static bool test_object_equal(UtObject *object, UtObject *other) {
  TestObject *self = (TestObject *)object;
  TestObject *other_self = (TestObject *)other;
  return self->value == other_self->value;
}

static int test_object_hash(UtObject *object) { return 12345678; }

static void test_object_cleanup(UtObject *object) { cleaned_up = true; }

static UtObjectInterface test_object_interface = {
    .type_name = "TestObject",
    .init = test_object_init,
    .to_string = test_object_to_string,
    .equal = test_object_equal,
    .hash = test_object_hash,
    .cleanup = test_object_cleanup};

static UtObject *test_object_new(uint32_t value) {
  UtObject *object = ut_object_new(sizeof(TestObject), &test_object_interface);
  TestObject *self = (TestObject *)object;
  self->value = value;
  return object;
}

static uint32_t ut_object_get_value(UtObject *object) {
  TestObject *self = (TestObject *)object;
  return self->value;
}

int main(int argc, char **argv) {
  ut_assert_false(initialized);
  UtObject *object = test_object_new(42);
  ut_assert_true(initialized);

  ut_assert_true(ut_object_is_type(object, &test_object_interface));

  ut_assert_int_equal(ut_object_get_value(object), 42);

  ut_assert_cstring_equal(ut_object_get_type_name(object), "TestObject");

  ut_cstring_ref text = ut_object_to_string(object);
  ut_assert_cstring_equal(text, "<TestObject>(42)");

  UtObjectRef object2 = test_object_new(99);
  UtObjectRef object3 = test_object_new(42);
  ut_assert_true(ut_object_equal(object, object));
  ut_assert_false(ut_object_equal(object, object2));
  ut_assert_true(ut_object_equal(object, object3));

  ut_assert_int_equal(ut_object_get_hash(object), 12345678);

  ut_assert_false(cleaned_up);
  ut_object_unref(object);
  ut_assert_true(cleaned_up);

  UtObjectRef object4 = test_object_new(444);
  UtObjectRef object5 = test_object_new(555);
  UtObject *o = NULL;
  ut_object_set(&o, object4);
  ut_assert_int_equal(ut_object_get_value(o), 444);
  ut_object_set(&o, object5);
  ut_assert_int_equal(ut_object_get_value(o), 555);
  ut_object_clear(&o);
  ut_assert_null(o);

  return 0;
}
