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
#include "optguardapi.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
This subroutine initializes "internal" OptGuard report,  i.e. one intended
for internal use by optimizers.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
void optguardinitinternal(optguardreport* rep,
     ae_int_t n,
     ae_int_t k,
     ae_state *_state)
{


    rep->nonc0suspected = ae_false;
    rep->nonc0test0positive = ae_false;
    rep->nonc0lipschitzc = (double)(0);
    rep->nonc0fidx = -1;
    rep->nonc1suspected = ae_false;
    rep->nonc1test0positive = ae_false;
    rep->nonc1test1positive = ae_false;
    rep->nonc1lipschitzc = (double)(0);
    rep->nonc1fidx = -1;
    rep->badgradsuspected = ae_false;
    rep->badgradfidx = -1;
    rep->badgradvidx = -1;
}


/*************************************************************************
This subroutine exports report to user-readable representation (all arrays
are forced to have exactly same size as needed; unused arrays are  set  to
zero length).

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
void optguardexportreport(const optguardreport* srcrep,
     ae_int_t n,
     ae_int_t k,
     ae_bool badgradhasxj,
     optguardreport* dstrep,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;


    dstrep->nonc0suspected = srcrep->nonc0suspected;
    dstrep->nonc0test0positive = srcrep->nonc0test0positive;
    if( srcrep->nonc0suspected )
    {
        dstrep->nonc0lipschitzc = srcrep->nonc0lipschitzc;
        dstrep->nonc0fidx = srcrep->nonc0fidx;
    }
    else
    {
        dstrep->nonc0lipschitzc = (double)(0);
        dstrep->nonc0fidx = -1;
    }
    dstrep->nonc1suspected = srcrep->nonc1suspected;
    dstrep->nonc1test0positive = srcrep->nonc1test0positive;
    dstrep->nonc1test1positive = srcrep->nonc1test1positive;
    if( srcrep->nonc1suspected )
    {
        dstrep->nonc1lipschitzc = srcrep->nonc1lipschitzc;
        dstrep->nonc1fidx = srcrep->nonc1fidx;
    }
    else
    {
        dstrep->nonc1lipschitzc = (double)(0);
        dstrep->nonc1fidx = -1;
    }
    dstrep->badgradsuspected = srcrep->badgradsuspected;
    if( srcrep->badgradsuspected )
    {
        dstrep->badgradfidx = srcrep->badgradfidx;
        dstrep->badgradvidx = srcrep->badgradvidx;
    }
    else
    {
        dstrep->badgradfidx = -1;
        dstrep->badgradvidx = -1;
    }
    if( badgradhasxj )
    {
        ae_vector_set_length(&dstrep->badgradxbase, n, _state);
        for(j=0; j<=n-1; j++)
        {
            dstrep->badgradxbase.ptr.p_double[j] = srcrep->badgradxbase.ptr.p_double[j];
        }
        ae_matrix_set_length(&dstrep->badgraduser, k, n, _state);
        ae_matrix_set_length(&dstrep->badgradnum, k, n, _state);
        for(i=0; i<=k-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                dstrep->badgraduser.ptr.pp_double[i][j] = srcrep->badgraduser.ptr.pp_double[i][j];
                dstrep->badgradnum.ptr.pp_double[i][j] = srcrep->badgradnum.ptr.pp_double[i][j];
            }
        }
    }
    else
    {
        ae_vector_set_length(&dstrep->badgradxbase, 0, _state);
        ae_matrix_set_length(&dstrep->badgraduser, 0, 0, _state);
        ae_matrix_set_length(&dstrep->badgradnum, 0, 0, _state);
    }
}


/*************************************************************************
This subroutine exports report to user-readable representation (all arrays
are forced to have exactly same size as needed; unused arrays are  set  to
zero length).

NOTE: we assume that SrcRep contains scaled X0[] and  D[],  i.e.  explicit
      variable scaling was applied. We need to rescale them during export,
      that's why we need S[] parameter.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
void smoothnessmonitorexportc1test0report(const optguardnonc1test0report* srcrep,
     /* Real    */ const ae_vector* s,
     optguardnonc1test0report* dstrep,
     ae_state *_state)
{
    ae_int_t i;


    dstrep->positive = srcrep->positive;
    if( srcrep->positive )
    {
        dstrep->stpidxa = srcrep->stpidxa;
        dstrep->stpidxb = srcrep->stpidxb;
        dstrep->inneriter = srcrep->inneriter;
        dstrep->outeriter = srcrep->outeriter;
        dstrep->fidx = srcrep->fidx;
        dstrep->cnt = srcrep->cnt;
        dstrep->n = srcrep->n;
        ae_vector_set_length(&dstrep->x0, srcrep->n, _state);
        ae_vector_set_length(&dstrep->d, srcrep->n, _state);
        for(i=0; i<=srcrep->n-1; i++)
        {
            dstrep->x0.ptr.p_double[i] = srcrep->x0.ptr.p_double[i]*s->ptr.p_double[i];
            dstrep->d.ptr.p_double[i] = srcrep->d.ptr.p_double[i]*s->ptr.p_double[i];
        }
        ae_vector_set_length(&dstrep->stp, srcrep->cnt, _state);
        ae_vector_set_length(&dstrep->f, srcrep->cnt, _state);
        for(i=0; i<=srcrep->cnt-1; i++)
        {
            dstrep->stp.ptr.p_double[i] = srcrep->stp.ptr.p_double[i];
            dstrep->f.ptr.p_double[i] = srcrep->f.ptr.p_double[i];
        }
    }
    else
    {
        dstrep->stpidxa = -1;
        dstrep->stpidxb = -1;
        dstrep->inneriter = -1;
        dstrep->outeriter = -1;
        dstrep->fidx = -1;
        dstrep->cnt = 0;
        dstrep->n = 0;
        ae_vector_set_length(&dstrep->x0, 0, _state);
        ae_vector_set_length(&dstrep->d, 0, _state);
        ae_vector_set_length(&dstrep->stp, 0, _state);
        ae_vector_set_length(&dstrep->f, 0, _state);
    }
}


