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
#include "knn.h"


/*$ Declarations $*/
static ae_int_t knn_knnfirstversion = 0;
static void knn_clearreport(knnreport* rep, ae_state *_state);
static void knn_processinternal(const knnmodel* model,
     knnbuffer* buf,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
This function creates buffer  structure  which  can  be  used  to  perform
parallel KNN requests.

KNN subpackage provides two sets of computing functions - ones  which  use
internal buffer of KNN model (these  functions are single-threaded because
they use same buffer, which can not  shared  between  threads),  and  ones
which use external buffer.

This function is used to initialize external buffer.

INPUT PARAMETERS
    Model       -   KNN model which is associated with newly created buffer

OUTPUT PARAMETERS
    Buf         -   external buffer.
    
    
IMPORTANT: buffer object should be used only with model which was used  to
           initialize buffer. Any attempt to  use  buffer  with  different
           object is dangerous - you  may   get  integrity  check  failure
           (exception) because sizes of internal  arrays  do  not  fit  to
           dimensions of the model structure.

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
void knncreatebuffer(const knnmodel* model,
     knnbuffer* buf,
     ae_state *_state)
{

    _knnbuffer_clear(buf);

    if( !model->isdummy )
    {
        kdtreecreaterequestbuffer(&model->tree, &buf->treebuf, _state);
    }
    ae_vector_set_length(&buf->x, model->nvars, _state);
    ae_vector_set_length(&buf->y, model->nout, _state);
}


/*************************************************************************
This subroutine creates KNNBuilder object which is used to train KNN models.

By default, new builder stores empty dataset and some  reasonable  default
settings. At the very least, you should specify dataset prior to  building
KNN model. You can also tweak settings of the model construction algorithm
(recommended, although default settings should work well).

Following actions are mandatory:
* calling knnbuildersetdataset() to specify dataset
* calling knnbuilderbuildknnmodel() to build KNN model using current
  dataset and default settings
  
Additionally, you may call:
* knnbuildersetnorm() to change norm being used

INPUT PARAMETERS:
    none

OUTPUT PARAMETERS:
    S           -   KNN builder

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
void knnbuildercreate(knnbuilder* s, ae_state *_state)
{

    _knnbuilder_clear(s);

    
    /*
     * Empty dataset
     */
    s->dstype = -1;
    s->npoints = 0;
    s->nvars = 0;
    s->iscls = ae_false;
    s->nout = 1;
    
    /*
     * Default training settings
     */
    s->knnnrm = 2;
}


/*************************************************************************
Specifies regression problem (one or more continuous  output variables are
predicted). There also exists "classification" version of this function.

This subroutine adds dense dataset to the internal storage of the  builder
object. Specifying your dataset in the dense format means that  the  dense
version of the KNN construction algorithm will be invoked.

INPUT PARAMETERS:
    S           -   KNN builder object
    XY          -   array[NPoints,NVars+NOut] (note: actual  size  can  be
                    larger, only leading part is used anyway), dataset:
                    * first NVars elements of each row store values of the
                      independent variables
                    * next NOut elements store  values  of  the  dependent
                      variables
    NPoints     -   number of rows in the dataset, NPoints>=1
    NVars       -   number of independent variables, NVars>=1 
    NOut        -   number of dependent variables, NOut>=1

OUTPUT PARAMETERS:
    S           -   KNN builder

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
void knnbuildersetdatasetreg(knnbuilder* s,
     /* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nvars,
     ae_int_t nout,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;


    
    /*
     * Check parameters
     */
    ae_assert(npoints>=1, "knnbuildersetdatasetreg: npoints<1", _state);
    ae_assert(nvars>=1, "knnbuildersetdatasetreg: nvars<1", _state);
    ae_assert(nout>=1, "knnbuildersetdatasetreg: nout<1", _state);
    ae_assert(xy->rows>=npoints, "knnbuildersetdatasetreg: rows(xy)<npoints", _state);
    ae_assert(xy->cols>=nvars+nout, "knnbuildersetdatasetreg: cols(xy)<nvars+nout", _state);
    ae_assert(apservisfinitematrix(xy, npoints, nvars+nout, _state), "knnbuildersetdatasetreg: xy parameter contains INFs or NANs", _state);
    
    /*
     * Set dataset
     */
    s->dstype = 0;
    s->iscls = ae_false;
    s->npoints = npoints;
    s->nvars = nvars;
    s->nout = nout;
    rmatrixsetlengthatleast(&s->dsdata, npoints, nvars, _state);
    for(i=0; i<=npoints-1; i++)
    {
        for(j=0; j<=nvars-1; j++)
        {
            s->dsdata.ptr.pp_double[i][j] = xy->ptr.pp_double[i][j];
        }
    }
    rvectorsetlengthatleast(&s->dsrval, npoints*nout, _state);
    for(i=0; i<=npoints-1; i++)
    {
        for(j=0; j<=nout-1; j++)
        {
            s->dsrval.ptr.p_double[i*nout+j] = xy->ptr.pp_double[i][nvars+j];
        }
    }
}


