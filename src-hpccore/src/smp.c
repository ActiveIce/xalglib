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
#include "stdafx.h"

/*
 * if AE_OS==AE_LINUX (will be redefined to AE_POSIX in aenv.h),
 * set _GNU_SOURCE flag BEFORE any #includes to get affinity
 * management functions
 */
#if (AE_OS==AE_LINUX) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

/* includes, with _GNU_SOURCE being set if necessary */
#include "smp.h"

/*
 * These extern declarations are not included into C++ source
 * because aenv.c and smp.c are merged together.
 */
extern ae_bool _use_vendor_kernels;
extern ae_bool    debug_workstealing;
extern ae_int_t   dbgws_pushroot_ok;
extern ae_int_t   dbgws_pushroot_failed;
extern unsigned char    _alglib_global_threading_flags;
extern ae_int_t         _alglib_cores_to_use;

/*$ Declarations $*/
#define AE_CRITICAL_ASSERT(x) if( !(x) ) abort()
    
#if AE_OS==AE_WINDOWS
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#include <windows.h>
#include <process.h>
#elif AE_OS==AE_POSIX
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#endif

#if !defined(_ALGLIB_HAS_WORKSTEALING)
/*************************************************************************
*                                                                        *
*                                                                        *
*    OS-agnostic stub for multithreading framework                       *
*                                                                        *
*                                                                        *
*************************************************************************/

#else
/*************************************************************************
*                                                                        *
*                                                                        *
*    OS-aware work-stealing multithreading framework                     *
*                                                                        *
*                                                                        *
*************************************************************************/
#define AE_QUEUE_SIZE 1024

#define AE_QUICK_SCANS_COUNT 1024
#define AE_SLEEP_ON_IDLE 1
#define AE_SLEEP_ON_FULL_QUEUE 1

#define AE_WRK_DISPOSE 1
#define AE_WRK_NEXT 2

#if AE_OS==AE_WINDOWS
void ae_worker_loop(void *T);
#elif AE_OS==AE_POSIX
void* ae_worker_loop(void *T);
#else
void ae_worker_loop(void *T);
#endif

void ae_init_event(ae_event *event, ae_bool manual_reset);
void ae_set_event(ae_event *event);
void ae_reset_event(ae_event *event);
void ae_free_event(ae_event *event);

/*************************************************************************
This structure provides OS-independent abstraction for thread handle:
* under Windows/Posix systems it uses system-provided thread
* under Boost it uses OS-independent thread provided by Boost package
* when no OS is defined, this structure has no fields
*************************************************************************/
typedef struct
{
#if AE_OS==AE_WINDOWS
    HANDLE thread;
#elif AE_OS==AE_POSIX
    pthread_t posix_thread;
#else
    ae_bool fake_field;
#endif
} ae_thread_handle;


/*************************************************************************
This is an internal structure which implements 'Event' functionality.
*************************************************************************/
typedef struct
{
#if AE_OS==AE_WINDOWS
    HANDLE event;
#elif AE_OS==AE_POSIX
    pthread_cond_t cond_var;
    pthread_mutex_t mutex;
    ae_bool is_signaling, manual_reset;
#else
    ae_bool is_signaling, manual_reset;
#endif
} _event;

/*************************************************************************
Task queue
*************************************************************************/
typedef struct ae_worker_queue
{
    /*
     * DESCRIPTION: queue status:
     *              * queue_lock    - lock which protects status fields
     *                                (implemented using interlocked
     *                                operations, 1 when acquired,
     *                                0 when released)
     *              * tasks         - circular buffer of tasks, unused
     *                                elements are equal to null.
     *                                tasks are pushed to top, popped
     *                                from top, stealed from bottom.
     *              * top           - index of top element. Tasks are
     *                                stored in tasks[top], tasks[top+1], ..
     *              * cnt           - number of tasks in a queue
     *              * queue_size    - size of the queue
     * PROTECTION:  by queue_lock.
     */
    ae_lock queue_lock;
    ae_task_info **tasks;
    ae_int_t top;
    ae_int_t cnt;
    ae_int_t queue_size;
} ae_worker_queue;


/*************************************************************************
Thread pool
*************************************************************************/
typedef struct ae_thread_pool
{
    /*
     * DESCRIPTION: queues, including primary queue and worker queues.
     *              can be NULL when queues_count==0.
     * PROTECTION:  not needed (initialized during creation, not changed since then)
     */
    ae_worker_queue *queues;
    
    /*
     * DESCRIPTION: total number of queues, including primary queue
     *              and worker queues, >=2. Equal to number of cores+1.
     * PROTECTION:  not needed (initialized during creation, not changed since then)
     */
    ae_int_t queues_count;
    
    /*
     * DESCRIPTION: this pair of objects is used to track status of the root tasks.
     *              Every time we push root task, root_cnt is increased. Every time
     *              we solve root task (one with no task group), root_cnt is decreased.
     *
     *              Every time root_cnt becomes nonzero, root_tasks_are_present
     *              event is set to signalling state. Every time root_cnt becomes
     *              zero, root_tasks_are_present event is set to non-signalling.
     *
     *              ae_push_root_task() is responsible for increase of root_cnt and
     *              setting event to signalling, ae_solve_task() is responsible for
     *              decrease of root_cnt and clearing event.
     *
     * PROTECTION:  both fields are protected by queues[0].queue_lock.  No protection
     *              when queues_count==0. Although events have their own protection,
     *              we MUST set/unset event only when lock is acquired.
     */
    volatile ae_int_t root_cnt;
	ae_event root_tasks_are_present;
    
    /*
     * DESCRIPTION: pool of disposed tasks
     * PROTECTION:  tasks_lock
     */
    ae_task_info *disposed_tasks;
    ae_lock tasks_lock;
    
    /*
     * DESCRIPTION: pool of disposed groups
     * PROTECTION:  groups_lock
     */
    ae_task_group *disposed_groups;
    ae_lock groups_lock;
    
    /*
     * DESCRIPTION: pool of disposed worker threads
     * PROTECTION:  threads_lock
     */
    ae_worker_thread *disposed_threads;
    ae_lock threads_lock;
    
    /*
     * DESCRIPTION: thread pool
     * PROTECTION:  not needed. Pool itself is not protected,
     *              individual threads have individual locks which protect then
     *
    ae_worker_thread *threads;
    
    *
     * DESCRIPTION: total number of worker threads in a pool, >0
     *
    ae_int_t threads_count;*/
} ae_thread_pool;

ae_thread_pool* ae_init_pool_internal();
void ae_free_pool(ae_thread_pool *smp_ptr);
ae_task_info* ae_pop_specific_task(ae_task_info *task, ae_int_t queue_idx);
void ae_terminate_group(ae_task_group *group, ae_state *_state);
void ae_terminate_child_groups(void*);

/*
 * Main thread pool.
 * This pool is initialized when we have AE_OS other than AE_UNKNOWN
 */
static ae_thread_pool *main_thread_pool = NULL;
static ae_bool finalization_request = ae_false;
#if AE_OS==AE_POSIX
static pthread_once_t once_init_pool = PTHREAD_ONCE_INIT;
#define AE_INIT_POOL_() pthread_once(&once_init_pool, ae_init_pool)
#else
#define AE_INIT_POOL_() ae_init_pool()
#endif

