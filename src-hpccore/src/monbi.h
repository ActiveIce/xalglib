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

#ifndef _monbi_h
#define _monbi_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "linmin.h"
#include "apserv.h"
#include "ablasf.h"
#include "tsort.h"
#include "optguardapi.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "creflections.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "matgen.h"
#include "scodes.h"
#include "sparse.h"
#include "dlu.h"
#include "sptrf.h"
#include "apstruct.h"
#include "amdordering.h"
#include "spchol.h"
#include "rotations.h"
#include "trfac.h"
#include "hblas.h"
#include "sblas.h"
#include "ortfac.h"
#include "blas.h"
#include "bdsvd.h"
#include "svd.h"
#include "hsschur.h"
#include "basicstatops.h"
#include "evd.h"
#include "trlinsolve.h"
#include "safesolve.h"
#include "rcond.h"
#include "matinv.h"
#include "optserv.h"
#include "fbls.h"
#include "snnls.h"
#include "minlbfgs.h"
#include "cqmodels.h"
#include "sactivesets.h"
#include "minbleic.h"
#include "lpqppresolve.h"
#include "reviseddualsimplex.h"
#include "nlcslp.h"
#include "xblas.h"
#include "directdensesolvers.h"
#include "lpqpserv.h"
#include "vipmsolver.h"
#include "ipm2solver.h"
#include "nlcsqp.h"
#include "nlcfsqp.h"
#include "nlcaul.h"
#include "minnlc.h"


/*$ Declarations $*/


/*************************************************************************
This object stores temporaries of the NBI solver.
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    double epsx;
    ae_int_t maxits;
    ae_bool xrep;
    ae_vector xstart;
    ae_int_t frontsize;
    ae_bool polishsolutions;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t ksparse;
    ae_int_t kdense;
    ae_matrix densec;
    sparsematrix sparsec;
    ae_vector cl;
    ae_vector cu;
    ae_int_t nnlc;
    ae_vector nl;
    ae_vector nu;
    ae_vector x;
    double f;
    ae_vector fi;
    ae_matrix j;
    ae_bool needfij;
    ae_bool xupdated;
    rcommstate rstate;
    ae_bool userrequestedtermination;
    ae_matrix repparetofront;
    ae_int_t repfrontsize;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repnfev;
    ae_int_t repterminationtype;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    minnlcstate nlcsolver;
    minnlcreport nlcrep;
    ae_vector tmpzero;
    ae_vector tmpone;
    ae_vector tmp0;
    ae_matrix olddensec;
    ae_vector olddensect;
    ae_int_t olddensek;
    ae_vector nlcidx;
    ae_vector nlcmul;
    ae_vector nlcadd;
    ae_int_t nlcnlec;
    ae_int_t nlcnlic;
    ae_vector fideal;
    ae_matrix payoff;
    ae_vector beta;
    ae_vector delta;
    ae_vector subproblemstart;
    hqrndstate rs;
    ae_vector bndlx;
    ae_vector bndux;
    ae_matrix olddensecx;
    ae_vector x1;
    ae_vector x2;
    ae_vector fix1;
} nbistate;


/*$ Body $*/


/*************************************************************************
Initialize NBI solver with the problem formulation

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void nbiscaleandinitbuf(/* Real    */ const ae_vector* x0,
     /* Real    */ const ae_vector* s,
     ae_int_t n,
     ae_int_t m,
     ae_int_t frontsize,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     const sparsematrix* sparsea,
     /* Real    */ const ae_matrix* densea,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t ksparse,
     ae_int_t kdense,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t nnlc,
     double epsx,
     ae_int_t maxits,
     ae_bool polishsolutions,
     nbistate* state,
     ae_state *_state);


/*************************************************************************
RCOMM function

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
ae_bool nbiiteration(nbistate* state, ae_state *_state);
void _nbistate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _nbistate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _nbistate_clear(void* _p);
void _nbistate_destroy(void* _p);


/*$ End $*/
#endif

