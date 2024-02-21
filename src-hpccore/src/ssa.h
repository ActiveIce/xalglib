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

#ifndef _ssa_h
#define _ssa_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "hblas.h"
#include "creflections.h"
#include "sblas.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "ortfac.h"
#include "blas.h"
#include "rotations.h"
#include "bdsvd.h"
#include "svd.h"
#include "matgen.h"
#include "scodes.h"
#include "tsort.h"
#include "sparse.h"
#include "hsschur.h"
#include "basicstatops.h"
#include "evd.h"


/*$ Declarations $*/


/*************************************************************************
This object stores state of the SSA model.

You should use ALGLIB functions to work with this object.
*************************************************************************/
typedef struct
{
    ae_int_t nsequences;
    ae_vector sequenceidx;
    ae_vector sequencedata;
    ae_int_t algotype;
    ae_int_t windowwidth;
    ae_int_t rtpowerup;
    ae_int_t topk;
    ae_int_t precomputedwidth;
    ae_int_t precomputednbasis;
    ae_matrix precomputedbasis;
    ae_int_t defaultsubspaceits;
    ae_int_t memorylimit;
    ae_bool arebasisandsolvervalid;
    ae_matrix basis;
    ae_matrix basist;
    ae_vector sv;
    ae_vector forecasta;
    ae_int_t nbasis;
    eigsubspacestate solver;
    ae_matrix xxt;
    hqrndstate rs;
    ae_int_t rngseed;
    ae_vector rtqueue;
    ae_int_t rtqueuecnt;
    ae_int_t rtqueuechunk;
    ae_int_t dbgcntevd;
    ae_vector tmp0;
    ae_vector tmp1;
    eigsubspacereport solverrep;
    ae_vector alongtrend;
    ae_vector alongnoise;
    ae_matrix aseqtrajectory;
    ae_matrix aseqtbproduct;
    ae_vector aseqcounts;
    ae_vector fctrend;
    ae_vector fcnoise;
    ae_matrix fctrendm;
    ae_matrix uxbatch;
    ae_int_t uxbatchwidth;
    ae_int_t uxbatchsize;
    ae_int_t uxbatchlimit;
} ssamodel;


/*$ Body $*/


/*************************************************************************
This function creates SSA model object.  Right after creation model is  in
"dummy" mode - you can add data,  but   analyzing/prediction  will  return
just zeros (it assumes that basis is empty).

HOW TO USE SSA MODEL:

1. create model with ssacreate()
2. add data with one/many ssaaddsequence() calls
3. choose SSA algorithm with one of ssasetalgo...() functions:
   * ssasetalgotopkdirect() for direct one-run analysis
   * ssasetalgotopkrealtime() for algorithm optimized for many  subsequent
     runs with warm-start capabilities
   * ssasetalgoprecomputed() for user-supplied basis
4. set window width with ssasetwindow()
5. perform one of the analysis-related activities:
   a) call ssagetbasis() to get basis
   b) call ssaanalyzelast() ssaanalyzesequence() or ssaanalyzelastwindow()
      to perform analysis (trend/noise separation)
   c) call  one  of   the   forecasting   functions  (ssaforecastlast() or
      ssaforecastsequence()) to perform prediction; alternatively, you can
      extract linear recurrence coefficients with ssagetlrr().
   SSA analysis will be performed during first  call  to  analysis-related
   function. SSA model is smart enough to track all changes in the dataset
   and  model  settings,  to  cache  previously  computed  basis  and   to
   re-evaluate basis only when necessary.
      
Additionally, if your setting involves constant stream  of  incoming data,
you can perform quick update already calculated  model  with  one  of  the
incremental   append-and-update   functions:  ssaappendpointandupdate() or
ssaappendsequenceandupdate().

NOTE: steps (2), (3), (4) can be performed in arbitrary order.
   
INPUT PARAMETERS:
    none

OUTPUT PARAMETERS:
    S               -   structure which stores model state

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
void ssacreate(ssamodel* s, ae_state *_state);


/*************************************************************************
This function sets window width for SSA model. You should call  it  before
analysis phase. Default window width is 1 (not for real use).

Special notes:
* this function call can be performed at any moment before  first call  to
  analysis-related functions
* changing window width invalidates internally stored basis; if you change
  window width AFTER you call analysis-related  function,  next  analysis
  phase will require re-calculation of  the  basis  according  to  current
  algorithm.
* calling this function with exactly  same window width as current one has
  no effect
* if you specify window width larger  than any data sequence stored in the
  model, analysis will return zero basis.
   
INPUT PARAMETERS:
    S               -   SSA model created with ssacreate()
    WindowWidth     -   >=1, new window width

OUTPUT PARAMETERS:
    S               -   SSA model, updated

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
void ssasetwindow(ssamodel* s, ae_int_t windowwidth, ae_state *_state);


/*************************************************************************
This  function  sets  seed  which  is used to initialize internal RNG when
we make pseudorandom decisions on model updates.

By default, deterministic seed is used - which results in same sequence of
pseudorandom decisions every time you run SSA model. If you  specify  non-
deterministic seed value, then SSA  model  may  return  slightly different
results after each run.

This function can be useful when you have several SSA models updated  with
sseappendpointandupdate() called with 0<UpdateIts<1 (fractional value) and
due to performance limitations want them to perform updates  at  different
moments.

INPUT PARAMETERS:
    S       -   SSA model
    Seed    -   seed:
                * positive values = use deterministic seed for each run of
                  algorithms which depend on random initialization
                * zero or negative values = use non-deterministic seed

  -- ALGLIB --
     Copyright 03.11.2017 by Bochkanov Sergey
*************************************************************************/
void ssasetseed(ssamodel* s, ae_int_t seed, ae_state *_state);


