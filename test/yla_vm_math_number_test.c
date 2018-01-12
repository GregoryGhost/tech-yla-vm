/*
    Virtual machine Math Test For Numbers #2

    This file is part of YLA VM (Yet another Language for Academic purpose: Virtual Machine).

    YLA VM is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "yla_vm_math_test_implementation.c"


YLATEST_BEGIN(yla_vm_math_number_test)
   //Add tests cases
   YLATEST_ADD_TEST_CASE(test_positive_add_two_numbers)
   YLATEST_ADD_TEST_CASE(test_negative_add_two_numbers)
   YLATEST_ADD_TEST_CASE(test_positive_add_some_numbers)
   YLATEST_ADD_TEST_CASE(test_negative_add_some_numbers)
   
   //Sub tests cases
   YLATEST_ADD_TEST_CASE(test_positive_sub_two_numbers)
   YLATEST_ADD_TEST_CASE(test_negative_sub_two_numbers)
   YLATEST_ADD_TEST_CASE(test_positive_sub_some_numbers)
   YLATEST_ADD_TEST_CASE(test_negative_sub_some_numbers)
   
   //Mult tests cases
   YLATEST_ADD_TEST_CASE(test_positive_mult_two_numbers)
   YLATEST_ADD_TEST_CASE(test_negative_mult_two_numbers)
   YLATEST_ADD_TEST_CASE(test_positive_mult_some_numbers)
   YLATEST_ADD_TEST_CASE(test_negative_mult_some_numbers)
   
   //Div tests cases
   YLATEST_ADD_TEST_CASE(test_positive_div_two_numbers)
   YLATEST_ADD_TEST_CASE(test_negative_div_two_numbers)
   YLATEST_ADD_TEST_CASE(test_positive_div_some_numbers)
   YLATEST_ADD_TEST_CASE(test_negative_div_some_numbers)
   YLATEST_ADD_TEST_CASE(test_div_on_zero_numbers)
YLATEST_END
