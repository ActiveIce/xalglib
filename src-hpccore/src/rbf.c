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
#include "rbf.h"


/*$ Declarations $*/
static double rbf_eps = 1.0E-6;
static double rbf_rbffarradius = 6;
static ae_int_t rbf_rbffirstversion = 0;
static ae_int_t rbf_rbfversion2 = 2;
static ae_int_t rbf_rbfversion3 = 3;
static void rbf_rbfpreparenonserializablefields(rbfmodel* s,
     ae_state *_state);
static void rbf_initializev1(ae_int_t nx,
     ae_int_t ny,
     rbfv1model* s,
     ae_state *_state);
static void rbf_initializev2(ae_int_t nx,
     ae_int_t ny,
     rbfv2model* s,
     ae_state *_state);
static void rbf_initializev3(ae_int_t nx,
     ae_int_t ny,
     rbfv3model* s,
     ae_state *_state);
static void rbf_clearreportfields(rbfreport* rep, ae_state *_state);


/*$ Body $*/


/*************************************************************************
This function creates RBF  model  for  a  scalar (NY=1)  or  vector (NY>1)
function in a NX-dimensional space (NX>=1).

Newly created model is empty. It can be used for interpolation right after
creation, but it just returns zeros. You have to add points to the  model,
tune interpolation settings, and then  call  model  construction  function
rbfbuildmodel() which will update model according to your specification.

USAGE:
1. User creates model with rbfcreate()
2. User adds dataset with rbfsetpoints() or rbfsetpointsandscales()
3. User selects RBF solver by calling:
   * rbfsetalgohierarchical() - for a HRBF solver,  a  hierarchical large-
     scale Gaussian RBFs  (works  well  for  uniformly  distributed  point
     clouds, but may fail when the data are non-uniform; use other solvers
     below in such cases)
   * rbfsetalgothinplatespline() - for a large-scale DDM-RBF  solver  with
     thin plate spline basis function being used
   * rbfsetalgobiharmonic() -  for  a  large-scale  DDM-RBF  solver   with
     biharmonic basis function being used
   * rbfsetalgomultiquadricauto() -  for a large-scale DDM-RBF solver with
     multiquadric basis function being used (automatic  selection  of  the
     scale parameter Alpha)
   * rbfsetalgomultiquadricmanual() -  for a  large-scale  DDM-RBF  solver
     with multiquadric basis function being used (manual selection  of the
     scale parameter Alpha)
4. (OPTIONAL) User chooses polynomial term by calling:
   * rbflinterm() to set linear term (default)
   * rbfconstterm() to set constant term
   * rbfzeroterm() to set zero term
5. User calls rbfbuildmodel() function which rebuilds model  according  to
   the specification
   
INPUT PARAMETERS:
    NX      -   dimension of the space, NX>=1
    NY      -   function dimension, NY>=1

OUTPUT PARAMETERS:
    S       -   RBF model (initially equals to zero)

NOTE 1: memory requirements. RBF models require amount of memory  which is
        proportional  to the number of data points. Some additional memory
        is allocated during model construction, but most of this memory is
        freed after the model  coefficients  are   calculated.  Amount  of
        this additional memory depends  on  model  construction  algorithm
        being used.

  -- ALGLIB --
     Copyright 13.12.2011, 20.06.2016 by Bochkanov Sergey
*************************************************************************/
void rbfcreate(ae_int_t nx, ae_int_t ny, rbfmodel* s, ae_state *_state)
{

    _rbfmodel_clear(s);

    ae_assert(nx>=1, "RBFCreate: NX<1", _state);
    ae_assert(ny>=1, "RBFCreate: NY<1", _state);
    s->nx = nx;
    s->ny = ny;
    rbf_rbfpreparenonserializablefields(s, _state);
    
    /*
     * Select default model version according to NX.
     *
     * The idea is that when we call this function with NX=2 or NX=3, backward
     * compatible dummy (zero) V1 model is created, so serialization produces
     * model which are compatible with pre-3.11 ALGLIB.
     */
    rbf_initializev1(nx, ny, &s->model1, _state);
    rbf_initializev2(nx, ny, &s->model2, _state);
    rbf_initializev3(nx, ny, &s->model3, _state);
    if( nx==2||nx==3 )
    {
        s->modelversion = 1;
    }
    else
    {
        s->modelversion = 2;
    }
    
    /*
     * Report fields
     */
    s->progress10000 = 0;
    s->terminationrequest = ae_false;
    
    /*
     * Prepare buffers
     */
    rbfcreatecalcbuffer(s, &s->calcbuf, _state);
}


/*************************************************************************
This function creates buffer  structure  which  can  be  used  to  perform
parallel  RBF  model  evaluations  (with  one  RBF  model  instance  being
used from multiple threads, as long as  different  threads  use  different
instances of the buffer).

This buffer object can be used with  rbftscalcbuf()  function  (here  "ts"
stands for "thread-safe", "buf" is a suffix which denotes  function  which
reuses previously allocated output space).

A buffer creation function (this function) is also thread-safe.  I.e.  you
may safely create multiple buffers for the same  RBF  model  from multiple
threads.

NOTE: the  buffer  object  is  just  a  collection of several preallocated
      dynamic arrays and precomputed values. If you  delete  its  "parent"
      RBF model when the buffer is still alive, nothing  bad  will  happen
      (no dangling pointers or resource leaks).  The  buffer  will  simply
      become useless.

How to use it:
* create RBF model structure with rbfcreate()
* load data, tune parameters
* call rbfbuildmodel()
* call rbfcreatecalcbuffer(), once per thread working with RBF model  (you
  should call this function only AFTER call to rbfbuildmodel(), see  below
  for more information)
* call rbftscalcbuf() from different threads,  with  each  thread  working
  with its own copy of buffer object.
* it is recommended to reuse buffer as much  as  possible  because  buffer
  creation involves allocation of several large dynamic arrays.  It  is  a
  huge waste of resource to use it just once.

INPUT PARAMETERS
    S           -   RBF model

OUTPUT PARAMETERS
    Buf         -   external buffer.
    
IMPORTANT: buffer object should be used only with  RBF model object  which
           was used to initialize buffer. Any attempt to use buffer   with
           different object is dangerous - you may  get  memory  violation
           error because sizes of internal arrays do not fit to dimensions
           of RBF structure.
           
IMPORTANT: you  should  call  this function only for model which was built
           with rbfbuildmodel() function, after successful  invocation  of
           rbfbuildmodel().  Sizes   of   some   internal  structures  are
           determined only after model is built, so buffer object  created
           before model  construction  stage  will  be  useless  (and  any
           attempt to use it will result in exception).

  -- ALGLIB --
     Copyright 02.04.2016 by Sergey Bochkanov
*************************************************************************/
void rbfcreatecalcbuffer(const rbfmodel* s,
     rbfcalcbuffer* buf,
     ae_state *_state)
{

    _rbfcalcbuffer_clear(buf);

    if( s->modelversion==1 )
    {
        buf->modelversion = 1;
        rbfv1createcalcbuffer(&s->model1, &buf->bufv1, _state);
        return;
    }
    if( s->modelversion==2 )
    {
        buf->modelversion = 2;
        rbfv2createcalcbuffer(&s->model2, &buf->bufv2, _state);
        return;
    }
    if( s->modelversion==3 )
    {
        buf->modelversion = 3;
        rbfv3createcalcbuffer(&s->model3, &buf->bufv3, _state);
        return;
    }
    ae_assert(ae_false, "RBFCreateCalcBuffer: integrity check failed", _state);
}


/*************************************************************************
This function adds dataset.

This function overrides results of the previous calls, i.e. multiple calls
of this function will result in only the last set being added.

IMPORTANT: ALGLIB version 3.11 and later allows you to specify  a  set  of
           per-dimension scales. Interpolation radii are multiplied by the
           scale vector. It may be useful if you have mixed spatio-temporal
           data (say, a set of 3D slices recorded at different times).
           You should call rbfsetpointsandscales() function  to  use  this
           feature.

INPUT PARAMETERS:
    S       -   RBF model, initialized by rbfcreate() call.
    XY      -   points, array[N,NX+NY]. One row corresponds to  one  point
                in the dataset. First NX elements  are  coordinates,  next
                NY elements are function values. Array may  be larger than 
                specified, in  this  case  only leading [N,NX+NY] elements 
                will be used.
    N       -   number of points in the dataset

After you've added dataset and (optionally) tuned algorithm  settings  you
should call rbfbuildmodel() in order to build a model for you.

NOTE: dataset added by this function is not saved during model serialization.
      MODEL ITSELF is serialized, but data used to build it are not.
      
      So, if you 1) add dataset to  empty  RBF  model,  2)  serialize  and
      unserialize it, then you will get an empty RBF model with no dataset
      being attached.
      
      From the other side, if you call rbfbuildmodel() between (1) and (2),
      then after (2) you will get your fully constructed RBF model  -  but
      again with no dataset attached, so subsequent calls to rbfbuildmodel()
      will produce empty model.
      

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
void rbfsetpoints(rbfmodel* s,
     /* Real    */ const ae_matrix* xy,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;


    ae_assert(n>0, "RBFSetPoints: N<=0", _state);
    ae_assert(xy->rows>=n, "RBFSetPoints: Rows(XY)<N", _state);
    ae_assert(xy->cols>=s->nx+s->ny, "RBFSetPoints: Cols(XY)<NX+NY", _state);
    ae_assert(apservisfinitematrix(xy, n, s->nx+s->ny, _state), "RBFSetPoints: XY contains infinite or NaN values!", _state);
    s->n = n;
    s->hasscale = ae_false;
    ae_matrix_set_length(&s->x, s->n, s->nx, _state);
    ae_matrix_set_length(&s->y, s->n, s->ny, _state);
    for(i=0; i<=s->n-1; i++)
    {
        for(j=0; j<=s->nx-1; j++)
        {
            s->x.ptr.pp_double[i][j] = xy->ptr.pp_double[i][j];
        }
        for(j=0; j<=s->ny-1; j++)
        {
            s->y.ptr.pp_double[i][j] = xy->ptr.pp_double[i][j+s->nx];
        }
    }
}


/*************************************************************************
This function adds dataset and a vector of per-dimension scales.

It may be useful if you have mixed spatio-temporal data - say, a set of 3D
slices recorded at different times. Such data typically require  different
RBF radii for spatial and temporal dimensions. ALGLIB solves this  problem
by specifying single RBF radius, which is (optionally) multiplied  by  the
scale vector.

This function overrides results of the previous calls, i.e. multiple calls
of this function will result in only the last set being added.

IMPORTANT: only modern RBF algorithms  support  variable  scaling.  Legacy
           algorithms like RBF-ML or QNN algorithms  will  result  in   -3
           completion code being returned (incorrect algorithm).

INPUT PARAMETERS:
    R       -   RBF model, initialized by rbfcreate() call.
    XY      -   points, array[N,NX+NY]. One row corresponds to  one  point
                in the dataset. First NX elements  are  coordinates,  next
                NY elements are function values. Array may  be larger than 
                specified, in  this  case  only leading [N,NX+NY] elements 
                will be used.
    N       -   number of points in the dataset
    S       -   array[NX], scale vector, S[i]>0.

After you've added dataset and (optionally) tuned algorithm  settings  you
should call rbfbuildmodel() in order to build a model for you.

NOTE: dataset added by this function is not saved during model serialization.
      MODEL ITSELF is serialized, but data used to build it are not.
      
      So, if you 1) add dataset to  empty  RBF  model,  2)  serialize  and
      unserialize it, then you will get an empty RBF model with no dataset
      being attached.
      
      From the other side, if you call rbfbuildmodel() between (1) and (2),
      then after (2) you will get your fully constructed RBF model  -  but
      again with no dataset attached, so subsequent calls to rbfbuildmodel()
      will produce empty model.
      

  -- ALGLIB --
     Copyright 20.06.2016 by Bochkanov Sergey
*************************************************************************/
void rbfsetpointsandscales(rbfmodel* r,
     /* Real    */ const ae_matrix* xy,
     ae_int_t n,
     /* Real    */ const ae_vector* s,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;


    ae_assert(n>0, "RBFSetPointsAndScales: N<=0", _state);
    ae_assert(xy->rows>=n, "RBFSetPointsAndScales: Rows(XY)<N", _state);
    ae_assert(xy->cols>=r->nx+r->ny, "RBFSetPointsAndScales: Cols(XY)<NX+NY", _state);
    ae_assert(s->cnt>=r->nx, "RBFSetPointsAndScales: Length(S)<NX", _state);
    r->n = n;
    r->hasscale = ae_true;
    ae_matrix_set_length(&r->x, r->n, r->nx, _state);
    ae_matrix_set_length(&r->y, r->n, r->ny, _state);
    for(i=0; i<=r->n-1; i++)
    {
        for(j=0; j<=r->nx-1; j++)
        {
            r->x.ptr.pp_double[i][j] = xy->ptr.pp_double[i][j];
        }
        for(j=0; j<=r->ny-1; j++)
        {
            r->y.ptr.pp_double[i][j] = xy->ptr.pp_double[i][j+r->nx];
        }
    }
    ae_vector_set_length(&r->s, r->nx, _state);
    for(i=0; i<=r->nx-1; i++)
    {
        ae_assert(ae_isfinite(s->ptr.p_double[i], _state), "RBFSetPointsAndScales: S[i] is not finite number", _state);
        ae_assert(ae_fp_greater(s->ptr.p_double[i],(double)(0)), "RBFSetPointsAndScales: S[i]<=0", _state);
        r->s.ptr.p_double[i] = s->ptr.p_double[i];
    }
}


/*************************************************************************
DEPRECATED: this function is deprecated. ALGLIB  includes  new  RBF  model
            construction algorithms: DDM-RBF (since version 3.19) and HRBF
            (since version 3.11).

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
void rbfsetalgoqnn(rbfmodel* s, double q, double z, ae_state *_state)
{


    ae_assert(ae_isfinite(q, _state), "RBFSetAlgoQNN: Q is infinite or NAN", _state);
    ae_assert(ae_fp_greater(q,(double)(0)), "RBFSetAlgoQNN: Q<=0", _state);
    ae_assert(ae_isfinite(z, _state), "RBFSetAlgoQNN: Z is infinite or NAN", _state);
    ae_assert(ae_fp_greater(z,(double)(0)), "RBFSetAlgoQNN: Z<=0", _state);
    s->radvalue = q;
    s->radzvalue = z;
    s->algorithmtype = 1;
}


/*************************************************************************
DEPRECATED: this function is deprecated. ALGLIB  includes  new  RBF  model
            construction algorithms: DDM-RBF (since version 3.19) and HRBF
            (since version 3.11).
   
  -- ALGLIB --
     Copyright 02.03.2012 by Bochkanov Sergey
*************************************************************************/
void rbfsetalgomultilayer(rbfmodel* s,
     double rbase,
     ae_int_t nlayers,
     double lambdav,
     ae_state *_state)
{


    ae_assert(ae_isfinite(rbase, _state), "RBFSetAlgoMultiLayer: RBase is infinite or NaN", _state);
    ae_assert(ae_fp_greater(rbase,(double)(0)), "RBFSetAlgoMultiLayer: RBase<=0", _state);
    ae_assert(nlayers>=0, "RBFSetAlgoMultiLayer: NLayers<0", _state);
    ae_assert(ae_isfinite(lambdav, _state), "RBFSetAlgoMultiLayer: LambdaV is infinite or NAN", _state);
    ae_assert(ae_fp_greater_eq(lambdav,(double)(0)), "RBFSetAlgoMultiLayer: LambdaV<0", _state);
    s->radvalue = rbase;
    s->nlayers = nlayers;
    s->algorithmtype = 2;
    s->lambdav = lambdav;
}


/*************************************************************************
This function chooses HRBF solver, a 2nd version of ALGLIB RBFs.

This  algorithm is called Hierarchical RBF. It  similar  to  its  previous
incarnation, RBF-ML, i.e.  it  also  builds  a  sequence  of  models  with
decreasing radii. However, it uses more economical way of  building  upper
layers (ones with large radii), which results in faster model construction
and evaluation, as well as smaller memory footprint during construction.

This algorithm has following important features:
* ability to handle millions of points
* controllable smoothing via nonlinearity penalization
* support for specification of per-dimensional  radii  via  scale  vector,
  which is set by means of rbfsetpointsandscales() function. This  feature
  is useful if you solve  spatio-temporal  interpolation  problems,  where
  different radii are required for spatial and temporal dimensions.

Running times are roughly proportional to:
* N*log(N)*NLayers - for the model construction
* N*NLayers - for the model evaluation
You may see that running time does not depend on search radius  or  points
density, just on the number of layers in the hierarchy.

INPUT PARAMETERS:
    S       -   RBF model, initialized by rbfcreate() call
    RBase   -   RBase parameter, RBase>0
    NLayers -   NLayers parameter, NLayers>0, recommended value  to  start
                with - about 5.
    LambdaNS-   >=0, nonlinearity penalty coefficient, negative values are
                not allowed. This parameter adds controllable smoothing to
                the problem, which may reduce noise. Specification of non-
                zero lambda means that in addition to fitting error solver
                will  also  minimize   LambdaNS*|S''(x)|^2  (appropriately
                generalized to multiple dimensions.
                
                Specification of exactly zero value means that no  penalty
                is added  (we  do  not  even  evaluate  matrix  of  second
                derivatives which is necessary for smoothing).
                
                Calculation of nonlinearity penalty is costly - it results
                in  several-fold  increase  of  model  construction  time.
                Evaluation time remains the same.
                
                Optimal  lambda  is  problem-dependent and requires  trial
                and  error.  Good  value to  start  from  is  1e-5...1e-6,
                which corresponds to slightly noticeable smoothing  of the
                function.  Value  1e-2  usually  means  that  quite  heavy
                smoothing is applied.

TUNING ALGORITHM

In order to use this algorithm you have to choose three parameters:
* initial radius RBase
* number of layers in the model NLayers
* penalty coefficient LambdaNS

Initial radius is easy to choose - you can pick any number  several  times
larger  than  the  average  distance between points. Algorithm won't break
down if you choose radius which is too large (model construction time will
increase, but model will be built correctly).

Choose such number of layers that RLast=RBase/2^(NLayers-1)  (radius  used
by  the  last  layer)  will  be  smaller than the typical distance between
points.  In  case  model  error  is  too large, you can increase number of
layers.  Having  more  layers  will make model construction and evaluation
proportionally slower, but it will allow you to have model which precisely
fits your data. From the other side, if you want to  suppress  noise,  you
can DECREASE number of layers to make your model less flexible (or specify
non-zero LambdaNS).

TYPICAL ERRORS

1. Using too small number of layers - RBF models with large radius are not
   flexible enough to reproduce small variations in the  target  function.
   You  need  many  layers  with  different radii, from large to small, in
   order to have good model.

2. Using  initial  radius  which  is  too  small.  You will get model with
   "holes" in the areas which are too far away from interpolation centers.
   However, algorithm will work correctly (and quickly) in this case.

  -- ALGLIB --
     Copyright 20.06.2016 by Bochkanov Sergey
*************************************************************************/
void rbfsetalgohierarchical(rbfmodel* s,
     double rbase,
     ae_int_t nlayers,
     double lambdans,
     ae_state *_state)
{


    ae_assert(ae_isfinite(rbase, _state), "RBFSetAlgoHierarchical: RBase is infinite or NaN", _state);
    ae_assert(ae_fp_greater(rbase,(double)(0)), "RBFSetAlgoHierarchical: RBase<=0", _state);
    ae_assert(nlayers>=0, "RBFSetAlgoHierarchical: NLayers<0", _state);
    ae_assert(ae_isfinite(lambdans, _state)&&ae_fp_greater_eq(lambdans,(double)(0)), "RBFSetAlgoHierarchical: LambdaNS<0 or infinite", _state);
    s->radvalue = rbase;
    s->nlayers = nlayers;
    s->algorithmtype = 3;
    s->lambdav = lambdans;
}


