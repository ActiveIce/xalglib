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
#include "ssa.h"


/*$ Declarations $*/
static ae_bool ssa_hassomethingtoanalyze(const ssamodel* s,
     ae_state *_state);
static ae_bool ssa_issequencebigenough(const ssamodel* s,
     ae_int_t i,
     ae_state *_state);
static void ssa_updatebasis(ssamodel* s,
     ae_int_t appendlen,
     double updateits,
     ae_state *_state);
static void ssa_analyzesequence(ssamodel* s,
     /* Real    */ const ae_vector* data,
     ae_int_t i0,
     ae_int_t i1,
     /* Real    */ ae_vector* trend,
     /* Real    */ ae_vector* noise,
     ae_int_t offs,
     ae_state *_state);
static void ssa_forecastavgsequence(ssamodel* s,
     /* Real    */ const ae_vector* data,
     ae_int_t i0,
     ae_int_t i1,
     ae_int_t m,
     ae_int_t forecastlen,
     ae_bool smooth,
     /* Real    */ ae_vector* trend,
     ae_int_t offs,
     ae_state *_state);
static void ssa_realtimedequeue(ssamodel* s,
     double beta,
     ae_int_t cnt,
     ae_state *_state);
static void ssa_updatexxtprepare(ssamodel* s,
     ae_int_t updatesize,
     ae_int_t windowwidth,
     ae_int_t memorylimit,
     ae_state *_state);
static void ssa_updatexxtsend(ssamodel* s,
     /* Real    */ const ae_vector* u,
     ae_int_t i0,
     /* Real    */ ae_matrix* xxt,
     ae_state *_state);
