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
#include <stdio.h>
#include "apserv.h"


/*$ Declarations $*/
static ae_int_t apserv_maxtemporariesinnpool = 1000;


/*$ Body $*/


/*************************************************************************
Internally calls SetErrorFlag() with condition:

    Abs(Val-RefVal)>Tol*Max(Abs(RefVal),S)
    
This function is used to test relative error in Val against  RefVal,  with
relative error being replaced by absolute when scale  of  RefVal  is  less
than S.

This function returns value of COND.
*************************************************************************/
void seterrorflagdiff(ae_bool* flag,
     double val,
     double refval,
     double tol,
     double s,
     ae_state *_state)
{


    ae_set_error_flag(flag, ae_fp_greater(ae_fabs(val-refval, _state),tol*ae_maxreal(ae_fabs(refval, _state), s, _state)), __FILE__, __LINE__, "apserv.ap:238");
}


/*************************************************************************
The function always returns False.
It may be used sometimes to prevent spurious warnings.

  -- ALGLIB --
     Copyright 17.09.2012 by Bochkanov Sergey
*************************************************************************/
ae_bool alwaysfalse(ae_state *_state)
{
    ae_bool result;


    result = ae_false;
    return result;
}


/*************************************************************************
The function "touches" boolean - it is used  to  avoid  compiler  messages
about unused variables (in rare cases when we do NOT want to remove  these
variables).

  -- ALGLIB --
     Copyright 17.09.2012 by Bochkanov Sergey
*************************************************************************/
void touchboolean(ae_bool* a, ae_state *_state)
{


}


/*************************************************************************
The function "touches" integer - it is used  to  avoid  compiler  messages
about unused variables (in rare cases when we do NOT want to remove  these
variables).

  -- ALGLIB --
     Copyright 17.09.2012 by Bochkanov Sergey
*************************************************************************/
void touchint(ae_int_t* a, ae_state *_state)
{


}


/*************************************************************************
The function "touches" real   -  it is used  to  avoid  compiler  messages
about unused variables (in rare cases when we do NOT want to remove  these
variables).

  -- ALGLIB --
     Copyright 17.09.2012 by Bochkanov Sergey
*************************************************************************/
void touchreal(double* a, ae_state *_state)
{


}


/*************************************************************************
The function performs zero-coalescing on real value.

NOTE: no check is performed for B<>0

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
double coalesce(double a, double b, ae_state *_state)
{
    double result;


    result = a;
    if( ae_fp_eq(a,0.0) )
    {
        result = b;
    }
    return result;
}


/*************************************************************************
The function performs zero-coalescing on integer value.

NOTE: no check is performed for B<>0

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
ae_int_t coalescei(ae_int_t a, ae_int_t b, ae_state *_state)
{
    ae_int_t result;


    result = a;
    if( a==0 )
    {
        result = b;
    }
    return result;
}


/*************************************************************************
The function convert integer value to real value.

  -- ALGLIB --
     Copyright 17.09.2012 by Bochkanov Sergey
*************************************************************************/
double inttoreal(ae_int_t a, ae_state *_state)
{
    double result;


    result = (double)(a);
    return result;
}


/*************************************************************************
The function calculates binary logarithm.

NOTE: it costs twice as much as Ln(x)

  -- ALGLIB --
     Copyright 17.09.2012 by Bochkanov Sergey
*************************************************************************/
double logbase2(double x, ae_state *_state)
{
    double result;


    result = ae_log(x, _state)/ae_log((double)(2), _state);
    return result;
}


/*************************************************************************
This function compares two numbers for approximate equality, with tolerance
to errors as large as tol.


  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
ae_bool approxequal(double a, double b, double tol, ae_state *_state)
{
    ae_bool result;


    result = ae_fp_less_eq(ae_fabs(a-b, _state),tol);
    return result;
}


/*************************************************************************
This function compares two numbers for approximate equality, with tolerance
to errors as large as max(|a|,|b|)*tol.


  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
ae_bool approxequalrel(double a, double b, double tol, ae_state *_state)
{
    ae_bool result;


    result = ae_fp_less_eq(ae_fabs(a-b, _state),ae_maxreal(ae_fabs(a, _state), ae_fabs(b, _state), _state)*tol);
    return result;
}


/*************************************************************************
This  function  generates  1-dimensional  general  interpolation task with
moderate Lipshitz constant (close to 1.0)

If N=1 then suborutine generates only one point at the middle of [A,B]

  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
void taskgenint1d(double a,
     double b,
     ae_int_t n,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t i;
    double h;

    ae_vector_clear(x);
    ae_vector_clear(y);

    ae_assert(n>=1, "TaskGenInterpolationEqdist1D: N<1!", _state);
    ae_vector_set_length(x, n, _state);
    ae_vector_set_length(y, n, _state);
    if( n>1 )
    {
        x->ptr.p_double[0] = a;
        y->ptr.p_double[0] = (double)2*ae_randomreal(_state)-(double)1;
        h = (b-a)/(double)(n-1);
        for(i=1; i<=n-1; i++)
        {
            if( i!=n-1 )
            {
                x->ptr.p_double[i] = a+((double)i+0.2*((double)2*ae_randomreal(_state)-(double)1))*h;
            }
            else
            {
                x->ptr.p_double[i] = b;
            }
            y->ptr.p_double[i] = y->ptr.p_double[i-1]+((double)2*ae_randomreal(_state)-(double)1)*(x->ptr.p_double[i]-x->ptr.p_double[i-1]);
        }
    }
    else
    {
        x->ptr.p_double[0] = 0.5*(a+b);
        y->ptr.p_double[0] = (double)2*ae_randomreal(_state)-(double)1;
    }
}


/*************************************************************************
This function generates  1-dimensional equidistant interpolation task with
moderate Lipshitz constant (close to 1.0)

If N=1 then suborutine generates only one point at the middle of [A,B]

  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
void taskgenint1dequidist(double a,
     double b,
     ae_int_t n,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t i;
    double h;

    ae_vector_clear(x);
    ae_vector_clear(y);

    ae_assert(n>=1, "TaskGenInterpolationEqdist1D: N<1!", _state);
    ae_vector_set_length(x, n, _state);
    ae_vector_set_length(y, n, _state);
    if( n>1 )
    {
        x->ptr.p_double[0] = a;
        y->ptr.p_double[0] = (double)2*ae_randomreal(_state)-(double)1;
        h = (b-a)/(double)(n-1);
        for(i=1; i<=n-1; i++)
        {
            x->ptr.p_double[i] = a+(double)i*h;
            y->ptr.p_double[i] = y->ptr.p_double[i-1]+((double)2*ae_randomreal(_state)-(double)1)*h;
        }
    }
    else
    {
        x->ptr.p_double[0] = 0.5*(a+b);
        y->ptr.p_double[0] = (double)2*ae_randomreal(_state)-(double)1;
    }
}


/*************************************************************************
This function generates  1-dimensional Chebyshev-1 interpolation task with
moderate Lipshitz constant (close to 1.0)

If N=1 then suborutine generates only one point at the middle of [A,B]

  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
void taskgenint1dcheb1(double a,
     double b,
     ae_int_t n,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t i;

    ae_vector_clear(x);
    ae_vector_clear(y);

    ae_assert(n>=1, "TaskGenInterpolation1DCheb1: N<1!", _state);
    ae_vector_set_length(x, n, _state);
    ae_vector_set_length(y, n, _state);
    if( n>1 )
    {
        for(i=0; i<=n-1; i++)
        {
            x->ptr.p_double[i] = 0.5*(b+a)+0.5*(b-a)*ae_cos(ae_pi*(double)(2*i+1)/(double)(2*n), _state);
            if( i==0 )
            {
                y->ptr.p_double[i] = (double)2*ae_randomreal(_state)-(double)1;
            }
            else
            {
                y->ptr.p_double[i] = y->ptr.p_double[i-1]+((double)2*ae_randomreal(_state)-(double)1)*(x->ptr.p_double[i]-x->ptr.p_double[i-1]);
            }
        }
    }
    else
    {
        x->ptr.p_double[0] = 0.5*(a+b);
        y->ptr.p_double[0] = (double)2*ae_randomreal(_state)-(double)1;
    }
}


/*************************************************************************
This function generates  1-dimensional Chebyshev-2 interpolation task with
moderate Lipshitz constant (close to 1.0)

If N=1 then suborutine generates only one point at the middle of [A,B]

  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
void taskgenint1dcheb2(double a,
     double b,
     ae_int_t n,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t i;

    ae_vector_clear(x);
    ae_vector_clear(y);

    ae_assert(n>=1, "TaskGenInterpolation1DCheb2: N<1!", _state);
    ae_vector_set_length(x, n, _state);
    ae_vector_set_length(y, n, _state);
    if( n>1 )
    {
        for(i=0; i<=n-1; i++)
        {
            x->ptr.p_double[i] = 0.5*(b+a)+0.5*(b-a)*ae_cos(ae_pi*(double)i/(double)(n-1), _state);
            if( i==0 )
            {
                y->ptr.p_double[i] = (double)2*ae_randomreal(_state)-(double)1;
            }
            else
            {
                y->ptr.p_double[i] = y->ptr.p_double[i-1]+((double)2*ae_randomreal(_state)-(double)1)*(x->ptr.p_double[i]-x->ptr.p_double[i-1]);
            }
        }
    }
    else
    {
        x->ptr.p_double[0] = 0.5*(a+b);
        y->ptr.p_double[0] = (double)2*ae_randomreal(_state)-(double)1;
    }
}


/*************************************************************************
This function checks that all values from X[] are distinct. It does more
than just usual floating point comparison:
* first, it calculates max(X) and min(X)
* second, it maps X[] from [min,max] to [1,2]
* only at this stage actual comparison is done

The meaning of such check is to ensure that all values are "distinct enough"
and will not cause interpolation subroutine to fail.

NOTE:
    X[] must be sorted by ascending (subroutine ASSERT's it)

  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
ae_bool aredistinct(/* Real    */ const ae_vector* x,
     ae_int_t n,
     ae_state *_state)
{
    double a;
    double b;
    ae_int_t i;
    ae_bool nonsorted;
    ae_bool result;


    ae_assert(n>=1, "APSERVAreDistinct: internal error (N<1)", _state);
    if( n==1 )
    {
        
        /*
         * everything is alright, it is up to caller to decide whether it
         * can interpolate something with just one point
         */
        result = ae_true;
        return result;
    }
    a = x->ptr.p_double[0];
    b = x->ptr.p_double[0];
    nonsorted = ae_false;
    for(i=1; i<=n-1; i++)
    {
        a = ae_minreal(a, x->ptr.p_double[i], _state);
        b = ae_maxreal(b, x->ptr.p_double[i], _state);
        nonsorted = nonsorted||ae_fp_greater_eq(x->ptr.p_double[i-1],x->ptr.p_double[i]);
    }
    ae_assert(!nonsorted, "APSERVAreDistinct: internal error (not sorted)", _state);
    for(i=1; i<=n-1; i++)
    {
        if( ae_fp_eq((x->ptr.p_double[i]-a)/(b-a)+(double)1,(x->ptr.p_double[i-1]-a)/(b-a)+(double)1) )
        {
            result = ae_false;
            return result;
        }
    }
    result = ae_true;
    return result;
}


/*************************************************************************
This function checks that two boolean values are the same (both  are  True 
or both are False).

  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
ae_bool aresameboolean(ae_bool v1, ae_bool v2, ae_state *_state)
{
    ae_bool result;


    result = (v1&&v2)||(!v1&&!v2);
    return result;
}


/*************************************************************************
Resizes X and fills by zeros

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void setlengthzero(/* Real    */ ae_vector* x,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(n>=0, "SetLengthZero: N<0", _state);
    ae_vector_set_length(x, n, _state);
    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = (double)(0);
    }
}


/*************************************************************************
If Length(X)<N, resizes X

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void bvectorsetlengthatleast(/* Boolean */ ae_vector* x,
     ae_int_t n,
     ae_state *_state)
{


    if( x->cnt<n )
    {
        ae_vector_set_length(x, n, _state);
    }
}


/*************************************************************************
If Length(X)<N, resizes X

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void ivectorsetlengthatleast(/* Integer */ ae_vector* x,
     ae_int_t n,
     ae_state *_state)
{


    if( x->cnt<n )
    {
        ae_vector_set_length(x, n, _state);
    }
}


/*************************************************************************
If Length(X)<N, resizes X

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void rvectorsetlengthatleast(/* Real    */ ae_vector* x,
     ae_int_t n,
     ae_state *_state)
{


    if( x->cnt<n )
    {
        ae_vector_set_length(x, n, _state);
    }
}


/*************************************************************************
If Cols(X)<N or Rows(X)<M, resizes X

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void rmatrixsetlengthatleast(/* Real    */ ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state)
{


    if( m>0&&n>0 )
    {
        if( x->rows<m||x->cols<n )
        {
            ae_matrix_set_length(x, m, n, _state);
        }
    }
}


/*************************************************************************
If Cols(X)<N or Rows(X)<M, resizes X

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void bmatrixsetlengthatleast(/* Boolean */ ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state)
{


    if( m>0&&n>0 )
    {
        if( x->rows<m||x->cols<n )
        {
            ae_matrix_set_length(x, m, n, _state);
        }
    }
}


