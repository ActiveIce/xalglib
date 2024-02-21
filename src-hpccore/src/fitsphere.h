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

#ifndef _fitsphere_h
#define _fitsphere_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "linmin.h"
#include "apserv.h"
#include "ablasf.h"
#include "tsort.h"
#include "optguardapi.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "creflections.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "matgen.h"
#include "scodes.h"
#include "sparse.h"
#include "dlu.h"
#include "sptrf.h"
#include "apstruct.h"
#include "amdordering.h"
#include "spchol.h"
#include "rotations.h"
#include "trfac.h"
#include "hblas.h"
#include "sblas.h"
#include "ortfac.h"
#include "blas.h"
#include "bdsvd.h"
#include "svd.h"
#include "hsschur.h"
#include "basicstatops.h"
#include "evd.h"
#include "trlinsolve.h"
#include "safesolve.h"
#include "rcond.h"
#include "matinv.h"
#include "optserv.h"
#include "fbls.h"
#include "cqmodels.h"
#include "snnls.h"
#include "sactivesets.h"
#include "minbleic.h"
#include "xblas.h"
#include "directdensesolvers.h"
#include "normestimator.h"
#include "linlsqr.h"
#include "minlbfgs.h"
#include "lpqpserv.h"
#include "qqpsolver.h"
#include "qpdenseaulsolver.h"
#include "qpbleicsolver.h"
#include "vipmsolver.h"
#include "ipm2solver.h"
#include "minqp.h"
#include "minlm.h"
#include "lpqppresolve.h"
#include "reviseddualsimplex.h"
#include "nlcslp.h"
#include "nlcsqp.h"
#include "nlcfsqp.h"
#include "nlcaul.h"
#include "minnlc.h"


/*$ Declarations $*/


typedef struct
{
    ae_int_t nfev;
    ae_int_t iterationscount;
} fitsphereinternalreport;


/*$ Body $*/


/*************************************************************************
Fits least squares (LS) circle (or NX-dimensional sphere) to data  (a  set
of points in NX-dimensional space).

Least squares circle minimizes sum of squared deviations between distances
from points to the center and  some  "candidate"  radius,  which  is  also
fitted to the data.

INPUT PARAMETERS:
    XY      -   array[NPoints,NX] (or larger), contains dataset.
                One row = one point in NX-dimensional space.
    NPoints -   dataset size, NPoints>0
    NX      -   space dimensionality, NX>0 (1, 2, 3, 4, 5 and so on)

OUTPUT PARAMETERS:
    CX      -   central point for a sphere
    R       -   radius
                                    
  -- ALGLIB --
     Copyright 07.05.2018 by Bochkanov Sergey
*************************************************************************/
void fitspherels(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nx,
     /* Real    */ ae_vector* cx,
     double* r,
     ae_state *_state);


/*************************************************************************
Fits minimum circumscribed (MC) circle (or NX-dimensional sphere) to  data
(a set of points in NX-dimensional space).

INPUT PARAMETERS:
    XY      -   array[NPoints,NX] (or larger), contains dataset.
                One row = one point in NX-dimensional space.
    NPoints -   dataset size, NPoints>0
    NX      -   space dimensionality, NX>0 (1, 2, 3, 4, 5 and so on)

OUTPUT PARAMETERS:
    CX      -   central point for a sphere
    RHi     -   radius

NOTE: this function is an easy-to-use wrapper around more powerful "expert"
      function fitspherex().
      
      This  wrapper  is optimized  for  ease of use and stability - at the
      cost of somewhat lower  performance  (we  have  to  use  very  tight
      stopping criteria for inner optimizer because we want to  make  sure
      that it will converge on any dataset).
      
      If you are ready to experiment with settings of  "expert"  function,
      you can achieve ~2-4x speedup over standard "bulletproof" settings.

                                    
  -- ALGLIB --
     Copyright 14.04.2017 by Bochkanov Sergey
*************************************************************************/
void fitspheremc(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nx,
     /* Real    */ ae_vector* cx,
     double* rhi,
     ae_state *_state);


