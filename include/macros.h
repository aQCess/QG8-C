/*
 * macros.h
 * Common macro header.
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

#ifndef _FR_RAYMENT_MACROS_H_
#define _FR_RAYMENT_MACROS_H_ 1

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define STRINGIFY(x) #x

#define READNN(x,y,z,w) _size_check(fread(x, y, z, w), z, __LINE__)
#define READN(x,y,z) _size_check(fread(x, 1, y, z), y, __LINE__)
#define READ(x,y,z) _size_check(fread(x, 1, sizeof(y), z), sizeof(y), __LINE__)

#define DIE(x) fprintf(stderr, x); exit(EXIT_FAILURE);

#define ALLOC(x) if (!x) { perror("malloc"); exit(EXIT_FAILURE); }

#define SHAPER(x,y,z) \
static \
y * \
_shape##x(z *data, \
         size_t size) \
{ \
	y *shape##x; \
	size_t i; \
	shape##x = (y *) malloc(sizeof(y) * size); \
	ALLOC(shape##x); \
	for (i = 0; i < size; ++i) \
		*(shape##x+i) = (y) *(data+i); \
	return shape##x; \
}

void _size_check(size_t, size_t, int);
uint8_t _type_to_size(uint8_t);

#endif /* _FR_RAYMENT_MACROS_H_ */

