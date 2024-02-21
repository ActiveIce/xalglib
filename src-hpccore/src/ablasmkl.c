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


#include <stdafx.h>
#include "ablasmkl.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
MKL-based kernel

  -- ALGLIB routine --
     12.10.2017
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixgermkl(ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     double alpha,
     /* Real    */ const ae_vector* u,
     ae_int_t iu,
     /* Real    */ const ae_vector* v,
     ae_int_t iv,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixgermkl(m, n, a, ia, ja, alpha, u, iu, v, iv);
#endif
}


/*************************************************************************
MKL-based kernel

  -- ALGLIB routine --
     12.10.2017
     Bochkanov Sergey
*************************************************************************/
ae_bool cmatrixrank1mkl(ae_int_t m,
     ae_int_t n,
     /* Complex */ ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Complex */ const ae_vector* u,
     ae_int_t iu,
     /* Complex */ const ae_vector* v,
     ae_int_t iv,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_cmatrixrank1mkl(m, n, a, ia, ja, u, iu, v, iv);
#endif
}


/*************************************************************************
MKL-based kernel

  -- ALGLIB routine --
     12.10.2017
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixrank1mkl(ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Real    */ const ae_vector* u,
     ae_int_t iu,
     /* Real    */ const ae_vector* v,
     ae_int_t iv,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixrank1mkl(m, n, a, ia, ja, u, iu, v, iv);
#endif
}


/*************************************************************************
MKL-based kernel

  -- ALGLIB routine --
     12.10.2017
     Bochkanov Sergey
*************************************************************************/
ae_bool cmatrixmvmkl(ae_int_t m,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t opa,
     /* Complex */ const ae_vector* x,
     ae_int_t ix,
     /* Complex */ ae_vector* y,
     ae_int_t iy,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_cmatrixmvmkl(m, n, a, ia, ja, opa, x, ix, y, iy);
#endif
}


/*************************************************************************
MKL-based kernel

  -- ALGLIB routine --
     12.10.2017
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixmvmkl(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t opa,
     /* Real    */ const ae_vector* x,
     ae_int_t ix,
     /* Real    */ ae_vector* y,
     ae_int_t iy,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixmvmkl(m, n, a, ia, ja, opa, x, ix, y, iy);
#endif
}


/*************************************************************************
MKL-based kernel

  -- ALGLIB routine --
     12.10.2017
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixgemvmkl(ae_int_t m,
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
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixgemvmkl(m, n, alpha, a, ia, ja, opa, x, ix, beta, y, iy);
#endif
}


/*************************************************************************
MKL-based kernel

  -- ALGLIB routine --
     12.10.2017
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixtrsvmkl(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Real    */ ae_vector* x,
     ae_int_t ix,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixtrsvmkl(n, a, ia, ja, isupper, isunit, optype, x, ix);
#endif
}


/*************************************************************************
MKL-based kernel

  -- ALGLIB routine --
     01.10.2013
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixsyrkmkl(ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     double beta,
     /* Real    */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_bool isupper,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixsyrkmkl(n, k, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper);
#endif
}


/*************************************************************************
MKL-based kernel

  -- ALGLIB routine --
     01.10.2013
     Bochkanov Sergey
*************************************************************************/
ae_bool cmatrixherkmkl(ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     double beta,
     /* Complex */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_bool isupper,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_cmatrixherkmkl(n, k, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper);
#endif
}


/*************************************************************************
MKL-based kernel

  -- ALGLIB routine --
     01.10.2013
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixgemmmkl(ae_int_t m,
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
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixgemmmkl(m, n, k, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc);
#endif
}


/*************************************************************************
MKL-based kernel

  -- ALGLIB routine --
     01.10.2017
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixsymvmkl(ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_bool isupper,
     /* Real    */ const ae_vector* x,
     ae_int_t ix,
     double beta,
     /* Real    */ ae_vector* y,
     ae_int_t iy,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixsymvmkl(n, alpha, a, ia, ja, isupper, x, ix, beta, y, iy);
#endif
}


/*************************************************************************
MKL-based kernel

  -- ALGLIB routine --
     16.10.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool cmatrixgemmmkl(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     ae_complex alpha,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     /* Complex */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_int_t optypeb,
     ae_complex beta,
     /* Complex */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_cmatrixgemmmkl(m, n, k, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc);
#endif
}


/*************************************************************************
MKL-based kernel

  -- ALGLIB routine --
     16.10.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool cmatrixlefttrsmmkl(ae_int_t m,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Complex */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_cmatrixlefttrsmmkl(m, n, a, i1, j1, isupper, isunit, optype, x, i2, j2);
