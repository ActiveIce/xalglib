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
#include "sptrf.h"


/*$ Declarations $*/
static double sptrf_densebnd = 0.10;
static ae_int_t sptrf_slswidth = 8;
static void sptrf_sluv2list1init(ae_int_t n,
     sluv2list1matrix* a,
     ae_state *_state);
static void sptrf_sluv2list1swap(sluv2list1matrix* a,
     ae_int_t i,
     ae_int_t j,
     ae_state *_state);
static void sptrf_sluv2list1dropsequence(sluv2list1matrix* a,
     ae_int_t i,
     ae_state *_state);
static void sptrf_sluv2list1appendsequencetomatrix(const sluv2list1matrix* a,
     ae_int_t src,
     ae_bool hasdiagonal,
     double d,
     ae_int_t nzmax,
     sparsematrix* s,
     ae_int_t dst,
     ae_state *_state);
static void sptrf_sluv2list1pushsparsevector(sluv2list1matrix* a,
     /* Integer */ const ae_vector* si,
     /* Real    */ const ae_vector* sv,
     ae_int_t nz,
     ae_state *_state);
static void sptrf_densetrailinit(sluv2densetrail* d,
     ae_int_t n,
     ae_state *_state);
static void sptrf_densetrailappendcolumn(sluv2densetrail* d,
     /* Real    */ const ae_vector* x,
     ae_int_t id,
     ae_state *_state);
static void sptrf_sparsetrailinit(const sparsematrix* s,
     sluv2sparsetrail* a,
     ae_state *_state);
static ae_bool sptrf_sparsetrailfindpivot(sluv2sparsetrail* a,
     ae_int_t pivottype,
     ae_int_t* ipiv,
     ae_int_t* jpiv,
     ae_state *_state);
static void sptrf_sparsetrailpivotout(sluv2sparsetrail* a,
     ae_int_t ipiv,
     ae_int_t jpiv,
     double* uu,
     /* Integer */ ae_vector* v0i,
     /* Real    */ ae_vector* v0r,
     ae_int_t* nz0,
     /* Integer */ ae_vector* v1i,
     /* Real    */ ae_vector* v1r,
     ae_int_t* nz1,
     ae_state *_state);
static void sptrf_sparsetraildensify(sluv2sparsetrail* a,
     ae_int_t i1,
     sluv2list1matrix* bupper,
     sluv2densetrail* dtrail,
     ae_state *_state);
