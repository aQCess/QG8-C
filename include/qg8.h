/*
 * qg8.h
 * QG8 base library header file.
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

#ifndef _FR_RAYMENT_QG8_H_
#define _FR_RAYMENT_QG8_H_ 1

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdint.h>
#include <stdio.h>

/* File format */

#define QG8_VERSION                1
#define QG8_MAGIC                  "QG8ustra"

#define QG8_TYPE_ADJACENCY         1
#define QG8_TYPE_INPUT             2
#define QG8_TYPE_CONSTANT          3
#define QG8_TYPE_KET               4
#define QG8_TYPE_OPERATOR          5
#define QG8_TYPE_OBSERVABLE        6
#define QG8_TYPE_TIME              7
#define QG8_TYPE_TRACK             8
#define QG8_TYPE_NOISESPEC         9
#define QG8_TYPE_ADD               10
#define QG8_TYPE_SUBTRACT          11
#define QG8_TYPE_MATMUL            12
#define QG8_TYPE_JOIN              13
#define QG8_TYPE_SOLVE             14
#define QG8_TYPE_EXPECTATIONVALUE  15
#define QG8_TYPE_SAMPLE            16

#define QG8_SIZE_8                 1
#define QG8_SIZE_16                2
#define QG8_SIZE_32                4
#define QG8_SIZE_64                8

#define QG8_DTYPE_BOOL             1
#define QG8_DTYPE_CHAR             2
#define QG8_DTYPE_UINT8            3
#define QG8_DTYPE_UINT16           4
#define QG8_DTYPE_UINT32           5
#define QG8_DTYPE_UINT64           6
#define QG8_DTYPE_INT8             7
#define QG8_DTYPE_INT16            8
#define QG8_DTYPE_INT32            9
#define QG8_DTYPE_INT64            10
#define QG8_DTYPE_FLOAT32          11
#define QG8_DTYPE_FLOAT64          12
#define QG8_DTYPE_COMPLEX64        13
#define QG8_DTYPE_COMPLEX128       14

#define QG8_PACKING_FULL           1
#define QG8_PACKING_SPARSE_COO     2
#define QG8_PACKING_HALF_HERMITIAN 3

#define QG8_FLAG_LABEL             1

#define QG8_MODE_READ              1
#define QG8_MODE_WRITE             2
/*#define QG8_MODE_APPEND            3*/

typedef struct
qg8_file_header_s
{
	uint8_t signature[8];
	uint16_t version;
	uint8_t _reserved[6];
} __attribute__((__packed__))
qg8_file_header;

typedef struct
qg8_chunk_header_s
{
	uint16_t type;
	uint8_t flags;
	uint8_t id[16];
	uint8_t _reserved[5];
	uint64_t skip;
} __attribute__((__packed__))
qg8_chunk_header;

typedef struct
qg8_tensor_header_s
{
	uint8_t packing;
	uint8_t itype_id;
	uint8_t dtype_id;
	uint16_t rank;
	uint8_t _reserved[3];
	/*uint8_t *dims;*/
	uint64_t num_elements;
} __attribute__((__packed__))
qg8_tensor_header;

typedef struct
qg8_tensor_element_header_s
{
	uint8_t *pos;
	float *re;     /* real part */
	float *im;     /* imaginary part */
} __attribute__((__packed__))
qg8_tensor_element_header;

/* Tensors */

typedef struct
qg8_tensor_s
{
	uint8_t packing;
	uint8_t itype_id;
	uint8_t dtype_id;
	uint16_t rank;
	uint8_t loaded;
	uint64_t *dimensions;
	uint64_t num_elems;
	uint64_t **indices;
	double *redouble;
	double *imdouble;
	float *refloat;
	float *imfloat;
	uint8_t *reu8;
	uint16_t *reu16;
	uint32_t *reu32;
	uint64_t *reu64;
	int8_t *rei8;
	int16_t *rei16;
	int32_t *rei32;
	int64_t *rei64;
} qg8_tensor;

qg8_tensor *qg8_tensor_create_float(uint64_t **, float *, float *, uint64_t,
                                    uint64_t *, uint8_t, uint8_t);

qg8_tensor *qg8_tensor_create_double(uint64_t **, double *, double *, uint64_t,
                                     uint64_t *, uint8_t, uint8_t);

qg8_tensor *qg8_tensor_create_uint8(uint64_t **, uint8_t *, uint64_t,
                                    uint64_t *, uint8_t, uint8_t);

