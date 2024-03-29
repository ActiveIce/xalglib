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

#ifndef _normestimator_h
#define _normestimator_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "scodes.h"
#include "ablasmkl.h"
#include "tsort.h"
#include "sparse.h"
#include "ablas.h"
#include "creflections.h"
#include "matgen.h"


/*$ Declarations $*/


/*************************************************************************
This object stores state of the iterative norm estimation algorithm.

You should use ALGLIB functions to work with this object.
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t nstart;
    ae_int_t nits;
    ae_int_t seedval;
    ae_vector x0;
    ae_vector x1;
    ae_vector t;
    ae_vector xbest;
    hqrndstate r;
    ae_vector x;
    ae_vector mv;
    ae_vector mtv;
    ae_bool needmv;
    ae_bool needmtv;
    double repnorm;
    rcommstate rstate;
} normestimatorstate;


/*$ Body $*/


/*************************************************************************
This procedure initializes matrix norm estimator.

USAGE:
1. User initializes algorithm state with NormEstimatorCreate() call
2. User calls NormEstimatorEstimateSparse() (or NormEstimatorIteration())
3. User calls NormEstimatorResults() to get solution.
   
INPUT PARAMETERS:
    M       -   number of rows in the matrix being estimated, M>0
    N       -   number of columns in the matrix being estimated, N>0
    NStart  -   number of random starting vectors
                recommended value - at least 5.
    NIts    -   number of iterations to do with best starting vector
                recommended value - at least 5.

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

    
NOTE: this algorithm is effectively deterministic, i.e. it always  returns
same result when repeatedly called for the same matrix. In fact, algorithm
uses randomized starting vectors, but internal  random  numbers  generator
always generates same sequence of the random values (it is a  feature, not
bug).

Algorithm can be made non-deterministic with NormEstimatorSetSeed(0) call.

  -- ALGLIB --
     Copyright 06.12.2011 by Bochkanov Sergey
*************************************************************************/
void normestimatorcreate(ae_int_t m,
     ae_int_t n,
     ae_int_t nstart,
     ae_int_t nits,
     normestimatorstate* state,
     ae_state *_state);


/*************************************************************************
This function changes seed value used by algorithm. In some cases we  need
deterministic processing, i.e. subsequent calls must return equal results,
in other cases we need non-deterministic algorithm which returns different
results for the same matrix on every pass.

Setting zero seed will lead to non-deterministic algorithm, while non-zero 
value will make our algorithm deterministic.

INPUT PARAMETERS:
    State       -   norm estimator state, must be initialized with a  call
                    to NormEstimatorCreate()
    SeedVal     -   seed value, >=0. Zero value = non-deterministic algo.

  -- ALGLIB --
     Copyright 06.12.2011 by Bochkanov Sergey
*************************************************************************/
void normestimatorsetseed(normestimatorstate* state,
     ae_int_t seedval,
     ae_state *_state);


/*************************************************************************

  -- ALGLIB --
     Copyright 06.12.2011 by Bochkanov Sergey
*************************************************************************/
ae_bool normestimatoriteration(normestimatorstate* state,
     ae_state *_state);


/*************************************************************************
This function estimates norm of the sparse M*N matrix A.

INPUT PARAMETERS:
    State       -   norm estimator state, must be initialized with a  call
                    to NormEstimatorCreate()
    A           -   sparse M*N matrix, must be converted to CRS format
                    prior to calling this function.

After this function  is  over  you can call NormEstimatorResults() to get 
estimate of the norm(A).

  -- ALGLIB --
     Copyright 06.12.2011 by Bochkanov Sergey
*************************************************************************/
void normestimatorestimatesparse(normestimatorstate* state,
     const sparsematrix* a,
     ae_state *_state);


/*************************************************************************
Matrix norm estimation results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    Nrm     -   estimate of the matrix norm, Nrm>=0

  -- ALGLIB --
     Copyright 06.12.2011 by Bochkanov Sergey
*************************************************************************/
void normestimatorresults(const normestimatorstate* state,
     double* nrm,
     ae_state *_state);


/*************************************************************************
This  function  restarts estimator and prepares it for the next estimation
round.

INPUT PARAMETERS:
    State   -   algorithm state
  -- ALGLIB --
     Copyright 06.12.2011 by Bochkanov Sergey
*************************************************************************/
void normestimatorrestart(normestimatorstate* state, ae_state *_state);
void _normestimatorstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _normestimatorstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _normestimatorstate_clear(void* _p);
void _normestimatorstate_destroy(void* _p);


/*$ End $*/
#endif

