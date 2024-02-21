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

#ifndef _iterativesparse_h
#define _iterativesparse_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "scodes.h"
#include "ablasmkl.h"
#include "ablasf.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "tsort.h"
#include "sparse.h"
#include "ablas.h"
#include "rotations.h"
#include "hblas.h"
#include "creflections.h"
#include "sblas.h"
#include "ortfac.h"
#include "fbls.h"
#include "matgen.h"


/*$ Declarations $*/


/*************************************************************************
This structure is a sparse solver report (both direct and iterative solvers
use this structure).

Following fields can be accessed by users:
* TerminationType (specific error codes depend on the solver  being  used,
  with positive values ALWAYS signaling  that something useful is returned
  in X, and negative values ALWAYS meaning critical failures.
* NMV - number of matrix-vector products performed (0 for direct solvers)
* IterationsCount - inner iterations count (0 for direct solvers)
* R2 - squared residual
*************************************************************************/
typedef struct
{
    ae_int_t terminationtype;
    ae_int_t nmv;
    ae_int_t iterationscount;
    double r2;
} sparsesolverreport;


/*************************************************************************
This object stores state of the sparse linear solver object.

You should use ALGLIB functions to work with this object.
Never try to access its fields directly!
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_vector x0;
    double epsf;
    ae_int_t maxits;
    ae_int_t algotype;
    ae_int_t gmresk;
    ae_bool xrep;
    ae_bool running;
    ae_bool userterminationneeded;
    ae_vector b;
    ae_vector xf;
    ae_int_t repiterationscount;
    ae_int_t repnmv;
    ae_int_t repterminationtype;
    double repr2;
    ae_int_t requesttype;
    ae_vector x;
    ae_vector ax;
    double reply1;
    ae_vector wrkb;
    sparsematrix convbuf;
    fblsgmresstate gmressolver;
    rcommstate rstate;
} sparsesolverstate;


/*$ Body $*/


/*************************************************************************
Solving sparse symmetric linear system A*x=b using GMRES(k) method. Sparse
symmetric A is given by its lower or upper triangle.

NOTE: use SparseSolveGMRES() to solve system with nonsymmetric A.

This function provides convenience API for an 'expert' interface  provided
by SparseSolverState class. Use SparseSolver  API  if  you  need  advanced
functions like providing initial point, using out-of-core API and so on.

INPUT PARAMETERS:
    A       -   sparse symmetric NxN matrix in any sparse storage  format.
                Using CRS format is recommended because it avoids internal
                conversion.
                An exception will be generated if  A  is  not  NxN  matrix
                (where  N  is  a  size   specified  during  solver  object
                creation).
    IsUpper -   whether upper or lower triangle of A is used:
                * IsUpper=True  => only upper triangle is used and lower
                                   triangle is not referenced at all 
                * IsUpper=False => only lower triangle is used and upper
                                   triangle is not referenced at all
    B       -   right part, array[N]
    K       -   k parameter for  GMRES(k), k>=0.  Zero  value  means  that
                algorithm will choose it automatically.
    EpsF    -   stopping condition, EpsF>=0. The algorithm will stop  when
                residual will decrease below EpsF*|B|. Having EpsF=0 means
                that this stopping condition is ignored.
    MaxIts  -   stopping condition, MaxIts>=0.  The  algorithm  will  stop
                after performing MaxIts iterations. Zero  value  means  no
                limit.

NOTE: having both EpsF=0 and MaxIts=0 means that stopping criteria will be
      chosen automatically.
                
OUTPUT PARAMETERS:
    X       -   array[N], the solution
    Rep     -   solution report:
                * Rep.TerminationType completion code:
                    * -5    CG method was used for a matrix which  is  not
                            positive definite
                    * -4    overflow/underflow during solution
                            (ill conditioned problem)
                    *  1    ||residual||<=EpsF*||b||
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            best point found is returned
                    *  8    the  algorithm  was  terminated   early  with
                            SparseSolverRequestTermination() being called
                            from other thread.
                * Rep.IterationsCount contains iterations count
                * Rep.NMV contains number of matrix-vector calculations
                * Rep.R2 contains squared residual

  -- ALGLIB --
     Copyright 25.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolvesymmetricgmres(const sparsematrix* a,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     ae_int_t k,
     double epsf,
     ae_int_t maxits,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state);


/*************************************************************************
Solving sparse linear system A*x=b using GMRES(k) method.

This function provides convenience API for an 'expert' interface  provided
by SparseSolverState class. Use SparseSolver  API  if  you  need  advanced
functions like providing initial point, using out-of-core API and so on.

INPUT PARAMETERS:
    A       -   sparse NxN matrix in any sparse storage format. Using  CRS
                format   is   recommended   because   it  avoids  internal
                conversion.
                An exception will be generated if  A  is  not  NxN  matrix
                (where  N  is  a  size   specified  during  solver  object
                creation).
    B       -   right part, array[N]
    K       -   k parameter for  GMRES(k), k>=0.  Zero  value  means  that
                algorithm will choose it automatically.
    EpsF    -   stopping condition, EpsF>=0. The algorithm will stop  when
                residual will decrease below EpsF*|B|. Having EpsF=0 means
                that this stopping condition is ignored.
    MaxIts  -   stopping condition, MaxIts>=0.  The  algorithm  will  stop
                after performing MaxIts iterations. Zero  value  means  no
                limit.

NOTE: having both EpsF=0 and MaxIts=0 means that stopping criteria will be
      chosen automatically.
                
OUTPUT PARAMETERS:
    X       -   array[N], the solution
    Rep     -   solution report:
                * Rep.TerminationType completion code:
                    * -5    CG method was used for a matrix which  is  not
                            positive definite
                    * -4    overflow/underflow during solution
                            (ill conditioned problem)
                    *  1    ||residual||<=EpsF*||b||
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            best point found is returned
                    *  8    the  algorithm  was  terminated   early  with
                            SparseSolverRequestTermination() being called
                            from other thread.
                * Rep.IterationsCount contains iterations count
                * Rep.NMV contains number of matrix-vector calculations
                * Rep.R2 contains squared residual

  -- ALGLIB --
     Copyright 25.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolvegmres(const sparsematrix* a,
     /* Real    */ const ae_vector* b,
     ae_int_t k,
     double epsf,
     ae_int_t maxits,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state);


