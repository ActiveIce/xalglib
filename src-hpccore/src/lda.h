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

#ifndef _lda_h
#define _lda_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "hblas.h"
#include "creflections.h"
#include "sblas.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "ortfac.h"
#include "matgen.h"
#include "scodes.h"
#include "tsort.h"
#include "sparse.h"
#include "blas.h"
#include "rotations.h"
#include "hsschur.h"
#include "basicstatops.h"
#include "evd.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Multiclass Fisher LDA

The function finds coefficients of a linear  combination  which  optimally
separates training set. Most suited for 2-class problems, see fisherldan()
for an variant that returns N-dimensional basis.

INPUT PARAMETERS:
    XY          -   training set, array[NPoints,NVars+1].
                    First NVars columns store values of independent
                    variables, the next column stores class index (from 0
                    to NClasses-1) which dataset element belongs to.
                    Fractional values are rounded to the nearest integer.
                    The class index must be in the [0,NClasses-1] range,
                    an exception is generated otherwise.
    NPoints     -   training set size, NPoints>=0
    NVars       -   number of independent variables, NVars>=1
    NClasses    -   number of classes, NClasses>=2


OUTPUT PARAMETERS:
    W           -   linear combination coefficients, array[NVars]

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

  -- ALGLIB --
     Copyright 31.05.2008 by Bochkanov Sergey
*************************************************************************/
void fisherlda(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nvars,
     ae_int_t nclasses,
     /* Real    */ ae_vector* w,
     ae_state *_state);


/*************************************************************************
N-dimensional multiclass Fisher LDA

Subroutine finds coefficients of linear combinations which optimally separates
training set on classes. It returns N-dimensional basis whose vector are sorted
by quality of training set separation (in descending order).

INPUT PARAMETERS:
    XY          -   training set, array[NPoints,NVars+1].
                    First NVars columns store values of independent
                    variables, the next column stores class index (from 0
                    to NClasses-1) which dataset element belongs to.
                    Fractional values are rounded to the nearest integer.
                    The class index must be in the [0,NClasses-1] range,
                    an exception is generated otherwise.
    NPoints     -   training set size, NPoints>=0
    NVars       -   number of independent variables, NVars>=1
    NClasses    -   number of classes, NClasses>=2


OUTPUT PARAMETERS:
    W           -   basis, array[NVars,NVars]
                    columns of matrix stores basis vectors, sorted by
                    quality of training set separation (in descending order)

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

  -- ALGLIB --
     Copyright 31.05.2008 by Bochkanov Sergey
*************************************************************************/
void fisherldan(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nvars,
     ae_int_t nclasses,
     /* Real    */ ae_matrix* w,
     ae_state *_state);


/*$ End $*/
#endif

