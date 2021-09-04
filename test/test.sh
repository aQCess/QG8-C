#!/bin/sh

#
# test.sh
# Library test suite.
#
# Author       : Finn Rayment <finn@rayment.fr>
# Date created : 25/07/2021
#

#
# Copyright 2021 University of Strasbourg
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

fail=0
total=0
passed=0

succeed_tests () {
	for i in $2; do
		echo "--- $i ---"
		total=$((total+1))
		./$1/$i
		if [ ! $? -eq 0 ];
		then
			fail=1
			printf "\e[1;31mFAILURE: \e[0mUnexpected failure occurred.\n"
		else
			passed=$((passed+1))
			printf "\e[1;32mSUCCESS: \e[0mTest passed successfully.\n"
		fi
	done
}

fail_tests () {
	for i in $2; do
		echo "--- $i ---"
		total=$((total+1))
		./$1/$i
		if [ ! $? -eq 1 ];
		then
			fail=1
			printf "\e[1;31mFAILURE: \e[0mUnexpected success occurred.\n"
		else
			passed=$((passed+1))
			printf "\e[1;32mSUCCESS: \e[0mTest failed successfully.\n"
		fi
	done
}

# to call a test program, use the following commands:
# succeed_tests <folder> <prog1> <prog2> <...>
#   for tests that must pass completely
# fail_tests <folder> <prog1> <prog2> <...>
#   for tests that must fail completely

# tensor tests
succeed_tests "tensor" "tensor_test"
fail_tests "tensor" "bad_tensor1 bad_tensor2 bad_tensor3 bad_tensor4"

# chunk tests
succeed_tests "chunk" "chunk_test"

# file tests
succeed_tests "file" "file_write file_read"

# graph tests
succeed_tests "graph" "graph_load" "graph_create"

echo "-- $passed/$total tests passed --"

