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

#ifndef _spline1d_h
#define _spline1d_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "tsort.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "scodes.h"
#include "xdebug.h"
#include "hqrnd.h"
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
#include "hblas.h"
#include "sblas.h"
#include "ortfac.h"
#include "fbls.h"
#include "normestimator.h"
#include "blas.h"
#include "bdsvd.h"
#include "svd.h"
#include "linlsqr.h"


/*$ Declarations $*/


/*************************************************************************
1-dimensional spline interpolant
*************************************************************************/
typedef struct
{
    ae_bool periodic;
    ae_int_t n;
    ae_int_t k;
    ae_int_t continuity;
    ae_vector x;
    ae_vector c;
} spline1dinterpolant;


/*************************************************************************
Spline fitting report:
    TerminationType completion code:
                    * >0 for success
                    * <0 for failure
    RMSError        RMS error
    AvgError        average error
    AvgRelError     average relative error (for non-zero Y[I])
    MaxError        maximum error
    
Fields  below are  filled  by   obsolete    functions   (Spline1DFitCubic,
Spline1DFitHermite). Modern fitting functions do NOT fill these fields:
    TaskRCond       reciprocal of task's condition number
*************************************************************************/
typedef struct
{
    ae_int_t terminationtype;
    double taskrcond;
    double rmserror;
    double avgerror;
    double avgrelerror;
    double maxerror;
} spline1dfitreport;


/*************************************************************************
A set of M B-spline basis functions for a 1D spline fitting problem.

The functions are defined on [0,1],  with  0-th  function  having  maximum
value at X=0, (M-1)-th function having maximum value at X=1, and the  rest
being equdistantly spaced.

  -- ALGLIB PROJECT --
     Copyright 09.04.2022 by Bochkanov Sergey.
*************************************************************************/
typedef struct
{
    ae_int_t m;
    ae_int_t bfrad;
    spline1dinterpolant s0;
    spline1dinterpolant s1;
    spline1dinterpolant s2;
    ae_vector tmpx;
    ae_vector tmpy;
} spline1dbbasis;


/*$ Body $*/