/*************************************************************************
Specifies classification problem (two  or  more  classes  are  predicted).
There also exists "regression" version of this function.

This subroutine adds dense dataset to the internal storage of the  builder
object. Specifying your dataset in the dense format means that  the  dense
version of the KNN construction algorithm will be invoked.

INPUT PARAMETERS:
    S           -   KNN builder object
    XY          -   array[NPoints,NVars+1] (note:   actual   size  can  be
                    larger, only leading part is used anyway), dataset:
                    * first NVars elements of each row store values of the
                      independent variables
                    * next element stores class index, in [0,NClasses)
    NPoints     -   number of rows in the dataset, NPoints>=1
    NVars       -   number of independent variables, NVars>=1 
    NClasses    -   number of classes, NClasses>=2

OUTPUT PARAMETERS:
    S           -   KNN builder

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
void knnbuildersetdatasetcls(knnbuilder* s,
     /* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nvars,
     ae_int_t nclasses,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;


    
    /*
     * Check parameters
     */
    ae_assert(npoints>=1, "knnbuildersetdatasetcls: npoints<1", _state);
    ae_assert(nvars>=1, "knnbuildersetdatasetcls: nvars<1", _state);
    ae_assert(nclasses>=2, "knnbuildersetdatasetcls: nclasses<2", _state);
    ae_assert(xy->rows>=npoints, "knnbuildersetdatasetcls: rows(xy)<npoints", _state);
    ae_assert(xy->cols>=nvars+1, "knnbuildersetdatasetcls: cols(xy)<nvars+1", _state);
    ae_assert(apservisfinitematrix(xy, npoints, nvars+1, _state), "knnbuildersetdatasetcls: xy parameter contains INFs or NANs", _state);
    for(i=0; i<=npoints-1; i++)
    {
        j = ae_round(xy->ptr.pp_double[i][nvars], _state);
        ae_assert(j>=0&&j<nclasses, "knnbuildersetdatasetcls: last column of xy contains invalid class number", _state);
    }
    
    /*
     * Set dataset
     */
    s->iscls = ae_true;
    s->dstype = 0;
    s->npoints = npoints;
    s->nvars = nvars;
    s->nout = nclasses;
    rmatrixsetlengthatleast(&s->dsdata, npoints, nvars, _state);
    for(i=0; i<=npoints-1; i++)
    {
        for(j=0; j<=nvars-1; j++)
        {
            s->dsdata.ptr.pp_double[i][j] = xy->ptr.pp_double[i][j];
        }
    }
    ivectorsetlengthatleast(&s->dsival, npoints, _state);
    for(i=0; i<=npoints-1; i++)
    {
        s->dsival.ptr.p_int[i] = ae_round(xy->ptr.pp_double[i][nvars], _state);
    }
}


