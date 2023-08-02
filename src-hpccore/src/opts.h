###########################################################################
# ALGLIB 4.00.0 (source code generated 2023-05-21)
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

#ifndef _opts_h
#define _opts_h

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
#include "reviseddualsimplex.h"
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
#include "vipmsolver.h"
#include "minlp.h"


/*$ Declarations $*/


/*************************************************************************
This is a test problem class  intended  for  internal  performance  tests.
Never use it directly in your projects.

*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_bool hasknowntarget;
    double targetf;
    ae_vector s;
    ae_vector c;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t m;
    sparsematrix a;
    ae_vector al;
    ae_vector au;
} lptestproblem;


/*$ Body $*/


/*************************************************************************
Initialize test LP problem.

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemcreate(ae_int_t n,
     ae_bool hasknowntarget,
     double targetf,
     lptestproblem* p,
     ae_state *_state);


/*************************************************************************
Query test problem info

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
ae_bool lptestproblemhasknowntarget(lptestproblem* p, ae_state *_state);


/*************************************************************************
Query test problem info

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
double lptestproblemgettargetf(lptestproblem* p, ae_state *_state);


/*************************************************************************
Query test problem info

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
ae_int_t lptestproblemgetn(lptestproblem* p, ae_state *_state);


/*************************************************************************
Query test problem info

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
ae_int_t lptestproblemgetm(lptestproblem* p, ae_state *_state);


/*************************************************************************
Set scale for test LP problem

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemsetscale(lptestproblem* p,
     /* Real    */ const ae_vector* s,
     ae_state *_state);


/*************************************************************************
Set cost for test LP problem

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemsetcost(lptestproblem* p,
     /* Real    */ const ae_vector* c,
     ae_state *_state);


/*************************************************************************
Set box constraints for test LP problem

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemsetbc(lptestproblem* p,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);


/*************************************************************************
Set box constraints for test LP problem

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemsetlc2(lptestproblem* p,
     const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t m,
     ae_state *_state);


/*************************************************************************
Serializer: allocation

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemalloc(ae_serializer* s,
     const lptestproblem* p,
     ae_state *_state);


/*************************************************************************
Serializer: serialization

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemserialize(ae_serializer* s,
     const lptestproblem* p,
     ae_state *_state);


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemunserialize(ae_serializer* s,
     lptestproblem* p,
     ae_state *_state);


/*************************************************************************
This is internal function intended to  be  used  only  by  ALGLIB  itself.
Although for technical reasons it is made publicly available (and has  its
own manual entry), you should never call it.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void xdbgminlpcreatefromtestproblem(const lptestproblem* p,
     minlpstate* state,
     ae_state *_state);
void _lptestproblem_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _lptestproblem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _lptestproblem_clear(void* _p);
void _lptestproblem_destroy(void* _p);


/*$ End $*/
#endif

