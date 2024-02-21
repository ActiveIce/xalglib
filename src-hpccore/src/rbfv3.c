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
#include <stdio.h>
#include "rbfv3.h"


/*$ Declarations $*/
static double rbfv3_epsred = 0.999999;
static ae_int_t rbfv3_maxddmits = 25;
static double rbfv3_polyharmonic2scale = 4.0;
static ae_int_t rbfv3_acbfparallelthreshold = 512;
static ae_int_t rbfv3_ddmparallelthreshold = 512;
static ae_int_t rbfv3_bfparallelthreshold = 512;
static ae_int_t rbfv3_defaultmaxpanelsize = 128;
static ae_int_t rbfv3_maxcomputebatchsize = 128;
static ae_int_t rbfv3_minfarfieldsize = 256;
static ae_int_t rbfv3_biharmonicseriesmax = 15;
static ae_int_t rbfv3_farfieldnone = -1;
static ae_int_t rbfv3_farfieldbiharmonic = 1;
static double rbfv3_defaultfastevaltol = 1.0E-3;
static ae_bool rbfv3_userelaxederrorestimates = ae_true;
static void rbfv3_evalbufferinit(rbf3evaluatorbuffer* buf,
     ae_int_t nx,
     ae_int_t maxpanelsize,
     ae_state *_state);
static ae_int_t rbfv3_fastevaluatorinitrec(rbf3fastevaluator* eval,
     /* Real    */ const ae_matrix* xx,
     /* Integer */ ae_vector* ptidx,
     /* Real    */ ae_vector* coordbuf,
     ae_int_t idx0,
     ae_int_t idx1,
     nrpool* nxpool,
     ae_state *_state);
static void rbfv3_fastevaluatorinit(rbf3fastevaluator* eval,
     /* Real    */ const ae_matrix* _x,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t maxpanelsize,
     ae_int_t bftype,
     double bfparam,
     ae_bool usedebugcounters,
     ae_state *_state);
static void rbfv3_fastevaluatorloadcoeffsrec(rbf3fastevaluator* eval,
     ae_int_t treenodeidx,
     ae_state *_state);
static void rbfv3_fastevaluatorloadcoeffs1(rbf3fastevaluator* eval,
     /* Real    */ const ae_vector* w,
     ae_state *_state);
static void rbfv3_fastevaluatorloadcoeffs(rbf3fastevaluator* eval,
     /* Real    */ const ae_matrix* w,
     ae_state *_state);
static void rbfv3_fastevaluatorpushtolrec(rbf3fastevaluator* eval,
     ae_int_t treenodeidx,
     ae_bool dotrace,
     ae_int_t dbglevel,
     double maxcomputeerr,
     ae_state *_state);
static void rbfv3_fastevaluatorpushtol(rbf3fastevaluator* eval,
     double maxcomputeerr,
     ae_state *_state);
static void rbfv3_fastevaluatorcomputepanel2panel(rbf3fastevaluator* eval,
     rbf3panel* dstpanel,
     rbf3panel* srcpanel,
     rbf3evaluatorbuffer* buf,
     /* Real    */ ae_vector* y,
     ae_state *_state);
static void rbfv3_fastevaluatorcomputeallrecurseonsources(rbf3fastevaluator* eval,
     rbf3panel* dstpanel,
     rbf3evaluatorbuffer* buf,
     ae_int_t sourcetreenode,
     /* Real    */ ae_vector* y,
     ae_state *_state);
