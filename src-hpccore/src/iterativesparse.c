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


#include <stdafx.h>
#include "iterativesparse.h"


/*$ Declarations $*/
static ae_bool iterativesparse_sparsesolveriteration(sparsesolverstate* state,
     ae_state *_state);
static void iterativesparse_clearrequestfields(sparsesolverstate* state,
     ae_state *_state);
static void iterativesparse_clearreportfields(sparsesolverstate* state,
     ae_state *_state);


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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t n;
    sparsematrix convbuf;
    sparsesolverstate solver;

    ae_frame_make(_state, &_frame_block);
    memset(&convbuf, 0, sizeof(convbuf));
    memset(&solver, 0, sizeof(solver));
    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);
    _sparsematrix_init(&convbuf, _state, ae_true);
    _sparsesolverstate_init(&solver, _state, ae_true);

    n = sparsegetnrows(a, _state);
    
    /*
     * Test inputs
     */
    ae_assert(n>=1, "SparseSolveSymmetricGMRES: tried to automatically detect N from sizeof(A), got nonpositive size", _state);
    ae_assert(sparsegetnrows(a, _state)==n, "SparseSolveSymmetricGMRES: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseSolveSymmetricGMRES: cols(A)!=N", _state);
    ae_assert(b->cnt>=n, "SparseSolveSymmetricGMRES: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseSolveSymmetricGMRES: B contains NAN/INF", _state);
    ae_assert(ae_isfinite(epsf, _state)&&ae_fp_greater_eq(epsf,(double)(0)), "SparseSolveSymmetricGMRES: EpsF<0 or infinite", _state);
    ae_assert(maxits>=0, "SparseSolveSymmetricGMRES: MaxIts<0", _state);
    if( ae_fp_eq(epsf,(double)(0))&&maxits==0 )
    {
        epsf = 1.0E-6;
    }
    
    /*
     * If A is non-CRS, perform conversion
     */
    if( !sparseiscrs(a, _state) )
    {
        sparsecopytocrsbuf(a, &convbuf, _state);
        sparsesolvesymmetricgmres(&convbuf, isupper, b, k, epsf, maxits, x, rep, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Solve using temporary solver object
     */
    sparsesolvercreate(n, &solver, _state);
    sparsesolversetalgogmres(&solver, k, _state);
    sparsesolversetcond(&solver, epsf, maxits, _state);
    sparsesolversolvesymmetric(&solver, a, isupper, b, _state);
    sparsesolverresults(&solver, x, rep, _state);
    ae_frame_leave(_state);
}


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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t n;
    sparsematrix convbuf;
    sparsesolverstate solver;

    ae_frame_make(_state, &_frame_block);
    memset(&convbuf, 0, sizeof(convbuf));
    memset(&solver, 0, sizeof(solver));
    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);
    _sparsematrix_init(&convbuf, _state, ae_true);
    _sparsesolverstate_init(&solver, _state, ae_true);

    n = sparsegetnrows(a, _state);
    
    /*
     * Test inputs
     */
    ae_assert(n>=1, "SparseSolveGMRES: tried to automatically detect N from sizeof(A), got nonpositive size", _state);
    ae_assert(sparsegetnrows(a, _state)==n, "SparseSolveGMRES: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseSolveGMRES: cols(A)!=N", _state);
    ae_assert(b->cnt>=n, "SparseSolveGMRES: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseSolveGMRES: B contains NAN/INF", _state);
    ae_assert(ae_isfinite(epsf, _state)&&ae_fp_greater_eq(epsf,(double)(0)), "SparseSolveGMRES: EpsF<0 or infinite", _state);
    ae_assert(maxits>=0, "SparseSolveGMRES: MaxIts<0", _state);
    if( ae_fp_eq(epsf,(double)(0))&&maxits==0 )
    {
        epsf = 1.0E-6;
    }
    
    /*
     * If A is non-CRS, perform conversion
     */
    if( !sparseiscrs(a, _state) )
    {
        sparsecopytocrsbuf(a, &convbuf, _state);
        sparsesolvegmres(&convbuf, b, k, epsf, maxits, x, rep, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Solve using temporary solver object
     */
    sparsesolvercreate(n, &solver, _state);
    sparsesolversetalgogmres(&solver, k, _state);
    sparsesolversetcond(&solver, epsf, maxits, _state);
    sparsesolversolve(&solver, a, b, _state);
    sparsesolverresults(&solver, x, rep, _state);
    ae_frame_leave(_state);
}


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
     ae_state *_state)
{

    _sparsesolverstate_clear(state);

    ae_assert(n>=1, "SparseSolverCreate: N<=0", _state);
    state->n = n;
    state->running = ae_false;
    state->userterminationneeded = ae_false;
    rsetallocv(state->n, 0.0, &state->x0, _state);
    rsetallocv(state->n, 0.0, &state->x, _state);
    rsetallocv(state->n, 0.0, &state->ax, _state);
    rsetallocv(state->n, 0.0, &state->xf, _state);
    rsetallocv(state->n, 0.0, &state->b, _state);
    rsetallocv(state->n, 0.0, &state->wrkb, _state);
    state->reply1 = 0.0;
    sparsesolversetxrep(state, ae_false, _state);
    sparsesolversetcond(state, 0.0, 0, _state);
    sparsesolversetalgogmres(state, 0, _state);
    iterativesparse_clearrequestfields(state, _state);
    iterativesparse_clearreportfields(state, _state);
}


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
     ae_state *_state)
{


    ae_assert(k>=0, "SparseSolverSetAlgoGMRESK: K<0", _state);
    state->algotype = 0;
    if( k==0 )
    {
        k = 50;
    }
    state->gmresk = ae_minint(k, state->n, _state);
}


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
     ae_state *_state)
{


    ae_assert(state->n<=x->cnt, "SparseSolverSetStartingPoint: Length(X)<N", _state);
    ae_assert(isfinitevector(x, state->n, _state), "SparseSolverSetStartingPoint: X contains infinite or NaN values!", _state);
    rcopyv(state->n, x, &state->x0, _state);
}


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
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsf, _state)&&ae_fp_greater_eq(epsf,(double)(0)), "SparseSolverSetCond: EpsF is negative or contains infinite or NaN values", _state);
    ae_assert(maxits>=0, "SparseSolverSetCond: MaxIts is negative", _state);
    if( ae_fp_eq(epsf,(double)(0))&&maxits==0 )
    {
        state->epsf = 1.0E-6;
        state->maxits = 0;
    }
    else
    {
        state->epsf = epsf;
        state->maxits = maxits;
    }
}


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
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    
    /*
     * Test inputs
     */
    ae_assert(sparsegetnrows(a, _state)==n, "SparseSolverSolveSymmetric: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseSolverSolveSymmetric: cols(A)!=N", _state);
    ae_assert(b->cnt>=n, "SparseSolverSolveSymmetric: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseSolverSolveSymmetric: B contains NAN/INF", _state);
    
    /*
     * If A is non-CRS, perform conversion
     */
    if( !sparseiscrs(a, _state) )
    {
        sparsecopytocrsbuf(a, &state->convbuf, _state);
        sparsesolversolvesymmetric(state, &state->convbuf, isupper, b, _state);
        return;
    }
    
    /*
     * Solve using out-of-core API
     */
    sparsesolveroocstart(state, b, _state);
    while(sparsesolverooccontinue(state, _state))
    {
        if( state->requesttype==-1 )
        {
            
            /*
             * Skip location reports
             */
            continue;
        }
        ae_assert(state->requesttype==0, "SparseSolverSolveSymmetric: integrity check 7372 failed", _state);
        sparsesmv(a, isupper, &state->x, &state->ax, _state);
    }
}


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
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    
    /*
     * Test inputs
     */
    ae_assert(sparsegetnrows(a, _state)==n, "SparseSolverSolve: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseSolverSolve: cols(A)!=N", _state);
    ae_assert(b->cnt>=n, "SparseSolverSolve: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseSolverSolve: B contains NAN/INF", _state);
    
    /*
     * If A is non-CRS, perform conversion
     */
    if( !sparseiscrs(a, _state) )
    {
        sparsecopytocrsbuf(a, &state->convbuf, _state);
        sparsesolversolve(state, &state->convbuf, b, _state);
        return;
    }
    
    /*
     * Solve using out-of-core API
     */
    sparsesolveroocstart(state, b, _state);
    while(sparsesolverooccontinue(state, _state))
    {
        if( state->requesttype==-1 )
        {
            
            /*
             * Skip location reports
             */
            continue;
        }
        ae_assert(state->requesttype==0, "SparseSolverSolve: integrity check 7372 failed", _state);
        sparsemv(a, &state->x, &state->ax, _state);
    }
}


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
     ae_state *_state)
{

    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);

    sparsesolveroocstop(state, x, rep, _state);
}


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
     ae_state *_state)
{


    state->xrep = needxrep;
}


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
     ae_state *_state)
{


    ae_vector_set_length(&state->rstate.ia, 0+1, _state);
    ae_vector_set_length(&state->rstate.ra, 2+1, _state);
    state->rstate.stage = -1;
    iterativesparse_clearrequestfields(state, _state);
    iterativesparse_clearreportfields(state, _state);
    state->running = ae_true;
    state->userterminationneeded = ae_false;
    rcopyv(state->n, b, &state->b, _state);
}


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
     ae_state *_state)
{
    ae_bool result;


    ae_assert(state->running, "SparseSolverContinue: the solver is not running", _state);
    result = iterativesparse_sparsesolveriteration(state, _state);
    state->running = result;
    return result;
}


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
     ae_state *_state)
{

    *requesttype = 0;

    ae_assert(state->running, "SparseSolverOOCGetRequestInfo: the solver is not running", _state);
    *requesttype = state->requesttype;
}


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
     ae_state *_state)
{


    ae_assert(state->running, "SparseSolverOOCGetRequestInfo: the solver is not running", _state);
    rcopyallocv(state->n, &state->x, x, _state);
}


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
     ae_state *_state)
{

    *v = 0.0;

    ae_assert(state->running, "SparseSolverOOCGetRequestInfo: the solver is not running", _state);
    *v = state->reply1;
}


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
     ae_state *_state)
{


    ae_assert(state->running, "SparseSolverOOCSendResult: the solver is not running", _state);
    ae_assert(state->requesttype==0, "SparseSolverOOCSendResult: this request type does not accept replies", _state);
    rcopyv(state->n, ax, &state->ax, _state);
}


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
     ae_state *_state)
{

    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);

    ae_assert(!state->running, "SparseSolverOOCStop: the solver is still running", _state);
    ae_vector_set_length(x, state->n, _state);
    rcopyv(state->n, &state->xf, x, _state);
    initsparsesolverreport(rep, _state);
    rep->iterationscount = state->repiterationscount;
    rep->nmv = state->repnmv;
    rep->terminationtype = state->repterminationtype;
    rep->r2 = state->repr2;
}


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
     ae_state *_state)
{


    state->userterminationneeded = ae_true;
}


