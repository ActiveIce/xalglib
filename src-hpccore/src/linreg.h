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

#ifndef _linreg_h
#define _linreg_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "tsort.h"
#include "basicstatops.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "basestat.h"
#include "gammafunc.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "normaldistr.h"
#include "igammaf.h"
#include "hblas.h"
#include "creflections.h"
#include "sblas.h"
#include "ortfac.h"
#include "blas.h"
#include "rotations.h"
#include "bdsvd.h"
#include "svd.h"


/*$ Declarations $*/


typedef struct
{
    ae_vector w;
} linearmodel;


/*************************************************************************
LRReport structure contains additional information about linear model:
* C             -   covariation matrix,  array[0..NVars,0..NVars].
                    C[i,j] = Cov(A[i],A[j])
* RMSError      -   root mean square error on a training set
* AvgError      -   average error on a training set
* AvgRelError   -   average relative error on a training set (excluding
                    observations with zero function value).
* CVRMSError    -   leave-one-out cross-validation estimate of
                    generalization error. Calculated using fast algorithm
                    with O(NVars*NPoints) complexity.
* CVAvgError    -   cross-validation estimate of average error
* CVAvgRelError -   cross-validation estimate of average relative error

All other fields of the structure are intended for internal use and should
not be used outside ALGLIB.
*************************************************************************/
typedef struct
{
    ae_matrix c;
    double rmserror;
    double avgerror;
    double avgrelerror;
    double cvrmserror;
    double cvavgerror;
    double cvavgrelerror;
    ae_int_t ncvdefects;
    ae_vector cvdefects;
} lrreport;


/*$ Body $*/


/*************************************************************************
Linear regression

Subroutine builds model:

    Y = A(0)*X[0] + ... + A(N-1)*X[N-1] + A(N)

and model found in ALGLIB format, covariation matrix, training set  errors
(rms,  average,  average  relative)   and  leave-one-out  cross-validation
estimate of the generalization error. CV  estimate calculated  using  fast
algorithm with O(NPoints*NVars) complexity.

When  covariation  matrix  is  calculated  standard deviations of function
values are assumed to be equal to RMS error on the training set.

INPUT PARAMETERS:
    XY          -   training set, array [0..NPoints-1,0..NVars]:
                    * NVars columns - independent variables
                    * last column - dependent variable
    NPoints     -   training set size, NPoints>NVars+1. An exception is
                    generated otherwise.
    NVars       -   number of independent variables

OUTPUT PARAMETERS:
    LM          -   linear model in the ALGLIB format. Use subroutines of
                    this unit to work with the model.
    Rep         -   additional results, see comments on LRReport structure.

  -- ALGLIB --
     Copyright 02.08.2008 by Bochkanov Sergey
*************************************************************************/
void lrbuild(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nvars,
     linearmodel* lm,
     lrreport* rep,
     ae_state *_state);


/*************************************************************************
Linear regression

Variant of LRBuild which uses vector of standatd deviations (errors in
function values).

INPUT PARAMETERS:
    XY          -   training set, array [0..NPoints-1,0..NVars]:
                    * NVars columns - independent variables
                    * last column - dependent variable
    S           -   standard deviations (errors in function values)
                    array[NPoints], S[i]>0.
    NPoints     -   training set size, NPoints>NVars+1
    NVars       -   number of independent variables

OUTPUT PARAMETERS:
    LM          -   linear model in the ALGLIB format. Use subroutines of
                    this unit to work with the model.
    Rep         -   additional results, see comments on LRReport structure.

  -- ALGLIB --
     Copyright 02.08.2008 by Bochkanov Sergey
*************************************************************************/
void lrbuilds(/* Real    */ const ae_matrix* xy,
     /* Real    */ const ae_vector* s,
     ae_int_t npoints,
     ae_int_t nvars,
     linearmodel* lm,
     lrreport* rep,
     ae_state *_state);


/*************************************************************************
Like LRBuildS, but builds model

    Y = A(0)*X[0] + ... + A(N-1)*X[N-1]

i.e. with zero constant term.

  -- ALGLIB --
     Copyright 30.10.2008 by Bochkanov Sergey
*************************************************************************/
void lrbuildzs(/* Real    */ const ae_matrix* xy,
     /* Real    */ const ae_vector* s,
     ae_int_t npoints,
     ae_int_t nvars,
     linearmodel* lm,
     lrreport* rep,
     ae_state *_state);


