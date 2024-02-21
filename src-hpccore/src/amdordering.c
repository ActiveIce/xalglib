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
#include "amdordering.h"


/*$ Declarations $*/
static ae_int_t amdordering_knsheadersize = 2;
static ae_int_t amdordering_llmentrysize = 6;
static void amdordering_nsaddkth(niset* sa,
     const amdknset* src,
     ae_int_t k,
     ae_state *_state);
static void amdordering_nssubtractkth(niset* sa,
     const amdknset* src,
     ae_int_t k,
     ae_state *_state);
static ae_int_t amdordering_nscountnotkth(const niset* sa,
     const amdknset* src,
     ae_int_t k,
     ae_state *_state);
static ae_int_t amdordering_nscountandkth(const niset* sa,
     const amdknset* src,
     ae_int_t k,
     ae_state *_state);
static void amdordering_knscompressstorage(amdknset* sa, ae_state *_state);
static void amdordering_knsreallocate(amdknset* sa,
     ae_int_t setidx,
     ae_int_t newallocated,
     ae_state *_state);
static void amdordering_knsinit(ae_int_t k,
     ae_int_t n,
     ae_int_t kprealloc,
     amdknset* sa,
     ae_state *_state);
static void amdordering_knsinitfroma(const sparsematrix* a,
     ae_int_t n,
     ae_bool ignorediagonal,
     amdknset* sa,
     ae_state *_state);
static void amdordering_knsstartenumeration(amdknset* sa,
     ae_int_t i,
     ae_state *_state);
static ae_bool amdordering_knsenumerate(amdknset* sa,
     ae_int_t* i,
     ae_state *_state);
static void amdordering_knsdirectaccess(const amdknset* sa,
     ae_int_t k,
     ae_int_t* idxbegin,
     ae_int_t* idxend,
     ae_state *_state);
static void amdordering_knsaddnewelement(amdknset* sa,
     ae_int_t i,
     ae_int_t k,
     ae_state *_state);
static void amdordering_knssubtract1(amdknset* sa,
     ae_int_t i,
     const niset* src,
     ae_state *_state);
static void amdordering_knsaddkthdistinct(amdknset* sa,
     ae_int_t i,
     const amdknset* src,
     ae_int_t k,
     ae_state *_state);
static ae_int_t amdordering_knscountkth(const amdknset* s0,
     ae_int_t k,
     ae_state *_state);
static ae_int_t amdordering_knscountnot(amdknset* s0,
     ae_int_t i,
     const niset* s1,
     ae_state *_state);
static ae_int_t amdordering_knscountnotkth(amdknset* s0,
     ae_int_t i,
     const amdknset* s1,
     ae_int_t k,
     ae_state *_state);
static ae_int_t amdordering_knscountandkth(amdknset* s0,
     ae_int_t i,
     const amdknset* s1,
     ae_int_t k,
     ae_state *_state);
static ae_int_t amdordering_knssumkth(const amdknset* s0,
     ae_int_t i,
     ae_state *_state);
static void amdordering_knsclearkthnoreclaim(amdknset* sa,
     ae_int_t k,
     ae_state *_state);
static void amdordering_knsclearkthreclaim(amdknset* sa,
     ae_int_t k,
     ae_state *_state);
static void amdordering_mtxinit(ae_int_t n,
     amdllmatrix* a,
     ae_state *_state);
static void amdordering_mtxaddcolumnto(const amdllmatrix* a,
     ae_int_t j,
     niset* s,
     ae_state *_state);
static void amdordering_mtxinsertnewelement(amdllmatrix* a,
     ae_int_t i,
     ae_int_t j,
     ae_state *_state);
static ae_int_t amdordering_mtxcountcolumnnot(const amdllmatrix* a,
     ae_int_t j,
     const niset* s,
     ae_state *_state);
static ae_int_t amdordering_mtxcountcolumn(const amdllmatrix* a,
     ae_int_t j,
     ae_state *_state);
static void amdordering_mtxclearx(amdllmatrix* a,
     ae_int_t k,
     ae_bool iscol,
     ae_state *_state);
static void amdordering_mtxclearcolumn(amdllmatrix* a,
     ae_int_t j,
     ae_state *_state);
static void amdordering_mtxclearrow(amdllmatrix* a,
     ae_int_t j,
     ae_state *_state);
static void amdordering_vtxinit(const sparsematrix* a,
     ae_int_t n,
     /* Boolean */ const ae_vector* eligible,
     ae_bool haseligible,
     ae_bool checkexactdegrees,
     amdvertexset* s,
     ae_state *_state);
static void amdordering_vtxremovevertex(amdvertexset* s,
     ae_int_t p,
     ae_state *_state);
static ae_int_t amdordering_vtxgetapprox(const amdvertexset* s,
     ae_int_t p,
     ae_state *_state);
static ae_int_t amdordering_vtxgetexact(const amdvertexset* s,
     ae_int_t p,
     ae_state *_state);
static ae_int_t amdordering_vtxgetapproxmindegree(amdvertexset* s,
     ae_state *_state);
static void amdordering_vtxupdateapproximatedegree(amdvertexset* s,
     ae_int_t p,
     ae_int_t dnew,
     ae_state *_state);
static void amdordering_vtxupdateexactdegree(amdvertexset* s,
     ae_int_t p,
     ae_int_t d,
     ae_state *_state);
static void amdordering_amdselectpivotelement(amdbuffer* buf,
     ae_int_t k,
     ae_int_t* p,
     ae_int_t* nodesize,
     ae_state *_state);
static void amdordering_amdcomputelp(amdbuffer* buf,
     ae_int_t p,
     ae_state *_state);
static void amdordering_amdmasselimination(amdbuffer* buf,
     ae_int_t p,
     ae_int_t k,
     ae_int_t tau,
     ae_state *_state);
static void amdordering_amddetectsupernodes(amdbuffer* buf,
     ae_state *_state);
