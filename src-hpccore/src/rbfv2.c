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
#include "rbfv2.h"


/*$ Declarations $*/
static double rbfv2_defaultlambdareg = 1.0E-6;
static double rbfv2_defaultsupportr = 0.10;
static ae_int_t rbfv2_defaultmaxits = 400;
static ae_int_t rbfv2_defaultbf = 1;
static ae_int_t rbfv2_maxnodesize = 6;
static double rbfv2_complexitymultiplier = 100.0;
static ae_bool rbfv2_rbfv2buildlinearmodel(/* Real    */ const ae_matrix* x,
     /* Real    */ ae_matrix* y,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t modeltype,
     /* Real    */ ae_matrix* v,
     ae_state *_state);
static void rbfv2_allocatecalcbuffer(const rbfv2model* s,
     rbfv2calcbuffer* buf,
     ae_state *_state);
static void rbfv2_convertandappendtree(const kdtree* curtree,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t ny,
     /* Integer */ ae_vector* kdnodes,
     /* Real    */ ae_vector* kdsplits,
     /* Real    */ ae_vector* cw,
     ae_state *_state);
static void rbfv2_converttreerec(const kdtree* curtree,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t nodeoffset,
     ae_int_t nodesbase,
     ae_int_t splitsbase,
     ae_int_t cwbase,
     /* Integer */ ae_vector* localnodes,
     ae_int_t* localnodessize,
     /* Real    */ ae_vector* localsplits,
     ae_int_t* localsplitssize,
     /* Real    */ ae_vector* localcw,
     ae_int_t* localcwsize,
     /* Real    */ ae_matrix* xybuf,
     ae_state *_state);
static void rbfv2_partialcalcrec(const rbfv2model* s,
     rbfv2calcbuffer* buf,
     ae_int_t rootidx,
     double invr2,
     double queryr2,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     /* Real    */ ae_vector* dy,
     /* Real    */ ae_vector* d2y,
     ae_int_t needdy,
     ae_state *_state);
static void rbfv2_partialrowcalcrec(const rbfv2model* s,
     rbfv2calcbuffer* buf,
     ae_int_t rootidx,
     double invr2,
     double rquery2,
     double rfar2,
     /* Real    */ const ae_vector* cx,
     /* Real    */ const ae_vector* rx,
     /* Boolean */ const ae_vector* rf,
     ae_int_t rowsize,
     /* Real    */ ae_vector* ry,
     ae_state *_state);
static void rbfv2_preparepartialquery(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* kdboxmin,
     /* Real    */ const ae_vector* kdboxmax,
     ae_int_t nx,
     rbfv2calcbuffer* buf,
     ae_int_t* cnt,
     ae_state *_state);
static void rbfv2_partialqueryrec(/* Integer */ const ae_vector* kdnodes,
     /* Real    */ const ae_vector* kdsplits,
     /* Real    */ const ae_vector* cw,
     ae_int_t nx,
     ae_int_t ny,
     rbfv2calcbuffer* buf,
     ae_int_t rootidx,
     double queryr2,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* r2,
     /* Integer */ ae_vector* offs,
     ae_int_t* k,
     ae_state *_state);
static ae_int_t rbfv2_partialcountrec(/* Integer */ const ae_vector* kdnodes,
     /* Real    */ const ae_vector* kdsplits,
     /* Real    */ const ae_vector* cw,
     ae_int_t nx,
     ae_int_t ny,
     rbfv2calcbuffer* buf,
     ae_int_t rootidx,
     double queryr2,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
static void rbfv2_partialunpackrec(/* Integer */ const ae_vector* kdnodes,
     /* Real    */ const ae_vector* kdsplits,
     /* Real    */ const ae_vector* cw,
     /* Real    */ const ae_vector* s,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t rootidx,
     double r,
     /* Real    */ ae_matrix* xwr,
     ae_int_t* k,
     ae_state *_state);
static ae_int_t rbfv2_designmatrixrowsize(/* Integer */ const ae_vector* kdnodes,
     /* Real    */ const ae_vector* kdsplits,
     /* Real    */ const ae_vector* cw,
     /* Real    */ const ae_vector* ri,
     /* Integer */ const ae_vector* kdroots,
     /* Real    */ const ae_vector* kdboxmin,
     /* Real    */ const ae_vector* kdboxmax,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t nh,
     ae_int_t level,
     double rcoeff,
     /* Real    */ ae_vector* x0,
     rbfv2calcbuffer* calcbuf,
     ae_state *_state);
static void rbfv2_designmatrixgeneraterow(/* Integer */ const ae_vector* kdnodes,
     /* Real    */ const ae_vector* kdsplits,
     /* Real    */ const ae_vector* cw,
     /* Real    */ const ae_vector* ri,
     /* Integer */ const ae_vector* kdroots,
     /* Real    */ const ae_vector* kdboxmin,
     /* Real    */ const ae_vector* kdboxmax,
     /* Integer */ const ae_vector* cwrange,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t nh,
     ae_int_t level,
     ae_int_t bf,
     double rcoeff,
     ae_int_t rowsperpoint,
     double penalty,
     /* Real    */ ae_vector* x0,
     rbfv2calcbuffer* calcbuf,
     /* Real    */ ae_vector* tmpr2,
     /* Integer */ ae_vector* tmpoffs,
     /* Integer */ ae_vector* rowidx,
     /* Real    */ ae_vector* rowval,
     ae_int_t* rowsize,
     ae_state *_state);
static void rbfv2_zerofill(rbfv2model* s,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t bf,
     ae_state *_state);


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;

    _rbfv2model_clear(s);

    ae_assert(nx>=1, "RBFCreate: NX<1", _state);
    ae_assert(ny>=1, "RBFCreate: NY<1", _state);
    
    /*
     * Serializable parameters
     */
    s->nx = nx;
    s->ny = ny;
    s->bf = 0;
    s->nh = 0;
    ae_matrix_set_length(&s->v, ny, nx+1, _state);
    for(i=0; i<=ny-1; i++)
    {
        for(j=0; j<=nx; j++)
        {
            s->v.ptr.pp_double[i][j] = (double)(0);
        }
    }
    
    /*
     * Non-serializable parameters
     */
    s->lambdareg = rbfv2_defaultlambdareg;
    s->maxits = rbfv2_defaultmaxits;
    s->supportr = rbfv2_defaultsupportr;
    s->basisfunction = rbfv2_defaultbf;
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
     Copyright 02.04.2016 by Sergey Bochkanov
*************************************************************************/
void rbfv2createcalcbuffer(const rbfv2model* s,
     rbfv2calcbuffer* buf,
     ae_state *_state)
{

    _rbfv2calcbuffer_clear(buf);

    rbfv2_allocatecalcbuffer(s, buf, _state);
}


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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t nx;
    ae_int_t ny;
    ae_int_t bf;
    ae_matrix rhs;
    ae_matrix residualy;
    ae_matrix v;
    ae_int_t rowsperpoint;
    ae_vector hidx;
    ae_vector xr;
    ae_vector ri;
    ae_vector kdroots;
    ae_vector kdnodes;
    ae_vector kdsplits;
    ae_vector kdboxmin;
    ae_vector kdboxmax;
    ae_vector cw;
    ae_vector cwrange;
    ae_matrix curxy;
    ae_int_t curn;
    ae_int_t nbasis;
    kdtree curtree;
    kdtree globaltree;
    ae_vector x0;
    ae_vector x1;
    ae_vector tags;
    ae_vector dist;
    ae_vector nncnt;
    ae_vector rowsizes;
    ae_vector diagata;
    ae_vector prec;
    ae_vector tmpx;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t k2;
    ae_int_t levelidx;
    ae_int_t offsi;
    ae_int_t offsj;
    double val;
    double criticalr;
    ae_int_t cnt;
    double avgdiagata;
    ae_vector avgrowsize;
    double sumrowsize;
    double rprogress;
    ae_int_t maxits;
    linlsqrstate linstate;
    linlsqrreport lsqrrep;
    sparsematrix sparseacrs;
    ae_vector densew1;
    ae_vector denseb1;
    rbfv2calcbuffer calcbuf;
    ae_vector vr2;
    ae_vector voffs;
    ae_vector rowindexes;
    ae_vector rowvals;
    double penalty;

    ae_frame_make(_state, &_frame_block);
    memset(&rhs, 0, sizeof(rhs));
    memset(&residualy, 0, sizeof(residualy));
    memset(&v, 0, sizeof(v));
    memset(&hidx, 0, sizeof(hidx));
    memset(&xr, 0, sizeof(xr));
    memset(&ri, 0, sizeof(ri));
    memset(&kdroots, 0, sizeof(kdroots));
    memset(&kdnodes, 0, sizeof(kdnodes));
    memset(&kdsplits, 0, sizeof(kdsplits));
    memset(&kdboxmin, 0, sizeof(kdboxmin));
    memset(&kdboxmax, 0, sizeof(kdboxmax));
    memset(&cw, 0, sizeof(cw));
    memset(&cwrange, 0, sizeof(cwrange));
    memset(&curxy, 0, sizeof(curxy));
    memset(&curtree, 0, sizeof(curtree));
    memset(&globaltree, 0, sizeof(globaltree));
    memset(&x0, 0, sizeof(x0));
    memset(&x1, 0, sizeof(x1));
    memset(&tags, 0, sizeof(tags));
    memset(&dist, 0, sizeof(dist));
    memset(&nncnt, 0, sizeof(nncnt));
    memset(&rowsizes, 0, sizeof(rowsizes));
    memset(&diagata, 0, sizeof(diagata));
    memset(&prec, 0, sizeof(prec));
    memset(&tmpx, 0, sizeof(tmpx));
    memset(&avgrowsize, 0, sizeof(avgrowsize));
    memset(&linstate, 0, sizeof(linstate));
    memset(&lsqrrep, 0, sizeof(lsqrrep));
    memset(&sparseacrs, 0, sizeof(sparseacrs));
    memset(&densew1, 0, sizeof(densew1));
    memset(&denseb1, 0, sizeof(denseb1));
    memset(&calcbuf, 0, sizeof(calcbuf));
    memset(&vr2, 0, sizeof(vr2));
    memset(&voffs, 0, sizeof(voffs));
    memset(&rowindexes, 0, sizeof(rowindexes));
    memset(&rowvals, 0, sizeof(rowvals));
    _rbfv2report_clear(rep);
    ae_matrix_init(&rhs, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&residualy, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&v, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&hidx, 0, DT_INT, _state, ae_true);
    ae_vector_init(&xr, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ri, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&kdroots, 0, DT_INT, _state, ae_true);
    ae_vector_init(&kdnodes, 0, DT_INT, _state, ae_true);
    ae_vector_init(&kdsplits, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&kdboxmin, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&kdboxmax, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&cw, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&cwrange, 0, DT_INT, _state, ae_true);
    ae_matrix_init(&curxy, 0, 0, DT_REAL, _state, ae_true);
    _kdtree_init(&curtree, _state, ae_true);
    _kdtree_init(&globaltree, _state, ae_true);
    ae_vector_init(&x0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&x1, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tags, 0, DT_INT, _state, ae_true);
    ae_vector_init(&dist, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&nncnt, 0, DT_INT, _state, ae_true);
    ae_vector_init(&rowsizes, 0, DT_INT, _state, ae_true);
    ae_vector_init(&diagata, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&prec, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmpx, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&avgrowsize, 0, DT_REAL, _state, ae_true);
    _linlsqrstate_init(&linstate, _state, ae_true);
    _linlsqrreport_init(&lsqrrep, _state, ae_true);
    _sparsematrix_init(&sparseacrs, _state, ae_true);
    ae_vector_init(&densew1, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&denseb1, 0, DT_REAL, _state, ae_true);
    _rbfv2calcbuffer_init(&calcbuf, _state, ae_true);
    ae_vector_init(&vr2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&voffs, 0, DT_INT, _state, ae_true);
    ae_vector_init(&rowindexes, 0, DT_INT, _state, ae_true);
    ae_vector_init(&rowvals, 0, DT_REAL, _state, ae_true);

    ae_assert(s->nx>0, "RBFV2BuildHierarchical: incorrect NX", _state);
    ae_assert(s->ny>0, "RBFV2BuildHierarchical: incorrect NY", _state);
    ae_assert(ae_fp_greater_eq(lambdans,(double)(0)), "RBFV2BuildHierarchical: incorrect LambdaNS", _state);
    for(j=0; j<=s->nx-1; j++)
    {
        ae_assert(ae_fp_greater(scalevec->ptr.p_double[j],(double)(0)), "RBFV2BuildHierarchical: incorrect ScaleVec", _state);
    }
    nx = s->nx;
    ny = s->ny;
    bf = s->basisfunction;
    ae_assert(bf==0||bf==1, "RBFV2BuildHierarchical: incorrect BF", _state);
    
    /*
     * Clean up communication and report fields
     */
    *progress10000 = 0;
    rep->maxerror = (double)(0);
    rep->rmserror = (double)(0);
    
    /*
     * Quick exit when we have no points
     */
    if( n==0 )
    {
        rbfv2_zerofill(s, nx, ny, bf, _state);
        rep->terminationtype = 1;
        *progress10000 = 10000;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * First model in a sequence - linear model.
     * Residuals from linear regression are stored in the ResidualY variable
     * (used later to build RBF models).
     */
    ae_matrix_set_length(&residualy, n, ny, _state);
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=ny-1; j++)
        {
            residualy.ptr.pp_double[i][j] = y->ptr.pp_double[i][j];
        }
    }
    if( !rbfv2_rbfv2buildlinearmodel(x, &residualy, n, nx, ny, aterm, &v, _state) )
    {
        rbfv2_zerofill(s, nx, ny, bf, _state);
        rep->terminationtype = -5;
        *progress10000 = 10000;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Handle special case: multilayer model with NLayers=0.
     * Quick exit.
     */
    if( nh==0 )
    {
        rep->terminationtype = 1;
        rbfv2_zerofill(s, nx, ny, bf, _state);
        for(i=0; i<=ny-1; i++)
        {
            for(j=0; j<=nx; j++)
            {
                s->v.ptr.pp_double[i][j] = v.ptr.pp_double[i][j];
            }
        }
        rep->maxerror = (double)(0);
        rep->rmserror = (double)(0);
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=ny-1; j++)
            {
                rep->maxerror = ae_maxreal(rep->maxerror, ae_fabs(residualy.ptr.pp_double[i][j], _state), _state);
                rep->rmserror = rep->rmserror+ae_sqr(residualy.ptr.pp_double[i][j], _state);
            }
        }
        rep->rmserror = ae_sqrt(rep->rmserror/(double)(n*ny), _state);
        *progress10000 = 10000;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Penalty coefficient is set to LambdaNS*RBase^2.
     *
     * We use such normalization because VALUES of radial basis
     * functions have roughly unit magnitude, but their DERIVATIVES
     * are (roughly) inversely proportional to the radius. Thus,
     * without additional scaling, regularization coefficient
     * looses invariancy w.r.t. scaling of variables.
     */
    if( ae_fp_eq(lambdans,(double)(0)) )
    {
        rowsperpoint = 1;
    }
    else
    {
        
        /*
         * NOTE: simplified penalty function is used, which does not provide rotation invariance
         */
        rowsperpoint = 1+nx;
    }
    penalty = lambdans*ae_sqr(rbase, _state);
    
    /*
     * Prepare temporary structures
     */
    ae_matrix_set_length(&rhs, n*rowsperpoint, ny, _state);
    ae_matrix_set_length(&curxy, n, nx+ny, _state);
    ae_vector_set_length(&x0, nx, _state);
    ae_vector_set_length(&x1, nx, _state);
    ae_vector_set_length(&tags, n, _state);
    ae_vector_set_length(&dist, n, _state);
    ae_vector_set_length(&vr2, n, _state);
    ae_vector_set_length(&voffs, n, _state);
    ae_vector_set_length(&nncnt, n, _state);
    ae_vector_set_length(&rowsizes, n*rowsperpoint, _state);
    ae_vector_set_length(&denseb1, n*rowsperpoint, _state);
    for(i=0; i<=n*rowsperpoint-1; i++)
    {
        for(j=0; j<=ny-1; j++)
        {
            rhs.ptr.pp_double[i][j] = (double)(0);
        }
    }
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            curxy.ptr.pp_double[i][j] = x->ptr.pp_double[i][j]/scalevec->ptr.p_double[j];
        }
        for(j=0; j<=ny-1; j++)
        {
            rhs.ptr.pp_double[i*rowsperpoint][j] = residualy.ptr.pp_double[i][j];
        }
        tags.ptr.p_int[i] = i;
    }
    kdtreebuildtagged(&curxy, &tags, n, nx, 0, 2, &globaltree, _state);
    
    /*
     * Generate sequence of layer radii.
     * Prepare assignment of different levels to points.
     */
    ae_assert(n>0, "RBFV2BuildHierarchical: integrity check failed", _state);
    ae_vector_set_length(&ri, nh, _state);
    for(levelidx=0; levelidx<=nh-1; levelidx++)
    {
        ri.ptr.p_double[levelidx] = rbase*ae_pow((double)(2), (double)(-levelidx), _state);
    }
    ae_vector_set_length(&hidx, n, _state);
    ae_vector_set_length(&xr, n, _state);
    for(i=0; i<=n-1; i++)
    {
        hidx.ptr.p_int[i] = nh;
        xr.ptr.p_double[i] = ae_maxrealnumber;
        ae_assert(ae_fp_greater(xr.ptr.p_double[i],ri.ptr.p_double[0]), "RBFV2BuildHierarchical: integrity check failed", _state);
    }
    for(levelidx=0; levelidx<=nh-1; levelidx++)
    {
        
        /*
         * Scan dataset points, for each such point that distance to nearest
         * "support" point is larger than SupportR*Ri[LevelIdx] we:
         * * set distance of current point to 0 (it is support now) and update HIdx
         * * perform R-NN request with radius SupportR*Ri[LevelIdx]
         * * for each point in request update its distance
         */
        criticalr = s->supportr*ri.ptr.p_double[levelidx];
        for(i=0; i<=n-1; i++)
        {
            if( ae_fp_greater(xr.ptr.p_double[i],criticalr) )
            {
                
                /*
                 * Mark point as support
                 */
                ae_assert(hidx.ptr.p_int[i]==nh, "RBFV2BuildHierarchical: integrity check failed", _state);
                hidx.ptr.p_int[i] = levelidx;
                xr.ptr.p_double[i] = (double)(0);
                
                /*
                 * Update neighbors
                 */
                for(j=0; j<=nx-1; j++)
                {
                    x0.ptr.p_double[j] = x->ptr.pp_double[i][j]/scalevec->ptr.p_double[j];
                }
                k = kdtreequeryrnn(&globaltree, &x0, criticalr, ae_true, _state);
                kdtreequeryresultstags(&globaltree, &tags, _state);
                kdtreequeryresultsdistances(&globaltree, &dist, _state);
                for(j=0; j<=k-1; j++)
                {
                    xr.ptr.p_double[tags.ptr.p_int[j]] = ae_minreal(xr.ptr.p_double[tags.ptr.p_int[j]], dist.ptr.p_double[j], _state);
                }
            }
        }
    }
    
    /*
     * Build multitree (with zero weights) according to hierarchy.
     *
     * NOTE: this code assumes that during every iteration kdNodes,
     *       kdSplits and CW have size which EXACTLY fits their
     *       contents, and that these variables are resized at each
     *       iteration when we add new hierarchical model.
     */
    ae_vector_set_length(&kdroots, nh+1, _state);
    ae_vector_set_length(&kdnodes, 0, _state);
    ae_vector_set_length(&kdsplits, 0, _state);
    ae_vector_set_length(&kdboxmin, nx, _state);
    ae_vector_set_length(&kdboxmax, nx, _state);
    ae_vector_set_length(&cw, 0, _state);
    ae_vector_set_length(&cwrange, nh+1, _state);
    kdtreeexplorebox(&globaltree, &kdboxmin, &kdboxmax, _state);
    cwrange.ptr.p_int[0] = 0;
    for(levelidx=0; levelidx<=nh-1; levelidx++)
    {
        
        /*
         * Prepare radius and root offset
         */
        kdroots.ptr.p_int[levelidx] = kdnodes.cnt;
        
        /*
         * Generate LevelIdx-th tree and append to multi-tree
         */
        curn = 0;
        for(i=0; i<=n-1; i++)
        {
            if( hidx.ptr.p_int[i]<=levelidx )
            {
                for(j=0; j<=nx-1; j++)
                {
                    curxy.ptr.pp_double[curn][j] = x->ptr.pp_double[i][j]/scalevec->ptr.p_double[j];
                }
                for(j=0; j<=ny-1; j++)
                {
                    curxy.ptr.pp_double[curn][nx+j] = (double)(0);
                }
                inc(&curn, _state);
            }
        }
        ae_assert(curn>0, "RBFV2BuildHierarchical: integrity check failed", _state);
        kdtreebuild(&curxy, curn, nx, ny, 2, &curtree, _state);
        rbfv2_convertandappendtree(&curtree, curn, nx, ny, &kdnodes, &kdsplits, &cw, _state);
        
        /*
         * Fill entry of CWRange (we assume that length of CW exactly fits its actual size)
         */
        cwrange.ptr.p_int[levelidx+1] = cw.cnt;
    }
    kdroots.ptr.p_int[nh] = kdnodes.cnt;
    
    /*
     * Prepare buffer and scaled dataset
     */
    rbfv2_allocatecalcbuffer(s, &calcbuf, _state);
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            curxy.ptr.pp_double[i][j] = x->ptr.pp_double[i][j]/scalevec->ptr.p_double[j];
        }
    }
    
    /*
     * Calculate average row sizes for each layer; these values are used
     * for smooth progress reporting (it adds some overhead, but in most
     * cases - insignificant one).
     */
    rvectorsetlengthatleast(&avgrowsize, nh, _state);
    sumrowsize = (double)(0);
    for(levelidx=0; levelidx<=nh-1; levelidx++)
    {
        cnt = 0;
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=nx-1; j++)
            {
                x0.ptr.p_double[j] = curxy.ptr.pp_double[i][j];
            }
            cnt = cnt+rbfv2_designmatrixrowsize(&kdnodes, &kdsplits, &cw, &ri, &kdroots, &kdboxmin, &kdboxmax, nx, ny, nh, levelidx, rbfv2nearradius(bf, _state), &x0, &calcbuf, _state);
        }
        avgrowsize.ptr.p_double[levelidx] = coalesce((double)(cnt), (double)(1), _state)/coalesce((double)(n), (double)(1), _state);
        sumrowsize = sumrowsize+avgrowsize.ptr.p_double[levelidx];
    }
    
    /*
     * Build unconstrained model with LSQR solver, applied layer by layer
     */
    for(levelidx=0; levelidx<=nh-1; levelidx++)
    {
        
        /*
         * Generate A - matrix of basis functions (near radius is used)
         *
         * NOTE: AvgDiagATA is average value of diagonal element of A^T*A.
         *       It is used to calculate value of Tikhonov regularization
         *       coefficient.
         */
        nbasis = (cwrange.ptr.p_int[levelidx+1]-cwrange.ptr.p_int[levelidx])/(nx+ny);
        ae_assert(cwrange.ptr.p_int[levelidx+1]-cwrange.ptr.p_int[levelidx]==nbasis*(nx+ny), "Assertion failed", _state);
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=nx-1; j++)
            {
                x0.ptr.p_double[j] = curxy.ptr.pp_double[i][j];
            }
            cnt = rbfv2_designmatrixrowsize(&kdnodes, &kdsplits, &cw, &ri, &kdroots, &kdboxmin, &kdboxmax, nx, ny, nh, levelidx, rbfv2nearradius(bf, _state), &x0, &calcbuf, _state);
            nncnt.ptr.p_int[i] = cnt;
            for(j=0; j<=rowsperpoint-1; j++)
            {
                rowsizes.ptr.p_int[i*rowsperpoint+j] = cnt;
            }
        }
        ivectorsetlengthatleast(&rowindexes, nbasis, _state);
        rvectorsetlengthatleast(&rowvals, nbasis*rowsperpoint, _state);
        rvectorsetlengthatleast(&diagata, nbasis, _state);
        sparsecreatecrsbuf(n*rowsperpoint, nbasis, &rowsizes, &sparseacrs, _state);
        avgdiagata = 0.0;
        for(j=0; j<=nbasis-1; j++)
        {
            diagata.ptr.p_double[j] = (double)(0);
        }
        for(i=0; i<=n-1; i++)
        {
            
            /*
             * Fill design matrix row, diagonal of A^T*A
             */
            for(j=0; j<=nx-1; j++)
            {
                x0.ptr.p_double[j] = curxy.ptr.pp_double[i][j];
            }
            rbfv2_designmatrixgeneraterow(&kdnodes, &kdsplits, &cw, &ri, &kdroots, &kdboxmin, &kdboxmax, &cwrange, nx, ny, nh, levelidx, bf, rbfv2nearradius(bf, _state), rowsperpoint, penalty, &x0, &calcbuf, &vr2, &voffs, &rowindexes, &rowvals, &cnt, _state);
            ae_assert(cnt==nncnt.ptr.p_int[i], "RBFV2BuildHierarchical: integrity check failed", _state);
            for(k=0; k<=rowsperpoint-1; k++)
            {
                for(j=0; j<=cnt-1; j++)
                {
                    val = rowvals.ptr.p_double[j*rowsperpoint+k];
                    sparseset(&sparseacrs, i*rowsperpoint+k, rowindexes.ptr.p_int[j], val, _state);
                    avgdiagata = avgdiagata+ae_sqr(val, _state);
                    diagata.ptr.p_double[rowindexes.ptr.p_int[j]] = diagata.ptr.p_double[rowindexes.ptr.p_int[j]]+ae_sqr(val, _state);
                }
            }
            
            /*
             * Handle possible termination requests
             */
            if( *terminationrequest )
            {
                
                /*
                 * Request for termination was submitted, terminate immediately
                 */
                rbfv2_zerofill(s, nx, ny, bf, _state);
                rep->terminationtype = 8;
                *progress10000 = 10000;
                ae_frame_leave(_state);
                return;
            }
        }
        avgdiagata = avgdiagata/(double)nbasis;
        rvectorsetlengthatleast(&prec, nbasis, _state);
        for(j=0; j<=nbasis-1; j++)
        {
            prec.ptr.p_double[j] = (double)1/coalesce(ae_sqrt(diagata.ptr.p_double[j], _state), (double)(1), _state);
        }
        
        /*
         * solve
         */
        maxits = coalescei(s->maxits, rbfv2_defaultmaxits, _state);
        rvectorsetlengthatleast(&tmpx, nbasis, _state);
        linlsqrcreate(n*rowsperpoint, nbasis, &linstate, _state);
        linlsqrsetcond(&linstate, 0.0, 0.0, maxits, _state);
        linlsqrsetlambdai(&linstate, ae_sqrt(s->lambdareg*avgdiagata, _state), _state);
        for(j=0; j<=ny-1; j++)
        {
            for(i=0; i<=n*rowsperpoint-1; i++)
            {
                denseb1.ptr.p_double[i] = rhs.ptr.pp_double[i][j];
            }
            linlsqrsetb(&linstate, &denseb1, _state);
            linlsqrrestart(&linstate, _state);
            linlsqrsetxrep(&linstate, ae_true, _state);
            while(linlsqriteration(&linstate, _state))
            {
                if( *terminationrequest )
                {
                    
                    /*
                     * Request for termination was submitted, terminate immediately
                     */
                    rbfv2_zerofill(s, nx, ny, bf, _state);
                    rep->terminationtype = 8;
                    *progress10000 = 10000;
                    ae_frame_leave(_state);
                    return;
                }
                if( linstate.needmv )
                {
                    for(i=0; i<=nbasis-1; i++)
                    {
                        tmpx.ptr.p_double[i] = prec.ptr.p_double[i]*linstate.x.ptr.p_double[i];
                    }
                    sparsemv(&sparseacrs, &tmpx, &linstate.mv, _state);
                    continue;
                }
                if( linstate.needmtv )
                {
                    sparsemtv(&sparseacrs, &linstate.x, &linstate.mtv, _state);
                    for(i=0; i<=nbasis-1; i++)
                    {
                        linstate.mtv.ptr.p_double[i] = prec.ptr.p_double[i]*linstate.mtv.ptr.p_double[i];
                    }
                    continue;
                }
                if( linstate.xupdated )
                {
                    rprogress = (double)(0);
                    for(i=0; i<=levelidx-1; i++)
                    {
                        rprogress = rprogress+(double)(maxits*ny)*avgrowsize.ptr.p_double[i];
                    }
                    rprogress = rprogress+(double)(linlsqrpeekiterationscount(&linstate, _state)+j*maxits)*avgrowsize.ptr.p_double[levelidx];
                    rprogress = rprogress/(sumrowsize*(double)maxits*(double)ny);
                    rprogress = (double)10000*rprogress;
                    rprogress = ae_maxreal(rprogress, (double)(0), _state);
                    rprogress = ae_minreal(rprogress, (double)(10000), _state);
                    ae_assert(*progress10000<=ae_round(rprogress, _state)+1, "HRBF: integrity check failed (progress indicator) even after +1 safeguard correction", _state);
                    *progress10000 = ae_round(rprogress, _state);
                    continue;
                }
                ae_assert(ae_false, "HRBF: unexpected request from LSQR solver", _state);
            }
            linlsqrresults(&linstate, &densew1, &lsqrrep, _state);
            ae_assert(lsqrrep.terminationtype>0, "RBFV2BuildHierarchical: integrity check failed", _state);
            for(i=0; i<=nbasis-1; i++)
            {
                densew1.ptr.p_double[i] = prec.ptr.p_double[i]*densew1.ptr.p_double[i];
            }
            for(i=0; i<=nbasis-1; i++)
            {
                offsi = cwrange.ptr.p_int[levelidx]+(nx+ny)*i;
                cw.ptr.p_double[offsi+nx+j] = densew1.ptr.p_double[i];
            }
        }
        
        /*
         * Update residuals (far radius is used)
         */
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=nx-1; j++)
            {
                x0.ptr.p_double[j] = curxy.ptr.pp_double[i][j];
            }
            rbfv2_designmatrixgeneraterow(&kdnodes, &kdsplits, &cw, &ri, &kdroots, &kdboxmin, &kdboxmax, &cwrange, nx, ny, nh, levelidx, bf, rbfv2farradius(bf, _state), rowsperpoint, penalty, &x0, &calcbuf, &vr2, &voffs, &rowindexes, &rowvals, &cnt, _state);
            for(j=0; j<=cnt-1; j++)
            {
                offsj = cwrange.ptr.p_int[levelidx]+(nx+ny)*rowindexes.ptr.p_int[j]+nx;
                for(k=0; k<=rowsperpoint-1; k++)
                {
                    val = rowvals.ptr.p_double[j*rowsperpoint+k];
                    for(k2=0; k2<=ny-1; k2++)
                    {
                        rhs.ptr.pp_double[i*rowsperpoint+k][k2] = rhs.ptr.pp_double[i*rowsperpoint+k][k2]-val*cw.ptr.p_double[offsj+k2];
                    }
                }
            }
        }
    }
    
    /*
     * Model is built.
     *
     * Copy local variables by swapping, global ones (ScaleVec) are copied
     * explicitly.
     */
    s->bf = bf;
    s->nh = nh;
    ae_swap_vectors(&s->ri, &ri);
    ae_swap_vectors(&s->kdroots, &kdroots);
    ae_swap_vectors(&s->kdnodes, &kdnodes);
    ae_swap_vectors(&s->kdsplits, &kdsplits);
    ae_swap_vectors(&s->kdboxmin, &kdboxmin);
    ae_swap_vectors(&s->kdboxmax, &kdboxmax);
    ae_swap_vectors(&s->cw, &cw);
    ae_swap_matrices(&s->v, &v);
    ae_vector_set_length(&s->s, nx, _state);
    for(i=0; i<=nx-1; i++)
    {
        s->s.ptr.p_double[i] = scalevec->ptr.p_double[i];
    }
    rep->terminationtype = 1;
    
    /*
     * Calculate maximum and RMS errors
     */
    rep->maxerror = (double)(0);
    rep->rmserror = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=ny-1; j++)
        {
            rep->maxerror = ae_maxreal(rep->maxerror, ae_fabs(rhs.ptr.pp_double[i*rowsperpoint][j], _state), _state);
            rep->rmserror = rep->rmserror+ae_sqr(rhs.ptr.pp_double[i*rowsperpoint][j], _state);
        }
    }
    rep->rmserror = ae_sqrt(rep->rmserror/(double)(n*ny), _state);
    
    /*
     * Update progress reports
     */
    *progress10000 = 10000;
    ae_frame_leave(_state);
}


