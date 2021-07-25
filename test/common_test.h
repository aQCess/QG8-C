/*
 * common_test.h
 * Common test suite defines.
 *
 * Author       : Finn Rayment <finn@rayment.fr>
 * Date created : 25/03/2021
 */

/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2021, Finn Rayment
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RAYMENT_FR_TEST_COMMON_TEST_H
#define _RAYMENT_FR_TEST_COMMON_TEST_H 1

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RED    "\x1b[1;31m"
#define YELLOW "\x1b[1;33m"
#define GREEN  "\x1b[1;32m"
#define BOLD  "\x1b[1m"
#define RESET  "\x1b[0m"

#define INIT() int clockiter; clock_t tic, toc;\
               double tmptime, totaltime, besttime, worsttime, avgtime;\
               (void) clockiter;\
               (void) tic;\
               (void) toc;\
               (void) tmptime;\
               (void) totaltime;\
               (void) besttime;\
               (void) worsttime;\
               (void) avgtime;

#define FAIL(x,y) if (!(x)) {\
                      fprintf(stdout, "\r" RED "FAILURE: " RESET "%s\n", y);\
                      return EXIT_FAILURE;\
                  } else {\
                      fprintf(stdout, "\r" GREEN "SUCCESS: " RESET "%s\n", y);\
                  }
#define TEST(x,y,z) fprintf(stdout, "TEST: %s", z); fflush(stdout); x FAIL(y,z)

#define TIME(x,y,z) clockiter = 0;\
                    totaltime = 0; besttime = 0; worsttime = 0, avgtime = 0;\
                    while (clockiter++ < z) {\
                        tic = clock(); x toc = clock();\
                        tmptime = (double) (toc - tic) / CLOCKS_PER_SEC;\
                        avgtime += tmptime;\
                        if (clockiter == 1 || tmptime < besttime)\
                            besttime = tmptime;\
                        if (clockiter == 1 || tmptime > worsttime)\
                            worsttime = tmptime;\
                    }\
                    totaltime = avgtime; avgtime /= z;\
                    fprintf(stdout, YELLOW "BENCHMARK " RESET "(%s):\n"\
                            "          total: "\
                            BOLD "%fs" RESET ", avg: "\
                            BOLD "%fs" RESET ", best: "\
                            BOLD "%fs" RESET ", worst: "\
                            BOLD "%fs" RESET ", iters: %d\n",\
                            y, totaltime, avgtime, besttime, worsttime, z);

#define PASS() exit(EXIT_SUCCESS);

#endif /* _RAYMENT_FR_TEST_COMMON_TEST_H */

