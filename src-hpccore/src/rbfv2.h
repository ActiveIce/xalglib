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

#ifndef _rbfv2_h
#define _rbfv2_h

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
#include "ablasmkl.h"
#include "ablas.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "sparse.h"
#include "dlu.h"
#include "sptrf.h"
#include "apstruct.h"
#include "amdordering.h"
#include "spchol.h"
#include "creflections.h"
#include "matgen.h"
#include "rotations.h"
#include "trfac.h"
#include "intfitserv.h"
#include "ratint.h"
#include "polint.h"
#include "hblas.h"
#include "sblas.h"
#include "ortfac.h"
#include "fbls.h"
#include "normestimator.h"
#include "blas.h"
#include "bdsvd.h"
#include "svd.h"
#include "linlsqr.h"
#include "spline1d.h"
#include "optguardapi.h"
#include "hsschur.h"
#include "basicstatops.h"
#include "evd.h"
#include "trlinsolve.h"
#include "safesolve.h"
#include "rcond.h"
#include "matinv.h"
#include "optserv.h"
#include "xblas.h"
#include "directdensesolvers.h"
#include "linmin.h"
#include "minlbfgs.h"
#include "cqmodels.h"
#include "lpqpserv.h"
#include "snnls.h"
#include "sactivesets.h"
#include "qqpsolver.h"
#include "qpdenseaulsolver.h"
#include "minbleic.h"
#include "qpbleicsolver.h"
#include "vipmsolver.h"
#include "minqp.h"
#include "minlm.h"
#include "lsfit.h"


/*$ Declarations $*/


/*************************************************************************
Buffer object which is used to perform nearest neighbor  requests  in  the
multithreaded mode (multiple threads working with same KD-tree object).

This object should be created with KDTreeCreateBuffer().
*************************************************************************/
typedef struct
{
    ae_vector x;
    ae_vector curboxmin;
    ae_vector curboxmax;
    double curdist2;
    ae_vector x123;
    ae_vector y123;
} rbfv2calcbuffer;


/*************************************************************************
RBF model.

Never try to work with fields of this object directly - always use  ALGLIB
functions to use this object.
*************************************************************************/
typedef struct
{
    ae_int_t ny;
    ae_int_t nx;
    ae_int_t bf;
    ae_int_t nh;
    ae_vector ri;
    ae_vector s;
    ae_vector kdroots;
    ae_vector kdnodes;
    ae_vector kdsplits;
    ae_vector kdboxmin;
    ae_vector kdboxmax;
    ae_vector cw;
    ae_matrix v;
    double lambdareg;
    ae_int_t maxits;
    double supportr;
    ae_int_t basisfunction;
    rbfv2calcbuffer calcbuf;
} rbfv2model;


/*************************************************************************
Internal buffer for GridCalc3
*************************************************************************/
typedef struct
{
    rbfv2calcbuffer calcbuf;
    ae_vector cx;
    ae_vector rx;
    ae_vector ry;
    ae_vector tx;
    ae_vector ty;
    ae_vector rf;
} rbfv2gridcalcbuffer;


/*************************************************************************
RBF solution report:
* TerminationType   -   termination type, positive values - success,
                        non-positive - failure.
*************************************************************************/
typedef struct
{
    ae_int_t terminationtype;
    double maxerror;
    double rmserror;
} rbfv2report;


/*$ Body $*/


