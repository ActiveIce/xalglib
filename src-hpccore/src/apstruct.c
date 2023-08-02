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
#include "apstruct.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Initializes n-set by empty structure.

IMPORTANT: this function need O(N) time for initialization. It is recommended
           to reduce its usage as much as possible, and use nisClear()
           where possible.

INPUT PARAMETERS
    N           -   possible set size
    
OUTPUT PARAMETERS
    SA          -   empty N-set

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
void nisinitemptyslow(ae_int_t n, niset* sa, ae_state *_state)
{


    sa->n = n;
    sa->nstored = 0;
    isetallocv(n, -999999999, &sa->locationof, _state);
    isetallocv(n, -999999999, &sa->items, _state);
}


/*************************************************************************
Copies n-set to properly initialized target set. The target set has to  be
properly initialized, and it can be non-empty. If  it  is  non-empty,  its
contents is quickly erased before copying.

The cost of this function is O(max(SrcSize,DstSize))

INPUT PARAMETERS
    SSrc        -   source N-set
    SDst        -   destination N-set (has same size as SSrc)
    
OUTPUT PARAMETERS
    SDst        -   copy of SSrc

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
void niscopy(const niset* ssrc, niset* sdst, ae_state *_state)
{
    ae_int_t ns;
    ae_int_t i;
    ae_int_t k;


    nisclear(sdst, _state);
    ns = ssrc->nstored;
    for(i=0; i<=ns-1; i++)
    {
        k = ssrc->items.ptr.p_int[i];
        sdst->items.ptr.p_int[i] = k;
        sdst->locationof.ptr.p_int[k] = i;
    }
    sdst->nstored = ns;
}


/*************************************************************************
Add K-th element to the set. The element may already exist in the set.

INPUT PARAMETERS
    SA          -   set
    K           -   element to add, 0<=K<N.
    
OUTPUT PARAMETERS
    SA          -   modified SA

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
void nisaddelement(niset* sa, ae_int_t k, ae_state *_state)
{
    ae_int_t ns;


    if( sa->locationof.ptr.p_int[k]>=0 )
    {
        return;
    }
    ns = sa->nstored;
    sa->locationof.ptr.p_int[k] = ns;
    sa->items.ptr.p_int[ns] = k;
    sa->nstored = ns+1;
}


/*************************************************************************
Subtracts K-th set from the source structure

INPUT PARAMETERS
    SA          -   set
    Src, K      -   source kn-set and set index K
    
OUTPUT PARAMETERS
    SA          -   modified SA

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
void nissubtract1(niset* sa, const niset* src, ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t loc;
    ae_int_t item;
    ae_int_t ns;
    ae_int_t ss;


    ns = sa->nstored;
    ss = src->nstored;
    if( ss<ns )
    {
        for(i=0; i<=ss-1; i++)
        {
            j = src->items.ptr.p_int[i];
            loc = sa->locationof.ptr.p_int[j];
            if( loc>=0 )
            {
                item = sa->items.ptr.p_int[ns-1];
                sa->items.ptr.p_int[loc] = item;
                sa->locationof.ptr.p_int[item] = loc;
                sa->locationof.ptr.p_int[j] = -1;
                ns = ns-1;
            }
        }
    }
    else
    {
        i = 0;
        while(i<ns)
        {
            j = sa->items.ptr.p_int[i];
            loc = src->locationof.ptr.p_int[j];
            if( loc>=0 )
            {
                item = sa->items.ptr.p_int[ns-1];
                sa->items.ptr.p_int[i] = item;
                sa->locationof.ptr.p_int[item] = i;
                sa->locationof.ptr.p_int[j] = -1;
                ns = ns-1;
            }
            else
            {
                i = i+1;
            }
        }
    }
    sa->nstored = ns;
}


/*************************************************************************
Clears set

INPUT PARAMETERS
    SA          -   set to be cleared
    

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
void nisclear(niset* sa, ae_state *_state)
{
    ae_int_t i;
    ae_int_t ns;


    ns = sa->nstored;
    for(i=0; i<=ns-1; i++)
    {
        sa->locationof.ptr.p_int[sa->items.ptr.p_int[i]] = -1;
    }
    sa->nstored = 0;
}


/*************************************************************************
Counts set elements

INPUT PARAMETERS
    SA          -   set
    
RESULT
    number of elements in SA

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
ae_int_t niscount(const niset* sa, ae_state *_state)
{
    ae_int_t result;


    result = sa->nstored;
    return result;
}


/*************************************************************************
Compare two sets, returns True for equal sets

INPUT PARAMETERS
    S0          -   set 0
    S1          -   set 1, must have same parameter N as set 0
    
RESULT
    True, if sets are equal

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
ae_bool nisequal(const niset* s0, const niset* s1, ae_state *_state)
{
    ae_int_t i;
    ae_int_t ns0;
    ae_int_t ns1;
    ae_bool result;


    result = ae_false;
    if( s0->n!=s1->n )
    {
        return result;
    }
    if( s0->nstored!=s1->nstored )
    {
        return result;
    }
    ns0 = s0->nstored;
    ns1 = s1->nstored;
    for(i=0; i<=ns0-1; i++)
    {
        if( s1->locationof.ptr.p_int[s0->items.ptr.p_int[i]]<0 )
        {
            return result;
        }
    }
    for(i=0; i<=ns1-1; i++)
    {
        if( s0->locationof.ptr.p_int[s1->items.ptr.p_int[i]]<0 )
        {
            return result;
        }
    }
    result = ae_true;
    return result;
}


/*************************************************************************
Prepares iteration over set

INPUT PARAMETERS
    SA          -   set
    
OUTPUT PARAMETERS
    SA          -   SA ready for repeated calls of nisEnumerate()

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
void nisstartenumeration(niset* sa, ae_state *_state)
{


    sa->iteridx = 0;
}


/*************************************************************************
Iterates over the set. Subsequent calls return True and set J to  new  set
item until iteration stops and False is returned.

INPUT PARAMETERS
    SA          -   n-set
    
OUTPUT PARAMETERS
    J           -   if:
                    * Result=True - index of element in the set
                    * Result=False - not set


  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
ae_bool nisenumerate(niset* sa, ae_int_t* i, ae_state *_state)
{
    ae_int_t k;
    ae_bool result;

    *i = 0;

    k = sa->iteridx;
    if( k>=sa->nstored )
    {
        result = ae_false;
        return result;
    }
    *i = sa->items.ptr.p_int[k];
    sa->iteridx = k+1;
    result = ae_true;
    return result;
}


void _niset_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    niset *p = (niset*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->items, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->locationof, 0, DT_INT, _state, make_automatic);
}


void _niset_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    niset       *dst = (niset*)_dst;
    const niset *src = (const niset*)_src;
    dst->n = src->n;
    dst->nstored = src->nstored;
    ae_vector_init_copy(&dst->items, &src->items, _state, make_automatic);
    ae_vector_init_copy(&dst->locationof, &src->locationof, _state, make_automatic);
    dst->iteridx = src->iteridx;
}


void _niset_clear(void* _p)
{
    niset *p = (niset*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->items);
    ae_vector_clear(&p->locationof);
}


void _niset_destroy(void* _p)
{
    niset *p = (niset*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->items);
    ae_vector_destroy(&p->locationof);
}


/*$ End $*/
