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

#ifndef _reviseddualsimplex_h
#define _reviseddualsimplex_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "scodes.h"
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
#include "lpqppresolve.h"


/*$ Declarations $*/


/*************************************************************************
This object stores settings for dual simplex solver
*************************************************************************/
typedef struct
{
    double pivottol;
    double perturbmag;
    ae_int_t maxtrfage;
    ae_int_t trftype;
    ae_int_t ratiotest;
    ae_int_t pricing;
    ae_int_t shifting;
    double xtolabs;
    double xtolrelabs;
    double dtolabs;
} dualsimplexsettings;


/*************************************************************************
A vector that is stored in dual dense/sparse format.
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t k;
    ae_vector idx;
    ae_vector vals;
    ae_vector dense;
} dssvector;


/*************************************************************************
This object stores basis and its triangular factorization for DualSimplexState
*************************************************************************/
typedef struct
{
    ae_int_t ns;
    ae_int_t m;
    ae_vector idx;
    ae_vector nidx;
    ae_vector isbasic;
    ae_int_t trftype;
    ae_bool isvalidtrf;
    ae_int_t trfage;
    ae_matrix denselu;
    sparsematrix sparsel;
    sparsematrix sparseu;
    sparsematrix sparseut;
    ae_vector rowpermbwd;
    ae_vector colpermbwd;
    ae_vector densepfieta;
    ae_vector densemu;
    ae_vector rk;
    ae_vector dk;
    ae_vector dseweights;
    ae_bool dsevalid;
    double eminu;
    ae_int_t statfact;
    ae_int_t statupdt;
    double statoffdiag;
    ae_vector wtmp0;
    ae_vector wtmp1;
    ae_vector wtmp2;
    ae_vector nrs;
    ae_vector tcinvidx;
    ae_matrix denselu2;
    ae_vector densep2;
    ae_vector densep2c;
    sparsematrix sparselu1;
    sparsematrix sparselu2;
    sluv2buffer lubuf2;
    ae_vector tmpi;
    ae_vector utmp0;
    ae_vector utmpi;
    sparsematrix sparseludbg;
} dualsimplexbasis;


/*************************************************************************
This object stores subproblem for DualSimplexState object
*************************************************************************/
typedef struct
{
    ae_int_t ns;
    ae_int_t m;
    ae_vector rawc;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector bndt;
    ae_vector xa;
    ae_vector d;
    ae_int_t state;
    ae_vector xb;
    ae_vector bndlb;
    ae_vector bndub;
    ae_vector bndtb;
    ae_vector bndtollb;
    ae_vector bndtolub;
    ae_vector effc;
} dualsimplexsubproblem;


/*************************************************************************
This object stores state of the DSS solver.
MUST be initialized with DSSInit().
*************************************************************************/
typedef struct
{
    ae_vector rowscales;
    ae_vector rawbndl;
    ae_vector rawbndu;
    ae_int_t ns;
    ae_int_t m;
    sparsematrix a;
    sparsematrix at;
    dualsimplexbasis basis;
    dualsimplexsubproblem primary;
    dualsimplexsubproblem phase1;
    dualsimplexsubproblem phase3;
    ae_vector repx;
    ae_vector replagbc;
    ae_vector replaglc;
    ae_vector repstats;
    ae_int_t repterminationtype;
    ae_int_t repiterationscount;
    ae_int_t repiterationscount1;
    ae_int_t repiterationscount2;
    ae_int_t repiterationscount3;
    ae_int_t repphase1time;
    ae_int_t repphase2time;
    ae_int_t repphase3time;
    ae_int_t repdualpricingtime;
    ae_int_t repdualbtrantime;
    ae_int_t repdualpivotrowtime;
    ae_int_t repdualratiotesttime;
    ae_int_t repdualftrantime;
    ae_int_t repdualupdatesteptime;
    double repfillpivotrow;
    ae_int_t repfillpivotrowcnt;
    double repfillrhor;
    ae_int_t repfillrhorcnt;
    double repfilldensemu;
    ae_int_t repfilldensemucnt;
    ae_bool dotrace;
    ae_bool dodetailedtrace;
    ae_bool dotimers;
    ae_vector btrantmp0;
    ae_vector btrantmp1;
    ae_vector btrantmp2;
    ae_vector ftrantmp0;
    ae_vector ftrantmp1;
    ae_vector possibleflips;
    ae_int_t possibleflipscnt;
    ae_vector dfctmp0;
    ae_vector dfctmp1;
    ae_vector dfctmp2;
    ae_vector ustmpi;
    apbuffers xydsbuf;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
    dssvector alphar;
    dssvector rhor;
    ae_vector tau;
    ae_vector alphaq;
    ae_vector alphaqim;
    ae_vector eligiblealphar;
    ae_vector harrisset;
} dualsimplexstate;


