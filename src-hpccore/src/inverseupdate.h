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

#ifndef _inverseupdate_h
#define _inverseupdate_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Inverse matrix update by the Sherman-Morrison formula

The algorithm updates matrix A^-1 when adding a number to an element
of matrix A.

Input parameters:
    InvA    -   inverse of matrix A.
                Array whose indexes range within [0..N-1, 0..N-1].
    N       -   size of matrix A.
    UpdRow  -   row where the element to be updated is stored.
    UpdColumn - column where the element to be updated is stored.
    UpdVal  -   a number to be added to the element.


Output parameters:
    InvA    -   inverse of modified matrix A.

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
void rmatrixinvupdatesimple(/* Real    */ ae_matrix* inva,
     ae_int_t n,
     ae_int_t updrow,
     ae_int_t updcolumn,
     double updval,
     ae_state *_state);


/*************************************************************************
Inverse matrix update by the Sherman-Morrison formula

The algorithm updates matrix A^-1 when adding a vector to a row
of matrix A.

Input parameters:
    InvA    -   inverse of matrix A.
                Array whose indexes range within [0..N-1, 0..N-1].
    N       -   size of matrix A.
    UpdRow  -   the row of A whose vector V was added.
                0 <= Row <= N-1
    V       -   the vector to be added to a row.
                Array whose index ranges within [0..N-1].

Output parameters:
    InvA    -   inverse of modified matrix A.

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
void rmatrixinvupdaterow(/* Real    */ ae_matrix* inva,
     ae_int_t n,
     ae_int_t updrow,
     /* Real    */ const ae_vector* v,
     ae_state *_state);


/*************************************************************************
Inverse matrix update by the Sherman-Morrison formula

The algorithm updates matrix A^-1 when adding a vector to a column
of matrix A.

Input parameters:
    InvA        -   inverse of matrix A.
                    Array whose indexes range within [0..N-1, 0..N-1].
    N           -   size of matrix A.
    UpdColumn   -   the column of A whose vector U was added.
                    0 <= UpdColumn <= N-1
    U           -   the vector to be added to a column.
                    Array whose index ranges within [0..N-1].

Output parameters:
    InvA        -   inverse of modified matrix A.

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
void rmatrixinvupdatecolumn(/* Real    */ ae_matrix* inva,
     ae_int_t n,
     ae_int_t updcolumn,
     /* Real    */ const ae_vector* u,
     ae_state *_state);


/*************************************************************************
Inverse matrix update by the Sherman-Morrison formula

The algorithm computes the inverse of matrix A+u*v' by using the given matrix
A^-1 and the vectors u and v.

Input parameters:
    InvA    -   inverse of matrix A.
                Array whose indexes range within [0..N-1, 0..N-1].
    N       -   size of matrix A.
    U       -   the vector modifying the matrix.
                Array whose index ranges within [0..N-1].
    V       -   the vector modifying the matrix.
                Array whose index ranges within [0..N-1].

Output parameters:
    InvA - inverse of matrix A + u*v'.

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
void rmatrixinvupdateuv(/* Real    */ ae_matrix* inva,
     ae_int_t n,
     /* Real    */ const ae_vector* u,
     /* Real    */ const ae_vector* v,
     ae_state *_state);


/*$ End $*/
#endif

