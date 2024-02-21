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

#ifndef _spline2d_h
#define _spline2d_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "scodes.h"
#include "ablasf.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "tsort.h"
#include "sparse.h"
#include "dlu.h"
#include "sptrf.h"
#include "apstruct.h"
#include "amdordering.h"
#include "spchol.h"
#include "creflections.h"
#include "matgen.h"
#include "rotations.h"
#include "trfac.h"
#include "intfitserv.h"
#include "normestimator.h"
#include "hblas.h"
#include "sblas.h"
#include "ortfac.h"
#include "blas.h"
#include "bdsvd.h"
#include "svd.h"
#include "linlsqr.h"
#include "fbls.h"
#include "spline1d.h"


/*$ Declarations $*/


/*************************************************************************
2-dimensional spline inteprolant
*************************************************************************/
typedef struct
{
    ae_int_t stype;
    ae_bool hasmissingcells;
    ae_int_t n;
    ae_int_t m;
    ae_int_t d;
    ae_vector x;
    ae_vector y;
    ae_vector f;
    ae_vector ismissingnode;
    ae_vector ismissingcell;
} spline2dinterpolant;


/*************************************************************************
Nonlinear least squares solver used to fit 2D splines to data
*************************************************************************/
typedef struct
{
    ae_int_t priorterm;
    double priortermval;
    ae_int_t areatype;
    double xa;
    double xb;
    double ya;
    double yb;
    ae_int_t gridtype;
    ae_int_t kx;
    ae_int_t ky;
    double smoothing;
    ae_int_t nlayers;
    ae_int_t solvertype;
    double lambdabase;
    ae_vector xy;
    ae_int_t npoints;
    ae_int_t d;
    double sx;
    double sy;
    ae_bool adddegreeoffreedom;
    ae_int_t interfacesize;
    ae_int_t lsqrcnt;
    ae_int_t maxcoresize;
} spline2dbuilder;


/*************************************************************************
Spline 2D fitting report:
    rmserror        RMS error
    avgerror        average error
    maxerror        maximum error
    r2              coefficient of determination,  R-squared, 1-RSS/TSS
*************************************************************************/
typedef struct
{
    double rmserror;
    double avgerror;
    double maxerror;
    double r2;
} spline2dfitreport;


/*************************************************************************
Design matrix stored in batch/block sparse format.

The idea is that design matrix for bicubic spline fitting has very regular
structure:

1. I-th row has non-zero entries in elements with indexes starting
from some IDX, and including: IDX, IDX+1, IDX+2, IDX+3, IDX+KX+0, IDX+KX+1,
and so on, up to 16 elements in total.

Rows corresponding to dataset points have 16 non-zero elements, rows
corresponding to nonlinearity penalty have 9 non-zero elements, and rows
of regularizer have 1 element.

For the sake of simplicity, we can use 16 elements for dataset rows and
penalty rows, and process regularizer explicitly.

2. points located in the same cell of the grid have same pattern of non-zeros,
so we can use dense Level 2 and Level 3 linear algebra to work with such
matrices.
*************************************************************************/
typedef struct
{
    ae_int_t blockwidth;
    ae_int_t kx;
    ae_int_t ky;
    ae_int_t npoints;
    ae_int_t nrows;
    ae_int_t ndenserows;
    ae_int_t ndensebatches;
    ae_int_t d;
    ae_int_t maxbatch;
    ae_matrix vals;
    ae_vector batches;
    ae_vector batchbases;
    double lambdareg;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_matrix tmp2;
} spline2dxdesignmatrix;


/*************************************************************************
Temporaries for BlockLLS solver
*************************************************************************/
typedef struct
{
    linlsqrstate solver;
    linlsqrreport solverrep;
    ae_matrix blockata;
    ae_matrix trsmbuf2;
    ae_matrix cholbuf2;
    ae_vector cholbuf1;
    ae_vector tmp0;
    ae_vector tmp1;
} spline2dblockllsbuf;


/*************************************************************************
Temporaries for FastDDM solver
*************************************************************************/
typedef struct
{
    spline2dxdesignmatrix xdesignmatrix;
    ae_vector tmp0;
    ae_vector tmpz;
    spline2dfitreport dummyrep;
    spline2dinterpolant localmodel;
    spline2dblockllsbuf blockllsbuf;
} spline2dfastddmbuf;


/*$ Body $*/


/*************************************************************************
This subroutine calculates the value of the bilinear or bicubic spline  at
the given point X.

Input parameters:
    C   -   2D spline object.
            Built by spline2dbuildbilinearv or spline2dbuildbicubicv.
    X, Y-   point

Result:
    S(x,y)

  -- ALGLIB PROJECT --
     Copyright 05.07.2007 by Bochkanov Sergey
*************************************************************************/
double spline2dcalc(const spline2dinterpolant* c,
     double x,
     double y,
     ae_state *_state);


/*************************************************************************
This subroutine calculates the value of a bilinear or bicubic spline   and
its derivatives.

Use Spline2DDiff2() if you need second derivatives Sxx and Syy.

Input parameters:
    C   -   spline interpolant.
    X, Y-   point

Output parameters:
    F   -   S(x,y)
    FX  -   dS(x,y)/dX
    FY  -   dS(x,y)/dY

  -- ALGLIB PROJECT --
     Copyright 05.07.2007 by Bochkanov Sergey
*************************************************************************/
void spline2ddiff(const spline2dinterpolant* c,
     double x,
     double y,
     double* f,
     double* fx,
     double* fy,
     ae_state *_state);


