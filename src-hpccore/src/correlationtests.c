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
#include "correlationtests.h"


/*$ Declarations $*/
static double correlationtests_spearmantail5(double s, ae_state *_state);
static double correlationtests_spearmantail6(double s, ae_state *_state);
static double correlationtests_spearmantail7(double s, ae_state *_state);
static double correlationtests_spearmantail8(double s, ae_state *_state);
static double correlationtests_spearmantail9(double s, ae_state *_state);
static double correlationtests_spearmantail(double t,
     ae_int_t n,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
Pearson's correlation coefficient significance test

This test checks hypotheses about whether X  and  Y  are  samples  of  two
continuous  distributions  having  zero  correlation  or   whether   their
correlation is non-zero.

The following tests are performed:
    * two-tailed test (null hypothesis - X and Y have zero correlation)
    * left-tailed test (null hypothesis - the correlation  coefficient  is
      greater than or equal to 0)
    * right-tailed test (null hypothesis - the correlation coefficient  is
      less than or equal to 0).

Requirements:
    * the number of elements in each sample is not less than 5
    * normality of distributions of X and Y.

Input parameters:
    R   -   Pearson's correlation coefficient for X and Y
    N   -   number of elements in samples, N>=5.

Output parameters:
    BothTails   -   p-value for two-tailed test.
                    If BothTails is less than the given significance level
                    the null hypothesis is rejected.
    LeftTail    -   p-value for left-tailed test.
                    If LeftTail is less than the given significance level,
                    the null hypothesis is rejected.
    RightTail   -   p-value for right-tailed test.
                    If RightTail is less than the given significance level
                    the null hypothesis is rejected.

  -- ALGLIB --
     Copyright 09.04.2007 by Bochkanov Sergey
*************************************************************************/
void pearsoncorrelationsignificance(double r,
     ae_int_t n,
     double* bothtails,
     double* lefttail,
     double* righttail,
     ae_state *_state)
{
    double t;
    double p;

    *bothtails = 0.0;
    *lefttail = 0.0;
    *righttail = 0.0;

    
    /*
     * Some special cases
     */
    if( ae_fp_greater_eq(r,(double)(1)) )
    {
        *bothtails = 0.0;
        *lefttail = 1.0;
        *righttail = 0.0;
        return;
    }
    if( ae_fp_less_eq(r,(double)(-1)) )
    {
        *bothtails = 0.0;
        *lefttail = 0.0;
        *righttail = 1.0;
        return;
    }
    if( n<5 )
    {
        *bothtails = 1.0;
        *lefttail = 1.0;
        *righttail = 1.0;
        return;
    }
    
    /*
     * General case
     */
    t = r*ae_sqrt((double)(n-2)/((double)1-ae_sqr(r, _state)), _state);
    p = studenttdistribution(n-2, t, _state);
    *bothtails = (double)2*ae_minreal(p, (double)1-p, _state);
    *lefttail = p;
    *righttail = (double)1-p;
}


/*************************************************************************
Spearman's rank correlation coefficient significance test

This test checks hypotheses about whether X  and  Y  are  samples  of  two
continuous  distributions  having  zero  correlation  or   whether   their
correlation is non-zero.

The following tests are performed:
    * two-tailed test (null hypothesis - X and Y have zero correlation)
    * left-tailed test (null hypothesis - the correlation  coefficient  is
      greater than or equal to 0)
    * right-tailed test (null hypothesis - the correlation coefficient  is
      less than or equal to 0).

Requirements:
    * the number of elements in each sample is not less than 5.

The test is non-parametric and doesn't require distributions X and Y to be
normal.

Input parameters:
    R   -   Spearman's rank correlation coefficient for X and Y
    N   -   number of elements in samples, N>=5.

Output parameters:
    BothTails   -   p-value for two-tailed test.
                    If BothTails is less than the given significance level
                    the null hypothesis is rejected.
    LeftTail    -   p-value for left-tailed test.
                    If LeftTail is less than the given significance level,
                    the null hypothesis is rejected.
    RightTail   -   p-value for right-tailed test.
                    If RightTail is less than the given significance level
                    the null hypothesis is rejected.

  -- ALGLIB --
     Copyright 09.04.2007 by Bochkanov Sergey
*************************************************************************/
void spearmanrankcorrelationsignificance(double r,
     ae_int_t n,
     double* bothtails,
     double* lefttail,
     double* righttail,
     ae_state *_state)
{
    double t;
    double p;

    *bothtails = 0.0;
    *lefttail = 0.0;
    *righttail = 0.0;

    
    /*
     * Special case
     */
    if( n<5 )
    {
        *bothtails = 1.0;
        *lefttail = 1.0;
        *righttail = 1.0;
        return;
    }
    
    /*
     * General case
     */
    if( ae_fp_greater_eq(r,(double)(1)) )
    {
        t = 1.0E10;
    }
    else
    {
        if( ae_fp_less_eq(r,(double)(-1)) )
        {
            t = -1.0E10;
        }
        else
        {
            t = r*ae_sqrt((double)(n-2)/((double)1-ae_sqr(r, _state)), _state);
        }
    }
    if( ae_fp_less(t,(double)(0)) )
    {
        p = correlationtests_spearmantail(t, n, _state);
        *bothtails = (double)2*p;
        *lefttail = p;
        *righttail = (double)1-p;
    }
    else
    {
        p = correlationtests_spearmantail(-t, n, _state);
        *bothtails = (double)2*p;
        *lefttail = (double)1-p;
        *righttail = p;
    }
}


/*************************************************************************
Tail(S, 5)
*************************************************************************/
static double correlationtests_spearmantail5(double s, ae_state *_state)
{
    double result;


    if( ae_fp_less(s,0.000e+00) )
    {
        result = studenttdistribution(3, -s, _state);
        return result;
    }
    if( ae_fp_greater_eq(s,3.580e+00) )
    {
        result = 8.304e-03;
        return result;
    }
    if( ae_fp_greater_eq(s,2.322e+00) )
    {
        result = 4.163e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,1.704e+00) )
    {
        result = 6.641e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,1.303e+00) )
    {
        result = 1.164e-01;
        return result;
    }
    if( ae_fp_greater_eq(s,1.003e+00) )
    {
        result = 1.748e-01;
        return result;
    }
    if( ae_fp_greater_eq(s,7.584e-01) )
    {
        result = 2.249e-01;
        return result;
    }
    if( ae_fp_greater_eq(s,5.468e-01) )
    {
        result = 2.581e-01;
        return result;
    }
    if( ae_fp_greater_eq(s,3.555e-01) )
    {
        result = 3.413e-01;
        return result;
    }
    if( ae_fp_greater_eq(s,1.759e-01) )
    {
        result = 3.911e-01;
        return result;
    }
    if( ae_fp_greater_eq(s,1.741e-03) )
    {
        result = 4.747e-01;
        return result;
    }
    if( ae_fp_greater_eq(s,0.000e+00) )
    {
        result = 5.248e-01;
        return result;
    }
    result = (double)(0);
    return result;
}


