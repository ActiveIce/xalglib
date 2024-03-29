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

#ifndef _conv_h
#define _conv_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ntheory.h"
#include "ftbase.h"
#include "ablasf.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "fft.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
1-dimensional complex convolution.

For given A/B returns conv(A,B) (non-circular). Subroutine can automatically
choose between three implementations: straightforward O(M*N)  formula  for
very small N (or M), overlap-add algorithm for  cases  where  max(M,N)  is
significantly larger than min(M,N), but O(M*N) algorithm is too slow,  and
general FFT-based formula for cases where two previous algorithms are  too
slow.

Algorithm has max(M,N)*log(max(M,N)) complexity for any M/N.

INPUT PARAMETERS
    A   -   array[M] - complex function to be transformed
    M   -   problem size
    B   -   array[N] - complex function to be transformed
    N   -   problem size

OUTPUT PARAMETERS
    R   -   convolution: A*B. array[N+M-1]

NOTE:
    It is assumed that A is zero at T<0, B is zero too.  If  one  or  both
    functions have non-zero values at negative T's, you can still use this
    subroutine - just shift its result correspondingly.
    
NOTE: there is a buffered version of this  function,  ConvC1DBuf(),  which
      can reuse space previously allocated in its output parameter R.

  -- ALGLIB --
     Copyright 21.07.2009 by Bochkanov Sergey
*************************************************************************/
void convc1d(/* Complex */ const ae_vector* a,
     ae_int_t m,
     /* Complex */ const ae_vector* b,
     ae_int_t n,
     /* Complex */ ae_vector* r,
     ae_state *_state);


/*************************************************************************
1-dimensional complex convolution, buffered version of ConvC1DBuf(), which
does not reallocate R[] if its length is enough to store the result  (i.e.
it reuses previously allocated memory as much as possible).

  -- ALGLIB --
     Copyright 30.11.2023 by Bochkanov Sergey
*************************************************************************/
void convc1dbuf(/* Complex */ const ae_vector* a,
     ae_int_t m,
     /* Complex */ const ae_vector* b,
     ae_int_t n,
     /* Complex */ ae_vector* r,
     ae_state *_state);


/*************************************************************************
1-dimensional complex non-circular deconvolution (inverse of ConvC1D()).

Algorithm has M*log(M)) complexity for any M (composite or prime).

INPUT PARAMETERS
    A   -   array[0..M-1] - convolved signal, A = conv(R, B)
    M   -   convolved signal length
    B   -   array[0..N-1] - response
    N   -   response length, N<=M

OUTPUT PARAMETERS
    R   -   deconvolved signal. array[0..M-N].

NOTE:
    deconvolution is unstable process and may result in division  by  zero
(if your response function is degenerate, i.e. has zero Fourier coefficient).

NOTE:
    It is assumed that A is zero at T<0, B is zero too.  If  one  or  both
functions have non-zero values at negative T's, you  can  still  use  this
subroutine - just shift its result correspondingly.
    
NOTE: there is a buffered version of this  function,  ConvC1DInvBuf(),
      which can reuse space previously allocated in its output parameter R

  -- ALGLIB --
     Copyright 21.07.2009 by Bochkanov Sergey
*************************************************************************/
void convc1dinv(/* Complex */ const ae_vector* a,
     ae_int_t m,
     /* Complex */ const ae_vector* b,
     ae_int_t n,
     /* Complex */ ae_vector* r,
     ae_state *_state);


/*************************************************************************
1-dimensional complex non-circular deconvolution (inverse of ConvC1D()).

A buffered version, which does not reallocate R[] if its length is  enough
to store the result (i.e. it reuses previously allocated memory as much as
possible).

  -- ALGLIB --
     Copyright 30.11.2023 by Bochkanov Sergey
*************************************************************************/
void convc1dinvbuf(/* Complex */ const ae_vector* a,
     ae_int_t m,
     /* Complex */ const ae_vector* b,
     ae_int_t n,
     /* Complex */ ae_vector* r,
     ae_state *_state);