/*************************************************************************
Grows X, i.e. changes its size in such a way that:
a) contents is preserved
b) new size is at least N
c) new size can be larger than N, so subsequent grow() calls can return
   without reallocation

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void bvectorgrowto(/* Boolean */ ae_vector* x,
     ae_int_t n,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector oldx;
    ae_int_t i;
    ae_int_t n2;

    ae_frame_make(_state, &_frame_block);
    memset(&oldx, 0, sizeof(oldx));
    ae_vector_init(&oldx, 0, DT_BOOL, _state, ae_true);

    
    /*
     * Enough place
     */
    if( x->cnt>=n )
    {
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Choose new size
     */
    n = ae_maxint(n, ae_round(1.8*(double)x->cnt+(double)1, _state), _state);
    
    /*
     * Grow
     */
    n2 = x->cnt;
    ae_swap_vectors(x, &oldx);
    ae_vector_set_length(x, n, _state);
    for(i=0; i<=n-1; i++)
    {
        if( i<n2 )
        {
            x->ptr.p_bool[i] = oldx.ptr.p_bool[i];
        }
        else
        {
            x->ptr.p_bool[i] = ae_false;
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Grows X, i.e. changes its size in such a way that:
a) contents is preserved
b) new size is at least N
c) new size can be larger than N, so subsequent grow() calls can return
   without reallocation

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void ivectorgrowto(/* Integer */ ae_vector* x,
     ae_int_t n,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector oldx;
    ae_int_t i;
    ae_int_t n2;

    ae_frame_make(_state, &_frame_block);
    memset(&oldx, 0, sizeof(oldx));
    ae_vector_init(&oldx, 0, DT_INT, _state, ae_true);

    
    /*
     * Enough place
     */
    if( x->cnt>=n )
    {
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Choose new size
     */
    n = ae_maxint(n, ae_round(1.8*(double)x->cnt+(double)1, _state), _state);
    
    /*
     * Grow
     */
    n2 = x->cnt;
    ae_swap_vectors(x, &oldx);
    ae_vector_set_length(x, n, _state);
    for(i=0; i<=n-1; i++)
    {
        if( i<n2 )
        {
            x->ptr.p_int[i] = oldx.ptr.p_int[i];
        }
        else
        {
            x->ptr.p_int[i] = 0;
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Grows X, i.e. appends rows in such a way that:
a) contents is preserved
b) new row count is at least N
c) new row count can be larger than N, so subsequent grow() calls can return
   without reallocation
d) new matrix has at least MinCols columns (if less than specified amount
   of columns is present, new columns are added with undefined contents);
   MinCols can be 0 or negative value = ignored

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void rmatrixgrowrowsto(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_int_t mincols,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix olda;
    ae_int_t i;
    ae_int_t j;
    ae_int_t n2;
    ae_int_t m;

    ae_frame_make(_state, &_frame_block);
    memset(&olda, 0, sizeof(olda));
    ae_matrix_init(&olda, 0, 0, DT_REAL, _state, ae_true);

    
    /*
     * Enough place?
     */
    if( a->rows>=n&&a->cols>=mincols )
    {
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Sizes and metrics
     */
    if( a->rows<n )
    {
        n = ae_maxint(n, ae_round(1.8*(double)a->rows+(double)1, _state), _state);
    }
    n2 = ae_minint(a->rows, n, _state);
    m = a->cols;
    
    /*
     * Grow
     */
    ae_swap_matrices(a, &olda);
    ae_matrix_set_length(a, n, ae_maxint(m, mincols, _state), _state);
    for(i=0; i<=n2-1; i++)
    {
        for(j=0; j<=m-1; j++)
        {
            a->ptr.pp_double[i][j] = olda.ptr.pp_double[i][j];
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Grows X, i.e. appends cols in such a way that:
a) contents is preserved
b) new col count is at least N
c) new col count can be larger than N, so subsequent grow() calls can return
   without reallocation
d) new matrix has at least MinRows row (if less than specified amount
   of rows is present, new rows are added with undefined contents);
   MinRows can be 0 or negative value = ignored

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void rmatrixgrowcolsto(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_int_t minrows,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix olda;
    ae_int_t i;
    ae_int_t j;
    ae_int_t n2;
    ae_int_t m;

    ae_frame_make(_state, &_frame_block);
    memset(&olda, 0, sizeof(olda));
    ae_matrix_init(&olda, 0, 0, DT_REAL, _state, ae_true);

    
    /*
     * Enough place?
     */
    if( a->cols>=n&&a->rows>=minrows )
    {
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Sizes and metrics
     */
    if( a->cols<n )
    {
        n = ae_maxint(n, ae_round(1.8*(double)a->cols+(double)1, _state), _state);
    }
    n2 = ae_minint(a->cols, n, _state);
    m = a->rows;
    
    /*
     * Grow
     */
    ae_swap_matrices(a, &olda);
    ae_matrix_set_length(a, ae_maxint(m, minrows, _state), n, _state);
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n2-1; j++)
        {
            a->ptr.pp_double[i][j] = olda.ptr.pp_double[i][j];
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Grows X, i.e. changes its size in such a way that:
a) contents is preserved
b) new size is at least N
c) new size can be larger than N, so subsequent grow() calls can return
   without reallocation

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void rvectorgrowto(/* Real    */ ae_vector* x,
     ae_int_t n,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector oldx;
    ae_int_t i;
    ae_int_t n2;

    ae_frame_make(_state, &_frame_block);
    memset(&oldx, 0, sizeof(oldx));
    ae_vector_init(&oldx, 0, DT_REAL, _state, ae_true);

    
    /*
     * Enough place
     */
    if( x->cnt>=n )
    {
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Choose new size
     */
    n = ae_maxint(n, ae_round(1.8*(double)x->cnt+(double)1, _state), _state);
    
    /*
     * Grow
     */
    n2 = x->cnt;
    ae_swap_vectors(x, &oldx);
    ae_vector_set_length(x, n, _state);
    for(i=0; i<=n-1; i++)
    {
        if( i<n2 )
        {
            x->ptr.p_double[i] = oldx.ptr.p_double[i];
        }
        else
        {
            x->ptr.p_double[i] = (double)(0);
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Resizes X and:
* preserves old contents of X
* fills new elements by zeros

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void ivectorresize(/* Integer */ ae_vector* x,
     ae_int_t n,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector oldx;
    ae_int_t i;
    ae_int_t n2;

    ae_frame_make(_state, &_frame_block);
    memset(&oldx, 0, sizeof(oldx));
    ae_vector_init(&oldx, 0, DT_INT, _state, ae_true);

    n2 = x->cnt;
    ae_swap_vectors(x, &oldx);
    ae_vector_set_length(x, n, _state);
    for(i=0; i<=n-1; i++)
    {
        if( i<n2 )
        {
            x->ptr.p_int[i] = oldx.ptr.p_int[i];
        }
        else
        {
            x->ptr.p_int[i] = 0;
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Resizes X and:
* preserves old contents of X
* fills new elements by zeros

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void rvectorresize(/* Real    */ ae_vector* x,
     ae_int_t n,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector oldx;
    ae_int_t i;
    ae_int_t n2;

    ae_frame_make(_state, &_frame_block);
    memset(&oldx, 0, sizeof(oldx));
    ae_vector_init(&oldx, 0, DT_REAL, _state, ae_true);

    n2 = x->cnt;
    ae_swap_vectors(x, &oldx);
    ae_vector_set_length(x, n, _state);
    for(i=0; i<=n-1; i++)
    {
        if( i<n2 )
        {
            x->ptr.p_double[i] = oldx.ptr.p_double[i];
        }
        else
        {
            x->ptr.p_double[i] = (double)(0);
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Resizes X and:
* preserves old contents of X
* fills new elements by zeros

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void rmatrixresize(/* Real    */ ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix oldx;
    ae_int_t i;
    ae_int_t j;
    ae_int_t m2;
    ae_int_t n2;

    ae_frame_make(_state, &_frame_block);
    memset(&oldx, 0, sizeof(oldx));
    ae_matrix_init(&oldx, 0, 0, DT_REAL, _state, ae_true);

    m2 = x->rows;
    n2 = x->cols;
    ae_swap_matrices(x, &oldx);
    ae_matrix_set_length(x, m, n, _state);
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            if( i<m2&&j<n2 )
            {
                x->ptr.pp_double[i][j] = oldx.ptr.pp_double[i][j];
            }
            else
            {
                x->ptr.pp_double[i][j] = 0.0;
            }
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Resizes X and:
* preserves old contents of X
* fills new elements by zeros

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void imatrixresize(/* Integer */ ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix oldx;
    ae_int_t i;
    ae_int_t j;
    ae_int_t m2;
    ae_int_t n2;

    ae_frame_make(_state, &_frame_block);
    memset(&oldx, 0, sizeof(oldx));
    ae_matrix_init(&oldx, 0, 0, DT_INT, _state, ae_true);

    m2 = x->rows;
    n2 = x->cols;
    ae_swap_matrices(x, &oldx);
    ae_matrix_set_length(x, m, n, _state);
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            if( i<m2&&j<n2 )
            {
                x->ptr.pp_int[i][j] = oldx.ptr.pp_int[i][j];
            }
            else
            {
                x->ptr.pp_int[i][j] = 0;
            }
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
appends element to X

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void ivectorappend(/* Integer */ ae_vector* x,
     ae_int_t v,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector oldx;
    ae_int_t i;
    ae_int_t n;

    ae_frame_make(_state, &_frame_block);
    memset(&oldx, 0, sizeof(oldx));
    ae_vector_init(&oldx, 0, DT_INT, _state, ae_true);

    n = x->cnt;
    ae_swap_vectors(x, &oldx);
    ae_vector_set_length(x, n+1, _state);
    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_int[i] = oldx.ptr.p_int[i];
    }
    x->ptr.p_int[n] = v;
    ae_frame_leave(_state);
}


/*************************************************************************
This function checks that length(X) is at least N and first N values  from
X[] are finite

  -- ALGLIB --
     Copyright 18.06.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool isfinitevector(/* Real    */ const ae_vector* x,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;
    double v;
    ae_bool result;


    ae_assert(n>=0, "APSERVIsFiniteVector: internal error (N<0)", _state);
    if( n==0 )
    {
        result = ae_true;
        return result;
    }
    if( x->cnt<n )
    {
        result = ae_false;
        return result;
    }
    v = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        v = 0.01*v+x->ptr.p_double[i];
    }
    result = ae_isfinite(v, _state);
    return result;
}


/*************************************************************************
This function checks that length(X) is at least N and first N values  from
X[] are finite or NANs

  -- ALGLIB --
     Copyright 18.06.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool isfiniteornanvector(/* Real    */ const ae_vector* x,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;
    double v;
    ae_bool result;


    ae_assert(n>=0, "APSERVIsFiniteVector: internal error (N<0)", _state);
    if( n==0 )
    {
        result = ae_true;
        return result;
    }
    if( x->cnt<n )
    {
        result = ae_false;
        return result;
    }
    
    /*
     * Is it entirely finite?
     */
    v = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        v = 0.01*v+x->ptr.p_double[i];
    }
    if( ae_isfinite(v, _state) )
    {
        result = ae_true;
        return result;
    }
    
    /*
     * OK, check that either finite or nan
     */
    for(i=0; i<=n-1; i++)
    {
        if( !ae_isfinite(x->ptr.p_double[i], _state)&&!ae_isnan(x->ptr.p_double[i], _state) )
        {
            result = ae_false;
            return result;
        }
    }
    result = ae_true;
    return result;
}


/*************************************************************************
This function checks that first N values from X[] are finite

  -- ALGLIB --
     Copyright 18.06.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool isfinitecvector(/* Complex */ const ae_vector* z,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;
    ae_bool result;


    ae_assert(n>=0, "APSERVIsFiniteCVector: internal error (N<0)", _state);
    for(i=0; i<=n-1; i++)
    {
        if( !ae_isfinite(z->ptr.p_complex[i].x, _state)||!ae_isfinite(z->ptr.p_complex[i].y, _state) )
        {
            result = ae_false;
            return result;
        }
    }
    result = ae_true;
    return result;
}


/*************************************************************************
This function checks that size of X is at least MxN and values from
X[0..M-1,0..N-1] are finite.

  -- ALGLIB --
     Copyright 18.06.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool apservisfinitematrix(/* Real    */ const ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_bool result;


    ae_assert(n>=0, "APSERVIsFiniteMatrix: internal error (N<0)", _state);
    ae_assert(m>=0, "APSERVIsFiniteMatrix: internal error (M<0)", _state);
    if( m==0||n==0 )
    {
        result = ae_true;
        return result;
    }
    if( x->rows<m||x->cols<n )
    {
        result = ae_false;
        return result;
    }
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            if( !ae_isfinite(x->ptr.pp_double[i][j], _state) )
            {
                result = ae_false;
                return result;
            }
        }
    }
    result = ae_true;
    return result;
}


/*************************************************************************
This function checks that all values from X[0..M-1,0..N-1] are finite

  -- ALGLIB --
     Copyright 18.06.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool apservisfinitecmatrix(/* Complex */ const ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_bool result;


    ae_assert(n>=0, "APSERVIsFiniteCMatrix: internal error (N<0)", _state);
    ae_assert(m>=0, "APSERVIsFiniteCMatrix: internal error (M<0)", _state);
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            if( !ae_isfinite(x->ptr.pp_complex[i][j].x, _state)||!ae_isfinite(x->ptr.pp_complex[i][j].y, _state) )
            {
                result = ae_false;
                return result;
            }
        }
    }
    result = ae_true;
    return result;
}


