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

#ifndef _rbfv3_h
#define _rbfv3_h

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
#include "hblas.h"
#include "sblas.h"
#include "ortfac.h"
#include "blas.h"
#include "bdsvd.h"
#include "svd.h"
#include "hsschur.h"
#include "basicstatops.h"
#include "evd.h"
#include "trlinsolve.h"
#include "safesolve.h"
#include "rcond.h"
#include "fbls.h"
#include "iterativesparse.h"
#include "rbfv3farfields.h"


/*$ Declarations $*/


/*************************************************************************
Buffer object for parallel evaluation on the model matrix
*************************************************************************/
typedef struct
{
    ae_vector x;
    ae_vector y;
    ae_vector coeffbuf;
    ae_vector funcbuf;
    ae_vector wrkbuf;
    ae_vector mindist2;
    ae_vector df1;
    ae_vector df2;
    ae_vector x2;
    ae_vector y2;
    ae_matrix deltabuf;
} rbf3evaluatorbuffer;


/*************************************************************************
A points cluster for the fast evaluator
*************************************************************************/
typedef struct
{
    ae_int_t paneltype;
    double clusterrad;
    ae_vector clustercenter;
    double c0;
    double c1;
    double c2;
    double c3;
    ae_int_t farfieldexpansion;
    double farfielddistance;
    ae_int_t idx0;
    ae_int_t idx1;
    ae_int_t childa;
    ae_int_t childb;
    ae_vector ptidx;
    ae_matrix xt;
    ae_matrix wt;
    biharmonicpanel bhexpansion;
    rbf3evaluatorbuffer tgtbuf;
} rbf3panel;


/*************************************************************************
Fast model evaluator
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t nx;
    ae_int_t ny;
    ae_int_t maxpanelsize;
    ae_int_t functype;
    double funcparam;
    ae_matrix permx;
    ae_vector origptidx;
    ae_matrix wstoredorig;
    ae_bool isloaded;
    ae_obj_array panels;
    biharmonicevaluator bheval;
    ae_shared_pool bufferpool;
    ae_matrix tmpx3w;
    ae_bool usedebugcounters;
    ae_int_t dbgpanel2panelcnt;
    ae_int_t dbgfield2panelcnt;
    ae_int_t dbgpanelscnt;
} rbf3fastevaluator;


/*************************************************************************
Model evaluator:
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t storagetype;
    ae_matrix f;
    ae_int_t nx;
    ae_int_t functype;
    double funcparam;
    ae_int_t chunksize;
    ae_vector entireset;
    ae_matrix x;
    ae_matrix xtchunked;
    ae_shared_pool bufferpool;
    ae_vector chunk1;
} rbf3evaluator;


/*************************************************************************
Buffer object  which  is  used  to  perform  evaluation  requests  in  the
multithreaded mode (multiple threads working with same RBF object).
*************************************************************************/
typedef struct
{
    ae_vector x;
    rbf3evaluatorbuffer evalbuf;
    ae_vector x123;
    ae_vector y123;
    ae_matrix x2d;
    ae_matrix y2d;
    ae_vector xg;
    ae_vector yg;
} rbfv3calcbuffer;


/*************************************************************************
Approximate Cardinal Basis Function builder object

Following fields store problem formulation:
* NTotal                -   total points count in the dataset
* NX                    -   dimensions count
* XX                    -   array[NTotal,NX], points
* FuncType              -   basis function type
* FuncParam             -   basis function parameter
* RoughDatasetDiameter  -   a rough upper bound on the dataset diameter

Following global parameters are set:
* NGlobal               -   global nodes count, >=0
* GlobalGrid            -   global nodes
* GlobalGridSeparation  -   maximum distance between any pair of grid nodes;
                            also an upper bound on distance between any random
                            point in the dataset and a nearest grid node
* NLocal                -   number of nearest neighbors select for each
                            node.
* NCorrection           -   nodes count for each corrector layer
* CorrectorGrowth       -   growth factor for corrector layer
* BatchSize             -   batch size for ACBF construction
* LambdaV               -   smoothing coefficient, LambdaV>=0
* ATerm                 -   linear term for basis functions:
                            * 1 = linear polynomial        (STRONGLY RECOMMENDED)
                            * 2 = constant polynomial term (may break convergence for thin plate splines)
                            * 3 = zero polynomial term     (may break convergence for all types of splines)

Following fields are initialized:
* KDT                   -   KD-tree search structure for the entire dataset
* KDT1, KDT2            -   simplified KD-trees (build with progressively
                            sparsified dataset)
* BufferPool            -   shared pool for ACBFBuffer instances
* ChunksProducer        -   shared pool seeded with an instance of ACBFChunk
                            object (several rows of the preconditioner)
* ChunksPool            -   shared pool that contains computed preconditioner
                            chunks as recycled entries

Temporaries:
* WrkIdx
*************************************************************************/
typedef struct
{
    ae_bool dodetailedtrace;
    ae_int_t ntotal;
    ae_int_t nx;
    ae_matrix xx;
    ae_int_t functype;
    double funcparam;
    double roughdatasetdiameter;
    ae_int_t nglobal;
    ae_vector globalgrid;
    double globalgridseparation;
    ae_int_t nlocal;
    ae_int_t ncorrection;
    double correctorgrowth;
    ae_int_t batchsize;
    double lambdav;
    ae_int_t aterm;
    kdtree kdt;
    kdtree kdt1;
    kdtree kdt2;
    ae_shared_pool bufferpool;
    ae_shared_pool chunksproducer;
    ae_shared_pool chunkspool;
    ae_vector wrkidx;
} acbfbuilder;