/* End of multithreading declarations */
#endif
/*$ Body $*/
#ifdef AE_SMP_DEBUGCOUNTERS
__declspec(align(AE_LOCK_ALIGNMENT)) volatile ae_int64_t _ae_dbg_threads_spawned = 0;
__declspec(align(AE_LOCK_ALIGNMENT)) volatile ae_int64_t _ae_dbg_tasks_created   = 0;
__declspec(align(AE_LOCK_ALIGNMENT)) volatile ae_int64_t _ae_dbg_tasks_stolen    = 0;
__declspec(align(AE_LOCK_ALIGNMENT)) volatile ae_int64_t _ae_dbg_groups_created  = 0;
__declspec(align(AE_LOCK_ALIGNMENT)) volatile ae_int64_t _ae_dbg_worker_yields   = 0;
__declspec(align(AE_LOCK_ALIGNMENT)) volatile ae_int64_t _ae_dbg_workers_tasks_cnt = 16;
__declspec(align(AE_LOCK_ALIGNMENT)) volatile ae_int64_t _ae_dbg_workers_tasks[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#endif

#if !defined(_ALGLIB_HAS_WORKSTEALING)
/*************************************************************************
*                                                                        *
*                                                                        *
*    OS-agnostic stub for multithreading framework                       *
*                                                                        *
*                                                                        *
*************************************************************************/
ae_int_t ae_cores_count()
{
    return 0;
}

void ae_set_cores_to_use(ae_int_t ncores)
{
}

ae_int_t ae_get_cores_to_use()
{
    return 1;
}

ae_int_t ae_get_cores_to_use_positive()
{
    return 1;
}

ae_bool ae_can_pexec(ae_state *_state)
{
    return ae_false;
}

ae_bool ae_can_parallelize_callbacks(ae_state *_state)
{
    return ae_false;
}

void ae_set_smp_support(
    ae_task_group **_child_tasks,
    ae_bool *_smp_enabled,
    ae_bool new_support_status,
    ae_state *_state)
{
    AE_CRITICAL_ASSERT((*_child_tasks)==NULL);
}

void ae_sync(
    ae_task_group *_child_tasks,
    ae_bool smp_enabled,
    ae_bool dispose_group,
    ae_state *_state)
{
    AE_CRITICAL_ASSERT(_child_tasks==NULL);
}

void ae_wait_for_group(
    ae_task_group *group,
    ae_bool dispose_on_success,
    ae_state *_state)
{
    AE_CRITICAL_ASSERT(group==NULL);
}

void ae_free_disposed_items()
{
}

void ae_complete_finalization_before_exit()
{
}

#else
/*************************************************************************
*                                                                        *
*                                                                        *
*    OS-aware work-stealing multithreading framework                     *
*                                                                        *
*                                                                        *
*************************************************************************/

/************************************************************************
This function returns number of CPU cores or 0 (when no information about
number of cores can be obtained). Zero result is not something  rare,  it
routinely returns 0 when compiled with no information about underlying OS.
************************************************************************/
ae_int_t ae_cores_count()
{
#if defined(AE_NWORKERS)
    return AE_NWORKERS;
#elif AE_OS==AE_WINDOWS
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwNumberOfProcessors;
#elif AE_OS==AE_POSIX
    long r = sysconf(_SC_NPROCESSORS_ONLN);
    return r<0 ? 0 : r;
#else
    return 0;
#endif
}


/************************************************************************
This function returns number of CPU cores which should be used by  worker
threads, converted to [1,CORES_COUNT] range.

This function requires initialized thread pool. It will  fail  if  called
without initialized thread pool.
************************************************************************/
ae_int_t ae_cores_to_use()
{
    AE_CRITICAL_ASSERT(main_thread_pool!=NULL);
    ae_int_t cu = ae_unsafe_read_aeint(&_alglib_cores_to_use);
    if( cu<=0 )
    {
        ae_int_t r = (main_thread_pool->queues_count-1)+cu;
        return r>=1 ? r : 1;
    }
    else
        return cu;
}


/************************************************************************
This function sets number of CPU cores which should  be  used  by  worker
threads. In case user specified non-positive number of cores to use, this
number will be converted according to following rules:
*  0 => ae_cores_count()
* -1 => max(ae_cores_count()-1,1)
* -2 => max(ae_cores_count()-2,1)
and so on.

In case user specified positive number of  cores,  greater  than 1,  then
ALGLIB will launch no more than ncores threads (or less, when  ncores  is
less than actual number of cores).
************************************************************************/
void ae_set_cores_to_use(ae_int_t ncores)
{
    ae_unsafe_write_aeint(&_alglib_cores_to_use, ncores);
}

/************************************************************************
This function returns number of CPU cores which should  be used by worker
threads, as specified by user. Negative values are returned "as is".
************************************************************************/
ae_int_t ae_get_cores_to_use()
{
    return ae_unsafe_read_aeint(&_alglib_cores_to_use);
}

/************************************************************************
This function returns number of CPU cores which should  be used by worker
threads, converted to [1,CORES_COUNT] range
************************************************************************/
ae_int_t ae_get_cores_to_use_positive()
{
    ae_int_t cc = ae_cores_count();
    ae_int_t cu = ae_unsafe_read_aeint(&_alglib_cores_to_use);
    if( cu<=0 )
    {
        ae_int_t r = cc+cu;
        return r>=1 ? r : 1;
    }
    else
        return cu<cc ? cu : cc;
}

/************************************************************************
This function tells us whether we can parallelize current task (insert it
into root task queue) or not; it returns net result of all multithreading
settings and current execution mode.

We can NOT parallelize current task if:
* we have only one core
* we have more than one core, but cores_to_use allows us to use only one
* multithreading is disabled at the call-local level
* at the call-local level we have default settings (use global settings),
  and global settings prohibit multithreading
* everything above is fine, but we are already in the multithreaded mode,
  i.e. in some worker thread. It means that we should execute current task
  in the context of the current worker thread.
************************************************************************/
ae_bool ae_can_pexec(ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_int_t _cu;
    _cu = ae_get_cores_to_use();
    if( _cu==1 || (_cu<=0 && ae_cores_count()+_cu<=1) || (_cu>1 && ae_cores_count()<=1) )
        return ae_false;
    if( (_state->flags&_ALGLIB_FLG_THREADING_MASK_WRK)==_ALGLIB_FLG_THREADING_SERIAL )
        return ae_false;
    if( (_state->flags&_ALGLIB_FLG_THREADING_MASK_WRK)==_ALGLIB_FLG_THREADING_USE_GLOBAL )
    {
        ae_uint64_t g64 = ((ae_uint64_t)_alglib_global_threading_flags)<<_ALGLIB_FLG_THREADING_SHIFT;
        if( (g64&_ALGLIB_FLG_THREADING_MASK_WRK)==_ALGLIB_FLG_THREADING_SERIAL ||
            (g64&_ALGLIB_FLG_THREADING_MASK_WRK)==_ALGLIB_FLG_THREADING_USE_GLOBAL )
            return ae_false;
    }
    if( _state->worker_thread!=NULL )
        return ae_false;
    return ae_true;
#else
    return ae_false;
#endif
}

/************************************************************************
This function tells us whether we can parallelize callbacks, i.e. perform
parallel numerical differentiation or parallel batch evaluation.

It returns the net result of  all  multithreading  settings  and  current
execution mode.

We can NOT parallelize callbacks if:
* we have only one core
* we have more than one core, but cores_to_use allows us to use only one
* callback parallelism is disabled at the call-local level
* at the call-local level we have default settings (use global settings),
  and global settings prohibit callback parallelism
************************************************************************/
ae_bool ae_can_parallelize_callbacks(ae_state *_state)
{
    ae_int_t _cu;
    _cu = ae_get_cores_to_use();
    if( _cu==1 || (_cu<=0 && ae_cores_count()+_cu<=1) || (_cu>1 && ae_cores_count()<=1) )
        return ae_false;
    if( (_state->flags&_ALGLIB_FLG_THREADING_MASK_CBK)==_ALGLIB_FLG_THREADING_SERIAL_CALLBACKS )
        return ae_false;
    if( (_state->flags&_ALGLIB_FLG_THREADING_MASK_CBK)==_ALGLIB_FLG_THREADING_PARALLEL_CALLBACKS )
        return ae_true;
    if( (_state->flags&_ALGLIB_FLG_THREADING_MASK_CBK)==_ALGLIB_FLG_THREADING_USE_GLOBAL )
    {
        ae_uint64_t g64 = ((ae_uint64_t)_alglib_global_threading_flags)<<_ALGLIB_FLG_THREADING_SHIFT;
        if( (g64&_ALGLIB_FLG_THREADING_MASK_CBK)==_ALGLIB_FLG_THREADING_SERIAL_CALLBACKS ||
            (g64&_ALGLIB_FLG_THREADING_MASK_CBK)==_ALGLIB_FLG_THREADING_USE_GLOBAL )
            return ae_false;
        return ae_true;
    }
    return ae_false;
}

/************************************************************************
This function initializes ae_event structure and sets to non-signalling
mode.
************************************************************************/
void ae_init_event(ae_event *event, ae_bool manual_reset)
{
    _event *e;
    
    event->ptr = malloc(sizeof(_event));
    AE_CRITICAL_ASSERT(event->ptr!=NULL);
    e = (_event*)event->ptr;
#if AE_OS==AE_WINDOWS
    e->event = CreateEvent(NULL, manual_reset ? TRUE : FALSE, FALSE, NULL);
#elif AE_OS==AE_POSIX
    pthread_mutex_init(&e->mutex, NULL);
    pthread_cond_init(&e->cond_var, NULL);
    e->is_signaling = ae_false;
    e->manual_reset = manual_reset;
#else
    e->is_signaling = ae_false;
    e->manual_reset = manual_reset;
#endif
}


/************************************************************************
This function waits for event
************************************************************************/
void ae_wait_for_event(ae_event *event)
{
    _event *e = (_event*)event->ptr;
#if AE_OS==AE_WINDOWS
    WaitForSingleObject(e->event, INFINITE);
#elif AE_OS==AE_POSIX
    pthread_mutex_lock(&e->mutex);
    for(;;)
    {
        if( e->is_signaling )
        {
            if( !e->manual_reset )
                e->is_signaling = ae_false;
            pthread_mutex_unlock(&e->mutex);
            return;
        }
        pthread_cond_wait(&e->cond_var, &e->mutex);
    }
#else
    AE_CRITICAL_ASSERT(e->is_signaling);
    if( !e->manual_reset )
        e->is_signaling = ae_false;
#endif
}


/************************************************************************
This function sets event to signalling state
************************************************************************/
void ae_set_event(ae_event *event)
{
    _event *e = (_event*)event->ptr;
#if AE_OS==AE_WINDOWS
    SetEvent(e->event);
#elif AE_OS==AE_POSIX
    pthread_mutex_lock(&e->mutex);
    e->is_signaling = ae_true;
    pthread_cond_broadcast(&e->cond_var);
    pthread_mutex_unlock(&e->mutex);
#else
    e->is_signaling = ae_true;
#endif
}


/************************************************************************
This function sets event to nonsignalling state
************************************************************************/
void ae_reset_event(ae_event *event)
{
    _event *e = (_event*)event->ptr;
#if AE_OS==AE_WINDOWS
    ResetEvent(e->event);
#elif AE_OS==AE_POSIX
    pthread_mutex_lock(&e->mutex);
    e->is_signaling = ae_false;
    pthread_mutex_unlock(&e->mutex);
#else
    e->is_signaling = ae_false;
#endif
}


/************************************************************************
This function frees ae_event structure.
************************************************************************/
void ae_free_event(ae_event *event)
{
    _event *e = (_event*)event->ptr;
#if AE_OS==AE_WINDOWS
     CloseHandle(e->event);
#elif AE_OS==AE_POSIX
    pthread_mutex_destroy(&e->mutex);
    pthread_cond_destroy(&e->cond_var);
#endif
    free(e);
}

/************************************************************************
This function starts the thread and stores its handle  into  thread_info.
It provides OS-independent abstraction layer for thread creation.

PARAMETERS:
    thread_function     -   main function
    instance            -   instance of ae_worker_thread, must be fully
                            inialized by the caller except for
                            instance->thread_handle parameter which  MUST
                            be NULL on entry, and  which  is  initialized
                            by this function.

NOTE: after  its  creation new thread MUST wait for instace->wakeup_event
      which will be set to signalling state after thread handle  will  be
      successfully stored in the thread instance structure.
      
NOTE: this function should NOT be called when AE_OS is AE_UNKNOWN  -  the
      whole program will be abnormally terminated.
************************************************************************/
#if AE_OS==AE_WINDOWS
void ae_start_thread(
    void (*thread_function)(void *T),
    ae_worker_thread *instance)
{
    /* create thread */
    AE_CRITICAL_ASSERT(instance->thread_handle==NULL);
    instance->thread_handle = malloc(sizeof(ae_thread_handle));
    AE_CRITICAL_ASSERT(instance->thread_handle!=NULL);
    ((ae_thread_handle*)(instance->thread_handle))->thread = (HANDLE)_beginthread(thread_function, 0, (void*)instance);
    AE_CRITICAL_ASSERT(((ae_thread_handle*)(instance->thread_handle))->thread!=NULL);
    SetThreadPriority(((ae_thread_handle*)(instance->thread_handle))->thread, THREAD_PRIORITY_HIGHEST);
    
    /* set event to signalling state */
    ae_set_event(&instance->wakeup_event);
}
#elif AE_OS==AE_POSIX
void ae_start_thread(
    void* (*thread_function)(void *T),
    ae_worker_thread *instance)
{
    /* create thread */
    AE_CRITICAL_ASSERT(instance->thread_handle==NULL);
    instance->thread_handle = malloc(sizeof(ae_thread_handle));
    AE_CRITICAL_ASSERT(instance->thread_handle!=NULL);
    if( pthread_create(&((ae_thread_handle*)(instance->thread_handle))->posix_thread, NULL, thread_function, instance)!=0 )
        abort();
    
    /* set event to signalling state */
    ae_set_event(&instance->wakeup_event);
}
#else
void ae_start_thread(
    void  (*thread_function)(void *T),
    ae_worker_thread *instance)
{
    abort();
}
#endif

/************************************************************************
This function exits from the current thread.
It provides OS-independent abstraction layer for thread finalization.
      
NOTE: this function should NOT be called when AE_OS is AE_UNKNOWN  -  the
      whole program will be abnormally terminated.
************************************************************************/
void ae_exit_thread()
{
#if AE_OS==AE_WINDOWS
    ExitThread(0);
#elif AE_OS==AE_POSIX
    pthread_exit(NULL);
#else
    abort();
#endif
}


/************************************************************************
This function pauses current thread for specified number of milliseconds.
It provides OS-independent abstraction layer for Sleep() call.

NOTE: function returns immediately for non-positive ms_to_sleep.  If  you
      want to perform YIELD operation (execute some other  thread  during
      current time slice), use ae_yield() function.
      
NOTE: this function should NOT be called when AE_OS is AE_UNKNOWN  -  the
      whole program will be abnormally terminated.
************************************************************************/
void ae_sleep_thread(ae_int_t ms_to_sleep)
{
    if( ms_to_sleep<=0 )
        return;
#if AE_OS==AE_WINDOWS
    Sleep((DWORD)ms_to_sleep);
#elif AE_OS==AE_POSIX
    struct timespec sleeptime;
    sleeptime.tv_sec = ms_to_sleep/1000;
    sleeptime.tv_nsec = (ms_to_sleep%1000)*1000000;
    if( nanosleep(&sleeptime,NULL)!=0 )
        abort();
#else
    abort();
#endif
}


/************************************************************************
This function sets worker_idx of the thread and (depending on  OS)  tries
to pin thread to its personal core.

NOTE: thread->worker_idx must be clear (negative) prior to  calling  this
      function.
      
NOTE: no synchronization is used during this call, it is your responsibility
      to ensure that access to worker_idx is synchronized.
************************************************************************/
void ae_pin_thread(ae_worker_thread *thread, ae_int_t worker_idx)
{
    AE_CRITICAL_ASSERT(main_thread_pool!=NULL);
    AE_CRITICAL_ASSERT(thread->worker_idx<0);
    AE_CRITICAL_ASSERT(worker_idx>0 && worker_idx<main_thread_pool->queues_count);
    thread->worker_idx = worker_idx;

#if 0    
/*--------------------------------*/
    /*
     * Affinity management: only for automatic detection of worker count,
     * and with some tricky details.
     */
#if !defined(AE_NWORKERS)
#if AE_OS==AE_WINDOWS
    /*
     * Windows-specific;
     * pinning is done only for workers with 1<=worker_idx<=64 (pre-Win7 limitations);
     * see similar code in ae_unpin_thread()
     */
    if( worker_idx<=64 )
        SetThreadAffinityMask(((ae_thread_handle*)thread->thread_handle)->thread, 1ULL<<(worker_idx-1));
#elif (AE_OS==AE_POSIX) && defined(_ALGLIB_USE_LINUX_EXTENSIONS)
    {
        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(worker_idx-1, &mask);
        pthread_setaffinity_np(((ae_thread_handle*)thread->thread_handle)->posix_thread, sizeof(mask), &mask);
    }
#else
#endif
#endif
/*--------------------------------*/
#endif
}


/************************************************************************
This function clears thread->worker_idx and clears thread  affinity  mask
of the worker.
************************************************************************/
void ae_unpin_thread(ae_worker_thread *thread)
{
    AE_CRITICAL_ASSERT(thread->worker_idx>0);
    AE_CRITICAL_ASSERT(main_thread_pool!=NULL);
    
    /*
     * Affinity management: only for automatic detection of worker count,
     * and with some tricky details.
     */
#if 0    
/*--------------------------------*/
#if !defined(AE_NWORKERS)
#if AE_OS==AE_WINDOWS
    /*
     * Windows-specific;
     * unpinning is done only for workers with 1<=worker_idx<=64 (pre-Win7 limitations);
     * see similar code in ae_pin_thread()
     */
    if( thread->worker_idx<=64 )
        SetThreadAffinityMask(((ae_thread_handle*)thread->thread_handle)->thread, ~0ULL);
#elif (AE_OS==AE_POSIX) && defined(_ALGLIB_USE_LINUX_EXTENSIONS)
    {
        cpu_set_t mask;
        int i;
        CPU_ZERO(&mask);
        for(i=0; i<CPU_COUNT(&mask); i++)
            CPU_SET(i, &mask);
        pthread_setaffinity_np(((ae_thread_handle*)thread->thread_handle)->posix_thread, sizeof(mask), &mask);
    }
#else
#endif
#endif
/*--------------------------------*/
#endif
    
    /*
     * Clear worker_idx
     */
    thread->worker_idx = -1;
}

/************************************************************************
This function  creates  worker  thread  and  assigns it to specific queue
given by worker_idx. New worker thread is active and running  immediately
after its creation.

It is assumed that this method is called  by  the  worker   thread  which
previously worked with that queue (owns the queue). After this  call, new
worker thread owns the queue, and previous worker has lost its ownership.

NOTE: worker_idx>=1
      
NOTE: this function should NOT be called when AE_OS is AE_UNKNOWN  -  the
      whole program will be abnormally terminated.
************************************************************************/
void ae_create_worker(ae_int_t worker_idx)
{
    ae_worker_thread *thread;
#ifdef AE_SMP_DEBUGCOUNTERS
    ae_bool created_new;
#endif
    AE_CRITICAL_ASSERT(main_thread_pool!=NULL);
    AE_CRITICAL_ASSERT(worker_idx>0 && worker_idx<main_thread_pool->queues_count);

    /* thread object: use list of disposed threads or start new thread */
    ae_acquire_lock(&main_thread_pool->threads_lock);
    if( main_thread_pool->disposed_threads==NULL )
    {
        /* release lock */
        ae_release_lock(&main_thread_pool->threads_lock);

        /* no disposed threads, create new one */
        thread = (ae_worker_thread*)malloc(sizeof(ae_worker_thread));
        AE_CRITICAL_ASSERT(thread!=NULL);
        ae_init_event(&thread->wakeup_event, ae_false);
        thread->worker_idx = worker_idx;
        thread->next_thread = NULL;
        thread->thread_handle = NULL;
    
        /* start thread; new thread is automatically pinned to appropriate core in the worker_loop function. */
        ae_start_thread(ae_worker_loop, thread);
        
#ifdef AE_SMP_DEBUGCOUNTERS
        /* new thread was created */
        created_new = ae_true;
#endif
    }
    else
    {
        /* we have thread in the main_thread_pool, use it */
        thread = (ae_worker_thread*)main_thread_pool->disposed_threads;
        main_thread_pool->disposed_threads = thread->next_thread;
        
        /* release lock */
        ae_release_lock(&main_thread_pool->threads_lock);
        
        /* initialize fields */
        AE_CRITICAL_ASSERT(thread->worker_idx<0);
        thread->next_thread = NULL;
        
        /* pin thread to appropriate core (it is VERY important to pin thread BEFORE waking it up) */
        ae_pin_thread(thread, worker_idx);
        
        /* wake up thread */
        ae_set_event(&thread->wakeup_event);
        
#ifdef AE_SMP_DEBUGCOUNTERS
        /* new thread was created */
        created_new = ae_false;
#endif
    }
    
    /* update debug counters */
#ifdef AE_SMP_DEBUGCOUNTERS
    if( created_new )
        InterlockedIncrement((LONG volatile *)&_ae_dbg_threads_spawned);
#endif
}


/************************************************************************
This function disposes worker thread -  worker thread is pauses and moved
to internal list of paused workers which can be reused later.

NOTE: it is expected that thread to be  disposed  has  no  child  groups.
      critical error is raised when list has unfinished childs.
      
NOTE: this function should NOT be called when AE_OS is AE_UNKNOWN  -  the
      whole program will be abnormally terminated.
************************************************************************/
void ae_dispose_worker(ae_worker_thread *thread)
{
    AE_CRITICAL_ASSERT(main_thread_pool!=NULL);
    AE_CRITICAL_ASSERT(thread->worker_idx<0);
    
    /* move thread to list of disposed threads, wait for wakeup */
    ae_acquire_lock(&main_thread_pool->threads_lock);
    thread->next_thread = (ae_worker_thread*)main_thread_pool->disposed_threads;
    main_thread_pool->disposed_threads = thread;
    ae_release_lock(&main_thread_pool->threads_lock);
    ae_wait_for_event(&thread->wakeup_event);
}


/************************************************************************
This function initializes ae_thread_pool structure:
* it allocates memory for internal data structures, creates queues
* it creates worker threads and points them to queues
* after initialization is completed, it replaces value of the smp_ptr
  parameter by pointer to the initialized structure.
* initialization is performed in thread-safe manner. Multiple threads can
  call this function in a concurrent manner, such calls will be correctly
  handled.
  
This function is automatically called when we create root task with
ae_create_task() function.

It has no effect when called with AE_OS==AE_UNKNOWN
************************************************************************/
void ae_init_pool()
{
	if( main_thread_pool == NULL )
	{
        ae_thread_pool *pool;
        ae_int_t i;

        pool = ae_init_pool_internal();
        if( pool==NULL )
            return;
        
        /*
         * Check whether we are first to initialize pool:
         * - under Windows interlocked operations are used
         * - under POSIX we rely on pthread_once() capabilities
         */
#if AE_OS==AE_WINDOWS
        if( InterlockedCompareExchangePointer((PVOID*)&main_thread_pool, (PVOID)pool, NULL)!=NULL )
        {
            ae_free_pool(pool);
            return;
        }
#else
        main_thread_pool = pool;
#endif
        
        /*
         * Create worker threads
         */
        for(i=1; i<pool->queues_count; i++)
            ae_create_worker(i);
    }
}


/************************************************************************
This is an internal initialization function for thread pool.

It returns thread pool (when we have more than one core) or NULL (when no
OS-specific support for threading is present or we have only 1 core).
************************************************************************/
ae_thread_pool* ae_init_pool_internal()
{
#if AE_OS!=AE_UNKNOWN
    ae_thread_pool *pool;
    ae_int_t i, j, cores_count;
    
    /*
     * Determine cores count
     */
    cores_count = ae_cores_count();
    if( cores_count<2 )
        return NULL;
    
    /*
     * Allocation
     */
    pool = (ae_thread_pool*)malloc(sizeof(ae_thread_pool));
    memset(pool, 0, sizeof(ae_thread_pool));
    AE_CRITICAL_ASSERT(pool!=NULL);
    
    /*
     * Initialize pool object as if we have no multicore support.
     */
    pool->queues_count = 0;
    pool->queues = NULL;
    pool->root_cnt = 0;
    pool->disposed_threads = NULL;
    pool->disposed_tasks = NULL;
    pool->disposed_groups = NULL;
    ae_init_event(&pool->root_tasks_are_present, ae_true);
    ae_init_lock_eternal(&pool->threads_lock);
    ae_init_lock_eternal(&pool->tasks_lock);
    ae_init_lock_eternal(&pool->groups_lock);
    
    /*
     * Initialize queues
     */
    pool->queues_count = cores_count + 1;
    pool->queues = (ae_worker_queue*)malloc(sizeof(ae_worker_queue)*(size_t)pool->queues_count);
    memset(pool->queues, 0, sizeof(ae_worker_queue)*(size_t)pool->queues_count);
    AE_CRITICAL_ASSERT(pool->queues!=NULL);
    for(i=0; i<pool->queues_count; i++)
    {
        ae_task_info **pp_task;
        
        pp_task = (ae_task_info**)malloc(sizeof(ae_task_info*)*(size_t)AE_QUEUE_SIZE);
        AE_CRITICAL_ASSERT(pp_task!=NULL);
        
        ae_init_lock_eternal(&pool->queues[i].queue_lock);
        pool->queues[i].top = 0;
        pool->queues[i].cnt = 0;
        pool->queues[i].queue_size = AE_QUEUE_SIZE;
        pool->queues[i].tasks = pp_task;
        for(j=0; j<AE_QUEUE_SIZE; j++)
            pool->queues[i].tasks[j] = NULL;
    }
    
    /* return */
    return pool;
#else
    /*
     * No SMP support by OS
     */
    return NULL;
#endif
}

/************************************************************************
This function frees ae_thread_pool structure and terminates all of its
worker threads.
************************************************************************/
void ae_free_pool(ae_thread_pool *smp_ptr)
{
    // TODO!!!!!!!!!!!!!!
    
    // worker threads may be fully initalized or only partially attached to the pool
    // (for example, it is possible to have zero active workers)
}


/*************************************************************************
This function solves task, handles errors.

For non-root tasks it reports results to task group, for root ones  -   it
decreases root_cnt field and sets event to signalling state.

Boolean parameter from_queue is true when task was extracted  from  queue,
false when task was executed without pushing it to queue.  The  difference
is that in the second case we do NOT decrement parent_group->waiting_count.

Return value:
* AE_WRK_NEXT in case task was successfully solved and we have  to  search
  for the next one.
* AE_WRK_DISPOSE in case task was successfully solved and waked  up  other
  worker thread which now owns our queue. Current worker  thread  have  to
  dispose itself.
  
The only situation when AE_WRK_DISPOSE can be returned is  when  there was
some worker thread (different from current worker) which  posted  task  to
the queue and called wait_for_group(). Thus, if:
* from_queue is false => AE_WRK_NEXT is returned
* task was posted by current worker => AE_WRK_NEXT is returned

NOTE 1: _state parameter stores information about threading state (current
        worker thread and current thread pool).
        
NOTE 2: It is expected that this function is called only from  the  worker
        thread. Any attempt to call it from the program main  thread  will
        lead to abnormal termination of the program.
        
NOTE 3: this function solves task and makes sure that no unprocessed child
        groups left. It terminates program in case  unprocessed  group  is
        detected.
        
NOTE:   on exception this function sets task->exception AND
        task->parent_group->exception to current instance of exception.
        Exception is NOT rethrown - only silently signalled.
*************************************************************************/
ae_int_t ae_solve_task(ae_task_info *task, ae_state *_state, ae_bool from_queue)
{
    ae_worker_thread *current_worker, *sleeping_worker;
    ae_state child_state;
    const char* exception;
    jmp_buf _break_jump;
    
    current_worker = (ae_worker_thread*)_state->worker_thread;
    AE_CRITICAL_ASSERT(task!=NULL);
    AE_CRITICAL_ASSERT(task->child_groups==NULL);
    
    /*
     * Create state structure, solve task
     */
    ae_state_init(&child_state);
    child_state.worker_thread = current_worker;
    child_state.parent_task = task;
    child_state.thread_exception_handler = ae_terminate_child_groups;
    exception = NULL;
    if( !setjmp(_break_jump) )
    {
        ae_state_set_break_jump(&child_state, &_break_jump);
        task->data.func(&task->data, &child_state);
    }
    else
        exception = child_state.error_msg;
    AE_CRITICAL_ASSERT(task->child_groups==NULL);
    ae_state_clear(&child_state);
    task->exception = exception;
#ifdef AE_SMP_DEBUGCOUNTERS
    if( current_worker->worker_idx<_ae_dbg_workers_tasks_cnt )
        InterlockedIncrement((LONG volatile *)(&_ae_dbg_workers_tasks[current_worker->worker_idx]));
#endif
    
    /*
     * Problem is solved, postprocessing
     */
    if( task->parent_group!=NULL )
    {
        /*
         * task is a part of some group:
         * 1. decrease waiting count for this group
         * 2. update group->exception
         * 2. in case some thread waits for completion, wake it up and give our queue to this thread
         */
        ae_task_group *parent_group = task->parent_group;
        if( from_queue )
        {
            ae_acquire_lock(&parent_group->group_lock);
            parent_group->waiting_count--;
            if( exception!=NULL )
                parent_group->exception = exception;
            if( parent_group->waiting_count==0 && parent_group->wake_up_worker_on_completion )
            {
                ae_int_t queue_idx;
                
                /*
                 * There is some worker thread which waits for completion of this group.
                 * We wake up this thread and give our queue to it.
                 *
                 * NOTE: this branch of code is NOT executed when we work without SMP support.
                 *       In this case tasks are executed immediately when they are pushed to
                 *       queue - BEFORE parent task calls wait_for_group() and sets 
                 *       wake_up_worker_on_completion field.
                 *       However, we perform several safety checks here.
                 */
                AE_CRITICAL_ASSERT(main_thread_pool!=NULL);
                AE_CRITICAL_ASSERT(main_thread_pool->queues_count>=2);
                AE_CRITICAL_ASSERT(current_worker!=NULL);
                AE_CRITICAL_ASSERT(parent_group->parent_worker!=NULL);
                AE_CRITICAL_ASSERT(parent_group->parent_worker!=current_worker);
                AE_CRITICAL_ASSERT(parent_group->parent_worker->worker_idx<0);
                
                /* cache variables */
                queue_idx = current_worker->worker_idx;
                sleeping_worker = parent_group->parent_worker;
                
                /* change group state, unpin one worker, pin another one */
                parent_group->wake_up_worker_on_completion = ae_false;
                ae_unpin_thread(current_worker);
                ae_pin_thread(sleeping_worker, queue_idx);
                
                /* release lock, notify sleeping worker that it is time to wake up */
                ae_release_lock(&parent_group->group_lock);
                ae_set_event(&sleeping_worker->wakeup_event);
                
                return AE_WRK_DISPOSE;
            }
            else
                ae_release_lock(&parent_group->group_lock);
        }
        else
        {
            if( exception!=NULL )
            {
                ae_acquire_lock(&parent_group->group_lock);
                parent_group->exception = exception;
                ae_release_lock(&parent_group->group_lock);
            }
        }
        return AE_WRK_NEXT;
    }
    else
    {
        /*
         * We've solved root task.
         *
         * NOTE: this branch of code is NOT executed when we work without SMP support.
         *       In this case root tasks are solved in push_root_task() without calling
         *       this function. However, we perform several safety checks here.
         */
        AE_CRITICAL_ASSERT(main_thread_pool!=NULL);
        ae_acquire_lock(&main_thread_pool->queues[0].queue_lock);
        main_thread_pool->root_cnt--;
        if( main_thread_pool->root_cnt==0 )
            ae_reset_event(&main_thread_pool->root_tasks_are_present);
        ae_release_lock(&main_thread_pool->queues[0].queue_lock);
        ae_set_event(&task->done_event);
        return AE_WRK_NEXT;
    }
}


/*************************************************************************
This  function  creates  new  instance of task_info structure, either root
task (in case no parent group is specified)  or  child  task  attached  to
group passed as argument.

Additional parameter _state must point to the current state of the  ALGLIB
environment. Among other information it stores current threading settings.
Checks listed in notes 2-3  are  performed  using  information  stored  in
_state.

NOTE 1: all fields of the task_info object have their default  values,  as
        specified in the task_info description.
        
NOTE 2: in case parent_group is NULL, we must call this function from  the
        external (non-worker) thread. Only non-worker  thread  can  create
        root task.
        
NOTE 3: in case parent_group is non-NULL, we must call this function  from
        the worker thread. External threads  can  not  create  groups  and
        attach tasks to them.
*************************************************************************/
ae_task_info* ae_create_task(ae_task_group *parent_group, ae_state *_state)
{
    ae_task_info *task;
#ifdef AE_SMP_DEBUGCOUNTERS
    ae_bool created_new;
#endif
    /*
     * When we create root task, make sure that thread pool is allocated.
     * For non-root tasks it is not necessary.
     */
    if( parent_group==NULL )
        AE_INIT_POOL_();
    
    /* quick exit for OS without SMP support */
    if( main_thread_pool==NULL )
    {
        AE_CRITICAL_ASSERT(_state!=NULL);
        task = (ae_task_info*)malloc(sizeof(ae_task_info));
        AE_CRITICAL_ASSERT(task!=NULL);
        ae_init_event(&task->done_event, ae_false);
        task->exception = NULL;
        task->parent_group = parent_group;
        task->child_groups = NULL;
        if( parent_group!=NULL )
        {
            task->next_task = parent_group->childs;
            parent_group->childs = task;
        }
        else
            task->next_task = NULL;
        return task;
    }
    
    /* check correctness of parameters */
    AE_CRITICAL_ASSERT(_state!=NULL);
    
    /* allocate memory, primary initialization */
    ae_acquire_lock(&main_thread_pool->tasks_lock);
    if( main_thread_pool->disposed_tasks==NULL )
    {
        /* release lock */
        ae_release_lock(&main_thread_pool->tasks_lock);

        /* no disposed tasks, create new one */
        task = (ae_task_info*)malloc(sizeof(ae_task_info));
        AE_CRITICAL_ASSERT(task!=NULL);
        ae_init_event(&task->done_event, ae_false);
        
#ifdef AE_SMP_DEBUGCOUNTERS
        /* new task was created */
        created_new = ae_true;
#endif
    }
    else
    {
        /* we have thread in the pool, use it */
        task = (ae_task_info*)main_thread_pool->disposed_tasks;
        main_thread_pool->disposed_tasks = task->next_task;
        
        /* release lock */
        ae_release_lock(&main_thread_pool->tasks_lock);
        
#ifdef AE_SMP_DEBUGCOUNTERS
        /* new task was created */
        created_new = ae_false;
#endif
    }
    
    /* initialization of other fields */
    task->exception = NULL;
    task->parent_group = parent_group;
    task->child_groups = NULL;
    if( parent_group!=NULL )
    {
        task->next_task = parent_group->childs;
        parent_group->childs = task;
    }
    else
        task->next_task = NULL;
    
    /* update debug counters */
#ifdef AE_SMP_DEBUGCOUNTERS
    if( created_new )
        InterlockedIncrement((LONG volatile *)&_ae_dbg_tasks_created);
#endif

    /* exit */
    return task;
}


/*************************************************************************
This function disposes instance of ae_task_info structure by  freeing  all
dynamically allocated structures. After call to this  function  pointer to
ae_task_info becomes invalid.

This function  may store structure in the internal list for reuse.
*************************************************************************/
void ae_dispose_task(ae_task_info *task)
{
    /* check correctness of parameters */
    AE_CRITICAL_ASSERT(task!=NULL);
    AE_CRITICAL_ASSERT(task->child_groups==NULL);
    
    /* dispose task depending on SMP support */
    if( main_thread_pool==NULL )
    {
        /*
         * quick exit for OS without SMP support
         */
        ae_destroy_and_free_task(task);
    }
    else
    {
        /*
         * OS support for SMP is detected.
         * Move task to list of disposed tasks
         */
        AE_CRITICAL_ASSERT(main_thread_pool!=NULL);
        ae_acquire_lock(&main_thread_pool->tasks_lock);
        task->next_task = (ae_task_info*)main_thread_pool->disposed_tasks;
        main_thread_pool->disposed_tasks = task;
        ae_release_lock(&main_thread_pool->tasks_lock);
    }
}


/*************************************************************************
This  function  performs  guaranteed  destruction of the task structure.
All members of the task  structure are destroyed, and memory allocated for
the task itself is also freed.
*************************************************************************/
void ae_destroy_and_free_task(ae_task_info *task)
{
    ae_free_event(&task->done_event);
    free(task);
}


/*************************************************************************
This function creates new  instance  of  task_group.  The  task  group  is
attached to the parent task (as specified by _state->parent_task).

Additional parameter _state must point to the current state of the  ALGLIB
environment. Among other information it stores current threading settings.

NOTE: this function may be called from non-worker thread, in this case  it
      returns NULL. It also returns NULL when OS provides no  support  for
      SMP (main_thread_pool==NULL).
*************************************************************************/
ae_task_group* ae_create_task_group(ae_state *_state)
{
    ae_task_group *group;
    ae_task_info *parent_task;
#ifdef AE_SMP_DEBUGCOUNTERS
    ae_bool created_new;
#endif
    
    /*
     * No SMP support is present
     */
    if( main_thread_pool==NULL )
    {
        group = (ae_task_group*)malloc(sizeof(ae_task_group));
        memset(group, 0, sizeof(ae_task_group));
        AE_CRITICAL_ASSERT(group!=NULL);
        ae_init_lock(&group->group_lock, NULL, ae_false);
        parent_task = (ae_task_info*)_state->parent_task;
        AE_CRITICAL_ASSERT(parent_task!=NULL);
        group->parent_worker = NULL;
        group->waiting_count = 0;
        group->wake_up_worker_on_completion = ae_false;
        group->childs = NULL;
        group->exception = NULL;
        group->next_group = parent_task->child_groups;
        parent_task->child_groups = group;
        return group;
    }
    
    /* allocate memory, primary initialization */
    ae_acquire_lock(&main_thread_pool->groups_lock);
    if( main_thread_pool->disposed_groups==NULL )
    {
        /* release lock */
        ae_release_lock(&main_thread_pool->groups_lock);

        /* no disposed groups, create new one */
        group = (ae_task_group*)malloc(sizeof(ae_task_group));
        memset(group, 0, sizeof(ae_task_group));
        AE_CRITICAL_ASSERT(group!=NULL);
        ae_init_lock(&group->group_lock, NULL, ae_false);
        
#ifdef AE_SMP_DEBUGCOUNTERS
        /* new task was created */
        created_new = ae_true;
#endif
    }
    else
    {
        /* we have thread in the pool, use it */
        group = (ae_task_group*)main_thread_pool->disposed_groups;
        main_thread_pool->disposed_groups = group->next_group;
        
        /* release lock */
        ae_release_lock(&main_thread_pool->groups_lock);
        
#ifdef AE_SMP_DEBUGCOUNTERS
        /* new task was created */
        created_new = ae_false;
#endif
    }
    
    /* initialize other fields */
    parent_task = (ae_task_info*)_state->parent_task;
    AE_CRITICAL_ASSERT(parent_task!=NULL);
    group->parent_worker = (ae_worker_thread*)_state->worker_thread;
    group->waiting_count = 0;
    group->wake_up_worker_on_completion = ae_false;
    group->childs = NULL;
    group->exception = NULL;
    group->next_group = parent_task->child_groups;
    parent_task->child_groups = group;
    
    /* update debug counters */
#ifdef AE_SMP_DEBUGCOUNTERS
    if( created_new )
        InterlockedIncrement((LONG volatile *)&_ae_dbg_groups_created);
#endif

    /* exit */
    return group;
}


/*************************************************************************
This function disposes instance of ae_task_group structure by  freeing all
dynamically allocated structures. After call to this  function  pointer to
ae_task_group becomes invalid.

This function may store structure in the internal list for reuse.

NOTE: all  tasks  must  be  disposed  prior  to  calling this function. It
      simply ignores presence of child tasks.
      
NOTE: this function can be used with NULL group.
*************************************************************************/
void ae_dispose_group(ae_task_group *group)
{
    /* dispose group depending on SMP support */
    if( main_thread_pool==NULL )
    {
        /*
         * free memory - for OS without SMP support
         */
        ae_destroy_and_free_group(group);
    }
    else
    {
        /*
         * Move group to list of disposed groups
         */
        if( group==NULL )
            return;
        ae_acquire_lock(&main_thread_pool->groups_lock);
        group->next_group = (ae_task_group*)main_thread_pool->disposed_groups;
        main_thread_pool->disposed_groups = group;
        ae_release_lock(&main_thread_pool->groups_lock);
    }
}


/*************************************************************************
This function performs guaranteed destruction of ae_task_group structure.
All members of the group structure are destroyed, and memory allocated for
the group itself is also freed.

No memory is retained after call to this function.
*************************************************************************/
void ae_destroy_and_free_group(ae_task_group *group)
{
    ae_free_lock(&group->group_lock);
    free(group);
}


/*************************************************************************
This  function  waits for the completion of the task group  owned  by  the
current task. Only task which owns the group can call this method.

This method:
* tries to execute child tasks in the context of  the  current  worker  by
  traversing list of the child tasks,  calling ae_pop_specific_task()  and
  executing them. It is important to traverse list of the  child  problems
  in the correct direction - starting from the most recently added task.
* if all child tasks were executed in the context of  the current  worker,
  we return.
* if one of the child tasks was not found on top of the  stack,  it  means
  that it was stolen. In this case we wait for the group to  complete  and
  give our queue to other worker thread.  After  waking  up  (possibly  in
  another queue) we return.
  
NOTE 1: this  function  can  be  called  from  non-worker thread with NULL
        group.

NOTE 2: when called with group=null, this function silently  returns  back
        to  caller.  Group  can  be  NULL when called from any kind of the
        thread.

NOTE 3: depending on dispose_on_success parameter function may either:
        * dispose all child tasks AND group itself (group is removed  from
          childs of the current task)
        * dispose all child tasks, but leave group in the default  (empty)
          state. Group is not removed from the childs of the task.
        
NOTE 4: this function rethrows exceptions raised in child tasks. In case of
        multiple exceptions only one is rethrown.

NOTE 5: on exception this function disposes only group and its child tasks,
        but leaves other child groups of the current task unchanged. It is
        responsibility of ae_solve_task to dispose other groups.

NOTE:   this method may reassign current worker object to another queue
        in case we had to actually wait for the childs to complete.
*************************************************************************/
void ae_wait_for_group(
    ae_task_group *group,
    ae_bool dispose_on_success,
    ae_state *_state)
{
    ae_worker_thread *worker_thread;
    ae_task_info *task;
    ae_task_info *parent_task;
    const char* exception;
    ae_int_t queue_idx;
    
    /* check consistency of the threading information */
    AE_CRITICAL_ASSERT( _state!=NULL );
    AE_CRITICAL_ASSERT( group==NULL || (void*)group->parent_worker==_state->worker_thread );
    
    /* quick exit */
    if( group==NULL )
        return;
    
    /* start waiting for childs */
    worker_thread = (ae_worker_thread*)_state->worker_thread;
    parent_task = (ae_task_info*)_state->parent_task;
    ae_acquire_lock(&group->group_lock);
    if( group->waiting_count>0 )
    {
        /* 
         * There are childs waiting for processing.
         * Try to process them within context of the current thread.
         *
         * NOTE: this branch of code is executed only when we have SMP support because
         *       in other cases tasks are executed immediately when they are push'ed.
         *       However, we perform several safety checks here.
         *
         * NOTE: we do not protect access to group->childs because only owner of the
         *       group may modify its childs - and we are the owner
         *
         */
        AE_CRITICAL_ASSERT( main_thread_pool!=NULL );
        AE_CRITICAL_ASSERT( worker_thread!=NULL );
        AE_CRITICAL_ASSERT( group!=NULL );
        AE_CRITICAL_ASSERT( group->childs!=NULL );
        ae_release_lock(&group->group_lock);
        for(task = group->childs;
            (task!=NULL) && (ae_pop_specific_task(task,worker_thread->worker_idx)!=NULL);
            task = task->next_task)
        {
            ae_int_t tmp;
            
            tmp = ae_solve_task(task, _state, ae_false);
            AE_CRITICAL_ASSERT(tmp==AE_WRK_NEXT);
            ae_acquire_lock(&group->group_lock);
            AE_CRITICAL_ASSERT(group->waiting_count>0);
            group->waiting_count--;
            ae_release_lock(&group->group_lock);
        }
        
        /* in case there are still exist unprocessed childs,
           wait for them to be completed by other threads */
        ae_acquire_lock(&group->group_lock);
        if( group->waiting_count>0 )
        {
            group->wake_up_worker_on_completion = ae_true;
            queue_idx = worker_thread->worker_idx;
            ae_unpin_thread(worker_thread);
            ae_release_lock(&group->group_lock);
            ae_create_worker(queue_idx);
            ae_wait_for_event(&worker_thread->wakeup_event);
            AE_CRITICAL_ASSERT(worker_thread->worker_idx>0);
            ae_acquire_lock(&group->group_lock);
            exception = (const char*)group->exception;
            ae_release_lock(&group->group_lock);
        }
        else
        {
            exception = (const char*)group->exception;
            ae_release_lock(&group->group_lock);
        }
    }
    else
    {
        exception = (const char*)group->exception;
        ae_release_lock(&group->group_lock);
    }
        
    /* childs are done, dispose child tasks */
    while( group->childs!=NULL )
    {
        task = group->childs;
        group->childs = task->next_task;
        ae_dispose_task(task);
    }
    
    /* dispose group itself (if needed) */
    if( exception!=NULL || dispose_on_success )
    {
        /* remove group from list of child groups */
        ae_task_group * volatile * volatile cg;
        for(cg = &parent_task->child_groups; ; cg = &((*cg)->next_group) )
        {
            AE_CRITICAL_ASSERT((*cg)!=NULL);
            if( *cg==group )
            {
                *cg = group->next_group;
                break;
            }
        }
        
        /* group is removed from list, we can dispose it */
        ae_dispose_group(group);
    }
    
    /* rethrow exception if needed */
    ae_assert(exception==NULL, exception, _state);
}

/*************************************************************************
This  function  terminates child groups of parent task  (as  specified  by
the state structure).

This function must be called prior to raising exception in order  to  make
sure  that  all  child  tasks  are  done  before  we  start  to deallocate
dynamically allocated data structures.

NOTE: this function does not terminate parent task itself.

NOTE: _state parameter must be pointer to ae_state structure
*************************************************************************/
void ae_terminate_child_groups(void *state)
{
    ae_task_info *parent_task;
    ae_state *_state;
    
    _state = (ae_state*)state;
    AE_CRITICAL_ASSERT( _state!=NULL );
    AE_CRITICAL_ASSERT( _state->parent_task!=NULL );
    
    parent_task = (ae_task_info*)_state->parent_task;
    while( parent_task->child_groups!=NULL )
        ae_terminate_group(parent_task->child_groups, _state);
}

/*************************************************************************
This  function  terminates  task  group  by  removing its tasks from queue
(tasks are not solved - just removed) and waiting for completion of  tasks
which were stolen by other workers. Only task which  owns  the  group  can
call this method. Finally, we dispose group.

NOTE:   this function does not rethrow exceptions in child tasks.
*************************************************************************/
void ae_terminate_group(
    ae_task_group *group,
    ae_state *_state)
{
    ae_worker_thread *worker_thread;
    ae_task_info *task;
    ae_task_info *parent_task;
    ae_int_t queue_idx;
    ae_task_group * volatile * volatile cg;
    
    /* check consistency of the threading information */
    AE_CRITICAL_ASSERT( _state!=NULL );
    AE_CRITICAL_ASSERT( _state->parent_task!=NULL );
    AE_CRITICAL_ASSERT( group==NULL || (void*)group->parent_worker==_state->worker_thread );
    
    /* quick exit */
    if( group==NULL )
        return;
    
    /* start waiting for childs */
    worker_thread = (ae_worker_thread*)_state->worker_thread;
    ae_acquire_lock(&group->group_lock);
    if( group->waiting_count>0 )
    {
        /* 
         * There are childs waiting for processing.
         * Try to remove them without solving.
         *
         * NOTE: this branch of code is executed only when we have SMP support because
         *       in other cases tasks are executed immediately when they are push'ed.
         *       However, we perform several safety checks here.
         *
         * NOTE: we do not protect access to group->childs because only owner of the
         *       group may modify its childs - and we are the owner
         */
        AE_CRITICAL_ASSERT( main_thread_pool!=NULL );
        AE_CRITICAL_ASSERT( worker_thread!=NULL );
        AE_CRITICAL_ASSERT( group->childs!=NULL );
        ae_release_lock(&group->group_lock);
        for(task = group->childs;
            (task!=NULL) && (ae_pop_specific_task(task,worker_thread->worker_idx)!=NULL);
            task = task->next_task)
        {
            ae_acquire_lock(&group->group_lock);
            AE_CRITICAL_ASSERT(group->waiting_count>0);
            group->waiting_count--;
            ae_release_lock(&group->group_lock);
        }
        
        /* in case there are still exist unprocessed childs,
           wait for them to be completed by other threads */
        ae_acquire_lock(&group->group_lock);
        if( group->waiting_count>0 )
        {
            group->wake_up_worker_on_completion = ae_true;
            queue_idx = worker_thread->worker_idx;
            ae_unpin_thread(worker_thread);
            ae_release_lock(&group->group_lock);
            ae_create_worker(queue_idx);
            ae_wait_for_event(&worker_thread->wakeup_event);
            AE_CRITICAL_ASSERT(worker_thread->worker_idx>0);
        }
        else
            ae_release_lock(&group->group_lock);
    }
    else
        ae_release_lock(&group->group_lock);
        
    /* childs are done, dispose child tasks */
    while( group->childs!=NULL )
    {
        task = group->childs;
        group->childs = task->next_task;
        ae_dispose_task(task);
    }
    
    /* dispose group itself */
    parent_task = (ae_task_info*)_state->parent_task;
    for(cg = &parent_task->child_groups; ; cg = &((*cg)->next_group) )
    {
        AE_CRITICAL_ASSERT((*cg)!=NULL);
        if( *cg==group )
        {
            *cg = group->next_group;
            break;
        }
    }
    ae_dispose_group(group);
}


/************************************************************************
This  function  pushes  non-root  task  object  to the top of the current
worker queue.

NOTE 1: this method must act as full barrier, i.e. all pending
        modifications to task_info instance will be committed to the
        memory before return from this method.

NOTE 2: in case queue has free space, task is added to the queue. In case
        queue is full, this task is solved immediately without modyfing queue.

NOTE 3: _state parameter stores information about threading state (current
        worker thread and current thread pool).
        
NOTE 4: It is expected that this function is called only from the child
        task. Any attempt to call it from the program main thread will
        lead to abnormal termination of the program.

NOTE 5: task MUST be part of some task group. Any attempt to use this
        function on a task which is not part of the group will terminate
        the program.
        
NOTE 6: parent group MUST be owned by the same worker as one which calls
        push_task(). Attempts to push tasks tied to groups owned by other
        workers will crash program.
************************************************************************/
void ae_push_task(ae_task_info *task, ae_state *_state, ae_bool execute_immediately)
{
    ae_worker_queue *queue;
    
        
    /*
     * Handle situation when no threading support is present or
     * task must be executed immediately
     */
    if( main_thread_pool==NULL  || execute_immediately )
    {
        ae_int_t tmp;
        AE_CRITICAL_ASSERT(_state->parent_task!=NULL);
        tmp = ae_solve_task(task, _state, ae_false);
        AE_CRITICAL_ASSERT(tmp==AE_WRK_NEXT);
        return;
    }
    
    /*
     * Threading support is present. Check parameters.
     */
    AE_CRITICAL_ASSERT(_state->parent_task!=NULL);
    AE_CRITICAL_ASSERT(_state->worker_thread!=NULL);
    AE_CRITICAL_ASSERT(task!=NULL);
    AE_CRITICAL_ASSERT(task->parent_group!=NULL);
    AE_CRITICAL_ASSERT((void*)task->parent_group->parent_worker==_state->worker_thread);
    
    /*
     * Try to push task to queue
     */
    queue = &main_thread_pool->queues[((ae_worker_thread*)(_state->worker_thread))->worker_idx];
    ae_acquire_lock(&queue->queue_lock);
    if( queue->cnt>=queue->queue_size )
    {
        ae_int_t tmp;
        ae_release_lock(&queue->queue_lock);
        tmp = ae_solve_task(task, _state, ae_false);
        AE_CRITICAL_ASSERT(tmp==AE_WRK_NEXT);
    }
    else
    {
        queue->top = queue->top==0 ? queue->queue_size-1 : queue->top-1;
        queue->cnt++;
        queue->tasks[queue->top] = task;
        ae_release_lock(&queue->queue_lock);
        ae_acquire_lock(&task->parent_group->group_lock);
        task->parent_group->waiting_count++;
        ae_release_lock(&task->parent_group->group_lock);
    }
}


/*************************************************************************
This function pushes root task object to  the  top   of   the  main  queue
(one with index 0). After task added to queue this  function  returns.  If
you  want  to  wait  for  the  task  completion,  you  have  to  wait  for
task->done_event.

NOTE 1: this method must act as full barrier, i.e. all pending
        modifications to task_info instance will be committed to the
        memory before return from this method.

NOTE 2: in case queue has free space, task is added to the queue. In  case
        queue is full, we wait for some time before trying add something.
        
NOTE 3: It is expected that this function is called  only  from  the  main
        thread. Any attempt to call it from the worker  thread  will  lead
        to abnormal termination of the program.

NOTE 4: task MUST must NOT be part of some task group. Any attempt to  use
        this function on a task which is part of the group will  terminate
        the program.
        
NOTE 5: this method correctly handles situation when  we  have  NO  worker
        threads by solving task in the context of the calling thread. Such
        situation is possible, for example, when AE_OS==AE_UNKNOWN
*************************************************************************/
void ae_push_root_task(ae_task_info *task)
{
    ae_worker_queue *queue;
    AE_CRITICAL_ASSERT(task!=NULL);
    AE_CRITICAL_ASSERT(task->parent_group==NULL);
    
    /*
     * Handle situation when no SMP support is detected
     */
    if( main_thread_pool==NULL )
    {
        ae_state _state;
        const char *exception;
        jmp_buf _break_jump;
        if( debug_workstealing )
            ae_optional_atomic_add_i(&dbgws_pushroot_failed, 1);
        ae_state_init(&_state);
        exception = NULL;
        _state.parent_task = task;
        _state.thread_exception_handler = ae_terminate_child_groups;
        if( !setjmp(_break_jump) )
        {
            ae_state_set_break_jump(&_state, &_break_jump);
            task->data.func(&task->data, &_state);
        }
        else
            exception = _state.error_msg;
        ae_state_clear(&_state);
        task->exception = exception;
        ae_set_event(&task->done_event);
        return;
    }
    
    /*
     * SMP support is present, post task to queue
     */
    AE_CRITICAL_ASSERT(main_thread_pool->queues_count>=2);
    queue = &main_thread_pool->queues[0];
    ae_acquire_lock(&queue->queue_lock);
    for(;;)
    {
        /*
         * pause execution in case queue is full
         */
        if( queue->cnt==queue->queue_size)
        {
            ae_release_lock(&queue->queue_lock);
            ae_sleep_thread(AE_SLEEP_ON_FULL_QUEUE); /* TODO: better way of pausing execution */
            ae_acquire_lock(&queue->queue_lock);
            continue;
        }
        
        /*
         * we have free space, insert task and return
         */
        queue->tasks[(queue->top+queue->cnt)%queue->queue_size] = task;
        queue->cnt++;
        if( main_thread_pool->root_cnt==0 )
            ae_set_event(&main_thread_pool->root_tasks_are_present);
        main_thread_pool->root_cnt++;
        ae_release_lock(&queue->queue_lock);
        if( debug_workstealing )
            ae_optional_atomic_add_i(&dbgws_pushroot_ok, 1);
        return;
    }        
}


/*************************************************************************
This method pops task from the top of the  corresponding  queue.  In  case
queue has items, this method will return non-NULL pointer. In  case  queue
is empty, it will return NULL.

NOTE 1: this method can pop elements from any queue, independently of  the
        queue owner - oven from queues which belong to  other  threads  or
        from the root queue.
*************************************************************************/
ae_task_info* ae_pop_task(ae_int_t queue_idx)
{
    ae_worker_queue *queue;
    
    AE_CRITICAL_ASSERT(main_thread_pool!=NULL);
    AE_CRITICAL_ASSERT(queue_idx>=0 && queue_idx<main_thread_pool->queues_count);
    
    queue = &main_thread_pool->queues[queue_idx];
    ae_acquire_lock(&queue->queue_lock);
    if( queue->cnt==0 )
    {
        ae_release_lock(&queue->queue_lock);
        return NULL;
    }
    else
    {
        ae_task_info *task = queue->tasks[queue->top];
        queue->tasks[queue->top] = NULL;
        queue->cnt--;
        queue->top = (queue->top+1)%queue->queue_size;
        ae_release_lock(&queue->queue_lock);
        return task;
    }
}


/*************************************************************************
This method tries to pop specific task from the top of the   corresponding
queue. In case specific task is not found in the queue, it returns NULL.

NOTE 1: this method can pop elements from any queue, independently of  the
        queue owner - oven from queues which belong to  other  threads  or
        from the root queue.
*************************************************************************/
ae_task_info* ae_pop_specific_task(ae_task_info *task, ae_int_t queue_idx)
{
    ae_worker_queue *queue;
    
    AE_CRITICAL_ASSERT(main_thread_pool!=NULL);
    AE_CRITICAL_ASSERT(queue_idx>=0 && queue_idx<main_thread_pool->queues_count);
    
    queue = &main_thread_pool->queues[queue_idx];
    ae_acquire_lock(&queue->queue_lock);
    if( queue->cnt==0 || queue->tasks[queue->top]!=task )
    {
        ae_release_lock(&queue->queue_lock);
        return NULL;
    }
    else
    {
        queue->tasks[queue->top] = NULL;
        queue->cnt--;
        queue->top = (queue->top+1)%queue->queue_size;
        ae_release_lock(&queue->queue_lock);
        return task;
    }
}

/*************************************************************************
This method steals task from the BOTTOM of the  corresponding  queue.   In
case queue has items, this method will return non-NULL pointer.  In   case
queue is empty, it will return NULL.

NOTE 1: this method can steal elements from any  queue,  independently  of
        the queue owner - oven from queues which belong to  other  threads
        or from the root queue.
*************************************************************************/
ae_task_info* ae_steal_task(ae_int_t queue_idx)
{
    ae_worker_queue *queue;
    
    AE_CRITICAL_ASSERT(main_thread_pool!=NULL);
    AE_CRITICAL_ASSERT(queue_idx>=0 && queue_idx<main_thread_pool->queues_count);
    
    queue = &main_thread_pool->queues[queue_idx];
    ae_acquire_lock(&queue->queue_lock);
    if( queue->cnt==0 )
    {
        ae_release_lock(&queue->queue_lock);
        return NULL;
    }
    else
    {
        ae_int_t idx;
        ae_task_info *task;
        idx = (queue->top+queue->cnt-1)%queue->queue_size;
        task = queue->tasks[idx];
        queue->tasks[idx] = NULL;
        queue->cnt--;
        ae_release_lock(&queue->queue_lock);
    
        /* update debug counters */
#ifdef AE_SMP_DEBUGCOUNTERS
        InterlockedIncrement((LONG volatile *)&_ae_dbg_tasks_stolen);
#endif

        /* exit */
        return task;
    }
}

/*************************************************************************
This method is used to update SMP status of ALGLIB function.

Every SMP-capable ALGLIB function can work in multi-threaded  and  single-
threaded  modes.  When  working  in  single-threaded  mode, SPAWN operator
executes  task  immediately  in  the context of the current worker thread.
When  working  in  multithreaded  mode,  SPAWN operator creates child task
which is attached to automatically created group and pushed to queue.

ALGLIB  functions  can switch between two modes during their execution. In
order  to  store  information  about  current  mode, each function has two
automatically created variables:
* _child_tasks  -   group of child tasks, which is NULL  by  default,  but
                    automatically created when SMP support is turned on.
* _smp_enabled  -   current SMP status, false by default.

When SMP is enabled, task_group object is automatically created.  It  will
never  be  freed  until  the  end  of  the  current  function. When SMP is
disabled,  child  group  is  left non-NULL. Thus,  three  combinations  of
variable values are possible:
*  _smp_enabled && _child_tasks!=NULL       SMP is active, group created
* !_smp_enabled && _child_tasks!=NULL       SMP is inactive, but was previously
                                            active; we can have child tasks
                                            in the queue.
* !_smp_enabled && _child_tasks==NULL       SMP was inactive since the beginning
                                            of the current function.

This method  updates  SMP  status  and  changes  state  of  the  variables
according to new state and their  previous  values.  It  is  important  to
understand, that this method changes only values of  variables  passed  by
reference.  It  does  NOT  changes  some  global  settings, like number of
working threads, threads status and so on.

NOTE: this function does not change anything  when  called from non-worker
      thread. SMP support can be activated only  from  the  worker  thread
      executed as part of the ALGLIB thread pool.
      When it is called from external, non-worker thread, it just silently
      returns without changing SMP status.
*************************************************************************/
void ae_set_smp_support(
    ae_task_group **_child_tasks,
    ae_bool *_smp_enabled,
    ae_bool new_support_status,
    ae_state *_state)
{
    if( main_thread_pool==NULL || _state->worker_thread==NULL )
    {
        AE_CRITICAL_ASSERT((*_child_tasks)==NULL && !*_smp_enabled);
        return;
    }
    if( new_support_status )
    {
        if( !*_smp_enabled )
        {
            if( *_child_tasks==NULL )
                *_child_tasks = ae_create_task_group(_state);
            *_smp_enabled = ae_true;
        }
        AE_CRITICAL_ASSERT((*_child_tasks)!=NULL);
    }
    else
        *_smp_enabled = ae_false;
}

/*************************************************************************
This method is used  to  synchronize  with  child  problems  according  to
presence of child tasks and current status of SMP support.

Every SMP-capable ALGLIB function can work in multi-threaded  and  single-
threaded modes. It is possible to switch between two modes and to be in  a
single-threaded  mode,  but  to have child tasks previously spawned in the
multi-threaded mode.

This function performs synchronization with childs:
* in case code is executed in context of the  non-worker  thread,  it just
  silently returns. We check that no task group was created and SMP is off
  and abort program in case these conditions  are  not  satisfied  (it  is
  considered to be critical error in the program logic).
* in case we have NULL  task  group,  we  silently  return.  Similarly  to
  previous case we check that SMP support is turned off.
  
*************************************************************************/
void ae_sync(
    ae_task_group *_child_tasks,
    ae_bool smp_enabled,
    ae_bool dispose_group,
    ae_state *_state)
{
    if( main_thread_pool==NULL || _state->worker_thread==NULL )
    {
        AE_CRITICAL_ASSERT(_child_tasks==NULL && !smp_enabled);
        return;
    }
    if( _child_tasks==NULL )
    {
        AE_CRITICAL_ASSERT(!smp_enabled);
        return;
    }
    ae_wait_for_group(_child_tasks, dispose_group, _state);
}


/*************************************************************************
This function frees disposed instances of ae_task_info and ae_task_group.

It is used in the unit testing because having unfreed  instances  prevents
us  from  distinguishing  between  true   memory leaks and simply disposed
instances.

NOTE: thus function is thread-unsafe! You should call it at most once from
      one and just one thread (main program thread).
*************************************************************************/
void ae_free_disposed_items()
{
    if( main_thread_pool==NULL )
        return;
    
    /* free disposed tasks */
    ae_acquire_lock(&main_thread_pool->tasks_lock);
    while( main_thread_pool->disposed_tasks!=NULL )
    {
        ae_task_info *task = (ae_task_info*)main_thread_pool->disposed_tasks;
        main_thread_pool->disposed_tasks = task->next_task;
        ae_destroy_and_free_task(task);
    }
    ae_release_lock(&main_thread_pool->tasks_lock);
    
    /* free disposed groups */
    ae_acquire_lock(&main_thread_pool->groups_lock);
    while( main_thread_pool->disposed_groups!=NULL )
    {
        ae_task_group *group = (ae_task_group*)main_thread_pool->disposed_groups;
        main_thread_pool->disposed_groups = group->next_group;
        ae_destroy_and_free_group(group);
    }
    ae_release_lock(&main_thread_pool->groups_lock);
}


/*************************************************************************
This function completely frees all thread-related structures, including
disposed instances of ae_task_info and ae_task_group, worker threads and
all related data.

It is used in the unit testing just before finalizing entire program. Calling
this function helps Valgrind to produce clean report with no "still reachable"
temporary structures.

NOTE: thus function is thread-unsafe! You should call it at most once from
      one and just one thread (main program thread).
*************************************************************************/
void ae_complete_finalization_before_exit()
{   
    /*
     * If no thread pool was initialized, do nothing
     */
    if( main_thread_pool==NULL )
        return;
    
    /*
     * Set finalization request flag
     */
    finalization_request = ae_true;
    
    /*
     * Free disposed tasks and groups
     */
    ae_free_disposed_items();
    
    /*
     * Finalize disposed worker threads
     */
    ae_acquire_lock(&main_thread_pool->threads_lock);
    while( main_thread_pool->disposed_threads!=NULL )
    {
        ae_worker_thread *thread;
        
        /*
         * wake up disposed workers one by one; because finalization
         * request flag was set, we do not have to tie them to cores
         * - workers will notice the flag and jump immediately to the
         * finalization code.
         */
        thread = (ae_worker_thread*)main_thread_pool->disposed_threads;
        main_thread_pool->disposed_threads = thread->next_thread;
        ae_set_event(&thread->wakeup_event);
    }
    ae_release_lock(&main_thread_pool->threads_lock);
    
    /*
     * Wakeup non-disposed worker threads
     */
    ae_set_event(&main_thread_pool->root_tasks_are_present);
    
    /*
     * Sleep
     */
    ae_sleep_thread(500);
}


/*************************************************************************
Perform a quick scan and return an index of non-empty queue. The queue may
be emptied by someone else working from a parallel thread, so  the  result
is not guaranteed to point to non-empty queue.

Returns -1 if all queues are empty.

This function has a data race inside, it is not an error.
*************************************************************************/
static ae_int_t quick_scan_for_nonempty(ae_thread_pool *pool, ae_int_t scans_count)
{
    for(ae_int_t i=0; i<=scans_count; i++)
    {
        for(ae_int_t j=0; j<pool->queues_count; j++)
            if( pool->queues[j].cnt>0 )
                return j;
    }
    return -1;
}


#if AE_OS==AE_WINDOWS
void ae_worker_loop(void *T)
#elif AE_OS==AE_POSIX
void* ae_worker_loop(void *T)
#else
void ae_worker_loop(void *T)
#endif
{
    ae_worker_thread *thread;
    ae_thread_pool *pool;
    ae_state _state;
    ae_int_t result, hint_index, queue_idx;

    /*
    ae_int_t i;
    ae_int_t num_queue = -1, prompt = -1;
    ae_task_info *local_task;
    ae_thread_parameter *local_data = (ae_thread_parameter*)T;
*/
    
    thread = (ae_worker_thread*)T;
    
    /*
     * Wait for wakeup_event.
     *
     * Pin thhread to worker queue (some tricky ops with worker_idx are
     * required because ae_pin_thread needs thread with clear worked_idx).
     *
     */
    ae_wait_for_event(&thread->wakeup_event);
    queue_idx = thread->worker_idx;
    thread->worker_idx = -1;
    ae_pin_thread(thread, queue_idx);
    
    /*
     * Outer loop:
     * 1. execute inner loop (until exit from loop)
     * 2. wait for pool->root_tasks_are_present
     * 3. goto (1)
     */
    pool = main_thread_pool;
    AE_CRITICAL_ASSERT(pool!=NULL);
    AE_CRITICAL_ASSERT(thread->worker_idx>0 && thread->worker_idx<pool->queues_count);
    for(;;)
    {
        /*
         * Inner loop:
         * 0. in case worker_idx is greater than number of cores to use (as requested by user),
         *    sleep for 1 ms, then exit inner loop
         * 1. perform full scan (with synchronization) for new tasks to process:
         *    pop from our queue, steal from other queues.
         *    In case task is found, goto (4). Goto (2) otherwise.
         * 2. in case pool->root_cnt is zero, exit inner loop
         * 3. perform for AE_QUICK_SCANS_COUNT quick scans (without synchronization),
         *    goto (0) in case something was detected (or all AE_QUICK_SCANS_COUNT
         *    scans were negative).
         * 4. solve task
         * 5. depending on task result, dispose worker (we awoke another
         *    worker which owns our queue) or goto (0)
         *
         */
        hint_index = -1;
        for(;;)
        {
            ae_task_info *current_task;
            ae_int_t i, j;
            AE_CRITICAL_ASSERT(thread->worker_idx>0 && thread->worker_idx<pool->queues_count);
            
            /*
             * (0) check that worker index is less than or equal to number of cores which are allowed to use.
             *     This block allows user to dynamically control number of cores which can be utilized
             *     by ALGLIB.
             *
             *     In case this specific worker is restricted from performing activity, we perform short sleep
             *     for AE_SLEEP_ON_IDLE ms, and then we jump back to outer loop.
             *
             *     NOTE 1: short period of sleep is important because it prevents us from utilizing all CPU
             *             power while continuously checking for permission to work.
             *
             *     NOTE 2: worker may leave some unprocessed tasks in its queue; it is not problem because
             *             these tasks will be stolen by some other worker.
             */
            if( thread->worker_idx>ae_cores_to_use() )
            {
                ae_sleep_thread(AE_SLEEP_ON_IDLE);
                break;
            }
            
            /*
             * (1) scan for new tasks:
             *     * first, we try to pop from our own queue
             *     * second, we try to steal task from quueue hint_index (in case hint_index is non-negative).
             *       in any case hint_index is reset to -1 after this block.
             *     * then, we try to steal from non-root queues
             *     * finally, we try to steal from root queue
             */
            AE_CRITICAL_ASSERT(hint_index<pool->queues_count);
            current_task = ae_pop_task(thread->worker_idx);
            if( current_task==NULL && hint_index>=0 )
                current_task = ae_steal_task(hint_index);
            hint_index = -1;
            if( current_task==NULL )
            {
                ae_int_t offs = thread->worker_idx;
                ae_int_t cnt = pool->queues_count;
                for(i=0; i<=cnt; i++)
                    if( (i+offs)%cnt!=thread->worker_idx && (i+offs)%cnt!=0 )
                    {
                        current_task = ae_steal_task((i+offs)%cnt);
                        if( current_task!=NULL )
                            break;
                    }
            }
            if( current_task==NULL )
                current_task = ae_steal_task(0);
            
            /*
             * (2), (3): no task found
             */
            if( current_task==NULL )
            {
                /*
                 * No tasks found.
                 * Depending on presense of active root tasks we either 
                 * a) BREAK to outer cycle (no root tasks running)
                 * b) perform specified amount of quick scans.
                 */
                ae_bool no_root_tasks;
                
                /* breaking to ourer cycle if needed */
                ae_acquire_lock(&pool->queues[0].queue_lock);
                no_root_tasks = pool->root_cnt==0;
                ae_release_lock(&pool->queues[0].queue_lock);
                if( no_root_tasks )
                    break;
                
                /* wait (we scan queues during waiting) and continue inner cycle */
                hint_index = quick_scan_for_nonempty(pool, AE_QUICK_SCANS_COUNT);
                if( hint_index<0 )
                {
#ifdef AE_SMP_DEBUGCOUNTERS
                    InterlockedIncrement((LONG volatile *)&_ae_dbg_worker_yields);
#endif
                    ae_yield();
                }
                continue;
            }
            
            /*
             * (4) execute task
             */
            AE_CRITICAL_ASSERT(thread->worker_idx>=0 && thread->worker_idx<pool->queues_count);
            ae_state_init(&_state);
            _state.worker_thread = (void*)thread;
            result = ae_solve_task(current_task, &_state, ae_true);
            ae_state_clear(&_state);
            AE_CRITICAL_ASSERT( (result==AE_WRK_DISPOSE) || (thread->worker_idx>=0 && thread->worker_idx<pool->queues_count) );
            
            // TODO: check that after successful solution of the task worker thread have no child groups.
            //       in case there exists unprocessed group, user-mode exception should be generated.
            //       the only situation where there can be unprocessed groups is when task generated exception
            //       during its execution.
            
            /*
             * (5) if worker was disposed then:
             *     * send it to the list of disposed workers (call ae_dispose_worker)
             *     * wait until disposed worker wakes up (return from ae_dispose_worker)
             *     * check finalization request - finalize worker if needed
             */
            if( result==AE_WRK_DISPOSE )
            {
                ae_dispose_worker(thread);
                if( finalization_request )
                    goto lbl_finalize;
            }
        }
        
        /*
         * We've exited from inner loop, it means that no active root
         * tasks is present. Wait for new tasks to come. Check finalization
         * request on wakeup.
         */
        ae_wait_for_event(&pool->root_tasks_are_present);
        if( finalization_request )
            goto lbl_finalize;
    }
    
    /*
     * Someone requested finalization of all thread-related structures.
     *
     * It is usually done in Valgrind doctests/unittests/xtests in order
     * to deallocate some internally allocated structures.
     */
lbl_finalize:
    ae_free_event(&thread->wakeup_event);
    free(thread->thread_handle);
    free(thread);
#if AE_OS==AE_WINDOWS
#elif AE_OS==AE_POSIX
    pthread_detach(pthread_self());
    return NULL;
#else
#endif
}


/*************************************************************************
SMP self-tests.

Test 1: test for basic processing and exception handling.

Task parameters:
- params[0].val      array to sort
- params[1].val      temporary buffer
- params[2].ival     left bound of [i0,i1) subinterval to sort
- params[3].ival     right bound of [i0,i1) subinterval to sort
- params[4].ival     task with i0=params[4].ival=i1-1 throws exception
*************************************************************************/

/* test 1: child function - merge sorted subarrays */
void smptests_test1_merge_func(struct ae_task_data *data, ae_state *_state)
{
    ae_int_t *a, *buf, idx0, idx1, idx2, srcleft, srcright, dst;

    a    = (ae_int_t*)data->parameters[0].value.val;
    buf  = (ae_int_t*)data->parameters[1].value.val;
    idx0 = data->parameters[2].value.ival;
    idx1 = data->parameters[3].value.ival;
    idx2 = data->parameters[4].value.ival;
    
    srcleft = idx0;
    srcright = idx1;
    dst = idx0;
    for(;;)
    {
        if( srcleft==idx1&&srcright==idx2 )
        {
            break;
        }
        if( srcleft==idx1 )
        {
            buf[dst] = a[srcright];
            srcright = srcright+1;
            dst = dst+1;
            continue;
        }
        if( srcright==idx2 )
        {
            buf[dst] = a[srcleft];
            srcleft = srcleft+1;
            dst = dst+1;
            continue;
        }
        if( a[srcleft]<a[srcright] )
        {
            buf[dst] = a[srcleft];
            srcleft = srcleft+1;
            dst = dst+1;
        }
        else
        {
            buf[dst] = a[srcright];
            srcright = srcright+1;
            dst = dst+1;
        }
    }
    for(dst=idx0; dst<=idx2-1; dst++)
        a[dst] = buf[dst];
}

/* root function for test 1 */
void smptests_test1_root_func(struct ae_task_data *data, ae_state *_state)
{
    /*
     * unload parameters
     */
    ae_int_t *arr, *buf, i0, i1, idxa, eidx;
    ae_task_group *group0, *group1;
    ae_task_info  *task0, *task1;
    
    arr  = (ae_int_t*)data->parameters[0].value.val;
    buf  = (ae_int_t*)data->parameters[1].value.val;
    i0   = data->parameters[2].value.ival;
    i1   = data->parameters[3].value.ival;
    eidx = data->parameters[4].value.ival;
    
    /* exit on unit subproblem */
    if( i0==i1-1 )
    {
        ae_assert(eidx<i0 || eidx>=i1, "exception generated", _state);
        return;
    }
    
    /* split subproblem into [i0, idxa) and [idxa, i1) */
    do { idxa = i0+ae_rand()%(i1-i0); } while( idxa==i0 || idxa==i1 );
    group0 = ae_create_task_group(_state);
    task0 = ae_create_task(group0, _state);
    task0->data.func = smptests_test1_root_func;
    task0->data.parameters[0].value.val = arr;
    task0->data.parameters[1].value.val = buf;
    task0->data.parameters[2].value.ival = i0;
    task0->data.parameters[3].value.ival = idxa;
    task0->data.parameters[4].value.ival = eidx;
    ae_push_task(task0, _state, ae_false);
    task1 = ae_create_task(group0, _state);
    task1->data.func = smptests_test1_root_func;
    task1->data.parameters[0].value.val = arr;
    task1->data.parameters[1].value.val = buf;
    task1->data.parameters[2].value.ival = idxa;
    task1->data.parameters[3].value.ival = i1;
    task1->data.parameters[4].value.ival = eidx;
    ae_push_task(task1, _state, ae_false);
    ae_wait_for_group(group0, ae_true, _state);
    
    /* merge */
    group1 = ae_create_task_group(_state);
    task0 = ae_create_task(group1, _state);
    task0->data.func = smptests_test1_merge_func;
    task0->data.parameters[0].value.val = arr;
    task0->data.parameters[1].value.val = buf;
    task0->data.parameters[2].value.ival = i0;
    task0->data.parameters[3].value.ival = idxa;
    task0->data.parameters[4].value.ival = i1;
    ae_push_task(task0, _state, ae_false);
    ae_wait_for_group(group1, ae_true, _state);
}

/* this function returns true on success, false on failure */
ae_bool smptests_perform_test1()
{
    ae_int_t *arr, *buf, n, pass, i, j, k, eidx;
    ae_task_info *task;
    ae_bool result;
    ae_state _alglib_env_state;

    ae_state_init(&_alglib_env_state);
    n = 100000;
    arr = (ae_int_t*)malloc(sizeof(ae_int_t)*(size_t)n);
    buf = (ae_int_t*)malloc(sizeof(ae_int_t)*(size_t)n);
    result = ae_true;
    task = NULL;
    for(pass=0; pass<2; pass++)
    {
        /* decide whether we want to raise exception or to solve problem successfully */
        if( pass==0 )
            eidx = -1;
        else
            eidx = ae_rand()%n;
    
        /* create task */
        for(i=0; i<n; i++)
            arr[i] = i;
        for(i=0; i<n; i++)
        {
            j = ae_rand()%n;
            if( j!=i )
            {
                k = arr[i];
                arr[i] = arr[j];
                arr[j] = k;
            }
        }
        task = ae_create_task(NULL, &_alglib_env_state);
        task->data.func = smptests_test1_root_func;
        task->data.parameters[0].value.val = arr;
        task->data.parameters[1].value.val = buf;
        task->data.parameters[2].value.ival = 0;
        task->data.parameters[3].value.ival = n;
        task->data.parameters[4].value.ival = eidx;
        
        /* push task, no exception should be generated at this moment */
        ae_push_root_task(task);
        
        /* wait for the solution, no exception should be generated at this moment */
        ae_wait_for_event(&task->done_event);
    
        /* check solution status */
        if( eidx>=0 && eidx<n && task->exception==NULL )
        { result = ae_false; goto lbl_finalize; }
        if( (eidx<0 || eidx>n) && task->exception!=NULL )
        { result = ae_false; goto lbl_finalize; }
        if( eidx<0 || eidx>n )
            for(i=0; i<n; i++)
                if( arr[i]!=i )
                { result = ae_false; goto lbl_finalize; }
        
        /* dispose */
        ae_dispose_task(task);
        task = NULL;
    }
    
lbl_finalize:
    ae_state_clear(&_alglib_env_state);
    if( task!=NULL )
        ae_dispose_task(task);
    free(arr);
    free(buf);
    return result;
}


/*************************************************************************
SMP self-tests.

Test 2: this test spawns A LOT OF root tasks. The idea is to
        check scheduler's ability to handle such stream of tasks.

Task input parameters:
- params[0].ival     input value

Task output parameters:
- params[0].ival     input*input+1

*************************************************************************/

/* root function for test 2 */
void smptests_test2_root_func(struct ae_task_data *data, ae_state *_state)
{
    ae_int_t v;
    v = data->parameters[0].value.ival;
    data->parameters[0].value.ival = v*v+1;
}

/* this function returns true on success, false on failure */
ae_bool smptests_perform_test2()
{
    ae_task_info **tasks;
    ae_bool result;
    ae_state _alglib_env_state;
    ae_int_t n, i;

    n = 100000;
    result = ae_true;
    
    /* allocate space */
    ae_state_init(&_alglib_env_state);
    tasks = (ae_task_info**)malloc(sizeof(ae_task_info*)*(size_t)n);
    
    /*
     * Create and push tasks.
     * NOTE: we store tasks into the array before pushing in order
     *       to push them as fast as possible.
     */
    for(i=0; i<n; i++)
    {
        tasks[i] = ae_create_task(NULL, &_alglib_env_state);
        tasks[i]->data.parameters[0].value.ival = i;
        tasks[i]->data.func = smptests_test2_root_func;
    }
    for(i=0; i<n; i++)
        ae_push_root_task(tasks[i]);
    
    /*
     * wait for tasks and check results
     */
    for(i=0; i<n; i++)
    {
        ae_wait_for_event(&tasks[i]->done_event);
        result = result && (tasks[i]->data.parameters[0].value.ival==i*i+1);
    }
    
    /* dispose */
    for(i=0; i<n; i++)
        ae_dispose_task(tasks[i]);
    free(tasks);
    
    return result;
}


/*************************************************************************
SMP self-tests.

Test 3: this test spawns A LOT OF child tasks. The idea is to
        check scheduler's ability to handle such stream of tasks.

Child task input parameters:
- params[0].ival     input value
- params[1].val      double* to store result=sin(input)

Root task input parameters:
- params[0].ival     N

Root task output parameters:
- params[0].dval     sum of child outputs for i=0..N-1
*************************************************************************/

/* child function for test 3 */
void smptests_test3_child_func(struct ae_task_data *data, ae_state *_state)
{
    ae_int_t n;
    double v;
    volatile ae_int_t cnt;
    
    /* slow down problem a bit - problems should require
       some amount of time in order to let queue overflow */
    n = 10000;
    for(cnt=0; cnt<n; cnt++);
    
    /* solve problem */
    v = (double)(data->parameters[0].value.ival);
    *((double*)data->parameters[1].value.val) = sin(v);
}


/* root function for test 3 */
void smptests_test3_root_func(struct ae_task_data *data, ae_state *_state)
{
    ae_task_info **tasks;
    ae_task_group **groups;
    double *results;
    ae_int_t n, i;
    double result;

    n = data->parameters[0].value.ival;
    
    /* allocate space */
    tasks   = (ae_task_info**)malloc(sizeof(ae_task_info*)*(size_t)n);
    groups  = (ae_task_group**)malloc(sizeof(ae_task_group*)*(size_t)n);
    memset(groups, 0, sizeof(ae_task_group*)*(size_t)n);
    results = (double*)malloc(sizeof(double)*(size_t)n);
    
    /*
     * Create and push tasks.
     * NOTE: we store tasks into the array before pushing in order
     *       to push them as fast as possible.
     */
    for(i=0; i<n; i++)
    {
        groups[i] = ae_create_task_group(_state);
        tasks[i] = ae_create_task(groups[i], _state);
        tasks[i]->data.parameters[0].value.ival = i;
        tasks[i]->data.parameters[1].value.val  = (void*)(results+i);
        tasks[i]->data.func = smptests_test3_child_func;
        results[i] = 0.0;
    }
    for(i=0; i<n; i++)
        ae_push_task(tasks[i], _state, ae_false);
    
    /*
     * wait for tasks and check results.
     *
     * NOTE: it is important to wait for groups in backward order,
     *       i.e. most recently added group will be waited first.
     *
     *       In fact, it is possible to wait for groups in arbitrary
     *       order, but when we wait for the most recent group, is is
     *       removed from the beginning of the list - almost immediately.
     *       And when we wait for a group in another end of the list,
     *       we have to spent too much time traversing list and removing
     *       group from it.
     */
    result = 0.0;
    for(i=n-1; i>=0; i--)
    {
        ae_wait_for_group(groups[i], ae_true, _state);
        result += results[i];
    }
    data->parameters[0].value.dval = result;
    
    /* dispose */
    free(tasks);
    free(groups);
    free(results);
}

/* this function returns true on success, false on failure */
ae_bool smptests_perform_test3()
{
    ae_task_info *task;
    ae_bool result;
    ae_state _alglib_env_state;
    ae_int_t n, i;
    double t;

    n = 99991;
    ae_state_init(&_alglib_env_state);
    task = ae_create_task(NULL, &_alglib_env_state);
    task->data.parameters[0].value.ival = n;
    task->data.func = smptests_test3_root_func;
    ae_push_root_task(task);
    ae_wait_for_event(&task->done_event);
    t = 0.0;
    for(i=0; i<n; i++)
        t = t+sin((double)i);
    result = fabs(task->data.parameters[0].value.dval-t)<=1.0E-6*fabs(t);
    //printf("%.9f vs %.9f = %.9f\n", (double)task->data.parameters[0].value.dval, (double)t, (double)fabs(task->data.parameters[0].value.dval-t));
    ae_dispose_task(task);
    return result;
}

/*************************************************************************
SMP self-tests.

Returns true on success, false on failures.
*************************************************************************/
ae_bool ae_smpselftests()
{
    ae_bool t1, t2, t3;
    
    t1 = smptests_perform_test1();
    t2 = smptests_perform_test2();
    t3 = smptests_perform_test3();
    
    return t1 && t2 && t3;
}
#endif /* _ALGLIB_HAS_WORKSTEALING */

/*
 * MKL links
 *
 * NOTE: this code is located in smp.c because it is NOT included in GPL
 *       version of ALGLIB.
 */
#ifdef ALGLIB_INTERCEPTS_MKL
#ifdef AE_USE_CPP
}
extern "C" {
#endif

char _alglib2mkl_disable_fast_mm();
#if defined(AE_USE_CPP)
alglib_impl::ae_int64_t _alglib2mkl_memstat();
#else
ae_int64_t _alglib2mkl_memstat();
#endif

char _alglib2mkl_rmatrixtrsv(
     ptrdiff_t n,
     const double *a_ptr,
     ptrdiff_t a_stride,
     char isupper,
     char isunit,
     ptrdiff_t opa,
     double *x_ptr,
     ptrdiff_t x_stride);
char _alglib2mkl_cmatrixgeru(
     ptrdiff_t m,
     ptrdiff_t n,
     void* a_ptr,
     ptrdiff_t a_stride,
     const void *alpha,
     const void *u_ptr,
     ptrdiff_t u_stride,
     const void *v_ptr,
     ptrdiff_t v_stride);
char _alglib2mkl_cmatrixgerc(
     ptrdiff_t m,
     ptrdiff_t n,
     void* a_ptr,
     ptrdiff_t a_stride,
     const void *alpha,
     const void *u_ptr,
     ptrdiff_t u_stride,
     const void *v_ptr,
     ptrdiff_t v_stride);
char _alglib2mkl_rmatrixger(
     ptrdiff_t m,
     ptrdiff_t n,
     double* a_ptr,
     ptrdiff_t a_stride,
     double alpha,
     const double *u_ptr,
     ptrdiff_t u_stride,
     const double *v_ptr,
     ptrdiff_t v_stride);
char _alglib2mkl_cmatrixgemv(
     ptrdiff_t m,
     ptrdiff_t n,
     const void *alpha,
     const void *a_ptr,
     ptrdiff_t a_stride,
     ptrdiff_t optypea,
     const void *x_ptr,
     ptrdiff_t x_stride,
     const void *beta,
     void *y_ptr,
     ptrdiff_t y_stride);
char _alglib2mkl_rmatrixgemv(
     ptrdiff_t m,
     ptrdiff_t n,
     double alpha,
     const double *a_ptr,
     ptrdiff_t a_stride,
     ptrdiff_t optypea,
     const double *x_ptr,
     ptrdiff_t x_stride,
     double beta,
     double *y_ptr,
     ptrdiff_t y_stride);
char _alglib2mkl_rmatrixsymv(
     ptrdiff_t n,
     double alpha,
     const double *a_ptr,
     ptrdiff_t a_stride,
     char isupper,
     const double *x_ptr,
     ptrdiff_t x_stride,
     double beta,
     double *y_ptr,
     ptrdiff_t y_stride);
char _alglib2mkl_rmatrixsyrkmkl(
     ptrdiff_t n,
     ptrdiff_t k,
     double alpha,
     double *a_ptr,
     ptrdiff_t a_stride,
     ptrdiff_t optypea,
     double beta,
     double *c_ptr,
     ptrdiff_t c_stride,
     char isupper);
char _alglib2mkl_cmatrixherkmkl(
     ptrdiff_t n,
     ptrdiff_t k,
     double alpha,
     void *a_ptr,
     ptrdiff_t a_stride,
     ptrdiff_t optypea,
     double beta,
     void *c_ptr,
     ptrdiff_t c_stride,
     char isupper);
char _alglib2mkl_rmatrixgemmmkl(
     ptrdiff_t m,
     ptrdiff_t n,
     ptrdiff_t k,
     double alpha,
     double *a_ptr,
     ptrdiff_t a_stride,
     ptrdiff_t optypea,
     double *b_ptr,
     ptrdiff_t b_stride,
     ptrdiff_t optypeb,
     double beta,
     double *c_ptr,
     ptrdiff_t c_stride);
char _alglib2mkl_cmatrixgemmmkl(
     ptrdiff_t m,
     ptrdiff_t n,
     ptrdiff_t k,
     void *alpha,
     void *a_ptr,
     ptrdiff_t a_stride,
     ptrdiff_t optypea,
     void *b_ptr,
     ptrdiff_t b_stride,
     ptrdiff_t optypeb,
     void *beta,
     void *c_ptr,
     ptrdiff_t c_stride);
char _alglib2mkl_rmatrixtrsmmkl(
     char is_rightside,
     ptrdiff_t m,
     ptrdiff_t n,
     double* a,
     ptrdiff_t a_stride,
     char isupper,
     char isunit,
     ptrdiff_t optype,
     double* x,
     ptrdiff_t x_stride);
char _alglib2mkl_cmatrixtrsmmkl(
     char is_rightside,
     ptrdiff_t m,
     ptrdiff_t n,
     void* a,
     ptrdiff_t a_stride,
     char isupper,
     char isunit,
     ptrdiff_t optype,
     void* x,
     ptrdiff_t x_stride);
char _alglib2mkl_spdmatrixcholeskymkl(
     double* a,
     ptrdiff_t a_stride,
     ptrdiff_t n,
     char isupper,
     char* cholresult);
char _alglib2mkl_rmatrixplumkl(
     double* a,
     ptrdiff_t a_stride,
     ptrdiff_t m,
     ptrdiff_t n,
     ptrdiff_t offs,
     ptrdiff_t *pivots);
char _alglib2mkl_rmatrixbdmkl(
     double    *a,
     ptrdiff_t a_stride,
     ptrdiff_t m,
     ptrdiff_t n,
     double    *d,
     double    *e,
     double    *tauq,
     double    *taup);
char _alglib2mkl_rmatrixbdmultiplybymkl(
     double *qp,
     ptrdiff_t qp_stride,
     ptrdiff_t m,
     ptrdiff_t n,
     double *tauq,
     double *taup,
     double *z,
     ptrdiff_t z_stride,
     ptrdiff_t zrows,
     ptrdiff_t zcolumns,
     char byq,
     char fromtheright,
     char dotranspose);
char _alglib2mkl_rmatrixhessenbergmkl(
     double *a,
     ptrdiff_t a_stride,
     ptrdiff_t n,
     double* tau);
char _alglib2mkl_rmatrixhessenbergunpackqmkl(
     double    *a,
     ptrdiff_t a_stride,
     ptrdiff_t n,
     double   *tau,
     double    *q,
     ptrdiff_t q_stride);
char _alglib2mkl_smatrixtdmkl(
     double *a,
     ptrdiff_t a_stride,
     ptrdiff_t n,
     char isupper,
     double* tau,
     double* d,
     double* e);
char _alglib2mkl_smatrixtdunpackqmkl(
     double* a,
     ptrdiff_t a_stride,
     ptrdiff_t n,
     char isupper,
     double* tau,
     double* q,
     ptrdiff_t q_stride);
char _alglib2mkl_hmatrixtdmkl(
     void *a,
     ptrdiff_t a_stride,
     ptrdiff_t n,
     char isupper,
     void* tau,
     double* d,
     double* e);
char _alglib2mkl_hmatrixtdunpackqmkl(
     void* a,
     ptrdiff_t a_stride,
     ptrdiff_t n,
     char isupper,
     void* tau,
     void* q,
     ptrdiff_t q_stride);
char _alglib2mkl_rmatrixbdsvdmkl(
     double* d,
     double* e,
     ptrdiff_t n,
     char isupper,
     double* u,
     ptrdiff_t u_stride,
     ptrdiff_t nru,
     double* c,
     ptrdiff_t c_stride,
     ptrdiff_t ncc,
     double* vt,
     ptrdiff_t vt_stride,
     ptrdiff_t ncvt,
     char* svdresult);
char _alglib2mkl_rmatrixinternalschurdecompositionmkl(
     double* h,
     ptrdiff_t h_stride,
     ptrdiff_t n,
     ptrdiff_t tneeded,
     ptrdiff_t zneeded,
     double* wr,
     double* wi,
     double* z,
     ptrdiff_t z_stride,
     ptrdiff_t* info);
char _alglib2mkl_rmatrixinternaltrevcmkl(
     double* t,
     ptrdiff_t t_stride,
     ptrdiff_t n,
     ptrdiff_t side,
     ptrdiff_t howmny,
     double* vl,
     ptrdiff_t vl_stride,
     double* vr,
     ptrdiff_t vr_stride,
     ptrdiff_t* m,
     ptrdiff_t* info);
char _alglib2mkl_smatrixtdevdmkl(
     double* d,
     double* e,
     ptrdiff_t n,
     ptrdiff_t zneeded,
     double* z,
     ptrdiff_t z_stride,
     char* evdresult);
char _alglib2mkl_sparsegemvcrsmkl(
     ptrdiff_t opa,
     ptrdiff_t arows,
     ptrdiff_t acols,
     double alpha,
     const double* vals,
     const ptrdiff_t* cidx,
     const ptrdiff_t* ridx,
     const double* x,
     double beta,
     double* y);
#ifdef AE_USE_CPP
}
namespace alglib_impl
{
#endif
ae_bool ae_mkl_disable_fast_mm()
{
    return _alglib2mkl_disable_fast_mm();
}

ae_int64_t ae_mkl_memstat()
{
    return _alglib2mkl_memstat();
}

ae_int_t _ialglib_i_matrixtilesizeb()
{
    return 128;
}

ae_bool _ialglib_i_rmatrixtrsvmkl(ae_int_t n,
     const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     ae_vector* x,
     ae_int_t ix)
{
    /* if vendor kernels are deactivated, skip */
    if( !_use_vendor_kernels )
        return ae_false;
    
    /* handle degenerate cases like empty matrices by ALGLIB - greatly simplifies passing data to MKL */
    if( n<=0)
        return ae_false;
    
    /* MKL */
    return _alglib2mkl_rmatrixtrsv(
        n,
        &(a->ptr.pp_double[ia][ja]), a->stride,
        isupper, isunit, optype,
        x->ptr.p_double+ix, 1);
}

ae_bool _ialglib_i_rmatrixgermkl(ae_int_t m,
     ae_int_t n,
     ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     double alpha,
     const ae_vector* u,
     ae_int_t iu,
     const ae_vector* v,
     ae_int_t iv)
{
    /* if vendor kernels are deactivated, skip */
    if( !_use_vendor_kernels )
        return ae_false;
    
    /* handle degenerate cases like empty matrices by ALGLIB - greatly simplifies passing data to MKL */
    if( m<=0 || n<=0)
        return ae_false;
    
    /* MKL */
    return _alglib2mkl_rmatrixger(
        m, n,
        &(a->ptr.pp_double[ia][ja]), a->stride,
        alpha,
        u->ptr.p_double+iu, 1,
        v->ptr.p_double+iv, 1);
}

ae_bool _ialglib_i_rmatrixgemvmkl(ae_int_t m,
     ae_int_t n,
     double alpha,
     const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t opa,
     const ae_vector* x,
     ae_int_t ix,
     double beta,
     ae_vector* y,
     ae_int_t iy)
{  
    /* if vendor kernels are deactivated, skip */
    if( !_use_vendor_kernels )
        return ae_false;
    
    /* handle degenerate cases like empty matrices by ALGLIB - greatly simplifies passing data to MKL */
    if( m<=0 || n<=0)
        return ae_false;
    
    /* MKL */
    return _alglib2mkl_rmatrixgemv(
        opa==0 ? m : n, opa==0 ? n : m, /* _alglib2mkl_rmatrixgemv() needs (m,n)=sizeof(A), whilst ALGLIB uses (m,n)=sizeof(op(A)) */
        alpha,
        &(a->ptr.pp_double[ia][ja]), a->stride, opa,
        x->ptr.p_double+ix, 1,
        beta,
        y->ptr.p_double+iy, 1);
}

ae_bool _ialglib_i_rmatrixsymvmkl(
     ae_int_t n,
     double alpha,
     const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_bool isupper,
     const ae_vector* x,
     ae_int_t ix,
     double beta,
     ae_vector* y,
     ae_int_t iy)
{
    /* if vendor kernels are deactivated, skip */
    if( !_use_vendor_kernels )
        return ae_false;
    
    /* handle degenerate cases like empty matrices by ALGLIB - greatly simplifies passing data to MKL */
    if( n<=0)
        return ae_false;
    
    /* MKL */
    return _alglib2mkl_rmatrixsymv(
        n,
        alpha,
        &(a->ptr.pp_double[ia][ja]), a->stride, isupper,
        x->ptr.p_double+ix, 1,
        beta,
        y->ptr.p_double+iy, 1);
}

ae_bool _ialglib_i_cmatrixrank1mkl(ae_int_t m,
     ae_int_t n,
     ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     const ae_vector* u,
     ae_int_t iu,
     const ae_vector* v,
     ae_int_t iv)
{
    /* if vendor kernels are deactivated, skip */
    if( !_use_vendor_kernels )
        return ae_false;
    
    /* handle degenerate cases like empty matrices by ALGLIB - greatly simplifies passing data to MKL */
    if( m<=0 || n<=0)
        return ae_false;
    
    /* MKL */
    ae_complex alpha;
    alpha.x = 1.0;
    alpha.y = 0.0;
    return _alglib2mkl_cmatrixgeru(
        m, n,
        &(a->ptr.pp_complex[ia][ja]), a->stride,
        &alpha,
        u->ptr.p_complex+iu, 1,
        v->ptr.p_complex+iv, 1);
}


ae_bool _ialglib_i_rmatrixrank1mkl(ae_int_t m,
     ae_int_t n,
     ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     const ae_vector* u,
     ae_int_t iu,
     const ae_vector* v,
     ae_int_t iv)
{
    /* if vendor kernels are deactivated, skip */
    if( !_use_vendor_kernels )
        return ae_false;
    
    /* handle degenerate cases like empty matrices by ALGLIB - greatly simplifies passing data to MKL */
    if( m<=0 || n<=0)
        return ae_false;
    
    /* MKL */
    return _alglib2mkl_rmatrixger(
        m, n,
        &(a->ptr.pp_double[ia][ja]), a->stride,
        1.0,
        u->ptr.p_double+iu, 1,
        v->ptr.p_double+iv, 1);
}

ae_bool _ialglib_i_cmatrixmvmkl(ae_int_t m,
     ae_int_t n,
     const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t opa,
     const ae_vector* x,
     ae_int_t ix,
     ae_vector* y,
     ae_int_t iy)
{
    /* if vendor kernels are deactivated, skip */
    if( !_use_vendor_kernels )
        return ae_false;
    
    /* handle degenerate cases like empty matrices by ALGLIB - greatly simplifies passing data to MKL */
    if( m<=0 || n<=0)
        return ae_false;
    
    /* MKL */
    ae_complex alpha, beta;
    alpha.x = 1.0;
    alpha.y = 0.0;
    beta.x = 0.0;
    beta.y = 0.0;
    return _alglib2mkl_cmatrixgemv(
        opa==0 ? m : n, opa==0 ? n : m, /* _alglib2mkl_cmatrixgemv() needs (m,n)=sizeof(A), whilst ALGLIB uses (m,n)=sizeof(op(A)) */
        &alpha,
        &(a->ptr.pp_complex[ia][ja]), a->stride, opa,
        x->ptr.p_complex+ix, 1,
        &beta,
        y->ptr.p_complex+iy, 1);
}

ae_bool _ialglib_i_rmatrixmvmkl(ae_int_t m,
     ae_int_t n,
     const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t opa,
     const ae_vector* x,
     ae_int_t ix,
     ae_vector* y,
     ae_int_t iy)
{  
    /* if vendor kernels are deactivated, skip */
    if( !_use_vendor_kernels )
        return ae_false;
    
    /* handle degenerate cases like empty matrices by ALGLIB - greatly simplifies passing data to MKL */
    if( m<=0 || n<=0)
        return ae_false;
    
    /* MKL */
    return _alglib2mkl_rmatrixgemv(
        opa==0 ? m : n, opa==0 ? n : m, /* _alglib2mkl_rmatrixgemv() needs (m,n)=sizeof(A), whilst ALGLIB uses (m,n)=sizeof(op(A)) */
        1.0,
        &(a->ptr.pp_double[ia][ja]), a->stride, opa,
        x->ptr.p_double+ix, 1,
        0.0,
        y->ptr.p_double+iy, 1);
}
 
ae_bool _ialglib_i_rmatrixsyrkmkl(ae_int_t n,
     ae_int_t k,
     double alpha,
     const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     double beta,
     ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_bool isupper)
{
    /* if vendor kernels are deactivated, skip */
    if( !_use_vendor_kernels )
        return ae_false;
    
    /* handle degenerate cases like zero matrices by ALGLIB - greatly simplifies passing data to MKL */
    if( alpha==0.0 || k==0 || n==0)
        return ae_false;
        
    /* MKL */
    return _alglib2mkl_rmatrixsyrkmkl(
        n, k,
        alpha, &(a->ptr.pp_double[ia][ja]), a->stride, optypea,
        beta, &(c->ptr.pp_double[ic][jc]), c->stride, isupper);
}

ae_bool _ialglib_i_cmatrixherkmkl(ae_int_t n,
     ae_int_t k,
     double alpha,
     const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     double beta,
     ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_bool isupper)
{
    /* if vendor kernels are deactivated, skip */
    if( !_use_vendor_kernels )
        return ae_false;
    
    /* handle degenerate cases like zero matrices by ALGLIB - greatly simplifies passing data to MKL */
    if( alpha==0.0 || k==0 || n==0)
        return ae_false;
        
    /* MKL */
    return _alglib2mkl_cmatrixherkmkl(
        n, k,
        alpha, &(a->ptr.pp_complex[ia][ja]), a->stride, optypea,
        beta, &(c->ptr.pp_complex[ic][jc]), c->stride, isupper);
}

ae_bool _ialglib_i_rmatrixgemmmkl(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     double alpha,
     const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_int_t optypeb,
     double beta,
     ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc)
{
    /* if vendor kernels are deactivated, skip */
    if( !_use_vendor_kernels )
        return ae_false;
    
    /* handle degenerate cases like zero matrices by ALGLIB - greatly simplifies passing data to MKL */
    if( alpha==0.0 || k==0 || n==0 || m==0)
        return ae_false;
        
    /* MKL */
    return _alglib2mkl_rmatrixgemmmkl(
        m, n, k,
        alpha,
        &(a->ptr.pp_double[ia][ja]), a->stride, optypea,
        &(b->ptr.pp_double[ib][jb]), b->stride, optypeb,
        beta, &(c->ptr.pp_double[ic][jc]), c->stride);
}

ae_bool _ialglib_i_cmatrixgemmmkl(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     ae_complex alpha,
     const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_int_t optypeb,
     ae_complex beta,
     ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc)
{
    /* if vendor kernels are deactivated, skip */
    if( !_use_vendor_kernels )
        return ae_false;
    
    /* handle degenerate cases like zero matrices by ALGLIB - greatly simplifies passing data to MKL */
    if( (alpha.x==0.0 && alpha.y==0) || k==0 || n==0 || m==0 )
        return ae_false;
        
    /* MKL */
    return _alglib2mkl_cmatrixgemmmkl(
        m, n, k,
        (void*)&alpha,
        (void*)&(a->ptr.pp_complex[ia][ja]), a->stride, optypea,
        (void*)&(b->ptr.pp_complex[ib][jb]), b->stride, optypeb,
        (void*)&beta,
        (void*)&(c->ptr.pp_complex[ic][jc]), c->stride);
}

ae_bool _ialglib_i_rmatrixlefttrsmmkl(ae_int_t m,
     ae_int_t n,
     const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2)
{
    /* if vendor kernels are deactivated, skip */
    if( !_use_vendor_kernels )
        return ae_false;
    
    /* handle degenerate cases like zero matrices by ALGLIB - greatly simplifies passing data to MKL */
    if( m==0 || n==0)
        return ae_false;
        
    /* MKL */
    return _alglib2mkl_rmatrixtrsmmkl(
        0,
        m, n,
        &(a->ptr.pp_double[i1][j1]), a->stride, isupper, isunit, optype,
        &(x->ptr.pp_double[i2][j2]), x->stride);
}

ae_bool _ialglib_i_rmatrixrighttrsmmkl(ae_int_t m,
     ae_int_t n,
     const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2)
{
    /* if vendor kernels are deactivated, skip */
    if( !_use_vendor_kernels )
        return ae_false;
    
    /* handle degenerate cases like zero matrices by ALGLIB - greatly simplifies passing data to MKL */
    if( m==0 || n==0)
        return ae_false;
        
    /* MKL */
    return _alglib2mkl_rmatrixtrsmmkl(
        1,
        m, n,
        &(a->ptr.pp_double[i1][j1]), a->stride, isupper, isunit, optype,
        &(x->ptr.pp_double[i2][j2]), x->stride);
}

ae_bool _ialglib_i_cmatrixlefttrsmmkl(ae_int_t m,
     ae_int_t n,
     const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2)
{
    /* if vendor kernels are deactivated, skip */
    if( !_use_vendor_kernels )
        return ae_false;
    
    /* handle degenerate cases like zero matrices by ALGLIB - greatly simplifies passing data to MKL */
    if( m==0 || n==0)
        return ae_false;
        
    /* MKL */
    return _alglib2mkl_cmatrixtrsmmkl(
        0,
        m, n,
        (void*)&(a->ptr.pp_complex[i1][j1]), a->stride, isupper, isunit, optype,
        (void*)&(x->ptr.pp_complex[i2][j2]), x->stride);
}

ae_bool _ialglib_i_cmatrixrighttrsmmkl(ae_int_t m,
     ae_int_t n,
     const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2)
{
    /* if vendor kernels are deactivated, skip */
    if( !_use_vendor_kernels )
        return ae_false;
    
    /* handle degenerate cases like zero matrices by ALGLIB - greatly simplifies passing data to MKL */
    if( m==0 || n==0)
        return ae_false;
        
    /* MKL */
    return _alglib2mkl_cmatrixtrsmmkl(
        1,
        m, n,
        (void*)&(a->ptr.pp_complex[i1][j1]), a->stride, isupper, isunit, optype,
        (void*)&(x->ptr.pp_complex[i2][j2]), x->stride);
}

ae_bool _ialglib_i_spdmatrixcholeskymkl(
     ae_matrix* a,
     ae_int_t offs,
     ae_int_t n,
     ae_bool isupper,
     ae_bool* cholresult)
{
    char c_result, c_cholresult;
    if( !_use_vendor_kernels )
        return ae_false;
    c_result = _alglib2mkl_spdmatrixcholeskymkl(
        &(a->ptr.pp_double[offs][offs]),
        a->stride,
        n,
        isupper,
        &c_cholresult);
    *cholresult = c_cholresult;
    return c_result;
}

ae_bool _ialglib_i_rmatrixplumkl(
     ae_matrix* a,
     ae_int_t offs,
     ae_int_t m,
     ae_int_t n,
     ae_vector* pivots)
{
    if( !_use_vendor_kernels )
        return ae_false;
    return _alglib2mkl_rmatrixplumkl(
        &(a->ptr.pp_double[offs][offs]),
        a->stride,
        m,
        n,
        offs,
        &(pivots->ptr.p_int[offs]));
}

ae_bool _ialglib_i_rmatrixbdmkl(
     ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     ae_vector* d,
     ae_vector* e,
     ae_vector* tauq,
     ae_vector* taup)
{
    if( !_use_vendor_kernels )
        return ae_false;    
    return _alglib2mkl_rmatrixbdmkl(
        &(a->ptr.pp_double[0][0]), a->stride,
        m, n,
        &(d->ptr.p_double[0]), &(e->ptr.p_double[0]),
        &(tauq->ptr.p_double[0]), &(taup->ptr.p_double[0]));
}

ae_bool _ialglib_i_rmatrixbdmultiplybymkl(
     const ae_matrix* qp,
     ae_int_t m,
     ae_int_t n,
     const ae_vector* tauq,
     const ae_vector* taup,
     ae_matrix* z,
     ae_int_t zrows,
     ae_int_t zcolumns,
     ae_bool byq,
     ae_bool fromtheright,
     ae_bool dotranspose)
{
    /* if vendor kernels are deactivated, skip */
    if( !_use_vendor_kernels )
        return ae_false;

    /* handle degenerate cases like zero matrices by ALGLIB - greatly simplifies passing data to MKL */
    if( m==0 || n==0 || zrows==0 || zcolumns==0 )
        return ae_false;
        
    /* MKL */
    return _alglib2mkl_rmatrixbdmultiplybymkl(
        &(qp->ptr.pp_double[0][0]), qp->stride,
        m, n,
        tauq->ptr.p_double, taup->ptr.p_double,
        &(z->ptr.pp_double[0][0]), z->stride,
        zrows, zcolumns,
        byq, fromtheright, dotranspose);
}

ae_bool _ialglib_i_rmatrixhessenbergmkl(
     ae_matrix* a,
     ae_int_t n,
     ae_vector* tau)
{
    if( !_use_vendor_kernels )
        return ae_false;
    return _alglib2mkl_rmatrixhessenbergmkl(
        &(a->ptr.pp_double[0][0]), a->stride,
        n,
        tau->ptr.p_double);
}

ae_bool _ialglib_i_rmatrixhessenbergunpackqmkl(
     const ae_matrix* a,
     ae_int_t n,
     const ae_vector* tau,
     ae_matrix* q)
{
    if( !_use_vendor_kernels )
        return ae_false;
    return _alglib2mkl_rmatrixhessenbergunpackqmkl(
        &(a->ptr.pp_double[0][0]), a->stride,
        n,
        tau->ptr.p_double,
        &(q->ptr.pp_double[0][0]), q->stride);
}

ae_bool _ialglib_i_smatrixtdmkl(
     ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     ae_vector* tau,
     ae_vector* d,
     ae_vector* e)
{
    if( !_use_vendor_kernels )
        return ae_false;
    return _alglib2mkl_smatrixtdmkl(
        &(a->ptr.pp_double[0][0]), a->stride,
        n,
        isupper,
        tau->ptr.p_double,
        d->ptr.p_double,
        e->ptr.p_double);
}

ae_bool _ialglib_i_smatrixtdunpackqmkl(
     const ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     const ae_vector* tau,
     ae_matrix* q)
{
    if( !_use_vendor_kernels )
        return ae_false;
    return _alglib2mkl_smatrixtdunpackqmkl(
        &(a->ptr.pp_double[0][0]), a->stride,
        n,
        isupper,
        tau->ptr.p_double,
        &(q->ptr.pp_double[0][0]), q->stride);
}

ae_bool _ialglib_i_hmatrixtdmkl(
     ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     ae_vector* tau,
     ae_vector* d,
     ae_vector* e)
{
    if( !_use_vendor_kernels )
        return ae_false;
    return _alglib2mkl_hmatrixtdmkl(
        (void*)&(a->ptr.pp_complex[0][0]), a->stride,
        n,
        isupper,
        (void*)tau->ptr.p_complex,
        d->ptr.p_double,
        e->ptr.p_double);
}

ae_bool _ialglib_i_hmatrixtdunpackqmkl(
     const ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     const ae_vector* tau,
     ae_matrix* q)
{
    if( !_use_vendor_kernels )
        return ae_false;
    return _alglib2mkl_hmatrixtdunpackqmkl(
        (void*)&(a->ptr.pp_complex[0][0]), a->stride,
        n,
        isupper,
        (void*)tau->ptr.p_complex,
        (void*)&(q->ptr.pp_complex[0][0]), q->stride);
}

ae_bool _ialglib_i_rmatrixbdsvdmkl(
     ae_vector* d,
     ae_vector* e,
     ae_int_t n,
     ae_bool isupper,
     ae_matrix* u,
     ae_int_t nru,
     ae_matrix* c,
     ae_int_t ncc,
     ae_matrix* vt,
     ae_int_t ncvt,
     ae_bool* svdresult)
{
    char cr = 0;
    char fr;
    if( !_use_vendor_kernels )
        return ae_false;
    fr = _alglib2mkl_rmatrixbdsvdmkl(
        d->ptr.p_double, e->ptr.p_double, n, isupper,
         nru>0 ? &(u->ptr.pp_double[0][0]) : NULL,  nru>0 ? u->stride : n, nru,
         ncc>0 ? &(c->ptr.pp_double[0][0]) : NULL,  ncc>0 ? c->stride : 1, ncc,
        ncvt>0 ? &(vt->ptr.pp_double[0][0]): NULL, ncvt>0 ? vt->stride: 1, ncvt,
        &cr);
    if( fr )
        *svdresult = cr;
    return fr;
}

ae_bool _ialglib_i_rmatrixinternalschurdecompositionmkl(
     ae_matrix* h,
     ae_int_t n,
     ae_int_t tneeded,
     ae_int_t zneeded,
     ae_vector* wr,
     ae_vector* wi,
     ae_matrix* z,
     ae_int_t* info)
{
    char r;
    ptrdiff_t locinfo;
    if( !_use_vendor_kernels )
        return ae_false;
    locinfo = 0;
    r = _alglib2mkl_rmatrixinternalschurdecompositionmkl(
        &(h->ptr.pp_double[0][0]), h->stride,
        n, tneeded, zneeded,
        wr->ptr.p_double, wi->ptr.p_double,
        zneeded!=0 ? &(z->ptr.pp_double[0][0]) : NULL, zneeded!=0 ? z->stride : n,
        &locinfo);
    *info = locinfo;
    return r;
}

ae_bool _ialglib_i_rmatrixinternaltrevcmkl(
     const ae_matrix* t,
     ae_int_t n,
     ae_int_t side,
     ae_int_t howmny,
     ae_matrix* vl,
     ae_matrix* vr,
     ae_int_t* m,
     ae_int_t* info)
{
    ptrdiff_t loc_m, loc_info;
    char r;

    if( !_use_vendor_kernels )
        return ae_false;
    if( howmny==3 )
        return ae_false; /* not supported! */
    loc_m = 0;
    loc_info = 0;
    r = _alglib2mkl_rmatrixinternaltrevcmkl(
        &(t->ptr.pp_double[0][0]), t->stride,
        n, side, howmny,
        (side==2) || (side==3) ? &(vl->ptr.pp_double[0][0]) : NULL,
        (side==2) || (side==3) ? vl->stride : n,
        (side==1) || (side==3) ? &(vr->ptr.pp_double[0][0]) : NULL,
        (side==1) || (side==3) ? vr->stride : n,
        &loc_m,
        &loc_info);
    *m = loc_m;
    *info = loc_info;
    return r;
}

ae_bool _ialglib_i_smatrixtdevdmkl(
     ae_vector* d,
     ae_vector* e,
     ae_int_t n,
     ae_int_t zneeded,
     ae_matrix* z,
     ae_bool* evdresult)
{
    char fr, er;
    
    /* if vendor kernels are deactivated, skip */
    if( !_use_vendor_kernels )
        return ae_false;
    
    /* MKL version supports only zneeded=0,1,2; no need to try other values */
    if( zneeded<0 || zneeded>=3 )
        return ae_false;
        
    /*
     * try to use MKL; here we assume that
     * (a) zneeded=0,1,2 and
     * (b) either zneeded=0 and z is unallocated, or zneeded>0 and z is n*n matrix
     */
    er = 0;
    fr = _alglib2mkl_smatrixtdevdmkl(
        d->ptr.p_double,
        e->ptr.p_double,
        n,
        zneeded,
        zneeded>0 ? &(z->ptr.pp_double[0][0]) : NULL,
        zneeded>0 ? z->stride : n,
        &er);
    if( fr )
        *evdresult = er;
    return fr;
}

ae_bool _ialglib_i_sparsegemvcrsmkl(ae_int_t opa,
     ae_int_t arows,
     ae_int_t acols,
     double alpha,
     /* Real    */ const ae_vector* vals,
     /* Integer */ const ae_vector* cidx,
     /* Integer */ const ae_vector* ridx,
     /* Real    */ const ae_vector* x,
     ae_int_t ix,
     double beta,
     /* Real    */ ae_vector* y,
     ae_int_t iy)
{
    /* if vendor kernels are deactivated, skip */
    if( !_use_vendor_kernels )
        return ae_false;
    
    /* try to use MKL (it may return ae_false if compiled under LP64) */
    return _alglib2mkl_sparsegemvcrsmkl(opa, arows, acols,
        alpha,
        vals->ptr.p_double, cidx->ptr.p_int, ridx->ptr.p_int,
        x->ptr.p_double+ix,
        beta,
        y->ptr.p_double+iy);
}
#endif


/*
 * HPC cores with SSE2 support
 *
 * NOTE: this code is located in smp.c because it is NOT included in GPL
 *       version of ALGLIB.
 */
#ifdef ALGLIB_INTERCEPTS_SSE2
static double _safecrossentropy(double t,
     double z)
{
    double r;
    double result;


    if( ae_fp_eq(t,0) )
    {
        result = 0;
    }
    else
    {
        if( ae_fp_greater(fabs(z),1) )
        {
            
            /*
             * Shouldn't be the case with softmax,
             * but we just want to be sure.
             */
            if( ae_fp_eq(t/z,0) )
            {
                r = ae_minrealnumber;
            }
            else
            {
                r = t/z;
            }
        }
        else
        {
            
            /*
             * Normal case
             */
            if( ae_fp_eq(z,0)||ae_fp_greater_eq(fabs(t),ae_maxrealnumber*fabs(z)) )
            {
                r = ae_maxrealnumber;
            }
            else
            {
                r = t/z;
            }
        }
        result = t*log(r);
    }
    return result;
}

ae_bool _ialglib_i_hpcpreparechunkedgradientx(
    const ae_vector* weights,
    ae_int_t wcount,
    ae_vector* hpcbuf)
{
    ae_int_t i;
    double *srcptr;
    float  *dstptr;
    
    /*
     * This function converts weights from double  precision  to  single
     * precision format. Having weights directly in the single precision
     * allows us to perform batch gradient calculation more efficiently.
     * Weights are stored in the first half of hpcbuf.
     *
     * Second half is occupied by single-precision gradient, which is set
     * to zero.
     */
    ae_assert(wcount>=1, "hpcconvertmlpweights: wcount<0", NULL);
    if( hpcbuf->cnt<wcount )
    {
        ae_state _state1;
        ae_state_init(&_state1);
        ae_vector_set_length(hpcbuf, wcount, &_state1);
        ae_state_clear(&_state1);
    }
    srcptr = weights->ptr.p_double;
    dstptr = (float*)hpcbuf->ptr.p_ptr;
    for(i=0; i<wcount; i++)
        dstptr[i] = (float)srcptr[i];
    for(i=0; i<wcount; i++)
        dstptr[wcount+i] = 0.0;
    return ae_true;
}

ae_bool _ialglib_i_hpcfinalizechunkedgradientx(
    const ae_vector* hpcbuf,
    ae_int_t wcount,
    ae_vector* grad)
{
    ae_int_t i;
    float  *srcptr;
    double *dstptr;
    
    srcptr = (float*)hpcbuf->ptr.p_ptr;
    dstptr = grad->ptr.p_double;
    for(i=0; i<wcount; i++)
        dstptr[i] += srcptr[wcount+i];
    return ae_true;
}


ae_bool _ialglib_i_hpcchunkedgradient(/* Real    */ const ae_vector* _weights,
     /* Integer */ const ae_vector* _structinfo,
     /* Real    */ const ae_vector* _columnmeans,
     /* Real    */ const ae_vector* _columnsigmas,
     /* Real    */ const ae_matrix* xy,
     ae_int_t cstart,
     ae_int_t csize,
     /* Real    */ ae_vector* _batch4buf,
     /* Real    */ ae_vector* _hpcbuf,
     double* e,
     ae_bool naturalerrorfunc)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t kl;
    ae_int_t ntotal;
    ae_int_t nin;
    ae_int_t nout;
    ae_int_t offs;
    double f;
    double df;
    double v;
    double vv;
    double s;
    double fown;
    double deown;
    ae_bool bflag;
    ae_int_t istart;
    ae_int_t entryoffs;
    ae_int_t neuronidx;
    ae_int_t srcentryoffs;
    ae_int_t srcneuronidx;
    ae_int_t srcweightidx;
    ae_int_t neurontype;
    ae_int_t nweights;
    ae_int_t offs0;
    ae_int_t offs1;
    ae_int_t offs2;
    double v0;
    double v1;
    ae_int_t *structinfo;
    double *columnmeans, *columnsigmas;
    float *hpcweights;
    float *batch4buf;
    float *grad;
    ae_int_t fieldwidth = 4;
    ae_int_t chunksize = 4;

