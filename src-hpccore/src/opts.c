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
#include "opts.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Initialize test LP problem.

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemcreate(ae_int_t n,
     ae_bool hasknowntarget,
     double targetf,
     lptestproblem* p,
     ae_state *_state)
{

    _lptestproblem_clear(p);

    ae_assert(n>=1, "LPTestProblemCreate: N<1", _state);
    p->n = n;
    p->hasknowntarget = hasknowntarget;
    if( hasknowntarget )
    {
        p->targetf = targetf;
    }
    else
    {
        p->targetf = _state->v_nan;
    }
    ae_vector_set_length(&p->s, n, _state);
    rsetv(n, 1.0, &p->s, _state);
    ae_vector_set_length(&p->c, n, _state);
    rsetv(n, 0.0, &p->c, _state);
    ae_vector_set_length(&p->bndl, n, _state);
    rsetv(n, 0.0, &p->bndl, _state);
    ae_vector_set_length(&p->bndu, n, _state);
    rsetv(n, 0.0, &p->bndu, _state);
    p->m = 0;
    ae_vector_set_length(&p->al, 0, _state);
    ae_vector_set_length(&p->au, 0, _state);
}


/*************************************************************************
Query test problem info

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
ae_bool lptestproblemhasknowntarget(lptestproblem* p, ae_state *_state)
{
    ae_bool result;


    result = p->hasknowntarget;
    return result;
}


/*************************************************************************
Query test problem info

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
double lptestproblemgettargetf(lptestproblem* p, ae_state *_state)
{
    double result;


    result = p->targetf;
    return result;
}


/*************************************************************************
Query test problem info

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
ae_int_t lptestproblemgetn(lptestproblem* p, ae_state *_state)
{
    ae_int_t result;


    result = p->n;
    return result;
}


/*************************************************************************
Query test problem info

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
ae_int_t lptestproblemgetm(lptestproblem* p, ae_state *_state)
{
    ae_int_t result;


    result = p->m;
    return result;
}


/*************************************************************************
Set scale for test LP problem

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemsetscale(lptestproblem* p,
     /* Real    */ const ae_vector* s,
     ae_state *_state)
{


    rcopyv(p->n, s, &p->s, _state);
}


/*************************************************************************
Set cost for test LP problem

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemsetcost(lptestproblem* p,
     /* Real    */ const ae_vector* c,
     ae_state *_state)
{


    rcopyv(p->n, c, &p->c, _state);
}


