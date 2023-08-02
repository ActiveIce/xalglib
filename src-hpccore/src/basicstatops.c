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


#include <stdafx.h>
#include "basicstatops.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Internal tied ranking subroutine.

INPUT PARAMETERS:
    X       -   array to rank
    N       -   array size
    IsCentered- whether ranks are centered or not:
                * True      -   ranks are centered in such way that  their
                                sum is zero
                * False     -   ranks are not centered
    Buf     -   temporary buffers
    
NOTE: when IsCentered is True and all X[] are equal, this  function  fills
      X by zeros (exact zeros are used, not sum which is only approximately
      equal to zero).
*************************************************************************/
void rankx(/* Real    */ ae_vector* x,
     ae_int_t n,
     ae_bool iscentered,
     apbuffers* buf,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double tmp;
    double voffs;


    
    /*
     * Prepare
     */
    if( n<1 )
    {
        return;
    }
    if( n==1 )
    {
        x->ptr.p_double[0] = (double)(0);
        return;
    }
    if( buf->ra1.cnt<n )
    {
        ae_vector_set_length(&buf->ra1, n, _state);
    }
    if( buf->ia1.cnt<n )
    {
        ae_vector_set_length(&buf->ia1, n, _state);
    }
    for(i=0; i<=n-1; i++)
    {
        buf->ra1.ptr.p_double[i] = x->ptr.p_double[i];
        buf->ia1.ptr.p_int[i] = i;
    }
    tagsortfasti(&buf->ra1, &buf->ia1, &buf->ra2, &buf->ia2, n, _state);
    
    /*
     * Special test for all values being equal
     */
    if( ae_fp_eq(buf->ra1.ptr.p_double[0],buf->ra1.ptr.p_double[n-1]) )
    {
        if( iscentered )
        {
            tmp = 0.0;
        }
        else
        {
            tmp = (double)(n-1)/(double)2;
        }
        for(i=0; i<=n-1; i++)
        {
            x->ptr.p_double[i] = tmp;
        }
        return;
    }
    
    /*
     * compute tied ranks
     */
    i = 0;
    while(i<=n-1)
    {
        j = i+1;
        while(j<=n-1)
        {
            if( ae_fp_neq(buf->ra1.ptr.p_double[j],buf->ra1.ptr.p_double[i]) )
            {
                break;
            }
            j = j+1;
        }
        for(k=i; k<=j-1; k++)
        {
            buf->ra1.ptr.p_double[k] = (double)(i+j-1)/(double)2;
        }
        i = j;
    }
    
    /*
     * back to x
     */
    if( iscentered )
    {
        voffs = (double)(n-1)/(double)2;
    }
    else
    {
        voffs = 0.0;
    }
    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[buf->ia1.ptr.p_int[i]] = buf->ra1.ptr.p_double[i]-voffs;
    }
}


/*************************************************************************
Internal untied ranking subroutine.

INPUT PARAMETERS:
    X       -   array to rank
    N       -   array size
    Buf     -   temporary buffers

Returns untied ranks (in case of a tie ranks are resolved arbitrarily).
*************************************************************************/
void rankxuntied(/* Real    */ ae_vector* x,
     ae_int_t n,
     apbuffers* buf,
     ae_state *_state)
{
    ae_int_t i;


    
    /*
     * Prepare
     */
    if( n<1 )
    {
        return;
    }
    if( n==1 )
    {
        x->ptr.p_double[0] = (double)(0);
        return;
    }
    if( buf->ra1.cnt<n )
    {
        ae_vector_set_length(&buf->ra1, n, _state);
    }
    if( buf->ia1.cnt<n )
    {
        ae_vector_set_length(&buf->ia1, n, _state);
    }
    for(i=0; i<=n-1; i++)
    {
        buf->ra1.ptr.p_double[i] = x->ptr.p_double[i];
        buf->ia1.ptr.p_int[i] = i;
    }
    tagsortfasti(&buf->ra1, &buf->ia1, &buf->ra2, &buf->ia2, n, _state);
    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[buf->ia1.ptr.p_int[i]] = (double)(i);
    }
}


/*$ End $*/
