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
#include "normaldistr.h"


/*$ Declarations $*/
static double normaldistr_bvnintegrate3(double rangea,
     double rangeb,
     double x,
     double y,
     double gw,
     double gx,
     ae_state *_state);
static double normaldistr_bvnintegrate6(double rangea,
     double rangeb,
     double x,
     double y,
     double s,
     double gw,
     double gx,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
Error function

The integral is

                          x
                           -
                2         | |          2
  erf(x)  =  --------     |    exp( - t  ) dt.
             sqrt(pi)   | |
                         -
                          0

For 0 <= |x| < 1, erf(x) = x * P4(x**2)/Q5(x**2); otherwise
erf(x) = 1 - erfc(x).


ACCURACY:

                     Relative error:
arithmetic   domain     # trials      peak         rms
   IEEE      0,1         30000       3.7e-16     1.0e-16

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1988, 1992, 2000 by Stephen L. Moshier
*************************************************************************/
double errorfunction(double x, ae_state *_state)
{
    double xsq;
    double s;
    double p;
    double q;
    double result;


    s = (double)(ae_sign(x, _state));
    x = ae_fabs(x, _state);
    if( ae_fp_less(x,0.5) )
    {
        xsq = x*x;
        p = 0.007547728033418631287834;
        p = -0.288805137207594084924010+xsq*p;
        p = 14.3383842191748205576712+xsq*p;
        p = 38.0140318123903008244444+xsq*p;
        p = 3017.82788536507577809226+xsq*p;
        p = 7404.07142710151470082064+xsq*p;
        p = 80437.3630960840172832162+xsq*p;
        q = 0.0;
        q = 1.00000000000000000000000+xsq*q;
        q = 38.0190713951939403753468+xsq*q;
        q = 658.070155459240506326937+xsq*q;
        q = 6379.60017324428279487120+xsq*q;
        q = 34216.5257924628539769006+xsq*q;
        q = 80437.3630960840172826266+xsq*q;
        result = s*1.1283791670955125738961589031*x*p/q;
        return result;
    }
    if( ae_fp_greater_eq(x,(double)(10)) )
    {
        result = s;
        return result;
    }
    result = s*((double)1-errorfunctionc(x, _state));
    return result;
}


/*************************************************************************
Complementary error function

 1 - erf(x) =

                          inf.
                            -
                 2         | |          2
  erfc(x)  =  --------     |    exp( - t  ) dt
              sqrt(pi)   | |
                          -
                           x


For small x, erfc(x) = 1 - erf(x); otherwise rational
approximations are computed.


ACCURACY:

                     Relative error:
arithmetic   domain     # trials      peak         rms
   IEEE      0,26.6417   30000       5.7e-14     1.5e-14

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1988, 1992, 2000 by Stephen L. Moshier
*************************************************************************/
double errorfunctionc(double x, ae_state *_state)
{
    double p;
    double q;
    double result;


    if( ae_fp_less(x,(double)(0)) )
    {
        result = (double)2-errorfunctionc(-x, _state);
        return result;
    }
    if( ae_fp_less(x,0.5) )
    {
        result = 1.0-errorfunction(x, _state);
        return result;
    }
    if( ae_fp_greater_eq(x,(double)(10)) )
    {
        result = (double)(0);
        return result;
    }
    p = 0.0;
    p = 0.5641877825507397413087057563+x*p;
    p = 9.675807882987265400604202961+x*p;
    p = 77.08161730368428609781633646+x*p;
    p = 368.5196154710010637133875746+x*p;
    p = 1143.262070703886173606073338+x*p;
    p = 2320.439590251635247384768711+x*p;
    p = 2898.0293292167655611275846+x*p;
    p = 1826.3348842295112592168999+x*p;
    q = 1.0;
    q = 17.14980943627607849376131193+x*q;
    q = 137.1255960500622202878443578+x*q;
    q = 661.7361207107653469211984771+x*q;
    q = 2094.384367789539593790281779+x*q;
    q = 4429.612803883682726711528526+x*q;
    q = 6089.5424232724435504633068+x*q;
    q = 4958.82756472114071495438422+x*q;
    q = 1826.3348842295112595576438+x*q;
    result = ae_exp(-ae_sqr(x, _state), _state)*p/q;
    return result;
}


/*************************************************************************
Same as normalcdf(), obsolete name.
*************************************************************************/
double normaldistribution(double x, ae_state *_state)
{
    double result;


    result = 0.5*(errorfunction(x/1.41421356237309504880, _state)+(double)1);
    return result;
}


/*************************************************************************
Normal distribution PDF

Returns Gaussian probability density function:

               1
   f(x)  = --------- * exp(-x^2/2)
           sqrt(2pi)

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1988, 1992, 2000 by Stephen L. Moshier
*************************************************************************/
double normalpdf(double x, ae_state *_state)
{
    double result;


    ae_assert(ae_isfinite(x, _state), "NormalPDF: X is infinite", _state);
    result = ae_exp(-x*x/(double)2, _state)/ae_sqrt((double)2*ae_pi, _state);
    return result;
}


/*************************************************************************
Normal distribution CDF

Returns the area under the Gaussian probability density
function, integrated from minus infinity to x:

                           x
                            -
                  1        | |          2
   ndtr(x)  = ---------    |    exp( - t /2 ) dt
              sqrt(2pi)  | |
                          -
                         -inf.

            =  ( 1 + erf(z) ) / 2
            =  erfc(z) / 2

where z = x/sqrt(2). Computation is via the functions
erf and erfc.


ACCURACY:

                     Relative error:
arithmetic   domain     # trials      peak         rms
   IEEE     -13,0        30000       3.4e-14     6.7e-15

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1988, 1992, 2000 by Stephen L. Moshier
*************************************************************************/
double normalcdf(double x, ae_state *_state)
{
    double result;


    result = 0.5*(errorfunction(x/1.41421356237309504880, _state)+(double)1);
    return result;
}


/*************************************************************************
Inverse of the error function

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1988, 1992, 2000 by Stephen L. Moshier
*************************************************************************/
double inverf(double e, ae_state *_state)
{
    double result;


    result = invnormaldistribution(0.5*(e+(double)1), _state)/ae_sqrt((double)(2), _state);
    return result;
}


/*************************************************************************
Same as invnormalcdf(), deprecated name
*************************************************************************/
double invnormaldistribution(double y0, ae_state *_state)
{
    double result;


    result = invnormalcdf(y0, _state);
    return result;
}


/*************************************************************************
Inverse of Normal CDF

Returns the argument, x, for which the area under the
Gaussian probability density function (integrated from
minus infinity to x) is equal to y.


For small arguments 0 < y < exp(-2), the program computes
z = sqrt( -2.0 * log(y) );  then the approximation is
x = z - log(z)/z  - (1/z) P(1/z) / Q(1/z).
There are two rational functions P/Q, one for 0 < y < exp(-32)
and the other for y up to exp(-2).  For larger arguments,
w = y - 0.5, and  x/sqrt(2pi) = w + w**3 R(w**2)/S(w**2)).

ACCURACY:

                     Relative error:
arithmetic   domain        # trials      peak         rms
   IEEE     0.125, 1        20000       7.2e-16     1.3e-16
   IEEE     3e-308, 0.135   50000       4.6e-16     9.8e-17

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1988, 1992, 2000 by Stephen L. Moshier
*************************************************************************/
double invnormalcdf(double y0, ae_state *_state)
{
    double expm2;
    double s2pi;
    double x;
    double y;
    double z;
    double y2;
    double x0;
    double x1;
    ae_int_t code;
    double p0;
    double q0;
    double p1;
    double q1;
    double p2;
    double q2;
    double result;


    expm2 = 0.13533528323661269189;
    s2pi = 2.50662827463100050242;
    if( ae_fp_less_eq(y0,(double)(0)) )
    {
        result = -ae_maxrealnumber;
        return result;
    }
    if( ae_fp_greater_eq(y0,(double)(1)) )
    {
        result = ae_maxrealnumber;
        return result;
    }
    code = 1;
    y = y0;
    if( ae_fp_greater(y,1.0-expm2) )
    {
        y = 1.0-y;
        code = 0;
    }
    if( ae_fp_greater(y,expm2) )
    {
        y = y-0.5;
        y2 = y*y;
        p0 = -59.9633501014107895267;
        p0 = 98.0010754185999661536+y2*p0;
        p0 = -56.6762857469070293439+y2*p0;
        p0 = 13.9312609387279679503+y2*p0;
        p0 = -1.23916583867381258016+y2*p0;
        q0 = (double)(1);
        q0 = 1.95448858338141759834+y2*q0;
        q0 = 4.67627912898881538453+y2*q0;
        q0 = 86.3602421390890590575+y2*q0;
        q0 = -225.462687854119370527+y2*q0;
        q0 = 200.260212380060660359+y2*q0;
        q0 = -82.0372256168333339912+y2*q0;
        q0 = 15.9056225126211695515+y2*q0;
        q0 = -1.18331621121330003142+y2*q0;
        x = y+y*y2*p0/q0;
        x = x*s2pi;
        result = x;
        return result;
    }
    x = ae_sqrt(-2.0*ae_log(y, _state), _state);
    x0 = x-ae_log(x, _state)/x;
    z = 1.0/x;
    if( ae_fp_less(x,8.0) )
    {
        p1 = 4.05544892305962419923;
        p1 = 31.5251094599893866154+z*p1;
        p1 = 57.1628192246421288162+z*p1;
        p1 = 44.0805073893200834700+z*p1;
        p1 = 14.6849561928858024014+z*p1;
        p1 = 2.18663306850790267539+z*p1;
        p1 = -1.40256079171354495875*0.1+z*p1;
        p1 = -3.50424626827848203418*0.01+z*p1;
        p1 = -8.57456785154685413611*0.0001+z*p1;
        q1 = (double)(1);
        q1 = 15.7799883256466749731+z*q1;
        q1 = 45.3907635128879210584+z*q1;
        q1 = 41.3172038254672030440+z*q1;
        q1 = 15.0425385692907503408+z*q1;
        q1 = 2.50464946208309415979+z*q1;
        q1 = -1.42182922854787788574*0.1+z*q1;
        q1 = -3.80806407691578277194*0.01+z*q1;
        q1 = -9.33259480895457427372*0.0001+z*q1;
        x1 = z*p1/q1;
    }
    else
    {
        p2 = 3.23774891776946035970;
        p2 = 6.91522889068984211695+z*p2;
        p2 = 3.93881025292474443415+z*p2;
        p2 = 1.33303460815807542389+z*p2;
        p2 = 2.01485389549179081538*0.1+z*p2;
        p2 = 1.23716634817820021358*0.01+z*p2;
        p2 = 3.01581553508235416007*0.0001+z*p2;
        p2 = 2.65806974686737550832*0.000001+z*p2;
        p2 = 6.23974539184983293730*0.000000001+z*p2;
        q2 = (double)(1);
        q2 = 6.02427039364742014255+z*q2;
        q2 = 3.67983563856160859403+z*q2;
        q2 = 1.37702099489081330271+z*q2;
        q2 = 2.16236993594496635890*0.1+z*q2;
        q2 = 1.34204006088543189037*0.01+z*q2;
        q2 = 3.28014464682127739104*0.0001+z*q2;
        q2 = 2.89247864745380683936*0.000001+z*q2;
        q2 = 6.79019408009981274425*0.000000001+z*q2;
        x1 = z*p2/q2;
    }
    x = x0-x1;
    if( code!=0 )
    {
        x = -x;
    }
    result = x;
    return result;
}


/*************************************************************************
Bivariate normal PDF

Returns probability density function of the bivariate  Gaussian  with
correlation parameter equal to Rho:

                         1              (    x^2 - 2*rho*x*y + y^2  )
    f(x,y,rho) = ----------------- * exp( - ----------------------- )
                 2pi*sqrt(1-rho^2)      (        2*(1-rho^2)        )


with -1<rho<+1 and arbitrary x, y.

This function won't fail as long as Rho is in (-1,+1) range.

  -- ALGLIB --
     Copyright 15.11.2019 by Bochkanov Sergey
*************************************************************************/
double bivariatenormalpdf(double x,
     double y,
     double rho,
     ae_state *_state)
{
    double onerho2;
    double result;


    ae_assert(ae_isfinite(x, _state), "BivariateNormalCDF: X is infinite", _state);
    ae_assert(ae_isfinite(y, _state), "BivariateNormalCDF: Y is infinite", _state);
    ae_assert(ae_isfinite(rho, _state), "BivariateNormalCDF: Rho is infinite", _state);
    ae_assert(ae_fp_less((double)(-1),rho)&&ae_fp_less(rho,(double)(1)), "BivariateNormalCDF: Rho is not in (-1,+1) range", _state);
    onerho2 = ((double)1-rho)*((double)1+rho);
    result = ae_exp(-(x*x+y*y-(double)2*rho*x*y)/((double)2*onerho2), _state)/((double)2*ae_pi*ae_sqrt(onerho2, _state));
    return result;
}


/*************************************************************************
Bivariate normal CDF

Returns the area under the bivariate Gaussian  PDF  with  correlation
parameter equal to Rho, integrated from minus infinity to (x,y):


                                          x      y
                                          -      -  
                            1            | |    | | 
    bvn(x,y,rho) = -------------------   |      |   f(u,v,rho)*du*dv
                    2pi*sqrt(1-rho^2)  | |    | |    
                                        -      -
                                       -INF   -INF

                                       
where

                      (    u^2 - 2*rho*u*v + v^2  )
    f(u,v,rho)   = exp( - ----------------------- )
                      (        2*(1-rho^2)        )


with -1<rho<+1 and arbitrary x, y.

This subroutine uses high-precision approximation scheme proposed  by
Alan Genz in "Numerical  Computation  of  Rectangular  Bivariate  and
Trivariate Normal and  t  probabilities",  which  computes  CDF  with
absolute error roughly equal to 1e-14.

This function won't fail as long as Rho is in (-1,+1) range.

  -- ALGLIB --
     Copyright 15.11.2019 by Bochkanov Sergey
*************************************************************************/
double bivariatenormalcdf(double x,
     double y,
     double rho,
     ae_state *_state)
{
    double rangea;
    double rangeb;
    double s;
    double v;
    double v0;
    double v1;
    double fxys;
    double ta;
    double tb;
    double tc;
    double result;


    ae_assert(ae_isfinite(x, _state), "BivariateNormalCDF: X is infinite", _state);
    ae_assert(ae_isfinite(y, _state), "BivariateNormalCDF: Y is infinite", _state);
    ae_assert(ae_isfinite(rho, _state), "BivariateNormalCDF: Rho is infinite", _state);
    ae_assert(ae_fp_less((double)(-1),rho)&&ae_fp_less(rho,(double)(1)), "BivariateNormalCDF: Rho is not in (-1,+1) range", _state);
    if( ae_fp_eq(rho,(double)(0)) )
    {
        result = normalcdf(x, _state)*normalcdf(y, _state);
        return result;
    }
    if( ae_fp_less_eq(ae_fabs(rho, _state),0.8) )
    {
        
        /*
         * Rho is small, compute integral using using formula (3) by Alan Genz, integrated
         * by means of 10-point Gauss-Legendre quadrature
         */
        rangea = (double)(0);
        rangeb = ae_asin(rho, _state);
        v = (double)(0);
        v = v+normaldistr_bvnintegrate3(rangea, rangeb, x, y, 0.2491470458134028, -0.1252334085114689, _state);
        v = v+normaldistr_bvnintegrate3(rangea, rangeb, x, y, 0.2491470458134028, 0.1252334085114689, _state);
        v = v+normaldistr_bvnintegrate3(rangea, rangeb, x, y, 0.2334925365383548, -0.3678314989981802, _state);
        v = v+normaldistr_bvnintegrate3(rangea, rangeb, x, y, 0.2334925365383548, 0.3678314989981802, _state);
        v = v+normaldistr_bvnintegrate3(rangea, rangeb, x, y, 0.2031674267230659, -0.5873179542866175, _state);
        v = v+normaldistr_bvnintegrate3(rangea, rangeb, x, y, 0.2031674267230659, 0.5873179542866175, _state);
        v = v+normaldistr_bvnintegrate3(rangea, rangeb, x, y, 0.1600783285433462, -0.7699026741943047, _state);
        v = v+normaldistr_bvnintegrate3(rangea, rangeb, x, y, 0.1600783285433462, 0.7699026741943047, _state);
        v = v+normaldistr_bvnintegrate3(rangea, rangeb, x, y, 0.1069393259953184, -0.9041172563704749, _state);
        v = v+normaldistr_bvnintegrate3(rangea, rangeb, x, y, 0.1069393259953184, 0.9041172563704749, _state);
        v = v+normaldistr_bvnintegrate3(rangea, rangeb, x, y, 0.0471753363865118, -0.9815606342467192, _state);
        v = v+normaldistr_bvnintegrate3(rangea, rangeb, x, y, 0.0471753363865118, 0.9815606342467192, _state);
        v = v*0.5*(rangeb-rangea)/((double)2*ae_pi);
        result = normalcdf(x, _state)*normalcdf(y, _state)+v;
    }
    else
    {
        
        /*
         * Rho is large, compute integral using using formula (6) by Alan Genz, integrated
         * by means of 20-point Gauss-Legendre quadrature.
         */
        x = -x;
        y = -y;
        s = (double)(ae_sign(rho, _state));
        if( ae_fp_greater(s,(double)(0)) )
        {
            fxys = normalcdf(-ae_maxreal(x, y, _state), _state);
        }
        else
        {
            fxys = ae_maxreal(0.0, normalcdf(-x, _state)-normalcdf(y, _state), _state);
        }
        rangea = (double)(0);
        rangeb = ae_sqrt(((double)1-rho)*((double)1+rho), _state);
        
        /*
         * Compute first term (analytic integral) from formula (6)
         */
        ta = rangeb;
        tb = ae_fabs(x-s*y, _state);
        tc = ((double)4-s*x*y)/(double)8;
        v0 = ta*((double)1-tc*(tb*tb-ta*ta)/(double)3)*ae_exp(-tb*tb/((double)2*ta*ta), _state)-tb*((double)1-tc*tb*tb/(double)3)*ae_sqrt((double)2*ae_pi, _state)*normalcdf(-tb/ta, _state);
        v0 = v0*ae_exp(-s*x*y/(double)2, _state)/((double)2*ae_pi);
        
        /*
         * Compute second term (numerical integral, 20-point Gauss-Legendre rule) from formula (6)
         */
        v1 = (double)(0);
        v1 = v1+normaldistr_bvnintegrate6(rangea, rangeb, x, y, s, 0.1527533871307258, -0.0765265211334973, _state);
        v1 = v1+normaldistr_bvnintegrate6(rangea, rangeb, x, y, s, 0.1527533871307258, 0.0765265211334973, _state);
        v1 = v1+normaldistr_bvnintegrate6(rangea, rangeb, x, y, s, 0.1491729864726037, -0.2277858511416451, _state);
        v1 = v1+normaldistr_bvnintegrate6(rangea, rangeb, x, y, s, 0.1491729864726037, 0.2277858511416451, _state);
        v1 = v1+normaldistr_bvnintegrate6(rangea, rangeb, x, y, s, 0.1420961093183820, -0.3737060887154195, _state);
        v1 = v1+normaldistr_bvnintegrate6(rangea, rangeb, x, y, s, 0.1420961093183820, 0.3737060887154195, _state);
        v1 = v1+normaldistr_bvnintegrate6(rangea, rangeb, x, y, s, 0.1316886384491766, -0.5108670019508271, _state);
        v1 = v1+normaldistr_bvnintegrate6(rangea, rangeb, x, y, s, 0.1316886384491766, 0.5108670019508271, _state);
        v1 = v1+normaldistr_bvnintegrate6(rangea, rangeb, x, y, s, 0.1181945319615184, -0.6360536807265150, _state);
        v1 = v1+normaldistr_bvnintegrate6(rangea, rangeb, x, y, s, 0.1181945319615184, 0.6360536807265150, _state);
        v1 = v1+normaldistr_bvnintegrate6(rangea, rangeb, x, y, s, 0.1019301198172404, -0.7463319064601508, _state);
        v1 = v1+normaldistr_bvnintegrate6(rangea, rangeb, x, y, s, 0.1019301198172404, 0.7463319064601508, _state);
        v1 = v1+normaldistr_bvnintegrate6(rangea, rangeb, x, y, s, 0.0832767415767048, -0.8391169718222188, _state);
        v1 = v1+normaldistr_bvnintegrate6(rangea, rangeb, x, y, s, 0.0832767415767048, 0.8391169718222188, _state);
        v1 = v1+normaldistr_bvnintegrate6(rangea, rangeb, x, y, s, 0.0626720483341091, -0.9122344282513259, _state);
        v1 = v1+normaldistr_bvnintegrate6(rangea, rangeb, x, y, s, 0.0626720483341091, 0.9122344282513259, _state);
        v1 = v1+normaldistr_bvnintegrate6(rangea, rangeb, x, y, s, 0.0406014298003869, -0.9639719272779138, _state);
        v1 = v1+normaldistr_bvnintegrate6(rangea, rangeb, x, y, s, 0.0406014298003869, 0.9639719272779138, _state);
        v1 = v1+normaldistr_bvnintegrate6(rangea, rangeb, x, y, s, 0.0176140071391521, -0.9931285991850949, _state);
        v1 = v1+normaldistr_bvnintegrate6(rangea, rangeb, x, y, s, 0.0176140071391521, 0.9931285991850949, _state);
        v1 = v1*0.5*(rangeb-rangea)/((double)2*ae_pi);
        result = fxys-s*(v0+v1);
    }
    result = ae_maxreal(result, (double)(0), _state);
    result = ae_minreal(result, (double)(1), _state);
    return result;
}


/*************************************************************************
Internal function which computes integrand of  formula  (3)  by  Alan
Genz times Gaussian weights (passed by user).

  -- ALGLIB --
     Copyright 15.11.2019 by Bochkanov Sergey
*************************************************************************/
static double normaldistr_bvnintegrate3(double rangea,
     double rangeb,
     double x,
     double y,
     double gw,
     double gx,
     ae_state *_state)
{
    double r;
    double t2;
    double dd;
    double sinr;
    double cosr;
    double result;


    r = (rangeb-rangea)*0.5*gx+(rangeb+rangea)*0.5;
    t2 = ae_tan(0.5*r, _state);
    dd = (double)1/((double)1+t2*t2);
    sinr = (double)2*t2*dd;
    cosr = ((double)1-t2*t2)*dd;
    result = gw*ae_exp(-(x*x+y*y-(double)2*x*y*sinr)/((double)2*cosr*cosr), _state);
    return result;
}


/*************************************************************************
Internal function which computes integrand of  formula  (6)  by  Alan
Genz times Gaussian weights (passed by user).

  -- ALGLIB --
     Copyright 15.11.2019 by Bochkanov Sergey
*************************************************************************/
static double normaldistr_bvnintegrate6(double rangea,
     double rangeb,
     double x,
     double y,
     double s,
     double gw,
     double gx,
     ae_state *_state)
{
    double r;
    double exphsk22x2;
    double exphsk2;
    double sqrt1x2;
    double exphsk1sqrt1x2;
    double result;


    r = (rangeb-rangea)*0.5*gx+(rangeb+rangea)*0.5;
    exphsk22x2 = ae_exp(-(x-s*y)*(x-s*y)/((double)2*r*r), _state);
    exphsk2 = ae_exp(-x*s*y/(double)2, _state);
    sqrt1x2 = ae_sqrt(((double)1-r)*((double)1+r), _state);
    exphsk1sqrt1x2 = ae_exp(-x*s*y/((double)1+sqrt1x2), _state);
    result = gw*exphsk22x2*(exphsk1sqrt1x2/sqrt1x2-exphsk2*((double)1+((double)4-x*y*s)*r*r/(double)8));
    return result;
}


/*$ End $*/
