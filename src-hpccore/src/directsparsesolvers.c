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


#include <stdafx.h>
#include "directsparsesolvers.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Sparse linear solver for A*x=b with N*N  sparse  real  symmetric  positive
definite matrix A, N*1 vectors x and b.

This solver  converts  input  matrix  to  SKS  format,  performs  Cholesky
factorization using  SKS  Cholesky  subroutine  (works  well  for  limited
bandwidth matrices) and uses sparse triangular solvers to get solution  of
the original system.

INPUT PARAMETERS
    A       -   sparse matrix, must be NxN exactly
    IsUpper -   which half of A is provided (another half is ignored)
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate or non-SPD system).

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void sparsespdsolvesks(const sparsematrix* a,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    sparsematrix a2;
    ae_int_t n;

    ae_frame_make(_state, &_frame_block);
    memset(&a2, 0, sizeof(a2));
    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);
    _sparsematrix_init(&a2, _state, ae_true);

    n = sparsegetnrows(a, _state);
    ae_assert(n>0, "SparseSPDSolveSKS: N<=0", _state);
    ae_assert(sparsegetnrows(a, _state)==n, "SparseSPDSolveSKS: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseSPDSolveSKS: cols(A)!=N", _state);
    ae_assert(b->cnt>=n, "SparseSPDSolveSKS: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseSPDSolveSKS: B contains infinities or NANs", _state);
    initsparsesolverreport(rep, _state);
    ae_vector_set_length(x, n, _state);
    sparsecopytosks(a, &a2, _state);
    if( !sparsecholeskyskyline(&a2, n, isupper, _state) )
    {
        rep->terminationtype = -3;
        for(i=0; i<=n-1; i++)
        {
            x->ptr.p_double[i] = (double)(0);
        }
        ae_frame_leave(_state);
        return;
    }
    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = b->ptr.p_double[i];
    }
    if( isupper )
    {
        sparsetrsv(&a2, isupper, ae_false, 1, x, _state);
        sparsetrsv(&a2, isupper, ae_false, 0, x, _state);
    }
    else
    {
        sparsetrsv(&a2, isupper, ae_false, 0, x, _state);
        sparsetrsv(&a2, isupper, ae_false, 1, x, _state);
    }
    rep->terminationtype = 1;
    ae_frame_leave(_state);
}


