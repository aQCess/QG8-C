/*
 * file.c
 * QG8 base library file source.
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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"
#include "qg8.h"

SHAPER(64_32, uint32_t, uint64_t)
SHAPER(64_16, uint16_t, uint64_t)
SHAPER(64_8,  uint8_t,  uint64_t)

static
int
_integrity_check(qg8_file *qg8f)
{
	uint8_t sig[8];
	uint16_t version;
	uint8_t res[6];
	int i;

	READ(sig, sig, qg8f->fp);
	READ(&version, version, qg8f->fp);
	READ(res, res, qg8f->fp);

	if (memcmp(sig, "QG8", 3) != 0)
	{
		fprintf(stderr, "Invalid magic string '%s'\n", sig);
		return 0;
	}
	else if (version < 1 || version > QG8_VERSION)
	{
		fprintf(stderr, "Unsupported file version %d\n", version);
		return 0;
	}
	for (i = 0; i < 6; ++i)
	{
		if (*(res+i) != 0)
		{
			fprintf(stderr, "Reserved file header bytes are not 0\n");
			return 0;
		}
	}
	return 1;
}

qg8_file *
qg8_file_open(const char *filename,
              int mode)
{
	qg8_file *qg8f;

	if (!filename)
	{
		fprintf(stderr, "Cannot open a NULL file.\n");
		exit(EXIT_FAILURE);
	}

	if (mode != QG8_MODE_READ && mode != QG8_MODE_WRITE/* &&
	    mode != QG8_MODE_APPEND*/)
	{
		fprintf(stderr, "Invalid QG8 file mode %d.\n", mode);
		exit(EXIT_FAILURE);
	}

	qg8f = (qg8_file *) malloc(sizeof(qg8_file));
	ALLOC(qg8f);
	if (mode == QG8_MODE_READ)
	{
		qg8f->fp = fopen(filename, "r");
		if (!_integrity_check(qg8f))
		{
			free(qg8f);
			exit(EXIT_FAILURE);
		}
	}
	else if (mode == QG8_MODE_WRITE)
	{
		qg8f->fp = fopen(filename, "w");
	}
	else
	{
		qg8f->fp = fopen(filename, "a");
	}
	if (!qg8f->fp)
	{
		perror("fopen");
		free(qg8f);
		exit(EXIT_FAILURE);
	}
	qg8f->mode = mode;
	qg8f->chunks = NULL;

	return qg8f;
}

int
qg8_file_write_chunk(qg8_file *qg8f,
                     qg8_chunk *chunk)
{
	qg8_chunk_linkedlist *node, *tmp;

	if (!qg8f)
	{
		fprintf(stderr, "Cannot write chunk to a NULL file.\n");
		exit(EXIT_FAILURE);
	}

	if (!chunk)
	{
		fprintf(stderr, "Cannot write NULL chunk to a file.\n");
		exit(EXIT_FAILURE);
	}

	if (qg8f->mode == QG8_MODE_READ)
	{
		fprintf(stderr, "Cannot write to a file open in read mode.\n");
		exit(EXIT_FAILURE);
	}

	node = (qg8_chunk_linkedlist *) malloc(sizeof(qg8_chunk_linkedlist));
	ALLOC(node);
	/* TODO: Needs to not happen on no node or no tensor. */
	/* Actually, it's an error in this case, so handle it */
	node->chunk = chunk;
	node->next = NULL;
	if (!qg8f->chunks)
	{
		qg8f->chunks = node;
	}
	else
	{
		tmp = qg8f->chunks;
		while (tmp && tmp->next)
			tmp = tmp->next;
		tmp->next = node;
	}
	return 1;
}

/*int
qg8_file_write_graph(qg8_file *qg8f,
                     qg8_graph *graph)
{
	qg8_chunk_linkedlist *ll;

	if (!qg8f || !graph)
		return 0;

	if (qg8f->mode == QG8_MODE_READ)
	{
		fprintf(stderr, "Cannot write to a QG8 file open in read mode.\n");
		return 0;
	}

	ll = graph->chunks;
	while (ll)
	{
		qg8_file_write_chunk(qg8f, ll->chunk);
		ll = ll->next;
	}
	return 1;
}*/