/*************************************************************************
This subroutine builds linear spline interpolant

INPUT PARAMETERS:
    X   -   spline nodes, array[0..N-1]
    Y   -   function values, array[0..N-1]
    N   -   points count (optional):
            * N>=2
            * if given, only first N points are used to build spline
            * if not given, automatically detected from X/Y sizes
              (len(X) must be equal to len(Y))
    
OUTPUT PARAMETERS:
    C   -   spline interpolant


ORDER OF POINTS

Subroutine automatically sorts points, so caller may pass unsorted array.

  -- ALGLIB PROJECT --
     Copyright 24.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline1dbuildlinear(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     spline1dinterpolant* c,
     ae_state *_state);


/*************************************************************************
This subroutine builds linear spline interpolant.

Buffered version of Spline1DBuildLinear() which reused  memory  previously
allocated in C as much as possible.

  -- ALGLIB PROJECT --
     Copyright 24.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline1dbuildlinearbuf(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     ae_int_t n,
     spline1dinterpolant* c,
     ae_state *_state);


/*************************************************************************
This subroutine builds cubic spline interpolant.

INPUT PARAMETERS:
    X           -   spline nodes, array[0..N-1].
    Y           -   function values, array[0..N-1].
    
OPTIONAL PARAMETERS:
    N           -   points count:
                    * N>=2
                    * if given, only first N points are used to build spline
                    * if not given, automatically detected from X/Y sizes
                      (len(X) must be equal to len(Y))
    BoundLType  -   boundary condition type for the left boundary
    BoundL      -   left boundary condition (first or second derivative,
                    depending on the BoundLType)
    BoundRType  -   boundary condition type for the right boundary
    BoundR      -   right boundary condition (first or second derivative,
                    depending on the BoundRType)

OUTPUT PARAMETERS:
    C           -   spline interpolant

ORDER OF POINTS

Subroutine automatically sorts points, so caller may pass unsorted array.

SETTING BOUNDARY VALUES:

The BoundLType/BoundRType parameters can have the following values:
    * -1, which corresonds to the periodic (cyclic) boundary conditions.
          In this case:
          * both BoundLType and BoundRType must be equal to -1.
          * BoundL/BoundR are ignored
          * Y[last] is ignored (it is assumed to be equal to Y[first]).
    *  0, which  corresponds  to  the  parabolically   terminated  spline
          (BoundL and/or BoundR are ignored).
    *  1, which corresponds to the first derivative boundary condition
    *  2, which corresponds to the second derivative boundary condition
    *  by default, BoundType=0 is used

PROBLEMS WITH PERIODIC BOUNDARY CONDITIONS:

Problems with periodic boundary conditions have Y[first_point]=Y[last_point].
However, this subroutine doesn't require you to specify equal  values  for
the first and last points - it automatically forces them  to  be  equal by
copying  Y[first_point]  (corresponds  to the leftmost,  minimal  X[])  to
Y[last_point]. However it is recommended to pass consistent values of Y[],
i.e. to make Y[first_point]=Y[last_point].

  -- ALGLIB PROJECT --
     Copyright 23.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline1dbuildcubic(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     ae_int_t n,
     ae_int_t boundltype,
     double boundl,
     ae_int_t boundrtype,
     double boundr,
     spline1dinterpolant* c,
     ae_state *_state);


/*************************************************************************
This function solves following problem: given table y[] of function values
at nodes x[], it calculates and returns table of function derivatives  d[]
(calculated at the same nodes x[]).

This function yields same result as Spline1DBuildCubic() call followed  by
sequence of Spline1DDiff() calls, but it can be several times faster  when
called for ordered X[] and X2[].

INPUT PARAMETERS:
    X           -   spline nodes
    Y           -   function values

OPTIONAL PARAMETERS:
    N           -   points count:
                    * N>=2
                    * if given, only first N points are used
                    * if not given, automatically detected from X/Y sizes
                      (len(X) must be equal to len(Y))
    BoundLType  -   boundary condition type for the left boundary
    BoundL      -   left boundary condition (first or second derivative,
                    depending on the BoundLType)
    BoundRType  -   boundary condition type for the right boundary
    BoundR      -   right boundary condition (first or second derivative,
                    depending on the BoundRType)

OUTPUT PARAMETERS:
    D           -   derivative values at X[]

ORDER OF POINTS

Subroutine automatically sorts points, so caller may pass unsorted array.
Derivative values are correctly reordered on return, so  D[I]  is  always
equal to S'(X[I]) independently of points order.

SETTING BOUNDARY VALUES:

The BoundLType/BoundRType parameters can have the following values:
    * -1, which corresonds to the periodic (cyclic) boundary conditions.
          In this case:
          * both BoundLType and BoundRType must be equal to -1.
          * BoundL/BoundR are ignored
          * Y[last] is ignored (it is assumed to be equal to Y[first]).
    *  0, which  corresponds  to  the  parabolically   terminated  spline
          (BoundL and/or BoundR are ignored).
    *  1, which corresponds to the first derivative boundary condition
    *  2, which corresponds to the second derivative boundary condition
    *  by default, BoundType=0 is used

PROBLEMS WITH PERIODIC BOUNDARY CONDITIONS:

Problems with periodic boundary conditions have Y[first_point]=Y[last_point].
However, this subroutine doesn't require you to specify equal  values  for
the first and last points - it automatically forces them  to  be  equal by
copying  Y[first_point]  (corresponds  to the leftmost,  minimal  X[])  to
Y[last_point]. However it is recommended to pass consistent values of Y[],
i.e. to make Y[first_point]=Y[last_point].

  -- ALGLIB PROJECT --
     Copyright 03.09.2010 by Bochkanov Sergey
*************************************************************************/
void spline1dgriddiffcubic(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     ae_int_t n,
     ae_int_t boundltype,
     double boundl,
     ae_int_t boundrtype,
     double boundr,
     /* Real    */ ae_vector* d,
     ae_state *_state);


/*************************************************************************
This function solves following problem: given table y[] of function values
at  nodes  x[],  it  calculates  and  returns  tables  of first and second
function derivatives d1[] and d2[] (calculated at the same nodes x[]).

This function yields same result as Spline1DBuildCubic() call followed  by
sequence of Spline1DDiff() calls, but it can be several times faster  when
called for ordered X[] and X2[].

INPUT PARAMETERS:
    X           -   spline nodes
    Y           -   function values

OPTIONAL PARAMETERS:
    N           -   points count:
                    * N>=2
                    * if given, only first N points are used
                    * if not given, automatically detected from X/Y sizes
                      (len(X) must be equal to len(Y))
    BoundLType  -   boundary condition type for the left boundary
    BoundL      -   left boundary condition (first or second derivative,
                    depending on the BoundLType)
    BoundRType  -   boundary condition type for the right boundary
    BoundR      -   right boundary condition (first or second derivative,
                    depending on the BoundRType)

OUTPUT PARAMETERS:
    D1          -   S' values at X[]
    D2          -   S'' values at X[]

ORDER OF POINTS

Subroutine automatically sorts points, so caller may pass unsorted array.
Derivative values are correctly reordered on return, so  D[I]  is  always
equal to S'(X[I]) independently of points order.

SETTING BOUNDARY VALUES:

The BoundLType/BoundRType parameters can have the following values:
    * -1, which corresonds to the periodic (cyclic) boundary conditions.
          In this case:
          * both BoundLType and BoundRType must be equal to -1.
          * BoundL/BoundR are ignored
          * Y[last] is ignored (it is assumed to be equal to Y[first]).
    *  0, which  corresponds  to  the  parabolically   terminated  spline
          (BoundL and/or BoundR are ignored).
    *  1, which corresponds to the first derivative boundary condition
    *  2, which corresponds to the second derivative boundary condition
    *  by default, BoundType=0 is used

PROBLEMS WITH PERIODIC BOUNDARY CONDITIONS:

Problems with periodic boundary conditions have Y[first_point]=Y[last_point].
However, this subroutine doesn't require you to specify equal  values  for
the first and last points - it automatically forces them  to  be  equal by
copying  Y[first_point]  (corresponds  to the leftmost,  minimal  X[])  to
Y[last_point]. However it is recommended to pass consistent values of Y[],
i.e. to make Y[first_point]=Y[last_point].

  -- ALGLIB PROJECT --
     Copyright 03.09.2010 by Bochkanov Sergey
*************************************************************************/
void spline1dgriddiff2cubic(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     ae_int_t n,
     ae_int_t boundltype,
     double boundl,
     ae_int_t boundrtype,
     double boundr,
     /* Real    */ ae_vector* d1,
     /* Real    */ ae_vector* d2,
     ae_state *_state);