/*************************************************************************
Serializer: allocation

  -- ALGLIB --
     Copyright 02.02.2012 by Bochkanov Sergey
*************************************************************************/
void rbfv2alloc(ae_serializer* s,
     const rbfv2model* model,
     ae_state *_state)
{


    
    /*
     * Data
     */
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    allocrealarray(s, &model->ri, -1, _state);
    allocrealarray(s, &model->s, -1, _state);
    allocintegerarray(s, &model->kdroots, -1, _state);
    allocintegerarray(s, &model->kdnodes, -1, _state);
    allocrealarray(s, &model->kdsplits, -1, _state);
    allocrealarray(s, &model->kdboxmin, -1, _state);
    allocrealarray(s, &model->kdboxmax, -1, _state);
    allocrealarray(s, &model->cw, -1, _state);
    allocrealmatrix(s, &model->v, -1, -1, _state);
}


/*************************************************************************
Serializer: serialization

  -- ALGLIB --
     Copyright 02.02.2012 by Bochkanov Sergey
*************************************************************************/
void rbfv2serialize(ae_serializer* s,
     const rbfv2model* model,
     ae_state *_state)
{


    
    /*
     * Data
     */
    ae_serializer_serialize_int(s, model->nx, _state);
    ae_serializer_serialize_int(s, model->ny, _state);
    ae_serializer_serialize_int(s, model->nh, _state);
    ae_serializer_serialize_int(s, model->bf, _state);
    serializerealarray(s, &model->ri, -1, _state);
    serializerealarray(s, &model->s, -1, _state);
    serializeintegerarray(s, &model->kdroots, -1, _state);
    serializeintegerarray(s, &model->kdnodes, -1, _state);
    serializerealarray(s, &model->kdsplits, -1, _state);
    serializerealarray(s, &model->kdboxmin, -1, _state);
    serializerealarray(s, &model->kdboxmax, -1, _state);
    serializerealarray(s, &model->cw, -1, _state);
    serializerealmatrix(s, &model->v, -1, -1, _state);
}


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 02.02.2012 by Bochkanov Sergey
*************************************************************************/
void rbfv2unserialize(ae_serializer* s,
     rbfv2model* model,
     ae_state *_state)
{
    ae_int_t nx;
    ae_int_t ny;

    _rbfv2model_clear(model);

    
    /*
     * Unserialize primary model parameters, initialize model.
     *
     * It is necessary to call RBFCreate() because some internal fields
     * which are NOT unserialized will need initialization.
     */
    ae_serializer_unserialize_int(s, &nx, _state);
    ae_serializer_unserialize_int(s, &ny, _state);
    rbfv2create(nx, ny, model, _state);
    ae_serializer_unserialize_int(s, &model->nh, _state);
    ae_serializer_unserialize_int(s, &model->bf, _state);
    unserializerealarray(s, &model->ri, _state);
    unserializerealarray(s, &model->s, _state);
    unserializeintegerarray(s, &model->kdroots, _state);
    unserializeintegerarray(s, &model->kdnodes, _state);
    unserializerealarray(s, &model->kdsplits, _state);
    unserializerealarray(s, &model->kdboxmin, _state);
    unserializerealarray(s, &model->kdboxmax, _state);
    unserializerealarray(s, &model->cw, _state);
    unserializerealmatrix(s, &model->v, _state);
}


/*************************************************************************
Returns far radius for basis function type
*************************************************************************/
double rbfv2farradius(ae_int_t bf, ae_state *_state)
{
    double result;


    result = (double)(1);
    if( bf==0 )
    {
        result = 5.0;
    }
    if( bf==1 )
    {
        result = (double)(3);
    }
    return result;
}


/*************************************************************************
Returns near radius for basis function type
*************************************************************************/
double rbfv2nearradius(ae_int_t bf, ae_state *_state)
{
    double result;


    result = (double)(1);
    if( bf==0 )
    {
        result = 3.0;
    }
    if( bf==1 )
    {
        result = (double)(3);
    }
    return result;
}


