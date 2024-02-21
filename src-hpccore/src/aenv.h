###########################################################################
# ALGLIB 4.01.0 (source code generated 2023-12-27)
# Copyright (c) Sergey Bochkanov (ALGLIB project).
# 
# >>> SOURCE LICENSE >>>
# This software is a non-commercial edition of  ALGLIB  package,  which  is
# licensed under ALGLIB Personal and Academic Use License Agreement (PAULA).
# 
# See paula-v1.0.pdf file in the archive  root  for  full  text  of  license
# agreement.
# >>> END OF LICENSE >>>

##########################################################################
#ifndef _AENV_H
#define _AENV_H

/*
 * include automatically generated declarations for datatypes, if there are any
 * this #include is not copied to the C++ version of ALGLIB
 */
#include <ae_types.h>

/*$ Includes $*/
/* Definitions */
#define AE_UNKNOWN 0
#define AE_INTEL 1
#define AE_SPARC 2

/* OS definitions */
#define AE_WINDOWS                    1
#define AE_POSIX                      2
#define AE_LINUX                    304
#if !defined(AE_OS)
#define AE_OS AE_UNKNOWN
#endif
#if AE_OS==AE_LINUX
#undef AE_OS
#define AE_OS AE_POSIX
#define _ALGLIB_USE_LINUX_EXTENSIONS
#endif

/* return types for worker functions for different OS types*/
#if AE_OS==AE_WINDOWS
#define _ALGLIB_THREAD_RETURN_TYPE  DWORD WINAPI
#define _ALGLIB_THREAD_RETURN       return 0
#elif AE_OS==AE_POSIX
#define _ALGLIB_THREAD_RETURN_TYPE  void*
#define _ALGLIB_THREAD_RETURN       return NULL
#else
#define _ALGLIB_THREAD_RETURN_TYPE  void
#define _ALGLIB_THREAD_RETURN       return
#endif

/* threading models for AE_THREADING */
#define AE_PARALLEL                 100
#define AE_SERIAL                   101
#define AE_SERIAL_UNSAFE            102
#if !defined(AE_THREADING)
#define AE_THREADING AE_PARALLEL
#endif

/* which entropy source to use */
#define ALGLIB_ENTROPY_SRC_STDRAND    0
#define ALGLIB_ENTROPY_SRC_OPENSSL    1

/* malloc types for AE_MALLOC */
#define AE_STDLIB_MALLOC            200
#define AE_BASIC_STATIC_MALLOC      201
#if !defined(AE_MALLOC)
#define AE_MALLOC AE_STDLIB_MALLOC
#endif

#define AE_LOCK_ALIGNMENT 16

/* automatically determine compiler */
#define AE_MSVC 1
#define AE_GNUC 2
#define AE_SUNC 3
#define AE_COMPILER AE_UNKNOWN
#ifdef __GNUC__
#undef AE_COMPILER
#define AE_COMPILER AE_GNUC
#endif
#if defined(__SUNPRO_C)||defined(__SUNPRO_CC)
#undef AE_COMPILER
#define AE_COMPILER AE_SUNC
#endif
#ifdef _MSC_VER
#undef AE_COMPILER
#define AE_COMPILER AE_MSVC
#endif

/* compiler-specific definitions */
#if AE_COMPILER==AE_MSVC
#define ALIGNED __declspec(align(8))
#elif AE_COMPILER==AE_GNUC
#define ALIGNED __attribute__((aligned(8)))
#else
#define ALIGNED
#endif

/* state flags */
#define _ALGLIB_FLG_THREADING_MASK_WRK              0x7
#define _ALGLIB_FLG_THREADING_MASK_CBK              (0x7<<3)
#define _ALGLIB_FLG_THREADING_MASK_ALL              (_ALGLIB_FLG_THREADING_MASK_WRK|_ALGLIB_FLG_THREADING_MASK_CBK)
#define _ALGLIB_FLG_THREADING_SHIFT                 0
#define _ALGLIB_FLG_THREADING_USE_GLOBAL            0x0
#define _ALGLIB_FLG_THREADING_SERIAL                0x1
#define _ALGLIB_FLG_THREADING_PARALLEL              0x2
#define _ALGLIB_FLG_THREADING_SERIAL_CALLBACKS      (0x1<<3)
#define _ALGLIB_FLG_THREADING_PARALLEL_CALLBACKS    (0x2<<3)


/* now we are ready to include headers */
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>
#include <math.h>
#include <stddef.h>

#if defined(AE_HAVE_STDINT)
#include <stdint.h>
#endif

/*
 * Intel SIMD intrinsics
 *
 * Preprocessor directives below:
 * - include headers for SSE2/AVX2/AVX2+FMA3 intrinsics
 * - defines _ALGLIB_HAS_SSE2_INTRINSICS, _ALGLIB_HAS_AVX2_INTRINSICS and _ALGLIB_HAS_FMA_INTRINSICS definitions
 *
 * These actions are performed when we have:
 * - x86 architecture definition (AE_CPU==AE_INTEL)
 * - compiler which supports intrinsics
 *
 * Presence of _ALGLIB_HAS_???_INTRINSICS does NOT mean that our CPU
 * actually supports these intrinsics - such things should be determined
 * at runtime with ae_cpuid() call. It means that we are working under
 * Intel and out compiler can issue SIMD-capable code.
 *
 */
#if defined(AE_CPU)
#if AE_CPU==AE_INTEL
    /*
     * Intel definitions
     */
    #if AE_COMPILER==AE_MSVC
        /*
         * MSVC is detected.
         * We assume that compiler supports all instruction sets
         * unless something is explicitly turned off.
         */
        #if !defined(AE_NO_SSE2)
            #include <emmintrin.h>
            #define AE_HAS_SSE2_INTRINSICS
            #define _ALGLIB_HAS_SSE2_INTRINSICS
            #if !defined(AE_NO_AVX2)
                #include <intrin.h>
                #define _ALGLIB_HAS_AVX2_INTRINSICS
                #if !defined(AE_NO_FMA)
                    #define _ALGLIB_HAS_FMA_INTRINSICS
                #endif
            #endif
        #endif
    #elif AE_COMPILER==AE_GNUC
        /*
         * GCC/CLANG/ICC is detected.
         * We assume that compiler supports all instruction sets
         * unless something is explicitly turned off.
         */
        #if !defined(AE_NO_SSE2)
            #include <xmmintrin.h>
            #define AE_HAS_SSE2_INTRINSICS
            #define _ALGLIB_HAS_SSE2_INTRINSICS
            #if !defined(AE_NO_AVX2)
                #include <immintrin.h>
                #define _ALGLIB_HAS_AVX2_INTRINSICS
                #if !defined(AE_NO_FMA)
                    #define _ALGLIB_HAS_FMA_INTRINSICS
                #endif
            #endif
        #endif
    #elif AE_COMPILER==AE_SUNC
        /*
         * Sun studio
         */
        #include <xmmintrin.h>
        #include <emmintrin.h>
        #define AE_HAS_SSE2_INTRINSICS
        #define _ALGLIB_HAS_SSE2_INTRINSICS
        #include <immintrin.h>
        #define _ALGLIB_HAS_AVX2_INTRINSICS
        #define _ALGLIB_HAS_FMA_INTRINSICS
    #else
        /*
         * Unknown compiler
         */
        #if !defined(AE_NO_SSE2)
            #include <immintrin.h>
            #define AE_HAS_SSE2_INTRINSICS
            #define _ALGLIB_HAS_SSE2_INTRINSICS
            #if !defined(AE_NO_AVX2)
                #define _ALGLIB_HAS_AVX2_INTRINSICS
                #if !defined(AE_NO_FMA)
                    #define _ALGLIB_HAS_FMA_INTRINSICS
                #endif
            #endif
        #endif
    #endif

    /*
     * Intel integrity checks
     */
    #if defined(_ALGLIB_INTEGRITY_CHECKS_ONCE)
        #if defined(_ALGLIB_FAIL_WITHOUT_FMA_INTRINSICS) && !defined(_ALGLIB_HAS_FMA_INTRINSICS)
