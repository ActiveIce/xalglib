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
#include "variancetests.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Two-sample F-test

This test checks three hypotheses about dispersions of the given  samples.
The following tests are performed:
    * two-tailed test (null hypothesis - the dispersions are equal)
    * left-tailed test (null hypothesis  -  the  dispersion  of  the first
      sample is greater than or equal to  the  dispersion  of  the  second
      sample).
    * right-tailed test (null hypothesis - the  dispersion  of  the  first
      sample is less than or equal to the dispersion of the second sample)

The test is based on the following assumptions:
    * the given samples have normal distributions
    * the samples are independent.

Input parameters:
    X   -   sample 1. Array whose index goes from 0 to N-1.
    N   -   sample size.
    Y   -   sample 2. Array whose index goes from 0 to M-1.
    M   -   sample size.

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
     Copyright 19.09.2006 by Bochkanov Sergey
*************************************************************************/
void ftest(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     double* bothtails,
     double* lefttail,
     double* righttail,
     ae_state *_state)
{
    ae_int_t i;
    double xmean;
    double ymean;
    double xvar;
    double yvar;
    ae_int_t df1;
    ae_int_t df2;
    double stat;

    *bothtails = 0.0;
    *lefttail = 0.0;
    *righttail = 0.0;

    if( n<=2||m<=2 )
    {
        *bothtails = 1.0;
        *lefttail = 1.0;
        *righttail = 1.0;
        return;
    }
    
    /*
     * Mean
     */
    xmean = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        xmean = xmean+x->ptr.p_double[i];
    }
    xmean = xmean/(double)n;
    ymean = (double)(0);
    for(i=0; i<=m-1; i++)
    {
        ymean = ymean+y->ptr.p_double[i];
    }
    ymean = ymean/(double)m;
    
    /*
     * Variance (using corrected two-pass algorithm)
     */
    xvar = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        xvar = xvar+ae_sqr(x->ptr.p_double[i]-xmean, _state);
    }
    xvar = xvar/(double)(n-1);
    yvar = (double)(0);
    for(i=0; i<=m-1; i++)
    {
        yvar = yvar+ae_sqr(y->ptr.p_double[i]-ymean, _state);
    }
    yvar = yvar/(double)(m-1);
    if( ae_fp_eq(xvar,(double)(0))||ae_fp_eq(yvar,(double)(0)) )
    {
        *bothtails = 1.0;
        *lefttail = 1.0;
        *righttail = 1.0;
        return;
    }
    
    /*
     * Statistic
     */
    df1 = n-1;
    df2 = m-1;
    stat = ae_minreal(xvar/yvar, yvar/xvar, _state);
    *bothtails = (double)1-(fdistribution(df1, df2, (double)1/stat, _state)-fdistribution(df1, df2, stat, _state));
    *lefttail = fdistribution(df1, df2, xvar/yvar, _state);
    *righttail = (double)1-(*lefttail);
}


/*************************************************************************
One-sample chi-square test

This test checks three hypotheses about the dispersion of the given sample
The following tests are performed:
    * two-tailed test (null hypothesis - the dispersion equals  the  given
      number)
    * left-tailed test (null hypothesis - the dispersion is  greater  than
      or equal to the given number)
    * right-tailed test (null hypothesis  -  dispersion is  less  than  or
      equal to the given number).

Test is based on the following assumptions:
    * the given sample has a normal distribution.

Input parameters:
    X           -   sample 1. Array whose index goes from 0 to N-1.
    N           -   size of the sample.
    Variance    -   dispersion value to compare with.

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
     Copyright 19.09.2006 by Bochkanov Sergey
*************************************************************************/
void onesamplevariancetest(/* Real    */ const ae_vector* x,
     ae_int_t n,
     double variance,
     double* bothtails,
     double* lefttail,
     double* righttail,
     ae_state *_state)
{
    ae_int_t i;
    double xmean;
    double xvar;
    double s;
    double stat;

    *bothtails = 0.0;
    *lefttail = 0.0;
    *righttail = 0.0;

    if( n<=1 )
    {
        *bothtails = 1.0;
        *lefttail = 1.0;
        *righttail = 1.0;
        return;
    }
    
    /*
     * Mean
     */
    xmean = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        xmean = xmean+x->ptr.p_double[i];
    }
    xmean = xmean/(double)n;
    
    /*
     * Variance
     */
    xvar = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        xvar = xvar+ae_sqr(x->ptr.p_double[i]-xmean, _state);
    }
    xvar = xvar/(double)(n-1);
    if( ae_fp_eq(xvar,(double)(0)) )
    {
        *bothtails = 1.0;
        *lefttail = 1.0;
        *righttail = 1.0;
        return;
    }
    
    /*
     * Statistic
     */
    stat = (double)(n-1)*xvar/variance;
    s = chisquaredistribution((double)(n-1), stat, _state);
    *bothtails = (double)2*ae_minreal(s, (double)1-s, _state);
    *lefttail = s;
    *righttail = (double)1-(*lefttail);
}


/*$ End $*/