/*************************************************************************
This function solves following problem: given table y[] of function values
at old nodes x[]  and new nodes  x2[],  it calculates and returns table of
function values y2[] (calculated at x2[]).

This function yields same result as Spline1DBuildCubic() call followed  by
sequence of Spline1DDiff() calls, but it can be several times faster  when
called for ordered X[] and X2[].

INPUT PARAMETERS:
    X           -   old spline nodes
    Y           -   function values
    X2           -  new spline nodes

OPTIONAL PARAMETERS:
    N           -   points count:
                    * N>=2
                    * if given, only first N points from X/Y are used
                    * if not given, automatically detected from X/Y sizes
                      (len(X) must be equal to len(Y))
    BoundLType  -   boundary condition type for the left boundary
    BoundL      -   left boundary condition (first or second derivative,
                    depending on the BoundLType)
    BoundRType  -   boundary condition type for the right boundary
    BoundR      -   right boundary condition (first or second derivative,
                    depending on the BoundRType)
    N2          -   new points count:
                    * N2>=2
                    * if given, only first N2 points from X2 are used
                    * if not given, automatically detected from X2 size

OUTPUT PARAMETERS:
    F2          -   function values at X2[]

ORDER OF POINTS

Subroutine automatically sorts points, so caller  may pass unsorted array.
Function  values  are correctly reordered on  return, so F2[I]  is  always
equal to S(X2[I]) independently of points order.

SETTING BOUNDARY VALUES:

The BoundLType/BoundRType parameters can have the following values:
    * -1, which corresonds to the periodic (cyclic) boundary conditions.
          In this case:
          * both BoundLType and BoundRType must be equal to -1.
          * BoundL/BoundR are ignored
          * Y[last] is ignored (it is assumed to be equal to Y[first]).
    *  0, which  corresponds  to  the  parabolically   terminated  spline
          (BoundL and/or BoundR are ignored).
    *  1, which corresponds to the first derivative boundary condition
    *  2, which corresponds to the second derivative boundary condition
    *  by default, BoundType=0 is used

PROBLEMS WITH PERIODIC BOUNDARY CONDITIONS:

Problems with periodic boundary conditions have Y[first_point]=Y[last_point].
However, this subroutine doesn't require you to specify equal  values  for
the first and last points - it automatically forces them  to  be  equal by
copying  Y[first_point]  (corresponds  to the leftmost,  minimal  X[])  to
Y[last_point]. However it is recommended to pass consistent values of Y[],
i.e. to make Y[first_point]=Y[last_point].

  -- ALGLIB PROJECT --
     Copyright 03.09.2010 by Bochkanov Sergey
*************************************************************************/
void spline1dconvcubic(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     ae_int_t n,
     ae_int_t boundltype,
     double boundl,
     ae_int_t boundrtype,
     double boundr,
     /* Real    */ const ae_vector* _x2,
     ae_int_t n2,
     /* Real    */ ae_vector* y2,
     ae_state *_state);


