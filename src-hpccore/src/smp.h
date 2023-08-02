###########################################################################
# ALGLIB 4.00.0 (source code generated 2023-05-21)
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
#ifndef _SMP_H
#define _SMP_H

/* include files */
#include "aenv.h"

/*$ Includes $*/

/*$ Body $*/
#define AE_HPC
#ifdef AE_HAS_SSE2_INTRINSICS
#define ALGLIB_INTERCEPTS_SSE2
#endif
#ifdef AE_MKL
#define ALGLIB_INTERCEPTS_MKL
#endif
#define AE_SMP_MAXPARAMS 32

#if (AE_OS!=AE_UNKNOWN) && (AE_THREADING==AE_PARALLEL)
#define _ALGLIB_HAS_WORKSTEALING
#endif

#if !defined(_ALGLIB_HAS_WORKSTEALING)
/*************************************************************************
*                                                                        *
*                                                                        *
*    OS-agnostic stub for multithreading framework                       *
*                                                                        *
*                                                                        *
*************************************************************************/
typedef struct ae_task_info
{
    ae_int_t dummy;
} ae_task_info;

typedef struct ae_task_group
{
    ae_int_t dummy;
} ae_task_group;

ae_int_t ae_cores_count();
void ae_set_cores_to_use(ae_int_t ncores);
ae_int_t ae_get_cores_to_use();
ae_bool ae_multithreading_enabled(ae_state *_state);
void ae_set_smp_support(
    ae_task_group **_child_tasks,
    ae_bool *_smp_enabled,
    ae_bool new_support_status,
    ae_state *_state);
void ae_sync(
    ae_task_group *_child_tasks,
    ae_bool smp_enabled,
    ae_bool dispose_group,
    ae_state *_state);
void ae_wait_for_group(
    ae_task_group *group,
    ae_bool dispose_on_success,
    ae_state *_state);
void ae_free_disposed_items();
void ae_complete_finalization_before_exit();

#else /* _ALGLIB_HAS_WORKSTEALING */
/*************************************************************************
*                                                                        *
*                                                                        *
*    OS-aware work-stealing multithreading framework                     *
*                                                                        *
*                                                                        *
*************************************************************************/

/*************************************************************************
Event.

This structure provides OS-independent event:
* under Windows/Posix systems it uses system-provided event
* under Boost it uses OS-independent event provided by Boost package
* when no OS is defined, it is just a stub, "fake event":
  a) "fake event" can be in signalled or non-signalled mode
  b) "fake event" can be used only from one thread - one which created event
  c) when thread waits for signalling event, it immediately returns
  d) when thread waits for non-singalling event, program is terminated
     (because no one else can set event to signalling state)
     
     
IMPORTANT: event working in an auto reset mode does not  support  multiple
           threads  waiting  for  it  to  signal . It is reset after first
           thread is released.
*************************************************************************/
typedef struct
{
    /*
     * Pointer to _event structure. This pointer has type void* in order to
     * make header file OS-independent (event declaration depends on OS).
     */
    void *ptr;
} ae_event;

/*************************************************************************
Structure which is used to store parameters of the task.
*************************************************************************/
typedef struct
{
    union
    {
        ae_complex  cval;
        double      dval;
        ae_int_t    ival;
        ae_bool     bval;
        void        *val;
        const void  *const_val;
    }
    value;
} ae_task_parameter;


/*************************************************************************
Task data: set of task parameters and pointer to task function.
*************************************************************************/
typedef struct ae_task_data
{
    /*
     * Task parameters
     */
    ae_task_parameter parameters[AE_SMP_MAXPARAMS];

    /*
     * Pointer on a function which is called from the worker thread.
     */
    void (*func)(struct ae_task_data*, ae_state*);
    
    /*
     * Flags inherited from the task creator
     */
    ae_uint64_t flags;
} ae_task_data;


