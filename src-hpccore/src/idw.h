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

#ifndef _idw_h
#define _idw_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "scodes.h"
#include "apserv.h"
#include "ablasf.h"
#include "tsort.h"
#include "nearestneighbor.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "ablasmkl.h"
#include "ablas.h"


/*$ Declarations $*/


/*************************************************************************
Buffer  object  which  is  used  to  perform  evaluation  requests  in  the
multithreaded mode (multiple threads working with same IDW object).

This object should be created with idwcreatecalcbuffer().
*************************************************************************/
typedef struct
{
    ae_vector x;
    ae_vector y;
    ae_vector tsyw;
    ae_vector tsw;
    ae_matrix tsxy;
    ae_vector tsdist;
    kdtreerequestbuffer requestbuffer;
} idwcalcbuffer;


/*************************************************************************
IDW (Inverse Distance Weighting) model object.
*************************************************************************/
typedef struct
{
    ae_int_t nx;
    ae_int_t ny;
    ae_vector globalprior;
    ae_int_t algotype;
    ae_int_t nlayers;
    double r0;
    double rdecay;
    double lambda0;
    double lambdalast;
    double lambdadecay;
    double shepardp;
    kdtree tree;
    ae_int_t npoints;
    ae_vector shepardxy;
    idwcalcbuffer buffer;
} idwmodel;


/*************************************************************************
Builder object used to generate IDW (Inverse Distance Weighting) model.
*************************************************************************/
typedef struct
{
    ae_int_t priortermtype;
    ae_vector priortermval;
    ae_int_t algotype;
    ae_int_t nlayers;
    double r0;
    double rdecay;
    double lambda0;
    double lambdalast;
    double lambdadecay;
    double shepardp;
    ae_vector xy;
    ae_int_t npoints;
    ae_int_t nx;
    ae_int_t ny;
    ae_matrix tmpxy;
    ae_matrix tmplayers;
    ae_vector tmptags;
    ae_vector tmpdist;
    ae_vector tmpx;
    ae_vector tmpwy;
    ae_vector tmpw;
    kdtree tmptree;
    ae_vector tmpmean;
} idwbuilder;


/*************************************************************************
IDW fitting report:
    rmserror        RMS error
    avgerror        average error
    maxerror        maximum error
    r2              coefficient of determination,  R-squared, 1-RSS/TSS
*************************************************************************/
typedef struct
{
    double rmserror;
    double avgerror;
    double maxerror;
    double r2;
} idwreport;


/*$ Body $*/


/*************************************************************************
This function creates buffer  structure  which  can  be  used  to  perform
parallel  IDW  model  evaluations  (with  one  IDW  model  instance  being
used from multiple threads, as long as  different  threads  use  different
instances of buffer).

This buffer object can be used with  idwtscalcbuf()  function  (here  "ts"
stands for "thread-safe", "buf" is a suffix which denotes  function  which
reuses previously allocated output space).

How to use it:
* create IDW model structure or load it from file
* call idwcreatecalcbuffer(), once per thread working with IDW model  (you
  should call this function only AFTER model initialization, see below for
  more information)
* call idwtscalcbuf() from different threads,  with  each  thread  working
  with its own copy of buffer object.

INPUT PARAMETERS
    S           -   IDW model

OUTPUT PARAMETERS
    Buf         -   external buffer.
    
    
IMPORTANT: buffer object should be used only with  IDW model object  which
           was used to initialize buffer. Any attempt to use buffer   with
           different object is dangerous - you may  get  memory  violation
           error because sizes of internal arrays do not fit to dimensions
           of the IDW structure.
           
IMPORTANT: you  should  call  this function only for model which was built
           with model builder (or unserialized from file). Sizes  of  some
           internal structures are determined only after model  is  built,
           so buffer object created before model construction  stage  will
           be useless (and any attempt to use it will result in exception).

  -- ALGLIB --
     Copyright 22.10.2018 by Sergey Bochkanov
*************************************************************************/
void idwcreatecalcbuffer(const idwmodel* s,
     idwcalcbuffer* buf,
     ae_state *_state);


