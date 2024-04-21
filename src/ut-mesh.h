#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new mesh made of [triangles] that join [verticies].
///
/// !arg-type verticies UtFloat32List
/// !arg-type triangles UtUint16List
/// !return-ref
/// !return-type UtMesh
UtObject *ut_mesh_new(UtObject *verticies, UtObject *triangles);

/// Gets the verticies this mesh uses.
///
/// !return-type UtFloat32List
UtObject *ut_mesh_get_verticies(UtObject *object);

/// Gets the triangles that make up this mesh.
///
/// !return-type UtUint16List
UtObject *ut_mesh_get_triangles(UtObject *object);

/// Get the number of triangles in this mesh.
size_t ut_mesh_get_n_triangles(UtObject *object);

/// Returns the co-ordinates of the triangles with [index].
void ut_mesh_get_triangle(UtObject *object, size_t index, float *x0, float *y0,
                          float *x1, float *y1, float *x2, float *y2);

/// Returns [true] if [object] is a [UtMesh].
bool ut_object_is_mesh(UtObject *object);