static void amdordering_amdmovetoquasidense(amdbuffer* buf,
     niset* cand,
     ae_int_t p,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
This function generates approximate minimum degree ordering

INPUT PARAMETERS
    A           -   lower triangular sparse matrix  in  CRS  format.  Only
                    sparsity structure (as given by Idx[] field)  matters,
                    specific values of matrix elements are ignored.
    N           -   problem size
    Buf         -   reusable buffer object, does not need special initialization
    
OUTPUT PARAMETERS
    Perm        -   array[N], maps original indexes I to permuted indexes
    InvPerm     -   array[N], maps permuted indexes I to original indexes
    
NOTE: definite 'DEBUG.SLOW' trace tag will  activate  extra-slow  (roughly
      N^3 ops) integrity checks, in addition to cheap O(1) ones.

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
void generateamdpermutation(const sparsematrix* a,
     ae_int_t n,
     /* Integer */ ae_vector* perm,
     /* Integer */ ae_vector* invperm,
     amdbuffer* buf,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t r;
    ae_vector dummy;

    ae_frame_make(_state, &_frame_block);
    memset(&dummy, 0, sizeof(dummy));
    ae_vector_init(&dummy, 0, DT_BOOL, _state, ae_true);

    r = generateamdpermutationx(a, &dummy, n, 0.0, perm, invperm, 0, buf, _state);
    ae_assert(r==n, "GenerateAMDPermutation: integrity check failed, the matrix is only partially processed", _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This  function  generates  approximate  minimum  degree ordering,   either
classic or improved with better support for dense rows:
* the classic version processed entire matrix and returns N as result. The
  problem with classic version is that it may be slow  for  matrices  with
  dense or nearly dense rows
* the improved version processes K most sparse rows, and moves  other  N-K
  ones to the end. The number of sparse rows  K  is  returned.  The  tail,
  which is now a (N-K)*(N-K) matrix, should be repeatedly processed by the
  same function until zero is returned.

INPUT PARAMETERS
    A           -   lower triangular sparse matrix in CRS format
    Eligible    -   array[N], set of boolean flags that mark columns of  A
                    as eligible for ordering. Columns that are not eligible
                    are postponed (moved to the end) by the  improved  AMD
                    algorithm. This array is ignored  (not  referenced  at
                    all) when AMDType=0.
    N           -   problem size
    PromoteAbove-   columns with degrees higher than PromoteAbove*max(MEAN(Degree),1)
                    may be postponed. Ignored for AMDType<>1.
                    This parameter controls postponement of dense columns
                    (and algorithm ability to efficiently handle them):
                    * big PromoteAbove (N or more) effectively means that
                      no eligible columns are postponed. Better to combine
                      with your own heuristic to choose eligible columns,
                      otherwise algorithm will have hard time on problems
                      with dense columns in the eligible set.
                    * values between 2 and 10 are usually  a  good  choice
                      for manual control
                    * zero  value  means   that   appropriate   value   is
                      automatically chosen. Specific value may  change  in
                      future ALGLIB versions. Recommended.
    AMDType     -   ordering type:
                    * 0 for the classic AMD
                    * 1 for the improved AMD
    Buf         -   reusable buffer object, does not need special initialization
    
OUTPUT PARAMETERS
    Perm        -   array[N], maps original indexes I to permuted indexes
    InvPerm     -   array[N], maps permuted indexes I to original indexes
    
RESULT:
    number of successfully ordered rows/cols;
    for AMDType=0:  Result=N
    for AMDType=1:  0<=Result<=N. Result=0 is returned only when there are
                    no columns that are both sparse enough and eligible.
    
NOTE: defining 'DEBUG.SLOW' trace tag will  activate  extra-slow  (roughly
      N^3 ops) integrity checks, in addition to cheap O(1) ones.

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
ae_int_t generateamdpermutationx(const sparsematrix* a,
     /* Boolean */ const ae_vector* eligible,
     ae_int_t n,
     double promoteabove,
     /* Integer */ ae_vector* perm,
     /* Integer */ ae_vector* invperm,
     ae_int_t amdtype,
     amdbuffer* buf,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t p;
    ae_int_t setprealloc;
    ae_int_t inithashbucketsize;
    ae_bool extendeddebug;
    ae_int_t nodesize;
    ae_int_t cnt0;
    ae_int_t cnt1;
    ae_int_t tau;
    double meand;
    ae_int_t neligible;
    ae_int_t d;
    ae_int_t result;


    ae_assert(amdtype==0||amdtype==1, "GenerateAMDPermutationX: unexpected ordering type", _state);
    ae_assert(amdtype==0||(ae_isfinite(promoteabove, _state)&&ae_fp_greater_eq(promoteabove,(double)(0))), "GenerateAMDPermutationX: unexpected PromoteAbove - infinite or negative", _state);
    setprealloc = 3;
    inithashbucketsize = 16;
    extendeddebug = ae_is_trace_enabled("DEBUG.SLOW")&&n<=100;
    result = n;
    buf->n = n;
    buf->checkexactdegrees = extendeddebug;
    buf->extendeddebug = extendeddebug;
    amdordering_mtxinit(n, &buf->mtxl, _state);
    amdordering_knsinitfroma(a, n, ae_true, &buf->seta, _state);
    amdordering_knsinit(n, n, setprealloc, &buf->setsuper, _state);
    for(i=0; i<=n-1; i++)
    {
        amdordering_knsaddnewelement(&buf->setsuper, i, i, _state);
    }
    amdordering_knsinit(n, n, setprealloc, &buf->sete, _state);
    amdordering_knsinit(n, n, inithashbucketsize, &buf->hashbuckets, _state);
    nisinitemptyslow(n, &buf->nonemptybuckets, _state);
    ivectorsetlengthatleast(&buf->perm, n, _state);
    ivectorsetlengthatleast(&buf->invperm, n, _state);
    ivectorsetlengthatleast(&buf->columnswaps, n, _state);
    for(i=0; i<=n-1; i++)
    {
        buf->perm.ptr.p_int[i] = i;
        buf->invperm.ptr.p_int[i] = i;
        buf->columnswaps.ptr.p_int[i] = i;
    }
    bsetallocv(n, ae_true, &buf->iseligible, _state);
    if( amdtype==1 )
    {
        bcopyv(n, eligible, &buf->iseligible, _state);
    }
    amdordering_vtxinit(a, n, &buf->iseligible, ae_true, buf->checkexactdegrees, &buf->vertexdegrees, _state);
    bsetallocv(n, ae_true, &buf->issupernode, _state);
    bsetallocv(n, ae_false, &buf->iseliminated, _state);
    isetallocv(n, -1, &buf->arrwe, _state);
    iallocv(n, &buf->ls, _state);
    nisinitemptyslow(n, &buf->setp, _state);
    nisinitemptyslow(n, &buf->lp, _state);
    nisinitemptyslow(n, &buf->setrp, _state);
    nisinitemptyslow(n, &buf->ep, _state);
    nisinitemptyslow(n, &buf->exactdegreetmp0, _state);
    nisinitemptyslow(n, &buf->adji, _state);
    nisinitemptyslow(n, &buf->adjj, _state);
    nisinitemptyslow(n, &buf->setq, _state);
    nisinitemptyslow(n, &buf->setqsupercand, _state);
    if( extendeddebug )
    {
        ae_matrix_set_length(&buf->dbga, n, n, _state);
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                if( (j<=i&&sparseexists(a, i, j, _state))||(j>=i&&sparseexists(a, j, i, _state)) )
                {
                    buf->dbga.ptr.pp_double[i][j] = 0.1/(double)n*(ae_sin((double)i+0.17, _state)+ae_cos(ae_sqrt((double)j+0.65, _state), _state));
                }
                else
                {
                    buf->dbga.ptr.pp_double[i][j] = (double)(0);
                }
            }
        }
        for(i=0; i<=n-1; i++)
        {
            buf->dbga.ptr.pp_double[i][i] = (double)(1);
        }
    }
    neligible = n;
    tau = 0;
    if( amdtype==1 )
    {
        ae_assert(eligible->cnt>=n, "GenerateAMDPermutationX: length(Eligible)<N", _state);
        meand = 0.0;
        neligible = 0;
        for(i=0; i<=n-1; i++)
        {
            if( eligible->ptr.p_bool[i] )
            {
                d = amdordering_vtxgetapprox(&buf->vertexdegrees, i, _state);
                meand = meand+(double)d;
                neligible = neligible+1;
            }
        }
        meand = meand/coalesce((double)(neligible), (double)(1), _state);
        tau = ae_round(rcase2(ae_fp_greater(promoteabove,(double)(0)), ae_maxreal(promoteabove, (double)(1), _state), (double)(10), _state)*ae_maxreal(meand, (double)(1), _state), _state);
        tau = ae_maxint(tau, 1, _state);
        for(i=0; i<=n-1; i++)
        {
            if( amdordering_vtxgetapprox(&buf->vertexdegrees, i, _state)>tau )
            {
                nisaddelement(&buf->setqsupercand, i, _state);
            }
        }
        amdordering_amdmovetoquasidense(buf, &buf->setqsupercand, -1, _state);
    }
    k = 0;
    while(k<n-niscount(&buf->setq, _state))
    {
        amdordering_amdselectpivotelement(buf, k, &p, &nodesize, _state);
        if( p<0 )
        {
            break;
        }
        amdordering_amdcomputelp(buf, p, _state);
        amdordering_amdmasselimination(buf, p, k, tau, _state);
        amdordering_amdmovetoquasidense(buf, &buf->setqsupercand, p, _state);
        amdordering_amddetectsupernodes(buf, _state);
        if( extendeddebug )
        {
            ae_assert(buf->checkexactdegrees, "AMD: extended debug needs exact degrees", _state);
            for(i=k; i<=k+nodesize-1; i++)
            {
                if( buf->columnswaps.ptr.p_int[i]!=i )
                {
                    swaprows(&buf->dbga, i, buf->columnswaps.ptr.p_int[i], n, _state);
                    swapcols(&buf->dbga, i, buf->columnswaps.ptr.p_int[i], n, _state);
                }
            }
            for(i=0; i<=nodesize-1; i++)
            {
                rmatrixgemm(n-k-i, n-k-i, k+i, -1.0, &buf->dbga, k+i, 0, 0, &buf->dbga, 0, k+i, 0, 1.0, &buf->dbga, k+i, k+i, _state);
            }
            cnt0 = niscount(&buf->lp, _state);
            cnt1 = 0;
            for(i=k+1; i<=n-1; i++)
            {
                if( ae_fp_neq(buf->dbga.ptr.pp_double[i][k],(double)(0)) )
                {
                    inc(&cnt1, _state);
                }
            }
            ae_assert(cnt0+nodesize-1==cnt1, "AMD: integrity check 7344 failed", _state);
            ae_assert(amdordering_vtxgetapprox(&buf->vertexdegrees, p, _state)>=amdordering_vtxgetexact(&buf->vertexdegrees, p, _state), "AMD: integrity check for ApproxD failed", _state);
            ae_assert(amdordering_vtxgetexact(&buf->vertexdegrees, p, _state)==cnt0, "AMD: integrity check for ExactD failed", _state);
        }
        ae_assert(amdordering_vtxgetapprox(&buf->vertexdegrees, p, _state)>=niscount(&buf->lp, _state), "AMD: integrity check 7956 failed", _state);
        ae_assert((amdordering_knscountkth(&buf->sete, p, _state)>2||niscount(&buf->setq, _state)>0)||amdordering_vtxgetapprox(&buf->vertexdegrees, p, _state)==niscount(&buf->lp, _state), "AMD: integrity check 7295 failed", _state);
        amdordering_knsstartenumeration(&buf->sete, p, _state);
        while(amdordering_knsenumerate(&buf->sete, &j, _state))
        {
            amdordering_mtxclearcolumn(&buf->mtxl, j, _state);
        }
        amdordering_knsstartenumeration(&buf->setsuper, p, _state);
        while(amdordering_knsenumerate(&buf->setsuper, &j, _state))
        {
            buf->iseliminated.ptr.p_bool[j] = ae_true;
            amdordering_mtxclearrow(&buf->mtxl, j, _state);
        }
        amdordering_knsclearkthreclaim(&buf->seta, p, _state);
        amdordering_knsclearkthreclaim(&buf->sete, p, _state);
        buf->issupernode.ptr.p_bool[p] = ae_false;
        amdordering_vtxremovevertex(&buf->vertexdegrees, p, _state);
        k = k+nodesize;
    }
    ae_assert(k>0||amdtype==1, "AMD: integrity check 9463 failed", _state);
    result = k;
    ivectorsetlengthatleast(perm, n, _state);
    ivectorsetlengthatleast(invperm, n, _state);
    for(i=0; i<=n-1; i++)
    {
        perm->ptr.p_int[i] = buf->perm.ptr.p_int[i];
        invperm->ptr.p_int[i] = buf->invperm.ptr.p_int[i];
    }
    return result;
}


/*************************************************************************
Add K-th set from the source kn-set

INPUT PARAMETERS
    SA          -   set
    Src, K      -   source kn-set and set index K
    
OUTPUT PARAMETERS
    SA          -   modified SA

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_nsaddkth(niset* sa,
     const amdknset* src,
     ae_int_t k,
     ae_state *_state)
{
    ae_int_t idxbegin;
    ae_int_t idxend;
    ae_int_t j;
    ae_int_t ns;


    idxbegin = src->vbegin.ptr.p_int[k];
    idxend = idxbegin+src->vcnt.ptr.p_int[k];
    ns = sa->nstored;
    while(idxbegin<idxend)
    {
        j = src->data.ptr.p_int[idxbegin];
        if( sa->locationof.ptr.p_int[j]<0 )
        {
            sa->locationof.ptr.p_int[j] = ns;
            sa->items.ptr.p_int[ns] = j;
            ns = ns+1;
        }
        idxbegin = idxbegin+1;
    }
    sa->nstored = ns;
}


/*************************************************************************
Subtracts K-th set from the source structure

INPUT PARAMETERS
    SA          -   set
    Src, K      -   source kn-set and set index K
    
OUTPUT PARAMETERS
    SA          -   modified SA

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_nssubtractkth(niset* sa,
     const amdknset* src,
     ae_int_t k,
     ae_state *_state)
{
    ae_int_t idxbegin;
    ae_int_t idxend;
    ae_int_t j;
    ae_int_t loc;
    ae_int_t ns;
    ae_int_t item;


    idxbegin = src->vbegin.ptr.p_int[k];
    idxend = idxbegin+src->vcnt.ptr.p_int[k];
    ns = sa->nstored;
    while(idxbegin<idxend)
    {
        j = src->data.ptr.p_int[idxbegin];
        loc = sa->locationof.ptr.p_int[j];
        if( loc>=0 )
        {
            item = sa->items.ptr.p_int[ns-1];
            sa->items.ptr.p_int[loc] = item;
            sa->locationof.ptr.p_int[item] = loc;
            sa->locationof.ptr.p_int[j] = -1;
            ns = ns-1;
        }
        idxbegin = idxbegin+1;
    }
    sa->nstored = ns;
}


/*************************************************************************
Counts set elements not present in the K-th set of the source structure

INPUT PARAMETERS
    SA          -   set
    Src, K      -   source kn-set and set index K
    
RESULT
    number of elements in SA not present in Src[K]

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static ae_int_t amdordering_nscountnotkth(const niset* sa,
     const amdknset* src,
     ae_int_t k,
     ae_state *_state)
{
    ae_int_t idxbegin;
    ae_int_t idxend;
    ae_int_t intersectcnt;
    ae_int_t result;


    idxbegin = src->vbegin.ptr.p_int[k];
    idxend = idxbegin+src->vcnt.ptr.p_int[k];
    intersectcnt = 0;
    while(idxbegin<idxend)
    {
        if( sa->locationof.ptr.p_int[src->data.ptr.p_int[idxbegin]]>=0 )
        {
            intersectcnt = intersectcnt+1;
        }
        idxbegin = idxbegin+1;
    }
    result = sa->nstored-intersectcnt;
    return result;
}


/*************************************************************************
Counts set elements also present in the K-th set of the source structure

INPUT PARAMETERS
    SA          -   set
    Src, K      -   source kn-set and set index K
    
RESULT
    number of elements in SA also present in Src[K]

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static ae_int_t amdordering_nscountandkth(const niset* sa,
     const amdknset* src,
     ae_int_t k,
     ae_state *_state)
{
    ae_int_t idxbegin;
    ae_int_t idxend;
    ae_int_t result;


    idxbegin = src->vbegin.ptr.p_int[k];
    idxend = idxbegin+src->vcnt.ptr.p_int[k];
    result = 0;
    while(idxbegin<idxend)
    {
        if( sa->locationof.ptr.p_int[src->data.ptr.p_int[idxbegin]]>=0 )
        {
            result = result+1;
        }
        idxbegin = idxbegin+1;
    }
    return result;
}


/*************************************************************************
Compresses internal storage, reclaiming previously dropped blocks. To be
used internally by kn-set modification functions.

INPUT PARAMETERS
    SA          -   kn-set to compress

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_knscompressstorage(amdknset* sa, ae_state *_state)
{
    ae_int_t i;
    ae_int_t blocklen;
    ae_int_t setidx;
    ae_int_t srcoffs;
    ae_int_t dstoffs;


    srcoffs = 0;
    dstoffs = 0;
    while(srcoffs<sa->dataused)
    {
        blocklen = sa->data.ptr.p_int[srcoffs+0];
        setidx = sa->data.ptr.p_int[srcoffs+1];
        ae_assert(blocklen>=amdordering_knsheadersize, "knsCompressStorage: integrity check 6385 failed", _state);
        if( setidx<0 )
        {
            srcoffs = srcoffs+blocklen;
            continue;
        }
        if( srcoffs!=dstoffs )
        {
            for(i=0; i<=blocklen-1; i++)
            {
                sa->data.ptr.p_int[dstoffs+i] = sa->data.ptr.p_int[srcoffs+i];
            }
            sa->vbegin.ptr.p_int[setidx] = dstoffs+amdordering_knsheadersize;
        }
        dstoffs = dstoffs+blocklen;
        srcoffs = srcoffs+blocklen;
    }
    ae_assert(srcoffs==sa->dataused, "knsCompressStorage: integrity check 9464 failed", _state);
    sa->dataused = dstoffs;
}


/*************************************************************************
Reallocates internal storage for set #SetIdx, increasing its  capacity  to
NewAllocated exactly. This function may invalidate internal  pointers  for
ALL   sets  in  the  kn-set  structure  because  it  may  perform  storage
compression in order to reclaim previously freed space.

INPUT PARAMETERS
    SA          -   kn-set structure
    SetIdx      -   set to reallocate
    NewAllocated -  new size for the set, must be at least equal to already
                    allocated

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_knsreallocate(amdknset* sa,
     ae_int_t setidx,
     ae_int_t newallocated,
     ae_state *_state)
{
    ae_int_t oldbegin;
    ae_int_t oldcnt;
    ae_int_t newbegin;
    ae_int_t j;


    if( sa->data.cnt<sa->dataused+amdordering_knsheadersize+newallocated )
    {
        amdordering_knscompressstorage(sa, _state);
        if( sa->data.cnt<sa->dataused+amdordering_knsheadersize+newallocated )
        {
            ivectorgrowto(&sa->data, sa->dataused+amdordering_knsheadersize+newallocated, _state);
        }
    }
    oldbegin = sa->vbegin.ptr.p_int[setidx];
    oldcnt = sa->vcnt.ptr.p_int[setidx];
    newbegin = sa->dataused+amdordering_knsheadersize;
    sa->vbegin.ptr.p_int[setidx] = newbegin;
    sa->vallocated.ptr.p_int[setidx] = newallocated;
    sa->data.ptr.p_int[oldbegin-1] = -1;
    sa->data.ptr.p_int[newbegin-2] = amdordering_knsheadersize+newallocated;
    sa->data.ptr.p_int[newbegin-1] = setidx;
    sa->dataused = sa->dataused+sa->data.ptr.p_int[newbegin-2];
    for(j=0; j<=oldcnt-1; j++)
    {
        sa->data.ptr.p_int[newbegin+j] = sa->data.ptr.p_int[oldbegin+j];
    }
}


/*************************************************************************
Initialize kn-set

INPUT PARAMETERS
    K           -   sets count
    N           -   set size
    kPrealloc   -   preallocate place per set (can be zero)
    
OUTPUT PARAMETERS
    SA          -   K sets of N elements, initially empty

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_knsinit(ae_int_t k,
     ae_int_t n,
     ae_int_t kprealloc,
     amdknset* sa,
     ae_state *_state)
{
    ae_int_t i;


    sa->k = n;
    sa->n = n;
    isetallocv(n, -1, &sa->flagarray, _state);
    isetallocv(n, kprealloc, &sa->vallocated, _state);
    ivectorsetlengthatleast(&sa->vbegin, n, _state);
    sa->vbegin.ptr.p_int[0] = amdordering_knsheadersize;
    for(i=1; i<=n-1; i++)
    {
        sa->vbegin.ptr.p_int[i] = sa->vbegin.ptr.p_int[i-1]+sa->vallocated.ptr.p_int[i-1]+amdordering_knsheadersize;
    }
    sa->dataused = sa->vbegin.ptr.p_int[n-1]+sa->vallocated.ptr.p_int[n-1];
    ivectorsetlengthatleast(&sa->data, sa->dataused, _state);
    for(i=0; i<=n-1; i++)
    {
        sa->data.ptr.p_int[sa->vbegin.ptr.p_int[i]-2] = amdordering_knsheadersize+sa->vallocated.ptr.p_int[i];
        sa->data.ptr.p_int[sa->vbegin.ptr.p_int[i]-1] = i;
    }
    isetallocv(n, 0, &sa->vcnt, _state);
}


/*************************************************************************
Initialize kn-set from lower triangle of symmetric A

INPUT PARAMETERS
    A           -   lower triangular sparse matrix in CRS format
    N           -   problem size
    IgnoreDiagonal- if True, diagonal elements are not included into kn-set
    
OUTPUT PARAMETERS
    SA          -   N sets of N elements, reproducing both lower and upper
                    triangles of A

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_knsinitfroma(const sparsematrix* a,
     ae_int_t n,
     ae_bool ignorediagonal,
     amdknset* sa,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t jj;
    ae_int_t j0;
    ae_int_t j1;


    sa->k = n;
    sa->n = n;
    isetallocv(n, -1, &sa->flagarray, _state);
    ivectorsetlengthatleast(&sa->vallocated, n, _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(a->didx.ptr.p_int[i]<a->uidx.ptr.p_int[i], "knsInitFromA: integrity check for diagonal of A failed", _state);
        j0 = a->ridx.ptr.p_int[i];
        j1 = a->didx.ptr.p_int[i]-1;
        sa->vallocated.ptr.p_int[i] = j1-j0+1;
        if( !ignorediagonal )
        {
            sa->vallocated.ptr.p_int[i] = sa->vallocated.ptr.p_int[i]+1;
        }
        for(jj=j0; jj<=j1; jj++)
        {
            j = a->idx.ptr.p_int[jj];
            sa->vallocated.ptr.p_int[j] = sa->vallocated.ptr.p_int[j]+1;
        }
    }
    ivectorsetlengthatleast(&sa->vbegin, n, _state);
    sa->vbegin.ptr.p_int[0] = amdordering_knsheadersize;
    for(i=1; i<=n-1; i++)
    {
        sa->vbegin.ptr.p_int[i] = sa->vbegin.ptr.p_int[i-1]+sa->vallocated.ptr.p_int[i-1]+amdordering_knsheadersize;
    }
    sa->dataused = sa->vbegin.ptr.p_int[n-1]+sa->vallocated.ptr.p_int[n-1];
    ivectorsetlengthatleast(&sa->data, sa->dataused, _state);
    for(i=0; i<=n-1; i++)
    {
        sa->data.ptr.p_int[sa->vbegin.ptr.p_int[i]-2] = amdordering_knsheadersize+sa->vallocated.ptr.p_int[i];
        sa->data.ptr.p_int[sa->vbegin.ptr.p_int[i]-1] = i;
    }
    isetallocv(n, 0, &sa->vcnt, _state);
    for(i=0; i<=n-1; i++)
    {
        if( !ignorediagonal )
        {
            sa->data.ptr.p_int[sa->vbegin.ptr.p_int[i]+sa->vcnt.ptr.p_int[i]] = i;
            sa->vcnt.ptr.p_int[i] = sa->vcnt.ptr.p_int[i]+1;
        }
        j0 = a->ridx.ptr.p_int[i];
        j1 = a->didx.ptr.p_int[i]-1;
        for(jj=j0; jj<=j1; jj++)
        {
            j = a->idx.ptr.p_int[jj];
            sa->data.ptr.p_int[sa->vbegin.ptr.p_int[i]+sa->vcnt.ptr.p_int[i]] = j;
            sa->data.ptr.p_int[sa->vbegin.ptr.p_int[j]+sa->vcnt.ptr.p_int[j]] = i;
            sa->vcnt.ptr.p_int[i] = sa->vcnt.ptr.p_int[i]+1;
            sa->vcnt.ptr.p_int[j] = sa->vcnt.ptr.p_int[j]+1;
        }
    }
}


/*************************************************************************
Prepares iteration over I-th set

INPUT PARAMETERS
    SA          -   kn-set
    I           -   set index
    
OUTPUT PARAMETERS
    SA          -   SA ready for repeated calls of knsEnumerate()

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_knsstartenumeration(amdknset* sa,
     ae_int_t i,
     ae_state *_state)
{


    sa->iterrow = i;
    sa->iteridx = 0;
}


/*************************************************************************
Iterates over I-th set (as specified during recent knsStartEnumeration call).
Subsequent calls return True and set J to new set item until iteration
stops and False is returned.

INPUT PARAMETERS
    SA          -   kn-set
    
OUTPUT PARAMETERS
    J           -   if:
                    * Result=True - index of element in the set
                    * Result=False - not set


  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static ae_bool amdordering_knsenumerate(amdknset* sa,
     ae_int_t* i,
     ae_state *_state)
{
    ae_bool result;

    *i = 0;

    if( sa->iteridx<sa->vcnt.ptr.p_int[sa->iterrow] )
    {
        *i = sa->data.ptr.p_int[sa->vbegin.ptr.p_int[sa->iterrow]+sa->iteridx];
        sa->iteridx = sa->iteridx+1;
        result = ae_true;
    }
    else
    {
        result = ae_false;
    }
    return result;
}


/*************************************************************************
Allows direct access to internal storage  of  kn-set  structure  - returns
range of elements SA.Data[idxBegin...idxEnd-1] used to store K-th set

INPUT PARAMETERS
    SA          -   kn-set
    K           -   set index
    
OUTPUT PARAMETERS
    idxBegin,
    idxEnd      -   half-range [idxBegin,idxEnd) of SA.Data that stores
                    K-th set


  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_knsdirectaccess(const amdknset* sa,
     ae_int_t k,
     ae_int_t* idxbegin,
     ae_int_t* idxend,
     ae_state *_state)
{

    *idxbegin = 0;
    *idxend = 0;

    *idxbegin = sa->vbegin.ptr.p_int[k];
    *idxend = *idxbegin+sa->vcnt.ptr.p_int[k];
}


/*************************************************************************
Add K-th element to I-th set. The caller guarantees that  the  element  is
not present in the target set.

INPUT PARAMETERS
    SA          -   kn-set
    I           -   set index
    K           -   element to add
    
OUTPUT PARAMETERS
    SA          -   modified SA

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_knsaddnewelement(amdknset* sa,
     ae_int_t i,
     ae_int_t k,
     ae_state *_state)
{
    ae_int_t cnt;


    cnt = sa->vcnt.ptr.p_int[i];
    if( cnt==sa->vallocated.ptr.p_int[i] )
    {
        amdordering_knsreallocate(sa, i, 2*sa->vallocated.ptr.p_int[i]+1, _state);
    }
    sa->data.ptr.p_int[sa->vbegin.ptr.p_int[i]+cnt] = k;
    sa->vcnt.ptr.p_int[i] = cnt+1;
}


/*************************************************************************
Subtracts source n-set from the I-th set of the destination kn-set.

INPUT PARAMETERS
    SA          -   destination kn-set structure
    I           -   set index in the structure
    Src         -   source n-set
    
OUTPUT PARAMETERS
    SA          -   I-th set except for elements in Src

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_knssubtract1(amdknset* sa,
     ae_int_t i,
     const niset* src,
     ae_state *_state)
{
    ae_int_t j;
    ae_int_t idxbegin;
    ae_int_t idxend;
    ae_int_t cnt;


    cnt = sa->vcnt.ptr.p_int[i];
    idxbegin = sa->vbegin.ptr.p_int[i];
    idxend = idxbegin+cnt;
    while(idxbegin<idxend)
    {
        j = sa->data.ptr.p_int[idxbegin];
        if( src->locationof.ptr.p_int[j]>=0 )
        {
            sa->data.ptr.p_int[idxbegin] = sa->data.ptr.p_int[idxend-1];
            idxend = idxend-1;
            cnt = cnt-1;
        }
        else
        {
            idxbegin = idxbegin+1;
        }
    }
    sa->vcnt.ptr.p_int[i] = cnt;
}


/*************************************************************************
Adds Kth set of the source kn-set to the I-th destination set. The  caller
guarantees that SA[I] and Src[J] do NOT intersect, i.e. do not have shared
elements - it allows to use faster algorithms.

INPUT PARAMETERS
    SA          -   destination kn-set structure
    I           -   set index in the structure
    Src         -   source kn-set
    K           -   set index
    
OUTPUT PARAMETERS
    SA          -   I-th set plus for elements in K-th set of Src

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_knsaddkthdistinct(amdknset* sa,
     ae_int_t i,
     const amdknset* src,
     ae_int_t k,
     ae_state *_state)
{
    ae_int_t idxdst;
    ae_int_t idxsrcbegin;
    ae_int_t cnt;
    ae_int_t srccnt;
    ae_int_t j;


    cnt = sa->vcnt.ptr.p_int[i];
    srccnt = src->vcnt.ptr.p_int[k];
    if( cnt+srccnt>sa->vallocated.ptr.p_int[i] )
    {
        amdordering_knsreallocate(sa, i, 2*(cnt+srccnt)+1, _state);
    }
    idxsrcbegin = src->vbegin.ptr.p_int[k];
    idxdst = sa->vbegin.ptr.p_int[i]+cnt;
    for(j=0; j<=srccnt-1; j++)
    {
        sa->data.ptr.p_int[idxdst] = src->data.ptr.p_int[idxsrcbegin+j];
        idxdst = idxdst+1;
    }
    sa->vcnt.ptr.p_int[i] = cnt+srccnt;
}


/*************************************************************************
Counts elements of K-th set of S0

INPUT PARAMETERS
    S0          -   kn-set structure
    K           -   set index in the structure S0
    
RESULT
    K-th set element count

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static ae_int_t amdordering_knscountkth(const amdknset* s0,
     ae_int_t k,
     ae_state *_state)
{
    ae_int_t result;


    result = s0->vcnt.ptr.p_int[k];
    return result;
}


/*************************************************************************
Counts elements of I-th set of S0 not present in S1

INPUT PARAMETERS
    S0          -   kn-set structure
    I           -   set index in the structure S0
    S1          -   kn-set to compare against
    
RESULT
    count

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static ae_int_t amdordering_knscountnot(amdknset* s0,
     ae_int_t i,
     const niset* s1,
     ae_state *_state)
{
    ae_int_t idxbegin0;
    ae_int_t cnt0;
    ae_int_t j;
    ae_int_t result;


    cnt0 = s0->vcnt.ptr.p_int[i];
    idxbegin0 = s0->vbegin.ptr.p_int[i];
    result = 0;
    for(j=0; j<=cnt0-1; j++)
    {
        if( s1->locationof.ptr.p_int[s0->data.ptr.p_int[idxbegin0+j]]<0 )
        {
            result = result+1;
        }
    }
    return result;
}


/*************************************************************************
Counts elements of I-th set of S0 not present in K-th set of S1

INPUT PARAMETERS
    S0          -   kn-set structure
    I           -   set index in the structure S0
    S1          -   kn-set to compare against
    K           -   set index in the structure S1
    
RESULT
    count

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static ae_int_t amdordering_knscountnotkth(amdknset* s0,
     ae_int_t i,
     const amdknset* s1,
     ae_int_t k,
     ae_state *_state)
{
    ae_int_t idxbegin0;
    ae_int_t idxbegin1;
    ae_int_t cnt0;
    ae_int_t cnt1;
    ae_int_t j;
    ae_int_t result;


    cnt0 = s0->vcnt.ptr.p_int[i];
    cnt1 = s1->vcnt.ptr.p_int[k];
    idxbegin0 = s0->vbegin.ptr.p_int[i];
    idxbegin1 = s1->vbegin.ptr.p_int[k];
    for(j=0; j<=cnt1-1; j++)
    {
        s0->flagarray.ptr.p_int[s1->data.ptr.p_int[idxbegin1+j]] = 1;
    }
    result = 0;
    for(j=0; j<=cnt0-1; j++)
    {
        if( s0->flagarray.ptr.p_int[s0->data.ptr.p_int[idxbegin0+j]]<0 )
        {
            result = result+1;
        }
    }
    for(j=0; j<=cnt1-1; j++)
    {
        s0->flagarray.ptr.p_int[s1->data.ptr.p_int[idxbegin1+j]] = -1;
    }
    return result;
}


/*************************************************************************
Counts elements of I-th set of S0 that are also present in K-th set of S1

INPUT PARAMETERS
    S0          -   kn-set structure
    I           -   set index in the structure S0
    S1          -   kn-set to compare against
    K           -   set index in the structure S1
    
RESULT
    count

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static ae_int_t amdordering_knscountandkth(amdknset* s0,
     ae_int_t i,
     const amdknset* s1,
     ae_int_t k,
     ae_state *_state)
{
    ae_int_t idxbegin0;
    ae_int_t idxbegin1;
    ae_int_t cnt0;
    ae_int_t cnt1;
    ae_int_t j;
    ae_int_t result;


    cnt0 = s0->vcnt.ptr.p_int[i];
    cnt1 = s1->vcnt.ptr.p_int[k];
    idxbegin0 = s0->vbegin.ptr.p_int[i];
    idxbegin1 = s1->vbegin.ptr.p_int[k];
    for(j=0; j<=cnt1-1; j++)
    {
        s0->flagarray.ptr.p_int[s1->data.ptr.p_int[idxbegin1+j]] = 1;
    }
    result = 0;
    for(j=0; j<=cnt0-1; j++)
    {
        if( s0->flagarray.ptr.p_int[s0->data.ptr.p_int[idxbegin0+j]]>0 )
        {
            result = result+1;
        }
    }
    for(j=0; j<=cnt1-1; j++)
    {
        s0->flagarray.ptr.p_int[s1->data.ptr.p_int[idxbegin1+j]] = -1;
    }
    return result;
}


/*************************************************************************
Sums elements in I-th set of S0, returns sum.

INPUT PARAMETERS
    S0          -   kn-set structure
    I           -   set index in the structure S0
    
RESULT
    sum

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static ae_int_t amdordering_knssumkth(const amdknset* s0,
     ae_int_t i,
     ae_state *_state)
{
    ae_int_t idxbegin0;
    ae_int_t cnt0;
    ae_int_t j;
    ae_int_t result;


    cnt0 = s0->vcnt.ptr.p_int[i];
    idxbegin0 = s0->vbegin.ptr.p_int[i];
    result = 0;
    for(j=0; j<=cnt0-1; j++)
    {
        result = result+s0->data.ptr.p_int[idxbegin0+j];
    }
    return result;
}


/*************************************************************************
Clear k-th kn-set in collection.

Freed memory is NOT reclaimed for future garbage collection.

INPUT PARAMETERS
    SA          -   kn-set structure
    K           -   set index
    
OUTPUT PARAMETERS
    SA          -   K-th set was cleared

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_knsclearkthnoreclaim(amdknset* sa,
     ae_int_t k,
     ae_state *_state)
{


    sa->vcnt.ptr.p_int[k] = 0;
}


/*************************************************************************
Clear k-th kn-set in collection.

Freed memory is reclaimed for future garbage collection. This function  is
NOT recommended if you intend to add elements to this set in some  future,
because every addition will result in  reallocation  of  previously  freed
memory. Use knsClearKthNoReclaim().

INPUT PARAMETERS
    SA          -   kn-set structure
    K           -   set index
    
OUTPUT PARAMETERS
    SA          -   K-th set was cleared

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_knsclearkthreclaim(amdknset* sa,
     ae_int_t k,
     ae_state *_state)
{
    ae_int_t idxbegin;
    ae_int_t allocated;


    idxbegin = sa->vbegin.ptr.p_int[k];
    allocated = sa->vallocated.ptr.p_int[k];
    sa->vcnt.ptr.p_int[k] = 0;
    if( allocated>=amdordering_knsheadersize )
    {
        sa->data.ptr.p_int[idxbegin-2] = 2;
        sa->data.ptr.p_int[idxbegin+0] = allocated;
        sa->data.ptr.p_int[idxbegin+1] = -1;
        sa->vallocated.ptr.p_int[k] = 0;
    }
}


/*************************************************************************
Initialize linked list matrix

INPUT PARAMETERS
    N           -   matrix size
    
OUTPUT PARAMETERS
    A           -   NxN linked list matrix

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_mtxinit(ae_int_t n,
     amdllmatrix* a,
     ae_state *_state)
{


    a->n = n;
    isetallocv(2*n+1, -1, &a->vbegin, _state);
    isetallocv(n, 0, &a->vcolcnt, _state);
    a->entriesinitialized = 0;
}


/*************************************************************************
Adds column from matrix to n-set

INPUT PARAMETERS
    A           -   NxN linked list matrix
    J           -   column index to add
    S           -   target n-set
    
OUTPUT PARAMETERS
    S           -   elements from J-th column are added to S
    

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_mtxaddcolumnto(const amdllmatrix* a,
     ae_int_t j,
     niset* s,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t eidx;


    n = a->n;
    eidx = a->vbegin.ptr.p_int[n+j];
    while(eidx>=0)
    {
        nisaddelement(s, a->entries.ptr.p_int[eidx*amdordering_llmentrysize+4], _state);
        eidx = a->entries.ptr.p_int[eidx*amdordering_llmentrysize+3];
    }
}


/*************************************************************************
Inserts new element into column J, row I. The caller guarantees  that  the
element being inserted is NOT already present in the matrix.

INPUT PARAMETERS
    A           -   NxN linked list matrix
    I           -   row index
    J           -   column index
    
OUTPUT PARAMETERS
    A           -   element (I,J) added to the list.
    

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_mtxinsertnewelement(amdllmatrix* a,
     ae_int_t i,
     ae_int_t j,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t k;
    ae_int_t newsize;
    ae_int_t eidx;
    ae_int_t offs;


    n = a->n;
    if( a->vbegin.ptr.p_int[2*n]<0 )
    {
        newsize = 2*a->entriesinitialized+1;
        ivectorresize(&a->entries, newsize*amdordering_llmentrysize, _state);
        for(k=a->entriesinitialized; k<=newsize-2; k++)
        {
            a->entries.ptr.p_int[k*amdordering_llmentrysize+0] = k+1;
        }
        a->entries.ptr.p_int[(newsize-1)*amdordering_llmentrysize+0] = a->vbegin.ptr.p_int[2*n];
        a->vbegin.ptr.p_int[2*n] = a->entriesinitialized;
        a->entriesinitialized = newsize;
    }
    eidx = a->vbegin.ptr.p_int[2*n];
    offs = eidx*amdordering_llmentrysize;
    a->vbegin.ptr.p_int[2*n] = a->entries.ptr.p_int[offs+0];
    a->entries.ptr.p_int[offs+0] = -1;
    a->entries.ptr.p_int[offs+1] = a->vbegin.ptr.p_int[i];
    if( a->vbegin.ptr.p_int[i]>=0 )
    {
        a->entries.ptr.p_int[a->vbegin.ptr.p_int[i]*amdordering_llmentrysize+0] = eidx;
    }
    a->entries.ptr.p_int[offs+2] = -1;
    a->entries.ptr.p_int[offs+3] = a->vbegin.ptr.p_int[j+n];
    if( a->vbegin.ptr.p_int[j+n]>=0 )
    {
        a->entries.ptr.p_int[a->vbegin.ptr.p_int[j+n]*amdordering_llmentrysize+2] = eidx;
    }
    a->entries.ptr.p_int[offs+4] = i;
    a->entries.ptr.p_int[offs+5] = j;
    a->vbegin.ptr.p_int[i] = eidx;
    a->vbegin.ptr.p_int[j+n] = eidx;
    a->vcolcnt.ptr.p_int[j] = a->vcolcnt.ptr.p_int[j]+1;
}


/*************************************************************************
Counts elements in J-th column that are not present in n-set S

INPUT PARAMETERS
    A           -   NxN linked list matrix
    J           -   column index
    S           -   n-set to compare against
    
RESULT
    element count
    

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static ae_int_t amdordering_mtxcountcolumnnot(const amdllmatrix* a,
     ae_int_t j,
     const niset* s,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t eidx;
    ae_int_t result;


    n = a->n;
    result = 0;
    eidx = a->vbegin.ptr.p_int[n+j];
    while(eidx>=0)
    {
        if( s->locationof.ptr.p_int[a->entries.ptr.p_int[eidx*amdordering_llmentrysize+4]]<0 )
        {
            result = result+1;
        }
        eidx = a->entries.ptr.p_int[eidx*amdordering_llmentrysize+3];
    }
    return result;
}


/*************************************************************************
Counts elements in J-th column

INPUT PARAMETERS
    A           -   NxN linked list matrix
    J           -   column index
    
RESULT
    element count
    

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static ae_int_t amdordering_mtxcountcolumn(const amdllmatrix* a,
     ae_int_t j,
     ae_state *_state)
{
    ae_int_t result;


    result = a->vcolcnt.ptr.p_int[j];
    return result;
}


/*************************************************************************
Clears K-th column or row

INPUT PARAMETERS
    A           -   NxN linked list matrix
    K           -   column/row index to clear
    IsCol       -   whether we want to clear row or column
    
OUTPUT PARAMETERS
    A           -   K-th column or row is empty
    

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_mtxclearx(amdllmatrix* a,
     ae_int_t k,
     ae_bool iscol,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t eidx;
    ae_int_t enext;
    ae_int_t idxprev;
    ae_int_t idxnext;
    ae_int_t idxr;
    ae_int_t idxc;


    n = a->n;
    if( iscol )
    {
        eidx = a->vbegin.ptr.p_int[n+k];
    }
    else
    {
        eidx = a->vbegin.ptr.p_int[k];
    }
    while(eidx>=0)
    {
        idxr = a->entries.ptr.p_int[eidx*amdordering_llmentrysize+4];
        idxc = a->entries.ptr.p_int[eidx*amdordering_llmentrysize+5];
        if( iscol )
        {
            enext = a->entries.ptr.p_int[eidx*amdordering_llmentrysize+3];
        }
        else
        {
            enext = a->entries.ptr.p_int[eidx*amdordering_llmentrysize+1];
        }
        idxprev = a->entries.ptr.p_int[eidx*amdordering_llmentrysize+0];
        idxnext = a->entries.ptr.p_int[eidx*amdordering_llmentrysize+1];
        if( idxprev>=0 )
        {
            a->entries.ptr.p_int[idxprev*amdordering_llmentrysize+1] = idxnext;
        }
        else
        {
            a->vbegin.ptr.p_int[idxr] = idxnext;
        }
        if( idxnext>=0 )
        {
            a->entries.ptr.p_int[idxnext*amdordering_llmentrysize+0] = idxprev;
        }
        idxprev = a->entries.ptr.p_int[eidx*amdordering_llmentrysize+2];
        idxnext = a->entries.ptr.p_int[eidx*amdordering_llmentrysize+3];
        if( idxprev>=0 )
        {
            a->entries.ptr.p_int[idxprev*amdordering_llmentrysize+3] = idxnext;
        }
        else
        {
            a->vbegin.ptr.p_int[idxc+n] = idxnext;
        }
        if( idxnext>=0 )
        {
            a->entries.ptr.p_int[idxnext*amdordering_llmentrysize+2] = idxprev;
        }
        a->entries.ptr.p_int[eidx*amdordering_llmentrysize+0] = a->vbegin.ptr.p_int[2*n];
        a->vbegin.ptr.p_int[2*n] = eidx;
        eidx = enext;
        if( !iscol )
        {
            a->vcolcnt.ptr.p_int[idxc] = a->vcolcnt.ptr.p_int[idxc]-1;
        }
    }
    if( iscol )
    {
        a->vcolcnt.ptr.p_int[k] = 0;
    }
}


/*************************************************************************
Clears J-th column

INPUT PARAMETERS
    A           -   NxN linked list matrix
    J           -   column index to clear
    
OUTPUT PARAMETERS
    A           -   J-th column is empty
    

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_mtxclearcolumn(amdllmatrix* a,
     ae_int_t j,
     ae_state *_state)
{


    amdordering_mtxclearx(a, j, ae_true, _state);
}


/*************************************************************************
Clears J-th row

INPUT PARAMETERS
    A           -   NxN linked list matrix
    J           -   row index to clear
    
OUTPUT PARAMETERS
    A           -   J-th row is empty
    

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_mtxclearrow(amdllmatrix* a,
     ae_int_t j,
     ae_state *_state)
{


    amdordering_mtxclearx(a, j, ae_false, _state);
}


/*************************************************************************
Initialize vertex storage using A to estimate initial degrees

INPUT PARAMETERS
    A           -   NxN lower triangular sparse CRS matrix
    N           -   problem size
    Eligible    -   array[N], only eligible vertices can be extracted
                    with vtxGetApproxMinDegree()
    HasEligible -   if False, Eligible is ignored
    CheckExactDegrees-
                    whether we want to maintain additional exact degress
                    (the search is still done using approximate ones)
    
OUTPUT PARAMETERS
    S           -   vertex set
    

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_vtxinit(const sparsematrix* a,
     ae_int_t n,
     /* Boolean */ const ae_vector* eligible,
     ae_bool haseligible,
     ae_bool checkexactdegrees,
     amdvertexset* s,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t jj;
    ae_int_t j0;
    ae_int_t j1;

    _amdvertexset_clear(s);

    s->n = n;
    s->checkexactdegrees = checkexactdegrees;
    s->smallestdegree = 0;
    bsetallocv(n, ae_true, &s->isvertex, _state);
    if( haseligible )
    {
        bcopyallocv(n, eligible, &s->eligible, _state);
    }
    else
    {
        bsetallocv(n, ae_true, &s->eligible, _state);
    }
    isetallocv(n, 0, &s->approxd, _state);
    for(i=0; i<=n-1; i++)
    {
        j0 = a->ridx.ptr.p_int[i];
        j1 = a->didx.ptr.p_int[i]-1;
        s->approxd.ptr.p_int[i] = j1-j0+1;
        for(jj=j0; jj<=j1; jj++)
        {
            j = a->idx.ptr.p_int[jj];
            s->approxd.ptr.p_int[j] = s->approxd.ptr.p_int[j]+1;
        }
    }
    if( checkexactdegrees )
    {
        icopyallocv(n, &s->approxd, &s->optionalexactd, _state);
    }
    isetallocv(n, -1, &s->vbegin, _state);
    isetallocv(n, -1, &s->vprev, _state);
    isetallocv(n, -1, &s->vnext, _state);
    for(i=0; i<=n-1; i++)
    {
        if( s->eligible.ptr.p_bool[i] )
        {
            j = s->approxd.ptr.p_int[i];
            j0 = s->vbegin.ptr.p_int[j];
            s->vbegin.ptr.p_int[j] = i;
            s->vnext.ptr.p_int[i] = j0;
            s->vprev.ptr.p_int[i] = -1;
            if( j0>=0 )
            {
                s->vprev.ptr.p_int[j0] = i;
            }
        }
    }
}