/*************************************************************************
This function sets length of power-up cycle for real-time algorithm.

By default, this algorithm performs costly O(N*WindowWidth^2)  init  phase
followed by full run of truncated  EVD.  However,  if  you  are  ready  to
live with a bit lower-quality basis during first few iterations,  you  can
split this O(N*WindowWidth^2) initialization  between  several  subsequent
append-and-update rounds. It results in better latency of the algorithm.

This function invalidates basis/solver, next analysis call will result  in
full recalculation of everything.

INPUT PARAMETERS:
    S       -   SSA model
    PWLen   -   length of the power-up stage:
                * 0 means that no power-up is requested
                * 1 is the same as 0
                * >1 means that delayed power-up is performed

  -- ALGLIB --
     Copyright 03.11.2017 by Bochkanov Sergey
*************************************************************************/
void ssasetpoweruplength(ssamodel* s, ae_int_t pwlen, ae_state *_state);


/*************************************************************************
This function sets memory limit of SSA analysis.

Straightforward SSA with sequence length T and window width W needs O(T*W)
memory. It is possible to reduce memory consumption by splitting task into
smaller chunks.

Thus function allows you to specify approximate memory limit (measured  in
double precision numbers used for buffers). Actual memory consumption will
be comparable to the number specified by you.

Default memory limit is 50.000.000 (400Mbytes) in current version.

INPUT PARAMETERS:
    S       -   SSA model
    MemLimit-   memory limit, >=0. Zero value means no limit.

  -- ALGLIB --
     Copyright 20.12.2017 by Bochkanov Sergey
*************************************************************************/
void ssasetmemorylimit(ssamodel* s, ae_int_t memlimit, ae_state *_state);


/*************************************************************************
This function adds data sequence to SSA  model.  Only   single-dimensional
sequences are supported.

What is a sequences? Following definitions/requirements apply:
* a sequence  is  an  array of  values  measured  in  subsequent,  equally
  separated time moments (ticks).
* you may have many sequences  in your  dataset;  say,  one  sequence  may
  correspond to one trading session.
* sequence length should be larger  than current  window  length  (shorter
  sequences will be ignored during analysis).
* analysis is performed within a  sequence; different  sequences  are  NOT
  stacked together to produce one large contiguous stream of data.
* analysis is performed for all  sequences at once, i.e. same set of basis
  vectors is computed for all sequences
  
INCREMENTAL ANALYSIS
  
This function is non intended for  incremental updates of previously found
SSA basis. Calling it invalidates  all previous analysis results (basis is
reset and will be recalculated from zero during next analysis).

If  you  want  to  perform   incremental/real-time  SSA,  consider   using
following functions:
* ssaappendpointandupdate() for appending one point
* ssaappendsequenceandupdate() for appending new sequence
   
INPUT PARAMETERS:
    S               -   SSA model created with ssacreate()
    X               -   array[N], data, can be larger (additional values
                        are ignored)
    N               -   data length, can be automatically determined from
                        the array length. N>=0.

OUTPUT PARAMETERS:
    S               -   SSA model, updated
    
NOTE: you can clear dataset with ssacleardata()

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
void ssaaddsequence(ssamodel* s,
     /* Real    */ const ae_vector* x,
     ae_int_t n,
     ae_state *_state);


