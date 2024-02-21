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

#ifndef _rbfv1_h
#define _rbfv1_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "scodes.h"
#include "apserv.h"
#include "ablasf.h"
#include "tsort.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "nearestneighbor.h"
#include "ablasmkl.h"
#include "ablas.h"
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
#include "ipm2solver.h"
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
    ae_vector calcbufxcx;
    ae_matrix calcbufx;
    ae_vector calcbuftags;
    kdtreerequestbuffer requestbuffer;
} rbfv1calcbuffer;


/*************************************************************************
RBF model.

Never try to directly work with fields of this object - always use  ALGLIB
functions to use this object.
*************************************************************************/
typedef struct
{
    ae_int_t ny;
    ae_int_t nx;
    ae_int_t nc;
    ae_int_t nl;
    kdtree tree;
    ae_matrix xc;
    ae_matrix wr;
    double rmax;
    ae_matrix v;
    ae_vector calcbufxcx;
    ae_matrix calcbufx;
    ae_vector calcbuftags;
} rbfv1model;


/*************************************************************************
Internal buffer for GridCalc3
*************************************************************************/
typedef struct
{
    ae_vector tx;
    ae_vector cx;
    ae_vector ty;
    ae_vector flag0;
    ae_vector flag1;
    ae_vector flag2;
    ae_vector flag12;
    ae_vector expbuf0;
    ae_vector expbuf1;
    ae_vector expbuf2;
    kdtreerequestbuffer requestbuf;
    ae_matrix calcbufx;
    ae_vector calcbuftags;
} gridcalc3v1buf;