/*************************************************************************
Removes vertex from the storage

INPUT PARAMETERS
    S           -   vertex set
    P           -   vertex to be removed
    
OUTPUT PARAMETERS
    S           -   modified
    

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_vtxremovevertex(amdvertexset* s,
     ae_int_t p,
     ae_state *_state)
{
    ae_int_t d;
    ae_int_t idxprev;
    ae_int_t idxnext;


    ae_assert(s->isvertex.ptr.p_bool[p], "AMD: trying to remove already removed vertex", _state);
    if( s->eligible.ptr.p_bool[p] )
    {
        d = s->approxd.ptr.p_int[p];
        idxprev = s->vprev.ptr.p_int[p];
        idxnext = s->vnext.ptr.p_int[p];
        if( idxprev>=0 )
        {
            s->vnext.ptr.p_int[idxprev] = idxnext;
        }
        else
        {
            s->vbegin.ptr.p_int[d] = idxnext;
        }
        if( idxnext>=0 )
        {
            s->vprev.ptr.p_int[idxnext] = idxprev;
        }
    }
    s->eligible.ptr.p_bool[p] = ae_false;
    s->isvertex.ptr.p_bool[p] = ae_false;
    s->approxd.ptr.p_int[p] = -9999999;
    if( s->checkexactdegrees )
    {
        s->optionalexactd.ptr.p_int[p] = -9999999;
    }
}


/*************************************************************************
Get approximate degree.

Fails for removed or non-eligible vertexes.

INPUT PARAMETERS
    S           -   vertex set
    P           -   vertex index
    
RESULT
    vertex degree
    

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static ae_int_t amdordering_vtxgetapprox(const amdvertexset* s,
     ae_int_t p,
     ae_state *_state)
{
    ae_int_t result;


    ae_assert(s->isvertex.ptr.p_bool[p], "AMD: trying to call vtxGetApprox() for removed vertex", _state);
    result = s->approxd.ptr.p_int[p];
    return result;
}


/*************************************************************************
Get exact degree (or 0, if not supported).  Result is undefined for
removed vertexes.

INPUT PARAMETERS
    S           -   vertex set
    P           -   vertex index
    
RESULT
    vertex degree
    

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static ae_int_t amdordering_vtxgetexact(const amdvertexset* s,
     ae_int_t p,
     ae_state *_state)
{
    ae_int_t result;


    if( s->checkexactdegrees )
    {
        result = s->optionalexactd.ptr.p_int[p];
    }
    else
    {
        result = 0;
    }
    return result;
}


/*************************************************************************
Returns index of vertex with minimum approximate degree, or -1 when there
is no vertex.

INPUT PARAMETERS
    S           -   vertex set
    
RESULT
    vertex index, or -1
    

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static ae_int_t amdordering_vtxgetapproxmindegree(amdvertexset* s,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;
    ae_int_t result;


    n = s->n;
    result = -1;
    for(i=s->smallestdegree; i<=n-1; i++)
    {
        if( s->vbegin.ptr.p_int[i]>=0 )
        {
            s->smallestdegree = i;
            result = s->vbegin.ptr.p_int[i];
            return result;
        }
    }
    return result;
}


/*************************************************************************
Update approximate degree

INPUT PARAMETERS
    S           -   vertex set
    P           -   vertex to be updated
    DNew        -   new degree
    
OUTPUT PARAMETERS
    S           -   modified
    

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_vtxupdateapproximatedegree(amdvertexset* s,
     ae_int_t p,
     ae_int_t dnew,
     ae_state *_state)
{
    ae_int_t dold;
    ae_int_t idxprev;
    ae_int_t idxnext;
    ae_int_t oldbegin;


    ae_assert(s->isvertex.ptr.p_bool[p], "AMD: trying to call vtxUpdateApproximateDegree() for removed vertex", _state);
    dold = s->approxd.ptr.p_int[p];
    if( dold==dnew )
    {
        return;
    }
    s->approxd.ptr.p_int[p] = dnew;
    if( s->eligible.ptr.p_bool[p] )
    {
        idxprev = s->vprev.ptr.p_int[p];
        idxnext = s->vnext.ptr.p_int[p];
        if( idxprev>=0 )
        {
            s->vnext.ptr.p_int[idxprev] = idxnext;
        }
        else
        {
            s->vbegin.ptr.p_int[dold] = idxnext;
        }
        if( idxnext>=0 )
        {
            s->vprev.ptr.p_int[idxnext] = idxprev;
        }
        oldbegin = s->vbegin.ptr.p_int[dnew];
        s->vbegin.ptr.p_int[dnew] = p;
        s->vnext.ptr.p_int[p] = oldbegin;
        s->vprev.ptr.p_int[p] = -1;
        if( oldbegin>=0 )
        {
            s->vprev.ptr.p_int[oldbegin] = p;
        }
        if( dnew<s->smallestdegree )
        {
            s->smallestdegree = dnew;
        }
    }
}


/*************************************************************************
Update optional exact degree. Silently returns if vertex set does not store
exact degrees.

INPUT PARAMETERS
    S           -   vertex set
    P           -   vertex to be updated
    D           -   new degree
    
OUTPUT PARAMETERS
    S           -   modified
    

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_vtxupdateexactdegree(amdvertexset* s,
     ae_int_t p,
     ae_int_t d,
     ae_state *_state)
{


    if( !s->checkexactdegrees )
    {
        return;
    }
    s->optionalexactd.ptr.p_int[p] = d;
}


/*************************************************************************
This function selects K-th  pivot  with  minimum  approximate  degree  and
generates permutation that reorders variable to the K-th position  in  the
matrix.

Due to supernodal structure of the matrix more than one pivot variable can
be selected and moved to the beginning. The actual count of pivots selected
is returned in NodeSize.

INPUT PARAMETERS
    Buf         -   properly initialized buffer object
    K           -   pivot index
    
OUTPUT PARAMETERS
    Buf.Perm    -   entries [K,K+NodeSize) are initialized by permutation
    Buf.InvPerm -   entries [K,K+NodeSize) are initialized by permutation
    Buf.ColumnSwaps-entries [K,K+NodeSize) are initialized by permutation
    P           -   pivot supervariable
    NodeSize    -   supernode size
    
If P<0, then we exhausted all eligible vertices, nothing is returned.

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_amdselectpivotelement(amdbuffer* buf,
     ae_int_t k,
     ae_int_t* p,
     ae_int_t* nodesize,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;

    *p = 0;
    *nodesize = 0;

    *p = amdordering_vtxgetapproxmindegree(&buf->vertexdegrees, _state);
    if( *p<0 )
    {
        return;
    }
    ae_assert(amdordering_vtxgetapprox(&buf->vertexdegrees, *p, _state)>=0, "integrity check RDFD2 failed", _state);
    *nodesize = 0;
    amdordering_knsstartenumeration(&buf->setsuper, *p, _state);
    while(amdordering_knsenumerate(&buf->setsuper, &j, _state))
    {
        i = buf->perm.ptr.p_int[j];
        buf->columnswaps.ptr.p_int[k+(*nodesize)] = i;
        buf->invperm.ptr.p_int[i] = buf->invperm.ptr.p_int[k+(*nodesize)];
        buf->invperm.ptr.p_int[k+(*nodesize)] = j;
        buf->perm.ptr.p_int[buf->invperm.ptr.p_int[i]] = i;
        buf->perm.ptr.p_int[buf->invperm.ptr.p_int[k+(*nodesize)]] = k+(*nodesize);
        inc(nodesize, _state);
    }
    ae_assert(amdordering_vtxgetapprox(&buf->vertexdegrees, *p, _state)>=0&&(!buf->checkexactdegrees||amdordering_vtxgetexact(&buf->vertexdegrees, *p, _state)>=0), "AMD: integrity check RDFD failed", _state);
}


/*************************************************************************
This function computes nonzero pattern of Lp, the column that is added  to
the lower triangular Cholesky factor.

INPUT PARAMETERS
    Buf         -   properly initialized buffer object
    P           -   pivot column
    
OUTPUT PARAMETERS
    Buf.setP    -   initialized with setSuper[P]
    Buf.Lp      -   initialized with Lp\P
    Buf.setRp   -   initialized with Lp\{P+Q}
    Buf.Ep      -   initialized with setE[P]
    Buf.mtxL    -   L := L+Lp
    Buf.Ls      -   first Buf.LSCnt elements contain subset of Lp elements
                    that are principal nodes in supervariables.

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_amdcomputelp(amdbuffer* buf,
     ae_int_t p,
     ae_state *_state)
{
    ae_int_t i;


    nisclear(&buf->setp, _state);
    amdordering_nsaddkth(&buf->setp, &buf->setsuper, p, _state);
    nisclear(&buf->lp, _state);
    amdordering_nsaddkth(&buf->lp, &buf->seta, p, _state);
    amdordering_knsstartenumeration(&buf->sete, p, _state);
    while(amdordering_knsenumerate(&buf->sete, &i, _state))
    {
        amdordering_mtxaddcolumnto(&buf->mtxl, i, &buf->lp, _state);
    }
    amdordering_nssubtractkth(&buf->lp, &buf->setsuper, p, _state);
    niscopy(&buf->lp, &buf->setrp, _state);
    nissubtract1(&buf->setrp, &buf->setq, _state);
    buf->lscnt = 0;
    nisstartenumeration(&buf->lp, _state);
    while(nisenumerate(&buf->lp, &i, _state))
    {
        ae_assert(!buf->iseliminated.ptr.p_bool[i], "AMD: integrity check 0740 failed", _state);
        amdordering_mtxinsertnewelement(&buf->mtxl, i, p, _state);
        if( buf->issupernode.ptr.p_bool[i] )
        {
            buf->ls.ptr.p_int[buf->lscnt] = i;
            buf->lscnt = buf->lscnt+1;
        }
    }
    nisclear(&buf->ep, _state);
    amdordering_nsaddkth(&buf->ep, &buf->sete, p, _state);
}


/*************************************************************************
Having output of AMDComputeLp() in the Buf object, this function  performs
mass elimination in the quotient graph.

INPUT PARAMETERS
    Buf         -   properly initialized buffer object
    P           -   pivot column
    K           -   number of already eliminated columns (P-th is not counted)
    Tau         -   variables with degrees higher than Tau will be classified
                    as quasidense
    
OUTPUT PARAMETERS
    Buf.setA    -   Lp is eliminated from setA
    Buf.setE    -   Ep is eliminated from setE, P is added
    approxD     -   updated
    Buf.setQSuperCand-   contains candidates for quasidense status assignment

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_amdmasselimination(amdbuffer* buf,
     ae_int_t p,
     ae_int_t k,
     ae_int_t tau,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t lidx;
    ae_int_t lpi;
    ae_int_t cntsuperi;
    ae_int_t cntq;
    ae_int_t cntainoti;
    ae_int_t cntainotqi;
    ae_int_t cntlpnoti;
    ae_int_t cntlpnotqi;
    ae_int_t cc;
    ae_int_t j;
    ae_int_t e;
    ae_int_t we;
    ae_int_t cnttoclean;
    ae_int_t idxbegin;
    ae_int_t idxend;
    ae_int_t jj;
    ae_int_t bnd0;
    ae_int_t bnd1;
    ae_int_t bnd2;
    ae_int_t d;


    n = buf->n;
    ivectorsetlengthatleast(&buf->tmp0, n, _state);
    cnttoclean = 0;
    for(lidx=0; lidx<=buf->lscnt-1; lidx++)
    {
        if( buf->setq.locationof.ptr.p_int[buf->ls.ptr.p_int[lidx]]<0 )
        {
            lpi = buf->ls.ptr.p_int[lidx];
            cntsuperi = amdordering_knscountkth(&buf->setsuper, lpi, _state);
            amdordering_knsdirectaccess(&buf->sete, lpi, &idxbegin, &idxend, _state);
            for(jj=idxbegin; jj<=idxend-1; jj++)
            {
                e = buf->sete.data.ptr.p_int[jj];
                we = buf->arrwe.ptr.p_int[e];
                if( we<0 )
                {
                    we = amdordering_mtxcountcolumnnot(&buf->mtxl, e, &buf->setq, _state);
                    buf->tmp0.ptr.p_int[cnttoclean] = e;
                    cnttoclean = cnttoclean+1;
                }
                buf->arrwe.ptr.p_int[e] = we-cntsuperi;
            }
        }
    }
    nisclear(&buf->setqsupercand, _state);
    for(lidx=0; lidx<=buf->lscnt-1; lidx++)
    {
        if( buf->setq.locationof.ptr.p_int[buf->ls.ptr.p_int[lidx]]<0 )
        {
            lpi = buf->ls.ptr.p_int[lidx];
            amdordering_knssubtract1(&buf->seta, lpi, &buf->lp, _state);
            amdordering_knssubtract1(&buf->seta, lpi, &buf->setp, _state);
            amdordering_knssubtract1(&buf->sete, lpi, &buf->ep, _state);
            amdordering_knsaddnewelement(&buf->sete, lpi, p, _state);
            if( buf->extendeddebug )
            {
                ae_assert(amdordering_knscountnotkth(&buf->seta, lpi, &buf->setsuper, lpi, _state)==amdordering_knscountkth(&buf->seta, lpi, _state), "AMD: integrity check 454F failed", _state);
                ae_assert(amdordering_knscountandkth(&buf->seta, lpi, &buf->setsuper, lpi, _state)==0, "AMD: integrity check kl5nv failed", _state);
                ae_assert(amdordering_nscountandkth(&buf->lp, &buf->setsuper, lpi, _state)==amdordering_knscountkth(&buf->setsuper, lpi, _state), "AMD: integrity check 8463 failed", _state);
            }
            cntq = niscount(&buf->setq, _state);
            cntsuperi = amdordering_knscountkth(&buf->setsuper, lpi, _state);
            cntainoti = amdordering_knscountkth(&buf->seta, lpi, _state);
            if( cntq>0 )
            {
                cntainotqi = amdordering_knscountnot(&buf->seta, lpi, &buf->setq, _state);
            }
            else
            {
                cntainotqi = cntainoti;
            }
            cntlpnoti = niscount(&buf->lp, _state)-cntsuperi;
            cntlpnotqi = niscount(&buf->setrp, _state)-cntsuperi;
            cc = 0;
            amdordering_knsdirectaccess(&buf->sete, lpi, &idxbegin, &idxend, _state);
            for(jj=idxbegin; jj<=idxend-1; jj++)
            {
                j = buf->sete.data.ptr.p_int[jj];
                if( j==p )
                {
                    continue;
                }
                e = buf->arrwe.ptr.p_int[j];
                if( e<0 )
                {
                    if( cntq>0 )
                    {
                        e = amdordering_mtxcountcolumnnot(&buf->mtxl, j, &buf->setq, _state);
                    }
                    else
                    {
                        e = amdordering_mtxcountcolumn(&buf->mtxl, j, _state);
                    }
                }
                cc = cc+e;
            }
            bnd0 = n-k-niscount(&buf->setp, _state);
            bnd1 = amdordering_vtxgetapprox(&buf->vertexdegrees, lpi, _state)+cntlpnoti;
            bnd2 = cntq+cntainotqi+cntlpnotqi+cc;
            d = imin3(bnd0, bnd1, bnd2, _state);
            amdordering_vtxupdateapproximatedegree(&buf->vertexdegrees, lpi, d, _state);
            if( tau>0&&d+cntsuperi>tau )
            {
                nisaddelement(&buf->setqsupercand, lpi, _state);
            }
            if( buf->checkexactdegrees )
            {
                nisclear(&buf->exactdegreetmp0, _state);
                amdordering_knsstartenumeration(&buf->sete, lpi, _state);
                while(amdordering_knsenumerate(&buf->sete, &j, _state))
                {
                    amdordering_mtxaddcolumnto(&buf->mtxl, j, &buf->exactdegreetmp0, _state);
                }
                amdordering_vtxupdateexactdegree(&buf->vertexdegrees, lpi, cntainoti+amdordering_nscountnotkth(&buf->exactdegreetmp0, &buf->setsuper, lpi, _state), _state);
                ae_assert((amdordering_knscountkth(&buf->sete, lpi, _state)>2||cntq>0)||amdordering_vtxgetapprox(&buf->vertexdegrees, lpi, _state)==amdordering_vtxgetexact(&buf->vertexdegrees, lpi, _state), "AMD: integrity check 7206 failed", _state);
                ae_assert(amdordering_vtxgetapprox(&buf->vertexdegrees, lpi, _state)>=amdordering_vtxgetexact(&buf->vertexdegrees, lpi, _state), "AMD: integrity check 8206 failed", _state);
            }
        }
    }
    for(j=0; j<=cnttoclean-1; j++)
    {
        buf->arrwe.ptr.p_int[buf->tmp0.ptr.p_int[j]] = -1;
    }
}


/*************************************************************************
After mass elimination, but before removal of vertex  P,  we  may  perform
supernode detection. Only variables/supernodes in  Lp  (P  itself  is  NOT
included) can be merged into larger supernodes.

INPUT PARAMETERS
    Buf         -   properly initialized buffer object
    
OUTPUT PARAMETERS
    Buf         -   following fields of Buf may be modified:
                    * Buf.setSuper
                    * Buf.setA
                    * Buf.setE
                    * Buf.IsSupernode
                    * ApproxD and ExactD

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_amddetectsupernodes(amdbuffer* buf,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    ae_int_t cnt;
    ae_int_t lpi;
    ae_int_t lpj;
    ae_int_t nj;
    ae_int_t hashi;


    n = buf->n;
    ivectorsetlengthatleast(&buf->sncandidates, n, _state);
    if( buf->lscnt<2 )
    {
        return;
    }
    for(i=0; i<=buf->lscnt-1; i++)
    {
        if( buf->setq.locationof.ptr.p_int[buf->ls.ptr.p_int[i]]<0 )
        {
            lpi = buf->ls.ptr.p_int[i];
            hashi = (amdordering_knssumkth(&buf->seta, lpi, _state)+amdordering_knssumkth(&buf->sete, lpi, _state))%n;
            nisaddelement(&buf->nonemptybuckets, hashi, _state);
            amdordering_knsaddnewelement(&buf->hashbuckets, hashi, lpi, _state);
        }
    }
    nisstartenumeration(&buf->nonemptybuckets, _state);
    while(nisenumerate(&buf->nonemptybuckets, &hashi, _state))
    {
        if( amdordering_knscountkth(&buf->hashbuckets, hashi, _state)>=2 )
        {
            cnt = 0;
            amdordering_knsstartenumeration(&buf->hashbuckets, hashi, _state);
            while(amdordering_knsenumerate(&buf->hashbuckets, &i, _state))
            {
                buf->sncandidates.ptr.p_int[cnt] = i;
                cnt = cnt+1;
            }
            for(i=cnt-1; i>=0; i--)
            {
                for(j=cnt-1; j>=i+1; j--)
                {
                    if( buf->issupernode.ptr.p_bool[buf->sncandidates.ptr.p_int[i]]&&buf->issupernode.ptr.p_bool[buf->sncandidates.ptr.p_int[j]] )
                    {
                        lpi = buf->sncandidates.ptr.p_int[i];
                        lpj = buf->sncandidates.ptr.p_int[j];
                        if( buf->iseligible.ptr.p_bool[buf->sncandidates.ptr.p_int[i]]&&!buf->iseligible.ptr.p_bool[buf->sncandidates.ptr.p_int[j]] )
                        {
                            continue;
                        }
                        if( !buf->iseligible.ptr.p_bool[buf->sncandidates.ptr.p_int[i]]&&buf->iseligible.ptr.p_bool[buf->sncandidates.ptr.p_int[j]] )
                        {
                            continue;
                        }
                        nisclear(&buf->adji, _state);
                        nisclear(&buf->adjj, _state);
                        amdordering_nsaddkth(&buf->adji, &buf->seta, lpi, _state);
                        amdordering_nsaddkth(&buf->adjj, &buf->seta, lpj, _state);
                        amdordering_nsaddkth(&buf->adji, &buf->sete, lpi, _state);
                        amdordering_nsaddkth(&buf->adjj, &buf->sete, lpj, _state);
                        nisaddelement(&buf->adji, lpi, _state);
                        nisaddelement(&buf->adji, lpj, _state);
                        nisaddelement(&buf->adjj, lpi, _state);
                        nisaddelement(&buf->adjj, lpj, _state);
                        if( !nisequal(&buf->adji, &buf->adjj, _state) )
                        {
                            continue;
                        }
                        if( buf->extendeddebug )
                        {
                            ae_assert(amdordering_vtxgetapprox(&buf->vertexdegrees, lpi, _state)>=1&&(!buf->checkexactdegrees||amdordering_vtxgetexact(&buf->vertexdegrees, lpi, _state)>=1), "AMD: integrity check &GBFF1 failed", _state);
                            ae_assert(amdordering_vtxgetapprox(&buf->vertexdegrees, lpj, _state)>=1&&(!buf->checkexactdegrees||amdordering_vtxgetexact(&buf->vertexdegrees, lpj, _state)>=1), "AMD: integrity check &GBFF2 failed", _state);
                            ae_assert(amdordering_knscountandkth(&buf->setsuper, lpi, &buf->setsuper, lpj, _state)==0, "AMD: integrity check &GBFF3 failed", _state);
                        }
                        nj = amdordering_knscountkth(&buf->setsuper, lpj, _state);
                        amdordering_knsaddkthdistinct(&buf->setsuper, lpi, &buf->setsuper, lpj, _state);
                        amdordering_knsclearkthreclaim(&buf->setsuper, lpj, _state);
                        amdordering_knsclearkthreclaim(&buf->seta, lpj, _state);
                        amdordering_knsclearkthreclaim(&buf->sete, lpj, _state);
                        buf->issupernode.ptr.p_bool[lpj] = ae_false;
                        amdordering_vtxremovevertex(&buf->vertexdegrees, lpj, _state);
                        amdordering_vtxupdateapproximatedegree(&buf->vertexdegrees, lpi, amdordering_vtxgetapprox(&buf->vertexdegrees, lpi, _state)-nj, _state);
                        if( buf->checkexactdegrees )
                        {
                            amdordering_vtxupdateexactdegree(&buf->vertexdegrees, lpi, amdordering_vtxgetexact(&buf->vertexdegrees, lpi, _state)-nj, _state);
                        }
                    }
                }
            }
        }
        amdordering_knsclearkthnoreclaim(&buf->hashbuckets, hashi, _state);
    }
    nisclear(&buf->nonemptybuckets, _state);
}


/*************************************************************************
Assign quasidense status to proposed supervars,  perform all the necessary
cleanup (remove vertices, etc)

INPUT PARAMETERS
    Buf         -   properly initialized buffer object
    Cand        -   supervariables to be moved to quasidense status
    P           -   current pivot element (used for integrity checks)
                    or -1, when this function is used for initial status
                    assignment.
    
OUTPUT PARAMETERS
    Buf         -   variables belonging  to  supervariables  in  cand  are
                    added to SetQ. Supervariables are removed from all lists

  -- ALGLIB PROJECT --
     Copyright 15.11.2021 by Bochkanov Sergey.
*************************************************************************/
static void amdordering_amdmovetoquasidense(amdbuffer* buf,
     niset* cand,
     ae_int_t p,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;


    nisstartenumeration(cand, _state);
    while(nisenumerate(cand, &j, _state))
    {
        ae_assert(j!=p, "AMD: integrity check 9464 failed", _state);
        ae_assert(buf->issupernode.ptr.p_bool[j], "AMD: integrity check 6284 failed", _state);
        ae_assert(!buf->iseliminated.ptr.p_bool[j], "AMD: integrity check 3858 failed", _state);
        amdordering_knsstartenumeration(&buf->setsuper, j, _state);
        while(amdordering_knsenumerate(&buf->setsuper, &i, _state))
        {
            nisaddelement(&buf->setq, i, _state);
        }
        amdordering_knsclearkthreclaim(&buf->seta, j, _state);
        amdordering_knsclearkthreclaim(&buf->sete, j, _state);
        buf->issupernode.ptr.p_bool[j] = ae_false;
        amdordering_vtxremovevertex(&buf->vertexdegrees, j, _state);
    }
}


