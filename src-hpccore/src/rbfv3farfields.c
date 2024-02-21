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
#include "rbfv3farfields.h"


/*$ Declarations $*/
#ifdef ALGLIB_NO_FAST_KERNELS
static ae_bool rbfv3farfields_bhpaneleval1fastkernel(double d0,
     double d1,
     double d2,
     ae_int_t panelp,
     /* Real    */ const ae_vector* pnma,
     /* Real    */ const ae_vector* pnmb,
     /* Real    */ const ae_vector* pmmcdiag,
     /* Real    */ const ae_vector* ynma,
     /* Real    */ const ae_vector* tblrmodmn,
     double* f,
     double* invpowrpplus1,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
static ae_bool rbfv3farfields_bhpanelevalfastkernel(double d0,
     double d1,
     double d2,
     ae_int_t ny,
     ae_int_t panelp,
     /* Real    */ const ae_vector* pnma,
     /* Real    */ const ae_vector* pnmb,
     /* Real    */ const ae_vector* pmmcdiag,
     /* Real    */ const ae_vector* ynma,
     /* Real    */ const ae_vector* tblrmodmn,
     /* Real    */ ae_vector* f,
     double* invpowrpplus1,
     ae_state *_state);
#endif


/*$ Body $*/


/*************************************************************************
Initialize precomputed table for a biharmonic evaluator

  -- ALGLIB --
     Copyright 26.08.2022 by Sergey Bochkanov
*************************************************************************/
void biharmonicevaluatorinit(biharmonicevaluator* eval,
     ae_int_t maxp,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;
    ae_int_t m;
    ae_complex cplxi;
    ae_complex cplxminusi;


    ae_assert(maxp>=2, "BiharmonicEvaluatorInit: MaxP<2", _state);
    eval->maxp = maxp;
    
    /*
     * Precompute some often used values
     *
     * NOTE: we use SetLength() instead of rAllocV() in order to enforce strict length
     *       of the precomputed tables which results in better bounds checking during
     *       the running time.
     */
    eval->precomputedcount = 2*maxp+3;
    ae_vector_set_length(&eval->tpowminus1, eval->precomputedcount, _state);
    ae_vector_set_length(&eval->tpowminusi, eval->precomputedcount, _state);
    ae_vector_set_length(&eval->tpowi, eval->precomputedcount, _state);
    cplxi.x = (double)(0);
    cplxi.y = (double)(1);
    cplxminusi.x = (double)(0);
    cplxminusi.y = (double)(-1);
    eval->tpowminus1.ptr.p_double[0] = (double)(1);
    eval->tpowminusi.ptr.p_complex[0] = ae_complex_from_i(1);
    eval->tpowi.ptr.p_complex[0] = ae_complex_from_i(1);
    for(i=1; i<=eval->precomputedcount-1; i++)
    {
        eval->tpowminus1.ptr.p_double[i] = eval->tpowminus1.ptr.p_double[i-1]*(double)(-1);
        eval->tpowminusi.ptr.p_complex[i] = ae_c_mul(eval->tpowminusi.ptr.p_complex[i-1],cplxminusi);
        eval->tpowi.ptr.p_complex[i] = ae_c_mul(eval->tpowi.ptr.p_complex[i-1],cplxi);
    }
    ae_vector_set_length(&eval->tfactorial, eval->precomputedcount, _state);
    ae_vector_set_length(&eval->tsqrtfactorial, eval->precomputedcount, _state);
    eval->tfactorial.ptr.p_double[0] = (double)(1);
    for(i=1; i<=eval->precomputedcount-1; i++)
    {
        eval->tfactorial.ptr.p_double[i] = (double)i*eval->tfactorial.ptr.p_double[i-1];
    }
    for(i=0; i<=eval->precomputedcount-1; i++)
    {
        eval->tsqrtfactorial.ptr.p_double[i] = ae_sqrt(eval->tfactorial.ptr.p_double[i], _state);
    }
    ae_vector_set_length(&eval->tdoublefactorial, eval->precomputedcount, _state);
    ae_assert(eval->precomputedcount>=2, "BiharmonicEvaluatorInit: integrity check 8446 failed", _state);
    eval->tdoublefactorial.ptr.p_double[0] = (double)(1);
    eval->tdoublefactorial.ptr.p_double[1] = (double)(1);
    for(i=2; i<=eval->precomputedcount-1; i++)
    {
        eval->tdoublefactorial.ptr.p_double[i] = (double)i*eval->tdoublefactorial.ptr.p_double[i-2];
    }
    
    /*
     * Precompute coefficients for the associated Legendre recurrence relation
     *
     *   P[n+1,m] = P[n,m]*CosTheta*(2*n-1)/(N-M) - P[n-1,m]*(N+M-1)/(N-M)  (for n>m)
     *            = P[n,m]*CosTheta*PnmA[n+1,m] + P[n-1,m]*PnmB[n+1,m]      (for n>m)
     */
    rsetallocv((maxp+1)*(maxp+1), 0.0, &eval->pnma, _state);
    rsetallocv((maxp+1)*(maxp+1), 0.0, &eval->pnmb, _state);
    for(n=0; n<=maxp; n++)
    {
        for(m=0; m<=n-1; m++)
        {
            eval->pnma.ptr.p_double[n*(maxp+1)+m] = (double)(2*n-1)/(double)(n-m);
            eval->pnmb.ptr.p_double[n*(maxp+1)+m] = -(double)(n+m-1)/(double)(n-m);
        }
    }
    
    /*
     * Precompute coefficient used during computation of initial values of the
     * associated Legendre recurrence
     */
    rsetallocv(maxp+1, 0.0, &eval->pmmc, _state);
    rsetallocv((maxp+1)*(maxp+1), 0.0, &eval->pmmcdiag, _state);
    for(m=0; m<=maxp; m++)
    {
        eval->pmmc.ptr.p_double[m] = eval->tpowminus1.ptr.p_double[m]*eval->tdoublefactorial.ptr.p_double[ae_maxint(2*m-1, 0, _state)];
        eval->pmmcdiag.ptr.p_double[m*(maxp+1)+m] = eval->pmmc.ptr.p_double[m];
    }
    
    /*
     * Precompute coefficient YnmA used during computation of the spherical harmonic Ynm
     */
    rsetallocv((maxp+1)*(maxp+1), 0.0, &eval->ynma, _state);
    for(n=0; n<=maxp; n++)
    {
        for(m=0; m<=n; m++)
        {
            eval->ynma.ptr.p_double[n*(maxp+1)+m] = eval->tpowminus1.ptr.p_double[m]*eval->tsqrtfactorial.ptr.p_double[n-m]/eval->tsqrtfactorial.ptr.p_double[n+m];
        }
    }
    
    /*
     * Precompute coefficient InmA used during computation of the inner function Inm
     */
    csetallocv((maxp+1)*(maxp+1), ae_complex_from_d(0.0), &eval->inma, _state);
    for(n=0; n<=maxp; n++)
    {
        for(m=0; m<=n; m++)
        {
            eval->inma.ptr.p_complex[n*(maxp+1)+m] = ae_c_mul_d(eval->tpowminusi.ptr.p_complex[m],eval->tpowminus1.ptr.p_double[n]/(eval->tsqrtfactorial.ptr.p_double[n+m]*eval->tsqrtfactorial.ptr.p_double[n-m]));
        }
    }
    
    /*
     * Precompute coefficients MnmA and NnmA used during computation of expansion functions Mnm and Nnm
     */
    rsetallocv(maxp+1, 0.0, &eval->mnma, _state);
    rsetallocv(maxp+1, 0.0, &eval->nnma, _state);
    for(n=0; n<=maxp; n++)
    {
        eval->nnma.ptr.p_double[n] = -eval->tpowminus1.ptr.p_double[n]/(double)(2*n-1);
        if( n<=maxp-2 )
        {
            eval->mnma.ptr.p_double[n] = eval->tpowminus1.ptr.p_double[n]/(double)(2*n+3);
        }
    }
}


/*************************************************************************
Build a panel with biharmonic far field expansions. The  function  assumes
that we work with 3D data. Lower dimensional data can be zero-padded. Data
with higher dimensionality is NOT supported by biharmonic code.

IMPORTANT: this function computes far field expansion,  but  it  does  NOT
           compute error bounds. By default, far field distance is set  to
           some extremely big number.
           You should explicitly set desired far  field  tolerance  (which
           leads to automatic computation of the UseAtDistance  field)  by
           calling bhPanelSetPrec().

INPUT PARAMETERS:
    Panel           -   panel to be initialized. Previously allocated
                        memory is reused as much as possible.
    XW              -   array[?,3+NY]:
                        * 3 first columns are X,Y,Z coordinates
                        * subsequent NY columns are basis function coefficients
    XIdx0, XIdx1    -   defines row range [XIdx0,XIdx1) of XW to process,
                        XIdx1-XIdx0 rows are processed, the rest is ignored
    NY              -   NY>=1, output values count
    Eval            -   precomputed table

  -- ALGLIB --
     Copyright 26.08.2022 by Sergey Bochkanov
*************************************************************************/
void bhpanelinit(biharmonicpanel* panel,
     /* Real    */ const ae_matrix* xw,
     ae_int_t xidx0,
     ae_int_t xidx1,
     ae_int_t ny,
     const biharmonicevaluator* eval,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t d;
    ae_int_t n;
    ae_int_t offs;
    ae_int_t doffs;
    double x0;
    double x1;
    double x2;
    double r;
    double r2;
    double r01;
    double v;
    double v0;
    double v1;
    double v2;
    double vnij;
    double vmij;
    double costheta;
    double sintheta;
    double powsinthetaj;
    double pnmprev;
    double pnm;
    double pnmnew;
    ae_complex inma;
    ae_complex expiminusphi;
    ae_complex expiminusjphi;
    ae_complex sphericaly;
    ae_complex innernm;
    ae_complex nnm;
    ae_complex mnm;
    ae_complex fmult;
    ae_int_t stride2;


    ae_assert(xidx1-xidx0>=1, "bhPanelInit: XIdx1<=XIdx0", _state);
    
    /*
     * Allocate space
     */
    panel->ny = ny;
    panel->p = eval->maxp;
    panel->stride = eval->maxp+1;
    panel->sizeinner = eval->maxp+1;
    panel->sizen = eval->maxp+1;
    panel->sizem = eval->maxp-1;
    panel->useatdistance = 0.001*ae_sqrt(ae_maxrealnumber, _state);
    csetallocv(ny*panel->stride*panel->stride, ae_complex_from_d(0.0), &panel->tbln, _state);
    csetallocv(ny*panel->stride*panel->stride, ae_complex_from_d(0.0), &panel->tblm, _state);
    csetallocv(ny*panel->stride*panel->stride, ae_complex_from_d(0.0), &panel->tblmodn, _state);
    csetallocv(ny*panel->stride*panel->stride, ae_complex_from_d(0.0), &panel->tblmodm, _state);
    rsetallocv(ny*4*panel->stride*panel->stride, 0.0, &panel->tblrmodmn, _state);
    stride2 = panel->stride*panel->stride;
    
    /*
     * Compute center, SubAbs and RMax
     */
    panel->maxsumabs = (double)(0);
    panel->c0 = (double)(0);
    panel->c1 = (double)(0);
    panel->c2 = (double)(0);
    for(k=xidx0; k<=xidx1-1; k++)
    {
        panel->c0 = panel->c0+xw->ptr.pp_double[k][0];
        panel->c1 = panel->c1+xw->ptr.pp_double[k][1];
        panel->c2 = panel->c2+xw->ptr.pp_double[k][2];
    }
    panel->c0 = panel->c0/(double)(xidx1-xidx0);
    panel->c1 = panel->c1/(double)(xidx1-xidx0);
    panel->c2 = panel->c2/(double)(xidx1-xidx0);
    panel->rmax = ae_machineepsilon;
    for(k=xidx0; k<=xidx1-1; k++)
    {
        v0 = xw->ptr.pp_double[k][0]-panel->c0;
        v1 = xw->ptr.pp_double[k][1]-panel->c1;
        v2 = xw->ptr.pp_double[k][2]-panel->c2;
        panel->rmax = ae_maxreal(panel->rmax, ae_sqrt(v0*v0+v1*v1+v2*v2, _state), _state);
    }
    for(d=0; d<=ny-1; d++)
    {
        v = (double)(0);
        for(k=xidx0; k<=xidx1-1; k++)
        {
            v = v+ae_fabs(xw->ptr.pp_double[k][3+d], _state);
        }
        panel->maxsumabs = ae_maxreal(panel->maxsumabs, v, _state);
    }
    
    /*
     * Precompute powers of RMax up to MaxP+1
     */
    rallocv(eval->maxp+2, &panel->tblpowrmax, _state);
    panel->tblpowrmax.ptr.p_double[0] = (double)(1);
    for(i=1; i<=eval->maxp+1; i++)
    {
        panel->tblpowrmax.ptr.p_double[i] = panel->tblpowrmax.ptr.p_double[i-1]*panel->rmax;
    }
    
    /*
     * Fill tables N and M
     */
    rallocv(panel->sizeinner, &panel->tpowr, _state);
    for(k=xidx0; k<=xidx1-1; k++)
    {
        
        /*
         * Prepare table of spherical harmonics for point K (to be used later to compute
         * inner functions I_nm and expansion functions N_nm and M_nm).
         */
        x0 = xw->ptr.pp_double[k][0]-panel->c0;
        x1 = xw->ptr.pp_double[k][1]-panel->c1;
        x2 = xw->ptr.pp_double[k][2]-panel->c2;
        r2 = x0*x0+x1*x1+x2*x2+ae_minrealnumber;
        r = ae_sqrt(r2, _state);
        r01 = ae_sqrt(x0*x0+x1*x1+ae_minrealnumber, _state);
        costheta = x2/r;
        sintheta = r01/r;
        expiminusphi.x = x0/r01;
        expiminusphi.y = -x1/r01;
        panel->tpowr.ptr.p_double[0] = (double)(1);
        for(i=1; i<=panel->sizeinner-1; i++)
        {
            panel->tpowr.ptr.p_double[i] = panel->tpowr.ptr.p_double[i-1]*r;
        }
        
        /*
         * Compute table of associated Legrengre polynomials, with
         *
         *     P_nm(N=I,M=-J,X)
         *
         * being an associated Legendre polynomial, as defined in "Numerical
         * recipes in C" and Wikipedia.
         *
         * It is important that the section 3 of 'Fast evaluation of polyharmonic
         * splines in three dimensions' by R.K. Beatson, M.J.D. Powell and A.M. Tan
         * uses different formulation of P_nm. We will account for the difference
         * during the computation of the spherical harmonics.
         */
        powsinthetaj = 1.0;
        expiminusjphi.x = 1.0;
        expiminusjphi.y = 0.0;
        for(j=0; j<=panel->stride-1; j++)
        {
            
            /*
             * Prepare recursion for associated Legendre polynomials
             */
            pnmprev = (double)(0);
            pnm = powsinthetaj*eval->pmmc.ptr.p_double[j];
            
            /*
             * Perform recursion on N
             */
            for(n=j; n<=panel->stride-1; n++)
            {
                offs = n*panel->stride+j;
                
                /*
                 * Compute table of associated Legrengre polynomials with
                 *
                 *     P_nm(N=I,M=-J,X)
                 *
                 * being an associated Legendre polynomial, as defined in "Numerical
                 * recipes in C" and Wikipedia.
                 *
                 * It is important that the section 3 of 'Fast evaluation of polyharmonic
                 * splines in three dimensions' by R.K. Beatson, M.J.D. Powell and A.M. Tan
                 * uses different formulation of P_nm. We will account for the difference
                 * during the computation of the spherical harmonics.
                 */
                if( n>j )
                {
                    
                    /*
                     * Recursion on N
                     */
                    pnmnew = pnm*costheta*eval->pnma.ptr.p_double[offs]+pnmprev*eval->pnmb.ptr.p_double[offs];
                    pnmprev = pnm;
                    pnm = pnmnew;
                }
                
                /*
                 * Compute table of spherical harmonics Y_nm(N=I,M=-J) with
                 *
                 *     Y_nm(N,M) = E_m*sqrt((n-m)!/(n+m)!)*P_nm(cos(theta))*exp(i*m*phi)
                 *     E_m = m>0 ? pow(-1,m) : 1
                 *     (because we always compute Y_nm for m<=0, E_m is always 1)
                 *
                 * being a spherical harmonic, as defined in the equation (18) of 'Fast evaluation of polyharmonic
                 * splines in three dimensions' by R.K. Beatson, M.J.D. Powell and A.M. Tan.
                 *
                 * Here P_nm is an associated Legendre polynomial as defined by Beatson et al. However, Pnm variable
                 * stores values of associated Legendre polynomials as defined by Wikipedia. Below we perform conversion
                 * between one format and another one:
                 *
                 *     P_nm(Beatson)  = P_nm(Wiki)*(-1)^(-m)*(n+m)!/(n-m)!
                 *     Y_nm(n=N,m=-J) = E_m*sqrt((n-m)!/(n+m)!)*P_nm(Beatson)*exp(i*m*phi)
                 *                    = E_m*sqrt((n-m)!/(n+m)!)*P_nm(Wiki)*(-1)^(-m)*(n+m)!/(n-m)!*exp(i*m*phi)
                 *                    = [E_m*(-1)^(-m)*sqrt((n+m)!/(n-m)!)*P_nm(Wiki)]*exp(i*m*phi)
                 *                    = A_ynm*P_nm(Wiki)*exp(i*m*phi)
                 */
                v = pnm*eval->ynma.ptr.p_double[offs];
                sphericaly.x = v*expiminusjphi.x;
                sphericaly.y = v*expiminusjphi.y;
                
                /*
                 * Compute inner functions table where
                 *
                 *     InnerNM = I_nm(n=I,m=-J),
                 *
                 * with
                 *
                 *     I_nm(n,m) = pow(i,|m|)*pow(-1,n)*pow(r,n)/sqrt((n-m)!(n+m)!)*Y_nm(theta,phi)
                 *               = I_ynm*r^n*Y_nm(theta,phi)
                 *
                 * being an inner function, as defined in equation (20) of 'Fast evaluation of polyharmonic splines in three dimensions'
                 * by R.K. Beatson, M.J.D. Powell and A.M. Tan 1.
                 */
                v = panel->tpowr.ptr.p_double[n];
                inma = eval->inma.ptr.p_complex[offs];
                innernm.x = v*(inma.x*sphericaly.x-inma.y*sphericaly.y);
                innernm.y = v*(inma.x*sphericaly.y+inma.y*sphericaly.x);
                
                /*
                 * Update expansion functions N_nm and M_nm with harmonics coming from the point #K
                 *
                 * NOTE: precomputed coefficient MnmA[] takes care of the fact that we require
                 *       Mnm for n,m>P-2 to be zero. It is exactly zero at the corresponding positions,
                 *       so we may proceed without conditional operators.
                 */
                for(d=0; d<=ny-1; d++)
                {
                    doffs = offs+d*stride2;
                    vnij = xw->ptr.pp_double[k][3+d]*eval->nnma.ptr.p_double[n];
                    nnm = panel->tbln.ptr.p_complex[doffs];
                    panel->tbln.ptr.p_complex[doffs].x = nnm.x+vnij*innernm.x;
                    panel->tbln.ptr.p_complex[doffs].y = nnm.y+vnij*innernm.y;
                    vmij = xw->ptr.pp_double[k][3+d]*panel->tpowr.ptr.p_double[2]*eval->mnma.ptr.p_double[n];
                    mnm = panel->tblm.ptr.p_complex[doffs];
                    panel->tblm.ptr.p_complex[doffs].x = mnm.x+vmij*innernm.x;
                    panel->tblm.ptr.p_complex[doffs].y = mnm.y+vmij*innernm.y;
                }
            }
            
            /*
             * Prepare for the next iteration
             */
            powsinthetaj = powsinthetaj*sintheta;
            v0 = expiminusjphi.x*expiminusphi.x-expiminusjphi.y*expiminusphi.y;
            v1 = expiminusjphi.x*expiminusphi.y+expiminusjphi.y*expiminusphi.x;
            expiminusjphi.x = v0;
            expiminusjphi.y = v1;
        }
    }
    
    /*
     * Compute modified N_nm and M_nm by multiplying original values by sqrt((n-m)!(n+m)!)*i^(-m),
     * with additional multiplication by 2 for J<>0
     *
     * This scaling factor is a part of the outer function O_nm which is computed during the model
     * evaluation, and it does NOT depends on the trial point X. So, merging it with N_nm/M_nm
     * saves us a lot of computational effort.
     */
    for(i=0; i<=panel->p; i++)
    {
        for(j=0; j<=i; j++)
        {
            v = eval->tsqrtfactorial.ptr.p_double[i+j]*eval->tsqrtfactorial.ptr.p_double[i-j];
            if( j!=0 )
            {
                v = v*(double)2;
            }
            fmult = eval->tpowi.ptr.p_complex[j];
            fmult.x = fmult.x*v;
            fmult.y = fmult.y*v;
            if( i<panel->sizen )
            {
                for(d=0; d<=ny-1; d++)
                {
                    nnm = panel->tbln.ptr.p_complex[d*stride2+i*panel->stride+j];
                    panel->tblmodn.ptr.p_complex[d*stride2+i*panel->stride+j].x = nnm.x*fmult.x-nnm.y*fmult.y;
                    panel->tblmodn.ptr.p_complex[d*stride2+i*panel->stride+j].y = nnm.x*fmult.y+nnm.y*fmult.x;
                }
            }
            if( i<panel->sizem )
            {
                for(d=0; d<=ny-1; d++)
                {
                    mnm = panel->tblm.ptr.p_complex[d*stride2+i*panel->stride+j];
                    panel->tblmodm.ptr.p_complex[d*stride2+i*panel->stride+j].x = mnm.x*fmult.x-mnm.y*fmult.y;
                    panel->tblmodm.ptr.p_complex[d*stride2+i*panel->stride+j].y = mnm.x*fmult.y+mnm.y*fmult.x;
                }
            }
        }
    }
    
    /*
     * Convert tblModN and tblModN into packed storage
     */
    offs = 0;
    doffs = 0;
    for(i=0; i<=(panel->p+1)*ny-1; i++)
    {
        for(j=0; j<=panel->p; j++)
        {
            panel->tblrmodmn.ptr.p_double[doffs+j] = panel->tblmodm.ptr.p_complex[offs+j].x;
        }
        doffs = doffs+panel->stride;
        for(j=0; j<=panel->p; j++)
        {
            panel->tblrmodmn.ptr.p_double[doffs+j] = panel->tblmodm.ptr.p_complex[offs+j].y;
        }
        doffs = doffs+panel->stride;
        for(j=0; j<=panel->p; j++)
        {
            panel->tblrmodmn.ptr.p_double[doffs+j] = panel->tblmodn.ptr.p_complex[offs+j].x;
        }
        doffs = doffs+panel->stride;
        for(j=0; j<=panel->p; j++)
        {
            panel->tblrmodmn.ptr.p_double[doffs+j] = panel->tblmodn.ptr.p_complex[offs+j].y;
        }
        doffs = doffs+panel->stride;
        offs = offs+panel->stride;
    }
    
    /*
     * Default UseAtDistance, means that far field is not used
     */
    panel->useatdistance = 1.0E50+1.0E6*panel->rmax;
}


/*************************************************************************
This function sets far field distance depending on desired accuracy.

INPUT PARAMETERS:
    Panel           -   panel with valid far field expansion
    Tol             -   desired tolerance

  -- ALGLIB --
     Copyright 20.11.2022 by Sergey Bochkanov
*************************************************************************/
void bhpanelsetprec(biharmonicpanel* panel, double tol, ae_state *_state)
{
    double errbnd;
    double rcand;


    ae_assert(ae_isfinite(tol, _state)&&ae_fp_greater(tol,(double)(0)), "bhPanelSetPrec: Tol<=0 or infinite", _state);
    rcand = panel->rmax;
    do
    {
        rcand = 1.05*rcand+ae_machineepsilon;
        errbnd = panel->maxsumabs*rcand*((double)2/(double)(2*panel->p+1))*ae_pow(panel->rmax/rcand, (double)(panel->p+1), _state)/((double)1-panel->rmax/rcand);
    }
    while(ae_fp_greater_eq(errbnd,tol));
    panel->useatdistance = rcand;
}


/*************************************************************************
Tries evaluating model using the far field expansion stored in the panel,
special case for NY=1

INPUT PARAMETERS:
    Panel           -   panel
    Eval            -   precomputed table
    X0, X1, X2      -   evaluation point
    NeedErrBnd      -   whether error bound is needed or not

OUTPUT PARAMETERS:
    F               -   model value
    ErrBnd          -   upper bound on the far field expansion error, if
                        requested. Zero otherwise.

  -- ALGLIB --
     Copyright 26.08.2022 by Sergey Bochkanov
*************************************************************************/
void bhpaneleval1(const biharmonicpanel* panel,
     const biharmonicevaluator* eval,
     double x0,
     double x1,
     double x2,
     double* f,
     ae_bool neederrbnd,
     double* errbnd,
     ae_state *_state)
{
    ae_int_t j;
    ae_int_t n;
    ae_complex vsummn;
    double v;
    double r;
    double r2;
    double r01;
    double v0;
    double v1;
    double invr;
    double invpowrmplus1;
    double invpowrnplus1;
    double invpowrpplus1;
    double pnmnew;
    double pnm;
    double pnmprev;
    double sintheta;
    double powsinthetaj;
    double costheta;
    ae_complex expiphi;
    ae_complex expijphi;
    ae_complex sphericaly;
    ae_int_t offs;

    *f = 0.0;
    *errbnd = 0.0;

    ae_assert(panel->ny==1, "RBF3EVAL1: NY>1", _state);
    
    /*
     * Center evaluation point
     */
    x0 = x0-panel->c0;
    x1 = x1-panel->c1;
    x2 = x2-panel->c2;
    r2 = x0*x0+x1*x1+x2*x2+ae_minrealnumber;
    r = ae_sqrt(r2, _state);
    
    /*
     * Try to use fast kernel.
     * If fast kernel returns False, use reference implementation below.
     */
    if( !rbfv3farfields_bhpaneleval1fastkernel(x0, x1, x2, panel->p, &eval->pnma, &eval->pnmb, &eval->pmmcdiag, &eval->ynma, &panel->tblrmodmn, f, &invpowrpplus1, _state) )
    {
        
        /*
         * No fast kernel.
         *
         * Convert to spherical polar coordinates.
         *
         * NOTE: we make sure that R is non-zero by adding extremely small perturbation
         */
        r01 = ae_sqrt(x0*x0+x1*x1+ae_minrealnumber, _state);
        costheta = x2/r;
        sintheta = r01/r;
        expiphi.x = x0/r01;
        expiphi.y = x1/r01;
        
        /*
         * Compute far field expansion for a cluster of basis functions f=r
         *
         * NOTE: the original paper by Beatson et al. uses f=r as the basis function,
         *       whilst ALGLIB uses f=-r due to conditional positive definiteness requirement.
         *       We will perform conversion later.
         */
        powsinthetaj = 1.0;
        *f = (double)(0);
        invr = (double)1/r;
        invpowrmplus1 = invr;
        expijphi.x = 1.0;
        expijphi.y = 0.0;
        for(j=0; j<=panel->p; j++)
        {
            invpowrnplus1 = invpowrmplus1;
            
            /*
             * Prepare recursion for associated Legendre polynomials
             */
            pnmprev = (double)(0);
            pnm = powsinthetaj*eval->pmmc.ptr.p_double[j];
            
            /*
             *
             */
            for(n=j; n<=panel->p; n++)
            {
                offs = n*panel->stride+j;
                
                /*
                 * Compute table of associated Legrengre polynomials with
                 *
                 *     P_nm(N=I,M=-J,X)
                 *
                 * being an associated Legendre polynomial, as defined in "Numerical
                 * recipes in C" and Wikipedia.
                 *
                 * It is important that the section 3 of 'Fast evaluation of polyharmonic
                 * splines in three dimensions' by R.K. Beatson, M.J.D. Powell and A.M. Tan
                 * uses different formulation of P_nm. We will account for the difference
                 * during the computation of the spherical harmonics.
                 */
                if( n>j )
                {
                    
                    /*
                     * Recursion on N
                     */
                    pnmnew = pnm*costheta*eval->pnma.ptr.p_double[offs]+pnmprev*eval->pnmb.ptr.p_double[offs];
                    pnmprev = pnm;
                    pnm = pnmnew;
                }
                
                /*
                 * Compute table of spherical harmonics where
                 *
                 *     funcSphericalY[I*N+J] = Y_nm(N=I,M=-J),
                 *
                 * with
                 *
                 *     Y_nm(N,M) = E_m*sqrt((n-m)!/(n+m)!)*P_nm(cos(theta))*exp(i*m*phi)
                 *     E_m = m>0 ? pow(-1,m) : 1
                 *     (because we always compute Y_nm for m<=0, E_m is always 1)
                 *
                 * being a spherical harmonic, as defined in the equation (18) of 'Fast evaluation of polyharmonic
                 * splines in three dimensions' by R.K. Beatson, M.J.D. Powell and A.M. Tan.
                 *
                 * Here P_nm is an associated Legendre polynomial as defined by Beatson et al. However, the Pnm
                 * variable stores values of associated Legendre polynomials as defined by Wikipedia. Below we perform conversion
                 * between one format and another one:
                 *
                 *     P_nm(Beatson) = P_nm(Wiki)*(-1)^(-m)*(n+m)!/(n-m)!
                 *     Y_nm(N,M)     = E_m*sqrt((n-m)!/(n+m)!)*P_nm(Beatson)*exp(i*m*phi)
                 *                   = E_m*sqrt((n-m)!/(n+m)!)*P_nm(Wiki)*(-1)^(-m)*(n+m)!/(n-m)!*exp(i*m*phi)
                 *                   = [E_m*(-1)^(-m)*sqrt((n+m)!/(n-m)!)*P_nm(Wiki)]*exp(i*m*phi)
                 *                   = YnmA[n,m]*P_nm(Wiki)*exp(i*m*phi)
                 */
                v = pnm*eval->ynma.ptr.p_double[offs];
                sphericaly.x = v*expijphi.x;
                sphericaly.y = v*expijphi.y;
                
                /*
                 * Compute outer function for n=N, m=1..N
                 * Update result with O_mn*(M_mn + R^2*N_mn).
                 *
                 * The most straighforward implementation of the loop below should look like as follows:
                 *
                 *     O_nm  = [sqrt((n-m)!(n+m)!)*i^m]*Y_nm/R^(n+1)
                 *     RES  += 2*RealPart[(R^2*N_nm+M_nm)*O_nm]
                 *
                 * However, we may save a lot of computational effort by moving [sqrt((n-m)!(n+m)!)*i^m]
                 * multiplier to the left part of the product, i.e. by merging it with N_nm and M_nm
                 * and producing MODIFIED expansions NMod and MMod. Because computing this multiplier
                 * involves three lookups into precomputed tables and one complex product, it may
                 * save us a lot of time.
                 */
                vsummn.x = r2*panel->tblmodn.ptr.p_complex[offs].x+panel->tblmodm.ptr.p_complex[offs].x;
                vsummn.y = r2*panel->tblmodn.ptr.p_complex[offs].y+panel->tblmodm.ptr.p_complex[offs].y;
                *f = *f+invpowrnplus1*(vsummn.x*sphericaly.x-vsummn.y*sphericaly.y);
                invpowrnplus1 = invpowrnplus1*invr;
            }
            
            /*
             * Prepare for the next iteration
             */
            powsinthetaj = powsinthetaj*sintheta;
            invpowrmplus1 = invpowrmplus1*invr;
            v0 = expijphi.x*expiphi.x-expijphi.y*expiphi.y;
            v1 = expijphi.x*expiphi.y+expijphi.y*expiphi.x;
            expijphi.x = v0;
            expijphi.y = v1;
        }
        invpowrpplus1 = r*invpowrmplus1;
    }
    
    /*
     * Convert from f=r to f=-r
     */
    *f = -*f;
    
    /*
     * Compute error bound
     */
    *errbnd = 0.0;
    if( neederrbnd )
    {
        *errbnd = panel->maxsumabs*r2*(double)2*panel->tblpowrmax.ptr.p_double[panel->p+1]*invpowrpplus1/((double)(2*panel->p+1)*(r-panel->rmax));
        *errbnd = *errbnd+(double)100*ae_machineepsilon*(panel->maxsumabs*r+ae_fabs(*f, _state));
    }
}


/*************************************************************************
Tries evaluating model using the far field expansion stored in the panel,
general case for NY>=1

INPUT PARAMETERS:
    Panel           -   panel
    Eval            -   precomputed table
    X0, X1, X2      -   evaluation point
    NeedErrBnd      -   whether error bound is needed or not

OUTPUT PARAMETERS:
    F               -   model value
    ErrBnd          -   upper bound on the far field expansion error, if
                        requested. Zero otherwise.

  -- ALGLIB --
     Copyright 10.11.2022 by Sergey Bochkanov
*************************************************************************/
void bhpaneleval(const biharmonicpanel* panel,
     const biharmonicevaluator* eval,
     double x0,
     double x1,
     double x2,
     /* Real    */ ae_vector* f,
     ae_bool neederrbnd,
     double* errbnd,
     ae_state *_state)
{
    ae_int_t j;
    ae_int_t k;
    ae_int_t n;
    ae_int_t ny;
    ae_int_t stride2;
    ae_complex vsummn;
    double v;
    double r;
    double r2;
    double r01;
    double v0;
    double v1;
    double invr;
    double invpowrmplus1;
    double invpowrnplus1;
    double pnmnew;
    double pnm;
    double pnmprev;
    double sintheta;
    double powsinthetaj;
    double invpowrpplus1;
    double costheta;
    ae_complex expiphi;
    ae_complex expijphi;
    ae_complex sphericaly;
    ae_int_t offs;
    ae_int_t offsk;
    double af;

    *errbnd = 0.0;

    ny = panel->ny;
    if( f->cnt<ny )
    {
        ae_vector_set_length(f, ny, _state);
    }
    
    /*
     * Center and convert to spherical polar coordinates.
     *
     * NOTE: we make sure that R is non-zero by adding extremely small perturbation
     */
    x0 = x0-panel->c0;
    x1 = x1-panel->c1;
    x2 = x2-panel->c2;
    r2 = x0*x0+x1*x1+x2*x2+ae_minrealnumber;
    r = ae_sqrt(r2, _state);
    r01 = ae_sqrt(x0*x0+x1*x1+ae_minrealnumber, _state);
    costheta = x2/r;
    sintheta = r01/r;
    expiphi.x = x0/r01;
    expiphi.y = x1/r01;
    
    /*
     * Try to use fast kernel.
     * If fast kernel returns False, use reference implementation below.
     */
    if( !rbfv3farfields_bhpanelevalfastkernel(x0, x1, x2, ny, panel->p, &eval->pnma, &eval->pnmb, &eval->pmmcdiag, &eval->ynma, &panel->tblrmodmn, f, &invpowrpplus1, _state) )
    {
        
        /*
         * No fast kernel.
         *
         * Compute far field expansion for a cluster of basis functions f=r
         *
         * NOTE: the original paper by Beatson et al. uses f=r as the basis function,
         *       whilst ALGLIB uses f=-r due to conditional positive definiteness requirement.
         *       We will perform conversion later.
         */
        powsinthetaj = 1.0;
        for(k=0; k<=ny-1; k++)
        {
            f->ptr.p_double[k] = (double)(0);
        }
        invr = (double)1/r;
        invpowrmplus1 = invr;
        expijphi.x = 1.0;
        expijphi.y = 0.0;
        stride2 = panel->stride*panel->stride;
        for(j=0; j<=panel->p; j++)
        {
            invpowrnplus1 = invpowrmplus1;
            
            /*
             * Prepare recursion for associated Legendre polynomials
             */
            pnmprev = (double)(0);
            pnm = powsinthetaj*eval->pmmc.ptr.p_double[j];
            
            /*
             *
             */
            for(n=j; n<=panel->p; n++)
            {
                offs = n*panel->stride+j;
                
                /*
                 * Compute table of associated Legrengre polynomials with
                 *
                 *     P_nm(N=I,M=-J,X)
                 *
                 * being an associated Legendre polynomial, as defined in "Numerical
                 * recipes in C" and Wikipedia.
                 *
                 * It is important that the section 3 of 'Fast evaluation of polyharmonic
                 * splines in three dimensions' by R.K. Beatson, M.J.D. Powell and A.M. Tan
                 * uses different formulation of P_nm. We will account for the difference
                 * during the computation of the spherical harmonics.
                 */
                if( n>j )
                {
                    
                    /*
                     * Recursion on N
                     */
                    pnmnew = pnm*costheta*eval->pnma.ptr.p_double[offs]+pnmprev*eval->pnmb.ptr.p_double[offs];
                    pnmprev = pnm;
                    pnm = pnmnew;
                }
                
                /*
                 * Compute table of spherical harmonics where
                 *
                 *     funcSphericalY[I*N+J] = Y_nm(N=I,M=-J),
                 *
                 * with
                 *
                 *     Y_nm(N,M) = E_m*sqrt((n-m)!/(n+m)!)*P_nm(cos(theta))*exp(i*m*phi)
                 *     E_m = m>0 ? pow(-1,m) : 1
                 *     (because we always compute Y_nm for m<=0, E_m is always 1)
                 *
                 * being a spherical harmonic, as defined in the equation (18) of 'Fast evaluation of polyharmonic
                 * splines in three dimensions' by R.K. Beatson, M.J.D. Powell and A.M. Tan.
                 *
                 * Here P_nm is an associated Legendre polynomial as defined by Beatson et al. However, the Pnm
                 * variable stores values of associated Legendre polynomials as defined by Wikipedia. Below we perform conversion
                 * between one format and another one:
                 *
                 *     P_nm(Beatson) = P_nm(Wiki)*(-1)^(-m)*(n+m)!/(n-m)!
                 *     Y_nm(N,M)     = E_m*sqrt((n-m)!/(n+m)!)*P_nm(Beatson)*exp(i*m*phi)
                 *                   = E_m*sqrt((n-m)!/(n+m)!)*P_nm(Wiki)*(-1)^(-m)*(n+m)!/(n-m)!*exp(i*m*phi)
                 *                   = [E_m*(-1)^(-m)*sqrt((n+m)!/(n-m)!)*P_nm(Wiki)]*exp(i*m*phi)
                 *                   = YnmA[n,m]*P_nm(Wiki)*exp(i*m*phi)
                 */
                v = pnm*eval->ynma.ptr.p_double[offs];
                sphericaly.x = v*expijphi.x;
                sphericaly.y = v*expijphi.y;
                
                /*
                 * Compute outer function for n=N, m=1..N
                 * Update result with O_mn*(M_mn + R^2*N_mn).
                 *
                 * The most straighforward implementation of the loop below should look like as follows:
                 *
                 *     O_nm  = [sqrt((n-m)!(n+m)!)*i^m]*Y_nm/R^(n+1)
                 *     RES  += 2*RealPart[(R^2*N_nm+M_nm)*O_nm]
                 *
                 * However, we may save a lot of computational effort by moving [sqrt((n-m)!(n+m)!)*i^m]
                 * multiplier to the left part of the product, i.e. by merging it with N_nm and M_nm
                 * and producing MODIFIED expansions NMod and MMod. Because computing this multiplier
                 * involves three lookups into precomputed tables and one complex product, it may
                 * save us a lot of time.
                 */
                offsk = offs;
                for(k=0; k<=ny-1; k++)
                {
                    vsummn.x = r2*panel->tblmodn.ptr.p_complex[offsk].x+panel->tblmodm.ptr.p_complex[offsk].x;
                    vsummn.y = r2*panel->tblmodn.ptr.p_complex[offsk].y+panel->tblmodm.ptr.p_complex[offsk].y;
                    f->ptr.p_double[k] = f->ptr.p_double[k]+invpowrnplus1*(vsummn.x*sphericaly.x-vsummn.y*sphericaly.y);
                    offsk = offsk+stride2;
                }
                invpowrnplus1 = invpowrnplus1*invr;
            }
            
            /*
             * Prepare for the next iteration
             */
            powsinthetaj = powsinthetaj*sintheta;
            invpowrmplus1 = invpowrmplus1*invr;
            v0 = expijphi.x*expiphi.x-expijphi.y*expiphi.y;
            v1 = expijphi.x*expiphi.y+expijphi.y*expiphi.x;
            expijphi.x = v0;
            expijphi.y = v1;
        }
        invpowrpplus1 = r*invpowrmplus1;
    }
    
    /*
     * Convert from f=r to f=-r
     */
    for(k=0; k<=ny-1; k++)
    {
        f->ptr.p_double[k] = -f->ptr.p_double[k];
    }
    
    /*
     * Compute error bound, if needed
     */
    *errbnd = 0.0;
    if( neederrbnd )
    {
        af = (double)(0);
        for(k=0; k<=ny-1; k++)
        {
            af = ae_maxreal(af, ae_fabs(f->ptr.p_double[k], _state), _state);
        }
        *errbnd = panel->maxsumabs*r2*(double)2*panel->tblpowrmax.ptr.p_double[panel->p+1]*invpowrpplus1/((double)(2*panel->p+1)*(r-panel->rmax));
        *errbnd = *errbnd+(double)100*ae_machineepsilon*(panel->maxsumabs*r+af);
    }
}


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Fast kernel for biharmonic panel with NY=1

INPUT PARAMETERS:
    D0, D1, D2      -   evaluation point minus (Panel.C0,Panel.C1,Panel.C2)

OUTPUT PARAMETERS:
    F               -   model value
    InvPowRPPlus1   -   1/(R^(P+1))

  -- ALGLIB --
     Copyright 26.08.2022 by Sergey Bochkanov
*************************************************************************/
static ae_bool rbfv3farfields_bhpaneleval1fastkernel(double d0,
     double d1,
     double d2,
     ae_int_t panelp,
     /* Real    */ const ae_vector* pnma,
     /* Real    */ const ae_vector* pnmb,
     /* Real    */ const ae_vector* pmmcdiag,
     /* Real    */ const ae_vector* ynma,
     /* Real    */ const ae_vector* tblrmodmn,
     double* f,
     double* invpowrpplus1,
     ae_state *_state)
{
    ae_bool result;

    *f = 0.0;
    *invpowrpplus1 = 0.0;

    *f = (double)(0);
    *invpowrpplus1 = (double)(0);
    result = ae_false;
    return result;
}
#endif


#ifdef ALGLIB_NO_FAST_KERNELS
/*************************************************************************
Fast kernel for biharmonic panel with general NY

INPUT PARAMETERS:
    D0, D1, D2      -   evaluation point minus (Panel.C0,Panel.C1,Panel.C2)

OUTPUT PARAMETERS:
    F               -   model value
    InvPowRPPlus1   -   1/(R^(P+1))

  -- ALGLIB --
     Copyright 26.08.2022 by Sergey Bochkanov
*************************************************************************/
static ae_bool rbfv3farfields_bhpanelevalfastkernel(double d0,
     double d1,
     double d2,
     ae_int_t ny,
     ae_int_t panelp,
     /* Real    */ const ae_vector* pnma,
     /* Real    */ const ae_vector* pnmb,
     /* Real    */ const ae_vector* pmmcdiag,
     /* Real    */ const ae_vector* ynma,
     /* Real    */ const ae_vector* tblrmodmn,
     /* Real    */ ae_vector* f,
     double* invpowrpplus1,
     ae_state *_state)
{
    ae_bool result;

    *invpowrpplus1 = 0.0;

    *invpowrpplus1 = (double)(0);
    result = ae_false;
    return result;
}
#endif


void _biharmonicevaluator_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    biharmonicevaluator *p = (biharmonicevaluator*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->tdoublefactorial, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tfactorial, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tsqrtfactorial, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tpowminus1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tpowi, 0, DT_COMPLEX, _state, make_automatic);
    ae_vector_init(&p->tpowminusi, 0, DT_COMPLEX, _state, make_automatic);
    ae_vector_init(&p->ynma, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->pnma, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->pnmb, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->pmmc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->pmmcdiag, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->mnma, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nnma, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->inma, 0, DT_COMPLEX, _state, make_automatic);
}


