/*
 * graph_load.c
 * Graph loading from file.
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
	qg8_graph *g;
	qg8_chunk *c;
	qg8_tensor *t;
	uint64_t *dims;
	int i;

	INIT();

	(void) argc;
	(void) argv;

	TEST(
		g = qg8_graph_load("graph/test_numpy.qg8");
	, g != NULL, "qg8_graph_load"
	);

	TEST(
		;
	, qg8_graph_get_number_chunks(g) == 5, "qg8_graph_get_number_chunks == 5"
	);

	TEST(
		c = qg8_graph_get_chunk(g, 0);
		t = qg8_chunk_get_tensor(c);
		dims = (uint64_t *) qg8_tensor_get_dims(t);
	, qg8_chunk_get_type(c) == QG8_TYPE_INPUT &&
	  qg8_chunk_get_flags(c) == QG8_FLAG_LABEL &&
	  memcmp(qg8_chunk_get_string_id(c), "~this\\label%is!t", 16) == 0 &&
	  t != NULL &&
	  qg8_tensor_get_itypeid(t) == QG8_DTYPE_UINT8 &&
	  qg8_tensor_get_dtypeid(t) == QG8_DTYPE_COMPLEX128 &&
	  qg8_tensor_get_rank(t) == 2 &&
	  dims[0] == 64 && dims[1] == 64 &&
	  qg8_tensor_get_num_elems(t) == 3120
	, "chunk 1 verify"
	  /*chunk:
	    type: QG8_TYPE_INPUT
	    flags: QG8_FLAG_LABEL
	    name: ~this\label%is!t
	    tensor:
	      itype: QG8_DTYPE_UINT8
	      dtype: QG8_DTYPE_COMPLEX128
	      rank: 2
	      dimensions: 64x64
	      num elements: 3120*/
	);

	TEST(
		c = qg8_graph_get_chunk(g, 1);
		t = qg8_chunk_get_tensor(c);
		dims = (uint64_t *) qg8_tensor_get_dims(t);
	, qg8_chunk_get_type(c) == QG8_TYPE_CONSTANT &&
	  qg8_chunk_get_flags(c) == 0 &&
	  t != NULL &&
	  qg8_tensor_get_itypeid(t) == QG8_DTYPE_UINT8 &&
	  qg8_tensor_get_dtypeid(t) == QG8_DTYPE_UINT16 &&
	  qg8_tensor_get_rank(t) == 6 &&
	  dims[0] == 1 && dims[1] == 2 && dims[2] == 3 &&
	  dims[3] == 4 && dims[4] == 5 && dims[5] == 6 &&
	  qg8_tensor_get_num_elems(t) == 720
	, "chunk 2 verify"
	  /*chunk:
	    type: QG8_TYPE_CONSTANT
	    flags: nil
	    tensor:
	      itype: QG8_DTYPE_UINT8
	      dtype: QG8_DTYPE_UINT16
	      rank: 6
	      dimensions: 1x2x3x4x5x6
	      num elements: 720*/
	);

	TEST(
		c = qg8_graph_get_chunk(g, 2);
		t = qg8_chunk_get_tensor(c);
		dims = (uint64_t *) qg8_tensor_get_dims(t);
	, qg8_chunk_get_type(c) == QG8_TYPE_CONSTANT &&
	  qg8_chunk_get_flags(c) == QG8_FLAG_LABEL &&
	  memcmp(qg8_chunk_get_string_id(c), "2D dense array\0\0", 16) == 0 &&
	  t != NULL &&
	  qg8_tensor_get_itypeid(t) == QG8_DTYPE_UINT16 &&
	  qg8_tensor_get_dtypeid(t) == QG8_DTYPE_FLOAT32 &&
	  qg8_tensor_get_rank(t) == 2 &&
	  dims[0] == 256 && dims[1] == 256 &&
	  qg8_tensor_get_num_elems(t) == 65536
	, "chunk 3 verify"
	  /*chunk:
	    type: QG8_TYPE_CONSTANT
	    flags: QG8_FLAG_LABEL
	    name: 2D dense array
	    tensor:
	      itype: QG8_DTYPE_UINT16
	      dtype: QG8_DTYPE_FLOAT32
	      rank: 2
	      dimensions: 256x26
	      num elements: 65536*/
	);

	TEST(
		c = qg8_graph_get_chunk(g, 3);
		t = qg8_chunk_get_tensor(c);
		dims = (uint64_t *) qg8_tensor_get_dims(t);
	, qg8_chunk_get_type(c) == QG8_TYPE_CONSTANT &&
	  qg8_chunk_get_flags(c) == QG8_FLAG_LABEL &&
	  memcmp(qg8_chunk_get_string_id(c), "1D sparse array\0", 16) == 0 &&
	  t != NULL &&
	  qg8_tensor_get_itypeid(t) == QG8_DTYPE_UINT32 &&
	  qg8_tensor_get_dtypeid(t) == QG8_DTYPE_UINT8 &&
	  qg8_tensor_get_rank(t) == 1 &&
	  dims[0] == 65536 &&
	  qg8_tensor_get_num_elems(t) == 32942
	, "chunk 4 verify"
	  /*chunk:
	    type: QG8_TYPE_CONSTANT
	    flags: QG8_FLAG_LABEL
	    name: 1D sparse array
	    tensor:
	      itype: QG8_DTYPE_UINT32
	      dtype: QG8_DTYPE_UINT8
	      rank: 1
	      dimensions: 65536
	      num elements: 32942*/
	);

	TEST(
		c = qg8_graph_get_chunk(g, 4);
		t = qg8_chunk_get_tensor(c);
		dims = (uint64_t *) qg8_tensor_get_dims(t);
	, qg8_chunk_get_type(c) == QG8_TYPE_CONSTANT &&
	  qg8_chunk_get_flags(c) == QG8_FLAG_LABEL &&
	  memcmp(qg8_chunk_get_string_id(c), "constant\0\0\0\0\0\0\0\0", 16) == 0 &&
	  t != NULL &&
	  qg8_tensor_get_itypeid(t) == QG8_DTYPE_UINT8 &&
	  qg8_tensor_get_dtypeid(t) == QG8_DTYPE_INT64 &&
	  qg8_tensor_get_rank(t) == 1 &&
	  dims[0] == 1 &&
	  qg8_tensor_get_num_elems(t) == 1
	, "chunk 5 verify"
	  /*chunk:
	    type: QG8_TYPE_CONSTANT
	    flags: QG8_FLAG_LABEL
	    name: constant
	    tensor:
	      itype: QG8_DTYPE_UINT8
	      dtype: QG8_DTYPE_INT64
	      rank: 1
	      dimensions: 1
	      num elements: 1*/
	);

	TEST(
		i = qg8_graph_destroy(g);
	, i == 1, "qg8_graph_destroy"
	);

	PASS();
}

