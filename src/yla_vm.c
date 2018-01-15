/*
    Virtual Machine implementation

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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "../include/yla_vm.h"
#include "../include/yla_cop.h"
#include "../include/yla_type.h"


int yla_vm_get_value(yla_vm *vm, yla_number_type *value);

int yla_vm_set_var(yla_vm *vm, size_t index, yla_number_type value);
int yla_vm_get_var(yla_vm *vm, size_t index, yla_number_type *value);

yla_number_type yla_vm_get_value_internal(yla_cop_type *start);

int yla_vm_read_header(yla_vm *vm, yla_cop_type *program, size_t program_size);
int yla_vm_check_magic(yla_cop_type** program);
int yla_vm_read_sizes(yla_vm *vm, yla_cop_type **program);

int yla_vm_do_command_internal(yla_vm *vm, yla_cop_type cop);

char *yla_vm_error_message(int error_code);

//yla_vm_stack
int yla_vm_stack_pull(yla_vm *vm, yla_number_type *value);
int yla_vm_stack_push(yla_vm *vm, yla_number_type value);
int yla_vm_stack_top(yla_vm *vm, yla_number_type *value);
yla_number_type *yla_vm_stack_pull_set(yla_vm *vm, size_t *size_of_set);

int yla_vm_print_stack_elements(yla_vm *vm, yla_stack *stack);

//yla_vm_interp
int yla_vm_interp_pull(yla_vm *vm, yla_number_type *type_data);
int yla_vm_interp_push(yla_vm *vm);
int yla_vm_interp_push(yla_vm *vm);
int yla_vm_interp_pushset(yla_vm *vm, size_t size_of_set);
int yla_vm_interp_top(yla_vm *vm, yla_number_type *type_data);



//Format output
char *format_number(yla_number_type num);
char *format_set(size_t size_of_set, yla_number_type *set);
/*
Public functions
*/

int yla_vm_init(yla_vm *vm, yla_cop_type *program, size_t program_size)
{
	if (!vm) {
		return 0;
	}

	if (!yla_vm_read_header(vm, program, program_size)) {
		vm->last_error = YLA_VM_ERROR_NO_PROGRAM_CODE;
		return 0;
	}
	
	yla_stack_init(&vm->stack, vm->stack_size);
	yla_stack_init(&vm->interp_stack, vm->interp_stack_size);
	vm->vartable = calloc(vm->vartable_size, sizeof(yla_number_type));
	vm->code = malloc(vm->code_size);
	vm->pc = 0;
	if (program_size < HEADER_SIZE + vm->code_size) {
		vm->last_error = YLA_VM_ERROR_NO_PROGRAM_CODE;
		return 0;
	}

	memcpy(vm->code, program + HEADER_SIZE, vm->code_size);
	
	vm->last_error = YLA_VM_ERROR_OK;
	vm->last_output = NULL;
	return 1;
}

int yla_vm_done(yla_vm *vm)
{
	if (!vm) {
		return 0;
	}

	if (vm->code) {
		free(vm->code);
	}

	if (vm->vartable) {
		free(vm->vartable);
	}
	
	if (vm->last_output) {
		free(vm->last_output);
	}

	yla_stack_done(&vm->stack);
	yla_stack_done(&vm->interp_stack);
	
	return 1;
}

int yla_vm_run(yla_vm *vm)
{
	int cmd_result;

	if (!vm) {
		return 0;
	}

	if (!vm->code) {
		return 0;
	}

	while(1) {
		if (vm->pc + 1 > vm->code_size) {
			vm->last_error = YLA_VM_ERROR_CODE_SEG_EXCEED;
			return 0;
		}
		yla_cop_type cop = vm->code[vm->pc++];

		cmd_result = yla_vm_do_command_internal(vm, cop);
		
		if (cmd_result == -1) {
			return 1;
		}
		if (cmd_result == 0) {
			return 0;
		}
	}
}

int yla_vm_do_command(yla_vm *vm)
{
	if (!vm) {
		return 0;
	}

	if (!vm->code) {
		return 0;
	}

	if (vm->pc + 1 > vm->code_size) {
		vm->last_error = YLA_VM_ERROR_CODE_SEG_EXCEED;
		return 0;
	}
	yla_cop_type cop = vm->code[vm->pc++];

	return yla_vm_do_command_internal(vm, cop);
}

int yla_vm_last_error(yla_vm *vm)
{
	if (!vm) {
		return 0;
	}
	return vm->last_error;
}

