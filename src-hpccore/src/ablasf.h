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

#ifndef _ablasf_h
#define _ablasf_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"


/*$ Declarations $*/


/*$ Body $*/


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Computes dot product (X,Y) for elements [0,N) of X[] and Y[]

INPUT PARAMETERS:
    N       -   vector length
    X       -   array[N], vector to process
    Y       -   array[N], vector to process

RESULT:
    (X,Y)

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
double rdotv(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Computes dot product (X,A[i]) for elements [0,N) of vector X[] and row A[i,*]

INPUT PARAMETERS:
    N       -   vector length
    X       -   array[N], vector to process
    A       -   array[?,N], matrix to process
    I       -   row index

RESULT:
    (X,Ai)

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
double rdotvr(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Computes dot product (X,A[i]) for rows A[ia,*] and B[ib,*]

INPUT PARAMETERS:
    N       -   vector length
    X       -   array[N], vector to process
    A       -   array[?,N], matrix to process
    I       -   row index

RESULT:
    (X,Ai)

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
double rdotrr(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Computes dot product (X,X) for elements [0,N) of X[]

INPUT PARAMETERS:
    N       -   vector length
    X       -   array[N], vector to process

RESULT:
    (X,X)

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
double rdotv2(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs inplace addition of Y[] to X[]

INPUT PARAMETERS:
    N       -   vector length
    Alpha   -   multiplier
    Y       -   array[N], vector to process
    X       -   array[N], vector to process

RESULT:
    X := X + alpha*Y

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void raddv(ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs inplace addition of Y[]*Z[] to X[]

INPUT PARAMETERS:
    N       -   vector length
    Y       -   array[N], vector to process
    Z       -   array[N], vector to process
    X       -   array[N], vector to process

RESULT:
    X := X + Y*Z

  -- ALGLIB --
     Copyright 29.10.2021 by Bochkanov Sergey
*************************************************************************/
void rmuladdv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* z,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs inplace subtraction of Y[]*Z[] from X[]

INPUT PARAMETERS:
    N       -   vector length
    Y       -   array[N], vector to process
    Z       -   array[N], vector to process
    X       -   array[N], vector to process

RESULT:
    X := X - Y*Z

  -- ALGLIB --
     Copyright 29.10.2021 by Bochkanov Sergey
*************************************************************************/
void rnegmuladdv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* z,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs addition of Y[]*Z[] to X[], with result being stored to R[]

INPUT PARAMETERS:
    N       -   vector length
    Y       -   array[N], vector to process
    Z       -   array[N], vector to process
    X       -   array[N], vector to process
    R       -   array[N], vector to process

RESULT:
    R := X + Y*Z

  -- ALGLIB --
     Copyright 29.10.2021 by Bochkanov Sergey
*************************************************************************/
void rcopymuladdv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* z,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* r,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs subtraction of Y[]*Z[] from X[], with result being stored to R[]

INPUT PARAMETERS:
    N       -   vector length
    Y       -   array[N], vector to process
    Z       -   array[N], vector to process
    X       -   array[N], vector to process
    R       -   array[N], vector to process

RESULT:
    R := X - Y*Z

  -- ALGLIB --
     Copyright 29.10.2021 by Bochkanov Sergey
*************************************************************************/
void rcopynegmuladdv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* z,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* r,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs inplace addition of Y[] to X[]

INPUT PARAMETERS:
    N       -   vector length
    Alpha   -   multiplier
    Y       -   source vector
    OffsY   -   source offset
    X       -   destination vector
    OffsX   -   destination offset

RESULT:
    X := X + alpha*Y

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void raddvx(ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* y,
     ae_int_t offsy,
     /* Real    */ ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
#endif


/*************************************************************************
Performs inplace addition of vector Y[] to column X[]

INPUT PARAMETERS:
    N       -   vector length
    Alpha   -   multiplier
    Y       -   vector to add
    X       -   target column ColIdx

RESULT:
    X := X + alpha*Y

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void raddvc(ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t colidx,
     ae_state *_state);


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs inplace addition of vector Y[] to row X[]

INPUT PARAMETERS:
    N       -   vector length
    Alpha   -   multiplier
    Y       -   vector to add
    X       -   target row RowIdx

RESULT:
    X := X + alpha*Y

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void raddvr(ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs componentwise multiplication of vector X[] by vector Y[]

INPUT PARAMETERS:
    N       -   vector length
    Y       -   vector to multiply by
    X       -   target vector

RESULT:
    X := componentwise(X*Y)

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rmergemulv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs componentwise multiplication of row X[] by vector Y[]

INPUT PARAMETERS:
    N       -   vector length
    Y       -   vector to multiply by
    X       -   target row RowIdx

RESULT:
    X := componentwise(X*Y)

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rmergemulvr(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs componentwise multiplication of row X[] by vector Y[]

INPUT PARAMETERS:
    N       -   vector length
    Y       -   vector to multiply by
    X       -   target row RowIdx

RESULT:
    X := componentwise(X*Y)

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rmergemulrv(ae_int_t n,
     /* Real    */ const ae_matrix* y,
     ae_int_t rowidx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs componentwise division of vector X[] by vector Y[]

INPUT PARAMETERS:
    N       -   vector length
    Y       -   vector to divide by
    X       -   target vector

RESULT:
    X := componentwise(X/Y)

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rmergedivv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs componentwise division of row X[] by vector Y[]

INPUT PARAMETERS:
    N       -   vector length
    Y       -   vector to divide by
    X       -   target row RowIdx

RESULT:
    X := componentwise(X/Y)

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rmergedivvr(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs componentwise division of row X[] by vector Y[]

INPUT PARAMETERS:
    N       -   vector length
    Y       -   vector to divide by
    X       -   target row RowIdx

RESULT:
    X := componentwise(X/Y)

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rmergedivrv(ae_int_t n,
     /* Real    */ const ae_matrix* y,
     ae_int_t rowidx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs componentwise max of vector X[] and vector Y[]

INPUT PARAMETERS:
    N       -   vector length
    Y       -   vector to multiply by
    X       -   target vector

RESULT:
    X := componentwise_max(X,Y)

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rmergemaxv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs componentwise max of row X[] and vector Y[]

INPUT PARAMETERS:
    N       -   vector length
    Y       -   vector to multiply by
    X       -   target row RowIdx

RESULT:
    X := componentwise_max(X,Y)

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rmergemaxvr(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs componentwise max of row X[I] and vector Y[] 

INPUT PARAMETERS:
    N       -   vector length
    X       -   matrix, I-th row is source
    X       -   target row RowIdx

RESULT:
    Y := componentwise_max(Y,X)

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rmergemaxrv(ae_int_t n,
     /* Real    */ const ae_matrix* x,
     ae_int_t rowidx,
     /* Real    */ ae_vector* y,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs componentwise max of vector X[] and vector Y[]

INPUT PARAMETERS:
    N       -   vector length
    Y       -   vector to multiply by
    X       -   target vector

RESULT:
    X := componentwise_max(X,Y)

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rmergeminv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs componentwise max of row X[] and vector Y[]

INPUT PARAMETERS:
    N       -   vector length
    Y       -   vector to multiply by
    X       -   target row RowIdx

RESULT:
    X := componentwise_max(X,Y)

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rmergeminvr(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs componentwise max of row X[I] and vector Y[] 

INPUT PARAMETERS:
    N       -   vector length
    X       -   matrix, I-th row is source
    X       -   target row RowIdx

RESULT:
    X := componentwise_max(X,Y)

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rmergeminrv(ae_int_t n,
     /* Real    */ const ae_matrix* x,
     ae_int_t rowidx,
     /* Real    */ ae_vector* y,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs inplace addition of Y[RIdx,...] to X[]

INPUT PARAMETERS:
    N       -   vector length
    Alpha   -   multiplier
    Y       -   array[?,N], matrix whose RIdx-th row is added
    RIdx    -   row index
    X       -   array[N], vector to process

RESULT:
    X := X + alpha*Y

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void raddrv(ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* y,
     ae_int_t ridx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs inplace addition of Y[RIdx,...] to X[RIdxDst]

INPUT PARAMETERS:
    N       -   vector length
    Alpha   -   multiplier
    Y       -   array[?,N], matrix whose RIdxSrc-th row is added
    RIdxSrc -   source row index
    X       -   array[?,N], matrix whose RIdxDst-th row is target
    RIdxDst -   destination row index

RESULT:
    X := X + alpha*Y

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void raddrr(ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* y,
     ae_int_t ridxsrc,
     /* Real    */ ae_matrix* x,
     ae_int_t ridxdst,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs inplace multiplication of X[] by V

INPUT PARAMETERS:
    N       -   vector length
    X       -   array[N], vector to process
    V       -   multiplier

OUTPUT PARAMETERS:
    X       -   elements 0...N-1 multiplied by V

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rmulv(ae_int_t n,
     double v,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs inplace multiplication of X[] by V

INPUT PARAMETERS:
    N       -   row length
    X       -   array[?,N], row to process
    V       -   multiplier

OUTPUT PARAMETERS:
    X       -   elements 0...N-1 of row RowIdx are multiplied by V

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rmulr(ae_int_t n,
     double v,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs inplace computation of Sqrt(X)

INPUT PARAMETERS:
    N       -   vector length
    X       -   array[N], vector to process

OUTPUT PARAMETERS:
    X       -   elements 0...N-1 replaced by Sqrt(X)

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rsqrtv(ae_int_t n, /* Real    */ ae_vector* x, ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs inplace computation of Sqrt(X[RowIdx,*])

INPUT PARAMETERS:
    N       -   vector length
    X       -   array[?,N], matrix to process

OUTPUT PARAMETERS:
    X       -   elements 0...N-1 replaced by Sqrt(X)

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rsqrtr(ae_int_t n,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs inplace multiplication of X[OffsX:OffsX+N-1] by V

INPUT PARAMETERS:
    N       -   subvector length
    X       -   vector to process
    V       -   multiplier

OUTPUT PARAMETERS:
    X       -   elements OffsX:OffsX+N-1 multiplied by V

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rmulvx(ae_int_t n,
     double v,
     /* Real    */ ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Returns maximum X

INPUT PARAMETERS:
    N       -   vector length
    X       -   array[N], vector to process

OUTPUT PARAMETERS:
    max(X[i])
    zero for N=0

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
double rmaxv(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Returns maximum |X|

INPUT PARAMETERS:
    N       -   vector length
    X       -   array[N], vector to process

OUTPUT PARAMETERS:
    max(|X[i]|)
    zero for N=0

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
double rmaxabsv(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Returns maximum X

INPUT PARAMETERS:
    N       -   vector length
    X       -   matrix to process, RowIdx-th row is processed

OUTPUT PARAMETERS:
    max(X[RowIdx,i])
    zero for N=0

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
double rmaxr(ae_int_t n,
     /* Real    */ const ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Returns maximum |X|

INPUT PARAMETERS:
    N       -   vector length
    X       -   matrix to process, RowIdx-th row is processed

OUTPUT PARAMETERS:
    max(|X[RowIdx,i]|)
    zero for N=0

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
double rmaxabsr(ae_int_t n,
     /* Real    */ const ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Sets vector X[] to V

INPUT PARAMETERS:
    N       -   vector length
    V       -   value to set
    X       -   array[N]

OUTPUT PARAMETERS:
    X       -   leading N elements are replaced by V

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rsetv(ae_int_t n,
     double v,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Sets X[OffsX:OffsX+N-1] to V

INPUT PARAMETERS:
    N       -   subvector length
    V       -   value to set
    X       -   array[N]

OUTPUT PARAMETERS:
    X       -   X[OffsX:OffsX+N-1] is replaced by V

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rsetvx(ae_int_t n,
     double v,
     /* Real    */ ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Sets vector X[] to V

INPUT PARAMETERS:
    N       -   vector length
    V       -   value to set
    X       -   array[N]

OUTPUT PARAMETERS:
    X       -   leading N elements are replaced by V

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void isetv(ae_int_t n,
     ae_int_t v,
     /* Integer */ ae_vector* x,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Sets vector X[] to V

INPUT PARAMETERS:
    N       -   vector length
    V       -   value to set
    X       -   array[N]

OUTPUT PARAMETERS:
    X       -   leading N elements are replaced by V

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void bsetv(ae_int_t n,
     ae_bool v,
     /* Boolean */ ae_vector* x,
     ae_state *_state);
#endif


/*************************************************************************
Sets vector X[] to V

INPUT PARAMETERS:
    N       -   vector length
    V       -   value to set
    X       -   array[N]

OUTPUT PARAMETERS:
    X       -   leading N elements are replaced by V

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void csetv(ae_int_t n,
     ae_complex v,
     /* Complex */ ae_vector* x,
     ae_state *_state);


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Sets matrix A[] to V

INPUT PARAMETERS:
    M, N    -   rows/cols count
    V       -   value to set
    A       -   array[M,N]

OUTPUT PARAMETERS:
    A       -   leading M rows, N cols are replaced by V

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rsetm(ae_int_t m,
     ae_int_t n,
     double v,
     /* Real    */ ae_matrix* a,
     ae_state *_state);
#endif


/*************************************************************************
Sets vector X[] to V, reallocating X[] if too small

INPUT PARAMETERS:
    N       -   vector length
    V       -   value to set
    X       -   possibly preallocated array

OUTPUT PARAMETERS:
    X       -   leading N elements are replaced by V; array is reallocated
                if its length is less than N.

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rsetallocv(ae_int_t n,
     double v,
     /* Real    */ ae_vector* x,
     ae_state *_state);


/*************************************************************************
Sets vector A[] to V, reallocating A[] if too small.

INPUT PARAMETERS:
    M       -   rows count
    N       -   cols count
    V       -   value to set
    A       -   possibly preallocated matrix

OUTPUT PARAMETERS:
    A       -   leading M rows, N cols are replaced by V; the matrix is
                reallocated if its rows/cols count is less than M/N.

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rsetallocm(ae_int_t m,
     ae_int_t n,
     double v,
     /* Real    */ ae_matrix* a,
     ae_state *_state);


/*************************************************************************
Reallocates X[] if its length is less than required value. Does not change
its length and contents if it is large enough.

INPUT PARAMETERS:
    N       -   desired vector length
    X       -   possibly preallocated array

OUTPUT PARAMETERS:
    X       -   length(X)>=N

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rallocv(ae_int_t n, /* Real    */ ae_vector* x, ae_state *_state);


/*************************************************************************
Reallocates X[] if its length is less than required value. Does not change
its length and contents if it is large enough.

INPUT PARAMETERS:
    N       -   desired vector length
    X       -   possibly preallocated array

OUTPUT PARAMETERS:
    X       -   length(X)>=N

  -- ALGLIB --
     Copyright 20.07.2022 by Bochkanov Sergey
*************************************************************************/
void callocv(ae_int_t n, /* Complex */ ae_vector* x, ae_state *_state);


/*************************************************************************
Reallocates X[] if its length is less than required value. Does not change
its length and contents if it is large enough.

INPUT PARAMETERS:
    N       -   desired vector length
    X       -   possibly preallocated array

OUTPUT PARAMETERS:
    X       -   length(X)>=N

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void iallocv(ae_int_t n, /* Integer */ ae_vector* x, ae_state *_state);


/*************************************************************************
Reallocates X[] if its length is less than required value. Does not change
its length and contents if it is large enough.

INPUT PARAMETERS:
    N       -   desired vector length
    X       -   possibly preallocated array

OUTPUT PARAMETERS:
    X       -   length(X)>=N

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void ballocv(ae_int_t n, /* Boolean */ ae_vector* x, ae_state *_state);


/*************************************************************************
Reallocates matrix if its rows or cols count is less than  required.  Does
not change its size if it is exactly that size or larger.

INPUT PARAMETERS:
    M       -   rows count
    N       -   cols count
    A       -   possibly preallocated matrix

OUTPUT PARAMETERS:
    A       -   size is at least M*N

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rallocm(ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* a,
     ae_state *_state);


/*************************************************************************
Sets vector X[] to V, reallocating X[] if too small

INPUT PARAMETERS:
    N       -   vector length
    V       -   value to set
    X       -   possibly preallocated array

OUTPUT PARAMETERS:
    X       -   leading N elements are replaced by V; array is reallocated
                if its length is less than N.

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void isetallocv(ae_int_t n,
     ae_int_t v,
     /* Integer */ ae_vector* x,
     ae_state *_state);


/*************************************************************************
Sets vector X[] to V, reallocating X[] if too small

INPUT PARAMETERS:
    N       -   vector length
    V       -   value to set
    X       -   possibly preallocated array

OUTPUT PARAMETERS:
    X       -   leading N elements are replaced by V; array is reallocated
                if its length is less than N.

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void bsetallocv(ae_int_t n,
     ae_bool v,
     /* Boolean */ ae_vector* x,
     ae_state *_state);


/*************************************************************************
Sets vector X[] to V, reallocating X[] if too small

INPUT PARAMETERS:
    N       -   vector length
    V       -   value to set
    X       -   possibly preallocated array

OUTPUT PARAMETERS:
    X       -   leading N elements are replaced by V; array is reallocated
                if its length is less than N.

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void csetallocv(ae_int_t n,
     ae_complex v,
     /* Complex */ ae_vector* x,
     ae_state *_state);


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Sets row I of A[,] to V

INPUT PARAMETERS:
    N       -   vector length
    V       -   value to set
    A       -   array[N,N] or larger
    I       -   row index

OUTPUT PARAMETERS:
    A       -   leading N elements of I-th row are replaced by V

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rsetr(ae_int_t n,
     double v,
     /* Real    */ ae_matrix* a,
     ae_int_t i,
     ae_state *_state);
#endif


/*************************************************************************
Sets col J of A[,] to V

INPUT PARAMETERS:
    N       -   vector length
    V       -   value to set
    A       -   array[N,N] or larger
    J       -   col index

OUTPUT PARAMETERS:
    A       -   leading N elements of I-th col are replaced by V

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rsetc(ae_int_t n,
     double v,
     /* Real    */ ae_matrix* a,
     ae_int_t j,
     ae_state *_state);


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Copies vector X[] to Y[]

INPUT PARAMETERS:
    N       -   vector length
    X       -   array[N], source
    Y       -   preallocated array[N]

OUTPUT PARAMETERS:
    Y       -   leading N elements are replaced by X

    
NOTE: destination and source should NOT overlap

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rcopyv(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Copies vector X[] to Y[]

INPUT PARAMETERS:
    N       -   vector length
    X       -   array[N], source
    Y       -   preallocated array[N]

OUTPUT PARAMETERS:
    Y       -   leading N elements are replaced by X

    
NOTE: destination and source should NOT overlap

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void bcopyv(ae_int_t n,
     /* Boolean */ const ae_vector* x,
     /* Boolean */ ae_vector* y,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Copies vector X[] to Y[], extended version

INPUT PARAMETERS:
    N       -   vector length
    X       -   source array
    OffsX   -   source offset
    Y       -   preallocated array[N]
    OffsY   -   destination offset

OUTPUT PARAMETERS:
    Y       -   N elements starting from OffsY are replaced by X[OffsX:OffsX+N-1]
    
NOTE: destination and source should NOT overlap

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rcopyvx(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_int_t offsx,
     /* Real    */ ae_vector* y,
     ae_int_t offsy,
     ae_state *_state);
#endif


/*************************************************************************
Copies vector X[] to Y[], resizing Y[] if needed.

INPUT PARAMETERS:
    N       -   vector length
    X       -   array[N], source
    Y       -   possibly preallocated array[N] (resized if needed)

OUTPUT PARAMETERS:
    Y       -   leading N elements are replaced by X

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rcopyallocv(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);


/*************************************************************************
Copies matrix X[] to Y[], resizing Y[] if needed. On resize, dimensions of
Y[] are increased - but not decreased.

INPUT PARAMETERS:
    M       -   rows count
    N       -   cols count
    X       -   array[M,N], source
    Y       -   possibly preallocated array[M,N] (resized if needed)

OUTPUT PARAMETERS:
    Y       -   leading [M,N] elements are replaced by X

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rcopym(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* x,
     /* Real    */ ae_matrix* y,
     ae_state *_state);


/*************************************************************************
Copies matrix X[] to Y[], resizing Y[] if needed. On resize, dimensions of
Y[] are increased - but not decreased.

INPUT PARAMETERS:
    M       -   rows count
    N       -   cols count
    X       -   array[M,N], source
    Y       -   possibly preallocated array[M,N] (resized if needed)

OUTPUT PARAMETERS:
    Y       -   leading [M,N] elements are replaced by X

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rcopyallocm(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* x,
     /* Real    */ ae_matrix* y,
     ae_state *_state);


/*************************************************************************
Copies vector X[] to Y[], resizing Y[] if needed.

INPUT PARAMETERS:
    N       -   vector length
    X       -   array[N], source
    Y       -   possibly preallocated array[N] (resized if needed)

OUTPUT PARAMETERS:
    Y       -   leading N elements are replaced by X

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void icopyallocv(ae_int_t n,
     /* Integer */ const ae_vector* x,
     /* Integer */ ae_vector* y,
     ae_state *_state);


/*************************************************************************
Copies vector X[] to Y[], resizing Y[] if needed.

INPUT PARAMETERS:
    N       -   vector length
    X       -   array[N], source
    Y       -   possibly preallocated array[N] (resized if needed)

OUTPUT PARAMETERS:
    Y       -   leading N elements are replaced by X

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void bcopyallocv(ae_int_t n,
     /* Boolean */ const ae_vector* x,
     /* Boolean */ ae_vector* y,
     ae_state *_state);


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Copies vector X[] to Y[]

INPUT PARAMETERS:
    N       -   vector length
    X       -   source array
    Y       -   preallocated array[N]

OUTPUT PARAMETERS:
    Y       -   X copied to Y

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void icopyv(ae_int_t n,
     /* Integer */ const ae_vector* x,
     /* Integer */ ae_vector* y,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Copies vector X[] to Y[], extended version

INPUT PARAMETERS:
    N       -   vector length
    X       -   source array
    OffsX   -   source offset
    Y       -   preallocated array[N]
    OffsY   -   destination offset

OUTPUT PARAMETERS:
    Y       -   N elements starting from OffsY are replaced by X[OffsX:OffsX+N-1]
    
NOTE: destination and source should NOT overlap

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void icopyvx(ae_int_t n,
     /* Integer */ const ae_vector* x,
     ae_int_t offsx,
     /* Integer */ ae_vector* y,
     ae_int_t offsy,
     ae_state *_state);
#endif


/*************************************************************************
Grows X, i.e. changes its size in such a way that:
a) contents is preserved
b) new size is at least N
c) actual size can be larger than N, so subsequent grow() calls can return
   without reallocation

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void igrowv(ae_int_t newn, /* Integer */ ae_vector* x, ae_state *_state);


/*************************************************************************
Grows X, i.e. changes its size in such a way that:
a) contents is preserved
b) new size is at least N
c) actual size can be larger than N, so subsequent grow() calls can return
   without reallocation

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void rgrowv(ae_int_t newn, /* Real    */ ae_vector* x, ae_state *_state);


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs copying with multiplication of V*X[] to Y[]

INPUT PARAMETERS:
    N       -   vector length
    V       -   multiplier
    X       -   array[N], source
    Y       -   preallocated array[N]

OUTPUT PARAMETERS:
    Y       -   array[N], Y = V*X

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rcopymulv(ae_int_t n,
     double v,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Performs copying with multiplication of V*X[] to Y[I,*]

INPUT PARAMETERS:
    N       -   vector length
    V       -   multiplier
    X       -   array[N], source
    Y       -   preallocated array[?,N]
    RIdx    -   destination row index

OUTPUT PARAMETERS:
    Y       -   Y[RIdx,...] = V*X

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rcopymulvr(ae_int_t n,
     double v,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* y,
     ae_int_t ridx,
     ae_state *_state);
#endif


/*************************************************************************
Performs copying with multiplication of V*X[] to Y[*,J]

INPUT PARAMETERS:
    N       -   vector length
    V       -   multiplier
    X       -   array[N], source
    Y       -   preallocated array[N,?]
    CIdx    -   destination rocol index

OUTPUT PARAMETERS:
    Y       -   Y[RIdx,...] = V*X

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rcopymulvc(ae_int_t n,
     double v,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* y,
     ae_int_t cidx,
     ae_state *_state);


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Copies vector X[] to row I of A[,]

INPUT PARAMETERS:
    N       -   vector length
    X       -   array[N], source
    A       -   preallocated 2D array large enough to store result
    I       -   destination row index

OUTPUT PARAMETERS:
    A       -   leading N elements of I-th row are replaced by X

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rcopyvr(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* a,
     ae_int_t i,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Copies row I of A[,] to vector X[]

INPUT PARAMETERS:
    N       -   vector length
    A       -   2D array, source
    I       -   source row index
    X       -   preallocated destination

OUTPUT PARAMETERS:
    X       -   array[N], destination

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rcopyrv(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Copies row I of A[,] to row K of B[,].

A[i,...] and B[k,...] may overlap.

INPUT PARAMETERS:
    N       -   vector length
    A       -   2D array, source
    I       -   source row index
    B       -   preallocated destination
    K       -   destination row index

OUTPUT PARAMETERS:
    B       -   row K overwritten

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rcopyrr(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     /* Real    */ ae_matrix* b,
     ae_int_t k,
     ae_state *_state);
#endif


/*************************************************************************
Copies vector X[] to column J of A[,]

INPUT PARAMETERS:
    N       -   vector length
    X       -   array[N], source
    A       -   preallocated 2D array large enough to store result
    J       -   destination col index

OUTPUT PARAMETERS:
    A       -   leading N elements of J-th column are replaced by X

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rcopyvc(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* a,
     ae_int_t j,
     ae_state *_state);


/*************************************************************************
Copies column J of A[,] to vector X[]

INPUT PARAMETERS:
    N       -   vector length
    A       -   source 2D array
    J       -   source col index

OUTPUT PARAMETERS:
    X       -   preallocated array[N], destination

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rcopycv(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t j,
     /* Real    */ ae_vector* x,
     ae_state *_state);


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Matrix-vector product: y := alpha*op(A)*x + beta*y

NOTE: this  function  expects  Y  to  be  large enough to store result. No
      automatic preallocation happens for  smaller  arrays.  No  integrity
      checks is performed for sizes of A, x, y.

INPUT PARAMETERS:
    M   -   number of rows of op(A)
    N   -   number of columns of op(A)
    Alpha-  coefficient
    A   -   source matrix
    OpA -   operation type:
            * OpA=0     =>  op(A) = A
            * OpA=1     =>  op(A) = A^T
    X   -   input vector, has at least N elements
    Beta-   coefficient
    Y   -   preallocated output array, has at least M elements

OUTPUT PARAMETERS:
    Y   -   vector which stores result

HANDLING OF SPECIAL CASES:
    * if M=0, then subroutine does nothing. It does not even touch arrays.
    * if N=0 or Alpha=0.0, then:
      * if Beta=0, then Y is filled by zeros. A and X are  not  referenced
        at all. Initial values of Y are ignored (we do not  multiply  Y by
        zero, we just rewrite it by zeros)
      * if Beta<>0, then Y is replaced by Beta*Y
    * if M>0, N>0, Alpha<>0, but  Beta=0,  then  Y  is  replaced  by  A*x;
       initial state of Y is ignored (rewritten by  A*x,  without  initial
       multiplication by zeros).


  -- ALGLIB routine --

     01.09.2021
     Bochkanov Sergey
*************************************************************************/
void rgemv(ae_int_t m,
     ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t opa,
     /* Real    */ const ae_vector* x,
     double beta,
     /* Real    */ ae_vector* y,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Matrix-vector product: y := alpha*op(A)*x + beta*y

Here x, y, A are subvectors/submatrices of larger vectors/matrices.

NOTE: this  function  expects  Y  to  be  large enough to store result. No
      automatic preallocation happens for  smaller  arrays.  No  integrity
      checks is performed for sizes of A, x, y.

INPUT PARAMETERS:
    M   -   number of rows of op(A)
    N   -   number of columns of op(A)
    Alpha-  coefficient
    A   -   source matrix
    IA  -   submatrix offset (row index)
    JA  -   submatrix offset (column index)
    OpA -   operation type:
            * OpA=0     =>  op(A) = A
            * OpA=1     =>  op(A) = A^T
    X   -   input vector, has at least N+IX elements
    IX  -   subvector offset
    Beta-   coefficient
    Y   -   preallocated output array, has at least M+IY elements
    IY  -   subvector offset

OUTPUT PARAMETERS:
    Y   -   vector which stores result

HANDLING OF SPECIAL CASES:
    * if M=0, then subroutine does nothing. It does not even touch arrays.
    * if N=0 or Alpha=0.0, then:
      * if Beta=0, then Y is filled by zeros. A and X are  not  referenced
        at all. Initial values of Y are ignored (we do not  multiply  Y by
        zero, we just rewrite it by zeros)
      * if Beta<>0, then Y is replaced by Beta*Y
    * if M>0, N>0, Alpha<>0, but  Beta=0,  then  Y  is  replaced  by  A*x;
       initial state of Y is ignored (rewritten by  A*x,  without  initial
       multiplication by zeros).


  -- ALGLIB routine --

     01.09.2021
     Bochkanov Sergey
*************************************************************************/
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
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Rank-1 correction: A := A + alpha*u*v'

NOTE: this  function  expects  A  to  be  large enough to store result. No
      automatic preallocation happens for  smaller  arrays.  No  integrity
      checks is performed for sizes of A, u, v.

INPUT PARAMETERS:
    M   -   number of rows
    N   -   number of columns
    A   -   target MxN matrix
    Alpha-  coefficient
    U   -   vector #1
    V   -   vector #2


  -- ALGLIB routine --
     07.09.2021
     Bochkanov Sergey
*************************************************************************/
void rger(ae_int_t m,
     ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* u,
     /* Real    */ const ae_vector* v,
     /* Real    */ ae_matrix* a,
     ae_state *_state);
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
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

  -- ALGLIB routine --
     (c) 07.09.2021 Bochkanov Sergey
*************************************************************************/
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
#endif


/*************************************************************************
Fast kernel

  -- ALGLIB routine --
     19.01.2010
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixgerf(ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     double ralpha,
     /* Real    */ const ae_vector* u,
     ae_int_t iu,
     /* Real    */ const ae_vector* v,
     ae_int_t iv,
     ae_state *_state);


/*************************************************************************
Fast kernel

  -- ALGLIB routine --
     19.01.2010
     Bochkanov Sergey
*************************************************************************/
ae_bool cmatrixrank1f(ae_int_t m,
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
Fast kernel

  -- ALGLIB routine --
     19.01.2010
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixrank1f(ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Real    */ const ae_vector* u,
     ae_int_t iu,
     /* Real    */ const ae_vector* v,
     ae_int_t iv,
     ae_state *_state);


/*************************************************************************
Fast kernel

  -- ALGLIB routine --
     19.01.2010
     Bochkanov Sergey
*************************************************************************/
ae_bool cmatrixrighttrsmf(ae_int_t m,
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


/*************************************************************************
Fast kernel

  -- ALGLIB routine --
     19.01.2010
     Bochkanov Sergey
*************************************************************************/
ae_bool cmatrixlefttrsmf(ae_int_t m,
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


/*************************************************************************
Fast kernel

  -- ALGLIB routine --
     19.01.2010
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixrighttrsmf(ae_int_t m,
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


/*************************************************************************
Fast kernel

  -- ALGLIB routine --
     19.01.2010
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixlefttrsmf(ae_int_t m,
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


/*************************************************************************
Fast kernel

  -- ALGLIB routine --
     19.01.2010
     Bochkanov Sergey
*************************************************************************/
ae_bool cmatrixherkf(ae_int_t n,
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
Fast kernel

  -- ALGLIB routine --
     19.01.2010
     Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixsyrkf(ae_int_t n,
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


/*************************************************************************
Fast kernel

  -- ALGLIB routine --
     19.01.2010
     Bochkanov Sergey
*************************************************************************/
ae_bool cmatrixgemmf(ae_int_t m,
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


/*************************************************************************
CMatrixGEMM kernel, basecase code for CMatrixGEMM.

This subroutine calculates C = alpha*op1(A)*op2(B) +beta*C where:
* C is MxN general matrix
* op1(A) is MxK matrix
* op2(B) is KxN matrix
* "op" may be identity transformation, transposition, conjugate transposition

Additional info:
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
    C       -   PREALLOCATED output matrix
    IC      -   submatrix offset
    JC      -   submatrix offset

  -- ALGLIB routine --
     27.03.2013
     Bochkanov Sergey
*************************************************************************/
void cmatrixgemmk(ae_int_t m,
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


/*************************************************************************
RMatrixGEMM kernel, basecase code for RMatrixGEMM.

This subroutine calculates C = alpha*op1(A)*op2(B) +beta*C where:
* C is MxN general matrix
* op1(A) is MxK matrix
* op2(B) is KxN matrix
* "op" may be identity transformation, transposition

Additional info:
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
    C       -   PREALLOCATED output matrix
    IC      -   submatrix offset
    JC      -   submatrix offset

  -- ALGLIB routine --
     27.03.2013
     Bochkanov Sergey
*************************************************************************/
void rmatrixgemmk(ae_int_t m,
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


/*************************************************************************
RMatrixGEMM kernel, basecase code for RMatrixGEMM, specialized for sitation
with OpTypeA=0 and OpTypeB=0.

Additional info:
* this function requires that Alpha<>0 (assertion is thrown otherwise)

INPUT PARAMETERS
    M       -   matrix size, M>0
    N       -   matrix size, N>0
    K       -   matrix size, K>0
    Alpha   -   coefficient
    A       -   matrix
    IA      -   submatrix offset
    JA      -   submatrix offset
    B       -   matrix
    IB      -   submatrix offset
    JB      -   submatrix offset
    Beta    -   coefficient
    C       -   PREALLOCATED output matrix
    IC      -   submatrix offset
    JC      -   submatrix offset

  -- ALGLIB routine --
     27.03.2013
     Bochkanov Sergey
*************************************************************************/
void rmatrixgemmk44v00(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     double beta,
     /* Real    */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state);


/*************************************************************************
RMatrixGEMM kernel, basecase code for RMatrixGEMM, specialized for sitation
with OpTypeA=0 and OpTypeB=1.

Additional info:
* this function requires that Alpha<>0 (assertion is thrown otherwise)

INPUT PARAMETERS
    M       -   matrix size, M>0
    N       -   matrix size, N>0
    K       -   matrix size, K>0
    Alpha   -   coefficient
    A       -   matrix
    IA      -   submatrix offset
    JA      -   submatrix offset
    B       -   matrix
    IB      -   submatrix offset
    JB      -   submatrix offset
    Beta    -   coefficient
    C       -   PREALLOCATED output matrix
    IC      -   submatrix offset
    JC      -   submatrix offset

  -- ALGLIB routine --
     27.03.2013
     Bochkanov Sergey
*************************************************************************/
void rmatrixgemmk44v01(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     double beta,
     /* Real    */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state);


/*************************************************************************
RMatrixGEMM kernel, basecase code for RMatrixGEMM, specialized for sitation
with OpTypeA=1 and OpTypeB=0.

Additional info:
* this function requires that Alpha<>0 (assertion is thrown otherwise)

INPUT PARAMETERS
    M       -   matrix size, M>0
    N       -   matrix size, N>0
    K       -   matrix size, K>0
    Alpha   -   coefficient
    A       -   matrix
    IA      -   submatrix offset
    JA      -   submatrix offset
    B       -   matrix
    IB      -   submatrix offset
    JB      -   submatrix offset
    Beta    -   coefficient
    C       -   PREALLOCATED output matrix
    IC      -   submatrix offset
    JC      -   submatrix offset

  -- ALGLIB routine --
     27.03.2013
     Bochkanov Sergey
*************************************************************************/
void rmatrixgemmk44v10(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     double beta,
     /* Real    */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state);


/*************************************************************************
RMatrixGEMM kernel, basecase code for RMatrixGEMM, specialized for sitation
with OpTypeA=1 and OpTypeB=1.

Additional info:
* this function requires that Alpha<>0 (assertion is thrown otherwise)

INPUT PARAMETERS
    M       -   matrix size, M>0
    N       -   matrix size, N>0
    K       -   matrix size, K>0
    Alpha   -   coefficient
    A       -   matrix
    IA      -   submatrix offset
    JA      -   submatrix offset
    B       -   matrix
    IB      -   submatrix offset
    JB      -   submatrix offset
    Beta    -   coefficient
    C       -   PREALLOCATED output matrix
    IC      -   submatrix offset
    JC      -   submatrix offset

  -- ALGLIB routine --
     27.03.2013
     Bochkanov Sergey
*************************************************************************/
void rmatrixgemmk44v11(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     double beta,
     /* Real    */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state);


/*$ End $*/
#endif