/*************************************************************************
This subroutine calculates the value of a bilinear or bicubic spline   and
its second derivatives.

Input parameters:
    C   -   spline interpolant.
    X, Y-   point

Output parameters:
    F   -   S(x,y)
    FX  -   dS(x,y)/dX
    FY  -   dS(x,y)/dY
    FXX -   d2S(x,y)/dXdX
    FXY -   d2S(x,y)/dXdY
    FYY -   d2S(x,y)/dYdY

  -- ALGLIB PROJECT --
     Copyright 17.04.2023 by Bochkanov Sergey.
     
     The second derivatives code was contributed by  Horst  Greiner  under
     public domain terms.
*************************************************************************/
void spline2ddiff2(const spline2dinterpolant* c,
     double x,
     double y,
     double* f,
     double* fx,
     double* fy,
     double* fxx,
     double* fxy,
     double* fyy,
     ae_state *_state);


/*************************************************************************
This subroutine calculates bilinear or bicubic vector-valued spline at the
given point (X,Y).

If you need just some specific component of vector-valued spline, you  can
use spline2dcalcvi() function.

INPUT PARAMETERS:
    C   -   spline interpolant.
    X, Y-   point
    F   -   output buffer, possibly preallocated array. In case array size
            is large enough to store result, it is not reallocated.  Array
            which is too short will be reallocated

OUTPUT PARAMETERS:
    F   -   array[D] (or larger) which stores function values

  -- ALGLIB PROJECT --
     Copyright 01.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dcalcvbuf(const spline2dinterpolant* c,
     double x,
     double y,
     /* Real    */ ae_vector* f,
     ae_state *_state);


/*************************************************************************
This subroutine calculates specific component of vector-valued bilinear or
bicubic spline at the given point (X,Y).

INPUT PARAMETERS:
    C   -   spline interpolant.
    X, Y-   point
    I   -   component index, in [0,D). An exception is generated for out
            of range values.

RESULT:
    value of I-th component

  -- ALGLIB PROJECT --
     Copyright 01.02.2018 by Bochkanov Sergey
*************************************************************************/
double spline2dcalcvi(const spline2dinterpolant* c,
     double x,
     double y,
     ae_int_t i,
     ae_state *_state);


/*************************************************************************
This subroutine calculates bilinear or bicubic vector-valued spline at the
given point (X,Y).

INPUT PARAMETERS:
    C   -   spline interpolant.
    X, Y-   point

OUTPUT PARAMETERS:
    F   -   array[D] which stores function values.  F is out-parameter and
            it  is  reallocated  after  call to this function. In case you
            want  to    reuse  previously  allocated  F,   you   may   use
            Spline2DCalcVBuf(),  which  reallocates  F only when it is too
            small.

  -- ALGLIB PROJECT --
     Copyright 16.04.2012 by Bochkanov Sergey
*************************************************************************/
void spline2dcalcv(const spline2dinterpolant* c,
     double x,
     double y,
     /* Real    */ ae_vector* f,
     ae_state *_state);


/*************************************************************************
This subroutine calculates the value and the derivatives of I-th component
of a  vector-valued bilinear or bicubic spline.

Input parameters:
    C   -   spline interpolant.
    X, Y-   point
    I   -   component index, in [0,D)

Output parameters:
    F   -   S(x,y)
    FX  -   dS(x,y)/dX
    FY  -   dS(x,y)/dY

  -- ALGLIB PROJECT --
     Copyright 05.07.2007 by Bochkanov Sergey
*************************************************************************/
void spline2ddiffvi(const spline2dinterpolant* c,
     double x,
     double y,
     ae_int_t i,
     double* f,
     double* fx,
     double* fy,
     ae_state *_state);


/*************************************************************************
This subroutine calculates the value and the derivatives of I-th component
of a vector-valued bilinear or bicubic spline.

Input parameters:
    C   -   spline interpolant.
    X, Y-   point
    I   -   component index, in [0,D)

Output parameters:
    F   -   S(x,y)
    FX  -   dS(x,y)/dX
    FY  -   dS(x,y)/dY
    FXX -   d2S(x,y)/dXdX
    FXY -   d2S(x,y)/dXdY
    FYY -   d2S(x,y)/dYdY

  -- ALGLIB PROJECT --
     Copyright 17.04.2023 by Bochkanov Sergey.
     
     The second derivatives code was contributed by  Horst  Greiner  under
     public domain terms.
*************************************************************************/
void spline2ddiff2vi(const spline2dinterpolant* c,
     double x,
     double y,
     ae_int_t i,
     double* f,
     double* fx,
     double* fy,
     double* fxx,
     double* fxy,
     double* fyy,
     ae_state *_state);


/*************************************************************************
This subroutine performs linear transformation of the spline argument.

Input parameters:
    C       -   spline interpolant
    AX, BX  -   transformation coefficients: x = A*t + B
    AY, BY  -   transformation coefficients: y = A*u + B
Result:
    C   -   transformed spline

  -- ALGLIB PROJECT --
     Copyright 30.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline2dlintransxy(spline2dinterpolant* c,
     double ax,
     double bx,
     double ay,
     double by,
     ae_state *_state);


/*************************************************************************
This subroutine performs linear transformation of the spline.

Input parameters:
    C   -   spline interpolant.
    A, B-   transformation coefficients: S2(x,y) = A*S(x,y) + B
    
Output parameters:
    C   -   transformed spline

  -- ALGLIB PROJECT --
     Copyright 30.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline2dlintransf(spline2dinterpolant* c,
     double a,
     double b,
     ae_state *_state);


/*************************************************************************
This subroutine makes the copy of the spline model.

Input parameters:
    C   -   spline interpolant

Output parameters:
    CC  -   spline copy

  -- ALGLIB PROJECT --
     Copyright 29.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline2dcopy(const spline2dinterpolant* c,
     spline2dinterpolant* cc,
     ae_state *_state);


/*************************************************************************
Bicubic spline resampling

Input parameters:
    A           -   function values at the old grid,
                    array[0..OldHeight-1, 0..OldWidth-1]
    OldHeight   -   old grid height, OldHeight>1
    OldWidth    -   old grid width, OldWidth>1
    NewHeight   -   new grid height, NewHeight>1
    NewWidth    -   new grid width, NewWidth>1
    
Output parameters:
    B           -   function values at the new grid,
                    array[0..NewHeight-1, 0..NewWidth-1]

  -- ALGLIB routine --
     15 May, 2007
     Copyright by Bochkanov Sergey
*************************************************************************/
void spline2dresamplebicubic(/* Real    */ const ae_matrix* a,
     ae_int_t oldheight,
     ae_int_t oldwidth,
     /* Real    */ ae_matrix* b,
     ae_int_t newheight,
     ae_int_t newwidth,
     ae_state *_state);


