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
#include "rbfv1.h"


/*$ Declarations $*/
static ae_int_t rbfv1_mxnx = 3;
static double rbfv1_rbffarradius = 6;
static double rbfv1_rbfnearradius = 2.1;
static double rbfv1_rbfmlradius = 3;
static double rbfv1_minbasecasecost = 100000;
static ae_bool rbfv1_rbfv1buildlinearmodel(/* Real    */ const ae_matrix* x,
     /* Real    */ ae_matrix* y,
     ae_int_t n,
     ae_int_t ny,
     ae_int_t modeltype,
     /* Real    */ ae_matrix* v,
     ae_state *_state);
static void rbfv1_buildrbfmodellsqr(/* Real    */ const ae_matrix* x,
     /* Real    */ ae_matrix* y,
     /* Real    */ const ae_matrix* xc,
     /* Real    */ const ae_vector* r,
     ae_int_t n,
     ae_int_t nc,
     ae_int_t ny,
     kdtree* pointstree,
     kdtree* centerstree,
     double epsort,
     double epserr,
     ae_int_t maxits,
     ae_int_t* gnnz,
     ae_int_t* snnz,
     /* Real    */ ae_matrix* w,
     ae_int_t* info,
     ae_int_t* iterationscount,
     ae_int_t* nmv,
     ae_state *_state);
