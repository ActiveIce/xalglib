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

#ifndef _ablas_h
#define _ablas_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "ablasmkl.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Splits matrix length in two parts, left part should match ABLAS block size

INPUT PARAMETERS
    A   -   real matrix, is passed to ensure that we didn't split
            complex matrix using real splitting subroutine.
            matrix itself is not changed.
    N   -   length, N>0

OUTPUT PARAMETERS
    N1  -   length
    N2  -   length

N1+N2=N, N1>=N2, N2 may be zero

  -- ALGLIB routine --
     15.12.2009
     Bochkanov Sergey
*************************************************************************/
void ablassplitlength(/* Real    */ const ae_matrix* a,
     ae_int_t n,
     ae_int_t* n1,
     ae_int_t* n2,
     ae_state *_state);


/*************************************************************************
Complex ABLASSplitLength

  -- ALGLIB routine --
     15.12.2009
     Bochkanov Sergey
*************************************************************************/
void ablascomplexsplitlength(/* Complex */ const ae_matrix* a,
     ae_int_t n,
     ae_int_t* n1,
     ae_int_t* n2,
     ae_state *_state);


/*************************************************************************
Returns switch point for parallelism.

  -- ALGLIB routine --
     15.12.2009
     Bochkanov Sergey
*************************************************************************/
ae_int_t gemmparallelsize(ae_state *_state);


/*************************************************************************
Returns block size - subdivision size where  cache-oblivious  soubroutines
switch to the optimized kernel.

INPUT PARAMETERS
    A   -   real matrix, is passed to ensure that we didn't split
            complex matrix using real splitting subroutine.
            matrix itself is not changed.

  -- ALGLIB routine --
     15.12.2009
     Bochkanov Sergey
*************************************************************************/
ae_int_t ablasblocksize(/* Real    */ const ae_matrix* a,
     ae_state *_state);


/*************************************************************************
Block size for complex subroutines.

  -- ALGLIB routine --
     15.12.2009
     Bochkanov Sergey
*************************************************************************/
ae_int_t ablascomplexblocksize(/* Complex */ const ae_matrix* a,
     ae_state *_state);


/*************************************************************************
Microblock size

  -- ALGLIB routine --
     15.12.2009
     Bochkanov Sergey
*************************************************************************/
ae_int_t ablasmicroblocksize(ae_state *_state);


/*************************************************************************
Generation of an elementary reflection transformation

The subroutine generates elementary reflection H of order N, so that, for
a given X, the following equality holds true:

    ( X(1) )   ( Beta )
H * (  ..  ) = (  0   )
    ( X(n) )   (  0   )

where
              ( V(1) )
H = 1 - Tau * (  ..  ) * ( V(1), ..., V(n) )
              ( V(n) )

where the first component of vector V equals 1.

Input parameters:
    X   -   vector. Array whose index ranges within [1..N].
    N   -   reflection order.

Output parameters:
    X   -   components from 2 to N are replaced with vector V.
            The first component is replaced with parameter Beta.
    Tau -   scalar value Tau. If X is a null vector, Tau equals 0,
            otherwise 1 <= Tau <= 2.

This subroutine is the modification of the DLARFG subroutines from
the LAPACK library.

MODIFICATIONS:
    24.12.2005 sign(Alpha) was replaced with an analogous to the Fortran SIGN code.

  -- LAPACK auxiliary routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     September 30, 1994
*************************************************************************/
void generatereflection(/* Real    */ ae_vector* x,
     ae_int_t n,
     double* tau,
     ae_state *_state);