static void ssa_updatexxtfinalize(ssamodel* s,
     /* Real    */ ae_matrix* xxt,
     ae_state *_state);


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
void ssacreate(ssamodel* s, ae_state *_state)
{

    _ssamodel_clear(s);

    
    /*
     * Model data, algorithms and settings
     */
    s->nsequences = 0;
    ae_vector_set_length(&s->sequenceidx, 1, _state);
    s->sequenceidx.ptr.p_int[0] = 0;
    s->algotype = 0;
    s->windowwidth = 1;
    s->rtpowerup = 1;
    s->arebasisandsolvervalid = ae_false;
    s->rngseed = 1;
    s->defaultsubspaceits = 10;
    s->memorylimit = 50000000;
    
    /*
     * Debug counters
     */
    s->dbgcntevd = 0;
}


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
void ssasetwindow(ssamodel* s, ae_int_t windowwidth, ae_state *_state)
{


    ae_assert(windowwidth>=1, "SSASetWindow: WindowWidth<1", _state);
    if( windowwidth==s->windowwidth )
    {
        return;
    }
    s->windowwidth = windowwidth;
    s->arebasisandsolvervalid = ae_false;
}


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
void ssasetseed(ssamodel* s, ae_int_t seed, ae_state *_state)
{


    s->rngseed = seed;
}


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
void ssasetpoweruplength(ssamodel* s, ae_int_t pwlen, ae_state *_state)
{


    ae_assert(pwlen>=0, "SSASetPowerUpLength: PWLen<0", _state);
    s->rtpowerup = ae_maxint(pwlen, 1, _state);
    s->arebasisandsolvervalid = ae_false;
}


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
void ssasetmemorylimit(ssamodel* s, ae_int_t memlimit, ae_state *_state)
{


    if( memlimit<0 )
    {
        memlimit = 0;
    }
    s->memorylimit = memlimit;
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t offs;


    ae_assert(n>=0, "SSAAddSequence: N<0", _state);
    ae_assert(x->cnt>=n, "SSAAddSequence: X is too short", _state);
    ae_assert(isfinitevector(x, n, _state), "SSAAddSequence: X contains infinities NANs", _state);
    
    /*
     * Invalidate model
     */
    s->arebasisandsolvervalid = ae_false;
    
    /*
     * Add sequence
     */
    ivectorgrowto(&s->sequenceidx, s->nsequences+2, _state);
    s->sequenceidx.ptr.p_int[s->nsequences+1] = s->sequenceidx.ptr.p_int[s->nsequences]+n;
    rvectorgrowto(&s->sequencedata, s->sequenceidx.ptr.p_int[s->nsequences+1], _state);
    offs = s->sequenceidx.ptr.p_int[s->nsequences];
    for(i=0; i<=n-1; i++)
    {
        s->sequencedata.ptr.p_double[offs+i] = x->ptr.p_double[i];
    }
    inc(&s->nsequences, _state);
}


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
     ae_state *_state)
{


    ae_assert(ae_isfinite(x, _state), "SSAAppendPointAndUpdate: X is not finite", _state);
    ae_assert(ae_isfinite(updateits, _state), "SSAAppendPointAndUpdate: UpdateIts is not finite", _state);
    ae_assert(ae_fp_greater_eq(updateits,(double)(0)), "SSAAppendPointAndUpdate: UpdateIts<0", _state);
    ae_assert(s->nsequences>0, "SSAAppendPointAndUpdate: dataset is empty, no sequence to modify", _state);
    
    /*
     * Append point to dataset
     */
    rvectorgrowto(&s->sequencedata, s->sequenceidx.ptr.p_int[s->nsequences]+1, _state);
    s->sequencedata.ptr.p_double[s->sequenceidx.ptr.p_int[s->nsequences]] = x;
    s->sequenceidx.ptr.p_int[s->nsequences] = s->sequenceidx.ptr.p_int[s->nsequences]+1;
    
    /*
     * Do we have something to analyze? If no, invalidate basis
     * (just to be sure) and exit.
     */
    if( !ssa_hassomethingtoanalyze(s, _state) )
    {
        s->arebasisandsolvervalid = ae_false;
        return;
    }
    
    /*
     * Well, we have data to analyze and algorithm set, but basis is
     * invalid. Let's calculate it from scratch and exit.
     */
    if( !s->arebasisandsolvervalid )
    {
        ssa_updatebasis(s, 0, 0.0, _state);
        return;
    }
    
    /*
     * Update already computed basis
     */
    ssa_updatebasis(s, 1, updateits, _state);
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t offs;


    ae_assert(nticks>=0, "SSAAppendSequenceAndUpdate: NTicks<0", _state);
    ae_assert(x->cnt>=nticks, "SSAAppendSequenceAndUpdate: X is too short", _state);
    ae_assert(isfinitevector(x, nticks, _state), "SSAAppendSequenceAndUpdate: X contains infinities NANs", _state);
    
    /*
     * Add sequence
     */
    ivectorgrowto(&s->sequenceidx, s->nsequences+2, _state);
    s->sequenceidx.ptr.p_int[s->nsequences+1] = s->sequenceidx.ptr.p_int[s->nsequences]+nticks;
    rvectorgrowto(&s->sequencedata, s->sequenceidx.ptr.p_int[s->nsequences+1], _state);
    offs = s->sequenceidx.ptr.p_int[s->nsequences];
    for(i=0; i<=nticks-1; i++)
    {
        s->sequencedata.ptr.p_double[offs+i] = x->ptr.p_double[i];
    }
    inc(&s->nsequences, _state);
    
    /*
     * Do we have something to analyze? If no, invalidate basis
     * (just to be sure) and exit.
     */
    if( !ssa_hassomethingtoanalyze(s, _state) )
    {
        s->arebasisandsolvervalid = ae_false;
        return;
    }
    
    /*
     * Well, we have data to analyze and algorithm set, but basis is
     * invalid. Let's calculate it from scratch and exit.
     */
    if( !s->arebasisandsolvervalid )
    {
        ssa_updatebasis(s, 0, 0.0, _state);
        return;
    }
    
    /*
     * Update already computed basis
     */
    if( nticks>=s->windowwidth )
    {
        ssa_updatebasis(s, nticks-s->windowwidth+1, updateits, _state);
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;


    ae_assert(windowwidth>=1, "SSASetAlgoPrecomputed: WindowWidth<1", _state);
    ae_assert(nbasis>=1, "SSASetAlgoPrecomputed: NBasis<1", _state);
    ae_assert(nbasis<=windowwidth, "SSASetAlgoPrecomputed: NBasis>WindowWidth", _state);
    ae_assert(a->rows>=windowwidth, "SSASetAlgoPrecomputed: Rows(A)<WindowWidth", _state);
    ae_assert(a->cols>=nbasis, "SSASetAlgoPrecomputed: Rows(A)<NBasis", _state);
    ae_assert(apservisfinitematrix(a, windowwidth, nbasis, _state), "SSASetAlgoPrecomputed: Rows(A)<NBasis", _state);
    s->algotype = 1;
    s->precomputedwidth = windowwidth;
    s->precomputednbasis = nbasis;
    s->windowwidth = windowwidth;
    rmatrixsetlengthatleast(&s->precomputedbasis, windowwidth, nbasis, _state);
    for(i=0; i<=windowwidth-1; i++)
    {
        for(j=0; j<=nbasis-1; j++)
        {
            s->precomputedbasis.ptr.pp_double[i][j] = a->ptr.pp_double[i][j];
        }
    }
    s->arebasisandsolvervalid = ae_false;
}


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
void ssasetalgotopkdirect(ssamodel* s, ae_int_t topk, ae_state *_state)
{


    ae_assert(topk>=1, "SSASetAlgoTopKDirect: TopK<1", _state);
    
    /*
     * Ignore calls which change nothing
     */
    if( s->algotype==2&&s->topk==topk )
    {
        return;
    }
    
    /*
     * Update settings, invalidate model
     */
    s->algotype = 2;
    s->topk = topk;
    s->arebasisandsolvervalid = ae_false;
}


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
void ssasetalgotopkrealtime(ssamodel* s, ae_int_t topk, ae_state *_state)
{


    ae_assert(topk>=1, "SSASetAlgoTopKRealTime: TopK<1", _state);
    
    /*
     * Ignore calls which change nothing
     */
    if( s->algotype==3&&s->topk==topk )
    {
        return;
    }
    
    /*
     * Update settings, invalidate model
     */
    s->algotype = 3;
    s->topk = topk;
    s->arebasisandsolvervalid = ae_false;
}


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
void ssacleardata(ssamodel* s, ae_state *_state)
{


    s->nsequences = 0;
    s->arebasisandsolvervalid = ae_false;
}


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
     ae_state *_state)
{
    ae_int_t i;

    ae_matrix_clear(a);
    ae_vector_clear(sv);
    *windowwidth = 0;
    *nbasis = 0;

    
    /*
     * Is it degenerate case?
     */
    if( !ssa_hassomethingtoanalyze(s, _state) )
    {
        *windowwidth = s->windowwidth;
        *nbasis = 1;
        ae_matrix_set_length(a, *windowwidth, 1, _state);
        for(i=0; i<=*windowwidth-1; i++)
        {
            a->ptr.pp_double[i][0] = 0.0;
        }
        ae_vector_set_length(sv, 1, _state);
        sv->ptr.p_double[0] = 0.0;
        return;
    }
    
    /*
     * Update basis.
     *
     * It will take care of basis validity flags. AppendLen=0 which means
     * that we perform initial basis evaluation.
     */
    ssa_updatebasis(s, 0, 0.0, _state);
    
    /*
     * Output
     */
    ae_assert(s->nbasis>0, "SSAGetBasis: integrity check failed", _state);
    ae_assert(s->windowwidth>0, "SSAGetBasis: integrity check failed", _state);
    *nbasis = s->nbasis;
    *windowwidth = s->windowwidth;
    ae_matrix_set_length(a, *windowwidth, *nbasis, _state);
    rmatrixcopy(*windowwidth, *nbasis, &s->basis, 0, 0, a, 0, 0, _state);
    ae_vector_set_length(sv, *nbasis, _state);
    for(i=0; i<=*nbasis-1; i++)
    {
        sv->ptr.p_double[i] = s->sv.ptr.p_double[i];
    }
}


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
     ae_state *_state)
{
    ae_int_t i;

    ae_vector_clear(a);
    *windowwidth = 0;

    ae_assert(s->windowwidth>0, "SSAGetLRR: integrity check failed", _state);
    
    /*
     * Is it degenerate case?
     */
    if( !ssa_hassomethingtoanalyze(s, _state) )
    {
        *windowwidth = s->windowwidth;
        ae_vector_set_length(a, *windowwidth-1, _state);
        for(i=0; i<=*windowwidth-2; i++)
        {
            a->ptr.p_double[i] = 0.0;
        }
        return;
    }
    
    /*
     * Update basis.
     *
     * It will take care of basis validity flags. AppendLen=0 which means
     * that we perform initial basis evaluation.
     */
    ssa_updatebasis(s, 0, 0.0, _state);
    
    /*
     * Output
     */
    *windowwidth = s->windowwidth;
    ae_vector_set_length(a, *windowwidth-1, _state);
    for(i=0; i<=*windowwidth-2; i++)
    {
        a->ptr.p_double[i] = s->forecasta.ptr.p_double[i];
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t offs;
    ae_int_t cnt;

    ae_vector_clear(trend);
    ae_vector_clear(noise);
    *nticks = 0;

    
    /*
     * Init
     */
    *nticks = s->windowwidth;
    ae_vector_set_length(trend, s->windowwidth, _state);
    ae_vector_set_length(noise, s->windowwidth, _state);
    
    /*
     * Is it degenerate case?
     */
    if( !ssa_hassomethingtoanalyze(s, _state)||!ssa_issequencebigenough(s, -1, _state) )
    {
        for(i=0; i<=*nticks-1; i++)
        {
            trend->ptr.p_double[i] = (double)(0);
            noise->ptr.p_double[i] = (double)(0);
        }
        if( s->nsequences>=1 )
        {
            cnt = ae_minint(s->sequenceidx.ptr.p_int[s->nsequences]-s->sequenceidx.ptr.p_int[s->nsequences-1], *nticks, _state);
            offs = s->sequenceidx.ptr.p_int[s->nsequences]-cnt;
            for(i=0; i<=cnt-1; i++)
            {
                noise->ptr.p_double[*nticks-cnt+i] = s->sequencedata.ptr.p_double[offs+i];
            }
        }
        return;
    }
    
    /*
     * Update basis.
     *
     * It will take care of basis validity flags. AppendLen=0 which means
     * that we perform initial basis evaluation.
     */
    ssa_updatebasis(s, 0, 0.0, _state);
    
    /*
     * Perform analysis of the last window
     */
    ae_assert(s->sequenceidx.ptr.p_int[s->nsequences]-s->windowwidth>=0, "SSAAnalyzeLastWindow: integrity check failed", _state);
    rvectorsetlengthatleast(&s->tmp0, s->nbasis, _state);
    rmatrixgemv(s->nbasis, s->windowwidth, 1.0, &s->basist, 0, 0, 0, &s->sequencedata, s->sequenceidx.ptr.p_int[s->nsequences]-s->windowwidth, 0.0, &s->tmp0, 0, _state);
    rmatrixgemv(s->windowwidth, s->nbasis, 1.0, &s->basis, 0, 0, 0, &s->tmp0, 0, 0.0, trend, 0, _state);
    offs = s->sequenceidx.ptr.p_int[s->nsequences]-s->windowwidth;
    cnt = s->windowwidth;
    for(i=0; i<=cnt-1; i++)
    {
        noise->ptr.p_double[i] = s->sequencedata.ptr.p_double[offs+i]-trend->ptr.p_double[i];
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t offs;
    ae_int_t cnt;
    ae_int_t cntzeros;

    ae_vector_clear(trend);
    ae_vector_clear(noise);

    ae_assert(nticks>=1, "SSAAnalyzeLast: NTicks<1", _state);
    
    /*
     * Init
     */
    ae_vector_set_length(trend, nticks, _state);
    ae_vector_set_length(noise, nticks, _state);
    
    /*
     * Is it degenerate case?
     */
    if( !ssa_hassomethingtoanalyze(s, _state)||!ssa_issequencebigenough(s, -1, _state) )
    {
        for(i=0; i<=nticks-1; i++)
        {
            trend->ptr.p_double[i] = (double)(0);
            noise->ptr.p_double[i] = (double)(0);
        }
        if( s->nsequences>=1 )
        {
            cnt = ae_minint(s->sequenceidx.ptr.p_int[s->nsequences]-s->sequenceidx.ptr.p_int[s->nsequences-1], nticks, _state);
            offs = s->sequenceidx.ptr.p_int[s->nsequences]-cnt;
            for(i=0; i<=cnt-1; i++)
            {
                noise->ptr.p_double[nticks-cnt+i] = s->sequencedata.ptr.p_double[offs+i];
            }
        }
        return;
    }
    
    /*
     * Fast exit: NTicks<=WindowWidth, just last window is analyzed
     */
    if( nticks<=s->windowwidth )
    {
        ssaanalyzelastwindow(s, &s->alongtrend, &s->alongnoise, &cnt, _state);
        offs = s->windowwidth-nticks;
        for(i=0; i<=nticks-1; i++)
        {
            trend->ptr.p_double[i] = s->alongtrend.ptr.p_double[offs+i];
            noise->ptr.p_double[i] = s->alongnoise.ptr.p_double[offs+i];
        }
        return;
    }
    
    /*
     * Update basis.
     *
     * It will take care of basis validity flags. AppendLen=0 which means
     * that we perform initial basis evaluation.
     */
    ssa_updatebasis(s, 0, 0.0, _state);
    
    /*
     * Perform analysis:
     * * prepend max(NTicks-LastSequenceLength,0) zeros to the beginning
     *   of array
     * * analyze the rest with AnalyzeSequence() which assumes that we
     *   already have basis
     */
    ae_assert(s->sequenceidx.ptr.p_int[s->nsequences]-s->sequenceidx.ptr.p_int[s->nsequences-1]>=s->windowwidth, "SSAAnalyzeLast: integrity check failed / 23vd4", _state);
    cntzeros = ae_maxint(nticks-(s->sequenceidx.ptr.p_int[s->nsequences]-s->sequenceidx.ptr.p_int[s->nsequences-1]), 0, _state);
    for(i=0; i<=cntzeros-1; i++)
    {
        trend->ptr.p_double[i] = 0.0;
        noise->ptr.p_double[i] = 0.0;
    }
    cnt = ae_minint(nticks, s->sequenceidx.ptr.p_int[s->nsequences]-s->sequenceidx.ptr.p_int[s->nsequences-1], _state);
    ssa_analyzesequence(s, &s->sequencedata, s->sequenceidx.ptr.p_int[s->nsequences]-cnt, s->sequenceidx.ptr.p_int[s->nsequences], trend, noise, cntzeros, _state);
}


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
     ae_state *_state)
{
    ae_int_t i;

    ae_vector_clear(trend);
    ae_vector_clear(noise);

    ae_assert(nticks>=1, "SSAAnalyzeSequence: NTicks<1", _state);
    ae_assert(data->cnt>=nticks, "SSAAnalyzeSequence: Data is too short", _state);
    ae_assert(isfinitevector(data, nticks, _state), "SSAAnalyzeSequence: Data contains infinities NANs", _state);
    
    /*
     * Init
     */
    ae_vector_set_length(trend, nticks, _state);
    ae_vector_set_length(noise, nticks, _state);
    
    /*
     * Is it degenerate case?
     */
    if( !ssa_hassomethingtoanalyze(s, _state)||nticks<s->windowwidth )
    {
        for(i=0; i<=nticks-1; i++)
        {
            trend->ptr.p_double[i] = (double)(0);
            noise->ptr.p_double[i] = data->ptr.p_double[i];
        }
        return;
    }
    
    /*
     * Update basis.
     *
     * It will take care of basis validity flags. AppendLen=0 which means
     * that we perform initial basis evaluation.
     */
    ssa_updatebasis(s, 0, 0.0, _state);
    
    /*
     * Perform analysis
     */
    ssa_analyzesequence(s, data, 0, nticks, trend, noise, 0, _state);
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;
    ae_int_t winw;

    ae_vector_clear(trend);

    ae_assert(nticks>=1, "SSAForecast: NTicks<1", _state);
    
    /*
     * Init
     */
    winw = s->windowwidth;
    ae_vector_set_length(trend, nticks, _state);
    
    /*
     * Is it degenerate case?
     */
    if( !ssa_hassomethingtoanalyze(s, _state) )
    {
        for(i=0; i<=nticks-1; i++)
        {
            trend->ptr.p_double[i] = (double)(0);
        }
        return;
    }
    ae_assert(s->nsequences>0, "SSAForecastLast: integrity check failed", _state);
    if( s->sequenceidx.ptr.p_int[s->nsequences]-s->sequenceidx.ptr.p_int[s->nsequences-1]<winw )
    {
        for(i=0; i<=nticks-1; i++)
        {
            trend->ptr.p_double[i] = (double)(0);
        }
        return;
    }
    if( winw==1 )
    {
        ae_assert(s->nsequences>0, "SSAForecast: integrity check failed / 2355", _state);
        ae_assert(s->sequenceidx.ptr.p_int[s->nsequences]-s->sequenceidx.ptr.p_int[s->nsequences-1]>0, "SSAForecast: integrity check failed", _state);
        for(i=0; i<=nticks-1; i++)
        {
            trend->ptr.p_double[i] = s->sequencedata.ptr.p_double[s->sequenceidx.ptr.p_int[s->nsequences]-1];
        }
        return;
    }
    
    /*
     * Update basis and recurrent relation.
     *
     * It will take care of basis validity flags. AppendLen=0 which means
     * that we perform initial basis evaluation.
     */
    ssa_updatebasis(s, 0, 0.0, _state);
    ae_assert(s->nbasis<=winw&&s->nbasis>0, "SSAForecast: integrity check failed / 4f5et", _state);
    if( s->nbasis==winw )
    {
        
        /*
         * Handle degenerate situation with basis whose size
         * is equal to window length.
         */
        ae_assert(s->nsequences>0, "SSAForecast: integrity check failed / 2355", _state);
        ae_assert(s->sequenceidx.ptr.p_int[s->nsequences]-s->sequenceidx.ptr.p_int[s->nsequences-1]>0, "SSAForecast: integrity check failed", _state);
        for(i=0; i<=nticks-1; i++)
        {
            trend->ptr.p_double[i] = s->sequencedata.ptr.p_double[s->sequenceidx.ptr.p_int[s->nsequences]-1];
        }
        return;
    }
    
    /*
     * Apply recurrent formula for SSA forecasting:
     * * first, perform smoothing of the last window
     * * second, perform analysis phase
     */
    ae_assert(s->nsequences>0, "SSAForecastLast: integrity check failed", _state);
    ae_assert(s->sequenceidx.ptr.p_int[s->nsequences]-s->sequenceidx.ptr.p_int[s->nsequences-1]>=s->windowwidth, "SSAForecastLast: integrity check failed", _state);
    rvectorsetlengthatleast(&s->tmp0, s->nbasis, _state);
    rvectorsetlengthatleast(&s->fctrend, s->windowwidth, _state);
    rmatrixgemv(s->nbasis, s->windowwidth, 1.0, &s->basist, 0, 0, 0, &s->sequencedata, s->sequenceidx.ptr.p_int[s->nsequences]-s->windowwidth, 0.0, &s->tmp0, 0, _state);
    rmatrixgemv(s->windowwidth, s->nbasis, 1.0, &s->basis, 0, 0, 0, &s->tmp0, 0, 0.0, &s->fctrend, 0, _state);
    rvectorsetlengthatleast(&s->tmp1, winw-1, _state);
    for(i=1; i<=winw-1; i++)
    {
        s->tmp1.ptr.p_double[i-1] = s->fctrend.ptr.p_double[i];
    }
    for(i=0; i<=nticks-1; i++)
    {
        v = s->forecasta.ptr.p_double[0]*s->tmp1.ptr.p_double[0];
        for(j=1; j<=winw-2; j++)
        {
            v = v+s->forecasta.ptr.p_double[j]*s->tmp1.ptr.p_double[j];
            s->tmp1.ptr.p_double[j-1] = s->tmp1.ptr.p_double[j];
        }
        trend->ptr.p_double[i] = v;
        s->tmp1.ptr.p_double[winw-2] = v;
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;
    ae_int_t winw;

    ae_vector_clear(trend);

    ae_assert(datalen>=1, "SSAForecastSequence: DataLen<1", _state);
    ae_assert(data->cnt>=datalen, "SSAForecastSequence: Data is too short", _state);
    ae_assert(isfinitevector(data, datalen, _state), "SSAForecastSequence: Data contains infinities NANs", _state);
    ae_assert(forecastlen>=1, "SSAForecastSequence: ForecastLen<1", _state);
    
    /*
     * Init
     */
    winw = s->windowwidth;
    ae_vector_set_length(trend, forecastlen, _state);
    
    /*
     * Is it degenerate case?
     */
    if( !ssa_hassomethingtoanalyze(s, _state)||datalen<winw )
    {
        for(i=0; i<=forecastlen-1; i++)
        {
            trend->ptr.p_double[i] = (double)(0);
        }
        return;
    }
    if( winw==1 )
    {
        for(i=0; i<=forecastlen-1; i++)
        {
            trend->ptr.p_double[i] = data->ptr.p_double[datalen-1];
        }
        return;
    }
    
    /*
     * Update basis.
     *
     * It will take care of basis validity flags. AppendLen=0 which means
     * that we perform initial basis evaluation.
     */
    ssa_updatebasis(s, 0, 0.0, _state);
    ae_assert(s->nbasis<=winw&&s->nbasis>0, "SSAForecast: integrity check failed / 4f5et", _state);
    if( s->nbasis==winw )
    {
        
        /*
         * Handle degenerate situation with basis whose size
         * is equal to window length.
         */
        for(i=0; i<=forecastlen-1; i++)
        {
            trend->ptr.p_double[i] = data->ptr.p_double[datalen-1];
        }
        return;
    }
    
    /*
     * Perform trend extraction
     */
    rvectorsetlengthatleast(&s->fctrend, s->windowwidth, _state);
    if( applysmoothing )
    {
        ae_assert(datalen>=winw, "SSAForecastSequence: integrity check failed", _state);
        rvectorsetlengthatleast(&s->tmp0, s->nbasis, _state);
        rmatrixgemv(s->nbasis, winw, 1.0, &s->basist, 0, 0, 0, data, datalen-winw, 0.0, &s->tmp0, 0, _state);
        rmatrixgemv(winw, s->nbasis, 1.0, &s->basis, 0, 0, 0, &s->tmp0, 0, 0.0, &s->fctrend, 0, _state);
    }
    else
    {
        for(i=0; i<=winw-1; i++)
        {
            s->fctrend.ptr.p_double[i] = data->ptr.p_double[datalen+i-winw];
        }
    }
    
    /*
     * Apply recurrent formula for SSA forecasting
     */
    rvectorsetlengthatleast(&s->tmp1, winw-1, _state);
    for(i=1; i<=winw-1; i++)
    {
        s->tmp1.ptr.p_double[i-1] = s->fctrend.ptr.p_double[i];
    }
    for(i=0; i<=forecastlen-1; i++)
    {
        v = s->forecasta.ptr.p_double[0]*s->tmp1.ptr.p_double[0];
        for(j=1; j<=winw-2; j++)
        {
            v = v+s->forecasta.ptr.p_double[j]*s->tmp1.ptr.p_double[j];
            s->tmp1.ptr.p_double[j-1] = s->tmp1.ptr.p_double[j];
        }
        trend->ptr.p_double[i] = v;
        s->tmp1.ptr.p_double[winw-2] = v;
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t winw;

    ae_vector_clear(trend);

    ae_assert(nticks>=1, "SSAForecastAvgLast: NTicks<1", _state);
    ae_assert(m>=1, "SSAForecastAvgLast: M<1", _state);
    
    /*
     * Init
     */
    winw = s->windowwidth;
    ae_vector_set_length(trend, nticks, _state);
    
    /*
     * Is it degenerate case?
     */
    if( !ssa_hassomethingtoanalyze(s, _state) )
    {
        for(i=0; i<=nticks-1; i++)
        {
            trend->ptr.p_double[i] = (double)(0);
        }
        return;
    }
    ae_assert(s->nsequences>0, "SSAForecastAvgLast: integrity check failed", _state);
    if( s->sequenceidx.ptr.p_int[s->nsequences]-s->sequenceidx.ptr.p_int[s->nsequences-1]<winw )
    {
        for(i=0; i<=nticks-1; i++)
        {
            trend->ptr.p_double[i] = (double)(0);
        }
        return;
    }
    if( winw==1 )
    {
        ae_assert(s->nsequences>0, "SSAForecastAvgLast: integrity check failed / 2355", _state);
        ae_assert(s->sequenceidx.ptr.p_int[s->nsequences]-s->sequenceidx.ptr.p_int[s->nsequences-1]>0, "SSAForecastAvgLast: integrity check failed", _state);
        for(i=0; i<=nticks-1; i++)
        {
            trend->ptr.p_double[i] = s->sequencedata.ptr.p_double[s->sequenceidx.ptr.p_int[s->nsequences]-1];
        }
        return;
    }
    
    /*
     * Update basis and recurrent relation.
     *
     * It will take care of basis validity flags. AppendLen=0 which means
     * that we perform initial basis evaluation.
     */
    ssa_updatebasis(s, 0, 0.0, _state);
    ae_assert(s->nbasis<=winw&&s->nbasis>0, "SSAForecastAvgLast: integrity check failed / 4f5et", _state);
    if( s->nbasis==winw )
    {
        
        /*
         * Handle degenerate situation with basis whose size
         * is equal to window length.
         */
        ae_assert(s->nsequences>0, "SSAForecastAvgLast: integrity check failed / 2355", _state);
        ae_assert(s->sequenceidx.ptr.p_int[s->nsequences]-s->sequenceidx.ptr.p_int[s->nsequences-1]>0, "SSAForecastAvgLast: integrity check failed", _state);
        for(i=0; i<=nticks-1; i++)
        {
            trend->ptr.p_double[i] = s->sequencedata.ptr.p_double[s->sequenceidx.ptr.p_int[s->nsequences]-1];
        }
        return;
    }
    
    /*
     * Decrease M if we have less than M sliding windows.
     * Forecast.
     */
    m = ae_minint(m, s->sequenceidx.ptr.p_int[s->nsequences]-s->sequenceidx.ptr.p_int[s->nsequences-1]-winw+1, _state);
    ae_assert(m>=1, "SSAForecastAvgLast: integrity check failed", _state);
    ssa_forecastavgsequence(s, &s->sequencedata, s->sequenceidx.ptr.p_int[s->nsequences-1], s->sequenceidx.ptr.p_int[s->nsequences], m, nticks, ae_true, trend, 0, _state);
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t winw;

    ae_vector_clear(trend);

    ae_assert(datalen>=1, "SSAForecastAvgSequence: DataLen<1", _state);
    ae_assert(m>=1, "SSAForecastAvgSequence: M<1", _state);
    ae_assert(data->cnt>=datalen, "SSAForecastAvgSequence: Data is too short", _state);
    ae_assert(isfinitevector(data, datalen, _state), "SSAForecastAvgSequence: Data contains infinities NANs", _state);
    ae_assert(forecastlen>=1, "SSAForecastAvgSequence: ForecastLen<1", _state);
    
    /*
     * Init
     */
    winw = s->windowwidth;
    ae_vector_set_length(trend, forecastlen, _state);
    
    /*
     * Is it degenerate case?
     */
    if( !ssa_hassomethingtoanalyze(s, _state)||datalen<winw )
    {
        for(i=0; i<=forecastlen-1; i++)
        {
            trend->ptr.p_double[i] = (double)(0);
        }
        return;
    }
    if( winw==1 )
    {
        for(i=0; i<=forecastlen-1; i++)
        {
            trend->ptr.p_double[i] = data->ptr.p_double[datalen-1];
        }
        return;
    }
    
    /*
     * Update basis.
     *
     * It will take care of basis validity flags. AppendLen=0 which means
     * that we perform initial basis evaluation.
     */
    ssa_updatebasis(s, 0, 0.0, _state);
    ae_assert(s->nbasis<=winw&&s->nbasis>0, "SSAForecast: integrity check failed / 4f5et", _state);
    if( s->nbasis==winw )
    {
        
        /*
         * Handle degenerate situation with basis whose size
         * is equal to window length.
         */
        for(i=0; i<=forecastlen-1; i++)
        {
            trend->ptr.p_double[i] = data->ptr.p_double[datalen-1];
        }
        return;
    }
    
    /*
     * Decrease M if we have less than M sliding windows.
     * Forecast.
     */
    m = ae_minint(m, datalen-winw+1, _state);
    ae_assert(m>=1, "SSAForecastAvgLast: integrity check failed", _state);
    ssa_forecastavgsequence(s, data, 0, datalen, m, forecastlen, applysmoothing, trend, 0, _state);
}


/*************************************************************************
This function evaluates current model and tells whether we have some  data
which can be analyzed by current algorithm, or not.

No analysis can be done in the following degenerate cases:
* dataset is empty
* all sequences are shorter than the window length
* no algorithm is specified

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
static ae_bool ssa_hassomethingtoanalyze(const ssamodel* s,
     ae_state *_state)
{
    ae_int_t i;
    ae_bool allsmaller;
    ae_bool isdegenerate;
    ae_bool result;


    isdegenerate = ae_false;
    isdegenerate = isdegenerate||s->algotype==0;
    isdegenerate = isdegenerate||s->nsequences==0;
    allsmaller = ae_true;
    for(i=0; i<=s->nsequences-1; i++)
    {
        allsmaller = allsmaller&&s->sequenceidx.ptr.p_int[i+1]-s->sequenceidx.ptr.p_int[i]<s->windowwidth;
    }
    isdegenerate = isdegenerate||allsmaller;
    result = !isdegenerate;
    return result;
}


/*************************************************************************
This function checks whether I-th sequence is big enough for analysis or not.

I=-1 is used to denote last sequence (for NSequences=0)

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
static ae_bool ssa_issequencebigenough(const ssamodel* s,
     ae_int_t i,
     ae_state *_state)
{
    ae_bool result;


    ae_assert(i>=-1&&i<s->nsequences, "Assertion failed", _state);
    result = ae_false;
    if( s->nsequences==0 )
    {
        return result;
    }
    if( i<0 )
    {
        i = s->nsequences-1;
    }
    result = s->sequenceidx.ptr.p_int[i+1]-s->sequenceidx.ptr.p_int[i]>=s->windowwidth;
    return result;
}


/*************************************************************************
This function performs basis update. Either full update (recalculated from
the very beginning) or partial update (handles append to the  end  of  the
dataset).

With AppendLen=0 this function behaves as follows:
* if AreBasisAndSolverValid=False, then  solver  object  is  created  from
  scratch, initial calculations are performed according  to  specific  SSA
  algorithm being chosen. Basis/Solver validity flag is set to True,  then
  we immediately return.
* if AreBasisAndSolverValid=True, then nothing is done  -  we  immediately
  return.

With AppendLen>0 this function behaves as follows:
* if AreBasisAndSolverValid=False, then exception is  generated;  you  can
  append points only to fully constructed basis. Call this  function  with
  zero AppendLen BEFORE append, then perform append, then call it one more
  time with non-zero AppendLen.
* if AreBasisAndSolverValid=True, then basis is incrementally updated.  It
  also updates recurrence relation used for prediction. It is expected that
  either AppendLen=1, or AppendLen=length(last_sequence). Basis update  is
  performed with probability UpdateIts (larger-than-one values  mean  that
  some amount of iterations is always performed).


In any case, after calling this function we either:
* have an exception
* have completely valid basis

IMPORTANT: this function expects that we do NOT call it for degenerate tasks
           (no data). So, call it after check with HasSomethingToAnalyze()
           returned True.

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
static void ssa_updatebasis(ssamodel* s,
     ae_int_t appendlen,
     double updateits,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t srcoffs;
    ae_int_t dstoffs;
    ae_int_t winw;
    ae_int_t windowstotal;
    ae_int_t requesttype;
    ae_int_t requestsize;
    double v;
    ae_bool degeneraterecurrence;
    double nu2;
    ae_int_t subspaceits;
    ae_bool needevd;


    winw = s->windowwidth;
    
    /*
     * Critical checks
     */
    ae_assert(appendlen>=0, "SSA: incorrect parameters passed to UpdateBasis(), integrity check failed", _state);
    ae_assert(!(!s->arebasisandsolvervalid&&appendlen!=0), "SSA: incorrect parameters passed to UpdateBasis(), integrity check failed", _state);
    ae_assert(!(appendlen==0&&ae_fp_greater(updateits,0.0)), "SSA: incorrect parameters passed to UpdateBasis(), integrity check failed", _state);
    
    /*
     * Everything is OK, nothing to do
     */
    if( s->arebasisandsolvervalid&&appendlen==0 )
    {
        return;
    }
    
    /*
     * Seed RNG with fixed or random seed.
     *
     * RNG used when pseudorandomly deciding whether
     * to re-evaluate basis or not. Sandom seed is
     * important when we have several simultaneously
     * calculated SSA models - we do not want them
     * to be re-evaluated in same moments).
     */
    if( !s->arebasisandsolvervalid )
    {
        if( s->rngseed>0 )
        {
            hqrndseed(s->rngseed, s->rngseed+235, &s->rs, _state);
        }
        else
        {
            hqrndrandomize(&s->rs, _state);
        }
    }
    
    /*
     * Compute XXT for algorithms which need it
     */
    if( !s->arebasisandsolvervalid )
    {
        ae_assert(appendlen==0, "SSA: integrity check failed / 34cx6", _state);
        if( s->algotype==2 )
        {
            
            /*
             * Compute X*X^T for direct algorithm.
             * Quite straightforward, no subtle optimizations.
             */
            rmatrixsetlengthatleast(&s->xxt, winw, winw, _state);
            windowstotal = 0;
            for(i=0; i<=s->nsequences-1; i++)
            {
                windowstotal = windowstotal+ae_maxint(s->sequenceidx.ptr.p_int[i+1]-s->sequenceidx.ptr.p_int[i]-winw+1, 0, _state);
            }
            ae_assert(windowstotal>0, "SSA: integrity check in UpdateBasis() failed / 76t34", _state);
            for(i=0; i<=winw-1; i++)
            {
                for(j=0; j<=winw-1; j++)
                {
                    s->xxt.ptr.pp_double[i][j] = (double)(0);
                }
            }
            ssa_updatexxtprepare(s, windowstotal, winw, s->memorylimit, _state);
            for(i=0; i<=s->nsequences-1; i++)
            {
                for(j=0; j<=ae_maxint(s->sequenceidx.ptr.p_int[i+1]-s->sequenceidx.ptr.p_int[i]-winw+1, 0, _state)-1; j++)
                {
                    ssa_updatexxtsend(s, &s->sequencedata, s->sequenceidx.ptr.p_int[i]+j, &s->xxt, _state);
                }
            }
            ssa_updatexxtfinalize(s, &s->xxt, _state);
        }
        if( s->algotype==3 )
        {
            
            /*
             * Compute X*X^T for real-time algorithm:
             * * prepare queue of windows to merge into XXT
             * * shuffle queue in order to avoid time-related biases in algorithm
             * * dequeue first chunk
             */
            rmatrixsetlengthatleast(&s->xxt, winw, winw, _state);
            windowstotal = 0;
            for(i=0; i<=s->nsequences-1; i++)
            {
                windowstotal = windowstotal+ae_maxint(s->sequenceidx.ptr.p_int[i+1]-s->sequenceidx.ptr.p_int[i]-winw+1, 0, _state);
            }
            ae_assert(windowstotal>0, "SSA: integrity check in UpdateBasis() failed / 76t34", _state);
            ivectorsetlengthatleast(&s->rtqueue, windowstotal, _state);
            dstoffs = 0;
            for(i=0; i<=s->nsequences-1; i++)
            {
                for(j=0; j<=ae_maxint(s->sequenceidx.ptr.p_int[i+1]-s->sequenceidx.ptr.p_int[i]-winw+1, 0, _state)-1; j++)
                {
                    srcoffs = s->sequenceidx.ptr.p_int[i]+j;
                    s->rtqueue.ptr.p_int[dstoffs] = srcoffs;
                    inc(&dstoffs, _state);
                }
            }
            ae_assert(dstoffs==windowstotal, "SSA: integrity check in UpdateBasis() failed / fh45f", _state);
            if( s->rtpowerup>1 )
            {
                
                /*
                 * Shuffle queue, it helps to avoid time-related bias in algorithm
                 */
                for(i=0; i<=windowstotal-1; i++)
                {
                    j = i+hqrnduniformi(&s->rs, windowstotal-i, _state);
                    swapelementsi(&s->rtqueue, i, j, _state);
                }
            }
            s->rtqueuecnt = windowstotal;
            s->rtqueuechunk = 1;
            s->rtqueuechunk = ae_maxint(s->rtqueuechunk, s->rtqueuecnt/s->rtpowerup, _state);
            s->rtqueuechunk = ae_maxint(s->rtqueuechunk, 2*s->topk, _state);
            ssa_realtimedequeue(s, 0.0, ae_minint(s->rtqueuechunk, s->rtqueuecnt, _state), _state);
        }
    }
    
    /*
     * Handle possible updates for XXT:
     * * check that append involves either last point of last sequence,
     *   or entire last sequence
     * * if last sequence is shorter than window width, perform quick exit -
     *   we have nothing to update - no windows to insert into XXT
     * * update XXT
     */
    if( appendlen>0 )
    {
        ae_assert(s->arebasisandsolvervalid, "SSA: integrity check failed / 5gvz3", _state);
        ae_assert(s->nsequences>=1, "SSA: integrity check failed / 658ev", _state);
        ae_assert(appendlen==1||appendlen==s->sequenceidx.ptr.p_int[s->nsequences]-s->sequenceidx.ptr.p_int[s->nsequences-1]-winw+1, "SSA: integrity check failed / sd3g7", _state);
        if( s->sequenceidx.ptr.p_int[s->nsequences]-s->sequenceidx.ptr.p_int[s->nsequences-1]<winw )
        {
            
            /*
             * Last sequence is too short, nothing to update
             */
            return;
        }
        if( s->algotype==2||s->algotype==3 )
        {
            if( appendlen>1 )
            {
                
                /*
                 * Long append, use GEMM for updates
                 */
                ssa_updatexxtprepare(s, appendlen, winw, s->memorylimit, _state);
                for(j=0; j<=ae_maxint(s->sequenceidx.ptr.p_int[s->nsequences]-s->sequenceidx.ptr.p_int[s->nsequences-1]-winw+1, 0, _state)-1; j++)
                {
                    ssa_updatexxtsend(s, &s->sequencedata, s->sequenceidx.ptr.p_int[s->nsequences-1]+j, &s->xxt, _state);
                }
                ssa_updatexxtfinalize(s, &s->xxt, _state);
            }
            else
            {
                
                /*
                 * Just one element is added, use rank-1 update
                 */
                rmatrixger(winw, winw, &s->xxt, 0, 0, 1.0, &s->sequencedata, s->sequenceidx.ptr.p_int[s->nsequences]-winw, &s->sequencedata, s->sequenceidx.ptr.p_int[s->nsequences]-winw, _state);
            }
        }
    }
    
    /*
     * Now, perform basis calculation - either full recalculation (AppendLen=0)
     * or quick update (AppendLen>0).
     */
    if( s->algotype==1 )
    {
        
        /*
         * Precomputed basis
         */
        if( winw!=s->precomputedwidth )
        {
            
            /*
             * Window width has changed, reset basis to zeros
             */
            s->nbasis = 1;
            rmatrixsetlengthatleast(&s->basis, winw, 1, _state);
            rvectorsetlengthatleast(&s->sv, 1, _state);
            for(i=0; i<=winw-1; i++)
            {
                s->basis.ptr.pp_double[i][0] = 0.0;
            }
            s->sv.ptr.p_double[0] = 0.0;
        }
        else
        {
            
            /*
             * OK, use precomputed basis
             */
            s->nbasis = s->precomputednbasis;
            rmatrixsetlengthatleast(&s->basis, winw, s->nbasis, _state);
            rvectorsetlengthatleast(&s->sv, s->nbasis, _state);
            for(j=0; j<=s->nbasis-1; j++)
            {
                s->sv.ptr.p_double[j] = 0.0;
                for(i=0; i<=winw-1; i++)
                {
                    s->basis.ptr.pp_double[i][j] = s->precomputedbasis.ptr.pp_double[i][j];
                }
            }
        }
        rmatrixsetlengthatleast(&s->basist, s->nbasis, winw, _state);
        rmatrixtranspose(winw, s->nbasis, &s->basis, 0, 0, &s->basist, 0, 0, _state);
    }
    else
    {
        if( s->algotype==2 )
        {
            
            /*
             * Direct top-K algorithm
             *
             * Calculate eigenvectors with SMatrixEVD(), reorder by descending
             * of magnitudes.
             *
             * Update is performed for invalid basis or for non-zero UpdateIts.
             */
            needevd = !s->arebasisandsolvervalid;
            needevd = needevd||ae_fp_greater_eq(updateits,(double)(1));
            needevd = needevd||ae_fp_less(hqrnduniformr(&s->rs, _state),updateits-(double)ae_ifloor(updateits, _state));
            if( needevd )
            {
                inc(&s->dbgcntevd, _state);
                s->nbasis = ae_minint(winw, s->topk, _state);
                if( !smatrixevd(&s->xxt, winw, 1, ae_true, &s->sv, &s->basis, _state) )
                {
                    ae_assert(ae_false, "SSA: SMatrixEVD failed", _state);
                }
                for(i=0; i<=winw-1; i++)
                {
                    k = winw-1-i;
                    if( i>=k )
                    {
                        break;
                    }
                    v = s->sv.ptr.p_double[i];
                    s->sv.ptr.p_double[i] = s->sv.ptr.p_double[k];
                    s->sv.ptr.p_double[k] = v;
                    for(j=0; j<=winw-1; j++)
                    {
                        v = s->basis.ptr.pp_double[j][i];
                        s->basis.ptr.pp_double[j][i] = s->basis.ptr.pp_double[j][k];
                        s->basis.ptr.pp_double[j][k] = v;
                    }
                }
                for(i=0; i<=s->nbasis-1; i++)
                {
                    s->sv.ptr.p_double[i] = ae_sqrt(ae_maxreal(s->sv.ptr.p_double[i], 0.0, _state), _state);
                }
                rmatrixsetlengthatleast(&s->basist, s->nbasis, winw, _state);
                rmatrixtranspose(winw, s->nbasis, &s->basis, 0, 0, &s->basist, 0, 0, _state);
            }
        }
        else
        {
            if( s->algotype==3 )
            {
                
                /*
                 * Real-time top-K.
                 *
                 * Determine actual number of basis components, prepare subspace
                 * solver (either create from scratch or reuse).
                 *
                 * Update is always performed for invalid basis; for a valid basis
                 * it is performed with probability UpdateIts.
                 */
                if( s->rtpowerup==1 )
                {
                    subspaceits = s->defaultsubspaceits;
                }
                else
                {
                    subspaceits = 3;
                }
                if( appendlen>0 )
                {
                    ae_assert(s->arebasisandsolvervalid, "SSA: integrity check in UpdateBasis() failed / srg6f", _state);
                    ae_assert(ae_fp_greater_eq(updateits,(double)(0)), "SSA: integrity check in UpdateBasis() failed / srg4f", _state);
                    subspaceits = ae_ifloor(updateits, _state);
                    if( ae_fp_less(hqrnduniformr(&s->rs, _state),updateits-(double)ae_ifloor(updateits, _state)) )
                    {
                        inc(&subspaceits, _state);
                    }
                    ae_assert(subspaceits>=0, "SSA: integrity check in UpdateBasis() failed / srg9f", _state);
                }
                
                /*
                 * Dequeue pending dataset and merge it into XXT.
                 *
                 * Dequeuing is done only for appends, and only when we have
                 * non-empty queue.
                 */
                if( appendlen>0&&s->rtqueuecnt>0 )
                {
                    ssa_realtimedequeue(s, 1.0, ae_minint(s->rtqueuechunk, s->rtqueuecnt, _state), _state);
                }
                
                /*
                 * Now, proceed to solver
                 */
                if( subspaceits>0 )
                {
                    if( appendlen==0 )
                    {
                        s->nbasis = ae_minint(winw, s->topk, _state);
                        eigsubspacecreatebuf(winw, s->nbasis, &s->solver, _state);
                    }
                    else
                    {
                        eigsubspacesetwarmstart(&s->solver, ae_true, _state);
                    }
                    eigsubspacesetcond(&s->solver, 0.0, subspaceits, _state);
                    
                    /*
                     * Perform initial basis estimation
                     */
                    inc(&s->dbgcntevd, _state);
                    eigsubspaceoocstart(&s->solver, 0, _state);
                    while(eigsubspaceooccontinue(&s->solver, _state))
                    {
                        eigsubspaceoocgetrequestinfo(&s->solver, &requesttype, &requestsize, _state);
                        ae_assert(requesttype==0, "SSA: integrity check in UpdateBasis() failed / 346372", _state);
                        rmatrixgemm(winw, requestsize, winw, 1.0, &s->xxt, 0, 0, 0, &s->solver.x, 0, 0, 0, 0.0, &s->solver.ax, 0, 0, _state);
                    }
                    eigsubspaceoocstop(&s->solver, &s->sv, &s->basis, &s->solverrep, _state);
                    for(i=0; i<=s->nbasis-1; i++)
                    {
                        s->sv.ptr.p_double[i] = ae_sqrt(ae_maxreal(s->sv.ptr.p_double[i], 0.0, _state), _state);
                    }
                    rmatrixsetlengthatleast(&s->basist, s->nbasis, winw, _state);
                    rmatrixtranspose(winw, s->nbasis, &s->basis, 0, 0, &s->basist, 0, 0, _state);
                }
            }
            else
            {
                ae_assert(ae_false, "SSA: integrity check in UpdateBasis() failed / dfgs34", _state);
            }
        }
    }
    
    /*
     * Update recurrent relation
     */
    rvectorsetlengthatleast(&s->forecasta, ae_maxint(winw-1, 1, _state), _state);
    degeneraterecurrence = ae_false;
    if( winw>1 )
    {
        
        /*
         * Non-degenerate case
         */
        rvectorsetlengthatleast(&s->tmp0, s->nbasis, _state);
        nu2 = 0.0;
        for(i=0; i<=s->nbasis-1; i++)
        {
            v = s->basist.ptr.pp_double[i][winw-1];
            s->tmp0.ptr.p_double[i] = v;
            nu2 = nu2+v*v;
        }
        if( ae_fp_less(nu2,(double)1-(double)1000*ae_machineepsilon) )
        {
            rmatrixgemv(winw-1, s->nbasis, (double)1/((double)1-nu2), &s->basist, 0, 0, 1, &s->tmp0, 0, 0.0, &s->forecasta, 0, _state);
        }
        else
        {
            degeneraterecurrence = ae_true;
        }
    }
    else
    {
        degeneraterecurrence = ae_true;
    }
    if( degeneraterecurrence )
    {
        for(i=0; i<=ae_maxint(winw-1, 1, _state)-1; i++)
        {
            s->forecasta.ptr.p_double[i] = 0.0;
        }
        s->forecasta.ptr.p_double[ae_maxint(winw-1, 1, _state)-1] = 1.0;
    }
    
    /*
     * Set validity flag
     */
    s->arebasisandsolvervalid = ae_true;
}


/*************************************************************************
This function performs analysis using current basis. It assumes and checks
that validity flag AreBasisAndSolverValid is set.

INPUT PARAMETERS:
    S                   -   model
    Data                -   array which holds data in elements [I0,I1):
                            * right bound is not included.
                            * I1-I0>=WindowWidth (assertion is performed).
    Trend               -   preallocated output array, large enough
    Noise               -   preallocated output array, large enough
    Offs                -   offset in Trend/Noise where result is stored;
                            I1-I0 elements are written starting at offset
                            Offs.
                            
OUTPUT PARAMETERS:
    Trend, Noise - processing results
                            

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
static void ssa_analyzesequence(ssamodel* s,
     /* Real    */ const ae_vector* data,
     ae_int_t i0,
     ae_int_t i1,
     /* Real    */ ae_vector* trend,
     /* Real    */ ae_vector* noise,
     ae_int_t offs,
     ae_state *_state)
{
    ae_int_t winw;
    ae_int_t nwindows;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t cnt;
    ae_int_t batchstart;
    ae_int_t batchlimit;
    ae_int_t batchsize;


    ae_assert(s->arebasisandsolvervalid, "AnalyzeSequence: integrity check failed / d84sz0", _state);
    ae_assert(i1-i0>=s->windowwidth, "AnalyzeSequence: integrity check failed / d84sz1", _state);
    ae_assert(s->nbasis>=1, "AnalyzeSequence: integrity check failed / d84sz2", _state);
    nwindows = i1-i0-s->windowwidth+1;
    winw = s->windowwidth;
    batchlimit = ae_maxint(nwindows, 1, _state);
    if( s->memorylimit>0 )
    {
        batchlimit = ae_minint(batchlimit, ae_maxint(s->memorylimit/winw, 4*winw, _state), _state);
    }
    
    /*
     * Zero-initialize trend and counts
     */
    cnt = i1-i0;
    ivectorsetlengthatleast(&s->aseqcounts, cnt, _state);
    for(i=0; i<=cnt-1; i++)
    {
        s->aseqcounts.ptr.p_int[i] = 0;
        trend->ptr.p_double[offs+i] = 0.0;
    }
    
    /*
     * Reset temporaries if algorithm settings changed since last round
     */
    if( s->aseqtrajectory.cols!=winw )
    {
        ae_matrix_set_length(&s->aseqtrajectory, 0, 0, _state);
    }
    if( s->aseqtbproduct.cols!=s->nbasis )
    {
        ae_matrix_set_length(&s->aseqtbproduct, 0, 0, _state);
    }
    
    /*
     * Perform batch processing
     */
    rmatrixsetlengthatleast(&s->aseqtrajectory, batchlimit, winw, _state);
    rmatrixsetlengthatleast(&s->aseqtbproduct, batchlimit, s->nbasis, _state);
    batchsize = 0;
    batchstart = offs;
    for(i=0; i<=nwindows-1; i++)
    {
        
        /*
         * Enqueue next row of trajectory matrix
         */
        if( batchsize==0 )
        {
            batchstart = i;
        }
        for(j=0; j<=winw-1; j++)
        {
            s->aseqtrajectory.ptr.pp_double[batchsize][j] = data->ptr.p_double[i0+i+j];
        }
        inc(&batchsize, _state);
        
        /*
         * Process batch
         */
        if( batchsize==batchlimit||i==nwindows-1 )
        {
            
            /*
             * Project onto basis
             */
            rmatrixgemm(batchsize, s->nbasis, winw, 1.0, &s->aseqtrajectory, 0, 0, 0, &s->basist, 0, 0, 1, 0.0, &s->aseqtbproduct, 0, 0, _state);
            rmatrixgemm(batchsize, winw, s->nbasis, 1.0, &s->aseqtbproduct, 0, 0, 0, &s->basist, 0, 0, 0, 0.0, &s->aseqtrajectory, 0, 0, _state);
            
            /*
             * Hankelize
             */
            for(k=0; k<=batchsize-1; k++)
            {
                for(j=0; j<=winw-1; j++)
                {
                    trend->ptr.p_double[offs+batchstart+k+j] = trend->ptr.p_double[offs+batchstart+k+j]+s->aseqtrajectory.ptr.pp_double[k][j];
                    s->aseqcounts.ptr.p_int[batchstart+k+j] = s->aseqcounts.ptr.p_int[batchstart+k+j]+1;
                }
            }
            
            /*
             * Reset batch size
             */
            batchsize = 0;
        }
    }
    for(i=0; i<=cnt-1; i++)
    {
        trend->ptr.p_double[offs+i] = trend->ptr.p_double[offs+i]/(double)s->aseqcounts.ptr.p_int[i];
    }
    
    /*
     * Output noise
     */
    for(i=0; i<=cnt-1; i++)
    {
        noise->ptr.p_double[offs+i] = data->ptr.p_double[i0+i]-trend->ptr.p_double[offs+i];
    }
}


/*************************************************************************
This function performs  averaged  forecasting.  It  assumes  that basis is
already built, everything is valid and checked. See  comments  on  similar
public functions to find out more about averaged predictions.

INPUT PARAMETERS:
    S                   -   model
    Data                -   array which holds data in elements [I0,I1):
                            * right bound is not included.
                            * I1-I0>=WindowWidth (assertion is performed).
    M                   -   number  of  sliding  windows  to combine, M>=1. If
                            your dataset has less than M sliding windows, this
                            parameter will be silently reduced.
    ForecastLen         -   number of ticks to predict, ForecastLen>=1
    Trend               -   preallocated output array, large enough
    Offs                -   offset in Trend where result is stored;
                            I1-I0 elements are written starting at offset
                            Offs.

OUTPUT PARAMETERS:
    Trend           -   array[ForecastLen], forecasted trend

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
static void ssa_forecastavgsequence(ssamodel* s,
     /* Real    */ const ae_vector* data,
     ae_int_t i0,
     ae_int_t i1,
     ae_int_t m,
     ae_int_t forecastlen,
     ae_bool smooth,
     /* Real    */ ae_vector* trend,
     ae_int_t offs,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t winw;


    ae_assert(s->arebasisandsolvervalid, "ForecastAvgSequence: integrity check failed / d84sz0", _state);
    ae_assert(i1-i0-s->windowwidth+1>=m, "ForecastAvgSequence: integrity check failed / d84sz1", _state);
    ae_assert(s->nbasis>=1, "ForecastAvgSequence: integrity check failed / d84sz2", _state);
    ae_assert(s->windowwidth>=2, "ForecastAvgSequence: integrity check failed / 5tgdg5", _state);
    ae_assert(s->windowwidth>s->nbasis, "ForecastAvgSequence: integrity check failed / d5g56w", _state);
    winw = s->windowwidth;
    
    /*
     * Prepare M synchronized predictions for the last known tick
     * (last one is an actual value of the trend, previous M-1 predictions
     * are predictions from differently positioned sliding windows).
     */
    rmatrixsetlengthatleast(&s->fctrendm, m, winw, _state);
    rvectorsetlengthatleast(&s->tmp0, ae_maxint(m, s->nbasis, _state), _state);
    rvectorsetlengthatleast(&s->tmp1, winw, _state);
    for(k=0; k<=m-1; k++)
    {
        
        /*
         * Perform prediction for rows [0,K-1]
         */
        rmatrixgemv(k, winw-1, 1.0, &s->fctrendm, 0, 1, 0, &s->forecasta, 0, 0.0, &s->tmp0, 0, _state);
        for(i=0; i<=k-1; i++)
        {
            for(j=1; j<=winw-1; j++)
            {
                s->fctrendm.ptr.pp_double[i][j-1] = s->fctrendm.ptr.pp_double[i][j];
            }
            s->fctrendm.ptr.pp_double[i][winw-1] = s->tmp0.ptr.p_double[i];
        }
        
        /*
         * Perform trend extraction for row K, add it to dataset
         */
        if( smooth )
        {
            rmatrixgemv(s->nbasis, winw, 1.0, &s->basist, 0, 0, 0, data, i1-winw-(m-1-k), 0.0, &s->tmp0, 0, _state);
            rmatrixgemv(s->windowwidth, s->nbasis, 1.0, &s->basis, 0, 0, 0, &s->tmp0, 0, 0.0, &s->tmp1, 0, _state);
            for(j=0; j<=winw-1; j++)
            {
                s->fctrendm.ptr.pp_double[k][j] = s->tmp1.ptr.p_double[j];
            }
        }
        else
        {
            for(j=0; j<=winw-1; j++)
            {
                s->fctrendm.ptr.pp_double[k][j] = data->ptr.p_double[i1-winw-(m-1-k)+j];
            }
        }
    }
    
    /*
     * Now we have M synchronized predictions of the sequence state at the last
     * know moment (last "prediction" is just a copy of the trend). Let's start
     * batch prediction!
     */
    for(k=0; k<=forecastlen-1; k++)
    {
        rmatrixgemv(m, winw-1, 1.0, &s->fctrendm, 0, 1, 0, &s->forecasta, 0, 0.0, &s->tmp0, 0, _state);
        trend->ptr.p_double[offs+k] = 0.0;
        for(i=0; i<=m-1; i++)
        {
            for(j=1; j<=winw-1; j++)
            {
                s->fctrendm.ptr.pp_double[i][j-1] = s->fctrendm.ptr.pp_double[i][j];
            }
            s->fctrendm.ptr.pp_double[i][winw-1] = s->tmp0.ptr.p_double[i];
            trend->ptr.p_double[offs+k] = trend->ptr.p_double[offs+k]+s->tmp0.ptr.p_double[i];
        }
        trend->ptr.p_double[offs+k] = trend->ptr.p_double[offs+k]/(double)m;
    }
}