static void sptrf_sparsetrailupdate(sluv2sparsetrail* a,
     /* Integer */ ae_vector* v0i,
     /* Real    */ ae_vector* v0r,
     ae_int_t nz0,
     /* Integer */ const ae_vector* v1i,
     /* Real    */ const ae_vector* v1r,
     ae_int_t nz1,
     sluv2list1matrix* bupper,
     sluv2densetrail* dtrail,
     ae_bool densificationsupported,
     ae_state *_state);


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
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t k;
    ae_int_t i;
    ae_int_t j;
    ae_int_t jp;
    ae_int_t i0;
    ae_int_t i1;
    ae_int_t ibest;
    ae_int_t jbest;
    double v;
    double v0;
    ae_int_t nz0;
    ae_int_t nz1;
    double uu;
    ae_int_t offs;
    ae_int_t tmpndense;
    ae_bool densificationsupported;
    ae_int_t densifyabove;
    ae_bool result;


    ae_assert(sparseiscrs(a, _state), "SparseLU: A is not stored in CRS format", _state);
    ae_assert(sparsegetnrows(a, _state)==sparsegetncols(a, _state), "SparseLU: non-square A", _state);
    ae_assert((pivottype==0||pivottype==1)||pivottype==2, "SparseLU: unexpected pivot type", _state);
    result = ae_true;
    n = sparsegetnrows(a, _state);
    if( pivottype==0 )
    {
        pivottype = 2;
    }
    densificationsupported = pivottype==2;
    
    /*
     *
     */
    buf->n = n;
    ivectorsetlengthatleast(&buf->rowpermrawidx, n, _state);
    for(i=0; i<=n-1; i++)
    {
        buf->rowpermrawidx.ptr.p_int[i] = i;
    }
    
    /*
     * Allocate storage for sparse L and U factors
     *
     * NOTE: SparseMatrix structure for these factors is only
     *       partially initialized; we use it just as a temporary
     *       storage and do not intend to use facilities of the
     *       'sparse' subpackage to work with these objects.
     */
    buf->sparsel.matrixtype = 1;
    buf->sparsel.m = n;
    buf->sparsel.n = n;
    ivectorsetlengthatleast(&buf->sparsel.ridx, n+1, _state);
    buf->sparsel.ridx.ptr.p_int[0] = 0;
    buf->sparseut.matrixtype = 1;
    buf->sparseut.m = n;
    buf->sparseut.n = n;
    ivectorsetlengthatleast(&buf->sparseut.ridx, n+1, _state);
    buf->sparseut.ridx.ptr.p_int[0] = 0;
    
    /*
     * Allocate unprocessed yet part of the matrix,
     * two submatrices:
     * * BU, upper J rows of columns [J,N), upper submatrix
     * * BL, left J  cols of rows [J,N), left submatrix
     * * B1, (N-J)*(N-J) square submatrix
     */
    sptrf_sluv2list1init(n, &buf->bleft, _state);
    sptrf_sluv2list1init(n, &buf->bupper, _state);
    ivectorsetlengthatleast(pr, n, _state);
    ivectorsetlengthatleast(pc, n, _state);
    ivectorsetlengthatleast(&buf->v0i, n, _state);
    ivectorsetlengthatleast(&buf->v1i, n, _state);
    rvectorsetlengthatleast(&buf->v0r, n, _state);
    rvectorsetlengthatleast(&buf->v1r, n, _state);
    sptrf_sparsetrailinit(a, &buf->strail, _state);
    
    /*
     * Prepare dense trail, initial densification
     */
    sptrf_densetrailinit(&buf->dtrail, n, _state);
    densifyabove = ae_round(sptrf_densebnd*(double)n, _state)+1;
    if( densificationsupported )
    {
        for(i=0; i<=n-1; i++)
        {
            if( buf->strail.nzc.ptr.p_int[i]>densifyabove )
            {
                sptrf_sparsetraildensify(&buf->strail, i, &buf->bupper, &buf->dtrail, _state);
            }
        }
    }
    
    /*
     * Process sparse part
     */
    for(k=0; k<=n-1; k++)
    {
        
        /*
         * Find pivot column and pivot row
         */
        if( !sptrf_sparsetrailfindpivot(&buf->strail, pivottype, &ibest, &jbest, _state) )
        {
            
            /*
             * Only densified columns are left, break sparse iteration
             */
            ae_assert(buf->dtrail.ndense+k==n, "SPTRF: integrity check failed (35741)", _state);
            break;
        }
        pc->ptr.p_int[k] = jbest;
        pr->ptr.p_int[k] = ibest;
        j = buf->rowpermrawidx.ptr.p_int[k];
        buf->rowpermrawidx.ptr.p_int[k] = buf->rowpermrawidx.ptr.p_int[ibest];
        buf->rowpermrawidx.ptr.p_int[ibest] = j;
        
        /*
         * Apply pivoting to BL and BU
         */
        sptrf_sluv2list1swap(&buf->bleft, k, ibest, _state);
        sptrf_sluv2list1swap(&buf->bupper, k, jbest, _state);
        
        /*
         * Apply pivoting to sparse trail, pivot out
         */
        sptrf_sparsetrailpivotout(&buf->strail, ibest, jbest, &uu, &buf->v0i, &buf->v0r, &nz0, &buf->v1i, &buf->v1r, &nz1, _state);
        result = result&&uu!=(double)0;
        
        /*
         * Pivot dense trail
         */
        tmpndense = buf->dtrail.ndense;
        for(i=0; i<=tmpndense-1; i++)
        {
            v = buf->dtrail.d.ptr.pp_double[k][i];
            buf->dtrail.d.ptr.pp_double[k][i] = buf->dtrail.d.ptr.pp_double[ibest][i];
            buf->dtrail.d.ptr.pp_double[ibest][i] = v;
        }
        
        /*
         * Output to LU matrix
         */
        sptrf_sluv2list1appendsequencetomatrix(&buf->bupper, k, ae_true, uu, n, &buf->sparseut, k, _state);
        sptrf_sluv2list1appendsequencetomatrix(&buf->bleft, k, ae_false, 0.0, n, &buf->sparsel, k, _state);
        
        /*
         * Extract K-th col/row of B1, generate K-th col/row of BL/BU, update NZC
         */
        sptrf_sluv2list1pushsparsevector(&buf->bleft, &buf->v0i, &buf->v0r, nz0, _state);
        sptrf_sluv2list1pushsparsevector(&buf->bupper, &buf->v1i, &buf->v1r, nz1, _state);
        
        /*
         * Update the rest of the matrix
         */
        if( nz0*(nz1+buf->dtrail.ndense)>0 )
        {
            
            /*
             * Update dense trail
             *
             * NOTE: this update MUST be performed before we update sparse trail,
             *       because sparse update may move columns to dense storage after
             *       update is performed on them. Thus, we have to avoid applying
             *       same update twice.
             */
            if( buf->dtrail.ndense>0 )
            {
                tmpndense = buf->dtrail.ndense;
                for(i=0; i<=nz0-1; i++)
                {
                    i0 = buf->v0i.ptr.p_int[i];
                    v0 = buf->v0r.ptr.p_double[i];
                    for(j=0; j<=tmpndense-1; j++)
                    {
                        buf->dtrail.d.ptr.pp_double[i0][j] = buf->dtrail.d.ptr.pp_double[i0][j]-v0*buf->dtrail.d.ptr.pp_double[k][j];
                    }
                }
            }
            
            /*
             * Update sparse trail
             */
            sptrf_sparsetrailupdate(&buf->strail, &buf->v0i, &buf->v0r, nz0, &buf->v1i, &buf->v1r, nz1, &buf->bupper, &buf->dtrail, densificationsupported, _state);
        }
    }
    
    /*
     * Process densified trail
     */
    if( buf->dtrail.ndense>0 )
    {
        tmpndense = buf->dtrail.ndense;
        
        /*
         * Generate column pivots to bring actual order of columns in the
         * working part of the matrix to one used for dense storage
         */
        for(i=n-tmpndense; i<=n-1; i++)
        {
            k = buf->dtrail.did.ptr.p_int[i-(n-tmpndense)];
            jp = -1;
            for(j=i; j<=n-1; j++)
            {
                if( buf->strail.colid.ptr.p_int[j]==k )
                {
                    jp = j;
                    break;
                }
            }
            ae_assert(jp>=0, "SPTRF: integrity check failed during reordering", _state);
            k = buf->strail.colid.ptr.p_int[i];
            buf->strail.colid.ptr.p_int[i] = buf->strail.colid.ptr.p_int[jp];
            buf->strail.colid.ptr.p_int[jp] = k;
            pc->ptr.p_int[i] = jp;
        }
        
        /*
         * Perform dense LU decomposition on dense trail
         */
        rmatrixsetlengthatleast(&buf->dbuf, buf->dtrail.ndense, buf->dtrail.ndense, _state);
        for(i=0; i<=tmpndense-1; i++)
        {
            for(j=0; j<=tmpndense-1; j++)
            {
                buf->dbuf.ptr.pp_double[i][j] = buf->dtrail.d.ptr.pp_double[i+(n-tmpndense)][j];
            }
        }
        rvectorsetlengthatleast(&buf->tmp0, 2*n, _state);
        ivectorsetlengthatleast(&buf->tmpp, n, _state);
        rmatrixplurec(&buf->dbuf, 0, tmpndense, tmpndense, &buf->tmpp, &buf->tmp0, _state);
        
        /*
         * Convert indexes of rows pivots, swap elements of BLeft
         */
        for(i=0; i<=tmpndense-1; i++)
        {
            pr->ptr.p_int[i+(n-tmpndense)] = buf->tmpp.ptr.p_int[i]+(n-tmpndense);
            sptrf_sluv2list1swap(&buf->bleft, i+(n-tmpndense), pr->ptr.p_int[i+(n-tmpndense)], _state);
            j = buf->rowpermrawidx.ptr.p_int[i+(n-tmpndense)];
            buf->rowpermrawidx.ptr.p_int[i+(n-tmpndense)] = buf->rowpermrawidx.ptr.p_int[pr->ptr.p_int[i+(n-tmpndense)]];
            buf->rowpermrawidx.ptr.p_int[pr->ptr.p_int[i+(n-tmpndense)]] = j;
        }
        
        /*
         * Convert U-factor
         */
        ivectorgrowto(&buf->sparseut.idx, buf->sparseut.ridx.ptr.p_int[n-tmpndense]+n*tmpndense, _state);
        rvectorgrowto(&buf->sparseut.vals, buf->sparseut.ridx.ptr.p_int[n-tmpndense]+n*tmpndense, _state);
        for(j=0; j<=tmpndense-1; j++)
        {
            offs = buf->sparseut.ridx.ptr.p_int[j+(n-tmpndense)];
            k = n-tmpndense;
            
            /*
             * Convert leading N-NDense columns
             */
            for(i=0; i<=k-1; i++)
            {
                v = buf->dtrail.d.ptr.pp_double[i][j];
                if( v!=(double)0 )
                {
                    buf->sparseut.idx.ptr.p_int[offs] = i;
                    buf->sparseut.vals.ptr.p_double[offs] = v;
                    offs = offs+1;
                }
            }
            
            /*
             * Convert upper diagonal elements
             */
            for(i=0; i<=j-1; i++)
            {
                v = buf->dbuf.ptr.pp_double[i][j];
                if( v!=(double)0 )
                {
                    buf->sparseut.idx.ptr.p_int[offs] = i+(n-tmpndense);
                    buf->sparseut.vals.ptr.p_double[offs] = v;
                    offs = offs+1;
                }
            }
            
            /*
             * Convert diagonal element (always stored)
             */
            v = buf->dbuf.ptr.pp_double[j][j];
            buf->sparseut.idx.ptr.p_int[offs] = j+(n-tmpndense);
            buf->sparseut.vals.ptr.p_double[offs] = v;
            offs = offs+1;
            result = result&&v!=(double)0;
            
            /*
             * Column is done
             */
            buf->sparseut.ridx.ptr.p_int[j+(n-tmpndense)+1] = offs;
        }
        
        /*
         * Convert L-factor
         */
        ivectorgrowto(&buf->sparsel.idx, buf->sparsel.ridx.ptr.p_int[n-tmpndense]+n*tmpndense, _state);
        rvectorgrowto(&buf->sparsel.vals, buf->sparsel.ridx.ptr.p_int[n-tmpndense]+n*tmpndense, _state);
        for(i=0; i<=tmpndense-1; i++)
        {
            sptrf_sluv2list1appendsequencetomatrix(&buf->bleft, i+(n-tmpndense), ae_false, 0.0, n, &buf->sparsel, i+(n-tmpndense), _state);
            offs = buf->sparsel.ridx.ptr.p_int[i+(n-tmpndense)+1];
            for(j=0; j<=i-1; j++)
            {
                v = buf->dbuf.ptr.pp_double[i][j];
                if( v!=(double)0 )
                {
                    buf->sparsel.idx.ptr.p_int[offs] = j+(n-tmpndense);
                    buf->sparsel.vals.ptr.p_double[offs] = v;
                    offs = offs+1;
                }
            }
            buf->sparsel.ridx.ptr.p_int[i+(n-tmpndense)+1] = offs;
        }
    }
    
    /*
     * Allocate output
     */
    ivectorsetlengthatleast(&buf->tmpi, n, _state);
    for(i=0; i<=n-1; i++)
    {
        buf->tmpi.ptr.p_int[i] = buf->sparsel.ridx.ptr.p_int[i+1]-buf->sparsel.ridx.ptr.p_int[i];
    }
    for(i=0; i<=n-1; i++)
    {
        i0 = buf->sparseut.ridx.ptr.p_int[i];
        i1 = buf->sparseut.ridx.ptr.p_int[i+1]-1;
        for(j=i0; j<=i1; j++)
        {
            k = buf->sparseut.idx.ptr.p_int[j];
            buf->tmpi.ptr.p_int[k] = buf->tmpi.ptr.p_int[k]+1;
        }
    }
    a->matrixtype = 1;
    a->ninitialized = buf->sparsel.ridx.ptr.p_int[n]+buf->sparseut.ridx.ptr.p_int[n];
    a->m = n;
    a->n = n;
    ivectorsetlengthatleast(&a->ridx, n+1, _state);
    ivectorsetlengthatleast(&a->idx, a->ninitialized, _state);
    rvectorsetlengthatleast(&a->vals, a->ninitialized, _state);
    a->ridx.ptr.p_int[0] = 0;
    for(i=0; i<=n-1; i++)
    {
        a->ridx.ptr.p_int[i+1] = a->ridx.ptr.p_int[i]+buf->tmpi.ptr.p_int[i];
    }
    for(i=0; i<=n-1; i++)
    {
        i0 = buf->sparsel.ridx.ptr.p_int[i];
        i1 = buf->sparsel.ridx.ptr.p_int[i+1]-1;
        jp = a->ridx.ptr.p_int[i];
        for(j=i0; j<=i1; j++)
        {
            a->idx.ptr.p_int[jp+(j-i0)] = buf->sparsel.idx.ptr.p_int[j];
            a->vals.ptr.p_double[jp+(j-i0)] = buf->sparsel.vals.ptr.p_double[j];
        }
        buf->tmpi.ptr.p_int[i] = buf->sparsel.ridx.ptr.p_int[i+1]-buf->sparsel.ridx.ptr.p_int[i];
    }
    ivectorsetlengthatleast(&a->didx, n, _state);
    ivectorsetlengthatleast(&a->uidx, n, _state);
    for(i=0; i<=n-1; i++)
    {
        a->didx.ptr.p_int[i] = a->ridx.ptr.p_int[i]+buf->tmpi.ptr.p_int[i];
        a->uidx.ptr.p_int[i] = a->didx.ptr.p_int[i]+1;
        buf->tmpi.ptr.p_int[i] = a->didx.ptr.p_int[i];
    }
    for(i=0; i<=n-1; i++)
    {
        i0 = buf->sparseut.ridx.ptr.p_int[i];
        i1 = buf->sparseut.ridx.ptr.p_int[i+1]-1;
        for(j=i0; j<=i1; j++)
        {
            k = buf->sparseut.idx.ptr.p_int[j];
            offs = buf->tmpi.ptr.p_int[k];
            a->idx.ptr.p_int[offs] = i;
            a->vals.ptr.p_double[offs] = buf->sparseut.vals.ptr.p_double[j];
            buf->tmpi.ptr.p_int[k] = offs+1;
        }
    }
    return result;
}