/*************************************************************************
This function chooses a thin plate  spline  DDM-RBF  solver,  a  fast  RBF
solver with f(r)=r^2*ln(r) basis function.

This algorithm has following important features:
* easy setup - no tunable parameters
* C1 continuous RBF model (gradient is defined everywhere, but Hessian  is
  undefined at nodes), high-quality interpolation
* fast  model construction algorithm with O(N) memory and  O(N^2)  running
  time requirements. Hundreds of thousands of points can be  handled  with
  this algorithm.
* controllable smoothing via optional nonlinearity penalty

INPUT PARAMETERS:
    S       -   RBF model, initialized by rbfcreate() call
    LambdaV -   smoothing parameter, LambdaV>=0, defaults to 0.0:
                * LambdaV=0 means that no smoothing is applied,  i.e.  the
                  spline tries to pass through all dataset points exactly
                * LambdaV>0 means that a smoothing thin  plate  spline  is
                  built, with larger LambdaV corresponding to models  with
                  less nonlinearities. Smoothing spline reproduces  target
                  values at nodes with small error; from the  other  side,
                  it is much more stable.
                  Recommended values:
                  * 1.0E-6 for minimal stability improving smoothing
                  * 1.0E-3 a good value to start experiments; first results
                    are visible
                  * 1.0 for strong smoothing

IMPORTANT: this model construction algorithm was introduced in ALGLIB 3.19
           and  produces  models  which  are  INCOMPATIBLE  with  previous
           versions of ALGLIB. You can  not  unserialize  models  produced
           with this function in ALGLIB 3.18 or earlier.
           
NOTE:      polyharmonic RBFs, including thin plate splines,  are  somewhat
           slower than compactly supported RBFs built with  HRBF algorithm
           due to the fact that non-compact basis function does not vanish
           far away from the nodes. From the other side, polyharmonic RBFs
           often produce much better results than HRBFs.

NOTE:      this algorithm supports specification of per-dimensional  radii
           via scale vector, which is set by means of rbfsetpointsandscales()
           function. This feature is useful if  you solve  spatio-temporal
           interpolation problems where different radii are  required  for
           spatial and temporal dimensions.

  -- ALGLIB --
     Copyright 12.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfsetalgothinplatespline(rbfmodel* s,
     double lambdav,
     ae_state *_state)
{


    ae_assert(ae_isfinite(lambdav, _state), "RBFSetAlgoThinPlateSpline: LambdaV is not finite number", _state);
    ae_assert(ae_fp_greater_eq(lambdav,(double)(0)), "RBFSetAlgoThinPlateSpline: LambdaV is negative", _state);
    s->algorithmtype = 4;
    s->bftype = 2;
    s->bfparam = (double)(0);
    s->lambdav = lambdav;
}


/*************************************************************************
This function chooses a multiquadric DDM-RBF solver,  a  fast  RBF  solver
with f(r)=sqrt(r^2+Alpha^2) as a basis function,  with  manual  choice  of
the scale parameter Alpha.

This algorithm has following important features:
* C2 continuous RBF model (when Alpha>0 is used; for Alpha=0 the model  is
  merely C0 continuous)
* fast  model construction algorithm with O(N) memory and  O(N^2)  running
  time requirements. Hundreds of thousands of points can be  handled  with
  this algorithm.
* controllable smoothing via optional nonlinearity penalty
  
One important point is that  this  algorithm  includes  tunable  parameter
Alpha, which should be carefully chosen. Selecting too  large  value  will
result in extremely badly  conditioned  problems  (interpolation  accuracy
may degrade up to complete breakdown) whilst selecting too small value may
produce models that are precise but nearly nonsmooth at the nodes.

Good value to  start  from  is  mean  distance  between  nodes. Generally,
choosing too small Alpha is better than choosing too large - in the former
case you still have model that reproduces target values at the nodes.

In most cases, better option is to choose good Alpha automatically - it is
done by another version of the same algorithm that is activated by calling
rbfsetalgomultiquadricauto() method.

INPUT PARAMETERS:
    S       -   RBF model, initialized by rbfcreate() call
    Alpha   -   basis function parameter, Alpha>=0:
                * Alpha>0  means that multiquadric algorithm is used which
                  produces C2-continuous RBF model
                * Alpha=0  means that the multiquadric kernel  effectively
                  becomes a biharmonic one: f=r. As a  result,  the  model
                  becomes nonsmooth at nodes, and hence is C0 continuous
    LambdaV -   smoothing parameter, LambdaV>=0, defaults to 0.0:
                * LambdaV=0 means that no smoothing is applied,  i.e.  the
                  spline tries to pass through all dataset points exactly
                * LambdaV>0 means that a multiquadric spline is built with
                  larger  LambdaV   corresponding   to  models  with  less
                  nonlinearities.  Smoothing   spline   reproduces  target
                  values at nodes with small error; from the  other  side,
                  it is much more stable.
                  Recommended values:
                  * 1.0E-6 for minimal stability improving smoothing
                  * 1.0E-3 a good value to start experiments; first results
                    are visible
                  * 1.0 for strong smoothing

IMPORTANT: this model construction algorithm was introduced in ALGLIB 3.19
           and  produces  models  which  are  INCOMPATIBLE  with  previous
           versions of ALGLIB. You can  not  unserialize  models  produced
           with this function in ALGLIB 3.18 or earlier.
           
NOTE:      polyharmonic RBFs, including thin plate splines,  are  somewhat
           slower than compactly supported RBFs built with  HRBF algorithm
           due to the fact that non-compact basis function does not vanish
           far away from the nodes. From the other side, polyharmonic RBFs
           often produce much better results than HRBFs.

NOTE:      this algorithm supports specification of per-dimensional  radii
           via scale vector, which is set by means of rbfsetpointsandscales()
           function. This feature is useful if  you solve  spatio-temporal
           interpolation problems where different radii are  required  for
           spatial and temporal dimensions.

  -- ALGLIB --
     Copyright 12.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfsetalgomultiquadricmanual(rbfmodel* s,
     double alpha,
     double lambdav,
     ae_state *_state)
{


    ae_assert(ae_isfinite(alpha, _state), "RBFSetAlgoMultiquadricManual: Alpha is infinite or NAN", _state);
    ae_assert(ae_fp_greater_eq(alpha,(double)(0)), "RBFSetAlgoMultiquadricManual: Alpha<0", _state);
    ae_assert(ae_isfinite(lambdav, _state), "RBFSetAlgoMultiquadricManual: LambdaV is not finite number", _state);
    ae_assert(ae_fp_greater_eq(lambdav,(double)(0)), "RBFSetAlgoMultiquadricManual: LambdaV is negative", _state);
    s->algorithmtype = 4;
    s->bftype = 1;
    s->bfparam = alpha;
    s->lambdav = lambdav;
}


/*************************************************************************
This function chooses a multiquadric DDM-RBF solver,  a  fast  RBF  solver
with f(r)=sqrt(r^2+Alpha^2)  as  a  basis  function,  with   Alpha   being
automatically determined.

This algorithm has following important features:
* easy setup - no need to tune Alpha, good value is automatically assigned
* C2 continuous RBF model
* fast  model construction algorithm with O(N) memory and  O(N^2)  running
  time requirements. Hundreds of thousands of points can be  handled  with
  this algorithm.
* controllable smoothing via optional nonlinearity penalty

This algorithm automatically selects Alpha  as  a  mean  distance  to  the
nearest neighbor (ignoring neighbors that are too close).

INPUT PARAMETERS:
    S       -   RBF model, initialized by rbfcreate() call
    LambdaV -   smoothing parameter, LambdaV>=0, defaults to 0.0:
                * LambdaV=0 means that no smoothing is applied,  i.e.  the
                  spline tries to pass through all dataset points exactly
                * LambdaV>0 means that a multiquadric spline is built with
                  larger  LambdaV   corresponding   to  models  with  less
                  nonlinearities.  Smoothing   spline   reproduces  target
                  values at nodes with small error; from the  other  side,
                  it is much more stable.
                  Recommended values:
                  * 1.0E-6 for minimal stability improving smoothing
                  * 1.0E-3 a good value to start experiments; first results
                    are visible
                  * 1.0 for strong smoothing

IMPORTANT: this model construction algorithm was introduced in ALGLIB 3.19
           and  produces  models  which  are  INCOMPATIBLE  with  previous
           versions of ALGLIB. You can  not  unserialize  models  produced
           with this function in ALGLIB 3.18 or earlier.
           
NOTE:      polyharmonic RBFs, including thin plate splines,  are  somewhat
           slower than compactly supported RBFs built with  HRBF algorithm
           due to the fact that non-compact basis function does not vanish
           far away from the nodes. From the other side, polyharmonic RBFs
           often produce much better results than HRBFs.

NOTE:      this algorithm supports specification of per-dimensional  radii
           via scale vector, which is set by means of rbfsetpointsandscales()
           function. This feature is useful if  you solve  spatio-temporal
           interpolation problems where different radii are  required  for
           spatial and temporal dimensions.

  -- ALGLIB --
     Copyright 12.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfsetalgomultiquadricauto(rbfmodel* s,
     double lambdav,
     ae_state *_state)
{


    ae_assert(ae_isfinite(lambdav, _state), "RBFSetAlgoMultiquadricAuto: LambdaV is not finite number", _state);
    ae_assert(ae_fp_greater_eq(lambdav,(double)(0)), "RBFSetAlgoMultiquadricAuto: LambdaV is negative", _state);
    s->algorithmtype = 4;
    s->bftype = 1;
    s->bfparam = -1.0;
    s->lambdav = lambdav;
}


/*************************************************************************
This  function  chooses  a  biharmonic DDM-RBF solver, a fast  RBF  solver
with f(r)=r as a basis function.

This algorithm has following important features:
* no tunable parameters
* C0 continuous RBF model (the model has discontinuous derivatives at  the
  interpolation nodes)
* fast model construction algorithm with O(N) memory and O(N*logN) running
  time requirements. Hundreds of thousands of points can be  handled  with
  this algorithm.
* accelerated evaluation using far field expansions  (aka  fast multipoles
  method) is supported. See rbffastcalc() for more information.
* controllable smoothing via optional nonlinearity penalty

INPUT PARAMETERS:
    S       -   RBF model, initialized by rbfcreate() call
    LambdaV -   smoothing parameter, LambdaV>=0, defaults to 0.0:
                * LambdaV=0 means that no smoothing is applied,  i.e.  the
                  spline tries to pass through all dataset points exactly
                * LambdaV>0 means that a multiquadric spline is built with
                  larger  LambdaV   corresponding   to  models  with  less
                  nonlinearities.  Smoothing   spline   reproduces  target
                  values at nodes with small error; from the  other  side,
                  it is much more stable.
                  Recommended values:
                  * 1.0E-6 for minimal stability improving smoothing
                  * 1.0E-3 a good value to start experiments; first results
                    are visible
                  * 1.0 for strong smoothing

IMPORTANT: this model construction algorithm was introduced in ALGLIB 3.19
           and  produces  models  which  are  INCOMPATIBLE  with  previous
           versions of ALGLIB. You can  not  unserialize  models  produced
           with this function in ALGLIB 3.18 or earlier.
           
NOTE:      polyharmonic RBFs, including thin plate splines,  are  somewhat
           slower than compactly supported RBFs built with  HRBF algorithm
           due to the fact that non-compact basis function does not vanish
           far away from the nodes. From the other side, polyharmonic RBFs
           often produce much better results than HRBFs.

NOTE:      this algorithm supports specification of per-dimensional  radii
           via scale vector, which is set by means of rbfsetpointsandscales()
           function. This feature is useful if  you solve  spatio-temporal
           interpolation problems where different radii are  required  for
           spatial and temporal dimensions.

  -- ALGLIB --
     Copyright 12.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfsetalgobiharmonic(rbfmodel* s, double lambdav, ae_state *_state)
{


    ae_assert(ae_isfinite(lambdav, _state), "RBFSetAlgoBiharmonic: LambdaV is not finite number", _state);
    ae_assert(ae_fp_greater_eq(lambdav,(double)(0)), "RBFSetAlgoBiharmonic: LambdaV is negative", _state);
    s->algorithmtype = 4;
    s->bftype = 1;
    s->bfparam = (double)(0);
    s->lambdav = lambdav;
}


/*************************************************************************
This function sets linear term (model is a sum of radial  basis  functions
plus linear polynomial). This function won't have effect until  next  call 
to RBFBuildModel().

Using linear term is a default option and it is the best one - it provides
best convergence guarantees for all RBF model  types: legacy  RBF-QNN  and
RBF-ML, Gaussian HRBFs and all types of DDM-RBF models.

Other options, like constant or zero term, work for HRBFs,  almost  always
work for DDM-RBFs but provide no stability  guarantees  in the latter case
(e.g. the solver may fail on some carefully prepared problems).

INPUT PARAMETERS:
    S       -   RBF model, initialized by RBFCreate() call

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
void rbfsetlinterm(rbfmodel* s, ae_state *_state)
{


    s->aterm = 1;
}


/*************************************************************************
This function sets constant term (model is a sum of radial basis functions
plus constant).  This  function  won't  have  effect  until  next  call to 
RBFBuildModel().

IMPORTANT: thin plate splines require  polynomial term to be  linear,  not
           constant,  in  order  to  provide   interpolation   guarantees.
           Although  failures  are  exceptionally  rare,  some  small  toy
           problems may result in degenerate linear systems. Thus,  it  is
           advised to use linear term when one fits data with TPS.

INPUT PARAMETERS:
    S       -   RBF model, initialized by RBFCreate() call

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
void rbfsetconstterm(rbfmodel* s, ae_state *_state)
{


    s->aterm = 2;
}


/*************************************************************************
This  function  sets  zero  term (model is a sum of radial basis functions 
without polynomial term). This function won't have effect until next  call
to RBFBuildModel().

IMPORTANT: only  Gaussian  RBFs  (HRBF  algorithm)  provide  interpolation
           guarantees when no polynomial term is used.  Most  other  RBFs,
           including   biharmonic  splines,   thin   plate   splines   and
           multiquadrics, require at least constant term  (biharmonic  and
           multiquadric) or linear one (thin plate splines)  in  order  to
           guarantee non-degeneracy of linear systems being solved.
           
           Although  failures  are  exceptionally  rare,  some  small  toy
           problems still may result in degenerate linear systems. Thus,it
           is advised to use constant/linear term, unless one is 100% sure
           that he needs zero term.

INPUT PARAMETERS:
    S       -   RBF model, initialized by RBFCreate() call

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
void rbfsetzeroterm(rbfmodel* s, ae_state *_state)
{


    s->aterm = 3;
}


/*************************************************************************
This function sets basis function type, which can be:
* 0 for classic Gaussian
* 1 for fast and compact bell-like basis function, which  becomes  exactly
  zero at distance equal to 3*R (default option).

INPUT PARAMETERS:
    S       -   RBF model, initialized by RBFCreate() call
    BF      -   basis function type:
                * 0 - classic Gaussian
                * 1 - fast and compact one

  -- ALGLIB --
     Copyright 01.02.2017 by Bochkanov Sergey
*************************************************************************/
void rbfsetv2bf(rbfmodel* s, ae_int_t bf, ae_state *_state)
{


    ae_assert(bf==0||bf==1, "RBFSetV2Its: BF<>0 and BF<>1", _state);
    s->model2.basisfunction = bf;
}


/*************************************************************************
This function sets stopping criteria of the underlying linear  solver  for
hierarchical (version 2) RBF constructor.

INPUT PARAMETERS:
    S       -   RBF model, initialized by RBFCreate() call
    MaxIts  -   this criterion will stop algorithm after MaxIts iterations.
                Typically a few hundreds iterations is required,  with 400
                being a good default value to start experimentation.
                Zero value means that default value will be selected.

  -- ALGLIB --
     Copyright 01.02.2017 by Bochkanov Sergey
*************************************************************************/
void rbfsetv2its(rbfmodel* s, ae_int_t maxits, ae_state *_state)
{


    ae_assert(maxits>=0, "RBFSetV2Its: MaxIts is negative", _state);
    s->model2.maxits = maxits;
}


