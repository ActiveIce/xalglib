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

#ifndef _dforest_h
#define _dforest_h

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
#include "basicstatops.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "basestat.h"
#include "bdss.h"


/*$ Declarations $*/


/*************************************************************************
A random forest (decision forest) builder object.

Used to store dataset and specify decision forest training algorithm settings.
*************************************************************************/
typedef struct
{
    ae_int_t dstype;
    ae_int_t npoints;
    ae_int_t nvars;
    ae_int_t nclasses;
    ae_vector dsdata;
    ae_vector dsrval;
    ae_vector dsival;
    ae_int_t rdfalgo;
    double rdfratio;
    double rdfvars;
    ae_int_t rdfglobalseed;
    ae_int_t rdfsplitstrength;
    ae_int_t rdfimportance;
    ae_vector dsmin;
    ae_vector dsmax;
    ae_vector dsbinary;
    double dsravg;
    ae_vector dsctotals;
    ae_int_t rdfprogress;
    ae_int_t rdftotal;
    ae_shared_pool workpool;
    ae_shared_pool votepool;
    ae_shared_pool treepool;
    ae_shared_pool treefactory;
    ae_bool neediobmatrix;
    ae_matrix iobmatrix;
    ae_vector varimpshuffle2;
} decisionforestbuilder;


typedef struct
{
    ae_vector classpriors;
    ae_vector varpool;
    ae_int_t varpoolsize;
    ae_vector trnset;
    ae_int_t trnsize;
    ae_vector trnlabelsr;
    ae_vector trnlabelsi;
    ae_vector oobset;
    ae_int_t oobsize;
    ae_vector ooblabelsr;
    ae_vector ooblabelsi;
    ae_vector treebuf;
    ae_vector curvals;
    ae_vector bestvals;
    ae_vector tmp0i;
    ae_vector tmp1i;
    ae_vector tmp0r;
    ae_vector tmp1r;
    ae_vector tmp2r;
    ae_vector tmp3r;
    ae_vector tmpnrms2;
    ae_vector classtotals0;
    ae_vector classtotals1;
    ae_vector classtotals01;
} dfworkbuf;


typedef struct
{
    ae_vector trntotals;
    ae_vector oobtotals;
    ae_vector trncounts;
    ae_vector oobcounts;
    ae_vector giniimportances;
} dfvotebuf;


/*************************************************************************
Permutation importance buffer object, stores permutation-related losses
for some subset of the dataset + some temporaries

Losses      -   array[NVars+2], stores sum of squared residuals for each
                permutation type:
                * Losses[0..NVars-1] stores losses for permutation in J-th variable
                * Losses[NVars] stores loss for all variables being randomly perturbed
                * Losses[NVars+1] stores loss for unperturbed dataset
*************************************************************************/
typedef struct
{
    ae_vector losses;
    ae_vector xraw;
    ae_vector xdist;
    ae_vector xcur;
    ae_vector y;
    ae_vector yv;
    ae_vector targety;
    ae_vector startnodes;
} dfpermimpbuf;


typedef struct
{
    ae_vector treebuf;
    ae_int_t treeidx;
} dftreebuf;


/*************************************************************************
Buffer object which is used to perform  various  requests  (usually  model
inference) in the multithreaded mode (multiple threads working  with  same
DF object).

This object should be created with DFCreateBuffer().
*************************************************************************/
typedef struct
{
    ae_vector x;
    ae_vector y;
} decisionforestbuffer;


/*************************************************************************
Decision forest (random forest) model.
*************************************************************************/
typedef struct
{
    ae_int_t forestformat;
    ae_bool usemantissa8;
    ae_int_t nvars;
    ae_int_t nclasses;
    ae_int_t ntrees;
    ae_int_t bufsize;
    ae_vector trees;
    decisionforestbuffer buffer;
    ae_vector trees8;
} decisionforest;


