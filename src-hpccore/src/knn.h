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

#ifndef _knn_h
#define _knn_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "scodes.h"
#include "ablasf.h"
#include "tsort.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "nearestneighbor.h"
#include "basicstatops.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "basestat.h"
#include "bdss.h"


/*$ Declarations $*/


/*************************************************************************
Buffer object which is used to perform  various  requests  (usually  model
inference) in the multithreaded mode (multiple threads working  with  same
KNN object).

This object should be created with KNNCreateBuffer().
*************************************************************************/
typedef struct
{
    kdtreerequestbuffer treebuf;
    ae_vector x;
    ae_vector y;
    ae_vector tags;
    ae_matrix xy;
} knnbuffer;


/*************************************************************************
A KNN builder object; this object encapsulates  dataset  and  all  related
settings, it is used to create an actual instance of KNN model.
*************************************************************************/
typedef struct
{
    ae_int_t dstype;
    ae_int_t npoints;
    ae_int_t nvars;
    ae_bool iscls;
    ae_int_t nout;
    ae_matrix dsdata;
    ae_vector dsrval;
    ae_vector dsival;
    ae_int_t knnnrm;
} knnbuilder;


/*************************************************************************
KNN model, can be used for classification or regression
*************************************************************************/
typedef struct
{
    ae_int_t nvars;
    ae_int_t nout;
    ae_int_t k;
    double eps;
    ae_bool iscls;
    ae_bool isdummy;
    kdtree tree;
    knnbuffer buffer;
} knnmodel;


/*************************************************************************
KNN training report.

Following fields store training set errors:
* relclserror       -   fraction of misclassified cases, [0,1]
* avgce             -   average cross-entropy in bits per symbol
* rmserror          -   root-mean-square error
* avgerror          -   average error
* avgrelerror       -   average relative error

For classification problems:
* RMS, AVG and AVGREL errors are calculated for posterior probabilities

For regression problems:
* RELCLS and AVGCE errors are zero
*************************************************************************/
typedef struct
{
    double relclserror;
    double avgce;
    double rmserror;
    double avgerror;
    double avgrelerror;
} knnreport;


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
     ae_state *_state);


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
void knnbuildercreate(knnbuilder* s, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
void knnbuildersetnorm(knnbuilder* s, ae_int_t nrmtype, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


/*************************************************************************
Serializer: allocation

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
void knnalloc(ae_serializer* s, const knnmodel* model, ae_state *_state);


/*************************************************************************
Serializer: serialization

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
void knnserialize(ae_serializer* s,
     const knnmodel* model,
     ae_state *_state);


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
void knnunserialize(ae_serializer* s, knnmodel* model, ae_state *_state);
void _knnbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _knnbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _knnbuffer_clear(void* _p);
void _knnbuffer_destroy(void* _p);
void _knnbuilder_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _knnbuilder_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _knnbuilder_clear(void* _p);
void _knnbuilder_destroy(void* _p);
void _knnmodel_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _knnmodel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _knnmodel_clear(void* _p);
void _knnmodel_destroy(void* _p);
void _knnreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _knnreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _knnreport_clear(void* _p);
void _knnreport_destroy(void* _p);


/*$ End $*/
#endif

