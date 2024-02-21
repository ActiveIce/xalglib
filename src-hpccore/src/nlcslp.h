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

#ifndef _nlcslp_h
#define _nlcslp_h

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
#include "lpqppresolve.h"
#include "reviseddualsimplex.h"


/*$ Declarations $*/


/*************************************************************************
This object stores temporaries of SLP subsolver.
*************************************************************************/
typedef struct
{
    presolveinfo presolver;
    dualsimplexstate dss;
    dualsimplexsettings dsssettings;
    dualsimplexbasis lastbasis;
    ae_bool basispresent;
    ae_matrix curd;
    ae_int_t curdcnt;
    ae_vector curb;
    ae_vector curbndl;
    ae_vector curbndu;
    ae_vector cural;
    ae_vector curau;
    sparsematrix sparserawlc;
    sparsematrix sparseefflc;
    xbfgshessian hess;
    ae_matrix curhd;
    ae_matrix densedummy;
    sparsematrix sparsedummy;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector sk;
    ae_vector yk;
    ae_vector xs;
    ae_vector laglc;
    ae_vector lagbc;
    ae_vector cs;
} minslpsubsolver;


/*************************************************************************
This object stores temporaries for LagrangianFG() function
*************************************************************************/
typedef struct
{
    ae_vector sclagtmp0;
    ae_vector sclagtmp1;
} minslptmplagrangian;


/*************************************************************************
This object stores temporaries for LagrangianFG() function
*************************************************************************/
typedef struct
{
    ae_vector mftmp0;
} minslptmpmerit;


/*************************************************************************
This object stores temporaries of Phase13 SLP subsolver.
*************************************************************************/
typedef struct
{
    ae_bool usecorrection;
    ae_vector d;
    ae_vector dx;
    ae_vector stepkxc;
    ae_vector stepkxn;
    ae_vector stepkfic;
    ae_vector stepkfin;
    ae_matrix stepkjc;
    ae_matrix stepkjn;
    ae_vector dummylagmult;
    minslptmpmerit tmpmerit;
    rcommstate rphase13state;
} minslpphase13state;


/*************************************************************************
This object stores temporaries of Phase13 SLP subsolver.
*************************************************************************/
typedef struct
{
    ae_vector stepkxn;
    ae_vector stepkxc;
    ae_vector stepkfin;
    ae_vector stepkfic;
    ae_matrix stepkjn;
    ae_matrix stepkjc;
    ae_vector stepklaggrad;
    ae_vector stepknlaggrad;
    ae_vector stepknlagmult;
    ae_vector meritlagmult;
    minslptmplagrangian tmplagrangianfg;
    double lastlcerr;
    ae_int_t lastlcidx;
    double lastnlcerr;
    ae_int_t lastnlcidx;
    ae_vector tmp0;
    ae_vector d;
    linminstate mcstate;
    minslptmpmerit tmpmerit;
    rcommstate rphase2state;
} minslpphase2state;


/*************************************************************************
This object stores temporaries of SLP solver.
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t nlec;
    ae_int_t nlic;
    ae_vector s;
    ae_matrix scaledcleic;
    ae_vector lcsrcidx;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector scaledbndl;
    ae_vector scaledbndu;
    double epsx;
    ae_int_t maxits;
    ae_vector x;
    ae_vector fi;
    ae_matrix j;
    double f;
    ae_bool needfij;
    ae_bool xupdated;
    minslpphase13state state13;
    minslpphase2state state2;
    double trustrad;
    double bigc;
    ae_int_t lpfailurecnt;
    ae_int_t fstagnationcnt;
    ae_vector step0x;
    ae_vector stepkx;
    ae_vector backupx;
    ae_vector step0fi;
    ae_vector stepkfi;
    ae_vector backupfi;
    ae_matrix step0j;
    ae_matrix stepkj;
    ae_matrix backupj;
    ae_vector meritlagmult;
    ae_vector dummylagmult;
    ae_vector fscales;
    ae_vector meritfunctionhistory;
    ae_vector maxlaghistory;
    ae_int_t historylen;
    minslpsubsolver subsolver;
    minslptmpmerit tmpmerit;
    ae_int_t repsimplexiterations;
    ae_int_t repsimplexiterations1;
    ae_int_t repsimplexiterations2;
    ae_int_t repsimplexiterations3;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repterminationtype;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    rcommstate rstate;
} minslpstate;


/*$ Body $*/


void minslpinitbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n,
     /* Real    */ const ae_matrix* cleic,
     /* Integer */ const ae_vector* lcsrcidx,
     ae_int_t nec,
     ae_int_t nic,
     ae_int_t nlec,
     ae_int_t nlic,
     double epsx,
     ae_int_t maxits,
     minslpstate* state,
     ae_state *_state);


/*************************************************************************
This function performs actual processing for  SLP  algorithm.  It  expects
that caller redirects its reverse communication  requests NeedFiJ/XUpdated
to external user who will provide analytic derivative (or  handle  reports
about progress).

In case external user does not have analytic derivative, it is responsibility
of caller to intercept NeedFiJ request and  replace  it  with  appropriate
numerical differentiation scheme.

Results are stored:
* point - in State.StepKX

IMPORTANT: this function works with scaled problem formulation; it is
           responsibility of the caller to unscale request and scale
           Jacobian.
           
NOTE: SMonitor is expected to be correctly initialized smoothness monitor.

  -- ALGLIB --
     Copyright 05.03.2018 by Bochkanov Sergey
*************************************************************************/
ae_bool minslpiteration(minslpstate* state,
     smoothnessmonitor* smonitor,
     ae_bool userterminationneeded,
     ae_state *_state);
void _minslpsubsolver_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minslpsubsolver_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minslpsubsolver_clear(void* _p);
void _minslpsubsolver_destroy(void* _p);
void _minslptmplagrangian_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minslptmplagrangian_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minslptmplagrangian_clear(void* _p);
void _minslptmplagrangian_destroy(void* _p);
void _minslptmpmerit_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minslptmpmerit_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minslptmpmerit_clear(void* _p);
void _minslptmpmerit_destroy(void* _p);
void _minslpphase13state_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minslpphase13state_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minslpphase13state_clear(void* _p);
void _minslpphase13state_destroy(void* _p);
void _minslpphase2state_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minslpphase2state_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minslpphase2state_clear(void* _p);
void _minslpphase2state_destroy(void* _p);
void _minslpstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minslpstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minslpstate_clear(void* _p);
void _minslpstate_destroy(void* _p);


/*$ End $*/
#endif

