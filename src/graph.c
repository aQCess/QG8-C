/*
 * graph.c
 * QG8 base library graph source.
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

/*static
void
_qg8_graph_recalculate(qg8_graph *graph,
                       int remove,
                       size_t remove_idx)
{
	qg8_adjacencymatrix *adjtensor;
	qg8_chunk *old;
	size_t chunk_len, i, j;
	qg8_chunk_linkedlist *ll, *tmpll;
	uint64_t **indices, *shape, len;
	float *re;

	for (chunk_len = 0, ll = graph->chunks; ll != NULL; ll = ll->next)
		++chunk_len;
	if (remove)
		--chunk_len;
	if (chunk_len == 0 && graph->chunks)
	{
		if (graph->adjchunk)
			qg8_chunk_destroy(graph->adjchunk);
		graph->adjchunk = NULL;
		ll = graph->chunks;
		while (ll)
		{
			qg8_chunk_destroy(ll->chunk);
			tmpll = ll->next;
			free(ll);
			ll = tmpll;
		}
		graph->chunks = NULL;
		return;
	}

	old = graph->adjchunk;
	len = 0;
*/	/* prepass to find how many elements are needed */
/*	if (old)
	{
		for (i = 0; i < old->tensor->num_elems; ++i)
		{
			if (remove && (remove_idx == old->tensor->indices[0][i] ||
			               remove_idx == old->tensor->indices[1][i]))
			{
*/				/* don't count stuff that belongs to the removed element */
/*				continue;
			}
			++len;
		}
	}
*/	/* second pass to grab them */
/*	shape = (uint64_t *) malloc(2 * sizeof(uint64_t));
	ALLOC(shape);
	indices = (uint64_t **) malloc(2 * sizeof(uint64_t *));
	ALLOC(indices);
	j = 0;
	if (old)
	{
		shape[0] = chunk_len;
		shape[1] = chunk_len;
		indices[0] = (uint64_t *) malloc(len * sizeof(uint64_t));
		ALLOC(indices[0]);
		indices[1] = (uint64_t *) malloc(len * sizeof(uint64_t));
		ALLOC(indices[1]);
		re = (float *) malloc(len * sizeof(float));
		ALLOC(re);
		for (i = 0; i < old->tensor->num_elems; ++i)
		{
			if (remove)
			{
				if (remove_idx == old->tensor->indices[0][i] ||
				    remove_idx == old->tensor->indices[1][i])
				{
*/					/* don't count stuff that belongs to the removed element */
/*					continue;
				}
					if (old->tensor->indices[0][i] > remove_idx)
					--old->tensor->indices[0][i];
				if (old->tensor->indices[1][i] > remove_idx)
					--old->tensor->indices[1][i];
			}
			indices[0][j] = old->tensor->indices[0][i];
			indices[1][j] = old->tensor->indices[1][i];
			re[j] = old->tensor->refloat[i];
		}
	}
	else
	{
		shape[0] = 1;
		shape[1] = 1;
		indices[0] = (uint64_t *) malloc(sizeof(uint64_t));
		ALLOC(indices[0]);
		indices[1] = (uint64_t *) malloc(sizeof(uint64_t));
		ALLOC(indices[1]);
		indices[0][0] = 0;
		indices[1][0] = 0;
		re = (float *) malloc(sizeof(float));
		ALLOC(re);
		re[0] = 0xba;
		len = 1;
	}

	if (old)
		qg8_chunk_destroy(old);
	adjtensor = qg8_tensor_create_float(indices, re, NULL, len, shape, 2, 0);
	adjtensor->loaded = 1;
	graph->adjchunk = qg8_chunk_create((uint8_t *) NULL, QG8_NTYPE_ADJACENCYMATRIX, adjtensor);
}*/

qg8_graph *
qg8_graph_load(const char *filename)
{
	qg8_file *file;
	qg8_iter iter;
	qg8_chunk *chunk/*, *adjchunk*/;
	qg8_graph *g;

	if (!filename)
	{
		DIE("Cannot load graph with a NULL filename.\n");
	}
	file = qg8_file_open(filename, QG8_MODE_READ);

	/*adjchunk = NULL;*/
	g = qg8_graph_create();
	for (iter = qg8_file_iterator(file);
	     qg8_file_has_next(&iter) == 1;)
	{
		chunk = qg8_file_extract(&iter);
		if (!chunk || qg8_graph_add_chunk(g, chunk) != 1)
		{
			qg8_graph_destroy(g);
			DIE("Failed to load chunk into graph.\n");
		}
		/*if (chunk->type != QG8_NTYPE_ADJACENCYMATRIX)
		{
			_qg8_graph_add_chunk_internal(g, chunk, 0);
		}
		else
		{
			if (adjchunk)
			{} TODO: error on multiple adjacency matrices
			adjchunk = chunk;
		}*/
	}
	/*if (!adjchunk)
	{}*/ /* TODO: no adjacency matrix (create one?) */
	/*g->adjchunk = adjchunk;*/
	return g;
}

