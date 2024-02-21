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


#include <stdafx.h>
#include <stdio.h>
#include "spchol.h"


/*$ Declarations $*/
static ae_int_t spchol_maxsupernode = 4;
static double spchol_maxmergeinefficiency = 0.25;
static ae_int_t spchol_smallfakestolerance = 2;
static ae_int_t spchol_maxfastkernel = 4;
static ae_bool spchol_relaxedsupernodes = ae_true;
static ae_int_t spchol_updatesheadersize = 2;
static ae_int_t spchol_groupheadersize = 2;
static ae_int_t spchol_batchheadersize = 2;
static ae_int_t spchol_sequenceentrysize = 3;
static ae_int_t spchol_smallupdate = 128;
static double spchol_raw2sthreshold = 0.01;
#ifdef ALGLIB_NO_FAST_KERNELS
static ae_int_t spchol_spsymmgetmaxsimd(ae_state *_state);
#endif
static void spchol_spsymmfactorizeblockrec(spcholanalysis* analysis,
     /* Integer */ ae_vector* curladjrowbegin,
     ae_int_t blkoffs,
     ae_bool isrootcall,
     sboolean* failureflag,
     ae_state *_state);
void _spawn_spchol_spsymmfactorizeblockrec(spcholanalysis* analysis,
    /* Integer */ ae_vector* curladjrowbegin,
    ae_int_t blkoffs,
    ae_bool isrootcall,
    sboolean* failureflag, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_spchol_spsymmfactorizeblockrec(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_spchol_spsymmfactorizeblockrec(spcholanalysis* analysis,
    /* Integer */ ae_vector* curladjrowbegin,
    ae_int_t blkoffs,
    ae_bool isrootcall,
    sboolean* failureflag, ae_state *_state);
static void spchol_spsymmprocessupdatesgroup(spcholanalysis* analysis,
     ae_int_t blkoffs,
     sboolean* failureflag,
     ae_state *_state);
void _spawn_spchol_spsymmprocessupdatesgroup(spcholanalysis* analysis,
    ae_int_t blkoffs,
    sboolean* failureflag, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_spchol_spsymmprocessupdatesgroup(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_spchol_spsymmprocessupdatesgroup(spcholanalysis* analysis,
    ae_int_t blkoffs,
    sboolean* failureflag, ae_state *_state);
static void spchol_spsymmprocessupdatesbatch(spcholanalysis* analysis,
     ae_int_t blkoffs,
     sboolean* failureflag,
     ae_state *_state);
void _spawn_spchol_spsymmprocessupdatesbatch(spcholanalysis* analysis,
    ae_int_t blkoffs,
    sboolean* failureflag, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_spchol_spsymmprocessupdatesbatch(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_spchol_spsymmprocessupdatesbatch(spcholanalysis* analysis,
    ae_int_t blkoffs,
    sboolean* failureflag, ae_state *_state);
static void spchol_printblockedeliminationtreerec(const spcholanalysis* analysis,
     ae_int_t blkoffs,
     ae_int_t depth,
     ae_state *_state);
#ifdef ALGLIB_NO_FAST_KERNELS
static void spchol_propagatefwd(/* Real    */ const ae_vector* x,
     ae_int_t cols0,
     ae_int_t blocksize,
     /* Integer */ const ae_vector* superrowidx,
     ae_int_t rbase,
     ae_int_t offdiagsize,
     /* Real    */ const ae_vector* rowstorage,
     ae_int_t offss,
     ae_int_t sstride,
     /* Real    */ ae_vector* simdbuf,
     ae_int_t simdwidth,
     ae_state *_state);
#endif
static void spchol_generatedbgpermutation(const sparsematrix* a,
     ae_int_t n,
     /* Integer */ ae_vector* perm,
     /* Integer */ ae_vector* invperm,
     ae_state *_state);
static void spchol_buildunorderedetree(const sparsematrix* a,
     ae_int_t n,
     /* Integer */ ae_vector* parent,
     /* Integer */ ae_vector* tabove,
     ae_state *_state);
static void spchol_fromparenttochildren(/* Integer */ const ae_vector* parent,
     ae_int_t n,
     /* Integer */ ae_vector* childrenr,
     /* Integer */ ae_vector* childreni,
     /* Integer */ ae_vector* ttmp0,
     ae_state *_state);
static void spchol_buildorderedetree(const sparsematrix* a,
     ae_int_t n,
     /* Integer */ ae_vector* parent,
     /* Integer */ ae_vector* supernodalpermutation,
     /* Integer */ ae_vector* invsupernodalpermutation,
     /* Integer */ ae_vector* trawparentofrawnode,
     /* Integer */ ae_vector* trawparentofreorderednode,
     /* Integer */ ae_vector* ttmp,
     /* Boolean */ ae_vector* tflagarray,
     ae_state *_state);
static void spchol_createsupernodalstructure(const sparsematrix* at,
     /* Integer */ const ae_vector* parent,
     ae_int_t n,
     spcholanalysis* analysis,
     /* Integer */ ae_vector* node2supernode,
     /* Integer */ ae_vector* tchildrenr,
     /* Integer */ ae_vector* tchildreni,
     /* Integer */ ae_vector* tparentnodeofsupernode,
     /* Integer */ ae_vector* tfakenonzeros,
     /* Integer */ ae_vector* ttmp0,
     /* Boolean */ ae_vector* tflagarray,
     ae_state *_state);
static void spchol_analyzesupernodaldependencies(spcholanalysis* analysis,
     const sparsematrix* rawa,
     /* Integer */ const ae_vector* node2supernode,
     ae_int_t n,
     /* Integer */ ae_vector* ttmp0,
     /* Integer */ ae_vector* ttmp1,
     /* Boolean */ ae_vector* tflagarray,
     ae_state *_state);
static void spchol_createblockstructure(spcholanalysis* analysis,
     ae_state *_state);
static void spchol_processbatchofheadsrec(spcholanalysis* analysis,
     /* Integer */ ae_vector* headsstack,
     ae_int_t stackbase,
     ae_int_t nheads,
     /* Integer */ ae_vector* blkstruct,
     ae_int_t* offs,
     double* totalflops,
     double* sequentialflops,
     ae_state *_state);
static void spchol_scheduleupdatesforablockrec(const spcholanalysis* analysis,
     /* Integer */ ae_vector* rowbegin,
     /* Boolean */ ae_vector* isfactorized,
     /* Real    */ ae_vector* nflop,
     /* Integer */ ae_vector* blkstruct,
     ae_int_t blockitemsoffs,
     ae_int_t blocksize,
     ae_int_t depth,
     ae_int_t* offs,
     ae_int_t* groupscreated,
     double* totalflops,
     double* sequentialflops,
     ae_state *_state);
static void spchol_loadmatrix(spcholanalysis* analysis,
     const sparsematrix* at,
     ae_state *_state);
static void spchol_extractmatrix(const spcholanalysis* analysis,
     /* Integer */ const ae_vector* offsets,
     /* Integer */ const ae_vector* strides,
     /* Real    */ const ae_vector* rowstorage,
     /* Real    */ const ae_vector* diagd,
     ae_int_t n,
     sparsematrix* a,
     /* Real    */ ae_vector* d,
     /* Integer */ ae_vector* p,
     /* Integer */ ae_vector* tmpp,
     ae_state *_state);
static void spchol_partialcholeskypattern(const sparsematrix* a,
     ae_int_t head,
     ae_int_t tail,
     sparsematrix* atail,
     nipool* n1ipool,
     nbpool* n1bpool,
     sparsematrix* tmpbottomt,
     sparsematrix* tmpupdatet,
     sparsematrix* tmpupdate,
     sparsematrix* tmpnewtailt,
     ae_state *_state);
static void spchol_topologicalpermutation(const sparsematrix* a,
     /* Integer */ const ae_vector* p,
     sparsematrix* b,
     ae_state *_state);
static ae_int_t spchol_computenonzeropattern(const sparsematrix* wrkat,
     ae_int_t columnidx,
     ae_int_t n,
     /* Integer */ const ae_vector* superrowridx,
     /* Integer */ ae_vector* superrowidx,
     ae_int_t nsuper,
     /* Integer */ const ae_vector* childrennodesr,
     /* Integer */ const ae_vector* childrennodesi,
     /* Integer */ const ae_vector* node2supernode,
     /* Boolean */ ae_vector* truearray,
     /* Integer */ ae_vector* tmp0,
     ae_state *_state);
static void spchol_updatesupernode(spcholanalysis* analysis,
     ae_int_t sidx,
     ae_int_t cols0,
     ae_int_t cols1,
     ae_int_t offss,
     /* Integer */ const ae_vector* raw2smap,
     ae_int_t ladjidx0,
     ae_int_t ladjidx1,
     /* Real    */ const ae_vector* diagd,
     ae_state *_state);
static void spchol_updatesupernodegeneric(spcholanalysis* analysis,
     ae_int_t sidx,
     ae_int_t cols0,
     ae_int_t cols1,
     ae_int_t offss,
     /* Integer */ const ae_vector* raw2smap,
     ae_int_t ladjidx,
     /* Real    */ const ae_vector* diagd,
     ae_state *_state);
static ae_bool spchol_factorizesupernode(spcholanalysis* analysis,
     ae_int_t sidx,
     ae_state *_state);
static ae_int_t spchol_recommendedstridefor(ae_int_t rowsize,
     ae_state *_state);
static ae_int_t spchol_alignpositioninarray(ae_int_t offs,
     ae_state *_state);
#ifdef ALGLIB_NO_FAST_KERNELS
static ae_bool spchol_updatekernel4444(/* Real    */ ae_vector* rowstorage,
     ae_int_t offss,
     ae_int_t sheight,
     ae_int_t offsu,
     ae_int_t uheight,
     /* Real    */ const ae_vector* diagd,
     ae_int_t offsd,
     /* Integer */ const ae_vector* raw2smap,
     /* Integer */ const ae_vector* superrowidx,
     ae_int_t urbase,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
static ae_bool spchol_updatekernelabc4(/* Real    */ ae_vector* rowstorage,
     ae_int_t offss,
     ae_int_t twidth,
     ae_int_t offsu,
     ae_int_t uheight,
     ae_int_t urank,
     ae_int_t urowstride,
     ae_int_t uwidth,
     /* Real    */ const ae_vector* diagd,
     ae_int_t offsd,
     /* Integer */ const ae_vector* raw2smap,
     /* Integer */ const ae_vector* superrowidx,
     ae_int_t urbase,
     ae_state *_state);
#endif
static ae_bool spchol_updatekernelrank1(/* Real    */ ae_vector* rowstorage,
     ae_int_t offss,
     ae_int_t twidth,
     ae_int_t trowstride,
     ae_int_t offsu,
     ae_int_t uheight,
     ae_int_t uwidth,
     /* Real    */ const ae_vector* diagd,
     ae_int_t offsd,
     /* Integer */ const ae_vector* raw2smap,
     /* Integer */ const ae_vector* superrowidx,
     ae_int_t urbase,
     ae_state *_state);
static ae_bool spchol_updatekernelrank2(/* Real    */ ae_vector* rowstorage,
     ae_int_t offss,
     ae_int_t twidth,
     ae_int_t trowstride,
     ae_int_t offsu,
     ae_int_t uheight,
     ae_int_t uwidth,
     /* Real    */ const ae_vector* diagd,
     ae_int_t offsd,
     /* Integer */ const ae_vector* raw2smap,
     /* Integer */ const ae_vector* superrowidx,
     ae_int_t urbase,
     ae_state *_state);
static void spchol_generatepriorityamdpermutation(sparsematrix* wrka,
     /* Integer */ ae_vector* wrkpriorities,
     double promoteabove,
     ae_int_t promoteto,
     ae_bool debugordering,
     ae_bool dotrace,
     nbpool* n1bpool,
     nipool* n1ipool,
     priorityamdbuffers* buf,
     ae_bool userbuffers,
     /* Integer */ ae_vector* fillinperm,
     /* Integer */ ae_vector* invfillinperm,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
Informational function, useful for debugging
*************************************************************************/
ae_int_t spsymmgetmaxfastkernel(ae_state *_state)
{
    ae_int_t result;


    result = spchol_maxfastkernel;
    return result;
}


/*************************************************************************
Symbolic phase of Cholesky decomposition.

Performs preliminary analysis of Cholesky/LDLT factorization.  The  latter
is computed with strictly diagonal D (no Bunch-Kauffman pivoting).

The analysis object produced by this function will be used later to  guide
actual decomposition.

Depending on settings specified during factorization, may produce  vanilla
Cholesky or L*D*LT  decomposition  (with  strictly  diagonal  D),  without
permutation or with permutation P (being either  topological  ordering  or
sparsity preserving ordering).

Thus, A is represented as either L*LT or L*D*LT or P*L*LT*PT or P*L*D*LT*PT.

NOTE: L*D*LT family of factorization may be used to  factorize  indefinite
      matrices. However, numerical stability is guaranteed ONLY for a class
      of quasi-definite matrices.

INPUT PARAMETERS:
    A           -   sparse square matrix in CRS format, with LOWER triangle
                    being used to store the matrix.
    Priorities  -   array[N], optional priorities:
                    * ignored for PermType<>3 and PermType<>-3
                      (not referenced at all)
                    * for   PermType=3  or  PermType=-3  this  array  stores
                      nonnegative  column  elimination  priorities.  Columns
                      with  lower  priorities are eliminated first. At least
                      max(Priorities[])+1  internal  AMD  rounds   will   be
                      performed, so avoid specifying too large values here.
                      Ideally, 0<=Priorities[I]<5.
    PromoteAbove-   columns with degrees higher than PromoteAbove*max(MEAN(Degree),1)
                    may be promoted to the next priority group. Ignored  for
                    PermType<>3 and PermType<>-3.
                    This parameter can be used to make priorities  a  hard
                    requirement, a non-binding  suggestion,  or  something
                    in-between:
                    * big PromoteAbove (N or more) effectively means that
                      priorities are hard
                    * values between 2 and 10 are usually a good choice  for
                      soft priorities
                    * zero value means that appropriate  value  for  a  soft
                      priority (between 2 and 5) is automatically chosen.
                      Specific value may change in future ALGLIB versions.
    PromoteTo   -   controls column promotion:
                    * columns which will be postponed due to being too dense
                      will be promoted to the priority group #PromoteTo
                      instead of the next group.
                    * Ignored for PermType<>3 and PermType<>-3.
                    * If column already belongs to a priority group #PromoteTo
                      or higher, it will be promoted to the next priority group.
                    * Can be zero (means default way of promoting columns).
                    * Avoid specifying too large values (above 10) because
                      algorithm will perform at least (PromoteTo+1) elimination rounds.
    FactType    -   factorization type:
                    * 0 for traditional Cholesky
                    * 1 for LDLT decomposition with strictly diagonal D
    PermType    -   permutation type:
                    *-3 for debug improved AMD which debugs AMD itself, parallel
                        block supernodal code and advanced memory management:
                        * AMD is debugged by generating a sequence of decreasing
                          tail sizes, ~logN in total, even if ordering can be
                          done with just one round of AMD. This ordering is
                          used to test correctness of multiple AMD rounds.
                        * parallel block supernodal code is debugged by
                          partitioning problems into smallest possible chunks,
                          ignoring thresholds set by SMPActivationLevel()
                          and SpawnLevel().
                        * memory management is debugged by randomly switching
                          MemReuse between +1 and -1, ignoring its original value
                    *-2 for column count ordering (NOT RECOMMENDED!)
                    *-1 for absence of permutation
                    * 0 for best permutation available
                    * 1 for supernodal ordering (improves locality and
                      performance, but does NOT change fill-in pattern)
                    * 2 for supernodal AMD ordering (improves fill-in)
                    * 3 for  improved  AMD  (approximate  minimum  degree)
                        ordering with better  handling  of  matrices  with
                        dense rows/columns and ability to perform priority
                        ordering
    MemReuse    -   the memory management strategy:
                    * +1 means that the internally allocated memory is reused
                         as much as possible. What was once allocated is not
                         freed as long as SPCholAnalysis structure is alive.
                         Ideal for many small and medium-sized repeated
                         factorization problems.
                    * -1 means that some potentially large memory blocks
                         are freed as soon as they are not needed. Whilst
                         some limited amount of dynamically allocated memory
                         is still reused, the largest block are not.
                         Ideal for large-scale problems that occupy almost
                         all available RAM.
    Analysis    -   can be uninitialized instance, or previous analysis
                    results. Previously allocated memory is reused as much
                    as possible.
    Buf         -   buffer; may be completely uninitialized, or one remained
                    from previous calls (including ones with completely
                    different matrices). Previously allocated temporary
                    space will be reused.

OUTPUT PARAMETERS:
    Analysis    -   symbolic analysis of the matrix structure  which  will
                    be used later to guide  numerical  factorization.  The
                    numerical values are stored internally in the structure,
                    but you have to  run  factorization  phase  explicitly
                    with SPSymmFactorize().  You  can  also reload another
                    matrix with same sparsity pattern with  SPSymmReload()
                    or rewrite its diagonal with SPSymmReloadDiagonal().
    
This function fails if and only if the matrix A is symbolically degenerate
i.e. has diagonal element which is exactly zero. In  such  case  False  is
returned.

NOTE: defining 'SCHOLESKY' trace tag will activate tracing.
      defining 'SCHOLESKY.SS' trace tag will activate detailed tracing  of
      the supernodal structure.

NOTE: defining 'DEBUG.SLOW' trace tag will  activate  extra-slow  (roughly
      N^3 ops) integrity checks, in addition to cheap O(1) ones.

  -- ALGLIB routine --
     20.09.2020
     Bochkanov Sergey
*************************************************************************/
ae_bool spsymmanalyze(const sparsematrix* a,
     /* Integer */ const ae_vector* priorities,
     double promoteabove,
     ae_int_t promoteto,
     ae_int_t facttype,
     ae_int_t permtype,
     ae_int_t memreuse,
     spcholanalysis* analysis,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    ae_int_t jj;
    ae_int_t k;
    ae_bool permready;
    ae_bool result;


    ae_assert(ae_isfinite(promoteabove, _state)&&ae_fp_greater_eq(promoteabove,(double)(0)), "SPSymmAnalyze: PromoteAbove is negative or infinite", _state);
    ae_assert(promoteto>=0, "SPSymmAnalyze: PromoteTo is negative", _state);
    ae_assert(sparseiscrs(a, _state), "SPSymmAnalyze: A is not stored in CRS format", _state);
    ae_assert(sparsegetnrows(a, _state)==sparsegetncols(a, _state), "SPSymmAnalyze: non-square A", _state);
    ae_assert(facttype==0||facttype==1, "SPSymmAnalyze: unexpected FactType", _state);
    ae_assert((((((permtype==0||permtype==1)||permtype==2)||permtype==3)||permtype==-1)||permtype==-2)||permtype==-3, "SPSymmAnalyze: unexpected PermType", _state);
    ae_assert(memreuse==-1||memreuse==1, "SPSymmAnalyze: unexpected MemType", _state);
    ae_assert((permtype!=3&&permtype!=-3)||(ae_isfinite(promoteabove, _state)&&ae_fp_greater_eq(promoteabove,(double)(0))), "SPSymmAnalyze: unexpected PromoteAbove - infinite or negative", _state);
    result = ae_true;
    n = sparsegetnrows(a, _state);
    if( permtype==-3||permtype==3 )
    {
        ae_assert(priorities->cnt>=n, "SPSymmAnalyze: length(Priorities)<N", _state);
        icopyallocv(n, priorities, &analysis->curpriorities, _state);
    }
    if( permtype==0 )
    {
        isetallocv(n, 0, &analysis->curpriorities, _state);
        permtype = 3;
        promoteabove = 0.0;
        promoteto = 0;
    }
    analysis->tasktype = 0;
    analysis->n = n;
    analysis->unitd = facttype==0;
    analysis->permtype = permtype;
    analysis->debugblocksupernodal = permtype==-3;
    analysis->extendeddebug = ae_is_trace_enabled("DEBUG.SLOW")&&n<=100;
    analysis->dotrace = ae_is_trace_enabled("SCHOLESKY");
    analysis->dotracescheduler = analysis->dotrace&&ae_is_trace_enabled("SCHOLESKY.SCHEDULER");
    analysis->dotracesupernodalstructure = analysis->dotrace&&ae_is_trace_enabled("SCHOLESKY.SS");
    analysis->istopologicalordering = permtype==-1||permtype==1;
    analysis->applypermutationtooutput = permtype==-1;
    analysis->modtype = 0;
    analysis->modparam0 = 0.0;
    analysis->modparam1 = 0.0;
    analysis->modparam2 = 0.0;
    analysis->modparam3 = 0.0;
    analysis->useparallelism = ae_false;
    
    /*
     * Allocate temporaries
     */
    ivectorsetlengthatleast(&analysis->tmpparent, n+1, _state);
    ivectorsetlengthatleast(&analysis->tmp0, n+1, _state);
    ivectorsetlengthatleast(&analysis->tmp1, n+1, _state);
    ivectorsetlengthatleast(&analysis->tmp2, n+1, _state);
    ivectorsetlengthatleast(&analysis->tmp3, n+1, _state);
    ivectorsetlengthatleast(&analysis->tmp4, n+1, _state);
    bvectorsetlengthatleast(&analysis->flagarray, n+1, _state);
    nbpoolinit(&analysis->n1booleanpool, n+1, _state);
    nipoolinit(&analysis->n1integerpool, n+1, _state);
    nrpoolinit(&analysis->nrealpool, n, _state);
    
    /*
     * Initial trace message
     */
    if( analysis->dotrace )
    {
        ae_trace("\n\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        ae_trace("//  SPARSE CHOLESKY ANALYSIS STARTED                                                              //\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        
        /*
         * Nonzeros count of the original matrix
         */
        k = 0;
        for(i=0; i<=n-1; i++)
        {
            k = k+(a->didx.ptr.p_int[i]-a->ridx.ptr.p_int[i])+1;
        }
        ae_trace("NZ(A) = %0d\n",
            (int)(k));
        
        /*
         * Analyze row statistics
         */
        ae_trace("=== ANALYZING ROW STATISTICS =======================================================================\n");
        ae_trace("row size is:\n");
        isetv(n, 1, &analysis->tmp0, _state);
        for(i=0; i<=n-1; i++)
        {
            for(jj=a->ridx.ptr.p_int[i]; jj<=a->didx.ptr.p_int[i]-1; jj++)
            {
                j = a->idx.ptr.p_int[jj];
                analysis->tmp0.ptr.p_int[i] = analysis->tmp0.ptr.p_int[i]+1;
                analysis->tmp0.ptr.p_int[j] = analysis->tmp0.ptr.p_int[j]+1;
            }
        }
        k = 1;
        while(k<=n)
        {
            j = 0;
            for(i=0; i<=n-1; i++)
            {
                if( analysis->tmp0.ptr.p_int[i]>=k&&analysis->tmp0.ptr.p_int[i]<2*k )
                {
                    j = j+1;
                }
            }
            ae_trace("* [%6d..%6d) elements: %6d rows\n",
                (int)(k),
                (int)(2*k),
                (int)(j));
            k = k*2;
        }
    }
    
    /*
     * Initial integrity check - diagonal MUST be symbolically nonzero
     */
    for(i=0; i<=n-1; i++)
    {
        if( a->didx.ptr.p_int[i]==a->uidx.ptr.p_int[i] )
        {
            if( analysis->dotrace )
            {
                ae_trace("> the matrix diagonal is symbolically zero, stopping");
            }
            result = ae_false;
            return result;
        }
    }
    
    /*
     * What type of permutation do we have?
     */
    if( analysis->istopologicalordering )
    {
        ae_assert(permtype==-1||permtype==1, "SPSymmAnalyze: integrity check failed (ihebd)", _state);
        
        /*
         * Build topologically ordered elimination tree
         */
        spchol_buildorderedetree(a, n, &analysis->tmpparent, &analysis->superperm, &analysis->invsuperperm, &analysis->tmp0, &analysis->tmp1, &analysis->tmp2, &analysis->flagarray, _state);
        ivectorsetlengthatleast(&analysis->fillinperm, n, _state);
        ivectorsetlengthatleast(&analysis->invfillinperm, n, _state);
        ivectorsetlengthatleast(&analysis->effectiveperm, n, _state);
        ivectorsetlengthatleast(&analysis->inveffectiveperm, n, _state);
        for(i=0; i<=n-1; i++)
        {
            analysis->fillinperm.ptr.p_int[i] = i;
            analysis->invfillinperm.ptr.p_int[i] = i;
            analysis->effectiveperm.ptr.p_int[i] = analysis->superperm.ptr.p_int[i];
            analysis->inveffectiveperm.ptr.p_int[i] = analysis->invsuperperm.ptr.p_int[i];
        }
        
        /*
         * Reorder input matrix
         */
        spchol_topologicalpermutation(a, &analysis->superperm, &analysis->tmpat, _state);
        
        /*
         * Analyze etree, build supernodal structure
         */
        spchol_createsupernodalstructure(&analysis->tmpat, &analysis->tmpparent, n, analysis, &analysis->node2supernode, &analysis->tmp0, &analysis->tmp1, &analysis->tmp2, &analysis->tmp3, &analysis->tmp4, &analysis->flagarray, _state);
        
        /*
         * Having fully initialized supernodal structure, analyze dependencies
         */
        spchol_analyzesupernodaldependencies(analysis, a, &analysis->node2supernode, n, &analysis->tmp0, &analysis->tmp1, &analysis->flagarray, _state);
    }
    else
    {
        
        /*
         * Generate fill-in reducing permutation
         */
        permready = ae_false;
        if( permtype==-2 )
        {
            spchol_generatedbgpermutation(a, n, &analysis->fillinperm, &analysis->invfillinperm, _state);
            permready = ae_true;
        }
        if( permtype==2 )
        {
            generateamdpermutation(a, n, &analysis->fillinperm, &analysis->invfillinperm, &analysis->amdtmp, _state);
            permready = ae_true;
        }
        if( permtype==3||permtype==-3 )
        {
            sparsecopybuf(a, &analysis->tmpa, _state);
            spchol_generatepriorityamdpermutation(&analysis->tmpa, &analysis->curpriorities, promoteabove, promoteto, permtype==-3&&ae_randominteger(100, _state)>50, analysis->dotrace, &analysis->n1booleanpool, &analysis->n1integerpool, &analysis->pamdtmp, memreuse>0, &analysis->fillinperm, &analysis->invfillinperm, _state);
            permready = ae_true;
        }
        ae_assert(permready, "SPSymmAnalyze: integrity check failed (pp4td)", _state);
        
        /*
         * Apply permutation to the matrix, perform analysis on the initially reordered matrix
         * (we may need one more reordering, now topological one, due to supernodal analysis).
         * Build topologically ordered elimination tree
         */
        sparsesymmpermtblbuf(a, ae_false, &analysis->fillinperm, &analysis->tmpa, _state);
        spchol_buildorderedetree(&analysis->tmpa, n, &analysis->tmpparent, &analysis->superperm, &analysis->invsuperperm, &analysis->tmp0, &analysis->tmp1, &analysis->tmp2, &analysis->flagarray, _state);
        ivectorsetlengthatleast(&analysis->effectiveperm, n, _state);
        ivectorsetlengthatleast(&analysis->inveffectiveperm, n, _state);
        for(i=0; i<=n-1; i++)
        {
            analysis->effectiveperm.ptr.p_int[i] = analysis->superperm.ptr.p_int[analysis->fillinperm.ptr.p_int[i]];
            analysis->inveffectiveperm.ptr.p_int[analysis->effectiveperm.ptr.p_int[i]] = i;
        }
        
        /*
         * Reorder input matrix
         */
        spchol_topologicalpermutation(&analysis->tmpa, &analysis->superperm, &analysis->tmpat, _state);
        
        /*
         * Analyze etree, build supernodal structure
         */
        spchol_createsupernodalstructure(&analysis->tmpat, &analysis->tmpparent, n, analysis, &analysis->node2supernode, &analysis->tmp0, &analysis->tmp1, &analysis->tmp2, &analysis->tmp3, &analysis->tmp4, &analysis->flagarray, _state);
        
        /*
         * Having fully initialized supernodal structure, analyze dependencies
         */
        spchol_analyzesupernodaldependencies(analysis, &analysis->tmpa, &analysis->node2supernode, n, &analysis->tmp0, &analysis->tmp1, &analysis->flagarray, _state);
    }
    
    /*
     * Prepare block structure
     */
    spchol_createblockstructure(analysis, _state);
    
    /*
     * Save information for integrity checks
     */
    icopyallocv(n+1, &analysis->tmpat.ridx, &analysis->referenceridx, _state);
    
    /*
     * Load matrix into the supernodal storage
     */
    spchol_loadmatrix(analysis, &analysis->tmpat, _state);
    return result;
}


/*************************************************************************
Sets modified Cholesky type

INPUT PARAMETERS:
    Analysis    -   symbolic analysis of the matrix structure
    ModStrategy -   modification type:
                    * 0 for traditional Cholesky/LDLT (Cholesky fails when
                      encounters nonpositive pivot, LDLT fails  when  zero
                      pivot   is  encountered,  no  stability  checks  for
                      overflows/underflows)
                    * 1 for modified Cholesky with additional checks:
                      * pivots less than ModParam0 are increased; (similar
                        procedure with proper generalization is applied to
                        LDLT)
                      * if,  at  some  moment,  sum  of absolute values of
                        elements in column  J  will  become  greater  than
                        ModParam1, Cholesky/LDLT will treat it as  failure
                        and will stop immediately
                      * if ModParam0 is zero, no pivot modification is applied
                      * if ModParam1 is zero, no overflow check is performed
                    * 2 for modified Cholesky/LDLT which handles pivots
                      smaller than ModParam0 in the following way:
                      * a diagonal element is set to a very large value
                      * offdiagonal elements are zeroed
    P0, P1, P2,P3 - modification parameters #0 #1, #2 and #3.
                    Params #2 and #3 are ignored in current version.

OUTPUT PARAMETERS:
    Analysis    -   symbolic analysis of the matrix structure, new strategy
                    (results will be seen with next SPSymmFactorize() call)

  -- ALGLIB routine --
     20.09.2020
     Bochkanov Sergey
*************************************************************************/
void spsymmsetmodificationstrategy(spcholanalysis* analysis,
     ae_int_t modstrategy,
     double p0,
     double p1,
     double p2,
     double p3,
     ae_state *_state)
{


    ae_assert((modstrategy==0||modstrategy==1)||modstrategy==2, "SPSymmSetModificationStrategy: unexpected ModStrategy", _state);
    ae_assert(ae_isfinite(p0, _state)&&ae_fp_greater_eq(p0,(double)(0)), "SPSymmSetModificationStrategy: bad P0", _state);
    ae_assert(ae_isfinite(p1, _state), "SPSymmSetModificationStrategy: bad P1", _state);
    ae_assert(ae_isfinite(p2, _state), "SPSymmSetModificationStrategy: bad P2", _state);
    ae_assert(ae_isfinite(p3, _state), "SPSymmSetModificationStrategy: bad P3", _state);
    analysis->modtype = modstrategy;
    analysis->modparam0 = p0;
    analysis->modparam1 = p1;
    analysis->modparam2 = p2;
    analysis->modparam3 = p3;
}


/*************************************************************************
Updates symmetric  matrix  internally  stored  in  previously  initialized
Analysis object.

You can use this function to perform  multiple  factorizations  with  same
sparsity patterns: perform symbolic analysis  once  with  SPSymmAnalyze(),
then update internal matrix with SPSymmReload() and call SPSymmFactorize().

INPUT PARAMETERS:
    Analysis    -   symbolic analysis of the matrix structure
    A           -   sparse square matrix in CRS format with LOWER triangle
                    being used to store the matrix. The matrix  MUST  have
                    sparsity   pattern   exactly   same  as  one  used  to
                    initialize the Analysis object.
                    The algorithm will fail in  an  unpredictable  way  if
                    something different was passed.

OUTPUT PARAMETERS:
    Analysis    -   symbolic analysis of the matrix structure  which  will
                    be used later to guide  numerical  factorization.  The
                    numerical values are stored internally in the structure,
                    but you have to  run  factorization  phase  explicitly
                    with SPSymmAnalyze().  You  can  also  reload  another
                    matrix with same sparsity pattern with SPSymmReload().

  -- ALGLIB routine --
     20.09.2020
     Bochkanov Sergey
*************************************************************************/
void spsymmreload(spcholanalysis* analysis,
     const sparsematrix* a,
     ae_state *_state)
{


    ae_assert(sparseiscrs(a, _state), "SPSymmReload: A is not stored in CRS format", _state);
    ae_assert(sparsegetnrows(a, _state)==sparsegetncols(a, _state), "SPSymmReload: non-square A", _state);
    if( analysis->istopologicalordering )
    {
        
        /*
         * Topological (fill-in preserving) ordering is used, we can copy
         * A directly into WrkAT using joint permute+transpose
         */
        spchol_topologicalpermutation(a, &analysis->effectiveperm, &analysis->tmpat, _state);
        spchol_loadmatrix(analysis, &analysis->tmpat, _state);
    }
    else
    {
        
        /*
         * Non-topological permutation; first we perform generic symmetric
         * permutation, then transpose result
         */
        sparsesymmpermtblbuf(a, ae_false, &analysis->effectiveperm, &analysis->tmpa, _state);
        sparsecopytransposecrsbuf(&analysis->tmpa, &analysis->tmpat, _state);
        spchol_loadmatrix(analysis, &analysis->tmpat, _state);
    }
}


/*************************************************************************
Updates  diagonal  of  the  symmetric  matrix  internally  stored  in  the
previously initialized Analysis object.

When only diagonal of the  matrix  has  changed,  this  function  is  more
efficient than SPSymmReload() that has to perform  costly  permutation  of
the entire matrix.

You can use this function to perform  multiple  factorizations  with  same
off-diagonal elements: perform symbolic analysis once with SPSymmAnalyze(),
then update diagonal with SPSymmReloadDiagonal() and call SPSymmFactorize().

INPUT PARAMETERS:
    Analysis    -   symbolic analysis of the matrix structure
    D           -   array[N], diagonal factor

OUTPUT PARAMETERS:
    Analysis    -   symbolic analysis of the matrix structure  which  will
                    be used later to guide  numerical  factorization.  The
                    numerical values are stored internally in the structure,
                    but you have to  run  factorization  phase  explicitly
                    with SPSymmAnalyze().  You  can  also  reload  another
                    matrix with same sparsity pattern with SPSymmReload().

  -- ALGLIB routine --
     05.09.2021
     Bochkanov Sergey
*************************************************************************/
void spsymmreloaddiagonal(spcholanalysis* analysis,
     /* Real    */ const ae_vector* d,
     ae_state *_state)
{
    ae_int_t sidx;
    ae_int_t cols0;
    ae_int_t cols1;
    ae_int_t offss;
    ae_int_t sstride;
    ae_int_t j;


    ae_assert(d->cnt>=analysis->n, "SPSymmReloadDiagonal: length(D)<N", _state);
    for(sidx=0; sidx<=analysis->nsuper-1; sidx++)
    {
        cols0 = analysis->supercolrange.ptr.p_int[sidx];
        cols1 = analysis->supercolrange.ptr.p_int[sidx+1];
        offss = analysis->rowoffsets.ptr.p_int[sidx];
        sstride = analysis->rowstrides.ptr.p_int[sidx];
        for(j=cols0; j<=cols1-1; j++)
        {
            analysis->inputstorage.ptr.p_double[offss+(j-cols0)*sstride+(j-cols0)] = d->ptr.p_double[analysis->inveffectiveperm.ptr.p_int[j]];
        }
    }
}


/*************************************************************************
Sparse Cholesky factorization of symmetric matrix stored  in  CRS  format,
using precomputed analysis of the sparsity pattern stored  in the Analysis
object and specific numeric values that  are  presently  loaded  into  the
Analysis.

The factorization can be retrieved  with  SPSymmExtract().  Alternatively,
one can perform some operations without offloading  the  matrix  (somewhat
faster due to itilization of  SIMD-friendly  supernodal  data structures),
most importantly - linear system solution with SPSymmSolve().

Depending on settings specified during factorization, may produce  vanilla
Cholesky or L*D*LT  decomposition  (with  strictly  diagonal  D),  without
permutation or with permutation P (being either  topological  ordering  or
sparsity preserving ordering).

Thus, A is represented as either L*LT or L*D*LT or P*L*LT*PT or P*L*D*LT*PT.

NOTE: L*D*LT family of factorization may be used to  factorize  indefinite
      matrices. However, numerical stability is guaranteed ONLY for a class
      of quasi-definite matrices.

INPUT PARAMETERS:
    Analysis    -   prior  analysis  performed on some sparse matrix, with
                    matrix being stored in Analysis.

OUTPUT PARAMETERS:
    Analysis    -   contains factorization results

The function returns True  when  factorization  resulted  in nondegenerate
matrix. False is returned when factorization fails (Cholesky factorization
of indefinite matrix) or LDLT factorization has exactly zero  elements  at
the diagonal.

  -- ALGLIB routine --
     20.09.2020
     Bochkanov Sergey
*************************************************************************/
ae_bool spsymmfactorize(spcholanalysis* analysis, ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t n;
    sboolean b;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&b, 0, sizeof(b));
    _sboolean_init(&b, _state, ae_true);

    ae_assert(analysis->tasktype==0, "SPCholFactorize: Analysis type does not match current task", _state);
    n = analysis->n;
    
    /*
     * Allocate temporaries
     */
    ivectorsetlengthatleast(&analysis->tmp0, n+1, _state);
    rsetallocv(n, 0.0, &analysis->diagd, _state);
    rcopyallocv(analysis->rowoffsets.ptr.p_int[analysis->nsuper], &analysis->inputstorage, &analysis->outputstorage, _state);
    icopyallocv(analysis->nsuper, &analysis->ladj.rowbegin, &analysis->currowbegin, _state);
    
    /*
     * Perform recursive processing
     */
    b.val = ae_false;
    spchol_spsymmfactorizeblockrec(analysis, &analysis->currowbegin, 0, ae_true, &b, _state);
    result = !b.val;
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Extracts result of the last Cholesky/LDLT factorization performed  on  the
Analysis object.

Following calls will  result in the undefined behavior:
* calling for Analysis that was not factorized with SPSymmFactorize()
* calling after SPSymmFactorize() returned False

INPUT PARAMETERS:
    Analysis    -   prior factorization performed on some sparse matrix
    D, P        -   possibly preallocated buffers

OUTPUT PARAMETERS:
    A           -   Cholesky/LDLT decomposition  of A stored in CRS format
                    in LOWER triangle.
    D           -   array[N], diagonal factor. If no diagonal  factor  was
                    required during analysis  phase,  still  returned  but
                    filled with units.
    P           -   array[N], pivots. Permutation matrix P is a product of
                    P(0)*P(1)*...*P(N-1), where P(i) is a  permutation  of
                    row/col I and P[I] (with P[I]>=I).
                    If no permutation was requested during analysis phase,
                    still returned but filled with unit elements.

  -- ALGLIB routine --
     20.09.2020
     Bochkanov Sergey
*************************************************************************/
void spsymmextract(spcholanalysis* analysis,
     sparsematrix* a,
     /* Real    */ ae_vector* d,
     /* Integer */ ae_vector* p,
     ae_state *_state)
{


    spchol_extractmatrix(analysis, &analysis->rowoffsets, &analysis->rowstrides, &analysis->outputstorage, &analysis->diagd, analysis->n, a, d, p, &analysis->tmp0, _state);
}


/*************************************************************************
Solve linear system A*x=b, using internally stored  factorization  of  the
matrix A.

Works faster than extracting the matrix and solving with SparseTRSV()  due
to SIMD-friendly supernodal data structures being used.

INPUT PARAMETERS:
    Analysis    -   prior factorization performed on some sparse matrix
    B           -   array[N], right-hand side

OUTPUT PARAMETERS:
    B           -   overwritten by X

  -- ALGLIB routine --
     08.09.2021
     Bochkanov Sergey
*************************************************************************/
void spsymmsolve(spcholanalysis* analysis,
     /* Real    */ ae_vector* b,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double v;
    ae_int_t baseoffs;
    ae_int_t cols0;
    ae_int_t cols1;
    ae_int_t offss;
    ae_int_t sstride;
    ae_int_t sidx;
    ae_int_t blocksize;
    ae_int_t rbase;
    ae_int_t offdiagsize;
    double x0;
    double x1;
    double x2;
    double x3;


    n = analysis->n;
    rsetallocv(n, 0.0, &analysis->tmpx, _state);
    x0 = (double)(0);
    x1 = (double)(0);
    x2 = (double)(0);
    x3 = (double)(0);
    
    /*
     * Handle left-hand side permutation, convert data to internal SIMD-friendly format
     */
    for(i=0; i<=n-1; i++)
    {
        analysis->tmpx.ptr.p_double[i] = b->ptr.p_double[analysis->inveffectiveperm.ptr.p_int[i]];
    }
    
    /*
     * Solve for L*tmp_x=rhs.
     */
    for(sidx=0; sidx<=analysis->nsuper-1; sidx++)
    {
        cols0 = analysis->supercolrange.ptr.p_int[sidx];
        cols1 = analysis->supercolrange.ptr.p_int[sidx+1];
        blocksize = cols1-cols0;
        offss = analysis->rowoffsets.ptr.p_int[sidx];
        sstride = analysis->rowstrides.ptr.p_int[sidx];
        rbase = analysis->superrowridx.ptr.p_int[sidx];
        offdiagsize = analysis->superrowridx.ptr.p_int[sidx+1]-rbase;
        
        /*
         * Solve for variables in the supernode,
         * fetch vars to locals (when supernode is small enough)
         */
        ae_assert(blocksize<=4, "SPSymm: integrity check 4228 failed", _state);
        if( blocksize==1 )
        {
            
            /*
             * One column, fetch to X0
             */
            ae_assert(sstride==1, "SPSymm: integrity check 4620 failed", _state);
            x0 = analysis->tmpx.ptr.p_double[cols0]/analysis->outputstorage.ptr.p_double[offss];
            analysis->tmpx.ptr.p_double[cols0] = x0;
        }
        else
        {
            if( blocksize==2 )
            {
                
                /*
                 * Two columns, fetch to X0 and X1
                 */
                ae_assert(sstride==2, "SPSymm: integrity check 5730 failed", _state);
                for(i=cols0; i<=cols1-1; i++)
                {
                    baseoffs = offss+(i-cols0)*sstride+(-cols0);
                    v = analysis->tmpx.ptr.p_double[i];
                    for(j=cols0; j<=i-1; j++)
                    {
                        v = v-analysis->outputstorage.ptr.p_double[baseoffs+j]*analysis->tmpx.ptr.p_double[j];
                    }
                    analysis->tmpx.ptr.p_double[i] = v/analysis->outputstorage.ptr.p_double[baseoffs+i];
                }
                x0 = analysis->tmpx.ptr.p_double[cols0];
                x1 = analysis->tmpx.ptr.p_double[cols0+1];
            }
            else
            {
                if( blocksize==3 )
                {
                    
                    /*
                     * Three columns, fetch to X0, X1 and X2
                     */
                    ae_assert(sstride==4, "SPSymm: integrity check 7446 failed", _state);
                    for(i=cols0; i<=cols1-1; i++)
                    {
                        baseoffs = offss+(i-cols0)*sstride+(-cols0);
                        v = analysis->tmpx.ptr.p_double[i];
                        for(j=cols0; j<=i-1; j++)
                        {
                            v = v-analysis->outputstorage.ptr.p_double[baseoffs+j]*analysis->tmpx.ptr.p_double[j];
                        }
                        analysis->tmpx.ptr.p_double[i] = v/analysis->outputstorage.ptr.p_double[baseoffs+i];
                    }
                    x0 = analysis->tmpx.ptr.p_double[cols0];
                    x1 = analysis->tmpx.ptr.p_double[cols0+1];
                    x2 = analysis->tmpx.ptr.p_double[cols0+2];
                }
                else
                {
                    if( blocksize==4 )
                    {
                        
                        /*
                         * Four columns, fetch to X0, X1, X2, X3
                         */
                        ae_assert(sstride==4, "SPSymm: integrity check 9252 failed", _state);
                        for(i=cols0; i<=cols1-1; i++)
                        {
                            baseoffs = offss+(i-cols0)*sstride+(-cols0);
                            v = analysis->tmpx.ptr.p_double[i];
                            for(j=cols0; j<=i-1; j++)
                            {
                                v = v-analysis->outputstorage.ptr.p_double[baseoffs+j]*analysis->tmpx.ptr.p_double[j];
                            }
                            analysis->tmpx.ptr.p_double[i] = v/analysis->outputstorage.ptr.p_double[baseoffs+i];
                        }
                        x0 = analysis->tmpx.ptr.p_double[cols0];
                        x1 = analysis->tmpx.ptr.p_double[cols0+1];
                        x2 = analysis->tmpx.ptr.p_double[cols0+2];
                        x3 = analysis->tmpx.ptr.p_double[cols0+3];
                    }
                    else
                    {
                        
                        /*
                         * Generic case
                         */
                        for(i=cols0; i<=cols1-1; i++)
                        {
                            baseoffs = offss+(i-cols0)*sstride+(-cols0);
                            v = analysis->tmpx.ptr.p_double[i];
                            for(j=cols0; j<=i-1; j++)
                            {
                                v = v-analysis->outputstorage.ptr.p_double[baseoffs+j]*analysis->tmpx.ptr.p_double[j];
                            }
                            analysis->tmpx.ptr.p_double[i] = v/analysis->outputstorage.ptr.p_double[baseoffs+i];
                        }
                    }
                }
            }
        }
        
        /*
         * Propagate update to other variables
         */
        if( blocksize==1 )
        {
            
            /*
             * Special case: single column
             */
            baseoffs = offss+1;
            for(k=0; k<=offdiagsize-1; k++)
            {
                i = analysis->superrowidx.ptr.p_int[rbase+k];
                analysis->tmpx.ptr.p_double[i] = analysis->tmpx.ptr.p_double[i]-analysis->outputstorage.ptr.p_double[baseoffs]*x0;
                baseoffs = baseoffs+1;
            }
        }
        else
        {
            if( blocksize==2 )
            {
                
                /*
                 * Two columns
                 */
                baseoffs = offss+4;
                for(k=0; k<=offdiagsize-1; k++)
                {
                    i = analysis->superrowidx.ptr.p_int[rbase+k];
                    analysis->tmpx.ptr.p_double[i] = analysis->tmpx.ptr.p_double[i]-analysis->outputstorage.ptr.p_double[baseoffs]*x0-analysis->outputstorage.ptr.p_double[baseoffs+1]*x1;
                    baseoffs = baseoffs+2;
                }
            }
            else
            {
                if( blocksize==3 )
                {
                    
                    /*
                     * Three columns
                     */
                    baseoffs = offss+12;
                    for(k=0; k<=offdiagsize-1; k++)
                    {
                        i = analysis->superrowidx.ptr.p_int[rbase+k];
                        analysis->tmpx.ptr.p_double[i] = analysis->tmpx.ptr.p_double[i]-analysis->outputstorage.ptr.p_double[baseoffs]*x0-analysis->outputstorage.ptr.p_double[baseoffs+1]*x1-analysis->outputstorage.ptr.p_double[baseoffs+2]*x2;
                        baseoffs = baseoffs+4;
                    }
                }
                else
                {
                    if( blocksize==4 )
                    {
                        
                        /*
                         * Four columns
                         */
                        baseoffs = offss+16;
                        for(k=0; k<=offdiagsize-1; k++)
                        {
                            i = analysis->superrowidx.ptr.p_int[rbase+k];
                            analysis->tmpx.ptr.p_double[i] = analysis->tmpx.ptr.p_double[i]-analysis->outputstorage.ptr.p_double[baseoffs]*x0-analysis->outputstorage.ptr.p_double[baseoffs+1]*x1-analysis->outputstorage.ptr.p_double[baseoffs+2]*x2-analysis->outputstorage.ptr.p_double[baseoffs+3]*x3;
                            baseoffs = baseoffs+4;
                        }
                    }
                    else
                    {
                        
                        /*
                         * Generic propagate
                         */
                        for(k=0; k<=offdiagsize-1; k++)
                        {
                            i = analysis->superrowidx.ptr.p_int[rbase+k];
                            baseoffs = offss+(k+blocksize)*sstride;
                            v = analysis->tmpx.ptr.p_double[i];
                            for(j=0; j<=blocksize-1; j++)
                            {
                                v = v-analysis->outputstorage.ptr.p_double[baseoffs+j]*analysis->tmpx.ptr.p_double[cols0+j];
                            }
                            analysis->tmpx.ptr.p_double[i] = v;
                        }
                    }
                }
            }
        }
    }
    
    /*
     * Solve for D*tmp_x=rhs.
     */
    for(i=0; i<=n-1; i++)
    {
        if( analysis->diagd.ptr.p_double[i]!=0.0 )
        {
            analysis->tmpx.ptr.p_double[i] = analysis->tmpx.ptr.p_double[i]/analysis->diagd.ptr.p_double[i];
        }
        else
        {
            analysis->tmpx.ptr.p_double[i] = 0.0;
        }
    }
    
    /*
     * Solve for L'*tmp_x=rhs
     *
     */
    for(sidx=analysis->nsuper-1; sidx>=0; sidx--)
    {
        cols0 = analysis->supercolrange.ptr.p_int[sidx];
        cols1 = analysis->supercolrange.ptr.p_int[sidx+1];
        blocksize = cols1-cols0;
        offss = analysis->rowoffsets.ptr.p_int[sidx];
        sstride = analysis->rowstrides.ptr.p_int[sidx];
        rbase = analysis->superrowridx.ptr.p_int[sidx];
        offdiagsize = analysis->superrowridx.ptr.p_int[sidx+1]-rbase;
        
        /*
         * Subtract already computed variables
         */
        if( blocksize==1 )
        {
            
            /*
             * Single column, use value fetched in X0
             */
            x0 = analysis->tmpx.ptr.p_double[cols0];
            baseoffs = offss+1;
            for(k=0; k<=offdiagsize-1; k++)
            {
                x0 = x0-analysis->outputstorage.ptr.p_double[baseoffs]*analysis->tmpx.ptr.p_double[analysis->superrowidx.ptr.p_int[rbase+k]];
                baseoffs = baseoffs+1;
            }
            analysis->tmpx.ptr.p_double[cols0] = x0;
        }
        else
        {
            if( blocksize==2 )
            {
                
                /*
                 * Two columns, use values fetched in X0, X1
                 */
                x0 = analysis->tmpx.ptr.p_double[cols0];
                x1 = analysis->tmpx.ptr.p_double[cols0+1];
                baseoffs = offss+4;
                for(k=0; k<=offdiagsize-1; k++)
                {
                    v = analysis->tmpx.ptr.p_double[analysis->superrowidx.ptr.p_int[rbase+k]];
                    x0 = x0-analysis->outputstorage.ptr.p_double[baseoffs]*v;
                    x1 = x1-analysis->outputstorage.ptr.p_double[baseoffs+1]*v;
                    baseoffs = baseoffs+2;
                }
                analysis->tmpx.ptr.p_double[cols0] = x0;
                analysis->tmpx.ptr.p_double[cols0+1] = x1;
            }
            else
            {
                if( blocksize==3 )
                {
                    
                    /*
                     * Three columns, use values fetched in X0, X1, X2
                     */
                    x0 = analysis->tmpx.ptr.p_double[cols0];
                    x1 = analysis->tmpx.ptr.p_double[cols0+1];
                    x2 = analysis->tmpx.ptr.p_double[cols0+2];
                    baseoffs = offss+12;
                    for(k=0; k<=offdiagsize-1; k++)
                    {
                        v = analysis->tmpx.ptr.p_double[analysis->superrowidx.ptr.p_int[rbase+k]];
                        x0 = x0-analysis->outputstorage.ptr.p_double[baseoffs]*v;
                        x1 = x1-analysis->outputstorage.ptr.p_double[baseoffs+1]*v;
                        x2 = x2-analysis->outputstorage.ptr.p_double[baseoffs+2]*v;
                        baseoffs = baseoffs+4;
                    }
                    analysis->tmpx.ptr.p_double[cols0] = x0;
                    analysis->tmpx.ptr.p_double[cols0+1] = x1;
                    analysis->tmpx.ptr.p_double[cols0+2] = x2;
                }
                else
                {
                    if( blocksize==4 )
                    {
                        
                        /*
                         * Four columns, use values fetched in X0, X1, X2, X3
                         */
                        x0 = analysis->tmpx.ptr.p_double[cols0];
                        x1 = analysis->tmpx.ptr.p_double[cols0+1];
                        x2 = analysis->tmpx.ptr.p_double[cols0+2];
                        x3 = analysis->tmpx.ptr.p_double[cols0+3];
                        baseoffs = offss+16;
                        for(k=0; k<=offdiagsize-1; k++)
                        {
                            v = analysis->tmpx.ptr.p_double[analysis->superrowidx.ptr.p_int[rbase+k]];
                            x0 = x0-analysis->outputstorage.ptr.p_double[baseoffs]*v;
                            x1 = x1-analysis->outputstorage.ptr.p_double[baseoffs+1]*v;
                            x2 = x2-analysis->outputstorage.ptr.p_double[baseoffs+2]*v;
                            x3 = x3-analysis->outputstorage.ptr.p_double[baseoffs+3]*v;
                            baseoffs = baseoffs+4;
                        }
                        analysis->tmpx.ptr.p_double[cols0] = x0;
                        analysis->tmpx.ptr.p_double[cols0+1] = x1;
                        analysis->tmpx.ptr.p_double[cols0+2] = x2;
                        analysis->tmpx.ptr.p_double[cols0+3] = x3;
                    }
                    else
                    {
                        
                        /*
                         * Generic case
                         */
                        for(k=0; k<=offdiagsize-1; k++)
                        {
                            baseoffs = offss+(k+blocksize)*sstride;
                            v = analysis->tmpx.ptr.p_double[analysis->superrowidx.ptr.p_int[rbase+k]];
                            for(j=0; j<=blocksize-1; j++)
                            {
                                analysis->tmpx.ptr.p_double[cols0+j] = analysis->tmpx.ptr.p_double[cols0+j]-analysis->outputstorage.ptr.p_double[baseoffs+j]*v;
                            }
                        }
                    }
                }
            }
        }
        
        /*
         * Solve for variables in the supernode
         */
        for(i=blocksize-1; i>=0; i--)
        {
            baseoffs = offss+i*sstride;
            v = analysis->tmpx.ptr.p_double[cols0+i]/analysis->outputstorage.ptr.p_double[baseoffs+i];
            for(j=0; j<=i-1; j++)
            {
                analysis->tmpx.ptr.p_double[cols0+j] = analysis->tmpx.ptr.p_double[cols0+j]-v*analysis->outputstorage.ptr.p_double[baseoffs+j];
            }
            analysis->tmpx.ptr.p_double[cols0+i] = v;
        }
    }
    
    /*
     * Handle right-hand side permutation, convert data to internal SIMD-friendly format
     */
    for(i=0; i<=n-1; i++)
    {
        b->ptr.p_double[i] = analysis->tmpx.ptr.p_double[analysis->effectiveperm.ptr.p_int[i]];
    }
}


/*************************************************************************
Compares diag(L*L') with that of the original A and returns  two  metrics:
* SumSq - sum of squares of diag(A)
* ErrSq - sum of squared errors, i.e. Frobenius norm of diag(L*L')-diag(A)

These metrics can be used to check accuracy of the factorization.

INPUT PARAMETERS:
    Analysis    -   prior factorization performed on some sparse matrix

OUTPUT PARAMETERS:
    SumSq, ErrSq-   diagonal magnitude and absolute diagonal error

  -- ALGLIB routine --
     08.09.2021
     Bochkanov Sergey
*************************************************************************/
void spsymmdiagerr(spcholanalysis* analysis,
     double* sumsq,
     double* errsq,
     ae_state *_state)
{
    ae_int_t n;
    double v;
    double vv;
    ae_int_t simdwidth;
    ae_int_t baseoffs;
    ae_int_t cols0;
    ae_int_t cols1;
    ae_int_t offss;
    ae_int_t sstride;
    ae_int_t sidx;
    ae_int_t blocksize;
    ae_int_t rbase;
    ae_int_t offdiagsize;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;

    *sumsq = 0.0;
    *errsq = 0.0;

    n = analysis->n;
    simdwidth = 1;
    
    /*
     * Scan L, compute diag(L*L')
     */
    rsetallocv(simdwidth*n, 0.0, &analysis->simdbuf, _state);
    for(sidx=0; sidx<=analysis->nsuper-1; sidx++)
    {
        cols0 = analysis->supercolrange.ptr.p_int[sidx];
        cols1 = analysis->supercolrange.ptr.p_int[sidx+1];
        blocksize = cols1-cols0;
        offss = analysis->rowoffsets.ptr.p_int[sidx];
        sstride = analysis->rowstrides.ptr.p_int[sidx];
        rbase = analysis->superrowridx.ptr.p_int[sidx];
        offdiagsize = analysis->superrowridx.ptr.p_int[sidx+1]-rbase;
        
        /*
         * Handle triangular diagonal block
         */
        for(i=cols0; i<=cols1-1; i++)
        {
            baseoffs = offss+(i-cols0)*sstride+(-cols0);
            v = (double)(0);
            for(j=0; j<=simdwidth-1; j++)
            {
                v = v+analysis->simdbuf.ptr.p_double[i*simdwidth+j];
            }
            for(j=cols0; j<=i; j++)
            {
                vv = analysis->outputstorage.ptr.p_double[baseoffs+j];
                v = v+vv*vv*analysis->diagd.ptr.p_double[j];
            }
            *sumsq = *sumsq+ae_sqr(analysis->inputstorage.ptr.p_double[baseoffs+i], _state);
            *errsq = *errsq+ae_sqr(analysis->inputstorage.ptr.p_double[baseoffs+i]-v, _state);
        }
        
        /*
         * Accumulate entries below triangular diagonal block
         */
        for(k=0; k<=offdiagsize-1; k++)
        {
            i = analysis->superrowidx.ptr.p_int[rbase+k];
            baseoffs = offss+(k+blocksize)*sstride;
            v = analysis->simdbuf.ptr.p_double[i*simdwidth];
            for(j=0; j<=blocksize-1; j++)
            {
                vv = analysis->outputstorage.ptr.p_double[baseoffs+j];
                v = v+vv*vv*analysis->diagd.ptr.p_double[cols0+j];
            }
            analysis->simdbuf.ptr.p_double[i*simdwidth] = v;
        }
    }
}


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Informational function, useful for debugging
*************************************************************************/
static ae_int_t spchol_spsymmgetmaxsimd(ae_state *_state)
{
    ae_int_t result;


    result = 1;
    return result;
}
#endif


/*************************************************************************
Recursive factorization of the supernodal  block  (a set of interdependent
supernodes) stored at BlkStruct[] at offset BlkOffs.

Due to interdependencies, block supernodes can NOT be  factorized  in  the
parallel manner. However, it is still possible to perform  parallel column
updates (this part is handled by parallel DC  tree  associated  with  each
block).

INPUT PARAMETERS:
    Analysis    -   prior  analysis  performed on some sparse matrix, with
                    matrix being stored in Analysis.
    CurLAdjRowBegin-array[NSuper], pointers to unprocessed column  updates
                    stored in LAdj structure. Initially it is  a  copy  of
                    LAdj.RowBegin[], but every time  we  apply  update  to
                    a column, we advance its CurLAdjRowBegin[] entry.
                    This array is shared between all concurrently  running
                    worker threads.
    BlkOffs     -   block offset relative to the beginning of BlkStruct[]

OUTPUT PARAMETERS:
    Analysis    -   contains factorization results
    FailureFlag -   on the failure is set to True, ignored on success.
                    Such design is thread-safe.

The function returns True  when  factorization  resulted  in nondegenerate
matrix. False is returned when factorization fails (Cholesky factorization
of indefinite matrix) or LDLT factorization has exactly zero  elements  at
the diagonal.

  -- ALGLIB routine --
     09.07.2022
     Bochkanov Sergey
*************************************************************************/
static void spchol_spsymmfactorizeblockrec(spcholanalysis* analysis,
     /* Integer */ ae_vector* curladjrowbegin,
     ae_int_t blkoffs,
     ae_bool isrootcall,
     sboolean* failureflag,
     ae_state *_state)
{
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t bs;
    ae_int_t cc;
    ae_int_t curoffs;
    ae_int_t childrenlistoffs;
    ae_int_t gidx;
    ae_int_t groupscnt;
    ae_int_t i;


    ae_assert(analysis->tasktype==0, "SPCholFactorize: Analysis type does not match current task", _state);
    
    /*
     * Try parallel execution
     */
    if( isrootcall&&analysis->useparallelism )
    {
        if( _trypexec_spchol_spsymmfactorizeblockrec(analysis,curladjrowbegin,blkoffs,isrootcall,failureflag, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    
    /*
     * Analyze block information
     */
    curoffs = blkoffs;
    bs = analysis->blkstruct.ptr.p_int[curoffs];
    curoffs = curoffs+1+bs;
    cc = analysis->blkstruct.ptr.p_int[curoffs];
    childrenlistoffs = curoffs+2;
    curoffs = curoffs+2+cc;
    
    /*
     * Process children.
     * The very fact that we created more than one children node means that parallel processing is needed.
     */
    if( cc>0 )
    {
        ae_set_smp_support(&_child_tasks, &_smp_enabled, cc>1, _state);
        for(i=0; i<=cc-1; i++)
        {
            _spawn_spchol_spsymmfactorizeblockrec(analysis, curladjrowbegin, analysis->blkstruct.ptr.p_int[childrenlistoffs+i], ae_false, failureflag, _child_tasks, _smp_enabled, _state);
        }
        ae_sync(_child_tasks, _smp_enabled, ae_false, _state);
    }
    
    /*
     * Apply precomputed update-and-factorize sequence stored in the UPDATES section of the block
     */
    groupscnt = analysis->blkstruct.ptr.p_int[curoffs+1];
    curoffs = curoffs+spchol_updatesheadersize;
    for(gidx=0; gidx<=groupscnt-1; gidx++)
    {
        spchol_spsymmprocessupdatesgroup(analysis, curoffs, failureflag, _state);
        curoffs = curoffs+analysis->blkstruct.ptr.p_int[curoffs+0];
    }
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_spchol_spsymmfactorizeblockrec(spcholanalysis* analysis,
    /* Integer */ ae_vector* curladjrowbegin,
    ae_int_t blkoffs,
    ae_bool isrootcall,
    sboolean* failureflag,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        spchol_spsymmfactorizeblockrec(analysis,curladjrowbegin,blkoffs,isrootcall,failureflag, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_spchol_spsymmfactorizeblockrec;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = analysis;
        _task->data.parameters[1].value.val = curladjrowbegin;
        _task->data.parameters[2].value.ival = blkoffs;
        _task->data.parameters[3].value.bval = isrootcall;
        _task->data.parameters[4].value.val = failureflag;
        ae_push_task(_task, _state, ae_false);
    }
#else
    abort(); /* critical integrity failure */
#endif
}


/*************************************************************************
This method is called when worker thread starts working on a non-root task.
*************************************************************************/
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_spchol_spsymmfactorizeblockrec(ae_task_data *_data, ae_state *_state)
{
    spcholanalysis* analysis;
    ae_vector* curladjrowbegin;
    ae_int_t blkoffs;
    ae_bool isrootcall;
    sboolean* failureflag;
    analysis = (spcholanalysis*)_data->parameters[0].value.val;
    curladjrowbegin = (ae_vector*)_data->parameters[1].value.val;
    blkoffs = _data->parameters[2].value.ival;
    isrootcall = _data->parameters[3].value.bval;
    failureflag = (sboolean*)_data->parameters[4].value.val;
   ae_state_set_flags(_state, _data->flags);
   spchol_spsymmfactorizeblockrec(analysis,curladjrowbegin,blkoffs,isrootcall,failureflag, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_spchol_spsymmfactorizeblockrec(spcholanalysis* analysis,
    /* Integer */ ae_vector* curladjrowbegin,
    ae_int_t blkoffs,
    ae_bool isrootcall,
    sboolean* failureflag,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_spchol_spsymmfactorizeblockrec;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = analysis;
    _task->data.parameters[1].value.val = curladjrowbegin;
    _task->data.parameters[2].value.ival = blkoffs;
    _task->data.parameters[3].value.bval = isrootcall;
    _task->data.parameters[4].value.val = failureflag;
    ae_push_root_task(_task);
    ae_wait_for_event(&_task->done_event);
    _e = _task->exception;
    ae_dispose_task(_task);
    ae_assert(_e==NULL, _e, _state);
    return ae_true;
#else
    return ae_false;
#endif
}


/*************************************************************************
This function processes updates group (a set of precomputed update batches
that can be applied concurrently) stored in Analysis.BlkStruct

INPUT PARAMETERS:
    Analysis    -   prior  analysis  performed on some sparse matrix, with
                    matrix being stored in Analysis.
    BlkOffs     -   block offset relative to the beginning of BlkStruct[],
                    beginning of the group data

OUTPUT PARAMETERS:
    Analysis    -   partial supernode update is applied
    FailureFlag -   on the failure is set to True, ignored on success.
                    Such design is thread-safe.

  -- ALGLIB routine --
     09.07.2022
     Bochkanov Sergey
*************************************************************************/
static void spchol_spsymmprocessupdatesgroup(spcholanalysis* analysis,
     ae_int_t blkoffs,
     sboolean* failureflag,
     ae_state *_state)
{
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t bidx;
    ae_int_t batchescnt;


    batchescnt = analysis->blkstruct.ptr.p_int[blkoffs+1];
    blkoffs = blkoffs+spchol_groupheadersize;
    
    /*
     * One batch, sequential processing
     */
    if( batchescnt==1 )
    {
        spchol_spsymmprocessupdatesbatch(analysis, blkoffs, failureflag, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Parallel processing (more than one batch in the group means that we decided
     * to parallelize computations during the scheduling stage)
     */
    ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_true, _state);
    for(bidx=0; bidx<=batchescnt-1; bidx++)
    {
        _spawn_spchol_spsymmprocessupdatesbatch(analysis, blkoffs, failureflag, _child_tasks, _smp_enabled, _state);
        blkoffs = blkoffs+analysis->blkstruct.ptr.p_int[blkoffs+0];
    }
    ae_sync(_child_tasks, _smp_enabled, ae_false, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_spchol_spsymmprocessupdatesgroup(spcholanalysis* analysis,
    ae_int_t blkoffs,
    sboolean* failureflag,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        spchol_spsymmprocessupdatesgroup(analysis,blkoffs,failureflag, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_spchol_spsymmprocessupdatesgroup;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = analysis;
        _task->data.parameters[1].value.ival = blkoffs;
        _task->data.parameters[2].value.val = failureflag;
        ae_push_task(_task, _state, ae_false);
    }
#else
    abort(); /* critical integrity failure */
#endif
}


/*************************************************************************
This method is called when worker thread starts working on a non-root task.
*************************************************************************/
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_spchol_spsymmprocessupdatesgroup(ae_task_data *_data, ae_state *_state)
{
    spcholanalysis* analysis;
    ae_int_t blkoffs;
    sboolean* failureflag;
    analysis = (spcholanalysis*)_data->parameters[0].value.val;
    blkoffs = _data->parameters[1].value.ival;
    failureflag = (sboolean*)_data->parameters[2].value.val;
   ae_state_set_flags(_state, _data->flags);
   spchol_spsymmprocessupdatesgroup(analysis,blkoffs,failureflag, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_spchol_spsymmprocessupdatesgroup(spcholanalysis* analysis,
    ae_int_t blkoffs,
    sboolean* failureflag,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_spchol_spsymmprocessupdatesgroup;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = analysis;
    _task->data.parameters[1].value.ival = blkoffs;
    _task->data.parameters[2].value.val = failureflag;
    ae_push_root_task(_task);
    ae_wait_for_event(&_task->done_event);
    _e = _task->exception;
    ae_dispose_task(_task);
    ae_assert(_e==NULL, _e, _state);
    return ae_true;
#else
    return ae_false;
#endif
}


/*************************************************************************
This function processes updates batch (a set of precomputed update
sequences that has to be applied sequentially) stored in Analysis.BlkStruct

INPUT PARAMETERS:
    Analysis    -   prior  analysis  performed on some sparse matrix, with
                    matrix being stored in Analysis.
    BlkOffs     -   block offset relative to the beginning of BlkStruct[],
                    beginning of the batch data

OUTPUT PARAMETERS:
    Analysis    -   partial supernode update is applied
    FailureFlag -   on the failure is set to True, ignored on success.
                    Such design is thread-safe.

  -- ALGLIB routine --
     09.07.2022
     Bochkanov Sergey
*************************************************************************/
static void spchol_spsymmprocessupdatesbatch(spcholanalysis* analysis,
     ae_int_t blkoffs,
     sboolean* failureflag,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t seqidx;
    ae_int_t sequencescnt;
    ae_int_t sidx;
    ae_int_t cols0;
    ae_int_t cols1;
    ae_int_t supernodesize;
    ae_int_t offss;
    ae_int_t i;
    ae_int_t k;
    ae_int_t k0;
    ae_int_t k1;
    ae_int_t i0;
    ae_int_t i1;
    ae_vector raw2smap;

    ae_frame_make(_state, &_frame_block);
    memset(&raw2smap, 0, sizeof(raw2smap));
    ae_vector_init(&raw2smap, 0, DT_INT, _state, ae_true);

    nipoolretrieve(&analysis->n1integerpool, &raw2smap, _state);
    sequencescnt = analysis->blkstruct.ptr.p_int[blkoffs+1];
    blkoffs = blkoffs+spchol_batchheadersize;
    for(seqidx=0; seqidx<=sequencescnt-1; seqidx++)
    {
        sidx = analysis->blkstruct.ptr.p_int[blkoffs+0];
        i0 = analysis->blkstruct.ptr.p_int[blkoffs+1];
        i1 = analysis->blkstruct.ptr.p_int[blkoffs+2];
        
        /*
         * Do we need updates (do we have children columns)?
         */
        if( i1>i0 )
        {
            
            /*
             * Prepare mapping of raw (range 0...N-1) indexes into internal (range 0...SupernodeSize+OffdiagSize-1) ones
             */
            cols0 = analysis->supercolrange.ptr.p_int[sidx];
            cols1 = analysis->supercolrange.ptr.p_int[sidx+1];
            supernodesize = cols1-cols0;
            offss = analysis->rowoffsets.ptr.p_int[sidx];
            for(i=cols0; i<=cols1-1; i++)
            {
                raw2smap.ptr.p_int[i] = i-cols0;
            }
            k0 = analysis->superrowridx.ptr.p_int[sidx];
            k1 = analysis->superrowridx.ptr.p_int[sidx+1]-1;
            for(k=k0; k<=k1; k++)
            {
                raw2smap.ptr.p_int[analysis->superrowidx.ptr.p_int[k]] = supernodesize+(k-k0);
            }
            
            /*
             * Update current supernode with remaining updates.
             */
            spchol_updatesupernode(analysis, sidx, cols0, cols1, offss, &raw2smap, i0, i1, &analysis->diagd, _state);
        }
        
        /*
         * Factorize current supernode if last update was applied
         */
        if( i1==analysis->ladj.rowend.ptr.p_int[sidx]&&!spchol_factorizesupernode(analysis, sidx, _state) )
        {
            nipoolrecycle(&analysis->n1integerpool, &raw2smap, _state);
            failureflag->val = ae_true;
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            ae_frame_leave(_state);
            return;
        }
        
        /*
         * Next sequence
         */
        blkoffs = blkoffs+spchol_sequenceentrysize;
    }
    nipoolrecycle(&analysis->n1integerpool, &raw2smap, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_spchol_spsymmprocessupdatesbatch(spcholanalysis* analysis,
    ae_int_t blkoffs,
    sboolean* failureflag,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        spchol_spsymmprocessupdatesbatch(analysis,blkoffs,failureflag, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_spchol_spsymmprocessupdatesbatch;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = analysis;
        _task->data.parameters[1].value.ival = blkoffs;
        _task->data.parameters[2].value.val = failureflag;
        ae_push_task(_task, _state, ae_false);
    }
#else
    abort(); /* critical integrity failure */
#endif
}


/*************************************************************************
This method is called when worker thread starts working on a non-root task.
*************************************************************************/
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_spchol_spsymmprocessupdatesbatch(ae_task_data *_data, ae_state *_state)
{
    spcholanalysis* analysis;
    ae_int_t blkoffs;
    sboolean* failureflag;
    analysis = (spcholanalysis*)_data->parameters[0].value.val;
    blkoffs = _data->parameters[1].value.ival;
    failureflag = (sboolean*)_data->parameters[2].value.val;
   ae_state_set_flags(_state, _data->flags);
   spchol_spsymmprocessupdatesbatch(analysis,blkoffs,failureflag, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_spchol_spsymmprocessupdatesbatch(spcholanalysis* analysis,
    ae_int_t blkoffs,
    sboolean* failureflag,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_spchol_spsymmprocessupdatesbatch;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = analysis;
    _task->data.parameters[1].value.ival = blkoffs;
    _task->data.parameters[2].value.val = failureflag;
    ae_push_root_task(_task);
    ae_wait_for_event(&_task->done_event);
    _e = _task->exception;
    ae_dispose_task(_task);
    ae_assert(_e==NULL, _e, _state);
    return ae_true;
#else
    return ae_false;
#endif
}


/*************************************************************************
Print blocked elimination tree to trace log.

  -- ALGLIB routine --
     09.07.2022
     Bochkanov Sergey
*************************************************************************/
static void spchol_printblockedeliminationtreerec(const spcholanalysis* analysis,
     ae_int_t blkoffs,
     ae_int_t depth,
     ae_state *_state)
{
    ae_int_t curoffs;
    ae_int_t bs;
    ae_int_t cc;
    ae_int_t i;
    ae_int_t supernodeslistoffs;
    ae_int_t childrenlistoffs;
    double selfcost;
    double avgsnode;
    ae_int_t bidx;
    ae_int_t sidx;
    ae_int_t cols0;
    ae_int_t cols1;


    curoffs = blkoffs;
    bs = analysis->blkstruct.ptr.p_int[curoffs];
    supernodeslistoffs = curoffs+1;
    curoffs = curoffs+1+bs;
    cc = analysis->blkstruct.ptr.p_int[curoffs];
    childrenlistoffs = curoffs+2;
    curoffs = curoffs+2+cc;
    
    /*
     * Print blocked elimination tree node
     */
    selfcost = (double)(0);
    avgsnode = (double)(0);
    for(bidx=0; bidx<=bs-1; bidx++)
    {
        sidx = analysis->blkstruct.ptr.p_int[supernodeslistoffs+bidx];
        cols0 = analysis->supercolrange.ptr.p_int[sidx];
        cols1 = analysis->supercolrange.ptr.p_int[sidx+1];
        avgsnode = avgsnode+(double)(cols1-cols0)/(double)bs;
        selfcost = selfcost+analysis->ladj.nflop.ptr.p_double[sidx];
    }
    tracespaces(depth, _state);
    ae_trace("* block of %0d supernodes (avg.size=%0.1f)",
        (int)(bs),
        (double)(avgsnode));
    if( cc>0 )
    {
        ae_trace(", %0d children",
            (int)(cc));
    }
    ae_trace(", update-and-factorize = %0.1f MFLOP",
        (double)(1.0E-6*selfcost));
    ae_trace("\n");
    
    /*
     * Print children.
     */
    for(i=0; i<=cc-1; i++)
    {
        spchol_printblockedeliminationtreerec(analysis, analysis->blkstruct.ptr.p_int[childrenlistoffs+i], depth+1, _state);
    }
}


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Solving linear system: propagating computed supernode.

Propagates computed supernode to the rest of the RHS  using  SIMD-friendly
RHS storage format.

INPUT PARAMETERS:

OUTPUT PARAMETERS:

  -- ALGLIB routine --
     08.09.2021
     Bochkanov Sergey
*************************************************************************/
static void spchol_propagatefwd(/* Real    */ const ae_vector* x,
     ae_int_t cols0,
     ae_int_t blocksize,
     /* Integer */ const ae_vector* superrowidx,
     ae_int_t rbase,
     ae_int_t offdiagsize,
     /* Real    */ const ae_vector* rowstorage,
     ae_int_t offss,
     ae_int_t sstride,
     /* Real    */ ae_vector* simdbuf,
     ae_int_t simdwidth,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t baseoffs;
    double v;


    for(k=0; k<=offdiagsize-1; k++)
    {
        i = superrowidx->ptr.p_int[rbase+k];
        baseoffs = offss+(k+blocksize)*sstride;
        v = simdbuf->ptr.p_double[i*simdwidth];
        for(j=0; j<=blocksize-1; j++)
        {
            v = v-rowstorage->ptr.p_double[baseoffs+j]*x->ptr.p_double[cols0+j];
        }
        simdbuf->ptr.p_double[i*simdwidth] = v;
    }
}
#endif


/*************************************************************************
This function generates test reodering used for debug purposes only

INPUT PARAMETERS
    A           -   lower triangular sparse matrix in CRS format
    N           -   problem size
    
OUTPUT PARAMETERS
    Perm        -   array[N], maps original indexes I to permuted indexes
    InvPerm     -   array[N], maps permuted indexes I to original indexes

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void spchol_generatedbgpermutation(const sparsematrix* a,
     ae_int_t n,
     /* Integer */ ae_vector* perm,
     /* Integer */ ae_vector* invperm,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t jj;
    ae_vector d;
    ae_vector tmpr;
    ae_vector tmpperm;

    ae_frame_make(_state, &_frame_block);
    memset(&d, 0, sizeof(d));
    memset(&tmpr, 0, sizeof(tmpr));
    memset(&tmpperm, 0, sizeof(tmpperm));
    ae_vector_init(&d, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmpr, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmpperm, 0, DT_INT, _state, ae_true);

    
    /*
     * Initialize D by vertex degrees
     */
    rsetallocv(n, (double)(0), &d, _state);
    for(i=0; i<=n-1; i++)
    {
        j0 = a->ridx.ptr.p_int[i];
        j1 = a->didx.ptr.p_int[i]-1;
        d.ptr.p_double[i] = (double)(j1-j0+1);
        for(jj=j0; jj<=j1; jj++)
        {
            j = a->idx.ptr.p_int[jj];
            d.ptr.p_double[j] = d.ptr.p_double[j]+(double)1;
        }
    }
    
    /*
     * Prepare permutation that orders vertices by degrees
     */
    iallocv(n, invperm, _state);
    for(i=0; i<=n-1; i++)
    {
        invperm->ptr.p_int[i] = i;
    }
    tagsortfasti(&d, invperm, &tmpr, &tmpperm, n, _state);
    iallocv(n, perm, _state);
    for(i=0; i<=n-1; i++)
    {
        perm->ptr.p_int[invperm->ptr.p_int[i]] = i;
    }
    ae_frame_leave(_state);
}


/*************************************************************************
This function builds elimination tree in the original column order

INPUT PARAMETERS
    A           -   lower triangular sparse matrix in CRS format
    N           -   problem size
    Parent,
    tAbove      -   preallocated temporary array, length at least N+1, no
                    meaningful output is provided in these variables
    
OUTPUT PARAMETERS
    Parent      -   array[N], Parent[I] contains index of parent of I-th
                    column. -1 is used to denote column with no parents.

  -- ALGLIB PROJECT --
     Copyright 15.08.2021 by Bochkanov Sergey.
*************************************************************************/
static void spchol_buildunorderedetree(const sparsematrix* a,
     ae_int_t n,
     /* Integer */ ae_vector* parent,
     /* Integer */ ae_vector* tabove,
     ae_state *_state)
{
    ae_int_t r;
    ae_int_t abover;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t jj;


    ae_assert(parent->cnt>=n+1, "BuildUnorderedETree: input buffer Parent is too short", _state);
    ae_assert(tabove->cnt>=n+1, "BuildUnorderedETree: input buffer tAbove is too short", _state);
    
    /*
     * Build elimination tree using Liu's algorithm with path compression
     */
    for(j=0; j<=n-1; j++)
    {
        parent->ptr.p_int[j] = n;
        tabove->ptr.p_int[j] = n;
        j0 = a->ridx.ptr.p_int[j];
        j1 = a->didx.ptr.p_int[j]-1;
        for(jj=j0; jj<=j1; jj++)
        {
            r = a->idx.ptr.p_int[jj];
            abover = tabove->ptr.p_int[r];
            while(abover<j)
            {
                k = abover;
                tabove->ptr.p_int[r] = j;
                r = k;
                abover = tabove->ptr.p_int[r];
            }
            if( abover==n )
            {
                tabove->ptr.p_int[r] = j;
                parent->ptr.p_int[r] = j;
            }
        }
    }
    
    /*
     * Convert to external format
     */
    for(i=0; i<=n-1; i++)
    {
        if( parent->ptr.p_int[i]==n )
        {
            parent->ptr.p_int[i] = -1;
        }
    }
}


/*************************************************************************
This function analyzes  elimination  tree  stored  using  'parent-of-node'
format and converts it to the 'childrens-of-node' format.

INPUT PARAMETERS
    Parent      -   array[N], supernodal etree
    N           -   problem size
    ChildrenR,
    ChildrenI,
    tTmp0       -   preallocated arrays, length at least N+1
    
OUTPUT PARAMETERS
    ChildrenR   -   array[N+1], children range (see below)
    ChildrenI   -   array[N+1], childrens of K-th node are stored  in  the
                    elements ChildrenI[ChildrenR[K]...ChildrenR[K+1]-1]
                    
  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void spchol_fromparenttochildren(/* Integer */ const ae_vector* parent,
     ae_int_t n,
     /* Integer */ ae_vector* childrenr,
     /* Integer */ ae_vector* childreni,
     /* Integer */ ae_vector* ttmp0,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;
    ae_int_t nodeidx;


    ae_assert(ttmp0->cnt>=n+1, "FromParentToChildren: input buffer tTmp0 is too short", _state);
    ae_assert(childrenr->cnt>=n+1, "FromParentToChildren: input buffer ChildrenR is too short", _state);
    ae_assert(childreni->cnt>=n+1, "FromParentToChildren: input buffer ChildrenI is too short", _state);
    
    /*
     * Convert etree from per-column parent array to per-column children list
     */
    isetv(n, 0, ttmp0, _state);
    for(i=0; i<=n-1; i++)
    {
        nodeidx = parent->ptr.p_int[i];
        if( nodeidx>=0 )
        {
            ttmp0->ptr.p_int[nodeidx] = ttmp0->ptr.p_int[nodeidx]+1;
        }
    }
    childrenr->ptr.p_int[0] = 0;
    for(i=0; i<=n-1; i++)
    {
        childrenr->ptr.p_int[i+1] = childrenr->ptr.p_int[i]+ttmp0->ptr.p_int[i];
    }
    isetv(n, 0, ttmp0, _state);
    for(i=0; i<=n-1; i++)
    {
        k = parent->ptr.p_int[i];
        if( k>=0 )
        {
            childreni->ptr.p_int[childrenr->ptr.p_int[k]+ttmp0->ptr.p_int[k]] = i;
            ttmp0->ptr.p_int[k] = ttmp0->ptr.p_int[k]+1;
        }
    }
}


/*************************************************************************
This function builds elimination tree and reorders  it  according  to  the
topological post-ordering.

INPUT PARAMETERS
    A           -   lower triangular sparse matrix in CRS format
    N           -   problem size
    
    tRawParentOfRawNode,
    tRawParentOfReorderedNode,
    tTmp,
    tFlagArray  -   preallocated temporary arrays, length at least N+1, no
                    meaningful output is provided in these variables
    
OUTPUT PARAMETERS
    Parent      -   array[N], Parent[I] contains index of parent of I-th
                    column (after topological reordering). -1 is used to
                    denote column with no parents.
    SupernodalPermutation
                -   array[N], maps original indexes I to permuted indexes
    InvSupernodalPermutation
                -   array[N], maps permuted indexes I to original indexes

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void spchol_buildorderedetree(const sparsematrix* a,
     ae_int_t n,
     /* Integer */ ae_vector* parent,
     /* Integer */ ae_vector* supernodalpermutation,
     /* Integer */ ae_vector* invsupernodalpermutation,
     /* Integer */ ae_vector* trawparentofrawnode,
     /* Integer */ ae_vector* trawparentofreorderednode,
     /* Integer */ ae_vector* ttmp,
     /* Boolean */ ae_vector* tflagarray,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;
    ae_int_t sidx;
    ae_int_t unprocessedchildrencnt;


    ae_assert(trawparentofrawnode->cnt>=n+1, "BuildOrderedETree: input buffer tRawParentOfRawNode is too short", _state);
    ae_assert(ttmp->cnt>=n+1, "BuildOrderedETree: input buffer tTmp is too short", _state);
    ae_assert(trawparentofreorderednode->cnt>=n+1, "BuildOrderedETree: input buffer tRawParentOfReorderedNode is too short", _state);
    ae_assert(tflagarray->cnt>=n+1, "BuildOrderedETree: input buffer tFlagArray is too short", _state);
    
    /*
     * Avoid spurious compiler warnings
     */
    unprocessedchildrencnt = 0;
    
    /*
     * Build elimination tree with original column order
     */
    spchol_buildunorderedetree(a, n, trawparentofrawnode, ttmp, _state);
    
    /*
     * Compute topological ordering of the elimination tree, produce:
     * * direct and inverse permutations
     * * reordered etree stored in Parent[]
     */
    isetallocv(n, -1, invsupernodalpermutation, _state);
    isetallocv(n, -1, supernodalpermutation, _state);
    isetallocv(n, -1, parent, _state);
    isetv(n, -1, trawparentofreorderednode, _state);
    isetv(n, 0, ttmp, _state);
    for(i=0; i<=n-1; i++)
    {
        k = trawparentofrawnode->ptr.p_int[i];
        if( k>=0 )
        {
            ttmp->ptr.p_int[k] = ttmp->ptr.p_int[k]+1;
        }
    }
    bsetv(n, ae_true, tflagarray, _state);
    sidx = 0;
    for(i=0; i<=n-1; i++)
    {
        if( tflagarray->ptr.p_bool[i] )
        {
            
            /*
             * Move column I to position SIdx, decrease unprocessed children count
             */
            supernodalpermutation->ptr.p_int[i] = sidx;
            invsupernodalpermutation->ptr.p_int[sidx] = i;
            tflagarray->ptr.p_bool[i] = ae_false;
            k = trawparentofrawnode->ptr.p_int[i];
            trawparentofreorderednode->ptr.p_int[sidx] = k;
            if( k>=0 )
            {
                unprocessedchildrencnt = ttmp->ptr.p_int[k]-1;
                ttmp->ptr.p_int[k] = unprocessedchildrencnt;
            }
            sidx = sidx+1;
            
            /*
             * Add parents (as long as parent has no unprocessed children)
             */
            while(k>=0&&unprocessedchildrencnt==0)
            {
                supernodalpermutation->ptr.p_int[k] = sidx;
                invsupernodalpermutation->ptr.p_int[sidx] = k;
                tflagarray->ptr.p_bool[k] = ae_false;
                k = trawparentofrawnode->ptr.p_int[k];
                trawparentofreorderednode->ptr.p_int[sidx] = k;
                if( k>=0 )
                {
                    unprocessedchildrencnt = ttmp->ptr.p_int[k]-1;
                    ttmp->ptr.p_int[k] = unprocessedchildrencnt;
                }
                sidx = sidx+1;
            }
        }
    }
    for(i=0; i<=n-1; i++)
    {
        k = trawparentofreorderednode->ptr.p_int[i];
        if( k>=0 )
        {
            parent->ptr.p_int[i] = supernodalpermutation->ptr.p_int[k];
        }
    }
}


/*************************************************************************
This function analyzes postordered elimination tree and creates supernodal
structure in Analysis object.

INPUT PARAMETERS
    AT          -   upper triangular CRS matrix, transpose and  reordering
                    of the original input matrix A
    Parent      -   array[N], supernodal etree
    N           -   problem size
    
    tChildrenR,
    tChildrenI,
    tParentNodeOfSupernode,
    tNode2Supernode,
    tTmp0,
    tFlagArray  -   temporary arrays, length at least N+1, simply provide
                    preallocated place.
    
OUTPUT PARAMETERS
    Analysis    -   following fields are initialized:
                    * Analysis.NSuper
                    * Analysis.SuperColRange
                    * Analysis.SuperRowRIdx
                    * Analysis.SuperRowIdx
                    * Analysis.ParentSupernode
                    * Analysis.ChildSupernodesRIdx, Analysis.ChildSupernodesIdx
                    * Analysis.OutRowCounts
                    other fields are ignored and not changed.
    Node2Supernode- array[N] that maps node indexes to supernode indexes
                    
  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void spchol_createsupernodalstructure(const sparsematrix* at,
     /* Integer */ const ae_vector* parent,
     ae_int_t n,
     spcholanalysis* analysis,
     /* Integer */ ae_vector* node2supernode,
     /* Integer */ ae_vector* tchildrenr,
     /* Integer */ ae_vector* tchildreni,
     /* Integer */ ae_vector* tparentnodeofsupernode,
     /* Integer */ ae_vector* tfakenonzeros,
     /* Integer */ ae_vector* ttmp0,
     /* Boolean */ ae_vector* tflagarray,
     ae_state *_state)
{
    ae_int_t nsuper;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t sidx;
    ae_int_t i0;
    ae_int_t ii;
    ae_int_t columnidx;
    ae_int_t nodeidx;
    ae_int_t rfirst;
    ae_int_t rlast;
    ae_int_t cols0;
    ae_int_t cols1;
    ae_int_t blocksize;
    ae_bool createsupernode;
    ae_int_t colcount;
    ae_int_t offdiagcnt;
    ae_int_t childcolcount;
    ae_int_t childoffdiagcnt;
    ae_int_t fakezerosinnewsupernode;
    double mergeinefficiency;
    ae_bool hastheonlychild;


    ae_assert(ttmp0->cnt>=n+1, "CreateSupernodalStructure: input buffer tTmp0 is too short", _state);
    ae_assert(tchildrenr->cnt>=n+1, "CreateSupernodalStructure: input buffer ChildrenR is too short", _state);
    ae_assert(tchildreni->cnt>=n+1, "CreateSupernodalStructure: input buffer ChildrenI is too short", _state);
    ae_assert(tparentnodeofsupernode->cnt>=n+1, "CreateSupernodalStructure: input buffer tParentNodeOfSupernode is too short", _state);
    ae_assert(tfakenonzeros->cnt>=n+1, "CreateSupernodalStructure: input buffer tFakeNonzeros is too short", _state);
    ae_assert(tflagarray->cnt>=n+1, "CreateSupernodalStructure: input buffer tFlagArray is too short", _state);
    
    /*
     * Trace
     */
    if( analysis->dotracesupernodalstructure )
    {
        ae_trace("=== GENERATING SUPERNODAL STRUCTURE ================================================================\n");
    }
    
    /*
     * Convert etree from per-column parent array to per-column children list
     */
    spchol_fromparenttochildren(parent, n, tchildrenr, tchildreni, ttmp0, _state);
    
    /*
     * Analyze supernodal structure:
     * * determine children count for each node
     * * combine chains of children into supernodes
     * * generate direct and inverse supernodal (topological) permutations
     * * generate column structure of supernodes (after supernodal permutation)
     */
    isetallocv(n, -1, node2supernode, _state);
    ivectorsetlengthatleast(&analysis->supercolrange, n+1, _state);
    ivectorsetlengthatleast(&analysis->superrowridx, n+1, _state);
    isetv(n, n+1, tparentnodeofsupernode, _state);
    bsetv(n, ae_true, tflagarray, _state);
    nsuper = 0;
    analysis->supercolrange.ptr.p_int[0] = 0;
    analysis->superrowridx.ptr.p_int[0] = 0;
    while(analysis->supercolrange.ptr.p_int[nsuper]<n)
    {
        columnidx = analysis->supercolrange.ptr.p_int[nsuper];
        
        /*
         * Compute nonzero pattern of the column, create temporary standalone node
         * for possible supernodal merge. Newly created node has just one column
         * and no fake nonzeros.
         */
        rfirst = analysis->superrowridx.ptr.p_int[nsuper];
        rlast = spchol_computenonzeropattern(at, columnidx, n, &analysis->superrowridx, &analysis->superrowidx, nsuper, tchildrenr, tchildreni, node2supernode, tflagarray, ttmp0, _state);
        analysis->supercolrange.ptr.p_int[nsuper+1] = columnidx+1;
        analysis->superrowridx.ptr.p_int[nsuper+1] = rlast;
        node2supernode->ptr.p_int[columnidx] = nsuper;
        tparentnodeofsupernode->ptr.p_int[nsuper] = parent->ptr.p_int[columnidx];
        tfakenonzeros->ptr.p_int[nsuper] = 0;
        offdiagcnt = rlast-rfirst;
        colcount = 1;
        nsuper = nsuper+1;
        if( analysis->dotracesupernodalstructure )
        {
            ae_trace("> incoming column %0d\n",
                (int)(columnidx));
            ae_trace("offdiagnnz = %0d\n",
                (int)(rlast-rfirst));
            ae_trace("children   = [ ");
            for(i=tchildrenr->ptr.p_int[columnidx]; i<=tchildrenr->ptr.p_int[columnidx+1]-1; i++)
            {
                ae_trace("S%0d ",
                    (int)(node2supernode->ptr.p_int[tchildreni->ptr.p_int[i]]));
            }
            ae_trace("]\n");
        }
        
        /*
         * Decide whether to merge column with previous supernode or not
         */
        childcolcount = 0;
        childoffdiagcnt = 0;
        mergeinefficiency = 0.0;
        fakezerosinnewsupernode = 0;
        createsupernode = ae_false;
        hastheonlychild = ae_false;
        if( nsuper>=2&&tparentnodeofsupernode->ptr.p_int[nsuper-2]==columnidx )
        {
            childcolcount = analysis->supercolrange.ptr.p_int[nsuper-1]-analysis->supercolrange.ptr.p_int[nsuper-2];
            childoffdiagcnt = analysis->superrowridx.ptr.p_int[nsuper-1]-analysis->superrowridx.ptr.p_int[nsuper-2];
            hastheonlychild = tchildrenr->ptr.p_int[columnidx+1]-tchildrenr->ptr.p_int[columnidx]==1;
            if( (hastheonlychild||spchol_relaxedsupernodes)&&colcount+childcolcount<=spchol_maxsupernode )
            {
                i = colcount+childcolcount;
                k = i*(i+1)/2+offdiagcnt*i;
                fakezerosinnewsupernode = tfakenonzeros->ptr.p_int[nsuper-2]+tfakenonzeros->ptr.p_int[nsuper-1]+(offdiagcnt-(childoffdiagcnt-1))*childcolcount;
                mergeinefficiency = (double)fakezerosinnewsupernode/(double)k;
                if( colcount+childcolcount==2&&fakezerosinnewsupernode<=spchol_smallfakestolerance )
                {
                    createsupernode = ae_true;
                }
                if( ae_fp_less_eq(mergeinefficiency,spchol_maxmergeinefficiency) )
                {
                    createsupernode = ae_true;
                }
            }
        }
        
        /*
         * Create supernode if needed
         */
        if( createsupernode )
        {
            
            /*
             * Create supernode from nodes NSuper-2 and NSuper-1.
             * Because these nodes are in the child-parent relation, we can simply
             * copy nonzero pattern from NSuper-1.
             */
            ae_assert(tparentnodeofsupernode->ptr.p_int[nsuper-2]==columnidx, "CreateSupernodalStructure: integrity check 9472 failed", _state);
            i0 = analysis->superrowridx.ptr.p_int[nsuper-1];
            ii = analysis->superrowridx.ptr.p_int[nsuper]-analysis->superrowridx.ptr.p_int[nsuper-1];
            rfirst = analysis->superrowridx.ptr.p_int[nsuper-2];
            rlast = rfirst+ii;
            for(i=0; i<=ii-1; i++)
            {
                analysis->superrowidx.ptr.p_int[rfirst+i] = analysis->superrowidx.ptr.p_int[i0+i];
            }
            analysis->supercolrange.ptr.p_int[nsuper-1] = columnidx+1;
            analysis->superrowridx.ptr.p_int[nsuper-1] = rlast;
            node2supernode->ptr.p_int[columnidx] = nsuper-2;
            tfakenonzeros->ptr.p_int[nsuper-2] = fakezerosinnewsupernode;
            tparentnodeofsupernode->ptr.p_int[nsuper-2] = parent->ptr.p_int[columnidx];
            nsuper = nsuper-1;
            
            /*
             * Trace
             */
            if( analysis->dotracesupernodalstructure )
            {
                ae_trace("> merged with supernode S%0d",
                    (int)(nsuper-1));
                if( ae_fp_neq(mergeinefficiency,(double)(0)) )
                {
                    ae_trace(" (%2.0f%% inefficiency)",
                        (double)(mergeinefficiency*(double)100));
                }
                ae_trace("\n*\n");
            }
        }
        else
        {
            
            /*
             * Trace
             */
            if( analysis->dotracesupernodalstructure )
            {
                ae_trace("> standalone node S%0d created\n*\n",
                    (int)(nsuper-1));
            }
        }
    }
    analysis->nsuper = nsuper;
    ae_assert(analysis->nsuper>=1, "SPSymmAnalyze: integrity check failed (95mgd)", _state);
    ae_assert(analysis->supercolrange.ptr.p_int[0]==0, "SPCholFactorize: integrity check failed (f446s)", _state);
    ae_assert(analysis->supercolrange.ptr.p_int[nsuper]==n, "SPSymmAnalyze: integrity check failed (04ut4)", _state);
    isetallocv(nsuper, -1, &analysis->parentsupernode, _state);
    for(sidx=0; sidx<=nsuper-1; sidx++)
    {
        nodeidx = tparentnodeofsupernode->ptr.p_int[sidx];
        if( nodeidx>=0 )
        {
            nodeidx = node2supernode->ptr.p_int[nodeidx];
            analysis->parentsupernode.ptr.p_int[sidx] = nodeidx;
        }
    }
    iallocv(nsuper+2, &analysis->childsupernodesridx, _state);
    iallocv(nsuper+1, &analysis->childsupernodesidx, _state);
    spchol_fromparenttochildren(&analysis->parentsupernode, nsuper, &analysis->childsupernodesridx, &analysis->childsupernodesidx, ttmp0, _state);
    i = analysis->childsupernodesridx.ptr.p_int[nsuper];
    for(sidx=0; sidx<=nsuper-1; sidx++)
    {
        j = analysis->parentsupernode.ptr.p_int[sidx];
        if( j<0 )
        {
            analysis->childsupernodesidx.ptr.p_int[i] = sidx;
            i = i+1;
        }
    }
    ae_assert(i==nsuper, "SPSymmAnalyze: integrity check 4dr5 failed", _state);
    analysis->childsupernodesridx.ptr.p_int[nsuper+1] = i;
    
    /*
     * Allocate supernodal storage
     */
    ivectorsetlengthatleast(&analysis->rowoffsets, analysis->nsuper+1, _state);
    ivectorsetlengthatleast(&analysis->rowstrides, analysis->nsuper, _state);
    analysis->rowoffsets.ptr.p_int[0] = 0;
    for(i=0; i<=analysis->nsuper-1; i++)
    {
        blocksize = analysis->supercolrange.ptr.p_int[i+1]-analysis->supercolrange.ptr.p_int[i];
        analysis->rowstrides.ptr.p_int[i] = spchol_recommendedstridefor(blocksize, _state);
        analysis->rowoffsets.ptr.p_int[i+1] = analysis->rowoffsets.ptr.p_int[i];
        analysis->rowoffsets.ptr.p_int[i+1] = analysis->rowoffsets.ptr.p_int[i+1]+analysis->rowstrides.ptr.p_int[i]*blocksize;
        analysis->rowoffsets.ptr.p_int[i+1] = analysis->rowoffsets.ptr.p_int[i+1]+analysis->rowstrides.ptr.p_int[i]*(analysis->superrowridx.ptr.p_int[i+1]-analysis->superrowridx.ptr.p_int[i]);
        analysis->rowoffsets.ptr.p_int[i+1] = spchol_alignpositioninarray(analysis->rowoffsets.ptr.p_int[i+1], _state);
    }
    
    /*
     * Analyze output structure
     */
    isetallocv(n, 0, &analysis->outrowcounts, _state);
    for(sidx=0; sidx<=nsuper-1; sidx++)
    {
        cols0 = analysis->supercolrange.ptr.p_int[sidx];
        cols1 = analysis->supercolrange.ptr.p_int[sidx+1];
        rfirst = analysis->superrowridx.ptr.p_int[sidx];
        rlast = analysis->superrowridx.ptr.p_int[sidx+1];
        blocksize = cols1-cols0;
        for(j=cols0; j<=cols1-1; j++)
        {
            analysis->outrowcounts.ptr.p_int[j] = analysis->outrowcounts.ptr.p_int[j]+(j-cols0+1);
        }
        for(ii=rfirst; ii<=rlast-1; ii++)
        {
            i0 = analysis->superrowidx.ptr.p_int[ii];
            analysis->outrowcounts.ptr.p_int[i0] = analysis->outrowcounts.ptr.p_int[i0]+blocksize;
        }
    }
}


/*************************************************************************
This function analyzes supernodal  structure  and  precomputes  dependency
matrix LAdj+

INPUT PARAMETERS
    Analysis    -   analysis object with completely initialized supernodal
                    structure
    RawA        -   original (before reordering) input matrix
    Node2Supernode- mapping from node to supernode indexes
    N           -   problem size
    
    tTmp0,
    tTmp1,
    tFlagArray  -   temporary arrays, length at least N+1, simply provide
                    preallocated place.
    
OUTPUT PARAMETERS
    Analysis    -   Analysis.LAdj is initialized
    Node2Supernode- array[N] that maps node indexes to supernode indexes
                    
  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void spchol_analyzesupernodaldependencies(spcholanalysis* analysis,
     const sparsematrix* rawa,
     /* Integer */ const ae_vector* node2supernode,
     ae_int_t n,
     /* Integer */ ae_vector* ttmp0,
     /* Integer */ ae_vector* ttmp1,
     /* Boolean */ ae_vector* tflagarray,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t rowidx;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t jj;
    ae_int_t rfirst;
    ae_int_t rlast;
    ae_int_t sidx;
    ae_int_t uidx;
    ae_int_t ladjcnt;
    ae_int_t dbgnzl;
    ae_int_t dbgrank1nodes;
    ae_int_t dbgrank2nodes;
    ae_int_t dbgrank3nodes;
    ae_int_t dbgrank4nodes;
    ae_int_t dbgbignodes;
    double dbgtotalflop;
    double dbgnoscatterflop;
    double dbgnorowscatterflop;
    double dbgnocolscatterflop;
    double dbgcholeskyflop;
    double dbgcholesky4flop;
    double dbgrank1flop;
    double dbgrank4plusflop;
    double dbg444flop;
    double dbgxx4flop;
    double uflop;
    double sflop;
    ae_int_t wrkrow;
    ae_int_t offdiagrow;
    ae_int_t lastrow;
    ae_int_t uwidth;
    ae_int_t uheight;
    ae_int_t urank;
    ae_int_t theight;
    ae_int_t twidth;
    ae_vector dbgfastestpath;

    ae_frame_make(_state, &_frame_block);
    memset(&dbgfastestpath, 0, sizeof(dbgfastestpath));
    ae_vector_init(&dbgfastestpath, 0, DT_REAL, _state, ae_true);

    ae_assert(ttmp0->cnt>=n+1, "AnalyzeSupernodalDependencies: input buffer tTmp0 is too short", _state);
    ae_assert(ttmp1->cnt>=n+1, "AnalyzeSupernodalDependencies: input buffer tTmp1 is too short", _state);
    ae_assert(tflagarray->cnt>=n+1, "AnalyzeSupernodalDependencies: input buffer tTmp0 is too short", _state);
    ae_assert(sparseiscrs(rawa, _state), "AnalyzeSupernodalDependencies: RawA must be CRS matrix", _state);
    
    /*
     * Determine LAdj - supernodes feeding updates to the SIdx-th one.
     *
     * Without supernodes we have: K-th row of L (also denoted as ladj+(K))
     * includes original nonzeros from A (also denoted as ladj(K)) as well
     * as all elements on paths in elimination tree from ladj(K) to K.
     *
     * With supernodes: same principle applied.
     */
    isetallocv(analysis->nsuper, 0, &analysis->ladj.rowbegin, _state);
    isetallocv(analysis->nsuper, 0, &analysis->ladj.rowend, _state);
    rsetallocv(analysis->nsuper, (double)(0), &analysis->ladj.nflop, _state);
    if( analysis->dotrace )
    {
        rsetallocv(analysis->nsuper, (double)(0), &dbgfastestpath, _state);
    }
    bsetv(n, ae_true, tflagarray, _state);
    icopyv(analysis->nsuper, &analysis->superrowridx, ttmp0, _state);
    ladjcnt = 0;
    for(sidx=0; sidx<=analysis->nsuper-1; sidx++)
    {
        
        /*
         * Generate ordered list of nodes feeding updates to SIdx-th one
         */
        igrowv(ladjcnt+analysis->nsuper, &analysis->ladj.idx, _state);
        igrowv(ladjcnt+analysis->nsuper, &analysis->ladj.urow0, _state);
        igrowv(ladjcnt+analysis->nsuper, &analysis->ladj.uwidth, _state);
        rgrowv(ladjcnt+analysis->nsuper, &analysis->ladj.uflop, _state);
        rfirst = ladjcnt;
        rlast = rfirst;
        analysis->ladj.rowbegin.ptr.p_int[sidx] = rfirst;
        for(rowidx=analysis->supercolrange.ptr.p_int[sidx]; rowidx<=analysis->supercolrange.ptr.p_int[sidx+1]-1; rowidx++)
        {
            i = analysis->invsuperperm.ptr.p_int[rowidx];
            j0 = rawa->ridx.ptr.p_int[i];
            j1 = rawa->uidx.ptr.p_int[i]-1;
            for(jj=j0; jj<=j1; jj++)
            {
                j = node2supernode->ptr.p_int[analysis->superperm.ptr.p_int[rawa->idx.ptr.p_int[jj]]];
                
                /*
                 * add supernode and its parents up the chain
                 */
                while((j>=0&&j<sidx)&&tflagarray->ptr.p_bool[j])
                {
                    analysis->ladj.idx.ptr.p_int[rlast] = j;
                    tflagarray->ptr.p_bool[j] = ae_false;
                    rlast = rlast+1;
                    j = analysis->parentsupernode.ptr.p_int[j];
                }
            }
        }
        sortmiddlei(&analysis->ladj.idx, rfirst, rlast-rfirst, _state);
        
        /*
         * Compute update-related information
         */
        sflop = (double)(0);
        twidth = analysis->supercolrange.ptr.p_int[sidx+1]-analysis->supercolrange.ptr.p_int[sidx];
        theight = twidth+(analysis->superrowridx.ptr.p_int[sidx+1]-analysis->superrowridx.ptr.p_int[sidx]);
        for(i=rfirst; i<=rlast-1; i++)
        {
            j = analysis->ladj.idx.ptr.p_int[i];
            wrkrow = ttmp0->ptr.p_int[j];
            offdiagrow = wrkrow;
            lastrow = analysis->superrowridx.ptr.p_int[j+1];
            while(offdiagrow<lastrow&&analysis->superrowidx.ptr.p_int[offdiagrow]<analysis->supercolrange.ptr.p_int[sidx+1])
            {
                offdiagrow = offdiagrow+1;
            }
            uflop = (double)((offdiagrow-wrkrow)*(lastrow-wrkrow)*(analysis->supercolrange.ptr.p_int[j+1]-analysis->supercolrange.ptr.p_int[j]));
            analysis->ladj.urow0.ptr.p_int[i] = wrkrow;
            analysis->ladj.uwidth.ptr.p_int[i] = offdiagrow-wrkrow;
            analysis->ladj.uflop.ptr.p_double[i] = uflop;
            sflop = sflop+uflop;
            ttmp0->ptr.p_int[j] = offdiagrow;
        }
        for(i=0; i<=twidth-1; i++)
        {
            sflop = sflop+(double)((theight-i)*(twidth-i));
        }
        analysis->ladj.nflop.ptr.p_double[sidx] = sflop;
        j = analysis->parentsupernode.ptr.p_int[sidx];
        if( analysis->dotrace&&j>=0 )
        {
            dbgfastestpath.ptr.p_double[j] = ae_maxreal(dbgfastestpath.ptr.p_double[j], sflop+dbgfastestpath.ptr.p_double[sidx], _state);
        }
        
        /*
         * Finalize
         */
        for(i=rfirst; i<=rlast-1; i++)
        {
            tflagarray->ptr.p_bool[analysis->ladj.idx.ptr.p_int[i]] = ae_true;
        }
        analysis->ladj.rowend.ptr.p_int[sidx] = rlast;
        ladjcnt = rlast;
    }
    rcopyallocv(analysis->nsuper, &analysis->ladj.nflop, &analysis->ladj.sflop, _state);
    for(sidx=0; sidx<=analysis->nsuper-1; sidx++)
    {
        j = analysis->parentsupernode.ptr.p_int[sidx];
        if( j>=0 )
        {
            analysis->ladj.sflop.ptr.p_double[j] = analysis->ladj.sflop.ptr.p_double[j]+analysis->ladj.sflop.ptr.p_double[sidx];
        }
    }
    if( analysis->dotrace )
    {
        for(sidx=0; sidx<=analysis->nsuper-1; sidx++)
        {
            dbgfastestpath.ptr.p_double[sidx] = dbgfastestpath.ptr.p_double[sidx]+analysis->ladj.nflop.ptr.p_double[sidx];
        }
    }
    
    /*
     * Analyze statistics for trace output
     */
    if( analysis->dotrace )
    {
        ae_trace("=== ANALYZING SUPERNODAL DEPENDENCIES ==============================================================\n");
        dbgnzl = 0;
        dbgrank1nodes = 0;
        dbgrank2nodes = 0;
        dbgrank3nodes = 0;
        dbgrank4nodes = 0;
        dbgbignodes = 0;
        dbgtotalflop = (double)(0);
        dbgnoscatterflop = (double)(0);
        dbgnorowscatterflop = (double)(0);
        dbgnocolscatterflop = (double)(0);
        dbgrank1flop = (double)(0);
        dbgrank4plusflop = (double)(0);
        dbg444flop = (double)(0);
        dbgxx4flop = (double)(0);
        dbgcholeskyflop = (double)(0);
        dbgcholesky4flop = (double)(0);
        isetv(analysis->nsuper, 0, ttmp0, _state);
        for(sidx=0; sidx<=analysis->nsuper-1; sidx++)
        {
            
            /*
             * Node sizes
             */
            if( analysis->supercolrange.ptr.p_int[sidx+1]-analysis->supercolrange.ptr.p_int[sidx]==1 )
            {
                inc(&dbgrank1nodes, _state);
            }
            if( analysis->supercolrange.ptr.p_int[sidx+1]-analysis->supercolrange.ptr.p_int[sidx]==2 )
            {
                inc(&dbgrank2nodes, _state);
            }
            if( analysis->supercolrange.ptr.p_int[sidx+1]-analysis->supercolrange.ptr.p_int[sidx]==3 )
            {
                inc(&dbgrank3nodes, _state);
            }
            if( analysis->supercolrange.ptr.p_int[sidx+1]-analysis->supercolrange.ptr.p_int[sidx]==4 )
            {
                inc(&dbgrank4nodes, _state);
            }
            if( analysis->supercolrange.ptr.p_int[sidx+1]-analysis->supercolrange.ptr.p_int[sidx]>4 )
            {
                inc(&dbgbignodes, _state);
            }
            
            /*
             * Nonzeros and FLOP counts
             */
            twidth = analysis->supercolrange.ptr.p_int[sidx+1]-analysis->supercolrange.ptr.p_int[sidx];
            theight = twidth+(analysis->superrowridx.ptr.p_int[sidx+1]-analysis->superrowridx.ptr.p_int[sidx]);
            dbgnzl = dbgnzl+theight*twidth-twidth*(twidth-1)/2;
            for(i=analysis->ladj.rowbegin.ptr.p_int[sidx]; i<=analysis->ladj.rowend.ptr.p_int[sidx]-1; i++)
            {
                
                /*
                 * Determine update width, height, rank
                 */
                uidx = analysis->ladj.idx.ptr.p_int[i];
                uwidth = analysis->ladj.uwidth.ptr.p_int[i];
                uheight = analysis->superrowridx.ptr.p_int[uidx+1]-analysis->ladj.urow0.ptr.p_int[i];
                urank = analysis->supercolrange.ptr.p_int[uidx+1]-analysis->supercolrange.ptr.p_int[uidx];
                
                /*
                 * Compute update FLOP cost
                 */
                uflop = analysis->ladj.uflop.ptr.p_double[i];
                dbgtotalflop = dbgtotalflop+uflop;
                if( uheight==theight&&uwidth==twidth )
                {
                    dbgnoscatterflop = dbgnoscatterflop+uflop;
                }
                if( uheight==theight )
                {
                    dbgnorowscatterflop = dbgnorowscatterflop+uflop;
                }
                if( uwidth==twidth )
                {
                    dbgnocolscatterflop = dbgnocolscatterflop+uflop;
                }
                if( urank==1 )
                {
                    dbgrank1flop = dbgrank1flop+uflop;
                }
                if( urank>=4 )
                {
                    dbgrank4plusflop = dbgrank4plusflop+uflop;
                }
                if( (urank==4&&uwidth==4)&&twidth==4 )
                {
                    dbg444flop = dbg444flop+uflop;
                }
                if( twidth==4 )
                {
                    dbgxx4flop = dbgxx4flop+uflop;
                }
            }
            uflop = (double)(0);
            for(i=0; i<=twidth-1; i++)
            {
                uflop = uflop+(double)((theight-i)*i)+(double)(theight-i);
            }
            dbgtotalflop = dbgtotalflop+uflop;
            dbgcholeskyflop = dbgcholeskyflop+uflop;
            if( twidth==4 )
            {
                dbgcholesky4flop = dbgcholesky4flop+uflop;
            }
        }
        
        /*
         * Output
         */
        ae_trace("> factor size:\n");
        ae_trace("nz(L)        = %6d\n",
            (int)(dbgnzl));
        ae_trace("> node size statistics:\n");
        ae_trace("rank1        = %6d\n",
            (int)(dbgrank1nodes));
        ae_trace("rank2        = %6d\n",
            (int)(dbgrank2nodes));
        ae_trace("rank3        = %6d\n",
            (int)(dbgrank3nodes));
        ae_trace("rank4        = %6d\n",
            (int)(dbgrank4nodes));
        ae_trace("big nodes    = %6d\n",
            (int)(dbgbignodes));
        ae_trace("> Total FLOP count (fused multiply-adds):\n");
        ae_trace("total        = %9.1f MFLOP\n",
            (double)(1.0E-6*dbgtotalflop));
        ae_trace("> Analyzing potential parallelism speed-up (assuming infinite parallel resources):\n");
        ae_trace("etree        = %4.1fx (elimination tree parallelism, no internal parallelism)\n",
            (double)(dbgtotalflop/rmaxv(analysis->nsuper, &dbgfastestpath, _state)));
        ae_trace("> FLOP counts for updates:\n");
        ae_trace("no-sctr      = %9.1f MFLOP    (no row scatter, no col scatter, best case)\n",
            (double)(1.0E-6*dbgnoscatterflop));
        ae_trace("M4*44->N4    = %9.1f MFLOP    (no col scatter, big blocks, good case)\n",
            (double)(1.0E-6*dbg444flop));
        ae_trace("no-row-sctr  = %9.1f MFLOP    (no row scatter, good case for col-wise storage)\n",
            (double)(1.0E-6*dbgnorowscatterflop));
        ae_trace("no-col-sctr  = %9.1f MFLOP    (no col scatter, good case for row-wise storage)\n",
            (double)(1.0E-6*dbgnocolscatterflop));
        ae_trace("XX*XX->N4    = %9.1f MFLOP\n",
            (double)(1.0E-6*dbgxx4flop));
        ae_trace("rank1        = %9.1f MFLOP\n",
            (double)(1.0E-6*dbgrank1flop));
        ae_trace("rank4+       = %9.1f MFLOP\n",
            (double)(1.0E-6*dbgrank4plusflop));
        ae_trace("> FLOP counts for Cholesky:\n");
        ae_trace("cholesky     = %9.1f MFLOP\n",
            (double)(1.0E-6*dbgcholeskyflop));
        ae_trace("cholesky4    = %9.1f MFLOP\n",
            (double)(1.0E-6*dbgcholesky4flop));
    }
    ae_frame_leave(_state);
}


/*************************************************************************
This function creates block-tree structure from the supernodal elimination
tree.

INPUT PARAMETERS
    Analysis    -   analysis object with completely initialized supernodal
                    structure, including LAdj
    
OUTPUT PARAMETERS
    Analysis    -   Analysis.BlkStruct is initialized
                    Analysis.UseParallelism is set
                    
  -- ALGLIB PROJECT --
     Copyright 05.07.2022 by Bochkanov Sergey.
*************************************************************************/
static void spchol_createblockstructure(spcholanalysis* analysis,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t nheads;
    ae_int_t nrootbatches;
    ae_int_t nunprocessedheads;
    ae_int_t offs;
    ae_int_t childrenoffs;
    ae_vector heads;
    ae_vector rootbatchsizes;
    ae_vector costs;
    double minrootbatchcost;
    double smallcasecost;
    double curcost;
    double totalflops;
    double sequentialflops;
    double tmpsequentialflops;

    ae_frame_make(_state, &_frame_block);
    memset(&heads, 0, sizeof(heads));
    memset(&rootbatchsizes, 0, sizeof(rootbatchsizes));
    memset(&costs, 0, sizeof(costs));
    ae_vector_init(&heads, 0, DT_INT, _state, ae_true);
    ae_vector_init(&rootbatchsizes, 0, DT_INT, _state, ae_true);
    ae_vector_init(&costs, 0, DT_REAL, _state, ae_true);

    offs = 0;
    
    /*
     * Retrieve temporary arrays from the pool
     */
    nipoolretrieve(&analysis->n1integerpool, &heads, _state);
    nipoolretrieve(&analysis->n1integerpool, &rootbatchsizes, _state);
    nrpoolretrieve(&analysis->nrealpool, &costs, _state);
    
    /*
     * Generate list of elimination forest heads, sort them by cost decrease
     */
    nheads = 0;
    for(i=analysis->childsupernodesridx.ptr.p_int[analysis->nsuper]; i<=analysis->childsupernodesridx.ptr.p_int[analysis->nsuper+1]-1; i++)
    {
        heads.ptr.p_int[nheads] = analysis->childsupernodesidx.ptr.p_int[i];
        costs.ptr.p_double[nheads] = analysis->ladj.sflop.ptr.p_double[heads.ptr.p_int[nheads]];
        nheads = nheads+1;
    }
    ae_assert(nheads>=1, "SPChol: integrity check 4t6d failed", _state);
    rmulv(nheads, (double)(-1), &costs, _state);
    tagsortmiddleir(&heads, &costs, 0, nheads, _state);
    rmulv(nheads, (double)(-1), &costs, _state);
    
    /*
     * Determine root batches count and their sizes.
     */
    minrootbatchcost = spawnlevel(_state);
    smallcasecost = (double)(16*16*16);
    nrootbatches = 0;
    i = nheads-1;
    curcost = (double)(0);
    isetv(analysis->nsuper, 0, &rootbatchsizes, _state);
    while(i>=0)
    {
        while(i>=0)
        {
            
            /*
             * Aggregate heads until we aggregate all small heads AND total cost is greater than the spawn theshold.
             * When the debug mode is active, we randomly stop aggregating.
             */
            if( rootbatchsizes.ptr.p_int[nrootbatches]!=0 )
            {
                if( (!analysis->debugblocksupernodal&&ae_fp_greater(curcost,minrootbatchcost))&&ae_fp_greater(costs.ptr.p_double[i],smallcasecost) )
                {
                    break;
                }
                if( analysis->debugblocksupernodal&&ae_fp_greater(ae_randomreal(_state),0.5) )
                {
                    break;
                }
            }
            rootbatchsizes.ptr.p_int[nrootbatches] = rootbatchsizes.ptr.p_int[nrootbatches]+1;
            curcost = curcost+costs.ptr.p_double[i];
            i = i-1;
        }
        nrootbatches = nrootbatches+1;
        curcost = (double)(0);
    }
    
    /*
     * Output empty supernodes list
     */
    igrowv(offs+1, &analysis->blkstruct, _state);
    analysis->blkstruct.ptr.p_int[offs+0] = 0;
    offs = offs+1;
    
    /*
     * Allocate place for CHILDREN
     */
    igrowv(offs+2+nrootbatches, &analysis->blkstruct, _state);
    analysis->blkstruct.ptr.p_int[offs+0] = nrootbatches;
    analysis->blkstruct.ptr.p_int[offs+1] = 0;
    childrenoffs = offs+2;
    offs = childrenoffs+nrootbatches;
    
    /*
     * Output empty UPDATES
     */
    igrowv(offs+spchol_updatesheadersize, &analysis->blkstruct, _state);
    analysis->blkstruct.ptr.p_int[offs+0] = 2;
    analysis->blkstruct.ptr.p_int[offs+1] = 0;
    offs = offs+spchol_updatesheadersize;
    
    /*
     * For each batch of heads generate its child block
     */
    if( analysis->dotracescheduler )
    {
        ae_trace("--- printing blocked scheduler log -----------------------------------------------------------------\n");
    }
    totalflops = (double)(0);
    sequentialflops = (double)(0);
    nunprocessedheads = nheads;
    for(i=0; i<=nrootbatches-1; i++)
    {
        if( analysis->dotracescheduler )
        {
            ae_trace("> processing independent submatrix component (%0d of %0d)\n",
                (int)(i),
                (int)(nrootbatches));
        }
        tmpsequentialflops = (double)(0);
        analysis->blkstruct.ptr.p_int[childrenoffs+i] = offs;
        spchol_processbatchofheadsrec(analysis, &heads, nunprocessedheads-rootbatchsizes.ptr.p_int[i], rootbatchsizes.ptr.p_int[i], &analysis->blkstruct, &offs, &totalflops, &tmpsequentialflops, _state);
        sequentialflops = ae_maxreal(sequentialflops, tmpsequentialflops, _state);
        nunprocessedheads = nunprocessedheads-rootbatchsizes.ptr.p_int[i];
    }
    ae_assert(nunprocessedheads==0, "SPSymm: integrity check 2ff5 failed", _state);
    
    /*
     * Decide on parallelism support
     */
    analysis->useparallelism = ae_fp_greater(totalflops,smpactivationlevel(_state))&&ae_fp_greater(totalflops/(sequentialflops+(double)1),minspeedup(_state));
    
    /*
     * Trace
     */
    if( analysis->dotrace )
    {
        if( analysis->dotracescheduler )
        {
            ae_trace("--- printing blocked elimination tree --------------------------------------------------------------\n");
            spchol_printblockedeliminationtreerec(analysis, 0, 0, _state);
        }
        ae_trace("> printing scheduler report\n");
        ae_trace("FLOP count   = %9.1f MFLOP\n",
            (double)(1.0E-6*totalflops));
        ae_trace("CPUs count   = %0d\n",
            (int)(maxconcurrency(_state)));
        ae_trace("best speedup = %0.1fx (assuming infinite and fine grained parallelism resources)\n",
            (double)(totalflops/(sequentialflops+(double)1)));
        if( analysis->useparallelism )
        {
            ae_trace("parallelism  = RECOMMENDED\n");
        }
        else
        {
            ae_trace("parallelism  = NOT RECOMMENDED\n");
        }
    }
    
    /*
     * Recycle temporary arrays
     */
    nipoolrecycle(&analysis->n1integerpool, &heads, _state);
    nipoolrecycle(&analysis->n1integerpool, &rootbatchsizes, _state);
    nrpoolrecycle(&analysis->nrealpool, &costs, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function appends batch of supernodes (heads)  and  their  descendants
to the BlkStruct[] array.

INPUT PARAMETERS
    Analysis    -   analysis object with completely initialized supernodal
                    structure, including LAdj
    HeadsStack  -   array[NSuper] which is used as a stack. NHeads elements
                    starting from position StackBase store supernode indexes,
                    elements above them can be used and overwritten by this
                    function
    BlkStruct   -   array[Offs] or larger, elements up to Offs contain
                    previous blocks
    Offs        -   points to the end of BlkStruct[]
    TotalFLOPs  -   contains already accumulated FLOP count
    SequentialFLOPs-contains already accumulated FLOP count
    
OUTPUT PARAMETERS
    BlkStruct   -   elements from the former Offs (including) to the new
                    Offs (not including) contain supernodal blocks. The
                    array can be reallocated to store new data.
    Offs        -   positioned past the end of the block structure
    TotalFLOPs  -   updated with total FLOP count
    SequentialFLOPs-updated with sequential FLOP count - ones that can't
                    be parallelized; the rest can be and will be parallelized.
                    
  -- ALGLIB PROJECT --
     Copyright 05.07.2022 by Bochkanov Sergey.
*************************************************************************/
static void spchol_processbatchofheadsrec(spcholanalysis* analysis,
     /* Integer */ ae_vector* headsstack,
     ae_int_t stackbase,
     ae_int_t nheads,
     /* Integer */ ae_vector* blkstruct,
     ae_int_t* offs,
     double* totalflops,
     double* sequentialflops,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t blocksize;
    ae_int_t childrenbase;
    ae_int_t cidx;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t groupscreated;
    ae_int_t cndbigsubproblems;
    double bigsubproblemsize;
    ae_int_t blockitemslistoffs;
    ae_int_t childrenlistoffs;
    ae_int_t nchildren;
    ae_int_t updatesheaderoffs;
    double tmpsequentialflops;
    double sequentialblockflops;
    double sequentialchildrenflops;
    ae_vector isfactorized;
    ae_vector rowbegin;
    ae_vector nflop;

    ae_frame_make(_state, &_frame_block);
    memset(&isfactorized, 0, sizeof(isfactorized));
    memset(&rowbegin, 0, sizeof(rowbegin));
    memset(&nflop, 0, sizeof(nflop));
    ae_vector_init(&isfactorized, 0, DT_BOOL, _state, ae_true);
    ae_vector_init(&rowbegin, 0, DT_INT, _state, ae_true);
    ae_vector_init(&nflop, 0, DT_REAL, _state, ae_true);

    igrowv(*offs+1+analysis->nsuper+2, blkstruct, _state);
    
    /*
     * Perform breadth-first traversal of children of supernodes stored in HeadsStack[],
     * extending the list by the newly traversed nodes. When we decide that it is feasible
     * to spawn parallel subproblems we add them to HeadsStack[] starting from offset ChildrenBase.
     */
    blocksize = nheads;
    childrenbase = analysis->nsuper;
    i = stackbase;
    while(i<stackbase+blocksize)
    {
        j0 = analysis->childsupernodesridx.ptr.p_int[headsstack->ptr.p_int[i]];
        j1 = analysis->childsupernodesridx.ptr.p_int[headsstack->ptr.p_int[i]+1]-1;
        
        /*
         * Quick processing for a supernode with only one child: add to the block
         */
        if( j0==j1 )
        {
            headsstack->ptr.p_int[stackbase+blocksize] = analysis->childsupernodesidx.ptr.p_int[j0];
            blocksize = blocksize+1;
            i = i+1;
            continue;
        }
        
        /*
         * More than one child.
         * Count big subproblems.
         * When debug mode is activated, we randomly decide on subproblem size
         */
        cndbigsubproblems = 0;
        bigsubproblemsize = spawnlevel(_state);
        if( analysis->debugblocksupernodal&&ae_fp_greater(ae_randomreal(_state),0.5) )
        {
            bigsubproblemsize = (double)(-1);
        }
        for(j=analysis->childsupernodesridx.ptr.p_int[headsstack->ptr.p_int[i]]; j<=analysis->childsupernodesridx.ptr.p_int[headsstack->ptr.p_int[i]+1]-1; j++)
        {
            if( ae_fp_greater_eq(analysis->ladj.sflop.ptr.p_double[analysis->childsupernodesidx.ptr.p_int[j]],bigsubproblemsize) )
            {
                cndbigsubproblems = cndbigsubproblems+1;
            }
        }
        
        /*
         * Analyze child nodes
         */
        for(j=j0; j<=j1; j++)
        {
            if( cndbigsubproblems>=2&&ae_fp_greater_eq(analysis->ladj.sflop.ptr.p_double[analysis->childsupernodesidx.ptr.p_int[j]],bigsubproblemsize) )
            {
                
                /*
                 * Supernode has more than one child that is big enough to parallelize computations.
                 * Move big childs to a separate list.
                 */
                childrenbase = childrenbase-1;
                headsstack->ptr.p_int[childrenbase] = analysis->childsupernodesidx.ptr.p_int[j];
            }
            else
            {
                
                /*
                 * Either child is too small or we have only one big child (i.e. we follow elimination tree trunk).
                 * Sequential processing, add supernode to the block.
                 */
                headsstack->ptr.p_int[stackbase+blocksize] = analysis->childsupernodesidx.ptr.p_int[j];
                blocksize = blocksize+1;
            }
        }
        i = i+1;
    }
    ae_assert(stackbase+blocksize<=childrenbase, "SPSymm: integrity check 4fb6 failed", _state);
    ae_assert(childrenbase<=analysis->nsuper, "SPSymm: integrity check 4fb7 failed", _state);
    
    /*
     * Output SUPERNODES list.
     */
    blkstruct->ptr.p_int[*offs+0] = blocksize;
    blockitemslistoffs = *offs+1;
    for(i=0; i<=blocksize-1; i++)
    {
        blkstruct->ptr.p_int[blockitemslistoffs+i] = headsstack->ptr.p_int[stackbase+i];
    }
    sortmiddlei(blkstruct, blockitemslistoffs, blocksize, _state);
    *offs = blockitemslistoffs+blocksize;
    
    /*
     * Output CHILDREN list, temporarily store children supernode indexes instead of their offsets in BlkStruct[]
     */
    nchildren = analysis->nsuper-childrenbase;
    childrenlistoffs = *offs+2;
    blkstruct->ptr.p_int[*offs+0] = nchildren;
    blkstruct->ptr.p_int[*offs+1] = 0;
    for(i=0; i<=nchildren-1; i++)
    {
        blkstruct->ptr.p_int[childrenlistoffs+i] = headsstack->ptr.p_int[childrenbase+i];
    }
    *offs = *offs+2+nchildren;
    
    /*
     * Output UPDATES part
     */
    if( analysis->dotracescheduler )
    {
        ae_trace(">> running scheduler for a block of %0d supernodes\n",
            (int)(blocksize));
    }
    nbpoolretrieve(&analysis->n1booleanpool, &isfactorized, _state);
    nipoolretrieve(&analysis->n1integerpool, &rowbegin, _state);
    nrpoolretrieve(&analysis->nrealpool, &nflop, _state);
    bsetv(analysis->nsuper, ae_false, &isfactorized, _state);
    icopyv(analysis->nsuper, &analysis->ladj.rowbegin, &rowbegin, _state);
    rcopyv(analysis->nsuper, &analysis->ladj.nflop, &nflop, _state);
    groupscreated = 0;
    updatesheaderoffs = *offs;
    igrowv(updatesheaderoffs+spchol_updatesheadersize, blkstruct, _state);
    *offs = updatesheaderoffs+spchol_updatesheadersize;
    sequentialblockflops = (double)(0);
    spchol_scheduleupdatesforablockrec(analysis, &rowbegin, &isfactorized, &nflop, blkstruct, blockitemslistoffs, blocksize, 0, offs, &groupscreated, totalflops, &sequentialblockflops, _state);
    blkstruct->ptr.p_int[updatesheaderoffs+0] = *offs-updatesheaderoffs;
    blkstruct->ptr.p_int[updatesheaderoffs+1] = groupscreated;
    nbpoolrecycle(&analysis->n1booleanpool, &isfactorized, _state);
    nipoolrecycle(&analysis->n1integerpool, &rowbegin, _state);
    nrpoolrecycle(&analysis->nrealpool, &nflop, _state);
    
    /*
     * Recursively process children, replace supernode indexes by their offsets in BlkStruct[]
     */
    sequentialchildrenflops = (double)(0);
    for(i=0; i<=nchildren-1; i++)
    {
        cidx = blkstruct->ptr.p_int[childrenlistoffs+i];
        blkstruct->ptr.p_int[childrenlistoffs+i] = *offs;
        headsstack->ptr.p_int[stackbase+blocksize] = cidx;
        tmpsequentialflops = (double)(0);
        spchol_processbatchofheadsrec(analysis, headsstack, stackbase+blocksize, 1, blkstruct, offs, totalflops, &tmpsequentialflops, _state);
        sequentialchildrenflops = ae_maxreal(sequentialchildrenflops, tmpsequentialflops, _state);
    }
    
    /*
     * Compute final sequential FLOPs
     */
    *sequentialflops = sequentialblockflops+sequentialchildrenflops;
    ae_frame_leave(_state);
}


/*************************************************************************
This function appends a set of updates for a block of supernodes stored in
the BlkStruct[] array.

INPUT PARAMETERS
    Analysis    -   analysis object with completely initialized supernodal
                    structure, including LAdj
    RowBegin    -   on entry contains A COPY of LAdj.RowBegin[] array.
                    Modified during scheduling, so it is important to
                    provide a copy.
    IsFactorized-   array[NSuper], on entry must be set to False.
    NFLOP       -   on entry contains A COPY of LAdj.NFLOP[] array.
                    Modified during scheduling, so it is important to
                    provide a copy.
    BlkStruct   -   array[Offs] or larger, elements up to Offs contain
                    previous blocks
    BlockItemsOffs- offset in BlkStruct[] where block items are stored
                    (items must be sorted by ascending)
    BlockSize   -   number of supernodes in the block
    Depth       -   zero on the topmost call
    Offs        -   points to the end of BlkStruct[]
    GroupsCreated-  must be set to zero on entry
    TotalFLOPs  -   contains already accumulated FLOP count
    SequentialFLOPs-contains already accumulated FLOP count
    
OUTPUT PARAMETERS
    BlkStruct   -   elements from the former Offs (including) to the new
                    Offs (not including) contain update information. The
                    array can be reallocated to store new data.
    Offs        -   positioned past the end of the block structure
    GroupsCreated-  increased by count of newly created update groups
    TotalFLOPs  -   updated with total FLOP count
    SequentialFLOPs-updated with sequential FLOP count - ones that can't
                    be parallelized; the rest can be and will be parallelized.
                    
  -- ALGLIB PROJECT --
     Copyright 05.07.2022 by Bochkanov Sergey.
*************************************************************************/
static void spchol_scheduleupdatesforablockrec(const spcholanalysis* analysis,
     /* Integer */ ae_vector* rowbegin,
     /* Boolean */ ae_vector* isfactorized,
     /* Real    */ ae_vector* nflop,
     /* Integer */ ae_vector* blkstruct,
     ae_int_t blockitemsoffs,
     ae_int_t blocksize,
     ae_int_t depth,
     ae_int_t* offs,
     ae_int_t* groupscreated,
     double* totalflops,
     double* sequentialflops,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;
    ae_int_t kmid;
    ae_int_t sidx;
    ae_int_t batchesheaderoffs;
    ae_int_t groupsheaderoffs;
    ae_int_t updatesissued;
    ae_int_t batchesissued;
    ae_bool isbasecase;
    double residualcost;
    double leftcost;
    double updcost;
    double raw2scost;
    double batchcost;
    double minbatchcost;
    ae_int_t repsupernodesupdated;


    
    /*
     * Initial evaluation of the block - should we divide it into two
     * smaller subblocks A and B and schedule parallel update A-to-B
     * or process it as single factorization?
     */
    residualcost = (double)(0);
    for(k=0; k<=blocksize-1; k++)
    {
        sidx = blkstruct->ptr.p_int[blockitemsoffs+k];
        residualcost = residualcost+nflop->ptr.p_double[sidx];
    }
    kmid = blocksize/2;
    leftcost = (double)(0);
    for(k=0; k<=kmid-1; k++)
    {
        sidx = blkstruct->ptr.p_int[blockitemsoffs+k];
        leftcost = leftcost+nflop->ptr.p_double[sidx];
    }
    while(kmid<blocksize&&ae_fp_less(leftcost,0.05*residualcost))
    {
        sidx = blkstruct->ptr.p_int[blockitemsoffs+kmid];
        leftcost = leftcost+nflop->ptr.p_double[sidx];
        kmid = kmid+1;
    }
    raw2scost = (double)(0);
    for(k=kmid; k<=blocksize-1; k++)
    {
        sidx = blkstruct->ptr.p_int[blockitemsoffs+k];
        raw2scost = raw2scost+(double)(analysis->superrowridx.ptr.p_int[sidx+1]-analysis->superrowridx.ptr.p_int[sidx]);
    }
    repsupernodesupdated = 0;
    updcost = (double)(0);
    for(k=kmid; k<=blocksize-1; k++)
    {
        sidx = blkstruct->ptr.p_int[blockitemsoffs+k];
        i = rowbegin->ptr.p_int[sidx];
        for(;;)
        {
            if( i==analysis->ladj.rowend.ptr.p_int[sidx]||analysis->ladj.idx.ptr.p_int[i]>=blkstruct->ptr.p_int[blockitemsoffs+kmid] )
            {
                break;
            }
            updcost = updcost+analysis->ladj.uflop.ptr.p_double[i];
            i = i+1;
        }
        if( i!=rowbegin->ptr.p_int[sidx] )
        {
            repsupernodesupdated = repsupernodesupdated+1;
        }
    }
    
    /*
     * Basecase
     */
    isbasecase = ae_false;
    if( !analysis->debugblocksupernodal )
    {
        if( !isbasecase&&blocksize<spchol_smallupdate )
        {
            if( analysis->dotracescheduler )
            {
                tracespaces(depth+2, _state);
                ae_trace("* sequential block, %0d supernodes (small size)\n",
                    (int)(blocksize));
            }
            isbasecase = ae_true;
        }
        if( !isbasecase&&kmid==blocksize )
        {
            if( analysis->dotracescheduler )
            {
                tracespaces(depth+2, _state);
                ae_trace("* sequential block, %0d supernodes (unbalanced block)\n",
                    (int)(blocksize));
            }
            isbasecase = ae_true;
        }
        if( !isbasecase&&ae_fp_less(residualcost,spawnlevel(_state)) )
        {
            if( analysis->dotracescheduler )
            {
                tracespaces(depth+2, _state);
                ae_trace("* sequential block, %0d supernodes (lightweight block, %0.1f MFLOP)\n",
                    (int)(blocksize),
                    (double)(1.0E-6*residualcost));
            }
            isbasecase = ae_true;
        }
        if( !isbasecase&&ae_fp_less(updcost,spawnlevel(_state)) )
        {
            if( analysis->dotracescheduler )
            {
                tracespaces(depth+2, _state);
                ae_trace("* sequential block, %0d supernodes (lightweight update, %0.1f MFLOP; block cost is %0.1f MFLOP)\n",
                    (int)(blocksize),
                    (double)(1.0E-6*updcost),
                    (double)(1.0E-6*residualcost));
            }
            isbasecase = ae_true;
        }
        if( !isbasecase&&ae_fp_greater(raw2scost,spchol_raw2sthreshold*updcost) )
        {
            if( analysis->dotracescheduler )
            {
                tracespaces(depth+2, _state);
                ae_trace("* sequential block, %0d supernodes (splitting overhead too high, %0.1fM vs %0.1fM for an update)\n",
                    (int)(blocksize),
                    (double)(1.0E-6*raw2scost),
                    (double)(1.0E-6*updcost));
            }
            isbasecase = ae_true;
        }
    }
    if( analysis->debugblocksupernodal )
    {
        isbasecase = isbasecase||((blocksize<=1||kmid==blocksize)||ae_fp_less(ae_randomreal(_state),0.5));
    }
    if( isbasecase )
    {
        
        /*
         * Schedule sequential updates for the entire block
         */
        groupsheaderoffs = *offs;
        igrowv(groupsheaderoffs+spchol_groupheadersize, blkstruct, _state);
        blkstruct->ptr.p_int[groupsheaderoffs+0] = -1;
        blkstruct->ptr.p_int[groupsheaderoffs+1] = 1;
        *offs = groupsheaderoffs+spchol_groupheadersize;
        batchesheaderoffs = *offs;
        igrowv(batchesheaderoffs+spchol_batchheadersize+blocksize*spchol_sequenceentrysize, blkstruct, _state);
        blkstruct->ptr.p_int[batchesheaderoffs+0] = -1;
        blkstruct->ptr.p_int[batchesheaderoffs+1] = -1;
        *offs = batchesheaderoffs+spchol_batchheadersize;
        updatesissued = 0;
        for(k=0; k<=blocksize-1; k++)
        {
            sidx = blkstruct->ptr.p_int[blockitemsoffs+k];
            if( isfactorized->ptr.p_bool[sidx] )
            {
                continue;
            }
            blkstruct->ptr.p_int[*offs+0] = sidx;
            blkstruct->ptr.p_int[*offs+1] = rowbegin->ptr.p_int[sidx];
            blkstruct->ptr.p_int[*offs+2] = analysis->ladj.rowend.ptr.p_int[sidx];
            rowbegin->ptr.p_int[sidx] = analysis->ladj.rowend.ptr.p_int[sidx];
            nflop->ptr.p_double[sidx] = (double)(0);
            isfactorized->ptr.p_bool[sidx] = ae_true;
            updatesissued = updatesissued+1;
            *offs = *offs+spchol_sequenceentrysize;
        }
        blkstruct->ptr.p_int[batchesheaderoffs+0] = *offs-batchesheaderoffs;
        blkstruct->ptr.p_int[batchesheaderoffs+1] = updatesissued;
        blkstruct->ptr.p_int[groupsheaderoffs+0] = *offs-groupsheaderoffs;
        *groupscreated = *groupscreated+1;
        *totalflops = *totalflops+residualcost;
        *sequentialflops = *sequentialflops+residualcost;
        return;
    }
    
    /*
     * Separate
     */
    if( analysis->dotracescheduler )
    {
        tracespaces(depth+2, _state);
        ae_trace("* splitting %0d supernodes into %0d and %0d, update cost: %0.1f MFLOP (%0d supernodes)\n",
            (int)(blocksize),
            (int)(kmid),
            (int)(blocksize-kmid),
            (double)(updcost*1.0E-6),
            (int)(repsupernodesupdated));
    }
    spchol_scheduleupdatesforablockrec(analysis, rowbegin, isfactorized, nflop, blkstruct, blockitemsoffs, kmid, depth+1, offs, groupscreated, totalflops, sequentialflops, _state);
    groupsheaderoffs = *offs;
    igrowv(groupsheaderoffs+spchol_groupheadersize, blkstruct, _state);
    blkstruct->ptr.p_int[groupsheaderoffs+0] = -1;
    blkstruct->ptr.p_int[groupsheaderoffs+1] = -1;
    *offs = groupsheaderoffs+spchol_groupheadersize;
    batchesissued = 0;
    minbatchcost = ae_maxreal(1.01*updcost/(double)maxconcurrency(_state), spawnlevel(_state), _state);
    if( analysis->debugblocksupernodal )
    {
        minbatchcost = ae_randomreal(_state)*updcost;
    }
    k = kmid;
    while(k<blocksize)
    {
        batchcost = (double)(0);
        updatesissued = 0;
        batchesheaderoffs = *offs;
        igrowv(*offs+spchol_batchheadersize+(blocksize-kmid)*spchol_sequenceentrysize, blkstruct, _state);
        blkstruct->ptr.p_int[batchesheaderoffs+0] = -1;
        blkstruct->ptr.p_int[batchesheaderoffs+1] = -1;
        *offs = batchesheaderoffs+spchol_batchheadersize;
        while(k<blocksize&&(updatesissued==0||ae_fp_less(batchcost,minbatchcost)))
        {
            sidx = blkstruct->ptr.p_int[blockitemsoffs+k];
            if( isfactorized->ptr.p_bool[sidx] )
            {
                k = k+1;
                continue;
            }
            i = rowbegin->ptr.p_int[sidx];
            blkstruct->ptr.p_int[*offs+0] = sidx;
            blkstruct->ptr.p_int[*offs+1] = i;
            for(;;)
            {
                if( i==analysis->ladj.rowend.ptr.p_int[sidx]||analysis->ladj.idx.ptr.p_int[i]>=blkstruct->ptr.p_int[blockitemsoffs+kmid] )
                {
                    break;
                }
                nflop->ptr.p_double[sidx] = nflop->ptr.p_double[sidx]-analysis->ladj.uflop.ptr.p_double[i];
                *totalflops = *totalflops+analysis->ladj.uflop.ptr.p_double[i];
                batchcost = batchcost+analysis->ladj.uflop.ptr.p_double[i];
                i = i+1;
            }
            rowbegin->ptr.p_int[sidx] = i;
            blkstruct->ptr.p_int[*offs+2] = i;
            isfactorized->ptr.p_bool[sidx] = rowbegin->ptr.p_int[sidx]==analysis->ladj.rowend.ptr.p_int[sidx];
            if( isfactorized->ptr.p_bool[sidx] )
            {
                *totalflops = *totalflops+nflop->ptr.p_double[sidx];
                batchcost = batchcost+nflop->ptr.p_double[sidx];
                nflop->ptr.p_double[sidx] = (double)(0);
            }
            *offs = *offs+spchol_sequenceentrysize;
            updatesissued = updatesissued+1;
            k = k+1;
        }
        blkstruct->ptr.p_int[batchesheaderoffs+0] = *offs-batchesheaderoffs;
        blkstruct->ptr.p_int[batchesheaderoffs+1] = updatesissued;
        batchesissued = batchesissued+1;
    }
    blkstruct->ptr.p_int[groupsheaderoffs+0] = *offs-groupsheaderoffs;
    blkstruct->ptr.p_int[groupsheaderoffs+1] = batchesissued;
    *groupscreated = *groupscreated+1;
    spchol_scheduleupdatesforablockrec(analysis, rowbegin, isfactorized, nflop, blkstruct, blockitemsoffs+kmid, blocksize-kmid, depth+1, offs, groupscreated, totalflops, sequentialflops, _state);
}


/*************************************************************************
This function loads matrix into the supernodal storage.
                    
  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void spchol_loadmatrix(spcholanalysis* analysis,
     const sparsematrix* at,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t ii;
    ae_int_t i0;
    ae_int_t i1;
    ae_int_t n;
    ae_int_t cols0;
    ae_int_t cols1;
    ae_int_t offss;
    ae_int_t sstride;
    ae_int_t blocksize;
    ae_int_t sidx;
    ae_bool rowsizesmatch;


    n = analysis->n;
    
    /*
     * Perform quick integrity checks
     */
    rowsizesmatch = ae_true;
    for(i=0; i<=n; i++)
    {
        rowsizesmatch = rowsizesmatch&&analysis->referenceridx.ptr.p_int[i]==at->ridx.ptr.p_int[i];
    }
    ae_assert(rowsizesmatch, "LoadMatrix: sparsity patterns do not match", _state);
    
    /*
     * Load
     */
    iallocv(n, &analysis->raw2smap, _state);
    rsetallocv(analysis->rowoffsets.ptr.p_int[analysis->nsuper], 0.0, &analysis->inputstorage, _state);
    for(sidx=0; sidx<=analysis->nsuper-1; sidx++)
    {
        cols0 = analysis->supercolrange.ptr.p_int[sidx];
        cols1 = analysis->supercolrange.ptr.p_int[sidx+1];
        blocksize = cols1-cols0;
        offss = analysis->rowoffsets.ptr.p_int[sidx];
        sstride = analysis->rowstrides.ptr.p_int[sidx];
        
        /*
         * Load supernode #SIdx using Raw2SMap to perform quick transformation between global and local indexing.
         */
        for(i=cols0; i<=cols1-1; i++)
        {
            analysis->raw2smap.ptr.p_int[i] = i-cols0;
        }
        for(k=analysis->superrowridx.ptr.p_int[sidx]; k<=analysis->superrowridx.ptr.p_int[sidx+1]-1; k++)
        {
            analysis->raw2smap.ptr.p_int[analysis->superrowidx.ptr.p_int[k]] = blocksize+(k-analysis->superrowridx.ptr.p_int[sidx]);
        }
        for(j=cols0; j<=cols1-1; j++)
        {
            i0 = at->ridx.ptr.p_int[j];
            i1 = at->ridx.ptr.p_int[j+1]-1;
            for(ii=i0; ii<=i1; ii++)
            {
                analysis->inputstorage.ptr.p_double[offss+analysis->raw2smap.ptr.p_int[at->idx.ptr.p_int[ii]]*sstride+(j-cols0)] = at->vals.ptr.p_double[ii];
            }
        }
    }
}


/*************************************************************************
This function extracts computed matrix from the supernodal storage.
Depending on settings, a supernodal permutation can be applied to the matrix.

INPUT PARAMETERS
    Analysis    -   analysis object with completely initialized supernodal
                    structure
    Offsets     -   offsets for supernodal storage
    Strides     -   row strides for supernodal storage
    RowStorage  -   supernodal storage
    DiagD       -   diagonal factor
    N           -   problem size
    
    TmpP        -   preallocated temporary array[N+1]
    
OUTPUT PARAMETERS
    A           -   sparse matrix in CRS format:
                    * for PermType=0, sparse matrix in the original ordering
                      (i.e. the matrix is reordered prior to output that
                      may require considerable amount of operations due to
                      permutation being applied)
                    * for PermType=1, sparse matrix in the topological
                      ordering. The least overhead for output.
    D           -   array[N], diagonal
    P           -   output permutation in product form
                    
  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void spchol_extractmatrix(const spcholanalysis* analysis,
     /* Integer */ const ae_vector* offsets,
     /* Integer */ const ae_vector* strides,
     /* Real    */ const ae_vector* rowstorage,
     /* Real    */ const ae_vector* diagd,
     ae_int_t n,
     sparsematrix* a,
     /* Real    */ ae_vector* d,
     /* Integer */ ae_vector* p,
     /* Integer */ ae_vector* tmpp,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t sidx;
    ae_int_t i0;
    ae_int_t ii;
    ae_int_t rfirst;
    ae_int_t rlast;
    ae_int_t cols0;
    ae_int_t cols1;
    ae_int_t blocksize;
    ae_int_t rowstride;
    ae_int_t offdiagsize;
    ae_int_t offssdiag;


    ae_assert(tmpp->cnt>=n+1, "ExtractMatrix: preallocated temporary TmpP is too short", _state);
    
    /*
     * Basic initialization
     */
    a->matrixtype = 1;
    a->n = n;
    a->m = n;
    
    /*
     * Various permutation types
     */
    if( analysis->applypermutationtooutput )
    {
        ae_assert(analysis->istopologicalordering, "ExtractMatrix: critical integrity check failed (attempt to merge in nontopological permutation)", _state);
        
        /*
         * Output matrix is topologically permuted, so we return A=L*L' instead of A=P*L*L'*P'.
         * Somewhat inefficient because we have to apply permutation to L returned by supernodal code.
         */
        ivectorsetlengthatleast(&a->ridx, n+1, _state);
        ivectorsetlengthatleast(&a->didx, n, _state);
        a->ridx.ptr.p_int[0] = 0;
        for(i=0; i<=n-1; i++)
        {
            a->ridx.ptr.p_int[i+1] = a->ridx.ptr.p_int[i]+analysis->outrowcounts.ptr.p_int[analysis->effectiveperm.ptr.p_int[i]];
        }
        for(i=0; i<=n-1; i++)
        {
            a->didx.ptr.p_int[i] = a->ridx.ptr.p_int[i];
        }
        a->ninitialized = a->ridx.ptr.p_int[n];
        rvectorsetlengthatleast(&a->vals, a->ninitialized, _state);
        ivectorsetlengthatleast(&a->idx, a->ninitialized, _state);
        for(sidx=0; sidx<=analysis->nsuper-1; sidx++)
        {
            cols0 = analysis->supercolrange.ptr.p_int[sidx];
            cols1 = analysis->supercolrange.ptr.p_int[sidx+1];
            rfirst = analysis->superrowridx.ptr.p_int[sidx];
            rlast = analysis->superrowridx.ptr.p_int[sidx+1];
            blocksize = cols1-cols0;
            offdiagsize = rlast-rfirst;
            rowstride = strides->ptr.p_int[sidx];
            offssdiag = offsets->ptr.p_int[sidx];
            for(i=0; i<=blocksize-1; i++)
            {
                i0 = analysis->inveffectiveperm.ptr.p_int[cols0+i];
                ii = a->didx.ptr.p_int[i0];
                for(j=0; j<=i; j++)
                {
                    a->idx.ptr.p_int[ii] = analysis->inveffectiveperm.ptr.p_int[cols0+j];
                    a->vals.ptr.p_double[ii] = rowstorage->ptr.p_double[offssdiag+i*rowstride+j];
                    ii = ii+1;
                }
                a->didx.ptr.p_int[i0] = ii;
            }
            for(k=0; k<=offdiagsize-1; k++)
            {
                i0 = analysis->inveffectiveperm.ptr.p_int[analysis->superrowidx.ptr.p_int[k+rfirst]];
                ii = a->didx.ptr.p_int[i0];
                for(j=0; j<=blocksize-1; j++)
                {
                    a->idx.ptr.p_int[ii] = analysis->inveffectiveperm.ptr.p_int[cols0+j];
                    a->vals.ptr.p_double[ii] = rowstorage->ptr.p_double[offssdiag+(blocksize+k)*rowstride+j];
                    ii = ii+1;
                }
                a->didx.ptr.p_int[i0] = ii;
            }
        }
        for(i=0; i<=n-1; i++)
        {
            ae_assert(a->didx.ptr.p_int[i]==a->ridx.ptr.p_int[i+1], "ExtractMatrix: integrity check failed (9473t)", _state);
            tagsortmiddleir(&a->idx, &a->vals, a->ridx.ptr.p_int[i], a->ridx.ptr.p_int[i+1]-a->ridx.ptr.p_int[i], _state);
            ae_assert(a->idx.ptr.p_int[a->ridx.ptr.p_int[i+1]-1]==i, "ExtractMatrix: integrity check failed (e4tfd)", _state);
        }
        sparseinitduidx(a, _state);
        
        /*
         * Prepare D[] and P[]
         */
        rvectorsetlengthatleast(d, n, _state);
        ivectorsetlengthatleast(p, n, _state);
        for(i=0; i<=n-1; i++)
        {
            d->ptr.p_double[i] = diagd->ptr.p_double[analysis->effectiveperm.ptr.p_int[i]];
            p->ptr.p_int[i] = i;
        }
    }
    else
    {
        
        /*
         * The permutation is NOT applied to L prior to extraction,
         * we return both L and P: A=P*L*L'*P'.
         */
        ivectorsetlengthatleast(&a->ridx, n+1, _state);
        ivectorsetlengthatleast(&a->didx, n, _state);
        a->ridx.ptr.p_int[0] = 0;
        for(i=0; i<=n-1; i++)
        {
            a->ridx.ptr.p_int[i+1] = a->ridx.ptr.p_int[i]+analysis->outrowcounts.ptr.p_int[i];
        }
        for(i=0; i<=n-1; i++)
        {
            a->didx.ptr.p_int[i] = a->ridx.ptr.p_int[i];
        }
        a->ninitialized = a->ridx.ptr.p_int[n];
        rvectorsetlengthatleast(&a->vals, a->ninitialized, _state);
        ivectorsetlengthatleast(&a->idx, a->ninitialized, _state);
        for(sidx=0; sidx<=analysis->nsuper-1; sidx++)
        {
            cols0 = analysis->supercolrange.ptr.p_int[sidx];
            cols1 = analysis->supercolrange.ptr.p_int[sidx+1];
            rfirst = analysis->superrowridx.ptr.p_int[sidx];
            rlast = analysis->superrowridx.ptr.p_int[sidx+1];
            blocksize = cols1-cols0;
            offdiagsize = rlast-rfirst;
            rowstride = strides->ptr.p_int[sidx];
            offssdiag = offsets->ptr.p_int[sidx];
            for(i=0; i<=blocksize-1; i++)
            {
                i0 = cols0+i;
                ii = a->didx.ptr.p_int[i0];
                for(j=0; j<=i; j++)
                {
                    a->idx.ptr.p_int[ii] = cols0+j;
                    a->vals.ptr.p_double[ii] = rowstorage->ptr.p_double[offssdiag+i*rowstride+j];
                    ii = ii+1;
                }
                a->didx.ptr.p_int[i0] = ii;
            }
            for(k=0; k<=offdiagsize-1; k++)
            {
                i0 = analysis->superrowidx.ptr.p_int[k+rfirst];
                ii = a->didx.ptr.p_int[i0];
                for(j=0; j<=blocksize-1; j++)
                {
                    a->idx.ptr.p_int[ii] = cols0+j;
                    a->vals.ptr.p_double[ii] = rowstorage->ptr.p_double[offssdiag+(blocksize+k)*rowstride+j];
                    ii = ii+1;
                }
                a->didx.ptr.p_int[i0] = ii;
            }
        }
        for(i=0; i<=n-1; i++)
        {
            ae_assert(a->didx.ptr.p_int[i]==a->ridx.ptr.p_int[i+1], "ExtractMatrix: integrity check failed (34e43)", _state);
            ae_assert(a->idx.ptr.p_int[a->ridx.ptr.p_int[i+1]-1]==i, "ExtractMatrix: integrity check failed (k4df5)", _state);
        }
        sparseinitduidx(a, _state);
        
        /*
         * Extract diagonal
         */
        rvectorsetlengthatleast(d, n, _state);
        for(i=0; i<=n-1; i++)
        {
            d->ptr.p_double[i] = diagd->ptr.p_double[i];
        }
        
        /*
         * Convert permutation table into product form
         */
        ivectorsetlengthatleast(p, n, _state);
        for(i=0; i<=n-1; i++)
        {
            p->ptr.p_int[i] = i;
            tmpp->ptr.p_int[i] = i;
        }
        for(i=0; i<=n-1; i++)
        {
            
            /*
             * We need to move element K to position I.
             * J is where K actually stored
             */
            k = analysis->inveffectiveperm.ptr.p_int[i];
            j = tmpp->ptr.p_int[k];
            
            /*
             * Swap elements of P[I:N-1] that is used to store current locations of elements in different way
             */
            i0 = p->ptr.p_int[i];
            p->ptr.p_int[i] = p->ptr.p_int[j];
            p->ptr.p_int[j] = i0;
            
            /*
             * record pivoting of positions I and J
             */
            p->ptr.p_int[i] = j;
            tmpp->ptr.p_int[i0] = j;
        }
    }
}


/*************************************************************************
Sparisity pattern of partial Cholesky.

This function splits lower triangular L into two parts: leading HEAD  cols
and trailing TAIL*TAIL submatrix. Then it computes sparsity pattern of the
Cholesky decomposition of the HEAD, extracts bottom TAIL*HEAD update matrix
U and applies it to the tail:
    
    pattern(TAIL) += pattern(U*U')
    
The pattern(TAIL) is returned. It is important that pattern(TAIL)  is  not
the sparsity pattern of trailing Cholesky factor, it is the pattern of the
temporary matrix that will be factorized.

The sparsity pattern of HEAD is NOT returned.

INPUT PARAMETERS:
    A       -   lower triangular  matrix  A whose partial sparsity pattern
                is  needed.  Only  sparsity  structure  matters,  specific
                element values are ignored.
    Head,Tail-  sizes of the leading/traling submatrices
                
    tmpParent,
    tmpChildrenR,
    cmpChildrenI
    tmp1,
    FlagArray
            -   preallocated temporary arrays, length at least Head+Tail+1
    tmpBottomT,
    tmpUpdateT,
    tmpUpdate-  temporary sparsematrix instances; previously allocated
                space will be reused.
    
OUTPUT PARAMETERS:
    ATail   -   sparsity pattern of the lower triangular temporary  matrix
                computed prior to Cholesky factorization. Matrix  elements
                are initialized by placeholder values.

  -- ALGLIB PROJECT --
     Copyright 21.08.2021 by Bochkanov Sergey.
*************************************************************************/
static void spchol_partialcholeskypattern(const sparsematrix* a,
     ae_int_t head,
     ae_int_t tail,
     sparsematrix* atail,
     nipool* n1ipool,
     nbpool* n1bpool,
     sparsematrix* tmpbottomt,
     sparsematrix* tmpupdatet,
     sparsematrix* tmpupdate,
     sparsematrix* tmpnewtailt,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t i1;
    ae_int_t ii;
    ae_int_t j1;
    ae_int_t jj;
    ae_int_t kb;
    ae_int_t cursize;
    double v;
    ae_vector tmpparent;
    ae_vector tmpchildrenr;
    ae_vector tmpchildreni;
    ae_vector tmp1;
    ae_vector flagarray;

    ae_frame_make(_state, &_frame_block);
    memset(&tmpparent, 0, sizeof(tmpparent));
    memset(&tmpchildrenr, 0, sizeof(tmpchildrenr));
    memset(&tmpchildreni, 0, sizeof(tmpchildreni));
    memset(&tmp1, 0, sizeof(tmp1));
    memset(&flagarray, 0, sizeof(flagarray));
    ae_vector_init(&tmpparent, 0, DT_INT, _state, ae_true);
    ae_vector_init(&tmpchildrenr, 0, DT_INT, _state, ae_true);
    ae_vector_init(&tmpchildreni, 0, DT_INT, _state, ae_true);
    ae_vector_init(&tmp1, 0, DT_INT, _state, ae_true);
    ae_vector_init(&flagarray, 0, DT_BOOL, _state, ae_true);

    ae_assert(a->m==head+tail, "PartialCholeskyPattern: rows(A)!=Head+Tail", _state);
    ae_assert(a->n==head+tail, "PartialCholeskyPattern: cols(A)!=Head+Tail", _state);
    
    /*
     * Initialize and retrieve temporary arrays
     */
    nipoolretrieve(n1ipool, &tmpparent, _state);
    nipoolretrieve(n1ipool, &tmpchildrenr, _state);
    nipoolretrieve(n1ipool, &tmpchildreni, _state);
    nipoolretrieve(n1ipool, &tmp1, _state);
    nbpoolretrieve(n1bpool, &flagarray, _state);
    cursize = head+tail;
    v = (double)1/(double)cursize;
    ae_assert(tmpparent.cnt>=head+tail+1, "PartialCholeskyPattern: Length(tmpParent)<Head+Tail+1", _state);
    ae_assert(tmpchildrenr.cnt>=head+tail+1, "PartialCholeskyPattern: Length(tmpChildrenR)<Head+Tail+1", _state);
    ae_assert(tmpchildreni.cnt>=head+tail+1, "PartialCholeskyPattern: Length(tmpChildrenI)<Head+Tail+1", _state);
    ae_assert(tmp1.cnt>=head+tail+1, "PartialCholeskyPattern: Length(tmp1)<Head+Tail+1", _state);
    ae_assert(flagarray.cnt>=head+tail+1, "PartialCholeskyPattern: Length(tmp1)<Head+Tail+1", _state);
    
    /*
     * Compute leading Head columns of the Cholesky decomposition of A.
     * These columns will be used later to update sparsity pattern of the trailing
     * Tail*Tail matrix.
     *
     * Actually, we need just bottom Tail rows of these columns whose transpose (a
     * Head*Tail matrix) is stored in the tmpBottomT matrix. In order to do so in
     * the most efficient way we analyze elimination tree of the reordered matrix.
     *
     * In addition to BOTTOM matrix B we also compute an UPDATE matrix U which does
     * not include rows with duplicating sparsity patterns (only parents in the
     * elimination tree are included). Using update matrix to compute the sparsity
     * pattern is much more efficient because we do not spend time on children columns.
     *
     * NOTE: because Cholesky decomposition deals with matrix columns, we transpose
     *       A, store it into ATail, and work with transposed matrix.
     */
    sparsecopytransposecrsbuf(a, atail, _state);
    spchol_buildunorderedetree(a, cursize, &tmpparent, &tmp1, _state);
    spchol_fromparenttochildren(&tmpparent, cursize, &tmpchildrenr, &tmpchildreni, &tmp1, _state);
    tmpbottomt->m = head;
    tmpbottomt->n = tail;
    iallocv(head+1, &tmpbottomt->ridx, _state);
    tmpbottomt->ridx.ptr.p_int[0] = 0;
    tmpupdatet->m = head;
    tmpupdatet->n = tail;
    iallocv(head+1, &tmpupdatet->ridx, _state);
    tmpupdatet->ridx.ptr.p_int[0] = 0;
    bsetv(tail, ae_false, &flagarray, _state);
    for(j=0; j<=head-1; j++)
    {
        
        /*
         * Start J-th row of the tmpBottomT
         */
        kb = tmpbottomt->ridx.ptr.p_int[j];
        igrowv(kb+tail, &tmpbottomt->idx, _state);
        rgrowv(kb+tail, &tmpbottomt->vals, _state);
        
        /*
         * copy sparsity pattern J-th column of the reordered matrix
         */
        jj = atail->didx.ptr.p_int[j];
        j1 = atail->ridx.ptr.p_int[j+1]-1;
        while(jj<=j1&&atail->idx.ptr.p_int[jj]<head)
        {
            jj = jj+1;
        }
        while(jj<=j1)
        {
            i = atail->idx.ptr.p_int[jj]-head;
            tmpbottomt->idx.ptr.p_int[kb] = i;
            tmpbottomt->vals.ptr.p_double[kb] = v;
            flagarray.ptr.p_bool[i] = ae_true;
            kb = kb+1;
            jj = jj+1;
        }
        
        /*
         * Fetch sparsity pattern from the immediate children in the elimination tree
         */
        for(jj=tmpchildrenr.ptr.p_int[j]; jj<=tmpchildrenr.ptr.p_int[j+1]-1; jj++)
        {
            j1 = tmpchildreni.ptr.p_int[jj];
            ii = tmpbottomt->ridx.ptr.p_int[j1];
            i1 = tmpbottomt->ridx.ptr.p_int[j1+1]-1;
            while(ii<=i1)
            {
                i = tmpbottomt->idx.ptr.p_int[ii];
                if( !flagarray.ptr.p_bool[i] )
                {
                    tmpbottomt->idx.ptr.p_int[kb] = i;
                    tmpbottomt->vals.ptr.p_double[kb] = v;
                    flagarray.ptr.p_bool[i] = ae_true;
                    kb = kb+1;
                }
                ii = ii+1;
            }
        }
        
        /*
         * Finalize row of tmpBottomT
         */
        for(ii=tmpbottomt->ridx.ptr.p_int[j]; ii<=kb-1; ii++)
        {
            flagarray.ptr.p_bool[tmpbottomt->idx.ptr.p_int[ii]] = ae_false;
        }
        tmpbottomt->ridx.ptr.p_int[j+1] = kb;
        
        /*
         * Only columns that forward their sparsity pattern directly into the tail are added to tmpUpdateT
         */
        if( tmpparent.ptr.p_int[j]>=head )
        {
            
            /*
             * J-th column of the head forwards its sparsity pattern directly into the tail, save it to tmpUpdateT
             */
            k = tmpupdatet->ridx.ptr.p_int[j];
            igrowv(k+tail, &tmpupdatet->idx, _state);
            rgrowv(k+tail, &tmpupdatet->vals, _state);
            jj = tmpbottomt->ridx.ptr.p_int[j];
            j1 = tmpbottomt->ridx.ptr.p_int[j+1]-1;
            while(jj<=j1)
            {
                i = tmpbottomt->idx.ptr.p_int[jj];
                tmpupdatet->idx.ptr.p_int[k] = i;
                tmpupdatet->vals.ptr.p_double[k] = v;
                k = k+1;
                jj = jj+1;
            }
            tmpupdatet->ridx.ptr.p_int[j+1] = k;
        }
        else
        {
            
            /*
             * J-th column of the head forwards its sparsity pattern to another column in the head,
             * no need to save it to tmpUpdateT. Save empty row.
             */
            k = tmpupdatet->ridx.ptr.p_int[j];
            tmpupdatet->ridx.ptr.p_int[j+1] = k;
        }
    }
    sparsecreatecrsinplace(tmpupdatet, _state);
    sparsecopytransposecrsbuf(tmpupdatet, tmpupdate, _state);
    
    /*
     * Apply update U*U' to the trailing Tail*Tail matrix and generate new
     * residual matrix in tmpNewTailT. Then transpose/copy it to TmpA[].
     */
    bsetv(tail, ae_false, &flagarray, _state);
    tmpnewtailt->m = tail;
    tmpnewtailt->n = tail;
    iallocv(tail+1, &tmpnewtailt->ridx, _state);
    tmpnewtailt->ridx.ptr.p_int[0] = 0;
    for(j=0; j<=tail-1; j++)
    {
        k = tmpnewtailt->ridx.ptr.p_int[j];
        igrowv(k+tail, &tmpnewtailt->idx, _state);
        rgrowv(k+tail, &tmpnewtailt->vals, _state);
        
        /*
         * Copy row from the reordered/transposed matrix stored in TmpA
         */
        tmpnewtailt->idx.ptr.p_int[k] = j;
        tmpnewtailt->vals.ptr.p_double[k] = (double)(1);
        flagarray.ptr.p_bool[j] = ae_true;
        k = k+1;
        jj = atail->didx.ptr.p_int[head+j]+1;
        j1 = atail->ridx.ptr.p_int[head+j+1]-1;
        while(jj<=j1)
        {
            i = atail->idx.ptr.p_int[jj]-head;
            tmpnewtailt->idx.ptr.p_int[k] = i;
            tmpnewtailt->vals.ptr.p_double[k] = v;
            flagarray.ptr.p_bool[i] = ae_true;
            k = k+1;
            jj = jj+1;
        }
        
        /*
         * Apply update U*U' to J-th column of new tail (J-th row of tmpNewTailT):
         * * scan J-th row of U
         * * for each nonzero element, append corresponding row of U' (elements from J+1-th) to tmpNewTailT
         * * FlagArray[] is used to avoid duplication of nonzero elements
         */
        jj = tmpupdate->ridx.ptr.p_int[j];
        j1 = tmpupdate->ridx.ptr.p_int[j+1]-1;
        while(jj<=j1)
        {
            
            /*
             * Get row of U', skip leading elements up to J-th
             */
            ii = tmpupdatet->ridx.ptr.p_int[tmpupdate->idx.ptr.p_int[jj]];
            i1 = tmpupdatet->ridx.ptr.p_int[tmpupdate->idx.ptr.p_int[jj]+1]-1;
            while(ii<=i1&&tmpupdatet->idx.ptr.p_int[ii]<=j)
            {
                ii = ii+1;
            }
            
            /*
             * Append the rest of the row to tmpNewTailT
             */
            while(ii<=i1)
            {
                i = tmpupdatet->idx.ptr.p_int[ii];
                if( !flagarray.ptr.p_bool[i] )
                {
                    tmpnewtailt->idx.ptr.p_int[k] = i;
                    tmpnewtailt->vals.ptr.p_double[k] = v;
                    flagarray.ptr.p_bool[i] = ae_true;
                    k = k+1;
                }
                ii = ii+1;
            }
            
            /*
             * Continue or stop early (if we completely filled output buffer)
             */
            if( k-tmpnewtailt->ridx.ptr.p_int[j]==tail-j )
            {
                break;
            }
            jj = jj+1;
        }
        
        /*
         * Finalize:
         * * clean up FlagArray[]
         * * save K to RIdx[]
         */
        for(ii=tmpnewtailt->ridx.ptr.p_int[j]; ii<=k-1; ii++)
        {
            flagarray.ptr.p_bool[tmpnewtailt->idx.ptr.p_int[ii]] = ae_false;
        }
        tmpnewtailt->ridx.ptr.p_int[j+1] = k;
    }
    sparsecreatecrsinplace(tmpnewtailt, _state);
    sparsecopytransposecrsbuf(tmpnewtailt, atail, _state);
    
    /*
     * Recycle temporaries
     */
    nipoolrecycle(n1ipool, &tmpparent, _state);
    nipoolrecycle(n1ipool, &tmpchildrenr, _state);
    nipoolrecycle(n1ipool, &tmpchildreni, _state);
    nipoolrecycle(n1ipool, &tmp1, _state);
    nbpoolrecycle(n1bpool, &flagarray, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function is a specialized version of SparseSymmPermTbl()  that  takes
into   account specifics of topological reorderings (improves performance)
and additionally transposes its output.

INPUT PARAMETERS
    A           -   sparse lower triangular matrix in CRS format.
    P           -   array[N] which stores permutation table;  P[I]=J means
                    that I-th row/column of matrix  A  is  moved  to  J-th
                    position. For performance reasons we do NOT check that
                    P[] is  a   correct   permutation  (that there  is  no
                    repetitions, just that all its elements  are  in [0,N)
                    range.
    B           -   sparse matrix object that will hold output.
                    Previously allocated memory will be reused as much  as
                    possible.
    
OUTPUT PARAMETERS
    B           -   permuted and transposed upper triangular matrix in the
                    special internal CRS-like matrix format (MatrixType=-10082).

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void spchol_topologicalpermutation(const sparsematrix* a,
     /* Integer */ const ae_vector* p,
     sparsematrix* b,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t jj;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t k;
    ae_int_t k0;
    ae_int_t n;
    ae_bool bflag;


    ae_assert(a->matrixtype==1, "TopologicalPermutation: incorrect matrix type (convert your matrix to CRS)", _state);
    ae_assert(p->cnt>=a->n, "TopologicalPermutation: Length(P)<N", _state);
    ae_assert(a->m==a->n, "TopologicalPermutation: matrix is non-square", _state);
    ae_assert(a->ninitialized==a->ridx.ptr.p_int[a->n], "TopologicalPermutation: integrity check failed", _state);
    bflag = ae_true;
    n = a->n;
    for(i=0; i<=n-1; i++)
    {
        j = p->ptr.p_int[i];
        bflag = (bflag&&j>=0)&&j<n;
    }
    ae_assert(bflag, "TopologicalPermutation: P[] contains values outside of [0,N) range", _state);
    
    /*
     * Prepare output
     */
    b->matrixtype = -10082;
    b->n = n;
    b->m = n;
    ivectorsetlengthatleast(&b->didx, n, _state);
    ivectorsetlengthatleast(&b->uidx, n, _state);
    
    /*
     * Determine row sizes (temporary stored in DIdx) and ranges
     */
    isetv(n, 0, &b->uidx, _state);
    for(i=0; i<=n-1; i++)
    {
        j0 = a->ridx.ptr.p_int[i];
        j1 = a->uidx.ptr.p_int[i]-1;
        for(jj=j0; jj<=j1; jj++)
        {
            j = a->idx.ptr.p_int[jj];
            b->uidx.ptr.p_int[j] = b->uidx.ptr.p_int[j]+1;
        }
    }
    for(i=0; i<=n-1; i++)
    {
        b->didx.ptr.p_int[p->ptr.p_int[i]] = b->uidx.ptr.p_int[i];
    }
    ivectorsetlengthatleast(&b->ridx, n+1, _state);
    b->ridx.ptr.p_int[0] = 0;
    for(i=0; i<=n-1; i++)
    {
        b->ridx.ptr.p_int[i+1] = b->ridx.ptr.p_int[i]+b->didx.ptr.p_int[i];
        b->uidx.ptr.p_int[i] = b->ridx.ptr.p_int[i];
    }
    b->ninitialized = b->ridx.ptr.p_int[n];
    ivectorsetlengthatleast(&b->idx, b->ninitialized, _state);
    rvectorsetlengthatleast(&b->vals, b->ninitialized, _state);
    
    /*
     * Process matrix
     */
    for(i=0; i<=n-1; i++)
    {
        j0 = a->ridx.ptr.p_int[i];
        j1 = a->uidx.ptr.p_int[i]-1;
        k = p->ptr.p_int[i];
        for(jj=j0; jj<=j1; jj++)
        {
            j = p->ptr.p_int[a->idx.ptr.p_int[jj]];
            k0 = b->uidx.ptr.p_int[j];
            b->idx.ptr.p_int[k0] = k;
            b->vals.ptr.p_double[k0] = a->vals.ptr.p_double[jj];
            b->uidx.ptr.p_int[j] = k0+1;
        }
    }
}


/*************************************************************************
Determine nonzero pattern of the column.

This function takes as input:
* A^T - transpose of original input matrix
* index of column of L being computed
* SuperRowRIdx[] and SuperRowIdx[] - arrays that store row structure of
  supernodes, and NSuper - supernodes count
* ChildrenNodesR[], ChildrenNodesI[] - arrays that store children nodes
  for each node
* Node2Supernode[] - array that maps node indexes to supernodes
* TrueArray[] - array[N] that has all of its elements set to True (this
  invariant is preserved on output)
* Tmp0[] - array[N], temporary array

As output, it constructs nonzero pattern (diagonal element  not  included)
of  the  column #ColumnIdx on top  of  SuperRowIdx[]  array,  starting  at
location    SuperRowIdx[SuperRowRIdx[NSuper]]     and    till     location
SuperRowIdx[Result-1], where Result is a function result.

The SuperRowIdx[] array is automatically resized as needed.

It is important that this function computes nonzero pattern, but  it  does
NOT change other supernodal structures. The caller still has  to  finalize
the column (setup supernode ranges, mappings, etc).

  -- ALGLIB routine --
     20.09.2020
     Bochkanov Sergey
*************************************************************************/
static ae_int_t spchol_computenonzeropattern(const sparsematrix* wrkat,
     ae_int_t columnidx,
     ae_int_t n,
     /* Integer */ const ae_vector* superrowridx,
     /* Integer */ ae_vector* superrowidx,
     ae_int_t nsuper,
     /* Integer */ const ae_vector* childrennodesr,
     /* Integer */ const ae_vector* childrennodesi,
     /* Integer */ const ae_vector* node2supernode,
     /* Boolean */ ae_vector* truearray,
     /* Integer */ ae_vector* tmp0,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t ii;
    ae_int_t jj;
    ae_int_t i0;
    ae_int_t i1;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t cidx;
    ae_int_t rfirst;
    ae_int_t rlast;
    ae_int_t tfirst;
    ae_int_t tlast;
    ae_int_t supernodalchildrencount;
    ae_int_t result;


    ae_assert(truearray->cnt>=n, "ComputeNonzeroPattern: input temporary is too short", _state);
    ae_assert(tmp0->cnt>=n, "ComputeNonzeroPattern: input temporary is too short", _state);
    
    /*
     * Determine supernodal children in Tmp0
     */
    supernodalchildrencount = 0;
    i0 = childrennodesr->ptr.p_int[columnidx];
    i1 = childrennodesr->ptr.p_int[columnidx+1]-1;
    for(ii=i0; ii<=i1; ii++)
    {
        i = node2supernode->ptr.p_int[childrennodesi->ptr.p_int[ii]];
        if( truearray->ptr.p_bool[i] )
        {
            tmp0->ptr.p_int[supernodalchildrencount] = i;
            truearray->ptr.p_bool[i] = ae_false;
            supernodalchildrencount = supernodalchildrencount+1;
        }
    }
    for(i=0; i<=supernodalchildrencount-1; i++)
    {
        truearray->ptr.p_bool[tmp0->ptr.p_int[i]] = ae_true;
    }
    
    /*
     * Initialized column by nonzero pattern from A
     */
    rfirst = superrowridx->ptr.p_int[nsuper];
    tfirst = rfirst+n;
    igrowv(rfirst+2*n, superrowidx, _state);
    i0 = wrkat->ridx.ptr.p_int[columnidx]+1;
    i1 = wrkat->ridx.ptr.p_int[columnidx+1];
    icopyvx(i1-i0, &wrkat->idx, i0, superrowidx, rfirst, _state);
    rlast = rfirst+(i1-i0);
    
    /*
     * For column with small number of children use ordered merge algorithm.
     * For column with many children it is better to perform unsorted merge,
     * and then sort the sequence.
     */
    if( supernodalchildrencount<=4 )
    {
        
        /*
         * Ordered merge. The best approach for small number of children,
         * but may have O(N^2) running time when O(N) children are present.
         */
        for(cidx=0; cidx<=supernodalchildrencount-1; cidx++)
        {
            
            /*
             * Skip initial elements that do not contribute to subdiagonal nonzero pattern
             */
            i0 = superrowridx->ptr.p_int[tmp0->ptr.p_int[cidx]];
            i1 = superrowridx->ptr.p_int[tmp0->ptr.p_int[cidx]+1]-1;
            while(i0<=i1&&superrowidx->ptr.p_int[i0]<=columnidx)
            {
                i0 = i0+1;
            }
            j0 = rfirst;
            j1 = rlast-1;
            
            /*
             * Handle degenerate cases: empty merge target or empty merge source.
             */
            if( j1<j0 )
            {
                icopyvx(i1-i0+1, superrowidx, i0, superrowidx, rlast, _state);
                rlast = rlast+(i1-i0+1);
                continue;
            }
            if( i1<i0 )
            {
                continue;
            }
            
            /*
             * General case: two non-empty sorted sequences given by [I0,I1] and [J0,J1],
             * have to be merged and stored into [RFirst,RLast).
             */
            ii = superrowidx->ptr.p_int[i0];
            jj = superrowidx->ptr.p_int[j0];
            tlast = tfirst;
            for(;;)
            {
                if( ii<jj )
                {
                    superrowidx->ptr.p_int[tlast] = ii;
                    tlast = tlast+1;
                    i0 = i0+1;
                    if( i0>i1 )
                    {
                        break;
                    }
                    ii = superrowidx->ptr.p_int[i0];
                }
                if( jj<ii )
                {
                    superrowidx->ptr.p_int[tlast] = jj;
                    tlast = tlast+1;
                    j0 = j0+1;
                    if( j0>j1 )
                    {
                        break;
                    }
                    jj = superrowidx->ptr.p_int[j0];
                }
                if( jj==ii )
                {
                    superrowidx->ptr.p_int[tlast] = ii;
                    tlast = tlast+1;
                    i0 = i0+1;
                    j0 = j0+1;
                    if( i0>i1 )
                    {
                        break;
                    }
                    if( j0>j1 )
                    {
                        break;
                    }
                    ii = superrowidx->ptr.p_int[i0];
                    jj = superrowidx->ptr.p_int[j0];
                }
            }
            for(ii=i0; ii<=i1; ii++)
            {
                superrowidx->ptr.p_int[tlast] = superrowidx->ptr.p_int[ii];
                tlast = tlast+1;
            }
            for(jj=j0; jj<=j1; jj++)
            {
                superrowidx->ptr.p_int[tlast] = superrowidx->ptr.p_int[jj];
                tlast = tlast+1;
            }
            icopyvx(tlast-tfirst, superrowidx, tfirst, superrowidx, rfirst, _state);
            rlast = rfirst+(tlast-tfirst);
        }
        result = rlast;
    }
    else
    {
        
        /*
         * Unordered merge followed by sort. Guaranteed N*logN worst case.
         */
        for(ii=rfirst; ii<=rlast-1; ii++)
        {
            truearray->ptr.p_bool[superrowidx->ptr.p_int[ii]] = ae_false;
        }
        for(cidx=0; cidx<=supernodalchildrencount-1; cidx++)
        {
            
            /*
             * Skip initial elements that do not contribute to subdiagonal nonzero pattern
             */
            i0 = superrowridx->ptr.p_int[tmp0->ptr.p_int[cidx]];
            i1 = superrowridx->ptr.p_int[tmp0->ptr.p_int[cidx]+1]-1;
            while(i0<=i1&&superrowidx->ptr.p_int[i0]<=columnidx)
            {
                i0 = i0+1;
            }
            
            /*
             * Append elements not present in the sequence
             */
            for(ii=i0; ii<=i1; ii++)
            {
                i = superrowidx->ptr.p_int[ii];
                if( truearray->ptr.p_bool[i] )
                {
                    superrowidx->ptr.p_int[rlast] = i;
                    rlast = rlast+1;
                    truearray->ptr.p_bool[i] = ae_false;
                }
            }
        }
        for(ii=rfirst; ii<=rlast-1; ii++)
        {
            truearray->ptr.p_bool[superrowidx->ptr.p_int[ii]] = ae_true;
        }
        tagsortmiddlei(superrowidx, rfirst, rlast-rfirst, _state);
        result = rlast;
    }
    return result;
}


/*************************************************************************
Update target supernode with data from its children. This operation  is  a
supernodal equivalent of the column update by  all  preceding  cols  in  a
left-looking Cholesky.

This function applies LAdjIdx1-LAdjIdx0 updates, from LAdjidx0 to LAdjIdx1-1
from child columns. Each update has following form:

    S := S - scatter(U*D*Uc')

where
* S is an tHeight*tWidth rectangular target matrix that is:
  * stored with tStride>=tWidth in RowStorage[OffsS:OffsS+tHeight*tStride-1]
  * lower trapezoidal i.e. its leading tWidth*tWidth  submatrix  is  lower
    triangular. One may update either entire  tWidth*tWidth  submatrix  or
    just its lower part, because upper triangle is not referenced anyway.
  * the height of S is not given because it is not actually needed
* U is an uHeight*uRank rectangular update matrix tht is:
  * stored with row stride uStride>=uRank in RowStorage[OffsU:OffsU+uHeight*uStride-1].
* Uc is the leading uWidth*uRank submatrix of U
* D is uRank*uRank diagonal matrix that is:
  * stored in DiagD[OffsD:OffsD+uRank-1]
  * unit, when Analysis.UnitD=True. In this case it can be ignored, although
    DiagD still contains 1's in all of its entries
* uHeight<=tHeight, uWidth<=tWidth, so scatter operation is needed to update
  S with smaller update.
* scatter() is an operation  that  extends  smaller  uHeight*uWidth update
  matrix U*Uc' into larger tHeight*tWidth target matrix by adding zero rows
  and columns into U*Uc':
  * I-th row of update modifies Raw2SMap[SuperRowIdx[URBase+I]]-th row  of
    the matrix S
  * J-th column of update modifies Raw2SMap[SuperRowIdx[URBase+J]]-th  col
    of the matrix S

  -- ALGLIB routine --
     20.09.2020
     Bochkanov Sergey
*************************************************************************/
static void spchol_updatesupernode(spcholanalysis* analysis,
     ae_int_t sidx,
     ae_int_t cols0,
     ae_int_t cols1,
     ae_int_t offss,
     /* Integer */ const ae_vector* raw2smap,
     ae_int_t ladjidx0,
     ae_int_t ladjidx1,
     /* Real    */ const ae_vector* diagd,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t uidx;
    ae_int_t colu0;
    ae_int_t colu1;
    ae_int_t urbase;
    ae_int_t urlast;
    ae_int_t urank;
    ae_int_t uwidth;
    ae_int_t uheight;
    ae_int_t urowstride;
    ae_int_t twidth;
    ae_int_t theight;
    ae_int_t trowstride;
    ae_int_t offsu;
    ae_int_t wrkrow;
    ae_int_t ladjidx;


    twidth = cols1-cols0;
    theight = twidth+(analysis->superrowridx.ptr.p_int[sidx+1]-analysis->superrowridx.ptr.p_int[sidx]);
    trowstride = analysis->rowstrides.ptr.p_int[sidx];
    for(ladjidx=ladjidx0; ladjidx<=ladjidx1-1; ladjidx++)
    {
        uidx = analysis->ladj.idx.ptr.p_int[ladjidx];
        offsu = analysis->rowoffsets.ptr.p_int[uidx];
        colu0 = analysis->supercolrange.ptr.p_int[uidx];
        colu1 = analysis->supercolrange.ptr.p_int[uidx+1];
        urbase = analysis->superrowridx.ptr.p_int[uidx];
        urlast = analysis->superrowridx.ptr.p_int[uidx+1];
        urank = colu1-colu0;
        urowstride = analysis->rowstrides.ptr.p_int[uidx];
        wrkrow = analysis->ladj.urow0.ptr.p_int[ladjidx];
        uwidth = analysis->ladj.uwidth.ptr.p_int[ladjidx];
        uheight = urlast-wrkrow;
        if( analysis->extendeddebug )
        {
            
            /*
             * Extended integrity check (if requested)
             */
            ae_assert(uwidth>0, "SPCholFactorize: integrity check failed (44trg1)", _state);
            ae_assert(analysis->superrowidx.ptr.p_int[wrkrow]>=cols0, "SPCholFactorize: integrity check 6378 failed", _state);
            ae_assert(analysis->superrowidx.ptr.p_int[wrkrow]<cols1, "SPCholFactorize: integrity check 6729 failed", _state);
            for(i=wrkrow; i<=urlast-1; i++)
            {
                ae_assert(raw2smap->ptr.p_int[analysis->superrowidx.ptr.p_int[i]]>=0, "SPCholFactorize: integrity check failed (43t63)", _state);
            }
        }
        
        /*
         * Skip leading uRank+WrkRow rows of U because they are not used.
         */
        offsu = offsu+(urank+(wrkrow-urbase))*urowstride;
        
        /*
         * Handle special cases
         */
        if( trowstride==4 )
        {
            
            /*
             * Target is stride-4 column, try several kernels that may work with tWidth=3 and tWidth=4
             */
            if( ((uwidth==4&&twidth==4)&&urank==4)&&urowstride==4 )
            {
                if( spchol_updatekernel4444(&analysis->outputstorage, offss, theight, offsu, uheight, &analysis->diagd, colu0, raw2smap, &analysis->superrowidx, wrkrow, _state) )
                {
                    continue;
                }
            }
            if( spchol_updatekernelabc4(&analysis->outputstorage, offss, twidth, offsu, uheight, urank, urowstride, uwidth, &analysis->diagd, colu0, raw2smap, &analysis->superrowidx, wrkrow, _state) )
            {
                continue;
            }
        }
        if( urank==1&&urowstride==1 )
        {
            if( spchol_updatekernelrank1(&analysis->outputstorage, offss, twidth, trowstride, offsu, uheight, uwidth, &analysis->diagd, colu0, raw2smap, &analysis->superrowidx, wrkrow, _state) )
            {
                continue;
            }
        }
        if( urank==2&&urowstride==2 )
        {
            if( spchol_updatekernelrank2(&analysis->outputstorage, offss, twidth, trowstride, offsu, uheight, uwidth, &analysis->diagd, colu0, raw2smap, &analysis->superrowidx, wrkrow, _state) )
            {
                continue;
            }
        }
        
        /*
         * Handle general update with no specialized kernel
         */
        spchol_updatesupernodegeneric(analysis, sidx, cols0, cols1, offss, raw2smap, ladjidx, diagd, _state);
    }
}


/*************************************************************************
Generic supernode update kernel

  -- ALGLIB routine --
     20.09.2020
     Bochkanov Sergey
*************************************************************************/
static void spchol_updatesupernodegeneric(spcholanalysis* analysis,
     ae_int_t sidx,
     ae_int_t cols0,
     ae_int_t cols1,
     ae_int_t offss,
     /* Integer */ const ae_vector* raw2smap,
     ae_int_t ladjidx,
     /* Real    */ const ae_vector* diagd,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t uidx;
    ae_int_t colu0;
    ae_int_t colu1;
    ae_int_t urbase;
    ae_int_t urlast;
    ae_int_t urank;
    ae_int_t uwidth;
    ae_int_t uheight;
    ae_int_t urowstride;
    ae_int_t trowstride;
    ae_int_t targetrow;
    ae_int_t targetcol;
    ae_int_t offsu;
    ae_int_t offsd;
    ae_int_t offs0;
    ae_int_t offsj;
    ae_int_t offsk;
    double v;
    ae_int_t wrkrow;
    ae_vector u2smap;

    ae_frame_make(_state, &_frame_block);
    memset(&u2smap, 0, sizeof(u2smap));
    ae_vector_init(&u2smap, 0, DT_INT, _state, ae_true);

    uidx = analysis->ladj.idx.ptr.p_int[ladjidx];
    offsd = analysis->supercolrange.ptr.p_int[uidx];
    offsu = analysis->rowoffsets.ptr.p_int[uidx];
    colu0 = analysis->supercolrange.ptr.p_int[uidx];
    colu1 = analysis->supercolrange.ptr.p_int[uidx+1];
    urbase = analysis->superrowridx.ptr.p_int[uidx];
    urlast = analysis->superrowridx.ptr.p_int[uidx+1];
    urank = colu1-colu0;
    trowstride = analysis->rowstrides.ptr.p_int[sidx];
    urowstride = analysis->rowstrides.ptr.p_int[uidx];
    wrkrow = analysis->ladj.urow0.ptr.p_int[ladjidx];
    uwidth = analysis->ladj.uwidth.ptr.p_int[ladjidx];
    uheight = urlast-wrkrow;
    
    /*
     * Skip leading uRank+WrkRow rows of U because they are not used.
     */
    offsu = offsu+(colu1-colu0+(wrkrow-urbase))*urowstride;
    
    /*
     * Handle general update, rerefence code
     */
    nipoolretrieve(&analysis->n1integerpool, &u2smap, _state);
    ivectorsetlengthatleast(&u2smap, uheight, _state);
    for(i=0; i<=uheight-1; i++)
    {
        u2smap.ptr.p_int[i] = raw2smap->ptr.p_int[analysis->superrowidx.ptr.p_int[wrkrow+i]];
    }
    if( analysis->unitd )
    {
        
        /*
         * Unit D, vanilla Cholesky
         */
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = offss+u2smap.ptr.p_int[k]*trowstride;
            for(j=0; j<=uwidth-1; j++)
            {
                targetcol = u2smap.ptr.p_int[j];
                offsj = offsu+j*urowstride;
                offsk = offsu+k*urowstride;
                offs0 = targetrow+targetcol;
                v = analysis->outputstorage.ptr.p_double[offs0];
                for(i=0; i<=urank-1; i++)
                {
                    v = v-analysis->outputstorage.ptr.p_double[offsj+i]*analysis->outputstorage.ptr.p_double[offsk+i];
                }
                analysis->outputstorage.ptr.p_double[offs0] = v;
            }
        }
    }
    else
    {
        
        /*
         * Non-unit D, LDLT decomposition
         */
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = offss+u2smap.ptr.p_int[k]*trowstride;
            for(j=0; j<=uwidth-1; j++)
            {
                targetcol = u2smap.ptr.p_int[j];
                offsj = offsu+j*urowstride;
                offsk = offsu+k*urowstride;
                offs0 = targetrow+targetcol;
                v = analysis->outputstorage.ptr.p_double[offs0];
                for(i=0; i<=urank-1; i++)
                {
                    v = v-analysis->outputstorage.ptr.p_double[offsj+i]*diagd->ptr.p_double[offsd+i]*analysis->outputstorage.ptr.p_double[offsk+i];
                }
                analysis->outputstorage.ptr.p_double[offs0] = v;
            }
        }
    }
    nipoolrecycle(&analysis->n1integerpool, &u2smap, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Factorizes target supernode, returns True on success, False on failure.

  -- ALGLIB routine --
     20.09.2020
     Bochkanov Sergey
*************************************************************************/
static ae_bool spchol_factorizesupernode(spcholanalysis* analysis,
     ae_int_t sidx,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t cols0;
    ae_int_t cols1;
    ae_int_t offss;
    ae_int_t blocksize;
    ae_int_t offdiagsize;
    ae_int_t sstride;
    double v;
    double vs;
    double possignvraw;
    ae_bool controlpivot;
    ae_bool droppivot;
    ae_bool controloverflow;
    ae_bool result;


    cols0 = analysis->supercolrange.ptr.p_int[sidx];
    cols1 = analysis->supercolrange.ptr.p_int[sidx+1];
    offss = analysis->rowoffsets.ptr.p_int[sidx];
    blocksize = cols1-cols0;
    offdiagsize = analysis->superrowridx.ptr.p_int[sidx+1]-analysis->superrowridx.ptr.p_int[sidx];
    sstride = analysis->rowstrides.ptr.p_int[sidx];
    droppivot = analysis->modtype==2&&ae_fp_greater(analysis->modparam0,(double)(0));
    controlpivot = analysis->modtype==1&&ae_fp_greater(analysis->modparam0,(double)(0));
    controloverflow = (analysis->modtype==1||analysis->modtype==2)&&ae_fp_greater(analysis->modparam1,(double)(0));
    if( analysis->unitd )
    {
        
        /*
         * Classic Cholesky
         */
        for(j=0; j<=blocksize-1; j++)
        {
            
            /*
             * Compute J-th column
             */
            vs = (double)(0);
            for(k=j; k<=blocksize+offdiagsize-1; k++)
            {
                v = analysis->outputstorage.ptr.p_double[offss+k*sstride+j];
                for(i=0; i<=j-1; i++)
                {
                    v = v-analysis->outputstorage.ptr.p_double[offss+k*sstride+i]*analysis->outputstorage.ptr.p_double[offss+j*sstride+i];
                }
                analysis->outputstorage.ptr.p_double[offss+k*sstride+j] = v;
                vs = vs+ae_fabs(v, _state);
            }
            if( controloverflow&&vs>analysis->modparam1 )
            {
                
                /*
                 * Possible failure due to accumulation of numerical errors
                 */
                result = ae_false;
                return result;
            }
            
            /*
             * Handle pivot element
             */
            v = analysis->outputstorage.ptr.p_double[offss+j*sstride+j];
            if( controlpivot&&v<=analysis->modparam0 )
            {
                
                /*
                 * Basic modified Cholesky
                 */
                v = ae_sqrt(analysis->modparam0, _state);
                analysis->diagd.ptr.p_double[cols0+j] = 1.0;
                analysis->outputstorage.ptr.p_double[offss+j*sstride+j] = v;
                v = (double)1/v;
                for(k=j+1; k<=blocksize+offdiagsize-1; k++)
                {
                    analysis->outputstorage.ptr.p_double[offss+k*sstride+j] = v*analysis->outputstorage.ptr.p_double[offss+k*sstride+j];
                }
            }
            else
            {
                
                /*
                 * Default case
                 */
                if( v<=(double)0 )
                {
                    result = ae_false;
                    return result;
                }
                analysis->diagd.ptr.p_double[cols0+j] = 1.0;
                v = (double)1/ae_sqrt(v, _state);
                for(k=j; k<=blocksize+offdiagsize-1; k++)
                {
                    analysis->outputstorage.ptr.p_double[offss+k*sstride+j] = v*analysis->outputstorage.ptr.p_double[offss+k*sstride+j];
                }
            }
        }
    }
    else
    {
        
        /*
         * LDLT with diagonal D
         */
        for(j=0; j<=blocksize-1; j++)
        {
            
            /*
             * Compute J-th column
             */
            vs = (double)(0);
            for(k=j; k<=blocksize+offdiagsize-1; k++)
            {
                v = analysis->outputstorage.ptr.p_double[offss+k*sstride+j];
                for(i=0; i<=j-1; i++)
                {
                    v = v-analysis->outputstorage.ptr.p_double[offss+k*sstride+i]*analysis->diagd.ptr.p_double[cols0+i]*analysis->outputstorage.ptr.p_double[offss+j*sstride+i];
                }
                analysis->outputstorage.ptr.p_double[offss+k*sstride+j] = v;
                vs = vs+ae_fabs(v, _state);
            }
            if( controloverflow&&vs>analysis->modparam1 )
            {
                
                /*
                 * Possible failure due to accumulation of numerical errors
                 */
                result = ae_false;
                return result;
            }
            
            /*
             * Handle pivot element
             */
            possignvraw = possign(analysis->inputstorage.ptr.p_double[offss+j*sstride+j], _state);
            v = analysis->outputstorage.ptr.p_double[offss+j*sstride+j];
            if( controlpivot&&v/possignvraw<=analysis->modparam0 )
            {
                
                /*
                 * Basic modified LDLT
                 */
                v = possignvraw*analysis->modparam0;
                analysis->diagd.ptr.p_double[cols0+j] = v;
                analysis->outputstorage.ptr.p_double[offss+j*sstride+j] = 1.0;
                v = (double)1/v;
                for(k=j+1; k<=blocksize+offdiagsize-1; k++)
                {
                    analysis->outputstorage.ptr.p_double[offss+k*sstride+j] = v*analysis->outputstorage.ptr.p_double[offss+k*sstride+j];
                }
                continue;
            }
            if( droppivot&&v/possignvraw<=analysis->modparam0 )
            {
                
                /*
                 * Basic modified LDLT
                 */
                analysis->diagd.ptr.p_double[cols0+j] = 1.0E50;
                analysis->outputstorage.ptr.p_double[offss+j*sstride+j] = 1.0;
                for(k=j+1; k<=blocksize+offdiagsize-1; k++)
                {
                    analysis->outputstorage.ptr.p_double[offss+k*sstride+j] = 0.0;
                }
                continue;
            }
            
            /*
             * Unmodified LDLT
             */
            if( v==(double)0 )
            {
                result = ae_false;
                return result;
            }
            analysis->diagd.ptr.p_double[cols0+j] = v;
            v = (double)1/v;
            for(k=j; k<=blocksize+offdiagsize-1; k++)
            {
                analysis->outputstorage.ptr.p_double[offss+k*sstride+j] = v*analysis->outputstorage.ptr.p_double[offss+k*sstride+j];
            }
        }
    }
    result = ae_true;
    return result;
}


/*************************************************************************
This function returns recommended stride for given row size

  -- ALGLIB routine --
     20.10.2020
     Bochkanov Sergey
*************************************************************************/
static ae_int_t spchol_recommendedstridefor(ae_int_t rowsize,
     ae_state *_state)
{
    ae_int_t result;


    result = rowsize;
    if( rowsize==3 )
    {
        result = 4;
    }
    return result;
}


/*************************************************************************
This function aligns position in array in order to  better  accommodate to
SIMD specifics.

NOTE: this function aligns position measured in double precision  numbers,
      not in bits or bytes. If you want to have 256-bit aligned  position,
      round Offs to nearest multiple of 4 that is not less than Offs.

  -- ALGLIB routine --
     20.10.2020
     Bochkanov Sergey
*************************************************************************/
static ae_int_t spchol_alignpositioninarray(ae_int_t offs,
     ae_state *_state)
{
    ae_int_t result;


    result = offs;
    if( offs%4!=0 )
    {
        result = result+(4-offs%4);
    }
    return result;
}


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Fast kernels for small supernodal updates: special 4x4x4x4 function.

! See comments on UpdateSupernode() for information  on generic supernodal
! updates, including notation used below.

The generic update has following form:

    S := S - scatter(U*D*Uc')

This specialized function performs 4x4x4x4 update, i.e.:
* S is a tHeight*4 matrix
* U is a uHeight*4 matrix
* Uc' is a 4*4 matrix
* scatter() scatters rows of U*Uc', but does not scatter columns (they are
  densely packed).
  
Return value:
* True if update was applied
* False if kernel refused to perform an update.

  -- ALGLIB routine --
     20.09.2020
     Bochkanov Sergey
*************************************************************************/
static ae_bool spchol_updatekernel4444(/* Real    */ ae_vector* rowstorage,
     ae_int_t offss,
     ae_int_t sheight,
     ae_int_t offsu,
     ae_int_t uheight,
     /* Real    */ const ae_vector* diagd,
     ae_int_t offsd,
     /* Integer */ const ae_vector* raw2smap,
     /* Integer */ const ae_vector* superrowidx,
     ae_int_t urbase,
     ae_state *_state)
{
    ae_int_t k;
    ae_int_t targetrow;
    ae_int_t offsk;
    double d0;
    double d1;
    double d2;
    double d3;
    double u00;
    double u01;
    double u02;
    double u03;
    double u10;
    double u11;
    double u12;
    double u13;
    double u20;
    double u21;
    double u22;
    double u23;
    double u30;
    double u31;
    double u32;
    double u33;
    double uk0;
    double uk1;
    double uk2;
    double uk3;
    ae_bool result;


    d0 = diagd->ptr.p_double[offsd+0];
    d1 = diagd->ptr.p_double[offsd+1];
    d2 = diagd->ptr.p_double[offsd+2];
    d3 = diagd->ptr.p_double[offsd+3];
    u00 = d0*rowstorage->ptr.p_double[offsu+0*4+0];
    u01 = d1*rowstorage->ptr.p_double[offsu+0*4+1];
    u02 = d2*rowstorage->ptr.p_double[offsu+0*4+2];
    u03 = d3*rowstorage->ptr.p_double[offsu+0*4+3];
    u10 = d0*rowstorage->ptr.p_double[offsu+1*4+0];
    u11 = d1*rowstorage->ptr.p_double[offsu+1*4+1];
    u12 = d2*rowstorage->ptr.p_double[offsu+1*4+2];
    u13 = d3*rowstorage->ptr.p_double[offsu+1*4+3];
    u20 = d0*rowstorage->ptr.p_double[offsu+2*4+0];
    u21 = d1*rowstorage->ptr.p_double[offsu+2*4+1];
    u22 = d2*rowstorage->ptr.p_double[offsu+2*4+2];
    u23 = d3*rowstorage->ptr.p_double[offsu+2*4+3];
    u30 = d0*rowstorage->ptr.p_double[offsu+3*4+0];
    u31 = d1*rowstorage->ptr.p_double[offsu+3*4+1];
    u32 = d2*rowstorage->ptr.p_double[offsu+3*4+2];
    u33 = d3*rowstorage->ptr.p_double[offsu+3*4+3];
    for(k=0; k<=uheight-1; k++)
    {
        targetrow = offss+raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+k]]*4;
        offsk = offsu+k*4;
        uk0 = rowstorage->ptr.p_double[offsk+0];
        uk1 = rowstorage->ptr.p_double[offsk+1];
        uk2 = rowstorage->ptr.p_double[offsk+2];
        uk3 = rowstorage->ptr.p_double[offsk+3];
        rowstorage->ptr.p_double[targetrow+0] = rowstorage->ptr.p_double[targetrow+0]-u00*uk0-u01*uk1-u02*uk2-u03*uk3;
        rowstorage->ptr.p_double[targetrow+1] = rowstorage->ptr.p_double[targetrow+1]-u10*uk0-u11*uk1-u12*uk2-u13*uk3;
        rowstorage->ptr.p_double[targetrow+2] = rowstorage->ptr.p_double[targetrow+2]-u20*uk0-u21*uk1-u22*uk2-u23*uk3;
        rowstorage->ptr.p_double[targetrow+3] = rowstorage->ptr.p_double[targetrow+3]-u30*uk0-u31*uk1-u32*uk2-u33*uk3;
    }
    result = ae_true;
    return result;
}
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Fast kernels for small supernodal updates: special 4x4x4x4 function.

! See comments on UpdateSupernode() for information  on generic supernodal
! updates, including notation used below.

The generic update has following form:

    S := S - scatter(U*D*Uc')

This specialized function performs AxBxCx4 update, i.e.:
* S is a tHeight*A matrix with row stride equal to 4 (usually it means that
  it has 3 or 4 columns)
* U is a uHeight*B matrix
* Uc' is a B*C matrix, with C<=A
* scatter() scatters rows and columns of U*Uc'
  
Return value:
* True if update was applied
* False if kernel refused to perform an update (quick exit for unsupported
  combinations of input sizes)

  -- ALGLIB routine --
     20.09.2020
     Bochkanov Sergey
*************************************************************************/
static ae_bool spchol_updatekernelabc4(/* Real    */ ae_vector* rowstorage,
     ae_int_t offss,
     ae_int_t twidth,
     ae_int_t offsu,
     ae_int_t uheight,
     ae_int_t urank,
     ae_int_t urowstride,
     ae_int_t uwidth,
     /* Real    */ const ae_vector* diagd,
     ae_int_t offsd,
     /* Integer */ const ae_vector* raw2smap,
     /* Integer */ const ae_vector* superrowidx,
     ae_int_t urbase,
     ae_state *_state)
{
    ae_int_t k;
    ae_int_t targetrow;
    ae_int_t targetcol;
    ae_int_t offsk;
    double d0;
    double d1;
    double d2;
    double d3;
    double u00;
    double u01;
    double u02;
    double u03;
    double u10;
    double u11;
    double u12;
    double u13;
    double u20;
    double u21;
    double u22;
    double u23;
    double u30;
    double u31;
    double u32;
    double u33;
    double uk0;
    double uk1;
    double uk2;
    double uk3;
    ae_int_t srccol0;
    ae_int_t srccol1;
    ae_int_t srccol2;
    ae_int_t srccol3;
    ae_bool result;


    
    /*
     * Filter out unsupported combinations (ones that are too sparse for the non-SIMD code)
     */
    result = ae_false;
    if( twidth<3||twidth>4 )
    {
        return result;
    }
    if( uwidth<3||uwidth>4 )
    {
        return result;
    }
    if( urank>4 )
    {
        return result;
    }
    
    /*
     * Determine source columns for target columns, -1 if target column
     * is not updated.
     */
    srccol0 = -1;
    srccol1 = -1;
    srccol2 = -1;
    srccol3 = -1;
    for(k=0; k<=uwidth-1; k++)
    {
        targetcol = raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+k]];
        if( targetcol==0 )
        {
            srccol0 = k;
        }
        if( targetcol==1 )
        {
            srccol1 = k;
        }
        if( targetcol==2 )
        {
            srccol2 = k;
        }
        if( targetcol==3 )
        {
            srccol3 = k;
        }
    }
    
    /*
     * Load update matrix into aligned/rearranged 4x4 storage
     */
    d0 = (double)(0);
    d1 = (double)(0);
    d2 = (double)(0);
    d3 = (double)(0);
    u00 = (double)(0);
    u01 = (double)(0);
    u02 = (double)(0);
    u03 = (double)(0);
    u10 = (double)(0);
    u11 = (double)(0);
    u12 = (double)(0);
    u13 = (double)(0);
    u20 = (double)(0);
    u21 = (double)(0);
    u22 = (double)(0);
    u23 = (double)(0);
    u30 = (double)(0);
    u31 = (double)(0);
    u32 = (double)(0);
    u33 = (double)(0);
    if( urank>=1 )
    {
        d0 = diagd->ptr.p_double[offsd+0];
    }
    if( urank>=2 )
    {
        d1 = diagd->ptr.p_double[offsd+1];
    }
    if( urank>=3 )
    {
        d2 = diagd->ptr.p_double[offsd+2];
    }
    if( urank>=4 )
    {
        d3 = diagd->ptr.p_double[offsd+3];
    }
    if( srccol0>=0 )
    {
        if( urank>=1 )
        {
            u00 = d0*rowstorage->ptr.p_double[offsu+srccol0*urowstride+0];
        }
        if( urank>=2 )
        {
            u01 = d1*rowstorage->ptr.p_double[offsu+srccol0*urowstride+1];
        }
        if( urank>=3 )
        {
            u02 = d2*rowstorage->ptr.p_double[offsu+srccol0*urowstride+2];
        }
        if( urank>=4 )
        {
            u03 = d3*rowstorage->ptr.p_double[offsu+srccol0*urowstride+3];
        }
    }
    if( srccol1>=0 )
    {
        if( urank>=1 )
        {
            u10 = d0*rowstorage->ptr.p_double[offsu+srccol1*urowstride+0];
        }
        if( urank>=2 )
        {
            u11 = d1*rowstorage->ptr.p_double[offsu+srccol1*urowstride+1];
        }
        if( urank>=3 )
        {
            u12 = d2*rowstorage->ptr.p_double[offsu+srccol1*urowstride+2];
        }
        if( urank>=4 )
        {
            u13 = d3*rowstorage->ptr.p_double[offsu+srccol1*urowstride+3];
        }
    }
    if( srccol2>=0 )
    {
        if( urank>=1 )
        {
            u20 = d0*rowstorage->ptr.p_double[offsu+srccol2*urowstride+0];
        }
        if( urank>=2 )
        {
            u21 = d1*rowstorage->ptr.p_double[offsu+srccol2*urowstride+1];
        }
        if( urank>=3 )
        {
            u22 = d2*rowstorage->ptr.p_double[offsu+srccol2*urowstride+2];
        }
        if( urank>=4 )
        {
            u23 = d3*rowstorage->ptr.p_double[offsu+srccol2*urowstride+3];
        }
    }
    if( srccol3>=0 )
    {
        if( urank>=1 )
        {
            u30 = d0*rowstorage->ptr.p_double[offsu+srccol3*urowstride+0];
        }
        if( urank>=2 )
        {
            u31 = d1*rowstorage->ptr.p_double[offsu+srccol3*urowstride+1];
        }
        if( urank>=3 )
        {
            u32 = d2*rowstorage->ptr.p_double[offsu+srccol3*urowstride+2];
        }
        if( urank>=4 )
        {
            u33 = d3*rowstorage->ptr.p_double[offsu+srccol3*urowstride+3];
        }
    }
    
    /*
     * Run update
     */
    if( urank==1 )
    {
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = offss+raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+k]]*4;
            offsk = offsu+k*urowstride;
            uk0 = rowstorage->ptr.p_double[offsk+0];
            rowstorage->ptr.p_double[targetrow+0] = rowstorage->ptr.p_double[targetrow+0]-u00*uk0;
            rowstorage->ptr.p_double[targetrow+1] = rowstorage->ptr.p_double[targetrow+1]-u10*uk0;
            rowstorage->ptr.p_double[targetrow+2] = rowstorage->ptr.p_double[targetrow+2]-u20*uk0;
            rowstorage->ptr.p_double[targetrow+3] = rowstorage->ptr.p_double[targetrow+3]-u30*uk0;
        }
    }
    if( urank==2 )
    {
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = offss+raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+k]]*4;
            offsk = offsu+k*urowstride;
            uk0 = rowstorage->ptr.p_double[offsk+0];
            uk1 = rowstorage->ptr.p_double[offsk+1];
            rowstorage->ptr.p_double[targetrow+0] = rowstorage->ptr.p_double[targetrow+0]-u00*uk0-u01*uk1;
            rowstorage->ptr.p_double[targetrow+1] = rowstorage->ptr.p_double[targetrow+1]-u10*uk0-u11*uk1;
            rowstorage->ptr.p_double[targetrow+2] = rowstorage->ptr.p_double[targetrow+2]-u20*uk0-u21*uk1;
            rowstorage->ptr.p_double[targetrow+3] = rowstorage->ptr.p_double[targetrow+3]-u30*uk0-u31*uk1;
        }
    }
    if( urank==3 )
    {
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = offss+raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+k]]*4;
            offsk = offsu+k*urowstride;
            uk0 = rowstorage->ptr.p_double[offsk+0];
            uk1 = rowstorage->ptr.p_double[offsk+1];
            uk2 = rowstorage->ptr.p_double[offsk+2];
            rowstorage->ptr.p_double[targetrow+0] = rowstorage->ptr.p_double[targetrow+0]-u00*uk0-u01*uk1-u02*uk2;
            rowstorage->ptr.p_double[targetrow+1] = rowstorage->ptr.p_double[targetrow+1]-u10*uk0-u11*uk1-u12*uk2;
            rowstorage->ptr.p_double[targetrow+2] = rowstorage->ptr.p_double[targetrow+2]-u20*uk0-u21*uk1-u22*uk2;
            rowstorage->ptr.p_double[targetrow+3] = rowstorage->ptr.p_double[targetrow+3]-u30*uk0-u31*uk1-u32*uk2;
        }
    }
    if( urank==4 )
    {
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = offss+raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+k]]*4;
            offsk = offsu+k*urowstride;
            uk0 = rowstorage->ptr.p_double[offsk+0];
            uk1 = rowstorage->ptr.p_double[offsk+1];
            uk2 = rowstorage->ptr.p_double[offsk+2];
            uk3 = rowstorage->ptr.p_double[offsk+3];
            rowstorage->ptr.p_double[targetrow+0] = rowstorage->ptr.p_double[targetrow+0]-u00*uk0-u01*uk1-u02*uk2-u03*uk3;
            rowstorage->ptr.p_double[targetrow+1] = rowstorage->ptr.p_double[targetrow+1]-u10*uk0-u11*uk1-u12*uk2-u13*uk3;
            rowstorage->ptr.p_double[targetrow+2] = rowstorage->ptr.p_double[targetrow+2]-u20*uk0-u21*uk1-u22*uk2-u23*uk3;
            rowstorage->ptr.p_double[targetrow+3] = rowstorage->ptr.p_double[targetrow+3]-u30*uk0-u31*uk1-u32*uk2-u33*uk3;
        }
    }
    result = ae_true;
    return result;
}
#endif


/*************************************************************************
Fast kernels for small supernodal updates: special rank-1 function.

! See comments on UpdateSupernode() for information  on generic supernodal
! updates, including notation used below.

The generic update has following form:

    S := S - scatter(U*D*Uc')

This specialized function performs rank-1 update, i.e.:
* S is a tHeight*A matrix, with A<=4
* U is a uHeight*1 matrix with unit stride
* Uc' is a 1*B matrix, with B<=A
* scatter() scatters rows and columns of U*Uc'
  
Return value:
* True if update was applied
* False if kernel refused to perform an update (quick exit for unsupported
  combinations of input sizes)

  -- ALGLIB routine --
     20.09.2020
     Bochkanov Sergey
*************************************************************************/
static ae_bool spchol_updatekernelrank1(/* Real    */ ae_vector* rowstorage,
     ae_int_t offss,
     ae_int_t twidth,
     ae_int_t trowstride,
     ae_int_t offsu,
     ae_int_t uheight,
     ae_int_t uwidth,
     /* Real    */ const ae_vector* diagd,
     ae_int_t offsd,
     /* Integer */ const ae_vector* raw2smap,
     /* Integer */ const ae_vector* superrowidx,
     ae_int_t urbase,
     ae_state *_state)
{
    ae_int_t k;
    ae_int_t targetrow;
    double d0;
    double u00;
    double u10;
    double u20;
    double u30;
    double uk;
    ae_int_t col0;
    ae_int_t col1;
    ae_int_t col2;
    ae_int_t col3;
    ae_bool result;


    
    /*
     * Filter out unsupported combinations (ones that are too sparse for the non-SIMD code)
     */
    result = ae_false;
    if( twidth>4 )
    {
        return result;
    }
    if( uwidth>4 )
    {
        return result;
    }
    
    /*
     * Determine target columns, load update matrix
     */
    d0 = diagd->ptr.p_double[offsd];
    col0 = 0;
    col1 = 0;
    col2 = 0;
    col3 = 0;
    u00 = (double)(0);
    u10 = (double)(0);
    u20 = (double)(0);
    u30 = (double)(0);
    if( uwidth>=1 )
    {
        col0 = raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+0]];
        u00 = d0*rowstorage->ptr.p_double[offsu+0];
    }
    if( uwidth>=2 )
    {
        col1 = raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+1]];
        u10 = d0*rowstorage->ptr.p_double[offsu+1];
    }
    if( uwidth>=3 )
    {
        col2 = raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+2]];
        u20 = d0*rowstorage->ptr.p_double[offsu+2];
    }
    if( uwidth>=4 )
    {
        col3 = raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+3]];
        u30 = d0*rowstorage->ptr.p_double[offsu+3];
    }
    
    /*
     * Run update
     */
    if( uwidth==1 )
    {
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = offss+raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+k]]*trowstride;
            uk = rowstorage->ptr.p_double[offsu+k];
            rowstorage->ptr.p_double[targetrow+col0] = rowstorage->ptr.p_double[targetrow+col0]-u00*uk;
        }
    }
    if( uwidth==2 )
    {
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = offss+raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+k]]*trowstride;
            uk = rowstorage->ptr.p_double[offsu+k];
            rowstorage->ptr.p_double[targetrow+col0] = rowstorage->ptr.p_double[targetrow+col0]-u00*uk;
            rowstorage->ptr.p_double[targetrow+col1] = rowstorage->ptr.p_double[targetrow+col1]-u10*uk;
        }
    }
    if( uwidth==3 )
    {
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = offss+raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+k]]*trowstride;
            uk = rowstorage->ptr.p_double[offsu+k];
            rowstorage->ptr.p_double[targetrow+col0] = rowstorage->ptr.p_double[targetrow+col0]-u00*uk;
            rowstorage->ptr.p_double[targetrow+col1] = rowstorage->ptr.p_double[targetrow+col1]-u10*uk;
            rowstorage->ptr.p_double[targetrow+col2] = rowstorage->ptr.p_double[targetrow+col2]-u20*uk;
        }
    }
    if( uwidth==4 )
    {
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = offss+raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+k]]*trowstride;
            uk = rowstorage->ptr.p_double[offsu+k];
            rowstorage->ptr.p_double[targetrow+col0] = rowstorage->ptr.p_double[targetrow+col0]-u00*uk;
            rowstorage->ptr.p_double[targetrow+col1] = rowstorage->ptr.p_double[targetrow+col1]-u10*uk;
            rowstorage->ptr.p_double[targetrow+col2] = rowstorage->ptr.p_double[targetrow+col2]-u20*uk;
            rowstorage->ptr.p_double[targetrow+col3] = rowstorage->ptr.p_double[targetrow+col3]-u30*uk;
        }
    }
    result = ae_true;
    return result;
}