#error ALGLIB was requested to fail without FMA intrinsics
        #endif
    #endif
#endif
#endif

/*$ Body $*/

/* if we work under C++ environment, define several conditions */
#ifdef AE_USE_CPP
#define AE_USE_CPP_BOOL
#define AE_USE_CPP_SERIALIZATION
#include <iostream>
#endif

/*
 * define ae_int32_t, ae_int64_t, ae_int_t, ae_bool, ae_complex, ae_error_type and ae_datatype
 */

#if defined(AE_INT32_T)
typedef AE_INT32_T ae_int32_t;
#endif
#if defined(AE_HAVE_STDINT) && !defined(AE_INT32_T)
typedef int32_t ae_int32_t;
#endif
#if !defined(AE_HAVE_STDINT) && !defined(AE_INT32_T)
#if AE_COMPILER==AE_MSVC
typedef __int32 ae_int32_t;
#endif
#if (AE_COMPILER==AE_GNUC) || (AE_COMPILER==AE_SUNC) || (AE_COMPILER==AE_UNKNOWN)
typedef int ae_int32_t;
#endif
#endif

#if defined(AE_INT64_T)
typedef AE_INT64_T ae_int64_t;
#endif
#if defined(AE_HAVE_STDINT) && !defined(AE_INT64_T)
typedef int64_t ae_int64_t;
#endif
#if !defined(AE_HAVE_STDINT) && !defined(AE_INT64_T)
#if AE_COMPILER==AE_MSVC
typedef __int64 ae_int64_t;
#endif
#if (AE_COMPILER==AE_GNUC) || (AE_COMPILER==AE_SUNC) || (AE_COMPILER==AE_UNKNOWN)
typedef signed long long ae_int64_t;
#endif
#endif

#if defined(AE_UINT64_T)
typedef AE_UINT64_T ae_uint64_t;
#endif
#if defined(AE_HAVE_STDINT) && !defined(AE_UINT64_T)
typedef uint64_t ae_uint64_t;
#endif
#if !defined(AE_HAVE_STDINT) && !defined(AE_UINT64_T)
#if AE_COMPILER==AE_MSVC
typedef unsigned __int64 ae_uint64_t;
#endif
#if (AE_COMPILER==AE_GNUC) || (AE_COMPILER==AE_SUNC) || (AE_COMPILER==AE_UNKNOWN)
typedef unsigned long long ae_uint64_t;
#endif
#endif

#if !defined(AE_INT_T)
typedef ptrdiff_t ae_int_t;
#endif

#if !defined(AE_USE_CPP_BOOL)
#define ae_bool char
#define ae_true 1
#define ae_false 0
#else
#define ae_bool bool
#define ae_true true
#define ae_false false
#endif

typedef struct { double x, y; } ae_complex;

typedef enum
{
    ERR_OK = 0,
    ERR_OUT_OF_MEMORY = 1,
    ERR_XARRAY_TOO_LARGE = 2,
    ERR_ASSERTION_FAILED = 3
} ae_error_type;

typedef ae_int_t ae_datatype;

/*
 * other definitions
 */
enum { OWN_CALLER=1, OWN_AE=2 };
enum { ACT_UNCHANGED=1, ACT_SAME_LOCATION=2, ACT_NEW_LOCATION=3 };
enum { DT_BOOL=1, DT_BYTE=1, DT_INT=2, DT_REAL=3, DT_COMPLEX=4 };
enum { CPU_SSE2=0x1, CPU_AVX2=0x2, CPU_FMA=0x4 };
typedef void(*ae_destructor)(void*);

/************************************************************************
x-string (zero-terminated):
    owner       OWN_CALLER or OWN_AE. Determines what to do on realloc().
                If vector is owned by caller, X-interface  will  just set
                ptr to NULL before realloc(). If it is  owned  by  X,  it
                will call ae_free/x_free/aligned_free family functions.

    last_action ACT_UNCHANGED, ACT_SAME_LOCATION, ACT_NEW_LOCATION
                contents is either: unchanged, stored at the same location,
                stored at the new location.
                this field is set on return from X.

    ptr         pointer to the actual data

Members of this structure are ae_int64_t to avoid alignment problems.
************************************************************************/
typedef struct
{
    ALIGNED ae_int64_t     owner;
    ALIGNED ae_int64_t     last_action;
    ALIGNED char *ptr;
} x_string;

/************************************************************************
x-vector:
    cnt         number of elements

    datatype    one of the DT_XXXX values

    owner       OWN_CALLER or OWN_AE. Determines what to do on realloc().
                If vector is owned by caller, X-interface  will  just set
                ptr to NULL before realloc(). If it is  owned  by  X,  it
                will call ae_free/x_free/aligned_free family functions.

    last_action ACT_UNCHANGED, ACT_SAME_LOCATION, ACT_NEW_LOCATION
                contents is either: unchanged, stored at the same location,
                stored at the new location.
                this field is set on return from X interface and may be
                used by caller as hint when deciding what to do with data
                (if it was ACT_UNCHANGED or ACT_SAME_LOCATION, no array
                reallocation or copying is required).

    ptr         pointer to the actual data

Members of this structure are ae_int64_t to avoid alignment problems.
************************************************************************/
typedef struct
{
    ae_int64_t     cnt;
    ae_int64_t     datatype;
    ae_int64_t     owner;
    ae_int64_t     last_action;
    union
    {
        void *p_ptr;
        ae_int64_t portable_alignment_enforcer;
    } x_ptr;
} x_vector;


