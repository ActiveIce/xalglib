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

#ifndef _lpqpserv_h
#define _lpqpserv_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "scodes.h"
#include "ablasmkl.h"
#include "ablasf.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "tsort.h"
#include "sparse.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
This function generates scaled (by S) and shifted (by XC) reformulation of
two-sided "lower-bound/upper-bound" constraints stored in mixed dense/sparse format.

INPUT PARAMETERS:
    S               -   scale vector, array[N]:
                        * I-th element contains scale of I-th variable,
                        * SC[I]>0
    XOrigin         -   origin term, array[N]. Can be zero.
    N               -   number of variables.
    SparseA         -   sparse M*N constraint matrix in CRS format;
                        ignored if M=0.
    M               -   sparse constraint count, M>=0
    AL              -   lower bounds for constraints, array[M]
    AU              -   upper bounds for constraints, array[M]

OUTPUT PARAMETERS:
    SparseA         -   replaced by scaled/shifted constraints
    AL              -   replaced by scaled/shifted lower bounds
    AU              -   replaced by scaled/shifted upper bounds

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void scaleshiftmixedlcinplace(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     sparsematrix* sparsea,
     ae_int_t msparse,
     /* Real    */ ae_matrix* densea,
     ae_int_t mdense,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     ae_state *_state);


/*************************************************************************
This function generates scaled (by S) and shifted (by XC) reformulation of
the box constraints.

INPUT PARAMETERS:
    S               -   scale vector, array[N]:
                        * I-th element contains scale of I-th variable,
                        * SC[I]>0
    XOrigin         -   origin term, array[N]. Can be zero.
    BndL            -     raw lower bounds, array[N]
    BndU            -   raw upper bounds, array[N]
    N               -   number of variables.

OUTPUT PARAMETERS:
    BndL            -   replaced by scaled/shifted lower bounds, array[N]
    BndU            -   replaced by scaled/shifted upper bounds, array[N]

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void scaleshiftbcinplace(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     /* Real    */ ae_vector* bndl,
     /* Real    */ ae_vector* bndu,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
This function generates scaled (by S) and shifted (by XC) reformulation of
two-sided "lower-bound/range" constraints stored in dense format.

INPUT PARAMETERS:
    S               -   scale vector, array[N]:
                        * I-th element contains scale of I-th variable,
                        * SC[I]>0
    XOrigin         -   origin term, array[N]. Can be zero.
    N               -   number of variables.
    DenseA          -   array[M,N], constraint matrix
    AB              -   lower bounds for constraints, always present and
                        finite, array[M]
    AR              -   ranges for constraints, can be zero (equality
                        constraint), positive (range constraint) or +INF
                        (lower bound constraint), array[M]
    M               -   constraint count, M>=0

OUTPUT PARAMETERS:
    DenseA          -   replaced by scaled/shifted constraints, array[M,N]
    AB              -   replaced by scaled/shifted lower bounds, array[M]
    AR              -   replaced by scaled/shifted ranges, array[M]

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void scaleshiftdensebrlcinplace(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     /* Real    */ ae_matrix* densea,
     /* Real    */ ae_vector* ab,
     /* Real    */ ae_vector* ar,
     ae_int_t m,
     ae_state *_state);


/*************************************************************************
This function generates scaled (by S) and shifted (by XC) reformulation of
two-sided "lower-bound/range" constraints stored in dense format.

INPUT PARAMETERS:
    S               -   scale vector, array[N]:
                        * I-th element contains scale of I-th variable,
                        * SC[I]>0
    XOrigin         -   origin term, array[N]. Can be zero.
    N               -   number of variables.
    SparseA         -   sparse MSparse*N constraint matrix in CRS format;
                        ignored if MSparse=0.
    MSparse         -   dense constraint count, MSparse>=0
    DenseA          -   array[MDense,N], constraint matrix;
                        ignored if MDense=0.
    MDense          -   dense constraint count, MDense>=0
    AB              -   lower bounds for constraints, always present and
                        finite, array[MSparse+MDense]
    AR              -   ranges for constraints, can be zero (equality
                        constraint), positive (range constraint) or +INF
                        (lower bound constraint), array[MSparse+MDense]

OUTPUT PARAMETERS:
    DenseA          -   replaced by scaled/shifted constraints, array[MDense,N]
    SparseA         -   replaced by scaled/shifted constraints, array[MSparse,N]
    AB              -   replaced by scaled/shifted lower bounds, array[MDense+MSparse]
    AR              -   replaced by scaled/shifted ranges, array[MDense+MSparse]

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void scaleshiftmixedbrlcinplace(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     sparsematrix* sparsea,
     ae_int_t msparse,
     /* Real    */ ae_matrix* densea,
     ae_int_t mdense,
     /* Real    */ ae_vector* ab,
     /* Real    */ ae_vector* ar,
     ae_state *_state);