/*************************************************************************
This function checks that all values from X[0..M-1,0..N-1] are finite

  -- ALGLIB --
     Copyright 18.06.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool isfinitecmatrix(/* Complex */ const ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_bool result;


    ae_assert(n>=0, "IsFiniteCMatrix: internal error (N<0)", _state);
    ae_assert(m>=0, "IsFiniteCMatrix: internal error (M<0)", _state);
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            if( !ae_isfinite(x->ptr.pp_complex[i][j].x, _state)||!ae_isfinite(x->ptr.pp_complex[i][j].y, _state) )
            {
                result = ae_false;
                return result;
            }
        }
    }
    result = ae_true;
    return result;
}


/*************************************************************************
This function checks that size of X is at least NxN and all values from
upper/lower triangle of X[0..N-1,0..N-1] are finite

  -- ALGLIB --
     Copyright 18.06.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool isfinitertrmatrix(/* Real    */ const ae_matrix* x,
     ae_int_t n,
     ae_bool isupper,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j1;
    ae_int_t j2;
    ae_int_t j;
    ae_bool result;


    ae_assert(n>=0, "APSERVIsFiniteRTRMatrix: internal error (N<0)", _state);
    if( n==0 )
    {
        result = ae_true;
        return result;
    }
    if( x->rows<n||x->cols<n )
    {
        result = ae_false;
        return result;
    }
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
            if( !ae_isfinite(x->ptr.pp_double[i][j], _state) )
            {
                result = ae_false;
                return result;
            }
        }
    }
    result = ae_true;
    return result;
}


/*************************************************************************
This function checks that all values from upper/lower triangle of
X[0..N-1,0..N-1] are finite

  -- ALGLIB --
     Copyright 18.06.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool apservisfinitectrmatrix(/* Complex */ const ae_matrix* x,
     ae_int_t n,
     ae_bool isupper,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j1;
    ae_int_t j2;
    ae_int_t j;
    ae_bool result;


    ae_assert(n>=0, "APSERVIsFiniteCTRMatrix: internal error (N<0)", _state);
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
            if( !ae_isfinite(x->ptr.pp_complex[i][j].x, _state)||!ae_isfinite(x->ptr.pp_complex[i][j].y, _state) )
            {
                result = ae_false;
                return result;
            }
        }
    }
    result = ae_true;
    return result;
}


/*************************************************************************
This function checks that all values from upper/lower triangle of
X[0..N-1,0..N-1] are finite

  -- ALGLIB --
     Copyright 18.06.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool isfinitectrmatrix(/* Complex */ const ae_matrix* x,
     ae_int_t n,
     ae_bool isupper,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j1;
    ae_int_t j2;
    ae_int_t j;
    ae_bool result;


    ae_assert(n>=0, "APSERVIsFiniteCTRMatrix: internal error (N<0)", _state);
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
            if( !ae_isfinite(x->ptr.pp_complex[i][j].x, _state)||!ae_isfinite(x->ptr.pp_complex[i][j].y, _state) )
            {
                result = ae_false;
                return result;
            }
        }
    }
    result = ae_true;
    return result;
}


/*************************************************************************
This function checks that all values from X[0..M-1,0..N-1] are  finite  or
NaN's.

  -- ALGLIB --
     Copyright 18.06.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool apservisfiniteornanmatrix(/* Real    */ const ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_bool result;


    ae_assert(n>=0, "APSERVIsFiniteOrNaNMatrix: internal error (N<0)", _state);
    ae_assert(m>=0, "APSERVIsFiniteOrNaNMatrix: internal error (M<0)", _state);
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            if( !(ae_isfinite(x->ptr.pp_double[i][j], _state)||ae_isnan(x->ptr.pp_double[i][j], _state)) )
            {
                result = ae_false;
                return result;
            }
        }
    }
    result = ae_true;
    return result;
}


/*************************************************************************
Safe sqrt(x^2+y^2)

  -- ALGLIB --
     Copyright by Bochkanov Sergey
*************************************************************************/
double safepythag2(double x, double y, ae_state *_state)
{
    double w;
    double xabs;
    double yabs;
    double z;
    double result;


    xabs = ae_fabs(x, _state);
    yabs = ae_fabs(y, _state);
    w = ae_maxreal(xabs, yabs, _state);
    z = ae_minreal(xabs, yabs, _state);
    if( ae_fp_eq(z,(double)(0)) )
    {
        result = w;
    }
    else
    {
        result = w*ae_sqrt((double)1+ae_sqr(z/w, _state), _state);
    }
    return result;
}


/*************************************************************************
Safe sqrt(x^2+y^2)

  -- ALGLIB --
     Copyright by Bochkanov Sergey
*************************************************************************/
double safepythag3(double x, double y, double z, ae_state *_state)
{
    double w;
    double result;


    w = ae_maxreal(ae_fabs(x, _state), ae_maxreal(ae_fabs(y, _state), ae_fabs(z, _state), _state), _state);
    if( ae_fp_eq(w,(double)(0)) )
    {
        result = (double)(0);
        return result;
    }
    x = x/w;
    y = y/w;
    z = z/w;
    result = w*ae_sqrt(ae_sqr(x, _state)+ae_sqr(y, _state)+ae_sqr(z, _state), _state);
    return result;
}


/*************************************************************************
Safe division.

This function attempts to calculate R=X/Y without overflow.

It returns:
* +1, if abs(X/Y)>=MaxRealNumber or undefined - overflow-like situation
      (no overlfow is generated, R is either NAN, PosINF, NegINF)
*  0, if MinRealNumber<abs(X/Y)<MaxRealNumber or X=0, Y<>0
      (R contains result, may be zero)
* -1, if 0<abs(X/Y)<MinRealNumber - underflow-like situation
      (R contains zero; it corresponds to underflow)

No overflow is generated in any case.

  -- ALGLIB --
     Copyright by Bochkanov Sergey
*************************************************************************/
ae_int_t saferdiv(double x, double y, double* r, ae_state *_state)
{
    ae_int_t result;

    *r = 0.0;

    
    /*
     * Two special cases:
     * * Y=0
     * * X=0 and Y<>0
     */
    if( ae_fp_eq(y,(double)(0)) )
    {
        result = 1;
        if( ae_fp_eq(x,(double)(0)) )
        {
            *r = _state->v_nan;
        }
        if( ae_fp_greater(x,(double)(0)) )
        {
            *r = _state->v_posinf;
        }
        if( ae_fp_less(x,(double)(0)) )
        {
            *r = _state->v_neginf;
        }
        return result;
    }
    if( ae_fp_eq(x,(double)(0)) )
    {
        *r = (double)(0);
        result = 0;
        return result;
    }
    
    /*
     * make Y>0
     */
    if( ae_fp_less(y,(double)(0)) )
    {
        x = -x;
        y = -y;
    }
    
    /*
     *
     */
    if( ae_fp_greater_eq(y,(double)(1)) )
    {
        *r = x/y;
        if( ae_fp_less_eq(ae_fabs(*r, _state),ae_minrealnumber) )
        {
            result = -1;
            *r = (double)(0);
        }
        else
        {
            result = 0;
        }
    }
    else
    {
        if( ae_fp_greater_eq(ae_fabs(x, _state),ae_maxrealnumber*y) )
        {
            if( ae_fp_greater(x,(double)(0)) )
            {
                *r = _state->v_posinf;
            }
            else
            {
                *r = _state->v_neginf;
            }
            result = 1;
        }
        else
        {
            *r = x/y;
            result = 0;
        }
    }
    return result;
}


/*************************************************************************
This function calculates "safe" min(X/Y,V) for positive finite X, Y, V.
No overflow is generated in any case.

  -- ALGLIB --
     Copyright by Bochkanov Sergey
*************************************************************************/
double safeminposrv(double x, double y, double v, ae_state *_state)
{
    double r;
    double result;


    if( y>=(double)1 )
    {
        
        /*
         * Y>=1, we can safely divide by Y
         */
        r = x/y;
        result = v;
        if( v>r )
        {
            result = r;
        }
        else
        {
            result = v;
        }
    }
    else
    {
        
        /*
         * Y<1, we can safely multiply by Y
         */
        if( x<v*y )
        {
            result = x/y;
        }
        else
        {
            result = v;
        }
    }
    return result;
}


/*************************************************************************
This function makes periodic mapping of X to [A,B].

It accepts X, A, B (A>B). It returns T which lies in  [A,B] and integer K,
such that X = T + K*(B-A).

NOTES:
* K is represented as real value, although actually it is integer
* T is guaranteed to be in [A,B]
* T replaces X

  -- ALGLIB --
     Copyright by Bochkanov Sergey
*************************************************************************/
void apperiodicmap(double* x,
     double a,
     double b,
     double* k,
     ae_state *_state)
{

    *k = 0.0;

    ae_assert(ae_fp_less(a,b), "APPeriodicMap: internal error!", _state);
    *k = (double)(ae_ifloor((*x-a)/(b-a), _state));
    *x = *x-*k*(b-a);
    while(ae_fp_less(*x,a))
    {
        *x = *x+(b-a);
        *k = *k-(double)1;
    }
    while(ae_fp_greater(*x,b))
    {
        *x = *x-(b-a);
        *k = *k+(double)1;
    }
    *x = ae_maxreal(*x, a, _state);
    *x = ae_minreal(*x, b, _state);
}


/*************************************************************************
Returns random normal number using low-quality system-provided generator

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
double randomnormal(ae_state *_state)
{
    double u;
    double v;
    double s;
    double result;


    for(;;)
    {
        u = (double)2*ae_randomreal(_state)-(double)1;
        v = (double)2*ae_randomreal(_state)-(double)1;
        s = ae_sqr(u, _state)+ae_sqr(v, _state);
        if( ae_fp_greater(s,(double)(0))&&ae_fp_less(s,(double)(1)) )
        {
            
            /*
             * two Sqrt's instead of one to
             * avoid overflow when S is too small
             */
            s = ae_sqrt(-(double)2*ae_log(s, _state), _state)/ae_sqrt(s, _state);
            result = u*s;
            break;
        }
    }
    return result;
}


/*************************************************************************
Generates random unit vector using low-quality system-provided generator.
Reallocates array if its size is too short.

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void randomunit(ae_int_t n, /* Real    */ ae_vector* x, ae_state *_state)
{
    ae_int_t i;
    double v;
    double vv;


    ae_assert(n>0, "RandomUnit: N<=0", _state);
    if( x->cnt<n )
    {
        ae_vector_set_length(x, n, _state);
    }
    do
    {
        v = 0.0;
        for(i=0; i<=n-1; i++)
        {
            vv = randomnormal(_state);
            x->ptr.p_double[i] = vv;
            v = v+vv*vv;
        }
    }
    while(ae_fp_less_eq(v,(double)(0)));
    v = (double)1/ae_sqrt(v, _state);
    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = x->ptr.p_double[i]*v;
    }
}


/*************************************************************************
This function is used to swap two integer values
*************************************************************************/
void swapi(ae_int_t* v0, ae_int_t* v1, ae_state *_state)
{
    ae_int_t v;


    v = *v0;
    *v0 = *v1;
    *v1 = v;
}


/*************************************************************************
This function is used to swap two real values
*************************************************************************/
void swapr(double* v0, double* v1, ae_state *_state)
{
    double v;


    v = *v0;
    *v0 = *v1;
    *v1 = v;
}


/*************************************************************************
This function is used to swap two rows of the matrix; if NCols<0, automatically
determined from the matrix size.
*************************************************************************/
void swaprows(/* Real    */ ae_matrix* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_int_t ncols,
     ae_state *_state)
{
    ae_int_t j;
    double v;


    if( i0==i1 )
    {
        return;
    }
    if( ncols<0 )
    {
        ncols = a->cols;
    }
    for(j=0; j<=ncols-1; j++)
    {
        v = a->ptr.pp_double[i0][j];
        a->ptr.pp_double[i0][j] = a->ptr.pp_double[i1][j];
        a->ptr.pp_double[i1][j] = v;
    }
}


/*************************************************************************
This function is used to swap two cols of the matrix; if NRows<0, automatically
determined from the matrix size.
*************************************************************************/
void swapcols(/* Real    */ ae_matrix* a,
     ae_int_t j0,
     ae_int_t j1,
     ae_int_t nrows,
     ae_state *_state)
{
    ae_int_t i;
    double v;


    if( j0==j1 )
    {
        return;
    }
    if( nrows<0 )
    {
        nrows = a->rows;
    }
    for(i=0; i<=nrows-1; i++)
    {
        v = a->ptr.pp_double[i][j0];
        a->ptr.pp_double[i][j0] = a->ptr.pp_double[i][j1];
        a->ptr.pp_double[i][j1] = v;
    }
}


/*************************************************************************
This function is used to swap two "entries" in 1-dimensional array composed
from D-element entries
*************************************************************************/
void swapentries(/* Real    */ ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_int_t entrywidth,
     ae_state *_state)
{
    ae_int_t offs0;
    ae_int_t offs1;
    ae_int_t j;
    double v;


    if( i0==i1 )
    {
        return;
    }
    offs0 = i0*entrywidth;
    offs1 = i1*entrywidth;
    for(j=0; j<=entrywidth-1; j++)
    {
        v = a->ptr.p_double[offs0+j];
        a->ptr.p_double[offs0+j] = a->ptr.p_double[offs1+j];
        a->ptr.p_double[offs1+j] = v;
    }
}


