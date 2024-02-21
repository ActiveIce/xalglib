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

#ifndef _jacobianelliptic_h
#define _jacobianelliptic_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Jacobian Elliptic Functions

Evaluates the Jacobian elliptic functions sn(u|m), cn(u|m),
and dn(u|m) of parameter m between 0 and 1, and real
argument u.

These functions are periodic, with quarter-period on the
real axis equal to the complete elliptic integral
ellpk(1.0-m).

Relation to incomplete elliptic integral:
If u = ellik(phi,m), then sn(u|m) = sin(phi),
and cn(u|m) = cos(phi).  Phi is called the amplitude of u.

Computation is by means of the arithmetic-geometric mean
algorithm, except when m is within 1e-9 of 0 or 1.  In the
latter case with m close to 1, the approximation applies
only for phi < pi/2.

ACCURACY:

Tested at random points with u between 0 and 10, m between
0 and 1.

           Absolute error (* = relative error):
arithmetic   function   # trials      peak         rms
   IEEE      phi         10000       9.2e-16*    1.4e-16*
   IEEE      sn          50000       4.1e-15     4.6e-16
   IEEE      cn          40000       3.6e-15     4.4e-16
   IEEE      dn          10000       1.3e-12     1.8e-14

 Peak error observed in consistency check using addition
theorem for sn(u+v) was 4e-16 (absolute).  Also tested by
the above relation to the incomplete elliptic integral.
Accuracy deteriorates when u is large.

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 2000 by Stephen L. Moshier
*************************************************************************/
void jacobianellipticfunctions(double u,
     double m,
     double* sn,
     double* cn,
     double* dn,
     double* ph,
     ae_state *_state);


/*$ End $*/
#endif