/*************************************************************************
Tail(S, 6)
*************************************************************************/
static double correlationtests_spearmantail6(double s, ae_state *_state)
{
    double result;


    if( ae_fp_less(s,1.001e+00) )
    {
        result = studenttdistribution(4, -s, _state);
        return result;
    }
    if( ae_fp_greater_eq(s,5.663e+00) )
    {
        result = 1.366e-03;
        return result;
    }
    if( ae_fp_greater_eq(s,3.834e+00) )
    {
        result = 8.350e-03;
        return result;
    }
    if( ae_fp_greater_eq(s,2.968e+00) )
    {
        result = 1.668e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.430e+00) )
    {
        result = 2.921e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.045e+00) )
    {
        result = 5.144e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,1.747e+00) )
    {
        result = 6.797e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,1.502e+00) )
    {
        result = 8.752e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,1.295e+00) )
    {
        result = 1.210e-01;
        return result;
    }
    if( ae_fp_greater_eq(s,1.113e+00) )
    {
        result = 1.487e-01;
        return result;
    }
    if( ae_fp_greater_eq(s,1.001e+00) )
    {
        result = 1.780e-01;
        return result;
    }
    result = (double)(0);
    return result;
}


/*************************************************************************
Tail(S, 7)
*************************************************************************/
static double correlationtests_spearmantail7(double s, ae_state *_state)
{
    double result;


    if( ae_fp_less(s,1.001e+00) )
    {
        result = studenttdistribution(5, -s, _state);
        return result;
    }
    if( ae_fp_greater_eq(s,8.159e+00) )
    {
        result = 2.081e-04;
        return result;
    }
    if( ae_fp_greater_eq(s,5.620e+00) )
    {
        result = 1.393e-03;
        return result;
    }
    if( ae_fp_greater_eq(s,4.445e+00) )
    {
        result = 3.398e-03;
        return result;
    }
    if( ae_fp_greater_eq(s,3.728e+00) )
    {
        result = 6.187e-03;
        return result;
    }
    if( ae_fp_greater_eq(s,3.226e+00) )
    {
        result = 1.200e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.844e+00) )
    {
        result = 1.712e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.539e+00) )
    {
        result = 2.408e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.285e+00) )
    {
        result = 3.320e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.068e+00) )
    {
        result = 4.406e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,1.879e+00) )
    {
        result = 5.478e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,1.710e+00) )
    {
        result = 6.946e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,1.559e+00) )
    {
        result = 8.331e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,1.420e+00) )
    {
        result = 1.001e-01;
        return result;
    }
    if( ae_fp_greater_eq(s,1.292e+00) )
    {
        result = 1.180e-01;
        return result;
    }
    if( ae_fp_greater_eq(s,1.173e+00) )
    {
        result = 1.335e-01;
        return result;
    }
    if( ae_fp_greater_eq(s,1.062e+00) )
    {
        result = 1.513e-01;
        return result;
    }
    if( ae_fp_greater_eq(s,1.001e+00) )
    {
        result = 1.770e-01;
        return result;
    }
    result = (double)(0);
    return result;
}