/*************************************************************************
This function is used to swap two "entries" in 1-dimensional array composed
from D-element entries
*************************************************************************/
void swapentriesb(/* Boolean */ ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_int_t entrywidth,
     ae_state *_state)
{
    ae_int_t offs0;
    ae_int_t offs1;
    ae_int_t j;
    ae_bool v;


    if( i0==i1 )
    {
        return;
    }
    offs0 = i0*entrywidth;
    offs1 = i1*entrywidth;
    for(j=0; j<=entrywidth-1; j++)
    {
        v = a->ptr.p_bool[offs0+j];
        a->ptr.p_bool[offs0+j] = a->ptr.p_bool[offs1+j];
        a->ptr.p_bool[offs1+j] = v;
    }
}


/*************************************************************************
This function is used to swap two elements of the vector
*************************************************************************/
void swapelements(/* Real    */ ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_state *_state)
{
    double v;


    if( i0==i1 )
    {
        return;
    }
    v = a->ptr.p_double[i0];
    a->ptr.p_double[i0] = a->ptr.p_double[i1];
    a->ptr.p_double[i1] = v;
}


/*************************************************************************
This function is used to swap two elements of the vector
*************************************************************************/
void swapelementsi(/* Integer */ ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_state *_state)
{
    ae_int_t v;


    if( i0==i1 )
    {
        return;
    }
    v = a->ptr.p_int[i0];
    a->ptr.p_int[i0] = a->ptr.p_int[i1];
    a->ptr.p_int[i1] = v;
}


/*************************************************************************
This function is used to return maximum of three real values
*************************************************************************/
double maxreal3(double v0, double v1, double v2, ae_state *_state)
{
    double result;


    result = v0;
    if( ae_fp_less(result,v1) )
    {
        result = v1;
    }
    if( ae_fp_less(result,v2) )
    {
        result = v2;
    }
    return result;
}


/*************************************************************************
This function is used to increment value of integer variable
*************************************************************************/
void inc(ae_int_t* v, ae_state *_state)
{


    *v = *v+1;
}


/*************************************************************************
This function is used to decrement value of integer variable
*************************************************************************/
void dec(ae_int_t* v, ae_state *_state)
{


    *v = *v-1;
}


/*************************************************************************
This function is used to increment value of integer variable; name of  the
function suggests that increment is done in multithreaded setting  in  the
thread-unsafe manner (optional progress reports which do not need guaranteed
correctness)
*************************************************************************/
void threadunsafeinc(ae_int_t* v, ae_state *_state)
{


    *v = *v+1;
}


/*************************************************************************
This function is used to increment value of integer variable; name of  the
function suggests that increment is done in multithreaded setting  in  the
thread-unsafe manner (optional progress reports which do not need guaranteed
correctness)
*************************************************************************/
void threadunsafeincby(ae_int_t* v, ae_int_t k, ae_state *_state)
{


    *v = *v+k;
}


/*************************************************************************
This function performs two operations:
1. decrements value of integer variable, if it is positive
2. explicitly sets variable to zero if it is non-positive
It is used by some algorithms to decrease value of internal counters.
*************************************************************************/
void countdown(ae_int_t* v, ae_state *_state)
{


    if( *v>0 )
    {
        *v = *v-1;
    }
    else
    {
        *v = 0;
    }
}


/*************************************************************************
This function returns +1 or -1 depending on sign of X.
x=0 results in +1 being returned.
*************************************************************************/
double possign(double x, ae_state *_state)
{
    double result;


    if( ae_fp_greater_eq(x,(double)(0)) )
    {
        result = (double)(1);
    }
    else
    {
        result = (double)(-1);
    }
    return result;
}


/*************************************************************************
This function returns product of two real numbers. It is convenient when
you have to perform typecast-and-product of two INTEGERS.
*************************************************************************/
double rmul2(double v0, double v1, ae_state *_state)
{
    double result;


    result = v0*v1;
    return result;
}


/*************************************************************************
This function returns product of three real numbers. It is convenient when
you have to perform typecast-and-product of three INTEGERS.
*************************************************************************/
double rmul3(double v0, double v1, double v2, ae_state *_state)
{
    double result;


    result = v0*v1*v2;
    return result;
}


/*************************************************************************
This function returns product of four real numbers. It is convenient when
you have to perform typecast-and-product of four INTEGERS.
*************************************************************************/
double rmul4(double v0, double v1, double v2, double v3, ae_state *_state)
{
    double result;


    result = v0*v1*v2*v3;
    return result;
}


/*************************************************************************
This function returns (A div B) rounded up; it expects that A>0, B>0, but
does not check it.
*************************************************************************/
ae_int_t idivup(ae_int_t a, ae_int_t b, ae_state *_state)
{
    ae_int_t result;


    result = a/b;
    if( a%b>0 )
    {
        result = result+1;
    }
    return result;
}


/*************************************************************************
This function returns min(i0,i1)
*************************************************************************/
ae_int_t imin2(ae_int_t i0, ae_int_t i1, ae_state *_state)
{
    ae_int_t result;


    result = i0;
    if( i1<result )
    {
        result = i1;
    }
    return result;
}


/*************************************************************************
This function returns min(i0,i1,i2)
*************************************************************************/
ae_int_t imin3(ae_int_t i0, ae_int_t i1, ae_int_t i2, ae_state *_state)
{
    ae_int_t result;


    result = i0;
    if( i1<result )
    {
        result = i1;
    }
    if( i2<result )
    {
        result = i2;
    }
    return result;
}


/*************************************************************************
This function returns max(i0,i1)
*************************************************************************/
ae_int_t imax2(ae_int_t i0, ae_int_t i1, ae_state *_state)
{
    ae_int_t result;


    result = i0;
    if( i1>result )
    {
        result = i1;
    }
    return result;
}


/*************************************************************************
This function returns max(i0,i1,i2)
*************************************************************************/
ae_int_t imax3(ae_int_t i0, ae_int_t i1, ae_int_t i2, ae_state *_state)
{
    ae_int_t result;


    result = i0;
    if( i1>result )
    {
        result = i1;
    }
    if( i2>result )
    {
        result = i2;
    }
    return result;
}


/*************************************************************************
This function returns max(r0,r1,r2)
*************************************************************************/
double rmax3(double r0, double r1, double r2, ae_state *_state)
{
    double result;


    result = r0;
    if( ae_fp_greater(r1,result) )
    {
        result = r1;
    }
    if( ae_fp_greater(r2,result) )
    {
        result = r2;
    }
    return result;
}


/*************************************************************************
This function returns max(|r0|,|r1|,|r2|)
*************************************************************************/
double rmaxabs3(double r0, double r1, double r2, ae_state *_state)
{
    double result;


    r0 = ae_fabs(r0, _state);
    r1 = ae_fabs(r1, _state);
    r2 = ae_fabs(r2, _state);
    result = r0;
    if( ae_fp_greater(r1,result) )
    {
        result = r1;
    }
    if( ae_fp_greater(r2,result) )
    {
        result = r2;
    }
    return result;
}


/*************************************************************************
'bounds' value: maps X to [B1,B2]

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
double boundval(double x, double b1, double b2, ae_state *_state)
{
    double result;


    if( ae_fp_less_eq(x,b1) )
    {
        result = b1;
        return result;
    }
    if( ae_fp_greater_eq(x,b2) )
    {
        result = b2;
        return result;
    }
    result = x;
    return result;
}


/*************************************************************************
'bounds' value: maps X to [B1,B2]

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
ae_int_t iboundval(ae_int_t x, ae_int_t b1, ae_int_t b2, ae_state *_state)
{
    ae_int_t result;


    if( x<=b1 )
    {
        result = b1;
        return result;
    }
    if( x>=b2 )
    {
        result = b2;
        return result;
    }
    result = x;
    return result;
}


/*************************************************************************
'bounds' value: maps X to [B1,B2]

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
double rboundval(double x, double b1, double b2, ae_state *_state)
{
    double result;


    if( ae_fp_less_eq(x,b1) )
    {
        result = b1;
        return result;
    }
    if( ae_fp_greater_eq(x,b2) )
    {
        result = b2;
        return result;
    }
    result = x;
    return result;
}


/*************************************************************************
Boolean case-2: returns V0 if Cond=True, V1 otherwise
*************************************************************************/
ae_bool bcase2(ae_bool cond, ae_bool v0, ae_bool v1, ae_state *_state)
{
    ae_bool result;


    if( cond )
    {
        result = v0;
    }
    else
    {
        result = v1;
    }
    return result;
}


/*************************************************************************
Integer case-2: returns V0 if Cond=True, V1 otherwise
*************************************************************************/
ae_int_t icase2(ae_bool cond, ae_int_t v0, ae_int_t v1, ae_state *_state)
{
    ae_int_t result;


    if( cond )
    {
        result = v0;
    }
    else
    {
        result = v1;
    }
    return result;
}


/*************************************************************************
Real case-2: returns V0 if Cond=True, V1 otherwise
*************************************************************************/
double rcase2(ae_bool cond, double v0, double v1, ae_state *_state)
{
    double result;


    if( cond )
    {
        result = v0;
    }
    else
    {
        result = v1;
    }
    return result;
}


/*************************************************************************
Returns number of non-zeros
*************************************************************************/
ae_int_t countnz1(/* Real    */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t result;


    result = 0;
    for(i=0; i<=n-1; i++)
    {
        if( !(v->ptr.p_double[i]==(double)0) )
        {
            result = result+1;
        }
    }
    return result;
}


/*************************************************************************
Returns number of non-zeros
*************************************************************************/
ae_int_t countnz2(/* Real    */ const ae_matrix* v,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t result;


    result = 0;
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            if( !(v->ptr.pp_double[i][j]==(double)0) )
            {
                result = result+1;
            }
        }
    }
    return result;
}


/*************************************************************************
Allocation of serializer: complex value
*************************************************************************/
void alloccomplex(ae_serializer* s, ae_complex v, ae_state *_state)
{


    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
}


/*************************************************************************
Serialization: complex value
*************************************************************************/
void serializecomplex(ae_serializer* s, ae_complex v, ae_state *_state)
{


    ae_serializer_serialize_double(s, v.x, _state);
    ae_serializer_serialize_double(s, v.y, _state);
}


/*************************************************************************
Unserialization: complex value
*************************************************************************/
ae_complex unserializecomplex(ae_serializer* s, ae_state *_state)
{
    ae_complex result;


    ae_serializer_unserialize_double(s, &result.x, _state);
    ae_serializer_unserialize_double(s, &result.y, _state);
    return result;
}


/*************************************************************************
Allocation of serializer: real array
*************************************************************************/
void allocrealarray(ae_serializer* s,
     /* Real    */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;


    if( n<0 )
    {
        n = v->cnt;
    }
    ae_serializer_alloc_entry(s);
    for(i=0; i<=n-1; i++)
    {
        ae_serializer_alloc_entry(s);
    }
}


/*************************************************************************
Allocation of serializer: boolean array
*************************************************************************/
void allocbooleanarray(ae_serializer* s,
     /* Boolean */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;


    if( n<0 )
    {
        n = v->cnt;
    }
    ae_serializer_alloc_entry(s);
    for(i=0; i<=n-1; i++)
    {
        ae_serializer_alloc_entry(s);
    }
}


/*************************************************************************
Serialization: complex value
*************************************************************************/
void serializerealarray(ae_serializer* s,
     /* Real    */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;


    if( n<0 )
    {
        n = v->cnt;
    }
    ae_serializer_serialize_int(s, n, _state);
    for(i=0; i<=n-1; i++)
    {
        ae_serializer_serialize_double(s, v->ptr.p_double[i], _state);
    }
}


/*************************************************************************
Serialization: boolean array
*************************************************************************/
void serializebooleanarray(ae_serializer* s,
     /* Boolean */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;


    if( n<0 )
    {
        n = v->cnt;
    }
    ae_serializer_serialize_int(s, n, _state);
    for(i=0; i<=n-1; i++)
    {
        ae_serializer_serialize_bool(s, v->ptr.p_bool[i], _state);
    }
}


/*************************************************************************
Unserialization: complex value
*************************************************************************/
void unserializerealarray(ae_serializer* s,
     /* Real    */ ae_vector* v,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    double t;

    ae_vector_clear(v);

    ae_serializer_unserialize_int(s, &n, _state);
    if( n==0 )
    {
        return;
    }
    ae_vector_set_length(v, n, _state);
    for(i=0; i<=n-1; i++)
    {
        ae_serializer_unserialize_double(s, &t, _state);
        v->ptr.p_double[i] = t;
    }
}


/*************************************************************************
Unserialization: boolean value
*************************************************************************/
void unserializebooleanarray(ae_serializer* s,
     /* Boolean */ ae_vector* v,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_bool t;

    ae_vector_clear(v);

    ae_serializer_unserialize_int(s, &n, _state);
    if( n==0 )
    {
        return;
    }
    ae_vector_set_length(v, n, _state);
    for(i=0; i<=n-1; i++)
    {
        ae_serializer_unserialize_bool(s, &t, _state);
        v->ptr.p_bool[i] = t;
    }
}


