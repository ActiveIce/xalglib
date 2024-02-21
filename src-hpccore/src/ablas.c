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
#include "ablas.h"


/*$ Declarations $*/
static ae_int_t ablas_blas2minvendorkernelsize = 8;
static void ablas_ablasinternalsplitlength(ae_int_t n,
     ae_int_t nb,
     ae_int_t* n1,
     ae_int_t* n2,
     ae_state *_state);
static void ablas_cmatrixrighttrsm2(ae_int_t m,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Complex */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state);
static void ablas_cmatrixlefttrsm2(ae_int_t m,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Complex */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state);
static void ablas_rmatrixrighttrsm2(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Real    */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state);
static void ablas_rmatrixlefttrsm2(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Real    */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state);
static void ablas_cmatrixherk2(ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     double beta,
     /* Complex */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_bool isupper,
     ae_state *_state);
static void ablas_rmatrixsyrk2(ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     double beta,
     /* Real    */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_bool isupper,
     ae_state *_state);
static void ablas_cmatrixgemmrec(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     ae_complex alpha,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     /* Complex */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_int_t optypeb,
     ae_complex beta,
     /* Complex */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state);
void _spawn_ablas_cmatrixgemmrec(ae_int_t m,
    ae_int_t n,
    ae_int_t k,
    ae_complex alpha,
    /* Complex */ const ae_matrix* a,
    ae_int_t ia,
    ae_int_t ja,
    ae_int_t optypea,
    /* Complex */ const ae_matrix* b,
    ae_int_t ib,
    ae_int_t jb,
    ae_int_t optypeb,
    ae_complex beta,
    /* Complex */ ae_matrix* c,
    ae_int_t ic,
    ae_int_t jc, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_ablas_cmatrixgemmrec(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_ablas_cmatrixgemmrec(ae_int_t m,
    ae_int_t n,
    ae_int_t k,
    ae_complex alpha,
    /* Complex */ const ae_matrix* a,
    ae_int_t ia,
    ae_int_t ja,
    ae_int_t optypea,
    /* Complex */ const ae_matrix* b,
    ae_int_t ib,
    ae_int_t jb,
    ae_int_t optypeb,
    ae_complex beta,
    /* Complex */ ae_matrix* c,
    ae_int_t ic,
    ae_int_t jc, ae_state *_state);
static void ablas_rmatrixgemmrec(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_int_t optypeb,
     double beta,
     /* Real    */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state);
void _spawn_ablas_rmatrixgemmrec(ae_int_t m,
    ae_int_t n,
    ae_int_t k,
    double alpha,
    /* Real    */ const ae_matrix* a,
    ae_int_t ia,
    ae_int_t ja,
    ae_int_t optypea,
    /* Real    */ const ae_matrix* b,
    ae_int_t ib,
    ae_int_t jb,
    ae_int_t optypeb,
    double beta,
    /* Real    */ ae_matrix* c,
    ae_int_t ic,
    ae_int_t jc, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_ablas_rmatrixgemmrec(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_ablas_rmatrixgemmrec(ae_int_t m,
    ae_int_t n,
    ae_int_t k,
    double alpha,
    /* Real    */ const ae_matrix* a,
    ae_int_t ia,
    ae_int_t ja,
    ae_int_t optypea,
    /* Real    */ const ae_matrix* b,
    ae_int_t ib,
    ae_int_t jb,
    ae_int_t optypeb,
    double beta,
    /* Real    */ ae_matrix* c,
    ae_int_t ic,
    ae_int_t jc, ae_state *_state);


/*$ Body $*/


/*************************************************************************
Splits matrix length in two parts, left part should match ABLAS block size

INPUT PARAMETERS
    A   -   real matrix, is passed to ensure that we didn't split
            complex matrix using real splitting subroutine.
            matrix itself is not changed.
    N   -   length, N>0

OUTPUT PARAMETERS
    N1  -   length
    N2  -   length

N1+N2=N, N1>=N2, N2 may be zero

  -- ALGLIB routine --
     15.12.2009
     Bochkanov Sergey
*************************************************************************/
void ablassplitlength(/* Real    */ const ae_matrix* a,
     ae_int_t n,
     ae_int_t* n1,
     ae_int_t* n2,
     ae_state *_state)
{

    *n1 = 0;
    *n2 = 0;

    if( n>ablasblocksize(a, _state) )
    {
        ablas_ablasinternalsplitlength(n, ablasblocksize(a, _state), n1, n2, _state);
    }
    else
    {
        ablas_ablasinternalsplitlength(n, ablasmicroblocksize(_state), n1, n2, _state);
    }
}


/*************************************************************************
Complex ABLASSplitLength

  -- ALGLIB routine --
     15.12.2009
     Bochkanov Sergey
*************************************************************************/
void ablascomplexsplitlength(/* Complex */ const ae_matrix* a,
     ae_int_t n,
     ae_int_t* n1,
     ae_int_t* n2,
     ae_state *_state)
{

    *n1 = 0;
    *n2 = 0;

    if( n>ablascomplexblocksize(a, _state) )
    {
        ablas_ablasinternalsplitlength(n, ablascomplexblocksize(a, _state), n1, n2, _state);
    }
    else
    {
        ablas_ablasinternalsplitlength(n, ablasmicroblocksize(_state), n1, n2, _state);
    }
}


/*************************************************************************
Returns switch point for parallelism.

  -- ALGLIB routine --
     15.12.2009
     Bochkanov Sergey
*************************************************************************/
ae_int_t gemmparallelsize(ae_state *_state)
{
    ae_int_t result;


    result = 64;
    return result;
}


/*************************************************************************
Returns block size - subdivision size where  cache-oblivious  soubroutines
switch to the optimized kernel.

INPUT PARAMETERS
    A   -   real matrix, is passed to ensure that we didn't split
            complex matrix using real splitting subroutine.
            matrix itself is not changed.

  -- ALGLIB routine --
     15.12.2009
     Bochkanov Sergey
*************************************************************************/
ae_int_t ablasblocksize(/* Real    */ const ae_matrix* a,
     ae_state *_state)
{
    ae_int_t result;


    result = 32;
    return result;
}


/*************************************************************************
Block size for complex subroutines.

  -- ALGLIB routine --
     15.12.2009
     Bochkanov Sergey
*************************************************************************/
ae_int_t ablascomplexblocksize(/* Complex */ const ae_matrix* a,
     ae_state *_state)
{
    ae_int_t result;


    result = 24;
    return result;
}


/*************************************************************************
Microblock size

  -- ALGLIB routine --
     15.12.2009
     Bochkanov Sergey
*************************************************************************/
ae_int_t ablasmicroblocksize(ae_state *_state)
{
    ae_int_t result;


    result = 8;
    return result;
}


/*************************************************************************
Generation of an elementary reflection transformation

The subroutine generates elementary reflection H of order N, so that, for
a given X, the following equality holds true:

    ( X(1) )   ( Beta )
H * (  ..  ) = (  0   )
    ( X(n) )   (  0   )

where
              ( V(1) )
H = 1 - Tau * (  ..  ) * ( V(1), ..., V(n) )
              ( V(n) )

where the first component of vector V equals 1.

Input parameters:
    X   -   vector. Array whose index ranges within [1..N].
    N   -   reflection order.

Output parameters:
    X   -   components from 2 to N are replaced with vector V.
            The first component is replaced with parameter Beta.
    Tau -   scalar value Tau. If X is a null vector, Tau equals 0,
            otherwise 1 <= Tau <= 2.

This subroutine is the modification of the DLARFG subroutines from
the LAPACK library.

MODIFICATIONS:
    24.12.2005 sign(Alpha) was replaced with an analogous to the Fortran SIGN code.

  -- LAPACK auxiliary routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     September 30, 1994
*************************************************************************/
void generatereflection(/* Real    */ ae_vector* x,
     ae_int_t n,
     double* tau,
     ae_state *_state)
{
    ae_int_t j;
    double alpha;
    double xnorm;
    double v;
    double beta;
    double mx;
    double s;

    *tau = 0.0;

    if( n<=1 )
    {
        *tau = (double)(0);
        return;
    }
    
    /*
     * Scale if needed (to avoid overflow/underflow during intermediate
     * calculations).
     */
    mx = (double)(0);
    for(j=1; j<=n; j++)
    {
        mx = ae_maxreal(ae_fabs(x->ptr.p_double[j], _state), mx, _state);
    }
    s = (double)(1);
    if( ae_fp_neq(mx,(double)(0)) )
    {
        if( ae_fp_less_eq(mx,ae_minrealnumber/ae_machineepsilon) )
        {
            s = ae_minrealnumber/ae_machineepsilon;
            v = (double)1/s;
            ae_v_muld(&x->ptr.p_double[1], 1, ae_v_len(1,n), v);
            mx = mx*v;
        }
        else
        {
            if( ae_fp_greater_eq(mx,ae_maxrealnumber*ae_machineepsilon) )
            {
                s = ae_maxrealnumber*ae_machineepsilon;
                v = (double)1/s;
                ae_v_muld(&x->ptr.p_double[1], 1, ae_v_len(1,n), v);
                mx = mx*v;
            }
        }
    }
    
    /*
     * XNORM = DNRM2( N-1, X, INCX )
     */
    alpha = x->ptr.p_double[1];
    xnorm = (double)(0);
    if( ae_fp_neq(mx,(double)(0)) )
    {
        for(j=2; j<=n; j++)
        {
            xnorm = xnorm+ae_sqr(x->ptr.p_double[j]/mx, _state);
        }
        xnorm = ae_sqrt(xnorm, _state)*mx;
    }
    if( ae_fp_eq(xnorm,(double)(0)) )
    {
        
        /*
         * H  =  I
         */
        *tau = (double)(0);
        x->ptr.p_double[1] = x->ptr.p_double[1]*s;
        return;
    }
    
    /*
     * general case
     */
    mx = ae_maxreal(ae_fabs(alpha, _state), ae_fabs(xnorm, _state), _state);
    beta = -mx*ae_sqrt(ae_sqr(alpha/mx, _state)+ae_sqr(xnorm/mx, _state), _state);
    if( ae_fp_less(alpha,(double)(0)) )
    {
        beta = -beta;
    }
    *tau = (beta-alpha)/beta;
    v = (double)1/(alpha-beta);
    ae_v_muld(&x->ptr.p_double[2], 1, ae_v_len(2,n), v);
    x->ptr.p_double[1] = beta;
    
    /*
     * Scale back outputs
     */
    x->ptr.p_double[1] = x->ptr.p_double[1]*s;
}


/*************************************************************************
Application of an elementary reflection to a rectangular matrix of size MxN

The algorithm pre-multiplies the matrix by an elementary reflection transformation
which is given by column V and scalar Tau (see the description of the
GenerateReflection procedure). Not the whole matrix but only a part of it
is transformed (rows from M1 to M2, columns from N1 to N2). Only the elements
of this submatrix are changed.

Input parameters:
    C       -   matrix to be transformed.
    Tau     -   scalar defining the transformation.
    V       -   column defining the transformation.
                Array whose index ranges within [1..M2-M1+1].
    M1, M2  -   range of rows to be transformed.
    N1, N2  -   range of columns to be transformed.
    WORK    -   working array whose indexes goes from N1 to N2.

Output parameters:
    C       -   the result of multiplying the input matrix C by the
                transformation matrix which is given by Tau and V.
                If N1>N2 or M1>M2, C is not modified.

  -- LAPACK auxiliary routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     September 30, 1994
*************************************************************************/
void applyreflectionfromtheleft(/* Real    */ ae_matrix* c,
     double tau,
     /* Real    */ const ae_vector* v,
     ae_int_t m1,
     ae_int_t m2,
     ae_int_t n1,
     ae_int_t n2,
     /* Real    */ ae_vector* work,
     ae_state *_state)
{


    if( (ae_fp_eq(tau,(double)(0))||n1>n2)||m1>m2 )
    {
        return;
    }
    rvectorsetlengthatleast(work, n2-n1+1, _state);
    rmatrixgemv(n2-n1+1, m2-m1+1, 1.0, c, m1, n1, 1, v, 1, 0.0, work, 0, _state);
    rmatrixger(m2-m1+1, n2-n1+1, c, m1, n1, -tau, v, 1, work, 0, _state);
}


/*************************************************************************
Application of an elementary reflection to a rectangular matrix of size MxN

The algorithm post-multiplies the matrix by an elementary reflection transformation
which is given by column V and scalar Tau (see the description of the
GenerateReflection procedure). Not the whole matrix but only a part of it
is transformed (rows from M1 to M2, columns from N1 to N2). Only the
elements of this submatrix are changed.

Input parameters:
    C       -   matrix to be transformed.
    Tau     -   scalar defining the transformation.
    V       -   column defining the transformation.
                Array whose index ranges within [1..N2-N1+1].
    M1, M2  -   range of rows to be transformed.
    N1, N2  -   range of columns to be transformed.
    WORK    -   working array whose indexes goes from M1 to M2.

Output parameters:
    C       -   the result of multiplying the input matrix C by the
                transformation matrix which is given by Tau and V.
                If N1>N2 or M1>M2, C is not modified.

  -- LAPACK auxiliary routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     September 30, 1994
*************************************************************************/
void applyreflectionfromtheright(/* Real    */ ae_matrix* c,
     double tau,
     /* Real    */ const ae_vector* v,
     ae_int_t m1,
     ae_int_t m2,
     ae_int_t n1,
     ae_int_t n2,
     /* Real    */ ae_vector* work,
     ae_state *_state)
{


    if( (ae_fp_eq(tau,(double)(0))||n1>n2)||m1>m2 )
    {
        return;
    }
    rvectorsetlengthatleast(work, m2-m1+1, _state);
    rmatrixgemv(m2-m1+1, n2-n1+1, 1.0, c, m1, n1, 0, v, 1, 0.0, work, 0, _state);
    rmatrixger(m2-m1+1, n2-n1+1, c, m1, n1, -tau, work, 0, v, 1, _state);
}


/*************************************************************************
Cache-oblivous complex "copy-and-transpose"

Input parameters:
    M   -   number of rows
    N   -   number of columns
    A   -   source matrix, MxN submatrix is copied and transposed
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    B   -   destination matrix, must be large enough to store result
    IB  -   submatrix offset (row index)
    JB  -   submatrix offset (column index)
*************************************************************************/
void cmatrixtranspose(ae_int_t m,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Complex */ ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t s1;
    ae_int_t s2;


    if( m<=2*ablascomplexblocksize(a, _state)&&n<=2*ablascomplexblocksize(a, _state) )
    {
        
        /*
         * base case
         */
        for(i=0; i<=m-1; i++)
        {
            ae_v_cmove(&b->ptr.pp_complex[ib][jb+i], b->stride, &a->ptr.pp_complex[ia+i][ja], 1, "N", ae_v_len(ib,ib+n-1));
        }
    }
    else
    {
        
        /*
         * Cache-oblivious recursion
         */
        if( m>n )
        {
            ablascomplexsplitlength(a, m, &s1, &s2, _state);
            cmatrixtranspose(s1, n, a, ia, ja, b, ib, jb, _state);
            cmatrixtranspose(s2, n, a, ia+s1, ja, b, ib, jb+s1, _state);
        }
        else
        {
            ablascomplexsplitlength(a, n, &s1, &s2, _state);
            cmatrixtranspose(m, s1, a, ia, ja, b, ib, jb, _state);
            cmatrixtranspose(m, s2, a, ia, ja+s1, b, ib+s1, jb, _state);
        }
    }
}


/*************************************************************************
Cache-oblivous real "copy-and-transpose"

Input parameters:
    M   -   number of rows
    N   -   number of columns
    A   -   source matrix, MxN submatrix is copied and transposed
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    B   -   destination matrix, must be large enough to store result
    IB  -   submatrix offset (row index)
    JB  -   submatrix offset (column index)
*************************************************************************/
void rmatrixtranspose(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Real    */ ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t s1;
    ae_int_t s2;


    if( m<=2*ablasblocksize(a, _state)&&n<=2*ablasblocksize(a, _state) )
    {
        
        /*
         * base case
         */
        for(i=0; i<=m-1; i++)
        {
            ae_v_move(&b->ptr.pp_double[ib][jb+i], b->stride, &a->ptr.pp_double[ia+i][ja], 1, ae_v_len(ib,ib+n-1));
        }
    }
    else
    {
        
        /*
         * Cache-oblivious recursion
         */
        if( m>n )
        {
            ablassplitlength(a, m, &s1, &s2, _state);
            rmatrixtranspose(s1, n, a, ia, ja, b, ib, jb, _state);
            rmatrixtranspose(s2, n, a, ia+s1, ja, b, ib, jb+s1, _state);
        }
        else
        {
            ablassplitlength(a, n, &s1, &s2, _state);
            rmatrixtranspose(m, s1, a, ia, ja, b, ib, jb, _state);
            rmatrixtranspose(m, s2, a, ia, ja+s1, b, ib+s1, jb, _state);
        }
    }
}


/*************************************************************************
This code enforces symmetricy of the matrix by copying Upper part to lower
one (or vice versa).

INPUT PARAMETERS:
    A   -   matrix
    N   -   number of rows/columns
    IsUpper - whether we want to copy upper triangle to lower one (True)
            or vice versa (False).
*************************************************************************/
void rmatrixenforcesymmetricity(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;


    if( isupper )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=i+1; j<=n-1; j++)
            {
                a->ptr.pp_double[j][i] = a->ptr.pp_double[i][j];
            }
        }
    }
    else
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=i+1; j<=n-1; j++)
            {
                a->ptr.pp_double[i][j] = a->ptr.pp_double[j][i];
            }
        }
    }
}


/*************************************************************************
Copy

Input parameters:
    M   -   number of rows
    N   -   number of columns
    A   -   source matrix, MxN submatrix is copied and transposed
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    B   -   destination matrix, must be large enough to store result
    IB  -   submatrix offset (row index)
    JB  -   submatrix offset (column index)
*************************************************************************/
void cmatrixcopy(ae_int_t m,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Complex */ ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_state *_state)
{
    ae_int_t i;


    if( m==0||n==0 )
    {
        return;
    }
    for(i=0; i<=m-1; i++)
    {
        ae_v_cmove(&b->ptr.pp_complex[ib+i][jb], 1, &a->ptr.pp_complex[ia+i][ja], 1, "N", ae_v_len(jb,jb+n-1));
    }
}


/*************************************************************************
Copy

Input parameters:
    N   -   subvector size
    A   -   source vector, N elements are copied
    IA  -   source offset (first element index)
    B   -   destination vector, must be large enough to store result
    IB  -   destination offset (first element index)
*************************************************************************/
void rvectorcopy(ae_int_t n,
     /* Real    */ const ae_vector* a,
     ae_int_t ia,
     /* Real    */ ae_vector* b,
     ae_int_t ib,
     ae_state *_state)
{


    if( n==0 )
    {
        return;
    }
    if( ia==0&&ib==0 )
    {
        rcopyv(n, a, b, _state);
    }
    else
    {
        rcopyvx(n, a, ia, b, ib, _state);
    }
}