int yla_vm_error_text(yla_vm *vm, int error_code, char *buf, int buf_len)
{
	char *error_message;
	int message_len;

	if (!vm) {
		return 0;
	}

	error_message = yla_vm_error_message(error_code);
	message_len = strlen(error_message) + 1;
	if (buf == NULL || buf_len <= message_len) {
		return message_len;
	}

	strcpy(buf, error_message);

	return 0;
}

char *yla_vm_last_output(yla_vm *vm)
{
	if (!vm){
		return "vm not found";
	}
	return vm->last_output;
}
/*
Private functions
*/

/*
Get values
*/
int yla_vm_get_value(yla_vm *vm, yla_number_type *value)
{
	if (vm->pc + sizeof(yla_number_type) > vm->code_size) {
		vm->last_error = YLA_VM_ERROR_CODE_SEG_EXCEED;
		return 0;
	}
	
	*value = yla_vm_get_value_internal(&vm->code[vm->pc]);
	vm->pc += sizeof(yla_number_type);
	
	return 1;
}

yla_number_type yla_vm_get_value_internal(yla_cop_type *start)
{
	UNION_DOUBLE helper;
	helper.num = 2.0;
	
	for(int i = 0; i < sizeof(yla_number_type); i++){
		helper.chars[i] = (*(start+i));
	}
	return helper.num;
}

/*
Reading program
*/
int yla_vm_read_header(yla_vm *vm, yla_cop_type *program, size_t program_size)
{
	if (program == NULL) {
		return 0;
	}
	if (program_size < HEADER_SIZE) {
		return 0;
	}
	if (!yla_vm_check_magic(&program)) {
		return 0;
	}
	if (!yla_vm_read_sizes(vm, &program)) {
		return 0;
	}
	
	return 1;
}

int yla_vm_check_magic(yla_cop_type** program)
{
	yla_number_type magic_value;
	magic_value = yla_vm_get_value_internal(*program);
	if (magic_value != MAGIC_CODE1) {
		return 0;
	}
	*program += sizeof(yla_number_type);

	magic_value = yla_vm_get_value_internal(*program);
	if (magic_value != MAGIC_CODE2) {
		return 0;
	}
	*program += sizeof(yla_number_type);

	magic_value = yla_vm_get_value_internal(*program);
	if (magic_value != MAGIC_CODE3) {
		return 0;
	}
	*program += sizeof(yla_number_type);

	return 1;
}

int yla_vm_read_sizes(yla_vm *vm, yla_cop_type **program)
{
	vm->stack_size = (size_t)yla_vm_get_value_internal(*program);
	if (vm->stack_size > MAX_STACK_SIZE) {
		return 0;
	}
	*program += sizeof(yla_number_type);
	
	vm->interp_stack_size = (size_t)yla_vm_get_value_internal(*program);
	if (vm->interp_stack_size > MAX_STACK_SIZE) {
		return 0;
	}
	*program += sizeof(yla_number_type);
	
	vm->vartable_size = (size_t)yla_vm_get_value_internal(*program);
	if (vm->vartable_size > MAX_VARTABLE_SIZE) {
		return 0;
	}
	*program += sizeof(yla_number_type);
	
	vm->code_size = (size_t)yla_vm_get_value_internal(*program);

	if (vm->code_size > MAX_CODE_SIZE) {
		return 0;
	}
	*program += sizeof(yla_number_type);

	return 1;
}

/*
Vartable
*/
int yla_vm_get_var(yla_vm *vm, size_t index, yla_number_type *value)
{
	if (!vm) {
		return 0;
	}
	if (index >= vm->vartable_size) {
		return 0;
	}

	*value = vm->vartable[index];

	return 1;
}

int yla_vm_set_var(yla_vm *vm, size_t index, yla_number_type value)
{
	if (!vm) {
		return 0;
	}
	if (index >= vm->vartable_size) {
		return 0;
	}

	vm->vartable[index] = value;
}

/*
Stack
*/

int yla_vm_stack_pull(yla_vm *vm, yla_number_type *value)
{
	if (!vm) {
		return 0;
	}
	if (!yla_stack_pull(&vm->stack, value)) {
		vm->last_error = YLA_VM_ERROR_STACK_EMPTY;
		return 0;
	}
	return 1;
}

int yla_vm_stack_push(yla_vm *vm, yla_number_type value)
{
	if (!vm) {
		return 0;
	}
	if (!yla_stack_push(&vm->stack, value)) {
		vm->last_error = YLA_VM_ERROR_STACK_FULL;
		return 0;
	}
	return 1;
}

