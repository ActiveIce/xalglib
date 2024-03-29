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
#include "hpccores.h"


/*$ Declarations $*/
static ae_bool hpccores_hpcpreparechunkedgradientx(/* Real    */ const ae_vector* weights,
     ae_int_t wcount,
     /* Real    */ ae_vector* hpcbuf,
     ae_state *_state);
static ae_bool hpccores_hpcfinalizechunkedgradientx(/* Real    */ const ae_vector* buf,
     ae_int_t wcount,
     /* Real    */ ae_vector* grad,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
Prepares HPC compuations  of  chunked  gradient with HPCChunkedGradient().
You  have to call this function  before  calling  HPCChunkedGradient() for
a new set of weights. You have to call it only once, see example below:

HOW TO PROCESS DATASET WITH THIS FUNCTION:
    Grad:=0
    HPCPrepareChunkedGradient(Weights, WCount, NTotal, NOut, Buf)
    foreach chunk-of-dataset do
        HPCChunkedGradient(...)
    HPCFinalizeChunkedGradient(Buf, Grad)

*************************************************************************/
void hpcpreparechunkedgradient(/* Real    */ const ae_vector* weights,
     ae_int_t wcount,
     ae_int_t ntotal,
     ae_int_t nin,
     ae_int_t nout,
     mlpbuffers* buf,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t batch4size;
    ae_int_t chunksize;


    chunksize = 4;
    batch4size = 3*chunksize*ntotal+chunksize*(2*nout+1);
    if( buf->xy.rows<chunksize||buf->xy.cols<nin+nout )
    {
        ae_matrix_set_length(&buf->xy, chunksize, nin+nout, _state);
    }
    if( buf->xy2.rows<chunksize||buf->xy2.cols<nin+nout )
    {
        ae_matrix_set_length(&buf->xy2, chunksize, nin+nout, _state);
    }
    if( buf->xyrow.cnt<nin+nout )
    {
        ae_vector_set_length(&buf->xyrow, nin+nout, _state);
    }
    if( buf->x.cnt<nin )
    {
        ae_vector_set_length(&buf->x, nin, _state);
    }
    if( buf->y.cnt<nout )
    {
        ae_vector_set_length(&buf->y, nout, _state);
    }
    if( buf->desiredy.cnt<nout )
    {
        ae_vector_set_length(&buf->desiredy, nout, _state);
    }
    if( buf->batch4buf.cnt<batch4size )
    {
        ae_vector_set_length(&buf->batch4buf, batch4size, _state);
    }
    if( buf->hpcbuf.cnt<wcount )
    {
        ae_vector_set_length(&buf->hpcbuf, wcount, _state);
    }
    if( buf->g.cnt<wcount )
    {
        ae_vector_set_length(&buf->g, wcount, _state);
    }
    if( !hpccores_hpcpreparechunkedgradientx(weights, wcount, &buf->hpcbuf, _state) )
    {
        for(i=0; i<=wcount-1; i++)
        {
            buf->hpcbuf.ptr.p_double[i] = 0.0;
        }
    }
    buf->wcount = wcount;
    buf->ntotal = ntotal;
    buf->nin = nin;
    buf->nout = nout;
    buf->chunksize = chunksize;
}


/*************************************************************************
Finalizes HPC compuations  of  chunked gradient with HPCChunkedGradient().
You  have to call this function  after  calling  HPCChunkedGradient()  for
a new set of weights. You have to call it only once, see example below:

HOW TO PROCESS DATASET WITH THIS FUNCTION:
    Grad:=0
    HPCPrepareChunkedGradient(Weights, WCount, NTotal, NOut, Buf)
    foreach chunk-of-dataset do
        HPCChunkedGradient(...)
    HPCFinalizeChunkedGradient(Buf, Grad)

*************************************************************************/
void hpcfinalizechunkedgradient(const mlpbuffers* buf,
     /* Real    */ ae_vector* grad,
     ae_state *_state)
{
    ae_int_t i;


    if( !hpccores_hpcfinalizechunkedgradientx(&buf->hpcbuf, buf->wcount, grad, _state) )
    {
        for(i=0; i<=buf->wcount-1; i++)
        {
            grad->ptr.p_double[i] = grad->ptr.p_double[i]+buf->hpcbuf.ptr.p_double[i];
        }
    }
}


/*************************************************************************
Fast kernel for chunked gradient.

*************************************************************************/
ae_bool hpcchunkedgradient(/* Real    */ const ae_vector* weights,
     /* Integer */ const ae_vector* structinfo,
     /* Real    */ const ae_vector* columnmeans,
     /* Real    */ const ae_vector* columnsigmas,
     /* Real    */ const ae_matrix* xy,
     ae_int_t cstart,
     ae_int_t csize,
     /* Real    */ ae_vector* batch4buf,
     /* Real    */ ae_vector* hpcbuf,
     double* e,
     ae_bool naturalerrorfunc,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_SSE2
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_hpcchunkedgradient(weights, structinfo, columnmeans, columnsigmas, xy, cstart, csize, batch4buf, hpcbuf, e, naturalerrorfunc);
#endif
}


/*************************************************************************
Fast kernel for chunked processing.

*************************************************************************/
ae_bool hpcchunkedprocess(/* Real    */ const ae_vector* weights,
     /* Integer */ const ae_vector* structinfo,
     /* Real    */ const ae_vector* columnmeans,
     /* Real    */ const ae_vector* columnsigmas,
     /* Real    */ const ae_matrix* xy,
     ae_int_t cstart,
     ae_int_t csize,
     /* Real    */ ae_vector* batch4buf,
     /* Real    */ ae_vector* hpcbuf,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_SSE2
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_hpcchunkedprocess(weights, structinfo, columnmeans, columnsigmas, xy, cstart, csize, batch4buf, hpcbuf);
#endif
}


/*************************************************************************
Stub function.

  -- ALGLIB routine --
     14.06.2013
     Bochkanov Sergey
*************************************************************************/
static ae_bool hpccores_hpcpreparechunkedgradientx(/* Real    */ const ae_vector* weights,
     ae_int_t wcount,
     /* Real    */ ae_vector* hpcbuf,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_SSE2
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_hpcpreparechunkedgradientx(weights, wcount, hpcbuf);
#endif
}


/*************************************************************************
Stub function.

  -- ALGLIB routine --
     14.06.2013
     Bochkanov Sergey
*************************************************************************/
static ae_bool hpccores_hpcfinalizechunkedgradientx(/* Real    */ const ae_vector* buf,
     ae_int_t wcount,
     /* Real    */ ae_vector* grad,
     ae_state *_state)
{
#ifndef ALGLIB_INTERCEPTS_SSE2
    ae_bool result;


    result = ae_false;
    return result;
#else
    return _ialglib_i_hpcfinalizechunkedgradientx(buf, wcount, grad);
#endif
}


void _mlpbuffers_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    mlpbuffers *p = (mlpbuffers*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->batch4buf, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hpcbuf, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->xy, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->xy2, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xyrow, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->y, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->desiredy, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->g, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
}


void _mlpbuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    mlpbuffers       *dst = (mlpbuffers*)_dst;
    const mlpbuffers *src = (const mlpbuffers*)_src;
    dst->chunksize = src->chunksize;
    dst->ntotal = src->ntotal;
    dst->nin = src->nin;
    dst->nout = src->nout;
    dst->wcount = src->wcount;
    ae_vector_init_copy(&dst->batch4buf, &src->batch4buf, _state, make_automatic);
    ae_vector_init_copy(&dst->hpcbuf, &src->hpcbuf, _state, make_automatic);
    ae_matrix_init_copy(&dst->xy, &src->xy, _state, make_automatic);
    ae_matrix_init_copy(&dst->xy2, &src->xy2, _state, make_automatic);
    ae_vector_init_copy(&dst->xyrow, &src->xyrow, _state, make_automatic);
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->y, &src->y, _state, make_automatic);
    ae_vector_init_copy(&dst->desiredy, &src->desiredy, _state, make_automatic);
    dst->e = src->e;
    ae_vector_init_copy(&dst->g, &src->g, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
}


void _mlpbuffers_clear(void* _p)
{
    mlpbuffers *p = (mlpbuffers*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->batch4buf);
    ae_vector_clear(&p->hpcbuf);
    ae_matrix_clear(&p->xy);
    ae_matrix_clear(&p->xy2);
    ae_vector_clear(&p->xyrow);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->y);
    ae_vector_clear(&p->desiredy);
    ae_vector_clear(&p->g);
    ae_vector_clear(&p->tmp0);
}


void _mlpbuffers_destroy(void* _p)
{
    mlpbuffers *p = (mlpbuffers*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->batch4buf);
    ae_vector_destroy(&p->hpcbuf);
    ae_matrix_destroy(&p->xy);
    ae_matrix_destroy(&p->xy2);
    ae_vector_destroy(&p->xyrow);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->y);
    ae_vector_destroy(&p->desiredy);
    ae_vector_destroy(&p->g);
    ae_vector_destroy(&p->tmp0);
}


/*$ End $*/
