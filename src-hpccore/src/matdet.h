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

#ifndef _matdet_h
#define _matdet_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "scodes.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "tsort.h"
#include "sparse.h"
#include "dlu.h"
#include "sptrf.h"
#include "apstruct.h"
#include "amdordering.h"
#include "spchol.h"
#include "creflections.h"
#include "matgen.h"
#include "rotations.h"
#include "trfac.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Determinant calculation of the matrix given by its LU decomposition.

Input parameters:
    A       -   LU decomposition of the matrix (output of
                RMatrixLU subroutine).
    Pivots  -   table of permutations which were made during
                the LU decomposition.
                Output of RMatrixLU subroutine.
    N       -   (optional) size of matrix A:
                * if given, only principal NxN submatrix is processed and
                  overwritten. other elements are unchanged.
                * if not given, automatically determined from matrix size
                  (A must be square matrix)

Result: matrix determinant.

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
double rmatrixludet(/* Real    */ const ae_matrix* a,
     /* Integer */ const ae_vector* pivots,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Calculation of the determinant of a general matrix

Input parameters:
    A       -   matrix, array[0..N-1, 0..N-1]
    N       -   (optional) size of matrix A:
                * if given, only principal NxN submatrix is processed and
                  overwritten. other elements are unchanged.
                * if not given, automatically determined from matrix size
                  (A must be square matrix)

Result: determinant of matrix A.

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
double rmatrixdet(/* Real    */ const ae_matrix* _a,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Determinant calculation of the matrix given by its LU decomposition.

Input parameters:
    A       -   LU decomposition of the matrix (output of
                RMatrixLU subroutine).
    Pivots  -   table of permutations which were made during
                the LU decomposition.
                Output of RMatrixLU subroutine.
    N       -   (optional) size of matrix A:
                * if given, only principal NxN submatrix is processed and
                  overwritten. other elements are unchanged.
                * if not given, automatically determined from matrix size
                  (A must be square matrix)

Result: matrix determinant.

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
ae_complex cmatrixludet(/* Complex */ const ae_matrix* a,
     /* Integer */ const ae_vector* pivots,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Calculation of the determinant of a general matrix

Input parameters:
    A       -   matrix, array[0..N-1, 0..N-1]
    N       -   (optional) size of matrix A:
                * if given, only principal NxN submatrix is processed and
                  overwritten. other elements are unchanged.
                * if not given, automatically determined from matrix size
                  (A must be square matrix)

Result: determinant of matrix A.

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
ae_complex cmatrixdet(/* Complex */ const ae_matrix* _a,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Determinant calculation of the matrix given by the Cholesky decomposition.

Input parameters:
    A       -   Cholesky decomposition,
                output of SMatrixCholesky subroutine.
    N       -   (optional) size of matrix A:
                * if given, only principal NxN submatrix is processed and
                  overwritten. other elements are unchanged.
                * if not given, automatically determined from matrix size
                  (A must be square matrix)

As the determinant is equal to the product of squares of diagonal elements,
it's not necessary to specify which triangle - lower or upper - the matrix
is stored in.

Result:
    matrix determinant.

  -- ALGLIB --
     Copyright 2005-2008 by Bochkanov Sergey
*************************************************************************/
double spdmatrixcholeskydet(/* Real    */ const ae_matrix* a,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Determinant calculation of the symmetric positive definite matrix.

Input parameters:
    A       -   matrix, array[N,N]
    N       -   (optional) size of matrix A:
                * if given, only principal NxN submatrix is processed and
                  overwritten. other elements are unchanged.
                * if not given, automatically determined from matrix size
                  (A must be square matrix)
    IsUpper -   storage type:
                * if True, symmetric matrix  A  is  given  by  its  upper
                  triangle, and the lower triangle isn't used/changed  by
                  function
                * if False, symmetric matrix  A  is  given  by  its lower
                  triangle, and the upper triangle isn't used/changed  by
                  function

Result:
    determinant of matrix A.
    If matrix A is not positive definite, an exception is generated.

  -- ALGLIB --
     Copyright 2005-2008 by Bochkanov Sergey
*************************************************************************/
double spdmatrixdet(/* Real    */ const ae_matrix* _a,
     ae_int_t n,
     ae_bool isupper,
     ae_state *_state);


/*$ End $*/
#endif