int yla_vm_stack_top(yla_vm *vm, yla_number_type *value)
{
	if (!vm) {
		return 0;
	}
	if (!yla_stack_top(&vm->stack, value)) {
		vm->last_error = YLA_VM_ERROR_STACK_EMPTY;
		return 0;
	}
	return 1;
}

yla_number_type *yla_vm_stack_pull_set(yla_vm *vm, size_t *size_of_set)
{
	yla_number_type size_set;
	if (!yla_vm_interp_pull(vm, &size_set)) {
		return NULL;
	}
	*size_of_set = (yla_number_type)size_set;
	
	if ((size_set <= 0)
		|| (size_set > vm->stack.size))
	{
		vm->last_error = YLA_VM_ERROR_BAD_SET_SIZE;
		return NULL;
	}
	
	yla_number_type *set = (yla_number_type *)calloc(size_set, sizeof(yla_number_type));
	if (set == NULL) {
		vm->last_error = YLA_VM_ERROR_CALLOC_SET;
		return NULL;
	}
	
	yla_number_type op1;
	for (int i = 0; i < (int)size_set; i++) 
	{
		if (!yla_vm_stack_pull(vm, &op1)) {
			return NULL;
		}
		set[(int)size_set-i-1] = (yla_number_type)op1;
	}
	
	return set;
}


int yla_vm_stack_push_set(yla_vm *vm, yla_number_type *set, size_t size_of_set)
{
	if ((size_of_set <= 0)
		|| (size_of_set > vm->stack.size))
	{
		vm->last_error = YLA_VM_ERROR_BAD_SET_SIZE;
		return 0;
	}
	for (int i = 0; i < size_of_set; i++) 
	{
		if (!yla_vm_stack_push(vm, set[i])) {
			return 0;
		}
	}
	return 1;
}

/*
 * Interpretator stack of type data - CPUSH (yla_number_type), CPUSHSET (size_of_set, set)
 */
int yla_vm_interp_pull(yla_vm *vm, yla_number_type *type_data)
{
	if (!yla_stack_pull(&vm->interp_stack, type_data)) {
		vm->last_error = YLA_VM_ERROR_INTERP_STACK_EMPTY;
		return 0;
	}
	return 1;
} 

/**
 * Interp push for number - CPUSH
 **/
int yla_vm_interp_push(yla_vm *vm)
{
	if (!yla_stack_push(&vm->interp_stack, (yla_number_type)CPUSH)) {
		vm->last_error = YLA_VM_ERROR_INTERP_STACK_FULL;
		return 0;
	}
	return 1;
}
 
/**
 * Interp push for set - CPUSHSET
 **/ 
int yla_vm_interp_pushset(yla_vm *vm, size_t size_of_set)
{
	if (!yla_stack_push(&vm->interp_stack, (yla_number_type)size_of_set)) {
		vm->last_error = YLA_VM_ERROR_INTERP_STACK_FULL;
		return 0;
	}
	if (!yla_stack_push(&vm->interp_stack, (yla_number_type)CPUSHSET)) {
		vm->last_error = YLA_VM_ERROR_INTERP_STACK_FULL;
		return 0;
	}
	return 1;
}

int yla_vm_interp_top(yla_vm *vm, yla_number_type *type_data)
{
	if (!yla_stack_top(&vm->interp_stack, type_data)) {
		vm->last_error = YLA_VM_ERROR_INTERP_STACK_EMPTY;
		return 0;
	}
	return 1;
} 

