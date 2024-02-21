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
#include "sparse.h"


/*$ Declarations $*/
static double sparse_desiredloadfactor = 0.66;
static double sparse_maxloadfactor = 0.75;
static double sparse_growfactor = 2.00;
static ae_int_t sparse_additional = 10;
static ae_int_t sparse_linalgswitch = 16;
static ae_int_t sparse_hash(ae_int_t i,
     ae_int_t j,
     ae_int_t tabsize,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
This function creates sparse matrix in a Hash-Table format.

This function creates Hast-Table matrix, which can be  converted  to  CRS
format after its initialization is over. Typical  usage  scenario  for  a
sparse matrix is:
1. creation in a Hash-Table format
2. insertion of the matrix elements
3. conversion to the CRS representation
4. matrix is passed to some linear algebra algorithm

Some  information  about  different matrix formats can be found below, in
the "NOTES" section.

INPUT PARAMETERS
    M           -   number of rows in a matrix, M>=1
    N           -   number of columns in a matrix, N>=1
    K           -   K>=0, expected number of non-zero elements in a matrix.
                    K can be inexact approximation, can be less than actual
                    number  of  elements  (table will grow when needed) or 
                    even zero).
                    It is important to understand that although hash-table
                    may grow automatically, it is better to  provide  good
                    estimate of data size.

OUTPUT PARAMETERS
    S           -   sparse M*N matrix in Hash-Table representation.
                    All elements of the matrix are zero.

NOTE 1

Hash-tables use memory inefficiently, and they have to keep  some  amount
of the "spare memory" in order to have good performance. Hash  table  for
matrix with K non-zero elements will  need  C*K*(8+2*sizeof(int))  bytes,
where C is a small constant, about 1.5-2 in magnitude.

CRS storage, from the other side, is  more  memory-efficient,  and  needs
just K*(8+sizeof(int))+M*sizeof(int) bytes, where M is a number  of  rows
in a matrix.

When you convert from the Hash-Table to CRS  representation, all unneeded
memory will be freed.

NOTE 2

Comments of SparseMatrix structure outline  information  about  different
sparse storage formats. We recommend you to read them before starting  to
use ALGLIB sparse matrices.

NOTE 3

This function completely  overwrites S with new sparse matrix. Previously
allocated storage is NOT reused. If you  want  to reuse already allocated
memory, call SparseCreateBuf function.

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
void sparsecreate(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     sparsematrix* s,
     ae_state *_state)
{

    _sparsematrix_clear(s);

    sparsecreatebuf(m, n, k, s, _state);
}


/*************************************************************************
This version of SparseCreate function creates sparse matrix in Hash-Table
format, reusing previously allocated storage as much  as  possible.  Read
comments for SparseCreate() for more information.

INPUT PARAMETERS
    M           -   number of rows in a matrix, M>=1
    N           -   number of columns in a matrix, N>=1
    K           -   K>=0, expected number of non-zero elements in a matrix.
                    K can be inexact approximation, can be less than actual
                    number  of  elements  (table will grow when needed) or 
                    even zero).
                    It is important to understand that although hash-table
                    may grow automatically, it is better to  provide  good
                    estimate of data size.
    S           -   SparseMatrix structure which MAY contain some  already
                    allocated storage.

OUTPUT PARAMETERS
    S           -   sparse M*N matrix in Hash-Table representation.
                    All elements of the matrix are zero.
                    Previously allocated storage is reused, if  its  size
                    is compatible with expected number of non-zeros K.

  -- ALGLIB PROJECT --
     Copyright 14.01.2014 by Bochkanov Sergey
*************************************************************************/
void sparsecreatebuf(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     sparsematrix* s,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(m>0, "SparseCreateBuf: M<=0", _state);
    ae_assert(n>0, "SparseCreateBuf: N<=0", _state);
    ae_assert(k>=0, "SparseCreateBuf: K<0", _state);
    
    /*
     * Hash-table size is max(existing_size,requested_size)
     *
     * NOTE: it is important to use ALL available memory for hash table
     *       because it is impossible to efficiently reallocate table
     *       without temporary storage. So, if we want table with up to
     *       1.000.000 elements, we have to create such table from the
     *       very beginning. Otherwise, the very idea of memory reuse
     *       will be compromised.
     */
    s->tablesize = ae_round((double)k/sparse_desiredloadfactor+(double)sparse_additional, _state);
    rvectorsetlengthatleast(&s->vals, s->tablesize, _state);
    s->tablesize = s->vals.cnt;
    
    /*
     * Initialize other fields
     */
    s->matrixtype = 0;
    s->m = m;
    s->n = n;
    s->nfree = s->tablesize;
    ivectorsetlengthatleast(&s->idx, 2*s->tablesize, _state);
    for(i=0; i<=s->tablesize-1; i++)
    {
        s->idx.ptr.p_int[2*i] = -1;
    }
}


/*************************************************************************
This function creates sparse matrix in a CRS format - the least  flexible
but the most efficient format implemented in ALGLIB.

This function creates CRS matrix. Typical usage scenario for a CRS matrix 
is:
1. creation (you have to tell the number of non-zero elements at each row
   at this moment)
2. initialization of the matrix elements (row by row, from left to right) 
3. the matrix is passed to some linear algebra algorithm

This function is a memory-efficient alternative to SparseCreate(), but it
is more complex because it requires you to know in advance how large your
matrix is. Some  information about  different matrix formats can be found 
in comments on SparseMatrix structure.  We recommend  you  to  read  them
before starting to use ALGLIB sparse matrices.

INPUT PARAMETERS
    M           -   number of rows in a matrix, M>=1
    N           -   number of columns in a matrix, N>=1
    NER         -   number of elements at each row, array[M], NER[I]>=0

OUTPUT PARAMETERS
    S           -   sparse M*N matrix in CRS representation.
                    You have to fill ALL non-zero elements by calling
                    SparseSet() BEFORE you try to use this matrix.
                    
NOTE: this function completely  overwrites  S  with  new  sparse  matrix.
      Previously allocated storage is NOT reused. If you  want  to  reuse
      already allocated memory, call SparseCreateCRSBuf function.

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
void sparsecreatecrs(ae_int_t m,
     ae_int_t n,
     /* Integer */ const ae_vector* ner,
     sparsematrix* s,
     ae_state *_state)
{
    ae_int_t i;

    _sparsematrix_clear(s);

    ae_assert(m>0, "SparseCreateCRS: M<=0", _state);
    ae_assert(n>0, "SparseCreateCRS: N<=0", _state);
    ae_assert(ner->cnt>=m, "SparseCreateCRS: Length(NER)<M", _state);
    for(i=0; i<=m-1; i++)
    {
        ae_assert(ner->ptr.p_int[i]>=0, "SparseCreateCRS: NER[] contains negative elements", _state);
    }
    sparsecreatecrsbuf(m, n, ner, s, _state);
}


/*************************************************************************
This function creates sparse matrix in a CRS format (expert function  for
situations when you are running out  of  memory).  This  version  of  CRS
matrix creation function may reuse memory already allocated in S.

This function creates CRS matrix. Typical usage scenario for a CRS matrix 
is:
1. creation (you have to tell number of non-zero elements at each row  at 
   this moment)
2. insertion of the matrix elements (row by row, from left to right) 
3. matrix is passed to some linear algebra algorithm

This function is a memory-efficient alternative to SparseCreate(), but it
is more complex because it requires you to know in advance how large your
matrix is. Some  information about  different matrix formats can be found 
in comments on SparseMatrix structure.  We recommend  you  to  read  them
before starting to use ALGLIB sparse matrices..

INPUT PARAMETERS
    M           -   number of rows in a matrix, M>=1
    N           -   number of columns in a matrix, N>=1
    NER         -   number of elements at each row, array[M], NER[I]>=0
    S           -   sparse matrix structure with possibly preallocated
                    memory.

OUTPUT PARAMETERS
    S           -   sparse M*N matrix in CRS representation.
                    You have to fill ALL non-zero elements by calling
                    SparseSet() BEFORE you try to use this matrix.

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
void sparsecreatecrsbuf(ae_int_t m,
     ae_int_t n,
     /* Integer */ const ae_vector* ner,
     sparsematrix* s,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t noe;


    ae_assert(m>0, "SparseCreateCRSBuf: M<=0", _state);
    ae_assert(n>0, "SparseCreateCRSBuf: N<=0", _state);
    ae_assert(ner->cnt>=m, "SparseCreateCRSBuf: Length(NER)<M", _state);
    noe = 0;
    s->matrixtype = 1;
    s->ninitialized = 0;
    s->m = m;
    s->n = n;
    ivectorsetlengthatleast(&s->ridx, s->m+1, _state);
    s->ridx.ptr.p_int[0] = 0;
    for(i=0; i<=s->m-1; i++)
    {
        ae_assert(ner->ptr.p_int[i]>=0, "SparseCreateCRSBuf: NER[] contains negative elements", _state);
        noe = noe+ner->ptr.p_int[i];
        s->ridx.ptr.p_int[i+1] = s->ridx.ptr.p_int[i]+ner->ptr.p_int[i];
    }
    rvectorsetlengthatleast(&s->vals, noe, _state);
    ivectorsetlengthatleast(&s->idx, noe, _state);
    if( noe==0 )
    {
        sparseinitduidx(s, _state);
    }
}


/*************************************************************************
This function creates a CRS-based sparse matrix from  the  dense  matrix.

This function is intended for situations when you already  have  a  dense
matrix and need a convenient way of converting it to the CRS format.

INPUT PARAMETERS
    A           -   array[M,N]. If larger, only leading MxN submatrix
                    will be used.
    M           -   number of rows in a matrix, M>=1
    N           -   number of columns in a matrix, N>=1

OUTPUT PARAMETERS
    S           -   sparse M*N matrix A in the CRS format
                    
NOTE: this function completely  overwrites  S  with  new  sparse  matrix.
      Previously allocated storage is NOT reused. If you  want  to  reuse
      already allocated memory, call SparseCreateCRSFromDenseBuf function.

  -- ALGLIB PROJECT --
     Copyright 16.06.2023 by Bochkanov Sergey
*************************************************************************/
void sparsecreatecrsfromdense(/* Real    */ const ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     sparsematrix* s,
     ae_state *_state)
{

    _sparsematrix_clear(s);

    ae_assert(m>0, "SparseCreateCRSFromDense: M<=0", _state);
    ae_assert(n>0, "SparseCreateCRSFromDense: N<=0", _state);
    ae_assert(a->rows>=m, "SparseCreateCRSFromDense: rows(A)<M", _state);
    ae_assert(a->cols>=n, "SparseCreateCRSFromDense: cols(A)<N", _state);
    ae_assert(apservisfinitematrix(a, m, n, _state), "SparseCreateCRSFromDense: A contains NAN/INF", _state);
    sparsecreatecrsfromdensebuf(a, m, n, s, _state);
}


/*************************************************************************
This function creates a CRS-based sparse matrix from  the  dense  matrix.
A buffered version which reused memory already allocated in S as much  as
possible.

This function is intended for situations when you already  have  a  dense
matrix and need a convenient way of converting it to the CRS format.

INPUT PARAMETERS
    A           -   array[M,N]. If larger, only leading MxN submatrix
                    will be used.
    M           -   number of rows in a matrix, M>=1
    N           -   number of columns in a matrix, N>=1
    S           -   an already allocated structure; if it already has
                    enough memory to store the matrix, no new memory
                    will be allocated.

OUTPUT PARAMETERS
    S           -   sparse M*N matrix A in the CRS format.

  -- ALGLIB PROJECT --
     Copyright 16.06.2023 by Bochkanov Sergey
*************************************************************************/
void sparsecreatecrsfromdensebuf(/* Real    */ const ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     sparsematrix* s,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t nnz;
    ae_int_t offs;


    ae_assert(m>0, "SparseCreateCRSFromDenseBuf: M<=0", _state);
    ae_assert(n>0, "SparseCreateCRSFromDenseBuf: N<=0", _state);
    ae_assert(a->rows>=m, "SparseCreateCRSFromDenseBuf: rows(A)<M", _state);
    ae_assert(a->cols>=n, "SparseCreateCRSFromDenseBuf: cols(A)<N", _state);
    ae_assert(apservisfinitematrix(a, m, n, _state), "SparseCreateCRSFromDenseBuf: A contains NAN/INF", _state);
    nnz = 0;
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            if( a->ptr.pp_double[i][j]!=0.0 )
            {
                nnz = nnz+1;
            }
        }
    }
    s->matrixtype = 1;
    s->ninitialized = nnz;
    s->m = m;
    s->n = n;
    iallocv(m+1, &s->ridx, _state);
    iallocv(nnz, &s->idx, _state);
    rallocv(nnz, &s->vals, _state);
    s->ridx.ptr.p_int[0] = 0;
    offs = 0;
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            if( a->ptr.pp_double[i][j]!=0.0 )
            {
                s->idx.ptr.p_int[offs] = j;
                s->vals.ptr.p_double[offs] = a->ptr.pp_double[i][j];
                offs = offs+1;
            }
        }
        s->ridx.ptr.p_int[i+1] = offs;
    }
    ae_assert(offs==nnz, "SparseCreateCRSFromDenseBuf: integrity check 6447 failed", _state);
    sparseinitduidx(s, _state);
}


/*************************************************************************
This function creates sparse matrix in  a  SKS  format  (skyline  storage
format). In most cases you do not need this function - CRS format  better
suits most use cases.

INPUT PARAMETERS
    M, N        -   number of rows(M) and columns (N) in a matrix:
                    * M=N (as for now, ALGLIB supports only square SKS)
                    * N>=1
                    * M>=1
    D           -   "bottom" bandwidths, array[M], D[I]>=0.
                    I-th element stores number of non-zeros at I-th  row,
                    below the diagonal (diagonal itself is not  included)
    U           -   "top" bandwidths, array[N], U[I]>=0.
                    I-th element stores number of non-zeros  at I-th row,
                    above the diagonal (diagonal itself  is not included)

OUTPUT PARAMETERS
    S           -   sparse M*N matrix in SKS representation.
                    All elements are filled by zeros.
                    You may use sparseset() to change their values.
                    
NOTE: this function completely  overwrites  S  with  new  sparse  matrix.
      Previously allocated storage is NOT reused. If you  want  to  reuse
      already allocated memory, call SparseCreateSKSBuf function.

  -- ALGLIB PROJECT --
     Copyright 13.01.2014 by Bochkanov Sergey
*************************************************************************/
void sparsecreatesks(ae_int_t m,
     ae_int_t n,
     /* Integer */ const ae_vector* d,
     /* Integer */ const ae_vector* u,
     sparsematrix* s,
     ae_state *_state)
{
    ae_int_t i;

    _sparsematrix_clear(s);

    ae_assert(m>0, "SparseCreateSKS: M<=0", _state);
    ae_assert(n>0, "SparseCreateSKS: N<=0", _state);
    ae_assert(m==n, "SparseCreateSKS: M<>N", _state);
    ae_assert(d->cnt>=m, "SparseCreateSKS: Length(D)<M", _state);
    ae_assert(u->cnt>=n, "SparseCreateSKS: Length(U)<N", _state);
    for(i=0; i<=m-1; i++)
    {
        ae_assert(d->ptr.p_int[i]>=0, "SparseCreateSKS: D[] contains negative elements", _state);
        ae_assert(d->ptr.p_int[i]<=i, "SparseCreateSKS: D[I]>I for some I", _state);
    }
    for(i=0; i<=n-1; i++)
    {
        ae_assert(u->ptr.p_int[i]>=0, "SparseCreateSKS: U[] contains negative elements", _state);
        ae_assert(u->ptr.p_int[i]<=i, "SparseCreateSKS: U[I]>I for some I", _state);
    }
    sparsecreatesksbuf(m, n, d, u, s, _state);
}


/*************************************************************************
This is "buffered"  version  of  SparseCreateSKS()  which  reuses  memory
previously allocated in S (of course, memory is reallocated if needed).

This function creates sparse matrix in  a  SKS  format  (skyline  storage
format). In most cases you do not need this function - CRS format  better
suits most use cases.

INPUT PARAMETERS
    M, N        -   number of rows(M) and columns (N) in a matrix:
                    * M=N (as for now, ALGLIB supports only square SKS)
                    * N>=1
                    * M>=1
    D           -   "bottom" bandwidths, array[M], 0<=D[I]<=I.
                    I-th element stores number of non-zeros at I-th row,
                    below the diagonal (diagonal itself is not included)
    U           -   "top" bandwidths, array[N], 0<=U[I]<=I.
                    I-th element stores number of non-zeros at I-th row,
                    above the diagonal (diagonal itself is not included)

OUTPUT PARAMETERS
    S           -   sparse M*N matrix in SKS representation.
                    All elements are filled by zeros.
                    You may use sparseset() to change their values.

  -- ALGLIB PROJECT --
     Copyright 13.01.2014 by Bochkanov Sergey
*************************************************************************/
void sparsecreatesksbuf(ae_int_t m,
     ae_int_t n,
     /* Integer */ const ae_vector* d,
     /* Integer */ const ae_vector* u,
     sparsematrix* s,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t minmn;
    ae_int_t nz;
    ae_int_t mxd;
    ae_int_t mxu;


    ae_assert(m>0, "SparseCreateSKSBuf: M<=0", _state);
    ae_assert(n>0, "SparseCreateSKSBuf: N<=0", _state);
    ae_assert(m==n, "SparseCreateSKSBuf: M<>N", _state);
    ae_assert(d->cnt>=m, "SparseCreateSKSBuf: Length(D)<M", _state);
    ae_assert(u->cnt>=n, "SparseCreateSKSBuf: Length(U)<N", _state);
    for(i=0; i<=m-1; i++)
    {
        ae_assert(d->ptr.p_int[i]>=0, "SparseCreateSKSBuf: D[] contains negative elements", _state);
        ae_assert(d->ptr.p_int[i]<=i, "SparseCreateSKSBuf: D[I]>I for some I", _state);
    }
    for(i=0; i<=n-1; i++)
    {
        ae_assert(u->ptr.p_int[i]>=0, "SparseCreateSKSBuf: U[] contains negative elements", _state);
        ae_assert(u->ptr.p_int[i]<=i, "SparseCreateSKSBuf: U[I]>I for some I", _state);
    }
    minmn = ae_minint(m, n, _state);
    s->matrixtype = 2;
    s->ninitialized = 0;
    s->m = m;
    s->n = n;
    ivectorsetlengthatleast(&s->ridx, minmn+1, _state);
    s->ridx.ptr.p_int[0] = 0;
    nz = 0;
    for(i=0; i<=minmn-1; i++)
    {
        nz = nz+1+d->ptr.p_int[i]+u->ptr.p_int[i];
        s->ridx.ptr.p_int[i+1] = s->ridx.ptr.p_int[i]+1+d->ptr.p_int[i]+u->ptr.p_int[i];
    }
    rvectorsetlengthatleast(&s->vals, nz, _state);
    for(i=0; i<=nz-1; i++)
    {
        s->vals.ptr.p_double[i] = 0.0;
    }
    ivectorsetlengthatleast(&s->didx, m+1, _state);
    mxd = 0;
    for(i=0; i<=m-1; i++)
    {
        s->didx.ptr.p_int[i] = d->ptr.p_int[i];
        mxd = ae_maxint(mxd, d->ptr.p_int[i], _state);
    }
    s->didx.ptr.p_int[m] = mxd;
    ivectorsetlengthatleast(&s->uidx, n+1, _state);
    mxu = 0;
    for(i=0; i<=n-1; i++)
    {
        s->uidx.ptr.p_int[i] = u->ptr.p_int[i];
        mxu = ae_maxint(mxu, u->ptr.p_int[i], _state);
    }
    s->uidx.ptr.p_int[n] = mxu;
}


/*************************************************************************
This function creates sparse matrix in  a  SKS  format  (skyline  storage
format). Unlike more general  sparsecreatesks(),  this  function  creates
sparse matrix with constant bandwidth.

You may want to use this function instead of sparsecreatesks() when  your
matrix has  constant  or  nearly-constant  bandwidth,  and  you  want  to
simplify source code.

INPUT PARAMETERS
    M, N        -   number of rows(M) and columns (N) in a matrix:
                    * M=N (as for now, ALGLIB supports only square SKS)
                    * N>=1
                    * M>=1
    BW          -   matrix bandwidth, BW>=0

OUTPUT PARAMETERS
    S           -   sparse M*N matrix in SKS representation.
                    All elements are filled by zeros.
                    You may use sparseset() to  change  their values.
                    
NOTE: this function completely  overwrites  S  with  new  sparse  matrix.
      Previously allocated storage is NOT reused. If you  want  to  reuse
      already allocated memory, call sparsecreatesksbandbuf function.

  -- ALGLIB PROJECT --
     Copyright 25.12.2017 by Bochkanov Sergey
*************************************************************************/
void sparsecreatesksband(ae_int_t m,
     ae_int_t n,
     ae_int_t bw,
     sparsematrix* s,
     ae_state *_state)
{

    _sparsematrix_clear(s);

    ae_assert(m>0, "SparseCreateSKSBand: M<=0", _state);
    ae_assert(n>0, "SparseCreateSKSBand: N<=0", _state);
    ae_assert(bw>=0, "SparseCreateSKSBand: BW<0", _state);
    ae_assert(m==n, "SparseCreateSKSBand: M!=N", _state);
    sparsecreatesksbandbuf(m, n, bw, s, _state);
}


/*************************************************************************
This is "buffered" version  of  sparsecreatesksband() which reuses memory
previously allocated in S (of course, memory is reallocated if needed).

You may want to use this function instead  of  sparsecreatesksbuf()  when
your matrix has  constant or nearly-constant  bandwidth,  and you want to
simplify source code.

INPUT PARAMETERS
    M, N        -   number of rows(M) and columns (N) in a matrix:
                    * M=N (as for now, ALGLIB supports only square SKS)
                    * N>=1
                    * M>=1
    BW          -   bandwidth, BW>=0

OUTPUT PARAMETERS
    S           -   sparse M*N matrix in SKS representation.
                    All elements are filled by zeros.
                    You may use sparseset() to change their values.

  -- ALGLIB PROJECT --
     Copyright 13.01.2014 by Bochkanov Sergey
*************************************************************************/
void sparsecreatesksbandbuf(ae_int_t m,
     ae_int_t n,
     ae_int_t bw,
     sparsematrix* s,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t minmn;
    ae_int_t nz;
    ae_int_t mxd;
    ae_int_t mxu;
    ae_int_t dui;


    ae_assert(m>0, "SparseCreateSKSBandBuf: M<=0", _state);
    ae_assert(n>0, "SparseCreateSKSBandBuf: N<=0", _state);
    ae_assert(m==n, "SparseCreateSKSBandBuf: M!=N", _state);
    ae_assert(bw>=0, "SparseCreateSKSBandBuf: BW<0", _state);
    minmn = ae_minint(m, n, _state);
    s->matrixtype = 2;
    s->ninitialized = 0;
    s->m = m;
    s->n = n;
    ivectorsetlengthatleast(&s->ridx, minmn+1, _state);
    s->ridx.ptr.p_int[0] = 0;
    nz = 0;
    for(i=0; i<=minmn-1; i++)
    {
        dui = ae_minint(i, bw, _state);
        nz = nz+1+2*dui;
        s->ridx.ptr.p_int[i+1] = s->ridx.ptr.p_int[i]+1+2*dui;
    }
    rvectorsetlengthatleast(&s->vals, nz, _state);
    for(i=0; i<=nz-1; i++)
    {
        s->vals.ptr.p_double[i] = 0.0;
    }
    ivectorsetlengthatleast(&s->didx, m+1, _state);
    mxd = 0;
    for(i=0; i<=m-1; i++)
    {
        dui = ae_minint(i, bw, _state);
        s->didx.ptr.p_int[i] = dui;
        mxd = ae_maxint(mxd, dui, _state);
    }
    s->didx.ptr.p_int[m] = mxd;
    ivectorsetlengthatleast(&s->uidx, n+1, _state);
    mxu = 0;
    for(i=0; i<=n-1; i++)
    {
        dui = ae_minint(i, bw, _state);
        s->uidx.ptr.p_int[i] = dui;
        mxu = ae_maxint(mxu, dui, _state);
    }
    s->uidx.ptr.p_int[n] = mxu;
}


/*************************************************************************
This function copies S0 to S1.
This function completely deallocates memory owned by S1 before creating a
copy of S0. If you want to reuse memory, use SparseCopyBuf.

NOTE:  this  function  does  not verify its arguments, it just copies all
fields of the structure.

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
void sparsecopy(const sparsematrix* s0,
     sparsematrix* s1,
     ae_state *_state)
{

    _sparsematrix_clear(s1);

    sparsecopybuf(s0, s1, _state);
}


/*************************************************************************
This function copies S0 to S1.
Memory already allocated in S1 is reused as much as possible.

NOTE:  this  function  does  not verify its arguments, it just copies all
fields of the structure.

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
void sparsecopybuf(const sparsematrix* s0,
     sparsematrix* s1,
     ae_state *_state)
{
    ae_int_t l;
    ae_int_t i;


    s1->matrixtype = s0->matrixtype;
    s1->m = s0->m;
    s1->n = s0->n;
    s1->nfree = s0->nfree;
    s1->ninitialized = s0->ninitialized;
    s1->tablesize = s0->tablesize;
    
    /*
     * Initialization for arrays
     */
    l = s0->vals.cnt;
    rvectorsetlengthatleast(&s1->vals, l, _state);
    for(i=0; i<=l-1; i++)
    {
        s1->vals.ptr.p_double[i] = s0->vals.ptr.p_double[i];
    }
    l = s0->ridx.cnt;
    ivectorsetlengthatleast(&s1->ridx, l, _state);
    for(i=0; i<=l-1; i++)
    {
        s1->ridx.ptr.p_int[i] = s0->ridx.ptr.p_int[i];
    }
    l = s0->idx.cnt;
    ivectorsetlengthatleast(&s1->idx, l, _state);
    for(i=0; i<=l-1; i++)
    {
        s1->idx.ptr.p_int[i] = s0->idx.ptr.p_int[i];
    }
    
    /*
     * Initalization for CRS-parameters
     */
    l = s0->uidx.cnt;
    ivectorsetlengthatleast(&s1->uidx, l, _state);
    for(i=0; i<=l-1; i++)
    {
        s1->uidx.ptr.p_int[i] = s0->uidx.ptr.p_int[i];
    }
    l = s0->didx.cnt;
    ivectorsetlengthatleast(&s1->didx, l, _state);
    for(i=0; i<=l-1; i++)
    {
        s1->didx.ptr.p_int[i] = s0->didx.ptr.p_int[i];
    }
}


/*************************************************************************
This function efficiently swaps contents of S0 and S1.

  -- ALGLIB PROJECT --
     Copyright 16.01.2014 by Bochkanov Sergey
*************************************************************************/
void sparseswap(sparsematrix* s0, sparsematrix* s1, ae_state *_state)
{


    swapi(&s1->matrixtype, &s0->matrixtype, _state);
    swapi(&s1->m, &s0->m, _state);
    swapi(&s1->n, &s0->n, _state);
    swapi(&s1->nfree, &s0->nfree, _state);
    swapi(&s1->ninitialized, &s0->ninitialized, _state);
    swapi(&s1->tablesize, &s0->tablesize, _state);
    ae_swap_vectors(&s1->vals, &s0->vals);
    ae_swap_vectors(&s1->ridx, &s0->ridx);
    ae_swap_vectors(&s1->idx, &s0->idx);
    ae_swap_vectors(&s1->uidx, &s0->uidx);
    ae_swap_vectors(&s1->didx, &s0->didx);
}


