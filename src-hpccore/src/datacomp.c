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
#include "datacomp.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
k-means++ clusterization.
Backward compatibility function, we recommend to use CLUSTERING subpackage
as better replacement.

  -- ALGLIB --
     Copyright 21.03.2009 by Bochkanov Sergey
*************************************************************************/
void kmeansgenerate(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nvars,
     ae_int_t k,
     ae_int_t restarts,
     ae_int_t* info,
     /* Real    */ ae_matrix* c,
     /* Integer */ ae_vector* xyc,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix dummy;
    ae_int_t itscnt;
    double e;
    kmeansbuffers buf;

    ae_frame_make(_state, &_frame_block);
    memset(&dummy, 0, sizeof(dummy));
    memset(&buf, 0, sizeof(buf));
    *info = 0;
    ae_matrix_clear(c);
    ae_vector_clear(xyc);
    ae_matrix_init(&dummy, 0, 0, DT_REAL, _state, ae_true);
    _kmeansbuffers_init(&buf, _state, ae_true);

    kmeansinitbuf(&buf, _state);
    kmeansgenerateinternal(xy, npoints, nvars, k, 0, 1, 0, restarts, ae_false, info, &itscnt, c, ae_true, &dummy, ae_false, xyc, &e, &buf, _state);
    ae_frame_leave(_state);
}


/*$ End $*/