/*************************************************************************
Bilinear spline resampling

Input parameters:
    A           -   function values at the old grid,
                    array[0..OldHeight-1, 0..OldWidth-1]
    OldHeight   -   old grid height, OldHeight>1
    OldWidth    -   old grid width, OldWidth>1
    NewHeight   -   new grid height, NewHeight>1
    NewWidth    -   new grid width, NewWidth>1

Output parameters:
    B           -   function values at the new grid,
                    array[0..NewHeight-1, 0..NewWidth-1]

  -- ALGLIB routine --
     09.07.2007
     Copyright by Bochkanov Sergey
*************************************************************************/
void spline2dresamplebilinear(/* Real    */ const ae_matrix* a,
     ae_int_t oldheight,
     ae_int_t oldwidth,
     /* Real    */ ae_matrix* b,
     ae_int_t newheight,
     ae_int_t newwidth,
     ae_state *_state);


/*************************************************************************
This subroutine builds bilinear vector-valued spline.

This function produces C0-continuous spline, i.e.  the  spline  itself  is
continuous, however its first and second  derivatives have discontinuities
at the spline cell boundaries.

Input parameters:
    X   -   spline abscissas, array[0..N-1]
    Y   -   spline ordinates, array[0..M-1]
    F   -   function values, array[0..M*N*D-1]:
            * first D elements store D values at (X[0],Y[0])
            * next D elements store D values at (X[1],Y[0])
            * general form - D function values at (X[i],Y[j]) are stored
              at F[D*(J*N+I)...D*(J*N+I)+D-1].
    M,N -   grid size, M>=2, N>=2
    D   -   vector dimension, D>=1

Output parameters:
    C   -   spline interpolant

  -- ALGLIB PROJECT --
     Copyright 16.04.2012 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildbilinearv(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     /* Real    */ const ae_vector* f,
     ae_int_t d,
     spline2dinterpolant* c,
     ae_state *_state);


/*************************************************************************
This subroutine builds bilinear vector-valued spline.

Buffered version of Spline2DBuildBilinearV() which reuses memory previously
allocated in C as much as possible.

  -- ALGLIB PROJECT --
     Copyright 16.04.2012 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildbilinearvbuf(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     /* Real    */ const ae_vector* f,
     ae_int_t d,
     spline2dinterpolant* c,
     ae_state *_state);


/*************************************************************************
This subroutine builds bilinear vector-valued  spline,  with  some  spline
cells being missing due to missing nodes.

This function produces C0-continuous spline, i.e.  the  spline  itself  is
continuous, however its first and second  derivatives have discontinuities
at the spline cell boundaries.

When the node (i,j) is missing, it means that: a) we don't  have  function
value at this point (elements of F[] are ignored), and  b)  we  don't need
spline value at cells adjacent to the node (i,j), i.e. up to 4 spline cells
will be dropped. An attempt to compute spline value at  the  missing  cell
will return NAN.

It is important to  understand  that  this  subroutine  does  NOT  support
interpolation on scattered grids. It allows us to drop some nodes, but  at
the cost of making a "hole in the spline" around this point. If  you  want
function  that   can   "fill  the  gap",  use  RBF  or  another  scattered
interpolation method.

The  intended  usage  for  this  subroutine  are  regularly  sampled,  but
non-rectangular datasets.

Input parameters:
    X   -   spline abscissas, array[0..N-1]
    Y   -   spline ordinates, array[0..M-1]
    F   -   function values, array[0..M*N*D-1]:
            * first D elements store D values at (X[0],Y[0])
            * next D elements store D values at (X[1],Y[0])
            * general form - D function values at (X[i],Y[j]) are stored
              at F[D*(J*N+I)...D*(J*N+I)+D-1].
            * missing values are ignored
    Missing array[M*N], Missing[J*N+I]=True means that corresponding entries
            of F[] are missing nodes.
    M,N -   grid size, M>=2, N>=2
    D   -   vector dimension, D>=1

Output parameters:
    C   -   spline interpolant

  -- ALGLIB PROJECT --
     Copyright 27.06.2022 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildbilinearmissing(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     /* Real    */ const ae_vector* _f,
     /* Boolean */ const ae_vector* missing,
     ae_int_t d,
     spline2dinterpolant* c,
     ae_state *_state);


/*************************************************************************
This subroutine builds bilinear vector-valued  spline,  with  some  spline
cells being missing due to missing nodes.

Buffered version of  Spline2DBuildBilinearMissing()  which  reuses  memory
previously allocated in C as much as possible.

  -- ALGLIB PROJECT --
     Copyright 27.06.2022 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildbilinearmissingbuf(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     /* Real    */ const ae_vector* _f,
     /* Boolean */ const ae_vector* missing,
     ae_int_t d,
     spline2dinterpolant* c,
     ae_state *_state);