/*************************************************************************
This function solves following problem: given table y[] of function values
at old nodes x[]  and new nodes  x2[],  it calculates and returns table of
function values y2[] and derivatives d2[] (calculated at x2[]).

This function yields same result as Spline1DBuildCubic() call followed  by
sequence of Spline1DDiff() calls, but it can be several times faster  when
called for ordered X[] and X2[].

INPUT PARAMETERS:
    X           -   old spline nodes
    Y           -   function values
    X2           -  new spline nodes

OPTIONAL PARAMETERS:
    N           -   points count:
                    * N>=2
                    * if given, only first N points from X/Y are used
                    * if not given, automatically detected from X/Y sizes
                      (len(X) must be equal to len(Y))
    BoundLType  -   boundary condition type for the left boundary
    BoundL      -   left boundary condition (first or second derivative,
                    depending on the BoundLType)
    BoundRType  -   boundary condition type for the right boundary
    BoundR      -   right boundary condition (first or second derivative,
                    depending on the BoundRType)
    N2          -   new points count:
                    * N2>=2
                    * if given, only first N2 points from X2 are used
                    * if not given, automatically detected from X2 size

OUTPUT PARAMETERS:
    F2          -   function values at X2[]
    D2          -   first derivatives at X2[]

ORDER OF POINTS

Subroutine automatically sorts points, so caller  may pass unsorted array.
Function  values  are correctly reordered on  return, so F2[I]  is  always
equal to S(X2[I]) independently of points order.

SETTING BOUNDARY VALUES:

The BoundLType/BoundRType parameters can have the following values:
    * -1, which corresonds to the periodic (cyclic) boundary conditions.
          In this case:
          * both BoundLType and BoundRType must be equal to -1.
          * BoundL/BoundR are ignored
          * Y[last] is ignored (it is assumed to be equal to Y[first]).
    *  0, which  corresponds  to  the  parabolically   terminated  spline
          (BoundL and/or BoundR are ignored).
    *  1, which corresponds to the first derivative boundary condition
    *  2, which corresponds to the second derivative boundary condition
    *  by default, BoundType=0 is used

PROBLEMS WITH PERIODIC BOUNDARY CONDITIONS:

Problems with periodic boundary conditions have Y[first_point]=Y[last_point].
However, this subroutine doesn't require you to specify equal  values  for
the first and last points - it automatically forces them  to  be  equal by
copying  Y[first_point]  (corresponds  to the leftmost,  minimal  X[])  to
Y[last_point]. However it is recommended to pass consistent values of Y[],
i.e. to make Y[first_point]=Y[last_point].

  -- ALGLIB PROJECT --
     Copyright 03.09.2010 by Bochkanov Sergey
*************************************************************************/
void spline1dconvdiffcubic(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     ae_int_t n,
     ae_int_t boundltype,
     double boundl,
     ae_int_t boundrtype,
     double boundr,
     /* Real    */ const ae_vector* _x2,
     ae_int_t n2,
     /* Real    */ ae_vector* y2,
     /* Real    */ ae_vector* d2,
     ae_state *_state);


/*************************************************************************
This function solves following problem: given table y[] of function values
at old nodes x[]  and new nodes  x2[],  it calculates and returns table of
function  values  y2[],  first  and  second  derivatives  d2[]  and  dd2[]
(calculated at x2[]).

This function yields same result as Spline1DBuildCubic() call followed  by
sequence of Spline1DDiff() calls, but it can be several times faster  when
called for ordered X[] and X2[].

INPUT PARAMETERS:
    X           -   old spline nodes
    Y           -   function values
    X2           -  new spline nodes

OPTIONAL PARAMETERS:
    N           -   points count:
                    * N>=2
                    * if given, only first N points from X/Y are used
                    * if not given, automatically detected from X/Y sizes
                      (len(X) must be equal to len(Y))
    BoundLType  -   boundary condition type for the left boundary
    BoundL      -   left boundary condition (first or second derivative,
                    depending on the BoundLType)
    BoundRType  -   boundary condition type for the right boundary
    BoundR      -   right boundary condition (first or second derivative,
                    depending on the BoundRType)
    N2          -   new points count:
                    * N2>=2
                    * if given, only first N2 points from X2 are used
                    * if not given, automatically detected from X2 size

OUTPUT PARAMETERS:
    F2          -   function values at X2[]
    D2          -   first derivatives at X2[]
    DD2         -   second derivatives at X2[]

ORDER OF POINTS

Subroutine automatically sorts points, so caller  may pass unsorted array.
Function  values  are correctly reordered on  return, so F2[I]  is  always
equal to S(X2[I]) independently of points order.

SETTING BOUNDARY VALUES:

The BoundLType/BoundRType parameters can have the following values:
    * -1, which corresonds to the periodic (cyclic) boundary conditions.
          In this case:
          * both BoundLType and BoundRType must be equal to -1.
          * BoundL/BoundR are ignored
          * Y[last] is ignored (it is assumed to be equal to Y[first]).
    *  0, which  corresponds  to  the  parabolically   terminated  spline
          (BoundL and/or BoundR are ignored).
    *  1, which corresponds to the first derivative boundary condition
    *  2, which corresponds to the second derivative boundary condition
    *  by default, BoundType=0 is used

PROBLEMS WITH PERIODIC BOUNDARY CONDITIONS:

Problems with periodic boundary conditions have Y[first_point]=Y[last_point].
However, this subroutine doesn't require you to specify equal  values  for
the first and last points - it automatically forces them  to  be  equal by
copying  Y[first_point]  (corresponds  to the leftmost,  minimal  X[])  to
Y[last_point]. However it is recommended to pass consistent values of Y[],
i.e. to make Y[first_point]=Y[last_point].

  -- ALGLIB PROJECT --
     Copyright 03.09.2010 by Bochkanov Sergey
*************************************************************************/
void spline1dconvdiff2cubic(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     ae_int_t n,
     ae_int_t boundltype,
     double boundl,
     ae_int_t boundrtype,
     double boundr,
     /* Real    */ const ae_vector* _x2,
     ae_int_t n2,
     /* Real    */ ae_vector* y2,
     /* Real    */ ae_vector* d2,
     /* Real    */ ae_vector* dd2,
     ae_state *_state);


