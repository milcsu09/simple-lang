#include "array.h"
#include <stdlib.h>
#include <string.h>

#define HEADER_SIZE (HEADER * sizeof (size_t))
#define HEADER_FIELD(array, index) (((size_t *)(array) - HEADER)[(index)])

enum
{
  LENGTH,
  CAPACITY,
  STRIDE,
  HEADER
};

void *
array_create (size_t capacity, size_t stride)
{
  size_t *chunk = malloc (HEADER_SIZE + capacity * stride);

  chunk[LENGTH] = 0;
  chunk[CAPACITY] = capacity;
  chunk[STRIDE] = stride;

  return (void *)(chunk + HEADER);
}

void
array_destroy (void *array)
{
  free ((char *)array - HEADER_SIZE);
}

void (array_push) (void **array, const void *item)
{
  size_t length = array_length (*array);
  size_t capacity = array_capacity (*array);
  size_t stride = array_stride (*array);

  if (length >= capacity)
    {
      void *new = array_create (capacity * 2, stride);

      memcpy (new, *array, length * stride);
      HEADER_FIELD (new, LENGTH) = length;

      array_destroy (*array);
      *array = new;
    }

  void *dest = *(char **)array + length * stride;

  memcpy (dest, item, stride);
  HEADER_FIELD (*array, LENGTH) = length + 1;
}

size_t
array_length (void *array)
{
  return HEADER_FIELD (array, LENGTH);
}

size_t
array_capacity (void *array)
{
  return HEADER_FIELD (array, CAPACITY);
}

size_t
array_stride (void *array)
{
  return HEADER_FIELD (array, STRIDE);
}

