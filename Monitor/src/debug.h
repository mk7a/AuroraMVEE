#ifndef MVEE_DEBUG_H
#define MVEE_DEBUG_H

#include <stdio.h>
#include <stdarg.h>

#define debug

#ifdef debug
#define debug_printf(file, fmt, ...) fprintf(file, fmt, ##__VA_ARGS__)
#else
#define debug_printf(file, fmt, ...)
#endif

#endif //MVEE_DEBUG_H