/*************************************************************************
RBF solution report:
* TerminationType   -   termination type, positive values - success,
                        non-positive - failure.
*************************************************************************/
typedef struct
{
    ae_int_t arows;
    ae_int_t acols;
    ae_int_t annz;
    ae_int_t iterationscount;
    ae_int_t nmv;
    ae_int_t terminationtype;
} rbfv1report;


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
void rbfv1create(ae_int_t nx,
     ae_int_t ny,
     rbfv1model* s,
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
void rbfv1createcalcbuffer(const rbfv1model* s,
     rbfv1calcbuffer* buf,
     ae_state *_state);


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
                         interpolation aborted
                  * -4 - nonconvergence of the internal SVD solver
                  *  1 - successful termination
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
void rbfv1buildmodel(/* Real    */ const ae_matrix* x,
     /* Real    */ const ae_matrix* y,
     ae_int_t n,
     ae_int_t aterm,
     ae_int_t algorithmtype,
     ae_int_t nlayers,
     double radvalue,
     double radzvalue,
     double lambdav,
     double epsort,
     double epserr,
     ae_int_t maxits,
     rbfv1model* s,
     rbfv1report* rep,
     ae_state *_state);


/*************************************************************************
Serializer: allocation

  -- ALGLIB --
     Copyright 02.02.2012 by Bochkanov Sergey
*************************************************************************/
void rbfv1alloc(ae_serializer* s,
     const rbfv1model* model,
     ae_state *_state);


/*************************************************************************
Serializer: serialization

  -- ALGLIB --
     Copyright 02.02.2012 by Bochkanov Sergey
*************************************************************************/
void rbfv1serialize(ae_serializer* s,
     const rbfv1model* model,
     ae_state *_state);


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 02.02.2012 by Bochkanov Sergey
*************************************************************************/
void rbfv1unserialize(ae_serializer* s,
     rbfv1model* model,
     ae_state *_state);


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
double rbfv1calc2(rbfv1model* s, double x0, double x1, ae_state *_state);


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
double rbfv1calc3(rbfv1model* s,
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
void rbfv1calcbuf(rbfv1model* s,
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
void rbfv1tscalcbuf(const rbfv1model* s,
     rbfv1calcbuffer* buf,
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
void rbfv1tsdiffbuf(const rbfv1model* s,
     rbfv1calcbuffer* buf,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     /* Real    */ ae_vector* dy,
     ae_state *_state);


/*************************************************************************
This function calculates values of the RBF model at the  given  point  and
its first/second  derivatives,  using  external  buffer  object  (internal
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
    Y, DY, D2Y -   possibly preallocated arrays

OUTPUT PARAMETERS:
    Y       -   function value, array[NY]. Y is not reallocated when it
                is larger than NY.
    DY      -   derivatives, array[NY*NX]. DY is not reallocated when it
                is larger than NY*NX.
    D2Y     -   derivatives, array[NY*NX*NX]. D2Y is not reallocated when
                it is larger than NY*NX*NX.

  -- ALGLIB --
     Copyright 13.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfv1tshessbuf(const rbfv1model* s,
     rbfv1calcbuffer* buf,
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
void rbfv1gridcalc2(rbfv1model* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n0,
     /* Real    */ const ae_vector* x1,
     ae_int_t n1,
     /* Real    */ ae_matrix* y,
     ae_state *_state);


void rbfv1gridcalc3vrec(const rbfv1model* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n0,
     /* Real    */ const ae_vector* x1,
     ae_int_t n1,
     /* Real    */ const ae_vector* x2,
     ae_int_t n2,
     /* Integer */ const ae_vector* blocks0,
     ae_int_t block0a,
     ae_int_t block0b,
     /* Integer */ const ae_vector* blocks1,
     ae_int_t block1a,
     ae_int_t block1b,
     /* Integer */ const ae_vector* blocks2,
     ae_int_t block2a,
     ae_int_t block2b,
     /* Boolean */ const ae_vector* flagy,
     ae_bool sparsey,
     double searchradius,
     double avgfuncpernode,
     ae_shared_pool* bufpool,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void _spawn_rbfv1gridcalc3vrec(const rbfv1model* s,
    /* Real    */ const ae_vector* x0,
    ae_int_t n0,
    /* Real    */ const ae_vector* x1,
    ae_int_t n1,
    /* Real    */ const ae_vector* x2,
    ae_int_t n2,
    /* Integer */ const ae_vector* blocks0,
    ae_int_t block0a,
    ae_int_t block0b,
    /* Integer */ const ae_vector* blocks1,
    ae_int_t block1a,
    ae_int_t block1b,
    /* Integer */ const ae_vector* blocks2,
    ae_int_t block2a,
    ae_int_t block2b,
    /* Boolean */ const ae_vector* flagy,
    ae_bool sparsey,
    double searchradius,
    double avgfuncpernode,
    ae_shared_pool* bufpool,
    /* Real    */ ae_vector* y, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_rbfv1gridcalc3vrec(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_rbfv1gridcalc3vrec(const rbfv1model* s,
    /* Real    */ const ae_vector* x0,
    ae_int_t n0,
    /* Real    */ const ae_vector* x1,
    ae_int_t n1,
    /* Real    */ const ae_vector* x2,
    ae_int_t n2,
    /* Integer */ const ae_vector* blocks0,
    ae_int_t block0a,
    ae_int_t block0b,
    /* Integer */ const ae_vector* blocks1,
    ae_int_t block1a,
    ae_int_t block1b,
    /* Integer */ const ae_vector* blocks2,
    ae_int_t block2a,
    ae_int_t block2b,
    /* Boolean */ const ae_vector* flagy,
    ae_bool sparsey,
    double searchradius,
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
                * last column       - radius, same for all dimensions of
                                      the function being modelled
    NC      -   number of the centers
    V       -   polynomial  term , array[NY,NX+1]. One row per one 
                dimension of the function being modelled. First NX 
                elements are linear coefficients, V[NX] is equal to the 
                constant part.

  -- ALGLIB --
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
void rbfv1unpack(rbfv1model* s,
     ae_int_t* nx,
     ae_int_t* ny,
     /* Real    */ ae_matrix* xwr,
     ae_int_t* nc,
     /* Real    */ ae_matrix* v,
     ae_state *_state);
void _rbfv1calcbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _rbfv1calcbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _rbfv1calcbuffer_clear(void* _p);
void _rbfv1calcbuffer_destroy(void* _p);
void _rbfv1model_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _rbfv1model_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _rbfv1model_clear(void* _p);
void _rbfv1model_destroy(void* _p);
void _gridcalc3v1buf_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _gridcalc3v1buf_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _gridcalc3v1buf_clear(void* _p);
void _gridcalc3v1buf_destroy(void* _p);
void _rbfv1report_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _rbfv1report_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _rbfv1report_clear(void* _p);
void _rbfv1report_destroy(void* _p);


/*$ End $*/
#endif

