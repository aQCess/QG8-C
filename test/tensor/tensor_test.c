/*
 * tensor_test.c
 * Tensor suite for valid input.
 *
 * Author       : Finn Rayment <finn@rayment.fr>
 * Date created : 24/07/2021
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

#define TEST_CREATE_SINGLE(x)\
t1 = qg8_tensor_create_##x(ind, re, 8, dims, 3, QG8_PACKING_FULL);

#define TEST_CREATE_COMPLEX(x,y)\
if (y == 1)\
	t1 = qg8_tensor_create_##x(\
		     ind, re, im, 8, dims, 3, QG8_PACKING_FULL);\
else\
	t1 = qg8_tensor_create_##x(\
	         ind, re, NULL, 8, dims, 3, QG8_PACKING_FULL);\

/* big macro set for tensor functions */
#define TEST_TENSOR(x,y,z,w,q,r,s)\
int test_tensor_##x(void)\
{\
	uint64_t dims[3], *dims2;\
	uint64_t **ind, **ind2;\
	z re[2*2*2], *re2;\
	z im[2*2*2], *im2;\
	int i, j;\
	qg8_tensor *t1;\
\
	(void) im;\
	ind = (uint64_t **) malloc(sizeof(uint64_t *) * 3);\
	ALLOC(ind);\
	for (i = 0; i < 3; ++i)\
	{\
		ind[i] = (uint64_t *) malloc(sizeof(uint64_t) * 8);\
		ALLOC(ind[i]);\
	}\
\
	dims[0] = 2;\
	dims[1] = 2;\
	dims[2] = 2;\
	for (i = 0; i < 2*2*2; ++i)\
	{\
		ind[0][i] = i;\
		ind[1][i] = i;\
		ind[2][i] = i;\
		re[i] = i*i;\
		im[i] = -i;\
	}\
	TEST(\
		s\
	, t1 != NULL, "qg8_tensor_create_"STRINGIFY(y)" ("STRINGIFY(x)")"\
	);\
	TEST(\
		;\
	, qg8_tensor_get_rank(t1) == 3, "qg8_tensor_get_rank == 3"\
	);\
	TEST(\
		;\
	, qg8_tensor_get_num_elems(t1) == 8, "qg8_tensor_get_num_elems == 8"\
	);\
	TEST(\
		;\
	, qg8_tensor_get_itypeid(t1) == QG8_DTYPE_UINT8,\
	"qg8_tensor_get_itypeid == QG8_DTYPE_UINT8"\
	);\
	TEST(\
		;\
	, qg8_tensor_get_dtypeid(t1) == q,\
	"qg8_tensor_get_dtypeid == "r\
	);\
	TEST(\
		dims2 = qg8_tensor_get_dims(t1);\
	, dims2[0] == 2 && dims2[1] == 2 && dims2[2] == 2,\
	"qg8_tensor_get_dims == [2,2,2]"\
	);\
	TEST(\
		ind2 = qg8_tensor_get_indices(t1);\
		j = 1;\
		for (i = 0; i < 2*2*2; ++i)\
		{\
			if ((int) ind2[0][i] != i ||\
			    (int) ind2[1][i] != i ||\
			    (int) ind2[2][i] != i)\
			{\
				j = 0;\
				break;\
			}\
		}\
	, j == 1, "qg8_tensor_get_indices == [[0,0,0],[1,1,1],...]"\
	);\
	TEST(\
		re2 = qg8_tensor_get_re(t1);\
		j = 1;\
		for (i = 0; i < 2*2*2; ++i)\
		{\
			if (re2[i] != i*i)\
			{\
				j = 0;\
				break;\
			}\
		}\
	, j == 1, "qg8_tensor_get_re == [0,1,4,9,...]"\
	);\
	if (w == 1)\
	{\
		TEST(\
			im2 = qg8_tensor_get_im(t1);\
			j = 1;\
			if (!im2)\
			{\
				j = 0;\
			}\
			else\
			{\
				for (i = 0; i < 2*2*2; ++i)\
				{\
					if ((int) im2[i] != -i)\
					{\
						j = 0;\
						break;\
					}\
				}\
			}\
		, j == 1, "qg8_tensor_get_im == [0,-1,-2,...]"\
		);\
	}\
	else\
	{\
		TEST(\
			im2 = qg8_tensor_get_im(t1);\
		, im2 == NULL, "qg8_tensor_get_im == NULL"\
		);\
	}\
	TEST(\
		j = qg8_tensor_destroy(t1);\
	, j == 1, "qg8_tensor_destroy(t1)"\
	);\
\
	for (i = 0; i < 3; ++i)\
		free(ind[i]);\
	free(ind);\
	return 1;\
}

TEST_TENSOR(float_complex, float, float, 1,
            QG8_DTYPE_COMPLEX64, "QG8_DTYPE_COMPLEX64",
            TEST_CREATE_COMPLEX(float, 1))
TEST_TENSOR(double_complex, double, double, 1,
            QG8_DTYPE_COMPLEX128, "QG8_DTYPE_COMPLEX128",
            TEST_CREATE_COMPLEX(double, 1))
TEST_TENSOR(float, float, float, 0,
            QG8_DTYPE_FLOAT32, "QG8_DTYPE_FLOAT32",
            TEST_CREATE_COMPLEX(float, 0))
TEST_TENSOR(double, double, double, 0,
            QG8_DTYPE_FLOAT64, "QG8_DTYPE_FLOAT64",
            TEST_CREATE_COMPLEX(double, 0))
TEST_TENSOR(uint8, uint8, uint8_t, 0,
            QG8_DTYPE_UINT8, "QG8_DTYPE_UINT8",
            TEST_CREATE_SINGLE(uint8))
TEST_TENSOR(uint16, uint16, uint16_t, 0,
            QG8_DTYPE_UINT16, "QG8_DTYPE_UINT16",
            TEST_CREATE_SINGLE(uint16))
TEST_TENSOR(uint32, uint32, uint32_t, 0,
            QG8_DTYPE_UINT32, "QG8_DTYPE_UINT32",
            TEST_CREATE_SINGLE(uint32))
TEST_TENSOR(uint64, uint64, uint64_t, 0,
            QG8_DTYPE_UINT64, "QG8_DTYPE_UINT64",
            TEST_CREATE_SINGLE(uint64))
TEST_TENSOR(int8, int8, int8_t, 0,
            QG8_DTYPE_INT8, "QG8_DTYPE_INT8",
            TEST_CREATE_SINGLE(int8))
TEST_TENSOR(int16, int16, int16_t, 0,
            QG8_DTYPE_INT16, "QG8_DTYPE_INT16",
            TEST_CREATE_SINGLE(int16))
TEST_TENSOR(int32, int32, int32_t, 0,
            QG8_DTYPE_INT32, "QG8_DTYPE_INT32",
            TEST_CREATE_SINGLE(int32))
TEST_TENSOR(int64, int64, int64_t, 0,
            QG8_DTYPE_INT64, "QG8_DTYPE_INT64",
            TEST_CREATE_SINGLE(int64))

int
main(int argc,
     char **argv)
{
	INIT();

	(void) argc;
	(void) argv;

	test_tensor_float_complex();
	test_tensor_double_complex();
	test_tensor_float();
	test_tensor_double();
	test_tensor_uint8();
	test_tensor_uint16();
	test_tensor_uint32();
	test_tensor_uint64();
	test_tensor_int8();
	test_tensor_int16();
	test_tensor_int32();
	test_tensor_int64();

	PASS();
}