/*************************************************************************
This subroutine creates builder object used  to  generate IDW  model  from
irregularly sampled (scattered) dataset.  Multidimensional  scalar/vector-
-valued are supported.

Builder object is used to fit model to data as follows:
* builder object is created with idwbuildercreate() function
* dataset is added with idwbuildersetpoints() function
* one of the modern IDW algorithms is chosen with either:
  * idwbuildersetalgomstab()            - Multilayer STABilized algorithm (interpolation)
  Alternatively, one of the textbook algorithms can be chosen (not recommended):
  * idwbuildersetalgotextbookshepard()  - textbook Shepard algorithm
  * idwbuildersetalgotextbookmodshepard()-textbook modified Shepard algorithm
* finally, model construction is performed with idwfit() function.

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
    NX  -   dimensionality of the argument, NX>=1
    NY  -   dimensionality of the function being modeled, NY>=1;
            NY=1 corresponds to classic scalar function, NY>=1 corresponds
            to vector-valued function.
    
OUTPUT PARAMETERS:
    State-  builder object

  -- ALGLIB PROJECT --
     Copyright 22.10.2018 by Bochkanov Sergey
*************************************************************************/
void idwbuildercreate(ae_int_t nx,
     ae_int_t ny,
     idwbuilder* state,
     ae_state *_state);


/*************************************************************************
This function changes number of layers used by IDW-MSTAB algorithm.

The more layers you have, the finer details can  be  reproduced  with  IDW
model. The less layers you have, the less memory and CPU time is  consumed
by the model.

Memory consumption grows linearly with layers count,  running  time  grows
sub-linearly.

The default number of layers is 16, which allows you to reproduce  details
at distance down to SRad/65536. You will rarely need to change it.

INPUT PARAMETERS:
    State   -   builder object
    NLayers -   NLayers>=1, the number of layers used by the model.

  -- ALGLIB --
     Copyright 22.10.2018 by Bochkanov Sergey
*************************************************************************/
void idwbuildersetnlayers(idwbuilder* state,
     ae_int_t nlayers,
     ae_state *_state);


/*************************************************************************
This function adds dataset to the builder object.

This function overrides results of the previous calls, i.e. multiple calls
of this function will result in only the last set being added.

INPUT PARAMETERS:
    State   -   builder object
    XY      -   points, array[N,NX+NY]. One row  corresponds to  one point
                in the dataset. First NX elements  are  coordinates,  next
                NY elements are function values. Array may  be larger than 
                specified, in  this  case  only leading [N,NX+NY] elements 
                will be used.
    N       -   number of points in the dataset, N>=0.

  -- ALGLIB --
     Copyright 22.10.2018 by Bochkanov Sergey
*************************************************************************/
void idwbuildersetpoints(idwbuilder* state,
     /* Real    */ const ae_matrix* xy,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
This function sets IDW model  construction  algorithm  to  the  Multilayer
Stabilized IDW method (IDW-MSTAB), a  latest  incarnation  of  the inverse
distance weighting interpolation which fixes shortcomings of  the original
and modified Shepard's variants.

The distinctive features of IDW-MSTAB are:
1) exact interpolation  is  pursued  (as  opposed  to  fitting  and  noise
   suppression)
2) improved robustness when compared with that of other algorithms:
   * MSTAB shows almost no strange  fitting  artifacts  like  ripples  and
     sharp spikes (unlike N-dimensional splines and HRBFs)
   * MSTAB does not return function values far from the  interval  spanned
     by the dataset; say, if all your points have |f|<=1, you  can be sure
     that model value won't deviate too much from [-1,+1]
3) good model construction time competing with that of HRBFs  and  bicubic
   splines
4) ability to work with any number of dimensions, starting from NX=1

The drawbacks of IDW-MSTAB (and all IDW algorithms in general) are:
1) dependence of the model evaluation time on the search radius
2) bad extrapolation properties, models built by this method  are  usually
   conservative in their predictions

Thus, IDW-MSTAB is  a  good  "default"  option  if  you  want  to  perform
scattered multidimensional interpolation. Although it has  its  drawbacks,
it is easy to use and robust, which makes it a good first step.


