#ifndef _com_types_h
#define _com_types_h

// Base types

#ifndef NULL
# ifdef __cplusplus
#  define NULL    0
# else
#  define NULL    ((void *)0)
# endif
#endif

typedef signed   char           i8;
typedef unsigned char           u8;

typedef signed   short          i16;
typedef unsigned short          u16;

typedef signed   int            i32;
typedef unsigned int            u32;

typedef signed   long long      i64;
typedef unsigned long long      u64;

typedef float                   f32;
typedef double                  f64;

#endif // _com_types_h