/*************************************************************************
This function appends single point to last data sequence stored in the SSA
model and tries to update model in the  incremental  manner  (if  possible
with current algorithm).

If you want to add more than one point at once:
* if you want to add M points to the same sequence, perform M-1 calls with
  UpdateIts parameter set to 0.0, and last call with non-zero UpdateIts.
* if you want to add new sequence, use ssaappendsequenceandupdate()

Running time of this function does NOT depend on  dataset  size,  only  on
window width and number of singular vectors. Depending on algorithm  being
used, incremental update has complexity:
* for top-K real time   - O(UpdateIts*K*Width^2), with fractional UpdateIts
* for top-K direct      - O(Width^3) for any non-zero UpdateIts
* for precomputed basis - O(1), no update is performed
   
INPUT PARAMETERS:
    S               -   SSA model created with ssacreate()
    X               -   new point
    UpdateIts       -   >=0,  floating  point (!)  value,  desired  update
                        frequency:
                        * zero value means that point is  stored,  but  no
                          update is performed
                        * integer part of the value means  that  specified
                          number of iterations is always performed
                        * fractional part of  the  value  means  that  one
                          iteration is performed with this probability.
                          
                        Recommended value: 0<UpdateIts<=1.  Values  larger
                        than 1 are VERY seldom  needed.  If  your  dataset
                        changes slowly, you can set it  to  0.1  and  skip
                        90% of updates.
                        
                        In any case, no information is lost even with zero
                        value of UpdateIts! It will be  incorporated  into
                        model, sooner or later.

OUTPUT PARAMETERS:
    S               -   SSA model, updated

NOTE: this function uses internal  RNG  to  handle  fractional  values  of
      UpdateIts. By default it  is  initialized  with  fixed  seed  during
      initial calculation of basis. Thus subsequent calls to this function
      will result in the same sequence of pseudorandom decisions.
      
      However, if  you  have  several  SSA  models  which  are  calculated
      simultaneously, and if you want to reduce computational  bottlenecks
      by performing random updates at random moments, then fixed  seed  is
      not an option - all updates will fire at same moments.
      
      You may change it with ssasetseed() function.
      
NOTE: this function throws an exception if called for empty dataset (there
      is no "last" sequence to modify).

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
void ssaappendpointandupdate(ssamodel* s,
     double x,
     double updateits,
     ae_state *_state);


/*************************************************************************
This function appends new sequence to dataset stored in the SSA  model and
tries to update model in the incremental manner (if possible  with current
algorithm).

Notes:
* if you want to add M sequences at once, perform M-1 calls with UpdateIts
  parameter set to 0.0, and last call with non-zero UpdateIts.
* if you want to add just one point, use ssaappendpointandupdate()

Running time of this function does NOT depend on  dataset  size,  only  on
sequence length, window width and number of singular vectors. Depending on
algorithm being used, incremental update has complexity:
* for top-K real time   - O(UpdateIts*K*Width^2+(NTicks-Width)*Width^2)
* for top-K direct      - O(Width^3+(NTicks-Width)*Width^2)
* for precomputed basis - O(1), no update is performed
   
INPUT PARAMETERS:
    S               -   SSA model created with ssacreate()
    X               -   new sequence, array[NTicks] or larget
    NTicks          -   >=1, number of ticks in the sequence
    UpdateIts       -   >=0,  floating  point (!)  value,  desired  update
                        frequency:
                        * zero value means that point is  stored,  but  no
                          update is performed
                        * integer part of the value means  that  specified
                          number of iterations is always performed
                        * fractional part of  the  value  means  that  one
                          iteration is performed with this probability.
                          
                        Recommended value: 0<UpdateIts<=1.  Values  larger
                        than 1 are VERY seldom  needed.  If  your  dataset
                        changes slowly, you can set it  to  0.1  and  skip
                        90% of updates.
                        
                        In any case, no information is lost even with zero
                        value of UpdateIts! It will be  incorporated  into
                        model, sooner or later.

OUTPUT PARAMETERS:
    S               -   SSA model, updated

NOTE: this function uses internal  RNG  to  handle  fractional  values  of
      UpdateIts. By default it  is  initialized  with  fixed  seed  during
      initial calculation of basis. Thus subsequent calls to this function
      will result in the same sequence of pseudorandom decisions.
      
      However, if  you  have  several  SSA  models  which  are  calculated
      simultaneously, and if you want to reduce computational  bottlenecks
      by performing random updates at random moments, then fixed  seed  is
      not an option - all updates will fire at same moments.
      
      You may change it with ssasetseed() function.

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
void ssaappendsequenceandupdate(ssamodel* s,
     /* Real    */ const ae_vector* x,
     ae_int_t nticks,
     double updateits,
     ae_state *_state);


/*************************************************************************
This  function sets SSA algorithm to "precomputed vectors" algorithm.

This  algorithm  uses  precomputed  set  of  orthonormal  (orthogonal  AND
normalized) basis vectors supplied by user. Thus, basis calculation  phase
is not performed -  we  already  have  our  basis  -  and  only  analysis/
forecasting phase requires actual calculations.
      
This algorithm may handle "append" requests which add just  one/few  ticks
to the end of the last sequence in O(1) time.

NOTE: this algorithm accepts both basis and window  width,  because  these
      two parameters are naturally aligned.  Calling  this  function  sets
      window width; if you call ssasetwindow() with  other  window  width,
      then during analysis stage algorithm will detect conflict and  reset
      to zero basis.

INPUT PARAMETERS:
    S               -   SSA model
    A               -   array[WindowWidth,NBasis], orthonormalized  basis;
                        this function does NOT control  orthogonality  and
                        does NOT perform any kind of  renormalization.  It
                        is your responsibility to provide it with  correct
                        basis.
    WindowWidth     -   window width, >=1
    NBasis          -   number of basis vectors, 1<=NBasis<=WindowWidth

OUTPUT PARAMETERS:
    S               -   updated model

NOTE: calling this function invalidates basis in all cases.

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
void ssasetalgoprecomputed(ssamodel* s,
     /* Real    */ const ae_matrix* a,
     ae_int_t windowwidth,
     ae_int_t nbasis,
     ae_state *_state);


