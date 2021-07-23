/*
 * macros.h
 * Common macro source.
 *
 * Author       : Finn Rayment <finn@rayment.fr>
 * Date created : 30/05/2021
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
#include <stdlib.h>

#include "qg8.h"

void
_size_check(size_t receive,
            size_t expect,
            int line)
{
	if (expect != receive)
	{
		fprintf(stderr, "%d: Malformed file. Got %ld bytes and not %ld.\n",
		        line, receive, expect);
		exit(EXIT_FAILURE);
	}
}

uint8_t
_type_to_size(uint8_t type)
{
	switch (type)
	{
	case QG8_DTYPE_BOOL:
	case QG8_DTYPE_CHAR:
	case QG8_DTYPE_UINT8:
	case QG8_DTYPE_INT8:
		return QG8_SIZE_8;
	case QG8_DTYPE_UINT16:
	case QG8_DTYPE_INT16:
		return QG8_SIZE_16;
	case QG8_DTYPE_UINT32:
	case QG8_DTYPE_INT32:
	case QG8_DTYPE_FLOAT32:
	case QG8_DTYPE_COMPLEX64:
		return QG8_SIZE_32;
	case QG8_DTYPE_UINT64:
	case QG8_DTYPE_INT64:
	case QG8_DTYPE_FLOAT64:
	case QG8_DTYPE_COMPLEX128:
		return QG8_SIZE_64;
	default:
		fprintf(stderr, "Failed to convert type %d to a size.\n", type);
		exit(EXIT_FAILURE);
	}
}

