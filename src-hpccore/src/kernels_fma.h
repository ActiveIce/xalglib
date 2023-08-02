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
#ifndef _HALGLIB_KERNELS_FMA_H
#define _HALGLIB_KERNELS_FMA_H

#include "aenv.h"

/*$ Includes $*/

/*$ Body $*/
#if !defined(ALGLIB_NO_FAST_KERNELS) && defined(_ALGLIB_HAS_AVX2_INTRINSICS)

double rdotv_fma(const ae_int_t n,
    /* Real    */ const double* __restrict x,
    /* Real    */ const double* __restrict y,
    const ae_state* __restrict _state);
double rdotv2_fma(const ae_int_t n,
    /* Real    */ const double* __restrict x,
    const ae_state* __restrict _state);
void raddv_fma(const ae_int_t n,
     const double alpha,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state);
void raddvx_fma(const ae_int_t n, const double alpha, const double* __restrict y, double* __restrict x, ae_state *_state);
void rmuladdv_fma(const ae_int_t n,
     const double* __restrict y,
     const double* __restrict z,
     double* __restrict x,
     const ae_state* _state);
void rnegmuladdv_fma(const ae_int_t n,
     const double* __restrict y,
     const double* __restrict z,
     double* __restrict x,
     const ae_state* _state);
void rcopymuladdv_fma(const ae_int_t n,
     const double* __restrict y,
     const double* __restrict z,
     const double* __restrict x,
     double* __restrict r,
     const ae_state* _state);
void rcopynegmuladdv_fma(const ae_int_t n,
     const double* __restrict y,
     const double* __restrict z,
     const double* __restrict x,
     double* __restrict r,
     const ae_state* _state);
void rgemv_straight_fma(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a,
    const double* __restrict x, double* __restrict y, ae_state* _state);
void rgemv_transposed_fma(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a,
    const double* __restrict x, double* __restrict y, ae_state* _state);
void rgemvx_straight_fma(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a, const ae_int_t ia,
    const ae_int_t ja, const double* __restrict x,
    double* __restrict y, ae_state* _state);
void rgemvx_transposed_fma(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a, const ae_int_t ia,
    const ae_int_t ja, const double* __restrict x, double* __restrict y,
    ae_state* _state);

void ablasf_dotblkh_fma(
    const double *src_a,
    const double *src_b,
    ae_int_t round_length,
    ae_int_t block_size,
    ae_int_t micro_size,
    double *dst,
    ae_int_t dst_stride);
void spchol_propagatefwd_fma(/* Real    */ const ae_vector* x,
     ae_int_t cols0,
     ae_int_t blocksize,
     /* Integer */ const ae_vector* superrowidx,
     ae_int_t rbase,
     ae_int_t offdiagsize,
     /* Real    */ const ae_vector* rowstorage,
     ae_int_t offss,
     ae_int_t sstride,
     /* Real    */ ae_vector* simdbuf,
     ae_int_t simdwidth,
     ae_state *_state);
ae_bool spchol_updatekernelabc4_fma(double* rowstorage,
     ae_int_t offss,
     ae_int_t twidth,
     ae_int_t offsu,
     ae_int_t uheight,
     ae_int_t urank,
     ae_int_t urowstride,
     ae_int_t uwidth,
     const double* diagd,
     ae_int_t offsd,
     const ae_int_t* raw2smap,
     const ae_int_t* superrowidx,
     ae_int_t urbase,
     ae_state *_state);
ae_bool spchol_updatekernel4444_fma(
     double*  rowstorage,
     ae_int_t offss,
     ae_int_t sheight,
     ae_int_t offsu,
     ae_int_t uheight,
     const double*  diagd,
     ae_int_t offsd,
     const ae_int_t* raw2smap,
     const ae_int_t* superrowidx,
     ae_int_t urbase,
     ae_state *_state);


/* ALGLIB_NO_FAST_KERNELS, _ALGLIB_HAS_AVX2_INTRINSICS */
#endif
/*$ End $*/
#endif