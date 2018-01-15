/*
    Virtual machine Math Test Implementation For Numbers #2

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
#include "../include/yla_cop.h"
#include "../include/yla_test.h"
#include "../include/yla_test_gencode.h"
#include "../include/yla_type.h"
#include <stdlib.h>
 /* 
  * Add Tests
  */
static int test_positive_add_two_numbers()
{
    size_t sizePrg = 2 * sizeof(yla_number_type);
    size_t sizeCommd = 5;
    size_t stackSize = 2;
    yla_cop_type prg[HEADER_SIZE + sizePrg + sizeCommd];
    
    yla_number_type op1 = 3.22;
    yla_number_type op2 = 2.28;
    yla_number_type tresult = op1 + op2;
    char *stResult = format_number(tresult);
    yla_cop_type *ptr = prg;
    
    //3.22 2.28 + = 5.5
    put_header(&ptr, stackSize, 0, sizePrg + sizeCommd);
    put_number(&ptr, op1);    
    put_number(&ptr, op2);
    put_commd(&ptr, CADD);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
    char *exceptedResult = yla_vm_last_output(&vm);
    YLATEST_ASSERT_TRUE(strcmp(exceptedResult, stResult) == 0, "It was expected that the values would coincide");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    free(stResult);
    
    return 0;
}

static int test_negative_add_two_numbers()
{
    size_t sizePrg = 2 * sizeof(yla_number_type);
    size_t sizeCommd = 5;
    size_t stackSize = 2;
    yla_cop_type prg[HEADER_SIZE + sizePrg + sizeCommd];
    
    yla_number_type op1 = 3.22;
    yla_number_type op2 = 2.28;
    yla_cop_type *ptr = prg;
    
    //3.22 +(!) 2.28  = YLA_VM_ERROR_STACK_EMPTY
    put_header(&ptr, stackSize, 0, sizePrg + sizeCommd);
    put_number(&ptr, op1);
    put_commd(&ptr, CADD);    
    put_number(&ptr, op2);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
    YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_INTERP_STACK_EMPTY, "Expected error for wrong arguments CADD");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");  
    
    return 0;
}

static int test_positive_add_some_numbers()
{   
    const int lengthOps = 4;
    yla_number_type ops[] = {1.234, 3.22, 2.228, 5.28};
    yla_number_type tresult = ops[0];
    
    for(int i = 1; i < lengthOps; i++){
		tresult += ops[i];
	}
	
	char *stResult = format_number(tresult);
    
    size_t sizePrg = 4 * sizeof(yla_number_type);
    size_t sizeCommd = 9;
    size_t stackSize = 4;
    
    yla_cop_type prg[HEADER_SIZE + sizePrg + sizeCommd];
    yla_cop_type *ptr = prg;
    
    //1.234 3.22 + 2.228 + 5.28 + = tresult
    put_header(&ptr, stackSize, 0, sizePrg + sizeCommd);
	put_number(&ptr, ops[0]);
	put_number(&ptr, ops[1]);
	put_commd(&ptr, CADD);
	put_number(&ptr, ops[2]);
	put_commd(&ptr, CADD);
	put_number(&ptr, ops[3]);
	put_commd(&ptr, CADD);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
    char *stL = yla_vm_last_output(&vm);
    YLATEST_ASSERT_TRUE(strcmp(stL, stResult) == 0, "It was expected that the values would coincide");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    free(stResult);
    
    return 0;
}

static int test_negative_add_some_numbers()
{   
    yla_number_type ops[] = {1.234, 3.22, 2.228, 5.28};
    
    size_t sizePrg = 4 * sizeof(yla_number_type);
    size_t sizeCommd = 10;
    size_t stackSize = 4;
    
    yla_cop_type prg[HEADER_SIZE + sizePrg + sizeCommd];
    yla_cop_type *ptr = prg;
    
    //1.234 +(!) 3.22 + 2.228 + 5.28 + = YLA_VM_ERROR_STACK_EMPTY
    put_header(&ptr, stackSize, 0, sizePrg + sizeCommd);
	put_number(&ptr, ops[0]);
	put_commd(&ptr, CADD);
	put_number(&ptr, ops[1]);
	put_commd(&ptr, CADD);
	put_number(&ptr, ops[2]);
	put_commd(&ptr, CADD);
	put_number(&ptr, ops[3]);
	put_commd(&ptr, CADD);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
	YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_INTERP_STACK_EMPTY, "Expected error for wrong arguments CADD");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    return 0;
}

 /* 
  * Sub Tests
  */