/*************************************************************************
Fast kernels for small supernodal updates: special rank-2 function.

! See comments on UpdateSupernode() for information  on generic supernodal
! updates, including notation used below.

The generic update has following form:

    S := S - scatter(U*D*Uc')

This specialized function performs rank-2 update, i.e.:
* S is a tHeight*A matrix, with A<=4
* U is a uHeight*2 matrix with row stride equal to 2
* Uc' is a 2*B matrix, with B<=A
* scatter() scatters rows and columns of U*Uc
  
Return value:
* True if update was applied
* False if kernel refused to perform an update (quick exit for unsupported
  combinations of input sizes)

  -- ALGLIB routine --
     20.09.2020
     Bochkanov Sergey
*************************************************************************/
static ae_bool spchol_updatekernelrank2(/* Real    */ ae_vector* rowstorage,
     ae_int_t offss,
     ae_int_t twidth,
     ae_int_t trowstride,
     ae_int_t offsu,
     ae_int_t uheight,
     ae_int_t uwidth,
     /* Real    */ const ae_vector* diagd,
     ae_int_t offsd,
     /* Integer */ const ae_vector* raw2smap,
     /* Integer */ const ae_vector* superrowidx,
     ae_int_t urbase,
     ae_state *_state)
{
    ae_int_t k;
    ae_int_t targetrow;
    double d0;
    double d1;
    double u00;
    double u10;
    double u20;
    double u30;
    double u01;
    double u11;
    double u21;
    double u31;
    double uk0;
    double uk1;
    ae_int_t col0;
    ae_int_t col1;
    ae_int_t col2;
    ae_int_t col3;
    ae_bool result;


    
    /*
     * Filter out unsupported combinations (ones that are too sparse for the non-SIMD code)
     */
    result = ae_false;
    if( twidth>4 )
    {
        return result;
    }
    if( uwidth>4 )
    {
        return result;
    }
    
    /*
     * Determine target columns, load update matrix
     */
    d0 = diagd->ptr.p_double[offsd];
    d1 = diagd->ptr.p_double[offsd+1];
    col0 = 0;
    col1 = 0;
    col2 = 0;
    col3 = 0;
    u00 = (double)(0);
    u01 = (double)(0);
    u10 = (double)(0);
    u11 = (double)(0);
    u20 = (double)(0);
    u21 = (double)(0);
    u30 = (double)(0);
    u31 = (double)(0);
    if( uwidth>=1 )
    {
        col0 = raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+0]];
        u00 = d0*rowstorage->ptr.p_double[offsu+0];
        u01 = d1*rowstorage->ptr.p_double[offsu+1];
    }
    if( uwidth>=2 )
    {
        col1 = raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+1]];
        u10 = d0*rowstorage->ptr.p_double[offsu+1*2+0];
        u11 = d1*rowstorage->ptr.p_double[offsu+1*2+1];
    }
    if( uwidth>=3 )
    {
        col2 = raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+2]];
        u20 = d0*rowstorage->ptr.p_double[offsu+2*2+0];
        u21 = d1*rowstorage->ptr.p_double[offsu+2*2+1];
    }
    if( uwidth>=4 )
    {
        col3 = raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+3]];
        u30 = d0*rowstorage->ptr.p_double[offsu+3*2+0];
        u31 = d1*rowstorage->ptr.p_double[offsu+3*2+1];
    }
    
    /*
     * Run update
     */
    if( uwidth==1 )
    {
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = offss+raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+k]]*trowstride;
            uk0 = rowstorage->ptr.p_double[offsu+2*k+0];
            uk1 = rowstorage->ptr.p_double[offsu+2*k+1];
            rowstorage->ptr.p_double[targetrow+col0] = rowstorage->ptr.p_double[targetrow+col0]-u00*uk0-u01*uk1;
        }
    }
    if( uwidth==2 )
    {
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = offss+raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+k]]*trowstride;
            uk0 = rowstorage->ptr.p_double[offsu+2*k+0];
            uk1 = rowstorage->ptr.p_double[offsu+2*k+1];
            rowstorage->ptr.p_double[targetrow+col0] = rowstorage->ptr.p_double[targetrow+col0]-u00*uk0-u01*uk1;
            rowstorage->ptr.p_double[targetrow+col1] = rowstorage->ptr.p_double[targetrow+col1]-u10*uk0-u11*uk1;
        }
    }
    if( uwidth==3 )
    {
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = offss+raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+k]]*trowstride;
            uk0 = rowstorage->ptr.p_double[offsu+2*k+0];
            uk1 = rowstorage->ptr.p_double[offsu+2*k+1];
            rowstorage->ptr.p_double[targetrow+col0] = rowstorage->ptr.p_double[targetrow+col0]-u00*uk0-u01*uk1;
            rowstorage->ptr.p_double[targetrow+col1] = rowstorage->ptr.p_double[targetrow+col1]-u10*uk0-u11*uk1;
            rowstorage->ptr.p_double[targetrow+col2] = rowstorage->ptr.p_double[targetrow+col2]-u20*uk0-u21*uk1;
        }
    }
    if( uwidth==4 )
    {
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = offss+raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+k]]*trowstride;
            uk0 = rowstorage->ptr.p_double[offsu+2*k+0];
            uk1 = rowstorage->ptr.p_double[offsu+2*k+1];
            rowstorage->ptr.p_double[targetrow+col0] = rowstorage->ptr.p_double[targetrow+col0]-u00*uk0-u01*uk1;
            rowstorage->ptr.p_double[targetrow+col1] = rowstorage->ptr.p_double[targetrow+col1]-u10*uk0-u11*uk1;
            rowstorage->ptr.p_double[targetrow+col2] = rowstorage->ptr.p_double[targetrow+col2]-u20*uk0-u21*uk1;
            rowstorage->ptr.p_double[targetrow+col3] = rowstorage->ptr.p_double[targetrow+col3]-u30*uk0-u31*uk1;
        }
    }
    result = ae_true;
    return result;
}


