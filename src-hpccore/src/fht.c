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
#include "fht.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
1-dimensional Fast Hartley Transform.

Algorithm has O(N*logN) complexity for any N (composite or prime).

INPUT PARAMETERS
    A   -   array[0..N-1] - real function to be transformed
    N   -   problem size
    
OUTPUT PARAMETERS
    A   -   FHT of a input array, array[0..N-1],
            A_out[k] = sum(A_in[j]*(cos(2*pi*j*k/N)+sin(2*pi*j*k/N)), j=0..N-1)


  -- ALGLIB --
     Copyright 04.06.2009 by Bochkanov Sergey
*************************************************************************/
void fhtr1d(/* Real    */ ae_vector* a, ae_int_t n, ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_vector fa;

    ae_frame_make(_state, &_frame_block);
    memset(&fa, 0, sizeof(fa));
    ae_vector_init(&fa, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "FHTR1D: incorrect N!", _state);
    
    /*
     * Special case: N=1, FHT is just identity transform.
     * After this block we assume that N is strictly greater than 1.
     */
    if( n==1 )
    {
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Reduce FHt to real FFT
     */
    fftr1d(a, n, &fa, _state);
    for(i=0; i<=n-1; i++)
    {
        a->ptr.p_double[i] = fa.ptr.p_complex[i].x-fa.ptr.p_complex[i].y;
    }
    ae_frame_leave(_state);
}


/*************************************************************************
1-dimensional inverse FHT.

Algorithm has O(N*logN) complexity for any N (composite or prime).

INPUT PARAMETERS
    A   -   array[0..N-1] - complex array to be transformed
    N   -   problem size

OUTPUT PARAMETERS
    A   -   inverse FHT of a input array, array[0..N-1]


  -- ALGLIB --
     Copyright 29.05.2009 by Bochkanov Sergey
*************************************************************************/
void fhtr1dinv(/* Real    */ ae_vector* a, ae_int_t n, ae_state *_state)
{
    ae_int_t i;


    ae_assert(n>0, "FHTR1DInv: incorrect N!", _state);
    
    /*
     * Special case: N=1, iFHT is just identity transform.
     * After this block we assume that N is strictly greater than 1.
     */
    if( n==1 )
    {
        return;
    }
    
    /*
     * Inverse FHT can be expressed in terms of the FHT as
     *
     *     invfht(x) = fht(x)/N
     */
    fhtr1d(a, n, _state);
    for(i=0; i<=n-1; i++)
    {
        a->ptr.p_double[i] = a->ptr.p_double[i]/(double)n;
    }
}


/*$ End $*/
