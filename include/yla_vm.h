/*
    Virtual Machine header

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

#ifndef _yla_vm_h
#define _yla_vm_h

#include "yla_stack.h"
#include "yla_cop.h"
#include "yla_type.h"
#include <stddef.h>

#define MAGIC_SIZE 3
#define MAGIC_CODE1 3.22
#define MAGIC_CODE2 6.66
#define MAGIC_CODE3 12.258

#define spaceForCodeVartableProgramSizeSize 3

#define HEADER_SIZE ((MAGIC_SIZE + spaceForCodeVartableProgramSizeSize)* sizeof(yla_number_type))
#define MAX_CODE_SIZE ((size_t)65535)
#define MAX_STACK_SIZE ((size_t)65535)
#define MAX_VARTABLE_SIZE ((size_t)65535)

#define YLA_VM_ERROR_OK 0
#define YLA_VM_ERROR_NO_PROGRAM_CODE (-1)
#define YLA_VM_ERROR_CODE_SEG_EXCEED (-2)
#define YLA_VM_ERROR_DIV_BY_ZERO (-3)
#define YLA_VM_ERROR_UNKNOWN_COMMAND (-4)
#define YLA_VM_ERROR_STACK_EMPTY (-5)
#define YLA_VM_ERROR_STACK_FULL (-6)

/*
Executable program structure:
magic1: yla_number_type
magic2: yla_number_type
magic3: yla_number_type
stack_size: yla_number_type
vartable_size: yla_number_type
code_size: yla_number_type
code: array of char
*/

/**
 * structure contains all internal data of virtual machine
 */
typedef struct {
	yla_stack stack;
	size_t stack_size;

	yla_cop_type *code;
	size_t code_size;

	yla_number_type *vartable;
    size_t vartable_size;

	size_t pc;

	int last_error;
	char *lastOutput;
} yla_vm;

/*
TODO: move this structure on implementation level
*/

/**
 * Initialize virtual machine with block of program
 * @return 0: error, 1: ok
 **/
int yla_vm_init(yla_vm *vm, yla_cop_type *program, size_t program_size);

/**
 * Deinitialize virtual machine. 
 * Calling this function after run required
 * @return 0: error, 1: ok
 **/ 
int yla_vm_done(yla_vm *vm);

/**
 * Run program from current place to halt
 * @return 0: error, 1: ok
 **/
int yla_vm_run(yla_vm *vm);

/**
 * Make only one step (perform one command)
 * @return -1: halt, 0: error, 1: ok
 */ 
int yla_vm_do_command(yla_vm *vm);

/**
 * Returns code of last error.
 * @param vm virtual machine structure
 * @return integer value error code
 **/
int yla_vm_last_error(yla_vm *vm);

/**
 * Returns text of last error.
 * @param vm virtual machine structure
 * @param error_code code of error occurred
 * @param buf pointer to char to buffer to fill last error text
 * @param buf_len lenght of buffer to fill
 * @return 0 if all ok or required buffer lenght to error message if unsuite
 **/
int yla_vm_error_text(yla_vm *vm, int error_code, char *buf, int buf_len);

/**
 * Returns last output of set.
 * @param vm virtual machine structure
 * @return string value output of last command COUT
 **/
char *yla_vm_last_output(yla_vm *vm);


/**
 * Returns formatted number
 * @param number for formating
 * @return formatted number as string
 **/
 char *format_number(yla_number_type num);
/*
TODO: Add/Remove breakpoints
*/

#endif