/*************************************************************************
Task information: task data and synchronization structures
*************************************************************************/
typedef struct ae_task_info
{
	/*
     * DESCRIPTION: auto reset event which is set when root task is solved.
     *              non-signalling by default.
     */
	ae_event done_event;

	/*
     * DESCRIPTION: task data, undefined by default.
     * PROTECTION:  no lock-based protection. However, it is assumed that
     *              a) before push_task() only one thread works with data
     *              b) push_task() generates memory barrier
     *              c) after pop_task() or steal_task() only one thread
     *                 works with data
     *              These assumptions should give enough of protection.
     */
	ae_task_data data;

	/*
     * DESCRIPTION: parent task group. can be null. null by default.
     *              when isn't null, task completion is signalled by:
     *              * acquiring parent_group.group_lock
     *              * decreasing waiting_count (and setting exception if needed)
     *              * in case new value of waiting_count is not 0 OR wake_up_worker_on_completion
     *                is false, we release group_lock and exit.
     *              * otherwise, we wake up sleeping worker and
     *                give him our queue by:
     *                * updating worker_idx
     *                * releasing group_lock
     *                * clearing wake_up_worker_on_completion field
     *                * signalling parent_worker.wakeup_event
     *                Finally, we dispose themseles (dispose_thread()
     *                is called and we wait for the wakeup_event)
     *              This field is NULL by default.
     *
     * PROTECTION:  same as for task_info.data
     */
	struct ae_task_group * volatile parent_group;

	/*
     * DESCRIPTION: a list of child task groups owned by this task.
     *              Groups are linked using ae_task_group->next_group field.
     *              This field is NULL on initial creation.
     *              Every time we create group it is added to this list.
     *              Every time we dispose group, it is removed from the list.
     *
     * PROTECTION:  no protection. Only one worker thread works with this list.
     */
	struct ae_task_group * volatile child_groups;

	/*
     * DESCRIPTION: set to non-null value when task (or one of its childs)
     *              spawned exception during execution.
     *              null by default.
     * PROTECTION:  no protection. This field is modified in the context
     *              of the thread which works with task.
     */
    const char * exception;

	/*
     * DESCRIPTION: used to organize child tasks into linked list
     *              (or to organize disposed tasks into list).
     * PROTECTION:  not needed (only parent worker references it).
     */
	struct ae_task_info * volatile next_task;
} ae_task_info;


/*************************************************************************
Group of child tasks
*************************************************************************/
typedef struct ae_task_group
{
    /*
     * DESCRIPTION: parent thread which spawned this specific group of tasks.
     *              can NOT be null.
     * PROTECTION:  not needed. This field is modified only by
     *              create_task_group() method which generates appropriate barriers
     *              so its value is visible to all other threads. Other methods
     *              and/or threads can read this value, but can not modify it.
     */
    struct ae_worker_thread * volatile parent_worker;

    /*
     * DESCRIPTION: number of child tasks waiting for processing (i.e. tasks which
     *              were attached to group and push'ed to queue). This field is
     *              incremented by push_task(), decremented by solve_task().
     * PROTECTION:  by group_lock.
     */
    volatile ae_int_t waiting_count;
    
    /*
     * DESCRIPTION: worker thread sets this field to true when it wants to be
     *              waked up on completion of this group. This field is set to
     *              false by thread which wakes up worker.
     * PROTECTION:  by group_lock.
     */
    volatile ae_bool wake_up_worker_on_completion;

    /*
     * DESCRIPTION: lock which protects group
     * PROTECTION:  by itself
     */
    ae_lock group_lock;

    /*
     * DESCRIPTION: list of child tasks, updated by push_task().
     *              contains tasks which were not processed immediately
     *              after push_task() in the context of the worker thread.
     * PROTECTION:  not needed (only parent_worker works with list)
     */
    ae_task_info * volatile childs;

	/*
     * DESCRIPTION: set to non-NULL value when child task raised exception 
     *              during execution. NULL by default.
     * PROTECTION:  before group is completed - protected by group_lock.
     *              after completion - it is possible to access this field without locking.
     */
    const char * exception;

    /*
     * DESCRIPTION: this field is used to organize groups in a linked list:
     *              either list of child groups of the worker thread or
     *              list of disposed groups. null by default.
     * PROTECTION:  not needed (only parent worker can modify this list).
     */
    struct ae_task_group * volatile next_group;
} ae_task_group;