    const ae_int_t entrysize = 12;
    const ae_int_t dfoffs    = 4;
    const ae_int_t derroroffs= 8;
    /*
     * Check for SSE support
     */
    if( !(ae_cpuid()&CPU_SSE2) )
        return ae_false;

    /*
     * Other checks
     */
    ae_assert(csize<=4, "HPCChunkedGradient: internal error (CSize>ChunkSize)", NULL);
    
    /*
     * Prepare pointers
     */
    structinfo = _structinfo->ptr.p_int;
    batch4buf = (float*)_batch4buf->ptr.p_double;
    columnmeans = _columnmeans->ptr.p_double;
    columnsigmas = _columnsigmas->ptr.p_double;
    hpcweights = (float*)_hpcbuf->ptr.p_double;
    grad = ((float*)(_hpcbuf->ptr.p_double))+structinfo[4];
    
    /*
     * Read network geometry, prepare data
     */
    nin = structinfo[1];
    nout = structinfo[2];
    ntotal = structinfo[3];
    istart = structinfo[5];
    
    /*
     * Fill Batch4Buf by zeros.
     *
     * THIS STAGE IS VERY IMPORTANT!!!
     *
     * We fill all components of entry - neuron values, dF/dNET, dError/dF.
     * It allows us to easily handle  situations  when  CSize<ChunkSize  by
     * simply  working  with  ALL  components  of  Batch4Buf,  without ever
     * looking at CSize. The idea is that dError/dF for  absent  components
     * will be initialized by zeros - and won't be  rewritten  by  non-zero
     * values during backpropagation.
     */
    for(i=0; i<=entrysize*ntotal-1; i++)
        batch4buf[i] = 0;
    