/*************************************************************************
This function sets norm type used for neighbor search.

INPUT PARAMETERS:
    S           -   decision forest builder object
    NormType    -   norm type:
                    * 0      inf-norm
                    * 1      1-norm
                    * 2      Euclidean norm (default)

OUTPUT PARAMETERS:
    S           -   decision forest builder

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
void knnbuildersetnorm(knnbuilder* s, ae_int_t nrmtype, ae_state *_state)
{


    ae_assert((nrmtype==0||nrmtype==1)||nrmtype==2, "knnbuildersetnorm: unexpected norm type", _state);
    s->knnnrm = nrmtype;
}


/*************************************************************************
This subroutine builds KNN model  according  to  current  settings,  using
dataset internally stored in the builder object.

The model being built performs inference using Eps-approximate  K  nearest
neighbors search algorithm, with:
* K=1,  Eps=0 corresponding to the "nearest neighbor algorithm"
* K>1,  Eps=0 corresponding to the "K nearest neighbors algorithm"
* K>=1, Eps>0 corresponding to "approximate nearest neighbors algorithm"

An approximate KNN is a good option for high-dimensional  datasets  (exact
KNN works slowly when dimensions count grows).

An ALGLIB implementation of kd-trees is used to perform k-nn searches.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

INPUT PARAMETERS:
    S       -   KNN builder object
    K       -   number of neighbors to search for, K>=1
    Eps     -   approximation factor:
                * Eps=0 means that exact kNN search is performed
                * Eps>0 means that (1+Eps)-approximate search is performed

OUTPUT PARAMETERS:
    Model       -   KNN model
    Rep         -   report

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
void knnbuilderbuildknnmodel(knnbuilder* s,
     ae_int_t k,
     double eps,
     knnmodel* model,
     knnreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t nvars;
    ae_int_t nout;
    ae_int_t npoints;
    ae_bool iscls;
    ae_matrix xy;
    ae_vector tags;

    ae_frame_make(_state, &_frame_block);
    memset(&xy, 0, sizeof(xy));
    memset(&tags, 0, sizeof(tags));
    _knnmodel_clear(model);
    _knnreport_clear(rep);
    ae_matrix_init(&xy, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tags, 0, DT_INT, _state, ae_true);

    npoints = s->npoints;
    nvars = s->nvars;
    nout = s->nout;
    iscls = s->iscls;
    
    /*
     * Check settings
     */
    ae_assert(k>=1, "knnbuilderbuildknnmodel: k<1", _state);
    ae_assert(ae_isfinite(eps, _state)&&ae_fp_greater_eq(eps,(double)(0)), "knnbuilderbuildknnmodel: eps<0", _state);
    
    /*
     * Prepare output
     */
    knn_clearreport(rep, _state);
    model->nvars = nvars;
    model->nout = nout;
    model->iscls = iscls;
    model->k = k;
    model->eps = eps;
    model->isdummy = ae_false;
    
    /*
     * Quick exit for empty dataset
     */
    if( s->dstype==-1 )
    {
        model->isdummy = ae_true;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Build kd-tree
     */
    if( iscls )
    {
        ae_matrix_set_length(&xy, npoints, nvars+1, _state);
        ae_vector_set_length(&tags, npoints, _state);
        for(i=0; i<=npoints-1; i++)
        {
            for(j=0; j<=nvars-1; j++)
            {
                xy.ptr.pp_double[i][j] = s->dsdata.ptr.pp_double[i][j];
            }
            xy.ptr.pp_double[i][nvars] = (double)(s->dsival.ptr.p_int[i]);
            tags.ptr.p_int[i] = s->dsival.ptr.p_int[i];
        }
        kdtreebuildtagged(&xy, &tags, npoints, nvars, 0, s->knnnrm, &model->tree, _state);
    }
    else
    {
        ae_matrix_set_length(&xy, npoints, nvars+nout, _state);
        for(i=0; i<=npoints-1; i++)
        {
            for(j=0; j<=nvars-1; j++)
            {
                xy.ptr.pp_double[i][j] = s->dsdata.ptr.pp_double[i][j];
            }
            for(j=0; j<=nout-1; j++)
            {
                xy.ptr.pp_double[i][nvars+j] = s->dsrval.ptr.p_double[i*nout+j];
            }
        }
        kdtreebuild(&xy, npoints, nvars, nout, s->knnnrm, &model->tree, _state);
    }
    
    /*
     * Build buffer
     */
    knncreatebuffer(model, &model->buffer, _state);
    
    /*
     * Report
     */
    knnallerrors(model, &xy, npoints, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Changing search settings of KNN model.

K and EPS parameters of KNN  (AKNN)  search  are  specified  during  model
construction. However, plain KNN algorithm with Euclidean distance  allows
you to change them at any moment.

NOTE: future versions of KNN model may support advanced versions  of  KNN,
      such as NCA or LMNN. It is possible that such algorithms won't allow
      you to change search settings on the fly. If you call this  function
      for an algorithm which does not support on-the-fly changes, it  will
      throw an exception.

INPUT PARAMETERS:
    Model   -   KNN model
    K       -   K>=1, neighbors count
    EPS     -   accuracy of the EPS-approximate NN search. Set to 0.0,  if
                you want to perform "classic" KNN search.  Specify  larger
                values  if  you  need  to  speed-up  high-dimensional  KNN
                queries.

OUTPUT PARAMETERS:
    nothing on success, exception on failure

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
void knnrewritekeps(knnmodel* model,
     ae_int_t k,
     double eps,
     ae_state *_state)
{


    ae_assert(k>=1, "knnrewritekeps: k<1", _state);
    ae_assert(ae_isfinite(eps, _state)&&ae_fp_greater_eq(eps,(double)(0)), "knnrewritekeps: eps<0", _state);
    model->k = k;
    model->eps = eps;
}


/*************************************************************************
Inference using KNN model.

See also knnprocess0(), knnprocessi() and knnclassify() for options with a
bit more convenient interface.

IMPORTANT: this function is thread-unsafe and modifies internal structures
           of the model! You can not use same model  object  for  parallel
           evaluation from several threads.
           
           Use knntsprocess() with independent  thread-local  buffers,  if
           you need thread-safe evaluation.

INPUT PARAMETERS:
    Model   -   KNN model
    X       -   input vector,  array[0..NVars-1].
    Y       -   possible preallocated buffer. Reused if long enough.

OUTPUT PARAMETERS:
    Y       -   result. Regression estimate when solving regression  task,
                vector of posterior probabilities for classification task.

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
void knnprocess(knnmodel* model,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{


    knntsprocess(model, &model->buffer, x, y, _state);
}


/*************************************************************************
This function returns first component of the  inferred  vector  (i.e.  one
with index #0).

It is a convenience wrapper for knnprocess() intended for either:
* 1-dimensional regression problems
* 2-class classification problems

In the former case this function returns inference result as scalar, which
is definitely more convenient that wrapping it as vector.  In  the  latter
case it returns probability of object belonging to class #0.

If you call it for anything different from two cases above, it  will  work
as defined, i.e. return y[0], although it is of less use in such cases.

IMPORTANT: this function is thread-unsafe and modifies internal structures
           of the model! You can not use same model  object  for  parallel
           evaluation from several threads.
           
           Use knntsprocess() with independent  thread-local  buffers,  if
           you need thread-safe evaluation.

INPUT PARAMETERS:
    Model   -   KNN model
    X       -   input vector,  array[0..NVars-1].

RESULT:
    Y[0]

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
double knnprocess0(knnmodel* model,
     /* Real    */ const ae_vector* x,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t nvars;
    double result;


    nvars = model->nvars;
    for(i=0; i<=nvars-1; i++)
    {
        model->buffer.x.ptr.p_double[i] = x->ptr.p_double[i];
    }
    knn_processinternal(model, &model->buffer, _state);
    result = model->buffer.y.ptr.p_double[0];
    return result;
}


/*************************************************************************
This function returns most probable class number for an  input  X.  It  is
same as calling knnprocess(model,x,y), then determining i=argmax(y[i]) and
returning i.

A class number in [0,NOut) range in returned for classification  problems,
-1 is returned when this function is called for regression problems.

IMPORTANT: this function is thread-unsafe and modifies internal structures
           of the model! You can not use same model  object  for  parallel
           evaluation from several threads.
           
           Use knntsprocess() with independent  thread-local  buffers,  if
           you need thread-safe evaluation.

INPUT PARAMETERS:
    Model   -   KNN model
    X       -   input vector,  array[0..NVars-1].

RESULT:
    class number, -1 for regression tasks

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
ae_int_t knnclassify(knnmodel* model,
     /* Real    */ const ae_vector* x,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t nvars;
    ae_int_t nout;
    ae_int_t result;


    if( !model->iscls )
    {
        result = -1;
        return result;
    }
    nvars = model->nvars;
    nout = model->nout;
    for(i=0; i<=nvars-1; i++)
    {
        model->buffer.x.ptr.p_double[i] = x->ptr.p_double[i];
    }
    knn_processinternal(model, &model->buffer, _state);
    result = 0;
    for(i=1; i<=nout-1; i++)
    {
        if( model->buffer.y.ptr.p_double[i]>model->buffer.y.ptr.p_double[result] )
        {
            result = i;
        }
    }
    return result;
}