/*************************************************************************
Set box constraints for test LP problem

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemsetbc(lptestproblem* p,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state)
{


    rcopyv(p->n, bndl, &p->bndl, _state);
    rcopyv(p->n, bndu, &p->bndu, _state);
}


/*************************************************************************
Set box constraints for test LP problem

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemsetlc2(lptestproblem* p,
     const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t m,
     ae_state *_state)
{


    if( m<=0 )
    {
        p->m = 0;
        return;
    }
    ae_assert(sparsegetnrows(a, _state)==m, "LPTestProblemSetLC2: rows(A)<>M", _state);
    p->m = m;
    sparsecopytocrs(a, &p->a, _state);
    ae_vector_set_length(&p->al, m, _state);
    ae_vector_set_length(&p->au, m, _state);
    rcopyv(m, al, &p->al, _state);
    rcopyv(m, au, &p->au, _state);
}


/*************************************************************************
Serializer: allocation

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemalloc(ae_serializer* s,
     const lptestproblem* p,
     ae_state *_state)
{


    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    allocrealarray(s, &p->s, p->n, _state);
    allocrealarray(s, &p->c, p->n, _state);
    allocrealarray(s, &p->bndl, p->n, _state);
    allocrealarray(s, &p->bndu, p->n, _state);
    ae_serializer_alloc_entry(s);
    if( p->m>0 )
    {
        sparsealloc(s, &p->a, _state);
        allocrealarray(s, &p->al, p->m, _state);
        allocrealarray(s, &p->au, p->m, _state);
    }
    ae_serializer_alloc_entry(s);
}


/*************************************************************************
Serializer: serialization

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemserialize(ae_serializer* s,
     const lptestproblem* p,
     ae_state *_state)
{


    ae_serializer_serialize_int(s, getlptestserializationcode(_state), _state);
    ae_serializer_serialize_int(s, 0, _state);
    ae_serializer_serialize_int(s, p->n, _state);
    ae_serializer_serialize_bool(s, p->hasknowntarget, _state);
    ae_serializer_serialize_double(s, p->targetf, _state);
    serializerealarray(s, &p->s, p->n, _state);
    serializerealarray(s, &p->c, p->n, _state);
    serializerealarray(s, &p->bndl, p->n, _state);
    serializerealarray(s, &p->bndu, p->n, _state);
    ae_serializer_serialize_int(s, p->m, _state);
    if( p->m>0 )
    {
        sparseserialize(s, &p->a, _state);
        serializerealarray(s, &p->al, p->m, _state);
        serializerealarray(s, &p->au, p->m, _state);
    }
    ae_serializer_serialize_int(s, 872, _state);
}


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemunserialize(ae_serializer* s,
     lptestproblem* p,
     ae_state *_state)
{
    ae_int_t k;

    _lptestproblem_clear(p);

    ae_serializer_unserialize_int(s, &k, _state);
    ae_assert(k==getlptestserializationcode(_state), "LPTestProblemUnserialize: stream header corrupted", _state);
    ae_serializer_unserialize_int(s, &k, _state);
    ae_assert(k==0, "LPTestProblemUnserialize: stream header corrupted", _state);
    ae_serializer_unserialize_int(s, &p->n, _state);
    ae_serializer_unserialize_bool(s, &p->hasknowntarget, _state);
    ae_serializer_unserialize_double(s, &p->targetf, _state);
    unserializerealarray(s, &p->s, _state);
    unserializerealarray(s, &p->c, _state);
    unserializerealarray(s, &p->bndl, _state);
    unserializerealarray(s, &p->bndu, _state);
    ae_serializer_unserialize_int(s, &p->m, _state);
    if( p->m>0 )
    {
        sparseunserialize(s, &p->a, _state);
        unserializerealarray(s, &p->al, _state);
        unserializerealarray(s, &p->au, _state);
    }
    ae_serializer_unserialize_int(s, &k, _state);
    ae_assert(k==872, "LPTestProblemUnserialize: end-of-stream marker not found", _state);
}


/*************************************************************************
This is internal function intended to  be  used  only  by  ALGLIB  itself.
Although for technical reasons it is made publicly available (and has  its
own manual entry), you should never call it.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void xdbgminlpcreatefromtestproblem(const lptestproblem* p,
     minlpstate* state,
     ae_state *_state)
{

    _minlpstate_clear(state);

    minlpcreate(p->n, state, _state);
    minlpsetscale(state, &p->s, _state);
    minlpsetcost(state, &p->c, _state);
    minlpsetbc(state, &p->bndl, &p->bndu, _state);
    minlpsetlc2(state, &p->a, &p->al, &p->au, p->m, _state);
}


void _lptestproblem_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    lptestproblem *p = (lptestproblem*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->c, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->a, _state, make_automatic);
    ae_vector_init(&p->al, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->au, 0, DT_REAL, _state, make_automatic);
}


void _lptestproblem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    lptestproblem       *dst = (lptestproblem*)_dst;
    const lptestproblem *src = (const lptestproblem*)_src;
    dst->n = src->n;
    dst->hasknowntarget = src->hasknowntarget;
    dst->targetf = src->targetf;
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_vector_init_copy(&dst->c, &src->c, _state, make_automatic);
    ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic);
    ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic);
    dst->m = src->m;
    _sparsematrix_init_copy(&dst->a, &src->a, _state, make_automatic);
    ae_vector_init_copy(&dst->al, &src->al, _state, make_automatic);
    ae_vector_init_copy(&dst->au, &src->au, _state, make_automatic);
}


void _lptestproblem_clear(void* _p)
{
    lptestproblem *p = (lptestproblem*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->c);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    _sparsematrix_clear(&p->a);
    ae_vector_clear(&p->al);
    ae_vector_clear(&p->au);
}


void _lptestproblem_destroy(void* _p)
{
    lptestproblem *p = (lptestproblem*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->c);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    _sparsematrix_destroy(&p->a);
    ae_vector_destroy(&p->al);
    ae_vector_destroy(&p->au);
}


/*$ End $*/