/*************************************************************************
Decision forest training report.

=== training/oob errors ==================================================

Following fields store training set errors:
* relclserror           -   fraction of misclassified cases, [0,1]
* avgce                 -   average cross-entropy in bits per symbol
* rmserror              -   root-mean-square error
* avgerror              -   average error
* avgrelerror           -   average relative error

Out-of-bag estimates are stored in fields with same names, but "oob" prefix.

For classification problems:
* RMS, AVG and AVGREL errors are calculated for posterior probabilities

For regression problems:
* RELCLS and AVGCE errors are zero

=== variable importance ==================================================

Following fields are used to store variable importance information:

* topvars               -   variables ordered from the most  important  to
                            less  important  ones  (according  to  current
                            choice of importance raiting).
                            For example, topvars[0] contains index of  the
                            most important variable, and topvars[0:2]  are
                            indexes of 3 most important ones and so on.
                            
* varimportances        -   array[nvars], ratings (the  larger,  the  more
                            important the variable  is,  always  in  [0,1]
                            range).
                            By default, filled  by  zeros  (no  importance
                            ratings are  provided  unless  you  explicitly
                            request them).
                            Zero rating means that variable is not important,
                            however you will rarely encounter such a thing,
                            in many cases  unimportant  variables  produce
                            nearly-zero (but nonzero) ratings.

Variable importance report must be EXPLICITLY requested by calling:
* dfbuildersetimportancegini() function, if you need out-of-bag Gini-based
  importance rating also known as MDI  (fast to  calculate,  resistant  to
  overfitting  issues,   but   has   some   bias  towards  continuous  and
  high-cardinality categorical variables)
* dfbuildersetimportancetrngini() function, if you need training set Gini-
  -based importance rating (what other packages typically report).
* dfbuildersetimportancepermutation() function, if you  need  permutation-
  based importance rating also known as MDA (slower to calculate, but less
  biased)
* dfbuildersetimportancenone() function,  if  you  do  not  need  importance
  ratings - ratings will be zero, topvars[] will be [0,1,2,...]

Different importance ratings (Gini or permutation) produce  non-comparable
values. Although in all cases rating values lie in [0,1] range, there  are
exist differences:
* informally speaking, Gini importance rating tends to divide "unit amount
  of importance"  between  several  important  variables, i.e. it produces
  estimates which roughly sum to 1.0 (or less than 1.0, if your  task  can
  not be solved exactly). If all variables  are  equally  important,  they
  will have same rating,  roughly  1/NVars,  even  if  every  variable  is
  critically important.
* from the other side, permutation importance tells us what percentage  of
  the model predictive power will be ruined  by  permuting  this  specific
  variable. It does not produce estimates which  sum  to  one.  Critically
  important variable will have rating close  to  1.0,  and  you  may  have
  multiple variables with such a rating.

More information on variable importance ratings can be found  in  comments
on the dfbuildersetimportancegini() and dfbuildersetimportancepermutation()
functions.
*************************************************************************/
typedef struct
{
    double relclserror;
    double avgce;
    double rmserror;
    double avgerror;
    double avgrelerror;
    double oobrelclserror;
    double oobavgce;
    double oobrmserror;
    double oobavgerror;
    double oobavgrelerror;
    ae_vector topvars;
    ae_vector varimportances;
} dfreport;


typedef struct
{
    ae_vector treebuf;
    ae_vector idxbuf;
    ae_vector tmpbufr;
    ae_vector tmpbufr2;
    ae_vector tmpbufi;
    ae_vector classibuf;
    ae_vector sortrbuf;
    ae_vector sortrbuf2;
    ae_vector sortibuf;
    ae_vector varpool;
    ae_vector evsbin;
    ae_vector evssplits;
} dfinternalbuffers;


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
     ae_state *_state);


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
void dfbuildercreate(decisionforestbuilder* s, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
void dfbuildersetrndvarsauto(decisionforestbuilder* s, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


/*************************************************************************
This function is an alias for dfbuilderpeekprogress(), left in ALGLIB  for
backward compatibility reasons.

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
double dfbuildergetprogress(const decisionforestbuilder* s,
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
double dfbinarycompression(decisionforest* df, ae_state *_state);


/*************************************************************************
This is a 8-bit version of dfbinarycompression.
Not recommended for external use because it is too lossy.

  -- ALGLIB --
     Copyright 22.07.2019 by Bochkanov Sergey
*************************************************************************/
double dfbinarycompression8(decisionforest* df, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


/*************************************************************************
Serializer: allocation

  -- ALGLIB --
     Copyright 14.03.2011 by Bochkanov Sergey
*************************************************************************/
void dfalloc(ae_serializer* s,
     const decisionforest* forest,
     ae_state *_state);


/*************************************************************************
Serializer: serialization

  -- ALGLIB --
     Copyright 14.03.2011 by Bochkanov Sergey
*************************************************************************/
void dfserialize(ae_serializer* s,
     const decisionforest* forest,
     ae_state *_state);


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 14.03.2011 by Bochkanov Sergey
*************************************************************************/
void dfunserialize(ae_serializer* s,
     decisionforest* forest,
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);
void _decisionforestbuilder_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _decisionforestbuilder_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _decisionforestbuilder_clear(void* _p);
void _decisionforestbuilder_destroy(void* _p);
void _dfworkbuf_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dfworkbuf_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dfworkbuf_clear(void* _p);
void _dfworkbuf_destroy(void* _p);
void _dfvotebuf_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dfvotebuf_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dfvotebuf_clear(void* _p);
void _dfvotebuf_destroy(void* _p);
void _dfpermimpbuf_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dfpermimpbuf_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dfpermimpbuf_clear(void* _p);
void _dfpermimpbuf_destroy(void* _p);
void _dftreebuf_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dftreebuf_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dftreebuf_clear(void* _p);
void _dftreebuf_destroy(void* _p);
void _decisionforestbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _decisionforestbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _decisionforestbuffer_clear(void* _p);
void _decisionforestbuffer_destroy(void* _p);
void _decisionforest_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _decisionforest_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _decisionforest_clear(void* _p);
void _decisionforest_destroy(void* _p);
void _dfreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dfreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dfreport_clear(void* _p);
void _dfreport_destroy(void* _p);
void _dfinternalbuffers_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dfinternalbuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dfinternalbuffers_clear(void* _p);
void _dfinternalbuffers_destroy(void* _p);


/*$ End $*/
#endif

