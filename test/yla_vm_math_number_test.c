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

#include "../include/yla_vm.h"
#include "../include/yla_cop.h"
#include "../include/yla_test.h"
#include "../include/yla_test_gencode.h"
#include "../include/yla_type.h"

static int test_add_numbers()
{
    yla_vm vm;
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
    
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
    char *exceptedResult = yla_vm_last_output(&vm);
    YLATEST_ASSERT_TRUE(strcmp(exceptedResult, stResult) == 0, "It was expected that the values would coincide");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    return 0;
}

static int test_sub_numbers()
{
    yla_vm vm;
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
    
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
    char *exceptedResult = yla_vm_last_output(&vm);
    YLATEST_ASSERT_TRUE(strcmp(exceptedResult, stResult) == 0, "It was expected that the values would coincide");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    return 0;
}

static int test_mult_numbers()
{
    yla_vm vm;
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
    
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
    char *exceptedResult = yla_vm_last_output(&vm);
    YLATEST_ASSERT_TRUE(strcmp(exceptedResult, stResult) == 0, "It was expected that the values would coincide");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    return 0;
}

static int test_div_numbers()
{
    yla_vm vm;
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
    
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
    YLATEST_ASSERT_TRUE(yla_vm_run(&vm), "normal");
    char *exceptedResult = yla_vm_last_output(&vm);
    YLATEST_ASSERT_TRUE(strcmp(exceptedResult, stResult) == 0, "It was expected that the values would coincide");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    return 0;
}

static int test_div_on_zero_numbers()
{
    yla_vm vm;
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
    
    YLATEST_ASSERT_TRUE(yla_vm_init(&vm, prg, HEADER_SIZE + sizePrg + sizeCommd), "normal");
    YLATEST_ASSERT_FALSE(yla_vm_run(&vm), "not normal");
    YLATEST_ASSERT_TRUE(yla_vm_last_error(&vm) == YLA_VM_ERROR_DIV_BY_ZERO, "Excepted YLA_VM_ERROR_DIV_BY_ZERO");
    YLATEST_ASSERT_TRUE(yla_vm_done(&vm), "normal");
    
    return 0;
}

YLATEST_BEGIN(yla_vm_math_number_test)
   YLATEST_ADD_TEST_CASE(test_add_numbers)
   YLATEST_ADD_TEST_CASE(test_sub_numbers)
   YLATEST_ADD_TEST_CASE(test_mult_numbers)
   YLATEST_ADD_TEST_CASE(test_div_numbers)
   YLATEST_ADD_TEST_CASE(test_div_on_zero_numbers)
YLATEST_END
