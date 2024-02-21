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
#include "idw.h"


/*$ Declarations $*/
static double idw_w0 = 1.0;
static double idw_meps = 1.0E-50;
static ae_int_t idw_defaultnlayers = 16;
static double idw_defaultlambda0 = 0.3333;
static ae_int_t idw_idwbatchsize = 128;
static void idw_idwgridcalc2rec(const idwmodel* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t begin0,
     ae_int_t end0,
     ae_int_t n0,
     /* Real    */ const ae_vector* x1,
     ae_int_t begin1,
     ae_int_t end1,
     ae_int_t n1,
     /* Boolean */ const ae_vector* flagy,
     ae_bool sparsey,
     ae_shared_pool* cbpool,
     ae_bool isrootcall,
     double evalcost,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void _spawn_idw_idwgridcalc2rec(const idwmodel* s,
    /* Real    */ const ae_vector* x0,
    ae_int_t begin0,
    ae_int_t end0,
    ae_int_t n0,
    /* Real    */ const ae_vector* x1,
    ae_int_t begin1,
    ae_int_t end1,
    ae_int_t n1,
    /* Boolean */ const ae_vector* flagy,
    ae_bool sparsey,
    ae_shared_pool* cbpool,
    ae_bool isrootcall,
    double evalcost,
    /* Real    */ ae_vector* y, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_idw_idwgridcalc2rec(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_idw_idwgridcalc2rec(const idwmodel* s,
    /* Real    */ const ae_vector* x0,
    ae_int_t begin0,
    ae_int_t end0,
    ae_int_t n0,
    /* Real    */ const ae_vector* x1,
    ae_int_t begin1,
    ae_int_t end1,
    ae_int_t n1,
    /* Boolean */ const ae_vector* flagy,
    ae_bool sparsey,
    ae_shared_pool* cbpool,
    ae_bool isrootcall,
    double evalcost,
    /* Real    */ ae_vector* y, ae_state *_state);
static void idw_errormetricsviacalc(idwbuilder* state,
     idwmodel* model,
     idwreport* rep,
     ae_state *_state);
static void idw_mstabrec(idwbuilder* state,
     double rcur,
     double lambdacur,
     ae_int_t layeridx,
     ae_shared_pool* mbpool,
     ae_int_t idx0,
     ae_int_t idx1,
     ae_bool isrootcall,
     double querycost,
     double totalcost,
     /* Real    */ ae_matrix* layers,
     ae_state *_state);
void _spawn_idw_mstabrec(idwbuilder* state,
    double rcur,
    double lambdacur,
    ae_int_t layeridx,
    ae_shared_pool* mbpool,
    ae_int_t idx0,
    ae_int_t idx1,
    ae_bool isrootcall,
    double querycost,
    double totalcost,
    /* Real    */ ae_matrix* layers, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_idw_mstabrec(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_idw_mstabrec(idwbuilder* state,
    double rcur,
    double lambdacur,
    ae_int_t layeridx,
    ae_shared_pool* mbpool,
    ae_int_t idx0,
    ae_int_t idx1,
    ae_bool isrootcall,
    double querycost,
    double totalcost,
    /* Real    */ ae_matrix* layers, ae_state *_state);
static void idw_mstabbasecase(idwbuilder* state,
     double rcur,
     double lambdacur,
     ae_int_t layeridx,
     mstabbuffer* buf,
     ae_int_t idx0,
     ae_int_t idx1,
     double querycost,
     double totalcost,
     /* Real    */ ae_matrix* layers,
     ae_state *_state);


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
     ae_state *_state)
{

    _idwcalcbuffer_clear(buf);

    ae_assert(s->nx>=1, "IDWCreateCalcBuffer: integrity check failed", _state);
    ae_assert(s->ny>=1, "IDWCreateCalcBuffer: integrity check failed", _state);
    ae_assert(s->nlayers>=0, "IDWCreateCalcBuffer: integrity check failed", _state);
    ae_assert(s->algotype>=0, "IDWCreateCalcBuffer: integrity check failed", _state);
    if( s->nlayers>=1&&s->algotype!=0 )
    {
        kdtreecreaterequestbuffer(&s->tree, &buf->requestbuffer, _state);
    }
    rvectorsetlengthatleast(&buf->x, s->nx, _state);
    rvectorsetlengthatleast(&buf->y, s->ny, _state);
    rvectorsetlengthatleast(&buf->tsyw, s->ny*ae_maxint(s->nlayers, 1, _state), _state);
    rvectorsetlengthatleast(&buf->tsw, ae_maxint(s->nlayers, 1, _state), _state);
}


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
     ae_state *_state)
{

    _idwbuilder_clear(state);

    ae_assert(nx>=1, "IDWBuilderCreate: NX<=0", _state);
    ae_assert(ny>=1, "IDWBuilderCreate: NY<=0", _state);
    
    /*
     * We choose reasonable defaults for the algorithm:
     * * MSTAB algorithm
     * * 12 layers
     * * default radius
     * * default Lambda0
     */
    state->algotype = 2;
    state->priortermtype = 2;
    rvectorsetlengthatleast(&state->priortermval, ny, _state);
    state->nlayers = idw_defaultnlayers;
    state->r0 = (double)(0);
    state->rdecay = 0.5;
    state->lambda0 = idw_defaultlambda0;
    state->lambdalast = (double)(0);
    state->lambdadecay = 1.0;
    state->debugprofile = ae_false;
    
    /*
     * Other parameters, not used but initialized
     */
    state->shepardp = (double)(0);
    
    /*
     * Initial dataset is empty
     */
    state->npoints = 0;
    state->nx = nx;
    state->ny = ny;
    
    /*
     * Initial progress is zero
     */
    state->mprogress = (double)(0);
}


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
     ae_state *_state)
{


    ae_assert(nlayers>=1, "IDWBuilderSetNLayers: N<1", _state);
    state->nlayers = nlayers;
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t ew;


    ae_assert(n>=0, "IDWBuilderSetPoints: N<0", _state);
    ae_assert(xy->rows>=n, "IDWBuilderSetPoints: Rows(XY)<N", _state);
    ae_assert(n==0||xy->cols>=state->nx+state->ny, "IDWBuilderSetPoints: Cols(XY)<NX+NY", _state);
    ae_assert(apservisfinitematrix(xy, n, state->nx+state->ny, _state), "IDWBuilderSetPoints: XY contains infinite or NaN values!", _state);
    state->npoints = n;
    ew = state->nx+state->ny;
    rvectorsetlengthatleast(&state->xy, n*ew, _state);
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=ew-1; j++)
        {
            state->xy.ptr.p_double[i*ew+j] = xy->ptr.pp_double[i][j];
        }
    }
}


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
     ae_state *_state)
{


    ae_assert(ae_isfinite(srad, _state), "IDWBuilderSetAlgoMSTAB: SRad is not finite", _state);
    ae_assert(ae_fp_greater(srad,(double)(0)), "IDWBuilderSetAlgoMSTAB: SRad<=0", _state);
    
    /*
     * Set algorithm
     */
    state->algotype = 2;
    
    /*
     * Set options
     */
    state->r0 = srad;
    state->rdecay = 0.5;
    state->lambda0 = idw_defaultlambda0;
    state->lambdalast = (double)(0);
    state->lambdadecay = 1.0;
}


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
     ae_state *_state)
{


    ae_assert(ae_isfinite(p, _state), "IDWBuilderSetAlgoShepard: P is not finite", _state);
    ae_assert(ae_fp_greater(p,(double)(0)), "IDWBuilderSetAlgoShepard: P<=0", _state);
    
    /*
     * Set algorithm and options
     */
    state->algotype = 0;
    state->shepardp = p;
}


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
     ae_state *_state)
{


    ae_assert(ae_isfinite(r, _state), "IDWBuilderSetAlgoModShepard: R is not finite", _state);
    ae_assert(ae_fp_greater(r,(double)(0)), "IDWBuilderSetAlgoModShepard: R<=0", _state);
    
    /*
     * Set algorithm and options
     */
    state->algotype = 1;
    state->r0 = r;
}


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
void idwbuildersetuserterm(idwbuilder* state, double v, ae_state *_state)
{
    ae_int_t j;


    ae_assert(ae_isfinite(v, _state), "IDWBuilderSetUserTerm: infinite/NAN value passed", _state);
    state->priortermtype = 0;
    for(j=0; j<=state->ny-1; j++)
    {
        state->priortermval.ptr.p_double[j] = v;
    }
}


/*************************************************************************
This function sets constant prior term (model value at infinity).

Constant prior term is determined as mean value over dataset.

INPUT PARAMETERS:
    S       -   spline builder

  -- ALGLIB --
     Copyright 29.10.2018 by Bochkanov Sergey
*************************************************************************/
void idwbuildersetconstterm(idwbuilder* state, ae_state *_state)
{


    state->priortermtype = 2;
}