static void rbfv3_fastevaluatorcomputeallrecurseontargets(rbf3fastevaluator* eval,
     ae_int_t targettreenode,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void _spawn_rbfv3_fastevaluatorcomputeallrecurseontargets(rbf3fastevaluator* eval,
    ae_int_t targettreenode,
    /* Real    */ ae_vector* y, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_rbfv3_fastevaluatorcomputeallrecurseontargets(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_rbfv3_fastevaluatorcomputeallrecurseontargets(rbf3fastevaluator* eval,
    ae_int_t targettreenode,
    /* Real    */ ae_vector* y, ae_state *_state);
static void rbfv3_fastevaluatorcomputeall(rbf3fastevaluator* eval,
     /* Real    */ ae_vector* y,
     ae_state *_state);
static void rbfv3_fastevaluatorcomputebatchrecurseonsources(rbf3fastevaluator* eval,
     /* Real    */ const ae_matrix* x,
     ae_int_t tgtidx,
     ae_int_t sourcetreenode,
     ae_bool usefarfields,
     rbf3evaluatorbuffer* buf,
     /* Real    */ ae_matrix* y,
     ae_state *_state);
void _spawn_rbfv3_fastevaluatorcomputebatchrecurseonsources(rbf3fastevaluator* eval,
    /* Real    */ const ae_matrix* x,
    ae_int_t tgtidx,
    ae_int_t sourcetreenode,
    ae_bool usefarfields,
    rbf3evaluatorbuffer* buf,
    /* Real    */ ae_matrix* y, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_rbfv3_fastevaluatorcomputebatchrecurseonsources(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_rbfv3_fastevaluatorcomputebatchrecurseonsources(rbf3fastevaluator* eval,
    /* Real    */ const ae_matrix* x,
    ae_int_t tgtidx,
    ae_int_t sourcetreenode,
    ae_bool usefarfields,
    rbf3evaluatorbuffer* buf,
    /* Real    */ ae_matrix* y, ae_state *_state);
static void rbfv3_fastevaluatorcomputebatchrecurseontargets(rbf3fastevaluator* eval,
     /* Real    */ const ae_matrix* x,
     ae_int_t idx0,
     ae_int_t idx1,
     ae_bool isrootcall,
     ae_bool usefarfields,
     /* Real    */ ae_matrix* y,
     ae_state *_state);
void _spawn_rbfv3_fastevaluatorcomputebatchrecurseontargets(rbf3fastevaluator* eval,
    /* Real    */ const ae_matrix* x,
    ae_int_t idx0,
    ae_int_t idx1,
    ae_bool isrootcall,
    ae_bool usefarfields,
    /* Real    */ ae_matrix* y, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_rbfv3_fastevaluatorcomputebatchrecurseontargets(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_rbfv3_fastevaluatorcomputebatchrecurseontargets(rbf3fastevaluator* eval,
    /* Real    */ const ae_matrix* x,
    ae_int_t idx0,
    ae_int_t idx1,
    ae_bool isrootcall,
    ae_bool usefarfields,
    /* Real    */ ae_matrix* y, ae_state *_state);
static void rbfv3_fastevaluatorcomputebatch(rbf3fastevaluator* eval,
     /* Real    */ const ae_matrix* x,
     ae_int_t n,
     ae_bool usefarfields,
     /* Real    */ ae_matrix* y,
     ae_state *_state);
static void rbfv3_createfastevaluator(rbfv3model* model, ae_state *_state);
static void rbfv3_gridcalcrec(const rbfv3model* s,
     ae_int_t simdwidth,
     ae_int_t tileidx0,
     ae_int_t tileidx1,
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
     ae_shared_pool* calcpool,
     ae_bool isrootcall,
     ae_state *_state);
void _spawn_rbfv3_gridcalcrec(const rbfv3model* s,
    ae_int_t simdwidth,
    ae_int_t tileidx0,
    ae_int_t tileidx1,
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
    ae_shared_pool* calcpool,
    ae_bool isrootcall, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_rbfv3_gridcalcrec(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_rbfv3_gridcalcrec(const rbfv3model* s,
    ae_int_t simdwidth,
    ae_int_t tileidx0,
    ae_int_t tileidx1,
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
    ae_shared_pool* calcpool,
    ae_bool isrootcall, ae_state *_state);
static void rbfv3_zerofill(rbfv3model* s,
     ae_int_t nx,
     ae_int_t ny,
     ae_state *_state);
static void rbfv3_allocatecalcbuffer(const rbfv3model* s,
     rbfv3calcbuffer* buf,
     ae_state *_state);
static void rbfv3_preprocessdatasetrec(/* Real    */ ae_matrix* xbuf,
     /* Real    */ ae_matrix* ybuf,
     /* Integer */ ae_vector* initidx,
     ae_int_t wrk0,
     ae_int_t wrk1,
     ae_int_t nx,
     ae_int_t ny,
     double mergetol,
     /* Real    */ ae_vector* tmpboxmin,
     /* Real    */ ae_vector* tmpboxmax,
     /* Real    */ ae_matrix* xout,
     /* Real    */ ae_matrix* yout,
     /* Integer */ ae_vector* raw2wrkmap,
     /* Integer */ ae_vector* wrk2rawmap,
     ae_int_t* nout,
     ae_state *_state);
static void rbfv3_preprocessdataset(/* Real    */ const ae_matrix* _xraw,
     double mergetol,
     /* Real    */ const ae_matrix* _yraw,
     /* Real    */ const ae_vector* _xscaleraw,
     ae_int_t nraw,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t bftype,
     double bfparamraw,
     double lambdavraw,
     /* Real    */ ae_matrix* xwrk,
     /* Real    */ ae_matrix* ywrk,
     /* Integer */ ae_vector* raw2wrkmap,
     /* Integer */ ae_vector* wrk2rawmap,
     ae_int_t* nwrk,
     /* Real    */ ae_vector* xscalewrk,
     /* Real    */ ae_vector* xshift,
     double* bfparamwrk,
     double* lambdavwrk,
     double* addxrescaleaplied,
     ae_state *_state);
static void rbfv3_selectglobalnodes(/* Real    */ const ae_matrix* xx,
     ae_int_t n,
     ae_int_t nx,
     /* Integer */ const ae_vector* existingnodes,
     ae_int_t nexisting,
     ae_int_t nspec,
     /* Integer */ ae_vector* nodes,
     ae_int_t* nchosen,
     double* maxdist,
     ae_state *_state);
static void rbfv3_buildsimplifiedkdtree(/* Real    */ const ae_matrix* xx,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t reducefactor,
     ae_int_t minsize,
     kdtree* kdt,
     ae_state *_state);
static void rbfv3_computetargetscatterdesignmatrices(/* Real    */ const ae_matrix* xx,
     ae_int_t ntotal,
     ae_int_t nx,
     ae_int_t functype,
     double funcparam,
     /* Integer */ const ae_vector* workingnodes,
     ae_int_t nwrk,
     /* Integer */ const ae_vector* scatternodes,
     ae_int_t nscatter,
     /* Real    */ ae_matrix* atwrk,
     /* Real    */ ae_matrix* atsctr,
     ae_state *_state);
static void rbfv3_computeacbfpreconditionerbasecase(acbfbuilder* builder,
     acbfbuffer* buf,
     ae_int_t wrk0,
     ae_int_t wrk1,
     ae_state *_state);
static void rbfv3_computeacbfpreconditionerrecv2(acbfbuilder* builder,
     ae_int_t wrk0,
     ae_int_t wrk1,
     ae_state *_state);
void _spawn_rbfv3_computeacbfpreconditionerrecv2(acbfbuilder* builder,
    ae_int_t wrk0,
    ae_int_t wrk1, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_rbfv3_computeacbfpreconditionerrecv2(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_rbfv3_computeacbfpreconditionerrecv2(acbfbuilder* builder,
    ae_int_t wrk0,
    ae_int_t wrk1, ae_state *_state);
static void rbfv3_computeacbfpreconditioner(/* Real    */ const ae_matrix* xx,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t functype,
     double funcparam,
     ae_int_t aterm,
     ae_int_t batchsize,
     ae_int_t nglobal,
     ae_int_t nlocal,
     ae_int_t ncorrection,
     ae_int_t correctorgrowth,
     ae_int_t simplificationfactor,
     double lambdav,
     sparsematrix* sp,
     ae_state *_state);
static void rbfv3_ddmsolverinitbasecase(rbf3ddmsolver* solver,
     /* Real    */ const ae_matrix* x,
     ae_int_t n,
     ae_int_t nx,
     const rbf3evaluator* bfmatrix,
     double lambdav,
     const sparsematrix* sp,
     rbf3ddmbuffer* buf,
     /* Integer */ ae_vector* tgtidx,
     ae_int_t tgt0,
     ae_int_t tgt1,
     ae_int_t nneighbors,
     ae_bool dodetailedtrace,
     ae_state *_state);
static void rbfv3_ddmsolverinitrec(rbf3ddmsolver* solver,
     /* Real    */ const ae_matrix* x,
     ae_int_t n,
     ae_int_t nx,
     const rbf3evaluator* bfmatrix,
     double lambdav,
     const sparsematrix* sp,
     /* Integer */ ae_vector* wrkidx,
     ae_int_t wrk0,
     ae_int_t wrk1,
     ae_int_t nneighbors,
     ae_int_t nbatch,
     ae_bool dodetailedtrace,
     ae_state *_state);
void _spawn_rbfv3_ddmsolverinitrec(rbf3ddmsolver* solver,
    /* Real    */ const ae_matrix* x,
    ae_int_t n,
    ae_int_t nx,
    const rbf3evaluator* bfmatrix,
    double lambdav,
    const sparsematrix* sp,
    /* Integer */ ae_vector* wrkidx,
    ae_int_t wrk0,
    ae_int_t wrk1,
    ae_int_t nneighbors,
    ae_int_t nbatch,
    ae_bool dodetailedtrace, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_rbfv3_ddmsolverinitrec(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_rbfv3_ddmsolverinitrec(rbf3ddmsolver* solver,
    /* Real    */ const ae_matrix* x,
    ae_int_t n,
    ae_int_t nx,
    const rbf3evaluator* bfmatrix,
    double lambdav,
    const sparsematrix* sp,
    /* Integer */ ae_vector* wrkidx,
    ae_int_t wrk0,
    ae_int_t wrk1,
    ae_int_t nneighbors,
    ae_int_t nbatch,
    ae_bool dodetailedtrace, ae_state *_state);
static void rbfv3_ddmsolverinit(/* Real    */ const ae_matrix* x,
     double rescaledby,
     ae_int_t n,
     ae_int_t nx,
     const rbf3evaluator* bfmatrix,
     ae_int_t bftype,
     double bfparam,
     double lambdav,
     ae_int_t aterm,
     const sparsematrix* sp,
     ae_int_t nneighbors,
     ae_int_t nbatch,
     ae_int_t ncorrector,
     ae_bool dotrace,
     ae_bool dodetailedtrace,
     rbf3ddmsolver* solver,
     ae_int_t* timeddminit,
     ae_int_t* timecorrinit,
     ae_state *_state);
static void rbfv3_ddmsolverrunrec(rbf3ddmsolver* solver,
     /* Real    */ const ae_matrix* res,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t ny,
     /* Real    */ ae_matrix* c,
     ae_int_t cnt,
     ae_state *_state);
static void rbfv3_ddmsolverrun(rbf3ddmsolver* solver,
     /* Real    */ const ae_matrix* res,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t ny,
     const sparsematrix* sp,
     rbf3evaluator* bfmatrix,
     rbf3fastevaluator* fasteval,
     double fastevaltol,
     /* Real    */ ae_matrix* upd,
     ae_int_t* timeddmsolve,
     ae_int_t* timecorrsolve,
     ae_state *_state);
static void rbfv3_ddmsolverrun1(rbf3ddmsolver* solver,
     /* Real    */ const ae_vector* res,
     ae_int_t n,
     ae_int_t nx,
     const sparsematrix* sp,
     rbf3evaluator* bfmatrix,
     rbf3fastevaluator* fasteval,
     double fastevaltol,
     /* Real    */ ae_vector* upd,
     ae_int_t* timeddmsolve,
     ae_int_t* timecorrsolve,
     ae_state *_state);
static double rbfv3_autodetectscaleparameter(/* Real    */ const ae_matrix* xx,
     ae_int_t n,
     ae_int_t nx,
     ae_state *_state);
static void rbfv3_computebfmatrixrec(/* Real    */ const ae_matrix* xx,
     ae_int_t range0,
     ae_int_t range1,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t functype,
     double funcparam,
     /* Real    */ ae_matrix* f,
     ae_state *_state);
void _spawn_rbfv3_computebfmatrixrec(/* Real    */ const ae_matrix* xx,
    ae_int_t range0,
    ae_int_t range1,
    ae_int_t n,
    ae_int_t nx,
    ae_int_t functype,
    double funcparam,
    /* Real    */ ae_matrix* f, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_rbfv3_computebfmatrixrec(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_rbfv3_computebfmatrixrec(/* Real    */ const ae_matrix* xx,
    ae_int_t range0,
    ae_int_t range1,
    ae_int_t n,
    ae_int_t nx,
    ae_int_t functype,
    double funcparam,
    /* Real    */ ae_matrix* f, ae_state *_state);
static void rbfv3_computebfmatrix(/* Real    */ const ae_matrix* xx,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t functype,
     double funcparam,
     /* Real    */ ae_matrix* f,
     ae_state *_state);
static void rbfv3_modelmatrixinit(/* Real    */ const ae_matrix* xx,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t functype,
     double funcparam,
     ae_int_t storagetype,
     rbf3evaluator* modelmatrix,
     ae_state *_state);
static void rbfv3_modelmatrixcomputepartial(const rbf3evaluator* modelmatrix,
     /* Integer */ const ae_vector* ridx,
     ae_int_t m0,
     /* Integer */ const ae_vector* cidx,
     ae_int_t m1,
     /* Real    */ ae_matrix* r,
     ae_state *_state);
static void rbfv3_computerowchunk(const rbf3evaluator* evaluator,
     /* Real    */ const ae_vector* x,
     rbf3evaluatorbuffer* buf,
     ae_int_t chunksize,
     ae_int_t chunkidx,
     double distance0,
     ae_int_t needgradinfo,
     ae_state *_state);
static ae_bool rbfv3_iscpdfunction(ae_int_t functype,
     ae_int_t aterm,
     ae_state *_state);


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
     ae_state *_state)
{

    _rbfv3model_clear(s);

    ae_assert(nx>=1, "RBFCreate: NX<1", _state);
    ae_assert(ny>=1, "RBFCreate: NY<1", _state);
    ae_assert(bf==1||bf==2, "RBFCreate: unsupported basis function type", _state);
    ae_assert(ae_isfinite(bfp, _state)&&ae_fp_greater_eq(bfp,(double)(0)), "RBFCreate: infinite or negative basis function parameter", _state);
    
    /*
     * Serializable parameters
     */
    s->nx = nx;
    s->ny = ny;
    s->bftype = bf;
    s->bfparam = bfp;
    s->nc = 0;
    rsetallocv(nx, 1.0, &s->s, _state);
    rsetallocm(ny, nx+1, 0.0, &s->v, _state);
    rbfv3_allocatecalcbuffer(s, &s->calcbuf, _state);
    
    /*
     * Debug counters
     */
    s->dbgregqrusedforddm = ae_false;
    s->dbgworstfirstdecay = 0.0;
}


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
     ae_state *_state)
{

    _rbfv3calcbuffer_clear(buf);

    rbfv3_allocatecalcbuffer(s, buf, _state);
}


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
     ae_state *_state)
{
    ae_frame _frame_block;
    double fastevaltol;
    ae_int_t n;
    ae_int_t nx;
    ae_int_t ny;
    double bfparamscaled;
    double lambdavwrk;
    double rescaledby;
    double mergetol;
    ae_int_t matrixformat;
    ae_int_t acbfbatch;
    ae_int_t acbfglobal;
    ae_int_t acbflocal;
    ae_int_t acbfcorrection;
    ae_int_t ddmbatch;
    ae_int_t ddmneighbors;
    ae_int_t ddmcoarse;
    ae_int_t maxpanelsize;
    ae_matrix xscaled;
    ae_matrix yscaled;
    ae_matrix xcoarse;
    ae_matrix x1t;
    rbf3evaluator bfmatrix;
    rbf3fastevaluator fasteval;
    ae_vector b;
    ae_vector x0;
    ae_vector x1;
    ae_vector y0;
    ae_vector y1;
    ae_vector sft;
    ae_vector scalewrk;
    ae_matrix c2;
    ae_matrix res;
    ae_matrix upd0;
    ae_matrix upd1;
    ae_matrix ortbasis;
    ae_int_t ortbasissize;
    ae_vector raw2wrkmap;
    ae_vector wrk2rawmap;
    ae_vector idummy;
    sparsematrix sp;
    sparsesolverstate ss;
    sparsesolverreport ssrep;
    rbf3ddmsolver ddmsolver;
    double resnrm;
    double res0nrm;
    ae_int_t iteridx;
    ae_int_t yidx;
    ae_bool dotrace;
    ae_bool dodetailedtrace;
    fblsgmresstate gmressolver;
    double orterr;
    double l1nrm;
    double linfnrm;
    ae_int_t timeprec;
    ae_int_t timedesign;
    ae_int_t timeddminit;
    ae_int_t timeddmsolve;
    ae_int_t timecorrinit;
    ae_int_t timecorrsolve;
    ae_int_t timereeval;
    ae_int_t timetotal;
    savgcounter dbgfarfieldspeedup;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double v;
    double vv;
    ae_matrix refrhs;
    ae_vector refrhs1;
    ae_vector refsol1;
    double debugdamping;

    ae_frame_make(_state, &_frame_block);
    memset(&xscaled, 0, sizeof(xscaled));
    memset(&yscaled, 0, sizeof(yscaled));
    memset(&xcoarse, 0, sizeof(xcoarse));
    memset(&x1t, 0, sizeof(x1t));
    memset(&bfmatrix, 0, sizeof(bfmatrix));
    memset(&fasteval, 0, sizeof(fasteval));
    memset(&b, 0, sizeof(b));
    memset(&x0, 0, sizeof(x0));
    memset(&x1, 0, sizeof(x1));
    memset(&y0, 0, sizeof(y0));
    memset(&y1, 0, sizeof(y1));
    memset(&sft, 0, sizeof(sft));
    memset(&scalewrk, 0, sizeof(scalewrk));
    memset(&c2, 0, sizeof(c2));
    memset(&res, 0, sizeof(res));
    memset(&upd0, 0, sizeof(upd0));
    memset(&upd1, 0, sizeof(upd1));
    memset(&ortbasis, 0, sizeof(ortbasis));
    memset(&raw2wrkmap, 0, sizeof(raw2wrkmap));
    memset(&wrk2rawmap, 0, sizeof(wrk2rawmap));
    memset(&idummy, 0, sizeof(idummy));
    memset(&sp, 0, sizeof(sp));
    memset(&ss, 0, sizeof(ss));
    memset(&ssrep, 0, sizeof(ssrep));
    memset(&ddmsolver, 0, sizeof(ddmsolver));
    memset(&gmressolver, 0, sizeof(gmressolver));
    memset(&dbgfarfieldspeedup, 0, sizeof(dbgfarfieldspeedup));
    memset(&refrhs, 0, sizeof(refrhs));
    memset(&refrhs1, 0, sizeof(refrhs1));
    memset(&refsol1, 0, sizeof(refsol1));
    _rbfv3report_clear(rep);
    ae_matrix_init(&xscaled, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&yscaled, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xcoarse, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&x1t, 0, 0, DT_REAL, _state, ae_true);
    _rbf3evaluator_init(&bfmatrix, _state, ae_true);
    _rbf3fastevaluator_init(&fasteval, _state, ae_true);
    ae_vector_init(&b, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&x0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&x1, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&y0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&y1, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&sft, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&scalewrk, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&c2, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&res, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&upd0, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&upd1, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&ortbasis, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&raw2wrkmap, 0, DT_INT, _state, ae_true);
    ae_vector_init(&wrk2rawmap, 0, DT_INT, _state, ae_true);
    ae_vector_init(&idummy, 0, DT_INT, _state, ae_true);
    _sparsematrix_init(&sp, _state, ae_true);
    _sparsesolverstate_init(&ss, _state, ae_true);
    _sparsesolverreport_init(&ssrep, _state, ae_true);
    _rbf3ddmsolver_init(&ddmsolver, _state, ae_true);
    _fblsgmresstate_init(&gmressolver, _state, ae_true);
    _savgcounter_init(&dbgfarfieldspeedup, _state, ae_true);
    ae_matrix_init(&refrhs, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&refrhs1, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&refsol1, 0, DT_REAL, _state, ae_true);

    mergetol = (double)1000*ae_machineepsilon;
    ae_assert(ae_isfinite(tol, _state), "RBFV3Build: incorrect TOL", _state);
    ae_assert(s->nx>0, "RBFV3Build: incorrect NX", _state);
    ae_assert(s->ny>0, "RBFV3Build: incorrect NY", _state);
    ae_assert((bftype==1||bftype==2)||bftype==3, "RBFV3Build: incorrect BFType", _state);
    ae_assert((aterm==1||aterm==2)||aterm==3, "RBFV3Build: incorrect ATerm", _state);
    ae_assert((rbfprofile==-2||rbfprofile==-1)||rbfprofile==0, "RBFV3Build: incorrect RBFProfile", _state);
    for(j=0; j<=s->nx-1; j++)
    {
        ae_assert(ae_fp_greater(scaleraw->ptr.p_double[j],(double)(0)), "RBFV2BuildHierarchical: incorrect ScaleVec", _state);
    }
    nx = s->nx;
    ny = s->ny;
    bfparamscaled = bfparamraw;
    
    /*
     * Trace output (if needed)
     */
    dotrace = ae_is_trace_enabled("RBF");
    dodetailedtrace = dotrace&&ae_is_trace_enabled("RBF.DETAILED");
    if( dotrace )
    {
        ae_trace("\n\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        ae_trace("// DDM-RBF builder started                                                                        //\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
    }
    
    /*
     * Clean up communication and report fields
     */
    *progress10000 = 0;
    rep->maxerror = (double)(0);
    rep->rmserror = (double)(0);
    rep->iterationscount = 0;
    timeprec = 0;
    timedesign = 0;
    timeddminit = 0;
    timeddmsolve = 0;
    timecorrinit = 0;
    timecorrsolve = 0;
    timereeval = 0;
    timetotal = 0-ae_tickcount();
    s->dbgregqrusedforddm = ae_false;
    s->dbgworstfirstdecay = 0.0;
    savgcounterinit(&dbgfarfieldspeedup, 0.0, _state);
    
    /*
     * Quick exit when we have no points
     */
    if( nraw==0 )
    {
        rbfv3_zerofill(s, nx, ny, _state);
        rep->terminationtype = 1;
        *progress10000 = 10000;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Preprocess dataset (scale points, merge nondistinct ones)
     */
    rbfv3_preprocessdataset(xraw, mergetol, yraw, scaleraw, nraw, nx, ny, bftype, bfparamraw, lambdavraw, &xscaled, &yscaled, &raw2wrkmap, &wrk2rawmap, &n, &scalewrk, &sft, &bfparamscaled, &lambdavwrk, &rescaledby, _state);
    rallocm(nx+1, n, &x1t, _state);
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            x1t.ptr.pp_double[j][i] = xscaled.ptr.pp_double[i][j];
        }
        x1t.ptr.pp_double[nx][i] = 1.0;
    }
    
    /*
     * Set algorithm parameters according to the current profile
     */
    ae_assert((rbfprofile==-2||rbfprofile==-1)||rbfprofile==0, "RBFV3Build: incorrect RBFProfile", _state);
    if( dotrace )
    {
        ae_trace("=== PRINTING ALGORITHM SETTINGS ====================================================================\n");
        ae_trace("TOL         = %0.2e\nPROFILE     = %0d\n",
            (double)(tol),
            (int)(rbfprofile));
    }
    tol = ae_maxreal(tol, 1.0E-6, _state);
    acbfglobal = 0;
    acbflocal = ae_maxint(ae_round(ae_pow(5.0, (double)(nx), _state), _state), 25, _state);
    acbfcorrection = ae_round(ae_pow((double)(5), (double)(nx), _state), _state);
    acbfbatch = 32;
    ddmneighbors = 0;
    ddmbatch = imin2(1000, n, _state);
    ddmcoarse = imin3(ae_round(0.1*(double)n+(double)10, _state), 2048, n, _state);
    maxpanelsize = rbfv3_defaultmaxpanelsize;
    debugdamping = 0.0;
    if( rbfprofile==-1||rbfprofile==-2 )
    {
        
        /*
         * Decrease batch sizes and corrector efficiency.
         * Add debug damping which produces suboptimal ACBF basis.
         */
        if( dotrace )
        {
            ae_trace("> debug profile activated\n");
        }
        acbfbatch = 16;
        ddmneighbors = 3;
        ddmbatch = 16;
        ddmcoarse = imin3(ae_round(0.05*(double)n+(double)2, _state), 512, n, _state);
        maxpanelsize = 16;
        if( rbfprofile==-2 )
        {
            debugdamping = 0.000001;
        }
    }
    
    /*
     * Prepare fast evaluator
     *
     * NOTE: we set fast evaluation tolerance to TOL. Actually, it is better to have it somewhat below
     *       TOL, e.g. TOL/10 or TOL/100. However, we rely on the fact that fast evaluator error estimate
     *       is inherently pessimistic, i.e. actual evaluation accuracy is better than that.
     */
    fastevaltol = tol;
    rbfv3_fastevaluatorinit(&fasteval, &xscaled, n, nx, 1, maxpanelsize, bftype, bfparamscaled, dotrace, _state);
    
    /*
     * Compute design matrix
     */
    matrixformat = 1;
    if( dotrace )
    {
        ae_trace("=== MODEL MATRIX INITIALIZATION STARTED ============================================================\n");
        ae_trace("N           = %0d\nNX          = %0d\nNY          = %0d\n",
            (int)(n),
            (int)(nx),
            (int)(ny));
        ae_trace("BFType      = %0d",
            (int)(bftype));
        if( bftype==1&&ae_fp_greater(bfparamraw,(double)(0)) )
        {
            ae_trace("  ( f=sqrt(r^2+alpha^2), alpha=%0.3f, multiquadric with manual radius)",
                (double)(bfparamraw));
        }
        if( bftype==1&&ae_fp_eq(bfparamraw,(double)(0)) )
        {
            ae_trace("  ( f=r, biharmonic spline )");
        }
        if( bftype==1&&ae_fp_less(bfparamraw,(double)(0)) )
        {
            ae_trace("  ( f=sqrt(r^2+alpha^2), alpha=AUTO*%0.3f=%0.2e, multiquadric )",
                (double)(-bfparamraw),
                (double)(bfparamscaled));
        }
        if( bftype==2 )
        {
            ae_trace("  ( f=log(r)*r^2, thin plate spline )");
        }
        if( bftype==3 )
        {
            ae_trace("  ( f=r^3 )");
        }
        ae_trace("\n");
        ae_trace("Polinom.term= %0d ",
            (int)(aterm));
        if( aterm==1 )
        {
            ae_trace("(linear term)");
        }
        if( aterm==2 )
        {
            ae_trace("(constant term)");
        }
        if( aterm==3 )
        {
            ae_trace("(zero term)");
        }
        ae_trace("\n");
        ae_trace("LambdaV     = %0.2e (raw value of the smoothing parameter; effective value after adjusting for data spread is %0.2e)\n",
            (double)(lambdavraw),
            (double)(lambdavwrk));
        ae_trace("VarScales   = ");
        tracevectore3(scaleraw, 0, nx, _state);
        ae_trace(" (raw values of variable scales)\n");
    }
    timedesign = timedesign-ae_tickcount();
    rbfv3_modelmatrixinit(&xscaled, n, nx, bftype, bfparamscaled, matrixformat, &bfmatrix, _state);
    timedesign = timedesign+ae_tickcount();
    if( dotrace )
    {
        ae_trace("> model matrix initialized in %0d ms\n",
            (int)(timedesign));
    }
    
    /*
     * Build orthogonal basis of the subspace spanned by polynomials of 1st degree.
     * This basis is used later to check orthogonality conditions for the coefficients.
     */
    rallocm(nx+1, n, &ortbasis, _state);
    rsetr(n, (double)1/ae_sqrt((double)(n), _state), &ortbasis, 0, _state);
    ortbasissize = 1;
    rallocv(n, &x0, _state);
    for(k=0; k<=nx-1; k++)
    {
        for(j=0; j<=n-1; j++)
        {
            x0.ptr.p_double[j] = xscaled.ptr.pp_double[j][k];
        }
        v = ae_sqrt(rdotv2(n, &x0, _state), _state);
        rowwisegramschmidt(&ortbasis, ortbasissize, n, &x0, &x0, ae_false, _state);
        vv = ae_sqrt(rdotv2(n, &x0, _state), _state);
        if( ae_fp_greater(vv,ae_sqrt(ae_machineepsilon, _state)*(v+(double)1)) )
        {
            rcopymulvr(n, (double)1/vv, &x0, &ortbasis, ortbasissize, _state);
            ortbasissize = ortbasissize+1;
        }
    }
    
    /*
     * Build preconditioner
     */
    if( dotrace )
    {
        ae_trace("=== PRECONDITIONER CONSTRUCTION STARTED ============================================================\n");
        ae_trace("nglobal     = %0d\nnlocal      = %0d\nncorrection = %0d\nnbatch      = %0d\n",
            (int)(acbfglobal),
            (int)(acbflocal),
            (int)(acbfcorrection),
            (int)(acbfbatch));
    }
    timeprec = timeprec-ae_tickcount();
    rbfv3_computeacbfpreconditioner(&xscaled, n, nx, bftype, bfparamscaled, aterm, acbfbatch, acbfglobal, acbflocal, acbfcorrection, 5, 2, lambdavwrk+debugdamping, &sp, _state);
    timeprec = timeprec+ae_tickcount();
    if( dotrace )
    {
        ae_trace("> ACBF preconditioner computed in %0d ms\n",
            (int)(timeprec));
    }
    
    /*
     * DDM
     */
    if( dotrace )
    {
        ae_trace("=== DOMAIN DECOMPOSITION METHOD STARTED ============================================================\n");
    }
    rsetallocm(n+nx+1, ny, 0.0, &c2, _state);
    if( dotrace )
    {
        ae_trace("> problem metrics and settings\n");
        ae_trace("NNeighbors  = %0d\n",
            (int)(ddmneighbors));
        ae_trace("NBatch      = %0d\n",
            (int)(ddmbatch));
        ae_trace("NCoarse     = %0d\n",
            (int)(ddmcoarse));
    }
    rbfv3_ddmsolverinit(&xscaled, rescaledby, n, nx, &bfmatrix, bftype, bfparamscaled, lambdavwrk, aterm, &sp, ddmneighbors, ddmbatch, ddmcoarse, dotrace, dodetailedtrace, &ddmsolver, &timeddminit, &timecorrinit, _state);
    if( dotrace )
    {
        ae_trace("> DDM initialization done in %0d ms, %0d subproblems solved (%0d well-conditioned, %0d ill-conditioned)\n",
            (int)(timeddminit),
            (int)(ddmsolver.subproblemscnt),
            (int)(ddmsolver.cntlu),
            (int)(ddmsolver.cntregqr));
    }
    
    /*
     * Use preconditioned GMRES
     */
    rep->rmserror = (double)(0);
    rep->maxerror = (double)(0);
    rep->iterationscount = 0;
    for(yidx=0; yidx<=ny-1; yidx++)
    {
        if( dotrace )
        {
            ae_trace("> solving for component %2d:\n",
                (int)(yidx));
        }
        rsetallocv(n+nx+1, 0.0, &y0, _state);
        rsetallocv(n+nx+1, 0.0, &y1, _state);
        rcopycv(n, &yscaled, yidx, &y0, _state);
        res0nrm = ae_sqrt(rdotv2(n, &y0, _state), _state);
        fblsgmrescreate(&y0, n, ae_minint(rbfv3_maxddmits, n, _state), &gmressolver, _state);
        gmressolver.epsres = tol;
        gmressolver.epsred = rbfv3_epsred;
        iteridx = 0;
        while(fblsgmresiteration(&gmressolver, _state))
        {
            if( dotrace )
            {
                ae_trace(">> DDM iteration %2d: %0.2e relative residual\n",
                    (int)(iteridx),
                    (double)(gmressolver.reprelres));
            }
            rallocv(n+nx+1, &y0, _state);
            rallocv(n+nx+1, &y1, _state);
            rbfv3_ddmsolverrun1(&ddmsolver, &gmressolver.x, n, nx, &sp, &bfmatrix, &fasteval, fastevaltol, &y0, &timeddmsolve, &timecorrsolve, _state);
            timereeval = timereeval-ae_tickcount();
            rbfv3_fastevaluatorloadcoeffs1(&fasteval, &y0, _state);
            rbfv3_fastevaluatorpushtol(&fasteval, fastevaltol*res0nrm, _state);
            rbfv3_fastevaluatorcomputeall(&fasteval, &y1, _state);
            savgcounterenqueue(&dbgfarfieldspeedup, ae_sqr((double)(fasteval.dbgpanelscnt), _state)/coalesce((double)(fasteval.dbgpanel2panelcnt+fasteval.dbgfield2panelcnt), (double)(1), _state), _state);
            rgemvx(n, nx+1, 1.0, &x1t, 0, 0, 1, &y0, n, 1.0, &y1, 0, _state);
            for(i=0; i<=n-1; i++)
            {
                y1.ptr.p_double[i] = y1.ptr.p_double[i]+lambdavwrk*y0.ptr.p_double[i];
            }
            timereeval = timereeval+ae_tickcount();
            rcopyv(n, &y1, &gmressolver.ax, _state);
            rep->iterationscount = rep->iterationscount+1;
            if( iteridx==1 )
            {
                s->dbgworstfirstdecay = ae_maxreal(gmressolver.reprelres, s->dbgworstfirstdecay, _state);
            }
            iteridx = iteridx+1;
        }
        rbfv3_ddmsolverrun1(&ddmsolver, &gmressolver.xs, n, nx, &sp, &bfmatrix, &fasteval, fastevaltol, &x1, &timeddmsolve, &timecorrsolve, _state);
        ae_assert(ae_isfinite(rdotv2(n+nx+1, &x1, _state), _state), "RBF3: integrity check 4359 failed", _state);
        rcopyvc(n+nx+1, &x1, &c2, yidx, _state);
        
        /*
         * Compute predictions and errors
         *
         * NOTE: because dataset preprocessing may reorder and merge points we have
         *       to use raw-to-work mapping in order to be able to compute correct
         *       error metrics.
         */
        timereeval = timereeval-ae_tickcount();
        rbfv3_fastevaluatorloadcoeffs1(&fasteval, &x1, _state);
        rbfv3_fastevaluatorpushtol(&fasteval, fastevaltol*res0nrm, _state);
        rbfv3_fastevaluatorcomputeall(&fasteval, &y1, _state);
        rgemvx(n, nx+1, 1.0, &x1t, 0, 0, 1, &x1, n, 1.0, &y1, 0, _state);
        timereeval = timereeval+ae_tickcount();
        resnrm = (double)(0);
        for(i=0; i<=n-1; i++)
        {
            resnrm = resnrm+ae_sqr(yscaled.ptr.pp_double[i][yidx]-y1.ptr.p_double[i]-lambdavwrk*x1.ptr.p_double[i], _state);
        }
        resnrm = ae_sqrt(resnrm, _state);
        for(i=0; i<=nraw-1; i++)
        {
            v = yraw->ptr.pp_double[i][yidx]-y1.ptr.p_double[raw2wrkmap.ptr.p_int[i]];
            rep->maxerror = ae_maxreal(rep->maxerror, ae_fabs(v, _state), _state);
            rep->rmserror = rep->rmserror+v*v;
        }
        if( dotrace )
        {
            ae_trace(">> done with %0.2e relative residual, GMRES completion code %0d\n",
                (double)(resnrm/coalesce(res0nrm, (double)(1), _state)),
                (int)(gmressolver.retcode));
        }
    }
    rep->rmserror = ae_sqrt(rep->rmserror/(double)(nraw*ny), _state);
    timetotal = timetotal+ae_tickcount();
    if( dotrace )
    {
        rallocv(n, &y0, _state);
        orterr = (double)(0);
        l1nrm = (double)(0);
        linfnrm = (double)(0);
        for(k=0; k<=ny-1; k++)
        {
            rcopycv(n, &c2, k, &y0, _state);
            linfnrm = ae_maxreal(linfnrm, rmaxabsv(n, &y0, _state), _state);
            for(i=0; i<=n-1; i++)
            {
                l1nrm = l1nrm+ae_fabs(y0.ptr.p_double[i], _state);
            }
            for(i=0; i<=ortbasissize-1; i++)
            {
                orterr = ae_maxreal(orterr, ae_fabs(rdotvr(n, &y0, &ortbasis, i, _state), _state), _state);
            }
        }
        ae_trace("=== PRINTING RBF SOLVER RESULTS ====================================================================\n");
        ae_trace("> errors\n");
        ae_trace("RMS.err     = %0.2e\n",
            (double)(rep->rmserror));
        ae_trace("MAX.err     = %0.2e\n",
            (double)(rep->maxerror));
        ae_trace("ORT.err     = %0.2e (orthogonality condition)\n",
            (double)(orterr));
        ae_trace("> solution statistics:\n");
        ae_trace("L1-norm     = %0.2e\n",
            (double)(l1nrm));
        ae_trace("Linf-norm   = %0.2e\n",
            (double)(linfnrm));
        ae_trace("> DDM iterations\n");
        ae_trace("ItsCnt      = %0d\n",
            (int)(rep->iterationscount));
        ae_trace("> speedup due to far field expansions (ok to be 1.0x for datasets below 100K):\n");
        ae_trace("reeval      = %0.1fx (speed-up of the model reevaluation phase)\n",
            (double)(savgcounterget(&dbgfarfieldspeedup, _state)));
        ae_trace("overall     = %0.1fx (overall speed-up)\n",
            (double)(((double)timetotal+(double)timereeval*(savgcounterget(&dbgfarfieldspeedup, _state)-(double)1))/((double)timetotal+ae_machineepsilon)));
        ae_trace("> total running time is %0d ms, including:\n",
            (int)(timetotal));
        ae_trace(">> model matrix generation               %8d ms\n",
            (int)(timedesign));
        ae_trace(">> ACBF preconditioner construction      %8d ms\n",
            (int)(timeprec));
        ae_trace(">> DDM solver initialization             %8d ms\n",
            (int)(timeddminit));
        ae_trace(">> DDM corrector initialization          %8d ms\n",
            (int)(timecorrinit));
        ae_trace(">> DDM solution phase                    %8d ms\n",
            (int)(timeddmsolve));
        ae_trace(">> DDM correction phase                  %8d ms\n",
            (int)(timecorrsolve));
        ae_trace(">> DDM solver model reevaluation         %8d ms\n",
            (int)(timereeval));
    }
    s->bftype = bftype;
    s->bfparam = bfparamscaled;
    rcopyallocv(nx, &scalewrk, &s->s, _state);
    for(j=0; j<=ny-1; j++)
    {
        s->v.ptr.pp_double[j][nx] = c2.ptr.pp_double[n+nx][j];
        for(i=0; i<=nx-1; i++)
        {
            s->v.ptr.pp_double[j][i] = c2.ptr.pp_double[n+i][j]/scalewrk.ptr.p_double[i];
            s->v.ptr.pp_double[j][nx] = s->v.ptr.pp_double[j][nx]-c2.ptr.pp_double[n+i][j]*sft.ptr.p_double[i]/scalewrk.ptr.p_double[i];
        }
    }
    rallocv(n*(nx+ny), &s->cw, _state);
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            s->cw.ptr.p_double[i*(nx+ny)+j] = xscaled.ptr.pp_double[i][j]+sft.ptr.p_double[j]/scalewrk.ptr.p_double[j];
        }
        for(j=0; j<=ny-1; j++)
        {
            s->cw.ptr.p_double[i*(nx+ny)+nx+j] = c2.ptr.pp_double[i][j];
        }
    }
    icopyallocv(n, &wrk2rawmap, &s->pointindexes, _state);
    s->nc = n;
    rbfv3_createfastevaluator(s, _state);
    
    /*
     * Set up debug fields
     */
    s->dbgregqrusedforddm = ddmsolver.cntregqr>0;
    
    /*
     * Update progress reports
     */
    rep->terminationtype = 1;
    *progress10000 = 10000;
    ae_frame_leave(_state);
}


/*************************************************************************
Serializer: allocation

  -- ALGLIB --
     Copyright 12.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfv3alloc(ae_serializer* s,
     const rbfv3model* model,
     ae_state *_state)
{


    
    /*
     * Data
     */
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    allocrealarray(s, &model->s, model->nx, _state);
    allocrealmatrix(s, &model->v, model->ny, model->nx+1, _state);
    allocrealarray(s, &model->cw, model->nc*(model->nx+model->ny), _state);
    allocintegerarray(s, &model->pointindexes, model->nc, _state);
    
    /*
     * End of stream, no additional data
     */
    ae_serializer_alloc_entry(s);
}


/*************************************************************************
Serializer: serialization

  -- ALGLIB --
     Copyright 12.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfv3serialize(ae_serializer* s,
     const rbfv3model* model,
     ae_state *_state)
{


    
    /*
     * Data
     */
    ae_serializer_serialize_int(s, model->nx, _state);
    ae_serializer_serialize_int(s, model->ny, _state);
    ae_serializer_serialize_int(s, model->bftype, _state);
    ae_serializer_serialize_double(s, model->bfparam, _state);
    ae_serializer_serialize_int(s, model->nc, _state);
    serializerealarray(s, &model->s, model->nx, _state);
    serializerealmatrix(s, &model->v, model->ny, model->nx+1, _state);
    serializerealarray(s, &model->cw, model->nc*(model->nx+model->ny), _state);
    serializeintegerarray(s, &model->pointindexes, model->nc, _state);
    
    /*
     * End of stream, no additional data
     */
    ae_serializer_serialize_int(s, 117256, _state);
}


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 12.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfv3unserialize(ae_serializer* s,
     rbfv3model* model,
     ae_state *_state)
{
    ae_int_t nx;
    ae_int_t ny;
    ae_int_t bftype;
    ae_int_t k;
    double bfparam;

    _rbfv3model_clear(model);

    
    /*
     * Unserialize primary model parameters, initialize model.
     *
     * It is necessary to call RBFCreate() because some internal fields
     * which are NOT unserialized will need initialization.
     */
    ae_serializer_unserialize_int(s, &nx, _state);
    ae_serializer_unserialize_int(s, &ny, _state);
    ae_serializer_unserialize_int(s, &bftype, _state);
    ae_serializer_unserialize_double(s, &bfparam, _state);
    rbfv3create(nx, ny, bftype, bfparam, model, _state);
    ae_serializer_unserialize_int(s, &model->nc, _state);
    unserializerealarray(s, &model->s, _state);
    unserializerealmatrix(s, &model->v, _state);
    unserializerealarray(s, &model->cw, _state);
    unserializeintegerarray(s, &model->pointindexes, _state);
    
    /*
     * End of stream, check that no additional data is present
     */
    ae_serializer_unserialize_int(s, &k, _state);
    ae_assert(k==117256, "RBFV3Unserialize: unexpected payload detected in the data stream. Integrity check failed", _state);
    
    /*
     * Finalize construction
     */
    rbfv3_createfastevaluator(model, _state);
}


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
double rbfv3calc1(rbfv3model* s, double x0, ae_state *_state)
{
    double result;


    ae_assert(ae_isfinite(x0, _state), "RBFCalc1: invalid value for X0 (X0 is Inf)!", _state);
    if( s->ny!=1||s->nx!=1 )
    {
        result = (double)(0);
        return result;
    }
    result = s->v.ptr.pp_double[0][0]*x0-s->v.ptr.pp_double[0][1];
    s->calcbuf.x123.ptr.p_double[0] = x0;
    rbfv3tscalcbuf(s, &s->calcbuf, &s->calcbuf.x123, &s->calcbuf.y123, _state);
    result = s->calcbuf.y123.ptr.p_double[0];
    return result;
}


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
double rbfv3calc2(rbfv3model* s, double x0, double x1, ae_state *_state)
{
    double result;


    ae_assert(ae_isfinite(x0, _state), "RBFCalc2: invalid value for X0 (X0 is Inf)!", _state);
    ae_assert(ae_isfinite(x1, _state), "RBFCalc2: invalid value for X1 (X1 is Inf)!", _state);
    if( s->ny!=1||s->nx!=2 )
    {
        result = (double)(0);
        return result;
    }
    result = s->v.ptr.pp_double[0][0]*x0+s->v.ptr.pp_double[0][1]*x1+s->v.ptr.pp_double[0][2];
    if( s->nc==0 )
    {
        return result;
    }
    s->calcbuf.x123.ptr.p_double[0] = x0;
    s->calcbuf.x123.ptr.p_double[1] = x1;
    rbfv3tscalcbuf(s, &s->calcbuf, &s->calcbuf.x123, &s->calcbuf.y123, _state);
    result = s->calcbuf.y123.ptr.p_double[0];
    return result;
}


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
     ae_state *_state)
{
    double result;


    ae_assert(ae_isfinite(x0, _state), "RBFCalc3: invalid value for X0 (X0 is Inf or NaN)!", _state);
    ae_assert(ae_isfinite(x1, _state), "RBFCalc3: invalid value for X1 (X1 is Inf or NaN)!", _state);
    ae_assert(ae_isfinite(x2, _state), "RBFCalc3: invalid value for X2 (X2 is Inf or NaN)!", _state);
    if( s->ny!=1||s->nx!=3 )
    {
        result = (double)(0);
        return result;
    }
    result = s->v.ptr.pp_double[0][0]*x0+s->v.ptr.pp_double[0][1]*x1+s->v.ptr.pp_double[0][2]*x2+s->v.ptr.pp_double[0][3];
    if( s->nc==0 )
    {
        return result;
    }
    s->calcbuf.x123.ptr.p_double[0] = x0;
    s->calcbuf.x123.ptr.p_double[1] = x1;
    s->calcbuf.x123.ptr.p_double[2] = x2;
    rbfv3tscalcbuf(s, &s->calcbuf, &s->calcbuf.x123, &s->calcbuf.y123, _state);
    result = s->calcbuf.y123.ptr.p_double[0];
    return result;
}


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
     ae_state *_state)
{


    rbfv3tscalcbuf(s, &s->calcbuf, x, y, _state);
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
     Copyright 12.12.2021 by Bochkanov Sergey
*************************************************************************/
void rbfv3tscalcbuf(const rbfv3model* s,
     rbfv3calcbuffer* buf,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t nx;
    ae_int_t ny;
    ae_int_t i;
    ae_int_t j;
    double distance0;
    ae_int_t colidx;
    ae_int_t srcidx;
    ae_int_t widx;
    ae_int_t curchunk;


    ae_assert(x->cnt>=s->nx, "RBFV3TsCalcBuf: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFV3TsCalcBuf: X contains infinite or NaN values", _state);
    nx = s->nx;
    ny = s->ny;
    
    /*
     * Handle linear term
     */
    if( y->cnt<ny )
    {
        ae_vector_set_length(y, ny, _state);
    }
    for(i=0; i<=ny-1; i++)
    {
        y->ptr.p_double[i] = s->v.ptr.pp_double[i][nx];
        for(j=0; j<=nx-1; j++)
        {
            y->ptr.p_double[i] = y->ptr.p_double[i]+s->v.ptr.pp_double[i][j]*x->ptr.p_double[j];
        }
    }
    if( s->nc==0 )
    {
        return;
    }
    
    /*
     * Handle RBF term
     */
    ae_assert((s->bftype==1||s->bftype==2)||s->bftype==3, "RBFV3TsCalcBuf: unsupported basis function type", _state);
    for(j=0; j<=nx-1; j++)
    {
        buf->x.ptr.p_double[j] = x->ptr.p_double[j]/s->s.ptr.p_double[j];
    }
    rallocv(s->evaluator.chunksize, &buf->evalbuf.funcbuf, _state);
    rallocv(s->evaluator.chunksize, &buf->evalbuf.wrkbuf, _state);
    colidx = 0;
    srcidx = 0;
    widx = 0;
    distance0 = 1.0E-50;
    if( s->bftype==1 )
    {
        
        /*
         * Kernels that add squared parameter to the squared distance
         */
        distance0 = ae_sqr(s->bfparam, _state);
    }
    while(colidx<s->nc)
    {
        
        /*
         * Handle basecase with size at most ChunkSize*ChunkSize
         */
        curchunk = ae_minint(s->evaluator.chunksize, s->nc-colidx, _state);
        rbfv3_computerowchunk(&s->evaluator, &buf->x, &buf->evalbuf, curchunk, srcidx, distance0, 0, _state);
        for(i=0; i<=ny-1; i++)
        {
            y->ptr.p_double[i] = y->ptr.p_double[i]+rdotvr(curchunk, &buf->evalbuf.funcbuf, &s->wchunked, widx+i, _state);
        }
        colidx = colidx+curchunk;
        srcidx = srcidx+nx;
        widx = widx+ny;
    }
}


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
     ae_state *_state)
{
    ae_int_t nx;
    ae_int_t ny;
    ae_int_t i;
    ae_int_t j;


    ae_assert(x->cnt>=s->nx, "RBFV3TsCalcBuf: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFV3TsCalcBuf: X contains infinite or NaN values", _state);
    nx = s->nx;
    ny = s->ny;
    
    /*
     * Handle linear term
     */
    if( y->cnt<ny )
    {
        ae_vector_set_length(y, ny, _state);
    }
    for(i=0; i<=ny-1; i++)
    {
        y->ptr.p_double[i] = s->v.ptr.pp_double[i][nx];
        for(j=0; j<=nx-1; j++)
        {
            y->ptr.p_double[i] = y->ptr.p_double[i]+s->v.ptr.pp_double[i][j]*x->ptr.p_double[j];
        }
    }
    if( s->nc==0 )
    {
        return;
    }
    
    /*
     * Handle RBF term
     */
    rallocm(1, nx, &buf->x2d, _state);
    for(j=0; j<=nx-1; j++)
    {
        buf->x2d.ptr.pp_double[0][j] = x->ptr.p_double[j]/s->s.ptr.p_double[j];
    }
    rbfv3_fastevaluatorcomputebatch(&s->fasteval, &buf->x2d, 1, ae_true, &buf->y2d, _state);
    for(i=0; i<=ny-1; i++)
    {
        y->ptr.p_double[i] = y->ptr.p_double[i]+buf->y2d.ptr.pp_double[i][0];
    }
}


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
     ae_state *_state)
{
    ae_int_t nx;
    ae_int_t ny;
    ae_int_t i;
    ae_int_t j;
    double smalldist2;
    ae_bool nograd;
    ae_int_t colidx;
    ae_int_t srcidx;
    ae_int_t widx;
    ae_int_t curchunk;
    ae_int_t maxchunksize;
    double distance0;


    ae_assert(x->cnt>=s->nx, "RBFV3TsCalcBuf: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFV3TsCalcBuf: X contains infinite or NaN values", _state);
    nx = s->nx;
    ny = s->ny;
    
    /*
     * Handle linear term
     */
    if( y->cnt<ny )
    {
        ae_vector_set_length(y, ny, _state);
    }
    if( dy->cnt<s->ny*s->nx )
    {
        ae_vector_set_length(dy, s->ny*s->nx, _state);
    }
    for(i=0; i<=ny-1; i++)
    {
        y->ptr.p_double[i] = s->v.ptr.pp_double[i][nx];
        for(j=0; j<=nx-1; j++)
        {
            y->ptr.p_double[i] = y->ptr.p_double[i]+s->v.ptr.pp_double[i][j]*x->ptr.p_double[j];
            dy->ptr.p_double[i*nx+j] = s->v.ptr.pp_double[i][j];
        }
    }
    if( s->nc==0 )
    {
        return;
    }
    
    /*
     * Rescale X and DY to the internal scaling used by the RBF model
     */
    for(j=0; j<=nx-1; j++)
    {
        buf->x.ptr.p_double[j] = x->ptr.p_double[j]/s->s.ptr.p_double[j];
    }
    for(i=0; i<=ny-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            dy->ptr.p_double[i*nx+j] = dy->ptr.p_double[i*nx+j]*s->s.ptr.p_double[j];
        }
    }
    
    /*
     * Prepare information necessary for the detection of the nonexistent gradient
     */
    nograd = ae_false;
    smalldist2 = (rdotv2(nx, &buf->x, _state)+1.0)*ae_sqr((double)100*ae_machineepsilon, _state);
    
    /*
     * Handle RBF term
     */
    ae_assert((s->bftype==1||s->bftype==2)||s->bftype==3, "RBFV3TsDiffBuf: unsupported basis function type", _state);
    ae_assert(s->bftype!=1||ae_fp_greater_eq(s->bfparam,(double)(0)), "RBFV3TsDiffBuf: inconsistent BFType/BFParam", _state);
    maxchunksize = s->evaluator.chunksize;
    rallocv(maxchunksize, &buf->evalbuf.funcbuf, _state);
    rallocv(maxchunksize, &buf->evalbuf.wrkbuf, _state);
    rallocv(maxchunksize, &buf->evalbuf.df1, _state);
    rallocm(nx, maxchunksize, &buf->evalbuf.deltabuf, _state);
    rsetallocv(maxchunksize, 1.0E50, &buf->evalbuf.mindist2, _state);
    colidx = 0;
    srcidx = 0;
    widx = 0;
    distance0 = 1.0E-50;
    if( s->bftype==1 )
    {
        
        /*
         * Kernels that add squared parameter to the squared distance
         */
        distance0 = ae_sqr(s->bfparam, _state);
    }
    while(colidx<s->nc)
    {
        
        /*
         * Handle basecase with size at most ChunkSize*ChunkSize
         */
        curchunk = ae_minint(maxchunksize, s->nc-colidx, _state);
        rbfv3_computerowchunk(&s->evaluator, &buf->x, &buf->evalbuf, curchunk, srcidx, distance0, 1, _state);
        for(j=0; j<=nx-1; j++)
        {
            rmergemulvr(curchunk, &buf->evalbuf.df1, &buf->evalbuf.deltabuf, j, _state);
        }
        for(i=0; i<=ny-1; i++)
        {
            y->ptr.p_double[i] = y->ptr.p_double[i]+rdotvr(curchunk, &buf->evalbuf.funcbuf, &s->wchunked, widx+i, _state);
            for(j=0; j<=nx-1; j++)
            {
                dy->ptr.p_double[i*nx+j] = dy->ptr.p_double[i*nx+j]+(double)2*rdotrr(curchunk, &s->wchunked, widx+i, &buf->evalbuf.deltabuf, j, _state);
            }
        }
        colidx = colidx+curchunk;
        srcidx = srcidx+nx;
        widx = widx+ny;
    }
    if( s->bftype==1&&ae_fp_eq(s->bfparam,(double)(0)) )
    {
        
        /*
         * The kernel function is nondifferentiable at nodes, check whether we are close to one of the nodes or not
         */
        for(i=0; i<=maxchunksize-1; i++)
        {
            nograd = nograd||buf->evalbuf.mindist2.ptr.p_double[i]<=smalldist2;
        }
        if( nograd )
        {
            
            /*
             * The gradient is undefined at the trial point, flush it to zero
             */
            rsetv(ny*nx, 0.0, dy, _state);
        }
    }
    
    /*
     * Rescale derivatives back
     */
    for(i=0; i<=ny-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            dy->ptr.p_double[i*nx+j] = dy->ptr.p_double[i*nx+j]/s->s.ptr.p_double[j];
        }
    }
}


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
     ae_state *_state)
{
    ae_int_t nx;
    ae_int_t ny;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t k0;
    ae_int_t k1;
    ae_bool nearnode;
    ae_bool nograd;
    ae_bool nohess;
    double smalldist2;
    ae_int_t colidx;
    ae_int_t srcidx;
    ae_int_t widx;
    ae_int_t curchunk;
    ae_int_t maxchunksize;
    double distance0;


    ae_assert(x->cnt>=s->nx, "RBFV3TsCalcBuf: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFV3TsCalcBuf: X contains infinite or NaN values", _state);
    nx = s->nx;
    ny = s->ny;
    
    /*
     * Handle linear term
     */
    if( y->cnt<ny )
    {
        ae_vector_set_length(y, ny, _state);
    }
    if( dy->cnt<s->ny*s->nx )
    {
        ae_vector_set_length(dy, s->ny*s->nx, _state);
    }
    if( d2y->cnt<ny*nx*nx )
    {
        ae_vector_set_length(d2y, ny*nx*nx, _state);
    }
    for(i=0; i<=ny-1; i++)
    {
        y->ptr.p_double[i] = s->v.ptr.pp_double[i][nx];
        for(j=0; j<=nx-1; j++)
        {
            y->ptr.p_double[i] = y->ptr.p_double[i]+s->v.ptr.pp_double[i][j]*x->ptr.p_double[j];
            dy->ptr.p_double[i*nx+j] = s->v.ptr.pp_double[i][j];
        }
    }
    rsetv(ny*nx*nx, 0.0, d2y, _state);
    if( s->nc==0 )
    {
        return;
    }
    
    /*
     * Rescale X and DY to the internal scaling used by the RBF model (D2Y is zero,
     * so it does not need rescaling).
     */
    for(j=0; j<=nx-1; j++)
    {
        buf->x.ptr.p_double[j] = x->ptr.p_double[j]/s->s.ptr.p_double[j];
    }
    for(i=0; i<=ny-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            dy->ptr.p_double[i*nx+j] = dy->ptr.p_double[i*nx+j]*s->s.ptr.p_double[j];
        }
    }
    
    /*
     * Prepare information necessary for the detection of the nonexistent Hessian
     */
    nograd = ae_false;
    nohess = ae_false;
    smalldist2 = (rdotv2(nx, &buf->x, _state)+1.0)*ae_sqr((double)100*ae_machineepsilon, _state);
    
    /*
     * Handle RBF term
     */
    ae_assert(s->bftype==1||s->bftype==2, "RBFV3TsHessBuf: unsupported basis function type", _state);
    ae_assert(s->bftype!=1||ae_fp_greater_eq(s->bfparam,(double)(0)), "RBFV3TsHessBuf: inconsistent BFType/BFParam", _state);
    maxchunksize = s->evaluator.chunksize;
    rallocv(maxchunksize, &buf->evalbuf.funcbuf, _state);
    rallocv(maxchunksize, &buf->evalbuf.wrkbuf, _state);
    rallocv(maxchunksize, &buf->evalbuf.df1, _state);
    rallocv(maxchunksize, &buf->evalbuf.df2, _state);
    rallocm(nx, maxchunksize, &buf->evalbuf.deltabuf, _state);
    rsetallocv(maxchunksize, 1.0E50, &buf->evalbuf.mindist2, _state);
    colidx = 0;
    srcidx = 0;
    widx = 0;
    distance0 = 1.0E-50;
    if( s->bftype==1 )
    {
        
        /*
         * Kernels that add squared parameter to the squared distance
         */
        distance0 = ae_sqr(s->bfparam, _state);
    }
    while(colidx<s->nc)
    {
        
        /*
         * Handle basecase with size at most ChunkSize*ChunkSize
         */
        curchunk = ae_minint(maxchunksize, s->nc-colidx, _state);
        rbfv3_computerowchunk(&s->evaluator, &buf->x, &buf->evalbuf, curchunk, srcidx, distance0, 2, _state);
        for(i=0; i<=ny-1; i++)
        {
            y->ptr.p_double[i] = y->ptr.p_double[i]+rdotvr(curchunk, &buf->evalbuf.funcbuf, &s->wchunked, widx+i, _state);
            for(k0=0; k0<=nx-1; k0++)
            {
                rcopyrv(curchunk, &buf->evalbuf.deltabuf, k0, &buf->evalbuf.wrkbuf, _state);
                rmergemulv(curchunk, &buf->evalbuf.df1, &buf->evalbuf.wrkbuf, _state);
                dy->ptr.p_double[i*nx+k0] = dy->ptr.p_double[i*nx+k0]+(double)2*rdotvr(curchunk, &buf->evalbuf.wrkbuf, &s->wchunked, widx+i, _state);
            }
            for(k0=0; k0<=nx-1; k0++)
            {
                for(k1=0; k1<=nx-1; k1++)
                {
                    rcopyv(curchunk, &buf->evalbuf.df2, &buf->evalbuf.wrkbuf, _state);
                    rmergemulrv(curchunk, &buf->evalbuf.deltabuf, k0, &buf->evalbuf.wrkbuf, _state);
                    rmergemulrv(curchunk, &buf->evalbuf.deltabuf, k1, &buf->evalbuf.wrkbuf, _state);
                    d2y->ptr.p_double[i*nx*nx+k0*nx+k1] = d2y->ptr.p_double[i*nx*nx+k0*nx+k1]+(double)4*rdotvr(curchunk, &buf->evalbuf.wrkbuf, &s->wchunked, widx+i, _state);
                    if( k0==k1 )
                    {
                        d2y->ptr.p_double[i*nx*nx+k0*nx+k1] = d2y->ptr.p_double[i*nx*nx+k0*nx+k1]+(double)2*rdotvr(curchunk, &buf->evalbuf.df1, &s->wchunked, widx+i, _state);
                    }
                }
            }
        }
        colidx = colidx+curchunk;
        srcidx = srcidx+nx;
        widx = widx+ny;
    }
    nearnode = ae_false;
    if( (s->bftype==1&&ae_fp_eq(s->bfparam,(double)(0)))||s->bftype==2 )
    {
        
        /*
         * The kernel function is nondifferentiable at nodes, check whether we are close to one of the nodes or not
         */
        for(i=0; i<=maxchunksize-1; i++)
        {
            nearnode = nearnode||buf->evalbuf.mindist2.ptr.p_double[i]<=smalldist2;
        }
    }
    nograd = nearnode&&(s->bftype==1&&ae_fp_eq(s->bfparam,(double)(0)));
    nohess = nearnode&&((s->bftype==1&&ae_fp_eq(s->bfparam,(double)(0)))||s->bftype==2);
    if( nograd )
    {
        
        /*
         * The gradient is undefined at the trial point, flush it to zero
         */
        rsetv(ny*nx, 0.0, dy, _state);
    }
    if( nohess )
    {
        
        /*
         * The Hessian is undefined at the trial point, flush it to zero
         */
        rsetv(ny*nx*nx, 0.0, d2y, _state);
    }
    
    /*
     * Rescale derivatives back
     */
    for(i=0; i<=ny-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            dy->ptr.p_double[i*nx+j] = dy->ptr.p_double[i*nx+j]/s->s.ptr.p_double[j];
        }
    }
    for(i=0; i<=ny-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            for(k=0; k<=nx-1; k++)
            {
                d2y->ptr.p_double[i*nx*nx+j*nx+k] = d2y->ptr.p_double[i*nx*nx+j*nx+k]/(s->s.ptr.p_double[j]*s->s.ptr.p_double[k]);
            }
        }
    }
}


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
     ae_state *_state)
{
    ae_frame _frame_block;
    rbfv3calcbuffer bufseed;
    ae_shared_pool bufpool;
    ae_int_t simdwidth;
    ae_int_t tilescnt;

    ae_frame_make(_state, &_frame_block);
    memset(&bufseed, 0, sizeof(bufseed));
    memset(&bufpool, 0, sizeof(bufpool));
    _rbfv3calcbuffer_init(&bufseed, _state, ae_true);
    ae_shared_pool_init(&bufpool, _state, ae_true);

    
    /*
     * Perform integrity checks
     */
    ae_assert(s->nx==2||s->nx==3, "RBFGridCalcVX: integrity check failed", _state);
    ae_assert(((n0>=1&&n1>=1)&&n2>=1)&&n3>=1, "RBFGridCalcVX: integrity check failed", _state);
    ae_assert(s->nx>=4||((x3->cnt>=1&&ae_fp_eq(x3->ptr.p_double[0],(double)(0)))&&n3==1), "RBFGridCalcVX: integrity check failed", _state);
    ae_assert(s->nx>=3||((x2->cnt>=1&&ae_fp_eq(x2->ptr.p_double[0],(double)(0)))&&n2==1), "RBFGridCalcVX: integrity check failed", _state);
    ae_assert(s->nx>=2||((x1->cnt>=1&&ae_fp_eq(x1->ptr.p_double[0],(double)(0)))&&n1==1), "RBFGridCalcVX: integrity check failed", _state);
    ae_assert(!sparsey||flagy->cnt>=n0*n1*n2*n3, "RBFGridCalcVX: integrity check failed", _state);
    
    /*
     * Prepare shared pool
     */
    rbfv3createcalcbuffer(s, &bufseed, _state);
    ae_shared_pool_set_seed(&bufpool, &bufseed, (ae_int_t)sizeof(bufseed), (ae_constructor)_rbfv3calcbuffer_init, (ae_copy_constructor)_rbfv3calcbuffer_init_copy, (ae_destructor)_rbfv3calcbuffer_destroy, _state);
    
    /*
     * Call worker function
     */
    simdwidth = 8;
    tilescnt = idivup(n0, simdwidth, _state)*idivup(n1, simdwidth, _state)*idivup(n2, simdwidth, _state)*idivup(n3, simdwidth, _state);
    rbfv3_gridcalcrec(s, simdwidth, 0, tilescnt, x0, n0, x1, n1, x2, n2, x3, n3, flagy, sparsey, y, &bufpool, ae_true, _state);
    ae_frame_leave(_state);
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t cwwidth;
    ae_bool recognized;

    *nx = 0;
    *ny = 0;
    ae_matrix_clear(xwr);
    *nc = 0;
    ae_matrix_clear(v);

    *nx = s->nx;
    *ny = s->ny;
    *nc = s->nc;
    
    /*
     * Fill V
     */
    ae_matrix_set_length(v, s->ny, s->nx+1, _state);
    for(i=0; i<=s->ny-1; i++)
    {
        rcopyrr(*nx+1, &s->v, i, v, i, _state);
    }
    
    /*
     * Fill XWR
     */
    if( *nc>0 )
    {
        cwwidth = *nx+(*ny);
        ae_matrix_set_length(xwr, *nc, *nx+(*ny)+(*nx)+3, _state);
        for(i=0; i<=*nc-1; i++)
        {
            
            /*
             * Output centers (in the original variable scaling), weights and radii
             */
            for(j=0; j<=*nx-1; j++)
            {
                xwr->ptr.pp_double[i][j] = s->cw.ptr.p_double[i*cwwidth+j]*s->s.ptr.p_double[j];
            }
            for(j=0; j<=*ny-1; j++)
            {
                xwr->ptr.pp_double[i][*nx+j] = s->cw.ptr.p_double[i*cwwidth+(*nx)+j];
            }
            for(j=0; j<=*nx-1; j++)
            {
                xwr->ptr.pp_double[i][*nx+(*ny)+j] = s->s.ptr.p_double[j];
            }
            
            /*
             * Recognize specific basis function used and perform post-processing
             */
            recognized = ae_false;
            if( s->bftype==1&&ae_fp_eq(s->bfparam,(double)(0)) )
            {
                
                /*
                 * Biharmonic kernel f=r
                 *
                 * Weights are multiplied by -1 because actually it is f=-r (the latter
                 * is conditionally positive definite basis function, and the former is
                 * how it is known to most users)
                 */
                xwr->ptr.pp_double[i][*nx+(*ny)+(*nx)+0] = (double)(1);
                xwr->ptr.pp_double[i][*nx+(*ny)+(*nx)+1] = 0.0;
                for(j=0; j<=*ny-1; j++)
                {
                    xwr->ptr.pp_double[i][*nx+j] = -xwr->ptr.pp_double[i][*nx+j];
                }
                recognized = ae_true;
            }
            if( s->bftype==1&&ae_fp_greater(s->bfparam,(double)(0)) )
            {
                
                /*
                 * Multiquadric f=sqrt(r^2+alpha^2)
                 *
                 * Weights are multiplied by -1 because actually it is f=-sqrt(r^2+alpha^2)
                 * (the latter is conditionally positive definite basis function, and the
                 * former is how it is known to most users)
                 */
                xwr->ptr.pp_double[i][*nx+(*ny)+(*nx)+0] = (double)(10);
                xwr->ptr.pp_double[i][*nx+(*ny)+(*nx)+1] = s->bfparam;
                for(j=0; j<=*ny-1; j++)
                {
                    xwr->ptr.pp_double[i][*nx+j] = -xwr->ptr.pp_double[i][*nx+j];
                }
                recognized = ae_true;
            }
            if( s->bftype==2 )
            {
                
                /*
                 * Thin plate spline f=r^2*ln(r)
                 */
                xwr->ptr.pp_double[i][*nx+(*ny)+(*nx)+0] = (double)(2);
                xwr->ptr.pp_double[i][*nx+(*ny)+(*nx)+1] = (double)(0);
                recognized = ae_true;
            }
            ae_assert(recognized, "RBFV3: integrity check 5342 failed", _state);
            
            /*
             * Output indexes
             */
            xwr->ptr.pp_double[i][*nx+(*ny)+(*nx)+2] = (double)(s->pointindexes.ptr.p_int[i]);
        }
    }
}


