/*
 * graph_create.c
 * Graph loading from file.
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
	qg8_graph *g;
	qg8_chunk *c;

	INIT();

	(void) argc;
	(void) argv;

	TEST(
		g = qg8_graph_create();
	, g != NULL, "qg8_graph_create"
	);

	TEST(
		;
	, qg8_graph_get_number_chunks(g) == 0, "qg8_graph_get_number_chunks == 0"
	);

	c = qg8_chunk_create(QG8_TYPE_KET, 0, (uint8_t *) "chunk1", NULL);

	TEST(
		;
	, qg8_graph_add_chunk(g, c) == 1, "qg8_graph_add_chunk"
	);

	TEST(
		;
	, qg8_graph_get_number_chunks(g) == 1, "qg8_graph_get_number_chunks == 1"
	);

	TEST(
		;
	, c == qg8_graph_get_chunk(g, 0), "qg8_graph_get_chunk"
	);

	TEST(
		;
	, qg8_graph_remove_chunk(g, c) == 1, "qg8_graph_remove_chunk"
	);

	TEST(
		;
	, qg8_graph_get_number_chunks(g) == 0, "qg8_graph_get_numnber_chunks == 0"
	);

	TEST(
		;
	, qg8_graph_destroy(g) == 1, "qg8_graph_destroy"
	);

	PASS();
}