qg8_tensor *qg8_tensor_create_uint16(uint64_t **, uint16_t *, uint64_t,
                                     uint64_t *, uint8_t, uint8_t);

qg8_tensor *qg8_tensor_create_uint32(uint64_t **, uint32_t *, uint64_t,
                                     uint64_t *, uint8_t, uint8_t);

qg8_tensor *qg8_tensor_create_uint64(uint64_t **, uint64_t *, uint64_t,
                                     uint64_t *, uint8_t, uint8_t);

qg8_tensor *qg8_tensor_create_int8(uint64_t **, int8_t *, uint64_t,
                                   uint64_t *, uint8_t, uint8_t);

qg8_tensor *qg8_tensor_create_int16(uint64_t **, int16_t *, uint64_t,
                                    uint64_t *, uint8_t, uint8_t);

qg8_tensor *qg8_tensor_create_int32(uint64_t **, int32_t *, uint64_t,
                                     uint64_t *, uint8_t, uint8_t);

qg8_tensor *qg8_tensor_create_int64(uint64_t **, int64_t *, uint64_t,
                                     uint64_t *, uint8_t, uint8_t);

int qg8_tensor_destroy(qg8_tensor *);

uint16_t qg8_tensor_get_rank(qg8_tensor *);

uint64_t **qg8_tensor_get_indices(qg8_tensor *);

uint64_t qg8_tensor_get_num_elems(qg8_tensor *);

uint8_t qg8_tensor_get_dtypeid(qg8_tensor *);

uint8_t qg8_tensor_get_itypeid(qg8_tensor *);

void *qg8_tensor_get_dims(qg8_tensor *);

void *qg8_tensor_get_re(qg8_tensor *);

void *qg8_tensor_get_im(qg8_tensor *);

/* Adjacency matrices */

typedef qg8_tensor qg8_adjacencymatrix;

/* Chunks */

typedef struct
qg8_chunk_s
{
	qg8_tensor *tensor;
	uint16_t type;
	uint8_t flags;
	uint8_t string_id[16];
} qg8_chunk;

typedef struct
qg8_chunk_linkedlist_s
{
	qg8_chunk *chunk;
	struct qg8_chunk_linkedlist_s *next;
} qg8_chunk_linkedlist;

qg8_chunk *qg8_chunk_create(uint16_t, uint8_t, uint8_t *, qg8_tensor *);

int qg8_chunk_destroy(qg8_chunk *);

qg8_tensor *qg8_chunk_get_tensor(qg8_chunk *);

uint8_t qg8_chunk_get_flags(qg8_chunk *);

uint8_t *qg8_chunk_get_string_id(qg8_chunk *);

uint16_t qg8_chunk_get_type(qg8_chunk *);

/* Graph */

/* forward declaration */
typedef struct
qg8_file_s
{
	FILE *fp;
	int mode;
	qg8_chunk_linkedlist *chunks;
} qg8_file;

typedef struct
qg8_graph_s
{
	qg8_chunk_linkedlist *chunks;
	qg8_adjacencymatrix *adj;
	qg8_chunk *adjchunk;
} qg8_graph;

qg8_graph *qg8_graph_load(const char *);

qg8_graph *qg8_graph_create(void);

int qg8_graph_write(const char *, qg8_graph *);

int qg8_graph_destroy(qg8_graph *);

uint8_t qg8_graph_get_datasize(qg8_graph *);

/* TODO */
qg8_adjacencymatrix *qg8_graph_get_edges(qg8_graph *);

uint64_t qg8_graph_get_number_chunks(qg8_graph *);

qg8_chunk *qg8_graph_get_chunk(qg8_graph *, uint64_t);

int qg8_graph_remove_chunk(qg8_graph *, qg8_chunk *);

int qg8_graph_add_chunk(qg8_graph *, qg8_chunk *);

/* File I/O */

qg8_file *qg8_file_open(const char *, int);

qg8_tensor *qg8_file_read(qg8_file *, uint64_t *);

int qg8_file_write_chunk(qg8_file *, qg8_chunk *);

int qg8_file_flush(qg8_file *);

int qg8_file_close(qg8_file *);

/* Iterators */

typedef struct
qg8_iter_s
{
	size_t offset, nextoffset;
	qg8_file *f;
	int done_read;
} qg8_iter;

qg8_iter qg8_file_iterator(qg8_file *);

int qg8_file_has_next(qg8_iter *);

int qg8_file_next(qg8_iter *);

qg8_chunk *qg8_file_extract(qg8_iter *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _FR_RAYMENT_QG8_H_ */