/*************************************************************************
This subroutine builds Catmull-Rom spline interpolant.

INPUT PARAMETERS:
    X           -   spline nodes, array[0..N-1].
    Y           -   function values, array[0..N-1].
    
OPTIONAL PARAMETERS:
    N           -   points count:
                    * N>=2
                    * if given, only first N points are used to build spline
                    * if not given, automatically detected from X/Y sizes
                      (len(X) must be equal to len(Y))
    BoundType   -   boundary condition type:
                    * -1 for periodic boundary condition
                    *  0 for parabolically terminated spline (default)
    Tension     -   tension parameter:
                    * tension=0   corresponds to classic Catmull-Rom spline (default)
                    * 0<tension<1 corresponds to more general form - cardinal spline

OUTPUT PARAMETERS:
    C           -   spline interpolant


ORDER OF POINTS

Subroutine automatically sorts points, so caller may pass unsorted array.

PROBLEMS WITH PERIODIC BOUNDARY CONDITIONS:

Problems with periodic boundary conditions have Y[first_point]=Y[last_point].
However, this subroutine doesn't require you to specify equal  values  for
the first and last points - it automatically forces them  to  be  equal by
copying  Y[first_point]  (corresponds  to the leftmost,  minimal  X[])  to
Y[last_point]. However it is recommended to pass consistent values of Y[],
i.e. to make Y[first_point]=Y[last_point].

  -- ALGLIB PROJECT --
     Copyright 23.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline1dbuildcatmullrom(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     ae_int_t n,
     ae_int_t boundtype,
     double tension,
     spline1dinterpolant* c,
     ae_state *_state);


/*************************************************************************
This subroutine builds Hermite spline interpolant.

INPUT PARAMETERS:
    X           -   spline nodes, array[0..N-1]
    Y           -   function values, array[0..N-1]
    D           -   derivatives, array[0..N-1]
    N           -   points count (optional):
                    * N>=2
                    * if given, only first N points are used to build spline
                    * if not given, automatically detected from X/Y sizes
                      (len(X) must be equal to len(Y))

OUTPUT PARAMETERS:
    C           -   spline interpolant.


ORDER OF POINTS

Subroutine automatically sorts points, so caller may pass unsorted array.

  -- ALGLIB PROJECT --
     Copyright 23.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline1dbuildhermite(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* d,
     ae_int_t n,
     spline1dinterpolant* c,
     ae_state *_state);


/*************************************************************************
This subroutine builds Hermite spline interpolant.

Buffered version which reuses memory previously allocated in C as much  as
possible.

  -- ALGLIB PROJECT --
     Copyright 23.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline1dbuildhermitebuf(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     /* Real    */ const ae_vector* _d,
     ae_int_t n,
     spline1dinterpolant* c,
     ae_state *_state);


/*************************************************************************
This subroutine builds Akima spline interpolant

INPUT PARAMETERS:
    X           -   spline nodes, array[0..N-1]
    Y           -   function values, array[0..N-1]
    N           -   points count (optional):
                    * N>=2
                    * if given, only first N points are used to build spline
                    * if not given, automatically detected from X/Y sizes
                      (len(X) must be equal to len(Y))

OUTPUT PARAMETERS:
    C           -   spline interpolant


ORDER OF POINTS

Subroutine automatically sorts points, so caller may pass unsorted array.

  -- ALGLIB PROJECT --
     Copyright 24.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline1dbuildakima(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     spline1dinterpolant* c,
     ae_state *_state);


/*************************************************************************
This subroutine builds modified Akima spline interpolant, with weights

    W[i]=|Delta[I]-Delta[I-1]|

replaced by

    W[i]=|Delta[I]-Delta[I-1]|+0.5*|Delta[I]+Delta[I-1]|

INPUT PARAMETERS:
    X           -   spline nodes, array[0..N-1]
    Y           -   function values, array[0..N-1]
    N           -   points count (optional):
                    * N>=2
                    * if given, only first N points are used to build spline
                    * if not given, automatically detected from X/Y sizes
                      (len(X) must be equal to len(Y))

OUTPUT PARAMETERS:
    C           -   spline interpolant


ORDER OF POINTS

Subroutine automatically sorts points, so caller may pass unsorted array.

  -- ALGLIB PROJECT --
     Copyright 24.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline1dbuildakimamod(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     spline1dinterpolant* c,
     ae_state *_state);


/*************************************************************************
This subroutine calculates the value of the spline at the given point X.

INPUT PARAMETERS:
    C   -   spline interpolant
    X   -   point

Result:
    S(x)

  -- ALGLIB PROJECT --
     Copyright 23.06.2007 by Bochkanov Sergey
*************************************************************************/
double spline1dcalc(const spline1dinterpolant* c,
     double x,
     ae_state *_state);


