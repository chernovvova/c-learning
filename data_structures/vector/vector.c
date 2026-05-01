#include "vector.h"
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>

#define RESIZE_CONSTANT 2

typedef struct vector_t {
  int *data;
  size_t size;
  size_t capacity;
} vector_t;

vector_status vector_init(vector_t *vector) {
  if (vector == NULL) {
    return VECTOR_INVALID_ARGUMENT;
  }
  vector->data = NULL;
  vector->size = 0;
  vector->capacity = 0;

  return VECTOR_OK;
}

vector_status vector_resize(vector_t *vector) {
  if (vector == NULL) {
    return VECTOR_INVALID_ARGUMENT;
  }
  size_t capacity = vector->capacity;
  if (capacity == 0) {
    capacity = 1;
  } else {
    capacity *= RESIZE_CONSTANT;
  }

  if (capacity > SIZE_MAX / sizeof(int)) {
    return VECTOR_OOM;
  }

  int *tmp = realloc(vector->data, capacity * sizeof(int));
  if (tmp == NULL) {
    return VECTOR_OOM;
  }

  vector->capacity = capacity;
  vector->data = tmp;

  return VECTOR_OK;
}

vector_status vector_push_back(vector_t *vector, int value) {
  if (vector == NULL) {
    return VECTOR_INVALID_ARGUMENT;
  }

  if (vector->size >= vector->capacity) {
    vector_status resize_status = vector_resize(vector);
    if (resize_status != VECTOR_OK) {
      return resize_status;
    }
  }

  vector->data[vector->size++] = value;

  return VECTOR_OK;
}

vector_status vector_get(vector_t *vector, size_t index, int *out) {
  if (vector == NULL || out == NULL) {
    return VECTOR_INVALID_ARGUMENT;
  }

  if (index >= vector->size) {
    return VECTOR_INDEX_OUT_OF_BOUNDS;
  }

  *out = vector->data[index];

  return VECTOR_OK;
}

vector_status vector_pop_back(vector_t *vector, int *out) {
  if (vector == NULL || out == NULL) {
    return VECTOR_INVALID_ARGUMENT;
  }

  if (vector->size == 0) {
    return VECTOR_INDEX_OUT_OF_BOUNDS;
  }

  *out = vector->data[--vector->size];

  return VECTOR_OK;
}

vector_status vector_set(vector_t *vector, size_t index, int value) {
  if (vector == NULL) {
    return VECTOR_INVALID_ARGUMENT;
  }

  if (index >= vector->size) {
    return VECTOR_INDEX_OUT_OF_BOUNDS;
  }

  vector->data[index] = value;

  return VECTOR_OK;
}

vector_status vector_insert(vector_t *vector, size_t index, int value) {
  if (vector == NULL) {
    return VECTOR_INVALID_ARGUMENT;
  }

  if (index > vector->size) {
    return VECTOR_INDEX_OUT_OF_BOUNDS;
  }

  if (vector->size >= vector->capacity) {
    vector_status resize_status = vector_resize(vector);
    if (resize_status != VECTOR_OK) {
      return resize_status;
    }
  }

  for (size_t i = vector->size; i > index; i--) {
    vector->data[i] = vector->data[i - 1];
  }
  vector->data[index] = value;

  vector->size += 1;

  return VECTOR_OK;
}

void vector_free(vector_t *vector) {
  if (vector == NULL) {
    return;
  }
  free(vector->data);
  vector->capacity = 0;
  vector->size = 0;
  vector->data = NULL;
}