/*************************************************************************
Allocation of serializer: Integer array
*************************************************************************/
void allocintegerarray(ae_serializer* s,
     /* Integer */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;


    if( n<0 )
    {
        n = v->cnt;
    }
    ae_serializer_alloc_entry(s);
    for(i=0; i<=n-1; i++)
    {
        ae_serializer_alloc_entry(s);
    }
}


/*************************************************************************
Serialization: Integer array
*************************************************************************/
void serializeintegerarray(ae_serializer* s,
     /* Integer */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;


    if( n<0 )
    {
        n = v->cnt;
    }
    ae_serializer_serialize_int(s, n, _state);
    for(i=0; i<=n-1; i++)
    {
        ae_serializer_serialize_int(s, v->ptr.p_int[i], _state);
    }
}


/*************************************************************************
Unserialization: complex value
*************************************************************************/
void unserializeintegerarray(ae_serializer* s,
     /* Integer */ ae_vector* v,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t t;

    ae_vector_clear(v);

    ae_serializer_unserialize_int(s, &n, _state);
    if( n==0 )
    {
        return;
    }
    ae_vector_set_length(v, n, _state);
    for(i=0; i<=n-1; i++)
    {
        ae_serializer_unserialize_int(s, &t, _state);
        v->ptr.p_int[i] = t;
    }
}


/*************************************************************************
Allocation of serializer: real matrix
*************************************************************************/
void allocrealmatrix(ae_serializer* s,
     /* Real    */ const ae_matrix* v,
     ae_int_t n0,
     ae_int_t n1,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;


    if( n0<0 )
    {
        n0 = v->rows;
    }
    if( n1<0 )
    {
        n1 = v->cols;
    }
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    for(i=0; i<=n0-1; i++)
    {
        for(j=0; j<=n1-1; j++)
        {
            ae_serializer_alloc_entry(s);
        }
    }
}


/*************************************************************************
Serialization: complex value
*************************************************************************/
void serializerealmatrix(ae_serializer* s,
     /* Real    */ const ae_matrix* v,
     ae_int_t n0,
     ae_int_t n1,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;


    if( n0<0 )
    {
        n0 = v->rows;
    }
    if( n1<0 )
    {
        n1 = v->cols;
    }
    ae_serializer_serialize_int(s, n0, _state);
    ae_serializer_serialize_int(s, n1, _state);
    for(i=0; i<=n0-1; i++)
    {
        for(j=0; j<=n1-1; j++)
        {
            ae_serializer_serialize_double(s, v->ptr.pp_double[i][j], _state);
        }
    }
}


/*************************************************************************
Unserialization: complex value
*************************************************************************/
void unserializerealmatrix(ae_serializer* s,
     /* Real    */ ae_matrix* v,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t n0;
    ae_int_t n1;
    double t;

    ae_matrix_clear(v);

    ae_serializer_unserialize_int(s, &n0, _state);
    ae_serializer_unserialize_int(s, &n1, _state);
    if( n0==0||n1==0 )
    {
        return;
    }
    ae_matrix_set_length(v, n0, n1, _state);
    for(i=0; i<=n0-1; i++)
    {
        for(j=0; j<=n1-1; j++)
        {
            ae_serializer_unserialize_double(s, &t, _state);
            v->ptr.pp_double[i][j] = t;
        }
    }
}


/*************************************************************************
Copy boolean array
*************************************************************************/
void copybooleanarray(/* Boolean */ const ae_vector* src,
     /* Boolean */ ae_vector* dst,
     ae_state *_state)
{
    ae_int_t i;

    ae_vector_clear(dst);

    if( src->cnt>0 )
    {
        ae_vector_set_length(dst, src->cnt, _state);
        for(i=0; i<=src->cnt-1; i++)
        {
            dst->ptr.p_bool[i] = src->ptr.p_bool[i];
        }
    }
}


/*************************************************************************
Copy integer array
*************************************************************************/
void copyintegerarray(/* Integer */ const ae_vector* src,
     /* Integer */ ae_vector* dst,
     ae_state *_state)
{
    ae_int_t i;

    ae_vector_clear(dst);

    if( src->cnt>0 )
    {
        ae_vector_set_length(dst, src->cnt, _state);
        for(i=0; i<=src->cnt-1; i++)
        {
            dst->ptr.p_int[i] = src->ptr.p_int[i];
        }
    }
}


/*************************************************************************
Copy real array
*************************************************************************/
void copyrealarray(/* Real    */ const ae_vector* src,
     /* Real    */ ae_vector* dst,
     ae_state *_state)
{
    ae_int_t i;

    ae_vector_clear(dst);

    if( src->cnt>0 )
    {
        ae_vector_set_length(dst, src->cnt, _state);
        for(i=0; i<=src->cnt-1; i++)
        {
            dst->ptr.p_double[i] = src->ptr.p_double[i];
        }
    }
}


/*************************************************************************
Copy real matrix
*************************************************************************/
void copyrealmatrix(/* Real    */ const ae_matrix* src,
     /* Real    */ ae_matrix* dst,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;

    ae_matrix_clear(dst);

    if( src->rows>0&&src->cols>0 )
    {
        ae_matrix_set_length(dst, src->rows, src->cols, _state);
        for(i=0; i<=src->rows-1; i++)
        {
            for(j=0; j<=src->cols-1; j++)
            {
                dst->ptr.pp_double[i][j] = src->ptr.pp_double[i][j];
            }
        }
    }
}


/*************************************************************************
Clears integer array
*************************************************************************/
void unsetintegerarray(/* Integer */ ae_vector* a, ae_state *_state)
{

    ae_vector_clear(a);

    ae_vector_set_length(a, 0, _state);
}


/*************************************************************************
Clears real array
*************************************************************************/
void unsetrealarray(/* Real    */ ae_vector* a, ae_state *_state)
{

    ae_vector_clear(a);

    ae_vector_set_length(a, 0, _state);
}


/*************************************************************************
Clears real matrix
*************************************************************************/
void unsetrealmatrix(/* Real    */ ae_matrix* a, ae_state *_state)
{

    ae_matrix_clear(a);

    ae_matrix_set_length(a, 0, 0, _state);
}


/*************************************************************************
Initialize nbPool - prepare it to store N-length arrays, N>=0.
Tries to reuse previously allocated memory as much as possible.
*************************************************************************/
void nbpoolinit(nbpool* pool, ae_int_t n, ae_state *_state)
{


    ae_assert(n>=0, "niPoolInit: N<0", _state);
    pool->n = n;
    pool->temporariescount = 0;
    if( n==0 )
    {
        return;
    }
    if( pool->seed0.val.cnt!=0 )
    {
        ae_vector_set_length(&pool->seed0.val, 0, _state);
    }
    if( pool->seedn.val.cnt!=n )
    {
        ae_vector_set_length(&pool->seedn.val, n, _state);
    }
    ae_shared_pool_set_seed(&pool->sourcepool, &pool->seedn, (ae_int_t)sizeof(pool->seedn), (ae_constructor)_sbooleanarray_init, (ae_copy_constructor)_sbooleanarray_init_copy, (ae_destructor)_sbooleanarray_destroy, _state);
    ae_shared_pool_set_seed(&pool->temporarypool, &pool->seed0, (ae_int_t)sizeof(pool->seed0), (ae_constructor)_sbooleanarray_init, (ae_copy_constructor)_sbooleanarray_init_copy, (ae_destructor)_sbooleanarray_destroy, _state);
}