INPUT PARAMETERS:
    State   -   builder object
    SRad    -   initial search radius, SRad>0 is required. A model  value
                is obtained by "smart" averaging  of  the  dataset  points
                within search radius.

NOTE 1: IDW interpolation can  correctly  handle  ANY  dataset,  including
        datasets with non-distinct points. In case non-distinct points are
        found, an average value for this point will be calculated.
        
NOTE 2: the memory requirements for model storage are O(NPoints*NLayers).
        The model construction needs twice as much memory as model storage.
  
NOTE 3: by default 16 IDW layers are built which is enough for most cases.
        You can change this parameter with idwbuildersetnlayers()  method.
        Larger values may be necessary if you need to reproduce  extrafine
        details at distances smaller than SRad/65536.  Smaller value   may
        be necessary if you have to save memory and  computing  time,  and
        ready to sacrifice some model quality.


ALGORITHM DESCRIPTION
  
ALGLIB implementation of IDW is somewhat similar to the modified Shepard's
method (one with search radius R) but overcomes several of its  drawbacks,
namely:
1) a tendency to show stepwise behavior for uniform datasets
2) a tendency to show terrible interpolation properties for highly
   nonuniform datasets which often arise in geospatial tasks
  (function values are densely sampled across multiple separated
  "tracks")

IDW-MSTAB method performs several passes over dataset and builds a sequence
of progressively refined IDW models  (layers),  which starts from one with
largest search radius SRad  and continues to smaller  search  radii  until
required number of  layers  is  built.  Highest  layers  reproduce  global
behavior of the target function at larger distances  whilst  lower  layers
reproduce fine details at smaller distances.

Each layer is an IDW model built with following modifications:
* weights go to zero when distance approach to the current search radius
* an additional regularizing term is added to the distance: w=1/(d^2+lambda)
* an additional fictional term with unit weight and zero function value is
  added in order to promote continuity  properties  at  the  isolated  and
  boundary points
  
By default, 16 layers is built, which is enough for most  cases.  You  can
change this parameter with idwbuildersetnlayers() method.
   
  -- ALGLIB --
     Copyright 22.10.2018 by Bochkanov Sergey
*************************************************************************/
void idwbuildersetalgomstab(idwbuilder* state,
     double srad,
     ae_state *_state);


/*************************************************************************
This function sets  IDW  model  construction  algorithm  to  the  textbook
Shepard's algorithm with custom (user-specified) power parameter.

IMPORTANT: we do NOT recommend using textbook IDW algorithms because  they
           have terrible interpolation properties. Use MSTAB in all cases.

INPUT PARAMETERS:
    State   -   builder object
    P       -   power parameter, P>0; good value to start with is 2.0

NOTE 1: IDW interpolation can  correctly  handle  ANY  dataset,  including
        datasets with non-distinct points. In case non-distinct points are
        found, an average value for this point will be calculated.
   
  -- ALGLIB --
     Copyright 22.10.2018 by Bochkanov Sergey
*************************************************************************/
void idwbuildersetalgotextbookshepard(idwbuilder* state,
     double p,
     ae_state *_state);


/*************************************************************************
This function sets  IDW  model  construction  algorithm  to the 'textbook'
modified Shepard's algorithm with user-specified search radius.

IMPORTANT: we do NOT recommend using textbook IDW algorithms because  they
           have terrible interpolation properties. Use MSTAB in all cases.

INPUT PARAMETERS:
    State   -   builder object
    R       -   search radius

NOTE 1: IDW interpolation can  correctly  handle  ANY  dataset,  including
        datasets with non-distinct points. In case non-distinct points are
        found, an average value for this point will be calculated.
   
  -- ALGLIB --
     Copyright 22.10.2018 by Bochkanov Sergey
*************************************************************************/
void idwbuildersetalgotextbookmodshepard(idwbuilder* state,
     double r,
     ae_state *_state);


