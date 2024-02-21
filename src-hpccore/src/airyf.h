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

#ifndef _airyf_h
#define _airyf_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Airy function

Solution of the differential equation

y"(x) = xy.

The function returns the two independent solutions Ai, Bi
and their first derivatives Ai'(x), Bi'(x).

Evaluation is by power series summation for small x,
by rational minimax approximations for large x.



ACCURACY:
Error criterion is absolute when function <= 1, relative
when function > 1, except * denotes relative error criterion.
For large negative x, the absolute error increases as x^1.5.
For large positive x, the relative error increases as x^1.5.

Arithmetic  domain   function  # trials      peak         rms
IEEE        -10, 0     Ai        10000       1.6e-15     2.7e-16
IEEE          0, 10    Ai        10000       2.3e-14*    1.8e-15*
IEEE        -10, 0     Ai'       10000       4.6e-15     7.6e-16
IEEE          0, 10    Ai'       10000       1.8e-14*    1.5e-15*
IEEE        -10, 10    Bi        30000       4.2e-15     5.3e-16
IEEE        -10, 10    Bi'       30000       4.9e-15     7.3e-16

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1989, 2000 by Stephen L. Moshier
*************************************************************************/
void airy(double x,
     double* ai,
     double* aip,
     double* bi,
     double* bip,
     ae_state *_state);


/*$ End $*/
#endif