#endif
}


/*************************************************************************
MKL-based kernel

  -- ALGLIB routine --
     16.10.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool cmatrixrighttrsmmkl(ae_int_t m,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Complex */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_cmatrixrighttrsmmkl(m, n, a, i1, j1, isupper, isunit, optype, x, i2, j2);
#endif
}


/*************************************************************************
MKL-based kernel

  -- ALGLIB routine --
     16.10.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixlefttrsmmkl(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Real    */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixlefttrsmmkl(m, n, a, i1, j1, isupper, isunit, optype, x, i2, j2);
#endif
}


/*************************************************************************
MKL-based kernel

  -- ALGLIB routine --
     16.10.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixrighttrsmmkl(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Real    */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixrighttrsmmkl(m, n, a, i1, j1, isupper, isunit, optype, x, i2, j2);
#endif
}


/*************************************************************************
MKL-based kernel.

NOTE:

if function returned False, CholResult is NOT modified. Not ever referenced!
if function returned True, CholResult is set to status of Cholesky decomposition
(True on succeess).

  -- ALGLIB routine --
     16.10.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool spdmatrixcholeskymkl(/* Real    */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t n,
     ae_bool isupper,
     ae_bool* cholresult,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_spdmatrixcholeskymkl(a, offs, n, isupper, cholresult);
#endif
}


/*************************************************************************
MKL-based kernel.

  -- ALGLIB routine --
     20.10.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixplumkl(/* Real    */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixplumkl(a, offs, m, n, pivots);
#endif
}


/*************************************************************************
MKL-based kernel.

NOTE: this function needs preallocated output/temporary arrays.
      D and E must be at least max(M,N)-wide.

  -- ALGLIB routine --
     20.10.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixbdmkl(/* Real    */ ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_vector* e,
     /* Real    */ ae_vector* tauq,
     /* Real    */ ae_vector* taup,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixbdmkl(a, m, n, d, e, tauq, taup);
#endif
}


/*************************************************************************
MKL-based kernel.

If ByQ is True,  TauP is not used (can be empty array).
If ByQ is False, TauQ is not used (can be empty array).

  -- ALGLIB routine --
     20.10.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixbdmultiplybymkl(/* Real    */ const ae_matrix* qp,
     ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_vector* tauq,
     /* Real    */ const ae_vector* taup,
     /* Real    */ ae_matrix* z,
     ae_int_t zrows,
     ae_int_t zcolumns,
     ae_bool byq,
     ae_bool fromtheright,
     ae_bool dotranspose,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixbdmultiplybymkl(qp, m, n, tauq, taup, z, zrows, zcolumns, byq, fromtheright, dotranspose);
#endif
}


/*************************************************************************
MKL-based kernel.

NOTE: Tau must be preallocated array with at least N-1 elements.

  -- ALGLIB routine --
     20.10.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixhessenbergmkl(/* Real    */ ae_matrix* a,
     ae_int_t n,
     /* Real    */ ae_vector* tau,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixhessenbergmkl(a, n, tau);
#endif
}


/*************************************************************************
MKL-based kernel.

NOTE: Q must be preallocated N*N array

  -- ALGLIB routine --
     20.10.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixhessenbergunpackqmkl(/* Real    */ const ae_matrix* a,
     ae_int_t n,
     /* Real    */ const ae_vector* tau,
     /* Real    */ ae_matrix* q,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixhessenbergunpackqmkl(a, n, tau, q);
#endif
}


/*************************************************************************
MKL-based kernel.

NOTE: Tau, D, E must be preallocated arrays;
      length(E)=length(Tau)=N-1 (or larger)
      length(D)=N (or larger)

  -- ALGLIB routine --
     20.10.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool smatrixtdmkl(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_vector* tau,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_vector* e,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_smatrixtdmkl(a, n, isupper, tau, d, e);
#endif
}


/*************************************************************************
MKL-based kernel.

NOTE: Q must be preallocated N*N array

  -- ALGLIB routine --
     20.10.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool smatrixtdunpackqmkl(/* Real    */ const ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ const ae_vector* tau,
     /* Real    */ ae_matrix* q,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_smatrixtdunpackqmkl(a, n, isupper, tau, q);
#endif
}


/*************************************************************************
MKL-based kernel.

NOTE: Tau, D, E must be preallocated arrays;
      length(E)=length(Tau)=N-1 (or larger)
      length(D)=N (or larger)

  -- ALGLIB routine --
     20.10.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool hmatrixtdmkl(/* Complex */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_vector* tau,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_vector* e,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_hmatrixtdmkl(a, n, isupper, tau, d, e);
