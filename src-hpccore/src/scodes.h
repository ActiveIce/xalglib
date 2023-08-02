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

#ifndef _scodes_h
#define _scodes_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"


/*$ Declarations $*/


/*$ Body $*/


ae_int_t getrdfserializationcode(ae_state *_state);


ae_int_t getkdtreeserializationcode(ae_state *_state);


ae_int_t getmlpserializationcode(ae_state *_state);


ae_int_t getmlpeserializationcode(ae_state *_state);


ae_int_t getrbfserializationcode(ae_state *_state);


ae_int_t getspline2dserializationcode(ae_state *_state);


ae_int_t getidwserializationcode(ae_state *_state);


ae_int_t getsparsematrixserializationcode(ae_state *_state);


ae_int_t getspline2dwithmissingnodesserializationcode(ae_state *_state);


ae_int_t getspline1dserializationcode(ae_state *_state);


ae_int_t getknnserializationcode(ae_state *_state);


ae_int_t getlptestserializationcode(ae_state *_state);


/*$ End $*/
#endif