/*************************************************************************
Generates sparsity-reducing permutation using priority AMD ordering

INPUT PARAMETERS:
    Analysis        -   analysis object
    WrkA            -   matrix being analyzed, destroyed during analysis
    Priorities      -   element priorities, destroyed during analysis
    PromoteAbove,
    PromoteTo       -   parameters, see SPSymmAnalyze() for more info
    DebugOrdering   -   whether special debug ordering which tests all
                        algorithm branches is used.
    DoTrace         -   whether trace is needed
    Buf             -   temporary buffers provided by user
    UserBuffers     -   whether to use buffers provided by user or local
                        buffers:
                        * if True, temporaries will be allocated by this
                          function in Buf and will be retained after the
                          function is done. Future calls to this function
                          will reuse previously allocated memory. Good
                          for many sequential tasks.
                        * if True, the function will allocate its own
                          local buffers. All memory allocated by this
                          function will be freed upon exit. Good for
                          large-scale one-off problems.

  -- ALGLIB routine --
     17.11.2023
     Bochkanov Sergey
*************************************************************************/
static void spchol_generatepriorityamdpermutation(sparsematrix* wrka,
     /* Integer */ ae_vector* wrkpriorities,
     double promoteabove,
     ae_int_t promoteto,
     ae_bool debugordering,
     ae_bool dotrace,
     nbpool* n1bpool,
     nipool* n1ipool,
     priorityamdbuffers* buf,
     ae_bool userbuffers,
     /* Integer */ ae_vector* fillinperm,
     /* Integer */ ae_vector* invfillinperm,
     ae_state *_state)
{
    ae_frame _frame_block;
    priorityamdbuffers localbuf;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t jj;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t n;
    ae_int_t m;
    ae_int_t range0;
    ae_int_t range1;
    ae_int_t newrange0;
    ae_int_t promoteoffset;
    ae_int_t npostponed;
    ae_int_t eligiblecnt;
    ae_vector eligible;
    ae_vector tmp0;

    ae_frame_make(_state, &_frame_block);
    memset(&localbuf, 0, sizeof(localbuf));
    memset(&eligible, 0, sizeof(eligible));
    memset(&tmp0, 0, sizeof(tmp0));
    _priorityamdbuffers_init(&localbuf, _state, ae_true);
    ae_vector_init(&eligible, 0, DT_BOOL, _state, ae_true);
    ae_vector_init(&tmp0, 0, DT_INT, _state, ae_true);

    
    /*
     * If local buffers have to be used, allocate one and run again
     */
    if( !userbuffers )
    {
        spchol_generatepriorityamdpermutation(wrka, wrkpriorities, promoteabove, promoteto, debugordering, dotrace, n1bpool, n1ipool, buf, ae_true, fillinperm, invfillinperm, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Initialize
     */
    n = wrka->n;
    ae_assert(wrkpriorities->cnt>=n, "SPSymmAnalyze: integrity check failed (4653)", _state);
    
    /*
     * Retrieve temporary arrays
     */
    nbpoolretrieve(n1bpool, &eligible, _state);
    nipoolretrieve(n1ipool, &tmp0, _state);
    
    /*
     * Perform iterative AMD, with nearly-dense columns being postponed to be handled later.
     *
     * The current (residual) matrix A is divided into two parts: head, with its columns being
     * properly ordered, and tail, with its columns being reordered at the next iteration.
     *
     * After each partial AMD we compute sparsity pattern of the tail, set it as the new residual
     * and repeat iteration.
     */
    iallocv(n, fillinperm, _state);
    iallocv(n, invfillinperm, _state);
    iallocv(n, &buf->tmpperm, _state);
    iallocv(n, &buf->invtmpperm, _state);
    for(i=0; i<=n-1; i++)
    {
        fillinperm->ptr.p_int[i] = i;
        invfillinperm->ptr.p_int[i] = i;
    }
    range0 = 0;
    range1 = n;
    promoteoffset = 0;
    while(range0<range1)
    {
        m = range1-range0;
        
        /*
         * Perform partial AMD ordering of the residual matrix:
         * * determine columns in the residual part that are eligible for elimination.
         * * generate partial fill-in reducing permutation (leading Residual-Tail columns
         *   are properly ordered, the rest is unordered).
         * * update column elimination priorities and promotion target (decrease by 1)
         */
        bsetv(range1-range0, ae_false, &eligible, _state);
        eligiblecnt = 0;
        for(i=0; i<=n-1; i++)
        {
            j = fillinperm->ptr.p_int[i];
            if( (j>=range0&&j<range1)&&wrkpriorities->ptr.p_int[i]<=0 )
            {
                eligible.ptr.p_bool[j-range0] = ae_true;
                eligiblecnt = eligiblecnt+1;
            }
        }
        if( dotrace )
        {
            ae_trace("> multiround AMD, column_range=[%7d,%7d] (%7d out of %7d), %5.1f%% eligible",
                (int)(range0),
                (int)(range1),
                (int)(range1-range0),
                (int)(n),
                (double)((double)(100*eligiblecnt)/(double)m));
        }
        newrange0 = range0+generateamdpermutationx(wrka, &eligible, range1-range0, promoteabove, &buf->tmpperm, &buf->invtmpperm, 1, &buf->amdtmp, _state);
        if( debugordering )
        {
            
            /*
             * Special debug ordering in order to test correctness of multiple AMD rounds
             */
            newrange0 = ae_minint(newrange0, range0+m/2+1, _state);
        }
        for(i=0; i<=n-1; i++)
        {
            wrkpriorities->ptr.p_int[i] = wrkpriorities->ptr.p_int[i]-1;
        }
        promoteto = ae_maxint(promoteto-1, 0, _state);
        promoteoffset = promoteoffset+1;
        npostponed = 0;
        for(i=0; i<=range1-newrange0-1; i++)
        {
            if( eligible.ptr.p_bool[buf->invtmpperm.ptr.p_int[newrange0-range0+i]] )
            {
                
                /*
                 * The column was marked as eligible, but was postponed due to its density.
                 * Promote column to a higher priority group.
                 */
                wrkpriorities->ptr.p_int[range0+buf->invtmpperm.ptr.p_int[newrange0-range0+i]] = promoteto;
                npostponed = npostponed+1;
            }
        }
        if( dotrace )
        {
            if( npostponed>0 )
            {
                ae_trace(", %5.1f%% postponed (promoted to elimination group %0d)",
                    (double)((double)(100*npostponed)/(double)m),
                    (int)(promoteoffset+promoteto));
            }
            ae_trace("\n");
        }
        
        /*
         * If there were columns that both eligible and sparse enough,
         * apply permutation and recompute trail.
         */
        if( newrange0>range0 )
        {
            
            /*
             * Apply permutation TmpPerm[] to the tail of the permutation FillInPerm[]
             */
            for(i=0; i<=m-1; i++)
            {
                fillinperm->ptr.p_int[invfillinperm->ptr.p_int[range0+buf->invtmpperm.ptr.p_int[i]]] = range0+i;
            }
            for(i=0; i<=n-1; i++)
            {
                invfillinperm->ptr.p_int[fillinperm->ptr.p_int[i]] = i;
            }
            
            /*
             * Compute partial Cholesky of the trailing submatrix (after applying rank-K update to the
             * trailing submatrix but before Cholesky-factorizing it).
             */
            if( newrange0<range1 )
            {
                sparsesymmpermtblbuf(wrka, ae_false, &buf->tmpperm, &buf->tmpa2, _state);
                spchol_partialcholeskypattern(&buf->tmpa2, newrange0-range0, range1-newrange0, wrka, n1ipool, n1bpool, &buf->tmpbottomt, &buf->tmpupdatet, &buf->tmpupdate, &buf->tmpnewtailt, _state);
            }
            range0 = newrange0;
            m = range1-range0;
        }
        
        /*
         * Analyze sparsity pattern of the current submatrix (TmpA), manually move completely dense rows to the end.
         */
        if( m>0 )
        {
            ae_assert((wrka->m==m&&wrka->n==m)&&wrka->ninitialized==wrka->ridx.ptr.p_int[m], "SPSymmAnalyze: integrity check failed (0572)", _state);
            isetv(m, 1, &tmp0, _state);
            for(i=0; i<=m-1; i++)
            {
                j0 = wrka->ridx.ptr.p_int[i];
                j1 = wrka->didx.ptr.p_int[i]-1;
                for(jj=j0; jj<=j1; jj++)
                {
                    j = wrka->idx.ptr.p_int[jj];
                    tmp0.ptr.p_int[i] = tmp0.ptr.p_int[i]+1;
                    tmp0.ptr.p_int[j] = tmp0.ptr.p_int[j]+1;
                }
            }
            j = 0;
            k = 0;
            for(i=0; i<=m-1; i++)
            {
                if( tmp0.ptr.p_int[i]<m )
                {
                    buf->invtmpperm.ptr.p_int[j] = i;
                    j = j+1;
                }
            }
            for(i=0; i<=m-1; i++)
            {
                if( tmp0.ptr.p_int[i]==m )
                {
                    buf->invtmpperm.ptr.p_int[j] = i;
                    j = j+1;
                    k = k+1;
                }
            }
            for(i=0; i<=m-1; i++)
            {
                buf->tmpperm.ptr.p_int[buf->invtmpperm.ptr.p_int[i]] = i;
            }
            ae_assert(j==m, "SPSymmAnalyze: integrity check failed (6432)", _state);
            if( k>0 )
            {
                
                /*
                 * K dense rows are moved to the end
                 */
                if( k<m )
                {
                    
                    /*
                     * There are still exist sparse rows that need reordering, apply permutation and manually truncate matrix
                     */
                    for(i=0; i<=m-1; i++)
                    {
                        fillinperm->ptr.p_int[invfillinperm->ptr.p_int[range0+buf->invtmpperm.ptr.p_int[i]]] = range0+i;
                    }
                    for(i=0; i<=n-1; i++)
                    {
                        invfillinperm->ptr.p_int[fillinperm->ptr.p_int[i]] = i;
                    }
                    sparsesymmpermtblbuf(wrka, ae_false, &buf->tmpperm, &buf->tmpa2, _state);
                    sparsecopybuf(&buf->tmpa2, wrka, _state);
                    wrka->m = m-k;
                    wrka->n = m-k;
                    wrka->ninitialized = wrka->ridx.ptr.p_int[wrka->m];
                }
                range1 = range1-k;
                m = range1-range0;
            }
        }
    }
    if( dotrace )
    {
        ae_trace("> multiround AMD, column_range=[%7d,%7d], stopped\n",
            (int)(range0),
            (int)(range1));
    }
    
    /*
     * Recycle temporary arrays
     */
    nbpoolrecycle(n1bpool, &eligible, _state);
    nipoolrecycle(n1ipool, &tmp0, _state);
    ae_frame_leave(_state);
}


void _priorityamdbuffers_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    priorityamdbuffers *p = (priorityamdbuffers*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->tmpperm, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->invtmpperm, 0, DT_INT, _state, make_automatic);
    _amdbuffer_init(&p->amdtmp, _state, make_automatic);
    _sparsematrix_init(&p->tmpa2, _state, make_automatic);
    _sparsematrix_init(&p->tmpbottomt, _state, make_automatic);
    _sparsematrix_init(&p->tmpupdate, _state, make_automatic);
    _sparsematrix_init(&p->tmpupdatet, _state, make_automatic);
    _sparsematrix_init(&p->tmpnewtailt, _state, make_automatic);
}


void _priorityamdbuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    priorityamdbuffers       *dst = (priorityamdbuffers*)_dst;
    const priorityamdbuffers *src = (const priorityamdbuffers*)_src;
    ae_vector_init_copy(&dst->tmpperm, &src->tmpperm, _state, make_automatic);
    ae_vector_init_copy(&dst->invtmpperm, &src->invtmpperm, _state, make_automatic);
    _amdbuffer_init_copy(&dst->amdtmp, &src->amdtmp, _state, make_automatic);
    _sparsematrix_init_copy(&dst->tmpa2, &src->tmpa2, _state, make_automatic);
    _sparsematrix_init_copy(&dst->tmpbottomt, &src->tmpbottomt, _state, make_automatic);
    _sparsematrix_init_copy(&dst->tmpupdate, &src->tmpupdate, _state, make_automatic);
    _sparsematrix_init_copy(&dst->tmpupdatet, &src->tmpupdatet, _state, make_automatic);
    _sparsematrix_init_copy(&dst->tmpnewtailt, &src->tmpnewtailt, _state, make_automatic);
}