void _amdknset_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    amdknset *p = (amdknset*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->flagarray, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->vbegin, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->vallocated, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->vcnt, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->data, 0, DT_INT, _state, make_automatic);
}


void _amdknset_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    amdknset       *dst = (amdknset*)_dst;
    const amdknset *src = (const amdknset*)_src;
    dst->k = src->k;
    dst->n = src->n;
    ae_vector_init_copy(&dst->flagarray, &src->flagarray, _state, make_automatic);
    ae_vector_init_copy(&dst->vbegin, &src->vbegin, _state, make_automatic);
    ae_vector_init_copy(&dst->vallocated, &src->vallocated, _state, make_automatic);
    ae_vector_init_copy(&dst->vcnt, &src->vcnt, _state, make_automatic);
    ae_vector_init_copy(&dst->data, &src->data, _state, make_automatic);
    dst->dataused = src->dataused;
    dst->iterrow = src->iterrow;
    dst->iteridx = src->iteridx;
}


void _amdknset_clear(void* _p)
{
    amdknset *p = (amdknset*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->flagarray);
    ae_vector_clear(&p->vbegin);
    ae_vector_clear(&p->vallocated);
    ae_vector_clear(&p->vcnt);
    ae_vector_clear(&p->data);
}


void _amdknset_destroy(void* _p)
{
    amdknset *p = (amdknset*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->flagarray);
    ae_vector_destroy(&p->vbegin);
    ae_vector_destroy(&p->vallocated);
    ae_vector_destroy(&p->vcnt);
    ae_vector_destroy(&p->data);
}


