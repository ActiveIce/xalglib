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

#ifndef _vipmsolver_h
#define _vipmsolver_h

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
#include "hblas.h"
#include "creflections.h"
#include "sblas.h"
#include "ablas.h"
#include "ortfac.h"
#include "blas.h"
#include "rotations.h"
#include "bdsvd.h"
#include "svd.h"
#include "dlu.h"
#include "sptrf.h"
#include "apstruct.h"
#include "amdordering.h"
#include "spchol.h"
#include "matgen.h"
#include "trfac.h"
#include "hsschur.h"
#include "basicstatops.h"
#include "evd.h"
#include "trlinsolve.h"
#include "safesolve.h"
#include "rcond.h"
#include "xblas.h"
#include "directdensesolvers.h"
#include "linmin.h"
#include "optguardapi.h"
#include "matinv.h"
#include "optserv.h"
#include "fbls.h"
#include "minlbfgs.h"
#include "cqmodels.h"
#include "lpqpserv.h"


/*$ Declarations $*/


/*************************************************************************
Variables of IPM method (primal and dual, slacks)
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    ae_vector x;
    ae_vector g;
    ae_vector w;
    ae_vector t;
    ae_vector p;
    ae_vector y;
    ae_vector z;
    ae_vector v;
    ae_vector s;
    ae_vector q;
} vipmvars;


/*************************************************************************
Reduced (M+N)*(M+N) KKT system stored in sparse format.
*************************************************************************/
typedef struct
{
    sparsematrix rawsystem;
    ae_vector effectivediag;
    ae_vector isdiagonal;
    ae_vector rowdegrees;
    ae_vector coldegrees;
    ae_int_t ntotal;
    spcholanalysis analysis;
    ae_vector priorities;
    ae_vector diagterm;
    ae_vector dampterm;
    ae_vector tmpb;
    ae_vector tmprhs;
    ae_vector tmpcorr;
} vipmreducedsparsesystem;


/*************************************************************************
Right-hand side for KKT system:
* Rho           corresponds to Ax-w=b
* Nu            corresponds to x-g=l
* Tau           corresponds to x+t=u
* Alpha         corresponds to w+p=r
* Sigma         corresponds to A'y+z-s-Hx=c
* Beta          corresponds to y+q-v=0
* GammaZ, GammaS, GammaQ, GammaW correspond to complementarity conditions
*************************************************************************/
typedef struct
{
    ae_vector sigma;
    ae_vector beta;
    ae_vector rho;
    ae_vector nu;
    ae_vector tau;
    ae_vector alpha;
    ae_vector gammaz;
    ae_vector gammas;
    ae_vector gammaw;
    ae_vector gammaq;
} vipmrighthandside;


/*************************************************************************
VIPM state
*************************************************************************/
typedef struct
{
    ae_bool slacksforequalityconstraints;
    ae_int_t n;
    ae_int_t nmain;
    double epsp;
    double epsd;
    double epsgap;
    ae_bool islinear;
    ae_vector scl;
    ae_vector invscl;
    ae_vector xorigin;
    double targetscale;
    ae_vector c;
    ae_matrix denseh;
    sparsematrix sparseh;
    ae_vector diagr;
    ae_int_t hkind;
    ae_bool isdiagonalh;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector rawbndl;
    ae_vector rawbndu;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_matrix denseafull;
    ae_matrix denseamain;
    sparsematrix sparseafull;
    sparsematrix sparseamain;
    sparsematrix combinedaslack;
    ae_vector ascales;
    ae_vector aflips;
    ae_vector b;
    ae_vector r;
    ae_vector hasr;
    ae_int_t mdense;
    ae_int_t msparse;
    vipmvars current;
    vipmvars best;
    vipmvars trial;
    vipmvars deltaaff;
    vipmvars deltacorr;
    ae_vector isfrozen;
    ae_vector hasgz;
    ae_vector hasts;
    ae_vector haswv;
    ae_vector haspq;
    ae_int_t cntgz;
    ae_int_t cntts;
    ae_int_t cntwv;
    ae_int_t cntpq;
    ae_int_t repiterationscount;
    ae_int_t repncholesky;
    ae_bool dotrace;
    ae_bool dodetailedtrace;
    ae_int_t factorizationtype;
    ae_bool factorizationpoweredup;
    ae_bool factorizationpresent;
    ae_vector diagdz;
    ae_vector diagdzi;
    ae_vector diagdziri;
    ae_vector diagds;
    ae_vector diagdsi;
    ae_vector diagdsiri;
    ae_vector diagdw;
    ae_vector diagdwi;
    ae_vector diagdwir;
    ae_vector diagdq;
    ae_vector diagdqi;
    ae_vector diagdqiri;
    ae_vector diagddr;
    ae_vector diagde;
    ae_vector diagder;
    ae_matrix factdensehaug;
    ae_vector factregdhrh;
    ae_vector factinvregdzrz;
    ae_vector factregewave;
    ae_vector facttmpdiag;
    ae_vector facttmpdamp;
    vipmreducedsparsesystem reducedsparsesystem;
    vipmrighthandside rhs;
    ae_vector rhsalphacap;
    ae_vector rhsbetacap;
    ae_vector rhsnucap;
    ae_vector rhstaucap;
    ae_vector deltaxy;
    ae_vector tmphx;
    ae_vector tmpax;
    ae_vector tmpaty;
    vipmvars zerovars;
    ae_vector dummyr;
    ae_vector tmpy;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
    ae_matrix tmpr2;
    ae_vector tmplaggrad;
    ae_vector tmpi;
    sparsematrix tmpsparse0;
} vipmstate;