/************************************************************************
x-matrix:
    rows        number of rows. may be zero only when cols is zero too.

    cols        number of columns. may be zero only when rows is zero too.

    stride      stride, i.e. distance between first elements of rows (in bytes)

    datatype    one of the DT_XXXX values

    owner       OWN_CALLER or OWN_AE. Determines what to do on realloc().
                If vector is owned by caller, X-interface  will  just set
                ptr to NULL before realloc(). If it is  owned  by  X,  it
                will call ae_free/x_free/aligned_free family functions.

    last_action ACT_UNCHANGED, ACT_SAME_LOCATION, ACT_NEW_LOCATION
                contents is either: unchanged, stored at the same location,
                stored at the new location.
                this field is set on return from X interface and may be
                used by caller as hint when deciding what to do with data
                (if it was ACT_UNCHANGED or ACT_SAME_LOCATION, no array
                reallocation or copying is required).

    ptr         pointer to the actual data, stored rowwise

Members of this structure are ae_int64_t to avoid alignment problems.
************************************************************************/
typedef struct
{
    ae_int64_t     rows;
    ae_int64_t     cols;
    ae_int64_t     stride;
    ae_int64_t     datatype;
    ae_int64_t     owner;
    ae_int64_t     last_action;
    union
    {
        void *p_ptr;
        ae_int64_t portable_alignment_enforcer;
    } x_ptr;
} x_matrix;


/************************************************************************
dynamic block which may be automatically deallocated during stack unwinding

p_next          next block in the stack unwinding list.
                NULL means that this block is not in the list
deallocator     deallocator function which should be used to deallocate block.
                NULL for "special" blocks (frame/stack boundaries)
ptr             pointer which should be passed to the deallocator.
                may be null (for zero-size block), DYN_BOTTOM or DYN_FRAME
                for "special" blocks (frame/stack boundaries).

valgrind_hint   is a special field which stores a special hint pointer for
                Valgrind and other similar memory checking tools.  ALGLIB
                manually aligns pointers obtained via malloc, so ptr usually
                points to location past the beginning  of  the  actuallly
                allocated memory. In such cases memory testing tools  may
                report "(possibly) lost" memory.
                
                This "hint" field stores  pointer  actually  returned  by
                malloc (or NULL, if for some reason  we  do  not  support
                this feature). This field is used merely as  a  hint  for
                Valgrind - it should NOT be used for anything else.

************************************************************************/
typedef struct ae_dyn_block
{
    struct ae_dyn_block * volatile p_next;
    ae_destructor deallocator;
    void * volatile ptr;
    void* valgrind_hint;
} ae_dyn_block;


/************************************************************************
frame marker
************************************************************************/
typedef struct ae_frame
{
    ae_dyn_block db_marker;
} ae_frame;

/************************************************************************
ALGLIB environment state
************************************************************************/
typedef struct ae_state
{
    /*
     * endianness type: AE_LITTLE_ENDIAN or AE_BIG_ENDIAN
     */
    ae_int_t endianness;
    
    /*
     * double value for NAN
     */
    double v_nan;
    
    /*
     * double value for +INF
     */
    double v_posinf;
    
    /*
     * double value for -INF
     */
    double v_neginf;
    
    /*
     * pointer to the top block in a stack of frames
     * which hold dynamically allocated objects
     */
    ae_dyn_block * volatile p_top_block;
    ae_dyn_block last_block;
    
    /*
     * jmp_buf pointer for internal C-style exception handling
     */
    jmp_buf * volatile break_jump;

    /*
     * ae_error_type of the last error (filled when exception is thrown)
     */
    ae_error_type volatile last_error;
    
    /*
     * human-readable message (filled when exception is thrown)
     */
    const char* volatile error_msg;
    
    /*
     * Flags: call-local settings for ALGLIB
     */
    ae_uint64_t flags;
    
    /*
     * threading information:
     * a) current thread pool
     * b) current worker thread
     * c) parent task (one we are solving right now)
     * d) thread exception handler (function which must be called
     *    by ae_assert before raising exception).
     *
     * NOTE: we use void* to store pointers in order to avoid explicit dependency on smp.h
     */
    void *worker_thread;
    void *parent_task;
    void (*thread_exception_handler)(void*);
    
} ae_state;

typedef void(*ae_constructor)(void*,ae_state*,ae_bool);
typedef void(*ae_copy_constructor)(void*,const void*,ae_state*,ae_bool);


/************************************************************************
Serializer:

* ae_stream_writer type is a function pointer for stream  writer  method;
  this pointer is used by X-core for out-of-core serialization  (say,  to
  serialize ALGLIB structure directly to managed C# stream).
  
  This function accepts two parameters: pointer to  ANSI  (7-bit)  string
  and pointer-sized integer passed to serializer  during  initialization.
  String being passed is a part of the data stream; aux paramerer may  be
  arbitrary value intended to be used by actual implementation of  stream
  writer. String parameter may include spaces and  linefeed  symbols,  it
  should be written to stream as is.
  
  Return value must be zero for success or non-zero for failure.
  
* ae_stream_reader type is a function pointer for stream  reader  method;
  this pointer is used by X-core for out-of-core unserialization (say, to
  unserialize ALGLIB structure directly from managed C# stream).
  
  This function accepts three parameters: pointer-sized integer passed to
  serializer  during  initialization; number  of  symbols  to  read  from
  stream; pointer to buffer used to store next  token  read  from  stream
  (ANSI encoding is used, buffer is large enough to store all symbols and
  trailing zero symbol).
  
  Number of symbols to read is always positive.
  
  After being called by X-core, this function must:
  * skip all space and linefeed characters from the current  position  at
    the stream and until first non-space non-linefeed character is found
  * read exactly cnt symbols  from  stream  to  buffer;  check  that  all
    symbols being read are non-space non-linefeed ones
  * append trailing zero symbol to buffer
  * return value must be zero on success, non-zero if  even  one  of  the
    conditions above fails. When reader returns non-zero value,  contents
    of buf is not used.
************************************************************************/
typedef char(*ae_stream_writer)(const char *p_string, ae_int_t aux);
typedef char(*ae_stream_reader)(ae_int_t aux, ae_int_t cnt, char *p_buf);

typedef struct
{
    ae_int_t mode;
    ae_int_t entries_needed;
    ae_int_t entries_saved;
    ae_int_t bytes_asked;
    ae_int_t bytes_written;

#ifdef AE_USE_CPP_SERIALIZATION
    std::string     *out_cppstr;
#endif
    char            *out_str; /* pointer to the current position at the output buffer; advanced with each write operation */
    const char      *in_str;  /* pointer to the current position at the input  buffer; advanced with each read  operation */
    ae_int_t         stream_aux;
    ae_stream_writer stream_writer;
    ae_stream_reader stream_reader;
} ae_serializer;


typedef struct ae_vector
{
    /*
     * Number of elements in array, cnt>=0
     */
    ae_int_t cnt;
    
    /*
     * Either DT_BOOL/DT_BYTE, DT_INT, DT_REAL or DT_COMPLEX
     */
    ae_datatype datatype;
    
    /*
     * If ptr points to memory owned and managed by ae_vector itself,
     * this field is ae_false. If vector was attached to x_vector structure
     * with ae_vector_init_attach_to_x(), this field is ae_true.
     */
    ae_bool is_attached;
    
    /*
     * ae_dyn_block structure which manages data in ptr. This structure
     * is responsible for automatic deletion of object when its frame
     * is destroyed.
     */
    ae_dyn_block data;
    
    /*
     * Pointer to data.
     * User usually works with this field.
     */
    union
    {
        void *p_ptr;
        ae_bool *p_bool;
        unsigned char *p_ubyte;
        ae_int_t *p_int;
        double *p_double;
        ae_complex *p_complex;
    } ptr;
} ae_vector;

