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

#ifndef _spchol_h
#define _spchol_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "scodes.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "tsort.h"
#include "sparse.h"
#include "apstruct.h"
#include "amdordering.h"


/*$ Declarations $*/


/*************************************************************************
This structure is used  to  store supernodal dependencies  for  a Cholesky
factorization as well as precomputed update sizes, offsets, costs, etc.

* RowBegin, RowEnd          -   array[NSuper]. For an I-th supernode elements RowBegin[I] and
                                RowEnd[I] store range of locations in Idx[], URow0[] and other
                                arrays that describe updates feeding into supernode J.
                                
Below we assume that an element J, with RowBegin[I]<=J<RowEnd[I] for some I,
describes update for a supernode I from the supernode Idx[J]<I. Arrays below
store the following information:
* Idx                       -   indexes of supernodes feeding updates
* URow0                     -   update offsets. URow0[J] stores location
                                in Analysis.SuperRowIdx[] for a first row
                                of the update
* UWidth                    -   update widths
* UFLOP                     -   update flops (fused multiply-adds)

Flop counts for supernodes:
* NFLOP                     -   array[NSuper], NFLOP[I] is a number of fused
                                multiply-adds required to compute updates
                                targeting supernode I and to factorize supernode itself
* SFLOP                     -   array[NSuper], SFLOP[I] is a summary flop count
                                for supernode and all of its children.

    
*************************************************************************/
typedef struct
{
    ae_vector rowbegin;
    ae_vector rowend;
    ae_vector idx;
    ae_vector urow0;
    ae_vector uwidth;
    ae_vector uflop;
    ae_vector nflop;
    ae_vector sflop;
} spcholadj;


/*************************************************************************
This structure is used to store preliminary analysis  results  for  sparse
Cholesky: elimination tree, factorization costs, etc.
*************************************************************************/
typedef struct
{
    ae_int_t tasktype;
    ae_int_t n;
    ae_int_t permtype;
    ae_bool unitd;
    ae_int_t modtype;
    double modparam0;
    double modparam1;
    double modparam2;
    double modparam3;
    ae_bool debugblocksupernodal;
    ae_bool extendeddebug;
    ae_bool dotrace;
    ae_bool dotracescheduler;
    ae_bool dotracesupernodalstructure;
    ae_vector referenceridx;
    ae_int_t nsuper;
    ae_vector parentsupernode;
    ae_vector childsupernodesridx;
    ae_vector childsupernodesidx;
    ae_vector supercolrange;
    ae_vector superrowridx;
    ae_vector superrowidx;
    ae_vector blkstruct;
    ae_bool useparallelism;
    ae_vector fillinperm;
    ae_vector invfillinperm;
    ae_vector superperm;
    ae_vector invsuperperm;
    ae_vector effectiveperm;
    ae_vector inveffectiveperm;
    ae_bool istopologicalordering;
    ae_bool applypermutationtooutput;
    spcholadj ladj;
    ae_vector outrowcounts;
    ae_vector inputstorage;
    ae_vector outputstorage;
    ae_vector rowstrides;
    ae_vector rowoffsets;
    ae_vector diagd;
    nbpool nbooleanpool;
    nipool nintegerpool;
    nrpool nrealpool;
    ae_vector currowbegin;
    ae_vector flagarray;
    ae_vector eligible;
    ae_vector curpriorities;
    ae_vector tmpparent;
    ae_vector node2supernode;
    amdbuffer amdtmp;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
    ae_vector tmp3;
    ae_vector tmp4;
    ae_vector raw2smap;
    sparsematrix tmpa;
    sparsematrix tmpat;
    sparsematrix tmpa2;
    sparsematrix tmpbottomt;
    sparsematrix tmpupdate;
    sparsematrix tmpupdatet;
    sparsematrix tmpnewtailt;
    ae_vector tmpperm;
    ae_vector invtmpperm;
    ae_vector tmpx;
    ae_vector simdbuf;
} spcholanalysis;


/*$ Body $*/


/*************************************************************************
Informational function, useful for debugging
*************************************************************************/
ae_int_t spsymmgetmaxfastkernel(ae_state *_state);


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
    FactType    -   factorization type:
                    * 0 for traditional Cholesky
                    * 1 for LDLT decomposition with strictly diagonal D
    PermType    -   permutation type:
                    *-3 for debug improved AMD which debugs AMD itself and
                        parallel block supernodal code:
                        * AMD is debugged by generating a sequence of decreasing
                          tail sizes, ~logN in total, even if ordering can be
                          done with just one round of AMD. This ordering is
                          used to test correctness of multiple AMD rounds.
                        * parallel block supernodal code is debugged by
                          partitioning problems into smallest possible chunks,
                          ignoring thresholds set by SMPActivationLevel()
                          and SpawnLevel().
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
    Analysis    -   can be uninitialized instance, or previous analysis
                    results. Previously allocated memory is reused as much
                    as possible.
    Buf         -   buffer; may be completely uninitialized, or one remained
                    from previous calls (including ones with completely
                    different matrices). Previously allocated temporary
                    space will be reused as much as possible.

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
     ae_int_t facttype,
     ae_int_t permtype,
     spcholanalysis* analysis,
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
ae_bool spsymmfactorize(spcholanalysis* analysis, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);
void _spcholadj_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _spcholadj_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _spcholadj_clear(void* _p);
void _spcholadj_destroy(void* _p);
void _spcholanalysis_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _spcholanalysis_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _spcholanalysis_clear(void* _p);
void _spcholanalysis_destroy(void* _p);


/*$ End $*/
#endif