/*************************************************************************
This function sets prior term (model value at infinity) as  user-specified
value.

INPUT PARAMETERS:
    S       -   spline builder
    V       -   value for user-defined prior
    
NOTE: for vector-valued models all components of the prior are set to same
      user-specified value

  -- ALGLIB --
     Copyright 29.10.2018 by Bochkanov Sergey
*************************************************************************/
void idwbuildersetuserterm(idwbuilder* state, double v, ae_state *_state);


/*************************************************************************
This function sets constant prior term (model value at infinity).

Constant prior term is determined as mean value over dataset.

INPUT PARAMETERS:
    S       -   spline builder

  -- ALGLIB --
     Copyright 29.10.2018 by Bochkanov Sergey
*************************************************************************/
void idwbuildersetconstterm(idwbuilder* state, ae_state *_state);


/*************************************************************************
This function sets zero prior term (model value at infinity).

INPUT PARAMETERS:
    S       -   spline builder

  -- ALGLIB --
     Copyright 29.10.2018 by Bochkanov Sergey
*************************************************************************/
void idwbuildersetzeroterm(idwbuilder* state, ae_state *_state);


/*************************************************************************
IDW interpolation: scalar target, 1-dimensional argument

NOTE: this function modifies internal temporaries of the  IDW  model, thus
      IT IS NOT  THREAD-SAFE!  If  you  want  to  perform  parallel  model
      evaluation from the multiple threads, use idwtscalcbuf()  with  per-
      thread buffer object. 
      
INPUT PARAMETERS:
    S   -   IDW interpolant built with IDW builder
    X0  -   argument value

Result:
    IDW interpolant S(X0)

  -- ALGLIB --
     Copyright 22.10.2018 by Bochkanov Sergey
*************************************************************************/
double idwcalc1(idwmodel* s, double x0, ae_state *_state);


/*************************************************************************
IDW interpolation: scalar target, 2-dimensional argument

NOTE: this function modifies internal temporaries of the  IDW  model, thus
      IT IS NOT  THREAD-SAFE!  If  you  want  to  perform  parallel  model
      evaluation from the multiple threads, use idwtscalcbuf()  with  per-
      thread buffer object. 
      
INPUT PARAMETERS:
    S       -   IDW interpolant built with IDW builder
    X0, X1  -   argument value

Result:
    IDW interpolant S(X0,X1)

  -- ALGLIB --
     Copyright 22.10.2018 by Bochkanov Sergey
*************************************************************************/
double idwcalc2(idwmodel* s, double x0, double x1, ae_state *_state);


/*************************************************************************
IDW interpolation: scalar target, 3-dimensional argument

NOTE: this function modifies internal temporaries of the  IDW  model, thus
      IT IS NOT  THREAD-SAFE!  If  you  want  to  perform  parallel  model
      evaluation from the multiple threads, use idwtscalcbuf()  with  per-
      thread buffer object. 

INPUT PARAMETERS:
    S       -   IDW interpolant built with IDW builder
    X0,X1,X2-   argument value

Result:
    IDW interpolant S(X0,X1,X2)

  -- ALGLIB --
     Copyright 22.10.2018 by Bochkanov Sergey
*************************************************************************/
double idwcalc3(idwmodel* s,
     double x0,
     double x1,
     double x2,
     ae_state *_state);


/*************************************************************************
This function calculates values of the IDW model at the given point.

This is general function which can be used for arbitrary NX (dimension  of 
the space of arguments) and NY (dimension of the function itself). However
when  you  have  NY=1  you  may  find more convenient to  use  idwcalc1(),
idwcalc2() or idwcalc3().

NOTE: this function modifies internal temporaries of the  IDW  model, thus
      IT IS NOT  THREAD-SAFE!  If  you  want  to  perform  parallel  model
      evaluation from the multiple threads, use idwtscalcbuf()  with  per-
      thread buffer object. 
      
INPUT PARAMETERS:
    S       -   IDW model
    X       -   coordinates, array[NX]. X may have more than NX  elements,
                in this case only leading NX will be used.

OUTPUT PARAMETERS:
    Y       -   function value, array[NY]. Y is out-parameter and will  be
                reallocated after call to this function. In case you  want
                to reuse previously allocated Y, you may use idwcalcbuf(),
                which reallocates Y only when it is too small.

  -- ALGLIB --
     Copyright 22.10.2018 by Bochkanov Sergey
*************************************************************************/
void idwcalc(idwmodel* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);