typedef struct ae_matrix
{
    ae_int_t rows;
    ae_int_t cols;
    ae_int_t stride;
    ae_datatype datatype;
    
    /*
     * If ptr points to memory owned and managed by ae_vector itself,
     * this field is ae_false. If vector was attached to x_vector structure
     * with ae_vector_init_attach_to_x(), this field is ae_true.
     */
    ae_bool is_attached;
    
    ae_dyn_block data;
    union
    {
        void *p_ptr;
        void **pp_void;
        ae_bool **pp_bool;
        ae_int_t **pp_int;
        double **pp_double;
        ae_complex **pp_complex;
    } ptr;
} ae_matrix;

typedef struct ae_smart_ptr
{
    /* pointer to subscriber; all changes in ptr are translated to subscriber */
    void **subscriber;
    
    /* pointer to object */
    void *ptr;
    
    /* whether smart pointer owns ptr */
    ae_bool is_owner;
    
    /* whether object pointed by ptr is dynamic - clearing such object requires BOTH
       calling destructor function AND calling ae_free for memory occupied by object. */
    ae_bool is_dynamic;
    
    /* size of object; this field is used when we pass the object to ae_obj_array; it is zero for non-owned pointers */
    ae_int_t size_of_object;
    
    /* copy constructor for the pointer */
    ae_copy_constructor copy_constructor;
    
    /* destructor function for pointer; clears all dynamically allocated memory */
    ae_destructor destructor;
    
    /* frame entry; used to ensure automatic deallocation of smart pointer in case of exception/exit */
    ae_dyn_block frame_entry;
} ae_smart_ptr;


/*************************************************************************
Lock.

This structure provides OS-independent non-reentrant lock:
* under Windows/Posix systems it uses system-provided locks
* under Boost it uses OS-independent lock provided by Boost package
* when no OS is defined, it uses "fake lock" (just stub which is not thread-safe):
  a) "fake lock" can be in locked or free mode
  b) "fake lock" can be used only from one thread - one which created lock
  c) when thread acquires free lock, it immediately returns
  d) when thread acquires busy lock, program is terminated
     (because lock is already acquired and no one else can free it)
*************************************************************************/
typedef struct
{
    /*
     * Pointer to _lock structure. This pointer has type void* in order to
     * make header file OS-independent (lock declaration depends on OS).
     */
    void *lock_ptr;
    
    /*
     * For eternal=false this field manages pointer to _lock structure.
     *
     * ae_dyn_block structure is responsible for automatic deletion of
     * the memory allocated for the pointer when its frame is destroyed.
     */
    ae_dyn_block db;
    
    /*
     * Whether we have eternal lock object (used by thread pool) or
     * transient lock. Eternal locks are allocated without using ae_dyn_block
     * structure and do not allow deallocation.
     */
    ae_bool eternal;
} ae_lock;

typedef struct ae_obj_array
{   
    /* elements count */
    ae_int_t cnt;
    
    /* storage size */
    ae_int_t capacity;
    
    /* whether capacity can be automatically increased or not */
    ae_bool fixed_capacity;
    
    /* pointers to objects */
    void **pp_obj_ptr;
    
    /* pointers to object sizes */
    ae_int_t *pp_obj_sizes;
    
    /* pointers to deallocators */
    ae_copy_constructor *pp_copy;
    
    /* pointers to destructors */
    ae_destructor *pp_destroy;
    
    /* primary synchronization lock, used for thread-safe appends */
    ae_lock array_lock;
    
    /* a lock used to emulate full memory fence */
    ae_lock barrier_lock;
    
    /* frame entry; used to ensure automatic deallocation of array and its elements in case of exception/exit */
    ae_dyn_block frame_entry;
} ae_obj_array;

/*************************************************************************
Shared pool: data structure used to provide thread-safe access to pool  of
temporary variables.
*************************************************************************/
typedef struct ae_shared_pool_entry
{
    void * volatile obj;
    void * volatile next_entry;
} ae_shared_pool_entry;

typedef struct ae_shared_pool
{
    /* lock object which protects pool */
    ae_lock pool_lock;
    
    /* seed object (used to create new instances of temporaries) */
    void                    * volatile seed_object;
    
    /*
     * list of recycled OBJECTS:
     * 1. entries in this list store pointers to recycled objects
     * 2. every time we retrieve object, we retrieve first entry from this list,
     *    move it to recycled_entries and return its obj field to caller/
     */
    ae_shared_pool_entry    * volatile recycled_objects;
    
    /* 
     * list of recycled ENTRIES:
     * 1. this list holds entries which are not used to store recycled objects;
     *    every time recycled object is retrieved, its entry is moved to this list.
     * 2. every time object is recycled, we try to fetch entry for him from this list
     *    before allocating it with malloc()
     */
    ae_shared_pool_entry    * volatile recycled_entries;
    
    /* enumeration pointer, points to current recycled object*/
    ae_shared_pool_entry    * volatile enumeration_counter;
    
    /* size of object; this field is used when we call malloc() for new objects */
    ae_int_t                size_of_object;
    
    /* initializer function; accepts pointer to malloc'ed object, initializes its fields */
    ae_constructor init;
    
    /* copy constructor; accepts pointer to malloc'ed, but not initialized object */
    ae_copy_constructor init_copy;
    
    /* destructor function; */
    ae_destructor destroy;
    
    /* frame entry; contains pointer to the pool object itself */
    ae_dyn_block frame_entry;
} ae_shared_pool;

void ae_never_call_it();
void ae_set_dbg_flag(ae_int64_t flag_id, ae_int64_t flag_val);
ae_int64_t ae_get_dbg_value(ae_int64_t id);
void ae_set_global_threading(ae_uint64_t flg_value);
ae_uint64_t ae_get_global_threading();

/************************************************************************
Debugging and tracing functions
************************************************************************/
void ae_set_error_flag(ae_bool *p_flag, ae_bool cond, const char *filename, int lineno, const char *xdesc);
const char * ae_get_last_error_file();
int          ae_get_last_error_line();
const char * ae_get_last_error_xdesc();

void ae_trace_file(const char *tags, const char *filename);
void ae_trace_disable();
ae_bool ae_is_trace_enabled(const char *tag);
void ae_trace(const char * printf_fmt, ...);

ae_int_t ae_tickcount();


/************************************************************************
...
************************************************************************/
ae_int_t ae_misalignment(const void *ptr, size_t alignment);
void* ae_align(void *ptr, size_t alignment);
ae_int_t ae_get_effective_workers(ae_int_t nworkers);
void  ae_optional_atomic_add_i(ae_int_t *p, ae_int_t v);
void  ae_optional_atomic_sub_i(ae_int_t *p, ae_int_t v);
void  ae_mfence(ae_lock *lock);
ae_int_t ae_unsafe_read_aeint(ae_int_t *p);
void ae_unsafe_write_aeint(ae_int_t *dst, ae_int_t v);