    /*
     * Forward pass:
     * 1. Load data into Batch4Buf. If CSize<ChunkSize, data are padded by zeros.
     * 2. Perform forward pass through network
     */
    for(i=0; i<=nin-1; i++)
    {
        entryoffs = entrysize*i;
        for(j=0; j<=csize-1; j++)
        {
            if( columnsigmas[i]!=0 )
            {
                batch4buf[entryoffs+j] = (float)((xy->ptr.pp_double[cstart+j][i]-columnmeans[i])/columnsigmas[i]);
            }
            else
            {
                batch4buf[entryoffs+j] = (float)(xy->ptr.pp_double[cstart+j][i]-columnmeans[i]);
            }
        }
    }
    for(neuronidx=0; neuronidx<=ntotal-1; neuronidx++)
    {
        entryoffs = entrysize*neuronidx;
        offs = istart+neuronidx*fieldwidth;
        neurontype = structinfo[offs+0];
        if( neurontype>0 || neurontype==-5 )
        {
            /*
             * "activation function" neuron, which takes value of neuron SrcNeuronIdx
             * and applies activation function to it.
             *
             * This neuron has no weights and no tunable parameters.
             */
            srcneuronidx = structinfo[offs+2];
            srcentryoffs = entrysize*srcneuronidx;
            if( neurontype==-5 )
            {
                /*
                 * Linear activation function.
                 */
                _mm_store_ps(batch4buf+entryoffs,        _mm_load_ps(batch4buf+srcentryoffs));
                _mm_store_ps(batch4buf+entryoffs+dfoffs, _mm_set1_ps(1.0f));
                continue;
            }
            if( neurontype==1 )
            {
                /*
                 * tanh(NET)
                 *
                 * we use following formula for tanh:
                 *     tanh(NET)  = sign(NET)*(exp(2*|NET|)-1)/(exp(2*|NET|)+1)
                 *     exp(2*NET) = exp(2*NET/8)^8 = exp(NET/4)^8, for NET>=0
                 *     exp(NET/4) is approximated used Taylor series.
                 *
                 *     Taylor series approximation becomes increasingly inexact as
                 *     argument grows beyond NET/4 = 2.0, but luckily when this
                 *     inexact approximation is substituted into formula for tanh(),
                 *     we get almost exact answer (maximum error is 5E-6).
                 */
                __m128 mm_net, mm_absnet, mm_sgnnet, mm_1, mm_x, mm_s, mm_exp, mm_tanh, mm_dtanh;
                
                /* fetch data */
                mm_net = _mm_load_ps(batch4buf+srcentryoffs);
                
                /* reduce NET to [-15,+15] to prevent overflow */
                mm_net = _mm_max_ps(mm_net, _mm_set1_ps(-15.0f));
                mm_net = _mm_min_ps(mm_net, _mm_set1_ps(+15.0f));
                
                /* calculate |NET| = max(NET,-NET) */
                mm_absnet = _mm_max_ps(mm_net, _mm_sub_ps(_mm_set1_ps(0.0f),mm_net));
                
                /* calculate sign(NET) = NET / (|NET|+1.0E-30).
                   This formula is imprecise only for very small NET,
                   but we can tolerate such error because tanh() is almost zero for such values */
                mm_sgnnet = _mm_div_ps(mm_net, _mm_add_ps(mm_absnet,_mm_set1_ps(1.0E-30f)));
                
                /*
                 *     mm_x     = 2*|NET|/8
                 * exp(mm_x)    = 1+mm_x*(1+mm_x*(1/2+mm_x*(1/6+mm_x*(1/24+mm_x/120))))
                 * exp(2*|NET|) = exp(mm_x)^8
                 */
                mm_x = _mm_mul_ps(mm_absnet, _mm_set1_ps(0.25f));
                mm_s = _mm_add_ps(_mm_set1_ps(0.04166666666f), _mm_mul_ps(mm_x, _mm_set1_ps(0.008333333333f)));
                mm_s = _mm_add_ps(_mm_set1_ps(0.16666666666f), _mm_mul_ps(mm_x, mm_s));
                mm_s = _mm_add_ps(_mm_set1_ps(0.50000000000f), _mm_mul_ps(mm_x, mm_s));
                mm_s = _mm_add_ps(_mm_set1_ps(1.00000000000f), _mm_mul_ps(mm_x, mm_s));
                mm_s = _mm_add_ps(_mm_set1_ps(1.00000000000f), _mm_mul_ps(mm_x, mm_s));
                mm_exp = _mm_mul_ps(mm_s,mm_s);
                mm_exp = _mm_mul_ps(mm_exp,mm_exp);
                mm_exp = _mm_mul_ps(mm_exp,mm_exp);
                
                /*
                 * tanh() and its derivative.
                 * tanh(NET) = sign(NET)*(exp(2*|NET|)-1)/(exp(2*|NET|)+1),
                 */
                mm_1     = _mm_set1_ps(1.0f);
                mm_tanh  = _mm_div_ps(_mm_sub_ps(mm_exp, mm_1),_mm_add_ps(mm_exp, mm_1));
                mm_tanh  = _mm_mul_ps(mm_tanh, mm_sgnnet);
                mm_dtanh = _mm_sub_ps(mm_1, _mm_mul_ps(mm_tanh,mm_tanh));
                
                /* store results */
                _mm_store_ps(batch4buf+entryoffs, mm_tanh);
                _mm_store_ps(batch4buf+entryoffs+dfoffs, mm_dtanh);
                
                continue;
            }
            if( neurontype==2 )
            {
                /*
                 * exp(-NET^2)
                 */
                v = batch4buf[srcentryoffs+0];
                f = exp(-v*v);
                df = -2*v*f;
                batch4buf[entryoffs+0] = (float)f;
                batch4buf[entryoffs+0+dfoffs] = (float)df;
                v = batch4buf[srcentryoffs+1];
                f = exp(-v*v);
                df = -2*v*f;
                batch4buf[entryoffs+1] = (float)f;
                batch4buf[entryoffs+1+dfoffs] = (float)df;
                v = batch4buf[srcentryoffs+2];
                f = exp(-v*v);
                df = -2*v*f;
                batch4buf[entryoffs+2] = (float)f;
                batch4buf[entryoffs+2+dfoffs] = (float)df;
                v = batch4buf[srcentryoffs+3];
                f = exp(-v*v);
                df = -2*v*f;
                batch4buf[entryoffs+3] = (float)f;
                batch4buf[entryoffs+3+dfoffs] = (float)df;
                continue;
            }
            if( neurontype==3 )
            {
                /*
                 * EX() activation function, exponentially decays at negative axis,
                 * almost linearly grows at negative axis.
                 */
                v = batch4buf[srcentryoffs+0];
                if( v>=0 )
                {
                    double net2, arg, root, r;
                    net2 = v*v;
                    arg = net2+1;
                    root = sqrt(arg);
                    f = v+root;
                    r = v/root;
                    df = 1+r;
                }
                else
                {
                    f = exp(v);
                    df = f;
                }
                batch4buf[entryoffs+0] = (float)f;
                batch4buf[entryoffs+0+dfoffs] = (float)df;
                v = batch4buf[srcentryoffs+1];
                if( v>=0 )
                {
                    double net2, arg, root, r;
                    net2 = v*v;
                    arg = net2+1;
                    root = sqrt(arg);
                    f = v+root;
                    r = v/root;
                    df = 1+r;
                }
                else
                {
                    f = exp(v);
                    df = f;
                }
                batch4buf[entryoffs+1] = (float)f;
                batch4buf[entryoffs+1+dfoffs] = (float)df;
                v = batch4buf[srcentryoffs+2];
                if( v>=0 )
                {
                    double net2, arg, root, r;
                    net2 = v*v;
                    arg = net2+1;
                    root = sqrt(arg);
                    f = v+root;
                    r = v/root;
                    df = 1+r;
                }
                else
                {
                    f = exp(v);
                    df = f;
                }
                batch4buf[entryoffs+2] = (float)f;
                batch4buf[entryoffs+2+dfoffs] = (float)df;
                v = batch4buf[srcentryoffs+3];
                if( v>=0 )
                {
                    double net2, arg, root, r;
                    net2 = v*v;
                    arg = net2+1;
                    root = sqrt(arg);
                    f = v+root;
                    r = v/root;
                    df = 1+r;
                }
                else
                {
                    f = exp(v);
                    df = f;
                }
                batch4buf[entryoffs+3] = (float)f;
                batch4buf[entryoffs+3+dfoffs] = (float)df;
                continue;
            }
            
            /* unexpected neuron type, critical error */
            abort();
        }
        if( neurontype==0 )
        {
            __m128 _weight, _net, _in;
            __m128 _weight0, _net0, _in0;
            __m128 _weight1, _net1, _in1;
            float *_pw, *_pin;
            ae_int_t nw2;
            
            /*
             * "adaptive summator" neuron, whose output is a weighted sum of inputs.
             * It has weights, but has no activation function.
             */
            nweights = structinfo[offs+1];
            srcneuronidx = structinfo[offs+2];
            srcentryoffs = entrysize*srcneuronidx;
            srcweightidx = structinfo[offs+3];
            _pw  = hpcweights+srcweightidx;
            _pin = batch4buf+srcentryoffs;
            _net  = _mm_set1_ps(0.0);
            _net0 = _mm_set1_ps(0.0);
            _net1 = _mm_set1_ps(0.0);
            
            nw2 = nweights/2;
            for(j=0; j<nw2; j++)
            {
                _weight0 = _mm_load1_ps(_pw);
                _in0 = _mm_load_ps(_pin);
                _net0 = _mm_add_ps(_net0,_mm_mul_ps(_weight0,_in0));
                _weight1 = _mm_load1_ps(_pw+1);
                _in1 = _mm_load_ps(_pin+entrysize);
                _net1 = _mm_add_ps(_net1,_mm_mul_ps(_weight1,_in1));
                _pw+=2;
                _pin += 2*entrysize;
            }
            _net = _mm_add_ps(_net0,_net1);
            if( nweights%2 )
            {
                _weight = _mm_load1_ps(_pw);
                _pw++;
                _in = _mm_load_ps(_pin);
                _pin += entrysize;
                _net = _mm_add_ps(_net,_mm_mul_ps(_weight,_in));
            }
            _mm_store_ps(batch4buf+entryoffs,_net);
            batch4buf[entryoffs+0+dfoffs] = 1;
            batch4buf[entryoffs+1+dfoffs] = 1;
            batch4buf[entryoffs+2+dfoffs] = 1;
            batch4buf[entryoffs+3+dfoffs] = 1;
            continue;
        }
        if( neurontype<0 )
        {
            bflag = ae_false;
            if( neurontype==-2 )
            {
                
                /*
                 * Input neuron, left unchanged
                 */
                bflag = ae_true;
            }
            if( neurontype==-3 )
            {
                
                /*
                 * "-1" neuron
                 */
                batch4buf[entryoffs+0] = -1;
                batch4buf[entryoffs+1] = -1;
                batch4buf[entryoffs+2] = -1;
                batch4buf[entryoffs+3] = -1;
                batch4buf[entryoffs+0+dfoffs] = 0;
                batch4buf[entryoffs+1+dfoffs] = 0;
                batch4buf[entryoffs+2+dfoffs] = 0;
                batch4buf[entryoffs+3+dfoffs] = 0;
                bflag = ae_true;
            }
            if( neurontype==-4 )
            {
                
                /*
                 * "0" neuron
                 */
                batch4buf[entryoffs+0] = 0;
                batch4buf[entryoffs+1] = 0;
                batch4buf[entryoffs+2] = 0;
                batch4buf[entryoffs+3] = 0;
                batch4buf[entryoffs+0+dfoffs] = 0;
                batch4buf[entryoffs+1+dfoffs] = 0;
                batch4buf[entryoffs+2+dfoffs] = 0;
                batch4buf[entryoffs+3+dfoffs] = 0;
                bflag = ae_true;
            }
            ae_assert(bflag, "HPCChunkedGradient: internal error - unknown neuron type!", NULL);
            continue;
        }
    }
    
