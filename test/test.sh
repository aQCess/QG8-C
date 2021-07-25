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

# list of tests that must succeed
tests="tensor_test"
# lsit of tests that must fail
anti_tests="bad_tensor1 bad_tensor2 bad_tensor3 bad_tensor4"

for i in $tests; do
	echo "--- $i ---"
	total=$((total+1))
	./tensor/$i
	if [ ! $? -eq 0 ];
	then
		fail=1
		printf "\e[1;31mFAILURE: \e[0mUnexpected failure occurred.\n"
	else
		passed=$((passed+1))
		printf "\e[1;32mSUCCESS: \e[0mTest passed successfully.\n"
	fi
done

for i in $anti_tests; do
	echo "--- $i ---"
	total=$((total+1))
	./tensor/$i
	if [ ! $? -eq 1 ];
	then
		fail=1
		printf "\e[1;31mFAILURE: \e[0mUnexpected success occurred.\n"
	else
		passed=$((passed+1))
		printf "\e[1;32mSUCCESS: \e[0mTest failed successfully.\n"
	fi
done

echo "-- $passed/$total tests passed --"