void* aligned_malloc(size_t size, size_t alignment);
void* aligned_extract_ptr(void *block);
void  aligned_free(void *block);
void* eternal_malloc(size_t size);
#if AE_MALLOC==AE_BASIC_STATIC_MALLOC
void set_memory_pool(void *ptr, size_t size);
void memory_pool_stats(ae_int_t *bytes_used, ae_int_t *bytes_free);
#endif

void* ae_malloc(size_t size, ae_state *state);
void  ae_free(void *p);
ae_int_t ae_sizeof(ae_datatype datatype);
ae_bool ae_check_zeros(const void *ptr, ae_int_t n);
void ae_touch_ptr(void *p);

ae_int_t ae_rand();
ae_int_t ae_rand_max();

void ae_state_init(ae_state *state);
void ae_state_clear(ae_state *state);
void ae_state_set_break_jump(ae_state *state, jmp_buf *buf);
void ae_state_set_flags(ae_state *state, ae_uint64_t flags);
void ae_clean_up_before_breaking(ae_state *state);
void ae_break(ae_state *state, ae_error_type error_type, const char *msg);

void ae_frame_make(ae_state *state, ae_frame *tmp);
void ae_frame_leave(ae_state *state);

void ae_db_attach(ae_dyn_block *block, ae_state *state);
void ae_db_init(ae_dyn_block *block, ae_int_t size, ae_state *state, ae_bool make_automatic);
void ae_db_realloc(ae_dyn_block *block, ae_int_t size, ae_state *state);
void ae_db_free(ae_dyn_block *block);
void ae_db_swap(ae_dyn_block *block1, ae_dyn_block *block2);

void ae_vector_init(ae_vector *dst, ae_int_t size, ae_datatype datatype, ae_state *state, ae_bool make_automatic);
void ae_vector_init_copy(ae_vector *dst, const ae_vector *src, ae_state *state, ae_bool make_automatic);
void ae_vector_init_from_x(ae_vector *dst, x_vector *src, ae_state *state, ae_bool make_automatic);
void ae_vector_init_attach_to_x(ae_vector *dst, x_vector *src, ae_state *state, ae_bool make_automatic);
void ae_vector_set_length(ae_vector *dst, ae_int_t newsize, ae_state *state);
void ae_vector_resize(ae_vector *dst, ae_int_t newsize, ae_state *state);
void ae_vector_clear(ae_vector *dst);
void ae_vector_destroy(ae_vector *dst);
void ae_swap_vectors(ae_vector *vec1, ae_vector *vec2);

void ae_matrix_init(ae_matrix *dst, ae_int_t rows, ae_int_t cols, ae_datatype datatype, ae_state *state, ae_bool make_automatic);
void ae_matrix_init_copy(ae_matrix *dst, const ae_matrix *src, ae_state *state, ae_bool make_automatic);
void ae_matrix_init_from_x(ae_matrix *dst, x_matrix *src, ae_state *state, ae_bool make_automatic);
void ae_matrix_init_attach_to_x(ae_matrix *dst, x_matrix *src, ae_state *state, ae_bool make_automatic);
void ae_matrix_set_length(ae_matrix *dst, ae_int_t rows, ae_int_t cols, ae_state *state);
void ae_matrix_clear(ae_matrix *dst);
void ae_matrix_destroy(ae_matrix *dst);
void ae_swap_matrices(ae_matrix *mat1, ae_matrix *mat2);

void ae_smart_ptr_init(ae_smart_ptr *dst, void **subscriber, ae_state *state, ae_bool make_automatic);
void ae_smart_ptr_clear(void *_dst); /* accepts ae_smart_ptr* */
void ae_smart_ptr_destroy(void *_dst);
void ae_smart_ptr_assign(ae_smart_ptr *dst, void *new_ptr, ae_bool is_owner, ae_bool is_dynamic, ae_int_t obj_size, ae_copy_constructor cc, ae_destructor dd);
void ae_smart_ptr_release(ae_smart_ptr *dst);

void ae_obj_array_init(ae_obj_array *dst, ae_state *state, ae_bool make_automatic);
void ae_obj_array_init_copy(ae_obj_array *dst, const ae_obj_array *src, ae_state *state, ae_bool make_automatic);
void ae_obj_array_clear(ae_obj_array *dst);
void ae_obj_array_destroy(ae_obj_array *dst);
ae_int_t ae_obj_array_get_length(ae_obj_array *dst);
void ae_obj_array_fixed_capacity(ae_obj_array *arr, ae_int_t idx, ae_state *state);
void ae_obj_array_get(ae_obj_array *arr, ae_int_t idx, ae_smart_ptr *ptr, ae_state *state);
void ae_obj_array_set_transfer(ae_obj_array *arr, ae_int_t idx, ae_smart_ptr *ptr, ae_state *state);
ae_int_t ae_obj_array_append_transfer(ae_obj_array *arr, ae_smart_ptr *ptr, ae_state *state);

void ae_yield();
void ae_init_lock(ae_lock *lock, ae_state *state, ae_bool make_automatic);
void ae_init_lock_eternal(ae_lock *lock);
void ae_acquire_lock(ae_lock *lock);
void ae_release_lock(ae_lock *lock);
void ae_free_lock(ae_lock *lock);

void ae_shared_pool_init(void *_dst, ae_state *state, ae_bool make_automatic);
void ae_shared_pool_init_copy(void *_dst, const void *_src, ae_state *state, ae_bool make_automatic);
void ae_shared_pool_clear(void *dst);
void ae_shared_pool_destroy(void *dst);
ae_bool ae_shared_pool_is_initialized(void *_dst);
void ae_shared_pool_set_seed(
    ae_shared_pool  *dst,
    const void      *seed_object,
    ae_int_t        size_of_object,
    ae_constructor  constructor,
    ae_copy_constructor copy_constructor,
    ae_destructor   destructor,
    ae_state        *state);
void ae_shared_pool_retrieve(
    ae_shared_pool  *pool,
    ae_smart_ptr    *pptr,
    ae_state        *state);
void ae_shared_pool_recycle(
    ae_shared_pool  *pool,
    ae_smart_ptr    *pptr,
    ae_state        *state);
void ae_shared_pool_clear_recycled(
    ae_shared_pool  *pool,
    ae_state        *state);
void ae_shared_pool_first_recycled(
    ae_shared_pool  *pool,
    ae_smart_ptr    *pptr,
    ae_state        *state);
void ae_shared_pool_next_recycled(
    ae_shared_pool  *pool,
    ae_smart_ptr    *pptr,
    ae_state        *state);
void ae_shared_pool_reset(
    ae_shared_pool  *pool,
    ae_state        *state);