/*************************************************************************
This function extracts updates from real-time queue and  applies  them  to
the S.XXT matrix. XXT is premultiplied by  Beta,  which  can  be  0.0  for
initial creation, 1.0 for subsequent updates, or even within (0,1) for some
kind of updates with decay.

INPUT PARAMETERS:
    S                   -   model
    Beta                -   >=0, coefficient to premultiply XXT
    Cnt                 -   0<Cnt<=S.RTQueueCnt, number of updates to extract
                            from the end of the queue
                            
OUTPUT PARAMETERS:
    S                   -   S.XXT updated, S.RTQueueCnt decreased

  -- ALGLIB --
     Copyright 30.10.2017 by Bochkanov Sergey
*************************************************************************/
static void ssa_realtimedequeue(ssamodel* s,
     double beta,
     ae_int_t cnt,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t winw;


    ae_assert(cnt>0, "SSA: RealTimeDequeue() integrity check failed / 43tdv", _state);
    ae_assert(ae_isfinite(beta, _state)&&ae_fp_greater_eq(beta,(double)(0)), "SSA: RealTimeDequeue() integrity check failed / 5gdg6", _state);
    ae_assert(cnt<=s->rtqueuecnt, "SSA: RealTimeDequeue() integrity check failed / 547yh", _state);
    ae_assert(s->xxt.cols>=s->windowwidth, "SSA: RealTimeDequeue() integrity check failed / 54bf4", _state);
    ae_assert(s->xxt.rows>=s->windowwidth, "SSA: RealTimeDequeue() integrity check failed / 9gdfn", _state);
    winw = s->windowwidth;
    
    /*
     * Premultiply XXT by Beta
     */
    if( ae_fp_neq(beta,(double)(0)) )
    {
        for(i=0; i<=winw-1; i++)
        {
            for(j=0; j<=winw-1; j++)
            {
                s->xxt.ptr.pp_double[i][j] = s->xxt.ptr.pp_double[i][j]*beta;
            }
        }
    }
    else
    {
        for(i=0; i<=winw-1; i++)
        {
            for(j=0; j<=winw-1; j++)
            {
                s->xxt.ptr.pp_double[i][j] = (double)(0);
            }
        }
    }
    
    /*
     * Dequeue
     */
    ssa_updatexxtprepare(s, cnt, winw, s->memorylimit, _state);
    for(i=0; i<=cnt-1; i++)
    {
        ssa_updatexxtsend(s, &s->sequencedata, s->rtqueue.ptr.p_int[s->rtqueuecnt-1], &s->xxt, _state);
        dec(&s->rtqueuecnt, _state);
    }
    ssa_updatexxtfinalize(s, &s->xxt, _state);
}