/*************************************************************************
Returns basis function value.
Assumes that D2>=0
*************************************************************************/
double rbfv2basisfunc(ae_int_t bf, double d2, ae_state *_state)
{
    double v;
    double result;


    result = (double)(0);
    if( bf==0 )
    {
        result = ae_exp(-d2, _state);
        return result;
    }
    if( bf==1 )
    {
        
        /*
         * if D2<3:
         *     Exp(1)*Exp(-D2)*Exp(-1/(1-D2/9))
         * else:
         *     0
         */
        v = (double)1-d2/(double)9;
        if( ae_fp_less_eq(v,(double)(0)) )
        {
            result = (double)(0);
            return result;
        }
        result = 2.718281828459045*ae_exp(-d2, _state)*ae_exp(-(double)1/v, _state);
        return result;
    }
    ae_assert(ae_false, "RBFV2BasisFunc: unknown BF type", _state);
    return result;
}


/*************************************************************************
Returns basis function value, first and second derivatives
Assumes that D2>=0
*************************************************************************/
void rbfv2basisfuncdiff2(ae_int_t bf,
     double d2,
     double* f,
     double* df,
     double* d2f,
     ae_state *_state)
{
    double v;

    *f = 0.0;
    *df = 0.0;
    *d2f = 0.0;

    if( bf==0 )
    {
        *f = ae_exp(-d2, _state);
        *df = -*f;
        *d2f = *f;
        return;
    }
    if( bf==1 )
    {
        
        /*
         * if D2<3:
         *       F = Exp(1)*Exp(-D2)*Exp(-1/(1-D2/9))
         *      dF =  -F * [pow(D2/9-1,-2)/9 + 1]
         *     d2F = -dF * [pow(D2/9-1,-2)/9 + 1] - F*(2/81)*pow(D2/9-1,-3)
         * else:
         *     0
         */
        v = (double)1-d2/(double)9;
        if( ae_fp_less_eq(v,(double)(0)) )
        {
            *f = (double)(0);
            *df = (double)(0);
            *d2f = (double)(0);
            return;
        }
        *f = ae_exp((double)(1), _state)*ae_exp(-d2, _state)*ae_exp(-(double)1/v, _state);
        *df = -*f*((double)1/((double)9*v*v)+(double)1);
        *d2f = -*df*((double)1/((double)9*v*v)+(double)1)-*f*((double)2/(double)81)/(v*v*v);
        return;
    }
    ae_assert(ae_false, "RBFV2BasisFuncDiff2: unknown BF type", _state);
}


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
double rbfv2calc1(rbfv2model* s, double x0, ae_state *_state)
{
    double result;


    ae_assert(ae_isfinite(x0, _state), "RBFCalc1: invalid value for X0 (X0 is Inf)!", _state);
    if( s->ny!=1||s->nx!=1 )
    {
        result = (double)(0);
        return result;
    }
    result = s->v.ptr.pp_double[0][0]*x0-s->v.ptr.pp_double[0][1];
    if( s->nh==0 )
    {
        return result;
    }
    rbfv2_allocatecalcbuffer(s, &s->calcbuf, _state);
    s->calcbuf.x123.ptr.p_double[0] = x0;
    rbfv2tscalcbuf(s, &s->calcbuf, &s->calcbuf.x123, &s->calcbuf.y123, _state);
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
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
double rbfv2calc2(rbfv2model* s, double x0, double x1, ae_state *_state)
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
    if( s->nh==0 )
    {
        return result;
    }
    rbfv2_allocatecalcbuffer(s, &s->calcbuf, _state);
    s->calcbuf.x123.ptr.p_double[0] = x0;
    s->calcbuf.x123.ptr.p_double[1] = x1;
    rbfv2tscalcbuf(s, &s->calcbuf, &s->calcbuf.x123, &s->calcbuf.y123, _state);
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
     Copyright 13.12.2011 by Bochkanov Sergey
*************************************************************************/
double rbfv2calc3(rbfv2model* s,
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
    if( s->nh==0 )
    {
        return result;
    }
    rbfv2_allocatecalcbuffer(s, &s->calcbuf, _state);
    s->calcbuf.x123.ptr.p_double[0] = x0;
    s->calcbuf.x123.ptr.p_double[1] = x1;
    s->calcbuf.x123.ptr.p_double[2] = x2;
    rbfv2tscalcbuf(s, &s->calcbuf, &s->calcbuf.x123, &s->calcbuf.y123, _state);
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
void rbfv2calcbuf(rbfv2model* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{


    rbfv2tscalcbuf(s, &s->calcbuf, x, y, _state);
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
void rbfv2tscalcbuf(const rbfv2model* s,
     rbfv2calcbuffer* buf,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t levelidx;
    double rcur;
    double rquery2;
    double invrc2;
    ae_int_t nx;
    ae_int_t ny;


    ae_assert(x->cnt>=s->nx, "RBFCalcBuf: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFCalcBuf: X contains infinite or NaN values", _state);
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
    if( s->nh==0 )
    {
        return;
    }
    
    /*
     * Handle nonlinear term
     */
    rbfv2_allocatecalcbuffer(s, buf, _state);
    for(j=0; j<=nx-1; j++)
    {
        buf->x.ptr.p_double[j] = x->ptr.p_double[j]/s->s.ptr.p_double[j];
    }
    for(levelidx=0; levelidx<=s->nh-1; levelidx++)
    {
        
        /*
         * Prepare fields of Buf required by PartialCalcRec()
         */
        buf->curdist2 = (double)(0);
        for(j=0; j<=nx-1; j++)
        {
            buf->curboxmin.ptr.p_double[j] = s->kdboxmin.ptr.p_double[j];
            buf->curboxmax.ptr.p_double[j] = s->kdboxmax.ptr.p_double[j];
            if( ae_fp_less(buf->x.ptr.p_double[j],buf->curboxmin.ptr.p_double[j]) )
            {
                buf->curdist2 = buf->curdist2+ae_sqr(buf->curboxmin.ptr.p_double[j]-buf->x.ptr.p_double[j], _state);
            }
            else
            {
                if( ae_fp_greater(buf->x.ptr.p_double[j],buf->curboxmax.ptr.p_double[j]) )
                {
                    buf->curdist2 = buf->curdist2+ae_sqr(buf->x.ptr.p_double[j]-buf->curboxmax.ptr.p_double[j], _state);
                }
            }
        }
        
        /*
         * Call PartialCalcRec()
         */
        rcur = s->ri.ptr.p_double[levelidx];
        invrc2 = (double)1/(rcur*rcur);
        rquery2 = ae_sqr(rcur*rbfv2farradius(s->bf, _state), _state);
        rbfv2_partialcalcrec(s, buf, s->kdroots.ptr.p_int[levelidx], invrc2, rquery2, &buf->x, y, y, y, 0, _state);
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
void rbfv2tsdiffbuf(const rbfv2model* s,
     rbfv2calcbuffer* buf,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     /* Real    */ ae_vector* dy,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t levelidx;
    double rcur;
    double rquery2;
    double invrc2;
    ae_int_t nx;
    ae_int_t ny;


    ae_assert(x->cnt>=s->nx, "RBFDiffBuf: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFDiffBuf: X contains infinite or NaN values", _state);
    nx = s->nx;
    ny = s->ny;
    if( y->cnt<s->ny )
    {
        ae_vector_set_length(y, s->ny, _state);
    }
    if( dy->cnt<s->ny*s->nx )
    {
        ae_vector_set_length(dy, s->ny*s->nx, _state);
    }
    
    /*
     * Handle linear term
     */
    for(i=0; i<=ny-1; i++)
    {
        y->ptr.p_double[i] = s->v.ptr.pp_double[i][nx];
        for(j=0; j<=nx-1; j++)
        {
            y->ptr.p_double[i] = y->ptr.p_double[i]+s->v.ptr.pp_double[i][j]*x->ptr.p_double[j];
            dy->ptr.p_double[i*nx+j] = s->v.ptr.pp_double[i][j];
        }
    }
    if( s->nh==0 )
    {
        return;
    }
    
    /*
     * Handle nonlinear term
     */
    rbfv2_allocatecalcbuffer(s, buf, _state);
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
    for(levelidx=0; levelidx<=s->nh-1; levelidx++)
    {
        
        /*
         * Prepare fields of Buf required by PartialCalcRec()
         */
        buf->curdist2 = (double)(0);
        for(j=0; j<=nx-1; j++)
        {
            buf->curboxmin.ptr.p_double[j] = s->kdboxmin.ptr.p_double[j];
            buf->curboxmax.ptr.p_double[j] = s->kdboxmax.ptr.p_double[j];
            if( ae_fp_less(buf->x.ptr.p_double[j],buf->curboxmin.ptr.p_double[j]) )
            {
                buf->curdist2 = buf->curdist2+ae_sqr(buf->curboxmin.ptr.p_double[j]-buf->x.ptr.p_double[j], _state);
            }
            else
            {
                if( ae_fp_greater(buf->x.ptr.p_double[j],buf->curboxmax.ptr.p_double[j]) )
                {
                    buf->curdist2 = buf->curdist2+ae_sqr(buf->x.ptr.p_double[j]-buf->curboxmax.ptr.p_double[j], _state);
                }
            }
        }
        
        /*
         * Call PartialCalcRec()
         */
        rcur = s->ri.ptr.p_double[levelidx];
        invrc2 = (double)1/(rcur*rcur);
        rquery2 = ae_sqr(rcur*rbfv2farradius(s->bf, _state), _state);
        rbfv2_partialcalcrec(s, buf, s->kdroots.ptr.p_int[levelidx], invrc2, rquery2, &buf->x, y, dy, dy, 1, _state);
    }
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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t levelidx;
    double rcur;
    double rquery2;
    double invrc2;
    ae_int_t nx;
    ae_int_t ny;


    ae_assert(x->cnt>=s->nx, "RBFDiffBuf: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFDiffBuf: X contains infinite or NaN values", _state);
    nx = s->nx;
    ny = s->ny;
    if( y->cnt<s->ny )
    {
        ae_vector_set_length(y, s->ny, _state);
    }
    if( dy->cnt<s->ny*s->nx )
    {
        ae_vector_set_length(dy, s->ny*s->nx, _state);
    }
    if( d2y->cnt<ny*nx*nx )
    {
        ae_vector_set_length(d2y, ny*nx*nx, _state);
    }
    
    /*
     * Handle linear term
     */
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
    if( s->nh==0 )
    {
        return;
    }
    
    /*
     * Handle nonlinear term
     */
    rbfv2_allocatecalcbuffer(s, buf, _state);
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
    for(levelidx=0; levelidx<=s->nh-1; levelidx++)
    {
        
        /*
         * Prepare fields of Buf required by PartialCalcRec()
         */
        buf->curdist2 = (double)(0);
        for(j=0; j<=nx-1; j++)
        {
            buf->curboxmin.ptr.p_double[j] = s->kdboxmin.ptr.p_double[j];
            buf->curboxmax.ptr.p_double[j] = s->kdboxmax.ptr.p_double[j];
            if( ae_fp_less(buf->x.ptr.p_double[j],buf->curboxmin.ptr.p_double[j]) )
            {
                buf->curdist2 = buf->curdist2+ae_sqr(buf->curboxmin.ptr.p_double[j]-buf->x.ptr.p_double[j], _state);
            }
            else
            {
                if( ae_fp_greater(buf->x.ptr.p_double[j],buf->curboxmax.ptr.p_double[j]) )
                {
                    buf->curdist2 = buf->curdist2+ae_sqr(buf->x.ptr.p_double[j]-buf->curboxmax.ptr.p_double[j], _state);
                }
            }
        }
        
        /*
         * Call PartialCalcRec()
         */
        rcur = s->ri.ptr.p_double[levelidx];
        invrc2 = (double)1/(rcur*rcur);
        rquery2 = ae_sqr(rcur*rbfv2farradius(s->bf, _state), _state);
        rbfv2_partialcalcrec(s, buf, s->kdroots.ptr.p_int[levelidx], invrc2, rquery2, &buf->x, y, dy, d2y, 2, _state);
    }
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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector cpx0;
    ae_vector cpx1;
    ae_vector dummyx2;
    ae_vector dummyx3;
    ae_vector dummyflag;
    ae_vector p01;
    ae_vector p11;
    ae_vector p2;
    ae_vector vy;
    ae_int_t i;
    ae_int_t j;

    ae_frame_make(_state, &_frame_block);
    memset(&cpx0, 0, sizeof(cpx0));
    memset(&cpx1, 0, sizeof(cpx1));
    memset(&dummyx2, 0, sizeof(dummyx2));
    memset(&dummyx3, 0, sizeof(dummyx3));
    memset(&dummyflag, 0, sizeof(dummyflag));
    memset(&p01, 0, sizeof(p01));
    memset(&p11, 0, sizeof(p11));
    memset(&p2, 0, sizeof(p2));
    memset(&vy, 0, sizeof(vy));
    ae_matrix_clear(y);
    ae_vector_init(&cpx0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&cpx1, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&dummyx2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&dummyx3, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&dummyflag, 0, DT_BOOL, _state, ae_true);
    ae_vector_init(&p01, 0, DT_INT, _state, ae_true);
    ae_vector_init(&p11, 0, DT_INT, _state, ae_true);
    ae_vector_init(&p2, 0, DT_INT, _state, ae_true);
    ae_vector_init(&vy, 0, DT_REAL, _state, ae_true);

    ae_assert(n0>0, "RBFGridCalc2: invalid value for N0 (N0<=0)!", _state);
    ae_assert(n1>0, "RBFGridCalc2: invalid value for N1 (N1<=0)!", _state);
    ae_assert(x0->cnt>=n0, "RBFGridCalc2: Length(X0)<N0", _state);
    ae_assert(x1->cnt>=n1, "RBFGridCalc2: Length(X1)<N1", _state);
    ae_assert(isfinitevector(x0, n0, _state), "RBFGridCalc2: X0 contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(x1, n1, _state), "RBFGridCalc2: X1 contains infinite or NaN values!", _state);
    ae_matrix_set_length(y, n0, n1, _state);
    for(i=0; i<=n0-1; i++)
    {
        for(j=0; j<=n1-1; j++)
        {
            y->ptr.pp_double[i][j] = (double)(0);
        }
    }
    if( s->ny!=1||s->nx!=2 )
    {
        ae_frame_leave(_state);
        return;
    }
    
    /*
     *create and sort arrays
     */
    ae_vector_set_length(&cpx0, n0, _state);
    for(i=0; i<=n0-1; i++)
    {
        cpx0.ptr.p_double[i] = x0->ptr.p_double[i];
    }
    tagsort(&cpx0, n0, &p01, &p2, _state);
    ae_vector_set_length(&cpx1, n1, _state);
    for(i=0; i<=n1-1; i++)
    {
        cpx1.ptr.p_double[i] = x1->ptr.p_double[i];
    }
    tagsort(&cpx1, n1, &p11, &p2, _state);
    ae_vector_set_length(&dummyx2, 1, _state);
    dummyx2.ptr.p_double[0] = (double)(0);
    ae_vector_set_length(&dummyx3, 1, _state);
    dummyx3.ptr.p_double[0] = (double)(0);
    ae_vector_set_length(&vy, n0*n1, _state);
    rbfv2gridcalcvx(s, &cpx0, n0, &cpx1, n1, &dummyx2, 1, &dummyx3, 1, &dummyflag, ae_false, &vy, _state);
    for(i=0; i<=n0-1; i++)
    {
        for(j=0; j<=n1-1; j++)
        {
            y->ptr.pp_double[i][j] = vy.ptr.p_double[i+j*n0];
        }
    }
    ae_frame_leave(_state);
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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t nx;
    ae_int_t ny;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_vector tx;
    ae_vector ty;
    ae_vector z;
    ae_int_t dstoffs;
    ae_int_t dummy;
    rbfv2gridcalcbuffer bufseedv2;
    ae_shared_pool bufpool;
    ae_int_t rowidx;
    ae_int_t rowcnt;
    double v;
    double rcur;
    ae_int_t levelidx;
    double searchradius2;
    ae_int_t ntrials;
    double avgfuncpernode;
    hqrndstate rs;
    ae_vector blocks0;
    ae_vector blocks1;
    ae_vector blocks2;
    ae_vector blocks3;
    ae_int_t blockscnt0;
    ae_int_t blockscnt1;
    ae_int_t blockscnt2;
    ae_int_t blockscnt3;
    double blockwidth0;
    double blockwidth1;
    double blockwidth2;
    double blockwidth3;
    ae_int_t maxblocksize;

    ae_frame_make(_state, &_frame_block);
    memset(&tx, 0, sizeof(tx));
    memset(&ty, 0, sizeof(ty));
    memset(&z, 0, sizeof(z));
    memset(&bufseedv2, 0, sizeof(bufseedv2));
    memset(&bufpool, 0, sizeof(bufpool));
    memset(&rs, 0, sizeof(rs));
    memset(&blocks0, 0, sizeof(blocks0));
    memset(&blocks1, 0, sizeof(blocks1));
    memset(&blocks2, 0, sizeof(blocks2));
    memset(&blocks3, 0, sizeof(blocks3));
    ae_vector_init(&tx, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ty, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&z, 0, DT_REAL, _state, ae_true);
    _rbfv2gridcalcbuffer_init(&bufseedv2, _state, ae_true);
    ae_shared_pool_init(&bufpool, _state, ae_true);
    _hqrndstate_init(&rs, _state, ae_true);
    ae_vector_init(&blocks0, 0, DT_INT, _state, ae_true);
    ae_vector_init(&blocks1, 0, DT_INT, _state, ae_true);
    ae_vector_init(&blocks2, 0, DT_INT, _state, ae_true);
    ae_vector_init(&blocks3, 0, DT_INT, _state, ae_true);

    nx = s->nx;
    ny = s->ny;
    hqrndseed(532, 54734, &rs, _state);
    
    /*
     * Perform integrity checks
     */
    ae_assert(s->nx==2||s->nx==3, "RBFGridCalcVX: integrity check failed", _state);
    ae_assert(s->nx>=4||((x3->cnt>=1&&ae_fp_eq(x3->ptr.p_double[0],(double)(0)))&&n3==1), "RBFGridCalcVX: integrity check failed", _state);
    ae_assert(s->nx>=3||((x2->cnt>=1&&ae_fp_eq(x2->ptr.p_double[0],(double)(0)))&&n2==1), "RBFGridCalcVX: integrity check failed", _state);
    ae_assert(s->nx>=2||((x1->cnt>=1&&ae_fp_eq(x1->ptr.p_double[0],(double)(0)))&&n1==1), "RBFGridCalcVX: integrity check failed", _state);
    
    /*
     * Allocate arrays
     */
    ae_assert(s->nx<=4, "RBFGridCalcVX: integrity check failed", _state);
    ae_vector_set_length(&z, ny, _state);
    ae_vector_set_length(&tx, 4, _state);
    ae_vector_set_length(&ty, ny, _state);
    
    /*
     * Calculate linear term
     */
    rowcnt = n1*n2*n3;
    for(rowidx=0; rowidx<=rowcnt-1; rowidx++)
    {
        
        /*
         * Calculate TX - current position
         */
        k = rowidx;
        tx.ptr.p_double[0] = (double)(0);
        tx.ptr.p_double[1] = x1->ptr.p_double[k%n1];
        k = k/n1;
        tx.ptr.p_double[2] = x2->ptr.p_double[k%n2];
        k = k/n2;
        tx.ptr.p_double[3] = x3->ptr.p_double[k%n3];
        k = k/n3;
        ae_assert(k==0, "RBFGridCalcVX: integrity check failed", _state);
        for(j=0; j<=ny-1; j++)
        {
            v = s->v.ptr.pp_double[j][nx];
            for(k=1; k<=nx-1; k++)
            {
                v = v+tx.ptr.p_double[k]*s->v.ptr.pp_double[j][k];
            }
            z.ptr.p_double[j] = v;
        }
        for(i=0; i<=n0-1; i++)
        {
            dstoffs = ny*(rowidx*n0+i);
            if( sparsey&&!flagy->ptr.p_bool[rowidx*n0+i] )
            {
                for(j=0; j<=ny-1; j++)
                {
                    y->ptr.p_double[j+dstoffs] = (double)(0);
                }
                continue;
            }
            v = x0->ptr.p_double[i];
            for(j=0; j<=ny-1; j++)
            {
                y->ptr.p_double[j+dstoffs] = z.ptr.p_double[j]+v*s->v.ptr.pp_double[j][0];
            }
        }
    }
    if( s->nh==0 )
    {
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Process RBF terms, layer by layer
     */
    for(levelidx=0; levelidx<=s->nh-1; levelidx++)
    {
        rcur = s->ri.ptr.p_double[levelidx];
        blockwidth0 = (double)(1);
        blockwidth1 = (double)(1);
        blockwidth2 = (double)(1);
        blockwidth3 = (double)(1);
        if( nx>=1 )
        {
            blockwidth0 = rcur*s->s.ptr.p_double[0];
        }
        if( nx>=2 )
        {
            blockwidth1 = rcur*s->s.ptr.p_double[1];
        }
        if( nx>=3 )
        {
            blockwidth2 = rcur*s->s.ptr.p_double[2];
        }
        if( nx>=4 )
        {
            blockwidth3 = rcur*s->s.ptr.p_double[3];
        }
        maxblocksize = 8;
        
        /*
         * Group grid nodes into blocks according to current radius
         */
        ae_vector_set_length(&blocks0, n0+1, _state);
        blockscnt0 = 0;
        blocks0.ptr.p_int[0] = 0;
        for(i=1; i<=n0-1; i++)
        {
            if( ae_fp_greater(x0->ptr.p_double[i]-x0->ptr.p_double[blocks0.ptr.p_int[blockscnt0]],blockwidth0)||i-blocks0.ptr.p_int[blockscnt0]>=maxblocksize )
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
            if( ae_fp_greater(x1->ptr.p_double[i]-x1->ptr.p_double[blocks1.ptr.p_int[blockscnt1]],blockwidth1)||i-blocks1.ptr.p_int[blockscnt1]>=maxblocksize )
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
            if( ae_fp_greater(x2->ptr.p_double[i]-x2->ptr.p_double[blocks2.ptr.p_int[blockscnt2]],blockwidth2)||i-blocks2.ptr.p_int[blockscnt2]>=maxblocksize )
            {
                inc(&blockscnt2, _state);
                blocks2.ptr.p_int[blockscnt2] = i;
            }
        }
        inc(&blockscnt2, _state);
        blocks2.ptr.p_int[blockscnt2] = n2;
        ae_vector_set_length(&blocks3, n3+1, _state);
        blockscnt3 = 0;
        blocks3.ptr.p_int[0] = 0;
        for(i=1; i<=n3-1; i++)
        {
            if( ae_fp_greater(x3->ptr.p_double[i]-x3->ptr.p_double[blocks3.ptr.p_int[blockscnt3]],blockwidth3)||i-blocks3.ptr.p_int[blockscnt3]>=maxblocksize )
            {
                inc(&blockscnt3, _state);
                blocks3.ptr.p_int[blockscnt3] = i;
            }
        }
        inc(&blockscnt3, _state);
        blocks3.ptr.p_int[blockscnt3] = n3;
        
        /*
         * Prepare seed for shared pool
         */
        rbfv2_allocatecalcbuffer(s, &bufseedv2.calcbuf, _state);
        ae_shared_pool_set_seed(&bufpool, &bufseedv2, (ae_int_t)sizeof(bufseedv2), (ae_constructor)_rbfv2gridcalcbuffer_init, (ae_copy_constructor)_rbfv2gridcalcbuffer_init_copy, (ae_destructor)_rbfv2gridcalcbuffer_destroy, _state);
        
        /*
         * Determine average number of neighbor per node
         */
        searchradius2 = ae_sqr(rcur*rbfv2farradius(s->bf, _state), _state);
        ntrials = 100;
        avgfuncpernode = 0.0;
        for(i=0; i<=ntrials-1; i++)
        {
            tx.ptr.p_double[0] = x0->ptr.p_double[hqrnduniformi(&rs, n0, _state)];
            tx.ptr.p_double[1] = x1->ptr.p_double[hqrnduniformi(&rs, n1, _state)];
            tx.ptr.p_double[2] = x2->ptr.p_double[hqrnduniformi(&rs, n2, _state)];
            tx.ptr.p_double[3] = x3->ptr.p_double[hqrnduniformi(&rs, n3, _state)];
            rbfv2_preparepartialquery(&tx, &s->kdboxmin, &s->kdboxmax, nx, &bufseedv2.calcbuf, &dummy, _state);
            avgfuncpernode = avgfuncpernode+(double)rbfv2_partialcountrec(&s->kdnodes, &s->kdsplits, &s->cw, nx, ny, &bufseedv2.calcbuf, s->kdroots.ptr.p_int[levelidx], searchradius2, &tx, _state)/(double)ntrials;
        }
        
        /*
         * Perform calculation in multithreaded mode
         */
        rbfv2partialgridcalcrec(s, x0, n0, x1, n1, x2, n2, x3, n3, &blocks0, 0, blockscnt0, &blocks1, 0, blockscnt1, &blocks2, 0, blockscnt2, &blocks3, 0, blockscnt3, flagy, sparsey, levelidx, avgfuncpernode, &bufpool, y, _state);
    }
    ae_frame_leave(_state);
}


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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t nx;
    ae_int_t ny;
    ae_int_t k;
    ae_int_t l;
    ae_int_t blkidx;
    ae_int_t blkcnt;
    ae_int_t nodeidx;
    ae_int_t nodescnt;
    ae_int_t rowidx;
    ae_int_t rowscnt;
    ae_int_t i0;
    ae_int_t i1;
    ae_int_t i2;
    ae_int_t i3;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t j2;
    ae_int_t j3;
    double rcur;
    double invrc2;
    double rquery2;
    double rfar2;
    ae_int_t dstoffs;
    ae_int_t srcoffs;
    ae_int_t dummy;
    double rowwidth;
    double maxrowwidth;
    double problemcost;
    ae_int_t maxbs;
    ae_int_t midpoint;
    ae_bool emptyrow;
    rbfv2gridcalcbuffer *buf;
    ae_smart_ptr _buf;

    ae_frame_make(_state, &_frame_block);
    memset(&_buf, 0, sizeof(_buf));
    ae_smart_ptr_init(&_buf, (void**)&buf, _state, ae_true);

    nx = s->nx;
    ny = s->ny;
    
    /*
     * Integrity checks
     */
    ae_assert(s->nx==2||s->nx==3, "RBFV2PartialGridCalcRec: integrity check failed", _state);
    
    /*
     * Try to split large problem
     */
    problemcost = (double)(s->ny*2)*(avgfuncpernode+(double)1);
    problemcost = problemcost*(double)(blocks0->ptr.p_int[block0b]-blocks0->ptr.p_int[block0a]);
    problemcost = problemcost*(double)(blocks1->ptr.p_int[block1b]-blocks1->ptr.p_int[block1a]);
    problemcost = problemcost*(double)(blocks2->ptr.p_int[block2b]-blocks2->ptr.p_int[block2a]);
    problemcost = problemcost*(double)(blocks3->ptr.p_int[block3b]-blocks3->ptr.p_int[block3a]);
    maxbs = 0;
    maxbs = ae_maxint(maxbs, block0b-block0a, _state);
    maxbs = ae_maxint(maxbs, block1b-block1a, _state);
    maxbs = ae_maxint(maxbs, block2b-block2a, _state);
    maxbs = ae_maxint(maxbs, block3b-block3a, _state);
    if( ae_fp_greater_eq(problemcost*rbfv2_complexitymultiplier,smpactivationlevel(_state)) )
    {
        if( _trypexec_rbfv2partialgridcalcrec(s,x0,n0,x1,n1,x2,n2,x3,n3,blocks0,block0a,block0b,blocks1,block1a,block1b,blocks2,block2a,block2b,blocks3,block3a,block3b,flagy,sparsey,levelidx,avgfuncpernode,bufpool,y, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            ae_frame_leave(_state);
            return;
        }
    }
    if( ae_fp_greater_eq(problemcost*rbfv2_complexitymultiplier,spawnlevel(_state))&&maxbs>=2 )
    {
        ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_true, _state);
        if( block0b-block0a==maxbs )
        {
            midpoint = block0a+maxbs/2;
            _spawn_rbfv2partialgridcalcrec(s, x0, n0, x1, n1, x2, n2, x3, n3, blocks0, block0a, midpoint, blocks1, block1a, block1b, blocks2, block2a, block2b, blocks3, block3a, block3b, flagy, sparsey, levelidx, avgfuncpernode, bufpool, y, _child_tasks, _smp_enabled, _state);
            rbfv2partialgridcalcrec(s, x0, n0, x1, n1, x2, n2, x3, n3, blocks0, midpoint, block0b, blocks1, block1a, block1b, blocks2, block2a, block2b, blocks3, block3a, block3b, flagy, sparsey, levelidx, avgfuncpernode, bufpool, y, _state);
            ae_sync(_child_tasks, _smp_enabled, ae_false, _state);
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            ae_frame_leave(_state);
            return;
        }
        if( block1b-block1a==maxbs )
        {
            midpoint = block1a+maxbs/2;
            _spawn_rbfv2partialgridcalcrec(s, x0, n0, x1, n1, x2, n2, x3, n3, blocks0, block0a, block0b, blocks1, block1a, midpoint, blocks2, block2a, block2b, blocks3, block3a, block3b, flagy, sparsey, levelidx, avgfuncpernode, bufpool, y, _child_tasks, _smp_enabled, _state);
            rbfv2partialgridcalcrec(s, x0, n0, x1, n1, x2, n2, x3, n3, blocks0, block0a, block0b, blocks1, midpoint, block1b, blocks2, block2a, block2b, blocks3, block3a, block3b, flagy, sparsey, levelidx, avgfuncpernode, bufpool, y, _state);
            ae_sync(_child_tasks, _smp_enabled, ae_false, _state);
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            ae_frame_leave(_state);
            return;
        }
        if( block2b-block2a==maxbs )
        {
            midpoint = block2a+maxbs/2;
            _spawn_rbfv2partialgridcalcrec(s, x0, n0, x1, n1, x2, n2, x3, n3, blocks0, block0a, block0b, blocks1, block1a, block1b, blocks2, block2a, midpoint, blocks3, block3a, block3b, flagy, sparsey, levelidx, avgfuncpernode, bufpool, y, _child_tasks, _smp_enabled, _state);
            rbfv2partialgridcalcrec(s, x0, n0, x1, n1, x2, n2, x3, n3, blocks0, block0a, block0b, blocks1, block1a, block1b, blocks2, midpoint, block2b, blocks3, block3a, block3b, flagy, sparsey, levelidx, avgfuncpernode, bufpool, y, _state);
            ae_sync(_child_tasks, _smp_enabled, ae_false, _state);
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            ae_frame_leave(_state);
            return;
        }
        if( block3b-block3a==maxbs )
        {
            midpoint = block3a+maxbs/2;
            _spawn_rbfv2partialgridcalcrec(s, x0, n0, x1, n1, x2, n2, x3, n3, blocks0, block0a, block0b, blocks1, block1a, block1b, blocks2, block2a, block2b, blocks3, block3a, midpoint, flagy, sparsey, levelidx, avgfuncpernode, bufpool, y, _child_tasks, _smp_enabled, _state);
            rbfv2partialgridcalcrec(s, x0, n0, x1, n1, x2, n2, x3, n3, blocks0, block0a, block0b, blocks1, block1a, block1b, blocks2, block2a, block2b, blocks3, midpoint, block3b, flagy, sparsey, levelidx, avgfuncpernode, bufpool, y, _state);
            ae_sync(_child_tasks, _smp_enabled, ae_false, _state);
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            ae_frame_leave(_state);
            return;
        }
        ae_assert(ae_false, "RBFV2PartialGridCalcRec: integrity check failed", _state);
    }
    
    /*
     * Retrieve buffer object from pool (it will be returned later)
     */
    ae_shared_pool_retrieve(bufpool, &_buf, _state);
    
    /*
     * Calculate RBF model
     */
    ae_assert(nx<=4, "RBFV2PartialGridCalcRec: integrity check failed", _state);
    ae_vector_set_length(&buf->tx, 4, _state);
    ae_vector_set_length(&buf->cx, 4, _state);
    ae_vector_set_length(&buf->ty, ny, _state);
    rcur = s->ri.ptr.p_double[levelidx];
    invrc2 = (double)1/(rcur*rcur);
    blkcnt = (block3b-block3a)*(block2b-block2a)*(block1b-block1a)*(block0b-block0a);
    for(blkidx=0; blkidx<=blkcnt-1; blkidx++)
    {
        
        /*
         * Select block (I0,I1,I2,I3).
         *
         * NOTE: for problems with NX<4 corresponding I_? are zero.
         */
        k = blkidx;
        i0 = block0a+k%(block0b-block0a);
        k = k/(block0b-block0a);
        i1 = block1a+k%(block1b-block1a);
        k = k/(block1b-block1a);
        i2 = block2a+k%(block2b-block2a);
        k = k/(block2b-block2a);
        i3 = block3a+k%(block3b-block3a);
        k = k/(block3b-block3a);
        ae_assert(k==0, "RBFV2PartialGridCalcRec: integrity check failed", _state);
        
        /*
         * We partitioned grid into blocks and selected block with
         * index (I0,I1,I2,I3). This block is a 4D cube (some dimensions
         * may be zero) of nodes with indexes (J0,J1,J2,J3), which is
         * further partitioned into a set of rows, each row corresponding
         * to indexes J1...J3 being fixed.
         *
         * We process block row by row, and each row may be handled
         * by either "generic" (nodes are processed separately) or
         * batch algorithm (that's the reason to use rows, after all).
         *
         *
         * Process nodes of the block
         */
        rowscnt = (blocks3->ptr.p_int[i3+1]-blocks3->ptr.p_int[i3])*(blocks2->ptr.p_int[i2+1]-blocks2->ptr.p_int[i2])*(blocks1->ptr.p_int[i1+1]-blocks1->ptr.p_int[i1]);
        for(rowidx=0; rowidx<=rowscnt-1; rowidx++)
        {
            
            /*
             * Find out node indexes (*,J1,J2,J3).
             *
             * NOTE: for problems with NX<4 corresponding J_? are zero.
             */
            k = rowidx;
            j1 = blocks1->ptr.p_int[i1]+k%(blocks1->ptr.p_int[i1+1]-blocks1->ptr.p_int[i1]);
            k = k/(blocks1->ptr.p_int[i1+1]-blocks1->ptr.p_int[i1]);
            j2 = blocks2->ptr.p_int[i2]+k%(blocks2->ptr.p_int[i2+1]-blocks2->ptr.p_int[i2]);
            k = k/(blocks2->ptr.p_int[i2+1]-blocks2->ptr.p_int[i2]);
            j3 = blocks3->ptr.p_int[i3]+k%(blocks3->ptr.p_int[i3+1]-blocks3->ptr.p_int[i3]);
            k = k/(blocks3->ptr.p_int[i3+1]-blocks3->ptr.p_int[i3]);
            ae_assert(k==0, "RBFV2PartialGridCalcRec: integrity check failed", _state);
            
            /*
             * Analyze row, skip completely empty rows
             */
            nodescnt = blocks0->ptr.p_int[i0+1]-blocks0->ptr.p_int[i0];
            srcoffs = blocks0->ptr.p_int[i0]+(j1+(j2+j3*n2)*n1)*n0;
            emptyrow = ae_true;
            for(nodeidx=0; nodeidx<=nodescnt-1; nodeidx++)
            {
                emptyrow = emptyrow&&(sparsey&&!flagy->ptr.p_bool[srcoffs+nodeidx]);
            }
            if( emptyrow )
            {
                continue;
            }
            
            /*
             * Process row - use either "batch" (rowsize>1) or "generic"
             * (row size is 1) algorithm.
             *
             * NOTE: "generic" version may also be used as fallback code for
             *       situations when we do not want to use batch code.
             */
            maxrowwidth = 0.5*rbfv2nearradius(s->bf, _state)*rcur*s->s.ptr.p_double[0];
            rowwidth = x0->ptr.p_double[blocks0->ptr.p_int[i0+1]-1]-x0->ptr.p_double[blocks0->ptr.p_int[i0]];
            if( nodescnt>1&&ae_fp_less_eq(rowwidth,maxrowwidth) )
            {
                
                /*
                 * "Batch" code which processes entire row at once, saving
                 * some time in kd-tree search code.
                 */
                rquery2 = ae_sqr(rcur*rbfv2farradius(s->bf, _state)+0.5*rowwidth/s->s.ptr.p_double[0], _state);
                rfar2 = ae_sqr(rcur*rbfv2farradius(s->bf, _state), _state);
                j0 = blocks0->ptr.p_int[i0];
                if( nx>0 )
                {
                    buf->cx.ptr.p_double[0] = (x0->ptr.p_double[j0]+0.5*rowwidth)/s->s.ptr.p_double[0];
                }
                if( nx>1 )
                {
                    buf->cx.ptr.p_double[1] = x1->ptr.p_double[j1]/s->s.ptr.p_double[1];
                }
                if( nx>2 )
                {
                    buf->cx.ptr.p_double[2] = x2->ptr.p_double[j2]/s->s.ptr.p_double[2];
                }
                if( nx>3 )
                {
                    buf->cx.ptr.p_double[3] = x3->ptr.p_double[j3]/s->s.ptr.p_double[3];
                }
                srcoffs = j0+(j1+(j2+j3*n2)*n1)*n0;
                dstoffs = ny*srcoffs;
                rvectorsetlengthatleast(&buf->rx, nodescnt, _state);
                bvectorsetlengthatleast(&buf->rf, nodescnt, _state);
                rvectorsetlengthatleast(&buf->ry, nodescnt*ny, _state);
                for(nodeidx=0; nodeidx<=nodescnt-1; nodeidx++)
                {
                    buf->rx.ptr.p_double[nodeidx] = x0->ptr.p_double[j0+nodeidx]/s->s.ptr.p_double[0];
                    buf->rf.ptr.p_bool[nodeidx] = !sparsey||flagy->ptr.p_bool[srcoffs+nodeidx];
                }
                for(k=0; k<=nodescnt*ny-1; k++)
                {
                    buf->ry.ptr.p_double[k] = (double)(0);
                }
                rbfv2_preparepartialquery(&buf->cx, &s->kdboxmin, &s->kdboxmax, nx, &buf->calcbuf, &dummy, _state);
                rbfv2_partialrowcalcrec(s, &buf->calcbuf, s->kdroots.ptr.p_int[levelidx], invrc2, rquery2, rfar2, &buf->cx, &buf->rx, &buf->rf, nodescnt, &buf->ry, _state);
                for(k=0; k<=nodescnt*ny-1; k++)
                {
                    y->ptr.p_double[dstoffs+k] = y->ptr.p_double[dstoffs+k]+buf->ry.ptr.p_double[k];
                }
            }
            else
            {
                
                /*
                 * "Generic" code. Although we usually move here
                 * only when NodesCnt=1, we still use a loop on
                 * NodeIdx just to be able to use this branch as
                 * fallback code without any modifications.
                 */
                rquery2 = ae_sqr(rcur*rbfv2farradius(s->bf, _state), _state);
                for(nodeidx=0; nodeidx<=nodescnt-1; nodeidx++)
                {
                    
                    /*
                     * Prepare TX - current point
                     */
                    j0 = blocks0->ptr.p_int[i0]+nodeidx;
                    if( nx>0 )
                    {
                        buf->tx.ptr.p_double[0] = x0->ptr.p_double[j0]/s->s.ptr.p_double[0];
                    }
                    if( nx>1 )
                    {
                        buf->tx.ptr.p_double[1] = x1->ptr.p_double[j1]/s->s.ptr.p_double[1];
                    }
                    if( nx>2 )
                    {
                        buf->tx.ptr.p_double[2] = x2->ptr.p_double[j2]/s->s.ptr.p_double[2];
                    }
                    if( nx>3 )
                    {
                        buf->tx.ptr.p_double[3] = x3->ptr.p_double[j3]/s->s.ptr.p_double[3];
                    }
                    
                    /*
                     * Evaluate and add to Y
                     */
                    srcoffs = j0+(j1+(j2+j3*n2)*n1)*n0;
                    dstoffs = ny*srcoffs;
                    for(l=0; l<=ny-1; l++)
                    {
                        buf->ty.ptr.p_double[l] = (double)(0);
                    }
                    if( !sparsey||flagy->ptr.p_bool[srcoffs] )
                    {
                        rbfv2_preparepartialquery(&buf->tx, &s->kdboxmin, &s->kdboxmax, nx, &buf->calcbuf, &dummy, _state);
                        rbfv2_partialcalcrec(s, &buf->calcbuf, s->kdroots.ptr.p_int[levelidx], invrc2, rquery2, &buf->tx, &buf->ty, &buf->ty, &buf->ty, 0, _state);
                    }
                    for(l=0; l<=ny-1; l++)
                    {
                        y->ptr.p_double[dstoffs+l] = y->ptr.p_double[dstoffs+l]+buf->ty.ptr.p_double[l];
                    }
                }
            }
        }
    }
    
    /*
     * Recycle buffer object back to pool
     */
    ae_shared_pool_recycle(bufpool, &_buf, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
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
    /* Real    */ ae_vector* y,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        rbfv2partialgridcalcrec(s,x0,n0,x1,n1,x2,n2,x3,n3,blocks0,block0a,block0b,blocks1,block1a,block1b,blocks2,block2a,block2b,blocks3,block3a,block3b,flagy,sparsey,levelidx,avgfuncpernode,bufpool,y, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_rbfv2partialgridcalcrec;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.const_val = s;
        _task->data.parameters[1].value.const_val = x0;
        _task->data.parameters[2].value.ival = n0;
        _task->data.parameters[3].value.const_val = x1;
        _task->data.parameters[4].value.ival = n1;
        _task->data.parameters[5].value.const_val = x2;
        _task->data.parameters[6].value.ival = n2;
        _task->data.parameters[7].value.const_val = x3;
        _task->data.parameters[8].value.ival = n3;
        _task->data.parameters[9].value.const_val = blocks0;
        _task->data.parameters[10].value.ival = block0a;
        _task->data.parameters[11].value.ival = block0b;
        _task->data.parameters[12].value.const_val = blocks1;
        _task->data.parameters[13].value.ival = block1a;
        _task->data.parameters[14].value.ival = block1b;
        _task->data.parameters[15].value.const_val = blocks2;
        _task->data.parameters[16].value.ival = block2a;
        _task->data.parameters[17].value.ival = block2b;
        _task->data.parameters[18].value.const_val = blocks3;
        _task->data.parameters[19].value.ival = block3a;
        _task->data.parameters[20].value.ival = block3b;
        _task->data.parameters[21].value.const_val = flagy;
        _task->data.parameters[22].value.bval = sparsey;
        _task->data.parameters[23].value.ival = levelidx;
        _task->data.parameters[24].value.dval = avgfuncpernode;
        _task->data.parameters[25].value.val = bufpool;
        _task->data.parameters[26].value.val = y;
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
void _task_rbfv2partialgridcalcrec(ae_task_data *_data, ae_state *_state)
{
    const rbfv2model* s;
    const ae_vector* x0;
    ae_int_t n0;
    const ae_vector* x1;
    ae_int_t n1;
    const ae_vector* x2;
    ae_int_t n2;
    const ae_vector* x3;
    ae_int_t n3;
    const ae_vector* blocks0;
    ae_int_t block0a;
    ae_int_t block0b;
    const ae_vector* blocks1;
    ae_int_t block1a;
    ae_int_t block1b;
    const ae_vector* blocks2;
    ae_int_t block2a;
    ae_int_t block2b;
    const ae_vector* blocks3;
    ae_int_t block3a;
    ae_int_t block3b;
    const ae_vector* flagy;
    ae_bool sparsey;
    ae_int_t levelidx;
    double avgfuncpernode;
    ae_shared_pool* bufpool;
    ae_vector* y;
    s = (const rbfv2model*)_data->parameters[0].value.const_val;
    x0 = (const ae_vector*)_data->parameters[1].value.const_val;
    n0 = _data->parameters[2].value.ival;
    x1 = (const ae_vector*)_data->parameters[3].value.const_val;
    n1 = _data->parameters[4].value.ival;
    x2 = (const ae_vector*)_data->parameters[5].value.const_val;
    n2 = _data->parameters[6].value.ival;
    x3 = (const ae_vector*)_data->parameters[7].value.const_val;
    n3 = _data->parameters[8].value.ival;
    blocks0 = (const ae_vector*)_data->parameters[9].value.const_val;
    block0a = _data->parameters[10].value.ival;
    block0b = _data->parameters[11].value.ival;
    blocks1 = (const ae_vector*)_data->parameters[12].value.const_val;
    block1a = _data->parameters[13].value.ival;
    block1b = _data->parameters[14].value.ival;
    blocks2 = (const ae_vector*)_data->parameters[15].value.const_val;
    block2a = _data->parameters[16].value.ival;
    block2b = _data->parameters[17].value.ival;
    blocks3 = (const ae_vector*)_data->parameters[18].value.const_val;
    block3a = _data->parameters[19].value.ival;
    block3b = _data->parameters[20].value.ival;
    flagy = (const ae_vector*)_data->parameters[21].value.const_val;
    sparsey = _data->parameters[22].value.bval;
    levelidx = _data->parameters[23].value.ival;
    avgfuncpernode = _data->parameters[24].value.dval;
    bufpool = (ae_shared_pool*)_data->parameters[25].value.val;
    y = (ae_vector*)_data->parameters[26].value.val;
   ae_state_set_flags(_state, _data->flags);
   rbfv2partialgridcalcrec(s,x0,n0,x1,n1,x2,n2,x3,n3,blocks0,block0a,block0b,blocks1,block1a,block1b,blocks2,block2a,block2b,blocks3,block3a,block3b,flagy,sparsey,levelidx,avgfuncpernode,bufpool,y, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
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
    /* Real    */ ae_vector* y,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_rbfv2partialgridcalcrec;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.const_val = s;
    _task->data.parameters[1].value.const_val = x0;
    _task->data.parameters[2].value.ival = n0;
    _task->data.parameters[3].value.const_val = x1;
    _task->data.parameters[4].value.ival = n1;
    _task->data.parameters[5].value.const_val = x2;
    _task->data.parameters[6].value.ival = n2;
    _task->data.parameters[7].value.const_val = x3;
    _task->data.parameters[8].value.ival = n3;
    _task->data.parameters[9].value.const_val = blocks0;
    _task->data.parameters[10].value.ival = block0a;
    _task->data.parameters[11].value.ival = block0b;
    _task->data.parameters[12].value.const_val = blocks1;
    _task->data.parameters[13].value.ival = block1a;
    _task->data.parameters[14].value.ival = block1b;
    _task->data.parameters[15].value.const_val = blocks2;
    _task->data.parameters[16].value.ival = block2a;
    _task->data.parameters[17].value.ival = block2b;
    _task->data.parameters[18].value.const_val = blocks3;
    _task->data.parameters[19].value.ival = block3a;
    _task->data.parameters[20].value.ival = block3b;
    _task->data.parameters[21].value.const_val = flagy;
    _task->data.parameters[22].value.bval = sparsey;
    _task->data.parameters[23].value.ival = levelidx;
    _task->data.parameters[24].value.dval = avgfuncpernode;
    _task->data.parameters[25].value.val = bufpool;
    _task->data.parameters[26].value.val = y;
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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t ncactual;

    *nx = 0;
    *ny = 0;
    ae_matrix_clear(xwr);
    *nc = 0;
    ae_matrix_clear(v);

    *nx = s->nx;
    *ny = s->ny;
    *nc = 0;
    
    /*
     * Fill V
     */
    ae_matrix_set_length(v, s->ny, s->nx+1, _state);
    for(i=0; i<=s->ny-1; i++)
    {
        ae_v_move(&v->ptr.pp_double[i][0], 1, &s->v.ptr.pp_double[i][0], 1, ae_v_len(0,s->nx));
    }
    
    /*
     * Fill XWR
     */
    ae_assert(s->cw.cnt%(s->nx+s->ny)==0, "RBFV2Unpack: integrity error", _state);
    *nc = s->cw.cnt/(s->nx+s->ny);
    ncactual = 0;
    if( *nc>0 )
    {
        ae_matrix_set_length(xwr, *nc, s->nx+s->ny+s->nx, _state);
        for(i=0; i<=s->nh-1; i++)
        {
            rbfv2_partialunpackrec(&s->kdnodes, &s->kdsplits, &s->cw, &s->s, s->nx, s->ny, s->kdroots.ptr.p_int[i], s->ri.ptr.p_double[i], xwr, &ncactual, _state);
        }
    }
    ae_assert(*nc==ncactual, "RBFV2Unpack: integrity error", _state);
}


static ae_bool rbfv2_rbfv2buildlinearmodel(/* Real    */ const ae_matrix* x,
     /* Real    */ ae_matrix* y,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t modeltype,
     /* Real    */ ae_matrix* v,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector tmpy;
    ae_matrix a;
    double scaling;
    ae_vector shifting;
    double mn;
    double mx;
    ae_vector c;
    lsfitreport rep;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&tmpy, 0, sizeof(tmpy));
    memset(&a, 0, sizeof(a));
    memset(&shifting, 0, sizeof(shifting));
    memset(&c, 0, sizeof(c));
    memset(&rep, 0, sizeof(rep));
    ae_matrix_clear(v);
    ae_vector_init(&tmpy, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&a, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&shifting, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&c, 0, DT_REAL, _state, ae_true);
    _lsfitreport_init(&rep, _state, ae_true);

    ae_assert(n>=0, "BuildLinearModel: N<0", _state);
    ae_assert(nx>0, "BuildLinearModel: NX<=0", _state);
    ae_assert(ny>0, "BuildLinearModel: NY<=0", _state);
    
    /*
     * Handle degenerate case (N=0)
     */
    result = ae_true;
    ae_matrix_set_length(v, ny, nx+1, _state);
    if( n==0 )
    {
        for(j=0; j<=nx; j++)
        {
            for(i=0; i<=ny-1; i++)
            {
                v->ptr.pp_double[i][j] = (double)(0);
            }
        }
        ae_frame_leave(_state);
        return result;
    }
    
    /*
     * Allocate temporaries
     */
    ae_vector_set_length(&tmpy, n, _state);
    
    /*
     * General linear model.
     */
    if( modeltype==1 )
    {
        
        /*
         * Calculate scaling/shifting, transform variables, prepare LLS problem
         */
        ae_matrix_set_length(&a, n, nx+1, _state);
        ae_vector_set_length(&shifting, nx, _state);
        scaling = (double)(0);
        for(i=0; i<=nx-1; i++)
        {
            mn = x->ptr.pp_double[0][i];
            mx = mn;
            for(j=1; j<=n-1; j++)
            {
                if( ae_fp_greater(mn,x->ptr.pp_double[j][i]) )
                {
                    mn = x->ptr.pp_double[j][i];
                }
                if( ae_fp_less(mx,x->ptr.pp_double[j][i]) )
                {
                    mx = x->ptr.pp_double[j][i];
                }
            }
            scaling = ae_maxreal(scaling, mx-mn, _state);
            shifting.ptr.p_double[i] = 0.5*(mx+mn);
        }
        if( ae_fp_eq(scaling,(double)(0)) )
        {
            scaling = (double)(1);
        }
        else
        {
            scaling = 0.5*scaling;
        }
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=nx-1; j++)
            {
                a.ptr.pp_double[i][j] = (x->ptr.pp_double[i][j]-shifting.ptr.p_double[j])/scaling;
            }
        }
        for(i=0; i<=n-1; i++)
        {
            a.ptr.pp_double[i][nx] = (double)(1);
        }
        
        /*
         * Solve linear system in transformed variables, make backward 
         */
        for(i=0; i<=ny-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                tmpy.ptr.p_double[j] = y->ptr.pp_double[j][i];
            }
            lsfitlinear(&tmpy, &a, n, nx+1, &c, &rep, _state);
            if( rep.terminationtype<=0 )
            {
                result = ae_false;
                ae_frame_leave(_state);
                return result;
            }
            for(j=0; j<=nx-1; j++)
            {
                v->ptr.pp_double[i][j] = c.ptr.p_double[j]/scaling;
            }
            v->ptr.pp_double[i][nx] = c.ptr.p_double[nx];
            for(j=0; j<=nx-1; j++)
            {
                v->ptr.pp_double[i][nx] = v->ptr.pp_double[i][nx]-shifting.ptr.p_double[j]*v->ptr.pp_double[i][j];
            }
            for(j=0; j<=n-1; j++)
            {
                for(k=0; k<=nx-1; k++)
                {
                    y->ptr.pp_double[j][i] = y->ptr.pp_double[j][i]-x->ptr.pp_double[j][k]*v->ptr.pp_double[i][k];
                }
                y->ptr.pp_double[j][i] = y->ptr.pp_double[j][i]-v->ptr.pp_double[i][nx];
            }
        }
        ae_frame_leave(_state);
        return result;
    }
    
    /*
     * Constant model, very simple
     */
    if( modeltype==2 )
    {
        for(i=0; i<=ny-1; i++)
        {
            for(j=0; j<=nx; j++)
            {
                v->ptr.pp_double[i][j] = (double)(0);
            }
            for(j=0; j<=n-1; j++)
            {
                v->ptr.pp_double[i][nx] = v->ptr.pp_double[i][nx]+y->ptr.pp_double[j][i];
            }
            if( n>0 )
            {
                v->ptr.pp_double[i][nx] = v->ptr.pp_double[i][nx]/(double)n;
            }
            for(j=0; j<=n-1; j++)
            {
                y->ptr.pp_double[j][i] = y->ptr.pp_double[j][i]-v->ptr.pp_double[i][nx];
            }
        }
        ae_frame_leave(_state);
        return result;
    }
    
    /*
     * Zero model
     */
    ae_assert(modeltype==3, "BuildLinearModel: unknown model type", _state);
    for(i=0; i<=ny-1; i++)
    {
        for(j=0; j<=nx; j++)
        {
            v->ptr.pp_double[i][j] = (double)(0);
        }
    }
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Reallocates calcBuf if necessary, reuses previously allocated space if
possible.

  -- ALGLIB --
     Copyright 20.06.2016 by Sergey Bochkanov
*************************************************************************/
static void rbfv2_allocatecalcbuffer(const rbfv2model* s,
     rbfv2calcbuffer* buf,
     ae_state *_state)
{


    if( buf->x.cnt<s->nx )
    {
        ae_vector_set_length(&buf->x, s->nx, _state);
    }
    if( buf->curboxmin.cnt<s->nx )
    {
        ae_vector_set_length(&buf->curboxmin, s->nx, _state);
    }
    if( buf->curboxmax.cnt<s->nx )
    {
        ae_vector_set_length(&buf->curboxmax, s->nx, _state);
    }
    if( buf->x123.cnt<s->nx )
    {
        ae_vector_set_length(&buf->x123, s->nx, _state);
    }
    if( buf->y123.cnt<s->ny )
    {
        ae_vector_set_length(&buf->y123, s->ny, _state);
    }
}


/*************************************************************************
Extracts structure (and XY-values too) from  kd-tree  built  for  a  small
subset of points and appends it to multi-tree.


  -- ALGLIB --
     Copyright 20.06.2016 by Sergey Bochkanov
*************************************************************************/
static void rbfv2_convertandappendtree(const kdtree* curtree,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t ny,
     /* Integer */ ae_vector* kdnodes,
     /* Real    */ ae_vector* kdsplits,
     /* Real    */ ae_vector* cw,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t nodesbase;
    ae_int_t splitsbase;
    ae_int_t cwbase;
    ae_vector localnodes;
    ae_vector localsplits;
    ae_vector localcw;
    ae_matrix xybuf;
    ae_int_t localnodessize;
    ae_int_t localsplitssize;
    ae_int_t localcwsize;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&localnodes, 0, sizeof(localnodes));
    memset(&localsplits, 0, sizeof(localsplits));
    memset(&localcw, 0, sizeof(localcw));
    memset(&xybuf, 0, sizeof(xybuf));
    ae_vector_init(&localnodes, 0, DT_INT, _state, ae_true);
    ae_vector_init(&localsplits, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&localcw, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xybuf, 0, 0, DT_REAL, _state, ae_true);

    
    /*
     * Calculate base offsets
     */
    nodesbase = kdnodes->cnt;
    splitsbase = kdsplits->cnt;
    cwbase = cw->cnt;
    
    /*
     * Prepare local copy of tree
     */
    ae_vector_set_length(&localnodes, n*rbfv2_maxnodesize, _state);
    ae_vector_set_length(&localsplits, n, _state);
    ae_vector_set_length(&localcw, (nx+ny)*n, _state);
    localnodessize = 0;
    localsplitssize = 0;
    localcwsize = 0;
    rbfv2_converttreerec(curtree, n, nx, ny, 0, nodesbase, splitsbase, cwbase, &localnodes, &localnodessize, &localsplits, &localsplitssize, &localcw, &localcwsize, &xybuf, _state);
    
    /*
     * Append to multi-tree
     */
    ivectorresize(kdnodes, kdnodes->cnt+localnodessize, _state);
    rvectorresize(kdsplits, kdsplits->cnt+localsplitssize, _state);
    rvectorresize(cw, cw->cnt+localcwsize, _state);
    for(i=0; i<=localnodessize-1; i++)
    {
        kdnodes->ptr.p_int[nodesbase+i] = localnodes.ptr.p_int[i];
    }
    for(i=0; i<=localsplitssize-1; i++)
    {
        kdsplits->ptr.p_double[splitsbase+i] = localsplits.ptr.p_double[i];
    }
    for(i=0; i<=localcwsize-1; i++)
    {
        cw->ptr.p_double[cwbase+i] = localcw.ptr.p_double[i];
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Recurrent tree conversion

    CurTree         -   tree to convert
    N, NX, NY       -   dataset metrics
    NodeOffset      -   offset of current tree node, 0 for root
    NodesBase       -   a value which is added to intra-tree node indexes;
                        although this tree is stored in separate array, it
                        is intended to be stored in the larger tree,  with
                        localNodes being moved to offset NodesBase.
    SplitsBase      -   similarly, offset of localSplits in the final tree
    CWBase          -   similarly, offset of localCW in the final tree
*************************************************************************/
static void rbfv2_converttreerec(const kdtree* curtree,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t nodeoffset,
     ae_int_t nodesbase,
     ae_int_t splitsbase,
     ae_int_t cwbase,
     /* Integer */ ae_vector* localnodes,
     ae_int_t* localnodessize,
     /* Real    */ ae_vector* localsplits,
     ae_int_t* localsplitssize,
     /* Real    */ ae_vector* localcw,
     ae_int_t* localcwsize,
     /* Real    */ ae_matrix* xybuf,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t nodetype;
    ae_int_t cnt;
    ae_int_t d;
    double s;
    ae_int_t nodele;
    ae_int_t nodege;
    ae_int_t oldnodessize;


    kdtreeexplorenodetype(curtree, nodeoffset, &nodetype, _state);
    
    /*
     * Leaf node
     */
    if( nodetype==0 )
    {
        kdtreeexploreleaf(curtree, nodeoffset, xybuf, &cnt, _state);
        ae_assert(localnodes->cnt>=*localnodessize+2, "ConvertTreeRec: integrity check failed", _state);
        ae_assert(localcw->cnt>=*localcwsize+cnt*(nx+ny), "ConvertTreeRec: integrity check failed", _state);
        localnodes->ptr.p_int[*localnodessize+0] = cnt;
        localnodes->ptr.p_int[*localnodessize+1] = cwbase+(*localcwsize);
        *localnodessize = *localnodessize+2;
        for(i=0; i<=cnt-1; i++)
        {
            for(j=0; j<=nx+ny-1; j++)
            {
                localcw->ptr.p_double[*localcwsize+i*(nx+ny)+j] = xybuf->ptr.pp_double[i][j];
            }
        }
        *localcwsize = *localcwsize+cnt*(nx+ny);
        return;
    }
    
    /*
     * Split node
     */
    if( nodetype==1 )
    {
        kdtreeexploresplit(curtree, nodeoffset, &d, &s, &nodele, &nodege, _state);
        ae_assert(localnodes->cnt>=*localnodessize+rbfv2_maxnodesize, "ConvertTreeRec: integrity check failed", _state);
        ae_assert(localsplits->cnt>=*localsplitssize+1, "ConvertTreeRec: integrity check failed", _state);
        oldnodessize = *localnodessize;
        localnodes->ptr.p_int[*localnodessize+0] = 0;
        localnodes->ptr.p_int[*localnodessize+1] = d;
        localnodes->ptr.p_int[*localnodessize+2] = splitsbase+(*localsplitssize);
        localnodes->ptr.p_int[*localnodessize+3] = -1;
        localnodes->ptr.p_int[*localnodessize+4] = -1;
        *localnodessize = *localnodessize+5;
        localsplits->ptr.p_double[*localsplitssize+0] = s;
        *localsplitssize = *localsplitssize+1;
        localnodes->ptr.p_int[oldnodessize+3] = nodesbase+(*localnodessize);
        rbfv2_converttreerec(curtree, n, nx, ny, nodele, nodesbase, splitsbase, cwbase, localnodes, localnodessize, localsplits, localsplitssize, localcw, localcwsize, xybuf, _state);
        localnodes->ptr.p_int[oldnodessize+4] = nodesbase+(*localnodessize);
        rbfv2_converttreerec(curtree, n, nx, ny, nodege, nodesbase, splitsbase, cwbase, localnodes, localnodessize, localsplits, localsplitssize, localcw, localcwsize, xybuf, _state);
        return;
    }
    
    /*
     * Integrity error
     */
    ae_assert(ae_false, "ConvertTreeRec: integrity check failed", _state);
}


/*************************************************************************
This function performs partial calculation of  hierarchical  model:  given
evaluation point X and partially computed value Y, it updates Y by  values
computed using part of multi-tree given by RootIdx.

INPUT PARAMETERS:
    S       -   V2 model
    Buf     -   calc-buffer, this function uses following fields:
                * Buf.CurBoxMin - should be set by caller
                * Buf.CurBoxMax - should be set by caller
                * Buf.CurDist2  - squared distance from X to current bounding box,
                  should be set by caller
    RootIdx -   offset of partial kd-tree
    InvR2   -   1/R^2, where R is basis function radius
    QueryR2 -   squared query radius, usually it is (R*FarRadius(BasisFunction))^2
    X       -   evaluation point, array[NX]
    Y       -   current value for target, array[NY]
    DY      -   current value for derivative, array[NY*NX], if NeedDY>=1
    D2Y     -   current value for derivative, array[NY*NX*NX], if NeedDY>=2
    NeedDY  -   whether derivatives are required or not:
                * 0 if only Y is needed
                * 1 if Y and DY are needed
                * 2 if Y, DY, D2Y are needed
    
OUTPUT PARAMETERS
    Y       -   updated partial value
    DY      -   updated derivatives, if NeedDY>=1
    D2Y     -   updated Hessian, if NeedDY>=2

  -- ALGLIB --
     Copyright 20.06.2016 by Bochkanov Sergey
*************************************************************************/
static void rbfv2_partialcalcrec(const rbfv2model* s,
     rbfv2calcbuffer* buf,
     ae_int_t rootidx,
     double invr2,
     double queryr2,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     /* Real    */ ae_vector* dy,
     /* Real    */ ae_vector* d2y,
     ae_int_t needdy,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t k0;
    ae_int_t k1;
    double ptdist2;
    double w;
    double v;
    double v0;
    double v1;
    ae_int_t cwoffs;
    ae_int_t cwcnt;
    ae_int_t itemoffs;
    double arg;
    double val;
    double df;
    double d2f;
    ae_int_t d;
    double split;
    ae_int_t childle;
    ae_int_t childge;
    ae_int_t childoffs;
    ae_bool updatemin;
    double prevdist2;
    double t1;
    ae_int_t nx;
    ae_int_t ny;


    nx = s->nx;
    ny = s->ny;
    
    /*
     * Helps to avoid spurious warnings
     */
    val = (double)(0);
    
    /*
     * Leaf node.
     */
    if( s->kdnodes.ptr.p_int[rootidx]>0 )
    {
        cwcnt = s->kdnodes.ptr.p_int[rootidx+0];
        cwoffs = s->kdnodes.ptr.p_int[rootidx+1];
        for(i=0; i<=cwcnt-1; i++)
        {
            
            /*
             * Calculate distance
             */
            itemoffs = cwoffs+i*(nx+ny);
            ptdist2 = (double)(0);
            for(j=0; j<=nx-1; j++)
            {
                v = s->cw.ptr.p_double[itemoffs+j]-x->ptr.p_double[j];
                ptdist2 = ptdist2+v*v;
            }
            
            /*
             * Skip points if distance too large
             */
            if( ptdist2>=queryr2 )
            {
                continue;
            }
            
            /*
             * Update Y
             */
            arg = ptdist2*invr2;
            val = (double)(0);
            df = (double)(0);
            d2f = (double)(0);
            if( needdy==2 )
            {
                if( s->bf==0 )
                {
                    val = ae_exp(-arg, _state);
                    df = -val;
                    d2f = val;
                }
                else
                {
                    if( s->bf==1 )
                    {
                        rbfv2basisfuncdiff2(s->bf, arg, &val, &df, &d2f, _state);
                    }
                    else
                    {
                        ae_assert(ae_false, "PartialCalcRec: integrity check failed", _state);
                    }
                }
                for(j=0; j<=ny-1; j++)
                {
                    y->ptr.p_double[j] = y->ptr.p_double[j]+val*s->cw.ptr.p_double[itemoffs+nx+j];
                    w = s->cw.ptr.p_double[itemoffs+nx+j];
                    v = w*df*invr2*(double)2;
                    for(k0=0; k0<=nx-1; k0++)
                    {
                        for(k1=0; k1<=nx-1; k1++)
                        {
                            if( k0==k1 )
                            {
                                
                                /*
                                 * Compute derivative and diagonal element of the Hessian
                                 */
                                dy->ptr.p_double[j*nx+k0] = dy->ptr.p_double[j*nx+k0]+v*(x->ptr.p_double[k0]-s->cw.ptr.p_double[itemoffs+k0]);
                                d2y->ptr.p_double[j*nx*nx+k0*nx+k1] = d2y->ptr.p_double[j*nx*nx+k0*nx+k1]+w*(d2f*invr2*invr2*(double)4*ae_sqr(x->ptr.p_double[k0]-s->cw.ptr.p_double[itemoffs+k0], _state)+df*invr2*(double)2);
                            }
                            else
                            {
                                
                                /*
                                 * Compute offdiagonal element of the Hessian
                                 */
                                d2y->ptr.p_double[j*nx*nx+k0*nx+k1] = d2y->ptr.p_double[j*nx*nx+k0*nx+k1]+w*d2f*invr2*invr2*(double)4*(x->ptr.p_double[k0]-s->cw.ptr.p_double[itemoffs+k0])*(x->ptr.p_double[k1]-s->cw.ptr.p_double[itemoffs+k1]);
                            }
                        }
                    }
                }
            }
            if( needdy==1 )
            {
                if( s->bf==0 )
                {
                    val = ae_exp(-arg, _state);
                    df = -val;
                }
                else
                {
                    if( s->bf==1 )
                    {
                        rbfv2basisfuncdiff2(s->bf, arg, &val, &df, &d2f, _state);
                    }
                    else
                    {
                        ae_assert(ae_false, "PartialCalcRec: integrity check failed", _state);
                    }
                }
                for(j=0; j<=ny-1; j++)
                {
                    y->ptr.p_double[j] = y->ptr.p_double[j]+val*s->cw.ptr.p_double[itemoffs+nx+j];
                    v = s->cw.ptr.p_double[itemoffs+nx+j]*df*invr2*(double)2;
                    for(k=0; k<=nx-1; k++)
                    {
                        dy->ptr.p_double[j*nx+k] = dy->ptr.p_double[j*nx+k]+v*(x->ptr.p_double[k]-s->cw.ptr.p_double[itemoffs+k]);
                    }
                }
            }
            if( needdy==0 )
            {
                if( s->bf==0 )
                {
                    val = ae_exp(-arg, _state);
                }
                else
                {
                    if( s->bf==1 )
                    {
                        val = rbfv2basisfunc(s->bf, arg, _state);
                    }
                    else
                    {
                        ae_assert(ae_false, "PartialCalcRec: integrity check failed", _state);
                    }
                }
                for(j=0; j<=ny-1; j++)
                {
                    y->ptr.p_double[j] = y->ptr.p_double[j]+val*s->cw.ptr.p_double[itemoffs+nx+j];
                }
            }
        }
        return;
    }
    
    /*
     * Simple split
     */
    if( s->kdnodes.ptr.p_int[rootidx]==0 )
    {
        
        /*
         * Load:
         * * D      dimension to split
         * * Split  split position
         * * ChildLE, ChildGE - indexes of childs
         */
        d = s->kdnodes.ptr.p_int[rootidx+1];
        split = s->kdsplits.ptr.p_double[s->kdnodes.ptr.p_int[rootidx+2]];
        childle = s->kdnodes.ptr.p_int[rootidx+3];
        childge = s->kdnodes.ptr.p_int[rootidx+4];
        
        /*
         * Navigate through childs
         */
        for(i=0; i<=1; i++)
        {
            
            /*
             * Select child to process:
             * * ChildOffs      current child offset in Nodes[]
             * * UpdateMin      whether minimum or maximum value
             *                  of bounding box is changed on update
             */
            updatemin = i!=0;
            if( i==0 )
            {
                childoffs = childle;
            }
            else
            {
                childoffs = childge;
            }
            
            /*
             * Update bounding box and current distance
             */
            prevdist2 = buf->curdist2;
            t1 = x->ptr.p_double[d];
            if( updatemin )
            {
                v = buf->curboxmin.ptr.p_double[d];
                if( t1<=split )
                {
                    v0 = v-t1;
                    if( v0<(double)0 )
                    {
                        v0 = (double)(0);
                    }
                    v1 = split-t1;
                    buf->curdist2 = buf->curdist2-v0*v0+v1*v1;
                }
                buf->curboxmin.ptr.p_double[d] = split;
            }
            else
            {
                v = buf->curboxmax.ptr.p_double[d];
                if( t1>=split )
                {
                    v0 = t1-v;
                    if( v0<(double)0 )
                    {
                        v0 = (double)(0);
                    }
                    v1 = t1-split;
                    buf->curdist2 = buf->curdist2-v0*v0+v1*v1;
                }
                buf->curboxmax.ptr.p_double[d] = split;
            }
            
            /*
             * Decide: to dive into cell or not to dive
             */
            if( buf->curdist2<queryr2 )
            {
                rbfv2_partialcalcrec(s, buf, childoffs, invr2, queryr2, x, y, dy, d2y, needdy, _state);
            }
            
            /*
             * Restore bounding box and distance
             */
            if( updatemin )
            {
                buf->curboxmin.ptr.p_double[d] = v;
            }
            else
            {
                buf->curboxmax.ptr.p_double[d] = v;
            }
            buf->curdist2 = prevdist2;
        }
        return;
    }
    
    /*
     * Integrity failure
     */
    ae_assert(ae_false, "PartialCalcRec: integrity check failed", _state);
}


/*************************************************************************
This function performs same operation as partialcalcrec(), but  for entire
row of the grid. "Row" is a set of nodes (x0,x1,x2,x3) which share x1..x3,
but have different x0's. (note: for 2D/3D problems x2..x3 are zero).

Row is given by:
* central point XC, which is located at the center of the row, and used to
  perform kd-tree requests
* set of x0 coordinates stored in RX array (array may be unordered, but it
  is expected that spread of x0  is  no  more  than  R;  function  may  be
  inefficient for larger spreads).
* set of YFlag values stored in RF

INPUT PARAMETERS:
    S       -   V2 model
    Buf     -   calc-buffer, this function uses following fields:
                * Buf.CurBoxMin - should be set by caller
                * Buf.CurBoxMax - should be set by caller
                * Buf.CurDist2  - squared distance from X to current bounding box,
                  should be set by caller
    RootIdx -   offset of partial kd-tree
    InvR2   -   1/R^2, where R is basis function radius
    RQuery2 -   squared query radius, usually it is (R*FarRadius(BasisFunction)+0.5*RowWidth)^2,
                where RowWidth is its spatial  extent  (after  scaling  of
                variables). This radius is used to perform  initial  query
                for neighbors of CX.
    RFar2   -   squared far radius; far radius is used to perform actual
                filtering of results of query made with RQuery2.
    CX      -   central point, array[NX], used for queries
    RX      -   x0 coordinates, array[RowSize]
    RF      -   sparsity flags, array[RowSize]
    RowSize -   row size in elements
    RY      -   input partial value, array[NY]
    
OUTPUT PARAMETERS
    RY      -   updated partial value (function adds its results to RY)

  -- ALGLIB --
     Copyright 20.06.2016 by Bochkanov Sergey
*************************************************************************/
static void rbfv2_partialrowcalcrec(const rbfv2model* s,
     rbfv2calcbuffer* buf,
     ae_int_t rootidx,
     double invr2,
     double rquery2,
     double rfar2,
     /* Real    */ const ae_vector* cx,
     /* Real    */ const ae_vector* rx,
     /* Boolean */ const ae_vector* rf,
     ae_int_t rowsize,
     /* Real    */ ae_vector* ry,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t i0;
    ae_int_t i1;
    double partialptdist2;
    double ptdist2;
    double v;
    double v0;
    double v1;
    ae_int_t cwoffs;
    ae_int_t cwcnt;
    ae_int_t itemoffs;
    ae_int_t woffs;
    double val;
    ae_int_t d;
    double split;
    ae_int_t childle;
    ae_int_t childge;
    ae_int_t childoffs;
    ae_bool updatemin;
    double prevdist2;
    double t1;
    ae_int_t nx;
    ae_int_t ny;


    nx = s->nx;
    ny = s->ny;
    
    /*
     * Leaf node.
     */
    if( s->kdnodes.ptr.p_int[rootidx]>0 )
    {
        cwcnt = s->kdnodes.ptr.p_int[rootidx+0];
        cwoffs = s->kdnodes.ptr.p_int[rootidx+1];
        for(i0=0; i0<=cwcnt-1; i0++)
        {
            
            /*
             * Calculate partial distance (components from 1 to NX-1)
             */
            itemoffs = cwoffs+i0*(nx+ny);
            partialptdist2 = (double)(0);
            for(j=1; j<=nx-1; j++)
            {
                v = s->cw.ptr.p_double[itemoffs+j]-cx->ptr.p_double[j];
                partialptdist2 = partialptdist2+v*v;
            }
            
            /*
             * Process each element of the row
             */
            for(i1=0; i1<=rowsize-1; i1++)
            {
                if( rf->ptr.p_bool[i1] )
                {
                    
                    /*
                     * Calculate distance
                     */
                    v = s->cw.ptr.p_double[itemoffs]-rx->ptr.p_double[i1];
                    ptdist2 = partialptdist2+v*v;
                    
                    /*
                     * Skip points if distance too large
                     */
                    if( ptdist2>=rfar2 )
                    {
                        continue;
                    }
                    
                    /*
                     * Update Y
                     */
                    val = rbfv2basisfunc(s->bf, ptdist2*invr2, _state);
                    woffs = itemoffs+nx;
                    for(j=0; j<=ny-1; j++)
                    {
                        ry->ptr.p_double[j+i1*ny] = ry->ptr.p_double[j+i1*ny]+val*s->cw.ptr.p_double[woffs+j];
                    }
                }
            }
        }
        return;
    }
    
    /*
     * Simple split
     */
    if( s->kdnodes.ptr.p_int[rootidx]==0 )
    {
        
        /*
         * Load:
         * * D      dimension to split
         * * Split  split position
         * * ChildLE, ChildGE - indexes of childs
         */
        d = s->kdnodes.ptr.p_int[rootidx+1];
        split = s->kdsplits.ptr.p_double[s->kdnodes.ptr.p_int[rootidx+2]];
        childle = s->kdnodes.ptr.p_int[rootidx+3];
        childge = s->kdnodes.ptr.p_int[rootidx+4];
        
        /*
         * Navigate through childs
         */
        for(i=0; i<=1; i++)
        {
            
            /*
             * Select child to process:
             * * ChildOffs      current child offset in Nodes[]
             * * UpdateMin      whether minimum or maximum value
             *                  of bounding box is changed on update
             */
            updatemin = i!=0;
            if( i==0 )
            {
                childoffs = childle;
            }
            else
            {
                childoffs = childge;
            }
            
            /*
             * Update bounding box and current distance
             */
            prevdist2 = buf->curdist2;
            t1 = cx->ptr.p_double[d];
            if( updatemin )
            {
                v = buf->curboxmin.ptr.p_double[d];
                if( t1<=split )
                {
                    v0 = v-t1;
                    if( v0<(double)0 )
                    {
                        v0 = (double)(0);
                    }
                    v1 = split-t1;
                    buf->curdist2 = buf->curdist2-v0*v0+v1*v1;
                }
                buf->curboxmin.ptr.p_double[d] = split;
            }
            else
            {
                v = buf->curboxmax.ptr.p_double[d];
                if( t1>=split )
                {
                    v0 = t1-v;
                    if( v0<(double)0 )
                    {
                        v0 = (double)(0);
                    }
                    v1 = t1-split;
                    buf->curdist2 = buf->curdist2-v0*v0+v1*v1;
                }
                buf->curboxmax.ptr.p_double[d] = split;
            }
            
            /*
             * Decide: to dive into cell or not to dive
             */
            if( buf->curdist2<rquery2 )
            {
                rbfv2_partialrowcalcrec(s, buf, childoffs, invr2, rquery2, rfar2, cx, rx, rf, rowsize, ry, _state);
            }
            
            /*
             * Restore bounding box and distance
             */
            if( updatemin )
            {
                buf->curboxmin.ptr.p_double[d] = v;
            }
            else
            {
                buf->curboxmax.ptr.p_double[d] = v;
            }
            buf->curdist2 = prevdist2;
        }
        return;
    }
    
    /*
     * Integrity failure
     */
    ae_assert(ae_false, "PartialCalcRec: integrity check failed", _state);
}


/*************************************************************************
This function prepares partial query

INPUT PARAMETERS:
    X       -   query point
    kdBoxMin, kdBoxMax - current bounding box
    NX      -   problem size
    Buf     -   preallocated buffer; this function just loads data, but
                does not allocate place for them.
    Cnt     -   counter variable which is set to zery by this function, as
                convenience, and to remember about necessity to zero counter
                prior to calling partialqueryrec().
    
OUTPUT PARAMETERS
    Buf     -   calc-buffer:
                * Buf.CurBoxMin - current box
                * Buf.CurBoxMax - current box
                * Buf.CurDist2  - squared distance from X to current box
    Cnt     -   set to zero

  -- ALGLIB --
     Copyright 20.06.2016 by Bochkanov Sergey
*************************************************************************/
static void rbfv2_preparepartialquery(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* kdboxmin,
     /* Real    */ const ae_vector* kdboxmax,
     ae_int_t nx,
     rbfv2calcbuffer* buf,
     ae_int_t* cnt,
     ae_state *_state)
{
    ae_int_t j;


    *cnt = 0;
    buf->curdist2 = (double)(0);
    for(j=0; j<=nx-1; j++)
    {
        buf->curboxmin.ptr.p_double[j] = kdboxmin->ptr.p_double[j];
        buf->curboxmax.ptr.p_double[j] = kdboxmax->ptr.p_double[j];
        if( ae_fp_less(x->ptr.p_double[j],buf->curboxmin.ptr.p_double[j]) )
        {
            buf->curdist2 = buf->curdist2+ae_sqr(buf->curboxmin.ptr.p_double[j]-x->ptr.p_double[j], _state);
        }
        else
        {
            if( ae_fp_greater(x->ptr.p_double[j],buf->curboxmax.ptr.p_double[j]) )
            {
                buf->curdist2 = buf->curdist2+ae_sqr(x->ptr.p_double[j]-buf->curboxmax.ptr.p_double[j], _state);
            }
        }
    }
}


/*************************************************************************
This function performs partial (for just one subtree of multi-tree)  query
for neighbors located in R-sphere around X. It returns  squared  distances
from X to points and offsets in S.CW[] array for points being found.

INPUT PARAMETERS:
    kdNodes, kdSplits, CW, NX, NY - corresponding fields of V2 model
    Buf     -   calc-buffer, this function uses following fields:
                * Buf.CurBoxMin - should be set by caller
                * Buf.CurBoxMax - should be set by caller
                * Buf.CurDist2  - squared distance from X to current
                  bounding box, should be set by caller
                You may use preparepartialquery() function to initialize
                these fields.
    RootIdx -   offset of partial kd-tree
    QueryR2 -   squared query radius
    X       -   array[NX], point being queried
    R2      -   preallocated output buffer; it is caller's responsibility
                to make sure that R2 has enough space.
    Offs    -   preallocated output buffer; it is caller's responsibility
                to make sure that Offs has enough space.
    K       -   MUST BE ZERO ON INITIAL CALL. This variable is incremented,
                not set. So, any no-zero value will result in the incorrect
                points count being returned.
    
OUTPUT PARAMETERS
    R2      -   squared distances in first K elements
    Offs    -   offsets in S.CW in first K elements
    K       -   points count

  -- ALGLIB --
     Copyright 20.06.2016 by Bochkanov Sergey
*************************************************************************/
static void rbfv2_partialqueryrec(/* Integer */ const ae_vector* kdnodes,
     /* Real    */ const ae_vector* kdsplits,
     /* Real    */ const ae_vector* cw,
     ae_int_t nx,
     ae_int_t ny,
     rbfv2calcbuffer* buf,
     ae_int_t rootidx,
     double queryr2,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* r2,
     /* Integer */ ae_vector* offs,
     ae_int_t* k,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double ptdist2;
    double v;
    ae_int_t cwoffs;
    ae_int_t cwcnt;
    ae_int_t itemoffs;
    ae_int_t d;
    double split;
    ae_int_t childle;
    ae_int_t childge;
    ae_int_t childoffs;
    ae_bool updatemin;
    double prevdist2;
    double t1;


    
    /*
     * Leaf node.
     */
    if( kdnodes->ptr.p_int[rootidx]>0 )
    {
        cwcnt = kdnodes->ptr.p_int[rootidx+0];
        cwoffs = kdnodes->ptr.p_int[rootidx+1];
        for(i=0; i<=cwcnt-1; i++)
        {
            
            /*
             * Calculate distance
             */
            itemoffs = cwoffs+i*(nx+ny);
            ptdist2 = (double)(0);
            for(j=0; j<=nx-1; j++)
            {
                v = cw->ptr.p_double[itemoffs+j]-x->ptr.p_double[j];
                ptdist2 = ptdist2+v*v;
            }
            
            /*
             * Skip points if distance too large
             */
            if( ae_fp_greater_eq(ptdist2,queryr2) )
            {
                continue;
            }
            
            /*
             * Output
             */
            r2->ptr.p_double[*k] = ptdist2;
            offs->ptr.p_int[*k] = itemoffs;
            *k = *k+1;
        }
        return;
    }
    
    /*
     * Simple split
     */
    if( kdnodes->ptr.p_int[rootidx]==0 )
    {
        
        /*
         * Load:
         * * D      dimension to split
         * * Split  split position
         * * ChildLE, ChildGE - indexes of childs
         */
        d = kdnodes->ptr.p_int[rootidx+1];
        split = kdsplits->ptr.p_double[kdnodes->ptr.p_int[rootidx+2]];
        childle = kdnodes->ptr.p_int[rootidx+3];
        childge = kdnodes->ptr.p_int[rootidx+4];
        
        /*
         * Navigate through childs
         */
        for(i=0; i<=1; i++)
        {
            
            /*
             * Select child to process:
             * * ChildOffs      current child offset in Nodes[]
             * * UpdateMin      whether minimum or maximum value
             *                  of bounding box is changed on update
             */
            updatemin = i!=0;
            if( i==0 )
            {
                childoffs = childle;
            }
            else
            {
                childoffs = childge;
            }
            
            /*
             * Update bounding box and current distance
             */
            prevdist2 = buf->curdist2;
            t1 = x->ptr.p_double[d];
            if( updatemin )
            {
                v = buf->curboxmin.ptr.p_double[d];
                if( ae_fp_less_eq(t1,split) )
                {
                    buf->curdist2 = buf->curdist2-ae_sqr(ae_maxreal(v-t1, (double)(0), _state), _state)+ae_sqr(split-t1, _state);
                }
                buf->curboxmin.ptr.p_double[d] = split;
            }
            else
            {
                v = buf->curboxmax.ptr.p_double[d];
                if( ae_fp_greater_eq(t1,split) )
                {
                    buf->curdist2 = buf->curdist2-ae_sqr(ae_maxreal(t1-v, (double)(0), _state), _state)+ae_sqr(t1-split, _state);
                }
                buf->curboxmax.ptr.p_double[d] = split;
            }
            
            /*
             * Decide: to dive into cell or not to dive
             */
            if( ae_fp_less(buf->curdist2,queryr2) )
            {
                rbfv2_partialqueryrec(kdnodes, kdsplits, cw, nx, ny, buf, childoffs, queryr2, x, r2, offs, k, _state);
            }
            
            /*
             * Restore bounding box and distance
             */
            if( updatemin )
            {
                buf->curboxmin.ptr.p_double[d] = v;
            }
            else
            {
                buf->curboxmax.ptr.p_double[d] = v;
            }
            buf->curdist2 = prevdist2;
        }
        return;
    }
    
    /*
     * Integrity failure
     */
    ae_assert(ae_false, "PartialQueryRec: integrity check failed", _state);
}


/*************************************************************************
This function performs  partial  (for  just  one  subtree  of  multi-tree)
counting of neighbors located in R-sphere around X.

This function does not guarantee consistency of results with other partial
queries, it should be used only to get approximate estimates (well, we  do
not  use   approximate   algorithms,  but  rounding  errors  may  give  us
inconsistent results in just-at-the-boundary cases).

INPUT PARAMETERS:
    kdNodes, kdSplits, CW, NX, NY - corresponding fields of V2 model
    Buf     -   calc-buffer, this function uses following fields:
                * Buf.CurBoxMin - should be set by caller
                * Buf.CurBoxMax - should be set by caller
                * Buf.CurDist2  - squared distance from X to current
                  bounding box, should be set by caller
                You may use preparepartialquery() function to initialize
                these fields.
    RootIdx -   offset of partial kd-tree
    QueryR2 -   squared query radius
    X       -   array[NX], point being queried
    
RESULT:
    points count

  -- ALGLIB --
     Copyright 20.06.2016 by Bochkanov Sergey
*************************************************************************/
static ae_int_t rbfv2_partialcountrec(/* Integer */ const ae_vector* kdnodes,
     /* Real    */ const ae_vector* kdsplits,
     /* Real    */ const ae_vector* cw,
     ae_int_t nx,
     ae_int_t ny,
     rbfv2calcbuffer* buf,
     ae_int_t rootidx,
     double queryr2,
     /* Real    */ const ae_vector* x,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double ptdist2;
    double v;
    ae_int_t cwoffs;
    ae_int_t cwcnt;
    ae_int_t itemoffs;
    ae_int_t d;
    double split;
    ae_int_t childle;
    ae_int_t childge;
    ae_int_t childoffs;
    ae_bool updatemin;
    double prevdist2;
    double t1;
    ae_int_t result;


    result = 0;
    
    /*
     * Leaf node.
     */
    if( kdnodes->ptr.p_int[rootidx]>0 )
    {
        cwcnt = kdnodes->ptr.p_int[rootidx+0];
        cwoffs = kdnodes->ptr.p_int[rootidx+1];
        for(i=0; i<=cwcnt-1; i++)
        {
            
            /*
             * Calculate distance
             */
            itemoffs = cwoffs+i*(nx+ny);
            ptdist2 = (double)(0);
            for(j=0; j<=nx-1; j++)
            {
                v = cw->ptr.p_double[itemoffs+j]-x->ptr.p_double[j];
                ptdist2 = ptdist2+v*v;
            }
            
            /*
             * Skip points if distance too large
             */
            if( ae_fp_greater_eq(ptdist2,queryr2) )
            {
                continue;
            }
            
            /*
             * Output
             */
            result = result+1;
        }
        return result;
    }
    
    /*
     * Simple split
     */
    if( kdnodes->ptr.p_int[rootidx]==0 )
    {
        
        /*
         * Load:
         * * D      dimension to split
         * * Split  split position
         * * ChildLE, ChildGE - indexes of childs
         */
        d = kdnodes->ptr.p_int[rootidx+1];
        split = kdsplits->ptr.p_double[kdnodes->ptr.p_int[rootidx+2]];
        childle = kdnodes->ptr.p_int[rootidx+3];
        childge = kdnodes->ptr.p_int[rootidx+4];
        
        /*
         * Navigate through childs
         */
        for(i=0; i<=1; i++)
        {
            
            /*
             * Select child to process:
             * * ChildOffs      current child offset in Nodes[]
             * * UpdateMin      whether minimum or maximum value
             *                  of bounding box is changed on update
             */
            updatemin = i!=0;
            if( i==0 )
            {
                childoffs = childle;
            }
            else
            {
                childoffs = childge;
            }
            
            /*
             * Update bounding box and current distance
             */
            prevdist2 = buf->curdist2;
            t1 = x->ptr.p_double[d];
            if( updatemin )
            {
                v = buf->curboxmin.ptr.p_double[d];
                if( ae_fp_less_eq(t1,split) )
                {
                    buf->curdist2 = buf->curdist2-ae_sqr(ae_maxreal(v-t1, (double)(0), _state), _state)+ae_sqr(split-t1, _state);
                }
                buf->curboxmin.ptr.p_double[d] = split;
            }
            else
            {
                v = buf->curboxmax.ptr.p_double[d];
                if( ae_fp_greater_eq(t1,split) )
                {
                    buf->curdist2 = buf->curdist2-ae_sqr(ae_maxreal(t1-v, (double)(0), _state), _state)+ae_sqr(t1-split, _state);
                }
                buf->curboxmax.ptr.p_double[d] = split;
            }
            
            /*
             * Decide: to dive into cell or not to dive
             */
            if( ae_fp_less(buf->curdist2,queryr2) )
            {
                result = result+rbfv2_partialcountrec(kdnodes, kdsplits, cw, nx, ny, buf, childoffs, queryr2, x, _state);
            }
            
            /*
             * Restore bounding box and distance
             */
            if( updatemin )
            {
                buf->curboxmin.ptr.p_double[d] = v;
            }
            else
            {
                buf->curboxmax.ptr.p_double[d] = v;
            }
            buf->curdist2 = prevdist2;
        }
        return result;
    }
    
    /*
     * Integrity failure
     */
    ae_assert(ae_false, "PartialCountRec: integrity check failed", _state);
    return result;
}


/*************************************************************************
This function performs partial (for just one subtree of multi-tree) unpack
for RBF model. It appends center coordinates,  weights  and  per-dimension
radii (according to current scaling) to preallocated output array.

INPUT PARAMETERS:
    kdNodes, kdSplits, CW, S, NX, NY - corresponding fields of V2 model
    RootIdx -   offset of partial kd-tree
    R       -   radius for current partial tree
    XWR     -   preallocated output buffer; it is caller's responsibility
                to make sure that XWR has enough space. First K rows are
                already occupied.
    K       -   number of already occupied rows in XWR.
    
OUTPUT PARAMETERS
    XWR     -   updated XWR
    K       -   updated rows count

  -- ALGLIB --
     Copyright 20.06.2016 by Bochkanov Sergey
*************************************************************************/
static void rbfv2_partialunpackrec(/* Integer */ const ae_vector* kdnodes,
     /* Real    */ const ae_vector* kdsplits,
     /* Real    */ const ae_vector* cw,
     /* Real    */ const ae_vector* s,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t rootidx,
     double r,
     /* Real    */ ae_matrix* xwr,
     ae_int_t* k,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t childle;
    ae_int_t childge;
    ae_int_t itemoffs;
    ae_int_t cwoffs;
    ae_int_t cwcnt;


    
    /*
     * Leaf node.
     */
    if( kdnodes->ptr.p_int[rootidx]>0 )
    {
        cwcnt = kdnodes->ptr.p_int[rootidx+0];
        cwoffs = kdnodes->ptr.p_int[rootidx+1];
        for(i=0; i<=cwcnt-1; i++)
        {
            itemoffs = cwoffs+i*(nx+ny);
            for(j=0; j<=nx+ny-1; j++)
            {
                xwr->ptr.pp_double[*k][j] = cw->ptr.p_double[itemoffs+j];
            }
            for(j=0; j<=nx-1; j++)
            {
                xwr->ptr.pp_double[*k][j] = xwr->ptr.pp_double[*k][j]*s->ptr.p_double[j];
            }
            for(j=0; j<=nx-1; j++)
            {
                xwr->ptr.pp_double[*k][nx+ny+j] = r*s->ptr.p_double[j];
            }
            *k = *k+1;
        }
        return;
    }
    
    /*
     * Simple split
     */
    if( kdnodes->ptr.p_int[rootidx]==0 )
    {
        
        /*
         * Load:
         * * ChildLE, ChildGE - indexes of childs
         */
        childle = kdnodes->ptr.p_int[rootidx+3];
        childge = kdnodes->ptr.p_int[rootidx+4];
        
        /*
         * Process both parts of split
         */
        rbfv2_partialunpackrec(kdnodes, kdsplits, cw, s, nx, ny, childle, r, xwr, k, _state);
        rbfv2_partialunpackrec(kdnodes, kdsplits, cw, s, nx, ny, childge, r, xwr, k, _state);
        return;
    }
    
    /*
     * Integrity failure
     */
    ae_assert(ae_false, "PartialUnpackRec: integrity check failed", _state);
}


/*************************************************************************
This function returns size of design matrix row for evaluation point X0,
given:
* query radius multiplier (either RBFV2NearRadius() or RBFV2FarRadius())
* hierarchy level: value in [0,NH) for single-level model, or negative
  value for multilevel model (all levels of hierarchy in single matrix,
  like one used by nonnegative RBF)

INPUT PARAMETERS:
    kdNodes, kdSplits, CW, Ri, kdRoots, kdBoxMin, kdBoxMax, NX, NY, NH - corresponding fields of V2 model
    Level   -   value in [0,NH) for single-level design matrix, negative
                value for multilevel design matrix
    RCoeff  -   radius coefficient, either RBFV2NearRadius() or RBFV2FarRadius()
    X0      -   query point
    CalcBuf -   buffer for PreparePartialQuery(), allocated by caller
    
RESULT:
    row size

  -- ALGLIB --
     Copyright 28.09.2016 by Bochkanov Sergey
*************************************************************************/
static ae_int_t rbfv2_designmatrixrowsize(/* Integer */ const ae_vector* kdnodes,
     /* Real    */ const ae_vector* kdsplits,
     /* Real    */ const ae_vector* cw,
     /* Real    */ const ae_vector* ri,
     /* Integer */ const ae_vector* kdroots,
     /* Real    */ const ae_vector* kdboxmin,
     /* Real    */ const ae_vector* kdboxmax,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t nh,
     ae_int_t level,
     double rcoeff,
     /* Real    */ ae_vector* x0,
     rbfv2calcbuffer* calcbuf,
     ae_state *_state)
{
    ae_int_t dummy;
    ae_int_t levelidx;
    ae_int_t level0;
    ae_int_t level1;
    double curradius2;
    ae_int_t result;


    ae_assert(nh>0, "DesignMatrixRowSize: integrity failure", _state);
    if( level>=0 )
    {
        level0 = level;
        level1 = level;
    }
    else
    {
        level0 = 0;
        level1 = nh-1;
    }
    result = 0;
    for(levelidx=level0; levelidx<=level1; levelidx++)
    {
        curradius2 = ae_sqr(ri->ptr.p_double[levelidx]*rcoeff, _state);
        rbfv2_preparepartialquery(x0, kdboxmin, kdboxmax, nx, calcbuf, &dummy, _state);
        result = result+rbfv2_partialcountrec(kdnodes, kdsplits, cw, nx, ny, calcbuf, kdroots->ptr.p_int[levelidx], curradius2, x0, _state);
    }
    return result;
}


/*************************************************************************
This function generates design matrix row for evaluation point X0, given:
* query radius multiplier (either RBFV2NearRadius() or RBFV2FarRadius())
* hierarchy level: value in [0,NH) for single-level model, or negative
  value for multilevel model (all levels of hierarchy in single matrix,
  like one used by nonnegative RBF)

INPUT PARAMETERS:
    kdNodes, kdSplits, CW, Ri, kdRoots, kdBoxMin, kdBoxMax, NX, NY, NH - corresponding fields of V2 model

    CWRange -   internal array[NH+1] used by RBF construction function,
                stores ranges of CW occupied by NH trees.
    Level   -   value in [0,NH) for single-level design matrix, negative
                value for multilevel design matrix
    BF      -   basis function type
    RCoeff  -   radius coefficient, either RBFV2NearRadius() or RBFV2FarRadius()
    RowsPerPoint-equal to:
                * 1 for unpenalized regression model
                * 1+NX for basic form of nonsmoothness penalty
    Penalty -   nonsmoothness penalty coefficient
    
    X0      -   query point
    
    CalcBuf -   buffer for PreparePartialQuery(), allocated by caller
    R2      -   preallocated temporary buffer, size is at least NPoints;
                it is caller's responsibility to make sure that R2 has enough space.
    Offs    -   preallocated temporary buffer; size is at least NPoints;
                it is caller's responsibility to make sure that Offs has enough space.
    K       -   MUST BE ZERO ON INITIAL CALL. This variable is incremented,
                not set. So, any no-zero value will result in the incorrect
                points count being returned.
    RowIdx  -   preallocated array, at least RowSize elements
    RowVal  -   preallocated array, at least RowSize*RowsPerPoint elements
    
RESULT:
    RowIdx  -   RowSize elements are filled with column indexes of non-zero
                design matrix entries
    RowVal  -   RowSize*RowsPerPoint elements are filled with design matrix
                values, with column RowIdx[0] being stored in first RowsPerPoint
                elements of RowVal, column RowIdx[1] being stored in next
                RowsPerPoint elements, and so on.
                
                First element in contiguous set of RowsPerPoint elements
                corresponds to 
                
    RowSize -   number of columns per row

  -- ALGLIB --
     Copyright 28.09.2016 by Bochkanov Sergey
*************************************************************************/
static void rbfv2_designmatrixgeneraterow(/* Integer */ const ae_vector* kdnodes,
     /* Real    */ const ae_vector* kdsplits,
     /* Real    */ const ae_vector* cw,
     /* Real    */ const ae_vector* ri,
     /* Integer */ const ae_vector* kdroots,
     /* Real    */ const ae_vector* kdboxmin,
     /* Real    */ const ae_vector* kdboxmax,
     /* Integer */ const ae_vector* cwrange,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t nh,
     ae_int_t level,
     ae_int_t bf,
     double rcoeff,
     ae_int_t rowsperpoint,
     double penalty,
     /* Real    */ ae_vector* x0,
     rbfv2calcbuffer* calcbuf,
     /* Real    */ ae_vector* tmpr2,
     /* Integer */ ae_vector* tmpoffs,
     /* Integer */ ae_vector* rowidx,
     /* Real    */ ae_vector* rowval,
     ae_int_t* rowsize,
     ae_state *_state)
{
    ae_int_t j;
    ae_int_t k;
    ae_int_t cnt;
    ae_int_t levelidx;
    ae_int_t level0;
    ae_int_t level1;
    double invri2;
    double curradius2;
    double val;
    double dval;
    double d2val;

    *rowsize = 0;

    ae_assert(nh>0, "DesignMatrixGenerateRow: integrity failure (a)", _state);
    ae_assert(rowsperpoint==1||rowsperpoint==1+nx, "DesignMatrixGenerateRow: integrity failure (b)", _state);
    if( level>=0 )
    {
        level0 = level;
        level1 = level;
    }
    else
    {
        level0 = 0;
        level1 = nh-1;
    }
    *rowsize = 0;
    for(levelidx=level0; levelidx<=level1; levelidx++)
    {
        curradius2 = ae_sqr(ri->ptr.p_double[levelidx]*rcoeff, _state);
        invri2 = (double)1/ae_sqr(ri->ptr.p_double[levelidx], _state);
        rbfv2_preparepartialquery(x0, kdboxmin, kdboxmax, nx, calcbuf, &cnt, _state);
        rbfv2_partialqueryrec(kdnodes, kdsplits, cw, nx, ny, calcbuf, kdroots->ptr.p_int[levelidx], curradius2, x0, tmpr2, tmpoffs, &cnt, _state);
        ae_assert(tmpr2->cnt>=cnt, "DesignMatrixRowSize: integrity failure (c)", _state);
        ae_assert(tmpoffs->cnt>=cnt, "DesignMatrixRowSize: integrity failure (d)", _state);
        ae_assert(rowidx->cnt>=*rowsize+cnt, "DesignMatrixRowSize: integrity failure (e)", _state);
        ae_assert(rowval->cnt>=rowsperpoint*(*rowsize+cnt), "DesignMatrixRowSize: integrity failure (f)", _state);
        for(j=0; j<=cnt-1; j++)
        {
            
            /*
             * Generate element corresponding to fitting error.
             * Store derivative information which may be required later.
             */
            ae_assert((tmpoffs->ptr.p_int[j]-cwrange->ptr.p_int[level0])%(nx+ny)==0, "DesignMatrixRowSize: integrity failure (g)", _state);
            rbfv2basisfuncdiff2(bf, tmpr2->ptr.p_double[j]*invri2, &val, &dval, &d2val, _state);
            rowidx->ptr.p_int[*rowsize+j] = (tmpoffs->ptr.p_int[j]-cwrange->ptr.p_int[level0])/(nx+ny);
            rowval->ptr.p_double[(*rowsize+j)*rowsperpoint+0] = val;
            if( rowsperpoint==1 )
            {
                continue;
            }
            
            /*
             * Generate elements corresponding to nonsmoothness penalty
             */
            ae_assert(rowsperpoint==1+nx, "DesignMatrixRowSize: integrity failure (h)", _state);
            for(k=0; k<=nx-1; k++)
            {
                rowval->ptr.p_double[(*rowsize+j)*rowsperpoint+1+k] = penalty*(dval*(double)2*invri2+d2val*ae_sqr((double)2*(x0->ptr.p_double[k]-cw->ptr.p_double[tmpoffs->ptr.p_int[j]+k])*invri2, _state));
            }
        }
        
        /*
         * Update columns counter
         */
        *rowsize = *rowsize+cnt;
    }
}


/*************************************************************************
This function fills RBF model by zeros.

  -- ALGLIB --
     Copyright 17.11.2018 by Bochkanov Sergey
*************************************************************************/
static void rbfv2_zerofill(rbfv2model* s,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t bf,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;


    s->bf = bf;
    s->nh = 0;
    ae_vector_set_length(&s->ri, 0, _state);
    ae_vector_set_length(&s->s, 0, _state);
    ae_vector_set_length(&s->kdroots, 0, _state);
    ae_vector_set_length(&s->kdnodes, 0, _state);
    ae_vector_set_length(&s->kdsplits, 0, _state);
    ae_vector_set_length(&s->kdboxmin, 0, _state);
    ae_vector_set_length(&s->kdboxmax, 0, _state);
    ae_vector_set_length(&s->cw, 0, _state);
    ae_matrix_set_length(&s->v, ny, nx+1, _state);
    for(i=0; i<=ny-1; i++)
    {
        for(j=0; j<=nx; j++)
        {
            s->v.ptr.pp_double[i][j] = (double)(0);
        }
    }
}


void _rbfv2calcbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    rbfv2calcbuffer *p = (rbfv2calcbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->curboxmin, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->curboxmax, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x123, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->y123, 0, DT_REAL, _state, make_automatic);
}


void _rbfv2calcbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    rbfv2calcbuffer       *dst = (rbfv2calcbuffer*)_dst;
    const rbfv2calcbuffer *src = (const rbfv2calcbuffer*)_src;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->curboxmin, &src->curboxmin, _state, make_automatic);
    ae_vector_init_copy(&dst->curboxmax, &src->curboxmax, _state, make_automatic);
    dst->curdist2 = src->curdist2;
    ae_vector_init_copy(&dst->x123, &src->x123, _state, make_automatic);
    ae_vector_init_copy(&dst->y123, &src->y123, _state, make_automatic);
}


void _rbfv2calcbuffer_clear(void* _p)
{
    rbfv2calcbuffer *p = (rbfv2calcbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->curboxmin);
    ae_vector_clear(&p->curboxmax);
    ae_vector_clear(&p->x123);
    ae_vector_clear(&p->y123);
}


void _rbfv2calcbuffer_destroy(void* _p)
{
    rbfv2calcbuffer *p = (rbfv2calcbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->curboxmin);
    ae_vector_destroy(&p->curboxmax);
    ae_vector_destroy(&p->x123);
    ae_vector_destroy(&p->y123);
}


void _rbfv2model_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    rbfv2model *p = (rbfv2model*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->ri, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->kdroots, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->kdnodes, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->kdsplits, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->kdboxmin, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->kdboxmax, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->cw, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->v, 0, 0, DT_REAL, _state, make_automatic);
    _rbfv2calcbuffer_init(&p->calcbuf, _state, make_automatic);
}


void _rbfv2model_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    rbfv2model       *dst = (rbfv2model*)_dst;
    const rbfv2model *src = (const rbfv2model*)_src;
    dst->ny = src->ny;
    dst->nx = src->nx;
    dst->bf = src->bf;
    dst->nh = src->nh;
    ae_vector_init_copy(&dst->ri, &src->ri, _state, make_automatic);
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_vector_init_copy(&dst->kdroots, &src->kdroots, _state, make_automatic);
    ae_vector_init_copy(&dst->kdnodes, &src->kdnodes, _state, make_automatic);
    ae_vector_init_copy(&dst->kdsplits, &src->kdsplits, _state, make_automatic);
    ae_vector_init_copy(&dst->kdboxmin, &src->kdboxmin, _state, make_automatic);
    ae_vector_init_copy(&dst->kdboxmax, &src->kdboxmax, _state, make_automatic);
    ae_vector_init_copy(&dst->cw, &src->cw, _state, make_automatic);
    ae_matrix_init_copy(&dst->v, &src->v, _state, make_automatic);
    dst->lambdareg = src->lambdareg;
    dst->maxits = src->maxits;
    dst->supportr = src->supportr;
    dst->basisfunction = src->basisfunction;
    _rbfv2calcbuffer_init_copy(&dst->calcbuf, &src->calcbuf, _state, make_automatic);
}


void _rbfv2model_clear(void* _p)
{
    rbfv2model *p = (rbfv2model*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->ri);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->kdroots);
    ae_vector_clear(&p->kdnodes);
    ae_vector_clear(&p->kdsplits);
    ae_vector_clear(&p->kdboxmin);
    ae_vector_clear(&p->kdboxmax);
    ae_vector_clear(&p->cw);
    ae_matrix_clear(&p->v);
    _rbfv2calcbuffer_clear(&p->calcbuf);
}


void _rbfv2model_destroy(void* _p)
{
    rbfv2model *p = (rbfv2model*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->ri);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->kdroots);
    ae_vector_destroy(&p->kdnodes);
    ae_vector_destroy(&p->kdsplits);
    ae_vector_destroy(&p->kdboxmin);
    ae_vector_destroy(&p->kdboxmax);
    ae_vector_destroy(&p->cw);
    ae_matrix_destroy(&p->v);
    _rbfv2calcbuffer_destroy(&p->calcbuf);
}


void _rbfv2gridcalcbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    rbfv2gridcalcbuffer *p = (rbfv2gridcalcbuffer*)_p;
    ae_touch_ptr((void*)p);
    _rbfv2calcbuffer_init(&p->calcbuf, _state, make_automatic);
    ae_vector_init(&p->cx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->ry, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->ty, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rf, 0, DT_BOOL, _state, make_automatic);
}


void _rbfv2gridcalcbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    rbfv2gridcalcbuffer       *dst = (rbfv2gridcalcbuffer*)_dst;
    const rbfv2gridcalcbuffer *src = (const rbfv2gridcalcbuffer*)_src;
    _rbfv2calcbuffer_init_copy(&dst->calcbuf, &src->calcbuf, _state, make_automatic);
    ae_vector_init_copy(&dst->cx, &src->cx, _state, make_automatic);
    ae_vector_init_copy(&dst->rx, &src->rx, _state, make_automatic);
    ae_vector_init_copy(&dst->ry, &src->ry, _state, make_automatic);
    ae_vector_init_copy(&dst->tx, &src->tx, _state, make_automatic);
    ae_vector_init_copy(&dst->ty, &src->ty, _state, make_automatic);
    ae_vector_init_copy(&dst->rf, &src->rf, _state, make_automatic);
}