void ae_x_set_vector(x_vector *dst, ae_vector *src, ae_state *state);
void ae_x_set_matrix(x_matrix *dst, ae_matrix *src, ae_state *state);
void ae_x_attach_to_vector(x_vector *dst, ae_vector *src);
void ae_x_attach_to_matrix(x_matrix *dst, ae_matrix *src);

void x_vector_clear(x_vector *dst);

ae_bool x_is_symmetric(x_matrix *a);
ae_bool x_is_hermitian(x_matrix *a);
ae_bool x_force_symmetric(x_matrix *a);
ae_bool x_force_hermitian(x_matrix *a);
ae_bool ae_is_symmetric(ae_matrix *a);
ae_bool ae_is_hermitian(ae_matrix *a);
ae_bool ae_force_symmetric(ae_matrix *a);
ae_bool ae_force_hermitian(ae_matrix *a);

void ae_serializer_init(ae_serializer *serializer);
void ae_serializer_clear(ae_serializer *serializer);

void ae_serializer_alloc_start(ae_serializer *serializer);
void ae_serializer_alloc_entry(ae_serializer *serializer);
void ae_serializer_alloc_byte_array(ae_serializer *serializer, const ae_vector *bytes);
ae_int_t ae_serializer_get_alloc_size(ae_serializer *serializer);

#ifdef AE_USE_CPP_SERIALIZATION
void ae_serializer_sstart_str(ae_serializer *serializer, std::string *buf);
void ae_serializer_ustart_str(ae_serializer *serializer, const std::string *buf);
void ae_serializer_sstart_stream(ae_serializer *serializer, std::ostream *stream);
void ae_serializer_ustart_stream(ae_serializer *serializer, const std::istream *stream);
#endif
void ae_serializer_sstart_str(ae_serializer *serializer, char *buf);
void ae_serializer_ustart_str(ae_serializer *serializer, const char *buf);
void ae_serializer_sstart_stream(ae_serializer *serializer, ae_stream_writer writer, ae_int_t aux);
void ae_serializer_ustart_stream(ae_serializer *serializer, ae_stream_reader reader, ae_int_t aux);

void ae_serializer_serialize_bool(ae_serializer *serializer, ae_bool v, ae_state *state);
void ae_serializer_serialize_int(ae_serializer *serializer, ae_int_t v, ae_state *state);
void ae_serializer_serialize_int64(ae_serializer *serializer, ae_int64_t v, ae_state *state);
void ae_serializer_serialize_double(ae_serializer *serializer, double v, ae_state *state);
void ae_serializer_serialize_byte_array(ae_serializer *serializer, const ae_vector *bytes, ae_state *state);
void ae_serializer_unserialize_bool(ae_serializer *serializer, ae_bool *v, ae_state *state);
void ae_serializer_unserialize_int(ae_serializer *serializer, ae_int_t *v, ae_state *state);
void ae_serializer_unserialize_int64(ae_serializer *serializer, ae_int64_t *v, ae_state *state);
void ae_serializer_unserialize_double(ae_serializer *serializer, double *v, ae_state *state);
void ae_serializer_unserialize_byte_array(ae_serializer *serializer, ae_vector *bytes, ae_state *state);

void ae_serializer_stop(ae_serializer *serializer, ae_state *state);

/************************************************************************
Service functions
************************************************************************/
void ae_assert(ae_bool cond, const char *msg, ae_state *state);
ae_int_t ae_cpuid();

/************************************************************************
Real math functions:
* IEEE-compliant floating point comparisons
* standard functions
************************************************************************/
ae_bool ae_fp_eq(double v1, double v2);
ae_bool ae_fp_neq(double v1, double v2);
ae_bool ae_fp_less(double v1, double v2);
ae_bool ae_fp_less_eq(double v1, double v2);
ae_bool ae_fp_greater(double v1, double v2);
ae_bool ae_fp_greater_eq(double v1, double v2);

ae_bool ae_isfinite_stateless(double x, ae_int_t endianness);
ae_bool ae_isnan_stateless(double x,    ae_int_t endianness);
ae_bool ae_isinf_stateless(double x,    ae_int_t endianness);
ae_bool ae_isposinf_stateless(double x, ae_int_t endianness);
ae_bool ae_isneginf_stateless(double x, ae_int_t endianness);

ae_int_t ae_get_endianness();

ae_bool ae_isfinite(double x,ae_state *state);
ae_bool ae_isnan(double x,   ae_state *state);
ae_bool ae_isinf(double x,   ae_state *state);
ae_bool ae_isposinf(double x,ae_state *state);
ae_bool ae_isneginf(double x,ae_state *state);

double   ae_fabs(double x,   ae_state *state);
ae_int_t ae_iabs(ae_int_t x, ae_state *state);
double   ae_sqr(double x,    ae_state *state);
double   ae_sqrt(double x,   ae_state *state);

ae_int_t ae_sign(double x,   ae_state *state);
ae_int_t ae_round(double x,  ae_state *state);
ae_int_t ae_trunc(double x,  ae_state *state);
ae_int_t ae_ifloor(double x, ae_state *state);
ae_int_t ae_iceil(double x,  ae_state *state);

ae_int_t ae_maxint(ae_int_t m1, ae_int_t m2, ae_state *state);
ae_int_t ae_minint(ae_int_t m1, ae_int_t m2, ae_state *state);
double   ae_maxreal(double m1, double m2, ae_state *state);
double   ae_minreal(double m1, double m2, ae_state *state);
double   ae_randomreal(ae_state *state);
ae_int_t ae_randominteger(ae_int_t maxv, ae_state *state);

double   ae_sin(double x, ae_state *state);
double   ae_cos(double x, ae_state *state);
double   ae_tan(double x, ae_state *state);
double   ae_sinh(double x, ae_state *state);
double   ae_cosh(double x, ae_state *state);
double   ae_tanh(double x, ae_state *state);
double   ae_asin(double x, ae_state *state);
double   ae_acos(double x, ae_state *state);
double   ae_atan(double x, ae_state *state);
double   ae_atan2(double y, double x, ae_state *state);

double   ae_log(double x, ae_state *state);
double   ae_pow(double x, double y, ae_state *state);
double   ae_exp(double x, ae_state *state);

/************************************************************************
Complex math functions:
* basic arithmetic operations
* standard functions
************************************************************************/
ae_complex ae_complex_from_i(ae_int_t v);
ae_complex ae_complex_from_d(double v);

ae_complex ae_c_neg(ae_complex lhs);
ae_bool ae_c_eq(ae_complex lhs,       ae_complex rhs);
ae_bool ae_c_neq(ae_complex lhs,      ae_complex rhs);
ae_complex ae_c_add(ae_complex lhs,   ae_complex rhs);
ae_complex ae_c_mul(ae_complex lhs,   ae_complex rhs);
ae_complex ae_c_sub(ae_complex lhs,   ae_complex rhs);
ae_complex ae_c_div(ae_complex lhs,   ae_complex rhs);
ae_bool ae_c_eq_d(ae_complex lhs,     double rhs);
ae_bool ae_c_neq_d(ae_complex lhs,    double rhs);
ae_complex ae_c_add_d(ae_complex lhs, double rhs);
ae_complex ae_c_mul_d(ae_complex lhs, double rhs);
ae_complex ae_c_sub_d(ae_complex lhs, double rhs);
ae_complex ae_c_d_sub(double lhs,     ae_complex rhs);
ae_complex ae_c_div_d(ae_complex lhs, double rhs);
ae_complex ae_c_d_div(double lhs,   ae_complex rhs);

