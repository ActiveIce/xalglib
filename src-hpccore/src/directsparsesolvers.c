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
#include "directsparsesolvers.h"


/*$ Declarations $*/
static void directsparsesolvers_sparsesolveaug(const sparsematrix* a,
     /* Real    */ const ae_vector* b,
     double reg1f,
     double reg2f,
     double reg1r,
     double reg2r,
     ae_int_t gmresk,
     ae_int_t maxits,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
Sparse linear solver for A*x=b with N*N  sparse  real  symmetric  positive
definite matrix A, N*1 vectors x and b.

This solver  converts  input  matrix  to  SKS  format,  performs  Cholesky
factorization using  SKS  Cholesky  subroutine  (works  well  for  limited
bandwidth matrices) and uses sparse triangular solvers to get solution  of
the original system.

IMPORTANT: this  function  is  intended  for  low  profile (variable band)
           linear systems with dense or nearly-dense bands. Only  in  such
           cases  it  provides  some  performance  improvement  over  more
           general sparsrspdsolve(). If your  system  has  high  bandwidth
           or sparse band, the general sparsrspdsolve() is  likely  to  be
           more efficient.

INPUT PARAMETERS
    A       -   sparse matrix, must be NxN exactly
    IsUpper -   which half of A is provided (another half is ignored)
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate or non-SPD system).

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void sparsespdsolvesks(const sparsematrix* a,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    sparsematrix a2;
    ae_int_t n;

    ae_frame_make(_state, &_frame_block);
    memset(&a2, 0, sizeof(a2));
    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);
    _sparsematrix_init(&a2, _state, ae_true);

    n = sparsegetnrows(a, _state);
    ae_assert(n>0, "SparseSPDSolveSKS: N<=0", _state);
    ae_assert(sparsegetnrows(a, _state)==n, "SparseSPDSolveSKS: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseSPDSolveSKS: cols(A)!=N", _state);
    ae_assert(b->cnt>=n, "SparseSPDSolveSKS: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseSPDSolveSKS: B contains infinities or NANs", _state);
    initsparsesolverreport(rep, _state);
    ae_vector_set_length(x, n, _state);
    sparsecopytosks(a, &a2, _state);
    if( !sparsecholeskyskyline(&a2, n, isupper, _state) )
    {
        rep->terminationtype = -3;
        for(i=0; i<=n-1; i++)
        {
            x->ptr.p_double[i] = (double)(0);
        }
        ae_frame_leave(_state);
        return;
    }
    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = b->ptr.p_double[i];
    }
    if( isupper )
    {
        sparsetrsv(&a2, isupper, ae_false, 1, x, _state);
        sparsetrsv(&a2, isupper, ae_false, 0, x, _state);
    }
    else
    {
        sparsetrsv(&a2, isupper, ae_false, 0, x, _state);
        sparsetrsv(&a2, isupper, ae_false, 1, x, _state);
    }
    rep->terminationtype = 1;
    ae_frame_leave(_state);
}


