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

#ifndef _dlu_h
#define _dlu_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "ablasmkl.h"
#include "ablas.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Recurrent complex LU subroutine.
Never call it directly.

  -- ALGLIB routine --
     04.01.2010
     Bochkanov Sergey
*************************************************************************/
void cmatrixluprec(/* Complex */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     /* Complex */ ae_vector* tmp,
     ae_state *_state);


/*************************************************************************
Recurrent real LU subroutine.
Never call it directly.

  -- ALGLIB routine --
     04.01.2010
     Bochkanov Sergey
*************************************************************************/
void rmatrixluprec(/* Real    */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     /* Real    */ ae_vector* tmp,
     ae_state *_state);


/*************************************************************************
Recurrent complex LU subroutine.
Never call it directly.

  -- ALGLIB routine --
     04.01.2010
     Bochkanov Sergey
*************************************************************************/
void cmatrixplurec(/* Complex */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     /* Complex */ ae_vector* tmp,
     ae_state *_state);


/*************************************************************************
Recurrent real LU subroutine.
Never call it directly.

  -- ALGLIB routine --
     04.01.2010
     Bochkanov Sergey
*************************************************************************/
void rmatrixplurec(/* Real    */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     /* Real    */ ae_vector* tmp,
     ae_state *_state);


/*$ End $*/
#endif