void _rbfv2gridcalcbuffer_clear(void* _p)
{
    rbfv2gridcalcbuffer *p = (rbfv2gridcalcbuffer*)_p;
    ae_touch_ptr((void*)p);
    _rbfv2calcbuffer_clear(&p->calcbuf);
    ae_vector_clear(&p->cx);
    ae_vector_clear(&p->rx);
    ae_vector_clear(&p->ry);
    ae_vector_clear(&p->tx);
    ae_vector_clear(&p->ty);
    ae_vector_clear(&p->rf);
}


void _rbfv2gridcalcbuffer_destroy(void* _p)
{
    rbfv2gridcalcbuffer *p = (rbfv2gridcalcbuffer*)_p;
    ae_touch_ptr((void*)p);
    _rbfv2calcbuffer_destroy(&p->calcbuf);
    ae_vector_destroy(&p->cx);
    ae_vector_destroy(&p->rx);
    ae_vector_destroy(&p->ry);
    ae_vector_destroy(&p->tx);
    ae_vector_destroy(&p->ty);
    ae_vector_destroy(&p->rf);
}


void _rbfv2report_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    rbfv2report *p = (rbfv2report*)_p;
    ae_touch_ptr((void*)p);
}


void _rbfv2report_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    rbfv2report       *dst = (rbfv2report*)_dst;
    const rbfv2report *src = (const rbfv2report*)_src;
    dst->terminationtype = src->terminationtype;
    dst->maxerror = src->maxerror;
    dst->rmserror = src->rmserror;
}


void _rbfv2report_clear(void* _p)
{
    rbfv2report *p = (rbfv2report*)_p;
    ae_touch_ptr((void*)p);
}


void _rbfv2report_destroy(void* _p)
{
    rbfv2report *p = (rbfv2report*)_p;
    ae_touch_ptr((void*)p);
}


/*$ End $*/
