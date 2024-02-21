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

#ifndef _jarquebera_h
#define _jarquebera_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Jarque-Bera test

This test checks hypotheses about the fact that a  given  sample  X  is  a
sample of normal random variable.

Requirements:
    * the number of elements in the sample is not less than 5.

Input parameters:
    X   -   sample. Array whose index goes from 0 to N-1.
    N   -   size of the sample. N>=5

Output parameters:
    P           -   p-value for the test

Accuracy of the approximation used (5<=N<=1951):

p-value  	    relative error (5<=N<=1951)
[1, 0.1]            < 1%
[0.1, 0.01]         < 2%
[0.01, 0.001]       < 6%
[0.001, 0]          wasn't measured

For N>1951 accuracy wasn't measured but it shouldn't be sharply  different
from table values.

  -- ALGLIB --
     Copyright 09.04.2007 by Bochkanov Sergey
*************************************************************************/
void jarqueberatest(/* Real    */ const ae_vector* x,
     ae_int_t n,
     double* p,
     ae_state *_state);


/*$ End $*/
#endif