/*************************************************************************
This function prepares batch buffer for XXT update. The idea  is  that  we
send a stream of "XXT += u*u'" updates, and we want to package  them  into
one big matrix update U*U', applied with SYRK() kernel, but U can  consume
too much memory, so we want to transparently divide it  into  few  smaller
chunks.

This set of functions solves this problem:
* UpdateXXTPrepare() prepares temporary buffers
* UpdateXXTSend() sends next u to the buffer, possibly initiating next SYRK()
* UpdateXXTFinalize() performs last SYRK() update

INPUT PARAMETERS:
    S                   -   model, only fields with UX prefix are used
    UpdateSize          -   number of updates
    WindowWidth         -   window width, >0
    MemoryLimit         -   memory limit, non-positive value means no limit
                            
OUTPUT PARAMETERS:
    S                   -   UX temporaries updated

  -- ALGLIB --
     Copyright 20.12.2017 by Bochkanov Sergey
*************************************************************************/
static void ssa_updatexxtprepare(ssamodel* s,
     ae_int_t updatesize,
     ae_int_t windowwidth,
     ae_int_t memorylimit,
     ae_state *_state)
{


    ae_assert(windowwidth>0, "UpdateXXTPrepare: WinW<=0", _state);
    s->uxbatchlimit = ae_maxint(updatesize, 1, _state);
    if( memorylimit>0 )
    {
        s->uxbatchlimit = ae_minint(s->uxbatchlimit, ae_maxint(memorylimit/windowwidth, 4*windowwidth, _state), _state);
    }
    s->uxbatchwidth = windowwidth;
    s->uxbatchsize = 0;
    if( s->uxbatch.cols!=windowwidth )
    {
        ae_matrix_set_length(&s->uxbatch, 0, 0, _state);
    }
    rmatrixsetlengthatleast(&s->uxbatch, s->uxbatchlimit, windowwidth, _state);
}


