#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtXmlDocument] with the given [root] element.
///
/// !return-type UtXmlDocument
/// !return-ref
UtObject *ut_xml_document_new(UtObject *root);

/// Creates a new [UtXmlDocument] from the given XML [text].
///
/// !return-type UtXmlDocument
/// !return-ref
UtObject *ut_xml_document_new_from_text(const char *text);

/// Returns the root element of [object].
/// !return-type UtXmlElement
UtObject *ut_xml_document_get_root(UtObject *object);

/// Creates an XML representation of [object].
/// !return-ref
char *ut_xml_document_to_text(UtObject *object);

/// Returns [true] if [object] is a [UtXmlDocument].
bool ut_object_is_xml_document(UtObject *object);
