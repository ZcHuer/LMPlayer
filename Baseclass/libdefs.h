#define MAX_THREADS 5 /* Maximum number of threads */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef WIN32
# include <sys/types.h>
# include <malloc.h>
# define HAVE_MEMMOVE
# define HAVE_MEMSET
# define STDC_HEADERS
#endif


#ifdef STDC_HEADERS
# include <string.h>
# include <stdlib.h>
# include <stdio.h>
#endif

#ifdef WIN32
# include <windows.h>
# include <string.h>
# include <sys/types.h>
# include <malloc.h>
#endif

#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SIZEOF_UNSIGNED_LONG_INT == 4
 typedef unsigned long word32;
 typedef signed long sword32;
#elif SIZEOF_UNSIGNED_INT == 4
 typedef unsigned int word32;
 typedef signed int sword32;
#else
//# warn "Cannot find a 32 bit integer in your system. Using defaults."
 typedef unsigned int word32;
 typedef signed int sword32; 
#endif

#if SIZEOF_UNSIGNED_INT == 2
 typedef unsigned int word16;
#elif SIZEOF_UNSIGNED_SHORT_INT == 2
 typedef unsigned short word16;
#else 
//# warn "Cannot find a 16 bit integer in your system. Using defaults."
 typedef unsigned short word16;
#endif

#if SIZEOF_UNSIGNED_CHAR == 1
 typedef unsigned char word8; 
#else
//# warn "Cannot find an 8 bit char in your system. Using defaults"
 typedef unsigned char word8; 
#endif

#ifdef __cplusplus
	}
#endif

/* originally was in 3way.c */
#define RAND32 (word32) ((word32)rand() << 17 ^ (word32)rand() << 9 ^ rand())

#ifndef HAVE_MEMMOVE
# ifdef HAVE_BCOPY
#  define memmove(d, s, n) bcopy ((s), (d), (n))
# else
#  error "Neither memmove nor bcopy exists on your system."
# endif
#endif

#define ENCRYPT 0
#define DECRYPT 1

#define MAX_KEY_LEN 513 /* for gen_key */
