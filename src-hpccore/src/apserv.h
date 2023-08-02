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

#ifndef _apserv_h
#define _apserv_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"


/*$ Declarations $*/


/*************************************************************************
Buffers for internal functions which need buffers:
* check for size of the buffer you want to use.
* if buffer is too small, resize it; leave unchanged, if it is larger than
  needed.
* use it.

We can pass this structure to multiple functions;  after first run through
functions buffer sizes will be finally determined,  and  on  a next run no
allocation will be required.
*************************************************************************/
typedef struct
{
    ae_vector ba0;
    ae_vector ia0;
    ae_vector ia1;
    ae_vector ia2;
    ae_vector ia3;
    ae_vector ra0;
    ae_vector ra1;
    ae_vector ra2;
    ae_vector ra3;
    ae_matrix rm0;
    ae_matrix rm1;
} apbuffers;


/*************************************************************************
Structure which is used to workaround limitations of ALGLIB parallellization
environment.

  -- ALGLIB --
     Copyright 12.04.2009 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    ae_bool val;
} sboolean;


/*************************************************************************
Structure which is used to workaround limitations of ALGLIB parallellization
environment.

  -- ALGLIB --
     Copyright 12.04.2009 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    ae_vector val;
} sbooleanarray;


/*************************************************************************
Structure which is used to workaround limitations of ALGLIB parallellization
environment.

  -- ALGLIB --
     Copyright 12.04.2009 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    ae_int_t val;
} sinteger;


/*************************************************************************
Structure which is used to workaround limitations of ALGLIB parallellization
environment.

  -- ALGLIB --
     Copyright 12.04.2009 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    ae_vector val;
} sintegerarray;


/*************************************************************************
Structure which is used to workaround limitations of ALGLIB parallellization
environment.

  -- ALGLIB --
     Copyright 12.04.2009 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    double val;
} sreal;


/*************************************************************************
Structure which is used to workaround limitations of ALGLIB parallellization
environment.

  -- ALGLIB --
     Copyright 12.04.2009 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    ae_vector val;
} srealarray;


/*************************************************************************
Structure which is used to workaround limitations of ALGLIB parallellization
environment.

  -- ALGLIB --
     Copyright 12.04.2009 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    ae_complex val;
} scomplex;


/*************************************************************************
Structure which is used to workaround limitations of ALGLIB parallellization
environment.

  -- ALGLIB --
     Copyright 12.04.2009 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    ae_vector val;
} scomplexarray;


/*************************************************************************
Thread-safe pool used to store/retrieve/recycle N-length boolean arrays.

  -- ALGLIB --
     Copyright 06.07.2022 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t temporariescount;
    ae_shared_pool sourcepool;
    ae_shared_pool temporarypool;
    sbooleanarray seed0;
    sbooleanarray seedn;
} nbpool;


/*************************************************************************
Thread-safe pool used to store/retrieve/recycle N-length integer arrays.

  -- ALGLIB --
     Copyright 06.07.2022 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t temporariescount;
    ae_shared_pool sourcepool;
    ae_shared_pool temporarypool;
    sintegerarray seed0;
    sintegerarray seedn;
} nipool;


/*************************************************************************
Thread-safe pool used to store/retrieve/recycle N-length real arrays.

  -- ALGLIB --
     Copyright 06.07.2022 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t temporariescount;
    ae_shared_pool sourcepool;
    ae_shared_pool temporarypool;
    srealarray seed0;
    srealarray seedn;
} nrpool;


/*************************************************************************
Counter used to compute average value of a set of numbers

  -- ALGLIB --
     Copyright 06.07.2022 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    double rsum;
    double rcnt;
    double prior;
} savgcounter;


/*************************************************************************
Counter used to compute a quantile of a set of numbers. Internally stores
entire set, recomputes quantile with O(N) algo every time we req

  -- ALGLIB --
     Copyright 06.07.2022 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    ae_int_t cnt;
    ae_vector elems;
    double prior;
} squantilecounter;


/*$ Body $*/


/*************************************************************************
Internally calls SetErrorFlag() with condition:

    Abs(Val-RefVal)>Tol*Max(Abs(RefVal),S)
    
This function is used to test relative error in Val against  RefVal,  with
relative error being replaced by absolute when scale  of  RefVal  is  less
than S.

This function returns value of COND.
*************************************************************************/
void seterrorflagdiff(ae_bool* flag,
     double val,
     double refval,
     double tol,
     double s,
     ae_state *_state);


/*************************************************************************
The function always returns False.
It may be used sometimes to prevent spurious warnings.

  -- ALGLIB --
     Copyright 17.09.2012 by Bochkanov Sergey
*************************************************************************/
ae_bool alwaysfalse(ae_state *_state);


/*************************************************************************
The function "touches" boolean - it is used  to  avoid  compiler  messages
about unused variables (in rare cases when we do NOT want to remove  these
variables).

  -- ALGLIB --
     Copyright 17.09.2012 by Bochkanov Sergey
*************************************************************************/
void touchboolean(ae_bool* a, ae_state *_state);