/*************************************************************************
1-dimensional circular complex convolution.

For given S/R returns conv(S,R) (circular). Algorithm has linearithmic
complexity for any M/N.

IMPORTANT:  normal convolution is commutative,  i.e.   it  is symmetric  -
conv(A,B)=conv(B,A).  Cyclic convolution IS NOT.  One function - S - is  a
signal,  periodic function, and another - R - is a response,  non-periodic
function with limited length.

INPUT PARAMETERS
    S   -   array[M] - complex periodic signal
    M   -   problem size
    B   -   array[N] - complex non-periodic response
    N   -   problem size

OUTPUT PARAMETERS
    R   -   convolution: A*B. array[M].

NOTE:
    It is assumed that B is zero at T<0. If  it  has  non-zero  values  at
negative T's, you can still use this subroutine - just  shift  its  result
correspondingly.
    
NOTE: there is a buffered version of this  function,  ConvC1DCircularBuf(),
      which can reuse space previously allocated in its output parameter R.

  -- ALGLIB --
     Copyright 21.07.2009 by Bochkanov Sergey
*************************************************************************/
void convc1dcircular(/* Complex */ const ae_vector* s,
     ae_int_t m,
     /* Complex */ const ae_vector* r,
     ae_int_t n,
     /* Complex */ ae_vector* c,
     ae_state *_state);


/*************************************************************************
1-dimensional circular complex convolution.

Buffered version of ConvC1DCircular(), which does not  reallocate  C[]  if
its length is enough to  store  the  result  (i.e.  it  reuses  previously
allocated memory as much as possible).

  -- ALGLIB --
     Copyright 30.11.2023 by Bochkanov Sergey
*************************************************************************/
void convc1dcircularbuf(/* Complex */ const ae_vector* s,
     ae_int_t m,
     /* Complex */ const ae_vector* r,
     ae_int_t n,
     /* Complex */ ae_vector* c,
     ae_state *_state);


/*************************************************************************
1-dimensional circular complex deconvolution (inverse of ConvC1DCircular()).

Algorithm has M*log(M)) complexity for any M (composite or prime).

INPUT PARAMETERS
    A   -   array[0..M-1] - convolved periodic signal, A = conv(R, B)
    M   -   convolved signal length
    B   -   array[0..N-1] - non-periodic response
    N   -   response length

OUTPUT PARAMETERS
    R   -   deconvolved signal. array[0..M-1].

NOTE:
    deconvolution is unstable process and may result in division  by  zero
(if your response function is degenerate, i.e. has zero Fourier coefficient).

NOTE:
    It is assumed that B is zero at T<0. If  it  has  non-zero  values  at
negative T's, you can still use this subroutine - just  shift  its  result
correspondingly.
    
NOTE: there is a buffered version of this  function,  ConvC1DCircularInvBuf(),
      which can reuse space previously allocated in its output parameter R.

  -- ALGLIB --
     Copyright 21.07.2009 by Bochkanov Sergey
*************************************************************************/
void convc1dcircularinv(/* Complex */ const ae_vector* a,
     ae_int_t m,
     /* Complex */ const ae_vector* b,
     ae_int_t n,
     /* Complex */ ae_vector* r,
     ae_state *_state);


/*************************************************************************
1-dimensional circular complex deconvolution (inverse of ConvC1DCircular()).

Buffered version of ConvC1DCircularInv(), which does not reallocate R[] if
its length is enough to  store  the  result  (i.e.  it  reuses  previously
allocated memory as much as possible).

  -- ALGLIB --
     Copyright 30.11.2023 by Bochkanov Sergey
*************************************************************************/
void convc1dcircularinvbuf(/* Complex */ const ae_vector* a,
     ae_int_t m,
     /* Complex */ const ae_vector* b,
     ae_int_t n,
     /* Complex */ ae_vector* r,
     ae_state *_state);


/*************************************************************************
1-dimensional real convolution.

Analogous to ConvC1D(), see ConvC1D() comments for more details.

INPUT PARAMETERS
    A   -   array[0..M-1] - real function to be transformed
    M   -   problem size
    B   -   array[0..N-1] - real function to be transformed
    N   -   problem size

OUTPUT PARAMETERS
    R   -   convolution: A*B. array[0..N+M-2].

NOTE:
    It is assumed that A is zero at T<0, B is zero too.  If  one  or  both
functions have non-zero values at negative T's, you  can  still  use  this
subroutine - just shift its result correspondingly.
    
NOTE: there is a buffered version of this  function,  ConvR1DBuf(),
      which can reuse space previously allocated in its output parameter R.


  -- ALGLIB --
     Copyright 21.07.2009 by Bochkanov Sergey
*************************************************************************/
void convr1d(/* Real    */ const ae_vector* a,
     ae_int_t m,
     /* Real    */ const ae_vector* b,
     ae_int_t n,
     /* Real    */ ae_vector* r,
     ae_state *_state);


