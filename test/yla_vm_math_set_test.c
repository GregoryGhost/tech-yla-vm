/*
    Virtual machine Math Test For Set #3

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
#include "../include/yla_vm.h"
//#include "../src/yla_vm.c"
#include "../include/yla_cop.h"
#include "../include/yla_test.h"
#include "../include/yla_test_gencode.h"
#include "../include/yla_type.h"
#include <stdlib.h>

/**
 * Positive test set
 * 
 * [2.11,3,4] 4 ?
 * [3,3.22,4] [3.234,3.22,3] *
 **/

/**
 * Negative test set
 * [3,2,4] 4 +
 * [3,2,4] [4] ?
 * [2,4,5] 11 *
 * 
 * 11 [2,3,4] *
 * 66 [3,2,6] +
 * 12 [12,2,4] ?
 **/
 
static int test_positive_union_of_sets_one_size()
{
	const size_t kCommd = 5;
	const size_t size_of_set = 3;
	const size_t count_sets = 2;
	size_t count_for_size_of_set = size_of_set * count_sets;
	size_t stack_size = size_of_set * count_sets;
	
	size_t sizePrg = kCommd + (count_for_size_of_set + stack_size) * sizeof(yla_number_type);
    yla_cop_type prg[HEADER_SIZE + sizePrg];
    yla_cop_type *ptr = prg;
    
	size_t interp_stack_size = count_sets * count_for_size_of_set;
	yla_number_type set1[] = {3.234, 4, 2.22};
	yla_number_type set2[] = {2, 3.3, 4};
	size_t size_of_rset;
	
	//NOTE: Пустая вм, ради получения кода ошибки
	yla_vm vm_empty;
	vm_empty.last_error = YLA_VM_ERROR_OK;
	yla_number_type *rSet = union_sets(&vm_empty, size_of_set, size_of_set, set1, set2, &size_of_rset);
	//printf("%s\n", yla_vm_error_message(yla_vm_last_error(&vm_empty)));
	YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm_empty) == YLA_VM_ERROR_OK, "excepted without error");
	char *stResult = format_set(size_of_rset, rSet);
	
	//NOTE: должно быть [3.234, 4, 2.22] [2, 3.3, 4] + = [2, 2.22, 3.234, 3.3, 4]
	//NOTE: но вот так  [3.234, 4, 2.22] [2, 3.3, 4] + = [3.234, 4, 2.22, 2, 3.3, 4]
    put_header_ext(&ptr, stack_size, interp_stack_size, 0, sizePrg);
    put_set(&ptr, size_of_set, set1);
    put_set(&ptr, size_of_set, set2);
    put_commd(&ptr, CADD);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg), "normal");
    //yla_vm_run(&vm);
    //printf("%s\n", yla_vm_error_message(yla_vm_last_error(&vm)));
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
    char *stL = yla_vm_last_output(&vm);
    YLATEST_ASSERT_TRUE(stL != NULL, "Expected last_output string");
    YLATEST_ASSERT_TRUE(strcmp(stL, stResult) == 0, "It was expected that the values would coincide");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    free(stResult);

    return 0;
}
  
static int test_positive_union_of_sets_other_size()
{
	const size_t kCommd = 5;
	const size_t size_of_set1 = 3;
	const size_t size_of_set2 = 5;
	const size_t count_sets = 2;
	size_t count_for_size_of_set = count_sets;
	size_t stack_size = size_of_set1 + size_of_set2;
	
	size_t sizePrg = kCommd + (count_for_size_of_set + stack_size) * sizeof(yla_number_type);
    yla_cop_type prg[HEADER_SIZE + sizePrg];
    yla_cop_type *ptr = prg;
    
	size_t interp_stack_size = count_sets * count_for_size_of_set;
	yla_number_type set1[] = {3.234, 4, 2.22};
	yla_number_type set2[] = {2, 3.3, 4, 12.28, 322.666};
	size_t size_of_rset;
	
	//NOTE: Пустая вм, ради получения кода ошибки
	yla_vm vm_empty;
	vm_empty.last_error = YLA_VM_ERROR_OK;
	yla_number_type *rSet = union_sets(&vm_empty, size_of_set1, size_of_set2, set1, set2, &size_of_rset);
	//printf("%s\n", yla_vm_error_message(yla_vm_last_error(&vm_empty)));
	YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm_empty) == YLA_VM_ERROR_OK, "excepted without error");
	char *stResult = format_set(size_of_rset, rSet);
	
	//NOTE: должно быть [3.234, 4, 2.22] [2, 3.3, 4, 12.28, 322.666] + = [2, 2.22, 3.234, 3.3, 4, 12.28, 322.666]
	//NOTE: но вот так  [3.234, 4, 2.22] [2, 3.3, 4, 12.28, 322.666] + = [2, 2.22, 3.234, 3.3, 4, 4, 12.28, 322.666]
    put_header_ext(&ptr, stack_size, interp_stack_size, 0, sizePrg);
    put_set(&ptr, size_of_set1, set1);
    put_set(&ptr, size_of_set2, set2);
    put_commd(&ptr, CADD);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg), "normal");
    //yla_vm_run(&vm);
    //printf("%s\n", yla_vm_error_message(yla_vm_last_error(&vm)));
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
    char *stL = yla_vm_last_output(&vm);
    YLATEST_ASSERT_TRUE(stL != NULL, "Expected last_output string");
    YLATEST_ASSERT_TRUE(strcmp(stL, stResult) == 0, "It was expected that the values would coincide");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    free(stResult);

    return 0;
}

static int test_negative_union_of_sets()
{

}

static int test_positive_include_of_sets()
{

}

static int test_negative_include_of_sets()
{

}

static int test_positive_intersection_of_sets()
{

}

static int test_negative_intersection_of_sets()
{

}
 
YLATEST_BEGIN(yla_vm_math_set_test)
   YLATEST_ADD_TEST_CASE(test_positive_union_of_sets_one_size)
   YLATEST_ADD_TEST_CASE(test_positive_union_of_sets_other_size)
   /*YLATEST_ADD_TEST_CASE(test_negative_union_of_sets)
   
   YLATEST_ADD_TEST_CASE(test_positive_include_of_sets)
   YLATEST_ADD_TEST_CASE(test_negative_include_of_sets)
   
   YLATEST_ADD_TEST_CASE(test_positive_intersection_of_sets)
   YLATEST_ADD_TEST_CASE(test_negative_intersection_of_sets)*/
YLATEST_END