/*************************************************************************
This subroutine differentiates the spline.

INPUT PARAMETERS:
    C   -   spline interpolant.
    X   -   point

Result:
    S   -   S(x)
    DS  -   S'(x)
    D2S -   S''(x)

  -- ALGLIB PROJECT --
     Copyright 24.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline1ddiff(const spline1dinterpolant* c,
     double x,
     double* s,
     double* ds,
     double* d2s,
     ae_state *_state);


/*************************************************************************
This subroutine makes the copy of the spline.

INPUT PARAMETERS:
    C   -   spline interpolant.

Result:
    CC  -   spline copy

  -- ALGLIB PROJECT --
     Copyright 29.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline1dcopy(const spline1dinterpolant* c,
     spline1dinterpolant* cc,
     ae_state *_state);


/*************************************************************************
Serializer: allocation

  -- ALGLIB --
     Copyright 16.04.2023 by Bochkanov Sergey
*************************************************************************/
void spline1dalloc(ae_serializer* s,
     const spline1dinterpolant* model,
     ae_state *_state);


/*************************************************************************
Serializer: serialization

  -- ALGLIB --
     Copyright 16.04.2023 by Bochkanov Sergey
*************************************************************************/
void spline1dserialize(ae_serializer* s,
     const spline1dinterpolant* model,
     ae_state *_state);


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 16.04.2023 by Bochkanov Sergey
*************************************************************************/
void spline1dunserialize(ae_serializer* s,
     spline1dinterpolant* model,
     ae_state *_state);


/*************************************************************************
This subroutine unpacks the spline into the coefficients table.

INPUT PARAMETERS:
    C   -   spline interpolant.
    X   -   point

OUTPUT PARAMETERS:
    Tbl -   coefficients table, unpacked format, array[0..N-2, 0..5].
            For I = 0...N-2:
                Tbl[I,0] = X[i]
                Tbl[I,1] = X[i+1]
                Tbl[I,2] = C0
                Tbl[I,3] = C1
                Tbl[I,4] = C2
                Tbl[I,5] = C3
            On [x[i], x[i+1]] spline is equals to:
                S(x) = C0 + C1*t + C2*t^2 + C3*t^3
                t = x-x[i]
                
NOTE:
    You  can rebuild spline with  Spline1DBuildHermite()  function,  which
    accepts as inputs function values and derivatives at nodes, which  are
    easy to calculate when you have coefficients.

  -- ALGLIB PROJECT --
     Copyright 29.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline1dunpack(const spline1dinterpolant* c,
     ae_int_t* n,
     /* Real    */ ae_matrix* tbl,
     ae_state *_state);


/*************************************************************************
This subroutine performs linear transformation of the spline argument.

INPUT PARAMETERS:
    C   -   spline interpolant.
    A, B-   transformation coefficients: x = A*t + B
Result:
    C   -   transformed spline

  -- ALGLIB PROJECT --
     Copyright 30.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline1dlintransx(spline1dinterpolant* c,
     double a,
     double b,
     ae_state *_state);


/*************************************************************************
This subroutine performs linear transformation of the spline.

INPUT PARAMETERS:
    C   -   spline interpolant.
    A, B-   transformation coefficients: S2(x) = A*S(x) + B
Result:
    C   -   transformed spline

  -- ALGLIB PROJECT --
     Copyright 30.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline1dlintransy(spline1dinterpolant* c,
     double a,
     double b,
     ae_state *_state);


/*************************************************************************
This subroutine integrates the spline.

INPUT PARAMETERS:
    C   -   spline interpolant.
    X   -   right bound of the integration interval [a, x],
            here 'a' denotes min(x[])
Result:
    integral(S(t)dt,a,x)

  -- ALGLIB PROJECT --
     Copyright 23.06.2007 by Bochkanov Sergey
*************************************************************************/
double spline1dintegrate(const spline1dinterpolant* c,
     double x,
     ae_state *_state);


/*************************************************************************
Fitting by the smoothing (penalized) cubic spline.

This function approximates N scattered points (some of X[] may be equal to
each other) by the cubic spline with M equidistant nodes spanning interval
[min(x),max(x)].

The problem is regularized by adding nonlinearity  penalty  to  the  usual
least squares penalty function:

    MERIT_FUNC = F_LS + F_NL

where F_LS is a least squares error  term,  and  F_NL  is  a  nonlinearity
penalty which is roughly proportional to LambdaNS*integral{ S''(x)^2*dx }.
Algorithm applies automatic renormalization of F_NL  which  makes  penalty
term roughly invariant to scaling of X[] and changes in M.

This function is a new edition  of  penalized  regression  spline fitting,
a fast and compact one which needs much less resources that  its  previous
version: just O(maxMN) memory and O(maxMN) time.

NOTE: it is OK to run this function with both M<<N and M>>N;  say,  it  is
      possible to process 100 points with 1000-node spline.
           
INPUT PARAMETERS:
    X           -   points, array[0..N-1].
    Y           -   function values, array[0..N-1].
    N           -   number of points (optional):
                    * N>0
                    * if given, only first N elements of X/Y are processed
                    * if not given, automatically determined from lengths
    M           -   number of basis functions ( = number_of_nodes), M>=4.
    LambdaNS    -   LambdaNS>=0, regularization  constant  passed by user.
                    It penalizes nonlinearity in the regression spline.
                    Possible values to start from are 0.00001, 0.1, 1

OUTPUT PARAMETERS:
    S   -   spline interpolant.
    Rep -   Following fields are set:
            * TerminationType set to 1
            * RMSError      rms error on the (X,Y).
            * AvgError      average error on the (X,Y).
            * AvgRelError   average relative error on the non-zero Y
            * MaxError      maximum error

  -- ALGLIB PROJECT --
     Copyright 10.04.2023 by Bochkanov Sergey
*************************************************************************/
void spline1dfit(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     ae_int_t n,
     ae_int_t m,
     double lambdans,
     spline1dinterpolant* s,
     spline1dfitreport* rep,
     ae_state *_state);


