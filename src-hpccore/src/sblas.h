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

#ifndef _sblas_h
#define _sblas_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"


/*$ Declarations $*/


/*$ Body $*/


void symmetricmatrixvectormultiply(/* Real    */ const ae_matrix* a,
     ae_bool isupper,
     ae_int_t i1,
     ae_int_t i2,
     /* Real    */ const ae_vector* x,
     double alpha,
     /* Real    */ ae_vector* y,
     ae_state *_state);


void symmetricrank2update(/* Real    */ ae_matrix* a,
     ae_bool isupper,
     ae_int_t i1,
     ae_int_t i2,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* t,
     double alpha,
     ae_state *_state);


/*$ End $*/
#endif