/*************************************************************************
Fits maximum inscribed circle (or NX-dimensional sphere) to data (a set of
points in NX-dimensional space).

INPUT PARAMETERS:
    XY      -   array[NPoints,NX] (or larger), contains dataset.
                One row = one point in NX-dimensional space.
    NPoints -   dataset size, NPoints>0
    NX      -   space dimensionality, NX>0 (1, 2, 3, 4, 5 and so on)

OUTPUT PARAMETERS:
    CX      -   central point for a sphere
    RLo     -   radius

NOTE: this function is an easy-to-use wrapper around more powerful "expert"
      function fitspherex().
      
      This  wrapper  is optimized  for  ease of use and stability - at the
      cost of somewhat lower  performance  (we  have  to  use  very  tight
      stopping criteria for inner optimizer because we want to  make  sure
      that it will converge on any dataset).
      
      If you are ready to experiment with settings of  "expert"  function,
      you can achieve ~2-4x speedup over standard "bulletproof" settings.

                                    
  -- ALGLIB --
     Copyright 14.04.2017 by Bochkanov Sergey
*************************************************************************/
void fitspheremi(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nx,
     /* Real    */ ae_vector* cx,
     double* rlo,
     ae_state *_state);


/*************************************************************************
Fits minimum zone circle (or NX-dimensional sphere)  to  data  (a  set  of
points in NX-dimensional space).

INPUT PARAMETERS:
    XY      -   array[NPoints,NX] (or larger), contains dataset.
                One row = one point in NX-dimensional space.
    NPoints -   dataset size, NPoints>0
    NX      -   space dimensionality, NX>0 (1, 2, 3, 4, 5 and so on)

OUTPUT PARAMETERS:
    CX      -   central point for a sphere
    RLo     -   radius of inscribed circle
    RHo     -   radius of circumscribed circle

NOTE: this function is an easy-to-use wrapper around more powerful "expert"
      function fitspherex().
      
      This  wrapper  is optimized  for  ease of use and stability - at the
      cost of somewhat lower  performance  (we  have  to  use  very  tight
      stopping criteria for inner optimizer because we want to  make  sure
      that it will converge on any dataset).
      
      If you are ready to experiment with settings of  "expert"  function,
      you can achieve ~2-4x speedup over standard "bulletproof" settings.

                                    
  -- ALGLIB --
     Copyright 14.04.2017 by Bochkanov Sergey
*************************************************************************/
void fitspheremz(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nx,
     /* Real    */ ae_vector* cx,
     double* rlo,
     double* rhi,
     ae_state *_state);