ae_complex ae_c_conj(ae_complex lhs, ae_state *state);
ae_complex ae_c_sqr(ae_complex lhs, ae_state *state);
double     ae_c_abs(ae_complex z, ae_state *state);

/************************************************************************
Complex BLAS operations
************************************************************************/
ae_complex ae_v_cdotproduct(const ae_complex *v0, ae_int_t stride0, const char *conj0, const ae_complex *v1, ae_int_t stride1, const char *conj1, ae_int_t n);
void ae_v_cmove(ae_complex *vdst,    ae_int_t stride_dst, const ae_complex* vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n);
void ae_v_cmoveneg(ae_complex *vdst, ae_int_t stride_dst, const ae_complex* vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n);
void ae_v_cmoved(ae_complex *vdst,   ae_int_t stride_dst, const ae_complex* vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, double alpha);
void ae_v_cmovec(ae_complex *vdst,   ae_int_t stride_dst, const ae_complex* vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, ae_complex alpha);
void ae_v_cadd(ae_complex *vdst,     ae_int_t stride_dst, const ae_complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n);
void ae_v_caddd(ae_complex *vdst,    ae_int_t stride_dst, const ae_complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, double alpha);
void ae_v_caddc(ae_complex *vdst,    ae_int_t stride_dst, const ae_complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, ae_complex alpha);
void ae_v_csub(ae_complex *vdst,     ae_int_t stride_dst, const ae_complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n);
void ae_v_csubd(ae_complex *vdst, ae_int_t stride_dst, const ae_complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, double alpha);
void ae_v_csubc(ae_complex *vdst, ae_int_t stride_dst, const ae_complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, ae_complex alpha);
void ae_v_cmuld(ae_complex *vdst, ae_int_t stride_dst, ae_int_t n, double alpha);
void ae_v_cmulc(ae_complex *vdst, ae_int_t stride_dst, ae_int_t n, ae_complex alpha);

/************************************************************************
Real BLAS operations
************************************************************************/
double ae_v_dotproduct(const double *v0, ae_int_t stride0, const double *v1, ae_int_t stride1, ae_int_t n);
void ae_v_move(double *vdst,    ae_int_t stride_dst, const double* vsrc,  ae_int_t stride_src, ae_int_t n);
void ae_v_moveneg(double *vdst, ae_int_t stride_dst, const double* vsrc,  ae_int_t stride_src, ae_int_t n);
void ae_v_moved(double *vdst,   ae_int_t stride_dst, const double* vsrc,  ae_int_t stride_src, ae_int_t n, double alpha);
void ae_v_add(double *vdst,     ae_int_t stride_dst, const double *vsrc,  ae_int_t stride_src, ae_int_t n);
void ae_v_addd(double *vdst,    ae_int_t stride_dst, const double *vsrc,  ae_int_t stride_src, ae_int_t n, double alpha);
void ae_v_sub(double *vdst,     ae_int_t stride_dst, const double *vsrc,  ae_int_t stride_src, ae_int_t n);
void ae_v_subd(double *vdst,    ae_int_t stride_dst, const double *vsrc,  ae_int_t stride_src, ae_int_t n, double alpha);
void ae_v_muld(double *vdst,  ae_int_t stride_dst, ae_int_t n, double alpha);

/************************************************************************
Other functions
************************************************************************/
ae_int_t ae_v_len(ae_int_t a, ae_int_t b);

/*
extern const double ae_machineepsilon;
extern const double ae_maxrealnumber;
extern const double ae_minrealnumber;
extern const double ae_pi;
*/
#define ae_machineepsilon 5E-16
#define ae_maxrealnumber  1E300
#define ae_minrealnumber  1E-300
#define ae_pi 3.1415926535897932384626433832795


/************************************************************************
RComm functions
************************************************************************/
typedef struct rcommstate
{
    int stage;
    ae_vector ia;
    ae_vector ba;
    ae_vector ra;
    ae_vector ca;
} rcommstate;
void _rcommstate_init(rcommstate* p, ae_state *_state, ae_bool make_automatic);
void _rcommstate_init_copy(rcommstate* dst, const rcommstate* src, ae_state *_state, ae_bool make_automatic);
void _rcommstate_clear(rcommstate* p);
void _rcommstate_destroy(rcommstate* p);


/************************************************************************
Allocation counters, inactive by default.
Turned on when needed for debugging purposes.

_alloc_counter is incremented by 1 on malloc(), decremented on free().
_alloc_counter_total is only incremented by 1.
************************************************************************/
extern ae_int_t   _alloc_counter;
extern ae_int_t   _alloc_counter_total;
extern ae_bool    _use_alloc_counter;


/************************************************************************
Malloc debugging:

* _force_malloc_failure - set this flag to ae_true in  order  to  enforce
  failure of ALGLIB malloc(). Useful to debug handling of  errors  during
  memory allocation. As long as this flag is set, ALGLIB malloc will fail.
* _malloc_failure_after - set it to non-zero value in  order  to  enforce
  malloc failure as soon as _alloc_counter_total increases above value of
  this variable. This value has no effect if  _use_alloc_counter  is  not
  set.
************************************************************************/
extern ae_bool    _force_malloc_failure;
extern ae_int_t   _malloc_failure_after;


/************************************************************************
Trace file descriptor (to be used by ALGLIB code which sends messages  to
trace log)
************************************************************************/
extern FILE       *alglib_trace_file;


/************************************************************************
debug functions (must be turned on by preprocessor definitions):
* flushconsole(), fluches console
* ae_debugrng(), returns random number generated with high-quality random numbers generator
* ae_set_seed(), sets seed of the debug RNG (NON-THREAD-SAFE!!!)
* ae_get_seed(), returns two seed values of the debug RNG (NON-THREAD-SAFE!!!)
************************************************************************/
#ifdef AE_DEBUG4WINDOWS
#define flushconsole(s) fflush(stdout)
#endif
#ifdef AE_DEBUG4POSIX
#define flushconsole(s) fflush(stdout)
#endif