/*************************************************************************
This function sends update u*u' to the batch buffer.

INPUT PARAMETERS:
    S                   -   model, only fields with UX prefix are used
    U                   -   WindowWidth-sized update, starts at I0
    I0                  -   starting position for update
                            
OUTPUT PARAMETERS:
    S                   -   UX temporaries updated
    XXT                 -   array[WindowWidth,WindowWidth], in the middle
                            of update. All intermediate updates are
                            applied to the upper triangle.

  -- ALGLIB --
     Copyright 20.12.2017 by Bochkanov Sergey
*************************************************************************/
static void ssa_updatexxtsend(ssamodel* s,
     /* Real    */ const ae_vector* u,
     ae_int_t i0,
     /* Real    */ ae_matrix* xxt,
     ae_state *_state)
{


    ae_assert(i0+s->uxbatchwidth-1<u->cnt, "UpdateXXTSend: incorrect U size", _state);
    ae_assert(s->uxbatchsize>=0, "UpdateXXTSend: integrity check failure", _state);
    ae_assert(s->uxbatchsize<=s->uxbatchlimit, "UpdateXXTSend: integrity check failure", _state);
    ae_assert(s->uxbatchlimit>=1, "UpdateXXTSend: integrity check failure", _state);
    
    /*
     * Send pending batch if full
     */
    if( s->uxbatchsize==s->uxbatchlimit )
    {
        rmatrixsyrk(s->uxbatchwidth, s->uxbatchsize, 1.0, &s->uxbatch, 0, 0, 2, 1.0, xxt, 0, 0, ae_true, _state);
        s->uxbatchsize = 0;
    }
    
    /*
     * Append update to batch
     */
    ae_v_move(&s->uxbatch.ptr.pp_double[s->uxbatchsize][0], 1, &u->ptr.p_double[i0], 1, ae_v_len(0,s->uxbatchwidth-1));
    inc(&s->uxbatchsize, _state);
}