/*************************************************************************
Reset report fields

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void initsparsesolverreport(sparsesolverreport* rep, ae_state *_state)
{


    rep->terminationtype = 0;
    rep->nmv = 0;
    rep->iterationscount = 0;
    rep->r2 = (double)(0);
}


/*************************************************************************
Reverse communication sparse iteration subroutine

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
static ae_bool iterativesparse_sparsesolveriteration(sparsesolverstate* state,
     ae_state *_state)
{
    ae_int_t outeridx;
    double res;
    double prevres;
    double res0;
    ae_bool result;


    
    /*
     * Reverse communication preparations
     * I know it looks ugly, but it works the same way
     * anywhere from C++ to Python.
     *
     * This code initializes locals by:
     * * random values determined during code
     *   generation - on first subroutine call
     * * values from previous call - on subsequent calls
     */
    if( state->rstate.stage>=0 )
    {
        outeridx = state->rstate.ia.ptr.p_int[0];
        res = state->rstate.ra.ptr.p_double[0];
        prevres = state->rstate.ra.ptr.p_double[1];
        res0 = state->rstate.ra.ptr.p_double[2];
    }
    else
    {
        outeridx = 359;
        res = -58.0;
        prevres = -919.0;
        res0 = -909.0;
    }
    if( state->rstate.stage==0 )
    {
        goto lbl_0;
    }
    if( state->rstate.stage==1 )
    {
        goto lbl_1;
    }
    if( state->rstate.stage==2 )
    {
        goto lbl_2;
    }
    if( state->rstate.stage==3 )
    {
        goto lbl_3;
    }
    if( state->rstate.stage==4 )
    {
        goto lbl_4;
    }
    
    /*
     * Routine body
     */
    state->running = ae_true;
    iterativesparse_clearrequestfields(state, _state);
    iterativesparse_clearreportfields(state, _state);
    
    /*
     * GMRES?
     */
    if( state->algotype!=0 )
    {
        goto lbl_5;
    }
    if( ae_fp_neq(rdotv2(state->n, &state->x0, _state),(double)(0)) )
    {
        goto lbl_7;
    }
    
    /*
     * Starting point is default one (zero), quick initialization
     */
    rsetv(state->n, 0.0, &state->xf, _state);
    rcopyv(state->n, &state->b, &state->wrkb, _state);
    goto lbl_8;