void _amdvertexset_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    amdvertexset *p = (amdvertexset*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->approxd, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->optionalexactd, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->isvertex, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->eligible, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->vbegin, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->vprev, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->vnext, 0, DT_INT, _state, make_automatic);
}


void _amdvertexset_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    amdvertexset       *dst = (amdvertexset*)_dst;
    const amdvertexset *src = (const amdvertexset*)_src;
    dst->n = src->n;
    dst->checkexactdegrees = src->checkexactdegrees;
    dst->smallestdegree = src->smallestdegree;
    ae_vector_init_copy(&dst->approxd, &src->approxd, _state, make_automatic);
    ae_vector_init_copy(&dst->optionalexactd, &src->optionalexactd, _state, make_automatic);
    ae_vector_init_copy(&dst->isvertex, &src->isvertex, _state, make_automatic);
    ae_vector_init_copy(&dst->eligible, &src->eligible, _state, make_automatic);
    ae_vector_init_copy(&dst->vbegin, &src->vbegin, _state, make_automatic);
    ae_vector_init_copy(&dst->vprev, &src->vprev, _state, make_automatic);
    ae_vector_init_copy(&dst->vnext, &src->vnext, _state, make_automatic);
}


void _amdvertexset_clear(void* _p)
{
    amdvertexset *p = (amdvertexset*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->approxd);
    ae_vector_clear(&p->optionalexactd);
    ae_vector_clear(&p->isvertex);
    ae_vector_clear(&p->eligible);
    ae_vector_clear(&p->vbegin);
    ae_vector_clear(&p->vprev);
    ae_vector_clear(&p->vnext);
}


