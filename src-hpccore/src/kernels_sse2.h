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
/*************************************************************************
Copyright (c) Sergey Bochkanov (ALGLIB project).

>>> SOURCE LICENSE >>>
This software is a private code owned by ALGLIB Project.  If  you  somehow
received it without explicit written approval of ALGLIB Project,  then you
CAN NOT use it and  should  immediately  delete  all  files  bearing  this
notice.

If you did received it from ALGLIB Project, then:
* you may NOT modify it without explicit written consent of ALGLIB Project.
* you may NOT distribute it to  other  parties  without  explicit  written
  consent of ALGLIB Project.
* you may NOT  include  it  into  your  application  and  distribute  such
  applications without explicit written consent of ALGLIB Project.

In all circumstances:
* you may NOT use, copy or distribute it except as explicitly approved  by
  ALGLIB Project.
* you may NOT rent or lease it to any third party.
* you may NOT remove any copyright notice from the software.
* you may NOT disable/remove code which checks  for  presence  of  license
  keys (if such code is included in software).
>>> END OF LICENSE >>>
$NON-GPL$
*************************************************************************/
#ifndef _HALGLIB_KERNELS_SSE2_H
#define _HALGLIB_KERNELS_SSE2_H

#include "aenv.h"

/*$ Includes $*/

/*$ Body $*/
#if !defined(ALGLIB_NO_FAST_KERNELS) && defined(_ALGLIB_HAS_SSE2_INTRINSICS)

double rdotv_sse2(ae_int_t n,
     /* Real    */ const double* x,
     /* Real    */ const double* y,
     ae_state *_state);
double rdotv2_sse2(ae_int_t n,
     /* Real    */ const double* x,
     ae_state *_state);
void rcopyv_sse2(const ae_int_t n,
     /* Real    */ const double* __restrict x,
     /* Real    */ double* __restrict y,
     ae_state* __restrict _state);
void rcopymulv_sse2(const ae_int_t n,
     const double v,
     /* Real    */ const double* __restrict x,
     /* Real    */ double* __restrict y,
     const ae_state* __restrict _state);
void icopyv_sse2(const ae_int_t n, const ae_int_t* __restrict x,
                ae_int_t* __restrict y, ae_state* __restrict _state);
void bcopyv_sse2(const ae_int_t n, const ae_bool* __restrict x,
                ae_bool* __restrict y, ae_state* __restrict _state);
void rsetv_sse2(const ae_int_t n,
     const double v,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state);
void rsetvx_sse2(const ae_int_t n,
     const double v,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state);
void isetv_sse2(const ae_int_t n, const ae_int_t v,
    ae_int_t* __restrict x, ae_state* __restrict _state);
void bsetv_sse2(const ae_int_t n, const ae_bool v, ae_bool* __restrict x,
    ae_state* __restrict _state);
void rmulv_sse2(const ae_int_t n, const double v, double* __restrict x,
     const ae_state* __restrict _state);
void rmulvx_sse2(const ae_int_t n, const double v, double* __restrict x,
    const ae_state* __restrict _state);
void raddv_sse2(const ae_int_t n,
     const double alpha,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state);
void raddvx_sse2(const ae_int_t n, const double alpha,
    const double* __restrict y, double* __restrict x, ae_state *_state);
void rmergemulv_sse2(const ae_int_t n,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state);
void rmergemaxv_sse2(const ae_int_t n,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     ae_state* __restrict _state);
void rmergeminv_sse2(const ae_int_t n,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     ae_state* __restrict _state);
double rmaxv_sse2(ae_int_t n, /* Real    */ const double* __restrict x, ae_state* __restrict _state);
double rmaxabsv_sse2(ae_int_t n, /* Real    */ const double* __restrict x, ae_state* __restrict _state);
void rcopyvx_sse2(const ae_int_t n, const double* __restrict x,
    double* __restrict y, ae_state *_state);
void icopyvx_sse2(const ae_int_t n, const ae_int_t* __restrict x,
                ae_int_t* __restrict y, ae_state* __restrict _state);
/* ALGLIB_NO_FAST_KERNELS, _ALGLIB_HAS_SSE2_INTRINSICS */
#endif
/*$ End $*/
#endif