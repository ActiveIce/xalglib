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

#ifndef _gq_h
#define _gq_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "hblas.h"
#include "creflections.h"
#include "sblas.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "ortfac.h"
#include "matgen.h"
#include "scodes.h"
#include "tsort.h"
#include "sparse.h"
#include "blas.h"
#include "rotations.h"
#include "hsschur.h"
#include "basicstatops.h"
#include "evd.h"
#include "gammafunc.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Computation of nodes and weights for a Gauss quadrature formula

The algorithm generates the N-point Gauss quadrature formula  with  weight
function given by coefficients alpha and beta  of  a  recurrence  relation
which generates a system of orthogonal polynomials:

P-1(x)   =  0
P0(x)    =  1
Pn+1(x)  =  (x-alpha(n))*Pn(x)  -  beta(n)*Pn-1(x)

and zeroth moment Mu0

Mu0 = integral(W(x)dx,a,b)

INPUT PARAMETERS:
    Alpha   -   array[0..N-1], alpha coefficients
    Beta    -   array[0..N-1], beta coefficients
                Zero-indexed element is not used and may be arbitrary.
                Beta[I]>0.
    Mu0     -   zeroth moment of the weight function.
    N       -   number of nodes of the quadrature formula, N>=1

OUTPUT PARAMETERS:
    Info    -   error code:
                * -3    internal eigenproblem solver hasn't converged
                * -2    Beta[i]<=0
                * -1    incorrect N was passed
                *  1    OK
    X       -   array[0..N-1] - array of quadrature nodes,
                in ascending order.
    W       -   array[0..N-1] - array of quadrature weights.

  -- ALGLIB --
     Copyright 2005-2009 by Bochkanov Sergey
*************************************************************************/
void gqgeneraterec(/* Real    */ const ae_vector* alpha,
     /* Real    */ const ae_vector* beta,
     double mu0,
     ae_int_t n,
     ae_int_t* info,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* w,
     ae_state *_state);


/*************************************************************************
Computation of nodes and weights for a Gauss-Lobatto quadrature formula

The algorithm generates the N-point Gauss-Lobatto quadrature formula  with
weight function given by coefficients alpha and beta of a recurrence which
generates a system of orthogonal polynomials.

P-1(x)   =  0
P0(x)    =  1
Pn+1(x)  =  (x-alpha(n))*Pn(x)  -  beta(n)*Pn-1(x)

and zeroth moment Mu0

Mu0 = integral(W(x)dx,a,b)

INPUT PARAMETERS:
    Alpha   -   array[0..N-2], alpha coefficients
    Beta    -   array[0..N-2], beta coefficients.
                Zero-indexed element is not used, may be arbitrary.
                Beta[I]>0
    Mu0     -   zeroth moment of the weighting function.
    A       -   left boundary of the integration interval.
    B       -   right boundary of the integration interval.
    N       -   number of nodes of the quadrature formula, N>=3
                (including the left and right boundary nodes).

OUTPUT PARAMETERS:
    Info    -   error code:
                * -3    internal eigenproblem solver hasn't converged
                * -2    Beta[i]<=0
                * -1    incorrect N was passed
                *  1    OK
    X       -   array[0..N-1] - array of quadrature nodes,
                in ascending order.
    W       -   array[0..N-1] - array of quadrature weights.

  -- ALGLIB --
     Copyright 2005-2009 by Bochkanov Sergey
*************************************************************************/
void gqgenerategausslobattorec(/* Real    */ const ae_vector* _alpha,
     /* Real    */ const ae_vector* _beta,
     double mu0,
     double a,
     double b,
     ae_int_t n,
     ae_int_t* info,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* w,
     ae_state *_state);


/*************************************************************************
Computation of nodes and weights for a Gauss-Radau quadrature formula

The algorithm generates the N-point Gauss-Radau  quadrature  formula  with
weight function given by the coefficients alpha and  beta  of a recurrence
which generates a system of orthogonal polynomials.

P-1(x)   =  0
P0(x)    =  1
Pn+1(x)  =  (x-alpha(n))*Pn(x)  -  beta(n)*Pn-1(x)

and zeroth moment Mu0

Mu0 = integral(W(x)dx,a,b)

INPUT PARAMETERS:
    Alpha   -   array[0..N-2], alpha coefficients.
    Beta    -   array[0..N-1], beta coefficients
                Zero-indexed element is not used.
                Beta[I]>0
    Mu0     -   zeroth moment of the weighting function.
    A       -   left boundary of the integration interval.
    N       -   number of nodes of the quadrature formula, N>=2
                (including the left boundary node).

OUTPUT PARAMETERS:
    Info    -   error code:
                * -3    internal eigenproblem solver hasn't converged
                * -2    Beta[i]<=0
                * -1    incorrect N was passed
                *  1    OK
    X       -   array[0..N-1] - array of quadrature nodes,
                in ascending order.
    W       -   array[0..N-1] - array of quadrature weights.


  -- ALGLIB --
     Copyright 2005-2009 by Bochkanov Sergey
*************************************************************************/
void gqgenerategaussradaurec(/* Real    */ const ae_vector* _alpha,
     /* Real    */ const ae_vector* _beta,
     double mu0,
     double a,
     ae_int_t n,
     ae_int_t* info,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* w,
     ae_state *_state);


