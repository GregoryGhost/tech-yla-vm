/*
    Virtual machine Test #1

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

#include "../src/yla_vm.c"
#include "../include/yla_cop.h"
#include "../include/yla_test.h"
#include "../include/yla_test_gencode.h"

static int test_init_null()
{
    yla_vm vm;
    size_t sizePrg = 100;
    yla_cop_type buf[sizePrg];
	
    YLATEST_ASSERT_FALSE(yla_vm_init(&vm, NULL, sizePrg), "init NULL");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm)==YLA_VM_ERROR_NO_PROGRAM_CODE, "init NULL");
    
    return 0;
}

static int test_init_0()
{
	size_t sizePrg = 100;
    yla_cop_type code[sizePrg];
    yla_vm vm;

    YLATEST_ASSERT_FALSE(yla_vm_init(&vm, code, sizePrg), "init 0 size code");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm)==YLA_VM_ERROR_NO_PROGRAM_CODE, "init 0 size code");

    return 0;
}

static int test_init_simple()
{
	size_t sizePrg = 1;
    yla_cop_type prg[HEADER_SIZE + sizePrg];
    yla_cop_type *ptr = prg;

    put_header(&ptr, 0, 0, sizePrg);
    put_commd(&ptr, CHALT);

    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_do_command(&vm) == -1, "halt expected");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_init_simple2()
{
	size_t sizePrg = 2;
    yla_cop_type prg[HEADER_SIZE + sizePrg];
    yla_cop_type *ptr = prg;

    put_header(&ptr, 0, 0, sizePrg);
    put_commd(&ptr, CNOP);
    put_commd(&ptr, CHALT);

    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_do_command(&vm) == 1, "OK expected");
    YLATEST_ASSERT_TRUE(yla_vm_do_command(&vm) == -1, "halt expected");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_init_simple_run()
{
	size_t sizePrg = 2;
    yla_cop_type prg[HEADER_SIZE + sizePrg];
    yla_cop_type *ptr = prg;

    put_header(&ptr, 0, 0, sizePrg);
    put_commd(&ptr, CNOP);
    put_commd(&ptr, CHALT);

    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_push_number()
{
	const size_t kCommd = 3;
	size_t sizePrg = kCommd + sizeof(yla_number_type);
    yla_cop_type prg[HEADER_SIZE + sizePrg];
    yla_cop_type *ptr = prg;
    
	const size_t stackSize = 1;
	yla_number_type tResult = 3.2223;
	char *stResult = format_number(tResult);

    put_header(&ptr, stackSize, 0, sizePrg);
    put_number(&ptr, tResult);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    yla_vm vm;
    
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_OK, "normal");
    char *stL = yla_vm_last_output(&vm);
    YLATEST_ASSERT_TRUE(strcmp(stL, stResult) == 0, "It was expected that the values would coincide");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    free(stResult);

    return 0;
}

static int test_negative_output()
{
	const size_t kCommd = 2;
	size_t sizePrg = kCommd + 0*sizeof(yla_number_type);
    yla_cop_type prg[HEADER_SIZE + sizePrg];
    yla_cop_type *ptr = prg;
    
	const size_t stackSize = 0;

    put_header(&ptr, stackSize, 0, sizePrg);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg), "normal");
    YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_INTERP_STACK_EMPTY, "Excepted error in work COUT command");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}


static int test_push_set()
{
	const size_t kCommd = 3;
	const size_t count_sets = 1;
	size_t size_of_set = 3;
	size_t sizePrg = kCommd + (size_of_set + count_sets) * sizeof(yla_number_type);
    yla_cop_type prg[HEADER_SIZE + sizePrg];
    yla_cop_type *ptr = prg;
    
	const size_t stack_size = size_of_set;
	size_t interp_stack_size = count_sets * 2;
	yla_number_type tResult[] = {3.22, 1.234, 2.228};
	char *stResult = format_set(size_of_set, tResult);

    put_header_ext(&ptr, stack_size, interp_stack_size, 0, sizePrg);
    put_set(&ptr, size_of_set, tResult);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    yla_vm vm;
    
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
    //yla_vm_run(&vm);
    //printf("%s\n", yla_vm_error_message(yla_vm_last_error(&vm)));
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_OK, "normal");
    char *stL = yla_vm_last_output(&vm);
    //printf("stL=%s, stR=%s\n", stL, stResult);
    YLATEST_ASSERT_TRUE(strcmp(stL, stResult) == 0, "It was expected that the values would coincide");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    free(stResult);

    return 0;
}

static int test_get_interp_stack_full()
{
	const int kNop = 10;
	
	int sizePrg = 2 + kNop;
	
    yla_cop_type prg[HEADER_SIZE + sizePrg];
    yla_cop_type *ptr = prg;

    put_header(&ptr, 0, 0, sizePrg);
    put_commd(&ptr, CPUSH);
    for(int i = 0; i <= sizePrg; i++){
		put_commd(&ptr, CNOP);
	}
    put_commd(&ptr, CHALT);

    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg), "normal");
    YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_INTERP_STACK_FULL, "incorrect error code");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_get_stack_full()
{
	const int kNop = 10;
	
	int sizePrg = 2 + kNop;
	
    yla_cop_type prg[HEADER_SIZE + sizePrg];
    size_t interp_stack_size = 1;
    yla_cop_type *ptr = prg;

    
    put_header_ext(&ptr, 0, interp_stack_size, 0, sizePrg);
    put_commd(&ptr, CPUSH);
    for(int i = 0; i <= sizePrg; i++){
		put_commd(&ptr, CNOP);
	}
    put_commd(&ptr, CHALT);

    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg), "normal");
    YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "normal");
    //printf("%s\n", yla_vm_error_message(yla_vm_last_error(&vm)));
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_STACK_FULL, "incorrect error code");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_code_limit()
{
    yla_cop_type prg[HEADER_SIZE + 1];
    yla_cop_type *ptr = prg;

    put_header(&ptr, 0, 0, 1);
    put_commd(&ptr, CNOP);

    yla_vm vm;

    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + 1), "normal");
    YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_CODE_SEG_EXCEED, "incorrect error code");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");

    return 0;
}

static int test_code_get_value_internal_from_program(){
	const size_t kCommd = 2;
	size_t sizePrg = kCommd + sizeof(yla_number_type);
    yla_cop_type prg[HEADER_SIZE + sizePrg];
    yla_cop_type *ptr = prg;
	const size_t stackSize = 1;
	yla_number_type tresult = 6234.234;
	
    put_header(&ptr, stackSize, 0, sizePrg);
    put_number(&ptr, tresult);
    put_commd(&ptr, CHALT);

    
    yla_number_type exceptedResult = yla_vm_get_value_internal(prg+HEADER_SIZE+1);
    YLATEST_ASSERT_TRUE(exceptedResult == tresult, "It was expected that the values would coincide");

    return 0;
}

YLATEST_BEGIN(yla_vm_test)
   YLATEST_ADD_TEST_CASE(test_init_null)
   YLATEST_ADD_TEST_CASE(test_init_0)
   YLATEST_ADD_TEST_CASE(test_init_simple)
   YLATEST_ADD_TEST_CASE(test_init_simple2)
   YLATEST_ADD_TEST_CASE(test_init_simple_run)
   YLATEST_ADD_TEST_CASE(test_push_number)
   YLATEST_ADD_TEST_CASE(test_push_set)
   YLATEST_ADD_TEST_CASE(test_negative_output)
   YLATEST_ADD_TEST_CASE(test_get_interp_stack_full)
   YLATEST_ADD_TEST_CASE(test_get_stack_full)
   YLATEST_ADD_TEST_CASE(test_code_limit)
   YLATEST_ADD_TEST_CASE(test_code_get_value_internal_from_program)
YLATEST_END
