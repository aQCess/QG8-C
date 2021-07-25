/*
 * file_write.c
 * Write chunks to file.
 *
 * Author       : Finn Rayment <finn@rayment.fr>
 * Date created : 25/07/2021
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

#include "common_test.h"
#include "macros.h"
#include "qg8.h"

int
main(int argc,
     char **argv)
{
	qg8_file *f;
	qg8_chunk *c1, *c2;
	qg8_tensor *t1, *t2;
	uint64_t dims[3], dims2[2];
	uint64_t **ind, **ind2;
	int8_t re[2*2*2], re2[3];
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
		ind2[0][i] = j++ % 2;
		ind2[1][i] = i;
	}

	(void) argc;
	(void) argv;

	t1 = qg8_tensor_create_int8(ind, re, 8, dims, 3, QG8_PACKING_FULL);
	t2 = qg8_tensor_create_int8(ind2, re2, 3, dims2, 2, QG8_PACKING_FULL);

	c1 = qg8_chunk_create(QG8_TYPE_KET, 0, (uint8_t *) "chunk1", t1);
	c2 = qg8_chunk_create(QG8_TYPE_KET, 0, (uint8_t *) "chunk2", t2);

	TEST(
		f = qg8_file_open("file/test_write.qg8", QG8_MODE_WRITE);
	, f != NULL, "qg8_file_open"
	);

	TEST(
		i = qg8_file_write_chunk(f, c1) && qg8_file_write_chunk(f, c2);
	, i == 1, "qg8_file_write_chunk"
	);

	TEST(
		i = qg8_file_flush(f);
	, i == 1, "qg8_file_flush"
	);

	TEST(
		i = qg8_file_close(f);
	, i == 1, "qg8_file_close"
	);

	qg8_chunk_destroy(c1);
	qg8_chunk_destroy(c2);

	for (i = 0; i < 3; ++i)
		free(ind[i]);
	free(ind);
	for (i = 0; i < 2; ++i)
		free(ind2[i]);
	free(ind2);

	PASS();
}