void _priorityamdbuffers_clear(void* _p)
{
    priorityamdbuffers *p = (priorityamdbuffers*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->tmpperm);
    ae_vector_clear(&p->invtmpperm);
    _amdbuffer_clear(&p->amdtmp);
    _sparsematrix_clear(&p->tmpa2);
    _sparsematrix_clear(&p->tmpbottomt);
    _sparsematrix_clear(&p->tmpupdate);
    _sparsematrix_clear(&p->tmpupdatet);
    _sparsematrix_clear(&p->tmpnewtailt);
}


void _priorityamdbuffers_destroy(void* _p)
{
    priorityamdbuffers *p = (priorityamdbuffers*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->tmpperm);
    ae_vector_destroy(&p->invtmpperm);
    _amdbuffer_destroy(&p->amdtmp);
    _sparsematrix_destroy(&p->tmpa2);
    _sparsematrix_destroy(&p->tmpbottomt);
    _sparsematrix_destroy(&p->tmpupdate);
    _sparsematrix_destroy(&p->tmpupdatet);
    _sparsematrix_destroy(&p->tmpnewtailt);
}


void _spcholadj_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    spcholadj *p = (spcholadj*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->rowbegin, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->rowend, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->idx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->urow0, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->uwidth, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->uflop, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nflop, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->sflop, 0, DT_REAL, _state, make_automatic);
}