/*************************************************************************
This function initialized rectangular submatrix structure.

After initialization this structure stores  matrix[N,0],  which contains N
rows (sequences), stored as single-linked lists.

  -- ALGLIB routine --
     15.01.2019
     Bochkanov Sergey
*************************************************************************/
static void sptrf_sluv2list1init(ae_int_t n,
     sluv2list1matrix* a,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(n>=1, "SLUV2List1Init: N<1", _state);
    a->nfixed = n;
    a->ndynamic = 0;
    a->nallocated = n;
    a->nused = 0;
    ivectorgrowto(&a->idxfirst, n, _state);
    ivectorgrowto(&a->strgidx, 2*a->nallocated, _state);
    rvectorgrowto(&a->strgval, a->nallocated, _state);
    for(i=0; i<=n-1; i++)
    {
        a->idxfirst.ptr.p_int[i] = -1;
    }
}


/*************************************************************************
This function swaps sequences #I and #J stored by the structure

  -- ALGLIB routine --
     15.01.2019
     Bochkanov Sergey
*************************************************************************/
static void sptrf_sluv2list1swap(sluv2list1matrix* a,
     ae_int_t i,
     ae_int_t j,
     ae_state *_state)
{
    ae_int_t k;


    k = a->idxfirst.ptr.p_int[i];
    a->idxfirst.ptr.p_int[i] = a->idxfirst.ptr.p_int[j];
    a->idxfirst.ptr.p_int[j] = k;
}


/*************************************************************************
This function drops sequence #I from the structure

  -- ALGLIB routine --
     15.01.2019
     Bochkanov Sergey
*************************************************************************/
static void sptrf_sluv2list1dropsequence(sluv2list1matrix* a,
     ae_int_t i,
     ae_state *_state)
{


    a->idxfirst.ptr.p_int[i] = -1;
}


/*************************************************************************
This function appends sequence from the structure to the sparse matrix.

It is assumed that S is a lower triangular  matrix,  and A stores strictly
lower triangular elements (no diagonal ones!). You can explicitly  control
whether you want to add diagonal elements or not.

Output matrix is assumed to be stored in CRS format and  to  be  partially
initialized (up to, but not including, Dst-th row). DIdx and UIdx are  NOT
updated by this function as well as NInitialized.

INPUT PARAMETERS:
    A           -   rectangular matrix structure
    Src         -   sequence (row or column) index in the structure
    HasDiagonal -   whether we want to add diagonal element
    D           -   diagonal element, if HasDiagonal=True
    NZMAX       -   maximum estimated number of non-zeros in the row,
                    this function will preallocate storage in the output
                    matrix.
    S           -   destination matrix in CRS format, partially initialized
    Dst         -   destination row index


  -- ALGLIB routine --
     15.01.2019
     Bochkanov Sergey
*************************************************************************/
static void sptrf_sluv2list1appendsequencetomatrix(const sluv2list1matrix* a,
     ae_int_t src,
     ae_bool hasdiagonal,
     double d,
     ae_int_t nzmax,
     sparsematrix* s,
     ae_int_t dst,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t i0;
    ae_int_t i1;
    ae_int_t jp;
    ae_int_t nnz;


    i0 = s->ridx.ptr.p_int[dst];
    ivectorgrowto(&s->idx, i0+nzmax, _state);
    rvectorgrowto(&s->vals, i0+nzmax, _state);
    if( hasdiagonal )
    {
        i1 = i0+nzmax-1;
        s->idx.ptr.p_int[i1] = dst;
        s->vals.ptr.p_double[i1] = d;
        nnz = 1;
    }
    else
    {
        i1 = i0+nzmax;
        nnz = 0;
    }
    jp = a->idxfirst.ptr.p_int[src];
    while(jp>=0)
    {
        i1 = i1-1;
        s->idx.ptr.p_int[i1] = a->strgidx.ptr.p_int[2*jp+1];
        s->vals.ptr.p_double[i1] = a->strgval.ptr.p_double[jp];
        nnz = nnz+1;
        jp = a->strgidx.ptr.p_int[2*jp+0];
    }
    for(i=0; i<=nnz-1; i++)
    {
        s->idx.ptr.p_int[i0+i] = s->idx.ptr.p_int[i1+i];
        s->vals.ptr.p_double[i0+i] = s->vals.ptr.p_double[i1+i];
    }
    s->ridx.ptr.p_int[dst+1] = s->ridx.ptr.p_int[dst]+nnz;
}


/*************************************************************************
This function appends sparse column to the  matrix,  increasing  its  size
from [N,K] to [N,K+1]

  -- ALGLIB routine --
     15.01.2019
     Bochkanov Sergey
*************************************************************************/
static void sptrf_sluv2list1pushsparsevector(sluv2list1matrix* a,
     /* Integer */ const ae_vector* si,
     /* Real    */ const ae_vector* sv,
     ae_int_t nz,
     ae_state *_state)
{
    ae_int_t idx;
    ae_int_t i;
    ae_int_t k;
    ae_int_t nused;
    double v;


    
    /*
     * Fetch matrix size, increase
     */
    k = a->ndynamic;
    ae_assert(k<a->nfixed, "Assertion failed", _state);
    a->ndynamic = k+1;
    
    /*
     * Allocate new storage if needed
     */
    nused = a->nused;
    a->nallocated = ae_maxint(a->nallocated, nused+nz, _state);
    ivectorgrowto(&a->strgidx, 2*a->nallocated, _state);
    rvectorgrowto(&a->strgval, a->nallocated, _state);
    
    /*
     * Append to list
     */
    for(idx=0; idx<=nz-1; idx++)
    {
        i = si->ptr.p_int[idx];
        v = sv->ptr.p_double[idx];
        a->strgidx.ptr.p_int[2*nused+0] = a->idxfirst.ptr.p_int[i];
        a->strgidx.ptr.p_int[2*nused+1] = k;
        a->strgval.ptr.p_double[nused] = v;
        a->idxfirst.ptr.p_int[i] = nused;
        nused = nused+1;
    }
    a->nused = nused;
}


