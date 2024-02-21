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
#include "pca.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Principal components analysis

This function builds orthogonal basis  where  first  axis  corresponds  to
direction with maximum variance, second axis  maximizes  variance  in  the
subspace orthogonal to first axis and so on.

This function builds FULL basis, i.e. returns N vectors  corresponding  to
ALL directions, no matter how informative. If you need  just a  few  (say,
10 or 50) of the most important directions, you may find it faster to  use
one of the reduced versions:
* pcatruncatedsubspace() - for subspace iteration based method

It should be noted that, unlike LDA, PCA does not use class labels.

INPUT PARAMETERS:
    X           -   dataset, array[NPoints,NVars].
                    matrix contains ONLY INDEPENDENT VARIABLES.
    NPoints     -   dataset size, NPoints>=0
    NVars       -   number of independent variables, NVars>=1

OUTPUT PARAMETERS:
    S2          -   array[NVars]. variance values corresponding
                    to basis vectors.
    V           -   array[NVars,NVars]
                    matrix, whose columns store basis vectors.

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 25.08.2008 by Bochkanov Sergey
*************************************************************************/
void pcabuildbasis(/* Real    */ const ae_matrix* x,
     ae_int_t npoints,
     ae_int_t nvars,
     /* Real    */ ae_vector* s2,
     /* Real    */ ae_matrix* v,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix a;
    ae_matrix u;
    ae_matrix vt;
    ae_vector m;
    ae_vector t;
    ae_int_t i;
    ae_int_t j;
    double mean;
    double variance;
    double skewness;
    double kurtosis;

    ae_frame_make(_state, &_frame_block);
    memset(&a, 0, sizeof(a));
    memset(&u, 0, sizeof(u));
    memset(&vt, 0, sizeof(vt));
    memset(&m, 0, sizeof(m));
    memset(&t, 0, sizeof(t));
    ae_vector_clear(s2);
    ae_matrix_clear(v);
    ae_matrix_init(&a, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&u, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&vt, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&m, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&t, 0, DT_REAL, _state, ae_true);

    
    /*
     * Check input data
     */
    ae_assert(npoints>=0, "PCABuildBasis: NPoints<0", _state);
    ae_assert(nvars>=1, "PCABuildBasis: NVars<1", _state);
    ae_assert(x->rows>=npoints, "PCABuildBasis: rows(X)<NPoints", _state);
    ae_assert(x->cols>=nvars||npoints==0, "PCABuildBasis: cols(X)<NVars", _state);
    ae_assert(apservisfinitematrix(x, npoints, nvars, _state), "PCABuildBasis: X contains INF/NAN", _state);
    
    /*
     * Special case: NPoints=0
     */
    if( npoints==0 )
    {
        ae_vector_set_length(s2, nvars, _state);
        ae_matrix_set_length(v, nvars, nvars, _state);
        for(i=0; i<=nvars-1; i++)
        {
            s2->ptr.p_double[i] = (double)(0);
        }
        for(i=0; i<=nvars-1; i++)
        {
            for(j=0; j<=nvars-1; j++)
            {
                if( i==j )
                {
                    v->ptr.pp_double[i][j] = (double)(1);
                }
                else
                {
                    v->ptr.pp_double[i][j] = (double)(0);
                }
            }
        }
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Calculate means
     */
    ae_vector_set_length(&m, nvars, _state);
    ae_vector_set_length(&t, npoints, _state);
    for(j=0; j<=nvars-1; j++)
    {
        ae_v_move(&t.ptr.p_double[0], 1, &x->ptr.pp_double[0][j], x->stride, ae_v_len(0,npoints-1));
        samplemoments(&t, npoints, &mean, &variance, &skewness, &kurtosis, _state);
        m.ptr.p_double[j] = mean;
    }
    
    /*
     * Center, apply SVD, prepare output
     */
    ae_matrix_set_length(&a, ae_maxint(npoints, nvars, _state), nvars, _state);
    for(i=0; i<=npoints-1; i++)
    {
        ae_v_move(&a.ptr.pp_double[i][0], 1, &x->ptr.pp_double[i][0], 1, ae_v_len(0,nvars-1));
        ae_v_sub(&a.ptr.pp_double[i][0], 1, &m.ptr.p_double[0], 1, ae_v_len(0,nvars-1));
    }
    for(i=npoints; i<=nvars-1; i++)
    {
        for(j=0; j<=nvars-1; j++)
        {
            a.ptr.pp_double[i][j] = (double)(0);
        }
    }
    if( !rmatrixsvd(&a, ae_maxint(npoints, nvars, _state), nvars, 0, 1, 2, s2, &u, &vt, _state) )
    {
        ae_assert(ae_false, "PCABuildBasis: internal SVD solver failure", _state);
        ae_frame_leave(_state);
        return;
    }
    if( npoints!=1 )
    {
        for(i=0; i<=nvars-1; i++)
        {
            s2->ptr.p_double[i] = ae_sqr(s2->ptr.p_double[i], _state)/(double)(npoints-1);
        }
    }
    ae_matrix_set_length(v, nvars, nvars, _state);
    copyandtranspose(&vt, 0, nvars-1, 0, nvars-1, v, 0, nvars-1, 0, nvars-1, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Principal components analysis

This function performs truncated PCA, i.e. returns just a few most important
directions.

Internally it uses iterative eigensolver which is very efficient when only
a minor fraction of full basis is required. Thus, if you need full  basis,
it is better to use pcabuildbasis() function.

It should be noted that, unlike LDA, PCA does not use class labels.

INPUT PARAMETERS:
    X           -   dataset, array[0..NPoints-1,0..NVars-1].
                    matrix contains ONLY INDEPENDENT VARIABLES.
    NPoints     -   dataset size, NPoints>=0
    NVars       -   number of independent variables, NVars>=1
    NNeeded     -   number of requested components, in [1,NVars] range;
                    this function is efficient only for NNeeded<<NVars.
    Eps         -   desired  precision  of  vectors  returned;  underlying
                    solver will stop iterations as soon as absolute  error
                    in corresponding singular values  reduces  to  roughly
                    eps*MAX(lambda[]), with lambda[] being array of  eigen
                    values.
                    Zero value means that  algorithm  performs  number  of
                    iterations  specified  by  maxits  parameter,  without
                    paying attention to precision.
    MaxIts      -   number of iterations performed by  subspace  iteration
                    method. Zero value means that no  limit  on  iteration
                    count is placed (eps-based stopping condition is used).
                    

OUTPUT PARAMETERS:
    S2          -   array[NNeeded]. Variance values corresponding
                    to basis vectors.
    V           -   array[NVars,NNeeded]
                    matrix, whose columns store basis vectors.
                    
NOTE: passing eps=0 and maxits=0 results in small eps  being  selected  as
stopping condition. Exact value of automatically selected eps is  version-
-dependent.

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 10.01.2017 by Bochkanov Sergey
*************************************************************************/
void pcatruncatedsubspace(/* Real    */ const ae_matrix* x,
     ae_int_t npoints,
     ae_int_t nvars,
     ae_int_t nneeded,
     double eps,
     ae_int_t maxits,
     /* Real    */ ae_vector* s2,
     /* Real    */ ae_matrix* v,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix a;
    ae_matrix b;
    ae_vector means;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double vv;
    eigsubspacestate solver;
    eigsubspacereport rep;

    ae_frame_make(_state, &_frame_block);
    memset(&a, 0, sizeof(a));
    memset(&b, 0, sizeof(b));
    memset(&means, 0, sizeof(means));
    memset(&solver, 0, sizeof(solver));
    memset(&rep, 0, sizeof(rep));
    ae_vector_clear(s2);
    ae_matrix_clear(v);
    ae_matrix_init(&a, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&b, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&means, 0, DT_REAL, _state, ae_true);
    _eigsubspacestate_init(&solver, _state, ae_true);
    _eigsubspacereport_init(&rep, _state, ae_true);

    ae_assert(npoints>=0, "PCATruncatedSubspace: npoints<0", _state);
    ae_assert(nvars>=1, "PCATruncatedSubspace: nvars<1", _state);
    ae_assert(nneeded>0, "PCATruncatedSubspace: nneeded<1", _state);
    ae_assert(nneeded<=nvars, "PCATruncatedSubspace: nneeded>nvars", _state);
    ae_assert(maxits>=0, "PCATruncatedSubspace: maxits<0", _state);
    ae_assert(ae_isfinite(eps, _state)&&ae_fp_greater_eq(eps,(double)(0)), "PCATruncatedSubspace: eps<0 or is not finite", _state);
    ae_assert(x->rows>=npoints, "PCATruncatedSubspace: rows(x)<npoints", _state);
    ae_assert(x->cols>=nvars||npoints==0, "PCATruncatedSubspace: cols(x)<nvars", _state);
    ae_assert(apservisfinitematrix(x, npoints, nvars, _state), "PCATruncatedSubspace: X contains INF/NAN", _state);
    
    /*
     * Special case: NPoints=0
     */
    if( npoints==0 )
    {
        ae_vector_set_length(s2, nneeded, _state);
        ae_matrix_set_length(v, nvars, nneeded, _state);
        for(i=0; i<=nvars-1; i++)
        {
            s2->ptr.p_double[i] = (double)(0);
        }
        for(i=0; i<=nvars-1; i++)
        {
            for(j=0; j<=nneeded-1; j++)
            {
                if( i==j )
                {
                    v->ptr.pp_double[i][j] = (double)(1);
                }
                else
                {
                    v->ptr.pp_double[i][j] = (double)(0);
                }
            }
        }
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Center matrix
     */
    ae_vector_set_length(&means, nvars, _state);
    for(i=0; i<=nvars-1; i++)
    {
        means.ptr.p_double[i] = (double)(0);
    }
    vv = (double)1/(double)npoints;
    for(i=0; i<=npoints-1; i++)
    {
        ae_v_addd(&means.ptr.p_double[0], 1, &x->ptr.pp_double[i][0], 1, ae_v_len(0,nvars-1), vv);
    }
    ae_matrix_set_length(&a, npoints, nvars, _state);
    for(i=0; i<=npoints-1; i++)
    {
        ae_v_move(&a.ptr.pp_double[i][0], 1, &x->ptr.pp_double[i][0], 1, ae_v_len(0,nvars-1));
        ae_v_sub(&a.ptr.pp_double[i][0], 1, &means.ptr.p_double[0], 1, ae_v_len(0,nvars-1));
    }
    
    /*
     * Find eigenvalues with subspace iteration solver
     */
    eigsubspacecreate(nvars, nneeded, &solver, _state);
    eigsubspacesetcond(&solver, eps, maxits, _state);
    eigsubspaceoocstart(&solver, 0, _state);
    while(eigsubspaceooccontinue(&solver, _state))
    {
        ae_assert(solver.requesttype==0, "PCATruncatedSubspace: integrity check failed", _state);
        k = solver.requestsize;
        rmatrixsetlengthatleast(&b, npoints, k, _state);
        rmatrixgemm(npoints, k, nvars, 1.0, &a, 0, 0, 0, &solver.x, 0, 0, 0, 0.0, &b, 0, 0, _state);
        rmatrixgemm(nvars, k, npoints, 1.0, &a, 0, 0, 1, &b, 0, 0, 0, 0.0, &solver.ax, 0, 0, _state);
    }
    eigsubspaceoocstop(&solver, s2, v, &rep, _state);
    if( npoints!=1 )
    {
        for(i=0; i<=nneeded-1; i++)
        {
            s2->ptr.p_double[i] = s2->ptr.p_double[i]/(double)(npoints-1);
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Sparse truncated principal components analysis

This function performs sparse truncated PCA, i.e. returns just a few  most
important principal components for a sparse input X.

Internally it uses iterative eigensolver which is very efficient when only
a minor fraction of full basis is required.

It should be noted that, unlike LDA, PCA does not use class labels.

INPUT PARAMETERS:
    X           -   sparse dataset, sparse  npoints*nvars  matrix.  It  is
                    recommended to use CRS sparse storage format;  non-CRS
                    input will be internally converted to CRS.
                    Matrix contains ONLY INDEPENDENT VARIABLES,  and  must
                    be EXACTLY npoints*nvars.
    NPoints     -   dataset size, NPoints>=0
    NVars       -   number of independent variables, NVars>=1
    NNeeded     -   number of requested components, in [1,NVars] range;
                    this function is efficient only for NNeeded<<NVars.
    Eps         -   desired  precision  of  vectors  returned;  underlying
                    solver will stop iterations as soon as absolute  error
                    in corresponding singular values  reduces  to  roughly
                    eps*MAX(lambda[]), with lambda[] being array of  eigen
                    values.
                    Zero value means that  algorithm  performs  number  of
                    iterations  specified  by  maxits  parameter,  without
                    paying attention to precision.
    MaxIts      -   number of iterations performed by  subspace  iteration
                    method. Zero value means that no  limit  on  iteration
                    count is placed (eps-based stopping condition is used).
                    

OUTPUT PARAMETERS:
    S2          -   array[NNeeded]. Variance values corresponding
                    to basis vectors.
    V           -   array[NVars,NNeeded]
                    matrix, whose columns store basis vectors.
                    
NOTE: passing eps=0 and maxits=0 results in small eps  being  selected  as
      a stopping condition. Exact value of automatically selected  eps  is
      version-dependent.

NOTE: zero  MaxIts  is  silently  replaced  by some reasonable value which
      prevents eternal loops (possible when inputs are degenerate and  too
      stringent stopping criteria are specified). In  current  version  it
      is 50+2*NVars.

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 10.01.2017 by Bochkanov Sergey
*************************************************************************/
void pcatruncatedsubspacesparse(const sparsematrix* x,
     ae_int_t npoints,
     ae_int_t nvars,
     ae_int_t nneeded,
     double eps,
     ae_int_t maxits,
     /* Real    */ ae_vector* s2,
     /* Real    */ ae_matrix* v,
     ae_state *_state)
{
    ae_frame _frame_block;
    sparsematrix xcrs;
    ae_vector b1;
    ae_vector c1;
    ae_vector z1;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double vv;
    ae_vector means;
    eigsubspacestate solver;
    eigsubspacereport rep;

    ae_frame_make(_state, &_frame_block);
    memset(&xcrs, 0, sizeof(xcrs));
    memset(&b1, 0, sizeof(b1));
    memset(&c1, 0, sizeof(c1));
    memset(&z1, 0, sizeof(z1));
    memset(&means, 0, sizeof(means));
    memset(&solver, 0, sizeof(solver));
    memset(&rep, 0, sizeof(rep));
    ae_vector_clear(s2);
    ae_matrix_clear(v);
    _sparsematrix_init(&xcrs, _state, ae_true);
    ae_vector_init(&b1, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&c1, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&z1, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&means, 0, DT_REAL, _state, ae_true);
    _eigsubspacestate_init(&solver, _state, ae_true);
    _eigsubspacereport_init(&rep, _state, ae_true);

    ae_assert(npoints>=0, "PCATruncatedSubspaceSparse: npoints<0", _state);
    ae_assert(nvars>=1, "PCATruncatedSubspaceSparse: nvars<1", _state);
    ae_assert(nneeded>0, "PCATruncatedSubspaceSparse: nneeded<1", _state);
    ae_assert(nneeded<=nvars, "PCATruncatedSubspaceSparse: nneeded>nvars", _state);
    ae_assert(maxits>=0, "PCATruncatedSubspaceSparse: maxits<0", _state);
    ae_assert(ae_isfinite(eps, _state)&&ae_fp_greater_eq(eps,(double)(0)), "PCATruncatedSubspaceSparse: eps<0 or is not finite", _state);
    if( npoints>0 )
    {
        ae_assert(sparsegetnrows(x, _state)==npoints, "PCATruncatedSubspaceSparse: rows(x)!=npoints", _state);
        ae_assert(sparsegetncols(x, _state)==nvars, "PCATruncatedSubspaceSparse: cols(x)!=nvars", _state);
    }
    
    /*
     * Special case: NPoints=0
     */
    if( npoints==0 )
    {
        ae_vector_set_length(s2, nneeded, _state);
        ae_matrix_set_length(v, nvars, nneeded, _state);
        for(i=0; i<=nvars-1; i++)
        {
            s2->ptr.p_double[i] = (double)(0);
        }
        for(i=0; i<=nvars-1; i++)
        {
            for(j=0; j<=nneeded-1; j++)
            {
                if( i==j )
                {
                    v->ptr.pp_double[i][j] = (double)(1);
                }
                else
                {
                    v->ptr.pp_double[i][j] = (double)(0);
                }
            }
        }
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * If input data are not in CRS format, perform conversion to CRS
     */
    if( !sparseiscrs(x, _state) )
    {
        sparsecopytocrs(x, &xcrs, _state);
        pcatruncatedsubspacesparse(&xcrs, npoints, nvars, nneeded, eps, maxits, s2, v, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Initialize parameters, prepare buffers
     */
    ae_vector_set_length(&b1, npoints, _state);
    ae_vector_set_length(&z1, nvars, _state);
    if( ae_fp_eq(eps,(double)(0))&&maxits==0 )
    {
        eps = 1.0E-6;
    }
    if( maxits==0 )
    {
        maxits = 50+2*nvars;
    }
    
    /*
     * Calculate mean values
     */
    vv = (double)1/(double)npoints;
    for(i=0; i<=npoints-1; i++)
    {
        b1.ptr.p_double[i] = vv;
    }
    sparsemtv(x, &b1, &means, _state);
    
    /*
     * Find eigenvalues with subspace iteration solver
     */
    eigsubspacecreate(nvars, nneeded, &solver, _state);
    eigsubspacesetcond(&solver, eps, maxits, _state);
    eigsubspaceoocstart(&solver, 0, _state);
    while(eigsubspaceooccontinue(&solver, _state))
    {
        ae_assert(solver.requesttype==0, "PCATruncatedSubspace: integrity check failed", _state);
        for(k=0; k<=solver.requestsize-1; k++)
        {
            
            /*
             * Calculate B1=(X-meansX)*Zk
             */
            ae_v_move(&z1.ptr.p_double[0], 1, &solver.x.ptr.pp_double[0][k], solver.x.stride, ae_v_len(0,nvars-1));
            sparsemv(x, &z1, &b1, _state);
            vv = ae_v_dotproduct(&solver.x.ptr.pp_double[0][k], solver.x.stride, &means.ptr.p_double[0], 1, ae_v_len(0,nvars-1));
            for(i=0; i<=npoints-1; i++)
            {
                b1.ptr.p_double[i] = b1.ptr.p_double[i]-vv;
            }
            
            /*
             * Calculate (X-meansX)^T*B1
             */
            sparsemtv(x, &b1, &c1, _state);
            vv = (double)(0);
            for(i=0; i<=npoints-1; i++)
            {
                vv = vv+b1.ptr.p_double[i];
            }
            for(j=0; j<=nvars-1; j++)
            {
                solver.ax.ptr.pp_double[j][k] = c1.ptr.p_double[j]-vv*means.ptr.p_double[j];
            }
        }
    }
    eigsubspaceoocstop(&solver, s2, v, &rep, _state);
    if( npoints!=1 )
    {
        for(i=0; i<=nneeded-1; i++)
        {
            s2->ptr.p_double[i] = s2->ptr.p_double[i]/(double)(npoints-1);
        }
    }
    ae_frame_leave(_state);
}


/*$ End $*/
