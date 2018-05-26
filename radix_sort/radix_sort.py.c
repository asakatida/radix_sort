#include <Python.h>

#include <math.h>
#include <stdbool.h>
#include <string.h>

typedef unsigned long long ull_t;

struct Item {
  ull_t key;
  size_t idx;
};

struct Bucket {
  struct Item *array;
  size_t size;
};

static void linear(struct Bucket *buckets, size_t size, struct Item *array) {
  for (size_t i = 0; i < size; ++i) {
    size_t _size = buckets[i].size;
    memcpy(array, buckets[i].array, _size * sizeof(struct Item));
    buckets[i].size = 0;
    array += _size;
  }
}

static void helper(struct Item *array, size_t size, ull_t radix, ull_t digit, struct Bucket *buckets) {
  for (size_t i = 0; i < size; ++i) {
    struct Item elem = array[i];
    ull_t bucket = (elem.key >> digit) & radix;
    buckets[bucket].array[buckets[bucket].size] = elem;
    buckets[bucket].size += 1;
  }
}

// Sort an array using a radix sort.
static void radix_sort(ull_t *array, size_t size, struct Item *sorted) {
  bool is_sorted = true;
  ull_t max_digit = 0;
  for (size_t i = 0; i < size; ++i) {
    ull_t elem = array[i];
    if (is_sorted && i > 0) {
      is_sorted = array[i - 1] <= elem;
    }
    if (max_digit < elem) {
      max_digit = elem;
    }
    sorted[i].idx = i;
    sorted[i].key = elem;
  }
  if (is_sorted) {
    return;
  }
  ull_t digit_count = (ull_t)log2(max_digit);
  digit_count /= 10;
  ull_t radix = 1;
  for (ull_t i = 1; i < digit_count; ++i) {
    radix |= 1 << i;
  }
  size_t bucket_size = ((size_t)1) << digit_count;
  struct Bucket *buckets = calloc(bucket_size, sizeof(struct Bucket));
  for (size_t i = 0; i < bucket_size; ++i) {
    buckets[i].array = calloc(size, sizeof(struct Item));
    buckets[i].size = 0;
  }
  memcpy(sorted, array, size);
  ull_t digit = 0;
  for (unsigned i = 0; i < 11; ++i) {
    helper(sorted, size, radix, digit, buckets);
    linear(buckets, bucket_size, sorted);
    digit += digit_count;
  }
  for (ull_t i = 0; i < radix; ++i) {
    free(buckets[i].array);
  }
  free(buckets);
}

static PyObject *radix_sort_py(PyObject *self, PyObject *args) {
  PyObject *array = NULL;
  if (!PyArg_ParseTuple(args, "O", &array)) {
    return NULL;
  }

  Py_ssize_t ssize = PySequence_Size(array);

  size_t size = (size_t)ssize;

  ull_t *_array = calloc(size, sizeof(struct Item));

  for (size_t i = 0; i < size; ++i) {
    _array[i] = PyLong_AsUnsignedLongLong(PySequence_GetItem(array, (Py_ssize_t)i));
  }

  // Py_BEGIN_ALLOW_THREADS;

  struct Item *sorted = calloc(size, sizeof(struct Item));
  radix_sort(_array, size, sorted);
  free(_array);

  // Py_END_ALLOW_THREADS;

  PyObject *output = PyList_New(size);

  for (size_t i = 0; i < size; ++i) {
    PyList_SET_ITEM(output, i, PySequence_GetItem(array, sorted[i].idx));
  }

  free(sorted);

  return output;
}

static PyMethodDef radix_sort_methods[] = {
  {"radix_sort", radix_sort_py, METH_VARARGS, "Sort an array using a radix sort."},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef radix_sort_module = {
  PyModuleDef_HEAD_INIT, "radix_sort", NULL, -1, radix_sort_methods
};

PyMODINIT_FUNC PyInit_radix_sort(void) {
  return PyModule_Create(&radix_sort_module);
}