/*************************************************************************
This function adds value to S[i,j] - element of the sparse matrix. Matrix
must be in a Hash-Table mode.

In case S[i,j] already exists in the table, V i added to  its  value.  In
case  S[i,j]  is  non-existent,  it  is  inserted  in  the  table.  Table
automatically grows when necessary.

INPUT PARAMETERS
    S           -   sparse M*N matrix in Hash-Table representation.
                    Exception will be thrown for CRS matrix.
    I           -   row index of the element to modify, 0<=I<M
    J           -   column index of the element to modify, 0<=J<N
    V           -   value to add, must be finite number

OUTPUT PARAMETERS
    S           -   modified matrix
    
NOTE 1:  when  S[i,j]  is exactly zero after modification, it is  deleted
from the table.

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
void sparseadd(sparsematrix* s,
     ae_int_t i,
     ae_int_t j,
     double v,
     ae_state *_state)
{
    ae_int_t hashcode;
    ae_int_t tcode;
    ae_int_t k;


    ae_assert(s->matrixtype==0, "SparseAdd: matrix must be in the Hash-Table mode to do this operation", _state);
    ae_assert(i>=0, "SparseAdd: I<0", _state);
    ae_assert(i<s->m, "SparseAdd: I>=M", _state);
    ae_assert(j>=0, "SparseAdd: J<0", _state);
    ae_assert(j<s->n, "SparseAdd: J>=N", _state);
    ae_assert(ae_isfinite(v, _state), "SparseAdd: V is not finite number", _state);
    if( ae_fp_eq(v,(double)(0)) )
    {
        return;
    }
    tcode = -1;
    k = s->tablesize;
    if( ae_fp_greater_eq(((double)1-sparse_maxloadfactor)*(double)k,(double)(s->nfree)) )
    {
        sparseresizematrix(s, _state);
        k = s->tablesize;
    }
    hashcode = sparse_hash(i, j, k, _state);
    for(;;)
    {
        if( s->idx.ptr.p_int[2*hashcode]==-1 )
        {
            if( tcode!=-1 )
            {
                hashcode = tcode;
            }
            s->vals.ptr.p_double[hashcode] = v;
            s->idx.ptr.p_int[2*hashcode] = i;
            s->idx.ptr.p_int[2*hashcode+1] = j;
            if( tcode==-1 )
            {
                s->nfree = s->nfree-1;
            }
            return;
        }
        else
        {
            if( s->idx.ptr.p_int[2*hashcode]==i&&s->idx.ptr.p_int[2*hashcode+1]==j )
            {
                s->vals.ptr.p_double[hashcode] = s->vals.ptr.p_double[hashcode]+v;
                if( ae_fp_eq(s->vals.ptr.p_double[hashcode],(double)(0)) )
                {
                    s->idx.ptr.p_int[2*hashcode] = -2;
                }
                return;
            }
            
            /*
             * Is it deleted element?
             */
            if( tcode==-1&&s->idx.ptr.p_int[2*hashcode]==-2 )
            {
                tcode = hashcode;
            }
            
            /*
             * Next step
             */
            hashcode = (hashcode+1)%k;
        }
    }
}


/*************************************************************************
This function modifies S[i,j] - element of the sparse matrix.

For Hash-based storage format:
* this function can be called at any moment - during matrix initialization
  or later
* new value can be zero or non-zero.  In case new value of S[i,j] is zero,
  this element is deleted from the table.
* this  function  has  no  effect when called with zero V for non-existent
  element.

For CRS-bases storage format:
* this function can be called ONLY DURING MATRIX INITIALIZATION
* zero values are stored in the matrix similarly to non-zero ones
* elements must be initialized in correct order -  from top row to bottom,
  within row - from left to right.
  
For SKS storage:
* this function can be called at any moment - during matrix initialization
  or later
* zero values are stored in the matrix similarly to non-zero ones
* this function CAN NOT be called for non-existent (outside  of  the  band
  specified during SKS matrix creation) elements. Say, if you created  SKS
  matrix  with  bandwidth=2  and  tried to call sparseset(s,0,10,VAL),  an
  exception will be generated.

INPUT PARAMETERS
    S           -   sparse M*N matrix in Hash-Table, SKS or CRS format.
    I           -   row index of the element to modify, 0<=I<M
    J           -   column index of the element to modify, 0<=J<N
    V           -   value to set, must be finite number, can be zero

OUTPUT PARAMETERS
    S           -   modified matrix

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
void sparseset(sparsematrix* s,
     ae_int_t i,
     ae_int_t j,
     double v,
     ae_state *_state)
{
    ae_int_t hashcode;
    ae_int_t tcode;
    ae_int_t k;
    ae_bool b;


    ae_assert((s->matrixtype==0||s->matrixtype==1)||s->matrixtype==2, "SparseSet: unsupported matrix storage format", _state);
    ae_assert(i>=0, "SparseSet: I<0", _state);
    ae_assert(i<s->m, "SparseSet: I>=M", _state);
    ae_assert(j>=0, "SparseSet: J<0", _state);
    ae_assert(j<s->n, "SparseSet: J>=N", _state);
    ae_assert(ae_isfinite(v, _state), "SparseSet: V is not finite number", _state);
    
    /*
     * Hash-table matrix
     */
    if( s->matrixtype==0 )
    {
        tcode = -1;
        k = s->tablesize;
        if( ae_fp_greater_eq(((double)1-sparse_maxloadfactor)*(double)k,(double)(s->nfree)) )
        {
            sparseresizematrix(s, _state);
            k = s->tablesize;
        }
        hashcode = sparse_hash(i, j, k, _state);
        for(;;)
        {
            if( s->idx.ptr.p_int[2*hashcode]==-1 )
            {
                if( ae_fp_neq(v,(double)(0)) )
                {
                    if( tcode!=-1 )
                    {
                        hashcode = tcode;
                    }
                    s->vals.ptr.p_double[hashcode] = v;
                    s->idx.ptr.p_int[2*hashcode] = i;
                    s->idx.ptr.p_int[2*hashcode+1] = j;
                    if( tcode==-1 )
                    {
                        s->nfree = s->nfree-1;
                    }
                }
                return;
            }
            else
            {
                if( s->idx.ptr.p_int[2*hashcode]==i&&s->idx.ptr.p_int[2*hashcode+1]==j )
                {
                    if( ae_fp_eq(v,(double)(0)) )
                    {
                        s->idx.ptr.p_int[2*hashcode] = -2;
                    }
                    else
                    {
                        s->vals.ptr.p_double[hashcode] = v;
                    }
                    return;
                }
                if( tcode==-1&&s->idx.ptr.p_int[2*hashcode]==-2 )
                {
                    tcode = hashcode;
                }
                
                /*
                 * Next step
                 */
                hashcode = (hashcode+1)%k;
            }
        }
    }
    
    /*
     * CRS matrix
     */
    if( s->matrixtype==1 )
    {
        ae_assert(s->ridx.ptr.p_int[i]<=s->ninitialized, "SparseSet: too few initialized elements at some row (you have promised more when called SparceCreateCRS)", _state);
        ae_assert(s->ridx.ptr.p_int[i+1]>s->ninitialized, "SparseSet: too many initialized elements at some row (you have promised less when called SparceCreateCRS)", _state);
        ae_assert(s->ninitialized==s->ridx.ptr.p_int[i]||s->idx.ptr.p_int[s->ninitialized-1]<j, "SparseSet: incorrect column order (you must fill every row from left to right)", _state);
        s->vals.ptr.p_double[s->ninitialized] = v;
        s->idx.ptr.p_int[s->ninitialized] = j;
        s->ninitialized = s->ninitialized+1;
        
        /*
         * If matrix has been created then
         * initiale 'S.UIdx' and 'S.DIdx'
         */
        if( s->ninitialized==s->ridx.ptr.p_int[s->m] )
        {
            sparseinitduidx(s, _state);
        }
        return;
    }
    
    /*
     * SKS matrix
     */
    if( s->matrixtype==2 )
    {
        b = sparserewriteexisting(s, i, j, v, _state);
        ae_assert(b, "SparseSet: an attempt to initialize out-of-band element of the SKS matrix", _state);
        return;
    }
}


/*************************************************************************
This function returns S[i,j] - element of the sparse matrix.  Matrix  can
be in any mode (Hash-Table, CRS, SKS), but this function is less efficient
for CRS matrices. Hash-Table and SKS matrices can find  element  in  O(1)
time, while  CRS  matrices need O(log(RS)) time, where RS is an number of
non-zero elements in a row.

INPUT PARAMETERS
    S           -   sparse M*N matrix
    I           -   row index of the element to modify, 0<=I<M
    J           -   column index of the element to modify, 0<=J<N

RESULT
    value of S[I,J] or zero (in case no element with such index is found)

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
double sparseget(const sparsematrix* s,
     ae_int_t i,
     ae_int_t j,
     ae_state *_state)
{
    ae_int_t hashcode;
    ae_int_t k;
    ae_int_t k0;
    ae_int_t k1;
    double result;


    ae_assert(i>=0, "SparseGet: I<0", _state);
    ae_assert(i<s->m, "SparseGet: I>=M", _state);
    ae_assert(j>=0, "SparseGet: J<0", _state);
    ae_assert(j<s->n, "SparseGet: J>=N", _state);
    result = 0.0;
    if( s->matrixtype==0 )
    {
        
        /*
         * Hash-based storage
         */
        result = (double)(0);
        k = s->tablesize;
        hashcode = sparse_hash(i, j, k, _state);
        for(;;)
        {
            if( s->idx.ptr.p_int[2*hashcode]==-1 )
            {
                return result;
            }
            if( s->idx.ptr.p_int[2*hashcode]==i&&s->idx.ptr.p_int[2*hashcode+1]==j )
            {
                result = s->vals.ptr.p_double[hashcode];
                return result;
            }
            hashcode = (hashcode+1)%k;
        }
    }
    if( s->matrixtype==1 )
    {
        
        /*
         * CRS
         */
        ae_assert(s->ninitialized==s->ridx.ptr.p_int[s->m], "SparseGet: some rows/elements of the CRS matrix were not initialized (you must initialize everything you promised to SparseCreateCRS)", _state);
        k0 = s->ridx.ptr.p_int[i];
        k1 = s->ridx.ptr.p_int[i+1]-1;
        result = (double)(0);
        while(k0<=k1)
        {
            k = (k0+k1)/2;
            if( s->idx.ptr.p_int[k]==j )
            {
                result = s->vals.ptr.p_double[k];
                return result;
            }
            if( s->idx.ptr.p_int[k]<j )
            {
                k0 = k+1;
            }
            else
            {
                k1 = k-1;
            }
        }
        return result;
    }
    if( s->matrixtype==2 )
    {
        
        /*
         * SKS
         */
        ae_assert(s->m==s->n, "SparseGet: non-square SKS matrix not supported", _state);
        result = (double)(0);
        if( i==j )
        {
            
            /*
             * Return diagonal element
             */
            result = s->vals.ptr.p_double[s->ridx.ptr.p_int[i]+s->didx.ptr.p_int[i]];
            return result;
        }
        if( j<i )
        {
            
            /*
             * Return subdiagonal element at I-th "skyline block"
             */
            k = s->didx.ptr.p_int[i];
            if( i-j<=k )
            {
                result = s->vals.ptr.p_double[s->ridx.ptr.p_int[i]+k+j-i];
            }
        }
        else
        {
            
            /*
             * Return superdiagonal element at J-th "skyline block"
             */
            k = s->uidx.ptr.p_int[j];
            if( j-i<=k )
            {
                result = s->vals.ptr.p_double[s->ridx.ptr.p_int[j+1]-(j-i)];
            }
            return result;
        }
        return result;
    }
    ae_assert(ae_false, "SparseGet: unexpected matrix type", _state);
    return result;
}


/*************************************************************************
This function checks whether S[i,j] is present in the sparse  matrix.  It
returns True even for elements  that  are  numerically  zero  (but  still
have place allocated for them).

The matrix  can be in any mode (Hash-Table, CRS, SKS), but this  function
is less efficient for CRS matrices. Hash-Table and SKS matrices can  find
element in O(1) time, while  CRS  matrices need O(log(RS)) time, where RS
is an number of non-zero elements in a row.

INPUT PARAMETERS
    S           -   sparse M*N matrix
    I           -   row index of the element to modify, 0<=I<M
    J           -   column index of the element to modify, 0<=J<N

RESULT
    whether S[I,J] is present in the data structure or not

  -- ALGLIB PROJECT --
     Copyright 14.10.2020 by Bochkanov Sergey
*************************************************************************/
ae_bool sparseexists(const sparsematrix* s,
     ae_int_t i,
     ae_int_t j,
     ae_state *_state)
{
    ae_int_t hashcode;
    ae_int_t k;
    ae_int_t k0;
    ae_int_t k1;
    ae_bool result;


    ae_assert(i>=0, "SparseExists: I<0", _state);
    ae_assert(i<s->m, "SparseExists: I>=M", _state);
    ae_assert(j>=0, "SparseExists: J<0", _state);
    ae_assert(j<s->n, "SparseExists: J>=N", _state);
    result = ae_false;
    if( s->matrixtype==0 )
    {
        
        /*
         * Hash-based storage
         */
        k = s->tablesize;
        hashcode = sparse_hash(i, j, k, _state);
        for(;;)
        {
            if( s->idx.ptr.p_int[2*hashcode]==-1 )
            {
                return result;
            }
            if( s->idx.ptr.p_int[2*hashcode]==i&&s->idx.ptr.p_int[2*hashcode+1]==j )
            {
                result = ae_true;
                return result;
            }
            hashcode = (hashcode+1)%k;
        }
    }
    if( s->matrixtype==1 )
    {
        
        /*
         * CRS
         */
        ae_assert(s->ninitialized==s->ridx.ptr.p_int[s->m], "SparseExists: some rows/elements of the CRS matrix were not initialized (you must initialize everything you promised to SparseCreateCRS)", _state);
        k0 = s->ridx.ptr.p_int[i];
        k1 = s->ridx.ptr.p_int[i+1]-1;
        while(k0<=k1)
        {
            k = (k0+k1)/2;
            if( s->idx.ptr.p_int[k]==j )
            {
                result = ae_true;
                return result;
            }
            if( s->idx.ptr.p_int[k]<j )
            {
                k0 = k+1;
            }
            else
            {
                k1 = k-1;
            }
        }
        return result;
    }
    if( s->matrixtype==2 )
    {
        
        /*
         * SKS
         */
        ae_assert(s->m==s->n, "SparseExists: non-square SKS matrix not supported", _state);
        if( i==j )
        {
            
            /*
             * Return diagonal element
             */
            result = ae_true;
            return result;
        }
        if( j<i )
        {
            
            /*
             * Return subdiagonal element at I-th "skyline block"
             */
            if( i-j<=s->didx.ptr.p_int[i] )
            {
                result = ae_true;
            }
        }
        else
        {
            
            /*
             * Return superdiagonal element at J-th "skyline block"
             */
            if( j-i<=s->uidx.ptr.p_int[j] )
            {
                result = ae_true;
            }
            return result;
        }
        return result;
    }
    ae_assert(ae_false, "SparseExists: unexpected matrix type", _state);
    return result;
}


/*************************************************************************
This function returns I-th diagonal element of the sparse matrix.

Matrix can be in any mode (Hash-Table or CRS storage), but this  function
is most efficient for CRS matrices - it requires less than 50 CPU  cycles
to extract diagonal element. For Hash-Table matrices we still  have  O(1)
query time, but function is many times slower.

INPUT PARAMETERS
    S           -   sparse M*N matrix in Hash-Table representation.
                    Exception will be thrown for CRS matrix.
    I           -   index of the element to modify, 0<=I<min(M,N)

RESULT
    value of S[I,I] or zero (in case no element with such index is found)

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
double sparsegetdiagonal(const sparsematrix* s,
     ae_int_t i,
     ae_state *_state)
{
    double result;


    ae_assert(i>=0, "SparseGetDiagonal: I<0", _state);
    ae_assert(i<s->m, "SparseGetDiagonal: I>=M", _state);
    ae_assert(i<s->n, "SparseGetDiagonal: I>=N", _state);
    result = (double)(0);
    if( s->matrixtype==0 )
    {
        result = sparseget(s, i, i, _state);
        return result;
    }
    if( s->matrixtype==1 )
    {
        if( s->didx.ptr.p_int[i]!=s->uidx.ptr.p_int[i] )
        {
            result = s->vals.ptr.p_double[s->didx.ptr.p_int[i]];
        }
        return result;
    }
    if( s->matrixtype==2 )
    {
        ae_assert(s->m==s->n, "SparseGetDiagonal: non-square SKS matrix not supported", _state);
        result = s->vals.ptr.p_double[s->ridx.ptr.p_int[i]+s->didx.ptr.p_int[i]];
        return result;
    }
    ae_assert(ae_false, "SparseGetDiagonal: unexpected matrix type", _state);
    return result;
}


/*************************************************************************
This function calculates matrix-vector product  S*x.  Matrix  S  must  be
stored in CRS or SKS format (exception will be thrown otherwise).

INPUT PARAMETERS
    S           -   sparse M*N matrix in CRS or SKS format.
    X           -   array[N], input vector. For  performance  reasons  we 
                    make only quick checks - we check that array size  is
                    at least N, but we do not check for NAN's or INF's.
    Y           -   output buffer, possibly preallocated. In case  buffer
                    size is too small to store  result,  this  buffer  is
                    automatically resized.
    
OUTPUT PARAMETERS
    Y           -   array[M], S*x
    
NOTE: this function throws exception when called for non-CRS/SKS  matrix.
You must convert your matrix with SparseConvertToCRS/SKS()  before  using
this function.

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
void sparsemv(const sparsematrix* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    double tval;
    double v;
    double vv;
    ae_int_t i;
    ae_int_t j;
    ae_int_t lt;
    ae_int_t rt;
    ae_int_t lt1;
    ae_int_t rt1;
    ae_int_t n;
    ae_int_t m;
    ae_int_t d;
    ae_int_t u;
    ae_int_t ri;
    ae_int_t ri1;


    ae_assert(x->cnt>=s->n, "SparseMV: length(X)<N", _state);
    ae_assert(s->matrixtype==1||s->matrixtype==2, "SparseMV: incorrect matrix type (convert your matrix to CRS/SKS)", _state);
    rvectorsetlengthatleast(y, s->m, _state);
    n = s->n;
    m = s->m;
    if( s->matrixtype==1 )
    {
        
        /*
         * CRS format.
         * Perform integrity check.
         */
        ae_assert(s->ninitialized==s->ridx.ptr.p_int[s->m], "SparseMV: some rows/elements of the CRS matrix were not initialized (you must initialize everything you promised to SparseCreateCRS)", _state);
        
        /*
         * Try vendor kernels
         */
        if( sparsegemvcrsmkl(0, s->m, s->n, 1.0, &s->vals, &s->idx, &s->ridx, x, 0, 0.0, y, 0, _state) )
        {
            return;
        }
        
        /*
         * Our own implementation
         */
        for(i=0; i<=m-1; i++)
        {
            tval = (double)(0);
            lt = s->ridx.ptr.p_int[i];
            rt = s->ridx.ptr.p_int[i+1]-1;
            for(j=lt; j<=rt; j++)
            {
                tval = tval+x->ptr.p_double[s->idx.ptr.p_int[j]]*s->vals.ptr.p_double[j];
            }
            y->ptr.p_double[i] = tval;
        }
        return;
    }
    if( s->matrixtype==2 )
    {
        
        /*
         * SKS format
         */
        ae_assert(s->m==s->n, "SparseMV: non-square SKS matrices are not supported", _state);
        for(i=0; i<=n-1; i++)
        {
            ri = s->ridx.ptr.p_int[i];
            ri1 = s->ridx.ptr.p_int[i+1];
            d = s->didx.ptr.p_int[i];
            u = s->uidx.ptr.p_int[i];
            v = s->vals.ptr.p_double[ri+d]*x->ptr.p_double[i];
            if( d>0 )
            {
                lt = ri;
                rt = ri+d-1;
                lt1 = i-d;
                rt1 = i-1;
                vv = ae_v_dotproduct(&s->vals.ptr.p_double[lt], 1, &x->ptr.p_double[lt1], 1, ae_v_len(lt,rt));
                v = v+vv;
            }
            y->ptr.p_double[i] = v;
            if( u>0 )
            {
                raddvx(u, x->ptr.p_double[i], &s->vals, ri1-u, y, i-u, _state);
            }
        }
        touchint(&rt1, _state);
        return;
    }
}


/*************************************************************************
This function calculates matrix-vector product  S^T*x. Matrix S  must  be
stored in CRS or SKS format (exception will be thrown otherwise).

INPUT PARAMETERS
    S           -   sparse M*N matrix in CRS or SKS format.
    X           -   array[M], input vector. For  performance  reasons  we 
                    make only quick checks - we check that array size  is
                    at least M, but we do not check for NAN's or INF's.
    Y           -   output buffer, possibly preallocated. In case  buffer
                    size is too small to store  result,  this  buffer  is
                    automatically resized.
    
OUTPUT PARAMETERS
    Y           -   array[N], S^T*x
    
NOTE: this function throws exception when called for non-CRS/SKS  matrix.
You must convert your matrix with SparseConvertToCRS/SKS()  before  using
this function.

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
void sparsemtv(const sparsematrix* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t lt;
    ae_int_t rt;
    ae_int_t ct;
    ae_int_t lt1;
    ae_int_t rt1;
    double v;
    double vv;
    ae_int_t n;
    ae_int_t m;
    ae_int_t ri;
    ae_int_t ri1;
    ae_int_t d;
    ae_int_t u;


    ae_assert(s->matrixtype==1||s->matrixtype==2, "SparseMTV: incorrect matrix type (convert your matrix to CRS/SKS)", _state);
    ae_assert(x->cnt>=s->m, "SparseMTV: Length(X)<M", _state);
    n = s->n;
    m = s->m;
    rvectorsetlengthatleast(y, n, _state);
    for(i=0; i<=n-1; i++)
    {
        y->ptr.p_double[i] = (double)(0);
    }
    if( s->matrixtype==1 )
    {
        
        /*
         * CRS format
         * Perform integrity check.
         */
        ae_assert(s->ninitialized==s->ridx.ptr.p_int[m], "SparseMTV: some rows/elements of the CRS matrix were not initialized (you must initialize everything you promised to SparseCreateCRS)", _state);
        
        /*
         * Try vendor kernels
         */
        if( sparsegemvcrsmkl(1, s->m, s->n, 1.0, &s->vals, &s->idx, &s->ridx, x, 0, 0.0, y, 0, _state) )
        {
            return;
        }
        
        /*
         * Our own implementation
         */
        for(i=0; i<=m-1; i++)
        {
            lt = s->ridx.ptr.p_int[i];
            rt = s->ridx.ptr.p_int[i+1];
            v = x->ptr.p_double[i];
            for(j=lt; j<=rt-1; j++)
            {
                ct = s->idx.ptr.p_int[j];
                y->ptr.p_double[ct] = y->ptr.p_double[ct]+v*s->vals.ptr.p_double[j];
            }
        }
        return;
    }
    if( s->matrixtype==2 )
    {
        
        /*
         * SKS format
         */
        ae_assert(s->m==s->n, "SparseMV: non-square SKS matrices are not supported", _state);
        for(i=0; i<=n-1; i++)
        {
            ri = s->ridx.ptr.p_int[i];
            ri1 = s->ridx.ptr.p_int[i+1];
            d = s->didx.ptr.p_int[i];
            u = s->uidx.ptr.p_int[i];
            if( d>0 )
            {
                lt = ri;
                lt1 = i-d;
                v = x->ptr.p_double[i];
                raddvx(d, v, &s->vals, lt, y, lt1, _state);
            }
            v = s->vals.ptr.p_double[ri+d]*x->ptr.p_double[i];
            if( u>0 )
            {
                lt = ri1-u;
                rt = ri1-1;
                lt1 = i-u;
                rt1 = i-1;
                vv = ae_v_dotproduct(&s->vals.ptr.p_double[lt], 1, &x->ptr.p_double[lt1], 1, ae_v_len(lt,rt));
                v = v+vv;
            }
            y->ptr.p_double[i] = v;
        }
        touchint(&rt1, _state);
        return;
    }
}


/*************************************************************************
This function calculates generalized sparse matrix-vector product

    y := alpha*op(S)*x + beta*y

Matrix S must be stored in CRS or SKS format (exception  will  be  thrown
otherwise). op(S) can be either S or S^T.

NOTE: this  function  expects  Y  to  be  large enough to store result. No
      automatic preallocation happens for smaller arrays.

INPUT PARAMETERS
    S           -   sparse matrix in CRS or SKS format.
    Alpha       -   source coefficient
    OpS         -   operation type:
                    * OpS=0     =>  op(S) = S
                    * OpS=1     =>  op(S) = S^T
    X           -   input vector, must have at least Cols(op(S))+IX elements
    IX          -   subvector offset
    Beta        -   destination coefficient
    Y           -   preallocated output array, must have at least Rows(op(S))+IY elements
    IY          -   subvector offset
    
OUTPUT PARAMETERS
    Y           -   elements [IY...IY+Rows(op(S))-1] are replaced by result,
                    other elements are not modified

HANDLING OF SPECIAL CASES:
* below M=Rows(op(S)) and N=Cols(op(S)). Although current  ALGLIB  version
  does not allow you to  create  zero-sized  sparse  matrices,  internally
  ALGLIB  can  deal  with  such matrices. So, comments for M or N equal to
  zero are for internal use only.
* if M=0, then subroutine does nothing. It does not even touch arrays.
* if N=0 or Alpha=0.0, then:
  * if Beta=0, then Y is filled by zeros. S and X are  not  referenced  at
    all. Initial values of Y are ignored (we do not  multiply  Y by  zero,
    we just rewrite it by zeros)
  * if Beta<>0, then Y is replaced by Beta*Y
* if M>0, N>0, Alpha<>0, but  Beta=0, then  Y is replaced by alpha*op(S)*x
  initial state of Y  is ignored (rewritten without initial multiplication
  by zeros).
    
NOTE: this function throws exception when called for non-CRS/SKS  matrix.
You must convert your matrix with SparseConvertToCRS/SKS()  before  using
this function.
     
  -- ALGLIB PROJECT --
     Copyright 10.12.2019 by Bochkanov Sergey
*************************************************************************/
void sparsegemv(const sparsematrix* s,
     double alpha,
     ae_int_t ops,
     /* Real    */ const ae_vector* x,
     ae_int_t ix,
     double beta,
     /* Real    */ ae_vector* y,
     ae_int_t iy,
     ae_state *_state)
{
    ae_int_t opm;
    ae_int_t opn;
    ae_int_t rawm;
    ae_int_t rawn;
    ae_int_t i;
    ae_int_t j;
    double tval;
    ae_int_t lt;
    ae_int_t rt;
    ae_int_t ct;
    ae_int_t d;
    ae_int_t u;
    ae_int_t ri;
    ae_int_t ri1;
    double v;
    double vv;
    ae_int_t lt1;
    ae_int_t rt1;


    ae_assert(ops==0||ops==1, "SparseGEMV: incorrect OpS", _state);
    ae_assert(s->matrixtype==1||s->matrixtype==2, "SparseGEMV: incorrect matrix type (convert your matrix to CRS/SKS)", _state);
    if( ops==0 )
    {
        opm = s->m;
        opn = s->n;
    }
    else
    {
        opm = s->n;
        opn = s->m;
    }
    ae_assert(opm>=0&&opn>=0, "SparseGEMV: op(S) has negative size", _state);
    ae_assert(opn==0||x->cnt+ix>=opn, "SparseGEMV: X is too short", _state);
    ae_assert(opm==0||y->cnt+iy>=opm, "SparseGEMV: X is too short", _state);
    rawm = s->m;
    rawn = s->n;
    
    /*
     * Quick exit strategies
     */
    if( opm==0 )
    {
        return;
    }
    if( ae_fp_neq(beta,(double)(0)) )
    {
        for(i=0; i<=opm-1; i++)
        {
            y->ptr.p_double[iy+i] = beta*y->ptr.p_double[iy+i];
        }
    }
    else
    {
        for(i=0; i<=opm-1; i++)
        {
            y->ptr.p_double[iy+i] = 0.0;
        }
    }
    if( opn==0||ae_fp_eq(alpha,(double)(0)) )
    {
        return;
    }
    
    /*
     * Now we have OpM>=1, OpN>=1, Alpha<>0
     */
    if( ops==0 )
    {
        
        /*
         * Compute generalized product y := alpha*S*x + beta*y
         * (with "beta*y" part already computed).
         */
        if( s->matrixtype==1 )
        {
            
            /*
             * CRS format.
             * Perform integrity check.
             */
            ae_assert(s->ninitialized==s->ridx.ptr.p_int[s->m], "SparseGEMV: some rows/elements of the CRS matrix were not initialized (you must initialize everything you promised to SparseCreateCRS)", _state);
            
            /*
             * Try vendor kernels
             */
            if( sparsegemvcrsmkl(0, s->m, s->n, alpha, &s->vals, &s->idx, &s->ridx, x, ix, 1.0, y, iy, _state) )
            {
                return;
            }
            
            /*
             * Our own implementation
             */
            for(i=0; i<=rawm-1; i++)
            {
                tval = (double)(0);
                lt = s->ridx.ptr.p_int[i];
                rt = s->ridx.ptr.p_int[i+1]-1;
                for(j=lt; j<=rt; j++)
                {
                    tval = tval+x->ptr.p_double[s->idx.ptr.p_int[j]+ix]*s->vals.ptr.p_double[j];
                }
                y->ptr.p_double[i+iy] = alpha*tval+y->ptr.p_double[i+iy];
            }
            return;
        }
        if( s->matrixtype==2 )
        {
            
            /*
             * SKS format
             */
            ae_assert(s->m==s->n, "SparseMV: non-square SKS matrices are not supported", _state);
            for(i=0; i<=rawn-1; i++)
            {
                ri = s->ridx.ptr.p_int[i];
                ri1 = s->ridx.ptr.p_int[i+1];
                d = s->didx.ptr.p_int[i];
                u = s->uidx.ptr.p_int[i];
                v = s->vals.ptr.p_double[ri+d]*x->ptr.p_double[i+ix];
                if( d>0 )
                {
                    lt = ri;
                    rt = ri+d-1;
                    lt1 = i-d+ix;
                    rt1 = i-1+ix;
                    vv = ae_v_dotproduct(&s->vals.ptr.p_double[lt], 1, &x->ptr.p_double[lt1], 1, ae_v_len(lt,rt));
                    v = v+vv;
                }
                y->ptr.p_double[i+iy] = alpha*v+y->ptr.p_double[i+iy];
                if( u>0 )
                {
                    raddvx(u, alpha*x->ptr.p_double[i+ix], &s->vals, ri1-u, y, i-u+iy, _state);
                }
            }
            touchint(&rt1, _state);
            return;
        }
    }
    else
    {
        
        /*
         * Compute generalized product y := alpha*S^T*x + beta*y
         * (with "beta*y" part already computed).
         */
        if( s->matrixtype==1 )
        {
            
            /*
             * CRS format
             * Perform integrity check.
             */
            ae_assert(s->ninitialized==s->ridx.ptr.p_int[s->m], "SparseGEMV: some rows/elements of the CRS matrix were not initialized (you must initialize everything you promised to SparseCreateCRS)", _state);
            
            /*
             * Try vendor kernels
             */
            if( sparsegemvcrsmkl(1, s->m, s->n, alpha, &s->vals, &s->idx, &s->ridx, x, ix, 1.0, y, iy, _state) )
            {
                return;
            }
            
            /*
             * Our own implementation
             */
            for(i=0; i<=rawm-1; i++)
            {
                lt = s->ridx.ptr.p_int[i];
                rt = s->ridx.ptr.p_int[i+1];
                v = alpha*x->ptr.p_double[i+ix];
                for(j=lt; j<=rt-1; j++)
                {
                    ct = s->idx.ptr.p_int[j]+iy;
                    y->ptr.p_double[ct] = y->ptr.p_double[ct]+v*s->vals.ptr.p_double[j];
                }
            }
            return;
        }
        if( s->matrixtype==2 )
        {
            
            /*
             * SKS format
             */
            ae_assert(s->m==s->n, "SparseGEMV: non-square SKS matrices are not supported", _state);
            for(i=0; i<=rawn-1; i++)
            {
                ri = s->ridx.ptr.p_int[i];
                ri1 = s->ridx.ptr.p_int[i+1];
                d = s->didx.ptr.p_int[i];
                u = s->uidx.ptr.p_int[i];
                if( d>0 )
                {
                    raddvx(d, alpha*x->ptr.p_double[i+ix], &s->vals, ri, y, i-d+iy, _state);
                }
                v = alpha*s->vals.ptr.p_double[ri+d]*x->ptr.p_double[i+ix];
                if( u>0 )
                {
                    lt = ri1-u;
                    rt = ri1-1;
                    lt1 = i-u+ix;
                    rt1 = i-1+ix;
                    vv = ae_v_dotproduct(&s->vals.ptr.p_double[lt], 1, &x->ptr.p_double[lt1], 1, ae_v_len(lt,rt));
                    v = v+alpha*vv;
                }
                y->ptr.p_double[i+iy] = v+y->ptr.p_double[i+iy];
            }
            touchint(&rt1, _state);
            return;
        }
    }
}


