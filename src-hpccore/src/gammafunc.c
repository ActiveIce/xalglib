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
#include "gammafunc.h"


/*$ Declarations $*/
static double gammafunc_gammastirf(double x, ae_state *_state);


/*$ Body $*/


/*************************************************************************
Gamma function

Input parameters:
    X   -   argument

Domain:
    0 < X < 171.6
    -170 < X < 0, X is not an integer.

Relative error:
 arithmetic   domain     # trials      peak         rms
    IEEE    -170,-33      20000       2.3e-15     3.3e-16
    IEEE     -33,  33     20000       9.4e-16     2.2e-16
    IEEE      33, 171.6   20000       2.3e-15     3.2e-16

Cephes Math Library Release 2.8:  June, 2000
Original copyright 1984, 1987, 1989, 1992, 2000 by Stephen L. Moshier
Translated to AlgoPascal by Bochkanov Sergey (2005, 2006, 2007).
*************************************************************************/
double gammafunction(double x, ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_SPECFUNCS
    double p;
    double pp;
    double q;
    double qq;
    double z;
    ae_int_t i;
    double sgngam;
    double result;


    sgngam = (double)(1);
    q = ae_fabs(x, _state);
    if( ae_fp_greater(q,33.0) )
    {
        if( ae_fp_less(x,0.0) )
        {
            p = (double)(ae_ifloor(q, _state));
            i = ae_round(p, _state);
            if( i%2==0 )
            {
                sgngam = (double)(-1);
            }
            z = q-p;
            if( ae_fp_greater(z,0.5) )
            {
                p = p+(double)1;
                z = q-p;
            }
            z = q*ae_sin(ae_pi*z, _state);
            z = ae_fabs(z, _state);
            z = ae_pi/(z*gammafunc_gammastirf(q, _state));
        }
        else
        {
            z = gammafunc_gammastirf(x, _state);
        }
        result = sgngam*z;
        return result;
    }
    z = (double)(1);
    while(ae_fp_greater_eq(x,(double)(3)))
    {
        x = x-(double)1;
        z = z*x;
    }
    while(ae_fp_less(x,(double)(0)))
    {
        if( ae_fp_greater(x,-0.000000001) )
        {
            result = z/(((double)1+0.5772156649015329*x)*x);
            return result;
        }
        z = z/x;
        x = x+(double)1;
    }
    while(ae_fp_less(x,(double)(2)))
    {
        if( ae_fp_less(x,0.000000001) )
        {
            result = z/(((double)1+0.5772156649015329*x)*x);
            return result;
        }
        z = z/x;
        x = x+1.0;
    }
    if( ae_fp_eq(x,(double)(2)) )
    {
        result = z;
        return result;
    }
    x = x-2.0;
    pp = 1.60119522476751861407E-4;
    pp = 1.19135147006586384913E-3+x*pp;
    pp = 1.04213797561761569935E-2+x*pp;
    pp = 4.76367800457137231464E-2+x*pp;
    pp = 2.07448227648435975150E-1+x*pp;
    pp = 4.94214826801497100753E-1+x*pp;
    pp = 9.99999999999999996796E-1+x*pp;
    qq = -2.31581873324120129819E-5;
    qq = 5.39605580493303397842E-4+x*qq;
    qq = -4.45641913851797240494E-3+x*qq;
    qq = 1.18139785222060435552E-2+x*qq;
    qq = 3.58236398605498653373E-2+x*qq;
    qq = -2.34591795718243348568E-1+x*qq;
    qq = 7.14304917030273074085E-2+x*qq;
    qq = 1.00000000000000000320+x*qq;
    result = z*pp/qq;
    return result;
#else
    return _ialglib_i_gammafunction(x);
#endif
}


