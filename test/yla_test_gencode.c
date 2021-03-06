/*
    Generate code for test Virtual Machine

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

#include "../include/yla_test_gencode.h"
#include "../include/yla_type.h"
#include "../include/yla_vm.h"

void put_commd(yla_cop_type** prg, yla_cop_type value)
{
	**prg = value;
	(*prg)++;
}

void put_value(yla_cop_type** prg, yla_number_type value)
{
	UNION_DOUBLE helper;
    helper.num = value;
    for (int i = 0; i < sizeof(yla_number_type); i++)
    {
        put_commd(prg, helper.chars[i]);
    }
}

void put_number(yla_cop_type** prg, yla_number_type value)
{
	put_commd(prg, CPUSH);
	put_value(prg, value);
}

void put_header(yla_cop_type** prg, size_t stack_size, size_t vartable_size, size_t code_size)
{
	size_t interp_stack_size = stack_size;
	put_header_ext(prg, stack_size, interp_stack_size, vartable_size, code_size);
}

void put_header_ext(yla_cop_type** prg, size_t stack_size, size_t interp_stack_size, size_t vartable_size, size_t code_size)
{
	put_value(prg, MAGIC_CODE1);
    put_value(prg, MAGIC_CODE2);
    put_value(prg, MAGIC_CODE3);
    
    //NOTE: warning - size_t разбирается на double, происходит отбрасывание дробной части
    //неявное приведение size_t к double
    put_value(prg, stack_size);
    put_value(prg, interp_stack_size);
    put_value(prg, vartable_size);
    put_value(prg, code_size);
}

void put_set(yla_cop_type** prg, size_t size_of_set, yla_number_type *set)
{
	put_commd(prg, CPUSHSET);
	put_value(prg, size_of_set);
	for (int i = 0; i < size_of_set; i++)
	{
		put_value(prg, set[i]);
	}
}