/*************************************************************************
This function sets support radius parameter  of  hierarchical  (version 2)
RBF constructor.

Hierarchical RBF model achieves great speed-up  by removing from the model
excessive (too dense) nodes. Say, if you have RBF radius equal to 1 meter,
and two nodes are just 1 millimeter apart, you  may  remove  one  of  them
without reducing model quality.

Support radius parameter is used to justify which points need removal, and
which do not. If two points are less than  SUPPORT_R*CUR_RADIUS  units  of
distance apart, one of them is removed from the model. The larger  support
radius  is, the faster model  construction  AND  evaluation are.  However,
too large values result in "bumpy" models.

INPUT PARAMETERS:
    S       -   RBF model, initialized by RBFCreate() call
    R       -   support radius coefficient, >=0.
                Recommended values are [0.1,0.4] range, with 0.1 being
                default value.

  -- ALGLIB --
     Copyright 01.02.2017 by Bochkanov Sergey
*************************************************************************/
void rbfsetv2supportr(rbfmodel* s, double r, ae_state *_state)
{


    ae_assert(ae_isfinite(r, _state), "RBFSetV2SupportR: R is not finite", _state);
    ae_assert(ae_fp_greater_eq(r,(double)(0)), "RBFSetV2SupportR: R<0", _state);
    s->model2.supportr = r;
}


/*************************************************************************
This function sets desired accuracy for a version 3 RBF model.

As of ALGLIB 3.20.0, version 3 models include biharmonic RBFs, thin  plate
splines, multiquadrics.

Version 3 models are fit  with  specialized  domain  decomposition  method
which splits problem into smaller  chunks.  Models  with  size  less  than
the DDM chunk size are computed nearly exactly in one step. Larger  models
are built with an iterative linear solver. This function controls accuracy
of the solver.

INPUT PARAMETERS:
    S       -   RBF model, initialized by RBFCreate() call
    TOL     -   desired precision:
                * must be non-negative
                * should be somewhere between 0.001 and 0.000001
                * values higher than 0.001 make little sense   -  you  may
                  lose a lot of precision with no performance gains.
                * values below 1E-6 usually require too much time to converge,
                  so they are silenly replaced by a 1E-6 cutoff value. Thus,
                  zero can be used to denote 'maximum precision'.

  -- ALGLIB --
     Copyright 01.10.2022 by Bochkanov Sergey
*************************************************************************/
void rbfsetv3tol(rbfmodel* s, double tol, ae_state *_state)
{


    ae_assert(ae_isfinite(tol, _state)&&ae_fp_greater_eq(tol,(double)(0)), "RBFSetV3TOL: TOL is negative or infinite", _state);
    s->v3tol = tol;
}


