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

#ifndef _intcomp_h
#define _intcomp_h

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
#include "cqmodels.h"
#include "snnls.h"
#include "sactivesets.h"
#include "minbleic.h"
#include "xblas.h"
#include "directdensesolvers.h"
#include "normestimator.h"
#include "linlsqr.h"
#include "minlbfgs.h"
#include "lpqpserv.h"
#include "qqpsolver.h"
#include "qpdenseaulsolver.h"
#include "qpbleicsolver.h"
#include "vipmsolver.h"
#include "ipm2solver.h"
#include "minqp.h"
#include "minlm.h"
#include "lpqppresolve.h"
#include "reviseddualsimplex.h"
#include "nlcslp.h"
#include "nlcsqp.h"
#include "nlcfsqp.h"
#include "nlcaul.h"
#include "minnlc.h"
#include "fitsphere.h"
#include "intfitserv.h"
#include "spline1d.h"
#include "ratint.h"
#include "polint.h"
#include "lsfit.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
This function is left for backward compatibility.
Use fitspheremc() instead.

                                    
  -- ALGLIB --
     Copyright 14.04.2017 by Bochkanov Sergey
*************************************************************************/
void nsfitspheremcc(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nx,
     /* Real    */ ae_vector* cx,
     double* rhi,
     ae_state *_state);


/*************************************************************************
This function is left for backward compatibility.
Use fitspheremi() instead.
                                    
  -- ALGLIB --
     Copyright 14.04.2017 by Bochkanov Sergey
*************************************************************************/
void nsfitspheremic(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nx,
     /* Real    */ ae_vector* cx,
     double* rlo,
     ae_state *_state);


/*************************************************************************
This function is left for backward compatibility.
Use fitspheremz() instead.
                                    
  -- ALGLIB --
     Copyright 14.04.2017 by Bochkanov Sergey
*************************************************************************/
void nsfitspheremzc(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nx,
     /* Real    */ ae_vector* cx,
     double* rlo,
     double* rhi,
     ae_state *_state);


/*************************************************************************
This function is left for backward compatibility.
Use fitspherex() instead.
                                    
  -- ALGLIB --
     Copyright 14.04.2017 by Bochkanov Sergey
*************************************************************************/
void nsfitspherex(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nx,
     ae_int_t problemtype,
     double epsx,
     ae_int_t aulits,
     double penalty,
     /* Real    */ ae_vector* cx,
     double* rlo,
     double* rhi,
     ae_state *_state);


/*************************************************************************
This function is an obsolete and deprecated version of fitting by
penalized cubic spline.

It was superseded by spline1dfit(), which is an orders of magnitude faster
and more memory-efficient implementation.

Do NOT use this function in the new code!

  -- ALGLIB PROJECT --
     Copyright 18.08.2009 by Bochkanov Sergey
*************************************************************************/
void spline1dfitpenalized(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     ae_int_t n,
     ae_int_t m,
     double rho,
     ae_int_t* info,
     spline1dinterpolant* s,
     spline1dfitreport* rep,
     ae_state *_state);


/*************************************************************************
This function is an obsolete and deprecated version of fitting by
penalized cubic spline.

It was superseded by spline1dfit(), which is an orders of magnitude faster
and more memory-efficient implementation.

Do NOT use this function in the new code!

  -- ALGLIB PROJECT --
     Copyright 19.10.2010 by Bochkanov Sergey
*************************************************************************/
void spline1dfitpenalizedw(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     /* Real    */ const ae_vector* _w,
     ae_int_t n,
     ae_int_t m,
     double rho,
     ae_int_t* info,
     spline1dinterpolant* s,
     spline1dfitreport* rep,
     ae_state *_state);


/*************************************************************************
Deprecated fitting function with O(N*M^2+M^3) running time. Superseded  by
spline1dfit().

  -- ALGLIB PROJECT --
     Copyright 18.08.2009 by Bochkanov Sergey
*************************************************************************/
void spline1dfitcubic(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     ae_int_t m,
     spline1dinterpolant* s,
     spline1dfitreport* rep,
     ae_state *_state);


/*************************************************************************
Deprecated fitting function with O(N*M^2+M^3) running time. Superseded  by
spline1dfit().

  -- ALGLIB PROJECT --
     Copyright 18.08.2009 by Bochkanov Sergey
*************************************************************************/
void spline1dfithermite(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     ae_int_t m,
     spline1dinterpolant* s,
     spline1dfitreport* rep,
     ae_state *_state);


/*$ End $*/
#endif

