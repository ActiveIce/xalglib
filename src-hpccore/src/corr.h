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

#ifndef _corr_h
#define _corr_h

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
#include "conv.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
1-dimensional complex cross-correlation.

For given Pattern/Signal returns corr(Pattern,Signal) (non-circular).

Correlation is calculated using reduction to  convolution.  Algorithm with
max(N,N)*log(max(N,N)) complexity is used (see  ConvC1D()  for  more  info
about performance).

IMPORTANT:
    for  historical reasons subroutine accepts its parameters in  reversed
    order: CorrC1D(Signal, Pattern) = Pattern x Signal (using  traditional
    definition of cross-correlation, denoting cross-correlation as "x").

INPUT PARAMETERS
    Signal  -   array[0..N-1] - complex function to be transformed,
                signal containing pattern
    N       -   problem size
    Pattern -   array[0..M-1] - complex function to be transformed,
                pattern to 'search' within a signal
    M       -   problem size

OUTPUT PARAMETERS
    R       -   cross-correlation, array[0..N+M-2]:
                * positive lags are stored in R[0..N-1],
                  R[i] = sum(conj(pattern[j])*signal[i+j]
                * negative lags are stored in R[N..N+M-2],
                  R[N+M-1-i] = sum(conj(pattern[j])*signal[-i+j]

NOTE:
    It is assumed that pattern domain is [0..M-1].  If Pattern is non-zero
on [-K..M-1],  you can still use this subroutine, just shift result by K.
    
NOTE: there is a buffered version of this  function,  CorrC1DBuf(), which
     can reuse space previously allocated in its output parameter R.

  -- ALGLIB --
     Copyright 21.07.2009 by Bochkanov Sergey
*************************************************************************/
void corrc1d(/* Complex */ const ae_vector* signal,
     ae_int_t n,
     /* Complex */ const ae_vector* pattern,
     ae_int_t m,
     /* Complex */ ae_vector* r,
     ae_state *_state);


/*************************************************************************
1-dimensional complex cross-correlation, a buffered version  of  CorrC1D()
which does not reallocate R[] if its length is enough to store the  result
(i.e. it reuses previously allocated memory as much as possible).

  -- ALGLIB --
     Copyright 21.07.2009 by Bochkanov Sergey
*************************************************************************/
void corrc1dbuf(/* Complex */ const ae_vector* signal,
     ae_int_t n,
     /* Complex */ const ae_vector* pattern,
     ae_int_t m,
     /* Complex */ ae_vector* r,
     ae_state *_state);


/*************************************************************************
1-dimensional circular complex cross-correlation.

For given Pattern/Signal returns corr(Pattern,Signal) (circular).
Algorithm has linearithmic complexity for any M/N.

IMPORTANT:
    for  historical reasons subroutine accepts its parameters in  reversed
    order:   CorrC1DCircular(Signal, Pattern) = Pattern x Signal    (using
    traditional definition of cross-correlation, denoting cross-correlation
    as "x").

INPUT PARAMETERS
    Signal  -   array[0..N-1] - complex function to be transformed,
                periodic signal containing pattern
    N       -   problem size
    Pattern -   array[0..M-1] - complex function to be transformed,
                non-periodic pattern to 'search' within a signal
    M       -   problem size

OUTPUT PARAMETERS
    R   -   convolution: A*B. array[0..M-1].
    
NOTE: there is a buffered version of this  function,  CorrC1DCircular(),
      which can reuse space previously allocated in its output parameter R.

  -- ALGLIB --
     Copyright 21.07.2009 by Bochkanov Sergey
*************************************************************************/
void corrc1dcircular(/* Complex */ const ae_vector* signal,
     ae_int_t m,
     /* Complex */ const ae_vector* pattern,
     ae_int_t n,
     /* Complex */ ae_vector* c,
     ae_state *_state);


/*************************************************************************
1-dimensional circular complex cross-correlation.

A buffered function which does not reallocate C[] if its length is  enough
to store the result (i.e. it reuses previously allocated memory as much as
possible).

  -- ALGLIB --
     Copyright 21.07.2009 by Bochkanov Sergey
*************************************************************************/
void corrc1dcircularbuf(/* Complex */ const ae_vector* signal,
     ae_int_t m,
     /* Complex */ const ae_vector* pattern,
     ae_int_t n,
     /* Complex */ ae_vector* c,
     ae_state *_state);


/*************************************************************************
1-dimensional real cross-correlation.

For given Pattern/Signal returns corr(Pattern,Signal) (non-circular).

Correlation is calculated using reduction to  convolution.  Algorithm with
max(N,N)*log(max(N,N)) complexity is used (see  ConvC1D()  for  more  info
about performance).

IMPORTANT:
    for  historical reasons subroutine accepts its parameters in  reversed
    order: CorrR1D(Signal, Pattern) = Pattern x Signal (using  traditional
    definition of cross-correlation, denoting cross-correlation as "x").

INPUT PARAMETERS
    Signal  -   array[0..N-1] - real function to be transformed,
                signal containing pattern
    N       -   problem size
    Pattern -   array[0..M-1] - real function to be transformed,
                pattern to 'search' withing signal
    M       -   problem size

OUTPUT PARAMETERS
    R       -   cross-correlation, array[0..N+M-2]:
                * positive lags are stored in R[0..N-1],
                  R[i] = sum(pattern[j]*signal[i+j]
                * negative lags are stored in R[N..N+M-2],
                  R[N+M-1-i] = sum(pattern[j]*signal[-i+j]

NOTE:
    It is assumed that pattern domain is [0..M-1].  If Pattern is non-zero
on [-K..M-1],  you can still use this subroutine, just shift result by K.

NOTE: there is a buffered version of this  function,  CorrR1DBuf(),  which
      can reuse space previously allocated in its output parameter R.

  -- ALGLIB --
     Copyright 21.07.2009 by Bochkanov Sergey
*************************************************************************/
void corrr1d(/* Real    */ const ae_vector* signal,
     ae_int_t n,
     /* Real    */ const ae_vector* pattern,
     ae_int_t m,
     /* Real    */ ae_vector* r,
     ae_state *_state);


/*************************************************************************
1-dimensional real cross-correlation, buffered function,  which  does  not
reallocate R[] if its length is enough to store the result (i.e. it reuses
previously allocated memory as much as possible).

  -- ALGLIB --
     Copyright 21.07.2009 by Bochkanov Sergey
*************************************************************************/
void corrr1dbuf(/* Real    */ const ae_vector* signal,
     ae_int_t n,
     /* Real    */ const ae_vector* pattern,
     ae_int_t m,
     /* Real    */ ae_vector* r,
     ae_state *_state);


/*************************************************************************
1-dimensional circular real cross-correlation.

For given Pattern/Signal returns corr(Pattern,Signal) (circular).
Algorithm has linearithmic complexity for any M/N.

IMPORTANT:
    for  historical reasons subroutine accepts its parameters in  reversed
    order:   CorrR1DCircular(Signal, Pattern) = Pattern x Signal    (using
    traditional definition of cross-correlation, denoting cross-correlation
    as "x").

INPUT PARAMETERS
    Signal  -   array[0..N-1] - real function to be transformed,
                periodic signal containing pattern
    N       -   problem size
    Pattern -   array[0..M-1] - real function to be transformed,
                non-periodic pattern to search withing signal
    M       -   problem size

OUTPUT PARAMETERS
    R   -   convolution: A*B. array[0..M-1].

NOTE: there is a buffered version of this  function,  CorrR1DCircularBuf(),
      which can reuse space previously allocated in its output parameter C.

  -- ALGLIB --
     Copyright 21.07.2009 by Bochkanov Sergey
*************************************************************************/
void corrr1dcircular(/* Real    */ const ae_vector* signal,
     ae_int_t m,
     /* Real    */ const ae_vector* pattern,
     ae_int_t n,
     /* Real    */ ae_vector* c,
     ae_state *_state);


/*************************************************************************
1-dimensional circular real cross-correlation,  buffered  version ,  which
does not reallocate C[] if its length is enough to store the result  (i.e.
it reuses previously allocated memory as much as possible).

  -- ALGLIB --
     Copyright 21.07.2009 by Bochkanov Sergey
*************************************************************************/
void corrr1dcircularbuf(/* Real    */ const ae_vector* signal,
     ae_int_t m,
     /* Real    */ const ae_vector* pattern,
     ae_int_t n,
     /* Real    */ ae_vector* c,
     ae_state *_state);


/*$ End $*/
#endif

