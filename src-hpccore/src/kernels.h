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
#ifndef _HALGLIB_KERNELS_H
#define _HALGLIB_KERNELS_H

#include "aenv.h"

/*$ Includes $*/

/*$ Body $*/
#if !defined(ALGLIB_NO_FAST_KERNELS)

#if defined(_ALGLIB_IMPL_DEFINES)
    /*
     * Arrays shorter than that will be processed with generic C implementation
     */
    #if !defined(_ABLASF_KERNEL_SIZE1)
    #define _ABLASF_KERNEL_SIZE1 16
    #endif
    #if !defined(_ABLASF_KERNEL_SIZE2)
    #define _ABLASF_KERNEL_SIZE2 16
    #endif
    #define _ABLASF_BLOCK_SIZE 32
    #define _ABLASF_MICRO_SIZE  2
    #if defined(_ALGLIB_HAS_AVX2_INTRINSICS) || defined(_ALGLIB_HAS_FMA_INTRINSICS)
        #define ULOAD256PD(x) _mm256_loadu_pd((const double*)(&x))
    #endif
#endif

/*
 * ABLASF kernels
 */
double rdotv(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_state *_state);
double rdotvr(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     ae_state *_state);
double rdotrr(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_state *_state);
double rdotv2(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void rcopyv(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void rcopyvr(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* a,
     ae_int_t i,
     ae_state *_state);
void rcopyrv(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rcopyrr(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     /* Real    */ ae_matrix* b,
     ae_int_t k,
     ae_state *_state);
void rcopymulv(ae_int_t n,
     double v,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void rcopymulvr(ae_int_t n,
     double v,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* y,
     ae_int_t ridx,
     ae_state *_state);
void icopyv(ae_int_t n,
     /* Integer */ const ae_vector* x,
     /* Integer */ ae_vector* y,
     ae_state *_state);
void bcopyv(ae_int_t n,
     /* Boolean */ const ae_vector* x,
     /* Boolean */ ae_vector* y,
     ae_state *_state);
void rsetv(ae_int_t n,
     double v,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rsetr(ae_int_t n,
     double v,
     /* Real    */ ae_matrix* a,
     ae_int_t i,
     ae_state *_state);
void rsetvx(ae_int_t n,
     double v,
     /* Real    */ ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
void rsetm(ae_int_t m,
     ae_int_t n,
     double v,
     /* Real    */ ae_matrix* a,
     ae_state *_state);
void isetv(ae_int_t n,
     ae_int_t v,
     /* Integer */ ae_vector* x,
     ae_state *_state);
void bsetv(ae_int_t n,
     ae_bool v,
     /* Boolean */ ae_vector* x,
     ae_state *_state);
void rmulv(ae_int_t n,
     double v,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmulr(ae_int_t n,
     double v,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rsqrtv(ae_int_t n,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rsqrtr(ae_int_t n,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmulvx(ae_int_t n,
     double v,
     /* Real    */ ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
void raddv(ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void raddvr(ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void raddrv(ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* y,
     ae_int_t ridx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void raddrr(ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* y,
     ae_int_t ridxsrc,
     /* Real    */ ae_matrix* x,
     ae_int_t ridxdst,
     ae_state *_state);
void raddvx(ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* y,
     ae_int_t offsy,
     /* Real    */ ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
void rmuladdv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* z,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rnegmuladdv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* z,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rcopymuladdv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* z,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* r,
     ae_state *_state);
void rcopynegmuladdv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* z,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* r,
     ae_state *_state);
void rmergemulv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmergemulvr(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmergemulrv(ae_int_t n,
     /* Real    */ const ae_matrix* y,
     ae_int_t rowidx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmergedivv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmergedivvr(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmergedivrv(ae_int_t n,
     /* Real    */ const ae_matrix* y,
     ae_int_t rowidx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmergemaxv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmergemaxvr(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmergemaxrv(ae_int_t n,
     /* Real    */ const ae_matrix* y,
     ae_int_t rowidx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmergeminv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rmergeminvr(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rmergeminrv(ae_int_t n,
     /* Real    */ const ae_matrix* y,
     ae_int_t rowidx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
double rmaxv(ae_int_t n,
    /* Real    */ const ae_vector* x,
    ae_state *_state);
double rmaxr(ae_int_t n,
     /* Real    */ const ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
double rmaxabsv(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
double rmaxabsr(ae_int_t n,
     /* Real    */ const ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
void rcopyvx(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_int_t offsx,
     /* Real    */ ae_vector* y,
     ae_int_t offsy,
     ae_state *_state);
void icopyvx(ae_int_t n,
     /* Integer */ const ae_vector* x,
     ae_int_t offsx,
     /* Integer */ ae_vector* y,
     ae_int_t offsy,
     ae_state *_state);
 
void rgemv(ae_int_t m,
     ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t opa,
     /* Real    */ const ae_vector* x,
     double beta,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void rgemvx(ae_int_t m,
     ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t opa,
     /* Real    */ const ae_vector* x,
     ae_int_t ix,
     double beta,
     /* Real    */ ae_vector* y,
     ae_int_t iy,
     ae_state *_state);
void rger(ae_int_t m,
     ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* u,
     /* Real    */ const ae_vector* v,
     /* Real    */ ae_matrix* a,
     ae_state *_state);
void rtrsvx(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Real    */ ae_vector* x,
     ae_int_t ix,
     ae_state *_state);

ae_bool ablasf_rgemm32basecase(
     ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_int_t optypeb,
     double beta,
     /* Real    */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state);
 
/*
 * Sparse supernodal Cholesky kernels
 */
ae_int_t spchol_spsymmgetmaxsimd(ae_state *_state);
void spchol_propagatefwd(/* Real    */ const ae_vector* x,
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
ae_bool spchol_updatekernelabc4(/* Real    */ ae_vector* rowstorage,
     ae_int_t offss,
     ae_int_t twidth,
     ae_int_t offsu,
     ae_int_t uheight,
     ae_int_t urank,
     ae_int_t urowstride,
     ae_int_t uwidth,
     /* Real    */ const ae_vector* diagd,
     ae_int_t offsd,
     /* Integer */ const ae_vector* raw2smap,
     /* Integer */ const ae_vector* superrowidx,
     ae_int_t urbase,
     ae_state *_state);
ae_bool spchol_updatekernel4444(/* Real    */ ae_vector* rowstorage,
     ae_int_t offss,
     ae_int_t sheight,
     ae_int_t offsu,
     ae_int_t uheight,
     /* Real    */ const ae_vector* diagd,
     ae_int_t offsd,
     /* Integer */ const ae_vector* raw2smap,
     /* Integer */ const ae_vector* superrowidx,
     ae_int_t urbase,
     ae_state *_state);
     
/*
 * Far field expansions for RBFs
 */
ae_bool rbfv3farfields_bhpaneleval1fastkernel(double d0,
     double d1,
     double d2,
     ae_int_t panelp,
     /* Real    */ const ae_vector* pnma,
     /* Real    */ const ae_vector* pnmb,
     /* Real    */ const ae_vector* pmmcdiag,
     /* Real    */ const ae_vector* ynma,
     /* Real    */ const ae_vector* tblrmodmn,
     double* f,
     double* invpowrpplus1,
     ae_state *_state);
ae_bool rbfv3farfields_bhpanelevalfastkernel(double d0,
     double d1,
     double d2,
     ae_int_t ny,
     ae_int_t panelp,
     /* Real    */ const ae_vector* pnma,
     /* Real    */ const ae_vector* pnmb,
     /* Real    */ const ae_vector* pmmcdiag,
     /* Real    */ const ae_vector* ynma,
     /* Real    */ const ae_vector* tblrmodmn,
     /* Real    */ ae_vector* f,
     double* invpowrpplus1,
     ae_state *_state);

/* ALGLIB_NO_FAST_KERNELS */
#endif
/*$ End $*/
#endif