/*************************************************************************
This  function sets SSA algorithm to "direct top-K" algorithm.

"Direct top-K" algorithm performs full  SVD  of  the  N*WINDOW  trajectory
matrix (hence its name - direct solver  is  used),  then  extracts  top  K
components. Overall running time is O(N*WINDOW^2), where N is a number  of
ticks in the dataset, WINDOW is window width.

This algorithm may handle "append" requests which add just  one/few  ticks
to the end of the last sequence in O(WINDOW^3) time,  which  is  ~N/WINDOW
times faster than re-computing everything from scratch.

INPUT PARAMETERS:
    S               -   SSA model
    TopK            -   number of components to analyze; TopK>=1.

OUTPUT PARAMETERS:
    S               -   updated model


NOTE: TopK>WindowWidth is silently decreased to WindowWidth during analysis
      phase
      
NOTE: calling this function invalidates basis, except  for  the  situation
      when this algorithm was already set with same parameters.

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
void ssasetalgotopkdirect(ssamodel* s, ae_int_t topk, ae_state *_state);


/*************************************************************************
This function sets SSA algorithm to "top-K real time algorithm". This algo
extracts K components with largest singular values.

It  is  real-time  version  of  top-K  algorithm  which  is  optimized for
incremental processing and  fast  start-up. Internally  it  uses  subspace
eigensolver for truncated SVD. It results  in  ability  to  perform  quick
updates of the basis when only a few points/sequences is added to dataset.

Performance profile of the algorithm is given below:
* O(K*WindowWidth^2) running time for incremental update  of  the  dataset
  with one of the "append-and-update" functions (ssaappendpointandupdate()
  or ssaappendsequenceandupdate()).
* O(N*WindowWidth^2) running time for initial basis evaluation (N=size  of
  dataset)
* ability  to  split  costly  initialization  across  several  incremental
  updates of the basis (so called "Power-Up" functionality,  activated  by
  ssasetpoweruplength() function)
  
INPUT PARAMETERS:
    S               -   SSA model
    TopK            -   number of components to analyze; TopK>=1.

OUTPUT PARAMETERS:
    S               -   updated model

NOTE: this  algorithm  is  optimized  for  large-scale  tasks  with  large
      datasets. On toy problems with just  5-10 points it can return basis
      which is slightly different from that returned by  direct  algorithm
      (ssasetalgotopkdirect() function). However, the  difference  becomes
      negligible as dataset grows.

NOTE: TopK>WindowWidth is silently decreased to WindowWidth during analysis
      phase
      
NOTE: calling this function invalidates basis, except  for  the  situation
      when this algorithm was already set with same parameters.

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
void ssasetalgotopkrealtime(ssamodel* s, ae_int_t topk, ae_state *_state);


/*************************************************************************
This function clears all data stored in the  model  and  invalidates  all
basis components found so far.
   
INPUT PARAMETERS:
    S               -   SSA model created with ssacreate()

OUTPUT PARAMETERS:
    S               -   SSA model, updated

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
void ssacleardata(ssamodel* s, ae_state *_state);


/*************************************************************************
This function executes SSA on internally stored dataset and returns  basis
found by current method.
   
INPUT PARAMETERS:
    S               -   SSA model

OUTPUT PARAMETERS:
    A               -   array[WindowWidth,NBasis],   basis;  vectors  are
                        stored in matrix columns, by descreasing variance
    SV              -   array[NBasis]:
                        * zeros - for model initialized with SSASetAlgoPrecomputed()
                        * singular values - for other algorithms
    WindowWidth     -   current window
    NBasis          -   basis size
    

CACHING/REUSE OF THE BASIS

Caching/reuse of previous results is performed:
* first call performs full run of SSA; basis is stored in the cache
* subsequent calls reuse previously cached basis
* if you call any function which changes model properties (window  length,
  algorithm, dataset), internal basis will be invalidated.
* the only calls which do NOT invalidate basis are listed below:
  a) ssasetwindow() with same window length
  b) ssaappendpointandupdate()
  c) ssaappendsequenceandupdate()
  d) ssasetalgotopk...() with exactly same K
  Calling these functions will result in reuse of previously found basis.
  
  
HANDLING OF DEGENERATE CASES
    
Calling  this  function  in  degenerate  cases  (no  data  or all data are
shorter than window size; no algorithm is specified)  returns  basis  with
just one zero vector.

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
void ssagetbasis(ssamodel* s,
     /* Real    */ ae_matrix* a,
     /* Real    */ ae_vector* sv,
     ae_int_t* windowwidth,
     ae_int_t* nbasis,
     ae_state *_state);


/*************************************************************************
This function returns linear recurrence relation (LRR) coefficients  found
by current SSA algorithm.
   
INPUT PARAMETERS:
    S               -   SSA model

OUTPUT PARAMETERS:
    A               -   array[WindowWidth-1]. Coefficients  of  the
                        linear recurrence of the form:
                        X[W-1] = X[W-2]*A[W-2] + X[W-3]*A[W-3] + ... + X[0]*A[0].
                        Empty array for WindowWidth=1.
    WindowWidth     -   current window width
    

CACHING/REUSE OF THE BASIS

Caching/reuse of previous results is performed:
* first call performs full run of SSA; basis is stored in the cache
* subsequent calls reuse previously cached basis
* if you call any function which changes model properties (window  length,
  algorithm, dataset), internal basis will be invalidated.
* the only calls which do NOT invalidate basis are listed below:
  a) ssasetwindow() with same window length
  b) ssaappendpointandupdate()
  c) ssaappendsequenceandupdate()
  d) ssasetalgotopk...() with exactly same K
  Calling these functions will result in reuse of previously found basis.
  
  
HANDLING OF DEGENERATE CASES
    
Calling  this  function  in  degenerate  cases  (no  data  or all data are
shorter than window size; no algorithm is specified) returns zeros.

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
void ssagetlrr(ssamodel* s,
     /* Real    */ ae_vector* a,
     ae_int_t* windowwidth,
     ae_state *_state);