/*************************************************************************
This function initializes dense trail, by default it is matrix[N,0]

  -- ALGLIB routine --
     15.01.2019
     Bochkanov Sergey
*************************************************************************/
static void sptrf_densetrailinit(sluv2densetrail* d,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t excessivesize;


    
    /*
     * Note: excessive rows are allocated to accomodate for situation when
     *       this buffer is used to solve successive problems with increasing
     *       sizes.
     */
    excessivesize = ae_maxint(ae_round(1.333*(double)n, _state), n, _state);
    d->n = n;
    d->ndense = 0;
    ivectorsetlengthatleast(&d->did, n, _state);
    if( d->d.rows<=excessivesize )
    {
        rmatrixsetlengthatleast(&d->d, n, 1, _state);
    }
    else
    {
        ae_matrix_set_length(&d->d, excessivesize, 1, _state);
    }
}


/*************************************************************************
This function appends column with id=ID to the dense trail (column IDs are
integer numbers in [0,N) which can be used to track column permutations).

  -- ALGLIB routine --
     15.01.2019
     Bochkanov Sergey
*************************************************************************/
static void sptrf_densetrailappendcolumn(sluv2densetrail* d,
     /* Real    */ const ae_vector* x,
     ae_int_t id,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t targetidx;


    n = d->n;
    
    /*
     * Reallocate storage
     */
    rmatrixgrowcolsto(&d->d, d->ndense+1, n, _state);
    
    /*
     * Copy to dense storage:
     * * BUpper
     * * BTrail
     * Remove from sparse storage
     */
    targetidx = d->ndense;
    for(i=0; i<=n-1; i++)
    {
        d->d.ptr.pp_double[i][targetidx] = x->ptr.p_double[i];
    }
    d->did.ptr.p_int[targetidx] = id;
    d->ndense = targetidx+1;
}


/*************************************************************************
This function initializes sparse trail from the sparse matrix. By default,
sparse trail spans columns and rows in [0,N)  range.  Subsequent  pivoting
out of rows/columns changes its range to [K,N), [K+1,N) and so on.

  -- ALGLIB routine --
     15.01.2019
     Bochkanov Sergey
*************************************************************************/
static void sptrf_sparsetrailinit(const sparsematrix* s,
     sluv2sparsetrail* a,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t n;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t jj;
    ae_int_t p;
    ae_int_t slsused;


    ae_assert(s->m==s->n, "SparseTrailInit: M<>N", _state);
    ae_assert(s->matrixtype==1, "SparseTrailInit: non-CRS input", _state);
    n = s->n;
    a->n = s->n;
    a->k = 0;
    ivectorsetlengthatleast(&a->nzc, n, _state);
    ivectorsetlengthatleast(&a->colid, n, _state);
    rvectorsetlengthatleast(&a->tmp0, n, _state);
    for(i=0; i<=n-1; i++)
    {
        a->colid.ptr.p_int[i] = i;
    }
    bvectorsetlengthatleast(&a->isdensified, n, _state);
    for(i=0; i<=n-1; i++)
    {
        a->isdensified.ptr.p_bool[i] = ae_false;
    }
    
    /*
     * Working set of columns
     */
    a->maxwrkcnt = iboundval(ae_round((double)1+(double)n/(double)3, _state), 1, ae_minint(n, 50, _state), _state);
    a->wrkcnt = 0;
    ivectorsetlengthatleast(&a->wrkset, a->maxwrkcnt, _state);
    
    /*
     * Sparse linked storage (SLS). Store CRS matrix to SLS format,
     * row by row, starting from the last one.
     */
    ivectorsetlengthatleast(&a->slscolptr, n, _state);
    ivectorsetlengthatleast(&a->slsrowptr, n, _state);
    ivectorsetlengthatleast(&a->slsidx, s->ridx.ptr.p_int[n]*sptrf_slswidth, _state);
    rvectorsetlengthatleast(&a->slsval, s->ridx.ptr.p_int[n], _state);
    for(i=0; i<=n-1; i++)
    {
        a->nzc.ptr.p_int[i] = 0;
    }
    for(i=0; i<=n-1; i++)
    {
        a->slscolptr.ptr.p_int[i] = -1;
        a->slsrowptr.ptr.p_int[i] = -1;
    }
    slsused = 0;
    for(i=n-1; i>=0; i--)
    {
        j0 = s->ridx.ptr.p_int[i];
        j1 = s->ridx.ptr.p_int[i+1]-1;
        for(jj=j1; jj>=j0; jj--)
        {
            j = s->idx.ptr.p_int[jj];
            
            /*
             * Update non-zero counts for columns
             */
            a->nzc.ptr.p_int[j] = a->nzc.ptr.p_int[j]+1;
            
            /*
             * Insert into column list
             */
            p = a->slscolptr.ptr.p_int[j];
            if( p>=0 )
            {
                a->slsidx.ptr.p_int[p*sptrf_slswidth+0] = slsused;
            }
            a->slsidx.ptr.p_int[slsused*sptrf_slswidth+0] = -1;
            a->slsidx.ptr.p_int[slsused*sptrf_slswidth+1] = p;
            a->slscolptr.ptr.p_int[j] = slsused;
            
            /*
             * Insert into row list
             */
            p = a->slsrowptr.ptr.p_int[i];
            if( p>=0 )
            {
                a->slsidx.ptr.p_int[p*sptrf_slswidth+2] = slsused;
            }
            a->slsidx.ptr.p_int[slsused*sptrf_slswidth+2] = -1;
            a->slsidx.ptr.p_int[slsused*sptrf_slswidth+3] = p;
            a->slsrowptr.ptr.p_int[i] = slsused;
            
            /*
             * Store index and value
             */
            a->slsidx.ptr.p_int[slsused*sptrf_slswidth+4] = i;
            a->slsidx.ptr.p_int[slsused*sptrf_slswidth+5] = j;
            a->slsval.ptr.p_double[slsused] = s->vals.ptr.p_double[jj];
            slsused = slsused+1;
        }
    }
    a->slsused = slsused;
}


