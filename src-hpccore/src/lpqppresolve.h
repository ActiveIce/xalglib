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

#ifndef _lpqppresolve_h
#define _lpqppresolve_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "apstruct.h"
#include "scodes.h"
#include "ablasmkl.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "tsort.h"
#include "sparse.h"


/*$ Declarations $*/


typedef struct
{
    ae_int_t m;
    ae_int_t n;
    ae_vector rowbegin;
    ae_vector rowend;
    ae_vector idx;
    ae_vector vals;
} dynamiccrs;


/*************************************************************************
This object stores temporaries for presolver.
*************************************************************************/
typedef struct
{
    niset setn;
    niset setm;
} presolvebuffers;


/*************************************************************************
This object stores a sequence of transformations  applied  to  the  linear
problem. It can be used to 'playback'  these  transformations  to  restore
solution for the untransformed problem.

TrfType is an array[NTrf] which stores transformation types:
* 0         cost scaling
* 1         column scaling
* 2         row scaling
* 3         drop empty column
* 4         drop empty row
* 5         convert singleton row to box constraint
* 6         fix variable
* 7         explicit slack
* 8         implicit slack
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t ntrf;
    ae_vector trftype;
    ae_vector idata;
    ae_vector rdata;
    ae_vector idataridx;
    ae_vector rdataridx;
    ae_int_t sourceidx;
    ae_int_t isrc;
    ae_int_t rsrc;
    ae_vector sparseidx0;
    ae_vector sparseval0;
} presolverstack;


/*************************************************************************
This object stores transformation used to convert solution (primal and dual)
to original variables. It is also used to store temporaries.
*************************************************************************/
typedef struct
{
    ae_int_t newn;
    ae_int_t oldn;
    ae_int_t newm;
    ae_int_t oldm;
    ae_vector rawc;
    ae_vector rawbndl;
    ae_vector rawbndu;
    sparsematrix rawa;
    ae_int_t problemstatus;
    ae_vector lagrangefromresidual;
    ae_vector c;
    ae_vector bndl;
    ae_vector bndu;
    sparsematrix sparsea;
    ae_vector al;
    ae_vector au;
    ae_vector packxperm;
    ae_vector packyperm;
    ae_vector packstatperm;
    ae_vector unpackxperm;
    ae_vector unpackyperm;
    ae_vector unpackstatperm;
    presolverstack trfstack;
    ae_vector s1;
    ae_vector bc1;
    ae_vector x1;
    ae_vector y1;
    ae_vector d;
    presolvebuffers buf;
} presolveinfo;


/*$ Body $*/


/*************************************************************************
No presolve, just user-supplied scaling + constraint and cost vector
normalization.

INPUT PARAMETERS:
    S           -   array[N], user-supplied scale vector, S[I]>0
    C           -   array[N], costs
    BndL        -   array[N], lower bounds (may contain -INF)
    BndU        -   array[N], upper bounds (may contain +INF)
    N           -   variable count, N>0
    SparseA     -   matrix[K,N], sparse constraints
    AL          -   array[K], lower constraint bounds (may contain -INF)
    AU          -   array[K], upper constraint bounds (may contain +INF)
    K           -   constraint count, K>=0
    Info        -   presolve info structure; temporaries allocated during
                    previous calls may be reused by this function.
                    
OUTPUT PARAMETERS:
    Info        -   contains transformed C, BndL, bndU,  SparseA,  AL,  AU
                    and   information   necessary   to   perform  backward
                    transformation.
                    Following fields can be accessed:
                    * ProblemStatus    which is:
                                       *  0 for successful transformation
                                       * -3 for infeasible problem
                                       * -4 for unbounded problem
                    
                    If Info.ProblemStatus=0, then the following fields can
                    be accessed:
                    * Info.NewN>0  for transformed problem size
                    * Info.NewM>=0 for transformed constraint count
                    * always:          Info.C, Info.BndL, Info.BndU - array[NewN]
                    * for Info.NewM>0: Info.SparseA, Info.AL, Info.AU
    
NOTE: this routine does not reallocate arrays if NNew<=NOld and/or KNew<=KOld.

  -- ALGLIB --
     Copyright 01.07.2020 by Bochkanov Sergey
*************************************************************************/
void presolvenonescaleuser(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_int_t n,
     const sparsematrix* sparsea,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_bool dotrace,
     presolveinfo* info,
     ae_state *_state);