    /*
     * Intermediate phase between forward and backward passes.
     *
     * For regression networks:
     * * forward pass is completely done (no additional post-processing is
     *   needed).
     * * before starting backward pass, we have to  calculate  dError/dOut
     *   for output neurons. We also update error at this phase.
     *
     * For classification networks:
     * * in addition to forward pass we  apply  SOFTMAX  normalization  to
     *   output neurons.
     * * after applying normalization, we have to  calculate  dError/dOut,
     *   which is calculated in two steps:
     *   * first, we calculate derivative of error with respect to SOFTMAX
     *     normalized outputs (normalized dError)
     *   * then,  we calculate derivative of error with respect to  values
     *     of outputs BEFORE normalization was applied to them
     */
    ae_assert(structinfo[6]==0||structinfo[6]==1, "HPCChunkedGradient: unknown normalization type!", NULL);
    if( structinfo[6]==1 )
    {
        
        /*
         * SOFTMAX-normalized network.
         *
         * First, we have to  apply  SOFTMAX-normalization  to  last  NOut
         * neurons, because forward pass does NOT applies normalization.
         *
         * We do so as follows:
         * * we pass through last NOut entries and calculate max(entries[])
         * * we append to the end of batch4buf exp(entries-max(entries[]))
         *   - exponentials of output neurons pre-normalized  by  division
         *   by maximum exponent.
         * * we also store sum-of-all-prenormalized-exponents
         * * original neuron values are NOT changed by stis stage
         *
         */
        {
            __m128 mm_maxnet, mm_sumofexp;
            
            /* calculate componentwise maximum */
            entryoffs = entrysize*(ntotal-nout);
            mm_maxnet = _mm_load_ps(batch4buf+entryoffs);
            entryoffs = entryoffs+entrysize;
            for(i=1; i<=nout-1; i++)
            {
                mm_maxnet = _mm_max_ps(mm_maxnet,_mm_load_ps(batch4buf+entryoffs));
                entryoffs = entryoffs+entrysize;
            }
            
            /*
             * Calculate exponentials using Taylor series:
             * * we rely on fact that all exponentials are  evaluated  for
             *   negative values of argument due to pre-normalization.
             * * exp(x)  = 1/exp(-x) for x<0
             * * exp(-x) = exp(-x/8)^8
             * * exp(-x/8) is calculated using Taylor series
             * * error of such formula is less than 3E-6 for ALL negative x.
             *
             */
            entryoffs = entrysize*(ntotal-nout);
            offs0 = entrysize*ntotal;
            mm_sumofexp = _mm_set1_ps(0.0f);
            for(i=0; i<=nout-1; i++)
            {
                __m128 mm_arg, mm_x, mm_s, mm_exp;
                
                /* load MAXNET-NET */
                mm_arg = _mm_load_ps(batch4buf+entryoffs);
                mm_arg = _mm_sub_ps(mm_maxnet, mm_arg);
                
                /* reduce argument to [-20,+20] to prevent overflow */
                mm_arg = _mm_max_ps(mm_arg, _mm_set1_ps(-20.0f));
                mm_arg = _mm_min_ps(mm_arg, _mm_set1_ps(+20.0f));
                
                /* calculate exp(MAXNET-NET) */
                mm_x = _mm_mul_ps(mm_arg, _mm_set1_ps(0.125));
                mm_s = _mm_add_ps(_mm_set1_ps(0.04166666666f), _mm_mul_ps(mm_x, _mm_set1_ps(0.008333333333f)));
                mm_s = _mm_add_ps(_mm_set1_ps(0.16666666666f), _mm_mul_ps(mm_x, mm_s));
                mm_s = _mm_add_ps(_mm_set1_ps(0.50000000000f), _mm_mul_ps(mm_x, mm_s));
                mm_s = _mm_add_ps(_mm_set1_ps(1.00000000000f), _mm_mul_ps(mm_x, mm_s));
                mm_s = _mm_add_ps(_mm_set1_ps(1.00000000000f), _mm_mul_ps(mm_x, mm_s));
                mm_exp = _mm_mul_ps(mm_s,mm_s);
                mm_exp = _mm_mul_ps(mm_exp,mm_exp);
                mm_exp = _mm_mul_ps(mm_exp,mm_exp);
                
                /* replace mm_exp by exp(NET-MAXNET) = 1/exp(MAXNET-NET) */
                mm_exp = _mm_div_ps(_mm_set1_ps(1.0f), mm_exp);
                
                /* update sumofexp */
                mm_sumofexp = _mm_add_ps(mm_sumofexp, mm_exp);
                
                /* store */
                _mm_store_ps(batch4buf+offs0, mm_exp);
                
                /* update pointers */
                entryoffs = entryoffs+entrysize;
                offs0 = offs0+chunksize;
            }
            offs0 = entrysize*ntotal+2*nout*chunksize;
            _mm_store_ps(batch4buf+offs0, mm_sumofexp);
        }
        
        
        /*
         * Now we have:
         * * Batch4Buf[0...EntrySize*NTotal-1] stores NTotal entries, each
         *   of them containing:
         *   * ChunkSize neuron output values (SOFTMAX  normalization  was
         *     not applied to these values),
         *   * ChunkSize  values  of  dF/dNET (derivative of neuron output
         *     with respect to its input)
         *   * ChunkSize zeros in the elements which correspond to dError/dOut
         *     (derivative of error with respect to neuron output)
         * * Batch4Buf[EntrySize*NTotal...EntrySize*NTotal+ChunkSize*NOut-1] -
         *   stores exponentials of last NOut neurons.
         * * Batch4Buf[EntrySize*NTotal+ChunkSize*NOut-1...EntrySize*NTotal+ChunkSize*2*NOut-1]
         *   - can be used for temporary calculations
         * * Batch4Buf[EntrySize*NTotal+ChunkSize*2*NOut...EntrySize*NTotal+ChunkSize*2*NOut+ChunkSize-1]
         *   - stores sum-of-exponentials
         *
         * Block below calculates derivatives of error function with respect 
         * to non-SOFTMAX-normalized output values of last NOut neurons.
         *
         * It is quite complicated; we do not describe algebra behind it,
         * but if you want you may check it yourself :)
         */
        if( naturalerrorfunc )
        {
            
            /*
             * Calculate  derivative  of  error  with respect to values of
             * output  neurons  PRIOR TO SOFTMAX NORMALIZATION. Because we
             * use natural error function (cross-entropy), we  can  do  so
             * very easy.
             */
            offs0 = entrysize*ntotal+2*nout*chunksize;
            for(k=0; k<=csize-1; k++)
            {
                s = batch4buf[offs0+k];
                kl = (ae_int_t)(xy->ptr.pp_double[cstart+k][nin]+0.5);
                offs1 = (ntotal-nout)*entrysize+derroroffs+k;
                offs2 = entrysize*ntotal+k;
                for(i=0; i<=nout-1; i++)
                {
                    if( i==kl )
                    {
                        v = 1;
                    }
                    else
                    {
                        v = 0;
                    }
                    vv = batch4buf[offs2];
                    batch4buf[offs1] = (float)(vv/s-v);
                    *e = *e+_safecrossentropy(v, vv/s);
                    offs1 = offs1+entrysize;
                    offs2 = offs2+chunksize;
                }
            }
        }
        else
        {
            
            /*
             * SOFTMAX normalization makes things very difficult.
             * Sorry, we do not dare to describe this esoteric math
             * in details.
             */
            offs0 = entrysize*ntotal+chunksize*2*nout;
            for(k=0; k<=csize-1; k++)
            {
                s = batch4buf[offs0+k];
                kl = (ae_int_t)(xy->ptr.pp_double[cstart+k][nin]+0.5);
                vv = 0;
                offs1 = entrysize*ntotal+k;
                offs2 = entrysize*ntotal+nout*chunksize+k;
                for(i=0; i<=nout-1; i++)
                {
                    fown = batch4buf[offs1];
                    if( i==kl )
                    {
                        deown = fown/s-1;
                    }
                    else
                    {
                        deown = fown/s;
                    }
                    batch4buf[offs2] = (float)deown;
                    vv = vv+deown*fown;
                    *e = *e+deown*deown/2;
                    offs1 = offs1+chunksize;
                    offs2 = offs2+chunksize;
                }
                offs1 = entrysize*ntotal+k;
                offs2 = entrysize*ntotal+nout*chunksize+k;
                for(i=0; i<=nout-1; i++)
                {
                    fown = batch4buf[offs1];
                    deown = batch4buf[offs2];
                    batch4buf[(ntotal-nout+i)*entrysize+derroroffs+k] = (float)((-vv+deown*fown+deown*(s-fown))*fown/(s*s));
                    offs1 = offs1+chunksize;
                    offs2 = offs2+chunksize;
                }
            }
        }
    }
    else
    {
        
        /*
         * Regression network with sum-of-squares function.
         *
         * For each NOut of last neurons:
         * * calculate difference between actual and desired output
         * * calculate dError/dOut for this neuron (proportional to difference)
         * * store in in last 4 components of entry (these values are used
         *   to start backpropagation)
         * * update error
         */
        for(i=0; i<=nout-1; i++)
        {
            v0 = columnsigmas[nin+i];
            v1 = columnmeans[nin+i];
            entryoffs = entrysize*(ntotal-nout+i);
            offs0 = entryoffs;
            offs1 = entryoffs+derroroffs;
            for(j=0; j<=csize-1; j++)
            {
                v = batch4buf[offs0+j]*v0+v1-xy->ptr.pp_double[cstart+j][nin+i];
                batch4buf[offs1+j] = (float)(v*v0);
                *e = *e+v*v/2;
            }
        }
    }
    