/*************************************************************************
Tail(S, 8)
*************************************************************************/
static double correlationtests_spearmantail8(double s, ae_state *_state)
{
    double result;


    if( ae_fp_less(s,2.001e+00) )
    {
        result = studenttdistribution(6, -s, _state);
        return result;
    }
    if( ae_fp_greater_eq(s,1.103e+01) )
    {
        result = 2.194e-05;
        return result;
    }
    if( ae_fp_greater_eq(s,7.685e+00) )
    {
        result = 2.008e-04;
        return result;
    }
    if( ae_fp_greater_eq(s,6.143e+00) )
    {
        result = 5.686e-04;
        return result;
    }
    if( ae_fp_greater_eq(s,5.213e+00) )
    {
        result = 1.138e-03;
        return result;
    }
    if( ae_fp_greater_eq(s,4.567e+00) )
    {
        result = 2.310e-03;
        return result;
    }
    if( ae_fp_greater_eq(s,4.081e+00) )
    {
        result = 3.634e-03;
        return result;
    }
    if( ae_fp_greater_eq(s,3.697e+00) )
    {
        result = 5.369e-03;
        return result;
    }
    if( ae_fp_greater_eq(s,3.381e+00) )
    {
        result = 7.708e-03;
        return result;
    }
    if( ae_fp_greater_eq(s,3.114e+00) )
    {
        result = 1.087e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.884e+00) )
    {
        result = 1.397e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.682e+00) )
    {
        result = 1.838e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.502e+00) )
    {
        result = 2.288e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.340e+00) )
    {
        result = 2.883e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.192e+00) )
    {
        result = 3.469e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.057e+00) )
    {
        result = 4.144e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.001e+00) )
    {
        result = 4.804e-02;
        return result;
    }
    result = (double)(0);
    return result;
}


/*************************************************************************
Tail(S, 9)
*************************************************************************/
static double correlationtests_spearmantail9(double s, ae_state *_state)
{
    double result;


    if( ae_fp_less(s,2.001e+00) )
    {
        result = studenttdistribution(7, -s, _state);
        return result;
    }
    if( ae_fp_greater_eq(s,9.989e+00) )
    {
        result = 2.306e-05;
        return result;
    }
    if( ae_fp_greater_eq(s,8.069e+00) )
    {
        result = 8.167e-05;
        return result;
    }
    if( ae_fp_greater_eq(s,6.890e+00) )
    {
        result = 1.744e-04;
        return result;
    }
    if( ae_fp_greater_eq(s,6.077e+00) )
    {
        result = 3.625e-04;
        return result;
    }
    if( ae_fp_greater_eq(s,5.469e+00) )
    {
        result = 6.450e-04;
        return result;
    }
    if( ae_fp_greater_eq(s,4.991e+00) )
    {
        result = 1.001e-03;
        return result;
    }
    if( ae_fp_greater_eq(s,4.600e+00) )
    {
        result = 1.514e-03;
        return result;
    }
    if( ae_fp_greater_eq(s,4.272e+00) )
    {
        result = 2.213e-03;
        return result;
    }
    if( ae_fp_greater_eq(s,3.991e+00) )
    {
        result = 2.990e-03;
        return result;
    }
    if( ae_fp_greater_eq(s,3.746e+00) )
    {
        result = 4.101e-03;
        return result;
    }
    if( ae_fp_greater_eq(s,3.530e+00) )
    {
        result = 5.355e-03;
        return result;
    }
    if( ae_fp_greater_eq(s,3.336e+00) )
    {
        result = 6.887e-03;
        return result;
    }
    if( ae_fp_greater_eq(s,3.161e+00) )
    {
        result = 8.598e-03;
        return result;
    }
    if( ae_fp_greater_eq(s,3.002e+00) )
    {
        result = 1.065e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.855e+00) )
    {
        result = 1.268e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.720e+00) )
    {
        result = 1.552e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.595e+00) )
    {
        result = 1.836e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.477e+00) )
    {
        result = 2.158e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.368e+00) )
    {
        result = 2.512e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.264e+00) )
    {
        result = 2.942e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.166e+00) )
    {
        result = 3.325e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.073e+00) )
    {
        result = 3.800e-02;
        return result;
    }
    if( ae_fp_greater_eq(s,2.001e+00) )
    {
        result = 4.285e-02;
        return result;
    }
    result = (double)(0);
    return result;
}


/*************************************************************************
Tail(T,N), accepts T<0
*************************************************************************/
static double correlationtests_spearmantail(double t,
     ae_int_t n,
     ae_state *_state)
{
    double result;


    if( n==5 )
    {
        result = correlationtests_spearmantail5(-t, _state);
        return result;
    }
    if( n==6 )
    {
        result = correlationtests_spearmantail6(-t, _state);
        return result;
    }
    if( n==7 )
    {
        result = correlationtests_spearmantail7(-t, _state);
        return result;
    }
    if( n==8 )
    {
        result = correlationtests_spearmantail8(-t, _state);
        return result;
    }
    if( n==9 )
    {
        result = correlationtests_spearmantail9(-t, _state);
        return result;
    }
    result = studenttdistribution(n-2, t, _state);
    return result;
}


/*$ End $*/