void _spcholadj_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    spcholadj       *dst = (spcholadj*)_dst;
    const spcholadj *src = (const spcholadj*)_src;
    ae_vector_init_copy(&dst->rowbegin, &src->rowbegin, _state, make_automatic);
    ae_vector_init_copy(&dst->rowend, &src->rowend, _state, make_automatic);
    ae_vector_init_copy(&dst->idx, &src->idx, _state, make_automatic);
    ae_vector_init_copy(&dst->urow0, &src->urow0, _state, make_automatic);
    ae_vector_init_copy(&dst->uwidth, &src->uwidth, _state, make_automatic);
    ae_vector_init_copy(&dst->uflop, &src->uflop, _state, make_automatic);
    ae_vector_init_copy(&dst->nflop, &src->nflop, _state, make_automatic);
    ae_vector_init_copy(&dst->sflop, &src->sflop, _state, make_automatic);
}


void _spcholadj_clear(void* _p)
{
    spcholadj *p = (spcholadj*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->rowbegin);
    ae_vector_clear(&p->rowend);
    ae_vector_clear(&p->idx);
    ae_vector_clear(&p->urow0);
    ae_vector_clear(&p->uwidth);
    ae_vector_clear(&p->uflop);
    ae_vector_clear(&p->nflop);
    ae_vector_clear(&p->sflop);
}