/*************************************************************************
The function "touches" integer - it is used  to  avoid  compiler  messages
about unused variables (in rare cases when we do NOT want to remove  these
variables).

  -- ALGLIB --
     Copyright 17.09.2012 by Bochkanov Sergey
*************************************************************************/
void touchint(ae_int_t* a, ae_state *_state);


/*************************************************************************
The function "touches" real   -  it is used  to  avoid  compiler  messages
about unused variables (in rare cases when we do NOT want to remove  these
variables).

  -- ALGLIB --
     Copyright 17.09.2012 by Bochkanov Sergey
*************************************************************************/
void touchreal(double* a, ae_state *_state);


/*************************************************************************
The function performs zero-coalescing on real value.

NOTE: no check is performed for B<>0

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
double coalesce(double a, double b, ae_state *_state);


/*************************************************************************
The function performs zero-coalescing on integer value.

NOTE: no check is performed for B<>0

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
ae_int_t coalescei(ae_int_t a, ae_int_t b, ae_state *_state);


/*************************************************************************
The function convert integer value to real value.

  -- ALGLIB --
     Copyright 17.09.2012 by Bochkanov Sergey
*************************************************************************/
double inttoreal(ae_int_t a, ae_state *_state);


/*************************************************************************
The function calculates binary logarithm.

NOTE: it costs twice as much as Ln(x)

  -- ALGLIB --
     Copyright 17.09.2012 by Bochkanov Sergey
*************************************************************************/
double logbase2(double x, ae_state *_state);


/*************************************************************************
This function compares two numbers for approximate equality, with tolerance
to errors as large as tol.


  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
ae_bool approxequal(double a, double b, double tol, ae_state *_state);


/*************************************************************************
This function compares two numbers for approximate equality, with tolerance
to errors as large as max(|a|,|b|)*tol.


  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
ae_bool approxequalrel(double a, double b, double tol, ae_state *_state);


/*************************************************************************
This  function  generates  1-dimensional  general  interpolation task with
moderate Lipshitz constant (close to 1.0)

If N=1 then suborutine generates only one point at the middle of [A,B]

  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
void taskgenint1d(double a,
     double b,
     ae_int_t n,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);


/*************************************************************************
This function generates  1-dimensional equidistant interpolation task with
moderate Lipshitz constant (close to 1.0)

If N=1 then suborutine generates only one point at the middle of [A,B]

  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
void taskgenint1dequidist(double a,
     double b,
     ae_int_t n,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);


/*************************************************************************
This function generates  1-dimensional Chebyshev-1 interpolation task with
moderate Lipshitz constant (close to 1.0)

If N=1 then suborutine generates only one point at the middle of [A,B]

  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
void taskgenint1dcheb1(double a,
     double b,
     ae_int_t n,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);


/*************************************************************************
This function generates  1-dimensional Chebyshev-2 interpolation task with
moderate Lipshitz constant (close to 1.0)

If N=1 then suborutine generates only one point at the middle of [A,B]

  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
void taskgenint1dcheb2(double a,
     double b,
     ae_int_t n,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);


/*************************************************************************
This function checks that all values from X[] are distinct. It does more
than just usual floating point comparison:
* first, it calculates max(X) and min(X)
* second, it maps X[] from [min,max] to [1,2]
* only at this stage actual comparison is done

The meaning of such check is to ensure that all values are "distinct enough"
and will not cause interpolation subroutine to fail.

NOTE:
    X[] must be sorted by ascending (subroutine ASSERT's it)

  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
ae_bool aredistinct(/* Real    */ const ae_vector* x,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
This function checks that two boolean values are the same (both  are  True 
or both are False).

  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
ae_bool aresameboolean(ae_bool v1, ae_bool v2, ae_state *_state);


/*************************************************************************
Resizes X and fills by zeros

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void setlengthzero(/* Real    */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
If Length(X)<N, resizes X

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void bvectorsetlengthatleast(/* Boolean */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
If Length(X)<N, resizes X

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void ivectorsetlengthatleast(/* Integer */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
If Length(X)<N, resizes X

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void rvectorsetlengthatleast(/* Real    */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
If Cols(X)<N or Rows(X)<M, resizes X

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void rmatrixsetlengthatleast(/* Real    */ ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
If Cols(X)<N or Rows(X)<M, resizes X

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void bmatrixsetlengthatleast(/* Boolean */ ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Grows X, i.e. changes its size in such a way that:
a) contents is preserved
b) new size is at least N
c) new size can be larger than N, so subsequent grow() calls can return
   without reallocation

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void bvectorgrowto(/* Boolean */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Grows X, i.e. changes its size in such a way that:
a) contents is preserved
b) new size is at least N
c) new size can be larger than N, so subsequent grow() calls can return
   without reallocation

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void ivectorgrowto(/* Integer */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Grows X, i.e. appends rows in such a way that:
a) contents is preserved
b) new row count is at least N
c) new row count can be larger than N, so subsequent grow() calls can return
   without reallocation
d) new matrix has at least MinCols columns (if less than specified amount
   of columns is present, new columns are added with undefined contents);
   MinCols can be 0 or negative value = ignored

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void rmatrixgrowrowsto(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_int_t mincols,
     ae_state *_state);