/*************************************************************************
Natural logarithm of gamma function

Input parameters:
    X       -   argument

Result:
    logarithm of the absolute value of the Gamma(X).

Output parameters:
    SgnGam  -   sign(Gamma(X))

Domain:
    0 < X < 2.55e305
    -2.55e305 < X < 0, X is not an integer.

ACCURACY:
arithmetic      domain        # trials     peak         rms
   IEEE    0, 3                 28000     5.4e-16     1.1e-16
   IEEE    2.718, 2.556e305     40000     3.5e-16     8.3e-17
The error criterion was relative when the function magnitude
was greater than one but absolute when it was less than one.

The following test used the relative error criterion, though
at certain points the relative error could be much higher than
indicated.
   IEEE    -200, -4             10000     4.8e-16     1.3e-16

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1989, 1992, 2000 by Stephen L. Moshier
Translated to AlgoPascal by Bochkanov Sergey (2005, 2006, 2007).
*************************************************************************/
double lngamma(double x, double* sgngam, ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_SPECFUNCS
    double a;
    double b;
    double c;
    double p;
    double q;
    double u;
    double w;
    double z;
    ae_int_t i;
    double logpi;
    double ls2pi;
    double tmp;
    double result;

    *sgngam = 0.0;

    *sgngam = (double)(1);
    logpi = 1.14472988584940017414;
    ls2pi = 0.91893853320467274178;
    if( ae_fp_less(x,-34.0) )
    {
        q = -x;
        w = lngamma(q, &tmp, _state);
        p = (double)(ae_ifloor(q, _state));
        i = ae_round(p, _state);
        if( i%2==0 )
        {
            *sgngam = (double)(-1);
        }
        else
        {
            *sgngam = (double)(1);
        }
        z = q-p;
        if( ae_fp_greater(z,0.5) )
        {
            p = p+(double)1;
            z = p-q;
        }
        z = q*ae_sin(ae_pi*z, _state);
        result = logpi-ae_log(z, _state)-w;
        return result;
    }
    if( ae_fp_less(x,(double)(13)) )
    {
        z = (double)(1);
        p = (double)(0);
        u = x;
        while(ae_fp_greater_eq(u,(double)(3)))
        {
            p = p-(double)1;
            u = x+p;
            z = z*u;
        }
        while(ae_fp_less(u,(double)(2)))
        {
            z = z/u;
            p = p+(double)1;
            u = x+p;
        }
        if( ae_fp_less(z,(double)(0)) )
        {
            *sgngam = (double)(-1);
            z = -z;
        }
        else
        {
            *sgngam = (double)(1);
        }
        if( ae_fp_eq(u,(double)(2)) )
        {
            result = ae_log(z, _state);
            return result;
        }
        p = p-(double)2;
        x = x+p;
        b = -1378.25152569120859100;
        b = -38801.6315134637840924+x*b;
        b = -331612.992738871184744+x*b;
        b = -1162370.97492762307383+x*b;
        b = -1721737.00820839662146+x*b;
        b = -853555.664245765465627+x*b;
        c = (double)(1);
        c = -351.815701436523470549+x*c;
        c = -17064.2106651881159223+x*c;
        c = -220528.590553854454839+x*c;
        c = -1139334.44367982507207+x*c;
        c = -2532523.07177582951285+x*c;
        c = -2018891.41433532773231+x*c;
        p = x*b/c;
        result = ae_log(z, _state)+p;
        return result;
    }
    q = (x-0.5)*ae_log(x, _state)-x+ls2pi;
    if( ae_fp_greater(x,(double)(100000000)) )
    {
        result = q;
        return result;
    }
    p = (double)1/(x*x);
    if( ae_fp_greater_eq(x,1000.0) )
    {
        q = q+((7.9365079365079365079365*0.0001*p-2.7777777777777777777778*0.001)*p+0.0833333333333333333333)/x;
    }
    else
    {
        a = 8.11614167470508450300*0.0001;
        a = -5.95061904284301438324*0.0001+p*a;
        a = 7.93650340457716943945*0.0001+p*a;
        a = -2.77777777730099687205*0.001+p*a;
        a = 8.33333333333331927722*0.01+p*a;
        q = q+a/x;
    }
    result = q;
    return result;
#else
    return _ialglib_i_lngamma(x, sgngam);
#endif
}


static double gammafunc_gammastirf(double x, ae_state *_state)
{
    double y;
    double w;
    double v;
    double stir;
    double result;


    w = (double)1/x;
    stir = 7.87311395793093628397E-4;
    stir = -2.29549961613378126380E-4+w*stir;
    stir = -2.68132617805781232825E-3+w*stir;
    stir = 3.47222221605458667310E-3+w*stir;
    stir = 8.33333333333482257126E-2+w*stir;
    w = (double)1+w*stir;
    y = ae_exp(x, _state);
    if( ae_fp_greater(x,143.01608) )
    {
        v = ae_pow(x, 0.5*x-0.25, _state);
        y = v*(v/y);
    }
    else
    {
        y = ae_pow(x, x-0.5, _state)/y;
    }
    result = 2.50662827463100050242*y*w;
    return result;
}


/*$ End $*/