/*************************************************************************
'interactive' variant of knnprocess()  for  languages  like  Python  which
support constructs like "y = knnprocessi(model,x)" and interactive mode of
the interpreter.

This function allocates new array on each call,  so  it  is  significantly
slower than its 'non-interactive' counterpart, but it is  more  convenient
when you call it from command line.

IMPORTANT: this  function  is  thread-unsafe  and  may   modify   internal
           structures of the model! You can not use same model  object for
           parallel evaluation from several threads.
           
           Use knntsprocess()  with  independent  thread-local  buffers if
           you need thread-safe evaluation.

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
void knnprocessi(knnmodel* model,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{

    ae_vector_clear(y);

    knnprocess(model, x, y, _state);
}


/*************************************************************************
Thread-safe procesing using external buffer for temporaries.

This function is thread-safe (i.e .  you  can  use  same  KNN  model  from
multiple threads) as long as you use different buffer objects for different
threads.

INPUT PARAMETERS:
    Model   -   KNN model
    Buf     -   buffer object, must be  allocated  specifically  for  this
                model with knncreatebuffer().
    X       -   input vector,  array[NVars]

OUTPUT PARAMETERS:
    Y       -   result, array[NOut].   Regression  estimate  when  solving
                regression task,  vector  of  posterior  probabilities for
                a classification task.

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
void knntsprocess(const knnmodel* model,
     knnbuffer* buf,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t nvars;
    ae_int_t nout;


    nvars = model->nvars;
    nout = model->nout;
    for(i=0; i<=nvars-1; i++)
    {
        buf->x.ptr.p_double[i] = x->ptr.p_double[i];
    }
    knn_processinternal(model, buf, _state);
    if( y->cnt<nout )
    {
        ae_vector_set_length(y, nout, _state);
    }
    for(i=0; i<=nout-1; i++)
    {
        y->ptr.p_double[i] = buf->y.ptr.p_double[i];
    }
}


/*************************************************************************
Relative classification error on the test set

INPUT PARAMETERS:
    Model   -   KNN model
    XY      -   test set
    NPoints -   test set size

RESULT:
    percent of incorrectly classified cases.
    Zero if model solves regression task.
    
NOTE: if  you  need several different kinds of error metrics, it is better
      to use knnallerrors() which computes all error metric  with just one
      pass over dataset. 

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
double knnrelclserror(const knnmodel* model,
     /* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_state *_state)
{
    ae_frame _frame_block;
    knnreport rep;
    double result;

    ae_frame_make(_state, &_frame_block);
    memset(&rep, 0, sizeof(rep));
    _knnreport_init(&rep, _state, ae_true);

    knnallerrors(model, xy, npoints, &rep, _state);
    result = rep.relclserror;
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Average cross-entropy (in bits per element) on the test set

INPUT PARAMETERS:
    Model   -   KNN model
    XY      -   test set
    NPoints -   test set size

RESULT:
    CrossEntropy/NPoints.
    Zero if model solves regression task.

NOTE: the cross-entropy metric is too unstable when used to  evaluate  KNN
      models (such models can report exactly  zero probabilities),  so  we
      do not recommend using it.
    
NOTE: if  you  need several different kinds of error metrics, it is better
      to use knnallerrors() which computes all error metric  with just one
      pass over dataset. 

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
double knnavgce(const knnmodel* model,
     /* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_state *_state)
{
    ae_frame _frame_block;
    knnreport rep;
    double result;

    ae_frame_make(_state, &_frame_block);
    memset(&rep, 0, sizeof(rep));
    _knnreport_init(&rep, _state, ae_true);

    knnallerrors(model, xy, npoints, &rep, _state);
    result = rep.avgce;
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
RMS error on the test set.

Its meaning for regression task is obvious. As for classification problems,
RMS error means error when estimating posterior probabilities.

INPUT PARAMETERS:
    Model   -   KNN model
    XY      -   test set
    NPoints -   test set size

RESULT:
    root mean square error.
    
NOTE: if  you  need several different kinds of error metrics, it is better
      to use knnallerrors() which computes all error metric  with just one
      pass over dataset.

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
double knnrmserror(const knnmodel* model,
     /* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_state *_state)
{
    ae_frame _frame_block;
    knnreport rep;
    double result;

    ae_frame_make(_state, &_frame_block);
    memset(&rep, 0, sizeof(rep));
    _knnreport_init(&rep, _state, ae_true);

    knnallerrors(model, xy, npoints, &rep, _state);
    result = rep.rmserror;
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Average error on the test set

Its meaning for regression task is obvious. As for classification problems,
average error means error when estimating posterior probabilities.

INPUT PARAMETERS:
    Model   -   KNN model
    XY      -   test set
    NPoints -   test set size

RESULT:
    average error
    
NOTE: if  you  need several different kinds of error metrics, it is better
      to use knnallerrors() which computes all error metric  with just one
      pass over dataset.

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
double knnavgerror(const knnmodel* model,
     /* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_state *_state)
{
    ae_frame _frame_block;
    knnreport rep;
    double result;

    ae_frame_make(_state, &_frame_block);
    memset(&rep, 0, sizeof(rep));
    _knnreport_init(&rep, _state, ae_true);

    knnallerrors(model, xy, npoints, &rep, _state);
    result = rep.avgerror;
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Average relative error on the test set

Its meaning for regression task is obvious. As for classification problems,
average relative error means error when estimating posterior probabilities.

INPUT PARAMETERS:
    Model   -   KNN model
    XY      -   test set
    NPoints -   test set size

RESULT:
    average relative error
    
NOTE: if  you  need several different kinds of error metrics, it is better
      to use knnallerrors() which computes all error metric  with just one
      pass over dataset.

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
double knnavgrelerror(const knnmodel* model,
     /* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_state *_state)
{
    ae_frame _frame_block;
    knnreport rep;
    double result;

    ae_frame_make(_state, &_frame_block);
    memset(&rep, 0, sizeof(rep));
    _knnreport_init(&rep, _state, ae_true);

    knnallerrors(model, xy, npoints, &rep, _state);
    result = rep.avgrelerror;
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Calculates all kinds of errors for the model in one call.

INPUT PARAMETERS:
    Model   -   KNN model
    XY      -   test set:
                * one row per point
                * first NVars columns store independent variables
                * depending on problem type:
                  * next column stores class number in [0,NClasses) -  for
                    classification problems
                  * next NOut columns  store  dependent  variables  -  for
                    regression problems
    NPoints -   test set size, NPoints>=0

OUTPUT PARAMETERS:
    Rep     -   following fields are loaded with errors for both regression
                and classification models:
                * rep.rmserror - RMS error for the output
                * rep.avgerror - average error
                * rep.avgrelerror - average relative error
                following fields are set only  for classification  models,
                zero for regression ones:
                * relclserror   - relative classification error, in [0,1]
                * avgce - average cross-entropy in bits per dataset entry

NOTE: the cross-entropy metric is too unstable when used to  evaluate  KNN
      models (such models can report exactly  zero probabilities),  so  we
      do not recommend using it.

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
void knnallerrors(const knnmodel* model,
     /* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     knnreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    knnbuffer buf;
    ae_vector desiredy;
    ae_vector errbuf;
    ae_int_t nvars;
    ae_int_t nout;
    ae_int_t ny;
    ae_bool iscls;
    ae_int_t i;
    ae_int_t j;

    ae_frame_make(_state, &_frame_block);
    memset(&buf, 0, sizeof(buf));
    memset(&desiredy, 0, sizeof(desiredy));
    memset(&errbuf, 0, sizeof(errbuf));
    _knnreport_clear(rep);
    _knnbuffer_init(&buf, _state, ae_true);
    ae_vector_init(&desiredy, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&errbuf, 0, DT_REAL, _state, ae_true);

    nvars = model->nvars;
    nout = model->nout;
    iscls = model->iscls;
    if( iscls )
    {
        ny = 1;
    }
    else
    {
        ny = nout;
    }
    
    /*
     * Check input
     */
    ae_assert(npoints>=0, "knnallerrors: npoints<0", _state);
    ae_assert(xy->rows>=npoints, "knnallerrors: rows(xy)<npoints", _state);
    ae_assert(xy->cols>=nvars+ny, "knnallerrors: cols(xy)<nvars+nout", _state);
    ae_assert(apservisfinitematrix(xy, npoints, nvars+ny, _state), "knnallerrors: xy parameter contains INFs or NANs", _state);
    
    /*
     * Clean up report
     */
    knn_clearreport(rep, _state);
    
    /*
     * Quick exit if needed
     */
    if( model->isdummy||npoints==0 )
    {
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Process using local buffer
     */
    knncreatebuffer(model, &buf, _state);
    if( iscls )
    {
        dserrallocate(nout, &errbuf, _state);
    }
    else
    {
        dserrallocate(-nout, &errbuf, _state);
    }
    ae_vector_set_length(&desiredy, ny, _state);
    for(i=0; i<=npoints-1; i++)
    {
        for(j=0; j<=nvars-1; j++)
        {
            buf.x.ptr.p_double[j] = xy->ptr.pp_double[i][j];
        }
        if( iscls )
        {
            j = ae_round(xy->ptr.pp_double[i][nvars], _state);
            ae_assert(j>=0&&j<nout, "knnallerrors: one of the class labels is not in [0,NClasses)", _state);
            desiredy.ptr.p_double[0] = (double)(j);
        }
        else
        {
            for(j=0; j<=nout-1; j++)
            {
                desiredy.ptr.p_double[j] = xy->ptr.pp_double[i][nvars+j];
            }
        }
        knn_processinternal(model, &buf, _state);
        dserraccumulate(&errbuf, &buf.y, &desiredy, _state);
    }
    dserrfinish(&errbuf, _state);
    
    /*
     * Extract results
     */
    if( iscls )
    {
        rep->relclserror = errbuf.ptr.p_double[0];
        rep->avgce = errbuf.ptr.p_double[1];
    }
    rep->rmserror = errbuf.ptr.p_double[2];
    rep->avgerror = errbuf.ptr.p_double[3];
    rep->avgrelerror = errbuf.ptr.p_double[4];
    ae_frame_leave(_state);
}


