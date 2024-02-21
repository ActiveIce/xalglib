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

#ifndef _basicstatops_h
#define _basicstatops_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "tsort.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Internal tied ranking subroutine.

INPUT PARAMETERS:
    X       -   array to rank
    N       -   array size
    IsCentered- whether ranks are centered or not:
                * True      -   ranks are centered in such way that  their
                                sum is zero
                * False     -   ranks are not centered
    Buf     -   temporary buffers
    
NOTE: when IsCentered is True and all X[] are equal, this  function  fills
      X by zeros (exact zeros are used, not sum which is only approximately
      equal to zero).
*************************************************************************/
void rankx(/* Real    */ ae_vector* x,
     ae_int_t n,
     ae_bool iscentered,
     apbuffers* buf,
     ae_state *_state);


/*************************************************************************
Internal untied ranking subroutine.

INPUT PARAMETERS:
    X       -   array to rank
    N       -   array size
    Buf     -   temporary buffers

Returns untied ranks (in case of a tie ranks are resolved arbitrarily).
*************************************************************************/
void rankxuntied(/* Real    */ ae_vector* x,
     ae_int_t n,
     apbuffers* buf,
     ae_state *_state);


/*$ End $*/
#endif