/*
Perform command by code of operation
*/
int yla_vm_do_command_internal(yla_vm *vm, yla_cop_type cop)
{
	yla_number_type op1;
	yla_number_type op2;
	yla_number_type res;
	yla_number_type size_of_set;
	yla_number_type *set1 = NULL;
	yla_number_type *set2 = NULL;
	yla_number_type *rset = NULL;
	size_t size_of_rset;
	size_t size_of_set1;
	size_t size_of_set2;
	yla_number_type cmd;
	
	switch(cop) {

		case CNOP:	
			break;

		case CPUSH:
			if (!yla_vm_get_value(vm, &res)) {
				return 0;
			}
			if (!yla_vm_interp_push(vm)) {
				return 0;
			}
			if (!yla_vm_stack_push(vm, res)) {
				return 0;
			}
			break;
		case CPUSHSET:
			if (!yla_vm_get_value(vm, &size_of_set)) {
				return 0;
			}
			if ((size_of_set <= 0) 
				|| (size_of_set > vm->stack.size)){
				vm->last_error = YLA_VM_ERROR_BAD_SET_SIZE;
				return 0;
			}
			if (!yla_vm_interp_pushset(vm, size_of_set)) {
				return 0;
			}
			for (int i = 0; i < size_of_set; i++)
			{
				if (!yla_vm_get_value(vm, &op1)) {
					return 0;
				}
				if (!yla_vm_stack_push(vm, op1)) {
					return 0;
				}
			}
			break;
		case CADD:
			if (!yla_vm_interp_pull(vm, &cmd)) {
				return 0;
			}
			switch ((int)cmd) {
				case CPUSH:
					if (!yla_vm_stack_pull(vm, &op1)) {
						return 0;
					}
					
					if (!yla_vm_interp_pull(vm, &cmd)) {
						return 0;
					}

					if (((int)cmd) != CPUSH) {
						vm->last_error = YLA_VM_ERROR_INTERP_STACK_UNKNOWN_COMMAND;
						return 0;
					}
					
					if (!yla_vm_stack_pull(vm, &op2)) {
						return 0;
					}
					
					res = op2 + op1;
					if (!yla_vm_interp_push(vm)) {
						return 0;
					}
					if (!yla_vm_stack_push(vm, res)) {
						return 0;
					}
					break;
				case CPUSHSET:
					set1 = yla_vm_stack_pull_set(vm, &size_of_set1);
					if (set1 == NULL) {
						return 0;
					}
					if (!yla_vm_interp_pull(vm, &cmd)) {
						return 0;
					}
					
					if (((int)cmd) != CPUSHSET) {
						vm->last_error = YLA_VM_ERROR_INTERP_STACK_UNKNOWN_COMMAND;
						return 0;
					}
					set2 = yla_vm_stack_pull_set(vm, &size_of_set2);
					if (set2 == NULL) {
						return 0;
					}
					//set1 set2 + = [set1, set2]
					rset = union_sets(vm, size_of_set, size_of_set2, set1, set2, &size_of_rset);
					if (rset == NULL) {
						return 0;
					}
					
					if (!yla_vm_interp_pushset(vm, size_of_rset)) {
						return 0;
					}
					if (!yla_vm_stack_push_set(vm, rset, size_of_rset)) {
						return 0;
					}
					break;
				default:
					vm->last_error = YLA_VM_ERROR_UNKNOWN_COMMAND;
					return 0;
			}
			break;
		case CSUB: 
			if (!yla_vm_stack_pull(vm, &op1)) {
				return 0;
			}
			if (!yla_vm_stack_pull(vm, &op2)) {
				return 0;
			}
			res = op2 - op1;
			if (!yla_vm_stack_push(vm, res)) {
				return 0;
			}
			break;

		case CMULT:
			if (!yla_vm_interp_pull(vm, &cmd)) {
				return 0;
			}
			switch((int)cmd){
				case CPUSH:
					if (!yla_vm_stack_pull(vm, &op1)) {
						return 0;
					}
					
					if (!yla_vm_interp_pull(vm, &cmd)) {
						return 0;
					}

					if (((int)cmd) != CPUSH) {
						vm->last_error = YLA_VM_ERROR_INTERP_STACK_UNKNOWN_COMMAND;
						return 0;
					}
					
					if (!yla_vm_stack_pull(vm, &op2)) {
						return 0;
					}
					
					res = op2 * op1;
					if (!yla_vm_interp_push(vm)) {
						return 0;
					}
					if (!yla_vm_stack_push(vm, res)) {
						return 0;
					}
				case CPUSHSET:
					set1 = yla_vm_stack_pull_set(vm, &size_of_set1);
					if (set1 == NULL) {
						return 0;
					}
					if (!yla_vm_interp_pull(vm, &cmd)) {
						return 0;
					}
					
					if (((int)cmd) != CPUSHSET) {
						vm->last_error = YLA_VM_ERROR_INTERP_STACK_UNKNOWN_COMMAND;
						return 0;
					}
					set2 = yla_vm_stack_pull_set(vm, &size_of_set2);
					if (set2 == NULL) {
						return 0;
					}
					//set1 set2 * = [intersection of sets]
					rset = intersection_of_sets(vm, size_of_set, size_of_set2, set1, set2, &size_of_rset);
					if (rset == NULL) {
						return 0;
					}
					
					if (!yla_vm_interp_pushset(vm, size_of_rset)) {
						return 0;
					}
					if (!yla_vm_stack_push_set(vm, rset, size_of_rset)) {
						return 0;
					}
					break;
				default:
					vm->last_error = YLA_VM_ERROR_UNKNOWN_COMMAND;
					return 0;
			}
			break;

		case CDIV: 
			if (!yla_vm_stack_pull(vm, &op1)) {
				return 0;
			}
			if (!yla_vm_stack_pull(vm, &op2)) {
				return 0;
			}

			if (op1==0.0) {
				vm->last_error = YLA_VM_ERROR_DIV_BY_ZERO;
				return 0;
			}
			res = op2 / op1;
			if (!yla_vm_stack_push(vm, res)) {
				return 0;
			}
			break;
		case CINCLUDE:
			if (!yla_vm_interp_pull(vm, &cmd)) {
				return 0;
			}
			switch((yla_cop_type)cmd){
				case CPUSH:
					if (!yla_vm_stack_top(vm, &op1)) {
						return 0;
					}
					
					if (!yla_vm_interp_pull(vm, &cmd)) {
						return 0;
					}
					
					if (((int)cmd) != CPUSHSET) {
						vm->last_error = YLA_VM_ERROR_INTERP_STACK_UNKNOWN_COMMAND;
						return 0;
					}
					set1 = yla_vm_stack_pull_set(vm, &size_of_set1);
					
					res = include_of_set(vm, size_of_set1, set1, op1);
					
					if (res == -1) {
						return 0;
					}
					if (!yla_vm_interp_push(vm)) {
						return 0;
					}
					if (!yla_vm_stack_push(vm, res)) {
						return 0;
					}
					
					break;
				default:
					vm->last_error = YLA_VM_ERROR_INTERP_STACK_UNKNOWN_COMMAND;
					return 0;
			}
			break;
		case COUT:
			if (!yla_vm_interp_pull(vm, &cmd)) {
				return 0;
			}
			switch((yla_cop_type)cmd){
				case CPUSH:
					if (!yla_vm_stack_top(vm, &res)) {
						return 0;
					}
					if (vm->last_output) {
						free(vm->last_output);
					}
					vm->last_output = format_number(res);
					break;
				case CPUSHSET:
					set1 = yla_vm_stack_pull_set(vm, &size_of_set1);
					if (set1 == NULL) {
						return 0;
					}
					if (vm->last_output) {
						free(vm->last_output);
					}
					vm->last_output = format_set(size_of_set1, set1);
					free(set1);
					break;
				default:
					vm->last_error = YLA_VM_ERROR_INTERP_STACK_UNKNOWN_COMMAND;
					return 0;
			}
			break;
		case CHALT:
			return -1;

		default:
			vm->last_error = YLA_VM_ERROR_UNKNOWN_COMMAND;
			return 0;
	}
	return 1;
}

