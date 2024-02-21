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

#ifndef _ipm2solver_h
#define _ipm2solver_h

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
    ae_int_t ntotal;
    ae_int_t m;
    ae_vector x;
    ae_vector g;
    ae_vector t;
    ae_vector y;
    ae_vector z;
    ae_vector s;
} ipm2vars;


/*************************************************************************
Reduced (NWrk+M)*(NWrk+M) KKT system stored in sparse format.
*************************************************************************/
typedef struct
{
    sparsematrix compactkkt;
    ae_vector compactpriorities;
    spcholanalysis analysis;
    ae_vector extendedrawdiagonal;
    ae_vector extendedeffdiagonal;
    ae_vector compacteffdiagonal;
    ae_vector extendedrhs;
    ae_vector extendedregrhs;
    ae_vector compactrhs;
    ae_vector u0;
    ae_vector u1;
    ae_vector v0;
    ae_vector v1;
    ae_vector vm0;
    ae_vector vm1;
    ae_vector um0;
    ae_vector d0;
    ae_vector d1;
    ae_vector d2;
    ae_vector d3;
    ae_vector d4;
    ae_vector d5;
    ae_vector d6;
    ae_vector d7;
    ae_vector rowdegrees;
    ae_vector coldegrees;
} ipm2reducedsystem;


/*************************************************************************
Right-hand side for a big (non-condensed) KKT system of the form

-(ST+Eps)*(dT/T) - T*dS                             = -GammaST
-(GZ+Eps)*(dG/G) - G*dZ                             = -GammaGZ
-(H+Eps)*dX + A'*dY + dZ - dS - W'*dQ               = ED
A*dX + Eps*dY                                       = EA
-G*(dG/G) + dX + Eps*dZ                             = EL
-T*(dT/T) - dX + Eps*dS                             = EU
W*dX + (-inv(F)+Eps)*dQ                             = EQ

where
    GammaST = mu-ST
    GammaGZ = mu-GZ
    ED      = c-A'*y-Z+S+H*x+W'*Q
    EA      = -A*X
    EL      = L-X+G
    EU      = T+X-U
    EQ      = inv(F)*Q-W*X
*************************************************************************/
typedef struct
{
    ae_vector gammagz;
    ae_vector gammats;
    ae_vector ed;
    ae_vector ea;
    ae_vector el;
    ae_vector eu;
    ae_vector eq;
} ipm2righthandside;


/*************************************************************************
VIPM state
*************************************************************************/
typedef struct
{
    ae_int_t nuser;
    ae_int_t naug;
    ae_int_t ntotal;
    double epsp;
    double epsd;
    double epsgap;
    ae_bool islinear;
    ae_vector sclx;
    ae_vector invsclx;
    ae_vector xoriginx;
    double targetscale;
    ae_vector ce;
    sparsematrix sparsehe;
    ae_vector diagr;
    ae_bool isdiagonalh;
    ae_vector rawbndl;
    ae_vector rawbndu;
    ae_vector bndle;
    ae_vector bndue;
    ae_vector bndlef;
    ae_vector bnduef;
    ae_vector hasbndle;
    ae_vector hasbndue;
    sparsematrix rawae;
    ae_vector ascales;
    ae_int_t mraw;
    ipm2vars current;
    ipm2vars best;
    ipm2vars delta;
    ipm2vars corr;
    ae_vector hasgz;
    ae_vector hasts;
    ae_vector maskgz;
    ae_vector maskts;
    ae_int_t cntgz;
    ae_int_t cntts;
    ae_vector currenthx;
    ae_vector currentax;
    ae_vector currentaty;
    ipm2righthandside rhstarget;
    ae_int_t repiterationscount;
    ae_int_t repncholesky;
    ae_bool dotrace;
    ae_bool dodetailedtrace;
    ipm2reducedsystem reducedsystem;
    ipm2righthandside rhsprogress;
    ae_vector tmphx;
    ae_vector tmpax;
    ae_vector tmpaty;
    ae_vector dummyr;
    ae_vector tmp0;
    ae_matrix tmpr2;
    sparsematrix tmpa;
    ae_vector tmpal;
    ae_vector tmpau;
    sparsematrix tmpsparse0;
    sparsematrix tmplowerh;
    ae_matrix tmpccorr;
    ae_vector tmpdcorr;
} ipm2state;


/*$ Body $*/


/*************************************************************************
Initializes QP-IPM state and prepares it to receive quadratic/linear terms
and constraints.

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

  -- ALGLIB --
     Copyright 21.12.2022 by Bochkanov Sergey
*************************************************************************/
void ipm2init(ipm2state* state,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t nuser,
     /* Real    */ const ae_matrix* denseh,
     const sparsematrix* sparseh,
     ae_int_t hkind,
     ae_bool isupper,
     /* Real    */ const ae_matrix* ccorr,
     /* Real    */ const ae_vector* dcorr,
     ae_int_t kcorr,
     /* Real    */ const ae_vector* c,
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
     Copyright 21.12.2022 by Bochkanov Sergey
*************************************************************************/
void ipm2setcond(ipm2state* state,
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
void ipm2optimize(ipm2state* state,
     ae_bool dropbigbounds,
     /* Real    */ ae_vector* xs,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     ae_int_t* terminationtype,
     ae_state *_state);
void _ipm2vars_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _ipm2vars_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _ipm2vars_clear(void* _p);
void _ipm2vars_destroy(void* _p);
void _ipm2reducedsystem_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _ipm2reducedsystem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _ipm2reducedsystem_clear(void* _p);
void _ipm2reducedsystem_destroy(void* _p);
void _ipm2righthandside_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _ipm2righthandside_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _ipm2righthandside_clear(void* _p);
void _ipm2righthandside_destroy(void* _p);
void _ipm2state_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _ipm2state_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _ipm2state_clear(void* _p);
void _ipm2state_destroy(void* _p);


/*$ End $*/
#endif

