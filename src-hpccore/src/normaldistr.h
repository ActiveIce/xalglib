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

#ifndef _normaldistr_h
#define _normaldistr_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "xdebug.h"
#include "hqrnd.h"


/*$ Declarations $*/


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
double errorfunction(double x, ae_state *_state);


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
double errorfunctionc(double x, ae_state *_state);


/*************************************************************************
Same as normalcdf(), obsolete name.
*************************************************************************/
double normaldistribution(double x, ae_state *_state);


/*************************************************************************
Normal distribution PDF

Returns Gaussian probability density function:

               1
   f(x)  = --------- * exp(-x^2/2)
           sqrt(2pi)

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1988, 1992, 2000 by Stephen L. Moshier
*************************************************************************/
double normalpdf(double x, ae_state *_state);


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
double normalcdf(double x, ae_state *_state);


/*************************************************************************
Inverse of the error function

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1988, 1992, 2000 by Stephen L. Moshier
*************************************************************************/
double inverf(double e, ae_state *_state);


/*************************************************************************
Same as invnormalcdf(), deprecated name
*************************************************************************/
double invnormaldistribution(double y0, ae_state *_state);


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
double invnormalcdf(double y0, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


/*$ End $*/
#endif