void _spcholadj_destroy(void* _p)
{
    spcholadj *p = (spcholadj*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->rowbegin);
    ae_vector_destroy(&p->rowend);
    ae_vector_destroy(&p->idx);
    ae_vector_destroy(&p->urow0);
    ae_vector_destroy(&p->uwidth);
    ae_vector_destroy(&p->uflop);
    ae_vector_destroy(&p->nflop);
    ae_vector_destroy(&p->sflop);
}


void _spcholanalysis_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    spcholanalysis *p = (spcholanalysis*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->referenceridx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->parentsupernode, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->childsupernodesridx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->childsupernodesidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->supercolrange, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->superrowridx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->superrowidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->blkstruct, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->fillinperm, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->invfillinperm, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->superperm, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->invsuperperm, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->effectiveperm, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->inveffectiveperm, 0, DT_INT, _state, make_automatic);
    _spcholadj_init(&p->ladj, _state, make_automatic);
    ae_vector_init(&p->outrowcounts, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->inputstorage, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->outputstorage, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rowstrides, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->rowoffsets, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->diagd, 0, DT_REAL, _state, make_automatic);
    _nbpool_init(&p->n1booleanpool, _state, make_automatic);
    _nipool_init(&p->n1integerpool, _state, make_automatic);
    _nrpool_init(&p->nrealpool, _state, make_automatic);
    ae_vector_init(&p->currowbegin, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->flagarray, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->curpriorities, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->tmpparent, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->node2supernode, 0, DT_INT, _state, make_automatic);
    _amdbuffer_init(&p->amdtmp, _state, make_automatic);
    _priorityamdbuffers_init(&p->pamdtmp, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->tmp1, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->tmp2, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->tmp3, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->tmp4, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->raw2smap, 0, DT_INT, _state, make_automatic);
    _sparsematrix_init(&p->tmpa, _state, make_automatic);
    _sparsematrix_init(&p->tmpat, _state, make_automatic);
    ae_vector_init(&p->tmpx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->simdbuf, 0, DT_REAL, _state, make_automatic);
}


