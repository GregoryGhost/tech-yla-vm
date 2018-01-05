cmake_minimum_required(VERSION 3.9)
project(tests)			# Название проекта

set(TESTS yla_stack_test.c yla_test_gencode.c yla_test_test.c yla_tests.c yla_vm_test1.c)

add_executable(${PROJECT_NAME} ${TESTS})