/*************************************************************************
This function sets stopping criteria of the underlying linear solver.

INPUT PARAMETERS:
    S       -   RBF model, initialized by RBFCreate() call
    EpsOrt  -   orthogonality stopping criterion, EpsOrt>=0. Algorithm will
                stop when ||A'*r||<=EpsOrt where A' is a transpose of  the 
                system matrix, r is a residual vector.
                Recommended value of EpsOrt is equal to 1E-6.
                This criterion will stop algorithm when we have "bad fit"
                situation, i.e. when we should stop in a point with large,
                nonzero residual.
    EpsErr  -   residual stopping  criterion.  Algorithm  will  stop  when
                ||r||<=EpsErr*||b||, where r is a residual vector, b is  a
                right part of the system (function values).
                Recommended value of EpsErr is equal to 1E-3 or 1E-6.
                This  criterion  will  stop  algorithm  in  a  "good  fit" 
                situation when we have near-zero residual near the desired
                solution.
    MaxIts  -   this criterion will stop algorithm after MaxIts iterations.
                It should be used for debugging purposes only!
                Zero MaxIts means that no limit is placed on the number of
                iterations.

We  recommend  to  set  moderate  non-zero  values   EpsOrt   and   EpsErr 
simultaneously. Values equal to 10E-6 are good to start with. In case  you
need high performance and do not need high precision ,  you  may  decrease
EpsErr down to 0.001. However, we do not recommend decreasing EpsOrt.

As for MaxIts, we recommend to leave it zero unless you know what you do.

NOTE: this   function  has   some   serialization-related  subtleties.  We
      recommend you to study serialization examples from ALGLIB  Reference
      Manual if you want to perform serialization of your models.

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
void rbfsetcond(rbfmodel* s,
     double epsort,
     double epserr,
     ae_int_t maxits,
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsort, _state)&&ae_fp_greater_eq(epsort,(double)(0)), "RBFSetCond: EpsOrt is negative, INF or NAN", _state);
    ae_assert(ae_isfinite(epserr, _state)&&ae_fp_greater_eq(epserr,(double)(0)), "RBFSetCond: EpsB is negative, INF or NAN", _state);
    ae_assert(maxits>=0, "RBFSetCond: MaxIts is negative", _state);
    if( (ae_fp_eq(epsort,(double)(0))&&ae_fp_eq(epserr,(double)(0)))&&maxits==0 )
    {
        s->epsort = rbf_eps;
        s->epserr = rbf_eps;
        s->maxits = 0;
    }
    else
    {
        s->epsort = epsort;
        s->epserr = epserr;
        s->maxits = maxits;
    }
}


/*************************************************************************
This   function  builds  RBF  model  and  returns  report  (contains  some 
information which can be used for evaluation of the algorithm properties).

Call to this function modifies RBF model by calculating its centers/radii/
weights  and  saving  them  into  RBFModel  structure.  Initially RBFModel 
contain zero coefficients, but after call to this function  we  will  have
coefficients which were calculated in order to fit our dataset.

After you called this function you can call RBFCalc(),  RBFGridCalc()  and
other model calculation functions.

INPUT PARAMETERS:
    S       -   RBF model, initialized by RBFCreate() call
    Rep     -   report:
                * Rep.TerminationType:
                  * -5 - non-distinct basis function centers were detected,
                         interpolation  aborted;  only  QNN  returns  this
                         error   code, other  algorithms  can  handle non-
                         distinct nodes.
                  * -4 - nonconvergence of the internal SVD solver
                  * -3   incorrect model construction algorithm was chosen:
                         QNN or RBF-ML, combined with one of the incompatible
                         features:
                         * NX=1 or NX>3
                         * points with per-dimension scales.
                  *  1 - successful termination
                  *  8 - a termination request was submitted via
                         rbfrequesttermination() function.
                
                Fields which are set only by modern RBF solvers (hierarchical
                or nonnegative; older solvers like QNN and ML initialize these
                fields by NANs):
                * rep.rmserror - root-mean-square error at nodes
                * rep.maxerror - maximum error at nodes
                
                Fields are used for debugging purposes:
                * Rep.IterationsCount - iterations count of the LSQR solver
                * Rep.NMV - number of matrix-vector products
                * Rep.ARows - rows count for the system matrix
                * Rep.ACols - columns count for the system matrix
                * Rep.ANNZ - number of significantly non-zero elements
                  (elements above some algorithm-determined threshold)

NOTE:  failure  to  build  model will leave current state of the structure
unchanged.

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
void rbfbuildmodel(rbfmodel* s, rbfreport* rep, ae_state *_state)
{
    ae_frame _frame_block;
    rbfv1report rep1;
    rbfv2report rep2;
    rbfv3report rep3;
    ae_matrix x3;
    ae_vector scalevec;
    ae_int_t i;
    ae_int_t v3bftype;
    double v3bfparam;
    ae_int_t curalgorithmtype;

    ae_frame_make(_state, &_frame_block);
    memset(&rep1, 0, sizeof(rep1));
    memset(&rep2, 0, sizeof(rep2));
    memset(&rep3, 0, sizeof(rep3));
    memset(&x3, 0, sizeof(x3));
    memset(&scalevec, 0, sizeof(scalevec));
    _rbfreport_clear(rep);
    _rbfv1report_init(&rep1, _state, ae_true);
    _rbfv2report_init(&rep2, _state, ae_true);
    _rbfv3report_init(&rep3, _state, ae_true);
    ae_matrix_init(&x3, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&scalevec, 0, DT_REAL, _state, ae_true);

    
    /*
     * Clean fields prior to processing
     */
    rbf_clearreportfields(rep, _state);
    s->progress10000 = 0;
    s->terminationrequest = ae_false;
    
    /*
     * Autoselect algorithm
     */
    v3bftype = -999;
    v3bfparam = 0.0;
    if( s->algorithmtype==0 )
    {
        curalgorithmtype = 4;
        v3bftype = 2;
        v3bfparam = 0.0;
    }
    else
    {
        curalgorithmtype = s->algorithmtype;
        if( s->algorithmtype==4 )
        {
            v3bftype = s->bftype;
            v3bfparam = s->bfparam;
        }
    }
    
    /*
     * Algorithms which generate V1 models
     */
    if( curalgorithmtype==1||curalgorithmtype==2 )
    {
        
        /*
         * Perform compatibility checks
         */
        if( (s->nx<2||s->nx>3)||s->hasscale )
        {
            rep->terminationtype = -3;
            ae_frame_leave(_state);
            return;
        }
        
        /*
         * Try to build model.
         *
         * NOTE: due to historical reasons RBFV1BuildModel() accepts points
         *       cast to 3-dimensional space, even if they are really 2-dimensional.
         *       So, for 2D data we have to explicitly convert them to 3D.
         */
        if( s->nx==2 )
        {
            
            /*
             * Convert data to 3D
             */
            rmatrixsetlengthatleast(&x3, s->n, 3, _state);
            for(i=0; i<=s->n-1; i++)
            {
                x3.ptr.pp_double[i][0] = s->x.ptr.pp_double[i][0];
                x3.ptr.pp_double[i][1] = s->x.ptr.pp_double[i][1];
                x3.ptr.pp_double[i][2] = (double)(0);
            }
            rbfv1buildmodel(&x3, &s->y, s->n, s->aterm, curalgorithmtype, s->nlayers, s->radvalue, s->radzvalue, s->lambdav, s->epsort, s->epserr, s->maxits, &s->model1, &rep1, _state);
        }
        else
        {
            
            /*
             * Work with raw data
             */
            rbfv1buildmodel(&s->x, &s->y, s->n, s->aterm, curalgorithmtype, s->nlayers, s->radvalue, s->radzvalue, s->lambdav, s->epsort, s->epserr, s->maxits, &s->model1, &rep1, _state);
        }
        s->modelversion = 1;
        rbfcreatecalcbuffer(s, &s->calcbuf, _state);
        
        /*
         * Convert report fields
         */
        rep->arows = rep1.arows;
        rep->acols = rep1.acols;
        rep->annz = rep1.annz;
        rep->iterationscount = rep1.iterationscount;
        rep->nmv = rep1.nmv;
        rep->terminationtype = rep1.terminationtype;
        
        /*
         * Done
         */
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Algorithms which generate V2 models
     */
    if( curalgorithmtype==3 )
    {
        
        /*
         * Prepare scale vector - use unit values or user supplied ones
         */
        ae_vector_set_length(&scalevec, s->nx, _state);
        for(i=0; i<=s->nx-1; i++)
        {
            if( s->hasscale )
            {
                scalevec.ptr.p_double[i] = s->s.ptr.p_double[i];
            }
            else
            {
                scalevec.ptr.p_double[i] = (double)(1);
            }
        }
        
        /*
         * Build model
         */
        rbfv2buildhierarchical(&s->x, &s->y, s->n, &scalevec, s->aterm, s->nlayers, s->radvalue, s->lambdav, &s->model2, &s->progress10000, &s->terminationrequest, &rep2, _state);
        s->modelversion = 2;
        rbfcreatecalcbuffer(s, &s->calcbuf, _state);
        
        /*
         * Convert report fields
         */
        rep->terminationtype = rep2.terminationtype;
        rep->rmserror = rep2.rmserror;
        rep->maxerror = rep2.maxerror;
        
        /*
         * Done
         */
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Algorithms which generate DDM-RBF models
     */
    if( curalgorithmtype==4 )
    {
        
        /*
         * Prepare scale vector - use unit values or user supplied ones
         */
        ae_vector_set_length(&scalevec, s->nx, _state);
        for(i=0; i<=s->nx-1; i++)
        {
            if( s->hasscale )
            {
                scalevec.ptr.p_double[i] = s->s.ptr.p_double[i];
            }
            else
            {
                scalevec.ptr.p_double[i] = (double)(1);
            }
        }
        
        /*
         * Build model
         */
        rbfv3build(&s->x, &s->y, s->n, &scalevec, v3bftype, v3bfparam, s->lambdav, s->aterm, s->rbfprofile, s->v3tol, &s->model3, &s->progress10000, &s->terminationrequest, &rep3, _state);
        s->modelversion = 3;
        rbfcreatecalcbuffer(s, &s->calcbuf, _state);
        pushfastevaltol(s, s->fastevaltol, _state);
        
        /*
         * Convert report fields
         */
        rep->iterationscount = rep3.iterationscount;
        rep->terminationtype = rep3.terminationtype;
        rep->rmserror = rep3.rmserror;
        rep->maxerror = rep3.maxerror;
        
        /*
         * Done
         */
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Critical error
     */
    ae_assert(ae_false, "RBFBuildModel: integrity check failure", _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function calculates values of the 1-dimensional RBF model with scalar
output (NY=1) at the given point.

IMPORTANT: this function works only with modern  (hierarchical)  RBFs.  It 
           can not be used with legacy (version 1) RBFs because older  RBF
           code does not support 1-dimensional models.

IMPORTANT: THIS FUNCTION IS THREAD-UNSAFE. It uses fields of  rbfmodel  as
           temporary arrays, i.e. it is  impossible  to  perform  parallel
           evaluation on the same rbfmodel object (parallel calls of  this
           function for independent rbfmodel objects are safe).
           If you want to perform parallel model evaluation  from multiple
           threads, use rbftscalcbuf() with per-thread buffer object.

This function returns 0.0 when:
* the model is not initialized
* NX<>1
* NY<>1

INPUT PARAMETERS:
    S       -   RBF model
    X0      -   X-coordinate, finite number

RESULT:
    value of the model or 0.0 (as defined above)

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
double rbfcalc1(rbfmodel* s, double x0, ae_state *_state)
{
    double result;


    ae_assert(ae_isfinite(x0, _state), "RBFCalc1: invalid value for X0 (X0 is Inf)!", _state);
    result = (double)(0);
    if( s->ny!=1||s->nx!=1 )
    {
        return result;
    }
    if( s->modelversion==1 )
    {
        result = (double)(0);
        return result;
    }
    if( s->modelversion==2 )
    {
        result = rbfv2calc1(&s->model2, x0, _state);
        return result;
    }
    if( s->modelversion==3 )
    {
        result = rbfv3calc1(&s->model3, x0, _state);
        return result;
    }
    ae_assert(ae_false, "RBFCalc1: integrity check failed", _state);
    return result;
}


/*************************************************************************
This function calculates values of the 2-dimensional RBF model with scalar
output (NY=1) at the given point.

IMPORTANT: THIS FUNCTION IS THREAD-UNSAFE. It uses fields of  rbfmodel  as
           temporary arrays, i.e. it is  impossible  to  perform  parallel
           evaluation on the same rbfmodel object (parallel calls of  this
           function for independent rbfmodel objects are safe).
           If you want to perform parallel model evaluation  from multiple
           threads, use rbftscalcbuf() with per-thread buffer object.

This function returns 0.0 when:
* model is not initialized
* NX<>2
 *NY<>1

INPUT PARAMETERS:
    S       -   RBF model
    X0      -   first coordinate, finite number
    X1      -   second coordinate, finite number

RESULT:
    value of the model or 0.0 (as defined above)

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
double rbfcalc2(rbfmodel* s, double x0, double x1, ae_state *_state)
{
    double result;


    ae_assert(ae_isfinite(x0, _state), "RBFCalc2: invalid value for X0 (X0 is Inf)!", _state);
    ae_assert(ae_isfinite(x1, _state), "RBFCalc2: invalid value for X1 (X1 is Inf)!", _state);
    result = (double)(0);
    if( s->ny!=1||s->nx!=2 )
    {
        return result;
    }
    if( s->modelversion==1 )
    {
        result = rbfv1calc2(&s->model1, x0, x1, _state);
        return result;
    }
    if( s->modelversion==2 )
    {
        result = rbfv2calc2(&s->model2, x0, x1, _state);
        return result;
    }
    if( s->modelversion==3 )
    {
        result = rbfv3calc2(&s->model3, x0, x1, _state);
        return result;
    }
    ae_assert(ae_false, "RBFCalc2: integrity check failed", _state);
    return result;
}


/*************************************************************************
This function calculates values of the 3-dimensional RBF model with scalar
output (NY=1) at the given point.

IMPORTANT: THIS FUNCTION IS THREAD-UNSAFE. It uses fields of  rbfmodel  as
           temporary arrays, i.e. it is  impossible  to  perform  parallel
           evaluation on the same rbfmodel object (parallel calls of  this
           function for independent rbfmodel objects are safe).
           If you want to perform parallel model evaluation  from multiple
           threads, use rbftscalcbuf() with per-thread buffer object.

This function returns 0.0 when:
* model is not initialized
* NX<>3
 *NY<>1

INPUT PARAMETERS:
    S       -   RBF model
    X0      -   first coordinate, finite number
    X1      -   second coordinate, finite number
    X2      -   third coordinate, finite number

RESULT:
    value of the model or 0.0 (as defined above)

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
double rbfcalc3(rbfmodel* s,
     double x0,
     double x1,
     double x2,
     ae_state *_state)
{
    double result;


    ae_assert(ae_isfinite(x0, _state), "RBFCalc3: invalid value for X0 (X0 is Inf or NaN)!", _state);
    ae_assert(ae_isfinite(x1, _state), "RBFCalc3: invalid value for X1 (X1 is Inf or NaN)!", _state);
    ae_assert(ae_isfinite(x2, _state), "RBFCalc3: invalid value for X2 (X2 is Inf or NaN)!", _state);
    result = (double)(0);
    if( s->ny!=1||s->nx!=3 )
    {
        return result;
    }
    if( s->modelversion==1 )
    {
        result = rbfv1calc3(&s->model1, x0, x1, x2, _state);
        return result;
    }
    if( s->modelversion==2 )
    {
        result = rbfv2calc3(&s->model2, x0, x1, x2, _state);
        return result;
    }
    if( s->modelversion==3 )
    {
        result = rbfv3calc3(&s->model3, x0, x1, x2, _state);
        return result;
    }
    ae_assert(ae_false, "RBFCalc3: integrity check failed", _state);
    return result;
}


/*************************************************************************
This function calculates value and derivatives of  the  1-dimensional  RBF
model with scalar output (NY=1) at the given point.

IMPORTANT: THIS FUNCTION IS THREAD-UNSAFE. It uses fields of  rbfmodel  as
           temporary arrays, i.e. it is  impossible  to  perform  parallel
           evaluation on the same rbfmodel object (parallel calls of  this
           function for independent rbfmodel objects are safe).
           If you want to perform parallel model evaluation  from multiple
           threads, use rbftscalcbuf() with per-thread buffer object.

This function returns 0.0 in Y and/or DY in the following cases:
* the model is not initialized (Y=0, DY=0)
* NX<>1 or NY<>1 (Y=0, DY=0)
* the gradient is undefined at the trial point. Some basis  functions have
  discontinuous derivatives at the interpolation nodes:
  * biharmonic splines f=r have no Hessian and no gradient at the nodes
  In these cases only DY is set to zero (Y is still returned)

INPUT PARAMETERS:
    S       -   RBF model
    X0      -   first coordinate, finite number

OUTPUT PARAMETERS:
    Y       -   value of the model or 0.0 (as defined above)
    DY0     -   derivative with respect to X0

  -- ALGLIB --
     Copyright 13.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfdiff1(rbfmodel* s,
     double x0,
     double* y,
     double* dy0,
     ae_state *_state)
{

    *y = 0.0;
    *dy0 = 0.0;

    ae_assert(ae_isfinite(x0, _state), "RBFDiff1: invalid value for X0 (X0 is Inf or NaN)!", _state);
    *y = (double)(0);
    *dy0 = (double)(0);
    if( s->ny!=1||s->nx!=1 )
    {
        return;
    }
    rallocv(1, &s->calcbuf.x, _state);
    s->calcbuf.x.ptr.p_double[0] = x0;
    rbftsdiffbuf(s, &s->calcbuf, &s->calcbuf.x, &s->calcbuf.y, &s->calcbuf.dy, _state);
    *y = s->calcbuf.y.ptr.p_double[0];
    *dy0 = s->calcbuf.dy.ptr.p_double[0];
}


/*************************************************************************
This function calculates value and derivatives of  the  2-dimensional  RBF
model with scalar output (NY=1) at the given point.

IMPORTANT: THIS FUNCTION IS THREAD-UNSAFE. It uses fields of  rbfmodel  as
           temporary arrays, i.e. it is  impossible  to  perform  parallel
           evaluation on the same rbfmodel object (parallel calls of  this
           function for independent rbfmodel objects are safe).
           If you want to perform parallel model evaluation  from multiple
           threads, use rbftscalcbuf() with per-thread buffer object.

This function returns 0.0 in Y and/or DY in the following cases:
* the model is not initialized (Y=0, DY=0)
* NX<>2 or NY<>1 (Y=0, DY=0)
* the gradient is undefined at the trial point. Some basis  functions have
  discontinuous derivatives at the interpolation nodes:
  * biharmonic splines f=r have no Hessian and no gradient at the nodes
  In these cases only DY is set to zero (Y is still returned)

INPUT PARAMETERS:
    S       -   RBF model
    X0      -   first coordinate, finite number
    X1      -   second coordinate, finite number

OUTPUT PARAMETERS:
    Y       -   value of the model or 0.0 (as defined above)
    DY0     -   derivative with respect to X0
    DY1     -   derivative with respect to X1

  -- ALGLIB --
     Copyright 13.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfdiff2(rbfmodel* s,
     double x0,
     double x1,
     double* y,
     double* dy0,
     double* dy1,
     ae_state *_state)
{

    *y = 0.0;
    *dy0 = 0.0;
    *dy1 = 0.0;

    ae_assert(ae_isfinite(x0, _state), "RBFDiff2: invalid value for X0 (X0 is Inf or NaN)!", _state);
    ae_assert(ae_isfinite(x1, _state), "RBFDiff2: invalid value for X1 (X1 is Inf or NaN)!", _state);
    *y = (double)(0);
    *dy0 = (double)(0);
    *dy1 = (double)(0);
    if( s->ny!=1||s->nx!=2 )
    {
        return;
    }
    rallocv(2, &s->calcbuf.x, _state);
    s->calcbuf.x.ptr.p_double[0] = x0;
    s->calcbuf.x.ptr.p_double[1] = x1;
    rbftsdiffbuf(s, &s->calcbuf, &s->calcbuf.x, &s->calcbuf.y, &s->calcbuf.dy, _state);
    *y = s->calcbuf.y.ptr.p_double[0];
    *dy0 = s->calcbuf.dy.ptr.p_double[0];
    *dy1 = s->calcbuf.dy.ptr.p_double[1];
}


/*************************************************************************
This function calculates value and derivatives of  the  3-dimensional  RBF
model with scalar output (NY=1) at the given point.

IMPORTANT: THIS FUNCTION IS THREAD-UNSAFE. It uses fields of  rbfmodel  as
           temporary arrays, i.e. it is  impossible  to  perform  parallel
           evaluation on the same rbfmodel object (parallel calls of  this
           function for independent rbfmodel objects are safe).
           If you want to perform parallel model evaluation  from multiple
           threads, use rbftscalcbuf() with per-thread buffer object.

This function returns 0.0 in Y and/or DY in the following cases:
* the model is not initialized (Y=0, DY=0)
* NX<>3 or NY<>1 (Y=0, DY=0)
* the gradient is undefined at the trial point. Some basis  functions have
  discontinuous derivatives at the interpolation nodes:
  * biharmonic splines f=r have no Hessian and no gradient at the nodes
  In these cases only DY is set to zero (Y is still returned)

INPUT PARAMETERS:
    S       -   RBF model
    X0      -   first coordinate, finite number
    X1      -   second coordinate, finite number
    X2      -   third coordinate, finite number

OUTPUT PARAMETERS:
    Y       -   value of the model or 0.0 (as defined above)
    DY0     -   derivative with respect to X0
    DY1     -   derivative with respect to X1
    DY2     -   derivative with respect to X2

  -- ALGLIB --
     Copyright 13.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfdiff3(rbfmodel* s,
     double x0,
     double x1,
     double x2,
     double* y,
     double* dy0,
     double* dy1,
     double* dy2,
     ae_state *_state)
{

    *y = 0.0;
    *dy0 = 0.0;
    *dy1 = 0.0;
    *dy2 = 0.0;

    ae_assert(ae_isfinite(x0, _state), "RBFDiff3: invalid value for X0 (X0 is Inf or NaN)!", _state);
    ae_assert(ae_isfinite(x1, _state), "RBFDiff3: invalid value for X1 (X1 is Inf or NaN)!", _state);
    ae_assert(ae_isfinite(x2, _state), "RBFDiff3: invalid value for X2 (X2 is Inf or NaN)!", _state);
    *y = (double)(0);
    *dy0 = (double)(0);
    *dy1 = (double)(0);
    *dy2 = (double)(0);
    if( s->ny!=1||s->nx!=3 )
    {
        return;
    }
    rallocv(3, &s->calcbuf.x, _state);
    s->calcbuf.x.ptr.p_double[0] = x0;
    s->calcbuf.x.ptr.p_double[1] = x1;
    s->calcbuf.x.ptr.p_double[2] = x2;
    rbftsdiffbuf(s, &s->calcbuf, &s->calcbuf.x, &s->calcbuf.y, &s->calcbuf.dy, _state);
    *y = s->calcbuf.y.ptr.p_double[0];
    *dy0 = s->calcbuf.dy.ptr.p_double[0];
    *dy1 = s->calcbuf.dy.ptr.p_double[1];
    *dy2 = s->calcbuf.dy.ptr.p_double[2];
}


/*************************************************************************
This function sets absolute accuracy of a  fast evaluation  algorithm used
by rbffastcalc() and other fast evaluation functions.

A fast evaluation algorithm is model-dependent and is available  only  for
some RBF models. Usually it utilizes far field expansions (a generalization
of the fast multipoles  method).  If  no  approximate  fast  evaluator  is
available for the  current RBF model type, this function has no effect.

NOTE: this function can be called before or after the model was built. The
      result will be the same.

NOTE: this  function  has  O(N) running time, where N is a  points  count.
      Most fast evaluators work by aggregating influence of  point groups,
      i.e. by computing so called far field. Changing evaluator  tolerance
      means that far field radii have to  be  recomputed  for  each  point
      cluster, and we have O(N) such clusters.
      
      This function is still very fast, but  it  should  not be called too
      often, e.g. every time you call rbffastcalc() in a loop.

NOTE: the tolerance  set  by this function is an accuracy of an  evaluator
      which computes the value of the model. It is  NOT  accuracy  of  the
      model itself.
      
      E.g., if you set evaluation accuracy to 1E-12, the model value  will
      be computed with required precision. However, the model itself is an
      approximation of the target (the default requirement is to fit model
      with ~6 digits of precision) and THIS accuracy can  not  be  changed
      after the model was built.

IMPORTANT: THIS FUNCTION IS THREAD-UNSAFE. Calling it while another thread
           tries to use rbffastcalc() is unsafe because it means that  the
           accuracy requirements will change in the middle of computations.
           The algorithm may behave unpredictably.

INPUT PARAMETERS:
    S       -   RBF model
    TOL     -   TOL>0, desired evaluation tolerance:
                * should be somewhere between 1E-3 and 1E-6
                * values outside of this range will cause no problems (the
                  evaluator will do the job anyway). However,  too  strict
                  precision requirements may mean  that  no  approximation
                  speed-up will be achieved.

  -- ALGLIB --
     Copyright 19.09.2022 by Bochkanov Sergey
*************************************************************************/
void rbfsetfastevaltol(rbfmodel* s, double tol, ae_state *_state)
{


    ae_assert(ae_isfinite(tol, _state), "RBFSetFastEvalTol: TOL is not a finite number", _state);
    ae_assert(ae_fp_greater(tol,(double)(0)), "RBFSetFastEvalTol: TOL<=0", _state);
    s->fastevaltol = tol;
    pushfastevaltol(s, tol, _state);
}


/*************************************************************************
This function calculates values of the RBF model at the given point  using
a fast approximate algorithm whenever possible. If no  fast  algorithm  is
available for a given model type, traditional O(N) approach is used.

Presently, fast evaluation is implemented only for biharmonic splines.

The absolute approximation accuracy is controlled by the rbfsetfastevaltol()
function.

IMPORTANT: THIS FUNCTION IS THREAD-UNSAFE. It uses fields of  rbfmodel  as
           temporary arrays, i.e. it is  impossible  to  perform  parallel
           evaluation on the same rbfmodel object (parallel calls of  this
           function for independent rbfmodel objects are safe).
           If you want to perform parallel model evaluation  from multiple
           threads, use rbftscalcbuf() with a per-thread buffer object.

This function returns 0.0 when model is not initialized.

INPUT PARAMETERS:
    S       -   RBF model
    X       -   coordinates, array[NX].
                X may have more than NX elements, in this case only 
                leading NX will be used.

OUTPUT PARAMETERS:
    Y       -   function value, array[NY]. Y is out-parameter and 
                reallocated after call to this function. In case you  want
                to reuse previously allocated Y, you may use RBFCalcBuf(),
                which reallocates Y only when it is too small.

  -- ALGLIB --
     Copyright 19.09.2022 by Bochkanov Sergey
*************************************************************************/
void rbffastcalc(rbfmodel* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t i;

    ae_vector_clear(y);

    ae_assert(x->cnt>=s->nx, "RBFCalc: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFCalc: X contains infinite or NaN values", _state);
    if( y->cnt<s->ny )
    {
        ae_vector_set_length(y, s->ny, _state);
    }
    for(i=0; i<=s->ny-1; i++)
    {
        y->ptr.p_double[i] = (double)(0);
    }
    if( s->modelversion==1 )
    {
        rbfv1calcbuf(&s->model1, x, y, _state);
        return;
    }
    if( s->modelversion==2 )
    {
        rbfv2calcbuf(&s->model2, x, y, _state);
        return;
    }
    if( s->modelversion==3 )
    {
        rbfv3tsfastcalcbuf(&s->model3, &s->model3.calcbuf, x, y, _state);
        return;
    }
    ae_assert(ae_false, "RBFCalcBuf: integrity check failed", _state);
}


/*************************************************************************
This function calculates values of the RBF model at the given point.

This is general function which can be used for arbitrary NX (dimension  of 
the space of arguments) and NY (dimension of the function itself). However
when  you  have  NY=1  you  may  find more convenient to use rbfcalc2() or 
rbfcalc3().

IMPORTANT: THIS FUNCTION IS THREAD-UNSAFE. It uses fields of  rbfmodel  as
           temporary arrays, i.e. it is  impossible  to  perform  parallel
           evaluation on the same rbfmodel object (parallel calls of  this
           function for independent rbfmodel objects are safe).
           If you want to perform parallel model evaluation  from multiple
           threads, use rbftscalcbuf() with per-thread buffer object.

This function returns 0.0 when model is not initialized.

INPUT PARAMETERS:
    S       -   RBF model
    X       -   coordinates, array[NX].
                X may have more than NX elements, in this case only 
                leading NX will be used.

OUTPUT PARAMETERS:
    Y       -   function value, array[NY]. Y is out-parameter and 
                reallocated after call to this function. In case you  want
                to reuse previously allocated Y, you may use RBFCalcBuf(),
                which reallocates Y only when it is too small.

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
void rbfcalc(rbfmodel* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{

    ae_vector_clear(y);

    ae_assert(x->cnt>=s->nx, "RBFCalc: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFCalc: X contains infinite or NaN values", _state);
    rbfcalcbuf(s, x, y, _state);
}


/*************************************************************************
This function calculates values of the RBF model and  its  derivatives  at
the given point.

This is general function which can be used for arbitrary NX (dimension  of 
the space of arguments) and NY (dimension of the function itself). However
if you have NX=3 and NY=1, you may find more convenient to use rbfdiff3().

IMPORTANT: THIS FUNCTION IS THREAD-UNSAFE. It uses fields of  rbfmodel  as
           temporary arrays, i.e. it is  impossible  to  perform  parallel
           evaluation on the same rbfmodel object (parallel calls of  this
           function for independent rbfmodel objects are safe).
           
           If you want to perform parallel model evaluation  from multiple
           threads, use rbftsdiffbuf() with per-thread buffer object.

This function returns 0.0 in Y and/or DY in the following cases:
* the model is not initialized (Y=0, DY=0)
* the gradient is undefined at the trial point. Some basis  functions have
  discontinuous derivatives at the interpolation nodes:
  * biharmonic splines f=r have no Hessian and no gradient at the nodes
  In these cases only DY is set to zero (Y is still returned)

INPUT PARAMETERS:
    S       -   RBF model
    X       -   coordinates, array[NX].
                X may have more than NX elements, in this case only 
                leading NX will be used.

OUTPUT PARAMETERS:
    Y       -   function value, array[NY]. Y is out-parameter and 
                reallocated after call to this function. In case you  want
                to reuse previously allocated Y, you may use RBFDiffBuf(),
                which reallocates Y only when it is too small.
    DY      -   derivatives, array[NX*NY]:
                * Y[I*NX+J] with 0<=I<NY and 0<=J<NX  stores derivative of
                  function component I with respect to input J.
                * for NY=1 it is simply NX-dimensional gradient of the
                  scalar NX-dimensional function
                DY is out-parameter and reallocated  after  call  to  this
                function. In case you want to reuse  previously  allocated
                DY, you may use RBFDiffBuf(), which  reallocates  DY  only
                when it is too small to store the result.

  -- ALGLIB --
     Copyright 13.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfdiff(rbfmodel* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     /* Real    */ ae_vector* dy,
     ae_state *_state)
{

    ae_vector_clear(y);
    ae_vector_clear(dy);

    ae_assert(x->cnt>=s->nx, "RBFDiff: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFDiff: X contains infinite or NaN values", _state);
    rbfdiffbuf(s, x, y, dy, _state);
}


/*************************************************************************
This function calculates values of the RBF model and  its first and second
derivatives (Hessian matrix) at the given point.

This function supports both scalar (NY=1) and vector-valued (NY>1) RBFs.

IMPORTANT: THIS FUNCTION IS THREAD-UNSAFE. It uses fields of  rbfmodel  as
           temporary arrays, i.e. it is  impossible  to  perform  parallel
           evaluation on the same rbfmodel object (parallel calls of  this
           function for independent rbfmodel objects are safe).
           
           If you want to perform parallel model evaluation  from multiple
           threads, use rbftshessbuf() with per-thread buffer object.

This function returns 0 in Y and/or DY and/or D2Y in the following cases:
* the model is not initialized (Y=0, DY=0, D2Y=0)
* the gradient and/or Hessian is undefined at the trial point.  Some basis
  functions have discontinuous derivatives at the interpolation nodes:
  * thin plate splines have no Hessian at the nodes
  * biharmonic splines f=r have no Hessian and no gradient at the  nodes
  In these cases only corresponding derivative is set  to  zero,  and  the
  rest of the derivatives is still returned.

INPUT PARAMETERS:
    S       -   RBF model
    X       -   coordinates, array[NX].
                X may have more than NX elements, in this case only 
                leading NX will be used.

OUTPUT PARAMETERS:
    Y       -   function value, array[NY].
                Y is out-parameter and  reallocated  after  call  to  this
                function. In case you  want to reuse previously  allocated
                Y, you may use RBFHessBuf(), which reallocates Y only when
                it is too small.
    DY      -   first derivatives, array[NY*NX]:
                * Y[I*NX+J] with 0<=I<NY and 0<=J<NX  stores derivative of
                  function component I with respect to input J.
                * for NY=1 it is simply NX-dimensional gradient of the
                  scalar NX-dimensional function
                DY is out-parameter and reallocated  after  call  to  this
                function. In case you want to reuse  previously  allocated
                DY, you may use RBFHessBuf(), which  reallocates  DY  only
                when it is too small to store the result.
    D2Y     -   second derivatives, array[NY*NX*NX]:
                * for NY=1 it is NX*NX array that stores  Hessian  matrix,
                  with Y[I*NX+J]=Y[J*NX+I].
                * for  a  vector-valued  RBF  with  NY>1  it  contains  NY
                  subsequently stored Hessians: an element Y[K*NX*NX+I*NX+J]
                  with  0<=K<NY,  0<=I<NX  and  0<=J<NX    stores   second
                  derivative of the function #K  with  respect  to  inputs
                  #I and #J.
                D2Y is out-parameter and reallocated  after  call  to this
                function. In case you want to reuse  previously  allocated
                D2Y, you may use RBFHessBuf(), which  reallocates D2Y only
                when it is too small to store the result.

  -- ALGLIB --
     Copyright 13.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfhess(rbfmodel* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     /* Real    */ ae_vector* dy,
     /* Real    */ ae_vector* d2y,
     ae_state *_state)
{

    ae_vector_clear(y);
    ae_vector_clear(dy);
    ae_vector_clear(d2y);

    ae_assert(x->cnt>=s->nx, "RBFHess: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFHess: X contains infinite or NaN values", _state);
    rbftshessbuf(s, &s->calcbuf, x, y, dy, d2y, _state);
}


/*************************************************************************
This function calculates values of the RBF model at the given point.

Same as rbfcalc(), but does not reallocate Y when in is large enough to 
store function values.

IMPORTANT: THIS FUNCTION IS THREAD-UNSAFE. It uses fields of  rbfmodel  as
           temporary arrays, i.e. it is  impossible  to  perform  parallel
           evaluation on the same rbfmodel object (parallel calls of  this
           function for independent rbfmodel objects are safe).
           If you want to perform parallel model evaluation  from multiple
           threads, use rbftscalcbuf() with per-thread buffer object.

INPUT PARAMETERS:
    S       -   RBF model
    X       -   coordinates, array[NX].
                X may have more than NX elements, in this case only 
                leading NX will be used.
    Y       -   possibly preallocated array

OUTPUT PARAMETERS:
    Y       -   function value, array[NY]. Y is not reallocated when it
                is larger than NY.

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
void rbfcalcbuf(rbfmodel* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(x->cnt>=s->nx, "RBFCalcBuf: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFCalcBuf: X contains infinite or NaN values", _state);
    if( y->cnt<s->ny )
    {
        ae_vector_set_length(y, s->ny, _state);
    }
    for(i=0; i<=s->ny-1; i++)
    {
        y->ptr.p_double[i] = (double)(0);
    }
    if( s->modelversion==1 )
    {
        rbfv1calcbuf(&s->model1, x, y, _state);
        return;
    }
    if( s->modelversion==2 )
    {
        rbfv2calcbuf(&s->model2, x, y, _state);
        return;
    }
    if( s->modelversion==3 )
    {
        rbfv3calcbuf(&s->model3, x, y, _state);
        return;
    }
    ae_assert(ae_false, "RBFCalcBuf: integrity check failed", _state);
}


/*************************************************************************
This function calculates values of the RBF model and  its  derivatives  at
the given point. It is a buffered version of the RBFDiff() which tries  to
reuse possibly preallocated output arrays Y/DY as much as possible.

This is general function which can be used for arbitrary NX (dimension  of 
the space of arguments) and NY (dimension of the function itself). However
if you have NX=1, 2 or 3 and NY=1, you may find  more  convenient  to  use
rbfdiff1(), rbfdiff2() or rbfdiff3().

IMPORTANT: THIS FUNCTION IS THREAD-UNSAFE. It uses fields of  rbfmodel  as
           temporary arrays, i.e. it is  impossible  to  perform  parallel
           evaluation on the same rbfmodel object (parallel calls of  this
           function for independent rbfmodel objects are safe).
           
           If you want to perform parallel model evaluation  from multiple
           threads, use rbftsdiffbuf() with per-thread buffer object.

This function returns 0.0 in Y and/or DY in the following cases:
* the model is not initialized (Y=0, DY=0)
* the gradient is undefined at the trial point. Some basis  functions have
  discontinuous derivatives at the interpolation nodes:
  * biharmonic splines f=r have no Hessian and no gradient at the nodes
  In these cases only DY is set to zero (Y is still returned)

INPUT PARAMETERS:
    S       -   RBF model
    X       -   coordinates, array[NX].
                X may have more than NX elements, in this case only 
                leading NX will be used.
    Y, DY   -   possibly preallocated arrays; if array size is large enough
                to store results, this function does not  reallocate  array
                to fit output size exactly.

OUTPUT PARAMETERS:
    Y       -   function value, array[NY].
    DY      -   derivatives, array[NX*NY]:
                * Y[I*NX+J] with 0<=I<NY and 0<=J<NX  stores derivative of
                  function component I with respect to input J.
                * for NY=1 it is simply NX-dimensional gradient of the
                  scalar NX-dimensional function

  -- ALGLIB --
     Copyright 13.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfdiffbuf(rbfmodel* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     /* Real    */ ae_vector* dy,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(x->cnt>=s->nx, "RBFDiffBuf: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFDiffBuf: X contains infinite or NaN values", _state);
    ae_assert(s->modelversion==s->calcbuf.modelversion, "RBF: integrity check 3945 failed", _state);
    if( y->cnt<s->ny )
    {
        ae_vector_set_length(y, s->ny, _state);
    }
    if( dy->cnt<s->ny*s->nx )
    {
        ae_vector_set_length(dy, s->ny*s->nx, _state);
    }
    for(i=0; i<=s->ny-1; i++)
    {
        y->ptr.p_double[i] = (double)(0);
    }
    for(i=0; i<=s->ny*s->nx-1; i++)
    {
        dy->ptr.p_double[i] = (double)(0);
    }
    if( s->modelversion==1 )
    {
        rbfv1tsdiffbuf(&s->model1, &s->calcbuf.bufv1, x, y, dy, _state);
        return;
    }
    if( s->modelversion==2 )
    {
        rbfv2tsdiffbuf(&s->model2, &s->calcbuf.bufv2, x, y, dy, _state);
        return;
    }
    if( s->modelversion==3 )
    {
        rbfv3tsdiffbuf(&s->model3, &s->calcbuf.bufv3, x, y, dy, _state);
        return;
    }
    ae_assert(ae_false, "RBFDiffBuf: integrity check failed", _state);
}


/*************************************************************************
This function calculates values of the RBF model and  its first and second
derivatives (Hessian matrix) at the given point. It is a buffered  version
that reuses memory  allocated  in  output  buffers  Y/DY/D2Y  as  much  as
possible.

This function supports both scalar (NY=1) and vector-valued (NY>1) RBFs.

IMPORTANT: THIS FUNCTION IS THREAD-UNSAFE. It uses fields of  rbfmodel  as
           temporary arrays, i.e. it is  impossible  to  perform  parallel
           evaluation on the same rbfmodel object (parallel calls of  this
           function for independent rbfmodel objects are safe).
           
           If you want to perform parallel model evaluation  from multiple
           threads, use rbftshessbuf() with per-thread buffer object.

This function returns 0 in Y and/or DY and/or D2Y in the following cases:
* the model is not initialized (Y=0, DY=0, D2Y=0)
* the gradient and/or Hessian is undefined at the trial point.  Some basis
  functions have discontinuous derivatives at the interpolation nodes:
  * thin plate splines have no Hessian at the nodes
  * biharmonic splines f=r have no Hessian and no gradient at the  nodes
  In these cases only corresponding derivative is set  to  zero,  and  the
  rest of the derivatives is still returned.

INPUT PARAMETERS:
    S       -   RBF model
    X       -   coordinates, array[NX].
                X may have more than NX elements, in this case only 
                leading NX will be used.
    Y,DY,D2Y-   possible preallocated output arrays. If these  arrays  are
                smaller than  required  to  store  the  result,  they  are
                automatically reallocated. If array is large enough, it is
                not resized.

OUTPUT PARAMETERS:
    Y       -   function value, array[NY].
    DY      -   first derivatives, array[NY*NX]:
                * Y[I*NX+J] with 0<=I<NY and 0<=J<NX  stores derivative of
                  function component I with respect to input J.
                * for NY=1 it is simply NX-dimensional gradient of the
                  scalar NX-dimensional function
    D2Y     -   second derivatives, array[NY*NX*NX]:
                * for NY=1 it is NX*NX array that stores  Hessian  matrix,
                  with Y[I*NX+J]=Y[J*NX+I].
                * for  a  vector-valued  RBF  with  NY>1  it  contains  NY
                  subsequently stored Hessians: an element Y[K*NX*NX+I*NX+J]
                  with  0<=K<NY,  0<=I<NX  and  0<=J<NX    stores   second
                  derivative of the function #K  with  respect  to  inputs
                  #I and #J.

  -- ALGLIB --
     Copyright 13.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfhessbuf(rbfmodel* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     /* Real    */ ae_vector* dy,
     /* Real    */ ae_vector* d2y,
     ae_state *_state)
{


    ae_assert(x->cnt>=s->nx, "RBFHess: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFHess: X contains infinite or NaN values", _state);
    rbftshessbuf(s, &s->calcbuf, x, y, dy, d2y, _state);
}