/*************************************************************************
Internal version of Spline1DGridDiffCubic.

Accepts pre-ordered X/Y, temporary arrays (which may be  preallocated,  if
you want to save time, or not) and output array (which may be preallocated
too).

Y is passed as var-parameter because we may need to force last element  to
be equal to the first one (if periodic boundary conditions are specified).

  -- ALGLIB PROJECT --
     Copyright 03.09.2010 by Bochkanov Sergey
*************************************************************************/
void spline1dgriddiffcubicinternal(/* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_int_t n,
     ae_int_t boundltype,
     double boundl,
     ae_int_t boundrtype,
     double boundr,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_vector* a1,
     /* Real    */ ae_vector* a2,
     /* Real    */ ae_vector* a3,
     /* Real    */ ae_vector* b,
     /* Real    */ ae_vector* dt,
     ae_state *_state);


/*************************************************************************
Internal version of Spline1DConvDiff

Converts from Hermite spline given by grid XOld to new grid X2

INPUT PARAMETERS:
    XOld    -   old grid
    YOld    -   values at old grid
    DOld    -   first derivative at old grid
    N       -   grid size
    X2      -   new grid
    N2      -   new grid size
    Y       -   possibly preallocated output array
                (reallocate if too small)
    NeedY   -   do we need Y?
    D1      -   possibly preallocated output array
                (reallocate if too small)
    NeedD1  -   do we need D1?
    D2      -   possibly preallocated output array
                (reallocate if too small)
    NeedD2  -   do we need D1?

OUTPUT ARRAYS:
    Y       -   values, if needed
    D1      -   first derivative, if needed
    D2      -   second derivative, if needed

  -- ALGLIB PROJECT --
     Copyright 03.09.2010 by Bochkanov Sergey
*************************************************************************/
void spline1dconvdiffinternal(/* Real    */ const ae_vector* xold,
     /* Real    */ const ae_vector* yold,
     /* Real    */ const ae_vector* dold,
     ae_int_t n,
     /* Real    */ const ae_vector* x2,
     ae_int_t n2,
     /* Real    */ ae_vector* y,
     ae_bool needy,
     /* Real    */ ae_vector* d1,
     ae_bool needd1,
     /* Real    */ ae_vector* d2,
     ae_bool needd2,
     ae_state *_state);


/*************************************************************************
This function finds all roots and extrema of the spline S(x)  defined  at
[A,B] (interval which contains spline nodes).

It  does not extrapolates function, so roots and extrema located  outside 
of [A,B] will not be found. It returns all isolated (including  multiple)
roots and extrema.

INPUT PARAMETERS
    C           -   spline interpolant
    
OUTPUT PARAMETERS
    R           -   array[NR], contains roots of the spline. 
                    In case there is no roots, this array has zero length.
    NR          -   number of roots, >=0
    DR          -   is set to True in case there is at least one interval
                    where spline is just a zero constant. Such degenerate
                    cases are not reported in the R/NR
    E           -   array[NE], contains  extrema  (maximums/minimums)  of 
                    the spline. In case there is no extrema,  this  array 
                    has zero length.
    ET          -   array[NE], extrema types:
                    * ET[i]>0 in case I-th extrema is a minimum
                    * ET[i]<0 in case I-th extrema is a maximum
    NE          -   number of extrema, >=0
    DE          -   is set to True in case there is at least one interval
                    where spline is a constant. Such degenerate cases are  
                    not reported in the E/NE.
                    
NOTES:

1. This function does NOT report following kinds of roots:
   * intervals where function is constantly zero
   * roots which are outside of [A,B] (note: it CAN return A or B)

2. This function does NOT report following kinds of extrema:
   * intervals where function is a constant
   * extrema which are outside of (A,B) (note: it WON'T return A or B)
   
 -- ALGLIB PROJECT --
     Copyright 26.09.2011 by Bochkanov Sergey   
*************************************************************************/
void spline1drootsandextrema(const spline1dinterpolant* c,
     /* Real    */ ae_vector* r,
     ae_int_t* nr,
     ae_bool* dr,
     /* Real    */ ae_vector* e,
     /* Integer */ ae_vector* et,
     ae_int_t* ne,
     ae_bool* de,
     ae_state *_state);