/*************************************************************************
This subroutine builds a bicubic vector-valued spline using  parabolically
terminated end conditions.

This function produces a C2-continuous spline, i.e. the  spline has smooth
first and second  derivatives  both  inside  spline  cells  and  at  their
boundaries.

INPUT PARAMETERS:
    X   -   spline abscissas, array[N]
    N   -   N>=2:
            * if not given, automatically determined as len(X)
            * if given, only leading N elements of X are used
    Y   -   spline ordinates, array[M]
    M   -   M>=2:
            * if not given, automatically determined as len(Y)
            * if given, only leading M elements of Y are used
    F   -   function values, array[M*N*D]:
            * first D elements store D values at (X[0],Y[0])
            * next D elements store D values at (X[1],Y[0])
            * general form - D function values at (X[i],Y[j]) are stored
              at F[D*(J*N+I)...D*(J*N+I)+D-1].
    D   -   vector dimension, D>=1:
            * D=1 means scalar-valued bicubic spline
            * D>1 means vector-valued bicubic spline

OUTPUT PARAMETERS:
    C   -   spline interpolant

  -- ALGLIB PROJECT --
     Copyright 2012-2023 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildbicubicv(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     /* Real    */ const ae_vector* f,
     ae_int_t d,
     spline2dinterpolant* c,
     ae_state *_state);


/*************************************************************************
This subroutine  builds  a  bicubic  vector-valued  spline  using  clamped
boundary conditions:
* spline values at the grid nodes are specified
* boundary conditions for  first,  second  derivatives  or  for  parabolic
  termination at four boundaries (bottom  y=min(Y[]), top y=max(Y[]), left
  x=min(X[]), right x=max(X[])) are specified
* mixed derivatives at corners are specified
* it is possible to  have  different  boundary  conditions  for  different
  boundaries (first derivatives along  one  boundary,  second  derivatives
  along other one, parabolic termination along the rest and so on)
* it is possible to have either a scalar (D=1) or a vector-valued spline

This function produces a C2-continuous spline, i.e. the  spline has smooth
first and second  derivatives  both  inside  spline  cells  and  at  their
boundaries.

INPUT PARAMETERS:
    X           -   spline  abscissas,  array[N].  Can  be  unsorted,  the
                    function will sort it together with boundary conditions
                    and F[] array (the same set of  permutations  will  be
                    applied to X[] and F[]).
    N           -   N>=2:
                    * if not given, automatically determined as len(X)
                    * if given, only leading N elements of X are used
    Y           -   spline ordinates, array[M].  Can   be   unsorted,  the
                    function will sort it together with boundary conditions
                    and F[] array (the same set of  permutations  will  be
                    applied to X[] and F[]).
    M           -   M>=2:
                    * if not given, automatically determined as len(Y)
                    * if given, only leading M elements of Y are used
    BndBtm      -   array[D*N], boundary conditions at the bottom boundary
                    of the interpolation area  (corresponds to y=min(Y[]):
                    * if  BndTypeBtm=0,  the  spline  has   a   'parabolic
                      termination' boundary condition across that specific
                      boundary. In this case BndBtm is not even referenced
                      by the function and can be unallocated.
                    * otherwise contains derivatives with respect to X
                    * if BndTypeBtm=1, first derivatives are given
                    * if BndTypeBtm=2, second derivatives are given
                    * first D entries store derivatives at x=X[0], y=minY,
                      subsequent D entries store  derivatives  at  x=X[1],
                      y=minY and so on
    BndTop      -   array[D*N],  boundary  conditions  at the top boundary
                    of the interpolation area  (corresponds to y=max(Y[]):
                    * if  BndTypeTop=0,  the  spline  has   a   'parabolic
                      termination' boundary condition across that specific
                      boundary. In this case BndTop is not even referenced
                      by the function and can be unallocated.
                    * otherwise contains derivatives with respect to X
                    * if BndTypeTop=1, first derivatives are given
                    * if BndTypeTop=2, second derivatives are given
                    * first D entries store derivatives at x=X[0], y=maxY,
                      subsequent D entries store  derivatives  at  x=X[1],
                      y=maxY and so on
    BndLft      -   array[D*M], boundary conditions at  the  left boundary
                    of the  interpolation area (corresponds to x=min(X[]):
                    * if  BndTypeLft=0,  the  spline  has   a   'parabolic
                      termination' boundary condition across that specific
                      boundary. In this case BndLft is not even referenced
                      by the function and can be unallocated.
                    * otherwise contains derivatives with respect to Y
                    * if BndTypeLft=1, first derivatives are given
                    * if BndTypeLft=2, second derivatives are given
                    * first D entries store derivatives at x=minX, y=Y[0],
                      subsequent D entries store  derivatives  at  x=minX,
                      y=Y[1] and so on
    BndRgt      -   array[D*M], boundary conditions at  the right boundary
                    of the  interpolation area (corresponds to x=max(X[]):
                    * if  BndTypeRgt=0,  the  spline  has   a   'parabolic
                      termination' boundary condition across that specific
                      boundary. In this case BndRgt is not even referenced
                      by the function and can be unallocated.
                    * otherwise contains derivatives with respect to Y
                    * if BndTypeRgt=1, first derivatives are given
                    * if BndTypeRgt=2, second derivatives are given
                    * first D entries store derivatives at x=maxX, y=Y[0],
                      subsequent D entries store  derivatives  at  x=maxX,
                      y=Y[1] and so on
    MixedD      -   array[D*4], mixed derivatives  at  4  corners  of  the
                    interpolation area:
                    * derivative order depends on the order  of   boundary
                      conditions (bottom/top and left/right)  intersecting
                      at that corner:
                      **  for BndType(Btm|Top)=BndType(Lft|Rgt)=1 user has
                          to provide d2S/dXdY
                      **  for BndType(Btm|Top)=BndType(Lft|Rgt)=2 user has
                          to provide d4S/(dX^2*dY^2)
                      **  for BndType(Btm|Top)=1, BndType(Lft|Rgt)=2  user
                          has to provide d3S/(dX^2*dY)
                      **  for BndType(Btm|Top)=2, BndType(Lft|Rgt)=1  user
                          has to provide d3S/(dX*dY^2)
                      **  if one of the intersecting bounds has 'parabolic
                          termination'  condition,   this  specific  mixed
                          derivative is not used
                    * first D entries store derivatives at the bottom left
                      corner x=min(X[]), y=min(Y[])
                    * subsequent D entries store derivatives at the bottom
                      right corner x=max(X[]), y=min(Y[])
                    * subsequent D entries store derivatives  at  the  top
                      left corner  x=min(X[]), y=max(Y[])
                    * subsequent D entries store derivatives  at  the  top
                      right corner x=max(X[]), y=max(Y[])
                    * if all bounds have 'parabolic termination' condition,
                      MixedD[]  is  not  referenced  at  all  and  can  be
                      unallocated.
    F           -   function values, array[M*N*D]:
                    * first D elements store D values at (X[0],Y[0])
                    * next D elements store D values at (X[1],Y[0])
                    * general form - D function values at (X[i],Y[j])  are
                      stored at F[D*(J*N+I)...D*(J*N+I)+D-1].
    D           -   vector dimension, D>=1:
                    * D=1 means scalar-valued bicubic spline
                    * D>1 means vector-valued bicubic spline

OUTPUT PARAMETERS:
    C   -   spline interpolant

  -- ALGLIB PROJECT --
     Copyright 2012-2023 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildclampedv(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     /* Real    */ const ae_vector* _bndbtm,
     ae_int_t bndtypebtm,
     /* Real    */ const ae_vector* _bndtop,
     ae_int_t bndtypetop,
     /* Real    */ const ae_vector* _bndlft,
     ae_int_t bndtypelft,
     /* Real    */ const ae_vector* _bndrgt,
     ae_int_t bndtypergt,
     /* Real    */ const ae_vector* mixedd,
     /* Real    */ const ae_vector* _f,
     ae_int_t d,
     spline2dinterpolant* c,
     ae_state *_state);


