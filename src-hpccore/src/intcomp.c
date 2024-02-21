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
#include "intcomp.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
This function is left for backward compatibility.
Use fitspheremc() instead.

                                    
  -- ALGLIB --
     Copyright 14.04.2017 by Bochkanov Sergey
*************************************************************************/
void nsfitspheremcc(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nx,
     /* Real    */ ae_vector* cx,
     double* rhi,
     ae_state *_state)
{
    double dummy;

    ae_vector_clear(cx);
    *rhi = 0.0;

    nsfitspherex(xy, npoints, nx, 1, 0.0, 0, 0.0, cx, &dummy, rhi, _state);
}


/*************************************************************************
This function is left for backward compatibility.
Use fitspheremi() instead.
                                    
  -- ALGLIB --
     Copyright 14.04.2017 by Bochkanov Sergey
*************************************************************************/
void nsfitspheremic(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nx,
     /* Real    */ ae_vector* cx,
     double* rlo,
     ae_state *_state)
{
    double dummy;

    ae_vector_clear(cx);
    *rlo = 0.0;

    nsfitspherex(xy, npoints, nx, 2, 0.0, 0, 0.0, cx, rlo, &dummy, _state);
}


/*************************************************************************
This function is left for backward compatibility.
Use fitspheremz() instead.
                                    
  -- ALGLIB --
     Copyright 14.04.2017 by Bochkanov Sergey
*************************************************************************/
void nsfitspheremzc(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nx,
     /* Real    */ ae_vector* cx,
     double* rlo,
     double* rhi,
     ae_state *_state)
{

    ae_vector_clear(cx);
    *rlo = 0.0;
    *rhi = 0.0;

    nsfitspherex(xy, npoints, nx, 3, 0.0, 0, 0.0, cx, rlo, rhi, _state);
}


/*************************************************************************
This function is left for backward compatibility.
Use fitspherex() instead.
                                    
  -- ALGLIB --
     Copyright 14.04.2017 by Bochkanov Sergey
*************************************************************************/
void nsfitspherex(/* Real    */ const ae_matrix* xy,
     ae_int_t npoints,
     ae_int_t nx,
     ae_int_t problemtype,
     double epsx,
     ae_int_t aulits,
     double penalty,
     /* Real    */ ae_vector* cx,
     double* rlo,
     double* rhi,
     ae_state *_state)
{

    ae_vector_clear(cx);
    *rlo = 0.0;
    *rhi = 0.0;

    fitspherex(xy, npoints, nx, problemtype, epsx, aulits, cx, rlo, rhi, _state);
}