/*************************************************************************
This function calculates values of the RBF model at the given point, using
external  buffer  object  (internal  temporaries  of  RBF  model  are  not
modified).

This function allows to use same RBF model object  in  different  threads,
assuming  that  different   threads  use  different  instances  of  buffer
structure.

INPUT PARAMETERS:
    S       -   RBF model, may be shared between different threads
    Buf     -   buffer object created for this particular instance of  RBF
                model with rbfcreatecalcbuffer().
    X       -   coordinates, array[NX].
                X may have more than NX elements, in this case only 
                leading NX will be used.
    Y       -   possibly preallocated array

OUTPUT PARAMETERS:
    Y       -   function value, array[NY]. Y is not reallocated when it
                is larger than NY.

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
void rbftscalcbuf(const rbfmodel* s,
     rbfcalcbuffer* buf,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(x->cnt>=s->nx, "RBFCalcBuf: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFCalcBuf: X contains infinite or NaN values", _state);
    ae_assert(s->modelversion==buf->modelversion, "RBFCalcBuf: buffer object is not compatible with RBF model", _state);
    if( y->cnt<s->ny )
    {
        ae_vector_set_length(y, s->ny, _state);
    }
    for(i=0; i<=s->ny-1; i++)
    {
        y->ptr.p_double[i] = (double)(0);
    }
    if( s->modelversion==1 )
    {
        rbfv1tscalcbuf(&s->model1, &buf->bufv1, x, y, _state);
        return;
    }
    if( s->modelversion==2 )
    {
        rbfv2tscalcbuf(&s->model2, &buf->bufv2, x, y, _state);
        return;
    }
    if( s->modelversion==3 )
    {
        rbfv3tscalcbuf(&s->model3, &buf->bufv3, x, y, _state);
        return;
    }
    ae_assert(ae_false, "RBFTsCalcBuf: integrity check failed", _state);
}


/*************************************************************************
This function calculates values of the RBF model and  its  derivatives  at
the given point, using external buffer object (internal temporaries of the
RBF model are not modified).

This function allows to use same RBF model object  in  different  threads,
assuming  that  different   threads  use different instances of the buffer
structure.

This function returns 0.0 in Y and/or DY in the following cases:
* the model is not initialized (Y=0, DY=0)
* the gradient is undefined at the trial point. Some basis  functions have
  discontinuous derivatives at the interpolation nodes:
  * biharmonic splines f=r have no Hessian and no gradient at the nodes
  In these cases only DY is set to zero (Y is still returned)

INPUT PARAMETERS:
    S       -   RBF model, may be shared between different threads
    Buf     -   buffer object created for this particular instance of  RBF
                model with rbfcreatecalcbuffer().
    X       -   coordinates, array[NX].
                X may have more than NX elements, in this case only 
                leading NX will be used.
    Y, DY   -   possibly preallocated arrays; if array size is large enough
                to store results, this function does not  reallocate  array
                to fit output size exactly.

OUTPUT PARAMETERS:
    Y       -   function value, array[NY].
    DY      -   derivatives, array[NX*NY]:
                * Y[I*NX+J] with 0<=I<NY and 0<=J<NX  stores derivative of
                  function component I with respect to input J.
                * for NY=1 it is simply NX-dimensional gradient of the
                  scalar NX-dimensional function
                Zero is returned when the first derivative is undefined.

  -- ALGLIB --
     Copyright 13.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbftsdiffbuf(const rbfmodel* s,
     rbfcalcbuffer* buf,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     /* Real    */ ae_vector* dy,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(x->cnt>=s->nx, "RBFTsDiffBuf: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFTsDiffBuf: X contains infinite or NaN values", _state);
    ae_assert(s->modelversion==buf->modelversion, "RBFTsDiffBuf: integrity check 3985 failed", _state);
    if( y->cnt<s->ny )
    {
        ae_vector_set_length(y, s->ny, _state);
    }
    if( dy->cnt<s->ny*s->nx )
    {
        ae_vector_set_length(dy, s->ny*s->nx, _state);
    }
    for(i=0; i<=s->ny-1; i++)
    {
        y->ptr.p_double[i] = (double)(0);
    }
    for(i=0; i<=s->ny*s->nx-1; i++)
    {
        dy->ptr.p_double[i] = (double)(0);
    }
    if( s->modelversion==1 )
    {
        rbfv1tsdiffbuf(&s->model1, &buf->bufv1, x, y, dy, _state);
        return;
    }
    if( s->modelversion==2 )
    {
        rbfv2tsdiffbuf(&s->model2, &buf->bufv2, x, y, dy, _state);
        return;
    }
    if( s->modelversion==3 )
    {
        rbfv3tsdiffbuf(&s->model3, &buf->bufv3, x, y, dy, _state);
        return;
    }
    ae_assert(ae_false, "RBFDiffBuf: integrity check failed", _state);
}


/*************************************************************************
This function calculates values of the RBF model and  its first and second
derivatives (Hessian matrix) at the given  point,  using  external  buffer
object (internal temporaries of the RBF  model  are  not  modified).

This function allows to use same RBF model object  in  different  threads,
assuming  that  different   threads  use different instances of the buffer
structure.

This function returns 0 in Y and/or DY and/or D2Y in the following cases:
* the model is not initialized (Y=0, DY=0, D2Y=0)
* the gradient and/or Hessian is undefined at the trial point.  Some basis
  functions have discontinuous derivatives at the interpolation nodes:
  * thin plate splines have no Hessian at the nodes
  * biharmonic splines f=r have no Hessian and no gradient at the  nodes
  In these cases only corresponding derivative is set  to  zero,  and  the
  rest of the derivatives is still returned.

INPUT PARAMETERS:
    S       -   RBF model, may be shared between different threads
    Buf     -   buffer object created for this particular instance of  RBF
                model with rbfcreatecalcbuffer().
    X       -   coordinates, array[NX].
                X may have more than NX elements, in this case only 
                leading NX will be used.
    Y,DY,D2Y-   possible preallocated output arrays. If these  arrays  are
                smaller than  required  to  store  the  result,  they  are
                automatically reallocated. If array is large enough, it is
                not resized.

OUTPUT PARAMETERS:
    Y       -   function value, array[NY].
    DY      -   first derivatives, array[NY*NX]:
                * Y[I*NX+J] with 0<=I<NY and 0<=J<NX  stores derivative of
                  function component I with respect to input J.
                * for NY=1 it is simply NX-dimensional gradient of the
                  scalar NX-dimensional function
                Zero is returned when the first derivative is undefined.
    D2Y     -   second derivatives, array[NY*NX*NX]:
                * for NY=1 it is NX*NX array that stores  Hessian  matrix,
                  with Y[I*NX+J]=Y[J*NX+I].
                * for  a  vector-valued  RBF  with  NY>1  it  contains  NY
                  subsequently stored Hessians: an element Y[K*NX*NX+I*NX+J]
                  with  0<=K<NY,  0<=I<NX  and  0<=J<NX    stores   second
                  derivative of the function #K  with  respect  to  inputs
                  #I and #J.
                Zero is returned when the second derivative is undefined.

  -- ALGLIB --
     Copyright 13.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbftshessbuf(const rbfmodel* s,
     rbfcalcbuffer* buf,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     /* Real    */ ae_vector* dy,
     /* Real    */ ae_vector* d2y,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(x->cnt>=s->nx, "RBFTsHessBuf: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFTsHessBuf: X contains infinite or NaN values", _state);
    ae_assert(s->modelversion==buf->modelversion, "RBFTsHessBuf: integrity check 3953 failed", _state);
    if( y->cnt<s->ny )
    {
        ae_vector_set_length(y, s->ny, _state);
    }
    if( dy->cnt<s->ny*s->nx )
    {
        ae_vector_set_length(dy, s->ny*s->nx, _state);
    }
    if( d2y->cnt<s->ny*s->nx*s->nx )
    {
        ae_vector_set_length(d2y, s->ny*s->nx*s->nx, _state);
    }
    for(i=0; i<=s->ny-1; i++)
    {
        y->ptr.p_double[i] = (double)(0);
    }
    for(i=0; i<=s->ny*s->nx-1; i++)
    {
        dy->ptr.p_double[i] = (double)(0);
    }
    for(i=0; i<=s->ny*s->nx*s->nx-1; i++)
    {
        d2y->ptr.p_double[i] = (double)(0);
    }
    if( s->modelversion==1 )
    {
        rbfv1tshessbuf(&s->model1, &buf->bufv1, x, y, dy, d2y, _state);
        return;
    }
    if( s->modelversion==2 )
    {
        rbfv2tshessbuf(&s->model2, &buf->bufv2, x, y, dy, d2y, _state);
        return;
    }
    if( s->modelversion==3 )
    {
        rbfv3tshessbuf(&s->model3, &buf->bufv3, x, y, dy, d2y, _state);
        return;
    }
    ae_assert(ae_false, "RBFDiffBuf: integrity check failed", _state);
}