int
qg8_file_flush(qg8_file *qg8f)
{
	uint64_t i;
	uint16_t version;
	uint8_t blank[8];
	uint64_t tmp2, tmp3, tmp4;
	uint32_t *shape32;
	uint16_t *shape16;
	uint8_t *shape8;
	uint8_t data_size;
	qg8_chunk_linkedlist *tlist;
	qg8_chunk *chunk;
	qg8_tensor *tensor;

	if (!qg8f)
	{
		fprintf(stderr, "Cannot flush to a NULL file.\n");
		exit(EXIT_FAILURE);
	}

	/* preparation */
	memset(blank, 0, 8); /* blank byte buffer prepare */
	version = QG8_VERSION;

	/* file header */
	if (qg8f->mode == QG8_MODE_WRITE) /* ignore in append mode */
	{
		fwrite(QG8_MAGIC, strlen(QG8_MAGIC), 1, qg8f->fp);
		fwrite(&version, sizeof(version), 1, qg8f->fp);
		fwrite(blank, 6, 1, qg8f->fp);
	}
	else
	{
		fprintf(stderr, "Cannot write to a file open in read mode.\n");
		exit(EXIT_FAILURE);
	}

	/* tensors */
	tlist = qg8f->chunks;
	while (tlist)
	{
		chunk = tlist->chunk;
		tensor = chunk->tensor;
		data_size = _type_to_size(tensor->dtype_id);

		/* chunk header */
		fwrite(&chunk->type, sizeof(chunk->type), 1, qg8f->fp);
		fwrite(&chunk->flags, sizeof(chunk->flags), 1, qg8f->fp);
		if ((chunk->flags & QG8_FLAG_LABEL) == QG8_FLAG_LABEL)
			fwrite(chunk->string_id, sizeof(uint8_t) * 16, 1, qg8f->fp);
		fwrite(blank, 5, 1, qg8f->fp); /* _reserved */
		tmp4 = _type_to_size(tensor->itype_id);
		if (!tensor)
		{
			tmp3 = 0;
			/* no skip sans tensor */
			fwrite(&tmp3, sizeof(uint64_t), 1, qg8f->fp);
		}
		else
		{
			/*
			   (
			   index type size * ranks
			   +
			   data type size * re/im
			   )
			   *
			   number of elements
			   +
			   index type size * ranks
			   +
			   bytes in the tensor header
		    */
			if (tensor->dtype_id == QG8_DTYPE_COMPLEX64 ||
			    tensor->dtype_id == QG8_DTYPE_COMPLEX128)
				tmp2 = 2;
			else
				tmp2 = 1;
			tmp3 = (((tmp4 * tensor->rank) + (data_size * tmp2)) *
			       tensor->num_elems) + (tmp4 * tensor->rank) +
			       sizeof(qg8_tensor_header);
			fwrite(&tmp3, sizeof(uint64_t), 1, qg8f->fp);

			/* tensor header */
			fwrite(&tensor->packing, sizeof(tensor->packing), 1, qg8f->fp);
			fwrite(&tensor->itype_id, sizeof(uint8_t), 1, qg8f->fp);
			fwrite(&tensor->dtype_id, sizeof(uint8_t), 1, qg8f->fp);
			fwrite(&tensor->rank, sizeof(tensor->rank), 1, qg8f->fp);
			fwrite(blank, 3, 1, qg8f->fp);
			for (i = 0; i < tensor->rank; ++i)
				fwrite(tensor->dimensions+i, tmp4, 1, qg8f->fp);
			fwrite(&tensor->num_elems, sizeof(tensor->num_elems), 1, qg8f->fp);

			/* tensor data */
			for (i = 0; i < tensor->rank; ++i)
			{
				if (tmp4 == QG8_SIZE_64)
				{
					fwrite(*(tensor->indices+i), tmp4,
					       tensor->num_elems, qg8f->fp);
				}
				else if (tmp4 == QG8_SIZE_32)
				{
					shape32 = _shape64_32(*(tensor->indices+i),
					                      tensor->num_elems);
					fwrite(shape32, tmp4,
					       tensor->num_elems, qg8f->fp);
					free(shape32);
				}
				else if (tmp4 == QG8_SIZE_16)
				{
					shape16 = _shape64_16(*(tensor->indices+i),
					                      tensor->num_elems);
					fwrite(shape16, tmp4,
					       tensor->num_elems, qg8f->fp);
					free(shape16);
				}
				else if (tmp4 == QG8_SIZE_8)
				{
					shape8 = _shape64_8(*(tensor->indices+i),
					                    tensor->num_elems);
					fwrite(shape8, tmp4,
					       tensor->num_elems, qg8f->fp);
					free(shape8);
				}
			}
			switch (tensor->dtype_id)
			{
			case QG8_DTYPE_FLOAT64:
			case QG8_DTYPE_COMPLEX128:
				fwrite(tensor->redouble, data_size, tensor->num_elems,
				       qg8f->fp);
				if (tensor->dtype_id == QG8_DTYPE_COMPLEX128)
				{
					fwrite(tensor->imdouble, data_size,
					       tensor->num_elems, qg8f->fp);
				}
				break;
			case QG8_DTYPE_FLOAT32:
			case QG8_DTYPE_COMPLEX64:
				fwrite(tensor->refloat, data_size, tensor->num_elems, qg8f->fp);
				if (tensor->dtype_id == QG8_DTYPE_COMPLEX64)
				{
					fwrite(tensor->imfloat, data_size,
					       tensor->num_elems, qg8f->fp);
				}
				break;
			case QG8_DTYPE_UINT8:
				fwrite(tensor->reu8, data_size, tensor->num_elems, qg8f->fp);
				break;
			case QG8_DTYPE_UINT16:
				fwrite(tensor->reu16, data_size, tensor->num_elems, qg8f->fp);
				break;
			case QG8_DTYPE_UINT32:
				fwrite(tensor->reu32, data_size, tensor->num_elems, qg8f->fp);
				break;
			case QG8_DTYPE_UINT64:
				fwrite(tensor->reu64, data_size, tensor->num_elems, qg8f->fp);
				break;
			case QG8_DTYPE_INT8:
				fwrite(tensor->rei8, data_size, tensor->num_elems, qg8f->fp);
				break;
			case QG8_DTYPE_INT16:
				fwrite(tensor->rei16, data_size, tensor->num_elems, qg8f->fp);
				break;
			case QG8_DTYPE_INT32:
				fwrite(tensor->rei32, data_size, tensor->num_elems, qg8f->fp);
				break;
			case QG8_DTYPE_INT64:
				fwrite(tensor->rei64, data_size, tensor->num_elems, qg8f->fp);
				break;
			default:
				fprintf(stderr, "Tried to write data with dtype %d to file.\n",
				        tensor->dtype_id);
				exit(EXIT_FAILURE);
			}
		}

		tlist = tlist->next;
	}

	/* flush file */
	fflush(qg8f->fp);
	return 1;
}

int
qg8_file_close(qg8_file *qg8f)
{
	qg8_chunk_linkedlist *tlist, *tmp;

	if (!qg8f)
	{
		fprintf(stderr, "Cannot close a NULL file.\n");
		exit(EXIT_FAILURE);
	}

	tlist = qg8f->chunks;
	while (tlist)
	{
		tmp = tlist->next;
		/* should be deleted manually */
		/*qg8_chunk_destroy(tlist->chunk);*/
		free(tlist);
		tlist = tmp;
	}
	fclose(qg8f->fp);
	free(qg8f);
	return 1;
}