static int test_positive_sub_two_numbers()
{
    size_t sizePrg = 2 * sizeof(yla_number_type);
    size_t sizeCommd = 5;
    size_t stackSize = 2;
    yla_cop_type prg[HEADER_SIZE + sizePrg + sizeCommd];
    
    yla_number_type op1 = 3.22;
    yla_number_type op2 = 2.28;
    yla_number_type tresult = op1 - op2;
    char *stResult = format_number(tresult);
    yla_cop_type *ptr = prg;
    
    //3.22 2.28 - = 0.94
    put_header(&ptr, stackSize, 0, sizePrg + sizeCommd);
    put_number(&ptr, op1);    
    put_number(&ptr, op2);
    put_commd(&ptr, CSUB);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
    char *exceptedResult = yla_vm_last_output(&vm);
    YLATEST_ASSERT_TRUE(strcmp(exceptedResult, stResult) == 0, "It was expected that the values would coincide");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    free(stResult);
    
    return 0;
}

static int test_negative_sub_two_numbers()
{
    size_t sizePrg = 2 * sizeof(yla_number_type);
    size_t sizeCommd = 5;
    size_t stackSize = 2;
    yla_cop_type prg[HEADER_SIZE + sizePrg + sizeCommd];
    
    yla_number_type op1 = 3.22;
    yla_number_type op2 = 2.28;
    yla_cop_type *ptr = prg;
    
    //3.22 -(!) 2.28 = YLA_VM_ERROR_STACK_EMPTY
    put_header(&ptr, stackSize, 0, sizePrg + sizeCommd);
    put_number(&ptr, op1); 
    put_commd(&ptr, CSUB);   
    put_number(&ptr, op2);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
	YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_STACK_EMPTY, "Expected error for wrong arguments CSUB");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    return 0;
}

static int test_positive_sub_some_numbers()
{   
    const int lengthOps = 4;
    yla_number_type ops[] = {1.234, 3.22, 2.228, 5.28};
    yla_number_type tresult = ops[0];
    
    for(int i = 1; i < lengthOps; i++){
		tresult -= ops[i];
	}
	
	char *stResult = format_number(tresult);
    
    size_t sizePrg = 4 * sizeof(yla_number_type);
    size_t sizeCommd = 9;
    size_t stackSize = 4;
    
    yla_cop_type prg[HEADER_SIZE + sizePrg + sizeCommd];
    yla_cop_type *ptr = prg;
    
    //1.234 3.22 - 2.228 - 5.28 - = tresult
    put_header(&ptr, stackSize, 0, sizePrg + sizeCommd);
	put_number(&ptr, ops[0]);
	put_number(&ptr, ops[1]);
	put_commd(&ptr, CSUB);
	put_number(&ptr, ops[2]);
	put_commd(&ptr, CSUB);
	put_number(&ptr, ops[3]);
	put_commd(&ptr, CSUB);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
    char *stL = yla_vm_last_output(&vm);
    YLATEST_ASSERT_TRUE(strcmp(stL, stResult) == 0, "It was expected that the values would coincide");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    free(stResult);
    
    return 0;
}

static int test_negative_sub_some_numbers()
{   
    yla_number_type ops[] = {1.234, 3.22, 2.228, 5.28};
    
    size_t sizePrg = 4 * sizeof(yla_number_type);
    size_t sizeCommd = 10;
    size_t stackSize = 4;
    
    yla_cop_type prg[HEADER_SIZE + sizePrg + sizeCommd];
    yla_cop_type *ptr = prg;
    
    //1.234 -(!) 3.22 - 2.228 - 5.28 - = YLA_VM_ERROR_STACK_EMPTY
    put_header(&ptr, stackSize, 0, sizePrg + sizeCommd);
	put_number(&ptr, ops[0]);
	put_commd(&ptr, CSUB);
	put_number(&ptr, ops[1]);
	put_commd(&ptr, CSUB);
	put_number(&ptr, ops[2]);
	put_commd(&ptr, CSUB);
	put_number(&ptr, ops[3]);
	put_commd(&ptr, CSUB);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
	YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_STACK_EMPTY, "Expected error for wrong arguments CSUB");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    return 0;
}
 /* 
  * Mult Tests
  */