/*************************************************************************
This function sets zero prior term (model value at infinity).

INPUT PARAMETERS:
    S       -   spline builder

  -- ALGLIB --
     Copyright 29.10.2018 by Bochkanov Sergey
*************************************************************************/
void idwbuildersetzeroterm(idwbuilder* state, ae_state *_state)
{


    state->priortermtype = 3;
}


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
double idwcalc1(idwmodel* s, double x0, ae_state *_state)
{
    double result;


    ae_assert(s->nx==1, "IDWCalc1: S.NX<>1", _state);
    ae_assert(s->ny==1, "IDWCalc1: S.NY<>1", _state);
    ae_assert(ae_isfinite(x0, _state), "IDWCalc1: X0 is INF or NAN", _state);
    s->buffer.x.ptr.p_double[0] = x0;
    idwtscalcbuf(s, &s->buffer, &s->buffer.x, &s->buffer.y, _state);
    result = s->buffer.y.ptr.p_double[0];
    return result;
}


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
double idwcalc2(idwmodel* s, double x0, double x1, ae_state *_state)
{
    double result;


    ae_assert(s->nx==2, "IDWCalc2: S.NX<>2", _state);
    ae_assert(s->ny==1, "IDWCalc2: S.NY<>1", _state);
    ae_assert(ae_isfinite(x0, _state), "IDWCalc2: X0 is INF or NAN", _state);
    ae_assert(ae_isfinite(x1, _state), "IDWCalc2: X1 is INF or NAN", _state);
    s->buffer.x.ptr.p_double[0] = x0;
    s->buffer.x.ptr.p_double[1] = x1;
    idwtscalcbuf(s, &s->buffer, &s->buffer.x, &s->buffer.y, _state);
    result = s->buffer.y.ptr.p_double[0];
    return result;
}


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
     ae_state *_state)
{
    double result;


    ae_assert(s->nx==3, "IDWCalc3: S.NX<>3", _state);
    ae_assert(s->ny==1, "IDWCalc3: S.NY<>1", _state);
    ae_assert(ae_isfinite(x0, _state), "IDWCalc3: X0 is INF or NAN", _state);
    ae_assert(ae_isfinite(x1, _state), "IDWCalc3: X1 is INF or NAN", _state);
    ae_assert(ae_isfinite(x2, _state), "IDWCalc3: X2 is INF or NAN", _state);
    s->buffer.x.ptr.p_double[0] = x0;
    s->buffer.x.ptr.p_double[1] = x1;
    s->buffer.x.ptr.p_double[2] = x2;
    idwtscalcbuf(s, &s->buffer, &s->buffer.x, &s->buffer.y, _state);
    result = s->buffer.y.ptr.p_double[0];
    return result;
}


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
     ae_state *_state)
{

    ae_vector_clear(y);

    idwtscalcbuf(s, &s->buffer, x, y, _state);
}


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
     ae_state *_state)
{


    idwtscalcbuf(s, &s->buffer, x, y, _state);
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t ew;
    ae_int_t k;
    ae_int_t layeridx;
    ae_int_t nx;
    ae_int_t ny;
    ae_int_t npoints;
    double v;
    double vv;
    double f;
    double p;
    double r;
    double eps;
    double lambdacur;
    double lambdadecay;
    double invrdecay;
    double invr;
    ae_bool fastcalcpossible;
    double wf0;
    double ws0;
    double wf1;
    double ws1;


    nx = s->nx;
    ny = s->ny;
    ae_assert(x->cnt>=nx, "IDWTsCalcBuf: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, nx, _state), "IDWTsCalcBuf: X contains infinite or NaN values", _state);
    
    /*
     * Avoid spurious compiler warnings
     */
    wf0 = (double)(0);
    ws0 = (double)(0);
    wf1 = (double)(0);
    ws1 = (double)(0);
    
    /*
     * Allocate output
     */
    if( y->cnt<ny )
    {
        ae_vector_set_length(y, ny, _state);
    }
    
    /*
     * Quick exit for NLayers=0 (no dataset)
     */
    if( s->nlayers==0 )
    {
        for(j=0; j<=ny-1; j++)
        {
            y->ptr.p_double[j] = s->globalprior.ptr.p_double[j];
        }
        return;
    }
    
    /*
     * Textbook Shepard's method
     */
    if( s->algotype==0 )
    {
        npoints = s->npoints;
        ae_assert(npoints>0, "IDWTsCalcBuf: integrity check failed", _state);
        eps = 1.0E-50;
        ew = nx+ny;
        p = s->shepardp;
        for(j=0; j<=ny-1; j++)
        {
            y->ptr.p_double[j] = (double)(0);
            buf->tsyw.ptr.p_double[j] = eps;
        }
        for(i=0; i<=npoints-1; i++)
        {
            
            /*
             * Compute squared distance
             */
            v = (double)(0);
            for(j=0; j<=nx-1; j++)
            {
                vv = s->shepardxy.ptr.p_double[i*ew+j]-x->ptr.p_double[j];
                v = v+vv*vv;
            }
            
            /*
             * Compute weight (with small regularizing addition)
             */
            v = ae_pow(v, p*0.5, _state);
            v = (double)1/(eps+v);
            
            /*
             * Accumulate
             */
            for(j=0; j<=ny-1; j++)
            {
                y->ptr.p_double[j] = y->ptr.p_double[j]+v*s->shepardxy.ptr.p_double[i*ew+nx+j];
                buf->tsyw.ptr.p_double[j] = buf->tsyw.ptr.p_double[j]+v;
            }
        }
        for(j=0; j<=ny-1; j++)
        {
            y->ptr.p_double[j] = y->ptr.p_double[j]/buf->tsyw.ptr.p_double[j]+s->globalprior.ptr.p_double[j];
        }
        return;
    }
    
    /*
     * Textbook modified Shepard's method
     */
    if( s->algotype==1 )
    {
        eps = 1.0E-50;
        r = s->r0;
        for(j=0; j<=ny-1; j++)
        {
            y->ptr.p_double[j] = (double)(0);
            buf->tsyw.ptr.p_double[j] = eps;
        }
        k = kdtreetsqueryrnn(&s->tree, &buf->requestbuffer, x, r, ae_true, _state);
        kdtreetsqueryresultsxy(&s->tree, &buf->requestbuffer, &buf->tsxy, _state);
        kdtreetsqueryresultsdistances(&s->tree, &buf->requestbuffer, &buf->tsdist, _state);
        for(i=0; i<=k-1; i++)
        {
            v = buf->tsdist.ptr.p_double[i];
            v = (r-v)/(r*v+eps);
            v = v*v;
            for(j=0; j<=ny-1; j++)
            {
                y->ptr.p_double[j] = y->ptr.p_double[j]+v*buf->tsxy.ptr.pp_double[i][nx+j];
                buf->tsyw.ptr.p_double[j] = buf->tsyw.ptr.p_double[j]+v;
            }
        }
        for(j=0; j<=ny-1; j++)
        {
            y->ptr.p_double[j] = y->ptr.p_double[j]/buf->tsyw.ptr.p_double[j]+s->globalprior.ptr.p_double[j];
        }
        return;
    }
    
    /*
     * MSTAB
     */
    if( s->algotype==2 )
    {
        ae_assert(ae_fp_eq(idw_w0,(double)(1)), "IDWTsCalcBuf: unexpected W0, integrity check failed", _state);
        invrdecay = (double)1/s->rdecay;
        invr = (double)1/s->r0;
        lambdadecay = s->lambdadecay;
        fastcalcpossible = (ny==1&&s->nlayers>=3)&&ae_fp_eq(lambdadecay,(double)(1));
        if( fastcalcpossible )
        {
            
            /*
             * Important special case, NY=1, no lambda-decay,
             * we can perform optimized fast evaluation
             */
            wf0 = (double)(0);
            ws0 = idw_w0;
            wf1 = (double)(0);
            ws1 = idw_w0;
            for(j=0; j<=s->nlayers-1; j++)
            {
                buf->tsyw.ptr.p_double[j] = (double)(0);
                buf->tsw.ptr.p_double[j] = idw_w0;
            }
        }
        else
        {
            
            /*
             * Setup variables for generic evaluation path
             */
            for(j=0; j<=ny*s->nlayers-1; j++)
            {
                buf->tsyw.ptr.p_double[j] = (double)(0);
            }
            for(j=0; j<=s->nlayers-1; j++)
            {
                buf->tsw.ptr.p_double[j] = idw_w0;
            }
        }
        k = kdtreetsqueryrnnu(&s->tree, &buf->requestbuffer, x, s->r0, ae_true, _state);
        kdtreetsqueryresultsxy(&s->tree, &buf->requestbuffer, &buf->tsxy, _state);
        kdtreetsqueryresultsdistances(&s->tree, &buf->requestbuffer, &buf->tsdist, _state);
        for(i=0; i<=k-1; i++)
        {
            lambdacur = s->lambda0;
            vv = buf->tsdist.ptr.p_double[i]*invr;
            if( fastcalcpossible )
            {
                
                /*
                 * Important special case, fast evaluation possible
                 */
                v = vv*vv;
                v = ((double)1-v)*((double)1-v)/(v+lambdacur);
                f = buf->tsxy.ptr.pp_double[i][nx+0];
                wf0 = wf0+v*f;
                ws0 = ws0+v;
                vv = vv*invrdecay;
                if( vv>=1.0 )
                {
                    continue;
                }
                v = vv*vv;
                v = ((double)1-v)*((double)1-v)/(v+lambdacur);
                f = buf->tsxy.ptr.pp_double[i][nx+1];
                wf1 = wf1+v*f;
                ws1 = ws1+v;
                vv = vv*invrdecay;
                if( vv>=1.0 )
                {
                    continue;
                }
                for(layeridx=2; layeridx<=s->nlayers-1; layeridx++)
                {
                    if( layeridx==s->nlayers-1 )
                    {
                        lambdacur = s->lambdalast;
                    }
                    v = vv*vv;
                    v = ((double)1-v)*((double)1-v)/(v+lambdacur);
                    f = buf->tsxy.ptr.pp_double[i][nx+layeridx];
                    buf->tsyw.ptr.p_double[layeridx] = buf->tsyw.ptr.p_double[layeridx]+v*f;
                    buf->tsw.ptr.p_double[layeridx] = buf->tsw.ptr.p_double[layeridx]+v;
                    vv = vv*invrdecay;
                    if( vv>=1.0 )
                    {
                        break;
                    }
                }
            }
            else
            {
                
                /*
                 * General case
                 */
                for(layeridx=0; layeridx<=s->nlayers-1; layeridx++)
                {
                    if( layeridx==s->nlayers-1 )
                    {
                        lambdacur = s->lambdalast;
                    }
                    if( vv>=1.0 )
                    {
                        break;
                    }
                    v = vv*vv;
                    v = ((double)1-v)*((double)1-v)/(v+lambdacur);
                    for(j=0; j<=ny-1; j++)
                    {
                        f = buf->tsxy.ptr.pp_double[i][nx+layeridx*ny+j];
                        buf->tsyw.ptr.p_double[layeridx*ny+j] = buf->tsyw.ptr.p_double[layeridx*ny+j]+v*f;
                    }
                    buf->tsw.ptr.p_double[layeridx] = buf->tsw.ptr.p_double[layeridx]+v;
                    lambdacur = lambdacur*lambdadecay;
                    vv = vv*invrdecay;
                }
            }
        }
        if( fastcalcpossible )
        {
            
            /*
             * Important special case, finalize evaluations
             */
            buf->tsyw.ptr.p_double[0] = wf0;
            buf->tsw.ptr.p_double[0] = ws0;
            buf->tsyw.ptr.p_double[1] = wf1;
            buf->tsw.ptr.p_double[1] = ws1;
        }
        for(j=0; j<=ny-1; j++)
        {
            y->ptr.p_double[j] = s->globalprior.ptr.p_double[j];
        }
        for(layeridx=0; layeridx<=s->nlayers-1; layeridx++)
        {
            for(j=0; j<=ny-1; j++)
            {
                y->ptr.p_double[j] = y->ptr.p_double[j]+buf->tsyw.ptr.p_double[layeridx*ny+j]/buf->tsw.ptr.p_double[layeridx];
            }
        }
        return;
    }
    
    /*
     *
     */
    ae_assert(ae_false, "IDWTsCalcBuf: unexpected AlgoType", _state);
}


/*************************************************************************
This function fits IDW model to the dataset using current IDW construction
algorithm. A model being built and fitting report are returned.

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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t layeridx;
    double v;
    ae_int_t npoints;
    ae_int_t nx;
    ae_int_t ny;
    double rss;
    double tss;
    mstabbuffer mbuffer;
    ae_shared_pool mbpool;
    ae_vector layerrad;
    ae_vector layerlambda;
    ae_vector querycosts;
    double totalcost;

    ae_frame_make(_state, &_frame_block);
    memset(&mbuffer, 0, sizeof(mbuffer));
    memset(&mbpool, 0, sizeof(mbpool));
    memset(&layerrad, 0, sizeof(layerrad));
    memset(&layerlambda, 0, sizeof(layerlambda));
    memset(&querycosts, 0, sizeof(querycosts));
    _idwmodel_clear(model);
    _idwreport_clear(rep);
    _mstabbuffer_init(&mbuffer, _state, ae_true);
    ae_shared_pool_init(&mbpool, _state, ae_true);
    ae_vector_init(&layerrad, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&layerlambda, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&querycosts, 0, DT_REAL, _state, ae_true);

    nx = state->nx;
    ny = state->ny;
    npoints = state->npoints;
    
    /*
     * Clear report fields
     */
    rep->rmserror = (double)(0);
    rep->avgerror = (double)(0);
    rep->maxerror = (double)(0);
    rep->r2 = 1.0;
    state->mprogress = (double)(0);
    
    /*
     * Quick exit for empty dataset
     */
    if( state->npoints==0 )
    {
        model->nx = nx;
        model->ny = ny;
        ae_vector_set_length(&model->globalprior, ny, _state);
        for(i=0; i<=ny-1; i++)
        {
            model->globalprior.ptr.p_double[i] = (double)(0);
        }
        model->algotype = 0;
        model->nlayers = 0;
        model->r0 = (double)(1);
        model->rdecay = 0.5;
        model->lambda0 = (double)(0);
        model->lambdalast = (double)(0);
        model->lambdadecay = (double)(1);
        model->shepardp = (double)(2);
        model->npoints = 0;
        model->debugprofile = ae_false;
        idwcreatecalcbuffer(model, &model->buffer, _state);
        state->mprogress = 1.0;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Compute temporaries which will be required later:
     * * global mean
     */
    ae_assert(state->npoints>0, "IDWFit: integrity check failed", _state);
    rvectorsetlengthatleast(&state->tmpmean, ny, _state);
    for(j=0; j<=ny-1; j++)
    {
        state->tmpmean.ptr.p_double[j] = (double)(0);
    }
    for(i=0; i<=npoints-1; i++)
    {
        for(j=0; j<=ny-1; j++)
        {
            state->tmpmean.ptr.p_double[j] = state->tmpmean.ptr.p_double[j]+state->xy.ptr.p_double[i*(nx+ny)+nx+j];
        }
    }
    for(j=0; j<=ny-1; j++)
    {
        state->tmpmean.ptr.p_double[j] = state->tmpmean.ptr.p_double[j]/(double)npoints;
    }
    
    /*
     * Compute global prior
     *
     * NOTE: for original Shepard's method it is always mean value
     */
    rvectorsetlengthatleast(&model->globalprior, ny, _state);
    for(j=0; j<=ny-1; j++)
    {
        model->globalprior.ptr.p_double[j] = state->tmpmean.ptr.p_double[j];
    }
    if( state->algotype!=0 )
    {
        
        /*
         * Algorithm is set to one of the "advanced" versions with search
         * radius which can handle non-mean prior term
         */
        if( state->priortermtype==0 )
        {
            
            /*
             * User-specified prior
             */
            for(j=0; j<=ny-1; j++)
            {
                model->globalprior.ptr.p_double[j] = state->priortermval.ptr.p_double[j];
            }
        }
        if( state->priortermtype==3 )
        {
            
            /*
             * Zero prior
             */
            for(j=0; j<=ny-1; j++)
            {
                model->globalprior.ptr.p_double[j] = (double)(0);
            }
        }
    }
    
    /*
     * Textbook Shepard
     */
    if( state->algotype==0 )
    {
        
        /*
         * Initialize model
         */
        model->algotype = 0;
        model->nx = nx;
        model->ny = ny;
        model->nlayers = 1;
        model->r0 = (double)(1);
        model->rdecay = 0.5;
        model->lambda0 = (double)(0);
        model->lambdalast = (double)(0);
        model->lambdadecay = (double)(1);
        model->shepardp = state->shepardp;
        model->debugprofile = ae_false;
        
        /*
         * Copy dataset
         */
        rvectorsetlengthatleast(&model->shepardxy, npoints*(nx+ny), _state);
        for(i=0; i<=npoints-1; i++)
        {
            for(j=0; j<=nx-1; j++)
            {
                model->shepardxy.ptr.p_double[i*(nx+ny)+j] = state->xy.ptr.p_double[i*(nx+ny)+j];
            }
            for(j=0; j<=ny-1; j++)
            {
                model->shepardxy.ptr.p_double[i*(nx+ny)+nx+j] = state->xy.ptr.p_double[i*(nx+ny)+nx+j]-model->globalprior.ptr.p_double[j];
            }
        }
        model->npoints = npoints;
        
        /*
         * Prepare internal buffer
         * Evaluate report fields
         */
        idwcreatecalcbuffer(model, &model->buffer, _state);
        idw_errormetricsviacalc(state, model, rep, _state);
        state->mprogress = 1.0;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Textbook modified Shepard's method
     */
    if( state->algotype==1 )
    {
        
        /*
         * Initialize model
         */
        model->algotype = 1;
        model->nx = nx;
        model->ny = ny;
        model->nlayers = 1;
        model->r0 = state->r0;
        model->rdecay = (double)(1);
        model->lambda0 = (double)(0);
        model->lambdalast = (double)(0);
        model->lambdadecay = (double)(1);
        model->shepardp = (double)(0);
        model->debugprofile = ae_false;
        
        /*
         * Build kd-tree search structure
         */
        rmatrixsetlengthatleast(&state->tmpxy, npoints, nx+ny, _state);
        for(i=0; i<=npoints-1; i++)
        {
            for(j=0; j<=nx-1; j++)
            {
                state->tmpxy.ptr.pp_double[i][j] = state->xy.ptr.p_double[i*(nx+ny)+j];
            }
            for(j=0; j<=ny-1; j++)
            {
                state->tmpxy.ptr.pp_double[i][nx+j] = state->xy.ptr.p_double[i*(nx+ny)+nx+j]-model->globalprior.ptr.p_double[j];
            }
        }
        kdtreebuild(&state->tmpxy, npoints, nx, ny, 2, &model->tree, _state);
        
        /*
         * Prepare internal buffer
         * Evaluate report fields
         */
        idwcreatecalcbuffer(model, &model->buffer, _state);
        idw_errormetricsviacalc(state, model, rep, _state);
        state->mprogress = 1.0;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * MSTAB algorithm
     */
    if( state->algotype==2 )
    {
        ae_assert(state->nlayers>=1, "IDWFit: integrity check failed", _state);
        
        /*
         * Initialize model
         */
        model->algotype = 2;
        model->nx = nx;
        model->ny = ny;
        model->nlayers = state->nlayers;
        model->r0 = state->r0;
        model->rdecay = 0.5;
        model->lambda0 = state->lambda0;
        model->lambdadecay = 1.0;
        model->lambdalast = idw_meps;
        model->shepardp = (double)(0);
        model->debugprofile = ae_false;
        
        /*
         * Build kd-tree search structure,
         * prepare input residuals for the first layer of the model
         */
        rmatrixsetlengthatleast(&state->tmpxy, npoints, nx, _state);
        rmatrixsetlengthatleast(&state->tmplayers, npoints, nx+ny*(state->nlayers+1), _state);
        ivectorsetlengthatleast(&state->tmptags, npoints, _state);
        for(i=0; i<=npoints-1; i++)
        {
            for(j=0; j<=nx-1; j++)
            {
                v = state->xy.ptr.p_double[i*(nx+ny)+j];
                state->tmpxy.ptr.pp_double[i][j] = v;
                state->tmplayers.ptr.pp_double[i][j] = v;
            }
            state->tmptags.ptr.p_int[i] = i;
            for(j=0; j<=ny-1; j++)
            {
                state->tmplayers.ptr.pp_double[i][nx+j] = state->xy.ptr.p_double[i*(nx+ny)+nx+j]-model->globalprior.ptr.p_double[j];
            }
        }
        kdtreebuildtagged(&state->tmpxy, &state->tmptags, npoints, nx, 0, 2, &state->tmptree, _state);
        
        /*
         * Evaluate per-layer parameters:
         * * query radius R
         * * smoothing parameter Lambda
         * * cost of a R-nn query (the biggest part of the model evaluation)
         *
         * Set global fields depending on the profile and cost of computations.
         */
        rallocv(state->nlayers, &layerrad, _state);
        rallocv(state->nlayers, &layerlambda, _state);
        rallocv(state->nlayers, &querycosts, _state);
        totalcost = (double)(0);
        for(layeridx=0; layeridx<=state->nlayers-1; layeridx++)
        {
            
            /*
             * Determine layer metrics
             */
            layerrad.ptr.p_double[layeridx] = model->r0*ae_pow(model->rdecay, (double)(layeridx), _state);
            layerlambda.ptr.p_double[layeridx] = model->lambda0*ae_pow(model->lambdadecay, (double)(layeridx), _state);
            if( layeridx==state->nlayers-1 )
            {
                layerlambda.ptr.p_double[layeridx] = model->lambdalast;
            }
            querycosts.ptr.p_double[layeridx] = kdtreeapproxrnnquerycost(&state->tmptree, layerrad.ptr.p_double[layeridx], _state);
            totalcost = totalcost+querycosts.ptr.p_double[layeridx]*(double)npoints;
        }
        if( state->debugprofile )
        {
            
            /*
             * Use debug profile for the model construction
             */
            state->mbatchsize = 1+ae_randominteger(3, _state);
        }
        else
        {
            
            /*
             * Use performance profile for the model construction
             */
            state->mbatchsize = idw_idwbatchsize;
        }
        
        /*
         * Prepare temporary buffer for MSTABBasecase
         */
        rallocv(nx, &mbuffer.x, _state);
        rallocv(ny, &mbuffer.w, _state);
        rallocv(ny, &mbuffer.wy, _state);
        kdtreecreaterequestbuffer(&state->tmptree, &mbuffer.requestbuffer, _state);
        
        /*
         * Iteratively build layer by layer
         */
        for(layeridx=0; layeridx<=state->nlayers-1; layeridx++)
        {
            
            /*
             * For each point compute residual from fitting with current layer
             */
            if( npoints>state->mbatchsize&&(ae_fp_greater((double)npoints*querycosts.ptr.p_double[layeridx],smpactivationlevel(_state))||state->debugprofile) )
            {
                ae_shared_pool_set_seed(&mbpool, &mbuffer, (ae_int_t)sizeof(mbuffer), (ae_constructor)_mstabbuffer_init, (ae_copy_constructor)_mstabbuffer_init_copy, (ae_destructor)_mstabbuffer_destroy, _state);
                idw_mstabrec(state, layerrad.ptr.p_double[layeridx], layerlambda.ptr.p_double[layeridx], layeridx, &mbpool, 0, npoints, ae_true, querycosts.ptr.p_double[layeridx], totalcost, &state->tmplayers, _state);
            }
            else
            {
                idw_mstabbasecase(state, layerrad.ptr.p_double[layeridx], layerlambda.ptr.p_double[layeridx], layeridx, &mbuffer, 0, npoints, querycosts.ptr.p_double[layeridx], totalcost, &state->tmplayers, _state);
            }
        }
        kdtreebuild(&state->tmplayers, npoints, nx, ny*state->nlayers, 2, &model->tree, _state);
        
        /*
         * Evaluate report fields
         */
        rep->rmserror = (double)(0);
        rep->avgerror = (double)(0);
        rep->maxerror = (double)(0);
        rss = (double)(0);
        tss = (double)(0);
        for(i=0; i<=npoints-1; i++)
        {
            for(j=0; j<=ny-1; j++)
            {
                v = ae_fabs(state->tmplayers.ptr.pp_double[i][nx+state->nlayers*ny+j], _state);
                rep->rmserror = rep->rmserror+v*v;
                rep->avgerror = rep->avgerror+v;
                rep->maxerror = ae_maxreal(rep->maxerror, ae_fabs(v, _state), _state);
                rss = rss+v*v;
                tss = tss+ae_sqr(state->xy.ptr.p_double[i*(nx+ny)+nx+j]-state->tmpmean.ptr.p_double[j], _state);
            }
        }
        rep->rmserror = ae_sqrt(rep->rmserror/(double)(npoints*ny), _state);
        rep->avgerror = rep->avgerror/(double)(npoints*ny);
        rep->r2 = 1.0-rss/coalesce(tss, 1.0, _state);
        state->mprogress = 1.0;
        
        /*
         * Prepare internal buffer
         */
        idwcreatecalcbuffer(model, &model->buffer, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Unknown algorithm
     */
    ae_assert(ae_false, "IDWFit: integrity check failed, unexpected algorithm", _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function is used to peek into the IDW construction  process from some
other thread and get the progress indicator. It returns value in [0,1].

IMPORTANT: only MSTAB algorithm supports peeking into progress  indicator.
           Legacy versions of the Shepard's method do  not support it. You
           will always get zero as the result.

INPUT PARAMETERS:
    S           -   RBF model object

RESULT:
    progress value, in [0,1]

  -- ALGLIB --
     Copyright 27.11.2023 by Bochkanov Sergey
*************************************************************************/
double idwpeekprogress(const idwbuilder* s, ae_state *_state)
{
    double result;


    result = s->mprogress;
    return result;
}


/*************************************************************************
This function calculates values  of  an  IDW  model  at  a  regular  grid,
which  has  N0*N1 points, with Point[I,J] = (X0[I], X1[J]).  Vector-valued
IDW models are supported.

This function returns 0.0 when:
* the model is not initialized
* NX<>2

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

NOTE: Parallel  processing  is  implemented only for modern (MSTAB) IDW's.

INPUT PARAMETERS:
    S       -   IDW model, used in read-only mode, can be  shared  between
                multiple   invocations  of  this  function  from  multiple
                threads.
    
    X0      -   array of grid nodes, first coordinates, array[N0].
                Must be ordered by ascending. Exception is generated
                if the array is not correctly ordered.
    N0      -   grid size (number of nodes) in the first dimension, N0>=1
    
    X1      -   array of grid nodes, second coordinates, array[N1]
                Must be ordered by ascending. Exception is generated
                if the array is not correctly ordered.
    N1      -   grid size (number of nodes) in the second dimension, N1>=1

OUTPUT PARAMETERS:
    Y       -   function values, array[NY*N0*N1], where NY is a  number of
                "output" vector values (this  function   supports  vector-
                valued IDW models). Y is out-variable and  is  reallocated
                by this function.
                Y[K+NY*(I0+I1*N0)]=F_k(X0[I0],X1[I1]), for:
                *  K=0...NY-1
                * I0=0...N0-1
                * I1=0...N1-1

NOTE: this function supports weakly ordered grid nodes, i.e. you may  have                
      X[i]=X[i+1] for some i. It does  not  provide  you  any  performance
      benefits  due  to   duplication  of  points,  just  convenience  and
      flexibility.
      
NOTE: this  function  is  re-entrant,  i.e.  you  may  use  same  idwmodel
      structure in multiple threads calling  this function  for  different
      grids.
      
NOTE: if you need function values on some subset  of  regular  grid, which
      may be described as "several compact and  dense  islands",  you  may
      use idwgridcalc2vsubset().

  -- ALGLIB --
     Copyright 24.11.2023 by Bochkanov Sergey
*************************************************************************/
void idwgridcalc2v(const idwmodel* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n0,
     /* Real    */ const ae_vector* x1,
     ae_int_t n1,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_vector dummy;

    ae_frame_make(_state, &_frame_block);
    memset(&dummy, 0, sizeof(dummy));
    ae_vector_clear(y);
    ae_vector_init(&dummy, 0, DT_BOOL, _state, ae_true);

    ae_assert(n0>0, "IDWGridCalc2V: invalid value for N0 (N0<=0)!", _state);
    ae_assert(n1>0, "IDWGridCalc2V: invalid value for N1 (N1<=0)!", _state);
    ae_assert(x0->cnt>=n0, "IDWGridCalc2V: Length(X0)<N0", _state);
    ae_assert(x1->cnt>=n1, "IDWGridCalc2V: Length(X1)<N1", _state);
    ae_assert(isfinitevector(x0, n0, _state), "IDWGridCalc2V: X0 contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(x1, n1, _state), "IDWGridCalc2V: X1 contains infinite or NaN values!", _state);
    for(i=0; i<=n0-2; i++)
    {
        ae_assert(ae_fp_less_eq(x0->ptr.p_double[i],x0->ptr.p_double[i+1]), "IDWGridCalc2V: X0 is not ordered by ascending", _state);
    }
    for(i=0; i<=n1-2; i++)
    {
        ae_assert(ae_fp_less_eq(x1->ptr.p_double[i],x1->ptr.p_double[i+1]), "IDWGridCalc2V: X1 is not ordered by ascending", _state);
    }
    idwgridcalc2vx(s, x0, n0, x1, n1, &dummy, ae_false, y, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function calculates values of an  IDW  model  at  some  subset  of  a
regular grid:
* the grid has N0*N1 points, with Point[I,J] = (X0[I], X1[J])
* only values at some subset of the grid are required
Vector-valued IDW models are supported.

This function returns 0.0 when:
* the model is not initialized
* NX<>2

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

NOTE: Parallel processing is implemented only for modern (MSTAB) IDW's.

INPUT PARAMETERS:
    S       -   IDW model, used in read-only mode, can be  shared  between
                multiple   invocations  of  this  function  from  multiple
                threads.
    
    X0      -   array of grid nodes, first coordinates, array[N0].
                Must be ordered by ascending. Exception is generated
                if the array is not correctly ordered.
    N0      -   grid size (number of nodes) in the first dimension, N0>=1
    
    X1      -   array of grid nodes, second coordinates, array[N1]
                Must be ordered by ascending. Exception is generated
                if the array is not correctly ordered.
    N1      -   grid size (number of nodes) in the second dimension, N1>=1
    
    FlagY   -   array[N0*N1]:
                * Y[I0+I1*N0] corresponds to node (X0[I0],X1[I1])
                * it is a "bitmap" array which contains  False  for  nodes
                  which are NOT calculated, and True for nodes  which  are
                  required.

OUTPUT PARAMETERS:
    Y       -   function values, array[NY*N0*N1*N2], where NY is a  number
                of "output" vector values (this function  supports vector-
                valued IDW models):
                * Y[K+NY*(I0+I1*N0)]=F_k(X0[I0],X1[I1]),
                  for K=0...NY-1, I0=0...N0-1, I1=0...N1-1.
                * elements of Y[] which correspond  to  FlagY[]=True   are
                  loaded by model values (which may be  exactly  zero  for
                  some nodes).
                * elements of Y[] which correspond to FlagY[]=False MAY be
                  initialized by zeros OR may  be  calculated.  Generally,
                  they  are   not   calculated,  but  future  SIMD-capable
                  versions may compute several elements in a batch.

NOTE: this function supports weakly ordered grid nodes, i.e. you may  have                
      X[i]=X[i+1] for some i. It does  not  provide  you  any  performance
      benefits  due  to   duplication  of  points,  just  convenience  and
      flexibility.
      
NOTE: this  function  is  re-entrant,  i.e.  you  may  use  same  idwmodel
      structure in multiple threads calling  this function  for  different
      grids.

  -- ALGLIB --
     Copyright 24.11.2023 by Bochkanov Sergey
*************************************************************************/
void idwgridcalc2vsubset(const idwmodel* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n0,
     /* Real    */ const ae_vector* x1,
     ae_int_t n1,
     /* Boolean */ const ae_vector* flagy,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t i;

    ae_vector_clear(y);

    ae_assert(n0>0, "IDWGridCalc2VSubset: invalid value for N0 (N0<=0)!", _state);
    ae_assert(n1>0, "IDWGridCalc2VSubset: invalid value for N1 (N1<=0)!", _state);
    ae_assert(x0->cnt>=n0, "IDWGridCalc2VSubset: Length(X0)<N0", _state);
    ae_assert(x1->cnt>=n1, "IDWGridCalc2VSubset: Length(X1)<N1", _state);
    ae_assert(flagy->cnt>=n0*n1, "IDWGridCalc2VSubset: Length(FlagY)<N0*N1*N2", _state);
    ae_assert(isfinitevector(x0, n0, _state), "IDWGridCalc2VSubset: X0 contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(x1, n1, _state), "IDWGridCalc2VSubset: X1 contains infinite or NaN values!", _state);
    for(i=0; i<=n0-2; i++)
    {
        ae_assert(ae_fp_less_eq(x0->ptr.p_double[i],x0->ptr.p_double[i+1]), "IDWGridCalc2VSubset: X0 is not ordered by ascending", _state);
    }
    for(i=0; i<=n1-2; i++)
    {
        ae_assert(ae_fp_less_eq(x1->ptr.p_double[i],x1->ptr.p_double[i+1]), "IDWGridCalc2VSubset: X1 is not ordered by ascending", _state);
    }
    idwgridcalc2vx(s, x0, n0, x1, n1, flagy, ae_true, y, _state);
}


/*************************************************************************
This function, depending on SparseY, acts as IDWGridCalc2V (SparseY=False)
or IDWGridCalc2VSubset (SparseY=True) function.  See  comments  for  these
functions for more information

  -- ALGLIB --
     Copyright 04.03.2016 by Bochkanov Sergey
*************************************************************************/
void idwgridcalc2vx(const idwmodel* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n0,
     /* Real    */ const ae_vector* x1,
     ae_int_t n1,
     /* Boolean */ const ae_vector* flagy,
     ae_bool sparsey,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t nx;
    ae_int_t ny;
    ae_int_t i;
    ae_int_t k;
    double rcur;
    ae_int_t ylen;
    double evalcost;
    ae_shared_pool cbpool;
    idwcalcbuffer *calcbuf;
    ae_smart_ptr _calcbuf;

    ae_frame_make(_state, &_frame_block);
    memset(&cbpool, 0, sizeof(cbpool));
    memset(&_calcbuf, 0, sizeof(_calcbuf));
    ae_shared_pool_init(&cbpool, _state, ae_true);
    ae_smart_ptr_init(&_calcbuf, (void**)&calcbuf, _state, ae_true);

    ae_assert(n0>0, "IDWGridCalc2VX: invalid value for N0 (N0<=0)!", _state);
    ae_assert(n1>0, "IDWGridCalc2VX: invalid value for N1 (N1<=0)!", _state);
    ae_assert(x0->cnt>=n0, "IDWGridCalc2VX: Length(X0)<N0", _state);
    ae_assert(x1->cnt>=n1, "IDWGridCalc2VX: Length(X1)<N1", _state);
    ae_assert(isfinitevector(x0, n0, _state), "IDWGridCalc2VX: X0 contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(x1, n1, _state), "IDWGridCalc2VX: X1 contains infinite or NaN values!", _state);
    for(i=0; i<=n0-2; i++)
    {
        ae_assert(ae_fp_less_eq(x0->ptr.p_double[i],x0->ptr.p_double[i+1]), "IDWGridCalc2VX: X0 is not ordered by ascending", _state);
    }
    for(i=0; i<=n1-2; i++)
    {
        ae_assert(ae_fp_less_eq(x1->ptr.p_double[i],x1->ptr.p_double[i+1]), "IDWGridCalc2VX: X1 is not ordered by ascending", _state);
    }
    
    /*
     * Prepare local variables
     */
    nx = s->nx;
    ny = s->ny;
    ae_shared_pool_set_seed(&cbpool, &s->buffer, (ae_int_t)sizeof(s->buffer), (ae_constructor)_idwcalcbuffer_init, (ae_copy_constructor)_idwcalcbuffer_init_copy, (ae_destructor)_idwcalcbuffer_destroy, _state);
    
    /*
     * Prepare output array
     */
    ylen = ny*n0*n1;
    ae_vector_set_length(y, ylen, _state);
    rsetv(ylen, 0.0, y, _state);
    if( nx!=2 )
    {
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Evaluate cost (in CPU cycles) of a single model evaluation.
     * Very crude approximation is used.
     */
    evalcost = (double)(0);
    ae_assert((s->algotype==0||s->algotype==1)||s->algotype==2, "IDW: integrity check 9144 failed", _state);
    if( s->algotype==0 )
    {
        
        /*
         * Original Shepard: single evaluation cost is O(NPoints)
         */
        evalcost = evalcost+(double)(s->npoints*(5*nx+5*ny+50));
    }
    if( s->algotype==1 )
    {
        
        /*
         * Modified Shepard: single evaluation cost is mostly kd-tree request
         */
        ae_shared_pool_retrieve(&cbpool, &_calcbuf, _state);
        evalcost = evalcost+kdtreetsapproxrnnquerycost(&s->tree, &calcbuf->requestbuffer, s->r0, _state);
        ae_shared_pool_recycle(&cbpool, &_calcbuf, _state);
    }
    if( s->algotype==2 )
    {
        
        /*
         * MSTAB: single evaluation cost is a sum of per-layer costs
         */
        ae_shared_pool_retrieve(&cbpool, &_calcbuf, _state);
        rcur = s->r0;
        for(k=0; k<=s->nlayers-1; k++)
        {
            evalcost = evalcost+(kdtreetsapproxrnnquerycost(&s->tree, &calcbuf->requestbuffer, rcur, _state)+(double)50);
            rcur = rcur*s->rdecay;
        }
        ae_shared_pool_recycle(&cbpool, &_calcbuf, _state);
    }
    evalcost = coalesce(evalcost, (double)(50), _state);
    
    /*
     * Perform the evaluation
     */
    idw_idwgridcalc2rec(s, x0, 0, n0, n0, x1, 0, n1, n1, flagy, sparsey, &cbpool, ae_true, evalcost, y, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Serializer: allocation

  -- ALGLIB --
     Copyright 28.02.2018 by Bochkanov Sergey
*************************************************************************/
void idwalloc(ae_serializer* s, const idwmodel* model, ae_state *_state)
{
    ae_bool processed;


    
    /*
     * Header
     */
    ae_serializer_alloc_entry(s);
    
    /*
     * Algorithm type and fields which are set for all algorithms
     */
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    allocrealarray(s, &model->globalprior, -1, _state);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    
    /*
     * Algorithm-specific fields
     */
    processed = ae_false;
    if( model->algotype==0 )
    {
        ae_serializer_alloc_entry(s);
        allocrealarray(s, &model->shepardxy, -1, _state);
        processed = ae_true;
    }
    if( model->algotype>0 )
    {
        kdtreealloc(s, &model->tree, _state);
        processed = ae_true;
    }
    ae_assert(processed, "IDW: integrity check failed during serialization", _state);
}


/*************************************************************************
Serializer: serialization

  -- ALGLIB --
     Copyright 28.02.2018 by Bochkanov Sergey
*************************************************************************/
void idwserialize(ae_serializer* s,
     const idwmodel* model,
     ae_state *_state)
{
    ae_bool processed;


    
    /*
     * Header
     */
    ae_serializer_serialize_int(s, getidwserializationcode(_state), _state);
    
    /*
     * Algorithm type and fields which are set for all algorithms
     */
    ae_serializer_serialize_int(s, model->algotype, _state);
    ae_serializer_serialize_int(s, model->nx, _state);
    ae_serializer_serialize_int(s, model->ny, _state);
    serializerealarray(s, &model->globalprior, -1, _state);
    ae_serializer_serialize_int(s, model->nlayers, _state);
    ae_serializer_serialize_double(s, model->r0, _state);
    ae_serializer_serialize_double(s, model->rdecay, _state);
    ae_serializer_serialize_double(s, model->lambda0, _state);
    ae_serializer_serialize_double(s, model->lambdalast, _state);
    ae_serializer_serialize_double(s, model->lambdadecay, _state);
    ae_serializer_serialize_double(s, model->shepardp, _state);
    
    /*
     * Algorithm-specific fields
     */
    processed = ae_false;
    if( model->algotype==0 )
    {
        ae_serializer_serialize_int(s, model->npoints, _state);
        serializerealarray(s, &model->shepardxy, -1, _state);
        processed = ae_true;
    }
    if( model->algotype>0 )
    {
        kdtreeserialize(s, &model->tree, _state);
        processed = ae_true;
    }
    ae_assert(processed, "IDW: integrity check failed during serialization", _state);
}


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 28.02.2018 by Bochkanov Sergey
*************************************************************************/
void idwunserialize(ae_serializer* s, idwmodel* model, ae_state *_state)
{
    ae_bool processed;
    ae_int_t scode;

    _idwmodel_clear(model);

    
    /*
     * Header
     */
    ae_serializer_unserialize_int(s, &scode, _state);
    ae_assert(scode==getidwserializationcode(_state), "IDWUnserialize: stream header corrupted", _state);
    
    /*
     * Algorithm type and fields which are set for all algorithms
     */
    ae_serializer_unserialize_int(s, &model->algotype, _state);
    ae_serializer_unserialize_int(s, &model->nx, _state);
    ae_serializer_unserialize_int(s, &model->ny, _state);
    unserializerealarray(s, &model->globalprior, _state);
    ae_serializer_unserialize_int(s, &model->nlayers, _state);
    ae_serializer_unserialize_double(s, &model->r0, _state);
    ae_serializer_unserialize_double(s, &model->rdecay, _state);
    ae_serializer_unserialize_double(s, &model->lambda0, _state);
    ae_serializer_unserialize_double(s, &model->lambdalast, _state);
    ae_serializer_unserialize_double(s, &model->lambdadecay, _state);
    ae_serializer_unserialize_double(s, &model->shepardp, _state);
    model->debugprofile = ae_false;
    
    /*
     * Algorithm-specific fields
     */
    processed = ae_false;
    if( model->algotype==0 )
    {
        ae_serializer_unserialize_int(s, &model->npoints, _state);
        unserializerealarray(s, &model->shepardxy, _state);
        processed = ae_true;
    }
    if( model->algotype>0 )
    {
        kdtreeunserialize(s, &model->tree, _state);
        processed = ae_true;
    }
    ae_assert(processed, "IDW: integrity check failed during serialization", _state);
    
    /*
     * Temporary buffers
     */
    idwcreatecalcbuffer(model, &model->buffer, _state);
}


/*************************************************************************
This function, depending on SparseY, acts as IDWGridCalc2V (SparseY=False)
or IDWGridCalc2VSubset (SparseY=True) function.  See  comments  for  these
functions for more information

  -- ALGLIB --
     Copyright 04.03.2016 by Bochkanov Sergey
*************************************************************************/
static void idw_idwgridcalc2rec(const idwmodel* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t begin0,
     ae_int_t end0,
     ae_int_t n0,
     /* Real    */ const ae_vector* x1,
     ae_int_t begin1,
     ae_int_t end1,
     ae_int_t n1,
     /* Boolean */ const ae_vector* flagy,
     ae_bool sparsey,
     ae_shared_pool* cbpool,
     ae_bool isrootcall,
     double evalcost,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t nx;
    ae_int_t ny;
    ae_int_t batchsize;
    ae_int_t chunksize;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_bool abovepexeclevel;
    ae_bool abovespawnlevel;
    idwcalcbuffer *calcbuf;
    ae_smart_ptr _calcbuf;

    ae_frame_make(_state, &_frame_block);
    memset(&_calcbuf, 0, sizeof(_calcbuf));
    ae_smart_ptr_init(&_calcbuf, (void**)&calcbuf, _state, ae_true);

    nx = s->nx;
    ny = s->ny;
    batchsize = icase2(s->debugprofile, 1, idw_idwbatchsize, _state);
    chunksize = (end0-begin0)*(end1-begin1);
    ae_assert(nx==2, "IDW: integrity check 5621 failed", _state);
    
    /*
     * Perform parallel execution if needed
     */
    abovepexeclevel = ae_fp_greater((double)chunksize*evalcost,smpactivationlevel(_state))||s->debugprofile;
    if( (isrootcall&&chunksize>batchsize)&&abovepexeclevel )
    {
        if( _trypexec_idw_idwgridcalc2rec(s,x0,begin0,end0,n0,x1,begin1,end1,n1,flagy,sparsey,cbpool,isrootcall,evalcost,y, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            ae_frame_leave(_state);
            return;
        }
    }
    
    /*
     * Perform subdivision
     */
    abovespawnlevel = ae_fp_greater((double)chunksize*evalcost,spawnlevel(_state))||s->debugprofile;
    if( chunksize>batchsize&&abovespawnlevel )
    {
        
        /*
         * Subdivision
         */
        ae_assert(ae_maxint(end0-begin0, end1-begin1, _state)>1, "IDW: integrity check 6712 failed", _state);
        ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_true, _state);
        if( end0-begin0>end1-begin1 )
        {
            
            /*
             * Split on variable 0
             */
            k = begin0+(end0-begin0)/2;
            _spawn_idw_idwgridcalc2rec(s, x0, begin0, k, n0, x1, begin1, end1, n1, flagy, sparsey, cbpool, ae_false, evalcost, y, _child_tasks, _smp_enabled, _state);
            idw_idwgridcalc2rec(s, x0, k, end0, n0, x1, begin1, end1, n1, flagy, sparsey, cbpool, ae_false, evalcost, y, _state);
        }
        else
        {
            k = begin1+(end1-begin1)/2;
            _spawn_idw_idwgridcalc2rec(s, x0, begin0, end0, n0, x1, begin1, k, n1, flagy, sparsey, cbpool, ae_false, evalcost, y, _child_tasks, _smp_enabled, _state);
            idw_idwgridcalc2rec(s, x0, begin0, end0, n0, x1, k, end1, n1, flagy, sparsey, cbpool, ae_false, evalcost, y, _state);
        }
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Basecase code
     */
    ae_shared_pool_retrieve(cbpool, &_calcbuf, _state);
    rallocv(nx, &calcbuf->x, _state);
    for(i=begin0; i<=end0-1; i++)
    {
        for(j=begin1; j<=end1-1; j++)
        {
            if( !sparsey||flagy->ptr.p_bool[i+j*n0] )
            {
                calcbuf->x.ptr.p_double[0] = x0->ptr.p_double[i];
                calcbuf->x.ptr.p_double[1] = x1->ptr.p_double[j];
                idwtscalcbuf(s, calcbuf, &calcbuf->x, &calcbuf->y, _state);
                for(k=0; k<=ny-1; k++)
                {
                    y->ptr.p_double[k+ny*(i+j*n0)] = calcbuf->y.ptr.p_double[k];
                }
            }
        }
    }
    ae_shared_pool_recycle(cbpool, &_calcbuf, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_idw_idwgridcalc2rec(const idwmodel* s,
    /* Real    */ const ae_vector* x0,
    ae_int_t begin0,
    ae_int_t end0,
    ae_int_t n0,
    /* Real    */ const ae_vector* x1,
    ae_int_t begin1,
    ae_int_t end1,
    ae_int_t n1,
    /* Boolean */ const ae_vector* flagy,
    ae_bool sparsey,
    ae_shared_pool* cbpool,
    ae_bool isrootcall,
    double evalcost,
    /* Real    */ ae_vector* y,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        idw_idwgridcalc2rec(s,x0,begin0,end0,n0,x1,begin1,end1,n1,flagy,sparsey,cbpool,isrootcall,evalcost,y, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_idw_idwgridcalc2rec;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.const_val = s;
        _task->data.parameters[1].value.const_val = x0;
        _task->data.parameters[2].value.ival = begin0;
        _task->data.parameters[3].value.ival = end0;
        _task->data.parameters[4].value.ival = n0;
        _task->data.parameters[5].value.const_val = x1;
        _task->data.parameters[6].value.ival = begin1;
        _task->data.parameters[7].value.ival = end1;
        _task->data.parameters[8].value.ival = n1;
        _task->data.parameters[9].value.const_val = flagy;
        _task->data.parameters[10].value.bval = sparsey;
        _task->data.parameters[11].value.val = cbpool;
        _task->data.parameters[12].value.bval = isrootcall;
        _task->data.parameters[13].value.dval = evalcost;
        _task->data.parameters[14].value.val = y;
        ae_push_task(_task, _state, ae_false);
    }
#else
    abort(); /* critical integrity failure */
#endif
}


/*************************************************************************
This method is called when worker thread starts working on a non-root task.
*************************************************************************/
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_idw_idwgridcalc2rec(ae_task_data *_data, ae_state *_state)
{
    const idwmodel* s;
    const ae_vector* x0;
    ae_int_t begin0;
    ae_int_t end0;
    ae_int_t n0;
    const ae_vector* x1;
    ae_int_t begin1;
    ae_int_t end1;
    ae_int_t n1;
    const ae_vector* flagy;
    ae_bool sparsey;
    ae_shared_pool* cbpool;
    ae_bool isrootcall;
    double evalcost;
    ae_vector* y;
    s = (const idwmodel*)_data->parameters[0].value.const_val;
    x0 = (const ae_vector*)_data->parameters[1].value.const_val;
    begin0 = _data->parameters[2].value.ival;
    end0 = _data->parameters[3].value.ival;
    n0 = _data->parameters[4].value.ival;
    x1 = (const ae_vector*)_data->parameters[5].value.const_val;
    begin1 = _data->parameters[6].value.ival;
    end1 = _data->parameters[7].value.ival;
    n1 = _data->parameters[8].value.ival;
    flagy = (const ae_vector*)_data->parameters[9].value.const_val;
    sparsey = _data->parameters[10].value.bval;
    cbpool = (ae_shared_pool*)_data->parameters[11].value.val;
    isrootcall = _data->parameters[12].value.bval;
    evalcost = _data->parameters[13].value.dval;
    y = (ae_vector*)_data->parameters[14].value.val;
   ae_state_set_flags(_state, _data->flags);
   idw_idwgridcalc2rec(s,x0,begin0,end0,n0,x1,begin1,end1,n1,flagy,sparsey,cbpool,isrootcall,evalcost,y, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_idw_idwgridcalc2rec(const idwmodel* s,
    /* Real    */ const ae_vector* x0,
    ae_int_t begin0,
    ae_int_t end0,
    ae_int_t n0,
    /* Real    */ const ae_vector* x1,
    ae_int_t begin1,
    ae_int_t end1,
    ae_int_t n1,
    /* Boolean */ const ae_vector* flagy,
    ae_bool sparsey,
    ae_shared_pool* cbpool,
    ae_bool isrootcall,
    double evalcost,
    /* Real    */ ae_vector* y,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_idw_idwgridcalc2rec;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.const_val = s;
    _task->data.parameters[1].value.const_val = x0;
    _task->data.parameters[2].value.ival = begin0;
    _task->data.parameters[3].value.ival = end0;
    _task->data.parameters[4].value.ival = n0;
    _task->data.parameters[5].value.const_val = x1;
    _task->data.parameters[6].value.ival = begin1;
    _task->data.parameters[7].value.ival = end1;
    _task->data.parameters[8].value.ival = n1;
    _task->data.parameters[9].value.const_val = flagy;
    _task->data.parameters[10].value.bval = sparsey;
    _task->data.parameters[11].value.val = cbpool;
    _task->data.parameters[12].value.bval = isrootcall;
    _task->data.parameters[13].value.dval = evalcost;
    _task->data.parameters[14].value.val = y;
    ae_push_root_task(_task);
    ae_wait_for_event(&_task->done_event);
    _e = _task->exception;
    ae_dispose_task(_task);
    ae_assert(_e==NULL, _e, _state);
    return ae_true;
#else
    return ae_false;
#endif
}


/*************************************************************************
This function evaluates error metrics for the model  using  IDWTsCalcBuf()
to calculate model at each point.

NOTE: modern IDW algorithms (MSTAB, MSMOOTH) can generate residuals during
      model construction, so they do not need this function  in  order  to
      evaluate error metrics.

Following fields of Rep are filled:
* rep.rmserror
* rep.avgerror
* rep.maxerror
* rep.r2
   
  -- ALGLIB --
     Copyright 22.10.2018 by Bochkanov Sergey
*************************************************************************/
static void idw_errormetricsviacalc(idwbuilder* state,
     idwmodel* model,
     idwreport* rep,
     ae_state *_state)
{
    ae_int_t npoints;
    ae_int_t nx;
    ae_int_t ny;
    ae_int_t i;
    ae_int_t j;
    double v;
    double vv;
    double rss;
    double tss;


    npoints = state->npoints;
    nx = state->nx;
    ny = state->ny;
    if( npoints==0 )
    {
        rep->rmserror = (double)(0);
        rep->avgerror = (double)(0);
        rep->maxerror = (double)(0);
        rep->r2 = (double)(1);
        return;
    }
    rep->rmserror = (double)(0);
    rep->avgerror = (double)(0);
    rep->maxerror = (double)(0);
    rss = (double)(0);
    tss = (double)(0);
    for(i=0; i<=npoints-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            model->buffer.x.ptr.p_double[j] = state->xy.ptr.p_double[i*(nx+ny)+j];
        }
        idwtscalcbuf(model, &model->buffer, &model->buffer.x, &model->buffer.y, _state);
        for(j=0; j<=ny-1; j++)
        {
            vv = state->xy.ptr.p_double[i*(nx+ny)+nx+j];
            v = ae_fabs(vv-model->buffer.y.ptr.p_double[j], _state);
            rep->rmserror = rep->rmserror+v*v;
            rep->avgerror = rep->avgerror+v;
            rep->maxerror = ae_maxreal(rep->maxerror, v, _state);
            rss = rss+v*v;
            tss = tss+ae_sqr(vv-state->tmpmean.ptr.p_double[j], _state);
        }
    }
    rep->rmserror = ae_sqrt(rep->rmserror/(double)(npoints*ny), _state);
    rep->avgerror = rep->avgerror/(double)(npoints*ny);
    rep->r2 = 1.0-rss/coalesce(tss, 1.0, _state);
}


/*************************************************************************
Basecase for IDW-MSTAB model construction algorithm
   
  -- ALGLIB --
     Copyright 22.10.2018 by Bochkanov Sergey
*************************************************************************/
static void idw_mstabrec(idwbuilder* state,
     double rcur,
     double lambdacur,
     ae_int_t layeridx,
     ae_shared_pool* mbpool,
     ae_int_t idx0,
     ae_int_t idx1,
     ae_bool isrootcall,
     double querycost,
     double totalcost,
     /* Real    */ ae_matrix* layers,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t tile0;
    ae_int_t tile1;
    mstabbuffer *mbuffer;
    ae_smart_ptr _mbuffer;
    ae_bool abovepexeclevel;
    ae_bool abovespawnlevel;

    ae_frame_make(_state, &_frame_block);
    memset(&_mbuffer, 0, sizeof(_mbuffer));
    ae_smart_ptr_init(&_mbuffer, (void**)&mbuffer, _state, ae_true);

    
    /*
     * Try parallel execution, if needed
     */
    abovepexeclevel = ae_fp_greater((double)(idx1-idx0)*querycost,smpactivationlevel(_state))||state->debugprofile;
    if( (isrootcall&&idx1-idx0>state->mbatchsize)&&abovepexeclevel )
    {
        if( _trypexec_idw_mstabrec(state,rcur,lambdacur,layeridx,mbpool,idx0,idx1,isrootcall,querycost,totalcost,layers, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            ae_frame_leave(_state);
            return;
        }
    }
    
    /*
     * Handle basecase or recursively subdivide
     */
    abovespawnlevel = ae_fp_greater((double)(idx1-idx0)*querycost,spawnlevel(_state))||state->debugprofile;
    if( idx1-idx0>state->mbatchsize&&abovespawnlevel )
    {
        
        /*
         * Subdivision
         */
        tiledsplit(idx1-idx0, state->mbatchsize, &tile0, &tile1, _state);
        ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_true, _state);
        _spawn_idw_mstabrec(state, rcur, lambdacur, layeridx, mbpool, idx0, idx0+tile0, ae_false, querycost, totalcost, layers, _child_tasks, _smp_enabled, _state);
        idw_mstabrec(state, rcur, lambdacur, layeridx, mbpool, idx0+tile0, idx1, ae_false, querycost, totalcost, layers, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Basecase
     */
    ae_shared_pool_retrieve(mbpool, &_mbuffer, _state);
    idw_mstabbasecase(state, rcur, lambdacur, layeridx, mbuffer, idx0, idx1, querycost, totalcost, layers, _state);
    ae_shared_pool_recycle(mbpool, &_mbuffer, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_idw_mstabrec(idwbuilder* state,
    double rcur,
    double lambdacur,
    ae_int_t layeridx,
    ae_shared_pool* mbpool,
    ae_int_t idx0,
    ae_int_t idx1,
    ae_bool isrootcall,
    double querycost,
    double totalcost,
    /* Real    */ ae_matrix* layers,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        idw_mstabrec(state,rcur,lambdacur,layeridx,mbpool,idx0,idx1,isrootcall,querycost,totalcost,layers, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_idw_mstabrec;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = state;
        _task->data.parameters[1].value.dval = rcur;
        _task->data.parameters[2].value.dval = lambdacur;
        _task->data.parameters[3].value.ival = layeridx;
        _task->data.parameters[4].value.val = mbpool;
        _task->data.parameters[5].value.ival = idx0;
        _task->data.parameters[6].value.ival = idx1;
        _task->data.parameters[7].value.bval = isrootcall;
        _task->data.parameters[8].value.dval = querycost;
        _task->data.parameters[9].value.dval = totalcost;
        _task->data.parameters[10].value.val = layers;
        ae_push_task(_task, _state, ae_false);
    }
#else
    abort(); /* critical integrity failure */
#endif
}


/*************************************************************************
This method is called when worker thread starts working on a non-root task.
*************************************************************************/
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_idw_mstabrec(ae_task_data *_data, ae_state *_state)
{
    idwbuilder* state;
    double rcur;
    double lambdacur;
    ae_int_t layeridx;
    ae_shared_pool* mbpool;
    ae_int_t idx0;
    ae_int_t idx1;
    ae_bool isrootcall;
    double querycost;
    double totalcost;
    ae_matrix* layers;
    state = (idwbuilder*)_data->parameters[0].value.val;
    rcur = _data->parameters[1].value.dval;
    lambdacur = _data->parameters[2].value.dval;
    layeridx = _data->parameters[3].value.ival;
    mbpool = (ae_shared_pool*)_data->parameters[4].value.val;
    idx0 = _data->parameters[5].value.ival;
    idx1 = _data->parameters[6].value.ival;
    isrootcall = _data->parameters[7].value.bval;
    querycost = _data->parameters[8].value.dval;
    totalcost = _data->parameters[9].value.dval;
    layers = (ae_matrix*)_data->parameters[10].value.val;
   ae_state_set_flags(_state, _data->flags);
   idw_mstabrec(state,rcur,lambdacur,layeridx,mbpool,idx0,idx1,isrootcall,querycost,totalcost,layers, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_idw_mstabrec(idwbuilder* state,
    double rcur,
    double lambdacur,
    ae_int_t layeridx,
    ae_shared_pool* mbpool,
    ae_int_t idx0,
    ae_int_t idx1,
    ae_bool isrootcall,
    double querycost,
    double totalcost,
    /* Real    */ ae_matrix* layers,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_idw_mstabrec;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = state;
    _task->data.parameters[1].value.dval = rcur;
    _task->data.parameters[2].value.dval = lambdacur;
    _task->data.parameters[3].value.ival = layeridx;
    _task->data.parameters[4].value.val = mbpool;
    _task->data.parameters[5].value.ival = idx0;
    _task->data.parameters[6].value.ival = idx1;
    _task->data.parameters[7].value.bval = isrootcall;
    _task->data.parameters[8].value.dval = querycost;
    _task->data.parameters[9].value.dval = totalcost;
    _task->data.parameters[10].value.val = layers;
    ae_push_root_task(_task);
    ae_wait_for_event(&_task->done_event);
    _e = _task->exception;
    ae_dispose_task(_task);
    ae_assert(_e==NULL, _e, _state);
    return ae_true;
#else
    return ae_false;
#endif
}


/*************************************************************************
Basecase for IDW-MSTAB model construction algorithm
   
  -- ALGLIB --
     Copyright 22.10.2018 by Bochkanov Sergey
*************************************************************************/
static void idw_mstabbasecase(idwbuilder* state,
     double rcur,
     double lambdacur,
     ae_int_t layeridx,
     mstabbuffer* buf,
     ae_int_t idx0,
     ae_int_t idx1,
     double querycost,
     double totalcost,
     /* Real    */ ae_matrix* layers,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t i0;
    ae_int_t srcidx;
    ae_int_t nx;
    ae_int_t ny;
    double v;
    double vv;
    double localprogress;


    nx = state->nx;
    ny = state->ny;
    localprogress = 0.0;
    for(i=idx0; i<=idx1-1; i++)
    {
        
        /*
         * Build the model
         */
        for(j=0; j<=nx-1; j++)
        {
            buf->x.ptr.p_double[j] = layers->ptr.pp_double[i][j];
        }
        k = kdtreetsqueryrnn(&state->tmptree, &buf->requestbuffer, &buf->x, rcur, ae_true, _state);
        kdtreetsqueryresultstags(&state->tmptree, &buf->requestbuffer, &buf->tags, _state);
        kdtreetsqueryresultsdistances(&state->tmptree, &buf->requestbuffer, &buf->dist, _state);
        for(j=0; j<=ny-1; j++)
        {
            buf->wy.ptr.p_double[j] = (double)(0);
            buf->w.ptr.p_double[j] = idw_w0;
        }
        for(i0=0; i0<=k-1; i0++)
        {
            vv = buf->dist.ptr.p_double[i0]/rcur;
            vv = vv*vv;
            v = ((double)1-vv)*((double)1-vv)/(vv+lambdacur);
            srcidx = buf->tags.ptr.p_int[i0];
            for(j=0; j<=ny-1; j++)
            {
                buf->wy.ptr.p_double[j] = buf->wy.ptr.p_double[j]+v*layers->ptr.pp_double[srcidx][nx+layeridx*ny+j];
                buf->w.ptr.p_double[j] = buf->w.ptr.p_double[j]+v;
            }
        }
        for(j=0; j<=ny-1; j++)
        {
            v = layers->ptr.pp_double[i][nx+layeridx*ny+j];
            layers->ptr.pp_double[i][nx+(layeridx+1)*ny+j] = v-buf->wy.ptr.p_double[j]/buf->w.ptr.p_double[j];
        }
        
        /*
         * Update progress
         */
        localprogress = localprogress+querycost/coalesce(totalcost, (double)(1), _state);
        if( ae_fp_greater_eq(localprogress,0.001)||i==idx1-1 )
        {
            rthreadunsafeset(&state->mprogress, boundval(rthreadunsafeget(&state->mprogress, _state)+localprogress, (double)(0), (double)(1), _state), _state);
            localprogress = 0.0;
        }
    }
}


void _idwcalcbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    idwcalcbuffer *p = (idwcalcbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->y, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tsyw, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tsw, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->tsxy, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tsdist, 0, DT_REAL, _state, make_automatic);
    _kdtreerequestbuffer_init(&p->requestbuffer, _state, make_automatic);
}


void _idwcalcbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    idwcalcbuffer       *dst = (idwcalcbuffer*)_dst;
    const idwcalcbuffer *src = (const idwcalcbuffer*)_src;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->y, &src->y, _state, make_automatic);
    ae_vector_init_copy(&dst->tsyw, &src->tsyw, _state, make_automatic);
    ae_vector_init_copy(&dst->tsw, &src->tsw, _state, make_automatic);
    ae_matrix_init_copy(&dst->tsxy, &src->tsxy, _state, make_automatic);
    ae_vector_init_copy(&dst->tsdist, &src->tsdist, _state, make_automatic);
    _kdtreerequestbuffer_init_copy(&dst->requestbuffer, &src->requestbuffer, _state, make_automatic);
}


void _idwcalcbuffer_clear(void* _p)
{
    idwcalcbuffer *p = (idwcalcbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->y);
    ae_vector_clear(&p->tsyw);
    ae_vector_clear(&p->tsw);
    ae_matrix_clear(&p->tsxy);
    ae_vector_clear(&p->tsdist);
    _kdtreerequestbuffer_clear(&p->requestbuffer);
}


void _idwcalcbuffer_destroy(void* _p)
{
    idwcalcbuffer *p = (idwcalcbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->y);
    ae_vector_destroy(&p->tsyw);
    ae_vector_destroy(&p->tsw);
    ae_matrix_destroy(&p->tsxy);
    ae_vector_destroy(&p->tsdist);
    _kdtreerequestbuffer_destroy(&p->requestbuffer);
}


void _mstabbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    mstabbuffer *p = (mstabbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->dist, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->w, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->wy, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tags, 0, DT_INT, _state, make_automatic);
    _kdtreerequestbuffer_init(&p->requestbuffer, _state, make_automatic);
}


void _mstabbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    mstabbuffer       *dst = (mstabbuffer*)_dst;
    const mstabbuffer *src = (const mstabbuffer*)_src;
    ae_vector_init_copy(&dst->dist, &src->dist, _state, make_automatic);
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->w, &src->w, _state, make_automatic);
    ae_vector_init_copy(&dst->wy, &src->wy, _state, make_automatic);
    ae_vector_init_copy(&dst->tags, &src->tags, _state, make_automatic);
    _kdtreerequestbuffer_init_copy(&dst->requestbuffer, &src->requestbuffer, _state, make_automatic);
}


void _mstabbuffer_clear(void* _p)
{
    mstabbuffer *p = (mstabbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->dist);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->w);
    ae_vector_clear(&p->wy);
    ae_vector_clear(&p->tags);
    _kdtreerequestbuffer_clear(&p->requestbuffer);
}


void _mstabbuffer_destroy(void* _p)
{
    mstabbuffer *p = (mstabbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->dist);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->w);
    ae_vector_destroy(&p->wy);
    ae_vector_destroy(&p->tags);
    _kdtreerequestbuffer_destroy(&p->requestbuffer);
}


void _idwmodel_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    idwmodel *p = (idwmodel*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->globalprior, 0, DT_REAL, _state, make_automatic);
    _kdtree_init(&p->tree, _state, make_automatic);
    ae_vector_init(&p->shepardxy, 0, DT_REAL, _state, make_automatic);
    _idwcalcbuffer_init(&p->buffer, _state, make_automatic);
}


void _idwmodel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    idwmodel       *dst = (idwmodel*)_dst;
    const idwmodel *src = (const idwmodel*)_src;
    dst->nx = src->nx;
    dst->ny = src->ny;
    ae_vector_init_copy(&dst->globalprior, &src->globalprior, _state, make_automatic);
    dst->algotype = src->algotype;
    dst->nlayers = src->nlayers;
    dst->r0 = src->r0;
    dst->rdecay = src->rdecay;
    dst->lambda0 = src->lambda0;
    dst->lambdalast = src->lambdalast;
    dst->lambdadecay = src->lambdadecay;
    dst->shepardp = src->shepardp;
    dst->debugprofile = src->debugprofile;
    _kdtree_init_copy(&dst->tree, &src->tree, _state, make_automatic);
    dst->npoints = src->npoints;
    ae_vector_init_copy(&dst->shepardxy, &src->shepardxy, _state, make_automatic);
    _idwcalcbuffer_init_copy(&dst->buffer, &src->buffer, _state, make_automatic);
}


void _idwmodel_clear(void* _p)
{
    idwmodel *p = (idwmodel*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->globalprior);
    _kdtree_clear(&p->tree);
    ae_vector_clear(&p->shepardxy);
    _idwcalcbuffer_clear(&p->buffer);
}


void _idwmodel_destroy(void* _p)
{
    idwmodel *p = (idwmodel*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->globalprior);
    _kdtree_destroy(&p->tree);
    ae_vector_destroy(&p->shepardxy);
    _idwcalcbuffer_destroy(&p->buffer);
}


void _idwbuilder_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    idwbuilder *p = (idwbuilder*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->priortermval, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xy, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->tmpxy, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->tmplayers, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmptags, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->tmpdist, 0, DT_REAL, _state, make_automatic);
    _kdtree_init(&p->tmptree, _state, make_automatic);
    ae_vector_init(&p->tmpmean, 0, DT_REAL, _state, make_automatic);
}


void _idwbuilder_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    idwbuilder       *dst = (idwbuilder*)_dst;
    const idwbuilder *src = (const idwbuilder*)_src;
    dst->priortermtype = src->priortermtype;
    ae_vector_init_copy(&dst->priortermval, &src->priortermval, _state, make_automatic);
    dst->algotype = src->algotype;
    dst->nlayers = src->nlayers;
    dst->r0 = src->r0;
    dst->rdecay = src->rdecay;
    dst->lambda0 = src->lambda0;
    dst->lambdalast = src->lambdalast;
    dst->lambdadecay = src->lambdadecay;
    dst->shepardp = src->shepardp;
    dst->debugprofile = src->debugprofile;
    dst->mbatchsize = src->mbatchsize;
    dst->mprogress = src->mprogress;
    ae_vector_init_copy(&dst->xy, &src->xy, _state, make_automatic);
    dst->npoints = src->npoints;
    dst->nx = src->nx;
    dst->ny = src->ny;
    ae_matrix_init_copy(&dst->tmpxy, &src->tmpxy, _state, make_automatic);
    ae_matrix_init_copy(&dst->tmplayers, &src->tmplayers, _state, make_automatic);
    ae_vector_init_copy(&dst->tmptags, &src->tmptags, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpdist, &src->tmpdist, _state, make_automatic);
    _kdtree_init_copy(&dst->tmptree, &src->tmptree, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpmean, &src->tmpmean, _state, make_automatic);
}


void _idwbuilder_clear(void* _p)
{
    idwbuilder *p = (idwbuilder*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->priortermval);
    ae_vector_clear(&p->xy);
    ae_matrix_clear(&p->tmpxy);
    ae_matrix_clear(&p->tmplayers);
    ae_vector_clear(&p->tmptags);
    ae_vector_clear(&p->tmpdist);
    _kdtree_clear(&p->tmptree);
    ae_vector_clear(&p->tmpmean);
}


void _idwbuilder_destroy(void* _p)
{
    idwbuilder *p = (idwbuilder*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->priortermval);
    ae_vector_destroy(&p->xy);
    ae_matrix_destroy(&p->tmpxy);
    ae_matrix_destroy(&p->tmplayers);
    ae_vector_destroy(&p->tmptags);
    ae_vector_destroy(&p->tmpdist);
    _kdtree_destroy(&p->tmptree);
    ae_vector_destroy(&p->tmpmean);
}


void _idwreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    idwreport *p = (idwreport*)_p;
    ae_touch_ptr((void*)p);
}


void _idwreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    idwreport       *dst = (idwreport*)_dst;
    const idwreport *src = (const idwreport*)_src;
    dst->rmserror = src->rmserror;
    dst->avgerror = src->avgerror;
    dst->maxerror = src->maxerror;
    dst->r2 = src->r2;
}


void _idwreport_clear(void* _p)
{
    idwreport *p = (idwreport*)_p;
    ae_touch_ptr((void*)p);
}


void _idwreport_destroy(void* _p)
{
    idwreport *p = (idwreport*)_p;
    ae_touch_ptr((void*)p);
}


/*$ End $*/
