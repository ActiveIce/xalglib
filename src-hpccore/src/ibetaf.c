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
#include "ibetaf.h"


/*$ Declarations $*/
static double ibetaf_incompletebetafe(double a,
     double b,
     double x,
     double big,
     double biginv,
     ae_state *_state);
static double ibetaf_incompletebetafe2(double a,
     double b,
     double x,
     double big,
     double biginv,
     ae_state *_state);
static double ibetaf_incompletebetaps(double a,
     double b,
     double x,
     double maxgam,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
Incomplete beta integral

Returns incomplete beta integral of the arguments, evaluated
from zero to x.  The function is defined as

                 x
    -            -
   | (a+b)      | |  a-1     b-1
 -----------    |   t   (1-t)   dt.
  -     -     | |
 | (a) | (b)   -
                0

The domain of definition is 0 <= x <= 1.  In this
implementation a and b are restricted to positive values.
The integral from x to 1 may be obtained by the symmetry
relation

   1 - incbet( a, b, x )  =  incbet( b, a, 1-x ).

The integral is evaluated by a continued fraction expansion
or, when b*x is small, by a power series.

ACCURACY:

Tested at uniformly distributed random points (a,b,x) with a and b
in "domain" and x between 0 and 1.
                                       Relative error
arithmetic   domain     # trials      peak         rms
   IEEE      0,5         10000       6.9e-15     4.5e-16
   IEEE      0,85       250000       2.2e-13     1.7e-14
   IEEE      0,1000      30000       5.3e-12     6.3e-13
   IEEE      0,10000    250000       9.3e-11     7.1e-12
   IEEE      0,100000    10000       8.7e-10     4.8e-11
Outputs smaller than the IEEE gradual underflow threshold
were excluded from these statistics.

Cephes Math Library, Release 2.8:  June, 2000
Copyright 1984, 1995, 2000 by Stephen L. Moshier
*************************************************************************/
double incompletebeta(double a, double b, double x, ae_state *_state)
{
    double t;
    double xc;
    double w;
    double y;
    ae_int_t flag;
    double sg;
    double big;
    double biginv;
    double maxgam;
    double minlog;
    double maxlog;
    double result;


    big = 4.503599627370496e15;
    biginv = 2.22044604925031308085e-16;
    maxgam = 171.624376956302725;
    minlog = ae_log(ae_minrealnumber, _state);
    maxlog = ae_log(ae_maxrealnumber, _state);
    ae_assert(ae_fp_greater(a,(double)(0))&&ae_fp_greater(b,(double)(0)), "Domain error in IncompleteBeta", _state);
    ae_assert(ae_fp_greater_eq(x,(double)(0))&&ae_fp_less_eq(x,(double)(1)), "Domain error in IncompleteBeta", _state);
    if( ae_fp_eq(x,(double)(0)) )
    {
        result = (double)(0);
        return result;
    }
    if( ae_fp_eq(x,(double)(1)) )
    {
        result = (double)(1);
        return result;
    }
    flag = 0;
    if( ae_fp_less_eq(b*x,1.0)&&ae_fp_less_eq(x,0.95) )
    {
        result = ibetaf_incompletebetaps(a, b, x, maxgam, _state);
        return result;
    }
    w = 1.0-x;
    if( ae_fp_greater(x,a/(a+b)) )
    {
        flag = 1;
        t = a;
        a = b;
        b = t;
        xc = x;
        x = w;
    }
    else
    {
        xc = w;
    }
    if( (flag==1&&ae_fp_less_eq(b*x,1.0))&&ae_fp_less_eq(x,0.95) )
    {
        t = ibetaf_incompletebetaps(a, b, x, maxgam, _state);
        if( ae_fp_less_eq(t,ae_machineepsilon) )
        {
            result = 1.0-ae_machineepsilon;
        }
        else
        {
            result = 1.0-t;
        }
        return result;
    }
    y = x*(a+b-2.0)-(a-1.0);
    if( ae_fp_less(y,0.0) )
    {
        w = ibetaf_incompletebetafe(a, b, x, big, biginv, _state);
    }
    else
    {
        w = ibetaf_incompletebetafe2(a, b, x, big, biginv, _state)/xc;
    }
    y = a*ae_log(x, _state);
    t = b*ae_log(xc, _state);
    if( (ae_fp_less(a+b,maxgam)&&ae_fp_less(ae_fabs(y, _state),maxlog))&&ae_fp_less(ae_fabs(t, _state),maxlog) )
    {
        t = ae_pow(xc, b, _state);
        t = t*ae_pow(x, a, _state);
        t = t/a;
        t = t*w;
        t = t*(gammafunction(a+b, _state)/(gammafunction(a, _state)*gammafunction(b, _state)));
        if( flag==1 )
        {
            if( ae_fp_less_eq(t,ae_machineepsilon) )
            {
                result = 1.0-ae_machineepsilon;
            }
            else
            {
                result = 1.0-t;
            }
        }
        else
        {
            result = t;
        }
        return result;
    }
    y = y+t+lngamma(a+b, &sg, _state)-lngamma(a, &sg, _state)-lngamma(b, &sg, _state);
    y = y+ae_log(w/a, _state);
    if( ae_fp_less(y,minlog) )
    {
        t = 0.0;
    }
    else
    {
        t = ae_exp(y, _state);
    }
    if( flag==1 )
    {
        if( ae_fp_less_eq(t,ae_machineepsilon) )
        {
            t = 1.0-ae_machineepsilon;
        }
        else
        {
            t = 1.0-t;
        }
    }
    result = t;
    return result;
}


/*************************************************************************
Inverse of imcomplete beta integral

Given y, the function finds x such that

 incbet( a, b, x ) = y .

The routine performs interval halving or Newton iterations to find the
root of incbet(a,b,x) - y = 0.


ACCURACY:

                     Relative error:
               x     a,b
arithmetic   domain  domain  # trials    peak       rms
   IEEE      0,1    .5,10000   50000    5.8e-12   1.3e-13
   IEEE      0,1   .25,100    100000    1.8e-13   3.9e-15
   IEEE      0,1     0,5       50000    1.1e-12   5.5e-15
With a and b constrained to half-integer or integer values:
   IEEE      0,1    .5,10000   50000    5.8e-12   1.1e-13
   IEEE      0,1    .5,100    100000    1.7e-14   7.9e-16
With a = .5, b constrained to half-integer or integer values:
   IEEE      0,1    .5,10000   10000    8.3e-11   1.0e-11

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1996, 2000 by Stephen L. Moshier
*************************************************************************/
double invincompletebeta(double a, double b, double y, ae_state *_state)
{
    double aaa;
    double bbb;
    double y0;
    double d;
    double yyy;
    double x;
    double x0;
    double x1;
    double lgm;
    double yp;
    double di;
    double dithresh;
    double yl;
    double yh;
    double xt;
    ae_int_t i;
    ae_int_t rflg;
    ae_int_t dir;
    ae_int_t nflg;
    double s;
    ae_int_t mainlooppos;
    ae_int_t ihalve;
    ae_int_t ihalvecycle;
    ae_int_t newt;
    ae_int_t newtcycle;
    ae_int_t breaknewtcycle;
    ae_int_t breakihalvecycle;
    double result;


    i = 0;
    ae_assert(ae_fp_greater_eq(y,(double)(0))&&ae_fp_less_eq(y,(double)(1)), "Domain error in InvIncompleteBeta", _state);
    
    /*
     * special cases
     */
    if( ae_fp_eq(y,(double)(0)) )
    {
        result = (double)(0);
        return result;
    }
    if( ae_fp_eq(y,1.0) )
    {
        result = (double)(1);
        return result;
    }
    
    /*
     * these initializations are not really necessary,
     * but without them compiler complains about 'possibly uninitialized variables'.
     */
    dithresh = (double)(0);
    rflg = 0;
    aaa = (double)(0);
    bbb = (double)(0);
    y0 = (double)(0);
    x = (double)(0);
    yyy = (double)(0);
    lgm = (double)(0);
    dir = 0;
    di = (double)(0);
    
    /*
     * normal initializations
     */
    x0 = 0.0;
    yl = 0.0;
    x1 = 1.0;
    yh = 1.0;
    nflg = 0;
    mainlooppos = 0;
    ihalve = 1;
    ihalvecycle = 2;
    newt = 3;
    newtcycle = 4;
    breaknewtcycle = 5;
    breakihalvecycle = 6;
    
    /*
     * main loop
     */
    for(;;)
    {
        
        /*
         * start
         */
        if( mainlooppos==0 )
        {
            if( ae_fp_less_eq(a,1.0)||ae_fp_less_eq(b,1.0) )
            {
                dithresh = 1.0e-6;
                rflg = 0;
                aaa = a;
                bbb = b;
                y0 = y;
                x = aaa/(aaa+bbb);
                yyy = incompletebeta(aaa, bbb, x, _state);
                mainlooppos = ihalve;
                continue;
            }
            else
            {
                dithresh = 1.0e-4;
            }
            yp = -invnormaldistribution(y, _state);
            if( ae_fp_greater(y,0.5) )
            {
                rflg = 1;
                aaa = b;
                bbb = a;
                y0 = 1.0-y;
                yp = -yp;
            }
            else
            {
                rflg = 0;
                aaa = a;
                bbb = b;
                y0 = y;
            }
            lgm = (yp*yp-3.0)/6.0;
            x = 2.0/(1.0/(2.0*aaa-1.0)+1.0/(2.0*bbb-1.0));
            d = yp*ae_sqrt(x+lgm, _state)/x-(1.0/(2.0*bbb-1.0)-1.0/(2.0*aaa-1.0))*(lgm+5.0/6.0-2.0/(3.0*x));
            d = 2.0*d;
            if( ae_fp_less(d,ae_log(ae_minrealnumber, _state)) )
            {
                x = (double)(0);
                break;
            }
            x = aaa/(aaa+bbb*ae_exp(d, _state));
            yyy = incompletebeta(aaa, bbb, x, _state);
            yp = (yyy-y0)/y0;
            if( ae_fp_less(ae_fabs(yp, _state),0.2) )
            {
                mainlooppos = newt;
                continue;
            }
            mainlooppos = ihalve;
            continue;
        }
        
        /*
         * ihalve
         */
        if( mainlooppos==ihalve )
        {
            dir = 0;
            di = 0.5;
            i = 0;
            mainlooppos = ihalvecycle;
            continue;
        }
        
        /*
         * ihalvecycle
         */
        if( mainlooppos==ihalvecycle )
        {
            if( i<=99 )
            {
                if( i!=0 )
                {
                    x = x0+di*(x1-x0);
                    if( ae_fp_eq(x,1.0) )
                    {
                        x = 1.0-ae_machineepsilon;
                    }
                    if( ae_fp_eq(x,0.0) )
                    {
                        di = 0.5;
                        x = x0+di*(x1-x0);
                        if( ae_fp_eq(x,0.0) )
                        {
                            break;
                        }
                    }
                    yyy = incompletebeta(aaa, bbb, x, _state);
                    yp = (x1-x0)/(x1+x0);
                    if( ae_fp_less(ae_fabs(yp, _state),dithresh) )
                    {
                        mainlooppos = newt;
                        continue;
                    }
                    yp = (yyy-y0)/y0;
                    if( ae_fp_less(ae_fabs(yp, _state),dithresh) )
                    {
                        mainlooppos = newt;
                        continue;
                    }
                }
                if( ae_fp_less(yyy,y0) )
                {
                    x0 = x;
                    yl = yyy;
                    if( dir<0 )
                    {
                        dir = 0;
                        di = 0.5;
                    }
                    else
                    {
                        if( dir>3 )
                        {
                            di = 1.0-(1.0-di)*(1.0-di);
                        }
                        else
                        {
                            if( dir>1 )
                            {
                                di = 0.5*di+0.5;
                            }
                            else
                            {
                                di = (y0-yyy)/(yh-yl);
                            }
                        }
                    }
                    dir = dir+1;
                    if( ae_fp_greater(x0,0.75) )
                    {
                        if( rflg==1 )
                        {
                            rflg = 0;
                            aaa = a;
                            bbb = b;
                            y0 = y;
                        }
                        else
                        {
                            rflg = 1;
                            aaa = b;
                            bbb = a;
                            y0 = 1.0-y;
                        }
                        x = 1.0-x;
                        yyy = incompletebeta(aaa, bbb, x, _state);
                        x0 = 0.0;
                        yl = 0.0;
                        x1 = 1.0;
                        yh = 1.0;
                        mainlooppos = ihalve;
                        continue;
                    }
                }
                else
                {
                    x1 = x;
                    if( rflg==1&&ae_fp_less(x1,ae_machineepsilon) )
                    {
                        x = 0.0;
                        break;
                    }
                    yh = yyy;
                    if( dir>0 )
                    {
                        dir = 0;
                        di = 0.5;
                    }
                    else
                    {
                        if( dir<-3 )
                        {
                            di = di*di;
                        }
                        else
                        {
                            if( dir<-1 )
                            {
                                di = 0.5*di;
                            }
                            else
                            {
                                di = (yyy-y0)/(yh-yl);
                            }
                        }
                    }
                    dir = dir-1;
                }
                i = i+1;
                mainlooppos = ihalvecycle;
                continue;
            }
            else
            {
                mainlooppos = breakihalvecycle;
                continue;
            }
        }
        
        /*
         * breakihalvecycle
         */
        if( mainlooppos==breakihalvecycle )
        {
            if( ae_fp_greater_eq(x0,1.0) )
            {
                x = 1.0-ae_machineepsilon;
                break;
            }
            if( ae_fp_less_eq(x,0.0) )
            {
                x = 0.0;
                break;
            }
            mainlooppos = newt;
            continue;
        }
        
        /*
         * newt
         */
        if( mainlooppos==newt )
        {
            if( nflg!=0 )
            {
                break;
            }
            nflg = 1;
            lgm = lngamma(aaa+bbb, &s, _state)-lngamma(aaa, &s, _state)-lngamma(bbb, &s, _state);
            i = 0;
            mainlooppos = newtcycle;
            continue;
        }
        
        /*
         * newtcycle
         */
        if( mainlooppos==newtcycle )
        {
            if( i<=7 )
            {
                if( i!=0 )
                {
                    yyy = incompletebeta(aaa, bbb, x, _state);
                }
                if( ae_fp_less(yyy,yl) )
                {
                    x = x0;
                    yyy = yl;
                }
                else
                {
                    if( ae_fp_greater(yyy,yh) )
                    {
                        x = x1;
                        yyy = yh;
                    }
                    else
                    {
                        if( ae_fp_less(yyy,y0) )
                        {
                            x0 = x;
                            yl = yyy;
                        }
                        else
                        {
                            x1 = x;
                            yh = yyy;
                        }
                    }
                }
                if( ae_fp_eq(x,1.0)||ae_fp_eq(x,0.0) )
                {
                    mainlooppos = breaknewtcycle;
                    continue;
                }
                d = (aaa-1.0)*ae_log(x, _state)+(bbb-1.0)*ae_log(1.0-x, _state)+lgm;
                if( ae_fp_less(d,ae_log(ae_minrealnumber, _state)) )
                {
                    break;
                }
                if( ae_fp_greater(d,ae_log(ae_maxrealnumber, _state)) )
                {
                    mainlooppos = breaknewtcycle;
                    continue;
                }
                d = ae_exp(d, _state);
                d = (yyy-y0)/d;
                xt = x-d;
                if( ae_fp_less_eq(xt,x0) )
                {
                    yyy = (x-x0)/(x1-x0);
                    xt = x0+0.5*yyy*(x-x0);
                    if( ae_fp_less_eq(xt,0.0) )
                    {
                        mainlooppos = breaknewtcycle;
                        continue;
                    }
                }
                if( ae_fp_greater_eq(xt,x1) )
                {
                    yyy = (x1-x)/(x1-x0);
                    xt = x1-0.5*yyy*(x1-x);
                    if( ae_fp_greater_eq(xt,1.0) )
                    {
                        mainlooppos = breaknewtcycle;
                        continue;
                    }
                }
                x = xt;
                if( ae_fp_less(ae_fabs(d/x, _state),128.0*ae_machineepsilon) )
                {
                    break;
                }
                i = i+1;
                mainlooppos = newtcycle;
                continue;
            }
            else
            {
                mainlooppos = breaknewtcycle;
                continue;
            }
        }
        
        /*
         * breaknewtcycle
         */
        if( mainlooppos==breaknewtcycle )
        {
            dithresh = 256.0*ae_machineepsilon;
            mainlooppos = ihalve;
            continue;
        }
    }
    
    /*
     * done
     */
    if( rflg!=0 )
    {
        if( ae_fp_less_eq(x,ae_machineepsilon) )
        {
            x = 1.0-ae_machineepsilon;
        }
        else
        {
            x = 1.0-x;
        }
    }
    result = x;
    return result;
}


/*************************************************************************
Continued fraction expansion #1 for incomplete beta integral

Cephes Math Library, Release 2.8:  June, 2000
Copyright 1984, 1995, 2000 by Stephen L. Moshier
*************************************************************************/
static double ibetaf_incompletebetafe(double a,
     double b,
     double x,
     double big,
     double biginv,
     ae_state *_state)
{
    double xk;
    double pk;
    double pkm1;
    double pkm2;
    double qk;
    double qkm1;
    double qkm2;
    double k1;
    double k2;
    double k3;
    double k4;
    double k5;
    double k6;
    double k7;
    double k8;
    double r;
    double t;
    double ans;
    double thresh;
    ae_int_t n;
    double result;


    k1 = a;
    k2 = a+b;
    k3 = a;
    k4 = a+1.0;
    k5 = 1.0;
    k6 = b-1.0;
    k7 = k4;
    k8 = a+2.0;
    pkm2 = 0.0;
    qkm2 = 1.0;
    pkm1 = 1.0;
    qkm1 = 1.0;
    ans = 1.0;
    r = 1.0;
    n = 0;
    thresh = 3.0*ae_machineepsilon;
    do
    {
        xk = -x*k1*k2/(k3*k4);
        pk = pkm1+pkm2*xk;
        qk = qkm1+qkm2*xk;
        pkm2 = pkm1;
        pkm1 = pk;
        qkm2 = qkm1;
        qkm1 = qk;
        xk = x*k5*k6/(k7*k8);
        pk = pkm1+pkm2*xk;
        qk = qkm1+qkm2*xk;
        pkm2 = pkm1;
        pkm1 = pk;
        qkm2 = qkm1;
        qkm1 = qk;
        if( ae_fp_neq(qk,(double)(0)) )
        {
            r = pk/qk;
        }
        if( ae_fp_neq(r,(double)(0)) )
        {
            t = ae_fabs((ans-r)/r, _state);
            ans = r;
        }
        else
        {
            t = 1.0;
        }
        if( ae_fp_less(t,thresh) )
        {
            break;
        }
        k1 = k1+1.0;
        k2 = k2+1.0;
        k3 = k3+2.0;
        k4 = k4+2.0;
        k5 = k5+1.0;
        k6 = k6-1.0;
        k7 = k7+2.0;
        k8 = k8+2.0;
        if( ae_fp_greater(ae_fabs(qk, _state)+ae_fabs(pk, _state),big) )
        {
            pkm2 = pkm2*biginv;
            pkm1 = pkm1*biginv;
            qkm2 = qkm2*biginv;
            qkm1 = qkm1*biginv;
        }
        if( ae_fp_less(ae_fabs(qk, _state),biginv)||ae_fp_less(ae_fabs(pk, _state),biginv) )
        {
            pkm2 = pkm2*big;
            pkm1 = pkm1*big;
            qkm2 = qkm2*big;
            qkm1 = qkm1*big;
        }
        n = n+1;
    }
    while(n!=300);
    result = ans;
    return result;
}


/*************************************************************************
Continued fraction expansion #2
for incomplete beta integral

Cephes Math Library, Release 2.8:  June, 2000
Copyright 1984, 1995, 2000 by Stephen L. Moshier
*************************************************************************/
static double ibetaf_incompletebetafe2(double a,
     double b,
     double x,
     double big,
     double biginv,
     ae_state *_state)
{
    double xk;
    double pk;
    double pkm1;
    double pkm2;
    double qk;
    double qkm1;
    double qkm2;
    double k1;
    double k2;
    double k3;
    double k4;
    double k5;
    double k6;
    double k7;
    double k8;
    double r;
    double t;
    double ans;
    double z;
    double thresh;
    ae_int_t n;
    double result;


    k1 = a;
    k2 = b-1.0;
    k3 = a;
    k4 = a+1.0;
    k5 = 1.0;
    k6 = a+b;
    k7 = a+1.0;
    k8 = a+2.0;
    pkm2 = 0.0;
    qkm2 = 1.0;
    pkm1 = 1.0;
    qkm1 = 1.0;
    z = x/(1.0-x);
    ans = 1.0;
    r = 1.0;
    n = 0;
    thresh = 3.0*ae_machineepsilon;
    do
    {
        xk = -z*k1*k2/(k3*k4);
        pk = pkm1+pkm2*xk;
        qk = qkm1+qkm2*xk;
        pkm2 = pkm1;
        pkm1 = pk;
        qkm2 = qkm1;
        qkm1 = qk;
        xk = z*k5*k6/(k7*k8);
        pk = pkm1+pkm2*xk;
        qk = qkm1+qkm2*xk;
        pkm2 = pkm1;
        pkm1 = pk;
        qkm2 = qkm1;
        qkm1 = qk;
        if( ae_fp_neq(qk,(double)(0)) )
        {
            r = pk/qk;
        }
        if( ae_fp_neq(r,(double)(0)) )
        {
            t = ae_fabs((ans-r)/r, _state);
            ans = r;
        }
        else
        {
            t = 1.0;
        }
        if( ae_fp_less(t,thresh) )
        {
            break;
        }
        k1 = k1+1.0;
        k2 = k2-1.0;
        k3 = k3+2.0;
        k4 = k4+2.0;
        k5 = k5+1.0;
        k6 = k6+1.0;
        k7 = k7+2.0;
        k8 = k8+2.0;
        if( ae_fp_greater(ae_fabs(qk, _state)+ae_fabs(pk, _state),big) )
        {
            pkm2 = pkm2*biginv;
            pkm1 = pkm1*biginv;
            qkm2 = qkm2*biginv;
            qkm1 = qkm1*biginv;
        }
        if( ae_fp_less(ae_fabs(qk, _state),biginv)||ae_fp_less(ae_fabs(pk, _state),biginv) )
        {
            pkm2 = pkm2*big;
            pkm1 = pkm1*big;
            qkm2 = qkm2*big;
            qkm1 = qkm1*big;
        }
        n = n+1;
    }
    while(n!=300);
    result = ans;
    return result;
}


/*************************************************************************
Power series for incomplete beta integral.
Use when b*x is small and x not too close to 1.

Cephes Math Library, Release 2.8:  June, 2000
Copyright 1984, 1995, 2000 by Stephen L. Moshier
*************************************************************************/
static double ibetaf_incompletebetaps(double a,
     double b,
     double x,
     double maxgam,
     ae_state *_state)
{
    double s;
    double t;
    double u;
    double v;
    double n;
    double t1;
    double z;
    double ai;
    double sg;
    double result;


    ai = 1.0/a;
    u = (1.0-b)*x;
    v = u/(a+1.0);
    t1 = v;
    t = u;
    n = 2.0;
    s = 0.0;
    z = ae_machineepsilon*ai;
    while(ae_fp_greater(ae_fabs(v, _state),z))
    {
        u = (n-b)*x/n;
        t = t*u;
        v = t/(a+n);
        s = s+v;
        n = n+1.0;
    }
    s = s+t1;
    s = s+ai;
    u = a*ae_log(x, _state);
    if( ae_fp_less(a+b,maxgam)&&ae_fp_less(ae_fabs(u, _state),ae_log(ae_maxrealnumber, _state)) )
    {
        t = gammafunction(a+b, _state)/(gammafunction(a, _state)*gammafunction(b, _state));
        s = s*t*ae_pow(x, a, _state);
    }
    else
    {
        t = lngamma(a+b, &sg, _state)-lngamma(a, &sg, _state)-lngamma(b, &sg, _state)+u+ae_log(s, _state);
        if( ae_fp_less(t,ae_log(ae_minrealnumber, _state)) )
        {
            s = 0.0;
        }
        else
        {
            s = ae_exp(t, _state);
        }
    }
    result = s;
    return result;
}


/*$ End $*/
