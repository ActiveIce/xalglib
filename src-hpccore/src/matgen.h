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

#ifndef _matgen_h
#define _matgen_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "creflections.h"
#include "xdebug.h"
#include "hqrnd.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Generation of a random uniformly distributed (Haar) orthogonal matrix

INPUT PARAMETERS:
    N   -   matrix size, N>=1
    
OUTPUT PARAMETERS:
    A   -   orthogonal NxN matrix, array[0..N-1,0..N-1]

NOTE: this function uses algorithm  described  in  Stewart, G. W.  (1980),
      "The Efficient Generation of  Random  Orthogonal  Matrices  with  an
      Application to Condition Estimators".
      
      Speaking short, to generate an (N+1)x(N+1) orthogonal matrix, it:
      * takes an NxN one
      * takes uniformly distributed unit vector of dimension N+1.
      * constructs a Householder reflection from the vector, then applies
        it to the smaller matrix (embedded in the larger size with a 1 at
        the bottom right corner).

  -- ALGLIB routine --
     04.12.2009
     Bochkanov Sergey
*************************************************************************/
void rmatrixrndorthogonal(ae_int_t n,
     /* Real    */ ae_matrix* a,
     ae_state *_state);


/*************************************************************************
Generation of random NxN matrix with given condition number and norm2(A)=1

INPUT PARAMETERS:
    N   -   matrix size
    C   -   condition number (in 2-norm)

OUTPUT PARAMETERS:
    A   -   random matrix with norm2(A)=1 and cond(A)=C

  -- ALGLIB routine --
     04.12.2009
     Bochkanov Sergey
*************************************************************************/
void rmatrixrndcond(ae_int_t n,
     double c,
     /* Real    */ ae_matrix* a,
     ae_state *_state);


/*************************************************************************
Generation of a random Haar distributed orthogonal complex matrix

INPUT PARAMETERS:
    N   -   matrix size, N>=1

OUTPUT PARAMETERS:
    A   -   orthogonal NxN matrix, array[0..N-1,0..N-1]

NOTE: this function uses algorithm  described  in  Stewart, G. W.  (1980),
      "The Efficient Generation of  Random  Orthogonal  Matrices  with  an
      Application to Condition Estimators".
      
      Speaking short, to generate an (N+1)x(N+1) orthogonal matrix, it:
      * takes an NxN one
      * takes uniformly distributed unit vector of dimension N+1.
      * constructs a Householder reflection from the vector, then applies
        it to the smaller matrix (embedded in the larger size with a 1 at
        the bottom right corner).

  -- ALGLIB routine --
     04.12.2009
     Bochkanov Sergey
*************************************************************************/
void cmatrixrndorthogonal(ae_int_t n,
     /* Complex */ ae_matrix* a,
     ae_state *_state);


/*************************************************************************
Generation of random NxN complex matrix with given condition number C and
norm2(A)=1

INPUT PARAMETERS:
    N   -   matrix size
    C   -   condition number (in 2-norm)

OUTPUT PARAMETERS:
    A   -   random matrix with norm2(A)=1 and cond(A)=C

  -- ALGLIB routine --
     04.12.2009
     Bochkanov Sergey
*************************************************************************/
void cmatrixrndcond(ae_int_t n,
     double c,
     /* Complex */ ae_matrix* a,
     ae_state *_state);


/*************************************************************************
Generation of random NxN symmetric matrix with given condition number  and
norm2(A)=1

INPUT PARAMETERS:
    N   -   matrix size
    C   -   condition number (in 2-norm)

OUTPUT PARAMETERS:
    A   -   random matrix with norm2(A)=1 and cond(A)=C

  -- ALGLIB routine --
     04.12.2009
     Bochkanov Sergey
*************************************************************************/
void smatrixrndcond(ae_int_t n,
     double c,
     /* Real    */ ae_matrix* a,
     ae_state *_state);


/*************************************************************************
Generation of random NxN symmetric positive definite matrix with given
condition number and norm2(A)=1

INPUT PARAMETERS:
    N   -   matrix size
    C   -   condition number (in 2-norm)

OUTPUT PARAMETERS:
    A   -   random SPD matrix with norm2(A)=1 and cond(A)=C

  -- ALGLIB routine --
     04.12.2009
     Bochkanov Sergey
*************************************************************************/
void spdmatrixrndcond(ae_int_t n,
     double c,
     /* Real    */ ae_matrix* a,
     ae_state *_state);