/*************************************************************************
Serializer: allocation

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
void knnalloc(ae_serializer* s, const knnmodel* model, ae_state *_state)
{


    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    if( !model->isdummy )
    {
        kdtreealloc(s, &model->tree, _state);
    }
}


/*************************************************************************
Serializer: serialization

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
void knnserialize(ae_serializer* s,
     const knnmodel* model,
     ae_state *_state)
{


    ae_serializer_serialize_int(s, getknnserializationcode(_state), _state);
    ae_serializer_serialize_int(s, knn_knnfirstversion, _state);
    ae_serializer_serialize_int(s, model->nvars, _state);
    ae_serializer_serialize_int(s, model->nout, _state);
    ae_serializer_serialize_int(s, model->k, _state);
    ae_serializer_serialize_double(s, model->eps, _state);
    ae_serializer_serialize_bool(s, model->iscls, _state);
    ae_serializer_serialize_bool(s, model->isdummy, _state);
    if( !model->isdummy )
    {
        kdtreeserialize(s, &model->tree, _state);
    }
}


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
void knnunserialize(ae_serializer* s, knnmodel* model, ae_state *_state)
{
    ae_int_t i0;
    ae_int_t i1;

    _knnmodel_clear(model);

    
    /*
     * check correctness of header
     */
    ae_serializer_unserialize_int(s, &i0, _state);
    ae_assert(i0==getknnserializationcode(_state), "KNNUnserialize: stream header corrupted", _state);
    ae_serializer_unserialize_int(s, &i1, _state);
    ae_assert(i1==knn_knnfirstversion, "KNNUnserialize: stream header corrupted", _state);
    
    /*
     * Unserialize data
     */
    ae_serializer_unserialize_int(s, &model->nvars, _state);
    ae_serializer_unserialize_int(s, &model->nout, _state);
    ae_serializer_unserialize_int(s, &model->k, _state);
    ae_serializer_unserialize_double(s, &model->eps, _state);
    ae_serializer_unserialize_bool(s, &model->iscls, _state);
    ae_serializer_unserialize_bool(s, &model->isdummy, _state);
    if( !model->isdummy )
    {
        kdtreeunserialize(s, &model->tree, _state);
    }
    
    /*
     * Prepare local buffer
     */
    knncreatebuffer(model, &model->buffer, _state);
}


