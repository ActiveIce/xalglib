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
#include "fitsphere.h"


/*$ Declarations $*/


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
     ae_state *_state)
{
    double dummy;

    ae_vector_clear(cx);
    *r = 0.0;

    fitspherex(xy, npoints, nx, 0, 0.0, 0, 0.0, cx, &dummy, r, _state);
}


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
     ae_state *_state)
{
    double dummy;

    ae_vector_clear(cx);
    *rhi = 0.0;

    fitspherex(xy, npoints, nx, 1, 0.0, 0, 0.0, cx, &dummy, rhi, _state);
}


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
     ae_state *_state)
{
    double dummy;

    ae_vector_clear(cx);
    *rlo = 0.0;

    fitspherex(xy, npoints, nx, 2, 0.0, 0, 0.0, cx, rlo, &dummy, _state);
}


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
     ae_state *_state)
{

    ae_vector_clear(cx);
    *rlo = 0.0;
    *rhi = 0.0;

    fitspherex(xy, npoints, nx, 3, 0.0, 0, 0.0, cx, rlo, rhi, _state);
}


/*************************************************************************
Fitting minimum circumscribed, maximum inscribed or minimum  zone  circles
(or NX-dimensional spheres)  to  data  (a  set of points in NX-dimensional
space).

This  is  expert  function  which  allows  to  tweak  many  parameters  of
underlying nonlinear solver:
* stopping criteria for inner iterations
* number of outer iterations
* penalty coefficient used to handle  nonlinear  constraints  (we  convert
  unconstrained nonsmooth optimization problem ivolving max() and/or min()
  operations to quadratically constrained smooth one).

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
    Penalty -   penalty coefficient for NLC optimizer:
                * must be non-negative
                * use 0 to choose default value (1.0E6 in current version)
                * it should be really large, 1.0E6...1.0E7 is a good value
                  to start from;
                * generally, default value is good enough
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
     double penalty,
     /* Real    */ ae_vector* cx,
     double* rlo,
     double* rhi,
     ae_state *_state)
{
    ae_frame _frame_block;
    fitsphereinternalreport rep;

    ae_frame_make(_state, &_frame_block);
    memset(&rep, 0, sizeof(rep));
    ae_vector_clear(cx);
    *rlo = 0.0;
    *rhi = 0.0;
    _fitsphereinternalreport_init(&rep, _state, ae_true);

    ae_assert(ae_isfinite(penalty, _state)&&ae_fp_greater_eq(penalty,(double)(0)), "FitSphereX: Penalty<0 or is not finite", _state);
    ae_assert(ae_isfinite(epsx, _state)&&ae_fp_greater_eq(epsx,(double)(0)), "FitSphereX: EpsX<0 or is not finite", _state);
    ae_assert(aulits>=0, "FitSphereX: AULIts<0", _state);
    fitsphereinternal(xy, npoints, nx, problemtype, 0, epsx, aulits, penalty, cx, rlo, rhi, &rep, _state);
    ae_frame_leave(_state);
}


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
    Penalty -   penalty coefficient for NLC optimizer (ignored  for  SLP):
                * must be non-negative
                * use 0 to choose default value (1.0E6 in current version)
                * it should be really large, 1.0E6...1.0E7 is a good value
                  to start from;
                * generally, default value is good enough
                * ignored by SLP optimizer
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
     double penalty,
     /* Real    */ ae_vector* cx,
     double* rlo,
     double* rhi,
     fitsphereinternalreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    double v;
    double vv;
    ae_int_t cpr;
    ae_bool userlo;
    ae_bool userhi;
    double vlo;
    double vhi;
    ae_vector vmin;
    ae_vector vmax;
    double spread;
    ae_vector pcr;
    ae_vector scr;
    ae_vector bl;
    ae_vector bu;
    ae_int_t suboffset;
    ae_int_t dstrow;
    minnlcstate nlcstate;
    minnlcreport nlcrep;
    ae_matrix cmatrix;
    ae_vector ct;
    ae_int_t outeridx;
    ae_int_t maxouterits;
    ae_int_t maxits;
    double safeguard;
    double bi;
    minbleicstate blcstate;
    minbleicreport blcrep;
    ae_vector prevc;
    minlmstate lmstate;
    minlmreport lmrep;

    ae_frame_make(_state, &_frame_block);
    memset(&vmin, 0, sizeof(vmin));
    memset(&vmax, 0, sizeof(vmax));
    memset(&pcr, 0, sizeof(pcr));
    memset(&scr, 0, sizeof(scr));
    memset(&bl, 0, sizeof(bl));
    memset(&bu, 0, sizeof(bu));
    memset(&nlcstate, 0, sizeof(nlcstate));
    memset(&nlcrep, 0, sizeof(nlcrep));
    memset(&cmatrix, 0, sizeof(cmatrix));
    memset(&ct, 0, sizeof(ct));
    memset(&blcstate, 0, sizeof(blcstate));
    memset(&blcrep, 0, sizeof(blcrep));
    memset(&prevc, 0, sizeof(prevc));
    memset(&lmstate, 0, sizeof(lmstate));
    memset(&lmrep, 0, sizeof(lmrep));
    ae_vector_clear(cx);
    *rlo = 0.0;
    *rhi = 0.0;
    _fitsphereinternalreport_clear(rep);
    ae_vector_init(&vmin, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&vmax, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&pcr, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&scr, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bl, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bu, 0, DT_REAL, _state, ae_true);
    _minnlcstate_init(&nlcstate, _state, ae_true);
    _minnlcreport_init(&nlcrep, _state, ae_true);
    ae_matrix_init(&cmatrix, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ct, 0, DT_INT, _state, ae_true);
    _minbleicstate_init(&blcstate, _state, ae_true);
    _minbleicreport_init(&blcrep, _state, ae_true);
    ae_vector_init(&prevc, 0, DT_REAL, _state, ae_true);
    _minlmstate_init(&lmstate, _state, ae_true);
    _minlmreport_init(&lmrep, _state, ae_true);

    
    /*
     * Check input parameters
     */
    ae_assert(npoints>0, "FitSphereX: NPoints<=0", _state);
    ae_assert(nx>0, "FitSphereX: NX<=0", _state);
    ae_assert(apservisfinitematrix(xy, npoints, nx, _state), "FitSphereX: XY contains infinite or NAN values", _state);
    ae_assert(problemtype>=0&&problemtype<=3, "FitSphereX: ProblemType is neither 0, 1, 2 or 3", _state);
    ae_assert(solvertype>=0&&solvertype<=3, "FitSphereX: ProblemType is neither 1, 2 or 3", _state);
    ae_assert(ae_isfinite(penalty, _state)&&ae_fp_greater_eq(penalty,(double)(0)), "FitSphereX: Penalty<0 or is not finite", _state);
    ae_assert(ae_isfinite(epsx, _state)&&ae_fp_greater_eq(epsx,(double)(0)), "FitSphereX: EpsX<0 or is not finite", _state);
    ae_assert(aulits>=0, "FitSphereX: AULIts<0", _state);
    if( solvertype==0 )
    {
        solvertype = 1;
    }
    if( ae_fp_eq(penalty,(double)(0)) )
    {
        penalty = 1.0E6;
    }
    if( ae_fp_eq(epsx,(double)(0)) )
    {
        epsx = 1.0E-12;
    }
    if( aulits==0 )
    {
        aulits = 20;
    }
    safeguard = (double)(10);
    maxouterits = 10;
    maxits = 10000;
    rep->nfev = 0;
    rep->iterationscount = 0;
    
    /*
     * Determine initial values, initial estimates and spread of the points
     */
    ae_vector_set_length(&vmin, nx, _state);
    ae_vector_set_length(&vmax, nx, _state);
    ae_vector_set_length(cx, nx, _state);
    for(j=0; j<=nx-1; j++)
    {
        vmin.ptr.p_double[j] = xy->ptr.pp_double[0][j];
        vmax.ptr.p_double[j] = xy->ptr.pp_double[0][j];
        cx->ptr.p_double[j] = (double)(0);
    }
    for(i=0; i<=npoints-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            cx->ptr.p_double[j] = cx->ptr.p_double[j]+xy->ptr.pp_double[i][j];
            vmin.ptr.p_double[j] = ae_minreal(vmin.ptr.p_double[j], xy->ptr.pp_double[i][j], _state);
            vmax.ptr.p_double[j] = ae_maxreal(vmax.ptr.p_double[j], xy->ptr.pp_double[i][j], _state);
        }
    }
    spread = (double)(0);
    for(j=0; j<=nx-1; j++)
    {
        cx->ptr.p_double[j] = cx->ptr.p_double[j]/(double)npoints;
        spread = ae_maxreal(spread, vmax.ptr.p_double[j]-vmin.ptr.p_double[j], _state);
    }
    *rlo = ae_maxrealnumber;
    *rhi = (double)(0);
    for(i=0; i<=npoints-1; i++)
    {
        v = (double)(0);
        for(j=0; j<=nx-1; j++)
        {
            v = v+ae_sqr(xy->ptr.pp_double[i][j]-cx->ptr.p_double[j], _state);
        }
        v = ae_sqrt(v, _state);
        *rhi = ae_maxreal(*rhi, v, _state);
        *rlo = ae_minreal(*rlo, v, _state);
    }
    
    /*
     * Handle degenerate case of zero spread
     */
    if( ae_fp_eq(spread,(double)(0)) )
    {
        for(j=0; j<=nx-1; j++)
        {
            cx->ptr.p_double[j] = vmin.ptr.p_double[j];
        }
        *rhi = (double)(0);
        *rlo = (double)(0);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Prepare initial point for optimizer, scale vector and box constraints
     */
    ae_vector_set_length(&pcr, nx+2, _state);
    ae_vector_set_length(&scr, nx+2, _state);
    ae_vector_set_length(&bl, nx+2, _state);
    ae_vector_set_length(&bu, nx+2, _state);
    for(j=0; j<=nx-1; j++)
    {
        pcr.ptr.p_double[j] = cx->ptr.p_double[j];
        scr.ptr.p_double[j] = 0.1*spread;
        bl.ptr.p_double[j] = cx->ptr.p_double[j]-safeguard*spread;
        bu.ptr.p_double[j] = cx->ptr.p_double[j]+safeguard*spread;
    }
    pcr.ptr.p_double[nx+0] = *rlo;
    pcr.ptr.p_double[nx+1] = *rhi;
    scr.ptr.p_double[nx+0] = 0.5*spread;
    scr.ptr.p_double[nx+1] = 0.5*spread;
    bl.ptr.p_double[nx+0] = (double)(0);
    bl.ptr.p_double[nx+1] = (double)(0);
    bu.ptr.p_double[nx+0] = safeguard*(*rhi);
    bu.ptr.p_double[nx+1] = safeguard*(*rhi);
    
    /*
     * First branch: least squares fitting vs MI/MC/MZ fitting
     */
    if( problemtype==0 )
    {
        
        /*
         * Solve problem with Levenberg-Marquardt algorithm
         */
        pcr.ptr.p_double[nx] = *rhi;
        minlmcreatevj(nx+1, npoints, &pcr, &lmstate, _state);
        minlmsetscale(&lmstate, &scr, _state);
        minlmsetbc(&lmstate, &bl, &bu, _state);
        minlmsetcond(&lmstate, epsx, maxits, _state);
        while(minlmiteration(&lmstate, _state))
        {
            if( lmstate.needfij||lmstate.needfi )
            {
                inc(&rep->nfev, _state);
                for(i=0; i<=npoints-1; i++)
                {
                    v = (double)(0);
                    for(j=0; j<=nx-1; j++)
                    {
                        v = v+ae_sqr(lmstate.x.ptr.p_double[j]-xy->ptr.pp_double[i][j], _state);
                    }
                    lmstate.fi.ptr.p_double[i] = ae_sqrt(v, _state)-lmstate.x.ptr.p_double[nx];
                    if( lmstate.needfij )
                    {
                        for(j=0; j<=nx-1; j++)
                        {
                            lmstate.j.ptr.pp_double[i][j] = 0.5/(1.0E-9*spread+ae_sqrt(v, _state))*(double)2*(lmstate.x.ptr.p_double[j]-xy->ptr.pp_double[i][j]);
                        }
                        lmstate.j.ptr.pp_double[i][nx] = (double)(-1);
                    }
                }
                continue;
            }
            ae_assert(ae_false, "Assertion failed", _state);
        }
        minlmresults(&lmstate, &pcr, &lmrep, _state);
        ae_assert(lmrep.terminationtype>0, "FitSphereX: unexpected failure of LM solver", _state);
        rep->iterationscount = rep->iterationscount+lmrep.iterationscount;
        
        /*
         * Offload center coordinates from PCR to CX,
         * re-calculate exact value of RLo/RHi using CX.
         */
        for(j=0; j<=nx-1; j++)
        {
            cx->ptr.p_double[j] = pcr.ptr.p_double[j];
        }
        vv = (double)(0);
        for(i=0; i<=npoints-1; i++)
        {
            v = (double)(0);
            for(j=0; j<=nx-1; j++)
            {
                v = v+ae_sqr(xy->ptr.pp_double[i][j]-cx->ptr.p_double[j], _state);
            }
            v = ae_sqrt(v, _state);
            vv = vv+v/(double)npoints;
        }
        *rlo = vv;
        *rhi = vv;
    }
    else
    {
        
        /*
         * MI, MC, MZ fitting.
         * Prepare problem metrics
         */
        userlo = problemtype==2||problemtype==3;
        userhi = problemtype==1||problemtype==3;
        if( userlo&&userhi )
        {
            cpr = 2;
        }
        else
        {
            cpr = 1;
        }
        if( userlo )
        {
            vlo = (double)(1);
        }
        else
        {
            vlo = (double)(0);
        }
        if( userhi )
        {
            vhi = (double)(1);
        }
        else
        {
            vhi = (double)(0);
        }
        
        /*
         * Solve with NLC solver; problem is treated as general nonlinearly constrained
         * programming, with augmented Lagrangian solver or SLP being used.
         */
        if( solvertype==1||solvertype==3 )
        {
            minnlccreate(nx+2, &pcr, &nlcstate, _state);
            minnlcsetscale(&nlcstate, &scr, _state);
            minnlcsetbc(&nlcstate, &bl, &bu, _state);
            minnlcsetnlc(&nlcstate, 0, cpr*npoints, _state);
            minnlcsetcond(&nlcstate, epsx, maxits, _state);
            minnlcsetprecexactrobust(&nlcstate, 5, _state);
            minnlcsetstpmax(&nlcstate, 0.1, _state);
            if( solvertype==1 )
            {
                minnlcsetalgoaul(&nlcstate, penalty, aulits, _state);
            }
            else
            {
                minnlcsetalgoslp(&nlcstate, _state);
            }
            minnlcrestartfrom(&nlcstate, &pcr, _state);
            while(minnlciteration(&nlcstate, _state))
            {
                if( nlcstate.needfij )
                {
                    inc(&rep->nfev, _state);
                    nlcstate.fi.ptr.p_double[0] = vhi*nlcstate.x.ptr.p_double[nx+1]-vlo*nlcstate.x.ptr.p_double[nx+0];
                    for(j=0; j<=nx-1; j++)
                    {
                        nlcstate.j.ptr.pp_double[0][j] = (double)(0);
                    }
                    nlcstate.j.ptr.pp_double[0][nx+0] = -(double)1*vlo;
                    nlcstate.j.ptr.pp_double[0][nx+1] = (double)1*vhi;
                    for(i=0; i<=npoints-1; i++)
                    {
                        suboffset = 0;
                        if( userhi )
                        {
                            dstrow = 1+cpr*i+suboffset;
                            v = (double)(0);
                            for(j=0; j<=nx-1; j++)
                            {
                                vv = nlcstate.x.ptr.p_double[j]-xy->ptr.pp_double[i][j];
                                v = v+vv*vv;
                                nlcstate.j.ptr.pp_double[dstrow][j] = (double)2*vv;
                            }
                            vv = nlcstate.x.ptr.p_double[nx+1];
                            v = v-vv*vv;
                            nlcstate.j.ptr.pp_double[dstrow][nx+0] = (double)(0);
                            nlcstate.j.ptr.pp_double[dstrow][nx+1] = -(double)2*vv;
                            nlcstate.fi.ptr.p_double[dstrow] = v;
                            inc(&suboffset, _state);
                        }
                        if( userlo )
                        {
                            dstrow = 1+cpr*i+suboffset;
                            v = (double)(0);
                            for(j=0; j<=nx-1; j++)
                            {
                                vv = nlcstate.x.ptr.p_double[j]-xy->ptr.pp_double[i][j];
                                v = v-vv*vv;
                                nlcstate.j.ptr.pp_double[dstrow][j] = -(double)2*vv;
                            }
                            vv = nlcstate.x.ptr.p_double[nx+0];
                            v = v+vv*vv;
                            nlcstate.j.ptr.pp_double[dstrow][nx+0] = (double)2*vv;
                            nlcstate.j.ptr.pp_double[dstrow][nx+1] = (double)(0);
                            nlcstate.fi.ptr.p_double[dstrow] = v;
                            inc(&suboffset, _state);
                        }
                        ae_assert(suboffset==cpr, "Assertion failed", _state);
                    }
                    continue;
                }
                ae_assert(ae_false, "Assertion failed", _state);
            }
            minnlcresults(&nlcstate, &pcr, &nlcrep, _state);
            ae_assert(nlcrep.terminationtype>0, "FitSphereX: unexpected failure of NLC solver", _state);
            rep->iterationscount = rep->iterationscount+nlcrep.iterationscount;
            
            /*
             * Offload center coordinates from PCR to CX,
             * re-calculate exact value of RLo/RHi using CX.
             */
            for(j=0; j<=nx-1; j++)
            {
                cx->ptr.p_double[j] = pcr.ptr.p_double[j];
            }
            *rlo = ae_maxrealnumber;
            *rhi = (double)(0);
            for(i=0; i<=npoints-1; i++)
            {
                v = (double)(0);
                for(j=0; j<=nx-1; j++)
                {
                    v = v+ae_sqr(xy->ptr.pp_double[i][j]-cx->ptr.p_double[j], _state);
                }
                v = ae_sqrt(v, _state);
                *rhi = ae_maxreal(*rhi, v, _state);
                *rlo = ae_minreal(*rlo, v, _state);
            }
            if( !userlo )
            {
                *rlo = (double)(0);
            }
            if( !userhi )
            {
                *rhi = (double)(0);
            }
            ae_frame_leave(_state);
            return;
        }
        
        /*
         * Solve problem with SLP (sequential LP) approach; this approach
         * is much faster than NLP, but often fails for MI and MC (for MZ
         * it performs well enough).
         *
         * REFERENCE: "On a sequential linear programming approach to finding
         *            the smallest circumscribed, largest inscribed, and minimum
         *            zone circle or sphere", Helmuth Spath and G.A.Watson
         */
        if( solvertype==2 )
        {
            ae_matrix_set_length(&cmatrix, cpr*npoints, nx+3, _state);
            ae_vector_set_length(&ct, cpr*npoints, _state);
            ae_vector_set_length(&prevc, nx, _state);
            minbleiccreate(nx+2, &pcr, &blcstate, _state);
            minbleicsetscale(&blcstate, &scr, _state);
            minbleicsetbc(&blcstate, &bl, &bu, _state);
            minbleicsetcond(&blcstate, (double)(0), (double)(0), epsx, maxits, _state);
            for(outeridx=0; outeridx<=maxouterits-1; outeridx++)
            {
                
                /*
                 * Prepare initial point for algorithm; center coordinates at
                 * PCR are used to calculate RLo/RHi and update PCR with them.
                 */
                *rlo = ae_maxrealnumber;
                *rhi = (double)(0);
                for(i=0; i<=npoints-1; i++)
                {
                    v = (double)(0);
                    for(j=0; j<=nx-1; j++)
                    {
                        v = v+ae_sqr(xy->ptr.pp_double[i][j]-pcr.ptr.p_double[j], _state);
                    }
                    v = ae_sqrt(v, _state);
                    *rhi = ae_maxreal(*rhi, v, _state);
                    *rlo = ae_minreal(*rlo, v, _state);
                }
                pcr.ptr.p_double[nx+0] = *rlo*0.99999;
                pcr.ptr.p_double[nx+1] = *rhi/0.99999;
                
                /*
                 * Generate matrix of linear constraints
                 */
                for(i=0; i<=npoints-1; i++)
                {
                    v = (double)(0);
                    for(j=0; j<=nx-1; j++)
                    {
                        v = v+ae_sqr(xy->ptr.pp_double[i][j], _state);
                    }
                    bi = -v/(double)2;
                    suboffset = 0;
                    if( userhi )
                    {
                        dstrow = cpr*i+suboffset;
                        for(j=0; j<=nx-1; j++)
                        {
                            cmatrix.ptr.pp_double[dstrow][j] = pcr.ptr.p_double[j]/(double)2-xy->ptr.pp_double[i][j];
                        }
                        cmatrix.ptr.pp_double[dstrow][nx+0] = (double)(0);
                        cmatrix.ptr.pp_double[dstrow][nx+1] = -*rhi/(double)2;
                        cmatrix.ptr.pp_double[dstrow][nx+2] = bi;
                        ct.ptr.p_int[dstrow] = -1;
                        inc(&suboffset, _state);
                    }
                    if( userlo )
                    {
                        dstrow = cpr*i+suboffset;
                        for(j=0; j<=nx-1; j++)
                        {
                            cmatrix.ptr.pp_double[dstrow][j] = -(pcr.ptr.p_double[j]/(double)2-xy->ptr.pp_double[i][j]);
                        }
                        cmatrix.ptr.pp_double[dstrow][nx+0] = *rlo/(double)2;
                        cmatrix.ptr.pp_double[dstrow][nx+1] = (double)(0);
                        cmatrix.ptr.pp_double[dstrow][nx+2] = -bi;
                        ct.ptr.p_int[dstrow] = -1;
                        inc(&suboffset, _state);
                    }
                    ae_assert(suboffset==cpr, "Assertion failed", _state);
                }
                
                /*
                 * Solve LP subproblem with MinBLEIC
                 */
                for(j=0; j<=nx-1; j++)
                {
                    prevc.ptr.p_double[j] = pcr.ptr.p_double[j];
                }
                minbleicsetlc(&blcstate, &cmatrix, &ct, cpr*npoints, _state);
                minbleicrestartfrom(&blcstate, &pcr, _state);
                while(minbleiciteration(&blcstate, _state))
                {
                    if( blcstate.needfg )
                    {
                        inc(&rep->nfev, _state);
                        blcstate.f = vhi*blcstate.x.ptr.p_double[nx+1]-vlo*blcstate.x.ptr.p_double[nx+0];
                        for(j=0; j<=nx-1; j++)
                        {
                            blcstate.g.ptr.p_double[j] = (double)(0);
                        }
                        blcstate.g.ptr.p_double[nx+0] = -(double)1*vlo;
                        blcstate.g.ptr.p_double[nx+1] = (double)1*vhi;
                        continue;
                    }
                }
                minbleicresults(&blcstate, &pcr, &blcrep, _state);
                ae_assert(blcrep.terminationtype>0, "FitSphereX: unexpected failure of BLEIC solver", _state);
                rep->iterationscount = rep->iterationscount+blcrep.iterationscount;
                
                /*
                 * Terminate iterations early if we converged
                 */
                v = (double)(0);
                for(j=0; j<=nx-1; j++)
                {
                    v = v+ae_sqr(prevc.ptr.p_double[j]-pcr.ptr.p_double[j], _state);
                }
                v = ae_sqrt(v, _state);
                if( ae_fp_less_eq(v,epsx) )
                {
                    break;
                }
            }
            
            /*
             * Offload center coordinates from PCR to CX,
             * re-calculate exact value of RLo/RHi using CX.
             */
            for(j=0; j<=nx-1; j++)
            {
                cx->ptr.p_double[j] = pcr.ptr.p_double[j];
            }
            *rlo = ae_maxrealnumber;
            *rhi = (double)(0);
            for(i=0; i<=npoints-1; i++)
            {
                v = (double)(0);
                for(j=0; j<=nx-1; j++)
                {
                    v = v+ae_sqr(xy->ptr.pp_double[i][j]-cx->ptr.p_double[j], _state);
                }
                v = ae_sqrt(v, _state);
                *rhi = ae_maxreal(*rhi, v, _state);
                *rlo = ae_minreal(*rlo, v, _state);
            }
            if( !userlo )
            {
                *rlo = (double)(0);
            }
            if( !userhi )
            {
                *rhi = (double)(0);
            }
            ae_frame_leave(_state);
            return;
        }
        
        /*
         * Oooops...!
         */
        ae_assert(ae_false, "FitSphereX: integrity check failed", _state);
    }
    ae_frame_leave(_state);
}


void _fitsphereinternalreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    fitsphereinternalreport *p = (fitsphereinternalreport*)_p;
    ae_touch_ptr((void*)p);
}


void _fitsphereinternalreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    fitsphereinternalreport       *dst = (fitsphereinternalreport*)_dst;
    const fitsphereinternalreport *src = (const fitsphereinternalreport*)_src;
    dst->nfev = src->nfev;
    dst->iterationscount = src->iterationscount;
}


void _fitsphereinternalreport_clear(void* _p)
{
    fitsphereinternalreport *p = (fitsphereinternalreport*)_p;
    ae_touch_ptr((void*)p);
}


void _fitsphereinternalreport_destroy(void* _p)
{
    fitsphereinternalreport *p = (fitsphereinternalreport*)_p;
    ae_touch_ptr((void*)p);
}


/*$ End $*/