/*************************************************************************
Generation of random NxN Hermitian matrix with given condition number  and
norm2(A)=1

INPUT PARAMETERS:
    N   -   matrix size
    C   -   condition number (in 2-norm)

OUTPUT PARAMETERS:
    A   -   random matrix with norm2(A)=1 and cond(A)=C

  -- ALGLIB routine --
     04.12.2009
     Bochkanov Sergey
*************************************************************************/
void hmatrixrndcond(ae_int_t n,
     double c,
     /* Complex */ ae_matrix* a,
     ae_state *_state);


/*************************************************************************
Generation of random NxN Hermitian positive definite matrix with given
condition number and norm2(A)=1

INPUT PARAMETERS:
    N   -   matrix size
    C   -   condition number (in 2-norm)

OUTPUT PARAMETERS:
    A   -   random HPD matrix with norm2(A)=1 and cond(A)=C

  -- ALGLIB routine --
     04.12.2009
     Bochkanov Sergey
*************************************************************************/
void hpdmatrixrndcond(ae_int_t n,
     double c,
     /* Complex */ ae_matrix* a,
     ae_state *_state);


/*************************************************************************
Multiplication of MxN matrix by NxN random Haar distributed orthogonal matrix

INPUT PARAMETERS:
    A   -   matrix, array[0..M-1, 0..N-1]
    M, N-   matrix size

OUTPUT PARAMETERS:
    A   -   A*Q, where Q is random NxN orthogonal matrix

  -- ALGLIB routine --
     04.12.2009
     Bochkanov Sergey
*************************************************************************/
void rmatrixrndorthogonalfromtheright(/* Real    */ ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Multiplication of MxN matrix by MxM random Haar distributed orthogonal matrix

INPUT PARAMETERS:
    A   -   matrix, array[0..M-1, 0..N-1]
    M, N-   matrix size

OUTPUT PARAMETERS:
    A   -   Q*A, where Q is random MxM orthogonal matrix

  -- ALGLIB routine --
     04.12.2009
     Bochkanov Sergey
*************************************************************************/
void rmatrixrndorthogonalfromtheleft(/* Real    */ ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Multiplication of MxN complex matrix by NxN random Haar distributed
complex orthogonal matrix

INPUT PARAMETERS:
    A   -   matrix, array[0..M-1, 0..N-1]
    M, N-   matrix size

OUTPUT PARAMETERS:
    A   -   A*Q, where Q is random NxN orthogonal matrix

  -- ALGLIB routine --
     04.12.2009
     Bochkanov Sergey
*************************************************************************/
void cmatrixrndorthogonalfromtheright(/* Complex */ ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Multiplication of MxN complex matrix by MxM random Haar distributed
complex orthogonal matrix

INPUT PARAMETERS:
    A   -   matrix, array[0..M-1, 0..N-1]
    M, N-   matrix size

OUTPUT PARAMETERS:
    A   -   Q*A, where Q is random MxM orthogonal matrix

  -- ALGLIB routine --
     04.12.2009
     Bochkanov Sergey
*************************************************************************/
void cmatrixrndorthogonalfromtheleft(/* Complex */ ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Symmetric multiplication of NxN matrix by random Haar distributed
orthogonal  matrix

INPUT PARAMETERS:
    A   -   matrix, array[0..N-1, 0..N-1]
    N   -   matrix size

OUTPUT PARAMETERS:
    A   -   Q'*A*Q, where Q is random NxN orthogonal matrix

  -- ALGLIB routine --
     04.12.2009
     Bochkanov Sergey
*************************************************************************/
void smatrixrndmultiply(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Hermitian multiplication of NxN matrix by random Haar distributed
complex orthogonal matrix

INPUT PARAMETERS:
    A   -   matrix, array[0..N-1, 0..N-1]
    N   -   matrix size

OUTPUT PARAMETERS:
    A   -   Q^H*A*Q, where Q is random NxN orthogonal matrix

  -- ALGLIB routine --
     04.12.2009
     Bochkanov Sergey
*************************************************************************/
void hmatrixrndmultiply(/* Complex */ ae_matrix* a,
     ae_int_t n,
     ae_state *_state);


/*$ End $*/
#endif

