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
#include "spline2d.h"


/*$ Declarations $*/
static double spline2d_cholreg = 1.0E-12;
static double spline2d_lambdaregblocklls = 1.0E-6;
static double spline2d_lambdaregfastddm = 1.0E-4;
static double spline2d_lambdadecay = 0.5;
static void spline2d_bicubiccalcderivatives(/* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* dx,
     /* Real    */ ae_matrix* dy,
     /* Real    */ ae_matrix* dxy,
     ae_state *_state);
static void spline2d_bicubiccalcderivativesmissing(/* Real    */ const ae_matrix* a,
     /* Boolean */ const ae_vector* ismissingnode,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* dx,
     /* Real    */ ae_matrix* dy,
     /* Real    */ ae_matrix* dxy,
     ae_state *_state);
static ae_bool spline2d_scanfornonmissingsegment(/* Boolean */ const ae_vector* ismissing,
     ae_int_t n,
     ae_int_t* i1,
     ae_int_t* i2,
     ae_state *_state);
static void spline2d_generatedesignmatrix(/* Real    */ const ae_vector* xy,
     ae_int_t npoints,
     ae_int_t d,
     ae_int_t kx,
     ae_int_t ky,
     double smoothing,
     double lambdareg,
     const spline1dinterpolant* basis1,
     sparsematrix* av,
     sparsematrix* ah,
     ae_int_t* arows,
     ae_state *_state);
static void spline2d_updatesplinetable(/* Real    */ const ae_vector* z,
     ae_int_t kx,
     ae_int_t ky,
     ae_int_t d,
     const spline1dinterpolant* basis1,
     ae_int_t bfrad,
     /* Real    */ ae_vector* ftbl,
     ae_int_t m,
     ae_int_t n,
     ae_int_t scalexy,
     ae_state *_state);
static void spline2d_fastddmfit(/* Real    */ ae_vector* xy,
     ae_int_t npoints,
     ae_int_t d,
     ae_int_t kx,
     ae_int_t ky,
     ae_int_t basecasex,
     ae_int_t basecasey,
     ae_int_t maxcoresize,
     ae_int_t interfacesize,
     ae_int_t nlayers,
     double smoothing,
     ae_int_t lsqrcnt,
     const spline1dinterpolant* basis1,
     spline2dinterpolant* spline,
     spline2dfitreport* rep,
     double tss,
     ae_state *_state);
static void spline2d_fastddmfitlayer(/* Real    */ const ae_vector* xy,
     ae_int_t d,
     ae_int_t scalexy,
     /* Integer */ const ae_vector* xyindex,
     ae_int_t basecasex,
     ae_int_t tilex0,
     ae_int_t tilex1,
     ae_int_t tilescountx,
     ae_int_t basecasey,
     ae_int_t tiley0,
     ae_int_t tiley1,
     ae_int_t tilescounty,
     ae_int_t maxcoresize,
     ae_int_t interfacesize,
     ae_int_t lsqrcnt,
     double lambdareg,
     const spline1dinterpolant* basis1,
     ae_shared_pool* pool,
     spline2dinterpolant* spline,
     ae_state *_state);
void _spawn_spline2d_fastddmfitlayer(/* Real    */ const ae_vector* xy,
    ae_int_t d,
    ae_int_t scalexy,
    /* Integer */ const ae_vector* xyindex,
    ae_int_t basecasex,
    ae_int_t tilex0,
    ae_int_t tilex1,
    ae_int_t tilescountx,
    ae_int_t basecasey,
    ae_int_t tiley0,
    ae_int_t tiley1,
    ae_int_t tilescounty,
    ae_int_t maxcoresize,
    ae_int_t interfacesize,
    ae_int_t lsqrcnt,
    double lambdareg,
    const spline1dinterpolant* basis1,
    ae_shared_pool* pool,
    spline2dinterpolant* spline, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_spline2d_fastddmfitlayer(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_spline2d_fastddmfitlayer(/* Real    */ const ae_vector* xy,
    ae_int_t d,
    ae_int_t scalexy,
    /* Integer */ const ae_vector* xyindex,
    ae_int_t basecasex,
    ae_int_t tilex0,
    ae_int_t tilex1,
    ae_int_t tilescountx,
    ae_int_t basecasey,
    ae_int_t tiley0,
    ae_int_t tiley1,
    ae_int_t tilescounty,
    ae_int_t maxcoresize,
    ae_int_t interfacesize,
    ae_int_t lsqrcnt,
    double lambdareg,
    const spline1dinterpolant* basis1,
    ae_shared_pool* pool,
    spline2dinterpolant* spline, ae_state *_state);
static void spline2d_blockllsfit(spline2dxdesignmatrix* xdesign,
     ae_int_t lsqrcnt,
     /* Real    */ ae_vector* z,
     spline2dfitreport* rep,
     double tss,
     spline2dblockllsbuf* buf,
     ae_state *_state);
static void spline2d_naivellsfit(const sparsematrix* av,
     const sparsematrix* ah,
     ae_int_t arows,
     /* Real    */ ae_vector* xy,
     ae_int_t kx,
     ae_int_t ky,
     ae_int_t npoints,
     ae_int_t d,
     ae_int_t lsqrcnt,
     /* Real    */ ae_vector* z,
     spline2dfitreport* rep,
     double tss,
     ae_state *_state);
static ae_int_t spline2d_getcelloffset(ae_int_t kx,
     ae_int_t ky,
     ae_int_t blockbandwidth,
     ae_int_t i,
     ae_int_t j,
     ae_state *_state);
static void spline2d_copycellto(ae_int_t kx,
     ae_int_t ky,
     ae_int_t blockbandwidth,
     /* Real    */ const ae_matrix* blockata,
     ae_int_t i,
     ae_int_t j,
     /* Real    */ ae_matrix* dst,
     ae_int_t dst0,
     ae_int_t dst1,
     ae_state *_state);
static void spline2d_flushtozerocell(ae_int_t kx,
     ae_int_t ky,
     ae_int_t blockbandwidth,
     /* Real    */ ae_matrix* blockata,
     ae_int_t i,
     ae_int_t j,
     double eps,
     ae_state *_state);
static void spline2d_blockllsgenerateata(const sparsematrix* ah,
     ae_int_t ky0,
     ae_int_t ky1,
     ae_int_t kx,
     ae_int_t ky,
     /* Real    */ ae_matrix* blockata,
     sreal* mxata,
     ae_state *_state);
void _spawn_spline2d_blockllsgenerateata(const sparsematrix* ah,
    ae_int_t ky0,
    ae_int_t ky1,
    ae_int_t kx,
    ae_int_t ky,
    /* Real    */ ae_matrix* blockata,
    sreal* mxata, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_spline2d_blockllsgenerateata(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_spline2d_blockllsgenerateata(const sparsematrix* ah,
    ae_int_t ky0,
    ae_int_t ky1,
    ae_int_t kx,
    ae_int_t ky,
    /* Real    */ ae_matrix* blockata,
    sreal* mxata, ae_state *_state);
static ae_bool spline2d_blockllscholesky(/* Real    */ ae_matrix* blockata,
     ae_int_t kx,
     ae_int_t ky,
     /* Real    */ ae_matrix* trsmbuf2,
     /* Real    */ ae_matrix* cholbuf2,
     /* Real    */ ae_vector* cholbuf1,
     ae_state *_state);
static void spline2d_blockllstrsv(/* Real    */ const ae_matrix* blockata,
     ae_int_t kx,
     ae_int_t ky,
     ae_bool transu,
     /* Real    */ ae_vector* b,
     ae_state *_state);
static void spline2d_computeresidualsfromscratch(/* Real    */ ae_vector* xy,
     /* Real    */ const ae_vector* yraw,
     ae_int_t npoints,
     ae_int_t d,
     ae_int_t scalexy,
     const spline2dinterpolant* spline,
     ae_state *_state);
void _spawn_spline2d_computeresidualsfromscratch(/* Real    */ ae_vector* xy,
    /* Real    */ const ae_vector* yraw,
    ae_int_t npoints,
    ae_int_t d,
    ae_int_t scalexy,
    const spline2dinterpolant* spline, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_spline2d_computeresidualsfromscratch(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_spline2d_computeresidualsfromscratch(/* Real    */ ae_vector* xy,
    /* Real    */ const ae_vector* yraw,
    ae_int_t npoints,
    ae_int_t d,
    ae_int_t scalexy,
    const spline2dinterpolant* spline, ae_state *_state);
static void spline2d_computeresidualsfromscratchrec(/* Real    */ ae_vector* xy,
     /* Real    */ const ae_vector* yraw,
     ae_int_t pt0,
     ae_int_t pt1,
     ae_int_t chunksize,
     ae_int_t d,
     ae_int_t scalexy,
     const spline2dinterpolant* spline,
     ae_shared_pool* pool,
     ae_state *_state);
void _spawn_spline2d_computeresidualsfromscratchrec(/* Real    */ ae_vector* xy,
    /* Real    */ const ae_vector* yraw,
    ae_int_t pt0,
    ae_int_t pt1,
    ae_int_t chunksize,
    ae_int_t d,
    ae_int_t scalexy,
    const spline2dinterpolant* spline,
    ae_shared_pool* pool, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_spline2d_computeresidualsfromscratchrec(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_spline2d_computeresidualsfromscratchrec(/* Real    */ ae_vector* xy,
    /* Real    */ const ae_vector* yraw,
    ae_int_t pt0,
    ae_int_t pt1,
    ae_int_t chunksize,
    ae_int_t d,
    ae_int_t scalexy,
    const spline2dinterpolant* spline,
    ae_shared_pool* pool, ae_state *_state);
static void spline2d_reorderdatasetandbuildindex(/* Real    */ ae_vector* xy,
     ae_int_t npoints,
     ae_int_t d,
     /* Real    */ ae_vector* shadow,
     ae_int_t ns,
     ae_int_t kx,
     ae_int_t ky,
     /* Integer */ ae_vector* xyindex,
     /* Integer */ ae_vector* bufi,
     ae_state *_state);
static void spline2d_rescaledatasetandrefineindex(/* Real    */ ae_vector* xy,
     ae_int_t npoints,
     ae_int_t d,
     /* Real    */ ae_vector* shadow,
     ae_int_t ns,
     ae_int_t kx,
     ae_int_t ky,
     /* Integer */ ae_vector* xyindex,
     /* Integer */ ae_vector* bufi,
     ae_state *_state);
static void spline2d_expandindexrows(/* Real    */ ae_vector* xy,
     ae_int_t d,
     /* Real    */ ae_vector* shadow,
     ae_int_t ns,
     /* Integer */ ae_vector* cidx,
     ae_int_t pt0,
     ae_int_t pt1,
     /* Integer */ ae_vector* xyindexprev,
     ae_int_t row0,
     ae_int_t row1,
     /* Integer */ ae_vector* xyindexnew,
     ae_int_t kxnew,
     ae_int_t kynew,
     ae_bool rootcall,
     ae_state *_state);
void _spawn_spline2d_expandindexrows(/* Real    */ ae_vector* xy,
    ae_int_t d,
    /* Real    */ ae_vector* shadow,
    ae_int_t ns,
    /* Integer */ ae_vector* cidx,
    ae_int_t pt0,
    ae_int_t pt1,
    /* Integer */ ae_vector* xyindexprev,
    ae_int_t row0,
    ae_int_t row1,
    /* Integer */ ae_vector* xyindexnew,
    ae_int_t kxnew,
    ae_int_t kynew,
    ae_bool rootcall, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_spline2d_expandindexrows(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_spline2d_expandindexrows(/* Real    */ ae_vector* xy,
    ae_int_t d,
    /* Real    */ ae_vector* shadow,
    ae_int_t ns,
    /* Integer */ ae_vector* cidx,
    ae_int_t pt0,
    ae_int_t pt1,
    /* Integer */ ae_vector* xyindexprev,
    ae_int_t row0,
    ae_int_t row1,
    /* Integer */ ae_vector* xyindexnew,
    ae_int_t kxnew,
    ae_int_t kynew,
    ae_bool rootcall, ae_state *_state);
static void spline2d_reorderdatasetandbuildindexrec(/* Real    */ ae_vector* xy,
     ae_int_t d,
     /* Real    */ ae_vector* shadow,
     ae_int_t ns,
     /* Integer */ ae_vector* cidx,
     ae_int_t pt0,
     ae_int_t pt1,
     /* Integer */ ae_vector* xyindex,
     ae_int_t idx0,
     ae_int_t idx1,
     ae_bool rootcall,
     ae_state *_state);
void _spawn_spline2d_reorderdatasetandbuildindexrec(/* Real    */ ae_vector* xy,
    ae_int_t d,
    /* Real    */ ae_vector* shadow,
    ae_int_t ns,
    /* Integer */ ae_vector* cidx,
    ae_int_t pt0,
    ae_int_t pt1,
    /* Integer */ ae_vector* xyindex,
    ae_int_t idx0,
    ae_int_t idx1,
    ae_bool rootcall, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_spline2d_reorderdatasetandbuildindexrec(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_spline2d_reorderdatasetandbuildindexrec(/* Real    */ ae_vector* xy,
    ae_int_t d,
    /* Real    */ ae_vector* shadow,
    ae_int_t ns,
    /* Integer */ ae_vector* cidx,
    ae_int_t pt0,
    ae_int_t pt1,
    /* Integer */ ae_vector* xyindex,
    ae_int_t idx0,
    ae_int_t idx1,
    ae_bool rootcall, ae_state *_state);
static void spline2d_xdesigngenerate(/* Real    */ const ae_vector* xy,
     /* Integer */ const ae_vector* xyindex,
     ae_int_t kx0,
     ae_int_t kx1,
     ae_int_t kxtotal,
     ae_int_t ky0,
     ae_int_t ky1,
     ae_int_t kytotal,
     ae_int_t d,
     double lambdareg,
     double lambdans,
     const spline1dinterpolant* basis1,
     spline2dxdesignmatrix* a,
     ae_state *_state);
static void spline2d_xdesignmv(spline2dxdesignmatrix* a,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
static void spline2d_xdesignmtv(spline2dxdesignmatrix* a,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
static void spline2d_xdesignblockata(spline2dxdesignmatrix* a,
     /* Real    */ ae_matrix* blockata,
     double* mxata,
     ae_state *_state);
static ae_bool spline2d_adjustevaluationinterval(const spline2dinterpolant* s,
     double* x,
     double* t,
     double* dt,
     ae_int_t* ix,
     double* y,
     double* u,
     double* du,
     ae_int_t* iy,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
This subroutine calculates the value of the bilinear or bicubic spline  at
the given point X.

Input parameters:
    C   -   2D spline object.
            Built by spline2dbuildbilinearv or spline2dbuildbicubicv.
    X, Y-   point

Result:
    S(x,y)

  -- ALGLIB PROJECT --
     Copyright 05.07.2007 by Bochkanov Sergey
*************************************************************************/
double spline2dcalc(const spline2dinterpolant* c,
     double x,
     double y,
     ae_state *_state)
{
    ae_int_t ix;
    ae_int_t iy;
    ae_int_t l;
    ae_int_t r;
    ae_int_t h;
    double t;
    double dt;
    double u;
    double du;
    double y1;
    double y2;
    double y3;
    double y4;
    ae_int_t s1;
    ae_int_t s2;
    ae_int_t s3;
    ae_int_t s4;
    ae_int_t sfx;
    ae_int_t sfy;
    ae_int_t sfxy;
    double t2;
    double t3;
    double u2;
    double u3;
    double ht00;
    double ht01;
    double ht10;
    double ht11;
    double hu00;
    double hu01;
    double hu10;
    double hu11;
    double result;


    ae_assert(c->stype==-1||c->stype==-3, "Spline2DCalc: incorrect C (incorrect parameter C.SType)", _state);
    ae_assert(ae_isfinite(x, _state)&&ae_isfinite(y, _state), "Spline2DCalc: X or Y contains NaN or Infinite value", _state);
    if( c->d!=1 )
    {
        result = (double)(0);
        return result;
    }
    
    /*
     * Determine evaluation interval
     */
    l = 0;
    r = c->n-1;
    while(l!=r-1)
    {
        h = (l+r)/2;
        if( ae_fp_greater_eq(c->x.ptr.p_double[h],x) )
        {
            r = h;
        }
        else
        {
            l = h;
        }
    }
    dt = 1.0/(c->x.ptr.p_double[l+1]-c->x.ptr.p_double[l]);
    t = (x-c->x.ptr.p_double[l])*dt;
    ix = l;
    l = 0;
    r = c->m-1;
    while(l!=r-1)
    {
        h = (l+r)/2;
        if( ae_fp_greater_eq(c->y.ptr.p_double[h],y) )
        {
            r = h;
        }
        else
        {
            l = h;
        }
    }
    du = 1.0/(c->y.ptr.p_double[l+1]-c->y.ptr.p_double[l]);
    u = (y-c->y.ptr.p_double[l])*du;
    iy = l;
    
    /*
     * Handle possible missing cells
     */
    if( c->hasmissingcells&&!spline2d_adjustevaluationinterval(c, &x, &t, &dt, &ix, &y, &u, &du, &iy, _state) )
    {
        result = _state->v_nan;
        return result;
    }
    
    /*
     * Bilinear interpolation
     */
    if( c->stype==-1 )
    {
        y1 = c->f.ptr.p_double[c->n*iy+ix];
        y2 = c->f.ptr.p_double[c->n*iy+(ix+1)];
        y3 = c->f.ptr.p_double[c->n*(iy+1)+(ix+1)];
        y4 = c->f.ptr.p_double[c->n*(iy+1)+ix];
        result = ((double)1-t)*((double)1-u)*y1+t*((double)1-u)*y2+t*u*y3+((double)1-t)*u*y4;
        return result;
    }
    
    /*
     * Bicubic interpolation:
     * * calculate Hermite basis for dimensions X and Y (variables T and U),
     *   here HTij means basis function whose I-th derivative has value 1 at T=J.
     *   Same for HUij.
     * * after initial calculation, apply scaling by DT/DU to the basis
     * * calculate using stored table of second derivatives
     */
    ae_assert(c->stype==-3, "Spline2DCalc: integrity check failed", _state);
    sfx = c->n*c->m;
    sfy = 2*c->n*c->m;
    sfxy = 3*c->n*c->m;
    s1 = c->n*iy+ix;
    s2 = c->n*iy+(ix+1);
    s3 = c->n*(iy+1)+ix;
    s4 = c->n*(iy+1)+(ix+1);
    t2 = t*t;
    t3 = t*t2;
    u2 = u*u;
    u3 = u*u2;
    ht00 = (double)2*t3-(double)3*t2+(double)1;
    ht10 = t3-(double)2*t2+t;
    ht01 = -(double)2*t3+(double)3*t2;
    ht11 = t3-t2;
    hu00 = (double)2*u3-(double)3*u2+(double)1;
    hu10 = u3-(double)2*u2+u;
    hu01 = -(double)2*u3+(double)3*u2;
    hu11 = u3-u2;
    ht10 = ht10/dt;
    ht11 = ht11/dt;
    hu10 = hu10/du;
    hu11 = hu11/du;
    result = (double)(0);
    result = result+c->f.ptr.p_double[s1]*ht00*hu00+c->f.ptr.p_double[s2]*ht01*hu00+c->f.ptr.p_double[s3]*ht00*hu01+c->f.ptr.p_double[s4]*ht01*hu01;
    result = result+c->f.ptr.p_double[sfx+s1]*ht10*hu00+c->f.ptr.p_double[sfx+s2]*ht11*hu00+c->f.ptr.p_double[sfx+s3]*ht10*hu01+c->f.ptr.p_double[sfx+s4]*ht11*hu01;
    result = result+c->f.ptr.p_double[sfy+s1]*ht00*hu10+c->f.ptr.p_double[sfy+s2]*ht01*hu10+c->f.ptr.p_double[sfy+s3]*ht00*hu11+c->f.ptr.p_double[sfy+s4]*ht01*hu11;
    result = result+c->f.ptr.p_double[sfxy+s1]*ht10*hu10+c->f.ptr.p_double[sfxy+s2]*ht11*hu10+c->f.ptr.p_double[sfxy+s3]*ht10*hu11+c->f.ptr.p_double[sfxy+s4]*ht11*hu11;
    return result;
}


/*************************************************************************
This subroutine calculates the value of a bilinear or bicubic spline   and
its derivatives.

Use Spline2DDiff2() if you need second derivatives Sxx and Syy.

Input parameters:
    C   -   spline interpolant.
    X, Y-   point

Output parameters:
    F   -   S(x,y)
    FX  -   dS(x,y)/dX
    FY  -   dS(x,y)/dY

  -- ALGLIB PROJECT --
     Copyright 05.07.2007 by Bochkanov Sergey
*************************************************************************/
void spline2ddiff(const spline2dinterpolant* c,
     double x,
     double y,
     double* f,
     double* fx,
     double* fy,
     ae_state *_state)
{
    double t;
    double dt;
    double u;
    double du;
    ae_int_t ix;
    ae_int_t iy;
    ae_int_t l;
    ae_int_t r;
    ae_int_t h;
    ae_int_t s1;
    ae_int_t s2;
    ae_int_t s3;
    ae_int_t s4;
    ae_int_t sfx;
    ae_int_t sfy;
    ae_int_t sfxy;
    double y1;
    double y2;
    double y3;
    double y4;
    double v0;
    double v1;
    double v2;
    double v3;
    double t2;
    double t3;
    double u2;
    double u3;
    double ht00;
    double ht01;
    double ht10;
    double ht11;
    double hu00;
    double hu01;
    double hu10;
    double hu11;
    double dht00;
    double dht01;
    double dht10;
    double dht11;
    double dhu00;
    double dhu01;
    double dhu10;
    double dhu11;

    *f = 0.0;
    *fx = 0.0;
    *fy = 0.0;

    ae_assert(c->stype==-1||c->stype==-3, "Spline2DDiff: incorrect C (incorrect parameter C.SType)", _state);
    ae_assert(ae_isfinite(x, _state)&&ae_isfinite(y, _state), "Spline2DDiff: X or Y contains NaN or Infinite value", _state);
    
    /*
     * Prepare F, dF/dX, dF/dY, d2F/dXdY
     */
    *f = (double)(0);
    *fx = (double)(0);
    *fy = (double)(0);
    if( c->d!=1 )
    {
        return;
    }
    
    /*
     * Binary search in the [ x[0], ..., x[n-2] ] (x[n-1] is not included)
     */
    l = 0;
    r = c->n-1;
    while(l!=r-1)
    {
        h = (l+r)/2;
        if( ae_fp_greater_eq(c->x.ptr.p_double[h],x) )
        {
            r = h;
        }
        else
        {
            l = h;
        }
    }
    t = (x-c->x.ptr.p_double[l])/(c->x.ptr.p_double[l+1]-c->x.ptr.p_double[l]);
    dt = 1.0/(c->x.ptr.p_double[l+1]-c->x.ptr.p_double[l]);
    ix = l;
    
    /*
     * Binary search in the [ y[0], ..., y[m-2] ] (y[m-1] is not included)
     */
    l = 0;
    r = c->m-1;
    while(l!=r-1)
    {
        h = (l+r)/2;
        if( ae_fp_greater_eq(c->y.ptr.p_double[h],y) )
        {
            r = h;
        }
        else
        {
            l = h;
        }
    }
    u = (y-c->y.ptr.p_double[l])/(c->y.ptr.p_double[l+1]-c->y.ptr.p_double[l]);
    du = 1.0/(c->y.ptr.p_double[l+1]-c->y.ptr.p_double[l]);
    iy = l;
    
    /*
     * Handle possible missing cells
     */
    if( c->hasmissingcells&&!spline2d_adjustevaluationinterval(c, &x, &t, &dt, &ix, &y, &u, &du, &iy, _state) )
    {
        *f = _state->v_nan;
        *fx = _state->v_nan;
        *fy = _state->v_nan;
        return;
    }
    
    /*
     * Bilinear interpolation
     */
    if( c->stype==-1 )
    {
        y1 = c->f.ptr.p_double[c->n*iy+ix];
        y2 = c->f.ptr.p_double[c->n*iy+(ix+1)];
        y3 = c->f.ptr.p_double[c->n*(iy+1)+(ix+1)];
        y4 = c->f.ptr.p_double[c->n*(iy+1)+ix];
        *f = ((double)1-t)*((double)1-u)*y1+t*((double)1-u)*y2+t*u*y3+((double)1-t)*u*y4;
        *fx = (-((double)1-u)*y1+((double)1-u)*y2+u*y3-u*y4)*dt;
        *fy = (-((double)1-t)*y1-t*y2+t*y3+((double)1-t)*y4)*du;
        return;
    }
    
    /*
     * Bicubic interpolation
     */
    if( c->stype==-3 )
    {
        sfx = c->n*c->m;
        sfy = 2*c->n*c->m;
        sfxy = 3*c->n*c->m;
        s1 = c->n*iy+ix;
        s2 = c->n*iy+(ix+1);
        s3 = c->n*(iy+1)+ix;
        s4 = c->n*(iy+1)+(ix+1);
        t2 = t*t;
        t3 = t*t2;
        u2 = u*u;
        u3 = u*u2;
        ht00 = (double)2*t3-(double)3*t2+(double)1;
        ht10 = t3-(double)2*t2+t;
        ht01 = -(double)2*t3+(double)3*t2;
        ht11 = t3-t2;
        hu00 = (double)2*u3-(double)3*u2+(double)1;
        hu10 = u3-(double)2*u2+u;
        hu01 = -(double)2*u3+(double)3*u2;
        hu11 = u3-u2;
        ht10 = ht10/dt;
        ht11 = ht11/dt;
        hu10 = hu10/du;
        hu11 = hu11/du;
        dht00 = (double)6*t2-(double)6*t;
        dht10 = (double)3*t2-(double)4*t+(double)1;
        dht01 = -(double)6*t2+(double)6*t;
        dht11 = (double)3*t2-(double)2*t;
        dhu00 = (double)6*u2-(double)6*u;
        dhu10 = (double)3*u2-(double)4*u+(double)1;
        dhu01 = -(double)6*u2+(double)6*u;
        dhu11 = (double)3*u2-(double)2*u;
        dht00 = dht00*dt;
        dht01 = dht01*dt;
        dhu00 = dhu00*du;
        dhu01 = dhu01*du;
        *f = (double)(0);
        *fx = (double)(0);
        *fy = (double)(0);
        v0 = c->f.ptr.p_double[s1];
        v1 = c->f.ptr.p_double[s2];
        v2 = c->f.ptr.p_double[s3];
        v3 = c->f.ptr.p_double[s4];
        *f = *f+v0*ht00*hu00+v1*ht01*hu00+v2*ht00*hu01+v3*ht01*hu01;
        *fx = *fx+v0*dht00*hu00+v1*dht01*hu00+v2*dht00*hu01+v3*dht01*hu01;
        *fy = *fy+v0*ht00*dhu00+v1*ht01*dhu00+v2*ht00*dhu01+v3*ht01*dhu01;
        v0 = c->f.ptr.p_double[sfx+s1];
        v1 = c->f.ptr.p_double[sfx+s2];
        v2 = c->f.ptr.p_double[sfx+s3];
        v3 = c->f.ptr.p_double[sfx+s4];
        *f = *f+v0*ht10*hu00+v1*ht11*hu00+v2*ht10*hu01+v3*ht11*hu01;
        *fx = *fx+v0*dht10*hu00+v1*dht11*hu00+v2*dht10*hu01+v3*dht11*hu01;
        *fy = *fy+v0*ht10*dhu00+v1*ht11*dhu00+v2*ht10*dhu01+v3*ht11*dhu01;
        v0 = c->f.ptr.p_double[sfy+s1];
        v1 = c->f.ptr.p_double[sfy+s2];
        v2 = c->f.ptr.p_double[sfy+s3];
        v3 = c->f.ptr.p_double[sfy+s4];
        *f = *f+v0*ht00*hu10+v1*ht01*hu10+v2*ht00*hu11+v3*ht01*hu11;
        *fx = *fx+v0*dht00*hu10+v1*dht01*hu10+v2*dht00*hu11+v3*dht01*hu11;
        *fy = *fy+v0*ht00*dhu10+v1*ht01*dhu10+v2*ht00*dhu11+v3*ht01*dhu11;
        v0 = c->f.ptr.p_double[sfxy+s1];
        v1 = c->f.ptr.p_double[sfxy+s2];
        v2 = c->f.ptr.p_double[sfxy+s3];
        v3 = c->f.ptr.p_double[sfxy+s4];
        *f = *f+v0*ht10*hu10+v1*ht11*hu10+v2*ht10*hu11+v3*ht11*hu11;
        *fx = *fx+v0*dht10*hu10+v1*dht11*hu10+v2*dht10*hu11+v3*dht11*hu11;
        *fy = *fy+v0*ht10*dhu10+v1*ht11*dhu10+v2*ht10*dhu11+v3*ht11*dhu11;
        return;
    }
}


/*************************************************************************
This subroutine calculates the value of a bilinear or bicubic spline   and
its second derivatives.

Input parameters:
    C   -   spline interpolant.
    X, Y-   point

Output parameters:
    F   -   S(x,y)
    FX  -   dS(x,y)/dX
    FY  -   dS(x,y)/dY
    FXX -   d2S(x,y)/dXdX
    FXY -   d2S(x,y)/dXdY
    FYY -   d2S(x,y)/dYdY

  -- ALGLIB PROJECT --
     Copyright 17.04.2023 by Bochkanov Sergey.
     
     The second derivatives code was contributed by  Horst  Greiner  under
     public domain terms.
*************************************************************************/
void spline2ddiff2(const spline2dinterpolant* c,
     double x,
     double y,
     double* f,
     double* fx,
     double* fy,
     double* fxx,
     double* fxy,
     double* fyy,
     ae_state *_state)
{
    double t;
    double dt;
    double u;
    double du;
    ae_int_t ix;
    ae_int_t iy;
    ae_int_t l;
    ae_int_t r;
    ae_int_t h;
    ae_int_t s1;
    ae_int_t s2;
    ae_int_t s3;
    ae_int_t s4;
    ae_int_t sfx;
    ae_int_t sfy;
    ae_int_t sfxy;
    double y1;
    double y2;
    double y3;
    double y4;
    double v0;
    double v1;
    double v2;
    double v3;
    double t2;
    double t3;
    double u2;
    double u3;
    double ht00;
    double ht01;
    double ht10;
    double ht11;
    double hu00;
    double hu01;
    double hu10;
    double hu11;
    double dht00;
    double dht01;
    double dht10;
    double dht11;
    double dhu00;
    double dhu01;
    double dhu10;
    double dhu11;
    double d2ht00;
    double d2ht01;
    double d2ht10;
    double d2ht11;
    double d2hu00;
    double d2hu01;
    double d2hu10;
    double d2hu11;

    *f = 0.0;
    *fx = 0.0;
    *fy = 0.0;
    *fxx = 0.0;
    *fxy = 0.0;
    *fyy = 0.0;

    ae_assert(c->stype==-1||c->stype==-3, "Spline2DDiff: incorrect C (incorrect parameter C.SType)", _state);
    ae_assert(ae_isfinite(x, _state)&&ae_isfinite(y, _state), "Spline2DDiff: X or Y contains NaN or Infinite value", _state);
    
    /*
     * Prepare F, dF/dX, dF/dY, d2F/dXdY
     */
    *f = (double)(0);
    *fx = (double)(0);
    *fy = (double)(0);
    *fxx = (double)(0);
    *fxy = (double)(0);
    *fyy = (double)(0);
    if( c->d!=1 )
    {
        return;
    }
    
    /*
     * Binary search in the [ x[0], ..., x[n-2] ] (x[n-1] is not included)
     */
    l = 0;
    r = c->n-1;
    while(l!=r-1)
    {
        h = (l+r)/2;
        if( ae_fp_greater_eq(c->x.ptr.p_double[h],x) )
        {
            r = h;
        }
        else
        {
            l = h;
        }
    }
    t = (x-c->x.ptr.p_double[l])/(c->x.ptr.p_double[l+1]-c->x.ptr.p_double[l]);
    dt = 1.0/(c->x.ptr.p_double[l+1]-c->x.ptr.p_double[l]);
    ix = l;
    
    /*
     * Binary search in the [ y[0], ..., y[m-2] ] (y[m-1] is not included)
     */
    l = 0;
    r = c->m-1;
    while(l!=r-1)
    {
        h = (l+r)/2;
        if( ae_fp_greater_eq(c->y.ptr.p_double[h],y) )
        {
            r = h;
        }
        else
        {
            l = h;
        }
    }
    u = (y-c->y.ptr.p_double[l])/(c->y.ptr.p_double[l+1]-c->y.ptr.p_double[l]);
    du = 1.0/(c->y.ptr.p_double[l+1]-c->y.ptr.p_double[l]);
    iy = l;
    
    /*
     * Handle possible missing cells
     */
    if( c->hasmissingcells&&!spline2d_adjustevaluationinterval(c, &x, &t, &dt, &ix, &y, &u, &du, &iy, _state) )
    {
        *f = _state->v_nan;
        *fx = _state->v_nan;
        *fy = _state->v_nan;
        *fxx = _state->v_nan;
        *fxy = _state->v_nan;
        *fyy = _state->v_nan;
        return;
    }
    
    /*
     * Bilinear interpolation
     */
    if( c->stype==-1 )
    {
        y1 = c->f.ptr.p_double[c->n*iy+ix];
        y2 = c->f.ptr.p_double[c->n*iy+(ix+1)];
        y3 = c->f.ptr.p_double[c->n*(iy+1)+(ix+1)];
        y4 = c->f.ptr.p_double[c->n*(iy+1)+ix];
        *f = ((double)1-t)*((double)1-u)*y1+t*((double)1-u)*y2+t*u*y3+((double)1-t)*u*y4;
        *fx = (-((double)1-u)*y1+((double)1-u)*y2+u*y3-u*y4)*dt;
        *fy = (-((double)1-t)*y1-t*y2+t*y3+((double)1-t)*y4)*du;
        *fxx = (double)(0);
        *fxy = (y1-y2+y3-y4)*du*dt;
        *fyy = (double)(0);
        return;
    }
    
    /*
     * Bicubic interpolation
     */
    if( c->stype==-3 )
    {
        sfx = c->n*c->m;
        sfy = 2*c->n*c->m;
        sfxy = 3*c->n*c->m;
        s1 = c->n*iy+ix;
        s2 = c->n*iy+(ix+1);
        s3 = c->n*(iy+1)+ix;
        s4 = c->n*(iy+1)+(ix+1);
        t2 = t*t;
        t3 = t*t2;
        u2 = u*u;
        u3 = u*u2;
        ht00 = (double)2*t3-(double)3*t2+(double)1;
        ht10 = t3-(double)2*t2+t;
        ht01 = -(double)2*t3+(double)3*t2;
        ht11 = t3-t2;
        hu00 = (double)2*u3-(double)3*u2+(double)1;
        hu10 = u3-(double)2*u2+u;
        hu01 = -(double)2*u3+(double)3*u2;
        hu11 = u3-u2;
        ht10 = ht10/dt;
        ht11 = ht11/dt;
        hu10 = hu10/du;
        hu11 = hu11/du;
        dht00 = (double)6*t2-(double)6*t;
        dht10 = (double)3*t2-(double)4*t+(double)1;
        dht01 = -(double)6*t2+(double)6*t;
        dht11 = (double)3*t2-(double)2*t;
        dhu00 = (double)6*u2-(double)6*u;
        dhu10 = (double)3*u2-(double)4*u+(double)1;
        dhu01 = -(double)6*u2+(double)6*u;
        dhu11 = (double)3*u2-(double)2*u;
        dht00 = dht00*dt;
        dht01 = dht01*dt;
        dhu00 = dhu00*du;
        dhu01 = dhu01*du;
        d2ht00 = ((double)12*t-(double)6)*dt*dt;
        d2ht01 = (-(double)12*t+(double)6)*dt*dt;
        d2ht10 = ((double)6*t-(double)4)*dt;
        d2ht11 = ((double)6*t-(double)2)*dt;
        d2hu00 = ((double)12*u-(double)6)*du*du;
        d2hu01 = (-(double)12*u+(double)6)*du*du;
        d2hu10 = ((double)6*u-(double)4)*du;
        d2hu11 = ((double)6*u-(double)2)*du;
        *f = (double)(0);
        *fx = (double)(0);
        *fy = (double)(0);
        *fxy = (double)(0);
        v0 = c->f.ptr.p_double[s1];
        v1 = c->f.ptr.p_double[s2];
        v2 = c->f.ptr.p_double[s3];
        v3 = c->f.ptr.p_double[s4];
        *f = *f+v0*ht00*hu00+v1*ht01*hu00+v2*ht00*hu01+v3*ht01*hu01;
        *fx = *fx+v0*dht00*hu00+v1*dht01*hu00+v2*dht00*hu01+v3*dht01*hu01;
        *fy = *fy+v0*ht00*dhu00+v1*ht01*dhu00+v2*ht00*dhu01+v3*ht01*dhu01;
        *fxx = *fxx+v0*d2ht00*hu00+v1*d2ht01*hu00+v2*d2ht00*hu01+v3*d2ht01*hu01;
        *fxy = *fxy+v0*dht00*dhu00+v1*dht01*dhu00+v2*dht00*dhu01+v3*dht01*dhu01;
        *fyy = *fyy+v0*ht00*d2hu00+v1*ht01*d2hu00+v2*ht00*d2hu01+v3*ht01*d2hu01;
        v0 = c->f.ptr.p_double[sfx+s1];
        v1 = c->f.ptr.p_double[sfx+s2];
        v2 = c->f.ptr.p_double[sfx+s3];
        v3 = c->f.ptr.p_double[sfx+s4];
        *f = *f+v0*ht10*hu00+v1*ht11*hu00+v2*ht10*hu01+v3*ht11*hu01;
        *fx = *fx+v0*dht10*hu00+v1*dht11*hu00+v2*dht10*hu01+v3*dht11*hu01;
        *fy = *fy+v0*ht10*dhu00+v1*ht11*dhu00+v2*ht10*dhu01+v3*ht11*dhu01;
        *fxx = *fxx+v0*d2ht10*hu00+v1*d2ht11*hu00+v2*d2ht10*hu01+v3*d2ht11*hu01;
        *fxy = *fxy+v0*dht10*dhu00+v1*dht11*dhu00+v2*dht10*dhu01+v3*dht11*dhu01;
        *fyy = *fyy+v0*ht10*d2hu00+v1*ht11*d2hu00+v2*ht10*d2hu01+v3*ht11*d2hu01;
        v0 = c->f.ptr.p_double[sfy+s1];
        v1 = c->f.ptr.p_double[sfy+s2];
        v2 = c->f.ptr.p_double[sfy+s3];
        v3 = c->f.ptr.p_double[sfy+s4];
        *f = *f+v0*ht00*hu10+v1*ht01*hu10+v2*ht00*hu11+v3*ht01*hu11;
        *fx = *fx+v0*dht00*hu10+v1*dht01*hu10+v2*dht00*hu11+v3*dht01*hu11;
        *fy = *fy+v0*ht00*dhu10+v1*ht01*dhu10+v2*ht00*dhu11+v3*ht01*dhu11;
        *fxx = *fxx+v0*d2ht00*hu10+v1*d2ht01*hu10+v2*d2ht00*hu11+v3*d2ht01*hu11;
        *fxy = *fxy+v0*dht00*dhu10+v1*dht01*dhu10+v2*dht00*dhu11+v3*dht01*dhu11;
        *fyy = *fyy+v0*ht00*d2hu10+v1*ht01*d2hu10+v2*ht00*d2hu11+v3*ht01*d2hu11;
        v0 = c->f.ptr.p_double[sfxy+s1];
        v1 = c->f.ptr.p_double[sfxy+s2];
        v2 = c->f.ptr.p_double[sfxy+s3];
        v3 = c->f.ptr.p_double[sfxy+s4];
        *f = *f+v0*ht10*hu10+v1*ht11*hu10+v2*ht10*hu11+v3*ht11*hu11;
        *fx = *fx+v0*dht10*hu10+v1*dht11*hu10+v2*dht10*hu11+v3*dht11*hu11;
        *fy = *fy+v0*ht10*dhu10+v1*ht11*dhu10+v2*ht10*dhu11+v3*ht11*dhu11;
        *fxx = *fxx+v0*d2ht10*hu10+v1*d2ht11*hu10+v2*d2ht10*hu11+v3*d2ht11*hu11;
        *fxy = *fxy+v0*dht10*dhu10+v1*dht11*dhu10+v2*dht10*dhu11+v3*dht11*dhu11;
        *fyy = *fyy+v0*ht10*d2hu10+v1*ht11*d2hu10+v2*ht10*d2hu11+v3*ht11*d2hu11;
        return;
    }
}


/*************************************************************************
This subroutine calculates bilinear or bicubic vector-valued spline at the
given point (X,Y).

If you need just some specific component of vector-valued spline, you  can
use spline2dcalcvi() function.

INPUT PARAMETERS:
    C   -   spline interpolant.
    X, Y-   point
    F   -   output buffer, possibly preallocated array. In case array size
            is large enough to store result, it is not reallocated.  Array
            which is too short will be reallocated

OUTPUT PARAMETERS:
    F   -   array[D] (or larger) which stores function values

  -- ALGLIB PROJECT --
     Copyright 01.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dcalcvbuf(const spline2dinterpolant* c,
     double x,
     double y,
     /* Real    */ ae_vector* f,
     ae_state *_state)
{
    ae_int_t ix;
    ae_int_t iy;
    ae_int_t l;
    ae_int_t r;
    ae_int_t h;
    ae_int_t i;
    double t;
    double dt;
    double u;
    double du;
    double y1;
    double y2;
    double y3;
    double y4;
    ae_int_t s1;
    ae_int_t s2;
    ae_int_t s3;
    ae_int_t s4;
    ae_int_t sfx;
    ae_int_t sfy;
    ae_int_t sfxy;
    double t2;
    double t3;
    double u2;
    double u3;
    double ht00;
    double ht01;
    double ht10;
    double ht11;
    double hu00;
    double hu01;
    double hu10;
    double hu11;


    ae_assert(c->stype==-1||c->stype==-3, "Spline2DCalcVBuf: incorrect C (incorrect parameter C.SType)", _state);
    ae_assert(ae_isfinite(x, _state)&&ae_isfinite(y, _state), "Spline2DCalcVBuf: X or Y contains NaN or Infinite value", _state);
    
    /*
     * Allocate place for output
     */
    rvectorsetlengthatleast(f, c->d, _state);
    
    /*
     * Determine evaluation interval
     */
    l = 0;
    r = c->n-1;
    while(l!=r-1)
    {
        h = (l+r)/2;
        if( ae_fp_greater_eq(c->x.ptr.p_double[h],x) )
        {
            r = h;
        }
        else
        {
            l = h;
        }
    }
    dt = 1.0/(c->x.ptr.p_double[l+1]-c->x.ptr.p_double[l]);
    t = (x-c->x.ptr.p_double[l])*dt;
    ix = l;
    l = 0;
    r = c->m-1;
    while(l!=r-1)
    {
        h = (l+r)/2;
        if( ae_fp_greater_eq(c->y.ptr.p_double[h],y) )
        {
            r = h;
        }
        else
        {
            l = h;
        }
    }
    du = 1.0/(c->y.ptr.p_double[l+1]-c->y.ptr.p_double[l]);
    u = (y-c->y.ptr.p_double[l])*du;
    iy = l;
    
    /*
     * Handle possible missing cells
     */
    if( c->hasmissingcells&&!spline2d_adjustevaluationinterval(c, &x, &t, &dt, &ix, &y, &u, &du, &iy, _state) )
    {
        rsetv(c->d, _state->v_nan, f, _state);
        return;
    }
    
    /*
     * Bilinear interpolation
     */
    if( c->stype==-1 )
    {
        for(i=0; i<=c->d-1; i++)
        {
            y1 = c->f.ptr.p_double[c->d*(c->n*iy+ix)+i];
            y2 = c->f.ptr.p_double[c->d*(c->n*iy+(ix+1))+i];
            y3 = c->f.ptr.p_double[c->d*(c->n*(iy+1)+(ix+1))+i];
            y4 = c->f.ptr.p_double[c->d*(c->n*(iy+1)+ix)+i];
            f->ptr.p_double[i] = ((double)1-t)*((double)1-u)*y1+t*((double)1-u)*y2+t*u*y3+((double)1-t)*u*y4;
        }
        return;
    }
    
    /*
     * Bicubic interpolation:
     * * calculate Hermite basis for dimensions X and Y (variables T and U),
     *   here HTij means basis function whose I-th derivative has value 1 at T=J.
     *   Same for HUij.
     * * after initial calculation, apply scaling by DT/DU to the basis
     * * calculate using stored table of second derivatives
     */
    ae_assert(c->stype==-3, "Spline2DCalc: integrity check failed", _state);
    sfx = c->n*c->m*c->d;
    sfy = 2*c->n*c->m*c->d;
    sfxy = 3*c->n*c->m*c->d;
    s1 = (c->n*iy+ix)*c->d;
    s2 = (c->n*iy+(ix+1))*c->d;
    s3 = (c->n*(iy+1)+ix)*c->d;
    s4 = (c->n*(iy+1)+(ix+1))*c->d;
    t2 = t*t;
    t3 = t*t2;
    u2 = u*u;
    u3 = u*u2;
    ht00 = (double)2*t3-(double)3*t2+(double)1;
    ht10 = t3-(double)2*t2+t;
    ht01 = -(double)2*t3+(double)3*t2;
    ht11 = t3-t2;
    hu00 = (double)2*u3-(double)3*u2+(double)1;
    hu10 = u3-(double)2*u2+u;
    hu01 = -(double)2*u3+(double)3*u2;
    hu11 = u3-u2;
    ht10 = ht10/dt;
    ht11 = ht11/dt;
    hu10 = hu10/du;
    hu11 = hu11/du;
    for(i=0; i<=c->d-1; i++)
    {
        
        /*
         * Calculate I-th component
         */
        f->ptr.p_double[i] = (double)(0);
        f->ptr.p_double[i] = f->ptr.p_double[i]+c->f.ptr.p_double[s1]*ht00*hu00+c->f.ptr.p_double[s2]*ht01*hu00+c->f.ptr.p_double[s3]*ht00*hu01+c->f.ptr.p_double[s4]*ht01*hu01;
        f->ptr.p_double[i] = f->ptr.p_double[i]+c->f.ptr.p_double[sfx+s1]*ht10*hu00+c->f.ptr.p_double[sfx+s2]*ht11*hu00+c->f.ptr.p_double[sfx+s3]*ht10*hu01+c->f.ptr.p_double[sfx+s4]*ht11*hu01;
        f->ptr.p_double[i] = f->ptr.p_double[i]+c->f.ptr.p_double[sfy+s1]*ht00*hu10+c->f.ptr.p_double[sfy+s2]*ht01*hu10+c->f.ptr.p_double[sfy+s3]*ht00*hu11+c->f.ptr.p_double[sfy+s4]*ht01*hu11;
        f->ptr.p_double[i] = f->ptr.p_double[i]+c->f.ptr.p_double[sfxy+s1]*ht10*hu10+c->f.ptr.p_double[sfxy+s2]*ht11*hu10+c->f.ptr.p_double[sfxy+s3]*ht10*hu11+c->f.ptr.p_double[sfxy+s4]*ht11*hu11;
        
        /*
         * Advance source indexes
         */
        s1 = s1+1;
        s2 = s2+1;
        s3 = s3+1;
        s4 = s4+1;
    }
}


/*************************************************************************
This subroutine calculates specific component of vector-valued bilinear or
bicubic spline at the given point (X,Y).

INPUT PARAMETERS:
    C   -   spline interpolant.
    X, Y-   point
    I   -   component index, in [0,D). An exception is generated for out
            of range values.

RESULT:
    value of I-th component

  -- ALGLIB PROJECT --
     Copyright 01.02.2018 by Bochkanov Sergey
*************************************************************************/
double spline2dcalcvi(const spline2dinterpolant* c,
     double x,
     double y,
     ae_int_t i,
     ae_state *_state)
{
    ae_int_t ix;
    ae_int_t iy;
    ae_int_t l;
    ae_int_t r;
    ae_int_t h;
    double t;
    double dt;
    double u;
    double du;
    double y1;
    double y2;
    double y3;
    double y4;
    ae_int_t s1;
    ae_int_t s2;
    ae_int_t s3;
    ae_int_t s4;
    ae_int_t sfx;
    ae_int_t sfy;
    ae_int_t sfxy;
    double t2;
    double t3;
    double u2;
    double u3;
    double ht00;
    double ht01;
    double ht10;
    double ht11;
    double hu00;
    double hu01;
    double hu10;
    double hu11;
    double result;


    ae_assert(c->stype==-1||c->stype==-3, "Spline2DCalcVi: incorrect C (incorrect parameter C.SType)", _state);
    ae_assert(ae_isfinite(x, _state)&&ae_isfinite(y, _state), "Spline2DCalcVi: X or Y contains NaN or Infinite value", _state);
    ae_assert(i>=0&&i<c->d, "Spline2DCalcVi: incorrect I (I<0 or I>=D)", _state);
    
    /*
     * Determine evaluation interval
     */
    l = 0;
    r = c->n-1;
    while(l!=r-1)
    {
        h = (l+r)/2;
        if( ae_fp_greater_eq(c->x.ptr.p_double[h],x) )
        {
            r = h;
        }
        else
        {
            l = h;
        }
    }
    dt = 1.0/(c->x.ptr.p_double[l+1]-c->x.ptr.p_double[l]);
    t = (x-c->x.ptr.p_double[l])*dt;
    ix = l;
    l = 0;
    r = c->m-1;
    while(l!=r-1)
    {
        h = (l+r)/2;
        if( ae_fp_greater_eq(c->y.ptr.p_double[h],y) )
        {
            r = h;
        }
        else
        {
            l = h;
        }
    }
    du = 1.0/(c->y.ptr.p_double[l+1]-c->y.ptr.p_double[l]);
    u = (y-c->y.ptr.p_double[l])*du;
    iy = l;
    
    /*
     * Handle possible missing cells
     */
    if( c->hasmissingcells&&!spline2d_adjustevaluationinterval(c, &x, &t, &dt, &ix, &y, &u, &du, &iy, _state) )
    {
        result = _state->v_nan;
        return result;
    }
    
    /*
     * Bilinear interpolation
     */
    if( c->stype==-1 )
    {
        y1 = c->f.ptr.p_double[c->d*(c->n*iy+ix)+i];
        y2 = c->f.ptr.p_double[c->d*(c->n*iy+(ix+1))+i];
        y3 = c->f.ptr.p_double[c->d*(c->n*(iy+1)+(ix+1))+i];
        y4 = c->f.ptr.p_double[c->d*(c->n*(iy+1)+ix)+i];
        result = ((double)1-t)*((double)1-u)*y1+t*((double)1-u)*y2+t*u*y3+((double)1-t)*u*y4;
        return result;
    }
    
    /*
     * Bicubic interpolation:
     * * calculate Hermite basis for dimensions X and Y (variables T and U),
     *   here HTij means basis function whose I-th derivative has value 1 at T=J.
     *   Same for HUij.
     * * after initial calculation, apply scaling by DT/DU to the basis
     * * calculate using stored table of second derivatives
     */
    ae_assert(c->stype==-3, "Spline2DCalc: integrity check failed", _state);
    sfx = c->n*c->m*c->d;
    sfy = 2*c->n*c->m*c->d;
    sfxy = 3*c->n*c->m*c->d;
    s1 = (c->n*iy+ix)*c->d;
    s2 = (c->n*iy+(ix+1))*c->d;
    s3 = (c->n*(iy+1)+ix)*c->d;
    s4 = (c->n*(iy+1)+(ix+1))*c->d;
    t2 = t*t;
    t3 = t*t2;
    u2 = u*u;
    u3 = u*u2;
    ht00 = (double)2*t3-(double)3*t2+(double)1;
    ht10 = t3-(double)2*t2+t;
    ht01 = -(double)2*t3+(double)3*t2;
    ht11 = t3-t2;
    hu00 = (double)2*u3-(double)3*u2+(double)1;
    hu10 = u3-(double)2*u2+u;
    hu01 = -(double)2*u3+(double)3*u2;
    hu11 = u3-u2;
    ht10 = ht10/dt;
    ht11 = ht11/dt;
    hu10 = hu10/du;
    hu11 = hu11/du;
    
    /*
     * Advance source indexes to I-th position
     */
    s1 = s1+i;
    s2 = s2+i;
    s3 = s3+i;
    s4 = s4+i;
    
    /*
     * Calculate I-th component
     */
    result = (double)(0);
    result = result+c->f.ptr.p_double[s1]*ht00*hu00+c->f.ptr.p_double[s2]*ht01*hu00+c->f.ptr.p_double[s3]*ht00*hu01+c->f.ptr.p_double[s4]*ht01*hu01;
    result = result+c->f.ptr.p_double[sfx+s1]*ht10*hu00+c->f.ptr.p_double[sfx+s2]*ht11*hu00+c->f.ptr.p_double[sfx+s3]*ht10*hu01+c->f.ptr.p_double[sfx+s4]*ht11*hu01;
    result = result+c->f.ptr.p_double[sfy+s1]*ht00*hu10+c->f.ptr.p_double[sfy+s2]*ht01*hu10+c->f.ptr.p_double[sfy+s3]*ht00*hu11+c->f.ptr.p_double[sfy+s4]*ht01*hu11;
    result = result+c->f.ptr.p_double[sfxy+s1]*ht10*hu10+c->f.ptr.p_double[sfxy+s2]*ht11*hu10+c->f.ptr.p_double[sfxy+s3]*ht10*hu11+c->f.ptr.p_double[sfxy+s4]*ht11*hu11;
    return result;
}


/*************************************************************************
This subroutine calculates bilinear or bicubic vector-valued spline at the
given point (X,Y).

INPUT PARAMETERS:
    C   -   spline interpolant.
    X, Y-   point

OUTPUT PARAMETERS:
    F   -   array[D] which stores function values.  F is out-parameter and
            it  is  reallocated  after  call to this function. In case you
            want  to    reuse  previously  allocated  F,   you   may   use
            Spline2DCalcVBuf(),  which  reallocates  F only when it is too
            small.

  -- ALGLIB PROJECT --
     Copyright 16.04.2012 by Bochkanov Sergey
*************************************************************************/
void spline2dcalcv(const spline2dinterpolant* c,
     double x,
     double y,
     /* Real    */ ae_vector* f,
     ae_state *_state)
{

    ae_vector_clear(f);

    ae_assert(c->stype==-1||c->stype==-3, "Spline2DCalcV: incorrect C (incorrect parameter C.SType)", _state);
    ae_assert(ae_isfinite(x, _state)&&ae_isfinite(y, _state), "Spline2DCalcV: either X=NaN/Infinite or Y=NaN/Infinite", _state);
    spline2dcalcvbuf(c, x, y, f, _state);
}


/*************************************************************************
This subroutine calculates the value and the derivatives of I-th component
of a  vector-valued bilinear or bicubic spline.

Input parameters:
    C   -   spline interpolant.
    X, Y-   point
    I   -   component index, in [0,D)

Output parameters:
    F   -   S(x,y)
    FX  -   dS(x,y)/dX
    FY  -   dS(x,y)/dY

  -- ALGLIB PROJECT --
     Copyright 05.07.2007 by Bochkanov Sergey
*************************************************************************/
void spline2ddiffvi(const spline2dinterpolant* c,
     double x,
     double y,
     ae_int_t i,
     double* f,
     double* fx,
     double* fy,
     ae_state *_state)
{
    ae_int_t d;
    double t;
    double dt;
    double u;
    double du;
    ae_int_t ix;
    ae_int_t iy;
    ae_int_t l;
    ae_int_t r;
    ae_int_t h;
    ae_int_t s1;
    ae_int_t s2;
    ae_int_t s3;
    ae_int_t s4;
    ae_int_t sfx;
    ae_int_t sfy;
    ae_int_t sfxy;
    double y1;
    double y2;
    double y3;
    double y4;
    double v0;
    double v1;
    double v2;
    double v3;
    double t2;
    double t3;
    double u2;
    double u3;
    double ht00;
    double ht01;
    double ht10;
    double ht11;
    double hu00;
    double hu01;
    double hu10;
    double hu11;
    double dht00;
    double dht01;
    double dht10;
    double dht11;
    double dhu00;
    double dhu01;
    double dhu10;
    double dhu11;

    *f = 0.0;
    *fx = 0.0;
    *fy = 0.0;

    ae_assert(c->stype==-1||c->stype==-3, "Spline2DDiffVI: incorrect C (incorrect parameter C.SType)", _state);
    ae_assert(ae_isfinite(x, _state)&&ae_isfinite(y, _state), "Spline2DDiffVI: X or Y contains NaN or Infinite value", _state);
    ae_assert(i>=0&&i<c->d, "Spline2DDiffVI: I<0 or I>=D", _state);
    
    /*
     * Prepare F, dF/dX, dF/dY, d2F/dXdY
     */
    *f = (double)(0);
    *fx = (double)(0);
    *fy = (double)(0);
    d = c->d;
    
    /*
     * Binary search in the [ x[0], ..., x[n-2] ] (x[n-1] is not included)
     */
    l = 0;
    r = c->n-1;
    while(l!=r-1)
    {
        h = (l+r)/2;
        if( ae_fp_greater_eq(c->x.ptr.p_double[h],x) )
        {
            r = h;
        }
        else
        {
            l = h;
        }
    }
    t = (x-c->x.ptr.p_double[l])/(c->x.ptr.p_double[l+1]-c->x.ptr.p_double[l]);
    dt = 1.0/(c->x.ptr.p_double[l+1]-c->x.ptr.p_double[l]);
    ix = l;
    
    /*
     * Binary search in the [ y[0], ..., y[m-2] ] (y[m-1] is not included)
     */
    l = 0;
    r = c->m-1;
    while(l!=r-1)
    {
        h = (l+r)/2;
        if( ae_fp_greater_eq(c->y.ptr.p_double[h],y) )
        {
            r = h;
        }
        else
        {
            l = h;
        }
    }
    u = (y-c->y.ptr.p_double[l])/(c->y.ptr.p_double[l+1]-c->y.ptr.p_double[l]);
    du = 1.0/(c->y.ptr.p_double[l+1]-c->y.ptr.p_double[l]);
    iy = l;
    
    /*
     * Handle possible missing cells
     */
    if( c->hasmissingcells&&!spline2d_adjustevaluationinterval(c, &x, &t, &dt, &ix, &y, &u, &du, &iy, _state) )
    {
        *f = _state->v_nan;
        *fx = _state->v_nan;
        *fy = _state->v_nan;
        return;
    }
    
    /*
     * Bilinear interpolation
     */
    if( c->stype==-1 )
    {
        y1 = c->f.ptr.p_double[d*(c->n*iy+ix)+i];
        y2 = c->f.ptr.p_double[d*(c->n*iy+(ix+1))+i];
        y3 = c->f.ptr.p_double[d*(c->n*(iy+1)+(ix+1))+i];
        y4 = c->f.ptr.p_double[d*(c->n*(iy+1)+ix)+i];
        *f = ((double)1-t)*((double)1-u)*y1+t*((double)1-u)*y2+t*u*y3+((double)1-t)*u*y4;
        *fx = (-((double)1-u)*y1+((double)1-u)*y2+u*y3-u*y4)*dt;
        *fy = (-((double)1-t)*y1-t*y2+t*y3+((double)1-t)*y4)*du;
        return;
    }
    
    /*
     * Bicubic interpolation
     */
    if( c->stype==-3 )
    {
        sfx = c->n*c->m*d;
        sfy = 2*c->n*c->m*d;
        sfxy = 3*c->n*c->m*d;
        s1 = d*(c->n*iy+ix)+i;
        s2 = d*(c->n*iy+(ix+1))+i;
        s3 = d*(c->n*(iy+1)+ix)+i;
        s4 = d*(c->n*(iy+1)+(ix+1))+i;
        t2 = t*t;
        t3 = t*t2;
        u2 = u*u;
        u3 = u*u2;
        ht00 = (double)2*t3-(double)3*t2+(double)1;
        ht10 = t3-(double)2*t2+t;
        ht01 = -(double)2*t3+(double)3*t2;
        ht11 = t3-t2;
        hu00 = (double)2*u3-(double)3*u2+(double)1;
        hu10 = u3-(double)2*u2+u;
        hu01 = -(double)2*u3+(double)3*u2;
        hu11 = u3-u2;
        ht10 = ht10/dt;
        ht11 = ht11/dt;
        hu10 = hu10/du;
        hu11 = hu11/du;
        dht00 = (double)6*t2-(double)6*t;
        dht10 = (double)3*t2-(double)4*t+(double)1;
        dht01 = -(double)6*t2+(double)6*t;
        dht11 = (double)3*t2-(double)2*t;
        dhu00 = (double)6*u2-(double)6*u;
        dhu10 = (double)3*u2-(double)4*u+(double)1;
        dhu01 = -(double)6*u2+(double)6*u;
        dhu11 = (double)3*u2-(double)2*u;
        dht00 = dht00*dt;
        dht01 = dht01*dt;
        dhu00 = dhu00*du;
        dhu01 = dhu01*du;
        *f = (double)(0);
        *fx = (double)(0);
        *fy = (double)(0);
        v0 = c->f.ptr.p_double[s1];
        v1 = c->f.ptr.p_double[s2];
        v2 = c->f.ptr.p_double[s3];
        v3 = c->f.ptr.p_double[s4];
        *f = *f+v0*ht00*hu00+v1*ht01*hu00+v2*ht00*hu01+v3*ht01*hu01;
        *fx = *fx+v0*dht00*hu00+v1*dht01*hu00+v2*dht00*hu01+v3*dht01*hu01;
        *fy = *fy+v0*ht00*dhu00+v1*ht01*dhu00+v2*ht00*dhu01+v3*ht01*dhu01;
        v0 = c->f.ptr.p_double[sfx+s1];
        v1 = c->f.ptr.p_double[sfx+s2];
        v2 = c->f.ptr.p_double[sfx+s3];
        v3 = c->f.ptr.p_double[sfx+s4];
        *f = *f+v0*ht10*hu00+v1*ht11*hu00+v2*ht10*hu01+v3*ht11*hu01;
        *fx = *fx+v0*dht10*hu00+v1*dht11*hu00+v2*dht10*hu01+v3*dht11*hu01;
        *fy = *fy+v0*ht10*dhu00+v1*ht11*dhu00+v2*ht10*dhu01+v3*ht11*dhu01;
        v0 = c->f.ptr.p_double[sfy+s1];
        v1 = c->f.ptr.p_double[sfy+s2];
        v2 = c->f.ptr.p_double[sfy+s3];
        v3 = c->f.ptr.p_double[sfy+s4];
        *f = *f+v0*ht00*hu10+v1*ht01*hu10+v2*ht00*hu11+v3*ht01*hu11;
        *fx = *fx+v0*dht00*hu10+v1*dht01*hu10+v2*dht00*hu11+v3*dht01*hu11;
        *fy = *fy+v0*ht00*dhu10+v1*ht01*dhu10+v2*ht00*dhu11+v3*ht01*dhu11;
        v0 = c->f.ptr.p_double[sfxy+s1];
        v1 = c->f.ptr.p_double[sfxy+s2];
        v2 = c->f.ptr.p_double[sfxy+s3];
        v3 = c->f.ptr.p_double[sfxy+s4];
        *f = *f+v0*ht10*hu10+v1*ht11*hu10+v2*ht10*hu11+v3*ht11*hu11;
        *fx = *fx+v0*dht10*hu10+v1*dht11*hu10+v2*dht10*hu11+v3*dht11*hu11;
        *fy = *fy+v0*ht10*dhu10+v1*ht11*dhu10+v2*ht10*dhu11+v3*ht11*dhu11;
        return;
    }
}


/*************************************************************************
This subroutine calculates the value and the derivatives of I-th component
of a vector-valued bilinear or bicubic spline.

Input parameters:
    C   -   spline interpolant.
    X, Y-   point
    I   -   component index, in [0,D)

Output parameters:
    F   -   S(x,y)
    FX  -   dS(x,y)/dX
    FY  -   dS(x,y)/dY
    FXX -   d2S(x,y)/dXdX
    FXY -   d2S(x,y)/dXdY
    FYY -   d2S(x,y)/dYdY

  -- ALGLIB PROJECT --
     Copyright 17.04.2023 by Bochkanov Sergey.
     
     The second derivatives code was contributed by  Horst  Greiner  under
     public domain terms.
*************************************************************************/
void spline2ddiff2vi(const spline2dinterpolant* c,
     double x,
     double y,
     ae_int_t i,
     double* f,
     double* fx,
     double* fy,
     double* fxx,
     double* fxy,
     double* fyy,
     ae_state *_state)
{
    ae_int_t d;
    double t;
    double dt;
    double u;
    double du;
    ae_int_t ix;
    ae_int_t iy;
    ae_int_t l;
    ae_int_t r;
    ae_int_t h;
    ae_int_t s1;
    ae_int_t s2;
    ae_int_t s3;
    ae_int_t s4;
    ae_int_t sfx;
    ae_int_t sfy;
    ae_int_t sfxy;
    double y1;
    double y2;
    double y3;
    double y4;
    double v0;
    double v1;
    double v2;
    double v3;
    double t2;
    double t3;
    double u2;
    double u3;
    double ht00;
    double ht01;
    double ht10;
    double ht11;
    double hu00;
    double hu01;
    double hu10;
    double hu11;
    double dht00;
    double dht01;
    double dht10;
    double dht11;
    double dhu00;
    double dhu01;
    double dhu10;
    double dhu11;
    double d2ht00;
    double d2ht01;
    double d2ht10;
    double d2ht11;
    double d2hu00;
    double d2hu01;
    double d2hu10;
    double d2hu11;

    *f = 0.0;
    *fx = 0.0;
    *fy = 0.0;
    *fxx = 0.0;
    *fxy = 0.0;
    *fyy = 0.0;

    ae_assert(c->stype==-1||c->stype==-3, "Spline2DDiffVI: incorrect C (incorrect parameter C.SType)", _state);
    ae_assert(ae_isfinite(x, _state)&&ae_isfinite(y, _state), "Spline2DDiffVI: X or Y contains NaN or Infinite value", _state);
    ae_assert(i>=0&&i<c->d, "Spline2DDiffVI: I<0 or I>=D", _state);
    
    /*
     * Prepare F, dF/dX, dF/dY, d2F/dXdY
     */
    *f = (double)(0);
    *fx = (double)(0);
    *fy = (double)(0);
    *fxx = (double)(0);
    *fxy = (double)(0);
    *fyy = (double)(0);
    d = c->d;
    
    /*
     * Binary search in the [ x[0], ..., x[n-2] ] (x[n-1] is not included)
     */
    l = 0;
    r = c->n-1;
    while(l!=r-1)
    {
        h = (l+r)/2;
        if( ae_fp_greater_eq(c->x.ptr.p_double[h],x) )
        {
            r = h;
        }
        else
        {
            l = h;
        }
    }
    t = (x-c->x.ptr.p_double[l])/(c->x.ptr.p_double[l+1]-c->x.ptr.p_double[l]);
    dt = 1.0/(c->x.ptr.p_double[l+1]-c->x.ptr.p_double[l]);
    ix = l;
    
    /*
     * Binary search in the [ y[0], ..., y[m-2] ] (y[m-1] is not included)
     */
    l = 0;
    r = c->m-1;
    while(l!=r-1)
    {
        h = (l+r)/2;
        if( ae_fp_greater_eq(c->y.ptr.p_double[h],y) )
        {
            r = h;
        }
        else
        {
            l = h;
        }
    }
    u = (y-c->y.ptr.p_double[l])/(c->y.ptr.p_double[l+1]-c->y.ptr.p_double[l]);
    du = 1.0/(c->y.ptr.p_double[l+1]-c->y.ptr.p_double[l]);
    iy = l;
    
    /*
     * Handle possible missing cells
     */
    if( c->hasmissingcells&&!spline2d_adjustevaluationinterval(c, &x, &t, &dt, &ix, &y, &u, &du, &iy, _state) )
    {
        *f = _state->v_nan;
        *fx = _state->v_nan;
        *fy = _state->v_nan;
        *fxx = _state->v_nan;
        *fxy = _state->v_nan;
        *fyy = _state->v_nan;
        return;
    }
    
    /*
     * Bilinear interpolation
     */
    if( c->stype==-1 )
    {
        y1 = c->f.ptr.p_double[d*(c->n*iy+ix)+i];
        y2 = c->f.ptr.p_double[d*(c->n*iy+(ix+1))+i];
        y3 = c->f.ptr.p_double[d*(c->n*(iy+1)+(ix+1))+i];
        y4 = c->f.ptr.p_double[d*(c->n*(iy+1)+ix)+i];
        *f = ((double)1-t)*((double)1-u)*y1+t*((double)1-u)*y2+t*u*y3+((double)1-t)*u*y4;
        *fx = (-((double)1-u)*y1+((double)1-u)*y2+u*y3-u*y4)*dt;
        *fy = (-((double)1-t)*y1-t*y2+t*y3+((double)1-t)*y4)*du;
        *fxx = (double)(0);
        *fxy = (y1-y2+y3-y4)*du*dt;
        *fyy = (double)(0);
        return;
    }
    
    /*
     * Bicubic interpolation
     */
    if( c->stype==-3 )
    {
        sfx = c->n*c->m*d;
        sfy = 2*c->n*c->m*d;
        sfxy = 3*c->n*c->m*d;
        s1 = d*(c->n*iy+ix)+i;
        s2 = d*(c->n*iy+(ix+1))+i;
        s3 = d*(c->n*(iy+1)+ix)+i;
        s4 = d*(c->n*(iy+1)+(ix+1))+i;
        t2 = t*t;
        t3 = t*t2;
        u2 = u*u;
        u3 = u*u2;
        ht00 = (double)2*t3-(double)3*t2+(double)1;
        ht10 = t3-(double)2*t2+t;
        ht01 = -(double)2*t3+(double)3*t2;
        ht11 = t3-t2;
        hu00 = (double)2*u3-(double)3*u2+(double)1;
        hu10 = u3-(double)2*u2+u;
        hu01 = -(double)2*u3+(double)3*u2;
        hu11 = u3-u2;
        ht10 = ht10/dt;
        ht11 = ht11/dt;
        hu10 = hu10/du;
        hu11 = hu11/du;
        dht00 = (double)6*t2-(double)6*t;
        dht10 = (double)3*t2-(double)4*t+(double)1;
        dht01 = -(double)6*t2+(double)6*t;
        dht11 = (double)3*t2-(double)2*t;
        dhu00 = (double)6*u2-(double)6*u;
        dhu10 = (double)3*u2-(double)4*u+(double)1;
        dhu01 = -(double)6*u2+(double)6*u;
        dhu11 = (double)3*u2-(double)2*u;
        dht00 = dht00*dt;
        dht01 = dht01*dt;
        dhu00 = dhu00*du;
        dhu01 = dhu01*du;
        d2ht00 = ((double)12*t-(double)6)*dt*dt;
        d2ht01 = (-(double)12*t+(double)6)*dt*dt;
        d2ht10 = ((double)6*t-(double)4)*dt;
        d2ht11 = ((double)6*t-(double)2)*dt;
        d2hu00 = ((double)12*u-(double)6)*du*du;
        d2hu01 = (-(double)12*u+(double)6)*du*du;
        d2hu10 = ((double)6*u-(double)4)*du;
        d2hu11 = ((double)6*u-(double)2)*du;
        *f = (double)(0);
        *fx = (double)(0);
        *fy = (double)(0);
        *fxy = (double)(0);
        v0 = c->f.ptr.p_double[s1];
        v1 = c->f.ptr.p_double[s2];
        v2 = c->f.ptr.p_double[s3];
        v3 = c->f.ptr.p_double[s4];
        *f = *f+v0*ht00*hu00+v1*ht01*hu00+v2*ht00*hu01+v3*ht01*hu01;
        *fx = *fx+v0*dht00*hu00+v1*dht01*hu00+v2*dht00*hu01+v3*dht01*hu01;
        *fy = *fy+v0*ht00*dhu00+v1*ht01*dhu00+v2*ht00*dhu01+v3*ht01*dhu01;
        *fxx = *fxx+v0*d2ht00*hu00+v1*d2ht01*hu00+v2*d2ht00*hu01+v3*d2ht01*hu01;
        *fxy = *fxy+v0*dht00*dhu00+v1*dht01*dhu00+v2*dht00*dhu01+v3*dht01*dhu01;
        *fyy = *fyy+v0*ht00*d2hu00+v1*ht01*d2hu00+v2*ht00*d2hu01+v3*ht01*d2hu01;
        v0 = c->f.ptr.p_double[sfx+s1];
        v1 = c->f.ptr.p_double[sfx+s2];
        v2 = c->f.ptr.p_double[sfx+s3];
        v3 = c->f.ptr.p_double[sfx+s4];
        *f = *f+v0*ht10*hu00+v1*ht11*hu00+v2*ht10*hu01+v3*ht11*hu01;
        *fx = *fx+v0*dht10*hu00+v1*dht11*hu00+v2*dht10*hu01+v3*dht11*hu01;
        *fy = *fy+v0*ht10*dhu00+v1*ht11*dhu00+v2*ht10*dhu01+v3*ht11*dhu01;
        *fxx = *fxx+v0*d2ht10*hu00+v1*d2ht11*hu00+v2*d2ht10*hu01+v3*d2ht11*hu01;
        *fxy = *fxy+v0*dht10*dhu00+v1*dht11*dhu00+v2*dht10*dhu01+v3*dht11*dhu01;
        *fyy = *fyy+v0*ht10*d2hu00+v1*ht11*d2hu00+v2*ht10*d2hu01+v3*ht11*d2hu01;
        v0 = c->f.ptr.p_double[sfy+s1];
        v1 = c->f.ptr.p_double[sfy+s2];
        v2 = c->f.ptr.p_double[sfy+s3];
        v3 = c->f.ptr.p_double[sfy+s4];
        *f = *f+v0*ht00*hu10+v1*ht01*hu10+v2*ht00*hu11+v3*ht01*hu11;
        *fx = *fx+v0*dht00*hu10+v1*dht01*hu10+v2*dht00*hu11+v3*dht01*hu11;
        *fy = *fy+v0*ht00*dhu10+v1*ht01*dhu10+v2*ht00*dhu11+v3*ht01*dhu11;
        *fxx = *fxx+v0*d2ht00*hu10+v1*d2ht01*hu10+v2*d2ht00*hu11+v3*d2ht01*hu11;
        *fxy = *fxy+v0*dht00*dhu10+v1*dht01*dhu10+v2*dht00*dhu11+v3*dht01*dhu11;
        *fyy = *fyy+v0*ht00*d2hu10+v1*ht01*d2hu10+v2*ht00*d2hu11+v3*ht01*d2hu11;
        v0 = c->f.ptr.p_double[sfxy+s1];
        v1 = c->f.ptr.p_double[sfxy+s2];
        v2 = c->f.ptr.p_double[sfxy+s3];
        v3 = c->f.ptr.p_double[sfxy+s4];
        *f = *f+v0*ht10*hu10+v1*ht11*hu10+v2*ht10*hu11+v3*ht11*hu11;
        *fx = *fx+v0*dht10*hu10+v1*dht11*hu10+v2*dht10*hu11+v3*dht11*hu11;
        *fy = *fy+v0*ht10*dhu10+v1*ht11*dhu10+v2*ht10*dhu11+v3*ht11*dhu11;
        *fxx = *fxx+v0*d2ht10*hu10+v1*d2ht11*hu10+v2*d2ht10*hu11+v3*d2ht11*hu11;
        *fxy = *fxy+v0*dht10*dhu10+v1*dht11*dhu10+v2*dht10*dhu11+v3*dht11*dhu11;
        *fyy = *fyy+v0*ht10*d2hu10+v1*ht11*d2hu10+v2*ht10*d2hu11+v3*ht11*d2hu11;
        return;
    }
}


/*************************************************************************
This subroutine performs linear transformation of the spline argument.

Input parameters:
    C       -   spline interpolant
    AX, BX  -   transformation coefficients: x = A*t + B
    AY, BY  -   transformation coefficients: y = A*u + B
Result:
    C   -   transformed spline

  -- ALGLIB PROJECT --
     Copyright 30.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline2dlintransxy(spline2dinterpolant* c,
     double ax,
     double bx,
     double ay,
     double by,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector x;
    ae_vector y;
    ae_vector f;
    ae_vector v;
    ae_vector missing;
    ae_bool missingv;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&y, 0, sizeof(y));
    memset(&f, 0, sizeof(f));
    memset(&v, 0, sizeof(v));
    memset(&missing, 0, sizeof(missing));
    ae_vector_init(&x, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&y, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&f, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&v, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&missing, 0, DT_BOOL, _state, ae_true);

    ae_assert(c->stype==-3||c->stype==-1, "Spline2DLinTransXY: incorrect C (incorrect parameter C.SType)", _state);
    ae_assert(ae_isfinite(ax, _state), "Spline2DLinTransXY: AX is infinite or NaN", _state);
    ae_assert(ae_isfinite(bx, _state), "Spline2DLinTransXY: BX is infinite or NaN", _state);
    ae_assert(ae_isfinite(ay, _state), "Spline2DLinTransXY: AY is infinite or NaN", _state);
    ae_assert(ae_isfinite(by, _state), "Spline2DLinTransXY: BY is infinite or NaN", _state);
    ae_vector_set_length(&x, c->n, _state);
    ae_vector_set_length(&y, c->m, _state);
    ae_vector_set_length(&f, c->m*c->n*c->d, _state);
    for(j=0; j<=c->n-1; j++)
    {
        x.ptr.p_double[j] = c->x.ptr.p_double[j];
    }
    for(i=0; i<=c->m-1; i++)
    {
        y.ptr.p_double[i] = c->y.ptr.p_double[i];
    }
    for(i=0; i<=c->m-1; i++)
    {
        for(j=0; j<=c->n-1; j++)
        {
            for(k=0; k<=c->d-1; k++)
            {
                f.ptr.p_double[c->d*(i*c->n+j)+k] = c->f.ptr.p_double[c->d*(i*c->n+j)+k];
            }
        }
    }
    
    /*
     * Handle different combinations of AX/AY
     */
    bsetallocv(c->n*c->m, ae_false, &missing, _state);
    if( ae_fp_eq(ax,(double)(0))&&ae_fp_neq(ay,(double)(0)) )
    {
        for(i=0; i<=c->m-1; i++)
        {
            spline2dcalcvbuf(c, bx, y.ptr.p_double[i], &v, _state);
            y.ptr.p_double[i] = (y.ptr.p_double[i]-by)/ay;
            missingv = !ae_isfinite(v.ptr.p_double[0], _state);
            for(j=0; j<=c->n-1; j++)
            {
                for(k=0; k<=c->d-1; k++)
                {
                    f.ptr.p_double[c->d*(i*c->n+j)+k] = v.ptr.p_double[k];
                }
                missing.ptr.p_bool[i*c->n+j] = missingv;
            }
        }
    }
    if( ae_fp_neq(ax,(double)(0))&&ae_fp_eq(ay,(double)(0)) )
    {
        for(j=0; j<=c->n-1; j++)
        {
            spline2dcalcvbuf(c, x.ptr.p_double[j], by, &v, _state);
            x.ptr.p_double[j] = (x.ptr.p_double[j]-bx)/ax;
            missingv = !ae_isfinite(v.ptr.p_double[0], _state);
            for(i=0; i<=c->m-1; i++)
            {
                for(k=0; k<=c->d-1; k++)
                {
                    f.ptr.p_double[c->d*(i*c->n+j)+k] = v.ptr.p_double[k];
                }
                missing.ptr.p_bool[i*c->n+j] = missingv;
            }
        }
    }
    if( ae_fp_neq(ax,(double)(0))&&ae_fp_neq(ay,(double)(0)) )
    {
        for(j=0; j<=c->n-1; j++)
        {
            x.ptr.p_double[j] = (x.ptr.p_double[j]-bx)/ax;
        }
        for(i=0; i<=c->m-1; i++)
        {
            y.ptr.p_double[i] = (y.ptr.p_double[i]-by)/ay;
        }
        if( c->hasmissingcells )
        {
            bcopyv(c->n*c->m, &c->ismissingnode, &missing, _state);
        }
    }
    if( ae_fp_eq(ax,(double)(0))&&ae_fp_eq(ay,(double)(0)) )
    {
        spline2dcalcvbuf(c, bx, by, &v, _state);
        for(i=0; i<=c->m-1; i++)
        {
            for(j=0; j<=c->n-1; j++)
            {
                for(k=0; k<=c->d-1; k++)
                {
                    f.ptr.p_double[c->d*(i*c->n+j)+k] = v.ptr.p_double[k];
                }
            }
        }
        bsetv(c->n*c->m, !ae_isfinite(v.ptr.p_double[0], _state), &missing, _state);
    }
    
    /*
     * Rebuild spline
     */
    if( !c->hasmissingcells )
    {
        if( c->stype==-3 )
        {
            spline2dbuildbicubicvbuf(&x, c->n, &y, c->m, &f, c->d, c, _state);
        }
        if( c->stype==-1 )
        {
            spline2dbuildbilinearvbuf(&x, c->n, &y, c->m, &f, c->d, c, _state);
        }
    }
    else
    {
        if( c->stype==-3 )
        {
            spline2dbuildbicubicmissingbuf(&x, c->n, &y, c->m, &f, &missing, c->d, c, _state);
        }
        if( c->stype==-1 )
        {
            spline2dbuildbilinearmissingbuf(&x, c->n, &y, c->m, &f, &missing, c->d, c, _state);
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
This subroutine performs linear transformation of the spline.

Input parameters:
    C   -   spline interpolant.
    A, B-   transformation coefficients: S2(x,y) = A*S(x,y) + B
    
Output parameters:
    C   -   transformed spline

  -- ALGLIB PROJECT --
     Copyright 30.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline2dlintransf(spline2dinterpolant* c,
     double a,
     double b,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector x;
    ae_vector y;
    ae_vector f;
    ae_vector missing;
    ae_int_t i;
    ae_int_t j;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&y, 0, sizeof(y));
    memset(&f, 0, sizeof(f));
    memset(&missing, 0, sizeof(missing));
    ae_vector_init(&x, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&y, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&f, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&missing, 0, DT_BOOL, _state, ae_true);

    ae_assert(c->stype==-3||c->stype==-1, "Spline2DLinTransF: incorrect C (incorrect parameter C.SType)", _state);
    if( c->stype==-1 )
    {
        
        /*
         * Bilinear spline
         */
        if( !c->hasmissingcells )
        {
            
            /*
             * Quick code for a spline without missing cells
             */
            for(i=0; i<=c->m*c->n*c->d-1; i++)
            {
                c->f.ptr.p_double[i] = a*c->f.ptr.p_double[i]+b;
            }
        }
        else
        {
            
            /*
             * Slower code for missing cells
             */
            for(i=0; i<=c->m*c->n*c->d-1; i++)
            {
                if( !c->ismissingnode.ptr.p_bool[i/c->d] )
                {
                    c->f.ptr.p_double[i] = a*c->f.ptr.p_double[i]+b;
                }
            }
        }
    }
    else
    {
        
        /*
         * Bicubic spline
         */
        if( !c->hasmissingcells )
        {
            
            /*
             * Quick code for a spline without missing cells
             */
            ae_vector_set_length(&x, c->n, _state);
            ae_vector_set_length(&y, c->m, _state);
            ae_vector_set_length(&f, c->m*c->n*c->d, _state);
            for(j=0; j<=c->n-1; j++)
            {
                x.ptr.p_double[j] = c->x.ptr.p_double[j];
            }
            for(i=0; i<=c->m-1; i++)
            {
                y.ptr.p_double[i] = c->y.ptr.p_double[i];
            }
            for(i=0; i<=c->m*c->n*c->d-1; i++)
            {
                f.ptr.p_double[i] = a*c->f.ptr.p_double[i]+b;
            }
            spline2dbuildbicubicvbuf(&x, c->n, &y, c->m, &f, c->d, c, _state);
        }
        else
        {
            
            /*
             * Slower code for missing cells
             */
            ae_vector_set_length(&x, c->n, _state);
            ae_vector_set_length(&y, c->m, _state);
            rsetallocv(c->m*c->n*c->d, 0.0, &f, _state);
            for(j=0; j<=c->n-1; j++)
            {
                x.ptr.p_double[j] = c->x.ptr.p_double[j];
            }
            for(i=0; i<=c->m-1; i++)
            {
                y.ptr.p_double[i] = c->y.ptr.p_double[i];
            }
            for(i=0; i<=c->m*c->n*c->d-1; i++)
            {
                if( !c->ismissingnode.ptr.p_bool[i/c->d] )
                {
                    f.ptr.p_double[i] = a*c->f.ptr.p_double[i]+b;
                }
            }
            bcopyallocv(c->m*c->n, &c->ismissingnode, &missing, _state);
            spline2dbuildbicubicmissingbuf(&x, c->n, &y, c->m, &f, &missing, c->d, c, _state);
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
This subroutine makes the copy of the spline model.

Input parameters:
    C   -   spline interpolant

Output parameters:
    CC  -   spline copy

  -- ALGLIB PROJECT --
     Copyright 29.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline2dcopy(const spline2dinterpolant* c,
     spline2dinterpolant* cc,
     ae_state *_state)
{
    ae_int_t tblsize;

    _spline2dinterpolant_clear(cc);

    ae_assert(c->stype==-1||c->stype==-3, "Spline2DCopy: incorrect C (incorrect parameter C.SType)", _state);
    cc->n = c->n;
    cc->m = c->m;
    cc->d = c->d;
    cc->stype = c->stype;
    cc->hasmissingcells = c->hasmissingcells;
    tblsize = -1;
    if( c->stype==-3 )
    {
        tblsize = 4*c->n*c->m*c->d;
    }
    if( c->stype==-1 )
    {
        tblsize = c->n*c->m*c->d;
    }
    ae_assert(tblsize>0, "Spline2DCopy: internal error", _state);
    ae_vector_set_length(&cc->x, cc->n, _state);
    ae_vector_set_length(&cc->y, cc->m, _state);
    ae_vector_set_length(&cc->f, tblsize, _state);
    ae_v_move(&cc->x.ptr.p_double[0], 1, &c->x.ptr.p_double[0], 1, ae_v_len(0,cc->n-1));
    ae_v_move(&cc->y.ptr.p_double[0], 1, &c->y.ptr.p_double[0], 1, ae_v_len(0,cc->m-1));
    ae_v_move(&cc->f.ptr.p_double[0], 1, &c->f.ptr.p_double[0], 1, ae_v_len(0,tblsize-1));
    if( c->hasmissingcells )
    {
        bcopyallocv(c->n*c->m, &c->ismissingnode, &cc->ismissingnode, _state);
        bcopyallocv((c->n-1)*(c->m-1), &c->ismissingcell, &cc->ismissingcell, _state);
    }
}


/*************************************************************************
Bicubic spline resampling

Input parameters:
    A           -   function values at the old grid,
                    array[0..OldHeight-1, 0..OldWidth-1]
    OldHeight   -   old grid height, OldHeight>1
    OldWidth    -   old grid width, OldWidth>1
    NewHeight   -   new grid height, NewHeight>1
    NewWidth    -   new grid width, NewWidth>1
    
Output parameters:
    B           -   function values at the new grid,
                    array[0..NewHeight-1, 0..NewWidth-1]

  -- ALGLIB routine --
     15 May, 2007
     Copyright by Bochkanov Sergey
*************************************************************************/
void spline2dresamplebicubic(/* Real    */ const ae_matrix* a,
     ae_int_t oldheight,
     ae_int_t oldwidth,
     /* Real    */ ae_matrix* b,
     ae_int_t newheight,
     ae_int_t newwidth,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix buf;
    ae_vector x;
    ae_vector y;
    spline1dinterpolant c;
    ae_int_t mw;
    ae_int_t mh;
    ae_int_t i;
    ae_int_t j;

    ae_frame_make(_state, &_frame_block);
    memset(&buf, 0, sizeof(buf));
    memset(&x, 0, sizeof(x));
    memset(&y, 0, sizeof(y));
    memset(&c, 0, sizeof(c));
    ae_matrix_clear(b);
    ae_matrix_init(&buf, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&x, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&y, 0, DT_REAL, _state, ae_true);
    _spline1dinterpolant_init(&c, _state, ae_true);

    ae_assert(oldwidth>1&&oldheight>1, "Spline2DResampleBicubic: width/height less than 1", _state);
    ae_assert(newwidth>1&&newheight>1, "Spline2DResampleBicubic: width/height less than 1", _state);
    
    /*
     * Prepare
     */
    mw = ae_maxint(oldwidth, newwidth, _state);
    mh = ae_maxint(oldheight, newheight, _state);
    ae_matrix_set_length(b, newheight, newwidth, _state);
    ae_matrix_set_length(&buf, oldheight, newwidth, _state);
    ae_vector_set_length(&x, ae_maxint(mw, mh, _state), _state);
    ae_vector_set_length(&y, ae_maxint(mw, mh, _state), _state);
    
    /*
     * Horizontal interpolation
     */
    for(i=0; i<=oldheight-1; i++)
    {
        
        /*
         * Fill X, Y
         */
        for(j=0; j<=oldwidth-1; j++)
        {
            x.ptr.p_double[j] = (double)j/(double)(oldwidth-1);
            y.ptr.p_double[j] = a->ptr.pp_double[i][j];
        }
        
        /*
         * Interpolate and place result into temporary matrix
         */
        spline1dbuildcubic(&x, &y, oldwidth, 0, 0.0, 0, 0.0, &c, _state);
        for(j=0; j<=newwidth-1; j++)
        {
            buf.ptr.pp_double[i][j] = spline1dcalc(&c, (double)j/(double)(newwidth-1), _state);
        }
    }
    
    /*
     * Vertical interpolation
     */
    for(j=0; j<=newwidth-1; j++)
    {
        
        /*
         * Fill X, Y
         */
        for(i=0; i<=oldheight-1; i++)
        {
            x.ptr.p_double[i] = (double)i/(double)(oldheight-1);
            y.ptr.p_double[i] = buf.ptr.pp_double[i][j];
        }
        
        /*
         * Interpolate and place result into B
         */
        spline1dbuildcubic(&x, &y, oldheight, 0, 0.0, 0, 0.0, &c, _state);
        for(i=0; i<=newheight-1; i++)
        {
            b->ptr.pp_double[i][j] = spline1dcalc(&c, (double)i/(double)(newheight-1), _state);
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Bilinear spline resampling

Input parameters:
    A           -   function values at the old grid,
                    array[0..OldHeight-1, 0..OldWidth-1]
    OldHeight   -   old grid height, OldHeight>1
    OldWidth    -   old grid width, OldWidth>1
    NewHeight   -   new grid height, NewHeight>1
    NewWidth    -   new grid width, NewWidth>1

Output parameters:
    B           -   function values at the new grid,
                    array[0..NewHeight-1, 0..NewWidth-1]

  -- ALGLIB routine --
     09.07.2007
     Copyright by Bochkanov Sergey
*************************************************************************/
void spline2dresamplebilinear(/* Real    */ const ae_matrix* a,
     ae_int_t oldheight,
     ae_int_t oldwidth,
     /* Real    */ ae_matrix* b,
     ae_int_t newheight,
     ae_int_t newwidth,
     ae_state *_state)
{
    ae_int_t l;
    ae_int_t c;
    double t;
    double u;
    ae_int_t i;
    ae_int_t j;

    ae_matrix_clear(b);

    ae_assert(oldwidth>1&&oldheight>1, "Spline2DResampleBilinear: width/height less than 1", _state);
    ae_assert(newwidth>1&&newheight>1, "Spline2DResampleBilinear: width/height less than 1", _state);
    ae_matrix_set_length(b, newheight, newwidth, _state);
    for(i=0; i<=newheight-1; i++)
    {
        for(j=0; j<=newwidth-1; j++)
        {
            l = i*(oldheight-1)/(newheight-1);
            if( l==oldheight-1 )
            {
                l = oldheight-2;
            }
            u = (double)i/(double)(newheight-1)*(double)(oldheight-1)-(double)l;
            c = j*(oldwidth-1)/(newwidth-1);
            if( c==oldwidth-1 )
            {
                c = oldwidth-2;
            }
            t = (double)(j*(oldwidth-1))/(double)(newwidth-1)-(double)c;
            b->ptr.pp_double[i][j] = ((double)1-t)*((double)1-u)*a->ptr.pp_double[l][c]+t*((double)1-u)*a->ptr.pp_double[l][c+1]+t*u*a->ptr.pp_double[l+1][c+1]+((double)1-t)*u*a->ptr.pp_double[l+1][c];
        }
    }
}


/*************************************************************************
This subroutine builds bilinear vector-valued spline.

This function produces C0-continuous spline, i.e.  the  spline  itself  is
continuous, however its first and second  derivatives have discontinuities
at the spline cell boundaries.

Input parameters:
    X   -   spline abscissas, array[0..N-1]
    Y   -   spline ordinates, array[0..M-1]
    F   -   function values, array[0..M*N*D-1]:
            * first D elements store D values at (X[0],Y[0])
            * next D elements store D values at (X[1],Y[0])
            * general form - D function values at (X[i],Y[j]) are stored
              at F[D*(J*N+I)...D*(J*N+I)+D-1].
    M,N -   grid size, M>=2, N>=2
    D   -   vector dimension, D>=1

Output parameters:
    C   -   spline interpolant

  -- ALGLIB PROJECT --
     Copyright 16.04.2012 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildbilinearv(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     /* Real    */ const ae_vector* f,
     ae_int_t d,
     spline2dinterpolant* c,
     ae_state *_state)
{

    _spline2dinterpolant_clear(c);

    spline2dbuildbilinearvbuf(x, n, y, m, f, d, c, _state);
}


/*************************************************************************
This subroutine builds bilinear vector-valued spline.

Buffered version of Spline2DBuildBilinearV() which reuses memory previously
allocated in C as much as possible.

  -- ALGLIB PROJECT --
     Copyright 16.04.2012 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildbilinearvbuf(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     /* Real    */ const ae_vector* f,
     ae_int_t d,
     spline2dinterpolant* c,
     ae_state *_state)
{
    double t;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t i0;


    ae_assert(n>=2, "Spline2DBuildBilinearV: N is less then 2", _state);
    ae_assert(m>=2, "Spline2DBuildBilinearV: M is less then 2", _state);
    ae_assert(d>=1, "Spline2DBuildBilinearV: invalid argument D (D<1)", _state);
    ae_assert(x->cnt>=n&&y->cnt>=m, "Spline2DBuildBilinearV: length of X or Y is too short (Length(X/Y)<N/M)", _state);
    ae_assert(isfinitevector(x, n, _state)&&isfinitevector(y, m, _state), "Spline2DBuildBilinearV: X or Y contains NaN or Infinite value", _state);
    k = n*m*d;
    ae_assert(f->cnt>=k, "Spline2DBuildBilinearV: length of F is too short (Length(F)<N*M*D)", _state);
    ae_assert(isfinitevector(f, k, _state), "Spline2DBuildBilinearV: F contains NaN or Infinite value", _state);
    
    /*
     * Fill interpolant
     */
    c->n = n;
    c->m = m;
    c->d = d;
    c->stype = -1;
    c->hasmissingcells = ae_false;
    ae_vector_set_length(&c->x, c->n, _state);
    ae_vector_set_length(&c->y, c->m, _state);
    ae_vector_set_length(&c->f, k, _state);
    for(i=0; i<=c->n-1; i++)
    {
        c->x.ptr.p_double[i] = x->ptr.p_double[i];
    }
    for(i=0; i<=c->m-1; i++)
    {
        c->y.ptr.p_double[i] = y->ptr.p_double[i];
    }
    for(i=0; i<=k-1; i++)
    {
        c->f.ptr.p_double[i] = f->ptr.p_double[i];
    }
    
    /*
     * Sort points
     */
    for(j=0; j<=c->n-1; j++)
    {
        k = j;
        for(i=j+1; i<=c->n-1; i++)
        {
            if( ae_fp_less(c->x.ptr.p_double[i],c->x.ptr.p_double[k]) )
            {
                k = i;
            }
        }
        if( k!=j )
        {
            for(i=0; i<=c->m-1; i++)
            {
                for(i0=0; i0<=c->d-1; i0++)
                {
                    t = c->f.ptr.p_double[c->d*(i*c->n+j)+i0];
                    c->f.ptr.p_double[c->d*(i*c->n+j)+i0] = c->f.ptr.p_double[c->d*(i*c->n+k)+i0];
                    c->f.ptr.p_double[c->d*(i*c->n+k)+i0] = t;
                }
            }
            t = c->x.ptr.p_double[j];
            c->x.ptr.p_double[j] = c->x.ptr.p_double[k];
            c->x.ptr.p_double[k] = t;
        }
    }
    for(i=0; i<=c->m-1; i++)
    {
        k = i;
        for(j=i+1; j<=c->m-1; j++)
        {
            if( ae_fp_less(c->y.ptr.p_double[j],c->y.ptr.p_double[k]) )
            {
                k = j;
            }
        }
        if( k!=i )
        {
            for(j=0; j<=c->n-1; j++)
            {
                for(i0=0; i0<=c->d-1; i0++)
                {
                    t = c->f.ptr.p_double[c->d*(i*c->n+j)+i0];
                    c->f.ptr.p_double[c->d*(i*c->n+j)+i0] = c->f.ptr.p_double[c->d*(k*c->n+j)+i0];
                    c->f.ptr.p_double[c->d*(k*c->n+j)+i0] = t;
                }
            }
            t = c->y.ptr.p_double[i];
            c->y.ptr.p_double[i] = c->y.ptr.p_double[k];
            c->y.ptr.p_double[k] = t;
        }
    }
}


/*************************************************************************
This subroutine builds bilinear vector-valued  spline,  with  some  spline
cells being missing due to missing nodes.

This function produces C0-continuous spline, i.e.  the  spline  itself  is
continuous, however its first and second  derivatives have discontinuities
at the spline cell boundaries.

When the node (i,j) is missing, it means that: a) we don't  have  function
value at this point (elements of F[] are ignored), and  b)  we  don't need
spline value at cells adjacent to the node (i,j), i.e. up to 4 spline cells
will be dropped. An attempt to compute spline value at  the  missing  cell
will return NAN.

It is important to  understand  that  this  subroutine  does  NOT  support
interpolation on scattered grids. It allows us to drop some nodes, but  at
the cost of making a "hole in the spline" around this point. If  you  want
function  that   can   "fill  the  gap",  use  RBF  or  another  scattered
interpolation method.

The  intended  usage  for  this  subroutine  are  regularly  sampled,  but
non-rectangular datasets.

Input parameters:
    X   -   spline abscissas, array[0..N-1]
    Y   -   spline ordinates, array[0..M-1]
    F   -   function values, array[0..M*N*D-1]:
            * first D elements store D values at (X[0],Y[0])
            * next D elements store D values at (X[1],Y[0])
            * general form - D function values at (X[i],Y[j]) are stored
              at F[D*(J*N+I)...D*(J*N+I)+D-1].
            * missing values are ignored
    Missing array[M*N], Missing[J*N+I]=True means that corresponding entries
            of F[] are missing nodes.
    M,N -   grid size, M>=2, N>=2
    D   -   vector dimension, D>=1

Output parameters:
    C   -   spline interpolant

  -- ALGLIB PROJECT --
     Copyright 27.06.2022 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildbilinearmissing(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     /* Real    */ const ae_vector* _f,
     /* Boolean */ const ae_vector* missing,
     ae_int_t d,
     spline2dinterpolant* c,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector f;

    ae_frame_make(_state, &_frame_block);
    memset(&f, 0, sizeof(f));
    ae_vector_init_copy(&f, _f, _state, ae_true);
    _spline2dinterpolant_clear(c);

    spline2dbuildbilinearmissingbuf(x, n, y, m, &f, missing, d, c, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This subroutine builds bilinear vector-valued  spline,  with  some  spline
cells being missing due to missing nodes.

Buffered version of  Spline2DBuildBilinearMissing()  which  reuses  memory
previously allocated in C as much as possible.

  -- ALGLIB PROJECT --
     Copyright 27.06.2022 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildbilinearmissingbuf(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     /* Real    */ const ae_vector* _f,
     /* Boolean */ const ae_vector* missing,
     ae_int_t d,
     spline2dinterpolant* c,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector f;
    double t;
    ae_bool tb;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t i0;

    ae_frame_make(_state, &_frame_block);
    memset(&f, 0, sizeof(f));
    ae_vector_init_copy(&f, _f, _state, ae_true);

    ae_assert(n>=2, "Spline2DBuildBilinearMissing: N is less then 2", _state);
    ae_assert(m>=2, "Spline2DBuildBilinearMissing: M is less then 2", _state);
    ae_assert(d>=1, "Spline2DBuildBilinearMissing: invalid argument D (D<1)", _state);
    ae_assert(x->cnt>=n&&y->cnt>=m, "Spline2DBuildBilinearMissing: length of X or Y is too short (Length(X/Y)<N/M)", _state);
    ae_assert(isfinitevector(x, n, _state)&&isfinitevector(y, m, _state), "Spline2DBuildBilinearMissing: X or Y contains NaN or Infinite value", _state);
    k = n*m*d;
    ae_assert(f.cnt>=k, "Spline2DBuildBilinearMissing: length of F is too short (Length(F)<N*M*D)", _state);
    ae_assert(missing->cnt>=n*m, "Spline2DBuildBilinearMissing: Missing[] is shorter than M*N", _state);
    for(i=0; i<=k-1; i++)
    {
        if( !missing->ptr.p_bool[i/d]&&!ae_isfinite(f.ptr.p_double[i], _state) )
        {
            ae_assert(ae_false, "Spline2DBuildBilinearMissing: F[] contains NAN or INF in its non-missing entries", _state);
        }
    }
    
    /*
     * Fill interpolant.
     *
     * NOTE: we make sure that missing entries of F[] are filled by zeros.
     */
    c->n = n;
    c->m = m;
    c->d = d;
    c->stype = -1;
    c->hasmissingcells = ae_true;
    ae_vector_set_length(&c->x, c->n, _state);
    ae_vector_set_length(&c->y, c->m, _state);
    rsetallocv(k, 0.0, &c->f, _state);
    for(i=0; i<=c->n-1; i++)
    {
        c->x.ptr.p_double[i] = x->ptr.p_double[i];
    }
    for(i=0; i<=c->m-1; i++)
    {
        c->y.ptr.p_double[i] = y->ptr.p_double[i];
    }
    for(i=0; i<=k-1; i++)
    {
        if( !missing->ptr.p_bool[i/d] )
        {
            c->f.ptr.p_double[i] = f.ptr.p_double[i];
        }
    }
    bcopyallocv(c->n*c->m, missing, &c->ismissingnode, _state);
    
    /*
     * Sort points
     */
    for(j=0; j<=c->n-1; j++)
    {
        k = j;
        for(i=j+1; i<=c->n-1; i++)
        {
            if( ae_fp_less(c->x.ptr.p_double[i],c->x.ptr.p_double[k]) )
            {
                k = i;
            }
        }
        if( k!=j )
        {
            for(i=0; i<=c->m-1; i++)
            {
                for(i0=0; i0<=c->d-1; i0++)
                {
                    t = c->f.ptr.p_double[c->d*(i*c->n+j)+i0];
                    c->f.ptr.p_double[c->d*(i*c->n+j)+i0] = c->f.ptr.p_double[c->d*(i*c->n+k)+i0];
                    c->f.ptr.p_double[c->d*(i*c->n+k)+i0] = t;
                }
                tb = c->ismissingnode.ptr.p_bool[i*c->n+j];
                c->ismissingnode.ptr.p_bool[i*c->n+j] = c->ismissingnode.ptr.p_bool[i*c->n+k];
                c->ismissingnode.ptr.p_bool[i*c->n+k] = tb;
            }
            t = c->x.ptr.p_double[j];
            c->x.ptr.p_double[j] = c->x.ptr.p_double[k];
            c->x.ptr.p_double[k] = t;
        }
    }
    for(i=0; i<=c->m-1; i++)
    {
        k = i;
        for(j=i+1; j<=c->m-1; j++)
        {
            if( ae_fp_less(c->y.ptr.p_double[j],c->y.ptr.p_double[k]) )
            {
                k = j;
            }
        }
        if( k!=i )
        {
            for(j=0; j<=c->n-1; j++)
            {
                for(i0=0; i0<=c->d-1; i0++)
                {
                    t = c->f.ptr.p_double[c->d*(i*c->n+j)+i0];
                    c->f.ptr.p_double[c->d*(i*c->n+j)+i0] = c->f.ptr.p_double[c->d*(k*c->n+j)+i0];
                    c->f.ptr.p_double[c->d*(k*c->n+j)+i0] = t;
                }
                tb = c->ismissingnode.ptr.p_bool[i*c->n+j];
                c->ismissingnode.ptr.p_bool[i*c->n+j] = c->ismissingnode.ptr.p_bool[k*c->n+j];
                c->ismissingnode.ptr.p_bool[k*c->n+j] = tb;
            }
            t = c->y.ptr.p_double[i];
            c->y.ptr.p_double[i] = c->y.ptr.p_double[k];
            c->y.ptr.p_double[k] = t;
        }
    }
    
    /*
     * 1. Determine cells that are not missing. A cell is non-missing if it has four non-missing
     *    nodes at its corners.
     * 2. Normalize IsMissingNode[] array - all isolated points that are not part of some non-missing
     *    cell are marked as missing too
     */
    bsetallocv((c->m-1)*(c->n-1), ae_true, &c->ismissingcell, _state);
    for(i=0; i<=c->m-2; i++)
    {
        for(j=0; j<=c->n-2; j++)
        {
            if( ((!c->ismissingnode.ptr.p_bool[i*c->n+j]&&!c->ismissingnode.ptr.p_bool[(i+1)*c->n+j])&&!c->ismissingnode.ptr.p_bool[i*c->n+(j+1)])&&!c->ismissingnode.ptr.p_bool[(i+1)*c->n+(j+1)] )
            {
                c->ismissingcell.ptr.p_bool[i*(c->n-1)+j] = ae_false;
            }
        }
    }
    bsetv(c->m*c->n, ae_true, &c->ismissingnode, _state);
    for(i=0; i<=c->m-2; i++)
    {
        for(j=0; j<=c->n-2; j++)
        {
            if( !c->ismissingcell.ptr.p_bool[i*(c->n-1)+j] )
            {
                c->ismissingnode.ptr.p_bool[i*c->n+j] = ae_false;
                c->ismissingnode.ptr.p_bool[(i+1)*c->n+j] = ae_false;
                c->ismissingnode.ptr.p_bool[i*c->n+(j+1)] = ae_false;
                c->ismissingnode.ptr.p_bool[(i+1)*c->n+(j+1)] = ae_false;
            }
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
This subroutine builds bicubic vector-valued spline.

This function produces C2-continuous spline, i.e. the has smooth first and
second derivatives both inside spline cells and at the boundaries.

Input parameters:
    X   -   spline abscissas, array[0..N-1]
    Y   -   spline ordinates, array[0..M-1]
    F   -   function values, array[0..M*N*D-1]:
            * first D elements store D values at (X[0],Y[0])
            * next D elements store D values at (X[1],Y[0])
            * general form - D function values at (X[i],Y[j]) are stored
              at F[D*(J*N+I)...D*(J*N+I)+D-1].
    M,N -   grid size, M>=2, N>=2
    D   -   vector dimension, D>=1

Output parameters:
    C   -   spline interpolant

  -- ALGLIB PROJECT --
     Copyright 16.04.2012 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildbicubicv(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     /* Real    */ const ae_vector* _f,
     ae_int_t d,
     spline2dinterpolant* c,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector f;

    ae_frame_make(_state, &_frame_block);
    memset(&f, 0, sizeof(f));
    ae_vector_init_copy(&f, _f, _state, ae_true);
    _spline2dinterpolant_clear(c);

    spline2dbuildbicubicvbuf(x, n, y, m, &f, d, c, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This subroutine builds bicubic vector-valued spline.

Buffered version of Spline2DBuildBicubicV() which reuses memory previously
allocated in C as much as possible.

  -- ALGLIB PROJECT --
     Copyright 16.04.2012 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildbicubicvbuf(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     /* Real    */ const ae_vector* _f,
     ae_int_t d,
     spline2dinterpolant* c,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector f;
    ae_matrix tf;
    ae_matrix dx;
    ae_matrix dy;
    ae_matrix dxy;
    double t;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t di;

    ae_frame_make(_state, &_frame_block);
    memset(&f, 0, sizeof(f));
    memset(&tf, 0, sizeof(tf));
    memset(&dx, 0, sizeof(dx));
    memset(&dy, 0, sizeof(dy));
    memset(&dxy, 0, sizeof(dxy));
    ae_vector_init_copy(&f, _f, _state, ae_true);
    ae_matrix_init(&tf, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&dx, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&dy, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&dxy, 0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>=2, "Spline2DBuildBicubicV: N is less than 2", _state);
    ae_assert(m>=2, "Spline2DBuildBicubicV: M is less than 2", _state);
    ae_assert(d>=1, "Spline2DBuildBicubicV: invalid argument D (D<1)", _state);
    ae_assert(x->cnt>=n&&y->cnt>=m, "Spline2DBuildBicubicV: length of X or Y is too short (Length(X/Y)<N/M)", _state);
    ae_assert(isfinitevector(x, n, _state)&&isfinitevector(y, m, _state), "Spline2DBuildBicubicV: X or Y contains NaN or Infinite value", _state);
    k = n*m*d;
    ae_assert(f.cnt>=k, "Spline2DBuildBicubicV: length of F is too short (Length(F)<N*M*D)", _state);
    ae_assert(isfinitevector(&f, k, _state), "Spline2DBuildBicubicV: F contains NaN or Infinite value", _state);
    
    /*
     * Fill interpolant:
     *  F[0]...F[N*M*D-1]:
     *      f(i,j) table. f(0,0), f(0, 1), f(0,2) and so on...
     *  F[N*M*D]...F[2*N*M*D-1]:
     *      df(i,j)/dx table.
     *  F[2*N*M*D]...F[3*N*M*D-1]:
     *      df(i,j)/dy table.
     *  F[3*N*M*D]...F[4*N*M*D-1]:
     *      d2f(i,j)/dxdy table.
     */
    c->d = d;
    c->n = n;
    c->m = m;
    c->stype = -3;
    c->hasmissingcells = ae_false;
    k = 4*k;
    ae_vector_set_length(&c->x, c->n, _state);
    ae_vector_set_length(&c->y, c->m, _state);
    ae_vector_set_length(&c->f, k, _state);
    ae_matrix_set_length(&tf, c->m, c->n, _state);
    for(i=0; i<=c->n-1; i++)
    {
        c->x.ptr.p_double[i] = x->ptr.p_double[i];
    }
    for(i=0; i<=c->m-1; i++)
    {
        c->y.ptr.p_double[i] = y->ptr.p_double[i];
    }
    
    /*
     * Sort points
     */
    for(j=0; j<=c->n-1; j++)
    {
        k = j;
        for(i=j+1; i<=c->n-1; i++)
        {
            if( ae_fp_less(c->x.ptr.p_double[i],c->x.ptr.p_double[k]) )
            {
                k = i;
            }
        }
        if( k!=j )
        {
            for(i=0; i<=c->m-1; i++)
            {
                for(di=0; di<=c->d-1; di++)
                {
                    t = f.ptr.p_double[c->d*(i*c->n+j)+di];
                    f.ptr.p_double[c->d*(i*c->n+j)+di] = f.ptr.p_double[c->d*(i*c->n+k)+di];
                    f.ptr.p_double[c->d*(i*c->n+k)+di] = t;
                }
            }
            t = c->x.ptr.p_double[j];
            c->x.ptr.p_double[j] = c->x.ptr.p_double[k];
            c->x.ptr.p_double[k] = t;
        }
    }
    for(i=0; i<=c->m-1; i++)
    {
        k = i;
        for(j=i+1; j<=c->m-1; j++)
        {
            if( ae_fp_less(c->y.ptr.p_double[j],c->y.ptr.p_double[k]) )
            {
                k = j;
            }
        }
        if( k!=i )
        {
            for(j=0; j<=c->n-1; j++)
            {
                for(di=0; di<=c->d-1; di++)
                {
                    t = f.ptr.p_double[c->d*(i*c->n+j)+di];
                    f.ptr.p_double[c->d*(i*c->n+j)+di] = f.ptr.p_double[c->d*(k*c->n+j)+di];
                    f.ptr.p_double[c->d*(k*c->n+j)+di] = t;
                }
            }
            t = c->y.ptr.p_double[i];
            c->y.ptr.p_double[i] = c->y.ptr.p_double[k];
            c->y.ptr.p_double[k] = t;
        }
    }
    for(di=0; di<=c->d-1; di++)
    {
        for(i=0; i<=c->m-1; i++)
        {
            for(j=0; j<=c->n-1; j++)
            {
                tf.ptr.pp_double[i][j] = f.ptr.p_double[c->d*(i*c->n+j)+di];
            }
        }
        spline2d_bicubiccalcderivatives(&tf, &c->x, &c->y, c->m, c->n, &dx, &dy, &dxy, _state);
        for(i=0; i<=c->m-1; i++)
        {
            for(j=0; j<=c->n-1; j++)
            {
                k = c->d*(i*c->n+j)+di;
                c->f.ptr.p_double[k] = tf.ptr.pp_double[i][j];
                c->f.ptr.p_double[c->n*c->m*c->d+k] = dx.ptr.pp_double[i][j];
                c->f.ptr.p_double[2*c->n*c->m*c->d+k] = dy.ptr.pp_double[i][j];
                c->f.ptr.p_double[3*c->n*c->m*c->d+k] = dxy.ptr.pp_double[i][j];
            }
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
This  subroutine builds bicubic vector-valued  spline,  with  some  spline
cells being missing due to missing nodes.

This function produces C2-continuous spline, i.e. the has smooth first and
second derivatives both inside spline cells and at the boundaries.

When the node (i,j) is missing, it means that: a) we don't  have  function
value at this point (elements of F[] are ignored), and  b)  we  don't need
spline value at cells adjacent to the node (i,j), i.e. up to 4 spline cells
will be dropped. An attempt to compute spline value at  the  missing  cell
will return NAN.

It is important to  understand  that  this  subroutine  does  NOT  support
interpolation on scattered grids. It allows us to drop some nodes, but  at
the cost of making a "hole in the spline" around this point. If  you  want
function  that   can   "fill  the  gap",  use  RBF  or  another  scattered
interpolation method.

The  intended  usage  for  this  subroutine  are  regularly  sampled,  but
non-rectangular datasets.

Input parameters:
    X   -   spline abscissas, array[0..N-1]
    Y   -   spline ordinates, array[0..M-1]
    F   -   function values, array[0..M*N*D-1]:
            * first D elements store D values at (X[0],Y[0])
            * next D elements store D values at (X[1],Y[0])
            * general form - D function values at (X[i],Y[j]) are stored
              at F[D*(J*N+I)...D*(J*N+I)+D-1].
            * missing values are ignored
    Missing array[M*N], Missing[J*N+I]=True means that corresponding entries
            of F[] are missing nodes.
    M,N -   grid size, M>=2, N>=2
    D   -   vector dimension, D>=1

Output parameters:
    C   -   spline interpolant

  -- ALGLIB PROJECT --
     Copyright 27.06.2022 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildbicubicmissing(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     /* Real    */ const ae_vector* _f,
     /* Boolean */ const ae_vector* missing,
     ae_int_t d,
     spline2dinterpolant* c,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector f;

    ae_frame_make(_state, &_frame_block);
    memset(&f, 0, sizeof(f));
    ae_vector_init_copy(&f, _f, _state, ae_true);
    _spline2dinterpolant_clear(c);

    spline2dbuildbicubicmissingbuf(x, n, y, m, &f, missing, d, c, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This  subroutine builds bicubic vector-valued  spline,  with  some  spline
cells being missing due to missing nodes.

Buffered version  of  Spline2DBuildBicubicMissing()  which  reuses  memory
previously allocated in C as much as possible.

  -- ALGLIB PROJECT --
     Copyright 27.06.2022 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildbicubicmissingbuf(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     /* Real    */ const ae_vector* _f,
     /* Boolean */ const ae_vector* missing,
     ae_int_t d,
     spline2dinterpolant* c,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector f;
    ae_matrix tf;
    ae_matrix dx;
    ae_matrix dy;
    ae_matrix dxy;
    double t;
    ae_bool tb;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t di;
    ae_int_t i0;

    ae_frame_make(_state, &_frame_block);
    memset(&f, 0, sizeof(f));
    memset(&tf, 0, sizeof(tf));
    memset(&dx, 0, sizeof(dx));
    memset(&dy, 0, sizeof(dy));
    memset(&dxy, 0, sizeof(dxy));
    ae_vector_init_copy(&f, _f, _state, ae_true);
    ae_matrix_init(&tf, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&dx, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&dy, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&dxy, 0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>=2, "Spline2DBuildBicubicMissing: N is less than 2", _state);
    ae_assert(m>=2, "Spline2DBuildBicubicMissing: M is less than 2", _state);
    ae_assert(d>=1, "Spline2DBuildBicubicMissing: invalid argument D (D<1)", _state);
    ae_assert(x->cnt>=n&&y->cnt>=m, "Spline2DBuildBicubicMissing: length of X or Y is too short (Length(X/Y)<N/M)", _state);
    ae_assert(isfinitevector(x, n, _state)&&isfinitevector(y, m, _state), "Spline2DBuildBicubicMissing: X or Y contains NaN or Infinite value", _state);
    k = n*m*d;
    ae_assert(f.cnt>=k, "Spline2DBuildBicubicMissing: length of F is too short (Length(F)<N*M*D)", _state);
    ae_assert(missing->cnt>=n*m, "Spline2DBuildBicubicMissing: Missing[] is shorter than M*N", _state);
    for(i=0; i<=k-1; i++)
    {
        if( !missing->ptr.p_bool[i/d]&&!ae_isfinite(f.ptr.p_double[i], _state) )
        {
            ae_assert(ae_false, "Spline2DBuildBicubicMissing: F[] contains NAN or INF in its non-missing entries", _state);
        }
    }
    
    /*
     * Fill interpolant:
     *  F[0]...F[N*M*D-1]:
     *      f(i,j) table. f(0,0), f(0, 1), f(0,2) and so on...
     *  F[N*M*D]...F[2*N*M*D-1]:
     *      df(i,j)/dx table.
     *  F[2*N*M*D]...F[3*N*M*D-1]:
     *      df(i,j)/dy table.
     *  F[3*N*M*D]...F[4*N*M*D-1]:
     *      d2f(i,j)/dxdy table.
     */
    c->d = d;
    c->n = n;
    c->m = m;
    c->stype = -3;
    c->hasmissingcells = ae_true;
    ae_vector_set_length(&c->x, c->n, _state);
    ae_vector_set_length(&c->y, c->m, _state);
    rsetallocv(4*k, 0.0, &c->f, _state);
    bcopyallocv(c->n*c->m, missing, &c->ismissingnode, _state);
    ae_matrix_set_length(&tf, c->m, c->n, _state);
    for(i=0; i<=c->n-1; i++)
    {
        c->x.ptr.p_double[i] = x->ptr.p_double[i];
    }
    for(i=0; i<=c->m-1; i++)
    {
        c->y.ptr.p_double[i] = y->ptr.p_double[i];
    }
    for(i=0; i<=k-1; i++)
    {
        if( !missing->ptr.p_bool[i/d] )
        {
            c->f.ptr.p_double[i] = f.ptr.p_double[i];
        }
    }
    
    /*
     * Sort points
     */
    for(j=0; j<=c->n-1; j++)
    {
        k = j;
        for(i=j+1; i<=c->n-1; i++)
        {
            if( ae_fp_less(c->x.ptr.p_double[i],c->x.ptr.p_double[k]) )
            {
                k = i;
            }
        }
        if( k!=j )
        {
            for(i=0; i<=c->m-1; i++)
            {
                for(i0=0; i0<=c->d-1; i0++)
                {
                    t = c->f.ptr.p_double[c->d*(i*c->n+j)+i0];
                    c->f.ptr.p_double[c->d*(i*c->n+j)+i0] = c->f.ptr.p_double[c->d*(i*c->n+k)+i0];
                    c->f.ptr.p_double[c->d*(i*c->n+k)+i0] = t;
                }
                tb = c->ismissingnode.ptr.p_bool[i*c->n+j];
                c->ismissingnode.ptr.p_bool[i*c->n+j] = c->ismissingnode.ptr.p_bool[i*c->n+k];
                c->ismissingnode.ptr.p_bool[i*c->n+k] = tb;
            }
            t = c->x.ptr.p_double[j];
            c->x.ptr.p_double[j] = c->x.ptr.p_double[k];
            c->x.ptr.p_double[k] = t;
        }
    }
    for(i=0; i<=c->m-1; i++)
    {
        k = i;
        for(j=i+1; j<=c->m-1; j++)
        {
            if( ae_fp_less(c->y.ptr.p_double[j],c->y.ptr.p_double[k]) )
            {
                k = j;
            }
        }
        if( k!=i )
        {
            for(j=0; j<=c->n-1; j++)
            {
                for(i0=0; i0<=c->d-1; i0++)
                {
                    t = c->f.ptr.p_double[c->d*(i*c->n+j)+i0];
                    c->f.ptr.p_double[c->d*(i*c->n+j)+i0] = c->f.ptr.p_double[c->d*(k*c->n+j)+i0];
                    c->f.ptr.p_double[c->d*(k*c->n+j)+i0] = t;
                }
                tb = c->ismissingnode.ptr.p_bool[i*c->n+j];
                c->ismissingnode.ptr.p_bool[i*c->n+j] = c->ismissingnode.ptr.p_bool[k*c->n+j];
                c->ismissingnode.ptr.p_bool[k*c->n+j] = tb;
            }
            t = c->y.ptr.p_double[i];
            c->y.ptr.p_double[i] = c->y.ptr.p_double[k];
            c->y.ptr.p_double[k] = t;
        }
    }
    
    /*
     * 1. Determine cells that are not missing. A cell is non-missing if it has four non-missing
     *    nodes at its corners.
     * 2. Normalize IsMissingNode[] array - all isolated points that are not part of some non-missing
     *    cell are marked as missing too
     */
    bsetallocv((c->m-1)*(c->n-1), ae_true, &c->ismissingcell, _state);
    for(i=0; i<=c->m-2; i++)
    {
        for(j=0; j<=c->n-2; j++)
        {
            if( ((!c->ismissingnode.ptr.p_bool[i*c->n+j]&&!c->ismissingnode.ptr.p_bool[(i+1)*c->n+j])&&!c->ismissingnode.ptr.p_bool[i*c->n+(j+1)])&&!c->ismissingnode.ptr.p_bool[(i+1)*c->n+(j+1)] )
            {
                c->ismissingcell.ptr.p_bool[i*(c->n-1)+j] = ae_false;
            }
        }
    }
    bsetv(c->m*c->n, ae_true, &c->ismissingnode, _state);
    for(i=0; i<=c->m-2; i++)
    {
        for(j=0; j<=c->n-2; j++)
        {
            if( !c->ismissingcell.ptr.p_bool[i*(c->n-1)+j] )
            {
                c->ismissingnode.ptr.p_bool[i*c->n+j] = ae_false;
                c->ismissingnode.ptr.p_bool[(i+1)*c->n+j] = ae_false;
                c->ismissingnode.ptr.p_bool[i*c->n+(j+1)] = ae_false;
                c->ismissingnode.ptr.p_bool[(i+1)*c->n+(j+1)] = ae_false;
            }
        }
    }
    for(di=0; di<=c->d-1; di++)
    {
        for(i=0; i<=c->m-1; i++)
        {
            for(j=0; j<=c->n-1; j++)
            {
                tf.ptr.pp_double[i][j] = c->f.ptr.p_double[c->d*(i*c->n+j)+di];
            }
        }
        spline2d_bicubiccalcderivativesmissing(&tf, &c->ismissingnode, &c->x, &c->y, c->m, c->n, &dx, &dy, &dxy, _state);
        for(i=0; i<=c->m-1; i++)
        {
            for(j=0; j<=c->n-1; j++)
            {
                k = c->d*(i*c->n+j)+di;
                c->f.ptr.p_double[k] = tf.ptr.pp_double[i][j];
                c->f.ptr.p_double[c->n*c->m*c->d+k] = dx.ptr.pp_double[i][j];
                c->f.ptr.p_double[2*c->n*c->m*c->d+k] = dy.ptr.pp_double[i][j];
                c->f.ptr.p_double[3*c->n*c->m*c->d+k] = dxy.ptr.pp_double[i][j];
            }
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
This subroutine unpacks two-dimensional spline into the coefficients table

Input parameters:
    C   -   spline interpolant.

Result:
    M, N-   grid size (x-axis and y-axis)
    D   -   number of components
    Tbl -   coefficients table, unpacked format,
            D - components: [0..(N-1)*(M-1)*D-1, 0..20].
            For T=0..D-1 (component index), I = 0...N-2 (x index),
            J=0..M-2 (y index):
                K :=  T + I*D + J*D*(N-1)
                
                K-th row stores decomposition for T-th component of the
                vector-valued function
                
                Tbl[K,0] = X[i]
                Tbl[K,1] = X[i+1]
                Tbl[K,2] = Y[j]
                Tbl[K,3] = Y[j+1]
                Tbl[K,4] = C00
                Tbl[K,5] = C01
                Tbl[K,6] = C02
                Tbl[K,7] = C03
                Tbl[K,8] = C10
                Tbl[K,9] = C11
                ...
                Tbl[K,19] = C33
                Tbl[K,20] = 1 if the cell is present, 0 if the cell is missing.
                            In the latter case Tbl[4..19] are exactly zero.
            On each grid square spline is equals to:
                S(x) = SUM(c[i,j]*(t^i)*(u^j), i=0..3, j=0..3)
                t = x-x[j]
                u = y-y[i]

  -- ALGLIB PROJECT --
     Copyright 16.04.2012 by Bochkanov Sergey
*************************************************************************/
void spline2dunpackv(const spline2dinterpolant* c,
     ae_int_t* m,
     ae_int_t* n,
     ae_int_t* d,
     /* Real    */ ae_matrix* tbl,
     ae_state *_state)
{
    ae_int_t k;
    ae_int_t p;
    ae_int_t ci;
    ae_int_t cj;
    ae_int_t s1;
    ae_int_t s2;
    ae_int_t s3;
    ae_int_t s4;
    ae_int_t sfx;
    ae_int_t sfy;
    ae_int_t sfxy;
    double y1;
    double y2;
    double y3;
    double y4;
    double dt;
    double du;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k0;

    *m = 0;
    *n = 0;
    *d = 0;
    ae_matrix_clear(tbl);

    ae_assert(c->stype==-3||c->stype==-1, "Spline2DUnpackV: incorrect C (incorrect parameter C.SType)", _state);
    *n = c->n;
    *m = c->m;
    *d = c->d;
    rsetallocm((*n-1)*(*m-1)*(*d), 21, 0.0, tbl, _state);
    sfx = *n*(*m)*(*d);
    sfy = 2*(*n)*(*m)*(*d);
    sfxy = 3*(*n)*(*m)*(*d);
    for(i=0; i<=*m-2; i++)
    {
        for(j=0; j<=*n-2; j++)
        {
            for(k=0; k<=*d-1; k++)
            {
                p = *d*(i*(*n-1)+j)+k;
                
                /*
                 * Set up cell dimensions (always present)
                 */
                tbl->ptr.pp_double[p][0] = c->x.ptr.p_double[j];
                tbl->ptr.pp_double[p][1] = c->x.ptr.p_double[j+1];
                tbl->ptr.pp_double[p][2] = c->y.ptr.p_double[i];
                tbl->ptr.pp_double[p][3] = c->y.ptr.p_double[i+1];
                dt = (double)1/(tbl->ptr.pp_double[p][1]-tbl->ptr.pp_double[p][0]);
                du = (double)1/(tbl->ptr.pp_double[p][3]-tbl->ptr.pp_double[p][2]);
                
                /*
                 * Skip cell if it is missing
                 */
                if( c->hasmissingcells&&c->ismissingcell.ptr.p_bool[i*(c->n-1)+j] )
                {
                    continue;
                }
                tbl->ptr.pp_double[p][20] = (double)(1);
                
                /*
                 * Bilinear interpolation: output coefficients
                 */
                if( c->stype==-1 )
                {
                    for(k0=4; k0<=19; k0++)
                    {
                        tbl->ptr.pp_double[p][k0] = (double)(0);
                    }
                    y1 = c->f.ptr.p_double[*d*(*n*i+j)+k];
                    y2 = c->f.ptr.p_double[*d*(*n*i+(j+1))+k];
                    y3 = c->f.ptr.p_double[*d*(*n*(i+1)+(j+1))+k];
                    y4 = c->f.ptr.p_double[*d*(*n*(i+1)+j)+k];
                    tbl->ptr.pp_double[p][4] = y1;
                    tbl->ptr.pp_double[p][4+1*4+0] = y2-y1;
                    tbl->ptr.pp_double[p][4+0*4+1] = y4-y1;
                    tbl->ptr.pp_double[p][4+1*4+1] = y3-y2-y4+y1;
                }
                
                /*
                 * Bicubic interpolation: output coefficients
                 */
                if( c->stype==-3 )
                {
                    s1 = *d*(*n*i+j)+k;
                    s2 = *d*(*n*i+(j+1))+k;
                    s3 = *d*(*n*(i+1)+(j+1))+k;
                    s4 = *d*(*n*(i+1)+j)+k;
                    tbl->ptr.pp_double[p][4+0*4+0] = c->f.ptr.p_double[s1];
                    tbl->ptr.pp_double[p][4+0*4+1] = c->f.ptr.p_double[sfy+s1]/du;
                    tbl->ptr.pp_double[p][4+0*4+2] = -(double)3*c->f.ptr.p_double[s1]+(double)3*c->f.ptr.p_double[s4]-(double)2*c->f.ptr.p_double[sfy+s1]/du-c->f.ptr.p_double[sfy+s4]/du;
                    tbl->ptr.pp_double[p][4+0*4+3] = (double)2*c->f.ptr.p_double[s1]-(double)2*c->f.ptr.p_double[s4]+c->f.ptr.p_double[sfy+s1]/du+c->f.ptr.p_double[sfy+s4]/du;
                    tbl->ptr.pp_double[p][4+1*4+0] = c->f.ptr.p_double[sfx+s1]/dt;
                    tbl->ptr.pp_double[p][4+1*4+1] = c->f.ptr.p_double[sfxy+s1]/(dt*du);
                    tbl->ptr.pp_double[p][4+1*4+2] = -(double)3*c->f.ptr.p_double[sfx+s1]/dt+(double)3*c->f.ptr.p_double[sfx+s4]/dt-(double)2*c->f.ptr.p_double[sfxy+s1]/(dt*du)-c->f.ptr.p_double[sfxy+s4]/(dt*du);
                    tbl->ptr.pp_double[p][4+1*4+3] = (double)2*c->f.ptr.p_double[sfx+s1]/dt-(double)2*c->f.ptr.p_double[sfx+s4]/dt+c->f.ptr.p_double[sfxy+s1]/(dt*du)+c->f.ptr.p_double[sfxy+s4]/(dt*du);
                    tbl->ptr.pp_double[p][4+2*4+0] = -(double)3*c->f.ptr.p_double[s1]+(double)3*c->f.ptr.p_double[s2]-(double)2*c->f.ptr.p_double[sfx+s1]/dt-c->f.ptr.p_double[sfx+s2]/dt;
                    tbl->ptr.pp_double[p][4+2*4+1] = -(double)3*c->f.ptr.p_double[sfy+s1]/du+(double)3*c->f.ptr.p_double[sfy+s2]/du-(double)2*c->f.ptr.p_double[sfxy+s1]/(dt*du)-c->f.ptr.p_double[sfxy+s2]/(dt*du);
                    tbl->ptr.pp_double[p][4+2*4+2] = (double)9*c->f.ptr.p_double[s1]-(double)9*c->f.ptr.p_double[s2]+(double)9*c->f.ptr.p_double[s3]-(double)9*c->f.ptr.p_double[s4]+(double)6*c->f.ptr.p_double[sfx+s1]/dt+(double)3*c->f.ptr.p_double[sfx+s2]/dt-(double)3*c->f.ptr.p_double[sfx+s3]/dt-(double)6*c->f.ptr.p_double[sfx+s4]/dt+(double)6*c->f.ptr.p_double[sfy+s1]/du-(double)6*c->f.ptr.p_double[sfy+s2]/du-(double)3*c->f.ptr.p_double[sfy+s3]/du+(double)3*c->f.ptr.p_double[sfy+s4]/du+(double)4*c->f.ptr.p_double[sfxy+s1]/(dt*du)+(double)2*c->f.ptr.p_double[sfxy+s2]/(dt*du)+c->f.ptr.p_double[sfxy+s3]/(dt*du)+(double)2*c->f.ptr.p_double[sfxy+s4]/(dt*du);
                    tbl->ptr.pp_double[p][4+2*4+3] = -(double)6*c->f.ptr.p_double[s1]+(double)6*c->f.ptr.p_double[s2]-(double)6*c->f.ptr.p_double[s3]+(double)6*c->f.ptr.p_double[s4]-(double)4*c->f.ptr.p_double[sfx+s1]/dt-(double)2*c->f.ptr.p_double[sfx+s2]/dt+(double)2*c->f.ptr.p_double[sfx+s3]/dt+(double)4*c->f.ptr.p_double[sfx+s4]/dt-(double)3*c->f.ptr.p_double[sfy+s1]/du+(double)3*c->f.ptr.p_double[sfy+s2]/du+(double)3*c->f.ptr.p_double[sfy+s3]/du-(double)3*c->f.ptr.p_double[sfy+s4]/du-(double)2*c->f.ptr.p_double[sfxy+s1]/(dt*du)-c->f.ptr.p_double[sfxy+s2]/(dt*du)-c->f.ptr.p_double[sfxy+s3]/(dt*du)-(double)2*c->f.ptr.p_double[sfxy+s4]/(dt*du);
                    tbl->ptr.pp_double[p][4+3*4+0] = (double)2*c->f.ptr.p_double[s1]-(double)2*c->f.ptr.p_double[s2]+c->f.ptr.p_double[sfx+s1]/dt+c->f.ptr.p_double[sfx+s2]/dt;
                    tbl->ptr.pp_double[p][4+3*4+1] = (double)2*c->f.ptr.p_double[sfy+s1]/du-(double)2*c->f.ptr.p_double[sfy+s2]/du+c->f.ptr.p_double[sfxy+s1]/(dt*du)+c->f.ptr.p_double[sfxy+s2]/(dt*du);
                    tbl->ptr.pp_double[p][4+3*4+2] = -(double)6*c->f.ptr.p_double[s1]+(double)6*c->f.ptr.p_double[s2]-(double)6*c->f.ptr.p_double[s3]+(double)6*c->f.ptr.p_double[s4]-(double)3*c->f.ptr.p_double[sfx+s1]/dt-(double)3*c->f.ptr.p_double[sfx+s2]/dt+(double)3*c->f.ptr.p_double[sfx+s3]/dt+(double)3*c->f.ptr.p_double[sfx+s4]/dt-(double)4*c->f.ptr.p_double[sfy+s1]/du+(double)4*c->f.ptr.p_double[sfy+s2]/du+(double)2*c->f.ptr.p_double[sfy+s3]/du-(double)2*c->f.ptr.p_double[sfy+s4]/du-(double)2*c->f.ptr.p_double[sfxy+s1]/(dt*du)-(double)2*c->f.ptr.p_double[sfxy+s2]/(dt*du)-c->f.ptr.p_double[sfxy+s3]/(dt*du)-c->f.ptr.p_double[sfxy+s4]/(dt*du);
                    tbl->ptr.pp_double[p][4+3*4+3] = (double)4*c->f.ptr.p_double[s1]-(double)4*c->f.ptr.p_double[s2]+(double)4*c->f.ptr.p_double[s3]-(double)4*c->f.ptr.p_double[s4]+(double)2*c->f.ptr.p_double[sfx+s1]/dt+(double)2*c->f.ptr.p_double[sfx+s2]/dt-(double)2*c->f.ptr.p_double[sfx+s3]/dt-(double)2*c->f.ptr.p_double[sfx+s4]/dt+(double)2*c->f.ptr.p_double[sfy+s1]/du-(double)2*c->f.ptr.p_double[sfy+s2]/du-(double)2*c->f.ptr.p_double[sfy+s3]/du+(double)2*c->f.ptr.p_double[sfy+s4]/du+c->f.ptr.p_double[sfxy+s1]/(dt*du)+c->f.ptr.p_double[sfxy+s2]/(dt*du)+c->f.ptr.p_double[sfxy+s3]/(dt*du)+c->f.ptr.p_double[sfxy+s4]/(dt*du);
                }
                
                /*
                 * Rescale Cij
                 */
                for(ci=0; ci<=3; ci++)
                {
                    for(cj=0; cj<=3; cj++)
                    {
                        tbl->ptr.pp_double[p][4+ci*4+cj] = tbl->ptr.pp_double[p][4+ci*4+cj]*ae_pow(dt, (double)(ci), _state)*ae_pow(du, (double)(cj), _state);
                    }
                }
            }
        }
    }
}


/*************************************************************************
This subroutine was deprecated in ALGLIB 3.6.0

We recommend you to switch  to  Spline2DBuildBilinearV(),  which  is  more
flexible and accepts its arguments in more convenient order.

  -- ALGLIB PROJECT --
     Copyright 05.07.2007 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildbilinear(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_matrix* f,
     ae_int_t m,
     ae_int_t n,
     spline2dinterpolant* c,
     ae_state *_state)
{
    double t;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;

    _spline2dinterpolant_clear(c);

    ae_assert(n>=2, "Spline2DBuildBilinear: N<2", _state);
    ae_assert(m>=2, "Spline2DBuildBilinear: M<2", _state);
    ae_assert(x->cnt>=n&&y->cnt>=m, "Spline2DBuildBilinear: length of X or Y is too short (Length(X/Y)<N/M)", _state);
    ae_assert(isfinitevector(x, n, _state)&&isfinitevector(y, m, _state), "Spline2DBuildBilinear: X or Y contains NaN or Infinite value", _state);
    ae_assert(f->rows>=m&&f->cols>=n, "Spline2DBuildBilinear: size of F is too small (rows(F)<M or cols(F)<N)", _state);
    ae_assert(apservisfinitematrix(f, m, n, _state), "Spline2DBuildBilinear: F contains NaN or Infinite value", _state);
    
    /*
     * Fill interpolant
     */
    c->n = n;
    c->m = m;
    c->d = 1;
    c->stype = -1;
    c->hasmissingcells = ae_false;
    ae_vector_set_length(&c->x, c->n, _state);
    ae_vector_set_length(&c->y, c->m, _state);
    ae_vector_set_length(&c->f, c->n*c->m, _state);
    for(i=0; i<=c->n-1; i++)
    {
        c->x.ptr.p_double[i] = x->ptr.p_double[i];
    }
    for(i=0; i<=c->m-1; i++)
    {
        c->y.ptr.p_double[i] = y->ptr.p_double[i];
    }
    for(i=0; i<=c->m-1; i++)
    {
        for(j=0; j<=c->n-1; j++)
        {
            c->f.ptr.p_double[i*c->n+j] = f->ptr.pp_double[i][j];
        }
    }
    
    /*
     * Sort points
     */
    for(j=0; j<=c->n-1; j++)
    {
        k = j;
        for(i=j+1; i<=c->n-1; i++)
        {
            if( ae_fp_less(c->x.ptr.p_double[i],c->x.ptr.p_double[k]) )
            {
                k = i;
            }
        }
        if( k!=j )
        {
            for(i=0; i<=c->m-1; i++)
            {
                t = c->f.ptr.p_double[i*c->n+j];
                c->f.ptr.p_double[i*c->n+j] = c->f.ptr.p_double[i*c->n+k];
                c->f.ptr.p_double[i*c->n+k] = t;
            }
            t = c->x.ptr.p_double[j];
            c->x.ptr.p_double[j] = c->x.ptr.p_double[k];
            c->x.ptr.p_double[k] = t;
        }
    }
    for(i=0; i<=c->m-1; i++)
    {
        k = i;
        for(j=i+1; j<=c->m-1; j++)
        {
            if( ae_fp_less(c->y.ptr.p_double[j],c->y.ptr.p_double[k]) )
            {
                k = j;
            }
        }
        if( k!=i )
        {
            for(j=0; j<=c->n-1; j++)
            {
                t = c->f.ptr.p_double[i*c->n+j];
                c->f.ptr.p_double[i*c->n+j] = c->f.ptr.p_double[k*c->n+j];
                c->f.ptr.p_double[k*c->n+j] = t;
            }
            t = c->y.ptr.p_double[i];
            c->y.ptr.p_double[i] = c->y.ptr.p_double[k];
            c->y.ptr.p_double[k] = t;
        }
    }
}


/*************************************************************************
This subroutine was deprecated in ALGLIB 3.6.0

We recommend you to switch  to  Spline2DBuildBicubicV(),  which  is  more
flexible and accepts its arguments in more convenient order.

  -- ALGLIB PROJECT --
     Copyright 05.07.2007 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildbicubic(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_matrix* _f,
     ae_int_t m,
     ae_int_t n,
     spline2dinterpolant* c,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix f;
    ae_int_t sfx;
    ae_int_t sfy;
    ae_int_t sfxy;
    ae_matrix dx;
    ae_matrix dy;
    ae_matrix dxy;
    double t;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;

    ae_frame_make(_state, &_frame_block);
    memset(&f, 0, sizeof(f));
    memset(&dx, 0, sizeof(dx));
    memset(&dy, 0, sizeof(dy));
    memset(&dxy, 0, sizeof(dxy));
    ae_matrix_init_copy(&f, _f, _state, ae_true);
    _spline2dinterpolant_clear(c);
    ae_matrix_init(&dx, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&dy, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&dxy, 0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>=2, "Spline2DBuildBicubicSpline: N<2", _state);
    ae_assert(m>=2, "Spline2DBuildBicubicSpline: M<2", _state);
    ae_assert(x->cnt>=n&&y->cnt>=m, "Spline2DBuildBicubic: length of X or Y is too short (Length(X/Y)<N/M)", _state);
    ae_assert(isfinitevector(x, n, _state)&&isfinitevector(y, m, _state), "Spline2DBuildBicubic: X or Y contains NaN or Infinite value", _state);
    ae_assert(f.rows>=m&&f.cols>=n, "Spline2DBuildBicubic: size of F is too small (rows(F)<M or cols(F)<N)", _state);
    ae_assert(apservisfinitematrix(&f, m, n, _state), "Spline2DBuildBicubic: F contains NaN or Infinite value", _state);
    
    /*
     * Fill interpolant:
     *  F[0]...F[N*M-1]:
     *      f(i,j) table. f(0,0), f(0, 1), f(0,2) and so on...
     *  F[N*M]...F[2*N*M-1]:
     *      df(i,j)/dx table.
     *  F[2*N*M]...F[3*N*M-1]:
     *      df(i,j)/dy table.
     *  F[3*N*M]...F[4*N*M-1]:
     *      d2f(i,j)/dxdy table.
     */
    c->d = 1;
    c->n = n;
    c->m = m;
    c->stype = -3;
    c->hasmissingcells = ae_false;
    sfx = c->n*c->m;
    sfy = 2*c->n*c->m;
    sfxy = 3*c->n*c->m;
    ae_vector_set_length(&c->x, c->n, _state);
    ae_vector_set_length(&c->y, c->m, _state);
    ae_vector_set_length(&c->f, 4*c->n*c->m, _state);
    for(i=0; i<=c->n-1; i++)
    {
        c->x.ptr.p_double[i] = x->ptr.p_double[i];
    }
    for(i=0; i<=c->m-1; i++)
    {
        c->y.ptr.p_double[i] = y->ptr.p_double[i];
    }
    
    /*
     * Sort points
     */
    for(j=0; j<=c->n-1; j++)
    {
        k = j;
        for(i=j+1; i<=c->n-1; i++)
        {
            if( ae_fp_less(c->x.ptr.p_double[i],c->x.ptr.p_double[k]) )
            {
                k = i;
            }
        }
        if( k!=j )
        {
            for(i=0; i<=c->m-1; i++)
            {
                t = f.ptr.pp_double[i][j];
                f.ptr.pp_double[i][j] = f.ptr.pp_double[i][k];
                f.ptr.pp_double[i][k] = t;
            }
            t = c->x.ptr.p_double[j];
            c->x.ptr.p_double[j] = c->x.ptr.p_double[k];
            c->x.ptr.p_double[k] = t;
        }
    }
    for(i=0; i<=c->m-1; i++)
    {
        k = i;
        for(j=i+1; j<=c->m-1; j++)
        {
            if( ae_fp_less(c->y.ptr.p_double[j],c->y.ptr.p_double[k]) )
            {
                k = j;
            }
        }
        if( k!=i )
        {
            for(j=0; j<=c->n-1; j++)
            {
                t = f.ptr.pp_double[i][j];
                f.ptr.pp_double[i][j] = f.ptr.pp_double[k][j];
                f.ptr.pp_double[k][j] = t;
            }
            t = c->y.ptr.p_double[i];
            c->y.ptr.p_double[i] = c->y.ptr.p_double[k];
            c->y.ptr.p_double[k] = t;
        }
    }
    spline2d_bicubiccalcderivatives(&f, &c->x, &c->y, c->m, c->n, &dx, &dy, &dxy, _state);
    for(i=0; i<=c->m-1; i++)
    {
        for(j=0; j<=c->n-1; j++)
        {
            k = i*c->n+j;
            c->f.ptr.p_double[k] = f.ptr.pp_double[i][j];
            c->f.ptr.p_double[sfx+k] = dx.ptr.pp_double[i][j];
            c->f.ptr.p_double[sfy+k] = dy.ptr.pp_double[i][j];
            c->f.ptr.p_double[sfxy+k] = dxy.ptr.pp_double[i][j];
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
This subroutine was deprecated in ALGLIB 3.6.0

We recommend you to switch  to  Spline2DUnpackV(),  which is more flexible
and accepts its arguments in more convenient order.

  -- ALGLIB PROJECT --
     Copyright 29.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline2dunpack(const spline2dinterpolant* c,
     ae_int_t* m,
     ae_int_t* n,
     /* Real    */ ae_matrix* tbl,
     ae_state *_state)
{
    ae_int_t k;
    ae_int_t p;
    ae_int_t ci;
    ae_int_t cj;
    ae_int_t s1;
    ae_int_t s2;
    ae_int_t s3;
    ae_int_t s4;
    ae_int_t sfx;
    ae_int_t sfy;
    ae_int_t sfxy;
    double y1;
    double y2;
    double y3;
    double y4;
    double dt;
    double du;
    ae_int_t i;
    ae_int_t j;

    *m = 0;
    *n = 0;
    ae_matrix_clear(tbl);

    ae_assert(c->stype==-3||c->stype==-1, "Spline2DUnpack: incorrect C (incorrect parameter C.SType)", _state);
    if( c->d!=1 )
    {
        *n = 0;
        *m = 0;
        return;
    }
    *n = c->n;
    *m = c->m;
    ae_matrix_set_length(tbl, (*n-1)*(*m-1), 20, _state);
    sfx = *n*(*m);
    sfy = 2*(*n)*(*m);
    sfxy = 3*(*n)*(*m);
    
    /*
     * Fill
     */
    for(i=0; i<=*m-2; i++)
    {
        for(j=0; j<=*n-2; j++)
        {
            p = i*(*n-1)+j;
            tbl->ptr.pp_double[p][0] = c->x.ptr.p_double[j];
            tbl->ptr.pp_double[p][1] = c->x.ptr.p_double[j+1];
            tbl->ptr.pp_double[p][2] = c->y.ptr.p_double[i];
            tbl->ptr.pp_double[p][3] = c->y.ptr.p_double[i+1];
            dt = (double)1/(tbl->ptr.pp_double[p][1]-tbl->ptr.pp_double[p][0]);
            du = (double)1/(tbl->ptr.pp_double[p][3]-tbl->ptr.pp_double[p][2]);
            
            /*
             * Bilinear interpolation
             */
            if( c->stype==-1 )
            {
                for(k=4; k<=19; k++)
                {
                    tbl->ptr.pp_double[p][k] = (double)(0);
                }
                y1 = c->f.ptr.p_double[*n*i+j];
                y2 = c->f.ptr.p_double[*n*i+(j+1)];
                y3 = c->f.ptr.p_double[*n*(i+1)+(j+1)];
                y4 = c->f.ptr.p_double[*n*(i+1)+j];
                tbl->ptr.pp_double[p][4] = y1;
                tbl->ptr.pp_double[p][4+1*4+0] = y2-y1;
                tbl->ptr.pp_double[p][4+0*4+1] = y4-y1;
                tbl->ptr.pp_double[p][4+1*4+1] = y3-y2-y4+y1;
            }
            
            /*
             * Bicubic interpolation
             */
            if( c->stype==-3 )
            {
                s1 = *n*i+j;
                s2 = *n*i+(j+1);
                s3 = *n*(i+1)+(j+1);
                s4 = *n*(i+1)+j;
                tbl->ptr.pp_double[p][4+0*4+0] = c->f.ptr.p_double[s1];
                tbl->ptr.pp_double[p][4+0*4+1] = c->f.ptr.p_double[sfy+s1]/du;
                tbl->ptr.pp_double[p][4+0*4+2] = -(double)3*c->f.ptr.p_double[s1]+(double)3*c->f.ptr.p_double[s4]-(double)2*c->f.ptr.p_double[sfy+s1]/du-c->f.ptr.p_double[sfy+s4]/du;
                tbl->ptr.pp_double[p][4+0*4+3] = (double)2*c->f.ptr.p_double[s1]-(double)2*c->f.ptr.p_double[s4]+c->f.ptr.p_double[sfy+s1]/du+c->f.ptr.p_double[sfy+s4]/du;
                tbl->ptr.pp_double[p][4+1*4+0] = c->f.ptr.p_double[sfx+s1]/dt;
                tbl->ptr.pp_double[p][4+1*4+1] = c->f.ptr.p_double[sfxy+s1]/(dt*du);
                tbl->ptr.pp_double[p][4+1*4+2] = -(double)3*c->f.ptr.p_double[sfx+s1]/dt+(double)3*c->f.ptr.p_double[sfx+s4]/dt-(double)2*c->f.ptr.p_double[sfxy+s1]/(dt*du)-c->f.ptr.p_double[sfxy+s4]/(dt*du);
                tbl->ptr.pp_double[p][4+1*4+3] = (double)2*c->f.ptr.p_double[sfx+s1]/dt-(double)2*c->f.ptr.p_double[sfx+s4]/dt+c->f.ptr.p_double[sfxy+s1]/(dt*du)+c->f.ptr.p_double[sfxy+s4]/(dt*du);
                tbl->ptr.pp_double[p][4+2*4+0] = -(double)3*c->f.ptr.p_double[s1]+(double)3*c->f.ptr.p_double[s2]-(double)2*c->f.ptr.p_double[sfx+s1]/dt-c->f.ptr.p_double[sfx+s2]/dt;
                tbl->ptr.pp_double[p][4+2*4+1] = -(double)3*c->f.ptr.p_double[sfy+s1]/du+(double)3*c->f.ptr.p_double[sfy+s2]/du-(double)2*c->f.ptr.p_double[sfxy+s1]/(dt*du)-c->f.ptr.p_double[sfxy+s2]/(dt*du);
                tbl->ptr.pp_double[p][4+2*4+2] = (double)9*c->f.ptr.p_double[s1]-(double)9*c->f.ptr.p_double[s2]+(double)9*c->f.ptr.p_double[s3]-(double)9*c->f.ptr.p_double[s4]+(double)6*c->f.ptr.p_double[sfx+s1]/dt+(double)3*c->f.ptr.p_double[sfx+s2]/dt-(double)3*c->f.ptr.p_double[sfx+s3]/dt-(double)6*c->f.ptr.p_double[sfx+s4]/dt+(double)6*c->f.ptr.p_double[sfy+s1]/du-(double)6*c->f.ptr.p_double[sfy+s2]/du-(double)3*c->f.ptr.p_double[sfy+s3]/du+(double)3*c->f.ptr.p_double[sfy+s4]/du+(double)4*c->f.ptr.p_double[sfxy+s1]/(dt*du)+(double)2*c->f.ptr.p_double[sfxy+s2]/(dt*du)+c->f.ptr.p_double[sfxy+s3]/(dt*du)+(double)2*c->f.ptr.p_double[sfxy+s4]/(dt*du);
                tbl->ptr.pp_double[p][4+2*4+3] = -(double)6*c->f.ptr.p_double[s1]+(double)6*c->f.ptr.p_double[s2]-(double)6*c->f.ptr.p_double[s3]+(double)6*c->f.ptr.p_double[s4]-(double)4*c->f.ptr.p_double[sfx+s1]/dt-(double)2*c->f.ptr.p_double[sfx+s2]/dt+(double)2*c->f.ptr.p_double[sfx+s3]/dt+(double)4*c->f.ptr.p_double[sfx+s4]/dt-(double)3*c->f.ptr.p_double[sfy+s1]/du+(double)3*c->f.ptr.p_double[sfy+s2]/du+(double)3*c->f.ptr.p_double[sfy+s3]/du-(double)3*c->f.ptr.p_double[sfy+s4]/du-(double)2*c->f.ptr.p_double[sfxy+s1]/(dt*du)-c->f.ptr.p_double[sfxy+s2]/(dt*du)-c->f.ptr.p_double[sfxy+s3]/(dt*du)-(double)2*c->f.ptr.p_double[sfxy+s4]/(dt*du);
                tbl->ptr.pp_double[p][4+3*4+0] = (double)2*c->f.ptr.p_double[s1]-(double)2*c->f.ptr.p_double[s2]+c->f.ptr.p_double[sfx+s1]/dt+c->f.ptr.p_double[sfx+s2]/dt;
                tbl->ptr.pp_double[p][4+3*4+1] = (double)2*c->f.ptr.p_double[sfy+s1]/du-(double)2*c->f.ptr.p_double[sfy+s2]/du+c->f.ptr.p_double[sfxy+s1]/(dt*du)+c->f.ptr.p_double[sfxy+s2]/(dt*du);
                tbl->ptr.pp_double[p][4+3*4+2] = -(double)6*c->f.ptr.p_double[s1]+(double)6*c->f.ptr.p_double[s2]-(double)6*c->f.ptr.p_double[s3]+(double)6*c->f.ptr.p_double[s4]-(double)3*c->f.ptr.p_double[sfx+s1]/dt-(double)3*c->f.ptr.p_double[sfx+s2]/dt+(double)3*c->f.ptr.p_double[sfx+s3]/dt+(double)3*c->f.ptr.p_double[sfx+s4]/dt-(double)4*c->f.ptr.p_double[sfy+s1]/du+(double)4*c->f.ptr.p_double[sfy+s2]/du+(double)2*c->f.ptr.p_double[sfy+s3]/du-(double)2*c->f.ptr.p_double[sfy+s4]/du-(double)2*c->f.ptr.p_double[sfxy+s1]/(dt*du)-(double)2*c->f.ptr.p_double[sfxy+s2]/(dt*du)-c->f.ptr.p_double[sfxy+s3]/(dt*du)-c->f.ptr.p_double[sfxy+s4]/(dt*du);
                tbl->ptr.pp_double[p][4+3*4+3] = (double)4*c->f.ptr.p_double[s1]-(double)4*c->f.ptr.p_double[s2]+(double)4*c->f.ptr.p_double[s3]-(double)4*c->f.ptr.p_double[s4]+(double)2*c->f.ptr.p_double[sfx+s1]/dt+(double)2*c->f.ptr.p_double[sfx+s2]/dt-(double)2*c->f.ptr.p_double[sfx+s3]/dt-(double)2*c->f.ptr.p_double[sfx+s4]/dt+(double)2*c->f.ptr.p_double[sfy+s1]/du-(double)2*c->f.ptr.p_double[sfy+s2]/du-(double)2*c->f.ptr.p_double[sfy+s3]/du+(double)2*c->f.ptr.p_double[sfy+s4]/du+c->f.ptr.p_double[sfxy+s1]/(dt*du)+c->f.ptr.p_double[sfxy+s2]/(dt*du)+c->f.ptr.p_double[sfxy+s3]/(dt*du)+c->f.ptr.p_double[sfxy+s4]/(dt*du);
            }
            
            /*
             * Rescale Cij
             */
            for(ci=0; ci<=3; ci++)
            {
                for(cj=0; cj<=3; cj++)
                {
                    tbl->ptr.pp_double[p][4+ci*4+cj] = tbl->ptr.pp_double[p][4+ci*4+cj]*ae_pow(dt, (double)(ci), _state)*ae_pow(du, (double)(cj), _state);
                }
            }
        }
    }
}


/*************************************************************************
This subroutine creates least squares solver used to  fit  2D  splines  to
irregularly sampled (scattered) data.

Solver object is used to perform spline fits as follows:
* solver object is created with spline2dbuildercreate() function
* dataset is added with spline2dbuildersetpoints() function
* fit area is chosen:
  * spline2dbuildersetarea()     - for user-defined area
  * spline2dbuildersetareaauto() - for automatically chosen area
* number of grid nodes is chosen with spline2dbuildersetgrid()
* prior term is chosen with one of the following functions:
  * spline2dbuildersetlinterm()   to set linear prior
  * spline2dbuildersetconstterm() to set constant prior
  * spline2dbuildersetzeroterm()  to set zero prior
  * spline2dbuildersetuserterm()  to set user-defined constant prior
* solver algorithm is chosen with either:
  * spline2dbuildersetalgoblocklls() - BlockLLS algorithm, medium-scale problems
  * spline2dbuildersetalgofastddm()  - FastDDM algorithm, large-scale problems
* finally, fitting itself is performed with spline2dfit() function.

Most of the steps above can be omitted,  solver  is  configured with  good
defaults. The minimum is to call:
* spline2dbuildercreate() to create solver object
* spline2dbuildersetpoints() to specify dataset
* spline2dbuildersetgrid() to tell how many nodes you need
* spline2dfit() to perform fit

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

INPUT PARAMETERS:
    D   -   positive number, number of Y-components: D=1 for simple scalar
            fit, D>1 for vector-valued spline fitting.
    
OUTPUT PARAMETERS:
    S   -   solver object

  -- ALGLIB PROJECT --
     Copyright 29.01.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildercreate(ae_int_t d,
     spline2dbuilder* state,
     ae_state *_state)
{

    _spline2dbuilder_clear(state);

    ae_assert(d>=1, "Spline2DBuilderCreate: D<=0", _state);
    
    /*
     * NOTES:
     *
     * 1. Prior term is set to linear one (good default option)
     * 2. Solver is set to BlockLLS - good enough for small-scale problems.
     * 3. Refinement rounds: 5; enough to get good convergence.
     */
    state->priorterm = 1;
    state->priortermval = (double)(0);
    state->areatype = 0;
    state->gridtype = 0;
    state->smoothing = 0.0;
    state->nlayers = 0;
    state->solvertype = 1;
    state->npoints = 0;
    state->d = d;
    state->sx = 1.0;
    state->sy = 1.0;
    state->lsqrcnt = 5;
    
    /*
     * Algorithm settings
     */
    state->adddegreeoffreedom = ae_true;
    state->maxcoresize = 16;
    state->interfacesize = 5;
}


/*************************************************************************
This function sets constant prior term (model is a sum of  bicubic  spline
and global prior, which can be linear, constant, user-defined  constant or
zero).

Constant prior term is determined by least squares fitting.

INPUT PARAMETERS:
    S       -   spline builder
    V       -   value for user-defined prior

  -- ALGLIB --
     Copyright 01.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetuserterm(spline2dbuilder* state,
     double v,
     ae_state *_state)
{


    ae_assert(ae_isfinite(v, _state), "Spline2DBuilderSetUserTerm: infinite/NAN value passed", _state);
    state->priorterm = 0;
    state->priortermval = v;
}


/*************************************************************************
This function sets linear prior term (model is a sum of bicubic spline and
global  prior,  which  can  be  linear, constant, user-defined constant or
zero).

Linear prior term is determined by least squares fitting.

INPUT PARAMETERS:
    S       -   spline builder

  -- ALGLIB --
     Copyright 01.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetlinterm(spline2dbuilder* state, ae_state *_state)
{


    state->priorterm = 1;
}


/*************************************************************************
This function sets constant prior term (model is a sum of  bicubic  spline
and global prior, which can be linear, constant, user-defined  constant or
zero).

Constant prior term is determined by least squares fitting.

INPUT PARAMETERS:
    S       -   spline builder

  -- ALGLIB --
     Copyright 01.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetconstterm(spline2dbuilder* state, ae_state *_state)
{


    state->priorterm = 2;
}


/*************************************************************************
This function sets zero prior term (model is a sum of bicubic  spline  and
global  prior,  which  can  be  linear, constant, user-defined constant or
zero).

INPUT PARAMETERS:
    S       -   spline builder

  -- ALGLIB --
     Copyright 01.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetzeroterm(spline2dbuilder* state, ae_state *_state)
{


    state->priorterm = 3;
}


/*************************************************************************
This function adds dataset to the builder object.

This function overrides results of the previous calls, i.e. multiple calls
of this function will result in only the last set being added.

INPUT PARAMETERS:
    S       -   spline 2D builder object
    XY      -   points, array[N,2+D]. One  row  corresponds to  one  point
                in the dataset. First 2  elements  are  coordinates,  next
                D  elements are function values. Array may  be larger than 
                specified, in  this  case  only leading [N,NX+NY] elements 
                will be used.
    N       -   number of points in the dataset

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetpoints(spline2dbuilder* state,
     /* Real    */ const ae_matrix* xy,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t ew;


    ae_assert(n>0, "Spline2DBuilderSetPoints: N<0", _state);
    ae_assert(xy->rows>=n, "Spline2DBuilderSetPoints: Rows(XY)<N", _state);
    ae_assert(xy->cols>=2+state->d, "Spline2DBuilderSetPoints: Cols(XY)<NX+NY", _state);
    ae_assert(apservisfinitematrix(xy, n, 2+state->d, _state), "Spline2DBuilderSetPoints: XY contains infinite or NaN values!", _state);
    state->npoints = n;
    ew = 2+state->d;
    rvectorsetlengthatleast(&state->xy, n*ew, _state);
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=ew-1; j++)
        {
            state->xy.ptr.p_double[i*ew+j] = xy->ptr.pp_double[i][j];
        }
    }
}


/*************************************************************************
This function sets area where 2D spline interpolant is built. "Auto" means
that area extent is determined automatically from dataset extent.

INPUT PARAMETERS:
    S       -   spline 2D builder object

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetareaauto(spline2dbuilder* state, ae_state *_state)
{


    state->areatype = 0;
}


/*************************************************************************
This  function  sets  area  where  2D  spline  interpolant  is   built  to
user-defined one: [XA,XB]*[YA,YB]

INPUT PARAMETERS:
    S       -   spline 2D builder object
    XA,XB   -   spatial extent in the first (X) dimension, XA<XB
    YA,YB   -   spatial extent in the second (Y) dimension, YA<YB

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetarea(spline2dbuilder* state,
     double xa,
     double xb,
     double ya,
     double yb,
     ae_state *_state)
{


    ae_assert(ae_isfinite(xa, _state), "Spline2DBuilderSetArea: XA is not finite", _state);
    ae_assert(ae_isfinite(xb, _state), "Spline2DBuilderSetArea: XB is not finite", _state);
    ae_assert(ae_isfinite(ya, _state), "Spline2DBuilderSetArea: YA is not finite", _state);
    ae_assert(ae_isfinite(yb, _state), "Spline2DBuilderSetArea: YB is not finite", _state);
    ae_assert(ae_fp_less(xa,xb), "Spline2DBuilderSetArea: XA>=XB", _state);
    ae_assert(ae_fp_less(ya,yb), "Spline2DBuilderSetArea: YA>=YB", _state);
    state->areatype = 1;
    state->xa = xa;
    state->xb = xb;
    state->ya = ya;
    state->yb = yb;
}


/*************************************************************************
This  function  sets  nodes  count  for  2D spline interpolant. Fitting is
performed on area defined with one of the "setarea"  functions;  this  one
sets number of nodes placed upon the fitting area.

INPUT PARAMETERS:
    S       -   spline 2D builder object
    KX      -   nodes count for the first (X) dimension; fitting  interval
                [XA,XB] is separated into KX-1 subintervals, with KX nodes
                created at the boundaries.
    KY      -   nodes count for the first (Y) dimension; fitting  interval
                [YA,YB] is separated into KY-1 subintervals, with KY nodes
                created at the boundaries.

NOTE: at  least  4  nodes  is  created in each dimension, so KX and KY are
      silently increased if needed.

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetgrid(spline2dbuilder* state,
     ae_int_t kx,
     ae_int_t ky,
     ae_state *_state)
{


    ae_assert(kx>0, "Spline2DBuilderSetGridSizePrecisely: KX<=0", _state);
    ae_assert(ky>0, "Spline2DBuilderSetGridSizePrecisely: KY<=0", _state);
    state->gridtype = 1;
    state->kx = ae_maxint(kx, 4, _state);
    state->ky = ae_maxint(ky, 4, _state);
}


/*************************************************************************
This  function  allows  you to choose least squares solver used to perform
fitting. This function sets solver algorithm to "FastDDM", which  performs
fast parallel fitting by splitting problem into smaller chunks and merging
results together.

Unlike BlockLLS, this solver produces merely C1 continuous models  because
domain decomposition part disrupts C2 continuity.

This solver is optimized for large-scale problems, starting  from  256x256
grids, and up to 10000x10000 grids. Of course, it will  work  for  smaller
grids too.

More detailed description of the algorithm is given below:
* algorithm generates hierarchy  of  nested  grids,  ranging  from  ~16x16
  (topmost "layer" of the model) to ~KX*KY one (final layer). Upper layers
  model global behavior of the function, lower layers are  used  to  model
  fine details. Moving from layer to layer doubles grid density.
* fitting  is  started  from  topmost  layer, subsequent layers are fitted
  using residuals from previous ones.
* user may choose to skip generation of upper layers and generate  only  a
  few bottom ones, which  will  result  in  much  better  performance  and
  parallelization efficiency, at the cost of algorithm inability to "patch"
  large holes in the dataset.
* every layer is regularized using progressively increasing regularization
  coefficient; thus, increasing  LambdaV  penalizes  fine  details  first,
  leaving lower frequencies almost intact for a while.
* after fitting is done, all layers are merged together into  one  bicubic
  spline
  
IMPORTANT: regularization coefficient used by  this  solver  is  different
           from the one used by  BlockLLS.  Latter  utilizes  nonlinearity
           penalty,  which  is  global  in  nature  (large  regularization
           results in global linear trend being  extracted);  this  solver
           uses another, localized form of penalty, which is suitable  for
           parallel processing.

Notes on memory and performance:
* memory requirements: most memory is consumed  during  modeling   of  the
  higher layers; ~[512*NPoints] bytes is required for a  model  with  full
  hierarchy of grids being generated. However, if you skip a  few  topmost
  layers, you will get nearly constant (wrt. points count and  grid  size)
  memory consumption.
* serial running time: O(K*K)+O(NPoints) for a KxK grid
* parallelism potential: good. You may get  nearly  linear  speed-up  when
  performing fitting with just a few layers. Adding more layers results in
  model becoming more global, which somewhat  reduces  efficiency  of  the
  parallel code.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

INPUT PARAMETERS:
    S       -   spline 2D builder object
    NLayers -   number of layers in the model:
                * NLayers>=1 means that up  to  chosen  number  of  bottom
                  layers is fitted
                * NLayers=0 means that maximum number of layers is  chosen
                  (according to current grid size)
                * NLayers<=-1 means that up to |NLayers| topmost layers is
                  skipped
                Recommendations:
                * good "default" value is 2 layers
                * you may need  more  layers,  if  your  dataset  is  very
                  irregular and you want to "patch"  large  holes.  For  a
                  grid step H (equal to AreaWidth/GridSize) you may expect
                  that last layer reproduces variations at distance H (and
                  can patch holes that wide); that higher  layers  operate
                  at distances 2*H, 4*H, 8*H and so on.
                * good value for "bullletproof" mode is  NLayers=0,  which
                  results in complete hierarchy of layers being generated.
    LambdaV -   regularization coefficient, chosen in such a way  that  it
                penalizes bottom layers (fine details) first.
                LambdaV>=0, zero value means that no penalty is applied.

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetalgofastddm(spline2dbuilder* state,
     ae_int_t nlayers,
     double lambdav,
     ae_state *_state)
{


    ae_assert(ae_isfinite(lambdav, _state), "Spline2DBuilderSetAlgoFastDDM: LambdaV is not finite value", _state);
    ae_assert(ae_fp_greater_eq(lambdav,(double)(0)), "Spline2DBuilderSetAlgoFastDDM: LambdaV<0", _state);
    state->solvertype = 3;
    state->nlayers = nlayers;
    state->smoothing = lambdav;
}


/*************************************************************************
This  function  allows  you to choose least squares solver used to perform
fitting. This function sets solver algorithm to "BlockLLS", which performs
least squares fitting  with  fast  sparse  direct  solver,  with  optional
nonsmoothness penalty being applied.

This solver produces C2-continuous spline.

Nonlinearity penalty has the following form:

                          [                                            ]
    P() ~ Lambda* integral[ (d2S/dx2)^2 + 2*(d2S/dxdy)^2 + (d2S/dy2)^2 ]dxdy
                          [                                            ]
                  
here integral is calculated over entire grid, and "~" means "proportional"
because integral is normalized after calcilation. Extremely  large  values
of Lambda result in linear fit being performed.

NOTE: this algorithm is the most robust and controllable one,  but  it  is
      limited by 512x512 grids and (say) up to 1.000.000 points.  However,
      ALGLIB has one more  spline  solver:  FastDDM  algorithm,  which  is
      intended for really large-scale problems (in 10M-100M range). FastDDM
      algorithm also has better parallelism properties.
      
More information on BlockLLS solver:
* memory requirements: ~[32*K^3+256*NPoints]  bytes  for  KxK  grid   with
  NPoints-sized dataset
* serial running time: O(K^4+NPoints)
* parallelism potential: limited. You may get some sublinear gain when
  working with large grids (K's in 256..512 range)

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

INPUT PARAMETERS:
    S       -   spline 2D builder object
    LambdaNS-   non-negative value:
                * positive value means that some smoothing is applied
                * zero value means  that  no  smoothing  is  applied,  and
                  corresponding entries of design matrix  are  numerically
                  zero and dropped from consideration.

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetalgoblocklls(spline2dbuilder* state,
     double lambdans,
     ae_state *_state)
{


    ae_assert(ae_isfinite(lambdans, _state), "Spline2DBuilderSetAlgoBlockLLS: LambdaNS is not finite value", _state);
    ae_assert(ae_fp_greater_eq(lambdans,(double)(0)), "Spline2DBuilderSetAlgoBlockLLS: LambdaNS<0", _state);
    state->solvertype = 1;
    state->smoothing = lambdans;
}


/*************************************************************************
This  function  allows  you to choose least squares solver used to perform
fitting. This function sets solver algorithm to "NaiveLLS".

IMPORTANT: NaiveLLS is NOT intended to be used in  real  life  code!  This
           algorithm solves problem by generated dense (K^2)x(K^2+NPoints)
           matrix and solves  linear  least  squares  problem  with  dense
           solver.
           
           It is here just  to  test  BlockLLS  against  reference  solver
           (and maybe for someone trying to compare well optimized  solver
           against straightforward approach to the LLS problem).

More information on naive LLS solver:
* memory requirements: ~[8*K^4+256*NPoints] bytes for KxK grid.
* serial running time: O(K^6+NPoints) for KxK grid
* when compared with BlockLLS,  NaiveLLS  has ~K  larger memory demand and
  ~K^2  larger running time.

INPUT PARAMETERS:
    S       -   spline 2D builder object
    LambdaNS-   nonsmoothness penalty

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dbuildersetalgonaivells(spline2dbuilder* state,
     double lambdans,
     ae_state *_state)
{


    ae_assert(ae_isfinite(lambdans, _state), "Spline2DBuilderSetAlgoBlockLLS: LambdaNS is not finite value", _state);
    ae_assert(ae_fp_greater_eq(lambdans,(double)(0)), "Spline2DBuilderSetAlgoBlockLLS: LambdaNS<0", _state);
    state->solvertype = 2;
    state->smoothing = lambdans;
}


/*************************************************************************
This function fits bicubic spline to current dataset, using current  area/
grid and current LLS solver.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

INPUT PARAMETERS:
    State   -   spline 2D builder object

OUTPUT PARAMETERS:
    S       -   2D spline, fit result
    Rep     -   fitting report, which provides some additional info  about
                errors, R2 coefficient and so on.

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dfit(spline2dbuilder* state,
     spline2dinterpolant* s,
     spline2dfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    double xa;
    double xb;
    double ya;
    double yb;
    double xaraw;
    double xbraw;
    double yaraw;
    double ybraw;
    ae_int_t kx;
    ae_int_t ky;
    double hx;
    double hy;
    double invhx;
    double invhy;
    ae_int_t gridexpansion;
    ae_int_t nzwidth;
    ae_int_t bfrad;
    ae_int_t npoints;
    ae_int_t d;
    ae_int_t ew;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double v;
    ae_int_t k0;
    ae_int_t k1;
    double vx;
    double vy;
    ae_int_t arows;
    ae_int_t acopied;
    ae_int_t basecasex;
    ae_int_t basecasey;
    double eps;
    ae_vector xywork;
    ae_matrix vterm;
    ae_vector tmpx;
    ae_vector tmpy;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector meany;
    ae_vector xyindex;
    ae_vector tmpi;
    spline1dinterpolant basis1;
    sparsematrix av;
    sparsematrix ah;
    spline2dxdesignmatrix xdesignmatrix;
    ae_vector z;
    spline2dblockllsbuf blockllsbuf;
    ae_int_t sfx;
    ae_int_t sfy;
    ae_int_t sfxy;
    double tss;
    ae_int_t dstidx;

    ae_frame_make(_state, &_frame_block);
    memset(&xywork, 0, sizeof(xywork));
    memset(&vterm, 0, sizeof(vterm));
    memset(&tmpx, 0, sizeof(tmpx));
    memset(&tmpy, 0, sizeof(tmpy));
    memset(&tmp0, 0, sizeof(tmp0));
    memset(&tmp1, 0, sizeof(tmp1));
    memset(&meany, 0, sizeof(meany));
    memset(&xyindex, 0, sizeof(xyindex));
    memset(&tmpi, 0, sizeof(tmpi));
    memset(&basis1, 0, sizeof(basis1));
    memset(&av, 0, sizeof(av));
    memset(&ah, 0, sizeof(ah));
    memset(&xdesignmatrix, 0, sizeof(xdesignmatrix));
    memset(&z, 0, sizeof(z));
    memset(&blockllsbuf, 0, sizeof(blockllsbuf));
    _spline2dinterpolant_clear(s);
    _spline2dfitreport_clear(rep);
    ae_vector_init(&xywork, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&vterm, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmpx, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmpy, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp1, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&meany, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xyindex, 0, DT_INT, _state, ae_true);
    ae_vector_init(&tmpi, 0, DT_INT, _state, ae_true);
    _spline1dinterpolant_init(&basis1, _state, ae_true);
    _sparsematrix_init(&av, _state, ae_true);
    _sparsematrix_init(&ah, _state, ae_true);
    _spline2dxdesignmatrix_init(&xdesignmatrix, _state, ae_true);
    ae_vector_init(&z, 0, DT_REAL, _state, ae_true);
    _spline2dblockllsbuf_init(&blockllsbuf, _state, ae_true);

    nzwidth = 4;
    bfrad = 2;
    npoints = state->npoints;
    d = state->d;
    ew = 2+d;
    
    /*
     * Integrity checks
     */
    ae_assert(ae_fp_eq(state->sx,(double)(1)), "Spline2DFit: integrity error", _state);
    ae_assert(ae_fp_eq(state->sy,(double)(1)), "Spline2DFit: integrity error", _state);
    
    /*
     * Determine actual area size and grid step
     *
     * NOTE: initialize vars by zeros in order to avoid spurious
     *       compiler warnings.
     */
    xa = (double)(0);
    xb = (double)(0);
    ya = (double)(0);
    yb = (double)(0);
    if( state->areatype==0 )
    {
        if( npoints>0 )
        {
            xa = state->xy.ptr.p_double[0];
            xb = state->xy.ptr.p_double[0];
            ya = state->xy.ptr.p_double[1];
            yb = state->xy.ptr.p_double[1];
            for(i=1; i<=npoints-1; i++)
            {
                xa = ae_minreal(xa, state->xy.ptr.p_double[i*ew+0], _state);
                xb = ae_maxreal(xb, state->xy.ptr.p_double[i*ew+0], _state);
                ya = ae_minreal(ya, state->xy.ptr.p_double[i*ew+1], _state);
                yb = ae_maxreal(yb, state->xy.ptr.p_double[i*ew+1], _state);
            }
        }
        else
        {
            xa = (double)(-1);
            xb = (double)(1);
            ya = (double)(-1);
            yb = (double)(1);
        }
    }
    else
    {
        if( state->areatype==1 )
        {
            xa = state->xa;
            xb = state->xb;
            ya = state->ya;
            yb = state->yb;
        }
        else
        {
            ae_assert(ae_false, "Assertion failed", _state);
        }
    }
    if( ae_fp_eq(xa,xb) )
    {
        v = xa;
        if( ae_fp_greater_eq(v,(double)(0)) )
        {
            xa = v/(double)2-(double)1;
            xb = v*(double)2+(double)1;
        }
        else
        {
            xa = v*(double)2-(double)1;
            xb = v/(double)2+(double)1;
        }
    }
    if( ae_fp_eq(ya,yb) )
    {
        v = ya;
        if( ae_fp_greater_eq(v,(double)(0)) )
        {
            ya = v/(double)2-(double)1;
            yb = v*(double)2+(double)1;
        }
        else
        {
            ya = v*(double)2-(double)1;
            yb = v/(double)2+(double)1;
        }
    }
    ae_assert(ae_fp_less(xa,xb), "Spline2DFit: integrity error", _state);
    ae_assert(ae_fp_less(ya,yb), "Spline2DFit: integrity error", _state);
    kx = 0;
    ky = 0;
    if( state->gridtype==0 )
    {
        kx = 4;
        ky = 4;
    }
    else
    {
        if( state->gridtype==1 )
        {
            kx = state->kx;
            ky = state->ky;
        }
        else
        {
            ae_assert(ae_false, "Assertion failed", _state);
        }
    }
    ae_assert(kx>0, "Spline2DFit: integrity error", _state);
    ae_assert(ky>0, "Spline2DFit: integrity error", _state);
    basecasex = -1;
    basecasey = -1;
    if( state->solvertype==3 )
    {
        
        /*
         * Large-scale solver with special requirements to grid size.
         */
        kx = ae_maxint(kx, nzwidth, _state);
        ky = ae_maxint(ky, nzwidth, _state);
        k = 1;
        while(imin2(kx, ky, _state)>state->maxcoresize+1)
        {
            kx = idivup(kx-1, 2, _state)+1;
            ky = idivup(ky-1, 2, _state)+1;
            k = k+1;
        }
        basecasex = kx-1;
        k0 = 1;
        while(kx>state->maxcoresize+1)
        {
            basecasex = idivup(kx-1, 2, _state);
            kx = basecasex+1;
            k0 = k0+1;
        }
        while(k0>1)
        {
            kx = (kx-1)*2+1;
            k0 = k0-1;
        }
        basecasey = ky-1;
        k1 = 1;
        while(ky>state->maxcoresize+1)
        {
            basecasey = idivup(ky-1, 2, _state);
            ky = basecasey+1;
            k1 = k1+1;
        }
        while(k1>1)
        {
            ky = (ky-1)*2+1;
            k1 = k1-1;
        }
        while(k>1)
        {
            kx = (kx-1)*2+1;
            ky = (ky-1)*2+1;
            k = k-1;
        }
        
        /*
         * Grid is NOT expanded. We have very strict requirements on
         * grid size, and we do not want to overcomplicate it by
         * playing with grid size in order to add one more degree of
         * freedom. It is not relevant for such large tasks.
         */
        gridexpansion = 0;
    }
    else
    {
        
        /*
         * Medium-scale solvers which are tolerant to grid size.
         */
        kx = ae_maxint(kx, nzwidth, _state);
        ky = ae_maxint(ky, nzwidth, _state);
        
        /*
         * Grid is expanded by 1 in order to add one more effective degree
         * of freedom to the spline. Having additional nodes outside of the
         * area allows us to emulate changes in the derivative at the bound
         * without having specialized "boundary" version of the basis function.
         */
        if( state->adddegreeoffreedom )
        {
            gridexpansion = 1;
        }
        else
        {
            gridexpansion = 0;
        }
    }
    hx = coalesce(xb-xa, 1.0, _state)/(double)(kx-1);
    hy = coalesce(yb-ya, 1.0, _state)/(double)(ky-1);
    invhx = (double)1/hx;
    invhy = (double)1/hy;
    
    /*
     * We determined "raw" grid size. Now perform a grid correction according
     * to current grid expansion size.
     */
    xaraw = xa;
    yaraw = ya;
    xbraw = xb;
    ybraw = yb;
    xa = xa-hx*(double)gridexpansion;
    ya = ya-hy*(double)gridexpansion;
    xb = xb+hx*(double)gridexpansion;
    yb = yb+hy*(double)gridexpansion;
    kx = kx+2*gridexpansion;
    ky = ky+2*gridexpansion;
    
    /*
     * Create output spline using transformed (unit-scale)
     * coordinates, fill by zero values
     */
    s->d = d;
    s->n = kx;
    s->m = ky;
    s->stype = -3;
    s->hasmissingcells = ae_false;
    sfx = s->n*s->m*d;
    sfy = 2*s->n*s->m*d;
    sfxy = 3*s->n*s->m*d;
    ae_vector_set_length(&s->x, s->n, _state);
    ae_vector_set_length(&s->y, s->m, _state);
    ae_vector_set_length(&s->f, 4*s->n*s->m*d, _state);
    for(i=0; i<=s->n-1; i++)
    {
        s->x.ptr.p_double[i] = (double)(i);
    }
    for(i=0; i<=s->m-1; i++)
    {
        s->y.ptr.p_double[i] = (double)(i);
    }
    for(i=0; i<=4*s->n*s->m*d-1; i++)
    {
        s->f.ptr.p_double[i] = 0.0;
    }
    
    /*
     * Create local copy of dataset (only points in the grid are copied;
     * we allow small step out of the grid, by Eps*H, in order to deal
     * with numerical rounding errors).
     *
     * An additional copy of Y-values is created at columns beyond 2+J;
     * it is preserved during all transformations. This copy is used
     * to calculate error-related metrics.
     *
     * Calculate mean(Y), TSS
     */
    ae_vector_set_length(&meany, d, _state);
    for(j=0; j<=d-1; j++)
    {
        meany.ptr.p_double[j] = (double)(0);
    }
    rvectorsetlengthatleast(&xywork, npoints*ew, _state);
    acopied = 0;
    eps = 1.0E-6;
    for(i=0; i<=npoints-1; i++)
    {
        vx = state->xy.ptr.p_double[i*ew+0];
        vy = state->xy.ptr.p_double[i*ew+1];
        if( ((ae_fp_less_eq(xaraw-eps*hx,vx)&&ae_fp_less_eq(vx,xbraw+eps*hx))&&ae_fp_less_eq(yaraw-eps*hy,vy))&&ae_fp_less_eq(vy,ybraw+eps*hy) )
        {
            xywork.ptr.p_double[acopied*ew+0] = (vx-xa)*invhx;
            xywork.ptr.p_double[acopied*ew+1] = (vy-ya)*invhy;
            for(j=0; j<=d-1; j++)
            {
                v = state->xy.ptr.p_double[i*ew+2+j];
                xywork.ptr.p_double[acopied*ew+2+j] = v;
                meany.ptr.p_double[j] = meany.ptr.p_double[j]+v;
            }
            acopied = acopied+1;
        }
    }
    npoints = acopied;
    for(j=0; j<=d-1; j++)
    {
        meany.ptr.p_double[j] = meany.ptr.p_double[j]/coalesce((double)(npoints), (double)(1), _state);
    }
    tss = 0.0;
    for(i=0; i<=npoints-1; i++)
    {
        for(j=0; j<=d-1; j++)
        {
            tss = tss+ae_sqr(xywork.ptr.p_double[i*ew+2+j]-meany.ptr.p_double[j], _state);
        }
    }
    tss = coalesce(tss, 1.0, _state);
    
    /*
     * Handle prior term.
     * Modify output spline.
     * Quick exit if dataset is empty.
     */
    buildpriorterm1(&xywork, npoints, 2, d, state->priorterm, state->priortermval, &vterm, _state);
    if( npoints==0 )
    {
        
        /*
         * Quick exit
         */
        for(k=0; k<=s->n*s->m-1; k++)
        {
            k0 = k%s->n;
            k1 = k/s->n;
            for(j=0; j<=d-1; j++)
            {
                dstidx = d*(k1*s->n+k0)+j;
                s->f.ptr.p_double[dstidx] = s->f.ptr.p_double[dstidx]+vterm.ptr.pp_double[j][0]*s->x.ptr.p_double[k0]+vterm.ptr.pp_double[j][1]*s->y.ptr.p_double[k1]+vterm.ptr.pp_double[j][2];
                s->f.ptr.p_double[sfx+dstidx] = s->f.ptr.p_double[sfx+dstidx]+vterm.ptr.pp_double[j][0];
                s->f.ptr.p_double[sfy+dstidx] = s->f.ptr.p_double[sfy+dstidx]+vterm.ptr.pp_double[j][1];
            }
        }
        for(i=0; i<=s->n-1; i++)
        {
            s->x.ptr.p_double[i] = s->x.ptr.p_double[i]*hx+xa;
        }
        for(i=0; i<=s->m-1; i++)
        {
            s->y.ptr.p_double[i] = s->y.ptr.p_double[i]*hy+ya;
        }
        for(i=0; i<=s->n*s->m*d-1; i++)
        {
            s->f.ptr.p_double[sfx+i] = s->f.ptr.p_double[sfx+i]*invhx;
            s->f.ptr.p_double[sfy+i] = s->f.ptr.p_double[sfy+i]*invhy;
            s->f.ptr.p_double[sfxy+i] = s->f.ptr.p_double[sfxy+i]*invhx*invhy;
        }
        rep->rmserror = (double)(0);
        rep->avgerror = (double)(0);
        rep->maxerror = (double)(0);
        rep->r2 = 1.0;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Build 1D compact basis function
     * Generate design matrix
     */
    ae_vector_set_length(&tmpx, 7, _state);
    ae_vector_set_length(&tmpy, 7, _state);
    tmpx.ptr.p_double[0] = (double)(-3);
    tmpx.ptr.p_double[1] = (double)(-2);
    tmpx.ptr.p_double[2] = (double)(-1);
    tmpx.ptr.p_double[3] = (double)(0);
    tmpx.ptr.p_double[4] = (double)(1);
    tmpx.ptr.p_double[5] = (double)(2);
    tmpx.ptr.p_double[6] = (double)(3);
    tmpy.ptr.p_double[0] = (double)(0);
    tmpy.ptr.p_double[1] = (double)(0);
    tmpy.ptr.p_double[2] = (double)1/(double)12;
    tmpy.ptr.p_double[3] = (double)2/(double)6;
    tmpy.ptr.p_double[4] = (double)1/(double)12;
    tmpy.ptr.p_double[5] = (double)(0);
    tmpy.ptr.p_double[6] = (double)(0);
    spline1dbuildcubic(&tmpx, &tmpy, tmpx.cnt, 2, 0.0, 2, 0.0, &basis1, _state);
    
    /*
     * Solve.
     * Update spline.
     */
    if( state->solvertype==1 )
    {
        
        /*
         * BlockLLS
         */
        spline2d_reorderdatasetandbuildindex(&xywork, npoints, d, &tmp0, 0, kx, ky, &xyindex, &tmpi, _state);
        spline2d_xdesigngenerate(&xywork, &xyindex, 0, kx, kx, 0, ky, ky, d, spline2d_lambdaregblocklls, state->smoothing, &basis1, &xdesignmatrix, _state);
        spline2d_blockllsfit(&xdesignmatrix, state->lsqrcnt, &z, rep, tss, &blockllsbuf, _state);
        spline2d_updatesplinetable(&z, kx, ky, d, &basis1, bfrad, &s->f, s->m, s->n, 1, _state);
    }
    else
    {
        if( state->solvertype==2 )
        {
            
            /*
             * NaiveLLS, reference implementation
             */
            spline2d_generatedesignmatrix(&xywork, npoints, d, kx, ky, state->smoothing, spline2d_lambdaregblocklls, &basis1, &av, &ah, &arows, _state);
            spline2d_naivellsfit(&av, &ah, arows, &xywork, kx, ky, npoints, d, state->lsqrcnt, &z, rep, tss, _state);
            spline2d_updatesplinetable(&z, kx, ky, d, &basis1, bfrad, &s->f, s->m, s->n, 1, _state);
        }
        else
        {
            if( state->solvertype==3 )
            {
                
                /*
                 * FastDDM method
                 */
                ae_assert(basecasex>0, "Spline2DFit: integrity error", _state);
                ae_assert(basecasey>0, "Spline2DFit: integrity error", _state);
                spline2d_fastddmfit(&xywork, npoints, d, kx, ky, basecasex, basecasey, state->maxcoresize, state->interfacesize, state->nlayers, state->smoothing, state->lsqrcnt, &basis1, s, rep, tss, _state);
            }
            else
            {
                ae_assert(ae_false, "Spline2DFit: integrity error", _state);
            }
        }
    }
    
    /*
     * Append prior term.
     * Transform spline to original coordinates
     */
    for(k=0; k<=s->n*s->m-1; k++)
    {
        k0 = k%s->n;
        k1 = k/s->n;
        for(j=0; j<=d-1; j++)
        {
            dstidx = d*(k1*s->n+k0)+j;
            s->f.ptr.p_double[dstidx] = s->f.ptr.p_double[dstidx]+vterm.ptr.pp_double[j][0]*s->x.ptr.p_double[k0]+vterm.ptr.pp_double[j][1]*s->y.ptr.p_double[k1]+vterm.ptr.pp_double[j][2];
            s->f.ptr.p_double[sfx+dstidx] = s->f.ptr.p_double[sfx+dstidx]+vterm.ptr.pp_double[j][0];
            s->f.ptr.p_double[sfy+dstidx] = s->f.ptr.p_double[sfy+dstidx]+vterm.ptr.pp_double[j][1];
        }
    }
    for(i=0; i<=s->n-1; i++)
    {
        s->x.ptr.p_double[i] = s->x.ptr.p_double[i]*hx+xa;
    }
    for(i=0; i<=s->m-1; i++)
    {
        s->y.ptr.p_double[i] = s->y.ptr.p_double[i]*hy+ya;
    }
    for(i=0; i<=s->n*s->m*d-1; i++)
    {
        s->f.ptr.p_double[sfx+i] = s->f.ptr.p_double[sfx+i]*invhx;
        s->f.ptr.p_double[sfy+i] = s->f.ptr.p_double[sfy+i]*invhy;
        s->f.ptr.p_double[sfxy+i] = s->f.ptr.p_double[sfxy+i]*invhx*invhy;
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Serializer: allocation

  -- ALGLIB --
     Copyright 28.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dalloc(ae_serializer* s,
     const spline2dinterpolant* spline,
     ae_state *_state)
{


    
    /*
     * Which spline 2D format to use - V1 (no missing nodes) or V2 (missing nodes)?
     */
    if( !spline->hasmissingcells )
    {
        
        /*
         * V1 format
         */
        ae_serializer_alloc_entry(s);
        
        /*
         * Data
         */
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        allocrealarray(s, &spline->x, -1, _state);
        allocrealarray(s, &spline->y, -1, _state);
        allocrealarray(s, &spline->f, -1, _state);
    }
    else
    {
        
        /*
         * V2 format
         */
        ae_serializer_alloc_entry(s);
        
        /*
         * Data
         */
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        ae_serializer_alloc_entry(s);
        allocrealarray(s, &spline->x, -1, _state);
        allocrealarray(s, &spline->y, -1, _state);
        allocrealarray(s, &spline->f, -1, _state);
        allocbooleanarray(s, &spline->ismissingnode, -1, _state);
        allocbooleanarray(s, &spline->ismissingcell, -1, _state);
    }
}


/*************************************************************************
Serializer: serialization

  -- ALGLIB --
     Copyright 28.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dserialize(ae_serializer* s,
     const spline2dinterpolant* spline,
     ae_state *_state)
{


    
    /*
     * Which spline 2D format to use - V1 (no missing nodes) or V2 (missing nodes)?
     */
    if( !spline->hasmissingcells )
    {
        
        /*
         * V1 format
         */
        ae_serializer_serialize_int(s, getspline2dserializationcode(_state), _state);
        
        /*
         * Data
         */
        ae_serializer_serialize_int(s, spline->stype, _state);
        ae_serializer_serialize_int(s, spline->n, _state);
        ae_serializer_serialize_int(s, spline->m, _state);
        ae_serializer_serialize_int(s, spline->d, _state);
        serializerealarray(s, &spline->x, -1, _state);
        serializerealarray(s, &spline->y, -1, _state);
        serializerealarray(s, &spline->f, -1, _state);
    }
    else
    {
        
        /*
         * V2 format
         */
        ae_serializer_serialize_int(s, getspline2dwithmissingnodesserializationcode(_state), _state);
        
        /*
         * Data
         */
        ae_serializer_serialize_int(s, spline->stype, _state);
        ae_serializer_serialize_int(s, spline->n, _state);
        ae_serializer_serialize_int(s, spline->m, _state);
        ae_serializer_serialize_int(s, spline->d, _state);
        serializerealarray(s, &spline->x, -1, _state);
        serializerealarray(s, &spline->y, -1, _state);
        serializerealarray(s, &spline->f, -1, _state);
        serializebooleanarray(s, &spline->ismissingnode, -1, _state);
        serializebooleanarray(s, &spline->ismissingcell, -1, _state);
    }
}


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 28.02.2018 by Bochkanov Sergey
*************************************************************************/
void spline2dunserialize(ae_serializer* s,
     spline2dinterpolant* spline,
     ae_state *_state)
{
    ae_int_t scode;

    _spline2dinterpolant_clear(spline);

    
    /*
     * Header
     */
    ae_serializer_unserialize_int(s, &scode, _state);
    ae_assert(scode==getspline2dserializationcode(_state)||scode==getspline2dwithmissingnodesserializationcode(_state), "Spline2DUnserialize: stream header corrupted", _state);
    
    /*
     * Data
     */
    if( scode==getspline2dserializationcode(_state) )
    {
        ae_serializer_unserialize_int(s, &spline->stype, _state);
        ae_serializer_unserialize_int(s, &spline->n, _state);
        ae_serializer_unserialize_int(s, &spline->m, _state);
        ae_serializer_unserialize_int(s, &spline->d, _state);
        unserializerealarray(s, &spline->x, _state);
        unserializerealarray(s, &spline->y, _state);
        unserializerealarray(s, &spline->f, _state);
        spline->hasmissingcells = ae_false;
    }
    else
    {
        ae_serializer_unserialize_int(s, &spline->stype, _state);
        ae_serializer_unserialize_int(s, &spline->n, _state);
        ae_serializer_unserialize_int(s, &spline->m, _state);
        ae_serializer_unserialize_int(s, &spline->d, _state);
        unserializerealarray(s, &spline->x, _state);
        unserializerealarray(s, &spline->y, _state);
        unserializerealarray(s, &spline->f, _state);
        unserializebooleanarray(s, &spline->ismissingnode, _state);
        unserializebooleanarray(s, &spline->ismissingcell, _state);
        spline->hasmissingcells = ae_true;
    }
}


/*************************************************************************
Internal subroutine.
Calculation of the first derivatives and the cross-derivative.
*************************************************************************/
static void spline2d_bicubiccalcderivatives(/* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* dx,
     /* Real    */ ae_matrix* dy,
     /* Real    */ ae_matrix* dxy,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_vector xt;
    ae_vector ft;
    double s;
    double ds;
    double d2s;
    spline1dinterpolant c;

    ae_frame_make(_state, &_frame_block);
    memset(&xt, 0, sizeof(xt));
    memset(&ft, 0, sizeof(ft));
    memset(&c, 0, sizeof(c));
    ae_matrix_clear(dx);
    ae_matrix_clear(dy);
    ae_matrix_clear(dxy);
    ae_vector_init(&xt, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ft, 0, DT_REAL, _state, ae_true);
    _spline1dinterpolant_init(&c, _state, ae_true);

    ae_matrix_set_length(dx, m, n, _state);
    ae_matrix_set_length(dy, m, n, _state);
    ae_matrix_set_length(dxy, m, n, _state);
    
    /*
     * dF/dX
     */
    ae_vector_set_length(&xt, n, _state);
    ae_vector_set_length(&ft, n, _state);
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            xt.ptr.p_double[j] = x->ptr.p_double[j];
            ft.ptr.p_double[j] = a->ptr.pp_double[i][j];
        }
        spline1dbuildcubic(&xt, &ft, n, 0, 0.0, 0, 0.0, &c, _state);
        for(j=0; j<=n-1; j++)
        {
            spline1ddiff(&c, x->ptr.p_double[j], &s, &ds, &d2s, _state);
            dx->ptr.pp_double[i][j] = ds;
        }
    }
    
    /*
     * dF/dY
     */
    ae_vector_set_length(&xt, m, _state);
    ae_vector_set_length(&ft, m, _state);
    for(j=0; j<=n-1; j++)
    {
        for(i=0; i<=m-1; i++)
        {
            xt.ptr.p_double[i] = y->ptr.p_double[i];
            ft.ptr.p_double[i] = a->ptr.pp_double[i][j];
        }
        spline1dbuildcubic(&xt, &ft, m, 0, 0.0, 0, 0.0, &c, _state);
        for(i=0; i<=m-1; i++)
        {
            spline1ddiff(&c, y->ptr.p_double[i], &s, &ds, &d2s, _state);
            dy->ptr.pp_double[i][j] = ds;
        }
    }
    
    /*
     * d2F/dXdY
     */
    ae_vector_set_length(&xt, n, _state);
    ae_vector_set_length(&ft, n, _state);
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            xt.ptr.p_double[j] = x->ptr.p_double[j];
            ft.ptr.p_double[j] = dy->ptr.pp_double[i][j];
        }
        spline1dbuildcubic(&xt, &ft, n, 0, 0.0, 0, 0.0, &c, _state);
        for(j=0; j<=n-1; j++)
        {
            spline1ddiff(&c, x->ptr.p_double[j], &s, &ds, &d2s, _state);
            dxy->ptr.pp_double[i][j] = ds;
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Internal subroutine.

Calculation of the first derivatives and the cross-derivative  subject  to
a missing values map in IsMissingNode[].

The missing values map should be normalized, i.e. any isolated point  that
is not part of some non-missing cell should be marked as missing too.
*************************************************************************/
static void spline2d_bicubiccalcderivativesmissing(/* Real    */ const ae_matrix* a,
     /* Boolean */ const ae_vector* ismissingnode,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* dx,
     /* Real    */ ae_matrix* dy,
     /* Real    */ ae_matrix* dxy,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k1;
    ae_int_t k2;
    ae_vector xt;
    ae_vector ft;
    ae_vector t;
    ae_vector b;
    spline1dinterpolant c;
    double s;
    double ds;
    double d2s;

    ae_frame_make(_state, &_frame_block);
    memset(&xt, 0, sizeof(xt));
    memset(&ft, 0, sizeof(ft));
    memset(&t, 0, sizeof(t));
    memset(&b, 0, sizeof(b));
    memset(&c, 0, sizeof(c));
    ae_matrix_clear(dx);
    ae_matrix_clear(dy);
    ae_matrix_clear(dxy);
    ae_vector_init(&xt, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ft, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&t, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&b, 0, DT_BOOL, _state, ae_true);
    _spline1dinterpolant_init(&c, _state, ae_true);

    ae_assert(m>=2, "BicubicCalcDerivativesMissing: internal error (M<2)", _state);
    ae_assert(n>=2, "BicubicCalcDerivativesMissing: internal error (N<2)", _state);
    
    /*
     * Allocate DX/DY/DXY and make initial fill by zeros
     */
    rsetallocm(m, n, 0.0, dx, _state);
    rsetallocm(m, n, 0.0, dy, _state);
    rsetallocm(m, n, 0.0, dxy, _state);
    
    /*
     * dF/dX
     */
    ballocv(n, &b, _state);
    ae_vector_set_length(&xt, n, _state);
    ae_vector_set_length(&ft, n, _state);
    ae_vector_set_length(&t, n, _state);
    for(i=0; i<=m-1; i++)
    {
        k1 = -1;
        k2 = -1;
        rcopyrv(n, a, i, &t, _state);
        for(j=0; j<=n-1; j++)
        {
            b.ptr.p_bool[j] = ismissingnode->ptr.p_bool[i*n+j];
        }
        while(spline2d_scanfornonmissingsegment(&b, n, &k1, &k2, _state))
        {
            ae_v_move(&xt.ptr.p_double[0], 1, &x->ptr.p_double[k1], 1, ae_v_len(0,k2-k1));
            ae_v_move(&ft.ptr.p_double[0], 1, &t.ptr.p_double[k1], 1, ae_v_len(0,k2-k1));
            spline1dbuildcubic(&xt, &ft, k2-k1+1, 0, 0.0, 0, 0.0, &c, _state);
            for(j=0; j<=k2-k1; j++)
            {
                spline1ddiff(&c, x->ptr.p_double[k1+j], &s, &ds, &d2s, _state);
                dx->ptr.pp_double[i][k1+j] = ds;
            }
        }
    }
    
    /*
     * dF/dY
     */
    ballocv(m, &b, _state);
    ae_vector_set_length(&xt, m, _state);
    ae_vector_set_length(&ft, m, _state);
    ae_vector_set_length(&t, m, _state);
    for(j=0; j<=n-1; j++)
    {
        k1 = -1;
        k2 = -1;
        ae_v_move(&t.ptr.p_double[0], 1, &a->ptr.pp_double[0][j], a->stride, ae_v_len(0,m-1));
        for(i=0; i<=m-1; i++)
        {
            b.ptr.p_bool[i] = ismissingnode->ptr.p_bool[i*n+j];
        }
        while(spline2d_scanfornonmissingsegment(&b, m, &k1, &k2, _state))
        {
            ae_v_move(&xt.ptr.p_double[0], 1, &y->ptr.p_double[k1], 1, ae_v_len(0,k2-k1));
            ae_v_move(&ft.ptr.p_double[0], 1, &t.ptr.p_double[k1], 1, ae_v_len(0,k2-k1));
            spline1dbuildcubic(&xt, &ft, k2-k1+1, 0, 0.0, 0, 0.0, &c, _state);
            for(i=0; i<=k2-k1; i++)
            {
                spline1ddiff(&c, y->ptr.p_double[k1+i], &s, &ds, &d2s, _state);
                dy->ptr.pp_double[k1+i][j] = ds;
            }
        }
    }
    
    /*
     * d2F/dXdY
     */
    ballocv(n, &b, _state);
    ae_vector_set_length(&xt, n, _state);
    ae_vector_set_length(&ft, n, _state);
    ae_vector_set_length(&t, n, _state);
    for(i=0; i<=m-1; i++)
    {
        k1 = -1;
        k2 = -1;
        ae_v_move(&t.ptr.p_double[0], 1, &dy->ptr.pp_double[i][0], 1, ae_v_len(0,n-1));
        for(j=0; j<=n-1; j++)
        {
            b.ptr.p_bool[j] = ismissingnode->ptr.p_bool[i*n+j];
        }
        while(spline2d_scanfornonmissingsegment(&b, n, &k1, &k2, _state))
        {
            ae_v_move(&xt.ptr.p_double[0], 1, &x->ptr.p_double[k1], 1, ae_v_len(0,k2-k1));
            ae_v_move(&ft.ptr.p_double[0], 1, &t.ptr.p_double[k1], 1, ae_v_len(0,k2-k1));
            spline1dbuildcubic(&xt, &ft, k2-k1+1, 0, 0.0, 0, 0.0, &c, _state);
            for(j=0; j<=k2-k1; j++)
            {
                spline1ddiff(&c, x->ptr.p_double[k1+j], &s, &ds, &d2s, _state);
                dxy->ptr.pp_double[i][k1+j] = ds;
            }
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Internal subroutine.
Scans array IsMissing[] for segment containing non-missing values.

On the first call I1=I2=-1.
Ater return from subsequent call either
* 0<=I1<I2<N, B[I1:I2] is non-missing; result is True
* I1=I2=N; result is False
*************************************************************************/
static ae_bool spline2d_scanfornonmissingsegment(/* Boolean */ const ae_vector* ismissing,
     ae_int_t n,
     ae_int_t* i1,
     ae_int_t* i2,
     ae_state *_state)
{
    ae_int_t i;
    ae_bool result;


    ae_assert(n>=2, "ScanForNonmissingSegment: internal error (N<2)", _state);
    ae_assert(*i1<=(*i2), "ScanForNonmissingSegment: internal error (I1>I2)", _state);
    result = ae_false;
    
    /*
     * Initial call: prepare and pass
     */
    if( *i1<0||*i2<0 )
    {
        *i1 = -1;
        *i2 = -1;
    }
    
    /*
     * Scan for the next segment
     */
    if( *i1<n&&*i2<n )
    {
        
        /*
         * scan for the segment's start
         */
        i = *i2+1;
        *i1 = n;
        *i2 = n;
        result = ae_false;
        for(;;)
        {
            if( i>=n )
            {
                return result;
            }
            if( !ismissing->ptr.p_bool[i] )
            {
                *i1 = i;
                break;
            }
            i = i+1;
        }
        
        /*
         * Scan for segment's end
         */
        for(;;)
        {
            if( i>=n )
            {
                *i2 = n-1;
                break;
            }
            if( ismissing->ptr.p_bool[i] )
            {
                *i2 = i-1;
                break;
            }
            i = i+1;
        }
        ae_assert(*i2>(*i1), "ScanForFiniteSegment: internal error (segment is too short)", _state);
        result = ae_true;
    }
    return result;
}


/*************************************************************************
This function generates design matrix for the problem (in fact, two design
matrices are generated: "vertical" one and transposed (horizontal) one.

INPUT PARAMETERS:
    XY          -   array[NPoints*(2+D)]; dataset after scaling  in  such
                    way that grid step is equal to 1.0 in both dimensions.
    NPoints     -   dataset size, NPoints>=1
    KX, KY      -   grid size, KX,KY>=4
    Smoothing   -   nonlinearity penalty coefficient, >=0
    LambdaReg   -   regularization coefficient, >=0
    Basis1      -   basis spline, expected to be non-zero only at [-2,+2]
    AV, AH      -   possibly preallocated buffers

OUTPUT PARAMETERS:
    AV          -   sparse matrix[ARows,KX*KY]; design matrix
    AH          -   transpose of AV
    ARows       -   number of rows in design matrix
    
  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static void spline2d_generatedesignmatrix(/* Real    */ const ae_vector* xy,
     ae_int_t npoints,
     ae_int_t d,
     ae_int_t kx,
     ae_int_t ky,
     double smoothing,
     double lambdareg,
     const spline1dinterpolant* basis1,
     sparsematrix* av,
     sparsematrix* ah,
     ae_int_t* arows,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t nzwidth;
    ae_int_t nzshift;
    ae_int_t ew;
    ae_int_t i;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t k0;
    ae_int_t k1;
    ae_int_t dstidx;
    double v;
    double v0;
    double v1;
    double v2;
    double w0;
    double w1;
    double w2;
    ae_vector crx;
    ae_vector cry;
    ae_vector nrs;
    ae_matrix d2x;
    ae_matrix d2y;
    ae_matrix dxy;

    ae_frame_make(_state, &_frame_block);
    memset(&crx, 0, sizeof(crx));
    memset(&cry, 0, sizeof(cry));
    memset(&nrs, 0, sizeof(nrs));
    memset(&d2x, 0, sizeof(d2x));
    memset(&d2y, 0, sizeof(d2y));
    memset(&dxy, 0, sizeof(dxy));
    *arows = 0;
    ae_vector_init(&crx, 0, DT_INT, _state, ae_true);
    ae_vector_init(&cry, 0, DT_INT, _state, ae_true);
    ae_vector_init(&nrs, 0, DT_INT, _state, ae_true);
    ae_matrix_init(&d2x, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&d2y, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&dxy, 0, 0, DT_REAL, _state, ae_true);

    nzwidth = 4;
    nzshift = 1;
    ae_assert(npoints>0, "Spline2DFit: integrity check failed", _state);
    ae_assert(kx>=nzwidth, "Spline2DFit: integrity check failed", _state);
    ae_assert(ky>=nzwidth, "Spline2DFit: integrity check failed", _state);
    ew = 2+d;
    
    /*
     * Determine canonical rectangle for every point. Every point of the dataset is
     * influenced by at most NZWidth*NZWidth basis functions, which form NZWidth*NZWidth
     * canonical rectangle.
     *
     * Thus, we have (KX-NZWidth+1)*(KY-NZWidth+1) overlapping canonical rectangles.
     * Assigning every point to its rectangle simplifies creation of sparse basis
     * matrix at the next steps.
     */
    ae_vector_set_length(&crx, npoints, _state);
    ae_vector_set_length(&cry, npoints, _state);
    for(i=0; i<=npoints-1; i++)
    {
        crx.ptr.p_int[i] = iboundval(ae_ifloor(xy->ptr.p_double[i*ew+0], _state)-nzshift, 0, kx-nzwidth, _state);
        cry.ptr.p_int[i] = iboundval(ae_ifloor(xy->ptr.p_double[i*ew+1], _state)-nzshift, 0, ky-nzwidth, _state);
    }
    
    /*
     * Create vertical and horizontal design matrices 
     */
    *arows = npoints+kx*ky;
    if( ae_fp_neq(smoothing,0.0) )
    {
        ae_assert(ae_fp_greater(smoothing,0.0), "Spline2DFit: integrity check failed", _state);
        *arows = *arows+3*(kx-2)*(ky-2);
    }
    ae_vector_set_length(&nrs, *arows, _state);
    dstidx = 0;
    for(i=0; i<=npoints-1; i++)
    {
        nrs.ptr.p_int[dstidx+i] = nzwidth*nzwidth;
    }
    dstidx = dstidx+npoints;
    for(i=0; i<=kx*ky-1; i++)
    {
        nrs.ptr.p_int[dstidx+i] = 1;
    }
    dstidx = dstidx+kx*ky;
    if( ae_fp_neq(smoothing,0.0) )
    {
        for(i=0; i<=3*(kx-2)*(ky-2)-1; i++)
        {
            nrs.ptr.p_int[dstidx+i] = 3*3;
        }
        dstidx = dstidx+3*(kx-2)*(ky-2);
    }
    ae_assert(dstidx==(*arows), "Spline2DFit: integrity check failed", _state);
    sparsecreatecrs(*arows, kx*ky, &nrs, av, _state);
    dstidx = 0;
    for(i=0; i<=npoints-1; i++)
    {
        for(j1=0; j1<=nzwidth-1; j1++)
        {
            for(j0=0; j0<=nzwidth-1; j0++)
            {
                v0 = spline1dcalc(basis1, xy->ptr.p_double[i*ew+0]-(double)(crx.ptr.p_int[i]+j0), _state);
                v1 = spline1dcalc(basis1, xy->ptr.p_double[i*ew+1]-(double)(cry.ptr.p_int[i]+j1), _state);
                sparseset(av, dstidx+i, (cry.ptr.p_int[i]+j1)*kx+(crx.ptr.p_int[i]+j0), v0*v1, _state);
            }
        }
    }
    dstidx = dstidx+npoints;
    for(i=0; i<=kx*ky-1; i++)
    {
        sparseset(av, dstidx+i, i, lambdareg, _state);
    }
    dstidx = dstidx+kx*ky;
    if( ae_fp_neq(smoothing,0.0) )
    {
        
        /*
         * Smoothing is applied. Because all grid nodes are same,
         * we apply same smoothing kernel, which is calculated only
         * once at the beginning of design matrix generation.
         */
        ae_matrix_set_length(&d2x, 3, 3, _state);
        ae_matrix_set_length(&d2y, 3, 3, _state);
        ae_matrix_set_length(&dxy, 3, 3, _state);
        for(j1=0; j1<=2; j1++)
        {
            for(j0=0; j0<=2; j0++)
            {
                d2x.ptr.pp_double[j0][j1] = 0.0;
                d2y.ptr.pp_double[j0][j1] = 0.0;
                dxy.ptr.pp_double[j0][j1] = 0.0;
            }
        }
        for(k1=0; k1<=2; k1++)
        {
            for(k0=0; k0<=2; k0++)
            {
                spline1ddiff(basis1, (double)(-(k0-1)), &v0, &v1, &v2, _state);
                spline1ddiff(basis1, (double)(-(k1-1)), &w0, &w1, &w2, _state);
                d2x.ptr.pp_double[k0][k1] = d2x.ptr.pp_double[k0][k1]+v2*w0;
                d2y.ptr.pp_double[k0][k1] = d2y.ptr.pp_double[k0][k1]+w2*v0;
                dxy.ptr.pp_double[k0][k1] = dxy.ptr.pp_double[k0][k1]+v1*w1;
            }
        }
        
        /*
         * Now, kernel is ready - apply it to all inner nodes of the grid.
         */
        for(j1=1; j1<=ky-2; j1++)
        {
            for(j0=1; j0<=kx-2; j0++)
            {
                
                /*
                 * d2F/dx2 term
                 */
                v = smoothing;
                for(k1=-1; k1<=1; k1++)
                {
                    for(k0=-1; k0<=1; k0++)
                    {
                        sparseset(av, dstidx, (j1+k1)*kx+(j0+k0), v*d2x.ptr.pp_double[1+k0][1+k1], _state);
                    }
                }
                dstidx = dstidx+1;
                
                /*
                 * d2F/dy2 term
                 */
                v = smoothing;
                for(k1=-1; k1<=1; k1++)
                {
                    for(k0=-1; k0<=1; k0++)
                    {
                        sparseset(av, dstidx, (j1+k1)*kx+(j0+k0), v*d2y.ptr.pp_double[1+k0][1+k1], _state);
                    }
                }
                dstidx = dstidx+1;
                
                /*
                 * 2*d2F/dxdy term
                 */
                v = ae_sqrt((double)(2), _state)*smoothing;
                for(k1=-1; k1<=1; k1++)
                {
                    for(k0=-1; k0<=1; k0++)
                    {
                        sparseset(av, dstidx, (j1+k1)*kx+(j0+k0), v*dxy.ptr.pp_double[1+k0][1+k1], _state);
                    }
                }
                dstidx = dstidx+1;
            }
        }
    }
    ae_assert(dstidx==(*arows), "Spline2DFit: integrity check failed", _state);
    sparsecopy(av, ah, _state);
    sparsetransposecrs(ah, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function updates table of spline values/derivatives using coefficients
for a layer of basis functions.
    
  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static void spline2d_updatesplinetable(/* Real    */ const ae_vector* z,
     ae_int_t kx,
     ae_int_t ky,
     ae_int_t d,
     const spline1dinterpolant* basis1,
     ae_int_t bfrad,
     /* Real    */ ae_vector* ftbl,
     ae_int_t m,
     ae_int_t n,
     ae_int_t scalexy,
     ae_state *_state)
{
    ae_int_t k;
    ae_int_t k0;
    ae_int_t k1;
    ae_int_t j;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t j0a;
    ae_int_t j0b;
    ae_int_t j1a;
    ae_int_t j1b;
    double v;
    double v0;
    double v1;
    double v01;
    double v11;
    double rdummy;
    ae_int_t dstidx;
    ae_int_t sfx;
    ae_int_t sfy;
    ae_int_t sfxy;
    double invscalexy;


    ae_assert(n==(kx-1)*scalexy+1, "Spline2DFit.UpdateSplineTable: integrity check failed", _state);
    ae_assert(m==(ky-1)*scalexy+1, "Spline2DFit.UpdateSplineTable: integrity check failed", _state);
    invscalexy = (double)1/(double)scalexy;
    sfx = n*m*d;
    sfy = 2*n*m*d;
    sfxy = 3*n*m*d;
    for(k=0; k<=kx*ky-1; k++)
    {
        k0 = k%kx;
        k1 = k/kx;
        j0a = iboundval(k0*scalexy-(bfrad*scalexy-1), 0, n-1, _state);
        j0b = iboundval(k0*scalexy+(bfrad*scalexy-1), 0, n-1, _state);
        j1a = iboundval(k1*scalexy-(bfrad*scalexy-1), 0, m-1, _state);
        j1b = iboundval(k1*scalexy+(bfrad*scalexy-1), 0, m-1, _state);
        for(j1=j1a; j1<=j1b; j1++)
        {
            spline1ddiff(basis1, (double)(j1-k1*scalexy)*invscalexy, &v1, &v11, &rdummy, _state);
            v11 = v11*invscalexy;
            for(j0=j0a; j0<=j0b; j0++)
            {
                spline1ddiff(basis1, (double)(j0-k0*scalexy)*invscalexy, &v0, &v01, &rdummy, _state);
                v01 = v01*invscalexy;
                for(j=0; j<=d-1; j++)
                {
                    dstidx = d*(j1*n+j0)+j;
                    v = z->ptr.p_double[j*kx*ky+k];
                    ftbl->ptr.p_double[dstidx] = ftbl->ptr.p_double[dstidx]+v0*v1*v;
                    ftbl->ptr.p_double[sfx+dstidx] = ftbl->ptr.p_double[sfx+dstidx]+v01*v1*v;
                    ftbl->ptr.p_double[sfy+dstidx] = ftbl->ptr.p_double[sfy+dstidx]+v0*v11*v;
                    ftbl->ptr.p_double[sfxy+dstidx] = ftbl->ptr.p_double[sfxy+dstidx]+v01*v11*v;
                }
            }
        }
    }
}


/*************************************************************************
This function performs fitting with FastDDM solver.
Internal function, never use it directly.

INPUT PARAMETERS:
    XY          -   array[NPoints*(2+D)], dataset; destroyed in process
    KX, KY      -   grid size
    TileSize    -   tile size
    InterfaceSize-  interface size
    NPoints     -   points count
    D           -   number of components in vector-valued spline, D>=1
    LSQRCnt     -   number of iterations, non-zero:
                    * LSQRCnt>0 means that specified amount of  preconditioned
                      LSQR  iterations  will  be  performed  to solve problem;
                      usually  we  need  2..5  its.  Recommended option - best
                      convergence and stability/quality.
                    * LSQRCnt<0 means that instead of LSQR  we  use  iterative
                      refinement on normal equations. Again, 2..5 its is enough.
    Basis1      -   basis spline, expected to be non-zero only at [-2,+2]
    Z           -   possibly preallocated buffer for solution
    Residuals   -   possibly preallocated buffer for residuals at dataset points
    Rep         -   report structure; fields which are not set by this function
                    are left intact
    TSS         -   total sum of squares; used to calculate R2
    

OUTPUT PARAMETERS:
    XY          -   destroyed in process
    Z           -   array[KX*KY*D], filled by solution; KX*KY coefficients
                    corresponding to each of D dimensions are stored contiguously.
    Rep         -   following fields are set:
                    * Rep.RMSError
                    * Rep.AvgError
                    * Rep.MaxError
                    * Rep.R2

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static void spline2d_fastddmfit(/* Real    */ ae_vector* xy,
     ae_int_t npoints,
     ae_int_t d,
     ae_int_t kx,
     ae_int_t ky,
     ae_int_t basecasex,
     ae_int_t basecasey,
     ae_int_t maxcoresize,
     ae_int_t interfacesize,
     ae_int_t nlayers,
     double smoothing,
     ae_int_t lsqrcnt,
     const spline1dinterpolant* basis1,
     spline2dinterpolant* spline,
     spline2dfitreport* rep,
     double tss,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t nzwidth;
    ae_int_t xew;
    ae_int_t ntotallayers;
    ae_int_t scaleidx;
    ae_int_t scalexy;
    double invscalexy;
    ae_int_t kxcur;
    ae_int_t kycur;
    ae_int_t tilescount0;
    ae_int_t tilescount1;
    double v;
    double rss;
    ae_vector yraw;
    ae_vector xyindex;
    ae_vector tmp0;
    ae_vector bufi;
    spline2dfastddmbuf seed;
    ae_shared_pool pool;
    spline2dxdesignmatrix xdesignmatrix;
    spline2dblockllsbuf blockllsbuf;
    spline2dfitreport dummyrep;

    ae_frame_make(_state, &_frame_block);
    memset(&yraw, 0, sizeof(yraw));
    memset(&xyindex, 0, sizeof(xyindex));
    memset(&tmp0, 0, sizeof(tmp0));
    memset(&bufi, 0, sizeof(bufi));
    memset(&seed, 0, sizeof(seed));
    memset(&pool, 0, sizeof(pool));
    memset(&xdesignmatrix, 0, sizeof(xdesignmatrix));
    memset(&blockllsbuf, 0, sizeof(blockllsbuf));
    memset(&dummyrep, 0, sizeof(dummyrep));
    ae_vector_init(&yraw, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xyindex, 0, DT_INT, _state, ae_true);
    ae_vector_init(&tmp0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bufi, 0, DT_INT, _state, ae_true);
    _spline2dfastddmbuf_init(&seed, _state, ae_true);
    ae_shared_pool_init(&pool, _state, ae_true);
    _spline2dxdesignmatrix_init(&xdesignmatrix, _state, ae_true);
    _spline2dblockllsbuf_init(&blockllsbuf, _state, ae_true);
    _spline2dfitreport_init(&dummyrep, _state, ae_true);

    
    /*
     * Dataset metrics and integrity checks
     */
    nzwidth = 4;
    xew = 2+d;
    ae_assert(maxcoresize>=2, "Spline2DFit: integrity check failed", _state);
    ae_assert(interfacesize>=1, "Spline2DFit: integrity check failed", _state);
    ae_assert(kx>=nzwidth, "Spline2DFit: integrity check failed", _state);
    ae_assert(ky>=nzwidth, "Spline2DFit: integrity check failed", _state);
    
    /*
     * Verify consistency of the grid size (KX,KY) with basecase sizes.
     * Determine full number of layers.
     */
    ae_assert(basecasex<=maxcoresize, "Spline2DFit: integrity error", _state);
    ae_assert(basecasey<=maxcoresize, "Spline2DFit: integrity error", _state);
    ntotallayers = 1;
    scalexy = 1;
    kxcur = kx;
    kycur = ky;
    while(kxcur>basecasex+1&&kycur>basecasey+1)
    {
        ae_assert(kxcur%2==1, "Spline2DFit: integrity error", _state);
        ae_assert(kycur%2==1, "Spline2DFit: integrity error", _state);
        kxcur = (kxcur-1)/2+1;
        kycur = (kycur-1)/2+1;
        scalexy = scalexy*2;
        inc(&ntotallayers, _state);
    }
    invscalexy = (double)1/(double)scalexy;
    ae_assert((kxcur<=maxcoresize+1&&kxcur==basecasex+1)||kxcur%basecasex==1, "Spline2DFit: integrity error", _state);
    ae_assert((kycur<=maxcoresize+1&&kycur==basecasey+1)||kycur%basecasey==1, "Spline2DFit: integrity error", _state);
    ae_assert(kxcur==basecasex+1||kycur==basecasey+1, "Spline2DFit: integrity error", _state);
    
    /*
     * Initial scaling of dataset.
     * Store original target values to YRaw.
     */
    rvectorsetlengthatleast(&yraw, npoints*d, _state);
    for(i=0; i<=npoints-1; i++)
    {
        xy->ptr.p_double[xew*i+0] = xy->ptr.p_double[xew*i+0]*invscalexy;
        xy->ptr.p_double[xew*i+1] = xy->ptr.p_double[xew*i+1]*invscalexy;
        for(j=0; j<=d-1; j++)
        {
            yraw.ptr.p_double[i*d+j] = xy->ptr.p_double[xew*i+2+j];
        }
    }
    kxcur = (kx-1)/scalexy+1;
    kycur = (ky-1)/scalexy+1;
    
    /*
     * Build initial dataset index; area is divided into (KXCur-1)*(KYCur-1)
     * cells, with contiguous storage of points in the same cell.
     * Iterate over different scales
     */
    ae_shared_pool_set_seed(&pool, &seed, (ae_int_t)sizeof(seed), (ae_constructor)_spline2dfastddmbuf_init, (ae_copy_constructor)_spline2dfastddmbuf_init_copy, (ae_destructor)_spline2dfastddmbuf_destroy, _state);
    spline2d_reorderdatasetandbuildindex(xy, npoints, d, &yraw, d, kxcur, kycur, &xyindex, &bufi, _state);
    for(scaleidx=ntotallayers-1; scaleidx>=0; scaleidx--)
    {
        if( (nlayers>0&&scaleidx<nlayers)||(nlayers<=0&&scaleidx<imax2(ntotallayers+nlayers, 1, _state)) )
        {
            
            /*
             * Fit current layer
             */
            ae_assert(kxcur%basecasex==1, "Spline2DFit: integrity error", _state);
            ae_assert(kycur%basecasey==1, "Spline2DFit: integrity error", _state);
            tilescount0 = kxcur/basecasex;
            tilescount1 = kycur/basecasey;
            spline2d_fastddmfitlayer(xy, d, scalexy, &xyindex, basecasex, 0, tilescount0, tilescount0, basecasey, 0, tilescount1, tilescount1, maxcoresize, interfacesize, lsqrcnt, spline2d_lambdaregfastddm+smoothing*ae_pow(spline2d_lambdadecay, (double)(scaleidx), _state), basis1, &pool, spline, _state);
            
            /*
             * Compute residuals and update XY
             */
            spline2d_computeresidualsfromscratch(xy, &yraw, npoints, d, scalexy, spline, _state);
        }
        
        /*
         * Move to the next level
         */
        if( scaleidx!=0 )
        {
            
            /*
             * Transform dataset (multply everything by 2.0) and refine grid.
             */
            kxcur = 2*kxcur-1;
            kycur = 2*kycur-1;
            scalexy = scalexy/2;
            invscalexy = (double)1/(double)scalexy;
            spline2d_rescaledatasetandrefineindex(xy, npoints, d, &yraw, d, kxcur, kycur, &xyindex, &bufi, _state);
            
            /*
             * Clear temporaries from previous round.
             *
             * We have to do it because upper layer of the multilevel spline
             * needs more memory then subsequent layers, and we want to free
             * this memory as soon as possible.
             */
            ae_shared_pool_clear_recycled(&pool, _state);
        }
    }
    
    /*
     * Post-check
     */
    ae_assert(kxcur==kx, "Spline2DFit: integrity check failed", _state);
    ae_assert(kycur==ky, "Spline2DFit: integrity check failed", _state);
    ae_assert(scalexy==1, "Spline2DFit: integrity check failed", _state);
    
    /*
     * Report
     */
    rep->rmserror = (double)(0);
    rep->avgerror = (double)(0);
    rep->maxerror = (double)(0);
    rss = 0.0;
    for(i=0; i<=npoints-1; i++)
    {
        for(j=0; j<=d-1; j++)
        {
            v = xy->ptr.p_double[i*xew+2+j];
            rss = rss+v*v;
            rep->rmserror = rep->rmserror+ae_sqr(v, _state);
            rep->avgerror = rep->avgerror+ae_fabs(v, _state);
            rep->maxerror = ae_maxreal(rep->maxerror, ae_fabs(v, _state), _state);
        }
    }
    rep->rmserror = ae_sqrt(rep->rmserror/coalesce((double)(npoints*d), 1.0, _state), _state);
    rep->avgerror = rep->avgerror/coalesce((double)(npoints*d), 1.0, _state);
    rep->r2 = 1.0-rss/coalesce(tss, 1.0, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Recursive fitting function for FastDDM algorithm.

Works with KX*KY grid, with KX=BasecaseX*TilesCountX+1 and KY=BasecaseY*TilesCountY+1,
which is partitioned into TilesCountX*TilesCountY tiles, each having size
BasecaseX*BasecaseY.

This function processes tiles in range [TileX0,TileX1)x[TileY0,TileY1) and
recursively divides this range until we move down to single tile, which
is processed with BlockLLS solver.

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static void spline2d_fastddmfitlayer(/* Real    */ const ae_vector* xy,
     ae_int_t d,
     ae_int_t scalexy,
     /* Integer */ const ae_vector* xyindex,
     ae_int_t basecasex,
     ae_int_t tilex0,
     ae_int_t tilex1,
     ae_int_t tilescountx,
     ae_int_t basecasey,
     ae_int_t tiley0,
     ae_int_t tiley1,
     ae_int_t tilescounty,
     ae_int_t maxcoresize,
     ae_int_t interfacesize,
     ae_int_t lsqrcnt,
     double lambdareg,
     const spline1dinterpolant* basis1,
     ae_shared_pool* pool,
     spline2dinterpolant* spline,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t kx;
    ae_int_t ky;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t bfrad;
    ae_int_t xa;
    ae_int_t xb;
    ae_int_t ya;
    ae_int_t yb;
    ae_int_t tile0;
    ae_int_t tile1;
    ae_int_t tilesize0;
    ae_int_t tilesize1;
    ae_int_t sfx;
    ae_int_t sfy;
    ae_int_t sfxy;
    double dummytss;
    double invscalexy;
    ae_int_t cnt0;
    ae_int_t cnt1;
    ae_int_t offs;
    double vs;
    double vsx;
    double vsy;
    double vsxx;
    double vsxy;
    double vsyy;
    spline2dfastddmbuf *buf;
    ae_smart_ptr _buf;

    ae_frame_make(_state, &_frame_block);
    memset(&_buf, 0, sizeof(_buf));
    ae_smart_ptr_init(&_buf, (void**)&buf, _state, ae_true);

    
    /*
     * Dataset metrics and fast integrity checks;
     * no code with side effects is allowed before parallel split.
     */
    bfrad = 2;
    invscalexy = (double)1/(double)scalexy;
    kx = basecasex*tilescountx+1;
    ky = basecasey*tilescounty+1;
    
    /*
     * Parallelism; because this function is intended for
     * large-scale problems, we always try to:
     * * invoke parallel execution mode
     * * activate spawn support
     */
    if( _trypexec_spline2d_fastddmfitlayer(xy,d,scalexy,xyindex,basecasex,tilex0,tilex1,tilescountx,basecasey,tiley0,tiley1,tilescounty,maxcoresize,interfacesize,lsqrcnt,lambdareg,basis1,pool,spline, _state) )
    {
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        ae_frame_leave(_state);
        return;
    }
    ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_true, _state);
    if( imax2(tiley1-tiley0, tilex1-tilex0, _state)>=2 )
    {
        if( tiley1-tiley0>tilex1-tilex0 )
        {
            
            /*
             * Split problem in Y dimension
             *
             * NOTE: recursive calls to FastDDMFitLayer() compute
             *       residuals in the inner cells defined by XYIndex[],
             *       but we still have to compute residuals for cells
             *       BETWEEN two recursive subdivisions of the task.
             */
            tiledsplit(tiley1-tiley0, 1, &j0, &j1, _state);
            _spawn_spline2d_fastddmfitlayer(xy, d, scalexy, xyindex, basecasex, tilex0, tilex1, tilescountx, basecasey, tiley0, tiley0+j0, tilescounty, maxcoresize, interfacesize, lsqrcnt, lambdareg, basis1, pool, spline, _child_tasks, _smp_enabled, _state);
            spline2d_fastddmfitlayer(xy, d, scalexy, xyindex, basecasex, tilex0, tilex1, tilescountx, basecasey, tiley0+j0, tiley1, tilescounty, maxcoresize, interfacesize, lsqrcnt, lambdareg, basis1, pool, spline, _state);
        }
        else
        {
            
            /*
             * Split problem in X dimension
             *
             * NOTE: recursive calls to FastDDMFitLayer() compute
             *       residuals in the inner cells defined by XYIndex[],
             *       but we still have to compute residuals for cells
             *       BETWEEN two recursive subdivisions of the task.
             */
            tiledsplit(tilex1-tilex0, 1, &j0, &j1, _state);
            _spawn_spline2d_fastddmfitlayer(xy, d, scalexy, xyindex, basecasex, tilex0, tilex0+j0, tilescountx, basecasey, tiley0, tiley1, tilescounty, maxcoresize, interfacesize, lsqrcnt, lambdareg, basis1, pool, spline, _child_tasks, _smp_enabled, _state);
            spline2d_fastddmfitlayer(xy, d, scalexy, xyindex, basecasex, tilex0+j0, tilex1, tilescountx, basecasey, tiley0, tiley1, tilescounty, maxcoresize, interfacesize, lsqrcnt, lambdareg, basis1, pool, spline, _state);
        }
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        ae_frame_leave(_state);
        return;
    }
    ae_assert(tiley0==tiley1-1, "Spline2DFit.FastDDMFitLayer: integrity check failed", _state);
    ae_assert(tilex0==tilex1-1, "Spline2DFit.FastDDMFitLayer: integrity check failed", _state);
    tile1 = tiley0;
    tile0 = tilex0;
    
    /*
     * Retrieve temporaries
     */
    ae_shared_pool_retrieve(pool, &_buf, _state);
    
    /*
     * Analyze dataset
     */
    xa = iboundval(tile0*basecasex-interfacesize, 0, kx, _state);
    xb = iboundval((tile0+1)*basecasex+interfacesize, 0, kx, _state);
    ya = iboundval(tile1*basecasey-interfacesize, 0, ky, _state);
    yb = iboundval((tile1+1)*basecasey+interfacesize, 0, ky, _state);
    tilesize0 = xb-xa;
    tilesize1 = yb-ya;
    
    /*
     * Solve current chunk with BlockLLS
     */
    dummytss = 1.0;
    spline2d_xdesigngenerate(xy, xyindex, xa, xb, kx, ya, yb, ky, d, lambdareg, 0.0, basis1, &buf->xdesignmatrix, _state);
    spline2d_blockllsfit(&buf->xdesignmatrix, lsqrcnt, &buf->tmpz, &buf->dummyrep, dummytss, &buf->blockllsbuf, _state);
    buf->localmodel.d = d;
    buf->localmodel.m = tilesize1;
    buf->localmodel.n = tilesize0;
    buf->localmodel.stype = -3;
    buf->localmodel.hasmissingcells = ae_false;
    rvectorsetlengthatleast(&buf->localmodel.x, tilesize0, _state);
    rvectorsetlengthatleast(&buf->localmodel.y, tilesize1, _state);
    rvectorsetlengthatleast(&buf->localmodel.f, tilesize0*tilesize1*d*4, _state);
    for(i=0; i<=tilesize0-1; i++)
    {
        buf->localmodel.x.ptr.p_double[i] = (double)(xa+i);
    }
    for(i=0; i<=tilesize1-1; i++)
    {
        buf->localmodel.y.ptr.p_double[i] = (double)(ya+i);
    }
    for(i=0; i<=tilesize0*tilesize1*d*4-1; i++)
    {
        buf->localmodel.f.ptr.p_double[i] = 0.0;
    }
    spline2d_updatesplinetable(&buf->tmpz, tilesize0, tilesize1, d, basis1, bfrad, &buf->localmodel.f, tilesize1, tilesize0, 1, _state);
    
    /*
     * Transform local spline to original coordinates
     */
    sfx = buf->localmodel.n*buf->localmodel.m*d;
    sfy = 2*buf->localmodel.n*buf->localmodel.m*d;
    sfxy = 3*buf->localmodel.n*buf->localmodel.m*d;
    for(i=0; i<=tilesize0-1; i++)
    {
        buf->localmodel.x.ptr.p_double[i] = buf->localmodel.x.ptr.p_double[i]*(double)scalexy;
    }
    for(i=0; i<=tilesize1-1; i++)
    {
        buf->localmodel.y.ptr.p_double[i] = buf->localmodel.y.ptr.p_double[i]*(double)scalexy;
    }
    for(i=0; i<=tilesize0*tilesize1*d-1; i++)
    {
        buf->localmodel.f.ptr.p_double[sfx+i] = buf->localmodel.f.ptr.p_double[sfx+i]*invscalexy;
        buf->localmodel.f.ptr.p_double[sfy+i] = buf->localmodel.f.ptr.p_double[sfy+i]*invscalexy;
        buf->localmodel.f.ptr.p_double[sfxy+i] = buf->localmodel.f.ptr.p_double[sfxy+i]*(invscalexy*invscalexy);
    }
    
    /*
     * Output results; for inner and topmost/leftmost tiles we output only BasecaseX*BasecaseY
     * inner elements; for rightmost/bottom ones we also output one column/row of the interface
     * part.
     *
     * Such complexity is explained by the fact that area size (by design) is not evenly divisible
     * by the tile size; it is divisible with remainder=1, and we expect that interface size is
     * at least 1, so we can fill the missing rightmost/bottom elements of Z by the interface
     * values.
     */
    ae_assert(interfacesize>=1, "Spline2DFit: integrity check failed", _state);
    sfx = spline->n*spline->m*d;
    sfy = 2*spline->n*spline->m*d;
    sfxy = 3*spline->n*spline->m*d;
    cnt0 = basecasex*scalexy;
    cnt1 = basecasey*scalexy;
    if( tile0==tilescountx-1 )
    {
        inc(&cnt0, _state);
    }
    if( tile1==tilescounty-1 )
    {
        inc(&cnt1, _state);
    }
    offs = d*(spline->n*tile1*basecasey*scalexy+tile0*basecasex*scalexy);
    for(j1=0; j1<=cnt1-1; j1++)
    {
        for(j0=0; j0<=cnt0-1; j0++)
        {
            for(j=0; j<=d-1; j++)
            {
                spline2ddiff2vi(&buf->localmodel, (double)(tile0*basecasex*scalexy+j0), (double)(tile1*basecasey*scalexy+j1), j, &vs, &vsx, &vsy, &vsxx, &vsxy, &vsyy, _state);
                spline->f.ptr.p_double[offs+d*(spline->n*j1+j0)+j] = spline->f.ptr.p_double[offs+d*(spline->n*j1+j0)+j]+vs;
                spline->f.ptr.p_double[sfx+offs+d*(spline->n*j1+j0)+j] = spline->f.ptr.p_double[sfx+offs+d*(spline->n*j1+j0)+j]+vsx;
                spline->f.ptr.p_double[sfy+offs+d*(spline->n*j1+j0)+j] = spline->f.ptr.p_double[sfy+offs+d*(spline->n*j1+j0)+j]+vsy;
                spline->f.ptr.p_double[sfxy+offs+d*(spline->n*j1+j0)+j] = spline->f.ptr.p_double[sfxy+offs+d*(spline->n*j1+j0)+j]+vsxy;
            }
        }
    }
    
    /*
     * Recycle temporaries
     */
    ae_shared_pool_recycle(pool, &_buf, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_spline2d_fastddmfitlayer(/* Real    */ const ae_vector* xy,
    ae_int_t d,
    ae_int_t scalexy,
    /* Integer */ const ae_vector* xyindex,
    ae_int_t basecasex,
    ae_int_t tilex0,
    ae_int_t tilex1,
    ae_int_t tilescountx,
    ae_int_t basecasey,
    ae_int_t tiley0,
    ae_int_t tiley1,
    ae_int_t tilescounty,
    ae_int_t maxcoresize,
    ae_int_t interfacesize,
    ae_int_t lsqrcnt,
    double lambdareg,
    const spline1dinterpolant* basis1,
    ae_shared_pool* pool,
    spline2dinterpolant* spline,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        spline2d_fastddmfitlayer(xy,d,scalexy,xyindex,basecasex,tilex0,tilex1,tilescountx,basecasey,tiley0,tiley1,tilescounty,maxcoresize,interfacesize,lsqrcnt,lambdareg,basis1,pool,spline, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_spline2d_fastddmfitlayer;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.const_val = xy;
        _task->data.parameters[1].value.ival = d;
        _task->data.parameters[2].value.ival = scalexy;
        _task->data.parameters[3].value.const_val = xyindex;
        _task->data.parameters[4].value.ival = basecasex;
        _task->data.parameters[5].value.ival = tilex0;
        _task->data.parameters[6].value.ival = tilex1;
        _task->data.parameters[7].value.ival = tilescountx;
        _task->data.parameters[8].value.ival = basecasey;
        _task->data.parameters[9].value.ival = tiley0;
        _task->data.parameters[10].value.ival = tiley1;
        _task->data.parameters[11].value.ival = tilescounty;
        _task->data.parameters[12].value.ival = maxcoresize;
        _task->data.parameters[13].value.ival = interfacesize;
        _task->data.parameters[14].value.ival = lsqrcnt;
        _task->data.parameters[15].value.dval = lambdareg;
        _task->data.parameters[16].value.const_val = basis1;
        _task->data.parameters[17].value.val = pool;
        _task->data.parameters[18].value.val = spline;
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
void _task_spline2d_fastddmfitlayer(ae_task_data *_data, ae_state *_state)
{
    const ae_vector* xy;
    ae_int_t d;
    ae_int_t scalexy;
    const ae_vector* xyindex;
    ae_int_t basecasex;
    ae_int_t tilex0;
    ae_int_t tilex1;
    ae_int_t tilescountx;
    ae_int_t basecasey;
    ae_int_t tiley0;
    ae_int_t tiley1;
    ae_int_t tilescounty;
    ae_int_t maxcoresize;
    ae_int_t interfacesize;
    ae_int_t lsqrcnt;
    double lambdareg;
    const spline1dinterpolant* basis1;
    ae_shared_pool* pool;
    spline2dinterpolant* spline;
    xy = (const ae_vector*)_data->parameters[0].value.const_val;
    d = _data->parameters[1].value.ival;
    scalexy = _data->parameters[2].value.ival;
    xyindex = (const ae_vector*)_data->parameters[3].value.const_val;
    basecasex = _data->parameters[4].value.ival;
    tilex0 = _data->parameters[5].value.ival;
    tilex1 = _data->parameters[6].value.ival;
    tilescountx = _data->parameters[7].value.ival;
    basecasey = _data->parameters[8].value.ival;
    tiley0 = _data->parameters[9].value.ival;
    tiley1 = _data->parameters[10].value.ival;
    tilescounty = _data->parameters[11].value.ival;
    maxcoresize = _data->parameters[12].value.ival;
    interfacesize = _data->parameters[13].value.ival;
    lsqrcnt = _data->parameters[14].value.ival;
    lambdareg = _data->parameters[15].value.dval;
    basis1 = (const spline1dinterpolant*)_data->parameters[16].value.const_val;
    pool = (ae_shared_pool*)_data->parameters[17].value.val;
    spline = (spline2dinterpolant*)_data->parameters[18].value.val;
   ae_state_set_flags(_state, _data->flags);
   spline2d_fastddmfitlayer(xy,d,scalexy,xyindex,basecasex,tilex0,tilex1,tilescountx,basecasey,tiley0,tiley1,tilescounty,maxcoresize,interfacesize,lsqrcnt,lambdareg,basis1,pool,spline, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_spline2d_fastddmfitlayer(/* Real    */ const ae_vector* xy,
    ae_int_t d,
    ae_int_t scalexy,
    /* Integer */ const ae_vector* xyindex,
    ae_int_t basecasex,
    ae_int_t tilex0,
    ae_int_t tilex1,
    ae_int_t tilescountx,
    ae_int_t basecasey,
    ae_int_t tiley0,
    ae_int_t tiley1,
    ae_int_t tilescounty,
    ae_int_t maxcoresize,
    ae_int_t interfacesize,
    ae_int_t lsqrcnt,
    double lambdareg,
    const spline1dinterpolant* basis1,
    ae_shared_pool* pool,
    spline2dinterpolant* spline,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_spline2d_fastddmfitlayer;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.const_val = xy;
    _task->data.parameters[1].value.ival = d;
    _task->data.parameters[2].value.ival = scalexy;
    _task->data.parameters[3].value.const_val = xyindex;
    _task->data.parameters[4].value.ival = basecasex;
    _task->data.parameters[5].value.ival = tilex0;
    _task->data.parameters[6].value.ival = tilex1;
    _task->data.parameters[7].value.ival = tilescountx;
    _task->data.parameters[8].value.ival = basecasey;
    _task->data.parameters[9].value.ival = tiley0;
    _task->data.parameters[10].value.ival = tiley1;
    _task->data.parameters[11].value.ival = tilescounty;
    _task->data.parameters[12].value.ival = maxcoresize;
    _task->data.parameters[13].value.ival = interfacesize;
    _task->data.parameters[14].value.ival = lsqrcnt;
    _task->data.parameters[15].value.dval = lambdareg;
    _task->data.parameters[16].value.const_val = basis1;
    _task->data.parameters[17].value.val = pool;
    _task->data.parameters[18].value.val = spline;
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
This function performs fitting with  BlockLLS solver.  Internal  function,
never use it directly.

IMPORTANT: performance  and  memory  requirements  of  this  function  are
           asymmetric w.r.t. KX and KY: it has
           * O(KY*KX^2) memory requirements
           * O(KY*KX^3) running time
           Thus, if you have large KY and small KX,  simple  transposition
           of your dataset may give you great speedup.

INPUT PARAMETERS:
    AV      -   sparse matrix, [ARows,KX*KY] in size.  "Vertical"  version
                of design matrix, rows [0,NPoints) contain values of basis
                functions at dataset  points.  Other  rows  are  used  for
                nonlinearity penalty and other stuff like that.
    AH      -   transpose(AV), "horizontal" version of AV
    ARows   -   rows count
    XY      -   array[NPoints*(2+D)], dataset
    KX, KY  -   grid size
    NPoints -   points count
    D       -   number of components in vector-valued spline, D>=1
    LSQRCnt -   number of iterations, non-zero:
                * LSQRCnt>0 means that specified amount of  preconditioned
                  LSQR  iterations  will  be  performed  to solve problem;
                  usually  we  need  2..5  its.  Recommended option - best
                  convergence and stability/quality.
                * LSQRCnt<0 means that instead of LSQR  we  use  iterative
                  refinement on normal equations. Again, 2..5 its is enough.
    Z       -   possibly preallocated buffer for solution
    Rep     -   report structure; fields which are not set by this function
                are left intact
    TSS     -   total sum of squares; used to calculate R2
    

OUTPUT PARAMETERS:
    XY      -   destroyed in process
    Z       -   array[KX*KY*D], filled by solution; KX*KY coefficients
                corresponding to each of D dimensions are stored contiguously.
    Rep         -   following fields are set:
                    * Rep.RMSError
                    * Rep.AvgError
                    * Rep.MaxError
                    * Rep.R2

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static void spline2d_blockllsfit(spline2dxdesignmatrix* xdesign,
     ae_int_t lsqrcnt,
     /* Real    */ ae_vector* z,
     spline2dfitreport* rep,
     double tss,
     spline2dblockllsbuf* buf,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t blockbandwidth;
    ae_int_t d;
    ae_int_t i;
    ae_int_t j;
    double lambdachol;
    sreal mxata;
    double v;
    ae_int_t celloffset;
    ae_int_t i0;
    ae_int_t i1;
    double rss;
    ae_int_t arows;
    ae_int_t bw2;
    ae_int_t kx;
    ae_int_t ky;

    ae_frame_make(_state, &_frame_block);
    memset(&mxata, 0, sizeof(mxata));
    _sreal_init(&mxata, _state, ae_true);

    ae_assert(xdesign->blockwidth==4, "Spline2DFit: integrity check failed", _state);
    blockbandwidth = 3;
    d = xdesign->d;
    arows = xdesign->nrows;
    kx = xdesign->kx;
    ky = xdesign->ky;
    bw2 = xdesign->blockwidth*xdesign->blockwidth;
    
    /*
     * Initial values for Z/Residuals
     */
    rvectorsetlengthatleast(z, kx*ky*d, _state);
    for(i=0; i<=kx*ky*d-1; i++)
    {
        z->ptr.p_double[i] = (double)(0);
    }
    
    /*
     * Create and factorize design matrix. Add regularizer if
     * factorization failed (happens sometimes with zero
     * smoothing and sparsely populated datasets).
     *
     * The algorithm below is refactoring of NaiveLLS algorithm,
     * which uses sparsity properties and compressed block storage.
     *
     * Problem sparsity pattern results in block-band-diagonal
     * matrix (block matrix with limited bandwidth, equal to 3
     * for bicubic splines). Thus, we have KY*KY blocks, each
     * of them is KX*KX in size. Design matrix is stored in
     * large NROWS*KX matrix, with NROWS=(BlockBandwidth+1)*KY*KX.
     *
     * We use adaptation of block skyline storage format, with
     * TOWERSIZE*KX skyline bands (towers) stored sequentially;
     * here TOWERSIZE=(BlockBandwidth+1)*KX. So, we have KY
     * "towers", stored one below other, in BlockATA matrix.
     * Every "tower" is a sequence of BlockBandwidth+1 cells,
     * each of them being KX*KX in size.
     */
    lambdachol = spline2d_cholreg;
    rmatrixsetlengthatleast(&buf->blockata, (blockbandwidth+1)*ky*kx, kx, _state);
    for(;;)
    {
        
        /*
         * Parallel generation of squared design matrix.
         */
        spline2d_xdesignblockata(xdesign, &buf->blockata, &mxata.val, _state);
        
        /*
         * Regularization
         */
        v = coalesce(mxata.val, 1.0, _state)*lambdachol;
        for(i1=0; i1<=ky-1; i1++)
        {
            celloffset = spline2d_getcelloffset(kx, ky, blockbandwidth, i1, i1, _state);
            for(i0=0; i0<=kx-1; i0++)
            {
                buf->blockata.ptr.pp_double[celloffset+i0][i0] = buf->blockata.ptr.pp_double[celloffset+i0][i0]+v;
            }
        }
        
        /*
         * Try Cholesky factorization.
         */
        if( !spline2d_blockllscholesky(&buf->blockata, kx, ky, &buf->trsmbuf2, &buf->cholbuf2, &buf->cholbuf1, _state) )
        {
            
            /*
             * Factorization failed, increase regularizer and repeat
             */
            lambdachol = coalesce((double)10*lambdachol, 1.0E-12, _state);
            continue;
        }
        break;
    }
    
    /*
     * Solve
     */
    rss = 0.0;
    rep->rmserror = (double)(0);
    rep->avgerror = (double)(0);
    rep->maxerror = (double)(0);
    ae_assert(lsqrcnt>0, "Spline2DFit: integrity failure", _state);
    rvectorsetlengthatleast(&buf->tmp0, arows, _state);
    rvectorsetlengthatleast(&buf->tmp1, kx*ky, _state);
    linlsqrcreatebuf(arows, kx*ky, &buf->solver, _state);
    for(j=0; j<=d-1; j++)
    {
        
        /*
         * Preconditioned LSQR:
         *
         * use Cholesky factor U of squared design matrix A'*A to
         * transform min|A*x-b| to min|[A*inv(U)]*y-b| with y=U*x.
         *
         * Preconditioned problem is solved with LSQR solver, which
         * gives superior results than normal equations.
         */
        for(i=0; i<=arows-1; i++)
        {
            if( i<xdesign->npoints )
            {
                buf->tmp0.ptr.p_double[i] = xdesign->vals.ptr.pp_double[i][bw2+j];
            }
            else
            {
                buf->tmp0.ptr.p_double[i] = 0.0;
            }
        }
        linlsqrrestart(&buf->solver, _state);
        linlsqrsetb(&buf->solver, &buf->tmp0, _state);
        linlsqrsetcond(&buf->solver, 1.0E-14, 1.0E-14, lsqrcnt, _state);
        while(linlsqriteration(&buf->solver, _state))
        {
            if( buf->solver.needmv )
            {
                
                /*
                 * Use Cholesky factorization of the system matrix
                 * as preconditioner: solve TRSV(U,Solver.X)
                 */
                for(i=0; i<=kx*ky-1; i++)
                {
                    buf->tmp1.ptr.p_double[i] = buf->solver.x.ptr.p_double[i];
                }
                spline2d_blockllstrsv(&buf->blockata, kx, ky, ae_false, &buf->tmp1, _state);
                
                /*
                 * After preconditioning is done, multiply by A
                 */
                spline2d_xdesignmv(xdesign, &buf->tmp1, &buf->solver.mv, _state);
            }
            if( buf->solver.needmtv )
            {
                
                /*
                 * Multiply by design matrix A
                 */
                spline2d_xdesignmtv(xdesign, &buf->solver.x, &buf->solver.mtv, _state);
                
                /*
                 * Multiply by preconditioner: solve TRSV(U',A*Solver.X)
                 */
                spline2d_blockllstrsv(&buf->blockata, kx, ky, ae_true, &buf->solver.mtv, _state);
            }
        }
        
        /*
         * Get results and post-multiply by preconditioner to get
         * original variables.
         */
        linlsqrresults(&buf->solver, &buf->tmp1, &buf->solverrep, _state);
        spline2d_blockllstrsv(&buf->blockata, kx, ky, ae_false, &buf->tmp1, _state);
        for(i=0; i<=kx*ky-1; i++)
        {
            z->ptr.p_double[kx*ky*j+i] = buf->tmp1.ptr.p_double[i];
        }
        
        /*
         * Calculate model values
         */
        spline2d_xdesignmv(xdesign, &buf->tmp1, &buf->tmp0, _state);
        for(i=0; i<=xdesign->npoints-1; i++)
        {
            v = xdesign->vals.ptr.pp_double[i][bw2+j]-buf->tmp0.ptr.p_double[i];
            rss = rss+v*v;
            rep->rmserror = rep->rmserror+ae_sqr(v, _state);
            rep->avgerror = rep->avgerror+ae_fabs(v, _state);
            rep->maxerror = ae_maxreal(rep->maxerror, ae_fabs(v, _state), _state);
        }
    }
    rep->rmserror = ae_sqrt(rep->rmserror/coalesce((double)(xdesign->npoints*d), 1.0, _state), _state);
    rep->avgerror = rep->avgerror/coalesce((double)(xdesign->npoints*d), 1.0, _state);
    rep->r2 = 1.0-rss/coalesce(tss, 1.0, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function performs fitting with  NaiveLLS solver.  Internal  function,
never use it directly.

INPUT PARAMETERS:
    AV      -   sparse matrix, [ARows,KX*KY] in size.  "Vertical"  version
                of design matrix, rows [0,NPoints] contain values of basis
                functions at dataset  points.  Other  rows  are  used  for
                nonlinearity penalty and other stuff like that.
    AH      -   transpose(AV), "horizontal" version of AV
    ARows   -   rows count
    XY      -   array[NPoints*(2+D)], dataset
    KX, KY  -   grid size
    NPoints -   points count
    D       -   number of components in vector-valued spline, D>=1
    LSQRCnt -   number of iterations, non-zero:
                * LSQRCnt>0 means that specified amount of  preconditioned
                  LSQR  iterations  will  be  performed  to solve problem;
                  usually  we  need  2..5  its.  Recommended option - best
                  convergence and stability/quality.
                * LSQRCnt<0 means that instead of LSQR  we  use  iterative
                  refinement on normal equations. Again, 2..5 its is enough.
    Z       -   possibly preallocated buffer for solution
    Rep     -   report structure; fields which are not set by this function
                are left intact
    TSS     -   total sum of squares; used to calculate R2
    

OUTPUT PARAMETERS:
    XY      -   destroyed in process
    Z       -   array[KX*KY*D], filled by solution; KX*KY coefficients
                corresponding to each of D dimensions are stored contiguously.
    Rep     -   following fields are set:
                    * Rep.RMSError
                    * Rep.AvgError
                    * Rep.MaxError
                    * Rep.R2

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static void spline2d_naivellsfit(const sparsematrix* av,
     const sparsematrix* ah,
     ae_int_t arows,
     /* Real    */ ae_vector* xy,
     ae_int_t kx,
     ae_int_t ky,
     ae_int_t npoints,
     ae_int_t d,
     ae_int_t lsqrcnt,
     /* Real    */ ae_vector* z,
     spline2dfitreport* rep,
     double tss,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t ew;
    ae_int_t i;
    ae_int_t j;
    ae_int_t i0;
    ae_int_t i1;
    ae_int_t j0;
    ae_int_t j1;
    double v;
    ae_int_t blockbandwidth;
    double lambdareg;
    ae_int_t srci;
    ae_int_t srcj;
    ae_int_t idxi;
    ae_int_t idxj;
    ae_int_t endi;
    ae_int_t endj;
    ae_int_t rfsidx;
    ae_matrix ata;
    ae_vector tmp0;
    ae_vector tmp1;
    double mxata;
    linlsqrstate solver;
    linlsqrreport solverrep;
    double rss;

    ae_frame_make(_state, &_frame_block);
    memset(&ata, 0, sizeof(ata));
    memset(&tmp0, 0, sizeof(tmp0));
    memset(&tmp1, 0, sizeof(tmp1));
    memset(&solver, 0, sizeof(solver));
    memset(&solverrep, 0, sizeof(solverrep));
    ae_matrix_init(&ata, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp1, 0, DT_REAL, _state, ae_true);
    _linlsqrstate_init(&solver, _state, ae_true);
    _linlsqrreport_init(&solverrep, _state, ae_true);

    blockbandwidth = 3;
    ew = 2+d;
    
    /*
     * Initial values for Z/Residuals
     */
    rvectorsetlengthatleast(z, kx*ky*d, _state);
    for(i=0; i<=kx*ky*d-1; i++)
    {
        z->ptr.p_double[i] = (double)(0);
    }
    
    /*
     * Create and factorize design matrix.
     *
     * Add regularizer if factorization failed (happens sometimes
     * with zero smoothing and sparsely populated datasets).
     */
    lambdareg = spline2d_cholreg;
    rmatrixsetlengthatleast(&ata, kx*ky, kx*ky, _state);
    for(;;)
    {
        mxata = 0.0;
        for(i=0; i<=kx*ky-1; i++)
        {
            for(j=i; j<=kx*ky-1; j++)
            {
                
                /*
                 * Initialize by zero
                 */
                ata.ptr.pp_double[i][j] = (double)(0);
                
                /*
                 * Determine grid nodes corresponding to I and J;
                 * skip if too far away
                 */
                i0 = i%kx;
                i1 = i/kx;
                j0 = j%kx;
                j1 = j/kx;
                if( ae_iabs(i0-j0, _state)>blockbandwidth||ae_iabs(i1-j1, _state)>blockbandwidth )
                {
                    continue;
                }
                
                /*
                 * Nodes are close enough, calculate product of columns I and J of A.
                 */
                v = (double)(0);
                srci = ah->ridx.ptr.p_int[i];
                srcj = ah->ridx.ptr.p_int[j];
                endi = ah->ridx.ptr.p_int[i+1];
                endj = ah->ridx.ptr.p_int[j+1];
                for(;;)
                {
                    if( srci>=endi||srcj>=endj )
                    {
                        break;
                    }
                    idxi = ah->idx.ptr.p_int[srci];
                    idxj = ah->idx.ptr.p_int[srcj];
                    if( idxi==idxj )
                    {
                        v = v+ah->vals.ptr.p_double[srci]*ah->vals.ptr.p_double[srcj];
                        srci = srci+1;
                        srcj = srcj+1;
                        continue;
                    }
                    if( idxi<idxj )
                    {
                        srci = srci+1;
                    }
                    else
                    {
                        srcj = srcj+1;
                    }
                }
                ata.ptr.pp_double[i][j] = v;
                mxata = ae_maxreal(mxata, ae_fabs(v, _state), _state);
            }
        }
        v = coalesce(mxata, 1.0, _state)*lambdareg;
        for(i=0; i<=kx*ky-1; i++)
        {
            ata.ptr.pp_double[i][i] = ata.ptr.pp_double[i][i]+v;
        }
        if( spdmatrixcholesky(&ata, kx*ky, ae_true, _state) )
        {
            
            /*
             * Success!
             */
            break;
        }
        
        /*
         * Factorization failed, increase regularizer and repeat
         */
        lambdareg = coalesce((double)10*lambdareg, 1.0E-12, _state);
    }
    
    /*
     * Solve
     *
     * NOTE: we expect that Z is zero-filled, and we treat it
     *       like initial approximation to solution.
     */
    rvectorsetlengthatleast(&tmp0, arows, _state);
    rvectorsetlengthatleast(&tmp1, kx*ky, _state);
    if( lsqrcnt>0 )
    {
        linlsqrcreate(arows, kx*ky, &solver, _state);
    }
    for(j=0; j<=d-1; j++)
    {
        ae_assert(lsqrcnt!=0, "Spline2DFit: integrity failure", _state);
        if( lsqrcnt>0 )
        {
            
            /*
             * Preconditioned LSQR:
             *
             * use Cholesky factor U of squared design matrix A'*A to
             * transform min|A*x-b| to min|[A*inv(U)]*y-b| with y=U*x.
             *
             * Preconditioned problem is solved with LSQR solver, which
             * gives superior results than normal equations.
             */
            linlsqrcreate(arows, kx*ky, &solver, _state);
            for(i=0; i<=arows-1; i++)
            {
                if( i<npoints )
                {
                    tmp0.ptr.p_double[i] = xy->ptr.p_double[i*ew+2+j];
                }
                else
                {
                    tmp0.ptr.p_double[i] = 0.0;
                }
            }
            linlsqrsetb(&solver, &tmp0, _state);
            linlsqrsetcond(&solver, 1.0E-14, 1.0E-14, lsqrcnt, _state);
            while(linlsqriteration(&solver, _state))
            {
                if( solver.needmv )
                {
                    
                    /*
                     * Use Cholesky factorization of the system matrix
                     * as preconditioner: solve TRSV(U,Solver.X)
                     */
                    for(i=0; i<=kx*ky-1; i++)
                    {
                        tmp1.ptr.p_double[i] = solver.x.ptr.p_double[i];
                    }
                    rmatrixtrsv(kx*ky, &ata, 0, 0, ae_true, ae_false, 0, &tmp1, 0, _state);
                    
                    /*
                     * After preconditioning is done, multiply by A
                     */
                    sparsemv(av, &tmp1, &solver.mv, _state);
                }
                if( solver.needmtv )
                {
                    
                    /*
                     * Multiply by design matrix A
                     */
                    sparsemv(ah, &solver.x, &solver.mtv, _state);
                    
                    /*
                     * Multiply by preconditioner: solve TRSV(U',A*Solver.X)
                     */
                    rmatrixtrsv(kx*ky, &ata, 0, 0, ae_true, ae_false, 1, &solver.mtv, 0, _state);
                }
            }
            linlsqrresults(&solver, &tmp1, &solverrep, _state);
            rmatrixtrsv(kx*ky, &ata, 0, 0, ae_true, ae_false, 0, &tmp1, 0, _state);
            for(i=0; i<=kx*ky-1; i++)
            {
                z->ptr.p_double[kx*ky*j+i] = tmp1.ptr.p_double[i];
            }
            
            /*
             * Calculate model values
             */
            sparsemv(av, &tmp1, &tmp0, _state);
            for(i=0; i<=npoints-1; i++)
            {
                xy->ptr.p_double[i*ew+2+j] = xy->ptr.p_double[i*ew+2+j]-tmp0.ptr.p_double[i];
            }
        }
        else
        {
            
            /*
             * Iterative refinement, inferior to LSQR
             *
             * For each dimension D:
             * * fetch current estimate for solution from Z to Tmp1
             * * calculate residual r for current estimate, store in Tmp0
             * * calculate product of residual and design matrix A'*r, store it in Tmp1
             * * Cholesky solver
             * * update current estimate
             */
            for(rfsidx=1; rfsidx<=-lsqrcnt; rfsidx++)
            {
                for(i=0; i<=kx*ky-1; i++)
                {
                    tmp1.ptr.p_double[i] = z->ptr.p_double[kx*ky*j+i];
                }
                sparsemv(av, &tmp1, &tmp0, _state);
                for(i=0; i<=arows-1; i++)
                {
                    if( i<npoints )
                    {
                        v = xy->ptr.p_double[i*ew+2+j];
                    }
                    else
                    {
                        v = (double)(0);
                    }
                    tmp0.ptr.p_double[i] = v-tmp0.ptr.p_double[i];
                }
                sparsemv(ah, &tmp0, &tmp1, _state);
                rmatrixtrsv(kx*ky, &ata, 0, 0, ae_true, ae_false, 1, &tmp1, 0, _state);
                rmatrixtrsv(kx*ky, &ata, 0, 0, ae_true, ae_false, 0, &tmp1, 0, _state);
                for(i=0; i<=kx*ky-1; i++)
                {
                    z->ptr.p_double[kx*ky*j+i] = z->ptr.p_double[kx*ky*j+i]+tmp1.ptr.p_double[i];
                }
            }
            
            /*
             * Calculate model values
             */
            for(i=0; i<=kx*ky-1; i++)
            {
                tmp1.ptr.p_double[i] = z->ptr.p_double[kx*ky*j+i];
            }
            sparsemv(av, &tmp1, &tmp0, _state);
            for(i=0; i<=npoints-1; i++)
            {
                xy->ptr.p_double[i*ew+2+j] = xy->ptr.p_double[i*ew+2+j]-tmp0.ptr.p_double[i];
            }
        }
    }
    
    /*
     * Generate report
     */
    rep->rmserror = (double)(0);
    rep->avgerror = (double)(0);
    rep->maxerror = (double)(0);
    rss = 0.0;
    for(i=0; i<=npoints-1; i++)
    {
        for(j=0; j<=d-1; j++)
        {
            v = xy->ptr.p_double[i*ew+2+j];
            rss = rss+v*v;
            rep->rmserror = rep->rmserror+ae_sqr(v, _state);
            rep->avgerror = rep->avgerror+ae_fabs(v, _state);
            rep->maxerror = ae_maxreal(rep->maxerror, ae_fabs(v, _state), _state);
        }
    }
    rep->rmserror = ae_sqrt(rep->rmserror/coalesce((double)(npoints*d), 1.0, _state), _state);
    rep->avgerror = rep->avgerror/coalesce((double)(npoints*d), 1.0, _state);
    rep->r2 = 1.0-rss/coalesce(tss, 1.0, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This  is  convenience  function  for band block storage format; it returns
offset of KX*KX-sized block (I,J) in a compressed 2D array.

For specific offset=OFFSET,
block (I,J) will be stored in entries BlockMatrix[OFFSET:OFFSET+KX-1,0:KX-1]

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static ae_int_t spline2d_getcelloffset(ae_int_t kx,
     ae_int_t ky,
     ae_int_t blockbandwidth,
     ae_int_t i,
     ae_int_t j,
     ae_state *_state)
{
    ae_int_t result;


    ae_assert(i>=0&&i<ky, "Spline2DFit: GetCellOffset() integrity error", _state);
    ae_assert(j>=0&&j<ky, "Spline2DFit: GetCellOffset() integrity error", _state);
    ae_assert(j>=i&&j<=i+blockbandwidth, "Spline2DFit: GetCellOffset() integrity error", _state);
    result = j*(blockbandwidth+1)*kx;
    result = result+(blockbandwidth-(j-i))*kx;
    return result;
}


/*************************************************************************
This  is  convenience  function  for band block storage format; it  copies
cell (I,J) from compressed format to uncompressed general matrix, at desired
position.

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static void spline2d_copycellto(ae_int_t kx,
     ae_int_t ky,
     ae_int_t blockbandwidth,
     /* Real    */ const ae_matrix* blockata,
     ae_int_t i,
     ae_int_t j,
     /* Real    */ ae_matrix* dst,
     ae_int_t dst0,
     ae_int_t dst1,
     ae_state *_state)
{
    ae_int_t celloffset;
    ae_int_t idx0;
    ae_int_t idx1;


    celloffset = spline2d_getcelloffset(kx, ky, blockbandwidth, i, j, _state);
    for(idx0=0; idx0<=kx-1; idx0++)
    {
        for(idx1=0; idx1<=kx-1; idx1++)
        {
            dst->ptr.pp_double[dst0+idx0][dst1+idx1] = blockata->ptr.pp_double[celloffset+idx0][idx1];
        }
    }
}


/*************************************************************************
This  is  convenience  function  for band block storage format; it
truncates all elements of  cell (I,J) which are less than Eps in magnitude.

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static void spline2d_flushtozerocell(ae_int_t kx,
     ae_int_t ky,
     ae_int_t blockbandwidth,
     /* Real    */ ae_matrix* blockata,
     ae_int_t i,
     ae_int_t j,
     double eps,
     ae_state *_state)
{
    ae_int_t celloffset;
    ae_int_t idx0;
    ae_int_t idx1;
    double eps2;
    double v;


    celloffset = spline2d_getcelloffset(kx, ky, blockbandwidth, i, j, _state);
    eps2 = eps*eps;
    for(idx0=0; idx0<=kx-1; idx0++)
    {
        for(idx1=0; idx1<=kx-1; idx1++)
        {
            v = blockata->ptr.pp_double[celloffset+idx0][idx1];
            if( v*v<eps2 )
            {
                blockata->ptr.pp_double[celloffset+idx0][idx1] = (double)(0);
            }
        }
    }
}


/*************************************************************************
This function generates squared design matrix stored in block band format.

We use adaptation of block skyline storage format, with
TOWERSIZE*KX skyline bands (towers) stored sequentially;
here TOWERSIZE=(BlockBandwidth+1)*KX. So, we have KY
"towers", stored one below other, in BlockATA matrix.
Every "tower" is a sequence of BlockBandwidth+1 cells,
each of them being KX*KX in size.

INPUT PARAMETERS:
    AH      -   sparse matrix, [KX*KY,ARows] in size. "Horizontal" version
                of design matrix, cols [0,NPoints] contain values of basis
                functions at dataset  points.  Other  cols  are  used  for
                nonlinearity penalty and other stuff like that.
    KY0, KY1-   subset of output matrix bands to process; on entry it MUST
                be set to 0 and KY respectively.
    KX, KY  -   grid size
    BlockATA-   array[KY*(BlockBandwidth+1)*KX,KX],  preallocated  storage
                for output matrix in compressed block band format
    MXATA   -   on entry MUST be zero

OUTPUT PARAMETERS:
    BlockATA-   AH*AH', stored in compressed block band format

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static void spline2d_blockllsgenerateata(const sparsematrix* ah,
     ae_int_t ky0,
     ae_int_t ky1,
     ae_int_t kx,
     ae_int_t ky,
     /* Real    */ ae_matrix* blockata,
     sreal* mxata,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t blockbandwidth;
    double avgrowlen;
    double cellcost;
    double totalcost;
    sreal tmpmxata;
    ae_int_t i;
    ae_int_t j;
    ae_int_t i0;
    ae_int_t i1;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t celloffset;
    double v;
    ae_int_t srci;
    ae_int_t srcj;
    ae_int_t idxi;
    ae_int_t idxj;
    ae_int_t endi;
    ae_int_t endj;

    ae_frame_make(_state, &_frame_block);
    memset(&tmpmxata, 0, sizeof(tmpmxata));
    _sreal_init(&tmpmxata, _state, ae_true);

    ae_assert(ae_fp_greater_eq(mxata->val,(double)(0)), "BlockLLSGenerateATA: integrity check failed", _state);
    blockbandwidth = 3;
    
    /*
     * Determine problem cost, perform recursive subdivision
     * (with optional parallelization)
     */
    avgrowlen = (double)ah->ridx.ptr.p_int[kx*ky]/(double)(kx*ky);
    cellcost = rmul3((double)(kx), (double)(1+2*blockbandwidth), avgrowlen, _state);
    totalcost = rmul3((double)(ky1-ky0), (double)(1+2*blockbandwidth), cellcost, _state);
    if( ky1-ky0>=2&&ae_fp_greater(totalcost,smpactivationlevel(_state)) )
    {
        if( _trypexec_spline2d_blockllsgenerateata(ah,ky0,ky1,kx,ky,blockata,mxata, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            ae_frame_leave(_state);
            return;
        }
    }
    if( ky1-ky0>=2 )
    {
        
        /*
         * Split X: X*A = (X1 X2)^T*A
         */
        j = (ky1-ky0)/2;
        ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_fp_greater(totalcost,spawnlevel(_state)), _state);
        _spawn_spline2d_blockllsgenerateata(ah, ky0, ky0+j, kx, ky, blockata, &tmpmxata, _child_tasks, _smp_enabled, _state);
        spline2d_blockllsgenerateata(ah, ky0+j, ky1, kx, ky, blockata, mxata, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_false, _state);
        mxata->val = ae_maxreal(mxata->val, tmpmxata.val, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Splitting in Y-dimension is done, fill I1-th "tower"
     */
    ae_assert(ky1==ky0+1, "BlockLLSGenerateATA: integrity check failed", _state);
    i1 = ky0;
    for(j1=i1; j1<=ae_minint(ky-1, i1+blockbandwidth, _state); j1++)
    {
        celloffset = spline2d_getcelloffset(kx, ky, blockbandwidth, i1, j1, _state);
        
        /*
         * Clear cell (I1,J1)
         */
        for(i0=0; i0<=kx-1; i0++)
        {
            for(j0=0; j0<=kx-1; j0++)
            {
                blockata->ptr.pp_double[celloffset+i0][j0] = 0.0;
            }
        }
        
        /*
         * Initialize cell internals
         */
        for(i0=0; i0<=kx-1; i0++)
        {
            for(j0=0; j0<=kx-1; j0++)
            {
                if( ae_iabs(i0-j0, _state)<=blockbandwidth )
                {
                    
                    /*
                     * Nodes are close enough, calculate product of columns I and J of A.
                     */
                    v = (double)(0);
                    i = i1*kx+i0;
                    j = j1*kx+j0;
                    srci = ah->ridx.ptr.p_int[i];
                    srcj = ah->ridx.ptr.p_int[j];
                    endi = ah->ridx.ptr.p_int[i+1];
                    endj = ah->ridx.ptr.p_int[j+1];
                    for(;;)
                    {
                        if( srci>=endi||srcj>=endj )
                        {
                            break;
                        }
                        idxi = ah->idx.ptr.p_int[srci];
                        idxj = ah->idx.ptr.p_int[srcj];
                        if( idxi==idxj )
                        {
                            v = v+ah->vals.ptr.p_double[srci]*ah->vals.ptr.p_double[srcj];
                            srci = srci+1;
                            srcj = srcj+1;
                            continue;
                        }
                        if( idxi<idxj )
                        {
                            srci = srci+1;
                        }
                        else
                        {
                            srcj = srcj+1;
                        }
                    }
                    blockata->ptr.pp_double[celloffset+i0][j0] = v;
                    mxata->val = ae_maxreal(mxata->val, ae_fabs(v, _state), _state);
                }
            }
        }
    }
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_spline2d_blockllsgenerateata(const sparsematrix* ah,
    ae_int_t ky0,
    ae_int_t ky1,
    ae_int_t kx,
    ae_int_t ky,
    /* Real    */ ae_matrix* blockata,
    sreal* mxata,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        spline2d_blockllsgenerateata(ah,ky0,ky1,kx,ky,blockata,mxata, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_spline2d_blockllsgenerateata;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.const_val = ah;
        _task->data.parameters[1].value.ival = ky0;
        _task->data.parameters[2].value.ival = ky1;
        _task->data.parameters[3].value.ival = kx;
        _task->data.parameters[4].value.ival = ky;
        _task->data.parameters[5].value.val = blockata;
        _task->data.parameters[6].value.val = mxata;
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
void _task_spline2d_blockllsgenerateata(ae_task_data *_data, ae_state *_state)
{
    const sparsematrix* ah;
    ae_int_t ky0;
    ae_int_t ky1;
    ae_int_t kx;
    ae_int_t ky;
    ae_matrix* blockata;
    sreal* mxata;
    ah = (const sparsematrix*)_data->parameters[0].value.const_val;
    ky0 = _data->parameters[1].value.ival;
    ky1 = _data->parameters[2].value.ival;
    kx = _data->parameters[3].value.ival;
    ky = _data->parameters[4].value.ival;
    blockata = (ae_matrix*)_data->parameters[5].value.val;
    mxata = (sreal*)_data->parameters[6].value.val;
   ae_state_set_flags(_state, _data->flags);
   spline2d_blockllsgenerateata(ah,ky0,ky1,kx,ky,blockata,mxata, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_spline2d_blockllsgenerateata(const sparsematrix* ah,
    ae_int_t ky0,
    ae_int_t ky1,
    ae_int_t kx,
    ae_int_t ky,
    /* Real    */ ae_matrix* blockata,
    sreal* mxata,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_spline2d_blockllsgenerateata;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.const_val = ah;
    _task->data.parameters[1].value.ival = ky0;
    _task->data.parameters[2].value.ival = ky1;
    _task->data.parameters[3].value.ival = kx;
    _task->data.parameters[4].value.ival = ky;
    _task->data.parameters[5].value.val = blockata;
    _task->data.parameters[6].value.val = mxata;
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
This function performs Cholesky decomposition of squared design matrix
stored in block band format.

INPUT PARAMETERS:
    BlockATA        -   array[KY*(BlockBandwidth+1)*KX,KX], matrix in compressed
                        block band format
    KX, KY          -   grid size
    TrsmBuf2,
    CholBuf2,
    CholBuf1        -   buffers; reused by this function on subsequent calls,
                        automatically preallocated on the first call

OUTPUT PARAMETERS:
    BlockATA-   Cholesky factor, in compressed block band format

Result:
    True on success, False on Cholesky failure

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static ae_bool spline2d_blockllscholesky(/* Real    */ ae_matrix* blockata,
     ae_int_t kx,
     ae_int_t ky,
     /* Real    */ ae_matrix* trsmbuf2,
     /* Real    */ ae_matrix* cholbuf2,
     /* Real    */ ae_vector* cholbuf1,
     ae_state *_state)
{
    ae_int_t blockbandwidth;
    ae_int_t blockidx;
    ae_int_t i;
    ae_int_t j;
    ae_int_t celloffset;
    ae_int_t celloffset1;
    ae_bool result;


    blockbandwidth = 3;
    rmatrixsetlengthatleast(trsmbuf2, (blockbandwidth+1)*kx, (blockbandwidth+1)*kx, _state);
    rmatrixsetlengthatleast(cholbuf2, kx, kx, _state);
    rvectorsetlengthatleast(cholbuf1, kx, _state);
    result = ae_true;
    for(blockidx=0; blockidx<=ky-1; blockidx++)
    {
        
        /*
         * TRSM for TRAIL*TRAIL block matrix before current cell;
         * here TRAIL=MinInt(BlockIdx,BlockBandwidth).
         */
        for(i=0; i<=ae_minint(blockidx, blockbandwidth, _state)-1; i++)
        {
            for(j=i; j<=ae_minint(blockidx, blockbandwidth, _state)-1; j++)
            {
                spline2d_copycellto(kx, ky, blockbandwidth, blockata, ae_maxint(blockidx-blockbandwidth, 0, _state)+i, ae_maxint(blockidx-blockbandwidth, 0, _state)+j, trsmbuf2, i*kx, j*kx, _state);
            }
        }
        celloffset = spline2d_getcelloffset(kx, ky, blockbandwidth, ae_maxint(blockidx-blockbandwidth, 0, _state), blockidx, _state);
        rmatrixlefttrsm(ae_minint(blockidx, blockbandwidth, _state)*kx, kx, trsmbuf2, 0, 0, ae_true, ae_false, 1, blockata, celloffset, 0, _state);
        
        /*
         * SYRK for diagonal cell: MaxInt(BlockIdx-BlockBandwidth,0)
         * cells above diagonal one are used for update.
         */
        celloffset = spline2d_getcelloffset(kx, ky, blockbandwidth, ae_maxint(blockidx-blockbandwidth, 0, _state), blockidx, _state);
        celloffset1 = spline2d_getcelloffset(kx, ky, blockbandwidth, blockidx, blockidx, _state);
        rmatrixsyrk(kx, ae_minint(blockidx, blockbandwidth, _state)*kx, -1.0, blockata, celloffset, 0, 1, 1.0, blockata, celloffset1, 0, ae_true, _state);
        
        /*
         * Factorize diagonal cell
         */
        celloffset = spline2d_getcelloffset(kx, ky, blockbandwidth, blockidx, blockidx, _state);
        rmatrixcopy(kx, kx, blockata, celloffset, 0, cholbuf2, 0, 0, _state);
        if( !spdmatrixcholeskyrec(cholbuf2, 0, kx, ae_true, cholbuf1, _state) )
        {
            result = ae_false;
            return result;
        }
        rmatrixcopy(kx, kx, cholbuf2, 0, 0, blockata, celloffset, 0, _state);
        
        /*
         * PERFORMANCE TWEAK: drop nearly-denormals from last "tower".
         *
         * Sparse matrices like these may produce denormal numbers on
         * sparse datasets, with significant (10x!) performance penalty
         * on Intel chips. In order to avoid it, we manually truncate
         * small enough numbers.
         *
         * We use 1.0E-50 as clipping level (not really denormal, but
         * such small numbers are not actually important anyway).
         */
        for(i=ae_maxint(blockidx-blockbandwidth, 0, _state); i<=blockidx; i++)
        {
            spline2d_flushtozerocell(kx, ky, blockbandwidth, blockata, i, blockidx, 1.0E-50, _state);
        }
    }
    return result;
}


/*************************************************************************
This function performs TRSV on upper triangular Cholesky factor U, solving
either U*x=b or U'*x=b.

INPUT PARAMETERS:
    BlockATA        -   array[KY*(BlockBandwidth+1)*KX,KX], matrix U
                        in compressed block band format
    KX, KY          -   grid size
    TransU          -   whether to transpose U or not
    B               -   array[KX*KY], on entry - stores right part B

OUTPUT PARAMETERS:
    B               -   replaced by X

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static void spline2d_blockllstrsv(/* Real    */ const ae_matrix* blockata,
     ae_int_t kx,
     ae_int_t ky,
     ae_bool transu,
     /* Real    */ ae_vector* b,
     ae_state *_state)
{
    ae_int_t blockbandwidth;
    ae_int_t blockidx;
    ae_int_t blockidx1;
    ae_int_t celloffset;


    blockbandwidth = 3;
    if( !transu )
    {
        
        /*
         * Solve U*x=b
         */
        for(blockidx=ky-1; blockidx>=0; blockidx--)
        {
            for(blockidx1=1; blockidx1<=ae_minint(ky-(blockidx+1), blockbandwidth, _state); blockidx1++)
            {
                celloffset = spline2d_getcelloffset(kx, ky, blockbandwidth, blockidx, blockidx+blockidx1, _state);
                rmatrixgemv(kx, kx, -1.0, blockata, celloffset, 0, 0, b, (blockidx+blockidx1)*kx, 1.0, b, blockidx*kx, _state);
            }
            celloffset = spline2d_getcelloffset(kx, ky, blockbandwidth, blockidx, blockidx, _state);
            rmatrixtrsv(kx, blockata, celloffset, 0, ae_true, ae_false, 0, b, blockidx*kx, _state);
        }
    }
    else
    {
        
        /*
         * Solve U'*x=b
         */
        for(blockidx=0; blockidx<=ky-1; blockidx++)
        {
            celloffset = spline2d_getcelloffset(kx, ky, blockbandwidth, blockidx, blockidx, _state);
            rmatrixtrsv(kx, blockata, celloffset, 0, ae_true, ae_false, 1, b, blockidx*kx, _state);
            for(blockidx1=1; blockidx1<=ae_minint(ky-(blockidx+1), blockbandwidth, _state); blockidx1++)
            {
                celloffset = spline2d_getcelloffset(kx, ky, blockbandwidth, blockidx, blockidx+blockidx1, _state);
                rmatrixgemv(kx, kx, -1.0, blockata, celloffset, 0, 1, b, blockidx*kx, 1.0, b, (blockidx+blockidx1)*kx, _state);
            }
        }
    }
}


/*************************************************************************
This function computes residuals for dataset XY[], using array of original
values YRaw[], and loads residuals to XY.

Processing is performed in parallel manner.

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static void spline2d_computeresidualsfromscratch(/* Real    */ ae_vector* xy,
     /* Real    */ const ae_vector* yraw,
     ae_int_t npoints,
     ae_int_t d,
     ae_int_t scalexy,
     const spline2dinterpolant* spline,
     ae_state *_state)
{
    ae_frame _frame_block;
    srealarray seed;
    ae_shared_pool pool;
    ae_int_t chunksize;
    double pointcost;

    ae_frame_make(_state, &_frame_block);
    memset(&seed, 0, sizeof(seed));
    memset(&pool, 0, sizeof(pool));
    _srealarray_init(&seed, _state, ae_true);
    ae_shared_pool_init(&pool, _state, ae_true);

    
    /*
     * Setting up
     */
    chunksize = 1000;
    pointcost = 100.0;
    if( ae_fp_greater((double)npoints*pointcost,smpactivationlevel(_state)) )
    {
        if( _trypexec_spline2d_computeresidualsfromscratch(xy,yraw,npoints,d,scalexy,spline, _state) )
        {
            ae_frame_leave(_state);
            return;
        }
    }
    ae_shared_pool_set_seed(&pool, &seed, (ae_int_t)sizeof(seed), (ae_constructor)_srealarray_init, (ae_copy_constructor)_srealarray_init_copy, (ae_destructor)_srealarray_destroy, _state);
    
    /*
     * Call compute workhorse
     */
    spline2d_computeresidualsfromscratchrec(xy, yraw, 0, npoints, chunksize, d, scalexy, spline, &pool, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_spline2d_computeresidualsfromscratch(/* Real    */ ae_vector* xy,
    /* Real    */ const ae_vector* yraw,
    ae_int_t npoints,
    ae_int_t d,
    ae_int_t scalexy,
    const spline2dinterpolant* spline,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        spline2d_computeresidualsfromscratch(xy,yraw,npoints,d,scalexy,spline, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_spline2d_computeresidualsfromscratch;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = xy;
        _task->data.parameters[1].value.const_val = yraw;
        _task->data.parameters[2].value.ival = npoints;
        _task->data.parameters[3].value.ival = d;
        _task->data.parameters[4].value.ival = scalexy;
        _task->data.parameters[5].value.const_val = spline;
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
void _task_spline2d_computeresidualsfromscratch(ae_task_data *_data, ae_state *_state)
{
    ae_vector* xy;
    const ae_vector* yraw;
    ae_int_t npoints;
    ae_int_t d;
    ae_int_t scalexy;
    const spline2dinterpolant* spline;
    xy = (ae_vector*)_data->parameters[0].value.val;
    yraw = (const ae_vector*)_data->parameters[1].value.const_val;
    npoints = _data->parameters[2].value.ival;
    d = _data->parameters[3].value.ival;
    scalexy = _data->parameters[4].value.ival;
    spline = (const spline2dinterpolant*)_data->parameters[5].value.const_val;
   ae_state_set_flags(_state, _data->flags);
   spline2d_computeresidualsfromscratch(xy,yraw,npoints,d,scalexy,spline, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_spline2d_computeresidualsfromscratch(/* Real    */ ae_vector* xy,
    /* Real    */ const ae_vector* yraw,
    ae_int_t npoints,
    ae_int_t d,
    ae_int_t scalexy,
    const spline2dinterpolant* spline,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_spline2d_computeresidualsfromscratch;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = xy;
    _task->data.parameters[1].value.const_val = yraw;
    _task->data.parameters[2].value.ival = npoints;
    _task->data.parameters[3].value.ival = d;
    _task->data.parameters[4].value.ival = scalexy;
    _task->data.parameters[5].value.const_val = spline;
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
Recursive workhorse for ComputeResidualsFromScratch.

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static void spline2d_computeresidualsfromscratchrec(/* Real    */ ae_vector* xy,
     /* Real    */ const ae_vector* yraw,
     ae_int_t pt0,
     ae_int_t pt1,
     ae_int_t chunksize,
     ae_int_t d,
     ae_int_t scalexy,
     const spline2dinterpolant* spline,
     ae_shared_pool* pool,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t i;
    ae_int_t j;
    srealarray *pbuf;
    ae_smart_ptr _pbuf;
    ae_int_t xew;

    ae_frame_make(_state, &_frame_block);
    memset(&_pbuf, 0, sizeof(_pbuf));
    ae_smart_ptr_init(&_pbuf, (void**)&pbuf, _state, ae_true);

    xew = 2+d;
    
    /*
     * Parallelism
     */
    if( pt1-pt0>chunksize )
    {
        ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_true, _state);
        tiledsplit(pt1-pt0, chunksize, &i, &j, _state);
        _spawn_spline2d_computeresidualsfromscratchrec(xy, yraw, pt0, pt0+i, chunksize, d, scalexy, spline, pool, _child_tasks, _smp_enabled, _state);
        spline2d_computeresidualsfromscratchrec(xy, yraw, pt0+i, pt1, chunksize, d, scalexy, spline, pool, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Serial execution
     */
    ae_shared_pool_retrieve(pool, &_pbuf, _state);
    for(i=pt0; i<=pt1-1; i++)
    {
        spline2dcalcvbuf(spline, xy->ptr.p_double[i*xew+0]*(double)scalexy, xy->ptr.p_double[i*xew+1]*(double)scalexy, &pbuf->val, _state);
        for(j=0; j<=d-1; j++)
        {
            xy->ptr.p_double[i*xew+2+j] = yraw->ptr.p_double[i*d+j]-pbuf->val.ptr.p_double[j];
        }
    }
    ae_shared_pool_recycle(pool, &_pbuf, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_spline2d_computeresidualsfromscratchrec(/* Real    */ ae_vector* xy,
    /* Real    */ const ae_vector* yraw,
    ae_int_t pt0,
    ae_int_t pt1,
    ae_int_t chunksize,
    ae_int_t d,
    ae_int_t scalexy,
    const spline2dinterpolant* spline,
    ae_shared_pool* pool,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        spline2d_computeresidualsfromscratchrec(xy,yraw,pt0,pt1,chunksize,d,scalexy,spline,pool, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_spline2d_computeresidualsfromscratchrec;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = xy;
        _task->data.parameters[1].value.const_val = yraw;
        _task->data.parameters[2].value.ival = pt0;
        _task->data.parameters[3].value.ival = pt1;
        _task->data.parameters[4].value.ival = chunksize;
        _task->data.parameters[5].value.ival = d;
        _task->data.parameters[6].value.ival = scalexy;
        _task->data.parameters[7].value.const_val = spline;
        _task->data.parameters[8].value.val = pool;
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
void _task_spline2d_computeresidualsfromscratchrec(ae_task_data *_data, ae_state *_state)
{
    ae_vector* xy;
    const ae_vector* yraw;
    ae_int_t pt0;
    ae_int_t pt1;
    ae_int_t chunksize;
    ae_int_t d;
    ae_int_t scalexy;
    const spline2dinterpolant* spline;
    ae_shared_pool* pool;
    xy = (ae_vector*)_data->parameters[0].value.val;
    yraw = (const ae_vector*)_data->parameters[1].value.const_val;
    pt0 = _data->parameters[2].value.ival;
    pt1 = _data->parameters[3].value.ival;
    chunksize = _data->parameters[4].value.ival;
    d = _data->parameters[5].value.ival;
    scalexy = _data->parameters[6].value.ival;
    spline = (const spline2dinterpolant*)_data->parameters[7].value.const_val;
    pool = (ae_shared_pool*)_data->parameters[8].value.val;
   ae_state_set_flags(_state, _data->flags);
   spline2d_computeresidualsfromscratchrec(xy,yraw,pt0,pt1,chunksize,d,scalexy,spline,pool, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_spline2d_computeresidualsfromscratchrec(/* Real    */ ae_vector* xy,
    /* Real    */ const ae_vector* yraw,
    ae_int_t pt0,
    ae_int_t pt1,
    ae_int_t chunksize,
    ae_int_t d,
    ae_int_t scalexy,
    const spline2dinterpolant* spline,
    ae_shared_pool* pool,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_spline2d_computeresidualsfromscratchrec;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = xy;
    _task->data.parameters[1].value.const_val = yraw;
    _task->data.parameters[2].value.ival = pt0;
    _task->data.parameters[3].value.ival = pt1;
    _task->data.parameters[4].value.ival = chunksize;
    _task->data.parameters[5].value.ival = d;
    _task->data.parameters[6].value.ival = scalexy;
    _task->data.parameters[7].value.const_val = spline;
    _task->data.parameters[8].value.val = pool;
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
This function reorders dataset and builds index:
* it is assumed that all points have X in [0,KX-1], Y in [0,KY-1]
* area is divided into (KX-1)*(KY-1) cells
* all points are reordered in such way that points in same cell are stored
  contiguously
* dataset index, array[(KX-1)*(KY-1)+1], is generated. Points of cell I
  now have indexes XYIndex[I]..XYIndex[I+1]-1;

INPUT PARAMETERS:
    XY              -   array[NPoints*(2+D)], dataset
    KX, KY, D       -   grid size and dimensionality of the outputs
    Shadow          -   shadow array[NPoints*NS], which is sorted together
                        with XY; if NS=0, it is not referenced at all.
    NS              -   entry width of shadow array
    BufI            -   possibly preallocated temporary buffer; resized if
                        needed.

OUTPUT PARAMETERS:
    XY              -   reordered
    XYIndex         -   array[(KX-1)*(KY-1)+1], dataset index

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static void spline2d_reorderdatasetandbuildindex(/* Real    */ ae_vector* xy,
     ae_int_t npoints,
     ae_int_t d,
     /* Real    */ ae_vector* shadow,
     ae_int_t ns,
     ae_int_t kx,
     ae_int_t ky,
     /* Integer */ ae_vector* xyindex,
     /* Integer */ ae_vector* bufi,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t i0;
    ae_int_t i1;
    ae_int_t entrywidth;


    
    /*
     * Set up
     */
    ae_assert(kx>=2, "Spline2DFit.ReorderDatasetAndBuildIndex: integrity check failed", _state);
    ae_assert(ky>=2, "Spline2DFit.ReorderDatasetAndBuildIndex: integrity check failed", _state);
    entrywidth = 2+d;
    ivectorsetlengthatleast(xyindex, (kx-1)*(ky-1)+1, _state);
    ivectorsetlengthatleast(bufi, npoints, _state);
    for(i=0; i<=npoints-1; i++)
    {
        i0 = iboundval(ae_ifloor(xy->ptr.p_double[i*entrywidth+0], _state), 0, kx-2, _state);
        i1 = iboundval(ae_ifloor(xy->ptr.p_double[i*entrywidth+1], _state), 0, ky-2, _state);
        bufi->ptr.p_int[i] = i1*(kx-1)+i0;
    }
    
    /*
     * Reorder
     */
    spline2d_reorderdatasetandbuildindexrec(xy, d, shadow, ns, bufi, 0, npoints, xyindex, 0, (kx-1)*(ky-1), ae_true, _state);
    xyindex->ptr.p_int[(kx-1)*(ky-1)] = npoints;
}


/*************************************************************************
This function multiplies all points in dataset by 2.0 and rebuilds  index,
given previous index built for KX_prev=(KX-1)/2 and KY_prev=(KY-1)/2

INPUT PARAMETERS:
    XY              -   array[NPoints*(2+D)], dataset BEFORE scaling
    NPoints, D      -   dataset size and dimensionality of the outputs
    Shadow          -   shadow array[NPoints*NS], which is sorted together
                        with XY; if NS=0, it is not referenced at all.
    NS              -   entry width of shadow array
    KX, KY          -   new grid dimensionality
    XYIndex         -   index built for previous values of KX and KY
    BufI            -   possibly preallocated temporary buffer; resized if
                        needed.

OUTPUT PARAMETERS:
    XY              -   reordered and multiplied by 2.0
    XYIndex         -   array[(KX-1)*(KY-1)+1], dataset index

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static void spline2d_rescaledatasetandrefineindex(/* Real    */ ae_vector* xy,
     ae_int_t npoints,
     ae_int_t d,
     /* Real    */ ae_vector* shadow,
     ae_int_t ns,
     ae_int_t kx,
     ae_int_t ky,
     /* Integer */ ae_vector* xyindex,
     /* Integer */ ae_vector* bufi,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector xyindexprev;

    ae_frame_make(_state, &_frame_block);
    memset(&xyindexprev, 0, sizeof(xyindexprev));
    ae_vector_init(&xyindexprev, 0, DT_INT, _state, ae_true);

    
    /*
     * Set up
     */
    ae_assert(kx>=2, "Spline2DFit.RescaleDataset2AndRefineIndex: integrity check failed", _state);
    ae_assert(ky>=2, "Spline2DFit.RescaleDataset2AndRefineIndex: integrity check failed", _state);
    ae_assert((kx-1)%2==0, "Spline2DFit.RescaleDataset2AndRefineIndex: integrity check failed", _state);
    ae_assert((ky-1)%2==0, "Spline2DFit.RescaleDataset2AndRefineIndex: integrity check failed", _state);
    ae_swap_vectors(xyindex, &xyindexprev);
    ivectorsetlengthatleast(xyindex, (kx-1)*(ky-1)+1, _state);
    ivectorsetlengthatleast(bufi, npoints, _state);
    
    /*
     * Refine
     */
    spline2d_expandindexrows(xy, d, shadow, ns, bufi, 0, npoints, &xyindexprev, 0, (ky+1)/2-1, xyindex, kx, ky, ae_true, _state);
    xyindex->ptr.p_int[(kx-1)*(ky-1)] = npoints;
    
    /*
     * Integrity check
     */
    ae_frame_leave(_state);
}


/*************************************************************************
Recurrent divide-and-conquer indexing function

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static void spline2d_expandindexrows(/* Real    */ ae_vector* xy,
     ae_int_t d,
     /* Real    */ ae_vector* shadow,
     ae_int_t ns,
     /* Integer */ ae_vector* cidx,
     ae_int_t pt0,
     ae_int_t pt1,
     /* Integer */ ae_vector* xyindexprev,
     ae_int_t row0,
     ae_int_t row1,
     /* Integer */ ae_vector* xyindexnew,
     ae_int_t kxnew,
     ae_int_t kynew,
     ae_bool rootcall,
     ae_state *_state)
{
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t i;
    ae_int_t entrywidth;
    ae_int_t kxprev;
    double v;
    ae_int_t i0;
    ae_int_t i1;
    double efficiency;
    double cost;
    ae_int_t rowmid;


    kxprev = (kxnew+1)/2;
    entrywidth = 2+d;
    efficiency = 0.1;
    cost = (double)(d*(pt1-pt0+1))*(ae_log((double)(kxnew), _state)/ae_log((double)(2), _state))/efficiency;
    ae_assert(xyindexprev->ptr.p_int[row0*(kxprev-1)+0]==pt0, "Spline2DFit.ExpandIndexRows: integrity check failed", _state);
    ae_assert(xyindexprev->ptr.p_int[row1*(kxprev-1)+0]==pt1, "Spline2DFit.ExpandIndexRows: integrity check failed", _state);
    
    /*
     * Parallelism
     */
    if( ((rootcall&&pt1-pt0>10000)&&row1-row0>=2)&&ae_fp_greater(cost,smpactivationlevel(_state)) )
    {
        if( _trypexec_spline2d_expandindexrows(xy,d,shadow,ns,cidx,pt0,pt1,xyindexprev,row0,row1,xyindexnew,kxnew,kynew,rootcall, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    
    /*
     * Partition
     */
    if( row1-row0>=2 )
    {
        ae_set_smp_support(&_child_tasks, &_smp_enabled, pt1-pt0>10000&&ae_fp_greater(cost,spawnlevel(_state)), _state);
        tiledsplit(row1-row0, 1, &i0, &i1, _state);
        rowmid = row0+i0;
        _spawn_spline2d_expandindexrows(xy, d, shadow, ns, cidx, pt0, xyindexprev->ptr.p_int[rowmid*(kxprev-1)+0], xyindexprev, row0, rowmid, xyindexnew, kxnew, kynew, ae_false, _child_tasks, _smp_enabled, _state);
        spline2d_expandindexrows(xy, d, shadow, ns, cidx, xyindexprev->ptr.p_int[rowmid*(kxprev-1)+0], pt1, xyindexprev, rowmid, row1, xyindexnew, kxnew, kynew, ae_false, _state);
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Serial execution
     */
    for(i=pt0; i<=pt1-1; i++)
    {
        v = (double)2*xy->ptr.p_double[i*entrywidth+0];
        xy->ptr.p_double[i*entrywidth+0] = v;
        i0 = iboundval(ae_ifloor(v, _state), 0, kxnew-2, _state);
        v = (double)2*xy->ptr.p_double[i*entrywidth+1];
        xy->ptr.p_double[i*entrywidth+1] = v;
        i1 = iboundval(ae_ifloor(v, _state), 0, kynew-2, _state);
        cidx->ptr.p_int[i] = i1*(kxnew-1)+i0;
    }
    spline2d_reorderdatasetandbuildindexrec(xy, d, shadow, ns, cidx, pt0, pt1, xyindexnew, 2*row0*(kxnew-1)+0, 2*row1*(kxnew-1)+0, ae_false, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_spline2d_expandindexrows(/* Real    */ ae_vector* xy,
    ae_int_t d,
    /* Real    */ ae_vector* shadow,
    ae_int_t ns,
    /* Integer */ ae_vector* cidx,
    ae_int_t pt0,
    ae_int_t pt1,
    /* Integer */ ae_vector* xyindexprev,
    ae_int_t row0,
    ae_int_t row1,
    /* Integer */ ae_vector* xyindexnew,
    ae_int_t kxnew,
    ae_int_t kynew,
    ae_bool rootcall,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        spline2d_expandindexrows(xy,d,shadow,ns,cidx,pt0,pt1,xyindexprev,row0,row1,xyindexnew,kxnew,kynew,rootcall, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_spline2d_expandindexrows;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = xy;
        _task->data.parameters[1].value.ival = d;
        _task->data.parameters[2].value.val = shadow;
        _task->data.parameters[3].value.ival = ns;
        _task->data.parameters[4].value.val = cidx;
        _task->data.parameters[5].value.ival = pt0;
        _task->data.parameters[6].value.ival = pt1;
        _task->data.parameters[7].value.val = xyindexprev;
        _task->data.parameters[8].value.ival = row0;
        _task->data.parameters[9].value.ival = row1;
        _task->data.parameters[10].value.val = xyindexnew;
        _task->data.parameters[11].value.ival = kxnew;
        _task->data.parameters[12].value.ival = kynew;
        _task->data.parameters[13].value.bval = rootcall;
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
void _task_spline2d_expandindexrows(ae_task_data *_data, ae_state *_state)
{
    ae_vector* xy;
    ae_int_t d;
    ae_vector* shadow;
    ae_int_t ns;
    ae_vector* cidx;
    ae_int_t pt0;
    ae_int_t pt1;
    ae_vector* xyindexprev;
    ae_int_t row0;
    ae_int_t row1;
    ae_vector* xyindexnew;
    ae_int_t kxnew;
    ae_int_t kynew;
    ae_bool rootcall;
    xy = (ae_vector*)_data->parameters[0].value.val;
    d = _data->parameters[1].value.ival;
    shadow = (ae_vector*)_data->parameters[2].value.val;
    ns = _data->parameters[3].value.ival;
    cidx = (ae_vector*)_data->parameters[4].value.val;
    pt0 = _data->parameters[5].value.ival;
    pt1 = _data->parameters[6].value.ival;
    xyindexprev = (ae_vector*)_data->parameters[7].value.val;
    row0 = _data->parameters[8].value.ival;
    row1 = _data->parameters[9].value.ival;
    xyindexnew = (ae_vector*)_data->parameters[10].value.val;
    kxnew = _data->parameters[11].value.ival;
    kynew = _data->parameters[12].value.ival;
    rootcall = _data->parameters[13].value.bval;
   ae_state_set_flags(_state, _data->flags);
   spline2d_expandindexrows(xy,d,shadow,ns,cidx,pt0,pt1,xyindexprev,row0,row1,xyindexnew,kxnew,kynew,rootcall, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_spline2d_expandindexrows(/* Real    */ ae_vector* xy,
    ae_int_t d,
    /* Real    */ ae_vector* shadow,
    ae_int_t ns,
    /* Integer */ ae_vector* cidx,
    ae_int_t pt0,
    ae_int_t pt1,
    /* Integer */ ae_vector* xyindexprev,
    ae_int_t row0,
    ae_int_t row1,
    /* Integer */ ae_vector* xyindexnew,
    ae_int_t kxnew,
    ae_int_t kynew,
    ae_bool rootcall,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_spline2d_expandindexrows;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = xy;
    _task->data.parameters[1].value.ival = d;
    _task->data.parameters[2].value.val = shadow;
    _task->data.parameters[3].value.ival = ns;
    _task->data.parameters[4].value.val = cidx;
    _task->data.parameters[5].value.ival = pt0;
    _task->data.parameters[6].value.ival = pt1;
    _task->data.parameters[7].value.val = xyindexprev;
    _task->data.parameters[8].value.ival = row0;
    _task->data.parameters[9].value.ival = row1;
    _task->data.parameters[10].value.val = xyindexnew;
    _task->data.parameters[11].value.ival = kxnew;
    _task->data.parameters[12].value.ival = kynew;
    _task->data.parameters[13].value.bval = rootcall;
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
Recurrent divide-and-conquer indexing function

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static void spline2d_reorderdatasetandbuildindexrec(/* Real    */ ae_vector* xy,
     ae_int_t d,
     /* Real    */ ae_vector* shadow,
     ae_int_t ns,
     /* Integer */ ae_vector* cidx,
     ae_int_t pt0,
     ae_int_t pt1,
     /* Integer */ ae_vector* xyindex,
     ae_int_t idx0,
     ae_int_t idx1,
     ae_bool rootcall,
     ae_state *_state)
{
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t entrywidth;
    ae_int_t idxmid;
    ae_int_t wrk0;
    ae_int_t wrk1;
    double efficiency;
    double cost;


    
    /*
     * Efficiency - performance of the code when compared with that
     * of linear algebra code.
     */
    entrywidth = 2+d;
    efficiency = 0.1;
    cost = (double)(d*(pt1-pt0+1))*ae_log((double)(idx1-idx0+1), _state)/ae_log((double)(2), _state)/efficiency;
    
    /*
     * Parallelism
     */
    if( ((rootcall&&pt1-pt0>10000)&&idx1-idx0>=2)&&ae_fp_greater(cost,smpactivationlevel(_state)) )
    {
        if( _trypexec_spline2d_reorderdatasetandbuildindexrec(xy,d,shadow,ns,cidx,pt0,pt1,xyindex,idx0,idx1,rootcall, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    
    /*
     * Store left bound to XYIndex
     */
    xyindex->ptr.p_int[idx0] = pt0;
    
    /*
     * Quick exit strategies
     */
    if( idx1<=idx0+1 )
    {
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    if( pt0==pt1 )
    {
        for(idxmid=idx0+1; idxmid<=idx1-1; idxmid++)
        {
            xyindex->ptr.p_int[idxmid] = pt1;
        }
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Select middle element
     */
    idxmid = idx0+(idx1-idx0)/2;
    ae_assert(idx0<idxmid&&idxmid<idx1, "Spline2D: integrity check failed", _state);
    wrk0 = pt0;
    wrk1 = pt1-1;
    for(;;)
    {
        while(wrk0<pt1&&cidx->ptr.p_int[wrk0]<idxmid)
        {
            wrk0 = wrk0+1;
        }
        while(wrk1>=pt0&&cidx->ptr.p_int[wrk1]>=idxmid)
        {
            wrk1 = wrk1-1;
        }
        if( wrk1<=wrk0 )
        {
            break;
        }
        swapentries(xy, wrk0, wrk1, entrywidth, _state);
        if( ns>0 )
        {
            swapentries(shadow, wrk0, wrk1, ns, _state);
        }
        swapelementsi(cidx, wrk0, wrk1, _state);
    }
    ae_set_smp_support(&_child_tasks, &_smp_enabled, (pt1-pt0>10000&&idx1-idx0>=2)&&ae_fp_greater(cost,spawnlevel(_state)), _state);
    _spawn_spline2d_reorderdatasetandbuildindexrec(xy, d, shadow, ns, cidx, pt0, wrk0, xyindex, idx0, idxmid, ae_false, _child_tasks, _smp_enabled, _state);
    spline2d_reorderdatasetandbuildindexrec(xy, d, shadow, ns, cidx, wrk0, pt1, xyindex, idxmid, idx1, ae_false, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_spline2d_reorderdatasetandbuildindexrec(/* Real    */ ae_vector* xy,
    ae_int_t d,
    /* Real    */ ae_vector* shadow,
    ae_int_t ns,
    /* Integer */ ae_vector* cidx,
    ae_int_t pt0,
    ae_int_t pt1,
    /* Integer */ ae_vector* xyindex,
    ae_int_t idx0,
    ae_int_t idx1,
    ae_bool rootcall,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        spline2d_reorderdatasetandbuildindexrec(xy,d,shadow,ns,cidx,pt0,pt1,xyindex,idx0,idx1,rootcall, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_spline2d_reorderdatasetandbuildindexrec;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = xy;
        _task->data.parameters[1].value.ival = d;
        _task->data.parameters[2].value.val = shadow;
        _task->data.parameters[3].value.ival = ns;
        _task->data.parameters[4].value.val = cidx;
        _task->data.parameters[5].value.ival = pt0;
        _task->data.parameters[6].value.ival = pt1;
        _task->data.parameters[7].value.val = xyindex;
        _task->data.parameters[8].value.ival = idx0;
        _task->data.parameters[9].value.ival = idx1;
        _task->data.parameters[10].value.bval = rootcall;
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
void _task_spline2d_reorderdatasetandbuildindexrec(ae_task_data *_data, ae_state *_state)
{
    ae_vector* xy;
    ae_int_t d;
    ae_vector* shadow;
    ae_int_t ns;
    ae_vector* cidx;
    ae_int_t pt0;
    ae_int_t pt1;
    ae_vector* xyindex;
    ae_int_t idx0;
    ae_int_t idx1;
    ae_bool rootcall;
    xy = (ae_vector*)_data->parameters[0].value.val;
    d = _data->parameters[1].value.ival;
    shadow = (ae_vector*)_data->parameters[2].value.val;
    ns = _data->parameters[3].value.ival;
    cidx = (ae_vector*)_data->parameters[4].value.val;
    pt0 = _data->parameters[5].value.ival;
    pt1 = _data->parameters[6].value.ival;
    xyindex = (ae_vector*)_data->parameters[7].value.val;
    idx0 = _data->parameters[8].value.ival;
    idx1 = _data->parameters[9].value.ival;
    rootcall = _data->parameters[10].value.bval;
   ae_state_set_flags(_state, _data->flags);
   spline2d_reorderdatasetandbuildindexrec(xy,d,shadow,ns,cidx,pt0,pt1,xyindex,idx0,idx1,rootcall, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_spline2d_reorderdatasetandbuildindexrec(/* Real    */ ae_vector* xy,
    ae_int_t d,
    /* Real    */ ae_vector* shadow,
    ae_int_t ns,
    /* Integer */ ae_vector* cidx,
    ae_int_t pt0,
    ae_int_t pt1,
    /* Integer */ ae_vector* xyindex,
    ae_int_t idx0,
    ae_int_t idx1,
    ae_bool rootcall,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_spline2d_reorderdatasetandbuildindexrec;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = xy;
    _task->data.parameters[1].value.ival = d;
    _task->data.parameters[2].value.val = shadow;
    _task->data.parameters[3].value.ival = ns;
    _task->data.parameters[4].value.val = cidx;
    _task->data.parameters[5].value.ival = pt0;
    _task->data.parameters[6].value.ival = pt1;
    _task->data.parameters[7].value.val = xyindex;
    _task->data.parameters[8].value.ival = idx0;
    _task->data.parameters[9].value.ival = idx1;
    _task->data.parameters[10].value.bval = rootcall;
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
This function performs fitting with  BlockLLS solver.  Internal  function,
never use it directly.

INPUT PARAMETERS:
    XY      -   dataset, array[NPoints,2+D]
    XYIndex -   dataset index, see ReorderDatasetAndBuildIndex() for more info
    KX0, KX1-   X-indices of basis functions to select and fit;
                range [KX0,KX1) is processed
    KXTotal -   total number of indexes in the entire grid
    KY0, KY1-   Y-indices of basis functions to select and fit;
                range [KY0,KY1) is processed
    KYTotal -   total number of indexes in the entire grid
    D       -   number of components in vector-valued spline, D>=1
    LambdaReg-  regularization coefficient
    LambdaNS-   nonlinearity penalty, exactly zero value is specially handled
                (entire set of rows is not added to the matrix)
    Basis1  -   single-dimensional B-spline
    

OUTPUT PARAMETERS:
    A       -   design matrix

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static void spline2d_xdesigngenerate(/* Real    */ const ae_vector* xy,
     /* Integer */ const ae_vector* xyindex,
     ae_int_t kx0,
     ae_int_t kx1,
     ae_int_t kxtotal,
     ae_int_t ky0,
     ae_int_t ky1,
     ae_int_t kytotal,
     ae_int_t d,
     double lambdareg,
     double lambdans,
     const spline1dinterpolant* basis1,
     spline2dxdesignmatrix* a,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t entrywidth;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t k0;
    ae_int_t k1;
    ae_int_t kx;
    ae_int_t ky;
    ae_int_t rowsdone;
    ae_int_t batchesdone;
    ae_int_t pt0;
    ae_int_t pt1;
    ae_int_t base0;
    ae_int_t base1;
    ae_int_t baseidx;
    ae_int_t nzshift;
    ae_int_t nzwidth;
    ae_matrix d2x;
    ae_matrix d2y;
    ae_matrix dxy;
    double v;
    double v0;
    double v1;
    double v2;
    double w0;
    double w1;
    double w2;

    ae_frame_make(_state, &_frame_block);
    memset(&d2x, 0, sizeof(d2x));
    memset(&d2y, 0, sizeof(d2y));
    memset(&dxy, 0, sizeof(dxy));
    ae_matrix_init(&d2x, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&d2y, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&dxy, 0, 0, DT_REAL, _state, ae_true);

    nzshift = 1;
    nzwidth = 4;
    entrywidth = 2+d;
    kx = kx1-kx0;
    ky = ky1-ky0;
    a->lambdareg = lambdareg;
    a->blockwidth = 4;
    a->kx = kx;
    a->ky = ky;
    a->d = d;
    a->npoints = 0;
    a->ndenserows = 0;
    a->ndensebatches = 0;
    a->maxbatch = 0;
    for(j1=ky0; j1<=ky1-2; j1++)
    {
        for(j0=kx0; j0<=kx1-2; j0++)
        {
            i = xyindex->ptr.p_int[j1*(kxtotal-1)+j0+1]-xyindex->ptr.p_int[j1*(kxtotal-1)+j0];
            a->npoints = a->npoints+i;
            a->ndenserows = a->ndenserows+i;
            a->ndensebatches = a->ndensebatches+1;
            a->maxbatch = ae_maxint(a->maxbatch, i, _state);
        }
    }
    if( ae_fp_neq(lambdans,(double)(0)) )
    {
        ae_assert(ae_fp_greater_eq(lambdans,(double)(0)), "Spline2DFit: integrity check failed", _state);
        a->ndenserows = a->ndenserows+3*(kx-2)*(ky-2);
        a->ndensebatches = a->ndensebatches+(kx-2)*(ky-2);
        a->maxbatch = ae_maxint(a->maxbatch, 3, _state);
    }
    a->nrows = a->ndenserows+kx*ky;
    rmatrixsetlengthatleast(&a->vals, a->ndenserows, a->blockwidth*a->blockwidth+d, _state);
    ivectorsetlengthatleast(&a->batches, a->ndensebatches+1, _state);
    ivectorsetlengthatleast(&a->batchbases, a->ndensebatches, _state);
    
    /*
     * Setup output counters
     */
    batchesdone = 0;
    rowsdone = 0;
    
    /*
     * Generate rows corresponding to dataset points
     */
    ae_assert(kx>=nzwidth, "Spline2DFit: integrity check failed", _state);
    ae_assert(ky>=nzwidth, "Spline2DFit: integrity check failed", _state);
    rvectorsetlengthatleast(&a->tmp0, nzwidth, _state);
    rvectorsetlengthatleast(&a->tmp1, nzwidth, _state);
    a->batches.ptr.p_int[batchesdone] = 0;
    for(j1=ky0; j1<=ky1-2; j1++)
    {
        for(j0=kx0; j0<=kx1-2; j0++)
        {
            pt0 = xyindex->ptr.p_int[j1*(kxtotal-1)+j0];
            pt1 = xyindex->ptr.p_int[j1*(kxtotal-1)+j0+1];
            base0 = iboundval(j0-kx0-nzshift, 0, kx-nzwidth, _state);
            base1 = iboundval(j1-ky0-nzshift, 0, ky-nzwidth, _state);
            baseidx = base1*kx+base0;
            a->batchbases.ptr.p_int[batchesdone] = baseidx;
            for(i=pt0; i<=pt1-1; i++)
            {
                for(k0=0; k0<=nzwidth-1; k0++)
                {
                    a->tmp0.ptr.p_double[k0] = spline1dcalc(basis1, xy->ptr.p_double[i*entrywidth+0]-(double)(base0+kx0+k0), _state);
                }
                for(k1=0; k1<=nzwidth-1; k1++)
                {
                    a->tmp1.ptr.p_double[k1] = spline1dcalc(basis1, xy->ptr.p_double[i*entrywidth+1]-(double)(base1+ky0+k1), _state);
                }
                for(k1=0; k1<=nzwidth-1; k1++)
                {
                    for(k0=0; k0<=nzwidth-1; k0++)
                    {
                        a->vals.ptr.pp_double[rowsdone][k1*nzwidth+k0] = a->tmp0.ptr.p_double[k0]*a->tmp1.ptr.p_double[k1];
                    }
                }
                for(j=0; j<=d-1; j++)
                {
                    a->vals.ptr.pp_double[rowsdone][nzwidth*nzwidth+j] = xy->ptr.p_double[i*entrywidth+2+j];
                }
                rowsdone = rowsdone+1;
            }
            batchesdone = batchesdone+1;
            a->batches.ptr.p_int[batchesdone] = rowsdone;
        }
    }
    
    /*
     * Generate rows corresponding to nonlinearity penalty
     */
    if( ae_fp_greater(lambdans,(double)(0)) )
    {
        
        /*
         * Smoothing is applied. Because all grid nodes are same,
         * we apply same smoothing kernel, which is calculated only
         * once at the beginning of design matrix generation.
         */
        ae_matrix_set_length(&d2x, 3, 3, _state);
        ae_matrix_set_length(&d2y, 3, 3, _state);
        ae_matrix_set_length(&dxy, 3, 3, _state);
        for(j1=0; j1<=2; j1++)
        {
            for(j0=0; j0<=2; j0++)
            {
                d2x.ptr.pp_double[j0][j1] = 0.0;
                d2y.ptr.pp_double[j0][j1] = 0.0;
                dxy.ptr.pp_double[j0][j1] = 0.0;
            }
        }
        for(k1=0; k1<=2; k1++)
        {
            for(k0=0; k0<=2; k0++)
            {
                spline1ddiff(basis1, (double)(-(k0-1)), &v0, &v1, &v2, _state);
                spline1ddiff(basis1, (double)(-(k1-1)), &w0, &w1, &w2, _state);
                d2x.ptr.pp_double[k0][k1] = d2x.ptr.pp_double[k0][k1]+v2*w0;
                d2y.ptr.pp_double[k0][k1] = d2y.ptr.pp_double[k0][k1]+w2*v0;
                dxy.ptr.pp_double[k0][k1] = dxy.ptr.pp_double[k0][k1]+v1*w1;
            }
        }
        
        /*
         * Now, kernel is ready - apply it to all inner nodes of the grid.
         */
        for(j1=1; j1<=ky-2; j1++)
        {
            for(j0=1; j0<=kx-2; j0++)
            {
                base0 = imax2(j0-2, 0, _state);
                base1 = imax2(j1-2, 0, _state);
                baseidx = base1*kx+base0;
                a->batchbases.ptr.p_int[batchesdone] = baseidx;
                
                /*
                 * d2F/dx2 term
                 */
                v = lambdans;
                for(j=0; j<=nzwidth*nzwidth+d-1; j++)
                {
                    a->vals.ptr.pp_double[rowsdone][j] = (double)(0);
                }
                for(k1=j1-1; k1<=j1+1; k1++)
                {
                    for(k0=j0-1; k0<=j0+1; k0++)
                    {
                        a->vals.ptr.pp_double[rowsdone][nzwidth*(k1-base1)+(k0-base0)] = v*d2x.ptr.pp_double[1+(k0-j0)][1+(k1-j1)];
                    }
                }
                rowsdone = rowsdone+1;
                
                /*
                 * d2F/dy2 term
                 */
                v = lambdans;
                for(j=0; j<=nzwidth*nzwidth+d-1; j++)
                {
                    a->vals.ptr.pp_double[rowsdone][j] = (double)(0);
                }
                for(k1=j1-1; k1<=j1+1; k1++)
                {
                    for(k0=j0-1; k0<=j0+1; k0++)
                    {
                        a->vals.ptr.pp_double[rowsdone][nzwidth*(k1-base1)+(k0-base0)] = v*d2y.ptr.pp_double[1+(k0-j0)][1+(k1-j1)];
                    }
                }
                rowsdone = rowsdone+1;
                
                /*
                 * 2*d2F/dxdy term
                 */
                v = ae_sqrt((double)(2), _state)*lambdans;
                for(j=0; j<=nzwidth*nzwidth+d-1; j++)
                {
                    a->vals.ptr.pp_double[rowsdone][j] = (double)(0);
                }
                for(k1=j1-1; k1<=j1+1; k1++)
                {
                    for(k0=j0-1; k0<=j0+1; k0++)
                    {
                        a->vals.ptr.pp_double[rowsdone][nzwidth*(k1-base1)+(k0-base0)] = v*dxy.ptr.pp_double[1+(k0-j0)][1+(k1-j1)];
                    }
                }
                rowsdone = rowsdone+1;
                batchesdone = batchesdone+1;
                a->batches.ptr.p_int[batchesdone] = rowsdone;
            }
        }
    }
    
    /*
     * Integrity post-check
     */
    ae_assert(batchesdone==a->ndensebatches, "Spline2DFit: integrity check failed", _state);
    ae_assert(rowsdone==a->ndenserows, "Spline2DFit: integrity check failed", _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function performs matrix-vector product of design matrix and dense
vector.

INPUT PARAMETERS:
    A       -   design matrix, (a.nrows) X (a.kx*a.ky);
                some fields of A are used for temporaries,
                so it is non-constant.
    X       -   array[A.KX*A.KY]
    

OUTPUT PARAMETERS:
    Y       -   product, array[A.NRows], automatically allocated

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static void spline2d_xdesignmv(spline2dxdesignmatrix* a,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t bidx;
    ae_int_t i;
    ae_int_t cnt;
    double v;
    ae_int_t baseidx;
    ae_int_t outidx;
    ae_int_t batchsize;
    ae_int_t kx;
    ae_int_t k0;
    ae_int_t k1;
    ae_int_t nzwidth;


    nzwidth = 4;
    ae_assert(a->blockwidth==nzwidth, "Spline2DFit: integrity check failed", _state);
    ae_assert(x->cnt>=a->kx*a->ky, "Spline2DFit: integrity check failed", _state);
    
    /*
     * Prepare
     */
    rvectorsetlengthatleast(y, a->nrows, _state);
    rvectorsetlengthatleast(&a->tmp0, nzwidth*nzwidth, _state);
    rvectorsetlengthatleast(&a->tmp1, a->maxbatch, _state);
    kx = a->kx;
    outidx = 0;
    
    /*
     * Process dense part
     */
    for(bidx=0; bidx<=a->ndensebatches-1; bidx++)
    {
        if( a->batches.ptr.p_int[bidx+1]-a->batches.ptr.p_int[bidx]>0 )
        {
            batchsize = a->batches.ptr.p_int[bidx+1]-a->batches.ptr.p_int[bidx];
            baseidx = a->batchbases.ptr.p_int[bidx];
            for(k1=0; k1<=nzwidth-1; k1++)
            {
                for(k0=0; k0<=nzwidth-1; k0++)
                {
                    a->tmp0.ptr.p_double[k1*nzwidth+k0] = x->ptr.p_double[baseidx+k1*kx+k0];
                }
            }
            rmatrixgemv(batchsize, nzwidth*nzwidth, 1.0, &a->vals, a->batches.ptr.p_int[bidx], 0, 0, &a->tmp0, 0, 0.0, &a->tmp1, 0, _state);
            for(i=0; i<=batchsize-1; i++)
            {
                y->ptr.p_double[outidx+i] = a->tmp1.ptr.p_double[i];
            }
            outidx = outidx+batchsize;
        }
    }
    ae_assert(outidx==a->ndenserows, "Spline2DFit: integrity check failed", _state);
    
    /*
     * Process regularizer 
     */
    v = a->lambdareg;
    cnt = a->kx*a->ky;
    for(i=0; i<=cnt-1; i++)
    {
        y->ptr.p_double[outidx+i] = v*x->ptr.p_double[i];
    }
    outidx = outidx+cnt;
    
    /*
     * Post-check
     */
    ae_assert(outidx==a->nrows, "Spline2DFit: integrity check failed", _state);
}


/*************************************************************************
This function performs matrix-vector product of transposed design matrix and dense
vector.

INPUT PARAMETERS:
    A       -   design matrix, (a.nrows) X (a.kx*a.ky);
                some fields of A are used for temporaries,
                so it is non-constant.
    X       -   array[A.NRows]
    

OUTPUT PARAMETERS:
    Y       -   product, array[A.KX*A.KY], automatically allocated

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static void spline2d_xdesignmtv(spline2dxdesignmatrix* a,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t bidx;
    ae_int_t i;
    ae_int_t cnt;
    double v;
    ae_int_t baseidx;
    ae_int_t inidx;
    ae_int_t batchsize;
    ae_int_t kx;
    ae_int_t k0;
    ae_int_t k1;
    ae_int_t nzwidth;


    nzwidth = 4;
    ae_assert(a->blockwidth==nzwidth, "Spline2DFit: integrity check failed", _state);
    ae_assert(x->cnt>=a->nrows, "Spline2DFit: integrity check failed", _state);
    
    /*
     * Prepare
     */
    rvectorsetlengthatleast(y, a->kx*a->ky, _state);
    rvectorsetlengthatleast(&a->tmp0, nzwidth*nzwidth, _state);
    rvectorsetlengthatleast(&a->tmp1, a->maxbatch, _state);
    kx = a->kx;
    inidx = 0;
    cnt = a->kx*a->ky;
    for(i=0; i<=cnt-1; i++)
    {
        y->ptr.p_double[i] = (double)(0);
    }
    
    /*
     * Process dense part
     */
    for(bidx=0; bidx<=a->ndensebatches-1; bidx++)
    {
        if( a->batches.ptr.p_int[bidx+1]-a->batches.ptr.p_int[bidx]>0 )
        {
            batchsize = a->batches.ptr.p_int[bidx+1]-a->batches.ptr.p_int[bidx];
            baseidx = a->batchbases.ptr.p_int[bidx];
            for(i=0; i<=batchsize-1; i++)
            {
                a->tmp1.ptr.p_double[i] = x->ptr.p_double[inidx+i];
            }
            rmatrixgemv(nzwidth*nzwidth, batchsize, 1.0, &a->vals, a->batches.ptr.p_int[bidx], 0, 1, &a->tmp1, 0, 0.0, &a->tmp0, 0, _state);
            for(k1=0; k1<=nzwidth-1; k1++)
            {
                for(k0=0; k0<=nzwidth-1; k0++)
                {
                    y->ptr.p_double[baseidx+k1*kx+k0] = y->ptr.p_double[baseidx+k1*kx+k0]+a->tmp0.ptr.p_double[k1*nzwidth+k0];
                }
            }
            inidx = inidx+batchsize;
        }
    }
    ae_assert(inidx==a->ndenserows, "Spline2DFit: integrity check failed", _state);
    
    /*
     * Process regularizer 
     */
    v = a->lambdareg;
    cnt = a->kx*a->ky;
    for(i=0; i<=cnt-1; i++)
    {
        y->ptr.p_double[i] = y->ptr.p_double[i]+v*x->ptr.p_double[inidx+i];
    }
    inidx = inidx+cnt;
    
    /*
     * Post-check
     */
    ae_assert(inidx==a->nrows, "Spline2DFit: integrity check failed", _state);
}


/*************************************************************************
This function generates squared design matrix stored in block band format.

We  use  an   adaptation   of   block   skyline   storage   format,   with
TOWERSIZE*KX   skyline    bands   (towers)   stored   sequentially;   here
TOWERSIZE=(BlockBandwidth+1)*KX. So, we have KY "towers", stored one below
other, in BlockATA matrix. Every "tower" is a sequence of BlockBandwidth+1
cells, each of them being KX*KX in size.

INPUT PARAMETERS:
    A       -   design matrix; some of its fields are used for temporaries
    BlockATA-   array[KY*(BlockBandwidth+1)*KX,KX],  preallocated  storage
                for output matrix in compressed block band format

OUTPUT PARAMETERS:
    BlockATA-   AH*AH', stored in compressed block band format
    MXATA   -   max(|AH*AH'|), elementwise

  -- ALGLIB --
     Copyright 05.02.2018 by Bochkanov Sergey
*************************************************************************/
static void spline2d_xdesignblockata(spline2dxdesignmatrix* a,
     /* Real    */ ae_matrix* blockata,
     double* mxata,
     ae_state *_state)
{
    ae_int_t blockbandwidth;
    ae_int_t nzwidth;
    ae_int_t kx;
    ae_int_t ky;
    ae_int_t i0;
    ae_int_t i1;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t celloffset;
    ae_int_t bidx;
    ae_int_t baseidx;
    ae_int_t batchsize;
    ae_int_t offs0;
    ae_int_t offs1;
    double v;


    blockbandwidth = 3;
    nzwidth = 4;
    kx = a->kx;
    ky = a->ky;
    ae_assert(a->blockwidth==nzwidth, "Spline2DFit: integrity check failed", _state);
    rmatrixsetlengthatleast(&a->tmp2, nzwidth*nzwidth, nzwidth*nzwidth, _state);
    
    /*
     * Initial zero-fill:
     * * zero-fill ALL elements of BlockATA
     * * zero-fill ALL elements of Tmp2
     *
     * Filling ALL elements, including unused ones, is essential for the
     * purposes of calculating max(BlockATA).
     */
    for(i1=0; i1<=ky-1; i1++)
    {
        for(i0=i1; i0<=ae_minint(ky-1, i1+blockbandwidth, _state); i0++)
        {
            celloffset = spline2d_getcelloffset(kx, ky, blockbandwidth, i1, i0, _state);
            for(j1=0; j1<=kx-1; j1++)
            {
                for(j0=0; j0<=kx-1; j0++)
                {
                    blockata->ptr.pp_double[celloffset+j1][j0] = 0.0;
                }
            }
        }
    }
    for(j1=0; j1<=nzwidth*nzwidth-1; j1++)
    {
        for(j0=0; j0<=nzwidth*nzwidth-1; j0++)
        {
            a->tmp2.ptr.pp_double[j1][j0] = 0.0;
        }
    }
    
    /*
     * Process dense part of A
     */
    for(bidx=0; bidx<=a->ndensebatches-1; bidx++)
    {
        if( a->batches.ptr.p_int[bidx+1]-a->batches.ptr.p_int[bidx]>0 )
        {
            
            /*
             * Generate 16x16 U = BATCH'*BATCH and add it to ATA.
             *
             * NOTE: it is essential that lower triangle of Tmp2 is
             *       filled by zeros.
             */
            batchsize = a->batches.ptr.p_int[bidx+1]-a->batches.ptr.p_int[bidx];
            rmatrixsyrk(nzwidth*nzwidth, batchsize, 1.0, &a->vals, a->batches.ptr.p_int[bidx], 0, 2, 0.0, &a->tmp2, 0, 0, ae_true, _state);
            baseidx = a->batchbases.ptr.p_int[bidx];
            for(i1=0; i1<=nzwidth-1; i1++)
            {
                for(j1=i1; j1<=nzwidth-1; j1++)
                {
                    celloffset = spline2d_getcelloffset(kx, ky, blockbandwidth, baseidx/kx+i1, baseidx/kx+j1, _state);
                    offs0 = baseidx%kx;
                    offs1 = baseidx%kx;
                    for(i0=0; i0<=nzwidth-1; i0++)
                    {
                        for(j0=0; j0<=nzwidth-1; j0++)
                        {
                            v = a->tmp2.ptr.pp_double[i1*nzwidth+i0][j1*nzwidth+j0];
                            blockata->ptr.pp_double[celloffset+offs1+i0][offs0+j0] = blockata->ptr.pp_double[celloffset+offs1+i0][offs0+j0]+v;
                        }
                    }
                }
            }
        }
    }
    
    /*
     * Process regularizer term
     */
    for(i1=0; i1<=ky-1; i1++)
    {
        celloffset = spline2d_getcelloffset(kx, ky, blockbandwidth, i1, i1, _state);
        for(j1=0; j1<=kx-1; j1++)
        {
            blockata->ptr.pp_double[celloffset+j1][j1] = blockata->ptr.pp_double[celloffset+j1][j1]+ae_sqr(a->lambdareg, _state);
        }
    }
    
    /*
     * Calculate max(ATA)
     *
     * NOTE: here we rely on zero initialization of unused parts of
     *       BlockATA and Tmp2.
     */
    *mxata = 0.0;
    for(i1=0; i1<=ky-1; i1++)
    {
        for(i0=i1; i0<=ae_minint(ky-1, i1+blockbandwidth, _state); i0++)
        {
            celloffset = spline2d_getcelloffset(kx, ky, blockbandwidth, i1, i0, _state);
            for(j1=0; j1<=kx-1; j1++)
            {
                for(j0=0; j0<=kx-1; j0++)
                {
                    *mxata = ae_maxreal(*mxata, ae_fabs(blockata->ptr.pp_double[celloffset+j1][j0], _state), _state);
                }
            }
        }
    }
}


/*************************************************************************
Adjust evaluation interval: if we are inside missing cell, but very  close
to the nonmissing one, move to its boundaries.

This function is used to avoid situation when evaluation at the nodes adjacent
to missing cells fails due to rounding errors that move us away  from  the
feasible cell.

Returns True if X/Y, DX/DY, IX/IY were successfully  repositioned  to  the
nearest nonmissing cell (or were feasible from the very beginning).  False
is returned if we are deep in the missing cell.

  -- ALGLIB --
     Copyright 26.06.2022 by Bochkanov Sergey
*************************************************************************/
static ae_bool spline2d_adjustevaluationinterval(const spline2dinterpolant* s,
     double* x,
     double* t,
     double* dt,
     ae_int_t* ix,
     double* y,
     double* u,
     double* du,
     ae_int_t* iy,
     ae_state *_state)
{
    double tol;
    ae_bool tryleftbndx;
    ae_bool tryrightbndx;
    ae_bool trycenterx;
    ae_bool tryleftbndy;
    ae_bool tryrightbndy;
    ae_bool trycentery;
    ae_bool result;


    
    /*
     * Quick exit - no missing cells, or we are at non-missing cell
     */
    result = !s->hasmissingcells||!s->ismissingcell.ptr.p_bool[(s->n-1)*(*iy)+(*ix)];
    if( result )
    {
        return result;
    }
    
    /*
     * Missing cell, but maybe we are really close to some non-missing cell?
     */
    tol = (double)1000*ae_machineepsilon;
    tryleftbndx = ae_fp_less(*t,tol)&&*ix>0;
    tryrightbndx = ae_fp_greater(*t,(double)1-tol)&&*ix+1<s->n-1;
    trycenterx = ae_true;
    tryleftbndy = ae_fp_less(*u,tol)&&*iy>0;
    tryrightbndy = ae_fp_greater(*u,(double)1-tol)&&*iy+1<s->m-1;
    trycentery = ae_true;
    if( ((!result&&tryleftbndx)&&tryleftbndy)&&!s->ismissingcell.ptr.p_bool[(s->n-1)*(*iy-1)+(*ix-1)] )
    {
        *ix = *ix-1;
        *iy = *iy-1;
        *x = s->x.ptr.p_double[*ix+1];
        *y = s->y.ptr.p_double[*iy+1];
        result = ae_true;
    }
    if( ((!result&&tryleftbndx)&&trycentery)&&!s->ismissingcell.ptr.p_bool[(s->n-1)*(*iy+0)+(*ix-1)] )
    {
        *ix = *ix-1;
        *x = s->x.ptr.p_double[*ix+1];
        result = ae_true;
    }
    if( ((!result&&tryleftbndx)&&tryrightbndy)&&!s->ismissingcell.ptr.p_bool[(s->n-1)*(*iy+1)+(*ix-1)] )
    {
        *ix = *ix-1;
        *iy = *iy+1;
        *x = s->x.ptr.p_double[*ix+1];
        *y = s->y.ptr.p_double[*iy];
        result = ae_true;
    }
    if( ((!result&&trycenterx)&&tryleftbndy)&&!s->ismissingcell.ptr.p_bool[(s->n-1)*(*iy-1)+(*ix+0)] )
    {
        *iy = *iy-1;
        *y = s->y.ptr.p_double[*iy+1];
        result = ae_true;
    }
    if( ((!result&&trycenterx)&&tryrightbndy)&&!s->ismissingcell.ptr.p_bool[(s->n-1)*(*iy+1)+(*ix+0)] )
    {
        *iy = *iy+1;
        *y = s->y.ptr.p_double[*iy];
        result = ae_true;
    }
    if( ((!result&&tryrightbndx)&&tryleftbndy)&&!s->ismissingcell.ptr.p_bool[(s->n-1)*(*iy-1)+(*ix+1)] )
    {
        *ix = *ix+1;
        *iy = *iy-1;
        *x = s->x.ptr.p_double[*ix];
        *y = s->y.ptr.p_double[*iy+1];
        result = ae_true;
    }
    if( ((!result&&tryrightbndx)&&trycentery)&&!s->ismissingcell.ptr.p_bool[(s->n-1)*(*iy+0)+(*ix+1)] )
    {
        *ix = *ix+1;
        *x = s->x.ptr.p_double[*ix];
        result = ae_true;
    }
    if( ((!result&&tryrightbndx)&&tryrightbndy)&&!s->ismissingcell.ptr.p_bool[(s->n-1)*(*iy+1)+(*ix+1)] )
    {
        *ix = *ix+1;
        *iy = *iy+1;
        *x = s->x.ptr.p_double[*ix];
        *y = s->y.ptr.p_double[*iy];
        result = ae_true;
    }
    if( result )
    {
        *dt = 1.0/(s->x.ptr.p_double[*ix+1]-s->x.ptr.p_double[*ix]);
        *t = (*x-s->x.ptr.p_double[*ix])*(*dt);
        *du = 1.0/(s->y.ptr.p_double[*iy+1]-s->y.ptr.p_double[*iy]);
        *u = (*y-s->y.ptr.p_double[*iy])*(*du);
    }
    return result;
}


void _spline2dinterpolant_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    spline2dinterpolant *p = (spline2dinterpolant*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->y, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->f, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->ismissingnode, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->ismissingcell, 0, DT_BOOL, _state, make_automatic);
}


void _spline2dinterpolant_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    spline2dinterpolant       *dst = (spline2dinterpolant*)_dst;
    const spline2dinterpolant *src = (const spline2dinterpolant*)_src;
    dst->stype = src->stype;
    dst->hasmissingcells = src->hasmissingcells;
    dst->n = src->n;
    dst->m = src->m;
    dst->d = src->d;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->y, &src->y, _state, make_automatic);
    ae_vector_init_copy(&dst->f, &src->f, _state, make_automatic);
    ae_vector_init_copy(&dst->ismissingnode, &src->ismissingnode, _state, make_automatic);
    ae_vector_init_copy(&dst->ismissingcell, &src->ismissingcell, _state, make_automatic);
}


void _spline2dinterpolant_clear(void* _p)
{
    spline2dinterpolant *p = (spline2dinterpolant*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->y);
    ae_vector_clear(&p->f);
    ae_vector_clear(&p->ismissingnode);
    ae_vector_clear(&p->ismissingcell);
}


void _spline2dinterpolant_destroy(void* _p)
{
    spline2dinterpolant *p = (spline2dinterpolant*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->y);
    ae_vector_destroy(&p->f);
    ae_vector_destroy(&p->ismissingnode);
    ae_vector_destroy(&p->ismissingcell);
}


void _spline2dbuilder_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    spline2dbuilder *p = (spline2dbuilder*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->xy, 0, DT_REAL, _state, make_automatic);
}


void _spline2dbuilder_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    spline2dbuilder       *dst = (spline2dbuilder*)_dst;
    const spline2dbuilder *src = (const spline2dbuilder*)_src;
    dst->priorterm = src->priorterm;
    dst->priortermval = src->priortermval;
    dst->areatype = src->areatype;
    dst->xa = src->xa;
    dst->xb = src->xb;
    dst->ya = src->ya;
    dst->yb = src->yb;
    dst->gridtype = src->gridtype;
    dst->kx = src->kx;
    dst->ky = src->ky;
    dst->smoothing = src->smoothing;
    dst->nlayers = src->nlayers;
    dst->solvertype = src->solvertype;
    dst->lambdabase = src->lambdabase;
    ae_vector_init_copy(&dst->xy, &src->xy, _state, make_automatic);
    dst->npoints = src->npoints;
    dst->d = src->d;
    dst->sx = src->sx;
    dst->sy = src->sy;
    dst->adddegreeoffreedom = src->adddegreeoffreedom;
    dst->interfacesize = src->interfacesize;
    dst->lsqrcnt = src->lsqrcnt;
    dst->maxcoresize = src->maxcoresize;
}


void _spline2dbuilder_clear(void* _p)
{
    spline2dbuilder *p = (spline2dbuilder*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->xy);
}


void _spline2dbuilder_destroy(void* _p)
{
    spline2dbuilder *p = (spline2dbuilder*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->xy);
}


void _spline2dfitreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    spline2dfitreport *p = (spline2dfitreport*)_p;
    ae_touch_ptr((void*)p);
}


void _spline2dfitreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    spline2dfitreport       *dst = (spline2dfitreport*)_dst;
    const spline2dfitreport *src = (const spline2dfitreport*)_src;
    dst->rmserror = src->rmserror;
    dst->avgerror = src->avgerror;
    dst->maxerror = src->maxerror;
    dst->r2 = src->r2;
}


void _spline2dfitreport_clear(void* _p)
{
    spline2dfitreport *p = (spline2dfitreport*)_p;
    ae_touch_ptr((void*)p);
}


void _spline2dfitreport_destroy(void* _p)
{
    spline2dfitreport *p = (spline2dfitreport*)_p;
    ae_touch_ptr((void*)p);
}


void _spline2dxdesignmatrix_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    spline2dxdesignmatrix *p = (spline2dxdesignmatrix*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_init(&p->vals, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->batches, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->batchbases, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp1, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->tmp2, 0, 0, DT_REAL, _state, make_automatic);
}


void _spline2dxdesignmatrix_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    spline2dxdesignmatrix       *dst = (spline2dxdesignmatrix*)_dst;
    const spline2dxdesignmatrix *src = (const spline2dxdesignmatrix*)_src;
    dst->blockwidth = src->blockwidth;
    dst->kx = src->kx;
    dst->ky = src->ky;
    dst->npoints = src->npoints;
    dst->nrows = src->nrows;
    dst->ndenserows = src->ndenserows;
    dst->ndensebatches = src->ndensebatches;
    dst->d = src->d;
    dst->maxbatch = src->maxbatch;
    ae_matrix_init_copy(&dst->vals, &src->vals, _state, make_automatic);
    ae_vector_init_copy(&dst->batches, &src->batches, _state, make_automatic);
    ae_vector_init_copy(&dst->batchbases, &src->batchbases, _state, make_automatic);
    dst->lambdareg = src->lambdareg;
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp1, &src->tmp1, _state, make_automatic);
    ae_matrix_init_copy(&dst->tmp2, &src->tmp2, _state, make_automatic);
}


void _spline2dxdesignmatrix_clear(void* _p)
{
    spline2dxdesignmatrix *p = (spline2dxdesignmatrix*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_clear(&p->vals);
    ae_vector_clear(&p->batches);
    ae_vector_clear(&p->batchbases);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->tmp1);
    ae_matrix_clear(&p->tmp2);
}


void _spline2dxdesignmatrix_destroy(void* _p)
{
    spline2dxdesignmatrix *p = (spline2dxdesignmatrix*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_destroy(&p->vals);
    ae_vector_destroy(&p->batches);
    ae_vector_destroy(&p->batchbases);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->tmp1);
    ae_matrix_destroy(&p->tmp2);
}


void _spline2dblockllsbuf_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    spline2dblockllsbuf *p = (spline2dblockllsbuf*)_p;
    ae_touch_ptr((void*)p);
    _linlsqrstate_init(&p->solver, _state, make_automatic);
    _linlsqrreport_init(&p->solverrep, _state, make_automatic);
    ae_matrix_init(&p->blockata, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->trsmbuf2, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->cholbuf2, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->cholbuf1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp1, 0, DT_REAL, _state, make_automatic);
}


void _spline2dblockllsbuf_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    spline2dblockllsbuf       *dst = (spline2dblockllsbuf*)_dst;
    const spline2dblockllsbuf *src = (const spline2dblockllsbuf*)_src;
    _linlsqrstate_init_copy(&dst->solver, &src->solver, _state, make_automatic);
    _linlsqrreport_init_copy(&dst->solverrep, &src->solverrep, _state, make_automatic);
    ae_matrix_init_copy(&dst->blockata, &src->blockata, _state, make_automatic);
    ae_matrix_init_copy(&dst->trsmbuf2, &src->trsmbuf2, _state, make_automatic);
    ae_matrix_init_copy(&dst->cholbuf2, &src->cholbuf2, _state, make_automatic);
    ae_vector_init_copy(&dst->cholbuf1, &src->cholbuf1, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp1, &src->tmp1, _state, make_automatic);
}


void _spline2dblockllsbuf_clear(void* _p)
{
    spline2dblockllsbuf *p = (spline2dblockllsbuf*)_p;
    ae_touch_ptr((void*)p);
    _linlsqrstate_clear(&p->solver);
    _linlsqrreport_clear(&p->solverrep);
    ae_matrix_clear(&p->blockata);
    ae_matrix_clear(&p->trsmbuf2);
    ae_matrix_clear(&p->cholbuf2);
    ae_vector_clear(&p->cholbuf1);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->tmp1);
}


void _spline2dblockllsbuf_destroy(void* _p)
{
    spline2dblockllsbuf *p = (spline2dblockllsbuf*)_p;
    ae_touch_ptr((void*)p);
    _linlsqrstate_destroy(&p->solver);
    _linlsqrreport_destroy(&p->solverrep);
    ae_matrix_destroy(&p->blockata);
    ae_matrix_destroy(&p->trsmbuf2);
    ae_matrix_destroy(&p->cholbuf2);
    ae_vector_destroy(&p->cholbuf1);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->tmp1);
}


void _spline2dfastddmbuf_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    spline2dfastddmbuf *p = (spline2dfastddmbuf*)_p;
    ae_touch_ptr((void*)p);
    _spline2dxdesignmatrix_init(&p->xdesignmatrix, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpz, 0, DT_REAL, _state, make_automatic);
    _spline2dfitreport_init(&p->dummyrep, _state, make_automatic);
    _spline2dinterpolant_init(&p->localmodel, _state, make_automatic);
    _spline2dblockllsbuf_init(&p->blockllsbuf, _state, make_automatic);
}


void _spline2dfastddmbuf_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    spline2dfastddmbuf       *dst = (spline2dfastddmbuf*)_dst;
    const spline2dfastddmbuf *src = (const spline2dfastddmbuf*)_src;
    _spline2dxdesignmatrix_init_copy(&dst->xdesignmatrix, &src->xdesignmatrix, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpz, &src->tmpz, _state, make_automatic);
    _spline2dfitreport_init_copy(&dst->dummyrep, &src->dummyrep, _state, make_automatic);
    _spline2dinterpolant_init_copy(&dst->localmodel, &src->localmodel, _state, make_automatic);
    _spline2dblockllsbuf_init_copy(&dst->blockllsbuf, &src->blockllsbuf, _state, make_automatic);
}


void _spline2dfastddmbuf_clear(void* _p)
{
    spline2dfastddmbuf *p = (spline2dfastddmbuf*)_p;
    ae_touch_ptr((void*)p);
    _spline2dxdesignmatrix_clear(&p->xdesignmatrix);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->tmpz);
    _spline2dfitreport_clear(&p->dummyrep);
    _spline2dinterpolant_clear(&p->localmodel);
    _spline2dblockllsbuf_clear(&p->blockllsbuf);
}


void _spline2dfastddmbuf_destroy(void* _p)
{
    spline2dfastddmbuf *p = (spline2dfastddmbuf*)_p;
    ae_touch_ptr((void*)p);
    _spline2dxdesignmatrix_destroy(&p->xdesignmatrix);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->tmpz);
    _spline2dfitreport_destroy(&p->dummyrep);
    _spline2dinterpolant_destroy(&p->localmodel);
    _spline2dblockllsbuf_destroy(&p->blockllsbuf);
}


/*$ End $*/
