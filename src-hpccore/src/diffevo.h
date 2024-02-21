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

#ifndef _diffevo_h
#define _diffevo_h

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
This object stores a stream of pseudorandom numbers. Streams are used to
quickly generate good-enough sets of reusable random numbers.

NOT INTENDED AS A GENERAL PURPOSE RNG! Use with caution!!!
*************************************************************************/
typedef struct
{
    ae_int_t streamsize;
    ae_int_t streampos;
    ae_int_t maxval;
    ae_int_t maxsize;
    ae_vector vals;
} gdemointstream;


/*************************************************************************
This object stores a stream of pseudorandom numbers. Streams are used to
quickly generate good-enough sets of random numbers.

NOT INTENDED AS A GENERAL PURPOSE RNG! Use with caution!!!
*************************************************************************/
typedef struct
{
    ae_int_t streamsize;
    ae_int_t streampos;
    ae_int_t maxsize;
    ae_vector vals0;
    ae_vector vals1;
} gdemorealstream;


/*************************************************************************
This object stores temporaries of GDEMO solver.
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t cntlc;
    ae_int_t cntnlc;
    ae_vector s;
    ae_bool hasx0;
    ae_vector x0;
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
    ae_int_t popsize;
    ae_int_t epochscnt;
    double crossoverprob;
    double diffweight;
    ae_int_t constrmode;
    double rho1;
    double rho2;
    double eps;
    double nsample;
    ae_bool fixedparams;
    ae_int_t fixedstrategy;
    double fixedcrossoverprob;
    double fixeddifferentialweight;
    ae_bool stoponsmallf;
    double smallf;
    ae_int_t requesttype;
    ae_vector reportx;
    double reportf;
    ae_int_t querysize;
    ae_vector querydata;
    ae_vector replyfi;
    rcommstate rstate;
    hqrndstate rs;
    ae_matrix population2x;
    ae_matrix population2rawreplies;
    ae_matrix population2fitness;
    ae_matrix population2params;
    ae_matrix candidatesx;
    ae_matrix candidatesrawreplies;
    ae_matrix candidatesfitness;
    ae_matrix candidatesparams;
    ae_matrix paramspool;
    ae_int_t paramspoolsize;
    ae_int_t poolinsertpos;
    ae_matrix xbest;
    ae_vector fbest;
    ae_int_t repterminationtype;
    ae_int_t repiterationscount;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    ae_vector tmpzero;
    sparsematrix dummysparsea;
} gdemostate;


/*$ Body $*/


void gdemoinitbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t cntlc,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t cntnlc,
     ae_int_t popsize,
     ae_int_t epochscnt,
     ae_int_t seed,
     gdemostate* state,
     ae_state *_state);


/*************************************************************************
Set fixed algorithm params (the default is to use adaptive algorithm)

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
void gdemosetfixedparams(gdemostate* state,
     ae_int_t strategy,
     double crossoverprob,
     double differentialweight,
     ae_state *_state);


/*************************************************************************
Sets small F, the solver stops when the best value decreases below this level

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
void gdemosetsmallf(gdemostate* state, double f, ae_state *_state);


/*************************************************************************
Sets constraint handling mode to L1/L2 penalty

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
void gdemosetmodepenalty(gdemostate* state,
     double rhol1,
     double rhol2,
     ae_state *_state);


/*************************************************************************
Sets initial point

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
void gdemosetx0(gdemostate* state,
     /* Real    */ const ae_vector* x0,
     ae_state *_state);


/*************************************************************************
This function performs actual processing for GDEMO  algorithm.

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
ae_bool gdemoiteration(gdemostate* state,
     ae_bool userterminationneeded,
     ae_state *_state);
void _gdemointstream_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _gdemointstream_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _gdemointstream_clear(void* _p);
void _gdemointstream_destroy(void* _p);
void _gdemorealstream_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _gdemorealstream_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _gdemorealstream_clear(void* _p);
void _gdemorealstream_destroy(void* _p);
void _gdemostate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _gdemostate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _gdemostate_clear(void* _p);
void _gdemostate_destroy(void* _p);


/*$ End $*/
#endif

