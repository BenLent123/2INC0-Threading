#!/bin/bash
#
#  In case your platform uses different commands, flags or libraries,
#  you can adjust the following settings
#
CC=gcc
#
CFLAGS="-Wall"
#
LIBS="-lpthread"
#
#  To improve debugging with GDB, if compiled with gcc:
#CFLAGS="-Wall -ggdb2"
#
#  Useful commands for analyzing active applications:
#  gdb : debugger
#      gdb application
#            Start application within the gdb debug, in case the application fails.
#      gdb application pid
#            Connect gdb to the running application with the given process id pid.
#            Useful to check why or where the application is stuck.
#  strace : trace system calls
#  ltrace : trace library calls
#  valgrind : debugging memory and profiling
#  gprof : call graph execution profiler
#
$CC $CFLAGS -o intersection intersection.c intersection_time.c $LIBS
