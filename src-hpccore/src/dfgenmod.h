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

#ifndef _dfgenmod_h
#define _dfgenmod_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "linmin.h"
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
#include "optguardapi.h"
#include "optserv.h"
#include "xblas.h"
#include "directdensesolvers.h"
#include "fbls.h"
#include "minlbfgs.h"
#include "cqmodels.h"
#include "lpqpserv.h"
#include "vipmsolver.h"


/*$ Declarations $*/


/*************************************************************************
2PS model
*************************************************************************/
typedef struct
{
    ae_vector deltas;
    ae_matrix jac;
    ae_vector g;
    ae_matrix q;
} df2psmodel;


/*************************************************************************
DFO-LSA model
*************************************************************************/
typedef struct
{
    ae_matrix w;
    ae_matrix invw;
    ae_matrix jac;
    ae_vector g;
    ae_matrix q;
    ae_vector tmp0;
    ae_matrix tmprhs;
} dfolsamodel;


/*************************************************************************
This object stores temporaries of the general model-based derivative-free
solver
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t cntlc;
    ae_int_t cntnlc;
    ae_bool isleastsquares;
    ae_int_t modeltype;
    double rad0;
    ae_int_t nnoisyrestarts;
    ae_vector x0;
    ae_vector s;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector scaledbndl;
    ae_vector scaledbndu;
    ae_vector finitebndl;
    ae_vector finitebndu;
    double epsx;
    ae_int_t maxits;
    ae_int_t maxfev;
    double toosmalltrustrad;
    ae_int_t requesttype;
    ae_vector reportx;
    double reportf;
    ae_int_t querysize;
    ae_vector querydata;
    ae_vector replyfi;
    rcommstate rstate;
    rcommstate rstateimprove;
    rcommstate rstateupdate;
    ae_bool dotrace;
    ae_vector trustregion;
    ae_vector invtrustregion;
    double trustradfactor;
    double trustradbnd;
    ae_vector xp;
    ae_vector fpvec;
    double fp;
    double hp;
    ae_vector xk;
    ae_vector sk;
    ae_vector xn;
    ae_vector fkvec;
    ae_vector fnvec;
    double fk;
    double hk;
    ae_matrix wrkset;
    ae_int_t wrksetsize;
    hqrndstate rs;
    df2psmodel tpsmodel;
    dfolsamodel lsamodel;
    ae_bool infinitiesencountered;
    ae_bool recoveredfrominfinities;
    ae_int_t restartstoperform;
    double lasttrustrad;
    ae_matrix lastjac;
    ae_vector trustradhistory;
    ae_vector jacdiffhistory;
    ae_vector iteridxhistory;
    ae_int_t historylen;
    ae_int_t historynext;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
    ae_vector tmpdeltas;
    vipmstate ipmsolver;
    ae_matrix densedummy;
    sparsematrix sparsedummy;
    matinvreport invrep;
    ae_int_t repterminationtype;
    ae_int_t repiterationscount;
    ae_int_t repnfev;
} dfgmstate;


/*$ Body $*/


/*************************************************************************
Initialize derivative-free general model-based solver (DFGM)

IMPORTANT: the solver does NOT support fixed variables. The caller has  to
           preprocess the problem in order to drop all fixed vars.
*************************************************************************/
void dfgminitbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n,
     ae_int_t m,
     ae_bool isls,
     ae_int_t modeltype,
     const nlpstoppingcriteria* criteria,
     ae_int_t nnoisyrestarts,
     double rad0,
     ae_int_t maxfev,
     dfgmstate* state,
     ae_state *_state);


/*************************************************************************

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
ae_bool dfgmiteration(dfgmstate* state,
     ae_bool userterminationneeded,
     ae_state *_state);


void linregline(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     double* a,
     double* b,
     double* corrxy,
     ae_state *_state);
void _df2psmodel_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _df2psmodel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _df2psmodel_clear(void* _p);
void _df2psmodel_destroy(void* _p);
void _dfolsamodel_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dfolsamodel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dfolsamodel_clear(void* _p);
void _dfolsamodel_destroy(void* _p);
void _dfgmstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dfgmstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dfgmstate_clear(void* _p);
void _dfgmstate_destroy(void* _p);


/*$ End $*/
#endif