void _spcholanalysis_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    spcholanalysis       *dst = (spcholanalysis*)_dst;
    const spcholanalysis *src = (const spcholanalysis*)_src;
    dst->tasktype = src->tasktype;
    dst->n = src->n;
    dst->permtype = src->permtype;
    dst->unitd = src->unitd;
    dst->modtype = src->modtype;
    dst->modparam0 = src->modparam0;
    dst->modparam1 = src->modparam1;
    dst->modparam2 = src->modparam2;
    dst->modparam3 = src->modparam3;
    dst->debugblocksupernodal = src->debugblocksupernodal;
    dst->extendeddebug = src->extendeddebug;
    dst->dotrace = src->dotrace;
    dst->dotracescheduler = src->dotracescheduler;
    dst->dotracesupernodalstructure = src->dotracesupernodalstructure;
    ae_vector_init_copy(&dst->referenceridx, &src->referenceridx, _state, make_automatic);
    dst->nsuper = src->nsuper;
    ae_vector_init_copy(&dst->parentsupernode, &src->parentsupernode, _state, make_automatic);
    ae_vector_init_copy(&dst->childsupernodesridx, &src->childsupernodesridx, _state, make_automatic);
    ae_vector_init_copy(&dst->childsupernodesidx, &src->childsupernodesidx, _state, make_automatic);
    ae_vector_init_copy(&dst->supercolrange, &src->supercolrange, _state, make_automatic);
    ae_vector_init_copy(&dst->superrowridx, &src->superrowridx, _state, make_automatic);
    ae_vector_init_copy(&dst->superrowidx, &src->superrowidx, _state, make_automatic);
    ae_vector_init_copy(&dst->blkstruct, &src->blkstruct, _state, make_automatic);
    dst->useparallelism = src->useparallelism;
    ae_vector_init_copy(&dst->fillinperm, &src->fillinperm, _state, make_automatic);
    ae_vector_init_copy(&dst->invfillinperm, &src->invfillinperm, _state, make_automatic);
    ae_vector_init_copy(&dst->superperm, &src->superperm, _state, make_automatic);
    ae_vector_init_copy(&dst->invsuperperm, &src->invsuperperm, _state, make_automatic);
    ae_vector_init_copy(&dst->effectiveperm, &src->effectiveperm, _state, make_automatic);
    ae_vector_init_copy(&dst->inveffectiveperm, &src->inveffectiveperm, _state, make_automatic);
    dst->istopologicalordering = src->istopologicalordering;
    dst->applypermutationtooutput = src->applypermutationtooutput;
    _spcholadj_init_copy(&dst->ladj, &src->ladj, _state, make_automatic);
    ae_vector_init_copy(&dst->outrowcounts, &src->outrowcounts, _state, make_automatic);
    ae_vector_init_copy(&dst->inputstorage, &src->inputstorage, _state, make_automatic);
    ae_vector_init_copy(&dst->outputstorage, &src->outputstorage, _state, make_automatic);
    ae_vector_init_copy(&dst->rowstrides, &src->rowstrides, _state, make_automatic);
    ae_vector_init_copy(&dst->rowoffsets, &src->rowoffsets, _state, make_automatic);
    ae_vector_init_copy(&dst->diagd, &src->diagd, _state, make_automatic);
    _nbpool_init_copy(&dst->n1booleanpool, &src->n1booleanpool, _state, make_automatic);
    _nipool_init_copy(&dst->n1integerpool, &src->n1integerpool, _state, make_automatic);
    _nrpool_init_copy(&dst->nrealpool, &src->nrealpool, _state, make_automatic);
    ae_vector_init_copy(&dst->currowbegin, &src->currowbegin, _state, make_automatic);
    ae_vector_init_copy(&dst->flagarray, &src->flagarray, _state, make_automatic);
    ae_vector_init_copy(&dst->curpriorities, &src->curpriorities, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpparent, &src->tmpparent, _state, make_automatic);
    ae_vector_init_copy(&dst->node2supernode, &src->node2supernode, _state, make_automatic);
    _amdbuffer_init_copy(&dst->amdtmp, &src->amdtmp, _state, make_automatic);
    _priorityamdbuffers_init_copy(&dst->pamdtmp, &src->pamdtmp, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp1, &src->tmp1, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp2, &src->tmp2, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp3, &src->tmp3, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp4, &src->tmp4, _state, make_automatic);
    ae_vector_init_copy(&dst->raw2smap, &src->raw2smap, _state, make_automatic);
    _sparsematrix_init_copy(&dst->tmpa, &src->tmpa, _state, make_automatic);
    _sparsematrix_init_copy(&dst->tmpat, &src->tmpat, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpx, &src->tmpx, _state, make_automatic);
    ae_vector_init_copy(&dst->simdbuf, &src->simdbuf, _state, make_automatic);
}