void _biharmonicevaluator_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    biharmonicevaluator       *dst = (biharmonicevaluator*)_dst;
    const biharmonicevaluator *src = (const biharmonicevaluator*)_src;
    dst->maxp = src->maxp;
    dst->precomputedcount = src->precomputedcount;
    ae_vector_init_copy(&dst->tdoublefactorial, &src->tdoublefactorial, _state, make_automatic);
    ae_vector_init_copy(&dst->tfactorial, &src->tfactorial, _state, make_automatic);
    ae_vector_init_copy(&dst->tsqrtfactorial, &src->tsqrtfactorial, _state, make_automatic);
    ae_vector_init_copy(&dst->tpowminus1, &src->tpowminus1, _state, make_automatic);
    ae_vector_init_copy(&dst->tpowi, &src->tpowi, _state, make_automatic);
    ae_vector_init_copy(&dst->tpowminusi, &src->tpowminusi, _state, make_automatic);
    ae_vector_init_copy(&dst->ynma, &src->ynma, _state, make_automatic);
    ae_vector_init_copy(&dst->pnma, &src->pnma, _state, make_automatic);
    ae_vector_init_copy(&dst->pnmb, &src->pnmb, _state, make_automatic);
    ae_vector_init_copy(&dst->pmmc, &src->pmmc, _state, make_automatic);
    ae_vector_init_copy(&dst->pmmcdiag, &src->pmmcdiag, _state, make_automatic);
    ae_vector_init_copy(&dst->mnma, &src->mnma, _state, make_automatic);
    ae_vector_init_copy(&dst->nnma, &src->nnma, _state, make_automatic);
    ae_vector_init_copy(&dst->inma, &src->inma, _state, make_automatic);
}