void _amdvertexset_destroy(void* _p)
{
    amdvertexset *p = (amdvertexset*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->approxd);
    ae_vector_destroy(&p->optionalexactd);
    ae_vector_destroy(&p->isvertex);
    ae_vector_destroy(&p->eligible);
    ae_vector_destroy(&p->vbegin);
    ae_vector_destroy(&p->vprev);
    ae_vector_destroy(&p->vnext);
}


void _amdllmatrix_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    amdllmatrix *p = (amdllmatrix*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->vbegin, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->vcolcnt, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->entries, 0, DT_INT, _state, make_automatic);
}


void _amdllmatrix_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    amdllmatrix       *dst = (amdllmatrix*)_dst;
    const amdllmatrix *src = (const amdllmatrix*)_src;
    dst->n = src->n;
    ae_vector_init_copy(&dst->vbegin, &src->vbegin, _state, make_automatic);
    ae_vector_init_copy(&dst->vcolcnt, &src->vcolcnt, _state, make_automatic);
    ae_vector_init_copy(&dst->entries, &src->entries, _state, make_automatic);
    dst->entriesinitialized = src->entriesinitialized;
}


void _amdllmatrix_clear(void* _p)
{
    amdllmatrix *p = (amdllmatrix*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->vbegin);
    ae_vector_clear(&p->vcolcnt);
    ae_vector_clear(&p->entries);
}