/*************************************************************************
This function generates scaled (by S) reformulation of dense quadratic and
linear terms in QP problem.

INPUT PARAMETERS:
    N               -   number of variables.
    DenseA          -   array[NMain,NMain], quadratic term
    IsUpper         -   whether upper or lower triangle is present
    NMain           -   number of nonslack vars, 1<=NMain<=NTotal
    DenseB          -   array[NTotal], linear term
    NTotal          -   total number of variables, NTotal>=1
    S               -   scale vector, array[NTotal]:
                        * I-th element contains scale of I-th variable,
                        * SC[I]>0

OUTPUT PARAMETERS:
    DenseA          -   replaced by scaled term, array[N,N]
    DenseB          -   replaced by scaled term, array[N]

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void scaledenseqpinplace(/* Real    */ ae_matrix* densea,
     ae_bool isupper,
     ae_int_t nmain,
     /* Real    */ ae_vector* denseb,
     ae_int_t ntotal,
     /* Real    */ const ae_vector* s,
     ae_state *_state);


/*************************************************************************
This function generates scaled (by S) reformulation of sparse quadratic and
linear terms in QP problem.

INPUT PARAMETERS:
    S               -   scale vector, array[N]:
                        * I-th element contains scale of I-th variable,
                        * SC[I]>0
    N               -   number of variables.
    SparseA         -   quadratic term, NxN SparseMatrix in CRS format
                        (any triangle can be present, we will scale everything)
    DenseCorrC      -   array[CorrRank,N], low rank correction C'*D*C being
                        added to the quadratic term
    DenseCorrD      -   array[CorrRank], low rank correction, diagonal factors
    CorrRank        -   correction rank, >=0
    DenseB          -   array[N], linear term

OUTPUT PARAMETERS:
    SparseA         -   replaced by scaled term
    DenseB          -   replaced by scaled term

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void scalesparseqpinplace(/* Real    */ const ae_vector* s,
     ae_int_t n,
     sparsematrix* sparsea,
     /* Real    */ ae_matrix* densecorrc,
     /* Real    */ ae_vector* densecorrd,
     ae_int_t corrrank,
     /* Real    */ ae_vector* denseb,
     ae_state *_state);


/*************************************************************************
This function normalizes two-sided "lower-bound/range" constraints  stored
in dense format in such a way that L2 norms of rows (right hand  side  NOT
included) become equal to 1.0. Exactly zero rows are handled correctly.

INPUT PARAMETERS:
    DenseA          -   array[M,N], constraint matrix
    AB              -   lower bounds for constraints, always present and
                        finite, array[M]
    AR              -   ranges for constraints, can be zero (equality
                        constraint), positive (range constraint) or +INF
                        (lower bound constraint), array[M]
    N               -   number of variables, N>=1.
    M               -   constraint count, M>=0
    NeedNorms       -   whether we need row norms or not

OUTPUT PARAMETERS:
    DenseA          -   replaced by normalized constraints, array[M,N]
    AB              -   replaced by normalized lower bounds, array[M]
    AR              -   replaced by normalized ranges, array[M]
    RowNorms        -   if NeedNorms is true, leading M elements (resized
                        if length is less than M) are filled by row norms
                        before normalization was performed.
    

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void normalizedensebrlcinplace(/* Real    */ ae_matrix* densea,
     /* Real    */ ae_vector* ab,
     /* Real    */ ae_vector* ar,
     ae_int_t n,
     ae_int_t m,
     /* Real    */ ae_vector* rownorms,
     ae_bool neednorms,
     ae_state *_state);