/*************************************************************************
Fitting minimum circumscribed, maximum inscribed or minimum  zone  circles
(or NX-dimensional spheres)  to  data  (a  set of points in NX-dimensional
space).

This  is  expert  function  which  allows  to  tweak  many  parameters  of
underlying nonlinear solver:
* stopping criteria for inner iterations
* number of outer iterations

You may tweak all these parameters or only some  of  them,  leaving  other
ones at their default state - just specify zero  value,  and  solver  will
fill it with appropriate default one.

These comments also include some discussion of  approach  used  to  handle
such unusual fitting problem,  its  stability,  drawbacks  of  alternative
methods, and convergence properties.
  
INPUT PARAMETERS:
    XY      -   array[NPoints,NX] (or larger), contains dataset.
                One row = one point in NX-dimensional space.
    NPoints -   dataset size, NPoints>0
    NX      -   space dimensionality, NX>0 (1, 2, 3, 4, 5 and so on)
    ProblemType-used to encode problem type:
                * 0 for least squares circle
                * 1 for minimum circumscribed circle/sphere fitting (MC)
                * 2 for  maximum inscribed circle/sphere fitting (MI)
                * 3 for minimum zone circle fitting (difference between
                    Rhi and Rlo is minimized), denoted as MZ
    EpsX    -   stopping condition for NLC optimizer:
                * must be non-negative
                * use 0 to choose default value (1.0E-12 is used by default)
                * you may specify larger values, up to 1.0E-6, if you want
                  to   speed-up   solver;   NLC   solver  performs several
                  preconditioned  outer  iterations,   so   final   result
                  typically has precision much better than EpsX.
    AULIts  -   number of outer iterations performed by NLC optimizer:
                * must be non-negative
                * use 0 to choose default value (20 is used by default)
                * you may specify values smaller than 20 if you want to
                  speed up solver; 10 often results in good combination of
                  precision and speed; sometimes you may get good results
                  with just 6 outer iterations.
                Ignored for ProblemType=0.

OUTPUT PARAMETERS:
    CX      -   central point for a sphere
    RLo     -   radius:
                * for ProblemType=2,3, radius of the inscribed sphere
                * for ProblemType=0 - radius of the least squares sphere
                * for ProblemType=1 - zero
    RHo     -   radius:
                * for ProblemType=1,3, radius of the circumscribed sphere
                * for ProblemType=0 - radius of the least squares sphere
                * for ProblemType=2 - zero

NOTE: ON THE UNIQUENESS OF SOLUTIONS

ALGLIB provides solution to several related circle fitting  problems:   MC
(minimum circumscribed), MI (maximum inscribed)   and   MZ  (minimum zone)
fitting, LS (least squares) fitting.

It  is  important  to  note  that  among these problems only MC and LS are
convex and have unique solution independently from starting point.

As  for MI,  it  may (or  may  not, depending on dataset properties)  have
multiple solutions, and it always  has  one degenerate solution C=infinity
which corresponds to infinitely large radius. Thus, there are no guarantees
that solution to  MI returned by this solver will be the best one (and  no
one can provide you with such guarantee because problem is  NP-hard).  The
only guarantee you have is that this solution is locally optimal, i.e.  it
can not be improved by infinitesimally small tweaks in the parameters.

It  is  also  possible  to "run away" to infinity when  started  from  bad
initial point located outside of point cloud (or when point cloud does not
span entire circumference/surface of the sphere).

Finally,  MZ (minimum zone circle) stands somewhere between MC  and  MI in
stability. It is somewhat regularized by "circumscribed" term of the merit
function; however, solutions to  MZ may be non-unique, and in some unlucky
cases it is also possible to "run away to infinity".


NOTE: ON THE NONLINEARLY CONSTRAINED PROGRAMMING APPROACH

The problem formulation for MC  (minimum circumscribed   circle;  for  the
sake of simplicity we omit MZ and MI here) is:

        [     [         ]2 ]
    min [ max [ XY[i]-C ]  ]
     C  [  i  [         ]  ]

i.e. it is unconstrained nonsmooth optimization problem of finding  "best"
central point, with radius R being unambiguously  determined  from  C.  In
order to move away from non-smoothness we use following reformulation:

        [   ]                  [         ]2
    min [ R ] subject to R>=0, [ XY[i]-C ]  <= R^2
    C,R [   ]                  [         ]
    
i.e. it becomes smooth quadratically constrained optimization problem with
linear target function. Such problem statement is 100% equivalent  to  the
original nonsmooth one, but much easier  to  approach.  We solve  it  with
MinNLC solver provided by ALGLIB.


NOTE: ON INSTABILITY OF SEQUENTIAL LINEARIZATION APPROACH

ALGLIB  has  nonlinearly  constrained  solver which proved to be stable on
such problems. However, some authors proposed to linearize constraints  in
the vicinity of current approximation (Ci,Ri) and to get next  approximate
solution (Ci+1,Ri+1) as solution to linear programming problem. Obviously,
LP problems are easier than nonlinearly constrained ones.

Indeed,  such approach  to   MC/MI/MZ   resulted   in  ~10-20x increase in
performance (when compared with NLC solver). However, it turned  out  that
in some cases linearized model fails to predict correct direction for next
step and tells us that we converged to solution even when we are still 2-4
digits of precision away from it.

It is important that it is not failure of LP solver - it is failure of the
linear model;  even  when  solved  exactly,  it  fails  to  handle  subtle
nonlinearities which arise near the solution. We validated it by comparing
results returned by ALGLIB linear solver with that of MATLAB.

In our experiments with linearization:
* MC failed most often, at both realistic and synthetic datasets
* MI sometimes failed, but sometimes succeeded
* MZ often  succeeded; our guess is that presence of two independent  sets
  of constraints (one set for Rlo and another one for Rhi) and  two  terms
  in the target function (Rlo and Rhi) regularizes task,  so  when  linear
  model fails to handle nonlinearities from Rlo, it uses  Rhi  as  a  hint
  (and vice versa).
  
Because linearization approach failed to achieve stable results, we do not
include it in ALGLIB.

                                    
  -- ALGLIB --
     Copyright 14.04.2017 by Bochkanov Sergey
*************************************************************************/
void fitspherex(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nx,
     ae_int_t problemtype,
     double epsx,
     ae_int_t aulits,
     /* Real    */ ae_vector* cx,
     double* rlo,
     double* rhi,
     ae_state *_state);


