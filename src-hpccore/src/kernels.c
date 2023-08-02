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
#define _ALGLIB_IMPL_DEFINES
#include "kernels.h"

#if defined(_ALGLIB_HAS_SSE2_INTRINSICS)
#include "kernels_sse2.h"
#endif
#if defined(_ALGLIB_HAS_AVX2_INTRINSICS)
#include "kernels_avx2.h"
#endif
#if defined(_ALGLIB_HAS_FMA_INTRINSICS)
#include "kernels_fma.h"
#endif

/*$ Declarations $*/

/*$ Body $*/
#if !defined(ALGLIB_NO_FAST_KERNELS)

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
    
    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_RETURN_SSE2_AVX2_FMA(rdotv,(n,x->ptr.p_double,y->ptr.p_double,_state)) /* use _ALGLIB_KERNEL_VOID_ for a kernel that does not return result */

    /*
     * Original generic C implementation
     */
    result = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        result = result+x->ptr.p_double[i]*y->ptr.p_double[i];
    }
    return result;
}



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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_RETURN_SSE2_AVX2_FMA(rdotv,(n,x->ptr.p_double,a->ptr.pp_double[i],_state))

    result = (double)(0);
    for(j=0; j<=n-1; j++)
    {
        result = result+x->ptr.p_double[j]*a->ptr.pp_double[i][j];
    }
    return result;
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_RETURN_SSE2_AVX2_FMA(rdotv,(n,a->ptr.pp_double[ia],b->ptr.pp_double[ib],_state))

    result = (double)(0);
    for(j=0; j<=n-1; j++)
    {
        result = result+a->ptr.pp_double[ia][j]*b->ptr.pp_double[ib][j];
    }
    return result;
}


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
double rdotv2(ae_int_t n, /* Real    */ const ae_vector* x, ae_state *_state)
{
    ae_int_t i;
    double v;
    double result;

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_RETURN_SSE2_AVX2_FMA(rdotv2,(n,x->ptr.p_double,_state))

    result = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        v = x->ptr.p_double[i];
        result = result+v*v;
    }
    return result;
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rcopyv,
            (n,x->ptr.p_double,y->ptr.p_double,_state))


    for(j=0; j<=n-1; j++)
    {
        y->ptr.p_double[j] = x->ptr.p_double[j];
    }
}

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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rcopyv,
            (n, x->ptr.p_double, a->ptr.pp_double[i], _state))

    for(j=0; j<=n-1; j++)
    {
        a->ptr.pp_double[i][j] = x->ptr.p_double[j];
    }
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rcopyv,
            (n, a->ptr.pp_double[i], x->ptr.p_double, _state))

    for(j=0; j<=n-1; j++)
    {
        x->ptr.p_double[j] = a->ptr.pp_double[i][j];
    }
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rcopyv,
            (n, a->ptr.pp_double[i], b->ptr.pp_double[k], _state))

    for(j=0; j<=n-1; j++)
    {
        b->ptr.pp_double[k][j] = a->ptr.pp_double[i][j];
    }
}

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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rcopymulv,
            (n,v,x->ptr.p_double,y->ptr.p_double,_state))

    for(i=0; i<=n-1; i++)
    {
        y->ptr.p_double[i] = v*x->ptr.p_double[i];
    }
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rcopymulv,
            (n,v,x->ptr.p_double,y->ptr.pp_double[ridx],_state))

    for(i=0; i<=n-1; i++)
    {
        y->ptr.pp_double[ridx][i] = v*x->ptr.p_double[i];
    }
}

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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(icopyv,
            (n, x->ptr.p_int, y->ptr.p_int, _state))

    for(j=0; j<=n-1; j++)
    {
        y->ptr.p_int[j] = x->ptr.p_int[j];
    }
}

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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1*8 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(bcopyv,
            (n, x->ptr.p_bool, y->ptr.p_bool, _state))

    for(j=0; j<=n-1; j++)
    {
        y->ptr.p_bool[j] = x->ptr.p_bool[j];
    }
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rsetv,
            (n, v, x->ptr.p_double, _state))

    for(j=0; j<=n-1; j++)
    {
        x->ptr.p_double[j] = v;
    }
}

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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rsetv,
            (n, v, a->ptr.pp_double[i], _state))

    for(j=0; j<=n-1; j++)
    {
        a->ptr.pp_double[i][j] = v;
    }
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rsetvx,
            (n, v, x->ptr.p_double+offsx, _state))

    for(j=0; j<=n-1; j++)
    {
        x->ptr.p_double[offsx+j] = v;
    }
}


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
static void rsetm_simd(const ae_int_t n, const double v, double *pDest, ae_state *_state)
{
    _ALGLIB_KERNEL_VOID_SSE2_AVX2(rsetv, (n, v, pDest, _state));

    ae_int_t j;
    for(j=0; j<=n-1; j++) {
        pDest[j] = v;
    }
}

