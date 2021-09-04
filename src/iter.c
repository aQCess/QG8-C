/*
 * iter.c
 * QG8 base library iterator source.
 *
 * Author       : Finn Rayment <finn@rayment.fr>
 * Date created : 15/05/2021
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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "macros.h"
#include "qg8.h"

SHAPER(32_64, uint64_t, uint32_t)
SHAPER(16_64, uint64_t, uint16_t)
SHAPER(8_64,  uint64_t, uint8_t)

static
void
_load_indices(qg8_tensor *t,
              FILE *f,
              qg8_iter *iter)
{
	uint8_t  *u8;
	uint16_t *u16;
	uint32_t *u32;
	uint64_t *u64;
	size_t i;

	for (i = 0; i < t->rank; ++i)
	{
		if (t->itype_id == QG8_DTYPE_UINT8)
		{
			u8 = (uint8_t *) malloc(sizeof(uint8_t) * t->num_elems);
			ALLOC(u8);
			READNN(u8, sizeof(uint8_t), t->num_elems, f);
			*(t->indices+i) = _shape8_64(u8, t->num_elems);
			iter->offset += sizeof(uint8_t) * t->num_elems;
			free(u8);
		}
		else if (t->itype_id == QG8_DTYPE_UINT16)
		{
			u16 = (uint16_t *) malloc(sizeof(uint16_t) * t->num_elems);
			ALLOC(u16);
			READNN(u16, sizeof(uint16_t), t->num_elems, f);
			*(t->indices+i) = _shape16_64(u16, t->num_elems);
			iter->offset += sizeof(uint16_t) * t->num_elems;
			free(u16);
		}
		else if (t->itype_id == QG8_DTYPE_UINT32)
		{
			u32 = (uint32_t *) malloc(sizeof(uint32_t) * t->num_elems);
			ALLOC(u32);
			READNN(u32, sizeof(uint32_t), t->num_elems, f);
			*(t->indices+i) = _shape32_64(u32, t->num_elems);
			iter->offset += sizeof(uint32_t) * t->num_elems;
			free(u32);
		}
		else
		{
			u64 = (uint64_t *) malloc(sizeof(uint64_t) * t->num_elems);
			ALLOC(u64);
			READNN(u64, sizeof(uint64_t), t->num_elems, f);
			iter->offset += sizeof(uint64_t) * t->num_elems;
			*(t->indices+i) = u64;
		}
	}
}

qg8_iter
qg8_file_iterator(qg8_file *qg8f)
{
	qg8_iter i;

	if (!qg8f)
	{
		DIE("Cannot get iterator for NULL file.\n");
	}

	if (qg8f->mode != QG8_MODE_READ)
	{
		DIE("Cannot get iterator for file open in write mode.\n");
	}

	i.offset = sizeof(qg8_file_header);
	i.f = qg8f;
	i.done_read = 1;
	return i;
}

int
qg8_file_has_next(qg8_iter *i)
{
	if (!i)
	{
		DIE("Cannot iterate over file with a NULL iterator.\n");
	}
	/* TODO: dead/redundant error? */
	if (i->f->mode != QG8_MODE_READ)
	{
		DIE("Cannot iterate over file open in write mode.\n");
	}

	fseek(i->f->fp, 0, SEEK_END);
	i->done_read = 0;
	return i->offset < (size_t) ftell(i->f->fp);
}

int
qg8_file_next(qg8_iter *i)
{
	uint16_t type;
	uint8_t flags;
	uint8_t id[16], res[6];
	uint64_t skip;

	/* this call handles the error checks */
	if (!qg8_file_has_next(i))
		return 0;
	/*if (i->done_read)
	{
		i->done_read = 0;
		return 1;
	}*/
	i->done_read = 0;
	fseek(i->f->fp, i->offset, SEEK_SET);
	READ(&type, type, i->f->fp);
	READ(&flags, flags, i->f->fp);
	if ((flags & QG8_FLAG_LABEL) == QG8_FLAG_LABEL)
		READN(id, 16, i->f->fp);
	READN(res, 5, i->f->fp);
	READ(&skip, skip, i->f->fp);
	i->offset += skip + sizeof(qg8_chunk_header);
	/*((5 + 16 + 1) * sizeof(uint8_t)) + sizeof(uint16_t) + sizeof(uint64_t);*/
	return 1;
}

