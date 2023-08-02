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
#include "lda.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Multiclass Fisher LDA

The function finds coefficients of a linear  combination  which  optimally
separates training set. Most suited for 2-class problems, see fisherldan()
for an variant that returns N-dimensional basis.

INPUT PARAMETERS:
    XY          -   training set, array[NPoints,NVars+1].
                    First NVars columns store values of independent
                    variables, the next column stores class index (from 0
                    to NClasses-1) which dataset element belongs to.
                    Fractional values are rounded to the nearest integer.
                    The class index must be in the [0,NClasses-1] range,
                    an exception is generated otherwise.
    NPoints     -   training set size, NPoints>=0
    NVars       -   number of independent variables, NVars>=1
    NClasses    -   number of classes, NClasses>=2


OUTPUT PARAMETERS:
    W           -   linear combination coefficients, array[NVars]

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
     Copyright 31.05.2008 by Bochkanov Sergey
*************************************************************************/
void fisherlda(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nvars,
     ae_int_t nclasses,
     /* Real    */ ae_vector* w,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix w2;

    ae_frame_make(_state, &_frame_block);
    memset(&w2, 0, sizeof(w2));
    ae_vector_clear(w);
    ae_matrix_init(&w2, 0, 0, DT_REAL, _state, ae_true);

    fisherldan(xy, npoints, nvars, nclasses, &w2, _state);
    ae_vector_set_length(w, nvars, _state);
    ae_v_move(&w->ptr.p_double[0], 1, &w2.ptr.pp_double[0][0], w2.stride, ae_v_len(0,nvars-1));
    ae_frame_leave(_state);
}


