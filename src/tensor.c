/*
 * tensor.c
 * QG8 base library tensor source.
 *
 * Author       : Finn Rayment <finn@rayment.fr>
 * Date created : 01/05/2021
 */

/*
 * Copyright 2021 University of Strasbourg
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <string.h>

#include "macros.h"
#include "qg8.h"

#define CREATE_FUNC(x,y,z) \
qg8_tensor * \
qg8_tensor_create_##x(uint64_t **indices, \
                        y *re, \
                        uint64_t length, \
                        uint64_t *shape, \
                        uint8_t rank, \
                        uint8_t packing) \
{ \
	qg8_tensor *t; \
\
	t = (qg8_tensor *) malloc(sizeof(qg8_tensor)); \
	ALLOC(t); \
	_common_create(t, re, indices, length, shape, rank, packing); \
\
	t->redata = re; \
	t->dtype_id = z; \
\
	return t; \
}

static
int
_num_bytes(uint64_t val)
{
	if (val < (1<<8))
		return QG8_DTYPE_UINT8;
	if (val < (1<<16))
		return QG8_DTYPE_UINT16;
	if (val < ((uint64_t) 1<<32))
		return QG8_DTYPE_UINT32;
	return QG8_DTYPE_UINT64;
}

static
int
_tensor_index_size(qg8_tensor *t)
{
	uint64_t max;
	int i;

	max = 0;
	for (i = 0; i < t->rank; ++i)
	{
		if (max == 0 || *(t->dimensions+i) > max)
			max = *(t->dimensions+i);
	}
	return _num_bytes(max);
}

static
void
_common_create(qg8_tensor *t,
               void *re,
               uint64_t **indices,
               uint64_t length,
               uint64_t *shape,
               uint8_t rank,
               uint8_t packing)
{
	size_t i;
	t->loaded = 0;
	if (!indices)
	{
		DIE("Cannot create tensor with NULL indices.\n");
	}
	if (!re)
	{
		DIE("Cannot create tensor with NULL real component.\n");
	}
	if (!shape)
	{
		DIE("Cannot create tensor with NULL dimensions.\n");
	}
	if (rank < 1)
	{
		DIE("Cannot create tensor with rank 0.\n");
	}
	for (i = 0; i < rank; ++i)
	{
		if (*(shape+i) == 0)
		{
			DIE("Cannot create tensor with 0 dimension.\n");
		}
	}
	t->rank = rank;
	t->dimensions = shape;
	t->num_elems = length;
	t->indices = indices;
	t->packing = packing;
	t->itype_id = _tensor_index_size(t);
	t->redata = NULL;
	t->imdata = NULL;
}

qg8_tensor *
qg8_tensor_create_float(uint64_t **indices,
                        float *re,
                        float *im,
                        uint64_t length,
                        uint64_t *shape,
                        uint8_t rank,
                        uint8_t packing)
{
	qg8_tensor *t;

	t = (qg8_tensor *) malloc(sizeof(qg8_tensor));
	ALLOC(t);
	_common_create(t, re, indices, length, shape, rank, packing);

	t->redata = re;
	if (im)
	{
		t->imdata = im;
		t->dtype_id = QG8_DTYPE_COMPLEX64;
	}
	else
	{
		t->dtype_id = QG8_DTYPE_FLOAT32;
	}

	return t;
}

qg8_tensor *
qg8_tensor_create_double(uint64_t **indices,
                         double *re,
                         double *im,
                         uint64_t length,
                         uint64_t *shape,
                         uint8_t rank,
                         uint8_t packing)
{
	qg8_tensor *t;

	t = (qg8_tensor *) malloc(sizeof(qg8_tensor));
	ALLOC(t);
	_common_create(t, re, indices, length, shape, rank, packing);

	t->redata = re;
	if (im)
	{
		t->imdata = im;
		t->dtype_id = QG8_DTYPE_COMPLEX128;
	}
	else
	{
		t->dtype_id = QG8_DTYPE_FLOAT64;
	}

	return t;
}

/* fat macro expander */
CREATE_FUNC(uint8, uint8_t, QG8_DTYPE_UINT8)
CREATE_FUNC(uint16, uint16_t, QG8_DTYPE_UINT16)
CREATE_FUNC(uint32, uint32_t, QG8_DTYPE_UINT32)
CREATE_FUNC(uint64, uint64_t, QG8_DTYPE_UINT64)
CREATE_FUNC(int8, int8_t, QG8_DTYPE_INT8)
CREATE_FUNC(int16, int16_t, QG8_DTYPE_INT16)
CREATE_FUNC(int32, int32_t, QG8_DTYPE_INT32)
CREATE_FUNC(int64, int64_t, QG8_DTYPE_INT64)

int
qg8_tensor_destroy(qg8_tensor *t)
{
	size_t i;

	if (!t)
	{
		DIE("Cannot destroy a NULL tensor.\n");
	}

	if (t->loaded)
	{
		free(t->dimensions);
		for (i = 0; i < t->rank; ++i)
			free(*(t->indices+i));
		free(t->indices);
		if (t->redata)
			free(t->redata);
		if (t->imdata)
			free(t->imdata);
	}
	free(t);
	return 1;
}

uint16_t
qg8_tensor_get_rank(qg8_tensor *t)
{
	if (!t)
	{
		DIE("Cannot get rank from a NULL tensor.\n");
	}
	return t->rank;
}

uint64_t **
qg8_tensor_get_indices(qg8_tensor *t)
{
	if (!t)
	{
		DIE("Cannot get indices from a NULL tensor.\n");
	}
	return t->indices;
}

uint64_t
qg8_tensor_get_num_elems(qg8_tensor *t)
{
	if (!t)
	{
		DIE("Cannot get number of elements from a NULL tensor.\n");
	}
	return t->num_elems;
}

uint8_t
qg8_tensor_get_dtypeid(qg8_tensor *t)
{
	if (!t)
	{
		DIE("Cannot get data type from a NULL tensor.\n");
	}
	return t->dtype_id;
}

uint8_t
qg8_tensor_get_itypeid(qg8_tensor *t)
{
	if (!t)
	{
		DIE("Cannot get index type from a NULL tensor.\n");
	}
	return t->itype_id;
}

void *
qg8_tensor_get_dims(qg8_tensor *t)
{
	if (!t)
	{
		DIE("Cannot get dimensions from a NULL tensor.\n");
	}
	return t->dimensions;
}

void *
qg8_tensor_get_re(qg8_tensor *t)
{
	if (!t)
	{
		DIE("Cannot get real data from a NULL tensor.\n");
	}
	switch (t->dtype_id)
	{
	case QG8_DTYPE_FLOAT64: /* fall-through */
	case QG8_DTYPE_COMPLEX128:
	case QG8_DTYPE_FLOAT32:
	case QG8_DTYPE_COMPLEX64:
	case QG8_DTYPE_UINT8:
	case QG8_DTYPE_UINT16:
	case QG8_DTYPE_UINT32:
	case QG8_DTYPE_UINT64:
	case QG8_DTYPE_INT8:
	case QG8_DTYPE_INT16:
	case QG8_DTYPE_INT32:
	case QG8_DTYPE_INT64:
		return t->redata;
	default:
		return NULL;
	}
}

void *
qg8_tensor_get_im(qg8_tensor *t)
{
	if (!t)
	{
		DIE("Cannot get imaginary data from a NULL tensor.\n");
	}
	switch (t->dtype_id)
	{
	case QG8_DTYPE_COMPLEX64:
	case QG8_DTYPE_COMPLEX128:
		return t->imdata;
	default:
		return NULL;
	}
}