/*************************************************************************
1-dimensional real convolution.

Buffered version of ConvR1D(), which does not reallocate R[] if its length
is enough to store the result (i.e. it reuses previously allocated  memory
as much as possible).

  -- ALGLIB --
     Copyright 30.11.2023 by Bochkanov Sergey
*************************************************************************/
void convr1dbuf(/* Real    */ const ae_vector* a,
     ae_int_t m,
     /* Real    */ const ae_vector* b,
     ae_int_t n,
     /* Real    */ ae_vector* r,
     ae_state *_state);


/*************************************************************************
1-dimensional real deconvolution (inverse of ConvC1D()).

Algorithm has M*log(M)) complexity for any M (composite or prime).

INPUT PARAMETERS
    A   -   array[0..M-1] - convolved signal, A = conv(R, B)
    M   -   convolved signal length
    B   -   array[0..N-1] - response
    N   -   response length, N<=M

OUTPUT PARAMETERS
    R   -   deconvolved signal. array[0..M-N].

NOTE:
    deconvolution is unstable process and may result in division  by  zero
(if your response function is degenerate, i.e. has zero Fourier coefficient).

NOTE:
    It is assumed that A is zero at T<0, B is zero too.  If  one  or  both
functions have non-zero values at negative T's, you  can  still  use  this
subroutine - just shift its result correspondingly.
    
NOTE: there is a buffered version of this  function,  ConvR1DInvBuf(),
      which can reuse space previously allocated in its output parameter R.

  -- ALGLIB --
     Copyright 21.07.2009 by Bochkanov Sergey
*************************************************************************/
void convr1dinv(/* Real    */ const ae_vector* a,
     ae_int_t m,
     /* Real    */ const ae_vector* b,
     ae_int_t n,
     /* Real    */ ae_vector* r,
     ae_state *_state);


/*************************************************************************
1-dimensional real deconvolution (inverse of ConvR1D()), buffered version,
which does not reallocate R[] if its length is enough to store the  result
(i.e. it reuses previously allocated memory as much as possible).


  -- ALGLIB --
     Copyright 30.11.2023 by Bochkanov Sergey
*************************************************************************/
void convr1dinvbuf(/* Real    */ const ae_vector* a,
     ae_int_t m,
     /* Real    */ const ae_vector* b,
     ae_int_t n,
     /* Real    */ ae_vector* r,
     ae_state *_state);


/*************************************************************************
1-dimensional circular real convolution.

Analogous to ConvC1DCircular(), see ConvC1DCircular() comments for more details.

INPUT PARAMETERS
    S   -   array[0..M-1] - real signal
    M   -   problem size
    B   -   array[0..N-1] - real response
    N   -   problem size

OUTPUT PARAMETERS
    R   -   convolution: A*B. array[0..M-1].

NOTE:
    It is assumed that B is zero at T<0. If  it  has  non-zero  values  at
negative T's, you can still use this subroutine - just  shift  its  result
correspondingly.
    
NOTE: there is a buffered version of this  function,  ConvR1DCurcularBuf(),
      which can reuse space previously allocated in its output parameter R.

  -- ALGLIB --
     Copyright 21.07.2009 by Bochkanov Sergey
*************************************************************************/
void convr1dcircular(/* Real    */ const ae_vector* s,
     ae_int_t m,
     /* Real    */ const ae_vector* r,
     ae_int_t n,
     /* Real    */ ae_vector* c,
     ae_state *_state);


/*************************************************************************
1-dimensional circular real convolution, buffered version, which  does not
reallocate C[] if its length is enough to store the result (i.e. it reuses
previously allocated memory as much as possible).

  -- ALGLIB --
     Copyright 30.11.2023 by Bochkanov Sergey
*************************************************************************/
void convr1dcircularbuf(/* Real    */ const ae_vector* s,
     ae_int_t m,
     /* Real    */ const ae_vector* r,
     ae_int_t n,
     /* Real    */ ae_vector* c,
     ae_state *_state);