/*************************************************************************
Extensive presolving for an LP problem, all techniques are used

INPUT PARAMETERS:
    S           -   array[N], user-supplied scale vector, S[I]>0
    C           -   array[N], costs
    BndL        -   array[N], lower bounds (may contain -INF)
    BndU        -   array[N], upper bounds (may contain +INF)
    N           -   variable count, N>0
    SparseA     -   matrix[K,N], sparse constraints
    AL          -   array[K], lower constraint bounds (may contain -INF)
    AU          -   array[K], upper constraint bounds (may contain +INF)
    K           -   constraint count, K>=0
    Info        -   presolve info structure; temporaries allocated during
                    previous calls may be reused by this function.
                    
OUTPUT PARAMETERS:
    Info        -   contains transformed C, BndL, bndU,  SparseA,  AL,  AU
                    and   information   necessary   to   perform  backward
                    transformation.
                    Following fields can be acessed:
                    * Info.NewN>0  for transformed problem size
                    * Info.NewM>=0 for transformed constraint count
                    * always:          Info.C, Info.BndL, Info.BndU - array[NewN]
                    * for Info.NewM>0: Info.SparseA, Info.AL, Info.AU
    
NOTE: this routine does not reallocate arrays if NNew<=NOld and/or KNew<=KOld.

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
void presolvelp(/* Real    */ const ae_vector* raws,
     /* Real    */ const ae_vector* rawc,
     /* Real    */ const ae_vector* rawbndl,
     /* Real    */ const ae_vector* rawbndu,
     ae_int_t n,
     const sparsematrix* rawsparsea,
     /* Real    */ const ae_vector* rawal,
     /* Real    */ const ae_vector* rawau,
     ae_int_t m,
     ae_bool dotrace,
     presolveinfo* presolved,
     ae_state *_state);


/*************************************************************************
Backward transformation which extracts original solution from that of  the
converted problem.

Below NNew/KNew correspond to transformed problem size (as returned by the
presolve routine) and NOld/KOld correspond to original  problem  size  (as
specified by caller). We expect that caller knows  these  sizes,  so  this
routine does not report them.

INPUT PARAMETERS:
    Info        -   presolve info structure
    X           -   array[NNew], transformed solution (primal variables)
    Stats       -   array[NNew+MNew], transformed constraint status (negative -
                    at lower bound, positive -  at  upper  bound,  zero  -
                    inactive).
    LagBC       -   array[NNew], transformed Lagrange multipliers
    LagLC       -   array[KNew], transformed Lagrange multipliers
                    
OUTPUT PARAMETERS:
    X           -   array[NOld], original solution (primal variables)
    Stats       -   array[NOld+MOld], original constraint status
    LagBC       -   array[NOld], Lagrange multipliers
    LagLC       -   array[KOld], Lagrange multipliers
    
NOTE: this routine does not reallocate arrays if NOld<=NNew and/or KOld<=KNew.

  -- ALGLIB --
     Copyright 01.07.2020 by Bochkanov Sergey
*************************************************************************/
void presolvebwd(presolveinfo* info,
     /* Real    */ ae_vector* x,
     /* Integer */ ae_vector* stats,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     ae_state *_state);
void _dynamiccrs_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dynamiccrs_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dynamiccrs_clear(void* _p);
void _dynamiccrs_destroy(void* _p);
void _presolvebuffers_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _presolvebuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _presolvebuffers_clear(void* _p);
void _presolvebuffers_destroy(void* _p);
void _presolverstack_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _presolverstack_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _presolverstack_clear(void* _p);
void _presolverstack_destroy(void* _p);
void _presolveinfo_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _presolveinfo_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _presolveinfo_clear(void* _p);
void _presolveinfo_destroy(void* _p);


/*$ End $*/
#endif

