#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new XML document with the given [root] element.
///
/// !arg-type root UtXmlElement
/// !return-ref
/// !return-type UtXmlDocument
UtObject *ut_xml_document_new(UtObject *root);

/// Creates a new XML document from the given XML [text].
///
/// !return-ref
/// !return-type UtXmlDocument
UtObject *ut_xml_document_new_from_text(const char *text);

/// Returns the root element of this document.
///
/// !return-type UtXmlElement
UtObject *ut_xml_document_get_root(UtObject *object);

/// Creates an XML representation of this document.
///
/// !return-ref
char *ut_xml_document_to_text(UtObject *object);

/// Returns [true] if [object] is a [UtXmlDocument].
bool ut_object_is_xml_document(UtObject *object);
