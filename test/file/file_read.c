/*
 * file_read.c
 * Read chunks from file.
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
#include <string.h>

#include "common_test.h"
#include "macros.h"
#include "qg8.h"

int
main(int argc,
     char **argv)
{
	qg8_file *f;
	qg8_iter iter;
	qg8_chunk *c1, *c2;
	qg8_tensor *t1, *t2;
	uint64_t *dims, **ind;
	int8_t *re;
	int i, j, k;

	INIT();

	(void) argc;
	(void) argv;

	TEST(
		f = qg8_file_open("file/test_write.qg8", QG8_MODE_READ);
		iter = qg8_file_iterator(f);
	, f != NULL, "qg8_file_open"
	);

	TEST(
		;
	, qg8_file_has_next(&iter) == 1, "qg8_file_has_next == 1"
	);

	TEST(
		c1 = qg8_file_extract(&iter);
	, c1 != NULL, "qg8_file_extract"
	);

	TEST(
		;
	, qg8_file_has_next(&iter) == 1, "qg8_file_has_next == 1"
	);

	TEST(
		c2 = qg8_file_extract(&iter);
	, c2 != NULL, "qg8_file_extract"
	);

	TEST(
		;
	, qg8_file_has_next(&iter) == 0, "qg8_file_has_next == 0"
	);

	(void) t1;
	(void) t2;

	TEST(
		t1 = qg8_chunk_get_tensor(c1);
		dims = (uint64_t *) qg8_tensor_get_dims(t1);
	, qg8_chunk_get_type(c1) == QG8_TYPE_KET &&
	  qg8_chunk_get_flags(c1) == QG8_FLAG_LABEL &&
	  memcmp(qg8_chunk_get_string_id(c1), "chunk1\0\0\0\0\0\0\0\0\0\0", 16)
		== 0 &&
	  t1 != NULL &&
	  qg8_tensor_get_itypeid(t1) == QG8_DTYPE_UINT8 &&
	  qg8_tensor_get_dtypeid(t1) == QG8_DTYPE_INT8 &&
	  qg8_tensor_get_rank(t1) == 3 &&
	  dims[0] == 2 && dims[1] == 2 && dims[2] == 2 &&
	  qg8_tensor_get_num_elems(t1) == 8
	, "chunk 1 verify"
	  /*chunk:
	    type: QG8_TYPE_KET
	    flags: QG8_FLAG_LABEL
	    name: chunk1
	    tensor:
	      itype: QG8_DTYPE_UINT8
	      dtype: QG8_DTYPE_INT8
	      rank: 3
	      dimensions: 2x2x2
	      num elements: 8*/
	);

	TEST(
		re = qg8_tensor_get_re(t1);
		ind = qg8_tensor_get_indices(t1);
		j = 1;
		for (i = 0; i < 2*2*2; ++i)
		{
			if (re[i] != i*i ||
				(int) ind[0][i] != i ||
				(int) ind[1][i] != i ||
				(int) ind[2][i] != i)
				j = 0;
		}
	, j == 1 &&
	  qg8_tensor_get_im(t1) == NULL
	  , "tensor 1 verify"
	);

	TEST(
		t2 = qg8_chunk_get_tensor(c2);
		dims = (uint64_t *) qg8_tensor_get_dims(t2);
	, qg8_chunk_get_type(c2) == QG8_TYPE_KET &&
	  qg8_chunk_get_flags(c2) == QG8_FLAG_LABEL &&
	  memcmp(qg8_chunk_get_string_id(c2), "chunk2\0\0\0\0\0\0\0\0\0\0", 16)
		== 0 &&
	  t2 != NULL &&
	  qg8_tensor_get_itypeid(t2) == QG8_DTYPE_UINT8 &&
	  qg8_tensor_get_dtypeid(t2) == QG8_DTYPE_INT8 &&
	  qg8_tensor_get_rank(t2) == 2 &&
	  dims[0] == 2 && dims[1] == 3 &&
	  qg8_tensor_get_num_elems(t2) == 3
	, "chunk 2 verify"
	  /*chunk:
	    type: QG8_TYPE_KET
	    flags: QG8_FLAG_LABEL
	    name: chunk2
	    tensor:
	      itype: QG8_DTYPE_UINT8
	      dtype: QG8_DTYPE_INT8
	      rank: 2
	      dimensions: 2x3
	      num elements: 3*/
	);

	TEST(
		re = qg8_tensor_get_re(t2);
		ind = qg8_tensor_get_indices(t2);
		j = 1;
		k = 0;
		for (i = 0; i < 3; ++i)
		{
			if (re[i] != i ||
				(int) ind[0][i] != k++ % 2 ||
				(int) ind[1][i] != i)
				j = 0;
		}
	, j == 1 &&
	  qg8_tensor_get_im(t2) == NULL
	  , "tensor 2 verify"
	);

	TEST(
		i = qg8_file_close(f);
	, i == 1, "qg8_file_close"
	);

	PASS();
}