/*************************************************************************
Worker thread
*************************************************************************/
typedef struct ae_worker_thread
{
	/*
     * Auto-reset event which is set to signalled state when all child 
     * are done (thread, other than the worker thread, which decreases
     * waiting_count down to zero, sets this event).
     *
     * When this event signalled, all tasks were processed (solved
     * or failed), waiting_count is zero, failed_count contains
     * number of failed tasks.
     */
	ae_event wakeup_event;

	/*
     * DESCRIPTION: thread handle (pointer to ae_thread_handle structure),
     *              set during initial creation, remains unmodified through
     *              all the lifetime.
     * PROTECTION:  not needed, other threads do not reference it.
     */
	void *thread_handle;

	/*
     * DESCRIPTION: worker_idx - index of the worker thread, from 1 to  queue_count-1.
     *              This index is set by create_worker(). It can be modified by another
     *              worker thread which wakes up current worker.
     *
     *              It is responsibility of the party who changed worker_idx to call
     *              ae_pin_thread()/ae_unpin_thread() in order to change information
     *              about ownership. The only exception from this rule is initial
     *              creation, when ownership on the queue is acquired in the worker_loop()
     *              function.
     *
     * PROTECTION:  protected, but has no dedicated lock. Protection protocol
     *              is described below:
     *
     *              1. modification of worker_idx means that thread is assigned
     *                 to some queue or reassigned to new queue
     *
     *              2. worker_idx is modified only when:
     *                 a) thread is created
     *                 b) thread starts to wait for the unfinished child group
     *                     (worker_idx is set to -1)
     *                 c) other thread which finished child group awoke thread
     *                 d) thread is disposed
     *                 e) disposed thread is reused
     *
     *              3. in case (a) worker_idx is set before thread is created,
     *                 protection is provided by implicit barriers which OS
     *                 generates during thread creation
     *
     *              4. in cases (b) and (c) modification of worker_idx is protected
     *                 by group_lock of the group being waited for:
     *                 * in case (b) thread modifies its own worker_idx (sets to -1)
     *                   after acquiring group_lock of group G1 (group it waits for).
     *                 * in case (c) another thread mofifies worker_idx of the thread
     *                   after acquiring group_lock of G1.
     *                 * in both cases worker_idx is modified simultaneously with
     *                   wake_up_worker_on_completion flag
     *                 In these cases protection is provided by the fact that only
     *                 one group can have wake_up_worker_on_completion flag set to
     *                 TRUE, only one thread can finish this group, and all operations
     *                 with this flag and worker_idx are performed within group_lock.
     *
     *              5. in cases (d) and (e) it is guaranteed that all child groups were
     *                 finished prior to disposing/waking up thread. So, no one can
     *                 modify worker_idx in attempt to wakeup thread.
     *                 In these cases protection is provided by thread's wakeup_event:
     *                 * in case (d) worker_idx is set to -1 before waiting for event
     *                 * in case (e) worker_idx is modified before thread wakes up.
     *
     */
	volatile ae_int_t worker_idx;

	/*
     * DESCRIPTION: this field is used to organize disposed threads into linked list.
     * PROTECTION:  not needed (only dispose_thread can modify this list).
     */
	struct ae_worker_thread *next_thread;
} ae_worker_thread;

ae_int_t ae_cores_count();
void ae_set_cores_to_use(ae_int_t ncores);
ae_int_t ae_get_cores_to_use();
void ae_wait_for_event(ae_event *event);