/*$ Body $*/


/*************************************************************************
Initializes QP-IPM state and prepares it to receive quadratic/linear terms
and constraints.

The solver is configured to work internally with dense NxN  factorization,
no matter what exactly is passed - dense or sparse matrices.

INPUT PARAMETERS:
    State       -   solver  state  to  be configured; previously allocated
                    memory is reused as much as possible
    S           -   scale vector, array[N]:
                    * I-th element contains scale of I-th variable,
                    * S[I]>0
    XOrigin     -   origin term, array[N]. Can be zero. The solver solves
                    problem of the form
                    
                    >
                    > min(0.5*(x-x_origin)'*A*(x-x_origin)+b'*(x-x_origin))
                    >
                    
                    The terms A and b (as well as constraints) will be
                    specified later with separate calls.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void vipminitdense(vipmstate* state,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Initializes QP-IPM state and prepares it to receive quadratic/linear terms
and constraints.

The solver is configured to work internally with dense NxN problem divided
into two distinct parts - "main" and slack one:
* dense quadratic term  is  a  NMain*NMain  matrix  (NMain<=N),  quadratic
  coefficients are zero for variables outside of [0,NMain) range)
* linear term is general vector of length N
* linear constraints have special structure for variable with  indexes  in
  [NMain,N) range: at most one element per column can be nonzero.

This mode is intended for problems arising during SL1QP nonlinear programming.

INPUT PARAMETERS:
    State       -   solver  state  to  be configured; previously allocated
                    memory is reused as much as possible
    S           -   scale vector, array[N]:
                    * I-th element contains scale of I-th variable,
                    * S[I]>0
    XOrigin     -   origin term, array[N]. Can be zero. The solver solves
                    problem of the form
                    
                    >
                    > min(0.5*(x-x_origin)'*A*(x-x_origin)+b'*(x-x_origin))
                    >
                    
                    The terms A and b (as well as constraints) will be
                    specified later with separate calls.
    NMain       -   number of "main" variables, 1<=NMain<=N
    N           -   total number of variables including slack ones

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void vipminitdensewithslacks(vipmstate* state,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t nmain,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Initializes QP-IPM state and prepares it to receive quadratic/linear terms
and constraints.

The  solver  is configured  to  work  internally  with  sparse (N+M)x(N+M)
factorization no matter what exactly is passed - dense or sparse matrices.
Dense quadratic term will be sparsified prior to storage.

INPUT PARAMETERS:
    State       -   solver  state  to  be configured; previously allocated
                    memory is reused as much as possible
    S           -   scale vector, array[N]:
                    * I-th element contains scale of I-th variable,
                    * S[I]>0
    XOrigin     -   origin term, array[N]. Can be zero. The solver solves
                    problem of the form
                    >
                    > min(0.5*(x-x_origin)'*A*(x-x_origin)+b'*(x-x_origin))
                    >
                    The terms A and b (as well as constraints) will be
                    specified later with separate calls.
    N           -   total number of variables, N>=1
    
This optimization mode assumes that no slack variables is present.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void vipminitsparse(vipmstate* state,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Sets linear/quadratic terms for QP-IPM solver

If you initialized solver with VIMPInitDenseWithSlacks(), NMain below is a
number of non-slack variables. In other cases, NMain=N.

INPUT PARAMETERS:
    State               -   instance initialized with one of the initialization
                            functions
    DenseH              -   if HKind=0: array[NMain,NMain], dense quadratic term
                            (either upper or lower triangle)
    SparseH             -   if HKind=1: array[NMain,NMain], sparse quadratic term
                            (either upper or lower triangle)
    HKind               -   0 or 1, quadratic term format
    IsUpper             -   whether dense/sparse H contains lower or upper
                            triangle of the quadratic term
    C                   -   array[N], linear term

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void vipmsetquadraticlinear(vipmstate* state,
     /* Real    */ const ae_matrix* denseh,
     const sparsematrix* sparseh,
     ae_int_t hkind,
     ae_bool isupper,
     /* Real    */ const ae_vector* c,
     ae_state *_state);


