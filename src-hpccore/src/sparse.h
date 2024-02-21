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

#ifndef _sparse_h
#define _sparse_h

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


/*$ Declarations $*/


/*************************************************************************
Sparse matrix structure.

You should use ALGLIB functions to work with sparse matrix. Never  try  to
access its fields directly!

NOTES ON THE SPARSE STORAGE FORMATS

Sparse matrices can be stored using several formats:
* Hash-Table representation
* Compressed Row Storage (CRS)
* Skyline matrix storage (SKS)

Each of the formats has benefits and drawbacks:
* Hash-table is good for dynamic operations (insertion of new elements),
  but does not support linear algebra operations
* CRS is good for operations like matrix-vector or matrix-matrix products,
  but its initialization is less convenient - you have to tell row   sizes 
  at the initialization, and you have to fill  matrix  only  row  by  row,
  from left to right.
* SKS is a special format which is used to store triangular  factors  from
  Cholesky factorization. It does not support  dynamic  modification,  and
  support for linear algebra operations is very limited.

Tables below outline information about these two formats:

    OPERATIONS WITH MATRIX      HASH        CRS         SKS
    creation                    +           +           +
    SparseGet                   +           +           +
    SparseExists                +           +           +
    SparseRewriteExisting       +           +           +
    SparseSet                   +           +           +
    SparseAdd                   +
    SparseGetRow                            +           +
    SparseGetCompressedRow                  +           +
    sparse-dense linear algebra             +           +

*************************************************************************/
typedef struct
{
    ae_vector vals;
    ae_vector idx;
    ae_vector ridx;
    ae_vector didx;
    ae_vector uidx;
    ae_int_t matrixtype;
    ae_int_t m;
    ae_int_t n;
    ae_int_t nfree;
    ae_int_t ninitialized;
    ae_int_t tablesize;
} sparsematrix;


/*************************************************************************
Temporary buffers for sparse matrix operations.

You should pass an instance of this structure to factorization  functions.
It allows to reuse memory during repeated sparse  factorizations.  You  do
not have to call some initialization function - simply passing an instance
to factorization function is enough.

*************************************************************************/
typedef struct
{
    ae_vector d;
    ae_vector u;
    sparsematrix s;
} sparsebuffers;


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


/*************************************************************************
This function efficiently swaps contents of S0 and S1.

  -- ALGLIB PROJECT --
     Copyright 16.01.2014 by Bochkanov Sergey
*************************************************************************/
void sparseswap(sparsematrix* s0, sparsematrix* s1, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


/*************************************************************************
This procedure resizes Hash-Table matrix. It can be called when you  have
deleted too many elements from the matrix, and you want to  free unneeded
memory.

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
void sparseresizematrix(sparsematrix* s, ae_state *_state);


/*************************************************************************
Procedure for initialization 'S.DIdx' and 'S.UIdx'


  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
void sparseinitduidx(sparsematrix* s, ae_state *_state);


/*************************************************************************
This function return average length of chain at hash-table.

  -- ALGLIB PROJECT --
     Copyright 14.10.2011 by Bochkanov Sergey
*************************************************************************/
double sparsegetaveragelengthofchain(const sparsematrix* s,
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
void sparsetransposesks(sparsematrix* s, ae_state *_state);


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
void sparsetransposecrs(sparsematrix* s, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
void sparseconvertto(sparsematrix* s0, ae_int_t fmt, ae_state *_state);


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
     ae_state *_state);


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
void sparseconverttohash(sparsematrix* s, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
void sparseconverttocrs(sparsematrix* s, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
void sparseconverttosks(sparsematrix* s, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
void sparsecreatecrsinplace(sparsematrix* s, ae_state *_state);


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
ae_int_t sparsegetmatrixtype(const sparsematrix* s, ae_state *_state);


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
ae_bool sparseishash(const sparsematrix* s, ae_state *_state);


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
ae_bool sparseiscrs(const sparsematrix* s, ae_state *_state);


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
ae_bool sparseissks(const sparsematrix* s, ae_state *_state);


/*************************************************************************
The function frees all memory occupied by  sparse  matrix.  Sparse  matrix
structure becomes unusable after this call.

OUTPUT PARAMETERS
    S   -   sparse matrix to delete
    
  -- ALGLIB PROJECT --
     Copyright 24.07.2012 by Bochkanov Sergey
*************************************************************************/
void sparsefree(sparsematrix* s, ae_state *_state);


/*************************************************************************
The function returns number of rows of a sparse matrix.

RESULT: number of rows of a sparse matrix.
    
  -- ALGLIB PROJECT --
     Copyright 23.08.2012 by Bochkanov Sergey
*************************************************************************/
ae_int_t sparsegetnrows(const sparsematrix* s, ae_state *_state);


/*************************************************************************
The function returns number of columns of a sparse matrix.

RESULT: number of columns of a sparse matrix.
    
  -- ALGLIB PROJECT --
     Copyright 23.08.2012 by Bochkanov Sergey
*************************************************************************/
ae_int_t sparsegetncols(const sparsematrix* s, ae_state *_state);


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
ae_int_t sparsegetuppercount(const sparsematrix* s, ae_state *_state);


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
ae_int_t sparsegetlowercount(const sparsematrix* s, ae_state *_state);


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
     ae_state *_state);


/*************************************************************************
Serializer: allocation.

INTERNAL-ONLY FUNCTION, SUPPORTS ONLY CRS MATRICES

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void sparsealloc(ae_serializer* s,
     const sparsematrix* a,
     ae_state *_state);


/*************************************************************************
Serializer: serialization

INTERNAL-ONLY FUNCTION, SUPPORTS ONLY CRS MATRICES

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void sparseserialize(ae_serializer* s,
     const sparsematrix* a,
     ae_state *_state);


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void sparseunserialize(ae_serializer* s,
     sparsematrix* a,
     ae_state *_state);
void _sparsematrix_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sparsematrix_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sparsematrix_clear(void* _p);
void _sparsematrix_destroy(void* _p);
void _sparsebuffers_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sparsebuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sparsebuffers_clear(void* _p);
void _sparsebuffers_destroy(void* _p);


/*$ End $*/
#endif