/*************************************************************************
This function searches for a appropriate pivot column/row.

If there exists non-densified column, it returns indexes of  pivot  column
and row, with most sparse column selected for column pivoting, and largest
element selected for row pivoting. Function result is True.

PivotType=1 means that no column pivoting is performed
PivotType=2 means that both column and row pivoting are supported

If all columns were densified, False is returned.

  -- ALGLIB routine --
     15.01.2019
     Bochkanov Sergey
*************************************************************************/
static ae_bool sptrf_sparsetrailfindpivot(sluv2sparsetrail* a,
     ae_int_t pivottype,
     ae_int_t* ipiv,
     ae_int_t* jpiv,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t k;
    ae_int_t j;
    ae_int_t jp;
    ae_int_t entry;
    ae_int_t nz;
    ae_int_t maxwrknz;
    ae_int_t nnzbest;
    double s;
    double bbest;
    ae_int_t wrk0;
    ae_int_t wrk1;
    ae_bool result;

    *ipiv = 0;
    *jpiv = 0;

    n = a->n;
    k = a->k;
    nnzbest = n+1;
    *jpiv = -1;
    *ipiv = -1;
    result = ae_true;
    
    /*
     * Select pivot column
     */
    if( pivottype==1 )
    {
        
        /*
         * No column pivoting
         */
        ae_assert(!a->isdensified.ptr.p_bool[k], "SparseTrailFindPivot: integrity check failed", _state);
        *jpiv = k;
    }
    else
    {
        
        /*
         * Find pivot column
         */
        for(;;)
        {
            
            /*
             * Scan working set (if non-empty) for good columns
             */
            maxwrknz = a->maxwrknz;
            for(j=0; j<=a->wrkcnt-1; j++)
            {
                jp = a->wrkset.ptr.p_int[j];
                if( jp<k )
                {
                    continue;
                }
                if( a->isdensified.ptr.p_bool[jp] )
                {
                    continue;
                }
                nz = a->nzc.ptr.p_int[jp];
                if( nz>maxwrknz )
                {
                    continue;
                }
                if( *jpiv<0||nz<nnzbest )
                {
                    nnzbest = nz;
                    *jpiv = jp;
                }
            }
            if( *jpiv>=0 )
            {
                break;
            }
            
            /*
             * Well, nothing found. Recompute working set:
             * * determine most sparse unprocessed yet column
             * * gather all columns with density in [Wrk0,Wrk1) range,
             *   increase range, repeat, until working set is full
             */
            a->wrkcnt = 0;
            a->maxwrknz = 0;
            wrk0 = n+1;
            for(jp=k; jp<=n-1; jp++)
            {
                if( !a->isdensified.ptr.p_bool[jp]&&a->nzc.ptr.p_int[jp]<wrk0 )
                {
                    wrk0 = a->nzc.ptr.p_int[jp];
                }
            }
            if( wrk0>n )
            {
                
                /*
                 * Only densified columns are present, exit.
                 */
                result = ae_false;
                return result;
            }
            wrk1 = wrk0+1;
            while(a->wrkcnt<a->maxwrkcnt&&wrk0<=n)
            {
                
                /*
                 * Find columns with non-zero count in [Wrk0,Wrk1) range
                 */
                for(jp=k; jp<=n-1; jp++)
                {
                    if( a->wrkcnt==a->maxwrkcnt )
                    {
                        break;
                    }
                    if( a->isdensified.ptr.p_bool[jp] )
                    {
                        continue;
                    }
                    if( a->nzc.ptr.p_int[jp]>=wrk0&&a->nzc.ptr.p_int[jp]<wrk1 )
                    {
                        a->wrkset.ptr.p_int[a->wrkcnt] = jp;
                        a->wrkcnt = a->wrkcnt+1;
                        a->maxwrknz = ae_maxint(a->maxwrknz, a->nzc.ptr.p_int[jp], _state);
                    }
                }
                
                /*
                 * Advance scan range
                 */
                jp = ae_round(1.41*(double)(wrk1-wrk0), _state)+1;
                wrk0 = wrk1;
                wrk1 = wrk0+jp;
            }
        }
    }
    
    /*
     * Select pivot row
     */
    bbest = (double)(0);
    entry = a->slscolptr.ptr.p_int[*jpiv];
    while(entry>=0)
    {
        s = ae_fabs(a->slsval.ptr.p_double[entry], _state);
        if( *ipiv<0||ae_fp_greater(s,bbest) )
        {
            bbest = s;
            *ipiv = a->slsidx.ptr.p_int[entry*sptrf_slswidth+4];
        }
        entry = a->slsidx.ptr.p_int[entry*sptrf_slswidth+1];
    }
    if( *ipiv<0 )
    {
        *ipiv = k;
    }
    return result;
}