    /*
     * Backpropagation
     */
    for(neuronidx=ntotal-1; neuronidx>=0; neuronidx--)
    {
        entryoffs = entrysize*neuronidx;
        offs = istart+neuronidx*fieldwidth;
        neurontype = structinfo[offs+0];
        if( neurontype>0||neurontype==-5 )
        {
            __m128 mm_df, mm_derror, mm_srcderror;
            float *p_srcderror;
            
            /*
             * Activation function
             */
            srcneuronidx = structinfo[offs+2];
            srcentryoffs = entrysize*srcneuronidx;
            p_srcderror  =  batch4buf+srcentryoffs+derroroffs;
            mm_df        = _mm_load_ps(batch4buf+entryoffs+dfoffs);
            mm_derror    = _mm_load_ps(batch4buf+entryoffs+derroroffs);
            mm_srcderror = _mm_load_ps(p_srcderror);
            _mm_store_ps(p_srcderror, _mm_add_ps(mm_srcderror,_mm_mul_ps(mm_df,mm_derror)));
            continue;
        }
        if( neurontype==0 )
        {
            __m128 _weight, _derror;
            float *_pw, *_pg, *_psrcderror, *_psrcval;
            
            /*
             * Adaptive summator
             */
            nweights = structinfo[offs+1];
            srcneuronidx = structinfo[offs+2];
            srcentryoffs = entrysize*srcneuronidx;
            srcweightidx = structinfo[offs+3];
            
            _derror = _mm_load_ps(batch4buf+entryoffs+derroroffs);
            _pw  = hpcweights+srcweightidx;
            _pg  = grad+srcweightidx;
            _psrcval    = batch4buf+srcentryoffs;
            _psrcderror = batch4buf+srcentryoffs+derroroffs;
            for(j=0; j<nweights; j++)
            {
                __m128 _dot0, _dot1, _grad;
                _weight = _mm_load1_ps(_pw);
                
                // calculate gradient
                _dot0 = _mm_load_ps(_psrcval);
                _dot1 = _derror;
                _dot0 = _mm_mul_ps(_dot0, _dot1);
                _dot1 = _mm_shuffle_ps(_dot0, _dot0, _MM_SHUFFLE(2, 3, 0, 1));
                _dot0 = _mm_add_ps(_dot0, _dot1);
                _dot1 = _mm_shuffle_ps(_dot0, _dot0, _MM_SHUFFLE(0, 1, 2, 3));
                _dot0 = _mm_add_ps(_dot0, _dot1);
                _grad = _mm_load_ss(_pg);
                _grad = _mm_add_ss(_grad, _dot0);
                _mm_store_ss(_pg, _grad);
                
                // update source neuron's dError/dOut
                _mm_store_ps(_psrcderror, _mm_add_ps(_mm_load_ps(_psrcderror),_mm_mul_ps(_weight,_derror)));
                
                // update pointers
                _pw++;
                _pg++;
                _psrcderror += entrysize;
                _psrcval += entrysize;
            }
            continue;
        }
        if( neurontype<0 )
        {
            bflag = ae_false;
            if( (neurontype==-2||neurontype==-3)||neurontype==-4 )
            {
                
                /*
                 * Special neuron type, no back-propagation required
                 */
                bflag = ae_true;
            }
            ae_assert(bflag, "MLPInternalCalculateGradient: unknown neuron type!", NULL);
            continue;
        }
    }