void rsetm(ae_int_t m,
     ae_int_t n,
     double v,
     /* Real    */ ae_matrix* a,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n >=_ABLASF_KERNEL_SIZE1 ) {
        for(i=0; i<m; i++) {
            rsetm_simd(n, v, a->ptr.pp_double[i], _state);
        }
        return;
    }

    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            a->ptr.pp_double[i][j] = v;
        }
    }
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(isetv,
            (n, v, x->ptr.p_int, _state))

    for(j=0; j<=n-1; j++)
    {
        x->ptr.p_int[j] = v;
    }
}

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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1*8 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(bsetv,
            (n, v, x->ptr.p_bool, _state))

    for(j=0; j<=n-1; j++)
    {
        x->ptr.p_bool[j] = v;
    }
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rmulv,
            (n,v,x->ptr.p_double,_state))

    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = x->ptr.p_double[i]*v;
    }
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rmulv,
            (n, v, x->ptr.pp_double[rowidx], _state))

    for(i=0; i<=n-1; i++)
    {
        x->ptr.pp_double[rowidx][i] = x->ptr.pp_double[rowidx][i]*v;
    }
}


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
void rsqrtv(ae_int_t n,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_int_t i;

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_AVX2(rsqrtv,
            (n,x->ptr.p_double,_state))

    for(i=0; i<=n-1; i++)
        x->ptr.p_double[i] = sqrt(x->ptr.p_double[i]);
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_AVX2(rsqrtv,
            (n, x->ptr.pp_double[rowidx], _state))

    for(i=0; i<=n-1; i++)
        x->ptr.pp_double[rowidx][i] = sqrt(x->ptr.pp_double[rowidx][i]);
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rmulvx,
            (n, v, x->ptr.p_double+offsx, _state))

    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[offsx+i] = x->ptr.p_double[offsx+i]*v;
    }
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2_FMA(raddv,
            (n,alpha,y->ptr.p_double,x->ptr.p_double,_state))


    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = x->ptr.p_double[i]+alpha*y->ptr.p_double[i];
    }
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2_FMA(raddv,
            (n,alpha,y->ptr.p_double,x->ptr.pp_double[rowidx],_state))


    for(i=0; i<=n-1; i++)
    {
        x->ptr.pp_double[rowidx][i] = x->ptr.pp_double[rowidx][i]+alpha*y->ptr.p_double[i];
    }
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2_FMA(raddv,
            (n,alpha,y->ptr.pp_double[ridx],x->ptr.p_double,_state))

    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = x->ptr.p_double[i]+alpha*y->ptr.pp_double[ridx][i];
    }
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2_FMA(raddv,
            (n,alpha,y->ptr.pp_double[ridxsrc],x->ptr.pp_double[ridxdst],_state))

    for(i=0; i<=n-1; i++)
    {
        x->ptr.pp_double[ridxdst][i] = x->ptr.pp_double[ridxdst][i]+alpha*y->ptr.pp_double[ridxsrc][i];
    }
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2_FMA(raddvx,
            (n, alpha, y->ptr.p_double+offsy, x->ptr.p_double+offsx, _state))

    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[offsx+i] = x->ptr.p_double[offsx+i]+alpha*y->ptr.p_double[offsy+i];
    }
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_FMA(rmuladdv, (n, y->ptr.p_double, z->ptr.p_double, x->ptr.p_double, _state))

    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = x->ptr.p_double[i]+y->ptr.p_double[i]*z->ptr.p_double[i];
    }
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_FMA(rnegmuladdv, (n, y->ptr.p_double, z->ptr.p_double, x->ptr.p_double, _state))

    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] -= y->ptr.p_double[i]*z->ptr.p_double[i];
    }
}


/*************************************************************************
Performs addition of Y[]*Z[] to X[]

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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_FMA(rcopymuladdv, (n, y->ptr.p_double, z->ptr.p_double, x->ptr.p_double, r->ptr.p_double, _state))

    for(i=0; i<=n-1; i++)
        r->ptr.p_double[i] = x->ptr.p_double[i]+y->ptr.p_double[i]*z->ptr.p_double[i];
}


/*************************************************************************
Performs subtraction of Y[]*Z[] from X[]

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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_FMA(rcopynegmuladdv, (n, y->ptr.p_double, z->ptr.p_double, x->ptr.p_double, r->ptr.p_double, _state))

    for(i=0; i<=n-1; i++)
        r->ptr.p_double[i] = x->ptr.p_double[i]-y->ptr.p_double[i]*z->ptr.p_double[i];
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rmergemulv,
            (n,y->ptr.p_double,x->ptr.p_double,_state))


    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = x->ptr.p_double[i]*y->ptr.p_double[i];
    }
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rmergemulv,
            (n,y->ptr.p_double,x->ptr.pp_double[rowidx],_state))


    for(i=0; i<=n-1; i++)
    {
        x->ptr.pp_double[rowidx][i] = x->ptr.pp_double[rowidx][i]*y->ptr.p_double[i];
    }
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rmergemulv,
            (n,y->ptr.pp_double[rowidx],x->ptr.p_double,_state))

    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = x->ptr.p_double[i]*y->ptr.pp_double[rowidx][i];
    }
}




/*************************************************************************
Performs componentwise division of vector X[] by vector Y[]

  -- ALGLIB --
     Copyright 20.01.2020 by Bochkanov Sergey
*************************************************************************/
void rmergedivv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_int_t i;

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_AVX2(rmergedivv,
            (n,y->ptr.p_double,x->ptr.p_double,_state))


    for(i=0; i<=n-1; i++)
        x->ptr.p_double[i] /= y->ptr.p_double[i];
}