/*************************************************************************
This function pivots out specified row and column.

Sparse trail range changes from [K,N) to [K+1,N).

V0I, V0R, V1I, V1R must be preallocated arrays[N].

Following data are returned:
* UU - diagonal element (pivoted out), can be zero
* V0I, V0R, NZ0 - sparse column pivoted out to the left (after permutation
  is applied to its elements) and divided by UU.
  V0I is array[NZ0] which stores row indexes in [K+1,N) range, V0R  stores
  values.
* V1I, V1R, NZ1 - sparse row pivoted out to the top.

  -- ALGLIB routine --
     15.01.2019
     Bochkanov Sergey
*************************************************************************/
static void sptrf_sparsetrailpivotout(sluv2sparsetrail* a,
     ae_int_t ipiv,
     ae_int_t jpiv,
     double* uu,
     /* Integer */ ae_vector* v0i,
     /* Real    */ ae_vector* v0r,
     ae_int_t* nz0,
     /* Integer */ ae_vector* v1i,
     /* Real    */ ae_vector* v1r,
     ae_int_t* nz1,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t k;
    ae_int_t i;
    ae_int_t j;
    ae_int_t entry;
    double v;
    double s;
    ae_bool vb;
    ae_int_t pos0k;
    ae_int_t pos0piv;
    ae_int_t pprev;
    ae_int_t pnext;
    ae_int_t pnextnext;

    *uu = 0.0;
    *nz0 = 0;
    *nz1 = 0;

    n = a->n;
    k = a->k;
    ae_assert(k<n, "SparseTrailPivotOut: integrity check failed", _state);
    
    /*
     * Pivot out column JPiv from the sparse linked storage:
     * * remove column JPiv from the matrix
     * * update column K:
     *   * change element indexes after it is permuted to JPiv
     *   * resort rows affected by move K->JPiv
     *
     * NOTE: this code leaves V0I/V0R/NZ0 in the unfinalized state,
     *       i.e. these arrays do not account for pivoting performed
     *       on rows. They will be post-processed later.
     */
    *nz0 = 0;
    pos0k = -1;
    pos0piv = -1;
    entry = a->slscolptr.ptr.p_int[jpiv];
    while(entry>=0)
    {
        
        /*
         * Offload element
         */
        i = a->slsidx.ptr.p_int[entry*sptrf_slswidth+4];
        v0i->ptr.p_int[*nz0] = i;
        v0r->ptr.p_double[*nz0] = a->slsval.ptr.p_double[entry];
        if( i==k )
        {
            pos0k = *nz0;
        }
        if( i==ipiv )
        {
            pos0piv = *nz0;
        }
        *nz0 = *nz0+1;
        
        /*
         * Remove element from the row list
         */
        pprev = a->slsidx.ptr.p_int[entry*sptrf_slswidth+2];
        pnext = a->slsidx.ptr.p_int[entry*sptrf_slswidth+3];
        if( pprev>=0 )
        {
            a->slsidx.ptr.p_int[pprev*sptrf_slswidth+3] = pnext;
        }
        else
        {
            a->slsrowptr.ptr.p_int[i] = pnext;
        }
        if( pnext>=0 )
        {
            a->slsidx.ptr.p_int[pnext*sptrf_slswidth+2] = pprev;
        }
        
        /*
         * Select next entry
         */
        entry = a->slsidx.ptr.p_int[entry*sptrf_slswidth+1];
    }
    entry = a->slscolptr.ptr.p_int[k];
    a->slscolptr.ptr.p_int[jpiv] = entry;
    while(entry>=0)
    {
        
        /*
         * Change column index
         */
        a->slsidx.ptr.p_int[entry*sptrf_slswidth+5] = jpiv;
        
        /*
         * Next entry
         */
        entry = a->slsidx.ptr.p_int[entry*sptrf_slswidth+1];
    }
    
    /*
     * Post-process V0, account for pivoting.
     * Compute diagonal element UU.
     */
    *uu = (double)(0);
    if( pos0k>=0||pos0piv>=0 )
    {
        
        /*
         * Apply permutation to rows of pivoted out column, specific
         * implementation depends on the sparsity at locations #Pos0K
         * and #Pos0Piv of the V0 array.
         */
        if( pos0k>=0&&pos0piv>=0 )
        {
            
            /*
             * Obtain diagonal element
             */
            *uu = v0r->ptr.p_double[pos0piv];
            if( *uu!=(double)0 )
            {
                s = (double)1/(*uu);
            }
            else
            {
                s = (double)(1);
            }
            
            /*
             * Move pivoted out element, shift array by one in order
             * to remove heading diagonal element (not needed here
             * anymore).
             */
            v0r->ptr.p_double[pos0piv] = v0r->ptr.p_double[pos0k];
            for(i=0; i<=*nz0-2; i++)
            {
                v0i->ptr.p_int[i] = v0i->ptr.p_int[i+1];
                v0r->ptr.p_double[i] = v0r->ptr.p_double[i+1]*s;
            }
            *nz0 = *nz0-1;
        }
        if( pos0k>=0&&pos0piv<0 )
        {
            
            /*
             * Diagonal element is zero
             */
            *uu = (double)(0);
            
            /*
             * Pivot out element, reorder array
             */
            v0i->ptr.p_int[pos0k] = ipiv;
            for(i=pos0k; i<=*nz0-2; i++)
            {
                if( v0i->ptr.p_int[i]<v0i->ptr.p_int[i+1] )
                {
                    break;
                }
                j = v0i->ptr.p_int[i];
                v0i->ptr.p_int[i] = v0i->ptr.p_int[i+1];
                v0i->ptr.p_int[i+1] = j;
                v = v0r->ptr.p_double[i];
                v0r->ptr.p_double[i] = v0r->ptr.p_double[i+1];
                v0r->ptr.p_double[i+1] = v;
            }
        }
        if( pos0k<0&&pos0piv>=0 )
        {
            
            /*
             * Get diagonal element
             */
            *uu = v0r->ptr.p_double[pos0piv];
            if( *uu!=(double)0 )
            {
                s = (double)1/(*uu);
            }
            else
            {
                s = (double)(1);
            }
            
            /*
             * Shift array past the pivoted in element by one
             * in order to remove pivot
             */
            for(i=0; i<=pos0piv-1; i++)
            {
                v0r->ptr.p_double[i] = v0r->ptr.p_double[i]*s;
            }
            for(i=pos0piv; i<=*nz0-2; i++)
            {
                v0i->ptr.p_int[i] = v0i->ptr.p_int[i+1];
                v0r->ptr.p_double[i] = v0r->ptr.p_double[i+1]*s;
            }
            *nz0 = *nz0-1;
        }
    }
    
    /*
     * Pivot out row IPiv from the sparse linked storage:
     * * remove row IPiv from the matrix
     * * reindex elements of row K after it is permuted to IPiv
     * * apply permutation to the cols of the pivoted out row,
     *   resort columns
     */
    *nz1 = 0;
    entry = a->slsrowptr.ptr.p_int[ipiv];
    while(entry>=0)
    {
        
        /*
         * Offload element
         */
        j = a->slsidx.ptr.p_int[entry*sptrf_slswidth+5];
        v1i->ptr.p_int[*nz1] = j;
        v1r->ptr.p_double[*nz1] = a->slsval.ptr.p_double[entry];
        *nz1 = *nz1+1;
        
        /*
         * Remove element from the column list
         */
        pprev = a->slsidx.ptr.p_int[entry*sptrf_slswidth+0];
        pnext = a->slsidx.ptr.p_int[entry*sptrf_slswidth+1];
        if( pprev>=0 )
        {
            a->slsidx.ptr.p_int[pprev*sptrf_slswidth+1] = pnext;
        }
        else
        {
            a->slscolptr.ptr.p_int[j] = pnext;
        }
        if( pnext>=0 )
        {
            a->slsidx.ptr.p_int[pnext*sptrf_slswidth+0] = pprev;
        }
        
        /*
         * Select next entry
         */
        entry = a->slsidx.ptr.p_int[entry*sptrf_slswidth+3];
    }
    a->slsrowptr.ptr.p_int[ipiv] = a->slsrowptr.ptr.p_int[k];
    entry = a->slsrowptr.ptr.p_int[ipiv];
    while(entry>=0)
    {
        
        /*
         * Change row index
         */
        a->slsidx.ptr.p_int[entry*sptrf_slswidth+4] = ipiv;
        
        /*
         * Resort column affected by row pivoting
         */
        j = a->slsidx.ptr.p_int[entry*sptrf_slswidth+5];
        pprev = a->slsidx.ptr.p_int[entry*sptrf_slswidth+0];
        pnext = a->slsidx.ptr.p_int[entry*sptrf_slswidth+1];
        while(pnext>=0&&a->slsidx.ptr.p_int[pnext*sptrf_slswidth+4]<ipiv)
        {
            pnextnext = a->slsidx.ptr.p_int[pnext*sptrf_slswidth+1];
            
            /*
             * prev->next
             */
            if( pprev>=0 )
            {
                a->slsidx.ptr.p_int[pprev*sptrf_slswidth+1] = pnext;
            }
            else
            {
                a->slscolptr.ptr.p_int[j] = pnext;
            }
            
            /*
             * entry->prev, entry->next
             */
            a->slsidx.ptr.p_int[entry*sptrf_slswidth+0] = pnext;
            a->slsidx.ptr.p_int[entry*sptrf_slswidth+1] = pnextnext;
            
            /*
             * next->prev, next->next
             */
            a->slsidx.ptr.p_int[pnext*sptrf_slswidth+0] = pprev;
            a->slsidx.ptr.p_int[pnext*sptrf_slswidth+1] = entry;
            
            /*
             * nextnext->prev
             */
            if( pnextnext>=0 )
            {
                a->slsidx.ptr.p_int[pnextnext*sptrf_slswidth+0] = entry;
            }
            
            /*
             * PPrev, Item, PNext
             */
            pprev = pnext;
            pnext = pnextnext;
        }
        
        /*
         * Next entry
         */
        entry = a->slsidx.ptr.p_int[entry*sptrf_slswidth+3];
    }
    
    /*
     * Reorder other structures
     */
    i = a->nzc.ptr.p_int[k];
    a->nzc.ptr.p_int[k] = a->nzc.ptr.p_int[jpiv];
    a->nzc.ptr.p_int[jpiv] = i;
    i = a->colid.ptr.p_int[k];
    a->colid.ptr.p_int[k] = a->colid.ptr.p_int[jpiv];
    a->colid.ptr.p_int[jpiv] = i;
    vb = a->isdensified.ptr.p_bool[k];
    a->isdensified.ptr.p_bool[k] = a->isdensified.ptr.p_bool[jpiv];
    a->isdensified.ptr.p_bool[jpiv] = vb;
    
    /*
     * Handle removal of col/row #K
     */
    for(i=0; i<=*nz1-1; i++)
    {
        j = v1i->ptr.p_int[i];
        a->nzc.ptr.p_int[j] = a->nzc.ptr.p_int[j]-1;
    }
    a->k = a->k+1;
}


/*************************************************************************
This function densifies I1-th column of the sparse trail.

PARAMETERS:
    A           -   sparse trail
    I1          -   column index
    BUpper      -   upper rectangular submatrix, updated during densification
                    of the columns (densified columns are removed)
    DTrail      -   dense trail, receives densified columns from sparse
                    trail and BUpper

  -- ALGLIB routine --
     15.01.2019
     Bochkanov Sergey
*************************************************************************/
static void sptrf_sparsetraildensify(sluv2sparsetrail* a,
     ae_int_t i1,
     sluv2list1matrix* bupper,
     sluv2densetrail* dtrail,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t k;
    ae_int_t i;
    ae_int_t jp;
    ae_int_t entry;
    ae_int_t pprev;
    ae_int_t pnext;


    n = a->n;
    k = a->k;
    ae_assert(k<n, "SparseTrailDensify: integrity check failed", _state);
    ae_assert(k<=i1, "SparseTrailDensify: integrity check failed", _state);
    ae_assert(!a->isdensified.ptr.p_bool[i1], "SparseTrailDensify: integrity check failed", _state);
    
    /*
     * Offload items [0,K) of densified column from BUpper
     */
    for(i=0; i<=n-1; i++)
    {
        a->tmp0.ptr.p_double[i] = (double)(0);
    }
    jp = bupper->idxfirst.ptr.p_int[i1];
    while(jp>=0)
    {
        a->tmp0.ptr.p_double[bupper->strgidx.ptr.p_int[2*jp+1]] = bupper->strgval.ptr.p_double[jp];
        jp = bupper->strgidx.ptr.p_int[2*jp+0];
    }
    sptrf_sluv2list1dropsequence(bupper, i1, _state);
    
    /*
     * Offload items [K,N) of densified column from BLeft
     */
    entry = a->slscolptr.ptr.p_int[i1];
    while(entry>=0)
    {
        
        /*
         * Offload element
         */
        i = a->slsidx.ptr.p_int[entry*sptrf_slswidth+4];
        a->tmp0.ptr.p_double[i] = a->slsval.ptr.p_double[entry];
        
        /*
         * Remove element from the row list
         */
        pprev = a->slsidx.ptr.p_int[entry*sptrf_slswidth+2];
        pnext = a->slsidx.ptr.p_int[entry*sptrf_slswidth+3];
        if( pprev>=0 )
        {
            a->slsidx.ptr.p_int[pprev*sptrf_slswidth+3] = pnext;
        }
        else
        {
            a->slsrowptr.ptr.p_int[i] = pnext;
        }
        if( pnext>=0 )
        {
            a->slsidx.ptr.p_int[pnext*sptrf_slswidth+2] = pprev;
        }
        
        /*
         * Select next entry
         */
        entry = a->slsidx.ptr.p_int[entry*sptrf_slswidth+1];
    }
    
    /*
     * Densify
     */
    a->nzc.ptr.p_int[i1] = 0;
    a->isdensified.ptr.p_bool[i1] = ae_true;
    a->slscolptr.ptr.p_int[i1] = -1;
    sptrf_densetrailappendcolumn(dtrail, &a->tmp0, a->colid.ptr.p_int[i1], _state);
}