/*************************************************************************
This function simultaneously calculates two matrix-vector products:
    S*x and S^T*x.
S must be square (non-rectangular) matrix stored in  CRS  or  SKS  format
(exception will be thrown otherwise).

INPUT PARAMETERS
    S           -   sparse N*N matrix in CRS or SKS format.
    X           -   array[N], input vector. For  performance  reasons  we 
                    make only quick checks - we check that array size  is
                    at least N, but we do not check for NAN's or INF's.
    Y0          -   output buffer, possibly preallocated. In case  buffer
                    size is too small to store  result,  this  buffer  is
                    automatically resized.
    Y1          -   output buffer, possibly preallocated. In case  buffer
                    size is too small to store  result,  this  buffer  is
                    automatically resized.
    
OUTPUT PARAMETERS
    Y0          -   array[N], S*x
    Y1          -   array[N], S^T*x
    
NOTE: this function throws exception when called for non-CRS/SKS  matrix.
You must convert your matrix with SparseConvertToCRS/SKS()  before  using
this function.

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
void sparsemv2(const sparsematrix* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y0,
     /* Real    */ ae_vector* y1,
     ae_state *_state)
{
    ae_int_t l;
    double tval;
    ae_int_t i;
    ae_int_t j;
    double vx;
    double vs;
    double v;
    double vv;
    double vd0;
    double vd1;
    ae_int_t vi;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t n;
    ae_int_t ri;
    ae_int_t ri1;
    ae_int_t d;
    ae_int_t u;
    ae_int_t lt;
    ae_int_t rt;
    ae_int_t lt1;
    ae_int_t rt1;


    ae_assert(s->matrixtype==1||s->matrixtype==2, "SparseMV2: incorrect matrix type (convert your matrix to CRS/SKS)", _state);
    ae_assert(s->m==s->n, "SparseMV2: matrix is non-square", _state);
    l = x->cnt;
    ae_assert(l>=s->n, "SparseMV2: Length(X)<N", _state);
    n = s->n;
    rvectorsetlengthatleast(y0, l, _state);
    rvectorsetlengthatleast(y1, l, _state);
    for(i=0; i<=n-1; i++)
    {
        y0->ptr.p_double[i] = (double)(0);
        y1->ptr.p_double[i] = (double)(0);
    }
    if( s->matrixtype==1 )
    {
        
        /*
         * CRS format
         */
        ae_assert(s->ninitialized==s->ridx.ptr.p_int[s->m], "SparseMV2: some rows/elements of the CRS matrix were not initialized (you must initialize everything you promised to SparseCreateCRS)", _state);
        for(i=0; i<=s->m-1; i++)
        {
            tval = (double)(0);
            vx = x->ptr.p_double[i];
            j0 = s->ridx.ptr.p_int[i];
            j1 = s->ridx.ptr.p_int[i+1]-1;
            for(j=j0; j<=j1; j++)
            {
                vi = s->idx.ptr.p_int[j];
                vs = s->vals.ptr.p_double[j];
                tval = tval+x->ptr.p_double[vi]*vs;
                y1->ptr.p_double[vi] = y1->ptr.p_double[vi]+vx*vs;
            }
            y0->ptr.p_double[i] = tval;
        }
        return;
    }
    if( s->matrixtype==2 )
    {
        
        /*
         * SKS format
         */
        for(i=0; i<=n-1; i++)
        {
            ri = s->ridx.ptr.p_int[i];
            ri1 = s->ridx.ptr.p_int[i+1];
            d = s->didx.ptr.p_int[i];
            u = s->uidx.ptr.p_int[i];
            vd0 = s->vals.ptr.p_double[ri+d]*x->ptr.p_double[i];
            vd1 = vd0;
            if( d>0 )
            {
                lt = ri;
                rt = ri+d-1;
                lt1 = i-d;
                rt1 = i-1;
                v = x->ptr.p_double[i];
                ae_v_addd(&y1->ptr.p_double[lt1], 1, &s->vals.ptr.p_double[lt], 1, ae_v_len(lt1,rt1), v);
                vv = ae_v_dotproduct(&s->vals.ptr.p_double[lt], 1, &x->ptr.p_double[lt1], 1, ae_v_len(lt,rt));
                vd0 = vd0+vv;
            }
            if( u>0 )
            {
                lt = ri1-u;
                rt = ri1-1;
                lt1 = i-u;
                rt1 = i-1;
                v = x->ptr.p_double[i];
                ae_v_addd(&y0->ptr.p_double[lt1], 1, &s->vals.ptr.p_double[lt], 1, ae_v_len(lt1,rt1), v);
                vv = ae_v_dotproduct(&s->vals.ptr.p_double[lt], 1, &x->ptr.p_double[lt1], 1, ae_v_len(lt,rt));
                vd1 = vd1+vv;
            }
            y0->ptr.p_double[i] = vd0;
            y1->ptr.p_double[i] = vd1;
        }
        return;
    }
}


/*************************************************************************
This function calculates matrix-vector product  S*x, when S is  symmetric
matrix. Matrix S  must be stored in CRS or SKS format  (exception will be
thrown otherwise).

INPUT PARAMETERS
    S           -   sparse M*M matrix in CRS or SKS format.
    IsUpper     -   whether upper or lower triangle of S is given:
                    * if upper triangle is given,  only   S[i,j] for j>=i
                      are used, and lower triangle is ignored (it can  be
                      empty - these elements are not referenced at all).
                    * if lower triangle is given,  only   S[i,j] for j<=i
                      are used, and upper triangle is ignored.
    X           -   array[N], input vector. For  performance  reasons  we 
                    make only quick checks - we check that array size  is
                    at least N, but we do not check for NAN's or INF's.
    Y           -   output buffer, possibly preallocated. In case  buffer
                    size is too small to store  result,  this  buffer  is
                    automatically resized.
    
OUTPUT PARAMETERS
    Y           -   array[M], S*x
    
NOTE: this function throws exception when called for non-CRS/SKS  matrix.
You must convert your matrix with SparseConvertToCRS/SKS()  before  using
this function.

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
void sparsesmv(const sparsematrix* s,
     ae_bool isupper,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    ae_int_t id;
    ae_int_t lt;
    ae_int_t rt;
    double v;
    double vv;
    double vy;
    double vx;
    double vd;
    ae_int_t ri;
    ae_int_t ri1;
    ae_int_t d;
    ae_int_t u;
    ae_int_t lt1;
    ae_int_t rt1;


    ae_assert(s->matrixtype==1||s->matrixtype==2, "SparseSMV: incorrect matrix type (convert your matrix to CRS/SKS)", _state);
    ae_assert(x->cnt>=s->n, "SparseSMV: length(X)<N", _state);
    ae_assert(s->m==s->n, "SparseSMV: non-square matrix", _state);
    n = s->n;
    rvectorsetlengthatleast(y, n, _state);
    for(i=0; i<=n-1; i++)
    {
        y->ptr.p_double[i] = (double)(0);
    }
    if( s->matrixtype==1 )
    {
        
        /*
         * CRS format
         */
        ae_assert(s->ninitialized==s->ridx.ptr.p_int[s->m], "SparseSMV: some rows/elements of the CRS matrix were not initialized (you must initialize everything you promised to SparseCreateCRS)", _state);
        for(i=0; i<=n-1; i++)
        {
            if( s->didx.ptr.p_int[i]!=s->uidx.ptr.p_int[i] )
            {
                y->ptr.p_double[i] = y->ptr.p_double[i]+s->vals.ptr.p_double[s->didx.ptr.p_int[i]]*x->ptr.p_double[s->idx.ptr.p_int[s->didx.ptr.p_int[i]]];
            }
            if( isupper )
            {
                lt = s->uidx.ptr.p_int[i];
                rt = s->ridx.ptr.p_int[i+1];
                vy = (double)(0);
                vx = x->ptr.p_double[i];
                for(j=lt; j<=rt-1; j++)
                {
                    id = s->idx.ptr.p_int[j];
                    v = s->vals.ptr.p_double[j];
                    vy = vy+x->ptr.p_double[id]*v;
                    y->ptr.p_double[id] = y->ptr.p_double[id]+vx*v;
                }
                y->ptr.p_double[i] = y->ptr.p_double[i]+vy;
            }
            else
            {
                lt = s->ridx.ptr.p_int[i];
                rt = s->didx.ptr.p_int[i];
                vy = (double)(0);
                vx = x->ptr.p_double[i];
                for(j=lt; j<=rt-1; j++)
                {
                    id = s->idx.ptr.p_int[j];
                    v = s->vals.ptr.p_double[j];
                    vy = vy+x->ptr.p_double[id]*v;
                    y->ptr.p_double[id] = y->ptr.p_double[id]+vx*v;
                }
                y->ptr.p_double[i] = y->ptr.p_double[i]+vy;
            }
        }
        return;
    }
    if( s->matrixtype==2 )
    {
        
        /*
         * SKS format
         */
        for(i=0; i<=n-1; i++)
        {
            ri = s->ridx.ptr.p_int[i];
            ri1 = s->ridx.ptr.p_int[i+1];
            d = s->didx.ptr.p_int[i];
            u = s->uidx.ptr.p_int[i];
            vd = s->vals.ptr.p_double[ri+d]*x->ptr.p_double[i];
            if( d>0&&!isupper )
            {
                lt = ri;
                rt = ri+d-1;
                lt1 = i-d;
                rt1 = i-1;
                v = x->ptr.p_double[i];
                ae_v_addd(&y->ptr.p_double[lt1], 1, &s->vals.ptr.p_double[lt], 1, ae_v_len(lt1,rt1), v);
                vv = ae_v_dotproduct(&s->vals.ptr.p_double[lt], 1, &x->ptr.p_double[lt1], 1, ae_v_len(lt,rt));
                vd = vd+vv;
            }
            if( u>0&&isupper )
            {
                lt = ri1-u;
                rt = ri1-1;
                lt1 = i-u;
                rt1 = i-1;
                v = x->ptr.p_double[i];
                ae_v_addd(&y->ptr.p_double[lt1], 1, &s->vals.ptr.p_double[lt], 1, ae_v_len(lt1,rt1), v);
                vv = ae_v_dotproduct(&s->vals.ptr.p_double[lt], 1, &x->ptr.p_double[lt1], 1, ae_v_len(lt,rt));
                vd = vd+vv;
            }
            y->ptr.p_double[i] = vd;
        }
        return;
    }
}


/*************************************************************************
This function calculates vector-matrix-vector product x'*S*x, where  S is
symmetric matrix. Matrix S must be stored in CRS or SKS format (exception
will be thrown otherwise).

INPUT PARAMETERS
    S           -   sparse M*M matrix in CRS or SKS format.
    IsUpper     -   whether upper or lower triangle of S is given:
                    * if upper triangle is given,  only   S[i,j] for j>=i
                      are used, and lower triangle is ignored (it can  be
                      empty - these elements are not referenced at all).
                    * if lower triangle is given,  only   S[i,j] for j<=i
                      are used, and upper triangle is ignored.
    X           -   array[N], input vector. For  performance  reasons  we 
                    make only quick checks - we check that array size  is
                    at least N, but we do not check for NAN's or INF's.
    
RESULT
    x'*S*x
    
NOTE: this function throws exception when called for non-CRS/SKS  matrix.
You must convert your matrix with SparseConvertToCRS/SKS()  before  using
this function.

  -- ALGLIB PROJECT --
     Copyright 27.01.2014 by Bochkanov Sergey
*************************************************************************/
double sparsevsmv(const sparsematrix* s,
     ae_bool isupper,
     /* Real    */ const ae_vector* x,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t id;
    ae_int_t lt;
    ae_int_t rt;
    double v;
    double v0;
    double v1;
    ae_int_t ri;
    ae_int_t ri1;
    ae_int_t d;
    ae_int_t u;
    ae_int_t lt1;
    double result;


    ae_assert(s->matrixtype==1||s->matrixtype==2, "SparseVSMV: incorrect matrix type (convert your matrix to CRS/SKS)", _state);
    ae_assert(x->cnt>=s->n, "SparseVSMV: length(X)<N", _state);
    ae_assert(s->m==s->n, "SparseVSMV: non-square matrix", _state);
    n = s->n;
    result = 0.0;
    if( s->matrixtype==1 )
    {
        
        /*
         * CRS format
         */
        ae_assert(s->ninitialized==s->ridx.ptr.p_int[s->m], "SparseVSMV: some rows/elements of the CRS matrix were not initialized (you must initialize everything you promised to SparseCreateCRS)", _state);
        for(i=0; i<=n-1; i++)
        {
            if( s->didx.ptr.p_int[i]!=s->uidx.ptr.p_int[i] )
            {
                v = x->ptr.p_double[s->idx.ptr.p_int[s->didx.ptr.p_int[i]]];
                result = result+v*s->vals.ptr.p_double[s->didx.ptr.p_int[i]]*v;
            }
            if( isupper )
            {
                lt = s->uidx.ptr.p_int[i];
                rt = s->ridx.ptr.p_int[i+1];
            }
            else
            {
                lt = s->ridx.ptr.p_int[i];
                rt = s->didx.ptr.p_int[i];
            }
            v0 = x->ptr.p_double[i];
            for(j=lt; j<=rt-1; j++)
            {
                id = s->idx.ptr.p_int[j];
                v1 = x->ptr.p_double[id];
                v = s->vals.ptr.p_double[j];
                result = result+(double)2*v0*v1*v;
            }
        }
        return result;
    }
    if( s->matrixtype==2 )
    {
        
        /*
         * SKS format
         */
        for(i=0; i<=n-1; i++)
        {
            ri = s->ridx.ptr.p_int[i];
            ri1 = s->ridx.ptr.p_int[i+1];
            d = s->didx.ptr.p_int[i];
            u = s->uidx.ptr.p_int[i];
            v = x->ptr.p_double[i];
            result = result+v*s->vals.ptr.p_double[ri+d]*v;
            if( d>0&&!isupper )
            {
                lt = ri;
                lt1 = i-d;
                k = d-1;
                v0 = x->ptr.p_double[i];
                v = 0.0;
                for(j=0; j<=k; j++)
                {
                    v = v+x->ptr.p_double[lt1+j]*s->vals.ptr.p_double[lt+j];
                }
                result = result+2.0*v0*v;
            }
            if( u>0&&isupper )
            {
                lt = ri1-u;
                lt1 = i-u;
                k = u-1;
                v0 = x->ptr.p_double[i];
                v = 0.0;
                for(j=0; j<=k; j++)
                {
                    v = v+x->ptr.p_double[lt1+j]*s->vals.ptr.p_double[lt+j];
                }
                result = result+2.0*v0*v;
            }
        }
        return result;
    }
    return result;
}


/*************************************************************************
This function calculates matrix-matrix product  S*A.  Matrix  S  must  be
stored in CRS or SKS format (exception will be thrown otherwise).

INPUT PARAMETERS
    S           -   sparse M*N matrix in CRS or SKS format.
    A           -   array[N][K], input dense matrix. For  performance reasons
                    we make only quick checks - we check that array size  
                    is at least N, but we do not check for NAN's or INF's.
    K           -   number of columns of matrix (A).
    B           -   output buffer, possibly preallocated. In case  buffer
                    size is too small to store  result,  this  buffer  is
                    automatically resized.
    
OUTPUT PARAMETERS
    B           -   array[M][K], S*A
    
NOTE: this function throws exception when called for non-CRS/SKS  matrix.
You must convert your matrix with SparseConvertToCRS/SKS()  before  using
this function.

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
void sparsemm(const sparsematrix* s,
     /* Real    */ const ae_matrix* a,
     ae_int_t k,
     /* Real    */ ae_matrix* b,
     ae_state *_state)
{
    double tval;
    double v;
    ae_int_t id;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k0;
    ae_int_t k1;
    ae_int_t lt;
    ae_int_t rt;
    ae_int_t m;
    ae_int_t n;
    ae_int_t ri;
    ae_int_t ri1;
    ae_int_t lt1;
    ae_int_t rt1;
    ae_int_t d;
    ae_int_t u;
    double vd;


    ae_assert(s->matrixtype==1||s->matrixtype==2, "SparseMM: incorrect matrix type (convert your matrix to CRS/SKS)", _state);
    ae_assert(a->rows>=s->n, "SparseMM: Rows(A)<N", _state);
    ae_assert(k>0, "SparseMM: K<=0", _state);
    m = s->m;
    n = s->n;
    k1 = k-1;
    rmatrixsetlengthatleast(b, m, k, _state);
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=k-1; j++)
        {
            b->ptr.pp_double[i][j] = (double)(0);
        }
    }
    if( s->matrixtype==1 )
    {
        
        /*
         * CRS format
         */
        ae_assert(s->ninitialized==s->ridx.ptr.p_int[m], "SparseMM: some rows/elements of the CRS matrix were not initialized (you must initialize everything you promised to SparseCreateCRS)", _state);
        if( k<sparse_linalgswitch )
        {
            for(i=0; i<=m-1; i++)
            {
                for(j=0; j<=k-1; j++)
                {
                    tval = (double)(0);
                    lt = s->ridx.ptr.p_int[i];
                    rt = s->ridx.ptr.p_int[i+1];
                    for(k0=lt; k0<=rt-1; k0++)
                    {
                        tval = tval+s->vals.ptr.p_double[k0]*a->ptr.pp_double[s->idx.ptr.p_int[k0]][j];
                    }
                    b->ptr.pp_double[i][j] = tval;
                }
            }
        }
        else
        {
            for(i=0; i<=m-1; i++)
            {
                lt = s->ridx.ptr.p_int[i];
                rt = s->ridx.ptr.p_int[i+1];
                for(j=lt; j<=rt-1; j++)
                {
                    id = s->idx.ptr.p_int[j];
                    v = s->vals.ptr.p_double[j];
                    ae_v_addd(&b->ptr.pp_double[i][0], 1, &a->ptr.pp_double[id][0], 1, ae_v_len(0,k-1), v);
                }
            }
        }
        return;
    }
    if( s->matrixtype==2 )
    {
        
        /*
         * SKS format
         */
        ae_assert(m==n, "SparseMM: non-square SKS matrices are not supported", _state);
        for(i=0; i<=n-1; i++)
        {
            ri = s->ridx.ptr.p_int[i];
            ri1 = s->ridx.ptr.p_int[i+1];
            d = s->didx.ptr.p_int[i];
            u = s->uidx.ptr.p_int[i];
            if( d>0 )
            {
                lt = ri;
                lt1 = i-d;
                rt1 = i-1;
                for(j=lt1; j<=rt1; j++)
                {
                    v = s->vals.ptr.p_double[lt+(j-lt1)];
                    if( k<sparse_linalgswitch )
                    {
                        
                        /*
                         * Use loop
                         */
                        for(k0=0; k0<=k1; k0++)
                        {
                            b->ptr.pp_double[i][k0] = b->ptr.pp_double[i][k0]+v*a->ptr.pp_double[j][k0];
                        }
                    }
                    else
                    {
                        
                        /*
                         * Use vector operation
                         */
                        ae_v_addd(&b->ptr.pp_double[i][0], 1, &a->ptr.pp_double[j][0], 1, ae_v_len(0,k-1), v);
                    }
                }
            }
            if( u>0 )
            {
                lt = ri1-u;
                lt1 = i-u;
                rt1 = i-1;
                for(j=lt1; j<=rt1; j++)
                {
                    v = s->vals.ptr.p_double[lt+(j-lt1)];
                    if( k<sparse_linalgswitch )
                    {
                        
                        /*
                         * Use loop
                         */
                        for(k0=0; k0<=k1; k0++)
                        {
                            b->ptr.pp_double[j][k0] = b->ptr.pp_double[j][k0]+v*a->ptr.pp_double[i][k0];
                        }
                    }
                    else
                    {
                        
                        /*
                         * Use vector operation
                         */
                        ae_v_addd(&b->ptr.pp_double[j][0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,k-1), v);
                    }
                }
            }
            vd = s->vals.ptr.p_double[ri+d];
            ae_v_addd(&b->ptr.pp_double[i][0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,k-1), vd);
        }
        return;
    }
}


/*************************************************************************
This function calculates matrix-matrix product  S^T*A. Matrix S  must  be
stored in CRS or SKS format (exception will be thrown otherwise).

INPUT PARAMETERS
    S           -   sparse M*N matrix in CRS or SKS format.
    A           -   array[M][K], input dense matrix. For performance reasons
                    we make only quick checks - we check that array size  is
                    at least M, but we do not check for NAN's or INF's.
    K           -   number of columns of matrix (A).                    
    B           -   output buffer, possibly preallocated. In case  buffer
                    size is too small to store  result,  this  buffer  is
                    automatically resized.
    
OUTPUT PARAMETERS
    B           -   array[N][K], S^T*A
    
NOTE: this function throws exception when called for non-CRS/SKS  matrix.
You must convert your matrix with SparseConvertToCRS/SKS()  before  using
this function.

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
void sparsemtm(const sparsematrix* s,
     /* Real    */ const ae_matrix* a,
     ae_int_t k,
     /* Real    */ ae_matrix* b,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k0;
    ae_int_t k1;
    ae_int_t lt;
    ae_int_t rt;
    ae_int_t ct;
    double v;
    ae_int_t m;
    ae_int_t n;
    ae_int_t ri;
    ae_int_t ri1;
    ae_int_t lt1;
    ae_int_t rt1;
    ae_int_t d;
    ae_int_t u;


    ae_assert(s->matrixtype==1||s->matrixtype==2, "SparseMTM: incorrect matrix type (convert your matrix to CRS/SKS)", _state);
    ae_assert(a->rows>=s->m, "SparseMTM: Rows(A)<M", _state);
    ae_assert(k>0, "SparseMTM: K<=0", _state);
    m = s->m;
    n = s->n;
    k1 = k-1;
    rmatrixsetlengthatleast(b, n, k, _state);
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=k-1; j++)
        {
            b->ptr.pp_double[i][j] = (double)(0);
        }
    }
    if( s->matrixtype==1 )
    {
        
        /*
         * CRS format
         */
        ae_assert(s->ninitialized==s->ridx.ptr.p_int[m], "SparseMTM: some rows/elements of the CRS matrix were not initialized (you must initialize everything you promised to SparseCreateCRS)", _state);
        if( k<sparse_linalgswitch )
        {
            for(i=0; i<=m-1; i++)
            {
                lt = s->ridx.ptr.p_int[i];
                rt = s->ridx.ptr.p_int[i+1];
                for(k0=lt; k0<=rt-1; k0++)
                {
                    v = s->vals.ptr.p_double[k0];
                    ct = s->idx.ptr.p_int[k0];
                    for(j=0; j<=k-1; j++)
                    {
                        b->ptr.pp_double[ct][j] = b->ptr.pp_double[ct][j]+v*a->ptr.pp_double[i][j];
                    }
                }
            }
        }
        else
        {
            for(i=0; i<=m-1; i++)
            {
                lt = s->ridx.ptr.p_int[i];
                rt = s->ridx.ptr.p_int[i+1];
                for(j=lt; j<=rt-1; j++)
                {
                    v = s->vals.ptr.p_double[j];
                    ct = s->idx.ptr.p_int[j];
                    ae_v_addd(&b->ptr.pp_double[ct][0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,k-1), v);
                }
            }
        }
        return;
    }
    if( s->matrixtype==2 )
    {
        
        /*
         * SKS format
         */
        ae_assert(m==n, "SparseMTM: non-square SKS matrices are not supported", _state);
        for(i=0; i<=n-1; i++)
        {
            ri = s->ridx.ptr.p_int[i];
            ri1 = s->ridx.ptr.p_int[i+1];
            d = s->didx.ptr.p_int[i];
            u = s->uidx.ptr.p_int[i];
            if( d>0 )
            {
                lt = ri;
                lt1 = i-d;
                rt1 = i-1;
                for(j=lt1; j<=rt1; j++)
                {
                    v = s->vals.ptr.p_double[lt+(j-lt1)];
                    if( k<sparse_linalgswitch )
                    {
                        
                        /*
                         * Use loop
                         */
                        for(k0=0; k0<=k1; k0++)
                        {
                            b->ptr.pp_double[j][k0] = b->ptr.pp_double[j][k0]+v*a->ptr.pp_double[i][k0];
                        }
                    }
                    else
                    {
                        
                        /*
                         * Use vector operation
                         */
                        ae_v_addd(&b->ptr.pp_double[j][0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,k-1), v);
                    }
                }
            }
            if( u>0 )
            {
                lt = ri1-u;
                lt1 = i-u;
                rt1 = i-1;
                for(j=lt1; j<=rt1; j++)
                {
                    v = s->vals.ptr.p_double[lt+(j-lt1)];
                    if( k<sparse_linalgswitch )
                    {
                        
                        /*
                         * Use loop
                         */
                        for(k0=0; k0<=k1; k0++)
                        {
                            b->ptr.pp_double[i][k0] = b->ptr.pp_double[i][k0]+v*a->ptr.pp_double[j][k0];
                        }
                    }
                    else
                    {
                        
                        /*
                         * Use vector operation
                         */
                        ae_v_addd(&b->ptr.pp_double[i][0], 1, &a->ptr.pp_double[j][0], 1, ae_v_len(0,k-1), v);
                    }
                }
            }
            v = s->vals.ptr.p_double[ri+d];
            ae_v_addd(&b->ptr.pp_double[i][0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,k-1), v);
        }
        return;
    }
}