/*************************************************************************
This subroutine builds a Hermite bicubic vector-valued spline.

This function produces merely C1-continuous spline, i.e. the   spline  has
smooth first derivatives.

INPUT PARAMETERS:
    X   -   spline abscissas, array[N]
    N   -   N>=2:
            * if not given, automatically determined as len(X)
            * if given, only leading N elements of X are used
    Y   -   spline ordinates, array[M]
    M   -   M>=2:
            * if not given, automatically determined as len(Y)
            * if given, only leading M elements of Y are used
    F   -   function values, array[M*N*D]:
            * first D elements store D values at (X[0],Y[0])
            * next D elements store D values at (X[1],Y[0])
            * general form - D function values at (X[i],Y[j]) are stored
              at F[D*(J*N+I)...D*(J*N+I)+D-1].
    dFdX-   spline derivatives with respect to X, array[M*N*D]:
            * first D elements store D values at (X[0],Y[0])
            * next D elements store D values at (X[1],Y[0])
            * general form - D function values at (X[i],Y[j]) are stored
              at F[D*(J*N+I)...D*(J*N+I)+D-1].
    dFdY-   spline derivatives with respect to Y, array[M*N*D]:
            * first D elements store D values at (X[0],Y[0])
            * next D elements store D values at (X[1],Y[0])
            * general form - D function values at (X[i],Y[j]) are stored
              at F[D*(J*N+I)...D*(J*N+I)+D-1].
    d2FdXdY-mixed derivatives with respect to X and Y, array[M*N*D]:
            * first D elements store D values at (X[0],Y[0])
            * next D elements store D values at (X[1],Y[0])
            * general form - D function values at (X[i],Y[j]) are stored
              at F[D*(J*N+I)...D*(J*N+I)+D-1].
    D   -   vector dimension, D>=1:
            * D=1 means scalar-valued bicubic spline
            * D>1 means vector-valued bicubic spline

OUTPUT PARAMETERS:
    C   -   spline interpolant

  -- ALGLIB PROJECT --
     Copyright 2012-2023 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildhermitev(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     /* Real    */ const ae_vector* _f,
     /* Real    */ const ae_vector* _dfdx,
     /* Real    */ const ae_vector* _dfdy,
     /* Real    */ const ae_vector* _d2fdxdy,
     ae_int_t d,
     spline2dinterpolant* c,
     ae_state *_state);


/*************************************************************************
This subroutine builds bicubic vector-valued spline.

Buffered version of Spline2DBuildBicubicV() which reuses memory previously
allocated in C as much as possible.

  -- ALGLIB PROJECT --
     Copyright 16.04.2012 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildbicubicvbuf(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     /* Real    */ const ae_vector* _f,
     ae_int_t d,
     spline2dinterpolant* c,
     ae_state *_state);


/*************************************************************************
This  subroutine builds bicubic vector-valued  spline,  with  some  spline
cells being missing due to missing nodes.

This function produces C2-continuous spline, i.e. the has smooth first and
second derivatives both inside spline cells and at the boundaries.

When the node (i,j) is missing, it means that: a) we don't  have  function
value at this point (elements of F[] are ignored), and  b)  we  don't need
spline value at cells adjacent to the node (i,j), i.e. up to 4 spline cells
will be dropped. An attempt to compute spline value at  the  missing  cell
will return NAN.

It is important to  understand  that  this  subroutine  does  NOT  support
interpolation on scattered grids. It allows us to drop some nodes, but  at
the cost of making a "hole in the spline" around this point. If  you  want
function  that   can   "fill  the  gap",  use  RBF  or  another  scattered
interpolation method.

The  intended  usage  for  this  subroutine  are  regularly  sampled,  but
non-rectangular datasets.

Input parameters:
    X   -   spline abscissas, array[0..N-1]
    Y   -   spline ordinates, array[0..M-1]
    F   -   function values, array[0..M*N*D-1]:
            * first D elements store D values at (X[0],Y[0])
            * next D elements store D values at (X[1],Y[0])
            * general form - D function values at (X[i],Y[j]) are stored
              at F[D*(J*N+I)...D*(J*N+I)+D-1].
            * missing values are ignored
    Missing array[M*N], Missing[J*N+I]=True means that corresponding entries
            of F[] are missing nodes.
    M,N -   grid size, M>=2, N>=2
    D   -   vector dimension, D>=1

Output parameters:
    C   -   spline interpolant

  -- ALGLIB PROJECT --
     Copyright 27.06.2022 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildbicubicmissing(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     /* Real    */ const ae_vector* _f,
     /* Boolean */ const ae_vector* missing,
     ae_int_t d,
     spline2dinterpolant* c,
     ae_state *_state);