void _amdllmatrix_destroy(void* _p)
{
    amdllmatrix *p = (amdllmatrix*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->vbegin);
    ae_vector_destroy(&p->vcolcnt);
    ae_vector_destroy(&p->entries);
}


void _amdbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    amdbuffer *p = (amdbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->iseliminated, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->issupernode, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->iseligible, 0, DT_BOOL, _state, make_automatic);
    _amdknset_init(&p->setsuper, _state, make_automatic);
    _amdknset_init(&p->seta, _state, make_automatic);
    _amdknset_init(&p->sete, _state, make_automatic);
    _amdllmatrix_init(&p->mtxl, _state, make_automatic);
    _amdvertexset_init(&p->vertexdegrees, _state, make_automatic);
    _niset_init(&p->setq, _state, make_automatic);
    ae_vector_init(&p->perm, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->invperm, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->columnswaps, 0, DT_INT, _state, make_automatic);
    _niset_init(&p->setp, _state, make_automatic);
    _niset_init(&p->lp, _state, make_automatic);
    _niset_init(&p->setrp, _state, make_automatic);
    _niset_init(&p->ep, _state, make_automatic);
    _niset_init(&p->adji, _state, make_automatic);
    _niset_init(&p->adjj, _state, make_automatic);
    ae_vector_init(&p->ls, 0, DT_INT, _state, make_automatic);
    _niset_init(&p->setqsupercand, _state, make_automatic);
    _niset_init(&p->exactdegreetmp0, _state, make_automatic);
    _amdknset_init(&p->hashbuckets, _state, make_automatic);
    _niset_init(&p->nonemptybuckets, _state, make_automatic);
    ae_vector_init(&p->sncandidates, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->arrwe, 0, DT_INT, _state, make_automatic);
    ae_matrix_init(&p->dbga, 0, 0, DT_REAL, _state, make_automatic);
}