/*************************************************************************
Temporary buffers used by divide-and-conquer ACBF preconditioner.

This structure is initialized at the beginning of DC  procedure  and  put
into shared pool. Basecase handling routine retrieves it  from  the  bool
and returns back.

Following fields can be used:
* bFlags        -   boolean array[N], all values are set to False on  the
                    retrieval, and MUST  be  False  when  the  buffer  is
                    returned to the pool
* KDTBuf        -   KD-tree request buffer for thread-safe requests
* KDT1Buf, KDT2Buf- buffers for simplified KD-trees

Additional preallocated temporaries are provided:
* tmpBoxMin     -   array[NX], no special properties
* tmpBoxMax     -   array[NX], no special properties
* TargetNodes   -   dynamically resized as needed
*************************************************************************/
typedef struct
{
    ae_vector bflags;
    kdtreerequestbuffer kdtbuf;
    kdtreerequestbuffer kdt1buf;
    kdtreerequestbuffer kdt2buf;
    ae_vector tmpboxmin;
    ae_vector tmpboxmax;
    ae_vector currentnodes;
    ae_vector neighbors;
    ae_vector chosenneighbors;
    ae_vector y;
    ae_vector z;
    ae_vector d;
    ae_matrix atwrk;
    ae_matrix xq;
    ae_matrix q;
    ae_matrix q1;
    ae_matrix wrkq;
    ae_matrix b;
    ae_matrix c;
    ae_vector choltmp;
    ae_vector tau;
    ae_matrix r;
    ae_vector perm;
} acbfbuffer;


/*************************************************************************
Several rows of the ACBF preconditioner
*************************************************************************/
typedef struct
{
    ae_int_t ntargetrows;
    ae_int_t ntargetcols;
    ae_vector targetrows;
    ae_vector targetcols;
    ae_matrix s;
} acbfchunk;


/*************************************************************************
Temporary buffers used by divide-and-conquer DDM solver

This structure is initialized at the beginning of DC  procedure  and  put
into shared pool. Basecase handling routine retrieves it  from  the  pool
and returns back.

Following fields can be used:
* bFlags        -   boolean array[N], all values are set to False on  the
                    retrieval, and MUST  be  False  when  the  buffer  is
                    returned to the pool
* KDTBuf        -   KD-tree request buffer for thread-safe requests

Additional preallocated temporaries are provided:
* Idx2PrecCol   -   integer array[N+NX+1], no special properties
* tmpBoxMin     -   array[NX], no special properties
* tmpBoxMax     -   array[NX], no special properties
*************************************************************************/
typedef struct
{
    ae_vector bflags;
    ae_vector idx2preccol;
    kdtreerequestbuffer kdtbuf;
    ae_vector tmpboxmin;
    ae_vector tmpboxmax;
} rbf3ddmbuffer;