/*************************************************************************
This  function  executes  SSA  on  internally  stored  dataset and returns
analysis  for  the  last  window  of  the  last sequence. Such analysis is
an lightweight alternative for full scale reconstruction (see below).

Typical use case for this function is  real-time  setting,  when  you  are
interested in quick-and-dirty (very quick and very  dirty)  processing  of
just a few last ticks of the trend.

IMPORTANT: full  scale  SSA  involves  analysis  of  the  ENTIRE  dataset,
           with reconstruction being done for  all  positions  of  sliding
           window with subsequent hankelization  (diagonal  averaging)  of
           the resulting matrix.
           
           Such analysis requires O((DataLen-Window)*Window*NBasis)  FLOPs
           and can be quite costly. However, it has  nice  noise-canceling
           effects due to averaging.
           
           This function performs REDUCED analysis of the last window.  It
           is much faster - just O(Window*NBasis),  but  its  results  are
           DIFFERENT from that of ssaanalyzelast(). In  particular,  first
           few points of the trend are much more prone to noise.
   
INPUT PARAMETERS:
    S               -   SSA model

OUTPUT PARAMETERS:
    Trend           -   array[WindowSize], reconstructed trend line
    Noise           -   array[WindowSize], the rest of the signal;
                        it holds that ActualData = Trend+Noise.
    NTicks          -   current WindowSize
    

CACHING/REUSE OF THE BASIS

Caching/reuse of previous results is performed:
* first call performs full run of SSA; basis is stored in the cache
* subsequent calls reuse previously cached basis
* if you call any function which changes model properties (window  length,
  algorithm, dataset), internal basis will be invalidated.
* the only calls which do NOT invalidate basis are listed below:
  a) ssasetwindow() with same window length
  b) ssaappendpointandupdate()
  c) ssaappendsequenceandupdate()
  d) ssasetalgotopk...() with exactly same K
  Calling these functions will result in reuse of previously found basis.

In  any  case,  only  basis  is  reused. Reconstruction is performed  from
scratch every time you call this function.
  
  
HANDLING OF DEGENERATE CASES

Following degenerate cases may happen:
* dataset is empty (no analysis can be done)
* all sequences are shorter than the window length,no analysis can be done
* no algorithm is specified (no analysis can be done)
* last sequence is shorter than the window length (analysis can  be  done,
  but we can not perform reconstruction on the last sequence)
    
Calling this function in degenerate cases returns following result:
* in any case, WindowWidth ticks is returned
* trend is assumed to be zero
* noise is initialized by the last sequence; if last sequence  is  shorter
  than the window size, it is moved to  the  end  of  the  array, and  the
  beginning of the noise array is filled by zeros
  
No analysis is performed in degenerate cases (we immediately return  dummy
values, no basis is constructed).

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
void ssaanalyzelastwindow(ssamodel* s,
     /* Real    */ ae_vector* trend,
     /* Real    */ ae_vector* noise,
     ae_int_t* nticks,
     ae_state *_state);


/*************************************************************************
This function:
* builds SSA basis using internally stored (entire) dataset
* returns reconstruction for the last NTicks of the last sequence

If you want to analyze some other sequence, use ssaanalyzesequence().

Reconstruction phase involves  generation  of  NTicks-WindowWidth  sliding
windows, their decomposition using empirical orthogonal functions found by
SSA, followed by averaging of each data point across  several  overlapping
windows. Thus, every point in the output trend is reconstructed  using  up
to WindowWidth overlapping  windows  (WindowWidth windows exactly  in  the
inner points, just one window at the extremal points).

IMPORTANT: due to averaging this function returns  different  results  for
           different values of NTicks. It is expected and not a bug.
           
           For example:
           * Trend[NTicks-1] is always same because it is not averaged  in
             any case (same applies to Trend[0]).
           * Trend[NTicks-2] has different values  for  NTicks=WindowWidth
             and NTicks=WindowWidth+1 because former  case  means that  no
             averaging is performed, and latter  case means that averaging
             using two sliding windows  is  performed.  Larger  values  of
             NTicks produce same results as NTicks=WindowWidth+1.
           * ...and so on...

PERFORMANCE: this  function has O((NTicks-WindowWidth)*WindowWidth*NBasis)
             running time. If you work  in  time-constrained  setting  and
             have to analyze just a few last ticks, choosing NTicks  equal
             to WindowWidth+SmoothingLen, with SmoothingLen=1...WindowWidth
             will result in good compromise between noise cancellation and
             analysis speed.

INPUT PARAMETERS:
    S               -   SSA model
    NTicks          -   number of ticks to analyze, Nticks>=1.
                        * special case of NTicks<=WindowWidth  is  handled
                          by analyzing last window and  returning   NTicks 
                          last ticks.
                        * special case NTicks>LastSequenceLen  is  handled
                          by prepending result with NTicks-LastSequenceLen
                          zeros.

OUTPUT PARAMETERS:
    Trend           -   array[NTicks], reconstructed trend line
    Noise           -   array[NTicks], the rest of the signal;
                        it holds that ActualData = Trend+Noise.
    

CACHING/REUSE OF THE BASIS

Caching/reuse of previous results is performed:
* first call performs full run of SSA; basis is stored in the cache
* subsequent calls reuse previously cached basis
* if you call any function which changes model properties (window  length,
  algorithm, dataset), internal basis will be invalidated.
* the only calls which do NOT invalidate basis are listed below:
  a) ssasetwindow() with same window length
  b) ssaappendpointandupdate()
  c) ssaappendsequenceandupdate()
  d) ssasetalgotopk...() with exactly same K
  Calling these functions will result in reuse of previously found basis.

In  any  case,  only  basis  is  reused. Reconstruction is performed  from
scratch every time you call this function.
  
  
HANDLING OF DEGENERATE CASES

Following degenerate cases may happen:
* dataset is empty (no analysis can be done)
* all sequences are shorter than the window length,no analysis can be done
* no algorithm is specified (no analysis can be done)
* last sequence is shorter than the window length (analysis  can  be done,
  but we can not perform reconstruction on the last sequence)
    
Calling this function in degenerate cases returns following result:
* in any case, NTicks ticks is returned
* trend is assumed to be zero
* noise is initialized by the last sequence; if last sequence  is  shorter
  than the window size, it is moved to  the  end  of  the  array, and  the
  beginning of the noise array is filled by zeros
  
No analysis is performed in degenerate cases (we immediately return  dummy
values, no basis is constructed).

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
void ssaanalyzelast(ssamodel* s,
     ae_int_t nticks,
     /* Real    */ ae_vector* trend,
     /* Real    */ ae_vector* noise,
     ae_state *_state);