/************************************************************************
Internal macros, defined only when _ALGLIB_IMPL_DEFINES is defined before
inclusion of this header file
************************************************************************/
#if defined(_ALGLIB_IMPL_DEFINES)
    #define _ALGLIB_SIMD_ALIGNMENT_DOUBLES 8
    #define _ALGLIB_SIMD_ALIGNMENT_BYTES   (_ALGLIB_SIMD_ALIGNMENT_DOUBLES*8)
    /*
     * SIMD kernel dispatchers
     */
    #if defined(_ALGLIB_HAS_SSE2_INTRINSICS)
        #define _ALGLIB_KKK_VOID_SSE2(fname,params)   if( cached_cpuid&CPU_SSE2 ) { fname##_sse2 params; return; }
        #define _ALGLIB_KKK_RETURN_SSE2(fname,params) if( cached_cpuid&CPU_SSE2 ) { return fname##_sse2 params; }
    #else
        #define _ALGLIB_KKK_VOID_SSE2(fname,params)
        #define _ALGLIB_KKK_RETURN_SSE2(fname,params)
    #endif
    #if defined(_ALGLIB_HAS_AVX2_INTRINSICS)
        #define _ALGLIB_KKK_VOID_AVX2(fname,params)   if( cached_cpuid&CPU_AVX2 ) { fname##_avx2 params; return; }
        #define _ALGLIB_KKK_RETURN_AVX2(fname,params) if( cached_cpuid&CPU_AVX2 ) { return fname##_avx2 params; }
    #else
        #define _ALGLIB_KKK_VOID_AVX2(fname,params)
        #define _ALGLIB_KKK_RETURN_AVX2(fname,params)
    #endif
    #if defined(_ALGLIB_HAS_FMA_INTRINSICS)
        #define _ALGLIB_KKK_VOID_FMA(fname,params)    if( cached_cpuid&CPU_FMA )  { fname##_fma params; return; }
        #define _ALGLIB_KKK_RETURN_FMA(fname,params)  if( cached_cpuid&CPU_FMA )  { return fname##_fma params; }
    #else
        #define _ALGLIB_KKK_VOID_FMA(fname,params)
        #define _ALGLIB_KKK_RETURN_FMA(fname,params)
    #endif
    
    #if defined(_ALGLIB_HAS_SSE2_INTRINSICS) || defined(_ALGLIB_HAS_AVX2_INTRINSICS)
        #define _ALGLIB_KERNEL_VOID_SSE2_AVX2(fname,params) \
        {\
            ae_int_t cached_cpuid = ae_cpuid();\
            _ALGLIB_KKK_VOID_AVX2(fname,params)\
            _ALGLIB_KKK_VOID_SSE2(fname,params)\
        }
        #define _ALGLIB_KERNEL_RETURN_SSE2_AVX2(fname,params) \
        {\
            ae_int_t cached_cpuid = ae_cpuid();\
            _ALGLIB_KKK_RETURN_AVX2(fname,params)\
            _ALGLIB_KKK_RETURN_SSE2(fname,params)\
        }
    #else
        #define _ALGLIB_KERNEL_VOID_SSE2_AVX2(fname,params)   {}
        #define _ALGLIB_KERNEL_RETURN_SSE2_AVX2(fname,params) {}
    #endif
    
    #if defined(_ALGLIB_HAS_SSE2_INTRINSICS) || defined(_ALGLIB_HAS_AVX2_INTRINSICS) || defined(_ALGLIB_HAS_FMA_INTRINSICS)
        #define _ALGLIB_KERNEL_VOID_SSE2_AVX2_FMA(fname,params) \
        {\
            ae_int_t cached_cpuid = ae_cpuid();\
            _ALGLIB_KKK_VOID_FMA(fname,params)\
            _ALGLIB_KKK_VOID_AVX2(fname,params)\
            _ALGLIB_KKK_VOID_SSE2(fname,params)\
        }
        #define _ALGLIB_KERNEL_RETURN_SSE2_AVX2_FMA(fname,params) \
        {\
            ae_int_t cached_cpuid = ae_cpuid();\
            _ALGLIB_KKK_RETURN_FMA(fname,params)\
            _ALGLIB_KKK_RETURN_AVX2(fname,params)\
            _ALGLIB_KKK_RETURN_SSE2(fname,params)\
        }
    #else
        #define _ALGLIB_KERNEL_VOID_SSE2_AVX2_FMA(fname,params)   {}
        #define _ALGLIB_KERNEL_RETURN_SSE2_AVX2_FMA(fname,params) {}
    #endif
    
    #if defined(_ALGLIB_HAS_AVX2_INTRINSICS) || defined(_ALGLIB_HAS_FMA_INTRINSICS)
        #define _ALGLIB_KERNEL_VOID_AVX2_FMA(fname,params) \
        {\
            ae_int_t cached_cpuid = ae_cpuid();\
            _ALGLIB_KKK_VOID_FMA(fname,params)\
            _ALGLIB_KKK_VOID_AVX2(fname,params)\
        }
        #define _ALGLIB_KERNEL_RETURN_AVX2_FMA(fname,params) \
        {\
            ae_int_t cached_cpuid = ae_cpuid();\
            _ALGLIB_KKK_RETURN_FMA(fname,params)\
            _ALGLIB_KKK_RETURN_AVX2(fname,params)\
        }
    #else
        #define _ALGLIB_KERNEL_VOID_AVX2_FMA(fname,params) {}
        #define _ALGLIB_KERNEL_RETURN_AVX2_FMA(fname,params) {}
    #endif
    
    #if defined(_ALGLIB_HAS_AVX2_INTRINSICS)
        #define _ALGLIB_KERNEL_VOID_AVX2(fname,params) \
        {\
            ae_int_t cached_cpuid = ae_cpuid();\
            _ALGLIB_KKK_VOID_AVX2(fname,params)\
        }
        #define _ALGLIB_KERNEL_RETURN_AVX2(fname,params) \
        {\
            ae_int_t cached_cpuid = ae_cpuid();\
            _ALGLIB_KKK_RETURN_AVX2(fname,params)\
        }
    #else
        #define _ALGLIB_KERNEL_VOID_AVX2(fname,params) {}
        #define _ALGLIB_KERNEL_RETURN_AVX2(fname,params) {}
    #endif
    
    #if defined(_ALGLIB_HAS_FMA_INTRINSICS)
        #define _ALGLIB_KERNEL_VOID_FMA(fname,params) \
        {\
            ae_int_t cached_cpuid = ae_cpuid();\
            _ALGLIB_KKK_VOID_FMA(fname,params)\
        }
        #define _ALGLIB_KERNEL_RETURN_FMA(fname,params) \
        {\
            ae_int_t cached_cpuid = ae_cpuid();\
            _ALGLIB_KKK_RETURN_FMA(fname,params)\
        }
    #else
        #define _ALGLIB_KERNEL_VOID_FMA(fname,params) {}
        #define _ALGLIB_KERNEL_RETURN_FMA(fname,params) {}
    #endif
    
    #ifdef FP_FAST_FMA
        #define APPROX_FMA(x, y, z) fma((x), (y), (z))
    #else
        #define APPROX_FMA(x, y, z) ((x)*(y) + (z))
    #endif
    
#endif

/************************************************************************
APSERV overrides
************************************************************************/
#if !defined(ALGLIB_NO_FAST_KERNELS)
/*************************************************************************
Maximum concurrency on given system, with given compilation settings
*************************************************************************/
ae_int_t maxconcurrency(ae_state *_state);
#endif

/*$ End $*/
#endif