/*************************************************************************
Subproblem for DDM algorithm, stores precomputed factorization and  other
information.

Following fields are set during construction:
* IsValid       -   whether instance is valid subproblem or not
* NTarget       -   number of target nodes in the subproblem, NTarget>=1
* TargetNodes   -   array containing target node indexes
* NWork         -   number of working nodes in the subproblem, NWork>=NTarget
* WorkingNodes  -   array containing working node indexes
* RegSystem     -   smoothed (regularized) working system
* Decomposition -   decomposition type:
                    * 0 for LU
                    * 1 for regularized QR
* WrkLU         -   NWork*NWork sized LU factorization of the subproblem
* WrkP          -   pivots for the LU decomposition
* WrkQ, WrkR    -   NWork*NWork sized matrices, factors of QR decomposition
                    of RegSystem. Due to regularization rows added, the
                    Q factor is actually an 2NWork*NWork matrix, but in
                    order to solve the system we need only leading NWork
                    rows, so the rest is not stored.
*************************************************************************/
typedef struct
{
    ae_bool isvalid;
    ae_int_t ntarget;
    ae_vector targetnodes;
    ae_int_t nwork;
    ae_vector workingnodes;
    ae_matrix regsystem;
    ae_int_t decomposition;
    ae_matrix wrklu;
    ae_matrix rhs;
    ae_matrix qtrhs;
    ae_matrix sol;
    ae_matrix pred;
    ae_vector wrkp;
    ae_matrix wrkq;
    ae_matrix wrkr;
} rbf3ddmsubproblem;


/*************************************************************************
DDM solver

Following fields store information about problem:
    LambdaV     -   smoothing coefficient
    
Following fields related to DDM part are present:
    SubproblemsCnt-  number of subproblems created, SubproblemCnt>=1
    SubproblemsPool- shared pool seeded with instance of RBFV3DDMSubproblem
                    class (default seed has Seed.IsValid=False).  It  also
                    contains exactly SubproblemCnt subproblem instances as
                    recycled  entries,  each  of   these   instances   has
                    Seed.IsValid=True and  contains  a  partition  of  the
                    complete  problem  into  subproblems  and  precomputed
                    factorization
    SubproblemsBuffer-shared pool seeded with instance of RBFV3DDMSubproblem
                    class (default seed has Seed.IsValid=False).  Contains
                    no recycled entries, should be used just for temporary
                    storage of the already processed subproblems.
                    
Following fields store information about corrector spline:
    NCorrector  -   corrector nodes count, NCorrector>0
    CorrQ       -   Q factor from the QR decomposition of the corrector
                    linear system, array[NCorrector,NCorrector]
    CorrR       -   R factor from the QR decomposition of the corrector
                    linear system, array[NCorrector,NCorrector]
    CorrNodes   -   array[NCorrector], indexes of dataset nodes chosen
                    for the corrector spline
    CorrX       -   array[NCorrector,NX], dataset points

Following fields store information that is used for logging and testing:
    CntLU       -   number of subproblems solved with LU (well conditioned)
    CntRegQR    -   number of subproblems solved with Reg-QR (badly conditioned)    
*************************************************************************/
typedef struct
{
    double lambdav;
    kdtree kdt;
    ae_shared_pool bufferpool;
    ae_int_t subproblemscnt;
    ae_shared_pool subproblemspool;
    ae_shared_pool subproblemsbuffer;
    ae_int_t ncorrector;
    ae_matrix corrq;
    ae_matrix corrr;
    ae_vector corrnodes;
    ae_matrix corrx;
    ae_matrix tmpres1;
    ae_matrix tmpupd1;
    ae_int_t cntlu;
    ae_int_t cntregqr;
} rbf3ddmsolver;


/*************************************************************************
RBF model.

Never try to work with fields of this object directly - always use  ALGLIB
functions to use this object.
*************************************************************************/
typedef struct
{
    ae_int_t ny;
    ae_int_t nx;
    ae_int_t bftype;
    double bfparam;
    ae_vector s;
    ae_matrix v;
    ae_vector cw;
    ae_vector pointindexes;
    ae_int_t nc;
    rbf3evaluator evaluator;
    rbf3fastevaluator fasteval;
    ae_matrix wchunked;
    rbfv3calcbuffer calcbuf;
    ae_bool dbgregqrusedforddm;
    double dbgworstfirstdecay;
} rbfv3model;


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
    ae_int_t iterationscount;
} rbfv3report;


/*$ Body $*/


/*************************************************************************
This function creates RBF  model  for  a  scalar (NY=1)  or  vector (NY>1)
function in a NX-dimensional space (NX>=1).

INPUT PARAMETERS:
    NX      -   dimension of the space, NX>=1
    NY      -   function dimension, NY>=1
    BF      -   basis function type:
                * 1 for biharmonic/multiquadric f=sqrt(r^2+alpha^2) (with f=r being a special case)
                * 2 for polyharmonic f=r^2*ln(r)
    BFP     -   basis function parameter:
                * BF=0      parameter ignored

OUTPUT PARAMETERS:
    S       -   RBF model (initially equals to zero)

  -- ALGLIB --
     Copyright 13.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfv3create(ae_int_t nx,
     ae_int_t ny,
     ae_int_t bf,
     double bfp,
     rbfv3model* s,
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
     Copyright 02.04.2022 by Sergey Bochkanov
*************************************************************************/
void rbfv3createcalcbuffer(const rbfv3model* s,
     rbfv3calcbuffer* buf,
     ae_state *_state);