/*************************************************************************
This function appends rank-1 update to the sparse trail.  Dense  trail  is
not  updated  here,  but  we  may  move some columns to dense trail during
update (i.e. densify them). Thus, you have to update  dense  trail  BEFORE
you start updating sparse one (otherwise, recently densified columns  will
be updated twice).

PARAMETERS:
    A           -   sparse trail
    V0I, V0R    -   update column returned by SparseTrailPivotOut (MUST be
                    array[N] independently of the NZ0).
    NZ0         -   non-zero count for update column
    V1I, V1R    -   update row returned by SparseTrailPivotOut
    NZ1         -   non-zero count for update row
    BUpper      -   upper rectangular submatrix, updated during densification
                    of the columns (densified columns are removed)
    DTrail      -   dense trail, receives densified columns from sparse
                    trail and BUpper
    DensificationSupported- if False, no densification is performed

  -- ALGLIB routine --
     15.01.2019
     Bochkanov Sergey
*************************************************************************/
static void sptrf_sparsetrailupdate(sluv2sparsetrail* a,
     /* Integer */ ae_vector* v0i,
     /* Real    */ ae_vector* v0r,
     ae_int_t nz0,
     /* Integer */ const ae_vector* v1i,
     /* Real    */ const ae_vector* v1r,
     ae_int_t nz1,
     sluv2list1matrix* bupper,
     sluv2densetrail* dtrail,
     ae_bool densificationsupported,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t k;
    ae_int_t i;
    ae_int_t j;
    ae_int_t i0;
    ae_int_t i1;
    double v1;
    ae_int_t densifyabove;
    ae_int_t nnz;
    ae_int_t entry;
    ae_int_t newentry;
    ae_int_t pprev;
    ae_int_t pnext;
    ae_int_t p;
    ae_int_t nexti;
    ae_int_t newoffs;


    n = a->n;
    k = a->k;
    ae_assert(k<n, "SparseTrailPivotOut: integrity check failed", _state);
    densifyabove = ae_round(sptrf_densebnd*(double)(n-k), _state)+1;
    ae_assert(v0i->cnt>=nz0+1, "SparseTrailUpdate: integrity check failed", _state);
    ae_assert(v0r->cnt>=nz0+1, "SparseTrailUpdate: integrity check failed", _state);
    v0i->ptr.p_int[nz0] = -1;
    v0r->ptr.p_double[nz0] = (double)(0);
    
    /*
     * Update sparse representation
     */
    ivectorgrowto(&a->slsidx, (a->slsused+nz0*nz1)*sptrf_slswidth, _state);
    rvectorgrowto(&a->slsval, a->slsused+nz0*nz1, _state);
    for(j=0; j<=nz1-1; j++)
    {
        if( nz0==0 )
        {
            continue;
        }
        i1 = v1i->ptr.p_int[j];
        v1 = v1r->ptr.p_double[j];
        
        /*
         * Update column #I1
         */
        nnz = a->nzc.ptr.p_int[i1];
        i = 0;
        i0 = v0i->ptr.p_int[i];
        entry = a->slscolptr.ptr.p_int[i1];
        pprev = -1;
        while(i<nz0)
        {
            
            /*
             * Handle possible fill-in happening BEFORE already existing
             * entry of the column list (or simply fill-in, if no entry
             * is present).
             */
            pnext = entry;
            if( entry>=0 )
            {
                nexti = a->slsidx.ptr.p_int[entry*sptrf_slswidth+4];
            }
            else
            {
                nexti = n+1;
            }
            while(i<nz0)
            {
                if( i0>=nexti )
                {
                    break;
                }
                
                /*
                 * Allocate new entry, store column/row/value
                 */
                newentry = a->slsused;
                a->slsused = newentry+1;
                nnz = nnz+1;
                newoffs = newentry*sptrf_slswidth;
                a->slsidx.ptr.p_int[newoffs+4] = i0;
                a->slsidx.ptr.p_int[newoffs+5] = i1;
                a->slsval.ptr.p_double[newentry] = -v1*v0r->ptr.p_double[i];
                
                /*
                 * Insert entry into column list
                 */
                a->slsidx.ptr.p_int[newoffs+0] = pprev;
                a->slsidx.ptr.p_int[newoffs+1] = pnext;
                if( pprev>=0 )
                {
                    a->slsidx.ptr.p_int[pprev*sptrf_slswidth+1] = newentry;
                }
                else
                {
                    a->slscolptr.ptr.p_int[i1] = newentry;
                }
                if( entry>=0 )
                {
                    a->slsidx.ptr.p_int[entry*sptrf_slswidth+0] = newentry;
                }
                
                /*
                 * Insert entry into row list
                 */
                p = a->slsrowptr.ptr.p_int[i0];
                a->slsidx.ptr.p_int[newoffs+2] = -1;
                a->slsidx.ptr.p_int[newoffs+3] = p;
                if( p>=0 )
                {
                    a->slsidx.ptr.p_int[p*sptrf_slswidth+2] = newentry;
                }
                a->slsrowptr.ptr.p_int[i0] = newentry;
                
                /*
                 * Advance pointers
                 */
                pprev = newentry;
                i = i+1;
                i0 = v0i->ptr.p_int[i];
            }
            if( i>=nz0 )
            {
                break;
            }
            
            /*
             * Update already existing entry of the column list, if needed
             */
            if( entry>=0 )
            {
                if( i0==nexti )
                {
                    a->slsval.ptr.p_double[entry] = a->slsval.ptr.p_double[entry]-v1*v0r->ptr.p_double[i];
                    i = i+1;
                    i0 = v0i->ptr.p_int[i];
                }
                pprev = entry;
            }
            
            /*
             * Advance to the next pre-existing entry (if present)
             */
            if( entry>=0 )
            {
                entry = a->slsidx.ptr.p_int[entry*sptrf_slswidth+1];
            }
        }
        a->nzc.ptr.p_int[i1] = nnz;
        
        /*
         * Densify column if needed
         */
        if( (densificationsupported&&nnz>densifyabove)&&!a->isdensified.ptr.p_bool[i1] )
        {
            sptrf_sparsetraildensify(a, i1, bupper, dtrail, _state);
        }
    }
}


void _sluv2list1matrix_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    sluv2list1matrix *p = (sluv2list1matrix*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->idxfirst, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->strgidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->strgval, 0, DT_REAL, _state, make_automatic);
}


void _sluv2list1matrix_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    sluv2list1matrix       *dst = (sluv2list1matrix*)_dst;
    const sluv2list1matrix *src = (const sluv2list1matrix*)_src;
    dst->nfixed = src->nfixed;
    dst->ndynamic = src->ndynamic;
    ae_vector_init_copy(&dst->idxfirst, &src->idxfirst, _state, make_automatic);
    ae_vector_init_copy(&dst->strgidx, &src->strgidx, _state, make_automatic);
    ae_vector_init_copy(&dst->strgval, &src->strgval, _state, make_automatic);
    dst->nallocated = src->nallocated;
    dst->nused = src->nused;
}