/*************************************************************************
Copy

Input parameters:
    M   -   number of rows
    N   -   number of columns
    A   -   source matrix, MxN submatrix is copied and transposed
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    B   -   destination matrix, must be large enough to store result
    IB  -   submatrix offset (row index)
    JB  -   submatrix offset (column index)
*************************************************************************/
void rmatrixcopy(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Real    */ ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_state *_state)
{
    ae_int_t i;


    if( m==0||n==0 )
    {
        return;
    }
    for(i=0; i<=m-1; i++)
    {
        ae_v_move(&b->ptr.pp_double[ib+i][jb], 1, &a->ptr.pp_double[ia+i][ja], 1, ae_v_len(jb,jb+n-1));
    }
}


/*************************************************************************
Performs generalized copy: B := Beta*B + Alpha*A.

If Beta=0, then previous contents of B is simply ignored. If Alpha=0, then
A is ignored and not referenced. If both Alpha and Beta  are  zero,  B  is
filled by zeros.

Input parameters:
    M   -   number of rows
    N   -   number of columns
    Alpha-  coefficient
    A   -   source matrix, MxN submatrix is copied and transposed
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    Beta-   coefficient
    B   -   destination matrix, must be large enough to store result
    IB  -   submatrix offset (row index)
    JB  -   submatrix offset (column index)
*************************************************************************/
void rmatrixgencopy(ae_int_t m,
     ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     double beta,
     /* Real    */ ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;


    if( m==0||n==0 )
    {
        return;
    }
    
    /*
     * Zero-fill
     */
    if( ae_fp_eq(alpha,(double)(0))&&ae_fp_eq(beta,(double)(0)) )
    {
        for(i=0; i<=m-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                b->ptr.pp_double[ib+i][jb+j] = (double)(0);
            }
        }
        return;
    }
    
    /*
     * Inplace multiply
     */
    if( ae_fp_eq(alpha,(double)(0)) )
    {
        for(i=0; i<=m-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                b->ptr.pp_double[ib+i][jb+j] = beta*b->ptr.pp_double[ib+i][jb+j];
            }
        }
        return;
    }
    
    /*
     * Multiply and copy
     */
    if( ae_fp_eq(beta,(double)(0)) )
    {
        for(i=0; i<=m-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                b->ptr.pp_double[ib+i][jb+j] = alpha*a->ptr.pp_double[ia+i][ja+j];
            }
        }
        return;
    }
    
    /*
     * Generic
     */
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            b->ptr.pp_double[ib+i][jb+j] = alpha*a->ptr.pp_double[ia+i][ja+j]+beta*b->ptr.pp_double[ib+i][jb+j];
        }
    }
}


/*************************************************************************
Rank-1 correction: A := A + alpha*u*v'

NOTE: this  function  expects  A  to  be  large enough to store result. No
      automatic preallocation happens for  smaller  arrays.  No  integrity
      checks is performed for sizes of A, u, v.

INPUT PARAMETERS:
    M   -   number of rows
    N   -   number of columns
    A   -   target matrix, MxN submatrix is updated
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    Alpha-  coefficient
    U   -   vector #1
    IU  -   subvector offset
    V   -   vector #2
    IV  -   subvector offset


  -- ALGLIB routine --

     16.10.2017
     Bochkanov Sergey
*************************************************************************/
void rmatrixger(ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     double alpha,
     /* Real    */ const ae_vector* u,
     ae_int_t iu,
     /* Real    */ const ae_vector* v,
     ae_int_t iv,
     ae_state *_state)
{
    ae_int_t i;
    double s;


    
    /*
     * Quick exit
     */
    if( m<=0||n<=0 )
    {
        return;
    }
    
    /*
     * Try fast kernels:
     * * vendor kernel
     * * internal kernel
     */
    if( m>ablas_blas2minvendorkernelsize&&n>ablas_blas2minvendorkernelsize )
    {
        
        /*
         * Try MKL kernel first
         */
        if( rmatrixgermkl(m, n, a, ia, ja, alpha, u, iu, v, iv, _state) )
        {
            return;
        }
    }
    if( rmatrixgerf(m, n, a, ia, ja, alpha, u, iu, v, iv, _state) )
    {
        return;
    }
    
    /*
     * Generic code
     */
    for(i=0; i<=m-1; i++)
    {
        s = alpha*u->ptr.p_double[iu+i];
        ae_v_addd(&a->ptr.pp_double[ia+i][ja], 1, &v->ptr.p_double[iv], 1, ae_v_len(ja,ja+n-1), s);
    }
}


/*************************************************************************
Rank-1 correction: A := A + u*v'

INPUT PARAMETERS:
    M   -   number of rows
    N   -   number of columns
    A   -   target matrix, MxN submatrix is updated
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    U   -   vector #1
    IU  -   subvector offset
    V   -   vector #2
    IV  -   subvector offset
*************************************************************************/
void cmatrixrank1(ae_int_t m,
     ae_int_t n,
     /* Complex */ ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Complex */ const ae_vector* u,
     ae_int_t iu,
     /* Complex */ const ae_vector* v,
     ae_int_t iv,
     ae_state *_state)
{
    ae_int_t i;
    ae_complex s;


    
    /*
     * Quick exit
     */
    if( m<=0||n<=0 )
    {
        return;
    }
    
    /*
     * Try fast kernels:
     * * vendor kernel
     * * internal kernel
     */
    if( m>ablas_blas2minvendorkernelsize&&n>ablas_blas2minvendorkernelsize )
    {
        
        /*
         * Try MKL kernel first
         */
        if( cmatrixrank1mkl(m, n, a, ia, ja, u, iu, v, iv, _state) )
        {
            return;
        }
    }
    if( cmatrixrank1f(m, n, a, ia, ja, u, iu, v, iv, _state) )
    {
        return;
    }
    
    /*
     * Generic code
     */
    for(i=0; i<=m-1; i++)
    {
        s = u->ptr.p_complex[iu+i];
        ae_v_caddc(&a->ptr.pp_complex[ia+i][ja], 1, &v->ptr.p_complex[iv], 1, "N", ae_v_len(ja,ja+n-1), s);
    }
}


/*************************************************************************
IMPORTANT: this function is deprecated since ALGLIB 3.13. Use RMatrixGER()
           which is more generic version of this function.

Rank-1 correction: A := A + u*v'

INPUT PARAMETERS:
    M   -   number of rows
    N   -   number of columns
    A   -   target matrix, MxN submatrix is updated
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    U   -   vector #1
    IU  -   subvector offset
    V   -   vector #2
    IV  -   subvector offset
*************************************************************************/
void rmatrixrank1(ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Real    */ const ae_vector* u,
     ae_int_t iu,
     /* Real    */ const ae_vector* v,
     ae_int_t iv,
     ae_state *_state)
{
    ae_int_t i;
    double s;


    
    /*
     * Quick exit
     */
    if( m<=0||n<=0 )
    {
        return;
    }
    
    /*
     * Try fast kernels:
     * * vendor kernel
     * * internal kernel
     */
    if( m>ablas_blas2minvendorkernelsize&&n>ablas_blas2minvendorkernelsize )
    {
        
        /*
         * Try MKL kernel first
         */
        if( rmatrixrank1mkl(m, n, a, ia, ja, u, iu, v, iv, _state) )
        {
            return;
        }
    }
    if( rmatrixrank1f(m, n, a, ia, ja, u, iu, v, iv, _state) )
    {
        return;
    }
    
    /*
     * Generic code
     */
    for(i=0; i<=m-1; i++)
    {
        s = u->ptr.p_double[iu+i];
        ae_v_addd(&a->ptr.pp_double[ia+i][ja], 1, &v->ptr.p_double[iv], 1, ae_v_len(ja,ja+n-1), s);
    }
}


void rmatrixgemv(ae_int_t m,
     ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t opa,
     /* Real    */ const ae_vector* x,
     ae_int_t ix,
     double beta,
     /* Real    */ ae_vector* y,
     ae_int_t iy,
     ae_state *_state)
{


    
    /*
     * Quick exit for M=0, N=0 or Alpha=0.
     *
     * After this block we have M>0, N>0, Alpha<>0.
     */
    if( m<=0 )
    {
        return;
    }
    if( n<=0||ae_fp_eq(alpha,0.0) )
    {
        if( ae_fp_neq(beta,(double)(0)) )
        {
            rmulvx(m, beta, y, iy, _state);
        }
        else
        {
            rsetvx(m, 0.0, y, iy, _state);
        }
        return;
    }
    
    /*
     * Try fast kernels
     */
    if( m>ablas_blas2minvendorkernelsize&&n>ablas_blas2minvendorkernelsize )
    {
        
        /*
         * Try MKL kernel
         */
        if( rmatrixgemvmkl(m, n, alpha, a, ia, ja, opa, x, ix, beta, y, iy, _state) )
        {
            return;
        }
    }
    if( ia+ja+ix+iy==0 )
    {
        rgemv(m, n, alpha, a, opa, x, beta, y, _state);
    }
    else
    {
        rgemvx(m, n, alpha, a, ia, ja, opa, x, ix, beta, y, iy, _state);
    }
}


/*************************************************************************
Matrix-vector product: y := op(A)*x

INPUT PARAMETERS:
    M   -   number of rows of op(A)
            M>=0
    N   -   number of columns of op(A)
            N>=0
    A   -   target matrix
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    OpA -   operation type:
            * OpA=0     =>  op(A) = A
            * OpA=1     =>  op(A) = A^T
            * OpA=2     =>  op(A) = A^H
    X   -   input vector
    IX  -   subvector offset
    IY  -   subvector offset
    Y   -   preallocated matrix, must be large enough to store result

OUTPUT PARAMETERS:
    Y   -   vector which stores result

if M=0, then subroutine does nothing.
if N=0, Y is filled by zeros.


  -- ALGLIB routine --

     28.01.2010
     Bochkanov Sergey
*************************************************************************/
void cmatrixmv(ae_int_t m,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t opa,
     /* Complex */ const ae_vector* x,
     ae_int_t ix,
     /* Complex */ ae_vector* y,
     ae_int_t iy,
     ae_state *_state)
{
    ae_int_t i;
    ae_complex v;


    
    /*
     * Quick exit
     */
    if( m==0 )
    {
        return;
    }
    if( n==0 )
    {
        for(i=0; i<=m-1; i++)
        {
            y->ptr.p_complex[iy+i] = ae_complex_from_i(0);
        }
        return;
    }
    
    /*
     * Try fast kernels
     */
    if( m>ablas_blas2minvendorkernelsize&&n>ablas_blas2minvendorkernelsize )
    {
        
        /*
         * Try MKL kernel
         */
        if( cmatrixmvmkl(m, n, a, ia, ja, opa, x, ix, y, iy, _state) )
        {
            return;
        }
    }
    
    /*
     * Generic code
     */
    if( opa==0 )
    {
        
        /*
         * y = A*x
         */
        for(i=0; i<=m-1; i++)
        {
            v = ae_v_cdotproduct(&a->ptr.pp_complex[ia+i][ja], 1, "N", &x->ptr.p_complex[ix], 1, "N", ae_v_len(ja,ja+n-1));
            y->ptr.p_complex[iy+i] = v;
        }
        return;
    }
    if( opa==1 )
    {
        
        /*
         * y = A^T*x
         */
        for(i=0; i<=m-1; i++)
        {
            y->ptr.p_complex[iy+i] = ae_complex_from_i(0);
        }
        for(i=0; i<=n-1; i++)
        {
            v = x->ptr.p_complex[ix+i];
            ae_v_caddc(&y->ptr.p_complex[iy], 1, &a->ptr.pp_complex[ia+i][ja], 1, "N", ae_v_len(iy,iy+m-1), v);
        }
        return;
    }
    if( opa==2 )
    {
        
        /*
         * y = A^H*x
         */
        for(i=0; i<=m-1; i++)
        {
            y->ptr.p_complex[iy+i] = ae_complex_from_i(0);
        }
        for(i=0; i<=n-1; i++)
        {
            v = x->ptr.p_complex[ix+i];
            ae_v_caddc(&y->ptr.p_complex[iy], 1, &a->ptr.pp_complex[ia+i][ja], 1, "Conj", ae_v_len(iy,iy+m-1), v);
        }
        return;
    }
}


/*************************************************************************
IMPORTANT: this function is deprecated since ALGLIB 3.13. Use RMatrixGEMV()
           which is more generic version of this function.
           
Matrix-vector product: y := op(A)*x

INPUT PARAMETERS:
    M   -   number of rows of op(A)
    N   -   number of columns of op(A)
    A   -   target matrix
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    OpA -   operation type:
            * OpA=0     =>  op(A) = A
            * OpA=1     =>  op(A) = A^T
    X   -   input vector
    IX  -   subvector offset
    IY  -   subvector offset
    Y   -   preallocated matrix, must be large enough to store result

OUTPUT PARAMETERS:
    Y   -   vector which stores result

if M=0, then subroutine does nothing.
if N=0, Y is filled by zeros.


  -- ALGLIB routine --

     28.01.2010
     Bochkanov Sergey
*************************************************************************/
void rmatrixmv(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t opa,
     /* Real    */ const ae_vector* x,
     ae_int_t ix,
     /* Real    */ ae_vector* y,
     ae_int_t iy,
     ae_state *_state)
{
    ae_int_t i;
    double v;


    
    /*
     * Quick exit
     */
    if( m==0 )
    {
        return;
    }
    if( n==0 )
    {
        for(i=0; i<=m-1; i++)
        {
            y->ptr.p_double[iy+i] = (double)(0);
        }
        return;
    }
    
    /*
     * Try fast kernels
     */
    if( m>ablas_blas2minvendorkernelsize&&n>ablas_blas2minvendorkernelsize )
    {
        
        /*
         * Try MKL kernel
         */
        if( rmatrixmvmkl(m, n, a, ia, ja, opa, x, ix, y, iy, _state) )
        {
            return;
        }
    }
    
    /*
     * Generic code
     */
    if( opa==0 )
    {
        
        /*
         * y = A*x
         */
        for(i=0; i<=m-1; i++)
        {
            v = ae_v_dotproduct(&a->ptr.pp_double[ia+i][ja], 1, &x->ptr.p_double[ix], 1, ae_v_len(ja,ja+n-1));
            y->ptr.p_double[iy+i] = v;
        }
        return;
    }
    if( opa==1 )
    {
        
        /*
         * y = A^T*x
         */
        for(i=0; i<=m-1; i++)
        {
            y->ptr.p_double[iy+i] = (double)(0);
        }
        for(i=0; i<=n-1; i++)
        {
            v = x->ptr.p_double[ix+i];
            ae_v_addd(&y->ptr.p_double[iy], 1, &a->ptr.pp_double[ia+i][ja], 1, ae_v_len(iy,iy+m-1), v);
        }
        return;
    }
}


void rmatrixsymv(ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_bool isupper,
     /* Real    */ const ae_vector* x,
     ae_int_t ix,
     double beta,
     /* Real    */ ae_vector* y,
     ae_int_t iy,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;
    double vr;
    double vx;


    
    /*
     * Quick exit for M=0, N=0 or Alpha=0.
     *
     * After this block we have M>0, N>0, Alpha<>0.
     */
    if( n<=0 )
    {
        return;
    }
    if( ae_fp_eq(alpha,0.0) )
    {
        if( ae_fp_neq(beta,(double)(0)) )
        {
            for(i=0; i<=n-1; i++)
            {
                y->ptr.p_double[iy+i] = beta*y->ptr.p_double[iy+i];
            }
        }
        else
        {
            for(i=0; i<=n-1; i++)
            {
                y->ptr.p_double[iy+i] = 0.0;
            }
        }
        return;
    }
    
    /*
     * Try fast kernels
     */
    if( n>ablas_blas2minvendorkernelsize )
    {
        
        /*
         * Try MKL kernel
         */
        if( rmatrixsymvmkl(n, alpha, a, ia, ja, isupper, x, ix, beta, y, iy, _state) )
        {
            return;
        }
    }
    
    /*
     * Generic code
     */
    if( ae_fp_neq(beta,(double)(0)) )
    {
        for(i=0; i<=n-1; i++)
        {
            y->ptr.p_double[iy+i] = beta*y->ptr.p_double[iy+i];
        }
    }
    else
    {
        for(i=0; i<=n-1; i++)
        {
            y->ptr.p_double[iy+i] = 0.0;
        }
    }
    if( isupper )
    {
        
        /*
         * Upper triangle of A is stored
         */
        for(i=0; i<=n-1; i++)
        {
            
            /*
             * Process diagonal element
             */
            v = alpha*a->ptr.pp_double[ia+i][ja+i];
            y->ptr.p_double[iy+i] = y->ptr.p_double[iy+i]+v*x->ptr.p_double[ix+i];
            
            /*
             * Process off-diagonal elements
             */
            vr = 0.0;
            vx = x->ptr.p_double[ix+i];
            for(j=i+1; j<=n-1; j++)
            {
                v = alpha*a->ptr.pp_double[ia+i][ja+j];
                y->ptr.p_double[iy+j] = y->ptr.p_double[iy+j]+v*vx;
                vr = vr+v*x->ptr.p_double[ix+j];
            }
            y->ptr.p_double[iy+i] = y->ptr.p_double[iy+i]+vr;
        }
    }
    else
    {
        
        /*
         * Lower triangle of A is stored
         */
        for(i=0; i<=n-1; i++)
        {
            
            /*
             * Process diagonal element
             */
            v = alpha*a->ptr.pp_double[ia+i][ja+i];
            y->ptr.p_double[iy+i] = y->ptr.p_double[iy+i]+v*x->ptr.p_double[ix+i];
            
            /*
             * Process off-diagonal elements
             */
            vr = 0.0;
            vx = x->ptr.p_double[ix+i];
            for(j=0; j<=i-1; j++)
            {
                v = alpha*a->ptr.pp_double[ia+i][ja+j];
                y->ptr.p_double[iy+j] = y->ptr.p_double[iy+j]+v*vx;
                vr = vr+v*x->ptr.p_double[ix+j];
            }
            y->ptr.p_double[iy+i] = y->ptr.p_double[iy+i]+vr;
        }
    }
}


double rmatrixsyvmv(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_bool isupper,
     /* Real    */ const ae_vector* x,
     ae_int_t ix,
     /* Real    */ ae_vector* tmp,
     ae_state *_state)
{
    ae_int_t i;
    double result;


    
    /*
     * Quick exit for N=0
     */
    if( n<=0 )
    {
        result = (double)(0);
        return result;
    }
    
    /*
     * Generic code
     */
    rmatrixsymv(n, 1.0, a, ia, ja, isupper, x, ix, 0.0, tmp, 0, _state);
    result = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        result = result+x->ptr.p_double[ix+i]*tmp->ptr.p_double[i];
    }
    return result;
}