/*************************************************************************
This function normalizes two-sided "lower-bound/range" constraints  stored
in dense format in such a way that L2 norms of rows (right hand  side  NOT
included) become equal to 1.0. Exactly zero rows are handled correctly.

INPUT PARAMETERS:
    SparseA         -   sparse MSparse*N constraint matrix in CRS format;
                        ignored if MSparse=0.
    MSparse         -   dense constraint count, MSparse>=0
    DenseA          -   array[MDense,N], constraint matrix;
                        ignored if MDense=0.
    MDense          -   dense constraint count, MDense>=0
    AB              -   lower bounds for constraints, always present and
                        finite, array[MSparse+MDense]
    AR              -   ranges for constraints, can be zero (equality
                        constraint), positive (range constraint) or +INF
                        (lower bound constraint), array[MSparse+MDense]
    N               -   number of variables, N>=1.
    LimitedAmplification-   whether row amplification is limited or not:
                        * if False, rows with small norms (less than 1.0)
                          are always normalized
                        * if True, we do not increase individual row norms
                          during normalization - only decrease. However,
                          we may apply one amplification rount to entire
                          constraint matrix, i.e. amplify all rows by same
                          coefficient. As result, we do not overamplify
                          any single row, but still make sure than entire
                          problem is well scaled.
                        If True, only large rows are normalized.
    NeedNorms       -   whether we need row norms or not

OUTPUT PARAMETERS:
    DenseA          -   replaced by normalized constraints, array[M,N]
    AB              -   replaced by normalized lower bounds, array[M]
    AR              -   replaced by normalized ranges, array[M]
    RowNorms        -   if NeedNorms is true, leading M elements (resized
                        if length is less than M) are filled by row norms
                        before normalization was performed.
    

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void normalizemixedbrlcinplace(sparsematrix* sparsea,
     ae_int_t msparse,
     /* Real    */ ae_matrix* densea,
     ae_int_t mdense,
     /* Real    */ ae_vector* ab,
     /* Real    */ ae_vector* ar,
     ae_int_t n,
     ae_bool limitedamplification,
     /* Real    */ ae_vector* rownorms,
     ae_bool neednorms,
     ae_state *_state);


/*************************************************************************
This function normalizes dense QP problem in such a way that maximum  over
its linear/quadratic coefficients max(max(A),max(B)) becomes equal to 1.0.

NOTE: completely zero A and B are handled correctly.

INPUT PARAMETERS:
    DenseA          -   array[NMain,NMain], quadratic term
    IsUpper         -   whether upper or lower triangle is present
    NMain           -   number of nonslack vars, 1<=NMain<=NTotal
    DenseB          -   array[NTotal], linear term
    NTotal          -   total number of variables.

OUTPUT PARAMETERS:
    DenseA          -   replaced by normalized term
    DenseB          -   replaced by normalized term
    
RESULT:
    max(max(A),max(B)) is returned

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
double normalizedenseqpinplace(/* Real    */ ae_matrix* densea,
     ae_bool isupper,
     ae_int_t nmain,
     /* Real    */ ae_vector* denseb,
     ae_int_t ntotal,
     ae_state *_state);


/*************************************************************************
This function normalizes sparse QP problem in such a way that maximum over
its linear/quadratic coefficients max(max(A),max(B)) becomes equal to 1.0.

NOTE: completely zero A and B are handled correctly.

INPUT PARAMETERS:
    SparseA         -   Sparse NxN matrix, either upper or lower triangle,
                        diagonal MUST be present
    IsUpper         -   which triangle is present (other one is ignored)
    DenseCorrC      -   array[CorrRank,N], low rank correction C'*D*C being
                        added to the quadratic term
    DenseCorrD      -   array[CorrRank], low rank correction, diagonal factors
    CorrRank        -   correction rank, >=0
    DenseB          -   array[N], linear term
    N               -   number of variables.

OUTPUT PARAMETERS:
    DenseA          -   replaced by normalized term, array[N,N]
    DenseB          -   replaced by normalized term, array[N]
    
RESULT:
    max(max(A),max(B)) is returned

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
double normalizesparseqpinplace(sparsematrix* sparsea,
     ae_bool isupper,
     /* Real    */ ae_matrix* densecorrc,
     /* Real    */ ae_vector* densecorrd,
     ae_int_t corrrank,
     /* Real    */ ae_vector* denseb,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
This function performs transformation of X from scaled/shifted coordinates
to unscaled/unshifted ones, paying special attention to box constraints:
* points which were exactly at the boundary before scaling will be  mapped
  to corresponding boundary after scaling
* in any case, unscaled box constraints will be satisfied

  -- ALGLIB --
     Copyright 02.06.2015 by Bochkanov Sergey
*************************************************************************/
void unscaleunshiftpointbc(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     /* Real    */ const ae_vector* rawbndl,
     /* Real    */ const ae_vector* rawbndu,
     /* Real    */ const ae_vector* sclsftbndl,
     /* Real    */ const ae_vector* sclsftbndu,
     /* Boolean */ const ae_vector* hasbndl,
     /* Boolean */ const ae_vector* hasbndu,
     /* Real    */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);


/*$ End $*/
#endif

