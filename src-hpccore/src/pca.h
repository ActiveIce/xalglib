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

#ifndef _pca_h
#define _pca_h

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
#include "blas.h"
#include "rotations.h"
#include "bdsvd.h"
#include "svd.h"
#include "matgen.h"
#include "scodes.h"
#include "tsort.h"
#include "sparse.h"
#include "hsschur.h"
#include "basicstatops.h"
#include "evd.h"
#include "basestat.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Principal components analysis

This function builds orthogonal basis  where  first  axis  corresponds  to
direction with maximum variance, second axis  maximizes  variance  in  the
subspace orthogonal to first axis and so on.

This function builds FULL basis, i.e. returns N vectors  corresponding  to
ALL directions, no matter how informative. If you need  just a  few  (say,
10 or 50) of the most important directions, you may find it faster to  use
one of the reduced versions:
* pcatruncatedsubspace() - for subspace iteration based method

It should be noted that, unlike LDA, PCA does not use class labels.

INPUT PARAMETERS:
    X           -   dataset, array[NPoints,NVars].
                    matrix contains ONLY INDEPENDENT VARIABLES.
    NPoints     -   dataset size, NPoints>=0
    NVars       -   number of independent variables, NVars>=1

OUTPUT PARAMETERS:
    S2          -   array[NVars]. variance values corresponding
                    to basis vectors.
    V           -   array[NVars,NVars]
                    matrix, whose columns store basis vectors.

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
     Copyright 25.08.2008 by Bochkanov Sergey
*************************************************************************/
void pcabuildbasis(/* Real    */ const ae_matrix* x,
     ae_int_t npoints,
     ae_int_t nvars,
     /* Real    */ ae_vector* s2,
     /* Real    */ ae_matrix* v,
     ae_state *_state);


/*************************************************************************
Principal components analysis

This function performs truncated PCA, i.e. returns just a few most important
directions.

Internally it uses iterative eigensolver which is very efficient when only
a minor fraction of full basis is required. Thus, if you need full  basis,
it is better to use pcabuildbasis() function.

It should be noted that, unlike LDA, PCA does not use class labels.

INPUT PARAMETERS:
    X           -   dataset, array[0..NPoints-1,0..NVars-1].
                    matrix contains ONLY INDEPENDENT VARIABLES.
    NPoints     -   dataset size, NPoints>=0
    NVars       -   number of independent variables, NVars>=1
    NNeeded     -   number of requested components, in [1,NVars] range;
                    this function is efficient only for NNeeded<<NVars.
    Eps         -   desired  precision  of  vectors  returned;  underlying
                    solver will stop iterations as soon as absolute  error
                    in corresponding singular values  reduces  to  roughly
                    eps*MAX(lambda[]), with lambda[] being array of  eigen
                    values.
                    Zero value means that  algorithm  performs  number  of
                    iterations  specified  by  maxits  parameter,  without
                    paying attention to precision.
    MaxIts      -   number of iterations performed by  subspace  iteration
                    method. Zero value means that no  limit  on  iteration
                    count is placed (eps-based stopping condition is used).
                    

OUTPUT PARAMETERS:
    S2          -   array[NNeeded]. Variance values corresponding
                    to basis vectors.
    V           -   array[NVars,NNeeded]
                    matrix, whose columns store basis vectors.
                    
NOTE: passing eps=0 and maxits=0 results in small eps  being  selected  as
stopping condition. Exact value of automatically selected eps is  version-
-dependent.

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
     Copyright 10.01.2017 by Bochkanov Sergey
*************************************************************************/
void pcatruncatedsubspace(/* Real    */ const ae_matrix* x,
     ae_int_t npoints,
     ae_int_t nvars,
     ae_int_t nneeded,
     double eps,
     ae_int_t maxits,
     /* Real    */ ae_vector* s2,
     /* Real    */ ae_matrix* v,
     ae_state *_state);


/*************************************************************************
Sparse truncated principal components analysis

This function performs sparse truncated PCA, i.e. returns just a few  most
important principal components for a sparse input X.

Internally it uses iterative eigensolver which is very efficient when only
a minor fraction of full basis is required.

It should be noted that, unlike LDA, PCA does not use class labels.

INPUT PARAMETERS:
    X           -   sparse dataset, sparse  npoints*nvars  matrix.  It  is
                    recommended to use CRS sparse storage format;  non-CRS
                    input will be internally converted to CRS.
                    Matrix contains ONLY INDEPENDENT VARIABLES,  and  must
                    be EXACTLY npoints*nvars.
    NPoints     -   dataset size, NPoints>=0
    NVars       -   number of independent variables, NVars>=1
    NNeeded     -   number of requested components, in [1,NVars] range;
                    this function is efficient only for NNeeded<<NVars.
    Eps         -   desired  precision  of  vectors  returned;  underlying
                    solver will stop iterations as soon as absolute  error
                    in corresponding singular values  reduces  to  roughly
                    eps*MAX(lambda[]), with lambda[] being array of  eigen
                    values.
                    Zero value means that  algorithm  performs  number  of
                    iterations  specified  by  maxits  parameter,  without
                    paying attention to precision.
    MaxIts      -   number of iterations performed by  subspace  iteration
                    method. Zero value means that no  limit  on  iteration
                    count is placed (eps-based stopping condition is used).
                    

OUTPUT PARAMETERS:
    S2          -   array[NNeeded]. Variance values corresponding
                    to basis vectors.
    V           -   array[NVars,NNeeded]
                    matrix, whose columns store basis vectors.
                    
NOTE: passing eps=0 and maxits=0 results in small eps  being  selected  as
      a stopping condition. Exact value of automatically selected  eps  is
      version-dependent.

NOTE: zero  MaxIts  is  silently  replaced  by some reasonable value which
      prevents eternal loops (possible when inputs are degenerate and  too
      stringent stopping criteria are specified). In  current  version  it
      is 50+2*NVars.

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
     Copyright 10.01.2017 by Bochkanov Sergey
*************************************************************************/
void pcatruncatedsubspacesparse(const sparsematrix* x,
     ae_int_t npoints,
     ae_int_t nvars,
     ae_int_t nneeded,
     double eps,
     ae_int_t maxits,
     /* Real    */ ae_vector* s2,
     /* Real    */ ae_matrix* v,
     ae_state *_state);


/*$ End $*/
#endif