/*************************************************************************
This subroutine solves linear system op(A)*x=b where:
* A is NxN upper/lower triangular/unitriangular matrix
* X and B are Nx1 vectors
* "op" may be identity transformation or transposition

Solution replaces X.

IMPORTANT: * no overflow/underflow/denegeracy tests is performed.
           * no integrity checks for operand sizes, out-of-bounds accesses
             and so on is performed

INPUT PARAMETERS
    N   -   matrix size, N>=0
    A       -   matrix, actial matrix is stored in A[IA:IA+N-1,JA:JA+N-1]
    IA      -   submatrix offset
    JA      -   submatrix offset
    IsUpper -   whether matrix is upper triangular
    IsUnit  -   whether matrix is unitriangular
    OpType  -   transformation type:
                * 0 - no transformation
                * 1 - transposition
    X       -   right part, actual vector is stored in X[IX:IX+N-1]
    IX      -   offset
    
OUTPUT PARAMETERS
    X       -   solution replaces elements X[IX:IX+N-1]

  -- ALGLIB routine / remastering of LAPACK's DTRSV --
     (c) 2017 Bochkanov Sergey - converted to ALGLIB
     (c) 2016 Reference BLAS level1 routine (LAPACK version 3.7.0)
     Reference BLAS is a software package provided by Univ. of Tennessee,
     Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd.
*************************************************************************/
void rmatrixtrsv(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Real    */ ae_vector* x,
     ae_int_t ix,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;


    
    /*
     * Quick exit
     */
    if( n<=0 )
    {
        return;
    }
    
    /*
     * Try fast kernels
     */
    if( n>ablas_blas2minvendorkernelsize )
    {
        
        /*
         * Try MKL kernel
         */
        if( rmatrixtrsvmkl(n, a, ia, ja, isupper, isunit, optype, x, ix, _state) )
        {
            return;
        }
    }
    
    /*
     * Generic code
     */
    if( optype==0&&isupper )
    {
        for(i=n-1; i>=0; i--)
        {
            v = x->ptr.p_double[ix+i];
            for(j=i+1; j<=n-1; j++)
            {
                v = v-a->ptr.pp_double[ia+i][ja+j]*x->ptr.p_double[ix+j];
            }
            if( !isunit )
            {
                v = v/a->ptr.pp_double[ia+i][ja+i];
            }
            x->ptr.p_double[ix+i] = v;
        }
        return;
    }
    if( optype==0&&!isupper )
    {
        for(i=0; i<=n-1; i++)
        {
            v = x->ptr.p_double[ix+i];
            for(j=0; j<=i-1; j++)
            {
                v = v-a->ptr.pp_double[ia+i][ja+j]*x->ptr.p_double[ix+j];
            }
            if( !isunit )
            {
                v = v/a->ptr.pp_double[ia+i][ja+i];
            }
            x->ptr.p_double[ix+i] = v;
        }
        return;
    }
    if( optype==1&&isupper )
    {
        for(i=0; i<=n-1; i++)
        {
            v = x->ptr.p_double[ix+i];
            if( !isunit )
            {
                v = v/a->ptr.pp_double[ia+i][ja+i];
            }
            x->ptr.p_double[ix+i] = v;
            if( v==(double)0 )
            {
                continue;
            }
            for(j=i+1; j<=n-1; j++)
            {
                x->ptr.p_double[ix+j] = x->ptr.p_double[ix+j]-v*a->ptr.pp_double[ia+i][ja+j];
            }
        }
        return;
    }
    if( optype==1&&!isupper )
    {
        for(i=n-1; i>=0; i--)
        {
            v = x->ptr.p_double[ix+i];
            if( !isunit )
            {
                v = v/a->ptr.pp_double[ia+i][ja+i];
            }
            x->ptr.p_double[ix+i] = v;
            if( v==(double)0 )
            {
                continue;
            }
            for(j=0; j<=i-1; j++)
            {
                x->ptr.p_double[ix+j] = x->ptr.p_double[ix+j]-v*a->ptr.pp_double[ia+i][ja+j];
            }
        }
        return;
    }
    ae_assert(ae_false, "RMatrixTRSV: unexpected operation type", _state);
}


