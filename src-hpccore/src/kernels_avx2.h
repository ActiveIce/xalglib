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
#ifndef _HALGLIB_KERNELS_AVX2_H
#define _HALGLIB_KERNELS_AVX2_H

#include "aenv.h"

/*$ Includes $*/

/*$ Body $*/
#if !defined(ALGLIB_NO_FAST_KERNELS) && defined(_ALGLIB_HAS_AVX2_INTRINSICS)

double rdotv_avx2(const ae_int_t n,
    /* Real    */ const double* __restrict x,
    /* Real    */ const double* __restrict y,
    const ae_state* __restrict _state);
double rdotv2_avx2(const ae_int_t n,
    /* Real    */ const double* __restrict x,
    const ae_state* __restrict _state);
void rcopyv_avx2(ae_int_t n,
     /* Real    */ const double* __restrict x,
     /* Real    */ double* __restrict y,
     ae_state* __restrict _state);
void rcopymulv_avx2(const ae_int_t n,
     const double v,
     /* Real    */ const double* __restrict x,
     /* Real    */ double* __restrict y,
     const ae_state* __restrict _state);
void icopyv_avx2(const ae_int_t n, const ae_int_t* __restrict x,
                ae_int_t* __restrict y, ae_state* __restrict _state);
void bcopyv_avx2(const ae_int_t n, const ae_bool* __restrict x,
                ae_bool* __restrict y, ae_state* __restrict _state);
void rsetv_avx2(const ae_int_t n,
     const double v,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state);
void rsetvx_avx2(const ae_int_t n, const double v, double* __restrict x,
    const ae_state* __restrict _state);
void isetv_avx2(const ae_int_t n, const ae_int_t v,
    ae_int_t* __restrict x, ae_state* __restrict _state);
void bsetv_avx2(const ae_int_t n, const ae_bool v, ae_bool* __restrict x,
    ae_state* __restrict _state);
void rmulv_avx2(const ae_int_t n, const double v, double* __restrict x,
     const ae_state* __restrict _state);
void rsqrtv_avx2(const ae_int_t n, double* __restrict x, const ae_state* __restrict _state);
void rmulvx_avx2(const ae_int_t n, const double v, double* __restrict x,
    const ae_state* __restrict _state);
void raddv_avx2(const ae_int_t n,
     const double alpha,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state);
void raddvx_avx2(const ae_int_t n, const double alpha, const double* __restrict y,
    double* __restrict x, ae_state *_state);
void rmergemulv_avx2(ae_int_t n,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state);
void rmergedivv_avx2(ae_int_t n,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state);
void rmergemaxv_avx2(ae_int_t n,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     ae_state* __restrict _state);
void rmergeminv_avx2(ae_int_t n,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     ae_state* __restrict _state);
double rmaxv_avx2(ae_int_t n, /* Real    */ const double* __restrict x, ae_state* __restrict _state);
double rmaxabsv_avx2(ae_int_t n, /* Real    */ const double* __restrict x, ae_state* __restrict _state);
void rcopyvx_avx2(const ae_int_t n, const double* __restrict x,
    double* __restrict y, ae_state *_state);
void icopyvx_avx2(const ae_int_t n, const ae_int_t* __restrict x,
                ae_int_t* __restrict y, ae_state* __restrict _state);

void rgemv_straight_avx2(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a,
    const double* __restrict x, double* __restrict y, ae_state* _state);
void rgemv_transposed_avx2(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a,
    const double* __restrict x, double* __restrict y, ae_state* _state);
void rgemvx_straight_avx2(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a, const ae_int_t ia,
    const ae_int_t ja, const double* __restrict x,
    double* __restrict y, ae_state* _state);
void rgemvx_transposed_avx2(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a, const ae_int_t ia,
    const ae_int_t ja, const double* __restrict x, double* __restrict y,
    ae_state* _state);

ae_int_t ablasf_packblkh_avx2(
    const double *src,
    ae_int_t src_stride,
    ae_int_t op,
    ae_int_t opsrc_length,
    ae_int_t opsrc_width,
    double   *dst,
    ae_int_t block_size,
    ae_int_t micro_size);
ae_int_t ablasf_packblkh32_avx2(
    const double *src,
    ae_int_t src_stride,
    ae_int_t op,
    ae_int_t ignore_opsrc_length,
    ae_int_t opsrc_width,
    double   *dst,
    ae_int_t ignore_block_size,
    ae_int_t micro_size);
void ablasf_dotblkh_avx2(
    const double *src_a,
    const double *src_b,
    ae_int_t round_length,
    ae_int_t block_size,
    ae_int_t micro_size,
    double *dst,
    ae_int_t dst_stride);
void ablasf_daxpby_avx2(
    ae_int_t     n,
    double       alpha,
    const double *src,
    double       beta,
    double       *dst);
ae_bool spchol_updatekernelabc4_avx2(double* rowstorage,
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
ae_bool spchol_updatekernel4444_avx2(
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
ae_bool rbfv3farfields_bhpaneleval1fastkernel16_avx2(double d0,
     double d1,
     double d2,
     const double* pnma,
     const double* pnmb,
     const double* pmmcdiag,
     const double* ynma,
     const double* tblrmodmn,
     double* f,
     double* invpowrpplus1,
     ae_state *_state);
ae_bool rbfv3farfields_bhpanelevalfastkernel16_avx2(double d0,
     double d1,
     double d2,
     ae_int_t ny,
     const double* pnma,
     const double* pnmb,
     const double* pmmcdiag,
     const double* ynma,
     const double* tblrmodmn,
     double* f,
     double* invpowrpplus1,
     ae_state *_state);

/* ALGLIB_NO_FAST_KERNELS, _ALGLIB_HAS_AVX2_INTRINSICS */
#endif
/*$ End $*/
#endif