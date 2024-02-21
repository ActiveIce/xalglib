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

#ifndef _hpccores_h
#define _hpccores_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"


/*$ Declarations $*/


/*************************************************************************
This structure stores  temporary  buffers  used  by  gradient  calculation
functions for neural networks.
*************************************************************************/
typedef struct
{
    ae_int_t chunksize;
    ae_int_t ntotal;
    ae_int_t nin;
    ae_int_t nout;
    ae_int_t wcount;
    ae_vector batch4buf;
    ae_vector hpcbuf;
    ae_matrix xy;
    ae_matrix xy2;
    ae_vector xyrow;
    ae_vector x;
    ae_vector y;
    ae_vector desiredy;
    double e;
    ae_vector g;
    ae_vector tmp0;
} mlpbuffers;


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);
void _mlpbuffers_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _mlpbuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _mlpbuffers_clear(void* _p);
void _mlpbuffers_destroy(void* _p);


/*$ End $*/
#endif