/*************************************************************************
This function initializes sparse linear iterative solver object.

This solver can be used  to  solve  nonsymmetric  and  symmetric  positive
definite NxN (square) linear systems.

The solver provides  'expert'  API  which  allows  advanced  control  over
algorithms being used, including ability to get progress report, terminate
long-running solver from other thread, out-of-core solution and so on.

NOTE: there are also convenience  functions  that  allows  quick  one-line
      access to the solvers:
      * SparseSolveCG() to solve SPD linear systems
      * SparseSolveGMRES() to solve unsymmetric linear systems.

NOTE: if you want to solve MxN (rectangular) linear problem  you  may  use
      LinLSQR solver provided by ALGLIB.

USAGE (A is given by the SparseMatrix structure):

    1. User initializes algorithm state with SparseSolverCreate() call
    2. User  selects   algorithm  with one of the SparseSolverSetAlgo???()
       functions. By default, GMRES(k) is used with automatically chosen k
    3. Optionally, user tunes solver parameters, sets starting point, etc.
    4. Depending on whether system is symmetric or not, user calls:
       * SparseSolverSolveSymmetric() for a  symmetric system given by its
         lower or upper triangle
       * SparseSolverSolve() for a nonsymmetric system or a symmetric  one
         given by the full matrix
    5. User calls SparseSolverResults() to get the solution

    It is possible to call SparseSolverSolve???() again to  solve  another
    task with same dimensionality but different matrix and/or  right  part
    without reinitializing SparseSolverState structure.
  
USAGE (out-of-core mode):

    1. User initializes algorithm state with SparseSolverCreate() call
    2. User  selects   algorithm  with one of the SparseSolverSetAlgo???()
       functions. By default, GMRES(k) is used with automatically chosen k
    3. Optionally, user tunes solver parameters, sets starting point, etc.
    4. After that user should work with out-of-core interface  in  a  loop
       like one given below:
       
        > alglib.sparsesolveroocstart(state)
        > while alglib.sparsesolverooccontinue(state) do
        >     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
        >     alglib.sparsesolveroocgetrequestdata(state, out X)
        >     if RequestType=0 then
        >         [calculate  Y=A*X, with X=R^N]
        >     alglib.sparsesolveroocsendresult(state, in Y)
        > alglib.sparsesolveroocstop(state, out X, out Report)

INPUT PARAMETERS:
    N       -   problem dimensionality (fixed at start-up)

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolvercreate(ae_int_t n,
     sparsesolverstate* state,
     ae_state *_state);


/*************************************************************************
This function sets the solver algorithm to GMRES(k).

NOTE: if you do not need advanced functionality of the  SparseSolver  API,
      you   may   use   convenience   functions   SparseSolveGMRES()   and
      SparseSolveSymmetricGMRES().

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    K       -   GMRES parameter, K>=0:
                * recommended values are in 10..100 range
                * larger values up to N are possible but have little sense
                  - the algorithm will be slower than any dense solver.
                * values above N are truncated down to N
                * zero value means that  default  value  is  chosen.  This
                  value is 50 in the current version, but  it  may  change
                  in future ALGLIB releases.

  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolversetalgogmres(sparsesolverstate* state,
     ae_int_t k,
     ae_state *_state);


/*************************************************************************
This function sets starting point.
By default, zero starting point is used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    X       -   starting point, array[N]

OUTPUT PARAMETERS:
    State   -   new starting point was set

  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolversetstartingpoint(sparsesolverstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);


/*************************************************************************
This function sets stopping criteria.

INPUT PARAMETERS:
    EpsF    -   algorithm will be stopped if norm of residual is less than 
                EpsF*||b||.
    MaxIts  -   algorithm will be stopped if number of iterations is  more 
                than MaxIts.

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
If  both  EpsF  and  MaxIts  are  zero then small EpsF will be set to small 
value.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void sparsesolversetcond(sparsesolverstate* state,
     double epsf,
     ae_int_t maxits,
     ae_state *_state);


/*************************************************************************
Procedure for  the  solution of A*x=b with sparse symmetric A given by its
lower or upper triangle.

This function will work with any solver algorithm  being   used,  SPD  one
(like CG) or not (like GMRES). Using unsymmetric solvers (like  GMRES)  on
SPD problems is suboptimal, but still possible.

NOTE: the  solver  behavior is ill-defined  for  a  situation  when a  SPD
      solver is used on indefinite matrix. It  may solve the problem up to
      desired precision (sometimes, rarely)  or  return  with  error  code
      signalling violation of underlying assumptions.

INPUT PARAMETERS:
    State   -   algorithm state
    A       -   sparse symmetric NxN matrix in any sparse storage  format.
                Using CRS format is recommended because it avoids internal
                conversion.
                An exception will be generated if  A  is  not  NxN  matrix
                (where  N  is  a  size   specified  during  solver  object
                creation).
    IsUpper -   whether upper or lower triangle of A is used:
                * IsUpper=True  => only upper triangle is used and lower
                                   triangle is not referenced at all 
                * IsUpper=False => only lower triangle is used and upper
                                   triangle is not referenced at all
    B       -   right part, array[N]

RESULT:
    This function returns no result.
    You can get the solution by calling SparseSolverResults()

  -- ALGLIB --
     Copyright 25.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolversolvesymmetric(sparsesolverstate* state,
     const sparsematrix* a,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     ae_state *_state);


/*************************************************************************
Procedure for the solution of A*x=b with sparse nonsymmetric A

IMPORTANT: this function will work with any solver algorithm  being  used,
           symmetric solver like CG,  or  not.  However,  using  symmetric
           solvers on nonsymmetric problems is  dangerous.  It  may  solve
           the problem up  to  desired  precision  (sometimes,  rarely) or
           terminate with error code signalling  violation  of  underlying
           assumptions.

INPUT PARAMETERS:
    State   -   algorithm state
    A       -   sparse NxN matrix in any sparse storage  format.
                Using CRS format is recommended because it avoids internal
                conversion.
                An exception will be generated if  A  is  not  NxN  matrix
                (where  N  is  a  size   specified  during  solver  object
                creation).
    B       -   right part, array[N]

RESULT:
    This function returns no result.
    You can get the solution by calling SparseSolverResults()

  -- ALGLIB --
     Copyright 25.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolversolve(sparsesolverstate* state,
     const sparsematrix* a,
     /* Real    */ const ae_vector* b,
     ae_state *_state);