/*************************************************************************
Application of an elementary reflection to a rectangular matrix of size MxN

The algorithm pre-multiplies the matrix by an elementary reflection transformation
which is given by column V and scalar Tau (see the description of the
GenerateReflection procedure). Not the whole matrix but only a part of it
is transformed (rows from M1 to M2, columns from N1 to N2). Only the elements
of this submatrix are changed.

Input parameters:
    C       -   matrix to be transformed.
    Tau     -   scalar defining the transformation.
    V       -   column defining the transformation.
                Array whose index ranges within [1..M2-M1+1].
    M1, M2  -   range of rows to be transformed.
    N1, N2  -   range of columns to be transformed.
    WORK    -   working array whose indexes goes from N1 to N2.

Output parameters:
    C       -   the result of multiplying the input matrix C by the
                transformation matrix which is given by Tau and V.
                If N1>N2 or M1>M2, C is not modified.

  -- LAPACK auxiliary routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     September 30, 1994
*************************************************************************/
void applyreflectionfromtheleft(/* Real    */ ae_matrix* c,
     double tau,
     /* Real    */ const ae_vector* v,
     ae_int_t m1,
     ae_int_t m2,
     ae_int_t n1,
     ae_int_t n2,
     /* Real    */ ae_vector* work,
     ae_state *_state);


/*************************************************************************
Application of an elementary reflection to a rectangular matrix of size MxN

The algorithm post-multiplies the matrix by an elementary reflection transformation
which is given by column V and scalar Tau (see the description of the
GenerateReflection procedure). Not the whole matrix but only a part of it
is transformed (rows from M1 to M2, columns from N1 to N2). Only the
elements of this submatrix are changed.

Input parameters:
    C       -   matrix to be transformed.
    Tau     -   scalar defining the transformation.
    V       -   column defining the transformation.
                Array whose index ranges within [1..N2-N1+1].
    M1, M2  -   range of rows to be transformed.
    N1, N2  -   range of columns to be transformed.
    WORK    -   working array whose indexes goes from M1 to M2.

Output parameters:
    C       -   the result of multiplying the input matrix C by the
                transformation matrix which is given by Tau and V.
                If N1>N2 or M1>M2, C is not modified.

  -- LAPACK auxiliary routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     September 30, 1994
*************************************************************************/
void applyreflectionfromtheright(/* Real    */ ae_matrix* c,
     double tau,
     /* Real    */ const ae_vector* v,
     ae_int_t m1,
     ae_int_t m2,
     ae_int_t n1,
     ae_int_t n2,
     /* Real    */ ae_vector* work,
     ae_state *_state);


/*************************************************************************
Cache-oblivous complex "copy-and-transpose"

Input parameters:
    M   -   number of rows
    N   -   number of columns
    A   -   source matrix, MxN submatrix is copied and transposed
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    B   -   destination matrix, must be large enough to store result
    IB  -   submatrix offset (row index)
    JB  -   submatrix offset (column index)
*************************************************************************/
void cmatrixtranspose(ae_int_t m,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Complex */ ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_state *_state);


/*************************************************************************
Cache-oblivous real "copy-and-transpose"

Input parameters:
    M   -   number of rows
    N   -   number of columns
    A   -   source matrix, MxN submatrix is copied and transposed
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    B   -   destination matrix, must be large enough to store result
    IB  -   submatrix offset (row index)
    JB  -   submatrix offset (column index)
*************************************************************************/
void rmatrixtranspose(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Real    */ ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_state *_state);


/*************************************************************************
This code enforces symmetricy of the matrix by copying Upper part to lower
one (or vice versa).

INPUT PARAMETERS:
    A   -   matrix
    N   -   number of rows/columns
    IsUpper - whether we want to copy upper triangle to lower one (True)
            or vice versa (False).
*************************************************************************/
void rmatrixenforcesymmetricity(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     ae_state *_state);


/*************************************************************************
Copy

Input parameters:
    M   -   number of rows
    N   -   number of columns
    A   -   source matrix, MxN submatrix is copied and transposed
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    B   -   destination matrix, must be large enough to store result
    IB  -   submatrix offset (row index)
    JB  -   submatrix offset (column index)
*************************************************************************/
void cmatrixcopy(ae_int_t m,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Complex */ ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_state *_state);


