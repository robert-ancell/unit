#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtXmlElement] with [name], [attributes] and [content].
/// !arg-type attributes UtMap
/// !arg-type content UtList
/// !return-type UtXmlElement
/// !return-ref
UtObject *ut_xml_element_new(const char *name, UtObject *attributes,
                             UtObject *content);

/// Returns the name of [object].
const char *ut_xml_element_get_name(UtObject *object);

/// Returns the attributes of [object].
/// !return-type UtMap
UtObject *ut_xml_element_get_attributes(UtObject *object);

/// Returns the content of [object].
/// !return-type UtList
UtObject *ut_xml_element_get_content(UtObject *object);

/// Returns [true] if [object] is a [UtXmlElement].
bool ut_object_is_xml_element(UtObject *object);