/*************************************************************************
Thread-safe retrieval of array from the nbPool. If there are enough arrays
in the pool, it is performed without additional dynamic allocations.

INPUT PARAMETERS:
    Pool        -   nbPool properly initialized with nbPoolInit
    A           -   array[0], must have exactly zero length (exception will
                    be generated if length is different from zero)
                    
OUTPUT PARAMETERS:
    A           -   array[N], contents undefined
*************************************************************************/
void nbpoolretrieve(nbpool* pool,
     /* Boolean */ ae_vector* a,
     ae_state *_state)
{
    ae_frame _frame_block;
    sbooleanarray *tmp;
    ae_smart_ptr _tmp;

    ae_frame_make(_state, &_frame_block);
    memset(&_tmp, 0, sizeof(_tmp));
    ae_smart_ptr_init(&_tmp, (void**)&tmp, _state, ae_true);

    ae_assert(a->cnt==0, "nbPoolRetrieve: A has non-zero length on entry", _state);
    if( pool->n==0 )
    {
        ae_frame_leave(_state);
        return;
    }
    ae_shared_pool_retrieve(&pool->sourcepool, &_tmp, _state);
    ae_swap_vectors(&tmp->val, a);
    ae_shared_pool_recycle(&pool->temporarypool, &_tmp, _state);
    threadunsafeinc(&pool->temporariescount, _state);
    if( pool->temporariescount>apserv_maxtemporariesinnpool )
    {
        pool->temporariescount = 0;
        ae_shared_pool_clear_recycled(&pool->temporarypool, _state);
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Thread-safe recycling of N-length array to the nbPool.

INPUT PARAMETERS:
    Pool        -   nbPool properly initialized with nbPoolInit
    A           -   array[N], length must be N exactly (exception will
                    be generated if length is different from N)
                    
OUTPUT PARAMETERS:
    A           -   array[0], length is exactly zero on exit
*************************************************************************/
void nbpoolrecycle(nbpool* pool,
     /* Boolean */ ae_vector* a,
     ae_state *_state)
{
    ae_frame _frame_block;
    sbooleanarray *tmp;
    ae_smart_ptr _tmp;

    ae_frame_make(_state, &_frame_block);
    memset(&_tmp, 0, sizeof(_tmp));
    ae_smart_ptr_init(&_tmp, (void**)&tmp, _state, ae_true);

    ae_assert(a->cnt==pool->n, "nbPoolRecycle: A has length<>N on entry", _state);
    if( pool->n==0 )
    {
        ae_frame_leave(_state);
        return;
    }
    ae_shared_pool_retrieve(&pool->temporarypool, &_tmp, _state);
    ae_swap_vectors(&tmp->val, a);
    ae_shared_pool_recycle(&pool->sourcepool, &_tmp, _state);
    threadunsafeincby(&pool->temporariescount, -1, _state);
    if( pool->temporariescount<0 )
    {
        pool->temporariescount = 0;
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Initialize niPool - prepare it to store N-length arrays, N>=0.
Tries to reuse previously allocated memory as much as possible.
*************************************************************************/
void nipoolinit(nipool* pool, ae_int_t n, ae_state *_state)
{


    ae_assert(n>=0, "niPoolInit: N<0", _state);
    pool->n = n;
    pool->temporariescount = 0;
    if( n==0 )
    {
        return;
    }
    if( pool->seed0.val.cnt!=0 )
    {
        ae_vector_set_length(&pool->seed0.val, 0, _state);
    }
    if( pool->seedn.val.cnt!=n )
    {
        ae_vector_set_length(&pool->seedn.val, n, _state);
    }
    ae_shared_pool_set_seed(&pool->sourcepool, &pool->seedn, (ae_int_t)sizeof(pool->seedn), (ae_constructor)_sintegerarray_init, (ae_copy_constructor)_sintegerarray_init_copy, (ae_destructor)_sintegerarray_destroy, _state);
    ae_shared_pool_set_seed(&pool->temporarypool, &pool->seed0, (ae_int_t)sizeof(pool->seed0), (ae_constructor)_sintegerarray_init, (ae_copy_constructor)_sintegerarray_init_copy, (ae_destructor)_sintegerarray_destroy, _state);
}


/*************************************************************************
Thread-safe retrieval of array from the nrPool. If there are enough arrays
in the pool, it is performed without additional dynamic allocations.

INPUT PARAMETERS:
    Pool        -   niPool properly initialized with niPoolInit
    A           -   array[0], must have exactly zero length (exception will
                    be generated if length is different from zero)
                    
OUTPUT PARAMETERS:
    A           -   array[N], contents undefined
*************************************************************************/
void nipoolretrieve(nipool* pool,
     /* Integer */ ae_vector* a,
     ae_state *_state)
{
    ae_frame _frame_block;
    sintegerarray *tmp;
    ae_smart_ptr _tmp;

    ae_frame_make(_state, &_frame_block);
    memset(&_tmp, 0, sizeof(_tmp));
    ae_smart_ptr_init(&_tmp, (void**)&tmp, _state, ae_true);

    ae_assert(a->cnt==0, "niPoolRetrieve: A has non-zero length on entry", _state);
    if( pool->n==0 )
    {
        ae_frame_leave(_state);
        return;
    }
    ae_shared_pool_retrieve(&pool->sourcepool, &_tmp, _state);
    ae_swap_vectors(&tmp->val, a);
    ae_shared_pool_recycle(&pool->temporarypool, &_tmp, _state);
    threadunsafeinc(&pool->temporariescount, _state);
    if( pool->temporariescount>apserv_maxtemporariesinnpool )
    {
        pool->temporariescount = 0;
        ae_shared_pool_clear_recycled(&pool->temporarypool, _state);
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Thread-safe recycling of N-length array to the niPool.

INPUT PARAMETERS:
    Pool        -   niPool properly initialized with niPoolInit
    A           -   array[N], length must be N exactly (exception will
                    be generated if length is different from N)
                    
OUTPUT PARAMETERS:
    A           -   array[0], length is exactly zero on exit
*************************************************************************/
void nipoolrecycle(nipool* pool,
     /* Integer */ ae_vector* a,
     ae_state *_state)
{
    ae_frame _frame_block;
    sintegerarray *tmp;
    ae_smart_ptr _tmp;

    ae_frame_make(_state, &_frame_block);
    memset(&_tmp, 0, sizeof(_tmp));
    ae_smart_ptr_init(&_tmp, (void**)&tmp, _state, ae_true);

    ae_assert(a->cnt==pool->n, "niPoolRecycle: A has length<>N on entry", _state);
    if( pool->n==0 )
    {
        ae_frame_leave(_state);
        return;
    }
    ae_shared_pool_retrieve(&pool->temporarypool, &_tmp, _state);
    ae_swap_vectors(&tmp->val, a);
    ae_shared_pool_recycle(&pool->sourcepool, &_tmp, _state);
    threadunsafeincby(&pool->temporariescount, -1, _state);
    if( pool->temporariescount<0 )
    {
        pool->temporariescount = 0;
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Initialize nrPool - prepare it to store N-length arrays, N>=0.
Tries to reuse previously allocated memory as much as possible.
*************************************************************************/
void nrpoolinit(nrpool* pool, ae_int_t n, ae_state *_state)
{


    ae_assert(n>=0, "nrPoolInit: N<0", _state);
    pool->n = n;
    pool->temporariescount = 0;
    if( n==0 )
    {
        return;
    }
    if( pool->seed0.val.cnt!=0 )
    {
        ae_vector_set_length(&pool->seed0.val, 0, _state);
    }
    if( pool->seedn.val.cnt!=n )
    {
        ae_vector_set_length(&pool->seedn.val, n, _state);
    }
    ae_shared_pool_set_seed(&pool->sourcepool, &pool->seedn, (ae_int_t)sizeof(pool->seedn), (ae_constructor)_srealarray_init, (ae_copy_constructor)_srealarray_init_copy, (ae_destructor)_srealarray_destroy, _state);
    ae_shared_pool_set_seed(&pool->temporarypool, &pool->seed0, (ae_int_t)sizeof(pool->seed0), (ae_constructor)_srealarray_init, (ae_copy_constructor)_srealarray_init_copy, (ae_destructor)_srealarray_destroy, _state);
}


/*************************************************************************
Thread-safe retrieval of array from the nrPool. If there are enough arrays
in the pool, it is performed without additional dynamic allocations.

INPUT PARAMETERS:
    Pool        -   nrPool properly initialized with nrPoolInit
    A           -   array[0], must have exactly zero length (exception will
                    be generated if length is different from zero)
                    
OUTPUT PARAMETERS:
    A           -   array[N], contents undefined
*************************************************************************/
void nrpoolretrieve(nrpool* pool,
     /* Real    */ ae_vector* a,
     ae_state *_state)
{
    ae_frame _frame_block;
    srealarray *tmp;
    ae_smart_ptr _tmp;

    ae_frame_make(_state, &_frame_block);
    memset(&_tmp, 0, sizeof(_tmp));
    ae_smart_ptr_init(&_tmp, (void**)&tmp, _state, ae_true);

    ae_assert(a->cnt==0, "nrPoolRetrieve: A has non-zero length on entry", _state);
    if( pool->n==0 )
    {
        ae_frame_leave(_state);
        return;
    }
    ae_shared_pool_retrieve(&pool->sourcepool, &_tmp, _state);
    ae_swap_vectors(&tmp->val, a);
    ae_shared_pool_recycle(&pool->temporarypool, &_tmp, _state);
    threadunsafeinc(&pool->temporariescount, _state);
    if( pool->temporariescount>apserv_maxtemporariesinnpool )
    {
        pool->temporariescount = 0;
        ae_shared_pool_clear_recycled(&pool->temporarypool, _state);
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Thread-safe recycling of N-length array to the nrPool.

INPUT PARAMETERS:
    Pool        -   nrPool properly initialized with nrPoolInit
    A           -   array[N], length must be N exactly (exception will
                    be generated if length is different from N)
                    
OUTPUT PARAMETERS:
    A           -   array[0], length is exactly zero on exit
*************************************************************************/
void nrpoolrecycle(nrpool* pool,
     /* Real    */ ae_vector* a,
     ae_state *_state)
{
    ae_frame _frame_block;
    srealarray *tmp;
    ae_smart_ptr _tmp;

    ae_frame_make(_state, &_frame_block);
    memset(&_tmp, 0, sizeof(_tmp));
    ae_smart_ptr_init(&_tmp, (void**)&tmp, _state, ae_true);

    ae_assert(a->cnt==pool->n, "nrPoolRecycle: A has length<>N on entry", _state);
    if( pool->n==0 )
    {
        ae_frame_leave(_state);
        return;
    }
    ae_shared_pool_retrieve(&pool->temporarypool, &_tmp, _state);
    ae_swap_vectors(&tmp->val, a);
    ae_shared_pool_recycle(&pool->sourcepool, &_tmp, _state);
    threadunsafeincby(&pool->temporariescount, -1, _state);
    if( pool->temporariescount<0 )
    {
        pool->temporariescount = 0;
    }
    ae_frame_leave(_state);
}


/*************************************************************************
This function is used in parallel functions for recurrent division of large
task into two smaller tasks.

It has following properties:
* it works only for TaskSize>=2 and TaskSize>TileSize (assertion is thrown otherwise)
* Task0+Task1=TaskSize, Task0>0, Task1>0
* Task0 and Task1 are close to each other
* Task0>=Task1
* Task0 is always divisible by TileSize

  -- ALGLIB --
     Copyright 07.04.2013 by Bochkanov Sergey
*************************************************************************/
void tiledsplit(ae_int_t tasksize,
     ae_int_t tilesize,
     ae_int_t* task0,
     ae_int_t* task1,
     ae_state *_state)
{
    ae_int_t cc;

    *task0 = 0;
    *task1 = 0;

    ae_assert(tasksize>=2, "TiledSplit: TaskSize<2", _state);
    ae_assert(tasksize>tilesize, "TiledSplit: TaskSize<=TileSize", _state);
    cc = chunkscount(tasksize, tilesize, _state);
    ae_assert(cc>=2, "TiledSplit: integrity check failed", _state);
    *task0 = idivup(cc, 2, _state)*tilesize;
    *task1 = tasksize-(*task0);
    ae_assert(*task0>=1, "TiledSplit: internal error", _state);
    ae_assert(*task1>=1, "TiledSplit: internal error", _state);
    ae_assert(*task0%tilesize==0, "TiledSplit: internal error", _state);
    ae_assert(*task0>=(*task1), "TiledSplit: internal error", _state);
}


/*************************************************************************
This function searches integer array. Elements in this array are actually
records, each NRec elements wide. Each record has unique header - NHeader
integer values, which identify it. Records are lexicographically sorted by
header.

Records are identified by their index, not offset (offset = NRec*index).

This function searches A (records with indices [I0,I1)) for a record with
header B. It returns index of this record (not offset!), or -1 on failure.

  -- ALGLIB --
     Copyright 28.03.2011 by Bochkanov Sergey
*************************************************************************/
ae_int_t recsearch(/* Integer */ const ae_vector* a,
     ae_int_t nrec,
     ae_int_t nheader,
     ae_int_t i0,
     ae_int_t i1,
     /* Integer */ const ae_vector* b,
     ae_state *_state)
{
    ae_int_t mididx;
    ae_int_t cflag;
    ae_int_t k;
    ae_int_t offs;
    ae_int_t result;


    result = -1;
    for(;;)
    {
        if( i0>=i1 )
        {
            break;
        }
        mididx = (i0+i1)/2;
        offs = nrec*mididx;
        cflag = 0;
        for(k=0; k<=nheader-1; k++)
        {
            if( a->ptr.p_int[offs+k]<b->ptr.p_int[k] )
            {
                cflag = -1;
                break;
            }
            if( a->ptr.p_int[offs+k]>b->ptr.p_int[k] )
            {
                cflag = 1;
                break;
            }
        }
        if( cflag==0 )
        {
            result = mididx;
            return result;
        }
        if( cflag<0 )
        {
            i0 = mididx+1;
        }
        else
        {
            i1 = mididx;
        }
    }
    return result;
}


/*************************************************************************
This function is used in parallel functions for recurrent division of large
task into two smaller tasks.

It has following properties:
* it works only for TaskSize>=2 (assertion is thrown otherwise)
* for TaskSize=2, it returns Task0=1, Task1=1
* in case TaskSize is odd,  Task0=TaskSize-1, Task1=1
* in case TaskSize is even, Task0 and Task1 are approximately TaskSize/2
  and both Task0 and Task1 are even, Task0>=Task1

  -- ALGLIB --
     Copyright 07.04.2013 by Bochkanov Sergey
*************************************************************************/
void splitlengtheven(ae_int_t tasksize,
     ae_int_t* task0,
     ae_int_t* task1,
     ae_state *_state)
{

    *task0 = 0;
    *task1 = 0;

    ae_assert(tasksize>=2, "SplitLengthEven: TaskSize<2", _state);
    if( tasksize==2 )
    {
        *task0 = 1;
        *task1 = 1;
        return;
    }
    if( tasksize%2==0 )
    {
        
        /*
         * Even division
         */
        *task0 = tasksize/2;
        *task1 = tasksize/2;
        if( *task0%2!=0 )
        {
            *task0 = *task0+1;
            *task1 = *task1-1;
        }
    }
    else
    {
        
        /*
         * Odd task size, split trailing odd part from it.
         */
        *task0 = tasksize-1;
        *task1 = 1;
    }
    ae_assert(*task0>=1, "SplitLengthEven: internal error", _state);
    ae_assert(*task1>=1, "SplitLengthEven: internal error", _state);
}


/*************************************************************************
This function is used to calculate number of chunks (including partial,
non-complete chunks) in some set. It expects that ChunkSize>=1, TaskSize>=0.
Assertion is thrown otherwise.

Function result is equivalent to Ceil(TaskSize/ChunkSize), but with guarantees
that rounding errors won't ruin results.

  -- ALGLIB --
     Copyright 21.01.2015 by Bochkanov Sergey
*************************************************************************/
ae_int_t chunkscount(ae_int_t tasksize,
     ae_int_t chunksize,
     ae_state *_state)
{
    ae_int_t result;


    ae_assert(tasksize>=0, "ChunksCount: TaskSize<0", _state);
    ae_assert(chunksize>=1, "ChunksCount: ChunkSize<1", _state);
    result = tasksize/chunksize;
    if( tasksize%chunksize!=0 )
    {
        result = result+1;
    }
    return result;
}


/*************************************************************************
Returns maximum density for level 2 sparse/dense functions. Density values
below one returned by this function are better to handle via sparse Level 2
functionality.

  -- ALGLIB routine --
     10.01.2019
     Bochkanov Sergey
*************************************************************************/
double sparselevel2density(ae_state *_state)
{
    double result;


    result = 0.1;
    return result;
}


/*************************************************************************
Returns A-tile size for a matrix.

A-tiles are smallest tiles (32x32), suitable for processing by ALGLIB  own
implementation of Level 3 linear algebra.

  -- ALGLIB routine --
     10.01.2019
     Bochkanov Sergey
*************************************************************************/
ae_int_t matrixtilesizea(ae_state *_state)
{
    ae_int_t result;


    result = 32;
    return result;
}


/*************************************************************************
Returns B-tile size for a matrix.

B-tiles are larger  tiles (64x64), suitable for parallel execution or for
processing by vendor's implementation of Level 3 linear algebra.

  -- ALGLIB routine --
     10.01.2019
     Bochkanov Sergey
*************************************************************************/
ae_int_t matrixtilesizeb(ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_int_t result;


    result = 64;
    return result;
#else
    return _ialglib_i_matrixtilesizeb();
#endif
}


/*************************************************************************
This function returns minimum cost of task which is feasible for
multithreaded processing. It returns real number in order to avoid overflow
problems.

  -- ALGLIB --
     Copyright 10.01.2018 by Bochkanov Sergey
*************************************************************************/
double smpactivationlevel(ae_state *_state)
{
    double nn;
    double result;


    nn = (double)(2*matrixtilesizeb(_state));
    result = ae_maxreal(0.95*(double)2*nn*nn*nn, 1.0E7, _state);
    return result;
}


/*************************************************************************
This function returns minimum cost of task which is feasible for
spawn (given that multithreading is active).

It returns real number in order to avoid overflow problems.

  -- ALGLIB --
     Copyright 10.01.2018 by Bochkanov Sergey
*************************************************************************/
double spawnlevel(ae_state *_state)
{
    double nn;
    double result;


    nn = (double)(2*matrixtilesizea(_state));
    result = 0.95*(double)2*nn*nn*nn;
    return result;
}


/*************************************************************************
--- OBSOLETE FUNCTION, USE TILED SPLIT INSTEAD --- 

This function is used in parallel functions for recurrent division of large
task into two smaller tasks.

It has following properties:
* it works only for TaskSize>=2 and ChunkSize>=2
  (assertion is thrown otherwise)
* Task0+Task1=TaskSize, Task0>0, Task1>0
* Task0 and Task1 are close to each other
* in case TaskSize>ChunkSize, Task0 is always divisible by ChunkSize

  -- ALGLIB --
     Copyright 07.04.2013 by Bochkanov Sergey
*************************************************************************/
void splitlength(ae_int_t tasksize,
     ae_int_t chunksize,
     ae_int_t* task0,
     ae_int_t* task1,
     ae_state *_state)
{

    *task0 = 0;
    *task1 = 0;

    ae_assert(chunksize>=2, "SplitLength: ChunkSize<2", _state);
    ae_assert(tasksize>=2, "SplitLength: TaskSize<2", _state);
    *task0 = tasksize/2;
    if( *task0>chunksize&&*task0%chunksize!=0 )
    {
        *task0 = *task0-*task0%chunksize;
    }
    *task1 = tasksize-(*task0);
    ae_assert(*task0>=1, "SplitLength: internal error", _state);
    ae_assert(*task1>=1, "SplitLength: internal error", _state);
}


/*************************************************************************
Outputs vector A[I0,I1-1] to trace log using either:
a)  6-digit exponential format (no trace flags is set)
b) 15-ditit exponential format ('PREC.E15' trace flag is set)
b)  6-ditit fixed-point format ('PREC.F6' trace flag is set)

This function checks trace flags every time it is called.
*************************************************************************/
void tracevectorautoprec(/* Real    */ const ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t prectouse;


    
    /*
     * Determine precision to use
     */
    prectouse = 0;
    if( ae_is_trace_enabled("PREC.E15") )
    {
        prectouse = 1;
    }
    if( ae_is_trace_enabled("PREC.F6") )
    {
        prectouse = 2;
    }
    
    /*
     * Output
     */
    ae_trace("[ ");
    for(i=i0; i<=i1-1; i++)
    {
        if( prectouse==0 )
        {
            ae_trace("%14.6e",
                (double)(a->ptr.p_double[i]));
        }
        if( prectouse==1 )
        {
            ae_trace("%23.15e",
                (double)(a->ptr.p_double[i]));
        }
        if( prectouse==2 )
        {
            ae_trace("%13.6f",
                (double)(a->ptr.p_double[i]));
        }
        if( i<i1-1 )
        {
            ae_trace(" ");
        }
    }
    ae_trace(" ]");
}


/*************************************************************************
Outputs row A[I,J0..J1-1] to trace log using either:
a)  6-digit exponential format (no trace flags is set)
b) 15-ditit exponential format ('PREC.E15' trace flag is set)
b)  6-ditit fixed-point format ('PREC.F6' trace flag is set)

This function checks trace flags every time it is called.
*************************************************************************/
void tracerowautoprec(/* Real    */ const ae_matrix* a,
     ae_int_t i,
     ae_int_t j0,
     ae_int_t j1,
     ae_state *_state)
{
    ae_int_t j;
    ae_int_t prectouse;


    
    /*
     * Determine precision to use
     */
    prectouse = 0;
    if( ae_is_trace_enabled("PREC.E15") )
    {
        prectouse = 1;
    }
    if( ae_is_trace_enabled("PREC.F6") )
    {
        prectouse = 2;
    }
    
    /*
     * Output
     */
    ae_trace("[ ");
    for(j=j0; j<=j1-1; j++)
    {
        if( prectouse==0 )
        {
            ae_trace("%14.6e",
                (double)(a->ptr.pp_double[i][j]));
        }
        if( prectouse==1 )
        {
            ae_trace("%23.15e",
                (double)(a->ptr.pp_double[i][j]));
        }
        if( prectouse==2 )
        {
            ae_trace("%13.6f",
                (double)(a->ptr.pp_double[i][j]));
        }
        if( j<j1-1 )
        {
            ae_trace(" ");
        }
    }
    ae_trace(" ]");
}


/*************************************************************************
Unscales/unshifts vector A[N] by computing A*Scl+Sft and outputs result to
trace log using either:
a)  6-digit exponential format (no trace flags is set)
b) 15-ditit exponential format ('PREC.E15' trace flag is set)
b)  6-ditit fixed-point format ('PREC.F6' trace flag is set)

This function checks trace flags every time it is called.
Both Scl and Sft can be omitted.
*************************************************************************/
void tracevectorunscaledunshiftedautoprec(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* scl,
     ae_bool applyscl,
     /* Real    */ const ae_vector* sft,
     ae_bool applysft,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t prectouse;
    double v;


    
    /*
     * Determine precision to use
     */
    prectouse = 0;
    if( ae_is_trace_enabled("PREC.E15") )
    {
        prectouse = 1;
    }
    if( ae_is_trace_enabled("PREC.F6") )
    {
        prectouse = 2;
    }
    
    /*
     * Output
     */
    ae_trace("[ ");
    for(i=0; i<=n-1; i++)
    {
        v = x->ptr.p_double[i];
        if( applyscl )
        {
            v = v*scl->ptr.p_double[i];
        }
        if( applysft )
        {
            v = v+sft->ptr.p_double[i];
        }
        if( prectouse==0 )
        {
            ae_trace("%14.6e",
                (double)(v));
        }
        if( prectouse==1 )
        {
            ae_trace("%23.15e",
                (double)(v));
        }
        if( prectouse==2 )
        {
            ae_trace("%13.6f",
                (double)(v));
        }
        if( i<n-1 )
        {
            ae_trace(" ");
        }
    }
    ae_trace(" ]");
}


/*************************************************************************
Outputs vector of 1-norms of rows [I0,I1-1] of A[I0...I1-1,J0...J1-1]   to
trace log using either:
a)  6-digit exponential format (no trace flags is set)
b) 15-ditit exponential format ('PREC.E15' trace flag is set)
b)  6-ditit fixed-point format ('PREC.F6' trace flag is set)

This function checks trace flags every time it is called.
*************************************************************************/
void tracerownrm1autoprec(/* Real    */ const ae_matrix* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_int_t j0,
     ae_int_t j1,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;
    ae_int_t prectouse;


    
    /*
     * Determine precision to use
     */
    prectouse = 0;
    if( ae_is_trace_enabled("PREC.E15") )
    {
        prectouse = 1;
    }
    if( ae_is_trace_enabled("PREC.F6") )
    {
        prectouse = 2;
    }
    
    /*
     * Output
     */
    ae_trace("[ ");
    for(i=i0; i<=i1-1; i++)
    {
        v = (double)(0);
        for(j=j0; j<=j1-1; j++)
        {
            v = ae_maxreal(v, ae_fabs(a->ptr.pp_double[i][j], _state), _state);
        }
        if( prectouse==0 )
        {
            ae_trace("%14.6e",
                (double)(v));
        }
        if( prectouse==1 )
        {
            ae_trace("%23.15e",
                (double)(v));
        }
        if( prectouse==2 )
        {
            ae_trace("%13.6f",
                (double)(v));
        }
        if( i<i1-1 )
        {
            ae_trace(" ");
        }
    }
    ae_trace(" ]");
}


/*************************************************************************
Outputs vector A[I0,I1-1] to trace log using E3 precision
*************************************************************************/
void tracevectore3(/* Real    */ const ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_state *_state)
{
    ae_int_t i;


    ae_trace("[ ");
    for(i=i0; i<=i1-1; i++)
    {
        ae_trace("%11.3e",
            (double)(a->ptr.p_double[i]));
        if( i<i1-1 )
        {
            ae_trace(" ");
        }
    }
    ae_trace(" ]");
}


/*************************************************************************
Outputs vector A[I0,I1-1] to trace log using E6 precision
*************************************************************************/
void tracevectore6(/* Real    */ const ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_state *_state)
{
    ae_int_t i;


    ae_trace("[ ");
    for(i=i0; i<=i1-1; i++)
    {
        ae_trace("%14.6e",
            (double)(a->ptr.p_double[i]));
        if( i<i1-1 )
        {
            ae_trace(" ");
        }
    }
    ae_trace(" ]");
}


/*************************************************************************
Outputs vector A[I0,I1-1] to trace log using E8 or E15 precision
*************************************************************************/
void tracevectore615(/* Real    */ const ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_bool usee15,
     ae_state *_state)
{
    ae_int_t i;


    ae_trace("[ ");
    for(i=i0; i<=i1-1; i++)
    {
        if( usee15 )
        {
            ae_trace("%23.15e",
                (double)(a->ptr.p_double[i]));
        }
        else
        {
            ae_trace("%14.6e",
                (double)(a->ptr.p_double[i]));
        }
        if( i<i1-1 )
        {
            ae_trace(" ");
        }
    }
    ae_trace(" ]");
}


/*************************************************************************
Outputs vector of 1-norms of rows [I0,I1-1] of A[I0...I1-1,J0...J1-1]   to
trace log using E8 precision
*************************************************************************/
void tracerownrm1e6(/* Real    */ const ae_matrix* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_int_t j0,
     ae_int_t j1,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;


    ae_trace("[ ");
    for(i=i0; i<=i1-1; i++)
    {
        v = (double)(0);
        for(j=j0; j<=j1-1; j++)
        {
            v = ae_maxreal(v, ae_fabs(a->ptr.pp_double[i][j], _state), _state);
        }
        ae_trace("%14.6e",
            (double)(v));
        if( i<i1-1 )
        {
            ae_trace(" ");
        }
    }
    ae_trace(" ]");
}


/*************************************************************************
Outputs specified number of spaces
*************************************************************************/
void tracespaces(ae_int_t cnt, ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=cnt-1; i++)
    {
        ae_trace(" ");
    }
}


/*************************************************************************
Minimum speedup feasible for multithreading
*************************************************************************/
double minspeedup(ae_state *_state)
{
    double result;


    result = 1.5;
    return result;
}


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Maximum concurrency on given system, with given compilation settings
*************************************************************************/
ae_int_t maxconcurrency(ae_state *_state)
{
    ae_int_t result;


    result = 1;
    return result;
}
#endif


/*************************************************************************
Initialize SAvgCounter

Prior value is a value that is returned when no values are in the buffer
*************************************************************************/
void savgcounterinit(savgcounter* c, double priorvalue, ae_state *_state)
{


    c->rsum = (double)(0);
    c->rcnt = (double)(0);
    c->prior = priorvalue;
}


/*************************************************************************
Enqueue value into SAvgCounter
*************************************************************************/
void savgcounterenqueue(savgcounter* c, double v, ae_state *_state)
{


    c->rsum = c->rsum+v;
    c->rcnt = c->rcnt+(double)1;
}


/*************************************************************************
Enqueue value into SAvgCounter
*************************************************************************/
double savgcounterget(const savgcounter* c, ae_state *_state)
{
    double result;


    if( ae_fp_eq(c->rcnt,(double)(0)) )
    {
        result = c->prior;
    }
    else
    {
        result = c->rsum/c->rcnt;
    }
    return result;
}


/*************************************************************************
Initialize SQuantileCounter

Prior value is a value that is returned when no values are in the buffer
*************************************************************************/
void squantilecounterinit(squantilecounter* c,
     double priorvalue,
     ae_state *_state)
{


    c->cnt = 0;
    c->prior = priorvalue;
}


/*************************************************************************
Enqueue value into SQuantileCounter
*************************************************************************/
void squantilecounterenqueue(squantilecounter* c,
     double v,
     ae_state *_state)
{


    if( c->elems.cnt==c->cnt )
    {
        rvectorresize(&c->elems, 2*c->cnt+1, _state);
    }
    c->elems.ptr.p_double[c->cnt] = v;
    c->cnt = c->cnt+1;
}


/*************************************************************************
Get k-th quantile. Thread-unsafe, modifies internal structures.

0<=Q<=1.
*************************************************************************/
double squantilecounterget(squantilecounter* c,
     double q,
     ae_state *_state)
{
    ae_int_t left;
    ae_int_t right;
    ae_int_t k;
    ae_int_t pivotindex;
    double pivotvalue;
    ae_int_t storeindex;
    ae_int_t i;
    double result;


    ae_assert(ae_fp_greater_eq(q,(double)(0))&&ae_fp_less_eq(q,(double)(1)), "SQuantileCounterGet: incorrect Q", _state);
    if( c->cnt==0 )
    {
        result = c->prior;
        return result;
    }
    if( c->cnt==1 )
    {
        result = c->elems.ptr.p_double[0];
        return result;
    }
    k = ae_round(q*(double)(c->cnt-1), _state);
    left = 0;
    right = c->cnt-1;
    for(;;)
    {
        if( left==right )
        {
            result = c->elems.ptr.p_double[left];
            return result;
        }
        pivotindex = left+(right-left)/2;
        pivotvalue = c->elems.ptr.p_double[pivotindex];
        swapelements(&c->elems, pivotindex, right, _state);
        storeindex = left;
        for(i=left; i<=right-1; i++)
        {
            if( ae_fp_less(c->elems.ptr.p_double[i],pivotvalue) )
            {
                swapelements(&c->elems, storeindex, i, _state);
                storeindex = storeindex+1;
            }
        }
        swapelements(&c->elems, storeindex, right, _state);
        pivotindex = storeindex;
        if( pivotindex==k )
        {
            result = c->elems.ptr.p_double[k];
            return result;
        }
        if( k<pivotindex )
        {
            right = pivotindex-1;
        }
        else
        {
            left = pivotindex+1;
        }
    }
    return result;
}


void _apbuffers_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    apbuffers *p = (apbuffers*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->ba0, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->ia0, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->ia1, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->ia2, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->ia3, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->ra0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->ra1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->ra2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->ra3, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->rm0, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->rm1, 0, 0, DT_REAL, _state, make_automatic);
}


void _apbuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    apbuffers       *dst = (apbuffers*)_dst;
    const apbuffers *src = (const apbuffers*)_src;
    ae_vector_init_copy(&dst->ba0, &src->ba0, _state, make_automatic);
    ae_vector_init_copy(&dst->ia0, &src->ia0, _state, make_automatic);
    ae_vector_init_copy(&dst->ia1, &src->ia1, _state, make_automatic);
    ae_vector_init_copy(&dst->ia2, &src->ia2, _state, make_automatic);
    ae_vector_init_copy(&dst->ia3, &src->ia3, _state, make_automatic);
    ae_vector_init_copy(&dst->ra0, &src->ra0, _state, make_automatic);
    ae_vector_init_copy(&dst->ra1, &src->ra1, _state, make_automatic);
    ae_vector_init_copy(&dst->ra2, &src->ra2, _state, make_automatic);
    ae_vector_init_copy(&dst->ra3, &src->ra3, _state, make_automatic);
    ae_matrix_init_copy(&dst->rm0, &src->rm0, _state, make_automatic);
    ae_matrix_init_copy(&dst->rm1, &src->rm1, _state, make_automatic);
}


void _apbuffers_clear(void* _p)
{
    apbuffers *p = (apbuffers*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->ba0);
    ae_vector_clear(&p->ia0);
    ae_vector_clear(&p->ia1);
    ae_vector_clear(&p->ia2);
    ae_vector_clear(&p->ia3);
    ae_vector_clear(&p->ra0);
    ae_vector_clear(&p->ra1);
    ae_vector_clear(&p->ra2);
    ae_vector_clear(&p->ra3);
    ae_matrix_clear(&p->rm0);
    ae_matrix_clear(&p->rm1);
}


void _apbuffers_destroy(void* _p)
{
    apbuffers *p = (apbuffers*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->ba0);
    ae_vector_destroy(&p->ia0);
    ae_vector_destroy(&p->ia1);
    ae_vector_destroy(&p->ia2);
    ae_vector_destroy(&p->ia3);
    ae_vector_destroy(&p->ra0);
    ae_vector_destroy(&p->ra1);
    ae_vector_destroy(&p->ra2);
    ae_vector_destroy(&p->ra3);
    ae_matrix_destroy(&p->rm0);
    ae_matrix_destroy(&p->rm1);
}


void _sboolean_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    sboolean *p = (sboolean*)_p;
    ae_touch_ptr((void*)p);
}


void _sboolean_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    sboolean       *dst = (sboolean*)_dst;
    const sboolean *src = (const sboolean*)_src;
    dst->val = src->val;
}


void _sboolean_clear(void* _p)
{
    sboolean *p = (sboolean*)_p;
    ae_touch_ptr((void*)p);
}


void _sboolean_destroy(void* _p)
{
    sboolean *p = (sboolean*)_p;
    ae_touch_ptr((void*)p);
}


void _sbooleanarray_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    sbooleanarray *p = (sbooleanarray*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->val, 0, DT_BOOL, _state, make_automatic);
}


void _sbooleanarray_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    sbooleanarray       *dst = (sbooleanarray*)_dst;
    const sbooleanarray *src = (const sbooleanarray*)_src;
    ae_vector_init_copy(&dst->val, &src->val, _state, make_automatic);
}


void _sbooleanarray_clear(void* _p)
{
    sbooleanarray *p = (sbooleanarray*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->val);
}


void _sbooleanarray_destroy(void* _p)
{
    sbooleanarray *p = (sbooleanarray*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->val);
}


void _sinteger_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    sinteger *p = (sinteger*)_p;
    ae_touch_ptr((void*)p);
}


void _sinteger_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    sinteger       *dst = (sinteger*)_dst;
    const sinteger *src = (const sinteger*)_src;
    dst->val = src->val;
}


void _sinteger_clear(void* _p)
{
    sinteger *p = (sinteger*)_p;
    ae_touch_ptr((void*)p);
}


void _sinteger_destroy(void* _p)
{
    sinteger *p = (sinteger*)_p;
    ae_touch_ptr((void*)p);
}


void _sintegerarray_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    sintegerarray *p = (sintegerarray*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->val, 0, DT_INT, _state, make_automatic);
}