#endif
}


/*************************************************************************
MKL-based kernel.

NOTE: Q must be preallocated N*N array

  -- ALGLIB routine --
     20.10.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool hmatrixtdunpackqmkl(/* Complex */ const ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ const ae_vector* tau,
     /* Complex */ ae_matrix* q,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_hmatrixtdunpackqmkl(a, n, isupper, tau, q);
#endif
}


/*************************************************************************
MKL-based kernel.

Returns True if MKL was present and handled request (MKL  completion  code
is returned as separate output parameter).

D and E are pre-allocated arrays with length N (both of them!). On output,
D constraints singular values, and E is destroyed.

SVDResult is modified if and only if MKL is present.

  -- ALGLIB routine --
     20.10.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixbdsvdmkl(/* Real    */ ae_vector* d,
     /* Real    */ ae_vector* e,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_matrix* u,
     ae_int_t nru,
     /* Real    */ ae_matrix* c,
     ae_int_t ncc,
     /* Real    */ ae_matrix* vt,
     ae_int_t ncvt,
     ae_bool* svdresult,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixbdsvdmkl(d, e, n, isupper, u, nru, c, ncc, vt, ncvt, svdresult);
#endif
}


/*************************************************************************
MKL-based DHSEQR kernel.

Returns True if MKL was present and handled request.

WR and WI are pre-allocated arrays with length N.
Z is pre-allocated array[N,N].

  -- ALGLIB routine --
     20.10.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixinternalschurdecompositionmkl(/* Real    */ ae_matrix* h,
     ae_int_t n,
     ae_int_t tneeded,
     ae_int_t zneeded,
     /* Real    */ ae_vector* wr,
     /* Real    */ ae_vector* wi,
     /* Real    */ ae_matrix* z,
     ae_int_t* info,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixinternalschurdecompositionmkl(h, n, tneeded, zneeded, wr, wi, z, info);
#endif
}


/*************************************************************************
MKL-based DTREVC kernel.

Returns True if MKL was present and handled request.

NOTE: this function does NOT support HOWMNY=3!!!!

VL and VR are pre-allocated arrays with length N*N, if required. If particalar
variables is not required, it can be dummy (empty) array.

  -- ALGLIB routine --
     20.10.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixinternaltrevcmkl(/* Real    */ const ae_matrix* t,
     ae_int_t n,
     ae_int_t side,
     ae_int_t howmny,
     /* Real    */ ae_matrix* vl,
     /* Real    */ ae_matrix* vr,
     ae_int_t* m,
     ae_int_t* info,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixinternaltrevcmkl(t, n, side, howmny, vl, vr, m, info);
#endif
}


/*************************************************************************
MKL-based kernel.

Returns True if MKL was present and handled request (MKL  completion  code
is returned as separate output parameter).

D and E are pre-allocated arrays with length N (both of them!). On output,
D constraints eigenvalues, and E is destroyed.

Z is preallocated array[N,N] for ZNeeded<>0; ignored for ZNeeded=0.

EVDResult is modified if and only if MKL is present.

  -- ALGLIB routine --
     20.10.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool smatrixtdevdmkl(/* Real    */ ae_vector* d,
     /* Real    */ ae_vector* e,
     ae_int_t n,
     ae_int_t zneeded,
     /* Real    */ ae_matrix* z,
     ae_bool* evdresult,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_smatrixtdevdmkl(d, e, n, zneeded, z, evdresult);
#endif
}


/*************************************************************************
MKL-based kernel.

Returns True if MKL was present and handled request (MKL  completion  code
is returned as separate output parameter).

D and E are pre-allocated arrays with length N (both of them!). On output,
D constraints eigenvalues, and E is destroyed.

Z is preallocated array[N,N] for ZNeeded<>0; ignored for ZNeeded=0.

EVDResult is modified if and only if MKL is present.

  -- ALGLIB routine --
     20.10.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool sparsegemvcrsmkl(ae_int_t opa,
     ae_int_t arows,
     ae_int_t acols,
     double alpha,
     /* Real    */ const ae_vector* vals,
     /* Integer */ const ae_vector* cidx,
     /* Integer */ const ae_vector* ridx,
     /* Real    */ const ae_vector* x,
     ae_int_t ix,
     double beta,
     /* Real    */ ae_vector* y,
     ae_int_t iy,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_MKL
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_sparsegemvcrsmkl(opa, arows, acols, alpha, vals, cidx, ridx, x, ix, beta, y, iy);
#endif
}


/*$ End $*/