/*************************************************************************
This function finalizes batch buffer. Call it after the last update.

INPUT PARAMETERS:
    S                   -   model, only fields with UX prefix are used
                            
OUTPUT PARAMETERS:
    S                   -   UX temporaries updated
    XXT                 -   array[WindowWidth,WindowWidth], updated with
                            all previous updates, both triangles of the
                            symmetric matrix are present.

  -- ALGLIB --
     Copyright 20.12.2017 by Bochkanov Sergey
*************************************************************************/
static void ssa_updatexxtfinalize(ssamodel* s,
     /* Real    */ ae_matrix* xxt,
     ae_state *_state)
{


    ae_assert(s->uxbatchsize>=0, "UpdateXXTFinalize: integrity check failure", _state);
    ae_assert(s->uxbatchsize<=s->uxbatchlimit, "UpdateXXTFinalize: integrity check failure", _state);
    ae_assert(s->uxbatchlimit>=1, "UpdateXXTFinalize: integrity check failure", _state);
    if( s->uxbatchsize>0 )
    {
        rmatrixsyrk(s->uxbatchwidth, s->uxbatchsize, 1.0, &s->uxbatch, 0, 0, 2, 1.0, &s->xxt, 0, 0, ae_true, _state);
        s->uxbatchsize = 0;
    }
    rmatrixenforcesymmetricity(&s->xxt, s->uxbatchwidth, ae_true, _state);
}


