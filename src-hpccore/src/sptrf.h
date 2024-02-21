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

#ifndef _sptrf_h
#define _sptrf_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "scodes.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "tsort.h"
#include "sparse.h"
#include "dlu.h"


/*$ Declarations $*/


/*************************************************************************
This structure is used by sparse LU to store "left" and "upper" rectangular
submatrices BL and BU, as defined below:


    [    |         :       ]
    [ LU |   BU    :       ]
    [    |         :       ]
    [--------------: dense ]
    [    |         : trail ]
    [    |  sparse :       ]
    [ BL |         :       ]
    [    |  trail  :       ]
    [    |         :       ]
    

*************************************************************************/
typedef struct
{
    ae_int_t nfixed;
    ae_int_t ndynamic;
    ae_vector idxfirst;
    ae_vector strgidx;
    ae_vector strgval;
    ae_int_t nallocated;
    ae_int_t nused;
} sluv2list1matrix;


/*************************************************************************
This structure is used by sparse LU to store sparse trail submatrix as
defined below:


    [    |         :       ]
    [ LU |   BU    :       ]
    [    |         :       ]
    [--------------: dense ]
    [    |         : trail ]
    [    |  sparse :       ]
    [ BL |         :       ]
    [    |  trail  :       ]
    [    |         :       ]
    

*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t k;
    ae_vector nzc;
    ae_int_t maxwrkcnt;
    ae_int_t maxwrknz;
    ae_int_t wrkcnt;
    ae_vector wrkset;
    ae_vector colid;
    ae_vector isdensified;
    ae_vector slscolptr;
    ae_vector slsrowptr;
    ae_vector slsidx;
    ae_vector slsval;
    ae_int_t slsused;
    ae_vector tmp0;
} sluv2sparsetrail;


/*************************************************************************
This structure is used by sparse LU to store dense trail submatrix as
defined below:


    [    |         :       ]
    [ LU |   BU    :       ]
    [    |         :       ]
    [--------------: dense ]
    [    |         : trail ]
    [    |  sparse :       ]
    [ BL |         :       ]
    [    |  trail  :       ]
    [    |         :       ]
    

*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t ndense;
    ae_matrix d;
    ae_vector did;
} sluv2densetrail;


/*************************************************************************
This structure is used by sparse LU for buffer storage

*************************************************************************/
typedef struct
{
    ae_int_t n;
    sparsematrix sparsel;
    sparsematrix sparseut;
    sluv2list1matrix bleft;
    sluv2list1matrix bupper;
    sluv2sparsetrail strail;
    sluv2densetrail dtrail;
    ae_vector rowpermrawidx;
    ae_matrix dbuf;
    ae_vector v0i;
    ae_vector v1i;
    ae_vector v0r;
    ae_vector v1r;
    ae_vector tmp0;
    ae_vector tmpi;
    ae_vector tmpp;
} sluv2buffer;


/*$ Body $*/


/*************************************************************************
Sparse LU for square NxN CRS matrix with both row and column permutations.

Represents A as Pr*L*U*Pc, where:
* Pr is a product of row permutations Pr=Pr(0)*Pr(1)*...*Pr(n-2)*Pr(n-1)
* Pc is a product of col permutations Pc=Pc(n-1)*Pc(n-2)*...*Pc(1)*Pc(0)
* L is lower unitriangular
* U is upper triangular

INPUT PARAMETERS:
    A           -   sparse square matrix in CRS format
    PivotType   -   pivot type:
                    * 0 - for best pivoting available
                    * 1 - row-only pivoting
                    * 2 - row and column greedy pivoting  algorithm  (most
                          sparse pivot column is selected from the trailing
                          matrix at each step)
    Buf         -   temporary buffer, previously allocated memory is
                    reused as much as possible

OUTPUT PARAMETERS:
    A           -   LU decomposition of A
    PR          -   array[N], row pivots
    PC          -   array[N], column pivots
    Buf         -   following fields of Buf are set:
                    * Buf.RowPermRawIdx[] - contains row permutation, with
                      RawIdx[I]=J meaning that J-th row  of  the  original
                      input matrix was moved to Ith position of the output
                      factorization
    
This function always succeeds  i.e. it ALWAYS returns valid factorization,
but for your convenience it also  returns boolean  value  which  helps  to
detect symbolically degenerate matrix:
* function returns TRUE if the matrix was factorized AND symbolically
  non-degenerate
* function returns FALSE if the matrix was factorized but U has strictly
  zero elements at the diagonal (the factorization is returned anyway).

  -- ALGLIB routine --
     15.01.2019
     Bochkanov Sergey
*************************************************************************/
ae_bool sptrflu(sparsematrix* a,
     ae_int_t pivottype,
     /* Integer */ ae_vector* pr,
     /* Integer */ ae_vector* pc,
     sluv2buffer* buf,
     ae_state *_state);
void _sluv2list1matrix_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sluv2list1matrix_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sluv2list1matrix_clear(void* _p);
void _sluv2list1matrix_destroy(void* _p);
void _sluv2sparsetrail_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sluv2sparsetrail_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sluv2sparsetrail_clear(void* _p);
void _sluv2sparsetrail_destroy(void* _p);
void _sluv2densetrail_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sluv2densetrail_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sluv2densetrail_clear(void* _p);
void _sluv2densetrail_destroy(void* _p);
void _sluv2buffer_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sluv2buffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sluv2buffer_clear(void* _p);
void _sluv2buffer_destroy(void* _p);


/*$ End $*/
#endif

