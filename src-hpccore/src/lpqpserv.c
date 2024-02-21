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


#include <stdafx.h>
#include "lpqpserv.h"


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t k0;
    ae_int_t k1;
    double v;
    double vv;


    ae_assert(msparse==0||((sparsea->matrixtype==1&&sparsea->m==msparse)&&sparsea->n==n), "ScaleShiftMixedLCInplace: non-CRS sparse constraint matrix!", _state);
    for(i=0; i<=msparse+mdense-1; i++)
    {
        
        /*
         * Scale/shift constraint; shift its lower and upper bounds
         */
        if( i<msparse )
        {
            v = 0.0;
            k0 = sparsea->ridx.ptr.p_int[i];
            k1 = sparsea->ridx.ptr.p_int[i+1]-1;
            for(k=k0; k<=k1; k++)
            {
                j = sparsea->idx.ptr.p_int[k];
                vv = sparsea->vals.ptr.p_double[k];
                v = v+vv*xorigin->ptr.p_double[j];
                sparsea->vals.ptr.p_double[k] = vv*s->ptr.p_double[j];
            }
            al->ptr.p_double[i] = al->ptr.p_double[i]-v;
            au->ptr.p_double[i] = au->ptr.p_double[i]-v;
        }
        else
        {
            v = 0.0;
            for(j=0; j<=n-1; j++)
            {
                vv = densea->ptr.pp_double[i-msparse][j];
                v = v+vv*xorigin->ptr.p_double[j];
                densea->ptr.pp_double[i-msparse][j] = vv*s->ptr.p_double[j];
            }
            al->ptr.p_double[i] = al->ptr.p_double[i]-v;
            au->ptr.p_double[i] = au->ptr.p_double[i]-v;
        }
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_bool hasbndl;
    ae_bool hasbndu;


    for(i=0; i<=n-1; i++)
    {
        ae_assert(ae_isfinite(s->ptr.p_double[i], _state)&&s->ptr.p_double[i]>0.0, "ScaleShiftBC: S[i] is nonpositive", _state);
        ae_assert(ae_isfinite(bndl->ptr.p_double[i], _state)||ae_isneginf(bndl->ptr.p_double[i], _state), "ScaleShiftBC: BndL[i] is +INF or NAN", _state);
        ae_assert(ae_isfinite(bndu->ptr.p_double[i], _state)||ae_isposinf(bndu->ptr.p_double[i], _state), "ScaleShiftBC: BndU[i] is -INF or NAN", _state);
        hasbndl = ae_isfinite(bndl->ptr.p_double[i], _state);
        hasbndu = ae_isfinite(bndu->ptr.p_double[i], _state);
        if( (hasbndl&&hasbndu)&&ae_fp_eq(bndl->ptr.p_double[i],bndu->ptr.p_double[i]) )
        {
            
            /*
             * Make sure that BndL[I]=BndU[I] bit-to-bit
             * even with CRAZY optimizing compiler.
             */
            bndu->ptr.p_double[i] = (bndu->ptr.p_double[i]-xorigin->ptr.p_double[i])/s->ptr.p_double[i];
            bndl->ptr.p_double[i] = bndu->ptr.p_double[i];
            continue;
        }
        if( hasbndl )
        {
            bndl->ptr.p_double[i] = (bndl->ptr.p_double[i]-xorigin->ptr.p_double[i])/s->ptr.p_double[i];
        }
        if( hasbndu )
        {
            bndu->ptr.p_double[i] = (bndu->ptr.p_double[i]-xorigin->ptr.p_double[i])/s->ptr.p_double[i];
        }
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;
    double vv;


    for(i=0; i<=m-1; i++)
    {
        
        /*
         * Scale/shift constraint; shift its lower bound
         *
         * NOTE: range is NOT scaled or shifted
         */
        v = 0.0;
        for(j=0; j<=n-1; j++)
        {
            vv = densea->ptr.pp_double[i][j];
            v = v+vv*xorigin->ptr.p_double[j];
            densea->ptr.pp_double[i][j] = vv*s->ptr.p_double[j];
        }
        ab->ptr.p_double[i] = ab->ptr.p_double[i]-v;
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t k0;
    ae_int_t k1;
    double v;
    double vv;


    ae_assert(msparse==0||((sparsea->matrixtype==1&&sparsea->m==msparse)&&sparsea->n==n), "ScaleShiftMixedBRLCInplace: non-CRS sparse constraint matrix!", _state);
    for(i=0; i<=msparse-1; i++)
    {
        
        /*
         * Scale/shift constraint; shift its lower bound
         *
         * NOTE: range is NOT scaled or shifted
         */
        v = 0.0;
        k0 = sparsea->ridx.ptr.p_int[i];
        k1 = sparsea->ridx.ptr.p_int[i+1]-1;
        for(k=k0; k<=k1; k++)
        {
            j = sparsea->idx.ptr.p_int[k];
            vv = sparsea->vals.ptr.p_double[k];
            v = v+vv*xorigin->ptr.p_double[j];
            sparsea->vals.ptr.p_double[k] = vv*s->ptr.p_double[j];
        }
        ab->ptr.p_double[i] = ab->ptr.p_double[i]-v;
    }
    for(i=0; i<=mdense-1; i++)
    {
        
        /*
         * Scale/shift constraint; shift its lower bound
         *
         * NOTE: range is NOT scaled or shifted
         */
        v = 0.0;
        for(j=0; j<=n-1; j++)
        {
            vv = densea->ptr.pp_double[i][j];
            v = v+vv*xorigin->ptr.p_double[j];
            densea->ptr.pp_double[i][j] = vv*s->ptr.p_double[j];
        }
        ab->ptr.p_double[msparse+i] = ab->ptr.p_double[msparse+i]-v;
    }
}


/*************************************************************************
This function generates scaled (by S) and shifted (by XC) reformulation of
two-sided "lower-bound/upper-bound" constraints stored in sparse format.

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
void scaleshiftsparselcinplace(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     sparsematrix* sparsea,
     ae_int_t m,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t k0;
    ae_int_t k1;
    double v;
    double vv;


    ae_assert(m==0||((sparsea->matrixtype==1&&sparsea->m==m)&&sparsea->n==n), "ScaleShiftSparseLCInplace: non-CRS sparse constraint matrix!", _state);
    for(i=0; i<=m-1; i++)
    {
        
        /*
         * Scale/shift constraint; shift its lower and upper bounds
         */
        v = 0.0;
        k0 = sparsea->ridx.ptr.p_int[i];
        k1 = sparsea->ridx.ptr.p_int[i+1]-1;
        for(k=k0; k<=k1; k++)
        {
            j = sparsea->idx.ptr.p_int[k];
            vv = sparsea->vals.ptr.p_double[k];
            v = v+vv*xorigin->ptr.p_double[j];
            sparsea->vals.ptr.p_double[k] = vv*s->ptr.p_double[j];
        }
        al->ptr.p_double[i] = al->ptr.p_double[i]-v;
        au->ptr.p_double[i] = au->ptr.p_double[i]-v;
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t j0;
    ae_int_t j1;
    double si;


    for(i=0; i<=nmain-1; i++)
    {
        si = s->ptr.p_double[i];
        if( isupper )
        {
            j0 = i;
            j1 = nmain-1;
        }
        else
        {
            j0 = 0;
            j1 = i;
        }
        for(j=j0; j<=j1; j++)
        {
            densea->ptr.pp_double[i][j] = densea->ptr.pp_double[i][j]*si*s->ptr.p_double[j];
        }
    }
    for(i=0; i<=ntotal-1; i++)
    {
        denseb->ptr.p_double[i] = denseb->ptr.p_double[i]*s->ptr.p_double[i];
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k0;
    ae_int_t k1;
    ae_int_t k;
    double si;


    ae_assert((sparsea->matrixtype==1&&sparsea->m==n)&&sparsea->n==n, "ScaleSparseQPInplace: SparseA in unexpected format", _state);
    for(i=0; i<=n-1; i++)
    {
        si = s->ptr.p_double[i];
        k0 = sparsea->ridx.ptr.p_int[i];
        k1 = sparsea->ridx.ptr.p_int[i+1]-1;
        for(k=k0; k<=k1; k++)
        {
            sparsea->vals.ptr.p_double[k] = sparsea->vals.ptr.p_double[k]*si*s->ptr.p_double[sparsea->idx.ptr.p_int[k]];
        }
        denseb->ptr.p_double[i] = denseb->ptr.p_double[i]*si;
    }
    for(k=0; k<=corrrank-1; k++)
    {
        for(i=0; i<=n-1; i++)
        {
            densecorrc->ptr.pp_double[k][i] = densecorrc->ptr.pp_double[k][i]*s->ptr.p_double[i];
        }
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;
    double vv;


    if( neednorms )
    {
        rvectorsetlengthatleast(rownorms, m, _state);
    }
    for(i=0; i<=m-1; i++)
    {
        vv = 0.0;
        for(j=0; j<=n-1; j++)
        {
            v = densea->ptr.pp_double[i][j];
            vv = vv+v*v;
        }
        vv = ae_sqrt(vv, _state);
        if( neednorms )
        {
            rownorms->ptr.p_double[i] = vv;
        }
        if( ae_fp_greater(vv,(double)(0)) )
        {
            vv = (double)1/vv;
            for(j=0; j<=n-1; j++)
            {
                densea->ptr.pp_double[i][j] = densea->ptr.pp_double[i][j]*vv;
            }
            ab->ptr.p_double[i] = ab->ptr.p_double[i]*vv;
            if( ae_isfinite(ar->ptr.p_double[i], _state) )
            {
                ar->ptr.p_double[i] = ar->ptr.p_double[i]*vv;
            }
        }
    }
}


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
                          we may apply one amplification round to the entire
                          constraint matrix, i.e. amplify all rows by the same
                          coefficient. As result, we do not overamplify
                          any single row, but still make sure than the entire
                          problem is well scaled.
                        * large rows are always normalized
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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t k0;
    ae_int_t k1;
    double v;
    double vv;
    double maxnrm2;


    ae_assert(msparse==0||((sparsea->matrixtype==1&&sparsea->m==msparse)&&sparsea->n==n), "ScaleShiftMixedBRLCInplace: non-CRS sparse constraint matrix!", _state);
    if( neednorms )
    {
        rvectorsetlengthatleast(rownorms, mdense+msparse, _state);
    }
    
    /*
     * First round of normalization - normalize row 2-norms subject to limited amplification status
     */
    maxnrm2 = (double)(0);
    for(i=0; i<=msparse-1; i++)
    {
        vv = 0.0;
        k0 = sparsea->ridx.ptr.p_int[i];
        k1 = sparsea->ridx.ptr.p_int[i+1]-1;
        for(k=k0; k<=k1; k++)
        {
            v = sparsea->vals.ptr.p_double[k];
            vv = vv+v*v;
        }
        vv = ae_sqrt(vv, _state);
        maxnrm2 = ae_maxreal(maxnrm2, vv, _state);
        if( limitedamplification )
        {
            vv = ae_maxreal(vv, 1.0, _state);
        }
        if( neednorms )
        {
            rownorms->ptr.p_double[i] = vv;
        }
        if( ae_fp_greater(vv,(double)(0)) )
        {
            vv = (double)1/vv;
            for(k=k0; k<=k1; k++)
            {
                sparsea->vals.ptr.p_double[k] = sparsea->vals.ptr.p_double[k]*vv;
            }
            ab->ptr.p_double[i] = ab->ptr.p_double[i]*vv;
            if( ae_isfinite(ar->ptr.p_double[i], _state) )
            {
                ar->ptr.p_double[i] = ar->ptr.p_double[i]*vv;
            }
        }
    }
    for(i=0; i<=mdense-1; i++)
    {
        vv = 0.0;
        for(j=0; j<=n-1; j++)
        {
            v = densea->ptr.pp_double[i][j];
            vv = vv+v*v;
        }
        vv = ae_sqrt(vv, _state);
        maxnrm2 = ae_maxreal(maxnrm2, vv, _state);
        if( limitedamplification )
        {
            vv = ae_maxreal(vv, 1.0, _state);
        }
        if( neednorms )
        {
            rownorms->ptr.p_double[msparse+i] = vv;
        }
        if( ae_fp_greater(vv,(double)(0)) )
        {
            vv = (double)1/vv;
            for(j=0; j<=n-1; j++)
            {
                densea->ptr.pp_double[i][j] = densea->ptr.pp_double[i][j]*vv;
            }
            ab->ptr.p_double[msparse+i] = ab->ptr.p_double[msparse+i]*vv;
            if( ae_isfinite(ar->ptr.p_double[msparse+i], _state) )
            {
                ar->ptr.p_double[msparse+i] = ar->ptr.p_double[msparse+i]*vv;
            }
        }
    }
    
    /*
     * If amplification was limited, perform second round of normalization
     */
    if( limitedamplification )
    {
        
        /*
         * Recompute maximum norm
         */
        maxnrm2 = (double)(0);
        for(i=0; i<=msparse-1; i++)
        {
            vv = 0.0;
            k0 = sparsea->ridx.ptr.p_int[i];
            k1 = sparsea->ridx.ptr.p_int[i+1]-1;
            for(k=k0; k<=k1; k++)
            {
                v = sparsea->vals.ptr.p_double[k];
                vv = vv+v*v;
            }
            vv = ae_sqrt(vv, _state);
            maxnrm2 = ae_maxreal(maxnrm2, vv, _state);
        }
        for(i=0; i<=mdense-1; i++)
        {
            maxnrm2 = ae_maxreal(maxnrm2, ae_sqrt(rdotrr(n, densea, i, densea, i, _state), _state), _state);
        }
        
        /*
         * Amplify, if needed 
         */
        if( ae_fp_less(maxnrm2,1.0)&&ae_fp_greater(maxnrm2,(double)(0)) )
        {
            if( neednorms )
            {
                rmulv(mdense+msparse, maxnrm2, rownorms, _state);
            }
            vv = (double)1/maxnrm2;
            for(i=0; i<=msparse-1; i++)
            {
                k0 = sparsea->ridx.ptr.p_int[i];
                k1 = sparsea->ridx.ptr.p_int[i+1]-1;
                for(k=k0; k<=k1; k++)
                {
                    sparsea->vals.ptr.p_double[k] = sparsea->vals.ptr.p_double[k]*vv;
                }
                ab->ptr.p_double[i] = ab->ptr.p_double[i]*vv;
                if( ae_isfinite(ar->ptr.p_double[i], _state) )
                {
                    ar->ptr.p_double[i] = ar->ptr.p_double[i]*vv;
                }
            }
            for(i=0; i<=mdense-1; i++)
            {
                rmulr(n, vv, densea, i, _state);
                ab->ptr.p_double[msparse+i] = ab->ptr.p_double[msparse+i]*vv;
                if( ae_isfinite(ar->ptr.p_double[msparse+i], _state) )
                {
                    ar->ptr.p_double[msparse+i] = ar->ptr.p_double[msparse+i]*vv;
                }
            }
        }
    }
}


/*************************************************************************
This function normalizes two-sided  "lower-bound/upper-bound"  constraints
stored in dense  format in such a way that L2 norms of  rows  (right  hand
side NOT included) become equal to 1.0. 

Exactly zero rows are handled correctly.

INPUT PARAMETERS:
    DenseA          -   dense M*N constraint matrix;
                        ignored if M=0.
    M               -   constraint count, M>=0
    AL              -   lower bounds for constraints, array[M]
    AU              -   upper bounds for constraints, array[M]
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
                        * large rows are always normalized
    NeedNorms       -   whether we need row norms or not

OUTPUT PARAMETERS:
    SparseA         -   replaced by normalized constraints
    AL              -   replaced by normalized lower bounds
    AU              -   replaced by normalized upper bounds
    RowNorms        -   if NeedNorms is true, leading M elements (resized
                        if length is less than M) are filled by row norms
                        before normalization was performed.
    

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void normalizedenselcinplace(/* Real    */ ae_matrix* densea,
     ae_int_t m,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     ae_int_t n,
     ae_bool limitedamplification,
     /* Real    */ ae_vector* rownorms,
     ae_bool neednorms,
     ae_state *_state)
{
    ae_int_t i;
    double vv;
    double maxnrm2;


    ae_assert(m==0||(densea->rows>=m&&densea->cols>=n), "NormalizeDenseLCInplace: matrix size is too small", _state);
    if( neednorms )
    {
        rallocv(m, rownorms, _state);
    }
    
    /*
     * First round of normalization - normalize row 2-norms subject to limited amplification status
     */
    for(i=0; i<=m-1; i++)
    {
        vv = ae_sqrt(rdotrr(n, densea, i, densea, i, _state), _state);
        if( limitedamplification )
        {
            vv = ae_maxreal(vv, 1.0, _state);
        }
        if( neednorms )
        {
            rownorms->ptr.p_double[i] = vv;
        }
        if( ae_fp_greater(vv,(double)(0)) )
        {
            vv = (double)1/vv;
            rmulr(n, vv, densea, i, _state);
            al->ptr.p_double[i] = al->ptr.p_double[i]*vv;
            au->ptr.p_double[i] = au->ptr.p_double[i]*vv;
        }
    }
    
    /*
     * If amplification was limited, perform second round of normalization
     */
    if( limitedamplification )
    {
        maxnrm2 = (double)(0);
        for(i=0; i<=m-1; i++)
        {
            maxnrm2 = ae_maxreal(maxnrm2, ae_sqrt(rdotrr(n, densea, i, densea, i, _state), _state), _state);
        }
        if( ae_fp_less(maxnrm2,1.0)&&ae_fp_greater(maxnrm2,(double)(0)) )
        {
            if( neednorms )
            {
                rmulv(m, maxnrm2, rownorms, _state);
            }
            vv = (double)1/maxnrm2;
            for(i=0; i<=m-1; i++)
            {
                rmulr(n, vv, densea, i, _state);
                al->ptr.p_double[i] = al->ptr.p_double[i]*vv;
                au->ptr.p_double[i] = au->ptr.p_double[i]*vv;
            }
        }
    }
}


/*************************************************************************
This function normalizes two-sided  "lower-bound/upper-bound"  constraints
stored in sparse format in such a way that L2 norms of  rows  (right  hand
side NOT included) become equal to 1.0. 

Exactly zero rows are handled correctly.

INPUT PARAMETERS:
    SparseA         -   sparse M*N constraint matrix in CRS format;
                        ignored if M=0.
    M               -   constraint count, M>=0
    AL              -   lower bounds for constraints, array[M]
    AU              -   upper bounds for constraints, array[M]
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
                        * large rows are always normalized
    NeedNorms       -   whether we need row norms or not

OUTPUT PARAMETERS:
    SparseA         -   replaced by normalized constraints
    AL              -   replaced by normalized lower bounds
    AU              -   replaced by normalized upper bounds
    RowNorms        -   if NeedNorms is true, leading M elements (resized
                        if length is less than M) are filled by row norms
                        before normalization was performed.
    

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void normalizesparselcinplace(sparsematrix* sparsea,
     ae_int_t m,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     ae_int_t n,
     ae_bool limitedamplification,
     /* Real    */ ae_vector* rownorms,
     ae_bool neednorms,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;
    ae_int_t k0;
    ae_int_t k1;
    double v;
    double vv;
    double maxnrm2;


    ae_assert(m==0||((sparsea->matrixtype==1&&sparsea->m==m)&&sparsea->n==n), "ScaleShiftMixedBRLCInplace: non-CRS sparse constraint matrix!", _state);
    if( neednorms )
    {
        rallocv(m, rownorms, _state);
    }
    
    /*
     * First round of normalization - normalize row 2-norms subject to limited amplification status
     */
    maxnrm2 = (double)(0);
    for(i=0; i<=m-1; i++)
    {
        vv = 0.0;
        k0 = sparsea->ridx.ptr.p_int[i];
        k1 = sparsea->ridx.ptr.p_int[i+1]-1;
        for(k=k0; k<=k1; k++)
        {
            v = sparsea->vals.ptr.p_double[k];
            vv = vv+v*v;
        }
        vv = ae_sqrt(vv, _state);
        maxnrm2 = ae_maxreal(maxnrm2, vv, _state);
        if( limitedamplification )
        {
            vv = ae_maxreal(vv, 1.0, _state);
        }
        if( neednorms )
        {
            rownorms->ptr.p_double[i] = vv;
        }
        if( ae_fp_greater(vv,(double)(0)) )
        {
            vv = (double)1/vv;
            for(k=k0; k<=k1; k++)
            {
                sparsea->vals.ptr.p_double[k] = sparsea->vals.ptr.p_double[k]*vv;
            }
            al->ptr.p_double[i] = al->ptr.p_double[i]*vv;
            au->ptr.p_double[i] = au->ptr.p_double[i]*vv;
        }
    }
    
    /*
     * If amplification was limited, perform second round of normalization
     */
    if( limitedamplification )
    {
        
        /*
         * Recompute maximum row norm
         */
        maxnrm2 = (double)(0);
        for(i=0; i<=m-1; i++)
        {
            vv = 0.0;
            k0 = sparsea->ridx.ptr.p_int[i];
            k1 = sparsea->ridx.ptr.p_int[i+1]-1;
            for(k=k0; k<=k1; k++)
            {
                v = sparsea->vals.ptr.p_double[k];
                vv = vv+v*v;
            }
            vv = ae_sqrt(vv, _state);
            maxnrm2 = ae_maxreal(maxnrm2, vv, _state);
        }
        
        /*
         * Perform limited amplification, if needed
         */
        if( ae_fp_less(maxnrm2,1.0)&&ae_fp_greater(maxnrm2,(double)(0)) )
        {
            if( neednorms )
            {
                rmulv(m, maxnrm2, rownorms, _state);
            }
            vv = (double)1/maxnrm2;
            for(i=0; i<=m-1; i++)
            {
                k0 = sparsea->ridx.ptr.p_int[i];
                k1 = sparsea->ridx.ptr.p_int[i+1]-1;
                for(k=k0; k<=k1; k++)
                {
                    sparsea->vals.ptr.p_double[k] = sparsea->vals.ptr.p_double[k]*vv;
                }
                al->ptr.p_double[i] = al->ptr.p_double[i]*vv;
                au->ptr.p_double[i] = au->ptr.p_double[i]*vv;
            }
        }
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t j0;
    ae_int_t j1;
    double mx;
    double v;
    double result;


    mx = (double)(0);
    for(i=0; i<=nmain-1; i++)
    {
        if( isupper )
        {
            j0 = i;
            j1 = nmain-1;
        }
        else
        {
            j0 = 0;
            j1 = i;
        }
        for(j=j0; j<=j1; j++)
        {
            mx = ae_maxreal(mx, ae_fabs(densea->ptr.pp_double[i][j], _state), _state);
        }
    }
    for(i=0; i<=ntotal-1; i++)
    {
        mx = ae_maxreal(mx, ae_fabs(denseb->ptr.p_double[i], _state), _state);
    }
    result = mx;
    if( ae_fp_eq(mx,(double)(0)) )
    {
        return result;
    }
    v = (double)1/mx;
    for(i=0; i<=nmain-1; i++)
    {
        if( isupper )
        {
            j0 = i;
            j1 = nmain-1;
        }
        else
        {
            j0 = 0;
            j1 = i;
        }
        for(j=j0; j<=j1; j++)
        {
            densea->ptr.pp_double[i][j] = densea->ptr.pp_double[i][j]*v;
        }
    }
    for(i=0; i<=ntotal-1; i++)
    {
        denseb->ptr.p_double[i] = denseb->ptr.p_double[i]*v;
    }
    return result;
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;
    ae_int_t k0;
    ae_int_t k1;
    double mx;
    double v;
    double result;


    ae_assert((sparsea->matrixtype==1&&sparsea->m==n)&&sparsea->n==n, "NormalizeSparseQPInplace: SparseA in unexpected format", _state);
    mx = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(sparsea->didx.ptr.p_int[i]+1==sparsea->uidx.ptr.p_int[i], "NormalizeSparseQPInplace: critical integrity check failed, sparse diagonal not found", _state);
        v = sparsea->vals.ptr.p_double[sparsea->didx.ptr.p_int[i]];
        for(k=0; k<=corrrank-1; k++)
        {
            v = v+densecorrd->ptr.p_double[k]*ae_sqr(densecorrc->ptr.pp_double[k][i], _state);
        }
        mx = ae_maxreal(mx, ae_fabs(v, _state), _state);
        mx = ae_maxreal(mx, ae_fabs(denseb->ptr.p_double[i], _state), _state);
    }
    result = mx;
    if( ae_fp_eq(mx,(double)(0)) )
    {
        return result;
    }
    v = (double)1/mx;
    for(i=0; i<=n-1; i++)
    {
        k0 = sparsea->ridx.ptr.p_int[i];
        k1 = sparsea->ridx.ptr.p_int[i+1]-1;
        for(k=k0; k<=k1; k++)
        {
            sparsea->vals.ptr.p_double[k] = sparsea->vals.ptr.p_double[k]*v;
        }
        denseb->ptr.p_double[i] = denseb->ptr.p_double[i]*v;
    }
    for(k=0; k<=corrrank-1; k++)
    {
        rmulr(n, ae_sqrt(v, _state), densecorrc, k, _state);
    }
    return result;
}


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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=n-1; i++)
    {
        if( hasbndl->ptr.p_bool[i]&&ae_fp_less_eq(x->ptr.p_double[i],sclsftbndl->ptr.p_double[i]) )
        {
            x->ptr.p_double[i] = rawbndl->ptr.p_double[i];
            continue;
        }
        if( hasbndu->ptr.p_bool[i]&&ae_fp_greater_eq(x->ptr.p_double[i],sclsftbndu->ptr.p_double[i]) )
        {
            x->ptr.p_double[i] = rawbndu->ptr.p_double[i];
            continue;
        }
        x->ptr.p_double[i] = x->ptr.p_double[i]*s->ptr.p_double[i]+xorigin->ptr.p_double[i];
        if( hasbndl->ptr.p_bool[i]&&ae_fp_less_eq(x->ptr.p_double[i],rawbndl->ptr.p_double[i]) )
        {
            x->ptr.p_double[i] = rawbndl->ptr.p_double[i];
        }
        if( hasbndu->ptr.p_bool[i]&&ae_fp_greater_eq(x->ptr.p_double[i],rawbndu->ptr.p_double[i]) )
        {
            x->ptr.p_double[i] = rawbndu->ptr.p_double[i];
        }
    }
}


/*$ End $*/
