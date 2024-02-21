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

#ifndef _mincomp_h
#define _mincomp_h

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
#include "minlbfgs.h"
#include "cqmodels.h"
#include "snnls.h"
#include "sactivesets.h"
#include "minbleic.h"


/*$ Declarations $*/


typedef struct
{
    ae_int_t n;
    double epsg;
    double epsf;
    double epsx;
    ae_int_t maxits;
    ae_bool xrep;
    double stpmax;
    ae_int_t cgtype;
    ae_int_t k;
    ae_int_t nfev;
    ae_int_t mcstage;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t curalgo;
    ae_int_t acount;
    double mu;
    double finit;
    double dginit;
    ae_vector ak;
    ae_vector xk;
    ae_vector dk;
    ae_vector an;
    ae_vector xn;
    ae_vector dn;
    ae_vector d;
    double fold;
    double stp;
    ae_vector work;
    ae_vector yk;
    ae_vector gc;
    double laststep;
    ae_vector x;
    double f;
    ae_vector g;
    ae_bool needfg;
    ae_bool xupdated;
    rcommstate rstate;
    ae_int_t repiterationscount;
    ae_int_t repnfev;
    ae_int_t repterminationtype;
    ae_int_t debugrestartscount;
    linminstate lstate;
    double betahs;
    double betady;
} minasastate;


typedef struct
{
    ae_int_t iterationscount;
    ae_int_t nfev;
    ae_int_t terminationtype;
    ae_int_t activeconstraints;
} minasareport;


/*$ Body $*/


/*************************************************************************
Obsolete function, use MinLBFGSSetPrecDefault() instead.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetdefaultpreconditioner(minlbfgsstate* state,
     ae_state *_state);


/*************************************************************************
Obsolete function, use MinLBFGSSetCholeskyPreconditioner() instead.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetcholeskypreconditioner(minlbfgsstate* state,
     /* Real    */ const ae_matrix* p,
     ae_bool isupper,
     ae_state *_state);


/*************************************************************************
This is obsolete function which was used by previous version of the  BLEIC
optimizer. It does nothing in the current version of BLEIC.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetbarrierwidth(minbleicstate* state,
     double mu,
     ae_state *_state);


/*************************************************************************
This is obsolete function which was used by previous version of the  BLEIC
optimizer. It does nothing in the current version of BLEIC.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetbarrierdecay(minbleicstate* state,
     double mudecay,
     ae_state *_state);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 25.03.2010 by Bochkanov Sergey
*************************************************************************/
void minasacreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     minasastate* state,
     ae_state *_state);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minasasetcond(minasastate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minasasetxrep(minasastate* state, ae_bool needxrep, ae_state *_state);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minasasetalgorithm(minasastate* state,
     ae_int_t algotype,
     ae_state *_state);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minasasetstpmax(minasastate* state, double stpmax, ae_state *_state);


/*************************************************************************

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
ae_bool minasaiteration(minasastate* state, ae_state *_state);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void minasaresults(const minasastate* state,
     /* Real    */ ae_vector* x,
     minasareport* rep,
     ae_state *_state);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void minasaresultsbuf(const minasastate* state,
     /* Real    */ ae_vector* x,
     minasareport* rep,
     ae_state *_state);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void minasarestartfrom(minasastate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);


/*************************************************************************
Set V1 reverse communication protocol
*************************************************************************/
void minasasetprotocolv1(minasastate* state, ae_state *_state);
void _minasastate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minasastate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minasastate_clear(void* _p);
void _minasastate_destroy(void* _p);
void _minasareport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minasareport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minasareport_clear(void* _p);
void _minasareport_destroy(void* _p);


/*$ End $*/
#endif

