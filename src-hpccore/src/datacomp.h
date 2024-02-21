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

#ifndef _datacomp_h
#define _datacomp_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "tsort.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "blas.h"
#include "basicstatops.h"
#include "basestat.h"
#include "clustering.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
k-means++ clusterization.
Backward compatibility function, we recommend to use CLUSTERING subpackage
as better replacement.

  -- ALGLIB --
     Copyright 21.03.2009 by Bochkanov Sergey
*************************************************************************/
void kmeansgenerate(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nvars,
     ae_int_t k,
     ae_int_t restarts,
     ae_int_t* info,
     /* Real    */ ae_matrix* c,
     /* Integer */ ae_vector* xyc,
     ae_state *_state);


/*$ End $*/
#endif