/*************************************************************************
This function calculates values of the IDW model at the given point.

Same as idwcalc(), but does not reallocate Y when in is large enough to 
store function values.

NOTE: this function modifies internal temporaries of the  IDW  model, thus
      IT IS NOT  THREAD-SAFE!  If  you  want  to  perform  parallel  model
      evaluation from the multiple threads, use idwtscalcbuf()  with  per-
      thread buffer object. 
      
INPUT PARAMETERS:
    S       -   IDW model
    X       -   coordinates, array[NX]. X may have more than NX  elements,
                in this case only leading NX will be used.
    Y       -   possibly preallocated array

OUTPUT PARAMETERS:
    Y       -   function value, array[NY]. Y is not reallocated when it
                is larger than NY.

  -- ALGLIB --
     Copyright 22.10.2018 by Bochkanov Sergey
*************************************************************************/
void idwcalcbuf(idwmodel* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);


/*************************************************************************
This function calculates values of the IDW model at the given point, using
external  buffer  object  (internal  temporaries  of  IDW  model  are  not
modified).

This function allows to use same IDW model object  in  different  threads,
assuming  that  different   threads  use different instances of the buffer
structure.

INPUT PARAMETERS:
    S       -   IDW model, may be shared between different threads
    Buf     -   buffer object created for this particular instance of  IDW
                model with idwcreatecalcbuffer().
    X       -   coordinates, array[NX]. X may have more than NX  elements,
                in this case only  leading NX will be used.
    Y       -   possibly preallocated array

OUTPUT PARAMETERS:
    Y       -   function value, array[NY]. Y is not reallocated when it
                is larger than NY.

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
void idwtscalcbuf(const idwmodel* s,
     idwcalcbuffer* buf,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);


/*************************************************************************
This function fits IDW model to the dataset using current IDW construction
algorithm. A model being built and fitting report are returned.

INPUT PARAMETERS:
    State   -   builder object

OUTPUT PARAMETERS:
    Model   -   an IDW model built with current algorithm
    Rep     -   model fitting report, fields of this structure contain
                information about average fitting errors.
                
NOTE: although IDW-MSTAB algorithm is an  interpolation  method,  i.e.  it
      tries to fit the model exactly, it can  handle  datasets  with  non-
      distinct points which can not be fit exactly; in such  cases  least-
      squares fitting is performed.
   
  -- ALGLIB --
     Copyright 22.10.2018 by Bochkanov Sergey
*************************************************************************/
void idwfit(idwbuilder* state,
     idwmodel* model,
     idwreport* rep,
     ae_state *_state);


/*************************************************************************
Serializer: allocation

  -- ALGLIB --
     Copyright 28.02.2018 by Bochkanov Sergey
*************************************************************************/
void idwalloc(ae_serializer* s, const idwmodel* model, ae_state *_state);


/*************************************************************************
Serializer: serialization

  -- ALGLIB --
     Copyright 28.02.2018 by Bochkanov Sergey
*************************************************************************/
void idwserialize(ae_serializer* s,
     const idwmodel* model,
     ae_state *_state);


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 28.02.2018 by Bochkanov Sergey
*************************************************************************/
void idwunserialize(ae_serializer* s, idwmodel* model, ae_state *_state);
void _idwcalcbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _idwcalcbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _idwcalcbuffer_clear(void* _p);
void _idwcalcbuffer_destroy(void* _p);
void _idwmodel_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _idwmodel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _idwmodel_clear(void* _p);
void _idwmodel_destroy(void* _p);
void _idwbuilder_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _idwbuilder_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _idwbuilder_clear(void* _p);
void _idwbuilder_destroy(void* _p);
void _idwreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _idwreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _idwreport_clear(void* _p);
void _idwreport_destroy(void* _p);


/*$ End $*/
#endif

