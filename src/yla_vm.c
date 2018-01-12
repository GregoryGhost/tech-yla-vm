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
	vm->last_output = 0;
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
	if (!yla_stack_pull(&vm->stack, value)) {
		vm->last_error = YLA_VM_ERROR_STACK_EMPTY;
		return 0;
	}
	return 1;
}

int yla_vm_stack_push(yla_vm *vm, yla_number_type value)
{
	if (!yla_stack_push(&vm->stack, value)) {
		vm->last_error = YLA_VM_ERROR_STACK_FULL;
		return 0;
	}
	return 1;
}

int yla_vm_stack_top(yla_vm *vm, yla_number_type *value)
{
	if (!yla_stack_top(&vm->stack, value)) {
		vm->last_error = YLA_VM_ERROR_STACK_EMPTY;
		return 0;
	}
	return 1;
}


/*
 * Interpretator stack of type data - CPUSH (yla_number_type), CPUSHSET (size_of_set, set)
 */
int yla_vm_interp_pull(yla_vm *vm, yla_number_type *type_data)
{
	if (!yla_vm_stack_pull(&vm->interp_stack, type_data)) {
		vm->last_error = YLA_VM_ERROR_INTERP_STACK_EMPTY;
		return 0;
	}
	return 1;
} 

/**
 * Interp push for number - CPUSH
 **/
int yla_vm_interp_push(yla_vm *vm, yla_number_type type_data)
{
	if (!yla_vm_stack_push(&vm->interp_stack, type_data)) {
		vm->last_error = YLA_VM_ERROR_INTERP_STACK_FULL;
		return 0;
	}
	return 1;
}
 
/**
 * Interp push for set - CPUSHSET
 **/ 
int yla_vm_interp_pushset(yla_vm *vm, size_t size_of_set, yla_number_type type_data)
{
	if (!yla_vm_stack_push(&vm->interp_stack, type_data)) {
		vm->last_error = YLA_VM_ERROR_INTERP_STACK_FULL;
		return 0;
	}
	if (!yla_vm_stack_push(&vm->interp_stack, size_of_set)) {
		vm->last_error = YLA_VM_ERROR_INTERP_STACK_FULL;
		return 0;
	}
	return 1;
}

int yla_vm_interp_top(yla_vm *vm, yla_number_type *type_data)
{
	if (!yla_vm_stack_top(&vm->interp_stack, type_data)) {
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
	
	switch(cop) {

		case CNOP:	
			break;

		case CPUSH:
			if (!yla_vm_get_value(vm, &res)) {
				return 0;
			}
			if (!yla_vm_interp_push(vm, CPUSH)) {
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
			if (size_of_set <= 0){
				vm->last_error = YLA_VM_ERROR_BAD_SET_SIZE;
				return 0;
			}
			if (!yla_vm_interp_pushset(vm, size_of_set, CPUSHSET)) {
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
			if (!yla_vm_stack_pull(vm, &op1)) {
				return 0;
			}
			if (!yla_vm_stack_pull(vm, &op2)) {
				return 0;
			}
			
			res = op2 + op1;
			
			if (!yla_vm_stack_push(vm, res)) {
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
			if (!yla_vm_stack_pull(vm, &op1)) {
				return 0;
			}
			if (!yla_vm_stack_pull(vm, &op2)) {
				return 0;
			}
			res = op2 * op1;
			if (!yla_vm_stack_push(vm, res)) {
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

		case COUT:
			if (!yla_vm_stack_top(vm, &res)) {
				return 0;
			}
			if (vm->last_output) {
				free(vm->last_output);
			}
			vm->last_output = format_number(res);
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
		default:
			return "Unknown error";
	}
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
    
    pos += sprintf(pos, "[");
    for (int i = 0 ; i != 4 ; i++) {
        if (i) {
            pos += sprintf(pos, ", ");
        }
        pos += sprintf(pos, "%f", set[i]);
    }
    pos += sprintf(pos, "]");
	
	return result;
	
}