void _amdbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    amdbuffer       *dst = (amdbuffer*)_dst;
    const amdbuffer *src = (const amdbuffer*)_src;
    dst->n = src->n;
    dst->extendeddebug = src->extendeddebug;
    dst->checkexactdegrees = src->checkexactdegrees;
    ae_vector_init_copy(&dst->iseliminated, &src->iseliminated, _state, make_automatic);
    ae_vector_init_copy(&dst->issupernode, &src->issupernode, _state, make_automatic);
    ae_vector_init_copy(&dst->iseligible, &src->iseligible, _state, make_automatic);
    _amdknset_init_copy(&dst->setsuper, &src->setsuper, _state, make_automatic);
    _amdknset_init_copy(&dst->seta, &src->seta, _state, make_automatic);
    _amdknset_init_copy(&dst->sete, &src->sete, _state, make_automatic);
    _amdllmatrix_init_copy(&dst->mtxl, &src->mtxl, _state, make_automatic);
    _amdvertexset_init_copy(&dst->vertexdegrees, &src->vertexdegrees, _state, make_automatic);
    _niset_init_copy(&dst->setq, &src->setq, _state, make_automatic);
    ae_vector_init_copy(&dst->perm, &src->perm, _state, make_automatic);
    ae_vector_init_copy(&dst->invperm, &src->invperm, _state, make_automatic);
    ae_vector_init_copy(&dst->columnswaps, &src->columnswaps, _state, make_automatic);
    _niset_init_copy(&dst->setp, &src->setp, _state, make_automatic);
    _niset_init_copy(&dst->lp, &src->lp, _state, make_automatic);
    _niset_init_copy(&dst->setrp, &src->setrp, _state, make_automatic);
    _niset_init_copy(&dst->ep, &src->ep, _state, make_automatic);
    _niset_init_copy(&dst->adji, &src->adji, _state, make_automatic);
    _niset_init_copy(&dst->adjj, &src->adjj, _state, make_automatic);
    ae_vector_init_copy(&dst->ls, &src->ls, _state, make_automatic);
    dst->lscnt = src->lscnt;
    _niset_init_copy(&dst->setqsupercand, &src->setqsupercand, _state, make_automatic);
    _niset_init_copy(&dst->exactdegreetmp0, &src->exactdegreetmp0, _state, make_automatic);
    _amdknset_init_copy(&dst->hashbuckets, &src->hashbuckets, _state, make_automatic);
    _niset_init_copy(&dst->nonemptybuckets, &src->nonemptybuckets, _state, make_automatic);
    ae_vector_init_copy(&dst->sncandidates, &src->sncandidates, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->arrwe, &src->arrwe, _state, make_automatic);
    ae_matrix_init_copy(&dst->dbga, &src->dbga, _state, make_automatic);
}


void _amdbuffer_clear(void* _p)
{
    amdbuffer *p = (amdbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->iseliminated);
    ae_vector_clear(&p->issupernode);
    ae_vector_clear(&p->iseligible);
    _amdknset_clear(&p->setsuper);
    _amdknset_clear(&p->seta);
    _amdknset_clear(&p->sete);
    _amdllmatrix_clear(&p->mtxl);
    _amdvertexset_clear(&p->vertexdegrees);
    _niset_clear(&p->setq);
    ae_vector_clear(&p->perm);
    ae_vector_clear(&p->invperm);
    ae_vector_clear(&p->columnswaps);
    _niset_clear(&p->setp);
    _niset_clear(&p->lp);
    _niset_clear(&p->setrp);
    _niset_clear(&p->ep);
    _niset_clear(&p->adji);
    _niset_clear(&p->adjj);
    ae_vector_clear(&p->ls);
    _niset_clear(&p->setqsupercand);
    _niset_clear(&p->exactdegreetmp0);
    _amdknset_clear(&p->hashbuckets);
    _niset_clear(&p->nonemptybuckets);
    ae_vector_clear(&p->sncandidates);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->arrwe);
    ae_matrix_clear(&p->dbga);
}


void _amdbuffer_destroy(void* _p)
{
    amdbuffer *p = (amdbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->iseliminated);
    ae_vector_destroy(&p->issupernode);
    ae_vector_destroy(&p->iseligible);
    _amdknset_destroy(&p->setsuper);
    _amdknset_destroy(&p->seta);
    _amdknset_destroy(&p->sete);
    _amdllmatrix_destroy(&p->mtxl);
    _amdvertexset_destroy(&p->vertexdegrees);
    _niset_destroy(&p->setq);
    ae_vector_destroy(&p->perm);
    ae_vector_destroy(&p->invperm);
    ae_vector_destroy(&p->columnswaps);
    _niset_destroy(&p->setp);
    _niset_destroy(&p->lp);
    _niset_destroy(&p->setrp);
    _niset_destroy(&p->ep);
    _niset_destroy(&p->adji);
    _niset_destroy(&p->adjj);
    ae_vector_destroy(&p->ls);
    _niset_destroy(&p->setqsupercand);
    _niset_destroy(&p->exactdegreetmp0);
    _amdknset_destroy(&p->hashbuckets);
    _niset_destroy(&p->nonemptybuckets);
    ae_vector_destroy(&p->sncandidates);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->arrwe);
    ae_matrix_destroy(&p->dbga);
}


/*$ End $*/