/*
 * Operations with set
 */
static int compare (const void * a, const void * b)
{
  if (*(yla_number_type*)a > *(yla_number_type*)b) return 1;
  else if (*(yla_number_type*)a < *(yla_number_type*)b) return -1;
  else return 0;  
}

yla_number_type *union_sets(yla_vm *vm, size_t size_of_set1, size_t size_of_set2, yla_number_type *set1, yla_number_type *set2, size_t *size_of_rset)
{
	if (vm == NULL) {
		return NULL;
	}
	if ((size_of_set1 <= 0) || (size_of_set2 <= 0)){
		vm->last_error = YLA_VM_ERROR_BAD_SET_SIZE;
		return NULL;
	}
	if ((set1 == NULL) || (set2 == NULL)){
		vm->last_error = YLA_VM_ERROR_CODE_SEG_EXCEED;
		return NULL;
	}
	*size_of_rset = size_of_set1 + size_of_set2;
	yla_number_type *rset = (yla_number_type *)calloc(*size_of_rset, sizeof(yla_number_type));
	if (rset == NULL) {
		vm->last_error = YLA_VM_ERROR_CALLOC_SET;
		return NULL;
	}
	for (int i = 0; i < size_of_set1; i++)
	{
		rset[i] = set1[i];
	}
	int j = 0;
	int size = *size_of_rset;
	for (int i = size_of_set1; i < size; i++)
	{
		rset[i] = set2[j];
		j++;
	}
	qsort(rset, *size_of_rset, sizeof(yla_number_type), compare);
	return rset;
}