/*************************************************************************
This  subroutine builds bicubic vector-valued  spline,  with  some  spline
cells being missing due to missing nodes.

Buffered version  of  Spline2DBuildBicubicMissing()  which  reuses  memory
previously allocated in C as much as possible.

  -- ALGLIB PROJECT --
     Copyright 27.06.2022 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildbicubicmissingbuf(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     /* Real    */ const ae_vector* _f,
     /* Boolean */ const ae_vector* missing,
     ae_int_t d,
     spline2dinterpolant* c,
     ae_state *_state);


/*************************************************************************
This subroutine unpacks two-dimensional spline into the coefficients table

Input parameters:
    C   -   spline interpolant.

Result:
    M, N-   grid size (x-axis and y-axis)
    D   -   number of components
    Tbl -   coefficients table, unpacked format,
            D - components: [0..(N-1)*(M-1)*D-1, 0..20].
            For T=0..D-1 (component index), I = 0...N-2 (x index),
            J=0..M-2 (y index):
                K :=  T + I*D + J*D*(N-1)
                
                K-th row stores decomposition for T-th component of the
                vector-valued function
                
                Tbl[K,0] = X[i]
                Tbl[K,1] = X[i+1]
                Tbl[K,2] = Y[j]
                Tbl[K,3] = Y[j+1]
                Tbl[K,4] = C00
                Tbl[K,5] = C01
                Tbl[K,6] = C02
                Tbl[K,7] = C03
                Tbl[K,8] = C10
                Tbl[K,9] = C11
                ...
                Tbl[K,19] = C33
                Tbl[K,20] = 1 if the cell is present, 0 if the cell is missing.
                            In the latter case Tbl[4..19] are exactly zero.
            On each grid square spline is equals to:
                S(x) = SUM(c[i,j]*(t^i)*(u^j), i=0..3, j=0..3)
                t = x-x[j]
                u = y-y[i]

  -- ALGLIB PROJECT --
     Copyright 16.04.2012 by Bochkanov Sergey
*************************************************************************/
void spline2dunpackv(const spline2dinterpolant* c,
     ae_int_t* m,
     ae_int_t* n,
     ae_int_t* d,
     /* Real    */ ae_matrix* tbl,
     ae_state *_state);


/*************************************************************************
This subroutine was deprecated in ALGLIB 3.6.0

We recommend you to switch  to  Spline2DBuildBilinearV(),  which  is  more
flexible and accepts its arguments in more convenient order.

  -- ALGLIB PROJECT --
     Copyright 05.07.2007 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildbilinear(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_matrix* f,
     ae_int_t m,
     ae_int_t n,
     spline2dinterpolant* c,
     ae_state *_state);


/*************************************************************************
This subroutine was deprecated in ALGLIB 3.6.0

We recommend you to switch  to  Spline2DBuildBicubicV(),  which  is  more
flexible and accepts its arguments in more convenient order.

  -- ALGLIB PROJECT --
     Copyright 05.07.2007 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildbicubic(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_matrix* _f,
     ae_int_t m,
     ae_int_t n,
     spline2dinterpolant* c,
     ae_state *_state);


/*************************************************************************
This subroutine was deprecated in ALGLIB 3.6.0

We recommend you to switch  to  Spline2DUnpackV(),  which is more flexible
and accepts its arguments in more convenient order.

  -- ALGLIB PROJECT --
     Copyright 29.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline2dunpack(const spline2dinterpolant* c,
     ae_int_t* m,
     ae_int_t* n,
     /* Real    */ ae_matrix* tbl,
     ae_state *_state);


/*************************************************************************
This subroutine creates least squares solver used to  fit  2D  splines  to
irregularly sampled (scattered) data.

Solver object is used to perform spline fits as follows:
* solver object is created with spline2dbuildercreate() function
* dataset is added with spline2dbuildersetpoints() function
* fit area is chosen:
  * spline2dbuildersetarea()     - for user-defined area
  * spline2dbuildersetareaauto() - for automatically chosen area
* number of grid nodes is chosen with spline2dbuildersetgrid()
* prior term is chosen with one of the following functions:
  * spline2dbuildersetlinterm()   to set linear prior
  * spline2dbuildersetconstterm() to set constant prior
  * spline2dbuildersetzeroterm()  to set zero prior
  * spline2dbuildersetuserterm()  to set user-defined constant prior
* solver algorithm is chosen with either:
  * spline2dbuildersetalgoblocklls() - BlockLLS algorithm, medium-scale problems
  * spline2dbuildersetalgofastddm()  - FastDDM algorithm, large-scale problems
* finally, fitting itself is performed with spline2dfit() function.

Most of the steps above can be omitted,  solver  is  configured with  good
defaults. The minimum is to call:
* spline2dbuildercreate() to create solver object
* spline2dbuildersetpoints() to specify dataset
* spline2dbuildersetgrid() to tell how many nodes you need
* spline2dfit() to perform fit

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

INPUT PARAMETERS:
    D   -   positive number, number of Y-components: D=1 for simple scalar
            fit, D>1 for vector-valued spline fitting.
    
OUTPUT PARAMETERS:
    S   -   solver object

  -- ALGLIB PROJECT --
     Copyright 29.01.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildercreate(ae_int_t d,
     spline2dbuilder* state,
     ae_state *_state);


/*************************************************************************
This function sets constant prior term (model is a sum of  bicubic  spline
and global prior, which can be linear, constant, user-defined  constant or
zero).

Constant prior term is determined by least squares fitting.

INPUT PARAMETERS:
    S       -   spline builder
    V       -   value for user-defined prior

  -- ALGLIB --
     Copyright 01.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetuserterm(spline2dbuilder* state,
     double v,
     ae_state *_state);


/*************************************************************************
This function sets linear prior term (model is a sum of bicubic spline and
global  prior,  which  can  be  linear, constant, user-defined constant or
zero).

Linear prior term is determined by least squares fitting.

INPUT PARAMETERS:
    S       -   spline builder

  -- ALGLIB --
     Copyright 01.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetlinterm(spline2dbuilder* state, ae_state *_state);


/*************************************************************************
This function sets constant prior term (model is a sum of  bicubic  spline
and global prior, which can be linear, constant, user-defined  constant or
zero).

Constant prior term is determined by least squares fitting.

INPUT PARAMETERS:
    S       -   spline builder

  -- ALGLIB --
     Copyright 01.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetconstterm(spline2dbuilder* state, ae_state *_state);


/*************************************************************************
This function sets zero prior term (model is a sum of bicubic  spline  and
global  prior,  which  can  be  linear, constant, user-defined constant or
zero).

INPUT PARAMETERS:
    S       -   spline builder

  -- ALGLIB --
     Copyright 01.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetzeroterm(spline2dbuilder* state, ae_state *_state);


/*************************************************************************
This function adds dataset to the builder object.

This function overrides results of the previous calls, i.e. multiple calls
of this function will result in only the last set being added.

INPUT PARAMETERS:
    S       -   spline 2D builder object
    XY      -   points, array[N,2+D]. One  row  corresponds to  one  point
                in the dataset. First 2  elements  are  coordinates,  next
                D  elements are function values. Array may  be larger than 
                specified, in  this  case  only leading [N,NX+NY] elements 
                will be used.
    N       -   number of points in the dataset

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetpoints(spline2dbuilder* state,
     /* Real    */ const ae_matrix* xy,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