qg8_chunk *
qg8_file_extract(qg8_iter *iter)
{
	qg8_chunk *chunk;
	qg8_tensor *t;
	uint16_t u16;
	uint8_t u8buf[16], tmp;
	uint64_t u64;
	size_t i;

	/* return nothing on no iterator */
	if (!iter)
	{
		DIE("Cannot iterate over file with a NULL iterator.\n");
	}
	/* TODO: dead/redundant error? */
	if (iter->f->mode != QG8_MODE_READ)
	{
		DIE("Cannot iterate over file open in write mode.\n");
	}

	fseek(iter->f->fp, iter->offset, SEEK_SET);
	if (feof(iter->f->fp))
	{
		DIE("Cannot extract chunk due to unhandled EOF.\n");
	}
	chunk = (qg8_chunk *) malloc(sizeof(qg8_chunk));
	ALLOC(chunk);
	chunk->tensor = NULL;
	/* chunk header */
	READ(&chunk->type, chunk->type, iter->f->fp);
	READ(&chunk->flags, chunk->flags, iter->f->fp);
#ifdef DEBUG
	printf("-- chunk --\n");
	printf("type: %d\n", chunk->type);
	printf("flags: %d\n", chunk->flags);
#endif /* DEBUG */
	if ((chunk->flags & QG8_FLAG_LABEL) == QG8_FLAG_LABEL)
	{
		READN(chunk->string_id, 16, iter->f->fp);
		iter->offset += 16;
#ifdef DEBUG
		printf("id: '%.*s'\n", 16, chunk->string_id);
#endif /* DEBUG */
	}
	else
	{
		memset(chunk->string_id, 0, 16);
	}
	READN(u8buf, 5, iter->f->fp);
	READ(&u64, u64, iter->f->fp);
#ifdef DEBUG
	printf("skip: %lu\n", u64);
#endif /* DEBUG */
	/* type (u16), reserved + flags (6), skip (u64) */
	iter->offset += sizeof(u16) + 1 + 5 + sizeof(u64);
	/* if skip is 0, there's no tensor because the chunk is done */
	if (u64 > 0)
	{
		/* tensor header */
		t = (qg8_tensor *) malloc(sizeof(qg8_tensor));
		t->loaded = 1;
		ALLOC(t);
		READ(&t->packing, t->packing, iter->f->fp);
		READ(&t->itype_id, t->itype_id, iter->f->fp);
		READ(&t->dtype_id, t->dtype_id, iter->f->fp);
		READ(&t->rank, t->rank, iter->f->fp);
#ifdef DEBUG
		printf("-- tensor --\n");
		printf("packing: %d\n", t->packing);
		printf("itype: %d\n", t->itype_id);
		printf("dtype_id: %d\n", t->dtype_id);
		printf("rank: %u\n", t->rank);
#endif /* DEBUG */
		iter->offset += sizeof(t->packing) + sizeof(t->itype_id) +
		                sizeof(t->dtype_id) + sizeof(t->rank);
		READN(u8buf, 3, iter->f->fp);
		iter->offset += 3;
		t->dimensions = (uint64_t *) malloc(sizeof(uint64_t) * t->rank);
		ALLOC(t->dimensions);
		t->indices = (uint64_t **) malloc(sizeof(uint64_t *) * t->rank);
		ALLOC(t->indices);
		tmp = _type_to_size(t->itype_id);
		for (i = 0; i < t->rank; ++i)
		{
			*(t->dimensions+i) = 0;
			READN(t->dimensions+i, tmp, iter->f->fp);
#ifdef DEBUG
			printf("dimension %lu: %lu\n", i, *(t->dimensions+i));
#endif /* DEBUG */
			iter->offset += tmp;
		}
		READ(&t->num_elems, t->num_elems, iter->f->fp);
#ifdef DEBUG
		printf("num_elems: %lu\n", t->num_elems);
#endif /* DEBUG */
		iter->offset += sizeof(t->num_elems);
		/* tensor data */
		_load_indices(t, iter->f->fp, iter);
		t->refloat = NULL;
		t->imfloat = NULL;
		t->redouble = NULL;
		t->imdouble = NULL;
		t->reu8 = NULL;
		t->reu16 = NULL;
		t->reu32 = NULL;
		t->reu64 = NULL;
		t->rei8 = NULL;
		t->rei16 = NULL;
		t->rei32 = NULL;
		t->rei64 = NULL;
		if (t->dtype_id == QG8_DTYPE_FLOAT64 ||
		    t->dtype_id == QG8_DTYPE_COMPLEX128)
		{
			t->redouble = (double *) malloc(sizeof(double) * t->num_elems);
			ALLOC(t->redouble);
			READNN(t->redouble, sizeof(double), t->num_elems, iter->f->fp);
			iter->offset += sizeof(double) * t->num_elems;
			if (t->dtype_id == QG8_DTYPE_COMPLEX128)
			{
				t->imdouble = (double *) malloc(sizeof(double) * t->num_elems);
				ALLOC(t->imdouble);
				READNN(t->imdouble, sizeof(double), t->num_elems, iter->f->fp);
				iter->offset += sizeof(double) * t->num_elems;
			}
		}
		else if (t->dtype_id == QG8_DTYPE_FLOAT32 ||
		         t->dtype_id == QG8_DTYPE_COMPLEX128)
		{
			t->refloat = (float *) malloc(sizeof(float) * t->num_elems);
			ALLOC(t->refloat);
			READNN(t->refloat, sizeof(float), t->num_elems, iter->f->fp);
			iter->offset += sizeof(float) * t->num_elems;
			if (t->dtype_id == QG8_DTYPE_COMPLEX64)
			{
				t->imfloat = (float *) malloc(sizeof(float) * t->num_elems);
				ALLOC(t->imfloat);
				READNN(t->imfloat, sizeof(float), t->num_elems, iter->f->fp);
				iter->offset += sizeof(float) * t->num_elems;
			}
		}
		else if (t->dtype_id == QG8_DTYPE_UINT8)
		{
			t->reu8 = (uint8_t *) malloc(sizeof(uint8_t) * t->num_elems);
			ALLOC(t->reu8);
			READNN(t->reu8, sizeof(uint8_t), t->num_elems, iter->f->fp);
			iter->offset += sizeof(uint8_t) * t->num_elems;
		}
		else if (t->dtype_id == QG8_DTYPE_UINT16)
		{
			t->reu16 = (uint16_t *) malloc(sizeof(uint16_t) * t->num_elems);
			ALLOC(t->reu16);
			READNN(t->reu16, sizeof(uint16_t), t->num_elems, iter->f->fp);
			iter->offset += sizeof(uint16_t) * t->num_elems;
		}
		else if (t->dtype_id == QG8_DTYPE_UINT32)
		{
			t->reu32 = (uint32_t *) malloc(sizeof(uint32_t) * t->num_elems);
			ALLOC(t->reu32);
			READNN(t->reu32, sizeof(uint32_t), t->num_elems, iter->f->fp);
			iter->offset += sizeof(uint32_t) * t->num_elems;
		}
		else if (t->dtype_id == QG8_DTYPE_UINT64)
		{
			t->reu64 = (uint64_t *) malloc(sizeof(uint64_t) * t->num_elems);
			ALLOC(t->reu64);
			READNN(t->reu64, sizeof(uint64_t), t->num_elems, iter->f->fp);
			iter->offset += sizeof(uint64_t) * t->num_elems;
		}
		else if (t->dtype_id == QG8_DTYPE_INT8)
		{
			t->rei8 = (int8_t *) malloc(sizeof(int8_t) * t->num_elems);
			ALLOC(t->rei8);
			READNN(t->rei8, sizeof(int8_t), t->num_elems, iter->f->fp);
			iter->offset += sizeof(int8_t) * t->num_elems;
		}
		else if (t->dtype_id == QG8_DTYPE_INT16)
		{
			t->rei16 = (int16_t *) malloc(sizeof(int16_t) * t->num_elems);
			ALLOC(t->rei16);
			READNN(t->rei16, sizeof(int16_t), t->num_elems, iter->f->fp);
			iter->offset += sizeof(int16_t) * t->num_elems;
		}
		else if (t->dtype_id == QG8_DTYPE_INT32)
		{
			t->rei32 = (int32_t *) malloc(sizeof(int32_t) * t->num_elems);
			ALLOC(t->rei32);
			READNN(t->rei32, sizeof(int32_t), t->num_elems, iter->f->fp);
			iter->offset += sizeof(int32_t) * t->num_elems;
		}
		else if (t->dtype_id == QG8_DTYPE_INT64)
		{
			t->rei64 = (int64_t *) malloc(sizeof(int64_t) * t->num_elems);
			ALLOC(t->rei64);
			READNN(t->rei64, sizeof(int64_t), t->num_elems, iter->f->fp);
			iter->offset += sizeof(int64_t) * t->num_elems;
		}
		else
		{
			/* error on any other dtype_id */
			free(t->indices);
			free(t->dimensions);
			free(t);
			free(chunk);
			fprintf(stderr,
			        "Received unrecognised dtype_id %d for chunk tensor data.\n"
			        , t->dtype_id);
			exit(EXIT_FAILURE);
		}
		chunk->tensor = t;
	}
	else
	{
		chunk->tensor = NULL;
	}
	iter->done_read = 1;

	return chunk;
}