yla_number_type include_of_set(yla_vm *vm, size_t size_of_set, yla_number_type *set1, yla_number_type number)
{
	if (vm == NULL) {
		return -1;
	}
	if (size_of_set <= 0){
		vm->last_error = YLA_VM_ERROR_BAD_SET_SIZE;
		return -1;
	}
	if (set1 == NULL){
		vm->last_error = YLA_VM_ERROR_CODE_SEG_EXCEED;
		return -1;
	}
	yla_number_type result = 0.0;
	
	for (int i = 0; i < size_of_set; i++)
	{
		if (number == set1[i]) {
			result = 1.0;
			break;
		}
	}
	return result;
}

yla_number_type *intersection_of_sets(yla_vm *vm, size_t size_of_set1, size_t size_of_set2, yla_number_type *set1, yla_number_type *set2, size_t *size_of_rset)
{
	if (vm == NULL) {
		return NULL;
	}
	if ((size_of_set1 <= 0) || (size_of_set2 <= 0)){
		vm->last_error = YLA_VM_ERROR_BAD_SET_SIZE;
		return NULL;
	}
	if ((set1 == NULL) || (set2 == NULL)){
		vm->last_error = YLA_VM_ERROR_CODE_SEG_EXCEED;
		return NULL;
	}
	size_t intersection = 0;
	for (int i = 0; i < size_of_set1; i++)
	{
		for (int j = 0; j < size_of_set2; j++)
		{
			if (set1[i] == set2[j]) {
				intersection++;
			}
		}
	}
	*size_of_rset = intersection;
	yla_number_type *rset = (yla_number_type *)calloc(*size_of_rset, sizeof(yla_number_type));
	if (rset == NULL) {
		vm->last_error = YLA_VM_ERROR_CALLOC_SET;
		return NULL;
	}
	int k = 0;
	for (int i = 0; i < size_of_set1; i++)
	{
		for (int j = 0; j < size_of_set2; j++)
		{
			if (set1[i] == set2[j]) {
				rset[k] = set1[i];
				k++;
			}
		}
	}
	qsort(rset, *size_of_rset, sizeof(yla_number_type), compare);
	return rset;
}
/*
Error messages
*/
char *yla_vm_error_message(int error_code)
{
	switch(error_code) {
		case YLA_VM_ERROR_OK:
			return "No error";
		case YLA_VM_ERROR_NO_PROGRAM_CODE:
			return "No program code";
		case YLA_VM_ERROR_CODE_SEG_EXCEED:
			return "Code segment exceed";
		case YLA_VM_ERROR_DIV_BY_ZERO:
			return "Divide by zero";
		case YLA_VM_ERROR_UNKNOWN_COMMAND:
			return "Unknown command";
		case YLA_VM_ERROR_STACK_EMPTY:
			return "Empty stack";
		case YLA_VM_ERROR_STACK_FULL:
			return "Stack full_value";
		case YLA_VM_ERROR_BAD_SET_SIZE:
			return "Wrong size of set";
		case YLA_VM_ERROR_INTERP_STACK_EMPTY:
			return "Interpretator stack of type data is empty";
		case YLA_VM_ERROR_INTERP_STACK_FULL:
			return "Interpretator stack of type data is full";
		case YLA_VM_ERROR_INTERP_STACK_UNKNOWN_COMMAND:
			return "Interpretator stack found unknown command";
		case YLA_VM_ERROR_CALLOC_SET:
			return "Error in calloc for set";
		default:
			return "Unknown error";
	}
}


int yla_vm_print_stack_elements(yla_vm *vm, yla_stack *stack)
{
	yla_number_type op1 = 0.0;
	if (stack->size == 0) {
		vm->last_error = YLA_VM_ERROR_STACK_EMPTY;
		return 0;
	}
	for (int i = 0; i < stack->count; i++)
	{
		if (!yla_stack_get_deep(stack, i, &op1)) {
			vm->last_error = YLA_VM_ERROR_CODE_SEG_EXCEED;
			return 0;
		}
		printf("element=%f\n", op1);
	}
	return 1;
}
/*
 * Format output
 */
char *format_number(yla_number_type num)
{
	char *result = calloc(1, sizeof(yla_number_type));
	sprintf(result, "%f", num);
	return result;
}

char *format_set(size_t size_of_set, yla_number_type *set)
{
	size_t size = size_of_set * 1.5;
    char *pos = calloc(size, sizeof(yla_number_type));
    char *result = pos;
    
    pos += sprintf(pos, "set(");
    for (int i = 0 ; i < size_of_set ; i++) {
        if (i) {
            pos += sprintf(pos, ", ");
        }
        pos += sprintf(pos, "%f", set[i]);
    }
    pos += sprintf(pos, "%s", ")\0");
	
	return result;
}
