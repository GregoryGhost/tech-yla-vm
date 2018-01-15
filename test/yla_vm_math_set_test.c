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
 
static int test_positive_union_of_sets_one_size()
{
	const size_t kCommd = 5;
	const size_t size_of_set = 3;
	const size_t count_sets = 2;
	size_t count_for_size_of_set =  count_sets;
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
	
	//[3.234, 4, 2.22] [2, 3.3, 4] + = [2, 2.22, 3.234, 3.3, 4]
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
	
	//[3.234, 4, 2.22] [2, 3.3, 4, 12.28, 322.666] + = [2, 2.22, 3.234, 3.3, 4, 12.28, 322.666]
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
	const size_t kCommd = 5;
	const size_t size_of_set = 3;
	const size_t count_sets = 1;
	const size_t count_number = 1;
	size_t count_for_size_of_set = count_sets;
	size_t stack_size = size_of_set + count_number;
	
	size_t sizePrg = kCommd + (count_for_size_of_set + stack_size) * sizeof(yla_number_type);
    yla_cop_type prg[HEADER_SIZE + sizePrg];
    yla_cop_type *ptr = prg;
    
	size_t interp_stack_size = count_sets + count_for_size_of_set + count_number;
	yla_number_type set1[] = {3.234, 4, 2.22};
	yla_number_type number = 32.22;
	size_t size_of_rset;
	
	//NOTE: [3.234, 4, 2.22] 32.22 + = YLA_VM_ERROR_INTERP_STACK_UNKNOWN_COMMAND
    put_header_ext(&ptr, stack_size, interp_stack_size, 0, sizePrg);
    put_set(&ptr, size_of_set, set1);
    put_number(&ptr, number);
    put_commd(&ptr, CADD);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg), "normal");
    //yla_vm_run(&vm);
    YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "normal");
    //printf("%s\n", yla_vm_error_message(yla_vm_last_error(&vm)));
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_INTERP_STACK_UNKNOWN_COMMAND, "Excepted interp unknown command");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_positive_include_of_sets()
{
	const size_t kCommd = 5;
	const size_t size_of_set = 3;
	const size_t count_sets = 1;
	const size_t count_number = 1;
	size_t count_for_size_of_set =  count_sets;
	size_t stack_size = size_of_set + count_number;
	
	size_t sizePrg = kCommd + (count_for_size_of_set + stack_size) * sizeof(yla_number_type);
    yla_cop_type prg[HEADER_SIZE + sizePrg];
    yla_cop_type *ptr = prg;
    
	size_t interp_stack_size = count_sets + count_for_size_of_set + count_number;
	yla_number_type set1[] = {3.234, 4, 2.22};
	yla_number_type number = 4;
	
	//NOTE: Пустая вм, ради получения кода ошибки
	yla_vm vm_empty;
	vm_empty.last_error = YLA_VM_ERROR_OK;
	yla_number_type result = include_of_set(&vm_empty, size_of_set, set1, number);
	//printf("%s\n", yla_vm_error_message(yla_vm_last_error(&vm_empty)));
	YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm_empty) == YLA_VM_ERROR_OK, "excepted without error");
	char *stResult = format_number(result);
	
	//NOTE: [3.234, 4, 2.22] 4 ? = 1
    put_header_ext(&ptr, stack_size, interp_stack_size, 0, sizePrg);
    put_set(&ptr, size_of_set, set1);
    put_number(&ptr, number);
    put_commd(&ptr, CINCLUDE);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg), "normal");
    //yla_vm_run(&vm);
    //printf("%s\n", yla_vm_error_message(yla_vm_last_error(&vm)));
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
    char *stL = yla_vm_last_output(&vm);
    YLATEST_ASSERT_TRUE(strcmp(stL, stResult) == 0, "It was expected that the values would coincide");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    free(stResult);

    return 0;
}