static int test_positive_mult_two_numbers()
{
    size_t sizePrg = 2 * sizeof(yla_number_type);
    size_t sizeCommd = 5;
    size_t stackSize = 2;
    yla_cop_type prg[HEADER_SIZE + sizePrg + sizeCommd];
    
    yla_number_type op1 = 3.22;
    yla_number_type op2 = 2.28;
    yla_number_type tresult = op1 * op2;
    char *stResult = format_number(tresult);
    yla_cop_type *ptr = prg;
    
    //3.22 2.28 * = 7.3416
    put_header(&ptr, stackSize, 0, sizePrg + sizeCommd);
    put_number(&ptr, op1);    
    put_number(&ptr, op2);
    put_commd(&ptr, CMULT);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
    //yla_vm_run(&vm);
    //printf("%s\n", yla_vm_error_message(yla_vm_last_error(&vm)));
    char *exceptedResult = yla_vm_last_output(&vm);
    YLATEST_ASSERT_TRUE(strcmp(exceptedResult, stResult) == 0, "It was expected that the values would coincide");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    free(stResult);
    
    return 0;
}

static int test_negative_mult_two_numbers()
{
    size_t sizePrg = 2 * sizeof(yla_number_type);
    size_t sizeCommd = 5;
    size_t stackSize = 2;
    yla_cop_type prg[HEADER_SIZE + sizePrg + sizeCommd];
    
    yla_number_type op1 = 3.22;
    yla_number_type op2 = 2.28;
    yla_cop_type *ptr = prg;
    
    //3.22 *(!) 2.28 = YLA_VM_ERROR_STACK_EMPTY
    put_header(&ptr, stackSize, 0, sizePrg + sizeCommd);
    put_number(&ptr, op1); 
    put_commd(&ptr, CMULT);   
    put_number(&ptr, op2);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
	YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_INTERP_STACK_EMPTY, "Expected error for wrong arguments CMULT");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    return 0;
}

static int test_positive_mult_some_numbers()
{   
    const int lengthOps = 4;
    yla_number_type ops[] = {1.234, 3.22, 2.228, 5.28};
    yla_number_type tresult = ops[0];
    
    for(int i = 1; i < lengthOps; i++){
		tresult *= ops[i];
	}
	
	char *stResult = format_number(tresult);
    
    size_t sizePrg = 4 * sizeof(yla_number_type);
    size_t sizeCommd = 9;
    size_t stackSize = 4;
    
    yla_cop_type prg[HEADER_SIZE + sizePrg + sizeCommd];
    yla_cop_type *ptr = prg;
    
    //1.234 3.22 * 2.228 * 5.28 * = tresult
    put_header(&ptr, stackSize, 0, sizePrg + sizeCommd);
	put_number(&ptr, ops[0]);
	put_number(&ptr, ops[1]);
	put_commd(&ptr, CMULT);
	put_number(&ptr, ops[2]);
	put_commd(&ptr, CMULT);
	put_number(&ptr, ops[3]);
	put_commd(&ptr, CMULT);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
    //yla_vm_run(&vm);
    //printf("%s\n", yla_vm_error_message(yla_vm_last_error(&vm)));
    char *stL = yla_vm_last_output(&vm);
    YLATEST_ASSERT_TRUE(strcmp(stL, stResult) == 0, "It was expected that the values would coincide");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    free(stResult);
    
    return 0;
}

static int test_negative_mult_some_numbers()
{   
    const int lengthOps = 4;
    yla_number_type ops[] = {1.234, 3.22, 2.228, 5.28};
    
    size_t sizePrg = 4 * sizeof(yla_number_type);
    size_t sizeCommd = 10;
    size_t stackSize = 4;
    
    yla_cop_type prg[HEADER_SIZE + sizePrg + sizeCommd];
    yla_cop_type *ptr = prg;
    
    //1.234 *(!) 3.22 * 2.228 * 5.28 * = tresult
    put_header(&ptr, stackSize, 0, sizePrg + sizeCommd);
	put_number(&ptr, ops[0]);
	put_commd(&ptr, CMULT);
	put_number(&ptr, ops[1]);
	put_commd(&ptr, CMULT);
	put_number(&ptr, ops[2]);
	put_commd(&ptr, CMULT);
	put_number(&ptr, ops[3]);
	put_commd(&ptr, CMULT);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
	YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_INTERP_STACK_EMPTY, "Expected error for wrong arguments CMULT");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    return 0;
}

 /* 
  * Div Tests
  */

static int test_positive_div_two_numbers()
{
    size_t sizePrg = 2 * sizeof(yla_number_type);
    size_t sizeCommd = 5;
    size_t stackSize = 2;
    yla_cop_type prg[HEADER_SIZE + sizePrg + sizeCommd];
    
    yla_number_type op1 = 3.22;
    yla_number_type op2 = 2;
    yla_number_type tresult = op1 / op2;
    char *stResult = format_number(tresult);
    yla_cop_type *ptr = prg;
    
    //3.22 2 / = 1.61
    put_header(&ptr, stackSize, 0, sizePrg + sizeCommd);
    put_number(&ptr, op1);
    put_number(&ptr, op2);
    put_commd(&ptr, CDIV);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
    char *exceptedResult = yla_vm_last_output(&vm);
    YLATEST_ASSERT_TRUE(strcmp(exceptedResult, stResult) == 0, "It was expected that the values would coincide");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    free(stResult);
    
    return 0;
}