/*************************************************************************
Internal subroutine. Heap sort.
*************************************************************************/
void heapsortdpoints(/* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     /* Real    */ ae_vector* d,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
This procedure search roots of an quadratic equation inside [0;1] and it number of roots.

INPUT PARAMETERS:
    P0   -   value of a function at 0
    M0   -   value of a derivative at 0
    P1   -   value of a function at 1
    M1   -   value of a derivative at 1

OUTPUT PARAMETERS:
    X0   -  first root of an equation
    X1   -  second root of an equation
    NR   -  number of roots
    
RESTRICTIONS OF PARAMETERS:

Parameters for this procedure has't to be zero simultaneously. Is expected, 
that input polinom is't degenerate or constant identicaly ZERO.


REMARK:

The procedure always fill value for X1 and X2, even if it is't belongs to [0;1].
But first true root(even if existing one) is in X1.
Number of roots is NR.

 -- ALGLIB PROJECT --
     Copyright 26.09.2011 by Bochkanov Sergey
*************************************************************************/
void solvepolinom2(double p0,
     double m0,
     double p1,
     double m1,
     double* x0,
     double* x1,
     ae_int_t* nr,
     ae_state *_state);


/*************************************************************************
This procedure search roots of an cubic equation inside [A;B], it number of roots 
and number of extremums.

INPUT PARAMETERS:
    pA   -   value of a function at A
    mA   -   value of a derivative at A
    pB   -   value of a function at B
    mB   -   value of a derivative at B
    A0   -   left border [A0;B0]
    B0   -   right border [A0;B0]

OUTPUT PARAMETERS:
    X0   -  first root of an equation
    X1   -  second root of an equation
    X2   -  third root of an equation
    EX0  -  first extremum of a function
    EX0  -  second extremum of a function
    NR   -  number of roots
    NR   -  number of extrmums
    
RESTRICTIONS OF PARAMETERS:

Length of [A;B] must be positive and is't zero, i.e. A<>B and A<B.


REMARK:

If 'NR' is -1 it's mean, than polinom has infiniti roots.
If 'NE' is -1 it's mean, than polinom has infiniti extremums.

 -- ALGLIB PROJECT --
     Copyright 26.09.2011 by Bochkanov Sergey
*************************************************************************/
void solvecubicpolinom(double pa,
     double ma,
     double pb,
     double mb,
     double a,
     double b,
     double* x0,
     double* x1,
     double* x2,
     double* ex0,
     double* ex1,
     ae_int_t* nr,
     ae_int_t* ne,
     /* Real    */ ae_vector* tempdata,
     ae_state *_state);


/*************************************************************************
Function for searching a root at [A;B] by bisection method and return number of roots
(0 or 1)

INPUT PARAMETERS:
    pA   -   value of a function at A
    mA   -   value of a derivative at A
    pB   -   value of a function at B
    mB   -   value of a derivative at B
    A0   -   left border [A0;B0] 
    B0   -   right border [A0;B0]
    
RESTRICTIONS OF PARAMETERS:

We assume, that B0>A0.


REMARK:

Assume, that exist one root only at [A;B], else 
function may be work incorrectly.
The function dont check value A0,B0!

 -- ALGLIB PROJECT --
     Copyright 26.09.2011 by Bochkanov Sergey
*************************************************************************/
ae_int_t bisectmethod(double pa,
     double ma,
     double pb,
     double mb,
     double a,
     double b,
     double* x,
     ae_state *_state);


/*************************************************************************
This function builds monotone cubic Hermite interpolant. This interpolant
is monotonic in [x(0),x(n-1)] and is constant outside of this interval.

In  case  y[]  form  non-monotonic  sequence,  interpolant  is  piecewise
monotonic.  Say, for x=(0,1,2,3,4)  and  y=(0,1,2,1,0)  interpolant  will
monotonically grow at [0..2] and monotonically decrease at [2..4].

INPUT PARAMETERS:
    X           -   spline nodes, array[0..N-1]. Subroutine automatically
                    sorts points, so caller may pass unsorted array.
    Y           -   function values, array[0..N-1]
    N           -   the number of points(N>=2).

OUTPUT PARAMETERS:
    C           -   spline interpolant.

 -- ALGLIB PROJECT --
     Copyright 21.06.2012 by Bochkanov Sergey
*************************************************************************/
void spline1dbuildmonotone(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     ae_int_t n,
     spline1dinterpolant* c,
     ae_state *_state);
void _spline1dinterpolant_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _spline1dinterpolant_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _spline1dinterpolant_clear(void* _p);
void _spline1dinterpolant_destroy(void* _p);
void _spline1dfitreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _spline1dfitreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _spline1dfitreport_clear(void* _p);
void _spline1dfitreport_destroy(void* _p);
void _spline1dbbasis_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _spline1dbbasis_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _spline1dbbasis_clear(void* _p);
void _spline1dbbasis_destroy(void* _p);


/*$ End $*/
#endif