lbl_7:
    
    /*
     * Non-zero starting point is provided, 
     */
    rcopyv(state->n, &state->x0, &state->xf, _state);
    state->requesttype = 0;
    rcopyv(state->n, &state->x0, &state->x, _state);
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->requesttype = -999;
    state->repnmv = state->repnmv+1;
    rcopyv(state->n, &state->b, &state->wrkb, _state);
    raddv(state->n, -1.0, &state->ax, &state->wrkb, _state);
lbl_8:
    outeridx = 0;
    state->repterminationtype = 5;
    state->repr2 = rdotv2(state->n, &state->wrkb, _state);
    res0 = ae_sqrt(rdotv2(state->n, &state->b, _state), _state);
    res = ae_sqrt(state->repr2, _state);
    if( !state->xrep )
    {
        goto lbl_9;
    }
    
    /*
     * Report initial point
     */
    state->requesttype = -1;
    state->reply1 = res*res;
    rcopyv(state->n, &state->xf, &state->x, _state);
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->requesttype = -999;
lbl_9:
lbl_11:
    if( !(ae_fp_greater(res,(double)(0))&&(state->maxits==0||state->repiterationscount<state->maxits)) )
    {
        goto lbl_12;
    }
    
    /*
     * Solve with GMRES(k) for current residual.
     *
     * We set EpsF-based stopping condition for GMRES(k). It allows us
     * to quickly detect sufficient decrease in the residual. We still
     * have to recompute residual after the GMRES round because residuals
     * computed by GMRES are different from the true one (due to restarts).
     *
     * However, checking residual decrease within GMRES still gives us
     * an opportunity to stop early without waiting for GMRES round to
     * complete.
     */
    fblsgmrescreate(&state->wrkb, state->n, state->gmresk, &state->gmressolver, _state);
    state->gmressolver.epsres = state->epsf*res0/res;
