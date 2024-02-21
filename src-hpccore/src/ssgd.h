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

#ifndef _ssgd_h
#define _ssgd_h

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
#include "lpqpserv.h"


/*$ Declarations $*/


/*************************************************************************
This object stores temporaries of SSGD solver.
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t cntlc;
    ae_int_t cntnlc;
    ae_vector x0;
    ae_vector s;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector finitebndl;
    ae_vector finitebndu;
    ae_matrix densea;
    ae_vector al;
    ae_vector au;
    ae_vector hasal;
    ae_vector hasau;
    ae_vector rawnl;
    ae_vector rawnu;
    ae_vector hasnl;
    ae_vector hasnu;
    ae_vector ascales;
    ae_int_t maxits;
    double rate0;
    double rate1;
    double momentum;
    double blur0;
    double blur1;
    ae_int_t outerits;
    double rho;
    ae_vector x;
    double f;
    ae_vector fi;
    ae_bool needfi;
    ae_bool xupdated;
    ae_vector avgsqj;
    ae_vector fscales;
    hqrndstate rs;
    ae_vector xbest;
    ae_vector fibest;
    ae_vector xcur;
    ae_vector gcur;
    ae_vector dcur;
    double currate;
    double curblur;
    double zcursecantstep;
    ae_int_t repiterationscount;
    ae_int_t repnfev;
    rcommstate rstate;
    ae_vector sqj;
    ae_vector xleft;
    ae_vector xright;
    ae_vector fileft;
    ae_vector firight;
    ae_vector xoffs0;
    ae_vector xoffs1;
    ae_vector tmpzero;
    sparsematrix dummysparsea;
} ssgdstate;


/*$ Body $*/


void ssgdinitbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t cntlc,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t cntnlc,
     double rad0,
     double rad1,
     ae_int_t outerits,
     double rate0,
     double rate1,
     double momentum,
     ae_int_t maxits,
     double rho,
     ssgdstate* state,
     ae_state *_state);


/*************************************************************************
This function performs actual processing for SSGD  algorithm.  It  expects
that the caller redirects its reverse communication  requests  NeedFiJ  or
XUpdated to the external user,  possibly  wrapping  them  in  a  numerical
differentiation code.

  -- ALGLIB --
     Copyright 26.07.2023 by Bochkanov Sergey
*************************************************************************/
ae_bool ssgditeration(ssgdstate* state,
     ae_bool userterminationneeded,
     ae_state *_state);
void _ssgdstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _ssgdstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _ssgdstate_clear(void* _p);
void _ssgdstate_destroy(void* _p);


/*$ End $*/
#endif