void _ssamodel_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    ssamodel *p = (ssamodel*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->sequenceidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->sequencedata, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->precomputedbasis, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->basis, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->basist, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->sv, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->forecasta, 0, DT_REAL, _state, make_automatic);
    _eigsubspacestate_init(&p->solver, _state, make_automatic);
    ae_matrix_init(&p->xxt, 0, 0, DT_REAL, _state, make_automatic);
    _hqrndstate_init(&p->rs, _state, make_automatic);
    ae_vector_init(&p->rtqueue, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp1, 0, DT_REAL, _state, make_automatic);
    _eigsubspacereport_init(&p->solverrep, _state, make_automatic);
    ae_vector_init(&p->alongtrend, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->alongnoise, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->aseqtrajectory, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->aseqtbproduct, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->aseqcounts, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->fctrend, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fcnoise, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->fctrendm, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->uxbatch, 0, 0, DT_REAL, _state, make_automatic);
}


void _ssamodel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    ssamodel       *dst = (ssamodel*)_dst;
    const ssamodel *src = (const ssamodel*)_src;
    dst->nsequences = src->nsequences;
    ae_vector_init_copy(&dst->sequenceidx, &src->sequenceidx, _state, make_automatic);
    ae_vector_init_copy(&dst->sequencedata, &src->sequencedata, _state, make_automatic);
    dst->algotype = src->algotype;
    dst->windowwidth = src->windowwidth;
    dst->rtpowerup = src->rtpowerup;
    dst->topk = src->topk;
    dst->precomputedwidth = src->precomputedwidth;
    dst->precomputednbasis = src->precomputednbasis;
    ae_matrix_init_copy(&dst->precomputedbasis, &src->precomputedbasis, _state, make_automatic);
    dst->defaultsubspaceits = src->defaultsubspaceits;
    dst->memorylimit = src->memorylimit;
    dst->arebasisandsolvervalid = src->arebasisandsolvervalid;
    ae_matrix_init_copy(&dst->basis, &src->basis, _state, make_automatic);
    ae_matrix_init_copy(&dst->basist, &src->basist, _state, make_automatic);
    ae_vector_init_copy(&dst->sv, &src->sv, _state, make_automatic);
    ae_vector_init_copy(&dst->forecasta, &src->forecasta, _state, make_automatic);
    dst->nbasis = src->nbasis;
    _eigsubspacestate_init_copy(&dst->solver, &src->solver, _state, make_automatic);
    ae_matrix_init_copy(&dst->xxt, &src->xxt, _state, make_automatic);
    _hqrndstate_init_copy(&dst->rs, &src->rs, _state, make_automatic);
    dst->rngseed = src->rngseed;
    ae_vector_init_copy(&dst->rtqueue, &src->rtqueue, _state, make_automatic);
    dst->rtqueuecnt = src->rtqueuecnt;
    dst->rtqueuechunk = src->rtqueuechunk;
    dst->dbgcntevd = src->dbgcntevd;
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp1, &src->tmp1, _state, make_automatic);
    _eigsubspacereport_init_copy(&dst->solverrep, &src->solverrep, _state, make_automatic);
    ae_vector_init_copy(&dst->alongtrend, &src->alongtrend, _state, make_automatic);
    ae_vector_init_copy(&dst->alongnoise, &src->alongnoise, _state, make_automatic);
    ae_matrix_init_copy(&dst->aseqtrajectory, &src->aseqtrajectory, _state, make_automatic);
    ae_matrix_init_copy(&dst->aseqtbproduct, &src->aseqtbproduct, _state, make_automatic);
    ae_vector_init_copy(&dst->aseqcounts, &src->aseqcounts, _state, make_automatic);
    ae_vector_init_copy(&dst->fctrend, &src->fctrend, _state, make_automatic);
    ae_vector_init_copy(&dst->fcnoise, &src->fcnoise, _state, make_automatic);
    ae_matrix_init_copy(&dst->fctrendm, &src->fctrendm, _state, make_automatic);
    ae_matrix_init_copy(&dst->uxbatch, &src->uxbatch, _state, make_automatic);
    dst->uxbatchwidth = src->uxbatchwidth;
    dst->uxbatchsize = src->uxbatchsize;
    dst->uxbatchlimit = src->uxbatchlimit;
}