/*************************************************************************
This function simultaneously calculates two matrix-matrix products:
    S*A and S^T*A.
S  must  be  square (non-rectangular) matrix stored in CRS or  SKS  format
(exception will be thrown otherwise).

INPUT PARAMETERS
    S           -   sparse N*N matrix in CRS or SKS format.
    A           -   array[N][K], input dense matrix. For performance reasons
                    we make only quick checks - we check that array size  is
                    at least N, but we do not check for NAN's or INF's.
    K           -   number of columns of matrix (A).                    
    B0          -   output buffer, possibly preallocated. In case  buffer
                    size is too small to store  result,  this  buffer  is
                    automatically resized.
    B1          -   output buffer, possibly preallocated. In case  buffer
                    size is too small to store  result,  this  buffer  is
                    automatically resized.
    
OUTPUT PARAMETERS
    B0          -   array[N][K], S*A
    B1          -   array[N][K], S^T*A
    
NOTE: this function throws exception when called for non-CRS/SKS  matrix.
You must convert your matrix with SparseConvertToCRS/SKS()  before  using
this function.

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
void sparsemm2(const sparsematrix* s,
     /* Real    */ const ae_matrix* a,
     ae_int_t k,
     /* Real    */ ae_matrix* b0,
     /* Real    */ ae_matrix* b1,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k0;
    ae_int_t lt;
    ae_int_t rt;
    ae_int_t ct;
    double v;
    double tval;
    ae_int_t n;
    ae_int_t k1;
    ae_int_t ri;
    ae_int_t ri1;
    ae_int_t lt1;
    ae_int_t rt1;
    ae_int_t d;
    ae_int_t u;


    ae_assert(s->matrixtype==1||s->matrixtype==2, "SparseMM2: incorrect matrix type (convert your matrix to CRS/SKS)", _state);
    ae_assert(s->m==s->n, "SparseMM2: matrix is non-square", _state);
    ae_assert(a->rows>=s->n, "SparseMM2: Rows(A)<N", _state);
    ae_assert(k>0, "SparseMM2: K<=0", _state);
    n = s->n;
    k1 = k-1;
    rmatrixsetlengthatleast(b0, n, k, _state);
    rmatrixsetlengthatleast(b1, n, k, _state);
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=k-1; j++)
        {
            b1->ptr.pp_double[i][j] = (double)(0);
            b0->ptr.pp_double[i][j] = (double)(0);
        }
    }
    if( s->matrixtype==1 )
    {
        
        /*
         * CRS format
         */
        ae_assert(s->ninitialized==s->ridx.ptr.p_int[s->m], "SparseMM2: some rows/elements of the CRS matrix were not initialized (you must initialize everything you promised to SparseCreateCRS)", _state);
        if( k<sparse_linalgswitch )
        {
            for(i=0; i<=n-1; i++)
            {
                for(j=0; j<=k-1; j++)
                {
                    tval = (double)(0);
                    lt = s->ridx.ptr.p_int[i];
                    rt = s->ridx.ptr.p_int[i+1];
                    v = a->ptr.pp_double[i][j];
                    for(k0=lt; k0<=rt-1; k0++)
                    {
                        ct = s->idx.ptr.p_int[k0];
                        b1->ptr.pp_double[ct][j] = b1->ptr.pp_double[ct][j]+s->vals.ptr.p_double[k0]*v;
                        tval = tval+s->vals.ptr.p_double[k0]*a->ptr.pp_double[ct][j];
                    }
                    b0->ptr.pp_double[i][j] = tval;
                }
            }
        }
        else
        {
            for(i=0; i<=n-1; i++)
            {
                lt = s->ridx.ptr.p_int[i];
                rt = s->ridx.ptr.p_int[i+1];
                for(j=lt; j<=rt-1; j++)
                {
                    v = s->vals.ptr.p_double[j];
                    ct = s->idx.ptr.p_int[j];
                    ae_v_addd(&b0->ptr.pp_double[i][0], 1, &a->ptr.pp_double[ct][0], 1, ae_v_len(0,k-1), v);
                    ae_v_addd(&b1->ptr.pp_double[ct][0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,k-1), v);
                }
            }
        }
        return;
    }
    if( s->matrixtype==2 )
    {
        
        /*
         * SKS format
         */
        ae_assert(s->m==s->n, "SparseMM2: non-square SKS matrices are not supported", _state);
        for(i=0; i<=n-1; i++)
        {
            ri = s->ridx.ptr.p_int[i];
            ri1 = s->ridx.ptr.p_int[i+1];
            d = s->didx.ptr.p_int[i];
            u = s->uidx.ptr.p_int[i];
            if( d>0 )
            {
                lt = ri;
                lt1 = i-d;
                rt1 = i-1;
                for(j=lt1; j<=rt1; j++)
                {
                    v = s->vals.ptr.p_double[lt+(j-lt1)];
                    if( k<sparse_linalgswitch )
                    {
                        
                        /*
                         * Use loop
                         */
                        for(k0=0; k0<=k1; k0++)
                        {
                            b0->ptr.pp_double[i][k0] = b0->ptr.pp_double[i][k0]+v*a->ptr.pp_double[j][k0];
                            b1->ptr.pp_double[j][k0] = b1->ptr.pp_double[j][k0]+v*a->ptr.pp_double[i][k0];
                        }
                    }
                    else
                    {
                        
                        /*
                         * Use vector operation
                         */
                        ae_v_addd(&b0->ptr.pp_double[i][0], 1, &a->ptr.pp_double[j][0], 1, ae_v_len(0,k-1), v);
                        ae_v_addd(&b1->ptr.pp_double[j][0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,k-1), v);
                    }
                }
            }
            if( u>0 )
            {
                lt = ri1-u;
                lt1 = i-u;
                rt1 = i-1;
                for(j=lt1; j<=rt1; j++)
                {
                    v = s->vals.ptr.p_double[lt+(j-lt1)];
                    if( k<sparse_linalgswitch )
                    {
                        
                        /*
                         * Use loop
                         */
                        for(k0=0; k0<=k1; k0++)
                        {
                            b0->ptr.pp_double[j][k0] = b0->ptr.pp_double[j][k0]+v*a->ptr.pp_double[i][k0];
                            b1->ptr.pp_double[i][k0] = b1->ptr.pp_double[i][k0]+v*a->ptr.pp_double[j][k0];
                        }
                    }
                    else
                    {
                        
                        /*
                         * Use vector operation
                         */
                        ae_v_addd(&b0->ptr.pp_double[j][0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,k-1), v);
                        ae_v_addd(&b1->ptr.pp_double[i][0], 1, &a->ptr.pp_double[j][0], 1, ae_v_len(0,k-1), v);
                    }
                }
            }
            v = s->vals.ptr.p_double[ri+d];
            ae_v_addd(&b0->ptr.pp_double[i][0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,k-1), v);
            ae_v_addd(&b1->ptr.pp_double[i][0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,k-1), v);
        }
        return;
    }
}


/*************************************************************************
This function calculates matrix-matrix product  S*A, when S  is  symmetric
matrix. Matrix S must be stored in CRS or SKS format  (exception  will  be
thrown otherwise).

INPUT PARAMETERS
    S           -   sparse M*M matrix in CRS or SKS format.
    IsUpper     -   whether upper or lower triangle of S is given:
                    * if upper triangle is given,  only   S[i,j] for j>=i
                      are used, and lower triangle is ignored (it can  be
                      empty - these elements are not referenced at all).
                    * if lower triangle is given,  only   S[i,j] for j<=i
                      are used, and upper triangle is ignored.
    A           -   array[N][K], input dense matrix. For performance reasons
                    we make only quick checks - we check that array size is
                    at least N, but we do not check for NAN's or INF's.
    K           -   number of columns of matrix (A).  
    B           -   output buffer, possibly preallocated. In case  buffer
                    size is too small to store  result,  this  buffer  is
                    automatically resized.
    
OUTPUT PARAMETERS
    B           -   array[M][K], S*A
    
NOTE: this function throws exception when called for non-CRS/SKS  matrix.
You must convert your matrix with SparseConvertToCRS/SKS()  before  using
this function.

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
void sparsesmm(const sparsematrix* s,
     ae_bool isupper,
     /* Real    */ const ae_matrix* a,
     ae_int_t k,
     /* Real    */ ae_matrix* b,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k0;
    ae_int_t id;
    ae_int_t k1;
    ae_int_t lt;
    ae_int_t rt;
    double v;
    double vb;
    double va;
    ae_int_t n;
    ae_int_t ri;
    ae_int_t ri1;
    ae_int_t lt1;
    ae_int_t rt1;
    ae_int_t d;
    ae_int_t u;


    ae_assert(s->matrixtype==1||s->matrixtype==2, "SparseSMM: incorrect matrix type (convert your matrix to CRS/SKS)", _state);
    ae_assert(a->rows>=s->n, "SparseSMM: Rows(X)<N", _state);
    ae_assert(s->m==s->n, "SparseSMM: matrix is non-square", _state);
    n = s->n;
    k1 = k-1;
    rmatrixsetlengthatleast(b, n, k, _state);
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=k-1; j++)
        {
            b->ptr.pp_double[i][j] = (double)(0);
        }
    }
    if( s->matrixtype==1 )
    {
        
        /*
         * CRS format
         */
        ae_assert(s->ninitialized==s->ridx.ptr.p_int[s->m], "SparseSMM: some rows/elements of the CRS matrix were not initialized (you must initialize everything you promised to SparseCreateCRS)", _state);
        if( k>sparse_linalgswitch )
        {
            for(i=0; i<=n-1; i++)
            {
                for(j=0; j<=k-1; j++)
                {
                    if( s->didx.ptr.p_int[i]!=s->uidx.ptr.p_int[i] )
                    {
                        id = s->didx.ptr.p_int[i];
                        b->ptr.pp_double[i][j] = b->ptr.pp_double[i][j]+s->vals.ptr.p_double[id]*a->ptr.pp_double[s->idx.ptr.p_int[id]][j];
                    }
                    if( isupper )
                    {
                        lt = s->uidx.ptr.p_int[i];
                        rt = s->ridx.ptr.p_int[i+1];
                        vb = (double)(0);
                        va = a->ptr.pp_double[i][j];
                        for(k0=lt; k0<=rt-1; k0++)
                        {
                            id = s->idx.ptr.p_int[k0];
                            v = s->vals.ptr.p_double[k0];
                            vb = vb+a->ptr.pp_double[id][j]*v;
                            b->ptr.pp_double[id][j] = b->ptr.pp_double[id][j]+va*v;
                        }
                        b->ptr.pp_double[i][j] = b->ptr.pp_double[i][j]+vb;
                    }
                    else
                    {
                        lt = s->ridx.ptr.p_int[i];
                        rt = s->didx.ptr.p_int[i];
                        vb = (double)(0);
                        va = a->ptr.pp_double[i][j];
                        for(k0=lt; k0<=rt-1; k0++)
                        {
                            id = s->idx.ptr.p_int[k0];
                            v = s->vals.ptr.p_double[k0];
                            vb = vb+a->ptr.pp_double[id][j]*v;
                            b->ptr.pp_double[id][j] = b->ptr.pp_double[id][j]+va*v;
                        }
                        b->ptr.pp_double[i][j] = b->ptr.pp_double[i][j]+vb;
                    }
                }
            }
        }
        else
        {
            for(i=0; i<=n-1; i++)
            {
                if( s->didx.ptr.p_int[i]!=s->uidx.ptr.p_int[i] )
                {
                    id = s->didx.ptr.p_int[i];
                    v = s->vals.ptr.p_double[id];
                    ae_v_addd(&b->ptr.pp_double[i][0], 1, &a->ptr.pp_double[s->idx.ptr.p_int[id]][0], 1, ae_v_len(0,k-1), v);
                }
                if( isupper )
                {
                    lt = s->uidx.ptr.p_int[i];
                    rt = s->ridx.ptr.p_int[i+1];
                    for(j=lt; j<=rt-1; j++)
                    {
                        id = s->idx.ptr.p_int[j];
                        v = s->vals.ptr.p_double[j];
                        ae_v_addd(&b->ptr.pp_double[i][0], 1, &a->ptr.pp_double[id][0], 1, ae_v_len(0,k-1), v);
                        ae_v_addd(&b->ptr.pp_double[id][0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,k-1), v);
                    }
                }
                else
                {
                    lt = s->ridx.ptr.p_int[i];
                    rt = s->didx.ptr.p_int[i];
                    for(j=lt; j<=rt-1; j++)
                    {
                        id = s->idx.ptr.p_int[j];
                        v = s->vals.ptr.p_double[j];
                        ae_v_addd(&b->ptr.pp_double[i][0], 1, &a->ptr.pp_double[id][0], 1, ae_v_len(0,k-1), v);
                        ae_v_addd(&b->ptr.pp_double[id][0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,k-1), v);
                    }
                }
            }
        }
        return;
    }
    if( s->matrixtype==2 )
    {
        
        /*
         * SKS format
         */
        ae_assert(s->m==s->n, "SparseMM2: non-square SKS matrices are not supported", _state);
        for(i=0; i<=n-1; i++)
        {
            ri = s->ridx.ptr.p_int[i];
            ri1 = s->ridx.ptr.p_int[i+1];
            d = s->didx.ptr.p_int[i];
            u = s->uidx.ptr.p_int[i];
            if( d>0&&!isupper )
            {
                lt = ri;
                lt1 = i-d;
                rt1 = i-1;
                for(j=lt1; j<=rt1; j++)
                {
                    v = s->vals.ptr.p_double[lt+(j-lt1)];
                    if( k<sparse_linalgswitch )
                    {
                        
                        /*
                         * Use loop
                         */
                        for(k0=0; k0<=k1; k0++)
                        {
                            b->ptr.pp_double[i][k0] = b->ptr.pp_double[i][k0]+v*a->ptr.pp_double[j][k0];
                            b->ptr.pp_double[j][k0] = b->ptr.pp_double[j][k0]+v*a->ptr.pp_double[i][k0];
                        }
                    }
                    else
                    {
                        
                        /*
                         * Use vector operation
                         */
                        ae_v_addd(&b->ptr.pp_double[i][0], 1, &a->ptr.pp_double[j][0], 1, ae_v_len(0,k-1), v);
                        ae_v_addd(&b->ptr.pp_double[j][0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,k-1), v);
                    }
                }
            }
            if( u>0&&isupper )
            {
                lt = ri1-u;
                lt1 = i-u;
                rt1 = i-1;
                for(j=lt1; j<=rt1; j++)
                {
                    v = s->vals.ptr.p_double[lt+(j-lt1)];
                    if( k<sparse_linalgswitch )
                    {
                        
                        /*
                         * Use loop
                         */
                        for(k0=0; k0<=k1; k0++)
                        {
                            b->ptr.pp_double[j][k0] = b->ptr.pp_double[j][k0]+v*a->ptr.pp_double[i][k0];
                            b->ptr.pp_double[i][k0] = b->ptr.pp_double[i][k0]+v*a->ptr.pp_double[j][k0];
                        }
                    }
                    else
                    {
                        
                        /*
                         * Use vector operation
                         */
                        ae_v_addd(&b->ptr.pp_double[j][0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,k-1), v);
                        ae_v_addd(&b->ptr.pp_double[i][0], 1, &a->ptr.pp_double[j][0], 1, ae_v_len(0,k-1), v);
                    }
                }
            }
            v = s->vals.ptr.p_double[ri+d];
            ae_v_addd(&b->ptr.pp_double[i][0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,k-1), v);
        }
        return;
    }
}


/*************************************************************************
This function calculates matrix-vector product op(S)*x, when x is  vector,
S is symmetric triangular matrix, op(S) is transposition or no  operation.
Matrix S must be stored in CRS or SKS format  (exception  will  be  thrown
otherwise).

INPUT PARAMETERS
    S           -   sparse square matrix in CRS or SKS format.
    IsUpper     -   whether upper or lower triangle of S is used:
                    * if upper triangle is given,  only   S[i,j] for  j>=i
                      are used, and lower triangle is  ignored (it can  be
                      empty - these elements are not referenced at all).
                    * if lower triangle is given,  only   S[i,j] for  j<=i
                      are used, and upper triangle is ignored.
    IsUnit      -   unit or non-unit diagonal:
                    * if True, diagonal elements of triangular matrix  are
                      considered equal to 1.0. Actual elements  stored  in
                      S are not referenced at all.
                    * if False, diagonal stored in S is used
    OpType      -   operation type:
                    * if 0, S*x is calculated
                    * if 1, (S^T)*x is calculated (transposition)
    X           -   array[N] which stores input  vector.  For  performance
                    reasons we make only quick  checks  -  we  check  that
                    array  size  is  at  least  N, but we do not check for
                    NAN's or INF's.
    Y           -   possibly  preallocated  input   buffer.  Automatically 
                    resized if its size is too small.
    
OUTPUT PARAMETERS
    Y           -   array[N], op(S)*x
    
NOTE: this function throws exception when called for non-CRS/SKS  matrix.
You must convert your matrix with SparseConvertToCRS/SKS()  before  using
this function.

  -- ALGLIB PROJECT --
     Copyright 20.01.2014 by Bochkanov Sergey
*************************************************************************/
void sparsetrmv(const sparsematrix* s,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t j0;
    ae_int_t j1;
    double v;
    ae_int_t ri;
    ae_int_t ri1;
    ae_int_t d;
    ae_int_t u;
    ae_int_t lt;
    ae_int_t rt;
    ae_int_t lt1;
    ae_int_t rt1;


    ae_assert(s->matrixtype==1||s->matrixtype==2, "SparseTRMV: incorrect matrix type (convert your matrix to CRS/SKS)", _state);
    ae_assert(optype==0||optype==1, "SparseTRMV: incorrect operation type (must be 0 or 1)", _state);
    ae_assert(x->cnt>=s->n, "SparseTRMV: Length(X)<N", _state);
    ae_assert(s->m==s->n, "SparseTRMV: matrix is non-square", _state);
    n = s->n;
    rvectorsetlengthatleast(y, n, _state);
    if( isunit )
    {
        
        /*
         * Set initial value of y to x
         */
        for(i=0; i<=n-1; i++)
        {
            y->ptr.p_double[i] = x->ptr.p_double[i];
        }
    }
    else
    {
        
        /*
         * Set initial value of y to 0
         */
        for(i=0; i<=n-1; i++)
        {
            y->ptr.p_double[i] = (double)(0);
        }
    }
    if( s->matrixtype==1 )
    {
        
        /*
         * CRS format
         */
        ae_assert(s->ninitialized==s->ridx.ptr.p_int[s->m], "SparseTRMV: some rows/elements of the CRS matrix were not initialized (you must initialize everything you promised to SparseCreateCRS)", _state);
        for(i=0; i<=n-1; i++)
        {
            
            /*
             * Depending on IsUpper/IsUnit, select range of indexes to process
             */
            if( isupper )
            {
                if( isunit||s->didx.ptr.p_int[i]==s->uidx.ptr.p_int[i] )
                {
                    j0 = s->uidx.ptr.p_int[i];
                }
                else
                {
                    j0 = s->didx.ptr.p_int[i];
                }
                j1 = s->ridx.ptr.p_int[i+1]-1;
            }
            else
            {
                j0 = s->ridx.ptr.p_int[i];
                if( isunit||s->didx.ptr.p_int[i]==s->uidx.ptr.p_int[i] )
                {
                    j1 = s->didx.ptr.p_int[i]-1;
                }
                else
                {
                    j1 = s->didx.ptr.p_int[i];
                }
            }
            
            /*
             * Depending on OpType, process subset of I-th row of input matrix
             */
            if( optype==0 )
            {
                v = 0.0;
                for(j=j0; j<=j1; j++)
                {
                    v = v+s->vals.ptr.p_double[j]*x->ptr.p_double[s->idx.ptr.p_int[j]];
                }
                y->ptr.p_double[i] = y->ptr.p_double[i]+v;
            }
            else
            {
                v = x->ptr.p_double[i];
                for(j=j0; j<=j1; j++)
                {
                    k = s->idx.ptr.p_int[j];
                    y->ptr.p_double[k] = y->ptr.p_double[k]+v*s->vals.ptr.p_double[j];
                }
            }
        }
        return;
    }
    if( s->matrixtype==2 )
    {
        
        /*
         * SKS format
         */
        ae_assert(s->m==s->n, "SparseTRMV: non-square SKS matrices are not supported", _state);
        for(i=0; i<=n-1; i++)
        {
            ri = s->ridx.ptr.p_int[i];
            ri1 = s->ridx.ptr.p_int[i+1];
            d = s->didx.ptr.p_int[i];
            u = s->uidx.ptr.p_int[i];
            if( !isunit )
            {
                y->ptr.p_double[i] = y->ptr.p_double[i]+s->vals.ptr.p_double[ri+d]*x->ptr.p_double[i];
            }
            if( d>0&&!isupper )
            {
                lt = ri;
                rt = ri+d-1;
                lt1 = i-d;
                rt1 = i-1;
                if( optype==0 )
                {
                    v = ae_v_dotproduct(&s->vals.ptr.p_double[lt], 1, &x->ptr.p_double[lt1], 1, ae_v_len(lt,rt));
                    y->ptr.p_double[i] = y->ptr.p_double[i]+v;
                }
                else
                {
                    v = x->ptr.p_double[i];
                    ae_v_addd(&y->ptr.p_double[lt1], 1, &s->vals.ptr.p_double[lt], 1, ae_v_len(lt1,rt1), v);
                }
            }
            if( u>0&&isupper )
            {
                lt = ri1-u;
                rt = ri1-1;
                lt1 = i-u;
                rt1 = i-1;
                if( optype==0 )
                {
                    v = x->ptr.p_double[i];
                    ae_v_addd(&y->ptr.p_double[lt1], 1, &s->vals.ptr.p_double[lt], 1, ae_v_len(lt1,rt1), v);
                }
                else
                {
                    v = ae_v_dotproduct(&s->vals.ptr.p_double[lt], 1, &x->ptr.p_double[lt1], 1, ae_v_len(lt,rt));
                    y->ptr.p_double[i] = y->ptr.p_double[i]+v;
                }
            }
        }
        return;
    }
}


/*************************************************************************
This function solves linear system op(S)*y=x  where  x  is  vector,  S  is
symmetric  triangular  matrix,  op(S)  is  transposition  or no operation.
Matrix S must be stored in CRS or SKS format  (exception  will  be  thrown
otherwise).

INPUT PARAMETERS
    S           -   sparse square matrix in CRS or SKS format.
    IsUpper     -   whether upper or lower triangle of S is used:
                    * if upper triangle is given,  only   S[i,j] for  j>=i
                      are used, and lower triangle is  ignored (it can  be
                      empty - these elements are not referenced at all).
                    * if lower triangle is given,  only   S[i,j] for  j<=i
                      are used, and upper triangle is ignored.
    IsUnit      -   unit or non-unit diagonal:
                    * if True, diagonal elements of triangular matrix  are
                      considered equal to 1.0. Actual elements  stored  in
                      S are not referenced at all.
                    * if False, diagonal stored in S is used. It  is  your
                      responsibility  to  make  sure  that   diagonal   is
                      non-zero.
    OpType      -   operation type:
                    * if 0, S*x is calculated
                    * if 1, (S^T)*x is calculated (transposition)
    X           -   array[N] which stores input  vector.  For  performance
                    reasons we make only quick  checks  -  we  check  that
                    array  size  is  at  least  N, but we do not check for
                    NAN's or INF's.
    
OUTPUT PARAMETERS
    X           -   array[N], inv(op(S))*x
    
NOTE: this function throws exception when called for  non-CRS/SKS  matrix.
      You must convert your matrix  with  SparseConvertToCRS/SKS()  before
      using this function.

NOTE: no assertion or tests are done during algorithm  operation.   It  is
      your responsibility to provide invertible matrix to algorithm.

  -- ALGLIB PROJECT --
     Copyright 20.01.2014 by Bochkanov Sergey
*************************************************************************/
void sparsetrsv(const sparsematrix* s,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t fst;
    ae_int_t lst;
    ae_int_t stp;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double v;
    double vd;
    double v0;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t ri;
    ae_int_t ri1;
    ae_int_t d;
    ae_int_t u;
    ae_int_t lt;
    ae_int_t lt1;


    ae_assert(s->matrixtype==1||s->matrixtype==2, "SparseTRSV: incorrect matrix type (convert your matrix to CRS/SKS)", _state);
    ae_assert(optype==0||optype==1, "SparseTRSV: incorrect operation type (must be 0 or 1)", _state);
    ae_assert(x->cnt>=s->n, "SparseTRSV: Length(X)<N", _state);
    ae_assert(s->m==s->n, "SparseTRSV: matrix is non-square", _state);
    n = s->n;
    if( s->matrixtype==1 )
    {
        
        /*
         * CRS format.
         *
         * Several branches for different combinations of IsUpper and OpType
         */
        ae_assert(s->ninitialized==s->ridx.ptr.p_int[s->m], "SparseTRSV: some rows/elements of the CRS matrix were not initialized (you must initialize everything you promised to SparseCreateCRS)", _state);
        if( optype==0 )
        {
            
            /*
             * No transposition.
             *
             * S*x=y with upper or lower triangular S.
             */
            v0 = (double)(0);
            if( isupper )
            {
                fst = n-1;
                lst = 0;
                stp = -1;
            }
            else
            {
                fst = 0;
                lst = n-1;
                stp = 1;
            }
            i = fst;
            while((stp>0&&i<=lst)||(stp<0&&i>=lst))
            {
                
                /*
                 * Select range of indexes to process
                 */
                if( isupper )
                {
                    j0 = s->uidx.ptr.p_int[i];
                    j1 = s->ridx.ptr.p_int[i+1]-1;
                }
                else
                {
                    j0 = s->ridx.ptr.p_int[i];
                    j1 = s->didx.ptr.p_int[i]-1;
                }
                
                /*
                 * Calculate X[I]
                 */
                v = 0.0;
                for(j=j0; j<=j1; j++)
                {
                    v = v+s->vals.ptr.p_double[j]*x->ptr.p_double[s->idx.ptr.p_int[j]];
                }
                if( !isunit )
                {
                    if( s->didx.ptr.p_int[i]==s->uidx.ptr.p_int[i] )
                    {
                        vd = (double)(0);
                    }
                    else
                    {
                        vd = s->vals.ptr.p_double[s->didx.ptr.p_int[i]];
                    }
                }
                else
                {
                    vd = 1.0;
                }
                v = (x->ptr.p_double[i]-v)/vd;
                x->ptr.p_double[i] = v;
                v0 = 0.25*v0+v;
                
                /*
                 * Next I
                 */
                i = i+stp;
            }
            ae_assert(ae_isfinite(v0, _state), "SparseTRSV: overflow or division by exact zero", _state);
            return;
        }
        if( optype==1 )
        {
            
            /*
             * Transposition.
             *
             * (S^T)*x=y with upper or lower triangular S.
             */
            if( isupper )
            {
                fst = 0;
                lst = n-1;
                stp = 1;
            }
            else
            {
                fst = n-1;
                lst = 0;
                stp = -1;
            }
            i = fst;
            v0 = (double)(0);
            while((stp>0&&i<=lst)||(stp<0&&i>=lst))
            {
                v = x->ptr.p_double[i];
                if( v!=0.0 )
                {
                    
                    /*
                     * X[i] already stores A[i,i]*Y[i], the only thing left
                     * is to divide by diagonal element.
                     */
                    if( !isunit )
                    {
                        if( s->didx.ptr.p_int[i]==s->uidx.ptr.p_int[i] )
                        {
                            vd = (double)(0);
                        }
                        else
                        {
                            vd = s->vals.ptr.p_double[s->didx.ptr.p_int[i]];
                        }
                    }
                    else
                    {
                        vd = 1.0;
                    }
                    v = v/vd;
                    x->ptr.p_double[i] = v;
                    v0 = 0.25*v0+v;
                    
                    /*
                     * For upper triangular case:
                     *     subtract X[i]*Ai from X[i+1:N-1]
                     *
                     * For lower triangular case:
                     *     subtract X[i]*Ai from X[0:i-1]
                     *
                     * (here Ai is I-th row of original, untransposed A).
                     */
                    if( isupper )
                    {
                        j0 = s->uidx.ptr.p_int[i];
                        j1 = s->ridx.ptr.p_int[i+1]-1;
                    }
                    else
                    {
                        j0 = s->ridx.ptr.p_int[i];
                        j1 = s->didx.ptr.p_int[i]-1;
                    }
                    for(j=j0; j<=j1; j++)
                    {
                        k = s->idx.ptr.p_int[j];
                        x->ptr.p_double[k] = x->ptr.p_double[k]-s->vals.ptr.p_double[j]*v;
                    }
                }
                
                /*
                 * Next I
                 */
                i = i+stp;
            }
            ae_assert(ae_isfinite(v0, _state), "SparseTRSV: overflow or division by exact zero", _state);
            return;
        }
        ae_assert(ae_false, "SparseTRSV: internal error", _state);
    }
    if( s->matrixtype==2 )
    {
        
        /*
         * SKS format
         */
        ae_assert(s->m==s->n, "SparseTRSV: non-square SKS matrices are not supported", _state);
        if( (optype==0&&!isupper)||(optype==1&&isupper) )
        {
            
            /*
             * Lower triangular op(S) (matrix itself can be upper triangular).
             */
            v0 = (double)(0);
            for(i=0; i<=n-1; i++)
            {
                
                /*
                 * Select range of indexes to process
                 */
                ri = s->ridx.ptr.p_int[i];
                ri1 = s->ridx.ptr.p_int[i+1];
                d = s->didx.ptr.p_int[i];
                u = s->uidx.ptr.p_int[i];
                if( isupper )
                {
                    lt = i-u;
                    lt1 = ri1-u;
                    k = u-1;
                }
                else
                {
                    lt = i-d;
                    lt1 = ri;
                    k = d-1;
                }
                
                /*
                 * Calculate X[I]
                 */
                v = 0.0;
                for(j=0; j<=k; j++)
                {
                    v = v+s->vals.ptr.p_double[lt1+j]*x->ptr.p_double[lt+j];
                }
                if( isunit )
                {
                    vd = (double)(1);
                }
                else
                {
                    vd = s->vals.ptr.p_double[ri+d];
                }
                v = (x->ptr.p_double[i]-v)/vd;
                x->ptr.p_double[i] = v;
                v0 = 0.25*v0+v;
            }
            ae_assert(ae_isfinite(v0, _state), "SparseTRSV: overflow or division by exact zero", _state);
            return;
        }
        if( (optype==1&&!isupper)||(optype==0&&isupper) )
        {
            
            /*
             * Upper triangular op(S) (matrix itself can be lower triangular).
             */
            v0 = (double)(0);
            for(i=n-1; i>=0; i--)
            {
                ri = s->ridx.ptr.p_int[i];
                ri1 = s->ridx.ptr.p_int[i+1];
                d = s->didx.ptr.p_int[i];
                u = s->uidx.ptr.p_int[i];
                
                /*
                 * X[i] already stores A[i,i]*Y[i], the only thing left
                 * is to divide by diagonal element.
                 */
                if( isunit )
                {
                    vd = (double)(1);
                }
                else
                {
                    vd = s->vals.ptr.p_double[ri+d];
                }
                v = x->ptr.p_double[i]/vd;
                x->ptr.p_double[i] = v;
                v0 = 0.25*v0+v;
                
                /*
                 * Subtract product of X[i] and I-th column of "effective" A from
                 * unprocessed variables.
                 */
                v = x->ptr.p_double[i];
                if( isupper )
                {
                    lt = i-u;
                    lt1 = ri1-u;
                    k = u-1;
                }
                else
                {
                    lt = i-d;
                    lt1 = ri;
                    k = d-1;
                }
                for(j=0; j<=k; j++)
                {
                    x->ptr.p_double[lt+j] = x->ptr.p_double[lt+j]-v*s->vals.ptr.p_double[lt1+j];
                }
            }
            ae_assert(ae_isfinite(v0, _state), "SparseTRSV: overflow or division by exact zero", _state);
            return;
        }
        ae_assert(ae_false, "SparseTRSV: internal error", _state);
    }
    ae_assert(ae_false, "SparseTRSV: internal error", _state);
}