/*************************************************************************
Sparse linear solver for A*x=b with N*N  sparse  real  symmetric  positive
definite matrix A, N*1 vectors x and b.

This solver  converts  input  matrix  to  CRS  format,  performs  Cholesky
factorization using supernodal Cholesky  decomposition  with  permutation-
reducing ordering and uses sparse triangular solver to get solution of the
original system.

INPUT PARAMETERS
    A       -   sparse matrix, must be NxN exactly
    IsUpper -   which half of A is provided (another half is ignored)
    B       -   array[N], right part

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate or non-SPD system).

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void sparsespdsolve(const sparsematrix* a,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    sparsematrix a2;
    ae_int_t n;
    double v;
    ae_vector p;

    ae_frame_make(_state, &_frame_block);
    memset(&a2, 0, sizeof(a2));
    memset(&p, 0, sizeof(p));
    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);
    _sparsematrix_init(&a2, _state, ae_true);
    ae_vector_init(&p, 0, DT_INT, _state, ae_true);

    n = sparsegetnrows(a, _state);
    ae_assert(n>0, "SparseSPDSolve: N<=0", _state);
    ae_assert(sparsegetnrows(a, _state)==n, "SparseSPDSolve: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseSPDSolve: cols(A)!=N", _state);
    ae_assert(b->cnt>=n, "SparseSPDSolve: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseSPDSolve: B contains infinities or NANs", _state);
    initsparsesolverreport(rep, _state);
    sparsecopytocrs(a, &a2, _state);
    if( !sparsecholeskyp(&a2, isupper, &p, _state) )
    {
        rep->terminationtype = -3;
        rsetallocv(n, 0.0, x, _state);
        ae_frame_leave(_state);
        return;
    }
    rcopyallocv(n, b, x, _state);
    for(i=0; i<=n-1; i++)
    {
        j = p.ptr.p_int[i];
        v = x->ptr.p_double[i];
        x->ptr.p_double[i] = x->ptr.p_double[j];
        x->ptr.p_double[j] = v;
    }
    if( isupper )
    {
        sparsetrsv(&a2, isupper, ae_false, 1, x, _state);
        sparsetrsv(&a2, isupper, ae_false, 0, x, _state);
    }
    else
    {
        sparsetrsv(&a2, isupper, ae_false, 0, x, _state);
        sparsetrsv(&a2, isupper, ae_false, 1, x, _state);
    }
    for(i=n-1; i>=0; i--)
    {
        j = p.ptr.p_int[i];
        v = x->ptr.p_double[i];
        x->ptr.p_double[i] = x->ptr.p_double[j];
        x->ptr.p_double[j] = v;
    }
    rep->terminationtype = 1;
    ae_frame_leave(_state);
}


/*************************************************************************
Sparse linear solver for A*x=b with N*N real  symmetric  positive definite
matrix A given by its Cholesky decomposition, and N*1 vectors x and b.

IMPORTANT: this solver requires input matrix to be in  the  SKS  (Skyline)
           or CRS (compressed row storage) format. An  exception  will  be
           generated if you pass matrix in some other format.

INPUT PARAMETERS
    A       -   sparse NxN matrix stored in CRs or SKS format, must be NxN
                exactly
    IsUpper -   which half of A is provided (another half is ignored)
    B       -   array[N], right part

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate or non-SPD system).

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void sparsespdcholeskysolve(const sparsematrix* a,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;

    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);

    n = sparsegetnrows(a, _state);
    ae_assert(n>0, "SparseSPDCholeskySolve: N<=0", _state);
    ae_assert(sparsegetnrows(a, _state)==n, "SparseSPDCholeskySolve: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseSPDCholeskySolve: cols(A)!=N", _state);
    ae_assert(sparseissks(a, _state)||sparseiscrs(a, _state), "SparseSPDCholeskySolve: A is not an SKS/CRS matrix", _state);
    ae_assert(b->cnt>=n, "SparseSPDCholeskySolve: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseSPDCholeskySolve: B contains infinities or NANs", _state);
    initsparsesolverreport(rep, _state);
    ae_vector_set_length(x, n, _state);
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_eq(sparseget(a, i, i, _state),0.0) )
        {
            rep->terminationtype = -3;
            for(i=0; i<=n-1; i++)
            {
                x->ptr.p_double[i] = (double)(0);
            }
            return;
        }
    }
    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = b->ptr.p_double[i];
    }
    if( isupper )
    {
        sparsetrsv(a, isupper, ae_false, 1, x, _state);
        sparsetrsv(a, isupper, ae_false, 0, x, _state);
    }
    else
    {
        sparsetrsv(a, isupper, ae_false, 0, x, _state);
        sparsetrsv(a, isupper, ae_false, 1, x, _state);
    }
    rep->terminationtype = 1;
}


/*************************************************************************
Sparse linear solver for A*x=b with general (nonsymmetric) N*N sparse real
matrix A, N*1 vectors x and b.

This solver converts input matrix to CRS format, performs LU factorization
and uses sparse triangular solvers to get solution of the original system.

INPUT PARAMETERS
    A       -   sparse matrix, must be NxN exactly, any storage format
    N       -   size of A, N>0
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate system).

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void sparsesolve(const sparsematrix* a,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t n;
    double v;
    sparsematrix a2;
    ae_vector pivp;
    ae_vector pivq;

    ae_frame_make(_state, &_frame_block);
    memset(&a2, 0, sizeof(a2));
    memset(&pivp, 0, sizeof(pivp));
    memset(&pivq, 0, sizeof(pivq));
    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);
    _sparsematrix_init(&a2, _state, ae_true);
    ae_vector_init(&pivp, 0, DT_INT, _state, ae_true);
    ae_vector_init(&pivq, 0, DT_INT, _state, ae_true);

    n = sparsegetnrows(a, _state);
    ae_assert(n>0, "SparseSolve: N<=0", _state);
    ae_assert(sparsegetnrows(a, _state)==n, "SparseSolve: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseSolve: cols(A)!=N", _state);
    ae_assert(b->cnt>=n, "SparseSolve: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseSolve: B contains infinities or NANs", _state);
    initsparsesolverreport(rep, _state);
    ae_vector_set_length(x, n, _state);
    sparsecopytocrs(a, &a2, _state);
    if( !sparselu(&a2, 0, &pivp, &pivq, _state) )
    {
        rep->terminationtype = -3;
        for(i=0; i<=n-1; i++)
        {
            x->ptr.p_double[i] = (double)(0);
        }
        ae_frame_leave(_state);
        return;
    }
    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = b->ptr.p_double[i];
    }
    for(i=0; i<=n-1; i++)
    {
        j = pivp.ptr.p_int[i];
        v = x->ptr.p_double[i];
        x->ptr.p_double[i] = x->ptr.p_double[j];
        x->ptr.p_double[j] = v;
    }
    sparsetrsv(&a2, ae_false, ae_true, 0, x, _state);
    sparsetrsv(&a2, ae_true, ae_false, 0, x, _state);
    for(i=n-1; i>=0; i--)
    {
        j = pivq.ptr.p_int[i];
        v = x->ptr.p_double[i];
        x->ptr.p_double[i] = x->ptr.p_double[j];
        x->ptr.p_double[j] = v;
    }
    rep->terminationtype = 1;
    ae_frame_leave(_state);
}


/*************************************************************************
Sparse linear solver for A*x=b with general (nonsymmetric) N*N sparse real
matrix A given by its LU factorization, N*1 vectors x and b.

IMPORTANT: this solver requires input matrix  to  be  in  the  CRS  sparse
           storage format. An exception will  be  generated  if  you  pass
           matrix in some other format (HASH or SKS).

INPUT PARAMETERS
    A       -   LU factorization of the sparse matrix, must be NxN exactly
                in CRS storage format
    P, Q    -   pivot indexes from LU factorization
    N       -   size of A, N>0
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate system).

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void sparselusolve(const sparsematrix* a,
     /* Integer */ const ae_vector* p,
     /* Integer */ const ae_vector* q,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;
    ae_int_t n;

    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);

    n = sparsegetnrows(a, _state);
    ae_assert(n>0, "SparseLUSolve: N<=0", _state);
    ae_assert(sparsegetnrows(a, _state)==n, "SparseLUSolve: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseLUSolve: cols(A)!=N", _state);
    ae_assert(sparseiscrs(a, _state), "SparseLUSolve: A is not an SKS matrix", _state);
    ae_assert(b->cnt>=n, "SparseLUSolve: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseLUSolve: B contains infinities or NANs", _state);
    ae_assert(p->cnt>=n, "SparseLUSolve: length(P)<N", _state);
    ae_assert(q->cnt>=n, "SparseLUSolve: length(Q)<N", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=i&&p->ptr.p_int[i]<n, "SparseLUSolve: P is corrupted", _state);
        ae_assert(q->ptr.p_int[i]>=i&&q->ptr.p_int[i]<n, "SparseLUSolve: Q is corrupted", _state);
    }
    initsparsesolverreport(rep, _state);
    ae_vector_set_length(x, n, _state);
    for(i=0; i<=n-1; i++)
    {
        if( a->didx.ptr.p_int[i]==a->uidx.ptr.p_int[i]||a->vals.ptr.p_double[a->didx.ptr.p_int[i]]==0.0 )
        {
            rep->terminationtype = -3;
            for(i=0; i<=n-1; i++)
            {
                x->ptr.p_double[i] = (double)(0);
            }
            return;
        }
    }
    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = b->ptr.p_double[i];
    }
    for(i=0; i<=n-1; i++)
    {
        j = p->ptr.p_int[i];
        v = x->ptr.p_double[i];
        x->ptr.p_double[i] = x->ptr.p_double[j];
        x->ptr.p_double[j] = v;
    }
    sparsetrsv(a, ae_false, ae_true, 0, x, _state);
    sparsetrsv(a, ae_true, ae_false, 0, x, _state);
    for(i=n-1; i>=0; i--)
    {
        j = q->ptr.p_int[i];
        v = x->ptr.p_double[i];
        x->ptr.p_double[i] = x->ptr.p_double[j];
        x->ptr.p_double[j] = v;
    }
    rep->terminationtype = 1;
}


/*************************************************************************
Reset report fields

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void initsparsesolverreport(sparsesolverreport* rep, ae_state *_state)
{


    rep->terminationtype = 0;
    rep->nmv = 0;
    rep->iterationscount = 0;
    rep->r2 = (double)(0);
}


void _sparsesolverreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    sparsesolverreport *p = (sparsesolverreport*)_p;
    ae_touch_ptr((void*)p);
}


void _sparsesolverreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    sparsesolverreport       *dst = (sparsesolverreport*)_dst;
    const sparsesolverreport *src = (const sparsesolverreport*)_src;
    dst->terminationtype = src->terminationtype;
    dst->nmv = src->nmv;
    dst->iterationscount = src->iterationscount;
    dst->r2 = src->r2;
}


void _sparsesolverreport_clear(void* _p)
{
    sparsesolverreport *p = (sparsesolverreport*)_p;
    ae_touch_ptr((void*)p);
}


void _sparsesolverreport_destroy(void* _p)
{
    sparsesolverreport *p = (sparsesolverreport*)_p;
    ae_touch_ptr((void*)p);
}


/*$ End $*/