    return ae_true;
}


ae_bool _ialglib_i_hpcchunkedprocess(/* Real    */ const ae_vector* _weights,
     /* Integer */ const ae_vector* _structinfo,
     /* Real    */ const ae_vector* _columnmeans,
     /* Real    */ const ae_vector* _columnsigmas,
     /* Real    */ const ae_matrix* xy,
     ae_int_t cstart,
     ae_int_t csize,
     /* Real    */ ae_vector* _batch4buf,
     /* Real    */ ae_vector* _hpcbuf)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t ntotal;
    ae_int_t nin;
    ae_int_t nout;
    ae_int_t offs;
    double f;
    ae_bool bflag;
    ae_int_t istart;
    ae_int_t entryoffs;
    ae_int_t neuronidx;
    ae_int_t srcentryoffs;
    ae_int_t srcneuronidx;
    ae_int_t srcweightidx;
    ae_int_t neurontype;
    ae_int_t nweights;
    ae_int_t offs0;
    double v0;
    double v1;
    ae_int_t *structinfo;
    double *columnmeans, *columnsigmas;
    float *hpcweights;
    float *batch4buf;
    ae_int_t fieldwidth = 4;
    ae_int_t chunksize = 4;

    const ae_int_t entrysize = 4;
    /*
     * Check for SSE support
     */
    if( !(ae_cpuid()&CPU_SSE2) )
        return ae_false;
        
    /*
     * Other checks
     */
    ae_assert(csize<=4, "HPCChunkedGradient: internal error (CSize>ChunkSize)", NULL);
    
    /*
     * Prepare pointers
     */
    structinfo = _structinfo->ptr.p_int;
    batch4buf = (float*)_batch4buf->ptr.p_double;
    columnmeans = _columnmeans->ptr.p_double;
    columnsigmas = _columnsigmas->ptr.p_double;
    hpcweights = (float*)_hpcbuf->ptr.p_double;
    
    /*
     * Read network geometry, prepare data
     */
    nin = structinfo[1];
    nout = structinfo[2];
    ntotal = structinfo[3];
    istart = structinfo[5];
    
    /*
     * Fill Batch4Buf by zeros.
     *
     * THIS STAGE IS VERY IMPORTANT!!!
     *
     * We fill all components of entry - neuron values, dF/dNET, dError/dF.
     * It allows us to easily handle  situations  when  CSize<ChunkSize  by
     * simply  working  with  ALL  components  of  Batch4Buf,  without ever
     * looking at CSize.
     */
    for(i=0; i<=entrysize*ntotal-1; i++)
        batch4buf[i] = 0;
    
    /*
     * Forward pass:
     * 1. Load data into Batch4Buf. If CSize<ChunkSize, data are padded by zeros.
     * 2. Perform forward pass through network
     */
    for(i=0; i<=nin-1; i++)
    {
        entryoffs = entrysize*i;
        for(j=0; j<=csize-1; j++)
        {
            if( columnsigmas[i]!=0 )
            {
                batch4buf[entryoffs+j] = (float)((xy->ptr.pp_double[cstart+j][i]-columnmeans[i])/columnsigmas[i]);
            }
            else
            {
                batch4buf[entryoffs+j] = (float)(xy->ptr.pp_double[cstart+j][i]-columnmeans[i]);
            }
        }
    }
    for(neuronidx=0; neuronidx<=ntotal-1; neuronidx++)
    {
        entryoffs = entrysize*neuronidx;
        offs = istart+neuronidx*fieldwidth;
        neurontype = structinfo[offs+0];
        if( neurontype>0 || neurontype==-5 )
        {
            /*
             * "activation function" neuron, which takes value of neuron SrcNeuronIdx
             * and applies activation function to it.
             *
             * This neuron has no weights and no tunable parameters.
             */
            srcneuronidx = structinfo[offs+2];
            srcentryoffs = entrysize*srcneuronidx;
            if( neurontype==-5 )
            {
                /*
                 * Linear activation function.
                 */
                _mm_store_ps(batch4buf+entryoffs,        _mm_load_ps(batch4buf+srcentryoffs));
                continue;
            }
            if( neurontype==1 )
            {
                /*
                 * tanh(NET)
                 *
                 * we use following formula for tanh:
                 *     tanh(NET)  = sign(NET)*(exp(2*|NET|)-1)/(exp(2*|NET|)+1)
                 *     exp(2*NET) = exp(2*NET/8)^8 = exp(NET/4)^8, for NET>=0
                 *     exp(NET/4) is approximated used Taylor series.
                 *
                 *     Taylor series approximation becomes increasingly inexact as
                 *     argument grows beyond NET/4 = 2.0, but luckily when this
                 *     inexact approximation is substituted into formula for tanh(),
                 *     we get almost exact answer (maximum error is 5E-6).
                 */
                __m128 mm_net, mm_absnet, mm_sgnnet, mm_1, mm_x, mm_s, mm_exp, mm_tanh;
                
                /* fetch data */
                mm_net = _mm_load_ps(batch4buf+srcentryoffs);
                
                /* reduce NET to [-15,+15] to prevent overflow */
                mm_net = _mm_max_ps(mm_net, _mm_set1_ps(-15.0f));
                mm_net = _mm_min_ps(mm_net, _mm_set1_ps(+15.0f));
                
                /* calculate |NET| = max(NET,-NET) */
                mm_absnet = _mm_max_ps(mm_net, _mm_sub_ps(_mm_set1_ps(0.0f),mm_net));
                
                /* calculate sign(NET) = NET / (|NET|+1.0E-30).
                   This formula is imprecise only for very small NET,
                   but we can tolerate such error because tanh() is almost zero for such values */
                mm_sgnnet = _mm_div_ps(mm_net, _mm_add_ps(mm_absnet,_mm_set1_ps(1.0E-30f)));
                
                /*
                 *     mm_x     = 2*|NET|/8
                 * exp(mm_x)    = 1+mm_x*(1+mm_x*(1/2+mm_x*(1/6+mm_x*(1/24+mm_x/120))))
                 * exp(2*|NET|) = exp(mm_x)^8
                 */
                mm_x = _mm_mul_ps(mm_absnet, _mm_set1_ps(0.25f));
                mm_s = _mm_add_ps(_mm_set1_ps(0.04166666666f), _mm_mul_ps(mm_x, _mm_set1_ps(0.008333333333f)));
                mm_s = _mm_add_ps(_mm_set1_ps(0.16666666666f), _mm_mul_ps(mm_x, mm_s));
                mm_s = _mm_add_ps(_mm_set1_ps(0.50000000000f), _mm_mul_ps(mm_x, mm_s));
                mm_s = _mm_add_ps(_mm_set1_ps(1.00000000000f), _mm_mul_ps(mm_x, mm_s));
                mm_s = _mm_add_ps(_mm_set1_ps(1.00000000000f), _mm_mul_ps(mm_x, mm_s));
                mm_exp = _mm_mul_ps(mm_s,mm_s);
                mm_exp = _mm_mul_ps(mm_exp,mm_exp);
                mm_exp = _mm_mul_ps(mm_exp,mm_exp);
                
                /*
                 * tanh(NET) = sign(NET)*(exp(2*|NET|)-1)/(exp(2*|NET|)+1),
                 */
                mm_1     = _mm_set1_ps(1.0f);
                mm_tanh  = _mm_div_ps(_mm_sub_ps(mm_exp, mm_1),_mm_add_ps(mm_exp, mm_1));
                mm_tanh  = _mm_mul_ps(mm_tanh, mm_sgnnet);
                
                /* store result */
                _mm_store_ps(batch4buf+entryoffs, mm_tanh);
                
                continue;
            }
            if( neurontype==2 )
            {
                /*
                 * exp(-NET^2)
                 */
                double v;
                v = batch4buf[srcentryoffs+0];
                f = exp(-v*v);
                batch4buf[entryoffs+0] = (float)f;
                v = batch4buf[srcentryoffs+1];
                f = exp(-v*v);
                batch4buf[entryoffs+1] = (float)f;
                v = batch4buf[srcentryoffs+2];
                f = exp(-v*v);
                batch4buf[entryoffs+2] = (float)f;
                v = batch4buf[srcentryoffs+3];
                f = exp(-v*v);
                batch4buf[entryoffs+3] = (float)f;
                continue;
            }
            if( neurontype==3 )
            {
                /*
                 * EX() activation function, exponentially decays at negative axis,
                 * almost linearly grows at negative axis.
                 */
                double v;
                v = batch4buf[srcentryoffs+0];
                if( v>=0 )
                {
                    double net2, arg, root;
                    net2 = v*v;
                    arg = net2+1;
                    root = sqrt(arg);
                    f = v+root;
                }
                else
                {
                    f = exp(v);
                }
                batch4buf[entryoffs+0] = (float)f;
                v = batch4buf[srcentryoffs+1];
                if( v>=0 )
                {
                    double net2, arg, root;
                    net2 = v*v;
                    arg = net2+1;
                    root = sqrt(arg);
                    f = v+root;
                }
                else
                {
                    f = exp(v);
                }
                batch4buf[entryoffs+1] = (float)f;
                v = batch4buf[srcentryoffs+2];
                if( v>=0 )
                {
                    double net2, arg, root;
                    net2 = v*v;
                    arg = net2+1;
                    root = sqrt(arg);
                    f = v+root;
                }
                else
                {
                    f = exp(v);
                }
                batch4buf[entryoffs+2] = (float)f;
                v = batch4buf[srcentryoffs+3];
                if( v>=0 )
                {
                    double net2, arg, root;
                    net2 = v*v;
                    arg = net2+1;
                    root = sqrt(arg);
                    f = v+root;
                }
                else
                {
                    f = exp(v);
                }
                batch4buf[entryoffs+3] = (float)f;
                continue;
            }
            
            /* unexpected neuron type, critical error */
            abort();
        }
        if( neurontype==0 )
        {
            __m128 _weight, _net, _in;
            __m128 _weight0, _net0, _in0;
            __m128 _weight1, _net1, _in1;
            float *_pw, *_pin;
            ae_int_t nw2;
            
            /*
             * "adaptive summator" neuron, whose output is a weighted sum of inputs.
             * It has weights, but has no activation function.
             */
            nweights = structinfo[offs+1];
            srcneuronidx = structinfo[offs+2];
            srcentryoffs = entrysize*srcneuronidx;
            srcweightidx = structinfo[offs+3];
            _pw  = hpcweights+srcweightidx;
            _pin = batch4buf+srcentryoffs;
            _net  = _mm_set1_ps(0.0);
            _net0 = _mm_set1_ps(0.0);
            _net1 = _mm_set1_ps(0.0);
            
            nw2 = nweights/2;
            for(j=0; j<nw2; j++)
            {
                _weight0 = _mm_load1_ps(_pw);
                _in0 = _mm_load_ps(_pin);
                _net0 = _mm_add_ps(_net0,_mm_mul_ps(_weight0,_in0));
                _weight1 = _mm_load1_ps(_pw+1);
                _in1 = _mm_load_ps(_pin+entrysize);
                _net1 = _mm_add_ps(_net1,_mm_mul_ps(_weight1,_in1));
                _pw+=2;
                _pin += 2*entrysize;
            }
            _net = _mm_add_ps(_net0,_net1);
            if( nweights%2 )
            {
                _weight = _mm_load1_ps(_pw);
                _pw++;
                _in = _mm_load_ps(_pin);
                _pin += entrysize;
                _net = _mm_add_ps(_net,_mm_mul_ps(_weight,_in));
            }
            _mm_store_ps(batch4buf+entryoffs,_net);
            continue;
        }
        if( neurontype<0 )
        {
            bflag = ae_false;
            if( neurontype==-2 )
            {
                
                /*
                 * Input neuron, left unchanged
                 */
                bflag = ae_true;
            }
            if( neurontype==-3 )
            {
                
                /*
                 * "-1" neuron
                 */
                batch4buf[entryoffs+0] = -1;
                batch4buf[entryoffs+1] = -1;
                batch4buf[entryoffs+2] = -1;
                batch4buf[entryoffs+3] = -1;
                bflag = ae_true;
            }
            if( neurontype==-4 )
            {
                
                /*
                 * "0" neuron
                 */
                batch4buf[entryoffs+0] = 0;
                batch4buf[entryoffs+1] = 0;
                batch4buf[entryoffs+2] = 0;
                batch4buf[entryoffs+3] = 0;
                bflag = ae_true;
            }
            ae_assert(bflag, "HPCChunkedGradient: internal error - unknown neuron type!", NULL);
            continue;
        }
    }
    
    /*
     * Apply SOFTMAX or scaling to outputs
     */
    ae_assert(structinfo[6]==0||structinfo[6]==1, "HPCChunkedGradient: unknown normalization type!", NULL);
    if( structinfo[6]==1 )
    {
        float s[4];
        
        /*
         * SOFTMAX-normalized network.
         *
         * First, we have to  apply  SOFTMAX-normalization  to  last  NOut
         * neurons, because forward pass does NOT applies normalization.
         *
         * We do so as follows:
         * * we pass through last NOut entries and calculate max(entries[])
         * * we append to the end of batch4buf exp(entries-max(entries[]))
         *   - exponentials of output neurons pre-normalized  by  division
         *   by maximum exponent.
         * * we also store sum-of-all-prenormalized-exponents
         * * original neuron values are NOT changed by stis stage
         *
         */
        {
            __m128 mm_maxnet, mm_sumofexp;
            
            /* calculate componentwise maximum */
            entryoffs = entrysize*(ntotal-nout);
            mm_maxnet = _mm_load_ps(batch4buf+entryoffs);
            entryoffs = entryoffs+entrysize;
            for(i=1; i<=nout-1; i++)
            {
                mm_maxnet = _mm_max_ps(mm_maxnet,_mm_load_ps(batch4buf+entryoffs));
                entryoffs = entryoffs+entrysize;
            }
            
            /*
             * Calculate exponentials using Taylor series:
             * * we rely on fact that all exponentials are  evaluated  for
             *   negative values of argument due to pre-normalization.
             * * exp(x)  = 1/exp(-x) for x<0
             * * exp(-x) = exp(-x/8)^8
             * * exp(-x/8) is calculated using Taylor series
             * * error of such formula is less than 3E-6 for ALL negative x.
             *
             */
            entryoffs = entrysize*(ntotal-nout);
            offs0 = entrysize*ntotal;
            mm_sumofexp = _mm_set1_ps(0.0f);
            for(i=0; i<=nout-1; i++)
            {
                __m128 mm_arg, mm_x, mm_s, mm_exp;
                
                /* load MAXNET-NET */
                mm_arg = _mm_load_ps(batch4buf+entryoffs);
                mm_arg = _mm_sub_ps(mm_maxnet, mm_arg);
                
                /* reduce argument to [-15,+15] to prevent overflow */
                mm_arg = _mm_max_ps(mm_arg, _mm_set1_ps(-15.0f));
                mm_arg = _mm_min_ps(mm_arg, _mm_set1_ps(+15.0f));
                
                /* calculate exp(MAXNET-NET) */
                mm_x = _mm_mul_ps(mm_arg, _mm_set1_ps(0.125f));
                mm_s = _mm_add_ps(_mm_set1_ps(0.04166666666f), _mm_mul_ps(mm_x, _mm_set1_ps(0.008333333333f)));
                mm_s = _mm_add_ps(_mm_set1_ps(0.16666666666f), _mm_mul_ps(mm_x, mm_s));
                mm_s = _mm_add_ps(_mm_set1_ps(0.50000000000f), _mm_mul_ps(mm_x, mm_s));
                mm_s = _mm_add_ps(_mm_set1_ps(1.00000000000f), _mm_mul_ps(mm_x, mm_s));
                mm_s = _mm_add_ps(_mm_set1_ps(1.00000000000f), _mm_mul_ps(mm_x, mm_s));
                mm_exp = _mm_mul_ps(mm_s,mm_s);
                mm_exp = _mm_mul_ps(mm_exp,mm_exp);
                mm_exp = _mm_mul_ps(mm_exp,mm_exp);
                
                /* replace mm_exp by exp(NET-MAXNET) = 1/exp(MAXNET-NET) */
                mm_exp = _mm_div_ps(_mm_set1_ps(1.0f), mm_exp);
                
                /* update sumofexp */
                mm_sumofexp = _mm_add_ps(mm_sumofexp, mm_exp);
                
                /* store */
                _mm_store_ps(batch4buf+offs0, mm_exp);
                
                /* update pointers */
                entryoffs = entryoffs+entrysize;
                offs0 = offs0+chunksize;
            }
            offs0 = entrysize*ntotal+2*nout*chunksize;
            _mm_storeu_ps(&s[0], mm_sumofexp);
        }
        
        /*
         * Write SOFTMAX-normalized values to the output array.
         */
        offs0 = entrysize*ntotal;
        for(i=0; i<=nout-1; i++)
        {
            if( csize>0 )
                xy->ptr.pp_double[cstart+0][nin+i] = batch4buf[offs0+0]/s[0];
            if( csize>1 )
                xy->ptr.pp_double[cstart+1][nin+i] = batch4buf[offs0+1]/s[1];
            if( csize>2 )
                xy->ptr.pp_double[cstart+2][nin+i] = batch4buf[offs0+2]/s[2];
            if( csize>3 )
                xy->ptr.pp_double[cstart+3][nin+i] = batch4buf[offs0+3]/s[3];
            offs0 = offs0+chunksize;
        }
    }
    else
    {
        
        /*
         * Regression network
         */
        for(i=0; i<=nout-1; i++)
        {
            v0 = columnsigmas[nin+i];
            v1 = columnmeans[nin+i];
            entryoffs = entrysize*(ntotal-nout+i);
            for(j=0; j<=csize-1; j++)
            {
                xy->ptr.pp_double[cstart+j][nin+i] = batch4buf[entryoffs+j]*v0+v1;
            }
        }
    }
    
    return ae_true;
}
#endif
/*$ End $*/