/*************************************************************************
This function applies permutation given by permutation table P (as opposed
to product form of permutation) to sparse symmetric  matrix  A,  given  by
either upper or lower triangle: B := P*A*P'.

This function allocates completely new instance of B. Use buffered version
SparseSymmPermTblBuf() if you want to reuse already allocated structure.

INPUT PARAMETERS
    A           -   sparse square matrix in CRS format.
    IsUpper     -   whether upper or lower triangle of A is used:
                    * if upper triangle is given,  only   A[i,j] for  j>=i
                      are used, and lower triangle is  ignored (it can  be
                      empty - these elements are not referenced at all).
                    * if lower triangle is given,  only   A[i,j] for  j<=i
                      are used, and upper triangle is ignored.
    P           -   array[N] which stores permutation table;  P[I]=J means
                    that I-th row/column of matrix  A  is  moved  to  J-th
                    position. For performance reasons we do NOT check that
                    P[] is  a   correct   permutation  (that there  is  no
                    repetitions, just that all its elements  are  in [0,N)
                    range.
    
OUTPUT PARAMETERS
    B           -   permuted matrix.  Permutation  is  applied  to A  from
                    the both sides, only upper or lower triangle (depending
                    on IsUpper) is stored.
    
NOTE: this function throws exception when called for non-CRS  matrix.  You
      must convert your matrix with SparseConvertToCRS() before using this
      function.

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
void sparsesymmpermtbl(const sparsematrix* a,
     ae_bool isupper,
     /* Integer */ const ae_vector* p,
     sparsematrix* b,
     ae_state *_state)
{

    _sparsematrix_clear(b);

    sparsesymmpermtblbuf(a, isupper, p, b, _state);
}


/*************************************************************************
This function is a buffered version  of  SparseSymmPermTbl()  that  reuses
previously allocated storage in B as much as possible.

This function applies permutation given by permutation table P (as opposed
to product form of permutation) to sparse symmetric  matrix  A,  given  by
either upper or lower triangle: B := P*A*P'.

INPUT PARAMETERS
    A           -   sparse square matrix in CRS format.
    IsUpper     -   whether upper or lower triangle of A is used:
                    * if upper triangle is given,  only   A[i,j] for  j>=i
                      are used, and lower triangle is  ignored (it can  be
                      empty - these elements are not referenced at all).
                    * if lower triangle is given,  only   A[i,j] for  j<=i
                      are used, and upper triangle is ignored.
    P           -   array[N] which stores permutation table;  P[I]=J means
                    that I-th row/column of matrix  A  is  moved  to  J-th
                    position. For performance reasons we do NOT check that
                    P[] is  a   correct   permutation  (that there  is  no
                    repetitions, just that all its elements  are  in [0,N)
                    range.
    B           -   sparse matrix object that will hold output.
                    Previously allocated memory will be reused as much  as
                    possible.
    
OUTPUT PARAMETERS
    B           -   permuted matrix.  Permutation  is  applied  to A  from
                    the both sides, only upper or lower triangle (depending
                    on IsUpper) is stored.
    
NOTE: this function throws exception when called for non-CRS  matrix.  You
      must convert your matrix with SparseConvertToCRS() before using this
      function.

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
void sparsesymmpermtblbuf(const sparsematrix* a,
     ae_bool isupper,
     /* Integer */ const ae_vector* p,
     sparsematrix* b,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t jj;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t k0;
    ae_int_t k1;
    ae_int_t kk;
    ae_int_t n;
    ae_int_t dst;
    ae_bool bflag;


    ae_assert(a->matrixtype==1, "SparseSymmPermTblBuf: incorrect matrix type (convert your matrix to CRS)", _state);
    ae_assert(p->cnt>=a->n, "SparseSymmPermTblBuf: Length(P)<N", _state);
    ae_assert(a->m==a->n, "SparseSymmPermTblBuf: matrix is non-square", _state);
    bflag = ae_true;
    for(i=0; i<=a->n-1; i++)
    {
        bflag = (bflag&&p->ptr.p_int[i]>=0)&&p->ptr.p_int[i]<a->n;
    }
    ae_assert(bflag, "SparseSymmPermTblBuf: P[] contains values outside of [0,N) range", _state);
    n = a->n;
    
    /*
     * Prepare output
     */
    ae_assert(a->ninitialized==a->ridx.ptr.p_int[n], "SparseSymmPermTblBuf: integrity check failed", _state);
    b->matrixtype = 1;
    b->n = n;
    b->m = n;
    ivectorsetlengthatleast(&b->didx, n, _state);
    ivectorsetlengthatleast(&b->uidx, n, _state);
    
    /*
     * Determine row sizes (temporary stored in DIdx) and ranges
     */
    isetv(n, 0, &b->didx, _state);
    for(i=0; i<=n-1; i++)
    {
        if( isupper )
        {
            j0 = a->didx.ptr.p_int[i];
            j1 = a->ridx.ptr.p_int[i+1]-1;
            k0 = p->ptr.p_int[i];
            for(jj=j0; jj<=j1; jj++)
            {
                k1 = p->ptr.p_int[a->idx.ptr.p_int[jj]];
                if( k1<k0 )
                {
                    b->didx.ptr.p_int[k1] = b->didx.ptr.p_int[k1]+1;
                }
                else
                {
                    b->didx.ptr.p_int[k0] = b->didx.ptr.p_int[k0]+1;
                }
            }
        }
        else
        {
            j0 = a->ridx.ptr.p_int[i];
            j1 = a->uidx.ptr.p_int[i]-1;
            k0 = p->ptr.p_int[i];
            for(jj=j0; jj<=j1; jj++)
            {
                k1 = p->ptr.p_int[a->idx.ptr.p_int[jj]];
                if( k1>k0 )
                {
                    b->didx.ptr.p_int[k1] = b->didx.ptr.p_int[k1]+1;
                }
                else
                {
                    b->didx.ptr.p_int[k0] = b->didx.ptr.p_int[k0]+1;
                }
            }
        }
    }
    ivectorsetlengthatleast(&b->ridx, n+1, _state);
    b->ridx.ptr.p_int[0] = 0;
    for(i=0; i<=n-1; i++)
    {
        b->ridx.ptr.p_int[i+1] = b->ridx.ptr.p_int[i]+b->didx.ptr.p_int[i];
    }
    b->ninitialized = b->ridx.ptr.p_int[n];
    ivectorsetlengthatleast(&b->idx, b->ninitialized, _state);
    rvectorsetlengthatleast(&b->vals, b->ninitialized, _state);
    
    /*
     * Process matrix
     */
    for(i=0; i<=n-1; i++)
    {
        b->uidx.ptr.p_int[i] = b->ridx.ptr.p_int[i];
    }
    for(i=0; i<=n-1; i++)
    {
        if( isupper )
        {
            j0 = a->didx.ptr.p_int[i];
            j1 = a->ridx.ptr.p_int[i+1]-1;
            for(jj=j0; jj<=j1; jj++)
            {
                j = a->idx.ptr.p_int[jj];
                k0 = p->ptr.p_int[i];
                k1 = p->ptr.p_int[j];
                if( k1<k0 )
                {
                    kk = k0;
                    k0 = k1;
                    k1 = kk;
                }
                dst = b->uidx.ptr.p_int[k0];
                b->idx.ptr.p_int[dst] = k1;
                b->vals.ptr.p_double[dst] = a->vals.ptr.p_double[jj];
                b->uidx.ptr.p_int[k0] = dst+1;
            }
        }
        else
        {
            j0 = a->ridx.ptr.p_int[i];
            j1 = a->uidx.ptr.p_int[i]-1;
            for(jj=j0; jj<=j1; jj++)
            {
                j = a->idx.ptr.p_int[jj];
                k0 = p->ptr.p_int[i];
                k1 = p->ptr.p_int[j];
                if( k1>k0 )
                {
                    kk = k0;
                    k0 = k1;
                    k1 = kk;
                }
                dst = b->uidx.ptr.p_int[k0];
                b->idx.ptr.p_int[dst] = k1;
                b->vals.ptr.p_double[dst] = a->vals.ptr.p_double[jj];
                b->uidx.ptr.p_int[k0] = dst+1;
            }
        }
    }
    
    /*
     * Finalize matrix
     */
    for(i=0; i<=n-1; i++)
    {
        tagsortmiddleir(&b->idx, &b->vals, b->ridx.ptr.p_int[i], b->ridx.ptr.p_int[i+1]-b->ridx.ptr.p_int[i], _state);
    }
    sparseinitduidx(b, _state);
}