/*************************************************************************
This function is an obsolete and deprecated version of fitting by
penalized cubic spline.

It was superseded by spline1dfit(), which is an orders of magnitude faster
and more memory-efficient implementation.

Do NOT use this function in the new code!

  -- ALGLIB PROJECT --
     Copyright 18.08.2009 by Bochkanov Sergey
*************************************************************************/
void spline1dfitpenalized(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     ae_int_t n,
     ae_int_t m,
     double rho,
     ae_int_t* info,
     spline1dinterpolant* s,
     spline1dfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector x;
    ae_vector y;
    ae_vector w;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&y, 0, sizeof(y));
    memset(&w, 0, sizeof(w));
    ae_vector_init_copy(&x, _x, _state, ae_true);
    ae_vector_init_copy(&y, _y, _state, ae_true);
    *info = 0;
    _spline1dinterpolant_clear(s);
    _spline1dfitreport_clear(rep);
    ae_vector_init(&w, 0, DT_REAL, _state, ae_true);

    ae_assert(n>=1, "Spline1DFitPenalized: N<1!", _state);
    ae_assert(m>=4, "Spline1DFitPenalized: M<4!", _state);
    ae_assert(x.cnt>=n, "Spline1DFitPenalized: Length(X)<N!", _state);
    ae_assert(y.cnt>=n, "Spline1DFitPenalized: Length(Y)<N!", _state);
    ae_assert(isfinitevector(&x, n, _state), "Spline1DFitPenalized: X contains infinite or NAN values!", _state);
    ae_assert(isfinitevector(&y, n, _state), "Spline1DFitPenalized: Y contains infinite or NAN values!", _state);
    ae_assert(ae_isfinite(rho, _state), "Spline1DFitPenalized: Rho is infinite!", _state);
    ae_vector_set_length(&w, n, _state);
    for(i=0; i<=n-1; i++)
    {
        w.ptr.p_double[i] = (double)(1);
    }
    spline1dfitpenalizedw(&x, &y, &w, n, m, rho, info, s, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function is an obsolete and deprecated version of fitting by
penalized cubic spline.

It was superseded by spline1dfit(), which is an orders of magnitude faster
and more memory-efficient implementation.

Do NOT use this function in the new code!

  -- ALGLIB PROJECT --
     Copyright 19.10.2010 by Bochkanov Sergey
*************************************************************************/
void spline1dfitpenalizedw(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     /* Real    */ const ae_vector* _w,
     ae_int_t n,
     ae_int_t m,
     double rho,
     ae_int_t* info,
     spline1dinterpolant* s,
     spline1dfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector x;
    ae_vector y;
    ae_vector w;
    ae_int_t i;
    ae_int_t j;
    ae_int_t b;
    double v;
    double relcnt;
    double xa;
    double xb;
    double sa;
    double sb;
    ae_vector xoriginal;
    ae_vector yoriginal;
    double pdecay;
    double tdecay;
    ae_matrix fmatrix;
    ae_vector fcolumn;
    ae_vector y2;
    ae_vector w2;
    ae_vector xc;
    ae_vector yc;
    ae_vector dc;
    double fdmax;
    double admax;
    ae_matrix amatrix;
    ae_matrix d2matrix;
    double fa;
    double ga;
    double fb;
    double gb;
    double lambdav;
    ae_vector bx;
    ae_vector by;
    ae_vector bd1;
    ae_vector bd2;
    ae_vector tx;
    ae_vector ty;
    ae_vector td;
    spline1dinterpolant bs;
    ae_matrix nmatrix;
    ae_vector rightpart;
    fblslincgstate cgstate;
    ae_vector c;
    ae_vector tmp0;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&y, 0, sizeof(y));
    memset(&w, 0, sizeof(w));
    memset(&xoriginal, 0, sizeof(xoriginal));
    memset(&yoriginal, 0, sizeof(yoriginal));
    memset(&fmatrix, 0, sizeof(fmatrix));
    memset(&fcolumn, 0, sizeof(fcolumn));
    memset(&y2, 0, sizeof(y2));
    memset(&w2, 0, sizeof(w2));
    memset(&xc, 0, sizeof(xc));
    memset(&yc, 0, sizeof(yc));
    memset(&dc, 0, sizeof(dc));
    memset(&amatrix, 0, sizeof(amatrix));
    memset(&d2matrix, 0, sizeof(d2matrix));
    memset(&bx, 0, sizeof(bx));
    memset(&by, 0, sizeof(by));
    memset(&bd1, 0, sizeof(bd1));
    memset(&bd2, 0, sizeof(bd2));
    memset(&tx, 0, sizeof(tx));
    memset(&ty, 0, sizeof(ty));
    memset(&td, 0, sizeof(td));
    memset(&bs, 0, sizeof(bs));
    memset(&nmatrix, 0, sizeof(nmatrix));
    memset(&rightpart, 0, sizeof(rightpart));
    memset(&cgstate, 0, sizeof(cgstate));
    memset(&c, 0, sizeof(c));
    memset(&tmp0, 0, sizeof(tmp0));
    ae_vector_init_copy(&x, _x, _state, ae_true);
    ae_vector_init_copy(&y, _y, _state, ae_true);
    ae_vector_init_copy(&w, _w, _state, ae_true);
    *info = 0;
    _spline1dinterpolant_clear(s);
    _spline1dfitreport_clear(rep);
    ae_vector_init(&xoriginal, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&yoriginal, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&fmatrix, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&fcolumn, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&y2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&w2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xc, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&yc, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&dc, 0, DT_INT, _state, ae_true);
    ae_matrix_init(&amatrix, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&d2matrix, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bx, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&by, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bd1, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bd2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tx, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ty, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&td, 0, DT_REAL, _state, ae_true);
    _spline1dinterpolant_init(&bs, _state, ae_true);
    ae_matrix_init(&nmatrix, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&rightpart, 0, DT_REAL, _state, ae_true);
    _fblslincgstate_init(&cgstate, _state, ae_true);
    ae_vector_init(&c, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>=1, "Spline1DFitPenalizedW: N<1!", _state);
    ae_assert(m>=4, "Spline1DFitPenalizedW: M<4!", _state);
    ae_assert(x.cnt>=n, "Spline1DFitPenalizedW: Length(X)<N!", _state);
    ae_assert(y.cnt>=n, "Spline1DFitPenalizedW: Length(Y)<N!", _state);
    ae_assert(w.cnt>=n, "Spline1DFitPenalizedW: Length(W)<N!", _state);
    ae_assert(isfinitevector(&x, n, _state), "Spline1DFitPenalizedW: X contains infinite or NAN values!", _state);
    ae_assert(isfinitevector(&y, n, _state), "Spline1DFitPenalizedW: Y contains infinite or NAN values!", _state);
    ae_assert(isfinitevector(&w, n, _state), "Spline1DFitPenalizedW: Y contains infinite or NAN values!", _state);
    ae_assert(ae_isfinite(rho, _state), "Spline1DFitPenalizedW: Rho is infinite!", _state);
    
    /*
     * Prepare LambdaV
     */
    v = -ae_log(ae_machineepsilon, _state)/ae_log((double)(10), _state);
    if( ae_fp_less(rho,-v) )
    {
        rho = -v;
    }
    if( ae_fp_greater(rho,v) )
    {
        rho = v;
    }
    lambdav = ae_pow((double)(10), rho, _state);
    
    /*
     * Sort X, Y, W
     */
    heapsortdpoints(&x, &y, &w, n, _state);
    
    /*
     * Scale X, Y, XC, YC
     */
    lsfitscalexy(&x, &y, &w, n, &xc, &yc, &dc, 0, &xa, &xb, &sa, &sb, &xoriginal, &yoriginal, _state);
    
    /*
     * Allocate space
     */
    ae_matrix_set_length(&fmatrix, n, m, _state);
    ae_matrix_set_length(&amatrix, m, m, _state);
    ae_matrix_set_length(&d2matrix, m, m, _state);
    ae_vector_set_length(&bx, m, _state);
    ae_vector_set_length(&by, m, _state);
    ae_vector_set_length(&fcolumn, n, _state);
    ae_matrix_set_length(&nmatrix, m, m, _state);
    ae_vector_set_length(&rightpart, m, _state);
    ae_vector_set_length(&tmp0, ae_maxint(m, n, _state), _state);
    ae_vector_set_length(&c, m, _state);
    
    /*
     * Fill:
     * * FMatrix by values of basis functions
     * * TmpAMatrix by second derivatives of I-th function at J-th point
     * * CMatrix by constraints
     */
    fdmax = (double)(0);
    for(b=0; b<=m-1; b++)
    {
        
        /*
         * Prepare I-th basis function
         */
        for(j=0; j<=m-1; j++)
        {
            bx.ptr.p_double[j] = (double)(2*j)/(double)(m-1)-(double)1;
            by.ptr.p_double[j] = (double)(0);
        }
        by.ptr.p_double[b] = (double)(1);
        spline1dgriddiff2cubic(&bx, &by, m, 2, 0.0, 2, 0.0, &bd1, &bd2, _state);
        spline1dbuildcubic(&bx, &by, m, 2, 0.0, 2, 0.0, &bs, _state);
        
        /*
         * Calculate B-th column of FMatrix
         * Update FDMax (maximum column norm)
         */
        spline1dconvcubic(&bx, &by, m, 2, 0.0, 2, 0.0, &x, n, &fcolumn, _state);
        ae_v_move(&fmatrix.ptr.pp_double[0][b], fmatrix.stride, &fcolumn.ptr.p_double[0], 1, ae_v_len(0,n-1));
        v = (double)(0);
        for(i=0; i<=n-1; i++)
        {
            v = v+ae_sqr(w.ptr.p_double[i]*fcolumn.ptr.p_double[i], _state);
        }
        fdmax = ae_maxreal(fdmax, v, _state);
        
        /*
         * Fill temporary with second derivatives of basis function
         */
        ae_v_move(&d2matrix.ptr.pp_double[b][0], 1, &bd2.ptr.p_double[0], 1, ae_v_len(0,m-1));
    }
    
    /*
     * * calculate penalty matrix A
     * * calculate max of diagonal elements of A
     * * calculate PDecay - coefficient before penalty matrix
     */
    for(i=0; i<=m-1; i++)
    {
        for(j=i; j<=m-1; j++)
        {
            
            /*
             * calculate integral(B_i''*B_j'') where B_i and B_j are
             * i-th and j-th basis splines.
             * B_i and B_j are piecewise linear functions.
             */
            v = (double)(0);
            for(b=0; b<=m-2; b++)
            {
                fa = d2matrix.ptr.pp_double[i][b];
                fb = d2matrix.ptr.pp_double[i][b+1];
                ga = d2matrix.ptr.pp_double[j][b];
                gb = d2matrix.ptr.pp_double[j][b+1];
                v = v+(bx.ptr.p_double[b+1]-bx.ptr.p_double[b])*(fa*ga+(fa*(gb-ga)+ga*(fb-fa))/(double)2+(fb-fa)*(gb-ga)/(double)3);
            }
            amatrix.ptr.pp_double[i][j] = v;
            amatrix.ptr.pp_double[j][i] = v;
        }
    }
    admax = (double)(0);
    for(i=0; i<=m-1; i++)
    {
        admax = ae_maxreal(admax, ae_fabs(amatrix.ptr.pp_double[i][i], _state), _state);
    }
    pdecay = lambdav*fdmax/admax;
    
    /*
     * Calculate TDecay for Tikhonov regularization
     */
    tdecay = fdmax*((double)1+pdecay)*(double)10*ae_machineepsilon;
    
    /*
     * Prepare system
     *
     * NOTE: FMatrix is spoiled during this process
     */
    for(i=0; i<=n-1; i++)
    {
        v = w.ptr.p_double[i];
        ae_v_muld(&fmatrix.ptr.pp_double[i][0], 1, ae_v_len(0,m-1), v);
    }
    rmatrixgemm(m, m, n, 1.0, &fmatrix, 0, 0, 1, &fmatrix, 0, 0, 0, 0.0, &nmatrix, 0, 0, _state);
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=m-1; j++)
        {
            nmatrix.ptr.pp_double[i][j] = nmatrix.ptr.pp_double[i][j]+pdecay*amatrix.ptr.pp_double[i][j];
        }
    }
    for(i=0; i<=m-1; i++)
    {
        nmatrix.ptr.pp_double[i][i] = nmatrix.ptr.pp_double[i][i]+tdecay;
    }
    for(i=0; i<=m-1; i++)
    {
        rightpart.ptr.p_double[i] = (double)(0);
    }
    for(i=0; i<=n-1; i++)
    {
        v = y.ptr.p_double[i]*w.ptr.p_double[i];
        ae_v_addd(&rightpart.ptr.p_double[0], 1, &fmatrix.ptr.pp_double[i][0], 1, ae_v_len(0,m-1), v);
    }
    
    /*
     * Solve system
     */
    if( !spdmatrixcholesky(&nmatrix, m, ae_true, _state) )
    {
        *info = -4;
        ae_frame_leave(_state);
        return;
    }
    fblscholeskysolve(&nmatrix, 1.0, m, ae_true, &rightpart, &tmp0, _state);
    ae_v_move(&c.ptr.p_double[0], 1, &rightpart.ptr.p_double[0], 1, ae_v_len(0,m-1));
    
    /*
     * add nodes to force linearity outside of the fitting interval
     */
    spline1dgriddiffcubic(&bx, &c, m, 2, 0.0, 2, 0.0, &bd1, _state);
    ae_vector_set_length(&tx, m+2, _state);
    ae_vector_set_length(&ty, m+2, _state);
    ae_vector_set_length(&td, m+2, _state);
    ae_v_move(&tx.ptr.p_double[1], 1, &bx.ptr.p_double[0], 1, ae_v_len(1,m));
    ae_v_move(&ty.ptr.p_double[1], 1, &rightpart.ptr.p_double[0], 1, ae_v_len(1,m));
    ae_v_move(&td.ptr.p_double[1], 1, &bd1.ptr.p_double[0], 1, ae_v_len(1,m));
    tx.ptr.p_double[0] = tx.ptr.p_double[1]-(tx.ptr.p_double[2]-tx.ptr.p_double[1]);
    ty.ptr.p_double[0] = ty.ptr.p_double[1]-td.ptr.p_double[1]*(tx.ptr.p_double[2]-tx.ptr.p_double[1]);
    td.ptr.p_double[0] = td.ptr.p_double[1];
    tx.ptr.p_double[m+1] = tx.ptr.p_double[m]+(tx.ptr.p_double[m]-tx.ptr.p_double[m-1]);
    ty.ptr.p_double[m+1] = ty.ptr.p_double[m]+td.ptr.p_double[m]*(tx.ptr.p_double[m]-tx.ptr.p_double[m-1]);
    td.ptr.p_double[m+1] = td.ptr.p_double[m];
    spline1dbuildhermite(&tx, &ty, &td, m+2, s, _state);
    spline1dlintransx(s, (double)2/(xb-xa), -(xa+xb)/(xb-xa), _state);
    spline1dlintransy(s, sb-sa, sa, _state);
    *info = 1;
    
    /*
     * Fill report
     */
    rep->rmserror = (double)(0);
    rep->avgerror = (double)(0);
    rep->avgrelerror = (double)(0);
    rep->maxerror = (double)(0);
    relcnt = (double)(0);
    spline1dconvcubic(&bx, &rightpart, m, 2, 0.0, 2, 0.0, &x, n, &fcolumn, _state);
    for(i=0; i<=n-1; i++)
    {
        v = (sb-sa)*fcolumn.ptr.p_double[i]+sa;
        rep->rmserror = rep->rmserror+ae_sqr(v-yoriginal.ptr.p_double[i], _state);
        rep->avgerror = rep->avgerror+ae_fabs(v-yoriginal.ptr.p_double[i], _state);
        if( ae_fp_neq(yoriginal.ptr.p_double[i],(double)(0)) )
        {
            rep->avgrelerror = rep->avgrelerror+ae_fabs(v-yoriginal.ptr.p_double[i], _state)/ae_fabs(yoriginal.ptr.p_double[i], _state);
            relcnt = relcnt+(double)1;
        }
        rep->maxerror = ae_maxreal(rep->maxerror, ae_fabs(v-yoriginal.ptr.p_double[i], _state), _state);
    }
    rep->rmserror = ae_sqrt(rep->rmserror/(double)n, _state);
    rep->avgerror = rep->avgerror/(double)n;
    if( ae_fp_neq(relcnt,(double)(0)) )
    {
        rep->avgrelerror = rep->avgrelerror/relcnt;
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Deprecated fitting function with O(N*M^2+M^3) running time. Superseded  by
spline1dfit().

  -- ALGLIB PROJECT --
     Copyright 18.08.2009 by Bochkanov Sergey
*************************************************************************/
void spline1dfitcubic(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     ae_int_t m,
     spline1dinterpolant* s,
     spline1dfitreport* rep,
     ae_state *_state)
{

    _spline1dinterpolant_clear(s);
    _spline1dfitreport_clear(rep);

    spline1dfitcubicdeprecated(x, y, n, m, s, rep, _state);
}


/*************************************************************************
Deprecated fitting function with O(N*M^2+M^3) running time. Superseded  by
spline1dfit().

  -- ALGLIB PROJECT --
     Copyright 18.08.2009 by Bochkanov Sergey
*************************************************************************/
void spline1dfithermite(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     ae_int_t m,
     spline1dinterpolant* s,
     spline1dfitreport* rep,
     ae_state *_state)
{

    _spline1dinterpolant_clear(s);
    _spline1dfitreport_clear(rep);

    spline1dfithermitedeprecated(x, y, n, m, s, rep, _state);
}


/*$ End $*/