/*************************************************************************
Fitting minimum circumscribed, maximum inscribed or minimum  zone  circles
(or NX-dimensional spheres)  to  data  (a  set of points in NX-dimensional
space).

Internal computational function.

INPUT PARAMETERS:
    XY      -   array[NPoints,NX] (or larger), contains dataset.
                One row = one point in NX-dimensional space.
    NPoints -   dataset size, NPoints>0
    NX      -   space dimensionality, NX>0 (1, 2, 3, 4, 5 and so on)
    ProblemType-used to encode problem type:
                * 0 for least squares circle
                * 1 for minimum circumscribed circle/sphere fitting (MC)
                * 2 for  maximum inscribed circle/sphere fitting (MI)
                * 3 for minimum zone circle fitting (difference between
                    Rhi and Rlo is minimized), denoted as MZ
    SolverType- solver to use:
                * 0 use best solver available (1 in current version)
                * 1 use nonlinearly constrained optimization approach, AUL
                    (it is roughly 10-20 times  slower  than  SPC-LIN, but
                    much more stable)
                * 2 use special fast IMPRECISE solver, SPC-LIN  sequential
                    linearization approach; SPC-LIN is fast, but sometimes
                    fails to converge with more than 3 digits of precision;
                    see comments below.
                    NOT RECOMMENDED UNLESS YOU REALLY NEED HIGH PERFORMANCE
                    AT THE COST OF SOME PRECISION.
                * 3 use nonlinearly constrained optimization approach, SLP
                    (most robust one, but somewhat slower than AUL)
                Ignored for ProblemType=0.
    EpsX    -   stopping criteria for SLP and NLC optimizers:
                * must be non-negative
                * use 0 to choose default value (1.0E-12 is used by default)
                * if you use SLP solver, you should use default values
                * if you use NLC solver, you may specify larger values, up
                  to 1.0E-6, if you want to speed-up  solver;  NLC  solver
                  performs several preconditioned outer iterations, so final
                  result typically has precision much better than EpsX.
    AULIts  -   number of iterations performed by NLC optimizer:
                * must be non-negative
                * use 0 to choose default value (20 is used by default)
                * you may specify values smaller than 20 if you want to
                  speed up solver; 10 often results in good combination of
                  precision and speed
                Ignored for ProblemType=0.

OUTPUT PARAMETERS:
    CX      -   central point for a sphere
    RLo     -   radius:
                * for ProblemType=2,3, radius of the inscribed sphere
                * for ProblemType=0 - radius of the least squares sphere
                * for ProblemType=1 - zero
    RHo     -   radius:
                * for ProblemType=1,3, radius of the circumscribed sphere
                * for ProblemType=0 - radius of the least squares sphere
                * for ProblemType=2 - zero
                                    
  -- ALGLIB --
     Copyright 14.04.2017 by Bochkanov Sergey
*************************************************************************/
void fitsphereinternal(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nx,
     ae_int_t problemtype,
     ae_int_t solvertype,
     double epsx,
     ae_int_t aulits,
     /* Real    */ ae_vector* cx,
     double* rlo,
     double* rhi,
     fitsphereinternalreport* rep,
     ae_state *_state);
void _fitsphereinternalreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _fitsphereinternalreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _fitsphereinternalreport_clear(void* _p);
void _fitsphereinternalreport_destroy(void* _p);


/*$ End $*/
#endif