void _spcholanalysis_clear(void* _p)
{
    spcholanalysis *p = (spcholanalysis*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->referenceridx);
    ae_vector_clear(&p->parentsupernode);
    ae_vector_clear(&p->childsupernodesridx);
    ae_vector_clear(&p->childsupernodesidx);
    ae_vector_clear(&p->supercolrange);
    ae_vector_clear(&p->superrowridx);
    ae_vector_clear(&p->superrowidx);
    ae_vector_clear(&p->blkstruct);
    ae_vector_clear(&p->fillinperm);
    ae_vector_clear(&p->invfillinperm);
    ae_vector_clear(&p->superperm);
    ae_vector_clear(&p->invsuperperm);
    ae_vector_clear(&p->effectiveperm);
    ae_vector_clear(&p->inveffectiveperm);
    _spcholadj_clear(&p->ladj);
    ae_vector_clear(&p->outrowcounts);
    ae_vector_clear(&p->inputstorage);
    ae_vector_clear(&p->outputstorage);
    ae_vector_clear(&p->rowstrides);
    ae_vector_clear(&p->rowoffsets);
    ae_vector_clear(&p->diagd);
    _nbpool_clear(&p->n1booleanpool);
    _nipool_clear(&p->n1integerpool);
    _nrpool_clear(&p->nrealpool);
    ae_vector_clear(&p->currowbegin);
    ae_vector_clear(&p->flagarray);
    ae_vector_clear(&p->curpriorities);
    ae_vector_clear(&p->tmpparent);
    ae_vector_clear(&p->node2supernode);
    _amdbuffer_clear(&p->amdtmp);
    _priorityamdbuffers_clear(&p->pamdtmp);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->tmp1);
    ae_vector_clear(&p->tmp2);
    ae_vector_clear(&p->tmp3);
    ae_vector_clear(&p->tmp4);
    ae_vector_clear(&p->raw2smap);
    _sparsematrix_clear(&p->tmpa);
    _sparsematrix_clear(&p->tmpat);
    ae_vector_clear(&p->tmpx);
    ae_vector_clear(&p->simdbuf);
}


void _spcholanalysis_destroy(void* _p)
{
    spcholanalysis *p = (spcholanalysis*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->referenceridx);
    ae_vector_destroy(&p->parentsupernode);
    ae_vector_destroy(&p->childsupernodesridx);
    ae_vector_destroy(&p->childsupernodesidx);
    ae_vector_destroy(&p->supercolrange);
    ae_vector_destroy(&p->superrowridx);
    ae_vector_destroy(&p->superrowidx);
    ae_vector_destroy(&p->blkstruct);
    ae_vector_destroy(&p->fillinperm);
    ae_vector_destroy(&p->invfillinperm);
    ae_vector_destroy(&p->superperm);
    ae_vector_destroy(&p->invsuperperm);
    ae_vector_destroy(&p->effectiveperm);
    ae_vector_destroy(&p->inveffectiveperm);
    _spcholadj_destroy(&p->ladj);
    ae_vector_destroy(&p->outrowcounts);
    ae_vector_destroy(&p->inputstorage);
    ae_vector_destroy(&p->outputstorage);
    ae_vector_destroy(&p->rowstrides);
    ae_vector_destroy(&p->rowoffsets);
    ae_vector_destroy(&p->diagd);
    _nbpool_destroy(&p->n1booleanpool);
    _nipool_destroy(&p->n1integerpool);
    _nrpool_destroy(&p->nrealpool);
    ae_vector_destroy(&p->currowbegin);
    ae_vector_destroy(&p->flagarray);
    ae_vector_destroy(&p->curpriorities);
    ae_vector_destroy(&p->tmpparent);
    ae_vector_destroy(&p->node2supernode);
    _amdbuffer_destroy(&p->amdtmp);
    _priorityamdbuffers_destroy(&p->pamdtmp);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->tmp1);
    ae_vector_destroy(&p->tmp2);
    ae_vector_destroy(&p->tmp3);
    ae_vector_destroy(&p->tmp4);
    ae_vector_destroy(&p->raw2smap);
    _sparsematrix_destroy(&p->tmpa);
    _sparsematrix_destroy(&p->tmpat);
    ae_vector_destroy(&p->tmpx);
    ae_vector_destroy(&p->simdbuf);
}


/*$ End $*/
