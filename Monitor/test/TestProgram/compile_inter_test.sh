#!/bin/sh
gcc inter_main.c -o inter_main -lcap && gcc inter_child.c -o inter_child #&& sudo setcap 'cap_sys_ptrace=eip' ./inter_main