/*************************************************************************
This is legacy function for gridded calculation of RBF model.

It is superseded by rbfgridcalc2v() and  rbfgridcalc2vsubset()  functions.

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
void rbfgridcalc2(rbfmodel* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n0,
     /* Real    */ const ae_vector* x1,
     ae_int_t n1,
     /* Real    */ ae_matrix* y,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_vector yy;

    ae_frame_make(_state, &_frame_block);
    memset(&yy, 0, sizeof(yy));
    ae_matrix_clear(y);
    ae_vector_init(&yy, 0, DT_REAL, _state, ae_true);

    ae_assert(n0>0, "RBFGridCalc2: invalid value for N0 (N0<=0)!", _state);
    ae_assert(n1>0, "RBFGridCalc2: invalid value for N1 (N1<=0)!", _state);
    ae_assert(x0->cnt>=n0, "RBFGridCalc2: Length(X0)<N0", _state);
    ae_assert(x1->cnt>=n1, "RBFGridCalc2: Length(X1)<N1", _state);
    ae_assert(isfinitevector(x0, n0, _state), "RBFGridCalc2: X0 contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(x1, n1, _state), "RBFGridCalc2: X1 contains infinite or NaN values!", _state);
    if( s->modelversion==1 )
    {
        rbfv1gridcalc2(&s->model1, x0, n0, x1, n1, y, _state);
        ae_frame_leave(_state);
        return;
    }
    if( s->modelversion==2 )
    {
        rbfv2gridcalc2(&s->model2, x0, n0, x1, n1, y, _state);
        ae_frame_leave(_state);
        return;
    }
    if( s->modelversion==3 )
    {
        rallocm(n0, n1, y, _state);
        if( s->nx!=2||s->ny!=1 )
        {
            rsetm(n0, n1, 0.0, y, _state);
            ae_frame_leave(_state);
            return;
        }
        rbfgridcalc2v(s, x0, n0, x1, n1, &yy, _state);
        for(i=0; i<=n0-1; i++)
        {
            for(j=0; j<=n1-1; j++)
            {
                y->ptr.pp_double[i][j] = yy.ptr.p_double[i+j*n0];
            }
        }
        ae_frame_leave(_state);
        return;
    }
    ae_assert(ae_false, "RBFGridCalc2: integrity check failed", _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function calculates values of the RBF  model  at  the  regular  grid,
which  has  N0*N1 points, with Point[I,J] = (X0[I], X1[J]).  Vector-valued
RBF models are supported.

This function returns 0.0 when:
* model is not initialized
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

NOTE: Parallel  processing  is  implemented only for modern (hierarchical)
      RBFs. Legacy version 1 RBFs (created  by  QNN  or  RBF-ML) are still
      processed serially.

INPUT PARAMETERS:
    S       -   RBF model, used in read-only mode, can be  shared  between
                multiple   invocations  of  this  function  from  multiple
                threads.
    
    X0      -   array of grid nodes, first coordinates, array[N0].
                Must be ordered by ascending. Exception is generated
                if the array is not correctly ordered.
    N0      -   grid size (number of nodes) in the first dimension
    
    X1      -   array of grid nodes, second coordinates, array[N1]
                Must be ordered by ascending. Exception is generated
                if the array is not correctly ordered.
    N1      -   grid size (number of nodes) in the second dimension

OUTPUT PARAMETERS:
    Y       -   function values, array[NY*N0*N1], where NY is a  number of
                "output" vector values (this  function   supports  vector-
                valued RBF models). Y is out-variable and  is  reallocated
                by this function.
                Y[K+NY*(I0+I1*N0)]=F_k(X0[I0],X1[I1]), for:
                *  K=0...NY-1
                * I0=0...N0-1
                * I1=0...N1-1

NOTE: this function supports weakly ordered grid nodes, i.e. you may  have                
      X[i]=X[i+1] for some i. It does  not  provide  you  any  performance
      benefits  due  to   duplication  of  points,  just  convenience  and
      flexibility.
      
NOTE: this  function  is  re-entrant,  i.e.  you  may  use  same  rbfmodel
      structure in multiple threads calling  this function  for  different
      grids.
      
NOTE: if you need function values on some subset  of  regular  grid, which
      may be described as "several compact and  dense  islands",  you  may
      use rbfgridcalc2vsubset().

  -- ALGLIB --
     Copyright 27.01.2017 by Bochkanov Sergey
*************************************************************************/
void rbfgridcalc2v(const rbfmodel* s,
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

    ae_assert(n0>0, "RBFGridCalc2V: invalid value for N0 (N0<=0)!", _state);
    ae_assert(n1>0, "RBFGridCalc2V: invalid value for N1 (N1<=0)!", _state);
    ae_assert(x0->cnt>=n0, "RBFGridCalc2V: Length(X0)<N0", _state);
    ae_assert(x1->cnt>=n1, "RBFGridCalc2V: Length(X1)<N1", _state);
    ae_assert(isfinitevector(x0, n0, _state), "RBFGridCalc2V: X0 contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(x1, n1, _state), "RBFGridCalc2V: X1 contains infinite or NaN values!", _state);
    for(i=0; i<=n0-2; i++)
    {
        ae_assert(ae_fp_less_eq(x0->ptr.p_double[i],x0->ptr.p_double[i+1]), "RBFGridCalc2V: X0 is not ordered by ascending", _state);
    }
    for(i=0; i<=n1-2; i++)
    {
        ae_assert(ae_fp_less_eq(x1->ptr.p_double[i],x1->ptr.p_double[i+1]), "RBFGridCalc2V: X1 is not ordered by ascending", _state);
    }
    rbfgridcalc2vx(s, x0, n0, x1, n1, &dummy, ae_false, y, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function calculates values of the RBF model at some subset of regular
grid:
* grid has N0*N1 points, with Point[I,J] = (X0[I], X1[J])
* only values at some subset of this grid are required
Vector-valued RBF models are supported.

This function returns 0.0 when:
* model is not initialized
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

NOTE: Parallel  processing  is  implemented only for modern (hierarchical)
      RBFs. Legacy version 1 RBFs (created  by  QNN  or  RBF-ML) are still
      processed serially.

INPUT PARAMETERS:
    S       -   RBF model, used in read-only mode, can be  shared  between
                multiple   invocations  of  this  function  from  multiple
                threads.
    
    X0      -   array of grid nodes, first coordinates, array[N0].
                Must be ordered by ascending. Exception is generated
                if the array is not correctly ordered.
    N0      -   grid size (number of nodes) in the first dimension
    
    X1      -   array of grid nodes, second coordinates, array[N1]
                Must be ordered by ascending. Exception is generated
                if the array is not correctly ordered.
    N1      -   grid size (number of nodes) in the second dimension
    
    FlagY   -   array[N0*N1]:
                * Y[I0+I1*N0] corresponds to node (X0[I0],X1[I1])
                * it is a "bitmap" array which contains  False  for  nodes
                  which are NOT calculated, and True for nodes  which  are
                  required.

OUTPUT PARAMETERS:
    Y       -   function values, array[NY*N0*N1*N2], where NY is a  number
                of "output" vector values (this function  supports vector-
                valued RBF models):
                * Y[K+NY*(I0+I1*N0)]=F_k(X0[I0],X1[I1]),
                  for K=0...NY-1, I0=0...N0-1, I1=0...N1-1.
                * elements of Y[] which correspond  to  FlagY[]=True   are
                  loaded by model values (which may be  exactly  zero  for
                  some nodes).
                * elements of Y[] which correspond to FlagY[]=False MAY be
                  initialized by zeros OR may be calculated. This function
                  processes  grid  as  a  hierarchy  of  nested blocks and
                  micro-rows. If just one element of micro-row is required,
                  entire micro-row (up to 8 nodes in the current  version,
                  but no promises) is calculated.

NOTE: this function supports weakly ordered grid nodes, i.e. you may  have                
      X[i]=X[i+1] for some i. It does  not  provide  you  any  performance
      benefits  due  to   duplication  of  points,  just  convenience  and
      flexibility.
      
NOTE: this  function  is  re-entrant,  i.e.  you  may  use  same  rbfmodel
      structure in multiple threads calling  this function  for  different
      grids.

  -- ALGLIB --
     Copyright 04.03.2016 by Bochkanov Sergey
*************************************************************************/
void rbfgridcalc2vsubset(const rbfmodel* s,
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

    ae_assert(n0>0, "RBFGridCalc2VSubset: invalid value for N0 (N0<=0)!", _state);
    ae_assert(n1>0, "RBFGridCalc2VSubset: invalid value for N1 (N1<=0)!", _state);
    ae_assert(x0->cnt>=n0, "RBFGridCalc2VSubset: Length(X0)<N0", _state);
    ae_assert(x1->cnt>=n1, "RBFGridCalc2VSubset: Length(X1)<N1", _state);
    ae_assert(flagy->cnt>=n0*n1, "RBFGridCalc2VSubset: Length(FlagY)<N0*N1*N2", _state);
    ae_assert(isfinitevector(x0, n0, _state), "RBFGridCalc2VSubset: X0 contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(x1, n1, _state), "RBFGridCalc2VSubset: X1 contains infinite or NaN values!", _state);
    for(i=0; i<=n0-2; i++)
    {
        ae_assert(ae_fp_less_eq(x0->ptr.p_double[i],x0->ptr.p_double[i+1]), "RBFGridCalc2VSubset: X0 is not ordered by ascending", _state);
    }
    for(i=0; i<=n1-2; i++)
    {
        ae_assert(ae_fp_less_eq(x1->ptr.p_double[i],x1->ptr.p_double[i+1]), "RBFGridCalc2VSubset: X1 is not ordered by ascending", _state);
    }
    rbfgridcalc2vx(s, x0, n0, x1, n1, flagy, ae_true, y, _state);
}


/*************************************************************************
This function calculates values of the RBF  model  at  the  regular  grid,
which  has  N0*N1*N2  points,  with  Point[I,J,K] = (X0[I], X1[J], X2[K]).
Vector-valued RBF models are supported.

This function returns 0.0 when:
* model is not initialized
* NX<>3

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

NOTE: Parallel  processing  is  implemented only for modern (hierarchical)
      RBFs. Legacy version 1 RBFs (created  by  QNN  or  RBF-ML) are still
      processed serially.

INPUT PARAMETERS:
    S       -   RBF model, used in read-only mode, can be  shared  between
                multiple   invocations  of  this  function  from  multiple
                threads.
    
    X0      -   array of grid nodes, first coordinates, array[N0].
                Must be ordered by ascending. Exception is generated
                if the array is not correctly ordered.
    N0      -   grid size (number of nodes) in the first dimension
    
    X1      -   array of grid nodes, second coordinates, array[N1]
                Must be ordered by ascending. Exception is generated
                if the array is not correctly ordered.
    N1      -   grid size (number of nodes) in the second dimension
    
    X2      -   array of grid nodes, third coordinates, array[N2]
                Must be ordered by ascending. Exception is generated
                if the array is not correctly ordered.
    N2      -   grid size (number of nodes) in the third dimension

OUTPUT PARAMETERS:
    Y       -   function values, array[NY*N0*N1*N2], where NY is a  number
                of "output" vector values (this function  supports vector-
                valued RBF models). Y is out-variable and  is  reallocated
                by this function.
                Y[K+NY*(I0+I1*N0+I2*N0*N1)]=F_k(X0[I0],X1[I1],X2[I2]), for:
                *  K=0...NY-1
                * I0=0...N0-1
                * I1=0...N1-1
                * I2=0...N2-1

NOTE: this function supports weakly ordered grid nodes, i.e. you may  have                
      X[i]=X[i+1] for some i. It does  not  provide  you  any  performance
      benefits  due  to   duplication  of  points,  just  convenience  and
      flexibility.
      
NOTE: this  function  is  re-entrant,  i.e.  you  may  use  same  rbfmodel
      structure in multiple threads calling  this function  for  different
      grids.
      
NOTE: if you need function values on some subset  of  regular  grid, which
      may be described as "several compact and  dense  islands",  you  may
      use rbfgridcalc3vsubset().

  -- ALGLIB --
     Copyright 04.03.2016 by Bochkanov Sergey
*************************************************************************/
void rbfgridcalc3v(const rbfmodel* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n0,
     /* Real    */ const ae_vector* x1,
     ae_int_t n1,
     /* Real    */ const ae_vector* x2,
     ae_int_t n2,
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

    ae_assert(n0>0, "RBFGridCalc3V: invalid value for N0 (N0<=0)!", _state);
    ae_assert(n1>0, "RBFGridCalc3V: invalid value for N1 (N1<=0)!", _state);
    ae_assert(n2>0, "RBFGridCalc3V: invalid value for N2 (N2<=0)!", _state);
    ae_assert(x0->cnt>=n0, "RBFGridCalc3V: Length(X0)<N0", _state);
    ae_assert(x1->cnt>=n1, "RBFGridCalc3V: Length(X1)<N1", _state);
    ae_assert(x2->cnt>=n2, "RBFGridCalc3V: Length(X2)<N2", _state);
    ae_assert(isfinitevector(x0, n0, _state), "RBFGridCalc3V: X0 contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(x1, n1, _state), "RBFGridCalc3V: X1 contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(x2, n2, _state), "RBFGridCalc3V: X2 contains infinite or NaN values!", _state);
    for(i=0; i<=n0-2; i++)
    {
        ae_assert(ae_fp_less_eq(x0->ptr.p_double[i],x0->ptr.p_double[i+1]), "RBFGridCalc3V: X0 is not ordered by ascending", _state);
    }
    for(i=0; i<=n1-2; i++)
    {
        ae_assert(ae_fp_less_eq(x1->ptr.p_double[i],x1->ptr.p_double[i+1]), "RBFGridCalc3V: X1 is not ordered by ascending", _state);
    }
    for(i=0; i<=n2-2; i++)
    {
        ae_assert(ae_fp_less_eq(x2->ptr.p_double[i],x2->ptr.p_double[i+1]), "RBFGridCalc3V: X2 is not ordered by ascending", _state);
    }
    rbfgridcalc3vx(s, x0, n0, x1, n1, x2, n2, &dummy, ae_false, y, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function calculates values of the RBF model at some subset of regular
grid:
* grid has N0*N1*N2 points, with Point[I,J,K] = (X0[I], X1[J], X2[K])
* only values at some subset of this grid are required
Vector-valued RBF models are supported.

This function returns 0.0 when:
* model is not initialized
* NX<>3

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

NOTE: Parallel  processing  is  implemented only for modern (hierarchical)
      RBFs. Legacy version 1 RBFs (created  by  QNN  or  RBF-ML) are still
      processed serially.

INPUT PARAMETERS:
    S       -   RBF model, used in read-only mode, can be  shared  between
                multiple   invocations  of  this  function  from  multiple
                threads.
    
    X0      -   array of grid nodes, first coordinates, array[N0].
                Must be ordered by ascending. Exception is generated
                if the array is not correctly ordered.
    N0      -   grid size (number of nodes) in the first dimension
    
    X1      -   array of grid nodes, second coordinates, array[N1]
                Must be ordered by ascending. Exception is generated
                if the array is not correctly ordered.
    N1      -   grid size (number of nodes) in the second dimension
    
    X2      -   array of grid nodes, third coordinates, array[N2]
                Must be ordered by ascending. Exception is generated
                if the array is not correctly ordered.
    N2      -   grid size (number of nodes) in the third dimension
    
    FlagY   -   array[N0*N1*N2]:
                * Y[I0+I1*N0+I2*N0*N1] corresponds to node (X0[I0],X1[I1],X2[I2])
                * it is a "bitmap" array which contains  False  for  nodes
                  which are NOT calculated, and True for nodes  which  are
                  required.

OUTPUT PARAMETERS:
    Y       -   function values, array[NY*N0*N1*N2], where NY is a  number
                of "output" vector values (this function  supports vector-
                valued RBF models):
                * Y[K+NY*(I0+I1*N0+I2*N0*N1)]=F_k(X0[I0],X1[I1],X2[I2]),
                  for K=0...NY-1, I0=0...N0-1, I1=0...N1-1, I2=0...N2-1.
                * elements of Y[] which correspond  to  FlagY[]=True   are
                  loaded by model values (which may be  exactly  zero  for
                  some nodes).
                * elements of Y[] which correspond to FlagY[]=False MAY be
                  initialized by zeros OR may be calculated. This function
                  processes  grid  as  a  hierarchy  of  nested blocks and
                  micro-rows. If just one element of micro-row is required,
                  entire micro-row (up to 8 nodes in the current  version,
                  but no promises) is calculated.

NOTE: this function supports weakly ordered grid nodes, i.e. you may  have                
      X[i]=X[i+1] for some i. It does  not  provide  you  any  performance
      benefits  due  to   duplication  of  points,  just  convenience  and
      flexibility.
      
NOTE: this  function  is  re-entrant,  i.e.  you  may  use  same  rbfmodel
      structure in multiple threads calling  this function  for  different
      grids.

  -- ALGLIB --
     Copyright 04.03.2016 by Bochkanov Sergey
*************************************************************************/
void rbfgridcalc3vsubset(const rbfmodel* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n0,
     /* Real    */ const ae_vector* x1,
     ae_int_t n1,
     /* Real    */ const ae_vector* x2,
     ae_int_t n2,
     /* Boolean */ const ae_vector* flagy,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t i;

    ae_vector_clear(y);

    ae_assert(n0>0, "RBFGridCalc3VSubset: invalid value for N0 (N0<=0)!", _state);
    ae_assert(n1>0, "RBFGridCalc3VSubset: invalid value for N1 (N1<=0)!", _state);
    ae_assert(n2>0, "RBFGridCalc3VSubset: invalid value for N2 (N2<=0)!", _state);
    ae_assert(x0->cnt>=n0, "RBFGridCalc3VSubset: Length(X0)<N0", _state);
    ae_assert(x1->cnt>=n1, "RBFGridCalc3VSubset: Length(X1)<N1", _state);
    ae_assert(x2->cnt>=n2, "RBFGridCalc3VSubset: Length(X2)<N2", _state);
    ae_assert(flagy->cnt>=n0*n1*n2, "RBFGridCalc3VSubset: Length(FlagY)<N0*N1*N2", _state);
    ae_assert(isfinitevector(x0, n0, _state), "RBFGridCalc3VSubset: X0 contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(x1, n1, _state), "RBFGridCalc3VSubset: X1 contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(x2, n2, _state), "RBFGridCalc3VSubset: X2 contains infinite or NaN values!", _state);
    for(i=0; i<=n0-2; i++)
    {
        ae_assert(ae_fp_less_eq(x0->ptr.p_double[i],x0->ptr.p_double[i+1]), "RBFGridCalc3VSubset: X0 is not ordered by ascending", _state);
    }
    for(i=0; i<=n1-2; i++)
    {
        ae_assert(ae_fp_less_eq(x1->ptr.p_double[i],x1->ptr.p_double[i+1]), "RBFGridCalc3VSubset: X1 is not ordered by ascending", _state);
    }
    for(i=0; i<=n2-2; i++)
    {
        ae_assert(ae_fp_less_eq(x2->ptr.p_double[i],x2->ptr.p_double[i+1]), "RBFGridCalc3VSubset: X2 is not ordered by ascending", _state);
    }
    rbfgridcalc3vx(s, x0, n0, x1, n1, x2, n2, flagy, ae_true, y, _state);
}


/*************************************************************************
This function, depending on SparseY, acts as RBFGridCalc2V (SparseY=False)
or RBFGridCalc2VSubset (SparseY=True) function.  See  comments  for  these
functions for more information

  -- ALGLIB --
     Copyright 04.03.2016 by Bochkanov Sergey
*************************************************************************/
void rbfgridcalc2vx(const rbfmodel* s,
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
    ae_int_t ylen;
    hqrndstate rs;
    ae_vector dummyx2;
    ae_vector dummyx3;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t l;
    ae_vector tx;
    ae_vector ty;
    ae_int_t dstoffs;
    rbfcalcbuffer calcbuf;

    ae_frame_make(_state, &_frame_block);
    memset(&rs, 0, sizeof(rs));
    memset(&dummyx2, 0, sizeof(dummyx2));
    memset(&dummyx3, 0, sizeof(dummyx3));
    memset(&tx, 0, sizeof(tx));
    memset(&ty, 0, sizeof(ty));
    memset(&calcbuf, 0, sizeof(calcbuf));
    _hqrndstate_init(&rs, _state, ae_true);
    ae_vector_init(&dummyx2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&dummyx3, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tx, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ty, 0, DT_REAL, _state, ae_true);
    _rbfcalcbuffer_init(&calcbuf, _state, ae_true);

    ae_assert(n0>0, "RBFGridCalc2VX: invalid value for N0 (N0<=0)!", _state);
    ae_assert(n1>0, "RBFGridCalc2VX: invalid value for N1 (N1<=0)!", _state);
    ae_assert(x0->cnt>=n0, "RBFGridCalc2VX: Length(X0)<N0", _state);
    ae_assert(x1->cnt>=n1, "RBFGridCalc2VX: Length(X1)<N1", _state);
    ae_assert(isfinitevector(x0, n0, _state), "RBFGridCalc2VX: X0 contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(x1, n1, _state), "RBFGridCalc2VX: X1 contains infinite or NaN values!", _state);
    for(i=0; i<=n0-2; i++)
    {
        ae_assert(ae_fp_less_eq(x0->ptr.p_double[i],x0->ptr.p_double[i+1]), "RBFGridCalc2VX: X0 is not ordered by ascending", _state);
    }
    for(i=0; i<=n1-2; i++)
    {
        ae_assert(ae_fp_less_eq(x1->ptr.p_double[i],x1->ptr.p_double[i+1]), "RBFGridCalc2VX: X1 is not ordered by ascending", _state);
    }
    
    /*
     * Prepare local variables
     */
    nx = s->nx;
    ny = s->ny;
    hqrndseed(325, 46345, &rs, _state);
    
    /*
     * Prepare output array
     */
    ylen = ny*n0*n1;
    ae_vector_set_length(y, ylen, _state);
    for(i=0; i<=ylen-1; i++)
    {
        y->ptr.p_double[i] = (double)(0);
    }
    if( s->nx!=2 )
    {
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Reference code for V3 models
     */
    if( s->modelversion==3 )
    {
        ae_vector_set_length(&dummyx2, 1, _state);
        dummyx2.ptr.p_double[0] = (double)(0);
        ae_vector_set_length(&dummyx3, 1, _state);
        dummyx3.ptr.p_double[0] = (double)(0);
        rbfv3gridcalcvx(&s->model3, x0, n0, x1, n1, &dummyx2, 1, &dummyx3, 1, flagy, sparsey, y, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Process V2 model
     */
    if( s->modelversion==2 )
    {
        ae_vector_set_length(&dummyx2, 1, _state);
        dummyx2.ptr.p_double[0] = (double)(0);
        ae_vector_set_length(&dummyx3, 1, _state);
        dummyx3.ptr.p_double[0] = (double)(0);
        rbfv2gridcalcvx(&s->model2, x0, n0, x1, n1, &dummyx2, 1, &dummyx3, 1, flagy, sparsey, y, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Reference code for V1 models
     */
    if( s->modelversion==1 )
    {
        ae_vector_set_length(&tx, nx, _state);
        rbfcreatecalcbuffer(s, &calcbuf, _state);
        for(i=0; i<=n0-1; i++)
        {
            for(j=0; j<=n1-1; j++)
            {
                k = i+j*n0;
                dstoffs = ny*k;
                if( sparsey&&!flagy->ptr.p_bool[k] )
                {
                    for(l=0; l<=ny-1; l++)
                    {
                        y->ptr.p_double[l+dstoffs] = (double)(0);
                    }
                    continue;
                }
                tx.ptr.p_double[0] = x0->ptr.p_double[i];
                tx.ptr.p_double[1] = x1->ptr.p_double[j];
                rbftscalcbuf(s, &calcbuf, &tx, &ty, _state);
                for(l=0; l<=ny-1; l++)
                {
                    y->ptr.p_double[l+dstoffs] = ty.ptr.p_double[l];
                }
            }
        }
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Unknown model
     */
    ae_assert(ae_false, "RBFGridCalc2VX: integrity check failed", _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function, depending on SparseY, acts as RBFGridCalc3V (SparseY=False)
or RBFGridCalc3VSubset (SparseY=True) function.  See  comments  for  these
functions for more information

  -- ALGLIB --
     Copyright 04.03.2016 by Bochkanov Sergey
*************************************************************************/
void rbfgridcalc3vx(const rbfmodel* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n0,
     /* Real    */ const ae_vector* x1,
     ae_int_t n1,
     /* Real    */ const ae_vector* x2,
     ae_int_t n2,
     /* Boolean */ const ae_vector* flagy,
     ae_bool sparsey,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t ylen;
    ae_int_t nx;
    ae_int_t ny;
    double rmax;
    ae_vector blocks0;
    ae_vector blocks1;
    ae_vector blocks2;
    ae_int_t blockscnt0;
    ae_int_t blockscnt1;
    ae_int_t blockscnt2;
    double blockwidth;
    double searchradius;
    double avgfuncpernode;
    ae_int_t ntrials;
    ae_int_t maxblocksize;
    gridcalc3v1buf bufseedv1;
    ae_shared_pool bufpool;
    hqrndstate rs;
    ae_vector dummyx3;

    ae_frame_make(_state, &_frame_block);
    memset(&blocks0, 0, sizeof(blocks0));
    memset(&blocks1, 0, sizeof(blocks1));
    memset(&blocks2, 0, sizeof(blocks2));
    memset(&bufseedv1, 0, sizeof(bufseedv1));
    memset(&bufpool, 0, sizeof(bufpool));
    memset(&rs, 0, sizeof(rs));
    memset(&dummyx3, 0, sizeof(dummyx3));
    ae_vector_init(&blocks0, 0, DT_INT, _state, ae_true);
    ae_vector_init(&blocks1, 0, DT_INT, _state, ae_true);
    ae_vector_init(&blocks2, 0, DT_INT, _state, ae_true);
    _gridcalc3v1buf_init(&bufseedv1, _state, ae_true);
    ae_shared_pool_init(&bufpool, _state, ae_true);
    _hqrndstate_init(&rs, _state, ae_true);
    ae_vector_init(&dummyx3, 0, DT_REAL, _state, ae_true);

    ae_assert(n0>0, "RBFGridCalc3V: invalid value for N0 (N0<=0)!", _state);
    ae_assert(n1>0, "RBFGridCalc3V: invalid value for N1 (N1<=0)!", _state);
    ae_assert(n2>0, "RBFGridCalc3V: invalid value for N2 (N2<=0)!", _state);
    ae_assert(x0->cnt>=n0, "RBFGridCalc3V: Length(X0)<N0", _state);
    ae_assert(x1->cnt>=n1, "RBFGridCalc3V: Length(X1)<N1", _state);
    ae_assert(x2->cnt>=n2, "RBFGridCalc3V: Length(X2)<N2", _state);
    ae_assert(isfinitevector(x0, n0, _state), "RBFGridCalc3V: X0 contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(x1, n1, _state), "RBFGridCalc3V: X1 contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(x2, n2, _state), "RBFGridCalc3V: X2 contains infinite or NaN values!", _state);
    for(i=0; i<=n0-2; i++)
    {
        ae_assert(ae_fp_less_eq(x0->ptr.p_double[i],x0->ptr.p_double[i+1]), "RBFGridCalc3V: X0 is not ordered by ascending", _state);
    }
    for(i=0; i<=n1-2; i++)
    {
        ae_assert(ae_fp_less_eq(x1->ptr.p_double[i],x1->ptr.p_double[i+1]), "RBFGridCalc3V: X1 is not ordered by ascending", _state);
    }
    for(i=0; i<=n2-2; i++)
    {
        ae_assert(ae_fp_less_eq(x2->ptr.p_double[i],x2->ptr.p_double[i+1]), "RBFGridCalc3V: X2 is not ordered by ascending", _state);
    }
    
    /*
     * Prepare local variables
     */
    nx = s->nx;
    ny = s->ny;
    hqrndseed(325, 46345, &rs, _state);
    
    /*
     * Prepare output array
     */
    ylen = ny*n0*n1*n2;
    ae_vector_set_length(y, ylen, _state);
    for(i=0; i<=ylen-1; i++)
    {
        y->ptr.p_double[i] = (double)(0);
    }
    if( s->nx!=3 )
    {
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Process V1 model
     */
    if( s->modelversion==1 )
    {
        
        /*
         * Fast exit for models without centers
         */
        if( s->model1.nc==0 )
        {
            ae_frame_leave(_state);
            return;
        }
        
        /*
         * Prepare seed, create shared pool of temporary buffers
         */
        ae_vector_set_length(&bufseedv1.cx, nx, _state);
        ae_vector_set_length(&bufseedv1.tx, nx, _state);
        ae_vector_set_length(&bufseedv1.ty, ny, _state);
        ae_vector_set_length(&bufseedv1.expbuf0, n0, _state);
        ae_vector_set_length(&bufseedv1.expbuf1, n1, _state);
        ae_vector_set_length(&bufseedv1.expbuf2, n2, _state);
        kdtreecreaterequestbuffer(&s->model1.tree, &bufseedv1.requestbuf, _state);
        ae_shared_pool_set_seed(&bufpool, &bufseedv1, (ae_int_t)sizeof(bufseedv1), (ae_constructor)_gridcalc3v1buf_init, (ae_copy_constructor)_gridcalc3v1buf_init_copy, (ae_destructor)_gridcalc3v1buf_destroy, _state);
        
        /*
         * Analyze input grid:
         * * analyze average number of basis functions per grid node
         * * partition grid in into blocks
         */
        rmax = s->model1.rmax;
        blockwidth = (double)2*rmax;
        maxblocksize = 8;
        searchradius = rmax*rbf_rbffarradius+0.5*ae_sqrt((double)(s->nx), _state)*blockwidth;
        ntrials = 100;
        avgfuncpernode = 0.0;
        for(i=0; i<=ntrials-1; i++)
        {
            bufseedv1.tx.ptr.p_double[0] = x0->ptr.p_double[hqrnduniformi(&rs, n0, _state)];
            bufseedv1.tx.ptr.p_double[1] = x1->ptr.p_double[hqrnduniformi(&rs, n1, _state)];
            bufseedv1.tx.ptr.p_double[2] = x2->ptr.p_double[hqrnduniformi(&rs, n2, _state)];
            avgfuncpernode = avgfuncpernode+(double)kdtreetsqueryrnn(&s->model1.tree, &bufseedv1.requestbuf, &bufseedv1.tx, searchradius, ae_true, _state)/(double)ntrials;
        }
        ae_vector_set_length(&blocks0, n0+1, _state);
        blockscnt0 = 0;
        blocks0.ptr.p_int[0] = 0;
        for(i=1; i<=n0-1; i++)
        {
            if( ae_fp_greater(x0->ptr.p_double[i]-x0->ptr.p_double[blocks0.ptr.p_int[blockscnt0]],blockwidth)||i-blocks0.ptr.p_int[blockscnt0]>=maxblocksize )
            {
                inc(&blockscnt0, _state);
                blocks0.ptr.p_int[blockscnt0] = i;
            }
        }
        inc(&blockscnt0, _state);
        blocks0.ptr.p_int[blockscnt0] = n0;
        ae_vector_set_length(&blocks1, n1+1, _state);
        blockscnt1 = 0;
        blocks1.ptr.p_int[0] = 0;
        for(i=1; i<=n1-1; i++)
        {
            if( ae_fp_greater(x1->ptr.p_double[i]-x1->ptr.p_double[blocks1.ptr.p_int[blockscnt1]],blockwidth)||i-blocks1.ptr.p_int[blockscnt1]>=maxblocksize )
            {
                inc(&blockscnt1, _state);
                blocks1.ptr.p_int[blockscnt1] = i;
            }
        }
        inc(&blockscnt1, _state);
        blocks1.ptr.p_int[blockscnt1] = n1;
        ae_vector_set_length(&blocks2, n2+1, _state);
        blockscnt2 = 0;
        blocks2.ptr.p_int[0] = 0;
        for(i=1; i<=n2-1; i++)
        {
            if( ae_fp_greater(x2->ptr.p_double[i]-x2->ptr.p_double[blocks2.ptr.p_int[blockscnt2]],blockwidth)||i-blocks2.ptr.p_int[blockscnt2]>=maxblocksize )
            {
                inc(&blockscnt2, _state);
                blocks2.ptr.p_int[blockscnt2] = i;
            }
        }
        inc(&blockscnt2, _state);
        blocks2.ptr.p_int[blockscnt2] = n2;
        
        /*
         * Perform calculation in multithreaded mode
         */
        rbfv1gridcalc3vrec(&s->model1, x0, n0, x1, n1, x2, n2, &blocks0, 0, blockscnt0, &blocks1, 0, blockscnt1, &blocks2, 0, blockscnt2, flagy, sparsey, searchradius, avgfuncpernode, &bufpool, y, _state);
        
        /*
         * Done
         */
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Process V2 model
     */
    if( s->modelversion==2 )
    {
        ae_vector_set_length(&dummyx3, 1, _state);
        dummyx3.ptr.p_double[0] = (double)(0);
        rbfv2gridcalcvx(&s->model2, x0, n0, x1, n1, x2, n2, &dummyx3, 1, flagy, sparsey, y, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Process V3 model
     */
    if( s->modelversion==3 )
    {
        ae_vector_set_length(&dummyx3, 1, _state);
        dummyx3.ptr.p_double[0] = (double)(0);
        rbfv3gridcalcvx(&s->model3, x0, n0, x1, n1, x2, n2, &dummyx3, 1, flagy, sparsey, y, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Unknown model
     */
    ae_assert(ae_false, "RBFGridCalc3VX: integrity check failed", _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function "unpacks" RBF model by extracting its coefficients.

INPUT PARAMETERS:
    S       -   RBF model

OUTPUT PARAMETERS:
    NX      -   dimensionality of argument
    NY      -   dimensionality of the target function
    XWR     -   model  information ,  2D  array.  One  row  of  the  array
                corresponds to one basis function.
                
                For ModelVersion=1 we have NX+NY+1 columns:
                * first NX columns  - coordinates of the center 
                * next  NY columns  - weights, one per dimension of the 
                                      function being modeled
                * last column       - radius, same for all dimensions of
                                      the function being modeled
                
                For ModelVersion=2 we have NX+NY+NX columns:
                * first NX columns  - coordinates of the center 
                * next  NY columns  - weights, one per dimension of the 
                                      function being modeled
                * last NX columns   - radii, one per dimension
                
                For ModelVersion=3 we have NX+NY+NX+3 columns:
                * first NX columns  - coordinates of the center 
                * next  NY columns  - weights, one per dimension of the 
                                      function being modeled
                * next NX columns   - radii, one per dimension
                * next column       - basis function type:
                                      * 1  for f=r
                                      * 2  for f=r^2*ln(r)
                                      * 10 for multiquadric f=sqrt(r^2+alpha^2)
                * next column       - basis function parameter:
                                      * alpha, for basis function type 10
                                      * ignored (zero) for other basis function types
                * next column       - point index in the original dataset,
                                      or -1 for an artificial node created
                                      by the solver. The algorithm may reorder
                                      the nodes, drop some nodes or add
                                      artificial nodes. Thus, one parsing
                                      this column should expect all these
                                      kinds of alterations in the dataset.
                
    NC      -   number of the centers
    V       -   polynomial  term , array[NY,NX+1]. One row per one 
                dimension of the function being modelled. First NX 
                elements are linear coefficients, V[NX] is equal to the 
                constant part.
    ModelVersion-version of the RBF model:
                * 1 - for models created by QNN and RBF-ML algorithms,
                  compatible with ALGLIB 3.10 or earlier.
                * 2 - for models created by HierarchicalRBF, requires
                  ALGLIB 3.11 or later
                * 3 - for models created by DDM-RBF, requires
                  ALGLIB 3.19 or later

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
void rbfunpack(rbfmodel* s,
     ae_int_t* nx,
     ae_int_t* ny,
     /* Real    */ ae_matrix* xwr,
     ae_int_t* nc,
     /* Real    */ ae_matrix* v,
     ae_int_t* modelversion,
     ae_state *_state)
{

    *nx = 0;
    *ny = 0;
    ae_matrix_clear(xwr);
    *nc = 0;
    ae_matrix_clear(v);
    *modelversion = 0;

    if( s->modelversion==1 )
    {
        *modelversion = 1;
        rbfv1unpack(&s->model1, nx, ny, xwr, nc, v, _state);
        return;
    }
    if( s->modelversion==2 )
    {
        *modelversion = 2;
        rbfv2unpack(&s->model2, nx, ny, xwr, nc, v, _state);
        return;
    }
    if( s->modelversion==3 )
    {
        *modelversion = 3;
        rbfv3unpack(&s->model3, nx, ny, xwr, nc, v, _state);
        return;
    }
    ae_assert(ae_false, "RBFUnpack: integrity check failure", _state);
}


/*************************************************************************
This function returns model version.

INPUT PARAMETERS:
    S       -   RBF model

RESULT:
    * 1 - for models created by QNN and RBF-ML algorithms,
      compatible with ALGLIB 3.10 or earlier.
    * 2 - for models created by HierarchicalRBF, requires
      ALGLIB 3.11 or later

  -- ALGLIB --
     Copyright 06.07.2016 by Bochkanov Sergey
*************************************************************************/
ae_int_t rbfgetmodelversion(rbfmodel* s, ae_state *_state)
{
    ae_int_t result;


    result = s->modelversion;
    return result;
}


/*************************************************************************
This function is used to peek into hierarchical RBF  construction  process
from  some  other  thread  and  get current progress indicator. It returns
value in [0,1].

IMPORTANT: only HRBFs (hierarchical RBFs) support  peeking  into  progress
           indicator. Legacy RBF-ML and RBF-QNN do  not  support  it.  You
           will always get 0 value.

INPUT PARAMETERS:
    S           -   RBF model object

RESULT:
    progress value, in [0,1]

  -- ALGLIB --
     Copyright 17.11.2018 by Bochkanov Sergey
*************************************************************************/
double rbfpeekprogress(const rbfmodel* s, ae_state *_state)
{
    double result;


    result = (double)s->progress10000/(double)10000;
    return result;
}


/*************************************************************************
This function  is  used  to  submit  a  request  for  termination  of  the
hierarchical RBF construction process from some other thread.  As  result,
RBF construction is terminated smoothly (with proper deallocation  of  all
necessary resources) and resultant model is filled by zeros.

A rep.terminationtype=8 will be returned upon receiving such request.

IMPORTANT: only  HRBFs  (hierarchical  RBFs) support termination requests.
           Legacy RBF-ML and RBF-QNN do not  support  it.  An  attempt  to
           terminate their construction will be ignored.

IMPORTANT: termination request flag is cleared when the model construction
           starts. Thus, any pre-construction termination requests will be
           silently ignored - only ones submitted AFTER  construction  has
           actually began will be handled.

INPUT PARAMETERS:
    S           -   RBF model object

  -- ALGLIB --
     Copyright 17.11.2018 by Bochkanov Sergey
*************************************************************************/
void rbfrequesttermination(rbfmodel* s, ae_state *_state)
{


    s->terminationrequest = ae_true;
}


/*************************************************************************
This function sets RBF profile to standard or debug

INPUT PARAMETERS:
    S           -   RBF model object
    P           -   profile type:
                    * 0 for standard
                    * -1 for debug
                    * -2 for debug with artificially worsened numerical
                      precision. This profile is designed to test algorithm
                      ability to deal with difficult problems,

  -- ALGLIB --
     Copyright 17.11.2018 by Bochkanov Sergey
*************************************************************************/
void rbfsetprofile(rbfmodel* s, ae_int_t p, ae_state *_state)
{


    ae_assert((p==-2||p==-1)||p==0, "RBFSetProfile: incorrect P", _state);
    s->rbfprofile = p;
}


/*************************************************************************
This function changes evaluation tolerance of a fast evaluator (if present).
It is an actual implementation that is used by RBFSetFastEvalTol().

It usually has O(N) running time because evaluator has to be rebuilt according
to the new tolerance.

INPUT PARAMETERS:
    S           -   RBF model object
    TOL         -   desired tolerance

  -- ALGLIB --
     Copyright 19.09.2022 by Bochkanov Sergey
*************************************************************************/
void pushfastevaltol(rbfmodel* s, double tol, ae_state *_state)
{


    if( s->modelversion!=3 )
    {
        return;
    }
    rbf3pushfastevaltol(&s->model3, tol, _state);
}


/*************************************************************************
Serializer: allocation

  -- ALGLIB --
     Copyright 02.02.2012 by Bochkanov Sergey
*************************************************************************/
void rbfalloc(ae_serializer* s, const rbfmodel* model, ae_state *_state)
{


    
    /*
     * Header
     */
    ae_serializer_alloc_entry(s);
    
    /*
     * V1 model
     */
    if( model->modelversion==1 )
    {
        
        /*
         * Header
         */
        ae_serializer_alloc_entry(s);
        rbfv1alloc(s, &model->model1, _state);
        return;
    }
    
    /*
     * V2 model
     */
    if( model->modelversion==2 )
    {
        
        /*
         * Header
         */
        ae_serializer_alloc_entry(s);
        rbfv2alloc(s, &model->model2, _state);
        return;
    }
    
    /*
     * V3 model
     */
    if( model->modelversion==3 )
    {
        
        /*
         * Header
         */
        ae_serializer_alloc_entry(s);
        rbfv3alloc(s, &model->model3, _state);
        return;
    }
    ae_assert(ae_false, "Assertion failed", _state);
}


/*************************************************************************
Serializer: serialization

  -- ALGLIB --
     Copyright 02.02.2012 by Bochkanov Sergey
*************************************************************************/
void rbfserialize(ae_serializer* s,
     const rbfmodel* model,
     ae_state *_state)
{


    
    /*
     * Header
     */
    ae_serializer_serialize_int(s, getrbfserializationcode(_state), _state);
    
    /*
     * V1 model
     */
    if( model->modelversion==1 )
    {
        ae_serializer_serialize_int(s, rbf_rbffirstversion, _state);
        rbfv1serialize(s, &model->model1, _state);
        return;
    }
    
    /*
     * V2 model
     */
    if( model->modelversion==2 )
    {
        
        /*
         * Header
         */
        ae_serializer_serialize_int(s, rbf_rbfversion2, _state);
        rbfv2serialize(s, &model->model2, _state);
        return;
    }
    
    /*
     * V3 model
     */
    if( model->modelversion==3 )
    {
        
        /*
         * Header
         */
        ae_serializer_serialize_int(s, rbf_rbfversion3, _state);
        rbfv3serialize(s, &model->model3, _state);
        return;
    }
    ae_assert(ae_false, "Assertion failed", _state);
}


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 02.02.2012 by Bochkanov Sergey
*************************************************************************/
void rbfunserialize(ae_serializer* s, rbfmodel* model, ae_state *_state)
{
    ae_int_t i0;
    ae_int_t i1;

    _rbfmodel_clear(model);

    rbf_rbfpreparenonserializablefields(model, _state);
    
    /*
     * Header
     */
    ae_serializer_unserialize_int(s, &i0, _state);
    ae_assert(i0==getrbfserializationcode(_state), "RBFUnserialize: stream header corrupted", _state);
    ae_serializer_unserialize_int(s, &i1, _state);
    ae_assert((i1==rbf_rbffirstversion||i1==rbf_rbfversion2)||i1==rbf_rbfversion3, "RBFUnserialize: stream header corrupted", _state);
    
    /*
     * V1 model
     */
    if( i1==rbf_rbffirstversion )
    {
        rbfv1unserialize(s, &model->model1, _state);
        model->modelversion = 1;
        model->ny = model->model1.ny;
        model->nx = model->model1.nx;
        rbf_initializev2(model->nx, model->ny, &model->model2, _state);
        rbf_initializev3(model->nx, model->ny, &model->model3, _state);
        rbfcreatecalcbuffer(model, &model->calcbuf, _state);
        pushfastevaltol(model, model->fastevaltol, _state);
        return;
    }
    
    /*
     * V2 model
     */
    if( i1==rbf_rbfversion2 )
    {
        rbfv2unserialize(s, &model->model2, _state);
        model->modelversion = 2;
        model->ny = model->model2.ny;
        model->nx = model->model2.nx;
        rbf_initializev1(model->nx, model->ny, &model->model1, _state);
        rbf_initializev3(model->nx, model->ny, &model->model3, _state);
        rbfcreatecalcbuffer(model, &model->calcbuf, _state);
        pushfastevaltol(model, model->fastevaltol, _state);
        return;
    }
    
    /*
     * V3 model
     */
    if( i1==rbf_rbfversion3 )
    {
        rbfv3unserialize(s, &model->model3, _state);
        model->modelversion = 3;
        model->ny = model->model3.ny;
        model->nx = model->model3.nx;
        rbf_initializev1(model->nx, model->ny, &model->model1, _state);
        rbf_initializev2(model->nx, model->ny, &model->model2, _state);
        rbfcreatecalcbuffer(model, &model->calcbuf, _state);
        pushfastevaltol(model, model->fastevaltol, _state);
        return;
    }
    ae_assert(ae_false, "RBF: unserialiation error (unexpected model type)", _state);
}


/*************************************************************************
Initialize empty model

  -- ALGLIB --
     Copyright 12.05.2016 by Bochkanov Sergey
*************************************************************************/
static void rbf_rbfpreparenonserializablefields(rbfmodel* s,
     ae_state *_state)
{


    s->n = 0;
    s->hasscale = ae_false;
    s->radvalue = (double)(1);
    s->radzvalue = (double)(5);
    s->nlayers = 0;
    s->lambdav = (double)(0);
    s->aterm = 1;
    s->algorithmtype = 0;
    s->rbfprofile = 0;
    s->epsort = rbf_eps;
    s->epserr = rbf_eps;
    s->maxits = 0;
    s->v3tol = 1.0E-6;
    s->nnmaxits = 100;
    s->fastevaltol = 1.0E-3;
}


/*************************************************************************
Initialize V1 model (skip initialization for NX=1 or NX>3)

  -- ALGLIB --
     Copyright 12.05.2016 by Bochkanov Sergey
*************************************************************************/
static void rbf_initializev1(ae_int_t nx,
     ae_int_t ny,
     rbfv1model* s,
     ae_state *_state)
{

    _rbfv1model_clear(s);

    if( nx==2||nx==3 )
    {
        rbfv1create(nx, ny, s, _state);
    }
}


/*************************************************************************
Initialize V2 model

  -- ALGLIB --
     Copyright 12.05.2016 by Bochkanov Sergey
*************************************************************************/
static void rbf_initializev2(ae_int_t nx,
     ae_int_t ny,
     rbfv2model* s,
     ae_state *_state)
{

    _rbfv2model_clear(s);

    rbfv2create(nx, ny, s, _state);
}


/*************************************************************************
Initialize V3 model

  -- ALGLIB --
     Copyright 12.05.2016 by Bochkanov Sergey
*************************************************************************/
static void rbf_initializev3(ae_int_t nx,
     ae_int_t ny,
     rbfv3model* s,
     ae_state *_state)
{

    _rbfv3model_clear(s);

    rbfv3create(nx, ny, 2, (double)(0), s, _state);
}


/*************************************************************************
Cleans report fields

  -- ALGLIB --
     Copyright 16.06.2016 by Bochkanov Sergey
*************************************************************************/
static void rbf_clearreportfields(rbfreport* rep, ae_state *_state)
{


    rep->rmserror = _state->v_nan;
    rep->maxerror = _state->v_nan;
    rep->arows = 0;
    rep->acols = 0;
    rep->annz = 0;
    rep->iterationscount = 0;
    rep->nmv = 0;
    rep->terminationtype = 0;
}


void _rbfcalcbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    rbfcalcbuffer *p = (rbfcalcbuffer*)_p;
    ae_touch_ptr((void*)p);
    _rbfv1calcbuffer_init(&p->bufv1, _state, make_automatic);
    _rbfv2calcbuffer_init(&p->bufv2, _state, make_automatic);
    _rbfv3calcbuffer_init(&p->bufv3, _state, make_automatic);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->y, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dy, 0, DT_REAL, _state, make_automatic);
}


void _rbfcalcbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    rbfcalcbuffer       *dst = (rbfcalcbuffer*)_dst;
    const rbfcalcbuffer *src = (const rbfcalcbuffer*)_src;
    dst->modelversion = src->modelversion;
    _rbfv1calcbuffer_init_copy(&dst->bufv1, &src->bufv1, _state, make_automatic);
    _rbfv2calcbuffer_init_copy(&dst->bufv2, &src->bufv2, _state, make_automatic);
    _rbfv3calcbuffer_init_copy(&dst->bufv3, &src->bufv3, _state, make_automatic);
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->y, &src->y, _state, make_automatic);
    ae_vector_init_copy(&dst->dy, &src->dy, _state, make_automatic);
}


void _rbfcalcbuffer_clear(void* _p)
{
    rbfcalcbuffer *p = (rbfcalcbuffer*)_p;
    ae_touch_ptr((void*)p);
    _rbfv1calcbuffer_clear(&p->bufv1);
    _rbfv2calcbuffer_clear(&p->bufv2);
    _rbfv3calcbuffer_clear(&p->bufv3);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->y);
    ae_vector_clear(&p->dy);
}


void _rbfcalcbuffer_destroy(void* _p)
{
    rbfcalcbuffer *p = (rbfcalcbuffer*)_p;
    ae_touch_ptr((void*)p);
    _rbfv1calcbuffer_destroy(&p->bufv1);
    _rbfv2calcbuffer_destroy(&p->bufv2);
    _rbfv3calcbuffer_destroy(&p->bufv3);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->y);
    ae_vector_destroy(&p->dy);
}


void _rbfmodel_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    rbfmodel *p = (rbfmodel*)_p;
    ae_touch_ptr((void*)p);
    _rbfv1model_init(&p->model1, _state, make_automatic);
    _rbfv2model_init(&p->model2, _state, make_automatic);
    _rbfv3model_init(&p->model3, _state, make_automatic);
    _rbfcalcbuffer_init(&p->calcbuf, _state, make_automatic);
    ae_matrix_init(&p->x, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->y, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
}


void _rbfmodel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    rbfmodel       *dst = (rbfmodel*)_dst;
    const rbfmodel *src = (const rbfmodel*)_src;
    dst->nx = src->nx;
    dst->ny = src->ny;
    dst->modelversion = src->modelversion;
    _rbfv1model_init_copy(&dst->model1, &src->model1, _state, make_automatic);
    _rbfv2model_init_copy(&dst->model2, &src->model2, _state, make_automatic);
    _rbfv3model_init_copy(&dst->model3, &src->model3, _state, make_automatic);
    _rbfcalcbuffer_init_copy(&dst->calcbuf, &src->calcbuf, _state, make_automatic);
    dst->lambdav = src->lambdav;
    dst->radvalue = src->radvalue;
    dst->radzvalue = src->radzvalue;
    dst->nlayers = src->nlayers;
    dst->aterm = src->aterm;
    dst->algorithmtype = src->algorithmtype;
    dst->rbfprofile = src->rbfprofile;
    dst->bftype = src->bftype;
    dst->bfparam = src->bfparam;
    dst->epsort = src->epsort;
    dst->epserr = src->epserr;
    dst->maxits = src->maxits;
    dst->v3tol = src->v3tol;
    dst->nnmaxits = src->nnmaxits;
    dst->n = src->n;
    ae_matrix_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_matrix_init_copy(&dst->y, &src->y, _state, make_automatic);
    dst->hasscale = src->hasscale;
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    dst->fastevaltol = src->fastevaltol;
    dst->progress10000 = src->progress10000;
    dst->terminationrequest = src->terminationrequest;
}


void _rbfmodel_clear(void* _p)
{
    rbfmodel *p = (rbfmodel*)_p;
    ae_touch_ptr((void*)p);
    _rbfv1model_clear(&p->model1);
    _rbfv2model_clear(&p->model2);
    _rbfv3model_clear(&p->model3);
    _rbfcalcbuffer_clear(&p->calcbuf);
    ae_matrix_clear(&p->x);
    ae_matrix_clear(&p->y);
    ae_vector_clear(&p->s);
}


void _rbfmodel_destroy(void* _p)
{
    rbfmodel *p = (rbfmodel*)_p;
    ae_touch_ptr((void*)p);
    _rbfv1model_destroy(&p->model1);
    _rbfv2model_destroy(&p->model2);
    _rbfv3model_destroy(&p->model3);
    _rbfcalcbuffer_destroy(&p->calcbuf);
    ae_matrix_destroy(&p->x);
    ae_matrix_destroy(&p->y);
    ae_vector_destroy(&p->s);
}


void _rbfreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    rbfreport *p = (rbfreport*)_p;
    ae_touch_ptr((void*)p);
}


void _rbfreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    rbfreport       *dst = (rbfreport*)_dst;
    const rbfreport *src = (const rbfreport*)_src;
    dst->rmserror = src->rmserror;
    dst->maxerror = src->maxerror;
    dst->arows = src->arows;
    dst->acols = src->acols;
    dst->annz = src->annz;
    dst->iterationscount = src->iterationscount;
    dst->nmv = src->nmv;
    dst->terminationtype = src->terminationtype;
}


void _rbfreport_clear(void* _p)
{
    rbfreport *p = (rbfreport*)_p;
    ae_touch_ptr((void*)p);
}


void _rbfreport_destroy(void* _p)
{
    rbfreport *p = (rbfreport*)_p;
    ae_touch_ptr((void*)p);
}


/*$ End $*/