/*************************************************************************
Copy

Input parameters:
    N   -   subvector size
    A   -   source vector, N elements are copied
    IA  -   source offset (first element index)
    B   -   destination vector, must be large enough to store result
    IB  -   destination offset (first element index)
*************************************************************************/
void rvectorcopy(ae_int_t n,
     /* Real    */ const ae_vector* a,
     ae_int_t ia,
     /* Real    */ ae_vector* b,
     ae_int_t ib,
     ae_state *_state);


/*************************************************************************
Copy

Input parameters:
    M   -   number of rows
    N   -   number of columns
    A   -   source matrix, MxN submatrix is copied and transposed
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    B   -   destination matrix, must be large enough to store result
    IB  -   submatrix offset (row index)
    JB  -   submatrix offset (column index)
*************************************************************************/
void rmatrixcopy(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Real    */ ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_state *_state);


/*************************************************************************
Performs generalized copy: B := Beta*B + Alpha*A.

If Beta=0, then previous contents of B is simply ignored. If Alpha=0, then
A is ignored and not referenced. If both Alpha and Beta  are  zero,  B  is
filled by zeros.

Input parameters:
    M   -   number of rows
    N   -   number of columns
    Alpha-  coefficient
    A   -   source matrix, MxN submatrix is copied and transposed
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    Beta-   coefficient
    B   -   destination matrix, must be large enough to store result
    IB  -   submatrix offset (row index)
    JB  -   submatrix offset (column index)
*************************************************************************/
void rmatrixgencopy(ae_int_t m,
     ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     double beta,
     /* Real    */ ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_state *_state);


/*************************************************************************
Rank-1 correction: A := A + alpha*u*v'

NOTE: this  function  expects  A  to  be  large enough to store result. No
      automatic preallocation happens for  smaller  arrays.  No  integrity
      checks is performed for sizes of A, u, v.

INPUT PARAMETERS:
    M   -   number of rows
    N   -   number of columns
    A   -   target matrix, MxN submatrix is updated
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    Alpha-  coefficient
    U   -   vector #1
    IU  -   subvector offset
    V   -   vector #2
    IV  -   subvector offset


  -- ALGLIB routine --

     16.10.2017
     Bochkanov Sergey
*************************************************************************/
void rmatrixger(ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     double alpha,
     /* Real    */ const ae_vector* u,
     ae_int_t iu,
     /* Real    */ const ae_vector* v,
     ae_int_t iv,
     ae_state *_state);


/*************************************************************************
Rank-1 correction: A := A + u*v'

INPUT PARAMETERS:
    M   -   number of rows
    N   -   number of columns
    A   -   target matrix, MxN submatrix is updated
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    U   -   vector #1
    IU  -   subvector offset
    V   -   vector #2
    IV  -   subvector offset
*************************************************************************/
void cmatrixrank1(ae_int_t m,
     ae_int_t n,
     /* Complex */ ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Complex */ const ae_vector* u,
     ae_int_t iu,
     /* Complex */ const ae_vector* v,
     ae_int_t iv,
     ae_state *_state);


/*************************************************************************
IMPORTANT: this function is deprecated since ALGLIB 3.13. Use RMatrixGER()
           which is more generic version of this function.

Rank-1 correction: A := A + u*v'

INPUT PARAMETERS:
    M   -   number of rows
    N   -   number of columns
    A   -   target matrix, MxN submatrix is updated
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    U   -   vector #1
    IU  -   subvector offset
    V   -   vector #2
    IV  -   subvector offset
*************************************************************************/
void rmatrixrank1(ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Real    */ const ae_vector* u,
     ae_int_t iu,
     /* Real    */ const ae_vector* v,
     ae_int_t iv,
     ae_state *_state);