void _biharmonicevaluator_clear(void* _p)
{
    biharmonicevaluator *p = (biharmonicevaluator*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->tdoublefactorial);
    ae_vector_clear(&p->tfactorial);
    ae_vector_clear(&p->tsqrtfactorial);
    ae_vector_clear(&p->tpowminus1);
    ae_vector_clear(&p->tpowi);
    ae_vector_clear(&p->tpowminusi);
    ae_vector_clear(&p->ynma);
    ae_vector_clear(&p->pnma);
    ae_vector_clear(&p->pnmb);
    ae_vector_clear(&p->pmmc);
    ae_vector_clear(&p->pmmcdiag);
    ae_vector_clear(&p->mnma);
    ae_vector_clear(&p->nnma);
    ae_vector_clear(&p->inma);
}


void _biharmonicevaluator_destroy(void* _p)
{
    biharmonicevaluator *p = (biharmonicevaluator*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->tdoublefactorial);
    ae_vector_destroy(&p->tfactorial);
    ae_vector_destroy(&p->tsqrtfactorial);
    ae_vector_destroy(&p->tpowminus1);
    ae_vector_destroy(&p->tpowi);
    ae_vector_destroy(&p->tpowminusi);
    ae_vector_destroy(&p->ynma);
    ae_vector_destroy(&p->pnma);
    ae_vector_destroy(&p->pnmb);
    ae_vector_destroy(&p->pmmc);
    ae_vector_destroy(&p->pmmcdiag);
    ae_vector_destroy(&p->mnma);
    ae_vector_destroy(&p->nnma);
    ae_vector_destroy(&p->inma);
}


