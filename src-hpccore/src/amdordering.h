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

#ifndef _amdordering_h
#define _amdordering_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "apstruct.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "scodes.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "tsort.h"
#include "sparse.h"


/*$ Declarations $*/


/*************************************************************************
This structure is used to store K sets of N possible integers each.
The structure needs at least O(N) temporary memory.
*************************************************************************/
typedef struct
{
    ae_int_t k;
    ae_int_t n;
    ae_vector flagarray;
    ae_vector vbegin;
    ae_vector vallocated;
    ae_vector vcnt;
    ae_vector data;
    ae_int_t dataused;
    ae_int_t iterrow;
    ae_int_t iteridx;
} amdknset;


/*************************************************************************
This structure is used to store vertex degrees, with  ability  to  quickly
(in O(1) time) select one with smallest degree
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_bool checkexactdegrees;
    ae_int_t smallestdegree;
    ae_vector approxd;
    ae_vector optionalexactd;
    ae_vector isvertex;
    ae_vector eligible;
    ae_vector vbegin;
    ae_vector vprev;
    ae_vector vnext;
} amdvertexset;


/*************************************************************************
This structure is used to store linked list NxN matrix.

The fields are:
* VBegin - array[2*N+1], stores first entries in each row (N values),  col
  (N values), list of free entries (1 value), 2*N+1 in total
* Entries - stores EntriesInitialized elements, each occupying llmEntrySize
  elements of array. These entries are organized into linked row and column
  list, with each entry belonging to both row list and column list.
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_vector vbegin;
    ae_vector vcolcnt;
    ae_vector entries;
    ae_int_t entriesinitialized;
} amdllmatrix;


/*************************************************************************
This structure is used to store temporaries for AMD ordering
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_bool extendeddebug;
    ae_bool checkexactdegrees;
    ae_vector iseliminated;
    ae_vector issupernode;
    ae_vector iseligible;
    amdknset setsuper;
    amdknset seta;
    amdknset sete;
    amdllmatrix mtxl;
    amdvertexset vertexdegrees;
    niset setq;
    ae_vector perm;
    ae_vector invperm;
    ae_vector columnswaps;
    niset setp;
    niset lp;
    niset setrp;
    niset ep;
    niset adji;
    niset adjj;
    ae_vector ls;
    ae_int_t lscnt;
    niset setqsupercand;
    niset exactdegreetmp0;
    amdknset hashbuckets;
    niset nonemptybuckets;
    ae_vector sncandidates;
    ae_vector tmp0;
    ae_vector arrwe;
    ae_matrix dbga;
} amdbuffer;


/*$ Body $*/


/*************************************************************************
This function generates approximate minimum degree ordering

INPUT PARAMETERS
    A           -   lower triangular sparse matrix  in  CRS  format.  Only
                    sparsity structure (as given by Idx[] field)  matters,
                    specific values of matrix elements are ignored.
    N           -   problem size
    Buf         -   reusable buffer object, does not need special initialization
    
OUTPUT PARAMETERS
    Perm        -   array[N], maps original indexes I to permuted indexes
    InvPerm     -   array[N], maps permuted indexes I to original indexes
    
NOTE: definite 'DEBUG.SLOW' trace tag will  activate  extra-slow  (roughly
      N^3 ops) integrity checks, in addition to cheap O(1) ones.

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
void generateamdpermutation(const sparsematrix* a,
     ae_int_t n,
     /* Integer */ ae_vector* perm,
     /* Integer */ ae_vector* invperm,
     amdbuffer* buf,
     ae_state *_state);


/*************************************************************************
This  function  generates  approximate  minimum  degree ordering,   either
classic or improved with better support for dense rows:
* the classic version processed entire matrix and returns N as result. The
  problem with classic version is that it may be slow  for  matrices  with
  dense or nearly dense rows
* the improved version processes K most sparse rows, and moves  other  N-K
  ones to the end. The number of sparse rows  K  is  returned.  The  tail,
  which is now a (N-K)*(N-K) matrix, should be repeatedly processed by the
  same function until zero is returned.

INPUT PARAMETERS
    A           -   lower triangular sparse matrix in CRS format
    Eligible    -   array[N], set of boolean flags that mark columns of  A
                    as eligible for ordering. Columns that are not eligible
                    are postponed (moved to the end) by the  improved  AMD
                    algorithm. This array is ignored  (not  referenced  at
                    all) when AMDType=0.
    N           -   problem size
    PromoteAbove-   columns with degrees higher than PromoteAbove*max(MEAN(Degree),1)
                    may be postponed. Ignored for AMDType<>1.
                    This parameter controls postponement of dense columns
                    (and algorithm ability to efficiently handle them):
                    * big PromoteAbove (N or more) effectively means that
                      no eligible columns are postponed. Better to combine
                      with your own heuristic to choose eligible columns,
                      otherwise algorithm will have hard time on problems
                      with dense columns in the eligible set.
                    * values between 2 and 10 are usually  a  good  choice
                      for manual control
                    * zero  value  means   that   appropriate   value   is
                      automatically chosen. Specific value may  change  in
                      future ALGLIB versions. Recommended.
    AMDType     -   ordering type:
                    * 0 for the classic AMD
                    * 1 for the improved AMD
    Buf         -   reusable buffer object, does not need special initialization
    
OUTPUT PARAMETERS
    Perm        -   array[N], maps original indexes I to permuted indexes
    InvPerm     -   array[N], maps permuted indexes I to original indexes
    
RESULT:
    number of successfully ordered rows/cols;
    for AMDType=0:  Result=N
    for AMDType=1:  0<=Result<=N. Result=0 is returned only when there are
                    no columns that are both sparse enough and eligible.
    
NOTE: defining 'DEBUG.SLOW' trace tag will  activate  extra-slow  (roughly
      N^3 ops) integrity checks, in addition to cheap O(1) ones.

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
ae_int_t generateamdpermutationx(const sparsematrix* a,
     /* Boolean */ const ae_vector* eligible,
     ae_int_t n,
     double promoteabove,
     /* Integer */ ae_vector* perm,
     /* Integer */ ae_vector* invperm,
     ae_int_t amdtype,
     amdbuffer* buf,
     ae_state *_state);
void _amdknset_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _amdknset_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _amdknset_clear(void* _p);
void _amdknset_destroy(void* _p);
void _amdvertexset_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _amdvertexset_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _amdvertexset_clear(void* _p);
void _amdvertexset_destroy(void* _p);
void _amdllmatrix_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _amdllmatrix_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _amdllmatrix_clear(void* _p);
void _amdllmatrix_destroy(void* _p);
void _amdbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _amdbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _amdbuffer_clear(void* _p);
void _amdbuffer_destroy(void* _p);


/*$ End $*/
#endif

