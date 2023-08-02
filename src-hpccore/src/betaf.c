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
#include "betaf.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Beta function


                  -     -
                 | (a) | (b)
beta( a, b )  =  -----------.
                    -
                   | (a+b)

For large arguments the logarithm of the function is
evaluated using lgam(), then exponentiated.

ACCURACY:

                     Relative error:
arithmetic   domain     # trials      peak         rms
   IEEE       0,30       30000       8.1e-14     1.1e-14

Cephes Math Library Release 2.0:  April, 1987
Copyright 1984, 1987 by Stephen L. Moshier
*************************************************************************/
double beta(double a, double b, ae_state *_state)
{
    double y;
    double sg;
    double s;
    double result;


    sg = (double)(1);
    ae_assert(ae_fp_greater(a,(double)(0))||ae_fp_neq(a,(double)(ae_ifloor(a, _state))), "Overflow in Beta", _state);
    ae_assert(ae_fp_greater(b,(double)(0))||ae_fp_neq(b,(double)(ae_ifloor(b, _state))), "Overflow in Beta", _state);
    y = a+b;
    if( ae_fp_greater(ae_fabs(y, _state),171.624376956302725) )
    {
        y = lngamma(y, &s, _state);
        sg = sg*s;
        y = lngamma(b, &s, _state)-y;
        sg = sg*s;
        y = lngamma(a, &s, _state)+y;
        sg = sg*s;
        ae_assert(ae_fp_less_eq(y,ae_log(ae_maxrealnumber, _state)), "Overflow in Beta", _state);
        result = sg*ae_exp(y, _state);
        return result;
    }
    y = gammafunction(y, _state);
    ae_assert(ae_fp_neq(y,(double)(0)), "Overflow in Beta", _state);
    if( ae_fp_greater(a,b) )
    {
        y = gammafunction(a, _state)/y;
        y = y*gammafunction(b, _state);
    }
    else
    {
        y = gammafunction(b, _state)/y;
        y = y*gammafunction(a, _state);
    }
    result = y;
    return result;
}


/*$ End $*/