/*************************************************************************
Sets report fields to their default values

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
static void knn_clearreport(knnreport* rep, ae_state *_state)
{


    rep->relclserror = (double)(0);
    rep->avgce = (double)(0);
    rep->rmserror = (double)(0);
    rep->avgerror = (double)(0);
    rep->avgrelerror = (double)(0);
}


/*************************************************************************
This function processes buf.X and stores result to buf.Y

INPUT PARAMETERS
    Model       -   KNN model
    Buf         -   processing buffer.
    
    
IMPORTANT: buffer object should be used only with model which was used  to
           initialize buffer. Any attempt to  use  buffer  with  different
           object is dangerous - you  may   get  integrity  check  failure
           (exception) because sizes of internal  arrays  do  not  fit  to
           dimensions of the model structure.

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
static void knn_processinternal(const knnmodel* model,
     knnbuffer* buf,
     ae_state *_state)
{
    ae_int_t nvars;
    ae_int_t nout;
    ae_bool iscls;
    ae_int_t nncnt;
    ae_int_t i;
    ae_int_t j;
    double v;


    nvars = model->nvars;
    nout = model->nout;
    iscls = model->iscls;
    
    /*
     * Quick exit if needed
     */
    if( model->isdummy )
    {
        for(i=0; i<=nout-1; i++)
        {
            buf->y.ptr.p_double[i] = (double)(0);
        }
        return;
    }
    
    /*
     * Perform request, average results
     */
    for(i=0; i<=nout-1; i++)
    {
        buf->y.ptr.p_double[i] = (double)(0);
    }
    nncnt = kdtreetsqueryaknn(&model->tree, &buf->treebuf, &buf->x, model->k, ae_true, model->eps, _state);
    v = (double)1/coalesce((double)(nncnt), (double)(1), _state);
    if( iscls )
    {
        kdtreetsqueryresultstags(&model->tree, &buf->treebuf, &buf->tags, _state);
        for(i=0; i<=nncnt-1; i++)
        {
            j = buf->tags.ptr.p_int[i];
            buf->y.ptr.p_double[j] = buf->y.ptr.p_double[j]+v;
        }
    }
    else
    {
        kdtreetsqueryresultsxy(&model->tree, &buf->treebuf, &buf->xy, _state);
        for(i=0; i<=nncnt-1; i++)
        {
            for(j=0; j<=nout-1; j++)
            {
                buf->y.ptr.p_double[j] = buf->y.ptr.p_double[j]+v*buf->xy.ptr.pp_double[i][nvars+j];
            }
        }
    }
}