ae_bool ae_can_pexec(ae_state *_state);

ae_task_info* ae_create_task(ae_task_group *parent_group, ae_state *_state);
void ae_dispose_task(ae_task_info *task);
void ae_destroy_and_free_task(ae_task_info *task);

ae_task_group* ae_create_task_group(ae_state *_state);
void ae_dispose_group(ae_task_group *group);
void ae_destroy_and_free_group(ae_task_group *group);

void ae_push_root_task(ae_task_info *task);
void ae_push_task(ae_task_info *task, ae_state *_state, ae_bool execute_immediately);

void ae_set_smp_support(
    ae_task_group **_child_tasks,
    ae_bool *_smp_enabled,
    ae_bool new_support_status,
    ae_state *_state);
void ae_sync(
    ae_task_group *_child_tasks,
    ae_bool smp_enabled,
    ae_bool dispose_group,
    ae_state *_state);
void ae_wait_for_group(
    ae_task_group *group,
    ae_bool dispose_on_success,
    ae_state *_state);

void ae_free_disposed_items();
void ae_complete_finalization_before_exit();

ae_bool ae_smpselftests();
#endif /* _ALGLIB_HAS_WORKSTEALING */
    
#ifdef AE_SMP_DEBUGCOUNTERS
extern volatile ae_int64_t _ae_dbg_threads_spawned;
extern volatile ae_int64_t _ae_dbg_tasks_created;
extern volatile ae_int64_t _ae_dbg_tasks_stolen;
extern volatile ae_int64_t _ae_dbg_groups_created;
#endif

/*
 * MKL links
 *
 * NOTE: this code is located in smp.c because it is NOT included in HPC
 *       version of ALGLIB.
 */
ae_bool    ae_mkl_init(ae_int64_t flags);
ae_bool    ae_mkl_disable_fast_mm();
ae_int64_t ae_mkl_memstat();
#ifdef ALGLIB_INTERCEPTS_MKL
ae_int_t _ialglib_i_matrixtilesizeb();
ae_bool _ialglib_i_rmatrixtrsvmkl(ae_int_t n,
     const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     ae_vector* x,
     ae_int_t ix);
ae_bool _ialglib_i_rmatrixgermkl(ae_int_t m,
     ae_int_t n,
     ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     double alpha,
     const ae_vector* u,
     ae_int_t iu,
     const ae_vector* v,
     ae_int_t iv);
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
     ae_int_t iy);
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
     ae_int_t iy);
ae_bool _ialglib_i_cmatrixrank1mkl(ae_int_t m,
     ae_int_t n,
     ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     const ae_vector* u,
     ae_int_t iu,
     const ae_vector* v,
     ae_int_t iv);
ae_bool _ialglib_i_rmatrixrank1mkl(ae_int_t m,
     ae_int_t n,
     ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     const ae_vector* u,
     ae_int_t iu,
     const ae_vector* v,
     ae_int_t iv);
ae_bool _ialglib_i_cmatrixmvmkl(ae_int_t m,
     ae_int_t n,
     const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t opa,
     const ae_vector* x,
     ae_int_t ix,
     ae_vector* y,
     ae_int_t iy);
ae_bool _ialglib_i_rmatrixmvmkl(ae_int_t m,
     ae_int_t n,
     const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t opa,
     const ae_vector* x,
     ae_int_t ix,
     ae_vector* y,
     ae_int_t iy);
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
     ae_bool isupper);
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
     ae_bool isupper);
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
     ae_int_t jc);
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
     ae_int_t jc);
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
     ae_int_t j2);
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
     ae_int_t j2);
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
     ae_int_t j2);
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
     ae_int_t j2);
ae_bool _ialglib_i_spdmatrixcholeskymkl(
     ae_matrix* a,
     ae_int_t offs,
     ae_int_t n,
     ae_bool isupper,
     ae_bool* cholresult);