/*$ Body $*/


void dsssettingsinit(dualsimplexsettings* settings, ae_state *_state);


/*************************************************************************
This function initializes DSS structure. Previously  allocated  memory  is
reused as much as possible.

Default state of the problem is zero cost vector, all variables are  fixed
at zero.

  -- ALGLIB --
     Copyright 01.07.2018 by Bochkanov Sergey
*************************************************************************/
void dssinit(ae_int_t n, dualsimplexstate* s, ae_state *_state);


/*************************************************************************
This function specifies LP problem

INPUT PARAMETERS:
    State   -   structure previously allocated with minlpcreate() call.
    
    BndL    -   lower bounds, array[N].
    BndU    -   upper bounds, array[N].
    
    DenseA  -   dense array[K,N], dense linear constraints (not supported
                in present version)
    SparseA -   sparse linear constraints, sparsematrix[K,N] in CRS format
    AKind   -   type of A: 0 for dense, 1 for sparse
    AL, AU  -   lower and upper bounds, array[K]
    K       -   number of equality/inequality constraints, K>=0.
    
    ProposedBasis- basis to import from (if BasisType=2)
    BasisInitType-  what to do with basis:
                * 0 - set new basis to all-logicals
                * 1 - try to reuse previous basis as much as possible
                * 2 - try to import basis from ProposedBasis
    Settings-   algorithm settings

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void dsssetproblem(dualsimplexstate* state,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_matrix* densea,
     const sparsematrix* sparsea,
     ae_int_t akind,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     const dualsimplexbasis* proposedbasis,
     ae_int_t basisinittype,
     const dualsimplexsettings* settings,
     ae_state *_state);


/*************************************************************************
This function exports basis from the primary (phase II) subproblem.

INPUT PARAMETERS:
    State   -   structure
    
OUTPUT PARAMETERS
    Basis   -   current basis exported (no factorization, only set of
                basis/nonbasic variables)

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void dssexportbasis(const dualsimplexstate* state,
     dualsimplexbasis* basis,
     ae_state *_state);


/*************************************************************************
This function solves LP problem with dual simplex solver.

INPUT PARAMETERS:
    State   -   state
    
Solution results can be found in fields  of  State  which  are  explicitly
declared as accessible by external code.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void dssoptimize(dualsimplexstate* state,
     const dualsimplexsettings* settings,
     ae_state *_state);
void _dualsimplexsettings_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dualsimplexsettings_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dualsimplexsettings_clear(void* _p);
void _dualsimplexsettings_destroy(void* _p);
void _dssvector_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dssvector_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dssvector_clear(void* _p);
void _dssvector_destroy(void* _p);
void _dualsimplexbasis_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dualsimplexbasis_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dualsimplexbasis_clear(void* _p);
void _dualsimplexbasis_destroy(void* _p);
void _dualsimplexsubproblem_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dualsimplexsubproblem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dualsimplexsubproblem_clear(void* _p);
void _dualsimplexsubproblem_destroy(void* _p);
void _dualsimplexstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dualsimplexstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dualsimplexstate_clear(void* _p);
void _dualsimplexstate_destroy(void* _p);


/*$ End $*/
#endif

