#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *module;
  UtObject *symbols;
} UtAsn1ModuleImports;

static void ut_asn1_module_imports_cleanup(UtObject *object) {
  UtAsn1ModuleImports *self = (UtAsn1ModuleImports *)object;
  free(self->module);
  ut_object_unref(self->symbols);
}

static UtObjectInterface object_interface = {
    .type_name = "UtAsn1ModuleImports",
    .cleanup = ut_asn1_module_imports_cleanup};

UtObject *ut_asn1_module_imports_new(const char *module, UtObject *symbols) {
  UtObject *object =
      ut_object_new(sizeof(UtAsn1ModuleImports), &object_interface);
  UtAsn1ModuleImports *self = (UtAsn1ModuleImports *)object;

  self->module = ut_cstring_new(module);
  self->symbols =
      symbols != NULL ? ut_object_ref(symbols) : ut_object_list_new();

  return object;
}

const char *ut_asn1_module_imports_get_module(UtObject *object) {
  assert(ut_object_is_asn1_module_imports(object));
  UtAsn1ModuleImports *self = (UtAsn1ModuleImports *)object;
  return self->module;
}

UtObject *ut_asn1_module_imports_get_symbols(UtObject *object) {
  assert(ut_object_is_asn1_module_imports(object));
  UtAsn1ModuleImports *self = (UtAsn1ModuleImports *)object;
  return self->symbols;
}

bool ut_object_is_asn1_module_imports(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
