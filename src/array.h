#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>

#define array_push(array, item) (array_push) ((void **)&array, (item))

void *array_create (size_t capacity, size_t stride);
void array_destroy (void *array);

void (array_push) (void **array, const void *item);
size_t array_length (void *array);
size_t array_capacity (void *array);
size_t array_stride (void *array);

#endif // ARRAY_H