void _knnbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    knnbuffer *p = (knnbuffer*)_p;
    ae_touch_ptr((void*)p);
    _kdtreerequestbuffer_init(&p->treebuf, _state, make_automatic);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->y, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tags, 0, DT_INT, _state, make_automatic);
    ae_matrix_init(&p->xy, 0, 0, DT_REAL, _state, make_automatic);
}


void _knnbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    knnbuffer       *dst = (knnbuffer*)_dst;
    const knnbuffer *src = (const knnbuffer*)_src;
    _kdtreerequestbuffer_init_copy(&dst->treebuf, &src->treebuf, _state, make_automatic);
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->y, &src->y, _state, make_automatic);
    ae_vector_init_copy(&dst->tags, &src->tags, _state, make_automatic);
    ae_matrix_init_copy(&dst->xy, &src->xy, _state, make_automatic);
}


void _knnbuffer_clear(void* _p)
{
    knnbuffer *p = (knnbuffer*)_p;
    ae_touch_ptr((void*)p);
    _kdtreerequestbuffer_clear(&p->treebuf);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->y);
    ae_vector_clear(&p->tags);
    ae_matrix_clear(&p->xy);
}


void _knnbuffer_destroy(void* _p)
{
    knnbuffer *p = (knnbuffer*)_p;
    ae_touch_ptr((void*)p);
    _kdtreerequestbuffer_destroy(&p->treebuf);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->y);
    ae_vector_destroy(&p->tags);
    ae_matrix_destroy(&p->xy);
}