void _biharmonicpanel_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    biharmonicpanel *p = (biharmonicpanel*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->tbln, 0, DT_COMPLEX, _state, make_automatic);
    ae_vector_init(&p->tblm, 0, DT_COMPLEX, _state, make_automatic);
    ae_vector_init(&p->tblmodn, 0, DT_COMPLEX, _state, make_automatic);
    ae_vector_init(&p->tblmodm, 0, DT_COMPLEX, _state, make_automatic);
    ae_vector_init(&p->tblpowrmax, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tblrmodmn, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->funcsphericaly, 0, DT_COMPLEX, _state, make_automatic);
    ae_vector_init(&p->tpowr, 0, DT_REAL, _state, make_automatic);
}


void _biharmonicpanel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    biharmonicpanel       *dst = (biharmonicpanel*)_dst;
    const biharmonicpanel *src = (const biharmonicpanel*)_src;
    dst->c0 = src->c0;
    dst->c1 = src->c1;
    dst->c2 = src->c2;
    dst->rmax = src->rmax;
    dst->useatdistance = src->useatdistance;
    dst->ny = src->ny;
    dst->p = src->p;
    dst->sizen = src->sizen;
    dst->sizem = src->sizem;
    dst->stride = src->stride;
    dst->sizeinner = src->sizeinner;
    ae_vector_init_copy(&dst->tbln, &src->tbln, _state, make_automatic);
    ae_vector_init_copy(&dst->tblm, &src->tblm, _state, make_automatic);
    ae_vector_init_copy(&dst->tblmodn, &src->tblmodn, _state, make_automatic);
    ae_vector_init_copy(&dst->tblmodm, &src->tblmodm, _state, make_automatic);
    ae_vector_init_copy(&dst->tblpowrmax, &src->tblpowrmax, _state, make_automatic);
    ae_vector_init_copy(&dst->tblrmodmn, &src->tblrmodmn, _state, make_automatic);
    dst->maxsumabs = src->maxsumabs;
    ae_vector_init_copy(&dst->funcsphericaly, &src->funcsphericaly, _state, make_automatic);
    ae_vector_init_copy(&dst->tpowr, &src->tpowr, _state, make_automatic);
}


void _biharmonicpanel_clear(void* _p)
{
    biharmonicpanel *p = (biharmonicpanel*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->tbln);
    ae_vector_clear(&p->tblm);
    ae_vector_clear(&p->tblmodn);
    ae_vector_clear(&p->tblmodm);
    ae_vector_clear(&p->tblpowrmax);
    ae_vector_clear(&p->tblrmodmn);
    ae_vector_clear(&p->funcsphericaly);
    ae_vector_clear(&p->tpowr);
}


void _biharmonicpanel_destroy(void* _p)
{
    biharmonicpanel *p = (biharmonicpanel*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->tbln);
    ae_vector_destroy(&p->tblm);
    ae_vector_destroy(&p->tblmodn);
    ae_vector_destroy(&p->tblmodm);
    ae_vector_destroy(&p->tblpowrmax);
    ae_vector_destroy(&p->tblrmodmn);
    ae_vector_destroy(&p->funcsphericaly);
    ae_vector_destroy(&p->tpowr);
}


/*$ End $*/