static int test_negative_include_set_in_set()
{
	const size_t kCommd = 5;
	const size_t size_of_set1 = 3;
	const size_t size_of_set2 = 1;
	const size_t count_sets = 2;
	size_t count_for_size_of_set =  count_sets;
	size_t stack_size = size_of_set1 + size_of_set2;
	
	size_t sizePrg = kCommd + (count_for_size_of_set + stack_size) * sizeof(yla_number_type);
    yla_cop_type prg[HEADER_SIZE + sizePrg];
    yla_cop_type *ptr = prg;
    
	size_t interp_stack_size = count_sets * count_for_size_of_set;
	yla_number_type set1[] = {3.234, 4, 2.22};
	yla_number_type set2[] = {4};
	
	//NOTE: [3,2,4] [4] ? = YLA_VM_ERROR_INTERP_STACK_UNKNOWN_COMMAND
    put_header_ext(&ptr, stack_size, interp_stack_size, 0, sizePrg);
    put_set(&ptr, size_of_set1, set1);
    put_set(&ptr, size_of_set2, set2);
    put_commd(&ptr, CINCLUDE);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg), "normal");
    //yla_vm_run(&vm);
    //printf("%s\n", yla_vm_error_message(yla_vm_last_error(&vm)));
    YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "not normal");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_INTERP_STACK_UNKNOWN_COMMAND, "Excepted interp unknown command");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_negative_include_set_in_number()
{
	const size_t kCommd = 5;
	const size_t size_of_set = 3;
	const size_t count_sets = 1;
	const size_t count_number = 1;
	size_t count_for_size_of_set =  count_sets;
	size_t stack_size = size_of_set + count_number;
	
	size_t sizePrg = kCommd + (count_for_size_of_set + stack_size) * sizeof(yla_number_type);
    yla_cop_type prg[HEADER_SIZE + sizePrg];
    yla_cop_type *ptr = prg;
    
	size_t interp_stack_size = count_sets + count_for_size_of_set + count_number;
	yla_number_type set1[] = {3.234, 4, 2.22};
	yla_number_type number = 4;
	
	//NOTE: 4 [3,2,4] ? = YLA_VM_ERROR_INTERP_STACK_UNKNOWN_COMMAND
    put_header_ext(&ptr, stack_size, interp_stack_size, 0, sizePrg);
    put_number(&ptr, number);
    put_set(&ptr, size_of_set, set1);
    put_commd(&ptr, CINCLUDE);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg), "normal");
    //yla_vm_run(&vm);
    //printf("%s\n", yla_vm_error_message(yla_vm_last_error(&vm)));
    YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "not normal");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_INTERP_STACK_UNKNOWN_COMMAND, "Excepted interp unknown command");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_positive_intersection_of_sets_one_size()
{
	const size_t kCommd = 5;
	const size_t size_of_set = 3;
	const size_t count_sets = 2;
	size_t count_for_size_of_set =  count_sets;
	size_t stack_size = size_of_set * count_sets;
	
	size_t sizePrg = kCommd + (count_for_size_of_set + stack_size) * sizeof(yla_number_type);
    yla_cop_type prg[HEADER_SIZE + sizePrg];
    yla_cop_type *ptr = prg;
    
	size_t interp_stack_size = count_sets * count_for_size_of_set;
	yla_number_type set1[] = {3, 3.22, 3};
	yla_number_type set2[] = {3.234, 3.22, 3};
	size_t size_of_rset;
	
	//NOTE: Пустая вм, ради получения кода ошибки
	yla_vm vm_empty;
	vm_empty.last_error = YLA_VM_ERROR_OK;
	yla_number_type *result = intersection_of_sets(&vm_empty, size_of_set, size_of_set, set1, set2, &size_of_rset);
	//printf("%s\n", yla_vm_error_message(yla_vm_last_error(&vm_empty)));
	YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm_empty) == YLA_VM_ERROR_OK, "excepted without error");
	char *stResult = format_set(size_of_rset, result);
	
	//[3, 3.22, 3] [3.234, 3.22, 3] * = [3, 3.22]
    put_header_ext(&ptr, stack_size, interp_stack_size, 0, sizePrg);
    put_set(&ptr, size_of_set, set1);
    put_set(&ptr, size_of_set, set2);
    put_commd(&ptr, CMULT);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg), "normal");
    //yla_vm_run(&vm);
    //printf("%s\n", yla_vm_error_message(yla_vm_last_error(&vm)));
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
    char *stL = yla_vm_last_output(&vm);
    YLATEST_ASSERT_TRUE(strcmp(stL, stResult) == 0, "It was expected that the values would coincide");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    free(stResult);

    return 0;
}

