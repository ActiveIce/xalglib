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
#include "xblas.h"


/*$ Declarations $*/
static void xblas_xsum(/* Real    */ ae_vector* w,
     double mx,
     ae_int_t n,
     double* r,
     double* rerr,
     ae_state *_state);
static double xblas_xfastpow(double r, ae_int_t n, ae_state *_state);


/*$ Body $*/


/*************************************************************************
More precise dot-product. Absolute error of  subroutine  result  is  about
1 ulp of max(MX,V), where:
    MX = max( |a[i]*b[i]| )
    V  = |(a,b)|

INPUT PARAMETERS
    A       -   array[0..N-1], vector 1
    B       -   array[0..N-1], vector 2
    N       -   vectors length, N<2^29.
    Temp    -   array[0..N-1], pre-allocated temporary storage

OUTPUT PARAMETERS
    R       -   (A,B)
    RErr    -   estimate of error. This estimate accounts for both  errors
                during  calculation  of  (A,B)  and  errors  introduced by
                rounding of A and B to fit in double (about 1 ulp).

  -- ALGLIB --
     Copyright 24.08.2009 by Bochkanov Sergey
*************************************************************************/
void xdot(/* Real    */ const ae_vector* a,
     /* Real    */ const ae_vector* b,
     ae_int_t n,
     /* Real    */ ae_vector* temp,
     double* r,
     double* rerr,
     ae_state *_state)
{
    ae_int_t i;
    double mx;
    double v;

    *r = 0.0;
    *rerr = 0.0;

    
    /*
     * special cases:
     * * N=0
     */
    if( n==0 )
    {
        *r = (double)(0);
        *rerr = (double)(0);
        return;
    }
    mx = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        v = a->ptr.p_double[i]*b->ptr.p_double[i];
        temp->ptr.p_double[i] = v;
        mx = ae_maxreal(mx, ae_fabs(v, _state), _state);
    }
    if( ae_fp_eq(mx,(double)(0)) )
    {
        *r = (double)(0);
        *rerr = (double)(0);
        return;
    }
    xblas_xsum(temp, mx, n, r, rerr, _state);
}


/*************************************************************************
More precise complex dot-product. Absolute error of  subroutine  result is
about 1 ulp of max(MX,V), where:
    MX = max( |a[i]*b[i]| )
    V  = |(a,b)|

INPUT PARAMETERS
    A       -   array[0..N-1], vector 1
    B       -   array[0..N-1], vector 2
    N       -   vectors length, N<2^29.
    Temp    -   array[0..2*N-1], pre-allocated temporary storage

OUTPUT PARAMETERS
    R       -   (A,B)
    RErr    -   estimate of error. This estimate accounts for both  errors
                during  calculation  of  (A,B)  and  errors  introduced by
                rounding of A and B to fit in double (about 1 ulp).

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void xcdot(/* Complex */ const ae_vector* a,
     /* Complex */ const ae_vector* b,
     ae_int_t n,
     /* Real    */ ae_vector* temp,
     ae_complex* r,
     double* rerr,
     ae_state *_state)
{
    ae_int_t i;
    double mx;
    double v;
    double rerrx;
    double rerry;

    r->x = 0.0;
    r->y = 0.0;
    *rerr = 0.0;

    
    /*
     * special cases:
     * * N=0
     */
    if( n==0 )
    {
        *r = ae_complex_from_i(0);
        *rerr = (double)(0);
        return;
    }
    
    /*
     * calculate real part
     */
    mx = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        v = a->ptr.p_complex[i].x*b->ptr.p_complex[i].x;
        temp->ptr.p_double[2*i+0] = v;
        mx = ae_maxreal(mx, ae_fabs(v, _state), _state);
        v = -a->ptr.p_complex[i].y*b->ptr.p_complex[i].y;
        temp->ptr.p_double[2*i+1] = v;
        mx = ae_maxreal(mx, ae_fabs(v, _state), _state);
    }
    if( ae_fp_eq(mx,(double)(0)) )
    {
        r->x = (double)(0);
        rerrx = (double)(0);
    }
    else
    {
        xblas_xsum(temp, mx, 2*n, &r->x, &rerrx, _state);
    }
    
    /*
     * calculate imaginary part
     */
    mx = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        v = a->ptr.p_complex[i].x*b->ptr.p_complex[i].y;
        temp->ptr.p_double[2*i+0] = v;
        mx = ae_maxreal(mx, ae_fabs(v, _state), _state);
        v = a->ptr.p_complex[i].y*b->ptr.p_complex[i].x;
        temp->ptr.p_double[2*i+1] = v;
        mx = ae_maxreal(mx, ae_fabs(v, _state), _state);
    }
    if( ae_fp_eq(mx,(double)(0)) )
    {
        r->y = (double)(0);
        rerry = (double)(0);
    }
    else
    {
        xblas_xsum(temp, mx, 2*n, &r->y, &rerry, _state);
    }
    
    /*
     * total error
     */
    if( ae_fp_eq(rerrx,(double)(0))&&ae_fp_eq(rerry,(double)(0)) )
    {
        *rerr = (double)(0);
    }
    else
    {
        *rerr = ae_maxreal(rerrx, rerry, _state)*ae_sqrt((double)1+ae_sqr(ae_minreal(rerrx, rerry, _state)/ae_maxreal(rerrx, rerry, _state), _state), _state);
    }
}


