cmake_minimum_required(VERSION 3.9)
project(emptyVM)				# Название проекта

set(SOURCES yla_stack.c yla_vm.c yla_vm_main.c)

add_executable(${PROJECT_NAME} ${SOURCES})