This function sets area where 2D spline interpolant is built. "Auto" means
that area extent is determined automatically from dataset extent.

INPUT PARAMETERS:
    S       -   spline 2D builder object

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetareaauto(spline2dbuilder* state, ae_state *_state);


/*************************************************************************
This  function  sets  area  where  2D  spline  interpolant  is   built  to
user-defined one: [XA,XB]*[YA,YB]

INPUT PARAMETERS:
    S       -   spline 2D builder object
    XA,XB   -   spatial extent in the first (X) dimension, XA<XB
    YA,YB   -   spatial extent in the second (Y) dimension, YA<YB

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetarea(spline2dbuilder* state,
     double xa,
     double xb,
     double ya,
     double yb,
     ae_state *_state);


/*************************************************************************
This  function  sets  nodes  count  for  2D spline interpolant. Fitting is
performed on area defined with one of the "setarea"  functions;  this  one
sets number of nodes placed upon the fitting area.

INPUT PARAMETERS:
    S       -   spline 2D builder object
    KX      -   nodes count for the first (X) dimension; fitting  interval
                [XA,XB] is separated into KX-1 subintervals, with KX nodes
                created at the boundaries.
    KY      -   nodes count for the first (Y) dimension; fitting  interval
                [YA,YB] is separated into KY-1 subintervals, with KY nodes
                created at the boundaries.

NOTE: at  least  4  nodes  is  created in each dimension, so KX and KY are
      silently increased if needed.

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetgrid(spline2dbuilder* state,
     ae_int_t kx,
     ae_int_t ky,
     ae_state *_state);


/*************************************************************************
This  function  allows  you to choose least squares solver used to perform
fitting. This function sets solver algorithm to "FastDDM", which  performs
fast parallel fitting by splitting problem into smaller chunks and merging
results together.

Unlike BlockLLS, this solver produces merely C1 continuous models  because
domain decomposition part disrupts C2 continuity.

This solver is optimized for large-scale problems, starting  from  256x256
grids, and up to 10000x10000 grids. Of course, it will  work  for  smaller
grids too.

More detailed description of the algorithm is given below:
* algorithm generates hierarchy  of  nested  grids,  ranging  from  ~16x16
  (topmost "layer" of the model) to ~KX*KY one (final layer). Upper layers
  model global behavior of the function, lower layers are  used  to  model
  fine details. Moving from layer to layer doubles grid density.
* fitting  is  started  from  topmost  layer, subsequent layers are fitted
  using residuals from previous ones.
* user may choose to skip generation of upper layers and generate  only  a
  few bottom ones, which  will  result  in  much  better  performance  and
  parallelization efficiency, at the cost of algorithm inability to "patch"
  large holes in the dataset.
* every layer is regularized using progressively increasing regularization
  coefficient; thus, increasing  LambdaV  penalizes  fine  details  first,
  leaving lower frequencies almost intact for a while.
* after fitting is done, all layers are merged together into  one  bicubic
  spline
  
IMPORTANT: regularization coefficient used by  this  solver  is  different
           from the one used by  BlockLLS.  Latter  utilizes  nonlinearity
           penalty,  which  is  global  in  nature  (large  regularization
           results in global linear trend being  extracted);  this  solver
           uses another, localized form of penalty, which is suitable  for
           parallel processing.

Notes on memory and performance:
* memory requirements: most memory is consumed  during  modeling   of  the
  higher layers; ~[512*NPoints] bytes is required for a  model  with  full
  hierarchy of grids being generated. However, if you skip a  few  topmost
  layers, you will get nearly constant (wrt. points count and  grid  size)
  memory consumption.
* serial running time: O(K*K)+O(NPoints) for a KxK grid
* parallelism potential: good. You may get  nearly  linear  speed-up  when
  performing fitting with just a few layers. Adding more layers results in
  model becoming more global, which somewhat  reduces  efficiency  of  the
  parallel code.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

INPUT PARAMETERS:
    S       -   spline 2D builder object
    NLayers -   number of layers in the model:
                * NLayers>=1 means that up  to  chosen  number  of  bottom
                  layers is fitted
                * NLayers=0 means that maximum number of layers is  chosen
                  (according to current grid size)
                * NLayers<=-1 means that up to |NLayers| topmost layers is
                  skipped
                Recommendations:
                * good "default" value is 2 layers
                * you may need  more  layers,  if  your  dataset  is  very
                  irregular and you want to "patch"  large  holes.  For  a
                  grid step H (equal to AreaWidth/GridSize) you may expect
                  that last layer reproduces variations at distance H (and
                  can patch holes that wide); that higher  layers  operate
                  at distances 2*H, 4*H, 8*H and so on.
                * good value for "bullletproof" mode is  NLayers=0,  which
                  results in complete hierarchy of layers being generated.
    LambdaV -   regularization coefficient, chosen in such a way  that  it
                penalizes bottom layers (fine details) first.
                LambdaV>=0, zero value means that no penalty is applied.

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetalgofastddm(spline2dbuilder* state,
     ae_int_t nlayers,
     double lambdav,
     ae_state *_state);


/*************************************************************************
This  function  allows  you to choose least squares solver used to perform
fitting. This function sets solver algorithm to "BlockLLS", which performs
least squares fitting  with  fast  sparse  direct  solver,  with  optional
nonsmoothness penalty being applied.

This solver produces C2-continuous spline.

Nonlinearity penalty has the following form:

                          [                                            ]
    P() ~ Lambda* integral[ (d2S/dx2)^2 + 2*(d2S/dxdy)^2 + (d2S/dy2)^2 ]dxdy
                          [                                            ]
                  
here integral is calculated over entire grid, and "~" means "proportional"
because integral is normalized after calcilation. Extremely  large  values
of Lambda result in linear fit being performed.

NOTE: this algorithm is the most robust and controllable one,  but  it  is
      limited by 512x512 grids and (say) up to 1.000.000 points.  However,
      ALGLIB has one more  spline  solver:  FastDDM  algorithm,  which  is
      intended for really large-scale problems (in 10M-100M range). FastDDM
      algorithm also has better parallelism properties.
      
More information on BlockLLS solver:
* memory requirements: ~[32*K^3+256*NPoints]  bytes  for  KxK  grid   with
  NPoints-sized dataset
* serial running time: O(K^4+NPoints)
* parallelism potential: limited. You may get some sublinear gain when
  working with large grids (K's in 256..512 range)

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

INPUT PARAMETERS:
    S       -   spline 2D builder object
    LambdaNS-   non-negative value:
                * positive value means that some smoothing is applied
                * zero value means  that  no  smoothing  is  applied,  and
                  corresponding entries of design matrix  are  numerically
                  zero and dropped from consideration.

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetalgoblocklls(spline2dbuilder* state,
     double lambdans,
     ae_state *_state);


/*************************************************************************
This  function  allows  you to choose least squares solver used to perform
fitting. This function sets solver algorithm to "NaiveLLS".

IMPORTANT: NaiveLLS is NOT intended to be used in  real  life  code!  This
           algorithm solves problem by generated dense (K^2)x(K^2+NPoints)
           matrix and solves  linear  least  squares  problem  with  dense
           solver.
           
           It is here just  to  test  BlockLLS  against  reference  solver
           (and maybe for someone trying to compare well optimized  solver
           against straightforward approach to the LLS problem).

More information on naive LLS solver:
* memory requirements: ~[8*K^4+256*NPoints] bytes for KxK grid.
* serial running time: O(K^6+NPoints) for KxK grid
* when compared with BlockLLS,  NaiveLLS  has ~K  larger memory demand and
  ~K^2  larger running time.

INPUT PARAMETERS:
    S       -   spline 2D builder object
    LambdaNS-   nonsmoothness penalty

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetalgonaivells(spline2dbuilder* state,
     double lambdans,
     ae_state *_state);


/*************************************************************************
This function fits bicubic spline to current dataset, using current  area/
grid and current LLS solver.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

INPUT PARAMETERS:
    State   -   spline 2D builder object

OUTPUT PARAMETERS:
    S       -   2D spline, fit result
    Rep     -   fitting report, which provides some additional info  about
                errors, R2 coefficient and so on.

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dfit(spline2dbuilder* state,
     spline2dinterpolant* s,
     spline2dfitreport* rep,
     ae_state *_state);


/*************************************************************************
Serializer: allocation

  -- ALGLIB --
     Copyright 28.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dalloc(ae_serializer* s,
     const spline2dinterpolant* spline,
     ae_state *_state);


/*************************************************************************
Serializer: serialization

  -- ALGLIB --
     Copyright 28.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dserialize(ae_serializer* s,
     const spline2dinterpolant* spline,
     ae_state *_state);


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 28.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dunserialize(ae_serializer* s,
     spline2dinterpolant* spline,
     ae_state *_state);
void _spline2dinterpolant_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _spline2dinterpolant_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _spline2dinterpolant_clear(void* _p);
void _spline2dinterpolant_destroy(void* _p);
void _spline2dbuilder_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _spline2dbuilder_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _spline2dbuilder_clear(void* _p);
void _spline2dbuilder_destroy(void* _p);
void _spline2dfitreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _spline2dfitreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _spline2dfitreport_clear(void* _p);
void _spline2dfitreport_destroy(void* _p);
void _spline2dxdesignmatrix_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _spline2dxdesignmatrix_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _spline2dxdesignmatrix_clear(void* _p);
void _spline2dxdesignmatrix_destroy(void* _p);
void _spline2dblockllsbuf_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _spline2dblockllsbuf_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _spline2dblockllsbuf_clear(void* _p);
void _spline2dblockllsbuf_destroy(void* _p);
void _spline2dfastddmbuf_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _spline2dfastddmbuf_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _spline2dfastddmbuf_clear(void* _p);
void _spline2dfastddmbuf_destroy(void* _p);


/*$ End $*/
#endif