lbl_13:
    if( !fblsgmresiteration(&state->gmressolver, _state) )
    {
        goto lbl_14;
    }
    state->requesttype = 0;
    rcopyv(state->n, &state->gmressolver.x, &state->x, _state);
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->requesttype = -999;
    rcopyv(state->n, &state->ax, &state->gmressolver.ax, _state);
    state->repnmv = state->repnmv+1;
    if( state->userterminationneeded )
    {
        
        /*
         * User requested termination
         */
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    goto lbl_13;
lbl_14:
    state->repiterationscount = state->repiterationscount+state->gmressolver.itsperformed;
    raddv(state->n, 1.0, &state->gmressolver.xs, &state->xf, _state);
    
    /*
     * Update residual, evaluate residual decrease, terminate if needed
     */
    state->requesttype = 0;
    rcopyv(state->n, &state->xf, &state->x, _state);
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->requesttype = -999;
    state->repnmv = state->repnmv+1;
    rcopyv(state->n, &state->b, &state->wrkb, _state);
    raddv(state->n, -1.0, &state->ax, &state->wrkb, _state);
    state->repr2 = rdotv2(state->n, &state->wrkb, _state);
    prevres = res;
    res = ae_sqrt(state->repr2, _state);
    if( !state->xrep )
    {
        goto lbl_15;
    }
    
    /*
     * Report initial point
     */
    state->requesttype = -1;
    state->reply1 = res*res;
    rcopyv(state->n, &state->xf, &state->x, _state);
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->requesttype = -999;
lbl_15:
    if( ae_fp_less_eq(res,state->epsf*res0) )
    {
        
        /*
         * Residual decrease condition met, stopping
         */
        state->repterminationtype = 1;
        goto lbl_12;
    }
    if( ae_fp_greater_eq(res,prevres*((double)1-ae_sqrt(ae_machineepsilon, _state))) )
    {
        
        /*
         * The algorithm stagnated
         */
        state->repterminationtype = 7;
        goto lbl_12;
    }
    if( state->userterminationneeded )
    {
        
        /*
         * User requested termination
         */
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    outeridx = outeridx+1;
    goto lbl_11;
lbl_12:
    result = ae_false;
    return result;
lbl_5:
    ae_assert(ae_false, "SparseSolverIteration: integrity check failed (unexpected algo)", _state);
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = outeridx;
    state->rstate.ra.ptr.p_double[0] = res;
    state->rstate.ra.ptr.p_double[1] = prevres;
    state->rstate.ra.ptr.p_double[2] = res0;
    return result;
}


/*************************************************************************
Clears request fileds (to be sure that we don't forgot to clear something)
*************************************************************************/
static void iterativesparse_clearrequestfields(sparsesolverstate* state,
     ae_state *_state)
{


    state->requesttype = -999;
}


/*************************************************************************
Clears report fileds (to be sure that we don't forgot to clear something)
*************************************************************************/
static void iterativesparse_clearreportfields(sparsesolverstate* state,
     ae_state *_state)
{


    state->repiterationscount = 0;
    state->repnmv = 0;
    state->repterminationtype = 0;
    state->repr2 = (double)(0);
}


void _sparsesolverreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    sparsesolverreport *p = (sparsesolverreport*)_p;
    ae_touch_ptr((void*)p);
}