/*************************************************************************
Get maximum panel size for a fast evaluator

  -- ALGLIB --
     Copyright 27.08.2022 by Sergey Bochkanov
*************************************************************************/
ae_int_t rbf3getmaxpanelsize(ae_state *_state)
{
    ae_int_t result;


    result = rbfv3_defaultmaxpanelsize;
    return result;
}


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
void rbf3pushfastevaltol(rbfv3model* s, double tol, ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t seed0;
    ae_int_t seed1;
    hqrndstate rs;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t nsampled;
    ae_vector x;
    ae_vector ya;
    ae_vector yb;
    double avgrawerror;
    double maxrawerror;
    rbfv3calcbuffer buf;
    double tolgrowth;
    double maxtolgrowth;

    ae_frame_make(_state, &_frame_block);
    memset(&rs, 0, sizeof(rs));
    memset(&x, 0, sizeof(x));
    memset(&ya, 0, sizeof(ya));
    memset(&yb, 0, sizeof(yb));
    memset(&buf, 0, sizeof(buf));
    _hqrndstate_init(&rs, _state, ae_true);
    ae_vector_init(&x, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ya, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&yb, 0, DT_REAL, _state, ae_true);
    _rbfv3calcbuffer_init(&buf, _state, ae_true);

    ae_assert(ae_fp_greater(tol,(double)(0)), "RBF3PushFastEvalTol: TOL<=0", _state);
    if( s->nc==0 )
    {
        ae_frame_leave(_state);
        return;
    }
    rbfv3createcalcbuffer(s, &buf, _state);
    
    /*
     * Choose seeds for RNG that produces indexes of points being sampled
     */
    nsampled = 100;
    maxtolgrowth = 1.0E6;
    seed0 = 47623;
    seed1 = 83645264;
    
    /*
     * Push 'raw' tolerance, sample random points and compute errors
     */
    rbfv3_fastevaluatorpushtol(&s->fasteval, tol, _state);
    avgrawerror = (double)(0);
    maxrawerror = (double)(0);
    rallocv(s->nx, &x, _state);
    hqrndseed(seed0, seed1, &rs, _state);
    for(i=0; i<=nsampled-1; i++)
    {
        
        /*
         * Sample point
         */
        k = hqrnduniformi(&rs, s->nc, _state);
        for(j=0; j<=s->nx-1; j++)
        {
            x.ptr.p_double[j] = s->cw.ptr.p_double[(s->nx+s->ny)*k+j];
        }
        
        /*
         * Compute reference value, fast value, compare
         */
        rbfv3tscalcbuf(s, &buf, &x, &ya, _state);
        rbfv3tsfastcalcbuf(s, &buf, &x, &yb, _state);
        for(j=0; j<=s->ny-1; j++)
        {
            avgrawerror = avgrawerror+ae_fabs(ya.ptr.p_double[j]-yb.ptr.p_double[j], _state);
            maxrawerror = ae_maxreal(maxrawerror, ae_fabs(ya.ptr.p_double[j]-yb.ptr.p_double[j], _state), _state);
        }
    }
    avgrawerror = avgrawerror/(double)(nsampled*s->ny);
    
    /*
     * Compute proposed growth for the target tolerance.
     *
     * NOTE: a heuristic formula is used which works well in practice.
     */
    tolgrowth = tol/ae_maxreal(avgrawerror*(double)25+tol/maxtolgrowth, maxrawerror*(double)5+tol/maxtolgrowth, _state);
    if( ae_fp_less(tolgrowth,(double)(1)) )
    {
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Adjust tolerance
     */
    rbfv3_fastevaluatorpushtol(&s->fasteval, tol*tolgrowth, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Allocate temporaries in the evaluator buffer
*************************************************************************/
static void rbfv3_evalbufferinit(rbf3evaluatorbuffer* buf,
     ae_int_t nx,
     ae_int_t maxpanelsize,
     ae_state *_state)
{


    rallocv(maxpanelsize, &buf->funcbuf, _state);
    rallocv(maxpanelsize, &buf->wrkbuf, _state);
    rallocv(maxpanelsize, &buf->df1, _state);
    rallocv(maxpanelsize, &buf->df2, _state);
    rallocm(nx, maxpanelsize, &buf->deltabuf, _state);
    rallocv(maxpanelsize, &buf->mindist2, _state);
    rallocv(maxpanelsize, &buf->coeffbuf, _state);
    rallocv(nx, &buf->x, _state);
}


/*************************************************************************
Recursive function for the fast evaluator
*************************************************************************/
static ae_int_t rbfv3_fastevaluatorinitrec(rbf3fastevaluator* eval,
     /* Real    */ const ae_matrix* xx,
     /* Integer */ ae_vector* ptidx,
     /* Real    */ ae_vector* coordbuf,
     ae_int_t idx0,
     ae_int_t idx1,
     nrpool* nxpool,
     ae_state *_state)
{
    ae_frame _frame_block;
    rbf3panel *panel;
    ae_smart_ptr _panel;
    ae_vector boxmin;
    ae_vector boxmax;
    ae_int_t i;
    ae_int_t j;
    double v;
    ae_int_t idxmid;
    ae_int_t subsetlength;
    ae_int_t subset0;
    ae_int_t subset1;
    ae_int_t largestdim;
    ae_int_t result;

    ae_frame_make(_state, &_frame_block);
    memset(&_panel, 0, sizeof(_panel));
    memset(&boxmin, 0, sizeof(boxmin));
    memset(&boxmax, 0, sizeof(boxmax));
    ae_smart_ptr_init(&_panel, (void**)&panel, _state, ae_true);
    ae_vector_init(&boxmin, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&boxmax, 0, DT_REAL, _state, ae_true);

    ae_assert(idx1>idx0, "FastEvaluatorInitRec: Idx1<=Idx0", _state);
    subsetlength = idx1-idx0;
    
    /*
     * Add panel to the array, prepare to return its index
     */
    panel = (rbf3panel*)ae_malloc(sizeof(rbf3panel), _state); /* note: using panel as a temporary prior to assigning its value to _panel */
    memset(panel, 0, sizeof(rbf3panel));
    _rbf3panel_init(panel, _state, ae_false);
    ae_smart_ptr_assign(&_panel, panel, ae_true, ae_true, (ae_int_t)sizeof(rbf3panel), _rbf3panel_init_copy, _rbf3panel_destroy);
    result = ae_obj_array_append_transfer(&eval->panels, &_panel, _state);
    
    /*
     * Prepare panel fields that are always set:
     * * panel center and radius
     * * default far field state (not present)
     */
    rsetallocv(eval->nx, 0.0, &panel->clustercenter, _state);
    for(i=idx0; i<=idx1-1; i++)
    {
        for(j=0; j<=eval->nx-1; j++)
        {
            panel->clustercenter.ptr.p_double[j] = panel->clustercenter.ptr.p_double[j]+xx->ptr.pp_double[ptidx->ptr.p_int[i]][j];
        }
    }
    for(j=0; j<=eval->nx-1; j++)
    {
        panel->clustercenter.ptr.p_double[j] = panel->clustercenter.ptr.p_double[j]/(double)subsetlength;
    }
    if( eval->nx<=4&&eval->nx>0 )
    {
        panel->c0 = panel->clustercenter.ptr.p_double[0];
    }
    if( eval->nx<=4&&eval->nx>1 )
    {
        panel->c1 = panel->clustercenter.ptr.p_double[1];
    }
    if( eval->nx<=4&&eval->nx>2 )
    {
        panel->c2 = panel->clustercenter.ptr.p_double[2];
    }
    if( eval->nx<=4&&eval->nx>3 )
    {
        panel->c3 = panel->clustercenter.ptr.p_double[3];
    }
    panel->clusterrad = 1.0E-50;
    for(i=idx0; i<=idx1-1; i++)
    {
        v = (double)(0);
        for(j=0; j<=eval->nx-1; j++)
        {
            v = v+ae_sqr(panel->clustercenter.ptr.p_double[j]-xx->ptr.pp_double[ptidx->ptr.p_int[i]][j], _state);
        }
        panel->clusterrad = ae_maxreal(panel->clusterrad, v, _state);
    }
    panel->clusterrad = ae_sqrt(panel->clusterrad, _state);
    panel->farfieldexpansion = rbfv3_farfieldnone;
    panel->farfielddistance = 0.0;
    panel->idx0 = idx0;
    panel->idx1 = idx1;
    
    /*
     * Handle leaf panel (small enough)
     */
    if( subsetlength<=eval->maxpanelsize )
    {
        panel->paneltype = 0;
        iallocv(subsetlength, &panel->ptidx, _state);
        rallocm(eval->nx, subsetlength, &panel->xt, _state);
        for(i=idx0; i<=idx1-1; i++)
        {
            panel->ptidx.ptr.p_int[i-idx0] = ptidx->ptr.p_int[i];
            for(j=0; j<=eval->nx-1; j++)
            {
                v = xx->ptr.pp_double[ptidx->ptr.p_int[i]][j];
                panel->xt.ptr.pp_double[j][i-idx0] = v;
                eval->permx.ptr.pp_double[i][j] = v;
            }
        }
        rsetallocm(eval->ny, subsetlength, 0.0, &panel->wt, _state);
        rbfv3_evalbufferinit(&panel->tgtbuf, eval->nx, eval->maxpanelsize, _state);
        ae_frame_leave(_state);
        return result;
    }
    
    /*
     * Prepare temporaries
     */
    nrpoolretrieve(nxpool, &boxmin, _state);
    nrpoolretrieve(nxpool, &boxmax, _state);
    
    /*
     * Prepare to split large panel:
     * * compute bounding box and its largest dimension
     * * sort points by largest dimension of the bounding box
     * * split in two
     */
    rcopyrv(eval->nx, xx, ptidx->ptr.p_int[idx0], &boxmin, _state);
    rcopyrv(eval->nx, xx, ptidx->ptr.p_int[idx0], &boxmax, _state);
    for(i=idx0+1; i<=idx1-1; i++)
    {
        for(j=0; j<=eval->nx-1; j++)
        {
            boxmin.ptr.p_double[j] = ae_minreal(boxmin.ptr.p_double[j], xx->ptr.pp_double[ptidx->ptr.p_int[i]][j], _state);
            boxmax.ptr.p_double[j] = ae_maxreal(boxmax.ptr.p_double[j], xx->ptr.pp_double[ptidx->ptr.p_int[i]][j], _state);
        }
    }
    largestdim = 0;
    for(j=1; j<=eval->nx-1; j++)
    {
        if( ae_fp_greater(boxmax.ptr.p_double[j]-boxmin.ptr.p_double[j],boxmax.ptr.p_double[largestdim]-boxmin.ptr.p_double[largestdim]) )
        {
            largestdim = j;
        }
    }
    for(i=idx0; i<=idx1-1; i++)
    {
        coordbuf->ptr.p_double[i] = xx->ptr.pp_double[ptidx->ptr.p_int[i]][largestdim];
    }
    tagsortmiddleri(coordbuf, ptidx, idx0, subsetlength, _state);
    ae_assert(subsetlength>eval->maxpanelsize, "RBF3: integrity check 2955 failed", _state);
    tiledsplit(subsetlength, icase2(subsetlength>rbfv3_minfarfieldsize, rbfv3_minfarfieldsize, eval->maxpanelsize, _state), &subset0, &subset1, _state);
    idxmid = idx0+subset0;
    
    /*
     * Return temporaries back to nxPool and perform recursive processing
     */
    nrpoolrecycle(nxpool, &boxmin, _state);
    nrpoolrecycle(nxpool, &boxmax, _state);
    panel->paneltype = 1;
    panel->childa = rbfv3_fastevaluatorinitrec(eval, xx, ptidx, coordbuf, idx0, idxmid, nxpool, _state);
    panel->childb = rbfv3_fastevaluatorinitrec(eval, xx, ptidx, coordbuf, idxmid, idx1, nxpool, _state);
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Initialize fast model evaluator using current dataset and default  (zero)
coefficients.

The coefficients can be loaded later  with  RBF3FastEvaluatorLoadCoeffs()
or RBF3FastEvaluatorLoadCoeffs1().
*************************************************************************/
static void rbfv3_fastevaluatorinit(rbf3fastevaluator* eval,
     /* Real    */ const ae_matrix* _x,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t maxpanelsize,
     ae_int_t bftype,
     double bfparam,
     ae_bool usedebugcounters,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix x;
    ae_vector coordbuf;
    ae_int_t rootidx;
    nrpool nxpool;
    ae_int_t i;
    rbf3evaluatorbuffer bufseed;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&coordbuf, 0, sizeof(coordbuf));
    memset(&nxpool, 0, sizeof(nxpool));
    memset(&bufseed, 0, sizeof(bufseed));
    ae_matrix_init_copy(&x, _x, _state, ae_true);
    ae_vector_init(&coordbuf, 0, DT_REAL, _state, ae_true);
    _nrpool_init(&nxpool, _state, ae_true);
    _rbf3evaluatorbuffer_init(&bufseed, _state, ae_true);

    
    /*
     * Prepare the evaluator and temporaries
     */
    eval->n = n;
    eval->nx = nx;
    eval->ny = ny;
    eval->maxpanelsize = maxpanelsize;
    eval->functype = bftype;
    eval->funcparam = bfparam;
    ae_obj_array_clear(&eval->panels);
    rsetallocm(n, 3+ny, 0.0, &eval->tmpx3w, _state);
    rsetallocm(ny, n, 0.0, &eval->wstoredorig, _state);
    rallocm(n, nx, &eval->permx, _state);
    rbfv3_evalbufferinit(&bufseed, eval->nx, eval->maxpanelsize, _state);
    ae_shared_pool_set_seed(&eval->bufferpool, &bufseed, (ae_int_t)sizeof(bufseed), (ae_constructor)_rbf3evaluatorbuffer_init, (ae_copy_constructor)_rbf3evaluatorbuffer_init_copy, (ae_destructor)_rbf3evaluatorbuffer_destroy, _state);
    eval->usedebugcounters = usedebugcounters;
    eval->dbgpanel2panelcnt = 0;
    eval->dbgfield2panelcnt = 0;
    eval->dbgpanelscnt = 0;
    eval->isloaded = ae_false;
    
    /*
     * Perform recursive subdivision, generate panels
     */
    iallocv(n, &eval->origptidx, _state);
    for(i=0; i<=n-1; i++)
    {
        eval->origptidx.ptr.p_int[i] = i;
    }
    rallocv(n, &coordbuf, _state);
    nrpoolinit(&nxpool, nx, _state);
    rootidx = rbfv3_fastevaluatorinitrec(eval, &x, &eval->origptidx, &coordbuf, 0, n, &nxpool, _state);
    ae_assert(rootidx==0, "FastEvaluatorInit: integrity check for RootIdx failed", _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Recursive subroutine that loads coefficients into the fast evaluator. The
coefficients are expected to be in Eval.WStoredOrig[NY,N]

Depending  on  the  settings  specified  during evaluator creation (basis
function type and parameter), far field expansion can be  built  for  the
model.
*************************************************************************/
static void rbfv3_fastevaluatorloadcoeffsrec(rbf3fastevaluator* eval,
     ae_int_t treenodeidx,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t npts;
    rbf3panel *panel;
    ae_smart_ptr _panel;

    ae_frame_make(_state, &_frame_block);
    memset(&_panel, 0, sizeof(_panel));
    ae_smart_ptr_init(&_panel, (void**)&panel, _state, ae_true);

    ae_obj_array_get(&eval->panels, treenodeidx, &_panel, _state);
    npts = panel->idx1-panel->idx0;
    
    /*
     * Create far field expansion, if possible
     */
    panel->farfieldexpansion = rbfv3_farfieldnone;
    if( ((eval->functype==1&&npts>=rbfv3_minfarfieldsize)&&ae_fp_eq(eval->funcparam,0.0))&&eval->nx<=3 )
    {
        
        /*
         * Use far field expansions for a biharmonic kernel
         */
        for(i=panel->idx0; i<=panel->idx1-1; i++)
        {
            for(j=0; j<=eval->nx-1; j++)
            {
                eval->tmpx3w.ptr.pp_double[i][j] = eval->permx.ptr.pp_double[i][j];
            }
            for(j=0; j<=eval->ny-1; j++)
            {
                eval->tmpx3w.ptr.pp_double[i][3+j] = eval->wstoredorig.ptr.pp_double[j][eval->origptidx.ptr.p_int[i]];
            }
        }
        bhpanelinit(&panel->bhexpansion, &eval->tmpx3w, panel->idx0, panel->idx1, eval->ny, &eval->bheval, _state);
        panel->farfieldexpansion = rbfv3_farfieldbiharmonic;
        panel->farfielddistance = panel->bhexpansion.useatdistance;
    }
    
    /*
     * Non-leaf panel with two children
     */
    if( panel->paneltype==1 )
    {
        
        /*
         * Load coefficients into child panels.
         */
        rbfv3_fastevaluatorloadcoeffsrec(eval, panel->childa, _state);
        rbfv3_fastevaluatorloadcoeffsrec(eval, panel->childb, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Leaf panel
     */
    ae_assert(panel->paneltype==0, "RBF3: integrity check 4594 failed", _state);
    for(i=0; i<=eval->ny-1; i++)
    {
        for(j=0; j<=npts-1; j++)
        {
            panel->wt.ptr.pp_double[i][j] = eval->wstoredorig.ptr.pp_double[i][panel->ptidx.ptr.p_int[j]];
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Loads coefficients into fast evaluator built with NY=1.

Depending  on  the  settings  specified  during evaluator creation (basis
function type and parameter), far field expansion can be  built  for  the
model.

INPUT PARAMETERS:
    Eval            -   fast evaluator to load coefficients to
    W               -   coeffs vector
*************************************************************************/
static void rbfv3_fastevaluatorloadcoeffs1(rbf3fastevaluator* eval,
     /* Real    */ const ae_vector* w,
     ae_state *_state)
{


    ae_assert(eval->ny==1, "FastEvaluatorLoadCoeffs1: Eval.NY<>1", _state);
    ae_assert(ae_obj_array_get_length(&eval->panels)>0, "FastEvaluatorLoadCoeffs1: Length(Panels)=0", _state);
    
    /*
     * Prepare problem-specific evaluator, if any
     */
    if( (eval->functype==1&&ae_fp_eq(eval->funcparam,0.0))&&eval->nx<=3 )
    {
        biharmonicevaluatorinit(&eval->bheval, rbfv3_biharmonicseriesmax, _state);
    }
    
    /*
     * Recursively load coefficients into panels
     */
    rcopyvr(eval->n, w, &eval->wstoredorig, 0, _state);
    rbfv3_fastevaluatorloadcoeffsrec(eval, 0, _state);
    
    /*
     * Done
     */
    eval->isloaded = ae_true;
}


/*************************************************************************
Loads coefficients into fast evaluator (works with any NY>=1)

Depending  on  the  settings  specified  during evaluator creation (basis
function type and parameter), far field expansion can be  built  for  the
model.

INPUT PARAMETERS:
    Eval            -   fast evaluator to load coefficients to
    W               -   coeffs vector, array[NY,N]
*************************************************************************/
static void rbfv3_fastevaluatorloadcoeffs(rbf3fastevaluator* eval,
     /* Real    */ const ae_matrix* w,
     ae_state *_state)
{


    ae_assert(eval->ny<=w->rows, "FastEvaluatorLoadCoeffs: Eval.NY>Rows(W)", _state);
    ae_assert(ae_obj_array_get_length(&eval->panels)>0, "FastEvaluatorLoadCoeffs: Length(Panels)=0", _state);
    
    /*
     * Prepare problem-specific evaluator, if any
     */
    if( (eval->functype==1&&ae_fp_eq(eval->funcparam,0.0))&&eval->nx<=3 )
    {
        biharmonicevaluatorinit(&eval->bheval, rbfv3_biharmonicseriesmax, _state);
    }
    
    /*
     * Recursively load coefficients into panels
     */
    rmatrixcopy(eval->ny, eval->n, w, 0, 0, &eval->wstoredorig, 0, 0, _state);
    rbfv3_fastevaluatorloadcoeffsrec(eval, 0, _state);
    
    /*
     * Done
     */
    eval->isloaded = ae_true;
}


/*************************************************************************
Recursive subroutine that recomputes far field radii according  to  user-
specified accuracy requirements.

It should be called only for properly initialized models with coefficients
being loaded into them.
*************************************************************************/
static void rbfv3_fastevaluatorpushtolrec(rbf3fastevaluator* eval,
     ae_int_t treenodeidx,
     ae_bool dotrace,
     ae_int_t dbglevel,
     double maxcomputeerr,
     ae_state *_state)
{
    ae_frame _frame_block;
    double childerr;
    rbf3panel *panel;
    ae_smart_ptr _panel;
    ae_bool farfieldreconfigured;

    ae_frame_make(_state, &_frame_block);
    memset(&_panel, 0, sizeof(_panel));
    ae_smart_ptr_init(&_panel, (void**)&panel, _state, ae_true);

    ae_obj_array_get(&eval->panels, treenodeidx, &_panel, _state);
    
    /*
     * Reconfigure far field expansion, if present
     */
    if( panel->farfieldexpansion!=rbfv3_farfieldnone )
    {
        farfieldreconfigured = ae_false;
        
        /*
         * Far field expansions for a biharmonic kernel
         */
        if( panel->farfieldexpansion==rbfv3_farfieldbiharmonic )
        {
            bhpanelsetprec(&panel->bhexpansion, maxcomputeerr, _state);
            panel->farfielddistance = panel->bhexpansion.useatdistance;
            farfieldreconfigured = ae_true;
            if( dotrace )
            {
                tracespaces(dbglevel, _state);
                ae_trace("* n=%0d, |c|=%0.1e, r/R=%0.1f\n",
                    (int)(panel->idx1-panel->idx0),
                    (double)(panel->bhexpansion.maxsumabs),
                    (double)(panel->bhexpansion.useatdistance/(panel->bhexpansion.rmax+1.0E-50)));
            }
        }
        
        /*
         * Check that far field was recognized and processed
         */
        ae_assert(farfieldreconfigured, "RBF3: unexpected far field at PushTolRec()", _state);
    }
    
    /*
     * Non-leaf panel with two children
     */
    if( panel->paneltype==1 )
    {
        
        /*
         * Propagate relaxed error bounds to children. Instead of requiring it to be
         * MaxComputeErr/2 for each of the subpanels (the sum is MaxComputeErr, which
         * guarantees that the error bound is satisfied) we use larger value: MaxComputeErr/sqrt(2).
         *
         * The idea is that individual panel errors are uncorrelated, so we can achieve
         * better results by using relaxed error tolerances, but still having total
         * sum - on average - within prescribed bounds. Because error estimates are
         * overly cautious, it usually works fine.
         */
        childerr = rcase2(rbfv3_userelaxederrorestimates, maxcomputeerr/1.41, maxcomputeerr/(double)2, _state);
        rbfv3_fastevaluatorpushtolrec(eval, panel->childa, dotrace, dbglevel+1, childerr, _state);
        rbfv3_fastevaluatorpushtolrec(eval, panel->childb, dotrace, dbglevel+1, childerr, _state);
        ae_frame_leave(_state);
        return;
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Sets fast evaluator tolerance, reconfiguring far field  radii  all  along
the evaluation tree. Assumes that coefficients were loaded  with  one  of
FastEvaluatorLoadCoeffs()/FastEvaluatorLoadCoeffs1() calls.

INPUT PARAMETERS:
    Eval            -   fast evaluator with loaded coefficients
    MaxComputeErr   -   non-negative value which controls accuracy of the
                        model evaluation:
                        * =0 means that we try to perform exact evaluation
                             (subject to rounding errors) and do not try
                             to utilize far field expansions
                        * >0 means that far field expansions are used when
                             we can save some time. Maximum absolute error
                             of the model will be at most MaxComputeErr.
*************************************************************************/
static void rbfv3_fastevaluatorpushtol(rbf3fastevaluator* eval,
     double maxcomputeerr,
     ae_state *_state)
{
    ae_bool dotrace;


    ae_assert(ae_isfinite(maxcomputeerr, _state), "FastEvaluatorPushTol: MaxComputeErr is not finite", _state);
    ae_assert(ae_fp_greater_eq(maxcomputeerr,(double)(0)), "FastEvaluatorPushTol: MaxComputeErr<0", _state);
    ae_assert(eval->isloaded, "FastEvaluatorPushTol: coefficients are not loaded", _state);
    dotrace = ae_is_trace_enabled("RBF.DETAILED");
    if( dotrace )
    {
        ae_trace("----- recomputing fast eval tolerances, printing far field info ------------------------------------\n");
        ae_trace("> new tolerance is %0.3e\n",
            (double)(maxcomputeerr));
    }
    rbfv3_fastevaluatorpushtolrec(eval, 0, dotrace, 0, maxcomputeerr, _state);
}


/*************************************************************************
Updates Y[] with result of panel-to-panel interaction using straightforward
O(PANELSIZE^2) computation formula.
*************************************************************************/
static void rbfv3_fastevaluatorcomputepanel2panel(rbf3fastevaluator* eval,
     rbf3panel* dstpanel,
     rbf3panel* srcpanel,
     rbf3evaluatorbuffer* buf,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t ndstpts;
    ae_int_t i0;
    ae_int_t k;
    double distance0;
    ae_int_t srcsize;


    ae_assert(eval->ny==1, "RBF3Panel2Panel: ny>1", _state);
    ae_assert(dstpanel->paneltype==0&&dstpanel->idx1-dstpanel->idx0<=eval->maxpanelsize, "RBF3: integrity check 2735 failed", _state);
    ae_assert(srcpanel->paneltype==0&&srcpanel->idx1-srcpanel->idx0<=eval->maxpanelsize, "RBF3: integrity check 2736 failed", _state);
    ndstpts = dstpanel->idx1-dstpanel->idx0;
    srcsize = srcpanel->idx1-srcpanel->idx0;
    distance0 = 1.0E-50;
    if( eval->functype==1 )
    {
        distance0 = distance0+ae_sqr(eval->funcparam, _state);
    }
    ae_assert(eval->functype==1||eval->functype==2, "RBF3: integrity check 9132 failed", _state);
    for(i0=0; i0<=ndstpts-1; i0++)
    {
        rsetv(srcsize, distance0, &buf->funcbuf, _state);
        for(k=0; k<=eval->nx-1; k++)
        {
            rsetv(srcsize, dstpanel->xt.ptr.pp_double[k][i0], &buf->wrkbuf, _state);
            raddrv(srcsize, -1.0, &srcpanel->xt, k, &buf->wrkbuf, _state);
            rmuladdv(srcsize, &buf->wrkbuf, &buf->wrkbuf, &buf->funcbuf, _state);
        }
        if( eval->functype==1 )
        {
            
            /*
             * f=-sqrt(r^2+alpha^2), including f=-r as a special case
             */
            rsqrtv(srcsize, &buf->funcbuf, _state);
            rmulv(srcsize, -1.0, &buf->funcbuf, _state);
        }
        if( eval->functype==2 )
        {
            
            /*
             * f=r^2*ln(r)
             *
             * NOTE: FuncBuf[] is always positive due to small correction added,
             *       thus we have no need to handle ln(0) as a special case.
             */
            for(k=0; k<=srcsize-1; k++)
            {
                buf->funcbuf.ptr.p_double[k] = buf->funcbuf.ptr.p_double[k]*0.5*ae_log(buf->funcbuf.ptr.p_double[k], _state);
            }
        }
        y->ptr.p_double[dstpanel->ptidx.ptr.p_int[i0]] = y->ptr.p_double[dstpanel->ptidx.ptr.p_int[i0]]+rdotvr(srcsize, &buf->funcbuf, &srcpanel->wt, 0, _state);
    }
}


/*************************************************************************
Recursive evaluation function that recursively evaluates all source panels.

The plan is for each target panel to evaluate all source panels that may
contribute to model values at target points. This function evaluates all
sources, given some target.

This function has to be called with SourceTreeNode=0.
*************************************************************************/
static void rbfv3_fastevaluatorcomputeallrecurseonsources(rbf3fastevaluator* eval,
     rbf3panel* dstpanel,
     rbf3evaluatorbuffer* buf,
     ae_int_t sourcetreenode,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_frame _frame_block;
    rbf3panel *srcpanel;
    ae_smart_ptr _srcpanel;
    ae_int_t i0;
    ae_int_t k;
    double v;
    double vv;
    double dstpaneldistance;
    ae_bool farfieldprocessed;
    double c0;
    double c1;
    double c2;
    ae_int_t ndstpts;

    ae_frame_make(_state, &_frame_block);
    memset(&_srcpanel, 0, sizeof(_srcpanel));
    ae_smart_ptr_init(&_srcpanel, (void**)&srcpanel, _state, ae_true);

    ndstpts = dstpanel->idx1-dstpanel->idx0;
    ae_obj_array_get(&eval->panels, sourcetreenode, &_srcpanel, _state);
    
    /*
     * Analyze possibility of applying far field expansion
     */
    if( srcpanel->farfieldexpansion!=rbfv3_farfieldnone )
    {
        dstpaneldistance = (double)(0);
        for(k=0; k<=eval->nx-1; k++)
        {
            dstpaneldistance = dstpaneldistance+ae_sqr(dstpanel->clustercenter.ptr.p_double[k]-srcpanel->clustercenter.ptr.p_double[k], _state);
        }
        dstpaneldistance = ae_sqrt(dstpaneldistance, _state);
        dstpaneldistance = dstpaneldistance-dstpanel->clusterrad;
        if( ae_fp_greater(dstpaneldistance,srcpanel->farfielddistance) )
        {
            farfieldprocessed = ae_false;
            c0 = (double)(0);
            c1 = (double)(0);
            c2 = (double)(0);
            if( srcpanel->farfieldexpansion==rbfv3_farfieldbiharmonic )
            {
                for(i0=0; i0<=ndstpts-1; i0++)
                {
                    if( eval->nx>=1 )
                    {
                        c0 = dstpanel->xt.ptr.pp_double[0][i0];
                    }
                    if( eval->nx>=2 )
                    {
                        c1 = dstpanel->xt.ptr.pp_double[1][i0];
                    }
                    if( eval->nx>=3 )
                    {
                        c2 = dstpanel->xt.ptr.pp_double[2][i0];
                    }
                    bhpaneleval1(&srcpanel->bhexpansion, &eval->bheval, c0, c1, c2, &v, ae_false, &vv, _state);
                    y->ptr.p_double[dstpanel->ptidx.ptr.p_int[i0]] = y->ptr.p_double[dstpanel->ptidx.ptr.p_int[i0]]+v;
                }
                farfieldprocessed = ae_true;
            }
            ae_assert(farfieldprocessed, "RBF3: integrity check 4832 failed", _state);
            if( eval->usedebugcounters )
            {
                threadunsafeinc(&eval->dbgfield2panelcnt, _state);
            }
            ae_frame_leave(_state);
            return;
        }
    }
    
    /*
     * Far field expansion is not present, or we are too close to the expansion center.
     * Try recursive processing or handle leaf panel.
     */
    if( srcpanel->paneltype==1 )
    {
        rbfv3_fastevaluatorcomputeallrecurseonsources(eval, dstpanel, buf, srcpanel->childa, y, _state);
        rbfv3_fastevaluatorcomputeallrecurseonsources(eval, dstpanel, buf, srcpanel->childb, y, _state);
    }
    else
    {
        rbfv3_fastevaluatorcomputepanel2panel(eval, dstpanel, srcpanel, buf, y, _state);
        if( eval->usedebugcounters )
        {
            threadunsafeinc(&eval->dbgpanel2panelcnt, _state);
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Recursive evaluation function that recursively evaluates all target panels.

The plan is for each target panel to evaluate all source panels that may
contribute to model values at target points. This function evaluates all
targets, and it passes control to another function that evaluates all
sources.

This function has to be called with TargetTreeNode=0.
It can parallelize its computations.
*************************************************************************/
static void rbfv3_fastevaluatorcomputeallrecurseontargets(rbf3fastevaluator* eval,
     ae_int_t targettreenode,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    rbf3panel *dstpanel;
    ae_smart_ptr _dstpanel;

    ae_frame_make(_state, &_frame_block);
    memset(&_dstpanel, 0, sizeof(_dstpanel));
    ae_smart_ptr_init(&_dstpanel, (void**)&dstpanel, _state, ae_true);

    
    /*
     * Do we need parallel execution?
     * Checked only at the root.
     */
    if( (targettreenode==0&&ae_fp_greater(rmul2((double)(eval->n), (double)(eval->n), _state),smpactivationlevel(_state)))&&ae_obj_array_get_length(&eval->panels)>1 )
    {
        if( _trypexec_rbfv3_fastevaluatorcomputeallrecurseontargets(eval,targettreenode,y, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            ae_frame_leave(_state);
            return;
        }
    }
    
    /*
     * Evaluate destination panel
     */
    ae_obj_array_get(&eval->panels, targettreenode, &_dstpanel, _state);
    if( dstpanel->paneltype==1 )
    {
        ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_fp_greater(rmul2((double)(eval->n), (double)(dstpanel->idx1-dstpanel->idx0), _state),spawnlevel(_state)), _state);
        _spawn_rbfv3_fastevaluatorcomputeallrecurseontargets(eval, dstpanel->childa, y, _child_tasks, _smp_enabled, _state);
        rbfv3_fastevaluatorcomputeallrecurseontargets(eval, dstpanel->childb, y, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        ae_frame_leave(_state);
        return;
    }
    ae_assert(dstpanel->paneltype==0, "RBF3: integrity check 2735 failed", _state);
    
    /*
     * Recurse on sources.
     * Use evaluator buffer stored in target panel for temporaries.
     */
    rbfv3_fastevaluatorcomputeallrecurseonsources(eval, dstpanel, &dstpanel->tgtbuf, 0, y, _state);
    threadunsafeinc(&eval->dbgpanelscnt, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_rbfv3_fastevaluatorcomputeallrecurseontargets(rbf3fastevaluator* eval,
    ae_int_t targettreenode,
    /* Real    */ ae_vector* y,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        rbfv3_fastevaluatorcomputeallrecurseontargets(eval,targettreenode,y, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_rbfv3_fastevaluatorcomputeallrecurseontargets;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = eval;
        _task->data.parameters[1].value.ival = targettreenode;
        _task->data.parameters[2].value.val = y;
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
void _task_rbfv3_fastevaluatorcomputeallrecurseontargets(ae_task_data *_data, ae_state *_state)
{
    rbf3fastevaluator* eval;
    ae_int_t targettreenode;
    ae_vector* y;
    eval = (rbf3fastevaluator*)_data->parameters[0].value.val;
    targettreenode = _data->parameters[1].value.ival;
    y = (ae_vector*)_data->parameters[2].value.val;
   ae_state_set_flags(_state, _data->flags);
   rbfv3_fastevaluatorcomputeallrecurseontargets(eval,targettreenode,y, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_rbfv3_fastevaluatorcomputeallrecurseontargets(rbf3fastevaluator* eval,
    ae_int_t targettreenode,
    /* Real    */ ae_vector* y,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_rbfv3_fastevaluatorcomputeallrecurseontargets;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = eval;
    _task->data.parameters[1].value.ival = targettreenode;
    _task->data.parameters[2].value.val = y;
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
Performs batch evaluation at each node (assuming NY=1).
*************************************************************************/
static void rbfv3_fastevaluatorcomputeall(rbf3fastevaluator* eval,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{


    ae_assert(eval->ny==1, "FastEvaluatorComputeAll: Eval.NY<>1", _state);
    rsetallocv(eval->n, 0.0, y, _state);
    eval->dbgpanel2panelcnt = 0;
    eval->dbgfield2panelcnt = 0;
    eval->dbgpanelscnt = 0;
    rbfv3_fastevaluatorcomputeallrecurseontargets(eval, 0, y, _state);
}


/*************************************************************************
Recursion on source panels for batch evaluation

NOTE: this function is thread-safe, the same Eval object can be  used  by
      multiple threads calling ComputeBatch() concurrently. Although  for
      technical reasons Eval is accepted as non-const parameter,  thread-
      safety is guaranteed.

INPUT PARAMETERS:
    Eval        -   evaluator   
    X           -   array[N,NX], dataset
    TgtIdx      -   target row index
    SourceTreeNode- index of the current panel in the tree; must be zero.
    UseFarFields-   use far fields to accelerate computations - or use
                    slow but exact formulae

OUTPUT PARAMETERS:
    Y           -   array[NY,N], column TgtIdx is updated
*************************************************************************/
static void rbfv3_fastevaluatorcomputebatchrecurseonsources(rbf3fastevaluator* eval,
     /* Real    */ const ae_matrix* x,
     ae_int_t tgtidx,
     ae_int_t sourcetreenode,
     ae_bool usefarfields,
     rbf3evaluatorbuffer* buf,
     /* Real    */ ae_matrix* y,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    rbf3panel *srcpanel;
    ae_smart_ptr _srcpanel;
    ae_int_t srcsize;
    ae_int_t k;
    ae_int_t d;
    double distance0;
    double paneldistance;
    ae_bool farfieldprocessed;
    double c0;
    double c1;
    double c2;
    double v;
    double vv;

    ae_frame_make(_state, &_frame_block);
    memset(&_srcpanel, 0, sizeof(_srcpanel));
    ae_smart_ptr_init(&_srcpanel, (void**)&srcpanel, _state, ae_true);

    ae_obj_array_get(&eval->panels, sourcetreenode, &_srcpanel, _state);
    
    /*
     * Analyze possibility of applying far field expansion
     */
    if( srcpanel->farfieldexpansion!=rbfv3_farfieldnone&&usefarfields )
    {
        paneldistance = (double)(0);
        for(k=0; k<=eval->nx-1; k++)
        {
            paneldistance = paneldistance+ae_sqr(x->ptr.pp_double[tgtidx][k]-srcpanel->clustercenter.ptr.p_double[k], _state);
        }
        paneldistance = ae_sqrt(paneldistance, _state);
        if( ae_fp_greater(paneldistance,srcpanel->farfielddistance) )
        {
            farfieldprocessed = ae_false;
            c0 = (double)(0);
            c1 = (double)(0);
            c2 = (double)(0);
            if( srcpanel->farfieldexpansion==rbfv3_farfieldbiharmonic )
            {
                if( eval->nx>=1 )
                {
                    c0 = x->ptr.pp_double[tgtidx][0];
                }
                if( eval->nx>=2 )
                {
                    c1 = x->ptr.pp_double[tgtidx][1];
                }
                if( eval->nx>=3 )
                {
                    c2 = x->ptr.pp_double[tgtidx][2];
                }
                if( eval->ny==1 )
                {
                    bhpaneleval1(&srcpanel->bhexpansion, &eval->bheval, c0, c1, c2, &v, ae_false, &vv, _state);
                    y->ptr.pp_double[0][tgtidx] = y->ptr.pp_double[0][tgtidx]+v;
                }
                else
                {
                    bhpaneleval(&srcpanel->bhexpansion, &eval->bheval, c0, c1, c2, &buf->y, ae_false, &vv, _state);
                    for(k=0; k<=eval->ny-1; k++)
                    {
                        y->ptr.pp_double[k][tgtidx] = y->ptr.pp_double[k][tgtidx]+buf->y.ptr.p_double[k];
                    }
                }
                farfieldprocessed = ae_true;
            }
            ae_assert(farfieldprocessed, "RBF3: integrity check 4832 failed", _state);
            if( eval->usedebugcounters )
            {
                threadunsafeinc(&eval->dbgfield2panelcnt, _state);
            }
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            ae_frame_leave(_state);
            return;
        }
    }
    
    /*
     * Perform recursive processing if needed
     */
    if( srcpanel->paneltype==1 )
    {
        rbfv3_fastevaluatorcomputebatchrecurseonsources(eval, x, tgtidx, srcpanel->childa, usefarfields, buf, y, _state);
        rbfv3_fastevaluatorcomputebatchrecurseonsources(eval, x, tgtidx, srcpanel->childb, usefarfields, buf, y, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        ae_frame_leave(_state);
        return;
    }
    ae_assert(srcpanel->paneltype==0&&srcpanel->idx1-srcpanel->idx0<=eval->maxpanelsize, "RBF3: integrity check 2735 failed", _state);
    
    /*
     * Obtain evaluation buffer and process panel.
     * Recycle the buffer later.
     */
    ae_assert(eval->functype==1||eval->functype==2, "RBF3: integrity check 1132 failed", _state);
    srcsize = srcpanel->idx1-srcpanel->idx0;
    distance0 = 1.0E-50;
    if( eval->functype==1 )
    {
        distance0 = distance0+ae_sqr(eval->funcparam, _state);
    }
    rsetv(srcsize, distance0, &buf->funcbuf, _state);
    for(k=0; k<=eval->nx-1; k++)
    {
        rsetv(srcsize, x->ptr.pp_double[tgtidx][k], &buf->wrkbuf, _state);
        raddrv(srcsize, -1.0, &srcpanel->xt, k, &buf->wrkbuf, _state);
        rmuladdv(srcsize, &buf->wrkbuf, &buf->wrkbuf, &buf->funcbuf, _state);
    }
    if( eval->functype==1 )
    {
        
        /*
         * f=-sqrt(r^2+alpha^2), including f=-r as a special case
         */
        rsqrtv(srcsize, &buf->funcbuf, _state);
        rmulv(srcsize, -1.0, &buf->funcbuf, _state);
    }
    if( eval->functype==2 )
    {
        
        /*
         * f=r^2*ln(r)
         *
         * NOTE: FuncBuf[] is always positive due to small correction added,
         *       thus we have no need to handle ln(0) as a special case.
         */
        for(k=0; k<=srcsize-1; k++)
        {
            buf->funcbuf.ptr.p_double[k] = buf->funcbuf.ptr.p_double[k]*0.5*ae_log(buf->funcbuf.ptr.p_double[k], _state);
        }
    }
    for(d=0; d<=eval->ny-1; d++)
    {
        y->ptr.pp_double[d][tgtidx] = y->ptr.pp_double[d][tgtidx]+rdotvr(srcsize, &buf->funcbuf, &srcpanel->wt, d, _state);
    }
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_rbfv3_fastevaluatorcomputebatchrecurseonsources(rbf3fastevaluator* eval,
    /* Real    */ const ae_matrix* x,
    ae_int_t tgtidx,
    ae_int_t sourcetreenode,
    ae_bool usefarfields,
    rbf3evaluatorbuffer* buf,
    /* Real    */ ae_matrix* y,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        rbfv3_fastevaluatorcomputebatchrecurseonsources(eval,x,tgtidx,sourcetreenode,usefarfields,buf,y, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_rbfv3_fastevaluatorcomputebatchrecurseonsources;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = eval;
        _task->data.parameters[1].value.const_val = x;
        _task->data.parameters[2].value.ival = tgtidx;
        _task->data.parameters[3].value.ival = sourcetreenode;
        _task->data.parameters[4].value.bval = usefarfields;
        _task->data.parameters[5].value.val = buf;
        _task->data.parameters[6].value.val = y;
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
void _task_rbfv3_fastevaluatorcomputebatchrecurseonsources(ae_task_data *_data, ae_state *_state)
{
    rbf3fastevaluator* eval;
    const ae_matrix* x;
    ae_int_t tgtidx;
    ae_int_t sourcetreenode;
    ae_bool usefarfields;
    rbf3evaluatorbuffer* buf;
    ae_matrix* y;
    eval = (rbf3fastevaluator*)_data->parameters[0].value.val;
    x = (const ae_matrix*)_data->parameters[1].value.const_val;
    tgtidx = _data->parameters[2].value.ival;
    sourcetreenode = _data->parameters[3].value.ival;
    usefarfields = _data->parameters[4].value.bval;
    buf = (rbf3evaluatorbuffer*)_data->parameters[5].value.val;
    y = (ae_matrix*)_data->parameters[6].value.val;
   ae_state_set_flags(_state, _data->flags);
   rbfv3_fastevaluatorcomputebatchrecurseonsources(eval,x,tgtidx,sourcetreenode,usefarfields,buf,y, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_rbfv3_fastevaluatorcomputebatchrecurseonsources(rbf3fastevaluator* eval,
    /* Real    */ const ae_matrix* x,
    ae_int_t tgtidx,
    ae_int_t sourcetreenode,
    ae_bool usefarfields,
    rbf3evaluatorbuffer* buf,
    /* Real    */ ae_matrix* y,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_rbfv3_fastevaluatorcomputebatchrecurseonsources;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = eval;
    _task->data.parameters[1].value.const_val = x;
    _task->data.parameters[2].value.ival = tgtidx;
    _task->data.parameters[3].value.ival = sourcetreenode;
    _task->data.parameters[4].value.bval = usefarfields;
    _task->data.parameters[5].value.val = buf;
    _task->data.parameters[6].value.val = y;
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
Recursion on targets for batch evaluation, splits interval into smaller
ones.

NOTE: this function is thread-safe, the same Eval object can be  used  by
      multiple threads calling ComputeBatch() concurrently. Although  for
      technical reasons Eval is accepted as non-const parameter,  thread-
      safety is guaranteed.

INPUT PARAMETERS:
    Eval        -   evaluator   
    X           -   array[N,NX], dataset
    Idx0,Idx1   -   target range
    IsRootCall  -   must be True
    UseFarFields-   use far fields to accelerate computations - or use
                    slow but exact formulae

OUTPUT PARAMETERS:
    Y           -   array[NY,N], computed values
*************************************************************************/
static void rbfv3_fastevaluatorcomputebatchrecurseontargets(rbf3fastevaluator* eval,
     /* Real    */ const ae_matrix* x,
     ae_int_t idx0,
     ae_int_t idx1,
     ae_bool isrootcall,
     ae_bool usefarfields,
     /* Real    */ ae_matrix* y,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t i;
    ae_int_t size0;
    ae_int_t size1;
    rbf3evaluatorbuffer *buf;
    ae_smart_ptr _buf;

    ae_frame_make(_state, &_frame_block);
    memset(&_buf, 0, sizeof(_buf));
    ae_smart_ptr_init(&_buf, (void**)&buf, _state, ae_true);

    
    /*
     * Do we need parallel execution?
     * Checked only at the root.
     */
    if( (isrootcall&&idx1-idx0>rbfv3_maxcomputebatchsize)&&ae_fp_greater(rmul2((double)(eval->n), (double)(idx1-idx0), _state),smpactivationlevel(_state)) )
    {
        if( _trypexec_rbfv3_fastevaluatorcomputebatchrecurseontargets(eval,x,idx0,idx1,isrootcall,usefarfields,y, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            ae_frame_leave(_state);
            return;
        }
    }
    
    /*
     * Split targets
     */
    if( idx1-idx0>rbfv3_maxcomputebatchsize )
    {
        ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_fp_greater(rmul2((double)(eval->n), (double)(idx1-idx0), _state),spawnlevel(_state)), _state);
        tiledsplit(idx1-idx0, rbfv3_maxcomputebatchsize, &size0, &size1, _state);
        _spawn_rbfv3_fastevaluatorcomputebatchrecurseontargets(eval, x, idx0, idx0+size0, ae_false, usefarfields, y, _child_tasks, _smp_enabled, _state);
        rbfv3_fastevaluatorcomputebatchrecurseontargets(eval, x, idx0+size0, idx1, ae_false, usefarfields, y, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Run recursion on sources
     */
    ae_shared_pool_retrieve(&eval->bufferpool, &_buf, _state);
    for(i=idx0; i<=idx1-1; i++)
    {
        rbfv3_fastevaluatorcomputebatchrecurseonsources(eval, x, i, 0, usefarfields, buf, y, _state);
    }
    ae_shared_pool_recycle(&eval->bufferpool, &_buf, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_rbfv3_fastevaluatorcomputebatchrecurseontargets(rbf3fastevaluator* eval,
    /* Real    */ const ae_matrix* x,
    ae_int_t idx0,
    ae_int_t idx1,
    ae_bool isrootcall,
    ae_bool usefarfields,
    /* Real    */ ae_matrix* y,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        rbfv3_fastevaluatorcomputebatchrecurseontargets(eval,x,idx0,idx1,isrootcall,usefarfields,y, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_rbfv3_fastevaluatorcomputebatchrecurseontargets;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = eval;
        _task->data.parameters[1].value.const_val = x;
        _task->data.parameters[2].value.ival = idx0;
        _task->data.parameters[3].value.ival = idx1;
        _task->data.parameters[4].value.bval = isrootcall;
        _task->data.parameters[5].value.bval = usefarfields;
        _task->data.parameters[6].value.val = y;
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
void _task_rbfv3_fastevaluatorcomputebatchrecurseontargets(ae_task_data *_data, ae_state *_state)
{
    rbf3fastevaluator* eval;
    const ae_matrix* x;
    ae_int_t idx0;
    ae_int_t idx1;
    ae_bool isrootcall;
    ae_bool usefarfields;
    ae_matrix* y;
    eval = (rbf3fastevaluator*)_data->parameters[0].value.val;
    x = (const ae_matrix*)_data->parameters[1].value.const_val;
    idx0 = _data->parameters[2].value.ival;
    idx1 = _data->parameters[3].value.ival;
    isrootcall = _data->parameters[4].value.bval;
    usefarfields = _data->parameters[5].value.bval;
    y = (ae_matrix*)_data->parameters[6].value.val;
   ae_state_set_flags(_state, _data->flags);
   rbfv3_fastevaluatorcomputebatchrecurseontargets(eval,x,idx0,idx1,isrootcall,usefarfields,y, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_rbfv3_fastevaluatorcomputebatchrecurseontargets(rbf3fastevaluator* eval,
    /* Real    */ const ae_matrix* x,
    ae_int_t idx0,
    ae_int_t idx1,
    ae_bool isrootcall,
    ae_bool usefarfields,
    /* Real    */ ae_matrix* y,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_rbfv3_fastevaluatorcomputebatchrecurseontargets;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = eval;
    _task->data.parameters[1].value.const_val = x;
    _task->data.parameters[2].value.ival = idx0;
    _task->data.parameters[3].value.ival = idx1;
    _task->data.parameters[4].value.bval = isrootcall;
    _task->data.parameters[5].value.bval = usefarfields;
    _task->data.parameters[6].value.val = y;
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
Performs batch evaluation at user-specified points which does not have to
coincide with nodes. Assumes NY=1.

NOTE: this function is thread-safe, the same Eval object can be  used  by
      multiple threads calling ComputeBatch() concurrently. Although  for
      technical reasons Eval is accepted as non-const parameter,  thread-
      safety is guaranteed.

INPUT PARAMETERS:
    Eval        -   evaluator
    X           -   array[N,NX], dataset
    N           -   rows count
    UseFarFields-   use far fields to accelerate computations - or use
                    slow but exact formulae

OUTPUT PARAMETERS:
    Y           -   array[NY,N], computed values.
                    The array is reallocated if needed.
*************************************************************************/
static void rbfv3_fastevaluatorcomputebatch(rbf3fastevaluator* eval,
     /* Real    */ const ae_matrix* x,
     ae_int_t n,
     ae_bool usefarfields,
     /* Real    */ ae_matrix* y,
     ae_state *_state)
{


    rsetallocm(eval->ny, n, 0.0, y, _state);
    rbfv3_fastevaluatorcomputebatchrecurseontargets(eval, x, 0, n, ae_true, usefarfields, y, _state);
}


/*************************************************************************
Creates fast evaluation structures after initialization of the model

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_createfastevaluator(rbfv3model* model, ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t offs;
    ae_int_t ontheflystorage;
    ae_int_t i;
    ae_int_t j;
    ae_int_t nchunks;
    ae_int_t srcoffs;
    ae_int_t dstoffs;
    ae_int_t curlen;
    ae_matrix xx;
    ae_matrix ct;

    ae_frame_make(_state, &_frame_block);
    memset(&xx, 0, sizeof(xx));
    memset(&ct, 0, sizeof(ct));
    ae_matrix_init(&xx, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&ct, 0, 0, DT_REAL, _state, ae_true);

    
    /*
     * Extract dataset into separate matrix
     */
    rallocm(model->nc, model->nx, &xx, _state);
    rallocm(model->ny, model->nc, &ct, _state);
    offs = 0;
    for(i=0; i<=model->nc-1; i++)
    {
        for(j=0; j<=model->nx-1; j++)
        {
            xx.ptr.pp_double[i][j] = model->cw.ptr.p_double[offs+j];
        }
        offs = offs+model->nx;
        for(j=0; j<=model->ny-1; j++)
        {
            ct.ptr.pp_double[j][i] = model->cw.ptr.p_double[offs+j];
        }
        offs = offs+model->ny;
    }
    
    /*
     * Prepare fast evaluator
     */
    rbfv3_fastevaluatorinit(&model->fasteval, &xx, model->nc, model->nx, model->ny, rbfv3_defaultmaxpanelsize, model->bftype, model->bfparam, ae_false, _state);
    rbfv3_fastevaluatorloadcoeffs(&model->fasteval, &ct, _state);
    rbfv3_fastevaluatorpushtol(&model->fasteval, rbfv3_defaultfastevaltol, _state);
    
    /*
     * Setup model matrix structure
     */
    ontheflystorage = 1;
    rbfv3_modelmatrixinit(&xx, model->nc, model->nx, model->bftype, model->bfparam, ontheflystorage, &model->evaluator, _state);
    
    /*
     * Store model coefficients in the efficient chunked format (chunk size is aligned with that
     * of the Model.Evaluator).
     */
    ae_assert(model->evaluator.chunksize>=1, "RBFV3: integrity check 3535 failed", _state);
    nchunks = idivup(model->nc, model->evaluator.chunksize, _state);
    rsetallocm(nchunks*model->ny, model->evaluator.chunksize, 0.0, &model->wchunked, _state);
    srcoffs = 0;
    dstoffs = 0;
    while(srcoffs<model->nc)
    {
        curlen = ae_minint(model->evaluator.chunksize, model->nc-srcoffs, _state);
        for(i=0; i<=curlen-1; i++)
        {
            for(j=0; j<=model->ny-1; j++)
            {
                model->wchunked.ptr.pp_double[dstoffs+j][i] = model->cw.ptr.p_double[(srcoffs+i)*(model->nx+model->ny)+model->nx+j];
            }
        }
        srcoffs = srcoffs+curlen;
        dstoffs = dstoffs+model->ny;
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Recursive worker function for gridded calculation

  -- ALGLIB --
     Copyright 01.05.2022 by Bochkanov Sergey
*************************************************************************/
static void rbfv3_gridcalcrec(const rbfv3model* s,
     ae_int_t simdwidth,
     ae_int_t tileidx0,
     ae_int_t tileidx1,
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
     ae_shared_pool* calcpool,
     ae_bool isrootcall,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t ny;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t dstoffs;
    ae_int_t l;
    rbfv3calcbuffer *buf;
    ae_smart_ptr _buf;
    double problemcost;
    ae_int_t tileidxm;
    ae_int_t k0;
    ae_int_t k1;
    ae_int_t k2;
    ae_int_t r0a;
    ae_int_t r0b;
    ae_int_t r1a;
    ae_int_t r1b;
    ae_int_t r2a;
    ae_int_t r2b;

    ae_frame_make(_state, &_frame_block);
    memset(&_buf, 0, sizeof(_buf));
    ae_smart_ptr_init(&_buf, (void**)&buf, _state, ae_true);

    ny = s->ny;
    
    /*
     * Try parallelism if needed; then perform parallel subdivision:
     * * make all dimensions either (a) multiples of SIMDWidth or (b) less than SIMDWidth by
     *   splitting small chunks from tails
     * * after that iteratively subdivide largest side of the grid (one having largest length,
     *   not largest points count) until we have a chunk with nodes count not greater than SIMDWidth
     */
    problemcost = rmul2((double)(tileidx1-tileidx0), (double)(s->nc), _state);
    problemcost = problemcost*rmul4((double)(ae_minint(n0, simdwidth, _state)), (double)(ae_minint(n1, simdwidth, _state)), (double)(ae_minint(n2, simdwidth, _state)), (double)(ae_minint(n3, simdwidth, _state)), _state);
    if( isrootcall&&ae_fp_greater_eq(problemcost,smpactivationlevel(_state)) )
    {
        if( _trypexec_rbfv3_gridcalcrec(s,simdwidth,tileidx0,tileidx1,x0,n0,x1,n1,x2,n2,x3,n3,flagy,sparsey,y,calcpool,isrootcall, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            ae_frame_leave(_state);
            return;
        }
    }
    if( ae_fp_greater_eq(problemcost,spawnlevel(_state))&&tileidx1-tileidx0>=2 )
    {
        ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_true, _state);
    }
    if( tileidx1-tileidx0>=2 )
    {
        tileidxm = tileidx0+idivup(tileidx1-tileidx0, 2, _state);
        _spawn_rbfv3_gridcalcrec(s, simdwidth, tileidx0, tileidxm, x0, n0, x1, n1, x2, n2, x3, n3, flagy, sparsey, y, calcpool, ae_false, _child_tasks, _smp_enabled, _state);
        rbfv3_gridcalcrec(s, simdwidth, tileidxm, tileidx1, x0, n0, x1, n1, x2, n2, x3, n3, flagy, sparsey, y, calcpool, ae_false, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Handle basecase
     */
    k = tileidx0;
    k0 = k%idivup(n0, simdwidth, _state);
    k = k/idivup(n0, simdwidth, _state);
    k1 = k%idivup(n1, simdwidth, _state);
    k = k/idivup(n1, simdwidth, _state);
    k2 = k%idivup(n2, simdwidth, _state);
    k = k/idivup(n2, simdwidth, _state);
    k = k/idivup(n3, simdwidth, _state);
    ae_assert(k==0, "RBFV3: integrity check 7350 failed", _state);
    r0a = k0*simdwidth;
    r0b = ae_minint(r0a+simdwidth, n0, _state);
    r1a = k1*simdwidth;
    r1b = ae_minint(r1a+simdwidth, n1, _state);
    r2a = k2*simdwidth;
    r2b = ae_minint(r2a+simdwidth, n2, _state);
    ae_shared_pool_retrieve(calcpool, &_buf, _state);
    for(i=r0a; i<=r0b-1; i++)
    {
        for(j=r1a; j<=r1b-1; j++)
        {
            for(k=r2a; k<=r2b-1; k++)
            {
                dstoffs = i+j*n0+k*n0*n1;
                if( sparsey&&!flagy->ptr.p_bool[dstoffs] )
                {
                    for(l=0; l<=ny-1; l++)
                    {
                        y->ptr.p_double[l+ny*dstoffs] = (double)(0);
                    }
                    continue;
                }
                buf->xg.ptr.p_double[0] = x0->ptr.p_double[i];
                buf->xg.ptr.p_double[1] = x1->ptr.p_double[j];
                buf->xg.ptr.p_double[2] = x2->ptr.p_double[k];
                rbfv3tscalcbuf(s, buf, &buf->xg, &buf->yg, _state);
                for(l=0; l<=ny-1; l++)
                {
                    y->ptr.p_double[l+ny*dstoffs] = buf->yg.ptr.p_double[l];
                }
            }
        }
    }
    ae_shared_pool_recycle(calcpool, &_buf, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_rbfv3_gridcalcrec(const rbfv3model* s,
    ae_int_t simdwidth,
    ae_int_t tileidx0,
    ae_int_t tileidx1,
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
    ae_shared_pool* calcpool,
    ae_bool isrootcall,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        rbfv3_gridcalcrec(s,simdwidth,tileidx0,tileidx1,x0,n0,x1,n1,x2,n2,x3,n3,flagy,sparsey,y,calcpool,isrootcall, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_rbfv3_gridcalcrec;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.const_val = s;
        _task->data.parameters[1].value.ival = simdwidth;
        _task->data.parameters[2].value.ival = tileidx0;
        _task->data.parameters[3].value.ival = tileidx1;
        _task->data.parameters[4].value.const_val = x0;
        _task->data.parameters[5].value.ival = n0;
        _task->data.parameters[6].value.const_val = x1;
        _task->data.parameters[7].value.ival = n1;
        _task->data.parameters[8].value.const_val = x2;
        _task->data.parameters[9].value.ival = n2;
        _task->data.parameters[10].value.const_val = x3;
        _task->data.parameters[11].value.ival = n3;
        _task->data.parameters[12].value.const_val = flagy;
        _task->data.parameters[13].value.bval = sparsey;
        _task->data.parameters[14].value.val = y;
        _task->data.parameters[15].value.val = calcpool;
        _task->data.parameters[16].value.bval = isrootcall;
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
void _task_rbfv3_gridcalcrec(ae_task_data *_data, ae_state *_state)
{
    const rbfv3model* s;
    ae_int_t simdwidth;
    ae_int_t tileidx0;
    ae_int_t tileidx1;
    const ae_vector* x0;
    ae_int_t n0;
    const ae_vector* x1;
    ae_int_t n1;
    const ae_vector* x2;
    ae_int_t n2;
    const ae_vector* x3;
    ae_int_t n3;
    const ae_vector* flagy;
    ae_bool sparsey;
    ae_vector* y;
    ae_shared_pool* calcpool;
    ae_bool isrootcall;
    s = (const rbfv3model*)_data->parameters[0].value.const_val;
    simdwidth = _data->parameters[1].value.ival;
    tileidx0 = _data->parameters[2].value.ival;
    tileidx1 = _data->parameters[3].value.ival;
    x0 = (const ae_vector*)_data->parameters[4].value.const_val;
    n0 = _data->parameters[5].value.ival;
    x1 = (const ae_vector*)_data->parameters[6].value.const_val;
    n1 = _data->parameters[7].value.ival;
    x2 = (const ae_vector*)_data->parameters[8].value.const_val;
    n2 = _data->parameters[9].value.ival;
    x3 = (const ae_vector*)_data->parameters[10].value.const_val;
    n3 = _data->parameters[11].value.ival;
    flagy = (const ae_vector*)_data->parameters[12].value.const_val;
    sparsey = _data->parameters[13].value.bval;
    y = (ae_vector*)_data->parameters[14].value.val;
    calcpool = (ae_shared_pool*)_data->parameters[15].value.val;
    isrootcall = _data->parameters[16].value.bval;
   ae_state_set_flags(_state, _data->flags);
   rbfv3_gridcalcrec(s,simdwidth,tileidx0,tileidx1,x0,n0,x1,n1,x2,n2,x3,n3,flagy,sparsey,y,calcpool,isrootcall, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_rbfv3_gridcalcrec(const rbfv3model* s,
    ae_int_t simdwidth,
    ae_int_t tileidx0,
    ae_int_t tileidx1,
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
    ae_shared_pool* calcpool,
    ae_bool isrootcall,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_rbfv3_gridcalcrec;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.const_val = s;
    _task->data.parameters[1].value.ival = simdwidth;
    _task->data.parameters[2].value.ival = tileidx0;
    _task->data.parameters[3].value.ival = tileidx1;
    _task->data.parameters[4].value.const_val = x0;
    _task->data.parameters[5].value.ival = n0;
    _task->data.parameters[6].value.const_val = x1;
    _task->data.parameters[7].value.ival = n1;
    _task->data.parameters[8].value.const_val = x2;
    _task->data.parameters[9].value.ival = n2;
    _task->data.parameters[10].value.const_val = x3;
    _task->data.parameters[11].value.ival = n3;
    _task->data.parameters[12].value.const_val = flagy;
    _task->data.parameters[13].value.bval = sparsey;
    _task->data.parameters[14].value.val = y;
    _task->data.parameters[15].value.val = calcpool;
    _task->data.parameters[16].value.bval = isrootcall;
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
This function fills RBF model by zeros, also cleans up debug fields.

  -- ALGLIB --
     Copyright 12.12.2021 by Bochkanov Sergey
*************************************************************************/
static void rbfv3_zerofill(rbfv3model* s,
     ae_int_t nx,
     ae_int_t ny,
     ae_state *_state)
{


    s->bftype = 0;
    s->bfparam = (double)(0);
    s->nc = 0;
    rsetallocv(nx, 1.0, &s->s, _state);
    rsetallocm(ny, nx+1, 0.0, &s->v, _state);
}


/*************************************************************************
Reallocates calcBuf if necessary, reuses previously allocated space if
possible.

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_allocatecalcbuffer(const rbfv3model* s,
     rbfv3calcbuffer* buf,
     ae_state *_state)
{


    if( buf->x.cnt<s->nx )
    {
        ae_vector_set_length(&buf->x, s->nx, _state);
    }
    if( buf->x123.cnt<s->nx )
    {
        ae_vector_set_length(&buf->x123, s->nx, _state);
    }
    if( buf->y123.cnt<s->ny )
    {
        ae_vector_set_length(&buf->y123, s->ny, _state);
    }
    if( buf->xg.cnt<4 )
    {
        ae_vector_set_length(&buf->xg, 4, _state);
    }
    if( buf->yg.cnt<s->ny )
    {
        ae_vector_set_length(&buf->yg, s->ny, _state);
    }
}


/*************************************************************************
Recursive function that merges points, used by PreprocessDataset()

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_preprocessdatasetrec(/* Real    */ ae_matrix* xbuf,
     /* Real    */ ae_matrix* ybuf,
     /* Integer */ ae_vector* initidx,
     ae_int_t wrk0,
     ae_int_t wrk1,
     ae_int_t nx,
     ae_int_t ny,
     double mergetol,
     /* Real    */ ae_vector* tmpboxmin,
     /* Real    */ ae_vector* tmpboxmax,
     /* Real    */ ae_matrix* xout,
     /* Real    */ ae_matrix* yout,
     /* Integer */ ae_vector* raw2wrkmap,
     /* Integer */ ae_vector* wrk2rawmap,
     ae_int_t* nout,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k0;
    ae_int_t k1;
    ae_int_t largestdim;
    double splitval;


    if( wrk1<=wrk0 )
    {
        return;
    }
    
    /*
     * Analyze current working set
     */
    rallocv(nx, tmpboxmin, _state);
    rallocv(nx, tmpboxmax, _state);
    rcopyrv(nx, xbuf, wrk0, tmpboxmin, _state);
    rcopyrv(nx, xbuf, wrk0, tmpboxmax, _state);
    for(i=wrk0+1; i<=wrk1-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            tmpboxmin->ptr.p_double[j] = ae_minreal(tmpboxmin->ptr.p_double[j], xbuf->ptr.pp_double[i][j], _state);
            tmpboxmax->ptr.p_double[j] = ae_maxreal(tmpboxmax->ptr.p_double[j], xbuf->ptr.pp_double[i][j], _state);
        }
    }
    largestdim = 0;
    for(j=1; j<=nx-1; j++)
    {
        if( ae_fp_greater(tmpboxmax->ptr.p_double[j]-tmpboxmin->ptr.p_double[j],tmpboxmax->ptr.p_double[largestdim]-tmpboxmin->ptr.p_double[largestdim]) )
        {
            largestdim = j;
        }
    }
    
    /*
     * Handle basecase or perform recursive split
     */
    if( wrk1-wrk0==1||ae_fp_less(tmpboxmax->ptr.p_double[largestdim]-tmpboxmin->ptr.p_double[largestdim],mergetol*rmax3(rmaxabsv(nx, tmpboxmax, _state), rmaxabsv(nx, tmpboxmin, _state), (double)(1), _state)) )
    {
        
        /*
         * Merge all points, output
         */
        rsetr(nx, 0.0, xout, *nout, _state);
        rsetr(ny, 0.0, yout, *nout, _state);
        for(i=wrk0; i<=wrk1-1; i++)
        {
            raddrr(nx, (double)1/(double)(wrk1-wrk0), xbuf, i, xout, *nout, _state);
            raddrr(ny, (double)1/(double)(wrk1-wrk0), ybuf, i, yout, *nout, _state);
            raw2wrkmap->ptr.p_int[initidx->ptr.p_int[i]] = *nout;
        }
        wrk2rawmap->ptr.p_int[*nout] = initidx->ptr.p_int[wrk0];
        *nout = *nout+1;
    }
    else
    {
        
        /*
         * Perform recursive split along largest axis
         */
        splitval = 0.5*(tmpboxmax->ptr.p_double[largestdim]+tmpboxmin->ptr.p_double[largestdim]);
        k0 = wrk0;
        k1 = wrk1-1;
        while(k0<=k1)
        {
            if( ae_fp_less_eq(xbuf->ptr.pp_double[k0][largestdim],splitval) )
            {
                k0 = k0+1;
                continue;
            }
            if( ae_fp_greater(xbuf->ptr.pp_double[k1][largestdim],splitval) )
            {
                k1 = k1-1;
                continue;
            }
            swaprows(xbuf, k0, k1, nx, _state);
            swaprows(ybuf, k0, k1, ny, _state);
            swapelementsi(initidx, k0, k1, _state);
            k0 = k0+1;
            k1 = k1-1;
        }
        ae_assert(k0>wrk0&&k1<wrk1-1, "RBFV3: integrity check 5843 in the recursive subdivision code failed", _state);
        ae_assert(k0==k1+1, "RBFV3: integrity check 5364 in the recursive subdivision code failed", _state);
        rbfv3_preprocessdatasetrec(xbuf, ybuf, initidx, wrk0, k0, nx, ny, mergetol, tmpboxmin, tmpboxmax, xout, yout, raw2wrkmap, wrk2rawmap, nout, _state);
        rbfv3_preprocessdatasetrec(xbuf, ybuf, initidx, k0, wrk1, nx, ny, mergetol, tmpboxmin, tmpboxmax, xout, yout, raw2wrkmap, wrk2rawmap, nout, _state);
    }
}


/*************************************************************************
This function preprocesses dataset by:
* merging non-distinct points
* centering points
* applying user scale to X-values
* performing additional scaling of X-values
* normalizing Y-values

INPUT PARAMETERS:
    XRaw        -   array[NRaw,NX], variable values
    YRaw        -   array[NRaw,NY], target values
    XScaleRaw   -   array[NX], user scales
    NRaw,NX,NY  -   metrics; N>0, NX>0, NY>0
    BFType      -   basis function type
    BFParamRaw  -   initial value for basis function paramerer (before
                    applying additional rescaling AddXRescaleAplied)
    LambdaVRaw  -   smoothing coefficient, as specified by user
    
OUTPUT PARAMETERS:
    XWrk        -   array[NWrk,NX], processed points, XWrk=(XRaw-XShift)/XScaleWrk
    YWrk        -   array[NWrk,NY], targets, scaled by dividing by YScale
    PointIndexes-   array[NWrk], point indexes in the original dataset
    NWrk        -   number of points after preprocessing, 0<NWrk<=NRaw
    XScaleWrk   -   array[NX], XScaleWrk[]=XScaleRaw[]*AddXRescaleAplied
    XShift      -   array[NX], centering coefficients
    YScale      -   common scaling for targets
    BFParamWrk  -   BFParamRaw/AddXRescaleAplied
    LambdaVWrk  -   LambdaV after dataset scaling, automatically adjusted for
                    dataset spread
    AddXRescaleAplied-additional scaling applied after user scaling

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_preprocessdataset(/* Real    */ const ae_matrix* _xraw,
     double mergetol,
     /* Real    */ const ae_matrix* _yraw,
     /* Real    */ const ae_vector* _xscaleraw,
     ae_int_t nraw,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t bftype,
     double bfparamraw,
     double lambdavraw,
     /* Real    */ ae_matrix* xwrk,
     /* Real    */ ae_matrix* ywrk,
     /* Integer */ ae_vector* raw2wrkmap,
     /* Integer */ ae_vector* wrk2rawmap,
     ae_int_t* nwrk,
     /* Real    */ ae_vector* xscalewrk,
     /* Real    */ ae_vector* xshift,
     double* bfparamwrk,
     double* lambdavwrk,
     double* addxrescaleaplied,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix xraw;
    ae_matrix yraw;
    ae_vector xscaleraw;
    ae_int_t i;
    ae_int_t j;
    double diag2;
    double v;
    ae_matrix xbuf;
    ae_matrix ybuf;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector boxmin;
    ae_vector boxmax;
    ae_vector initidx;

    ae_frame_make(_state, &_frame_block);
    memset(&xraw, 0, sizeof(xraw));
    memset(&yraw, 0, sizeof(yraw));
    memset(&xscaleraw, 0, sizeof(xscaleraw));
    memset(&xbuf, 0, sizeof(xbuf));
    memset(&ybuf, 0, sizeof(ybuf));
    memset(&tmp0, 0, sizeof(tmp0));
    memset(&tmp1, 0, sizeof(tmp1));
    memset(&boxmin, 0, sizeof(boxmin));
    memset(&boxmax, 0, sizeof(boxmax));
    memset(&initidx, 0, sizeof(initidx));
    ae_matrix_init_copy(&xraw, _xraw, _state, ae_true);
    ae_matrix_init_copy(&yraw, _yraw, _state, ae_true);
    ae_vector_init_copy(&xscaleraw, _xscaleraw, _state, ae_true);
    ae_matrix_clear(xwrk);
    ae_matrix_clear(ywrk);
    ae_vector_clear(raw2wrkmap);
    ae_vector_clear(wrk2rawmap);
    *nwrk = 0;
    ae_vector_clear(xscalewrk);
    ae_vector_clear(xshift);
    *bfparamwrk = 0.0;
    *lambdavwrk = 0.0;
    *addxrescaleaplied = 0.0;
    ae_matrix_init(&xbuf, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&ybuf, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp1, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&boxmin, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&boxmax, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&initidx, 0, DT_INT, _state, ae_true);

    ae_assert(nraw>=1, "RBFV3: integrity check 7295 failed", _state);
    
    /*
     * Scale dataset:
     * * first, scale it according to user-supplied scale
     * * second, analyze original dataset and rescale it one more time (same scaling across
     *   all dimensions) so it has zero mean and unit deviation
     * As a result, user-supplied scaling handles dimensionality issues and our additional
     * scaling normalizes data.
     *
     * After this block we have NRaw-sized dataset in XWrk/YWrk
     */
    rcopyallocv(nx, &xscaleraw, xscalewrk, _state);
    rsetallocv(nx, 0.0, xshift, _state);
    rallocm(nraw, nx, xwrk, _state);
    for(i=0; i<=nraw-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            xwrk->ptr.pp_double[i][j] = xraw.ptr.pp_double[i][j]/xscalewrk->ptr.p_double[j];
            xshift->ptr.p_double[j] = xshift->ptr.p_double[j]+xwrk->ptr.pp_double[i][j];
        }
    }
    rmulv(nx, (double)1/(double)nraw, xshift, _state);
    v = (double)(0);
    for(i=0; i<=nraw-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            v = v+(xwrk->ptr.pp_double[i][j]-xshift->ptr.p_double[j])*(xwrk->ptr.pp_double[i][j]-xshift->ptr.p_double[j]);
        }
    }
    *addxrescaleaplied = ae_sqrt((v+ae_sqrt(ae_machineepsilon, _state))/(double)(nraw*nx), _state);
    *bfparamwrk = bfparamraw;
    if( bftype==1 )
    {
        
        /*
         * Basis function parameter needs rescaling
         */
        if( ae_fp_less(bfparamraw,(double)(0)) )
        {
            *bfparamwrk = rbfv3_autodetectscaleparameter(xwrk, nraw, nx, _state)*(-bfparamraw)/(*addxrescaleaplied);
        }
        else
        {
            *bfparamwrk = bfparamraw/(*addxrescaleaplied);
        }
    }
    else
    {
        if( bftype==2 )
        {
            
            /*
             * Thin plate splines need special scaling; no params to rescale
             */
            *addxrescaleaplied = rbfv3_polyharmonic2scale*(*addxrescaleaplied);
        }
        else
        {
            ae_assert(ae_false, "RBFV3: integrity check 0632 failed", _state);
        }
    }
    rmulv(nx, *addxrescaleaplied, xscalewrk, _state);
    for(i=0; i<=nraw-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            xwrk->ptr.pp_double[i][j] = (xraw.ptr.pp_double[i][j]-xshift->ptr.p_double[j])/xscalewrk->ptr.p_double[j];
        }
    }
    rcopyallocm(nraw, ny, &yraw, ywrk, _state);
    
    /*
     * Merge nondistinct points
     */
    iallocv(nraw, &initidx, _state);
    for(i=0; i<=nraw-1; i++)
    {
        initidx.ptr.p_int[i] = i;
    }
    rcopyallocm(nraw, nx, xwrk, &xbuf, _state);
    rcopyallocm(nraw, ny, ywrk, &ybuf, _state);
    iallocv(nraw, raw2wrkmap, _state);
    iallocv(nraw, wrk2rawmap, _state);
    *nwrk = 0;
    rbfv3_preprocessdatasetrec(&xbuf, &ybuf, &initidx, 0, nraw, nx, ny, mergetol, &tmp0, &tmp1, xwrk, ywrk, raw2wrkmap, wrk2rawmap, nwrk, _state);
    
    /*
     * Compute LambdaV:
     * * compute bounding box
     * * compute DIAG2 = squared diagonal of the box
     * * set LambdaVWrk = LambdaVRaw times upper bound of the basis function value
     */
    rallocv(nx, &boxmin, _state);
    rallocv(nx, &boxmax, _state);
    rcopyrv(nx, xwrk, 0, &boxmin, _state);
    rcopyrv(nx, xwrk, 0, &boxmax, _state);
    for(i=1; i<=*nwrk-1; i++)
    {
        rmergeminrv(nx, xwrk, i, &boxmin, _state);
        rmergemaxrv(nx, xwrk, i, &boxmax, _state);
    }
    diag2 = (double)(0);
    for(i=0; i<=nx-1; i++)
    {
        diag2 = diag2+ae_sqr(boxmax.ptr.p_double[i]-boxmin.ptr.p_double[i], _state);
    }
    diag2 = ae_maxreal(diag2, (double)(1), _state);
    if( bftype==1 )
    {
        *lambdavwrk = lambdavraw*ae_sqrt(diag2+*bfparamwrk*(*bfparamwrk), _state);
    }
    else
    {
        if( bftype==2 )
        {
            *lambdavwrk = lambdavraw*diag2*ae_maxreal(ae_fabs(0.5*ae_log(diag2, _state), _state), 1.0, _state);
        }
        else
        {
            *lambdavwrk = lambdavraw;
            ae_assert(ae_false, "RBFV3: integrity check 7232 failed", _state);
        }
    }
    *lambdavwrk = *lambdavwrk/ae_sqr(*addxrescaleaplied, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function selects NSpec global nodes for approximate cardinal basis functions.

This function has O(N*NSpec) running time and O(N) memory requirements.

Each approximate cardinal basis function is a combination of several local
nodes (ones nearby to the center) and several global nodes (ones scattered
over entire dataset span).

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_selectglobalnodes(/* Real    */ const ae_matrix* xx,
     ae_int_t n,
     ae_int_t nx,
     /* Integer */ const ae_vector* existingnodes,
     ae_int_t nexisting,
     ae_int_t nspec,
     /* Integer */ ae_vector* nodes,
     ae_int_t* nchosen,
     double* maxdist,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_vector d2;
    ae_vector x;
    ae_vector busy;
    double v;
    double vv;

    ae_frame_make(_state, &_frame_block);
    memset(&d2, 0, sizeof(d2));
    memset(&x, 0, sizeof(x));
    memset(&busy, 0, sizeof(busy));
    *nchosen = 0;
    *maxdist = 0.0;
    ae_vector_init(&d2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&x, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&busy, 0, DT_BOOL, _state, ae_true);

    ae_assert(n>=1, "RBFV3: integrity check 6429 failed", _state);
    ae_assert(nexisting>=0, "RBFV3: integrity check 6412 failed", _state);
    ae_assert(nspec>=1, "RBFV3: integrity check 6430 failed", _state);
    nspec = ae_minint(nspec, n, _state);
    rsetallocv(n, 1.0E50, &d2, _state);
    rsetallocv(nx, 0.0, &x, _state);
    bsetallocv(n, ae_false, &busy, _state);
    if( nexisting==0 )
    {
        
        /*
         * No initial grid is provided, start distance evaluation from the data center
         */
        for(i=0; i<=n-1; i++)
        {
            rcopyrv(nx, xx, i, &x, _state);
        }
        rmulv(nx, (double)1/(double)n, &x, _state);
    }
    else
    {
        
        /*
         *
         */
        ae_assert(ae_false, "SelectGlobalNodes: NExisting<>0", _state);
    }
    iallocv(nspec, nodes, _state);
    *nchosen = 0;
    *maxdist = ae_maxrealnumber;
    while(*nchosen<nspec)
    {
        
        /*
         * Update distances using last added point stored in X.
         */
        for(j=0; j<=n-1; j++)
        {
            v = (double)(0);
            for(k=0; k<=nx-1; k++)
            {
                vv = x.ptr.p_double[k]-xx->ptr.pp_double[j][k];
                v = v+vv*vv;
            }
            d2.ptr.p_double[j] = ae_minreal(d2.ptr.p_double[j], v, _state);
        }
        
        /*
         * Select point with largest distance, add
         */
        k = 0;
        for(j=0; j<=n-1; j++)
        {
            if( ae_fp_greater(d2.ptr.p_double[j],d2.ptr.p_double[k])&&!busy.ptr.p_bool[j] )
            {
                k = j;
            }
        }
        if( busy.ptr.p_bool[k] )
        {
            break;
        }
        *maxdist = ae_minreal(*maxdist, d2.ptr.p_double[k], _state);
        nodes->ptr.p_int[*nchosen] = k;
        busy.ptr.p_bool[k] = ae_true;
        rcopyrv(nx, xx, k, &x, _state);
        *nchosen = *nchosen+1;
    }
    *maxdist = ae_sqrt(*maxdist, _state);
    ae_assert(*nchosen>=1||nexisting>0, "RBFV3: integrity check 6431 failed", _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function builds simplified tagged KD-tree: it assigns a tag (index in
the dataset) to each point, then drops most points (leaving  approximately
1/ReduceFactor of the entire dataset)  trying to  spread  residual  points
uniformly, and then constructs KD-tree.

It ensures that at least min(N,MinSize) points is retained.

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_buildsimplifiedkdtree(/* Real    */ const ae_matrix* xx,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t reducefactor,
     ae_int_t minsize,
     kdtree* kdt,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t ns;
    ae_matrix xs;
    ae_vector idx;
    hqrndstate rs;

    ae_frame_make(_state, &_frame_block);
    memset(&xs, 0, sizeof(xs));
    memset(&idx, 0, sizeof(idx));
    memset(&rs, 0, sizeof(rs));
    _kdtree_clear(kdt);
    ae_matrix_init(&xs, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&idx, 0, DT_INT, _state, ae_true);
    _hqrndstate_init(&rs, _state, ae_true);

    ae_assert(n>=1, "BuildSimplifiedKDTree: N<1", _state);
    ae_assert(reducefactor>=1, "BuildSimplifiedKDTree: ReduceFactor<1", _state);
    ae_assert(minsize>=0, "BuildSimplifiedKDTree: ReduceFactor<1", _state);
    hqrndseed(7674, 45775, &rs, _state);
    ns = imax3(ae_round((double)n/(double)reducefactor, _state), minsize, 1, _state);
    ns = ae_minint(ns, n, _state);
    iallocv(n, &idx, _state);
    rallocm(ns, nx, &xs, _state);
    for(i=0; i<=n-1; i++)
    {
        idx.ptr.p_int[i] = i;
    }
    for(i=0; i<=ns-1; i++)
    {
        j = i+hqrnduniformi(&rs, n-i, _state);
        k = idx.ptr.p_int[i];
        idx.ptr.p_int[i] = idx.ptr.p_int[j];
        idx.ptr.p_int[j] = k;
        rcopyrr(nx, xx, idx.ptr.p_int[i], &xs, i, _state);
    }
    kdtreebuildtagged(&xs, &idx, ns, nx, 0, 2, kdt, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Compute design matrices for the target-scatter preconditioner

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_computetargetscatterdesignmatrices(/* Real    */ const ae_matrix* xx,
     ae_int_t ntotal,
     ae_int_t nx,
     ae_int_t functype,
     double funcparam,
     /* Integer */ const ae_vector* workingnodes,
     ae_int_t nwrk,
     /* Integer */ const ae_vector* scatternodes,
     ae_int_t nscatter,
     /* Real    */ ae_matrix* atwrk,
     /* Real    */ ae_matrix* atsctr,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double v;
    double vv;
    ae_int_t ni;
    ae_int_t nj;
    double alpha2;


    
    /*
     * Compute working set and scatter set design matrices ATWrk and ATSctr
     *
     * ATWrk  is a (NWrk+NX+1)*NWrk matrix whose entries a[i,j] store:
     * * for I<NWrk             BasisFunc(X[wrk[i]]-X[wrj[j]])
     * * for NWrk<=I<NWrk+NX    X[wrk[j]], coordinate #(i-NWrk)
     * * for I=NWrk+NX          1.0
     *
     * ATSctr is a (NWrk+NX+1)*NScatter matrix whose entries a[i,j] store:
     * * for I<NWrk             BasisFunc(X[wrk[i]]-X[scatter[j]])
     * * for NWrk<=I<NWrk+NX    X[scatter[j]], coordinate #(i-NWrk)
     * * for I=NWrk+NX          1.0
     */
    ae_assert((functype==1||functype==2)||functype==3, "ACBF: unexpected basis function type", _state);
    alpha2 = funcparam*funcparam;
    rallocm(nwrk+nx+1, nwrk, atwrk, _state);
    for(i=0; i<=nwrk-1; i++)
    {
        ni = workingnodes->ptr.p_int[i];
        for(j=i; j<=nwrk-1; j++)
        {
            nj = workingnodes->ptr.p_int[j];
            v = (double)(0);
            for(k=0; k<=nx-1; k++)
            {
                vv = xx->ptr.pp_double[ni][k]-xx->ptr.pp_double[nj][k];
                v = v+vv*vv;
            }
            if( functype==1 )
            {
                v = -ae_sqrt(v+alpha2, _state);
            }
            if( functype==2 )
            {
                if( v!=(double)0 )
                {
                    v = v*0.5*ae_log(v, _state);
                }
                else
                {
                    v = (double)(0);
                }
            }
            if( functype==3 )
            {
                v = v*ae_sqrt(v, _state);
            }
            atwrk->ptr.pp_double[i][j] = v;
            atwrk->ptr.pp_double[j][i] = v;
        }
    }
    for(j=0; j<=nwrk-1; j++)
    {
        nj = workingnodes->ptr.p_int[j];
        for(i=0; i<=nx-1; i++)
        {
            atwrk->ptr.pp_double[nwrk+i][j] = xx->ptr.pp_double[nj][i];
        }
    }
    for(j=0; j<=nwrk-1; j++)
    {
        atwrk->ptr.pp_double[nwrk+nx][j] = 1.0;
    }
    if( nscatter>0 )
    {
        
        /*
         * We have scattered points too
         */
        rallocm(nwrk+nx+1, nscatter, atsctr, _state);
        for(i=0; i<=nwrk-1; i++)
        {
            ni = workingnodes->ptr.p_int[i];
            for(j=0; j<=nscatter-1; j++)
            {
                nj = scatternodes->ptr.p_int[j];
                v = (double)(0);
                for(k=0; k<=nx-1; k++)
                {
                    vv = xx->ptr.pp_double[ni][k]-xx->ptr.pp_double[nj][k];
                    v = v+vv*vv;
                }
                if( functype==1 )
                {
                    v = -ae_sqrt(v+alpha2, _state);
                }
                if( functype==2 )
                {
                    if( v!=(double)0 )
                    {
                        v = v*0.5*ae_log(v, _state);
                    }
                    else
                    {
                        v = (double)(0);
                    }
                }
                if( functype==3 )
                {
                    v = v*ae_sqrt(v, _state);
                }
                atsctr->ptr.pp_double[i][j] = v;
            }
        }
        for(j=0; j<=nscatter-1; j++)
        {
            nj = scatternodes->ptr.p_int[j];
            for(i=0; i<=nx-1; i++)
            {
                atsctr->ptr.pp_double[nwrk+i][j] = xx->ptr.pp_double[nj][i];
            }
        }
        for(j=0; j<=nscatter-1; j++)
        {
            atsctr->ptr.pp_double[nwrk+nx][j] = 1.0;
        }
    }
}


/*************************************************************************
ACBF preconditioner generation basecase.

PARAMETERS:
    Builder             -   ACBF builder object
    Wrk0, Wrk1          -   elements [Wrk0...Wrk1-1] of Builder.WrkIdx[]
                            array store row indexes of XX that are processed.
    
OUTPUT:
    Builder.OutputPool is updated with new chunks

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_computeacbfpreconditionerbasecase(acbfbuilder* builder,
     acbfbuffer* buf,
     ae_int_t wrk0,
     ae_int_t wrk1,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t widx;
    ae_int_t targetidx;
    ae_int_t nx;
    ae_int_t nglobal;
    ae_int_t nlocal;
    ae_int_t ncorrection;
    ae_int_t ncenters;
    ae_int_t nchosen;
    ae_int_t ncoeff;
    ae_int_t batchsize;
    ae_int_t nk;
    ae_int_t nq;
    ae_vector x;
    ae_vector batchcenter;
    ae_vector idummy;
    double localrad;
    double currentrad;
    double reg;
    double v;
    double vv;
    double mx;
    double maxdist2;
    ae_int_t ortbasissize;
    ae_vector ortbasismap;
    acbfchunk *precchunk;
    ae_smart_ptr _precchunk;
    ae_int_t expansionscount;
    ae_matrix dbgb;
    double dbgerrnodes;
    double dbgerrort;
    double dbgcondq;
    double dbgmaxc;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&batchcenter, 0, sizeof(batchcenter));
    memset(&idummy, 0, sizeof(idummy));
    memset(&ortbasismap, 0, sizeof(ortbasismap));
    memset(&_precchunk, 0, sizeof(_precchunk));
    memset(&dbgb, 0, sizeof(dbgb));
    ae_vector_init(&x, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&batchcenter, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&idummy, 0, DT_INT, _state, ae_true);
    ae_vector_init(&ortbasismap, 0, DT_INT, _state, ae_true);
    ae_smart_ptr_init(&_precchunk, (void**)&precchunk, _state, ae_true);
    ae_matrix_init(&dbgb, 0, 0, DT_REAL, _state, ae_true);

    if( wrk1<=wrk0 )
    {
        ae_frame_leave(_state);
        return;
    }
    nx = builder->nx;
    nglobal = builder->nglobal;
    nlocal = builder->nlocal;
    ncorrection = builder->ncorrection;
    reg = ae_sqrt(ae_machineepsilon, _state);
    rallocv(nx, &x, _state);
    expansionscount = 0;
    
    /*
     * First, select a batch of central points and compute batch center
     */
    batchsize = wrk1-wrk0;
    iallocv(batchsize, &buf->currentnodes, _state);
    rsetallocv(nx, 0.0, &batchcenter, _state);
    for(i=0; i<=batchsize-1; i++)
    {
        targetidx = builder->wrkidx.ptr.p_int[wrk0+i];
        buf->currentnodes.ptr.p_int[i] = targetidx;
        buf->bflags.ptr.p_bool[targetidx] = ae_true;
        raddrv(nx, (double)1/(double)batchsize, &builder->xx, builder->wrkidx.ptr.p_int[wrk0+i], &batchcenter, _state);
    }
    ncenters = batchsize;
    
    /*
     * Then, add a hull of nearest neighbors and compute its radius
     */
    localrad = (double)(0);
    for(widx=0; widx<=batchsize-1; widx++)
    {
        
        /*
         * Select immediate neighbors
         */
        rcopyrv(nx, &builder->xx, builder->wrkidx.ptr.p_int[wrk0+widx], &x, _state);
        nq = kdtreetsqueryknn(&builder->kdt, &buf->kdtbuf, &x, nlocal, ae_true, _state);
        kdtreetsqueryresultstags(&builder->kdt, &buf->kdtbuf, &buf->neighbors, _state);
        kdtreetsqueryresultsdistances(&builder->kdt, &buf->kdtbuf, &buf->d, _state);
        for(k=0; k<=nq-1; k++)
        {
            nk = buf->neighbors.ptr.p_int[k];
            if( !buf->bflags.ptr.p_bool[nk] )
            {
                buf->bflags.ptr.p_bool[nk] = ae_true;
                igrowv(ncenters+1, &buf->currentnodes, _state);
                buf->currentnodes.ptr.p_int[ncenters] = nk;
                ncenters = ncenters+1;
                v = (double)(0);
                for(j=0; j<=nx-1; j++)
                {
                    v = v+(builder->xx.ptr.pp_double[nk][j]-batchcenter.ptr.p_double[j])*(builder->xx.ptr.pp_double[nk][j]-batchcenter.ptr.p_double[j]);
                }
                localrad = ae_maxreal(localrad, v, _state);
            }
        }
    }
    localrad = ae_sqrt(localrad, _state);
    currentrad = localrad;
    
    /*
     * Add global grid
     */
    if( nglobal>0 )
    {
        for(k=0; k<=nglobal-1; k++)
        {
            nk = builder->globalgrid.ptr.p_int[k];
            if( !buf->bflags.ptr.p_bool[nk] )
            {
                buf->bflags.ptr.p_bool[nk] = ae_true;
                igrowv(ncenters+1, &buf->currentnodes, _state);
                buf->currentnodes.ptr.p_int[ncenters] = nk;
                ncenters = ncenters+1;
            }
        }
    }
    
    /*
     * Add local correction grid: select more distant neighbors
     */
    while((ncorrection>0&&ae_fp_greater(currentrad,(double)(0)))&&ae_fp_less(currentrad,builder->roughdatasetdiameter))
    {
        
        /*
         * Select neighbors within CurrentRad*Builder.CorrectorGrowth
         */
        if( expansionscount==0 )
        {
            
            /*
             * First expansion, use simplified kd-tree #1
             */
            nq = kdtreetsqueryrnn(&builder->kdt1, &buf->kdt1buf, &batchcenter, currentrad*builder->correctorgrowth, ae_true, _state);
            kdtreetsqueryresultstags(&builder->kdt1, &buf->kdt1buf, &buf->neighbors, _state);
        }
        else
        {
            
            /*
             * Subsequent expansions, use simplified kd-tree #2
             */
            nq = kdtreetsqueryrnn(&builder->kdt2, &buf->kdt2buf, &batchcenter, currentrad*builder->correctorgrowth, ae_true, _state);
            kdtreetsqueryresultstags(&builder->kdt2, &buf->kdt2buf, &buf->neighbors, _state);
        }
        
        /*
         * Compute a grid of well-separated nodes using neighbors
         */
        rallocm(nq, nx, &buf->xq, _state);
        for(k=0; k<=nq-1; k++)
        {
            nk = buf->neighbors.ptr.p_int[k];
            for(j=0; j<=nx-1; j++)
            {
                buf->xq.ptr.pp_double[k][j] = builder->xx.ptr.pp_double[nk][j];
            }
        }
        rbfv3_selectglobalnodes(&buf->xq, nq, nx, &idummy, 0, ncorrection, &buf->chosenneighbors, &nchosen, &maxdist2, _state);
        
        /*
         * Select neighbrs that are NOT within CurrentRad from the batch center
         * and that are NOT already chosen.
         */
        for(k=0; k<=nchosen-1; k++)
        {
            nk = buf->neighbors.ptr.p_int[buf->chosenneighbors.ptr.p_int[k]];
            v = (double)(0);
            for(j=0; j<=nx-1; j++)
            {
                v = v+ae_sqr(builder->xx.ptr.pp_double[nk][j]-batchcenter.ptr.p_double[j], _state);
            }
            v = ae_sqrt(v, _state);
            if( !buf->bflags.ptr.p_bool[nk]&&ae_fp_greater(v,currentrad) )
            {
                buf->bflags.ptr.p_bool[nk] = ae_true;
                igrowv(ncenters+1, &buf->currentnodes, _state);
                buf->currentnodes.ptr.p_int[ncenters] = nk;
                ncenters = ncenters+1;
            }
        }
        
        /*
         * Update radius and debug counters
         */
        currentrad = currentrad*builder->correctorgrowth;
        inc(&expansionscount, _state);
    }
    
    /*
     * Clean up bFlags[]
     */
    for(k=0; k<=ncenters-1; k++)
    {
        buf->bflags.ptr.p_bool[buf->currentnodes.ptr.p_int[k]] = ae_false;
    }
    
    /*
     * Compute working set and scatter set design matrices ATWrk and ATSctr
     *
     * ATWrk  is a (NWrk+NX+1)*NWrk matrix whose entries a[i,j] store:
     * * for I<NWrk             BasisFunc(X[wrk[i]]-X[wrj[j]])
     * * for NWrk<=I<NWrk+NX    X[wrk[j]], coordinate #(i-NWrk)
     * * for I=NWrk+NX          1.0
     *
     * ATSctr is a (NWrk+NX+1)*NScatter matrix whose entries a[i,j] store:
     * * for I<NWrk             BasisFunc(X[wrk[i]]-X[scatter[j]])
     * * for NWrk<=I<NWrk+NX    X[scatter[j]], coordinate #(i-NWrk)
     * * for I=NWrk+NX          1.0
     */
    rbfv3_computetargetscatterdesignmatrices(&builder->xx, builder->ntotal, nx, builder->functype, builder->funcparam, &buf->currentnodes, ncenters, &buf->currentnodes, 0, &buf->atwrk, &buf->atwrk, _state);
    
    /*
     * Prepare and solve linear system, coefficients are stored in rows of Buf.B
     *
     * Depending on whether the basis is conditionally positive definite (given current polynomial term type),
     * we either:
     * * use generic QR solver to solve the linear system (when the basis is not CPD)
     * * use specialized CPD solver that is several times faster and is more accurate
     */
    if( builder->aterm!=1||!rbfv3_iscpdfunction(builder->functype, builder->aterm, _state) )
    {
        ae_assert(ae_fp_greater_eq(builder->lambdav,(double)(0)), "RBF3: integrity check 8363 failed", _state);
        ae_assert((builder->aterm==1||builder->aterm==2)||builder->aterm==3, "RBF3: integrity check 8364 failed", _state);
        
        /*
         * Basis function has no conditional positive definiteness guarantees (given the linear term type).
         *
         * Solve using QR decomposition.
         */
        ncoeff = ncenters+nx+1;
        rsetallocm(ncoeff, ncoeff+batchsize, 0.0, &buf->q, _state);
        for(i=0; i<=ncenters-1; i++)
        {
            rcopyrr(ncenters, &buf->atwrk, i, &buf->q, i, _state);
            buf->q.ptr.pp_double[i][i] = buf->q.ptr.pp_double[i][i]+builder->lambdav;
        }
        if( builder->aterm==1 )
        {
            
            /*
             * Linear term is used
             */
            for(i=0; i<=nx; i++)
            {
                for(j=0; j<=ncenters-1; j++)
                {
                    buf->q.ptr.pp_double[ncenters+i][j] = buf->atwrk.ptr.pp_double[ncenters+i][j];
                    buf->q.ptr.pp_double[j][ncenters+i] = buf->atwrk.ptr.pp_double[ncenters+i][j];
                }
            }
        }
        if( builder->aterm==2 )
        {
            
            /*
             * Constant term is used
             */
            for(i=0; i<=nx-1; i++)
            {
                buf->q.ptr.pp_double[ncenters+i][ncenters+i] = 1.0;
            }
            for(j=0; j<=ncenters-1; j++)
            {
                buf->q.ptr.pp_double[ncenters+nx][j] = 1.0;
                buf->q.ptr.pp_double[j][ncenters+nx] = 1.0;
            }
        }
        if( builder->aterm==3 )
        {
            
            /*
             * Zero term is used
             */
            for(i=0; i<=nx; i++)
            {
                buf->q.ptr.pp_double[ncenters+i][ncenters+i] = 1.0;
            }
        }
        for(i=0; i<=batchsize-1; i++)
        {
            buf->q.ptr.pp_double[i][ncoeff+i] = 1.0;
        }
        mx = 1.0;
        for(i=0; i<=ncoeff-1; i++)
        {
            for(j=i; j<=ncoeff-1; j++)
            {
                mx = ae_maxreal(mx, ae_fabs(buf->q.ptr.pp_double[i][j], _state), _state);
            }
        }
        for(j=0; j<=ncoeff-1; j++)
        {
            buf->q.ptr.pp_double[j][j] = buf->q.ptr.pp_double[j][j]+reg*mx*possign(buf->q.ptr.pp_double[j][j], _state);
        }
        rmatrixqr(&buf->q, ncoeff, ncoeff+batchsize, &buf->tau, _state);
        rallocm(batchsize, ncoeff, &buf->b, _state);
        rmatrixtranspose(ncoeff, batchsize, &buf->q, 0, ncoeff, &buf->b, 0, 0, _state);
        rmatrixrighttrsm(batchsize, ncoeff, &buf->q, 0, 0, ae_true, ae_false, 1, &buf->b, 0, 0, _state);
    }
    else
    {
        ae_assert(ae_fp_greater_eq(builder->lambdav,(double)(0)), "RBF3: integrity check 8368 failed", _state);
        ae_assert(builder->aterm==1, "RBF3: integrity check 7365 failed", _state);
        ncoeff = ncenters+nx+1;
        
        /*
         * First, compute orthogonal basis of space spanned by polynomials of degree 1
         */
        rallocm(ncenters, ncenters, &buf->r, _state);
        rallocm(nx+1, ncenters, &buf->q1, _state);
        iallocv(nx+1, &ortbasismap, _state);
        rsetr(ncenters, (double)1/ae_sqrt((double)(ncenters), _state), &buf->q1, 0, _state);
        buf->r.ptr.pp_double[0][0] = ae_sqrt((double)(ncenters), _state);
        ortbasismap.ptr.p_int[0] = nx;
        ortbasissize = 1;
        rallocv(ncenters, &buf->z, _state);
        for(k=0; k<=nx-1; k++)
        {
            for(j=0; j<=ncenters-1; j++)
            {
                buf->z.ptr.p_double[j] = buf->atwrk.ptr.pp_double[ncenters+k][j];
            }
            v = ae_sqrt(rdotv2(ncenters, &buf->z, _state), _state);
            rowwisegramschmidt(&buf->q1, ortbasissize, ncenters, &buf->z, &buf->y, ae_true, _state);
            vv = ae_sqrt(rdotv2(ncenters, &buf->z, _state), _state);
            if( ae_fp_greater(vv,ae_sqrt(ae_machineepsilon, _state)*(v+(double)1)) )
            {
                rcopymulvr(ncenters, (double)1/vv, &buf->z, &buf->q1, ortbasissize, _state);
                rcopyvc(ortbasissize, &buf->y, &buf->r, ortbasissize, _state);
                buf->r.ptr.pp_double[ortbasissize][ortbasissize] = vv;
                ortbasismap.ptr.p_int[ortbasissize] = k;
                ortbasissize = ortbasissize+1;
            }
        }
        
        /*
         * Second, compute system matrix Q and target values for cardinal basis functions B.
         *
         * The Q is conditionally positive definite, i.e. x'*Q*x>0 for any x satisfying orthogonality conditions
         * (orthogonal with respect to basis stored in Q1).
         */
        rsetallocm(ncenters, ncenters, 0.0, &buf->q, _state);
        for(i=0; i<=ncenters-1; i++)
        {
            rcopyrr(ncenters, &buf->atwrk, i, &buf->q, i, _state);
        }
        rsetallocm(batchsize, ncoeff, 0.0, &buf->b, _state);
        for(i=0; i<=batchsize-1; i++)
        {
            buf->b.ptr.pp_double[i][i] = 1.0;
        }
        for(i=0; i<=ncenters-1; i++)
        {
            buf->q.ptr.pp_double[i][i] = buf->q.ptr.pp_double[i][i]+builder->lambdav;
        }
        
        /*
         * Transform Q from conditionally positive definite to the (simply) positive definite one:
         * multiply linear system Q*x=RHS from both sides by (I-Q1'*Q1), apply additional regularization.
         *
         * NOTE: RHS is also multiplied by (I-Q1'*Q1), but from the left only.
         */
        rallocv(ncenters, &buf->z, _state);
        for(i=0; i<=ncenters-1; i++)
        {
            rcopyrv(ncenters, &buf->q, i, &buf->z, _state);
            rowwisegramschmidt(&buf->q1, ortbasissize, ncenters, &buf->z, &buf->y, ae_false, _state);
            rcopyvr(ncenters, &buf->z, &buf->q, i, _state);
        }
        for(i=0; i<=ncenters-1; i++)
        {
            rcopycv(ncenters, &buf->q, i, &buf->z, _state);
            rowwisegramschmidt(&buf->q1, ortbasissize, ncenters, &buf->z, &buf->y, ae_false, _state);
            rcopyvc(ncenters, &buf->z, &buf->q, i, _state);
        }
        for(i=0; i<=batchsize-1; i++)
        {
            rcopyrv(ncenters, &buf->b, i, &buf->z, _state);
            rowwisegramschmidt(&buf->q1, ortbasissize, ncenters, &buf->z, &buf->y, ae_false, _state);
            rcopyvr(ncenters, &buf->z, &buf->b, i, _state);
        }
        mx = 1.0;
        for(i=0; i<=ncenters-1; i++)
        {
            mx = ae_maxreal(mx, ae_fabs(buf->q.ptr.pp_double[i][i], _state), _state);
        }
        for(i=0; i<=ncenters-1; i++)
        {
            rcopyrv(ncenters, &buf->q, i, &buf->z, _state);
            for(j=0; j<=ortbasissize-1; j++)
            {
                raddrv(ncenters, mx*buf->q1.ptr.pp_double[j][i], &buf->q1, j, &buf->z, _state);
            }
            rcopyvr(ncenters, &buf->z, &buf->q, i, _state);
        }
        if( builder->dodetailedtrace )
        {
            
            /*
             * Compute condition number for future reports
             */
            dbgcondq = (double)1/(spdmatrixrcond(&buf->q, ncenters, ae_false, _state)+ae_machineepsilon);
        }
        else
        {
            dbgcondq = (double)(0);
        }
        for(i=0; i<=ncenters-1; i++)
        {
            buf->q.ptr.pp_double[i][i] = buf->q.ptr.pp_double[i][i]+reg*mx;
        }
        
        /*
         * Perform Cholesky factorization, solve and obtain RBF coefficients (we still have
         * to compute polynomial term - it will be done later)
         */
        if( !spdmatrixcholeskyrec(&buf->q, 0, ncenters, ae_false, &buf->choltmp, _state) )
        {
            ae_assert(ae_false, "RBFV3: ACBF solver failed due to extreme degeneracy", _state);
        }
        rmatrixrighttrsm(batchsize, ncenters, &buf->q, 0, 0, ae_false, ae_false, 1, &buf->b, 0, 0, _state);
        rmatrixrighttrsm(batchsize, ncenters, &buf->q, 0, 0, ae_false, ae_false, 0, &buf->b, 0, 0, _state);
        
        /*
         * Now, having RBF coefficients we can compute residual from fitting ACBF targets
         * with pure RBF term and fit polynomial term to this residual. In the ideal world
         * it should result in the nice and precise polynomial coefficients.
         */
        rallocv(ncenters, &buf->z, _state);
        rsetallocm(batchsize, ncenters, 0.0, &buf->c, _state);
        for(i=0; i<=batchsize-1; i++)
        {
            buf->c.ptr.pp_double[i][i] = 1.0;
        }
        rmatrixgemm(batchsize, ncenters, ncenters, -1.0, &buf->b, 0, 0, 0, &buf->atwrk, 0, 0, 1, 1.0, &buf->c, 0, 0, _state);
        for(i=0; i<=batchsize-1; i++)
        {
            rcopyrv(ncenters, &buf->c, i, &buf->z, _state);
            rowwisegramschmidt(&buf->q1, ortbasissize, ncenters, &buf->z, &buf->y, ae_true, _state);
            rmatrixtrsv(ortbasissize, &buf->r, 0, 0, ae_true, ae_false, 0, &buf->y, 0, _state);
            for(j=0; j<=nx; j++)
            {
                buf->b.ptr.pp_double[i][ncenters+j] = 0.0;
            }
            for(j=0; j<=ortbasissize-1; j++)
            {
                buf->b.ptr.pp_double[i][ncenters+ortbasismap.ptr.p_int[j]] = buf->y.ptr.p_double[j];
            }
        }
        
        /*
         * Trace if needeed
         */
        if( builder->dodetailedtrace )
        {
            rallocm(batchsize, ncenters, &dbgb, _state);
            rmatrixgemm(batchsize, ncenters, ncoeff, -1.0, &buf->b, 0, 0, 0, &buf->atwrk, 0, 0, 0, 0.0, &dbgb, 0, 0, _state);
            for(i=0; i<=batchsize-1; i++)
            {
                dbgb.ptr.pp_double[i][i] = dbgb.ptr.pp_double[i][i]+1.0;
            }
            dbgmaxc = (double)(0);
            for(i=0; i<=batchsize-1; i++)
            {
                dbgmaxc = ae_maxreal(dbgmaxc, rmaxabsr(ncenters, &buf->b, i, _state), _state);
            }
            dbgerrnodes = (double)(0);
            for(i=0; i<=batchsize-1; i++)
            {
                dbgerrnodes = dbgerrnodes+rdotrr(ncenters, &dbgb, i, &dbgb, i, _state);
            }
            dbgerrnodes = ae_sqrt(dbgerrnodes/(double)(batchsize*ncenters), _state);
            dbgerrort = (double)(0);
            for(i=0; i<=batchsize-1; i++)
            {
                for(j=0; j<=ortbasissize-1; j++)
                {
                    dbgerrort = ae_maxreal(dbgerrort, ae_fabs(rdotrr(ncenters, &buf->b, i, &buf->q1, j, _state), _state), _state);
                }
            }
            ae_trace("[ACBF_subprob]    BatchSize=%3d    NCenters=%4d    RadiusExpansions=%0d    cond(Q)=%0.2e    max|C|=%0.2e    rmsErr=%0.2e    OrtErr=%0.2e\n",
                (int)(batchsize),
                (int)(ncenters),
                (int)(expansionscount),
                (double)(dbgcondq),
                (double)(dbgmaxc),
                (double)(dbgerrnodes),
                (double)(dbgerrort));
        }
    }
    
    /*
     * Solve and save solution to Builder.ChunksPool
     */
    ae_shared_pool_retrieve(&builder->chunksproducer, &_precchunk, _state);
    ae_assert(precchunk->ntargetrows==-117, "RBFV3: integrity check 9724 failed", _state);
    ae_assert(precchunk->ntargetcols==-119, "RBFV3: integrity check 9725 failed", _state);
    precchunk->ntargetrows = batchsize;
    precchunk->ntargetcols = ncoeff;
    iallocv(precchunk->ntargetrows, &precchunk->targetrows, _state);
    iallocv(precchunk->ntargetcols, &precchunk->targetcols, _state);
    rallocm(batchsize, ncoeff, &precchunk->s, _state);
    for(widx=0; widx<=batchsize-1; widx++)
    {
        precchunk->targetrows.ptr.p_int[widx] = builder->wrkidx.ptr.p_int[wrk0+widx];
    }
    iallocv(ncoeff, &buf->perm, _state);
    for(k=0; k<=ncoeff-1; k++)
    {
        if( k<ncenters )
        {
            precchunk->targetcols.ptr.p_int[k] = buf->currentnodes.ptr.p_int[k];
        }
        else
        {
            precchunk->targetcols.ptr.p_int[k] = builder->ntotal+(k-ncenters);
        }
        buf->perm.ptr.p_int[k] = k;
    }
    tagsortmiddleii(&precchunk->targetcols, &buf->perm, 0, ncoeff, _state);
    for(widx=0; widx<=batchsize-1; widx++)
    {
        for(k=0; k<=ncoeff-1; k++)
        {
            precchunk->s.ptr.pp_double[widx][k] = buf->b.ptr.pp_double[widx][buf->perm.ptr.p_int[k]];
        }
    }
    ae_shared_pool_recycle(&builder->chunkspool, &_precchunk, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Recursive ACBF preconditioner generation subroutine.

PARAMETERS:
    Builder             -   ACBF builder object
    Wrk0, Wrk1          -   elements [Wrk0...Wrk1-1] of Builder.WrkIdx[]
                            array store row indexes of XX that are processed.
    
OUTPUT:
    Builder.OutputPool is updated with new chunks

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_computeacbfpreconditionerrecv2(acbfbuilder* builder,
     ae_int_t wrk0,
     ae_int_t wrk1,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t nx;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k0;
    ae_int_t k1;
    ae_int_t largestdim;
    double splitval;
    double basecasecomplexity;
    acbfbuffer *buf;
    ae_smart_ptr _buf;

    ae_frame_make(_state, &_frame_block);
    memset(&_buf, 0, sizeof(_buf));
    ae_smart_ptr_init(&_buf, (void**)&buf, _state, ae_true);

    nx = builder->nx;
    if( wrk1<=wrk0 )
    {
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        ae_frame_leave(_state);
        return;
    }
    basecasecomplexity = rmul3((double)(builder->nglobal+builder->nlocal+2*builder->ncorrection), (double)(builder->nglobal+builder->nlocal+2*builder->ncorrection), (double)(builder->nglobal+builder->nlocal+2*builder->ncorrection), _state);
    
    /*
     * Decide on parallelism
     */
    if( ae_fp_greater_eq(rmul2((double)(builder->ntotal), basecasecomplexity, _state),smpactivationlevel(_state))&&builder->ntotal>=rbfv3_acbfparallelthreshold )
    {
        if( _trypexec_rbfv3_computeacbfpreconditionerrecv2(builder,wrk0,wrk1, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            ae_frame_leave(_state);
            return;
        }
    }
    
    /*
     * Retrieve temporary buffer
     */
    ae_shared_pool_retrieve(&builder->bufferpool, &_buf, _state);
    
    /*
     * Analyze current working set
     */
    rallocv(nx, &buf->tmpboxmin, _state);
    rallocv(nx, &buf->tmpboxmax, _state);
    rcopyrv(nx, &builder->xx, builder->wrkidx.ptr.p_int[wrk0], &buf->tmpboxmin, _state);
    rcopyrv(nx, &builder->xx, builder->wrkidx.ptr.p_int[wrk0], &buf->tmpboxmax, _state);
    for(i=wrk0+1; i<=wrk1-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            buf->tmpboxmin.ptr.p_double[j] = ae_minreal(buf->tmpboxmin.ptr.p_double[j], builder->xx.ptr.pp_double[builder->wrkidx.ptr.p_int[i]][j], _state);
            buf->tmpboxmax.ptr.p_double[j] = ae_maxreal(buf->tmpboxmax.ptr.p_double[j], builder->xx.ptr.pp_double[builder->wrkidx.ptr.p_int[i]][j], _state);
        }
    }
    largestdim = 0;
    for(j=1; j<=nx-1; j++)
    {
        if( ae_fp_greater(buf->tmpboxmax.ptr.p_double[j]-buf->tmpboxmin.ptr.p_double[j],buf->tmpboxmax.ptr.p_double[largestdim]-buf->tmpboxmin.ptr.p_double[largestdim]) )
        {
            largestdim = j;
        }
    }
    
    /*
     * Perform either batch processing or recursive split
     */
    if( wrk1-wrk0<=builder->batchsize||ae_fp_eq(buf->tmpboxmax.ptr.p_double[largestdim],buf->tmpboxmin.ptr.p_double[largestdim]) )
    {
        
        /*
         * Either working set size is small enough or all points are non-distinct.
         * Perform batch processing
         */
        rbfv3_computeacbfpreconditionerbasecase(builder, buf, wrk0, wrk1, _state);
        
        /*
         * Recycle temporary buffers
         */
        ae_shared_pool_recycle(&builder->bufferpool, &_buf, _state);
    }
    else
    {
        
        /*
         * Compute recursive split along largest axis
         */
        splitval = 0.5*(buf->tmpboxmax.ptr.p_double[largestdim]+buf->tmpboxmin.ptr.p_double[largestdim]);
        k0 = wrk0;
        k1 = wrk1-1;
        while(k0<=k1)
        {
            if( ae_fp_less_eq(builder->xx.ptr.pp_double[builder->wrkidx.ptr.p_int[k0]][largestdim],splitval) )
            {
                k0 = k0+1;
                continue;
            }
            if( ae_fp_greater(builder->xx.ptr.pp_double[builder->wrkidx.ptr.p_int[k1]][largestdim],splitval) )
            {
                k1 = k1-1;
                continue;
            }
            swapelementsi(&builder->wrkidx, k0, k1, _state);
            k0 = k0+1;
            k1 = k1-1;
        }
        ae_assert(k0>wrk0&&k1<wrk1-1, "ACBF: integrity check 2843 in the recursive subdivision code failed", _state);
        ae_assert(k0==k1+1, "ACBF: integrity check 8364 in the recursive subdivision code failed", _state);
        
        /*
         * Recycle temporary buffer, perform recursive calls
         */
        ae_shared_pool_recycle(&builder->bufferpool, &_buf, _state);
        ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_fp_greater_eq(ae_minreal((double)(k0-wrk0), (double)(wrk1-k0), _state)*basecasecomplexity,spawnlevel(_state)), _state);
        _spawn_rbfv3_computeacbfpreconditionerrecv2(builder, wrk0, k0, _child_tasks, _smp_enabled, _state);
        rbfv3_computeacbfpreconditionerrecv2(builder, k0, wrk1, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_false, _state);
    }
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_rbfv3_computeacbfpreconditionerrecv2(acbfbuilder* builder,
    ae_int_t wrk0,
    ae_int_t wrk1,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        rbfv3_computeacbfpreconditionerrecv2(builder,wrk0,wrk1, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_rbfv3_computeacbfpreconditionerrecv2;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = builder;
        _task->data.parameters[1].value.ival = wrk0;
        _task->data.parameters[2].value.ival = wrk1;
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
void _task_rbfv3_computeacbfpreconditionerrecv2(ae_task_data *_data, ae_state *_state)
{
    acbfbuilder* builder;
    ae_int_t wrk0;
    ae_int_t wrk1;
    builder = (acbfbuilder*)_data->parameters[0].value.val;
    wrk0 = _data->parameters[1].value.ival;
    wrk1 = _data->parameters[2].value.ival;
   ae_state_set_flags(_state, _data->flags);
   rbfv3_computeacbfpreconditionerrecv2(builder,wrk0,wrk1, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_rbfv3_computeacbfpreconditionerrecv2(acbfbuilder* builder,
    ae_int_t wrk0,
    ae_int_t wrk1,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_rbfv3_computeacbfpreconditionerrecv2;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = builder;
    _task->data.parameters[1].value.ival = wrk0;
    _task->data.parameters[2].value.ival = wrk1;
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
This function generates ACBF (approximate cardinal basis functions)
preconditioner.

PARAMETERS:
    XX                  -   dataset (X-values), array[N,NX]
    N                   -   points count, N>=1
    NX                  -   dimensions count, NX>=1
    FuncType            -   basis function type
    
OUTPUT:
    SP                  -   preconditioner, sparse matrix in CRS format

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_computeacbfpreconditioner(/* Real    */ const ae_matrix* xx,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t functype,
     double funcparam,
     ae_int_t aterm,
     ae_int_t batchsize,
     ae_int_t nglobal,
     ae_int_t nlocal,
     ae_int_t ncorrection,
     ae_int_t correctorgrowth,
     ae_int_t simplificationfactor,
     double lambdav,
     sparsematrix* sp,
     ae_state *_state)
{
    ae_frame _frame_block;
    acbfbuilder builder;
    acbfbuffer bufferseed;
    acbfchunk chunkseed;
    acbfchunk *precchunk;
    ae_smart_ptr _precchunk;
    ae_int_t i;
    ae_int_t j;
    ae_int_t offs;
    ae_vector idummy;
    ae_vector rowsizes;
    ae_vector boxmin;
    ae_vector boxmax;

    ae_frame_make(_state, &_frame_block);
    memset(&builder, 0, sizeof(builder));
    memset(&bufferseed, 0, sizeof(bufferseed));
    memset(&chunkseed, 0, sizeof(chunkseed));
    memset(&_precchunk, 0, sizeof(_precchunk));
    memset(&idummy, 0, sizeof(idummy));
    memset(&rowsizes, 0, sizeof(rowsizes));
    memset(&boxmin, 0, sizeof(boxmin));
    memset(&boxmax, 0, sizeof(boxmax));
    _acbfbuilder_init(&builder, _state, ae_true);
    _acbfbuffer_init(&bufferseed, _state, ae_true);
    _acbfchunk_init(&chunkseed, _state, ae_true);
    ae_smart_ptr_init(&_precchunk, (void**)&precchunk, _state, ae_true);
    ae_vector_init(&idummy, 0, DT_INT, _state, ae_true);
    ae_vector_init(&rowsizes, 0, DT_INT, _state, ae_true);
    ae_vector_init(&boxmin, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&boxmax, 0, DT_REAL, _state, ae_true);

    ae_assert(n>=1, "RBFV3: integrity check 2524 failed", _state);
    
    /*
     * Prepare builder
     */
    builder.dodetailedtrace = ae_is_trace_enabled("RBF.DETAILED");
    builder.functype = functype;
    builder.funcparam = funcparam;
    builder.ntotal = n;
    builder.nx = nx;
    builder.batchsize = batchsize;
    if( nglobal>0 )
    {
        rbfv3_selectglobalnodes(xx, n, nx, &idummy, 0, nglobal, &builder.globalgrid, &builder.nglobal, &builder.globalgridseparation, _state);
    }
    else
    {
        builder.nglobal = 0;
    }
    builder.nlocal = nlocal;
    builder.ncorrection = ncorrection;
    builder.correctorgrowth = (double)(correctorgrowth);
    builder.lambdav = lambdav;
    builder.aterm = aterm;
    rcopyallocm(n, nx, xx, &builder.xx, _state);
    rallocv(nx, &boxmin, _state);
    rallocv(nx, &boxmax, _state);
    rcopyrv(nx, xx, 0, &boxmin, _state);
    rcopyrv(nx, xx, 0, &boxmax, _state);
    for(i=1; i<=n-1; i++)
    {
        rmergeminrv(nx, xx, i, &boxmin, _state);
        rmergemaxrv(nx, xx, i, &boxmax, _state);
    }
    builder.roughdatasetdiameter = (double)(0);
    for(i=0; i<=nx-1; i++)
    {
        builder.roughdatasetdiameter = builder.roughdatasetdiameter+ae_sqr(boxmax.ptr.p_double[i]-boxmin.ptr.p_double[i], _state);
    }
    builder.roughdatasetdiameter = ae_sqrt(builder.roughdatasetdiameter, _state);
    iallocv(n, &builder.wrkidx, _state);
    for(i=0; i<=n-1; i++)
    {
        builder.wrkidx.ptr.p_int[i] = i;
    }
    kdtreebuildtagged(xx, &builder.wrkidx, n, nx, 0, 2, &builder.kdt, _state);
    rbfv3_buildsimplifiedkdtree(xx, n, nx, ae_round(ae_pow((double)(simplificationfactor), (double)(nx), _state), _state), ae_round(ae_pow((double)(5), (double)(nx), _state), _state), &builder.kdt1, _state);
    rbfv3_buildsimplifiedkdtree(xx, n, nx, ae_round(ae_pow((double)(simplificationfactor), (double)(2*nx), _state), _state), ae_round(ae_pow((double)(5), (double)(nx), _state), _state), &builder.kdt2, _state);
    bsetallocv(n, ae_false, &bufferseed.bflags, _state);
    rallocv(nx, &bufferseed.tmpboxmin, _state);
    rallocv(nx, &bufferseed.tmpboxmax, _state);
    kdtreecreaterequestbuffer(&builder.kdt, &bufferseed.kdtbuf, _state);
    kdtreecreaterequestbuffer(&builder.kdt1, &bufferseed.kdt1buf, _state);
    kdtreecreaterequestbuffer(&builder.kdt2, &bufferseed.kdt2buf, _state);
    ae_shared_pool_set_seed(&builder.bufferpool, &bufferseed, (ae_int_t)sizeof(bufferseed), (ae_constructor)_acbfbuffer_init, (ae_copy_constructor)_acbfbuffer_init_copy, (ae_destructor)_acbfbuffer_destroy, _state);
    chunkseed.ntargetrows = -117;
    chunkseed.ntargetcols = -119;
    ae_shared_pool_set_seed(&builder.chunksproducer, &chunkseed, (ae_int_t)sizeof(chunkseed), (ae_constructor)_acbfchunk_init, (ae_copy_constructor)_acbfchunk_init_copy, (ae_destructor)_acbfchunk_destroy, _state);
    ae_shared_pool_set_seed(&builder.chunkspool, &chunkseed, (ae_int_t)sizeof(chunkseed), (ae_constructor)_acbfchunk_init, (ae_copy_constructor)_acbfchunk_init_copy, (ae_destructor)_acbfchunk_destroy, _state);
    
    /*
     * Prepare preconditioner matrix
     */
    rbfv3_computeacbfpreconditionerrecv2(&builder, 0, n, _state);
    isetallocv(n, -1, &rowsizes, _state);
    ae_shared_pool_first_recycled(&builder.chunkspool, &_precchunk, _state);
    while(precchunk!=NULL)
    {
        for(i=0; i<=precchunk->ntargetrows-1; i++)
        {
            ae_assert(rowsizes.ptr.p_int[precchunk->targetrows.ptr.p_int[i]]==-1, "RBFV3: integrity check 2568 failed", _state);
            rowsizes.ptr.p_int[precchunk->targetrows.ptr.p_int[i]] = precchunk->ntargetcols;
        }
        ae_shared_pool_next_recycled(&builder.chunkspool, &_precchunk, _state);
    }
    sp->matrixtype = 1;
    sp->m = n+nx+1;
    sp->n = n+nx+1;
    iallocv(n+nx+2, &sp->ridx, _state);
    sp->ridx.ptr.p_int[0] = 0;
    for(i=0; i<=n-1; i++)
    {
        ae_assert(rowsizes.ptr.p_int[i]>0, "RBFV3: integrity check 2668 failed", _state);
        sp->ridx.ptr.p_int[i+1] = sp->ridx.ptr.p_int[i]+rowsizes.ptr.p_int[i];
    }
    for(i=n; i<=n+nx; i++)
    {
        sp->ridx.ptr.p_int[i+1] = sp->ridx.ptr.p_int[i]+1;
    }
    iallocv(sp->ridx.ptr.p_int[sp->m], &sp->idx, _state);
    rallocv(sp->ridx.ptr.p_int[sp->m], &sp->vals, _state);
    for(i=n; i<=n+nx; i++)
    {
        sp->idx.ptr.p_int[sp->ridx.ptr.p_int[i]] = i;
        sp->vals.ptr.p_double[sp->ridx.ptr.p_int[i]] = 1.0;
    }
    ae_shared_pool_first_recycled(&builder.chunkspool, &_precchunk, _state);
    while(precchunk!=NULL)
    {
        for(i=0; i<=precchunk->ntargetrows-1; i++)
        {
            offs = sp->ridx.ptr.p_int[precchunk->targetrows.ptr.p_int[i]];
            for(j=0; j<=precchunk->ntargetcols-1; j++)
            {
                sp->idx.ptr.p_int[offs+j] = precchunk->targetcols.ptr.p_int[j];
                sp->vals.ptr.p_double[offs+j] = precchunk->s.ptr.pp_double[i][j];
            }
        }
        ae_shared_pool_next_recycled(&builder.chunkspool, &_precchunk, _state);
    }
    sp->ninitialized = sp->ridx.ptr.p_int[sp->m];
    sparseinitduidx(sp, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Basecase initialization routine for DDM solver.


Appends an instance of RBF3DDMSubproblem to Solver.SubproblemsPool.

INPUT PARAMETERS:
    Solver      -   solver object. This function may be  called  from  the
                    multiple threads, so it  is  important  to  work  with
                    Solver object using only thread-safe functions.
    X           -   array[N,NX], dataset points
    N, NX       -   dataset metrics, N>0, NX>0
    BFMatrix    -   basis function matrix object
    LambdaV     -   smoothing parameter
    SP          -   sparse ACBF preconditioner, (N+NX+1)*(N+NX+1) matrix
                    stored in CRS format
    Buf         -   an instance of RBF3DDMBuffer, reusable temporary buffers
    TgtIdx      -   array[], contains indexes of points in the current target
                    set. Elements [Tgt0,Tgt1) are processed by this function.
    NNeighbors  -   neighbors count; NNeighbors nearby nodes are added  to
                    inner points of the chunk
    DoDetailedTrace-whether trace output is needed or not. When trace is
                    activated, solver computes condition numbers. It results
                    in the several-fold slowdown of the algorithm.

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_ddmsolverinitbasecase(rbf3ddmsolver* solver,
     /* Real    */ const ae_matrix* x,
     ae_int_t n,
     ae_int_t nx,
     const rbf3evaluator* bfmatrix,
     double lambdav,
     const sparsematrix* sp,
     rbf3ddmbuffer* buf,
     /* Integer */ ae_vector* tgtidx,
     ae_int_t tgt0,
     ae_int_t tgt1,
     ae_int_t nneighbors,
     ae_bool dodetailedtrace,
     ae_state *_state)
{
    ae_frame _frame_block;
    rbf3ddmsubproblem *subproblem;
    ae_smart_ptr _subproblem;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t nc;
    ae_int_t nk;
    double v;
    double reg;
    ae_int_t nwrk;
    ae_int_t npreccol;
    ae_vector neighbors;
    ae_vector workingnodes;
    ae_vector preccolumns;
    ae_vector tau;
    ae_matrix q;
    ae_vector x0;
    double lurcond;
    ae_bool lusuccess;
    ae_int_t ni;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t jj;
    ae_matrix suba;
    ae_matrix subsp;
    ae_matrix dbga;

    ae_frame_make(_state, &_frame_block);
    memset(&_subproblem, 0, sizeof(_subproblem));
    memset(&neighbors, 0, sizeof(neighbors));
    memset(&workingnodes, 0, sizeof(workingnodes));
    memset(&preccolumns, 0, sizeof(preccolumns));
    memset(&tau, 0, sizeof(tau));
    memset(&q, 0, sizeof(q));
    memset(&x0, 0, sizeof(x0));
    memset(&suba, 0, sizeof(suba));
    memset(&subsp, 0, sizeof(subsp));
    memset(&dbga, 0, sizeof(dbga));
    ae_smart_ptr_init(&_subproblem, (void**)&subproblem, _state, ae_true);
    ae_vector_init(&neighbors, 0, DT_INT, _state, ae_true);
    ae_vector_init(&workingnodes, 0, DT_INT, _state, ae_true);
    ae_vector_init(&preccolumns, 0, DT_INT, _state, ae_true);
    ae_vector_init(&tau, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&q, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&x0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&suba, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&subsp, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&dbga, 0, 0, DT_REAL, _state, ae_true);

    ae_assert(tgt1-tgt0>0, "RBFV3: integrity check 7364 failed", _state);
    ae_assert(nneighbors>=0, "RBFV3: integrity check 7365 failed", _state);
    reg = ((double)100+ae_sqrt((double)(tgt1-tgt0+nneighbors), _state))*ae_machineepsilon;
    
    /*
     * Retrieve fresh subproblem. We expect that Solver.SubproblemsBuffer contains
     * no recycled entries and that fresh subproblem with Subproblem.IsValid=False
     * is returned.
     *
     * Start initialization
     */
    ae_shared_pool_retrieve(&solver->subproblemsbuffer, &_subproblem, _state);
    ae_assert(!subproblem->isvalid, "RBFV3: SubproblemsBuffer integrity check failed", _state);
    subproblem->isvalid = ae_true;
    subproblem->ntarget = tgt1-tgt0;
    iallocv(tgt1-tgt0, &subproblem->targetnodes, _state);
    icopyvx(tgt1-tgt0, tgtidx, tgt0, &subproblem->targetnodes, 0, _state);
    
    /*
     * Prepare working arrays
     */
    rallocv(nx, &x0, _state);
    
    /*
     * Determine working set: target nodes + neighbors of targets.
     * Prepare mapping from node index to position in WorkingNodes[]
     */
    nwrk = 0;
    iallocv(tgt1-tgt0, &workingnodes, _state);
    for(i=tgt0; i<=tgt1-1; i++)
    {
        nk = tgtidx->ptr.p_int[i];
        buf->bflags.ptr.p_bool[nk] = ae_true;
        workingnodes.ptr.p_int[nwrk] = nk;
        nwrk = nwrk+1;
    }
    for(i=tgt0; i<=tgt1-1; i++)
    {
        rcopyrv(nx, x, tgtidx->ptr.p_int[i], &x0, _state);
        nc = kdtreetsqueryknn(&solver->kdt, &buf->kdtbuf, &x0, nneighbors+1, ae_true, _state);
        kdtreetsqueryresultstags(&solver->kdt, &buf->kdtbuf, &neighbors, _state);
        for(k=0; k<=nc-1; k++)
        {
            nk = neighbors.ptr.p_int[k];
            if( !buf->bflags.ptr.p_bool[nk] )
            {
                buf->bflags.ptr.p_bool[nk] = ae_true;
                igrowv(nwrk+1, &workingnodes, _state);
                workingnodes.ptr.p_int[nwrk] = nk;
                nwrk = nwrk+1;
            }
        }
    }
    for(i=0; i<=nwrk-1; i++)
    {
        buf->bflags.ptr.p_bool[workingnodes.ptr.p_int[i]] = ae_false;
    }
    ae_assert(nwrk>0, "ACBF: integrity check for NWrk failed", _state);
    subproblem->nwork = nwrk;
    icopyallocv(nwrk, &workingnodes, &subproblem->workingnodes, _state);
    
    /*
     * Determine preconditioner columns that have nonzeros in rows corresponding
     * to working nodes. Prepare mapping from [0,N+NX+1) column indexing to [0,NPrecCol)
     * compressed one. Only these columns are extracted from the preconditioner
     * during design system computation.
     *
     * NOTE: we ensure that preconditioner columns N...N+NX which correspond to linear
     *       terms are placed last. It greatly simplifies desi
     */
    npreccol = 0;
    for(i=0; i<=nwrk-1; i++)
    {
        j0 = sp->ridx.ptr.p_int[workingnodes.ptr.p_int[i]];
        j1 = sp->ridx.ptr.p_int[workingnodes.ptr.p_int[i]+1]-1;
        for(jj=j0; jj<=j1; jj++)
        {
            j = sp->idx.ptr.p_int[jj];
            if( j<n&&!buf->bflags.ptr.p_bool[j] )
            {
                buf->bflags.ptr.p_bool[j] = ae_true;
                igrowv(npreccol+1, &preccolumns, _state);
                preccolumns.ptr.p_int[npreccol] = j;
                npreccol = npreccol+1;
            }
        }
    }
    for(j=n; j<=n+nx; j++)
    {
        ae_assert(!buf->bflags.ptr.p_bool[j], "RBFV3: integrity check 9435 failed", _state);
        buf->bflags.ptr.p_bool[j] = ae_true;
        igrowv(npreccol+1, &preccolumns, _state);
        preccolumns.ptr.p_int[npreccol] = j;
        npreccol = npreccol+1;
    }
    for(i=0; i<=npreccol-1; i++)
    {
        buf->idx2preccol.ptr.p_int[preccolumns.ptr.p_int[i]] = i;
        buf->bflags.ptr.p_bool[preccolumns.ptr.p_int[i]] = ae_false;
    }
    
    /*
     * Generate working system, apply regularization
     */
    rsetallocm(nwrk, npreccol, 0.0, &suba, _state);
    rbfv3_modelmatrixcomputepartial(bfmatrix, &workingnodes, nwrk, &preccolumns, npreccol-(nx+1), &suba, _state);
    for(i=0; i<=nwrk-1; i++)
    {
        ni = workingnodes.ptr.p_int[i];
        for(j=0; j<=nx-1; j++)
        {
            suba.ptr.pp_double[i][npreccol-(nx+1)+j] = x->ptr.pp_double[ni][j];
        }
        suba.ptr.pp_double[i][npreccol-1] = 1.0;
    }
    for(i=0; i<=nwrk-1; i++)
    {
        j = buf->idx2preccol.ptr.p_int[workingnodes.ptr.p_int[i]];
        suba.ptr.pp_double[i][j] = suba.ptr.pp_double[i][j]+lambdav;
    }
    rsetallocm(nwrk, npreccol, 0.0, &subsp, _state);
    for(i=0; i<=nwrk-1; i++)
    {
        ni = workingnodes.ptr.p_int[i];
        j0 = sp->ridx.ptr.p_int[ni];
        j1 = sp->ridx.ptr.p_int[ni+1]-1;
        for(jj=j0; jj<=j1; jj++)
        {
            subsp.ptr.pp_double[i][buf->idx2preccol.ptr.p_int[sp->idx.ptr.p_int[jj]]] = sp->vals.ptr.p_double[jj];
        }
    }
    rallocm(nwrk, nwrk, &subproblem->regsystem, _state);
    rmatrixgemm(nwrk, nwrk, npreccol, 1.0, &suba, 0, 0, 0, &subsp, 0, 0, 1, 0.0, &subproblem->regsystem, 0, 0, _state);
    
    /*
     * Try solving with LU decomposition
     */
    rcopyallocm(nwrk, nwrk, &subproblem->regsystem, &subproblem->wrklu, _state);
    rmatrixlu(&subproblem->wrklu, nwrk, nwrk, &subproblem->wrkp, _state);
    lurcond = rmatrixlurcondinf(&subproblem->wrklu, nwrk, _state);
    if( ae_fp_greater(lurcond,ae_sqrt(ae_machineepsilon, _state)) )
    {
        
        /*
         * LU success
         */
        subproblem->decomposition = 0;
        lusuccess = ae_true;
        if( dodetailedtrace )
        {
            ae_trace(">> DDM subproblem:  LU success, |target|=%4d,  |wrk|=%4d,  |preccol|=%4d, cond(LU)=%0.2e\n",
                (int)(tgt1-tgt0),
                (int)(nwrk),
                (int)(npreccol),
                (double)((double)1/(lurcond+ae_machineepsilon)));
        }
    }
    else
    {
        lusuccess = ae_false;
    }
    
    /*
     * Apply regularized QR if needed
     */
    if( !lusuccess )
    {
        rsetallocm(2*nwrk, nwrk, 0.0, &subproblem->wrkr, _state);
        rcopym(nwrk, nwrk, &subproblem->regsystem, &subproblem->wrkr, _state);
        v = ae_sqrt(reg, _state);
        for(i=0; i<=nwrk-1; i++)
        {
            subproblem->wrkr.ptr.pp_double[nwrk+i][i] = v;
        }
        rmatrixqr(&subproblem->wrkr, 2*nwrk, nwrk, &tau, _state);
        rmatrixqrunpackq(&subproblem->wrkr, 2*nwrk, nwrk, &tau, nwrk, &subproblem->wrkq, _state);
        subproblem->decomposition = 1;
        if( dodetailedtrace )
        {
            ae_trace(">> DDM subproblem:  LU failure, using reg-QR, |target|=%4d,  |wrk|=%4d,  |preccol|=%4d, cond(R)=%0.2e (cond(LU)=%0.2e)\n",
                (int)(tgt1-tgt0),
                (int)(nwrk),
                (int)(npreccol),
                (double)((double)1/(rmatrixtrrcondinf(&subproblem->wrkr, nwrk, ae_true, ae_false, _state)+ae_machineepsilon)),
                (double)((double)1/(lurcond+ae_machineepsilon)));
        }
    }
    
    /*
     * Subproblem is ready.
     * Move it to the SubproblemsPool
     */
    ae_shared_pool_recycle(&solver->subproblemspool, &_subproblem, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Recursive initialization routine for DDM solver

INPUT PARAMETERS:
    Solver      -   solver structure
    X           -   array[N,NX], dataset points
    N, NX       -   dataset metrics, N>0, NX>0
    BFMatrix    -   basis function evaluator
    LambdaV     -   smoothing parameter
    SP          -   sparse ACBF preconditioner, (N+NX+1)*(N+NX+1) matrix
                    stored in CRS format
    WrkIdx      -   array[], contains indexes of points in the current working
                    set. Elements [Wrk0,Wrk1) are processed by this function.
    NNeighbors  -   neighbors count; NNeighbors nearby nodes are added  to
                    inner points of the chunk
    NBatch      -   batch size
    DoDetailedTrace-whether trace output is needed or not. When trace is
                    activated, solver computes condition numbers. It results
                    in the several-fold slowdown of the algorithm.

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_ddmsolverinitrec(rbf3ddmsolver* solver,
     /* Real    */ const ae_matrix* x,
     ae_int_t n,
     ae_int_t nx,
     const rbf3evaluator* bfmatrix,
     double lambdav,
     const sparsematrix* sp,
     /* Integer */ ae_vector* wrkidx,
     ae_int_t wrk0,
     ae_int_t wrk1,
     ae_int_t nneighbors,
     ae_int_t nbatch,
     ae_bool dodetailedtrace,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k0;
    ae_int_t k1;
    ae_int_t largestdim;
    double splitval;
    double basecasecomplexity;
    rbf3ddmbuffer *buf;
    ae_smart_ptr _buf;

    ae_frame_make(_state, &_frame_block);
    memset(&_buf, 0, sizeof(_buf));
    ae_smart_ptr_init(&_buf, (void**)&buf, _state, ae_true);

    if( wrk1<=wrk0 )
    {
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        ae_frame_leave(_state);
        return;
    }
    basecasecomplexity = rmul3((double)(nbatch+nneighbors+nx+1), (double)(nbatch+nneighbors+nx+1), (double)(nbatch+nneighbors+nx+1), _state);
    
    /*
     * Decide on parallelism
     */
    if( (ae_fp_greater_eq(basecasecomplexity*((double)n/(double)nbatch),smpactivationlevel(_state))&&ae_fp_greater_eq((double)n/(double)nbatch,(double)(2)))&&n>=rbfv3_ddmparallelthreshold )
    {
        if( _trypexec_rbfv3_ddmsolverinitrec(solver,x,n,nx,bfmatrix,lambdav,sp,wrkidx,wrk0,wrk1,nneighbors,nbatch,dodetailedtrace, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            ae_frame_leave(_state);
            return;
        }
    }
    
    /*
     * Retrieve temporary buffer
     */
    ae_shared_pool_retrieve(&solver->bufferpool, &_buf, _state);
    
    /*
     * Analyze current working set
     */
    rallocv(nx, &buf->tmpboxmin, _state);
    rallocv(nx, &buf->tmpboxmax, _state);
    rcopyrv(nx, x, wrkidx->ptr.p_int[wrk0], &buf->tmpboxmin, _state);
    rcopyrv(nx, x, wrkidx->ptr.p_int[wrk0], &buf->tmpboxmax, _state);
    for(i=wrk0+1; i<=wrk1-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            buf->tmpboxmin.ptr.p_double[j] = ae_minreal(buf->tmpboxmin.ptr.p_double[j], x->ptr.pp_double[wrkidx->ptr.p_int[i]][j], _state);
            buf->tmpboxmax.ptr.p_double[j] = ae_maxreal(buf->tmpboxmax.ptr.p_double[j], x->ptr.pp_double[wrkidx->ptr.p_int[i]][j], _state);
        }
    }
    largestdim = 0;
    for(j=1; j<=nx-1; j++)
    {
        if( ae_fp_greater(buf->tmpboxmax.ptr.p_double[j]-buf->tmpboxmin.ptr.p_double[j],buf->tmpboxmax.ptr.p_double[largestdim]-buf->tmpboxmin.ptr.p_double[largestdim]) )
        {
            largestdim = j;
        }
    }
    
    /*
     * Perform either batch processing or recursive split
     */
    if( wrk1-wrk0<=nbatch||ae_fp_eq(buf->tmpboxmax.ptr.p_double[largestdim],buf->tmpboxmin.ptr.p_double[largestdim]) )
    {
        
        /*
         * Either working set size is small enough or all points are non-distinct.
         * Stop recursive subdivision.
         */
        rbfv3_ddmsolverinitbasecase(solver, x, n, nx, bfmatrix, lambdav, sp, buf, wrkidx, wrk0, wrk1, nneighbors, dodetailedtrace, _state);
        
        /*
         * Recycle temporary buffers
         */
        ae_shared_pool_recycle(&solver->bufferpool, &_buf, _state);
    }
    else
    {
        
        /*
         * Compute recursive split along largest axis
         */
        splitval = 0.5*(buf->tmpboxmax.ptr.p_double[largestdim]+buf->tmpboxmin.ptr.p_double[largestdim]);
        k0 = wrk0;
        k1 = wrk1-1;
        while(k0<=k1)
        {
            if( ae_fp_less_eq(x->ptr.pp_double[wrkidx->ptr.p_int[k0]][largestdim],splitval) )
            {
                k0 = k0+1;
                continue;
            }
            if( ae_fp_greater(x->ptr.pp_double[wrkidx->ptr.p_int[k1]][largestdim],splitval) )
            {
                k1 = k1-1;
                continue;
            }
            swapelementsi(wrkidx, k0, k1, _state);
            k0 = k0+1;
            k1 = k1-1;
        }
        ae_assert(k0>wrk0&&k1<wrk1-1, "ACBF: integrity check 2843 in the recursive subdivision code failed", _state);
        ae_assert(k0==k1+1, "ACBF: integrity check 8364 in the recursive subdivision code failed", _state);
        
        /*
         * Recycle temporary buffer, perform recursive calls
         */
        ae_shared_pool_recycle(&solver->bufferpool, &_buf, _state);
        ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_fp_greater_eq(ae_minreal((double)(k0-wrk0), (double)(wrk1-k0), _state)*basecasecomplexity,spawnlevel(_state)), _state);
        _spawn_rbfv3_ddmsolverinitrec(solver, x, n, nx, bfmatrix, lambdav, sp, wrkidx, wrk0, k0, nneighbors, nbatch, dodetailedtrace, _child_tasks, _smp_enabled, _state);
        rbfv3_ddmsolverinitrec(solver, x, n, nx, bfmatrix, lambdav, sp, wrkidx, k0, wrk1, nneighbors, nbatch, dodetailedtrace, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_false, _state);
    }
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_rbfv3_ddmsolverinitrec(rbf3ddmsolver* solver,
    /* Real    */ const ae_matrix* x,
    ae_int_t n,
    ae_int_t nx,
    const rbf3evaluator* bfmatrix,
    double lambdav,
    const sparsematrix* sp,
    /* Integer */ ae_vector* wrkidx,
    ae_int_t wrk0,
    ae_int_t wrk1,
    ae_int_t nneighbors,
    ae_int_t nbatch,
    ae_bool dodetailedtrace,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        rbfv3_ddmsolverinitrec(solver,x,n,nx,bfmatrix,lambdav,sp,wrkidx,wrk0,wrk1,nneighbors,nbatch,dodetailedtrace, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_rbfv3_ddmsolverinitrec;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = solver;
        _task->data.parameters[1].value.const_val = x;
        _task->data.parameters[2].value.ival = n;
        _task->data.parameters[3].value.ival = nx;
        _task->data.parameters[4].value.const_val = bfmatrix;
        _task->data.parameters[5].value.dval = lambdav;
        _task->data.parameters[6].value.const_val = sp;
        _task->data.parameters[7].value.val = wrkidx;
        _task->data.parameters[8].value.ival = wrk0;
        _task->data.parameters[9].value.ival = wrk1;
        _task->data.parameters[10].value.ival = nneighbors;
        _task->data.parameters[11].value.ival = nbatch;
        _task->data.parameters[12].value.bval = dodetailedtrace;
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
void _task_rbfv3_ddmsolverinitrec(ae_task_data *_data, ae_state *_state)
{
    rbf3ddmsolver* solver;
    const ae_matrix* x;
    ae_int_t n;
    ae_int_t nx;
    const rbf3evaluator* bfmatrix;
    double lambdav;
    const sparsematrix* sp;
    ae_vector* wrkidx;
    ae_int_t wrk0;
    ae_int_t wrk1;
    ae_int_t nneighbors;
    ae_int_t nbatch;
    ae_bool dodetailedtrace;
    solver = (rbf3ddmsolver*)_data->parameters[0].value.val;
    x = (const ae_matrix*)_data->parameters[1].value.const_val;
    n = _data->parameters[2].value.ival;
    nx = _data->parameters[3].value.ival;
    bfmatrix = (const rbf3evaluator*)_data->parameters[4].value.const_val;
    lambdav = _data->parameters[5].value.dval;
    sp = (const sparsematrix*)_data->parameters[6].value.const_val;
    wrkidx = (ae_vector*)_data->parameters[7].value.val;
    wrk0 = _data->parameters[8].value.ival;
    wrk1 = _data->parameters[9].value.ival;
    nneighbors = _data->parameters[10].value.ival;
    nbatch = _data->parameters[11].value.ival;
    dodetailedtrace = _data->parameters[12].value.bval;
   ae_state_set_flags(_state, _data->flags);
   rbfv3_ddmsolverinitrec(solver,x,n,nx,bfmatrix,lambdav,sp,wrkidx,wrk0,wrk1,nneighbors,nbatch,dodetailedtrace, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_rbfv3_ddmsolverinitrec(rbf3ddmsolver* solver,
    /* Real    */ const ae_matrix* x,
    ae_int_t n,
    ae_int_t nx,
    const rbf3evaluator* bfmatrix,
    double lambdav,
    const sparsematrix* sp,
    /* Integer */ ae_vector* wrkidx,
    ae_int_t wrk0,
    ae_int_t wrk1,
    ae_int_t nneighbors,
    ae_int_t nbatch,
    ae_bool dodetailedtrace,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_rbfv3_ddmsolverinitrec;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = solver;
    _task->data.parameters[1].value.const_val = x;
    _task->data.parameters[2].value.ival = n;
    _task->data.parameters[3].value.ival = nx;
    _task->data.parameters[4].value.const_val = bfmatrix;
    _task->data.parameters[5].value.dval = lambdav;
    _task->data.parameters[6].value.const_val = sp;
    _task->data.parameters[7].value.val = wrkidx;
    _task->data.parameters[8].value.ival = wrk0;
    _task->data.parameters[9].value.ival = wrk1;
    _task->data.parameters[10].value.ival = nneighbors;
    _task->data.parameters[11].value.ival = nbatch;
    _task->data.parameters[12].value.bval = dodetailedtrace;
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
This function prepares domain decomposition method for  RBF  interpolation
problem  -  it  partitions  problem  into   subproblems   and  precomputes
factorizations, and prepares a smaller correction spline that is  used  to
correct distortions introduced by domain decomposition  and  imperfections
in approximate cardinal basis.

INPUT PARAMETERS:
    X           -   array[N,NX], dataset points
    RescaledBy  -   additional scaling coefficient that was applied to the
                    dataset by preprocessor. Used ONLY for logging purposes
                    - without it all distances will be reported  in  [0,1]
                    scale, not one set by user.
    N, NX       -   dataset metrics, N>0, NX>0
    BFMatrix    -   RBF evaluator
    BFType      -   basis function type
    BFParam     -   basis function parameter
    LambdaV     -   regularization parameter, >=0
    ATerm       -   polynomial term type (1 for linear, 2 for constant, 3 for zero)
    SP          -   sparse ACBF preconditioner, (N+NX+1)*(N+NX+1) matrix
                    stored in CRS format
    NNeighbors  -   neighbors count; NNeighbors nearby nodes are added  to
                    inner points of the batch
    NBatch      -   batch size
    NCorrector  -   nodes count for correction spline
    DoTrace     -   whether low overhead logging is needed or not
    DoDetailedTrace-whether detailed trace output is needed or not. When trace is
                    activated, solver computes condition numbers. It results
                    in the small slowdown of the algorithm.
    
OUTPUT PARAMETERS:
    Solver      -   DDM solver
    timeDDMInit-    time used by the DDM part initialization, ms
    timeCorrInit-   time used by the corrector initialization, ms

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_ddmsolverinit(/* Real    */ const ae_matrix* x,
     double rescaledby,
     ae_int_t n,
     ae_int_t nx,
     const rbf3evaluator* bfmatrix,
     ae_int_t bftype,
     double bfparam,
     double lambdav,
     ae_int_t aterm,
     const sparsematrix* sp,
     ae_int_t nneighbors,
     ae_int_t nbatch,
     ae_int_t ncorrector,
     ae_bool dotrace,
     ae_bool dodetailedtrace,
     rbf3ddmsolver* solver,
     ae_int_t* timeddminit,
     ae_int_t* timecorrinit,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_vector idx;
    rbf3ddmbuffer bufferseed;
    rbf3ddmsubproblem subproblem;
    rbf3ddmsubproblem *p;
    ae_smart_ptr _p;
    double correctorgridseparation;
    ae_matrix corrsys;
    ae_vector corrtau;
    ae_vector idummy;
    double reg;
    ae_int_t nsys;

    ae_frame_make(_state, &_frame_block);
    memset(&idx, 0, sizeof(idx));
    memset(&bufferseed, 0, sizeof(bufferseed));
    memset(&subproblem, 0, sizeof(subproblem));
    memset(&_p, 0, sizeof(_p));
    memset(&corrsys, 0, sizeof(corrsys));
    memset(&corrtau, 0, sizeof(corrtau));
    memset(&idummy, 0, sizeof(idummy));
    *timeddminit = 0;
    *timecorrinit = 0;
    ae_vector_init(&idx, 0, DT_INT, _state, ae_true);
    _rbf3ddmbuffer_init(&bufferseed, _state, ae_true);
    _rbf3ddmsubproblem_init(&subproblem, _state, ae_true);
    ae_smart_ptr_init(&_p, (void**)&p, _state, ae_true);
    ae_matrix_init(&corrsys, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&corrtau, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&idummy, 0, DT_INT, _state, ae_true);

    ae_assert((aterm==1||aterm==2)||aterm==3, "RBF3: integrity check 3320 failed", _state);
    
    /*
     * Start DDM part
     */
    *timeddminit = ae_tickcount();
    
    /*
     * Save problem info
     */
    solver->lambdav = lambdav;
    
    /*
     * Prepare KD-tree
     */
    iallocv(n, &idx, _state);
    for(i=0; i<=n-1; i++)
    {
        idx.ptr.p_int[i] = i;
    }
    kdtreebuildtagged(x, &idx, n, nx, 0, 2, &solver->kdt, _state);
    
    /*
     * Prepare temporary buffer pool
     */
    bsetallocv(n+nx+1, ae_false, &bufferseed.bflags, _state);
    iallocv(n+nx+1, &bufferseed.idx2preccol, _state);
    rallocv(nx, &bufferseed.tmpboxmin, _state);
    rallocv(nx, &bufferseed.tmpboxmax, _state);
    kdtreecreaterequestbuffer(&solver->kdt, &bufferseed.kdtbuf, _state);
    ae_shared_pool_set_seed(&solver->bufferpool, &bufferseed, (ae_int_t)sizeof(bufferseed), (ae_constructor)_rbf3ddmbuffer_init, (ae_copy_constructor)_rbf3ddmbuffer_init_copy, (ae_destructor)_rbf3ddmbuffer_destroy, _state);
    
    /*
     * Prepare default subproblems buffer, run recursive procedure
     * and count subproblems in the buffer
     */
    subproblem.isvalid = ae_false;
    ae_shared_pool_set_seed(&solver->subproblemspool, &subproblem, (ae_int_t)sizeof(subproblem), (ae_constructor)_rbf3ddmsubproblem_init, (ae_copy_constructor)_rbf3ddmsubproblem_init_copy, (ae_destructor)_rbf3ddmsubproblem_destroy, _state);
    ae_shared_pool_set_seed(&solver->subproblemsbuffer, &subproblem, (ae_int_t)sizeof(subproblem), (ae_constructor)_rbf3ddmsubproblem_init, (ae_copy_constructor)_rbf3ddmsubproblem_init_copy, (ae_destructor)_rbf3ddmsubproblem_destroy, _state);
    rbfv3_ddmsolverinitrec(solver, x, n, nx, bfmatrix, solver->lambdav, sp, &idx, 0, n, nneighbors, nbatch, dodetailedtrace, _state);
    solver->subproblemscnt = 0;
    solver->cntlu = 0;
    solver->cntregqr = 0;
    ae_shared_pool_first_recycled(&solver->subproblemspool, &_p, _state);
    while(p!=NULL)
    {
        solver->subproblemscnt = solver->subproblemscnt+1;
        if( p->decomposition==0 )
        {
            inc(&solver->cntlu, _state);
        }
        if( p->decomposition==1 )
        {
            inc(&solver->cntregqr, _state);
        }
        ae_shared_pool_next_recycled(&solver->subproblemspool, &_p, _state);
    }
    ae_assert(solver->cntlu+solver->cntregqr==solver->subproblemscnt, "RBFV3: integrity check 5296 failed", _state);
    ae_assert(solver->subproblemscnt>0, "RBFV3: subproblems pool is empty, critical integrity check failed", _state);
    
    /*
     * DDM part is done
     */
    *timeddminit = ae_tickcount()-(*timeddminit);
    if( dotrace )
    {
        ae_trace("> DDM part was prepared in %0d ms, %0d subproblems solved (%0d well-conditioned, %0d ill-conditioned)\n",
            (int)(*timeddminit),
            (int)(solver->subproblemscnt),
            (int)(solver->cntlu),
            (int)(solver->cntregqr));
    }
    
    /*
     * Prepare correction spline
     */
    *timecorrinit = ae_tickcount();
    rbfv3_selectglobalnodes(x, n, nx, &idummy, 0, ncorrector, &solver->corrnodes, &solver->ncorrector, &correctorgridseparation, _state);
    ncorrector = solver->ncorrector;
    ae_assert(ncorrector>0, "RBFV3: NCorrector=0", _state);
    nsys = ncorrector+nx+1;
    rsetallocm(2*nsys, nsys, 0.0, &corrsys, _state);
    rallocm(ncorrector, nx, &solver->corrx, _state);
    for(i=0; i<=ncorrector-1; i++)
    {
        rcopyrr(nx, x, solver->corrnodes.ptr.p_int[i], &solver->corrx, i, _state);
    }
    rbfv3_computebfmatrix(&solver->corrx, ncorrector, nx, bftype, bfparam, &corrsys, _state);
    if( aterm==1 )
    {
        
        /*
         * Use linear term
         */
        for(i=0; i<=nx-1; i++)
        {
            for(j=0; j<=ncorrector-1; j++)
            {
                corrsys.ptr.pp_double[ncorrector+i][j] = x->ptr.pp_double[solver->corrnodes.ptr.p_int[j]][i];
                corrsys.ptr.pp_double[j][ncorrector+i] = x->ptr.pp_double[solver->corrnodes.ptr.p_int[j]][i];
            }
        }
        for(j=0; j<=ncorrector-1; j++)
        {
            corrsys.ptr.pp_double[ncorrector+nx][j] = 1.0;
            corrsys.ptr.pp_double[j][ncorrector+nx] = 1.0;
        }
    }
    if( aterm==2 )
    {
        
        /*
         * Use constant term
         */
        for(i=0; i<=nx-1; i++)
        {
            corrsys.ptr.pp_double[ncorrector+i][ncorrector+i] = 1.0;
        }
        for(j=0; j<=ncorrector-1; j++)
        {
            corrsys.ptr.pp_double[ncorrector+nx][j] = 1.0;
            corrsys.ptr.pp_double[j][ncorrector+nx] = 1.0;
        }
    }
    if( aterm==3 )
    {
        
        /*
         * Use zero term
         */
        for(i=0; i<=nx; i++)
        {
            corrsys.ptr.pp_double[ncorrector+i][ncorrector+i] = 1.0;
        }
    }
    for(j=0; j<=ncorrector-1; j++)
    {
        corrsys.ptr.pp_double[j][j] = corrsys.ptr.pp_double[j][j]+solver->lambdav;
    }
    reg = 1.0;
    for(i=0; i<=nsys-1; i++)
    {
        reg = ae_maxreal(reg, rmaxabsr(nsys, &corrsys, i, _state), _state);
    }
    reg = ae_sqrt(ae_machineepsilon, _state)*reg;
    for(j=0; j<=nsys-1; j++)
    {
        corrsys.ptr.pp_double[nsys+j][j] = reg;
    }
    rmatrixqr(&corrsys, 2*nsys, nsys, &corrtau, _state);
    rmatrixqrunpackq(&corrsys, 2*nsys, nsys, &corrtau, nsys, &solver->corrq, _state);
    rmatrixqrunpackr(&corrsys, 2*nsys, nsys, &solver->corrr, _state);
    *timecorrinit = ae_tickcount()-(*timecorrinit);
    if( dotrace )
    {
        ae_trace("> Corrector spline was prepared in %0d ms (%0d nodes, max distance from dataset points to nearest grid node is %0.2e)\n",
            (int)(*timecorrinit),
            (int)(ncorrector),
            (double)(correctorgridseparation*rescaledby));
    }
    if( dodetailedtrace )
    {
        ae_trace("> printing condition numbers for correction spline:\n");
        ae_trace("cond(A)     = %0.2e (Linf norm, leading NCoarsexNCoarse block)\n",
            (double)((double)1/(rmatrixtrrcondinf(&solver->corrr, ncorrector, ae_true, ae_false, _state)+ae_machineepsilon)));
        ae_trace("cond(A)     = %0.2e (Linf norm, full system)\n",
            (double)((double)1/(rmatrixtrrcondinf(&solver->corrr, nsys, ae_true, ae_false, _state)+ae_machineepsilon)));
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Recursive subroutine for DDM method. Given initial subproblems count  Cnt,
it perform two recursive calls (spawns children in parallel when possible)
with Cnt~Cnt/2 until we end up with Cnt=1.

Case with Cnt=1 is handled by retrieving subproblem from Solver.SubproblemsPool,
solving it and pushing subproblem to Solver.SubproblemsBuffer.

INPUT PARAMETERS:
    Solver      -   DDM solver object
    Res         -   array[N,NY], current residuals
    N, NX, NY   -   dataset metrics, N>0, NX>0, NY>0
    C           -   preallocated array[N+NX+1,NY]
    Cnt         -   number of subproblems to process
    
OUTPUT PARAMETERS:
    C           -   rows 0..N-1  contain spline coefficients
                    rows N..N+NX are filled by zeros

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_ddmsolverrunrec(rbf3ddmsolver* solver,
     /* Real    */ const ae_matrix* res,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t ny,
     /* Real    */ ae_matrix* c,
     ae_int_t cnt,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t nwrk;
    ae_int_t ntarget;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double v;
    rbf3ddmsubproblem *subproblem;
    ae_smart_ptr _subproblem;

    ae_frame_make(_state, &_frame_block);
    memset(&_subproblem, 0, sizeof(_subproblem));
    ae_smart_ptr_init(&_subproblem, (void**)&subproblem, _state, ae_true);

    
    /*
     * Run recursive procedure if needed
     */
    if( cnt>1 )
    {
        k = cnt/2;
        ae_assert(k<=cnt-k, "RBFV3: integrity check 2733 failed", _state);
        rbfv3_ddmsolverrunrec(solver, res, n, nx, ny, c, cnt-k, _state);
        rbfv3_ddmsolverrunrec(solver, res, n, nx, ny, c, k, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Retrieve subproblem from the source pool, solve it
     */
    ae_shared_pool_retrieve(&solver->subproblemspool, &_subproblem, _state);
    ae_assert(subproblem!=NULL&&subproblem->isvalid, "RBFV3: integrity check 1742 failed", _state);
    nwrk = subproblem->nwork;
    ntarget = subproblem->ntarget;
    if( subproblem->decomposition==0 )
    {
        
        /*
         * Solve using LU decomposition (the fastest option)
         */
        rallocm(nwrk, ny, &subproblem->rhs, _state);
        for(i=0; i<=nwrk-1; i++)
        {
            for(j=0; j<=ny-1; j++)
            {
                subproblem->rhs.ptr.pp_double[i][j] = res->ptr.pp_double[subproblem->workingnodes.ptr.p_int[i]][j];
            }
        }
        for(i=0; i<=nwrk-1; i++)
        {
            if( subproblem->wrkp.ptr.p_int[i]!=i )
            {
                for(j=0; j<=ny-1; j++)
                {
                    v = subproblem->rhs.ptr.pp_double[i][j];
                    subproblem->rhs.ptr.pp_double[i][j] = subproblem->rhs.ptr.pp_double[subproblem->wrkp.ptr.p_int[i]][j];
                    subproblem->rhs.ptr.pp_double[subproblem->wrkp.ptr.p_int[i]][j] = v;
                }
            }
        }
        rmatrixlefttrsm(nwrk, ny, &subproblem->wrklu, 0, 0, ae_false, ae_true, 0, &subproblem->rhs, 0, 0, _state);
        rmatrixlefttrsm(nwrk, ny, &subproblem->wrklu, 0, 0, ae_true, ae_false, 0, &subproblem->rhs, 0, 0, _state);
        rcopyallocm(nwrk, ny, &subproblem->rhs, &subproblem->sol, _state);
    }
    else
    {
        
        /*
         * Solve using regularized QR (well, we tried LU but it failed)
         */
        ae_assert(subproblem->decomposition==1, "RBFV3: integrity check 1743 failed", _state);
        rallocm(nwrk, ny, &subproblem->rhs, _state);
        for(i=0; i<=nwrk-1; i++)
        {
            for(j=0; j<=ny-1; j++)
            {
                subproblem->rhs.ptr.pp_double[i][j] = res->ptr.pp_double[subproblem->workingnodes.ptr.p_int[i]][j];
            }
        }
        rallocm(nwrk, ny, &subproblem->qtrhs, _state);
        rmatrixgemm(nwrk, ny, nwrk, 1.0, &subproblem->wrkq, 0, 0, 1, &subproblem->rhs, 0, 0, 0, 0.0, &subproblem->qtrhs, 0, 0, _state);
        rmatrixlefttrsm(nwrk, ny, &subproblem->wrkr, 0, 0, ae_true, ae_false, 0, &subproblem->qtrhs, 0, 0, _state);
        rcopyallocm(nwrk, ny, &subproblem->qtrhs, &subproblem->sol, _state);
    }
    for(i=0; i<=ntarget-1; i++)
    {
        for(j=0; j<=ny-1; j++)
        {
            c->ptr.pp_double[subproblem->targetnodes.ptr.p_int[i]][j] = subproblem->sol.ptr.pp_double[i][j];
        }
    }
    
    /*
     * Push to the destination pool
     */
    ae_shared_pool_recycle(&solver->subproblemsbuffer, &_subproblem, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function APPROXIMATELY solves RBF interpolation problem using  domain
decomposition method. Given current residuals Res, it computes approximate
basis function coefficients C (but does  NOT  compute linear  coefficients
- these are set to zero).

This function is a linear operator with respect to its input RES, thus  it
can be used as a preconditioner for an iterative linear solver like GMRES.

INPUT PARAMETERS:
    Solver      -   DDM solver object
    Res         -   array[N,NY], current residuals
    N, NX, NY   -   dataset metrics, N>0, NX>0, NY>0
    SP          -   preconditioner, (N+NX+1)*(N+NX+1) sparse matrix
    BFMatrix    -   basis functions evaluator
    C           -   preallocated array[N+NX+1,NY]
    timeDDMSolve,
    timeCorrSolve-  on input contain already accumulated timings
                    for DDM and CORR parts
    
OUTPUT PARAMETERS:
    C           -   rows 0..N-1  contain spline coefficients
                    rows N..N+NX are filled by zeros
    timeDDMSolve,
    timeCorrSolve-  updated with new timings
    

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_ddmsolverrun(rbf3ddmsolver* solver,
     /* Real    */ const ae_matrix* res,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t ny,
     const sparsematrix* sp,
     rbf3evaluator* bfmatrix,
     rbf3fastevaluator* fasteval,
     double fastevaltol,
     /* Real    */ ae_matrix* upd,
     ae_int_t* timeddmsolve,
     ae_int_t* timecorrsolve,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    rbf3ddmsubproblem *subproblem;
    ae_smart_ptr _subproblem;
    ae_matrix c;
    ae_vector x0;
    ae_vector x1;
    ae_vector refrhs1;
    ae_matrix corrpred;
    ae_matrix updt;

    ae_frame_make(_state, &_frame_block);
    memset(&_subproblem, 0, sizeof(_subproblem));
    memset(&c, 0, sizeof(c));
    memset(&x0, 0, sizeof(x0));
    memset(&x1, 0, sizeof(x1));
    memset(&refrhs1, 0, sizeof(refrhs1));
    memset(&corrpred, 0, sizeof(corrpred));
    memset(&updt, 0, sizeof(updt));
    ae_smart_ptr_init(&_subproblem, (void**)&subproblem, _state, ae_true);
    ae_matrix_init(&c, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&x0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&x1, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&refrhs1, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&corrpred, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&updt, 0, 0, DT_REAL, _state, ae_true);

    rsetallocm(ny, n+nx+1, 0.0, &updt, _state);
    rsetallocm(n+nx+1, ny, 0.0, &c, _state);
    for(j=0; j<=ny-1; j++)
    {
        for(i=n; i<=n+nx; i++)
        {
            c.ptr.pp_double[i][j] = (double)(0);
        }
    }
    
    /*
     * Solve DDM part:
     * * run recursive procedure that computes DDM part.
     * * clean-up: move processed subproblems from Solver.SubproblemsBuffer back to Solver.SubproblemsPool
     * * multiply solution by the preconditioner matrix
     */
    *timeddmsolve = *timeddmsolve-ae_tickcount();
    rbfv3_ddmsolverrunrec(solver, res, n, nx, ny, &c, solver->subproblemscnt, _state);
    for(i=0; i<=solver->subproblemscnt-1; i++)
    {
        ae_shared_pool_retrieve(&solver->subproblemsbuffer, &_subproblem, _state);
        ae_assert(subproblem!=NULL&&subproblem->isvalid, "RBFV3: integrity check 5223 failed", _state);
        ae_shared_pool_recycle(&solver->subproblemspool, &_subproblem, _state);
    }
    *timeddmsolve = *timeddmsolve+ae_tickcount();
    rallocv(n+nx+1, &x0, _state);
    rallocv(n+nx+1, &x1, _state);
    for(j=0; j<=ny-1; j++)
    {
        rcopycv(n+nx+1, &c, j, &x0, _state);
        sparsegemv(sp, 1.0, 1, &x0, 0, 0.0, &x1, 0, _state);
        rcopyvr(n+nx+1, &x1, &updt, j, _state);
    }
    
    /*
     * Compute correction spline that fixes oscillations introduced by the DDM part
     */
    *timecorrsolve = *timecorrsolve-ae_tickcount();
    rallocv(solver->ncorrector+nx+1, &x0, _state);
    rallocv(n+nx+1, &x1, _state);
    for(j=0; j<=ny-1; j++)
    {
        
        /*
         * Prepare right-hand side for the QR solver
         */
        rsetallocm(1, solver->ncorrector+nx+1, 0.0, &corrpred, _state);
        rsetallocv(solver->ncorrector+nx+1, 0.0, &refrhs1, _state);
        rcopyrv(n+nx+1, &updt, j, &x1, _state);
        rbfv3_fastevaluatorloadcoeffs1(fasteval, &x1, _state);
        rbfv3_fastevaluatorpushtol(fasteval, fastevaltol, _state);
        rbfv3_fastevaluatorcomputebatch(fasteval, &solver->corrx, solver->ncorrector, ae_true, &corrpred, _state);
        for(i=0; i<=solver->ncorrector-1; i++)
        {
            refrhs1.ptr.p_double[i] = res->ptr.pp_double[solver->corrnodes.ptr.p_int[i]][j]-corrpred.ptr.pp_double[0][i];
            for(k=0; k<=nx-1; k++)
            {
                refrhs1.ptr.p_double[i] = refrhs1.ptr.p_double[i]-solver->corrx.ptr.pp_double[i][k]*x1.ptr.p_double[n+k];
            }
            refrhs1.ptr.p_double[i] = refrhs1.ptr.p_double[i]-x1.ptr.p_double[n+nx];
            refrhs1.ptr.p_double[i] = refrhs1.ptr.p_double[i]-solver->lambdav*x1.ptr.p_double[solver->corrnodes.ptr.p_int[i]];
        }
        
        /*
         * Solve QR-factorized system
         */
        rgemv(solver->ncorrector+nx+1, solver->ncorrector+nx+1, 1.0, &solver->corrq, 1, &refrhs1, 0.0, &x0, _state);
        rmatrixtrsv(solver->ncorrector+nx+1, &solver->corrr, 0, 0, ae_true, ae_false, 0, &x0, 0, _state);
        for(i=0; i<=solver->ncorrector-1; i++)
        {
            updt.ptr.pp_double[j][solver->corrnodes.ptr.p_int[i]] = updt.ptr.pp_double[j][solver->corrnodes.ptr.p_int[i]]+x0.ptr.p_double[i];
        }
        for(i=0; i<=nx; i++)
        {
            updt.ptr.pp_double[j][n+i] = updt.ptr.pp_double[j][n+i]+x0.ptr.p_double[solver->ncorrector+i];
        }
    }
    *timecorrsolve = *timecorrsolve+ae_tickcount();
    rallocm(n+nx+1, ny, upd, _state);
    rmatrixtranspose(ny, n+nx+1, &updt, 0, 0, upd, 0, 0, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function is a specialized version of DDMSolverRun() for NY=1.

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_ddmsolverrun1(rbf3ddmsolver* solver,
     /* Real    */ const ae_vector* res,
     ae_int_t n,
     ae_int_t nx,
     const sparsematrix* sp,
     rbf3evaluator* bfmatrix,
     rbf3fastevaluator* fasteval,
     double fastevaltol,
     /* Real    */ ae_vector* upd,
     ae_int_t* timeddmsolve,
     ae_int_t* timecorrsolve,
     ae_state *_state)
{


    rallocm(n, 1, &solver->tmpres1, _state);
    rcopyvc(n, res, &solver->tmpres1, 0, _state);
    rbfv3_ddmsolverrun(solver, &solver->tmpres1, n, nx, 1, sp, bfmatrix, fasteval, fastevaltol, &solver->tmpupd1, timeddmsolve, timecorrsolve, _state);
    rallocv(n+nx+1, upd, _state);
    rcopycv(n+nx+1, &solver->tmpupd1, 0, upd, _state);
}


/*************************************************************************
Automatically detect scale parameter as a mean distance towards nearest
neighbor (not counting nearest neighbors that are too close)

PARAMETERS:
    XX                  -   dataset (X-values), array[N,NX]
    N                   -   points count, N>=1
    NX                  -   dimensions count, NX>=1
    
RESULT:
    suggested scale

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static double rbfv3_autodetectscaleparameter(/* Real    */ const ae_matrix* xx,
     ae_int_t n,
     ae_int_t nx,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t nq;
    ae_int_t nlocal;
    kdtree kdt;
    ae_vector x;
    ae_vector d;
    double result;

    ae_frame_make(_state, &_frame_block);
    memset(&kdt, 0, sizeof(kdt));
    memset(&x, 0, sizeof(x));
    memset(&d, 0, sizeof(d));
    _kdtree_init(&kdt, _state, ae_true);
    ae_vector_init(&x, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&d, 0, DT_REAL, _state, ae_true);

    ae_assert(n>=1, "RBFV3: integrity check 7624 failed", _state);
    rallocv(nx, &x, _state);
    kdtreebuild(xx, n, nx, 0, 2, &kdt, _state);
    nlocal = ae_round(ae_pow((double)(2), (double)(nx), _state)+(double)1, _state);
    result = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        
        /*
         * Query a batch of nearest neighbors
         */
        rcopyrv(nx, xx, i, &x, _state);
        nq = kdtreequeryknn(&kdt, &x, nlocal, ae_true, _state);
        ae_assert(nq>=1, "RBFV3: integrity check 7625 failed", _state);
        kdtreequeryresultsdistances(&kdt, &d, _state);
        
        /*
         * In order to filter out nearest neighbors that are too close,
         * we use distance R toward most distant of NQ nearest neighbors as
         * a reference and select nearest neighbor with distance >=0.5*R/NQ
         */
        for(j=0; j<=nq-1; j++)
        {
            if( ae_fp_greater_eq(d.ptr.p_double[j],0.5*d.ptr.p_double[nq-1]/(double)nq) )
            {
                result = result+d.ptr.p_double[j];
                break;
            }
        }
    }
    result = result/(double)n;
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Recursive functions matrix computation

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_computebfmatrixrec(/* Real    */ const ae_matrix* xx,
     ae_int_t range0,
     ae_int_t range1,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t functype,
     double funcparam,
     /* Real    */ ae_matrix* f,
     ae_state *_state)
{
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double v;
    double vv;
    double elemcost;
    double alpha2;


    ae_assert((functype==1||functype==2)||functype==3, "RBFV3.ComputeTransposedDesignSystem: unexpected FuncType", _state);
    
    /*
     * Try to parallelize
     */
    elemcost = 10.0;
    if( ((range0==0&&range1==n)&&ae_fp_greater_eq(0.5*rmul3((double)(n), (double)(n), elemcost, _state),smpactivationlevel(_state)))&&n>=rbfv3_bfparallelthreshold )
    {
        if( _trypexec_rbfv3_computebfmatrixrec(xx,range0,range1,n,nx,functype,funcparam,f, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    
    /*
     * Try recursive splits
     */
    if( range1-range0>16 )
    {
        k = range0+(range1-range0)/2;
        ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_fp_greater_eq(rmul3((double)(range1-range0), (double)(n-k), elemcost, _state),spawnlevel(_state)), _state);
        _spawn_rbfv3_computebfmatrixrec(xx, range0, k, n, nx, functype, funcparam, f, _child_tasks, _smp_enabled, _state);
        rbfv3_computebfmatrixrec(xx, k, range1, n, nx, functype, funcparam, f, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Serial processing
     */
    alpha2 = funcparam*funcparam;
    for(i=range0; i<=range1-1; i++)
    {
        for(j=i; j<=n-1; j++)
        {
            v = (double)(0);
            for(k=0; k<=nx-1; k++)
            {
                vv = xx->ptr.pp_double[i][k]-xx->ptr.pp_double[j][k];
                v = v+vv*vv;
            }
            if( functype==1 )
            {
                v = -ae_sqrt(v+alpha2, _state);
            }
            if( functype==2 )
            {
                if( v!=0.0 )
                {
                    v = v*0.5*ae_log(v, _state);
                }
                else
                {
                    v = 0.0;
                }
            }
            if( functype==3 )
            {
                v = v*ae_sqrt(v, _state);
            }
            f->ptr.pp_double[i][j] = v;
            f->ptr.pp_double[j][i] = v;
        }
    }
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_rbfv3_computebfmatrixrec(/* Real    */ const ae_matrix* xx,
    ae_int_t range0,
    ae_int_t range1,
    ae_int_t n,
    ae_int_t nx,
    ae_int_t functype,
    double funcparam,
    /* Real    */ ae_matrix* f,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        rbfv3_computebfmatrixrec(xx,range0,range1,n,nx,functype,funcparam,f, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_rbfv3_computebfmatrixrec;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.const_val = xx;
        _task->data.parameters[1].value.ival = range0;
        _task->data.parameters[2].value.ival = range1;
        _task->data.parameters[3].value.ival = n;
        _task->data.parameters[4].value.ival = nx;
        _task->data.parameters[5].value.ival = functype;
        _task->data.parameters[6].value.dval = funcparam;
        _task->data.parameters[7].value.val = f;
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
void _task_rbfv3_computebfmatrixrec(ae_task_data *_data, ae_state *_state)
{
    const ae_matrix* xx;
    ae_int_t range0;
    ae_int_t range1;
    ae_int_t n;
    ae_int_t nx;
    ae_int_t functype;
    double funcparam;
    ae_matrix* f;
    xx = (const ae_matrix*)_data->parameters[0].value.const_val;
    range0 = _data->parameters[1].value.ival;
    range1 = _data->parameters[2].value.ival;
    n = _data->parameters[3].value.ival;
    nx = _data->parameters[4].value.ival;
    functype = _data->parameters[5].value.ival;
    funcparam = _data->parameters[6].value.dval;
    f = (ae_matrix*)_data->parameters[7].value.val;
   ae_state_set_flags(_state, _data->flags);
   rbfv3_computebfmatrixrec(xx,range0,range1,n,nx,functype,funcparam,f, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_rbfv3_computebfmatrixrec(/* Real    */ const ae_matrix* xx,
    ae_int_t range0,
    ae_int_t range1,
    ae_int_t n,
    ae_int_t nx,
    ae_int_t functype,
    double funcparam,
    /* Real    */ ae_matrix* f,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_rbfv3_computebfmatrixrec;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.const_val = xx;
    _task->data.parameters[1].value.ival = range0;
    _task->data.parameters[2].value.ival = range1;
    _task->data.parameters[3].value.ival = n;
    _task->data.parameters[4].value.ival = nx;
    _task->data.parameters[5].value.ival = functype;
    _task->data.parameters[6].value.dval = funcparam;
    _task->data.parameters[7].value.val = f;
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
This function computes  basis  functions  matrix  (both  upper  and  lower
triangles)


      [ f(dist(x0,x0))     ... f(dist(x0,x(n-1)))     ]
      [ f(dist(x1,x0))     ... f(dist(x1,x(n-1)))     ]
      [ ............................................. ]
      [ f(dist(x(n-1),x0)) ... f(dist(x(n-1),x(n-1))) ]
      
NOTE: if F is large enough to store result, it is not reallocated. Values
      outside of [0,N)x[0,N) range are not modified.

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_computebfmatrix(/* Real    */ const ae_matrix* xx,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t functype,
     double funcparam,
     /* Real    */ ae_matrix* f,
     ae_state *_state)
{


    rallocm(n, n, f, _state);
    rbfv3_computebfmatrixrec(xx, 0, n, n, nx, functype, funcparam, f, _state);
}


/*************************************************************************
Initializes model matrix using specified matrix storage format:
* StorageType=0     a N*N matrix of basis function values is stored 
* StorageType=1     basis function values are recomputed on demand

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_modelmatrixinit(/* Real    */ const ae_matrix* xx,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t functype,
     double funcparam,
     ae_int_t storagetype,
     rbf3evaluator* modelmatrix,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t nchunks;
    ae_int_t i;
    ae_int_t j;
    ae_int_t srcoffs;
    ae_int_t dstoffs;
    ae_int_t curlen;
    rbf3evaluatorbuffer bufseed;

    ae_frame_make(_state, &_frame_block);
    memset(&bufseed, 0, sizeof(bufseed));
    _rbf3evaluator_clear(modelmatrix);
    _rbf3evaluatorbuffer_init(&bufseed, _state, ae_true);

    ae_assert(storagetype==0||storagetype==1, "RBFV3: unexpected StorageType for ModelMatrixInit()", _state);
    modelmatrix->n = n;
    modelmatrix->storagetype = storagetype;
    if( storagetype==0 )
    {
        rbfv3_computebfmatrix(xx, n, nx, functype, funcparam, &modelmatrix->f, _state);
        ae_frame_leave(_state);
        return;
    }
    if( storagetype==1 )
    {
        
        /*
         * Save model parameters
         */
        modelmatrix->nx = nx;
        modelmatrix->functype = functype;
        modelmatrix->funcparam = funcparam;
        modelmatrix->chunksize = 128;
        
        /*
         * Prepare temporary buffers
         */
        ae_shared_pool_set_seed(&modelmatrix->bufferpool, &bufseed, (ae_int_t)sizeof(bufseed), (ae_constructor)_rbf3evaluatorbuffer_init, (ae_copy_constructor)_rbf3evaluatorbuffer_init_copy, (ae_destructor)_rbf3evaluatorbuffer_destroy, _state);
        rsetallocv(modelmatrix->chunksize, 1.0, &modelmatrix->chunk1, _state);
        
        /*
         * Store dataset in the chunked row storage format (rows with size at most ChunkSize, one row per dimension/chunk)
         */
        iallocv(n, &modelmatrix->entireset, _state);
        for(i=0; i<=n-1; i++)
        {
            modelmatrix->entireset.ptr.p_int[i] = i;
        }
        rcopyallocm(n, nx, xx, &modelmatrix->x, _state);
        nchunks = idivup(n, modelmatrix->chunksize, _state);
        rsetallocm(nchunks*nx, modelmatrix->chunksize, 0.0, &modelmatrix->xtchunked, _state);
        srcoffs = 0;
        dstoffs = 0;
        while(srcoffs<n)
        {
            curlen = ae_minint(modelmatrix->chunksize, n-srcoffs, _state);
            for(i=0; i<=curlen-1; i++)
            {
                for(j=0; j<=nx-1; j++)
                {
                    modelmatrix->xtchunked.ptr.pp_double[dstoffs+j][i] = xx->ptr.pp_double[srcoffs+i][j];
                }
            }
            srcoffs = srcoffs+curlen;
            dstoffs = dstoffs+nx;
        }
        ae_frame_leave(_state);
        return;
    }
    ae_assert(ae_false, "ModelMatrixInit: integrity check failed", _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Computes subset of the model matrix (subset of rows, subset of columns) and
writes result to R.

NOTE: If R is longer than M0xM1, it is not reallocated and additional elements
      are not modified.

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_modelmatrixcomputepartial(const rbf3evaluator* modelmatrix,
     /* Integer */ const ae_vector* ridx,
     ae_int_t m0,
     /* Integer */ const ae_vector* cidx,
     ae_int_t m1,
     /* Real    */ ae_matrix* r,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t ni;
    ae_int_t nj;
    double v;
    double vv;


    ae_assert(modelmatrix->storagetype==0||modelmatrix->storagetype==1, "ModelMatrixComputePartial: unexpected StorageType", _state);
    rallocm(m0, m1, r, _state);
    if( modelmatrix->storagetype==0 )
    {
        for(i=0; i<=m0-1; i++)
        {
            ni = ridx->ptr.p_int[i];
            for(j=0; j<=m1-1; j++)
            {
                r->ptr.pp_double[i][j] = modelmatrix->f.ptr.pp_double[ni][cidx->ptr.p_int[j]];
            }
        }
        return;
    }
    if( modelmatrix->storagetype==1 )
    {
        ae_assert(modelmatrix->functype==1||modelmatrix->functype==2, "ModelMatrixComputePartial: unexpected FuncType", _state);
        for(i=0; i<=m0-1; i++)
        {
            ni = ridx->ptr.p_int[i];
            for(j=0; j<=m1-1; j++)
            {
                nj = cidx->ptr.p_int[j];
                v = (double)(0);
                if( modelmatrix->functype==1 )
                {
                    v = modelmatrix->funcparam*modelmatrix->funcparam;
                }
                if( modelmatrix->functype==2 )
                {
                    v = 1.0E-50;
                }
                for(k=0; k<=modelmatrix->nx-1; k++)
                {
                    vv = modelmatrix->x.ptr.pp_double[ni][k]-modelmatrix->x.ptr.pp_double[nj][k];
                    v = v+vv*vv;
                }
                if( modelmatrix->functype==1 )
                {
                    v = -ae_sqrt(v, _state);
                }
                if( modelmatrix->functype==2 )
                {
                    v = v*0.5*ae_log(v, _state);
                }
                r->ptr.pp_double[i][j] = v;
            }
        }
        return;
    }
    ae_assert(ae_false, "ModelMatrixComputePartial: integrity check failed", _state);
}


/*************************************************************************
This function computes ChunkSize basis function values and stores them in
the evaluator buffer. This function does not modify Evaluator object, thus
it can be used in multiple threads with the same evaluator as long as different
buffers are used.

INPUT PARAMETERS:
    Evaluator       -   evaluator object
    X               -   origin point
    Buf             -   preallocated buffers. Following fields are used and
                        must have at least ChunkSize elements:
                        * Buf.FuncBuf
                        * Buf.WrkBuf
                        When NeedGradInfo>=1, additionally we need
                        the following fields to be preallocated:
                        * Buf.MinDist2 - array[ChunkSize], filled by some
                          positive values; on the very first call it is 1.0E50
                          or something comparably large
                        * Buf.DeltaBuf - array[NX,ChunkSize]
                        * Buf.DF1 - array[ChunkSize]
                        When NeedGradInfo>=2, additionally we need
                        the following fields to be preallocated:
                        * Buf.DF2 - array[ChunkSize]
    ChunkSize       -   amount of basis functions to compute,
                        0<ChunkSize<=Evaluator.ChunkSize
    ChunkIdx        -   index of the chunk in Evaluator.XTChunked times NX
    Distance0       -   strictly positive value that is added to the
                        squared distance prior to passing it to the multiquadric
                        kernel function. For other kernels - set it to small
                        nonnegative value like 1.0E-50.
    NeedGradInfo    -   whether gradient-related information is needed or
                        not:
                        * if 0, only FuncBuf is set on exit
                        * if 1, MinDist2, DeltaBuf and DF1 are also set on exit
                        * if 2, additionally DF2 is set on exit
    
OUTPUT PARAMETERS:
    Buf.FuncBuf     -   array[ChunkSize], basis function values
    Buf.MinDist2    -   array[ChunkSize], if NeedGradInfo>=1 then its
                        I-th element is updated as MinDist2[I]:=min(MinDist2[I],DISTANCE_SQUARED(X,CENTER[I]))
    Buf.DeltaBuf    -   array[NX,ChunkSize], if NeedGradInfo>=1 then
                        J-th element of K-th row is set to X[K]-CENTER[J,K]
    Buf.DF1         -   array[ChunkSize], if NeedGradInfo>=1 then
                        J-th element is derivative of the kernel function
                        with respect to its input (squared distance)
    Buf.DF2         -   array[ChunkSize], if NeedGradInfo>=2 then
                        J-th element is derivative of the kernel function
                        with respect to its input (squared distance)
    
  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static void rbfv3_computerowchunk(const rbf3evaluator* evaluator,
     /* Real    */ const ae_vector* x,
     rbf3evaluatorbuffer* buf,
     ae_int_t chunksize,
     ae_int_t chunkidx,
     double distance0,
     ae_int_t needgradinfo,
     ae_state *_state)
{
    ae_int_t k;
    double r2;
    double lnr;


    
    /*
     * Compute squared distance in Buf.FuncBuf
     */
    rsetv(chunksize, distance0, &buf->funcbuf, _state);
    for(k=0; k<=evaluator->nx-1; k++)
    {
        rsetv(chunksize, x->ptr.p_double[k], &buf->wrkbuf, _state);
        raddrv(chunksize, -1.0, &evaluator->xtchunked, chunkidx+k, &buf->wrkbuf, _state);
        rmuladdv(chunksize, &buf->wrkbuf, &buf->wrkbuf, &buf->funcbuf, _state);
        if( needgradinfo>=1 )
        {
            rcopyvr(chunksize, &buf->wrkbuf, &buf->deltabuf, k, _state);
        }
    }
    if( needgradinfo>=1 )
    {
        rmergeminv(chunksize, &buf->funcbuf, &buf->mindist2, _state);
    }
    
    /*
     * Apply kernel function
     */
    if( evaluator->functype==1 )
    {
        
        /*
         * f=-sqrt(r^2+alpha^2), including f=-r as a special case
         */
        if( needgradinfo==0 )
        {
            
            /*
             * Only target f(r2)=-sqrt(r2) is needed
             */
            rsqrtv(chunksize, &buf->funcbuf, _state);
            rmulv(chunksize, -1.0, &buf->funcbuf, _state);
        }
        if( needgradinfo==1 )
        {
            
            /*
             * First derivative is needed:
             *
             * f(r2)   = -sqrt(r2)
             * f'(r2)  = -0.5/sqrt(r2)
             *
             * NOTE: FuncBuf[] is always positive due to small correction added,
             *       thus we have no need to handle zero value as a special case
             */
            rsqrtv(chunksize, &buf->funcbuf, _state);
            rmulv(chunksize, -1.0, &buf->funcbuf, _state);
            rsetv(chunksize, 0.5, &buf->df1, _state);
            rmergedivv(chunksize, &buf->funcbuf, &buf->df1, _state);
        }
        if( needgradinfo==2 )
        {
            
            /*
             * Second derivatives is needed:
             *
             * f(r2)   = -sqrt(r2+alpha2)
             * f'(r2)  = -0.5/sqrt(r2+alpha2)
             * f''(r2) =  0.25/((r2+alpha2)^(3/2))
             *
             * NOTE: FuncBuf[] is always positive due to small correction added,
             *       thus we have no need to handle zero value as a special case
             */
            rcopymulv(chunksize, -2.0, &buf->funcbuf, &buf->wrkbuf, _state);
            rsqrtv(chunksize, &buf->funcbuf, _state);
            rmulv(chunksize, -1.0, &buf->funcbuf, _state);
            rsetv(chunksize, 0.5, &buf->df1, _state);
            rmergedivv(chunksize, &buf->funcbuf, &buf->df1, _state);
            rcopyv(chunksize, &buf->df1, &buf->df2, _state);
            rmergedivv(chunksize, &buf->wrkbuf, &buf->df2, _state);
        }
        return;
    }
    if( evaluator->functype==2 )
    {
        
        /*
         * f=r^2*ln(r)
         *
         * NOTE: FuncBuf[] is always positive due to small correction added,
         *       thus we have no need to handle ln(0) as a special case.
         */
        if( needgradinfo==0 )
        {
            
            /*
             * No gradient info is required
             *
             * NOTE: FuncBuf[] is always positive due to small correction added,
             *       thus we have no need to handle zero value as a special case
             */
            for(k=0; k<=chunksize-1; k++)
            {
                buf->funcbuf.ptr.p_double[k] = buf->funcbuf.ptr.p_double[k]*0.5*ae_log(buf->funcbuf.ptr.p_double[k], _state);
            }
        }
        if( needgradinfo==1 )
        {
            
            /*
             * First derivative is needed:
             *
             * f(r2)  = 0.5*r2*ln(r2)
             * f'(r2) = 0.5*ln(r2) + 0.5 = 0.5*(ln(r2)+1) =ln(r)+0.5
             *
             * NOTE: FuncBuf[] is always positive due to small correction added,
             *       thus we have no need to handle zero value as a special case
             */
            for(k=0; k<=chunksize-1; k++)
            {
                r2 = buf->funcbuf.ptr.p_double[k];
                lnr = 0.5*ae_log(r2, _state);
                buf->funcbuf.ptr.p_double[k] = r2*lnr;
                buf->df1.ptr.p_double[k] = lnr+0.5;
            }
        }
        if( needgradinfo==2 )
        {
            
            /*
             * Second derivative is needed:
             *
             * f(r2)  = 0.5*r2*ln(r2)
             * f'(r2) = 0.5*ln(r2) + 0.5 = 0.5*(ln(r2)+1) =ln(r)+0.5
             * f''(r2)= 0.5/r2
             *
             * NOTE: FuncBuf[] is always positive due to small correction added,
             *       thus we have no need to handle zero value as a special case
             */
            for(k=0; k<=chunksize-1; k++)
            {
                r2 = buf->funcbuf.ptr.p_double[k];
                lnr = 0.5*ae_log(r2, _state);
                buf->funcbuf.ptr.p_double[k] = r2*lnr;
                buf->df1.ptr.p_double[k] = lnr+0.5;
                buf->df2.ptr.p_double[k] = 0.5/r2;
            }
        }
        return;
    }
    ae_assert(ae_false, "RBFV3: unexpected FuncType in ComputeRowChunk()", _state);
}


/*************************************************************************
Checks whether basis function is conditionally positive definite or not,
given the polynomial term type (ATerm=1 means linear, ATerm=2 means constant,
ATerm=3 means no polynomial term).

  -- ALGLIB --
     Copyright 12.12.2021 by Sergey Bochkanov
*************************************************************************/
static ae_bool rbfv3_iscpdfunction(ae_int_t functype,
     ae_int_t aterm,
     ae_state *_state)
{
    ae_bool result;


    ae_assert((aterm==1||aterm==2)||aterm==3, "RBFV3: integrity check 3563 failed", _state);
    result = ae_false;
    if( functype==1 )
    {
        result = aterm==2||aterm==1;
        return result;
    }
    if( functype==2 )
    {
        result = aterm==1;
        return result;
    }
    ae_assert(ae_false, "IsCPDFunction: unexpected FuncType", _state);
    return result;
}


void _rbf3evaluatorbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    rbf3evaluatorbuffer *p = (rbf3evaluatorbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->y, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->coeffbuf, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->funcbuf, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->wrkbuf, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->mindist2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->df1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->df2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->y2, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->deltabuf, 0, 0, DT_REAL, _state, make_automatic);
}


void _rbf3evaluatorbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    rbf3evaluatorbuffer       *dst = (rbf3evaluatorbuffer*)_dst;
    const rbf3evaluatorbuffer *src = (const rbf3evaluatorbuffer*)_src;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->y, &src->y, _state, make_automatic);
    ae_vector_init_copy(&dst->coeffbuf, &src->coeffbuf, _state, make_automatic);
    ae_vector_init_copy(&dst->funcbuf, &src->funcbuf, _state, make_automatic);
    ae_vector_init_copy(&dst->wrkbuf, &src->wrkbuf, _state, make_automatic);
    ae_vector_init_copy(&dst->mindist2, &src->mindist2, _state, make_automatic);
    ae_vector_init_copy(&dst->df1, &src->df1, _state, make_automatic);
    ae_vector_init_copy(&dst->df2, &src->df2, _state, make_automatic);
    ae_vector_init_copy(&dst->x2, &src->x2, _state, make_automatic);
    ae_vector_init_copy(&dst->y2, &src->y2, _state, make_automatic);
    ae_matrix_init_copy(&dst->deltabuf, &src->deltabuf, _state, make_automatic);
}


void _rbf3evaluatorbuffer_clear(void* _p)
{
    rbf3evaluatorbuffer *p = (rbf3evaluatorbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->y);
    ae_vector_clear(&p->coeffbuf);
    ae_vector_clear(&p->funcbuf);
    ae_vector_clear(&p->wrkbuf);
    ae_vector_clear(&p->mindist2);
    ae_vector_clear(&p->df1);
    ae_vector_clear(&p->df2);
    ae_vector_clear(&p->x2);
    ae_vector_clear(&p->y2);
    ae_matrix_clear(&p->deltabuf);
}


void _rbf3evaluatorbuffer_destroy(void* _p)
{
    rbf3evaluatorbuffer *p = (rbf3evaluatorbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->y);
    ae_vector_destroy(&p->coeffbuf);
    ae_vector_destroy(&p->funcbuf);
    ae_vector_destroy(&p->wrkbuf);
    ae_vector_destroy(&p->mindist2);
    ae_vector_destroy(&p->df1);
    ae_vector_destroy(&p->df2);
    ae_vector_destroy(&p->x2);
    ae_vector_destroy(&p->y2);
    ae_matrix_destroy(&p->deltabuf);
}


void _rbf3panel_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    rbf3panel *p = (rbf3panel*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->clustercenter, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->ptidx, 0, DT_INT, _state, make_automatic);
    ae_matrix_init(&p->xt, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->wt, 0, 0, DT_REAL, _state, make_automatic);
    _biharmonicpanel_init(&p->bhexpansion, _state, make_automatic);
    _rbf3evaluatorbuffer_init(&p->tgtbuf, _state, make_automatic);
}


void _rbf3panel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    rbf3panel       *dst = (rbf3panel*)_dst;
    const rbf3panel *src = (const rbf3panel*)_src;
    dst->paneltype = src->paneltype;
    dst->clusterrad = src->clusterrad;
    ae_vector_init_copy(&dst->clustercenter, &src->clustercenter, _state, make_automatic);
    dst->c0 = src->c0;
    dst->c1 = src->c1;
    dst->c2 = src->c2;
    dst->c3 = src->c3;
    dst->farfieldexpansion = src->farfieldexpansion;
    dst->farfielddistance = src->farfielddistance;
    dst->idx0 = src->idx0;
    dst->idx1 = src->idx1;
    dst->childa = src->childa;
    dst->childb = src->childb;
    ae_vector_init_copy(&dst->ptidx, &src->ptidx, _state, make_automatic);
    ae_matrix_init_copy(&dst->xt, &src->xt, _state, make_automatic);
    ae_matrix_init_copy(&dst->wt, &src->wt, _state, make_automatic);
    _biharmonicpanel_init_copy(&dst->bhexpansion, &src->bhexpansion, _state, make_automatic);
    _rbf3evaluatorbuffer_init_copy(&dst->tgtbuf, &src->tgtbuf, _state, make_automatic);
}


void _rbf3panel_clear(void* _p)
{
    rbf3panel *p = (rbf3panel*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->clustercenter);
    ae_vector_clear(&p->ptidx);
    ae_matrix_clear(&p->xt);
    ae_matrix_clear(&p->wt);
    _biharmonicpanel_clear(&p->bhexpansion);
    _rbf3evaluatorbuffer_clear(&p->tgtbuf);
}


void _rbf3panel_destroy(void* _p)
{
    rbf3panel *p = (rbf3panel*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->clustercenter);
    ae_vector_destroy(&p->ptidx);
    ae_matrix_destroy(&p->xt);
    ae_matrix_destroy(&p->wt);
    _biharmonicpanel_destroy(&p->bhexpansion);
    _rbf3evaluatorbuffer_destroy(&p->tgtbuf);
}


void _rbf3fastevaluator_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    rbf3fastevaluator *p = (rbf3fastevaluator*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_init(&p->permx, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->origptidx, 0, DT_INT, _state, make_automatic);
    ae_matrix_init(&p->wstoredorig, 0, 0, DT_REAL, _state, make_automatic);
    ae_obj_array_init(&p->panels, _state, make_automatic);
    _biharmonicevaluator_init(&p->bheval, _state, make_automatic);
    ae_shared_pool_init(&p->bufferpool, _state, make_automatic);
    ae_matrix_init(&p->tmpx3w, 0, 0, DT_REAL, _state, make_automatic);
}


void _rbf3fastevaluator_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    rbf3fastevaluator       *dst = (rbf3fastevaluator*)_dst;
    const rbf3fastevaluator *src = (const rbf3fastevaluator*)_src;
    dst->n = src->n;
    dst->nx = src->nx;
    dst->ny = src->ny;
    dst->maxpanelsize = src->maxpanelsize;
    dst->functype = src->functype;
    dst->funcparam = src->funcparam;
    ae_matrix_init_copy(&dst->permx, &src->permx, _state, make_automatic);
    ae_vector_init_copy(&dst->origptidx, &src->origptidx, _state, make_automatic);
    ae_matrix_init_copy(&dst->wstoredorig, &src->wstoredorig, _state, make_automatic);
    dst->isloaded = src->isloaded;
    ae_obj_array_init_copy(&dst->panels, &src->panels, _state, make_automatic);
    _biharmonicevaluator_init_copy(&dst->bheval, &src->bheval, _state, make_automatic);
    ae_shared_pool_init_copy(&dst->bufferpool, &src->bufferpool, _state, make_automatic);
    ae_matrix_init_copy(&dst->tmpx3w, &src->tmpx3w, _state, make_automatic);
    dst->usedebugcounters = src->usedebugcounters;
    dst->dbgpanel2panelcnt = src->dbgpanel2panelcnt;
    dst->dbgfield2panelcnt = src->dbgfield2panelcnt;
    dst->dbgpanelscnt = src->dbgpanelscnt;
}


void _rbf3fastevaluator_clear(void* _p)
{
    rbf3fastevaluator *p = (rbf3fastevaluator*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_clear(&p->permx);
    ae_vector_clear(&p->origptidx);
    ae_matrix_clear(&p->wstoredorig);
    ae_obj_array_clear(&p->panels);
    _biharmonicevaluator_clear(&p->bheval);
    ae_shared_pool_clear(&p->bufferpool);
    ae_matrix_clear(&p->tmpx3w);
}


void _rbf3fastevaluator_destroy(void* _p)
{
    rbf3fastevaluator *p = (rbf3fastevaluator*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_destroy(&p->permx);
    ae_vector_destroy(&p->origptidx);
    ae_matrix_destroy(&p->wstoredorig);
    ae_obj_array_destroy(&p->panels);
    _biharmonicevaluator_destroy(&p->bheval);
    ae_shared_pool_destroy(&p->bufferpool);
    ae_matrix_destroy(&p->tmpx3w);
}


void _rbf3evaluator_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    rbf3evaluator *p = (rbf3evaluator*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_init(&p->f, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->entireset, 0, DT_INT, _state, make_automatic);
    ae_matrix_init(&p->x, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->xtchunked, 0, 0, DT_REAL, _state, make_automatic);
    ae_shared_pool_init(&p->bufferpool, _state, make_automatic);
    ae_vector_init(&p->chunk1, 0, DT_REAL, _state, make_automatic);
}


void _rbf3evaluator_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    rbf3evaluator       *dst = (rbf3evaluator*)_dst;
    const rbf3evaluator *src = (const rbf3evaluator*)_src;
    dst->n = src->n;
    dst->storagetype = src->storagetype;
    ae_matrix_init_copy(&dst->f, &src->f, _state, make_automatic);
    dst->nx = src->nx;
    dst->functype = src->functype;
    dst->funcparam = src->funcparam;
    dst->chunksize = src->chunksize;
    ae_vector_init_copy(&dst->entireset, &src->entireset, _state, make_automatic);
    ae_matrix_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_matrix_init_copy(&dst->xtchunked, &src->xtchunked, _state, make_automatic);
    ae_shared_pool_init_copy(&dst->bufferpool, &src->bufferpool, _state, make_automatic);
    ae_vector_init_copy(&dst->chunk1, &src->chunk1, _state, make_automatic);
}


void _rbf3evaluator_clear(void* _p)
{
    rbf3evaluator *p = (rbf3evaluator*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_clear(&p->f);
    ae_vector_clear(&p->entireset);
    ae_matrix_clear(&p->x);
    ae_matrix_clear(&p->xtchunked);
    ae_shared_pool_clear(&p->bufferpool);
    ae_vector_clear(&p->chunk1);
}


void _rbf3evaluator_destroy(void* _p)
{
    rbf3evaluator *p = (rbf3evaluator*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_destroy(&p->f);
    ae_vector_destroy(&p->entireset);
    ae_matrix_destroy(&p->x);
    ae_matrix_destroy(&p->xtchunked);
    ae_shared_pool_destroy(&p->bufferpool);
    ae_vector_destroy(&p->chunk1);
}


void _rbfv3calcbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    rbfv3calcbuffer *p = (rbfv3calcbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    _rbf3evaluatorbuffer_init(&p->evalbuf, _state, make_automatic);
    ae_vector_init(&p->x123, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->y123, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->x2d, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->y2d, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xg, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->yg, 0, DT_REAL, _state, make_automatic);
}


void _rbfv3calcbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    rbfv3calcbuffer       *dst = (rbfv3calcbuffer*)_dst;
    const rbfv3calcbuffer *src = (const rbfv3calcbuffer*)_src;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    _rbf3evaluatorbuffer_init_copy(&dst->evalbuf, &src->evalbuf, _state, make_automatic);
    ae_vector_init_copy(&dst->x123, &src->x123, _state, make_automatic);
    ae_vector_init_copy(&dst->y123, &src->y123, _state, make_automatic);
    ae_matrix_init_copy(&dst->x2d, &src->x2d, _state, make_automatic);
    ae_matrix_init_copy(&dst->y2d, &src->y2d, _state, make_automatic);
    ae_vector_init_copy(&dst->xg, &src->xg, _state, make_automatic);
    ae_vector_init_copy(&dst->yg, &src->yg, _state, make_automatic);
}


void _rbfv3calcbuffer_clear(void* _p)
{
    rbfv3calcbuffer *p = (rbfv3calcbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x);
    _rbf3evaluatorbuffer_clear(&p->evalbuf);
    ae_vector_clear(&p->x123);
    ae_vector_clear(&p->y123);
    ae_matrix_clear(&p->x2d);
    ae_matrix_clear(&p->y2d);
    ae_vector_clear(&p->xg);
    ae_vector_clear(&p->yg);
}


void _rbfv3calcbuffer_destroy(void* _p)
{
    rbfv3calcbuffer *p = (rbfv3calcbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x);
    _rbf3evaluatorbuffer_destroy(&p->evalbuf);
    ae_vector_destroy(&p->x123);
    ae_vector_destroy(&p->y123);
    ae_matrix_destroy(&p->x2d);
    ae_matrix_destroy(&p->y2d);
    ae_vector_destroy(&p->xg);
    ae_vector_destroy(&p->yg);
}


void _acbfbuilder_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    acbfbuilder *p = (acbfbuilder*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_init(&p->xx, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->globalgrid, 0, DT_INT, _state, make_automatic);
    _kdtree_init(&p->kdt, _state, make_automatic);
    _kdtree_init(&p->kdt1, _state, make_automatic);
    _kdtree_init(&p->kdt2, _state, make_automatic);
    ae_shared_pool_init(&p->bufferpool, _state, make_automatic);
    ae_shared_pool_init(&p->chunksproducer, _state, make_automatic);
    ae_shared_pool_init(&p->chunkspool, _state, make_automatic);
    ae_vector_init(&p->wrkidx, 0, DT_INT, _state, make_automatic);
}


void _acbfbuilder_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    acbfbuilder       *dst = (acbfbuilder*)_dst;
    const acbfbuilder *src = (const acbfbuilder*)_src;
    dst->dodetailedtrace = src->dodetailedtrace;
    dst->ntotal = src->ntotal;
    dst->nx = src->nx;
    ae_matrix_init_copy(&dst->xx, &src->xx, _state, make_automatic);
    dst->functype = src->functype;
    dst->funcparam = src->funcparam;
    dst->roughdatasetdiameter = src->roughdatasetdiameter;
    dst->nglobal = src->nglobal;
    ae_vector_init_copy(&dst->globalgrid, &src->globalgrid, _state, make_automatic);
    dst->globalgridseparation = src->globalgridseparation;
    dst->nlocal = src->nlocal;
    dst->ncorrection = src->ncorrection;
    dst->correctorgrowth = src->correctorgrowth;
    dst->batchsize = src->batchsize;
    dst->lambdav = src->lambdav;
    dst->aterm = src->aterm;
    _kdtree_init_copy(&dst->kdt, &src->kdt, _state, make_automatic);
    _kdtree_init_copy(&dst->kdt1, &src->kdt1, _state, make_automatic);
    _kdtree_init_copy(&dst->kdt2, &src->kdt2, _state, make_automatic);
    ae_shared_pool_init_copy(&dst->bufferpool, &src->bufferpool, _state, make_automatic);
    ae_shared_pool_init_copy(&dst->chunksproducer, &src->chunksproducer, _state, make_automatic);
    ae_shared_pool_init_copy(&dst->chunkspool, &src->chunkspool, _state, make_automatic);
    ae_vector_init_copy(&dst->wrkidx, &src->wrkidx, _state, make_automatic);
}


void _acbfbuilder_clear(void* _p)
{
    acbfbuilder *p = (acbfbuilder*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_clear(&p->xx);
    ae_vector_clear(&p->globalgrid);
    _kdtree_clear(&p->kdt);
    _kdtree_clear(&p->kdt1);
    _kdtree_clear(&p->kdt2);
    ae_shared_pool_clear(&p->bufferpool);
    ae_shared_pool_clear(&p->chunksproducer);
    ae_shared_pool_clear(&p->chunkspool);
    ae_vector_clear(&p->wrkidx);
}


void _acbfbuilder_destroy(void* _p)
{
    acbfbuilder *p = (acbfbuilder*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_destroy(&p->xx);
    ae_vector_destroy(&p->globalgrid);
    _kdtree_destroy(&p->kdt);
    _kdtree_destroy(&p->kdt1);
    _kdtree_destroy(&p->kdt2);
    ae_shared_pool_destroy(&p->bufferpool);
    ae_shared_pool_destroy(&p->chunksproducer);
    ae_shared_pool_destroy(&p->chunkspool);
    ae_vector_destroy(&p->wrkidx);
}


void _acbfbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    acbfbuffer *p = (acbfbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->bflags, 0, DT_BOOL, _state, make_automatic);
    _kdtreerequestbuffer_init(&p->kdtbuf, _state, make_automatic);
    _kdtreerequestbuffer_init(&p->kdt1buf, _state, make_automatic);
    _kdtreerequestbuffer_init(&p->kdt2buf, _state, make_automatic);
    ae_vector_init(&p->tmpboxmin, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpboxmax, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->currentnodes, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->neighbors, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->chosenneighbors, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->y, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->z, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->atwrk, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->xq, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->q, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->q1, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->wrkq, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->b, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->c, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->choltmp, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tau, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->r, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->perm, 0, DT_INT, _state, make_automatic);
}


void _acbfbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    acbfbuffer       *dst = (acbfbuffer*)_dst;
    const acbfbuffer *src = (const acbfbuffer*)_src;
    ae_vector_init_copy(&dst->bflags, &src->bflags, _state, make_automatic);
    _kdtreerequestbuffer_init_copy(&dst->kdtbuf, &src->kdtbuf, _state, make_automatic);
    _kdtreerequestbuffer_init_copy(&dst->kdt1buf, &src->kdt1buf, _state, make_automatic);
    _kdtreerequestbuffer_init_copy(&dst->kdt2buf, &src->kdt2buf, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpboxmin, &src->tmpboxmin, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpboxmax, &src->tmpboxmax, _state, make_automatic);
    ae_vector_init_copy(&dst->currentnodes, &src->currentnodes, _state, make_automatic);
    ae_vector_init_copy(&dst->neighbors, &src->neighbors, _state, make_automatic);
    ae_vector_init_copy(&dst->chosenneighbors, &src->chosenneighbors, _state, make_automatic);
    ae_vector_init_copy(&dst->y, &src->y, _state, make_automatic);
    ae_vector_init_copy(&dst->z, &src->z, _state, make_automatic);
    ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic);
    ae_matrix_init_copy(&dst->atwrk, &src->atwrk, _state, make_automatic);
    ae_matrix_init_copy(&dst->xq, &src->xq, _state, make_automatic);
    ae_matrix_init_copy(&dst->q, &src->q, _state, make_automatic);
    ae_matrix_init_copy(&dst->q1, &src->q1, _state, make_automatic);
    ae_matrix_init_copy(&dst->wrkq, &src->wrkq, _state, make_automatic);
    ae_matrix_init_copy(&dst->b, &src->b, _state, make_automatic);
    ae_matrix_init_copy(&dst->c, &src->c, _state, make_automatic);
    ae_vector_init_copy(&dst->choltmp, &src->choltmp, _state, make_automatic);
    ae_vector_init_copy(&dst->tau, &src->tau, _state, make_automatic);
    ae_matrix_init_copy(&dst->r, &src->r, _state, make_automatic);
    ae_vector_init_copy(&dst->perm, &src->perm, _state, make_automatic);
}


void _acbfbuffer_clear(void* _p)
{
    acbfbuffer *p = (acbfbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->bflags);
    _kdtreerequestbuffer_clear(&p->kdtbuf);
    _kdtreerequestbuffer_clear(&p->kdt1buf);
    _kdtreerequestbuffer_clear(&p->kdt2buf);
    ae_vector_clear(&p->tmpboxmin);
    ae_vector_clear(&p->tmpboxmax);
    ae_vector_clear(&p->currentnodes);
    ae_vector_clear(&p->neighbors);
    ae_vector_clear(&p->chosenneighbors);
    ae_vector_clear(&p->y);
    ae_vector_clear(&p->z);
    ae_vector_clear(&p->d);
    ae_matrix_clear(&p->atwrk);
    ae_matrix_clear(&p->xq);
    ae_matrix_clear(&p->q);
    ae_matrix_clear(&p->q1);
    ae_matrix_clear(&p->wrkq);
    ae_matrix_clear(&p->b);
    ae_matrix_clear(&p->c);
    ae_vector_clear(&p->choltmp);
    ae_vector_clear(&p->tau);
    ae_matrix_clear(&p->r);
    ae_vector_clear(&p->perm);
}


void _acbfbuffer_destroy(void* _p)
{
    acbfbuffer *p = (acbfbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->bflags);
    _kdtreerequestbuffer_destroy(&p->kdtbuf);
    _kdtreerequestbuffer_destroy(&p->kdt1buf);
    _kdtreerequestbuffer_destroy(&p->kdt2buf);
    ae_vector_destroy(&p->tmpboxmin);
    ae_vector_destroy(&p->tmpboxmax);
    ae_vector_destroy(&p->currentnodes);
    ae_vector_destroy(&p->neighbors);
    ae_vector_destroy(&p->chosenneighbors);
    ae_vector_destroy(&p->y);
    ae_vector_destroy(&p->z);
    ae_vector_destroy(&p->d);
    ae_matrix_destroy(&p->atwrk);
    ae_matrix_destroy(&p->xq);
    ae_matrix_destroy(&p->q);
    ae_matrix_destroy(&p->q1);
    ae_matrix_destroy(&p->wrkq);
    ae_matrix_destroy(&p->b);
    ae_matrix_destroy(&p->c);
    ae_vector_destroy(&p->choltmp);
    ae_vector_destroy(&p->tau);
    ae_matrix_destroy(&p->r);
    ae_vector_destroy(&p->perm);
}


void _acbfchunk_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    acbfchunk *p = (acbfchunk*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->targetrows, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->targetcols, 0, DT_INT, _state, make_automatic);
    ae_matrix_init(&p->s, 0, 0, DT_REAL, _state, make_automatic);
}


void _acbfchunk_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    acbfchunk       *dst = (acbfchunk*)_dst;
    const acbfchunk *src = (const acbfchunk*)_src;
    dst->ntargetrows = src->ntargetrows;
    dst->ntargetcols = src->ntargetcols;
    ae_vector_init_copy(&dst->targetrows, &src->targetrows, _state, make_automatic);
    ae_vector_init_copy(&dst->targetcols, &src->targetcols, _state, make_automatic);
    ae_matrix_init_copy(&dst->s, &src->s, _state, make_automatic);
}


void _acbfchunk_clear(void* _p)
{
    acbfchunk *p = (acbfchunk*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->targetrows);
    ae_vector_clear(&p->targetcols);
    ae_matrix_clear(&p->s);
}


void _acbfchunk_destroy(void* _p)
{
    acbfchunk *p = (acbfchunk*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->targetrows);
    ae_vector_destroy(&p->targetcols);
    ae_matrix_destroy(&p->s);
}


void _rbf3ddmbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    rbf3ddmbuffer *p = (rbf3ddmbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->bflags, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->idx2preccol, 0, DT_INT, _state, make_automatic);
    _kdtreerequestbuffer_init(&p->kdtbuf, _state, make_automatic);
    ae_vector_init(&p->tmpboxmin, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpboxmax, 0, DT_REAL, _state, make_automatic);
}


void _rbf3ddmbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    rbf3ddmbuffer       *dst = (rbf3ddmbuffer*)_dst;
    const rbf3ddmbuffer *src = (const rbf3ddmbuffer*)_src;
    ae_vector_init_copy(&dst->bflags, &src->bflags, _state, make_automatic);
    ae_vector_init_copy(&dst->idx2preccol, &src->idx2preccol, _state, make_automatic);
    _kdtreerequestbuffer_init_copy(&dst->kdtbuf, &src->kdtbuf, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpboxmin, &src->tmpboxmin, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpboxmax, &src->tmpboxmax, _state, make_automatic);
}


void _rbf3ddmbuffer_clear(void* _p)
{
    rbf3ddmbuffer *p = (rbf3ddmbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->bflags);
    ae_vector_clear(&p->idx2preccol);
    _kdtreerequestbuffer_clear(&p->kdtbuf);
    ae_vector_clear(&p->tmpboxmin);
    ae_vector_clear(&p->tmpboxmax);
}


void _rbf3ddmbuffer_destroy(void* _p)
{
    rbf3ddmbuffer *p = (rbf3ddmbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->bflags);
    ae_vector_destroy(&p->idx2preccol);
    _kdtreerequestbuffer_destroy(&p->kdtbuf);
    ae_vector_destroy(&p->tmpboxmin);
    ae_vector_destroy(&p->tmpboxmax);
}


void _rbf3ddmsubproblem_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    rbf3ddmsubproblem *p = (rbf3ddmsubproblem*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->targetnodes, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->workingnodes, 0, DT_INT, _state, make_automatic);
    ae_matrix_init(&p->regsystem, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->wrklu, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->rhs, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->qtrhs, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->sol, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->pred, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->wrkp, 0, DT_INT, _state, make_automatic);
    ae_matrix_init(&p->wrkq, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->wrkr, 0, 0, DT_REAL, _state, make_automatic);
}


void _rbf3ddmsubproblem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    rbf3ddmsubproblem       *dst = (rbf3ddmsubproblem*)_dst;
    const rbf3ddmsubproblem *src = (const rbf3ddmsubproblem*)_src;
    dst->isvalid = src->isvalid;
    dst->ntarget = src->ntarget;
    ae_vector_init_copy(&dst->targetnodes, &src->targetnodes, _state, make_automatic);
    dst->nwork = src->nwork;
    ae_vector_init_copy(&dst->workingnodes, &src->workingnodes, _state, make_automatic);
    ae_matrix_init_copy(&dst->regsystem, &src->regsystem, _state, make_automatic);
    dst->decomposition = src->decomposition;
    ae_matrix_init_copy(&dst->wrklu, &src->wrklu, _state, make_automatic);
    ae_matrix_init_copy(&dst->rhs, &src->rhs, _state, make_automatic);
    ae_matrix_init_copy(&dst->qtrhs, &src->qtrhs, _state, make_automatic);
    ae_matrix_init_copy(&dst->sol, &src->sol, _state, make_automatic);
    ae_matrix_init_copy(&dst->pred, &src->pred, _state, make_automatic);
    ae_vector_init_copy(&dst->wrkp, &src->wrkp, _state, make_automatic);
    ae_matrix_init_copy(&dst->wrkq, &src->wrkq, _state, make_automatic);
    ae_matrix_init_copy(&dst->wrkr, &src->wrkr, _state, make_automatic);
}


void _rbf3ddmsubproblem_clear(void* _p)
{
    rbf3ddmsubproblem *p = (rbf3ddmsubproblem*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->targetnodes);
    ae_vector_clear(&p->workingnodes);
    ae_matrix_clear(&p->regsystem);
    ae_matrix_clear(&p->wrklu);
    ae_matrix_clear(&p->rhs);
    ae_matrix_clear(&p->qtrhs);
    ae_matrix_clear(&p->sol);
    ae_matrix_clear(&p->pred);
    ae_vector_clear(&p->wrkp);
    ae_matrix_clear(&p->wrkq);
    ae_matrix_clear(&p->wrkr);
}


void _rbf3ddmsubproblem_destroy(void* _p)
{
    rbf3ddmsubproblem *p = (rbf3ddmsubproblem*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->targetnodes);
    ae_vector_destroy(&p->workingnodes);
    ae_matrix_destroy(&p->regsystem);
    ae_matrix_destroy(&p->wrklu);
    ae_matrix_destroy(&p->rhs);
    ae_matrix_destroy(&p->qtrhs);
    ae_matrix_destroy(&p->sol);
    ae_matrix_destroy(&p->pred);
    ae_vector_destroy(&p->wrkp);
    ae_matrix_destroy(&p->wrkq);
    ae_matrix_destroy(&p->wrkr);
}


void _rbf3ddmsolver_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    rbf3ddmsolver *p = (rbf3ddmsolver*)_p;
    ae_touch_ptr((void*)p);
    _kdtree_init(&p->kdt, _state, make_automatic);
    ae_shared_pool_init(&p->bufferpool, _state, make_automatic);
    ae_shared_pool_init(&p->subproblemspool, _state, make_automatic);
    ae_shared_pool_init(&p->subproblemsbuffer, _state, make_automatic);
    ae_matrix_init(&p->corrq, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->corrr, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->corrnodes, 0, DT_INT, _state, make_automatic);
    ae_matrix_init(&p->corrx, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->tmpres1, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->tmpupd1, 0, 0, DT_REAL, _state, make_automatic);
}


void _rbf3ddmsolver_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    rbf3ddmsolver       *dst = (rbf3ddmsolver*)_dst;
    const rbf3ddmsolver *src = (const rbf3ddmsolver*)_src;
    dst->lambdav = src->lambdav;
    _kdtree_init_copy(&dst->kdt, &src->kdt, _state, make_automatic);
    ae_shared_pool_init_copy(&dst->bufferpool, &src->bufferpool, _state, make_automatic);
    dst->subproblemscnt = src->subproblemscnt;
    ae_shared_pool_init_copy(&dst->subproblemspool, &src->subproblemspool, _state, make_automatic);
    ae_shared_pool_init_copy(&dst->subproblemsbuffer, &src->subproblemsbuffer, _state, make_automatic);
    dst->ncorrector = src->ncorrector;
    ae_matrix_init_copy(&dst->corrq, &src->corrq, _state, make_automatic);
    ae_matrix_init_copy(&dst->corrr, &src->corrr, _state, make_automatic);
    ae_vector_init_copy(&dst->corrnodes, &src->corrnodes, _state, make_automatic);
    ae_matrix_init_copy(&dst->corrx, &src->corrx, _state, make_automatic);
    ae_matrix_init_copy(&dst->tmpres1, &src->tmpres1, _state, make_automatic);
    ae_matrix_init_copy(&dst->tmpupd1, &src->tmpupd1, _state, make_automatic);
    dst->cntlu = src->cntlu;
    dst->cntregqr = src->cntregqr;
}


void _rbf3ddmsolver_clear(void* _p)
{
    rbf3ddmsolver *p = (rbf3ddmsolver*)_p;
    ae_touch_ptr((void*)p);
    _kdtree_clear(&p->kdt);
    ae_shared_pool_clear(&p->bufferpool);
    ae_shared_pool_clear(&p->subproblemspool);
    ae_shared_pool_clear(&p->subproblemsbuffer);
    ae_matrix_clear(&p->corrq);
    ae_matrix_clear(&p->corrr);
    ae_vector_clear(&p->corrnodes);
    ae_matrix_clear(&p->corrx);
    ae_matrix_clear(&p->tmpres1);
    ae_matrix_clear(&p->tmpupd1);
}


void _rbf3ddmsolver_destroy(void* _p)
{
    rbf3ddmsolver *p = (rbf3ddmsolver*)_p;
    ae_touch_ptr((void*)p);
    _kdtree_destroy(&p->kdt);
    ae_shared_pool_destroy(&p->bufferpool);
    ae_shared_pool_destroy(&p->subproblemspool);
    ae_shared_pool_destroy(&p->subproblemsbuffer);
    ae_matrix_destroy(&p->corrq);
    ae_matrix_destroy(&p->corrr);
    ae_vector_destroy(&p->corrnodes);
    ae_matrix_destroy(&p->corrx);
    ae_matrix_destroy(&p->tmpres1);
    ae_matrix_destroy(&p->tmpupd1);
}


void _rbfv3model_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    rbfv3model *p = (rbfv3model*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->v, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->cw, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->pointindexes, 0, DT_INT, _state, make_automatic);
    _rbf3evaluator_init(&p->evaluator, _state, make_automatic);
    _rbf3fastevaluator_init(&p->fasteval, _state, make_automatic);
    ae_matrix_init(&p->wchunked, 0, 0, DT_REAL, _state, make_automatic);
    _rbfv3calcbuffer_init(&p->calcbuf, _state, make_automatic);
}


void _rbfv3model_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    rbfv3model       *dst = (rbfv3model*)_dst;
    const rbfv3model *src = (const rbfv3model*)_src;
    dst->ny = src->ny;
    dst->nx = src->nx;
    dst->bftype = src->bftype;
    dst->bfparam = src->bfparam;
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_matrix_init_copy(&dst->v, &src->v, _state, make_automatic);
    ae_vector_init_copy(&dst->cw, &src->cw, _state, make_automatic);
    ae_vector_init_copy(&dst->pointindexes, &src->pointindexes, _state, make_automatic);
    dst->nc = src->nc;
    _rbf3evaluator_init_copy(&dst->evaluator, &src->evaluator, _state, make_automatic);
    _rbf3fastevaluator_init_copy(&dst->fasteval, &src->fasteval, _state, make_automatic);
    ae_matrix_init_copy(&dst->wchunked, &src->wchunked, _state, make_automatic);
    _rbfv3calcbuffer_init_copy(&dst->calcbuf, &src->calcbuf, _state, make_automatic);
    dst->dbgregqrusedforddm = src->dbgregqrusedforddm;
    dst->dbgworstfirstdecay = src->dbgworstfirstdecay;
}


void _rbfv3model_clear(void* _p)
{
    rbfv3model *p = (rbfv3model*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->s);
    ae_matrix_clear(&p->v);
    ae_vector_clear(&p->cw);
    ae_vector_clear(&p->pointindexes);
    _rbf3evaluator_clear(&p->evaluator);
    _rbf3fastevaluator_clear(&p->fasteval);
    ae_matrix_clear(&p->wchunked);
    _rbfv3calcbuffer_clear(&p->calcbuf);
}


void _rbfv3model_destroy(void* _p)
{
    rbfv3model *p = (rbfv3model*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->s);
    ae_matrix_destroy(&p->v);
    ae_vector_destroy(&p->cw);
    ae_vector_destroy(&p->pointindexes);
    _rbf3evaluator_destroy(&p->evaluator);
    _rbf3fastevaluator_destroy(&p->fasteval);
    ae_matrix_destroy(&p->wchunked);
    _rbfv3calcbuffer_destroy(&p->calcbuf);
}


void _rbfv3report_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    rbfv3report *p = (rbfv3report*)_p;
    ae_touch_ptr((void*)p);
}


void _rbfv3report_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    rbfv3report       *dst = (rbfv3report*)_dst;
    const rbfv3report *src = (const rbfv3report*)_src;
    dst->terminationtype = src->terminationtype;
    dst->maxerror = src->maxerror;
    dst->rmserror = src->rmserror;
    dst->iterationscount = src->iterationscount;
}


void _rbfv3report_clear(void* _p)
{
    rbfv3report *p = (rbfv3report*)_p;
    ae_touch_ptr((void*)p);
}


void _rbfv3report_destroy(void* _p)
{
    rbfv3report *p = (rbfv3report*)_p;
    ae_touch_ptr((void*)p);
}


/*$ End $*/