qg8_graph *
qg8_graph_create(void)
{
	qg8_graph *g;
	g = (qg8_graph *) malloc(sizeof(qg8_graph));
	ALLOC(g);
	g->chunks = NULL;
	/*g->adjchunk = NULL;*/
	return g;
}

int
qg8_graph_write(const char *filename,
                qg8_graph *graph)
{
	qg8_chunk_linkedlist *ll;
	qg8_file *qg8f;

	if (!filename)
	{
		DIE("Cannot write graph to file with NULL filename.\n");
	}
	if (!graph)
	{
		DIE("Cannot write a NULL graph to a file.\n");
	}

	qg8f = qg8_file_open(filename, QG8_MODE_WRITE);

	ll = graph->chunks;
	while (ll)
	{
		qg8_file_write_chunk(qg8f, ll->chunk);
		ll = ll->next;
	}
	qg8_file_flush(qg8f);
	qg8_file_close(qg8f);
	return 1;
}

/*uint8_t
qg8_graph_get_datasize(qg8_graph *graph)
{
	*//* TODO: implement qg8_graph_get_datasize */
/*	(void) graph;
	return 0;
	*//*return graph->datasize;*/
/*}

qg8_adjacencymatrix *
qg8_graph_get_adjacencymatrix(qg8_graph *graph)
{
	if (graph->adjchunk)
		return graph->adjchunk->tensor;
	return NULL; *//* TODO good ? */
/*}*/

uint64_t
qg8_graph_get_number_chunks(qg8_graph *graph)
{
	uint64_t i;
	qg8_chunk_linkedlist *l;
	if (!graph || !graph->chunks)
	{
		DIE("Cannot get number of chunks from a NULL graph.\n");
	}
	i = 0;
	l = graph->chunks;
	while (l)
	{
		++i;
		l = l->next;
	}
	return i;
}

qg8_chunk *
qg8_graph_get_chunk(qg8_graph *graph,
                    uint64_t idx)
{
	qg8_chunk_linkedlist *l;
	if (!graph)
	{
		DIE("Cannot get chunk from a NULL graph.\n");
	}
	l = graph->chunks;
	while (l)
	{
		if (idx == 0)
			return l->chunk;
		--idx;
		l = l->next;
	}
	fprintf(stderr, "Cannot get chunk at index %ld from graph with size %ld.\n",
	        idx, qg8_graph_get_number_chunks(graph));
	exit(EXIT_FAILURE);
	return NULL; /* fallback */
}

int
qg8_graph_add_chunk(qg8_graph *graph,
                    qg8_chunk *chunk)
{
	qg8_chunk_linkedlist *l;
	l = (qg8_chunk_linkedlist *) malloc(sizeof(qg8_chunk_linkedlist));
	ALLOC(l);
	l->chunk = chunk;
	l->next = graph->chunks;
	graph->chunks = l;
	return 1;
}

int
qg8_graph_remove_chunk(qg8_graph *graph,
                       qg8_chunk *chunk)
{
	qg8_chunk_linkedlist *l, *last;
	size_t idx;
	int removed;

	if (!graph)
	{
		DIE("Cannot remove chunk from NULL graph.\n");
	}
	if (!chunk)
	{
		DIE("Cannot remove NULL chunk from graph.\n");
	}

	l = graph->chunks;
	if (!l)
		return 0;
	idx = 0;
	removed = 0;
	while (l)
	{
		if (l->chunk != chunk)
		{
			last = l;
			++idx;
			continue;
		}
		if (idx == 0)
			graph->chunks = graph->chunks->next;
		else
			last->next = l->next;
		qg8_chunk_destroy(l->chunk);
		free(l);
		removed = 1;
	}
	return removed;
}

int
qg8_graph_destroy(qg8_graph *graph)
{
	qg8_chunk_linkedlist *l, *tmp;

	if (!graph)
	{
		DIE("Cannot destroy a NULL graph\n");
	}
	/*if (graph->adjchunk)
	{
		if (!qg8_chunk_destroy(graph->adjchunk))
			return 0;
	}*/
	l = graph->chunks;
	while (l)
	{
		qg8_chunk_destroy(l->chunk);
		tmp = l->next;
		free(l);
		l = tmp;
	}
	free(graph);
	return 1;
}

