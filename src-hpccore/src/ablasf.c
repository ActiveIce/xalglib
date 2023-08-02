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


#include <stdafx.h>
#include "ablasf.h"


/*$ Declarations $*/
#ifdef ALGLIB_NO_FAST_KERNELS
static ae_bool ablasf_rgemm32basecase(ae_int_t m,
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
#endif


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
     ae_state *_state)
{
    ae_int_t i;
    double result;


    result = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        result = result+x->ptr.p_double[i]*y->ptr.p_double[i];
    }
    return result;
}
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
     ae_state *_state)
{
    ae_int_t j;
    double result;


    result = (double)(0);
    for(j=0; j<=n-1; j++)
    {
        result = result+x->ptr.p_double[j]*a->ptr.pp_double[i][j];
    }
    return result;
}
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
     ae_state *_state)
{
    ae_int_t j;
    double result;


    result = (double)(0);
    for(j=0; j<=n-1; j++)
    {
        result = result+a->ptr.pp_double[ia][j]*b->ptr.pp_double[ib][j];
    }
    return result;
}
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
     ae_state *_state)
{
    ae_int_t i;
    double v;
    double result;


    result = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        v = x->ptr.p_double[i];
        result = result+v*v;
    }
    return result;
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = x->ptr.p_double[i]+alpha*y->ptr.p_double[i];
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = x->ptr.p_double[i]+y->ptr.p_double[i]*z->ptr.p_double[i];
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = x->ptr.p_double[i]-y->ptr.p_double[i]*z->ptr.p_double[i];
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        r->ptr.p_double[i] = x->ptr.p_double[i]+y->ptr.p_double[i]*z->ptr.p_double[i];
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        r->ptr.p_double[i] = x->ptr.p_double[i]-y->ptr.p_double[i]*z->ptr.p_double[i];
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[offsx+i] = x->ptr.p_double[offsx+i]+alpha*y->ptr.p_double[offsy+i];
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.pp_double[i][colidx] = x->ptr.pp_double[i][colidx]+alpha*y->ptr.p_double[i];
    }
}


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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.pp_double[rowidx][i] = x->ptr.pp_double[rowidx][i]+alpha*y->ptr.p_double[i];
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = x->ptr.p_double[i]*y->ptr.p_double[i];
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.pp_double[rowidx][i] = x->ptr.pp_double[rowidx][i]*y->ptr.p_double[i];
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = x->ptr.p_double[i]*y->ptr.pp_double[rowidx][i];
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = x->ptr.p_double[i]/y->ptr.p_double[i];
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.pp_double[rowidx][i] = x->ptr.pp_double[rowidx][i]/y->ptr.p_double[i];
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = x->ptr.p_double[i]/y->ptr.pp_double[rowidx][i];
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = ae_maxreal(x->ptr.p_double[i], y->ptr.p_double[i], _state);
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.pp_double[rowidx][i] = ae_maxreal(x->ptr.pp_double[rowidx][i], y->ptr.p_double[i], _state);
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        y->ptr.p_double[i] = ae_maxreal(y->ptr.p_double[i], x->ptr.pp_double[rowidx][i], _state);
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = ae_minreal(x->ptr.p_double[i], y->ptr.p_double[i], _state);
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.pp_double[rowidx][i] = ae_minreal(x->ptr.pp_double[rowidx][i], y->ptr.p_double[i], _state);
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        y->ptr.p_double[i] = ae_minreal(y->ptr.p_double[i], x->ptr.pp_double[rowidx][i], _state);
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = x->ptr.p_double[i]+alpha*y->ptr.pp_double[ridx][i];
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.pp_double[ridxdst][i] = x->ptr.pp_double[ridxdst][i]+alpha*y->ptr.pp_double[ridxsrc][i];
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = x->ptr.p_double[i]*v;
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.pp_double[rowidx][i] = x->ptr.pp_double[rowidx][i]*v;
    }
}
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
void rsqrtv(ae_int_t n, /* Real    */ ae_vector* x, ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = ae_sqrt(x->ptr.p_double[i], _state);
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.pp_double[rowidx][i] = ae_sqrt(x->ptr.pp_double[rowidx][i], _state);
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[offsx+i] = x->ptr.p_double[offsx+i]*v;
    }
}
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
     ae_state *_state)
{
    ae_int_t i;
    double v;
    double result;


    if( n<=0 )
    {
        result = (double)(0);
        return result;
    }
    result = x->ptr.p_double[0];
    for(i=1; i<=n-1; i++)
    {
        v = x->ptr.p_double[i];
        if( v>result )
        {
            result = v;
        }
    }
    return result;
}
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
     ae_state *_state)
{
    ae_int_t i;
    double v;
    double result;


    result = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        v = ae_fabs(x->ptr.p_double[i], _state);
        if( v>result )
        {
            result = v;
        }
    }
    return result;
}
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
     ae_state *_state)
{
    ae_int_t i;
    double v;
    double result;


    if( n<=0 )
    {
        result = (double)(0);
        return result;
    }
    result = x->ptr.pp_double[rowidx][0];
    for(i=1; i<=n-1; i++)
    {
        v = x->ptr.pp_double[rowidx][i];
        if( v>result )
        {
            result = v;
        }
    }
    return result;
}
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
     ae_state *_state)
{
    ae_int_t i;
    double v;
    double result;


    result = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        v = ae_fabs(x->ptr.pp_double[rowidx][i], _state);
        if( v>result )
        {
            result = v;
        }
    }
    return result;
}
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
     ae_state *_state)
{
    ae_int_t j;


    for(j=0; j<=n-1; j++)
    {
        x->ptr.p_double[j] = v;
    }
}
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
     ae_state *_state)
{
    ae_int_t j;


    for(j=0; j<=n-1; j++)
    {
        x->ptr.p_double[offsx+j] = v;
    }
}
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
     ae_state *_state)
{
    ae_int_t j;


    for(j=0; j<=n-1; j++)
    {
        x->ptr.p_int[j] = v;
    }
}
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
     ae_state *_state)
{
    ae_int_t j;


    for(j=0; j<=n-1; j++)
    {
        x->ptr.p_bool[j] = v;
    }
}
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
     ae_state *_state)
{
    ae_int_t j;


    for(j=0; j<=n-1; j++)
    {
        x->ptr.p_complex[j].x = v.x;
        x->ptr.p_complex[j].y = v.y;
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;


    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            a->ptr.pp_double[i][j] = v;
        }
    }
}
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
     ae_state *_state)
{


    if( x->cnt<n )
    {
        ae_vector_set_length(x, n, _state);
    }
    rsetv(n, v, x, _state);
}


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
     ae_state *_state)
{


    if( a->rows<m||a->cols<n )
    {
        ae_matrix_set_length(a, m, n, _state);
    }
    rsetm(m, n, v, a, _state);
}


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
void rallocv(ae_int_t n, /* Real    */ ae_vector* x, ae_state *_state)
{


    if( x->cnt<n )
    {
        ae_vector_set_length(x, n, _state);
    }
}


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
void callocv(ae_int_t n, /* Complex */ ae_vector* x, ae_state *_state)
{


    if( x->cnt<n )
    {
        ae_vector_set_length(x, n, _state);
    }
}


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
void iallocv(ae_int_t n, /* Integer */ ae_vector* x, ae_state *_state)
{


    if( x->cnt<n )
    {
        ae_vector_set_length(x, n, _state);
    }
}


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
void ballocv(ae_int_t n, /* Boolean */ ae_vector* x, ae_state *_state)
{


    if( x->cnt<n )
    {
        ae_vector_set_length(x, n, _state);
    }
}


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
     ae_state *_state)
{


    if( a->rows<m||a->cols<n )
    {
        ae_matrix_set_length(a, m, n, _state);
    }
}


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
     ae_state *_state)
{


    if( x->cnt<n )
    {
        ae_vector_set_length(x, n, _state);
    }
    isetv(n, v, x, _state);
}


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
     ae_state *_state)
{


    if( x->cnt<n )
    {
        ae_vector_set_length(x, n, _state);
    }
    bsetv(n, v, x, _state);
}


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
     ae_state *_state)
{


    if( x->cnt<n )
    {
        ae_vector_set_length(x, n, _state);
    }
    csetv(n, v, x, _state);
}


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
     ae_state *_state)
{
    ae_int_t j;


    for(j=0; j<=n-1; j++)
    {
        a->ptr.pp_double[i][j] = v;
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        a->ptr.pp_double[i][j] = v;
    }
}


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
     ae_state *_state)
{
    ae_int_t j;


    for(j=0; j<=n-1; j++)
    {
        y->ptr.p_double[j] = x->ptr.p_double[j];
    }
}
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
     ae_state *_state)
{
    ae_int_t j;


    for(j=0; j<=n-1; j++)
    {
        y->ptr.p_bool[j] = x->ptr.p_bool[j];
    }
}
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
     ae_state *_state)
{
    ae_int_t j;


    for(j=0; j<=n-1; j++)
    {
        y->ptr.p_double[offsy+j] = x->ptr.p_double[offsx+j];
    }
}
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
     ae_state *_state)
{


    if( y->cnt<n )
    {
        ae_vector_set_length(y, n, _state);
    }
    rcopyv(n, x, y, _state);
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;


    if( m==0||n==0 )
    {
        return;
    }
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            y->ptr.pp_double[i][j] = x->ptr.pp_double[i][j];
        }
    }
}


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
     ae_state *_state)
{


    if( m==0||n==0 )
    {
        return;
    }
    if( y->rows<m||y->cols<n )
    {
        ae_matrix_set_length(y, ae_maxint(m, y->rows, _state), ae_maxint(n, y->cols, _state), _state);
    }
    rcopym(m, n, x, y, _state);
}


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
     ae_state *_state)
{


    if( y->cnt<n )
    {
        ae_vector_set_length(y, n, _state);
    }
    icopyv(n, x, y, _state);
}


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
     ae_state *_state)
{


    if( y->cnt<n )
    {
        ae_vector_set_length(y, n, _state);
    }
    bcopyv(n, x, y, _state);
}


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
     ae_state *_state)
{
    ae_int_t j;


    for(j=0; j<=n-1; j++)
    {
        y->ptr.p_int[j] = x->ptr.p_int[j];
    }
}
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
     ae_state *_state)
{
    ae_int_t j;


    for(j=0; j<=n-1; j++)
    {
        y->ptr.p_int[offsy+j] = x->ptr.p_int[offsx+j];
    }
}
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
void igrowv(ae_int_t newn, /* Integer */ ae_vector* x, ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector oldx;
    ae_int_t oldn;

    ae_frame_make(_state, &_frame_block);
    memset(&oldx, 0, sizeof(oldx));
    ae_vector_init(&oldx, 0, DT_INT, _state, ae_true);

    if( x->cnt>=newn )
    {
        ae_frame_leave(_state);
        return;
    }
    oldn = x->cnt;
    newn = ae_maxint(newn, ae_round(1.8*(double)oldn+(double)1, _state), _state);
    ae_swap_vectors(x, &oldx);
    ae_vector_set_length(x, newn, _state);
    icopyv(oldn, &oldx, x, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Grows X, i.e. changes its size in such a way that:
a) contents is preserved
b) new size is at least N
c) actual size can be larger than N, so subsequent grow() calls can return
   without reallocation

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void rgrowv(ae_int_t newn, /* Real    */ ae_vector* x, ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector oldx;
    ae_int_t oldn;

    ae_frame_make(_state, &_frame_block);
    memset(&oldx, 0, sizeof(oldx));
    ae_vector_init(&oldx, 0, DT_REAL, _state, ae_true);

    if( x->cnt>=newn )
    {
        ae_frame_leave(_state);
        return;
    }
    oldn = x->cnt;
    newn = ae_maxint(newn, ae_round(1.8*(double)oldn+(double)1, _state), _state);
    ae_swap_vectors(x, &oldx);
    ae_vector_set_length(x, newn, _state);
    rcopyv(oldn, &oldx, x, _state);
    ae_frame_leave(_state);
}


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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        y->ptr.p_double[i] = v*x->ptr.p_double[i];
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        y->ptr.pp_double[ridx][i] = v*x->ptr.p_double[i];
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        y->ptr.pp_double[i][cidx] = v*x->ptr.p_double[i];
    }
}


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
     ae_state *_state)
{
    ae_int_t j;


    for(j=0; j<=n-1; j++)
    {
        a->ptr.pp_double[i][j] = x->ptr.p_double[j];
    }
}
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
     ae_state *_state)
{
    ae_int_t j;


    for(j=0; j<=n-1; j++)
    {
        x->ptr.p_double[j] = a->ptr.pp_double[i][j];
    }
}
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
     ae_state *_state)
{
    ae_int_t j;


    for(j=0; j<=n-1; j++)
    {
        b->ptr.pp_double[k][j] = a->ptr.pp_double[i][j];
    }
}
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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        a->ptr.pp_double[i][j] = x->ptr.p_double[i];
    }
}


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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = a->ptr.pp_double[i][j];
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;


    
    /*
     * Properly premultiply Y by Beta.
     *
     * Quick exit for M=0, N=0 or Alpha=0.
     * After this block we have M>0, N>0, Alpha<>0.
     */
    if( m<=0 )
    {
        return;
    }
    if( ae_fp_neq(beta,(double)(0)) )
    {
        rmulv(m, beta, y, _state);
    }
    else
    {
        rsetv(m, 0.0, y, _state);
    }
    if( n<=0||ae_fp_eq(alpha,0.0) )
    {
        return;
    }
    
    /*
     * Generic code
     */
    if( opa==0 )
    {
        
        /*
         * y += A*x
         */
        for(i=0; i<=m-1; i++)
        {
            v = (double)(0);
            for(j=0; j<=n-1; j++)
            {
                v = v+a->ptr.pp_double[i][j]*x->ptr.p_double[j];
            }
            y->ptr.p_double[i] = alpha*v+y->ptr.p_double[i];
        }
        return;
    }
    if( opa==1 )
    {
        
        /*
         * y += A^T*x
         */
        for(i=0; i<=n-1; i++)
        {
            v = alpha*x->ptr.p_double[i];
            for(j=0; j<=m-1; j++)
            {
                y->ptr.p_double[j] = y->ptr.p_double[j]+v*a->ptr.pp_double[i][j];
            }
        }
        return;
    }
}
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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;


    
    /*
     * Properly premultiply Y by Beta.
     *
     * Quick exit for M=0, N=0 or Alpha=0.
     * After this block we have M>0, N>0, Alpha<>0.
     */
    if( m<=0 )
    {
        return;
    }
    if( ae_fp_neq(beta,(double)(0)) )
    {
        rmulvx(m, beta, y, iy, _state);
    }
    else
    {
        rsetvx(m, 0.0, y, iy, _state);
    }
    if( n<=0||ae_fp_eq(alpha,0.0) )
    {
        return;
    }
    
    /*
     * Generic code
     */
    if( opa==0 )
    {
        
        /*
         * y += A*x
         */
        for(i=0; i<=m-1; i++)
        {
            v = (double)(0);
            for(j=0; j<=n-1; j++)
            {
                v = v+a->ptr.pp_double[ia+i][ja+j]*x->ptr.p_double[ix+j];
            }
            y->ptr.p_double[iy+i] = alpha*v+y->ptr.p_double[iy+i];
        }
        return;
    }
    if( opa==1 )
    {
        
        /*
         * y += A^T*x
         */
        for(i=0; i<=n-1; i++)
        {
            v = alpha*x->ptr.p_double[ix+i];
            for(j=0; j<=m-1; j++)
            {
                y->ptr.p_double[iy+j] = y->ptr.p_double[iy+j]+v*a->ptr.pp_double[ia+i][ja+j];
            }
        }
        return;
    }
}
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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double s;


    if( (m<=0||n<=0)||ae_fp_eq(alpha,(double)(0)) )
    {
        return;
    }
    for(i=0; i<=m-1; i++)
    {
        s = alpha*u->ptr.p_double[i];
        for(j=0; j<=n-1; j++)
        {
            a->ptr.pp_double[i][j] = a->ptr.pp_double[i][j]+s*v->ptr.p_double[j];
        }
    }
}
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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;


    if( n<=0 )
    {
        return;
    }
    if( optype==0&&isupper )
    {
        for(i=n-1; i>=0; i--)
        {
            v = x->ptr.p_double[ix+i];
            for(j=i+1; j<=n-1; j++)
            {
                v = v-a->ptr.pp_double[ia+i][ja+j]*x->ptr.p_double[ix+j];
            }
            if( !isunit )
            {
                v = v/a->ptr.pp_double[ia+i][ja+i];
            }
            x->ptr.p_double[ix+i] = v;
        }
        return;
    }
    if( optype==0&&!isupper )
    {
        for(i=0; i<=n-1; i++)
        {
            v = x->ptr.p_double[ix+i];
            for(j=0; j<=i-1; j++)
            {
                v = v-a->ptr.pp_double[ia+i][ja+j]*x->ptr.p_double[ix+j];
            }
            if( !isunit )
            {
                v = v/a->ptr.pp_double[ia+i][ja+i];
            }
            x->ptr.p_double[ix+i] = v;
        }
        return;
    }
    if( optype==1&&isupper )
    {
        for(i=0; i<=n-1; i++)
        {
            v = x->ptr.p_double[ix+i];
            if( !isunit )
            {
                v = v/a->ptr.pp_double[ia+i][ja+i];
            }
            x->ptr.p_double[ix+i] = v;
            if( v==(double)0 )
            {
                continue;
            }
            for(j=i+1; j<=n-1; j++)
            {
                x->ptr.p_double[ix+j] = x->ptr.p_double[ix+j]-v*a->ptr.pp_double[ia+i][ja+j];
            }
        }
        return;
    }
    if( optype==1&&!isupper )
    {
        for(i=n-1; i>=0; i--)
        {
            v = x->ptr.p_double[ix+i];
            if( !isunit )
            {
                v = v/a->ptr.pp_double[ia+i][ja+i];
            }
            x->ptr.p_double[ix+i] = v;
            if( v==(double)0 )
            {
                continue;
            }
            for(j=0; j<=i-1; j++)
            {
                x->ptr.p_double[ix+j] = x->ptr.p_double[ix+j]-v*a->ptr.pp_double[ia+i][ja+j];
            }
        }
        return;
    }
    ae_assert(ae_false, "rTRSVX: unexpected operation type", _state);
}
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
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_ABLAS
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixgerf(m, n, a, ia, ja, ralpha, u, iu, v, iv);
#endif
}


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
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_ABLAS
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_cmatrixrank1f(m, n, a, ia, ja, u, iu, v, iv);
#endif
}


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
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_ABLAS
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixrank1f(m, n, a, ia, ja, u, iu, v, iv);
#endif
}


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
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_ABLAS
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_cmatrixrighttrsmf(m, n, a, i1, j1, isupper, isunit, optype, x, i2, j2);
#endif
}


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
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_ABLAS
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_cmatrixlefttrsmf(m, n, a, i1, j1, isupper, isunit, optype, x, i2, j2);
#endif
}


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
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_ABLAS
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixrighttrsmf(m, n, a, i1, j1, isupper, isunit, optype, x, i2, j2);
#endif
}


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
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_ABLAS
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixlefttrsmf(m, n, a, i1, j1, isupper, isunit, optype, x, i2, j2);
#endif
}


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
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_ABLAS
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_cmatrixherkf(n, k, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper);
#endif
}


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
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_ABLAS
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_rmatrixsyrkf(n, k, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper);
#endif
}


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
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_ABLAS
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_cmatrixgemmf(m, n, k, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc);
#endif
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_complex v;
    ae_complex v00;
    ae_complex v01;
    ae_complex v10;
    ae_complex v11;
    double v00x;
    double v00y;
    double v01x;
    double v01y;
    double v10x;
    double v10y;
    double v11x;
    double v11y;
    double a0x;
    double a0y;
    double a1x;
    double a1y;
    double b0x;
    double b0y;
    double b1x;
    double b1y;
    ae_int_t idxa0;
    ae_int_t idxa1;
    ae_int_t idxb0;
    ae_int_t idxb1;
    ae_int_t i0;
    ae_int_t i1;
    ae_int_t ik;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t jk;
    ae_int_t t;
    ae_int_t offsa;
    ae_int_t offsb;


    
    /*
     * if matrix size is zero
     */
    if( m==0||n==0 )
    {
        return;
    }
    
    /*
     * Try optimized code
     */
    if( cmatrixgemmf(m, n, k, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _state) )
    {
        return;
    }
    
    /*
     * if K=0 or Alpha=0, then C=Beta*C
     */
    if( k==0||ae_c_eq_d(alpha,(double)(0)) )
    {
        if( ae_c_neq_d(beta,(double)(1)) )
        {
            if( ae_c_neq_d(beta,(double)(0)) )
            {
                for(i=0; i<=m-1; i++)
                {
                    for(j=0; j<=n-1; j++)
                    {
                        c->ptr.pp_complex[ic+i][jc+j] = ae_c_mul(beta,c->ptr.pp_complex[ic+i][jc+j]);
                    }
                }
            }
            else
            {
                for(i=0; i<=m-1; i++)
                {
                    for(j=0; j<=n-1; j++)
                    {
                        c->ptr.pp_complex[ic+i][jc+j] = ae_complex_from_i(0);
                    }
                }
            }
        }
        return;
    }
    
    /*
     * This phase is not really necessary, but compiler complains
     * about "possibly uninitialized variables"
     */
    a0x = (double)(0);
    a0y = (double)(0);
    a1x = (double)(0);
    a1y = (double)(0);
    b0x = (double)(0);
    b0y = (double)(0);
    b1x = (double)(0);
    b1y = (double)(0);
    
    /*
     * General case
     */
    i = 0;
    while(i<m)
    {
        j = 0;
        while(j<n)
        {
            
            /*
             * Choose between specialized 4x4 code and general code
             */
            if( i+2<=m&&j+2<=n )
            {
                
                /*
                 * Specialized 4x4 code for [I..I+3]x[J..J+3] submatrix of C.
                 *
                 * This submatrix is calculated as sum of K rank-1 products,
                 * with operands cached in local variables in order to speed
                 * up operations with arrays.
                 */
                v00x = 0.0;
                v00y = 0.0;
                v01x = 0.0;
                v01y = 0.0;
                v10x = 0.0;
                v10y = 0.0;
                v11x = 0.0;
                v11y = 0.0;
                if( optypea==0 )
                {
                    idxa0 = ia+i+0;
                    idxa1 = ia+i+1;
                    offsa = ja;
                }
                else
                {
                    idxa0 = ja+i+0;
                    idxa1 = ja+i+1;
                    offsa = ia;
                }
                if( optypeb==0 )
                {
                    idxb0 = jb+j+0;
                    idxb1 = jb+j+1;
                    offsb = ib;
                }
                else
                {
                    idxb0 = ib+j+0;
                    idxb1 = ib+j+1;
                    offsb = jb;
                }
                for(t=0; t<=k-1; t++)
                {
                    if( optypea==0 )
                    {
                        a0x = a->ptr.pp_complex[idxa0][offsa].x;
                        a0y = a->ptr.pp_complex[idxa0][offsa].y;
                        a1x = a->ptr.pp_complex[idxa1][offsa].x;
                        a1y = a->ptr.pp_complex[idxa1][offsa].y;
                    }
                    if( optypea==1 )
                    {
                        a0x = a->ptr.pp_complex[offsa][idxa0].x;
                        a0y = a->ptr.pp_complex[offsa][idxa0].y;
                        a1x = a->ptr.pp_complex[offsa][idxa1].x;
                        a1y = a->ptr.pp_complex[offsa][idxa1].y;
                    }
                    if( optypea==2 )
                    {
                        a0x = a->ptr.pp_complex[offsa][idxa0].x;
                        a0y = -a->ptr.pp_complex[offsa][idxa0].y;
                        a1x = a->ptr.pp_complex[offsa][idxa1].x;
                        a1y = -a->ptr.pp_complex[offsa][idxa1].y;
                    }
                    if( optypeb==0 )
                    {
                        b0x = b->ptr.pp_complex[offsb][idxb0].x;
                        b0y = b->ptr.pp_complex[offsb][idxb0].y;
                        b1x = b->ptr.pp_complex[offsb][idxb1].x;
                        b1y = b->ptr.pp_complex[offsb][idxb1].y;
                    }
                    if( optypeb==1 )
                    {
                        b0x = b->ptr.pp_complex[idxb0][offsb].x;
                        b0y = b->ptr.pp_complex[idxb0][offsb].y;
                        b1x = b->ptr.pp_complex[idxb1][offsb].x;
                        b1y = b->ptr.pp_complex[idxb1][offsb].y;
                    }
                    if( optypeb==2 )
                    {
                        b0x = b->ptr.pp_complex[idxb0][offsb].x;
                        b0y = -b->ptr.pp_complex[idxb0][offsb].y;
                        b1x = b->ptr.pp_complex[idxb1][offsb].x;
                        b1y = -b->ptr.pp_complex[idxb1][offsb].y;
                    }
                    v00x = v00x+a0x*b0x-a0y*b0y;
                    v00y = v00y+a0x*b0y+a0y*b0x;
                    v01x = v01x+a0x*b1x-a0y*b1y;
                    v01y = v01y+a0x*b1y+a0y*b1x;
                    v10x = v10x+a1x*b0x-a1y*b0y;
                    v10y = v10y+a1x*b0y+a1y*b0x;
                    v11x = v11x+a1x*b1x-a1y*b1y;
                    v11y = v11y+a1x*b1y+a1y*b1x;
                    offsa = offsa+1;
                    offsb = offsb+1;
                }
                v00.x = v00x;
                v00.y = v00y;
                v10.x = v10x;
                v10.y = v10y;
                v01.x = v01x;
                v01.y = v01y;
                v11.x = v11x;
                v11.y = v11y;
                if( ae_c_eq_d(beta,(double)(0)) )
                {
                    c->ptr.pp_complex[ic+i+0][jc+j+0] = ae_c_mul(alpha,v00);
                    c->ptr.pp_complex[ic+i+0][jc+j+1] = ae_c_mul(alpha,v01);
                    c->ptr.pp_complex[ic+i+1][jc+j+0] = ae_c_mul(alpha,v10);
                    c->ptr.pp_complex[ic+i+1][jc+j+1] = ae_c_mul(alpha,v11);
                }
                else
                {
                    c->ptr.pp_complex[ic+i+0][jc+j+0] = ae_c_add(ae_c_mul(beta,c->ptr.pp_complex[ic+i+0][jc+j+0]),ae_c_mul(alpha,v00));
                    c->ptr.pp_complex[ic+i+0][jc+j+1] = ae_c_add(ae_c_mul(beta,c->ptr.pp_complex[ic+i+0][jc+j+1]),ae_c_mul(alpha,v01));
                    c->ptr.pp_complex[ic+i+1][jc+j+0] = ae_c_add(ae_c_mul(beta,c->ptr.pp_complex[ic+i+1][jc+j+0]),ae_c_mul(alpha,v10));
                    c->ptr.pp_complex[ic+i+1][jc+j+1] = ae_c_add(ae_c_mul(beta,c->ptr.pp_complex[ic+i+1][jc+j+1]),ae_c_mul(alpha,v11));
                }
            }
            else
            {
                
                /*
                 * Determine submatrix [I0..I1]x[J0..J1] to process
                 */
                i0 = i;
                i1 = ae_minint(i+1, m-1, _state);
                j0 = j;
                j1 = ae_minint(j+1, n-1, _state);
                
                /*
                 * Process submatrix
                 */
                for(ik=i0; ik<=i1; ik++)
                {
                    for(jk=j0; jk<=j1; jk++)
                    {
                        if( k==0||ae_c_eq_d(alpha,(double)(0)) )
                        {
                            v = ae_complex_from_i(0);
                        }
                        else
                        {
                            v = ae_complex_from_d(0.0);
                            if( optypea==0&&optypeb==0 )
                            {
                                v = ae_v_cdotproduct(&a->ptr.pp_complex[ia+ik][ja], 1, "N", &b->ptr.pp_complex[ib][jb+jk], b->stride, "N", ae_v_len(ja,ja+k-1));
                            }
                            if( optypea==0&&optypeb==1 )
                            {
                                v = ae_v_cdotproduct(&a->ptr.pp_complex[ia+ik][ja], 1, "N", &b->ptr.pp_complex[ib+jk][jb], 1, "N", ae_v_len(ja,ja+k-1));
                            }
                            if( optypea==0&&optypeb==2 )
                            {
                                v = ae_v_cdotproduct(&a->ptr.pp_complex[ia+ik][ja], 1, "N", &b->ptr.pp_complex[ib+jk][jb], 1, "Conj", ae_v_len(ja,ja+k-1));
                            }
                            if( optypea==1&&optypeb==0 )
                            {
                                v = ae_v_cdotproduct(&a->ptr.pp_complex[ia][ja+ik], a->stride, "N", &b->ptr.pp_complex[ib][jb+jk], b->stride, "N", ae_v_len(ia,ia+k-1));
                            }
                            if( optypea==1&&optypeb==1 )
                            {
                                v = ae_v_cdotproduct(&a->ptr.pp_complex[ia][ja+ik], a->stride, "N", &b->ptr.pp_complex[ib+jk][jb], 1, "N", ae_v_len(ia,ia+k-1));
                            }
                            if( optypea==1&&optypeb==2 )
                            {
                                v = ae_v_cdotproduct(&a->ptr.pp_complex[ia][ja+ik], a->stride, "N", &b->ptr.pp_complex[ib+jk][jb], 1, "Conj", ae_v_len(ia,ia+k-1));
                            }
                            if( optypea==2&&optypeb==0 )
                            {
                                v = ae_v_cdotproduct(&a->ptr.pp_complex[ia][ja+ik], a->stride, "Conj", &b->ptr.pp_complex[ib][jb+jk], b->stride, "N", ae_v_len(ia,ia+k-1));
                            }
                            if( optypea==2&&optypeb==1 )
                            {
                                v = ae_v_cdotproduct(&a->ptr.pp_complex[ia][ja+ik], a->stride, "Conj", &b->ptr.pp_complex[ib+jk][jb], 1, "N", ae_v_len(ia,ia+k-1));
                            }
                            if( optypea==2&&optypeb==2 )
                            {
                                v = ae_v_cdotproduct(&a->ptr.pp_complex[ia][ja+ik], a->stride, "Conj", &b->ptr.pp_complex[ib+jk][jb], 1, "Conj", ae_v_len(ia,ia+k-1));
                            }
                        }
                        if( ae_c_eq_d(beta,(double)(0)) )
                        {
                            c->ptr.pp_complex[ic+ik][jc+jk] = ae_c_mul(alpha,v);
                        }
                        else
                        {
                            c->ptr.pp_complex[ic+ik][jc+jk] = ae_c_add(ae_c_mul(beta,c->ptr.pp_complex[ic+ik][jc+jk]),ae_c_mul(alpha,v));
                        }
                    }
                }
            }
            j = j+2;
        }
        i = i+2;
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;


    
    /*
     * if matrix size is zero
     */
    if( m==0||n==0 )
    {
        return;
    }
    
    /*
     * Try optimized code
     */
    if( ablasf_rgemm32basecase(m, n, k, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc, _state) )
    {
        return;
    }
    
    /*
     * if K=0 or Alpha=0, then C=Beta*C
     */
    if( k==0||ae_fp_eq(alpha,(double)(0)) )
    {
        if( ae_fp_neq(beta,(double)(1)) )
        {
            if( ae_fp_neq(beta,(double)(0)) )
            {
                for(i=0; i<=m-1; i++)
                {
                    for(j=0; j<=n-1; j++)
                    {
                        c->ptr.pp_double[ic+i][jc+j] = beta*c->ptr.pp_double[ic+i][jc+j];
                    }
                }
            }
            else
            {
                for(i=0; i<=m-1; i++)
                {
                    for(j=0; j<=n-1; j++)
                    {
                        c->ptr.pp_double[ic+i][jc+j] = (double)(0);
                    }
                }
            }
        }
        return;
    }
    
    /*
     * Call specialized code.
     *
     * NOTE: specialized code was moved to separate function because of strange
     *       issues with instructions cache on some systems; Having too long
     *       functions significantly slows down internal loop of the algorithm.
     */
    if( optypea==0&&optypeb==0 )
    {
        rmatrixgemmk44v00(m, n, k, alpha, a, ia, ja, b, ib, jb, beta, c, ic, jc, _state);
    }
    if( optypea==0&&optypeb!=0 )
    {
        rmatrixgemmk44v01(m, n, k, alpha, a, ia, ja, b, ib, jb, beta, c, ic, jc, _state);
    }
    if( optypea!=0&&optypeb==0 )
    {
        rmatrixgemmk44v10(m, n, k, alpha, a, ia, ja, b, ib, jb, beta, c, ic, jc, _state);
    }
    if( optypea!=0&&optypeb!=0 )
    {
        rmatrixgemmk44v11(m, n, k, alpha, a, ia, ja, b, ib, jb, beta, c, ic, jc, _state);
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;
    double v00;
    double v01;
    double v02;
    double v03;
    double v10;
    double v11;
    double v12;
    double v13;
    double v20;
    double v21;
    double v22;
    double v23;
    double v30;
    double v31;
    double v32;
    double v33;
    double a0;
    double a1;
    double a2;
    double a3;
    double b0;
    double b1;
    double b2;
    double b3;
    ae_int_t idxa0;
    ae_int_t idxa1;
    ae_int_t idxa2;
    ae_int_t idxa3;
    ae_int_t idxb0;
    ae_int_t idxb1;
    ae_int_t idxb2;
    ae_int_t idxb3;
    ae_int_t i0;
    ae_int_t i1;
    ae_int_t ik;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t jk;
    ae_int_t t;
    ae_int_t offsa;
    ae_int_t offsb;


    ae_assert(ae_fp_neq(alpha,(double)(0)), "RMatrixGEMMK44V00: internal error (Alpha=0)", _state);
    
    /*
     * if matrix size is zero
     */
    if( m==0||n==0 )
    {
        return;
    }
    
    /*
     * A*B
     */
    i = 0;
    while(i<m)
    {
        j = 0;
        while(j<n)
        {
            
            /*
             * Choose between specialized 4x4 code and general code
             */
            if( i+4<=m&&j+4<=n )
            {
                
                /*
                 * Specialized 4x4 code for [I..I+3]x[J..J+3] submatrix of C.
                 *
                 * This submatrix is calculated as sum of K rank-1 products,
                 * with operands cached in local variables in order to speed
                 * up operations with arrays.
                 */
                idxa0 = ia+i+0;
                idxa1 = ia+i+1;
                idxa2 = ia+i+2;
                idxa3 = ia+i+3;
                offsa = ja;
                idxb0 = jb+j+0;
                idxb1 = jb+j+1;
                idxb2 = jb+j+2;
                idxb3 = jb+j+3;
                offsb = ib;
                v00 = 0.0;
                v01 = 0.0;
                v02 = 0.0;
                v03 = 0.0;
                v10 = 0.0;
                v11 = 0.0;
                v12 = 0.0;
                v13 = 0.0;
                v20 = 0.0;
                v21 = 0.0;
                v22 = 0.0;
                v23 = 0.0;
                v30 = 0.0;
                v31 = 0.0;
                v32 = 0.0;
                v33 = 0.0;
                
                /*
                 * Different variants of internal loop
                 */
                for(t=0; t<=k-1; t++)
                {
                    a0 = a->ptr.pp_double[idxa0][offsa];
                    a1 = a->ptr.pp_double[idxa1][offsa];
                    b0 = b->ptr.pp_double[offsb][idxb0];
                    b1 = b->ptr.pp_double[offsb][idxb1];
                    v00 = v00+a0*b0;
                    v01 = v01+a0*b1;
                    v10 = v10+a1*b0;
                    v11 = v11+a1*b1;
                    a2 = a->ptr.pp_double[idxa2][offsa];
                    a3 = a->ptr.pp_double[idxa3][offsa];
                    v20 = v20+a2*b0;
                    v21 = v21+a2*b1;
                    v30 = v30+a3*b0;
                    v31 = v31+a3*b1;
                    b2 = b->ptr.pp_double[offsb][idxb2];
                    b3 = b->ptr.pp_double[offsb][idxb3];
                    v22 = v22+a2*b2;
                    v23 = v23+a2*b3;
                    v32 = v32+a3*b2;
                    v33 = v33+a3*b3;
                    v02 = v02+a0*b2;
                    v03 = v03+a0*b3;
                    v12 = v12+a1*b2;
                    v13 = v13+a1*b3;
                    offsa = offsa+1;
                    offsb = offsb+1;
                }
                if( ae_fp_eq(beta,(double)(0)) )
                {
                    c->ptr.pp_double[ic+i+0][jc+j+0] = alpha*v00;
                    c->ptr.pp_double[ic+i+0][jc+j+1] = alpha*v01;
                    c->ptr.pp_double[ic+i+0][jc+j+2] = alpha*v02;
                    c->ptr.pp_double[ic+i+0][jc+j+3] = alpha*v03;
                    c->ptr.pp_double[ic+i+1][jc+j+0] = alpha*v10;
                    c->ptr.pp_double[ic+i+1][jc+j+1] = alpha*v11;
                    c->ptr.pp_double[ic+i+1][jc+j+2] = alpha*v12;
                    c->ptr.pp_double[ic+i+1][jc+j+3] = alpha*v13;
                    c->ptr.pp_double[ic+i+2][jc+j+0] = alpha*v20;
                    c->ptr.pp_double[ic+i+2][jc+j+1] = alpha*v21;
                    c->ptr.pp_double[ic+i+2][jc+j+2] = alpha*v22;
                    c->ptr.pp_double[ic+i+2][jc+j+3] = alpha*v23;
                    c->ptr.pp_double[ic+i+3][jc+j+0] = alpha*v30;
                    c->ptr.pp_double[ic+i+3][jc+j+1] = alpha*v31;
                    c->ptr.pp_double[ic+i+3][jc+j+2] = alpha*v32;
                    c->ptr.pp_double[ic+i+3][jc+j+3] = alpha*v33;
                }
                else
                {
                    c->ptr.pp_double[ic+i+0][jc+j+0] = beta*c->ptr.pp_double[ic+i+0][jc+j+0]+alpha*v00;
                    c->ptr.pp_double[ic+i+0][jc+j+1] = beta*c->ptr.pp_double[ic+i+0][jc+j+1]+alpha*v01;
                    c->ptr.pp_double[ic+i+0][jc+j+2] = beta*c->ptr.pp_double[ic+i+0][jc+j+2]+alpha*v02;
                    c->ptr.pp_double[ic+i+0][jc+j+3] = beta*c->ptr.pp_double[ic+i+0][jc+j+3]+alpha*v03;
                    c->ptr.pp_double[ic+i+1][jc+j+0] = beta*c->ptr.pp_double[ic+i+1][jc+j+0]+alpha*v10;
                    c->ptr.pp_double[ic+i+1][jc+j+1] = beta*c->ptr.pp_double[ic+i+1][jc+j+1]+alpha*v11;
                    c->ptr.pp_double[ic+i+1][jc+j+2] = beta*c->ptr.pp_double[ic+i+1][jc+j+2]+alpha*v12;
                    c->ptr.pp_double[ic+i+1][jc+j+3] = beta*c->ptr.pp_double[ic+i+1][jc+j+3]+alpha*v13;
                    c->ptr.pp_double[ic+i+2][jc+j+0] = beta*c->ptr.pp_double[ic+i+2][jc+j+0]+alpha*v20;
                    c->ptr.pp_double[ic+i+2][jc+j+1] = beta*c->ptr.pp_double[ic+i+2][jc+j+1]+alpha*v21;
                    c->ptr.pp_double[ic+i+2][jc+j+2] = beta*c->ptr.pp_double[ic+i+2][jc+j+2]+alpha*v22;
                    c->ptr.pp_double[ic+i+2][jc+j+3] = beta*c->ptr.pp_double[ic+i+2][jc+j+3]+alpha*v23;
                    c->ptr.pp_double[ic+i+3][jc+j+0] = beta*c->ptr.pp_double[ic+i+3][jc+j+0]+alpha*v30;
                    c->ptr.pp_double[ic+i+3][jc+j+1] = beta*c->ptr.pp_double[ic+i+3][jc+j+1]+alpha*v31;
                    c->ptr.pp_double[ic+i+3][jc+j+2] = beta*c->ptr.pp_double[ic+i+3][jc+j+2]+alpha*v32;
                    c->ptr.pp_double[ic+i+3][jc+j+3] = beta*c->ptr.pp_double[ic+i+3][jc+j+3]+alpha*v33;
                }
            }
            else
            {
                
                /*
                 * Determine submatrix [I0..I1]x[J0..J1] to process
                 */
                i0 = i;
                i1 = ae_minint(i+3, m-1, _state);
                j0 = j;
                j1 = ae_minint(j+3, n-1, _state);
                
                /*
                 * Process submatrix
                 */
                for(ik=i0; ik<=i1; ik++)
                {
                    for(jk=j0; jk<=j1; jk++)
                    {
                        if( k==0||ae_fp_eq(alpha,(double)(0)) )
                        {
                            v = (double)(0);
                        }
                        else
                        {
                            v = ae_v_dotproduct(&a->ptr.pp_double[ia+ik][ja], 1, &b->ptr.pp_double[ib][jb+jk], b->stride, ae_v_len(ja,ja+k-1));
                        }
                        if( ae_fp_eq(beta,(double)(0)) )
                        {
                            c->ptr.pp_double[ic+ik][jc+jk] = alpha*v;
                        }
                        else
                        {
                            c->ptr.pp_double[ic+ik][jc+jk] = beta*c->ptr.pp_double[ic+ik][jc+jk]+alpha*v;
                        }
                    }
                }
            }
            j = j+4;
        }
        i = i+4;
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;
    double v00;
    double v01;
    double v02;
    double v03;
    double v10;
    double v11;
    double v12;
    double v13;
    double v20;
    double v21;
    double v22;
    double v23;
    double v30;
    double v31;
    double v32;
    double v33;
    double a0;
    double a1;
    double a2;
    double a3;
    double b0;
    double b1;
    double b2;
    double b3;
    ae_int_t idxa0;
    ae_int_t idxa1;
    ae_int_t idxa2;
    ae_int_t idxa3;
    ae_int_t idxb0;
    ae_int_t idxb1;
    ae_int_t idxb2;
    ae_int_t idxb3;
    ae_int_t i0;
    ae_int_t i1;
    ae_int_t ik;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t jk;
    ae_int_t t;
    ae_int_t offsa;
    ae_int_t offsb;


    ae_assert(ae_fp_neq(alpha,(double)(0)), "RMatrixGEMMK44V00: internal error (Alpha=0)", _state);
    
    /*
     * if matrix size is zero
     */
    if( m==0||n==0 )
    {
        return;
    }
    
    /*
     * A*B'
     */
    i = 0;
    while(i<m)
    {
        j = 0;
        while(j<n)
        {
            
            /*
             * Choose between specialized 4x4 code and general code
             */
            if( i+4<=m&&j+4<=n )
            {
                
                /*
                 * Specialized 4x4 code for [I..I+3]x[J..J+3] submatrix of C.
                 *
                 * This submatrix is calculated as sum of K rank-1 products,
                 * with operands cached in local variables in order to speed
                 * up operations with arrays.
                 */
                idxa0 = ia+i+0;
                idxa1 = ia+i+1;
                idxa2 = ia+i+2;
                idxa3 = ia+i+3;
                offsa = ja;
                idxb0 = ib+j+0;
                idxb1 = ib+j+1;
                idxb2 = ib+j+2;
                idxb3 = ib+j+3;
                offsb = jb;
                v00 = 0.0;
                v01 = 0.0;
                v02 = 0.0;
                v03 = 0.0;
                v10 = 0.0;
                v11 = 0.0;
                v12 = 0.0;
                v13 = 0.0;
                v20 = 0.0;
                v21 = 0.0;
                v22 = 0.0;
                v23 = 0.0;
                v30 = 0.0;
                v31 = 0.0;
                v32 = 0.0;
                v33 = 0.0;
                for(t=0; t<=k-1; t++)
                {
                    a0 = a->ptr.pp_double[idxa0][offsa];
                    a1 = a->ptr.pp_double[idxa1][offsa];
                    b0 = b->ptr.pp_double[idxb0][offsb];
                    b1 = b->ptr.pp_double[idxb1][offsb];
                    v00 = v00+a0*b0;
                    v01 = v01+a0*b1;
                    v10 = v10+a1*b0;
                    v11 = v11+a1*b1;
                    a2 = a->ptr.pp_double[idxa2][offsa];
                    a3 = a->ptr.pp_double[idxa3][offsa];
                    v20 = v20+a2*b0;
                    v21 = v21+a2*b1;
                    v30 = v30+a3*b0;
                    v31 = v31+a3*b1;
                    b2 = b->ptr.pp_double[idxb2][offsb];
                    b3 = b->ptr.pp_double[idxb3][offsb];
                    v22 = v22+a2*b2;
                    v23 = v23+a2*b3;
                    v32 = v32+a3*b2;
                    v33 = v33+a3*b3;
                    v02 = v02+a0*b2;
                    v03 = v03+a0*b3;
                    v12 = v12+a1*b2;
                    v13 = v13+a1*b3;
                    offsa = offsa+1;
                    offsb = offsb+1;
                }
                if( ae_fp_eq(beta,(double)(0)) )
                {
                    c->ptr.pp_double[ic+i+0][jc+j+0] = alpha*v00;
                    c->ptr.pp_double[ic+i+0][jc+j+1] = alpha*v01;
                    c->ptr.pp_double[ic+i+0][jc+j+2] = alpha*v02;
                    c->ptr.pp_double[ic+i+0][jc+j+3] = alpha*v03;
                    c->ptr.pp_double[ic+i+1][jc+j+0] = alpha*v10;
                    c->ptr.pp_double[ic+i+1][jc+j+1] = alpha*v11;
                    c->ptr.pp_double[ic+i+1][jc+j+2] = alpha*v12;
                    c->ptr.pp_double[ic+i+1][jc+j+3] = alpha*v13;
                    c->ptr.pp_double[ic+i+2][jc+j+0] = alpha*v20;
                    c->ptr.pp_double[ic+i+2][jc+j+1] = alpha*v21;
                    c->ptr.pp_double[ic+i+2][jc+j+2] = alpha*v22;
                    c->ptr.pp_double[ic+i+2][jc+j+3] = alpha*v23;
                    c->ptr.pp_double[ic+i+3][jc+j+0] = alpha*v30;
                    c->ptr.pp_double[ic+i+3][jc+j+1] = alpha*v31;
                    c->ptr.pp_double[ic+i+3][jc+j+2] = alpha*v32;
                    c->ptr.pp_double[ic+i+3][jc+j+3] = alpha*v33;
                }
                else
                {
                    c->ptr.pp_double[ic+i+0][jc+j+0] = beta*c->ptr.pp_double[ic+i+0][jc+j+0]+alpha*v00;
                    c->ptr.pp_double[ic+i+0][jc+j+1] = beta*c->ptr.pp_double[ic+i+0][jc+j+1]+alpha*v01;
                    c->ptr.pp_double[ic+i+0][jc+j+2] = beta*c->ptr.pp_double[ic+i+0][jc+j+2]+alpha*v02;
                    c->ptr.pp_double[ic+i+0][jc+j+3] = beta*c->ptr.pp_double[ic+i+0][jc+j+3]+alpha*v03;
                    c->ptr.pp_double[ic+i+1][jc+j+0] = beta*c->ptr.pp_double[ic+i+1][jc+j+0]+alpha*v10;
                    c->ptr.pp_double[ic+i+1][jc+j+1] = beta*c->ptr.pp_double[ic+i+1][jc+j+1]+alpha*v11;
                    c->ptr.pp_double[ic+i+1][jc+j+2] = beta*c->ptr.pp_double[ic+i+1][jc+j+2]+alpha*v12;
                    c->ptr.pp_double[ic+i+1][jc+j+3] = beta*c->ptr.pp_double[ic+i+1][jc+j+3]+alpha*v13;
                    c->ptr.pp_double[ic+i+2][jc+j+0] = beta*c->ptr.pp_double[ic+i+2][jc+j+0]+alpha*v20;
                    c->ptr.pp_double[ic+i+2][jc+j+1] = beta*c->ptr.pp_double[ic+i+2][jc+j+1]+alpha*v21;
                    c->ptr.pp_double[ic+i+2][jc+j+2] = beta*c->ptr.pp_double[ic+i+2][jc+j+2]+alpha*v22;
                    c->ptr.pp_double[ic+i+2][jc+j+3] = beta*c->ptr.pp_double[ic+i+2][jc+j+3]+alpha*v23;
                    c->ptr.pp_double[ic+i+3][jc+j+0] = beta*c->ptr.pp_double[ic+i+3][jc+j+0]+alpha*v30;
                    c->ptr.pp_double[ic+i+3][jc+j+1] = beta*c->ptr.pp_double[ic+i+3][jc+j+1]+alpha*v31;
                    c->ptr.pp_double[ic+i+3][jc+j+2] = beta*c->ptr.pp_double[ic+i+3][jc+j+2]+alpha*v32;
                    c->ptr.pp_double[ic+i+3][jc+j+3] = beta*c->ptr.pp_double[ic+i+3][jc+j+3]+alpha*v33;
                }
            }
            else
            {
                
                /*
                 * Determine submatrix [I0..I1]x[J0..J1] to process
                 */
                i0 = i;
                i1 = ae_minint(i+3, m-1, _state);
                j0 = j;
                j1 = ae_minint(j+3, n-1, _state);
                
                /*
                 * Process submatrix
                 */
                for(ik=i0; ik<=i1; ik++)
                {
                    for(jk=j0; jk<=j1; jk++)
                    {
                        if( k==0||ae_fp_eq(alpha,(double)(0)) )
                        {
                            v = (double)(0);
                        }
                        else
                        {
                            v = ae_v_dotproduct(&a->ptr.pp_double[ia+ik][ja], 1, &b->ptr.pp_double[ib+jk][jb], 1, ae_v_len(ja,ja+k-1));
                        }
                        if( ae_fp_eq(beta,(double)(0)) )
                        {
                            c->ptr.pp_double[ic+ik][jc+jk] = alpha*v;
                        }
                        else
                        {
                            c->ptr.pp_double[ic+ik][jc+jk] = beta*c->ptr.pp_double[ic+ik][jc+jk]+alpha*v;
                        }
                    }
                }
            }
            j = j+4;
        }
        i = i+4;
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;
    double v00;
    double v01;
    double v02;
    double v03;
    double v10;
    double v11;
    double v12;
    double v13;
    double v20;
    double v21;
    double v22;
    double v23;
    double v30;
    double v31;
    double v32;
    double v33;
    double a0;
    double a1;
    double a2;
    double a3;
    double b0;
    double b1;
    double b2;
    double b3;
    ae_int_t idxa0;
    ae_int_t idxa1;
    ae_int_t idxa2;
    ae_int_t idxa3;
    ae_int_t idxb0;
    ae_int_t idxb1;
    ae_int_t idxb2;
    ae_int_t idxb3;
    ae_int_t i0;
    ae_int_t i1;
    ae_int_t ik;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t jk;
    ae_int_t t;
    ae_int_t offsa;
    ae_int_t offsb;


    ae_assert(ae_fp_neq(alpha,(double)(0)), "RMatrixGEMMK44V00: internal error (Alpha=0)", _state);
    
    /*
     * if matrix size is zero
     */
    if( m==0||n==0 )
    {
        return;
    }
    
    /*
     * A'*B
     */
    i = 0;
    while(i<m)
    {
        j = 0;
        while(j<n)
        {
            
            /*
             * Choose between specialized 4x4 code and general code
             */
            if( i+4<=m&&j+4<=n )
            {
                
                /*
                 * Specialized 4x4 code for [I..I+3]x[J..J+3] submatrix of C.
                 *
                 * This submatrix is calculated as sum of K rank-1 products,
                 * with operands cached in local variables in order to speed
                 * up operations with arrays.
                 */
                idxa0 = ja+i+0;
                idxa1 = ja+i+1;
                idxa2 = ja+i+2;
                idxa3 = ja+i+3;
                offsa = ia;
                idxb0 = jb+j+0;
                idxb1 = jb+j+1;
                idxb2 = jb+j+2;
                idxb3 = jb+j+3;
                offsb = ib;
                v00 = 0.0;
                v01 = 0.0;
                v02 = 0.0;
                v03 = 0.0;
                v10 = 0.0;
                v11 = 0.0;
                v12 = 0.0;
                v13 = 0.0;
                v20 = 0.0;
                v21 = 0.0;
                v22 = 0.0;
                v23 = 0.0;
                v30 = 0.0;
                v31 = 0.0;
                v32 = 0.0;
                v33 = 0.0;
                for(t=0; t<=k-1; t++)
                {
                    a0 = a->ptr.pp_double[offsa][idxa0];
                    a1 = a->ptr.pp_double[offsa][idxa1];
                    b0 = b->ptr.pp_double[offsb][idxb0];
                    b1 = b->ptr.pp_double[offsb][idxb1];
                    v00 = v00+a0*b0;
                    v01 = v01+a0*b1;
                    v10 = v10+a1*b0;
                    v11 = v11+a1*b1;
                    a2 = a->ptr.pp_double[offsa][idxa2];
                    a3 = a->ptr.pp_double[offsa][idxa3];
                    v20 = v20+a2*b0;
                    v21 = v21+a2*b1;
                    v30 = v30+a3*b0;
                    v31 = v31+a3*b1;
                    b2 = b->ptr.pp_double[offsb][idxb2];
                    b3 = b->ptr.pp_double[offsb][idxb3];
                    v22 = v22+a2*b2;
                    v23 = v23+a2*b3;
                    v32 = v32+a3*b2;
                    v33 = v33+a3*b3;
                    v02 = v02+a0*b2;
                    v03 = v03+a0*b3;
                    v12 = v12+a1*b2;
                    v13 = v13+a1*b3;
                    offsa = offsa+1;
                    offsb = offsb+1;
                }
                if( ae_fp_eq(beta,(double)(0)) )
                {
                    c->ptr.pp_double[ic+i+0][jc+j+0] = alpha*v00;
                    c->ptr.pp_double[ic+i+0][jc+j+1] = alpha*v01;
                    c->ptr.pp_double[ic+i+0][jc+j+2] = alpha*v02;
                    c->ptr.pp_double[ic+i+0][jc+j+3] = alpha*v03;
                    c->ptr.pp_double[ic+i+1][jc+j+0] = alpha*v10;
                    c->ptr.pp_double[ic+i+1][jc+j+1] = alpha*v11;
                    c->ptr.pp_double[ic+i+1][jc+j+2] = alpha*v12;
                    c->ptr.pp_double[ic+i+1][jc+j+3] = alpha*v13;
                    c->ptr.pp_double[ic+i+2][jc+j+0] = alpha*v20;
                    c->ptr.pp_double[ic+i+2][jc+j+1] = alpha*v21;
                    c->ptr.pp_double[ic+i+2][jc+j+2] = alpha*v22;
                    c->ptr.pp_double[ic+i+2][jc+j+3] = alpha*v23;
                    c->ptr.pp_double[ic+i+3][jc+j+0] = alpha*v30;
                    c->ptr.pp_double[ic+i+3][jc+j+1] = alpha*v31;
                    c->ptr.pp_double[ic+i+3][jc+j+2] = alpha*v32;
                    c->ptr.pp_double[ic+i+3][jc+j+3] = alpha*v33;
                }
                else
                {
                    c->ptr.pp_double[ic+i+0][jc+j+0] = beta*c->ptr.pp_double[ic+i+0][jc+j+0]+alpha*v00;
                    c->ptr.pp_double[ic+i+0][jc+j+1] = beta*c->ptr.pp_double[ic+i+0][jc+j+1]+alpha*v01;
                    c->ptr.pp_double[ic+i+0][jc+j+2] = beta*c->ptr.pp_double[ic+i+0][jc+j+2]+alpha*v02;
                    c->ptr.pp_double[ic+i+0][jc+j+3] = beta*c->ptr.pp_double[ic+i+0][jc+j+3]+alpha*v03;
                    c->ptr.pp_double[ic+i+1][jc+j+0] = beta*c->ptr.pp_double[ic+i+1][jc+j+0]+alpha*v10;
                    c->ptr.pp_double[ic+i+1][jc+j+1] = beta*c->ptr.pp_double[ic+i+1][jc+j+1]+alpha*v11;
                    c->ptr.pp_double[ic+i+1][jc+j+2] = beta*c->ptr.pp_double[ic+i+1][jc+j+2]+alpha*v12;
                    c->ptr.pp_double[ic+i+1][jc+j+3] = beta*c->ptr.pp_double[ic+i+1][jc+j+3]+alpha*v13;
                    c->ptr.pp_double[ic+i+2][jc+j+0] = beta*c->ptr.pp_double[ic+i+2][jc+j+0]+alpha*v20;
                    c->ptr.pp_double[ic+i+2][jc+j+1] = beta*c->ptr.pp_double[ic+i+2][jc+j+1]+alpha*v21;
                    c->ptr.pp_double[ic+i+2][jc+j+2] = beta*c->ptr.pp_double[ic+i+2][jc+j+2]+alpha*v22;
                    c->ptr.pp_double[ic+i+2][jc+j+3] = beta*c->ptr.pp_double[ic+i+2][jc+j+3]+alpha*v23;
                    c->ptr.pp_double[ic+i+3][jc+j+0] = beta*c->ptr.pp_double[ic+i+3][jc+j+0]+alpha*v30;
                    c->ptr.pp_double[ic+i+3][jc+j+1] = beta*c->ptr.pp_double[ic+i+3][jc+j+1]+alpha*v31;
                    c->ptr.pp_double[ic+i+3][jc+j+2] = beta*c->ptr.pp_double[ic+i+3][jc+j+2]+alpha*v32;
                    c->ptr.pp_double[ic+i+3][jc+j+3] = beta*c->ptr.pp_double[ic+i+3][jc+j+3]+alpha*v33;
                }
            }
            else
            {
                
                /*
                 * Determine submatrix [I0..I1]x[J0..J1] to process
                 */
                i0 = i;
                i1 = ae_minint(i+3, m-1, _state);
                j0 = j;
                j1 = ae_minint(j+3, n-1, _state);
                
                /*
                 * Process submatrix
                 */
                for(ik=i0; ik<=i1; ik++)
                {
                    for(jk=j0; jk<=j1; jk++)
                    {
                        if( k==0||ae_fp_eq(alpha,(double)(0)) )
                        {
                            v = (double)(0);
                        }
                        else
                        {
                            v = 0.0;
                            v = ae_v_dotproduct(&a->ptr.pp_double[ia][ja+ik], a->stride, &b->ptr.pp_double[ib][jb+jk], b->stride, ae_v_len(ia,ia+k-1));
                        }
                        if( ae_fp_eq(beta,(double)(0)) )
                        {
                            c->ptr.pp_double[ic+ik][jc+jk] = alpha*v;
                        }
                        else
                        {
                            c->ptr.pp_double[ic+ik][jc+jk] = beta*c->ptr.pp_double[ic+ik][jc+jk]+alpha*v;
                        }
                    }
                }
            }
            j = j+4;
        }
        i = i+4;
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;
    double v00;
    double v01;
    double v02;
    double v03;
    double v10;
    double v11;
    double v12;
    double v13;
    double v20;
    double v21;
    double v22;
    double v23;
    double v30;
    double v31;
    double v32;
    double v33;
    double a0;
    double a1;
    double a2;
    double a3;
    double b0;
    double b1;
    double b2;
    double b3;
    ae_int_t idxa0;
    ae_int_t idxa1;
    ae_int_t idxa2;
    ae_int_t idxa3;
    ae_int_t idxb0;
    ae_int_t idxb1;
    ae_int_t idxb2;
    ae_int_t idxb3;
    ae_int_t i0;
    ae_int_t i1;
    ae_int_t ik;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t jk;
    ae_int_t t;
    ae_int_t offsa;
    ae_int_t offsb;


    ae_assert(ae_fp_neq(alpha,(double)(0)), "RMatrixGEMMK44V00: internal error (Alpha=0)", _state);
    
    /*
     * if matrix size is zero
     */
    if( m==0||n==0 )
    {
        return;
    }
    
    /*
     * A'*B'
     */
    i = 0;
    while(i<m)
    {
        j = 0;
        while(j<n)
        {
            
            /*
             * Choose between specialized 4x4 code and general code
             */
            if( i+4<=m&&j+4<=n )
            {
                
                /*
                 * Specialized 4x4 code for [I..I+3]x[J..J+3] submatrix of C.
                 *
                 * This submatrix is calculated as sum of K rank-1 products,
                 * with operands cached in local variables in order to speed
                 * up operations with arrays.
                 */
                idxa0 = ja+i+0;
                idxa1 = ja+i+1;
                idxa2 = ja+i+2;
                idxa3 = ja+i+3;
                offsa = ia;
                idxb0 = ib+j+0;
                idxb1 = ib+j+1;
                idxb2 = ib+j+2;
                idxb3 = ib+j+3;
                offsb = jb;
                v00 = 0.0;
                v01 = 0.0;
                v02 = 0.0;
                v03 = 0.0;
                v10 = 0.0;
                v11 = 0.0;
                v12 = 0.0;
                v13 = 0.0;
                v20 = 0.0;
                v21 = 0.0;
                v22 = 0.0;
                v23 = 0.0;
                v30 = 0.0;
                v31 = 0.0;
                v32 = 0.0;
                v33 = 0.0;
                for(t=0; t<=k-1; t++)
                {
                    a0 = a->ptr.pp_double[offsa][idxa0];
                    a1 = a->ptr.pp_double[offsa][idxa1];
                    b0 = b->ptr.pp_double[idxb0][offsb];
                    b1 = b->ptr.pp_double[idxb1][offsb];
                    v00 = v00+a0*b0;
                    v01 = v01+a0*b1;
                    v10 = v10+a1*b0;
                    v11 = v11+a1*b1;
                    a2 = a->ptr.pp_double[offsa][idxa2];
                    a3 = a->ptr.pp_double[offsa][idxa3];
                    v20 = v20+a2*b0;
                    v21 = v21+a2*b1;
                    v30 = v30+a3*b0;
                    v31 = v31+a3*b1;
                    b2 = b->ptr.pp_double[idxb2][offsb];
                    b3 = b->ptr.pp_double[idxb3][offsb];
                    v22 = v22+a2*b2;
                    v23 = v23+a2*b3;
                    v32 = v32+a3*b2;
                    v33 = v33+a3*b3;
                    v02 = v02+a0*b2;
                    v03 = v03+a0*b3;
                    v12 = v12+a1*b2;
                    v13 = v13+a1*b3;
                    offsa = offsa+1;
                    offsb = offsb+1;
                }
                if( ae_fp_eq(beta,(double)(0)) )
                {
                    c->ptr.pp_double[ic+i+0][jc+j+0] = alpha*v00;
                    c->ptr.pp_double[ic+i+0][jc+j+1] = alpha*v01;
                    c->ptr.pp_double[ic+i+0][jc+j+2] = alpha*v02;
                    c->ptr.pp_double[ic+i+0][jc+j+3] = alpha*v03;
                    c->ptr.pp_double[ic+i+1][jc+j+0] = alpha*v10;
                    c->ptr.pp_double[ic+i+1][jc+j+1] = alpha*v11;
                    c->ptr.pp_double[ic+i+1][jc+j+2] = alpha*v12;
                    c->ptr.pp_double[ic+i+1][jc+j+3] = alpha*v13;
                    c->ptr.pp_double[ic+i+2][jc+j+0] = alpha*v20;
                    c->ptr.pp_double[ic+i+2][jc+j+1] = alpha*v21;
                    c->ptr.pp_double[ic+i+2][jc+j+2] = alpha*v22;
                    c->ptr.pp_double[ic+i+2][jc+j+3] = alpha*v23;
                    c->ptr.pp_double[ic+i+3][jc+j+0] = alpha*v30;
                    c->ptr.pp_double[ic+i+3][jc+j+1] = alpha*v31;
                    c->ptr.pp_double[ic+i+3][jc+j+2] = alpha*v32;
                    c->ptr.pp_double[ic+i+3][jc+j+3] = alpha*v33;
                }
                else
                {
                    c->ptr.pp_double[ic+i+0][jc+j+0] = beta*c->ptr.pp_double[ic+i+0][jc+j+0]+alpha*v00;
                    c->ptr.pp_double[ic+i+0][jc+j+1] = beta*c->ptr.pp_double[ic+i+0][jc+j+1]+alpha*v01;
                    c->ptr.pp_double[ic+i+0][jc+j+2] = beta*c->ptr.pp_double[ic+i+0][jc+j+2]+alpha*v02;
                    c->ptr.pp_double[ic+i+0][jc+j+3] = beta*c->ptr.pp_double[ic+i+0][jc+j+3]+alpha*v03;
                    c->ptr.pp_double[ic+i+1][jc+j+0] = beta*c->ptr.pp_double[ic+i+1][jc+j+0]+alpha*v10;
                    c->ptr.pp_double[ic+i+1][jc+j+1] = beta*c->ptr.pp_double[ic+i+1][jc+j+1]+alpha*v11;
                    c->ptr.pp_double[ic+i+1][jc+j+2] = beta*c->ptr.pp_double[ic+i+1][jc+j+2]+alpha*v12;
                    c->ptr.pp_double[ic+i+1][jc+j+3] = beta*c->ptr.pp_double[ic+i+1][jc+j+3]+alpha*v13;
                    c->ptr.pp_double[ic+i+2][jc+j+0] = beta*c->ptr.pp_double[ic+i+2][jc+j+0]+alpha*v20;
                    c->ptr.pp_double[ic+i+2][jc+j+1] = beta*c->ptr.pp_double[ic+i+2][jc+j+1]+alpha*v21;
                    c->ptr.pp_double[ic+i+2][jc+j+2] = beta*c->ptr.pp_double[ic+i+2][jc+j+2]+alpha*v22;
                    c->ptr.pp_double[ic+i+2][jc+j+3] = beta*c->ptr.pp_double[ic+i+2][jc+j+3]+alpha*v23;
                    c->ptr.pp_double[ic+i+3][jc+j+0] = beta*c->ptr.pp_double[ic+i+3][jc+j+0]+alpha*v30;
                    c->ptr.pp_double[ic+i+3][jc+j+1] = beta*c->ptr.pp_double[ic+i+3][jc+j+1]+alpha*v31;
                    c->ptr.pp_double[ic+i+3][jc+j+2] = beta*c->ptr.pp_double[ic+i+3][jc+j+2]+alpha*v32;
                    c->ptr.pp_double[ic+i+3][jc+j+3] = beta*c->ptr.pp_double[ic+i+3][jc+j+3]+alpha*v33;
                }
            }
            else
            {
                
                /*
                 * Determine submatrix [I0..I1]x[J0..J1] to process
                 */
                i0 = i;
                i1 = ae_minint(i+3, m-1, _state);
                j0 = j;
                j1 = ae_minint(j+3, n-1, _state);
                
                /*
                 * Process submatrix
                 */
                for(ik=i0; ik<=i1; ik++)
                {
                    for(jk=j0; jk<=j1; jk++)
                    {
                        if( k==0||ae_fp_eq(alpha,(double)(0)) )
                        {
                            v = (double)(0);
                        }
                        else
                        {
                            v = 0.0;
                            v = ae_v_dotproduct(&a->ptr.pp_double[ia][ja+ik], a->stride, &b->ptr.pp_double[ib+jk][jb], 1, ae_v_len(ia,ia+k-1));
                        }
                        if( ae_fp_eq(beta,(double)(0)) )
                        {
                            c->ptr.pp_double[ic+ik][jc+jk] = alpha*v;
                        }
                        else
                        {
                            c->ptr.pp_double[ic+ik][jc+jk] = beta*c->ptr.pp_double[ic+ik][jc+jk]+alpha*v;
                        }
                    }
                }
            }
            j = j+4;
        }
        i = i+4;
    }
}


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Fast kernel (new version with AVX2/SSE2)

  -- ALGLIB routine --
     19.01.2010
     Bochkanov Sergey
*************************************************************************/
static ae_bool ablasf_rgemm32basecase(ae_int_t m,
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
    ae_bool result;


    result = ae_false;
    return result;
}
#endif


/*$ End $*/
