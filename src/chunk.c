/*
 * chunk.c
 * QG8 base library chunk source.
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

qg8_chunk *
qg8_chunk_create(uint16_t type,
                 uint8_t flags,
                 uint8_t *string_id,
                 qg8_tensor *tensor)
{
	qg8_chunk *chunk;
	size_t num;
	uint8_t new_flags;
	chunk = (qg8_chunk *) malloc(sizeof(qg8_chunk));
	ALLOC(chunk);
	chunk->tensor = tensor;
	memset(chunk->string_id, 0, 16 * sizeof(chunk->string_id[0]));
	new_flags = flags;
	if (string_id)
	{
		new_flags |= QG8_FLAG_LABEL;
		num = strlen((char *) string_id);
		if (num > 16)
			num = 16;
		memcpy(chunk->string_id, string_id, num);
	}
	chunk->flags = new_flags;
	chunk->type = type;
	return chunk;
}

int
qg8_chunk_destroy(qg8_chunk *chunk)
{
	if (!chunk)
	{
		DIE("Cannot destroy a NULL chunk.\n");
	}
	if (chunk->tensor)
		qg8_tensor_destroy(chunk->tensor);
	free(chunk);
	return 1;
}

qg8_tensor *
qg8_chunk_get_tensor(qg8_chunk *chunk)
{
	if (!chunk)
	{
		DIE("Cannot get tensor from a NULL chunk.\n");
	}
	return chunk->tensor;
}

uint8_t
qg8_chunk_get_flags(qg8_chunk *chunk)
{
	if (!chunk)
	{
		DIE("Cannot get flags from a NULL chunk.\n");
	}
	return chunk->flags;
}

uint8_t *
qg8_chunk_get_string_id(qg8_chunk *chunk)
{
	if (!chunk)
	{
		DIE("Cannot get string id from a NULL chunk.\n");
	}
	return chunk->string_id;
}

uint16_t
qg8_chunk_get_type(qg8_chunk *chunk)
{
	if (!chunk)
	{
		DIE("Cannot get chunk type from a NULL chunk.\n");
	}
	return chunk->type;
}