/*************************************************************************
This function:
* builds SSA basis using internally stored (entire) dataset
* returns reconstruction for the sequence being passed to this function

If  you  want  to  analyze  last  sequence  stored  in   the   model,  use
ssaanalyzelast().

Reconstruction phase involves  generation  of  NTicks-WindowWidth  sliding
windows, their decomposition using empirical orthogonal functions found by
SSA, followed by averaging of each data point across  several  overlapping
windows. Thus, every point in the output trend is reconstructed  using  up
to WindowWidth overlapping  windows  (WindowWidth windows exactly  in  the
inner points, just one window at the extremal points).

PERFORMANCE: this  function has O((NTicks-WindowWidth)*WindowWidth*NBasis)
             running time. If you work  in  time-constrained  setting  and
             have to analyze just a few last ticks, choosing NTicks  equal
             to WindowWidth+SmoothingLen, with SmoothingLen=1...WindowWidth
             will result in good compromise between noise cancellation and
             analysis speed.

INPUT PARAMETERS:
    S               -   SSA model
    Data            -   array[NTicks], can be larger (only NTicks  leading
                        elements will be used)
    NTicks          -   number of ticks to analyze, Nticks>=1.
                        * special case of NTicks<WindowWidth  is   handled
                          by returning zeros as trend, and signal as noise

OUTPUT PARAMETERS:
    Trend           -   array[NTicks], reconstructed trend line
    Noise           -   array[NTicks], the rest of the signal;
                        it holds that ActualData = Trend+Noise.
    

CACHING/REUSE OF THE BASIS

Caching/reuse of previous results is performed:
* first call performs full run of SSA; basis is stored in the cache
* subsequent calls reuse previously cached basis
* if you call any function which changes model properties (window  length,
  algorithm, dataset), internal basis will be invalidated.
* the only calls which do NOT invalidate basis are listed below:
  a) ssasetwindow() with same window length
  b) ssaappendpointandupdate()
  c) ssaappendsequenceandupdate()
  d) ssasetalgotopk...() with exactly same K
  Calling these functions will result in reuse of previously found basis.

In  any  case,  only  basis  is  reused. Reconstruction is performed  from
scratch every time you call this function.
  
  
HANDLING OF DEGENERATE CASES

Following degenerate cases may happen:
* dataset is empty (no analysis can be done)
* all sequences are shorter than the window length,no analysis can be done
* no algorithm is specified (no analysis can be done)
* sequence being passed is shorter than the window length
    
Calling this function in degenerate cases returns following result:
* in any case, NTicks ticks is returned
* trend is assumed to be zero
* noise is initialized by the sequence.
  
No analysis is performed in degenerate cases (we immediately return  dummy
values, no basis is constructed).

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
void ssaanalyzesequence(ssamodel* s,
     /* Real    */ const ae_vector* data,
     ae_int_t nticks,
     /* Real    */ ae_vector* trend,
     /* Real    */ ae_vector* noise,
     ae_state *_state);


/*************************************************************************
This function builds SSA basis and performs forecasting  for  a  specified
number of ticks, returning value of trend.

Forecast is performed as follows:
* SSA  trend  extraction  is  applied  to last WindowWidth elements of the
  internally stored dataset; this step is basically a noise reduction.
* linear recurrence relation is applied to extracted trend

This function has following running time:
* O(NBasis*WindowWidth) for trend extraction phase (always performed)
* O(WindowWidth*NTicks) for forecast phase

NOTE: noise reduction is ALWAYS applied by this algorithm; if you want  to
      apply recurrence relation  to  raw  unprocessed  data,  use  another
      function - ssaforecastsequence() which allows to  turn  on  and  off
      noise reduction phase.
      
NOTE: this algorithm performs prediction using only one - last  -  sliding
      window.  Predictions  produced   by   such   approach   are   smooth
      continuations of the reconstructed  trend  line,  but  they  can  be
      easily corrupted by noise. If you need  noise-resistant  prediction,
      use ssaforecastavglast() function, which averages predictions  built
      using several sliding windows.

INPUT PARAMETERS:
    S               -   SSA model
    NTicks          -   number of ticks to forecast, NTicks>=1

OUTPUT PARAMETERS:
    Trend           -   array[NTicks], predicted trend line
    

CACHING/REUSE OF THE BASIS

Caching/reuse of previous results is performed:
* first call performs full run of SSA; basis is stored in the cache
* subsequent calls reuse previously cached basis
* if you call any function which changes model properties (window  length,
  algorithm, dataset), internal basis will be invalidated.
* the only calls which do NOT invalidate basis are listed below:
  a) ssasetwindow() with same window length
  b) ssaappendpointandupdate()
  c) ssaappendsequenceandupdate()
  d) ssasetalgotopk...() with exactly same K
  Calling these functions will result in reuse of previously found basis.


HANDLING OF DEGENERATE CASES

Following degenerate cases may happen:
* dataset is empty (no analysis can be done)
* all sequences are shorter than the window length,no analysis can be done
* no algorithm is specified (no analysis can be done)
* last sequence is shorter than the WindowWidth   (analysis  can  be done,
  but we can not perform forecasting on the last sequence)
* window lentgh is 1 (impossible to use for forecasting)
* SSA analysis algorithm is  configured  to  extract  basis  whose size is
  equal to window length (impossible to use for  forecasting;  only  basis
  whose size is less than window length can be used).
    
Calling this function in degenerate cases returns following result:
* NTicks  copies  of  the  last  value is returned for non-empty task with
  large enough dataset, but with overcomplete  basis  (window  width=1  or
  basis size is equal to window width)
* zero trend with length=NTicks is returned for empty task
  
No analysis is performed in degenerate cases (we immediately return  dummy
values, no basis is ever constructed).

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
void ssaforecastlast(ssamodel* s,
     ae_int_t nticks,
     /* Real    */ ae_vector* trend,
     ae_state *_state);


