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
#include "dforest.h"


/*$ Declarations $*/
static ae_int_t dforest_innernodewidth = 3;
static ae_int_t dforest_leafnodewidth = 2;
static ae_int_t dforest_dfusestrongsplits = 1;
static ae_int_t dforest_dfuseevs = 2;
static ae_int_t dforest_dfuncompressedv0 = 0;
static ae_int_t dforest_dfcompressedv0 = 1;
static ae_int_t dforest_needtrngini = 1;
static ae_int_t dforest_needoobgini = 2;
static ae_int_t dforest_needpermutation = 3;
static ae_int_t dforest_permutationimportancebatchsize = 512;
static void dforest_buildrandomtree(decisionforestbuilder* s,
     ae_int_t treeidx0,
     ae_int_t treeidx1,
     ae_state *_state);
void _spawn_dforest_buildrandomtree(decisionforestbuilder* s,
    ae_int_t treeidx0,
    ae_int_t treeidx1, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_dforest_buildrandomtree(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_dforest_buildrandomtree(decisionforestbuilder* s,
    ae_int_t treeidx0,
    ae_int_t treeidx1, ae_state *_state);
static void dforest_buildrandomtreerec(const decisionforestbuilder* s,
     dfworkbuf* workbuf,
     ae_int_t workingset,
     ae_int_t varstoselect,
     /* Real    */ ae_vector* treebuf,
     dfvotebuf* votebuf,
     hqrndstate* rs,
     ae_int_t idx0,
     ae_int_t idx1,
     ae_int_t oobidx0,
     ae_int_t oobidx1,
     double meanloss,
     double topmostmeanloss,
     ae_int_t* treesize,
     ae_state *_state);
static void dforest_estimatevariableimportance(decisionforestbuilder* s,
     ae_int_t sessionseed,
     const decisionforest* df,
     ae_int_t ntrees,
     dfreport* rep,
     ae_state *_state);
void _spawn_dforest_estimatevariableimportance(decisionforestbuilder* s,
    ae_int_t sessionseed,
    const decisionforest* df,
    ae_int_t ntrees,
    dfreport* rep, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_dforest_estimatevariableimportance(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_dforest_estimatevariableimportance(decisionforestbuilder* s,
    ae_int_t sessionseed,
    const decisionforest* df,
    ae_int_t ntrees,
    dfreport* rep, ae_state *_state);
static void dforest_estimatepermutationimportances(decisionforestbuilder* s,
     const decisionforest* df,
     ae_int_t ntrees,
     ae_shared_pool* permpool,
     ae_int_t idx0,
     ae_int_t idx1,
     ae_state *_state);
void _spawn_dforest_estimatepermutationimportances(decisionforestbuilder* s,
    const decisionforest* df,
    ae_int_t ntrees,
    ae_shared_pool* permpool,
    ae_int_t idx0,
    ae_int_t idx1, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_dforest_estimatepermutationimportances(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_dforest_estimatepermutationimportances(decisionforestbuilder* s,
    const decisionforest* df,
    ae_int_t ntrees,
    ae_shared_pool* permpool,
    ae_int_t idx0,
    ae_int_t idx1, ae_state *_state);
static void dforest_cleanreport(const decisionforestbuilder* s,
     dfreport* rep,
     ae_state *_state);
static double dforest_meannrms2(ae_int_t nclasses,
     /* Integer */ const ae_vector* trnlabelsi,
     /* Real    */ const ae_vector* trnlabelsr,
     ae_int_t trnidx0,
     ae_int_t trnidx1,
     /* Integer */ const ae_vector* tstlabelsi,
     /* Real    */ const ae_vector* tstlabelsr,
     ae_int_t tstidx0,
     ae_int_t tstidx1,
     /* Integer */ ae_vector* tmpi,
     ae_state *_state);
static void dforest_choosecurrentsplitdense(const decisionforestbuilder* s,
     dfworkbuf* workbuf,
     ae_int_t* varsinpool,
     ae_int_t varstoselect,
     hqrndstate* rs,
     ae_int_t idx0,
     ae_int_t idx1,
     ae_int_t* varbest,
     double* splitbest,
     ae_state *_state);
static void dforest_evaluatedensesplit(const decisionforestbuilder* s,
     dfworkbuf* workbuf,
     hqrndstate* rs,
     ae_int_t splitvar,
     ae_int_t idx0,
     ae_int_t idx1,
     ae_int_t* info,
     double* split,
     double* rms,
     ae_state *_state);
static void dforest_classifiersplit(const decisionforestbuilder* s,
     dfworkbuf* workbuf,
     /* Real    */ ae_vector* x,
     /* Integer */ ae_vector* c,
     ae_int_t n,
     hqrndstate* rs,
     ae_int_t* info,
     double* threshold,
     double* e,
     /* Real    */ ae_vector* sortrbuf,
     /* Integer */ ae_vector* sortibuf,
     ae_state *_state);
static void dforest_regressionsplit(const decisionforestbuilder* s,
     dfworkbuf* workbuf,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_int_t n,
     ae_int_t* info,
     double* threshold,
     double* e,
     /* Real    */ ae_vector* sortrbuf,
     /* Real    */ ae_vector* sortrbuf2,
     ae_state *_state);
static double dforest_getsplit(const decisionforestbuilder* s,
     double a,
     double b,
     hqrndstate* rs,
     ae_state *_state);
static void dforest_outputleaf(const decisionforestbuilder* s,
     dfworkbuf* workbuf,
     /* Real    */ ae_vector* treebuf,
     dfvotebuf* votebuf,
     ae_int_t idx0,
     ae_int_t idx1,
     ae_int_t oobidx0,
     ae_int_t oobidx1,
     ae_int_t* treesize,
     double leafval,
     ae_state *_state);
static void dforest_analyzeandpreprocessdataset(decisionforestbuilder* s,
     ae_state *_state);
static void dforest_mergetrees(decisionforestbuilder* s,
     decisionforest* df,
     ae_state *_state);
static void dforest_processvotingresults(decisionforestbuilder* s,
     ae_int_t ntrees,
     dfvotebuf* buf,
     dfreport* rep,
     ae_state *_state);
static double dforest_binarycompression(decisionforest* df,
     ae_bool usemantissa8,
     ae_state *_state);
static ae_int_t dforest_computecompressedsizerec(const decisionforest* df,
     ae_bool usemantissa8,
     ae_int_t treeroot,
     ae_int_t treepos,
     /* Integer */ ae_vector* compressedsizes,
     ae_bool savecompressedsizes,
     ae_state *_state);
static void dforest_compressrec(const decisionforest* df,
     ae_bool usemantissa8,
     ae_int_t treeroot,
     ae_int_t treepos,
     /* Integer */ const ae_vector* compressedsizes,
     ae_vector* buf,
     ae_int_t* dstoffs,
     ae_state *_state);
static ae_int_t dforest_computecompresseduintsize(ae_int_t v,
     ae_state *_state);
static void dforest_streamuint(ae_vector* buf,
     ae_int_t* offs,
     ae_int_t v,
     ae_state *_state);
static ae_int_t dforest_unstreamuint(const ae_vector* buf,
     ae_int_t* offs,
     ae_state *_state);
static void dforest_streamfloat(ae_vector* buf,
     ae_bool usemantissa8,
     ae_int_t* offs,
     double v,
     ae_state *_state);
static double dforest_unstreamfloat(const ae_vector* buf,
     ae_bool usemantissa8,
     ae_int_t* offs,
     ae_state *_state);
static ae_int_t dforest_dfclserror(const decisionforest* df,
     /* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_state *_state);
static void dforest_dfprocessinternaluncompressed(const decisionforest* df,
     ae_int_t subtreeroot,
     ae_int_t nodeoffs,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
static void dforest_dfprocessinternalcompressed(const decisionforest* df,
     ae_int_t offs,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
static double dforest_xfastpow(double r, ae_int_t n, ae_state *_state);


/*$ Body $*/


/*************************************************************************
This function creates buffer  structure  which  can  be  used  to  perform
parallel inference requests.

DF subpackage  provides two sets of computing functions - ones  which  use
internal buffer of DF model  (these  functions are single-threaded because
they use same buffer, which can not  shared  between  threads),  and  ones
which use external buffer.

This function is used to initialize external buffer.

INPUT PARAMETERS
    Model       -   DF model which is associated with newly created buffer

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
void dfcreatebuffer(const decisionforest* model,
     decisionforestbuffer* buf,
     ae_state *_state)
{

    _decisionforestbuffer_clear(buf);

    ae_vector_set_length(&buf->x, model->nvars, _state);
    ae_vector_set_length(&buf->y, model->nclasses, _state);
}


/*************************************************************************
This subroutine creates DecisionForestBuilder  object  which  is  used  to
train decision forests.

By default, new builder stores empty dataset and some  reasonable  default
settings. At the very least, you should specify dataset prior to  building
decision forest. You can also tweak settings of  the  forest  construction
algorithm (recommended, although default setting should work well).

Following actions are mandatory:
* calling dfbuildersetdataset() to specify dataset
* calling dfbuilderbuildrandomforest()  to  build  decision  forest  using
  current dataset and default settings
  
Additionally, you may call:
* dfbuildersetrndvars() or dfbuildersetrndvarsratio() to specify number of
  variables randomly chosen for each split
* dfbuildersetsubsampleratio() to specify fraction of the dataset randomly
  subsampled to build each tree
* dfbuildersetseed() to control random seed chosen for tree construction

INPUT PARAMETERS:
    none

OUTPUT PARAMETERS:
    S           -   decision forest builder

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
void dfbuildercreate(decisionforestbuilder* s, ae_state *_state)
{

    _decisionforestbuilder_clear(s);

    
    /*
     * Empty dataset
     */
    s->dstype = -1;
    s->npoints = 0;
    s->nvars = 0;
    s->nclasses = 1;
    
    /*
     * Default training settings
     */
    s->rdfalgo = 0;
    s->rdfratio = 0.5;
    s->rdfvars = 0.0;
    s->rdfglobalseed = 0;
    s->rdfsplitstrength = 2;
    s->rdfimportance = 0;
    
    /*
     * Other fields
     */
    s->rdfprogress = 0;
    s->rdftotal = 1;
}


/*************************************************************************
This subroutine adds dense dataset to the internal storage of the  builder
object. Specifying your dataset in the dense format means that  the  dense
version of the forest construction algorithm will be invoked.

INPUT PARAMETERS:
    S           -   decision forest builder object
    XY          -   array[NPoints,NVars+1] (minimum size; actual size  can
                    be larger, only leading part is used anyway), dataset:
                    * first NVars elements of each row store values of the
                      independent variables
                    * last  column  store class number (in 0...NClasses-1)
                      or real value of the dependent variable
    NPoints     -   number of rows in the dataset, NPoints>=1
    NVars       -   number of independent variables, NVars>=1 
    NClasses    -   indicates type of the problem being solved:
                    * NClasses>=2 means  that  classification  problem  is
                      solved  (last  column  of  the  dataset stores class
                      number)
                    * NClasses=1 means that regression problem  is  solved
                      (last column of the dataset stores variable value)

OUTPUT PARAMETERS:
    S           -   decision forest builder

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
void dfbuildersetdataset(decisionforestbuilder* s,
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
    ae_assert(npoints>=1, "dfbuildersetdataset: npoints<1", _state);
    ae_assert(nvars>=1, "dfbuildersetdataset: nvars<1", _state);
    ae_assert(nclasses>=1, "dfbuildersetdataset: nclasses<1", _state);
    ae_assert(xy->rows>=npoints, "dfbuildersetdataset: rows(xy)<npoints", _state);
    ae_assert(xy->cols>=nvars+1, "dfbuildersetdataset: cols(xy)<nvars+1", _state);
    ae_assert(apservisfinitematrix(xy, npoints, nvars+1, _state), "dfbuildersetdataset: xy parameter contains INFs or NANs", _state);
    if( nclasses>1 )
    {
        for(i=0; i<=npoints-1; i++)
        {
            j = ae_round(xy->ptr.pp_double[i][nvars], _state);
            ae_assert(j>=0&&j<nclasses, "dfbuildersetdataset: last column of xy contains invalid class number", _state);
        }
    }
    
    /*
     * Set dataset
     */
    s->dstype = 0;
    s->npoints = npoints;
    s->nvars = nvars;
    s->nclasses = nclasses;
    rvectorsetlengthatleast(&s->dsdata, npoints*nvars, _state);
    for(i=0; i<=npoints-1; i++)
    {
        for(j=0; j<=nvars-1; j++)
        {
            s->dsdata.ptr.p_double[j*npoints+i] = xy->ptr.pp_double[i][j];
        }
    }
    if( nclasses>1 )
    {
        ivectorsetlengthatleast(&s->dsival, npoints, _state);
        for(i=0; i<=npoints-1; i++)
        {
            s->dsival.ptr.p_int[i] = ae_round(xy->ptr.pp_double[i][nvars], _state);
        }
    }
    else
    {
        rvectorsetlengthatleast(&s->dsrval, npoints, _state);
        for(i=0; i<=npoints-1; i++)
        {
            s->dsrval.ptr.p_double[i] = xy->ptr.pp_double[i][nvars];
        }
    }
}


/*************************************************************************
This function sets number  of  variables  (in  [1,NVars]  range)  used  by
decision forest construction algorithm.

The default option is to use roughly sqrt(NVars) variables.

INPUT PARAMETERS:
    S           -   decision forest builder object
    RndVars     -   number of randomly selected variables; values  outside
                    of [1,NVars] range are silently clipped.

OUTPUT PARAMETERS:
    S           -   decision forest builder

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
void dfbuildersetrndvars(decisionforestbuilder* s,
     ae_int_t rndvars,
     ae_state *_state)
{


    s->rdfvars = (double)(ae_maxint(rndvars, 1, _state));
}


/*************************************************************************
This function sets number of variables used by decision forest construction
algorithm as a fraction of total variable count (0,1) range.

The default option is to use roughly sqrt(NVars) variables.

INPUT PARAMETERS:
    S           -   decision forest builder object
    F           -   round(NVars*F) variables are selected

OUTPUT PARAMETERS:
    S           -   decision forest builder

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
void dfbuildersetrndvarsratio(decisionforestbuilder* s,
     double f,
     ae_state *_state)
{


    ae_assert(ae_isfinite(f, _state), "dfbuildersetrndvarsratio: F is INF or NAN", _state);
    s->rdfvars = -ae_maxreal(f, ae_machineepsilon, _state);
}


/*************************************************************************
This function tells decision forest builder to automatically choose number
of  variables  used  by  decision forest construction  algorithm.  Roughly
sqrt(NVars) variables will be used.

INPUT PARAMETERS:
    S           -   decision forest builder object

OUTPUT PARAMETERS:
    S           -   decision forest builder

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
void dfbuildersetrndvarsauto(decisionforestbuilder* s, ae_state *_state)
{


    s->rdfvars = (double)(0);
}


/*************************************************************************
This function sets size of dataset subsample generated the decision forest
construction algorithm. Size is specified as a fraction of  total  dataset
size.

The default option is to use 50% of the dataset for training, 50% for  the
OOB estimates. You can decrease fraction F down to 10%, 1% or  even  below
in order to reduce overfitting.

INPUT PARAMETERS:
    S           -   decision forest builder object
    F           -   fraction of the dataset to use, in (0,1] range. Values
                    outside of this range will  be  silently  clipped.  At
                    least one element is always selected for the  training
                    set.

OUTPUT PARAMETERS:
    S           -   decision forest builder

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
void dfbuildersetsubsampleratio(decisionforestbuilder* s,
     double f,
     ae_state *_state)
{


    ae_assert(ae_isfinite(f, _state), "dfbuildersetrndvarsfraction: F is INF or NAN", _state);
    s->rdfratio = ae_maxreal(f, ae_machineepsilon, _state);
}


/*************************************************************************
This function sets seed used by internal RNG for  random  subsampling  and
random selection of variable subsets.

By default random seed is used, i.e. every time you build decision forest,
we seed generator with new value  obtained  from  system-wide  RNG.  Thus,
decision forest builder returns non-deterministic results. You can  change
such behavior by specyfing fixed positive seed value.

INPUT PARAMETERS:
    S           -   decision forest builder object
    SeedVal     -   seed value:
                    * positive values are used for seeding RNG with fixed
                      seed, i.e. subsequent runs on same data will return
                      same decision forests
                    * non-positive seed means that random seed is used
                      for every run of builder, i.e. subsequent  runs  on
                      same  datasets  will  return   slightly   different
                      decision forests

OUTPUT PARAMETERS:
    S           -   decision forest builder, see

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
void dfbuildersetseed(decisionforestbuilder* s,
     ae_int_t seedval,
     ae_state *_state)
{


    s->rdfglobalseed = seedval;
}


/*************************************************************************
This function sets random decision forest construction algorithm.

As for now, only one decision forest construction algorithm is supported -
a dense "baseline" RDF algorithm.

INPUT PARAMETERS:
    S           -   decision forest builder object
    AlgoType    -   algorithm type:
                    * 0 = baseline dense RDF

OUTPUT PARAMETERS:
    S           -   decision forest builder, see

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
void dfbuildersetrdfalgo(decisionforestbuilder* s,
     ae_int_t algotype,
     ae_state *_state)
{


    ae_assert(algotype==0, "dfbuildersetrdfalgo: unexpected algotype", _state);
    s->rdfalgo = algotype;
}


/*************************************************************************
This  function  sets  split  selection  algorithm used by decision  forest
classifier. You may choose several algorithms, with  different  speed  and
quality of the results.

INPUT PARAMETERS:
    S           -   decision forest builder object
    SplitStrength-  split type:
                    * 0 = split at the random position, fastest one
                    * 1 = split at the middle of the range
                    * 2 = strong split at the best point of the range (default)

OUTPUT PARAMETERS:
    S           -   decision forest builder, see

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
void dfbuildersetrdfsplitstrength(decisionforestbuilder* s,
     ae_int_t splitstrength,
     ae_state *_state)
{


    ae_assert((splitstrength==0||splitstrength==1)||splitstrength==2, "dfbuildersetrdfsplitstrength: unexpected split type", _state);
    s->rdfsplitstrength = splitstrength;
}


/*************************************************************************
This  function  tells  decision  forest  construction  algorithm  to   use
Gini impurity based variable importance estimation (also known as MDI).

This version of importance estimation algorithm analyzes mean decrease  in
impurity (MDI) on training sample during  splits.  The result  is  divided
by impurity at the root node in order to produce estimate in [0,1] range.

Such estimates are fast to calculate and beautifully  normalized  (sum  to
one) but have following downsides:
* They ALWAYS sum to 1.0, even if output is completely unpredictable. I.e.
  MDI allows to order variables by importance, but does not  tell us about
  "absolute" importances of variables
* there exist some bias towards continuous and high-cardinality categorical
  variables
  
NOTE: informally speaking, MDA (permutation importance) rating answers the
      question  "what  part  of  the  model  predictive power is ruined by
      permuting k-th variable?" while MDI tells us "what part of the model
      predictive power was achieved due to usage of k-th variable".

      Thus, MDA rates each variable independently at "0 to 1"  scale while
      MDI (and OOB-MDI too) tends to divide "unit  amount  of  importance"
      between several important variables.
      
      If  all  variables  are  equally  important,  they  will  have  same
      MDI/OOB-MDI rating, equal (for OOB-MDI: roughly equal)  to  1/NVars.
      However, roughly  same  picture  will  be  produced   for  the  "all
      variables provide information no one is critical" situation  and for
      the "all variables are critical, drop any one, everything is ruined"
      situation.
      
      Contrary to that, MDA will rate critical variable as ~1.0 important,
      and important but non-critical variable will  have  less  than  unit
      rating.

NOTE: quite an often MDA and MDI return same results. It generally happens
      on problems with low test set error (a few  percents  at  most)  and
      large enough training set to avoid overfitting.
      
      The difference between MDA, MDI and OOB-MDI becomes  important  only
      on "hard" tasks with high test set error and/or small training set.

INPUT PARAMETERS:
    S           -   decision forest builder object

OUTPUT PARAMETERS:
    S           -   decision forest builder object. Next call to the forest
                    construction function will produce:
                    * importance estimates in rep.varimportances field
                    * variable ranks in rep.topvars field

  -- ALGLIB --
     Copyright 29.07.2019 by Bochkanov Sergey
*************************************************************************/
void dfbuildersetimportancetrngini(decisionforestbuilder* s,
     ae_state *_state)
{


    s->rdfimportance = dforest_needtrngini;
}


/*************************************************************************
This  function  tells  decision  forest  construction  algorithm  to   use
out-of-bag version of Gini variable importance estimation (also  known  as
OOB-MDI).

This version of importance estimation algorithm analyzes mean decrease  in
impurity (MDI) on out-of-bag sample during splits. The result  is  divided
by impurity at the root node in order to produce estimate in [0,1] range.

Such estimates are fast to calculate and resistant to  overfitting  issues
(thanks to the  out-of-bag  estimates  used). However, OOB Gini rating has
following downsides:
* there exist some bias towards continuous and high-cardinality categorical
  variables
* Gini rating allows us to order variables by importance, but it  is  hard
  to define importance of the variable by itself.
  
NOTE: informally speaking, MDA (permutation importance) rating answers the
      question  "what  part  of  the  model  predictive power is ruined by
      permuting k-th variable?" while MDI tells us "what part of the model
      predictive power was achieved due to usage of k-th variable".

      Thus, MDA rates each variable independently at "0 to 1"  scale while
      MDI (and OOB-MDI too) tends to divide "unit  amount  of  importance"
      between several important variables.
      
      If  all  variables  are  equally  important,  they  will  have  same
      MDI/OOB-MDI rating, equal (for OOB-MDI: roughly equal)  to  1/NVars.
      However, roughly  same  picture  will  be  produced   for  the  "all
      variables provide information no one is critical" situation  and for
      the "all variables are critical, drop any one, everything is ruined"
      situation.
      
      Contrary to that, MDA will rate critical variable as ~1.0 important,
      and important but non-critical variable will  have  less  than  unit
      rating.

NOTE: quite an often MDA and MDI return same results. It generally happens
      on problems with low test set error (a few  percents  at  most)  and
      large enough training set to avoid overfitting.
      
      The difference between MDA, MDI and OOB-MDI becomes  important  only
      on "hard" tasks with high test set error and/or small training set.

INPUT PARAMETERS:
    S           -   decision forest builder object

OUTPUT PARAMETERS:
    S           -   decision forest builder object. Next call to the forest
                    construction function will produce:
                    * importance estimates in rep.varimportances field
                    * variable ranks in rep.topvars field

  -- ALGLIB --
     Copyright 29.07.2019 by Bochkanov Sergey
*************************************************************************/
void dfbuildersetimportanceoobgini(decisionforestbuilder* s,
     ae_state *_state)
{


    s->rdfimportance = dforest_needoobgini;
}


/*************************************************************************
This  function  tells  decision  forest  construction  algorithm  to   use
permutation variable importance estimator (also known as MDA).

This version of importance estimation algorithm analyzes mean increase  in
out-of-bag sum of squared  residuals  after  random  permutation  of  J-th
variable. The result is divided by error computed with all variables being
perturbed in order to produce R-squared-like estimate in [0,1] range.

Such estimate  is  slower to calculate than Gini-based rating  because  it
needs multiple inference runs for each of variables being studied.

ALGLIB uses parallelized and highly  optimized  algorithm  which  analyzes
path through the decision tree and allows  to  handle  most  perturbations
in O(1) time; nevertheless, requesting MDA importances may increase forest
construction time from 10% to 200% (or more,  if  you  have  thousands  of
variables).

However, MDA rating has following benefits over Gini-based ones:
* no bias towards specific variable types
* ability to directly evaluate "absolute" importance of some  variable  at
  "0 to 1" scale (contrary to Gini-based rating, which returns comparative
  importances).
  
NOTE: informally speaking, MDA (permutation importance) rating answers the
      question  "what  part  of  the  model  predictive power is ruined by
      permuting k-th variable?" while MDI tells us "what part of the model
      predictive power was achieved due to usage of k-th variable".

      Thus, MDA rates each variable independently at "0 to 1"  scale while
      MDI (and OOB-MDI too) tends to divide "unit  amount  of  importance"
      between several important variables.
      
      If  all  variables  are  equally  important,  they  will  have  same
      MDI/OOB-MDI rating, equal (for OOB-MDI: roughly equal)  to  1/NVars.
      However, roughly  same  picture  will  be  produced   for  the  "all
      variables provide information no one is critical" situation  and for
      the "all variables are critical, drop any one, everything is ruined"
      situation.
      
      Contrary to that, MDA will rate critical variable as ~1.0 important,
      and important but non-critical variable will  have  less  than  unit
      rating.

NOTE: quite an often MDA and MDI return same results. It generally happens
      on problems with low test set error (a few  percents  at  most)  and
      large enough training set to avoid overfitting.
      
      The difference between MDA, MDI and OOB-MDI becomes  important  only
      on "hard" tasks with high test set error and/or small training set.

INPUT PARAMETERS:
    S           -   decision forest builder object

OUTPUT PARAMETERS:
    S           -   decision forest builder object. Next call to the forest
                    construction function will produce:
                    * importance estimates in rep.varimportances field
                    * variable ranks in rep.topvars field

  -- ALGLIB --
     Copyright 29.07.2019 by Bochkanov Sergey
*************************************************************************/
void dfbuildersetimportancepermutation(decisionforestbuilder* s,
     ae_state *_state)
{


    s->rdfimportance = dforest_needpermutation;
}


/*************************************************************************
This  function  tells  decision  forest  construction  algorithm  to  skip
variable importance estimation.

INPUT PARAMETERS:
    S           -   decision forest builder object

OUTPUT PARAMETERS:
    S           -   decision forest builder object. Next call to the forest
                    construction function will result in forest being built
                    without variable importance estimation.

  -- ALGLIB --
     Copyright 29.07.2019 by Bochkanov Sergey
*************************************************************************/
void dfbuildersetimportancenone(decisionforestbuilder* s,
     ae_state *_state)
{


    s->rdfimportance = 0;
}


/*************************************************************************
This function is an alias for dfbuilderpeekprogress(), left in ALGLIB  for
backward compatibility reasons.

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
double dfbuildergetprogress(const decisionforestbuilder* s,
     ae_state *_state)
{
    double result;


    result = dfbuilderpeekprogress(s, _state);
    return result;
}


/*************************************************************************
This function is used to peek into  decision  forest  construction process
from some other thread and get current progress indicator.

It returns value in [0,1].

INPUT PARAMETERS:
    S           -   decision forest builder object used  to  build  forest
                    in some other thread

RESULT:
    progress value, in [0,1]

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
double dfbuilderpeekprogress(const decisionforestbuilder* s,
     ae_state *_state)
{
    double result;


    result = (double)s->rdfprogress/ae_maxreal((double)(s->rdftotal), (double)(1), _state);
    result = ae_maxreal(result, (double)(0), _state);
    result = ae_minreal(result, (double)(1), _state);
    return result;
}


/*************************************************************************
This subroutine builds decision forest according to current settings using
dataset internally stored in the builder object. Dense algorithm is used.

NOTE: this   function   uses   dense  algorithm  for  forest  construction
      independently from the dataset format (dense or sparse).
  
NOTE: forest built with this function is  stored  in-memory  using  64-bit
      data structures for offsets/indexes/split values. It is possible  to
      convert  forest  into  more  memory-efficient   compressed    binary
      representation.  Depending  on  the  problem  properties,  3.7x-5.7x
      compression factors are possible.
      
      The downsides of compression are (a) slight reduction in  the  model
      accuracy and (b) ~1.5x reduction in  the  inference  speed  (due  to
      increased complexity of the storage format).
      
      See comments on dfbinarycompression() for more info.

Default settings are used by the algorithm; you can tweak  them  with  the
help of the following functions:
* dfbuildersetrfactor() - to control a fraction of the  dataset  used  for
  subsampling
* dfbuildersetrandomvars() - to control number of variables randomly chosen
  for decision rule creation

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
    S           -   decision forest builder object
    NTrees      -   NTrees>=1, number of trees to train

OUTPUT PARAMETERS:
    DF          -   decision forest. You can compress this forest to  more
                    compact 16-bit representation with dfbinarycompression()
    Rep         -   report, see below for information on its fields.
    
=== report information produced by forest construction function ==========

Decision forest training report includes following information:
* training set errors
* out-of-bag estimates of errors
* variable importance ratings

Following fields are used to store information:
* training set errors are stored in rep.relclserror, rep.avgce, rep.rmserror,
  rep.avgerror and rep.avgrelerror
* out-of-bag estimates of errors are stored in rep.oobrelclserror, rep.oobavgce,
  rep.oobrmserror, rep.oobavgerror and rep.oobavgrelerror

Variable importance reports, if requested by dfbuildersetimportancegini(),
dfbuildersetimportancetrngini() or dfbuildersetimportancepermutation()
call, are stored in:
* rep.varimportances field stores importance ratings
* rep.topvars stores variable indexes ordered from the most important to
  less important ones

You can find more information about report fields in:
* comments on dfreport structure
* comments on dfbuildersetimportancegini function
* comments on dfbuildersetimportancetrngini function
* comments on dfbuildersetimportancepermutation function

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
void dfbuilderbuildrandomforest(decisionforestbuilder* s,
     ae_int_t ntrees,
     decisionforest* df,
     dfreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t nvars;
    ae_int_t nclasses;
    ae_int_t npoints;
    ae_int_t trnsize;
    ae_int_t maxtreesize;
    ae_int_t sessionseed;
    dfworkbuf workbufseed;
    dfvotebuf votebufseed;
    dftreebuf treebufseed;

    ae_frame_make(_state, &_frame_block);
    memset(&workbufseed, 0, sizeof(workbufseed));
    memset(&votebufseed, 0, sizeof(votebufseed));
    memset(&treebufseed, 0, sizeof(treebufseed));
    _decisionforest_clear(df);
    _dfreport_clear(rep);
    _dfworkbuf_init(&workbufseed, _state, ae_true);
    _dfvotebuf_init(&votebufseed, _state, ae_true);
    _dftreebuf_init(&treebufseed, _state, ae_true);

    ae_assert(ntrees>=1, "DFBuilderBuildRandomForest: ntrees<1", _state);
    dforest_cleanreport(s, rep, _state);
    npoints = s->npoints;
    nvars = s->nvars;
    nclasses = s->nclasses;
    
    /*
     * Set up progress counter
     */
    s->rdfprogress = 0;
    s->rdftotal = ntrees*npoints;
    if( s->rdfimportance==dforest_needpermutation )
    {
        s->rdftotal = s->rdftotal+ntrees*npoints;
    }
    
    /*
     * Quick exit for empty dataset
     */
    if( s->dstype==-1||npoints==0 )
    {
        ae_assert(dforest_leafnodewidth==2, "DFBuilderBuildRandomForest: integrity check failed", _state);
        df->forestformat = dforest_dfuncompressedv0;
        df->nvars = s->nvars;
        df->nclasses = s->nclasses;
        df->ntrees = 1;
        df->bufsize = 1+dforest_leafnodewidth;
        ae_vector_set_length(&df->trees, 1+dforest_leafnodewidth, _state);
        df->trees.ptr.p_double[0] = (double)(1+dforest_leafnodewidth);
        df->trees.ptr.p_double[1] = (double)(-1);
        df->trees.ptr.p_double[2] = 0.0;
        dfcreatebuffer(df, &df->buffer, _state);
        ae_frame_leave(_state);
        return;
    }
    ae_assert(npoints>0, "DFBuilderBuildRandomForest: integrity check failed", _state);
    
    /*
     * Analyze dataset statistics, perform preprocessing
     */
    dforest_analyzeandpreprocessdataset(s, _state);
    
    /*
     * Prepare "work", "vote" and "tree" pools and other settings
     */
    trnsize = ae_round((double)npoints*s->rdfratio, _state);
    trnsize = ae_maxint(trnsize, 1, _state);
    trnsize = ae_minint(trnsize, npoints, _state);
    maxtreesize = 1+dforest_innernodewidth*(trnsize-1)+dforest_leafnodewidth*trnsize;
    ae_vector_set_length(&workbufseed.varpool, nvars, _state);
    ae_vector_set_length(&workbufseed.trnset, trnsize, _state);
    ae_vector_set_length(&workbufseed.oobset, npoints-trnsize, _state);
    ae_vector_set_length(&workbufseed.tmp0i, npoints, _state);
    ae_vector_set_length(&workbufseed.tmp1i, npoints, _state);
    ae_vector_set_length(&workbufseed.tmp0r, npoints, _state);
    ae_vector_set_length(&workbufseed.tmp1r, npoints, _state);
    ae_vector_set_length(&workbufseed.tmp2r, npoints, _state);
    ae_vector_set_length(&workbufseed.tmp3r, npoints, _state);
    ae_vector_set_length(&workbufseed.trnlabelsi, npoints, _state);
    ae_vector_set_length(&workbufseed.trnlabelsr, npoints, _state);
    ae_vector_set_length(&workbufseed.ooblabelsi, npoints, _state);
    ae_vector_set_length(&workbufseed.ooblabelsr, npoints, _state);
    ae_vector_set_length(&workbufseed.curvals, npoints, _state);
    ae_vector_set_length(&workbufseed.bestvals, npoints, _state);
    ae_vector_set_length(&workbufseed.classpriors, nclasses, _state);
    ae_vector_set_length(&workbufseed.classtotals0, nclasses, _state);
    ae_vector_set_length(&workbufseed.classtotals1, nclasses, _state);
    ae_vector_set_length(&workbufseed.classtotals01, 2*nclasses, _state);
    ae_vector_set_length(&workbufseed.treebuf, maxtreesize, _state);
    workbufseed.trnsize = trnsize;
    workbufseed.oobsize = npoints-trnsize;
    ae_vector_set_length(&votebufseed.trntotals, npoints*nclasses, _state);
    ae_vector_set_length(&votebufseed.oobtotals, npoints*nclasses, _state);
    for(i=0; i<=npoints*nclasses-1; i++)
    {
        votebufseed.trntotals.ptr.p_double[i] = (double)(0);
        votebufseed.oobtotals.ptr.p_double[i] = (double)(0);
    }
    ae_vector_set_length(&votebufseed.trncounts, npoints, _state);
    ae_vector_set_length(&votebufseed.oobcounts, npoints, _state);
    for(i=0; i<=npoints-1; i++)
    {
        votebufseed.trncounts.ptr.p_int[i] = 0;
        votebufseed.oobcounts.ptr.p_int[i] = 0;
    }
    ae_vector_set_length(&votebufseed.giniimportances, nvars, _state);
    for(i=0; i<=nvars-1; i++)
    {
        votebufseed.giniimportances.ptr.p_double[i] = 0.0;
    }
    treebufseed.treeidx = -1;
    ae_shared_pool_set_seed(&s->workpool, &workbufseed, (ae_int_t)sizeof(workbufseed), (ae_constructor)_dfworkbuf_init, (ae_copy_constructor)_dfworkbuf_init_copy, (ae_destructor)_dfworkbuf_destroy, _state);
    ae_shared_pool_set_seed(&s->votepool, &votebufseed, (ae_int_t)sizeof(votebufseed), (ae_constructor)_dfvotebuf_init, (ae_copy_constructor)_dfvotebuf_init_copy, (ae_destructor)_dfvotebuf_destroy, _state);
    ae_shared_pool_set_seed(&s->treepool, &treebufseed, (ae_int_t)sizeof(treebufseed), (ae_constructor)_dftreebuf_init, (ae_copy_constructor)_dftreebuf_init_copy, (ae_destructor)_dftreebuf_destroy, _state);
    ae_shared_pool_set_seed(&s->treefactory, &treebufseed, (ae_int_t)sizeof(treebufseed), (ae_constructor)_dftreebuf_init, (ae_copy_constructor)_dftreebuf_init_copy, (ae_destructor)_dftreebuf_destroy, _state);
    
    /*
     * Select session seed (individual trees are constructed using
     * combination of session and local seeds).
     */
    sessionseed = s->rdfglobalseed;
    if( s->rdfglobalseed<=0 )
    {
        sessionseed = ae_randominteger(30000, _state);
    }
    
    /*
     * Prepare In-and-Out-of-Bag matrix, if needed
     */
    s->neediobmatrix = s->rdfimportance==dforest_needpermutation;
    if( s->neediobmatrix )
    {
        
        /*
         * Prepare default state of In-and-Out-of-Bag matrix
         */
        bmatrixsetlengthatleast(&s->iobmatrix, ntrees, npoints, _state);
        for(i=0; i<=ntrees-1; i++)
        {
            for(j=0; j<=npoints-1; j++)
            {
                s->iobmatrix.ptr.pp_bool[i][j] = ae_false;
            }
        }
    }
    
    /*
     * Build trees (in parallel, if possible)
     */
    dforest_buildrandomtree(s, 0, ntrees, _state);
    
    /*
     * Merge trees and output result
     */
    dforest_mergetrees(s, df, _state);
    
    /*
     * Process voting results and output training set and OOB errors.
     * Finalize tree construction.
     */
    dforest_processvotingresults(s, ntrees, &votebufseed, rep, _state);
    dfcreatebuffer(df, &df->buffer, _state);
    
    /*
     * Perform variable importance estimation
     */
    dforest_estimatevariableimportance(s, sessionseed, df, ntrees, rep, _state);
    
    /*
     * Update progress counter
     */
    s->rdfprogress = s->rdftotal;
    ae_frame_leave(_state);
}


/*************************************************************************
This function performs binary compression of the decision forest.

Original decision forest produced by the  forest  builder  is stored using
64-bit representation for all numbers - offsets, variable  indexes,  split
points.

It is possible to significantly reduce model size by means of:
* using compressed  dynamic encoding for integers  (offsets  and  variable
  indexes), which uses just 1 byte to store small ints  (less  than  128),
  just 2 bytes for larger values (less than 128^2) and so on
* storing floating point numbers using 8-bit exponent and 16-bit mantissa

As  result,  model  needs  significantly  less  memory (compression factor
depends on  variable and class counts). In particular:
* NVars<128   and NClasses<128 result in 4.4x-5.7x model size reduction
* NVars<16384 and NClasses<128 result in 3.7x-4.5x model size reduction

Such storage format performs lossless compression  of  all  integers,  but
compression of floating point values (split values) is lossy, with roughly
0.01% relative error introduced during rounding. Thus, we recommend you to
re-evaluate model accuracy after compression.

Another downside  of  compression  is  ~1.5x reduction  in  the  inference
speed due to necessity of dynamic decompression of the compressed model.

INPUT PARAMETERS:
    DF      -   decision forest built by forest builder

OUTPUT PARAMETERS:
    DF      -   replaced by compressed forest

RESULT:
    compression factor (in-RAM size of the compressed model vs than of the
    uncompressed one), positive number larger than 1.0

  -- ALGLIB --
     Copyright 22.07.2019 by Bochkanov Sergey
*************************************************************************/
double dfbinarycompression(decisionforest* df, ae_state *_state)
{
    double result;


    result = dforest_binarycompression(df, ae_false, _state);
    return result;
}


/*************************************************************************
This is a 8-bit version of dfbinarycompression.
Not recommended for external use because it is too lossy.

  -- ALGLIB --
     Copyright 22.07.2019 by Bochkanov Sergey
*************************************************************************/
double dfbinarycompression8(decisionforest* df, ae_state *_state)
{
    double result;


    result = dforest_binarycompression(df, ae_true, _state);
    return result;
}


/*************************************************************************
Inference using decision forest

IMPORTANT: this  function  is  thread-unsafe  and  may   modify   internal
           structures of the model! You can not use same model  object for
           parallel evaluation from several threads.
           
           Use dftsprocess()  with  independent  thread-local  buffers  if
           you need thread-safe evaluation.

INPUT PARAMETERS:
    DF      -   decision forest model
    X       -   input vector,  array[NVars]
    Y       -   possibly preallocated buffer, reallocated if too small

OUTPUT PARAMETERS:
    Y       -   result. Regression estimate when solving regression  task,
                vector of posterior probabilities for classification task.

See also DFProcessI.
      

  -- ALGLIB --
     Copyright 16.02.2009 by Bochkanov Sergey
*************************************************************************/
void dfprocess(const decisionforest* df,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t offs;
    ae_int_t i;
    double v;
    ae_int_t treesize;
    ae_bool processed;


    
    /*
     * Process
     *
     * Although comments above warn you about thread-unsafety of this
     * function, it is de facto thread-safe. However, thread safety is
     * an accidental side-effect of the specific inference algorithm
     * being used. It may disappear in the future versions of the DF
     * models, so you should NOT rely on it.
     */
    if( y->cnt<df->nclasses )
    {
        ae_vector_set_length(y, df->nclasses, _state);
    }
    for(i=0; i<=df->nclasses-1; i++)
    {
        y->ptr.p_double[i] = (double)(0);
    }
    processed = ae_false;
    if( df->forestformat==dforest_dfuncompressedv0 )
    {
        
        /*
         * Process trees stored in uncompressed format
         */
        offs = 0;
        for(i=0; i<=df->ntrees-1; i++)
        {
            dforest_dfprocessinternaluncompressed(df, offs, offs+1, x, y, _state);
            offs = offs+ae_round(df->trees.ptr.p_double[offs], _state);
        }
        processed = ae_true;
    }
    if( df->forestformat==dforest_dfcompressedv0 )
    {
        
        /*
         * Process trees stored in compressed format
         */
        offs = 0;
        for(i=0; i<=df->ntrees-1; i++)
        {
            treesize = dforest_unstreamuint(&df->trees8, &offs, _state);
            dforest_dfprocessinternalcompressed(df, offs, x, y, _state);
            offs = offs+treesize;
        }
        processed = ae_true;
    }
    ae_assert(processed, "DFProcess: integrity check failed (unexpected format?)", _state);
    v = (double)1/(double)df->ntrees;
    ae_v_muld(&y->ptr.p_double[0], 1, ae_v_len(0,df->nclasses-1), v);
}


/*************************************************************************
'interactive' variant of DFProcess for languages like Python which support
constructs like "Y = DFProcessI(DF,X)" and interactive mode of interpreter

This function allocates new array on each call,  so  it  is  significantly
slower than its 'non-interactive' counterpart, but it is  more  convenient
when you call it from command line.

IMPORTANT: this  function  is  thread-unsafe  and  may   modify   internal
           structures of the model! You can not use same model  object for
           parallel evaluation from several threads.
           
           Use dftsprocess()  with  independent  thread-local  buffers  if
           you need thread-safe evaluation.

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void dfprocessi(const decisionforest* df,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{

    ae_vector_clear(y);

    dfprocess(df, x, y, _state);
}


/*************************************************************************
This function returns first component of the  inferred  vector  (i.e.  one
with index #0).

It is a convenience wrapper for dfprocess() intended for either:
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
           
           Use dftsprocess() with  independent  thread-local  buffers,  if
           you need thread-safe evaluation.

INPUT PARAMETERS:
    Model   -   DF model
    X       -   input vector,  array[0..NVars-1].

RESULT:
    Y[0]

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
double dfprocess0(decisionforest* model,
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
    dfprocess(model, &model->buffer.x, &model->buffer.y, _state);
    result = model->buffer.y.ptr.p_double[0];
    return result;
}


/*************************************************************************
This function returns most probable class number for an  input  X.  It  is
same as calling  dfprocess(model,x,y), then determining i=argmax(y[i]) and
returning i.

A class number in [0,NOut) range in returned for classification  problems,
-1 is returned when this function is called for regression problems.

IMPORTANT: this function is thread-unsafe and modifies internal structures
           of the model! You can not use same model  object  for  parallel
           evaluation from several threads.
           
           Use dftsprocess()  with independent  thread-local  buffers,  if
           you need thread-safe evaluation.

INPUT PARAMETERS:
    Model   -   decision forest model
    X       -   input vector,  array[0..NVars-1].

RESULT:
    class number, -1 for regression tasks

  -- ALGLIB --
     Copyright 15.02.2019 by Bochkanov Sergey
*************************************************************************/
ae_int_t dfclassify(decisionforest* model,
     /* Real    */ const ae_vector* x,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t nvars;
    ae_int_t nout;
    ae_int_t result;


    if( model->nclasses<2 )
    {
        result = -1;
        return result;
    }
    nvars = model->nvars;
    nout = model->nclasses;
    for(i=0; i<=nvars-1; i++)
    {
        model->buffer.x.ptr.p_double[i] = x->ptr.p_double[i];
    }
    dfprocess(model, &model->buffer.x, &model->buffer.y, _state);
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
Inference using decision forest

Thread-safe procesing using external buffer for temporaries.

This function is thread-safe (i.e .  you  can  use  same  DF   model  from
multiple threads) as long as you use different buffer objects for different
threads.

INPUT PARAMETERS:
    DF      -   decision forest model
    Buf     -   buffer object, must be  allocated  specifically  for  this
                model with dfcreatebuffer().
    X       -   input vector,  array[NVars]
    Y       -   possibly preallocated buffer, reallocated if too small

OUTPUT PARAMETERS:
    Y       -   result. Regression estimate when solving regression  task,
                vector of posterior probabilities for classification task.

See also DFProcessI.
      

  -- ALGLIB --
     Copyright 16.02.2009 by Bochkanov Sergey
*************************************************************************/
void dftsprocess(const decisionforest* df,
     decisionforestbuffer* buf,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{


    
    /*
     * Although docs warn you about thread-unsafety of the dfprocess()
     * function, it is de facto thread-safe. However, thread safety is
     * an accidental side-effect of the specific inference algorithm
     * being used. It may disappear in the future versions of the DF
     * models, so you should NOT rely on it.
     */
    dfprocess(df, x, y, _state);
}


/*************************************************************************
Relative classification error on the test set

INPUT PARAMETERS:
    DF      -   decision forest model
    XY      -   test set
    NPoints -   test set size

RESULT:
    percent of incorrectly classified cases.
    Zero if model solves regression task.

  -- ALGLIB --
     Copyright 16.02.2009 by Bochkanov Sergey
*************************************************************************/
double dfrelclserror(const decisionforest* df,
     /* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_state *_state)
{
    double result;


    result = (double)dforest_dfclserror(df, xy, npoints, _state)/(double)npoints;
    return result;
}


/*************************************************************************
Average cross-entropy (in bits per element) on the test set

INPUT PARAMETERS:
    DF      -   decision forest model
    XY      -   test set
    NPoints -   test set size

RESULT:
    CrossEntropy/(NPoints*LN(2)).
    Zero if model solves regression task.

  -- ALGLIB --
     Copyright 16.02.2009 by Bochkanov Sergey
*************************************************************************/
double dfavgce(const decisionforest* df,
     /* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector x;
    ae_vector y;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t tmpi;
    double result;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&y, 0, sizeof(y));
    ae_vector_init(&x, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&y, 0, DT_REAL, _state, ae_true);

    ae_vector_set_length(&x, df->nvars-1+1, _state);
    ae_vector_set_length(&y, df->nclasses-1+1, _state);
    result = (double)(0);
    for(i=0; i<=npoints-1; i++)
    {
        ae_v_move(&x.ptr.p_double[0], 1, &xy->ptr.pp_double[i][0], 1, ae_v_len(0,df->nvars-1));
        dfprocess(df, &x, &y, _state);
        if( df->nclasses>1 )
        {
            
            /*
             * classification-specific code
             */
            k = ae_round(xy->ptr.pp_double[i][df->nvars], _state);
            tmpi = 0;
            for(j=1; j<=df->nclasses-1; j++)
            {
                if( ae_fp_greater(y.ptr.p_double[j],y.ptr.p_double[tmpi]) )
                {
                    tmpi = j;
                }
            }
            if( ae_fp_neq(y.ptr.p_double[k],(double)(0)) )
            {
                result = result-ae_log(y.ptr.p_double[k], _state);
            }
            else
            {
                result = result-ae_log(ae_minrealnumber, _state);
            }
        }
    }
    result = result/(double)npoints;
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
RMS error on the test set

INPUT PARAMETERS:
    DF      -   decision forest model
    XY      -   test set
    NPoints -   test set size

RESULT:
    root mean square error.
    Its meaning for regression task is obvious. As for
    classification task, RMS error means error when estimating posterior
    probabilities.

  -- ALGLIB --
     Copyright 16.02.2009 by Bochkanov Sergey
*************************************************************************/
double dfrmserror(const decisionforest* df,
     /* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector x;
    ae_vector y;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t tmpi;
    double result;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&y, 0, sizeof(y));
    ae_vector_init(&x, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&y, 0, DT_REAL, _state, ae_true);

    ae_vector_set_length(&x, df->nvars-1+1, _state);
    ae_vector_set_length(&y, df->nclasses-1+1, _state);
    result = (double)(0);
    for(i=0; i<=npoints-1; i++)
    {
        ae_v_move(&x.ptr.p_double[0], 1, &xy->ptr.pp_double[i][0], 1, ae_v_len(0,df->nvars-1));
        dfprocess(df, &x, &y, _state);
        if( df->nclasses>1 )
        {
            
            /*
             * classification-specific code
             */
            k = ae_round(xy->ptr.pp_double[i][df->nvars], _state);
            tmpi = 0;
            for(j=1; j<=df->nclasses-1; j++)
            {
                if( ae_fp_greater(y.ptr.p_double[j],y.ptr.p_double[tmpi]) )
                {
                    tmpi = j;
                }
            }
            for(j=0; j<=df->nclasses-1; j++)
            {
                if( j==k )
                {
                    result = result+ae_sqr(y.ptr.p_double[j]-(double)1, _state);
                }
                else
                {
                    result = result+ae_sqr(y.ptr.p_double[j], _state);
                }
            }
        }
        else
        {
            
            /*
             * regression-specific code
             */
            result = result+ae_sqr(y.ptr.p_double[0]-xy->ptr.pp_double[i][df->nvars], _state);
        }
    }
    result = ae_sqrt(result/(double)(npoints*df->nclasses), _state);
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Average error on the test set

INPUT PARAMETERS:
    DF      -   decision forest model
    XY      -   test set
    NPoints -   test set size

RESULT:
    Its meaning for regression task is obvious. As for
    classification task, it means average error when estimating posterior
    probabilities.

  -- ALGLIB --
     Copyright 16.02.2009 by Bochkanov Sergey
*************************************************************************/
double dfavgerror(const decisionforest* df,
     /* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector x;
    ae_vector y;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double result;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&y, 0, sizeof(y));
    ae_vector_init(&x, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&y, 0, DT_REAL, _state, ae_true);

    ae_vector_set_length(&x, df->nvars-1+1, _state);
    ae_vector_set_length(&y, df->nclasses-1+1, _state);
    result = (double)(0);
    for(i=0; i<=npoints-1; i++)
    {
        ae_v_move(&x.ptr.p_double[0], 1, &xy->ptr.pp_double[i][0], 1, ae_v_len(0,df->nvars-1));
        dfprocess(df, &x, &y, _state);
        if( df->nclasses>1 )
        {
            
            /*
             * classification-specific code
             */
            k = ae_round(xy->ptr.pp_double[i][df->nvars], _state);
            for(j=0; j<=df->nclasses-1; j++)
            {
                if( j==k )
                {
                    result = result+ae_fabs(y.ptr.p_double[j]-(double)1, _state);
                }
                else
                {
                    result = result+ae_fabs(y.ptr.p_double[j], _state);
                }
            }
        }
        else
        {
            
            /*
             * regression-specific code
             */
            result = result+ae_fabs(y.ptr.p_double[0]-xy->ptr.pp_double[i][df->nvars], _state);
        }
    }
    result = result/(double)(npoints*df->nclasses);
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Average relative error on the test set

INPUT PARAMETERS:
    DF      -   decision forest model
    XY      -   test set
    NPoints -   test set size

RESULT:
    Its meaning for regression task is obvious. As for
    classification task, it means average relative error when estimating
    posterior probability of belonging to the correct class.

  -- ALGLIB --
     Copyright 16.02.2009 by Bochkanov Sergey
*************************************************************************/
double dfavgrelerror(const decisionforest* df,
     /* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector x;
    ae_vector y;
    ae_int_t relcnt;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double result;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&y, 0, sizeof(y));
    ae_vector_init(&x, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&y, 0, DT_REAL, _state, ae_true);

    ae_vector_set_length(&x, df->nvars-1+1, _state);
    ae_vector_set_length(&y, df->nclasses-1+1, _state);
    result = (double)(0);
    relcnt = 0;
    for(i=0; i<=npoints-1; i++)
    {
        ae_v_move(&x.ptr.p_double[0], 1, &xy->ptr.pp_double[i][0], 1, ae_v_len(0,df->nvars-1));
        dfprocess(df, &x, &y, _state);
        if( df->nclasses>1 )
        {
            
            /*
             * classification-specific code
             */
            k = ae_round(xy->ptr.pp_double[i][df->nvars], _state);
            for(j=0; j<=df->nclasses-1; j++)
            {
                if( j==k )
                {
                    result = result+ae_fabs(y.ptr.p_double[j]-(double)1, _state);
                    relcnt = relcnt+1;
                }
            }
        }
        else
        {
            
            /*
             * regression-specific code
             */
            if( ae_fp_neq(xy->ptr.pp_double[i][df->nvars],(double)(0)) )
            {
                result = result+ae_fabs((y.ptr.p_double[0]-xy->ptr.pp_double[i][df->nvars])/xy->ptr.pp_double[i][df->nvars], _state);
                relcnt = relcnt+1;
            }
        }
    }
    if( relcnt>0 )
    {
        result = result/(double)relcnt;
    }
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Copying of DecisionForest strucure

INPUT PARAMETERS:
    DF1 -   original

OUTPUT PARAMETERS:
    DF2 -   copy

  -- ALGLIB --
     Copyright 13.02.2009 by Bochkanov Sergey
*************************************************************************/
void dfcopy(const decisionforest* df1,
     decisionforest* df2,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t bufsize;

    _decisionforest_clear(df2);

    if( df1->forestformat==dforest_dfuncompressedv0 )
    {
        df2->forestformat = df1->forestformat;
        df2->nvars = df1->nvars;
        df2->nclasses = df1->nclasses;
        df2->ntrees = df1->ntrees;
        df2->bufsize = df1->bufsize;
        ae_vector_set_length(&df2->trees, df1->bufsize, _state);
        ae_v_move(&df2->trees.ptr.p_double[0], 1, &df1->trees.ptr.p_double[0], 1, ae_v_len(0,df1->bufsize-1));
        dfcreatebuffer(df2, &df2->buffer, _state);
        return;
    }
    if( df1->forestformat==dforest_dfcompressedv0 )
    {
        df2->forestformat = df1->forestformat;
        df2->usemantissa8 = df1->usemantissa8;
        df2->nvars = df1->nvars;
        df2->nclasses = df1->nclasses;
        df2->ntrees = df1->ntrees;
        bufsize = df1->trees8.cnt;
        ae_vector_set_length(&(df2->trees8), bufsize, _state);
        for(i=0; i<=bufsize-1; i++)
        {
            df2->trees8.ptr.p_ubyte[i] = (unsigned char)(df1->trees8.ptr.p_ubyte[i]);
        }
        dfcreatebuffer(df2, &df2->buffer, _state);
        return;
    }
    ae_assert(ae_false, "DFCopy: unexpected forest format", _state);
}


/*************************************************************************
Serializer: allocation

  -- ALGLIB --
     Copyright 14.03.2011 by Bochkanov Sergey
*************************************************************************/
void dfalloc(ae_serializer* s,
     const decisionforest* forest,
     ae_state *_state)
{


    if( forest->forestformat==dforest_dfuncompressedv0 )
    {
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        allocrealarray(s, &forest->trees, forest->bufsize, _state);
        return;
    }
    if( forest->forestformat==dforest_dfcompressedv0 )
    {
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_byte_array(s, &forest->trees8);
        return;
    }
    ae_assert(ae_false, "DFAlloc: unexpected forest format", _state);
}


/*************************************************************************
Serializer: serialization

  -- ALGLIB --
     Copyright 14.03.2011 by Bochkanov Sergey
*************************************************************************/
void dfserialize(ae_serializer* s,
     const decisionforest* forest,
     ae_state *_state)
{


    if( forest->forestformat==dforest_dfuncompressedv0 )
    {
        ae_serializer_serialize_int(s, getrdfserializationcode(_state), _state);
        ae_serializer_serialize_int(s, dforest_dfuncompressedv0, _state);
        ae_serializer_serialize_int(s, forest->nvars, _state);
        ae_serializer_serialize_int(s, forest->nclasses, _state);
        ae_serializer_serialize_int(s, forest->ntrees, _state);
        ae_serializer_serialize_int(s, forest->bufsize, _state);
        serializerealarray(s, &forest->trees, forest->bufsize, _state);
        return;
    }
    if( forest->forestformat==dforest_dfcompressedv0 )
    {
        ae_serializer_serialize_int(s, getrdfserializationcode(_state), _state);
        ae_serializer_serialize_int(s, forest->forestformat, _state);
        ae_serializer_serialize_bool(s, forest->usemantissa8, _state);
        ae_serializer_serialize_int(s, forest->nvars, _state);
        ae_serializer_serialize_int(s, forest->nclasses, _state);
        ae_serializer_serialize_int(s, forest->ntrees, _state);
        ae_serializer_serialize_byte_array(s, &forest->trees8, _state);
        return;
    }
    ae_assert(ae_false, "DFSerialize: unexpected forest format", _state);
}


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 14.03.2011 by Bochkanov Sergey
*************************************************************************/
void dfunserialize(ae_serializer* s,
     decisionforest* forest,
     ae_state *_state)
{
    ae_int_t i0;
    ae_int_t forestformat;
    ae_bool processed;

    _decisionforest_clear(forest);

    
    /*
     * check correctness of header
     */
    ae_serializer_unserialize_int(s, &i0, _state);
    ae_assert(i0==getrdfserializationcode(_state), "DFUnserialize: stream header corrupted", _state);
    
    /*
     * Read forest
     */
    ae_serializer_unserialize_int(s, &forestformat, _state);
    processed = ae_false;
    if( forestformat==dforest_dfuncompressedv0 )
    {
        
        /*
         * Unserialize data
         */
        forest->forestformat = forestformat;
        ae_serializer_unserialize_int(s, &forest->nvars, _state);
        ae_serializer_unserialize_int(s, &forest->nclasses, _state);
        ae_serializer_unserialize_int(s, &forest->ntrees, _state);
        ae_serializer_unserialize_int(s, &forest->bufsize, _state);
        unserializerealarray(s, &forest->trees, _state);
        processed = ae_true;
    }
    if( forestformat==dforest_dfcompressedv0 )
    {
        
        /*
         * Unserialize data
         */
        forest->forestformat = forestformat;
        ae_serializer_unserialize_bool(s, &forest->usemantissa8, _state);
        ae_serializer_unserialize_int(s, &forest->nvars, _state);
        ae_serializer_unserialize_int(s, &forest->nclasses, _state);
        ae_serializer_unserialize_int(s, &forest->ntrees, _state);
        ae_serializer_unserialize_byte_array(s, &forest->trees8, _state);
        processed = ae_true;
    }
    ae_assert(processed, "DFUnserialize: unexpected forest format", _state);
    
    /*
     * Prepare buffer
     */
    dfcreatebuffer(forest, &forest->buffer, _state);
}


/*************************************************************************
This subroutine builds random decision forest.

--------- DEPRECATED VERSION! USE DECISION FOREST BUILDER OBJECT ---------

  -- ALGLIB --
     Copyright 19.02.2009 by Bochkanov Sergey
*************************************************************************/
void dfbuildrandomdecisionforest(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nvars,
     ae_int_t nclasses,
     ae_int_t ntrees,
     double r,
     ae_int_t* info,
     decisionforest* df,
     dfreport* rep,
     ae_state *_state)
{
    ae_int_t samplesize;

    *info = 0;
    _decisionforest_clear(df);
    _dfreport_clear(rep);

    if( ae_fp_less_eq(r,(double)(0))||ae_fp_greater(r,(double)(1)) )
    {
        *info = -1;
        return;
    }
    samplesize = ae_maxint(ae_round(r*(double)npoints, _state), 1, _state);
    dfbuildinternal(xy, npoints, nvars, nclasses, ntrees, samplesize, ae_maxint(nvars/2, 1, _state), dforest_dfusestrongsplits+dforest_dfuseevs, info, df, rep, _state);
}


/*************************************************************************
This subroutine builds random decision forest.

--------- DEPRECATED VERSION! USE DECISION FOREST BUILDER OBJECT ---------

  -- ALGLIB --
     Copyright 19.02.2009 by Bochkanov Sergey
*************************************************************************/
void dfbuildrandomdecisionforestx1(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nvars,
     ae_int_t nclasses,
     ae_int_t ntrees,
     ae_int_t nrndvars,
     double r,
     ae_int_t* info,
     decisionforest* df,
     dfreport* rep,
     ae_state *_state)
{
    ae_int_t samplesize;

    *info = 0;
    _decisionforest_clear(df);
    _dfreport_clear(rep);

    if( ae_fp_less_eq(r,(double)(0))||ae_fp_greater(r,(double)(1)) )
    {
        *info = -1;
        return;
    }
    if( nrndvars<=0||nrndvars>nvars )
    {
        *info = -1;
        return;
    }
    samplesize = ae_maxint(ae_round(r*(double)npoints, _state), 1, _state);
    dfbuildinternal(xy, npoints, nvars, nclasses, ntrees, samplesize, nrndvars, dforest_dfusestrongsplits+dforest_dfuseevs, info, df, rep, _state);
}


void dfbuildinternal(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nvars,
     ae_int_t nclasses,
     ae_int_t ntrees,
     ae_int_t samplesize,
     ae_int_t nfeatures,
     ae_int_t flags,
     ae_int_t* info,
     decisionforest* df,
     dfreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    decisionforestbuilder builder;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&builder, 0, sizeof(builder));
    *info = 0;
    _decisionforest_clear(df);
    _dfreport_clear(rep);
    _decisionforestbuilder_init(&builder, _state, ae_true);

    
    /*
     * Test for inputs
     */
    if( (((((npoints<1||samplesize<1)||samplesize>npoints)||nvars<1)||nclasses<1)||ntrees<1)||nfeatures<1 )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    if( nclasses>1 )
    {
        for(i=0; i<=npoints-1; i++)
        {
            if( ae_round(xy->ptr.pp_double[i][nvars], _state)<0||ae_round(xy->ptr.pp_double[i][nvars], _state)>=nclasses )
            {
                *info = -2;
                ae_frame_leave(_state);
                return;
            }
        }
    }
    *info = 1;
    dfbuildercreate(&builder, _state);
    dfbuildersetdataset(&builder, xy, npoints, nvars, nclasses, _state);
    dfbuildersetsubsampleratio(&builder, (double)samplesize/(double)npoints, _state);
    dfbuildersetrndvars(&builder, nfeatures, _state);
    dfbuilderbuildrandomforest(&builder, ntrees, df, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Builds a range of random trees [TreeIdx0,TreeIdx1) using decision forest
algorithm. Tree index is used to seed per-tree RNG.

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
static void dforest_buildrandomtree(decisionforestbuilder* s,
     ae_int_t treeidx0,
     ae_int_t treeidx1,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t treeidx;
    ae_int_t i;
    ae_int_t j;
    ae_int_t npoints;
    ae_int_t nvars;
    ae_int_t nclasses;
    hqrndstate rs;
    dfworkbuf *workbuf;
    ae_smart_ptr _workbuf;
    dfvotebuf *votebuf;
    ae_smart_ptr _votebuf;
    dftreebuf *treebuf;
    ae_smart_ptr _treebuf;
    ae_int_t treesize;
    ae_int_t varstoselect;
    ae_int_t workingsetsize;
    double meanloss;

    ae_frame_make(_state, &_frame_block);
    memset(&rs, 0, sizeof(rs));
    memset(&_workbuf, 0, sizeof(_workbuf));
    memset(&_votebuf, 0, sizeof(_votebuf));
    memset(&_treebuf, 0, sizeof(_treebuf));
    _hqrndstate_init(&rs, _state, ae_true);
    ae_smart_ptr_init(&_workbuf, (void**)&workbuf, _state, ae_true);
    ae_smart_ptr_init(&_votebuf, (void**)&votebuf, _state, ae_true);
    ae_smart_ptr_init(&_treebuf, (void**)&treebuf, _state, ae_true);

    
    /*
     * Perform parallelization
     */
    if( treeidx1-treeidx0>1 )
    {
        if( _trypexec_dforest_buildrandomtree(s,treeidx0,treeidx1, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            ae_frame_leave(_state);
            return;
        }
        ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_true, _state);
        j = (treeidx1-treeidx0)/2;
        _spawn_dforest_buildrandomtree(s, treeidx0, treeidx0+j, _child_tasks, _smp_enabled, _state);
        dforest_buildrandomtree(s, treeidx0+j, treeidx1, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_false, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        ae_frame_leave(_state);
        return;
    }
    else
    {
        ae_assert(treeidx1-treeidx0==1, "RDF: integrity check failed", _state);
        treeidx = treeidx0;
    }
    
    /*
     * Prepare
     */
    npoints = s->npoints;
    nvars = s->nvars;
    nclasses = s->nclasses;
    if( s->rdfglobalseed>0 )
    {
        hqrndseed(s->rdfglobalseed, 1+treeidx, &rs, _state);
    }
    else
    {
        hqrndseed(ae_randominteger(30000, _state), 1+treeidx, &rs, _state);
    }
    
    /*
     * Retrieve buffers.
     */
    ae_shared_pool_retrieve(&s->workpool, &_workbuf, _state);
    ae_shared_pool_retrieve(&s->votepool, &_votebuf, _state);
    
    /*
     * Prepare everything for tree construction.
     */
    ae_assert(workbuf->trnsize>=1, "DForest: integrity check failed (34636)", _state);
    ae_assert(workbuf->oobsize>=0, "DForest: integrity check failed (45745)", _state);
    ae_assert(workbuf->trnsize+workbuf->oobsize==npoints, "DForest: integrity check failed (89415)", _state);
    workingsetsize = -1;
    workbuf->varpoolsize = 0;
    for(i=0; i<=nvars-1; i++)
    {
        if( ae_fp_neq(s->dsmin.ptr.p_double[i],s->dsmax.ptr.p_double[i]) )
        {
            workbuf->varpool.ptr.p_int[workbuf->varpoolsize] = i;
            inc(&workbuf->varpoolsize, _state);
        }
    }
    workingsetsize = workbuf->varpoolsize;
    ae_assert(workingsetsize>=0, "DForest: integrity check failed (73f5)", _state);
    for(i=0; i<=npoints-1; i++)
    {
        workbuf->tmp0i.ptr.p_int[i] = i;
    }
    for(i=0; i<=workbuf->trnsize-1; i++)
    {
        j = hqrnduniformi(&rs, npoints-i, _state);
        swapelementsi(&workbuf->tmp0i, i, i+j, _state);
        workbuf->trnset.ptr.p_int[i] = workbuf->tmp0i.ptr.p_int[i];
        if( nclasses>1 )
        {
            workbuf->trnlabelsi.ptr.p_int[i] = s->dsival.ptr.p_int[workbuf->tmp0i.ptr.p_int[i]];
        }
        else
        {
            workbuf->trnlabelsr.ptr.p_double[i] = s->dsrval.ptr.p_double[workbuf->tmp0i.ptr.p_int[i]];
        }
        if( s->neediobmatrix )
        {
            s->iobmatrix.ptr.pp_bool[treeidx][workbuf->trnset.ptr.p_int[i]] = ae_true;
        }
    }
    for(i=0; i<=workbuf->oobsize-1; i++)
    {
        j = workbuf->tmp0i.ptr.p_int[workbuf->trnsize+i];
        workbuf->oobset.ptr.p_int[i] = j;
        if( nclasses>1 )
        {
            workbuf->ooblabelsi.ptr.p_int[i] = s->dsival.ptr.p_int[j];
        }
        else
        {
            workbuf->ooblabelsr.ptr.p_double[i] = s->dsrval.ptr.p_double[j];
        }
    }
    varstoselect = ae_round(ae_sqrt((double)(nvars), _state), _state);
    if( ae_fp_greater(s->rdfvars,(double)(0)) )
    {
        varstoselect = ae_round(s->rdfvars, _state);
    }
    if( ae_fp_less(s->rdfvars,(double)(0)) )
    {
        varstoselect = ae_round(-(double)nvars*s->rdfvars, _state);
    }
    varstoselect = ae_maxint(varstoselect, 1, _state);
    varstoselect = ae_minint(varstoselect, nvars, _state);
    
    /*
     * Perform recurrent construction
     */
    if( s->rdfimportance==dforest_needtrngini )
    {
        meanloss = dforest_meannrms2(nclasses, &workbuf->trnlabelsi, &workbuf->trnlabelsr, 0, workbuf->trnsize, &workbuf->trnlabelsi, &workbuf->trnlabelsr, 0, workbuf->trnsize, &workbuf->tmpnrms2, _state);
    }
    else
    {
        meanloss = dforest_meannrms2(nclasses, &workbuf->trnlabelsi, &workbuf->trnlabelsr, 0, workbuf->trnsize, &workbuf->ooblabelsi, &workbuf->ooblabelsr, 0, workbuf->oobsize, &workbuf->tmpnrms2, _state);
    }
    treesize = 1;
    dforest_buildrandomtreerec(s, workbuf, workingsetsize, varstoselect, &workbuf->treebuf, votebuf, &rs, 0, workbuf->trnsize, 0, workbuf->oobsize, meanloss, meanloss, &treesize, _state);
    workbuf->treebuf.ptr.p_double[0] = (double)(treesize);
    
    /*
     * Store tree
     */
    ae_shared_pool_retrieve(&s->treefactory, &_treebuf, _state);
    ae_vector_set_length(&treebuf->treebuf, treesize, _state);
    for(i=0; i<=treesize-1; i++)
    {
        treebuf->treebuf.ptr.p_double[i] = workbuf->treebuf.ptr.p_double[i];
    }
    treebuf->treeidx = treeidx;
    ae_shared_pool_recycle(&s->treepool, &_treebuf, _state);
    
    /*
     * Return other buffers to appropriate pools
     */
    ae_shared_pool_recycle(&s->workpool, &_workbuf, _state);
    ae_shared_pool_recycle(&s->votepool, &_votebuf, _state);
    
    /*
     * Update progress indicator
     */
    threadunsafeincby(&s->rdfprogress, npoints, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_dforest_buildrandomtree(decisionforestbuilder* s,
    ae_int_t treeidx0,
    ae_int_t treeidx1,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        dforest_buildrandomtree(s,treeidx0,treeidx1, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_dforest_buildrandomtree;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = s;
        _task->data.parameters[1].value.ival = treeidx0;
        _task->data.parameters[2].value.ival = treeidx1;
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
void _task_dforest_buildrandomtree(ae_task_data *_data, ae_state *_state)
{
    decisionforestbuilder* s;
    ae_int_t treeidx0;
    ae_int_t treeidx1;
    s = (decisionforestbuilder*)_data->parameters[0].value.val;
    treeidx0 = _data->parameters[1].value.ival;
    treeidx1 = _data->parameters[2].value.ival;
   ae_state_set_flags(_state, _data->flags);
   dforest_buildrandomtree(s,treeidx0,treeidx1, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_dforest_buildrandomtree(decisionforestbuilder* s,
    ae_int_t treeidx0,
    ae_int_t treeidx1,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_dforest_buildrandomtree;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = s;
    _task->data.parameters[1].value.ival = treeidx0;
    _task->data.parameters[2].value.ival = treeidx1;
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
Recurrent tree construction function using  caller-allocated  buffers  and
caller-initialized RNG.

Following iterms are processed:
* items [Idx0,Idx1) of WorkBuf.TrnSet
* items [OOBIdx0, OOBIdx1) of WorkBuf.OOBSet

TreeSize on input must be 1 (header element of the tree), on output it
contains size of the tree.

OOBLoss on input must contain value of MeanNRMS2(...) computed for entire
dataset.

Variables from #0 to #WorkingSet-1 from WorkBuf.VarPool are used (for
block algorithm: blocks, not vars)

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
static void dforest_buildrandomtreerec(const decisionforestbuilder* s,
     dfworkbuf* workbuf,
     ae_int_t workingset,
     ae_int_t varstoselect,
     /* Real    */ ae_vector* treebuf,
     dfvotebuf* votebuf,
     hqrndstate* rs,
     ae_int_t idx0,
     ae_int_t idx1,
     ae_int_t oobidx0,
     ae_int_t oobidx1,
     double meanloss,
     double topmostmeanloss,
     ae_int_t* treesize,
     ae_state *_state)
{
    ae_int_t npoints;
    ae_int_t nclasses;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j0;
    double v;
    ae_bool labelsaresame;
    ae_int_t offs;
    ae_int_t varbest;
    double splitbest;
    ae_int_t i1;
    ae_int_t i2;
    ae_int_t idxtrn;
    ae_int_t idxoob;
    double meanloss0;
    double meanloss1;


    ae_assert(s->dstype==0, "not supported skbdgfsi!", _state);
    ae_assert(idx0<idx1, "BuildRandomTreeRec: integrity check failed (3445)", _state);
    ae_assert(oobidx0<=oobidx1, "BuildRandomTreeRec: integrity check failed (7452)", _state);
    npoints = s->npoints;
    nclasses = s->nclasses;
    
    /*
     * Check labels: all same or not?
     */
    if( nclasses>1 )
    {
        labelsaresame = ae_true;
        for(i=0; i<=nclasses-1; i++)
        {
            workbuf->classpriors.ptr.p_int[i] = 0;
        }
        j0 = workbuf->trnlabelsi.ptr.p_int[idx0];
        for(i=idx0; i<=idx1-1; i++)
        {
            j = workbuf->trnlabelsi.ptr.p_int[i];
            workbuf->classpriors.ptr.p_int[j] = workbuf->classpriors.ptr.p_int[j]+1;
            labelsaresame = labelsaresame&&j0==j;
        }
    }
    else
    {
        labelsaresame = ae_false;
    }
    
    /*
     * Leaf node
     */
    if( idx1-idx0==1||labelsaresame )
    {
        if( nclasses==1 )
        {
            dforest_outputleaf(s, workbuf, treebuf, votebuf, idx0, idx1, oobidx0, oobidx1, treesize, workbuf->trnlabelsr.ptr.p_double[idx0], _state);
        }
        else
        {
            dforest_outputleaf(s, workbuf, treebuf, votebuf, idx0, idx1, oobidx0, oobidx1, treesize, (double)(workbuf->trnlabelsi.ptr.p_int[idx0]), _state);
        }
        return;
    }
    
    /*
     * Non-leaf node.
     * Investigate possible splits.
     */
    ae_assert(s->rdfalgo==0, "BuildRandomForest: unexpected algo", _state);
    dforest_choosecurrentsplitdense(s, workbuf, &workingset, varstoselect, rs, idx0, idx1, &varbest, &splitbest, _state);
    if( varbest<0 )
    {
        
        /*
         * No good split was found; make leaf (label is randomly chosen) and exit.
         */
        if( nclasses>1 )
        {
            v = (double)(workbuf->trnlabelsi.ptr.p_int[idx0+hqrnduniformi(rs, idx1-idx0, _state)]);
        }
        else
        {
            v = workbuf->trnlabelsr.ptr.p_double[idx0+hqrnduniformi(rs, idx1-idx0, _state)];
        }
        dforest_outputleaf(s, workbuf, treebuf, votebuf, idx0, idx1, oobidx0, oobidx1, treesize, v, _state);
        return;
    }
    
    /*
     * Good split WAS found, we can perform it:
     * * first, we split training set
     * * then, we similarly split OOB set
     */
    ae_assert(s->dstype==0, "not supported 54bfdh", _state);
    offs = npoints*varbest;
    i1 = idx0;
    i2 = idx1-1;
    while(i1<=i2)
    {
        
        /*
         * Reorder indexes so that left partition is in [Idx0..I1),
         * and right partition is in [I2+1..Idx1)
         */
        if( workbuf->bestvals.ptr.p_double[i1]<splitbest )
        {
            i1 = i1+1;
            continue;
        }
        if( workbuf->bestvals.ptr.p_double[i2]>=splitbest )
        {
            i2 = i2-1;
            continue;
        }
        j = workbuf->trnset.ptr.p_int[i1];
        workbuf->trnset.ptr.p_int[i1] = workbuf->trnset.ptr.p_int[i2];
        workbuf->trnset.ptr.p_int[i2] = j;
        if( nclasses>1 )
        {
            j = workbuf->trnlabelsi.ptr.p_int[i1];
            workbuf->trnlabelsi.ptr.p_int[i1] = workbuf->trnlabelsi.ptr.p_int[i2];
            workbuf->trnlabelsi.ptr.p_int[i2] = j;
        }
        else
        {
            v = workbuf->trnlabelsr.ptr.p_double[i1];
            workbuf->trnlabelsr.ptr.p_double[i1] = workbuf->trnlabelsr.ptr.p_double[i2];
            workbuf->trnlabelsr.ptr.p_double[i2] = v;
        }
        i1 = i1+1;
        i2 = i2-1;
    }
    ae_assert(i1==i2+1, "BuildRandomTreeRec: integrity check failed (45rds3)", _state);
    idxtrn = i1;
    if( oobidx0<oobidx1 )
    {
        
        /*
         * Unlike the training subset, the out-of-bag subset corresponding to the
         * current sequence of decisions can be empty; thus, we have to explicitly
         * handle situation of zero OOB subset.
         */
        i1 = oobidx0;
        i2 = oobidx1-1;
        while(i1<=i2)
        {
            
            /*
             * Reorder indexes so that left partition is in [Idx0..I1),
             * and right partition is in [I2+1..Idx1)
             */
            if( s->dsdata.ptr.p_double[offs+workbuf->oobset.ptr.p_int[i1]]<splitbest )
            {
                i1 = i1+1;
                continue;
            }
            if( s->dsdata.ptr.p_double[offs+workbuf->oobset.ptr.p_int[i2]]>=splitbest )
            {
                i2 = i2-1;
                continue;
            }
            j = workbuf->oobset.ptr.p_int[i1];
            workbuf->oobset.ptr.p_int[i1] = workbuf->oobset.ptr.p_int[i2];
            workbuf->oobset.ptr.p_int[i2] = j;
            if( nclasses>1 )
            {
                j = workbuf->ooblabelsi.ptr.p_int[i1];
                workbuf->ooblabelsi.ptr.p_int[i1] = workbuf->ooblabelsi.ptr.p_int[i2];
                workbuf->ooblabelsi.ptr.p_int[i2] = j;
            }
            else
            {
                v = workbuf->ooblabelsr.ptr.p_double[i1];
                workbuf->ooblabelsr.ptr.p_double[i1] = workbuf->ooblabelsr.ptr.p_double[i2];
                workbuf->ooblabelsr.ptr.p_double[i2] = v;
            }
            i1 = i1+1;
            i2 = i2-1;
        }
        ae_assert(i1==i2+1, "BuildRandomTreeRec: integrity check failed (643fs3)", _state);
        idxoob = i1;
    }
    else
    {
        idxoob = oobidx0;
    }
    
    /*
     * Compute estimates of NRMS2 loss over TRN or OOB subsets, update Gini importances
     */
    if( s->rdfimportance==dforest_needtrngini )
    {
        meanloss0 = dforest_meannrms2(nclasses, &workbuf->trnlabelsi, &workbuf->trnlabelsr, idx0, idxtrn, &workbuf->trnlabelsi, &workbuf->trnlabelsr, idx0, idxtrn, &workbuf->tmpnrms2, _state);
        meanloss1 = dforest_meannrms2(nclasses, &workbuf->trnlabelsi, &workbuf->trnlabelsr, idxtrn, idx1, &workbuf->trnlabelsi, &workbuf->trnlabelsr, idxtrn, idx1, &workbuf->tmpnrms2, _state);
    }
    else
    {
        meanloss0 = dforest_meannrms2(nclasses, &workbuf->trnlabelsi, &workbuf->trnlabelsr, idx0, idxtrn, &workbuf->ooblabelsi, &workbuf->ooblabelsr, oobidx0, idxoob, &workbuf->tmpnrms2, _state);
        meanloss1 = dforest_meannrms2(nclasses, &workbuf->trnlabelsi, &workbuf->trnlabelsr, idxtrn, idx1, &workbuf->ooblabelsi, &workbuf->ooblabelsr, idxoob, oobidx1, &workbuf->tmpnrms2, _state);
    }
    votebuf->giniimportances.ptr.p_double[varbest] = votebuf->giniimportances.ptr.p_double[varbest]+(meanloss-(meanloss0+meanloss1))/(topmostmeanloss+1.0e-20);
    
    /*
     * Generate tree node and subtrees (recursively)
     */
    treebuf->ptr.p_double[*treesize] = (double)(varbest);
    treebuf->ptr.p_double[*treesize+1] = splitbest;
    i = *treesize;
    *treesize = *treesize+dforest_innernodewidth;
    dforest_buildrandomtreerec(s, workbuf, workingset, varstoselect, treebuf, votebuf, rs, idx0, idxtrn, oobidx0, idxoob, meanloss0, topmostmeanloss, treesize, _state);
    treebuf->ptr.p_double[i+2] = (double)(*treesize);
    dforest_buildrandomtreerec(s, workbuf, workingset, varstoselect, treebuf, votebuf, rs, idxtrn, idx1, idxoob, oobidx1, meanloss1, topmostmeanloss, treesize, _state);
}


/*************************************************************************
Estimates permutation variable importance ratings for a range of dataset
points.

Initial call to this function should span entire range of the dataset,
[Idx0,Idx1)=[0,NPoints), because function performs initialization of some
internal structures when called with these arguments.

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
static void dforest_estimatevariableimportance(decisionforestbuilder* s,
     ae_int_t sessionseed,
     const decisionforest* df,
     ae_int_t ntrees,
     dfreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t npoints;
    ae_int_t nvars;
    ae_int_t nclasses;
    ae_int_t nperm;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    dfvotebuf *vote;
    ae_smart_ptr _vote;
    ae_vector tmpr0;
    ae_vector tmpr1;
    ae_vector tmpi0;
    ae_vector losses;
    dfpermimpbuf permseed;
    dfpermimpbuf *permresult;
    ae_smart_ptr _permresult;
    ae_shared_pool permpool;
    double nopermloss;
    double totalpermloss;
    hqrndstate varimprs;

    ae_frame_make(_state, &_frame_block);
    memset(&_vote, 0, sizeof(_vote));
    memset(&tmpr0, 0, sizeof(tmpr0));
    memset(&tmpr1, 0, sizeof(tmpr1));
    memset(&tmpi0, 0, sizeof(tmpi0));
    memset(&losses, 0, sizeof(losses));
    memset(&permseed, 0, sizeof(permseed));
    memset(&_permresult, 0, sizeof(_permresult));
    memset(&permpool, 0, sizeof(permpool));
    memset(&varimprs, 0, sizeof(varimprs));
    ae_smart_ptr_init(&_vote, (void**)&vote, _state, ae_true);
    ae_vector_init(&tmpr0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmpr1, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmpi0, 0, DT_INT, _state, ae_true);
    ae_vector_init(&losses, 0, DT_REAL, _state, ae_true);
    _dfpermimpbuf_init(&permseed, _state, ae_true);
    ae_smart_ptr_init(&_permresult, (void**)&permresult, _state, ae_true);
    ae_shared_pool_init(&permpool, _state, ae_true);
    _hqrndstate_init(&varimprs, _state, ae_true);

    npoints = s->npoints;
    nvars = s->nvars;
    nclasses = s->nclasses;
    
    /*
     * No importance rating
     */
    if( s->rdfimportance==0 )
    {
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Gini importance
     */
    if( s->rdfimportance==dforest_needtrngini||s->rdfimportance==dforest_needoobgini )
    {
        
        /*
         * Merge OOB Gini importances computed during tree generation
         */
        ae_shared_pool_first_recycled(&s->votepool, &_vote, _state);
        while(vote!=NULL)
        {
            for(i=0; i<=nvars-1; i++)
            {
                rep->varimportances.ptr.p_double[i] = rep->varimportances.ptr.p_double[i]+vote->giniimportances.ptr.p_double[i]/(double)ntrees;
            }
            ae_shared_pool_next_recycled(&s->votepool, &_vote, _state);
        }
        for(i=0; i<=nvars-1; i++)
        {
            rep->varimportances.ptr.p_double[i] = boundval(rep->varimportances.ptr.p_double[i], (double)(0), (double)(1), _state);
        }
        
        /*
         * Compute topvars[] array
         */
        ae_vector_set_length(&tmpr0, nvars, _state);
        for(j=0; j<=nvars-1; j++)
        {
            tmpr0.ptr.p_double[j] = -rep->varimportances.ptr.p_double[j];
            rep->topvars.ptr.p_int[j] = j;
        }
        tagsortfasti(&tmpr0, &rep->topvars, &tmpr1, &tmpi0, nvars, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Permutation importance
     */
    if( s->rdfimportance==dforest_needpermutation )
    {
        ae_assert(df->forestformat==dforest_dfuncompressedv0, "EstimateVariableImportance: integrity check failed (ff)", _state);
        ae_assert(s->iobmatrix.rows>=ntrees&&s->iobmatrix.cols>=npoints, "EstimateVariableImportance: integrity check failed (IOB)", _state);
        
        /*
         * Generate packed representation of the shuffle which is applied to all variables
         *
         * Ideally we want to apply different permutations to different variables,
         * i.e. we have to generate and store NPoints*NVars random numbers.
         * However due to performance and memory restrictions we prefer to use compact
         * representation:
         * * we store one "reference" permutation P_ref in VarImpShuffle2[0:NPoints-1]
         * * a permutation P_j applied to variable J is obtained by circularly shifting
         *   elements in P_ref by VarImpShuffle2[NPoints+J]
         */
        hqrndseed(sessionseed, 1117, &varimprs, _state);
        ivectorsetlengthatleast(&s->varimpshuffle2, npoints+nvars, _state);
        for(i=0; i<=npoints-1; i++)
        {
            s->varimpshuffle2.ptr.p_int[i] = i;
        }
        for(i=0; i<=npoints-2; i++)
        {
            j = i+hqrnduniformi(&varimprs, npoints-i, _state);
            k = s->varimpshuffle2.ptr.p_int[i];
            s->varimpshuffle2.ptr.p_int[i] = s->varimpshuffle2.ptr.p_int[j];
            s->varimpshuffle2.ptr.p_int[j] = k;
        }
        for(i=0; i<=nvars-1; i++)
        {
            s->varimpshuffle2.ptr.p_int[npoints+i] = hqrnduniformi(&varimprs, npoints, _state);
        }
        
        /*
         * Prepare buffer object, seed pool
         */
        nperm = nvars+2;
        ae_vector_set_length(&permseed.losses, nperm, _state);
        for(j=0; j<=nperm-1; j++)
        {
            permseed.losses.ptr.p_double[j] = (double)(0);
        }
        ae_vector_set_length(&permseed.yv, nperm*nclasses, _state);
        ae_vector_set_length(&permseed.xraw, nvars, _state);
        ae_vector_set_length(&permseed.xdist, nvars, _state);
        ae_vector_set_length(&permseed.xcur, nvars, _state);
        ae_vector_set_length(&permseed.targety, nclasses, _state);
        ae_vector_set_length(&permseed.startnodes, nvars, _state);
        ae_vector_set_length(&permseed.y, nclasses, _state);
        ae_shared_pool_set_seed(&permpool, &permseed, (ae_int_t)sizeof(permseed), (ae_constructor)_dfpermimpbuf_init, (ae_copy_constructor)_dfpermimpbuf_init_copy, (ae_destructor)_dfpermimpbuf_destroy, _state);
        
        /*
         * Recursively split subset and process (using parallel capabilities, if possible)
         */
        dforest_estimatepermutationimportances(s, df, ntrees, &permpool, 0, npoints, _state);
        
        /*
         * Merge results
         */
        ae_vector_set_length(&losses, nperm, _state);
        for(j=0; j<=nperm-1; j++)
        {
            losses.ptr.p_double[j] = 1.0e-20;
        }
        ae_shared_pool_first_recycled(&permpool, &_permresult, _state);
        while(permresult!=NULL)
        {
            for(j=0; j<=nperm-1; j++)
            {
                losses.ptr.p_double[j] = losses.ptr.p_double[j]+permresult->losses.ptr.p_double[j];
            }
            ae_shared_pool_next_recycled(&permpool, &_permresult, _state);
        }
        
        /*
         * Compute importances
         */
        nopermloss = losses.ptr.p_double[nvars+1];
        totalpermloss = losses.ptr.p_double[nvars];
        for(i=0; i<=nvars-1; i++)
        {
            rep->varimportances.ptr.p_double[i] = (double)1-nopermloss/totalpermloss-((double)1-losses.ptr.p_double[i]/totalpermloss);
            rep->varimportances.ptr.p_double[i] = boundval(rep->varimportances.ptr.p_double[i], (double)(0), (double)(1), _state);
        }
        
        /*
         * Compute topvars[] array
         */
        ae_vector_set_length(&tmpr0, nvars, _state);
        for(j=0; j<=nvars-1; j++)
        {
            tmpr0.ptr.p_double[j] = -rep->varimportances.ptr.p_double[j];
            rep->topvars.ptr.p_int[j] = j;
        }
        tagsortfasti(&tmpr0, &rep->topvars, &tmpr1, &tmpi0, nvars, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        ae_frame_leave(_state);
        return;
    }
    ae_assert(ae_false, "EstimateVariableImportance: unexpected importance type", _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_dforest_estimatevariableimportance(decisionforestbuilder* s,
    ae_int_t sessionseed,
    const decisionforest* df,
    ae_int_t ntrees,
    dfreport* rep,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        dforest_estimatevariableimportance(s,sessionseed,df,ntrees,rep, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_dforest_estimatevariableimportance;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = s;
        _task->data.parameters[1].value.ival = sessionseed;
        _task->data.parameters[2].value.const_val = df;
        _task->data.parameters[3].value.ival = ntrees;
        _task->data.parameters[4].value.val = rep;
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
void _task_dforest_estimatevariableimportance(ae_task_data *_data, ae_state *_state)
{
    decisionforestbuilder* s;
    ae_int_t sessionseed;
    const decisionforest* df;
    ae_int_t ntrees;
    dfreport* rep;
    s = (decisionforestbuilder*)_data->parameters[0].value.val;
    sessionseed = _data->parameters[1].value.ival;
    df = (const decisionforest*)_data->parameters[2].value.const_val;
    ntrees = _data->parameters[3].value.ival;
    rep = (dfreport*)_data->parameters[4].value.val;
   ae_state_set_flags(_state, _data->flags);
   dforest_estimatevariableimportance(s,sessionseed,df,ntrees,rep, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_dforest_estimatevariableimportance(decisionforestbuilder* s,
    ae_int_t sessionseed,
    const decisionforest* df,
    ae_int_t ntrees,
    dfreport* rep,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_dforest_estimatevariableimportance;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = s;
    _task->data.parameters[1].value.ival = sessionseed;
    _task->data.parameters[2].value.const_val = df;
    _task->data.parameters[3].value.ival = ntrees;
    _task->data.parameters[4].value.val = rep;
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
Estimates permutation variable importance ratings for a range of dataset
points.

Initial call to this function should span entire range of the dataset,
[Idx0,Idx1)=[0,NPoints), because function performs initialization of some
internal structures when called with these arguments.

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
static void dforest_estimatepermutationimportances(decisionforestbuilder* s,
     const decisionforest* df,
     ae_int_t ntrees,
     ae_shared_pool* permpool,
     ae_int_t idx0,
     ae_int_t idx1,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t npoints;
    ae_int_t nvars;
    ae_int_t nclasses;
    ae_int_t nperm;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double v;
    ae_int_t treeroot;
    ae_int_t nodeoffs;
    double prediction;
    ae_int_t varidx;
    ae_int_t oobcounts;
    ae_int_t srcidx;
    dfpermimpbuf *permimpbuf;
    ae_smart_ptr _permimpbuf;

    ae_frame_make(_state, &_frame_block);
    memset(&_permimpbuf, 0, sizeof(_permimpbuf));
    ae_smart_ptr_init(&_permimpbuf, (void**)&permimpbuf, _state, ae_true);

    npoints = s->npoints;
    nvars = s->nvars;
    nclasses = s->nclasses;
    ae_assert(df->forestformat==dforest_dfuncompressedv0, "EstimateVariableImportance: integrity check failed (ff)", _state);
    ae_assert((idx0>=0&&idx0<=idx1)&&idx1<=npoints, "EstimateVariableImportance: integrity check failed (idx)", _state);
    ae_assert(s->iobmatrix.rows>=ntrees&&s->iobmatrix.cols>=npoints, "EstimateVariableImportance: integrity check failed (IOB)", _state);
    
    /*
     * Perform parallelization if batch is too large
     */
    if( idx1-idx0>dforest_permutationimportancebatchsize )
    {
        if( _trypexec_dforest_estimatepermutationimportances(s,df,ntrees,permpool,idx0,idx1, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            ae_frame_leave(_state);
            return;
        }
        ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_true, _state);
        j = (idx1-idx0)/2;
        _spawn_dforest_estimatepermutationimportances(s, df, ntrees, permpool, idx0, idx0+j, _child_tasks, _smp_enabled, _state);
        dforest_estimatepermutationimportances(s, df, ntrees, permpool, idx0+j, idx1, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_false, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Retrieve buffer object from pool
     */
    ae_shared_pool_retrieve(permpool, &_permimpbuf, _state);
    
    /*
     * Process range of points [idx0,idx1)
     */
    nperm = nvars+2;
    for(i=idx0; i<=idx1-1; i++)
    {
        ae_assert(s->dstype==0, "EstimateVariableImportance: unexpected dataset type", _state);
        for(j=0; j<=nvars-1; j++)
        {
            permimpbuf->xraw.ptr.p_double[j] = s->dsdata.ptr.p_double[j*npoints+i];
            srcidx = s->varimpshuffle2.ptr.p_int[(i+s->varimpshuffle2.ptr.p_int[npoints+j])%npoints];
            permimpbuf->xdist.ptr.p_double[j] = s->dsdata.ptr.p_double[j*npoints+srcidx];
        }
        if( nclasses>1 )
        {
            for(j=0; j<=nclasses-1; j++)
            {
                permimpbuf->targety.ptr.p_double[j] = (double)(0);
            }
            permimpbuf->targety.ptr.p_double[s->dsival.ptr.p_int[i]] = (double)(1);
        }
        else
        {
            permimpbuf->targety.ptr.p_double[0] = s->dsrval.ptr.p_double[i];
        }
        
        /*
         * Process all trees, for each tree compute NPerm losses corresponding
         * to various permutations of variable values
         */
        for(j=0; j<=nperm*nclasses-1; j++)
        {
            permimpbuf->yv.ptr.p_double[j] = (double)(0);
        }
        oobcounts = 0;
        treeroot = 0;
        for(k=0; k<=ntrees-1; k++)
        {
            if( !s->iobmatrix.ptr.pp_bool[k][i] )
            {
                
                /*
                 * Process original (unperturbed) point and analyze path from the
                 * tree root to the final leaf. Output prediction to RawPrediction.
                 *
                 * Additionally, for each variable in [0,NVars-1] save offset of
                 * the first split on this variable. It allows us to quickly compute
                 * tree decision when perturbation does not change decision path.
                 */
                ae_assert(df->forestformat==dforest_dfuncompressedv0, "EstimateVariableImportance: integrity check failed (ff)", _state);
                nodeoffs = treeroot+1;
                for(j=0; j<=nvars-1; j++)
                {
                    permimpbuf->startnodes.ptr.p_int[j] = -1;
                }
                prediction = (double)(0);
                for(;;)
                {
                    if( ae_fp_eq(df->trees.ptr.p_double[nodeoffs],(double)(-1)) )
                    {
                        prediction = df->trees.ptr.p_double[nodeoffs+1];
                        break;
                    }
                    j = ae_round(df->trees.ptr.p_double[nodeoffs], _state);
                    if( permimpbuf->startnodes.ptr.p_int[j]<0 )
                    {
                        permimpbuf->startnodes.ptr.p_int[j] = nodeoffs;
                    }
                    if( permimpbuf->xraw.ptr.p_double[j]<df->trees.ptr.p_double[nodeoffs+1] )
                    {
                        nodeoffs = nodeoffs+dforest_innernodewidth;
                    }
                    else
                    {
                        nodeoffs = treeroot+ae_round(df->trees.ptr.p_double[nodeoffs+2], _state);
                    }
                }
                
                /*
                 * Save loss for unperturbed point
                 */
                varidx = nvars+1;
                if( nclasses>1 )
                {
                    j = ae_round(prediction, _state);
                    permimpbuf->yv.ptr.p_double[varidx*nclasses+j] = permimpbuf->yv.ptr.p_double[varidx*nclasses+j]+(double)1;
                }
                else
                {
                    permimpbuf->yv.ptr.p_double[varidx] = permimpbuf->yv.ptr.p_double[varidx]+prediction;
                }
                
                /*
                 * Save loss for all variables being perturbed (XDist).
                 * This loss is used as a reference loss when we compute R-squared.
                 */
                varidx = nvars;
                for(j=0; j<=nclasses-1; j++)
                {
                    permimpbuf->y.ptr.p_double[j] = (double)(0);
                }
                dforest_dfprocessinternaluncompressed(df, treeroot, treeroot+1, &permimpbuf->xdist, &permimpbuf->y, _state);
                for(j=0; j<=nclasses-1; j++)
                {
                    permimpbuf->yv.ptr.p_double[varidx*nclasses+j] = permimpbuf->yv.ptr.p_double[varidx*nclasses+j]+permimpbuf->y.ptr.p_double[j];
                }
                
                /*
                 * Compute losses for variable #VarIdx being perturbed. Quite an often decision
                 * process does not actually depend on the variable #VarIdx (path from the tree
                 * root does not include splits on this variable). In such cases we perform
                 * quick exit from the loop with precomputed value.
                 */
                for(j=0; j<=nvars-1; j++)
                {
                    permimpbuf->xcur.ptr.p_double[j] = permimpbuf->xraw.ptr.p_double[j];
                }
                for(varidx=0; varidx<=nvars-1; varidx++)
                {
                    if( permimpbuf->startnodes.ptr.p_int[varidx]>=0 )
                    {
                        
                        /*
                         * Path from tree root to the final leaf involves split on variable #VarIdx.
                         * Restart computation from the position first split on #VarIdx.
                         */
                        ae_assert(df->forestformat==dforest_dfuncompressedv0, "EstimateVariableImportance: integrity check failed (ff)", _state);
                        permimpbuf->xcur.ptr.p_double[varidx] = permimpbuf->xdist.ptr.p_double[varidx];
                        nodeoffs = permimpbuf->startnodes.ptr.p_int[varidx];
                        for(;;)
                        {
                            if( ae_fp_eq(df->trees.ptr.p_double[nodeoffs],(double)(-1)) )
                            {
                                if( nclasses>1 )
                                {
                                    j = ae_round(df->trees.ptr.p_double[nodeoffs+1], _state);
                                    permimpbuf->yv.ptr.p_double[varidx*nclasses+j] = permimpbuf->yv.ptr.p_double[varidx*nclasses+j]+(double)1;
                                }
                                else
                                {
                                    permimpbuf->yv.ptr.p_double[varidx] = permimpbuf->yv.ptr.p_double[varidx]+df->trees.ptr.p_double[nodeoffs+1];
                                }
                                break;
                            }
                            j = ae_round(df->trees.ptr.p_double[nodeoffs], _state);
                            if( permimpbuf->xcur.ptr.p_double[j]<df->trees.ptr.p_double[nodeoffs+1] )
                            {
                                nodeoffs = nodeoffs+dforest_innernodewidth;
                            }
                            else
                            {
                                nodeoffs = treeroot+ae_round(df->trees.ptr.p_double[nodeoffs+2], _state);
                            }
                        }
                        permimpbuf->xcur.ptr.p_double[varidx] = permimpbuf->xraw.ptr.p_double[varidx];
                    }
                    else
                    {
                        
                        /*
                         * Path from tree root to the final leaf does NOT involve split on variable #VarIdx.
                         * Permutation does not change tree output, reuse already computed value.
                         */
                        if( nclasses>1 )
                        {
                            j = ae_round(prediction, _state);
                            permimpbuf->yv.ptr.p_double[varidx*nclasses+j] = permimpbuf->yv.ptr.p_double[varidx*nclasses+j]+(double)1;
                        }
                        else
                        {
                            permimpbuf->yv.ptr.p_double[varidx] = permimpbuf->yv.ptr.p_double[varidx]+prediction;
                        }
                    }
                }
                
                /*
                 * update OOB counter
                 */
                inc(&oobcounts, _state);
            }
            treeroot = treeroot+ae_round(df->trees.ptr.p_double[treeroot], _state);
        }
        
        /*
         * Now YV[] stores NPerm versions of the forest output for various permutations of variable values.
         * Update losses.
         */
        for(j=0; j<=nperm-1; j++)
        {
            for(k=0; k<=nclasses-1; k++)
            {
                permimpbuf->yv.ptr.p_double[j*nclasses+k] = permimpbuf->yv.ptr.p_double[j*nclasses+k]/coalesce((double)(oobcounts), (double)(1), _state);
            }
            v = (double)(0);
            for(k=0; k<=nclasses-1; k++)
            {
                v = v+ae_sqr(permimpbuf->yv.ptr.p_double[j*nclasses+k]-permimpbuf->targety.ptr.p_double[k], _state);
            }
            permimpbuf->losses.ptr.p_double[j] = permimpbuf->losses.ptr.p_double[j]+v;
        }
        
        /*
         * Update progress indicator
         */
        threadunsafeincby(&s->rdfprogress, ntrees, _state);
    }
    
    /*
     * Recycle buffer object with updated Losses[] field
     */
    ae_shared_pool_recycle(permpool, &_permimpbuf, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_dforest_estimatepermutationimportances(decisionforestbuilder* s,
    const decisionforest* df,
    ae_int_t ntrees,
    ae_shared_pool* permpool,
    ae_int_t idx0,
    ae_int_t idx1,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        dforest_estimatepermutationimportances(s,df,ntrees,permpool,idx0,idx1, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_dforest_estimatepermutationimportances;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = s;
        _task->data.parameters[1].value.const_val = df;
        _task->data.parameters[2].value.ival = ntrees;
        _task->data.parameters[3].value.val = permpool;
        _task->data.parameters[4].value.ival = idx0;
        _task->data.parameters[5].value.ival = idx1;
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
void _task_dforest_estimatepermutationimportances(ae_task_data *_data, ae_state *_state)
{
    decisionforestbuilder* s;
    const decisionforest* df;
    ae_int_t ntrees;
    ae_shared_pool* permpool;
    ae_int_t idx0;
    ae_int_t idx1;
    s = (decisionforestbuilder*)_data->parameters[0].value.val;
    df = (const decisionforest*)_data->parameters[1].value.const_val;
    ntrees = _data->parameters[2].value.ival;
    permpool = (ae_shared_pool*)_data->parameters[3].value.val;
    idx0 = _data->parameters[4].value.ival;
    idx1 = _data->parameters[5].value.ival;
   ae_state_set_flags(_state, _data->flags);
   dforest_estimatepermutationimportances(s,df,ntrees,permpool,idx0,idx1, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_dforest_estimatepermutationimportances(decisionforestbuilder* s,
    const decisionforest* df,
    ae_int_t ntrees,
    ae_shared_pool* permpool,
    ae_int_t idx0,
    ae_int_t idx1,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_dforest_estimatepermutationimportances;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = s;
    _task->data.parameters[1].value.const_val = df;
    _task->data.parameters[2].value.ival = ntrees;
    _task->data.parameters[3].value.val = permpool;
    _task->data.parameters[4].value.ival = idx0;
    _task->data.parameters[5].value.ival = idx1;
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
Sets report fields to their default values

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
static void dforest_cleanreport(const decisionforestbuilder* s,
     dfreport* rep,
     ae_state *_state)
{
    ae_int_t i;


    rep->relclserror = (double)(0);
    rep->avgce = (double)(0);
    rep->rmserror = (double)(0);
    rep->avgerror = (double)(0);
    rep->avgrelerror = (double)(0);
    rep->oobrelclserror = (double)(0);
    rep->oobavgce = (double)(0);
    rep->oobrmserror = (double)(0);
    rep->oobavgerror = (double)(0);
    rep->oobavgrelerror = (double)(0);
    ae_vector_set_length(&rep->topvars, s->nvars, _state);
    ae_vector_set_length(&rep->varimportances, s->nvars, _state);
    for(i=0; i<=s->nvars-1; i++)
    {
        rep->topvars.ptr.p_int[i] = i;
        rep->varimportances.ptr.p_double[i] = (double)(0);
    }
}


/*************************************************************************
This function returns NRMS2 loss (sum of squared residuals) for a constant-
output model:
* model output is a mean over TRN set being passed (for classification
  problems - NClasses-dimensional vector of class probabilities)
* model is evaluated over TST set being passed, with L2 loss being returned

Input parameters:
    NClasses            -   ">1" for classification, "=1" for regression
    TrnLabelsI          -   training set labels, class indexes (for NClasses>1)
    TrnLabelsR          -   training set output values (for NClasses=1)
    TrnIdx0, TrnIdx1    -   a range [Idx0,Idx1) of elements in LabelsI/R is considered
    TstLabelsI          -   training set labels, class indexes (for NClasses>1)
    TstLabelsR          -   training set output values (for NClasses=1)
    TstIdx0, TstIdx1    -   a range [Idx0,Idx1) of elements in LabelsI/R is considered
    TmpI        -   temporary array, reallocated as needed
    
Result:
    sum of squared residuals;
    for NClasses>=2 it coincides with Gini impurity times (Idx1-Idx0)

Following fields of WorkBuf are used as temporaries:
* TmpMeanNRMS2

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
static double dforest_meannrms2(ae_int_t nclasses,
     /* Integer */ const ae_vector* trnlabelsi,
     /* Real    */ const ae_vector* trnlabelsr,
     ae_int_t trnidx0,
     ae_int_t trnidx1,
     /* Integer */ const ae_vector* tstlabelsi,
     /* Real    */ const ae_vector* tstlabelsr,
     ae_int_t tstidx0,
     ae_int_t tstidx1,
     /* Integer */ ae_vector* tmpi,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;
    ae_int_t ntrn;
    ae_int_t ntst;
    double v;
    double vv;
    double invntrn;
    double pitrn;
    double nitst;
    double result;


    ae_assert(trnidx0<=trnidx1, "MeanNRMS2: integrity check failed (8754)", _state);
    ae_assert(tstidx0<=tstidx1, "MeanNRMS2: integrity check failed (8754)", _state);
    result = (double)(0);
    ntrn = trnidx1-trnidx0;
    ntst = tstidx1-tstidx0;
    if( ntrn==0||ntst==0 )
    {
        return result;
    }
    invntrn = 1.0/(double)ntrn;
    if( nclasses>1 )
    {
        
        /*
         * Classification problem
         */
        ivectorsetlengthatleast(tmpi, 2*nclasses, _state);
        for(i=0; i<=2*nclasses-1; i++)
        {
            tmpi->ptr.p_int[i] = 0;
        }
        for(i=trnidx0; i<=trnidx1-1; i++)
        {
            k = trnlabelsi->ptr.p_int[i];
            tmpi->ptr.p_int[k] = tmpi->ptr.p_int[k]+1;
        }
        for(i=tstidx0; i<=tstidx1-1; i++)
        {
            k = tstlabelsi->ptr.p_int[i];
            tmpi->ptr.p_int[k+nclasses] = tmpi->ptr.p_int[k+nclasses]+1;
        }
        for(i=0; i<=nclasses-1; i++)
        {
            pitrn = (double)tmpi->ptr.p_int[i]*invntrn;
            nitst = (double)(tmpi->ptr.p_int[i+nclasses]);
            result = result+nitst*((double)1-pitrn)*((double)1-pitrn);
            result = result+((double)ntst-nitst)*pitrn*pitrn;
        }
    }
    else
    {
        
        /*
         * regression-specific code
         */
        v = (double)(0);
        for(i=trnidx0; i<=trnidx1-1; i++)
        {
            v = v+trnlabelsr->ptr.p_double[i];
        }
        v = v*invntrn;
        for(i=tstidx0; i<=tstidx1-1; i++)
        {
            vv = tstlabelsr->ptr.p_double[i]-v;
            result = result+vv*vv;
        }
    }
    return result;
}


/*************************************************************************
This function is a part of the recurrent tree construction function; it
selects variable for splitting according to current tree construction
algorithm.

Note: modifies VarsInPool, may decrease it if some variables become non-informative
and leave the pool.

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
static void dforest_choosecurrentsplitdense(const decisionforestbuilder* s,
     dfworkbuf* workbuf,
     ae_int_t* varsinpool,
     ae_int_t varstoselect,
     hqrndstate* rs,
     ae_int_t idx0,
     ae_int_t idx1,
     ae_int_t* varbest,
     double* splitbest,
     ae_state *_state)
{
    ae_int_t npoints;
    double errbest;
    ae_int_t varstried;
    ae_int_t varcur;
    ae_bool valuesaresame;
    ae_int_t offs;
    double split;
    ae_int_t i;
    double v;
    double v0;
    double currms;
    ae_int_t info;

    *varbest = 0;
    *splitbest = 0.0;

    ae_assert(s->dstype==0, "sparsity is not supported 4terg!", _state);
    ae_assert(s->rdfalgo==0, "BuildRandomTreeRec: integrity check failed (1657)", _state);
    ae_assert(idx0<idx1, "BuildRandomTreeRec: integrity check failed (3445)", _state);
    npoints = s->npoints;
    
    /*
     * Select split according to dense direct RDF algorithm
     */
    *varbest = -1;
    errbest = ae_maxrealnumber;
    *splitbest = (double)(0);
    varstried = 0;
    while(varstried<=ae_minint(varstoselect, *varsinpool, _state)-1)
    {
        
        /*
         * select variables from pool
         */
        swapelementsi(&workbuf->varpool, varstried, varstried+hqrnduniformi(rs, *varsinpool-varstried, _state), _state);
        varcur = workbuf->varpool.ptr.p_int[varstried];
        
        /*
         * Load variable values to working array.
         * If all variable values are same, variable is excluded from pool and we re-run variable selection.
         */
        valuesaresame = ae_true;
        ae_assert(s->dstype==0, "not supported segsv34fs", _state);
        offs = npoints*varcur;
        v0 = s->dsdata.ptr.p_double[offs+workbuf->trnset.ptr.p_int[idx0]];
        for(i=idx0; i<=idx1-1; i++)
        {
            v = s->dsdata.ptr.p_double[offs+workbuf->trnset.ptr.p_int[i]];
            workbuf->curvals.ptr.p_double[i] = v;
            valuesaresame = valuesaresame&&v==v0;
        }
        if( valuesaresame )
        {
            
            /*
             * Variable does not change across current subset.
             * Exclude variable from pool, go to the next iteration.
             * VarsTried is not increased.
             *
             * NOTE: it is essential that updated VarsInPool is passed
             *       down to children but not up to caller - it is
             *       possible that one level higher this variable is
             *       not-fixed.
             */
            swapelementsi(&workbuf->varpool, varstried, *varsinpool-1, _state);
            *varsinpool = *varsinpool-1;
            continue;
        }
        
        /*
         * Now we are ready to infer the split
         */
        dforest_evaluatedensesplit(s, workbuf, rs, varcur, idx0, idx1, &info, &split, &currms, _state);
        if( info>0&&(*varbest<0||ae_fp_less_eq(currms,errbest)) )
        {
            errbest = currms;
            *varbest = varcur;
            *splitbest = split;
            for(i=idx0; i<=idx1-1; i++)
            {
                workbuf->bestvals.ptr.p_double[i] = workbuf->curvals.ptr.p_double[i];
            }
        }
        
        /*
         * Next iteration
         */
        varstried = varstried+1;
    }
}


/*************************************************************************
This function performs split on some specific dense variable whose values
are stored in WorkBuf.CurVals[Idx0,Idx1) and labels are stored in
WorkBuf.TrnLabelsR/I[Idx0,Idx1).

It returns split value and associated RMS error. It is responsibility of
the caller to make sure that variable has at least two distinct values,
i.e. it is possible to make a split.

Precomputed values of following fields of WorkBuf are used:
* ClassPriors

Following fields of WorkBuf are used as temporaries:
* ClassTotals0,1,01
* Tmp0I, Tmp1I, Tmp0R, Tmp1R, Tmp2R, Tmp3R

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
static void dforest_evaluatedensesplit(const decisionforestbuilder* s,
     dfworkbuf* workbuf,
     hqrndstate* rs,
     ae_int_t splitvar,
     ae_int_t idx0,
     ae_int_t idx1,
     ae_int_t* info,
     double* split,
     double* rms,
     ae_state *_state)
{
    ae_int_t nclasses;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k0;
    ae_int_t k1;
    double v;
    double v0;
    double v1;
    double v2;
    ae_int_t sl;
    ae_int_t sr;

    *info = 0;
    *split = 0.0;
    *rms = 0.0;

    ae_assert(idx0<idx1, "BuildRandomTreeRec: integrity check failed (8754)", _state);
    nclasses = s->nclasses;
    if( s->dsbinary.ptr.p_bool[splitvar] )
    {
        
        /*
         * Try simple binary split, if possible
         * Split can be inferred from minimum/maximum values, just calculate RMS error
         */
        *info = 1;
        *split = dforest_getsplit(s, s->dsmin.ptr.p_double[splitvar], s->dsmax.ptr.p_double[splitvar], rs, _state);
        if( nclasses>1 )
        {
            
            /*
             * Classification problem
             */
            for(j=0; j<=nclasses-1; j++)
            {
                workbuf->classtotals0.ptr.p_int[j] = 0;
            }
            sl = 0;
            for(i=idx0; i<=idx1-1; i++)
            {
                if( workbuf->curvals.ptr.p_double[i]<*split )
                {
                    j = workbuf->trnlabelsi.ptr.p_int[i];
                    workbuf->classtotals0.ptr.p_int[j] = workbuf->classtotals0.ptr.p_int[j]+1;
                    sl = sl+1;
                }
            }
            sr = idx1-idx0-sl;
            ae_assert(sl!=0&&sr!=0, "BuildRandomTreeRec: something strange, impossible failure!", _state);
            v0 = (double)1/(double)sl;
            v1 = (double)1/(double)sr;
            *rms = (double)(0);
            for(j=0; j<=nclasses-1; j++)
            {
                k0 = workbuf->classtotals0.ptr.p_int[j];
                k1 = workbuf->classpriors.ptr.p_int[j]-k0;
                *rms = *rms+(double)k0*((double)1-v0*(double)k0)+(double)k1*((double)1-v1*(double)k1);
            }
            *rms = ae_sqrt(*rms/(double)(nclasses*(idx1-idx0+1)), _state);
        }
        else
        {
            
            /*
             * regression-specific code
             */
            sl = 0;
            sr = 0;
            v1 = (double)(0);
            v2 = (double)(0);
            for(j=idx0; j<=idx1-1; j++)
            {
                if( workbuf->curvals.ptr.p_double[j]<*split )
                {
                    v1 = v1+workbuf->trnlabelsr.ptr.p_double[j];
                    sl = sl+1;
                }
                else
                {
                    v2 = v2+workbuf->trnlabelsr.ptr.p_double[j];
                    sr = sr+1;
                }
            }
            ae_assert(sl!=0&&sr!=0, "BuildRandomTreeRec: something strange, impossible failure!", _state);
            v1 = v1/(double)sl;
            v2 = v2/(double)sr;
            *rms = (double)(0);
            for(j=0; j<=idx1-idx0-1; j++)
            {
                v = workbuf->trnlabelsr.ptr.p_double[idx0+j];
                if( workbuf->curvals.ptr.p_double[j]<*split )
                {
                    v = v-v1;
                }
                else
                {
                    v = v-v2;
                }
                *rms = *rms+v*v;
            }
            *rms = ae_sqrt(*rms/(double)(idx1-idx0+1), _state);
        }
    }
    else
    {
        
        /*
         * General split
         */
        *info = 0;
        if( nclasses>1 )
        {
            for(i=0; i<=idx1-idx0-1; i++)
            {
                workbuf->tmp0r.ptr.p_double[i] = workbuf->curvals.ptr.p_double[idx0+i];
                workbuf->tmp0i.ptr.p_int[i] = workbuf->trnlabelsi.ptr.p_int[idx0+i];
            }
            dforest_classifiersplit(s, workbuf, &workbuf->tmp0r, &workbuf->tmp0i, idx1-idx0, rs, info, split, rms, &workbuf->tmp1r, &workbuf->tmp1i, _state);
        }
        else
        {
            for(i=0; i<=idx1-idx0-1; i++)
            {
                workbuf->tmp0r.ptr.p_double[i] = workbuf->curvals.ptr.p_double[idx0+i];
                workbuf->tmp1r.ptr.p_double[i] = workbuf->trnlabelsr.ptr.p_double[idx0+i];
            }
            dforest_regressionsplit(s, workbuf, &workbuf->tmp0r, &workbuf->tmp1r, idx1-idx0, info, split, rms, &workbuf->tmp2r, &workbuf->tmp3r, _state);
        }
    }
}


/*************************************************************************
Classifier split
*************************************************************************/
static void dforest_classifiersplit(const decisionforestbuilder* s,
     dfworkbuf* workbuf,
     /* Real    */ ae_vector* x,
     /* Integer */ ae_vector* c,
     ae_int_t n,
     hqrndstate* rs,
     ae_int_t* info,
     double* threshold,
     double* e,
     /* Real    */ ae_vector* sortrbuf,
     /* Integer */ ae_vector* sortibuf,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t n0;
    ae_int_t n0prev;
    double v;
    ae_int_t advanceby;
    double rms;
    ae_int_t k0;
    ae_int_t k1;
    double v0;
    double v1;
    ae_int_t nclasses;
    double vmin;
    double vmax;

    *info = 0;
    *threshold = 0.0;
    *e = 0.0;

    ae_assert((s->rdfsplitstrength==0||s->rdfsplitstrength==1)||s->rdfsplitstrength==2, "RDF: unexpected split type at ClassifierSplit()", _state);
    nclasses = s->nclasses;
    advanceby = 1;
    if( n>=20 )
    {
        advanceby = ae_maxint(2, ae_round((double)n*0.05, _state), _state);
    }
    *info = -1;
    *threshold = (double)(0);
    *e = ae_maxrealnumber;
    
    /*
     * Random split
     */
    if( s->rdfsplitstrength==0 )
    {
        
        /*
         * Evaluate minimum, maximum and randomly selected values
         */
        vmin = x->ptr.p_double[0];
        vmax = x->ptr.p_double[0];
        for(i=1; i<=n-1; i++)
        {
            v = x->ptr.p_double[i];
            if( v<vmin )
            {
                vmin = v;
            }
            if( v>vmax )
            {
                vmax = v;
            }
        }
        if( ae_fp_eq(vmin,vmax) )
        {
            return;
        }
        v = x->ptr.p_double[hqrnduniformi(rs, n, _state)];
        if( ae_fp_eq(v,vmin) )
        {
            v = vmax;
        }
        
        /*
         * Calculate RMS error associated with the split
         */
        for(i=0; i<=nclasses-1; i++)
        {
            workbuf->classtotals0.ptr.p_int[i] = 0;
        }
        n0 = 0;
        for(i=0; i<=n-1; i++)
        {
            if( x->ptr.p_double[i]<v )
            {
                k = c->ptr.p_int[i];
                workbuf->classtotals0.ptr.p_int[k] = workbuf->classtotals0.ptr.p_int[k]+1;
                n0 = n0+1;
            }
        }
        ae_assert(n0>0&&n0<n, "RDF: critical integrity check failed at ClassifierSplit()", _state);
        v0 = (double)1/(double)n0;
        v1 = (double)1/(double)(n-n0);
        rms = (double)(0);
        for(j=0; j<=nclasses-1; j++)
        {
            k0 = workbuf->classtotals0.ptr.p_int[j];
            k1 = workbuf->classpriors.ptr.p_int[j]-k0;
            rms = rms+(double)k0*((double)1-v0*(double)k0)+(double)k1*((double)1-v1*(double)k1);
        }
        *threshold = v;
        *info = 1;
        *e = rms;
        return;
    }
    
    /*
     * Stronger splits which require us to sort the data
     * Quick check for degeneracy
     */
    tagsortfasti(x, c, sortrbuf, sortibuf, n, _state);
    v = 0.5*(x->ptr.p_double[0]+x->ptr.p_double[n-1]);
    if( !(ae_fp_less(x->ptr.p_double[0],v)&&ae_fp_less(v,x->ptr.p_double[n-1])) )
    {
        return;
    }
    
    /*
     * Split at the middle
     */
    if( s->rdfsplitstrength==1 )
    {
        
        /*
         * Select split position
         */
        vmin = x->ptr.p_double[0];
        vmax = x->ptr.p_double[n-1];
        v = x->ptr.p_double[n/2];
        if( ae_fp_eq(v,vmin) )
        {
            v = vmin+0.001*(vmax-vmin);
        }
        if( ae_fp_eq(v,vmin) )
        {
            v = vmax;
        }
        
        /*
         * Calculate RMS error associated with the split
         */
        for(i=0; i<=nclasses-1; i++)
        {
            workbuf->classtotals0.ptr.p_int[i] = 0;
        }
        n0 = 0;
        for(i=0; i<=n-1; i++)
        {
            if( x->ptr.p_double[i]<v )
            {
                k = c->ptr.p_int[i];
                workbuf->classtotals0.ptr.p_int[k] = workbuf->classtotals0.ptr.p_int[k]+1;
                n0 = n0+1;
            }
        }
        ae_assert(n0>0&&n0<n, "RDF: critical integrity check failed at ClassifierSplit()", _state);
        v0 = (double)1/(double)n0;
        v1 = (double)1/(double)(n-n0);
        rms = (double)(0);
        for(j=0; j<=nclasses-1; j++)
        {
            k0 = workbuf->classtotals0.ptr.p_int[j];
            k1 = workbuf->classpriors.ptr.p_int[j]-k0;
            rms = rms+(double)k0*((double)1-v0*(double)k0)+(double)k1*((double)1-v1*(double)k1);
        }
        *threshold = v;
        *info = 1;
        *e = rms;
        return;
    }
    
    /*
     * Strong split
     */
    if( s->rdfsplitstrength==2 )
    {
        
        /*
         * Prepare initial split.
         * Evaluate current split, prepare next one, repeat.
         */
        for(i=0; i<=nclasses-1; i++)
        {
            workbuf->classtotals0.ptr.p_int[i] = 0;
        }
        n0 = 1;
        while(n0<n&&x->ptr.p_double[n0]==x->ptr.p_double[n0-1])
        {
            n0 = n0+1;
        }
        ae_assert(n0<n, "RDF: critical integrity check failed in ClassifierSplit()", _state);
        for(i=0; i<=n0-1; i++)
        {
            k = c->ptr.p_int[i];
            workbuf->classtotals0.ptr.p_int[k] = workbuf->classtotals0.ptr.p_int[k]+1;
        }
        *info = -1;
        *threshold = x->ptr.p_double[n-1];
        *e = ae_maxrealnumber;
        while(n0<n)
        {
            
            /*
             * RMS error associated with current split
             */
            v0 = (double)1/(double)n0;
            v1 = (double)1/(double)(n-n0);
            rms = (double)(0);
            for(j=0; j<=nclasses-1; j++)
            {
                k0 = workbuf->classtotals0.ptr.p_int[j];
                k1 = workbuf->classpriors.ptr.p_int[j]-k0;
                rms = rms+(double)k0*((double)1-v0*(double)k0)+(double)k1*((double)1-v1*(double)k1);
            }
            if( *info<0||rms<*e )
            {
                *info = 1;
                *e = rms;
                *threshold = 0.5*(x->ptr.p_double[n0-1]+x->ptr.p_double[n0]);
                if( *threshold<=x->ptr.p_double[n0-1] )
                {
                    *threshold = x->ptr.p_double[n0];
                }
            }
            
            /*
             * Advance
             */
            n0prev = n0;
            while(n0<n&&n0-n0prev<advanceby)
            {
                v = x->ptr.p_double[n0];
                while(n0<n&&x->ptr.p_double[n0]==v)
                {
                    k = c->ptr.p_int[n0];
                    workbuf->classtotals0.ptr.p_int[k] = workbuf->classtotals0.ptr.p_int[k]+1;
                    n0 = n0+1;
                }
            }
        }
        if( *info>0 )
        {
            *e = ae_sqrt(*e/(double)(nclasses*n), _state);
        }
        return;
    }
    ae_assert(ae_false, "RDF: ClassifierSplit(), critical error", _state);
}


/*************************************************************************
Regression model split
*************************************************************************/
static void dforest_regressionsplit(const decisionforestbuilder* s,
     dfworkbuf* workbuf,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_int_t n,
     ae_int_t* info,
     double* threshold,
     double* e,
     /* Real    */ ae_vector* sortrbuf,
     /* Real    */ ae_vector* sortrbuf2,
     ae_state *_state)
{
    ae_int_t i;
    double vmin;
    double vmax;
    double bnd01;
    double bnd12;
    double bnd23;
    ae_int_t total0;
    ae_int_t total1;
    ae_int_t total2;
    ae_int_t total3;
    ae_int_t cnt0;
    ae_int_t cnt1;
    ae_int_t cnt2;
    ae_int_t cnt3;
    ae_int_t n0;
    ae_int_t advanceby;
    double v;
    double v0;
    double v1;
    double rms;
    ae_int_t n0prev;
    ae_int_t k0;
    ae_int_t k1;

    *info = 0;
    *threshold = 0.0;
    *e = 0.0;

    advanceby = 1;
    if( n>=20 )
    {
        advanceby = ae_maxint(2, ae_round((double)n*0.05, _state), _state);
    }
    
    /*
     * Sort data
     * Quick check for degeneracy
     */
    tagsortfastr(x, y, sortrbuf, sortrbuf2, n, _state);
    v = 0.5*(x->ptr.p_double[0]+x->ptr.p_double[n-1]);
    if( !(ae_fp_less(x->ptr.p_double[0],v)&&ae_fp_less(v,x->ptr.p_double[n-1])) )
    {
        *info = -1;
        *threshold = x->ptr.p_double[n-1];
        *e = ae_maxrealnumber;
        return;
    }
    
    /*
     * Prepare initial split.
     * Evaluate current split, prepare next one, repeat.
     */
    vmin = y->ptr.p_double[0];
    vmax = y->ptr.p_double[0];
    for(i=1; i<=n-1; i++)
    {
        v = y->ptr.p_double[i];
        if( v<vmin )
        {
            vmin = v;
        }
        if( v>vmax )
        {
            vmax = v;
        }
    }
    bnd12 = 0.5*(vmin+vmax);
    bnd01 = 0.5*(vmin+bnd12);
    bnd23 = 0.5*(vmax+bnd12);
    total0 = 0;
    total1 = 0;
    total2 = 0;
    total3 = 0;
    for(i=0; i<=n-1; i++)
    {
        v = y->ptr.p_double[i];
        if( v<bnd12 )
        {
            if( v<bnd01 )
            {
                total0 = total0+1;
            }
            else
            {
                total1 = total1+1;
            }
        }
        else
        {
            if( v<bnd23 )
            {
                total2 = total2+1;
            }
            else
            {
                total3 = total3+1;
            }
        }
    }
    n0 = 1;
    while(n0<n&&x->ptr.p_double[n0]==x->ptr.p_double[n0-1])
    {
        n0 = n0+1;
    }
    ae_assert(n0<n, "RDF: critical integrity check failed in ClassifierSplit()", _state);
    cnt0 = 0;
    cnt1 = 0;
    cnt2 = 0;
    cnt3 = 0;
    for(i=0; i<=n0-1; i++)
    {
        v = y->ptr.p_double[i];
        if( v<bnd12 )
        {
            if( v<bnd01 )
            {
                cnt0 = cnt0+1;
            }
            else
            {
                cnt1 = cnt1+1;
            }
        }
        else
        {
            if( v<bnd23 )
            {
                cnt2 = cnt2+1;
            }
            else
            {
                cnt3 = cnt3+1;
            }
        }
    }
    *info = -1;
    *threshold = x->ptr.p_double[n-1];
    *e = ae_maxrealnumber;
    while(n0<n)
    {
        
        /*
         * RMS error associated with current split
         */
        v0 = (double)1/(double)n0;
        v1 = (double)1/(double)(n-n0);
        rms = (double)(0);
        k0 = cnt0;
        k1 = total0-cnt0;
        rms = rms+(double)k0*((double)1-v0*(double)k0)+(double)k1*((double)1-v1*(double)k1);
        k0 = cnt1;
        k1 = total1-cnt1;
        rms = rms+(double)k0*((double)1-v0*(double)k0)+(double)k1*((double)1-v1*(double)k1);
        k0 = cnt2;
        k1 = total2-cnt2;
        rms = rms+(double)k0*((double)1-v0*(double)k0)+(double)k1*((double)1-v1*(double)k1);
        k0 = cnt3;
        k1 = total3-cnt3;
        rms = rms+(double)k0*((double)1-v0*(double)k0)+(double)k1*((double)1-v1*(double)k1);
        if( *info<0||rms<*e )
        {
            *info = 1;
            *e = rms;
            *threshold = 0.5*(x->ptr.p_double[n0-1]+x->ptr.p_double[n0]);
            if( *threshold<=x->ptr.p_double[n0-1] )
            {
                *threshold = x->ptr.p_double[n0];
            }
        }
        
        /*
         * Advance
         */
        n0prev = n0;
        while(n0<n&&n0-n0prev<advanceby)
        {
            v0 = x->ptr.p_double[n0];
            while(n0<n&&x->ptr.p_double[n0]==v0)
            {
                v = y->ptr.p_double[n0];
                if( v<bnd12 )
                {
                    if( v<bnd01 )
                    {
                        cnt0 = cnt0+1;
                    }
                    else
                    {
                        cnt1 = cnt1+1;
                    }
                }
                else
                {
                    if( v<bnd23 )
                    {
                        cnt2 = cnt2+1;
                    }
                    else
                    {
                        cnt3 = cnt3+1;
                    }
                }
                n0 = n0+1;
            }
        }
    }
    if( *info>0 )
    {
        *e = ae_sqrt(*e/(double)(4*n), _state);
    }
}


/*************************************************************************
Returns split: either deterministic split at the middle of [A,B], or randomly
chosen split.

It is guaranteed that A<Split<=B.

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
static double dforest_getsplit(const decisionforestbuilder* s,
     double a,
     double b,
     hqrndstate* rs,
     ae_state *_state)
{
    double result;


    result = 0.5*(a+b);
    if( ae_fp_less_eq(result,a) )
    {
        result = b;
    }
    return result;
}


/*************************************************************************
Outputs leaf to the tree

Following items of TRN and OOB sets are updated in the voting buffer:
* items [Idx0,Idx1) of WorkBuf.TrnSet
* items [OOBIdx0, OOBIdx1) of WorkBuf.OOBSet

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
static void dforest_outputleaf(const decisionforestbuilder* s,
     dfworkbuf* workbuf,
     /* Real    */ ae_vector* treebuf,
     dfvotebuf* votebuf,
     ae_int_t idx0,
     ae_int_t idx1,
     ae_int_t oobidx0,
     ae_int_t oobidx1,
     ae_int_t* treesize,
     double leafval,
     ae_state *_state)
{
    ae_int_t leafvali;
    ae_int_t nclasses;
    ae_int_t i;
    ae_int_t j;


    nclasses = s->nclasses;
    if( nclasses==1 )
    {
        
        /*
         * Store split to the tree
         */
        treebuf->ptr.p_double[*treesize] = (double)(-1);
        treebuf->ptr.p_double[*treesize+1] = leafval;
        
        /*
         * Update training and OOB voting stats
         */
        for(i=idx0; i<=idx1-1; i++)
        {
            j = workbuf->trnset.ptr.p_int[i];
            votebuf->trntotals.ptr.p_double[j] = votebuf->trntotals.ptr.p_double[j]+leafval;
            votebuf->trncounts.ptr.p_int[j] = votebuf->trncounts.ptr.p_int[j]+1;
        }
        for(i=oobidx0; i<=oobidx1-1; i++)
        {
            j = workbuf->oobset.ptr.p_int[i];
            votebuf->oobtotals.ptr.p_double[j] = votebuf->oobtotals.ptr.p_double[j]+leafval;
            votebuf->oobcounts.ptr.p_int[j] = votebuf->oobcounts.ptr.p_int[j]+1;
        }
    }
    else
    {
        
        /*
         * Store split to the tree
         */
        treebuf->ptr.p_double[*treesize] = (double)(-1);
        treebuf->ptr.p_double[*treesize+1] = leafval;
        
        /*
         * Update training and OOB voting stats
         */
        leafvali = ae_round(leafval, _state);
        for(i=idx0; i<=idx1-1; i++)
        {
            j = workbuf->trnset.ptr.p_int[i];
            votebuf->trntotals.ptr.p_double[j*nclasses+leafvali] = votebuf->trntotals.ptr.p_double[j*nclasses+leafvali]+(double)1;
            votebuf->trncounts.ptr.p_int[j] = votebuf->trncounts.ptr.p_int[j]+1;
        }
        for(i=oobidx0; i<=oobidx1-1; i++)
        {
            j = workbuf->oobset.ptr.p_int[i];
            votebuf->oobtotals.ptr.p_double[j*nclasses+leafvali] = votebuf->oobtotals.ptr.p_double[j*nclasses+leafvali]+(double)1;
            votebuf->oobcounts.ptr.p_int[j] = votebuf->oobcounts.ptr.p_int[j]+1;
        }
    }
    *treesize = *treesize+dforest_leafnodewidth;
}


/*************************************************************************
This function performs generic and algorithm-specific preprocessing of the
dataset

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
static void dforest_analyzeandpreprocessdataset(decisionforestbuilder* s,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t nvars;
    ae_int_t nclasses;
    ae_int_t npoints;
    ae_int_t i;
    ae_int_t j;
    ae_bool isbinary;
    double v;
    double v0;
    double v1;
    hqrndstate rs;

    ae_frame_make(_state, &_frame_block);
    memset(&rs, 0, sizeof(rs));
    _hqrndstate_init(&rs, _state, ae_true);

    ae_assert(s->dstype==0, "no sparsity", _state);
    npoints = s->npoints;
    nvars = s->nvars;
    nclasses = s->nclasses;
    
    /*
     * seed local RNG
     */
    if( s->rdfglobalseed>0 )
    {
        hqrndseed(s->rdfglobalseed, 3532, &rs, _state);
    }
    else
    {
        hqrndseed(ae_randominteger(30000, _state), 3532, &rs, _state);
    }
    
    /*
     * Generic processing
     */
    ae_assert(npoints>=1, "BuildRandomForest: integrity check failed", _state);
    rvectorsetlengthatleast(&s->dsmin, nvars, _state);
    rvectorsetlengthatleast(&s->dsmax, nvars, _state);
    bvectorsetlengthatleast(&s->dsbinary, nvars, _state);
    for(i=0; i<=nvars-1; i++)
    {
        v0 = s->dsdata.ptr.p_double[i*npoints+0];
        v1 = s->dsdata.ptr.p_double[i*npoints+0];
        for(j=1; j<=npoints-1; j++)
        {
            v = s->dsdata.ptr.p_double[i*npoints+j];
            if( v<v0 )
            {
                v0 = v;
            }
            if( v>v1 )
            {
                v1 = v;
            }
        }
        s->dsmin.ptr.p_double[i] = v0;
        s->dsmax.ptr.p_double[i] = v1;
        ae_assert(ae_fp_less_eq(v0,v1), "BuildRandomForest: strange integrity check failure", _state);
        isbinary = ae_true;
        for(j=0; j<=npoints-1; j++)
        {
            v = s->dsdata.ptr.p_double[i*npoints+j];
            isbinary = isbinary&&(v==v0||v==v1);
        }
        s->dsbinary.ptr.p_bool[i] = isbinary;
    }
    if( nclasses==1 )
    {
        s->dsravg = (double)(0);
        for(i=0; i<=npoints-1; i++)
        {
            s->dsravg = s->dsravg+s->dsrval.ptr.p_double[i];
        }
        s->dsravg = s->dsravg/(double)npoints;
    }
    else
    {
        ivectorsetlengthatleast(&s->dsctotals, nclasses, _state);
        for(i=0; i<=nclasses-1; i++)
        {
            s->dsctotals.ptr.p_int[i] = 0;
        }
        for(i=0; i<=npoints-1; i++)
        {
            s->dsctotals.ptr.p_int[s->dsival.ptr.p_int[i]] = s->dsctotals.ptr.p_int[s->dsival.ptr.p_int[i]]+1;
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
This function merges together trees generated during training and outputs
it to the decision forest.

INPUT PARAMETERS:
    S           -   decision forest builder object
    NTrees      -   NTrees>=1, number of trees to train

OUTPUT PARAMETERS:
    DF          -   decision forest
    Rep         -   report

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
static void dforest_mergetrees(decisionforestbuilder* s,
     decisionforest* df,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t cursize;
    ae_int_t offs;
    dftreebuf *tree;
    ae_smart_ptr _tree;
    ae_vector treesizes;
    ae_vector treeoffsets;

    ae_frame_make(_state, &_frame_block);
    memset(&_tree, 0, sizeof(_tree));
    memset(&treesizes, 0, sizeof(treesizes));
    memset(&treeoffsets, 0, sizeof(treeoffsets));
    ae_smart_ptr_init(&_tree, (void**)&tree, _state, ae_true);
    ae_vector_init(&treesizes, 0, DT_INT, _state, ae_true);
    ae_vector_init(&treeoffsets, 0, DT_INT, _state, ae_true);

    df->forestformat = dforest_dfuncompressedv0;
    df->nvars = s->nvars;
    df->nclasses = s->nclasses;
    df->bufsize = 0;
    df->ntrees = 0;
    
    /*
     * Determine trees count
     */
    ae_shared_pool_first_recycled(&s->treepool, &_tree, _state);
    while(tree!=NULL)
    {
        df->ntrees = df->ntrees+1;
        ae_shared_pool_next_recycled(&s->treepool, &_tree, _state);
    }
    ae_assert(df->ntrees>0, "MergeTrees: integrity check failed, zero trees count", _state);
    
    /*
     * Determine individual tree sizes and total buffer size
     */
    ae_vector_set_length(&treesizes, df->ntrees, _state);
    for(i=0; i<=df->ntrees-1; i++)
    {
        treesizes.ptr.p_int[i] = -1;
    }
    ae_shared_pool_first_recycled(&s->treepool, &_tree, _state);
    while(tree!=NULL)
    {
        ae_assert(tree->treeidx>=0&&tree->treeidx<df->ntrees, "MergeTrees: integrity check failed (wrong TreeIdx)", _state);
        ae_assert(treesizes.ptr.p_int[tree->treeidx]<0, "MergeTrees: integrity check failed (duplicate TreeIdx)", _state);
        df->bufsize = df->bufsize+ae_round(tree->treebuf.ptr.p_double[0], _state);
        treesizes.ptr.p_int[tree->treeidx] = ae_round(tree->treebuf.ptr.p_double[0], _state);
        ae_shared_pool_next_recycled(&s->treepool, &_tree, _state);
    }
    for(i=0; i<=df->ntrees-1; i++)
    {
        ae_assert(treesizes.ptr.p_int[i]>0, "MergeTrees: integrity check failed (wrong TreeSize)", _state);
    }
    
    /*
     * Determine offsets for individual trees in output buffer
     */
    ae_vector_set_length(&treeoffsets, df->ntrees, _state);
    treeoffsets.ptr.p_int[0] = 0;
    for(i=1; i<=df->ntrees-1; i++)
    {
        treeoffsets.ptr.p_int[i] = treeoffsets.ptr.p_int[i-1]+treesizes.ptr.p_int[i-1];
    }
    
    /*
     * Output trees
     *
     * NOTE: since ALGLIB 3.16.0 trees are sorted by tree index prior to
     *       output (necessary for variable importance estimation), that's
     *       why we need array of tree offsets
     */
    ae_vector_set_length(&df->trees, df->bufsize, _state);
    ae_shared_pool_first_recycled(&s->treepool, &_tree, _state);
    while(tree!=NULL)
    {
        cursize = ae_round(tree->treebuf.ptr.p_double[0], _state);
        offs = treeoffsets.ptr.p_int[tree->treeidx];
        for(i=0; i<=cursize-1; i++)
        {
            df->trees.ptr.p_double[offs+i] = tree->treebuf.ptr.p_double[i];
        }
        ae_shared_pool_next_recycled(&s->treepool, &_tree, _state);
    }
    ae_frame_leave(_state);
}


/*************************************************************************
This function post-processes voting array and calculates TRN and OOB errors.

INPUT PARAMETERS:
    S           -   decision forest builder object
    NTrees      -   number of trees in the forest
    Buf         -   possibly preallocated vote buffer, its contents is
                    overwritten by this function

OUTPUT PARAMETERS:
    Rep         -   report fields corresponding to errors are updated

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
static void dforest_processvotingresults(decisionforestbuilder* s,
     ae_int_t ntrees,
     dfvotebuf* buf,
     dfreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    dfvotebuf *vote;
    ae_smart_ptr _vote;
    ae_int_t nvars;
    ae_int_t nclasses;
    ae_int_t npoints;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t k1;
    double v;
    ae_int_t avgrelcnt;
    ae_int_t oobavgrelcnt;

    ae_frame_make(_state, &_frame_block);
    memset(&_vote, 0, sizeof(_vote));
    ae_smart_ptr_init(&_vote, (void**)&vote, _state, ae_true);

    npoints = s->npoints;
    nvars = s->nvars;
    nclasses = s->nclasses;
    ae_assert(npoints>0, "DFOREST: integrity check failed", _state);
    ae_assert(nvars>0, "DFOREST: integrity check failed", _state);
    ae_assert(nclasses>0, "DFOREST: integrity check failed", _state);
    
    /*
     * Prepare vote buffer
     */
    rvectorsetlengthatleast(&buf->trntotals, npoints*nclasses, _state);
    rvectorsetlengthatleast(&buf->oobtotals, npoints*nclasses, _state);
    for(i=0; i<=npoints*nclasses-1; i++)
    {
        buf->trntotals.ptr.p_double[i] = (double)(0);
        buf->oobtotals.ptr.p_double[i] = (double)(0);
    }
    ivectorsetlengthatleast(&buf->trncounts, npoints, _state);
    ivectorsetlengthatleast(&buf->oobcounts, npoints, _state);
    for(i=0; i<=npoints-1; i++)
    {
        buf->trncounts.ptr.p_int[i] = 0;
        buf->oobcounts.ptr.p_int[i] = 0;
    }
    
    /*
     * Merge voting arrays
     */
    ae_shared_pool_first_recycled(&s->votepool, &_vote, _state);
    while(vote!=NULL)
    {
        for(i=0; i<=npoints*nclasses-1; i++)
        {
            buf->trntotals.ptr.p_double[i] = buf->trntotals.ptr.p_double[i]+vote->trntotals.ptr.p_double[i]+vote->oobtotals.ptr.p_double[i];
            buf->oobtotals.ptr.p_double[i] = buf->oobtotals.ptr.p_double[i]+vote->oobtotals.ptr.p_double[i];
        }
        for(i=0; i<=npoints-1; i++)
        {
            buf->trncounts.ptr.p_int[i] = buf->trncounts.ptr.p_int[i]+vote->trncounts.ptr.p_int[i]+vote->oobcounts.ptr.p_int[i];
            buf->oobcounts.ptr.p_int[i] = buf->oobcounts.ptr.p_int[i]+vote->oobcounts.ptr.p_int[i];
        }
        ae_shared_pool_next_recycled(&s->votepool, &_vote, _state);
    }
    for(i=0; i<=npoints-1; i++)
    {
        v = (double)1/coalesce((double)(buf->trncounts.ptr.p_int[i]), (double)(1), _state);
        for(j=0; j<=nclasses-1; j++)
        {
            buf->trntotals.ptr.p_double[i*nclasses+j] = buf->trntotals.ptr.p_double[i*nclasses+j]*v;
        }
        v = (double)1/coalesce((double)(buf->oobcounts.ptr.p_int[i]), (double)(1), _state);
        for(j=0; j<=nclasses-1; j++)
        {
            buf->oobtotals.ptr.p_double[i*nclasses+j] = buf->oobtotals.ptr.p_double[i*nclasses+j]*v;
        }
    }
    
    /*
     * Use aggregated voting data to output error metrics
     */
    avgrelcnt = 0;
    oobavgrelcnt = 0;
    rep->rmserror = (double)(0);
    rep->avgerror = (double)(0);
    rep->avgrelerror = (double)(0);
    rep->relclserror = (double)(0);
    rep->avgce = (double)(0);
    rep->oobrmserror = (double)(0);
    rep->oobavgerror = (double)(0);
    rep->oobavgrelerror = (double)(0);
    rep->oobrelclserror = (double)(0);
    rep->oobavgce = (double)(0);
    for(i=0; i<=npoints-1; i++)
    {
        if( nclasses>1 )
        {
            
            /*
             * classification-specific code
             */
            k = s->dsival.ptr.p_int[i];
            for(j=0; j<=nclasses-1; j++)
            {
                v = buf->trntotals.ptr.p_double[i*nclasses+j];
                if( j==k )
                {
                    rep->avgce = rep->avgce-ae_log(coalesce(v, ae_minrealnumber, _state), _state);
                    rep->rmserror = rep->rmserror+ae_sqr(v-(double)1, _state);
                    rep->avgerror = rep->avgerror+ae_fabs(v-(double)1, _state);
                    rep->avgrelerror = rep->avgrelerror+ae_fabs(v-(double)1, _state);
                    inc(&avgrelcnt, _state);
                }
                else
                {
                    rep->rmserror = rep->rmserror+ae_sqr(v, _state);
                    rep->avgerror = rep->avgerror+ae_fabs(v, _state);
                }
                v = buf->oobtotals.ptr.p_double[i*nclasses+j];
                if( j==k )
                {
                    rep->oobavgce = rep->oobavgce-ae_log(coalesce(v, ae_minrealnumber, _state), _state);
                    rep->oobrmserror = rep->oobrmserror+ae_sqr(v-(double)1, _state);
                    rep->oobavgerror = rep->oobavgerror+ae_fabs(v-(double)1, _state);
                    rep->oobavgrelerror = rep->oobavgrelerror+ae_fabs(v-(double)1, _state);
                    inc(&oobavgrelcnt, _state);
                }
                else
                {
                    rep->oobrmserror = rep->oobrmserror+ae_sqr(v, _state);
                    rep->oobavgerror = rep->oobavgerror+ae_fabs(v, _state);
                }
            }
            
            /*
             * Classification errors are handled separately
             */
            k1 = 0;
            for(j=1; j<=nclasses-1; j++)
            {
                if( buf->trntotals.ptr.p_double[i*nclasses+j]>buf->trntotals.ptr.p_double[i*nclasses+k1] )
                {
                    k1 = j;
                }
            }
            if( k1!=k )
            {
                rep->relclserror = rep->relclserror+(double)1;
            }
            k1 = 0;
            for(j=1; j<=nclasses-1; j++)
            {
                if( buf->oobtotals.ptr.p_double[i*nclasses+j]>buf->oobtotals.ptr.p_double[i*nclasses+k1] )
                {
                    k1 = j;
                }
            }
            if( k1!=k )
            {
                rep->oobrelclserror = rep->oobrelclserror+(double)1;
            }
        }
        else
        {
            
            /*
             * regression-specific code
             */
            v = buf->trntotals.ptr.p_double[i]-s->dsrval.ptr.p_double[i];
            rep->rmserror = rep->rmserror+ae_sqr(v, _state);
            rep->avgerror = rep->avgerror+ae_fabs(v, _state);
            if( ae_fp_neq(s->dsrval.ptr.p_double[i],(double)(0)) )
            {
                rep->avgrelerror = rep->avgrelerror+ae_fabs(v/s->dsrval.ptr.p_double[i], _state);
                avgrelcnt = avgrelcnt+1;
            }
            v = buf->oobtotals.ptr.p_double[i]-s->dsrval.ptr.p_double[i];
            rep->oobrmserror = rep->oobrmserror+ae_sqr(v, _state);
            rep->oobavgerror = rep->oobavgerror+ae_fabs(v, _state);
            if( ae_fp_neq(s->dsrval.ptr.p_double[i],(double)(0)) )
            {
                rep->oobavgrelerror = rep->oobavgrelerror+ae_fabs(v/s->dsrval.ptr.p_double[i], _state);
                oobavgrelcnt = oobavgrelcnt+1;
            }
        }
    }
    rep->relclserror = rep->relclserror/(double)npoints;
    rep->rmserror = ae_sqrt(rep->rmserror/(double)(npoints*nclasses), _state);
    rep->avgerror = rep->avgerror/(double)(npoints*nclasses);
    rep->avgrelerror = rep->avgrelerror/coalesce((double)(avgrelcnt), (double)(1), _state);
    rep->oobrelclserror = rep->oobrelclserror/(double)npoints;
    rep->oobrmserror = ae_sqrt(rep->oobrmserror/(double)(npoints*nclasses), _state);
    rep->oobavgerror = rep->oobavgerror/(double)(npoints*nclasses);
    rep->oobavgrelerror = rep->oobavgrelerror/coalesce((double)(oobavgrelcnt), (double)(1), _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function performs binary compression of decision forest, using either
8-bit mantissa (a bit more compact representation) or 16-bit mantissa  for
splits and regression outputs.

Forest is compressed in-place.

Return value is a compression factor.

  -- ALGLIB --
     Copyright 22.07.2019 by Bochkanov Sergey
*************************************************************************/
static double dforest_binarycompression(decisionforest* df,
     ae_bool usemantissa8,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t size8;
    ae_int_t size8i;
    ae_int_t offssrc;
    ae_int_t offsdst;
    ae_int_t i;
    ae_vector dummyi;
    ae_int_t maxrawtreesize;
    ae_vector compressedsizes;
    double result;

    ae_frame_make(_state, &_frame_block);
    memset(&dummyi, 0, sizeof(dummyi));
    memset(&compressedsizes, 0, sizeof(compressedsizes));
    ae_vector_init(&dummyi, 0, DT_INT, _state, ae_true);
    ae_vector_init(&compressedsizes, 0, DT_INT, _state, ae_true);

    
    /*
     * Quick exit if already compressed
     */
    if( df->forestformat==dforest_dfcompressedv0 )
    {
        result = (double)(1);
        ae_frame_leave(_state);
        return result;
    }
    
    /*
     * Check that source format is supported
     */
    ae_assert(df->forestformat==dforest_dfuncompressedv0, "BinaryCompression: unexpected forest format", _state);
    
    /*
     * Compute sizes of uncompressed and compressed trees.
     */
    size8 = 0;
    offssrc = 0;
    maxrawtreesize = 0;
    for(i=0; i<=df->ntrees-1; i++)
    {
        size8i = dforest_computecompressedsizerec(df, usemantissa8, offssrc, offssrc+1, &dummyi, ae_false, _state);
        size8 = size8+dforest_computecompresseduintsize(size8i, _state)+size8i;
        maxrawtreesize = ae_maxint(maxrawtreesize, ae_round(df->trees.ptr.p_double[offssrc], _state), _state);
        offssrc = offssrc+ae_round(df->trees.ptr.p_double[offssrc], _state);
    }
    result = (double)(8*df->trees.cnt)/(double)(size8+1);
    
    /*
     * Allocate memory and perform compression
     */
    ae_vector_set_length(&(df->trees8), size8, _state);
    ae_vector_set_length(&compressedsizes, maxrawtreesize, _state);
    offssrc = 0;
    offsdst = 0;
    for(i=0; i<=df->ntrees-1; i++)
    {
        
        /*
         * Call compressed size evaluator one more time, now saving subtree sizes into temporary array
         */
        size8i = dforest_computecompressedsizerec(df, usemantissa8, offssrc, offssrc+1, &compressedsizes, ae_true, _state);
        
        /*
         * Output tree header (length in bytes)
         */
        dforest_streamuint(&df->trees8, &offsdst, size8i, _state);
        
        /*
         * Compress recursively
         */
        dforest_compressrec(df, usemantissa8, offssrc, offssrc+1, &compressedsizes, &df->trees8, &offsdst, _state);
        
        /*
         * Next tree
         */
        offssrc = offssrc+ae_round(df->trees.ptr.p_double[offssrc], _state);
    }
    ae_assert(offsdst==size8, "BinaryCompression: integrity check failed (stream length)", _state);
    
    /*
     * Finalize forest conversion, clear previously allocated memory
     */
    df->forestformat = dforest_dfcompressedv0;
    df->usemantissa8 = usemantissa8;
    ae_vector_set_length(&df->trees, 0, _state);
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
This function returns exact number of bytes required to  store  compressed
version of the tree starting at location TreeBase.

PARAMETERS:
    DF              -   decision forest
    UseMantissa8    -   whether 8-bit or 16-bit mantissas are used to store
                        floating point numbers
    TreeRoot        -   root of the specific tree being stored (offset in DF.Trees)
    TreePos         -   position within tree (first location in the tree
                        is TreeRoot+1)
    CompressedSizes -   not referenced if SaveCompressedSizes is False;
                        otherwise, values computed by this function for
                        specific values of TreePos are stored to
                        CompressedSizes[TreePos-TreeRoot] (other elements
                        of the array are not referenced).
                        This array must be preallocated by caller.

  -- ALGLIB --
     Copyright 22.07.2019 by Bochkanov Sergey
*************************************************************************/
static ae_int_t dforest_computecompressedsizerec(const decisionforest* df,
     ae_bool usemantissa8,
     ae_int_t treeroot,
     ae_int_t treepos,
     /* Integer */ ae_vector* compressedsizes,
     ae_bool savecompressedsizes,
     ae_state *_state)
{
    ae_int_t jmponbranch;
    ae_int_t child0size;
    ae_int_t child1size;
    ae_int_t fpwidth;
    ae_int_t result;


    if( usemantissa8 )
    {
        fpwidth = 2;
    }
    else
    {
        fpwidth = 3;
    }
    
    /*
     * Leaf or split?
     */
    if( ae_fp_eq(df->trees.ptr.p_double[treepos],(double)(-1)) )
    {
        
        /*
         * Leaf
         */
        result = dforest_computecompresseduintsize(2*df->nvars, _state);
        if( df->nclasses==1 )
        {
            result = result+fpwidth;
        }
        else
        {
            result = result+dforest_computecompresseduintsize(ae_round(df->trees.ptr.p_double[treepos+1], _state), _state);
        }
    }
    else
    {
        
        /*
         * Split
         */
        jmponbranch = ae_round(df->trees.ptr.p_double[treepos+2], _state);
        child0size = dforest_computecompressedsizerec(df, usemantissa8, treeroot, treepos+dforest_innernodewidth, compressedsizes, savecompressedsizes, _state);
        child1size = dforest_computecompressedsizerec(df, usemantissa8, treeroot, treeroot+jmponbranch, compressedsizes, savecompressedsizes, _state);
        if( child0size<=child1size )
        {
            
            /*
             * Child #0 comes first because it is shorter
             */
            result = dforest_computecompresseduintsize(ae_round(df->trees.ptr.p_double[treepos], _state), _state);
            result = result+fpwidth;
            result = result+dforest_computecompresseduintsize(child0size, _state);
        }
        else
        {
            
            /*
             * Child #1 comes first because it is shorter
             */
            result = dforest_computecompresseduintsize(ae_round(df->trees.ptr.p_double[treepos], _state)+df->nvars, _state);
            result = result+fpwidth;
            result = result+dforest_computecompresseduintsize(child1size, _state);
        }
        result = result+child0size+child1size;
    }
    
    /*
     * Do we have to save compressed sizes?
     */
    if( savecompressedsizes )
    {
        ae_assert(treepos-treeroot<compressedsizes->cnt, "ComputeCompressedSizeRec: integrity check failed", _state);
        compressedsizes->ptr.p_int[treepos-treeroot] = result;
    }
    return result;
}


/*************************************************************************
This function returns exact number of bytes required to  store  compressed
version of the tree starting at location TreeBase.

PARAMETERS:
    DF              -   decision forest
    UseMantissa8    -   whether 8-bit or 16-bit mantissas are used to store
                        floating point numbers
    TreeRoot        -   root of the specific tree being stored (offset in DF.Trees)
    TreePos         -   position within tree (first location in the tree
                        is TreeRoot+1)
    CompressedSizes -   not referenced if SaveCompressedSizes is False;
                        otherwise, values computed by this function for
                        specific values of TreePos are stored to
                        CompressedSizes[TreePos-TreeRoot] (other elements
                        of the array are not referenced).
                        This array must be preallocated by caller.

  -- ALGLIB --
     Copyright 22.07.2019 by Bochkanov Sergey
*************************************************************************/
static void dforest_compressrec(const decisionforest* df,
     ae_bool usemantissa8,
     ae_int_t treeroot,
     ae_int_t treepos,
     /* Integer */ const ae_vector* compressedsizes,
     ae_vector* buf,
     ae_int_t* dstoffs,
     ae_state *_state)
{
    ae_int_t jmponbranch;
    ae_int_t child0size;
    ae_int_t child1size;
    ae_int_t varidx;
    double leafval;
    double splitval;
    ae_int_t dstoffsold;


    dstoffsold = *dstoffs;
    
    /*
     * Leaf or split?
     */
    varidx = ae_round(df->trees.ptr.p_double[treepos], _state);
    if( varidx==-1 )
    {
        
        /*
         * Leaf node:
         * * stream special value which denotes leaf (2*NVars)
         * * then, stream scalar value (floating point) or class number (unsigned integer)
         */
        leafval = df->trees.ptr.p_double[treepos+1];
        dforest_streamuint(buf, dstoffs, 2*df->nvars, _state);
        if( df->nclasses==1 )
        {
            dforest_streamfloat(buf, usemantissa8, dstoffs, leafval, _state);
        }
        else
        {
            dforest_streamuint(buf, dstoffs, ae_round(leafval, _state), _state);
        }
    }
    else
    {
        
        /*
         * Split node:
         * * fetch compressed sizes of child nodes, decide which child goes first
         */
        jmponbranch = ae_round(df->trees.ptr.p_double[treepos+2], _state);
        splitval = df->trees.ptr.p_double[treepos+1];
        child0size = compressedsizes->ptr.p_int[treepos+dforest_innernodewidth-treeroot];
        child1size = compressedsizes->ptr.p_int[treeroot+jmponbranch-treeroot];
        if( child0size<=child1size )
        {
            
            /*
             * Child #0 comes first because it is shorter:
             * * stream variable index used for splitting;
             *   value in [0,NVars) range indicates that split is
             *   "if VAR<VAL then BRANCH0 else BRANCH1"
             * * stream value used for splitting
             * * stream children #0 and #1
             */
            dforest_streamuint(buf, dstoffs, varidx, _state);
            dforest_streamfloat(buf, usemantissa8, dstoffs, splitval, _state);
            dforest_streamuint(buf, dstoffs, child0size, _state);
            dforest_compressrec(df, usemantissa8, treeroot, treepos+dforest_innernodewidth, compressedsizes, buf, dstoffs, _state);
            dforest_compressrec(df, usemantissa8, treeroot, treeroot+jmponbranch, compressedsizes, buf, dstoffs, _state);
        }
        else
        {
            
            /*
             * Child #1 comes first because it is shorter:
             * * stream variable index used for splitting + NVars;
             *   value in [NVars,2*NVars) range indicates that split is
             *   "if VAR>=VAL then BRANCH0 else BRANCH1"
             * * stream value used for splitting
             * * stream children #0 and #1
             */
            dforest_streamuint(buf, dstoffs, varidx+df->nvars, _state);
            dforest_streamfloat(buf, usemantissa8, dstoffs, splitval, _state);
            dforest_streamuint(buf, dstoffs, child1size, _state);
            dforest_compressrec(df, usemantissa8, treeroot, treeroot+jmponbranch, compressedsizes, buf, dstoffs, _state);
            dforest_compressrec(df, usemantissa8, treeroot, treepos+dforest_innernodewidth, compressedsizes, buf, dstoffs, _state);
        }
    }
    
    /*
     * Integrity check at the end
     */
    ae_assert(*dstoffs-dstoffsold==compressedsizes->ptr.p_int[treepos-treeroot], "CompressRec: integrity check failed (compressed size at leaf)", _state);
}


/*************************************************************************
This function returns exact number of bytes required to  store  compressed
unsigned integer number (negative  arguments  result  in  assertion  being
generated).

  -- ALGLIB --
     Copyright 22.07.2019 by Bochkanov Sergey
*************************************************************************/
static ae_int_t dforest_computecompresseduintsize(ae_int_t v,
     ae_state *_state)
{
    ae_int_t result;


    ae_assert(v>=0, "Assertion failed", _state);
    result = 1;
    while(v>=128)
    {
        v = v/128;
        result = result+1;
    }
    return result;
}


/*************************************************************************
This function stores compressed unsigned integer number (negative arguments
result in assertion being generated) to byte array at  location  Offs  and
increments Offs by number of bytes being stored.

  -- ALGLIB --
     Copyright 22.07.2019 by Bochkanov Sergey
*************************************************************************/
static void dforest_streamuint(ae_vector* buf,
     ae_int_t* offs,
     ae_int_t v,
     ae_state *_state)
{
    ae_int_t v0;


    ae_assert(v>=0, "Assertion failed", _state);
    for(;;)
    {
        
        /*
         * Save 7 least significant bits of V, use 8th bit as a flag which
         * tells us whether subsequent 7-bit packages will be sent.
         */
        v0 = v%128;
        if( v>=128 )
        {
            v0 = v0+128;
        }
        buf->ptr.p_ubyte[*(offs)] = (unsigned char)(v0);
        *offs = *offs+1;
        v = v/128;
        if( v==0 )
        {
            break;
        }
    }
}


/*************************************************************************
This function reads compressed unsigned integer number from byte array
starting at location Offs and increments Offs by number of bytes being
read.

  -- ALGLIB --
     Copyright 22.07.2019 by Bochkanov Sergey
*************************************************************************/
static ae_int_t dforest_unstreamuint(const ae_vector* buf,
     ae_int_t* offs,
     ae_state *_state)
{
    ae_int_t v0;
    ae_int_t p;
    ae_int_t result;


    result = 0;
    p = 1;
    for(;;)
    {
        
        /*
         * Rad 7 bits of V, use 8th bit as a flag which tells us whether
         * subsequent 7-bit packages will be received.
         */
        v0 = buf->ptr.p_ubyte[*(offs)];
        *offs = *offs+1;
        result = result+v0%128*p;
        if( v0<128 )
        {
            break;
        }
        p = p*128;
    }
    return result;
}


/*************************************************************************
This function stores compressed floating point number  to  byte  array  at
location  Offs and increments Offs by number of bytes being stored.

Either 8-bit mantissa or 16-bit mantissa is used. The exponent  is  always
7 bits of exponent + sign. Values which do not fit into exponent range are
truncated to fit.

  -- ALGLIB --
     Copyright 22.07.2019 by Bochkanov Sergey
*************************************************************************/
static void dforest_streamfloat(ae_vector* buf,
     ae_bool usemantissa8,
     ae_int_t* offs,
     double v,
     ae_state *_state)
{
    ae_int_t signbit;
    ae_int_t e;
    ae_int_t m;
    double twopow30;
    double twopowm30;
    double twopow10;
    double twopowm10;


    ae_assert(ae_isfinite(v, _state), "StreamFloat: V is not finite number", _state);
    
    /*
     * Special case: zero
     */
    if( v==0.0 )
    {
        if( usemantissa8 )
        {
            buf->ptr.p_ubyte[*offs+0] = (unsigned char)(0);
            buf->ptr.p_ubyte[*offs+1] = (unsigned char)(0);
            *offs = *offs+2;
        }
        else
        {
            buf->ptr.p_ubyte[*offs+0] = (unsigned char)(0);
            buf->ptr.p_ubyte[*offs+1] = (unsigned char)(0);
            buf->ptr.p_ubyte[*offs+2] = (unsigned char)(0);
            *offs = *offs+3;
        }
        return;
    }
    
    /*
     * Handle sign
     */
    signbit = 0;
    if( v<0.0 )
    {
        v = -v;
        signbit = 128;
    }
    
    /*
     * Compute exponent
     */
    twopow30 = (double)(1073741824);
    twopow10 = (double)(1024);
    twopowm30 = 1.0/twopow30;
    twopowm10 = 1.0/twopow10;
    e = 0;
    while(v>=twopow30)
    {
        v = v*twopowm30;
        e = e+30;
    }
    while(v>=twopow10)
    {
        v = v*twopowm10;
        e = e+10;
    }
    while(v>=1.0)
    {
        v = v*0.5;
        e = e+1;
    }
    while(v<twopowm30)
    {
        v = v*twopow30;
        e = e-30;
    }
    while(v<twopowm10)
    {
        v = v*twopow10;
        e = e-10;
    }
    while(v<0.5)
    {
        v = v*(double)2;
        e = e-1;
    }
    ae_assert(v>=0.5&&v<1.0, "StreamFloat: integrity check failed", _state);
    
    /*
     * Handle exponent underflow/overflow
     */
    if( e<-63 )
    {
        signbit = 0;
        e = 0;
        v = (double)(0);
    }
    if( e>63 )
    {
        e = 63;
        v = 1.0;
    }
    
    /*
     * Save to stream
     */
    if( usemantissa8 )
    {
        m = ae_round(v*(double)256, _state);
        if( m==256 )
        {
            m = m/2;
            e = ae_minint(e+1, 63, _state);
        }
        buf->ptr.p_ubyte[*offs+0] = (unsigned char)(e+64+signbit);
        buf->ptr.p_ubyte[*offs+1] = (unsigned char)(m);
        *offs = *offs+2;
    }
    else
    {
        m = ae_round(v*(double)65536, _state);
        if( m==65536 )
        {
            m = m/2;
            e = ae_minint(e+1, 63, _state);
        }
        buf->ptr.p_ubyte[*offs+0] = (unsigned char)(e+64+signbit);
        buf->ptr.p_ubyte[*offs+1] = (unsigned char)(m%256);
        buf->ptr.p_ubyte[*offs+2] = (unsigned char)(m/256);
        *offs = *offs+3;
    }
}


/*************************************************************************
This function reads compressed floating point number from the byte array
starting from location Offs and increments Offs by number of bytes being
read.

Either 8-bit mantissa or 16-bit mantissa is used. The exponent  is  always
7 bits of exponent + sign. Values which do not fit into exponent range are
truncated to fit.

  -- ALGLIB --
     Copyright 22.07.2019 by Bochkanov Sergey
*************************************************************************/
static double dforest_unstreamfloat(const ae_vector* buf,
     ae_bool usemantissa8,
     ae_int_t* offs,
     ae_state *_state)
{
    ae_int_t e;
    double v;
    double inv256;
    double result;


    
    /*
     * Read from stream
     */
    inv256 = 1.0/256.0;
    if( usemantissa8 )
    {
        e = buf->ptr.p_ubyte[*offs+0];
        v = (double)buf->ptr.p_ubyte[*offs+1]*inv256;
        *offs = *offs+2;
    }
    else
    {
        e = buf->ptr.p_ubyte[*offs+0];
        v = ((double)buf->ptr.p_ubyte[*offs+1]*inv256+(double)buf->ptr.p_ubyte[*offs+2])*inv256;
        *offs = *offs+3;
    }
    
    /*
     * Decode
     */
    if( e>128 )
    {
        v = -v;
        e = e-128;
    }
    e = e-64;
    result = dforest_xfastpow((double)(2), e, _state)*v;
    return result;
}


/*************************************************************************
Classification error
*************************************************************************/
static ae_int_t dforest_dfclserror(const decisionforest* df,
     /* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector x;
    ae_vector y;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t tmpi;
    ae_int_t result;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&y, 0, sizeof(y));
    ae_vector_init(&x, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&y, 0, DT_REAL, _state, ae_true);

    if( df->nclasses<=1 )
    {
        result = 0;
        ae_frame_leave(_state);
        return result;
    }
    ae_vector_set_length(&x, df->nvars-1+1, _state);
    ae_vector_set_length(&y, df->nclasses-1+1, _state);
    result = 0;
    for(i=0; i<=npoints-1; i++)
    {
        ae_v_move(&x.ptr.p_double[0], 1, &xy->ptr.pp_double[i][0], 1, ae_v_len(0,df->nvars-1));
        dfprocess(df, &x, &y, _state);
        k = ae_round(xy->ptr.pp_double[i][df->nvars], _state);
        tmpi = 0;
        for(j=1; j<=df->nclasses-1; j++)
        {
            if( ae_fp_greater(y.ptr.p_double[j],y.ptr.p_double[tmpi]) )
            {
                tmpi = j;
            }
        }
        if( tmpi!=k )
        {
            result = result+1;
        }
    }
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Internal subroutine for processing one decision tree stored in uncompressed
format starting at SubtreeRoot (this index points to the header of the tree,
not its first node). First node being processed is located at NodeOffs.
*************************************************************************/
static void dforest_dfprocessinternaluncompressed(const decisionforest* df,
     ae_int_t subtreeroot,
     ae_int_t nodeoffs,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t idx;


    ae_assert(df->forestformat==dforest_dfuncompressedv0, "DFProcessInternal: unexpected forest format", _state);
    
    /*
     * Navigate through the tree
     */
    for(;;)
    {
        if( ae_fp_eq(df->trees.ptr.p_double[nodeoffs],(double)(-1)) )
        {
            if( df->nclasses==1 )
            {
                y->ptr.p_double[0] = y->ptr.p_double[0]+df->trees.ptr.p_double[nodeoffs+1];
            }
            else
            {
                idx = ae_round(df->trees.ptr.p_double[nodeoffs+1], _state);
                y->ptr.p_double[idx] = y->ptr.p_double[idx]+(double)1;
            }
            break;
        }
        if( x->ptr.p_double[ae_round(df->trees.ptr.p_double[nodeoffs], _state)]<df->trees.ptr.p_double[nodeoffs+1] )
        {
            nodeoffs = nodeoffs+dforest_innernodewidth;
        }
        else
        {
            nodeoffs = subtreeroot+ae_round(df->trees.ptr.p_double[nodeoffs+2], _state);
        }
    }
}


/*************************************************************************
Internal subroutine for processing one decision tree stored in compressed
format starting at Offs (this index points to the first node of the tree,
right past the header field).
*************************************************************************/
static void dforest_dfprocessinternalcompressed(const decisionforest* df,
     ae_int_t offs,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t leafindicator;
    ae_int_t varidx;
    double splitval;
    ae_int_t jmplen;
    double leafval;
    ae_int_t leafcls;


    ae_assert(df->forestformat==dforest_dfcompressedv0, "DFProcessInternal: unexpected forest format", _state);
    
    /*
     * Navigate through the tree
     */
    leafindicator = 2*df->nvars;
    for(;;)
    {
        
        /*
         * Read variable idx
         */
        varidx = dforest_unstreamuint(&df->trees8, &offs, _state);
        
        /*
         * Is it leaf?
         */
        if( varidx==leafindicator )
        {
            if( df->nclasses==1 )
            {
                
                /*
                 * Regression forest
                 */
                leafval = dforest_unstreamfloat(&df->trees8, df->usemantissa8, &offs, _state);
                y->ptr.p_double[0] = y->ptr.p_double[0]+leafval;
            }
            else
            {
                
                /*
                 * Classification forest
                 */
                leafcls = dforest_unstreamuint(&df->trees8, &offs, _state);
                y->ptr.p_double[leafcls] = y->ptr.p_double[leafcls]+(double)1;
            }
            break;
        }
        
        /*
         * Process node
         */
        splitval = dforest_unstreamfloat(&df->trees8, df->usemantissa8, &offs, _state);
        jmplen = dforest_unstreamuint(&df->trees8, &offs, _state);
        if( varidx<df->nvars )
        {
            
            /*
             * The split rule is "if VAR<VAL then BRANCH0 else BRANCH1"
             */
            if( x->ptr.p_double[varidx]>=splitval )
            {
                offs = offs+jmplen;
            }
        }
        else
        {
            
            /*
             * The split rule is "if VAR>=VAL then BRANCH0 else BRANCH1"
             */
            varidx = varidx-df->nvars;
            if( x->ptr.p_double[varidx]<splitval )
            {
                offs = offs+jmplen;
            }
        }
    }
}


/*************************************************************************
Fast Pow

  -- ALGLIB --
     Copyright 24.08.2009 by Bochkanov Sergey
*************************************************************************/
static double dforest_xfastpow(double r, ae_int_t n, ae_state *_state)
{
    double result;


    result = (double)(0);
    if( n>0 )
    {
        if( n%2==0 )
        {
            result = dforest_xfastpow(r, n/2, _state);
            result = result*result;
        }
        else
        {
            result = r*dforest_xfastpow(r, n-1, _state);
        }
        return result;
    }
    if( n==0 )
    {
        result = (double)(1);
    }
    if( n<0 )
    {
        result = dforest_xfastpow((double)1/r, -n, _state);
    }
    return result;
}


void _decisionforestbuilder_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    decisionforestbuilder *p = (decisionforestbuilder*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->dsdata, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dsrval, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dsival, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->dsmin, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dsmax, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dsbinary, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->dsctotals, 0, DT_INT, _state, make_automatic);
    ae_shared_pool_init(&p->workpool, _state, make_automatic);
    ae_shared_pool_init(&p->votepool, _state, make_automatic);
    ae_shared_pool_init(&p->treepool, _state, make_automatic);
    ae_shared_pool_init(&p->treefactory, _state, make_automatic);
    ae_matrix_init(&p->iobmatrix, 0, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->varimpshuffle2, 0, DT_INT, _state, make_automatic);
}


void _decisionforestbuilder_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    decisionforestbuilder       *dst = (decisionforestbuilder*)_dst;
    const decisionforestbuilder *src = (const decisionforestbuilder*)_src;
    dst->dstype = src->dstype;
    dst->npoints = src->npoints;
    dst->nvars = src->nvars;
    dst->nclasses = src->nclasses;
    ae_vector_init_copy(&dst->dsdata, &src->dsdata, _state, make_automatic);
    ae_vector_init_copy(&dst->dsrval, &src->dsrval, _state, make_automatic);
    ae_vector_init_copy(&dst->dsival, &src->dsival, _state, make_automatic);
    dst->rdfalgo = src->rdfalgo;
    dst->rdfratio = src->rdfratio;
    dst->rdfvars = src->rdfvars;
    dst->rdfglobalseed = src->rdfglobalseed;
    dst->rdfsplitstrength = src->rdfsplitstrength;
    dst->rdfimportance = src->rdfimportance;
    ae_vector_init_copy(&dst->dsmin, &src->dsmin, _state, make_automatic);
    ae_vector_init_copy(&dst->dsmax, &src->dsmax, _state, make_automatic);
    ae_vector_init_copy(&dst->dsbinary, &src->dsbinary, _state, make_automatic);
    dst->dsravg = src->dsravg;
    ae_vector_init_copy(&dst->dsctotals, &src->dsctotals, _state, make_automatic);
    dst->rdfprogress = src->rdfprogress;
    dst->rdftotal = src->rdftotal;
    ae_shared_pool_init_copy(&dst->workpool, &src->workpool, _state, make_automatic);
    ae_shared_pool_init_copy(&dst->votepool, &src->votepool, _state, make_automatic);
    ae_shared_pool_init_copy(&dst->treepool, &src->treepool, _state, make_automatic);
    ae_shared_pool_init_copy(&dst->treefactory, &src->treefactory, _state, make_automatic);
    dst->neediobmatrix = src->neediobmatrix;
    ae_matrix_init_copy(&dst->iobmatrix, &src->iobmatrix, _state, make_automatic);
    ae_vector_init_copy(&dst->varimpshuffle2, &src->varimpshuffle2, _state, make_automatic);
}


void _decisionforestbuilder_clear(void* _p)
{
    decisionforestbuilder *p = (decisionforestbuilder*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->dsdata);
    ae_vector_clear(&p->dsrval);
    ae_vector_clear(&p->dsival);
    ae_vector_clear(&p->dsmin);
    ae_vector_clear(&p->dsmax);
    ae_vector_clear(&p->dsbinary);
    ae_vector_clear(&p->dsctotals);
    ae_shared_pool_clear(&p->workpool);
    ae_shared_pool_clear(&p->votepool);
    ae_shared_pool_clear(&p->treepool);
    ae_shared_pool_clear(&p->treefactory);
    ae_matrix_clear(&p->iobmatrix);
    ae_vector_clear(&p->varimpshuffle2);
}


void _decisionforestbuilder_destroy(void* _p)
{
    decisionforestbuilder *p = (decisionforestbuilder*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->dsdata);
    ae_vector_destroy(&p->dsrval);
    ae_vector_destroy(&p->dsival);
    ae_vector_destroy(&p->dsmin);
    ae_vector_destroy(&p->dsmax);
    ae_vector_destroy(&p->dsbinary);
    ae_vector_destroy(&p->dsctotals);
    ae_shared_pool_destroy(&p->workpool);
    ae_shared_pool_destroy(&p->votepool);
    ae_shared_pool_destroy(&p->treepool);
    ae_shared_pool_destroy(&p->treefactory);
    ae_matrix_destroy(&p->iobmatrix);
    ae_vector_destroy(&p->varimpshuffle2);
}


void _dfworkbuf_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    dfworkbuf *p = (dfworkbuf*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->classpriors, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->varpool, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->trnset, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->trnlabelsr, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->trnlabelsi, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->oobset, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->ooblabelsr, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->ooblabelsi, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->treebuf, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->curvals, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bestvals, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp0i, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->tmp1i, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->tmp0r, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp1r, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp2r, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp3r, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpnrms2, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->classtotals0, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->classtotals1, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->classtotals01, 0, DT_INT, _state, make_automatic);
}


void _dfworkbuf_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    dfworkbuf       *dst = (dfworkbuf*)_dst;
    const dfworkbuf *src = (const dfworkbuf*)_src;
    ae_vector_init_copy(&dst->classpriors, &src->classpriors, _state, make_automatic);
    ae_vector_init_copy(&dst->varpool, &src->varpool, _state, make_automatic);
    dst->varpoolsize = src->varpoolsize;
    ae_vector_init_copy(&dst->trnset, &src->trnset, _state, make_automatic);
    dst->trnsize = src->trnsize;
    ae_vector_init_copy(&dst->trnlabelsr, &src->trnlabelsr, _state, make_automatic);
    ae_vector_init_copy(&dst->trnlabelsi, &src->trnlabelsi, _state, make_automatic);
    ae_vector_init_copy(&dst->oobset, &src->oobset, _state, make_automatic);
    dst->oobsize = src->oobsize;
    ae_vector_init_copy(&dst->ooblabelsr, &src->ooblabelsr, _state, make_automatic);
    ae_vector_init_copy(&dst->ooblabelsi, &src->ooblabelsi, _state, make_automatic);
    ae_vector_init_copy(&dst->treebuf, &src->treebuf, _state, make_automatic);
    ae_vector_init_copy(&dst->curvals, &src->curvals, _state, make_automatic);
    ae_vector_init_copy(&dst->bestvals, &src->bestvals, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0i, &src->tmp0i, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp1i, &src->tmp1i, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0r, &src->tmp0r, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp1r, &src->tmp1r, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp2r, &src->tmp2r, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp3r, &src->tmp3r, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpnrms2, &src->tmpnrms2, _state, make_automatic);
    ae_vector_init_copy(&dst->classtotals0, &src->classtotals0, _state, make_automatic);
    ae_vector_init_copy(&dst->classtotals1, &src->classtotals1, _state, make_automatic);
    ae_vector_init_copy(&dst->classtotals01, &src->classtotals01, _state, make_automatic);
}


void _dfworkbuf_clear(void* _p)
{
    dfworkbuf *p = (dfworkbuf*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->classpriors);
    ae_vector_clear(&p->varpool);
    ae_vector_clear(&p->trnset);
    ae_vector_clear(&p->trnlabelsr);
    ae_vector_clear(&p->trnlabelsi);
    ae_vector_clear(&p->oobset);
    ae_vector_clear(&p->ooblabelsr);
    ae_vector_clear(&p->ooblabelsi);
    ae_vector_clear(&p->treebuf);
    ae_vector_clear(&p->curvals);
    ae_vector_clear(&p->bestvals);
    ae_vector_clear(&p->tmp0i);
    ae_vector_clear(&p->tmp1i);
    ae_vector_clear(&p->tmp0r);
    ae_vector_clear(&p->tmp1r);
    ae_vector_clear(&p->tmp2r);
    ae_vector_clear(&p->tmp3r);
    ae_vector_clear(&p->tmpnrms2);
    ae_vector_clear(&p->classtotals0);
    ae_vector_clear(&p->classtotals1);
    ae_vector_clear(&p->classtotals01);
}


void _dfworkbuf_destroy(void* _p)
{
    dfworkbuf *p = (dfworkbuf*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->classpriors);
    ae_vector_destroy(&p->varpool);
    ae_vector_destroy(&p->trnset);
    ae_vector_destroy(&p->trnlabelsr);
    ae_vector_destroy(&p->trnlabelsi);
    ae_vector_destroy(&p->oobset);
    ae_vector_destroy(&p->ooblabelsr);
    ae_vector_destroy(&p->ooblabelsi);
    ae_vector_destroy(&p->treebuf);
    ae_vector_destroy(&p->curvals);
    ae_vector_destroy(&p->bestvals);
    ae_vector_destroy(&p->tmp0i);
    ae_vector_destroy(&p->tmp1i);
    ae_vector_destroy(&p->tmp0r);
    ae_vector_destroy(&p->tmp1r);
    ae_vector_destroy(&p->tmp2r);
    ae_vector_destroy(&p->tmp3r);
    ae_vector_destroy(&p->tmpnrms2);
    ae_vector_destroy(&p->classtotals0);
    ae_vector_destroy(&p->classtotals1);
    ae_vector_destroy(&p->classtotals01);
}


void _dfvotebuf_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    dfvotebuf *p = (dfvotebuf*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->trntotals, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->oobtotals, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->trncounts, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->oobcounts, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->giniimportances, 0, DT_REAL, _state, make_automatic);
}


void _dfvotebuf_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    dfvotebuf       *dst = (dfvotebuf*)_dst;
    const dfvotebuf *src = (const dfvotebuf*)_src;
    ae_vector_init_copy(&dst->trntotals, &src->trntotals, _state, make_automatic);
    ae_vector_init_copy(&dst->oobtotals, &src->oobtotals, _state, make_automatic);
    ae_vector_init_copy(&dst->trncounts, &src->trncounts, _state, make_automatic);
    ae_vector_init_copy(&dst->oobcounts, &src->oobcounts, _state, make_automatic);
    ae_vector_init_copy(&dst->giniimportances, &src->giniimportances, _state, make_automatic);
}


void _dfvotebuf_clear(void* _p)
{
    dfvotebuf *p = (dfvotebuf*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->trntotals);
    ae_vector_clear(&p->oobtotals);
    ae_vector_clear(&p->trncounts);
    ae_vector_clear(&p->oobcounts);
    ae_vector_clear(&p->giniimportances);
}


void _dfvotebuf_destroy(void* _p)
{
    dfvotebuf *p = (dfvotebuf*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->trntotals);
    ae_vector_destroy(&p->oobtotals);
    ae_vector_destroy(&p->trncounts);
    ae_vector_destroy(&p->oobcounts);
    ae_vector_destroy(&p->giniimportances);
}


void _dfpermimpbuf_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    dfpermimpbuf *p = (dfpermimpbuf*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->losses, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xraw, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xdist, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xcur, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->y, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->yv, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->targety, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->startnodes, 0, DT_INT, _state, make_automatic);
}


void _dfpermimpbuf_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    dfpermimpbuf       *dst = (dfpermimpbuf*)_dst;
    const dfpermimpbuf *src = (const dfpermimpbuf*)_src;
    ae_vector_init_copy(&dst->losses, &src->losses, _state, make_automatic);
    ae_vector_init_copy(&dst->xraw, &src->xraw, _state, make_automatic);
    ae_vector_init_copy(&dst->xdist, &src->xdist, _state, make_automatic);
    ae_vector_init_copy(&dst->xcur, &src->xcur, _state, make_automatic);
    ae_vector_init_copy(&dst->y, &src->y, _state, make_automatic);
    ae_vector_init_copy(&dst->yv, &src->yv, _state, make_automatic);
    ae_vector_init_copy(&dst->targety, &src->targety, _state, make_automatic);
    ae_vector_init_copy(&dst->startnodes, &src->startnodes, _state, make_automatic);
}


void _dfpermimpbuf_clear(void* _p)
{
    dfpermimpbuf *p = (dfpermimpbuf*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->losses);
    ae_vector_clear(&p->xraw);
    ae_vector_clear(&p->xdist);
    ae_vector_clear(&p->xcur);
    ae_vector_clear(&p->y);
    ae_vector_clear(&p->yv);
    ae_vector_clear(&p->targety);
    ae_vector_clear(&p->startnodes);
}


void _dfpermimpbuf_destroy(void* _p)
{
    dfpermimpbuf *p = (dfpermimpbuf*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->losses);
    ae_vector_destroy(&p->xraw);
    ae_vector_destroy(&p->xdist);
    ae_vector_destroy(&p->xcur);
    ae_vector_destroy(&p->y);
    ae_vector_destroy(&p->yv);
    ae_vector_destroy(&p->targety);
    ae_vector_destroy(&p->startnodes);
}


void _dftreebuf_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    dftreebuf *p = (dftreebuf*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->treebuf, 0, DT_REAL, _state, make_automatic);
}


void _dftreebuf_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    dftreebuf       *dst = (dftreebuf*)_dst;
    const dftreebuf *src = (const dftreebuf*)_src;
    ae_vector_init_copy(&dst->treebuf, &src->treebuf, _state, make_automatic);
    dst->treeidx = src->treeidx;
}


void _dftreebuf_clear(void* _p)
{
    dftreebuf *p = (dftreebuf*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->treebuf);
}


void _dftreebuf_destroy(void* _p)
{
    dftreebuf *p = (dftreebuf*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->treebuf);
}


void _decisionforestbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    decisionforestbuffer *p = (decisionforestbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->y, 0, DT_REAL, _state, make_automatic);
}


void _decisionforestbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    decisionforestbuffer       *dst = (decisionforestbuffer*)_dst;
    const decisionforestbuffer *src = (const decisionforestbuffer*)_src;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->y, &src->y, _state, make_automatic);
}


void _decisionforestbuffer_clear(void* _p)
{
    decisionforestbuffer *p = (decisionforestbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->y);
}


void _decisionforestbuffer_destroy(void* _p)
{
    decisionforestbuffer *p = (decisionforestbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->y);
}


void _decisionforest_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    decisionforest *p = (decisionforest*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->trees, 0, DT_REAL, _state, make_automatic);
    _decisionforestbuffer_init(&p->buffer, _state, make_automatic);
    ae_vector_init(&p->trees8, 0, DT_BYTE, _state, make_automatic);
}


void _decisionforest_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    decisionforest       *dst = (decisionforest*)_dst;
    const decisionforest *src = (const decisionforest*)_src;
    dst->forestformat = src->forestformat;
    dst->usemantissa8 = src->usemantissa8;
    dst->nvars = src->nvars;
    dst->nclasses = src->nclasses;
    dst->ntrees = src->ntrees;
    dst->bufsize = src->bufsize;
    ae_vector_init_copy(&dst->trees, &src->trees, _state, make_automatic);
    _decisionforestbuffer_init_copy(&dst->buffer, &src->buffer, _state, make_automatic);
    ae_vector_init_copy(&dst->trees8, &src->trees8, _state, make_automatic);
}


void _decisionforest_clear(void* _p)
{
    decisionforest *p = (decisionforest*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->trees);
    _decisionforestbuffer_clear(&p->buffer);
    ae_vector_clear(&p->trees8);
}


void _decisionforest_destroy(void* _p)
{
    decisionforest *p = (decisionforest*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->trees);
    _decisionforestbuffer_destroy(&p->buffer);
    ae_vector_destroy(&p->trees8);
}


void _dfreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    dfreport *p = (dfreport*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->topvars, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->varimportances, 0, DT_REAL, _state, make_automatic);
}


void _dfreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    dfreport       *dst = (dfreport*)_dst;
    const dfreport *src = (const dfreport*)_src;
    dst->relclserror = src->relclserror;
    dst->avgce = src->avgce;
    dst->rmserror = src->rmserror;
    dst->avgerror = src->avgerror;
    dst->avgrelerror = src->avgrelerror;
    dst->oobrelclserror = src->oobrelclserror;
    dst->oobavgce = src->oobavgce;
    dst->oobrmserror = src->oobrmserror;
    dst->oobavgerror = src->oobavgerror;
    dst->oobavgrelerror = src->oobavgrelerror;
    ae_vector_init_copy(&dst->topvars, &src->topvars, _state, make_automatic);
    ae_vector_init_copy(&dst->varimportances, &src->varimportances, _state, make_automatic);
}


void _dfreport_clear(void* _p)
{
    dfreport *p = (dfreport*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->topvars);
    ae_vector_clear(&p->varimportances);
}


void _dfreport_destroy(void* _p)
{
    dfreport *p = (dfreport*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->topvars);
    ae_vector_destroy(&p->varimportances);
}


void _dfinternalbuffers_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    dfinternalbuffers *p = (dfinternalbuffers*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->treebuf, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->idxbuf, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->tmpbufr, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpbufr2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpbufi, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->classibuf, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->sortrbuf, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->sortrbuf2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->sortibuf, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->varpool, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->evsbin, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->evssplits, 0, DT_REAL, _state, make_automatic);
}


void _dfinternalbuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    dfinternalbuffers       *dst = (dfinternalbuffers*)_dst;
    const dfinternalbuffers *src = (const dfinternalbuffers*)_src;
    ae_vector_init_copy(&dst->treebuf, &src->treebuf, _state, make_automatic);
    ae_vector_init_copy(&dst->idxbuf, &src->idxbuf, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpbufr, &src->tmpbufr, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpbufr2, &src->tmpbufr2, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpbufi, &src->tmpbufi, _state, make_automatic);
    ae_vector_init_copy(&dst->classibuf, &src->classibuf, _state, make_automatic);
    ae_vector_init_copy(&dst->sortrbuf, &src->sortrbuf, _state, make_automatic);
    ae_vector_init_copy(&dst->sortrbuf2, &src->sortrbuf2, _state, make_automatic);
    ae_vector_init_copy(&dst->sortibuf, &src->sortibuf, _state, make_automatic);
    ae_vector_init_copy(&dst->varpool, &src->varpool, _state, make_automatic);
    ae_vector_init_copy(&dst->evsbin, &src->evsbin, _state, make_automatic);
    ae_vector_init_copy(&dst->evssplits, &src->evssplits, _state, make_automatic);
}


void _dfinternalbuffers_clear(void* _p)
{
    dfinternalbuffers *p = (dfinternalbuffers*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->treebuf);
    ae_vector_clear(&p->idxbuf);
    ae_vector_clear(&p->tmpbufr);
    ae_vector_clear(&p->tmpbufr2);
    ae_vector_clear(&p->tmpbufi);
    ae_vector_clear(&p->classibuf);
    ae_vector_clear(&p->sortrbuf);
    ae_vector_clear(&p->sortrbuf2);
    ae_vector_clear(&p->sortibuf);
    ae_vector_clear(&p->varpool);
    ae_vector_clear(&p->evsbin);
    ae_vector_clear(&p->evssplits);
}


void _dfinternalbuffers_destroy(void* _p)
{
    dfinternalbuffers *p = (dfinternalbuffers*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->treebuf);
    ae_vector_destroy(&p->idxbuf);
    ae_vector_destroy(&p->tmpbufr);
    ae_vector_destroy(&p->tmpbufr2);
    ae_vector_destroy(&p->tmpbufi);
    ae_vector_destroy(&p->classibuf);
    ae_vector_destroy(&p->sortrbuf);
    ae_vector_destroy(&p->sortrbuf2);
    ae_vector_destroy(&p->sortibuf);
    ae_vector_destroy(&p->varpool);
    ae_vector_destroy(&p->evsbin);
    ae_vector_destroy(&p->evssplits);
}


/*$ End $*/