void rmatrixgemv(ae_int_t m,
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


/*************************************************************************
Matrix-vector product: y := op(A)*x

INPUT PARAMETERS:
    M   -   number of rows of op(A)
            M>=0
    N   -   number of columns of op(A)
            N>=0
    A   -   target matrix
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    OpA -   operation type:
            * OpA=0     =>  op(A) = A
            * OpA=1     =>  op(A) = A^T
            * OpA=2     =>  op(A) = A^H
    X   -   input vector
    IX  -   subvector offset
    IY  -   subvector offset
    Y   -   preallocated matrix, must be large enough to store result

OUTPUT PARAMETERS:
    Y   -   vector which stores result

if M=0, then subroutine does nothing.
if N=0, Y is filled by zeros.


  -- ALGLIB routine --

     28.01.2010
     Bochkanov Sergey
*************************************************************************/
void cmatrixmv(ae_int_t m,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t opa,
     /* Complex */ const ae_vector* x,
     ae_int_t ix,
     /* Complex */ ae_vector* y,
     ae_int_t iy,
     ae_state *_state);


/*************************************************************************
IMPORTANT: this function is deprecated since ALGLIB 3.13. Use RMatrixGEMV()
           which is more generic version of this function.
           
Matrix-vector product: y := op(A)*x

INPUT PARAMETERS:
    M   -   number of rows of op(A)
    N   -   number of columns of op(A)
    A   -   target matrix
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    OpA -   operation type:
            * OpA=0     =>  op(A) = A
            * OpA=1     =>  op(A) = A^T
    X   -   input vector
    IX  -   subvector offset
    IY  -   subvector offset
    Y   -   preallocated matrix, must be large enough to store result

OUTPUT PARAMETERS:
    Y   -   vector which stores result

if M=0, then subroutine does nothing.
if N=0, Y is filled by zeros.


  -- ALGLIB routine --

     28.01.2010
     Bochkanov Sergey
*************************************************************************/
void rmatrixmv(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t opa,
     /* Real    */ const ae_vector* x,
     ae_int_t ix,
     /* Real    */ ae_vector* y,
     ae_int_t iy,
     ae_state *_state);


void rmatrixsymv(ae_int_t n,
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
     ae_state *_state);


double rmatrixsyvmv(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_bool isupper,
     /* Real    */ const ae_vector* x,
     ae_int_t ix,
     /* Real    */ ae_vector* tmp,
     ae_state *_state);


/*************************************************************************
This subroutine solves linear system op(A)*x=b where:
* A is NxN upper/lower triangular/unitriangular matrix
* X and B are Nx1 vectors
* "op" may be identity transformation or transposition

Solution replaces X.

IMPORTANT: * no overflow/underflow/denegeracy tests is performed.
           * no integrity checks for operand sizes, out-of-bounds accesses
             and so on is performed

INPUT PARAMETERS
    N   -   matrix size, N>=0
    A       -   matrix, actial matrix is stored in A[IA:IA+N-1,JA:JA+N-1]
    IA      -   submatrix offset
    JA      -   submatrix offset
    IsUpper -   whether matrix is upper triangular
    IsUnit  -   whether matrix is unitriangular
    OpType  -   transformation type:
                * 0 - no transformation
                * 1 - transposition
    X       -   right part, actual vector is stored in X[IX:IX+N-1]
    IX      -   offset
    
OUTPUT PARAMETERS
    X       -   solution replaces elements X[IX:IX+N-1]

  -- ALGLIB routine / remastering of LAPACK's DTRSV --
     (c) 2017 Bochkanov Sergey - converted to ALGLIB
     (c) 2016 Reference BLAS level1 routine (LAPACK version 3.7.0)
     Reference BLAS is a software package provided by Univ. of Tennessee,
     Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd.
*************************************************************************/
void rmatrixtrsv(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Real    */ ae_vector* x,
     ae_int_t ix,
     ae_state *_state);


/*************************************************************************
This subroutine calculates X*op(A^-1) where:
* X is MxN general matrix
* A is NxN upper/lower triangular/unitriangular matrix
* "op" may be identity transformation, transposition, conjugate transposition
Multiplication result replaces X.

INPUT PARAMETERS
    N   -   matrix size, N>=0
    M   -   matrix size, N>=0
    A       -   matrix, actial matrix is stored in A[I1:I1+N-1,J1:J1+N-1]
    I1      -   submatrix offset
    J1      -   submatrix offset
    IsUpper -   whether matrix is upper triangular
    IsUnit  -   whether matrix is unitriangular
    OpType  -   transformation type:
                * 0 - no transformation
                * 1 - transposition
                * 2 - conjugate transposition
    X   -   matrix, actial matrix is stored in X[I2:I2+M-1,J2:J2+N-1]
    I2  -   submatrix offset
    J2  -   submatrix offset

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB routine --
     20.01.2018
     Bochkanov Sergey
*************************************************************************/
void cmatrixrighttrsm(ae_int_t m,
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
     ae_state *_state);
void _spawn_cmatrixrighttrsm(ae_int_t m,
    ae_int_t n,
    /* Complex */ const ae_matrix* a,
    ae_int_t i1,
    ae_int_t j1,
    ae_bool isupper,
    ae_bool isunit,
    ae_int_t optype,
    /* Complex */ ae_matrix* x,
    ae_int_t i2,
    ae_int_t j2, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_cmatrixrighttrsm(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_cmatrixrighttrsm(ae_int_t m,
    ae_int_t n,
    /* Complex */ const ae_matrix* a,
    ae_int_t i1,
    ae_int_t j1,
    ae_bool isupper,
    ae_bool isunit,
    ae_int_t optype,
    /* Complex */ ae_matrix* x,
    ae_int_t i2,
    ae_int_t j2, ae_state *_state);


/*************************************************************************
This subroutine calculates op(A^-1)*X where:
* X is MxN general matrix
* A is MxM upper/lower triangular/unitriangular matrix
* "op" may be identity transformation, transposition, conjugate transposition
Multiplication result replaces X.

INPUT PARAMETERS
    N   -   matrix size, N>=0
    M   -   matrix size, N>=0
    A       -   matrix, actial matrix is stored in A[I1:I1+M-1,J1:J1+M-1]
    I1      -   submatrix offset
    J1      -   submatrix offset
    IsUpper -   whether matrix is upper triangular
    IsUnit  -   whether matrix is unitriangular
    OpType  -   transformation type:
                * 0 - no transformation
                * 1 - transposition
                * 2 - conjugate transposition
    X   -   matrix, actial matrix is stored in X[I2:I2+M-1,J2:J2+N-1]
    I2  -   submatrix offset
    J2  -   submatrix offset

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB routine --
     15.12.2009-22.01.2018
     Bochkanov Sergey
*************************************************************************/
void cmatrixlefttrsm(ae_int_t m,
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
     ae_state *_state);
void _spawn_cmatrixlefttrsm(ae_int_t m,
    ae_int_t n,
    /* Complex */ const ae_matrix* a,
    ae_int_t i1,
    ae_int_t j1,
    ae_bool isupper,
    ae_bool isunit,
    ae_int_t optype,
    /* Complex */ ae_matrix* x,
    ae_int_t i2,
    ae_int_t j2, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_cmatrixlefttrsm(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_cmatrixlefttrsm(ae_int_t m,
    ae_int_t n,
    /* Complex */ const ae_matrix* a,
    ae_int_t i1,
    ae_int_t j1,
    ae_bool isupper,
    ae_bool isunit,
    ae_int_t optype,
    /* Complex */ ae_matrix* x,
    ae_int_t i2,
    ae_int_t j2, ae_state *_state);


/*************************************************************************
This subroutine calculates X*op(A^-1) where:
* X is MxN general matrix
* A is NxN upper/lower triangular/unitriangular matrix
* "op" may be identity transformation, transposition
Multiplication result replaces X.

INPUT PARAMETERS
    N   -   matrix size, N>=0
    M   -   matrix size, N>=0
    A       -   matrix, actial matrix is stored in A[I1:I1+N-1,J1:J1+N-1]
    I1      -   submatrix offset
    J1      -   submatrix offset
    IsUpper -   whether matrix is upper triangular
    IsUnit  -   whether matrix is unitriangular
    OpType  -   transformation type:
                * 0 - no transformation
                * 1 - transposition
    X   -   matrix, actial matrix is stored in X[I2:I2+M-1,J2:J2+N-1]
    I2  -   submatrix offset
    J2  -   submatrix offset

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB routine --
     15.12.2009-22.01.2018
     Bochkanov Sergey
*************************************************************************/
void rmatrixrighttrsm(ae_int_t m,
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
     ae_state *_state);
void _spawn_rmatrixrighttrsm(ae_int_t m,
    ae_int_t n,
    /* Real    */ const ae_matrix* a,
    ae_int_t i1,
    ae_int_t j1,
    ae_bool isupper,
    ae_bool isunit,
    ae_int_t optype,
    /* Real    */ ae_matrix* x,
    ae_int_t i2,
    ae_int_t j2, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_rmatrixrighttrsm(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_rmatrixrighttrsm(ae_int_t m,
    ae_int_t n,
    /* Real    */ const ae_matrix* a,
    ae_int_t i1,
    ae_int_t j1,
    ae_bool isupper,
    ae_bool isunit,
    ae_int_t optype,
    /* Real    */ ae_matrix* x,
    ae_int_t i2,
    ae_int_t j2, ae_state *_state);


/*************************************************************************
This subroutine calculates op(A^-1)*X where:
* X is MxN general matrix
* A is MxM upper/lower triangular/unitriangular matrix
* "op" may be identity transformation, transposition
Multiplication result replaces X.

INPUT PARAMETERS
    N   -   matrix size, N>=0
    M   -   matrix size, N>=0
    A       -   matrix, actial matrix is stored in A[I1:I1+M-1,J1:J1+M-1]
    I1      -   submatrix offset
    J1      -   submatrix offset
    IsUpper -   whether matrix is upper triangular
    IsUnit  -   whether matrix is unitriangular
    OpType  -   transformation type:
                * 0 - no transformation
                * 1 - transposition
    X   -   matrix, actial matrix is stored in X[I2:I2+M-1,J2:J2+N-1]
    I2  -   submatrix offset
    J2  -   submatrix offset

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB routine --
     15.12.2009-22.01.2018
     Bochkanov Sergey
*************************************************************************/
void rmatrixlefttrsm(ae_int_t m,
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
     ae_state *_state);
void _spawn_rmatrixlefttrsm(ae_int_t m,
    ae_int_t n,
    /* Real    */ const ae_matrix* a,
    ae_int_t i1,
    ae_int_t j1,
    ae_bool isupper,
    ae_bool isunit,
    ae_int_t optype,
    /* Real    */ ae_matrix* x,
    ae_int_t i2,
    ae_int_t j2, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_rmatrixlefttrsm(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_rmatrixlefttrsm(ae_int_t m,
    ae_int_t n,
    /* Real    */ const ae_matrix* a,
    ae_int_t i1,
    ae_int_t j1,
    ae_bool isupper,
    ae_bool isunit,
    ae_int_t optype,
    /* Real    */ ae_matrix* x,
    ae_int_t i2,
    ae_int_t j2, ae_state *_state);


/*************************************************************************
This subroutine calculates  C=alpha*A*A^H+beta*C  or  C=alpha*A^H*A+beta*C
where:
* C is NxN Hermitian matrix given by its upper/lower triangle
* A is NxK matrix when A*A^H is calculated, KxN matrix otherwise

Additional info:
* multiplication result replaces C. If Beta=0, C elements are not used in
  calculations (not multiplied by zero - just not referenced)
* if Alpha=0, A is not used (not multiplied by zero - just not referenced)
* if both Beta and Alpha are zero, C is filled by zeros.

INPUT PARAMETERS
    N       -   matrix size, N>=0
    K       -   matrix size, K>=0
    Alpha   -   coefficient
    A       -   matrix
    IA      -   submatrix offset (row index)
    JA      -   submatrix offset (column index)
    OpTypeA -   multiplication type:
                * 0 - A*A^H is calculated
                * 2 - A^H*A is calculated
    Beta    -   coefficient
    C       -   preallocated input/output matrix
    IC      -   submatrix offset (row index)
    JC      -   submatrix offset (column index)
    IsUpper -   whether upper or lower triangle of C is updated;
                this function updates only one half of C, leaving
                other half unchanged (not referenced at all).

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB routine --
     16.12.2009-22.01.2018
     Bochkanov Sergey
*************************************************************************/
void cmatrixherk(ae_int_t n,
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
     ae_state *_state);
void _spawn_cmatrixherk(ae_int_t n,
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
    ae_bool isupper, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_cmatrixherk(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_cmatrixherk(ae_int_t n,
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
    ae_bool isupper, ae_state *_state);


/*************************************************************************
This subroutine calculates  C=alpha*A*A^T+beta*C  or  C=alpha*A^T*A+beta*C
where:
* C is NxN symmetric matrix given by its upper/lower triangle
* A is NxK matrix when A*A^T is calculated, KxN matrix otherwise

Additional info:
* multiplication result replaces C. If Beta=0, C elements are not used in
  calculations (not multiplied by zero - just not referenced)
* if Alpha=0, A is not used (not multiplied by zero - just not referenced)
* if both Beta and Alpha are zero, C is filled by zeros.

INPUT PARAMETERS
    N       -   matrix size, N>=0
    K       -   matrix size, K>=0
    Alpha   -   coefficient
    A       -   matrix
    IA      -   submatrix offset (row index)
    JA      -   submatrix offset (column index)
    OpTypeA -   multiplication type:
                * 0 - A*A^T is calculated
                * 2 - A^T*A is calculated
    Beta    -   coefficient
    C       -   preallocated input/output matrix
    IC      -   submatrix offset (row index)
    JC      -   submatrix offset (column index)
    IsUpper -   whether C is upper triangular or lower triangular

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB routine --
     16.12.2009-22.01.2018
     Bochkanov Sergey
*************************************************************************/
void rmatrixsyrk(ae_int_t n,
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
     ae_state *_state);
void _spawn_rmatrixsyrk(ae_int_t n,
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
    ae_bool isupper, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_rmatrixsyrk(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_rmatrixsyrk(ae_int_t n,
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
    ae_bool isupper, ae_state *_state);


/*************************************************************************
This subroutine calculates C = alpha*op1(A)*op2(B) +beta*C where:
* C is MxN general matrix
* op1(A) is MxK matrix
* op2(B) is KxN matrix
* "op" may be identity transformation, transposition, conjugate transposition

Additional info:
* cache-oblivious algorithm is used.
* multiplication result replaces C. If Beta=0, C elements are not used in
  calculations (not multiplied by zero - just not referenced)
* if Alpha=0, A is not used (not multiplied by zero - just not referenced)
* if both Beta and Alpha are zero, C is filled by zeros.

IMPORTANT:

This function does NOT preallocate output matrix C, it MUST be preallocated
by caller prior to calling this function. In case C does not have  enough
space to store result, exception will be generated.

INPUT PARAMETERS
    M       -   matrix size, M>0
    N       -   matrix size, N>0
    K       -   matrix size, K>0
    Alpha   -   coefficient
    A       -   matrix
    IA      -   submatrix offset
    JA      -   submatrix offset
    OpTypeA -   transformation type:
                * 0 - no transformation
                * 1 - transposition
                * 2 - conjugate transposition
    B       -   matrix
    IB      -   submatrix offset
    JB      -   submatrix offset
    OpTypeB -   transformation type:
                * 0 - no transformation
                * 1 - transposition
                * 2 - conjugate transposition
    Beta    -   coefficient
    C       -   matrix (PREALLOCATED, large enough to store result)
    IC      -   submatrix offset
    JC      -   submatrix offset

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB routine --
     2009-2019
     Bochkanov Sergey
*************************************************************************/
void cmatrixgemm(ae_int_t m,
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
     ae_state *_state);
void _spawn_cmatrixgemm(ae_int_t m,
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
    ae_int_t jc, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_cmatrixgemm(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_cmatrixgemm(ae_int_t m,
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
    ae_int_t jc, ae_state *_state);


/*************************************************************************
This subroutine calculates C = alpha*op1(A)*op2(B) +beta*C where:
* C is MxN general matrix
* op1(A) is MxK matrix
* op2(B) is KxN matrix
* "op" may be identity transformation, transposition

Additional info:
* cache-oblivious algorithm is used.
* multiplication result replaces C. If Beta=0, C elements are not used in
  calculations (not multiplied by zero - just not referenced)
* if Alpha=0, A is not used (not multiplied by zero - just not referenced)
* if both Beta and Alpha are zero, C is filled by zeros.

IMPORTANT:

This function does NOT preallocate output matrix C, it MUST be preallocated
by caller prior to calling this function. In case C does not have  enough
space to store result, exception will be generated.

INPUT PARAMETERS
    M       -   matrix size, M>0
    N       -   matrix size, N>0
    K       -   matrix size, K>0
    Alpha   -   coefficient
    A       -   matrix
    IA      -   submatrix offset
    JA      -   submatrix offset
    OpTypeA -   transformation type:
                * 0 - no transformation
                * 1 - transposition
    B       -   matrix
    IB      -   submatrix offset
    JB      -   submatrix offset
    OpTypeB -   transformation type:
                * 0 - no transformation
                * 1 - transposition
    Beta    -   coefficient
    C       -   PREALLOCATED output matrix, large enough to store result
    IC      -   submatrix offset
    JC      -   submatrix offset

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB routine --
     2009-2019
     Bochkanov Sergey
*************************************************************************/
void rmatrixgemm(ae_int_t m,
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
void _spawn_rmatrixgemm(ae_int_t m,
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
    ae_int_t jc, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_rmatrixgemm(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_rmatrixgemm(ae_int_t m,
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
    ae_int_t jc, ae_state *_state);


/*************************************************************************
This subroutine is an older version of CMatrixHERK(), one with wrong  name
(it is HErmitian update, not SYmmetric). It  is  left  here  for  backward
compatibility.

  -- ALGLIB routine --
     16.12.2009
     Bochkanov Sergey
*************************************************************************/
void cmatrixsyrk(ae_int_t n,
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
     ae_state *_state);


/*************************************************************************
Performs one step  of stable Gram-Schmidt  process  on  vector  X[]  using
set of orthonormal rows Q[].

INPUT PARAMETERS:
    Q       -   array[M,N], matrix with orthonormal rows
    M, N    -   rows/cols
    X       -   array[N], vector to process
    NeedQX  -   whether we need QX or not 

OUTPUT PARAMETERS:
    X       -   stores X - Q'*(Q*X)
    QX      -   if NeedQX is True, array[M] filled with elements  of  Q*X,
                reallocated if length is less than M.
                Ignored otherwise.
                
NOTE: this function silently exits when M=0, doing nothing

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rowwisegramschmidt(/* Real    */ const ae_matrix* q,
     ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* qx,
     ae_bool needqx,
     ae_state *_state);


/*$ End $*/
#endif