/*************************************************************************
Performs componentwise division of row X[] by vector Y[]

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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_AVX2(rmergedivv,
            (n,y->ptr.p_double,x->ptr.pp_double[rowidx],_state))


    for(i=0; i<=n-1; i++)
        x->ptr.pp_double[rowidx][i] /= y->ptr.p_double[i];
}


/*************************************************************************
Performs componentwise division of row X[] by vector Y[]

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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_AVX2(rmergedivv,
            (n,y->ptr.pp_double[rowidx],x->ptr.p_double,_state))

    for(i=0; i<=n-1; i++)
        x->ptr.p_double[i] /= y->ptr.pp_double[rowidx][i];
}

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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rmergemaxv,
            (n,y->ptr.p_double,x->ptr.p_double,_state))

    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = ae_maxreal(x->ptr.p_double[i], y->ptr.p_double[i], _state);
    }
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rmergemaxv,
            (n,y->ptr.p_double,x->ptr.pp_double[rowidx],_state))

    for(i=0; i<=n-1; i++)
    {
        x->ptr.pp_double[rowidx][i] = ae_maxreal(x->ptr.pp_double[rowidx][i], y->ptr.p_double[i], _state);
    }
}


/*************************************************************************
Performs componentwise max of row X[I] and vector Y[] 

INPUT PARAMETERS:
    N       -   vector length
    X       -   matrix, I-th row is source
    rowidx  -   target row RowIdx

RESULT:
    Y := componentwise_max(X,Y)

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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rmergemaxv,
            (n,x->ptr.pp_double[rowidx],y->ptr.p_double,_state))

    for(i=0; i<=n-1; i++)
    {
        y->ptr.p_double[i] = ae_maxreal(y->ptr.p_double[i], x->ptr.pp_double[rowidx][i], _state);
    }
}

/*************************************************************************
Performs componentwise min of vector X[] and vector Y[]

INPUT PARAMETERS:
    N       -   vector length
    Y       -   source vector
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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rmergeminv,
            (n,y->ptr.p_double,x->ptr.p_double,_state))

    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = ae_minreal(x->ptr.p_double[i], y->ptr.p_double[i], _state);
    }
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rmergeminv,
            (n,y->ptr.p_double,x->ptr.pp_double[rowidx],_state))

    for(i=0; i<=n-1; i++)
    {
        x->ptr.pp_double[rowidx][i] = ae_minreal(x->ptr.pp_double[rowidx][i], y->ptr.p_double[i], _state);
    }
}


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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rmergeminv,
            (n,x->ptr.pp_double[rowidx],y->ptr.p_double,_state))

    for(i=0; i<=n-1; i++)
    {
        y->ptr.p_double[i] = ae_minreal(y->ptr.p_double[i], x->ptr.pp_double[rowidx][i], _state);
    }
}
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
double rmaxv(ae_int_t n, /* Real    */ const ae_vector* x, ae_state *_state)
{
    ae_int_t i;
    double v;
    double result;

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_RETURN_SSE2_AVX2(rmaxv, (n, x->ptr.p_double, _state));
    
    if(n == 0)
        return 0.0;
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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_RETURN_SSE2_AVX2(rmaxv,(n, x->ptr.pp_double[rowidx], _state))
    
    if(n == 0)
        return 0.0;
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
double rmaxabsv(ae_int_t n, /* Real    */ const ae_vector* x, ae_state *_state)
{
    ae_int_t i;
    double v;
    double result;

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_RETURN_SSE2_AVX2(rmaxabsv, (n, x->ptr.p_double, _state))

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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_RETURN_SSE2_AVX2(rmaxabsv,(n, x->ptr.pp_double[rowidx], _state))

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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(rcopyvx,(n, x->ptr.p_double+offsx, y->ptr.p_double+offsy, _state))

    for(j=0; j<=n-1; j++)
    {
        y->ptr.p_double[offsy+j] = x->ptr.p_double[offsx+j];
    }
}

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

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    if( n>=_ABLASF_KERNEL_SIZE1 )
        _ALGLIB_KERNEL_VOID_SSE2_AVX2(icopyvx,(n, x->ptr.p_int+offsx, y->ptr.p_int+offsy, _state))

    for(j=0; j<=n-1; j++)
    {
        y->ptr.p_int[offsy+j] = x->ptr.p_int[offsx+j];
    }
}

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
     * Straight or transposed?
     */
    if( opa==0 )
    {
        /*
         * Try SIMD code
         */
        if( n>=_ABLASF_KERNEL_SIZE2 )
            _ALGLIB_KERNEL_VOID_AVX2_FMA(rgemv_straight, (m, n, alpha, a,
                x->ptr.p_double, y->ptr.p_double, _state))
        
        /*
         * Generic C version: y += A*x
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
         * Try SIMD code
         */
        if( m>=_ABLASF_KERNEL_SIZE2 )
            _ALGLIB_KERNEL_VOID_AVX2_FMA(rgemv_transposed, (m, n, alpha, a,
                x->ptr.p_double, y->ptr.p_double, _state))


        /*
         * Generic C version: y += A^T*x
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
     * Straight or transposed?
     */
    if( opa==0 )
    {
        /*
         * Try SIMD code
         */
        if( n>=_ABLASF_KERNEL_SIZE2 )
            _ALGLIB_KERNEL_VOID_AVX2_FMA(rgemvx_straight, (m, n, alpha, a, ia, ja,
                x->ptr.p_double + ix, y->ptr.p_double + iy, _state))

        
        /*
         * Generic C code: y += A*x
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
         * Try SIMD code
         */
        if( m>=_ABLASF_KERNEL_SIZE2 )
            _ALGLIB_KERNEL_VOID_AVX2_FMA(rgemvx_transposed, (m, n, alpha, a, ia, ja,
                x->ptr.p_double+ix, y->ptr.p_double+iy, _state))

        /*
         * Generic C code: y += A^T*x
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
            if( v==0.0 )
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
            if( v==0.0 )
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

/*************************************************************************
Fast rGEMM kernel with AVX2/FMA support

  -- ALGLIB routine --
     19.09.2021
     Bochkanov Sergey
*************************************************************************/
ae_bool ablasf_rgemm32basecase(
     ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* _a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     /* Real    */ const ae_matrix* _b,
     ae_int_t ib,
     ae_int_t jb,
     ae_int_t optypeb,
     double beta,
     /* Real    */ ae_matrix* _c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state)
{
#if !defined(_ALGLIB_HAS_AVX2_INTRINSICS)
    return ae_false;
#else
    const ae_int_t block_size = _ABLASF_BLOCK_SIZE;
    const ae_int_t micro_size = _ABLASF_MICRO_SIZE;
    ae_int_t out0, out1;
    double *c;
    ae_int_t stride_c;
    ae_int_t cpu_id = ae_cpuid();
    ae_int_t (*ablasf_packblk)(const double*, ae_int_t, ae_int_t, ae_int_t, ae_int_t, double*, ae_int_t, ae_int_t) = (k==32 && block_size==32) ? ablasf_packblkh32_avx2 : ablasf_packblkh_avx2;
    void     (*ablasf_dotblk)(const double *, const double *, ae_int_t, ae_int_t, ae_int_t, double *, ae_int_t)    = ablasf_dotblkh_avx2;
    void     (*ablasf_daxpby)(ae_int_t, double, const double *, double, double*) = ablasf_daxpby_avx2;

    /*
     * Determine CPU and kernel support
     */
    if( m>block_size || n>block_size || k>block_size || m==0 || n==0 || !(cpu_id&CPU_AVX2) )
        return ae_false;
#if defined(_ALGLIB_HAS_FMA_INTRINSICS)
    if( cpu_id&CPU_FMA )
        ablasf_dotblk  = ablasf_dotblkh_fma;
#endif
    
    /*
     * Prepare C
     */
    c = _c->ptr.pp_double[ic]+jc;
    stride_c = _c->stride;
    
    /*
     * Do we have alpha*A*B ?
     */
    if( alpha!=0 && k>0 )
    {
        /*
         * Prepare structures
         */
        ae_int_t base0, base1, offs0;
        double *a = _a->ptr.pp_double[ia]+ja;
        double *b = _b->ptr.pp_double[ib]+jb;
        ae_int_t stride_a = _a->stride;
        ae_int_t stride_b = _b->stride;
        double      _blka[_ABLASF_BLOCK_SIZE*_ABLASF_MICRO_SIZE+_ALGLIB_SIMD_ALIGNMENT_DOUBLES];
        double _blkb_long[_ABLASF_BLOCK_SIZE*_ABLASF_BLOCK_SIZE+_ALGLIB_SIMD_ALIGNMENT_DOUBLES];
        double      _blkc[_ABLASF_MICRO_SIZE*_ABLASF_BLOCK_SIZE+_ALGLIB_SIMD_ALIGNMENT_DOUBLES];
        double *blka          = (double*)ae_align(_blka,     _ALGLIB_SIMD_ALIGNMENT_BYTES);
        double *storageb_long = (double*)ae_align(_blkb_long,_ALGLIB_SIMD_ALIGNMENT_BYTES);
        double *blkc          = (double*)ae_align(_blkc,     _ALGLIB_SIMD_ALIGNMENT_BYTES);
        
        /*
         * Pack transform(B) into precomputed block form
         */
        for(base1=0; base1<n; base1+=micro_size)
        {
            const ae_int_t lim1 = n-base1<micro_size ? n-base1 : micro_size;
            double *curb = storageb_long+base1*block_size;
            ablasf_packblk(
                b + (optypeb==0 ? base1 : base1*stride_b), stride_b, optypeb==0 ? 1 : 0, k, lim1,
                curb, block_size, micro_size);
        }
        
        /*
         * Output
         */
        for(base0=0; base0<m; base0+=micro_size)
        {
            /*
             * Load block row of transform(A)
             */
            const ae_int_t lim0    = m-base0<micro_size ? m-base0 : micro_size;
            const ae_int_t round_k = ablasf_packblk(
                a + (optypea==0 ? base0*stride_a : base0), stride_a, optypea, k, lim0,
                blka, block_size, micro_size);
                
            /*
             * Compute block(A)'*entire(B)
             */
            for(base1=0; base1<n; base1+=micro_size)
                ablasf_dotblk(blka, storageb_long+base1*block_size, round_k, block_size, micro_size, blkc+base1, block_size);

            /*
             * Output block row of block(A)'*entire(B)
             */
            for(offs0=0; offs0<lim0; offs0++)
                ablasf_daxpby(n, alpha, blkc+offs0*block_size, beta, c+(base0+offs0)*stride_c);
        }
    }
    else
    {
        /*
         * No A*B, just beta*C (degenerate case, not optimized)
         */
        if( beta==0 )
        {
            for(out0=0; out0<m; out0++)
                for(out1=0; out1<n; out1++)
                    c[out0*stride_c+out1] = 0.0;
        }
        else if( beta!=1 )
        {
            for(out0=0; out0<m; out0++)
                for(out1=0; out1<n; out1++)
                    c[out0*stride_c+out1] *= beta;
        }
    }
    return ae_true;
#endif
}


/*************************************************************************
Returns recommended width of the SIMD-friendly buffer
*************************************************************************/
ae_int_t spchol_spsymmgetmaxsimd(ae_state *_state)
{
#if AE_CPU==AE_INTEL
    return 4;
#else
    return 1;
#endif
}

/*************************************************************************
Solving linear system: propagating computed supernode.

Propagates computed supernode to the rest of the RHS  using  SIMD-friendly
RHS storage format.

INPUT PARAMETERS:

OUTPUT PARAMETERS:

  -- ALGLIB routine --
     08.09.2021
     Bochkanov Sergey
*************************************************************************/
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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t baseoffs;
    double v;
    
    /*
     * Try SIMD kernels
     */
#if defined(_ALGLIB_HAS_FMA_INTRINSICS)
    if( sstride==4 || (blocksize==2 && sstride==2) )
        if( ae_cpuid()&CPU_FMA )
        {
            spchol_propagatefwd_fma(x, cols0, blocksize, superrowidx, rbase, offdiagsize, rowstorage, offss, sstride, simdbuf, simdwidth, _state);
            return;
        }
#endif

    /*
     * Propagate rank-1 node (can not be accelerated with SIMD)
     */
    if( blocksize==1 && sstride==1 )
    {
        /*
         * blocksize is 1, stride is 1
         */
        double vx = x->ptr.p_double[cols0];
        double *p_mat_row  = rowstorage->ptr.p_double+offss+1*1;
        double *p_simd_buf = simdbuf->ptr.p_double;
        ae_int_t *p_rowidx = superrowidx->ptr.p_int+rbase;
        if( simdwidth==4 )
        {
            for(k=0; k<offdiagsize; k++)
                p_simd_buf[p_rowidx[k]*4] -= p_mat_row[k]*vx;
        }
        else
        {
            for(k=0; k<offdiagsize; k++)
                p_simd_buf[p_rowidx[k]*simdwidth] -= p_mat_row[k]*vx;
        }
        return;
    }

    /*
     * Generic C code for generic propagate
     */
    for(k=0; k<=offdiagsize-1; k++)
    {
        i = superrowidx->ptr.p_int[rbase+k];
        baseoffs = offss+(k+blocksize)*sstride;
        v = simdbuf->ptr.p_double[i*simdwidth];
        for(j=0; j<=blocksize-1; j++)
        {
            v = v-rowstorage->ptr.p_double[baseoffs+j]*x->ptr.p_double[cols0+j];
        }
        simdbuf->ptr.p_double[i*simdwidth] = v;
    }
}


/*************************************************************************
Fast kernels for small supernodal updates: special 4x4x4x4 function.

! See comments on UpdateSupernode() for information  on generic supernodal
! updates, including notation used below.

The generic update has following form:

    S := S - scatter(U*D*Uc')

This specialized function performs AxBxCx4 update, i.e.:
* S is a tHeight*A matrix with row stride equal to 4 (usually it means that
  it has 3 or 4 columns)
* U is a uHeight*B matrix
* Uc' is a B*C matrix, with C<=A
* scatter() scatters rows and columns of U*Uc'
  
Return value:
* True if update was applied
* False if kernel refused to perform an update (quick exit for unsupported
  combinations of input sizes)

  -- ALGLIB routine --
     20.09.2020
     Bochkanov Sergey
*************************************************************************/
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
     ae_state *_state)
{
    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    _ALGLIB_KERNEL_RETURN_AVX2_FMA(spchol_updatekernelabc4,(rowstorage->ptr.p_double, offss, twidth, offsu, uheight, urank, urowstride, uwidth, diagd->ptr.p_double, offsd, raw2smap->ptr.p_int, superrowidx->ptr.p_int, urbase, _state))

    /*
     * Generic code
     */
    ae_int_t k;
    ae_int_t targetrow;
    ae_int_t targetcol;
    ae_int_t offsk;
    double d0;
    double d1;
    double d2;
    double d3;
    double u00;
    double u01;
    double u02;
    double u03;
    double u10;
    double u11;
    double u12;
    double u13;
    double u20;
    double u21;
    double u22;
    double u23;
    double u30;
    double u31;
    double u32;
    double u33;
    double uk0;
    double uk1;
    double uk2;
    double uk3;
    ae_int_t srccol0;
    ae_int_t srccol1;
    ae_int_t srccol2;
    ae_int_t srccol3;
    ae_bool result;


    
    /*
     * Filter out unsupported combinations (ones that are too sparse for the non-SIMD code)
     */
    result = ae_false;
    if( twidth<3||twidth>4 )
    {
        return result;
    }
    if( uwidth<1||uwidth>4 )
    {
        return result;
    }
    if( urank>4 )
    {
        return result;
    }
    
    /*
     * Determine source columns for target columns, -1 if target column
     * is not updated.
     */
    srccol0 = -1;
    srccol1 = -1;
    srccol2 = -1;
    srccol3 = -1;
    for(k=0; k<=uwidth-1; k++)
    {
        targetcol = raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+k]];
        if( targetcol==0 )
        {
            srccol0 = k;
        }
        if( targetcol==1 )
        {
            srccol1 = k;
        }
        if( targetcol==2 )
        {
            srccol2 = k;
        }
        if( targetcol==3 )
        {
            srccol3 = k;
        }
    }
    
    /*
     * Load update matrix into aligned/rearranged 4x4 storage
     */
    d0 = (double)(0);
    d1 = (double)(0);
    d2 = (double)(0);
    d3 = (double)(0);
    u00 = (double)(0);
    u01 = (double)(0);
    u02 = (double)(0);
    u03 = (double)(0);
    u10 = (double)(0);
    u11 = (double)(0);
    u12 = (double)(0);
    u13 = (double)(0);
    u20 = (double)(0);
    u21 = (double)(0);
    u22 = (double)(0);
    u23 = (double)(0);
    u30 = (double)(0);
    u31 = (double)(0);
    u32 = (double)(0);
    u33 = (double)(0);
    if( urank>=1 )
    {
        d0 = diagd->ptr.p_double[offsd+0];
    }
    if( urank>=2 )
    {
        d1 = diagd->ptr.p_double[offsd+1];
    }
    if( urank>=3 )
    {
        d2 = diagd->ptr.p_double[offsd+2];
    }
    if( urank>=4 )
    {
        d3 = diagd->ptr.p_double[offsd+3];
    }
    if( srccol0>=0 )
    {
        if( urank>=1 )
        {
            u00 = d0*rowstorage->ptr.p_double[offsu+srccol0*urowstride+0];
        }
        if( urank>=2 )
        {
            u01 = d1*rowstorage->ptr.p_double[offsu+srccol0*urowstride+1];
        }
        if( urank>=3 )
        {
            u02 = d2*rowstorage->ptr.p_double[offsu+srccol0*urowstride+2];
        }
        if( urank>=4 )
        {
            u03 = d3*rowstorage->ptr.p_double[offsu+srccol0*urowstride+3];
        }
    }
    if( srccol1>=0 )
    {
        if( urank>=1 )
        {
            u10 = d0*rowstorage->ptr.p_double[offsu+srccol1*urowstride+0];
        }
        if( urank>=2 )
        {
            u11 = d1*rowstorage->ptr.p_double[offsu+srccol1*urowstride+1];
        }
        if( urank>=3 )
        {
            u12 = d2*rowstorage->ptr.p_double[offsu+srccol1*urowstride+2];
        }
        if( urank>=4 )
        {
            u13 = d3*rowstorage->ptr.p_double[offsu+srccol1*urowstride+3];
        }
    }
    if( srccol2>=0 )
    {
        if( urank>=1 )
        {
            u20 = d0*rowstorage->ptr.p_double[offsu+srccol2*urowstride+0];
        }
        if( urank>=2 )
        {
            u21 = d1*rowstorage->ptr.p_double[offsu+srccol2*urowstride+1];
        }
        if( urank>=3 )
        {
            u22 = d2*rowstorage->ptr.p_double[offsu+srccol2*urowstride+2];
        }
        if( urank>=4 )
        {
            u23 = d3*rowstorage->ptr.p_double[offsu+srccol2*urowstride+3];
        }
    }
    if( srccol3>=0 )
    {
        if( urank>=1 )
        {
            u30 = d0*rowstorage->ptr.p_double[offsu+srccol3*urowstride+0];
        }
        if( urank>=2 )
        {
            u31 = d1*rowstorage->ptr.p_double[offsu+srccol3*urowstride+1];
        }
        if( urank>=3 )
        {
            u32 = d2*rowstorage->ptr.p_double[offsu+srccol3*urowstride+2];
        }
        if( urank>=4 )
        {
            u33 = d3*rowstorage->ptr.p_double[offsu+srccol3*urowstride+3];
        }
    }
    
    /*
     * Run update
     */
    if( urank==1 )
    {
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = offss+raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+k]]*4;
            offsk = offsu+k*urowstride;
            uk0 = rowstorage->ptr.p_double[offsk+0];
            rowstorage->ptr.p_double[targetrow+0] = rowstorage->ptr.p_double[targetrow+0]-u00*uk0;
            rowstorage->ptr.p_double[targetrow+1] = rowstorage->ptr.p_double[targetrow+1]-u10*uk0;
            rowstorage->ptr.p_double[targetrow+2] = rowstorage->ptr.p_double[targetrow+2]-u20*uk0;
            rowstorage->ptr.p_double[targetrow+3] = rowstorage->ptr.p_double[targetrow+3]-u30*uk0;
        }
    }
    if( urank==2 )
    {
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = offss+raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+k]]*4;
            offsk = offsu+k*urowstride;
            uk0 = rowstorage->ptr.p_double[offsk+0];
            uk1 = rowstorage->ptr.p_double[offsk+1];
            rowstorage->ptr.p_double[targetrow+0] = rowstorage->ptr.p_double[targetrow+0]-u00*uk0-u01*uk1;
            rowstorage->ptr.p_double[targetrow+1] = rowstorage->ptr.p_double[targetrow+1]-u10*uk0-u11*uk1;
            rowstorage->ptr.p_double[targetrow+2] = rowstorage->ptr.p_double[targetrow+2]-u20*uk0-u21*uk1;
            rowstorage->ptr.p_double[targetrow+3] = rowstorage->ptr.p_double[targetrow+3]-u30*uk0-u31*uk1;
        }
    }
    if( urank==3 )
    {
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = offss+raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+k]]*4;
            offsk = offsu+k*urowstride;
            uk0 = rowstorage->ptr.p_double[offsk+0];
            uk1 = rowstorage->ptr.p_double[offsk+1];
            uk2 = rowstorage->ptr.p_double[offsk+2];
            rowstorage->ptr.p_double[targetrow+0] = rowstorage->ptr.p_double[targetrow+0]-u00*uk0-u01*uk1-u02*uk2;
            rowstorage->ptr.p_double[targetrow+1] = rowstorage->ptr.p_double[targetrow+1]-u10*uk0-u11*uk1-u12*uk2;
            rowstorage->ptr.p_double[targetrow+2] = rowstorage->ptr.p_double[targetrow+2]-u20*uk0-u21*uk1-u22*uk2;
            rowstorage->ptr.p_double[targetrow+3] = rowstorage->ptr.p_double[targetrow+3]-u30*uk0-u31*uk1-u32*uk2;
        }
    }
    if( urank==4 )
    {
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = offss+raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+k]]*4;
            offsk = offsu+k*urowstride;
            uk0 = rowstorage->ptr.p_double[offsk+0];
            uk1 = rowstorage->ptr.p_double[offsk+1];
            uk2 = rowstorage->ptr.p_double[offsk+2];
            uk3 = rowstorage->ptr.p_double[offsk+3];
            rowstorage->ptr.p_double[targetrow+0] = rowstorage->ptr.p_double[targetrow+0]-u00*uk0-u01*uk1-u02*uk2-u03*uk3;
            rowstorage->ptr.p_double[targetrow+1] = rowstorage->ptr.p_double[targetrow+1]-u10*uk0-u11*uk1-u12*uk2-u13*uk3;
            rowstorage->ptr.p_double[targetrow+2] = rowstorage->ptr.p_double[targetrow+2]-u20*uk0-u21*uk1-u22*uk2-u23*uk3;
            rowstorage->ptr.p_double[targetrow+3] = rowstorage->ptr.p_double[targetrow+3]-u30*uk0-u31*uk1-u32*uk2-u33*uk3;
        }
    }
    result = ae_true;
    return result;
}