/*************************************************************************
Sparse solver results.

This function must be called after calling one of the SparseSolverSolve()
functions.

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[N], solution
    Rep     -   solution report:
                * Rep.TerminationType completion code:
                    * -5    CG method was used for a matrix which  is  not
                            positive definite
                    * -4    overflow/underflow during solution
                            (ill conditioned problem)
                    *  1    ||residual||<=EpsF*||b||
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            best point found is returned
                    *  8    the  algorithm  was  terminated   early  with
                            SparseSolverRequestTermination() being called
                            from other thread.
                * Rep.IterationsCount contains iterations count
                * Rep.NMV contains number of matrix-vector calculations
                * Rep.R2 contains squared residual
s
  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void sparsesolverresults(sparsesolverstate* state,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state);


/*************************************************************************
This function turns on/off reporting during out-of-core processing.

When the solver works in the out-of-core mode, it  can  be  configured  to
report its progress by returning current location. These location  reports
are implemented as a special kind of the out-of-core request:
* SparseSolverOOCGetRequestInfo() returns -1
* SparseSolverOOCGetRequestData() returns current location
* SparseSolverOOCGetRequestData1() returns squared norm of the residual
* SparseSolverOOCSendResult() shall NOT be called

This function has no effect when SparseSolverSolve() is used because  this
function has no method of reporting its progress.

NOTE: when used with GMRES(k), this function reports progress  every  k-th
      iteration.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

  -- ALGLIB --
     Copyright 01.10.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolversetxrep(sparsesolverstate* state,
     ae_bool needxrep,
     ae_state *_state);


/*************************************************************************
This function initiates out-of-core mode of the sparse solver.  It  should
be used in conjunction with other out-of-core-related  functions  of  this
subspackage in a loop like one given below:

> alglib.sparsesolveroocstart(state)
> while alglib.sparsesolverooccontinue(state) do
>     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
>     alglib.sparsesolveroocgetrequestdata(state, out X)
>     if RequestType=0 then
>         [calculate  Y=A*X, with X=R^N]
>     alglib.sparsesolveroocsendresult(state, in Y)
> alglib.sparsesolveroocstop(state, out X, out Report)

INPUT PARAMETERS:
    State       -   solver object

  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolveroocstart(sparsesolverstate* state,
     /* Real    */ const ae_vector* b,
     ae_state *_state);