/*************************************************************************
Sets constraints for QP-IPM solver

INPUT PARAMETERS:
    State               -   instance initialized with one of the initialization
                            functions
    BndL, BndU          -   lower and upper bound. BndL[] can be -INF,
                            BndU[] can be +INF.
    SparseA             -   sparse constraint matrix, CRS format
    MSparse             -   number of sparse constraints
    DenseA              -   array[MDense,N], dense part of the constraints
    MDense              -   number of dense constraints
    CL, CU              -   lower and upper bounds for constraints, first
                            MSparse are bounds for sparse part, following
                            MDense ones are bounds for dense part,
                            MSparse+MDense in total.
                            -INF <= CL[I] <= CU[I] <= +INF.
                            
This function throws exception if constraints have inconsistent bounds, i.e.
either BndL[I]>BndU[I] or CL[I]>CU[I]. In all other cases it succeeds.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void vipmsetconstraints(vipmstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     const sparsematrix* sparsea,
     ae_int_t msparse,
     /* Real    */ const ae_matrix* densea,
     ae_int_t mdense,
     /* Real    */ const ae_vector* cl,
     /* Real    */ const ae_vector* cu,
     ae_state *_state);


/*************************************************************************
Sets stopping criteria for QP-IPM solver.

You can set all epsilon-values to one small value, about 1.0E-6.

INPUT PARAMETERS:
    State               -   instance initialized with one of the initialization
                            functions
    EpsP                -   maximum primal error allowed in the  solution,
                            EpsP>=0. Zero will be  automatically  replaced
                            by recommended default value,  which is  equal
                            to 10*Sqrt(Epsilon) in the current version
    EpsD                -   maximum  dual  error allowed in the  solution,
                            EpsP>=0. Zero will be  automatically  replaced
                            by recommended default value,  which is  equal
                            to 10*Sqrt(Epsilon) in the current version
    EpsGap              -   maximum  duality gap allowed in the  solution,
                            EpsP>=0. Zero will be  automatically  replaced
                            by recommended default value,  which is  equal
                            to 10*Sqrt(Epsilon) in the current version

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void vipmsetcond(vipmstate* state,
     double epsp,
     double epsd,
     double epsgap,
     ae_state *_state);


/*************************************************************************
Solve QP problem.

INPUT PARAMETERS:
    State               -   solver instance
    DropBigBounds       -   If True, algorithm may drop box and linear constraints
                            with huge bound values that destabilize algorithm.
    
OUTPUT PARAMETERS:
    XS                  -   array[N], solution
    LagBC               -   array[N], Lagrange multipliers for box constraints
    LagLC               -   array[M], Lagrange multipliers for linear constraints
    TerminationType     -   completion code, positive values for success,
                            negative for failures (XS constrains best point
                            found so far):
                            * -2    the task is either unbounded or infeasible;
                                    the IPM solver has difficulty distinguishing between these two.
                            * +1    stopping criteria are met
                            * +7    stopping criteria are too stringent

RESULT:

This function ALWAYS returns something  meaningful in XS, LagBC, LagLC - 
either solution or the best point so far, even for negative TerminationType.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void vipmoptimize(vipmstate* state,
     ae_bool dropbigbounds,
     /* Real    */ ae_vector* xs,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     ae_int_t* terminationtype,
     ae_state *_state);
void _vipmvars_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _vipmvars_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _vipmvars_clear(void* _p);
void _vipmvars_destroy(void* _p);
void _vipmreducedsparsesystem_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _vipmreducedsparsesystem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _vipmreducedsparsesystem_clear(void* _p);
void _vipmreducedsparsesystem_destroy(void* _p);
void _vipmrighthandside_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _vipmrighthandside_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _vipmrighthandside_clear(void* _p);
void _vipmrighthandside_destroy(void* _p);
void _vipmstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _vipmstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _vipmstate_clear(void* _p);
void _vipmstate_destroy(void* _p);


/*$ End $*/
#endif