static int test_positive_intersection_of_sets_other_size()
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
	yla_number_type set2[] = {2, 3.3, 4, 4, 322.666};
	size_t size_of_rset;
	
	//NOTE: Пустая вм, ради получения кода ошибки
	yla_vm vm_empty;
	vm_empty.last_error = YLA_VM_ERROR_OK;
	yla_number_type *rSet = intersection_of_sets(&vm_empty, size_of_set1, size_of_set2, set1, set2, &size_of_rset);
	//printf("%s\n", yla_vm_error_message(yla_vm_last_error(&vm_empty)));
	YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm_empty) == YLA_VM_ERROR_OK, "excepted without error");
	char *stResult = format_set(size_of_rset, rSet);
	
	//[3.234, 4, 2.22] [2, 3.3, 4, 4, 322.666] * = [4]
    put_header_ext(&ptr, stack_size, interp_stack_size, 0, sizePrg);
    put_set(&ptr, size_of_set1, set1);
    put_set(&ptr, size_of_set2, set2);
    put_commd(&ptr, CMULT);
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

static int test_negative_intersection_of_sets()
{
	const size_t kCommd = 5;
	const size_t size_of_set = 3;
	const size_t count_sets = 1;
	const size_t count_number = 1;
	size_t count_for_size_of_set = count_sets;
	size_t stack_size = size_of_set + count_number;
	
	size_t sizePrg = kCommd + (count_for_size_of_set + stack_size) * sizeof(yla_number_type);
    yla_cop_type prg[HEADER_SIZE + sizePrg];
    yla_cop_type *ptr = prg;
    
	size_t interp_stack_size = count_sets + count_for_size_of_set + count_number;
	yla_number_type set1[] = {3.234, 4, 2.22};
	yla_number_type number = 32.22;
	size_t size_of_rset;
	
	//NOTE: [3.234, 4, 2.22] 32.22 + = YLA_VM_ERROR_INTERP_STACK_UNKNOWN_COMMAND
    put_header_ext(&ptr, stack_size, interp_stack_size, 0, sizePrg);
    put_set(&ptr, size_of_set, set1);
    put_number(&ptr, number);
    put_commd(&ptr, CMULT);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg), "normal");
    //yla_vm_run(&vm);
    YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "normal");
    //printf("%s\n", yla_vm_error_message(yla_vm_last_error(&vm)));
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_INTERP_STACK_UNKNOWN_COMMAND, "Excepted interp unknown command");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}
 
YLATEST_BEGIN(yla_vm_math_set_test)
   YLATEST_ADD_TEST_CASE(test_positive_union_of_sets_one_size)
   YLATEST_ADD_TEST_CASE(test_positive_union_of_sets_other_size)
   YLATEST_ADD_TEST_CASE(test_negative_union_of_sets)
   
   YLATEST_ADD_TEST_CASE(test_positive_include_of_sets)
   YLATEST_ADD_TEST_CASE(test_negative_include_set_in_set)
   YLATEST_ADD_TEST_CASE(test_negative_include_set_in_number)
   
   YLATEST_ADD_TEST_CASE(test_positive_intersection_of_sets_one_size)
   YLATEST_ADD_TEST_CASE(test_positive_intersection_of_sets_other_size)
   YLATEST_ADD_TEST_CASE(test_negative_intersection_of_sets)
YLATEST_END