void _sintegerarray_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    sintegerarray       *dst = (sintegerarray*)_dst;
    const sintegerarray *src = (const sintegerarray*)_src;
    ae_vector_init_copy(&dst->val, &src->val, _state, make_automatic);
}


void _sintegerarray_clear(void* _p)
{
    sintegerarray *p = (sintegerarray*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->val);
}


void _sintegerarray_destroy(void* _p)
{
    sintegerarray *p = (sintegerarray*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->val);
}


void _sreal_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    sreal *p = (sreal*)_p;
    ae_touch_ptr((void*)p);
}


void _sreal_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    sreal       *dst = (sreal*)_dst;
    const sreal *src = (const sreal*)_src;
    dst->val = src->val;
}


void _sreal_clear(void* _p)
{
    sreal *p = (sreal*)_p;
    ae_touch_ptr((void*)p);
}


void _sreal_destroy(void* _p)
{
    sreal *p = (sreal*)_p;
    ae_touch_ptr((void*)p);
}


void _srealarray_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    srealarray *p = (srealarray*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->val, 0, DT_REAL, _state, make_automatic);
}


void _srealarray_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    srealarray       *dst = (srealarray*)_dst;
    const srealarray *src = (const srealarray*)_src;
    ae_vector_init_copy(&dst->val, &src->val, _state, make_automatic);
}


