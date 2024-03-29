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
#include "schur.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Subroutine performing the Schur decomposition of a general matrix by using
the QR algorithm with multiple shifts.

COMMERCIAL EDITION OF ALGLIB:

  ! Commercial version of ALGLIB includes one  important  improvement   of
  ! this function, which can be used from C++ and C#:
  ! * Intel MKL support (lightweight Intel MKL is shipped with ALGLIB)
  !
  ! Intel MKL gives approximately constant  (with  respect  to  number  of
  ! worker threads) acceleration factor which depends on CPU  being  used,
  ! problem  size  and  "baseline"  ALGLIB  edition  which  is  used   for
  ! comparison.
  !
  ! Multithreaded acceleration is NOT supported for this function.
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

The source matrix A is represented as S'*A*S = T, where S is an orthogonal
matrix (Schur vectors), T - upper quasi-triangular matrix (with blocks of
sizes 1x1 and 2x2 on the main diagonal).

Input parameters:
    A   -   matrix to be decomposed.
            Array whose indexes range within [0..N-1, 0..N-1].
    N   -   size of A, N>=0.


Output parameters:
    A   -   contains matrix T.
            Array whose indexes range within [0..N-1, 0..N-1].
    S   -   contains Schur vectors.
            Array whose indexes range within [0..N-1, 0..N-1].

Note 1:
    The block structure of matrix T can be easily recognized: since all
    the elements below the blocks are zeros, the elements a[i+1,i] which
    are equal to 0 show the block border.

Note 2:
    The algorithm performance depends on the value of the internal parameter
    NS of the InternalSchurDecomposition subroutine which defines the number
    of shifts in the QR algorithm (similarly to the block width in block-matrix
    algorithms in linear algebra). If you require maximum performance on
    your machine, it is recommended to adjust this parameter manually.

Result:
    True,
        if the algorithm has converged and parameters A and S contain the result.
    False,
        if the algorithm has not converged.

Algorithm implemented on the basis of the DHSEQR subroutine (LAPACK 3.0 library).
*************************************************************************/
ae_bool rmatrixschur(/* Real    */ ae_matrix* a,
     ae_int_t n,
     /* Real    */ ae_matrix* s,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector tau;
    ae_vector wi;
    ae_vector wr;
    ae_int_t info;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&tau, 0, sizeof(tau));
    memset(&wi, 0, sizeof(wi));
    memset(&wr, 0, sizeof(wr));
    ae_matrix_clear(s);
    ae_vector_init(&tau, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&wi, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&wr, 0, DT_REAL, _state, ae_true);

    
    /*
     * Upper Hessenberg form of the 0-based matrix
     */
    rmatrixhessenberg(a, n, &tau, _state);
    rmatrixhessenbergunpackq(a, n, &tau, s, _state);
    
    /*
     * Schur decomposition
     */
    rmatrixinternalschurdecomposition(a, n, 1, 1, &wr, &wi, s, &info, _state);
    result = info==0;
    ae_frame_leave(_state);
    return result;
}


/*$ End $*/