ae_bool _ialglib_i_rmatrixplumkl(
     ae_matrix* a,
     ae_int_t offs,
     ae_int_t m,
     ae_int_t n,
     ae_vector* pivots);
ae_bool _ialglib_i_rmatrixbdmkl(
     ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     ae_vector* d,
     ae_vector* e,
     ae_vector* tauq,
     ae_vector* taup);
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
     ae_bool dotranspose);
ae_bool _ialglib_i_rmatrixhessenbergmkl(
     ae_matrix* a,
     ae_int_t n,
     ae_vector* tau);
ae_bool _ialglib_i_rmatrixhessenbergunpackqmkl(
     const ae_matrix* a,
     ae_int_t n,
     const ae_vector* tau,
     ae_matrix* q);
ae_bool _ialglib_i_smatrixtdmkl(
     ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     ae_vector* tau,
     ae_vector* d,
     ae_vector* e);
ae_bool _ialglib_i_smatrixtdunpackqmkl(
     const ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     const ae_vector* tau,
     ae_matrix* q);
ae_bool _ialglib_i_hmatrixtdmkl(
     ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     ae_vector* tau,
     ae_vector* d,
     ae_vector* e);
ae_bool _ialglib_i_hmatrixtdunpackqmkl(
     const ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     const ae_vector* tau,
     ae_matrix* q);
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
     ae_bool* svdresult);
ae_bool _ialglib_i_rmatrixinternalschurdecompositionmkl(
     ae_matrix* h,
     ae_int_t n,
     ae_int_t tneeded,
     ae_int_t zneeded,
     ae_vector* wr,
     ae_vector* wi,
     ae_matrix* z,
     ae_int_t* info);
ae_bool _ialglib_i_rmatrixinternaltrevcmkl(
     const ae_matrix* t,
     ae_int_t n,
     ae_int_t side,
     ae_int_t howmny,
     ae_matrix* vl,
     ae_matrix* vr,
     ae_int_t* m,
     ae_int_t* info);
ae_bool _ialglib_i_smatrixtdevdmkl(
     ae_vector* d,
     ae_vector* e,
     ae_int_t n,
     ae_int_t zneeded,
     ae_matrix* z,
     ae_bool* evdresult);
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
     ae_int_t iy);
#endif

/*
 * HPC cores with SSE2 support
 */
#ifdef ALGLIB_INTERCEPTS_SSE2
ae_bool _ialglib_i_hpcpreparechunkedgradientx(
    /* Real    */  const ae_vector* weights,
                   ae_int_t wcount,
     /* Real    */ ae_vector* hpcbuf);
ae_bool _ialglib_i_hpcfinalizechunkedgradientx(
    /* Real    */  const ae_vector* hpcbuf,
                   ae_int_t wcount,
     /* Real    */ ae_vector* grad);
ae_bool _ialglib_i_hpcchunkedgradient(/* Real    */ const ae_vector* weights,
     /* Integer */ const ae_vector* _structinfo,
     /* Real    */ const ae_vector* _columnmeans,
     /* Real    */ const ae_vector* _columnsigmas,
     /* Real    */ const ae_matrix* xy,
     ae_int_t cstart,
     ae_int_t csize,
     /* Real    */ ae_vector* _batch4buf,
     /* Real    */ ae_vector* _hpcbuf,
     double* e,
     ae_bool naturalerrorfunc);
ae_bool _ialglib_i_hpcchunkedprocess(/* Real    */ const ae_vector* weights,
     /* Integer */ const ae_vector* _structinfo,
     /* Real    */ const ae_vector* _columnmeans,
     /* Real    */ const ae_vector* _columnsigmas,
     /* Real    */ const ae_matrix* xy,
     ae_int_t cstart,
     ae_int_t csize,
     /* Real    */ ae_vector* _batch4buf,
     /* Real    */ ae_vector* _hpcbuf);
#endif

/*$ End $*/
#endif