/*************************************************************************
Like LRBuild but builds model

    Y = A(0)*X[0] + ... + A(N-1)*X[N-1]

i.e. with zero constant term.

  -- ALGLIB --
     Copyright 30.10.2008 by Bochkanov Sergey
*************************************************************************/
void lrbuildz(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nvars,
     linearmodel* lm,
     lrreport* rep,
     ae_state *_state);


/*************************************************************************
Unpacks coefficients of linear model.

INPUT PARAMETERS:
    LM          -   linear model in ALGLIB format

OUTPUT PARAMETERS:
    V           -   coefficients, array[0..NVars]
                    constant term (intercept) is stored in the V[NVars].
    NVars       -   number of independent variables (one less than number
                    of coefficients)

  -- ALGLIB --
     Copyright 30.08.2008 by Bochkanov Sergey
*************************************************************************/
void lrunpack(const linearmodel* lm,
     /* Real    */ ae_vector* v,
     ae_int_t* nvars,
     ae_state *_state);


/*************************************************************************
"Packs" coefficients and creates linear model in ALGLIB format (LRUnpack
reversed).

INPUT PARAMETERS:
    V           -   coefficients, array[0..NVars]
    NVars       -   number of independent variables

OUTPUT PAREMETERS:
    LM          -   linear model.

  -- ALGLIB --
     Copyright 30.08.2008 by Bochkanov Sergey
*************************************************************************/
void lrpack(/* Real    */ const ae_vector* v,
     ae_int_t nvars,
     linearmodel* lm,
     ae_state *_state);


/*************************************************************************
Procesing

INPUT PARAMETERS:
    LM      -   linear model
    X       -   input vector,  array[0..NVars-1].

Result:
    value of linear model regression estimate

  -- ALGLIB --
     Copyright 03.09.2008 by Bochkanov Sergey
*************************************************************************/
double lrprocess(const linearmodel* lm,
     /* Real    */ const ae_vector* x,
     ae_state *_state);


/*************************************************************************
RMS error on the test set

INPUT PARAMETERS:
    LM      -   linear model
    XY      -   test set
    NPoints -   test set size

RESULT:
    root mean square error.

  -- ALGLIB --
     Copyright 30.08.2008 by Bochkanov Sergey
*************************************************************************/
double lrrmserror(const linearmodel* lm,
     /* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_state *_state);


/*************************************************************************
Average error on the test set

INPUT PARAMETERS:
    LM      -   linear model
    XY      -   test set
    NPoints -   test set size

RESULT:
    average error.

  -- ALGLIB --
     Copyright 30.08.2008 by Bochkanov Sergey
*************************************************************************/
double lravgerror(const linearmodel* lm,
     /* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_state *_state);


/*************************************************************************
RMS error on the test set

INPUT PARAMETERS:
    LM      -   linear model
    XY      -   test set
    NPoints -   test set size

RESULT:
    average relative error.

  -- ALGLIB --
     Copyright 30.08.2008 by Bochkanov Sergey
*************************************************************************/
double lravgrelerror(const linearmodel* lm,
     /* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_state *_state);


/*************************************************************************
Copying of LinearModel strucure

INPUT PARAMETERS:
    LM1 -   original

OUTPUT PARAMETERS:
    LM2 -   copy

  -- ALGLIB --
     Copyright 15.03.2009 by Bochkanov Sergey
*************************************************************************/
void lrcopy(const linearmodel* lm1, linearmodel* lm2, ae_state *_state);


void lrlines(/* Real    */ const ae_matrix* xy,
     /* Real    */ const ae_vector* s,
     ae_int_t n,
     double* a,
     double* b,
     double* vara,
     double* varb,
     double* covab,
     double* corrab,
     double* p,
     ae_state *_state);


void lrline(/* Real    */ const ae_matrix* xy,
     ae_int_t n,
     double* a,
     double* b,
     ae_state *_state);
void _linearmodel_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _linearmodel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _linearmodel_clear(void* _p);
void _linearmodel_destroy(void* _p);
void _lrreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _lrreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _lrreport_clear(void* _p);
void _lrreport_destroy(void* _p);


/*$ End $*/
#endif

