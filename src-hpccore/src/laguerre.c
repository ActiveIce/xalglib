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
#include "laguerre.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Calculation of the value of the Laguerre polynomial.

Parameters:
    n   -   degree, n>=0
    x   -   argument

Result:
    the value of the Laguerre polynomial Ln at x
*************************************************************************/
double laguerrecalculate(ae_int_t n, double x, ae_state *_state)
{
    double a;
    double b;
    double i;
    double result;


    result = (double)(1);
    a = (double)(1);
    b = (double)1-x;
    if( n==1 )
    {
        result = b;
    }
    i = (double)(2);
    while(ae_fp_less_eq(i,(double)(n)))
    {
        result = (((double)2*i-(double)1-x)*b-(i-(double)1)*a)/i;
        a = b;
        b = result;
        i = i+(double)1;
    }
    return result;
}


/*************************************************************************
Summation of Laguerre polynomials using Clenshaw's recurrence formula.

This routine calculates c[0]*L0(x) + c[1]*L1(x) + ... + c[N]*LN(x)

Parameters:
    n   -   degree, n>=0
    x   -   argument

Result:
    the value of the Laguerre polynomial at x
*************************************************************************/
double laguerresum(/* Real    */ const ae_vector* c,
     ae_int_t n,
     double x,
     ae_state *_state)
{
    double b1;
    double b2;
    ae_int_t i;
    double result;


    b1 = (double)(0);
    b2 = (double)(0);
    result = (double)(0);
    for(i=n; i>=0; i--)
    {
        result = ((double)(2*i+1)-x)*b1/(double)(i+1)-(double)(i+1)*b2/(double)(i+2)+c->ptr.p_double[i];
        b2 = b1;
        b1 = result;
    }
    return result;
}


/*************************************************************************
Representation of Ln as C[0] + C[1]*X + ... + C[N]*X^N

Input parameters:
    N   -   polynomial degree, n>=0

Output parameters:
    C   -   coefficients
*************************************************************************/
void laguerrecoefficients(ae_int_t n,
     /* Real    */ ae_vector* c,
     ae_state *_state)
{
    ae_int_t i;

    ae_vector_clear(c);

    ae_vector_set_length(c, n+1, _state);
    c->ptr.p_double[0] = (double)(1);
    for(i=0; i<=n-1; i++)
    {
        c->ptr.p_double[i+1] = -c->ptr.p_double[i]*(double)(n-i)/(double)(i+1)/(double)(i+1);
    }
}


/*$ End $*/