static void rbfv1_buildrbfmlayersmodellsqr(/* Real    */ const ae_matrix* x,
     /* Real    */ ae_matrix* y,
     /* Real    */ ae_matrix* xc,
     double rval,
     /* Real    */ ae_vector* r,
     ae_int_t n,
     ae_int_t* nc,
     ae_int_t ny,
     ae_int_t nlayers,
     kdtree* centerstree,
     double epsort,
     double epserr,
     ae_int_t maxits,
     double lambdav,
     ae_int_t* annz,
     /* Real    */ ae_matrix* w,
     ae_int_t* info,
     ae_int_t* iterationscount,
     ae_int_t* nmv,
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
void rbfv1create(ae_int_t nx,
     ae_int_t ny,
     rbfv1model* s,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;

    _rbfv1model_clear(s);

    ae_assert(nx==2||nx==3, "RBFCreate: NX<>2 and NX<>3", _state);
    ae_assert(ny>=1, "RBFCreate: NY<1", _state);
    s->nx = nx;
    s->ny = ny;
    s->nl = 0;
    s->nc = 0;
    ae_matrix_set_length(&s->v, ny, rbfv1_mxnx+1, _state);
    for(i=0; i<=ny-1; i++)
    {
        for(j=0; j<=rbfv1_mxnx; j++)
        {
            s->v.ptr.pp_double[i][j] = (double)(0);
        }
    }
    s->rmax = (double)(0);
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
void rbfv1createcalcbuffer(const rbfv1model* s,
     rbfv1calcbuffer* buf,
     ae_state *_state)
{

    _rbfv1calcbuffer_clear(buf);

    kdtreecreaterequestbuffer(&s->tree, &buf->requestbuffer, _state);
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
     ae_state *_state)
{
    ae_frame _frame_block;
    kdtree tree;
    kdtree ctree;
    ae_vector dist;
    ae_vector xcx;
    ae_matrix a;
    ae_matrix v;
    ae_matrix omega;
    ae_matrix residualy;
    ae_vector radius;
    ae_matrix xc;
    ae_int_t nc;
    double rmax;
    ae_vector tags;
    ae_vector ctags;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t snnz;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_int_t layerscnt;
    ae_bool modelstatus;

    ae_frame_make(_state, &_frame_block);
    memset(&tree, 0, sizeof(tree));
    memset(&ctree, 0, sizeof(ctree));
    memset(&dist, 0, sizeof(dist));
    memset(&xcx, 0, sizeof(xcx));
    memset(&a, 0, sizeof(a));
    memset(&v, 0, sizeof(v));
    memset(&omega, 0, sizeof(omega));
    memset(&residualy, 0, sizeof(residualy));
    memset(&radius, 0, sizeof(radius));
    memset(&xc, 0, sizeof(xc));
    memset(&tags, 0, sizeof(tags));
    memset(&ctags, 0, sizeof(ctags));
    memset(&tmp0, 0, sizeof(tmp0));
    memset(&tmp1, 0, sizeof(tmp1));
    _rbfv1report_clear(rep);
    _kdtree_init(&tree, _state, ae_true);
    _kdtree_init(&ctree, _state, ae_true);
    ae_vector_init(&dist, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xcx, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&a, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&v, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&omega, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&residualy, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&radius, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xc, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tags, 0, DT_INT, _state, ae_true);
    ae_vector_init(&ctags, 0, DT_INT, _state, ae_true);
    ae_vector_init(&tmp0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp1, 0, DT_REAL, _state, ae_true);

    ae_assert(s->nx==2||s->nx==3, "RBFBuildModel: S.NX<>2 or S.NX<>3!", _state);
    
    /*
     * Quick exit when we have no points
     */
    if( n==0 )
    {
        rep->terminationtype = 1;
        rep->iterationscount = 0;
        rep->nmv = 0;
        rep->arows = 0;
        rep->acols = 0;
        kdtreebuildtagged(&s->xc, &tags, 0, rbfv1_mxnx, 0, 2, &s->tree, _state);
        ae_matrix_set_length(&s->xc, 0, 0, _state);
        ae_matrix_set_length(&s->wr, 0, 0, _state);
        s->nc = 0;
        s->rmax = (double)(0);
        ae_matrix_set_length(&s->v, s->ny, rbfv1_mxnx+1, _state);
        for(i=0; i<=s->ny-1; i++)
        {
            for(j=0; j<=rbfv1_mxnx; j++)
            {
                s->v.ptr.pp_double[i][j] = (double)(0);
            }
        }
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * General case, N>0
     */
    rep->annz = 0;
    rep->iterationscount = 0;
    rep->nmv = 0;
    ae_vector_set_length(&xcx, rbfv1_mxnx, _state);
    
    /*
     * First model in a sequence - linear model.
     * Residuals from linear regression are stored in the ResidualY variable
     * (used later to build RBF models).
     */
    ae_matrix_set_length(&residualy, n, s->ny, _state);
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=s->ny-1; j++)
        {
            residualy.ptr.pp_double[i][j] = y->ptr.pp_double[i][j];
        }
    }
    if( !rbfv1_rbfv1buildlinearmodel(x, &residualy, n, s->ny, aterm, &v, _state) )
    {
        rep->terminationtype = -5;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Handle special case: multilayer model with NLayers=0.
     * Quick exit.
     */
    if( algorithmtype==2&&nlayers==0 )
    {
        rep->terminationtype = 1;
        rep->iterationscount = 0;
        rep->nmv = 0;
        rep->arows = 0;
        rep->acols = 0;
        kdtreebuildtagged(&s->xc, &tags, 0, rbfv1_mxnx, 0, 2, &s->tree, _state);
        ae_matrix_set_length(&s->xc, 0, 0, _state);
        ae_matrix_set_length(&s->wr, 0, 0, _state);
        s->nc = 0;
        s->rmax = (double)(0);
        ae_matrix_set_length(&s->v, s->ny, rbfv1_mxnx+1, _state);
        for(i=0; i<=s->ny-1; i++)
        {
            for(j=0; j<=rbfv1_mxnx; j++)
            {
                s->v.ptr.pp_double[i][j] = v.ptr.pp_double[i][j];
            }
        }
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Second model in a sequence - RBF term.
     *
     * NOTE: assignments below are not necessary, but without them
     *       MSVC complains about unitialized variables.
     */
    nc = 0;
    rmax = (double)(0);
    layerscnt = 0;
    modelstatus = ae_false;
    if( algorithmtype==1 )
    {
        
        /*
         * Add RBF model.
         * This model uses local KD-trees to speed-up nearest neighbor searches.
         */
        nc = n;
        ae_matrix_set_length(&xc, nc, rbfv1_mxnx, _state);
        for(i=0; i<=nc-1; i++)
        {
            for(j=0; j<=rbfv1_mxnx-1; j++)
            {
                xc.ptr.pp_double[i][j] = x->ptr.pp_double[i][j];
            }
        }
        rmax = (double)(0);
        ae_vector_set_length(&radius, nc, _state);
        ae_vector_set_length(&ctags, nc, _state);
        for(i=0; i<=nc-1; i++)
        {
            ctags.ptr.p_int[i] = i;
        }
        kdtreebuildtagged(&xc, &ctags, nc, rbfv1_mxnx, 0, 2, &ctree, _state);
        if( nc==0 )
        {
            rmax = (double)(1);
        }
        else
        {
            if( nc==1 )
            {
                radius.ptr.p_double[0] = radvalue;
                rmax = radius.ptr.p_double[0];
            }
            else
            {
                
                /*
                 * NC>1, calculate radii using distances to nearest neigbors
                 */
                for(i=0; i<=nc-1; i++)
                {
                    for(j=0; j<=rbfv1_mxnx-1; j++)
                    {
                        xcx.ptr.p_double[j] = xc.ptr.pp_double[i][j];
                    }
                    if( kdtreequeryknn(&ctree, &xcx, 1, ae_false, _state)>0 )
                    {
                        kdtreequeryresultsdistances(&ctree, &dist, _state);
                        radius.ptr.p_double[i] = radvalue*dist.ptr.p_double[0];
                    }
                    else
                    {
                        
                        /*
                         * No neighbors found (it will happen when we have only one center).
                         * Initialize radius with default value.
                         */
                        radius.ptr.p_double[i] = 1.0;
                    }
                }
                
                /*
                 * Apply filtering
                 */
                rvectorsetlengthatleast(&tmp0, nc, _state);
                for(i=0; i<=nc-1; i++)
                {
                    tmp0.ptr.p_double[i] = radius.ptr.p_double[i];
                }
                tagsortfast(&tmp0, &tmp1, nc, _state);
                for(i=0; i<=nc-1; i++)
                {
                    radius.ptr.p_double[i] = ae_minreal(radius.ptr.p_double[i], radzvalue*tmp0.ptr.p_double[nc/2], _state);
                }
                
                /*
                 * Calculate RMax, check that all radii are non-zero
                 */
                for(i=0; i<=nc-1; i++)
                {
                    rmax = ae_maxreal(rmax, radius.ptr.p_double[i], _state);
                }
                for(i=0; i<=nc-1; i++)
                {
                    if( ae_fp_eq(radius.ptr.p_double[i],(double)(0)) )
                    {
                        rep->terminationtype = -5;
                        ae_frame_leave(_state);
                        return;
                    }
                }
            }
        }
        ivectorsetlengthatleast(&tags, n, _state);
        for(i=0; i<=n-1; i++)
        {
            tags.ptr.p_int[i] = i;
        }
        kdtreebuildtagged(x, &tags, n, rbfv1_mxnx, 0, 2, &tree, _state);
        rbfv1_buildrbfmodellsqr(x, &residualy, &xc, &radius, n, nc, s->ny, &tree, &ctree, epsort, epserr, maxits, &rep->annz, &snnz, &omega, &rep->terminationtype, &rep->iterationscount, &rep->nmv, _state);
        layerscnt = 1;
        modelstatus = ae_true;
    }
    if( algorithmtype==2 )
    {
        rmax = radvalue;
        rbfv1_buildrbfmlayersmodellsqr(x, &residualy, &xc, radvalue, &radius, n, &nc, s->ny, nlayers, &ctree, 1.0E-6, 1.0E-6, 50, lambdav, &rep->annz, &omega, &rep->terminationtype, &rep->iterationscount, &rep->nmv, _state);
        layerscnt = nlayers;
        modelstatus = ae_true;
    }
    ae_assert(modelstatus, "RBFBuildModel: integrity error", _state);
    if( rep->terminationtype<=0 )
    {
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Model is built
     */
    s->nc = nc/layerscnt;
    s->rmax = rmax;
    s->nl = layerscnt;
    ae_matrix_set_length(&s->xc, s->nc, rbfv1_mxnx, _state);
    ae_matrix_set_length(&s->wr, s->nc, 1+s->nl*s->ny, _state);
    ae_matrix_set_length(&s->v, s->ny, rbfv1_mxnx+1, _state);
    for(i=0; i<=s->nc-1; i++)
    {
        for(j=0; j<=rbfv1_mxnx-1; j++)
        {
            s->xc.ptr.pp_double[i][j] = xc.ptr.pp_double[i][j];
        }
    }
    ivectorsetlengthatleast(&tags, s->nc, _state);
    for(i=0; i<=s->nc-1; i++)
    {
        tags.ptr.p_int[i] = i;
    }
    kdtreebuildtagged(&s->xc, &tags, s->nc, rbfv1_mxnx, 0, 2, &s->tree, _state);
    for(i=0; i<=s->nc-1; i++)
    {
        s->wr.ptr.pp_double[i][0] = radius.ptr.p_double[i];
        for(k=0; k<=layerscnt-1; k++)
        {
            for(j=0; j<=s->ny-1; j++)
            {
                s->wr.ptr.pp_double[i][1+k*s->ny+j] = omega.ptr.pp_double[k*s->nc+i][j];
            }
        }
    }
    for(i=0; i<=s->ny-1; i++)
    {
        for(j=0; j<=rbfv1_mxnx; j++)
        {
            s->v.ptr.pp_double[i][j] = v.ptr.pp_double[i][j];
        }
    }
    rep->terminationtype = 1;
    rep->arows = n;
    rep->acols = s->nc;
    ae_frame_leave(_state);
}


/*************************************************************************
Serializer: allocation

  -- ALGLIB --
     Copyright 02.02.2012 by Bochkanov Sergey
*************************************************************************/
void rbfv1alloc(ae_serializer* s,
     const rbfv1model* model,
     ae_state *_state)
{


    
    /*
     * Data
     */
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    kdtreealloc(s, &model->tree, _state);
    allocrealmatrix(s, &model->xc, -1, -1, _state);
    allocrealmatrix(s, &model->wr, -1, -1, _state);
    ae_serializer_alloc_entry(s);
    allocrealmatrix(s, &model->v, -1, -1, _state);
}


/*************************************************************************
Serializer: serialization

  -- ALGLIB --
     Copyright 02.02.2012 by Bochkanov Sergey
*************************************************************************/
void rbfv1serialize(ae_serializer* s,
     const rbfv1model* model,
     ae_state *_state)
{


    
    /*
     * Data
     */
    ae_serializer_serialize_int(s, model->nx, _state);
    ae_serializer_serialize_int(s, model->ny, _state);
    ae_serializer_serialize_int(s, model->nc, _state);
    ae_serializer_serialize_int(s, model->nl, _state);
    kdtreeserialize(s, &model->tree, _state);
    serializerealmatrix(s, &model->xc, -1, -1, _state);
    serializerealmatrix(s, &model->wr, -1, -1, _state);
    ae_serializer_serialize_double(s, model->rmax, _state);
    serializerealmatrix(s, &model->v, -1, -1, _state);
}


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 02.02.2012 by Bochkanov Sergey
*************************************************************************/
void rbfv1unserialize(ae_serializer* s,
     rbfv1model* model,
     ae_state *_state)
{
    ae_int_t nx;
    ae_int_t ny;

    _rbfv1model_clear(model);

    
    /*
     * Unserialize primary model parameters, initialize model.
     *
     * It is necessary to call RBFCreate() because some internal fields
     * which are NOT unserialized will need initialization.
     */
    ae_serializer_unserialize_int(s, &nx, _state);
    ae_serializer_unserialize_int(s, &ny, _state);
    rbfv1create(nx, ny, model, _state);
    ae_serializer_unserialize_int(s, &model->nc, _state);
    ae_serializer_unserialize_int(s, &model->nl, _state);
    kdtreeunserialize(s, &model->tree, _state);
    unserializerealmatrix(s, &model->xc, _state);
    unserializerealmatrix(s, &model->wr, _state);
    ae_serializer_unserialize_double(s, &model->rmax, _state);
    unserializerealmatrix(s, &model->v, _state);
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
double rbfv1calc2(rbfv1model* s, double x0, double x1, ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t lx;
    ae_int_t tg;
    double d2;
    double t;
    double bfcur;
    double rcur;
    double result;


    ae_assert(ae_isfinite(x0, _state), "RBFCalc2: invalid value for X0 (X0 is Inf)!", _state);
    ae_assert(ae_isfinite(x1, _state), "RBFCalc2: invalid value for X1 (X1 is Inf)!", _state);
    if( s->ny!=1||s->nx!=2 )
    {
        result = (double)(0);
        return result;
    }
    result = s->v.ptr.pp_double[0][0]*x0+s->v.ptr.pp_double[0][1]*x1+s->v.ptr.pp_double[0][rbfv1_mxnx];
    if( s->nc==0 )
    {
        return result;
    }
    rvectorsetlengthatleast(&s->calcbufxcx, rbfv1_mxnx, _state);
    for(i=0; i<=rbfv1_mxnx-1; i++)
    {
        s->calcbufxcx.ptr.p_double[i] = 0.0;
    }
    s->calcbufxcx.ptr.p_double[0] = x0;
    s->calcbufxcx.ptr.p_double[1] = x1;
    lx = kdtreequeryrnn(&s->tree, &s->calcbufxcx, s->rmax*rbfv1_rbffarradius, ae_true, _state);
    kdtreequeryresultsx(&s->tree, &s->calcbufx, _state);
    kdtreequeryresultstags(&s->tree, &s->calcbuftags, _state);
    for(i=0; i<=lx-1; i++)
    {
        tg = s->calcbuftags.ptr.p_int[i];
        d2 = ae_sqr(x0-s->calcbufx.ptr.pp_double[i][0], _state)+ae_sqr(x1-s->calcbufx.ptr.pp_double[i][1], _state);
        rcur = s->wr.ptr.pp_double[tg][0];
        bfcur = ae_exp(-d2/(rcur*rcur), _state);
        for(j=0; j<=s->nl-1; j++)
        {
            result = result+bfcur*s->wr.ptr.pp_double[tg][1+j];
            rcur = 0.5*rcur;
            t = bfcur*bfcur;
            bfcur = t*t;
        }
    }
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
double rbfv1calc3(rbfv1model* s,
     double x0,
     double x1,
     double x2,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t lx;
    ae_int_t tg;
    double t;
    double rcur;
    double bf;
    double result;


    ae_assert(ae_isfinite(x0, _state), "RBFCalc3: invalid value for X0 (X0 is Inf or NaN)!", _state);
    ae_assert(ae_isfinite(x1, _state), "RBFCalc3: invalid value for X1 (X1 is Inf or NaN)!", _state);
    ae_assert(ae_isfinite(x2, _state), "RBFCalc3: invalid value for X2 (X2 is Inf or NaN)!", _state);
    if( s->ny!=1||s->nx!=3 )
    {
        result = (double)(0);
        return result;
    }
    result = s->v.ptr.pp_double[0][0]*x0+s->v.ptr.pp_double[0][1]*x1+s->v.ptr.pp_double[0][2]*x2+s->v.ptr.pp_double[0][rbfv1_mxnx];
    if( s->nc==0 )
    {
        return result;
    }
    
    /*
     * calculating value for F(X)
     */
    rvectorsetlengthatleast(&s->calcbufxcx, rbfv1_mxnx, _state);
    for(i=0; i<=rbfv1_mxnx-1; i++)
    {
        s->calcbufxcx.ptr.p_double[i] = 0.0;
    }
    s->calcbufxcx.ptr.p_double[0] = x0;
    s->calcbufxcx.ptr.p_double[1] = x1;
    s->calcbufxcx.ptr.p_double[2] = x2;
    lx = kdtreequeryrnn(&s->tree, &s->calcbufxcx, s->rmax*rbfv1_rbffarradius, ae_true, _state);
    kdtreequeryresultsx(&s->tree, &s->calcbufx, _state);
    kdtreequeryresultstags(&s->tree, &s->calcbuftags, _state);
    for(i=0; i<=lx-1; i++)
    {
        tg = s->calcbuftags.ptr.p_int[i];
        rcur = s->wr.ptr.pp_double[tg][0];
        bf = ae_exp(-(ae_sqr(x0-s->calcbufx.ptr.pp_double[i][0], _state)+ae_sqr(x1-s->calcbufx.ptr.pp_double[i][1], _state)+ae_sqr(x2-s->calcbufx.ptr.pp_double[i][2], _state))/ae_sqr(rcur, _state), _state);
        for(j=0; j<=s->nl-1; j++)
        {
            result = result+bf*s->wr.ptr.pp_double[tg][1+j];
            t = bf*bf;
            bf = t*t;
        }
    }
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
void rbfv1calcbuf(rbfv1model* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t lx;
    ae_int_t tg;
    double t;
    double rcur;
    double bf;


    ae_assert(x->cnt>=s->nx, "RBFCalcBuf: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFCalcBuf: X contains infinite or NaN values", _state);
    if( y->cnt<s->ny )
    {
        ae_vector_set_length(y, s->ny, _state);
    }
    for(i=0; i<=s->ny-1; i++)
    {
        y->ptr.p_double[i] = s->v.ptr.pp_double[i][rbfv1_mxnx];
        for(j=0; j<=s->nx-1; j++)
        {
            y->ptr.p_double[i] = y->ptr.p_double[i]+s->v.ptr.pp_double[i][j]*x->ptr.p_double[j];
        }
    }
    if( s->nc==0 )
    {
        return;
    }
    rvectorsetlengthatleast(&s->calcbufxcx, rbfv1_mxnx, _state);
    for(i=0; i<=rbfv1_mxnx-1; i++)
    {
        s->calcbufxcx.ptr.p_double[i] = 0.0;
    }
    for(i=0; i<=s->nx-1; i++)
    {
        s->calcbufxcx.ptr.p_double[i] = x->ptr.p_double[i];
    }
    lx = kdtreequeryrnn(&s->tree, &s->calcbufxcx, s->rmax*rbfv1_rbffarradius, ae_true, _state);
    kdtreequeryresultsx(&s->tree, &s->calcbufx, _state);
    kdtreequeryresultstags(&s->tree, &s->calcbuftags, _state);
    for(i=0; i<=s->ny-1; i++)
    {
        for(j=0; j<=lx-1; j++)
        {
            tg = s->calcbuftags.ptr.p_int[j];
            rcur = s->wr.ptr.pp_double[tg][0];
            bf = ae_exp(-(ae_sqr(s->calcbufxcx.ptr.p_double[0]-s->calcbufx.ptr.pp_double[j][0], _state)+ae_sqr(s->calcbufxcx.ptr.p_double[1]-s->calcbufx.ptr.pp_double[j][1], _state)+ae_sqr(s->calcbufxcx.ptr.p_double[2]-s->calcbufx.ptr.pp_double[j][2], _state))/ae_sqr(rcur, _state), _state);
            for(k=0; k<=s->nl-1; k++)
            {
                y->ptr.p_double[i] = y->ptr.p_double[i]+bf*s->wr.ptr.pp_double[tg][1+k*s->ny+i];
                t = bf*bf;
                bf = t*t;
            }
        }
    }
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
void rbfv1tscalcbuf(const rbfv1model* s,
     rbfv1calcbuffer* buf,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t lx;
    ae_int_t tg;
    double t;
    double rcur;
    double bf;


    ae_assert(x->cnt>=s->nx, "RBFCalcBuf: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFCalcBuf: X contains infinite or NaN values", _state);
    if( y->cnt<s->ny )
    {
        ae_vector_set_length(y, s->ny, _state);
    }
    for(i=0; i<=s->ny-1; i++)
    {
        y->ptr.p_double[i] = s->v.ptr.pp_double[i][rbfv1_mxnx];
        for(j=0; j<=s->nx-1; j++)
        {
            y->ptr.p_double[i] = y->ptr.p_double[i]+s->v.ptr.pp_double[i][j]*x->ptr.p_double[j];
        }
    }
    if( s->nc==0 )
    {
        return;
    }
    rvectorsetlengthatleast(&buf->calcbufxcx, rbfv1_mxnx, _state);
    for(i=0; i<=rbfv1_mxnx-1; i++)
    {
        buf->calcbufxcx.ptr.p_double[i] = 0.0;
    }
    for(i=0; i<=s->nx-1; i++)
    {
        buf->calcbufxcx.ptr.p_double[i] = x->ptr.p_double[i];
    }
    lx = kdtreetsqueryrnn(&s->tree, &buf->requestbuffer, &buf->calcbufxcx, s->rmax*rbfv1_rbffarradius, ae_true, _state);
    kdtreetsqueryresultsx(&s->tree, &buf->requestbuffer, &buf->calcbufx, _state);
    kdtreetsqueryresultstags(&s->tree, &buf->requestbuffer, &buf->calcbuftags, _state);
    for(i=0; i<=s->ny-1; i++)
    {
        for(j=0; j<=lx-1; j++)
        {
            tg = buf->calcbuftags.ptr.p_int[j];
            rcur = s->wr.ptr.pp_double[tg][0];
            bf = ae_exp(-(ae_sqr(buf->calcbufxcx.ptr.p_double[0]-buf->calcbufx.ptr.pp_double[j][0], _state)+ae_sqr(buf->calcbufxcx.ptr.p_double[1]-buf->calcbufx.ptr.pp_double[j][1], _state)+ae_sqr(buf->calcbufxcx.ptr.p_double[2]-buf->calcbufx.ptr.pp_double[j][2], _state))/ae_sqr(rcur, _state), _state);
            for(k=0; k<=s->nl-1; k++)
            {
                y->ptr.p_double[i] = y->ptr.p_double[i]+bf*s->wr.ptr.pp_double[tg][1+k*s->ny+i];
                t = bf*bf;
                bf = t*t;
            }
        }
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
void rbfv1tsdiffbuf(const rbfv1model* s,
     rbfv1calcbuffer* buf,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     /* Real    */ ae_vector* dy,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t kk;
    ae_int_t lx;
    ae_int_t tg;
    double t;
    double rcur;
    double invrcur2;
    double f;
    double df;
    double w;


    ae_assert(x->cnt>=s->nx, "RBFDiffBuf: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFDiffBuf: X contains infinite or NaN values", _state);
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
        y->ptr.p_double[i] = s->v.ptr.pp_double[i][rbfv1_mxnx];
        for(j=0; j<=s->nx-1; j++)
        {
            y->ptr.p_double[i] = y->ptr.p_double[i]+s->v.ptr.pp_double[i][j]*x->ptr.p_double[j];
            dy->ptr.p_double[i*s->nx+j] = s->v.ptr.pp_double[i][j];
        }
    }
    if( s->nc==0 )
    {
        return;
    }
    rvectorsetlengthatleast(&buf->calcbufxcx, rbfv1_mxnx, _state);
    for(i=0; i<=rbfv1_mxnx-1; i++)
    {
        buf->calcbufxcx.ptr.p_double[i] = 0.0;
    }
    for(i=0; i<=s->nx-1; i++)
    {
        buf->calcbufxcx.ptr.p_double[i] = x->ptr.p_double[i];
    }
    lx = kdtreetsqueryrnn(&s->tree, &buf->requestbuffer, &buf->calcbufxcx, s->rmax*rbfv1_rbffarradius, ae_true, _state);
    kdtreetsqueryresultsx(&s->tree, &buf->requestbuffer, &buf->calcbufx, _state);
    kdtreetsqueryresultstags(&s->tree, &buf->requestbuffer, &buf->calcbuftags, _state);
    for(i=0; i<=s->ny-1; i++)
    {
        for(j=0; j<=lx-1; j++)
        {
            tg = buf->calcbuftags.ptr.p_int[j];
            rcur = s->wr.ptr.pp_double[tg][0];
            invrcur2 = (double)1/(rcur*rcur);
            f = ae_exp(-(ae_sqr(buf->calcbufxcx.ptr.p_double[0]-buf->calcbufx.ptr.pp_double[j][0], _state)+ae_sqr(buf->calcbufxcx.ptr.p_double[1]-buf->calcbufx.ptr.pp_double[j][1], _state)+ae_sqr(buf->calcbufxcx.ptr.p_double[2]-buf->calcbufx.ptr.pp_double[j][2], _state))*invrcur2, _state);
            df = -f;
            for(k=0; k<=s->nl-1; k++)
            {
                w = s->wr.ptr.pp_double[tg][1+k*s->ny+i];
                y->ptr.p_double[i] = y->ptr.p_double[i]+f*w;
                for(kk=0; kk<=s->nx-1; kk++)
                {
                    dy->ptr.p_double[i*s->nx+kk] = dy->ptr.p_double[i*s->nx+kk]+w*df*invrcur2*(double)2*(buf->calcbufxcx.ptr.p_double[kk]-buf->calcbufx.ptr.pp_double[j][kk]);
                }
                t = f*f;
                f = t*t;
                df = -f;
                invrcur2 = (double)4*invrcur2;
            }
        }
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t i0;
    ae_int_t i1;
    ae_int_t lx;
    ae_int_t tg;
    double t;
    double rcur;
    double invrcur2;
    double f;
    double df;
    double d2f;
    double w;


    ae_assert(x->cnt>=s->nx, "RBFDiffBuf: Length(X)<NX", _state);
    ae_assert(isfinitevector(x, s->nx, _state), "RBFDiffBuf: X contains infinite or NaN values", _state);
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
        y->ptr.p_double[i] = s->v.ptr.pp_double[i][rbfv1_mxnx];
        for(j=0; j<=s->nx-1; j++)
        {
            y->ptr.p_double[i] = y->ptr.p_double[i]+s->v.ptr.pp_double[i][j]*x->ptr.p_double[j];
            dy->ptr.p_double[i*s->nx+j] = s->v.ptr.pp_double[i][j];
        }
    }
    rsetv(s->ny*s->nx*s->nx, 0.0, d2y, _state);
    if( s->nc==0 )
    {
        return;
    }
    rvectorsetlengthatleast(&buf->calcbufxcx, rbfv1_mxnx, _state);
    for(i=0; i<=rbfv1_mxnx-1; i++)
    {
        buf->calcbufxcx.ptr.p_double[i] = 0.0;
    }
    for(i=0; i<=s->nx-1; i++)
    {
        buf->calcbufxcx.ptr.p_double[i] = x->ptr.p_double[i];
    }
    lx = kdtreetsqueryrnn(&s->tree, &buf->requestbuffer, &buf->calcbufxcx, s->rmax*rbfv1_rbffarradius, ae_true, _state);
    kdtreetsqueryresultsx(&s->tree, &buf->requestbuffer, &buf->calcbufx, _state);
    kdtreetsqueryresultstags(&s->tree, &buf->requestbuffer, &buf->calcbuftags, _state);
    for(i=0; i<=s->ny-1; i++)
    {
        for(j=0; j<=lx-1; j++)
        {
            tg = buf->calcbuftags.ptr.p_int[j];
            rcur = s->wr.ptr.pp_double[tg][0];
            invrcur2 = (double)1/(rcur*rcur);
            f = ae_exp(-(ae_sqr(buf->calcbufxcx.ptr.p_double[0]-buf->calcbufx.ptr.pp_double[j][0], _state)+ae_sqr(buf->calcbufxcx.ptr.p_double[1]-buf->calcbufx.ptr.pp_double[j][1], _state)+ae_sqr(buf->calcbufxcx.ptr.p_double[2]-buf->calcbufx.ptr.pp_double[j][2], _state))*invrcur2, _state);
            df = -f;
            d2f = f;
            for(k=0; k<=s->nl-1; k++)
            {
                w = s->wr.ptr.pp_double[tg][1+k*s->ny+i];
                y->ptr.p_double[i] = y->ptr.p_double[i]+f*w;
                for(i0=0; i0<=s->nx-1; i0++)
                {
                    for(i1=0; i1<=s->nx-1; i1++)
                    {
                        if( i0==i1 )
                        {
                            
                            /*
                             * Compute derivative and diagonal element of the Hessian
                             */
                            dy->ptr.p_double[i*s->nx+i0] = dy->ptr.p_double[i*s->nx+i0]+w*df*invrcur2*(double)2*(buf->calcbufxcx.ptr.p_double[i0]-buf->calcbufx.ptr.pp_double[j][i0]);
                            d2y->ptr.p_double[i*s->nx*s->nx+i0*s->nx+i1] = d2y->ptr.p_double[i*s->nx*s->nx+i0*s->nx+i1]+w*(d2f*invrcur2*invrcur2*(double)4*ae_sqr(buf->calcbufxcx.ptr.p_double[i0]-buf->calcbufx.ptr.pp_double[j][i0], _state)+df*invrcur2*(double)2);
                        }
                        else
                        {
                            
                            /*
                             * Compute off-diagonal element of the Hessian
                             */
                            d2y->ptr.p_double[i*s->nx*s->nx+i0*s->nx+i1] = d2y->ptr.p_double[i*s->nx*s->nx+i0*s->nx+i1]+w*d2f*invrcur2*invrcur2*(double)4*(buf->calcbufxcx.ptr.p_double[i0]-buf->calcbufx.ptr.pp_double[j][i0])*(buf->calcbufxcx.ptr.p_double[i1]-buf->calcbufx.ptr.pp_double[j][i1]);
                        }
                    }
                }
                t = f*f;
                f = t*t;
                df = -f;
                d2f = f;
                invrcur2 = (double)4*invrcur2;
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
void rbfv1gridcalc2(rbfv1model* s,
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
    ae_vector p01;
    ae_vector p11;
    ae_vector p2;
    double rlimit;
    double xcnorm2;
    ae_int_t hp01;
    double hcpx0;
    double xc0;
    double xc1;
    double omega;
    double radius;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t d;
    ae_int_t i00;
    ae_int_t i01;
    ae_int_t i10;
    ae_int_t i11;

    ae_frame_make(_state, &_frame_block);
    memset(&cpx0, 0, sizeof(cpx0));
    memset(&cpx1, 0, sizeof(cpx1));
    memset(&p01, 0, sizeof(p01));
    memset(&p11, 0, sizeof(p11));
    memset(&p2, 0, sizeof(p2));
    ae_matrix_clear(y);
    ae_vector_init(&cpx0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&cpx1, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&p01, 0, DT_INT, _state, ae_true);
    ae_vector_init(&p11, 0, DT_INT, _state, ae_true);
    ae_vector_init(&p2, 0, DT_INT, _state, ae_true);

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
    if( (s->ny!=1||s->nx!=2)||s->nc==0 )
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
    
    /*
     *calculate function's value
     */
    for(i=0; i<=s->nc-1; i++)
    {
        radius = s->wr.ptr.pp_double[i][0];
        for(d=0; d<=s->nl-1; d++)
        {
            omega = s->wr.ptr.pp_double[i][1+d];
            rlimit = radius*rbfv1_rbffarradius;
            
            /*
             *search lower and upper indexes
             */
            i00 = lowerbound(&cpx0, n0, s->xc.ptr.pp_double[i][0]-rlimit, _state);
            i01 = upperbound(&cpx0, n0, s->xc.ptr.pp_double[i][0]+rlimit, _state);
            i10 = lowerbound(&cpx1, n1, s->xc.ptr.pp_double[i][1]-rlimit, _state);
            i11 = upperbound(&cpx1, n1, s->xc.ptr.pp_double[i][1]+rlimit, _state);
            xc0 = s->xc.ptr.pp_double[i][0];
            xc1 = s->xc.ptr.pp_double[i][1];
            for(j=i00; j<=i01-1; j++)
            {
                hcpx0 = cpx0.ptr.p_double[j];
                hp01 = p01.ptr.p_int[j];
                for(k=i10; k<=i11-1; k++)
                {
                    xcnorm2 = ae_sqr(hcpx0-xc0, _state)+ae_sqr(cpx1.ptr.p_double[k]-xc1, _state);
                    if( ae_fp_less_eq(xcnorm2,rlimit*rlimit) )
                    {
                        y->ptr.pp_double[hp01][p11.ptr.p_int[k]] = y->ptr.pp_double[hp01][p11.ptr.p_int[k]]+ae_exp(-xcnorm2/ae_sqr(radius, _state), _state)*omega;
                    }
                }
            }
            radius = 0.5*radius;
        }
    }
    
    /*
     *add linear term
     */
    for(i=0; i<=n0-1; i++)
    {
        for(j=0; j<=n1-1; j++)
        {
            y->ptr.pp_double[i][j] = y->ptr.pp_double[i][j]+s->v.ptr.pp_double[0][0]*x0->ptr.p_double[i]+s->v.ptr.pp_double[0][1]*x1->ptr.p_double[j]+s->v.ptr.pp_double[0][rbfv1_mxnx];
        }
    }
    ae_frame_leave(_state);
}


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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t t;
    ae_int_t l;
    ae_int_t i0;
    ae_int_t i1;
    ae_int_t i2;
    ae_int_t ic;
    gridcalc3v1buf *pbuf;
    ae_smart_ptr _pbuf;
    ae_int_t flag12dim1;
    ae_int_t flag12dim2;
    double problemcost;
    ae_int_t maxbs;
    ae_int_t nx;
    ae_int_t ny;
    double v;
    ae_int_t kc;
    ae_int_t tg;
    double rcur;
    double rcur2;
    double basisfuncval;
    ae_int_t dstoffs;
    ae_int_t srcoffs;
    ae_int_t ubnd;
    double w0;
    double w1;
    double w2;
    ae_bool allnodes;
    ae_bool somenodes;

    ae_frame_make(_state, &_frame_block);
    memset(&_pbuf, 0, sizeof(_pbuf));
    ae_smart_ptr_init(&_pbuf, (void**)&pbuf, _state, ae_true);

    nx = s->nx;
    ny = s->ny;
    
    /*
     * Try to split large problem
     */
    problemcost = (double)((s->nl+1)*s->ny*2)*(avgfuncpernode+(double)1);
    problemcost = problemcost*(double)(blocks0->ptr.p_int[block0b]-blocks0->ptr.p_int[block0a]);
    problemcost = problemcost*(double)(blocks1->ptr.p_int[block1b]-blocks1->ptr.p_int[block1a]);
    problemcost = problemcost*(double)(blocks2->ptr.p_int[block2b]-blocks2->ptr.p_int[block2a]);
    maxbs = 0;
    maxbs = ae_maxint(maxbs, block0b-block0a, _state);
    maxbs = ae_maxint(maxbs, block1b-block1a, _state);
    maxbs = ae_maxint(maxbs, block2b-block2a, _state);
    if( ae_fp_greater_eq(problemcost,rbfv1_minbasecasecost)&&maxbs>=2 )
    {
        ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_true, _state);
        if( block0b-block0a==maxbs )
        {
            _spawn_rbfv1gridcalc3vrec(s, x0, n0, x1, n1, x2, n2, blocks0, block0a, block0a+maxbs/2, blocks1, block1a, block1b, blocks2, block2a, block2b, flagy, sparsey, searchradius, avgfuncpernode, bufpool, y, _child_tasks, _smp_enabled, _state);
            rbfv1gridcalc3vrec(s, x0, n0, x1, n1, x2, n2, blocks0, block0a+maxbs/2, block0b, blocks1, block1a, block1b, blocks2, block2a, block2b, flagy, sparsey, searchradius, avgfuncpernode, bufpool, y, _state);
            ae_sync(_child_tasks, _smp_enabled, ae_false, _state);
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            ae_frame_leave(_state);
            return;
        }
        if( block1b-block1a==maxbs )
        {
            _spawn_rbfv1gridcalc3vrec(s, x0, n0, x1, n1, x2, n2, blocks0, block0a, block0b, blocks1, block1a, block1a+maxbs/2, blocks2, block2a, block2b, flagy, sparsey, searchradius, avgfuncpernode, bufpool, y, _child_tasks, _smp_enabled, _state);
            rbfv1gridcalc3vrec(s, x0, n0, x1, n1, x2, n2, blocks0, block0a, block0b, blocks1, block1a+maxbs/2, block1b, blocks2, block2a, block2b, flagy, sparsey, searchradius, avgfuncpernode, bufpool, y, _state);
            ae_sync(_child_tasks, _smp_enabled, ae_false, _state);
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            ae_frame_leave(_state);
            return;
        }
        if( block2b-block2a==maxbs )
        {
            _spawn_rbfv1gridcalc3vrec(s, x0, n0, x1, n1, x2, n2, blocks0, block0a, block0b, blocks1, block1a, block1b, blocks2, block2a, block2a+maxbs/2, flagy, sparsey, searchradius, avgfuncpernode, bufpool, y, _child_tasks, _smp_enabled, _state);
            rbfv1gridcalc3vrec(s, x0, n0, x1, n1, x2, n2, blocks0, block0a, block0b, blocks1, block1a, block1b, blocks2, block2a+maxbs/2, block2b, flagy, sparsey, searchradius, avgfuncpernode, bufpool, y, _state);
            ae_sync(_child_tasks, _smp_enabled, ae_false, _state);
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            ae_frame_leave(_state);
            return;
        }
    }
    
    /*
     * Retrieve buffer object from pool (it will be returned later)
     */
    ae_shared_pool_retrieve(bufpool, &_pbuf, _state);
    
    /*
     * Calculate RBF model
     */
    for(i2=block2a; i2<=block2b-1; i2++)
    {
        for(i1=block1a; i1<=block1b-1; i1++)
        {
            for(i0=block0a; i0<=block0b-1; i0++)
            {
                
                /*
                 * Analyze block - determine what elements are needed and what are not.
                 *
                 * After this block is done, two flag variables can be used:
                 * * SomeNodes, which is True when there are at least one node which have
                 *   to be calculated
                 * * AllNodes, which is True when all nodes are required
                 */
                somenodes = ae_true;
                allnodes = ae_true;
                flag12dim1 = blocks1->ptr.p_int[i1+1]-blocks1->ptr.p_int[i1];
                flag12dim2 = blocks2->ptr.p_int[i2+1]-blocks2->ptr.p_int[i2];
                if( sparsey )
                {
                    
                    /*
                     * Use FlagY to determine what is required.
                     */
                    bvectorsetlengthatleast(&pbuf->flag0, n0, _state);
                    bvectorsetlengthatleast(&pbuf->flag1, n1, _state);
                    bvectorsetlengthatleast(&pbuf->flag2, n2, _state);
                    bvectorsetlengthatleast(&pbuf->flag12, flag12dim1*flag12dim2, _state);
                    for(i=blocks0->ptr.p_int[i0]; i<=blocks0->ptr.p_int[i0+1]-1; i++)
                    {
                        pbuf->flag0.ptr.p_bool[i] = ae_false;
                    }
                    for(j=blocks1->ptr.p_int[i1]; j<=blocks1->ptr.p_int[i1+1]-1; j++)
                    {
                        pbuf->flag1.ptr.p_bool[j] = ae_false;
                    }
                    for(k=blocks2->ptr.p_int[i2]; k<=blocks2->ptr.p_int[i2+1]-1; k++)
                    {
                        pbuf->flag2.ptr.p_bool[k] = ae_false;
                    }
                    for(i=0; i<=flag12dim1*flag12dim2-1; i++)
                    {
                        pbuf->flag12.ptr.p_bool[i] = ae_false;
                    }
                    somenodes = ae_false;
                    allnodes = ae_true;
                    for(k=blocks2->ptr.p_int[i2]; k<=blocks2->ptr.p_int[i2+1]-1; k++)
                    {
                        for(j=blocks1->ptr.p_int[i1]; j<=blocks1->ptr.p_int[i1+1]-1; j++)
                        {
                            dstoffs = j-blocks1->ptr.p_int[i1]+flag12dim1*(k-blocks2->ptr.p_int[i2]);
                            srcoffs = j*n0+k*n0*n1;
                            for(i=blocks0->ptr.p_int[i0]; i<=blocks0->ptr.p_int[i0+1]-1; i++)
                            {
                                if( flagy->ptr.p_bool[srcoffs+i] )
                                {
                                    pbuf->flag0.ptr.p_bool[i] = ae_true;
                                    pbuf->flag1.ptr.p_bool[j] = ae_true;
                                    pbuf->flag2.ptr.p_bool[k] = ae_true;
                                    pbuf->flag12.ptr.p_bool[dstoffs] = ae_true;
                                    somenodes = ae_true;
                                }
                                else
                                {
                                    allnodes = ae_false;
                                }
                            }
                        }
                    }
                }
                
                /*
                 * Skip block if it is completely empty.
                 */
                if( !somenodes )
                {
                    continue;
                }
                
                /*
                 * compute linear term for block (I0,I1,I2)
                 */
                for(k=blocks2->ptr.p_int[i2]; k<=blocks2->ptr.p_int[i2+1]-1; k++)
                {
                    for(j=blocks1->ptr.p_int[i1]; j<=blocks1->ptr.p_int[i1+1]-1; j++)
                    {
                        
                        /*
                         * do we need this micro-row?
                         */
                        if( !allnodes&&!pbuf->flag12.ptr.p_bool[j-blocks1->ptr.p_int[i1]+flag12dim1*(k-blocks2->ptr.p_int[i2])] )
                        {
                            continue;
                        }
                        
                        /*
                         * Compute linear term
                         */
                        for(i=blocks0->ptr.p_int[i0]; i<=blocks0->ptr.p_int[i0+1]-1; i++)
                        {
                            pbuf->tx.ptr.p_double[0] = x0->ptr.p_double[i];
                            pbuf->tx.ptr.p_double[1] = x1->ptr.p_double[j];
                            pbuf->tx.ptr.p_double[2] = x2->ptr.p_double[k];
                            for(l=0; l<=s->ny-1; l++)
                            {
                                v = s->v.ptr.pp_double[l][rbfv1_mxnx];
                                for(t=0; t<=nx-1; t++)
                                {
                                    v = v+s->v.ptr.pp_double[l][t]*pbuf->tx.ptr.p_double[t];
                                }
                                y->ptr.p_double[l+ny*(i+j*n0+k*n0*n1)] = v;
                            }
                        }
                    }
                }
                
                /*
                 * compute RBF term for block (I0,I1,I2)
                 */
                pbuf->tx.ptr.p_double[0] = 0.5*(x0->ptr.p_double[blocks0->ptr.p_int[i0]]+x0->ptr.p_double[blocks0->ptr.p_int[i0+1]-1]);
                pbuf->tx.ptr.p_double[1] = 0.5*(x1->ptr.p_double[blocks1->ptr.p_int[i1]]+x1->ptr.p_double[blocks1->ptr.p_int[i1+1]-1]);
                pbuf->tx.ptr.p_double[2] = 0.5*(x2->ptr.p_double[blocks2->ptr.p_int[i2]]+x2->ptr.p_double[blocks2->ptr.p_int[i2+1]-1]);
                kc = kdtreetsqueryrnn(&s->tree, &pbuf->requestbuf, &pbuf->tx, searchradius, ae_true, _state);
                kdtreetsqueryresultsx(&s->tree, &pbuf->requestbuf, &pbuf->calcbufx, _state);
                kdtreetsqueryresultstags(&s->tree, &pbuf->requestbuf, &pbuf->calcbuftags, _state);
                for(ic=0; ic<=kc-1; ic++)
                {
                    pbuf->cx.ptr.p_double[0] = pbuf->calcbufx.ptr.pp_double[ic][0];
                    pbuf->cx.ptr.p_double[1] = pbuf->calcbufx.ptr.pp_double[ic][1];
                    pbuf->cx.ptr.p_double[2] = pbuf->calcbufx.ptr.pp_double[ic][2];
                    tg = pbuf->calcbuftags.ptr.p_int[ic];
                    rcur = s->wr.ptr.pp_double[tg][0];
                    rcur2 = rcur*rcur;
                    for(i=blocks0->ptr.p_int[i0]; i<=blocks0->ptr.p_int[i0+1]-1; i++)
                    {
                        if( allnodes||pbuf->flag0.ptr.p_bool[i] )
                        {
                            pbuf->expbuf0.ptr.p_double[i] = ae_exp(-ae_sqr(x0->ptr.p_double[i]-pbuf->cx.ptr.p_double[0], _state)/rcur2, _state);
                        }
                        else
                        {
                            pbuf->expbuf0.ptr.p_double[i] = 0.0;
                        }
                    }
                    for(j=blocks1->ptr.p_int[i1]; j<=blocks1->ptr.p_int[i1+1]-1; j++)
                    {
                        if( allnodes||pbuf->flag1.ptr.p_bool[j] )
                        {
                            pbuf->expbuf1.ptr.p_double[j] = ae_exp(-ae_sqr(x1->ptr.p_double[j]-pbuf->cx.ptr.p_double[1], _state)/rcur2, _state);
                        }
                        else
                        {
                            pbuf->expbuf1.ptr.p_double[j] = 0.0;
                        }
                    }
                    for(k=blocks2->ptr.p_int[i2]; k<=blocks2->ptr.p_int[i2+1]-1; k++)
                    {
                        if( allnodes||pbuf->flag2.ptr.p_bool[k] )
                        {
                            pbuf->expbuf2.ptr.p_double[k] = ae_exp(-ae_sqr(x2->ptr.p_double[k]-pbuf->cx.ptr.p_double[2], _state)/rcur2, _state);
                        }
                        else
                        {
                            pbuf->expbuf2.ptr.p_double[k] = 0.0;
                        }
                    }
                    for(t=0; t<=s->nl-1; t++)
                    {
                        
                        /*
                         * Calculate
                         */
                        for(k=blocks2->ptr.p_int[i2]; k<=blocks2->ptr.p_int[i2+1]-1; k++)
                        {
                            for(j=blocks1->ptr.p_int[i1]; j<=blocks1->ptr.p_int[i1+1]-1; j++)
                            {
                                
                                /*
                                 * do we need this micro-row?
                                 */
                                if( !allnodes&&!pbuf->flag12.ptr.p_bool[j-blocks1->ptr.p_int[i1]+flag12dim1*(k-blocks2->ptr.p_int[i2])] )
                                {
                                    continue;
                                }
                                
                                /*
                                 * Prepare local variables
                                 */
                                dstoffs = ny*(blocks0->ptr.p_int[i0]+j*n0+k*n0*n1);
                                v = pbuf->expbuf1.ptr.p_double[j]*pbuf->expbuf2.ptr.p_double[k];
                                
                                /*
                                 * Optimized for NY=1
                                 */
                                if( s->ny==1 )
                                {
                                    w0 = s->wr.ptr.pp_double[tg][1+t*s->ny+0];
                                    ubnd = blocks0->ptr.p_int[i0+1]-1;
                                    for(i=blocks0->ptr.p_int[i0]; i<=ubnd; i++)
                                    {
                                        basisfuncval = pbuf->expbuf0.ptr.p_double[i]*v;
                                        y->ptr.p_double[dstoffs] = y->ptr.p_double[dstoffs]+basisfuncval*w0;
                                        dstoffs = dstoffs+1;
                                    }
                                    continue;
                                }
                                
                                /*
                                 * Optimized for NY=2
                                 */
                                if( s->ny==2 )
                                {
                                    w0 = s->wr.ptr.pp_double[tg][1+t*s->ny+0];
                                    w1 = s->wr.ptr.pp_double[tg][1+t*s->ny+1];
                                    ubnd = blocks0->ptr.p_int[i0+1]-1;
                                    for(i=blocks0->ptr.p_int[i0]; i<=ubnd; i++)
                                    {
                                        basisfuncval = pbuf->expbuf0.ptr.p_double[i]*v;
                                        y->ptr.p_double[dstoffs+0] = y->ptr.p_double[dstoffs+0]+basisfuncval*w0;
                                        y->ptr.p_double[dstoffs+1] = y->ptr.p_double[dstoffs+1]+basisfuncval*w1;
                                        dstoffs = dstoffs+2;
                                    }
                                    continue;
                                }
                                
                                /*
                                 * Optimized for NY=3
                                 */
                                if( s->ny==3 )
                                {
                                    w0 = s->wr.ptr.pp_double[tg][1+t*s->ny+0];
                                    w1 = s->wr.ptr.pp_double[tg][1+t*s->ny+1];
                                    w2 = s->wr.ptr.pp_double[tg][1+t*s->ny+2];
                                    ubnd = blocks0->ptr.p_int[i0+1]-1;
                                    for(i=blocks0->ptr.p_int[i0]; i<=ubnd; i++)
                                    {
                                        basisfuncval = pbuf->expbuf0.ptr.p_double[i]*v;
                                        y->ptr.p_double[dstoffs+0] = y->ptr.p_double[dstoffs+0]+basisfuncval*w0;
                                        y->ptr.p_double[dstoffs+1] = y->ptr.p_double[dstoffs+1]+basisfuncval*w1;
                                        y->ptr.p_double[dstoffs+2] = y->ptr.p_double[dstoffs+2]+basisfuncval*w2;
                                        dstoffs = dstoffs+3;
                                    }
                                    continue;
                                }
                                
                                /*
                                 * General case
                                 */
                                for(i=blocks0->ptr.p_int[i0]; i<=blocks0->ptr.p_int[i0+1]-1; i++)
                                {
                                    basisfuncval = pbuf->expbuf0.ptr.p_double[i]*v;
                                    for(l=0; l<=s->ny-1; l++)
                                    {
                                        y->ptr.p_double[l+dstoffs] = y->ptr.p_double[l+dstoffs]+basisfuncval*s->wr.ptr.pp_double[tg][1+t*s->ny+l];
                                    }
                                    dstoffs = dstoffs+ny;
                                }
                            }
                        }
                        
                        /*
                         * Update basis functions
                         */
                        if( t!=s->nl-1 )
                        {
                            ubnd = blocks0->ptr.p_int[i0+1]-1;
                            for(i=blocks0->ptr.p_int[i0]; i<=ubnd; i++)
                            {
                                if( allnodes||pbuf->flag0.ptr.p_bool[i] )
                                {
                                    v = pbuf->expbuf0.ptr.p_double[i]*pbuf->expbuf0.ptr.p_double[i];
                                    pbuf->expbuf0.ptr.p_double[i] = v*v;
                                }
                            }
                            ubnd = blocks1->ptr.p_int[i1+1]-1;
                            for(j=blocks1->ptr.p_int[i1]; j<=ubnd; j++)
                            {
                                if( allnodes||pbuf->flag1.ptr.p_bool[j] )
                                {
                                    v = pbuf->expbuf1.ptr.p_double[j]*pbuf->expbuf1.ptr.p_double[j];
                                    pbuf->expbuf1.ptr.p_double[j] = v*v;
                                }
                            }
                            ubnd = blocks2->ptr.p_int[i2+1]-1;
                            for(k=blocks2->ptr.p_int[i2]; k<=ubnd; k++)
                            {
                                if( allnodes||pbuf->flag2.ptr.p_bool[k] )
                                {
                                    v = pbuf->expbuf2.ptr.p_double[k]*pbuf->expbuf2.ptr.p_double[k];
                                    pbuf->expbuf2.ptr.p_double[k] = v*v;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    /*
     * Recycle buffer object back to pool
     */
    ae_shared_pool_recycle(bufpool, &_pbuf, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
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
    /* Real    */ ae_vector* y,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        rbfv1gridcalc3vrec(s,x0,n0,x1,n1,x2,n2,blocks0,block0a,block0b,blocks1,block1a,block1b,blocks2,block2a,block2b,flagy,sparsey,searchradius,avgfuncpernode,bufpool,y, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_rbfv1gridcalc3vrec;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.const_val = s;
        _task->data.parameters[1].value.const_val = x0;
        _task->data.parameters[2].value.ival = n0;
        _task->data.parameters[3].value.const_val = x1;
        _task->data.parameters[4].value.ival = n1;
        _task->data.parameters[5].value.const_val = x2;
        _task->data.parameters[6].value.ival = n2;
        _task->data.parameters[7].value.const_val = blocks0;
        _task->data.parameters[8].value.ival = block0a;
        _task->data.parameters[9].value.ival = block0b;
        _task->data.parameters[10].value.const_val = blocks1;
        _task->data.parameters[11].value.ival = block1a;
        _task->data.parameters[12].value.ival = block1b;
        _task->data.parameters[13].value.const_val = blocks2;
        _task->data.parameters[14].value.ival = block2a;
        _task->data.parameters[15].value.ival = block2b;
        _task->data.parameters[16].value.const_val = flagy;
        _task->data.parameters[17].value.bval = sparsey;
        _task->data.parameters[18].value.dval = searchradius;
        _task->data.parameters[19].value.dval = avgfuncpernode;
        _task->data.parameters[20].value.val = bufpool;
        _task->data.parameters[21].value.val = y;
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
void _task_rbfv1gridcalc3vrec(ae_task_data *_data, ae_state *_state)
{
    const rbfv1model* s;
    const ae_vector* x0;
    ae_int_t n0;
    const ae_vector* x1;
    ae_int_t n1;
    const ae_vector* x2;
    ae_int_t n2;
    const ae_vector* blocks0;
    ae_int_t block0a;
    ae_int_t block0b;
    const ae_vector* blocks1;
    ae_int_t block1a;
    ae_int_t block1b;
    const ae_vector* blocks2;
    ae_int_t block2a;
    ae_int_t block2b;
    const ae_vector* flagy;
    ae_bool sparsey;
    double searchradius;
    double avgfuncpernode;
    ae_shared_pool* bufpool;
    ae_vector* y;
    s = (const rbfv1model*)_data->parameters[0].value.const_val;
    x0 = (const ae_vector*)_data->parameters[1].value.const_val;
    n0 = _data->parameters[2].value.ival;
    x1 = (const ae_vector*)_data->parameters[3].value.const_val;
    n1 = _data->parameters[4].value.ival;
    x2 = (const ae_vector*)_data->parameters[5].value.const_val;
    n2 = _data->parameters[6].value.ival;
    blocks0 = (const ae_vector*)_data->parameters[7].value.const_val;
    block0a = _data->parameters[8].value.ival;
    block0b = _data->parameters[9].value.ival;
    blocks1 = (const ae_vector*)_data->parameters[10].value.const_val;
    block1a = _data->parameters[11].value.ival;
    block1b = _data->parameters[12].value.ival;
    blocks2 = (const ae_vector*)_data->parameters[13].value.const_val;
    block2a = _data->parameters[14].value.ival;
    block2b = _data->parameters[15].value.ival;
    flagy = (const ae_vector*)_data->parameters[16].value.const_val;
    sparsey = _data->parameters[17].value.bval;
    searchradius = _data->parameters[18].value.dval;
    avgfuncpernode = _data->parameters[19].value.dval;
    bufpool = (ae_shared_pool*)_data->parameters[20].value.val;
    y = (ae_vector*)_data->parameters[21].value.val;
   ae_state_set_flags(_state, _data->flags);
   rbfv1gridcalc3vrec(s,x0,n0,x1,n1,x2,n2,blocks0,block0a,block0b,blocks1,block1a,block1b,blocks2,block2a,block2b,flagy,sparsey,searchradius,avgfuncpernode,bufpool,y, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
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
    /* Real    */ ae_vector* y,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_rbfv1gridcalc3vrec;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.const_val = s;
    _task->data.parameters[1].value.const_val = x0;
    _task->data.parameters[2].value.ival = n0;
    _task->data.parameters[3].value.const_val = x1;
    _task->data.parameters[4].value.ival = n1;
    _task->data.parameters[5].value.const_val = x2;
    _task->data.parameters[6].value.ival = n2;
    _task->data.parameters[7].value.const_val = blocks0;
    _task->data.parameters[8].value.ival = block0a;
    _task->data.parameters[9].value.ival = block0b;
    _task->data.parameters[10].value.const_val = blocks1;
    _task->data.parameters[11].value.ival = block1a;
    _task->data.parameters[12].value.ival = block1b;
    _task->data.parameters[13].value.const_val = blocks2;
    _task->data.parameters[14].value.ival = block2a;
    _task->data.parameters[15].value.ival = block2b;
    _task->data.parameters[16].value.const_val = flagy;
    _task->data.parameters[17].value.bval = sparsey;
    _task->data.parameters[18].value.dval = searchradius;
    _task->data.parameters[19].value.dval = avgfuncpernode;
    _task->data.parameters[20].value.val = bufpool;
    _task->data.parameters[21].value.val = y;
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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double rcur;

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
        ae_v_move(&v->ptr.pp_double[i][0], 1, &s->v.ptr.pp_double[i][0], 1, ae_v_len(0,s->nx-1));
        v->ptr.pp_double[i][s->nx] = s->v.ptr.pp_double[i][rbfv1_mxnx];
    }
    
    /*
     * Fill XWR and V
     */
    if( *nc*s->nl>0 )
    {
        ae_matrix_set_length(xwr, s->nc*s->nl, s->nx+s->ny+1, _state);
        for(i=0; i<=s->nc-1; i++)
        {
            rcur = s->wr.ptr.pp_double[i][0];
            for(j=0; j<=s->nl-1; j++)
            {
                ae_v_move(&xwr->ptr.pp_double[i*s->nl+j][0], 1, &s->xc.ptr.pp_double[i][0], 1, ae_v_len(0,s->nx-1));
                ae_v_move(&xwr->ptr.pp_double[i*s->nl+j][s->nx], 1, &s->wr.ptr.pp_double[i][1+j*s->ny], 1, ae_v_len(s->nx,s->nx+s->ny-1));
                xwr->ptr.pp_double[i*s->nl+j][s->nx+s->ny] = rcur;
                rcur = 0.5*rcur;
            }
        }
    }
}


static ae_bool rbfv1_rbfv1buildlinearmodel(/* Real    */ const ae_matrix* x,
     /* Real    */ ae_matrix* y,
     ae_int_t n,
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
    ae_assert(ny>0, "BuildLinearModel: NY<=0", _state);
    
    /*
     * Handle degenerate case (N=0)
     */
    result = ae_true;
    ae_matrix_set_length(v, ny, rbfv1_mxnx+1, _state);
    if( n==0 )
    {
        for(j=0; j<=rbfv1_mxnx; j++)
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
        ae_matrix_set_length(&a, n, rbfv1_mxnx+1, _state);
        ae_vector_set_length(&shifting, rbfv1_mxnx, _state);
        scaling = (double)(0);
        for(i=0; i<=rbfv1_mxnx-1; i++)
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
            for(j=0; j<=rbfv1_mxnx-1; j++)
            {
                a.ptr.pp_double[i][j] = (x->ptr.pp_double[i][j]-shifting.ptr.p_double[j])/scaling;
            }
        }
        for(i=0; i<=n-1; i++)
        {
            a.ptr.pp_double[i][rbfv1_mxnx] = (double)(1);
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
            lsfitlinear(&tmpy, &a, n, rbfv1_mxnx+1, &c, &rep, _state);
            if( rep.terminationtype<=0 )
            {
                result = ae_false;
                ae_frame_leave(_state);
                return result;
            }
            for(j=0; j<=rbfv1_mxnx-1; j++)
            {
                v->ptr.pp_double[i][j] = c.ptr.p_double[j]/scaling;
            }
            v->ptr.pp_double[i][rbfv1_mxnx] = c.ptr.p_double[rbfv1_mxnx];
            for(j=0; j<=rbfv1_mxnx-1; j++)
            {
                v->ptr.pp_double[i][rbfv1_mxnx] = v->ptr.pp_double[i][rbfv1_mxnx]-shifting.ptr.p_double[j]*v->ptr.pp_double[i][j];
            }
            for(j=0; j<=n-1; j++)
            {
                for(k=0; k<=rbfv1_mxnx-1; k++)
                {
                    y->ptr.pp_double[j][i] = y->ptr.pp_double[j][i]-x->ptr.pp_double[j][k]*v->ptr.pp_double[i][k];
                }
                y->ptr.pp_double[j][i] = y->ptr.pp_double[j][i]-v->ptr.pp_double[i][rbfv1_mxnx];
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
            for(j=0; j<=rbfv1_mxnx; j++)
            {
                v->ptr.pp_double[i][j] = (double)(0);
            }
            for(j=0; j<=n-1; j++)
            {
                v->ptr.pp_double[i][rbfv1_mxnx] = v->ptr.pp_double[i][rbfv1_mxnx]+y->ptr.pp_double[j][i];
            }
            if( n>0 )
            {
                v->ptr.pp_double[i][rbfv1_mxnx] = v->ptr.pp_double[i][rbfv1_mxnx]/(double)n;
            }
            for(j=0; j<=n-1; j++)
            {
                y->ptr.pp_double[j][i] = y->ptr.pp_double[j][i]-v->ptr.pp_double[i][rbfv1_mxnx];
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
        for(j=0; j<=rbfv1_mxnx; j++)
        {
            v->ptr.pp_double[i][j] = (double)(0);
        }
    }
    ae_frame_leave(_state);
    return result;
}


static void rbfv1_buildrbfmodellsqr(/* Real    */ const ae_matrix* x,
     /* Real    */ ae_matrix* y,
     /* Real    */ const ae_matrix* xc,
     /* Real    */ const ae_vector* r,
     ae_int_t n,
     ae_int_t nc,
     ae_int_t ny,
     kdtree* pointstree,
     kdtree* centerstree,
     double epsort,
     double epserr,
     ae_int_t maxits,
     ae_int_t* gnnz,
     ae_int_t* snnz,
     /* Real    */ ae_matrix* w,
     ae_int_t* info,
     ae_int_t* iterationscount,
     ae_int_t* nmv,
     ae_state *_state)
{
    ae_frame _frame_block;
    linlsqrstate state;
    linlsqrreport lsqrrep;
    sparsematrix spg;
    sparsematrix sps;
    ae_vector nearcenterscnt;
    ae_vector nearpointscnt;
    ae_vector skipnearpointscnt;
    ae_vector farpointscnt;
    ae_int_t maxnearcenterscnt;
    ae_int_t maxnearpointscnt;
    ae_int_t maxfarpointscnt;
    ae_int_t sumnearcenterscnt;
    ae_int_t sumnearpointscnt;
    ae_int_t sumfarpointscnt;
    double maxrad;
    ae_vector pointstags;
    ae_vector centerstags;
    ae_matrix nearpoints;
    ae_matrix nearcenters;
    ae_matrix farpoints;
    ae_int_t tmpi;
    ae_int_t pointscnt;
    ae_int_t centerscnt;
    ae_vector xcx;
    ae_vector tmpy;
    ae_vector tc;
    ae_vector g;
    ae_vector c;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t sind;
    ae_matrix a;
    double vv;
    double vx;
    double vy;
    double vz;
    double vr;
    double gnorm2;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
    double fx;
    ae_matrix xx;
    ae_matrix cx;
    double mrad;

    ae_frame_make(_state, &_frame_block);
    memset(&state, 0, sizeof(state));
    memset(&lsqrrep, 0, sizeof(lsqrrep));
    memset(&spg, 0, sizeof(spg));
    memset(&sps, 0, sizeof(sps));
    memset(&nearcenterscnt, 0, sizeof(nearcenterscnt));
    memset(&nearpointscnt, 0, sizeof(nearpointscnt));
    memset(&skipnearpointscnt, 0, sizeof(skipnearpointscnt));
    memset(&farpointscnt, 0, sizeof(farpointscnt));
    memset(&pointstags, 0, sizeof(pointstags));
    memset(&centerstags, 0, sizeof(centerstags));
    memset(&nearpoints, 0, sizeof(nearpoints));
    memset(&nearcenters, 0, sizeof(nearcenters));
    memset(&farpoints, 0, sizeof(farpoints));
    memset(&xcx, 0, sizeof(xcx));
    memset(&tmpy, 0, sizeof(tmpy));
    memset(&tc, 0, sizeof(tc));
    memset(&g, 0, sizeof(g));
    memset(&c, 0, sizeof(c));
    memset(&a, 0, sizeof(a));
    memset(&tmp0, 0, sizeof(tmp0));
    memset(&tmp1, 0, sizeof(tmp1));
    memset(&tmp2, 0, sizeof(tmp2));
    memset(&xx, 0, sizeof(xx));
    memset(&cx, 0, sizeof(cx));
    *gnnz = 0;
    *snnz = 0;
    ae_matrix_clear(w);
    *info = 0;
    *iterationscount = 0;
    *nmv = 0;
    _linlsqrstate_init(&state, _state, ae_true);
    _linlsqrreport_init(&lsqrrep, _state, ae_true);
    _sparsematrix_init(&spg, _state, ae_true);
    _sparsematrix_init(&sps, _state, ae_true);
    ae_vector_init(&nearcenterscnt, 0, DT_INT, _state, ae_true);
    ae_vector_init(&nearpointscnt, 0, DT_INT, _state, ae_true);
    ae_vector_init(&skipnearpointscnt, 0, DT_INT, _state, ae_true);
    ae_vector_init(&farpointscnt, 0, DT_INT, _state, ae_true);
    ae_vector_init(&pointstags, 0, DT_INT, _state, ae_true);
    ae_vector_init(&centerstags, 0, DT_INT, _state, ae_true);
    ae_matrix_init(&nearpoints, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&nearcenters, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&farpoints, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xcx, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmpy, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tc, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&g, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&c, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&a, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp1, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp2, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xx, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&cx, 0, 0, DT_REAL, _state, ae_true);

    
    /*
     * Handle special cases: NC=0
     */
    if( nc==0 )
    {
        *info = 1;
        *iterationscount = 0;
        *nmv = 0;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Prepare for general case, NC>0
     */
    ae_vector_set_length(&xcx, rbfv1_mxnx, _state);
    ae_vector_set_length(&pointstags, n, _state);
    ae_vector_set_length(&centerstags, nc, _state);
    *info = -1;
    *iterationscount = 0;
    *nmv = 0;
    
    /*
     * This block prepares quantities used to compute approximate cardinal basis functions (ACBFs):
     * * NearCentersCnt[]   -   array[NC], whose elements store number of near centers used to build ACBF
     * * NearPointsCnt[]    -   array[NC], number of near points used to build ACBF
     * * FarPointsCnt[]     -   array[NC], number of far points (ones where ACBF is nonzero)
     * * MaxNearCentersCnt  -   max(NearCentersCnt)
     * * MaxNearPointsCnt   -   max(NearPointsCnt)
     * * SumNearCentersCnt  -   sum(NearCentersCnt)
     * * SumNearPointsCnt   -   sum(NearPointsCnt)
     * * SumFarPointsCnt    -   sum(FarPointsCnt)
     */
    ae_vector_set_length(&nearcenterscnt, nc, _state);
    ae_vector_set_length(&nearpointscnt, nc, _state);
    ae_vector_set_length(&skipnearpointscnt, nc, _state);
    ae_vector_set_length(&farpointscnt, nc, _state);
    maxnearcenterscnt = 0;
    maxnearpointscnt = 0;
    maxfarpointscnt = 0;
    sumnearcenterscnt = 0;
    sumnearpointscnt = 0;
    sumfarpointscnt = 0;
    for(i=0; i<=nc-1; i++)
    {
        for(j=0; j<=rbfv1_mxnx-1; j++)
        {
            xcx.ptr.p_double[j] = xc->ptr.pp_double[i][j];
        }
        
        /*
         * Determine number of near centers and maximum radius of near centers
         */
        nearcenterscnt.ptr.p_int[i] = kdtreequeryrnn(centerstree, &xcx, r->ptr.p_double[i]*rbfv1_rbfnearradius, ae_true, _state);
        kdtreequeryresultstags(centerstree, &centerstags, _state);
        maxrad = (double)(0);
        for(j=0; j<=nearcenterscnt.ptr.p_int[i]-1; j++)
        {
            maxrad = ae_maxreal(maxrad, ae_fabs(r->ptr.p_double[centerstags.ptr.p_int[j]], _state), _state);
        }
        
        /*
         * Determine number of near points (ones which used to build ACBF)
         * and skipped points (the most near points which are NOT used to build ACBF
         * and are NOT included in the near points count
         */
        skipnearpointscnt.ptr.p_int[i] = kdtreequeryrnn(pointstree, &xcx, 0.1*r->ptr.p_double[i], ae_true, _state);
        nearpointscnt.ptr.p_int[i] = kdtreequeryrnn(pointstree, &xcx, (r->ptr.p_double[i]+maxrad)*rbfv1_rbfnearradius, ae_true, _state)-skipnearpointscnt.ptr.p_int[i];
        ae_assert(nearpointscnt.ptr.p_int[i]>=0, "BuildRBFModelLSQR: internal error", _state);
        
        /*
         * Determine number of far points
         */
        farpointscnt.ptr.p_int[i] = kdtreequeryrnn(pointstree, &xcx, ae_maxreal(r->ptr.p_double[i]*rbfv1_rbfnearradius+maxrad*rbfv1_rbffarradius, r->ptr.p_double[i]*rbfv1_rbffarradius, _state), ae_true, _state);
        
        /*
         * calculate sum and max, make some basic checks
         */
        ae_assert(nearcenterscnt.ptr.p_int[i]>0, "BuildRBFModelLSQR: internal error", _state);
        maxnearcenterscnt = ae_maxint(maxnearcenterscnt, nearcenterscnt.ptr.p_int[i], _state);
        maxnearpointscnt = ae_maxint(maxnearpointscnt, nearpointscnt.ptr.p_int[i], _state);
        maxfarpointscnt = ae_maxint(maxfarpointscnt, farpointscnt.ptr.p_int[i], _state);
        sumnearcenterscnt = sumnearcenterscnt+nearcenterscnt.ptr.p_int[i];
        sumnearpointscnt = sumnearpointscnt+nearpointscnt.ptr.p_int[i];
        sumfarpointscnt = sumfarpointscnt+farpointscnt.ptr.p_int[i];
    }
    *snnz = sumnearcenterscnt;
    *gnnz = sumfarpointscnt;
    ae_assert(maxnearcenterscnt>0, "BuildRBFModelLSQR: internal error", _state);
    
    /*
     * Allocate temporaries.
     *
     * NOTE: we want to avoid allocation of zero-size arrays, so we
     *       use max(desired_size,1) instead of desired_size when performing
     *       memory allocation.
     */
    ae_matrix_set_length(&a, maxnearpointscnt+maxnearcenterscnt, maxnearcenterscnt, _state);
    ae_vector_set_length(&tmpy, maxnearpointscnt+maxnearcenterscnt, _state);
    ae_vector_set_length(&g, maxnearcenterscnt, _state);
    ae_vector_set_length(&c, maxnearcenterscnt, _state);
    ae_matrix_set_length(&nearcenters, maxnearcenterscnt, rbfv1_mxnx, _state);
    ae_matrix_set_length(&nearpoints, ae_maxint(maxnearpointscnt, 1, _state), rbfv1_mxnx, _state);
    ae_matrix_set_length(&farpoints, ae_maxint(maxfarpointscnt, 1, _state), rbfv1_mxnx, _state);
    
    /*
     * fill matrix SpG
     */
    sparsecreate(n, nc, *gnnz, &spg, _state);
    sparsecreate(nc, nc, *snnz, &sps, _state);
    for(i=0; i<=nc-1; i++)
    {
        centerscnt = nearcenterscnt.ptr.p_int[i];
        
        /*
         * main center
         */
        for(j=0; j<=rbfv1_mxnx-1; j++)
        {
            xcx.ptr.p_double[j] = xc->ptr.pp_double[i][j];
        }
        
        /*
         * center's tree
         */
        tmpi = kdtreequeryknn(centerstree, &xcx, centerscnt, ae_true, _state);
        ae_assert(tmpi==centerscnt, "BuildRBFModelLSQR: internal error", _state);
        kdtreequeryresultsx(centerstree, &cx, _state);
        kdtreequeryresultstags(centerstree, &centerstags, _state);
        
        /*
         * point's tree
         */
        mrad = (double)(0);
        for(j=0; j<=centerscnt-1; j++)
        {
            mrad = ae_maxreal(mrad, r->ptr.p_double[centerstags.ptr.p_int[j]], _state);
        }
        
        /*
         * we need to be sure that 'CTree' contains
         * at least one side center
         */
        sparseset(&sps, i, i, (double)(1), _state);
        c.ptr.p_double[0] = 1.0;
        for(j=1; j<=centerscnt-1; j++)
        {
            c.ptr.p_double[j] = 0.0;
        }
        if( centerscnt>1&&nearpointscnt.ptr.p_int[i]>0 )
        {
            
            /*
             * first KDTree request for points
             */
            pointscnt = nearpointscnt.ptr.p_int[i];
            tmpi = kdtreequeryknn(pointstree, &xcx, skipnearpointscnt.ptr.p_int[i]+nearpointscnt.ptr.p_int[i], ae_true, _state);
            ae_assert(tmpi==skipnearpointscnt.ptr.p_int[i]+nearpointscnt.ptr.p_int[i], "BuildRBFModelLSQR: internal error", _state);
            kdtreequeryresultsx(pointstree, &xx, _state);
            sind = skipnearpointscnt.ptr.p_int[i];
            for(j=0; j<=pointscnt-1; j++)
            {
                vx = xx.ptr.pp_double[sind+j][0];
                vy = xx.ptr.pp_double[sind+j][1];
                vz = xx.ptr.pp_double[sind+j][2];
                for(k=0; k<=centerscnt-1; k++)
                {
                    vr = 0.0;
                    vv = vx-cx.ptr.pp_double[k][0];
                    vr = vr+vv*vv;
                    vv = vy-cx.ptr.pp_double[k][1];
                    vr = vr+vv*vv;
                    vv = vz-cx.ptr.pp_double[k][2];
                    vr = vr+vv*vv;
                    vv = r->ptr.p_double[centerstags.ptr.p_int[k]];
                    a.ptr.pp_double[j][k] = ae_exp(-vr/(vv*vv), _state);
                }
            }
            for(j=0; j<=centerscnt-1; j++)
            {
                g.ptr.p_double[j] = ae_exp(-(ae_sqr(xcx.ptr.p_double[0]-cx.ptr.pp_double[j][0], _state)+ae_sqr(xcx.ptr.p_double[1]-cx.ptr.pp_double[j][1], _state)+ae_sqr(xcx.ptr.p_double[2]-cx.ptr.pp_double[j][2], _state))/ae_sqr(r->ptr.p_double[centerstags.ptr.p_int[j]], _state), _state);
            }
            
            /*
             * calculate the problem
             */
            gnorm2 = ae_v_dotproduct(&g.ptr.p_double[0], 1, &g.ptr.p_double[0], 1, ae_v_len(0,centerscnt-1));
            for(j=0; j<=pointscnt-1; j++)
            {
                vv = ae_v_dotproduct(&a.ptr.pp_double[j][0], 1, &g.ptr.p_double[0], 1, ae_v_len(0,centerscnt-1));
                vv = vv/gnorm2;
                tmpy.ptr.p_double[j] = -vv;
                ae_v_subd(&a.ptr.pp_double[j][0], 1, &g.ptr.p_double[0], 1, ae_v_len(0,centerscnt-1), vv);
            }
            for(j=pointscnt; j<=pointscnt+centerscnt-1; j++)
            {
                for(k=0; k<=centerscnt-1; k++)
                {
                    a.ptr.pp_double[j][k] = 0.0;
                }
                a.ptr.pp_double[j][j-pointscnt] = 1.0E-6;
                tmpy.ptr.p_double[j] = 0.0;
            }
            fblssolvels(&a, &tmpy, pointscnt+centerscnt, centerscnt, &tmp0, &tmp1, &tmp2, _state);
            ae_v_move(&c.ptr.p_double[0], 1, &tmpy.ptr.p_double[0], 1, ae_v_len(0,centerscnt-1));
            vv = ae_v_dotproduct(&g.ptr.p_double[0], 1, &c.ptr.p_double[0], 1, ae_v_len(0,centerscnt-1));
            vv = vv/gnorm2;
            ae_v_subd(&c.ptr.p_double[0], 1, &g.ptr.p_double[0], 1, ae_v_len(0,centerscnt-1), vv);
            vv = (double)1/gnorm2;
            ae_v_addd(&c.ptr.p_double[0], 1, &g.ptr.p_double[0], 1, ae_v_len(0,centerscnt-1), vv);
            for(j=0; j<=centerscnt-1; j++)
            {
                sparseset(&sps, i, centerstags.ptr.p_int[j], c.ptr.p_double[j], _state);
            }
        }
        
        /*
         * second KDTree request for points
         */
        pointscnt = farpointscnt.ptr.p_int[i];
        tmpi = kdtreequeryknn(pointstree, &xcx, pointscnt, ae_true, _state);
        ae_assert(tmpi==pointscnt, "BuildRBFModelLSQR: internal error", _state);
        kdtreequeryresultsx(pointstree, &xx, _state);
        kdtreequeryresultstags(pointstree, &pointstags, _state);
        
        /*
         *fill SpG matrix
         */
        for(j=0; j<=pointscnt-1; j++)
        {
            fx = (double)(0);
            vx = xx.ptr.pp_double[j][0];
            vy = xx.ptr.pp_double[j][1];
            vz = xx.ptr.pp_double[j][2];
            for(k=0; k<=centerscnt-1; k++)
            {
                vr = 0.0;
                vv = vx-cx.ptr.pp_double[k][0];
                vr = vr+vv*vv;
                vv = vy-cx.ptr.pp_double[k][1];
                vr = vr+vv*vv;
                vv = vz-cx.ptr.pp_double[k][2];
                vr = vr+vv*vv;
                vv = r->ptr.p_double[centerstags.ptr.p_int[k]];
                vv = vv*vv;
                fx = fx+c.ptr.p_double[k]*ae_exp(-vr/vv, _state);
            }
            sparseset(&spg, pointstags.ptr.p_int[j], i, fx, _state);
        }
    }
    sparseconverttocrs(&spg, _state);
    sparseconverttocrs(&sps, _state);
    
    /*
     * solve by LSQR method
     */
    ae_vector_set_length(&tmpy, n, _state);
    ae_vector_set_length(&tc, nc, _state);
    ae_matrix_set_length(w, nc, ny, _state);
    linlsqrcreate(n, nc, &state, _state);
    linlsqrsetcond(&state, epsort, epserr, maxits, _state);
    for(i=0; i<=ny-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            tmpy.ptr.p_double[j] = y->ptr.pp_double[j][i];
        }
        linlsqrsolvesparse(&state, &spg, &tmpy, _state);
        linlsqrresults(&state, &c, &lsqrrep, _state);
        if( lsqrrep.terminationtype<=0 )
        {
            *info = -4;
            ae_frame_leave(_state);
            return;
        }
        sparsemtv(&sps, &c, &tc, _state);
        for(j=0; j<=nc-1; j++)
        {
            w->ptr.pp_double[j][i] = tc.ptr.p_double[j];
        }
        *iterationscount = *iterationscount+lsqrrep.iterationscount;
        *nmv = *nmv+lsqrrep.nmv;
    }
    *info = 1;
    ae_frame_leave(_state);
}


static void rbfv1_buildrbfmlayersmodellsqr(/* Real    */ const ae_matrix* x,
     /* Real    */ ae_matrix* y,
     /* Real    */ ae_matrix* xc,
     double rval,
     /* Real    */ ae_vector* r,
     ae_int_t n,
     ae_int_t* nc,
     ae_int_t ny,
     ae_int_t nlayers,
     kdtree* centerstree,
     double epsort,
     double epserr,
     ae_int_t maxits,
     double lambdav,
     ae_int_t* annz,
     /* Real    */ ae_matrix* w,
     ae_int_t* info,
     ae_int_t* iterationscount,
     ae_int_t* nmv,
     ae_state *_state)
{
    ae_frame _frame_block;
    linlsqrstate state;
    linlsqrreport lsqrrep;
    sparsematrix spa;
    double anorm;
    ae_vector omega;
    ae_vector xx;
    ae_vector tmpy;
    ae_matrix cx;
    double yval;
    ae_int_t nec;
    ae_vector centerstags;
    ae_int_t layer;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double v;
    double rmaxbefore;
    double rmaxafter;

    ae_frame_make(_state, &_frame_block);
    memset(&state, 0, sizeof(state));
    memset(&lsqrrep, 0, sizeof(lsqrrep));
    memset(&spa, 0, sizeof(spa));
    memset(&omega, 0, sizeof(omega));
    memset(&xx, 0, sizeof(xx));
    memset(&tmpy, 0, sizeof(tmpy));
    memset(&cx, 0, sizeof(cx));
    memset(&centerstags, 0, sizeof(centerstags));
    ae_matrix_clear(xc);
    ae_vector_clear(r);
    *nc = 0;
    *annz = 0;
    ae_matrix_clear(w);
    *info = 0;
    *iterationscount = 0;
    *nmv = 0;
    _linlsqrstate_init(&state, _state, ae_true);
    _linlsqrreport_init(&lsqrrep, _state, ae_true);
    _sparsematrix_init(&spa, _state, ae_true);
    ae_vector_init(&omega, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xx, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmpy, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&cx, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&centerstags, 0, DT_INT, _state, ae_true);

    ae_assert(nlayers>=0, "BuildRBFMLayersModelLSQR: invalid argument(NLayers<0)", _state);
    ae_assert(n>=0, "BuildRBFMLayersModelLSQR: invalid argument(N<0)", _state);
    ae_assert(rbfv1_mxnx>0&&rbfv1_mxnx<=3, "BuildRBFMLayersModelLSQR: internal error(invalid global const MxNX: either MxNX<=0 or MxNX>3)", _state);
    *annz = 0;
    if( n==0||nlayers==0 )
    {
        *info = 1;
        *iterationscount = 0;
        *nmv = 0;
        ae_frame_leave(_state);
        return;
    }
    *nc = n*nlayers;
    ae_vector_set_length(&xx, rbfv1_mxnx, _state);
    ae_vector_set_length(&centerstags, n, _state);
    ae_matrix_set_length(xc, *nc, rbfv1_mxnx, _state);
    ae_vector_set_length(r, *nc, _state);
    for(i=0; i<=*nc-1; i++)
    {
        for(j=0; j<=rbfv1_mxnx-1; j++)
        {
            xc->ptr.pp_double[i][j] = x->ptr.pp_double[i%n][j];
        }
    }
    for(i=0; i<=*nc-1; i++)
    {
        r->ptr.p_double[i] = rval/ae_pow((double)(2), (double)(i/n), _state);
    }
    for(i=0; i<=n-1; i++)
    {
        centerstags.ptr.p_int[i] = i;
    }
    kdtreebuildtagged(xc, &centerstags, n, rbfv1_mxnx, 0, 2, centerstree, _state);
    ae_vector_set_length(&omega, n, _state);
    ae_vector_set_length(&tmpy, n, _state);
    ae_matrix_set_length(w, *nc, ny, _state);
    *info = -1;
    *iterationscount = 0;
    *nmv = 0;
    linlsqrcreate(n, n, &state, _state);
    linlsqrsetcond(&state, epsort, epserr, maxits, _state);
    linlsqrsetlambdai(&state, 1.0E-6, _state);
    
    /*
     * calculate number of non-zero elements for sparse matrix
     */
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=rbfv1_mxnx-1; j++)
        {
            xx.ptr.p_double[j] = x->ptr.pp_double[i][j];
        }
        *annz = *annz+kdtreequeryrnn(centerstree, &xx, r->ptr.p_double[0]*rbfv1_rbfmlradius, ae_true, _state);
    }
    for(layer=0; layer<=nlayers-1; layer++)
    {
        
        /*
         * Fill sparse matrix, calculate norm(A)
         */
        anorm = 0.0;
        sparsecreate(n, n, *annz, &spa, _state);
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=rbfv1_mxnx-1; j++)
            {
                xx.ptr.p_double[j] = x->ptr.pp_double[i][j];
            }
            nec = kdtreequeryrnn(centerstree, &xx, r->ptr.p_double[layer*n]*rbfv1_rbfmlradius, ae_true, _state);
            kdtreequeryresultsx(centerstree, &cx, _state);
            kdtreequeryresultstags(centerstree, &centerstags, _state);
            for(j=0; j<=nec-1; j++)
            {
                v = ae_exp(-(ae_sqr(xx.ptr.p_double[0]-cx.ptr.pp_double[j][0], _state)+ae_sqr(xx.ptr.p_double[1]-cx.ptr.pp_double[j][1], _state)+ae_sqr(xx.ptr.p_double[2]-cx.ptr.pp_double[j][2], _state))/ae_sqr(r->ptr.p_double[layer*n+centerstags.ptr.p_int[j]], _state), _state);
                sparseset(&spa, i, centerstags.ptr.p_int[j], v, _state);
                anorm = anorm+ae_sqr(v, _state);
            }
        }
        anorm = ae_sqrt(anorm, _state);
        sparseconverttocrs(&spa, _state);
        
        /*
         * Calculate maximum residual before adding new layer.
         * This value is not used by algorithm, the only purpose is to make debugging easier.
         */
        rmaxbefore = 0.0;
        for(j=0; j<=n-1; j++)
        {
            for(i=0; i<=ny-1; i++)
            {
                rmaxbefore = ae_maxreal(rmaxbefore, ae_fabs(y->ptr.pp_double[j][i], _state), _state);
            }
        }
        
        /*
         * Process NY dimensions of the target function
         */
        for(i=0; i<=ny-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                tmpy.ptr.p_double[j] = y->ptr.pp_double[j][i];
            }
            
            /*
             * calculate Omega for current layer
             */
            linlsqrsetlambdai(&state, lambdav*anorm/(double)n, _state);
            linlsqrsolvesparse(&state, &spa, &tmpy, _state);
            linlsqrresults(&state, &omega, &lsqrrep, _state);
            if( lsqrrep.terminationtype<=0 )
            {
                *info = -4;
                ae_frame_leave(_state);
                return;
            }
            
            /*
             * calculate error for current layer
             */
            for(j=0; j<=n-1; j++)
            {
                yval = (double)(0);
                for(k=0; k<=rbfv1_mxnx-1; k++)
                {
                    xx.ptr.p_double[k] = x->ptr.pp_double[j][k];
                }
                nec = kdtreequeryrnn(centerstree, &xx, r->ptr.p_double[layer*n]*rbfv1_rbffarradius, ae_true, _state);
                kdtreequeryresultsx(centerstree, &cx, _state);
                kdtreequeryresultstags(centerstree, &centerstags, _state);
                for(k=0; k<=nec-1; k++)
                {
                    yval = yval+omega.ptr.p_double[centerstags.ptr.p_int[k]]*ae_exp(-(ae_sqr(xx.ptr.p_double[0]-cx.ptr.pp_double[k][0], _state)+ae_sqr(xx.ptr.p_double[1]-cx.ptr.pp_double[k][1], _state)+ae_sqr(xx.ptr.p_double[2]-cx.ptr.pp_double[k][2], _state))/ae_sqr(r->ptr.p_double[layer*n+centerstags.ptr.p_int[k]], _state), _state);
                }
                y->ptr.pp_double[j][i] = y->ptr.pp_double[j][i]-yval;
            }
            
            /*
             * write Omega in out parameter W
             */
            for(j=0; j<=n-1; j++)
            {
                w->ptr.pp_double[layer*n+j][i] = omega.ptr.p_double[j];
            }
            *iterationscount = *iterationscount+lsqrrep.iterationscount;
            *nmv = *nmv+lsqrrep.nmv;
        }
        
        /*
         * Calculate maximum residual before adding new layer.
         * This value is not used by algorithm, the only purpose is to make debugging easier.
         */
        rmaxafter = 0.0;
        for(j=0; j<=n-1; j++)
        {
            for(i=0; i<=ny-1; i++)
            {
                rmaxafter = ae_maxreal(rmaxafter, ae_fabs(y->ptr.pp_double[j][i], _state), _state);
            }
        }
    }
    *info = 1;
    ae_frame_leave(_state);
}


void _rbfv1calcbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    rbfv1calcbuffer *p = (rbfv1calcbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->calcbufxcx, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->calcbufx, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->calcbuftags, 0, DT_INT, _state, make_automatic);
    _kdtreerequestbuffer_init(&p->requestbuffer, _state, make_automatic);
}


void _rbfv1calcbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    rbfv1calcbuffer       *dst = (rbfv1calcbuffer*)_dst;
    const rbfv1calcbuffer *src = (const rbfv1calcbuffer*)_src;
    ae_vector_init_copy(&dst->calcbufxcx, &src->calcbufxcx, _state, make_automatic);
    ae_matrix_init_copy(&dst->calcbufx, &src->calcbufx, _state, make_automatic);
    ae_vector_init_copy(&dst->calcbuftags, &src->calcbuftags, _state, make_automatic);
    _kdtreerequestbuffer_init_copy(&dst->requestbuffer, &src->requestbuffer, _state, make_automatic);
}


void _rbfv1calcbuffer_clear(void* _p)
{
    rbfv1calcbuffer *p = (rbfv1calcbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->calcbufxcx);
    ae_matrix_clear(&p->calcbufx);
    ae_vector_clear(&p->calcbuftags);
    _kdtreerequestbuffer_clear(&p->requestbuffer);
}


void _rbfv1calcbuffer_destroy(void* _p)
{
    rbfv1calcbuffer *p = (rbfv1calcbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->calcbufxcx);
    ae_matrix_destroy(&p->calcbufx);
    ae_vector_destroy(&p->calcbuftags);
    _kdtreerequestbuffer_destroy(&p->requestbuffer);
}


void _rbfv1model_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    rbfv1model *p = (rbfv1model*)_p;
    ae_touch_ptr((void*)p);
    _kdtree_init(&p->tree, _state, make_automatic);
    ae_matrix_init(&p->xc, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->wr, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->v, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->calcbufxcx, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->calcbufx, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->calcbuftags, 0, DT_INT, _state, make_automatic);
}


void _rbfv1model_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    rbfv1model       *dst = (rbfv1model*)_dst;
    const rbfv1model *src = (const rbfv1model*)_src;
    dst->ny = src->ny;
    dst->nx = src->nx;
    dst->nc = src->nc;
    dst->nl = src->nl;
    _kdtree_init_copy(&dst->tree, &src->tree, _state, make_automatic);
    ae_matrix_init_copy(&dst->xc, &src->xc, _state, make_automatic);
    ae_matrix_init_copy(&dst->wr, &src->wr, _state, make_automatic);
    dst->rmax = src->rmax;
    ae_matrix_init_copy(&dst->v, &src->v, _state, make_automatic);
    ae_vector_init_copy(&dst->calcbufxcx, &src->calcbufxcx, _state, make_automatic);
    ae_matrix_init_copy(&dst->calcbufx, &src->calcbufx, _state, make_automatic);
    ae_vector_init_copy(&dst->calcbuftags, &src->calcbuftags, _state, make_automatic);
}


void _rbfv1model_clear(void* _p)
{
    rbfv1model *p = (rbfv1model*)_p;
    ae_touch_ptr((void*)p);
    _kdtree_clear(&p->tree);
    ae_matrix_clear(&p->xc);
    ae_matrix_clear(&p->wr);
    ae_matrix_clear(&p->v);
    ae_vector_clear(&p->calcbufxcx);
    ae_matrix_clear(&p->calcbufx);
    ae_vector_clear(&p->calcbuftags);
}


void _rbfv1model_destroy(void* _p)
{
    rbfv1model *p = (rbfv1model*)_p;
    ae_touch_ptr((void*)p);
    _kdtree_destroy(&p->tree);
    ae_matrix_destroy(&p->xc);
    ae_matrix_destroy(&p->wr);
    ae_matrix_destroy(&p->v);
    ae_vector_destroy(&p->calcbufxcx);
    ae_matrix_destroy(&p->calcbufx);
    ae_vector_destroy(&p->calcbuftags);
}


void _gridcalc3v1buf_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    gridcalc3v1buf *p = (gridcalc3v1buf*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->tx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->cx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->ty, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->flag0, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->flag1, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->flag2, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->flag12, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->expbuf0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->expbuf1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->expbuf2, 0, DT_REAL, _state, make_automatic);
    _kdtreerequestbuffer_init(&p->requestbuf, _state, make_automatic);
    ae_matrix_init(&p->calcbufx, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->calcbuftags, 0, DT_INT, _state, make_automatic);
}


void _gridcalc3v1buf_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    gridcalc3v1buf       *dst = (gridcalc3v1buf*)_dst;
    const gridcalc3v1buf *src = (const gridcalc3v1buf*)_src;
    ae_vector_init_copy(&dst->tx, &src->tx, _state, make_automatic);
    ae_vector_init_copy(&dst->cx, &src->cx, _state, make_automatic);
    ae_vector_init_copy(&dst->ty, &src->ty, _state, make_automatic);
    ae_vector_init_copy(&dst->flag0, &src->flag0, _state, make_automatic);
    ae_vector_init_copy(&dst->flag1, &src->flag1, _state, make_automatic);
    ae_vector_init_copy(&dst->flag2, &src->flag2, _state, make_automatic);
    ae_vector_init_copy(&dst->flag12, &src->flag12, _state, make_automatic);
    ae_vector_init_copy(&dst->expbuf0, &src->expbuf0, _state, make_automatic);
    ae_vector_init_copy(&dst->expbuf1, &src->expbuf1, _state, make_automatic);
    ae_vector_init_copy(&dst->expbuf2, &src->expbuf2, _state, make_automatic);
    _kdtreerequestbuffer_init_copy(&dst->requestbuf, &src->requestbuf, _state, make_automatic);
    ae_matrix_init_copy(&dst->calcbufx, &src->calcbufx, _state, make_automatic);
    ae_vector_init_copy(&dst->calcbuftags, &src->calcbuftags, _state, make_automatic);
}


void _gridcalc3v1buf_clear(void* _p)
{
    gridcalc3v1buf *p = (gridcalc3v1buf*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->tx);
    ae_vector_clear(&p->cx);
    ae_vector_clear(&p->ty);
    ae_vector_clear(&p->flag0);
    ae_vector_clear(&p->flag1);
    ae_vector_clear(&p->flag2);
    ae_vector_clear(&p->flag12);
    ae_vector_clear(&p->expbuf0);
    ae_vector_clear(&p->expbuf1);
    ae_vector_clear(&p->expbuf2);
    _kdtreerequestbuffer_clear(&p->requestbuf);
    ae_matrix_clear(&p->calcbufx);
    ae_vector_clear(&p->calcbuftags);
}


void _gridcalc3v1buf_destroy(void* _p)
{
    gridcalc3v1buf *p = (gridcalc3v1buf*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->tx);
    ae_vector_destroy(&p->cx);
    ae_vector_destroy(&p->ty);
    ae_vector_destroy(&p->flag0);
    ae_vector_destroy(&p->flag1);
    ae_vector_destroy(&p->flag2);
    ae_vector_destroy(&p->flag12);
    ae_vector_destroy(&p->expbuf0);
    ae_vector_destroy(&p->expbuf1);
    ae_vector_destroy(&p->expbuf2);
    _kdtreerequestbuffer_destroy(&p->requestbuf);
    ae_matrix_destroy(&p->calcbufx);
    ae_vector_destroy(&p->calcbuftags);
}


void _rbfv1report_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    rbfv1report *p = (rbfv1report*)_p;
    ae_touch_ptr((void*)p);
}


void _rbfv1report_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    rbfv1report       *dst = (rbfv1report*)_dst;
    const rbfv1report *src = (const rbfv1report*)_src;
    dst->arows = src->arows;
    dst->acols = src->acols;
    dst->annz = src->annz;
    dst->iterationscount = src->iterationscount;
    dst->nmv = src->nmv;
    dst->terminationtype = src->terminationtype;
}


void _rbfv1report_clear(void* _p)
{
    rbfv1report *p = (rbfv1report*)_p;
    ae_touch_ptr((void*)p);
}


void _rbfv1report_destroy(void* _p)
{
    rbfv1report *p = (rbfv1report*)_p;
    ae_touch_ptr((void*)p);
}


/*$ End $*/
