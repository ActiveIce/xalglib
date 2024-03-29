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

#ifndef _fft_h
#define _fft_h

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


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
1-dimensional complex FFT.

Array size N may be arbitrary number (composite or prime).  Composite  N's
are handled with cache-oblivious variation of  a  Cooley-Tukey  algorithm.
Small prime-factors are transformed using hard coded  codelets (similar to
FFTW codelets, but without low-level  optimization),  large  prime-factors
are handled with Bluestein's algorithm.

Fastests transforms are for smooth N's (prime factors are 2, 3,  5  only),
most fast for powers of 2. When N have prime factors  larger  than  these,
but orders of magnitude smaller than N, computations will be about 4 times
slower than for nearby highly composite N's. When N itself is prime, speed
will be 6 times lower.

Algorithm has O(N*logN) complexity for any N (composite or prime).

INPUT PARAMETERS
    A   -   array[0..N-1] - complex function to be transformed
    N   -   problem size
    
OUTPUT PARAMETERS
    A   -   DFT of a input array, array[0..N-1]
            A_out[j] = SUM(A_in[k]*exp(-2*pi*sqrt(-1)*j*k/N), k = 0..N-1)


  -- ALGLIB --
     Copyright 29.05.2009 by Bochkanov Sergey
*************************************************************************/
void fftc1d(/* Complex */ ae_vector* a, ae_int_t n, ae_state *_state);


/*************************************************************************
1-dimensional complex inverse FFT.

Array size N may be arbitrary number (composite or prime).  Algorithm  has
O(N*logN) complexity for any N (composite or prime).

See FFTC1D() description for more information about algorithm performance.

INPUT PARAMETERS
    A   -   array[0..N-1] - complex array to be transformed
    N   -   problem size

OUTPUT PARAMETERS
    A   -   inverse DFT of a input array, array[0..N-1]
            A_out[j] = SUM(A_in[k]/N*exp(+2*pi*sqrt(-1)*j*k/N), k = 0..N-1)


  -- ALGLIB --
     Copyright 29.05.2009 by Bochkanov Sergey
*************************************************************************/
void fftc1dinv(/* Complex */ ae_vector* a, ae_int_t n, ae_state *_state);


/*************************************************************************
1-dimensional real FFT.

Algorithm has O(N*logN) complexity for any N (composite or prime).

INPUT PARAMETERS
    A   -   array[0..N-1] - real function to be transformed
    N   -   problem size

OUTPUT PARAMETERS
    F   -   DFT of a input array, array[0..N-1]
            F[j] = SUM(A[k]*exp(-2*pi*sqrt(-1)*j*k/N), k = 0..N-1)

NOTE: there is a buffered version  of  this  function, FFTR1DBuf(),  which
      reuses memory previously allocated for A as much as possible.

NOTE:
    F[] satisfies symmetry property F[k] = conj(F[N-k]),  so just one half
of  array  is  usually needed. But for convinience subroutine returns full
complex array (with frequencies above N/2), so its result may be  used  by
other FFT-related subroutines.


  -- ALGLIB --
     Copyright 01.06.2009 by Bochkanov Sergey
*************************************************************************/
void fftr1d(/* Real    */ const ae_vector* a,
     ae_int_t n,
     /* Complex */ ae_vector* f,
     ae_state *_state);


/*************************************************************************
1-dimensional real FFT, a buffered function which does not reallocate  F[]
if its length is enough to store the result  (i.e.  it  reuses  previously
allocated memory as much as possible).

  -- ALGLIB --
     Copyright 01.06.2009 by Bochkanov Sergey
*************************************************************************/
void fftr1dbuf(/* Real    */ const ae_vector* a,
     ae_int_t n,
     /* Complex */ ae_vector* f,
     ae_state *_state);


/*************************************************************************
1-dimensional real inverse FFT.

Algorithm has O(N*logN) complexity for any N (composite or prime).

INPUT PARAMETERS
    F   -   array[0..floor(N/2)] - frequencies from forward real FFT
    N   -   problem size

OUTPUT PARAMETERS
    A   -   inverse DFT of a input array, array[0..N-1]

NOTE: there is a buffered version of this function, FFTR1DInvBuf(),  which
      reuses memory previously allocated for A as much as possible.

NOTE:
    F[] should satisfy symmetry property F[k] = conj(F[N-k]), so just  one
half of frequencies array is needed - elements from 0 to floor(N/2).  F[0]
is ALWAYS real. If N is even F[floor(N/2)] is real too. If N is odd,  then
F[floor(N/2)] has no special properties.

Relying on properties noted above, FFTR1DInv subroutine uses only elements
from 0th to floor(N/2)-th. It ignores imaginary part of F[0],  and in case
N is even it ignores imaginary part of F[floor(N/2)] too.

When you call this function using full arguments list - "FFTR1DInv(F,N,A)"
- you can pass either either frequencies array with N elements or  reduced
array with roughly N/2 elements - subroutine will  successfully  transform
both.

If you call this function using reduced arguments list -  "FFTR1DInv(F,A)"
- you must pass FULL array with N elements (although higher  N/2 are still
not used) because array size is used to automatically determine FFT length

  -- ALGLIB --
     Copyright 01.06.2009 by Bochkanov Sergey
*************************************************************************/
void fftr1dinv(/* Complex */ const ae_vector* f,
     ae_int_t n,
     /* Real    */ ae_vector* a,
     ae_state *_state);


/*************************************************************************
1-dimensional real inverse FFT, buffered version, which does not reallocate
A[] if its length is enough to store the result (i.e. it reuses previously
allocated memory as much as possible).

  -- ALGLIB --
     Copyright 01.06.2009 by Bochkanov Sergey
*************************************************************************/
void fftr1dinvbuf(/* Complex */ const ae_vector* f,
     ae_int_t n,
     /* Real    */ ae_vector* a,
     ae_state *_state);


/*************************************************************************
Internal subroutine. Never call it directly!


  -- ALGLIB --
     Copyright 01.06.2009 by Bochkanov Sergey
*************************************************************************/
void fftr1dinternaleven(/* Real    */ ae_vector* a,
     ae_int_t n,
     /* Real    */ ae_vector* buf,
     fasttransformplan* plan,
     ae_state *_state);


/*************************************************************************
Internal subroutine. Never call it directly!


  -- ALGLIB --
     Copyright 01.06.2009 by Bochkanov Sergey
*************************************************************************/
void fftr1dinvinternaleven(/* Real    */ ae_vector* a,
     ae_int_t n,
     /* Real    */ ae_vector* buf,
     fasttransformplan* plan,
     ae_state *_state);


/*$ End $*/
#endif