/*************************************************************************
Sparse linear solver for A*x=b with N*N  sparse  real  symmetric  positive
definite matrix A, N*1 vectors x and b.

This solver  converts  input  matrix  to  CRS  format,  performs  Cholesky
factorization using supernodal Cholesky  decomposition  with  permutation-
reducing ordering and uses sparse triangular solver to get solution of the
original system.

INPUT PARAMETERS
    A       -   sparse matrix, must be NxN exactly.
                Can be stored in any sparse storage format, CRS is preferred.
    IsUpper -   which half of A is provided (another half is ignored).
                It is better to store the lower triangle because it allows
                us to avoid one transposition during internal conversion.
    B       -   array[N], right part

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate or non-SPD system).

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void sparsespdsolve(const sparsematrix* a,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    sparsematrix a2;
    sparsematrix a3;
    ae_int_t n;
    ae_vector p;
    ae_vector idummy;
    ae_int_t donotreusemem;
    spcholanalysis analysis;
    ae_bool flg;

    ae_frame_make(_state, &_frame_block);
    memset(&a2, 0, sizeof(a2));
    memset(&a3, 0, sizeof(a3));
    memset(&p, 0, sizeof(p));
    memset(&idummy, 0, sizeof(idummy));
    memset(&analysis, 0, sizeof(analysis));
    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);
    _sparsematrix_init(&a2, _state, ae_true);
    _sparsematrix_init(&a3, _state, ae_true);
    ae_vector_init(&p, 0, DT_INT, _state, ae_true);
    ae_vector_init(&idummy, 0, DT_INT, _state, ae_true);
    _spcholanalysis_init(&analysis, _state, ae_true);

    n = sparsegetnrows(a, _state);
    ae_assert(n>0, "SparseSPDSolve: N<=0", _state);
    ae_assert(sparsegetnrows(a, _state)==n, "SparseSPDSolve: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseSPDSolve: cols(A)!=N", _state);
    ae_assert(b->cnt>=n, "SparseSPDSolve: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseSPDSolve: B contains infinities or NANs", _state);
    initsparsesolverreport(rep, _state);
    
    /*
     * Perform factorization, depending on the sparse storage format and triangle being specified
     */
    donotreusemem = -1;
    if( !sparseiscrs(a, _state) )
    {
        
        /*
         * Non-CRS matrix, first has to be converted to CRS, then a transposition may be needed
         */
        sparsecopytocrs(a, &a2, _state);
        if( isupper )
        {
            sparsecopytransposecrs(&a2, &a3, _state);
            flg = spsymmanalyze(&a3, &idummy, 0.0, 0, 0, 0, donotreusemem, &analysis, _state);
        }
        else
        {
            flg = spsymmanalyze(&a2, &idummy, 0.0, 0, 0, 0, donotreusemem, &analysis, _state);
        }
    }
    else
    {
        
        /*
         * A CRS matrix
         */
        if( isupper )
        {
            sparsecopytransposecrs(a, &a2, _state);
            flg = spsymmanalyze(&a2, &idummy, 0.0, 0, 0, 0, donotreusemem, &analysis, _state);
        }
        else
        {
            flg = spsymmanalyze(a, &idummy, 0.0, 0, 0, 0, donotreusemem, &analysis, _state);
        }
    }
    if( !flg )
    {
        rep->terminationtype = -3;
        rsetallocv(n, 0.0, x, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Factorize
     */
    if( !spsymmfactorize(&analysis, _state) )
    {
        rep->terminationtype = -3;
        rsetallocv(n, 0.0, x, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Solve
     */
    rcopyallocv(n, b, x, _state);
    spsymmsolve(&analysis, x, _state);
    rep->terminationtype = 1;
    ae_frame_leave(_state);
}


/*************************************************************************
Sparse linear solver for A*x=b with N*N real  symmetric  positive definite
matrix A given by its Cholesky decomposition, and N*1 vectors x and b.

IMPORTANT: this solver requires input matrix to be in  the  SKS  (Skyline)
           or CRS (compressed row storage) format. An  exception  will  be
           generated if you pass matrix in some other format.

INPUT PARAMETERS
    A       -   sparse NxN matrix stored in CRs or SKS format, must be NxN
                exactly
    IsUpper -   which half of A is provided (another half is ignored)
    B       -   array[N], right part

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate or non-SPD system).

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void sparsespdcholeskysolve(const sparsematrix* a,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;

    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);

    n = sparsegetnrows(a, _state);
    ae_assert(n>0, "SparseSPDCholeskySolve: N<=0", _state);
    ae_assert(sparsegetnrows(a, _state)==n, "SparseSPDCholeskySolve: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseSPDCholeskySolve: cols(A)!=N", _state);
    ae_assert(sparseissks(a, _state)||sparseiscrs(a, _state), "SparseSPDCholeskySolve: A is not an SKS/CRS matrix", _state);
    ae_assert(b->cnt>=n, "SparseSPDCholeskySolve: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseSPDCholeskySolve: B contains infinities or NANs", _state);
    initsparsesolverreport(rep, _state);
    ae_vector_set_length(x, n, _state);
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_eq(sparseget(a, i, i, _state),0.0) )
        {
            rep->terminationtype = -3;
            for(i=0; i<=n-1; i++)
            {
                x->ptr.p_double[i] = (double)(0);
            }
            return;
        }
    }
    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = b->ptr.p_double[i];
    }
    if( isupper )
    {
        sparsetrsv(a, isupper, ae_false, 1, x, _state);
        sparsetrsv(a, isupper, ae_false, 0, x, _state);
    }
    else
    {
        sparsetrsv(a, isupper, ae_false, 0, x, _state);
        sparsetrsv(a, isupper, ae_false, 1, x, _state);
    }
    rep->terminationtype = 1;
}


/*************************************************************************
Sparse linear solver for A*x=b with general (nonsymmetric) N*N sparse real
matrix A, N*1 vectors x and b.

This function internally uses several solvers:
* supernodal solver with static pivoting applied to  a  2N*2N  regularized
  augmented  system, followed by iterative refinement. This solver  is   a
  recommended option because it provides the best speed and has the lowest
  memory requirements.
* sparse LU with dynamic pivoting for stability. Provides better  accuracy
  at the cost of a significantly lower performance. Recommended  only  for
  extremely unstable problems.

INPUT PARAMETERS
    A       -   sparse matrix, must be NxN exactly, any storage format
    B       -   array[N], right part
    SolverType- solver type to use:
                * 0     use the best solver. It is augmented system in the
                        current version, but may change in future releases
                * 10    use 'default profile' of the supernodal solver with
                        static   pivoting.   The  'default'   profile   is
                        intended for systems with plenty of memory; it  is
                        optimized for the best convergence at the cost  of
                        increased RAM usage. Recommended option.
                * 11    use 'limited memory'  profile  of  the  supernodal
                        solver with static  pivoting.  The  limited-memory
                        profile is intended for problems with millions  of
                        variables.  On  most  systems  it  has  the   same
                        convergence as the default profile, having somewhat
                        worse results only for ill-conditioned systems.
                * 20    use sparse LU with dynamic pivoting for stability.
                        Not intended for large-scale problems.

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate system).

  -- ALGLIB --
     Copyright 18.11.2023 by Bochkanov Sergey
*************************************************************************/
void sparsesolve(const sparsematrix* a,
     /* Real    */ const ae_vector* b,
     ae_int_t solvertype,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t n;
    double v;
    sparsematrix a2;
    ae_vector pivp;
    ae_vector pivq;
    ae_vector b2;
    ae_vector sr;
    ae_vector sc;
    double reg;
    normestimatorstate e;
    ae_int_t gmresk;
    ae_int_t maxits;

    ae_frame_make(_state, &_frame_block);
    memset(&a2, 0, sizeof(a2));
    memset(&pivp, 0, sizeof(pivp));
    memset(&pivq, 0, sizeof(pivq));
    memset(&b2, 0, sizeof(b2));
    memset(&sr, 0, sizeof(sr));
    memset(&sc, 0, sizeof(sc));
    memset(&e, 0, sizeof(e));
    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);
    _sparsematrix_init(&a2, _state, ae_true);
    ae_vector_init(&pivp, 0, DT_INT, _state, ae_true);
    ae_vector_init(&pivq, 0, DT_INT, _state, ae_true);
    ae_vector_init(&b2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&sr, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&sc, 0, DT_REAL, _state, ae_true);
    _normestimatorstate_init(&e, _state, ae_true);

    n = sparsegetnrows(a, _state);
    ae_assert(n>0, "SparseSolve: N<=0", _state);
    ae_assert((((solvertype==0||solvertype==-19)||solvertype==10)||solvertype==11)||solvertype==20, "SparseSolve: unexpected SolverType", _state);
    ae_assert(sparsegetnrows(a, _state)==n, "SparseSolve: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseSolve: cols(A)!=N", _state);
    ae_assert(b->cnt>=n, "SparseSolve: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseSolve: B contains infinities or NANs", _state);
    
    /*
     * Default solver
     */
    if( solvertype==0 )
    {
        solvertype = 10;
    }
    
    /*
     * Initialization
     */
    initsparsesolverreport(rep, _state);
    rsetallocv(n, 0.0, x, _state);
    sparsecopytocrs(a, &a2, _state);
    
    /*
     * Augmented system-based solver
     */
    if( (solvertype==-19||solvertype==10)||solvertype==11 )
    {
        gmresk = 200;
        maxits = 200;
        if( solvertype==11 )
        {
            
            /*
             * Limited memory profile - decreased amount of GMRES memory
             */
            gmresk = 25;
            maxits = 200;
        }
        if( solvertype==-19 )
        {
            
            /*
             * Debug profile - deliberately limited GMRES
             */
            gmresk = 5;
            maxits = 200;
        }
        sparsescale(&a2, 0, ae_true, ae_true, ae_true, &sr, &sc, _state);
        rcopyallocv(n, b, &b2, _state);
        rmergedivv(n, &sr, &b2, _state);
        normestimatorcreate(n, n, 5, 5, &e, _state);
        normestimatorsetseed(&e, 117, _state);
        normestimatorestimatesparse(&e, &a2, _state);
        normestimatorresults(&e, &v, _state);
        reg = ae_sqrt(ae_machineepsilon, _state)*coalesce(v, (double)(1), _state);
        directsparsesolvers_sparsesolveaug(&a2, &b2, reg, reg, 0.0, 0.0, gmresk, maxits, x, rep, _state);
        rmergedivv(n, &sc, x, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * LU-based solver
     */
    if( solvertype==20 )
    {
        if( !sparselu(&a2, 0, &pivp, &pivq, _state) )
        {
            rep->terminationtype = -3;
            for(i=0; i<=n-1; i++)
            {
                x->ptr.p_double[i] = (double)(0);
            }
            ae_frame_leave(_state);
            return;
        }
        for(i=0; i<=n-1; i++)
        {
            x->ptr.p_double[i] = b->ptr.p_double[i];
        }
        for(i=0; i<=n-1; i++)
        {
            j = pivp.ptr.p_int[i];
            v = x->ptr.p_double[i];
            x->ptr.p_double[i] = x->ptr.p_double[j];
            x->ptr.p_double[j] = v;
        }
        sparsetrsv(&a2, ae_false, ae_true, 0, x, _state);
        sparsetrsv(&a2, ae_true, ae_false, 0, x, _state);
        for(i=n-1; i>=0; i--)
        {
            j = pivq.ptr.p_int[i];
            v = x->ptr.p_double[i];
            x->ptr.p_double[i] = x->ptr.p_double[j];
            x->ptr.p_double[j] = v;
        }
        rep->terminationtype = 1;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * unexpected solver type
     */
    ae_assert(ae_false, "DIRECTSPARSESOLVERS: integrity check 1038 failed", _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Sparse linear least squares solver for A*x=b with  general  (nonsymmetric)
N*N sparse real matrix A, N*1 vectors x and b.

This function solves a regularized linear least squares problem of the form

        (                      )
    min ( |Ax-b|^2 + reg*|x|^2 ), with reg>=sqrt(MachineAccuracy)
        (                      )

The function internally uses supernodal  solver  to  solve  an  augmented-
regularized sparse system. The solver, which was initially used  to  solve
sparse square system, can also  be  used  to  solve  rectangular  systems,
provided that the system is regularized with regularizing  coefficient  at
least sqrt(MachineAccuracy), which is ~10^8 (double precision).

It can be used to solve both full rank and rank deficient systems.

INPUT PARAMETERS
    A       -   sparse MxN matrix, any storage format
    B       -   array[M], right part
    Reg     -   regularization coefficient, Reg>=sqrt(MachineAccuracy),
                lower values will be silently increased.
    SolverType- solver type to use:
                * 0     use the best solver. It is augmented system in the
                        current version, but may change in future releases
                * 10    use 'default profile' of the supernodal solver with
                        static   pivoting.   The  'default'   profile   is
                        intended for systems with plenty of memory; it  is
                        optimized for the best convergence at the cost  of
                        increased RAM usage. Recommended option.
                * 11    use 'limited memory'  profile  of  the  supernodal
                        solver with static  pivoting.  The  limited-memory
                        profile is intended for problems with millions  of
                        variables.  On  most  systems  it  has  the   same
                        convergence as the default profile, having somewhat
                        worse results only for ill-conditioned systems.

OUTPUT PARAMETERS
    X       -   array[N], least squares solution
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success.
                  
                  Present version of the solver does NOT returns negative
                  completion codes because  it  does  not  fail.  However,
                  future ALGLIB versions may include solvers which  return
                  negative completion codes.

  -- ALGLIB --
     Copyright 18.11.2023 by Bochkanov Sergey
*************************************************************************/
void sparsesolvelsreg(const sparsematrix* a,
     /* Real    */ const ae_vector* b,
     double reg,
     ae_int_t solvertype,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t n;
    ae_int_t m;
    double v;
    sparsematrix a2;
    ae_vector pivp;
    ae_vector pivq;
    ae_vector b2;
    ae_vector sr;
    ae_vector sc;
    double augreg;
    normestimatorstate e;
    ae_int_t gmresk;
    ae_int_t maxits;

    ae_frame_make(_state, &_frame_block);
    memset(&a2, 0, sizeof(a2));
    memset(&pivp, 0, sizeof(pivp));
    memset(&pivq, 0, sizeof(pivq));
    memset(&b2, 0, sizeof(b2));
    memset(&sr, 0, sizeof(sr));
    memset(&sc, 0, sizeof(sc));
    memset(&e, 0, sizeof(e));
    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);
    _sparsematrix_init(&a2, _state, ae_true);
    ae_vector_init(&pivp, 0, DT_INT, _state, ae_true);
    ae_vector_init(&pivq, 0, DT_INT, _state, ae_true);
    ae_vector_init(&b2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&sr, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&sc, 0, DT_REAL, _state, ae_true);
    _normestimatorstate_init(&e, _state, ae_true);

    m = sparsegetnrows(a, _state);
    n = sparsegetncols(a, _state);
    ae_assert(m>0, "SparseSolveLS: M<=0", _state);
    ae_assert(n>0, "SparseSolveLS: N<=0", _state);
    ae_assert(ae_isfinite(reg, _state)&&ae_fp_greater(reg,(double)(0)), "SparseSolveLS: Reg is not finite or non-positive", _state);
    ae_assert(((solvertype==0||solvertype==-19)||solvertype==10)||solvertype==11, "SparseSolveLS: unexpected SolverType", _state);
    ae_assert(b->cnt>=m, "SparseSolveLS: length(B)<M", _state);
    ae_assert(isfinitevector(b, m, _state), "SparseSolveLS: B contains infinities or NANs", _state);
    
    /*
     * Default solver
     */
    if( solvertype==0 )
    {
        solvertype = 10;
    }
    
    /*
     * Initialization
     */
    initsparsesolverreport(rep, _state);
    rsetallocv(n, 0.0, x, _state);
    sparsecopytocrs(a, &a2, _state);
    rep->terminationtype = 1;
    
    /*
     * Augmented system-based solver
     */
    if( (solvertype==-19||solvertype==10)||solvertype==11 )
    {
        reg = ae_maxreal(reg, ae_sqrt(ae_machineepsilon, _state), _state);
        gmresk = 200;
        maxits = 200;
        if( solvertype==11 )
        {
            
            /*
             * Limited memory profile
             */
            gmresk = 25;
            maxits = 200;
        }
        if( solvertype==-19 )
        {
            
            /*
             * Debug profile - deliberately limited GMRES
             */
            gmresk = 5;
            maxits = 200;
        }
        sparsescale(&a2, 0, ae_false, ae_true, ae_true, &sr, &sc, _state);
        rcopyallocv(m, b, &b2, _state);
        rmergedivv(m, &sr, &b2, _state);
        normestimatorcreate(m, n, 5, 5, &e, _state);
        normestimatorsetseed(&e, 117, _state);
        normestimatorestimatesparse(&e, &a2, _state);
        normestimatorresults(&e, &v, _state);
        augreg = (double)10*ae_sqrt(ae_machineepsilon, _state)*coalesce(v, (double)(1), _state);
        directsparsesolvers_sparsesolveaug(&a2, &b2, (double)(1), ae_maxreal(ae_sqr(augreg, _state), ae_sqr(reg, _state), _state), (double)(1), ae_sqr(reg, _state), gmresk, maxits, x, rep, _state);
        rmergedivv(n, &sc, x, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * unexpected solver type
     */
    ae_assert(ae_false, "DIRECTSPARSESOLVERS: integrity check 1622 failed", _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Sparse linear solver for A*x=b with general (nonsymmetric) N*N sparse real
matrix A given by its LU factorization, N*1 vectors x and b.

IMPORTANT: this solver requires input matrix  to  be  in  the  CRS  sparse
           storage format. An exception will  be  generated  if  you  pass
           matrix in some other format (HASH or SKS).

INPUT PARAMETERS
    A       -   LU factorization of the sparse matrix, must be NxN exactly
                in CRS storage format
    P, Q    -   pivot indexes from LU factorization
    N       -   size of A, N>0
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate system).

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void sparselusolve(const sparsematrix* a,
     /* Integer */ const ae_vector* p,
     /* Integer */ const ae_vector* q,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;
    ae_int_t n;

    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);

    n = sparsegetnrows(a, _state);
    ae_assert(n>0, "SparseLUSolve: N<=0", _state);
    ae_assert(sparsegetnrows(a, _state)==n, "SparseLUSolve: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseLUSolve: cols(A)!=N", _state);
    ae_assert(sparseiscrs(a, _state), "SparseLUSolve: A is not an SKS matrix", _state);
    ae_assert(b->cnt>=n, "SparseLUSolve: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseLUSolve: B contains infinities or NANs", _state);
    ae_assert(p->cnt>=n, "SparseLUSolve: length(P)<N", _state);
    ae_assert(q->cnt>=n, "SparseLUSolve: length(Q)<N", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=i&&p->ptr.p_int[i]<n, "SparseLUSolve: P is corrupted", _state);
        ae_assert(q->ptr.p_int[i]>=i&&q->ptr.p_int[i]<n, "SparseLUSolve: Q is corrupted", _state);
    }
    initsparsesolverreport(rep, _state);
    ae_vector_set_length(x, n, _state);
    for(i=0; i<=n-1; i++)
    {
        if( a->didx.ptr.p_int[i]==a->uidx.ptr.p_int[i]||a->vals.ptr.p_double[a->didx.ptr.p_int[i]]==0.0 )
        {
            rep->terminationtype = -3;
            for(i=0; i<=n-1; i++)
            {
                x->ptr.p_double[i] = (double)(0);
            }
            return;
        }
    }
    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = b->ptr.p_double[i];
    }
    for(i=0; i<=n-1; i++)
    {
        j = p->ptr.p_int[i];
        v = x->ptr.p_double[i];
        x->ptr.p_double[i] = x->ptr.p_double[j];
        x->ptr.p_double[j] = v;
    }
    sparsetrsv(a, ae_false, ae_true, 0, x, _state);
    sparsetrsv(a, ae_true, ae_false, 0, x, _state);
    for(i=n-1; i>=0; i--)
    {
        j = q->ptr.p_int[i];
        v = x->ptr.p_double[i];
        x->ptr.p_double[i] = x->ptr.p_double[j];
        x->ptr.p_double[j] = v;
    }
    rep->terminationtype = 1;
}


/*************************************************************************
Internal function which creates and solves a sparse augmented system

    (        |        )
    ( REG1*I |   A    )
    (        |        )
    (-----------------)
    (        |        )
    (    A'  | -REG2*I)
    (        |        )

This function uses supernodal LDLT to factorize the system,  then  applies
iterative refinement (actually, a preconditioned GMRES which  acts  as  an
improved version of the iterative refinement).

It can be   used  to  solve  square  nonsymmetric  systems,  or  to  solve
rectangular systems in a least squares  sense.  Its  performance  somewhat
deteriorates for rank-deficient systems.

INPUT PARAMETERS
    A       -   sparse matrix, M*N, CRS format
    B       -   array[N], right part
    Reg1F,Reg2F-regularizing factors used during the factorization of  the
                system, Reg1F>0, Reg2F>0
    Reg1R,Reg2R-regularizing factors used during the refinement stage,
                Reg1R>=0, Reg2R>=0
    X       -   preallocated buffer

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate system).
                * rep.iterationscount - set ot an amount of GMRES
                  iterations performed

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
static void directsparsesolvers_sparsesolveaug(const sparsematrix* a,
     /* Real    */ const ae_vector* b,
     double reg1f,
     double reg2f,
     double reg1r,
     double reg2r,
     ae_int_t gmresk,
     ae_int_t maxits,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t k;
    ae_int_t jj;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t m;
    ae_int_t n;
    ae_int_t nzaug;
    sparsematrix aug;
    ae_vector priorities;
    ae_vector bx;
    spcholanalysis analysis;
    sparsesolverstate solver;
    sparsesolverreport innerrep;
    ae_int_t requesttype;
    ae_vector rr;
    ae_vector q;
    ae_vector y;
    ae_int_t priorityordering;
    ae_int_t donotreusememory;

    ae_frame_make(_state, &_frame_block);
    memset(&aug, 0, sizeof(aug));
    memset(&priorities, 0, sizeof(priorities));
    memset(&bx, 0, sizeof(bx));
    memset(&analysis, 0, sizeof(analysis));
    memset(&solver, 0, sizeof(solver));
    memset(&innerrep, 0, sizeof(innerrep));
    memset(&rr, 0, sizeof(rr));
    memset(&q, 0, sizeof(q));
    memset(&y, 0, sizeof(y));
    _sparsematrix_init(&aug, _state, ae_true);
    ae_vector_init(&priorities, 0, DT_INT, _state, ae_true);
    ae_vector_init(&bx, 0, DT_REAL, _state, ae_true);
    _spcholanalysis_init(&analysis, _state, ae_true);
    _sparsesolverstate_init(&solver, _state, ae_true);
    _sparsesolverreport_init(&innerrep, _state, ae_true);
    ae_vector_init(&rr, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&q, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&y, 0, DT_REAL, _state, ae_true);

    ae_assert(sparseiscrs(a, _state), "SparseSolveAug: A is not stored in the CRS format", _state);
    m = sparsegetnrows(a, _state);
    n = sparsegetncols(a, _state);
    ae_assert(ae_isfinite(reg1f, _state)&&ae_fp_greater(reg1f,(double)(0)), "SparseSolveAug: Reg1F is non-positive", _state);
    ae_assert(ae_isfinite(reg2f, _state)&&ae_fp_greater(reg2f,(double)(0)), "SparseSolveAug: Reg2F is non-positive", _state);
    ae_assert(ae_isfinite(reg1r, _state)&&ae_fp_greater_eq(reg1r,(double)(0)), "SparseSolveAug: Reg1R is non-positive", _state);
    ae_assert(ae_isfinite(reg2r, _state)&&ae_fp_greater_eq(reg2r,(double)(0)), "SparseSolveAug: Reg2R is non-positive", _state);
    ae_assert(b->cnt>=m, "SparseSolveAug: length(B)<N", _state);
    ae_assert(isfinitevector(b, m, _state), "SparseSolveAug: B contains infinities or NANs", _state);
    ae_assert(x->cnt>=n, "SparseSolveAug: length(X)<N", _state);
    
    /*
     * Generate augmented matrix
     */
    rallocv(n+m, &rr, _state);
    nzaug = a->ridx.ptr.p_int[a->m]+n+m;
    aug.matrixtype = 1;
    aug.m = m+n;
    aug.n = m+n;
    iallocv(aug.m+1, &aug.ridx, _state);
    iallocv(nzaug, &aug.idx, _state);
    rallocv(nzaug, &aug.vals, _state);
    aug.ridx.ptr.p_int[0] = 0;
    for(i=0; i<=n-1; i++)
    {
        rr.ptr.p_double[i] = -reg2f;
        aug.idx.ptr.p_int[i] = i;
        aug.vals.ptr.p_double[i] = rr.ptr.p_double[i];
        aug.ridx.ptr.p_int[i+1] = i+1;
    }
    for(i=0; i<=m-1; i++)
    {
        rr.ptr.p_double[n+i] = reg1f;
        k = aug.ridx.ptr.p_int[n+i];
        j0 = a->ridx.ptr.p_int[i];
        j1 = a->ridx.ptr.p_int[i+1]-1;
        for(jj=j0; jj<=j1; jj++)
        {
            aug.idx.ptr.p_int[k] = a->idx.ptr.p_int[jj];
            aug.vals.ptr.p_double[k] = a->vals.ptr.p_double[jj];
            k = k+1;
        }
        aug.idx.ptr.p_int[k] = n+i;
        aug.vals.ptr.p_double[k] = rr.ptr.p_double[n+i];
        k = k+1;
        aug.ridx.ptr.p_int[n+i+1] = k;
    }
    ae_assert(aug.ridx.ptr.p_int[n+m]==nzaug, "SparseSolveAug: integrity check 2141 failed", _state);
    sparsecreatecrsinplace(&aug, _state);
    
    /*
     * Factorize augmented system
     */
    priorityordering = 3;
    donotreusememory = -1;
    isetallocv(n+m, 1, &priorities, _state);
    isetv(n, 0, &priorities, _state);
    if( !spsymmanalyze(&aug, &priorities, 0.0, 0, 1, priorityordering, donotreusememory, &analysis, _state) )
    {
        ae_assert(ae_false, "SparseSolveAug: integrity check 4141 failed", _state);
    }
    while(!spsymmfactorize(&analysis, _state))
    {
        
        /*
         * Factorization failure. Extremely rare, almost unable to reproduce.
         */
        rmulv(n+m, 10.0, &rr, _state);
        spsymmreloaddiagonal(&analysis, &rr, _state);
    }
    
    /*
     * Solve using GMRES as an improved iterative refinement
     */
    rsetallocv(n+m, reg1r, &rr, _state);
    rsetv(n, -reg2r, &rr, _state);
    rallocv(m+n, &q, _state);
    rallocv(m+n, &y, _state);
    rsetallocv(m+n, 0.0, &bx, _state);
    rcopyvx(m, b, 0, &bx, n, _state);
    sparsesolvercreate(m+n, &solver, _state);
    sparsesolversetalgogmres(&solver, gmresk, _state);
    sparsesolversetcond(&solver, (double)10*ae_machineepsilon, maxits, _state);
    sparsesolveroocstart(&solver, &bx, _state);
    while(sparsesolverooccontinue(&solver, _state))
    {
        sparsesolveroocgetrequestinfo(&solver, &requesttype, _state);
        ae_assert(requesttype==0, "SPARSESOLVE: integrity check 8618 failed", _state);
        sparsesolveroocgetrequestdata(&solver, &q, _state);
        spsymmsolve(&analysis, &q, _state);
        sparsegemv(a, 1.0, 0, &q, 0, 0.0, &y, n, _state);
        sparsegemv(a, 1.0, 1, &q, n, 0.0, &y, 0, _state);
        rmuladdv(m+n, &q, &rr, &y, _state);
        sparsesolveroocsendresult(&solver, &y, _state);
    }
    sparsesolveroocstop(&solver, &bx, &innerrep, _state);
    if( innerrep.terminationtype<=0 )
    {
        rep->terminationtype = innerrep.terminationtype;
        ae_frame_leave(_state);
        return;
    }
    spsymmsolve(&analysis, &bx, _state);
    rcopyvx(n, &bx, 0, x, 0, _state);
    rep->terminationtype = 1;
    rep->iterationscount = innerrep.iterationscount;
    ae_frame_leave(_state);
}


/*$ End $*/
