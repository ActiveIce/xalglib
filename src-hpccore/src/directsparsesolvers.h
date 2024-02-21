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

#ifndef _directsparsesolvers_h
#define _directsparsesolvers_h

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
#include "iterativesparse.h"
#include "dlu.h"
#include "sptrf.h"
#include "apstruct.h"
#include "amdordering.h"
#include "spchol.h"
#include "trfac.h"
#include "normestimator.h"


/*$ Declarations $*/


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


/*$ End $*/
#endif