void _srealarray_clear(void* _p)
{
    srealarray *p = (srealarray*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->val);
}


void _srealarray_destroy(void* _p)
{
    srealarray *p = (srealarray*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->val);
}


void _scomplex_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    scomplex *p = (scomplex*)_p;
    ae_touch_ptr((void*)p);
}


void _scomplex_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    scomplex       *dst = (scomplex*)_dst;
    const scomplex *src = (const scomplex*)_src;
    dst->val = src->val;
}


void _scomplex_clear(void* _p)
{
    scomplex *p = (scomplex*)_p;
    ae_touch_ptr((void*)p);
}


void _scomplex_destroy(void* _p)
{
    scomplex *p = (scomplex*)_p;
    ae_touch_ptr((void*)p);
}


void _scomplexarray_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    scomplexarray *p = (scomplexarray*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->val, 0, DT_COMPLEX, _state, make_automatic);
}


void _scomplexarray_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    scomplexarray       *dst = (scomplexarray*)_dst;
    const scomplexarray *src = (const scomplexarray*)_src;
    ae_vector_init_copy(&dst->val, &src->val, _state, make_automatic);
}


void _scomplexarray_clear(void* _p)
{
    scomplexarray *p = (scomplexarray*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->val);
}


void _scomplexarray_destroy(void* _p)
{
    scomplexarray *p = (scomplexarray*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->val);
}


void _nbpool_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    nbpool *p = (nbpool*)_p;
    ae_touch_ptr((void*)p);
    ae_shared_pool_init(&p->sourcepool, _state, make_automatic);
    ae_shared_pool_init(&p->temporarypool, _state, make_automatic);
    _sbooleanarray_init(&p->seed0, _state, make_automatic);
    _sbooleanarray_init(&p->seedn, _state, make_automatic);
}


void _nbpool_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    nbpool       *dst = (nbpool*)_dst;
    const nbpool *src = (const nbpool*)_src;
    dst->n = src->n;
    dst->temporariescount = src->temporariescount;
    ae_shared_pool_init_copy(&dst->sourcepool, &src->sourcepool, _state, make_automatic);
    ae_shared_pool_init_copy(&dst->temporarypool, &src->temporarypool, _state, make_automatic);
    _sbooleanarray_init_copy(&dst->seed0, &src->seed0, _state, make_automatic);
    _sbooleanarray_init_copy(&dst->seedn, &src->seedn, _state, make_automatic);
}


void _nbpool_clear(void* _p)
{
    nbpool *p = (nbpool*)_p;
    ae_touch_ptr((void*)p);
    ae_shared_pool_clear(&p->sourcepool);
    ae_shared_pool_clear(&p->temporarypool);
    _sbooleanarray_clear(&p->seed0);
    _sbooleanarray_clear(&p->seedn);
}


void _nbpool_destroy(void* _p)
{
    nbpool *p = (nbpool*)_p;
    ae_touch_ptr((void*)p);
    ae_shared_pool_destroy(&p->sourcepool);
    ae_shared_pool_destroy(&p->temporarypool);
    _sbooleanarray_destroy(&p->seed0);
    _sbooleanarray_destroy(&p->seedn);
}


void _nipool_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    nipool *p = (nipool*)_p;
    ae_touch_ptr((void*)p);
    ae_shared_pool_init(&p->sourcepool, _state, make_automatic);
    ae_shared_pool_init(&p->temporarypool, _state, make_automatic);
    _sintegerarray_init(&p->seed0, _state, make_automatic);
    _sintegerarray_init(&p->seedn, _state, make_automatic);
}


void _nipool_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    nipool       *dst = (nipool*)_dst;
    const nipool *src = (const nipool*)_src;
    dst->n = src->n;
    dst->temporariescount = src->temporariescount;
    ae_shared_pool_init_copy(&dst->sourcepool, &src->sourcepool, _state, make_automatic);
    ae_shared_pool_init_copy(&dst->temporarypool, &src->temporarypool, _state, make_automatic);
    _sintegerarray_init_copy(&dst->seed0, &src->seed0, _state, make_automatic);
    _sintegerarray_init_copy(&dst->seedn, &src->seedn, _state, make_automatic);
}


void _nipool_clear(void* _p)
{
    nipool *p = (nipool*)_p;
    ae_touch_ptr((void*)p);
    ae_shared_pool_clear(&p->sourcepool);
    ae_shared_pool_clear(&p->temporarypool);
    _sintegerarray_clear(&p->seed0);
    _sintegerarray_clear(&p->seedn);
}


void _nipool_destroy(void* _p)
{
    nipool *p = (nipool*)_p;
    ae_touch_ptr((void*)p);
    ae_shared_pool_destroy(&p->sourcepool);
    ae_shared_pool_destroy(&p->temporarypool);
    _sintegerarray_destroy(&p->seed0);
    _sintegerarray_destroy(&p->seedn);
}


void _nrpool_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    nrpool *p = (nrpool*)_p;
    ae_touch_ptr((void*)p);
    ae_shared_pool_init(&p->sourcepool, _state, make_automatic);
    ae_shared_pool_init(&p->temporarypool, _state, make_automatic);
    _srealarray_init(&p->seed0, _state, make_automatic);
    _srealarray_init(&p->seedn, _state, make_automatic);
}


void _nrpool_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    nrpool       *dst = (nrpool*)_dst;
    const nrpool *src = (const nrpool*)_src;
    dst->n = src->n;
    dst->temporariescount = src->temporariescount;
    ae_shared_pool_init_copy(&dst->sourcepool, &src->sourcepool, _state, make_automatic);
    ae_shared_pool_init_copy(&dst->temporarypool, &src->temporarypool, _state, make_automatic);
    _srealarray_init_copy(&dst->seed0, &src->seed0, _state, make_automatic);
    _srealarray_init_copy(&dst->seedn, &src->seedn, _state, make_automatic);
}


void _nrpool_clear(void* _p)
{
    nrpool *p = (nrpool*)_p;
    ae_touch_ptr((void*)p);
    ae_shared_pool_clear(&p->sourcepool);
    ae_shared_pool_clear(&p->temporarypool);
    _srealarray_clear(&p->seed0);
    _srealarray_clear(&p->seedn);
}


void _nrpool_destroy(void* _p)
{
    nrpool *p = (nrpool*)_p;
    ae_touch_ptr((void*)p);
    ae_shared_pool_destroy(&p->sourcepool);
    ae_shared_pool_destroy(&p->temporarypool);
    _srealarray_destroy(&p->seed0);
    _srealarray_destroy(&p->seedn);
}


void _savgcounter_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    savgcounter *p = (savgcounter*)_p;
    ae_touch_ptr((void*)p);
}


void _savgcounter_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    savgcounter       *dst = (savgcounter*)_dst;
    const savgcounter *src = (const savgcounter*)_src;
    dst->rsum = src->rsum;
    dst->rcnt = src->rcnt;
    dst->prior = src->prior;
}


void _savgcounter_clear(void* _p)
{
    savgcounter *p = (savgcounter*)_p;
    ae_touch_ptr((void*)p);
}


void _savgcounter_destroy(void* _p)
{
    savgcounter *p = (savgcounter*)_p;
    ae_touch_ptr((void*)p);
}


void _squantilecounter_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    squantilecounter *p = (squantilecounter*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->elems, 0, DT_REAL, _state, make_automatic);
}


void _squantilecounter_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    squantilecounter       *dst = (squantilecounter*)_dst;
    const squantilecounter *src = (const squantilecounter*)_src;
    dst->cnt = src->cnt;
    ae_vector_init_copy(&dst->elems, &src->elems, _state, make_automatic);
    dst->prior = src->prior;
}


void _squantilecounter_clear(void* _p)
{
    squantilecounter *p = (squantilecounter*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->elems);
}


void _squantilecounter_destroy(void* _p)
{
    squantilecounter *p = (squantilecounter*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->elems);
}


/*$ End $*/
