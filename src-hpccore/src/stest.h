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

#ifndef _stest_h
#define _stest_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "gammafunc.h"
#include "apserv.h"
#include "ablasf.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "normaldistr.h"
#include "ibetaf.h"
#include "nearunityunit.h"
#include "binomialdistr.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Sign test

This test checks three hypotheses about the median of  the  given  sample.
The following tests are performed:
    * two-tailed test (null hypothesis - the median is equal to the  given
      value)
    * left-tailed test (null hypothesis - the median is  greater  than  or
      equal to the given value)
    * right-tailed test (null hypothesis - the  median  is  less  than  or
      equal to the given value)

Requirements:
    * the scale of measurement should be ordinal, interval or ratio  (i.e.
      the test could not be applied to nominal variables).

The test is non-parametric and doesn't require distribution X to be normal

Input parameters:
    X       -   sample. Array whose index goes from 0 to N-1.
    N       -   size of the sample.
    Median  -   assumed median value.

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

While   calculating   p-values   high-precision   binomial    distribution
approximation is used, so significance levels have about 15 exact digits.

  -- ALGLIB --
     Copyright 08.09.2006 by Bochkanov Sergey
*************************************************************************/
void onesamplesigntest(/* Real    */ const ae_vector* x,
     ae_int_t n,
     double median,
     double* bothtails,
     double* lefttail,
     double* righttail,
     ae_state *_state);


/*$ End $*/
#endif