/*************************************************************************
This subroutine exports report to user-readable representation (all arrays
are forced to have exactly same size as needed; unused arrays are  set  to
zero length).

NOTE: we assume that SrcRep contains scaled X0[], D[] and G[], i.e. explicit
      variable scaling was applied. We need to rescale them during export,
      that's why we need S[] parameter.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
void smoothnessmonitorexportc1test1report(const optguardnonc1test1report* srcrep,
     /* Real    */ const ae_vector* s,
     optguardnonc1test1report* dstrep,
     ae_state *_state)
{
    ae_int_t i;


    dstrep->positive = srcrep->positive;
    if( srcrep->positive )
    {
        ae_assert(srcrep->vidx>=0&&srcrep->vidx<srcrep->n, "SmoothnessMonitorExportC1Test1Report: integrity check failed", _state);
        dstrep->stpidxa = srcrep->stpidxa;
        dstrep->stpidxb = srcrep->stpidxb;
        dstrep->inneriter = srcrep->inneriter;
        dstrep->outeriter = srcrep->outeriter;
        dstrep->fidx = srcrep->fidx;
        dstrep->vidx = srcrep->vidx;
        dstrep->cnt = srcrep->cnt;
        dstrep->n = srcrep->n;
        ae_vector_set_length(&dstrep->x0, srcrep->n, _state);
        ae_vector_set_length(&dstrep->d, srcrep->n, _state);
        for(i=0; i<=srcrep->n-1; i++)
        {
            dstrep->x0.ptr.p_double[i] = srcrep->x0.ptr.p_double[i]*s->ptr.p_double[i];
            dstrep->d.ptr.p_double[i] = srcrep->d.ptr.p_double[i]*s->ptr.p_double[i];
        }
        ae_vector_set_length(&dstrep->stp, srcrep->cnt, _state);
        ae_vector_set_length(&dstrep->g, srcrep->cnt, _state);
        for(i=0; i<=srcrep->cnt-1; i++)
        {
            dstrep->stp.ptr.p_double[i] = srcrep->stp.ptr.p_double[i];
            dstrep->g.ptr.p_double[i] = srcrep->g.ptr.p_double[i]/s->ptr.p_double[srcrep->vidx];
        }
    }
    else
    {
        dstrep->stpidxa = -1;
        dstrep->stpidxb = -1;
        dstrep->inneriter = -1;
        dstrep->outeriter = -1;
        dstrep->fidx = -1;
        dstrep->vidx = -1;
        dstrep->cnt = 0;
        dstrep->n = 0;
        ae_vector_set_length(&dstrep->x0, 0, _state);
        ae_vector_set_length(&dstrep->d, 0, _state);
        ae_vector_set_length(&dstrep->stp, 0, _state);
        ae_vector_set_length(&dstrep->g, 0, _state);
    }
}