void _ssamodel_clear(void* _p)
{
    ssamodel *p = (ssamodel*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->sequenceidx);
    ae_vector_clear(&p->sequencedata);
    ae_matrix_clear(&p->precomputedbasis);
    ae_matrix_clear(&p->basis);
    ae_matrix_clear(&p->basist);
    ae_vector_clear(&p->sv);
    ae_vector_clear(&p->forecasta);
    _eigsubspacestate_clear(&p->solver);
    ae_matrix_clear(&p->xxt);
    _hqrndstate_clear(&p->rs);
    ae_vector_clear(&p->rtqueue);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->tmp1);
    _eigsubspacereport_clear(&p->solverrep);
    ae_vector_clear(&p->alongtrend);
    ae_vector_clear(&p->alongnoise);
    ae_matrix_clear(&p->aseqtrajectory);
    ae_matrix_clear(&p->aseqtbproduct);
    ae_vector_clear(&p->aseqcounts);
    ae_vector_clear(&p->fctrend);
    ae_vector_clear(&p->fcnoise);
    ae_matrix_clear(&p->fctrendm);
    ae_matrix_clear(&p->uxbatch);
}


void _ssamodel_destroy(void* _p)
{
    ssamodel *p = (ssamodel*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->sequenceidx);
    ae_vector_destroy(&p->sequencedata);
    ae_matrix_destroy(&p->precomputedbasis);
    ae_matrix_destroy(&p->basis);
    ae_matrix_destroy(&p->basist);
    ae_vector_destroy(&p->sv);
    ae_vector_destroy(&p->forecasta);
    _eigsubspacestate_destroy(&p->solver);
    ae_matrix_destroy(&p->xxt);
    _hqrndstate_destroy(&p->rs);
    ae_vector_destroy(&p->rtqueue);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->tmp1);
    _eigsubspacereport_destroy(&p->solverrep);
    ae_vector_destroy(&p->alongtrend);
    ae_vector_destroy(&p->alongnoise);
    ae_matrix_destroy(&p->aseqtrajectory);
    ae_matrix_destroy(&p->aseqtbproduct);
    ae_vector_destroy(&p->aseqcounts);
    ae_vector_destroy(&p->fctrend);
    ae_vector_destroy(&p->fcnoise);
    ae_matrix_destroy(&p->fctrendm);
    ae_matrix_destroy(&p->uxbatch);
}


/*$ End $*/