void _sparsesolverreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    sparsesolverreport       *dst = (sparsesolverreport*)_dst;
    const sparsesolverreport *src = (const sparsesolverreport*)_src;
    dst->terminationtype = src->terminationtype;
    dst->nmv = src->nmv;
    dst->iterationscount = src->iterationscount;
    dst->r2 = src->r2;
}


void _sparsesolverreport_clear(void* _p)
{
    sparsesolverreport *p = (sparsesolverreport*)_p;
    ae_touch_ptr((void*)p);
}


void _sparsesolverreport_destroy(void* _p)
{
    sparsesolverreport *p = (sparsesolverreport*)_p;
    ae_touch_ptr((void*)p);
}


void _sparsesolverstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    sparsesolverstate *p = (sparsesolverstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->x0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->b, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xf, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->ax, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->wrkb, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->convbuf, _state, make_automatic);
    _fblsgmresstate_init(&p->gmressolver, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
}


void _sparsesolverstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    sparsesolverstate       *dst = (sparsesolverstate*)_dst;
    const sparsesolverstate *src = (const sparsesolverstate*)_src;
    dst->n = src->n;
    ae_vector_init_copy(&dst->x0, &src->x0, _state, make_automatic);
    dst->epsf = src->epsf;
    dst->maxits = src->maxits;
    dst->algotype = src->algotype;
    dst->gmresk = src->gmresk;
    dst->xrep = src->xrep;
    dst->running = src->running;
    dst->userterminationneeded = src->userterminationneeded;
    ae_vector_init_copy(&dst->b, &src->b, _state, make_automatic);
    ae_vector_init_copy(&dst->xf, &src->xf, _state, make_automatic);
    dst->repiterationscount = src->repiterationscount;
    dst->repnmv = src->repnmv;
    dst->repterminationtype = src->repterminationtype;
    dst->repr2 = src->repr2;
    dst->requesttype = src->requesttype;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->ax, &src->ax, _state, make_automatic);
    dst->reply1 = src->reply1;
    ae_vector_init_copy(&dst->wrkb, &src->wrkb, _state, make_automatic);
    _sparsematrix_init_copy(&dst->convbuf, &src->convbuf, _state, make_automatic);
    _fblsgmresstate_init_copy(&dst->gmressolver, &src->gmressolver, _state, make_automatic);
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
}


void _sparsesolverstate_clear(void* _p)
{
    sparsesolverstate *p = (sparsesolverstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x0);
    ae_vector_clear(&p->b);
    ae_vector_clear(&p->xf);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->ax);
    ae_vector_clear(&p->wrkb);
    _sparsematrix_clear(&p->convbuf);
    _fblsgmresstate_clear(&p->gmressolver);
    _rcommstate_clear(&p->rstate);
}


void _sparsesolverstate_destroy(void* _p)
{
    sparsesolverstate *p = (sparsesolverstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x0);
    ae_vector_destroy(&p->b);
    ae_vector_destroy(&p->xf);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->ax);
    ae_vector_destroy(&p->wrkb);
    _sparsematrix_destroy(&p->convbuf);
    _fblsgmresstate_destroy(&p->gmressolver);
    _rcommstate_destroy(&p->rstate);
}


/*$ End $*/