/*************************************************************************
Returns nodes/weights for Gauss-Legendre quadrature on [-1,1] with N
nodes.

INPUT PARAMETERS:
    N           -   number of nodes, >=1

OUTPUT PARAMETERS:
    Info        -   error code:
                    * -4    an  error   was   detected   when  calculating
                            weights/nodes.  N  is  too  large   to  obtain
                            weights/nodes  with  high   enough   accuracy.
                            Try  to   use   multiple   precision  version.
                    * -3    internal eigenproblem solver hasn't  converged
                    * -1    incorrect N was passed
                    * +1    OK
    X           -   array[0..N-1] - array of quadrature nodes,
                    in ascending order.
    W           -   array[0..N-1] - array of quadrature weights.


  -- ALGLIB --
     Copyright 12.05.2009 by Bochkanov Sergey
*************************************************************************/
void gqgenerategausslegendre(ae_int_t n,
     ae_int_t* info,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* w,
     ae_state *_state);


/*************************************************************************
Returns  nodes/weights  for  Gauss-Jacobi quadrature on [-1,1] with weight
function W(x)=Power(1-x,Alpha)*Power(1+x,Beta).

INPUT PARAMETERS:
    N           -   number of nodes, >=1
    Alpha       -   power-law coefficient, Alpha>-1
    Beta        -   power-law coefficient, Beta>-1

OUTPUT PARAMETERS:
    Info        -   error code:
                    * -4    an  error  was   detected   when   calculating
                            weights/nodes. Alpha or  Beta  are  too  close
                            to -1 to obtain weights/nodes with high enough
                            accuracy, or, may be, N is too large.  Try  to
                            use multiple precision version.
                    * -3    internal eigenproblem solver hasn't converged
                    * -1    incorrect N/Alpha/Beta was passed
                    * +1    OK
    X           -   array[0..N-1] - array of quadrature nodes,
                    in ascending order.
    W           -   array[0..N-1] - array of quadrature weights.


  -- ALGLIB --
     Copyright 12.05.2009 by Bochkanov Sergey
*************************************************************************/
void gqgenerategaussjacobi(ae_int_t n,
     double alpha,
     double beta,
     ae_int_t* info,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* w,
     ae_state *_state);


/*************************************************************************
Returns  nodes/weights  for  Gauss-Laguerre  quadrature  on  [0,+inf) with
weight function W(x)=Power(x,Alpha)*Exp(-x)

INPUT PARAMETERS:
    N           -   number of nodes, >=1
    Alpha       -   power-law coefficient, Alpha>-1

OUTPUT PARAMETERS:
    Info        -   error code:
                    * -4    an  error  was   detected   when   calculating
                            weights/nodes. Alpha is too  close  to  -1  to
                            obtain weights/nodes with high enough accuracy
                            or, may  be,  N  is  too  large.  Try  to  use
                            multiple precision version.
                    * -3    internal eigenproblem solver hasn't converged
                    * -1    incorrect N/Alpha was passed
                    * +1    OK
    X           -   array[0..N-1] - array of quadrature nodes,
                    in ascending order.
    W           -   array[0..N-1] - array of quadrature weights.


  -- ALGLIB --
     Copyright 12.05.2009 by Bochkanov Sergey
*************************************************************************/
void gqgenerategausslaguerre(ae_int_t n,
     double alpha,
     ae_int_t* info,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* w,
     ae_state *_state);


/*************************************************************************
Returns  nodes/weights  for  Gauss-Hermite  quadrature on (-inf,+inf) with
weight function W(x)=Exp(-x*x)

INPUT PARAMETERS:
    N           -   number of nodes, >=1

OUTPUT PARAMETERS:
    Info        -   error code:
                    * -4    an  error  was   detected   when   calculating
                            weights/nodes.  May be, N is too large. Try to
                            use multiple precision version.
                    * -3    internal eigenproblem solver hasn't converged
                    * -1    incorrect N/Alpha was passed
                    * +1    OK
    X           -   array[0..N-1] - array of quadrature nodes,
                    in ascending order.
    W           -   array[0..N-1] - array of quadrature weights.


  -- ALGLIB --
     Copyright 12.05.2009 by Bochkanov Sergey
*************************************************************************/
void gqgenerategausshermite(ae_int_t n,
     ae_int_t* info,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* w,
     ae_state *_state);


/*$ End $*/
#endif