void _knnbuilder_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    knnbuilder *p = (knnbuilder*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_init(&p->dsdata, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dsrval, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dsival, 0, DT_INT, _state, make_automatic);
}


void _knnbuilder_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    knnbuilder       *dst = (knnbuilder*)_dst;
    const knnbuilder *src = (const knnbuilder*)_src;
    dst->dstype = src->dstype;
    dst->npoints = src->npoints;
    dst->nvars = src->nvars;
    dst->iscls = src->iscls;
    dst->nout = src->nout;
    ae_matrix_init_copy(&dst->dsdata, &src->dsdata, _state, make_automatic);
    ae_vector_init_copy(&dst->dsrval, &src->dsrval, _state, make_automatic);
    ae_vector_init_copy(&dst->dsival, &src->dsival, _state, make_automatic);
    dst->knnnrm = src->knnnrm;
}


void _knnbuilder_clear(void* _p)
{
    knnbuilder *p = (knnbuilder*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_clear(&p->dsdata);
    ae_vector_clear(&p->dsrval);
    ae_vector_clear(&p->dsival);
}


void _knnbuilder_destroy(void* _p)
{
    knnbuilder *p = (knnbuilder*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_destroy(&p->dsdata);
    ae_vector_destroy(&p->dsrval);
    ae_vector_destroy(&p->dsival);
}


void _knnmodel_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    knnmodel *p = (knnmodel*)_p;
    ae_touch_ptr((void*)p);
    _kdtree_init(&p->tree, _state, make_automatic);
    _knnbuffer_init(&p->buffer, _state, make_automatic);
}


void _knnmodel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    knnmodel       *dst = (knnmodel*)_dst;
    const knnmodel *src = (const knnmodel*)_src;
    dst->nvars = src->nvars;
    dst->nout = src->nout;
    dst->k = src->k;
    dst->eps = src->eps;
    dst->iscls = src->iscls;
    dst->isdummy = src->isdummy;
    _kdtree_init_copy(&dst->tree, &src->tree, _state, make_automatic);
    _knnbuffer_init_copy(&dst->buffer, &src->buffer, _state, make_automatic);
}


void _knnmodel_clear(void* _p)
{
    knnmodel *p = (knnmodel*)_p;
    ae_touch_ptr((void*)p);
    _kdtree_clear(&p->tree);
    _knnbuffer_clear(&p->buffer);
}


void _knnmodel_destroy(void* _p)
{
    knnmodel *p = (knnmodel*)_p;
    ae_touch_ptr((void*)p);
    _kdtree_destroy(&p->tree);
    _knnbuffer_destroy(&p->buffer);
}


void _knnreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    knnreport *p = (knnreport*)_p;
    ae_touch_ptr((void*)p);
}


void _knnreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    knnreport       *dst = (knnreport*)_dst;
    const knnreport *src = (const knnreport*)_src;
    dst->relclserror = src->relclserror;
    dst->avgce = src->avgce;
    dst->rmserror = src->rmserror;
    dst->avgerror = src->avgerror;
    dst->avgrelerror = src->avgrelerror;
}


void _knnreport_clear(void* _p)
{
    knnreport *p = (knnreport*)_p;
    ae_touch_ptr((void*)p);
}


void _knnreport_destroy(void* _p)
{
    knnreport *p = (knnreport*)_p;
    ae_touch_ptr((void*)p);
}


/*$ End $*/
