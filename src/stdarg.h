#ifndef __STDARG_H__
#define __STDARG_H__

typedef __builtin_va_list va_list;

// Define the va_start, va_arg, and va_end macros based on your target architecture.
#define va_start(ap, last_arg) (__builtin_va_start(ap, last_arg))

#define va_arg(ap, type) (__builtin_va_arg(ap, type))

#define va_end(ap) (__builtin_va_end(ap))

#endif // __STDARG_H__