/*************************************************************************
This function creates RBF  model  for  a  scalar (NY=1)  or  vector (NY>1)
function in a NX-dimensional space (NX=2 or NX=3).

INPUT PARAMETERS:
    NX      -   dimension of the space, NX=2 or NX=3
    NY      -   function dimension, NY>=1

OUTPUT PARAMETERS:
    S       -   RBF model (initially equals to zero)

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
void rbfv2create(ae_int_t nx,
     ae_int_t ny,
     rbfv2model* s,
     ae_state *_state);


/*************************************************************************
This function creates buffer  structure  which  can  be  used  to  perform
parallel  RBF  model  evaluations  (with  one  RBF  model  instance  being
used from multiple threads, as long as  different  threads  use  different
instances of buffer).

This buffer object can be used with  rbftscalcbuf()  function  (here  "ts"
stands for "thread-safe", "buf" is a suffix which denotes  function  which
reuses previously allocated output space).

How to use it:
* create RBF model structure with rbfcreate()
* load data, tune parameters
* call rbfbuildmodel()
* call rbfcreatecalcbuffer(), once per thread working with RBF model  (you
  should call this function only AFTER call to rbfbuildmodel(), see  below
  for more information)
* call rbftscalcbuf() from different threads,  with  each  thread  working
  with its own copy of buffer object.

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
void rbfv2createcalcbuffer(const rbfv2model* s,
     rbfv2calcbuffer* buf,
     ae_state *_state);


/*************************************************************************
This   function  builds hierarchical RBF model.

INPUT PARAMETERS:
    X       -   array[N,S.NX], X-values
    Y       -   array[N,S.NY], Y-values
    ScaleVec-   array[S.NX], vector of per-dimension scales
    N       -   points count
    ATerm   -   linear term type, 1 for linear, 2 for constant, 3 for zero.
    NH      -   hierarchy height
    RBase   -   base RBF radius
    BF      -   basis function type: 0 for Gaussian, 1 for compact
    LambdaNS-   non-smoothness penalty coefficient. Exactly zero value means
                that no penalty is applied, and even system matrix does not
                contain penalty-related rows. Value of 1 means
    S       -   RBF model, initialized by RBFCreate() call.
    progress10000- variable used for progress reports, it is regularly set
                to the current progress multiplied by 10000, in order to
                get value in [0,10000] range. The rationale for such scaling
                is that it allows us to use integer type to store progress,
                which has less potential for non-atomic corruption on unprotected
                reads from another threads.
                You can read this variable from some other thread to get
                estimate of the current progress.
                Initial value of this variable is ignored, it is written by
                this function, but not read.
    terminationrequest - variable used for termination requests; its initial
                value must be False, and you can set it to True from some
                other thread. This routine regularly checks this variable
                and will terminate model construction shortly upon discovering
                that termination was requested.
    
OUTPUT PARAMETERS:
    S       -   updated model (for rep.terminationtype>0, unchanged otherwise)
    Rep     -   report:
                * Rep.TerminationType:
                  * -5 - non-distinct basis function centers were detected,
                         interpolation aborted
                  * -4 - nonconvergence of the internal SVD solver
                  *  1 - successful termination
                  *  8 terminated by user via rbfrequesttermination()
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
     Copyright 20.06.2016 by Bochkanov Sergey
*************************************************************************/
void rbfv2buildhierarchical(/* Real    */ const ae_matrix* x,
     /* Real    */ const ae_matrix* y,
     ae_int_t n,
     /* Real    */ const ae_vector* scalevec,
     ae_int_t aterm,
     ae_int_t nh,
     double rbase,
     double lambdans,
     rbfv2model* s,
     ae_int_t* progress10000,
     ae_bool* terminationrequest,
     rbfv2report* rep,
     ae_state *_state);


/*************************************************************************
Serializer: allocation

  -- ALGLIB --
     Copyright 02.02.2012 by Bochkanov Sergey
*************************************************************************/
void rbfv2alloc(ae_serializer* s,
     const rbfv2model* model,
     ae_state *_state);


/*************************************************************************
Serializer: serialization

  -- ALGLIB --
     Copyright 02.02.2012 by Bochkanov Sergey
*************************************************************************/
void rbfv2serialize(ae_serializer* s,
     const rbfv2model* model,
     ae_state *_state);


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 02.02.2012 by Bochkanov Sergey
*************************************************************************/
void rbfv2unserialize(ae_serializer* s,
     rbfv2model* model,
     ae_state *_state);


/*************************************************************************
Returns far radius for basis function type
*************************************************************************/
double rbfv2farradius(ae_int_t bf, ae_state *_state);


/*************************************************************************
Returns near radius for basis function type
*************************************************************************/
double rbfv2nearradius(ae_int_t bf, ae_state *_state);


/*************************************************************************
Returns basis function value.
Assumes that D2>=0
*************************************************************************/
double rbfv2basisfunc(ae_int_t bf, double d2, ae_state *_state);


/*************************************************************************
Returns basis function value, first and second derivatives
Assumes that D2>=0
*************************************************************************/
void rbfv2basisfuncdiff2(ae_int_t bf,
     double d2,
     double* f,
     double* df,
     double* d2f,
     ae_state *_state);


