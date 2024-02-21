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

#ifndef _apstruct_h
#define _apstruct_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"


/*$ Declarations $*/


/*************************************************************************
This structure is used to store set of N possible integers, in [0,N) range.
The structure needs O(N) memory, independently from the actual set size.

This structure allows external code to use following fields:
* N - maximum set size
* NStored - number of elements currently in the set
* Items - first NStored elements are UNSORTED items
* LocationOf - array[N] that allows quick access by key. If item I is present
  in the set, LocationOf[I]>=0 and stores position in Items[]  of  element
  I, i.e. Items[LocationOf[I]]=I.
  If item I is not present, LocationOf[I]<0.
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t nstored;
    ae_vector items;
    ae_vector locationof;
    ae_int_t iteridx;
} niset;


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
void nisinitemptyslow(ae_int_t n, niset* sa, ae_state *_state);


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
void niscopy(const niset* ssrc, niset* sdst, ae_state *_state);


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
void nisaddelement(niset* sa, ae_int_t k, ae_state *_state);


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
void nissubtract1(niset* sa, const niset* src, ae_state *_state);


/*************************************************************************
Clears set

INPUT PARAMETERS
    SA          -   set to be cleared
    

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
void nisclear(niset* sa, ae_state *_state);


/*************************************************************************
Counts set elements

INPUT PARAMETERS
    SA          -   set
    
RESULT
    number of elements in SA

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
ae_int_t niscount(const niset* sa, ae_state *_state);


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
ae_bool nisequal(const niset* s0, const niset* s1, ae_state *_state);


/*************************************************************************
Prepares iteration over set

INPUT PARAMETERS
    SA          -   set
    
OUTPUT PARAMETERS
    SA          -   SA ready for repeated calls of nisEnumerate()

  -- ALGLIB PROJECT --
     Copyright 05.10.2020 by Bochkanov Sergey.
*************************************************************************/
void nisstartenumeration(niset* sa, ae_state *_state);


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
ae_bool nisenumerate(niset* sa, ae_int_t* i, ae_state *_state);
void _niset_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _niset_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _niset_clear(void* _p);
void _niset_destroy(void* _p);


/*$ End $*/
#endif