/*************************************************************************
N-dimensional multiclass Fisher LDA

Subroutine finds coefficients of linear combinations which optimally separates
training set on classes. It returns N-dimensional basis whose vector are sorted
by quality of training set separation (in descending order).

INPUT PARAMETERS:
    XY          -   training set, array[NPoints,NVars+1].
                    First NVars columns store values of independent
                    variables, the next column stores class index (from 0
                    to NClasses-1) which dataset element belongs to.
                    Fractional values are rounded to the nearest integer.
                    The class index must be in the [0,NClasses-1] range,
                    an exception is generated otherwise.
    NPoints     -   training set size, NPoints>=0
    NVars       -   number of independent variables, NVars>=1
    NClasses    -   number of classes, NClasses>=2


OUTPUT PARAMETERS:
    W           -   basis, array[NVars,NVars]
                    columns of matrix stores basis vectors, sorted by
                    quality of training set separation (in descending order)

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
     Copyright 31.05.2008 by Bochkanov Sergey
*************************************************************************/
void fisherldan(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nvars,
     ae_int_t nclasses,
     /* Real    */ ae_matrix* w,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t m;
    double v;
    ae_vector c;
    ae_vector mu;
    ae_matrix muc;
    ae_vector nc;
    ae_matrix sw;
    ae_matrix st;
    ae_matrix z;
    ae_matrix z2;
    ae_matrix tm;
    ae_matrix sbroot;
    ae_matrix a;
    ae_matrix xyc;
    ae_matrix xyproj;
    ae_matrix wproj;
    ae_vector tf;
    ae_vector d;
    ae_vector d2;
    ae_vector work;

    ae_frame_make(_state, &_frame_block);
    memset(&c, 0, sizeof(c));
    memset(&mu, 0, sizeof(mu));
    memset(&muc, 0, sizeof(muc));
    memset(&nc, 0, sizeof(nc));
    memset(&sw, 0, sizeof(sw));
    memset(&st, 0, sizeof(st));
    memset(&z, 0, sizeof(z));
    memset(&z2, 0, sizeof(z2));
    memset(&tm, 0, sizeof(tm));
    memset(&sbroot, 0, sizeof(sbroot));
    memset(&a, 0, sizeof(a));
    memset(&xyc, 0, sizeof(xyc));
    memset(&xyproj, 0, sizeof(xyproj));
    memset(&wproj, 0, sizeof(wproj));
    memset(&tf, 0, sizeof(tf));
    memset(&d, 0, sizeof(d));
    memset(&d2, 0, sizeof(d2));
    memset(&work, 0, sizeof(work));
    ae_matrix_clear(w);
    ae_vector_init(&c, 0, DT_INT, _state, ae_true);
    ae_vector_init(&mu, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&muc, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&nc, 0, DT_INT, _state, ae_true);
    ae_matrix_init(&sw, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&st, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&z, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&z2, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&tm, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&sbroot, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&a, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xyc, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xyproj, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&wproj, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tf, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&d, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&d2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&work, 0, DT_REAL, _state, ae_true);

    
    /*
     * Test data
     */
    ae_assert(!(npoints<0), "FisherLDAN: NPoints<0", _state);
    ae_assert(!(nvars<1), "FisherLDAN: NVars<1", _state);
    ae_assert(!(nclasses<2), "FisherLDAN: NClasses<2", _state);
    for(i=0; i<=npoints-1; i++)
    {
        if( ae_round(xy->ptr.pp_double[i][nvars], _state)<0||ae_round(xy->ptr.pp_double[i][nvars], _state)>=nclasses )
        {
            ae_assert(ae_false, "FisherLDAN: class index is <0 or >NClasses-1", _state);
        }
    }
    
    /*
     * Special case: NPoints<=1
     * Degenerate task.
     */
    if( npoints<=1 )
    {
        ae_matrix_set_length(w, nvars, nvars, _state);
        for(i=0; i<=nvars-1; i++)
        {
            for(j=0; j<=nvars-1; j++)
            {
                if( i==j )
                {
                    w->ptr.pp_double[i][j] = (double)(1);
                }
                else
                {
                    w->ptr.pp_double[i][j] = (double)(0);
                }
            }
        }
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Prepare temporaries
     */
    ae_vector_set_length(&tf, nvars, _state);
    ae_vector_set_length(&work, ae_maxint(nvars, npoints, _state)+1, _state);
    ae_matrix_set_length(&xyc, npoints, nvars, _state);
    
    /*
     * Convert class labels from reals to integers (just for convenience)
     */
    ae_vector_set_length(&c, npoints, _state);
    for(i=0; i<=npoints-1; i++)
    {
        c.ptr.p_int[i] = ae_round(xy->ptr.pp_double[i][nvars], _state);
    }
    
    /*
     * Calculate class sizes, class means
     */
    ae_vector_set_length(&mu, nvars, _state);
    ae_matrix_set_length(&muc, nclasses, nvars, _state);
    ae_vector_set_length(&nc, nclasses, _state);
    for(j=0; j<=nvars-1; j++)
    {
        mu.ptr.p_double[j] = (double)(0);
    }
    for(i=0; i<=nclasses-1; i++)
    {
        nc.ptr.p_int[i] = 0;
        for(j=0; j<=nvars-1; j++)
        {
            muc.ptr.pp_double[i][j] = (double)(0);
        }
    }
    for(i=0; i<=npoints-1; i++)
    {
        ae_v_add(&mu.ptr.p_double[0], 1, &xy->ptr.pp_double[i][0], 1, ae_v_len(0,nvars-1));
        ae_v_add(&muc.ptr.pp_double[c.ptr.p_int[i]][0], 1, &xy->ptr.pp_double[i][0], 1, ae_v_len(0,nvars-1));
        nc.ptr.p_int[c.ptr.p_int[i]] = nc.ptr.p_int[c.ptr.p_int[i]]+1;
    }
    for(i=0; i<=nclasses-1; i++)
    {
        v = (double)1/(double)nc.ptr.p_int[i];
        ae_v_muld(&muc.ptr.pp_double[i][0], 1, ae_v_len(0,nvars-1), v);
    }
    v = (double)1/(double)npoints;
    ae_v_muld(&mu.ptr.p_double[0], 1, ae_v_len(0,nvars-1), v);
    
    /*
     * Create ST matrix
     */
    ae_matrix_set_length(&st, nvars, nvars, _state);
    for(i=0; i<=nvars-1; i++)
    {
        for(j=0; j<=nvars-1; j++)
        {
            st.ptr.pp_double[i][j] = (double)(0);
        }
    }
    for(k=0; k<=npoints-1; k++)
    {
        ae_v_move(&xyc.ptr.pp_double[k][0], 1, &xy->ptr.pp_double[k][0], 1, ae_v_len(0,nvars-1));
        ae_v_sub(&xyc.ptr.pp_double[k][0], 1, &mu.ptr.p_double[0], 1, ae_v_len(0,nvars-1));
    }
    rmatrixgemm(nvars, nvars, npoints, 1.0, &xyc, 0, 0, 1, &xyc, 0, 0, 0, 0.0, &st, 0, 0, _state);
    
    /*
     * Create SW matrix
     */
    ae_matrix_set_length(&sw, nvars, nvars, _state);
    for(i=0; i<=nvars-1; i++)
    {
        for(j=0; j<=nvars-1; j++)
        {
            sw.ptr.pp_double[i][j] = (double)(0);
        }
    }
    for(k=0; k<=npoints-1; k++)
    {
        ae_v_move(&xyc.ptr.pp_double[k][0], 1, &xy->ptr.pp_double[k][0], 1, ae_v_len(0,nvars-1));
        ae_v_sub(&xyc.ptr.pp_double[k][0], 1, &muc.ptr.pp_double[c.ptr.p_int[k]][0], 1, ae_v_len(0,nvars-1));
    }
    rmatrixgemm(nvars, nvars, npoints, 1.0, &xyc, 0, 0, 1, &xyc, 0, 0, 0, 0.0, &sw, 0, 0, _state);
    
    /*
     * Maximize ratio J=(w'*ST*w)/(w'*SW*w).
     *
     * First, make transition from w to v such that w'*ST*w becomes v'*v:
     *    v  = root(ST)*w = R*w
     *    R  = root(D)*Z'
     *    w  = (root(ST)^-1)*v = RI*v
     *    RI = Z*inv(root(D))
     *    J  = (v'*v)/(v'*(RI'*SW*RI)*v)
     *    ST = Z*D*Z'
     *
     *    so we have
     *
     *    J = (v'*v) / (v'*(inv(root(D))*Z'*SW*Z*inv(root(D)))*v)  =
     *      = (v'*v) / (v'*A*v)
     */
    if( !smatrixevd(&st, nvars, 1, ae_true, &d, &z, _state) )
    {
        ae_assert(ae_false, "FisherLDAN: EVD solver failure", _state);
    }
    ae_matrix_set_length(w, nvars, nvars, _state);
    if( ae_fp_less_eq(d.ptr.p_double[nvars-1],(double)(0))||ae_fp_less_eq(d.ptr.p_double[0],(double)1000*ae_machineepsilon*d.ptr.p_double[nvars-1]) )
    {
        
        /*
         * Special case: D[NVars-1]<=0
         * Degenerate task (all variables takes the same value).
         */
        if( ae_fp_less_eq(d.ptr.p_double[nvars-1],(double)(0)) )
        {
            for(i=0; i<=nvars-1; i++)
            {
                for(j=0; j<=nvars-1; j++)
                {
                    if( i==j )
                    {
                        w->ptr.pp_double[i][j] = (double)(1);
                    }
                    else
                    {
                        w->ptr.pp_double[i][j] = (double)(0);
                    }
                }
            }
            ae_frame_leave(_state);
            return;
        }
        
        /*
         * Special case: degenerate ST matrix, multicollinearity found.
         * Since we know ST eigenvalues/vectors we can translate task to
         * non-degenerate form.
         *
         * Let WG is orthogonal basis of the non zero variance subspace
         * of the ST and let WZ is orthogonal basis of the zero variance
         * subspace.
         *
         * Projection on WG allows us to use LDA on reduced M-dimensional
         * subspace, N-M vectors of WZ allows us to update reduced LDA
         * factors to full N-dimensional subspace.
         */
        m = 0;
        for(k=0; k<=nvars-1; k++)
        {
            if( ae_fp_less_eq(d.ptr.p_double[k],(double)1000*ae_machineepsilon*d.ptr.p_double[nvars-1]) )
            {
                m = k+1;
            }
        }
        ae_assert(m!=0, "FisherLDAN: internal error #1", _state);
        ae_matrix_set_length(&xyproj, npoints, nvars-m+1, _state);
        rmatrixgemm(npoints, nvars-m, nvars, 1.0, xy, 0, 0, 0, &z, 0, m, 0, 0.0, &xyproj, 0, 0, _state);
        for(i=0; i<=npoints-1; i++)
        {
            xyproj.ptr.pp_double[i][nvars-m] = xy->ptr.pp_double[i][nvars];
        }
        fisherldan(&xyproj, npoints, nvars-m, nclasses, &wproj, _state);
        rmatrixgemm(nvars, nvars-m, nvars-m, 1.0, &z, 0, m, 0, &wproj, 0, 0, 0, 0.0, w, 0, 0, _state);
        for(k=nvars-m; k<=nvars-1; k++)
        {
            ae_v_move(&w->ptr.pp_double[0][k], w->stride, &z.ptr.pp_double[0][k-(nvars-m)], z.stride, ae_v_len(0,nvars-1));
        }
    }
    else
    {
        
        /*
         * General case: no multicollinearity
         */
        ae_matrix_set_length(&tm, nvars, nvars, _state);
        ae_matrix_set_length(&a, nvars, nvars, _state);
        rmatrixgemm(nvars, nvars, nvars, 1.0, &sw, 0, 0, 0, &z, 0, 0, 0, 0.0, &tm, 0, 0, _state);
        rmatrixgemm(nvars, nvars, nvars, 1.0, &z, 0, 0, 1, &tm, 0, 0, 0, 0.0, &a, 0, 0, _state);
        for(i=0; i<=nvars-1; i++)
        {
            for(j=0; j<=nvars-1; j++)
            {
                a.ptr.pp_double[i][j] = a.ptr.pp_double[i][j]/ae_sqrt(d.ptr.p_double[i]*d.ptr.p_double[j], _state);
            }
        }
        if( !smatrixevd(&a, nvars, 1, ae_true, &d2, &z2, _state) )
        {
            ae_assert(ae_false, "FisherLDAN: EVD solver failure", _state);
        }
        for(i=0; i<=nvars-1; i++)
        {
            for(k=0; k<=nvars-1; k++)
            {
                z2.ptr.pp_double[i][k] = z2.ptr.pp_double[i][k]/ae_sqrt(d.ptr.p_double[i], _state);
            }
        }
        rmatrixgemm(nvars, nvars, nvars, 1.0, &z, 0, 0, 0, &z2, 0, 0, 0, 0.0, w, 0, 0, _state);
    }
    
    /*
     * Post-processing:
     * * normalization
     * * converting to non-negative form, if possible
     */
    for(k=0; k<=nvars-1; k++)
    {
        v = ae_v_dotproduct(&w->ptr.pp_double[0][k], w->stride, &w->ptr.pp_double[0][k], w->stride, ae_v_len(0,nvars-1));
        v = (double)1/ae_sqrt(v, _state);
        ae_v_muld(&w->ptr.pp_double[0][k], w->stride, ae_v_len(0,nvars-1), v);
        v = (double)(0);
        for(i=0; i<=nvars-1; i++)
        {
            v = v+w->ptr.pp_double[i][k];
        }
        if( ae_fp_less(v,(double)(0)) )
        {
            ae_v_muld(&w->ptr.pp_double[0][k], w->stride, ae_v_len(0,nvars-1), -1.0);
        }
    }
    ae_frame_leave(_state);
}


/*$ End $*/