/*************************************************************************
This function calculates values of the RBF model in the given point.

This function should be used when we have NY=1 (scalar function) and  NX=1
(1-dimensional space).

This function returns 0.0 when:
* model is not initialized
* NX<>1
 *NY<>1

INPUT PARAMETERS:
    S       -   RBF model
    X0      -   X-coordinate, finite number

RESULT:
    value of the model or 0.0 (as defined above)

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
double rbfv2calc1(rbfv2model* s, double x0, ae_state *_state);


/*************************************************************************
This function calculates values of the RBF model in the given point.

This function should be used when we have NY=1 (scalar function) and  NX=2
(2-dimensional space). If you have 3-dimensional space, use RBFCalc3(). If
you have general situation (NX-dimensional space, NY-dimensional function)
you should use general, less efficient implementation RBFCalc().

If  you  want  to  calculate  function  values  many times, consider using 
RBFGridCalc2(), which is far more efficient than many subsequent calls  to
RBFCalc2().

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
double rbfv2calc2(rbfv2model* s, double x0, double x1, ae_state *_state);


/*************************************************************************
This function calculates values of the RBF model in the given point.

This function should be used when we have NY=1 (scalar function) and  NX=3
(3-dimensional space). If you have 2-dimensional space, use RBFCalc2(). If
you have general situation (NX-dimensional space, NY-dimensional function)
you should use general, less efficient implementation RBFCalc().

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
double rbfv2calc3(rbfv2model* s,
     double x0,
     double x1,
     double x2,
     ae_state *_state);


/*************************************************************************
This function calculates values of the RBF model at the given point.

Same as RBFCalc(), but does not reallocate Y when in is large enough to 
store function values.

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
void rbfv2calcbuf(rbfv2model* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);


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
void rbfv2tscalcbuf(const rbfv2model* s,
     rbfv2calcbuffer* buf,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);


/*************************************************************************
This function calculates values of the RBF model at the  given  point  and
its derivatives, using external buffer object (internal temporaries of the
RBF model are not modified).

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
    Y, DY   -   possibly preallocated arrays

OUTPUT PARAMETERS:
    Y       -   function value, array[NY]. Y is not reallocated when it
                is larger than NY.
    DY      -   derivatives, array[NY*NX]. DY is not reallocated when it
                is larger than NY*NX.

  -- ALGLIB --
     Copyright 13.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfv2tsdiffbuf(const rbfv2model* s,
     rbfv2calcbuffer* buf,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     /* Real    */ ae_vector* dy,
     ae_state *_state);


/*************************************************************************
This function calculates values of the RBF model at the  given  point  and
its first and second derivatives, using external buffer  object  (internal
temporaries of the RBF model are not modified).

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
    Y,DY,D2Y -  possibly preallocated arrays

OUTPUT PARAMETERS:
    Y       -   function value, array[NY]. Y is not reallocated when it
                is larger than NY.
    DY      -   derivatives, array[NY*NX]. DY is not reallocated when it
                is larger than NY*NX.
    D2Y     -   second derivatives, array[NY*NX*NX]

  -- ALGLIB --
     Copyright 13.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfv2tshessbuf(const rbfv2model* s,
     rbfv2calcbuffer* buf,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     /* Real    */ ae_vector* dy,
     /* Real    */ ae_vector* d2y,
     ae_state *_state);


/*************************************************************************
This function calculates values of the RBF model at the regular grid.

Grid have N0*N1 points, with Point[I,J] = (X0[I], X1[J])

This function returns 0.0 when:
* model is not initialized
* NX<>2
 *NY<>1

INPUT PARAMETERS:
    S       -   RBF model
    X0      -   array of grid nodes, first coordinates, array[N0]
    N0      -   grid size (number of nodes) in the first dimension
    X1      -   array of grid nodes, second coordinates, array[N1]
    N1      -   grid size (number of nodes) in the second dimension

OUTPUT PARAMETERS:
    Y       -   function values, array[N0,N1]. Y is out-variable and 
                is reallocated by this function.
                
NOTE: as a special exception, this function supports unordered  arrays  X0
      and X1. However, future versions may be  more  efficient  for  X0/X1
      ordered by ascending.

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
void rbfv2gridcalc2(rbfv2model* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n0,
     /* Real    */ const ae_vector* x1,
     ae_int_t n1,
     /* Real    */ ae_matrix* y,
     ae_state *_state);


/*************************************************************************
This function is used to perform gridded calculation  for  2D,  3D  or  4D
problems. It accepts parameters X0...X3 and counters N0...N3. If RBF model
has dimensionality less than 4, corresponding arrays should  contain  just
one element equal to zero, and corresponding N's should be equal to 1.

NOTE: array Y should be preallocated by caller.

  -- ALGLIB --
     Copyright 12.07.2016 by Bochkanov Sergey
*************************************************************************/
void rbfv2gridcalcvx(const rbfv2model* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n0,
     /* Real    */ const ae_vector* x1,
     ae_int_t n1,
     /* Real    */ const ae_vector* x2,
     ae_int_t n2,
     /* Real    */ const ae_vector* x3,
     ae_int_t n3,
     /* Boolean */ const ae_vector* flagy,
     ae_bool sparsey,
     /* Real    */ ae_vector* y,
     ae_state *_state);