/*************************************************************************
This function builds SSA  basis  and  performs  forecasting  for  a  user-
specified sequence, returning value of trend.

Forecasting is done in two stages:
* first,  we  extract  trend  from the WindowWidth  last  elements of  the
  sequence. This stage is optional, you  can  turn  it  off  if  you  pass
  data which are already processed with SSA. Of course, you  can  turn  it
  off even for raw data, but it is not recommended - noise suppression  is
  very important for correct prediction.
* then, we apply LRR for last  WindowWidth-1  elements  of  the  extracted
  trend.

This function has following running time:
* O(NBasis*WindowWidth) for trend extraction phase
* O(WindowWidth*NTicks) for forecast phase
      
NOTE: this algorithm performs prediction using only one - last  -  sliding
      window.  Predictions  produced   by   such   approach   are   smooth
      continuations of the reconstructed  trend  line,  but  they  can  be
      easily corrupted by noise. If you need  noise-resistant  prediction,
      use ssaforecastavgsequence() function,  which  averages  predictions
      built using several sliding windows.

INPUT PARAMETERS:
    S               -   SSA model
    Data            -   array[NTicks], data to forecast
    DataLen         -   number of ticks in the data, DataLen>=1
    ForecastLen     -   number of ticks to predict, ForecastLen>=1
    ApplySmoothing  -   whether to apply smoothing trend extraction or not;
                        if you do not know what to specify, pass True.

OUTPUT PARAMETERS:
    Trend           -   array[ForecastLen], forecasted trend
    

CACHING/REUSE OF THE BASIS

Caching/reuse of previous results is performed:
* first call performs full run of SSA; basis is stored in the cache
* subsequent calls reuse previously cached basis
* if you call any function which changes model properties (window  length,
  algorithm, dataset), internal basis will be invalidated.
* the only calls which do NOT invalidate basis are listed below:
  a) ssasetwindow() with same window length
  b) ssaappendpointandupdate()
  c) ssaappendsequenceandupdate()
  d) ssasetalgotopk...() with exactly same K
  Calling these functions will result in reuse of previously found basis.


HANDLING OF DEGENERATE CASES

Following degenerate cases may happen:
* dataset is empty (no analysis can be done)
* all sequences are shorter than the window length,no analysis can be done
* no algorithm is specified (no analysis can be done)
* data sequence is shorter than the WindowWidth   (analysis  can  be done,
  but we can not perform forecasting on the last sequence)
* window lentgh is 1 (impossible to use for forecasting)
* SSA analysis algorithm is  configured  to  extract  basis  whose size is
  equal to window length (impossible to use for  forecasting;  only  basis
  whose size is less than window length can be used).
    
Calling this function in degenerate cases returns following result:
* ForecastLen copies of the last value is returned for non-empty task with
  large enough dataset, but with overcomplete  basis  (window  width=1  or
  basis size is equal to window width)
* zero trend with length=ForecastLen is returned for empty task
  
No analysis is performed in degenerate cases (we immediately return  dummy
values, no basis is ever constructed).

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
void ssaforecastsequence(ssamodel* s,
     /* Real    */ const ae_vector* data,
     ae_int_t datalen,
     ae_int_t forecastlen,
     ae_bool applysmoothing,
     /* Real    */ ae_vector* trend,
     ae_state *_state);