/*************************************************************************
1-dimensional complex deconvolution (inverse of ConvC1D()).

Algorithm has M*log(M)) complexity for any M (composite or prime).

INPUT PARAMETERS
    A   -   array[0..M-1] - convolved signal, A = conv(R, B)
    M   -   convolved signal length
    B   -   array[0..N-1] - response
    N   -   response length

OUTPUT PARAMETERS
    R   -   deconvolved signal. array[0..M-N].

NOTE:
    deconvolution is unstable process and may result in division  by  zero
(if your response function is degenerate, i.e. has zero Fourier coefficient).

NOTE:
    It is assumed that B is zero at T<0. If  it  has  non-zero  values  at
negative T's, you can still use this subroutine - just  shift  its  result
correspondingly.

  -- ALGLIB --
     Copyright 21.07.2009 by Bochkanov Sergey
*************************************************************************/
void convr1dcircularinv(/* Real    */ const ae_vector* a,
     ae_int_t m,
     /* Real    */ const ae_vector* b,
     ae_int_t n,
     /* Real    */ ae_vector* r,
     ae_state *_state);


/*************************************************************************
1-dimensional complex deconvolution, inverse of ConvR1DCircular().

Buffered version, which does not reallocate R[] if its length is enough to
store the result (i.e. it reuses previously allocated memory  as  much  as
possible).

  -- ALGLIB --
     Copyright 21.07.2009 by Bochkanov Sergey
*************************************************************************/
void convr1dcircularinvbuf(/* Real    */ const ae_vector* a,
     ae_int_t m,
     /* Real    */ const ae_vector* b,
     ae_int_t n,
     /* Real    */ ae_vector* r,
     ae_state *_state);


/*************************************************************************
1-dimensional complex convolution.

Extended subroutine which allows to choose convolution algorithm.
Intended for internal use, ALGLIB users should call ConvC1D()/ConvC1DCircular().

INPUT PARAMETERS
    A   -   array[0..M-1] - complex function to be transformed
    M   -   problem size
    B   -   array[0..N-1] - complex function to be transformed
    N   -   problem size, N<=M
    Alg -   algorithm type:
            *-2     auto-select Q for overlap-add
            *-1     auto-select algorithm and parameters
            * 0     straightforward formula for small N's
            * 1     general FFT-based code
            * 2     overlap-add with length Q
    Q   -   length for overlap-add

OUTPUT PARAMETERS
    R   -   convolution: A*B. array[0..N+M-1].

  -- ALGLIB --
     Copyright 21.07.2009 by Bochkanov Sergey
*************************************************************************/
void convc1dx(/* Complex */ const ae_vector* a,
     ae_int_t m,
     /* Complex */ const ae_vector* b,
     ae_int_t n,
     ae_bool circular,
     ae_int_t alg,
     ae_int_t q,
     /* Complex */ ae_vector* r,
     ae_state *_state);


/*************************************************************************
1-dimensional real convolution.

Extended subroutine which allows to choose convolution algorithm.
Intended for internal use, ALGLIB users should call ConvR1D().

INPUT PARAMETERS
    A   -   array[0..M-1] - complex function to be transformed
    M   -   problem size
    B   -   array[0..N-1] - complex function to be transformed
    N   -   problem size, N<=M
    Alg -   algorithm type:
            *-2     auto-select Q for overlap-add
            *-1     auto-select algorithm and parameters
            * 0     straightforward formula for small N's
            * 1     general FFT-based code
            * 2     overlap-add with length Q
    Q   -   length for overlap-add

OUTPUT PARAMETERS
    R   -   convolution: A*B. array[0..N+M-1].

  -- ALGLIB --
     Copyright 21.07.2009 by Bochkanov Sergey
*************************************************************************/
void convr1dx(/* Real    */ const ae_vector* a,
     ae_int_t m,
     /* Real    */ const ae_vector* b,
     ae_int_t n,
     ae_bool circular,
     ae_int_t alg,
     ae_int_t q,
     /* Real    */ ae_vector* r,
     ae_state *_state);


/*$ End $*/
#endif

