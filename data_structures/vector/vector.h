typedef struct vector_t vector_t;

typedef enum {
  VECTOR_OK = 0,
  VECTOR_OOM = 1,
  VECTOR_INVALID_ARGUMENT = 2,
  VECTOR_INDEX_OUT_OF_BOUNDS = 3,
} vector_status;

vector_status vector_init(vector_t *vector);

vector_status vector_push_back(vector_t *vector, int value);

vector_status vector_pop_back(vector_t *vector, int *out);

vector_status vector_get(vector_t *vector, size_t index, int *out);

vector_status vector_set(vector_t *vector, size_t index, int value);

void vector_free(vector_t *vector);