void _sluv2list1matrix_clear(void* _p)
{
    sluv2list1matrix *p = (sluv2list1matrix*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->idxfirst);
    ae_vector_clear(&p->strgidx);
    ae_vector_clear(&p->strgval);
}


void _sluv2list1matrix_destroy(void* _p)
{
    sluv2list1matrix *p = (sluv2list1matrix*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->idxfirst);
    ae_vector_destroy(&p->strgidx);
    ae_vector_destroy(&p->strgval);
}


void _sluv2sparsetrail_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    sluv2sparsetrail *p = (sluv2sparsetrail*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->nzc, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->wrkset, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->colid, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->isdensified, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->slscolptr, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->slsrowptr, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->slsidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->slsval, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
}


void _sluv2sparsetrail_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    sluv2sparsetrail       *dst = (sluv2sparsetrail*)_dst;
    const sluv2sparsetrail *src = (const sluv2sparsetrail*)_src;
    dst->n = src->n;
    dst->k = src->k;
    ae_vector_init_copy(&dst->nzc, &src->nzc, _state, make_automatic);
    dst->maxwrkcnt = src->maxwrkcnt;
    dst->maxwrknz = src->maxwrknz;
    dst->wrkcnt = src->wrkcnt;
    ae_vector_init_copy(&dst->wrkset, &src->wrkset, _state, make_automatic);
    ae_vector_init_copy(&dst->colid, &src->colid, _state, make_automatic);
    ae_vector_init_copy(&dst->isdensified, &src->isdensified, _state, make_automatic);
    ae_vector_init_copy(&dst->slscolptr, &src->slscolptr, _state, make_automatic);
    ae_vector_init_copy(&dst->slsrowptr, &src->slsrowptr, _state, make_automatic);
    ae_vector_init_copy(&dst->slsidx, &src->slsidx, _state, make_automatic);
    ae_vector_init_copy(&dst->slsval, &src->slsval, _state, make_automatic);
    dst->slsused = src->slsused;
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
}


void _sluv2sparsetrail_clear(void* _p)
{
    sluv2sparsetrail *p = (sluv2sparsetrail*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->nzc);
    ae_vector_clear(&p->wrkset);
    ae_vector_clear(&p->colid);
    ae_vector_clear(&p->isdensified);
    ae_vector_clear(&p->slscolptr);
    ae_vector_clear(&p->slsrowptr);
    ae_vector_clear(&p->slsidx);
    ae_vector_clear(&p->slsval);
    ae_vector_clear(&p->tmp0);
}


void _sluv2sparsetrail_destroy(void* _p)
{
    sluv2sparsetrail *p = (sluv2sparsetrail*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->nzc);
    ae_vector_destroy(&p->wrkset);
    ae_vector_destroy(&p->colid);
    ae_vector_destroy(&p->isdensified);
    ae_vector_destroy(&p->slscolptr);
    ae_vector_destroy(&p->slsrowptr);
    ae_vector_destroy(&p->slsidx);
    ae_vector_destroy(&p->slsval);
    ae_vector_destroy(&p->tmp0);
}


void _sluv2densetrail_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    sluv2densetrail *p = (sluv2densetrail*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_init(&p->d, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->did, 0, DT_INT, _state, make_automatic);
}


void _sluv2densetrail_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    sluv2densetrail       *dst = (sluv2densetrail*)_dst;
    const sluv2densetrail *src = (const sluv2densetrail*)_src;
    dst->n = src->n;
    dst->ndense = src->ndense;
    ae_matrix_init_copy(&dst->d, &src->d, _state, make_automatic);
    ae_vector_init_copy(&dst->did, &src->did, _state, make_automatic);
}


void _sluv2densetrail_clear(void* _p)
{
    sluv2densetrail *p = (sluv2densetrail*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_clear(&p->d);
    ae_vector_clear(&p->did);
}


void _sluv2densetrail_destroy(void* _p)
{
    sluv2densetrail *p = (sluv2densetrail*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_destroy(&p->d);
    ae_vector_destroy(&p->did);
}


void _sluv2buffer_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    sluv2buffer *p = (sluv2buffer*)_p;
    ae_touch_ptr((void*)p);
    _sparsematrix_init(&p->sparsel, _state, make_automatic);
    _sparsematrix_init(&p->sparseut, _state, make_automatic);
    _sluv2list1matrix_init(&p->bleft, _state, make_automatic);
    _sluv2list1matrix_init(&p->bupper, _state, make_automatic);
    _sluv2sparsetrail_init(&p->strail, _state, make_automatic);
    _sluv2densetrail_init(&p->dtrail, _state, make_automatic);
    ae_vector_init(&p->rowpermrawidx, 0, DT_INT, _state, make_automatic);
    ae_matrix_init(&p->dbuf, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->v0i, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->v1i, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->v0r, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->v1r, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpi, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->tmpp, 0, DT_INT, _state, make_automatic);
}


void _sluv2buffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    sluv2buffer       *dst = (sluv2buffer*)_dst;
    const sluv2buffer *src = (const sluv2buffer*)_src;
    dst->n = src->n;
    _sparsematrix_init_copy(&dst->sparsel, &src->sparsel, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparseut, &src->sparseut, _state, make_automatic);
    _sluv2list1matrix_init_copy(&dst->bleft, &src->bleft, _state, make_automatic);
    _sluv2list1matrix_init_copy(&dst->bupper, &src->bupper, _state, make_automatic);
    _sluv2sparsetrail_init_copy(&dst->strail, &src->strail, _state, make_automatic);
    _sluv2densetrail_init_copy(&dst->dtrail, &src->dtrail, _state, make_automatic);
    ae_vector_init_copy(&dst->rowpermrawidx, &src->rowpermrawidx, _state, make_automatic);
    ae_matrix_init_copy(&dst->dbuf, &src->dbuf, _state, make_automatic);
    ae_vector_init_copy(&dst->v0i, &src->v0i, _state, make_automatic);
    ae_vector_init_copy(&dst->v1i, &src->v1i, _state, make_automatic);
    ae_vector_init_copy(&dst->v0r, &src->v0r, _state, make_automatic);
    ae_vector_init_copy(&dst->v1r, &src->v1r, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpi, &src->tmpi, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpp, &src->tmpp, _state, make_automatic);
}


void _sluv2buffer_clear(void* _p)
{
    sluv2buffer *p = (sluv2buffer*)_p;
    ae_touch_ptr((void*)p);
    _sparsematrix_clear(&p->sparsel);
    _sparsematrix_clear(&p->sparseut);
    _sluv2list1matrix_clear(&p->bleft);
    _sluv2list1matrix_clear(&p->bupper);
    _sluv2sparsetrail_clear(&p->strail);
    _sluv2densetrail_clear(&p->dtrail);
    ae_vector_clear(&p->rowpermrawidx);
    ae_matrix_clear(&p->dbuf);
    ae_vector_clear(&p->v0i);
    ae_vector_clear(&p->v1i);
    ae_vector_clear(&p->v0r);
    ae_vector_clear(&p->v1r);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->tmpi);
    ae_vector_clear(&p->tmpp);
}


void _sluv2buffer_destroy(void* _p)
{
    sluv2buffer *p = (sluv2buffer*)_p;
    ae_touch_ptr((void*)p);
    _sparsematrix_destroy(&p->sparsel);
    _sparsematrix_destroy(&p->sparseut);
    _sluv2list1matrix_destroy(&p->bleft);
    _sluv2list1matrix_destroy(&p->bupper);
    _sluv2sparsetrail_destroy(&p->strail);
    _sluv2densetrail_destroy(&p->dtrail);
    ae_vector_destroy(&p->rowpermrawidx);
    ae_matrix_destroy(&p->dbuf);
    ae_vector_destroy(&p->v0i);
    ae_vector_destroy(&p->v1i);
    ae_vector_destroy(&p->v0r);
    ae_vector_destroy(&p->v1r);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->tmpi);
    ae_vector_destroy(&p->tmpp);
}


/*$ End $*/