static int test_negative_div_two_numbers()
{
    size_t sizePrg = 2 * sizeof(yla_number_type);
    size_t sizeCommd = 5;
    size_t stackSize = 2;
    yla_cop_type prg[HEADER_SIZE + sizePrg + sizeCommd];
    
    yla_number_type op1 = 3.22;
    yla_number_type op2 = 2;
    yla_cop_type *ptr = prg;
    
    //3.22 /(!) 2 = YLA_VM_ERROR_STACK_EMPTY
    put_header(&ptr, stackSize, 0, sizePrg + sizeCommd);
    put_number(&ptr, op1);
    put_commd(&ptr, CDIV);
    put_number(&ptr, op2);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
    YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_STACK_EMPTY, "Expected error for wrong arguments CDIV");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    return 0;
}

static int test_positive_div_some_numbers()
{   
    const int lengthOps = 4;
    yla_number_type ops[] = {1.234, 3.22, 2.228, 5.28};
    yla_number_type tresult = ops[0];
    
    for(int i = 1; i < lengthOps; i++){
		tresult /= ops[i];
	}
	
	char *stResult = format_number(tresult);
    
    size_t sizePrg = 4 * sizeof(yla_number_type);
    size_t sizeCommd = 9;
    size_t stackSize = 4;
    
    yla_cop_type prg[HEADER_SIZE + sizePrg + sizeCommd];
    yla_cop_type *ptr = prg;
    
    //1.234 3.22 / 2.228 / 5.28 / = tresult
    put_header(&ptr, stackSize, 0, sizePrg + sizeCommd);
	put_number(&ptr, ops[0]);
	put_number(&ptr, ops[1]);
	put_commd(&ptr, CDIV);
	put_number(&ptr, ops[2]);
	put_commd(&ptr, CDIV);
	put_number(&ptr, ops[3]);
	put_commd(&ptr, CDIV);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
    char *stL = yla_vm_last_output(&vm);
    YLATEST_ASSERT_TRUE(strcmp(stL, stResult) == 0, "It was expected that the values would coincide");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    free(stResult);
    
    return 0;
}

static int test_negative_div_some_numbers()
{   
    const int lengthOps = 4;
    yla_number_type ops[] = {1.234, 3.22, 2.228, 5.28};
    
    size_t sizePrg = 4 * sizeof(yla_number_type);
    size_t sizeCommd = 10;
    size_t stackSize = 4;
    
    yla_cop_type prg[HEADER_SIZE + sizePrg + sizeCommd];
    yla_cop_type *ptr = prg;
    
    //1.234 /(!) 3.22 / 2.228 / 5.28 / = tresult
    put_header(&ptr, stackSize, 0, sizePrg + sizeCommd);
	put_number(&ptr, ops[0]);
	put_commd(&ptr, CDIV);
	put_number(&ptr, ops[1]);
	put_commd(&ptr, CDIV);
	put_number(&ptr, ops[2]);
	put_commd(&ptr, CDIV);
	put_number(&ptr, ops[3]);
	put_commd(&ptr, CDIV);
    put_commd(&ptr, COUT);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
	YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_STACK_EMPTY, "Expected error for wrong arguments CDIV");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    return 0;
}

static int test_div_on_zero_numbers()
{
    size_t sizePrg = 2 * sizeof(yla_number_type);
    size_t sizeCommd = 4;
    size_t stackSize = 2;
    yla_cop_type prg[HEADER_SIZE + sizePrg + sizeCommd];
    
    yla_number_type op1 = 3.22;
    yla_number_type op2 = 0;
    yla_number_type tresult = op1 * op2;
    yla_cop_type *ptr = prg;
    
    //0 3.22 / = YLA_VM_ERROR_DIV_BY_ZERO
    put_header(&ptr, stackSize, 0, sizePrg + sizeCommd);
    put_number(&ptr, op1);
    put_number(&ptr, op2);
    put_commd(&ptr, CDIV);
    put_commd(&ptr, CHALT);
    
    yla_vm vm;
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
    YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "not normal");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_DIV_BY_ZERO, "Excepted YLA_VM_ERROR_DIV_BY_ZERO");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    return 0;
}