/*************************************************************************
This function performs iterative solution of  the  linear  system  in  the
out-of-core mode. It should be used in conjunction with other out-of-core-
related functions of this subspackage in a loop like one given below:

> alglib.sparsesolveroocstart(state)
> while alglib.sparsesolverooccontinue(state) do
>     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
>     alglib.sparsesolveroocgetrequestdata(state, out X)
>     if RequestType=0 then
>         [calculate  Y=A*X, with X=R^N]
>     alglib.sparsesolveroocsendresult(state, in Y)
> alglib.sparsesolveroocstop(state, out X, out Report)

  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
ae_bool sparsesolverooccontinue(sparsesolverstate* state,
     ae_state *_state);


/*************************************************************************
This function is used to retrieve information  about  out-of-core  request
sent by the solver:
* RequestType=0  means that matrix-vector products A*x is requested
* RequestType=-1 means that solver reports its progress; this  request  is
  returned only when reports are activated wit SparseSolverSetXRep().

This function returns just request type; in order  to  get contents of the
trial vector, use sparsesolveroocgetrequestdata().

It should be used in conjunction with other out-of-core-related  functions
of this subspackage in a loop like one given below:

> alglib.sparsesolveroocstart(state)
> while alglib.sparsesolverooccontinue(state) do
>     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
>     alglib.sparsesolveroocgetrequestdata(state, out X)
>     if RequestType=0 then
>         [calculate  Y=A*X, with X=R^N]
>     alglib.sparsesolveroocsendresult(state, in Y)
> alglib.sparsesolveroocstop(state, out X, out Report)

INPUT PARAMETERS:
    State           -   solver running in out-of-core mode
    
OUTPUT PARAMETERS:
    RequestType     -   type of the request to process:
                        * 0   for matrix-vector product A*x, with A  being
                          NxN system matrix  and X being N-dimensional
                          vector
                        *-1   for location and residual report


  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolveroocgetrequestinfo(sparsesolverstate* state,
     ae_int_t* requesttype,
     ae_state *_state);


/*************************************************************************
This function is used  to  retrieve  vector  associated  with  out-of-core
request sent by the solver to user code. Depending  on  the  request  type
(returned by the SparseSolverOOCGetRequestInfo()) this  vector  should  be
multiplied by A or subjected to another processing.

It should be used in conjunction with other out-of-core-related  functions
of this subspackage in a loop like one given below:

> alglib.sparsesolveroocstart(state)
> while alglib.sparsesolverooccontinue(state) do
>     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
>     alglib.sparsesolveroocgetrequestdata(state, out X)
>     if RequestType=0 then
>         [calculate  Y=A*X, with X=R^N]
>     alglib.sparsesolveroocsendresult(state, in Y)
> alglib.sparsesolveroocstop(state, out X, out Report)

INPUT PARAMETERS:
    State           -   solver running in out-of-core mode
    X               -   possibly  preallocated   storage;  reallocated  if
                        needed, left unchanged, if large enough  to  store
                        request data.
    
OUTPUT PARAMETERS:
    X               -   array[N] or larger, leading N elements are  filled
                        with vector X.


  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolveroocgetrequestdata(sparsesolverstate* state,
     /* Real    */ ae_vector* x,
     ae_state *_state);