/*************************************************************************
Internal subroutine for extra-precise calculation of SUM(w[i]).

INPUT PARAMETERS:
    W   -   array[0..N-1], values to be added
            W is modified during calculations.
    MX  -   max(W[i])
    N   -   array size
    
OUTPUT PARAMETERS:
    R   -   SUM(w[i])
    RErr-   error estimate for R

  -- ALGLIB --
     Copyright 24.08.2009 by Bochkanov Sergey
*************************************************************************/
static void xblas_xsum(/* Real    */ ae_vector* w,
     double mx,
     ae_int_t n,
     double* r,
     double* rerr,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;
    ae_int_t ks;
    double v;
    double s;
    double ln2;
    double chunk;
    double invchunk;
    ae_bool allzeros;

    *r = 0.0;
    *rerr = 0.0;

    
    /*
     * special cases:
     * * N=0
     * * N is too large to use integer arithmetics
     */
    if( n==0 )
    {
        *r = (double)(0);
        *rerr = (double)(0);
        return;
    }
    if( ae_fp_eq(mx,(double)(0)) )
    {
        *r = (double)(0);
        *rerr = (double)(0);
        return;
    }
    ae_assert(n<536870912, "XDot: N is too large!", _state);
    
    /*
     * Prepare
     */
    ln2 = ae_log((double)(2), _state);
    *rerr = mx*ae_machineepsilon;
    
    /*
     * 1. find S such that 0.5<=S*MX<1
     * 2. multiply W by S, so task is normalized in some sense
     * 3. S:=1/S so we can obtain original vector multiplying by S
     */
    k = ae_round(ae_log(mx, _state)/ln2, _state);
    s = xblas_xfastpow((double)(2), -k, _state);
    if( !ae_isfinite(s, _state) )
    {
        
        /*
         * Overflow or underflow during evaluation of S; fallback low-precision code
         */
        *r = (double)(0);
        *rerr = mx*ae_machineepsilon;
        for(i=0; i<=n-1; i++)
        {
            *r = *r+w->ptr.p_double[i];
        }
        return;
    }
    while(ae_fp_greater_eq(s*mx,(double)(1)))
    {
        s = 0.5*s;
    }
    while(ae_fp_less(s*mx,0.5))
    {
        s = (double)2*s;
    }
    ae_v_muld(&w->ptr.p_double[0], 1, ae_v_len(0,n-1), s);
    s = (double)1/s;
    
    /*
     * find Chunk=2^M such that N*Chunk<2^29
     *
     * we have chosen upper limit (2^29) with enough space left
     * to tolerate possible problems with rounding and N's close
     * to the limit, so we don't want to be very strict here.
     */
    k = ae_trunc(ae_log((double)536870912/(double)n, _state)/ln2, _state);
    chunk = xblas_xfastpow((double)(2), k, _state);
    if( ae_fp_less(chunk,(double)(2)) )
    {
        chunk = (double)(2);
    }
    invchunk = (double)1/chunk;
    
    /*
     * calculate result
     */
    *r = (double)(0);
    ae_v_muld(&w->ptr.p_double[0], 1, ae_v_len(0,n-1), chunk);
    for(;;)
    {
        s = s*invchunk;
        allzeros = ae_true;
        ks = 0;
        for(i=0; i<=n-1; i++)
        {
            v = w->ptr.p_double[i];
            k = ae_trunc(v, _state);
            if( ae_fp_neq(v,(double)(k)) )
            {
                allzeros = ae_false;
            }
            w->ptr.p_double[i] = chunk*(v-(double)k);
            ks = ks+k;
        }
        *r = *r+s*(double)ks;
        if( allzeros||ae_fp_eq(s*(double)n+mx,mx) )
        {
            break;
        }
    }
    
    /*
     * correct error
     */
    *rerr = ae_maxreal(*rerr, ae_fabs(*r, _state)*ae_machineepsilon, _state);
}


/*************************************************************************
Fast Pow

  -- ALGLIB --
     Copyright 24.08.2009 by Bochkanov Sergey
*************************************************************************/
static double xblas_xfastpow(double r, ae_int_t n, ae_state *_state)
{
    double result;


    result = (double)(0);
    if( n>0 )
    {
        if( n%2==0 )
        {
            result = ae_sqr(xblas_xfastpow(r, n/2, _state), _state);
        }
        else
        {
            result = r*xblas_xfastpow(r, n-1, _state);
        }
        return result;
    }
    if( n==0 )
    {
        result = (double)(1);
    }
    if( n<0 )
    {
        result = xblas_xfastpow((double)1/r, -n, _state);
    }
    return result;
}


/*$ End $*/