/*************************************************************************
Returns True when all flags are clear. Intended for easy  coding  of  unit
tests.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
ae_bool optguardallclear(const optguardreport* rep, ae_state *_state)
{
    ae_bool result;


    result = !((rep->badgradsuspected||rep->nonc0suspected)||rep->nonc1suspected);
    return result;
}


void _optguardreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    optguardreport *p = (optguardreport*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->badgradxbase, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->badgraduser, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->badgradnum, 0, 0, DT_REAL, _state, make_automatic);
}


void _optguardreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    optguardreport       *dst = (optguardreport*)_dst;
    const optguardreport *src = (const optguardreport*)_src;
    dst->nonc0suspected = src->nonc0suspected;
    dst->nonc0test0positive = src->nonc0test0positive;
    dst->nonc0fidx = src->nonc0fidx;
    dst->nonc0lipschitzc = src->nonc0lipschitzc;
    dst->nonc1suspected = src->nonc1suspected;
    dst->nonc1test0positive = src->nonc1test0positive;
    dst->nonc1test1positive = src->nonc1test1positive;
    dst->nonc1fidx = src->nonc1fidx;
    dst->nonc1lipschitzc = src->nonc1lipschitzc;
    dst->badgradsuspected = src->badgradsuspected;
    dst->badgradfidx = src->badgradfidx;
    dst->badgradvidx = src->badgradvidx;
    ae_vector_init_copy(&dst->badgradxbase, &src->badgradxbase, _state, make_automatic);
    ae_matrix_init_copy(&dst->badgraduser, &src->badgraduser, _state, make_automatic);
    ae_matrix_init_copy(&dst->badgradnum, &src->badgradnum, _state, make_automatic);
}


void _optguardreport_clear(void* _p)
{
    optguardreport *p = (optguardreport*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->badgradxbase);
    ae_matrix_clear(&p->badgraduser);
    ae_matrix_clear(&p->badgradnum);
}


void _optguardreport_destroy(void* _p)
{
    optguardreport *p = (optguardreport*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->badgradxbase);
    ae_matrix_destroy(&p->badgraduser);
    ae_matrix_destroy(&p->badgradnum);
}


void _optguardnonc0report_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    optguardnonc0report *p = (optguardnonc0report*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->x0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stp, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->f, 0, DT_REAL, _state, make_automatic);
}


void _optguardnonc0report_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    optguardnonc0report       *dst = (optguardnonc0report*)_dst;
    const optguardnonc0report *src = (const optguardnonc0report*)_src;
    dst->positive = src->positive;
    dst->fidx = src->fidx;
    ae_vector_init_copy(&dst->x0, &src->x0, _state, make_automatic);
    ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic);
    dst->n = src->n;
    ae_vector_init_copy(&dst->stp, &src->stp, _state, make_automatic);
    ae_vector_init_copy(&dst->f, &src->f, _state, make_automatic);
    dst->cnt = src->cnt;
    dst->stpidxa = src->stpidxa;
    dst->stpidxb = src->stpidxb;
    dst->inneriter = src->inneriter;
    dst->outeriter = src->outeriter;
}


void _optguardnonc0report_clear(void* _p)
{
    optguardnonc0report *p = (optguardnonc0report*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x0);
    ae_vector_clear(&p->d);
    ae_vector_clear(&p->stp);
    ae_vector_clear(&p->f);
}


void _optguardnonc0report_destroy(void* _p)
{
    optguardnonc0report *p = (optguardnonc0report*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x0);
    ae_vector_destroy(&p->d);
    ae_vector_destroy(&p->stp);
    ae_vector_destroy(&p->f);
}


void _optguardnonc1test0report_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    optguardnonc1test0report *p = (optguardnonc1test0report*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->x0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stp, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->f, 0, DT_REAL, _state, make_automatic);
}


void _optguardnonc1test0report_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    optguardnonc1test0report       *dst = (optguardnonc1test0report*)_dst;
    const optguardnonc1test0report *src = (const optguardnonc1test0report*)_src;
    dst->positive = src->positive;
    dst->fidx = src->fidx;
    ae_vector_init_copy(&dst->x0, &src->x0, _state, make_automatic);
    ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic);
    dst->n = src->n;
    ae_vector_init_copy(&dst->stp, &src->stp, _state, make_automatic);
    ae_vector_init_copy(&dst->f, &src->f, _state, make_automatic);
    dst->cnt = src->cnt;
    dst->stpidxa = src->stpidxa;
    dst->stpidxb = src->stpidxb;
    dst->inneriter = src->inneriter;
    dst->outeriter = src->outeriter;
}


void _optguardnonc1test0report_clear(void* _p)
{
    optguardnonc1test0report *p = (optguardnonc1test0report*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x0);
    ae_vector_clear(&p->d);
    ae_vector_clear(&p->stp);
    ae_vector_clear(&p->f);
}


void _optguardnonc1test0report_destroy(void* _p)
{
    optguardnonc1test0report *p = (optguardnonc1test0report*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x0);
    ae_vector_destroy(&p->d);
    ae_vector_destroy(&p->stp);
    ae_vector_destroy(&p->f);
}


void _optguardnonc1test1report_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    optguardnonc1test1report *p = (optguardnonc1test1report*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->x0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stp, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->g, 0, DT_REAL, _state, make_automatic);
}


void _optguardnonc1test1report_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    optguardnonc1test1report       *dst = (optguardnonc1test1report*)_dst;
    const optguardnonc1test1report *src = (const optguardnonc1test1report*)_src;
    dst->positive = src->positive;
    dst->fidx = src->fidx;
    dst->vidx = src->vidx;
    ae_vector_init_copy(&dst->x0, &src->x0, _state, make_automatic);
    ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic);
    dst->n = src->n;
    ae_vector_init_copy(&dst->stp, &src->stp, _state, make_automatic);
    ae_vector_init_copy(&dst->g, &src->g, _state, make_automatic);
    dst->cnt = src->cnt;
    dst->stpidxa = src->stpidxa;
    dst->stpidxb = src->stpidxb;
    dst->inneriter = src->inneriter;
    dst->outeriter = src->outeriter;
}


void _optguardnonc1test1report_clear(void* _p)
{
    optguardnonc1test1report *p = (optguardnonc1test1report*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x0);
    ae_vector_clear(&p->d);
    ae_vector_clear(&p->stp);
    ae_vector_clear(&p->g);
}


void _optguardnonc1test1report_destroy(void* _p)
{
    optguardnonc1test1report *p = (optguardnonc1test1report*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x0);
    ae_vector_destroy(&p->d);
    ae_vector_destroy(&p->stp);
    ae_vector_destroy(&p->g);
}


/*$ End $*/