/*************************************************************************
This function is used to retrieve scalar value associated with out-of-core
request sent by the solver to user code. In  the  current  ALGLIB  version
this function is used to retrieve squared residual  norm  during  progress
reports.

INPUT PARAMETERS:
    State           -   solver running in out-of-core mode
    
OUTPUT PARAMETERS:
    V               -   scalar value associated with the current request


  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolveroocgetrequestdata1(sparsesolverstate* state,
     double* v,
     ae_state *_state);


/*************************************************************************
This function is used to send user reply to out-of-core  request  sent  by
the solver. Usually it is product A*x for vector X returned by the solver.

It should be used in conjunction with other out-of-core-related  functions
of this subspackage in a loop like one given below:

> alglib.sparsesolveroocstart(state)
> while alglib.sparsesolverooccontinue(state) do
>     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
>     alglib.sparsesolveroocgetrequestdata(state, out X)
>     if RequestType=0 then
>         [calculate  Y=A*X, with X=R^N]
>     alglib.sparsesolveroocsendresult(state, in Y)
> alglib.sparsesolveroocstop(state, out X, out Report)

INPUT PARAMETERS:
    State           -   solver running in out-of-core mode
    AX              -   array[N] or larger, leading N elements contain A*x


  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolveroocsendresult(sparsesolverstate* state,
     /* Real    */ const ae_vector* ax,
     ae_state *_state);


/*************************************************************************
This  function  finalizes out-of-core mode of the linear solver. It should
be used in conjunction with other out-of-core-related  functions  of  this
subspackage in a loop like one given below:

> alglib.sparsesolveroocstart(state)
> while alglib.sparsesolverooccontinue(state) do
>     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
>     alglib.sparsesolveroocgetrequestdata(state, out X)
>     if RequestType=0 then
>         [calculate  Y=A*X, with X=R^N]
>     alglib.sparsesolveroocsendresult(state, in Y)
> alglib.sparsesolveroocstop(state, out X, out Report)

INPUT PARAMETERS:
    State       -   solver state
    
OUTPUT PARAMETERS:
    X       -   array[N], the solution.
                Zero-filled on the failure (Rep.TerminationType<0).
    Rep     -   report with additional info:
                * Rep.TerminationType completion code:
                    * -5    CG method was used for a matrix which  is  not
                            positive definite
                    * -4    overflow/underflow during solution
                            (ill conditioned problem)
                    *  1    ||residual||<=EpsF*||b||
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            best point found is returned
                    *  8    the  algorithm  was  terminated   early  with
                            SparseSolverRequestTermination() being called
                            from other thread.
                * Rep.IterationsCount contains iterations count
                * Rep.NMV contains number of matrix-vector calculations
                * Rep.R2 contains squared residual

  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolveroocstop(sparsesolverstate* state,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state);


/*************************************************************************
This subroutine submits request for termination of the running solver.  It
can be called from some other thread which wants the   solver to terminate
or when processing an out-of-core request.

As result, solver  stops  at  point  which  was  "current  accepted"  when
the termination request was submitted and returns error code 8 (successful
termination).  Such   termination   is  a smooth  process  which  properly
deallocates all temporaries.

INPUT PARAMETERS:
    State   -   solver structure

NOTE: calling this function on solver which is NOT running  will  have  no
      effect.
      
NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

NOTE: solver clears termination flag on its start, it means that  if  some
      other thread will request termination too soon, its request will went
      unnoticed.

  -- ALGLIB --
     Copyright 01.10.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolverrequesttermination(sparsesolverstate* state,
     ae_state *_state);


/*************************************************************************
Reset report fields

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void initsparsesolverreport(sparsesolverreport* rep, ae_state *_state);
void _sparsesolverreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sparsesolverreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sparsesolverreport_clear(void* _p);
void _sparsesolverreport_destroy(void* _p);
void _sparsesolverstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sparsesolverstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sparsesolverstate_clear(void* _p);
void _sparsesolverstate_destroy(void* _p);


/*$ End $*/
#endif