/*************************************************************************
This subroutine calculates X*op(A^-1) where:
* X is MxN general matrix
* A is NxN upper/lower triangular/unitriangular matrix
* "op" may be identity transformation, transposition, conjugate transposition
Multiplication result replaces X.

INPUT PARAMETERS
    N   -   matrix size, N>=0
    M   -   matrix size, N>=0
    A       -   matrix, actial matrix is stored in A[I1:I1+N-1,J1:J1+N-1]
    I1      -   submatrix offset
    J1      -   submatrix offset
    IsUpper -   whether matrix is upper triangular
    IsUnit  -   whether matrix is unitriangular
    OpType  -   transformation type:
                * 0 - no transformation
                * 1 - transposition
                * 2 - conjugate transposition
    X   -   matrix, actial matrix is stored in X[I2:I2+M-1,J2:J2+N-1]
    I2  -   submatrix offset
    J2  -   submatrix offset

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

  -- ALGLIB routine --
     20.01.2018
     Bochkanov Sergey
*************************************************************************/
void cmatrixrighttrsm(ae_int_t m,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Complex */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state)
{
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t s1;
    ae_int_t s2;
    ae_int_t tsa;
    ae_int_t tsb;
    ae_int_t tscur;


    tsa = matrixtilesizea(_state)/2;
    tsb = matrixtilesizeb(_state);
    tscur = tsb;
    if( imax2(m, n, _state)<=tsb )
    {
        tscur = tsa;
    }
    ae_assert(tscur>=1, "CMatrixRightTRSM: integrity check failed", _state);
    
    /*
     * Upper level parallelization:
     * * decide whether it is feasible to activate multithreading
     * * perform optionally parallelized splits on M
     */
    if( m>=2*tsb&&ae_fp_greater_eq((double)4*rmul3((double)(m), (double)(n), (double)(n), _state),smpactivationlevel(_state)) )
    {
        if( _trypexec_cmatrixrighttrsm(m,n,a,i1,j1,isupper,isunit,optype,x,i2,j2, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    if( m>=2*tsb )
    {
        
        /*
         * Split X: X*A = (X1 X2)^T*A
         */
        tiledsplit(m, tsb, &s1, &s2, _state);
        ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_fp_greater_eq((double)4*rmul3((double)(imin2(s1, s2, _state)), (double)(n), (double)(n), _state),spawnlevel(_state)), _state);
        _spawn_cmatrixrighttrsm(s1, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _child_tasks, _smp_enabled, _state);
        cmatrixrighttrsm(s2, n, a, i1, j1, isupper, isunit, optype, x, i2+s1, j2, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Basecase: either MKL-supported code or ALGLIB basecase code
     */
    if( imax2(m, n, _state)<=tsb )
    {
        if( cmatrixrighttrsmmkl(m, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    if( imax2(m, n, _state)<=tsa )
    {
        ablas_cmatrixrighttrsm2(m, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Recursive subdivision
     */
    if( m>=n )
    {
        
        /*
         * Split X: X*A = (X1 X2)^T*A
         */
        tiledsplit(m, tscur, &s1, &s2, _state);
        cmatrixrighttrsm(s1, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
        cmatrixrighttrsm(s2, n, a, i1, j1, isupper, isunit, optype, x, i2+s1, j2, _state);
    }
    else
    {
        
        /*
         * Split A:
         *               (A1  A12)
         * X*op(A) = X*op(       )
         *               (     A2)
         *
         * Different variants depending on
         * IsUpper/OpType combinations
         */
        tiledsplit(n, tscur, &s1, &s2, _state);
        if( isupper&&optype==0 )
        {
            
            /*
             *                  (A1  A12)-1
             * X*A^-1 = (X1 X2)*(       )
             *                  (     A2)
             */
            cmatrixrighttrsm(m, s1, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
            cmatrixgemm(m, s2, s1, ae_complex_from_d(-1.0), x, i2, j2, 0, a, i1, j1+s1, 0, ae_complex_from_d(1.0), x, i2, j2+s1, _state);
            cmatrixrighttrsm(m, s2, a, i1+s1, j1+s1, isupper, isunit, optype, x, i2, j2+s1, _state);
        }
        if( isupper&&optype!=0 )
        {
            
            /*
             *                  (A1'     )-1
             * X*A^-1 = (X1 X2)*(        )
             *                  (A12' A2')
             */
            cmatrixrighttrsm(m, s2, a, i1+s1, j1+s1, isupper, isunit, optype, x, i2, j2+s1, _state);
            cmatrixgemm(m, s1, s2, ae_complex_from_d(-1.0), x, i2, j2+s1, 0, a, i1, j1+s1, optype, ae_complex_from_d(1.0), x, i2, j2, _state);
            cmatrixrighttrsm(m, s1, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
        }
        if( !isupper&&optype==0 )
        {
            
            /*
             *                  (A1     )-1
             * X*A^-1 = (X1 X2)*(       )
             *                  (A21  A2)
             */
            cmatrixrighttrsm(m, s2, a, i1+s1, j1+s1, isupper, isunit, optype, x, i2, j2+s1, _state);
            cmatrixgemm(m, s1, s2, ae_complex_from_d(-1.0), x, i2, j2+s1, 0, a, i1+s1, j1, 0, ae_complex_from_d(1.0), x, i2, j2, _state);
            cmatrixrighttrsm(m, s1, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
        }
        if( !isupper&&optype!=0 )
        {
            
            /*
             *                  (A1' A21')-1
             * X*A^-1 = (X1 X2)*(        )
             *                  (     A2')
             */
            cmatrixrighttrsm(m, s1, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
            cmatrixgemm(m, s2, s1, ae_complex_from_d(-1.0), x, i2, j2, 0, a, i1+s1, j1, optype, ae_complex_from_d(1.0), x, i2, j2+s1, _state);
            cmatrixrighttrsm(m, s2, a, i1+s1, j1+s1, isupper, isunit, optype, x, i2, j2+s1, _state);
        }
    }
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_cmatrixrighttrsm(ae_int_t m,
    ae_int_t n,
    /* Complex */ const ae_matrix* a,
    ae_int_t i1,
    ae_int_t j1,
    ae_bool isupper,
    ae_bool isunit,
    ae_int_t optype,
    /* Complex */ ae_matrix* x,
    ae_int_t i2,
    ae_int_t j2,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        cmatrixrighttrsm(m,n,a,i1,j1,isupper,isunit,optype,x,i2,j2, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_cmatrixrighttrsm;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.ival = m;
        _task->data.parameters[1].value.ival = n;
        _task->data.parameters[2].value.const_val = a;
        _task->data.parameters[3].value.ival = i1;
        _task->data.parameters[4].value.ival = j1;
        _task->data.parameters[5].value.bval = isupper;
        _task->data.parameters[6].value.bval = isunit;
        _task->data.parameters[7].value.ival = optype;
        _task->data.parameters[8].value.val = x;
        _task->data.parameters[9].value.ival = i2;
        _task->data.parameters[10].value.ival = j2;
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
void _task_cmatrixrighttrsm(ae_task_data *_data, ae_state *_state)
{
    ae_int_t m;
    ae_int_t n;
    const ae_matrix* a;
    ae_int_t i1;
    ae_int_t j1;
    ae_bool isupper;
    ae_bool isunit;
    ae_int_t optype;
    ae_matrix* x;
    ae_int_t i2;
    ae_int_t j2;
    m = _data->parameters[0].value.ival;
    n = _data->parameters[1].value.ival;
    a = (const ae_matrix*)_data->parameters[2].value.const_val;
    i1 = _data->parameters[3].value.ival;
    j1 = _data->parameters[4].value.ival;
    isupper = _data->parameters[5].value.bval;
    isunit = _data->parameters[6].value.bval;
    optype = _data->parameters[7].value.ival;
    x = (ae_matrix*)_data->parameters[8].value.val;
    i2 = _data->parameters[9].value.ival;
    j2 = _data->parameters[10].value.ival;
   ae_state_set_flags(_state, _data->flags);
   cmatrixrighttrsm(m,n,a,i1,j1,isupper,isunit,optype,x,i2,j2, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_cmatrixrighttrsm(ae_int_t m,
    ae_int_t n,
    /* Complex */ const ae_matrix* a,
    ae_int_t i1,
    ae_int_t j1,
    ae_bool isupper,
    ae_bool isunit,
    ae_int_t optype,
    /* Complex */ ae_matrix* x,
    ae_int_t i2,
    ae_int_t j2,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_cmatrixrighttrsm;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.ival = m;
    _task->data.parameters[1].value.ival = n;
    _task->data.parameters[2].value.const_val = a;
    _task->data.parameters[3].value.ival = i1;
    _task->data.parameters[4].value.ival = j1;
    _task->data.parameters[5].value.bval = isupper;
    _task->data.parameters[6].value.bval = isunit;
    _task->data.parameters[7].value.ival = optype;
    _task->data.parameters[8].value.val = x;
    _task->data.parameters[9].value.ival = i2;
    _task->data.parameters[10].value.ival = j2;
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
This subroutine calculates op(A^-1)*X where:
* X is MxN general matrix
* A is MxM upper/lower triangular/unitriangular matrix
* "op" may be identity transformation, transposition, conjugate transposition
Multiplication result replaces X.

INPUT PARAMETERS
    N   -   matrix size, N>=0
    M   -   matrix size, N>=0
    A       -   matrix, actial matrix is stored in A[I1:I1+M-1,J1:J1+M-1]
    I1      -   submatrix offset
    J1      -   submatrix offset
    IsUpper -   whether matrix is upper triangular
    IsUnit  -   whether matrix is unitriangular
    OpType  -   transformation type:
                * 0 - no transformation
                * 1 - transposition
                * 2 - conjugate transposition
    X   -   matrix, actial matrix is stored in X[I2:I2+M-1,J2:J2+N-1]
    I2  -   submatrix offset
    J2  -   submatrix offset

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

  -- ALGLIB routine --
     15.12.2009-22.01.2018
     Bochkanov Sergey
*************************************************************************/
void cmatrixlefttrsm(ae_int_t m,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Complex */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state)
{
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t s1;
    ae_int_t s2;
    ae_int_t tsa;
    ae_int_t tsb;
    ae_int_t tscur;


    tsa = matrixtilesizea(_state)/2;
    tsb = matrixtilesizeb(_state);
    tscur = tsb;
    if( imax2(m, n, _state)<=tsb )
    {
        tscur = tsa;
    }
    ae_assert(tscur>=1, "CMatrixLeftTRSM: integrity check failed", _state);
    
    /*
     * Upper level parallelization:
     * * decide whether it is feasible to activate multithreading
     * * perform optionally parallelized splits on N
     */
    if( n>=2*tsb&&ae_fp_greater_eq((double)4*rmul3((double)(n), (double)(m), (double)(m), _state),smpactivationlevel(_state)) )
    {
        if( _trypexec_cmatrixlefttrsm(m,n,a,i1,j1,isupper,isunit,optype,x,i2,j2, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    if( n>=2*tsb )
    {
        tiledsplit(n, tscur, &s1, &s2, _state);
        ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_fp_greater_eq((double)4*rmul3((double)(imin2(s1, s2, _state)), (double)(m), (double)(m), _state),spawnlevel(_state)), _state);
        _spawn_cmatrixlefttrsm(m, s2, a, i1, j1, isupper, isunit, optype, x, i2, j2+s1, _child_tasks, _smp_enabled, _state);
        cmatrixlefttrsm(m, s1, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Basecase: either MKL-supported code or ALGLIB basecase code
     */
    if( imax2(m, n, _state)<=tsb )
    {
        if( cmatrixlefttrsmmkl(m, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    if( imax2(m, n, _state)<=tsa )
    {
        ablas_cmatrixlefttrsm2(m, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Recursive subdivision
     */
    if( n>=m )
    {
        
        /*
         * Split X: op(A)^-1*X = op(A)^-1*(X1 X2)
         */
        tiledsplit(n, tscur, &s1, &s2, _state);
        cmatrixlefttrsm(m, s1, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
        cmatrixlefttrsm(m, s2, a, i1, j1, isupper, isunit, optype, x, i2, j2+s1, _state);
    }
    else
    {
        
        /*
         * Split A
         */
        tiledsplit(m, tscur, &s1, &s2, _state);
        if( isupper&&optype==0 )
        {
            
            /*
             *           (A1  A12)-1  ( X1 )
             * A^-1*X* = (       )   *(    )
             *           (     A2)    ( X2 )
             */
            cmatrixlefttrsm(s2, n, a, i1+s1, j1+s1, isupper, isunit, optype, x, i2+s1, j2, _state);
            cmatrixgemm(s1, n, s2, ae_complex_from_d(-1.0), a, i1, j1+s1, 0, x, i2+s1, j2, 0, ae_complex_from_d(1.0), x, i2, j2, _state);
            cmatrixlefttrsm(s1, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
        }
        if( isupper&&optype!=0 )
        {
            
            /*
             *          (A1'     )-1 ( X1 )
             * A^-1*X = (        )  *(    )
             *          (A12' A2')   ( X2 )
             */
            cmatrixlefttrsm(s1, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
            cmatrixgemm(s2, n, s1, ae_complex_from_d(-1.0), a, i1, j1+s1, optype, x, i2, j2, 0, ae_complex_from_d(1.0), x, i2+s1, j2, _state);
            cmatrixlefttrsm(s2, n, a, i1+s1, j1+s1, isupper, isunit, optype, x, i2+s1, j2, _state);
        }
        if( !isupper&&optype==0 )
        {
            
            /*
             *          (A1     )-1 ( X1 )
             * A^-1*X = (       )  *(    )
             *          (A21  A2)   ( X2 )
             */
            cmatrixlefttrsm(s1, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
            cmatrixgemm(s2, n, s1, ae_complex_from_d(-1.0), a, i1+s1, j1, 0, x, i2, j2, 0, ae_complex_from_d(1.0), x, i2+s1, j2, _state);
            cmatrixlefttrsm(s2, n, a, i1+s1, j1+s1, isupper, isunit, optype, x, i2+s1, j2, _state);
        }
        if( !isupper&&optype!=0 )
        {
            
            /*
             *          (A1' A21')-1 ( X1 )
             * A^-1*X = (        )  *(    )
             *          (     A2')   ( X2 )
             */
            cmatrixlefttrsm(s2, n, a, i1+s1, j1+s1, isupper, isunit, optype, x, i2+s1, j2, _state);
            cmatrixgemm(s1, n, s2, ae_complex_from_d(-1.0), a, i1+s1, j1, optype, x, i2+s1, j2, 0, ae_complex_from_d(1.0), x, i2, j2, _state);
            cmatrixlefttrsm(s1, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
        }
    }
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_cmatrixlefttrsm(ae_int_t m,
    ae_int_t n,
    /* Complex */ const ae_matrix* a,
    ae_int_t i1,
    ae_int_t j1,
    ae_bool isupper,
    ae_bool isunit,
    ae_int_t optype,
    /* Complex */ ae_matrix* x,
    ae_int_t i2,
    ae_int_t j2,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        cmatrixlefttrsm(m,n,a,i1,j1,isupper,isunit,optype,x,i2,j2, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_cmatrixlefttrsm;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.ival = m;
        _task->data.parameters[1].value.ival = n;
        _task->data.parameters[2].value.const_val = a;
        _task->data.parameters[3].value.ival = i1;
        _task->data.parameters[4].value.ival = j1;
        _task->data.parameters[5].value.bval = isupper;
        _task->data.parameters[6].value.bval = isunit;
        _task->data.parameters[7].value.ival = optype;
        _task->data.parameters[8].value.val = x;
        _task->data.parameters[9].value.ival = i2;
        _task->data.parameters[10].value.ival = j2;
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
void _task_cmatrixlefttrsm(ae_task_data *_data, ae_state *_state)
{
    ae_int_t m;
    ae_int_t n;
    const ae_matrix* a;
    ae_int_t i1;
    ae_int_t j1;
    ae_bool isupper;
    ae_bool isunit;
    ae_int_t optype;
    ae_matrix* x;
    ae_int_t i2;
    ae_int_t j2;
    m = _data->parameters[0].value.ival;
    n = _data->parameters[1].value.ival;
    a = (const ae_matrix*)_data->parameters[2].value.const_val;
    i1 = _data->parameters[3].value.ival;
    j1 = _data->parameters[4].value.ival;
    isupper = _data->parameters[5].value.bval;
    isunit = _data->parameters[6].value.bval;
    optype = _data->parameters[7].value.ival;
    x = (ae_matrix*)_data->parameters[8].value.val;
    i2 = _data->parameters[9].value.ival;
    j2 = _data->parameters[10].value.ival;
   ae_state_set_flags(_state, _data->flags);
   cmatrixlefttrsm(m,n,a,i1,j1,isupper,isunit,optype,x,i2,j2, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_cmatrixlefttrsm(ae_int_t m,
    ae_int_t n,
    /* Complex */ const ae_matrix* a,
    ae_int_t i1,
    ae_int_t j1,
    ae_bool isupper,
    ae_bool isunit,
    ae_int_t optype,
    /* Complex */ ae_matrix* x,
    ae_int_t i2,
    ae_int_t j2,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_cmatrixlefttrsm;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.ival = m;
    _task->data.parameters[1].value.ival = n;
    _task->data.parameters[2].value.const_val = a;
    _task->data.parameters[3].value.ival = i1;
    _task->data.parameters[4].value.ival = j1;
    _task->data.parameters[5].value.bval = isupper;
    _task->data.parameters[6].value.bval = isunit;
    _task->data.parameters[7].value.ival = optype;
    _task->data.parameters[8].value.val = x;
    _task->data.parameters[9].value.ival = i2;
    _task->data.parameters[10].value.ival = j2;
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
This subroutine calculates X*op(A^-1) where:
* X is MxN general matrix
* A is NxN upper/lower triangular/unitriangular matrix
* "op" may be identity transformation, transposition
Multiplication result replaces X.

INPUT PARAMETERS
    N   -   matrix size, N>=0
    M   -   matrix size, N>=0
    A       -   matrix, actial matrix is stored in A[I1:I1+N-1,J1:J1+N-1]
    I1      -   submatrix offset
    J1      -   submatrix offset
    IsUpper -   whether matrix is upper triangular
    IsUnit  -   whether matrix is unitriangular
    OpType  -   transformation type:
                * 0 - no transformation
                * 1 - transposition
    X   -   matrix, actial matrix is stored in X[I2:I2+M-1,J2:J2+N-1]
    I2  -   submatrix offset
    J2  -   submatrix offset

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

  -- ALGLIB routine --
     15.12.2009-22.01.2018
     Bochkanov Sergey
*************************************************************************/
void rmatrixrighttrsm(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Real    */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state)
{
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t s1;
    ae_int_t s2;
    ae_int_t tsa;
    ae_int_t tsb;
    ae_int_t tscur;


    tsa = matrixtilesizea(_state);
    tsb = matrixtilesizeb(_state);
    tscur = tsb;
    if( imax2(m, n, _state)<=tsb )
    {
        tscur = tsa;
    }
    ae_assert(tscur>=1, "RMatrixRightTRSM: integrity check failed", _state);
    
    /*
     * Upper level parallelization:
     * * decide whether it is feasible to activate multithreading
     * * perform optionally parallelized splits on M
     */
    if( m>=2*tsb&&ae_fp_greater_eq(rmul3((double)(m), (double)(n), (double)(n), _state),smpactivationlevel(_state)) )
    {
        if( _trypexec_rmatrixrighttrsm(m,n,a,i1,j1,isupper,isunit,optype,x,i2,j2, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    if( m>=2*tsb )
    {
        
        /*
         * Split X: X*A = (X1 X2)^T*A
         */
        tiledsplit(m, tsb, &s1, &s2, _state);
        ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_fp_greater_eq(rmul3((double)(imin2(s1, s2, _state)), (double)(n), (double)(n), _state),spawnlevel(_state)), _state);
        _spawn_rmatrixrighttrsm(s1, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _child_tasks, _smp_enabled, _state);
        rmatrixrighttrsm(s2, n, a, i1, j1, isupper, isunit, optype, x, i2+s1, j2, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Basecase: MKL or ALGLIB code
     */
    if( imax2(m, n, _state)<=tsb )
    {
        if( rmatrixrighttrsmmkl(m, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    if( imax2(m, n, _state)<=tsa )
    {
        ablas_rmatrixrighttrsm2(m, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Recursive subdivision
     */
    if( m>=n )
    {
        
        /*
         * Split X: X*A = (X1 X2)^T*A
         */
        tiledsplit(m, tscur, &s1, &s2, _state);
        rmatrixrighttrsm(s1, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
        rmatrixrighttrsm(s2, n, a, i1, j1, isupper, isunit, optype, x, i2+s1, j2, _state);
    }
    else
    {
        
        /*
         * Split A:
         *               (A1  A12)
         * X*op(A) = X*op(       )
         *               (     A2)
         *
         * Different variants depending on
         * IsUpper/OpType combinations
         */
        tiledsplit(n, tscur, &s1, &s2, _state);
        if( isupper&&optype==0 )
        {
            
            /*
             *                  (A1  A12)-1
             * X*A^-1 = (X1 X2)*(       )
             *                  (     A2)
             */
            rmatrixrighttrsm(m, s1, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
            rmatrixgemm(m, s2, s1, -1.0, x, i2, j2, 0, a, i1, j1+s1, 0, 1.0, x, i2, j2+s1, _state);
            rmatrixrighttrsm(m, s2, a, i1+s1, j1+s1, isupper, isunit, optype, x, i2, j2+s1, _state);
        }
        if( isupper&&optype!=0 )
        {
            
            /*
             *                  (A1'     )-1
             * X*A^-1 = (X1 X2)*(        )
             *                  (A12' A2')
             */
            rmatrixrighttrsm(m, s2, a, i1+s1, j1+s1, isupper, isunit, optype, x, i2, j2+s1, _state);
            rmatrixgemm(m, s1, s2, -1.0, x, i2, j2+s1, 0, a, i1, j1+s1, optype, 1.0, x, i2, j2, _state);
            rmatrixrighttrsm(m, s1, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
        }
        if( !isupper&&optype==0 )
        {
            
            /*
             *                  (A1     )-1
             * X*A^-1 = (X1 X2)*(       )
             *                  (A21  A2)
             */
            rmatrixrighttrsm(m, s2, a, i1+s1, j1+s1, isupper, isunit, optype, x, i2, j2+s1, _state);
            rmatrixgemm(m, s1, s2, -1.0, x, i2, j2+s1, 0, a, i1+s1, j1, 0, 1.0, x, i2, j2, _state);
            rmatrixrighttrsm(m, s1, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
        }
        if( !isupper&&optype!=0 )
        {
            
            /*
             *                  (A1' A21')-1
             * X*A^-1 = (X1 X2)*(        )
             *                  (     A2')
             */
            rmatrixrighttrsm(m, s1, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
            rmatrixgemm(m, s2, s1, -1.0, x, i2, j2, 0, a, i1+s1, j1, optype, 1.0, x, i2, j2+s1, _state);
            rmatrixrighttrsm(m, s2, a, i1+s1, j1+s1, isupper, isunit, optype, x, i2, j2+s1, _state);
        }
    }
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_rmatrixrighttrsm(ae_int_t m,
    ae_int_t n,
    /* Real    */ const ae_matrix* a,
    ae_int_t i1,
    ae_int_t j1,
    ae_bool isupper,
    ae_bool isunit,
    ae_int_t optype,
    /* Real    */ ae_matrix* x,
    ae_int_t i2,
    ae_int_t j2,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        rmatrixrighttrsm(m,n,a,i1,j1,isupper,isunit,optype,x,i2,j2, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_rmatrixrighttrsm;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.ival = m;
        _task->data.parameters[1].value.ival = n;
        _task->data.parameters[2].value.const_val = a;
        _task->data.parameters[3].value.ival = i1;
        _task->data.parameters[4].value.ival = j1;
        _task->data.parameters[5].value.bval = isupper;
        _task->data.parameters[6].value.bval = isunit;
        _task->data.parameters[7].value.ival = optype;
        _task->data.parameters[8].value.val = x;
        _task->data.parameters[9].value.ival = i2;
        _task->data.parameters[10].value.ival = j2;
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
void _task_rmatrixrighttrsm(ae_task_data *_data, ae_state *_state)
{
    ae_int_t m;
    ae_int_t n;
    const ae_matrix* a;
    ae_int_t i1;
    ae_int_t j1;
    ae_bool isupper;
    ae_bool isunit;
    ae_int_t optype;
    ae_matrix* x;
    ae_int_t i2;
    ae_int_t j2;
    m = _data->parameters[0].value.ival;
    n = _data->parameters[1].value.ival;
    a = (const ae_matrix*)_data->parameters[2].value.const_val;
    i1 = _data->parameters[3].value.ival;
    j1 = _data->parameters[4].value.ival;
    isupper = _data->parameters[5].value.bval;
    isunit = _data->parameters[6].value.bval;
    optype = _data->parameters[7].value.ival;
    x = (ae_matrix*)_data->parameters[8].value.val;
    i2 = _data->parameters[9].value.ival;
    j2 = _data->parameters[10].value.ival;
   ae_state_set_flags(_state, _data->flags);
   rmatrixrighttrsm(m,n,a,i1,j1,isupper,isunit,optype,x,i2,j2, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_rmatrixrighttrsm(ae_int_t m,
    ae_int_t n,
    /* Real    */ const ae_matrix* a,
    ae_int_t i1,
    ae_int_t j1,
    ae_bool isupper,
    ae_bool isunit,
    ae_int_t optype,
    /* Real    */ ae_matrix* x,
    ae_int_t i2,
    ae_int_t j2,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_rmatrixrighttrsm;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.ival = m;
    _task->data.parameters[1].value.ival = n;
    _task->data.parameters[2].value.const_val = a;
    _task->data.parameters[3].value.ival = i1;
    _task->data.parameters[4].value.ival = j1;
    _task->data.parameters[5].value.bval = isupper;
    _task->data.parameters[6].value.bval = isunit;
    _task->data.parameters[7].value.ival = optype;
    _task->data.parameters[8].value.val = x;
    _task->data.parameters[9].value.ival = i2;
    _task->data.parameters[10].value.ival = j2;
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
This subroutine calculates op(A^-1)*X where:
* X is MxN general matrix
* A is MxM upper/lower triangular/unitriangular matrix
* "op" may be identity transformation, transposition
Multiplication result replaces X.

INPUT PARAMETERS
    N   -   matrix size, N>=0
    M   -   matrix size, N>=0
    A       -   matrix, actial matrix is stored in A[I1:I1+M-1,J1:J1+M-1]
    I1      -   submatrix offset
    J1      -   submatrix offset
    IsUpper -   whether matrix is upper triangular
    IsUnit  -   whether matrix is unitriangular
    OpType  -   transformation type:
                * 0 - no transformation
                * 1 - transposition
    X   -   matrix, actial matrix is stored in X[I2:I2+M-1,J2:J2+N-1]
    I2  -   submatrix offset
    J2  -   submatrix offset

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

  -- ALGLIB routine --
     15.12.2009-22.01.2018
     Bochkanov Sergey
*************************************************************************/
void rmatrixlefttrsm(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Real    */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state)
{
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t s1;
    ae_int_t s2;
    ae_int_t tsa;
    ae_int_t tsb;
    ae_int_t tscur;


    tsa = matrixtilesizea(_state);
    tsb = matrixtilesizeb(_state);
    tscur = tsb;
    if( imax2(m, n, _state)<=tsb )
    {
        tscur = tsa;
    }
    ae_assert(tscur>=1, "RMatrixLeftTRSMRec: integrity check failed", _state);
    
    /*
     * Upper level parallelization:
     * * decide whether it is feasible to activate multithreading
     * * perform optionally parallelized splits on N
     */
    if( n>=2*tsb&&ae_fp_greater_eq(rmul3((double)(n), (double)(m), (double)(m), _state),smpactivationlevel(_state)) )
    {
        if( _trypexec_rmatrixlefttrsm(m,n,a,i1,j1,isupper,isunit,optype,x,i2,j2, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    if( n>=2*tsb )
    {
        tiledsplit(n, tscur, &s1, &s2, _state);
        ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_fp_greater_eq(rmul3((double)(imin2(s1, s2, _state)), (double)(m), (double)(m), _state),spawnlevel(_state)), _state);
        _spawn_rmatrixlefttrsm(m, s2, a, i1, j1, isupper, isunit, optype, x, i2, j2+s1, _child_tasks, _smp_enabled, _state);
        rmatrixlefttrsm(m, s1, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Basecase: MKL or ALGLIB code
     */
    if( imax2(m, n, _state)<=tsb )
    {
        if( rmatrixlefttrsmmkl(m, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    if( imax2(m, n, _state)<=tsa )
    {
        ablas_rmatrixlefttrsm2(m, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Recursive subdivision
     */
    if( n>=m )
    {
        
        /*
         * Split X: op(A)^-1*X = op(A)^-1*(X1 X2)
         */
        tiledsplit(n, tscur, &s1, &s2, _state);
        rmatrixlefttrsm(m, s1, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
        rmatrixlefttrsm(m, s2, a, i1, j1, isupper, isunit, optype, x, i2, j2+s1, _state);
    }
    else
    {
        
        /*
         * Split A
         */
        tiledsplit(m, tscur, &s1, &s2, _state);
        if( isupper&&optype==0 )
        {
            
            /*
             *           (A1  A12)-1  ( X1 )
             * A^-1*X* = (       )   *(    )
             *           (     A2)    ( X2 )
             */
            rmatrixlefttrsm(s2, n, a, i1+s1, j1+s1, isupper, isunit, optype, x, i2+s1, j2, _state);
            rmatrixgemm(s1, n, s2, -1.0, a, i1, j1+s1, 0, x, i2+s1, j2, 0, 1.0, x, i2, j2, _state);
            rmatrixlefttrsm(s1, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
        }
        if( isupper&&optype!=0 )
        {
            
            /*
             *          (A1'     )-1 ( X1 )
             * A^-1*X = (        )  *(    )
             *          (A12' A2')   ( X2 )
             */
            rmatrixlefttrsm(s1, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
            rmatrixgemm(s2, n, s1, -1.0, a, i1, j1+s1, optype, x, i2, j2, 0, 1.0, x, i2+s1, j2, _state);
            rmatrixlefttrsm(s2, n, a, i1+s1, j1+s1, isupper, isunit, optype, x, i2+s1, j2, _state);
        }
        if( !isupper&&optype==0 )
        {
            
            /*
             *          (A1     )-1 ( X1 )
             * A^-1*X = (       )  *(    )
             *          (A21  A2)   ( X2 )
             */
            rmatrixlefttrsm(s1, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
            rmatrixgemm(s2, n, s1, -1.0, a, i1+s1, j1, 0, x, i2, j2, 0, 1.0, x, i2+s1, j2, _state);
            rmatrixlefttrsm(s2, n, a, i1+s1, j1+s1, isupper, isunit, optype, x, i2+s1, j2, _state);
        }
        if( !isupper&&optype!=0 )
        {
            
            /*
             *          (A1' A21')-1 ( X1 )
             * A^-1*X = (        )  *(    )
             *          (     A2')   ( X2 )
             */
            rmatrixlefttrsm(s2, n, a, i1+s1, j1+s1, isupper, isunit, optype, x, i2+s1, j2, _state);
            rmatrixgemm(s1, n, s2, -1.0, a, i1+s1, j1, optype, x, i2+s1, j2, 0, 1.0, x, i2, j2, _state);
            rmatrixlefttrsm(s1, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state);
        }
    }
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_rmatrixlefttrsm(ae_int_t m,
    ae_int_t n,
    /* Real    */ const ae_matrix* a,
    ae_int_t i1,
    ae_int_t j1,
    ae_bool isupper,
    ae_bool isunit,
    ae_int_t optype,
    /* Real    */ ae_matrix* x,
    ae_int_t i2,
    ae_int_t j2,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        rmatrixlefttrsm(m,n,a,i1,j1,isupper,isunit,optype,x,i2,j2, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_rmatrixlefttrsm;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.ival = m;
        _task->data.parameters[1].value.ival = n;
        _task->data.parameters[2].value.const_val = a;
        _task->data.parameters[3].value.ival = i1;
        _task->data.parameters[4].value.ival = j1;
        _task->data.parameters[5].value.bval = isupper;
        _task->data.parameters[6].value.bval = isunit;
        _task->data.parameters[7].value.ival = optype;
        _task->data.parameters[8].value.val = x;
        _task->data.parameters[9].value.ival = i2;
        _task->data.parameters[10].value.ival = j2;
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
void _task_rmatrixlefttrsm(ae_task_data *_data, ae_state *_state)
{
    ae_int_t m;
    ae_int_t n;
    const ae_matrix* a;
    ae_int_t i1;
    ae_int_t j1;
    ae_bool isupper;
    ae_bool isunit;
    ae_int_t optype;
    ae_matrix* x;
    ae_int_t i2;
    ae_int_t j2;
    m = _data->parameters[0].value.ival;
    n = _data->parameters[1].value.ival;
    a = (const ae_matrix*)_data->parameters[2].value.const_val;
    i1 = _data->parameters[3].value.ival;
    j1 = _data->parameters[4].value.ival;
    isupper = _data->parameters[5].value.bval;
    isunit = _data->parameters[6].value.bval;
    optype = _data->parameters[7].value.ival;
    x = (ae_matrix*)_data->parameters[8].value.val;
    i2 = _data->parameters[9].value.ival;
    j2 = _data->parameters[10].value.ival;
   ae_state_set_flags(_state, _data->flags);
   rmatrixlefttrsm(m,n,a,i1,j1,isupper,isunit,optype,x,i2,j2, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_rmatrixlefttrsm(ae_int_t m,
    ae_int_t n,
    /* Real    */ const ae_matrix* a,
    ae_int_t i1,
    ae_int_t j1,
    ae_bool isupper,
    ae_bool isunit,
    ae_int_t optype,
    /* Real    */ ae_matrix* x,
    ae_int_t i2,
    ae_int_t j2,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_rmatrixlefttrsm;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.ival = m;
    _task->data.parameters[1].value.ival = n;
    _task->data.parameters[2].value.const_val = a;
    _task->data.parameters[3].value.ival = i1;
    _task->data.parameters[4].value.ival = j1;
    _task->data.parameters[5].value.bval = isupper;
    _task->data.parameters[6].value.bval = isunit;
    _task->data.parameters[7].value.ival = optype;
    _task->data.parameters[8].value.val = x;
    _task->data.parameters[9].value.ival = i2;
    _task->data.parameters[10].value.ival = j2;
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
This subroutine calculates  C=alpha*A*A^H+beta*C  or  C=alpha*A^H*A+beta*C
where:
* C is NxN Hermitian matrix given by its upper/lower triangle
* A is NxK matrix when A*A^H is calculated, KxN matrix otherwise

Additional info:
* multiplication result replaces C. If Beta=0, C elements are not used in
  calculations (not multiplied by zero - just not referenced)
* if Alpha=0, A is not used (not multiplied by zero - just not referenced)
* if both Beta and Alpha are zero, C is filled by zeros.

INPUT PARAMETERS
    N       -   matrix size, N>=0
    K       -   matrix size, K>=0
    Alpha   -   coefficient
    A       -   matrix
    IA      -   submatrix offset (row index)
    JA      -   submatrix offset (column index)
    OpTypeA -   multiplication type:
                * 0 - A*A^H is calculated
                * 2 - A^H*A is calculated
    Beta    -   coefficient
    C       -   preallocated input/output matrix
    IC      -   submatrix offset (row index)
    JC      -   submatrix offset (column index)
    IsUpper -   whether upper or lower triangle of C is updated;
                this function updates only one half of C, leaving
                other half unchanged (not referenced at all).

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

  -- ALGLIB routine --
     16.12.2009-22.01.2018
     Bochkanov Sergey
*************************************************************************/
void cmatrixherk(ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     double beta,
     /* Complex */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_bool isupper,
     ae_state *_state)
{
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t s1;
    ae_int_t s2;
    ae_int_t tsa;
    ae_int_t tsb;
    ae_int_t tscur;


    tsa = matrixtilesizea(_state)/2;
    tsb = matrixtilesizeb(_state);
    tscur = tsb;
    if( imax2(n, k, _state)<=tsb )
    {
        tscur = tsa;
    }
    ae_assert(tscur>=1, "CMatrixHERK: integrity check failed", _state);
    
    /*
     * Decide whether it is feasible to activate multithreading
     */
    if( n>=2*tsb&&ae_fp_greater_eq((double)8*rmul3((double)(k), (double)(n), (double)(n), _state)/(double)2,smpactivationlevel(_state)) )
    {
        if( _trypexec_cmatrixherk(n,k,alpha,a,ia,ja,optypea,beta,c,ic,jc,isupper, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    
    /*
     * Use MKL or ALGLIB basecase code
     */
    if( imax2(n, k, _state)<=tsb )
    {
        if( cmatrixherkmkl(n, k, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    if( imax2(n, k, _state)<=tsa )
    {
        ablas_cmatrixherk2(n, k, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Recursive division of the problem
     */
    if( k>=n )
    {
        
        /*
         * Split K
         */
        tiledsplit(k, tscur, &s1, &s2, _state);
        if( optypea==0 )
        {
            cmatrixherk(n, s1, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper, _state);
            cmatrixherk(n, s2, alpha, a, ia, ja+s1, optypea, 1.0, c, ic, jc, isupper, _state);
        }
        else
        {
            cmatrixherk(n, s1, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper, _state);
            cmatrixherk(n, s2, alpha, a, ia+s1, ja, optypea, 1.0, c, ic, jc, isupper, _state);
        }
    }
    else
    {
        
        /*
         * Split N
         */
        tiledsplit(n, tscur, &s1, &s2, _state);
        ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_fp_greater_eq((double)4*rmul3((double)(imin2(s1, s2, _state)), (double)n/(double)2, (double)n/(double)2, _state),spawnlevel(_state)), _state);
        if( optypea==0&&isupper )
        {
            _spawn_cmatrixherk(s1, k, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper, _child_tasks, _smp_enabled, _state);
            _spawn_cmatrixherk(s2, k, alpha, a, ia+s1, ja, optypea, beta, c, ic+s1, jc+s1, isupper, _child_tasks, _smp_enabled, _state);
            cmatrixgemm(s1, s2, k, ae_complex_from_d(alpha), a, ia, ja, 0, a, ia+s1, ja, 2, ae_complex_from_d(beta), c, ic, jc+s1, _state);
        }
        if( optypea==0&&!isupper )
        {
            _spawn_cmatrixherk(s1, k, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper, _child_tasks, _smp_enabled, _state);
            _spawn_cmatrixherk(s2, k, alpha, a, ia+s1, ja, optypea, beta, c, ic+s1, jc+s1, isupper, _child_tasks, _smp_enabled, _state);
            cmatrixgemm(s2, s1, k, ae_complex_from_d(alpha), a, ia+s1, ja, 0, a, ia, ja, 2, ae_complex_from_d(beta), c, ic+s1, jc, _state);
        }
        if( optypea!=0&&isupper )
        {
            _spawn_cmatrixherk(s1, k, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper, _child_tasks, _smp_enabled, _state);
            _spawn_cmatrixherk(s2, k, alpha, a, ia, ja+s1, optypea, beta, c, ic+s1, jc+s1, isupper, _child_tasks, _smp_enabled, _state);
            cmatrixgemm(s1, s2, k, ae_complex_from_d(alpha), a, ia, ja, 2, a, ia, ja+s1, 0, ae_complex_from_d(beta), c, ic, jc+s1, _state);
        }
        if( optypea!=0&&!isupper )
        {
            _spawn_cmatrixherk(s1, k, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper, _child_tasks, _smp_enabled, _state);
            _spawn_cmatrixherk(s2, k, alpha, a, ia, ja+s1, optypea, beta, c, ic+s1, jc+s1, isupper, _child_tasks, _smp_enabled, _state);
            cmatrixgemm(s2, s1, k, ae_complex_from_d(alpha), a, ia, ja+s1, 2, a, ia, ja, 0, ae_complex_from_d(beta), c, ic+s1, jc, _state);
        }
    }
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_cmatrixherk(ae_int_t n,
    ae_int_t k,
    double alpha,
    /* Complex */ const ae_matrix* a,
    ae_int_t ia,
    ae_int_t ja,
    ae_int_t optypea,
    double beta,
    /* Complex */ ae_matrix* c,
    ae_int_t ic,
    ae_int_t jc,
    ae_bool isupper,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        cmatrixherk(n,k,alpha,a,ia,ja,optypea,beta,c,ic,jc,isupper, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_cmatrixherk;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.ival = n;
        _task->data.parameters[1].value.ival = k;
        _task->data.parameters[2].value.dval = alpha;
        _task->data.parameters[3].value.const_val = a;
        _task->data.parameters[4].value.ival = ia;
        _task->data.parameters[5].value.ival = ja;
        _task->data.parameters[6].value.ival = optypea;
        _task->data.parameters[7].value.dval = beta;
        _task->data.parameters[8].value.val = c;
        _task->data.parameters[9].value.ival = ic;
        _task->data.parameters[10].value.ival = jc;
        _task->data.parameters[11].value.bval = isupper;
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
void _task_cmatrixherk(ae_task_data *_data, ae_state *_state)
{
    ae_int_t n;
    ae_int_t k;
    double alpha;
    const ae_matrix* a;
    ae_int_t ia;
    ae_int_t ja;
    ae_int_t optypea;
    double beta;
    ae_matrix* c;
    ae_int_t ic;
    ae_int_t jc;
    ae_bool isupper;
    n = _data->parameters[0].value.ival;
    k = _data->parameters[1].value.ival;
    alpha = _data->parameters[2].value.dval;
    a = (const ae_matrix*)_data->parameters[3].value.const_val;
    ia = _data->parameters[4].value.ival;
    ja = _data->parameters[5].value.ival;
    optypea = _data->parameters[6].value.ival;
    beta = _data->parameters[7].value.dval;
    c = (ae_matrix*)_data->parameters[8].value.val;
    ic = _data->parameters[9].value.ival;
    jc = _data->parameters[10].value.ival;
    isupper = _data->parameters[11].value.bval;
   ae_state_set_flags(_state, _data->flags);
   cmatrixherk(n,k,alpha,a,ia,ja,optypea,beta,c,ic,jc,isupper, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_cmatrixherk(ae_int_t n,
    ae_int_t k,
    double alpha,
    /* Complex */ const ae_matrix* a,
    ae_int_t ia,
    ae_int_t ja,
    ae_int_t optypea,
    double beta,
    /* Complex */ ae_matrix* c,
    ae_int_t ic,
    ae_int_t jc,
    ae_bool isupper,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_cmatrixherk;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.ival = n;
    _task->data.parameters[1].value.ival = k;
    _task->data.parameters[2].value.dval = alpha;
    _task->data.parameters[3].value.const_val = a;
    _task->data.parameters[4].value.ival = ia;
    _task->data.parameters[5].value.ival = ja;
    _task->data.parameters[6].value.ival = optypea;
    _task->data.parameters[7].value.dval = beta;
    _task->data.parameters[8].value.val = c;
    _task->data.parameters[9].value.ival = ic;
    _task->data.parameters[10].value.ival = jc;
    _task->data.parameters[11].value.bval = isupper;
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
This subroutine calculates  C=alpha*A*A^T+beta*C  or  C=alpha*A^T*A+beta*C
where:
* C is NxN symmetric matrix given by its upper/lower triangle
* A is NxK matrix when A*A^T is calculated, KxN matrix otherwise

Additional info:
* multiplication result replaces C. If Beta=0, C elements are not used in
  calculations (not multiplied by zero - just not referenced)
* if Alpha=0, A is not used (not multiplied by zero - just not referenced)
* if both Beta and Alpha are zero, C is filled by zeros.

INPUT PARAMETERS
    N       -   matrix size, N>=0
    K       -   matrix size, K>=0
    Alpha   -   coefficient
    A       -   matrix
    IA      -   submatrix offset (row index)
    JA      -   submatrix offset (column index)
    OpTypeA -   multiplication type:
                * 0 - A*A^T is calculated
                * 2 - A^T*A is calculated
    Beta    -   coefficient
    C       -   preallocated input/output matrix
    IC      -   submatrix offset (row index)
    JC      -   submatrix offset (column index)
    IsUpper -   whether C is upper triangular or lower triangular

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

  -- ALGLIB routine --
     16.12.2009-22.01.2018
     Bochkanov Sergey
*************************************************************************/
void rmatrixsyrk(ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     double beta,
     /* Real    */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_bool isupper,
     ae_state *_state)
{
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t s1;
    ae_int_t s2;
    ae_int_t tsa;
    ae_int_t tsb;
    ae_int_t tscur;


    tsa = matrixtilesizea(_state);
    tsb = matrixtilesizeb(_state);
    tscur = tsb;
    if( imax2(n, k, _state)<=tsb )
    {
        tscur = tsa;
    }
    ae_assert(tscur>=1, "RMatrixSYRK: integrity check failed", _state);
    
    /*
     * Decide whether it is feasible to activate multithreading
     */
    if( n>=2*tsb&&ae_fp_greater_eq((double)2*rmul3((double)(k), (double)(n), (double)(n), _state)/(double)2,smpactivationlevel(_state)) )
    {
        if( _trypexec_rmatrixsyrk(n,k,alpha,a,ia,ja,optypea,beta,c,ic,jc,isupper, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    
    /*
     * Use MKL or generic basecase code
     */
    if( imax2(n, k, _state)<=tsb )
    {
        if( rmatrixsyrkmkl(n, k, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    if( imax2(n, k, _state)<=tsa )
    {
        ablas_rmatrixsyrk2(n, k, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Recursive subdivision of the problem
     */
    if( k>=n )
    {
        
        /*
         * Split K
         */
        tiledsplit(k, tscur, &s1, &s2, _state);
        if( optypea==0 )
        {
            rmatrixsyrk(n, s1, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper, _state);
            rmatrixsyrk(n, s2, alpha, a, ia, ja+s1, optypea, 1.0, c, ic, jc, isupper, _state);
        }
        else
        {
            rmatrixsyrk(n, s1, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper, _state);
            rmatrixsyrk(n, s2, alpha, a, ia+s1, ja, optypea, 1.0, c, ic, jc, isupper, _state);
        }
    }
    else
    {
        
        /*
         * Split N
         */
        tiledsplit(n, tscur, &s1, &s2, _state);
        ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_fp_greater_eq(rmul3((double)(imin2(s1, s2, _state)), (double)n/(double)2, (double)n/(double)2, _state),spawnlevel(_state)), _state);
        if( optypea==0&&isupper )
        {
            _spawn_rmatrixsyrk(s1, k, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper, _child_tasks, _smp_enabled, _state);
            _spawn_rmatrixsyrk(s2, k, alpha, a, ia+s1, ja, optypea, beta, c, ic+s1, jc+s1, isupper, _child_tasks, _smp_enabled, _state);
            rmatrixgemm(s1, s2, k, alpha, a, ia, ja, 0, a, ia+s1, ja, 1, beta, c, ic, jc+s1, _state);
        }
        if( optypea==0&&!isupper )
        {
            _spawn_rmatrixsyrk(s1, k, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper, _child_tasks, _smp_enabled, _state);
            _spawn_rmatrixsyrk(s2, k, alpha, a, ia+s1, ja, optypea, beta, c, ic+s1, jc+s1, isupper, _child_tasks, _smp_enabled, _state);
            rmatrixgemm(s2, s1, k, alpha, a, ia+s1, ja, 0, a, ia, ja, 1, beta, c, ic+s1, jc, _state);
        }
        if( optypea!=0&&isupper )
        {
            _spawn_rmatrixsyrk(s1, k, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper, _child_tasks, _smp_enabled, _state);
            _spawn_rmatrixsyrk(s2, k, alpha, a, ia, ja+s1, optypea, beta, c, ic+s1, jc+s1, isupper, _child_tasks, _smp_enabled, _state);
            rmatrixgemm(s1, s2, k, alpha, a, ia, ja, 1, a, ia, ja+s1, 0, beta, c, ic, jc+s1, _state);
        }
        if( optypea!=0&&!isupper )
        {
            _spawn_rmatrixsyrk(s1, k, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper, _child_tasks, _smp_enabled, _state);
            _spawn_rmatrixsyrk(s2, k, alpha, a, ia, ja+s1, optypea, beta, c, ic+s1, jc+s1, isupper, _child_tasks, _smp_enabled, _state);
            rmatrixgemm(s2, s1, k, alpha, a, ia, ja+s1, 1, a, ia, ja, 0, beta, c, ic+s1, jc, _state);
        }
    }
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_rmatrixsyrk(ae_int_t n,
    ae_int_t k,
    double alpha,
    /* Real    */ const ae_matrix* a,
    ae_int_t ia,
    ae_int_t ja,
    ae_int_t optypea,
    double beta,
    /* Real    */ ae_matrix* c,
    ae_int_t ic,
    ae_int_t jc,
    ae_bool isupper,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        rmatrixsyrk(n,k,alpha,a,ia,ja,optypea,beta,c,ic,jc,isupper, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_rmatrixsyrk;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.ival = n;
        _task->data.parameters[1].value.ival = k;
        _task->data.parameters[2].value.dval = alpha;
        _task->data.parameters[3].value.const_val = a;
        _task->data.parameters[4].value.ival = ia;
        _task->data.parameters[5].value.ival = ja;
        _task->data.parameters[6].value.ival = optypea;
        _task->data.parameters[7].value.dval = beta;
        _task->data.parameters[8].value.val = c;
        _task->data.parameters[9].value.ival = ic;
        _task->data.parameters[10].value.ival = jc;
        _task->data.parameters[11].value.bval = isupper;
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
void _task_rmatrixsyrk(ae_task_data *_data, ae_state *_state)
{
    ae_int_t n;
    ae_int_t k;
    double alpha;
    const ae_matrix* a;
    ae_int_t ia;
    ae_int_t ja;
    ae_int_t optypea;
    double beta;
    ae_matrix* c;
    ae_int_t ic;
    ae_int_t jc;
    ae_bool isupper;
    n = _data->parameters[0].value.ival;
    k = _data->parameters[1].value.ival;
    alpha = _data->parameters[2].value.dval;
    a = (const ae_matrix*)_data->parameters[3].value.const_val;
    ia = _data->parameters[4].value.ival;
    ja = _data->parameters[5].value.ival;
    optypea = _data->parameters[6].value.ival;
    beta = _data->parameters[7].value.dval;
    c = (ae_matrix*)_data->parameters[8].value.val;
    ic = _data->parameters[9].value.ival;
    jc = _data->parameters[10].value.ival;
    isupper = _data->parameters[11].value.bval;
   ae_state_set_flags(_state, _data->flags);
   rmatrixsyrk(n,k,alpha,a,ia,ja,optypea,beta,c,ic,jc,isupper, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_rmatrixsyrk(ae_int_t n,
    ae_int_t k,
    double alpha,
    /* Real    */ const ae_matrix* a,
    ae_int_t ia,
    ae_int_t ja,
    ae_int_t optypea,
    double beta,
    /* Real    */ ae_matrix* c,
    ae_int_t ic,
    ae_int_t jc,
    ae_bool isupper,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_rmatrixsyrk;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.ival = n;
    _task->data.parameters[1].value.ival = k;
    _task->data.parameters[2].value.dval = alpha;
    _task->data.parameters[3].value.const_val = a;
    _task->data.parameters[4].value.ival = ia;
    _task->data.parameters[5].value.ival = ja;
    _task->data.parameters[6].value.ival = optypea;
    _task->data.parameters[7].value.dval = beta;
    _task->data.parameters[8].value.val = c;
    _task->data.parameters[9].value.ival = ic;
    _task->data.parameters[10].value.ival = jc;
    _task->data.parameters[11].value.bval = isupper;
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
This subroutine calculates C = alpha*op1(A)*op2(B) +beta*C where:
* C is MxN general matrix
* op1(A) is MxK matrix
* op2(B) is KxN matrix
* "op" may be identity transformation, transposition, conjugate transposition

Additional info:
* cache-oblivious algorithm is used.
* multiplication result replaces C. If Beta=0, C elements are not used in
  calculations (not multiplied by zero - just not referenced)
* if Alpha=0, A is not used (not multiplied by zero - just not referenced)
* if both Beta and Alpha are zero, C is filled by zeros.

IMPORTANT:

This function does NOT preallocate output matrix C, it MUST be preallocated
by caller prior to calling this function. In case C does not have  enough
space to store result, exception will be generated.

INPUT PARAMETERS
    M       -   matrix size, M>0
    N       -   matrix size, N>0
    K       -   matrix size, K>0
    Alpha   -   coefficient
    A       -   matrix
    IA      -   submatrix offset
    JA      -   submatrix offset
    OpTypeA -   transformation type:
                * 0 - no transformation
                * 1 - transposition
                * 2 - conjugate transposition
    B       -   matrix
    IB      -   submatrix offset
    JB      -   submatrix offset
    OpTypeB -   transformation type:
                * 0 - no transformation
                * 1 - transposition
                * 2 - conjugate transposition
    Beta    -   coefficient
    C       -   matrix (PREALLOCATED, large enough to store result)
    IC      -   submatrix offset
    JC      -   submatrix offset

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

  -- ALGLIB routine --
     2009-2019
     Bochkanov Sergey
*************************************************************************/
void cmatrixgemm(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     ae_complex alpha,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     /* Complex */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_int_t optypeb,
     ae_complex beta,
     /* Complex */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state)
{
    ae_int_t ts;


    ts = matrixtilesizeb(_state);
    
    /*
     * Check input sizes for correctness
     */
    ae_assert((optypea==0||optypea==1)||optypea==2, "CMatrixGEMM: incorrect OpTypeA (must be 0 or 1 or 2)", _state);
    ae_assert((optypeb==0||optypeb==1)||optypeb==2, "CMatrixGEMM: incorrect OpTypeB (must be 0 or 1 or 2)", _state);
    ae_assert(ic+m<=c->rows, "CMatrixGEMM: incorect size of output matrix C", _state);
    ae_assert(jc+n<=c->cols, "CMatrixGEMM: incorect size of output matrix C", _state);
    
    /*
     * Decide whether it is feasible to activate multithreading
     */
    if( (m>=2*ts||n>=2*ts)&&ae_fp_greater_eq((double)8*rmul3((double)(m), (double)(n), (double)(k), _state),smpactivationlevel(_state)) )
    {
        if( _trypexec_cmatrixgemm(m,n,k,alpha,a,ia,ja,optypea,b,ib,jb,optypeb,beta,c,ic,jc, _state) )
        {
            return;
        }
    }
    
    /*
     * Start actual work
     */
    ablas_cmatrixgemmrec(m, n, k, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_cmatrixgemm(ae_int_t m,
    ae_int_t n,
    ae_int_t k,
    ae_complex alpha,
    /* Complex */ const ae_matrix* a,
    ae_int_t ia,
    ae_int_t ja,
    ae_int_t optypea,
    /* Complex */ const ae_matrix* b,
    ae_int_t ib,
    ae_int_t jb,
    ae_int_t optypeb,
    ae_complex beta,
    /* Complex */ ae_matrix* c,
    ae_int_t ic,
    ae_int_t jc,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        cmatrixgemm(m,n,k,alpha,a,ia,ja,optypea,b,ib,jb,optypeb,beta,c,ic,jc, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_cmatrixgemm;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.ival = m;
        _task->data.parameters[1].value.ival = n;
        _task->data.parameters[2].value.ival = k;
        _task->data.parameters[3].value.cval = alpha;
        _task->data.parameters[4].value.const_val = a;
        _task->data.parameters[5].value.ival = ia;
        _task->data.parameters[6].value.ival = ja;
        _task->data.parameters[7].value.ival = optypea;
        _task->data.parameters[8].value.const_val = b;
        _task->data.parameters[9].value.ival = ib;
        _task->data.parameters[10].value.ival = jb;
        _task->data.parameters[11].value.ival = optypeb;
        _task->data.parameters[12].value.cval = beta;
        _task->data.parameters[13].value.val = c;
        _task->data.parameters[14].value.ival = ic;
        _task->data.parameters[15].value.ival = jc;
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
void _task_cmatrixgemm(ae_task_data *_data, ae_state *_state)
{
    ae_int_t m;
    ae_int_t n;
    ae_int_t k;
    ae_complex alpha;
    const ae_matrix* a;
    ae_int_t ia;
    ae_int_t ja;
    ae_int_t optypea;
    const ae_matrix* b;
    ae_int_t ib;
    ae_int_t jb;
    ae_int_t optypeb;
    ae_complex beta;
    ae_matrix* c;
    ae_int_t ic;
    ae_int_t jc;
    m = _data->parameters[0].value.ival;
    n = _data->parameters[1].value.ival;
    k = _data->parameters[2].value.ival;
    alpha = _data->parameters[3].value.cval;
    a = (const ae_matrix*)_data->parameters[4].value.const_val;
    ia = _data->parameters[5].value.ival;
    ja = _data->parameters[6].value.ival;
    optypea = _data->parameters[7].value.ival;
    b = (const ae_matrix*)_data->parameters[8].value.const_val;
    ib = _data->parameters[9].value.ival;
    jb = _data->parameters[10].value.ival;
    optypeb = _data->parameters[11].value.ival;
    beta = _data->parameters[12].value.cval;
    c = (ae_matrix*)_data->parameters[13].value.val;
    ic = _data->parameters[14].value.ival;
    jc = _data->parameters[15].value.ival;
   ae_state_set_flags(_state, _data->flags);
   cmatrixgemm(m,n,k,alpha,a,ia,ja,optypea,b,ib,jb,optypeb,beta,c,ic,jc, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_cmatrixgemm(ae_int_t m,
    ae_int_t n,
    ae_int_t k,
    ae_complex alpha,
    /* Complex */ const ae_matrix* a,
    ae_int_t ia,
    ae_int_t ja,
    ae_int_t optypea,
    /* Complex */ const ae_matrix* b,
    ae_int_t ib,
    ae_int_t jb,
    ae_int_t optypeb,
    ae_complex beta,
    /* Complex */ ae_matrix* c,
    ae_int_t ic,
    ae_int_t jc,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_cmatrixgemm;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.ival = m;
    _task->data.parameters[1].value.ival = n;
    _task->data.parameters[2].value.ival = k;
    _task->data.parameters[3].value.cval = alpha;
    _task->data.parameters[4].value.const_val = a;
    _task->data.parameters[5].value.ival = ia;
    _task->data.parameters[6].value.ival = ja;
    _task->data.parameters[7].value.ival = optypea;
    _task->data.parameters[8].value.const_val = b;
    _task->data.parameters[9].value.ival = ib;
    _task->data.parameters[10].value.ival = jb;
    _task->data.parameters[11].value.ival = optypeb;
    _task->data.parameters[12].value.cval = beta;
    _task->data.parameters[13].value.val = c;
    _task->data.parameters[14].value.ival = ic;
    _task->data.parameters[15].value.ival = jc;
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
This subroutine calculates C = alpha*op1(A)*op2(B) +beta*C where:
* C is MxN general matrix
* op1(A) is MxK matrix
* op2(B) is KxN matrix
* "op" may be identity transformation, transposition

Additional info:
* cache-oblivious algorithm is used.
* multiplication result replaces C. If Beta=0, C elements are not used in
  calculations (not multiplied by zero - just not referenced)
* if Alpha=0, A is not used (not multiplied by zero - just not referenced)
* if both Beta and Alpha are zero, C is filled by zeros.

IMPORTANT:

This function does NOT preallocate output matrix C, it MUST be preallocated
by caller prior to calling this function. In case C does not have  enough
space to store result, exception will be generated.

INPUT PARAMETERS
    M       -   matrix size, M>0
    N       -   matrix size, N>0
    K       -   matrix size, K>0
    Alpha   -   coefficient
    A       -   matrix
    IA      -   submatrix offset
    JA      -   submatrix offset
    OpTypeA -   transformation type:
                * 0 - no transformation
                * 1 - transposition
    B       -   matrix
    IB      -   submatrix offset
    JB      -   submatrix offset
    OpTypeB -   transformation type:
                * 0 - no transformation
                * 1 - transposition
    Beta    -   coefficient
    C       -   PREALLOCATED output matrix, large enough to store result
    IC      -   submatrix offset
    JC      -   submatrix offset

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

  -- ALGLIB routine --
     2009-2019
     Bochkanov Sergey
*************************************************************************/
void rmatrixgemm(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_int_t optypeb,
     double beta,
     /* Real    */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state)
{
    ae_int_t ts;


    ts = matrixtilesizeb(_state);
    
    /*
     * Check input sizes for correctness
     */
    ae_assert(optypea==0||optypea==1, "RMatrixGEMM: incorrect OpTypeA (must be 0 or 1)", _state);
    ae_assert(optypeb==0||optypeb==1, "RMatrixGEMM: incorrect OpTypeB (must be 0 or 1)", _state);
    ae_assert(ic+m<=c->rows, "RMatrixGEMM: incorect size of output matrix C", _state);
    ae_assert(jc+n<=c->cols, "RMatrixGEMM: incorect size of output matrix C", _state);
    
    /*
     * Decide whether it is feasible to activate multithreading
     */
    if( (m>=2*ts||n>=2*ts)&&ae_fp_greater_eq((double)2*rmul3((double)(m), (double)(n), (double)(k), _state),smpactivationlevel(_state)) )
    {
        if( _trypexec_rmatrixgemm(m,n,k,alpha,a,ia,ja,optypea,b,ib,jb,optypeb,beta,c,ic,jc, _state) )
        {
            return;
        }
    }
    
    /*
     * Start actual work
     */
    ablas_rmatrixgemmrec(m, n, k, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_rmatrixgemm(ae_int_t m,
    ae_int_t n,
    ae_int_t k,
    double alpha,
    /* Real    */ const ae_matrix* a,
    ae_int_t ia,
    ae_int_t ja,
    ae_int_t optypea,
    /* Real    */ const ae_matrix* b,
    ae_int_t ib,
    ae_int_t jb,
    ae_int_t optypeb,
    double beta,
    /* Real    */ ae_matrix* c,
    ae_int_t ic,
    ae_int_t jc,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        rmatrixgemm(m,n,k,alpha,a,ia,ja,optypea,b,ib,jb,optypeb,beta,c,ic,jc, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_rmatrixgemm;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.ival = m;
        _task->data.parameters[1].value.ival = n;
        _task->data.parameters[2].value.ival = k;
        _task->data.parameters[3].value.dval = alpha;
        _task->data.parameters[4].value.const_val = a;
        _task->data.parameters[5].value.ival = ia;
        _task->data.parameters[6].value.ival = ja;
        _task->data.parameters[7].value.ival = optypea;
        _task->data.parameters[8].value.const_val = b;
        _task->data.parameters[9].value.ival = ib;
        _task->data.parameters[10].value.ival = jb;
        _task->data.parameters[11].value.ival = optypeb;
        _task->data.parameters[12].value.dval = beta;
        _task->data.parameters[13].value.val = c;
        _task->data.parameters[14].value.ival = ic;
        _task->data.parameters[15].value.ival = jc;
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
void _task_rmatrixgemm(ae_task_data *_data, ae_state *_state)
{
    ae_int_t m;
    ae_int_t n;
    ae_int_t k;
    double alpha;
    const ae_matrix* a;
    ae_int_t ia;
    ae_int_t ja;
    ae_int_t optypea;
    const ae_matrix* b;
    ae_int_t ib;
    ae_int_t jb;
    ae_int_t optypeb;
    double beta;
    ae_matrix* c;
    ae_int_t ic;
    ae_int_t jc;
    m = _data->parameters[0].value.ival;
    n = _data->parameters[1].value.ival;
    k = _data->parameters[2].value.ival;
    alpha = _data->parameters[3].value.dval;
    a = (const ae_matrix*)_data->parameters[4].value.const_val;
    ia = _data->parameters[5].value.ival;
    ja = _data->parameters[6].value.ival;
    optypea = _data->parameters[7].value.ival;
    b = (const ae_matrix*)_data->parameters[8].value.const_val;
    ib = _data->parameters[9].value.ival;
    jb = _data->parameters[10].value.ival;
    optypeb = _data->parameters[11].value.ival;
    beta = _data->parameters[12].value.dval;
    c = (ae_matrix*)_data->parameters[13].value.val;
    ic = _data->parameters[14].value.ival;
    jc = _data->parameters[15].value.ival;
   ae_state_set_flags(_state, _data->flags);
   rmatrixgemm(m,n,k,alpha,a,ia,ja,optypea,b,ib,jb,optypeb,beta,c,ic,jc, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_rmatrixgemm(ae_int_t m,
    ae_int_t n,
    ae_int_t k,
    double alpha,
    /* Real    */ const ae_matrix* a,
    ae_int_t ia,
    ae_int_t ja,
    ae_int_t optypea,
    /* Real    */ const ae_matrix* b,
    ae_int_t ib,
    ae_int_t jb,
    ae_int_t optypeb,
    double beta,
    /* Real    */ ae_matrix* c,
    ae_int_t ic,
    ae_int_t jc,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_rmatrixgemm;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.ival = m;
    _task->data.parameters[1].value.ival = n;
    _task->data.parameters[2].value.ival = k;
    _task->data.parameters[3].value.dval = alpha;
    _task->data.parameters[4].value.const_val = a;
    _task->data.parameters[5].value.ival = ia;
    _task->data.parameters[6].value.ival = ja;
    _task->data.parameters[7].value.ival = optypea;
    _task->data.parameters[8].value.const_val = b;
    _task->data.parameters[9].value.ival = ib;
    _task->data.parameters[10].value.ival = jb;
    _task->data.parameters[11].value.ival = optypeb;
    _task->data.parameters[12].value.dval = beta;
    _task->data.parameters[13].value.val = c;
    _task->data.parameters[14].value.ival = ic;
    _task->data.parameters[15].value.ival = jc;
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
This subroutine is an older version of CMatrixHERK(), one with wrong  name
(it is HErmitian update, not SYmmetric). It  is  left  here  for  backward
compatibility.

  -- ALGLIB routine --
     16.12.2009
     Bochkanov Sergey
*************************************************************************/
void cmatrixsyrk(ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     double beta,
     /* Complex */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_bool isupper,
     ae_state *_state)
{


    cmatrixherk(n, k, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper, _state);
}


/*************************************************************************
Performs one step  of stable Gram-Schmidt  process  on  vector  X[]  using
set of orthonormal rows Q[].

INPUT PARAMETERS:
    Q       -   array[M,N], matrix with orthonormal rows
    M, N    -   rows/cols
    X       -   array[N], vector to process
    NeedQX  -   whether we need QX or not 

OUTPUT PARAMETERS:
    X       -   stores X - Q'*(Q*X)
    QX      -   if NeedQX is True, array[M] filled with elements  of  Q*X,
                reallocated if length is less than M.
                Ignored otherwise.
                
NOTE: this function silently exits when M=0, doing nothing

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rowwisegramschmidt(/* Real    */ const ae_matrix* q,
     ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* qx,
     ae_bool needqx,
     ae_state *_state)
{
    ae_int_t i;
    double v;


    if( m==0 )
    {
        return;
    }
    if( needqx )
    {
        rvectorsetlengthatleast(qx, m, _state);
    }
    for(i=0; i<=m-1; i++)
    {
        v = rdotvr(n, x, q, i, _state);
        raddrv(n, -v, q, i, x, _state);
        if( needqx )
        {
            qx->ptr.p_double[i] = v;
        }
    }
}


/*************************************************************************
Complex ABLASSplitLength

  -- ALGLIB routine --
     15.12.2009
     Bochkanov Sergey
*************************************************************************/
static void ablas_ablasinternalsplitlength(ae_int_t n,
     ae_int_t nb,
     ae_int_t* n1,
     ae_int_t* n2,
     ae_state *_state)
{
    ae_int_t r;

    *n1 = 0;
    *n2 = 0;

    if( n<=nb )
    {
        
        /*
         * Block size, no further splitting
         */
        *n1 = n;
        *n2 = 0;
    }
    else
    {
        
        /*
         * Greater than block size
         */
        if( n%nb!=0 )
        {
            
            /*
             * Split remainder
             */
            *n2 = n%nb;
            *n1 = n-(*n2);
        }
        else
        {
            
            /*
             * Split on block boundaries
             */
            *n2 = n/2;
            *n1 = n-(*n2);
            if( *n1%nb==0 )
            {
                return;
            }
            r = nb-*n1%nb;
            *n1 = *n1+r;
            *n2 = *n2-r;
        }
    }
}


/*************************************************************************
Level 2 variant of CMatrixRightTRSM
*************************************************************************/
static void ablas_cmatrixrighttrsm2(ae_int_t m,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Complex */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_complex vc;
    ae_complex vd;


    
    /*
     * Special case
     */
    if( n*m==0 )
    {
        return;
    }
    
    /*
     * Try to call fast TRSM
     */
    if( cmatrixrighttrsmf(m, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state) )
    {
        return;
    }
    
    /*
     * General case
     */
    if( isupper )
    {
        
        /*
         * Upper triangular matrix
         */
        if( optype==0 )
        {
            
            /*
             * X*A^(-1)
             */
            for(i=0; i<=m-1; i++)
            {
                for(j=0; j<=n-1; j++)
                {
                    if( isunit )
                    {
                        vd = ae_complex_from_i(1);
                    }
                    else
                    {
                        vd = a->ptr.pp_complex[i1+j][j1+j];
                    }
                    x->ptr.pp_complex[i2+i][j2+j] = ae_c_div(x->ptr.pp_complex[i2+i][j2+j],vd);
                    if( j<n-1 )
                    {
                        vc = x->ptr.pp_complex[i2+i][j2+j];
                        ae_v_csubc(&x->ptr.pp_complex[i2+i][j2+j+1], 1, &a->ptr.pp_complex[i1+j][j1+j+1], 1, "N", ae_v_len(j2+j+1,j2+n-1), vc);
                    }
                }
            }
            return;
        }
        if( optype==1 )
        {
            
            /*
             * X*A^(-T)
             */
            for(i=0; i<=m-1; i++)
            {
                for(j=n-1; j>=0; j--)
                {
                    vc = ae_complex_from_i(0);
                    vd = ae_complex_from_i(1);
                    if( j<n-1 )
                    {
                        vc = ae_v_cdotproduct(&x->ptr.pp_complex[i2+i][j2+j+1], 1, "N", &a->ptr.pp_complex[i1+j][j1+j+1], 1, "N", ae_v_len(j2+j+1,j2+n-1));
                    }
                    if( !isunit )
                    {
                        vd = a->ptr.pp_complex[i1+j][j1+j];
                    }
                    x->ptr.pp_complex[i2+i][j2+j] = ae_c_div(ae_c_sub(x->ptr.pp_complex[i2+i][j2+j],vc),vd);
                }
            }
            return;
        }
        if( optype==2 )
        {
            
            /*
             * X*A^(-H)
             */
            for(i=0; i<=m-1; i++)
            {
                for(j=n-1; j>=0; j--)
                {
                    vc = ae_complex_from_i(0);
                    vd = ae_complex_from_i(1);
                    if( j<n-1 )
                    {
                        vc = ae_v_cdotproduct(&x->ptr.pp_complex[i2+i][j2+j+1], 1, "N", &a->ptr.pp_complex[i1+j][j1+j+1], 1, "Conj", ae_v_len(j2+j+1,j2+n-1));
                    }
                    if( !isunit )
                    {
                        vd = ae_c_conj(a->ptr.pp_complex[i1+j][j1+j], _state);
                    }
                    x->ptr.pp_complex[i2+i][j2+j] = ae_c_div(ae_c_sub(x->ptr.pp_complex[i2+i][j2+j],vc),vd);
                }
            }
            return;
        }
    }
    else
    {
        
        /*
         * Lower triangular matrix
         */
        if( optype==0 )
        {
            
            /*
             * X*A^(-1)
             */
            for(i=0; i<=m-1; i++)
            {
                for(j=n-1; j>=0; j--)
                {
                    if( isunit )
                    {
                        vd = ae_complex_from_i(1);
                    }
                    else
                    {
                        vd = a->ptr.pp_complex[i1+j][j1+j];
                    }
                    x->ptr.pp_complex[i2+i][j2+j] = ae_c_div(x->ptr.pp_complex[i2+i][j2+j],vd);
                    if( j>0 )
                    {
                        vc = x->ptr.pp_complex[i2+i][j2+j];
                        ae_v_csubc(&x->ptr.pp_complex[i2+i][j2], 1, &a->ptr.pp_complex[i1+j][j1], 1, "N", ae_v_len(j2,j2+j-1), vc);
                    }
                }
            }
            return;
        }
        if( optype==1 )
        {
            
            /*
             * X*A^(-T)
             */
            for(i=0; i<=m-1; i++)
            {
                for(j=0; j<=n-1; j++)
                {
                    vc = ae_complex_from_i(0);
                    vd = ae_complex_from_i(1);
                    if( j>0 )
                    {
                        vc = ae_v_cdotproduct(&x->ptr.pp_complex[i2+i][j2], 1, "N", &a->ptr.pp_complex[i1+j][j1], 1, "N", ae_v_len(j2,j2+j-1));
                    }
                    if( !isunit )
                    {
                        vd = a->ptr.pp_complex[i1+j][j1+j];
                    }
                    x->ptr.pp_complex[i2+i][j2+j] = ae_c_div(ae_c_sub(x->ptr.pp_complex[i2+i][j2+j],vc),vd);
                }
            }
            return;
        }
        if( optype==2 )
        {
            
            /*
             * X*A^(-H)
             */
            for(i=0; i<=m-1; i++)
            {
                for(j=0; j<=n-1; j++)
                {
                    vc = ae_complex_from_i(0);
                    vd = ae_complex_from_i(1);
                    if( j>0 )
                    {
                        vc = ae_v_cdotproduct(&x->ptr.pp_complex[i2+i][j2], 1, "N", &a->ptr.pp_complex[i1+j][j1], 1, "Conj", ae_v_len(j2,j2+j-1));
                    }
                    if( !isunit )
                    {
                        vd = ae_c_conj(a->ptr.pp_complex[i1+j][j1+j], _state);
                    }
                    x->ptr.pp_complex[i2+i][j2+j] = ae_c_div(ae_c_sub(x->ptr.pp_complex[i2+i][j2+j],vc),vd);
                }
            }
            return;
        }
    }
}


/*************************************************************************
Level-2 subroutine
*************************************************************************/
static void ablas_cmatrixlefttrsm2(ae_int_t m,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Complex */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_complex vc;
    ae_complex vd;


    
    /*
     * Special case
     */
    if( n*m==0 )
    {
        return;
    }
    
    /*
     * Try to call fast TRSM
     */
    if( cmatrixlefttrsmf(m, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state) )
    {
        return;
    }
    
    /*
     * General case
     */
    if( isupper )
    {
        
        /*
         * Upper triangular matrix
         */
        if( optype==0 )
        {
            
            /*
             * A^(-1)*X
             */
            for(i=m-1; i>=0; i--)
            {
                for(j=i+1; j<=m-1; j++)
                {
                    vc = a->ptr.pp_complex[i1+i][j1+j];
                    ae_v_csubc(&x->ptr.pp_complex[i2+i][j2], 1, &x->ptr.pp_complex[i2+j][j2], 1, "N", ae_v_len(j2,j2+n-1), vc);
                }
                if( !isunit )
                {
                    vd = ae_c_d_div((double)(1),a->ptr.pp_complex[i1+i][j1+i]);
                    ae_v_cmulc(&x->ptr.pp_complex[i2+i][j2], 1, ae_v_len(j2,j2+n-1), vd);
                }
            }
            return;
        }
        if( optype==1 )
        {
            
            /*
             * A^(-T)*X
             */
            for(i=0; i<=m-1; i++)
            {
                if( isunit )
                {
                    vd = ae_complex_from_i(1);
                }
                else
                {
                    vd = ae_c_d_div((double)(1),a->ptr.pp_complex[i1+i][j1+i]);
                }
                ae_v_cmulc(&x->ptr.pp_complex[i2+i][j2], 1, ae_v_len(j2,j2+n-1), vd);
                for(j=i+1; j<=m-1; j++)
                {
                    vc = a->ptr.pp_complex[i1+i][j1+j];
                    ae_v_csubc(&x->ptr.pp_complex[i2+j][j2], 1, &x->ptr.pp_complex[i2+i][j2], 1, "N", ae_v_len(j2,j2+n-1), vc);
                }
            }
            return;
        }
        if( optype==2 )
        {
            
            /*
             * A^(-H)*X
             */
            for(i=0; i<=m-1; i++)
            {
                if( isunit )
                {
                    vd = ae_complex_from_i(1);
                }
                else
                {
                    vd = ae_c_d_div((double)(1),ae_c_conj(a->ptr.pp_complex[i1+i][j1+i], _state));
                }
                ae_v_cmulc(&x->ptr.pp_complex[i2+i][j2], 1, ae_v_len(j2,j2+n-1), vd);
                for(j=i+1; j<=m-1; j++)
                {
                    vc = ae_c_conj(a->ptr.pp_complex[i1+i][j1+j], _state);
                    ae_v_csubc(&x->ptr.pp_complex[i2+j][j2], 1, &x->ptr.pp_complex[i2+i][j2], 1, "N", ae_v_len(j2,j2+n-1), vc);
                }
            }
            return;
        }
    }
    else
    {
        
        /*
         * Lower triangular matrix
         */
        if( optype==0 )
        {
            
            /*
             * A^(-1)*X
             */
            for(i=0; i<=m-1; i++)
            {
                for(j=0; j<=i-1; j++)
                {
                    vc = a->ptr.pp_complex[i1+i][j1+j];
                    ae_v_csubc(&x->ptr.pp_complex[i2+i][j2], 1, &x->ptr.pp_complex[i2+j][j2], 1, "N", ae_v_len(j2,j2+n-1), vc);
                }
                if( isunit )
                {
                    vd = ae_complex_from_i(1);
                }
                else
                {
                    vd = ae_c_d_div((double)(1),a->ptr.pp_complex[i1+j][j1+j]);
                }
                ae_v_cmulc(&x->ptr.pp_complex[i2+i][j2], 1, ae_v_len(j2,j2+n-1), vd);
            }
            return;
        }
        if( optype==1 )
        {
            
            /*
             * A^(-T)*X
             */
            for(i=m-1; i>=0; i--)
            {
                if( isunit )
                {
                    vd = ae_complex_from_i(1);
                }
                else
                {
                    vd = ae_c_d_div((double)(1),a->ptr.pp_complex[i1+i][j1+i]);
                }
                ae_v_cmulc(&x->ptr.pp_complex[i2+i][j2], 1, ae_v_len(j2,j2+n-1), vd);
                for(j=i-1; j>=0; j--)
                {
                    vc = a->ptr.pp_complex[i1+i][j1+j];
                    ae_v_csubc(&x->ptr.pp_complex[i2+j][j2], 1, &x->ptr.pp_complex[i2+i][j2], 1, "N", ae_v_len(j2,j2+n-1), vc);
                }
            }
            return;
        }
        if( optype==2 )
        {
            
            /*
             * A^(-H)*X
             */
            for(i=m-1; i>=0; i--)
            {
                if( isunit )
                {
                    vd = ae_complex_from_i(1);
                }
                else
                {
                    vd = ae_c_d_div((double)(1),ae_c_conj(a->ptr.pp_complex[i1+i][j1+i], _state));
                }
                ae_v_cmulc(&x->ptr.pp_complex[i2+i][j2], 1, ae_v_len(j2,j2+n-1), vd);
                for(j=i-1; j>=0; j--)
                {
                    vc = ae_c_conj(a->ptr.pp_complex[i1+i][j1+j], _state);
                    ae_v_csubc(&x->ptr.pp_complex[i2+j][j2], 1, &x->ptr.pp_complex[i2+i][j2], 1, "N", ae_v_len(j2,j2+n-1), vc);
                }
            }
            return;
        }
    }
}


/*************************************************************************
Level 2 subroutine

  -- ALGLIB routine --
     15.12.2009
     Bochkanov Sergey
*************************************************************************/
static void ablas_rmatrixrighttrsm2(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Real    */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double vr;
    double vd;


    
    /*
     * Special case
     */
    if( n*m==0 )
    {
        return;
    }
    
    /*
     * Try to use "fast" code
     */
    if( rmatrixrighttrsmf(m, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state) )
    {
        return;
    }
    
    /*
     * General case
     */
    if( isupper )
    {
        
        /*
         * Upper triangular matrix
         */
        if( optype==0 )
        {
            
            /*
             * X*A^(-1)
             */
            for(i=0; i<=m-1; i++)
            {
                for(j=0; j<=n-1; j++)
                {
                    if( isunit )
                    {
                        vd = (double)(1);
                    }
                    else
                    {
                        vd = a->ptr.pp_double[i1+j][j1+j];
                    }
                    x->ptr.pp_double[i2+i][j2+j] = x->ptr.pp_double[i2+i][j2+j]/vd;
                    if( j<n-1 )
                    {
                        vr = x->ptr.pp_double[i2+i][j2+j];
                        ae_v_subd(&x->ptr.pp_double[i2+i][j2+j+1], 1, &a->ptr.pp_double[i1+j][j1+j+1], 1, ae_v_len(j2+j+1,j2+n-1), vr);
                    }
                }
            }
            return;
        }
        if( optype==1 )
        {
            
            /*
             * X*A^(-T)
             */
            for(i=0; i<=m-1; i++)
            {
                for(j=n-1; j>=0; j--)
                {
                    vr = (double)(0);
                    vd = (double)(1);
                    if( j<n-1 )
                    {
                        vr = ae_v_dotproduct(&x->ptr.pp_double[i2+i][j2+j+1], 1, &a->ptr.pp_double[i1+j][j1+j+1], 1, ae_v_len(j2+j+1,j2+n-1));
                    }
                    if( !isunit )
                    {
                        vd = a->ptr.pp_double[i1+j][j1+j];
                    }
                    x->ptr.pp_double[i2+i][j2+j] = (x->ptr.pp_double[i2+i][j2+j]-vr)/vd;
                }
            }
            return;
        }
    }
    else
    {
        
        /*
         * Lower triangular matrix
         */
        if( optype==0 )
        {
            
            /*
             * X*A^(-1)
             */
            for(i=0; i<=m-1; i++)
            {
                for(j=n-1; j>=0; j--)
                {
                    if( isunit )
                    {
                        vd = (double)(1);
                    }
                    else
                    {
                        vd = a->ptr.pp_double[i1+j][j1+j];
                    }
                    x->ptr.pp_double[i2+i][j2+j] = x->ptr.pp_double[i2+i][j2+j]/vd;
                    if( j>0 )
                    {
                        vr = x->ptr.pp_double[i2+i][j2+j];
                        ae_v_subd(&x->ptr.pp_double[i2+i][j2], 1, &a->ptr.pp_double[i1+j][j1], 1, ae_v_len(j2,j2+j-1), vr);
                    }
                }
            }
            return;
        }
        if( optype==1 )
        {
            
            /*
             * X*A^(-T)
             */
            for(i=0; i<=m-1; i++)
            {
                for(j=0; j<=n-1; j++)
                {
                    vr = (double)(0);
                    vd = (double)(1);
                    if( j>0 )
                    {
                        vr = ae_v_dotproduct(&x->ptr.pp_double[i2+i][j2], 1, &a->ptr.pp_double[i1+j][j1], 1, ae_v_len(j2,j2+j-1));
                    }
                    if( !isunit )
                    {
                        vd = a->ptr.pp_double[i1+j][j1+j];
                    }
                    x->ptr.pp_double[i2+i][j2+j] = (x->ptr.pp_double[i2+i][j2+j]-vr)/vd;
                }
            }
            return;
        }
    }
}


/*************************************************************************
Level 2 subroutine
*************************************************************************/
static void ablas_rmatrixlefttrsm2(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Real    */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double vr;
    double vd;


    
    /*
     * Special case
     */
    if( n==0||m==0 )
    {
        return;
    }
    
    /*
     * Try fast code
     */
    if( rmatrixlefttrsmf(m, n, a, i1, j1, isupper, isunit, optype, x, i2, j2, _state) )
    {
        return;
    }
    
    /*
     * General case
     */
    if( isupper )
    {
        
        /*
         * Upper triangular matrix
         */
        if( optype==0 )
        {
            
            /*
             * A^(-1)*X
             */
            for(i=m-1; i>=0; i--)
            {
                for(j=i+1; j<=m-1; j++)
                {
                    vr = a->ptr.pp_double[i1+i][j1+j];
                    ae_v_subd(&x->ptr.pp_double[i2+i][j2], 1, &x->ptr.pp_double[i2+j][j2], 1, ae_v_len(j2,j2+n-1), vr);
                }
                if( !isunit )
                {
                    vd = (double)1/a->ptr.pp_double[i1+i][j1+i];
                    ae_v_muld(&x->ptr.pp_double[i2+i][j2], 1, ae_v_len(j2,j2+n-1), vd);
                }
            }
            return;
        }
        if( optype==1 )
        {
            
            /*
             * A^(-T)*X
             */
            for(i=0; i<=m-1; i++)
            {
                if( isunit )
                {
                    vd = (double)(1);
                }
                else
                {
                    vd = (double)1/a->ptr.pp_double[i1+i][j1+i];
                }
                ae_v_muld(&x->ptr.pp_double[i2+i][j2], 1, ae_v_len(j2,j2+n-1), vd);
                for(j=i+1; j<=m-1; j++)
                {
                    vr = a->ptr.pp_double[i1+i][j1+j];
                    ae_v_subd(&x->ptr.pp_double[i2+j][j2], 1, &x->ptr.pp_double[i2+i][j2], 1, ae_v_len(j2,j2+n-1), vr);
                }
            }
            return;
        }
    }
    else
    {
        
        /*
         * Lower triangular matrix
         */
        if( optype==0 )
        {
            
            /*
             * A^(-1)*X
             */
            for(i=0; i<=m-1; i++)
            {
                for(j=0; j<=i-1; j++)
                {
                    vr = a->ptr.pp_double[i1+i][j1+j];
                    ae_v_subd(&x->ptr.pp_double[i2+i][j2], 1, &x->ptr.pp_double[i2+j][j2], 1, ae_v_len(j2,j2+n-1), vr);
                }
                if( isunit )
                {
                    vd = (double)(1);
                }
                else
                {
                    vd = (double)1/a->ptr.pp_double[i1+j][j1+j];
                }
                ae_v_muld(&x->ptr.pp_double[i2+i][j2], 1, ae_v_len(j2,j2+n-1), vd);
            }
            return;
        }
        if( optype==1 )
        {
            
            /*
             * A^(-T)*X
             */
            for(i=m-1; i>=0; i--)
            {
                if( isunit )
                {
                    vd = (double)(1);
                }
                else
                {
                    vd = (double)1/a->ptr.pp_double[i1+i][j1+i];
                }
                ae_v_muld(&x->ptr.pp_double[i2+i][j2], 1, ae_v_len(j2,j2+n-1), vd);
                for(j=i-1; j>=0; j--)
                {
                    vr = a->ptr.pp_double[i1+i][j1+j];
                    ae_v_subd(&x->ptr.pp_double[i2+j][j2], 1, &x->ptr.pp_double[i2+i][j2], 1, ae_v_len(j2,j2+n-1), vr);
                }
            }
            return;
        }
    }
}


/*************************************************************************
Level 2 subroutine
*************************************************************************/
static void ablas_cmatrixherk2(ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     double beta,
     /* Complex */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_bool isupper,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t j1;
    ae_int_t j2;
    ae_complex v;


    
    /*
     * Fast exit (nothing to be done)
     */
    if( (ae_fp_eq(alpha,(double)(0))||k==0)&&ae_fp_eq(beta,(double)(1)) )
    {
        return;
    }
    
    /*
     * Try to call fast SYRK
     */
    if( cmatrixherkf(n, k, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper, _state) )
    {
        return;
    }
    
    /*
     * SYRK
     */
    if( optypea==0 )
    {
        
        /*
         * C=alpha*A*A^H+beta*C
         */
        for(i=0; i<=n-1; i++)
        {
            if( isupper )
            {
                j1 = i;
                j2 = n-1;
            }
            else
            {
                j1 = 0;
                j2 = i;
            }
            for(j=j1; j<=j2; j++)
            {
                if( ae_fp_neq(alpha,(double)(0))&&k>0 )
                {
                    v = ae_v_cdotproduct(&a->ptr.pp_complex[ia+i][ja], 1, "N", &a->ptr.pp_complex[ia+j][ja], 1, "Conj", ae_v_len(ja,ja+k-1));
                }
                else
                {
                    v = ae_complex_from_i(0);
                }
                if( ae_fp_eq(beta,(double)(0)) )
                {
                    c->ptr.pp_complex[ic+i][jc+j] = ae_c_mul_d(v,alpha);
                }
                else
                {
                    c->ptr.pp_complex[ic+i][jc+j] = ae_c_add(ae_c_mul_d(c->ptr.pp_complex[ic+i][jc+j],beta),ae_c_mul_d(v,alpha));
                }
            }
        }
        return;
    }
    else
    {
        
        /*
         * C=alpha*A^H*A+beta*C
         */
        for(i=0; i<=n-1; i++)
        {
            if( isupper )
            {
                j1 = i;
                j2 = n-1;
            }
            else
            {
                j1 = 0;
                j2 = i;
            }
            if( ae_fp_eq(beta,(double)(0)) )
            {
                for(j=j1; j<=j2; j++)
                {
                    c->ptr.pp_complex[ic+i][jc+j] = ae_complex_from_i(0);
                }
            }
            else
            {
                ae_v_cmuld(&c->ptr.pp_complex[ic+i][jc+j1], 1, ae_v_len(jc+j1,jc+j2), beta);
            }
        }
        if( ae_fp_neq(alpha,(double)(0))&&k>0 )
        {
            for(i=0; i<=k-1; i++)
            {
                for(j=0; j<=n-1; j++)
                {
                    if( isupper )
                    {
                        j1 = j;
                        j2 = n-1;
                    }
                    else
                    {
                        j1 = 0;
                        j2 = j;
                    }
                    v = ae_c_mul_d(ae_c_conj(a->ptr.pp_complex[ia+i][ja+j], _state),alpha);
                    ae_v_caddc(&c->ptr.pp_complex[ic+j][jc+j1], 1, &a->ptr.pp_complex[ia+i][ja+j1], 1, "N", ae_v_len(jc+j1,jc+j2), v);
                }
            }
        }
        return;
    }
}


/*************************************************************************
Level 2 subrotuine
*************************************************************************/
static void ablas_rmatrixsyrk2(ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     double beta,
     /* Real    */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_bool isupper,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t j1;
    ae_int_t j2;
    double v;


    
    /*
     * Fast exit (nothing to be done)
     */
    if( (ae_fp_eq(alpha,(double)(0))||k==0)&&ae_fp_eq(beta,(double)(1)) )
    {
        return;
    }
    
    /*
     * Try to call fast SYRK
     */
    if( rmatrixsyrkf(n, k, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper, _state) )
    {
        return;
    }
    
    /*
     * SYRK
     */
    if( optypea==0 )
    {
        
        /*
         * C=alpha*A*A^H+beta*C
         */
        for(i=0; i<=n-1; i++)
        {
            if( isupper )
            {
                j1 = i;
                j2 = n-1;
            }
            else
            {
                j1 = 0;
                j2 = i;
            }
            for(j=j1; j<=j2; j++)
            {
                if( ae_fp_neq(alpha,(double)(0))&&k>0 )
                {
                    v = ae_v_dotproduct(&a->ptr.pp_double[ia+i][ja], 1, &a->ptr.pp_double[ia+j][ja], 1, ae_v_len(ja,ja+k-1));
                }
                else
                {
                    v = (double)(0);
                }
                if( ae_fp_eq(beta,(double)(0)) )
                {
                    c->ptr.pp_double[ic+i][jc+j] = alpha*v;
                }
                else
                {
                    c->ptr.pp_double[ic+i][jc+j] = beta*c->ptr.pp_double[ic+i][jc+j]+alpha*v;
                }
            }
        }
        return;
    }
    else
    {
        
        /*
         * C=alpha*A^H*A+beta*C
         */
        for(i=0; i<=n-1; i++)
        {
            if( isupper )
            {
                j1 = i;
                j2 = n-1;
            }
            else
            {
                j1 = 0;
                j2 = i;
            }
            if( ae_fp_eq(beta,(double)(0)) )
            {
                for(j=j1; j<=j2; j++)
                {
                    c->ptr.pp_double[ic+i][jc+j] = (double)(0);
                }
            }
            else
            {
                ae_v_muld(&c->ptr.pp_double[ic+i][jc+j1], 1, ae_v_len(jc+j1,jc+j2), beta);
            }
        }
        if( ae_fp_neq(alpha,(double)(0))&&k>0 )
        {
            for(i=0; i<=k-1; i++)
            {
                for(j=0; j<=n-1; j++)
                {
                    if( isupper )
                    {
                        j1 = j;
                        j2 = n-1;
                    }
                    else
                    {
                        j1 = 0;
                        j2 = j;
                    }
                    v = alpha*a->ptr.pp_double[ia+i][ja+j];
                    ae_v_addd(&c->ptr.pp_double[ic+j][jc+j1], 1, &a->ptr.pp_double[ia+i][ja+j1], 1, ae_v_len(jc+j1,jc+j2), v);
                }
            }
        }
        return;
    }
}


/*************************************************************************
This subroutine is an actual implementation of CMatrixGEMM.  It  does  not
perform some integrity checks performed in the  driver  function,  and  it
does not activate multithreading  framework  (driver  decides  whether  to
activate workers or not).

  -- ALGLIB routine --
     10.01.2019
     Bochkanov Sergey
*************************************************************************/
static void ablas_cmatrixgemmrec(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     ae_complex alpha,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     /* Complex */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_int_t optypeb,
     ae_complex beta,
     /* Complex */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state)
{
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t s1;
    ae_int_t s2;
    ae_int_t tsa;
    ae_int_t tsb;
    ae_int_t tscur;


    
    /*
     * Tile hierarchy: B -> A -> A/2
     */
    tsa = matrixtilesizea(_state)/2;
    tsb = matrixtilesizeb(_state);
    tscur = tsb;
    if( imax3(m, n, k, _state)<=tsb )
    {
        tscur = tsa;
    }
    ae_assert(tscur>=1, "CMatrixGEMMRec: integrity check failed", _state);
    
    /*
     * Use MKL or ALGLIB basecase code
     */
    if( imax3(m, n, k, _state)<=tsb )
    {
        if( cmatrixgemmmkl(m, n, k, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    if( imax3(m, n, k, _state)<=tsa )
    {
        cmatrixgemmk(m, n, k, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Recursive algorithm: parallel splitting on M/N
     */
    if( m>=n&&m>=k )
    {
        
        /*
         * A*B = (A1 A2)^T*B
         */
        tiledsplit(m, tscur, &s1, &s2, _state);
        ae_set_smp_support(&_child_tasks, &_smp_enabled, m>=2*tsb&&ae_fp_greater_eq((double)8*rmul3((double)(ae_minint(s1, s2, _state)), (double)(n), (double)(k), _state),spawnlevel(_state)), _state);
        _spawn_ablas_cmatrixgemmrec(s1, n, k, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _child_tasks, _smp_enabled, _state);
        if( optypea==0 )
        {
            ablas_cmatrixgemmrec(s2, n, k, alpha, a, ia+s1, ja, optypea, b, ib, jb, optypeb, beta, c, ic+s1, jc, _state);
        }
        else
        {
            ablas_cmatrixgemmrec(s2, n, k, alpha, a, ia, ja+s1, optypea, b, ib, jb, optypeb, beta, c, ic+s1, jc, _state);
        }
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    if( n>=m&&n>=k )
    {
        
        /*
         * A*B = A*(B1 B2)
         */
        tiledsplit(n, tscur, &s1, &s2, _state);
        ae_set_smp_support(&_child_tasks, &_smp_enabled, n>=2*tsb&&ae_fp_greater_eq((double)8*rmul3((double)(m), (double)(ae_minint(s1, s2, _state)), (double)(k), _state),spawnlevel(_state)), _state);
        if( optypeb==0 )
        {
            _spawn_ablas_cmatrixgemmrec(m, s1, k, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _child_tasks, _smp_enabled, _state);
            ablas_cmatrixgemmrec(m, s2, k, alpha, a, ia, ja, optypea, b, ib, jb+s1, optypeb, beta, c, ic, jc+s1, _state);
        }
        else
        {
            _spawn_ablas_cmatrixgemmrec(m, s1, k, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _child_tasks, _smp_enabled, _state);
            ablas_cmatrixgemmrec(m, s2, k, alpha, a, ia, ja, optypea, b, ib+s1, jb, optypeb, beta, c, ic, jc+s1, _state);
        }
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Recursive algorithm: serial splitting on K
     */
    
    /*
     * A*B = (A1 A2)*(B1 B2)^T
     */
    tiledsplit(k, tscur, &s1, &s2, _state);
    if( optypea==0&&optypeb==0 )
    {
        ablas_cmatrixgemmrec(m, n, s1, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _state);
        ablas_cmatrixgemmrec(m, n, s2, alpha, a, ia, ja+s1, optypea, b, ib+s1, jb, optypeb, ae_complex_from_d(1.0), c, ic, jc, _state);
    }
    if( optypea==0&&optypeb!=0 )
    {
        ablas_cmatrixgemmrec(m, n, s1, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _state);
        ablas_cmatrixgemmrec(m, n, s2, alpha, a, ia, ja+s1, optypea, b, ib, jb+s1, optypeb, ae_complex_from_d(1.0), c, ic, jc, _state);
    }
    if( optypea!=0&&optypeb==0 )
    {
        ablas_cmatrixgemmrec(m, n, s1, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _state);
        ablas_cmatrixgemmrec(m, n, s2, alpha, a, ia+s1, ja, optypea, b, ib+s1, jb, optypeb, ae_complex_from_d(1.0), c, ic, jc, _state);
    }
    if( optypea!=0&&optypeb!=0 )
    {
        ablas_cmatrixgemmrec(m, n, s1, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _state);
        ablas_cmatrixgemmrec(m, n, s2, alpha, a, ia+s1, ja, optypea, b, ib, jb+s1, optypeb, ae_complex_from_d(1.0), c, ic, jc, _state);
    }
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_ablas_cmatrixgemmrec(ae_int_t m,
    ae_int_t n,
    ae_int_t k,
    ae_complex alpha,
    /* Complex */ const ae_matrix* a,
    ae_int_t ia,
    ae_int_t ja,
    ae_int_t optypea,
    /* Complex */ const ae_matrix* b,
    ae_int_t ib,
    ae_int_t jb,
    ae_int_t optypeb,
    ae_complex beta,
    /* Complex */ ae_matrix* c,
    ae_int_t ic,
    ae_int_t jc,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        ablas_cmatrixgemmrec(m,n,k,alpha,a,ia,ja,optypea,b,ib,jb,optypeb,beta,c,ic,jc, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_ablas_cmatrixgemmrec;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.ival = m;
        _task->data.parameters[1].value.ival = n;
        _task->data.parameters[2].value.ival = k;
        _task->data.parameters[3].value.cval = alpha;
        _task->data.parameters[4].value.const_val = a;
        _task->data.parameters[5].value.ival = ia;
        _task->data.parameters[6].value.ival = ja;
        _task->data.parameters[7].value.ival = optypea;
        _task->data.parameters[8].value.const_val = b;
        _task->data.parameters[9].value.ival = ib;
        _task->data.parameters[10].value.ival = jb;
        _task->data.parameters[11].value.ival = optypeb;
        _task->data.parameters[12].value.cval = beta;
        _task->data.parameters[13].value.val = c;
        _task->data.parameters[14].value.ival = ic;
        _task->data.parameters[15].value.ival = jc;
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
void _task_ablas_cmatrixgemmrec(ae_task_data *_data, ae_state *_state)
{
    ae_int_t m;
    ae_int_t n;
    ae_int_t k;
    ae_complex alpha;
    const ae_matrix* a;
    ae_int_t ia;
    ae_int_t ja;
    ae_int_t optypea;
    const ae_matrix* b;
    ae_int_t ib;
    ae_int_t jb;
    ae_int_t optypeb;
    ae_complex beta;
    ae_matrix* c;
    ae_int_t ic;
    ae_int_t jc;
    m = _data->parameters[0].value.ival;
    n = _data->parameters[1].value.ival;
    k = _data->parameters[2].value.ival;
    alpha = _data->parameters[3].value.cval;
    a = (const ae_matrix*)_data->parameters[4].value.const_val;
    ia = _data->parameters[5].value.ival;
    ja = _data->parameters[6].value.ival;
    optypea = _data->parameters[7].value.ival;
    b = (const ae_matrix*)_data->parameters[8].value.const_val;
    ib = _data->parameters[9].value.ival;
    jb = _data->parameters[10].value.ival;
    optypeb = _data->parameters[11].value.ival;
    beta = _data->parameters[12].value.cval;
    c = (ae_matrix*)_data->parameters[13].value.val;
    ic = _data->parameters[14].value.ival;
    jc = _data->parameters[15].value.ival;
   ae_state_set_flags(_state, _data->flags);
   ablas_cmatrixgemmrec(m,n,k,alpha,a,ia,ja,optypea,b,ib,jb,optypeb,beta,c,ic,jc, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_ablas_cmatrixgemmrec(ae_int_t m,
    ae_int_t n,
    ae_int_t k,
    ae_complex alpha,
    /* Complex */ const ae_matrix* a,
    ae_int_t ia,
    ae_int_t ja,
    ae_int_t optypea,
    /* Complex */ const ae_matrix* b,
    ae_int_t ib,
    ae_int_t jb,
    ae_int_t optypeb,
    ae_complex beta,
    /* Complex */ ae_matrix* c,
    ae_int_t ic,
    ae_int_t jc,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_ablas_cmatrixgemmrec;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.ival = m;
    _task->data.parameters[1].value.ival = n;
    _task->data.parameters[2].value.ival = k;
    _task->data.parameters[3].value.cval = alpha;
    _task->data.parameters[4].value.const_val = a;
    _task->data.parameters[5].value.ival = ia;
    _task->data.parameters[6].value.ival = ja;
    _task->data.parameters[7].value.ival = optypea;
    _task->data.parameters[8].value.const_val = b;
    _task->data.parameters[9].value.ival = ib;
    _task->data.parameters[10].value.ival = jb;
    _task->data.parameters[11].value.ival = optypeb;
    _task->data.parameters[12].value.cval = beta;
    _task->data.parameters[13].value.val = c;
    _task->data.parameters[14].value.ival = ic;
    _task->data.parameters[15].value.ival = jc;
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
This subroutine is an actual implementation of RMatrixGEMM.  It  does  not
perform some integrity checks performed in the  driver  function,  and  it
does not activate multithreading  framework  (driver  decides  whether  to
activate workers or not).

  -- ALGLIB routine --
     10.01.2019
     Bochkanov Sergey
*************************************************************************/
static void ablas_rmatrixgemmrec(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_int_t optypeb,
     double beta,
     /* Real    */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state)
{
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t s1;
    ae_int_t s2;
    ae_int_t tsa;
    ae_int_t tsb;
    ae_int_t tscur;


    tsa = matrixtilesizea(_state);
    tsb = matrixtilesizeb(_state);
    tscur = tsb;
    if( imax3(m, n, k, _state)<=tsb )
    {
        tscur = tsa;
    }
    ae_assert(tscur>=1, "RMatrixGEMMRec: integrity check failed", _state);
    
    /*
     * Use MKL or ALGLIB basecase code
     */
    if( (m<=tsb&&n<=tsb)&&k<=tsb )
    {
        if( rmatrixgemmmkl(m, n, k, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    if( (m<=tsa&&n<=tsa)&&k<=tsa )
    {
        rmatrixgemmk(m, n, k, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Recursive algorithm: split on M or N
     */
    if( m>=n&&m>=k )
    {
        
        /*
         * A*B = (A1 A2)^T*B
         */
        tiledsplit(m, tscur, &s1, &s2, _state);
        ae_set_smp_support(&_child_tasks, &_smp_enabled, m>=2*tsb&&ae_fp_greater_eq((double)2*rmul3((double)(ae_minint(s1, s2, _state)), (double)(n), (double)(k), _state),spawnlevel(_state)), _state);
        if( optypea==0 )
        {
            _spawn_ablas_rmatrixgemmrec(s2, n, k, alpha, a, ia+s1, ja, optypea, b, ib, jb, optypeb, beta, c, ic+s1, jc, _child_tasks, _smp_enabled, _state);
            ablas_rmatrixgemmrec(s1, n, k, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _state);
        }
        else
        {
            _spawn_ablas_rmatrixgemmrec(s2, n, k, alpha, a, ia, ja+s1, optypea, b, ib, jb, optypeb, beta, c, ic+s1, jc, _child_tasks, _smp_enabled, _state);
            ablas_rmatrixgemmrec(s1, n, k, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _state);
        }
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    if( n>=m&&n>=k )
    {
        
        /*
         * A*B = A*(B1 B2)
         */
        tiledsplit(n, tscur, &s1, &s2, _state);
        ae_set_smp_support(&_child_tasks, &_smp_enabled, n>=2*tsb&&ae_fp_greater_eq((double)2*rmul3((double)(m), (double)(ae_minint(s1, s2, _state)), (double)(k), _state),spawnlevel(_state)), _state);
        if( optypeb==0 )
        {
            _spawn_ablas_rmatrixgemmrec(m, s2, k, alpha, a, ia, ja, optypea, b, ib, jb+s1, optypeb, beta, c, ic, jc+s1, _child_tasks, _smp_enabled, _state);
            ablas_rmatrixgemmrec(m, s1, k, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _state);
        }
        else
        {
            _spawn_ablas_rmatrixgemmrec(m, s2, k, alpha, a, ia, ja, optypea, b, ib+s1, jb, optypeb, beta, c, ic, jc+s1, _child_tasks, _smp_enabled, _state);
            ablas_rmatrixgemmrec(m, s1, k, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _state);
        }
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Recursive algorithm: split on K
     */
    
    /*
     * A*B = (A1 A2)*(B1 B2)^T
     */
    tiledsplit(k, tscur, &s1, &s2, _state);
    if( optypea==0&&optypeb==0 )
    {
        ablas_rmatrixgemmrec(m, n, s1, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _state);
        ablas_rmatrixgemmrec(m, n, s2, alpha, a, ia, ja+s1, optypea, b, ib+s1, jb, optypeb, 1.0, c, ic, jc, _state);
    }
    if( optypea==0&&optypeb!=0 )
    {
        ablas_rmatrixgemmrec(m, n, s1, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _state);
        ablas_rmatrixgemmrec(m, n, s2, alpha, a, ia, ja+s1, optypea, b, ib, jb+s1, optypeb, 1.0, c, ic, jc, _state);
    }
    if( optypea!=0&&optypeb==0 )
    {
        ablas_rmatrixgemmrec(m, n, s1, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _state);
        ablas_rmatrixgemmrec(m, n, s2, alpha, a, ia+s1, ja, optypea, b, ib+s1, jb, optypeb, 1.0, c, ic, jc, _state);
    }
    if( optypea!=0&&optypeb!=0 )
    {
        ablas_rmatrixgemmrec(m, n, s1, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _state);
        ablas_rmatrixgemmrec(m, n, s2, alpha, a, ia+s1, ja, optypea, b, ib, jb+s1, optypeb, 1.0, c, ic, jc, _state);
    }
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_ablas_rmatrixgemmrec(ae_int_t m,
    ae_int_t n,
    ae_int_t k,
    double alpha,
    /* Real    */ const ae_matrix* a,
    ae_int_t ia,
    ae_int_t ja,
    ae_int_t optypea,
    /* Real    */ const ae_matrix* b,
    ae_int_t ib,
    ae_int_t jb,
    ae_int_t optypeb,
    double beta,
    /* Real    */ ae_matrix* c,
    ae_int_t ic,
    ae_int_t jc,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        ablas_rmatrixgemmrec(m,n,k,alpha,a,ia,ja,optypea,b,ib,jb,optypeb,beta,c,ic,jc, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_ablas_rmatrixgemmrec;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.ival = m;
        _task->data.parameters[1].value.ival = n;
        _task->data.parameters[2].value.ival = k;
        _task->data.parameters[3].value.dval = alpha;
        _task->data.parameters[4].value.const_val = a;
        _task->data.parameters[5].value.ival = ia;
        _task->data.parameters[6].value.ival = ja;
        _task->data.parameters[7].value.ival = optypea;
        _task->data.parameters[8].value.const_val = b;
        _task->data.parameters[9].value.ival = ib;
        _task->data.parameters[10].value.ival = jb;
        _task->data.parameters[11].value.ival = optypeb;
        _task->data.parameters[12].value.dval = beta;
        _task->data.parameters[13].value.val = c;
        _task->data.parameters[14].value.ival = ic;
        _task->data.parameters[15].value.ival = jc;
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
void _task_ablas_rmatrixgemmrec(ae_task_data *_data, ae_state *_state)
{
    ae_int_t m;
    ae_int_t n;
    ae_int_t k;
    double alpha;
    const ae_matrix* a;
    ae_int_t ia;
    ae_int_t ja;
    ae_int_t optypea;
    const ae_matrix* b;
    ae_int_t ib;
    ae_int_t jb;
    ae_int_t optypeb;
    double beta;
    ae_matrix* c;
    ae_int_t ic;
    ae_int_t jc;
    m = _data->parameters[0].value.ival;
    n = _data->parameters[1].value.ival;
    k = _data->parameters[2].value.ival;
    alpha = _data->parameters[3].value.dval;
    a = (const ae_matrix*)_data->parameters[4].value.const_val;
    ia = _data->parameters[5].value.ival;
    ja = _data->parameters[6].value.ival;
    optypea = _data->parameters[7].value.ival;
    b = (const ae_matrix*)_data->parameters[8].value.const_val;
    ib = _data->parameters[9].value.ival;
    jb = _data->parameters[10].value.ival;
    optypeb = _data->parameters[11].value.ival;
    beta = _data->parameters[12].value.dval;
    c = (ae_matrix*)_data->parameters[13].value.val;
    ic = _data->parameters[14].value.ival;
    jc = _data->parameters[15].value.ival;
   ae_state_set_flags(_state, _data->flags);
   ablas_rmatrixgemmrec(m,n,k,alpha,a,ia,ja,optypea,b,ib,jb,optypeb,beta,c,ic,jc, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_ablas_rmatrixgemmrec(ae_int_t m,
    ae_int_t n,
    ae_int_t k,
    double alpha,
    /* Real    */ const ae_matrix* a,
    ae_int_t ia,
    ae_int_t ja,
    ae_int_t optypea,
    /* Real    */ const ae_matrix* b,
    ae_int_t ib,
    ae_int_t jb,
    ae_int_t optypeb,
    double beta,
    /* Real    */ ae_matrix* c,
    ae_int_t ic,
    ae_int_t jc,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_ablas_rmatrixgemmrec;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.ival = m;
    _task->data.parameters[1].value.ival = n;
    _task->data.parameters[2].value.ival = k;
    _task->data.parameters[3].value.dval = alpha;
    _task->data.parameters[4].value.const_val = a;
    _task->data.parameters[5].value.ival = ia;
    _task->data.parameters[6].value.ival = ja;
    _task->data.parameters[7].value.ival = optypea;
    _task->data.parameters[8].value.const_val = b;
    _task->data.parameters[9].value.ival = ib;
    _task->data.parameters[10].value.ival = jb;
    _task->data.parameters[11].value.ival = optypeb;
    _task->data.parameters[12].value.dval = beta;
    _task->data.parameters[13].value.val = c;
    _task->data.parameters[14].value.ival = ic;
    _task->data.parameters[15].value.ival = jc;
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


/*$ End $*/
