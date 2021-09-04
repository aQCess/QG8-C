/*
 * chunk_test.c
 * Chunk creation tests.
 *
 * Author       : Finn Rayment <finn@rayment.fr>
 * Date created : 04/09/2021
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

#include "common_test.h"
#include "macros.h"
#include "qg8.h"

int
main(int argc,
     char **argv)
{
	qg8_chunk *c1, *c2;
	qg8_tensor *t1, *t2;
	uint64_t dims[3], dims2[2];
	uint64_t **ind, **ind2;
	int8_t re[2*2*2];
	float re2[3], im2[3];
	int i, j;

	INIT();

	ind = (uint64_t **) malloc(sizeof(uint64_t *) * 3);
	ALLOC(ind);
	ind2 = (uint64_t **) malloc(sizeof(uint64_t *) * 2);
	ALLOC(ind2);
	for (i = 0; i < 3; ++i)
	{
		ind[i] = (uint64_t *) malloc(sizeof(uint64_t) * 8);
		ALLOC(ind[i]);
		if (i < 2)
		{
			ind2[i] = (uint64_t *) malloc(sizeof(uint64_t) * 3);
			ALLOC(ind2[i]);
		}
	}

	dims[0] = 2;
	dims[1] = 2;
	dims[2] = 2;
	dims2[0] = 2;
	dims2[1] = 3;
	for (i = 0; i < 2*2*2; ++i)
	{
		ind[0][i] = i;
		ind[1][i] = i;
		ind[2][i] = i;
		re[i] = i*i;
	}
	j = 0;
	for (i = 0; i < 3; ++i)
	{
		ind2[0][i] = (float) (j++ % 2);
		ind2[1][i] = (float) i;
		re2[i] = (float) i;
		im2[i] = re2[i] + 3.141592;
	}

	(void) argc;
	(void) argv;

	t1 = qg8_tensor_create_int8(ind, re, 8, dims, 3, QG8_PACKING_FULL);
	t2 = qg8_tensor_create_float(ind2, re2, im2, 3, dims2, 2, QG8_PACKING_FULL);

	TEST(
		c1 = qg8_chunk_create(QG8_TYPE_KET, 0, (uint8_t *) "chunk1", t1);
		c2 = qg8_chunk_create(QG8_TYPE_KET, 0, (uint8_t *) "chunk2", t2);
	, c1 != NULL && c2 != NULL, "qg8_chunk_create"
	);

	TEST(
		;
	, qg8_chunk_get_tensor(c1) == t1 &&
	  qg8_chunk_get_tensor(c2) == t2, "qg8_chunk_get_tensor"
	);

	TEST(
		;
	, qg8_chunk_get_flags(c1) == QG8_FLAG_LABEL &&
	  qg8_chunk_get_flags(c2) == QG8_FLAG_LABEL, "qg8_chunk_get_flags"
	);

	TEST(
		;
	, memcmp(qg8_chunk_get_string_id(c1), "chunk1", 6) == 0 &&
	  memcmp(qg8_chunk_get_string_id(c2), "chunk2", 6) == 0,
	  "qg8_chunk_get_string_id"
	);

	TEST(
		;
	, qg8_chunk_get_type(c1) == QG8_TYPE_KET &&
	  qg8_chunk_get_type(c2) == QG8_TYPE_KET,
	  "qg8_chunk_get_type"
	);

	TEST(
		;
	, qg8_chunk_destroy(c1) == 1 && qg8_chunk_destroy(c2) == 1,
	  "qg8_chunk_destroy"
	);

	for (i = 0; i < 3; ++i)
		free(ind[i]);
	free(ind);
	for (i = 0; i < 2; ++i)
		free(ind2[i]);
	free(ind2);

	PASS();
}