/*************************************************************************
Fast kernels for small supernodal updates: special 4x4x4x4 function.

! See comments on UpdateSupernode() for information  on generic supernodal
! updates, including notation used below.

The generic update has following form:

    S := S - scatter(U*D*Uc')

This specialized function performs 4x4x4x4 update, i.e.:
* S is a tHeight*4 matrix
* U is a uHeight*4 matrix
* Uc' is a 4*4 matrix
* scatter() scatters rows of U*Uc', but does not scatter columns (they are
  densely packed).
  
Return value:
* True if update was applied
* False if kernel refused to perform an update.

  -- ALGLIB routine --
     20.09.2020
     Bochkanov Sergey
*************************************************************************/
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
     ae_state *_state)
{
    ae_int_t k;
    ae_int_t targetrow;
    ae_int_t offsk;
    double d0;
    double d1;
    double d2;
    double d3;
    double u00;
    double u01;
    double u02;
    double u03;
    double u10;
    double u11;
    double u12;
    double u13;
    double u20;
    double u21;
    double u22;
    double u23;
    double u30;
    double u31;
    double u32;
    double u33;
    double uk0;
    double uk1;
    double uk2;
    double uk3;
    ae_bool result;


    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    _ALGLIB_KERNEL_RETURN_AVX2_FMA(spchol_updatekernel4444,(rowstorage->ptr.p_double, offss, sheight, offsu, uheight, diagd->ptr.p_double, offsd, raw2smap->ptr.p_int, superrowidx->ptr.p_int, urbase, _state))

    /*
     * Generic C fallback code
     */
    d0 = diagd->ptr.p_double[offsd+0];
    d1 = diagd->ptr.p_double[offsd+1];
    d2 = diagd->ptr.p_double[offsd+2];
    d3 = diagd->ptr.p_double[offsd+3];
    u00 = d0*rowstorage->ptr.p_double[offsu+0*4+0];
    u01 = d1*rowstorage->ptr.p_double[offsu+0*4+1];
    u02 = d2*rowstorage->ptr.p_double[offsu+0*4+2];
    u03 = d3*rowstorage->ptr.p_double[offsu+0*4+3];
    u10 = d0*rowstorage->ptr.p_double[offsu+1*4+0];
    u11 = d1*rowstorage->ptr.p_double[offsu+1*4+1];
    u12 = d2*rowstorage->ptr.p_double[offsu+1*4+2];
    u13 = d3*rowstorage->ptr.p_double[offsu+1*4+3];
    u20 = d0*rowstorage->ptr.p_double[offsu+2*4+0];
    u21 = d1*rowstorage->ptr.p_double[offsu+2*4+1];
    u22 = d2*rowstorage->ptr.p_double[offsu+2*4+2];
    u23 = d3*rowstorage->ptr.p_double[offsu+2*4+3];
    u30 = d0*rowstorage->ptr.p_double[offsu+3*4+0];
    u31 = d1*rowstorage->ptr.p_double[offsu+3*4+1];
    u32 = d2*rowstorage->ptr.p_double[offsu+3*4+2];
    u33 = d3*rowstorage->ptr.p_double[offsu+3*4+3];
    if( sheight==uheight )
    {
        /*
         * No row scatter, the most efficient code
         */
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = offss+k*4;
            offsk = offsu+k*4;
            uk0 = rowstorage->ptr.p_double[offsk+0];
            uk1 = rowstorage->ptr.p_double[offsk+1];
            uk2 = rowstorage->ptr.p_double[offsk+2];
            uk3 = rowstorage->ptr.p_double[offsk+3];
            rowstorage->ptr.p_double[targetrow+0] = rowstorage->ptr.p_double[targetrow+0]-u00*uk0-u01*uk1-u02*uk2-u03*uk3;
            rowstorage->ptr.p_double[targetrow+1] = rowstorage->ptr.p_double[targetrow+1]-u10*uk0-u11*uk1-u12*uk2-u13*uk3;
            rowstorage->ptr.p_double[targetrow+2] = rowstorage->ptr.p_double[targetrow+2]-u20*uk0-u21*uk1-u22*uk2-u23*uk3;
            rowstorage->ptr.p_double[targetrow+3] = rowstorage->ptr.p_double[targetrow+3]-u30*uk0-u31*uk1-u32*uk2-u33*uk3;
        }
    }
    else
    {
        /*
         * Row scatter is performed, less efficient code using double mapping to determine target row index
         */
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = offss+raw2smap->ptr.p_int[superrowidx->ptr.p_int[urbase+k]]*4;
            offsk = offsu+k*4;
            uk0 = rowstorage->ptr.p_double[offsk+0];
            uk1 = rowstorage->ptr.p_double[offsk+1];
            uk2 = rowstorage->ptr.p_double[offsk+2];
            uk3 = rowstorage->ptr.p_double[offsk+3];
            rowstorage->ptr.p_double[targetrow+0] = rowstorage->ptr.p_double[targetrow+0]-u00*uk0-u01*uk1-u02*uk2-u03*uk3;
            rowstorage->ptr.p_double[targetrow+1] = rowstorage->ptr.p_double[targetrow+1]-u10*uk0-u11*uk1-u12*uk2-u13*uk3;
            rowstorage->ptr.p_double[targetrow+2] = rowstorage->ptr.p_double[targetrow+2]-u20*uk0-u21*uk1-u22*uk2-u23*uk3;
            rowstorage->ptr.p_double[targetrow+3] = rowstorage->ptr.p_double[targetrow+3]-u30*uk0-u31*uk1-u32*uk2-u33*uk3;
        }
    }
    result = ae_true;
    return result;
}

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
     ae_state *_state)
{
    /*
     * Only panelp=15 is supported
     */
    if( panelp!=15 )
        return ae_false;

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    _ALGLIB_KERNEL_RETURN_AVX2(rbfv3farfields_bhpaneleval1fastkernel16,(d0,d1,d2,pnma->ptr.p_double,pnmb->ptr.p_double,pmmcdiag->ptr.p_double,ynma->ptr.p_double,tblrmodmn->ptr.p_double,f,invpowrpplus1,_state))

    /*
     * No fast kernels, no generic C implementation
     */
    return ae_false;
}

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
     ae_state *_state)
{
    /*
     * Only panelp=15 is supported
     */
    if( panelp!=15 )
        return ae_false;

    /*
     * Try fast kernels.
     * On success this macro will return, on failure to find kernel it will pass execution to the generic C implementation
     */
    _ALGLIB_KERNEL_RETURN_AVX2(rbfv3farfields_bhpanelevalfastkernel16,(d0,d1,d2,ny,pnma->ptr.p_double,pnmb->ptr.p_double,pmmcdiag->ptr.p_double,ynma->ptr.p_double,tblrmodmn->ptr.p_double,f->ptr.p_double,invpowrpplus1,_state))

    /*
     * No fast kernels, no generic C implementation
     */
    return ae_false;
}

/* ALGLIB_NO_FAST_KERNELS */
#endif

/*$ End $*/