/*************************************************************************
This function builds hierarchical RBF model.

INPUT PARAMETERS:
    X       -   array[N,S.NX], X-values
    Y       -   array[N,S.NY], Y-values
    ScaleVec-   array[S.NX], vector of per-dimension scales
    N       -   points count
    BFtype  -   basis function type:
                * 1 for biharmonic spline f=r or multiquadric f=sqrt(r^2+param^2)
                * 2 for thin plate spline f=r^2*ln(r)
    BFParam -   for BFType=1 zero value means biharmonic, nonzero means multiquadric
                ignored for BFType=2
    LambdaV -   regularization parameter
    ATerm   -   polynomial term type:
                * 1 for linear term (STRONGLY RECOMMENDED)
                * 2 for constant term (may break convergence guarantees for thin plate splines)
                * 3 for zero term (may break convergence guarantees for all types of splines)
    RBFProfile- RBF profile to use:
                *  0 for the 'standard' profile
                * -1 for the 'debug' profile intended to test all possible code branches even
                     on small-scale problems. The idea is to choose very small batch sizes and
                     threshold values, such that small problems with N=100..200 can test all
                     nested levels of the algorithm.
    TOL     -   desired relative accuracy:
                * should between 1E-3 and 1E-6
                * values higher than 1E-3 usually make no sense (bad accuracy, no performance benefits)
                * values below 1E-6 may result in algorithm taking too much time,
                  so we silently override them to 1.0E-6
    S       -   RBF model, already initialized by RBFCreate() call.
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
                  *  1 - successful termination
                  *  8 terminated by user via rbfrequesttermination()
                Fields are used for debugging purposes:
                * Rep.IterationsCount - iterations count of the GMRES solver

NOTE:  failure  to  build  model will leave current state of the structure
unchanged.

  -- ALGLIB --
     Copyright 12.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfv3build(/* Real    */ const ae_matrix* xraw,
     /* Real    */ const ae_matrix* yraw,
     ae_int_t nraw,
     /* Real    */ const ae_vector* scaleraw,
     ae_int_t bftype,
     double bfparamraw,
     double lambdavraw,
     ae_int_t aterm,
     ae_int_t rbfprofile,
     double tol,
     rbfv3model* s,
     ae_int_t* progress10000,
     ae_bool* terminationrequest,
     rbfv3report* rep,
     ae_state *_state);


/*************************************************************************
Serializer: allocation

  -- ALGLIB --
     Copyright 12.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfv3alloc(ae_serializer* s,
     const rbfv3model* model,
     ae_state *_state);


/*************************************************************************
Serializer: serialization

  -- ALGLIB --
     Copyright 12.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfv3serialize(ae_serializer* s,
     const rbfv3model* model,
     ae_state *_state);


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 12.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfv3unserialize(ae_serializer* s,
     rbfv3model* model,
     ae_state *_state);


/*************************************************************************
This function calculates values of the RBF model in the given point.

This function should be used when we have NY=1 (scalar function) and  NX=1
(1-dimensional space).

This function returns 0.0 when:
* the model is not initialized
* NX<>1
 *NY<>1

INPUT PARAMETERS:
    S       -   RBF model
    X0      -   X-coordinate, finite number

RESULT:
    value of the model or 0.0 (as defined above)

  -- ALGLIB --
     Copyright 12.12.2021 by Bochkanov Sergey
*************************************************************************/
double rbfv3calc1(rbfv3model* s, double x0, ae_state *_state);


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
     Copyright 12.12.2021 by Bochkanov Sergey
*************************************************************************/
double rbfv3calc2(rbfv3model* s, double x0, double x1, ae_state *_state);


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
     Copyright 12.12.2021 by Bochkanov Sergey