/*************************************************************************
This function builds SSA basis and performs forecasting  for  a  specified
number of ticks, returning value of trend.

Forecast is performed as follows:
* SSA  trend  extraction  is  applied to last  M  sliding windows  of  the
  internally stored dataset
* for each of M sliding windows, M predictions are built
* average value of M predictions is returned

This function has following running time:
* O(NBasis*WindowWidth*M) for trend extraction phase (always performed)
* O(WindowWidth*NTicks*M) for forecast phase

NOTE: noise reduction is ALWAYS applied by this algorithm; if you want  to
      apply recurrence relation  to  raw  unprocessed  data,  use  another
      function - ssaforecastsequence() which allows to  turn  on  and  off
      noise reduction phase.
      
NOTE: combination of several predictions results in lesser sensitivity  to
      noise, but it may produce undesirable discontinuities  between  last
      point of the trend and first point of the prediction. The reason  is
      that  last  point  of  the  trend is usually corrupted by noise, but
      average  value of  several  predictions  is less sensitive to noise,
      thus discontinuity appears. It is not a bug.

INPUT PARAMETERS:
    S               -   SSA model
    M               -   number  of  sliding  windows  to combine, M>=1. If
                        your dataset has less than M sliding windows, this
                        parameter will be silently reduced.
    NTicks          -   number of ticks to forecast, NTicks>=1

OUTPUT PARAMETERS:
    Trend           -   array[NTicks], predicted trend line
    

CACHING/REUSE OF THE BASIS

Caching/reuse of previous results is performed:
* first call performs full run of SSA; basis is stored in the cache
* subsequent calls reuse previously cached basis
* if you call any function which changes model properties (window  length,
  algorithm, dataset), internal basis will be invalidated.
* the only calls which do NOT invalidate basis are listed below:
  a) ssasetwindow() with same window length
  b) ssaappendpointandupdate()
  c) ssaappendsequenceandupdate()
  d) ssasetalgotopk...() with exactly same K
  Calling these functions will result in reuse of previously found basis.


HANDLING OF DEGENERATE CASES

Following degenerate cases may happen:
* dataset is empty (no analysis can be done)
* all sequences are shorter than the window length,no analysis can be done
* no algorithm is specified (no analysis can be done)
* last sequence is shorter than the WindowWidth   (analysis  can  be done,
  but we can not perform forecasting on the last sequence)
* window lentgh is 1 (impossible to use for forecasting)
* SSA analysis algorithm is  configured  to  extract  basis  whose size is
  equal to window length (impossible to use for  forecasting;  only  basis
  whose size is less than window length can be used).
    
Calling this function in degenerate cases returns following result:
* NTicks  copies  of  the  last  value is returned for non-empty task with
  large enough dataset, but with overcomplete  basis  (window  width=1  or
  basis size is equal to window width)
* zero trend with length=NTicks is returned for empty task
  
No analysis is performed in degenerate cases (we immediately return  dummy
values, no basis is ever constructed).

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
void ssaforecastavglast(ssamodel* s,
     ae_int_t m,
     ae_int_t nticks,
     /* Real    */ ae_vector* trend,
     ae_state *_state);


/*************************************************************************
This function builds SSA  basis  and  performs  forecasting  for  a  user-
specified sequence, returning value of trend.

Forecasting is done in two stages:
* first,  we  extract  trend  from M last sliding windows of the sequence.
  This stage is optional, you can  turn  it  off  if  you  pass data which
  are already processed with SSA. Of course, you  can  turn  it  off  even
  for raw data, but it is not recommended  -  noise  suppression  is  very
  important for correct prediction.
* then, we apply LRR independently for M sliding windows
* average of M predictions is returned

This function has following running time:
* O(NBasis*WindowWidth*M) for trend extraction phase
* O(WindowWidth*NTicks*M) for forecast phase
      
NOTE: combination of several predictions results in lesser sensitivity  to
      noise, but it may produce undesirable discontinuities  between  last
      point of the trend and first point of the prediction. The reason  is
      that  last  point  of  the  trend is usually corrupted by noise, but
      average  value of  several  predictions  is less sensitive to noise,
      thus discontinuity appears. It is not a bug.

INPUT PARAMETERS:
    S               -   SSA model
    Data            -   array[NTicks], data to forecast
    DataLen         -   number of ticks in the data, DataLen>=1
    M               -   number  of  sliding  windows  to combine, M>=1. If
                        your dataset has less than M sliding windows, this
                        parameter will be silently reduced.
    ForecastLen     -   number of ticks to predict, ForecastLen>=1
    ApplySmoothing  -   whether to apply smoothing trend extraction or not.
                        if you do not know what to specify, pass true.

OUTPUT PARAMETERS:
    Trend           -   array[ForecastLen], forecasted trend
    

CACHING/REUSE OF THE BASIS

Caching/reuse of previous results is performed:
* first call performs full run of SSA; basis is stored in the cache
* subsequent calls reuse previously cached basis
* if you call any function which changes model properties (window  length,
  algorithm, dataset), internal basis will be invalidated.
* the only calls which do NOT invalidate basis are listed below:
  a) ssasetwindow() with same window length
  b) ssaappendpointandupdate()
  c) ssaappendsequenceandupdate()
  d) ssasetalgotopk...() with exactly same K
  Calling these functions will result in reuse of previously found basis.


HANDLING OF DEGENERATE CASES

Following degenerate cases may happen:
* dataset is empty (no analysis can be done)
* all sequences are shorter than the window length,no analysis can be done
* no algorithm is specified (no analysis can be done)
* data sequence is shorter than the WindowWidth   (analysis  can  be done,
  but we can not perform forecasting on the last sequence)
* window lentgh is 1 (impossible to use for forecasting)
* SSA analysis algorithm is  configured  to  extract  basis  whose size is
  equal to window length (impossible to use for  forecasting;  only  basis
  whose size is less than window length can be used).
    
Calling this function in degenerate cases returns following result:
* ForecastLen copies of the last value is returned for non-empty task with
  large enough dataset, but with overcomplete  basis  (window  width=1  or
  basis size is equal to window width)
* zero trend with length=ForecastLen is returned for empty task
  
No analysis is performed in degenerate cases (we immediately return  dummy
values, no basis is ever constructed).

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
void ssaforecastavgsequence(ssamodel* s,
     /* Real    */ const ae_vector* data,
     ae_int_t datalen,
     ae_int_t m,
     ae_int_t forecastlen,
     ae_bool applysmoothing,
     /* Real    */ ae_vector* trend,
     ae_state *_state);
void _ssamodel_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _ssamodel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _ssamodel_clear(void* _p);
void _ssamodel_destroy(void* _p);


/*$ End $*/
#endif

