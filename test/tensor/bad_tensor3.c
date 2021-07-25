/*
 * bad_tensor3.c
 * Tensor with NULL real component.
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

int
main(int argc,
     char **argv)
{
	uint64_t dims[3];
	uint64_t **ind;
	int i;

	INIT();

	ind = (uint64_t **) malloc(sizeof(uint64_t *) * 3);
	ALLOC(ind);
	for (i = 0; i < 3; ++i)
	{
		ind[i] = (uint64_t *) malloc(sizeof(uint64_t) * 8);
		ALLOC(ind[i]);
	}

	dims[0] = 2;
	dims[1] = 2;
	dims[2] = 2;
	for (i = 0; i < 2*2*2; ++i)
	{
		ind[0][i] = i;
		ind[1][i] = i;
		ind[2][i] = i;
	}

	(void) argc;
	(void) argv;

	TEST(
		qg8_tensor_create_uint64(ind, NULL, 8, dims, 3, QG8_PACKING_FULL);
	, 1 == 0, "qg8_tensor_create_uint64 (NULL real)\n"
	);

	PASS();
}