*************************************************************************/
double rbfv3calc3(rbfv3model* s,
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
void rbfv3calcbuf(rbfv3model* s,
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
     Copyright 12.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfv3tscalcbuf(const rbfv3model* s,
     rbfv3calcbuffer* buf,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);


/*************************************************************************
This function performs fast calculation  using  far  field  expansion  (if
supported for a current model, and if model size justifies utilization  of
far fields), using currently stored fast evaluation tolerance.

If no far field is present, straightforward O(N) evaluation is performed.

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
     Copyright 01.11.2022 by Bochkanov Sergey
*************************************************************************/
void rbfv3tsfastcalcbuf(rbfv3model* s,
     rbfv3calcbuffer* buf,
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
void rbfv3tsdiffbuf(const rbfv3model* s,
     rbfv3calcbuffer* buf,
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
    D2Y     -   second derivatives, array[NY*NX*NX].
                D2Y is not reallocated when it is larger than NY*NX*NX.

  -- ALGLIB --
     Copyright 13.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfv3tshessbuf(const rbfv3model* s,
     rbfv3calcbuffer* buf,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     /* Real    */ ae_vector* dy,
     /* Real    */ ae_vector* d2y,
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
void rbfv3gridcalcvx(const rbfv3model* s,
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


/*************************************************************************
This function "unpacks" RBF model by extracting its coefficients.

INPUT PARAMETERS:
    S       -   RBF model

OUTPUT PARAMETERS:
    NX      -   dimensionality of argument
    NY      -   dimensionality of the target function
    XWR     -   model information, array[NC,NX+NY+NX+2].
                One row of the array corresponds to one basis function
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

  -- ALGLIB --
     Copyright 12.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfv3unpack(rbfv3model* s,
     ae_int_t* nx,
     ae_int_t* ny,
     /* Real    */ ae_matrix* xwr,
     ae_int_t* nc,
     /* Real    */ ae_matrix* v,
     ae_state *_state);


/*************************************************************************
Get maximum panel size for a fast evaluator

  -- ALGLIB --
     Copyright 27.08.2022 by Sergey Bochkanov
*************************************************************************/
ae_int_t rbf3getmaxpanelsize(ae_state *_state);


/*************************************************************************
Changes fast evaluator tolerance.

The  original  fast  evaluator  provides too conservative error estimates,
even when relaxed tolerances are used.  Thus,  far  field  expansions  are
often ignored when it is pretty safe  to use them and  save  computational
time.

This function does the following:
* pushes 'raw' tolerance specified by user to the fast evaluator
* samples about 100 randomly selected points, computes  true  average  and
  maximum errors of the fast evaluator on these points.
  Usually these errors are 1000x-10000x less than ones allowed by user.
* adjusts 'raw' tolerance, producing so called 'working' tolerance in order
  to bring maximum error closer to limits, improving performance due to
  looser tolerances

The running time of this function is O(N). It is not thread-safe.

  -- ALGLIB --
     Copyright 01.11.2022 by Sergey Bochkanov
*************************************************************************/
void rbf3pushfastevaltol(rbfv3model* s, double tol, ae_state *_state);
void _rbf3evaluatorbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _rbf3evaluatorbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _rbf3evaluatorbuffer_clear(void* _p);
void _rbf3evaluatorbuffer_destroy(void* _p);
void _rbf3panel_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _rbf3panel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _rbf3panel_clear(void* _p);
void _rbf3panel_destroy(void* _p);
void _rbf3fastevaluator_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _rbf3fastevaluator_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _rbf3fastevaluator_clear(void* _p);
void _rbf3fastevaluator_destroy(void* _p);
void _rbf3evaluator_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _rbf3evaluator_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _rbf3evaluator_clear(void* _p);
void _rbf3evaluator_destroy(void* _p);
void _rbfv3calcbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _rbfv3calcbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _rbfv3calcbuffer_clear(void* _p);
void _rbfv3calcbuffer_destroy(void* _p);
void _acbfbuilder_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _acbfbuilder_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _acbfbuilder_clear(void* _p);
void _acbfbuilder_destroy(void* _p);
void _acbfbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _acbfbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _acbfbuffer_clear(void* _p);
void _acbfbuffer_destroy(void* _p);
void _acbfchunk_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _acbfchunk_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _acbfchunk_clear(void* _p);
void _acbfchunk_destroy(void* _p);
void _rbf3ddmbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _rbf3ddmbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _rbf3ddmbuffer_clear(void* _p);
void _rbf3ddmbuffer_destroy(void* _p);
void _rbf3ddmsubproblem_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _rbf3ddmsubproblem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _rbf3ddmsubproblem_clear(void* _p);
void _rbf3ddmsubproblem_destroy(void* _p);
void _rbf3ddmsolver_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _rbf3ddmsolver_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _rbf3ddmsolver_clear(void* _p);
void _rbf3ddmsolver_destroy(void* _p);
void _rbfv3model_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _rbfv3model_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _rbfv3model_clear(void* _p);
void _rbfv3model_destroy(void* _p);
void _rbfv3report_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _rbfv3report_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _rbfv3report_clear(void* _p);
void _rbfv3report_destroy(void* _p);


/*$ End $*/
#endif