/*************************************************************************
This procedure resizes Hash-Table matrix. It can be called when you  have
deleted too many elements from the matrix, and you want to  free unneeded
memory.

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
void sparseresizematrix(sparsematrix* s, ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t k;
    ae_int_t k1;
    ae_int_t i;
    ae_vector tvals;
    ae_vector tidx;

    ae_frame_make(_state, &_frame_block);
    memset(&tvals, 0, sizeof(tvals));
    memset(&tidx, 0, sizeof(tidx));
    ae_vector_init(&tvals, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tidx, 0, DT_INT, _state, ae_true);

    ae_assert(s->matrixtype==0, "SparseResizeMatrix: incorrect matrix type", _state);
    
    /*
     * Initialization for length and number of non-null elementd
     */
    k = s->tablesize;
    k1 = 0;
    
    /*
     * Calculating number of non-null elements
     */
    for(i=0; i<=k-1; i++)
    {
        if( s->idx.ptr.p_int[2*i]>=0 )
        {
            k1 = k1+1;
        }
    }
    
    /*
     * Initialization value for free space
     */
    s->tablesize = ae_round((double)k1/sparse_desiredloadfactor*sparse_growfactor+(double)sparse_additional, _state);
    s->nfree = s->tablesize-k1;
    ae_vector_set_length(&tvals, s->tablesize, _state);
    ae_vector_set_length(&tidx, 2*s->tablesize, _state);
    ae_swap_vectors(&s->vals, &tvals);
    ae_swap_vectors(&s->idx, &tidx);
    for(i=0; i<=s->tablesize-1; i++)
    {
        s->idx.ptr.p_int[2*i] = -1;
    }
    for(i=0; i<=k-1; i++)
    {
        if( tidx.ptr.p_int[2*i]>=0 )
        {
            sparseset(s, tidx.ptr.p_int[2*i], tidx.ptr.p_int[2*i+1], tvals.ptr.p_double[i], _state);
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Procedure for initialization 'S.DIdx' and 'S.UIdx'


  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
void sparseinitduidx(sparsematrix* s, ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t lt;
    ae_int_t rt;


    ae_assert(s->matrixtype==1, "SparseInitDUIdx: internal error, incorrect matrix type", _state);
    ivectorsetlengthatleast(&s->didx, s->m, _state);
    ivectorsetlengthatleast(&s->uidx, s->m, _state);
    for(i=0; i<=s->m-1; i++)
    {
        s->uidx.ptr.p_int[i] = -1;
        s->didx.ptr.p_int[i] = -1;
        lt = s->ridx.ptr.p_int[i];
        rt = s->ridx.ptr.p_int[i+1];
        for(j=lt; j<=rt-1; j++)
        {
            k = s->idx.ptr.p_int[j];
            if( k==i )
            {
                s->didx.ptr.p_int[i] = j;
            }
            else
            {
                if( k>i&&s->uidx.ptr.p_int[i]==-1 )
                {
                    s->uidx.ptr.p_int[i] = j;
                    break;
                }
            }
        }
        if( s->uidx.ptr.p_int[i]==-1 )
        {
            s->uidx.ptr.p_int[i] = s->ridx.ptr.p_int[i+1];
        }
        if( s->didx.ptr.p_int[i]==-1 )
        {
            s->didx.ptr.p_int[i] = s->uidx.ptr.p_int[i];
        }
    }
}


/*************************************************************************
This function return average length of chain at hash-table.

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
double sparsegetaveragelengthofchain(const sparsematrix* s,
     ae_state *_state)
{
    ae_int_t nchains;
    ae_int_t talc;
    ae_int_t l;
    ae_int_t i;
    ae_int_t ind0;
    ae_int_t ind1;
    ae_int_t hashcode;
    double result;


    
    /*
     * If matrix represent in CRS then return zero and exit
     */
    if( s->matrixtype!=0 )
    {
        result = (double)(0);
        return result;
    }
    nchains = 0;
    talc = 0;
    l = s->tablesize;
    for(i=0; i<=l-1; i++)
    {
        ind0 = 2*i;
        if( s->idx.ptr.p_int[ind0]!=-1 )
        {
            nchains = nchains+1;
            hashcode = sparse_hash(s->idx.ptr.p_int[ind0], s->idx.ptr.p_int[ind0+1], l, _state);
            for(;;)
            {
                talc = talc+1;
                ind1 = 2*hashcode;
                if( s->idx.ptr.p_int[ind0]==s->idx.ptr.p_int[ind1]&&s->idx.ptr.p_int[ind0+1]==s->idx.ptr.p_int[ind1+1] )
                {
                    break;
                }
                hashcode = (hashcode+1)%l;
            }
        }
    }
    if( nchains==0 )
    {
        result = (double)(0);
    }
    else
    {
        result = (double)talc/(double)nchains;
    }
    return result;
}


/*************************************************************************
This  function  is  used  to enumerate all elements of the sparse matrix.
Before  first  call  user  initializes  T0 and T1 counters by zero. These
counters are used to remember current position in a  matrix;  after  each
call they are updated by the function.

Subsequent calls to this function return non-zero elements of the  sparse
matrix, one by one. If you enumerate CRS matrix, matrix is traversed from
left to right, from top to bottom. In case you enumerate matrix stored as
Hash table, elements are returned in random order.

EXAMPLE
    > T0=0
    > T1=0
    > while SparseEnumerate(S,T0,T1,I,J,V) do
    >     ....do something with I,J,V

INPUT PARAMETERS
    S           -   sparse M*N matrix in Hash-Table or CRS representation.
    T0          -   internal counter
    T1          -   internal counter
    
OUTPUT PARAMETERS
    T0          -   new value of the internal counter
    T1          -   new value of the internal counter
    I           -   row index of non-zero element, 0<=I<M.
    J           -   column index of non-zero element, 0<=J<N
    V           -   value of the T-th element
    
RESULT
    True in case of success (next non-zero element was retrieved)
    False in case all non-zero elements were enumerated
    
NOTE: you may call SparseRewriteExisting() during enumeration, but it  is
      THE  ONLY  matrix  modification  function  you  can  call!!!  Other
      matrix modification functions should not be called during enumeration!

  -- ALGLIB PROJECT --
     Copyright 14.03.2012 by Bochkanov Sergey
*************************************************************************/
ae_bool sparseenumerate(const sparsematrix* s,
     ae_int_t* t0,
     ae_int_t* t1,
     ae_int_t* i,
     ae_int_t* j,
     double* v,
     ae_state *_state)
{
    ae_int_t sz;
    ae_int_t i0;
    ae_bool result;

    *i = 0;
    *j = 0;
    *v = 0.0;

    result = ae_false;
    if( *t0<0||(s->matrixtype!=0&&*t1<0) )
    {
        
        /*
         * Incorrect T0/T1, terminate enumeration
         */
        result = ae_false;
        return result;
    }
    if( s->matrixtype==0 )
    {
        
        /*
         * Hash-table matrix
         */
        sz = s->tablesize;
        for(i0=*t0; i0<=sz-1; i0++)
        {
            if( s->idx.ptr.p_int[2*i0]==-1||s->idx.ptr.p_int[2*i0]==-2 )
            {
                continue;
            }
            else
            {
                *i = s->idx.ptr.p_int[2*i0];
                *j = s->idx.ptr.p_int[2*i0+1];
                *v = s->vals.ptr.p_double[i0];
                *t0 = i0+1;
                result = ae_true;
                return result;
            }
        }
        *t0 = 0;
        *t1 = 0;
        result = ae_false;
        return result;
    }
    if( s->matrixtype==1 )
    {
        
        /*
         * CRS matrix
         */
        ae_assert(s->ninitialized==s->ridx.ptr.p_int[s->m], "SparseEnumerate: some rows/elements of the CRS matrix were not initialized (you must initialize everything you promised to SparseCreateCRS)", _state);
        if( *t0>=s->ninitialized )
        {
            *t0 = 0;
            *t1 = 0;
            result = ae_false;
            return result;
        }
        while(*t0>s->ridx.ptr.p_int[*t1+1]-1&&*t1<s->m)
        {
            *t1 = *t1+1;
        }
        *i = *t1;
        *j = s->idx.ptr.p_int[*t0];
        *v = s->vals.ptr.p_double[*t0];
        *t0 = *t0+1;
        result = ae_true;
        return result;
    }
    if( s->matrixtype==2 )
    {
        
        /*
         * SKS matrix:
         * * T0 stores current offset in Vals[] array
         * * T1 stores index of the diagonal block
         */
        ae_assert(s->m==s->n, "SparseEnumerate: non-square SKS matrices are not supported", _state);
        if( *t0>=s->ridx.ptr.p_int[s->m] )
        {
            *t0 = 0;
            *t1 = 0;
            result = ae_false;
            return result;
        }
        while(*t0>s->ridx.ptr.p_int[*t1+1]-1&&*t1<s->m)
        {
            *t1 = *t1+1;
        }
        i0 = *t0-s->ridx.ptr.p_int[*t1];
        if( i0<s->didx.ptr.p_int[*t1]+1 )
        {
            
            /*
             * subdiagonal or diagonal element, row index is T1.
             */
            *i = *t1;
            *j = *t1-s->didx.ptr.p_int[*t1]+i0;
        }
        else
        {
            
            /*
             * superdiagonal element, column index is T1.
             */
            *i = *t1-(s->ridx.ptr.p_int[*t1+1]-(*t0));
            *j = *t1;
        }
        *v = s->vals.ptr.p_double[*t0];
        *t0 = *t0+1;
        result = ae_true;
        return result;
    }
    ae_assert(ae_false, "SparseEnumerate: unexpected matrix type", _state);
    return result;
}


/*************************************************************************
This function rewrites existing (non-zero) element. It  returns  True   if
element  exists  or  False,  when  it  is  called for non-existing  (zero)
element.

This function works with any kind of the matrix.

The purpose of this function is to provide convenient thread-safe  way  to
modify  sparse  matrix.  Such  modification  (already  existing element is
rewritten) is guaranteed to be thread-safe without any synchronization, as
long as different threads modify different elements.

INPUT PARAMETERS
    S           -   sparse M*N matrix in any kind of representation
                    (Hash, SKS, CRS).
    I           -   row index of non-zero element to modify, 0<=I<M
    J           -   column index of non-zero element to modify, 0<=J<N
    V           -   value to rewrite, must be finite number

OUTPUT PARAMETERS
    S           -   modified matrix
RESULT
    True in case when element exists
    False in case when element doesn't exist or it is zero
    
  -- ALGLIB PROJECT --
     Copyright 14.03.2012 by Bochkanov Sergey
*************************************************************************/
ae_bool sparserewriteexisting(sparsematrix* s,
     ae_int_t i,
     ae_int_t j,
     double v,
     ae_state *_state)
{
    ae_int_t hashcode;
    ae_int_t k;
    ae_int_t k0;
    ae_int_t k1;
    ae_bool result;


    ae_assert(0<=i&&i<s->m, "SparseRewriteExisting: invalid argument I(either I<0 or I>=S.M)", _state);
    ae_assert(0<=j&&j<s->n, "SparseRewriteExisting: invalid argument J(either J<0 or J>=S.N)", _state);
    ae_assert(ae_isfinite(v, _state), "SparseRewriteExisting: invalid argument V(either V is infinite or V is NaN)", _state);
    result = ae_false;
    
    /*
     * Hash-table matrix
     */
    if( s->matrixtype==0 )
    {
        k = s->tablesize;
        hashcode = sparse_hash(i, j, k, _state);
        for(;;)
        {
            if( s->idx.ptr.p_int[2*hashcode]==-1 )
            {
                return result;
            }
            if( s->idx.ptr.p_int[2*hashcode]==i&&s->idx.ptr.p_int[2*hashcode+1]==j )
            {
                s->vals.ptr.p_double[hashcode] = v;
                result = ae_true;
                return result;
            }
            hashcode = (hashcode+1)%k;
        }
    }
    
    /*
     * CRS matrix
     */
    if( s->matrixtype==1 )
    {
        ae_assert(s->ninitialized==s->ridx.ptr.p_int[s->m], "SparseRewriteExisting: some rows/elements of the CRS matrix were not initialized (you must initialize everything you promised to SparseCreateCRS)", _state);
        k0 = s->ridx.ptr.p_int[i];
        k1 = s->ridx.ptr.p_int[i+1]-1;
        while(k0<=k1)
        {
            k = (k0+k1)/2;
            if( s->idx.ptr.p_int[k]==j )
            {
                s->vals.ptr.p_double[k] = v;
                result = ae_true;
                return result;
            }
            if( s->idx.ptr.p_int[k]<j )
            {
                k0 = k+1;
            }
            else
            {
                k1 = k-1;
            }
        }
    }
    
    /*
     * SKS
     */
    if( s->matrixtype==2 )
    {
        ae_assert(s->m==s->n, "SparseRewriteExisting: non-square SKS matrix not supported", _state);
        if( i==j )
        {
            
            /*
             * Rewrite diagonal element
             */
            result = ae_true;
            s->vals.ptr.p_double[s->ridx.ptr.p_int[i]+s->didx.ptr.p_int[i]] = v;
            return result;
        }
        if( j<i )
        {
            
            /*
             * Return subdiagonal element at I-th "skyline block"
             */
            k = s->didx.ptr.p_int[i];
            if( i-j<=k )
            {
                s->vals.ptr.p_double[s->ridx.ptr.p_int[i]+k+j-i] = v;
                result = ae_true;
            }
        }
        else
        {
            
            /*
             * Return superdiagonal element at J-th "skyline block"
             */
            k = s->uidx.ptr.p_int[j];
            if( j-i<=k )
            {
                s->vals.ptr.p_double[s->ridx.ptr.p_int[j+1]-(j-i)] = v;
                result = ae_true;
            }
        }
        return result;
    }
    return result;
}


/*************************************************************************
This function returns I-th row of the sparse matrix. Matrix must be stored
in CRS or SKS format.

INPUT PARAMETERS:
    S           -   sparse M*N matrix in CRS format
    I           -   row index, 0<=I<M
    IRow        -   output buffer, can be  preallocated.  In  case  buffer
                    size  is  too  small  to  store  I-th   row,   it   is
                    automatically reallocated.
 
OUTPUT PARAMETERS:
    IRow        -   array[M], I-th row.
    
NOTE: this function has O(N) running time, where N is a  column  count. It
      allocates and fills N-element  array,  even  although  most  of  its
      elemets are zero.
      
NOTE: If you have O(non-zeros-per-row) time and memory  requirements,  use
      SparseGetCompressedRow() function. It  returns  data  in  compressed
      format.

NOTE: when  incorrect  I  (outside  of  [0,M-1]) or  matrix (non  CRS/SKS)
      is passed, this function throws exception.

  -- ALGLIB PROJECT --
     Copyright 10.12.2014 by Bochkanov Sergey
*************************************************************************/
void sparsegetrow(const sparsematrix* s,
     ae_int_t i,
     /* Real    */ ae_vector* irow,
     ae_state *_state)
{
    ae_int_t i0;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t j;
    ae_int_t upperprofile;


    ae_assert(s->matrixtype==1||s->matrixtype==2, "SparseGetRow: S must be CRS/SKS-based matrix", _state);
    ae_assert(i>=0&&i<s->m, "SparseGetRow: I<0 or I>=M", _state);
    
    /*
     * Prepare output buffer
     */
    rvectorsetlengthatleast(irow, s->n, _state);
    for(i0=0; i0<=s->n-1; i0++)
    {
        irow->ptr.p_double[i0] = (double)(0);
    }
    
    /*
     * Output
     */
    if( s->matrixtype==1 )
    {
        for(i0=s->ridx.ptr.p_int[i]; i0<=s->ridx.ptr.p_int[i+1]-1; i0++)
        {
            irow->ptr.p_double[s->idx.ptr.p_int[i0]] = s->vals.ptr.p_double[i0];
        }
        return;
    }
    if( s->matrixtype==2 )
    {
        
        /*
         * Copy subdiagonal and diagonal parts
         */
        ae_assert(s->n==s->m, "SparseGetRow: non-square SKS matrices are not supported", _state);
        j0 = i-s->didx.ptr.p_int[i];
        i0 = -j0+s->ridx.ptr.p_int[i];
        for(j=j0; j<=i; j++)
        {
            irow->ptr.p_double[j] = s->vals.ptr.p_double[j+i0];
        }
        
        /*
         * Copy superdiagonal part
         */
        upperprofile = s->uidx.ptr.p_int[s->n];
        j0 = i+1;
        j1 = ae_minint(s->n-1, i+upperprofile, _state);
        for(j=j0; j<=j1; j++)
        {
            if( j-i<=s->uidx.ptr.p_int[j] )
            {
                irow->ptr.p_double[j] = s->vals.ptr.p_double[s->ridx.ptr.p_int[j+1]-(j-i)];
            }
        }
        return;
    }
}


/*************************************************************************
This function returns I-th row of the sparse matrix IN COMPRESSED FORMAT -
only non-zero elements are returned (with their indexes). Matrix  must  be
stored in CRS or SKS format.

INPUT PARAMETERS:
    S           -   sparse M*N matrix in CRS format
    I           -   row index, 0<=I<M
    ColIdx      -   output buffer for column indexes, can be preallocated.
                    In case buffer size is too small to store I-th row, it
                    is automatically reallocated.
    Vals        -   output buffer for values, can be preallocated. In case
                    buffer size is too small to  store  I-th  row,  it  is
                    automatically reallocated.
 
OUTPUT PARAMETERS:
    ColIdx      -   column   indexes   of  non-zero  elements,  sorted  by
                    ascending. Symbolically non-zero elements are  counted
                    (i.e. if you allocated place for element, but  it  has
                    zero numerical value - it is counted).
    Vals        -   values. Vals[K] stores value of  matrix  element  with
                    indexes (I,ColIdx[K]). Symbolically non-zero  elements
                    are counted (i.e. if you allocated place for  element,
                    but it has zero numerical value - it is counted).
    NZCnt       -   number of symbolically non-zero elements per row.

NOTE: when  incorrect  I  (outside  of  [0,M-1]) or  matrix (non  CRS/SKS)
      is passed, this function throws exception.
      
NOTE: this function may allocate additional, unnecessary place for  ColIdx
      and Vals arrays. It is dictated by  performance  reasons  -  on  SKS
      matrices it is faster  to  allocate  space  at  the  beginning  with
      some "extra"-space, than performing two passes over matrix  -  first
      time to calculate exact space required for data, second  time  -  to
      store data itself.

  -- ALGLIB PROJECT --
     Copyright 10.12.2014 by Bochkanov Sergey
*************************************************************************/
void sparsegetcompressedrow(const sparsematrix* s,
     ae_int_t i,
     /* Integer */ ae_vector* colidx,
     /* Real    */ ae_vector* vals,
     ae_int_t* nzcnt,
     ae_state *_state)
{
    ae_int_t k;
    ae_int_t k0;
    ae_int_t j;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t i0;
    ae_int_t upperprofile;

    *nzcnt = 0;

    ae_assert(s->matrixtype==1||s->matrixtype==2, "SparseGetRow: S must be CRS/SKS-based matrix", _state);
    ae_assert(i>=0&&i<s->m, "SparseGetRow: I<0 or I>=M", _state);
    
    /*
     * Initialize NZCnt
     */
    *nzcnt = 0;
    
    /*
     * CRS matrix - just copy data
     */
    if( s->matrixtype==1 )
    {
        *nzcnt = s->ridx.ptr.p_int[i+1]-s->ridx.ptr.p_int[i];
        ivectorsetlengthatleast(colidx, *nzcnt, _state);
        rvectorsetlengthatleast(vals, *nzcnt, _state);
        k0 = s->ridx.ptr.p_int[i];
        for(k=0; k<=*nzcnt-1; k++)
        {
            colidx->ptr.p_int[k] = s->idx.ptr.p_int[k0+k];
            vals->ptr.p_double[k] = s->vals.ptr.p_double[k0+k];
        }
        return;
    }
    
    /*
     * SKS matrix - a bit more complex sequence
     */
    if( s->matrixtype==2 )
    {
        ae_assert(s->n==s->m, "SparseGetCompressedRow: non-square SKS matrices are not supported", _state);
        
        /*
         * Allocate enough place for storage
         */
        upperprofile = s->uidx.ptr.p_int[s->n];
        ivectorsetlengthatleast(colidx, s->didx.ptr.p_int[i]+1+upperprofile, _state);
        rvectorsetlengthatleast(vals, s->didx.ptr.p_int[i]+1+upperprofile, _state);
        
        /*
         * Copy subdiagonal and diagonal parts
         */
        j0 = i-s->didx.ptr.p_int[i];
        i0 = -j0+s->ridx.ptr.p_int[i];
        for(j=j0; j<=i; j++)
        {
            colidx->ptr.p_int[*nzcnt] = j;
            vals->ptr.p_double[*nzcnt] = s->vals.ptr.p_double[j+i0];
            *nzcnt = *nzcnt+1;
        }
        
        /*
         * Copy superdiagonal part
         */
        j0 = i+1;
        j1 = ae_minint(s->n-1, i+upperprofile, _state);
        for(j=j0; j<=j1; j++)
        {
            if( j-i<=s->uidx.ptr.p_int[j] )
            {
                colidx->ptr.p_int[*nzcnt] = j;
                vals->ptr.p_double[*nzcnt] = s->vals.ptr.p_double[s->ridx.ptr.p_int[j+1]-(j-i)];
                *nzcnt = *nzcnt+1;
            }
        }
        return;
    }
}


/*************************************************************************
This function performs efficient in-place  transpose  of  SKS  matrix.  No
additional memory is allocated during transposition.

This function supports only skyline storage format (SKS).

INPUT PARAMETERS
    S       -   sparse matrix in SKS format.

OUTPUT PARAMETERS
    S           -   sparse matrix, transposed.

  -- ALGLIB PROJECT --
     Copyright 16.01.2014 by Bochkanov Sergey
*************************************************************************/
void sparsetransposesks(sparsematrix* s, ae_state *_state)
{
    ae_int_t n;
    ae_int_t d;
    ae_int_t u;
    ae_int_t i;
    ae_int_t k;
    ae_int_t t0;
    ae_int_t t1;
    double v;


    ae_assert(s->matrixtype==2, "SparseTransposeSKS: only SKS matrices are supported", _state);
    ae_assert(s->m==s->n, "SparseTransposeSKS: non-square SKS matrices are not supported", _state);
    n = s->n;
    for(i=1; i<=n-1; i++)
    {
        d = s->didx.ptr.p_int[i];
        u = s->uidx.ptr.p_int[i];
        k = s->uidx.ptr.p_int[i];
        s->uidx.ptr.p_int[i] = s->didx.ptr.p_int[i];
        s->didx.ptr.p_int[i] = k;
        if( d==u )
        {
            
            /*
             * Upper skyline height equal to lower skyline height,
             * simple exchange is needed for transposition
             */
            t0 = s->ridx.ptr.p_int[i];
            for(k=0; k<=d-1; k++)
            {
                v = s->vals.ptr.p_double[t0+k];
                s->vals.ptr.p_double[t0+k] = s->vals.ptr.p_double[t0+d+1+k];
                s->vals.ptr.p_double[t0+d+1+k] = v;
            }
        }
        if( d>u )
        {
            
            /*
             * Upper skyline height is less than lower skyline height.
             *
             * Transposition becomes a bit tricky: we have to rearrange
             * "L0 L1 D U" to "U D L0 L1", where |L0|=|U|=u, |L1|=d-u.
             *
             * In order to do this we perform a sequence of swaps and
             * in-place reversals:
             * * swap(L0,U)         =>  "U   L1  D   L0"
             * * reverse("L1 D L0") =>  "U   L0~ D   L1~" (where X~ is a reverse of X)
             * * reverse("L0~ D")   =>  "U   D   L0  L1~"
             * * reverse("L1")      =>  "U   D   L0  L1"
             */
            t0 = s->ridx.ptr.p_int[i];
            t1 = s->ridx.ptr.p_int[i]+d+1;
            for(k=0; k<=u-1; k++)
            {
                v = s->vals.ptr.p_double[t0+k];
                s->vals.ptr.p_double[t0+k] = s->vals.ptr.p_double[t1+k];
                s->vals.ptr.p_double[t1+k] = v;
            }
            t0 = s->ridx.ptr.p_int[i]+u;
            t1 = s->ridx.ptr.p_int[i+1]-1;
            while(t1>t0)
            {
                v = s->vals.ptr.p_double[t0];
                s->vals.ptr.p_double[t0] = s->vals.ptr.p_double[t1];
                s->vals.ptr.p_double[t1] = v;
                t0 = t0+1;
                t1 = t1-1;
            }
            t0 = s->ridx.ptr.p_int[i]+u;
            t1 = s->ridx.ptr.p_int[i]+u+u;
            while(t1>t0)
            {
                v = s->vals.ptr.p_double[t0];
                s->vals.ptr.p_double[t0] = s->vals.ptr.p_double[t1];
                s->vals.ptr.p_double[t1] = v;
                t0 = t0+1;
                t1 = t1-1;
            }
            t0 = s->ridx.ptr.p_int[i+1]-(d-u);
            t1 = s->ridx.ptr.p_int[i+1]-1;
            while(t1>t0)
            {
                v = s->vals.ptr.p_double[t0];
                s->vals.ptr.p_double[t0] = s->vals.ptr.p_double[t1];
                s->vals.ptr.p_double[t1] = v;
                t0 = t0+1;
                t1 = t1-1;
            }
        }
        if( d<u )
        {
            
            /*
             * Upper skyline height is greater than lower skyline height.
             *
             * Transposition becomes a bit tricky: we have to rearrange
             * "L D U0 U1" to "U0 U1 D L", where |U1|=|L|=d, |U0|=u-d.
             *
             * In order to do this we perform a sequence of swaps and
             * in-place reversals:
             * * swap(L,U1)         =>  "U1  D   U0  L"
             * * reverse("U1 D U0") =>  "U0~ D   U1~ L" (where X~ is a reverse of X)
             * * reverse("U0~")     =>  "U0  D   U1~ L"
             * * reverse("D U1~")   =>  "U0  U1  D   L"
             */
            t0 = s->ridx.ptr.p_int[i];
            t1 = s->ridx.ptr.p_int[i+1]-d;
            for(k=0; k<=d-1; k++)
            {
                v = s->vals.ptr.p_double[t0+k];
                s->vals.ptr.p_double[t0+k] = s->vals.ptr.p_double[t1+k];
                s->vals.ptr.p_double[t1+k] = v;
            }
            t0 = s->ridx.ptr.p_int[i];
            t1 = s->ridx.ptr.p_int[i]+u;
            while(t1>t0)
            {
                v = s->vals.ptr.p_double[t0];
                s->vals.ptr.p_double[t0] = s->vals.ptr.p_double[t1];
                s->vals.ptr.p_double[t1] = v;
                t0 = t0+1;
                t1 = t1-1;
            }
            t0 = s->ridx.ptr.p_int[i];
            t1 = s->ridx.ptr.p_int[i]+u-d-1;
            while(t1>t0)
            {
                v = s->vals.ptr.p_double[t0];
                s->vals.ptr.p_double[t0] = s->vals.ptr.p_double[t1];
                s->vals.ptr.p_double[t1] = v;
                t0 = t0+1;
                t1 = t1-1;
            }
            t0 = s->ridx.ptr.p_int[i]+u-d;
            t1 = s->ridx.ptr.p_int[i+1]-d-1;
            while(t1>t0)
            {
                v = s->vals.ptr.p_double[t0];
                s->vals.ptr.p_double[t0] = s->vals.ptr.p_double[t1];
                s->vals.ptr.p_double[t1] = v;
                t0 = t0+1;
                t1 = t1-1;
            }
        }
    }
    k = s->uidx.ptr.p_int[n];
    s->uidx.ptr.p_int[n] = s->didx.ptr.p_int[n];
    s->didx.ptr.p_int[n] = k;
}


/*************************************************************************
This function performs transpose of CRS matrix.

INPUT PARAMETERS
    S       -   sparse matrix in CRS format.

OUTPUT PARAMETERS
    S           -   sparse matrix, transposed.

NOTE: internal  temporary  copy  is  allocated   for   the   purposes   of
      transposition. It is deallocated after transposition.

  -- ALGLIB PROJECT --
     Copyright 30.01.2018 by Bochkanov Sergey
*************************************************************************/
void sparsetransposecrs(sparsematrix* s, ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector oldvals;
    ae_vector oldidx;
    ae_vector oldridx;
    ae_int_t oldn;
    ae_int_t oldm;
    ae_int_t newn;
    ae_int_t newm;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t nonne;
    ae_vector counts;

    ae_frame_make(_state, &_frame_block);
    memset(&oldvals, 0, sizeof(oldvals));
    memset(&oldidx, 0, sizeof(oldidx));
    memset(&oldridx, 0, sizeof(oldridx));
    memset(&counts, 0, sizeof(counts));
    ae_vector_init(&oldvals, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&oldidx, 0, DT_INT, _state, ae_true);
    ae_vector_init(&oldridx, 0, DT_INT, _state, ae_true);
    ae_vector_init(&counts, 0, DT_INT, _state, ae_true);

    ae_assert(s->matrixtype==1, "SparseTransposeCRS: only CRS matrices are supported", _state);
    ae_swap_vectors(&s->vals, &oldvals);
    ae_swap_vectors(&s->idx, &oldidx);
    ae_swap_vectors(&s->ridx, &oldridx);
    oldn = s->n;
    oldm = s->m;
    newn = oldm;
    newm = oldn;
    
    /*
     * Update matrix size
     */
    s->n = newn;
    s->m = newm;
    
    /*
     * Fill RIdx by number of elements per row:
     * RIdx[I+1] stores number of elements in I-th row.
     *
     * Convert RIdx from row sizes to row offsets.
     * Set NInitialized
     */
    nonne = 0;
    ivectorsetlengthatleast(&s->ridx, newm+1, _state);
    for(i=0; i<=newm; i++)
    {
        s->ridx.ptr.p_int[i] = 0;
    }
    for(i=0; i<=oldm-1; i++)
    {
        for(j=oldridx.ptr.p_int[i]; j<=oldridx.ptr.p_int[i+1]-1; j++)
        {
            k = oldidx.ptr.p_int[j]+1;
            s->ridx.ptr.p_int[k] = s->ridx.ptr.p_int[k]+1;
            nonne = nonne+1;
        }
    }
    for(i=0; i<=newm-1; i++)
    {
        s->ridx.ptr.p_int[i+1] = s->ridx.ptr.p_int[i+1]+s->ridx.ptr.p_int[i];
    }
    s->ninitialized = s->ridx.ptr.p_int[newm];
    
    /*
     * Allocate memory and move elements to Vals/Idx.
     */
    ae_vector_set_length(&counts, newm, _state);
    for(i=0; i<=newm-1; i++)
    {
        counts.ptr.p_int[i] = 0;
    }
    rvectorsetlengthatleast(&s->vals, nonne, _state);
    ivectorsetlengthatleast(&s->idx, nonne, _state);
    for(i=0; i<=oldm-1; i++)
    {
        for(j=oldridx.ptr.p_int[i]; j<=oldridx.ptr.p_int[i+1]-1; j++)
        {
            k = oldidx.ptr.p_int[j];
            k = s->ridx.ptr.p_int[k]+counts.ptr.p_int[k];
            s->idx.ptr.p_int[k] = i;
            s->vals.ptr.p_double[k] = oldvals.ptr.p_double[j];
            k = oldidx.ptr.p_int[j];
            counts.ptr.p_int[k] = counts.ptr.p_int[k]+1;
        }
    }
    
    /*
     * Initialization 'S.UIdx' and 'S.DIdx'
     */
    sparseinitduidx(s, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function performs copying with transposition of CRS matrix.

INPUT PARAMETERS
    S0      -   sparse matrix in CRS format.

OUTPUT PARAMETERS
    S1      -   sparse matrix, transposed

  -- ALGLIB PROJECT --
     Copyright 23.07.2018 by Bochkanov Sergey
*************************************************************************/
void sparsecopytransposecrs(const sparsematrix* s0,
     sparsematrix* s1,
     ae_state *_state)
{

    _sparsematrix_clear(s1);

    sparsecopytransposecrsbuf(s0, s1, _state);
}


/*************************************************************************
This function performs copying with transposition of CRS matrix  (buffered
version which reuses memory already allocated by  the  target as  much  as
possible).

INPUT PARAMETERS
    S0      -   sparse matrix in CRS format.

OUTPUT PARAMETERS
    S1      -   sparse matrix, transposed; previously allocated memory  is
                reused if possible.

  -- ALGLIB PROJECT --
     Copyright 23.07.2018 by Bochkanov Sergey
*************************************************************************/
void sparsecopytransposecrsbuf(const sparsematrix* s0,
     sparsematrix* s1,
     ae_state *_state)
{
    ae_int_t oldn;
    ae_int_t oldm;
    ae_int_t newn;
    ae_int_t newm;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t kk;
    ae_int_t j0;
    ae_int_t j1;


    ae_assert(s0->matrixtype==1, "SparseCopyTransposeCRSBuf: only CRS matrices are supported", _state);
    oldn = s0->n;
    oldm = s0->m;
    newn = oldm;
    newm = oldn;
    
    /*
     * Update matrix size
     */
    s1->matrixtype = 1;
    s1->n = newn;
    s1->m = newm;
    
    /*
     * Fill RIdx by number of elements per row:
     * RIdx[I+1] stores number of elements in I-th row.
     *
     * Convert RIdx from row sizes to row offsets.
     * Set NInitialized
     */
    isetallocv(newm+1, 0, &s1->ridx, _state);
    for(i=0; i<=oldm-1; i++)
    {
        j0 = s0->ridx.ptr.p_int[i];
        j1 = s0->ridx.ptr.p_int[i+1]-1;
        for(j=j0; j<=j1; j++)
        {
            k = s0->idx.ptr.p_int[j]+1;
            s1->ridx.ptr.p_int[k] = s1->ridx.ptr.p_int[k]+1;
        }
    }
    for(i=0; i<=newm-1; i++)
    {
        s1->ridx.ptr.p_int[i+1] = s1->ridx.ptr.p_int[i+1]+s1->ridx.ptr.p_int[i];
    }
    s1->ninitialized = s1->ridx.ptr.p_int[newm];
    
    /*
     * Allocate memory and move elements to Vals/Idx.
     */
    ivectorsetlengthatleast(&s1->didx, newm, _state);
    for(i=0; i<=newm-1; i++)
    {
        s1->didx.ptr.p_int[i] = s1->ridx.ptr.p_int[i];
    }
    rvectorsetlengthatleast(&s1->vals, s1->ninitialized, _state);
    ivectorsetlengthatleast(&s1->idx, s1->ninitialized, _state);
    for(i=0; i<=oldm-1; i++)
    {
        j0 = s0->ridx.ptr.p_int[i];
        j1 = s0->ridx.ptr.p_int[i+1]-1;
        for(j=j0; j<=j1; j++)
        {
            kk = s0->idx.ptr.p_int[j];
            k = s1->didx.ptr.p_int[kk];
            s1->idx.ptr.p_int[k] = i;
            s1->vals.ptr.p_double[k] = s0->vals.ptr.p_double[j];
            s1->didx.ptr.p_int[kk] = k+1;
        }
    }
    
    /*
     * Initialization 'S.UIdx' and 'S.DIdx'
     */
    sparseinitduidx(s1, _state);
}


/*************************************************************************
This  function  performs  in-place  conversion  to  desired sparse storage
format.

INPUT PARAMETERS
    S0      -   sparse matrix in any format.
    Fmt     -   desired storage format  of  the  output,  as  returned  by
                SparseGetMatrixType() function:
                * 0 for hash-based storage
                * 1 for CRS
                * 2 for SKS

OUTPUT PARAMETERS
    S0          -   sparse matrix in requested format.
    
NOTE: in-place conversion wastes a lot of memory which is  used  to  store
      temporaries.  If  you  perform  a  lot  of  repeated conversions, we
      recommend to use out-of-place buffered  conversion  functions,  like
      SparseCopyToBuf(), which can reuse already allocated memory.

  -- ALGLIB PROJECT --
     Copyright 16.01.2014 by Bochkanov Sergey
*************************************************************************/
void sparseconvertto(sparsematrix* s0, ae_int_t fmt, ae_state *_state)
{


    ae_assert((fmt==0||fmt==1)||fmt==2, "SparseConvertTo: invalid fmt parameter", _state);
    if( fmt==0 )
    {
        sparseconverttohash(s0, _state);
        return;
    }
    if( fmt==1 )
    {
        sparseconverttocrs(s0, _state);
        return;
    }
    if( fmt==2 )
    {
        sparseconverttosks(s0, _state);
        return;
    }
    ae_assert(ae_false, "SparseConvertTo: invalid matrix type", _state);
}


/*************************************************************************
This  function  performs out-of-place conversion to desired sparse storage
format. S0 is copied to S1 and converted on-the-fly. Memory  allocated  in
S1 is reused to maximum extent possible.

INPUT PARAMETERS
    S0      -   sparse matrix in any format.
    Fmt     -   desired storage format  of  the  output,  as  returned  by
                SparseGetMatrixType() function:
                * 0 for hash-based storage
                * 1 for CRS
                * 2 for SKS

OUTPUT PARAMETERS
    S1          -   sparse matrix in requested format.

  -- ALGLIB PROJECT --
     Copyright 16.01.2014 by Bochkanov Sergey
*************************************************************************/
void sparsecopytobuf(const sparsematrix* s0,
     ae_int_t fmt,
     sparsematrix* s1,
     ae_state *_state)
{


    ae_assert((fmt==0||fmt==1)||fmt==2, "SparseCopyToBuf: invalid fmt parameter", _state);
    if( fmt==0 )
    {
        sparsecopytohashbuf(s0, s1, _state);
        return;
    }
    if( fmt==1 )
    {
        sparsecopytocrsbuf(s0, s1, _state);
        return;
    }
    if( fmt==2 )
    {
        sparsecopytosksbuf(s0, s1, _state);
        return;
    }
    ae_assert(ae_false, "SparseCopyToBuf: invalid matrix type", _state);
}


/*************************************************************************
This function performs in-place conversion to Hash table storage.

INPUT PARAMETERS
    S           -   sparse matrix in CRS format.

OUTPUT PARAMETERS
    S           -   sparse matrix in Hash table format.

NOTE: this  function  has   no  effect  when  called with matrix which  is
      already in Hash table mode.

NOTE: in-place conversion involves allocation of temporary arrays. If  you
      perform a lot of repeated in- place  conversions,  it  may  lead  to
      memory fragmentation. Consider using out-of-place SparseCopyToHashBuf()
      function in this case.
    
  -- ALGLIB PROJECT --
     Copyright 20.07.2012 by Bochkanov Sergey
*************************************************************************/
void sparseconverttohash(sparsematrix* s, ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector tidx;
    ae_vector tridx;
    ae_vector tdidx;
    ae_vector tuidx;
    ae_vector tvals;
    ae_int_t n;
    ae_int_t m;
    ae_int_t offs0;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;

    ae_frame_make(_state, &_frame_block);
    memset(&tidx, 0, sizeof(tidx));
    memset(&tridx, 0, sizeof(tridx));
    memset(&tdidx, 0, sizeof(tdidx));
    memset(&tuidx, 0, sizeof(tuidx));
    memset(&tvals, 0, sizeof(tvals));
    ae_vector_init(&tidx, 0, DT_INT, _state, ae_true);
    ae_vector_init(&tridx, 0, DT_INT, _state, ae_true);
    ae_vector_init(&tdidx, 0, DT_INT, _state, ae_true);
    ae_vector_init(&tuidx, 0, DT_INT, _state, ae_true);
    ae_vector_init(&tvals, 0, DT_REAL, _state, ae_true);

    ae_assert((s->matrixtype==0||s->matrixtype==1)||s->matrixtype==2, "SparseConvertToHash: invalid matrix type", _state);
    if( s->matrixtype==0 )
    {
        
        /*
         * Already in Hash mode
         */
        ae_frame_leave(_state);
        return;
    }
    if( s->matrixtype==1 )
    {
        
        /*
         * From CRS to Hash
         */
        s->matrixtype = 0;
        m = s->m;
        n = s->n;
        ae_swap_vectors(&s->idx, &tidx);
        ae_swap_vectors(&s->ridx, &tridx);
        ae_swap_vectors(&s->vals, &tvals);
        sparsecreatebuf(m, n, tridx.ptr.p_int[m], s, _state);
        for(i=0; i<=m-1; i++)
        {
            for(j=tridx.ptr.p_int[i]; j<=tridx.ptr.p_int[i+1]-1; j++)
            {
                sparseset(s, i, tidx.ptr.p_int[j], tvals.ptr.p_double[j], _state);
            }
        }
        ae_frame_leave(_state);
        return;
    }
    if( s->matrixtype==2 )
    {
        
        /*
         * From SKS to Hash
         */
        s->matrixtype = 0;
        m = s->m;
        n = s->n;
        ae_swap_vectors(&s->ridx, &tridx);
        ae_swap_vectors(&s->didx, &tdidx);
        ae_swap_vectors(&s->uidx, &tuidx);
        ae_swap_vectors(&s->vals, &tvals);
        sparsecreatebuf(m, n, tridx.ptr.p_int[m], s, _state);
        for(i=0; i<=m-1; i++)
        {
            
            /*
             * copy subdiagonal and diagonal parts of I-th block
             */
            offs0 = tridx.ptr.p_int[i];
            k = tdidx.ptr.p_int[i]+1;
            for(j=0; j<=k-1; j++)
            {
                sparseset(s, i, i-tdidx.ptr.p_int[i]+j, tvals.ptr.p_double[offs0+j], _state);
            }
            
            /*
             * Copy superdiagonal part of I-th block
             */
            offs0 = tridx.ptr.p_int[i]+tdidx.ptr.p_int[i]+1;
            k = tuidx.ptr.p_int[i];
            for(j=0; j<=k-1; j++)
            {
                sparseset(s, i-k+j, i, tvals.ptr.p_double[offs0+j], _state);
            }
        }
        ae_frame_leave(_state);
        return;
    }
    ae_assert(ae_false, "SparseConvertToHash: invalid matrix type", _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This  function  performs  out-of-place  conversion  to  Hash table storage
format. S0 is copied to S1 and converted on-the-fly.

INPUT PARAMETERS
    S0          -   sparse matrix in any format.

OUTPUT PARAMETERS
    S1          -   sparse matrix in Hash table format.

NOTE: if S0 is stored as Hash-table, it is just copied without conversion.

NOTE: this function de-allocates memory  occupied  by  S1 before  starting
      conversion. If you perform a  lot  of  repeated  conversions, it may
      lead to memory fragmentation. In this case we recommend you  to  use
      SparseCopyToHashBuf() function which re-uses memory in S1 as much as
      possible.

  -- ALGLIB PROJECT --
     Copyright 20.07.2012 by Bochkanov Sergey
*************************************************************************/
void sparsecopytohash(const sparsematrix* s0,
     sparsematrix* s1,
     ae_state *_state)
{

    _sparsematrix_clear(s1);

    ae_assert((s0->matrixtype==0||s0->matrixtype==1)||s0->matrixtype==2, "SparseCopyToHash: invalid matrix type", _state);
    sparsecopytohashbuf(s0, s1, _state);
}


/*************************************************************************
This  function  performs  out-of-place  conversion  to  Hash table storage
format. S0 is copied to S1 and converted on-the-fly. Memory  allocated  in
S1 is reused to maximum extent possible.

INPUT PARAMETERS
    S0          -   sparse matrix in any format.

OUTPUT PARAMETERS
    S1          -   sparse matrix in Hash table format.

NOTE: if S0 is stored as Hash-table, it is just copied without conversion.

  -- ALGLIB PROJECT --
     Copyright 20.07.2012 by Bochkanov Sergey
*************************************************************************/
void sparsecopytohashbuf(const sparsematrix* s0,
     sparsematrix* s1,
     ae_state *_state)
{
    double val;
    ae_int_t t0;
    ae_int_t t1;
    ae_int_t i;
    ae_int_t j;


    ae_assert((s0->matrixtype==0||s0->matrixtype==1)||s0->matrixtype==2, "SparseCopyToHashBuf: invalid matrix type", _state);
    if( s0->matrixtype==0 )
    {
        
        /*
         * Already hash, just copy
         */
        sparsecopybuf(s0, s1, _state);
        return;
    }
    if( s0->matrixtype==1 )
    {
        
        /*
         * CRS storage
         */
        t0 = 0;
        t1 = 0;
        sparsecreatebuf(s0->m, s0->n, s0->ridx.ptr.p_int[s0->m], s1, _state);
        while(sparseenumerate(s0, &t0, &t1, &i, &j, &val, _state))
        {
            sparseset(s1, i, j, val, _state);
        }
        return;
    }
    if( s0->matrixtype==2 )
    {
        
        /*
         * SKS storage
         */
        t0 = 0;
        t1 = 0;
        sparsecreatebuf(s0->m, s0->n, s0->ridx.ptr.p_int[s0->m], s1, _state);
        while(sparseenumerate(s0, &t0, &t1, &i, &j, &val, _state))
        {
            sparseset(s1, i, j, val, _state);
        }
        return;
    }
    ae_assert(ae_false, "SparseCopyToHashBuf: invalid matrix type", _state);
}


/*************************************************************************
This function converts matrix to CRS format.

Some  algorithms  (linear  algebra ones, for example) require matrices in
CRS format. This function allows to perform in-place conversion.

INPUT PARAMETERS
    S           -   sparse M*N matrix in any format

OUTPUT PARAMETERS
    S           -   matrix in CRS format
    
NOTE: this   function  has  no  effect  when  called with matrix which is
      already in CRS mode.
      
NOTE: this function allocates temporary memory to store a   copy  of  the
      matrix. If you perform a lot of repeated conversions, we  recommend
      you  to  use  SparseCopyToCRSBuf()  function,   which   can   reuse
      previously allocated memory.

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
void sparseconverttocrs(sparsematrix* s, ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t m;
    ae_int_t i;
    ae_int_t j;
    ae_vector tvals;
    ae_vector tidx;
    ae_vector temp;
    ae_vector tridx;
    ae_int_t nonne;
    ae_int_t k;
    ae_int_t offs0;
    ae_int_t offs1;

    ae_frame_make(_state, &_frame_block);
    memset(&tvals, 0, sizeof(tvals));
    memset(&tidx, 0, sizeof(tidx));
    memset(&temp, 0, sizeof(temp));
    memset(&tridx, 0, sizeof(tridx));
    ae_vector_init(&tvals, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tidx, 0, DT_INT, _state, ae_true);
    ae_vector_init(&temp, 0, DT_INT, _state, ae_true);
    ae_vector_init(&tridx, 0, DT_INT, _state, ae_true);

    m = s->m;
    if( s->matrixtype==0 )
    {
        
        /*
         * From Hash-table to CRS.
         * First, create local copy of the hash table.
         */
        s->matrixtype = 1;
        k = s->tablesize;
        ae_swap_vectors(&s->vals, &tvals);
        ae_swap_vectors(&s->idx, &tidx);
        
        /*
         * Fill RIdx by number of elements per row:
         * RIdx[I+1] stores number of elements in I-th row.
         *
         * Convert RIdx from row sizes to row offsets.
         * Set NInitialized
         */
        nonne = 0;
        ivectorsetlengthatleast(&s->ridx, s->m+1, _state);
        for(i=0; i<=s->m; i++)
        {
            s->ridx.ptr.p_int[i] = 0;
        }
        for(i=0; i<=k-1; i++)
        {
            if( tidx.ptr.p_int[2*i]>=0 )
            {
                s->ridx.ptr.p_int[tidx.ptr.p_int[2*i]+1] = s->ridx.ptr.p_int[tidx.ptr.p_int[2*i]+1]+1;
                nonne = nonne+1;
            }
        }
        for(i=0; i<=s->m-1; i++)
        {
            s->ridx.ptr.p_int[i+1] = s->ridx.ptr.p_int[i+1]+s->ridx.ptr.p_int[i];
        }
        s->ninitialized = s->ridx.ptr.p_int[s->m];
        
        /*
         * Allocate memory and move elements to Vals/Idx.
         * Initially, elements are sorted by rows, but unsorted within row.
         * After initial insertion we sort elements within row.
         */
        ae_vector_set_length(&temp, s->m, _state);
        for(i=0; i<=s->m-1; i++)
        {
            temp.ptr.p_int[i] = 0;
        }
        rvectorsetlengthatleast(&s->vals, nonne, _state);
        ivectorsetlengthatleast(&s->idx, nonne, _state);
        for(i=0; i<=k-1; i++)
        {
            if( tidx.ptr.p_int[2*i]>=0 )
            {
                s->vals.ptr.p_double[s->ridx.ptr.p_int[tidx.ptr.p_int[2*i]]+temp.ptr.p_int[tidx.ptr.p_int[2*i]]] = tvals.ptr.p_double[i];
                s->idx.ptr.p_int[s->ridx.ptr.p_int[tidx.ptr.p_int[2*i]]+temp.ptr.p_int[tidx.ptr.p_int[2*i]]] = tidx.ptr.p_int[2*i+1];
                temp.ptr.p_int[tidx.ptr.p_int[2*i]] = temp.ptr.p_int[tidx.ptr.p_int[2*i]]+1;
            }
        }
        for(i=0; i<=s->m-1; i++)
        {
            tagsortmiddleir(&s->idx, &s->vals, s->ridx.ptr.p_int[i], s->ridx.ptr.p_int[i+1]-s->ridx.ptr.p_int[i], _state);
        }
        
        /*
         * Initialization 'S.UIdx' and 'S.DIdx'
         */
        sparseinitduidx(s, _state);
        ae_frame_leave(_state);
        return;
    }
    if( s->matrixtype==1 )
    {
        
        /*
         * Already CRS
         */
        ae_frame_leave(_state);
        return;
    }
    if( s->matrixtype==2 )
    {
        ae_assert(s->m==s->n, "SparseConvertToCRS: non-square SKS matrices are not supported", _state);
        
        /*
         * From SKS to CRS.
         *
         * First, create local copy of the SKS matrix (Vals,
         * Idx, RIdx are stored; DIdx/UIdx for some time are
         * left in the SparseMatrix structure).
         */
        s->matrixtype = 1;
        ae_swap_vectors(&s->vals, &tvals);
        ae_swap_vectors(&s->idx, &tidx);
        ae_swap_vectors(&s->ridx, &tridx);
        
        /*
         * Fill RIdx by number of elements per row:
         * RIdx[I+1] stores number of elements in I-th row.
         *
         * Convert RIdx from row sizes to row offsets.
         * Set NInitialized
         */
        ivectorsetlengthatleast(&s->ridx, m+1, _state);
        s->ridx.ptr.p_int[0] = 0;
        for(i=1; i<=m; i++)
        {
            s->ridx.ptr.p_int[i] = 1;
        }
        nonne = 0;
        for(i=0; i<=m-1; i++)
        {
            s->ridx.ptr.p_int[i+1] = s->didx.ptr.p_int[i]+s->ridx.ptr.p_int[i+1];
            for(j=i-s->uidx.ptr.p_int[i]; j<=i-1; j++)
            {
                s->ridx.ptr.p_int[j+1] = s->ridx.ptr.p_int[j+1]+1;
            }
            nonne = nonne+s->didx.ptr.p_int[i]+1+s->uidx.ptr.p_int[i];
        }
        for(i=0; i<=s->m-1; i++)
        {
            s->ridx.ptr.p_int[i+1] = s->ridx.ptr.p_int[i+1]+s->ridx.ptr.p_int[i];
        }
        s->ninitialized = s->ridx.ptr.p_int[s->m];
        
        /*
         * Allocate memory and move elements to Vals/Idx.
         * Initially, elements are sorted by rows, and are sorted within row too.
         * No additional post-sorting is required.
         */
        ae_vector_set_length(&temp, m, _state);
        for(i=0; i<=m-1; i++)
        {
            temp.ptr.p_int[i] = 0;
        }
        rvectorsetlengthatleast(&s->vals, nonne, _state);
        ivectorsetlengthatleast(&s->idx, nonne, _state);
        for(i=0; i<=m-1; i++)
        {
            
            /*
             * copy subdiagonal and diagonal parts of I-th block
             */
            offs0 = tridx.ptr.p_int[i];
            offs1 = s->ridx.ptr.p_int[i]+temp.ptr.p_int[i];
            k = s->didx.ptr.p_int[i]+1;
            for(j=0; j<=k-1; j++)
            {
                s->vals.ptr.p_double[offs1+j] = tvals.ptr.p_double[offs0+j];
                s->idx.ptr.p_int[offs1+j] = i-s->didx.ptr.p_int[i]+j;
            }
            temp.ptr.p_int[i] = temp.ptr.p_int[i]+s->didx.ptr.p_int[i]+1;
            
            /*
             * Copy superdiagonal part of I-th block
             */
            offs0 = tridx.ptr.p_int[i]+s->didx.ptr.p_int[i]+1;
            k = s->uidx.ptr.p_int[i];
            for(j=0; j<=k-1; j++)
            {
                offs1 = s->ridx.ptr.p_int[i-k+j]+temp.ptr.p_int[i-k+j];
                s->vals.ptr.p_double[offs1] = tvals.ptr.p_double[offs0+j];
                s->idx.ptr.p_int[offs1] = i;
                temp.ptr.p_int[i-k+j] = temp.ptr.p_int[i-k+j]+1;
            }
        }
        
        /*
         * Initialization 'S.UIdx' and 'S.DIdx'
         */
        sparseinitduidx(s, _state);
        ae_frame_leave(_state);
        return;
    }
    ae_assert(ae_false, "SparseConvertToCRS: invalid matrix type", _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This  function  performs  out-of-place  conversion  to  CRS format.  S0 is
copied to S1 and converted on-the-fly.

INPUT PARAMETERS
    S0          -   sparse matrix in any format.

OUTPUT PARAMETERS
    S1          -   sparse matrix in CRS format.
    
NOTE: if S0 is stored as CRS, it is just copied without conversion.

NOTE: this function de-allocates memory occupied by S1 before starting CRS
      conversion. If you perform a lot of repeated CRS conversions, it may
      lead to memory fragmentation. In this case we recommend you  to  use
      SparseCopyToCRSBuf() function which re-uses memory in S1 as much  as
      possible.

  -- ALGLIB PROJECT --
     Copyright 20.07.2012 by Bochkanov Sergey
*************************************************************************/
void sparsecopytocrs(const sparsematrix* s0,
     sparsematrix* s1,
     ae_state *_state)
{

    _sparsematrix_clear(s1);

    ae_assert((s0->matrixtype==0||s0->matrixtype==1)||s0->matrixtype==2, "SparseCopyToCRS: invalid matrix type", _state);
    sparsecopytocrsbuf(s0, s1, _state);
}


/*************************************************************************
This  function  performs  out-of-place  conversion  to  CRS format.  S0 is
copied to S1 and converted on-the-fly. Memory allocated in S1 is reused to
maximum extent possible.

INPUT PARAMETERS
    S0          -   sparse matrix in any format.
    S1          -   matrix which may contain some pre-allocated memory, or
                    can be just uninitialized structure.

OUTPUT PARAMETERS
    S1          -   sparse matrix in CRS format.
    
NOTE: if S0 is stored as CRS, it is just copied without conversion.

  -- ALGLIB PROJECT --
     Copyright 20.07.2012 by Bochkanov Sergey
*************************************************************************/
void sparsecopytocrsbuf(const sparsematrix* s0,
     sparsematrix* s1,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector temp;
    ae_int_t nonne;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t offs0;
    ae_int_t offs1;
    ae_int_t m;

    ae_frame_make(_state, &_frame_block);
    memset(&temp, 0, sizeof(temp));
    ae_vector_init(&temp, 0, DT_INT, _state, ae_true);

    ae_assert((s0->matrixtype==0||s0->matrixtype==1)||s0->matrixtype==2, "SparseCopyToCRSBuf: invalid matrix type", _state);
    m = s0->m;
    if( s0->matrixtype==0 )
    {
        
        /*
         * Convert from hash-table to CRS
         * Done like ConvertToCRS function
         */
        s1->matrixtype = 1;
        s1->m = s0->m;
        s1->n = s0->n;
        s1->nfree = s0->nfree;
        nonne = 0;
        k = s0->tablesize;
        ivectorsetlengthatleast(&s1->ridx, s1->m+1, _state);
        for(i=0; i<=s1->m; i++)
        {
            s1->ridx.ptr.p_int[i] = 0;
        }
        ae_vector_set_length(&temp, s1->m, _state);
        for(i=0; i<=s1->m-1; i++)
        {
            temp.ptr.p_int[i] = 0;
        }
        
        /*
         * Number of elements per row
         */
        for(i=0; i<=k-1; i++)
        {
            if( s0->idx.ptr.p_int[2*i]>=0 )
            {
                s1->ridx.ptr.p_int[s0->idx.ptr.p_int[2*i]+1] = s1->ridx.ptr.p_int[s0->idx.ptr.p_int[2*i]+1]+1;
                nonne = nonne+1;
            }
        }
        
        /*
         * Fill RIdx (offsets of rows)
         */
        for(i=0; i<=s1->m-1; i++)
        {
            s1->ridx.ptr.p_int[i+1] = s1->ridx.ptr.p_int[i+1]+s1->ridx.ptr.p_int[i];
        }
        
        /*
         * Allocate memory
         */
        rvectorsetlengthatleast(&s1->vals, nonne, _state);
        ivectorsetlengthatleast(&s1->idx, nonne, _state);
        for(i=0; i<=k-1; i++)
        {
            if( s0->idx.ptr.p_int[2*i]>=0 )
            {
                s1->vals.ptr.p_double[s1->ridx.ptr.p_int[s0->idx.ptr.p_int[2*i]]+temp.ptr.p_int[s0->idx.ptr.p_int[2*i]]] = s0->vals.ptr.p_double[i];
                s1->idx.ptr.p_int[s1->ridx.ptr.p_int[s0->idx.ptr.p_int[2*i]]+temp.ptr.p_int[s0->idx.ptr.p_int[2*i]]] = s0->idx.ptr.p_int[2*i+1];
                temp.ptr.p_int[s0->idx.ptr.p_int[2*i]] = temp.ptr.p_int[s0->idx.ptr.p_int[2*i]]+1;
            }
        }
        
        /*
         * Set NInitialized
         */
        s1->ninitialized = s1->ridx.ptr.p_int[s1->m];
        
        /*
         * Sorting of elements
         */
        for(i=0; i<=s1->m-1; i++)
        {
            tagsortmiddleir(&s1->idx, &s1->vals, s1->ridx.ptr.p_int[i], s1->ridx.ptr.p_int[i+1]-s1->ridx.ptr.p_int[i], _state);
        }
        
        /*
         * Initialization 'S.UIdx' and 'S.DIdx'
         */
        sparseinitduidx(s1, _state);
        ae_frame_leave(_state);
        return;
    }
    if( s0->matrixtype==1 )
    {
        
        /*
         * Already CRS, just copy
         */
        sparsecopybuf(s0, s1, _state);
        ae_frame_leave(_state);
        return;
    }
    if( s0->matrixtype==2 )
    {
        ae_assert(s0->m==s0->n, "SparseCopyToCRS: non-square SKS matrices are not supported", _state);
        
        /*
         * From SKS to CRS.
         */
        s1->m = s0->m;
        s1->n = s0->n;
        s1->matrixtype = 1;
        
        /*
         * Fill RIdx by number of elements per row:
         * RIdx[I+1] stores number of elements in I-th row.
         *
         * Convert RIdx from row sizes to row offsets.
         * Set NInitialized
         */
        ivectorsetlengthatleast(&s1->ridx, m+1, _state);
        s1->ridx.ptr.p_int[0] = 0;
        for(i=1; i<=m; i++)
        {
            s1->ridx.ptr.p_int[i] = 1;
        }
        nonne = 0;
        for(i=0; i<=m-1; i++)
        {
            s1->ridx.ptr.p_int[i+1] = s0->didx.ptr.p_int[i]+s1->ridx.ptr.p_int[i+1];
            for(j=i-s0->uidx.ptr.p_int[i]; j<=i-1; j++)
            {
                s1->ridx.ptr.p_int[j+1] = s1->ridx.ptr.p_int[j+1]+1;
            }
            nonne = nonne+s0->didx.ptr.p_int[i]+1+s0->uidx.ptr.p_int[i];
        }
        for(i=0; i<=m-1; i++)
        {
            s1->ridx.ptr.p_int[i+1] = s1->ridx.ptr.p_int[i+1]+s1->ridx.ptr.p_int[i];
        }
        s1->ninitialized = s1->ridx.ptr.p_int[m];
        
        /*
         * Allocate memory and move elements to Vals/Idx.
         * Initially, elements are sorted by rows, and are sorted within row too.
         * No additional post-sorting is required.
         */
        ae_vector_set_length(&temp, m, _state);
        for(i=0; i<=m-1; i++)
        {
            temp.ptr.p_int[i] = 0;
        }
        rvectorsetlengthatleast(&s1->vals, nonne, _state);
        ivectorsetlengthatleast(&s1->idx, nonne, _state);
        for(i=0; i<=m-1; i++)
        {
            
            /*
             * copy subdiagonal and diagonal parts of I-th block
             */
            offs0 = s0->ridx.ptr.p_int[i];
            offs1 = s1->ridx.ptr.p_int[i]+temp.ptr.p_int[i];
            k = s0->didx.ptr.p_int[i]+1;
            for(j=0; j<=k-1; j++)
            {
                s1->vals.ptr.p_double[offs1+j] = s0->vals.ptr.p_double[offs0+j];
                s1->idx.ptr.p_int[offs1+j] = i-s0->didx.ptr.p_int[i]+j;
            }
            temp.ptr.p_int[i] = temp.ptr.p_int[i]+s0->didx.ptr.p_int[i]+1;
            
            /*
             * Copy superdiagonal part of I-th block
             */
            offs0 = s0->ridx.ptr.p_int[i]+s0->didx.ptr.p_int[i]+1;
            k = s0->uidx.ptr.p_int[i];
            for(j=0; j<=k-1; j++)
            {
                offs1 = s1->ridx.ptr.p_int[i-k+j]+temp.ptr.p_int[i-k+j];
                s1->vals.ptr.p_double[offs1] = s0->vals.ptr.p_double[offs0+j];
                s1->idx.ptr.p_int[offs1] = i;
                temp.ptr.p_int[i-k+j] = temp.ptr.p_int[i-k+j]+1;
            }
        }
        
        /*
         * Initialization 'S.UIdx' and 'S.DIdx'
         */
        sparseinitduidx(s1, _state);
        ae_frame_leave(_state);
        return;
    }
    ae_assert(ae_false, "SparseCopyToCRSBuf: unexpected matrix type", _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function performs in-place conversion to SKS format.

INPUT PARAMETERS
    S           -   sparse matrix in any format.

OUTPUT PARAMETERS
    S           -   sparse matrix in SKS format.

NOTE: this  function  has   no  effect  when  called with matrix which  is
      already in SKS mode.

NOTE: in-place conversion involves allocation of temporary arrays. If  you
      perform a lot of repeated in- place  conversions,  it  may  lead  to
      memory fragmentation. Consider using out-of-place SparseCopyToSKSBuf()
      function in this case.
    
  -- ALGLIB PROJECT --
     Copyright 15.01.2014 by Bochkanov Sergey
*************************************************************************/
void sparseconverttosks(sparsematrix* s, ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector tridx;
    ae_vector tdidx;
    ae_vector tuidx;
    ae_vector tvals;
    ae_int_t n;
    ae_int_t t0;
    ae_int_t t1;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double v;

    ae_frame_make(_state, &_frame_block);
    memset(&tridx, 0, sizeof(tridx));
    memset(&tdidx, 0, sizeof(tdidx));
    memset(&tuidx, 0, sizeof(tuidx));
    memset(&tvals, 0, sizeof(tvals));
    ae_vector_init(&tridx, 0, DT_INT, _state, ae_true);
    ae_vector_init(&tdidx, 0, DT_INT, _state, ae_true);
    ae_vector_init(&tuidx, 0, DT_INT, _state, ae_true);
    ae_vector_init(&tvals, 0, DT_REAL, _state, ae_true);

    ae_assert((s->matrixtype==0||s->matrixtype==1)||s->matrixtype==2, "SparseConvertToSKS: invalid matrix type", _state);
    ae_assert(s->m==s->n, "SparseConvertToSKS: rectangular matrices are not supported", _state);
    n = s->n;
    if( s->matrixtype==2 )
    {
        
        /*
         * Already in SKS mode
         */
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Generate internal copy of SKS matrix
     */
    ivectorsetlengthatleast(&tdidx, n+1, _state);
    ivectorsetlengthatleast(&tuidx, n+1, _state);
    for(i=0; i<=n; i++)
    {
        tdidx.ptr.p_int[i] = 0;
        tuidx.ptr.p_int[i] = 0;
    }
    t0 = 0;
    t1 = 0;
    while(sparseenumerate(s, &t0, &t1, &i, &j, &v, _state))
    {
        if( j<i )
        {
            tdidx.ptr.p_int[i] = ae_maxint(tdidx.ptr.p_int[i], i-j, _state);
        }
        else
        {
            tuidx.ptr.p_int[j] = ae_maxint(tuidx.ptr.p_int[j], j-i, _state);
        }
    }
    ivectorsetlengthatleast(&tridx, n+1, _state);
    tridx.ptr.p_int[0] = 0;
    for(i=1; i<=n; i++)
    {
        tridx.ptr.p_int[i] = tridx.ptr.p_int[i-1]+tdidx.ptr.p_int[i-1]+1+tuidx.ptr.p_int[i-1];
    }
    rvectorsetlengthatleast(&tvals, tridx.ptr.p_int[n], _state);
    k = tridx.ptr.p_int[n];
    for(i=0; i<=k-1; i++)
    {
        tvals.ptr.p_double[i] = 0.0;
    }
    t0 = 0;
    t1 = 0;
    while(sparseenumerate(s, &t0, &t1, &i, &j, &v, _state))
    {
        if( j<=i )
        {
            tvals.ptr.p_double[tridx.ptr.p_int[i]+tdidx.ptr.p_int[i]-(i-j)] = v;
        }
        else
        {
            tvals.ptr.p_double[tridx.ptr.p_int[j+1]-(j-i)] = v;
        }
    }
    for(i=0; i<=n-1; i++)
    {
        tdidx.ptr.p_int[n] = ae_maxint(tdidx.ptr.p_int[n], tdidx.ptr.p_int[i], _state);
        tuidx.ptr.p_int[n] = ae_maxint(tuidx.ptr.p_int[n], tuidx.ptr.p_int[i], _state);
    }
    s->matrixtype = 2;
    s->ninitialized = 0;
    s->nfree = 0;
    s->m = n;
    s->n = n;
    ae_swap_vectors(&s->didx, &tdidx);
    ae_swap_vectors(&s->uidx, &tuidx);
    ae_swap_vectors(&s->ridx, &tridx);
    ae_swap_vectors(&s->vals, &tvals);
    ae_frame_leave(_state);
}


/*************************************************************************
This  function  performs  out-of-place  conversion  to SKS storage format.
S0 is copied to S1 and converted on-the-fly.

INPUT PARAMETERS
    S0          -   sparse matrix in any format.

OUTPUT PARAMETERS
    S1          -   sparse matrix in SKS format.

NOTE: if S0 is stored as SKS, it is just copied without conversion.

NOTE: this function de-allocates memory  occupied  by  S1 before  starting
      conversion. If you perform a  lot  of  repeated  conversions, it may
      lead to memory fragmentation. In this case we recommend you  to  use
      SparseCopyToSKSBuf() function which re-uses memory in S1 as much  as
      possible.

  -- ALGLIB PROJECT --
     Copyright 20.07.2012 by Bochkanov Sergey
*************************************************************************/
void sparsecopytosks(const sparsematrix* s0,
     sparsematrix* s1,
     ae_state *_state)
{

    _sparsematrix_clear(s1);

    ae_assert((s0->matrixtype==0||s0->matrixtype==1)||s0->matrixtype==2, "SparseCopyToSKS: invalid matrix type", _state);
    sparsecopytosksbuf(s0, s1, _state);
}


/*************************************************************************
This  function  performs  out-of-place  conversion  to SKS format.  S0  is
copied to S1 and converted on-the-fly. Memory  allocated  in S1 is  reused
to maximum extent possible.

INPUT PARAMETERS
    S0          -   sparse matrix in any format.

OUTPUT PARAMETERS
    S1          -   sparse matrix in SKS format.

NOTE: if S0 is stored as SKS, it is just copied without conversion.

  -- ALGLIB PROJECT --
     Copyright 20.07.2012 by Bochkanov Sergey
*************************************************************************/
void sparsecopytosksbuf(const sparsematrix* s0,
     sparsematrix* s1,
     ae_state *_state)
{
    double v;
    ae_int_t n;
    ae_int_t t0;
    ae_int_t t1;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;


    ae_assert((s0->matrixtype==0||s0->matrixtype==1)||s0->matrixtype==2, "SparseCopyToSKSBuf: invalid matrix type", _state);
    ae_assert(s0->m==s0->n, "SparseCopyToSKSBuf: rectangular matrices are not supported", _state);
    n = s0->n;
    if( s0->matrixtype==2 )
    {
        
        /*
         * Already SKS, just copy
         */
        sparsecopybuf(s0, s1, _state);
        return;
    }
    
    /*
     * Generate copy of matrix in the SKS format
     */
    ivectorsetlengthatleast(&s1->didx, n+1, _state);
    ivectorsetlengthatleast(&s1->uidx, n+1, _state);
    for(i=0; i<=n; i++)
    {
        s1->didx.ptr.p_int[i] = 0;
        s1->uidx.ptr.p_int[i] = 0;
    }
    t0 = 0;
    t1 = 0;
    while(sparseenumerate(s0, &t0, &t1, &i, &j, &v, _state))
    {
        if( j<i )
        {
            s1->didx.ptr.p_int[i] = ae_maxint(s1->didx.ptr.p_int[i], i-j, _state);
        }
        else
        {
            s1->uidx.ptr.p_int[j] = ae_maxint(s1->uidx.ptr.p_int[j], j-i, _state);
        }
    }
    ivectorsetlengthatleast(&s1->ridx, n+1, _state);
    s1->ridx.ptr.p_int[0] = 0;
    for(i=1; i<=n; i++)
    {
        s1->ridx.ptr.p_int[i] = s1->ridx.ptr.p_int[i-1]+s1->didx.ptr.p_int[i-1]+1+s1->uidx.ptr.p_int[i-1];
    }
    rvectorsetlengthatleast(&s1->vals, s1->ridx.ptr.p_int[n], _state);
    k = s1->ridx.ptr.p_int[n];
    for(i=0; i<=k-1; i++)
    {
        s1->vals.ptr.p_double[i] = 0.0;
    }
    t0 = 0;
    t1 = 0;
    while(sparseenumerate(s0, &t0, &t1, &i, &j, &v, _state))
    {
        if( j<=i )
        {
            s1->vals.ptr.p_double[s1->ridx.ptr.p_int[i]+s1->didx.ptr.p_int[i]-(i-j)] = v;
        }
        else
        {
            s1->vals.ptr.p_double[s1->ridx.ptr.p_int[j+1]-(j-i)] = v;
        }
    }
    for(i=0; i<=n-1; i++)
    {
        s1->didx.ptr.p_int[n] = ae_maxint(s1->didx.ptr.p_int[n], s1->didx.ptr.p_int[i], _state);
        s1->uidx.ptr.p_int[n] = ae_maxint(s1->uidx.ptr.p_int[n], s1->uidx.ptr.p_int[i], _state);
    }
    s1->matrixtype = 2;
    s1->ninitialized = 0;
    s1->nfree = 0;
    s1->m = n;
    s1->n = n;
}


/*************************************************************************
This non-accessible to user function performs  in-place  creation  of  CRS
matrix. It is expected that:
* S.M and S.N are initialized
* S.RIdx, S.Idx and S.Vals are loaded with values in CRS  format  used  by
  ALGLIB, with elements of S.Idx/S.Vals  possibly  being  unsorted  within
  each row (this constructor function may post-sort matrix,  assuming that
  it is sorted by rows).
  
Only 5 fields should be set by caller. Other fields will be  rewritten  by
this constructor function.

This function performs integrity check on user-specified values, with  the
only exception being Vals[] array:
* it does not require values to be non-zero
* it does not check for elements of Vals[] being finite IEEE-754 values

INPUT PARAMETERS
    S   -   sparse matrix with corresponding fields set by caller

OUTPUT PARAMETERS
    S   -   sparse matrix in CRS format.

  -- ALGLIB PROJECT --
     Copyright 20.08.2016 by Bochkanov Sergey
*************************************************************************/
void sparsecreatecrsinplace(sparsematrix* s, ae_state *_state)
{
    ae_int_t m;
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j0;
    ae_int_t j1;


    m = s->m;
    n = s->n;
    
    /*
     * Quick exit for M=0 or N=0
     */
    ae_assert(s->m>=0, "SparseCreateCRSInplace: integrity check failed", _state);
    ae_assert(s->n>=0, "SparseCreateCRSInplace: integrity check failed", _state);
    if( m==0||n==0 )
    {
        s->matrixtype = 1;
        s->ninitialized = 0;
        ivectorsetlengthatleast(&s->ridx, s->m+1, _state);
        ivectorsetlengthatleast(&s->didx, s->m, _state);
        ivectorsetlengthatleast(&s->uidx, s->m, _state);
        for(i=0; i<=s->m-1; i++)
        {
            s->ridx.ptr.p_int[i] = 0;
            s->uidx.ptr.p_int[i] = 0;
            s->didx.ptr.p_int[i] = 0;
        }
        s->ridx.ptr.p_int[s->m] = 0;
        return;
    }
    
    /*
     * Perform integrity check
     */
    ae_assert(s->m>0, "SparseCreateCRSInplace: integrity check failed", _state);
    ae_assert(s->n>0, "SparseCreateCRSInplace: integrity check failed", _state);
    ae_assert(s->ridx.cnt>=m+1, "SparseCreateCRSInplace: integrity check failed", _state);
    for(i=0; i<=m-1; i++)
    {
        ae_assert(s->ridx.ptr.p_int[i]>=0&&s->ridx.ptr.p_int[i]<=s->ridx.ptr.p_int[i+1], "SparseCreateCRSInplace: integrity check failed", _state);
    }
    ae_assert(s->ridx.ptr.p_int[m]<=s->idx.cnt, "SparseCreateCRSInplace: integrity check failed", _state);
    ae_assert(s->ridx.ptr.p_int[m]<=s->vals.cnt, "SparseCreateCRSInplace: integrity check failed", _state);
    for(i=0; i<=m-1; i++)
    {
        j0 = s->ridx.ptr.p_int[i];
        j1 = s->ridx.ptr.p_int[i+1]-1;
        for(j=j0; j<=j1; j++)
        {
            ae_assert(s->idx.ptr.p_int[j]>=0&&s->idx.ptr.p_int[j]<n, "SparseCreateCRSInplace: integrity check failed", _state);
        }
    }
    
    /*
     * Initialize
     */
    s->matrixtype = 1;
    s->ninitialized = s->ridx.ptr.p_int[m];
    for(i=0; i<=m-1; i++)
    {
        j0 = s->ridx.ptr.p_int[i];
        j1 = s->ridx.ptr.p_int[i+1];
        for(j=j0; j<=j1-2; j++)
        {
            if( s->idx.ptr.p_int[j]>s->idx.ptr.p_int[j+1] )
            {
                
                /*
                 * Unsorted sequence, needs sorting
                 */
                tagsortmiddleir(&s->idx, &s->vals, j0, j1-j0, _state);
                break;
            }
        }
    }
    sparseinitduidx(s, _state);
}


/*************************************************************************
This function returns type of the matrix storage format.

INPUT PARAMETERS:
    S           -   sparse matrix.

RESULT:
    sparse storage format used by matrix:
        0   -   Hash-table
        1   -   CRS (compressed row storage)
        2   -   SKS (skyline)

NOTE: future  versions  of  ALGLIB  may  include additional sparse storage
      formats.

    
  -- ALGLIB PROJECT --
     Copyright 20.07.2012 by Bochkanov Sergey
*************************************************************************/
ae_int_t sparsegetmatrixtype(const sparsematrix* s, ae_state *_state)
{
    ae_int_t result;


    ae_assert((((s->matrixtype==0||s->matrixtype==1)||s->matrixtype==2)||s->matrixtype==-10081)||s->matrixtype==-10082, "SparseGetMatrixType: invalid matrix type", _state);
    result = s->matrixtype;
    return result;
}


/*************************************************************************
This function checks matrix storage format and returns True when matrix is
stored using Hash table representation.

INPUT PARAMETERS:
    S   -   sparse matrix.

RESULT:
    True if matrix type is Hash table
    False if matrix type is not Hash table 
    
  -- ALGLIB PROJECT --
     Copyright 20.07.2012 by Bochkanov Sergey
*************************************************************************/
ae_bool sparseishash(const sparsematrix* s, ae_state *_state)
{
    ae_bool result;


    ae_assert((((s->matrixtype==0||s->matrixtype==1)||s->matrixtype==2)||s->matrixtype==-10081)||s->matrixtype==-10082, "SparseIsHash: invalid matrix type", _state);
    result = s->matrixtype==0;
    return result;
}


/*************************************************************************
This function checks matrix storage format and returns True when matrix is
stored using CRS representation.

INPUT PARAMETERS:
    S   -   sparse matrix.

RESULT:
    True if matrix type is CRS
    False if matrix type is not CRS
    
  -- ALGLIB PROJECT --
     Copyright 20.07.2012 by Bochkanov Sergey
*************************************************************************/
ae_bool sparseiscrs(const sparsematrix* s, ae_state *_state)
{
    ae_bool result;


    ae_assert((((s->matrixtype==0||s->matrixtype==1)||s->matrixtype==2)||s->matrixtype==-10081)||s->matrixtype==-10082, "SparseIsCRS: invalid matrix type", _state);
    result = s->matrixtype==1;
    return result;
}


/*************************************************************************
This function checks matrix storage format and returns True when matrix is
stored using SKS representation.

INPUT PARAMETERS:
    S   -   sparse matrix.

RESULT:
    True if matrix type is SKS
    False if matrix type is not SKS
    
  -- ALGLIB PROJECT --
     Copyright 20.07.2012 by Bochkanov Sergey
*************************************************************************/
ae_bool sparseissks(const sparsematrix* s, ae_state *_state)
{
    ae_bool result;


    ae_assert((((s->matrixtype==0||s->matrixtype==1)||s->matrixtype==2)||s->matrixtype==-10081)||s->matrixtype==-10082, "SparseIsSKS: invalid matrix type", _state);
    result = s->matrixtype==2;
    return result;
}


/*************************************************************************
The function frees all memory occupied by  sparse  matrix.  Sparse  matrix
structure becomes unusable after this call.

OUTPUT PARAMETERS
    S   -   sparse matrix to delete
    
  -- ALGLIB PROJECT --
     Copyright 24.07.2012 by Bochkanov Sergey
*************************************************************************/
void sparsefree(sparsematrix* s, ae_state *_state)
{

    _sparsematrix_clear(s);

    s->matrixtype = -1;
    s->m = 0;
    s->n = 0;
    s->nfree = 0;
    s->ninitialized = 0;
    s->tablesize = 0;
}


/*************************************************************************
The function returns number of rows of a sparse matrix.

RESULT: number of rows of a sparse matrix.
    
  -- ALGLIB PROJECT --
     Copyright 23.08.2012 by Bochkanov Sergey
*************************************************************************/
ae_int_t sparsegetnrows(const sparsematrix* s, ae_state *_state)
{
    ae_int_t result;


    result = s->m;
    return result;
}


/*************************************************************************
The function returns number of columns of a sparse matrix.

RESULT: number of columns of a sparse matrix.
    
  -- ALGLIB PROJECT --
     Copyright 23.08.2012 by Bochkanov Sergey
*************************************************************************/
ae_int_t sparsegetncols(const sparsematrix* s, ae_state *_state)
{
    ae_int_t result;


    result = s->n;
    return result;
}


/*************************************************************************
The function returns number of strictly upper triangular non-zero elements
in  the  matrix.  It  counts  SYMBOLICALLY non-zero elements, i.e. entries
in the sparse matrix data structure. If some element  has  zero  numerical
value, it is still counted.

This function has different cost for different types of matrices:
* for hash-based matrices it involves complete pass over entire hash-table
  with O(NNZ) cost, where NNZ is number of non-zero elements
* for CRS and SKS matrix types cost of counting is O(N) (N - matrix size).

RESULT: number of non-zero elements strictly above main diagonal
    
  -- ALGLIB PROJECT --
     Copyright 12.02.2014 by Bochkanov Sergey
*************************************************************************/
ae_int_t sparsegetuppercount(const sparsematrix* s, ae_state *_state)
{
    ae_int_t sz;
    ae_int_t i0;
    ae_int_t i;
    ae_int_t result;


    result = -1;
    if( s->matrixtype==0 )
    {
        
        /*
         * Hash-table matrix
         */
        result = 0;
        sz = s->tablesize;
        for(i0=0; i0<=sz-1; i0++)
        {
            i = s->idx.ptr.p_int[2*i0];
            if( i>=0&&s->idx.ptr.p_int[2*i0+1]>i )
            {
                result = result+1;
            }
        }
        return result;
    }
    if( s->matrixtype==1 )
    {
        
        /*
         * CRS matrix
         */
        ae_assert(s->ninitialized==s->ridx.ptr.p_int[s->m], "SparseGetUpperCount: some rows/elements of the CRS matrix were not initialized (you must initialize everything you promised to SparseCreateCRS)", _state);
        result = 0;
        sz = s->m;
        for(i=0; i<=sz-1; i++)
        {
            result = result+(s->ridx.ptr.p_int[i+1]-s->uidx.ptr.p_int[i]);
        }
        return result;
    }
    if( s->matrixtype==2 )
    {
        
        /*
         * SKS matrix
         */
        ae_assert(s->m==s->n, "SparseGetUpperCount: non-square SKS matrices are not supported", _state);
        result = 0;
        sz = s->m;
        for(i=0; i<=sz-1; i++)
        {
            result = result+s->uidx.ptr.p_int[i];
        }
        return result;
    }
    ae_assert(ae_false, "SparseGetUpperCount: internal error", _state);
    return result;
}


/*************************************************************************
The function returns number of strictly lower triangular non-zero elements
in  the  matrix.  It  counts  SYMBOLICALLY non-zero elements, i.e. entries
in the sparse matrix data structure. If some element  has  zero  numerical
value, it is still counted.

This function has different cost for different types of matrices:
* for hash-based matrices it involves complete pass over entire hash-table
  with O(NNZ) cost, where NNZ is number of non-zero elements
* for CRS and SKS matrix types cost of counting is O(N) (N - matrix size).

RESULT: number of non-zero elements strictly below main diagonal
    
  -- ALGLIB PROJECT --
     Copyright 12.02.2014 by Bochkanov Sergey
*************************************************************************/
ae_int_t sparsegetlowercount(const sparsematrix* s, ae_state *_state)
{
    ae_int_t sz;
    ae_int_t i0;
    ae_int_t i;
    ae_int_t result;


    result = -1;
    if( s->matrixtype==0 )
    {
        
        /*
         * Hash-table matrix
         */
        result = 0;
        sz = s->tablesize;
        for(i0=0; i0<=sz-1; i0++)
        {
            i = s->idx.ptr.p_int[2*i0];
            if( i>=0&&s->idx.ptr.p_int[2*i0+1]<i )
            {
                result = result+1;
            }
        }
        return result;
    }
    if( s->matrixtype==1 )
    {
        
        /*
         * CRS matrix
         */
        ae_assert(s->ninitialized==s->ridx.ptr.p_int[s->m], "SparseGetUpperCount: some rows/elements of the CRS matrix were not initialized (you must initialize everything you promised to SparseCreateCRS)", _state);
        result = 0;
        sz = s->m;
        for(i=0; i<=sz-1; i++)
        {
            result = result+(s->didx.ptr.p_int[i]-s->ridx.ptr.p_int[i]);
        }
        return result;
    }
    if( s->matrixtype==2 )
    {
        
        /*
         * SKS matrix
         */
        ae_assert(s->m==s->n, "SparseGetUpperCount: non-square SKS matrices are not supported", _state);
        result = 0;
        sz = s->m;
        for(i=0; i<=sz-1; i++)
        {
            result = result+s->didx.ptr.p_int[i];
        }
        return result;
    }
    ae_assert(ae_false, "SparseGetUpperCount: internal error", _state);
    return result;
}


/*************************************************************************
This function performs an in-place matrix conditioning scaling  such  that

    A = R*Z*C

where A is an original matrix, R and C are diagonal scaling  matrices, and
Z is a scaled matrix. Z replaces A, R and C are returned as 1D arrays.

INPUT PARAMETERS
    S           -   sparse M*N matrix in CRS format.
    SclType     -   scaling type:
                    * 0     for automatically chosen scaling
                    * 1     for equilibration scaling
    ScaleRows   -   if False, rows are not scaled (R=identity)
    ScaleCols   -   if False, cols are not scaled (C=identity)
    ColsFirst   -   scale columns first. If False, rows are  scaled  prior
                    to scaling columns. Ignored for ScaleCols=False.
    
OUTPUT PARAMETERS
    R           -   array[M], row scales, R[i]>0
    C           -   array[N], col scales, C[i]>0
    
NOTE: this function throws exception when called  for  a  non-CRS  matrix.
      You must convert your matrix with SparseConvertToCRS()  before using
      this function.

NOTE: this  function  works  with  general  (nonsymmetric)  matrices.  See
      sparsesymmscale() for a symmetric version. See sparsescalebuf()  for
      a version which reuses space already present in output arrays R/C.

NOTE: if both ScaleRows=False and ScaleCols=False, this  function  returns
      an identity scaling.

NOTE: R[] and C[] are guaranteed to be strictly positive. When the  matrix
      has zero rows/cols, corresponding elements of R/C are set to 1.

  -- ALGLIB PROJECT --
     Copyright 12.11.2023 by Bochkanov Sergey
*************************************************************************/
void sparsescale(sparsematrix* s,
     ae_int_t scltype,
     ae_bool scalerows,
     ae_bool scalecols,
     ae_bool colsfirst,
     /* Real    */ ae_vector* r,
     /* Real    */ ae_vector* c,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t n;
    ae_int_t m;
    ae_int_t axis;
    ae_vector tmp0;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t jj;
    double v;

    ae_frame_make(_state, &_frame_block);
    memset(&tmp0, 0, sizeof(tmp0));
    ae_vector_clear(r);
    ae_vector_clear(c);
    ae_vector_init(&tmp0, 0, DT_REAL, _state, ae_true);

    ae_assert(s->matrixtype==1, "SparseScale: incorrect matrix type (convert your matrix to CRS)", _state);
    ae_assert(scltype==0||scltype==1, "SparseScale: incorrect matrix type (convert your matrix to CRS)", _state);
    
    /*
     * Initialization and quick exit
     */
    n = s->n;
    m = s->m;
    if( scltype==0 )
    {
        scltype = 1;
    }
    rsetallocv(m, 1.0, r, _state);
    rsetallocv(n, 1.0, c, _state);
    if( !scalerows&&!scalecols )
    {
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Equilibration scaling
     */
    if( scltype==1 )
    {
        for(axis=0; axis<=1; axis++)
        {
            if( (colsfirst&&axis==0)||(!colsfirst&&axis==1) )
            {
                
                /*
                 * Scale columns
                 */
                if( scalecols )
                {
                    rallocv(n, &tmp0, _state);
                    rsetv(n, 0.0, c, _state);
                    for(i=0; i<=m-1; i++)
                    {
                        j0 = s->ridx.ptr.p_int[i];
                        j1 = s->ridx.ptr.p_int[i+1]-1;
                        for(jj=j0; jj<=j1; jj++)
                        {
                            j = s->idx.ptr.p_int[jj];
                            v = s->vals.ptr.p_double[jj];
                            c->ptr.p_double[j] = ae_maxreal(c->ptr.p_double[j], ae_fabs(v, _state), _state);
                        }
                    }
                    for(j=0; j<=n-1; j++)
                    {
                        if( ae_fp_eq(c->ptr.p_double[j],(double)(0)) )
                        {
                            c->ptr.p_double[j] = (double)(1);
                        }
                        tmp0.ptr.p_double[j] = (double)1/c->ptr.p_double[j];
                    }
                    for(i=0; i<=m-1; i++)
                    {
                        j0 = s->ridx.ptr.p_int[i];
                        j1 = s->ridx.ptr.p_int[i+1]-1;
                        for(jj=j0; jj<=j1; jj++)
                        {
                            s->vals.ptr.p_double[jj] = tmp0.ptr.p_double[s->idx.ptr.p_int[jj]]*s->vals.ptr.p_double[jj];
                        }
                    }
                }
            }
            else
            {
                
                /*
                 * Scale rows
                 */
                if( scalerows )
                {
                    for(i=0; i<=m-1; i++)
                    {
                        j0 = s->ridx.ptr.p_int[i];
                        j1 = s->ridx.ptr.p_int[i+1]-1;
                        v = (double)(0);
                        for(jj=j0; jj<=j1; jj++)
                        {
                            v = ae_maxreal(v, ae_fabs(s->vals.ptr.p_double[jj], _state), _state);
                        }
                        if( ae_fp_eq(v,(double)(0)) )
                        {
                            v = (double)(1);
                        }
                        r->ptr.p_double[i] = v;
                        v = (double)1/v;
                        for(jj=j0; jj<=j1; jj++)
                        {
                            s->vals.ptr.p_double[jj] = v*s->vals.ptr.p_double[jj];
                        }
                    }
                }
            }
        }
        ae_frame_leave(_state);
        return;
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Serializer: allocation.

INTERNAL-ONLY FUNCTION, SUPPORTS ONLY CRS MATRICES

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void sparsealloc(ae_serializer* s,
     const sparsematrix* a,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t nused;


    ae_assert((a->matrixtype==0||a->matrixtype==1)||a->matrixtype==2, "SparseAlloc: only CRS/SKS matrices are supported", _state);
    
    /*
     * Header
     */
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    
    /*
     * Alloc other parameters
     */
    if( a->matrixtype==0 )
    {
        
        /*
         * Alloc Hash
         */
        nused = 0;
        for(i=0; i<=a->tablesize-1; i++)
        {
            if( a->idx.ptr.p_int[2*i+0]>=0 )
            {
                nused = nused+1;
            }
        }
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        for(i=0; i<=a->tablesize-1; i++)
        {
            if( a->idx.ptr.p_int[2*i+0]>=0 )
            {
                ae_serializer_alloc_entry(s);
                ae_serializer_alloc_entry(s);
                ae_serializer_alloc_entry(s);
            }
        }
    }
    if( a->matrixtype==1 )
    {
        
        /*
         * Alloc CRS
         */
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        allocintegerarray(s, &a->ridx, a->m+1, _state);
        allocintegerarray(s, &a->idx, a->ridx.ptr.p_int[a->m], _state);
        allocrealarray(s, &a->vals, a->ridx.ptr.p_int[a->m], _state);
    }
    if( a->matrixtype==2 )
    {
        
        /*
         * Alloc SKS
         */
        ae_assert(a->m==a->n, "SparseAlloc: rectangular SKS serialization is not supported", _state);
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        allocintegerarray(s, &a->ridx, a->m+1, _state);
        allocintegerarray(s, &a->didx, a->n+1, _state);
        allocintegerarray(s, &a->uidx, a->n+1, _state);
        allocrealarray(s, &a->vals, a->ridx.ptr.p_int[a->m], _state);
    }
    
    /*
     * End of stream
     */
    ae_serializer_alloc_entry(s);
}


/*************************************************************************
Serializer: serialization

INTERNAL-ONLY FUNCTION, SUPPORTS ONLY CRS MATRICES

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void sparseserialize(ae_serializer* s,
     const sparsematrix* a,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t nused;


    ae_assert((a->matrixtype==0||a->matrixtype==1)||a->matrixtype==2, "SparseSerialize: only CRS/SKS matrices are supported", _state);
    
    /*
     * Header
     */
    ae_serializer_serialize_int(s, getsparsematrixserializationcode(_state), _state);
    ae_serializer_serialize_int(s, a->matrixtype, _state);
    ae_serializer_serialize_int(s, 0, _state);
    
    /*
     * Serialize other parameters
     */
    if( a->matrixtype==0 )
    {
        
        /*
         * Serialize Hash
         */
        nused = 0;
        for(i=0; i<=a->tablesize-1; i++)
        {
            if( a->idx.ptr.p_int[2*i+0]>=0 )
            {
                nused = nused+1;
            }
        }
        ae_serializer_serialize_int(s, a->m, _state);
        ae_serializer_serialize_int(s, a->n, _state);
        ae_serializer_serialize_int(s, nused, _state);
        for(i=0; i<=a->tablesize-1; i++)
        {
            if( a->idx.ptr.p_int[2*i+0]>=0 )
            {
                ae_serializer_serialize_int(s, a->idx.ptr.p_int[2*i+0], _state);
                ae_serializer_serialize_int(s, a->idx.ptr.p_int[2*i+1], _state);
                ae_serializer_serialize_double(s, a->vals.ptr.p_double[i], _state);
            }
        }
    }
    if( a->matrixtype==1 )
    {
        
        /*
         * Serialize CRS
         */
        ae_serializer_serialize_int(s, a->m, _state);
        ae_serializer_serialize_int(s, a->n, _state);
        ae_serializer_serialize_int(s, a->ninitialized, _state);
        serializeintegerarray(s, &a->ridx, a->m+1, _state);
        serializeintegerarray(s, &a->idx, a->ridx.ptr.p_int[a->m], _state);
        serializerealarray(s, &a->vals, a->ridx.ptr.p_int[a->m], _state);
    }
    if( a->matrixtype==2 )
    {
        
        /*
         * Serialize SKS
         */
        ae_assert(a->m==a->n, "SparseSerialize: rectangular SKS serialization is not supported", _state);
        ae_serializer_serialize_int(s, a->m, _state);
        ae_serializer_serialize_int(s, a->n, _state);
        serializeintegerarray(s, &a->ridx, a->m+1, _state);
        serializeintegerarray(s, &a->didx, a->n+1, _state);
        serializeintegerarray(s, &a->uidx, a->n+1, _state);
        serializerealarray(s, &a->vals, a->ridx.ptr.p_int[a->m], _state);
    }
    
    /*
     * End of stream
     */
    ae_serializer_serialize_int(s, 117, _state);
}


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void sparseunserialize(ae_serializer* s,
     sparsematrix* a,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t i0;
    ae_int_t i1;
    ae_int_t m;
    ae_int_t n;
    ae_int_t nused;
    ae_int_t k;
    double v;

    _sparsematrix_clear(a);

    
    /*
     * Check stream header: scode, matrix type, version type
     */
    ae_serializer_unserialize_int(s, &k, _state);
    ae_assert(k==getsparsematrixserializationcode(_state), "SparseUnserialize: stream header corrupted", _state);
    ae_serializer_unserialize_int(s, &a->matrixtype, _state);
    ae_assert((a->matrixtype==0||a->matrixtype==1)||a->matrixtype==2, "SparseUnserialize: unexpected matrix type", _state);
    ae_serializer_unserialize_int(s, &k, _state);
    ae_assert(k==0, "SparseUnserialize: stream header corrupted", _state);
    
    /*
     * Unserialize other parameters
     */
    if( a->matrixtype==0 )
    {
        
        /*
         * Unerialize Hash
         */
        ae_serializer_unserialize_int(s, &m, _state);
        ae_serializer_unserialize_int(s, &n, _state);
        ae_serializer_unserialize_int(s, &nused, _state);
        sparsecreate(m, n, nused, a, _state);
        for(i=0; i<=nused-1; i++)
        {
            ae_serializer_unserialize_int(s, &i0, _state);
            ae_serializer_unserialize_int(s, &i1, _state);
            ae_serializer_unserialize_double(s, &v, _state);
            sparseset(a, i0, i1, v, _state);
        }
    }
    if( a->matrixtype==1 )
    {
        
        /*
         * Unserialize CRS
         */
        ae_serializer_unserialize_int(s, &a->m, _state);
        ae_serializer_unserialize_int(s, &a->n, _state);
        ae_serializer_unserialize_int(s, &a->ninitialized, _state);
        unserializeintegerarray(s, &a->ridx, _state);
        unserializeintegerarray(s, &a->idx, _state);
        unserializerealarray(s, &a->vals, _state);
        sparseinitduidx(a, _state);
    }
    if( a->matrixtype==2 )
    {
        
        /*
         * Unserialize SKS
         */
        ae_serializer_unserialize_int(s, &a->m, _state);
        ae_serializer_unserialize_int(s, &a->n, _state);
        ae_assert(a->m==a->n, "SparseUnserialize: rectangular SKS unserialization is not supported", _state);
        unserializeintegerarray(s, &a->ridx, _state);
        unserializeintegerarray(s, &a->didx, _state);
        unserializeintegerarray(s, &a->uidx, _state);
        unserializerealarray(s, &a->vals, _state);
    }
    
    /*
     * End of stream
     */
    ae_serializer_unserialize_int(s, &k, _state);
    ae_assert(k==117, "SparseMatrixUnserialize: end-of-stream marker not found", _state);
}


/*************************************************************************
This is hash function.

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
static ae_int_t sparse_hash(ae_int_t i,
     ae_int_t j,
     ae_int_t tabsize,
     ae_state *_state)
{
    ae_frame _frame_block;
    hqrndstate r;
    ae_int_t result;

    ae_frame_make(_state, &_frame_block);
    memset(&r, 0, sizeof(r));
    _hqrndstate_init(&r, _state, ae_true);

    hqrndseed(i, j, &r, _state);
    result = hqrnduniformi(&r, tabsize, _state);
    ae_frame_leave(_state);
    return result;
}


void _sparsematrix_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    sparsematrix *p = (sparsematrix*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->vals, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->idx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->ridx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->didx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->uidx, 0, DT_INT, _state, make_automatic);
}


void _sparsematrix_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    sparsematrix       *dst = (sparsematrix*)_dst;
    const sparsematrix *src = (const sparsematrix*)_src;
    ae_vector_init_copy(&dst->vals, &src->vals, _state, make_automatic);
    ae_vector_init_copy(&dst->idx, &src->idx, _state, make_automatic);
    ae_vector_init_copy(&dst->ridx, &src->ridx, _state, make_automatic);
    ae_vector_init_copy(&dst->didx, &src->didx, _state, make_automatic);
    ae_vector_init_copy(&dst->uidx, &src->uidx, _state, make_automatic);
    dst->matrixtype = src->matrixtype;
    dst->m = src->m;
    dst->n = src->n;
    dst->nfree = src->nfree;
    dst->ninitialized = src->ninitialized;
    dst->tablesize = src->tablesize;
}


void _sparsematrix_clear(void* _p)
{
    sparsematrix *p = (sparsematrix*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->vals);
    ae_vector_clear(&p->idx);
    ae_vector_clear(&p->ridx);
    ae_vector_clear(&p->didx);
    ae_vector_clear(&p->uidx);
}


void _sparsematrix_destroy(void* _p)
{
    sparsematrix *p = (sparsematrix*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->vals);
    ae_vector_destroy(&p->idx);
    ae_vector_destroy(&p->ridx);
    ae_vector_destroy(&p->didx);
    ae_vector_destroy(&p->uidx);
}


void _sparsebuffers_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    sparsebuffers *p = (sparsebuffers*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->d, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->u, 0, DT_INT, _state, make_automatic);
    _sparsematrix_init(&p->s, _state, make_automatic);
}


void _sparsebuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    sparsebuffers       *dst = (sparsebuffers*)_dst;
    const sparsebuffers *src = (const sparsebuffers*)_src;
    ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic);
    ae_vector_init_copy(&dst->u, &src->u, _state, make_automatic);
    _sparsematrix_init_copy(&dst->s, &src->s, _state, make_automatic);
}


void _sparsebuffers_clear(void* _p)
{
    sparsebuffers *p = (sparsebuffers*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->d);
    ae_vector_clear(&p->u);
    _sparsematrix_clear(&p->s);
}


void _sparsebuffers_destroy(void* _p)
{
    sparsebuffers *p = (sparsebuffers*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->d);
    ae_vector_destroy(&p->u);
    _sparsematrix_destroy(&p->s);
}


/*$ End $*/