/*************************************************************************
Grows X, i.e. appends cols in such a way that:
a) contents is preserved
b) new col count is at least N
c) new col count can be larger than N, so subsequent grow() calls can return
   without reallocation
d) new matrix has at least MinRows row (if less than specified amount
   of rows is present, new rows are added with undefined contents);
   MinRows can be 0 or negative value = ignored

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void rmatrixgrowcolsto(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_int_t minrows,
     ae_state *_state);


/*************************************************************************
Grows X, i.e. changes its size in such a way that:
a) contents is preserved
b) new size is at least N
c) new size can be larger than N, so subsequent grow() calls can return
   without reallocation

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void rvectorgrowto(/* Real    */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Resizes X and:
* preserves old contents of X
* fills new elements by zeros

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void ivectorresize(/* Integer */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Resizes X and:
* preserves old contents of X
* fills new elements by zeros

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void rvectorresize(/* Real    */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Resizes X and:
* preserves old contents of X
* fills new elements by zeros

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void rmatrixresize(/* Real    */ ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Resizes X and:
* preserves old contents of X
* fills new elements by zeros

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void imatrixresize(/* Integer */ ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
appends element to X

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void ivectorappend(/* Integer */ ae_vector* x,
     ae_int_t v,
     ae_state *_state);


/*************************************************************************
This function checks that length(X) is at least N and first N values  from
X[] are finite

  -- ALGLIB --
     Copyright 18.06.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool isfinitevector(/* Real    */ const ae_vector* x,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
This function checks that length(X) is at least N and first N values  from
X[] are finite or NANs

  -- ALGLIB --
     Copyright 18.06.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool isfiniteornanvector(/* Real    */ const ae_vector* x,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
This function checks that first N values from X[] are finite

  -- ALGLIB --
     Copyright 18.06.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool isfinitecvector(/* Complex */ const ae_vector* z,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
This function checks that size of X is at least MxN and values from
X[0..M-1,0..N-1] are finite.

  -- ALGLIB --
     Copyright 18.06.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool apservisfinitematrix(/* Real    */ const ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
This function checks that all values from X[0..M-1,0..N-1] are finite

  -- ALGLIB --
     Copyright 18.06.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool apservisfinitecmatrix(/* Complex */ const ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
This function checks that all values from X[0..M-1,0..N-1] are finite

  -- ALGLIB --
     Copyright 18.06.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool isfinitecmatrix(/* Complex */ const ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
This function checks that size of X is at least NxN and all values from
upper/lower triangle of X[0..N-1,0..N-1] are finite

  -- ALGLIB --
     Copyright 18.06.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool isfinitertrmatrix(/* Real    */ const ae_matrix* x,
     ae_int_t n,
     ae_bool isupper,
     ae_state *_state);


/*************************************************************************
This function checks that all values from upper/lower triangle of
X[0..N-1,0..N-1] are finite

  -- ALGLIB --
     Copyright 18.06.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool apservisfinitectrmatrix(/* Complex */ const ae_matrix* x,
     ae_int_t n,
     ae_bool isupper,
     ae_state *_state);


/*************************************************************************
This function checks that all values from upper/lower triangle of
X[0..N-1,0..N-1] are finite

  -- ALGLIB --
     Copyright 18.06.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool isfinitectrmatrix(/* Complex */ const ae_matrix* x,
     ae_int_t n,
     ae_bool isupper,
     ae_state *_state);


/*************************************************************************
This function checks that all values from X[0..M-1,0..N-1] are  finite  or
NaN's.

  -- ALGLIB --
     Copyright 18.06.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool apservisfiniteornanmatrix(/* Real    */ const ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Safe sqrt(x^2+y^2)

  -- ALGLIB --
     Copyright by Bochkanov Sergey
*************************************************************************/
double safepythag2(double x, double y, ae_state *_state);


/*************************************************************************
Safe sqrt(x^2+y^2)

  -- ALGLIB --
     Copyright by Bochkanov Sergey
*************************************************************************/
double safepythag3(double x, double y, double z, ae_state *_state);


/*************************************************************************
Safe division.

This function attempts to calculate R=X/Y without overflow.

It returns:
* +1, if abs(X/Y)>=MaxRealNumber or undefined - overflow-like situation
      (no overlfow is generated, R is either NAN, PosINF, NegINF)
*  0, if MinRealNumber<abs(X/Y)<MaxRealNumber or X=0, Y<>0
      (R contains result, may be zero)
* -1, if 0<abs(X/Y)<MinRealNumber - underflow-like situation
      (R contains zero; it corresponds to underflow)

No overflow is generated in any case.

  -- ALGLIB --
     Copyright by Bochkanov Sergey
*************************************************************************/
ae_int_t saferdiv(double x, double y, double* r, ae_state *_state);


/*************************************************************************
This function calculates "safe" min(X/Y,V) for positive finite X, Y, V.
No overflow is generated in any case.

  -- ALGLIB --
     Copyright by Bochkanov Sergey
*************************************************************************/
double safeminposrv(double x, double y, double v, ae_state *_state);


/*************************************************************************
This function makes periodic mapping of X to [A,B].

It accepts X, A, B (A>B). It returns T which lies in  [A,B] and integer K,
such that X = T + K*(B-A).

NOTES:
* K is represented as real value, although actually it is integer
* T is guaranteed to be in [A,B]
* T replaces X

  -- ALGLIB --
     Copyright by Bochkanov Sergey
*************************************************************************/
void apperiodicmap(double* x,
     double a,
     double b,
     double* k,
     ae_state *_state);


/*************************************************************************
Returns random normal number using low-quality system-provided generator

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
double randomnormal(ae_state *_state);


/*************************************************************************
Generates random unit vector using low-quality system-provided generator.
Reallocates array if its size is too short.

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void randomunit(ae_int_t n, /* Real    */ ae_vector* x, ae_state *_state);


/*************************************************************************
This function is used to swap two integer values
*************************************************************************/
void swapi(ae_int_t* v0, ae_int_t* v1, ae_state *_state);


/*************************************************************************
This function is used to swap two real values
*************************************************************************/
void swapr(double* v0, double* v1, ae_state *_state);


/*************************************************************************
This function is used to swap two rows of the matrix; if NCols<0, automatically
determined from the matrix size.
*************************************************************************/
void swaprows(/* Real    */ ae_matrix* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_int_t ncols,
     ae_state *_state);


/*************************************************************************
This function is used to swap two cols of the matrix; if NRows<0, automatically
determined from the matrix size.
*************************************************************************/
void swapcols(/* Real    */ ae_matrix* a,
     ae_int_t j0,
     ae_int_t j1,
     ae_int_t nrows,
     ae_state *_state);


/*************************************************************************
This function is used to swap two "entries" in 1-dimensional array composed
from D-element entries
*************************************************************************/
void swapentries(/* Real    */ ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_int_t entrywidth,
     ae_state *_state);


/*************************************************************************
This function is used to swap two "entries" in 1-dimensional array composed
from D-element entries
*************************************************************************/
void swapentriesb(/* Boolean */ ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_int_t entrywidth,
     ae_state *_state);


/*************************************************************************
This function is used to swap two elements of the vector
*************************************************************************/
void swapelements(/* Real    */ ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_state *_state);


/*************************************************************************
This function is used to swap two elements of the vector
*************************************************************************/
void swapelementsi(/* Integer */ ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_state *_state);


/*************************************************************************
This function is used to return maximum of three real values
*************************************************************************/
double maxreal3(double v0, double v1, double v2, ae_state *_state);


/*************************************************************************
This function is used to increment value of integer variable
*************************************************************************/
void inc(ae_int_t* v, ae_state *_state);


/*************************************************************************
This function is used to decrement value of integer variable
*************************************************************************/
void dec(ae_int_t* v, ae_state *_state);


/*************************************************************************
This function is used to increment value of integer variable; name of  the
function suggests that increment is done in multithreaded setting  in  the
thread-unsafe manner (optional progress reports which do not need guaranteed
correctness)
*************************************************************************/
void threadunsafeinc(ae_int_t* v, ae_state *_state);


/*************************************************************************
This function is used to increment value of integer variable; name of  the
function suggests that increment is done in multithreaded setting  in  the
thread-unsafe manner (optional progress reports which do not need guaranteed
correctness)
*************************************************************************/
void threadunsafeincby(ae_int_t* v, ae_int_t k, ae_state *_state);


/*************************************************************************
This function performs two operations:
1. decrements value of integer variable, if it is positive
2. explicitly sets variable to zero if it is non-positive
It is used by some algorithms to decrease value of internal counters.
*************************************************************************/
void countdown(ae_int_t* v, ae_state *_state);


/*************************************************************************
This function returns +1 or -1 depending on sign of X.
x=0 results in +1 being returned.
*************************************************************************/
double possign(double x, ae_state *_state);


/*************************************************************************
This function returns product of two real numbers. It is convenient when
you have to perform typecast-and-product of two INTEGERS.
*************************************************************************/
double rmul2(double v0, double v1, ae_state *_state);


/*************************************************************************
This function returns product of three real numbers. It is convenient when
you have to perform typecast-and-product of three INTEGERS.
*************************************************************************/
double rmul3(double v0, double v1, double v2, ae_state *_state);


/*************************************************************************
This function returns product of four real numbers. It is convenient when
you have to perform typecast-and-product of four INTEGERS.
*************************************************************************/
double rmul4(double v0, double v1, double v2, double v3, ae_state *_state);


/*************************************************************************
This function returns (A div B) rounded up; it expects that A>0, B>0, but
does not check it.
*************************************************************************/
ae_int_t idivup(ae_int_t a, ae_int_t b, ae_state *_state);


/*************************************************************************
This function returns min(i0,i1)
*************************************************************************/
ae_int_t imin2(ae_int_t i0, ae_int_t i1, ae_state *_state);


/*************************************************************************
This function returns min(i0,i1,i2)
*************************************************************************/
ae_int_t imin3(ae_int_t i0, ae_int_t i1, ae_int_t i2, ae_state *_state);


/*************************************************************************
This function returns max(i0,i1)
*************************************************************************/
ae_int_t imax2(ae_int_t i0, ae_int_t i1, ae_state *_state);


/*************************************************************************
This function returns max(i0,i1,i2)
*************************************************************************/
ae_int_t imax3(ae_int_t i0, ae_int_t i1, ae_int_t i2, ae_state *_state);


/*************************************************************************
This function returns max(r0,r1,r2)
*************************************************************************/
double rmax3(double r0, double r1, double r2, ae_state *_state);


/*************************************************************************
This function returns max(|r0|,|r1|,|r2|)
*************************************************************************/
double rmaxabs3(double r0, double r1, double r2, ae_state *_state);


/*************************************************************************
'bounds' value: maps X to [B1,B2]

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
double boundval(double x, double b1, double b2, ae_state *_state);


/*************************************************************************
'bounds' value: maps X to [B1,B2]

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
ae_int_t iboundval(ae_int_t x, ae_int_t b1, ae_int_t b2, ae_state *_state);


/*************************************************************************
'bounds' value: maps X to [B1,B2]

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
double rboundval(double x, double b1, double b2, ae_state *_state);


/*************************************************************************
Boolean case-2: returns V0 if Cond=True, V1 otherwise
*************************************************************************/
ae_bool bcase2(ae_bool cond, ae_bool v0, ae_bool v1, ae_state *_state);


/*************************************************************************
Integer case-2: returns V0 if Cond=True, V1 otherwise
*************************************************************************/
ae_int_t icase2(ae_bool cond, ae_int_t v0, ae_int_t v1, ae_state *_state);


/*************************************************************************
Real case-2: returns V0 if Cond=True, V1 otherwise
*************************************************************************/
double rcase2(ae_bool cond, double v0, double v1, ae_state *_state);


/*************************************************************************
Returns number of non-zeros
*************************************************************************/
ae_int_t countnz1(/* Real    */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Returns number of non-zeros
*************************************************************************/
ae_int_t countnz2(/* Real    */ const ae_matrix* v,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Allocation of serializer: complex value
*************************************************************************/
void alloccomplex(ae_serializer* s, ae_complex v, ae_state *_state);


/*************************************************************************
Serialization: complex value
*************************************************************************/
void serializecomplex(ae_serializer* s, ae_complex v, ae_state *_state);


/*************************************************************************
Unserialization: complex value
*************************************************************************/
ae_complex unserializecomplex(ae_serializer* s, ae_state *_state);


/*************************************************************************
Allocation of serializer: real array
*************************************************************************/
void allocrealarray(ae_serializer* s,
     /* Real    */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Allocation of serializer: boolean array
*************************************************************************/
void allocbooleanarray(ae_serializer* s,
     /* Boolean */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Serialization: complex value
*************************************************************************/
void serializerealarray(ae_serializer* s,
     /* Real    */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Serialization: boolean array
*************************************************************************/
void serializebooleanarray(ae_serializer* s,
     /* Boolean */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Unserialization: complex value
*************************************************************************/
void unserializerealarray(ae_serializer* s,
     /* Real    */ ae_vector* v,
     ae_state *_state);


/*************************************************************************
Unserialization: boolean value
*************************************************************************/
void unserializebooleanarray(ae_serializer* s,
     /* Boolean */ ae_vector* v,
     ae_state *_state);


/*************************************************************************
Allocation of serializer: Integer array
*************************************************************************/
void allocintegerarray(ae_serializer* s,
     /* Integer */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Serialization: Integer array
*************************************************************************/
void serializeintegerarray(ae_serializer* s,
     /* Integer */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
Unserialization: complex value
*************************************************************************/
void unserializeintegerarray(ae_serializer* s,
     /* Integer */ ae_vector* v,
     ae_state *_state);


/*************************************************************************
Allocation of serializer: real matrix
*************************************************************************/
void allocrealmatrix(ae_serializer* s,
     /* Real    */ const ae_matrix* v,
     ae_int_t n0,
     ae_int_t n1,
     ae_state *_state);


/*************************************************************************
Serialization: complex value
*************************************************************************/
void serializerealmatrix(ae_serializer* s,
     /* Real    */ const ae_matrix* v,
     ae_int_t n0,
     ae_int_t n1,
     ae_state *_state);


/*************************************************************************
Unserialization: complex value
*************************************************************************/
void unserializerealmatrix(ae_serializer* s,
     /* Real    */ ae_matrix* v,
     ae_state *_state);


/*************************************************************************
Copy boolean array
*************************************************************************/
void copybooleanarray(/* Boolean */ const ae_vector* src,
     /* Boolean */ ae_vector* dst,
     ae_state *_state);


/*************************************************************************
Copy integer array
*************************************************************************/
void copyintegerarray(/* Integer */ const ae_vector* src,
     /* Integer */ ae_vector* dst,
     ae_state *_state);


/*************************************************************************
Copy real array
*************************************************************************/
void copyrealarray(/* Real    */ const ae_vector* src,
     /* Real    */ ae_vector* dst,
     ae_state *_state);


/*************************************************************************
Copy real matrix
*************************************************************************/
void copyrealmatrix(/* Real    */ const ae_matrix* src,
     /* Real    */ ae_matrix* dst,
     ae_state *_state);


/*************************************************************************
Clears integer array
*************************************************************************/
void unsetintegerarray(/* Integer */ ae_vector* a, ae_state *_state);


/*************************************************************************
Clears real array
*************************************************************************/
void unsetrealarray(/* Real    */ ae_vector* a, ae_state *_state);


/*************************************************************************
Clears real matrix
*************************************************************************/
void unsetrealmatrix(/* Real    */ ae_matrix* a, ae_state *_state);


/*************************************************************************
Initialize nbPool - prepare it to store N-length arrays, N>=0.
Tries to reuse previously allocated memory as much as possible.
*************************************************************************/
void nbpoolinit(nbpool* pool, ae_int_t n, ae_state *_state);


/*************************************************************************
Thread-safe retrieval of array from the nbPool. If there are enough arrays
in the pool, it is performed without additional dynamic allocations.

INPUT PARAMETERS:
    Pool        -   nbPool properly initialized with nbPoolInit
    A           -   array[0], must have exactly zero length (exception will
                    be generated if length is different from zero)
                    
OUTPUT PARAMETERS:
    A           -   array[N], contents undefined
*************************************************************************/
void nbpoolretrieve(nbpool* pool,
     /* Boolean */ ae_vector* a,
     ae_state *_state);


/*************************************************************************
Thread-safe recycling of N-length array to the nbPool.

INPUT PARAMETERS:
    Pool        -   nbPool properly initialized with nbPoolInit
    A           -   array[N], length must be N exactly (exception will
                    be generated if length is different from N)
                    
OUTPUT PARAMETERS:
    A           -   array[0], length is exactly zero on exit
*************************************************************************/
void nbpoolrecycle(nbpool* pool,
     /* Boolean */ ae_vector* a,
     ae_state *_state);


/*************************************************************************
Initialize niPool - prepare it to store N-length arrays, N>=0.
Tries to reuse previously allocated memory as much as possible.
*************************************************************************/
void nipoolinit(nipool* pool, ae_int_t n, ae_state *_state);


/*************************************************************************
Thread-safe retrieval of array from the nrPool. If there are enough arrays
in the pool, it is performed without additional dynamic allocations.

INPUT PARAMETERS:
    Pool        -   niPool properly initialized with niPoolInit
    A           -   array[0], must have exactly zero length (exception will
                    be generated if length is different from zero)
                    
OUTPUT PARAMETERS:
    A           -   array[N], contents undefined
*************************************************************************/
void nipoolretrieve(nipool* pool,
     /* Integer */ ae_vector* a,
     ae_state *_state);


/*************************************************************************
Thread-safe recycling of N-length array to the niPool.

INPUT PARAMETERS:
    Pool        -   niPool properly initialized with niPoolInit
    A           -   array[N], length must be N exactly (exception will
                    be generated if length is different from N)
                    
OUTPUT PARAMETERS:
    A           -   array[0], length is exactly zero on exit
*************************************************************************/
void nipoolrecycle(nipool* pool,
     /* Integer */ ae_vector* a,
     ae_state *_state);


/*************************************************************************
Initialize nrPool - prepare it to store N-length arrays, N>=0.
Tries to reuse previously allocated memory as much as possible.
*************************************************************************/
void nrpoolinit(nrpool* pool, ae_int_t n, ae_state *_state);


/*************************************************************************
Thread-safe retrieval of array from the nrPool. If there are enough arrays
in the pool, it is performed without additional dynamic allocations.

INPUT PARAMETERS:
    Pool        -   nrPool properly initialized with nrPoolInit
    A           -   array[0], must have exactly zero length (exception will
                    be generated if length is different from zero)
                    
OUTPUT PARAMETERS:
    A           -   array[N], contents undefined
*************************************************************************/
void nrpoolretrieve(nrpool* pool,
     /* Real    */ ae_vector* a,
     ae_state *_state);


/*************************************************************************
Thread-safe recycling of N-length array to the nrPool.

INPUT PARAMETERS:
    Pool        -   nrPool properly initialized with nrPoolInit
    A           -   array[N], length must be N exactly (exception will
                    be generated if length is different from N)
                    
OUTPUT PARAMETERS:
    A           -   array[0], length is exactly zero on exit
*************************************************************************/
void nrpoolrecycle(nrpool* pool,
     /* Real    */ ae_vector* a,
     ae_state *_state);


/*************************************************************************
This function is used in parallel functions for recurrent division of large
task into two smaller tasks.

It has following properties:
* it works only for TaskSize>=2 and TaskSize>TileSize (assertion is thrown otherwise)
* Task0+Task1=TaskSize, Task0>0, Task1>0
* Task0 and Task1 are close to each other
* Task0>=Task1
* Task0 is always divisible by TileSize

  -- ALGLIB --
     Copyright 07.04.2013 by Bochkanov Sergey
*************************************************************************/
void tiledsplit(ae_int_t tasksize,
     ae_int_t tilesize,
     ae_int_t* task0,
     ae_int_t* task1,
     ae_state *_state);


/*************************************************************************
This function searches integer array. Elements in this array are actually
records, each NRec elements wide. Each record has unique header - NHeader
integer values, which identify it. Records are lexicographically sorted by
header.

Records are identified by their index, not offset (offset = NRec*index).

This function searches A (records with indices [I0,I1)) for a record with
header B. It returns index of this record (not offset!), or -1 on failure.

  -- ALGLIB --
     Copyright 28.03.2011 by Bochkanov Sergey
*************************************************************************/
ae_int_t recsearch(/* Integer */ const ae_vector* a,
     ae_int_t nrec,
     ae_int_t nheader,
     ae_int_t i0,
     ae_int_t i1,
     /* Integer */ const ae_vector* b,
     ae_state *_state);


/*************************************************************************
This function is used in parallel functions for recurrent division of large
task into two smaller tasks.

It has following properties:
* it works only for TaskSize>=2 (assertion is thrown otherwise)
* for TaskSize=2, it returns Task0=1, Task1=1
* in case TaskSize is odd,  Task0=TaskSize-1, Task1=1
* in case TaskSize is even, Task0 and Task1 are approximately TaskSize/2
  and both Task0 and Task1 are even, Task0>=Task1

  -- ALGLIB --
     Copyright 07.04.2013 by Bochkanov Sergey
*************************************************************************/
void splitlengtheven(ae_int_t tasksize,
     ae_int_t* task0,
     ae_int_t* task1,
     ae_state *_state);


/*************************************************************************
This function is used to calculate number of chunks (including partial,
non-complete chunks) in some set. It expects that ChunkSize>=1, TaskSize>=0.
Assertion is thrown otherwise.

Function result is equivalent to Ceil(TaskSize/ChunkSize), but with guarantees
that rounding errors won't ruin results.

  -- ALGLIB --
     Copyright 21.01.2015 by Bochkanov Sergey
*************************************************************************/
ae_int_t chunkscount(ae_int_t tasksize,
     ae_int_t chunksize,
     ae_state *_state);


/*************************************************************************
Returns maximum density for level 2 sparse/dense functions. Density values
below one returned by this function are better to handle via sparse Level 2
functionality.

  -- ALGLIB routine --
     10.01.2019
     Bochkanov Sergey
*************************************************************************/
double sparselevel2density(ae_state *_state);


/*************************************************************************
Returns A-tile size for a matrix.

A-tiles are smallest tiles (32x32), suitable for processing by ALGLIB  own
implementation of Level 3 linear algebra.

  -- ALGLIB routine --
     10.01.2019
     Bochkanov Sergey
*************************************************************************/
ae_int_t matrixtilesizea(ae_state *_state);


/*************************************************************************
Returns B-tile size for a matrix.

B-tiles are larger  tiles (64x64), suitable for parallel execution or for
processing by vendor's implementation of Level 3 linear algebra.

  -- ALGLIB routine --
     10.01.2019
     Bochkanov Sergey
*************************************************************************/
ae_int_t matrixtilesizeb(ae_state *_state);


/*************************************************************************
This function returns minimum cost of task which is feasible for
multithreaded processing. It returns real number in order to avoid overflow
problems.

  -- ALGLIB --
     Copyright 10.01.2018 by Bochkanov Sergey
*************************************************************************/
double smpactivationlevel(ae_state *_state);


/*************************************************************************
This function returns minimum cost of task which is feasible for
spawn (given that multithreading is active).

It returns real number in order to avoid overflow problems.

  -- ALGLIB --
     Copyright 10.01.2018 by Bochkanov Sergey
*************************************************************************/
double spawnlevel(ae_state *_state);


/*************************************************************************
--- OBSOLETE FUNCTION, USE TILED SPLIT INSTEAD --- 

This function is used in parallel functions for recurrent division of large
task into two smaller tasks.

It has following properties:
* it works only for TaskSize>=2 and ChunkSize>=2
  (assertion is thrown otherwise)
* Task0+Task1=TaskSize, Task0>0, Task1>0
* Task0 and Task1 are close to each other
* in case TaskSize>ChunkSize, Task0 is always divisible by ChunkSize

  -- ALGLIB --
     Copyright 07.04.2013 by Bochkanov Sergey
*************************************************************************/
void splitlength(ae_int_t tasksize,
     ae_int_t chunksize,
     ae_int_t* task0,
     ae_int_t* task1,
     ae_state *_state);


/*************************************************************************
Outputs vector A[I0,I1-1] to trace log using either:
a)  6-digit exponential format (no trace flags is set)
b) 15-ditit exponential format ('PREC.E15' trace flag is set)
b)  6-ditit fixed-point format ('PREC.F6' trace flag is set)

This function checks trace flags every time it is called.
*************************************************************************/
void tracevectorautoprec(/* Real    */ const ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_state *_state);


/*************************************************************************
Outputs row A[I,J0..J1-1] to trace log using either:
a)  6-digit exponential format (no trace flags is set)
b) 15-ditit exponential format ('PREC.E15' trace flag is set)
b)  6-ditit fixed-point format ('PREC.F6' trace flag is set)

This function checks trace flags every time it is called.
*************************************************************************/
void tracerowautoprec(/* Real    */ const ae_matrix* a,
     ae_int_t i,
     ae_int_t j0,
     ae_int_t j1,
     ae_state *_state);


/*************************************************************************
Unscales/unshifts vector A[N] by computing A*Scl+Sft and outputs result to
trace log using either:
a)  6-digit exponential format (no trace flags is set)
b) 15-ditit exponential format ('PREC.E15' trace flag is set)
b)  6-ditit fixed-point format ('PREC.F6' trace flag is set)

This function checks trace flags every time it is called.
Both Scl and Sft can be omitted.
*************************************************************************/
void tracevectorunscaledunshiftedautoprec(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* scl,
     ae_bool applyscl,
     /* Real    */ const ae_vector* sft,
     ae_bool applysft,
     ae_state *_state);


/*************************************************************************
Outputs vector of 1-norms of rows [I0,I1-1] of A[I0...I1-1,J0...J1-1]   to
trace log using either:
a)  6-digit exponential format (no trace flags is set)
b) 15-ditit exponential format ('PREC.E15' trace flag is set)
b)  6-ditit fixed-point format ('PREC.F6' trace flag is set)

This function checks trace flags every time it is called.
*************************************************************************/
void tracerownrm1autoprec(/* Real    */ const ae_matrix* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_int_t j0,
     ae_int_t j1,
     ae_state *_state);


/*************************************************************************
Outputs vector A[I0,I1-1] to trace log using E3 precision
*************************************************************************/
void tracevectore3(/* Real    */ const ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_state *_state);


/*************************************************************************
Outputs vector A[I0,I1-1] to trace log using E6 precision
*************************************************************************/
void tracevectore6(/* Real    */ const ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_state *_state);


/*************************************************************************
Outputs vector A[I0,I1-1] to trace log using E8 or E15 precision
*************************************************************************/
void tracevectore615(/* Real    */ const ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_bool usee15,
     ae_state *_state);


/*************************************************************************
Outputs vector of 1-norms of rows [I0,I1-1] of A[I0...I1-1,J0...J1-1]   to
trace log using E8 precision
*************************************************************************/
void tracerownrm1e6(/* Real    */ const ae_matrix* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_int_t j0,
     ae_int_t j1,
     ae_state *_state);


/*************************************************************************
Outputs specified number of spaces
*************************************************************************/
void tracespaces(ae_int_t cnt, ae_state *_state);


/*************************************************************************
Minimum speedup feasible for multithreading
*************************************************************************/
double minspeedup(ae_state *_state);


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Maximum concurrency on given system, with given compilation settings
*************************************************************************/
ae_int_t maxconcurrency(ae_state *_state);
#endif


/*************************************************************************
Initialize SAvgCounter

Prior value is a value that is returned when no values are in the buffer
*************************************************************************/
void savgcounterinit(savgcounter* c, double priorvalue, ae_state *_state);


/*************************************************************************
Enqueue value into SAvgCounter
*************************************************************************/
void savgcounterenqueue(savgcounter* c, double v, ae_state *_state);


/*************************************************************************
Enqueue value into SAvgCounter
*************************************************************************/
double savgcounterget(const savgcounter* c, ae_state *_state);


/*************************************************************************
Initialize SQuantileCounter

Prior value is a value that is returned when no values are in the buffer
*************************************************************************/
void squantilecounterinit(squantilecounter* c,
     double priorvalue,
     ae_state *_state);


/*************************************************************************
Enqueue value into SQuantileCounter
*************************************************************************/
void squantilecounterenqueue(squantilecounter* c,
     double v,
     ae_state *_state);


/*************************************************************************
Get k-th quantile. Thread-unsafe, modifies internal structures.

0<=Q<=1.
*************************************************************************/
double squantilecounterget(squantilecounter* c,
     double q,
     ae_state *_state);
void _apbuffers_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _apbuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _apbuffers_clear(void* _p);
void _apbuffers_destroy(void* _p);
void _sboolean_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sboolean_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sboolean_clear(void* _p);
void _sboolean_destroy(void* _p);
void _sbooleanarray_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sbooleanarray_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sbooleanarray_clear(void* _p);
void _sbooleanarray_destroy(void* _p);
void _sinteger_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sinteger_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sinteger_clear(void* _p);
void _sinteger_destroy(void* _p);
void _sintegerarray_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sintegerarray_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sintegerarray_clear(void* _p);
void _sintegerarray_destroy(void* _p);
void _sreal_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sreal_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sreal_clear(void* _p);
void _sreal_destroy(void* _p);
void _srealarray_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _srealarray_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _srealarray_clear(void* _p);
void _srealarray_destroy(void* _p);
void _scomplex_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _scomplex_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _scomplex_clear(void* _p);
void _scomplex_destroy(void* _p);
void _scomplexarray_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _scomplexarray_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _scomplexarray_clear(void* _p);
void _scomplexarray_destroy(void* _p);
void _nbpool_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _nbpool_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _nbpool_clear(void* _p);
void _nbpool_destroy(void* _p);
void _nipool_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _nipool_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _nipool_clear(void* _p);
void _nipool_destroy(void* _p);
void _nrpool_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _nrpool_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _nrpool_clear(void* _p);
void _nrpool_destroy(void* _p);
void _savgcounter_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _savgcounter_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _savgcounter_clear(void* _p);
void _savgcounter_destroy(void* _p);
void _squantilecounter_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _squantilecounter_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _squantilecounter_clear(void* _p);
void _squantilecounter_destroy(void* _p);


/*$ End $*/
#endif