void rbfv2partialgridcalcrec(const rbfv2model* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n0,
     /* Real    */ const ae_vector* x1,
     ae_int_t n1,
     /* Real    */ const ae_vector* x2,
     ae_int_t n2,
     /* Real    */ const ae_vector* x3,
     ae_int_t n3,
     /* Integer */ const ae_vector* blocks0,
     ae_int_t block0a,
     ae_int_t block0b,
     /* Integer */ const ae_vector* blocks1,
     ae_int_t block1a,
     ae_int_t block1b,
     /* Integer */ const ae_vector* blocks2,
     ae_int_t block2a,
     ae_int_t block2b,
     /* Integer */ const ae_vector* blocks3,
     ae_int_t block3a,
     ae_int_t block3b,
     /* Boolean */ const ae_vector* flagy,
     ae_bool sparsey,
     ae_int_t levelidx,
     double avgfuncpernode,
     ae_shared_pool* bufpool,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void _spawn_rbfv2partialgridcalcrec(const rbfv2model* s,
    /* Real    */ const ae_vector* x0,
    ae_int_t n0,
    /* Real    */ const ae_vector* x1,
    ae_int_t n1,
    /* Real    */ const ae_vector* x2,
    ae_int_t n2,
    /* Real    */ const ae_vector* x3,
    ae_int_t n3,
    /* Integer */ const ae_vector* blocks0,
    ae_int_t block0a,
    ae_int_t block0b,
    /* Integer */ const ae_vector* blocks1,
    ae_int_t block1a,
    ae_int_t block1b,
    /* Integer */ const ae_vector* blocks2,
    ae_int_t block2a,
    ae_int_t block2b,
    /* Integer */ const ae_vector* blocks3,
    ae_int_t block3a,
    ae_int_t block3b,
    /* Boolean */ const ae_vector* flagy,
    ae_bool sparsey,
    ae_int_t levelidx,
    double avgfuncpernode,
    ae_shared_pool* bufpool,
    /* Real    */ ae_vector* y, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_rbfv2partialgridcalcrec(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_rbfv2partialgridcalcrec(const rbfv2model* s,
    /* Real    */ const ae_vector* x0,
    ae_int_t n0,
    /* Real    */ const ae_vector* x1,
    ae_int_t n1,
    /* Real    */ const ae_vector* x2,
    ae_int_t n2,
    /* Real    */ const ae_vector* x3,
    ae_int_t n3,
    /* Integer */ const ae_vector* blocks0,
    ae_int_t block0a,
    ae_int_t block0b,
    /* Integer */ const ae_vector* blocks1,
    ae_int_t block1a,
    ae_int_t block1b,
    /* Integer */ const ae_vector* blocks2,
    ae_int_t block2a,
    ae_int_t block2b,
    /* Integer */ const ae_vector* blocks3,
    ae_int_t block3a,
    ae_int_t block3b,
    /* Boolean */ const ae_vector* flagy,
    ae_bool sparsey,
    ae_int_t levelidx,
    double avgfuncpernode,
    ae_shared_pool* bufpool,
    /* Real    */ ae_vector* y, ae_state *_state);


/*************************************************************************
This function "unpacks" RBF model by extracting its coefficients.

INPUT PARAMETERS:
    S       -   RBF model

OUTPUT PARAMETERS:
    NX      -   dimensionality of argument
    NY      -   dimensionality of the target function
    XWR     -   model information, array[NC,NX+NY+1].
                One row of the array corresponds to one basis function:
                * first NX columns  - coordinates of the center 
                * next NY columns   - weights, one per dimension of the 
                                      function being modelled
                * last NX columns   - radii, per dimension
    NC      -   number of the centers
    V       -   polynomial  term , array[NY,NX+1]. One row per one 
                dimension of the function being modelled. First NX 
                elements are linear coefficients, V[NX] is equal to the 
                constant part.

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
void rbfv2unpack(rbfv2model* s,
     ae_int_t* nx,
     ae_int_t* ny,
     /* Real    */ ae_matrix* xwr,
     ae_int_t* nc,
     /* Real    */ ae_matrix* v,
     ae_state *_state);
void _rbfv2calcbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _rbfv2calcbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _rbfv2calcbuffer_clear(void* _p);
void _rbfv2calcbuffer_destroy(void* _p);
void _rbfv2model_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _rbfv2model_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _rbfv2model_clear(void* _p);
void _rbfv2model_destroy(void* _p);
void _rbfv2gridcalcbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _rbfv2gridcalcbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _rbfv2gridcalcbuffer_clear(void* _p);
void _rbfv2gridcalcbuffer_destroy(void* _p);
void _rbfv2report_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _rbfv2report_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _rbfv2report_clear(void* _p);
void _rbfv2report_destroy(void* _p);


/*$ End $*/
#endif

