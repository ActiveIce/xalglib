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
#include "lsfit.h"


/*$ Declarations $*/
static void lsfit_rdpanalyzesection(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t i0,
     ae_int_t i1,
     ae_int_t* worstidx,
     double* worsterror,
     ae_state *_state);
static void lsfit_rdprecursive(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t i0,
     ae_int_t i1,
     double eps,
     /* Real    */ ae_vector* xout,
     /* Real    */ ae_vector* yout,
     ae_int_t* nout,
     ae_state *_state);
static void lsfit_logisticfitinternal(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     ae_bool is4pl,
     double lambdav,
     minlmstate* state,
     minlmreport* replm,
     /* Real    */ ae_vector* p1,
     double* flast,
     ae_state *_state);
static void lsfit_logisticfit45errors(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     double a,
     double b,
     double c,
     double d,
     double g,
     lsfitreport* rep,
     ae_state *_state);
static void lsfit_spline1dfitinternal(ae_int_t st,
     /* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     /* Real    */ const ae_vector* _w,
     ae_int_t n,
     /* Real    */ const ae_vector* _xc,
     /* Real    */ const ae_vector* _yc,
     /* Integer */ const ae_vector* dc,
     ae_int_t k,
     ae_int_t m,
     spline1dinterpolant* s,
     spline1dfitreport* rep,
     ae_state *_state);
static void lsfit_lsfitlinearinternal(/* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* w,
     /* Real    */ const ae_matrix* fmatrix,
     ae_int_t n,
     ae_int_t m,
     /* Real    */ ae_vector* c,
     lsfitreport* rep,
     ae_state *_state);
static void lsfit_lsfitclearrequestfields(lsfitstate* state,
     ae_state *_state);
static void lsfit_barycentriccalcbasis(const barycentricinterpolant* b,
     double t,
     /* Real    */ ae_vector* y,
     ae_state *_state);
static void lsfit_internalchebyshevfit(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* w,
     ae_int_t n,
     /* Real    */ const ae_vector* _xc,
     /* Real    */ const ae_vector* _yc,
     /* Integer */ const ae_vector* dc,
     ae_int_t k,
     ae_int_t m,
     /* Real    */ ae_vector* c,
     lsfitreport* rep,
     ae_state *_state);
static void lsfit_barycentricfitwcfixedd(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     /* Real    */ const ae_vector* _w,
     ae_int_t n,
     /* Real    */ const ae_vector* _xc,
     /* Real    */ const ae_vector* _yc,
     /* Integer */ const ae_vector* dc,
     ae_int_t k,
     ae_int_t m,
     ae_int_t d,
     ae_int_t* info,
     barycentricinterpolant* b,
     barycentricfitreport* rep,
     ae_state *_state);
static void lsfit_clearreport(lsfitreport* rep, ae_state *_state);
static void lsfit_estimateerrors(/* Real    */ const ae_matrix* f1,
     /* Real    */ const ae_vector* f0,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* w,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* _s,
     ae_int_t n,
     ae_int_t k,
     lsfitreport* rep,
     /* Real    */ ae_matrix* z,
     ae_int_t zkind,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
This  subroutine fits piecewise linear curve to points with Ramer-Douglas-
Peucker algorithm, which stops after generating specified number of linear
sections.

IMPORTANT:
* it does NOT perform least-squares fitting; it  builds  curve,  but  this
  curve does not minimize some least squares metric.  See  description  of
  RDP algorithm (say, in Wikipedia) for more details on WHAT is performed.
* this function does NOT work with parametric curves  (i.e.  curves  which
  can be represented as {X(t),Y(t)}. It works with curves   which  can  be
  represented as Y(X). Thus,  it  is  impossible  to  model  figures  like
  circles  with  this  functions.
  If  you  want  to  work  with  parametric   curves,   you   should   use
  ParametricRDPFixed() function provided  by  "Parametric"  subpackage  of
  "Interpolation" package.

INPUT PARAMETERS:
    X       -   array of X-coordinates:
                * at least N elements
                * can be unordered (points are automatically sorted)
                * this function may accept non-distinct X (see below for
                  more information on handling of such inputs)
    Y       -   array of Y-coordinates:
                * at least N elements
    N       -   number of elements in X/Y
    M       -   desired number of sections:
                * at most M sections are generated by this function
                * less than M sections can be generated if we have N<M
                  (or some X are non-distinct).

OUTPUT PARAMETERS:
    X2      -   X-values of corner points for piecewise approximation,
                has length NSections+1 or zero (for NSections=0).
    Y2      -   Y-values of corner points,
                has length NSections+1 or zero (for NSections=0).
    NSections-  number of sections found by algorithm, NSections<=M,
                NSections can be zero for degenerate datasets
                (N<=1 or all X[] are non-distinct).

NOTE: X2/Y2 are ordered arrays, i.e. (X2[0],Y2[0]) is  a  first  point  of
      curve, (X2[NSection-1],Y2[NSection-1]) is the last point.

  -- ALGLIB --
     Copyright 02.10.2014 by Bochkanov Sergey
*************************************************************************/
void lstfitpiecewiselinearrdpfixed(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     ae_int_t n,
     ae_int_t m,
     /* Real    */ ae_vector* x2,
     /* Real    */ ae_vector* y2,
     ae_int_t* nsections,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector x;
    ae_vector y;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t k0;
    ae_int_t k1;
    ae_int_t k2;
    ae_vector buf0;
    ae_vector buf1;
    ae_matrix sections;
    ae_vector points;
    double v;
    ae_int_t worstidx;
    double worsterror;
    ae_int_t idx0;
    ae_int_t idx1;
    double e0;
    double e1;
    ae_vector heaperrors;
    ae_vector heaptags;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&y, 0, sizeof(y));
    memset(&buf0, 0, sizeof(buf0));
    memset(&buf1, 0, sizeof(buf1));
    memset(&sections, 0, sizeof(sections));
    memset(&points, 0, sizeof(points));
    memset(&heaperrors, 0, sizeof(heaperrors));
    memset(&heaptags, 0, sizeof(heaptags));
    ae_vector_init_copy(&x, _x, _state, ae_true);
    ae_vector_init_copy(&y, _y, _state, ae_true);
    ae_vector_clear(x2);
    ae_vector_clear(y2);
    *nsections = 0;
    ae_vector_init(&buf0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&buf1, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&sections, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&points, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&heaperrors, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&heaptags, 0, DT_INT, _state, ae_true);

    ae_assert(n>=0, "LSTFitPiecewiseLinearRDPFixed: N<0", _state);
    ae_assert(m>=1, "LSTFitPiecewiseLinearRDPFixed: M<1", _state);
    ae_assert(x.cnt>=n, "LSTFitPiecewiseLinearRDPFixed: Length(X)<N", _state);
    ae_assert(y.cnt>=n, "LSTFitPiecewiseLinearRDPFixed: Length(Y)<N", _state);
    if( n<=1 )
    {
        *nsections = 0;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Sort points.
     * Handle possible ties (tied values are replaced by their mean)
     */
    tagsortfastr(&x, &y, &buf0, &buf1, n, _state);
    i = 0;
    while(i<=n-1)
    {
        j = i+1;
        v = y.ptr.p_double[i];
        while(j<=n-1&&ae_fp_eq(x.ptr.p_double[j],x.ptr.p_double[i]))
        {
            v = v+y.ptr.p_double[j];
            j = j+1;
        }
        v = v/(double)(j-i);
        for(k=i; k<=j-1; k++)
        {
            y.ptr.p_double[k] = v;
        }
        i = j;
    }
    
    /*
     * Handle degenerate case x[0]=x[N-1]
     */
    if( ae_fp_eq(x.ptr.p_double[n-1],x.ptr.p_double[0]) )
    {
        *nsections = 0;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Prepare first section
     */
    lsfit_rdpanalyzesection(&x, &y, 0, n-1, &worstidx, &worsterror, _state);
    ae_matrix_set_length(&sections, m, 4, _state);
    ae_vector_set_length(&heaperrors, m, _state);
    ae_vector_set_length(&heaptags, m, _state);
    *nsections = 1;
    sections.ptr.pp_double[0][0] = (double)(0);
    sections.ptr.pp_double[0][1] = (double)(n-1);
    sections.ptr.pp_double[0][2] = (double)(worstidx);
    sections.ptr.pp_double[0][3] = worsterror;
    heaperrors.ptr.p_double[0] = worsterror;
    heaptags.ptr.p_int[0] = 0;
    ae_assert(ae_fp_eq(sections.ptr.pp_double[0][1],(double)(n-1)), "RDP algorithm: integrity check failed", _state);
    
    /*
     * Main loop.
     * Repeatedly find section with worst error and divide it.
     * Terminate after M-th section, or because of other reasons (see loop internals).
     */
    while(*nsections<m)
    {
        
        /*
         * Break if worst section has zero error.
         * Store index of worst section to K.
         */
        if( ae_fp_eq(heaperrors.ptr.p_double[0],(double)(0)) )
        {
            break;
        }
        k = heaptags.ptr.p_int[0];
        
        /*
         * K-th section is divided in two:
         * * first  one spans interval from X[Sections[K,0]] to X[Sections[K,2]]
         * * second one spans interval from X[Sections[K,2]] to X[Sections[K,1]]
         *
         * First section is stored at K-th position, second one is appended to the table.
         * Then we update heap which stores pairs of (error,section_index)
         */
        k0 = ae_round(sections.ptr.pp_double[k][0], _state);
        k1 = ae_round(sections.ptr.pp_double[k][1], _state);
        k2 = ae_round(sections.ptr.pp_double[k][2], _state);
        lsfit_rdpanalyzesection(&x, &y, k0, k2, &idx0, &e0, _state);
        lsfit_rdpanalyzesection(&x, &y, k2, k1, &idx1, &e1, _state);
        sections.ptr.pp_double[k][0] = (double)(k0);
        sections.ptr.pp_double[k][1] = (double)(k2);
        sections.ptr.pp_double[k][2] = (double)(idx0);
        sections.ptr.pp_double[k][3] = e0;
        tagheapreplacetopi(&heaperrors, &heaptags, *nsections, e0, k, _state);
        sections.ptr.pp_double[*nsections][0] = (double)(k2);
        sections.ptr.pp_double[*nsections][1] = (double)(k1);
        sections.ptr.pp_double[*nsections][2] = (double)(idx1);
        sections.ptr.pp_double[*nsections][3] = e1;
        tagheappushi(&heaperrors, &heaptags, nsections, e1, *nsections, _state);
    }
    
    /*
     * Convert from sections to points
     */
    ae_vector_set_length(&points, *nsections+1, _state);
    k = ae_round(sections.ptr.pp_double[0][1], _state);
    for(i=0; i<=*nsections-1; i++)
    {
        points.ptr.p_double[i] = (double)(ae_round(sections.ptr.pp_double[i][0], _state));
        if( ae_fp_greater(x.ptr.p_double[ae_round(sections.ptr.pp_double[i][1], _state)],x.ptr.p_double[k]) )
        {
            k = ae_round(sections.ptr.pp_double[i][1], _state);
        }
    }
    points.ptr.p_double[*nsections] = (double)(k);
    tagsortfast(&points, &buf0, *nsections+1, _state);
    
    /*
     * Output sections:
     * * first NSection elements of X2/Y2 are filled by x/y at left boundaries of sections
     * * last element of X2/Y2 is filled by right boundary of rightmost section
     * * X2/Y2 is sorted by ascending of X2
     */
    ae_vector_set_length(x2, *nsections+1, _state);
    ae_vector_set_length(y2, *nsections+1, _state);
    for(i=0; i<=*nsections; i++)
    {
        x2->ptr.p_double[i] = x.ptr.p_double[ae_round(points.ptr.p_double[i], _state)];
        y2->ptr.p_double[i] = y.ptr.p_double[ae_round(points.ptr.p_double[i], _state)];
    }
    ae_frame_leave(_state);
}


/*************************************************************************
This  subroutine fits piecewise linear curve to points with Ramer-Douglas-
Peucker algorithm, which stops after achieving desired precision.

IMPORTANT:
* it performs non-least-squares fitting; it builds curve, but  this  curve
  does not minimize some least squares  metric.  See  description  of  RDP
  algorithm (say, in Wikipedia) for more details on WHAT is performed.
* this function does NOT work with parametric curves  (i.e.  curves  which
  can be represented as {X(t),Y(t)}. It works with curves   which  can  be
  represented as Y(X). Thus, it is impossible to model figures like circles
  with this functions.
  If  you  want  to  work  with  parametric   curves,   you   should   use
  ParametricRDPFixed() function provided  by  "Parametric"  subpackage  of
  "Interpolation" package.

INPUT PARAMETERS:
    X       -   array of X-coordinates:
                * at least N elements
                * can be unordered (points are automatically sorted)
                * this function may accept non-distinct X (see below for
                  more information on handling of such inputs)
    Y       -   array of Y-coordinates:
                * at least N elements
    N       -   number of elements in X/Y
    Eps     -   positive number, desired precision.
    

OUTPUT PARAMETERS:
    X2      -   X-values of corner points for piecewise approximation,
                has length NSections+1 or zero (for NSections=0).
    Y2      -   Y-values of corner points,
                has length NSections+1 or zero (for NSections=0).
    NSections-  number of sections found by algorithm,
                NSections can be zero for degenerate datasets
                (N<=1 or all X[] are non-distinct).

NOTE: X2/Y2 are ordered arrays, i.e. (X2[0],Y2[0]) is  a  first  point  of
      curve, (X2[NSection-1],Y2[NSection-1]) is the last point.

  -- ALGLIB --
     Copyright 02.10.2014 by Bochkanov Sergey
*************************************************************************/
void lstfitpiecewiselinearrdp(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     ae_int_t n,
     double eps,
     /* Real    */ ae_vector* x2,
     /* Real    */ ae_vector* y2,
     ae_int_t* nsections,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector x;
    ae_vector y;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_vector buf0;
    ae_vector buf1;
    ae_vector xtmp;
    ae_vector ytmp;
    double v;
    ae_int_t npts;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&y, 0, sizeof(y));
    memset(&buf0, 0, sizeof(buf0));
    memset(&buf1, 0, sizeof(buf1));
    memset(&xtmp, 0, sizeof(xtmp));
    memset(&ytmp, 0, sizeof(ytmp));
    ae_vector_init_copy(&x, _x, _state, ae_true);
    ae_vector_init_copy(&y, _y, _state, ae_true);
    ae_vector_clear(x2);
    ae_vector_clear(y2);
    *nsections = 0;
    ae_vector_init(&buf0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&buf1, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xtmp, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ytmp, 0, DT_REAL, _state, ae_true);

    ae_assert(n>=0, "LSTFitPiecewiseLinearRDP: N<0", _state);
    ae_assert(ae_fp_greater(eps,(double)(0)), "LSTFitPiecewiseLinearRDP: Eps<=0", _state);
    ae_assert(x.cnt>=n, "LSTFitPiecewiseLinearRDP: Length(X)<N", _state);
    ae_assert(y.cnt>=n, "LSTFitPiecewiseLinearRDP: Length(Y)<N", _state);
    if( n<=1 )
    {
        *nsections = 0;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Sort points.
     * Handle possible ties (tied values are replaced by their mean)
     */
    tagsortfastr(&x, &y, &buf0, &buf1, n, _state);
    i = 0;
    while(i<=n-1)
    {
        j = i+1;
        v = y.ptr.p_double[i];
        while(j<=n-1&&ae_fp_eq(x.ptr.p_double[j],x.ptr.p_double[i]))
        {
            v = v+y.ptr.p_double[j];
            j = j+1;
        }
        v = v/(double)(j-i);
        for(k=i; k<=j-1; k++)
        {
            y.ptr.p_double[k] = v;
        }
        i = j;
    }
    
    /*
     * Handle degenerate case x[0]=x[N-1]
     */
    if( ae_fp_eq(x.ptr.p_double[n-1],x.ptr.p_double[0]) )
    {
        *nsections = 0;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Prepare data for recursive algorithm
     */
    ae_vector_set_length(&xtmp, n, _state);
    ae_vector_set_length(&ytmp, n, _state);
    npts = 2;
    xtmp.ptr.p_double[0] = x.ptr.p_double[0];
    ytmp.ptr.p_double[0] = y.ptr.p_double[0];
    xtmp.ptr.p_double[1] = x.ptr.p_double[n-1];
    ytmp.ptr.p_double[1] = y.ptr.p_double[n-1];
    lsfit_rdprecursive(&x, &y, 0, n-1, eps, &xtmp, &ytmp, &npts, _state);
    
    /*
     * Output sections:
     * * first NSection elements of X2/Y2 are filled by x/y at left boundaries of sections
     * * last element of X2/Y2 is filled by right boundary of rightmost section
     * * X2/Y2 is sorted by ascending of X2
     */
    *nsections = npts-1;
    ae_vector_set_length(x2, npts, _state);
    ae_vector_set_length(y2, npts, _state);
    for(i=0; i<=*nsections; i++)
    {
        x2->ptr.p_double[i] = xtmp.ptr.p_double[i];
        y2->ptr.p_double[i] = ytmp.ptr.p_double[i];
    }
    tagsortfastr(x2, y2, &buf0, &buf1, npts, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Fitting by polynomials in barycentric form. This function provides  simple
unterface for unconstrained unweighted fitting. See  PolynomialFitWC()  if
you need constrained fitting.

The task is linear, thus the linear least  squares  solver  is  used.  The
complexity of this computational scheme is O(N*M^2), mostly  dominated  by
the least squares solver

SEE ALSO:
    PolynomialFitWC()

NOTES:
    you can convert P from barycentric form  to  the  power  or  Chebyshev
    basis with PolynomialBar2Pow() or PolynomialBar2Cheb() functions  from
    POLINT subpackage.

INPUT PARAMETERS:
    X   -   points, array[0..N-1].
    Y   -   function values, array[0..N-1].
    N   -   number of points, N>0
            * if given, only leading N elements of X/Y are used
            * if not given, automatically determined from sizes of X/Y
    M   -   number of basis functions (= polynomial_degree + 1), M>=1

OUTPUT PARAMETERS:
    P   -   interpolant in barycentric form for Rep.TerminationType>0.
            undefined for Rep.TerminationType<0.
    Rep -   fitting report. The following fields are set:
                * Rep.TerminationType is a completion code which is always
                  set to 1 (success)
                * RMSError      rms error on the (X,Y).
                * AvgError      average error on the (X,Y).
                * AvgRelError   average relative error on the non-zero Y
                * MaxError      maximum error
                                NON-WEIGHTED ERRORS ARE CALCULATED

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

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

  -- ALGLIB PROJECT --
     Copyright 10.12.2009 by Bochkanov Sergey
*************************************************************************/
void polynomialfit(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     ae_int_t m,
     barycentricinterpolant* p,
     polynomialfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_vector w;
    ae_vector xc;
    ae_vector yc;
    ae_vector dc;

    ae_frame_make(_state, &_frame_block);
    memset(&w, 0, sizeof(w));
    memset(&xc, 0, sizeof(xc));
    memset(&yc, 0, sizeof(yc));
    memset(&dc, 0, sizeof(dc));
    _barycentricinterpolant_clear(p);
    _polynomialfitreport_clear(rep);
    ae_vector_init(&w, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xc, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&yc, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&dc, 0, DT_INT, _state, ae_true);

    ae_assert(n>0, "PolynomialFit: N<=0!", _state);
    ae_assert(m>0, "PolynomialFit: M<=0!", _state);
    ae_assert(x->cnt>=n, "PolynomialFit: Length(X)<N!", _state);
    ae_assert(y->cnt>=n, "PolynomialFit: Length(Y)<N!", _state);
    ae_assert(isfinitevector(x, n, _state), "PolynomialFit: X contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(y, n, _state), "PolynomialFit: Y contains infinite or NaN values!", _state);
    ae_vector_set_length(&w, n, _state);
    for(i=0; i<=n-1; i++)
    {
        w.ptr.p_double[i] = (double)(1);
    }
    polynomialfitwc(x, y, &w, n, &xc, &yc, &dc, 0, m, p, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Weighted  fitting by polynomials in barycentric form, with constraints  on
function values or first derivatives.

Small regularizing term is used when solving constrained tasks (to improve
stability).

Task is linear, so linear least squares solver is used. Complexity of this
computational scheme is O(N*M^2), mostly dominated by least squares solver

SEE ALSO:
    PolynomialFit()

NOTES:
    you can convert P from barycentric form  to  the  power  or  Chebyshev
    basis with PolynomialBar2Pow() or PolynomialBar2Cheb() functions  from
    the POLINT subpackage.

INPUT PARAMETERS:
    X   -   points, array[0..N-1].
    Y   -   function values, array[0..N-1].
    W   -   weights, array[0..N-1]
            Each summand in square  sum  of  approximation deviations from
            given  values  is  multiplied  by  the square of corresponding
            weight. Fill it by 1's if you don't  want  to  solve  weighted
            task.
    N   -   number of points, N>0.
            * if given, only leading N elements of X/Y/W are used
            * if not given, automatically determined from sizes of X/Y/W
    XC  -   points where polynomial values/derivatives are constrained,
            array[0..K-1].
    YC  -   values of constraints, array[0..K-1]
    DC  -   array[0..K-1], types of constraints:
            * DC[i]=0   means that P(XC[i])=YC[i]
            * DC[i]=1   means that P'(XC[i])=YC[i]
            SEE BELOW FOR IMPORTANT INFORMATION ON CONSTRAINTS
    K   -   number of constraints, 0<=K<M.
            K=0 means no constraints (XC/YC/DC are not used in such cases)
    M   -   number of basis functions (= polynomial_degree + 1), M>=1

OUTPUT PARAMETERS:
    P   -   interpolant in barycentric form for Rep.TerminationType>0.
            undefined for Rep.TerminationType<0.
    Rep -   fitting report. The following fields are set:
                * Rep.TerminationType is a completion code:
                  * set to  1 on success
                  * set to -3 on failure due to  problematic  constraints:
                    either too many  constraints,  degenerate  constraints
                    or inconsistent constraints were passed
                * RMSError      rms error on the (X,Y).
                * AvgError      average error on the (X,Y).
                * AvgRelError   average relative error on the non-zero Y
                * MaxError      maximum error
                                NON-WEIGHTED ERRORS ARE CALCULATED

IMPORTANT:
    this subroitine doesn't calculate task's condition number for K<>0.

SETTING CONSTRAINTS - DANGERS AND OPPORTUNITIES:

Setting constraints can lead  to undesired  results,  like ill-conditioned
behavior, or inconsistency being detected. From the other side,  it allows
us to improve quality of the fit. Here we summarize  our  experience  with
constrained regression splines:
* even simple constraints can be inconsistent, see  Wikipedia  article  on
  this subject: http://en.wikipedia.org/wiki/Birkhoff_interpolation
* the  greater  is  M (given  fixed  constraints),  the  more chances that
  constraints will be consistent
* in the general case, consistency of constraints is NOT GUARANTEED.
* in the one special cases, however, we can  guarantee  consistency.  This
  case  is:  M>1  and constraints on the function values (NOT DERIVATIVES)

Our final recommendation is to use constraints  WHEN  AND  ONLY  when  you
can't solve your task without them. Anything beyond  special  cases  given
above is not guaranteed and may result in inconsistency.

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

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

  -- ALGLIB PROJECT --
     Copyright 10.12.2009 by Bochkanov Sergey
*************************************************************************/
void polynomialfitwc(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     /* Real    */ const ae_vector* _w,
     ae_int_t n,
     /* Real    */ const ae_vector* _xc,
     /* Real    */ const ae_vector* _yc,
     /* Integer */ const ae_vector* dc,
     ae_int_t k,
     ae_int_t m,
     barycentricinterpolant* p,
     polynomialfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector x;
    ae_vector y;
    ae_vector w;
    ae_vector xc;
    ae_vector yc;
    double xa;
    double xb;
    double sa;
    double sb;
    ae_vector xoriginal;
    ae_vector yoriginal;
    ae_vector y2;
    ae_vector w2;
    ae_vector tmp;
    ae_vector tmp2;
    ae_vector bx;
    ae_vector by;
    ae_vector bw;
    ae_int_t i;
    ae_int_t j;
    double u;
    double v;
    double s;
    ae_int_t relcnt;
    lsfitreport lrep;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&y, 0, sizeof(y));
    memset(&w, 0, sizeof(w));
    memset(&xc, 0, sizeof(xc));
    memset(&yc, 0, sizeof(yc));
    memset(&xoriginal, 0, sizeof(xoriginal));
    memset(&yoriginal, 0, sizeof(yoriginal));
    memset(&y2, 0, sizeof(y2));
    memset(&w2, 0, sizeof(w2));
    memset(&tmp, 0, sizeof(tmp));
    memset(&tmp2, 0, sizeof(tmp2));
    memset(&bx, 0, sizeof(bx));
    memset(&by, 0, sizeof(by));
    memset(&bw, 0, sizeof(bw));
    memset(&lrep, 0, sizeof(lrep));
    ae_vector_init_copy(&x, _x, _state, ae_true);
    ae_vector_init_copy(&y, _y, _state, ae_true);
    ae_vector_init_copy(&w, _w, _state, ae_true);
    ae_vector_init_copy(&xc, _xc, _state, ae_true);
    ae_vector_init_copy(&yc, _yc, _state, ae_true);
    _barycentricinterpolant_clear(p);
    _polynomialfitreport_clear(rep);
    ae_vector_init(&xoriginal, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&yoriginal, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&y2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&w2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bx, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&by, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bw, 0, DT_REAL, _state, ae_true);
    _lsfitreport_init(&lrep, _state, ae_true);

    ae_assert(n>0, "PolynomialFitWC: N<=0!", _state);
    ae_assert(m>0, "PolynomialFitWC: M<=0!", _state);
    ae_assert(k>=0, "PolynomialFitWC: K<0!", _state);
    ae_assert(k<m, "PolynomialFitWC: K>=M!", _state);
    ae_assert(x.cnt>=n, "PolynomialFitWC: Length(X)<N!", _state);
    ae_assert(y.cnt>=n, "PolynomialFitWC: Length(Y)<N!", _state);
    ae_assert(w.cnt>=n, "PolynomialFitWC: Length(W)<N!", _state);
    ae_assert(xc.cnt>=k, "PolynomialFitWC: Length(XC)<K!", _state);
    ae_assert(yc.cnt>=k, "PolynomialFitWC: Length(YC)<K!", _state);
    ae_assert(dc->cnt>=k, "PolynomialFitWC: Length(DC)<K!", _state);
    ae_assert(isfinitevector(&x, n, _state), "PolynomialFitWC: X contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(&y, n, _state), "PolynomialFitWC: Y contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(&w, n, _state), "PolynomialFitWC: X contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(&xc, k, _state), "PolynomialFitWC: XC contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(&yc, k, _state), "PolynomialFitWC: YC contains infinite or NaN values!", _state);
    for(i=0; i<=k-1; i++)
    {
        ae_assert(dc->ptr.p_int[i]==0||dc->ptr.p_int[i]==1, "PolynomialFitWC: one of DC[] is not 0 or 1!", _state);
    }
    
    /*
     * Scale X, Y, XC, YC.
     * Solve scaled problem using internal Chebyshev fitting function.
     */
    lsfitscalexy(&x, &y, &w, n, &xc, &yc, dc, k, &xa, &xb, &sa, &sb, &xoriginal, &yoriginal, _state);
    lsfit_internalchebyshevfit(&x, &y, &w, n, &xc, &yc, dc, k, m, &tmp, &lrep, _state);
    rep->terminationtype = lrep.terminationtype;
    if( rep->terminationtype<=0 )
    {
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Generate barycentric model and scale it
     * * BX, BY store barycentric model nodes
     * * FMatrix is reused (remember - it is at least MxM, what we need)
     *
     * Model intialization is done in O(M^2). In principle, it can be
     * done in O(M*log(M)), but before it we solved task with O(N*M^2)
     * complexity, so it is only a small amount of total time spent.
     */
    ae_vector_set_length(&bx, m, _state);
    ae_vector_set_length(&by, m, _state);
    ae_vector_set_length(&bw, m, _state);
    ae_vector_set_length(&tmp2, m, _state);
    s = (double)(1);
    for(i=0; i<=m-1; i++)
    {
        if( m!=1 )
        {
            u = ae_cos(ae_pi*(double)i/(double)(m-1), _state);
        }
        else
        {
            u = (double)(0);
        }
        v = (double)(0);
        for(j=0; j<=m-1; j++)
        {
            if( j==0 )
            {
                tmp2.ptr.p_double[j] = (double)(1);
            }
            else
            {
                if( j==1 )
                {
                    tmp2.ptr.p_double[j] = u;
                }
                else
                {
                    tmp2.ptr.p_double[j] = (double)2*u*tmp2.ptr.p_double[j-1]-tmp2.ptr.p_double[j-2];
                }
            }
            v = v+tmp.ptr.p_double[j]*tmp2.ptr.p_double[j];
        }
        bx.ptr.p_double[i] = u;
        by.ptr.p_double[i] = v;
        bw.ptr.p_double[i] = s;
        if( i==0||i==m-1 )
        {
            bw.ptr.p_double[i] = 0.5*bw.ptr.p_double[i];
        }
        s = -s;
    }
    barycentricbuildxyw(&bx, &by, &bw, m, p, _state);
    barycentriclintransx(p, (double)2/(xb-xa), -(xa+xb)/(xb-xa), _state);
    barycentriclintransy(p, sb-sa, sa, _state);
    
    /*
     * Scale absolute errors obtained from LSFitLinearW.
     * Relative error should be calculated separately
     * (because of shifting/scaling of the task)
     */
    rep->taskrcond = lrep.taskrcond;
    rep->rmserror = lrep.rmserror*(sb-sa);
    rep->avgerror = lrep.avgerror*(sb-sa);
    rep->maxerror = lrep.maxerror*(sb-sa);
    rep->avgrelerror = (double)(0);
    relcnt = 0;
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_neq(yoriginal.ptr.p_double[i],(double)(0)) )
        {
            rep->avgrelerror = rep->avgrelerror+ae_fabs(barycentriccalc(p, xoriginal.ptr.p_double[i], _state)-yoriginal.ptr.p_double[i], _state)/ae_fabs(yoriginal.ptr.p_double[i], _state);
            relcnt = relcnt+1;
        }
    }
    if( relcnt!=0 )
    {
        rep->avgrelerror = rep->avgrelerror/(double)relcnt;
    }
    ae_frame_leave(_state);
}


/*************************************************************************
This function calculates value of four-parameter logistic (4PL)  model  at
specified point X. 4PL model has following form:

    F(x|A,B,C,D) = D+(A-D)/(1+Power(x/C,B))

INPUT PARAMETERS:
    X       -   current point, X>=0:
                * zero X is correctly handled even for B<=0
                * negative X results in exception.
    A, B, C, D- parameters of 4PL model:
                * A is unconstrained
                * B is unconstrained; zero or negative values are handled
                  correctly.
                * C>0, non-positive value results in exception
                * D is unconstrained
                
RESULT:
    model value at X

NOTE: if B=0, denominator is assumed to be equal to 2.0 even  for  zero  X
      (strictly speaking, 0^0 is undefined).

NOTE: this function also throws exception  if  all  input  parameters  are
      correct, but overflow was detected during calculations.
      
NOTE: this function performs a lot of checks;  if  you  need  really  high
      performance, consider evaluating model  yourself,  without  checking
      for degenerate cases.
      
    
  -- ALGLIB PROJECT --
     Copyright 14.05.2014 by Bochkanov Sergey
*************************************************************************/
double logisticcalc4(double x,
     double a,
     double b,
     double c,
     double d,
     ae_state *_state)
{
    double result;


    ae_assert(ae_isfinite(x, _state), "LogisticCalc4: X is not finite", _state);
    ae_assert(ae_isfinite(a, _state), "LogisticCalc4: A is not finite", _state);
    ae_assert(ae_isfinite(b, _state), "LogisticCalc4: B is not finite", _state);
    ae_assert(ae_isfinite(c, _state), "LogisticCalc4: C is not finite", _state);
    ae_assert(ae_isfinite(d, _state), "LogisticCalc4: D is not finite", _state);
    ae_assert(ae_fp_greater_eq(x,(double)(0)), "LogisticCalc4: X is negative", _state);
    ae_assert(ae_fp_greater(c,(double)(0)), "LogisticCalc4: C is non-positive", _state);
    
    /*
     * Check for degenerate cases
     */
    if( ae_fp_eq(b,(double)(0)) )
    {
        result = 0.5*(a+d);
        return result;
    }
    if( ae_fp_eq(x,(double)(0)) )
    {
        if( ae_fp_greater(b,(double)(0)) )
        {
            result = a;
        }
        else
        {
            result = d;
        }
        return result;
    }
    
    /*
     * General case
     */
    result = d+(a-d)/(1.0+ae_pow(x/c, b, _state));
    ae_assert(ae_isfinite(result, _state), "LogisticCalc4: overflow during calculations", _state);
    return result;
}


/*************************************************************************
This function calculates value of five-parameter logistic (5PL)  model  at
specified point X. 5PL model has following form:

    F(x|A,B,C,D,G) = D+(A-D)/Power(1+Power(x/C,B),G)

INPUT PARAMETERS:
    X       -   current point, X>=0:
                * zero X is correctly handled even for B<=0
                * negative X results in exception.
    A, B, C, D, G- parameters of 5PL model:
                * A is unconstrained
                * B is unconstrained; zero or negative values are handled
                  correctly.
                * C>0, non-positive value results in exception
                * D is unconstrained
                * G>0, non-positive value results in exception
                
RESULT:
    model value at X

NOTE: if B=0, denominator is assumed to be equal to Power(2.0,G) even  for
      zero X (strictly speaking, 0^0 is undefined).

NOTE: this function also throws exception  if  all  input  parameters  are
      correct, but overflow was detected during calculations.
      
NOTE: this function performs a lot of checks;  if  you  need  really  high
      performance, consider evaluating model  yourself,  without  checking
      for degenerate cases.
      
    
  -- ALGLIB PROJECT --
     Copyright 14.05.2014 by Bochkanov Sergey
*************************************************************************/
double logisticcalc5(double x,
     double a,
     double b,
     double c,
     double d,
     double g,
     ae_state *_state)
{
    double result;


    ae_assert(ae_isfinite(x, _state), "LogisticCalc5: X is not finite", _state);
    ae_assert(ae_isfinite(a, _state), "LogisticCalc5: A is not finite", _state);
    ae_assert(ae_isfinite(b, _state), "LogisticCalc5: B is not finite", _state);
    ae_assert(ae_isfinite(c, _state), "LogisticCalc5: C is not finite", _state);
    ae_assert(ae_isfinite(d, _state), "LogisticCalc5: D is not finite", _state);
    ae_assert(ae_isfinite(g, _state), "LogisticCalc5: G is not finite", _state);
    ae_assert(ae_fp_greater_eq(x,(double)(0)), "LogisticCalc5: X is negative", _state);
    ae_assert(ae_fp_greater(c,(double)(0)), "LogisticCalc5: C is non-positive", _state);
    ae_assert(ae_fp_greater(g,(double)(0)), "LogisticCalc5: G is non-positive", _state);
    
    /*
     * Check for degenerate cases
     */
    if( ae_fp_eq(b,(double)(0)) )
    {
        result = d+(a-d)/ae_pow(2.0, g, _state);
        return result;
    }
    if( ae_fp_eq(x,(double)(0)) )
    {
        if( ae_fp_greater(b,(double)(0)) )
        {
            result = a;
        }
        else
        {
            result = d;
        }
        return result;
    }
    
    /*
     * General case
     */
    result = d+(a-d)/ae_pow(1.0+ae_pow(x/c, b, _state), g, _state);
    ae_assert(ae_isfinite(result, _state), "LogisticCalc5: overflow during calculations", _state);
    return result;
}


/*************************************************************************
This function fits four-parameter logistic (4PL) model  to  data  provided
by user. 4PL model has following form:

    F(x|A,B,C,D) = D+(A-D)/(1+Power(x/C,B))

Here:
    * A, D - unconstrained (see LogisticFit4EC() for constrained 4PL)
    * B>=0
    * C>0
    
IMPORTANT: output of this function is constrained in  such  way that  B>0.
           Because 4PL model is symmetric with respect to B, there  is  no
           need to explore  B<0.  Constraining  B  makes  algorithm easier
           to stabilize and debug.
           Users  who  for  some  reason  prefer to work with negative B's
           should transform output themselves (swap A and D, replace B  by
           -B).
           
4PL fitting is implemented as follows:
* we perform small number of restarts from random locations which helps to
  solve problem of bad local extrema. Locations are only partially  random
  - we use input data to determine good  initial  guess,  but  we  include
  controlled amount of randomness.
* we perform Levenberg-Marquardt fitting with very  tight  constraints  on
  parameters B and C - it allows us to find good  initial  guess  for  the
  second stage without risk of running into "flat spot".
* second  Levenberg-Marquardt  round  is   performed   without   excessive
  constraints. Results from the previous round are used as initial guess.
* after fitting is done, we compare results with best values found so far,
  rewrite "best solution" if needed, and move to next random location.
  
Overall algorithm is very stable and is not prone to  bad  local  extrema.
Furthermore, it automatically scales when input data have  very  large  or
very small range.

INPUT PARAMETERS:
    X       -   array[N], stores X-values.
                MUST include only non-negative numbers  (but  may  include
                zero values). Can be unsorted.
    Y       -   array[N], values to fit.
    N       -   number of points. If N is less than  length  of  X/Y, only
                leading N elements are used.
                
OUTPUT PARAMETERS:
    A, B, C, D- parameters of 4PL model
    Rep     -   fitting report. This structure has many fields,  but  ONLY
                ONES LISTED BELOW ARE SET:
                * Rep.IterationsCount - number of iterations performed
                * Rep.RMSError - root-mean-square error
                * Rep.AvgError - average absolute error
                * Rep.AvgRelError - average relative error (calculated for
                  non-zero Y-values)
                * Rep.MaxError - maximum absolute error
                * Rep.R2 - coefficient of determination,  R-squared.  This
                  coefficient   is  calculated  as  R2=1-RSS/TSS  (in case
                  of nonlinear  regression  there  are  multiple  ways  to
                  define R2, each of them giving different results).

NOTE: for stability reasons the B parameter is restricted by [1/1000,1000]
      range. It prevents  algorithm from making trial steps  deep into the
      area of bad parameters.

NOTE: after  you  obtained  coefficients,  you  can  evaluate  model  with
      LogisticCalc4() function.

NOTE: if you need better control over fitting process than provided by this
      function, you may use LogisticFit45X().
                
NOTE: step is automatically scaled according to scale of parameters  being
      fitted before we compare its length with EpsX. Thus,  this  function
      can be used to fit data with very small or very large values without
      changing EpsX.
    

  -- ALGLIB PROJECT --
     Copyright 14.02.2014 by Bochkanov Sergey
*************************************************************************/
void logisticfit4(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     ae_int_t n,
     double* a,
     double* b,
     double* c,
     double* d,
     lsfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector x;
    ae_vector y;
    double g;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&y, 0, sizeof(y));
    ae_vector_init_copy(&x, _x, _state, ae_true);
    ae_vector_init_copy(&y, _y, _state, ae_true);
    *a = 0.0;
    *b = 0.0;
    *c = 0.0;
    *d = 0.0;
    _lsfitreport_clear(rep);

    logisticfit45x(&x, &y, n, _state->v_nan, _state->v_nan, ae_true, 0.0, 0.0, 0, a, b, c, d, &g, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function fits four-parameter logistic (4PL) model  to  data  provided
by user, with optional constraints on parameters A and D.  4PL  model  has
following form:

    F(x|A,B,C,D) = D+(A-D)/(1+Power(x/C,B))

Here:
    * A, D - with optional equality constraints
    * B>=0
    * C>0
    
IMPORTANT: output of this function is constrained in  such  way that  B>0.
           Because 4PL model is symmetric with respect to B, there  is  no
           need to explore  B<0.  Constraining  B  makes  algorithm easier
           to stabilize and debug.
           Users  who  for  some  reason  prefer to work with negative B's
           should transform output themselves (swap A and D, replace B  by
           -B).
           
4PL fitting is implemented as follows:
* we perform small number of restarts from random locations which helps to
  solve problem of bad local extrema. Locations are only partially  random
  - we use input data to determine good  initial  guess,  but  we  include
  controlled amount of randomness.
* we perform Levenberg-Marquardt fitting with very  tight  constraints  on
  parameters B and C - it allows us to find good  initial  guess  for  the
  second stage without risk of running into "flat spot".
* second  Levenberg-Marquardt  round  is   performed   without   excessive
  constraints. Results from the previous round are used as initial guess.
* after fitting is done, we compare results with best values found so far,
  rewrite "best solution" if needed, and move to next random location.
  
Overall algorithm is very stable and is not prone to  bad  local  extrema.
Furthermore, it automatically scales when input data have  very  large  or
very small range.

INPUT PARAMETERS:
    X       -   array[N], stores X-values.
                MUST include only non-negative numbers  (but  may  include
                zero values). Can be unsorted.
    Y       -   array[N], values to fit.
    N       -   number of points. If N is less than  length  of  X/Y, only
                leading N elements are used.
    CnstrLeft-  optional equality constraint for model value at the   left
                boundary (at X=0). Specify NAN (Not-a-Number)  if  you  do
                not need constraint on the model value at X=0 (in C++  you
                can pass alglib::fp_nan as parameter, in  C#  it  will  be
                Double.NaN).
                See  below,  section  "EQUALITY  CONSTRAINTS"   for   more
                information about constraints.
    CnstrRight- optional equality constraint for model value at X=infinity.
                Specify NAN (Not-a-Number) if you do not  need  constraint
                on the model value (in C++  you can pass alglib::fp_nan as
                parameter, in  C# it will  be Double.NaN).
                See  below,  section  "EQUALITY  CONSTRAINTS"   for   more
                information about constraints.
                
OUTPUT PARAMETERS:
    A, B, C, D- parameters of 4PL model
    Rep     -   fitting report. This structure has many fields,  but  ONLY
                ONES LISTED BELOW ARE SET:
                * Rep.IterationsCount - number of iterations performed
                * Rep.RMSError - root-mean-square error
                * Rep.AvgError - average absolute error
                * Rep.AvgRelError - average relative error (calculated for
                  non-zero Y-values)
                * Rep.MaxError - maximum absolute error
                * Rep.R2 - coefficient of determination,  R-squared.  This
                  coefficient   is  calculated  as  R2=1-RSS/TSS  (in case
                  of nonlinear  regression  there  are  multiple  ways  to
                  define R2, each of them giving different results).

NOTE: for stability reasons the B parameter is restricted by [1/1000,1000]
      range. It prevents  algorithm from making trial steps  deep into the
      area of bad parameters.

NOTE: after  you  obtained  coefficients,  you  can  evaluate  model  with
      LogisticCalc4() function.

NOTE: if you need better control over fitting process than provided by this
      function, you may use LogisticFit45X().
                
NOTE: step is automatically scaled according to scale of parameters  being
      fitted before we compare its length with EpsX. Thus,  this  function
      can be used to fit data with very small or very large values without
      changing EpsX.

EQUALITY CONSTRAINTS ON PARAMETERS

4PL/5PL solver supports equality constraints on model values at  the  left
boundary (X=0) and right  boundary  (X=infinity).  These  constraints  are
completely optional and you can specify both of them, only  one  -  or  no
constraints at all.

Parameter  CnstrLeft  contains  left  constraint (or NAN for unconstrained
fitting), and CnstrRight contains right  one.  For  4PL,  left  constraint
ALWAYS corresponds to parameter A, and right one is ALWAYS  constraint  on
D. That's because 4PL model is normalized in such way that B>=0.
    

  -- ALGLIB PROJECT --
     Copyright 14.02.2014 by Bochkanov Sergey
*************************************************************************/
void logisticfit4ec(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     ae_int_t n,
     double cnstrleft,
     double cnstrright,
     double* a,
     double* b,
     double* c,
     double* d,
     lsfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector x;
    ae_vector y;
    double g;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&y, 0, sizeof(y));
    ae_vector_init_copy(&x, _x, _state, ae_true);
    ae_vector_init_copy(&y, _y, _state, ae_true);
    *a = 0.0;
    *b = 0.0;
    *c = 0.0;
    *d = 0.0;
    _lsfitreport_clear(rep);

    logisticfit45x(&x, &y, n, cnstrleft, cnstrright, ae_true, 0.0, 0.0, 0, a, b, c, d, &g, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function fits five-parameter logistic (5PL) model  to  data  provided
by user. 5PL model has following form:

    F(x|A,B,C,D,G) = D+(A-D)/Power(1+Power(x/C,B),G)

Here:
    * A, D - unconstrained
    * B - unconstrained
    * C>0
    * G>0
    
IMPORTANT: unlike in  4PL  fitting,  output  of  this  function   is   NOT
           constrained in  such  way that B is guaranteed to be  positive.
           Furthermore,  unlike  4PL,  5PL  model  is  NOT  symmetric with
           respect to B, so you can NOT transform model to equivalent one,
           with B having desired sign (>0 or <0).
    
5PL fitting is implemented as follows:
* we perform small number of restarts from random locations which helps to
  solve problem of bad local extrema. Locations are only partially  random
  - we use input data to determine good  initial  guess,  but  we  include
  controlled amount of randomness.
* we perform Levenberg-Marquardt fitting with very  tight  constraints  on
  parameters B and C - it allows us to find good  initial  guess  for  the
  second stage without risk of running into "flat spot".  Parameter  G  is
  fixed at G=1.
* second  Levenberg-Marquardt  round  is   performed   without   excessive
  constraints on B and C, but with G still equal to 1.  Results  from  the
  previous round are used as initial guess.
* third Levenberg-Marquardt round relaxes constraints on G  and  tries  two
  different models - one with B>0 and one with B<0.
* after fitting is done, we compare results with best values found so far,
  rewrite "best solution" if needed, and move to next random location.
  
Overall algorithm is very stable and is not prone to  bad  local  extrema.
Furthermore, it automatically scales when input data have  very  large  or
very small range.

INPUT PARAMETERS:
    X       -   array[N], stores X-values.
                MUST include only non-negative numbers  (but  may  include
                zero values). Can be unsorted.
    Y       -   array[N], values to fit.
    N       -   number of points. If N is less than  length  of  X/Y, only
                leading N elements are used.
                
OUTPUT PARAMETERS:
    A,B,C,D,G-  parameters of 5PL model
    Rep     -   fitting report. This structure has many fields,  but  ONLY
                ONES LISTED BELOW ARE SET:
                * Rep.IterationsCount - number of iterations performed
                * Rep.RMSError - root-mean-square error
                * Rep.AvgError - average absolute error
                * Rep.AvgRelError - average relative error (calculated for
                  non-zero Y-values)
                * Rep.MaxError - maximum absolute error
                * Rep.R2 - coefficient of determination,  R-squared.  This
                  coefficient   is  calculated  as  R2=1-RSS/TSS  (in case
                  of nonlinear  regression  there  are  multiple  ways  to
                  define R2, each of them giving different results).

NOTE: for better stability B  parameter is restricted by [+-1/1000,+-1000]
      range, and G is restricted by [1/10,10] range. It prevents algorithm
      from making trial steps deep into the area of bad parameters.

NOTE: after  you  obtained  coefficients,  you  can  evaluate  model  with
      LogisticCalc5() function.

NOTE: if you need better control over fitting process than provided by this
      function, you may use LogisticFit45X().
                
NOTE: step is automatically scaled according to scale of parameters  being
      fitted before we compare its length with EpsX. Thus,  this  function
      can be used to fit data with very small or very large values without
      changing EpsX.
    

  -- ALGLIB PROJECT --
     Copyright 14.02.2014 by Bochkanov Sergey
*************************************************************************/
void logisticfit5(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     ae_int_t n,
     double* a,
     double* b,
     double* c,
     double* d,
     double* g,
     lsfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector x;
    ae_vector y;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&y, 0, sizeof(y));
    ae_vector_init_copy(&x, _x, _state, ae_true);
    ae_vector_init_copy(&y, _y, _state, ae_true);
    *a = 0.0;
    *b = 0.0;
    *c = 0.0;
    *d = 0.0;
    *g = 0.0;
    _lsfitreport_clear(rep);

    logisticfit45x(&x, &y, n, _state->v_nan, _state->v_nan, ae_false, 0.0, 0.0, 0, a, b, c, d, g, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function fits five-parameter logistic (5PL) model  to  data  provided
by user, subject to optional equality constraints on parameters A  and  D.
5PL model has following form:

    F(x|A,B,C,D,G) = D+(A-D)/Power(1+Power(x/C,B),G)

Here:
    * A, D - with optional equality constraints
    * B - unconstrained
    * C>0
    * G>0
    
IMPORTANT: unlike in  4PL  fitting,  output  of  this  function   is   NOT
           constrained in  such  way that B is guaranteed to be  positive.
           Furthermore,  unlike  4PL,  5PL  model  is  NOT  symmetric with
           respect to B, so you can NOT transform model to equivalent one,
           with B having desired sign (>0 or <0).
    
5PL fitting is implemented as follows:
* we perform small number of restarts from random locations which helps to
  solve problem of bad local extrema. Locations are only partially  random
  - we use input data to determine good  initial  guess,  but  we  include
  controlled amount of randomness.
* we perform Levenberg-Marquardt fitting with very  tight  constraints  on
  parameters B and C - it allows us to find good  initial  guess  for  the
  second stage without risk of running into "flat spot".  Parameter  G  is
  fixed at G=1.
* second  Levenberg-Marquardt  round  is   performed   without   excessive
  constraints on B and C, but with G still equal to 1.  Results  from  the
  previous round are used as initial guess.
* third Levenberg-Marquardt round relaxes constraints on G  and  tries  two
  different models - one with B>0 and one with B<0.
* after fitting is done, we compare results with best values found so far,
  rewrite "best solution" if needed, and move to next random location.
  
Overall algorithm is very stable and is not prone to  bad  local  extrema.
Furthermore, it automatically scales when input data have  very  large  or
very small range.

INPUT PARAMETERS:
    X       -   array[N], stores X-values.
                MUST include only non-negative numbers  (but  may  include
                zero values). Can be unsorted.
    Y       -   array[N], values to fit.
    N       -   number of points. If N is less than  length  of  X/Y, only
                leading N elements are used.
    CnstrLeft-  optional equality constraint for model value at the   left
                boundary (at X=0). Specify NAN (Not-a-Number)  if  you  do
                not need constraint on the model value at X=0 (in C++  you
                can pass alglib::fp_nan as parameter, in  C#  it  will  be
                Double.NaN).
                See  below,  section  "EQUALITY  CONSTRAINTS"   for   more
                information about constraints.
    CnstrRight- optional equality constraint for model value at X=infinity.
                Specify NAN (Not-a-Number) if you do not  need  constraint
                on the model value (in C++  you can pass alglib::fp_nan as
                parameter, in  C# it will  be Double.NaN).
                See  below,  section  "EQUALITY  CONSTRAINTS"   for   more
                information about constraints.
                
OUTPUT PARAMETERS:
    A,B,C,D,G-  parameters of 5PL model
    Rep     -   fitting report. This structure has many fields,  but  ONLY
                ONES LISTED BELOW ARE SET:
                * Rep.IterationsCount - number of iterations performed
                * Rep.RMSError - root-mean-square error
                * Rep.AvgError - average absolute error
                * Rep.AvgRelError - average relative error (calculated for
                  non-zero Y-values)
                * Rep.MaxError - maximum absolute error
                * Rep.R2 - coefficient of determination,  R-squared.  This
                  coefficient   is  calculated  as  R2=1-RSS/TSS  (in case
                  of nonlinear  regression  there  are  multiple  ways  to
                  define R2, each of them giving different results).

NOTE: for better stability B  parameter is restricted by [+-1/1000,+-1000]
      range, and G is restricted by [1/10,10] range. It prevents algorithm
      from making trial steps deep into the area of bad parameters.

NOTE: after  you  obtained  coefficients,  you  can  evaluate  model  with
      LogisticCalc5() function.

NOTE: if you need better control over fitting process than provided by this
      function, you may use LogisticFit45X().
                
NOTE: step is automatically scaled according to scale of parameters  being
      fitted before we compare its length with EpsX. Thus,  this  function
      can be used to fit data with very small or very large values without
      changing EpsX.

EQUALITY CONSTRAINTS ON PARAMETERS

5PL solver supports equality constraints on model  values  at   the   left
boundary (X=0) and right  boundary  (X=infinity).  These  constraints  are
completely optional and you can specify both of them, only  one  -  or  no
constraints at all.

Parameter  CnstrLeft  contains  left  constraint (or NAN for unconstrained
fitting), and CnstrRight contains right  one.

Unlike 4PL one, 5PL model is NOT symmetric with respect to  change in sign
of B. Thus, negative B's are possible, and left constraint  may  constrain
parameter A (for positive B's)  -  or  parameter  D  (for  negative  B's).
Similarly changes meaning of right constraint.

You do not have to decide what parameter to  constrain  -  algorithm  will
automatically determine correct parameters as fitting progresses. However,
question highlighted above is important when you interpret fitting results.
    

  -- ALGLIB PROJECT --
     Copyright 14.02.2014 by Bochkanov Sergey
*************************************************************************/
void logisticfit5ec(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     ae_int_t n,
     double cnstrleft,
     double cnstrright,
     double* a,
     double* b,
     double* c,
     double* d,
     double* g,
     lsfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector x;
    ae_vector y;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&y, 0, sizeof(y));
    ae_vector_init_copy(&x, _x, _state, ae_true);
    ae_vector_init_copy(&y, _y, _state, ae_true);
    *a = 0.0;
    *b = 0.0;
    *c = 0.0;
    *d = 0.0;
    *g = 0.0;
    _lsfitreport_clear(rep);

    logisticfit45x(&x, &y, n, cnstrleft, cnstrright, ae_false, 0.0, 0.0, 0, a, b, c, d, g, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This is "expert" 4PL/5PL fitting function, which can be used if  you  need
better control over fitting process than provided  by  LogisticFit4()  or
LogisticFit5().

This function fits model of the form

    F(x|A,B,C,D)   = D+(A-D)/(1+Power(x/C,B))           (4PL model)

or

    F(x|A,B,C,D,G) = D+(A-D)/Power(1+Power(x/C,B),G)    (5PL model)
    
Here:
    * A, D - unconstrained
    * B>=0 for 4PL, unconstrained for 5PL
    * C>0
    * G>0 (if present)

INPUT PARAMETERS:
    X       -   array[N], stores X-values.
                MUST include only non-negative numbers  (but  may  include
                zero values). Can be unsorted.
    Y       -   array[N], values to fit.
    N       -   number of points. If N is less than  length  of  X/Y, only
                leading N elements are used.
    CnstrLeft-  optional equality constraint for model value at the   left
                boundary (at X=0). Specify NAN (Not-a-Number)  if  you  do
                not need constraint on the model value at X=0 (in C++  you
                can pass alglib::fp_nan as parameter, in  C#  it  will  be
                Double.NaN).
                See  below,  section  "EQUALITY  CONSTRAINTS"   for   more
                information about constraints.
    CnstrRight- optional equality constraint for model value at X=infinity.
                Specify NAN (Not-a-Number) if you do not  need  constraint
                on the model value (in C++  you can pass alglib::fp_nan as
                parameter, in  C# it will  be Double.NaN).
                See  below,  section  "EQUALITY  CONSTRAINTS"   for   more
                information about constraints.
    Is4PL   -   whether 4PL or 5PL models are fitted
    LambdaV -   regularization coefficient, LambdaV>=0.
                Set it to zero unless you know what you are doing.
    EpsX    -   stopping condition (step size), EpsX>=0.
                Zero value means that small step is automatically chosen.
                See notes below for more information.
    RsCnt   -   number of repeated restarts from  random  points.  4PL/5PL
                models are prone to problem of bad local extrema. Utilizing
                multiple random restarts allows  us  to  improve algorithm
                convergence.
                RsCnt>=0.
                Zero value means that function automatically choose  small
                amount of restarts (recommended).
                
OUTPUT PARAMETERS:
    A, B, C, D- parameters of 4PL model
    G       -   parameter of 5PL model; for Is4PL=True, G=1 is returned.
    Rep     -   fitting report. This structure has many fields,  but  ONLY
                ONES LISTED BELOW ARE SET:
                * Rep.IterationsCount - number of iterations performed
                * Rep.RMSError - root-mean-square error
                * Rep.AvgError - average absolute error
                * Rep.AvgRelError - average relative error (calculated for
                  non-zero Y-values)
                * Rep.MaxError - maximum absolute error
                * Rep.R2 - coefficient of determination,  R-squared.  This
                  coefficient   is  calculated  as  R2=1-RSS/TSS  (in case
                  of nonlinear  regression  there  are  multiple  ways  to
                  define R2, each of them giving different results).
                
NOTE: for better stability B  parameter is restricted by [+-1/1000,+-1000]
      range, and G is restricted by [1/10,10] range. It prevents algorithm
      from making trial steps deep into the area of bad parameters.

NOTE: after  you  obtained  coefficients,  you  can  evaluate  model  with
      LogisticCalc5() function.

NOTE: step is automatically scaled according to scale of parameters  being
      fitted before we compare its length with EpsX. Thus,  this  function
      can be used to fit data with very small or very large values without
      changing EpsX.

EQUALITY CONSTRAINTS ON PARAMETERS

4PL/5PL solver supports equality constraints on model values at  the  left
boundary (X=0) and right  boundary  (X=infinity).  These  constraints  are
completely optional and you can specify both of them, only  one  -  or  no
constraints at all.

Parameter  CnstrLeft  contains  left  constraint (or NAN for unconstrained
fitting), and CnstrRight contains right  one.  For  4PL,  left  constraint
ALWAYS corresponds to parameter A, and right one is ALWAYS  constraint  on
D. That's because 4PL model is normalized in such way that B>=0.

For 5PL model things are different. Unlike  4PL  one,  5PL  model  is  NOT
symmetric with respect to  change  in  sign  of  B. Thus, negative B's are
possible, and left constraint may constrain parameter A (for positive B's)
- or parameter D (for negative B's). Similarly changes  meaning  of  right
constraint.

You do not have to decide what parameter to  constrain  -  algorithm  will
automatically determine correct parameters as fitting progresses. However,
question highlighted above is important when you interpret fitting results.
    

  -- ALGLIB PROJECT --
     Copyright 14.02.2014 by Bochkanov Sergey
*************************************************************************/
void logisticfit45x(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     ae_int_t n,
     double cnstrleft,
     double cnstrright,
     ae_bool is4pl,
     double lambdav,
     double epsx,
     ae_int_t rscnt,
     double* a,
     double* b,
     double* c,
     double* d,
     double* g,
     lsfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector x;
    ae_vector y;
    ae_int_t i;
    ae_int_t outerit;
    ae_int_t nz;
    double v;
    ae_vector p0;
    ae_vector p1;
    ae_vector p2;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector s;
    ae_vector bndl1;
    ae_vector bndu1;
    ae_vector bndl2;
    ae_vector bndu2;
    ae_matrix z;
    hqrndstate rs;
    minlmstate state;
    minlmreport replm;
    ae_int_t maxits;
    double fbest;
    double flast;
    double scalex;
    double scaley;
    ae_vector bufx;
    ae_vector bufy;
    double fposb;
    double fnegb;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&y, 0, sizeof(y));
    memset(&p0, 0, sizeof(p0));
    memset(&p1, 0, sizeof(p1));
    memset(&p2, 0, sizeof(p2));
    memset(&bndl, 0, sizeof(bndl));
    memset(&bndu, 0, sizeof(bndu));
    memset(&s, 0, sizeof(s));
    memset(&bndl1, 0, sizeof(bndl1));
    memset(&bndu1, 0, sizeof(bndu1));
    memset(&bndl2, 0, sizeof(bndl2));
    memset(&bndu2, 0, sizeof(bndu2));
    memset(&z, 0, sizeof(z));
    memset(&rs, 0, sizeof(rs));
    memset(&state, 0, sizeof(state));
    memset(&replm, 0, sizeof(replm));
    memset(&bufx, 0, sizeof(bufx));
    memset(&bufy, 0, sizeof(bufy));
    ae_vector_init_copy(&x, _x, _state, ae_true);
    ae_vector_init_copy(&y, _y, _state, ae_true);
    *a = 0.0;
    *b = 0.0;
    *c = 0.0;
    *d = 0.0;
    *g = 0.0;
    _lsfitreport_clear(rep);
    ae_vector_init(&p0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&p1, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&p2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bndl, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bndu, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&s, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bndl1, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bndu1, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bndl2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bndu2, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&z, 0, 0, DT_REAL, _state, ae_true);
    _hqrndstate_init(&rs, _state, ae_true);
    _minlmstate_init(&state, _state, ae_true);
    _minlmreport_init(&replm, _state, ae_true);
    ae_vector_init(&bufx, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bufy, 0, DT_REAL, _state, ae_true);

    ae_assert(ae_isfinite(epsx, _state), "LogisticFitX: EpsX is infinite/NAN", _state);
    ae_assert(ae_isfinite(lambdav, _state), "LogisticFitX: LambdaV is infinite/NAN", _state);
    ae_assert(ae_isfinite(cnstrleft, _state)||ae_isnan(cnstrleft, _state), "LogisticFitX: CnstrLeft is NOT finite or NAN", _state);
    ae_assert(ae_isfinite(cnstrright, _state)||ae_isnan(cnstrright, _state), "LogisticFitX: CnstrRight is NOT finite or NAN", _state);
    ae_assert(ae_fp_greater_eq(lambdav,(double)(0)), "LogisticFitX: negative LambdaV", _state);
    ae_assert(n>0, "LogisticFitX: N<=0", _state);
    ae_assert(rscnt>=0, "LogisticFitX: RsCnt<0", _state);
    ae_assert(ae_fp_greater_eq(epsx,(double)(0)), "LogisticFitX: EpsX<0", _state);
    ae_assert(x.cnt>=n, "LogisticFitX: Length(X)<N", _state);
    ae_assert(y.cnt>=n, "LogisticFitX: Length(Y)<N", _state);
    ae_assert(isfinitevector(&x, n, _state), "LogisticFitX: X contains infinite/NAN values", _state);
    ae_assert(isfinitevector(&y, n, _state), "LogisticFitX: X contains infinite/NAN values", _state);
    hqrndseed(2211, 1033044, &rs, _state);
    lsfit_clearreport(rep, _state);
    if( ae_fp_eq(epsx,(double)(0)) )
    {
        epsx = 1.0E-10;
    }
    if( rscnt==0 )
    {
        rscnt = 4;
    }
    maxits = 1000;
    
    /*
     * Sort points by X.
     * Determine number of zero and non-zero values.
     */
    tagsortfastr(&x, &y, &bufx, &bufy, n, _state);
    ae_assert(ae_fp_greater_eq(x.ptr.p_double[0],(double)(0)), "LogisticFitX: some X[] are negative", _state);
    nz = n;
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_greater(x.ptr.p_double[i],(double)(0)) )
        {
            nz = i;
            break;
        }
    }
    
    /*
     * For NZ=N (all X[] are zero) special code is used.
     * For NZ<N we use general-purpose code.
     */
    rep->iterationscount = 0;
    if( nz==n )
    {
        
        /*
         * NZ=N, degenerate problem.
         * No need to run optimizer.
         */
        v = 0.0;
        for(i=0; i<=n-1; i++)
        {
            v = v+y.ptr.p_double[i];
        }
        v = v/(double)n;
        if( ae_isfinite(cnstrleft, _state) )
        {
            *a = cnstrleft;
        }
        else
        {
            *a = v;
        }
        *b = (double)(1);
        *c = (double)(1);
        if( ae_isfinite(cnstrright, _state) )
        {
            *d = cnstrright;
        }
        else
        {
            *d = *a;
        }
        *g = (double)(1);
        lsfit_logisticfit45errors(&x, &y, n, *a, *b, *c, *d, *g, rep, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Non-degenerate problem.
     * Determine scale of data.
     */
    scalex = x.ptr.p_double[nz+(n-nz)/2];
    ae_assert(ae_fp_greater(scalex,(double)(0)), "LogisticFitX: internal error", _state);
    v = 0.0;
    for(i=0; i<=n-1; i++)
    {
        v = v+y.ptr.p_double[i];
    }
    v = v/(double)n;
    scaley = 0.0;
    for(i=0; i<=n-1; i++)
    {
        scaley = scaley+ae_sqr(y.ptr.p_double[i]-v, _state);
    }
    scaley = ae_sqrt(scaley/(double)n, _state);
    if( ae_fp_eq(scaley,(double)(0)) )
    {
        scaley = 1.0;
    }
    ae_vector_set_length(&s, 5, _state);
    s.ptr.p_double[0] = scaley;
    s.ptr.p_double[1] = 0.1;
    s.ptr.p_double[2] = scalex;
    s.ptr.p_double[3] = scaley;
    s.ptr.p_double[4] = 0.1;
    ae_vector_set_length(&p0, 5, _state);
    p0.ptr.p_double[0] = (double)(0);
    p0.ptr.p_double[1] = (double)(0);
    p0.ptr.p_double[2] = (double)(0);
    p0.ptr.p_double[3] = (double)(0);
    p0.ptr.p_double[4] = (double)(0);
    ae_vector_set_length(&bndl, 5, _state);
    ae_vector_set_length(&bndu, 5, _state);
    ae_vector_set_length(&bndl1, 5, _state);
    ae_vector_set_length(&bndu1, 5, _state);
    ae_vector_set_length(&bndl2, 5, _state);
    ae_vector_set_length(&bndu2, 5, _state);
    minlmcreatevj(5, n+5, &p0, &state, _state);
    minlmsetscale(&state, &s, _state);
    minlmsetcond(&state, epsx, maxits, _state);
    minlmsetxrep(&state, ae_true, _state);
    ae_vector_set_length(&p1, 5, _state);
    ae_vector_set_length(&p2, 5, _state);
    
    /*
     * Is it 4PL problem?
     */
    if( is4pl )
    {
        
        /*
         * Run outer iterations
         */
        *a = (double)(0);
        *b = (double)(1);
        *c = (double)(1);
        *d = (double)(1);
        *g = (double)(1);
        fbest = ae_maxrealnumber;
        for(outerit=0; outerit<=rscnt-1; outerit++)
        {
            
            /*
             * Prepare initial point; use B>0
             */
            if( ae_isfinite(cnstrleft, _state) )
            {
                p1.ptr.p_double[0] = cnstrleft;
            }
            else
            {
                p1.ptr.p_double[0] = y.ptr.p_double[0]+0.15*scaley*(hqrnduniformr(&rs, _state)-0.5);
            }
            p1.ptr.p_double[1] = 0.5+hqrnduniformr(&rs, _state);
            p1.ptr.p_double[2] = x.ptr.p_double[nz+hqrnduniformi(&rs, n-nz, _state)];
            if( ae_isfinite(cnstrright, _state) )
            {
                p1.ptr.p_double[3] = cnstrright;
            }
            else
            {
                p1.ptr.p_double[3] = y.ptr.p_double[n-1]+0.25*scaley*(hqrnduniformr(&rs, _state)-0.5);
            }
            p1.ptr.p_double[4] = 1.0;
            
            /*
             * Run optimization with tight constraints and increased regularization
             */
            if( ae_isfinite(cnstrleft, _state) )
            {
                bndl.ptr.p_double[0] = cnstrleft;
                bndu.ptr.p_double[0] = cnstrleft;
            }
            else
            {
                bndl.ptr.p_double[0] = _state->v_neginf;
                bndu.ptr.p_double[0] = _state->v_posinf;
            }
            bndl.ptr.p_double[1] = 0.5;
            bndu.ptr.p_double[1] = 2.0;
            bndl.ptr.p_double[2] = 0.5*scalex;
            bndu.ptr.p_double[2] = 2.0*scalex;
            if( ae_isfinite(cnstrright, _state) )
            {
                bndl.ptr.p_double[3] = cnstrright;
                bndu.ptr.p_double[3] = cnstrright;
            }
            else
            {
                bndl.ptr.p_double[3] = _state->v_neginf;
                bndu.ptr.p_double[3] = _state->v_posinf;
            }
            bndl.ptr.p_double[4] = 1.0;
            bndu.ptr.p_double[4] = 1.0;
            minlmsetbc(&state, &bndl, &bndu, _state);
            lsfit_logisticfitinternal(&x, &y, n, is4pl, (double)100*lambdav, &state, &replm, &p1, &flast, _state);
            rep->iterationscount = rep->iterationscount+replm.iterationscount;
            
            /*
             * Relax constraints, run optimization one more time
             */
            bndl.ptr.p_double[1] = 0.1;
            bndu.ptr.p_double[1] = 10.0;
            bndl.ptr.p_double[2] = ae_machineepsilon*scalex;
            bndu.ptr.p_double[2] = scalex/ae_machineepsilon;
            minlmsetbc(&state, &bndl, &bndu, _state);
            lsfit_logisticfitinternal(&x, &y, n, is4pl, lambdav, &state, &replm, &p1, &flast, _state);
            rep->iterationscount = rep->iterationscount+replm.iterationscount;
            
            /*
             * Relax constraints more, run optimization one more time
             */
            bndl.ptr.p_double[1] = 0.01;
            bndu.ptr.p_double[1] = 100.0;
            minlmsetbc(&state, &bndl, &bndu, _state);
            lsfit_logisticfitinternal(&x, &y, n, is4pl, lambdav, &state, &replm, &p1, &flast, _state);
            rep->iterationscount = rep->iterationscount+replm.iterationscount;
            
            /*
             * Relax constraints ever more, run optimization one more time
             */
            bndl.ptr.p_double[1] = 0.001;
            bndu.ptr.p_double[1] = 1000.0;
            minlmsetbc(&state, &bndl, &bndu, _state);
            lsfit_logisticfitinternal(&x, &y, n, is4pl, lambdav, &state, &replm, &p1, &flast, _state);
            rep->iterationscount = rep->iterationscount+replm.iterationscount;
            
            /*
             * Compare results with best value found so far.
             */
            if( ae_fp_less(flast,fbest) )
            {
                *a = p1.ptr.p_double[0];
                *b = p1.ptr.p_double[1];
                *c = p1.ptr.p_double[2];
                *d = p1.ptr.p_double[3];
                *g = p1.ptr.p_double[4];
                fbest = flast;
            }
        }
        lsfit_logisticfit45errors(&x, &y, n, *a, *b, *c, *d, *g, rep, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Well.... we have 5PL fit, and we have to test two separate branches:
     * B>0 and B<0, because of asymmetry in the curve. First, we run optimization
     * with tight constraints two times, in order to determine better sign for B.
     *
     * Run outer iterations
     */
    *a = (double)(0);
    *b = (double)(1);
    *c = (double)(1);
    *d = (double)(1);
    *g = (double)(1);
    fbest = ae_maxrealnumber;
    for(outerit=0; outerit<=rscnt-1; outerit++)
    {
        
        /*
         * First, we try positive B.
         */
        p1.ptr.p_double[0] = y.ptr.p_double[0]+0.15*scaley*(hqrnduniformr(&rs, _state)-0.5);
        p1.ptr.p_double[1] = 0.5+hqrnduniformr(&rs, _state);
        p1.ptr.p_double[2] = x.ptr.p_double[nz+hqrnduniformi(&rs, n-nz, _state)];
        p1.ptr.p_double[3] = y.ptr.p_double[n-1]+0.25*scaley*(hqrnduniformr(&rs, _state)-0.5);
        p1.ptr.p_double[4] = 1.0;
        bndl1.ptr.p_double[0] = _state->v_neginf;
        bndu1.ptr.p_double[0] = _state->v_posinf;
        bndl1.ptr.p_double[1] = 0.5;
        bndu1.ptr.p_double[1] = 2.0;
        bndl1.ptr.p_double[2] = 0.5*scalex;
        bndu1.ptr.p_double[2] = 2.0*scalex;
        bndl1.ptr.p_double[3] = _state->v_neginf;
        bndu1.ptr.p_double[3] = _state->v_posinf;
        bndl1.ptr.p_double[4] = 0.5;
        bndu1.ptr.p_double[4] = 2.0;
        if( ae_isfinite(cnstrleft, _state) )
        {
            p1.ptr.p_double[0] = cnstrleft;
            bndl1.ptr.p_double[0] = cnstrleft;
            bndu1.ptr.p_double[0] = cnstrleft;
        }
        if( ae_isfinite(cnstrright, _state) )
        {
            p1.ptr.p_double[3] = cnstrright;
            bndl1.ptr.p_double[3] = cnstrright;
            bndu1.ptr.p_double[3] = cnstrright;
        }
        minlmsetbc(&state, &bndl1, &bndu1, _state);
        lsfit_logisticfitinternal(&x, &y, n, is4pl, (double)100*lambdav, &state, &replm, &p1, &fposb, _state);
        rep->iterationscount = rep->iterationscount+replm.iterationscount;
        
        /*
         * Second attempt - with negative B (constraints are still tight).
         */
        p2.ptr.p_double[0] = y.ptr.p_double[n-1]+0.15*scaley*(hqrnduniformr(&rs, _state)-0.5);
        p2.ptr.p_double[1] = -(0.5+hqrnduniformr(&rs, _state));
        p2.ptr.p_double[2] = x.ptr.p_double[nz+hqrnduniformi(&rs, n-nz, _state)];
        p2.ptr.p_double[3] = y.ptr.p_double[0]+0.25*scaley*(hqrnduniformr(&rs, _state)-0.5);
        p2.ptr.p_double[4] = 1.0;
        bndl2.ptr.p_double[0] = _state->v_neginf;
        bndu2.ptr.p_double[0] = _state->v_posinf;
        bndl2.ptr.p_double[1] = -2.0;
        bndu2.ptr.p_double[1] = -0.5;
        bndl2.ptr.p_double[2] = 0.5*scalex;
        bndu2.ptr.p_double[2] = 2.0*scalex;
        bndl2.ptr.p_double[3] = _state->v_neginf;
        bndu2.ptr.p_double[3] = _state->v_posinf;
        bndl2.ptr.p_double[4] = 0.5;
        bndu2.ptr.p_double[4] = 2.0;
        if( ae_isfinite(cnstrleft, _state) )
        {
            p2.ptr.p_double[3] = cnstrleft;
            bndl2.ptr.p_double[3] = cnstrleft;
            bndu2.ptr.p_double[3] = cnstrleft;
        }
        if( ae_isfinite(cnstrright, _state) )
        {
            p2.ptr.p_double[0] = cnstrright;
            bndl2.ptr.p_double[0] = cnstrright;
            bndu2.ptr.p_double[0] = cnstrright;
        }
        minlmsetbc(&state, &bndl2, &bndu2, _state);
        lsfit_logisticfitinternal(&x, &y, n, is4pl, (double)100*lambdav, &state, &replm, &p2, &fnegb, _state);
        rep->iterationscount = rep->iterationscount+replm.iterationscount;
        
        /*
         * Select best version of B sign
         */
        if( ae_fp_less(fposb,fnegb) )
        {
            
            /*
             * Prepare relaxed constraints assuming that B is positive
             */
            bndl1.ptr.p_double[1] = 0.1;
            bndu1.ptr.p_double[1] = 10.0;
            bndl1.ptr.p_double[2] = ae_machineepsilon*scalex;
            bndu1.ptr.p_double[2] = scalex/ae_machineepsilon;
            bndl1.ptr.p_double[4] = 0.1;
            bndu1.ptr.p_double[4] = 10.0;
            minlmsetbc(&state, &bndl1, &bndu1, _state);
            lsfit_logisticfitinternal(&x, &y, n, is4pl, lambdav, &state, &replm, &p1, &flast, _state);
            rep->iterationscount = rep->iterationscount+replm.iterationscount;
            
            /*
             * Prepare stronger relaxation of constraints
             */
            bndl1.ptr.p_double[1] = 0.01;
            bndu1.ptr.p_double[1] = 100.0;
            minlmsetbc(&state, &bndl1, &bndu1, _state);
            lsfit_logisticfitinternal(&x, &y, n, is4pl, lambdav, &state, &replm, &p1, &flast, _state);
            rep->iterationscount = rep->iterationscount+replm.iterationscount;
            
            /*
             * Prepare stronger relaxation of constraints
             */
            bndl1.ptr.p_double[1] = 0.001;
            bndu1.ptr.p_double[1] = 1000.0;
            minlmsetbc(&state, &bndl1, &bndu1, _state);
            lsfit_logisticfitinternal(&x, &y, n, is4pl, lambdav, &state, &replm, &p1, &flast, _state);
            rep->iterationscount = rep->iterationscount+replm.iterationscount;
            
            /*
             * Compare results with best value found so far.
             */
            if( ae_fp_less(flast,fbest) )
            {
                *a = p1.ptr.p_double[0];
                *b = p1.ptr.p_double[1];
                *c = p1.ptr.p_double[2];
                *d = p1.ptr.p_double[3];
                *g = p1.ptr.p_double[4];
                fbest = flast;
            }
        }
        else
        {
            
            /*
             * Prepare relaxed constraints assuming that B is negative
             */
            bndl2.ptr.p_double[1] = -10.0;
            bndu2.ptr.p_double[1] = -0.1;
            bndl2.ptr.p_double[2] = ae_machineepsilon*scalex;
            bndu2.ptr.p_double[2] = scalex/ae_machineepsilon;
            bndl2.ptr.p_double[4] = 0.1;
            bndu2.ptr.p_double[4] = 10.0;
            minlmsetbc(&state, &bndl2, &bndu2, _state);
            lsfit_logisticfitinternal(&x, &y, n, is4pl, lambdav, &state, &replm, &p2, &flast, _state);
            rep->iterationscount = rep->iterationscount+replm.iterationscount;
            
            /*
             * Prepare stronger relaxation
             */
            bndl2.ptr.p_double[1] = -100.0;
            bndu2.ptr.p_double[1] = -0.01;
            minlmsetbc(&state, &bndl2, &bndu2, _state);
            lsfit_logisticfitinternal(&x, &y, n, is4pl, lambdav, &state, &replm, &p2, &flast, _state);
            rep->iterationscount = rep->iterationscount+replm.iterationscount;
            
            /*
             * Prepare stronger relaxation
             */
            bndl2.ptr.p_double[1] = -1000.0;
            bndu2.ptr.p_double[1] = -0.001;
            minlmsetbc(&state, &bndl2, &bndu2, _state);
            lsfit_logisticfitinternal(&x, &y, n, is4pl, lambdav, &state, &replm, &p2, &flast, _state);
            rep->iterationscount = rep->iterationscount+replm.iterationscount;
            
            /*
             * Compare results with best value found so far.
             */
            if( ae_fp_less(flast,fbest) )
            {
                *a = p2.ptr.p_double[0];
                *b = p2.ptr.p_double[1];
                *c = p2.ptr.p_double[2];
                *d = p2.ptr.p_double[3];
                *g = p2.ptr.p_double[4];
                fbest = flast;
            }
        }
    }
    lsfit_logisticfit45errors(&x, &y, n, *a, *b, *c, *d, *g, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Weghted rational least  squares  fitting  using  Floater-Hormann  rational
functions  with  optimal  D  chosen  from  [0,9],  with  constraints   and
individual weights.

Equidistant  grid  with M node on [min(x),max(x)]  is  used to build basis
functions. Different values of D are tried, optimal D (least WEIGHTED root
mean square error) is chosen.  Task  is  linear,  so  linear least squares
solver  is  used.  Complexity  of  this  computational  scheme is O(N*M^2)
(mostly dominated by the least squares solver).

SEE ALSO
* BarycentricFitFloaterHormann(), "lightweight" fitting without invididual
  weights and constraints.

INPUT PARAMETERS:
    X   -   points, array[0..N-1].
    Y   -   function values, array[0..N-1].
    W   -   weights, array[0..N-1]
            Each summand in square  sum  of  approximation deviations from
            given  values  is  multiplied  by  the square of corresponding
            weight. Fill it by 1's if you don't  want  to  solve  weighted
            task.
    N   -   number of points, N>0.
    XC  -   points where function values/derivatives are constrained,
            array[0..K-1].
    YC  -   values of constraints, array[0..K-1]
    DC  -   array[0..K-1], types of constraints:
            * DC[i]=0   means that S(XC[i])=YC[i]
            * DC[i]=1   means that S'(XC[i])=YC[i]
            SEE BELOW FOR IMPORTANT INFORMATION ON CONSTRAINTS
    K   -   number of constraints, 0<=K<M.
            K=0 means no constraints (XC/YC/DC are not used in such cases)
    M   -   number of basis functions ( = number_of_nodes), M>=2.

OUTPUT PARAMETERS:
    B   -   barycentric interpolant. Undefined for rep.terminationtype<0.
    Rep -   fitting report. The following fields are set:
            * Rep.TerminationType is a completion code:
              * set to  1 on success
              * set to -3 on failure due to  problematic  constraints:
                either too many  constraints,  degenerate  constraints
                or inconsistent constraints were passed
            * DBest         best value of the D parameter
            * RMSError      rms error on the (X,Y).
            * AvgError      average error on the (X,Y).
            * AvgRelError   average relative error on the non-zero Y
            * MaxError      maximum error
                            NON-WEIGHTED ERRORS ARE CALCULATED

IMPORTANT:
    this subroutine doesn't calculate task's condition number for K<>0.

SETTING CONSTRAINTS - DANGERS AND OPPORTUNITIES:

Setting constraints can lead  to undesired  results,  like ill-conditioned
behavior, or inconsistency being detected. From the other side,  it allows
us to improve quality of the fit. Here we summarize  our  experience  with
constrained barycentric interpolants:
* excessive  constraints  can  be  inconsistent.   Floater-Hormann   basis
  functions aren't as flexible as splines (although they are very smooth).
* the more evenly constraints are spread across [min(x),max(x)],  the more
  chances that they will be consistent
* the  greater  is  M (given  fixed  constraints),  the  more chances that
  constraints will be consistent
* in the general case, consistency of constraints IS NOT GUARANTEED.
* in the several special cases, however, we CAN guarantee consistency.
* one of this cases is constraints on the function  VALUES at the interval
  boundaries. Note that consustency of the  constraints  on  the  function
  DERIVATIVES is NOT guaranteed (you can use in such cases  cubic  splines
  which are more flexible).
* another  special  case  is ONE constraint on the function value (OR, but
  not AND, derivative) anywhere in the interval

Our final recommendation is to use constraints  WHEN  AND  ONLY  WHEN  you
can't solve your task without them. Anything beyond  special  cases  given
above is not guaranteed and may result in inconsistency.

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

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

  -- ALGLIB PROJECT --
     Copyright 18.08.2009 by Bochkanov Sergey
*************************************************************************/
void barycentricfitfloaterhormannwc(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* w,
     ae_int_t n,
     /* Real    */ const ae_vector* xc,
     /* Real    */ const ae_vector* yc,
     /* Integer */ const ae_vector* dc,
     ae_int_t k,
     ae_int_t m,
     barycentricinterpolant* b,
     barycentricfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t d;
    ae_int_t i;
    double wrmscur;
    double wrmsbest;
    barycentricinterpolant locb;
    barycentricfitreport locrep;
    ae_int_t locinfo;

    ae_frame_make(_state, &_frame_block);
    memset(&locb, 0, sizeof(locb));
    memset(&locrep, 0, sizeof(locrep));
    _barycentricinterpolant_clear(b);
    _barycentricfitreport_clear(rep);
    _barycentricinterpolant_init(&locb, _state, ae_true);
    _barycentricfitreport_init(&locrep, _state, ae_true);

    ae_assert(n>0, "BarycentricFitFloaterHormannWC: N<=0!", _state);
    ae_assert(m>0, "BarycentricFitFloaterHormannWC: M<=0!", _state);
    ae_assert(k>=0, "BarycentricFitFloaterHormannWC: K<0!", _state);
    ae_assert(k<m, "BarycentricFitFloaterHormannWC: K>=M!", _state);
    ae_assert(x->cnt>=n, "BarycentricFitFloaterHormannWC: Length(X)<N!", _state);
    ae_assert(y->cnt>=n, "BarycentricFitFloaterHormannWC: Length(Y)<N!", _state);
    ae_assert(w->cnt>=n, "BarycentricFitFloaterHormannWC: Length(W)<N!", _state);
    ae_assert(xc->cnt>=k, "BarycentricFitFloaterHormannWC: Length(XC)<K!", _state);
    ae_assert(yc->cnt>=k, "BarycentricFitFloaterHormannWC: Length(YC)<K!", _state);
    ae_assert(dc->cnt>=k, "BarycentricFitFloaterHormannWC: Length(DC)<K!", _state);
    ae_assert(isfinitevector(x, n, _state), "BarycentricFitFloaterHormannWC: X contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(y, n, _state), "BarycentricFitFloaterHormannWC: Y contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(w, n, _state), "BarycentricFitFloaterHormannWC: X contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(xc, k, _state), "BarycentricFitFloaterHormannWC: XC contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(yc, k, _state), "BarycentricFitFloaterHormannWC: YC contains infinite or NaN values!", _state);
    for(i=0; i<=k-1; i++)
    {
        ae_assert(dc->ptr.p_int[i]==0||dc->ptr.p_int[i]==1, "BarycentricFitFloaterHormannWC: one of DC[] is not 0 or 1!", _state);
    }
    
    /*
     * Find optimal D
     *
     * Info is -3 by default (degenerate constraints).
     * If LocInfo will always be equal to -3, Info will remain equal to -3.
     * If at least once LocInfo will be -4, Info will be -4.
     */
    wrmsbest = ae_maxrealnumber;
    rep->dbest = -1;
    rep->terminationtype = -3;
    for(d=0; d<=ae_minint(9, n-1, _state); d++)
    {
        lsfit_barycentricfitwcfixedd(x, y, w, n, xc, yc, dc, k, m, d, &locinfo, &locb, &locrep, _state);
        ae_assert((locinfo==-4||locinfo==-3)||locinfo>0, "BarycentricFitFloaterHormannWC: unexpected result from BarycentricFitWCFixedD!", _state);
        if( locinfo>0 )
        {
            
            /*
             * Calculate weghted RMS
             */
            wrmscur = (double)(0);
            for(i=0; i<=n-1; i++)
            {
                wrmscur = wrmscur+ae_sqr(w->ptr.p_double[i]*(y->ptr.p_double[i]-barycentriccalc(&locb, x->ptr.p_double[i], _state)), _state);
            }
            wrmscur = ae_sqrt(wrmscur/(double)n, _state);
            if( ae_fp_less(wrmscur,wrmsbest)||rep->dbest<0 )
            {
                barycentriccopy(&locb, b, _state);
                rep->dbest = d;
                rep->terminationtype = 1;
                rep->rmserror = locrep.rmserror;
                rep->avgerror = locrep.avgerror;
                rep->avgrelerror = locrep.avgrelerror;
                rep->maxerror = locrep.maxerror;
                rep->taskrcond = locrep.taskrcond;
                wrmsbest = wrmscur;
            }
        }
        else
        {
            if( locinfo!=-3&&rep->terminationtype<0 )
            {
                rep->terminationtype = locinfo;
            }
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Rational least squares fitting using  Floater-Hormann  rational  functions
with optimal D chosen from [0,9].

Equidistant  grid  with M node on [min(x),max(x)]  is  used to build basis
functions. Different values of D are tried, optimal  D  (least  root  mean
square error) is chosen.  Task  is  linear, so linear least squares solver
is used. Complexity  of  this  computational  scheme is  O(N*M^2)  (mostly
dominated by the least squares solver).

INPUT PARAMETERS:
    X   -   points, array[0..N-1].
    Y   -   function values, array[0..N-1].
    N   -   number of points, N>0.
    M   -   number of basis functions ( = number_of_nodes), M>=2.

OUTPUT PARAMETERS:
    B   -   barycentric interpolant.
    Rep -   fitting report. The following fields are set:
            * Rep.TerminationType is a completion code, always set to 1
            * DBest         best value of the D parameter
            * RMSError      rms error on the (X,Y).
            * AvgError      average error on the (X,Y).
            * AvgRelError   average relative error on the non-zero Y
            * MaxError      maximum error
                            NON-WEIGHTED ERRORS ARE CALCULATED

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

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

  -- ALGLIB PROJECT --
     Copyright 18.08.2009 by Bochkanov Sergey
*************************************************************************/
void barycentricfitfloaterhormann(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     ae_int_t m,
     barycentricinterpolant* b,
     barycentricfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector w;
    ae_vector xc;
    ae_vector yc;
    ae_vector dc;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&w, 0, sizeof(w));
    memset(&xc, 0, sizeof(xc));
    memset(&yc, 0, sizeof(yc));
    memset(&dc, 0, sizeof(dc));
    _barycentricinterpolant_clear(b);
    _barycentricfitreport_clear(rep);
    ae_vector_init(&w, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xc, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&yc, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&dc, 0, DT_INT, _state, ae_true);

    ae_assert(n>0, "BarycentricFitFloaterHormann: N<=0!", _state);
    ae_assert(m>0, "BarycentricFitFloaterHormann: M<=0!", _state);
    ae_assert(x->cnt>=n, "BarycentricFitFloaterHormann: Length(X)<N!", _state);
    ae_assert(y->cnt>=n, "BarycentricFitFloaterHormann: Length(Y)<N!", _state);
    ae_assert(isfinitevector(x, n, _state), "BarycentricFitFloaterHormann: X contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(y, n, _state), "BarycentricFitFloaterHormann: Y contains infinite or NaN values!", _state);
    ae_vector_set_length(&w, n, _state);
    for(i=0; i<=n-1; i++)
    {
        w.ptr.p_double[i] = (double)(1);
    }
    barycentricfitfloaterhormannwc(x, y, &w, n, &xc, &yc, &dc, 0, m, b, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Weighted fitting by cubic  spline,  with constraints on function values or
derivatives.

Equidistant grid with M-2 nodes on [min(x,xc),max(x,xc)] is  used to build
basis functions. Basis functions are cubic splines with continuous  second
derivatives  and  non-fixed first  derivatives  at  interval  ends.  Small
regularizing term is used  when  solving  constrained  tasks  (to  improve
stability).

Task is linear, so linear least squares solver is used. Complexity of this
computational scheme is O(N*M^2), mostly dominated by least squares solver

IMPORTANT: ALGLIB has a much faster version  of  the  cubic spline fitting
           function - spline1dfit(). This function performs least  squares
           fit in O(max(M,N)) time/memory. However, it  does  not  support
           constraints.
                                
INPUT PARAMETERS:
    X   -   points, array[0..N-1].
    Y   -   function values, array[0..N-1].
    W   -   weights, array[0..N-1]
            Each summand in square  sum  of  approximation deviations from
            given  values  is  multiplied  by  the square of corresponding
            weight. Fill it by 1's if you don't  want  to  solve  weighted
            task.
    N   -   number of points (optional):
            * N>0
            * if given, only first N elements of X/Y/W are processed
            * if not given, automatically determined from X/Y/W sizes
    XC  -   points where spline values/derivatives are constrained,
            array[0..K-1].
    YC  -   values of constraints, array[0..K-1]
    DC  -   array[0..K-1], types of constraints:
            * DC[i]=0   means that S(XC[i])=YC[i]
            * DC[i]=1   means that S'(XC[i])=YC[i]
            SEE BELOW FOR IMPORTANT INFORMATION ON CONSTRAINTS
    K   -   number of constraints (optional):
            * 0<=K<M.
            * K=0 means no constraints (XC/YC/DC are not used)
            * if given, only first K elements of XC/YC/DC are used
            * if not given, automatically determined from XC/YC/DC
    M   -   number of basis functions ( = number_of_nodes+2), M>=4.

OUTPUT PARAMETERS:
    S   -   spline interpolant.
    Rep     -   fitting report. The following fields are set:
                * Rep.TerminationType is a completion code:
                  * set to  1 on success
                  * set to -3 on failure due to  problematic  constraints:
                    either too many  constraints,  degenerate  constraints
                    or inconsistent constraints were passed
                * RMSError      rms error on the (X,Y).
                * AvgError      average error on the (X,Y).
                * AvgRelError   average relative error on the non-zero Y
                * MaxError      maximum error
                                NON-WEIGHTED ERRORS ARE CALCULATED

IMPORTANT:
    this subroitine doesn't calculate task's condition number for K<>0.


ORDER OF POINTS

Subroutine automatically sorts points, so caller may pass unsorted array.

SETTING CONSTRAINTS - DANGERS AND OPPORTUNITIES:

Setting constraints can lead  to undesired  results,  like ill-conditioned
behavior, or inconsistency being detected. From the other side,  it allows
us to improve quality of the fit. Here we summarize  our  experience  with
constrained regression splines:
* excessive constraints can be inconsistent. Splines are  piecewise  cubic
  functions, and it is easy to create an example, where  large  number  of
  constraints  concentrated  in  small  area will result in inconsistency.
  Just because spline is not flexible enough to satisfy all of  them.  And
  same constraints spread across the  [min(x),max(x)]  will  be  perfectly
  consistent.
* the more evenly constraints are spread across [min(x),max(x)],  the more
  chances that they will be consistent
* the  greater  is  M (given  fixed  constraints),  the  more chances that
  constraints will be consistent
* in the general case, consistency of constraints IS NOT GUARANTEED.
* in the several special cases, however, we CAN guarantee consistency.
* one of this cases is constraints  on  the  function  values  AND/OR  its
  derivatives at the interval boundaries.
* another  special  case  is ONE constraint on the function value (OR, but
  not AND, derivative) anywhere in the interval

Our final recommendation is to use constraints  WHEN  AND  ONLY  WHEN  you
can't solve your task without them. Anything beyond  special  cases  given
above is not guaranteed and may result in inconsistency.

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

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

  -- ALGLIB PROJECT --
     Copyright 18.08.2009 by Bochkanov Sergey
*************************************************************************/
void spline1dfitcubicwc(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* w,
     ae_int_t n,
     /* Real    */ const ae_vector* xc,
     /* Real    */ const ae_vector* yc,
     /* Integer */ const ae_vector* dc,
     ae_int_t k,
     ae_int_t m,
     spline1dinterpolant* s,
     spline1dfitreport* rep,
     ae_state *_state)
{
    ae_int_t i;

    _spline1dinterpolant_clear(s);
    _spline1dfitreport_clear(rep);

    ae_assert(n>=1, "Spline1DFitCubicWC: N<1!", _state);
    ae_assert(m>=4, "Spline1DFitCubicWC: M<4!", _state);
    ae_assert(k>=0, "Spline1DFitCubicWC: K<0!", _state);
    ae_assert(k<m, "Spline1DFitCubicWC: K>=M!", _state);
    ae_assert(x->cnt>=n, "Spline1DFitCubicWC: Length(X)<N!", _state);
    ae_assert(y->cnt>=n, "Spline1DFitCubicWC: Length(Y)<N!", _state);
    ae_assert(w->cnt>=n, "Spline1DFitCubicWC: Length(W)<N!", _state);
    ae_assert(xc->cnt>=k, "Spline1DFitCubicWC: Length(XC)<K!", _state);
    ae_assert(yc->cnt>=k, "Spline1DFitCubicWC: Length(YC)<K!", _state);
    ae_assert(dc->cnt>=k, "Spline1DFitCubicWC: Length(DC)<K!", _state);
    ae_assert(isfinitevector(x, n, _state), "Spline1DFitCubicWC: X contains infinite or NAN values!", _state);
    ae_assert(isfinitevector(y, n, _state), "Spline1DFitCubicWC: Y contains infinite or NAN values!", _state);
    ae_assert(isfinitevector(w, n, _state), "Spline1DFitCubicWC: Y contains infinite or NAN values!", _state);
    ae_assert(isfinitevector(xc, k, _state), "Spline1DFitCubicWC: X contains infinite or NAN values!", _state);
    ae_assert(isfinitevector(yc, k, _state), "Spline1DFitCubicWC: Y contains infinite or NAN values!", _state);
    for(i=0; i<=k-1; i++)
    {
        ae_assert(dc->ptr.p_int[i]==0||dc->ptr.p_int[i]==1, "Spline1DFitCubicWC: DC[i] is neither 0 or 1!", _state);
    }
    lsfit_spline1dfitinternal(0, x, y, w, n, xc, yc, dc, k, m, s, rep, _state);
}


/*************************************************************************
Weighted  fitting  by Hermite spline,  with constraints on function values
or first derivatives.

Equidistant grid with M nodes on [min(x,xc),max(x,xc)] is  used  to  build
basis functions. Basis functions are Hermite splines.  Small  regularizing
term is used when solving constrained tasks (to improve stability).

Task is linear, so linear least squares solver is used. Complexity of this
computational scheme is O(N*M^2), mostly dominated by least squares solver

IMPORTANT: ALGLIB has a much faster version  of  the  cubic spline fitting
           function - spline1dfit(). This function performs least  squares
           fit in O(max(M,N)) time/memory. However, it  does  not  support
           constraints.
                                
INPUT PARAMETERS:
    X   -   points, array[0..N-1].
    Y   -   function values, array[0..N-1].
    W   -   weights, array[0..N-1]
            Each summand in square  sum  of  approximation deviations from
            given  values  is  multiplied  by  the square of corresponding
            weight. Fill it by 1's if you don't  want  to  solve  weighted
            task.
    N   -   number of points (optional):
            * N>0
            * if given, only first N elements of X/Y/W are processed
            * if not given, automatically determined from X/Y/W sizes
    XC  -   points where spline values/derivatives are constrained,
            array[0..K-1].
    YC  -   values of constraints, array[0..K-1]
    DC  -   array[0..K-1], types of constraints:
            * DC[i]=0   means that S(XC[i])=YC[i]
            * DC[i]=1   means that S'(XC[i])=YC[i]
            SEE BELOW FOR IMPORTANT INFORMATION ON CONSTRAINTS
    K   -   number of constraints (optional):
            * 0<=K<M.
            * K=0 means no constraints (XC/YC/DC are not used)
            * if given, only first K elements of XC/YC/DC are used
            * if not given, automatically determined from XC/YC/DC
    M   -   number of basis functions (= 2 * number of nodes),
            M>=4,
            M IS EVEN!

OUTPUT PARAMETERS:
    S   -   spline interpolant.
    Rep     -   fitting report. The following fields are set:
                * Rep.TerminationType is a completion code:
                  * set to  1 on success
                  * set to -3 on failure due to  problematic  constraints:
                    either too many  constraints,  degenerate  constraints
                    or inconsistent constraints were passed
                  * RMSError      rms error on the (X,Y).
                * AvgError      average error on the (X,Y).
                * AvgRelError   average relative error on the non-zero Y
                * MaxError      maximum error
                                NON-WEIGHTED ERRORS ARE CALCULATED

IMPORTANT:
    this subroitine doesn't calculate task's condition number for K<>0.

IMPORTANT:
    this subroitine supports only even M's


ORDER OF POINTS

Subroutine automatically sorts points, so caller may pass unsorted array.

SETTING CONSTRAINTS - DANGERS AND OPPORTUNITIES:

Setting constraints can lead  to undesired  results,  like ill-conditioned
behavior, or inconsistency being detected. From the other side,  it allows
us to improve quality of the fit. Here we summarize  our  experience  with
constrained regression splines:
* excessive constraints can be inconsistent. Splines are  piecewise  cubic
  functions, and it is easy to create an example, where  large  number  of
  constraints  concentrated  in  small  area will result in inconsistency.
  Just because spline is not flexible enough to satisfy all of  them.  And
  same constraints spread across the  [min(x),max(x)]  will  be  perfectly
  consistent.
* the more evenly constraints are spread across [min(x),max(x)],  the more
  chances that they will be consistent
* the  greater  is  M (given  fixed  constraints),  the  more chances that
  constraints will be consistent
* in the general case, consistency of constraints is NOT GUARANTEED.
* in the several special cases, however, we can guarantee consistency.
* one of this cases is  M>=4  and   constraints  on   the  function  value
  (AND/OR its derivative) at the interval boundaries.
* another special case is M>=4  and  ONE  constraint on the function value
  (OR, BUT NOT AND, derivative) anywhere in [min(x),max(x)]

Our final recommendation is to use constraints  WHEN  AND  ONLY  when  you
can't solve your task without them. Anything beyond  special  cases  given
above is not guaranteed and may result in inconsistency.

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

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

  -- ALGLIB PROJECT --
     Copyright 18.08.2009 by Bochkanov Sergey
*************************************************************************/
void spline1dfithermitewc(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* w,
     ae_int_t n,
     /* Real    */ const ae_vector* xc,
     /* Real    */ const ae_vector* yc,
     /* Integer */ const ae_vector* dc,
     ae_int_t k,
     ae_int_t m,
     spline1dinterpolant* s,
     spline1dfitreport* rep,
     ae_state *_state)
{
    ae_int_t i;

    _spline1dinterpolant_clear(s);
    _spline1dfitreport_clear(rep);

    ae_assert(n>=1, "Spline1DFitHermiteWC: N<1!", _state);
    ae_assert(m>=4, "Spline1DFitHermiteWC: M<4!", _state);
    ae_assert(m%2==0, "Spline1DFitHermiteWC: M is odd!", _state);
    ae_assert(k>=0, "Spline1DFitHermiteWC: K<0!", _state);
    ae_assert(k<m, "Spline1DFitHermiteWC: K>=M!", _state);
    ae_assert(x->cnt>=n, "Spline1DFitHermiteWC: Length(X)<N!", _state);
    ae_assert(y->cnt>=n, "Spline1DFitHermiteWC: Length(Y)<N!", _state);
    ae_assert(w->cnt>=n, "Spline1DFitHermiteWC: Length(W)<N!", _state);
    ae_assert(xc->cnt>=k, "Spline1DFitHermiteWC: Length(XC)<K!", _state);
    ae_assert(yc->cnt>=k, "Spline1DFitHermiteWC: Length(YC)<K!", _state);
    ae_assert(dc->cnt>=k, "Spline1DFitHermiteWC: Length(DC)<K!", _state);
    ae_assert(isfinitevector(x, n, _state), "Spline1DFitHermiteWC: X contains infinite or NAN values!", _state);
    ae_assert(isfinitevector(y, n, _state), "Spline1DFitHermiteWC: Y contains infinite or NAN values!", _state);
    ae_assert(isfinitevector(w, n, _state), "Spline1DFitHermiteWC: Y contains infinite or NAN values!", _state);
    ae_assert(isfinitevector(xc, k, _state), "Spline1DFitHermiteWC: X contains infinite or NAN values!", _state);
    ae_assert(isfinitevector(yc, k, _state), "Spline1DFitHermiteWC: Y contains infinite or NAN values!", _state);
    for(i=0; i<=k-1; i++)
    {
        ae_assert(dc->ptr.p_int[i]==0||dc->ptr.p_int[i]==1, "Spline1DFitHermiteWC: DC[i] is neither 0 or 1!", _state);
    }
    lsfit_spline1dfitinternal(1, x, y, w, n, xc, yc, dc, k, m, s, rep, _state);
}


/*************************************************************************
Deprecated fitting function with O(N*M^2+M^3) running time. Superseded  by
spline1dfit().

  -- ALGLIB PROJECT --
     Copyright 18.08.2009 by Bochkanov Sergey
*************************************************************************/
void spline1dfitcubicdeprecated(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     ae_int_t m,
     spline1dinterpolant* s,
     spline1dfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_vector w;
    ae_vector xc;
    ae_vector yc;
    ae_vector dc;

    ae_frame_make(_state, &_frame_block);
    memset(&w, 0, sizeof(w));
    memset(&xc, 0, sizeof(xc));
    memset(&yc, 0, sizeof(yc));
    memset(&dc, 0, sizeof(dc));
    _spline1dinterpolant_clear(s);
    _spline1dfitreport_clear(rep);
    ae_vector_init(&w, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xc, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&yc, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&dc, 0, DT_INT, _state, ae_true);

    ae_assert(n>=1, "Spline1DFitCubic: N<1!", _state);
    ae_assert(m>=4, "Spline1DFitCubic: M<4!", _state);
    ae_assert(x->cnt>=n, "Spline1DFitCubic: Length(X)<N!", _state);
    ae_assert(y->cnt>=n, "Spline1DFitCubic: Length(Y)<N!", _state);
    ae_assert(isfinitevector(x, n, _state), "Spline1DFitCubic: X contains infinite or NAN values!", _state);
    ae_assert(isfinitevector(y, n, _state), "Spline1DFitCubic: Y contains infinite or NAN values!", _state);
    ae_vector_set_length(&w, n, _state);
    for(i=0; i<=n-1; i++)
    {
        w.ptr.p_double[i] = (double)(1);
    }
    spline1dfitcubicwc(x, y, &w, n, &xc, &yc, &dc, 0, m, s, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Deprecated fitting function with O(N*M^2+M^3) running time. Superseded  by
spline1dfit().

  -- ALGLIB PROJECT --
     Copyright 18.08.2009 by Bochkanov Sergey
*************************************************************************/
void spline1dfithermitedeprecated(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     ae_int_t m,
     spline1dinterpolant* s,
     spline1dfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_vector w;
    ae_vector xc;
    ae_vector yc;
    ae_vector dc;

    ae_frame_make(_state, &_frame_block);
    memset(&w, 0, sizeof(w));
    memset(&xc, 0, sizeof(xc));
    memset(&yc, 0, sizeof(yc));
    memset(&dc, 0, sizeof(dc));
    _spline1dinterpolant_clear(s);
    _spline1dfitreport_clear(rep);
    ae_vector_init(&w, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xc, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&yc, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&dc, 0, DT_INT, _state, ae_true);

    ae_assert(n>=1, "Spline1DFitHermite: N<1!", _state);
    ae_assert(m>=4, "Spline1DFitHermite: M<4!", _state);
    ae_assert(m%2==0, "Spline1DFitHermite: M is odd!", _state);
    ae_assert(x->cnt>=n, "Spline1DFitHermite: Length(X)<N!", _state);
    ae_assert(y->cnt>=n, "Spline1DFitHermite: Length(Y)<N!", _state);
    ae_assert(isfinitevector(x, n, _state), "Spline1DFitHermite: X contains infinite or NAN values!", _state);
    ae_assert(isfinitevector(y, n, _state), "Spline1DFitHermite: Y contains infinite or NAN values!", _state);
    ae_vector_set_length(&w, n, _state);
    for(i=0; i<=n-1; i++)
    {
        w.ptr.p_double[i] = (double)(1);
    }
    spline1dfithermitewc(x, y, &w, n, &xc, &yc, &dc, 0, m, s, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Weighted linear least squares fitting.

QR decomposition is used to reduce task to MxM, then triangular solver  or
SVD-based solver is used depending on condition number of the  system.  It
allows to maximize speed and retain decent accuracy.

IMPORTANT: if you want to perform  polynomial  fitting,  it  may  be  more
           convenient to use PolynomialFit() function. This function gives
           best  results  on  polynomial  problems  and  solves  numerical
           stability  issues  which  arise  when   you   fit   high-degree
           polynomials to your data.

INPUT PARAMETERS:
    Y       -   array[0..N-1] Function values in  N  points.
    W       -   array[0..N-1]  Weights  corresponding to function  values.
                Each summand in square  sum  of  approximation  deviations
                from  given  values  is  multiplied  by  the   square   of
                corresponding weight.
    FMatrix -   a table of basis functions values, array[0..N-1, 0..M-1].
                FMatrix[I, J] - value of J-th basis function in I-th point.
    N       -   number of points used. N>=1.
    M       -   number of basis functions, M>=1.

OUTPUT PARAMETERS:
    C       -   decomposition coefficients, array[0..M-1]
    Rep     -   fitting report. Following fields are set:
                * Rep.TerminationType always set to 1 (success)
                * Rep.TaskRCond     reciprocal of condition number
                * R2                non-adjusted coefficient of determination
                                    (non-weighted)
                * RMSError          rms error on the (X,Y).
                * AvgError          average error on the (X,Y).
                * AvgRelError       average relative error on the non-zero Y
                * MaxError          maximum error
                                    NON-WEIGHTED ERRORS ARE CALCULATED
                
ERRORS IN PARAMETERS                
                
This  solver  also  calculates different kinds of errors in parameters and
fills corresponding fields of report:
* Rep.CovPar        covariance matrix for parameters, array[K,K].
* Rep.ErrPar        errors in parameters, array[K],
                    errpar = sqrt(diag(CovPar))
* Rep.ErrCurve      vector of fit errors - standard deviations of empirical
                    best-fit curve from "ideal" best-fit curve built  with
                    infinite number of samples, array[N].
                    errcurve = sqrt(diag(F*CovPar*F')),
                    where F is functions matrix.
* Rep.Noise         vector of per-point estimates of noise, array[N]
            
NOTE:       noise in the data is estimated as follows:
            * for fitting without user-supplied  weights  all  points  are
              assumed to have same level of noise, which is estimated from
              the data
            * for fitting with user-supplied weights we assume that  noise
              level in I-th point is inversely proportional to Ith weight.
              Coefficient of proportionality is estimated from the data.
            
NOTE:       we apply small amount of regularization when we invert squared
            Jacobian and calculate covariance matrix. It  guarantees  that
            algorithm won't divide by zero  during  inversion,  but  skews
            error estimates a bit (fractional error is about 10^-9).
            
            However, we believe that this difference is insignificant  for
            all practical purposes except for the situation when you  want
            to compare ALGLIB results with "reference"  implementation  up
            to the last significant digit.
            
NOTE:       covariance matrix is estimated using  correction  for  degrees
            of freedom (covariances are divided by N-M instead of dividing
            by N).

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

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
                                    
  -- ALGLIB --
     Copyright 17.08.2009 by Bochkanov Sergey
*************************************************************************/
void lsfitlinearw(/* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* w,
     /* Real    */ const ae_matrix* fmatrix,
     ae_int_t n,
     ae_int_t m,
     /* Real    */ ae_vector* c,
     lsfitreport* rep,
     ae_state *_state)
{

    ae_vector_clear(c);
    _lsfitreport_clear(rep);

    ae_assert(n>=1, "LSFitLinearW: N<1!", _state);
    ae_assert(m>=1, "LSFitLinearW: M<1!", _state);
    ae_assert(y->cnt>=n, "LSFitLinearW: length(Y)<N!", _state);
    ae_assert(isfinitevector(y, n, _state), "LSFitLinearW: Y contains infinite or NaN values!", _state);
    ae_assert(w->cnt>=n, "LSFitLinearW: length(W)<N!", _state);
    ae_assert(isfinitevector(w, n, _state), "LSFitLinearW: W contains infinite or NaN values!", _state);
    ae_assert(fmatrix->rows>=n, "LSFitLinearW: rows(FMatrix)<N!", _state);
    ae_assert(fmatrix->cols>=m, "LSFitLinearW: cols(FMatrix)<M!", _state);
    ae_assert(apservisfinitematrix(fmatrix, n, m, _state), "LSFitLinearW: FMatrix contains infinite or NaN values!", _state);
    lsfit_lsfitlinearinternal(y, w, fmatrix, n, m, c, rep, _state);
}


/*************************************************************************
Weighted constained linear least squares fitting.

This  is  variation  of LSFitLinearW(), which searchs for min|A*x=b| given
that  K  additional  constaints  C*x=bc are satisfied. It reduces original
task to modified one: min|B*y-d| WITHOUT constraints,  then LSFitLinearW()
is called.

IMPORTANT: if you want to perform  polynomial  fitting,  it  may  be  more
           convenient to use PolynomialFit() function. This function gives
           best  results  on  polynomial  problems  and  solves  numerical
           stability  issues  which  arise  when   you   fit   high-degree
           polynomials to your data.

INPUT PARAMETERS:
    Y       -   array[0..N-1] Function values in  N  points.
    W       -   array[0..N-1]  Weights  corresponding to function  values.
                Each summand in square  sum  of  approximation  deviations
                from  given  values  is  multiplied  by  the   square   of
                corresponding weight.
    FMatrix -   a table of basis functions values, array[0..N-1, 0..M-1].
                FMatrix[I,J] - value of J-th basis function in I-th point.
    CMatrix -   a table of constaints, array[0..K-1,0..M].
                I-th row of CMatrix corresponds to I-th linear constraint:
                CMatrix[I,0]*C[0] + ... + CMatrix[I,M-1]*C[M-1] = CMatrix[I,M]
    N       -   number of points used. N>=1.
    M       -   number of basis functions, M>=1.
    K       -   number of constraints, 0 <= K < M
                K=0 corresponds to absence of constraints.

OUTPUT PARAMETERS:
    C       -   decomposition coefficients, array[0..M-1]
    Rep     -   fitting report. The following fields are set:
                * Rep.TerminationType is a completion code:
                  * set to  1 on success
                  * set to -3 on failure due to  problematic  constraints:
                    either too many  constraints (M or  more),  degenerate
                    constraints (some constraints are repetead  twice)  or
                    inconsistent constraints are specified
                * R2                non-adjusted coefficient of determination
                                    (non-weighted)
                * RMSError          rms error on the (X,Y).
                * AvgError          average error on the (X,Y).
                * AvgRelError       average relative error on the non-zero Y
                * MaxError          maximum error
                                    NON-WEIGHTED ERRORS ARE CALCULATED

IMPORTANT:
    this subroitine doesn't calculate task's condition number for K<>0.
                
ERRORS IN PARAMETERS                
                
This  solver  also  calculates different kinds of errors in parameters and
fills corresponding fields of report:
* Rep.CovPar        covariance matrix for parameters, array[K,K].
* Rep.ErrPar        errors in parameters, array[K],
                    errpar = sqrt(diag(CovPar))
* Rep.ErrCurve      vector of fit errors - standard deviations of empirical
                    best-fit curve from "ideal" best-fit curve built  with
                    infinite number of samples, array[N].
                    errcurve = sqrt(diag(F*CovPar*F')),
                    where F is functions matrix.
* Rep.Noise         vector of per-point estimates of noise, array[N]

IMPORTANT:  errors  in  parameters  are  calculated  without  taking  into
            account boundary/linear constraints! Presence  of  constraints
            changes distribution of errors, but there is no  easy  way  to
            account for constraints when you calculate covariance matrix.
            
NOTE:       noise in the data is estimated as follows:
            * for fitting without user-supplied  weights  all  points  are
              assumed to have same level of noise, which is estimated from
              the data
            * for fitting with user-supplied weights we assume that  noise
              level in I-th point is inversely proportional to Ith weight.
              Coefficient of proportionality is estimated from the data.
            
NOTE:       we apply small amount of regularization when we invert squared
            Jacobian and calculate covariance matrix. It  guarantees  that
            algorithm won't divide by zero  during  inversion,  but  skews
            error estimates a bit (fractional error is about 10^-9).
            
            However, we believe that this difference is insignificant  for
            all practical purposes except for the situation when you  want
            to compare ALGLIB results with "reference"  implementation  up
            to the last significant digit.
            
NOTE:       covariance matrix is estimated using  correction  for  degrees
            of freedom (covariances are divided by N-M instead of dividing
            by N).

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

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

  -- ALGLIB --
     Copyright 07.09.2009 by Bochkanov Sergey
*************************************************************************/
void lsfitlinearwc(/* Real    */ const ae_vector* _y,
     /* Real    */ const ae_vector* w,
     /* Real    */ const ae_matrix* fmatrix,
     /* Real    */ const ae_matrix* _cmatrix,
     ae_int_t n,
     ae_int_t m,
     ae_int_t k,
     /* Real    */ ae_vector* c,
     lsfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector y;
    ae_matrix cmatrix;
    ae_int_t i;
    ae_int_t j;
    ae_vector tau;
    ae_matrix q;
    ae_matrix f2;
    ae_vector tmp;
    ae_vector c0;
    double v;

    ae_frame_make(_state, &_frame_block);
    memset(&y, 0, sizeof(y));
    memset(&cmatrix, 0, sizeof(cmatrix));
    memset(&tau, 0, sizeof(tau));
    memset(&q, 0, sizeof(q));
    memset(&f2, 0, sizeof(f2));
    memset(&tmp, 0, sizeof(tmp));
    memset(&c0, 0, sizeof(c0));
    ae_vector_init_copy(&y, _y, _state, ae_true);
    ae_matrix_init_copy(&cmatrix, _cmatrix, _state, ae_true);
    ae_vector_clear(c);
    _lsfitreport_clear(rep);
    ae_vector_init(&tau, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&q, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&f2, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&c0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>=1, "LSFitLinearWC: N<1!", _state);
    ae_assert(m>=1, "LSFitLinearWC: M<1!", _state);
    ae_assert(k>=0, "LSFitLinearWC: K<0!", _state);
    ae_assert(y.cnt>=n, "LSFitLinearWC: length(Y)<N!", _state);
    ae_assert(isfinitevector(&y, n, _state), "LSFitLinearWC: Y contains infinite or NaN values!", _state);
    ae_assert(w->cnt>=n, "LSFitLinearWC: length(W)<N!", _state);
    ae_assert(isfinitevector(w, n, _state), "LSFitLinearWC: W contains infinite or NaN values!", _state);
    ae_assert(fmatrix->rows>=n, "LSFitLinearWC: rows(FMatrix)<N!", _state);
    ae_assert(fmatrix->cols>=m, "LSFitLinearWC: cols(FMatrix)<M!", _state);
    ae_assert(apservisfinitematrix(fmatrix, n, m, _state), "LSFitLinearWC: FMatrix contains infinite or NaN values!", _state);
    ae_assert(cmatrix.rows>=k, "LSFitLinearWC: rows(CMatrix)<K!", _state);
    ae_assert(cmatrix.cols>=m+1||k==0, "LSFitLinearWC: cols(CMatrix)<M+1!", _state);
    ae_assert(apservisfinitematrix(&cmatrix, k, m+1, _state), "LSFitLinearWC: CMatrix contains infinite or NaN values!", _state);
    if( k>=m )
    {
        rep->terminationtype = -3;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Solve
     */
    if( k==0 )
    {
        
        /*
         * no constraints
         */
        lsfit_lsfitlinearinternal(&y, w, fmatrix, n, m, c, rep, _state);
    }
    else
    {
        
        /*
         * First, find general form solution of constraints system:
         * * factorize C = L*Q
         * * unpack Q
         * * fill upper part of C with zeros (for RCond)
         *
         * We got C=C0+Q2'*y where Q2 is lower M-K rows of Q.
         */
        rmatrixlq(&cmatrix, k, m, &tau, _state);
        rmatrixlqunpackq(&cmatrix, k, m, &tau, m, &q, _state);
        for(i=0; i<=k-1; i++)
        {
            for(j=i+1; j<=m-1; j++)
            {
                cmatrix.ptr.pp_double[i][j] = 0.0;
            }
        }
        if( ae_fp_less(rmatrixlurcondinf(&cmatrix, k, _state),(double)1000*ae_machineepsilon) )
        {
            rep->terminationtype = -3;
            ae_frame_leave(_state);
            return;
        }
        ae_vector_set_length(&tmp, k, _state);
        for(i=0; i<=k-1; i++)
        {
            if( i>0 )
            {
                v = ae_v_dotproduct(&cmatrix.ptr.pp_double[i][0], 1, &tmp.ptr.p_double[0], 1, ae_v_len(0,i-1));
            }
            else
            {
                v = (double)(0);
            }
            tmp.ptr.p_double[i] = (cmatrix.ptr.pp_double[i][m]-v)/cmatrix.ptr.pp_double[i][i];
        }
        ae_vector_set_length(&c0, m, _state);
        for(i=0; i<=m-1; i++)
        {
            c0.ptr.p_double[i] = (double)(0);
        }
        for(i=0; i<=k-1; i++)
        {
            v = tmp.ptr.p_double[i];
            ae_v_addd(&c0.ptr.p_double[0], 1, &q.ptr.pp_double[i][0], 1, ae_v_len(0,m-1), v);
        }
        
        /*
         * Second, prepare modified matrix F2 = F*Q2' and solve modified task
         */
        ae_vector_set_length(&tmp, ae_maxint(n, m, _state)+1, _state);
        ae_matrix_set_length(&f2, n, m-k, _state);
        matrixvectormultiply(fmatrix, 0, n-1, 0, m-1, ae_false, &c0, 0, m-1, -1.0, &y, 0, n-1, 1.0, _state);
        rmatrixgemm(n, m-k, m, 1.0, fmatrix, 0, 0, 0, &q, k, 0, 1, 0.0, &f2, 0, 0, _state);
        lsfit_lsfitlinearinternal(&y, w, &f2, n, m-k, &tmp, rep, _state);
        rep->taskrcond = (double)(-1);
        if( rep->terminationtype<=0 )
        {
            ae_frame_leave(_state);
            return;
        }
        
        /*
         * then, convert back to original answer: C = C0 + Q2'*Y0
         */
        ae_vector_set_length(c, m, _state);
        ae_v_move(&c->ptr.p_double[0], 1, &c0.ptr.p_double[0], 1, ae_v_len(0,m-1));
        matrixvectormultiply(&q, k, m-1, 0, m-1, ae_true, &tmp, 0, m-k-1, 1.0, c, 0, m-1, 1.0, _state);
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Linear least squares fitting.

QR decomposition is used to reduce task to MxM, then triangular solver  or
SVD-based solver is used depending on condition number of the  system.  It
allows to maximize speed and retain decent accuracy.

IMPORTANT: if you want to perform  polynomial  fitting,  it  may  be  more
           convenient to use PolynomialFit() function. This function gives
           best  results  on  polynomial  problems  and  solves  numerical
           stability  issues  which  arise  when   you   fit   high-degree
           polynomials to your data.

INPUT PARAMETERS:
    Y       -   array[0..N-1] Function values in  N  points.
    FMatrix -   a table of basis functions values, array[0..N-1, 0..M-1].
                FMatrix[I, J] - value of J-th basis function in I-th point.
    N       -   number of points used. N>=1.
    M       -   number of basis functions, M>=1.

OUTPUT PARAMETERS:
    C       -   decomposition coefficients, array[0..M-1]
    Rep     -   fitting report. Following fields are set:
                * Rep.TerminationType is a completion code, always set  to
                  1 which denotes success
                * Rep.TaskRCond     reciprocal of condition number
                * R2                non-adjusted coefficient of determination
                                    (non-weighted)
                * RMSError          rms error on the (X,Y).
                * AvgError          average error on the (X,Y).
                * AvgRelError       average relative error on the non-zero Y
                * MaxError          maximum error
                                    NON-WEIGHTED ERRORS ARE CALCULATED
                
ERRORS IN PARAMETERS                
                
This  solver  also  calculates different kinds of errors in parameters and
fills corresponding fields of report:
* Rep.CovPar        covariance matrix for parameters, array[K,K].
* Rep.ErrPar        errors in parameters, array[K],
                    errpar = sqrt(diag(CovPar))
* Rep.ErrCurve      vector of fit errors - standard deviations of empirical
                    best-fit curve from "ideal" best-fit curve built  with
                    infinite number of samples, array[N].
                    errcurve = sqrt(diag(F*CovPar*F')),
                    where F is functions matrix.
* Rep.Noise         vector of per-point estimates of noise, array[N]
            
NOTE:       noise in the data is estimated as follows:
            * for fitting without user-supplied  weights  all  points  are
              assumed to have same level of noise, which is estimated from
              the data
            * for fitting with user-supplied weights we assume that  noise
              level in I-th point is inversely proportional to Ith weight.
              Coefficient of proportionality is estimated from the data.
            
NOTE:       we apply small amount of regularization when we invert squared
            Jacobian and calculate covariance matrix. It  guarantees  that
            algorithm won't divide by zero  during  inversion,  but  skews
            error estimates a bit (fractional error is about 10^-9).
            
            However, we believe that this difference is insignificant  for
            all practical purposes except for the situation when you  want
            to compare ALGLIB results with "reference"  implementation  up
            to the last significant digit.
            
NOTE:       covariance matrix is estimated using  correction  for  degrees
            of freedom (covariances are divided by N-M instead of dividing
            by N).

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

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

  -- ALGLIB --
     Copyright 17.08.2009 by Bochkanov Sergey
*************************************************************************/
void lsfitlinear(/* Real    */ const ae_vector* y,
     /* Real    */ const ae_matrix* fmatrix,
     ae_int_t n,
     ae_int_t m,
     /* Real    */ ae_vector* c,
     lsfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector w;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&w, 0, sizeof(w));
    ae_vector_clear(c);
    _lsfitreport_clear(rep);
    ae_vector_init(&w, 0, DT_REAL, _state, ae_true);

    ae_assert(n>=1, "LSFitLinear: N<1!", _state);
    ae_assert(m>=1, "LSFitLinear: M<1!", _state);
    ae_assert(y->cnt>=n, "LSFitLinear: length(Y)<N!", _state);
    ae_assert(isfinitevector(y, n, _state), "LSFitLinear: Y contains infinite or NaN values!", _state);
    ae_assert(fmatrix->rows>=n, "LSFitLinear: rows(FMatrix)<N!", _state);
    ae_assert(fmatrix->cols>=m, "LSFitLinear: cols(FMatrix)<M!", _state);
    ae_assert(apservisfinitematrix(fmatrix, n, m, _state), "LSFitLinear: FMatrix contains infinite or NaN values!", _state);
    ae_vector_set_length(&w, n, _state);
    for(i=0; i<=n-1; i++)
    {
        w.ptr.p_double[i] = (double)(1);
    }
    lsfit_lsfitlinearinternal(y, &w, fmatrix, n, m, c, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Constained linear least squares fitting.

This  is  variation  of LSFitLinear(),  which searchs for min|A*x=b| given
that  K  additional  constaints  C*x=bc are satisfied. It reduces original
task to modified one: min|B*y-d| WITHOUT constraints,  then  LSFitLinear()
is called.

IMPORTANT: if you want to perform  polynomial  fitting,  it  may  be  more
           convenient to use PolynomialFit() function. This function gives
           best  results  on  polynomial  problems  and  solves  numerical
           stability  issues  which  arise  when   you   fit   high-degree
           polynomials to your data.

INPUT PARAMETERS:
    Y       -   array[0..N-1] Function values in  N  points.
    FMatrix -   a table of basis functions values, array[0..N-1, 0..M-1].
                FMatrix[I,J] - value of J-th basis function in I-th point.
    CMatrix -   a table of constaints, array[0..K-1,0..M].
                I-th row of CMatrix corresponds to I-th linear constraint:
                CMatrix[I,0]*C[0] + ... + CMatrix[I,M-1]*C[M-1] = CMatrix[I,M]
    N       -   number of points used. N>=1.
    M       -   number of basis functions, M>=1.
    K       -   number of constraints, 0 <= K < M
                K=0 corresponds to absence of constraints.

OUTPUT PARAMETERS:
    C       -   decomposition coefficients, array[0..M-1]
    Rep     -   fitting report. Following fields are set:
                * Rep.TerminationType is a completion code:
                  * set to  1 on success
                  * set to -3 on failure due to  problematic  constraints:
                    either too many  constraints (M or  more),  degenerate
                    constraints (some constraints are repetead  twice)  or
                    inconsistent constraints are specified
                * R2                non-adjusted coefficient of determination
                                    (non-weighted)
                * RMSError          rms error on the (X,Y).
                * AvgError          average error on the (X,Y).
                * AvgRelError       average relative error on the non-zero Y
                * MaxError          maximum error
                                    NON-WEIGHTED ERRORS ARE CALCULATED

IMPORTANT:
    this subroitine doesn't calculate task's condition number for K<>0.
                
ERRORS IN PARAMETERS                
                
This  solver  also  calculates different kinds of errors in parameters and
fills corresponding fields of report:
* Rep.CovPar        covariance matrix for parameters, array[K,K].
* Rep.ErrPar        errors in parameters, array[K],
                    errpar = sqrt(diag(CovPar))
* Rep.ErrCurve      vector of fit errors - standard deviations of empirical
                    best-fit curve from "ideal" best-fit curve built  with
                    infinite number of samples, array[N].
                    errcurve = sqrt(diag(F*CovPar*F')),
                    where F is functions matrix.
* Rep.Noise         vector of per-point estimates of noise, array[N]

IMPORTANT:  errors  in  parameters  are  calculated  without  taking  into
            account boundary/linear constraints! Presence  of  constraints
            changes distribution of errors, but there is no  easy  way  to
            account for constraints when you calculate covariance matrix.
            
NOTE:       noise in the data is estimated as follows:
            * for fitting without user-supplied  weights  all  points  are
              assumed to have same level of noise, which is estimated from
              the data
            * for fitting with user-supplied weights we assume that  noise
              level in I-th point is inversely proportional to Ith weight.
              Coefficient of proportionality is estimated from the data.
            
NOTE:       we apply small amount of regularization when we invert squared
            Jacobian and calculate covariance matrix. It  guarantees  that
            algorithm won't divide by zero  during  inversion,  but  skews
            error estimates a bit (fractional error is about 10^-9).
            
            However, we believe that this difference is insignificant  for
            all practical purposes except for the situation when you  want
            to compare ALGLIB results with "reference"  implementation  up
            to the last significant digit.
            
NOTE:       covariance matrix is estimated using  correction  for  degrees
            of freedom (covariances are divided by N-M instead of dividing
            by N).

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

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

  -- ALGLIB --
     Copyright 07.09.2009 by Bochkanov Sergey
*************************************************************************/
void lsfitlinearc(/* Real    */ const ae_vector* _y,
     /* Real    */ const ae_matrix* fmatrix,
     /* Real    */ const ae_matrix* cmatrix,
     ae_int_t n,
     ae_int_t m,
     ae_int_t k,
     /* Real    */ ae_vector* c,
     lsfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector y;
    ae_vector w;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&y, 0, sizeof(y));
    memset(&w, 0, sizeof(w));
    ae_vector_init_copy(&y, _y, _state, ae_true);
    ae_vector_clear(c);
    _lsfitreport_clear(rep);
    ae_vector_init(&w, 0, DT_REAL, _state, ae_true);

    ae_assert(n>=1, "LSFitLinearC: N<1!", _state);
    ae_assert(m>=1, "LSFitLinearC: M<1!", _state);
    ae_assert(k>=0, "LSFitLinearC: K<0!", _state);
    ae_assert(y.cnt>=n, "LSFitLinearC: length(Y)<N!", _state);
    ae_assert(isfinitevector(&y, n, _state), "LSFitLinearC: Y contains infinite or NaN values!", _state);
    ae_assert(fmatrix->rows>=n, "LSFitLinearC: rows(FMatrix)<N!", _state);
    ae_assert(fmatrix->cols>=m, "LSFitLinearC: cols(FMatrix)<M!", _state);
    ae_assert(apservisfinitematrix(fmatrix, n, m, _state), "LSFitLinearC: FMatrix contains infinite or NaN values!", _state);
    ae_assert(cmatrix->rows>=k, "LSFitLinearC: rows(CMatrix)<K!", _state);
    ae_assert(cmatrix->cols>=m+1||k==0, "LSFitLinearC: cols(CMatrix)<M+1!", _state);
    ae_assert(apservisfinitematrix(cmatrix, k, m+1, _state), "LSFitLinearC: CMatrix contains infinite or NaN values!", _state);
    ae_vector_set_length(&w, n, _state);
    for(i=0; i<=n-1; i++)
    {
        w.ptr.p_double[i] = (double)(1);
    }
    lsfitlinearwc(&y, &w, fmatrix, cmatrix, n, m, k, c, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Weighted nonlinear least squares fitting using function values only.

Combination of numerical differentiation and secant updates is used to
obtain function Jacobian.

Nonlinear task min(F(c)) is solved, where

    F(c) = (w[0]*(f(c,x[0])-y[0]))^2 + ... + (w[n-1]*(f(c,x[n-1])-y[n-1]))^2,

    * N is a number of points,
    * M is a dimension of a space points belong to,
    * K is a dimension of a space of parameters being fitted,
    * w is an N-dimensional vector of weight coefficients,
    * x is a set of N points, each of them is an M-dimensional vector,
    * c is a K-dimensional vector of parameters being fitted

This subroutine uses only f(c,x[i]).

INPUT PARAMETERS:
    X       -   array[0..N-1,0..M-1], points (one row = one point)
    Y       -   array[0..N-1], function values.
    W       -   weights, array[0..N-1]
    C       -   array[0..K-1], initial approximation to the solution,
    N       -   number of points, N>1
    M       -   dimension of space
    K       -   number of parameters being fitted
    DiffStep-   numerical differentiation step;
                should not be very small or large;
                large = loss of accuracy
                small = growth of round-off errors

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

IMPORTANT: the LSFIT optimizer  supports  parallel  model  evaluation  and
           parallel numerical  differentiation  ('callback  parallelism').
           This feature, which is present in commercial  ALGLIB  editions,
           greatly accelerates fits with large datasets  and/or  expensive
           target functions.
           
           Callback parallelism is usually beneficial when a  single  pass
           over  the   entire   dataset   requires   more   than   several
           milliseconds.
           
           See ALGLIB Reference Manual, 'Working with commercial  version'
           section,  and  comments  on  lsfitfit()   function   for   more
           information.

  -- ALGLIB --
     Copyright 18.10.2008 by Bochkanov Sergey
*************************************************************************/
void lsfitcreatewf(/* Real    */ const ae_matrix* x,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* w,
     /* Real    */ const ae_vector* c,
     ae_int_t n,
     ae_int_t m,
     ae_int_t k,
     double diffstep,
     lsfitstate* state,
     ae_state *_state)
{
    ae_int_t i;

    _lsfitstate_clear(state);

    ae_assert(n>=1, "LSFitCreateWF: N<1!", _state);
    ae_assert(m>=1, "LSFitCreateWF: M<1!", _state);
    ae_assert(k>=1, "LSFitCreateWF: K<1!", _state);
    ae_assert(c->cnt>=k, "LSFitCreateWF: length(C)<K!", _state);
    ae_assert(isfinitevector(c, k, _state), "LSFitCreateWF: C contains infinite or NaN values!", _state);
    ae_assert(y->cnt>=n, "LSFitCreateWF: length(Y)<N!", _state);
    ae_assert(isfinitevector(y, n, _state), "LSFitCreateWF: Y contains infinite or NaN values!", _state);
    ae_assert(w->cnt>=n, "LSFitCreateWF: length(W)<N!", _state);
    ae_assert(isfinitevector(w, n, _state), "LSFitCreateWF: W contains infinite or NaN values!", _state);
    ae_assert(x->rows>=n, "LSFitCreateWF: rows(X)<N!", _state);
    ae_assert(x->cols>=m, "LSFitCreateWF: cols(X)<M!", _state);
    ae_assert(apservisfinitematrix(x, n, m, _state), "LSFitCreateWF: X contains infinite or NaN values!", _state);
    ae_assert(ae_isfinite(diffstep, _state), "LSFitCreateWF: DiffStep is not finite!", _state);
    ae_assert(ae_fp_greater(diffstep,(double)(0)), "LSFitCreateWF: DiffStep<=0!", _state);
    state->protocolversion = 1;
    state->teststep = (double)(0);
    state->diffstep = diffstep;
    state->npoints = n;
    state->nweights = n;
    state->wkind = 1;
    state->m = m;
    state->k = k;
    lsfitsetcond(state, 0.0, 0, _state);
    lsfitsetstpmax(state, 0.0, _state);
    lsfitsetxrep(state, ae_false, _state);
    ae_matrix_set_length(&state->taskx, n, m, _state);
    ae_vector_set_length(&state->tasky, n, _state);
    ae_vector_set_length(&state->taskw, n, _state);
    ae_vector_set_length(&state->c, k, _state);
    ae_vector_set_length(&state->c0, k, _state);
    ae_vector_set_length(&state->c1, k, _state);
    ae_v_move(&state->c0.ptr.p_double[0], 1, &c->ptr.p_double[0], 1, ae_v_len(0,k-1));
    ae_v_move(&state->c1.ptr.p_double[0], 1, &c->ptr.p_double[0], 1, ae_v_len(0,k-1));
    ae_vector_set_length(&state->x, m, _state);
    ae_v_move(&state->taskw.ptr.p_double[0], 1, &w->ptr.p_double[0], 1, ae_v_len(0,n-1));
    for(i=0; i<=n-1; i++)
    {
        ae_v_move(&state->taskx.ptr.pp_double[i][0], 1, &x->ptr.pp_double[i][0], 1, ae_v_len(0,m-1));
        state->tasky.ptr.p_double[i] = y->ptr.p_double[i];
    }
    ae_vector_set_length(&state->s, k, _state);
    ae_vector_set_length(&state->bndl, k, _state);
    ae_vector_set_length(&state->bndu, k, _state);
    for(i=0; i<=k-1; i++)
    {
        state->s.ptr.p_double[i] = 1.0;
        state->bndl.ptr.p_double[i] = _state->v_neginf;
        state->bndu.ptr.p_double[i] = _state->v_posinf;
    }
    state->optalgo = 0;
    state->prevnpt = -1;
    state->prevalgo = -1;
    state->nec = 0;
    state->nic = 0;
    minlmcreatev(k, n, &state->c0, diffstep, &state->optstate, _state);
    lsfit_lsfitclearrequestfields(state, _state);
    ae_vector_set_length(&state->rstate.ia, 6+1, _state);
    ae_vector_set_length(&state->rstate.ra, 10+1, _state);
    state->rstate.stage = -1;
}


/*************************************************************************
Nonlinear least squares fitting using function values only.

Combination of numerical differentiation and secant updates is used to
obtain function Jacobian.

Nonlinear task min(F(c)) is solved, where

    F(c) = (f(c,x[0])-y[0])^2 + ... + (f(c,x[n-1])-y[n-1])^2,

    * N is a number of points,
    * M is a dimension of a space points belong to,
    * K is a dimension of a space of parameters being fitted,
    * w is an N-dimensional vector of weight coefficients,
    * x is a set of N points, each of them is an M-dimensional vector,
    * c is a K-dimensional vector of parameters being fitted

This subroutine uses only f(c,x[i]).

INPUT PARAMETERS:
    X       -   array[0..N-1,0..M-1], points (one row = one point)
    Y       -   array[0..N-1], function values.
    C       -   array[0..K-1], initial approximation to the solution,
    N       -   number of points, N>1
    M       -   dimension of space
    K       -   number of parameters being fitted
    DiffStep-   numerical differentiation step;
                should not be very small or large;
                large = loss of accuracy
                small = growth of round-off errors

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

IMPORTANT: the LSFIT optimizer  supports  parallel  model  evaluation  and
           parallel numerical  differentiation  ('callback  parallelism').
           This feature, which is present in commercial  ALGLIB  editions,
           greatly accelerates fits with large datasets  and/or  expensive
           target functions.
           
           Callback parallelism is usually beneficial when a  single  pass
           over  the   entire   dataset   requires   more   than   several
           milliseconds.
           
           See ALGLIB Reference Manual, 'Working with commercial  version'
           section,  and  comments  on  lsfitfit()   function   for   more
           information.
           
  -- ALGLIB --
     Copyright 18.10.2008 by Bochkanov Sergey
*************************************************************************/
void lsfitcreatef(/* Real    */ const ae_matrix* x,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* c,
     ae_int_t n,
     ae_int_t m,
     ae_int_t k,
     double diffstep,
     lsfitstate* state,
     ae_state *_state)
{
    ae_int_t i;

    _lsfitstate_clear(state);

    ae_assert(n>=1, "LSFitCreateF: N<1!", _state);
    ae_assert(m>=1, "LSFitCreateF: M<1!", _state);
    ae_assert(k>=1, "LSFitCreateF: K<1!", _state);
    ae_assert(c->cnt>=k, "LSFitCreateF: length(C)<K!", _state);
    ae_assert(isfinitevector(c, k, _state), "LSFitCreateF: C contains infinite or NaN values!", _state);
    ae_assert(y->cnt>=n, "LSFitCreateF: length(Y)<N!", _state);
    ae_assert(isfinitevector(y, n, _state), "LSFitCreateF: Y contains infinite or NaN values!", _state);
    ae_assert(x->rows>=n, "LSFitCreateF: rows(X)<N!", _state);
    ae_assert(x->cols>=m, "LSFitCreateF: cols(X)<M!", _state);
    ae_assert(apservisfinitematrix(x, n, m, _state), "LSFitCreateF: X contains infinite or NaN values!", _state);
    ae_assert(x->rows>=n, "LSFitCreateF: rows(X)<N!", _state);
    ae_assert(x->cols>=m, "LSFitCreateF: cols(X)<M!", _state);
    ae_assert(apservisfinitematrix(x, n, m, _state), "LSFitCreateF: X contains infinite or NaN values!", _state);
    ae_assert(ae_isfinite(diffstep, _state), "LSFitCreateF: DiffStep is not finite!", _state);
    ae_assert(ae_fp_greater(diffstep,(double)(0)), "LSFitCreateF: DiffStep<=0!", _state);
    state->protocolversion = 1;
    state->teststep = (double)(0);
    state->diffstep = diffstep;
    state->npoints = n;
    state->wkind = 0;
    state->m = m;
    state->k = k;
    lsfitsetcond(state, 0.0, 0, _state);
    lsfitsetstpmax(state, 0.0, _state);
    lsfitsetxrep(state, ae_false, _state);
    ae_matrix_set_length(&state->taskx, n, m, _state);
    ae_vector_set_length(&state->tasky, n, _state);
    ae_vector_set_length(&state->c, k, _state);
    ae_vector_set_length(&state->c0, k, _state);
    ae_vector_set_length(&state->c1, k, _state);
    ae_v_move(&state->c0.ptr.p_double[0], 1, &c->ptr.p_double[0], 1, ae_v_len(0,k-1));
    ae_v_move(&state->c1.ptr.p_double[0], 1, &c->ptr.p_double[0], 1, ae_v_len(0,k-1));
    ae_vector_set_length(&state->x, m, _state);
    for(i=0; i<=n-1; i++)
    {
        ae_v_move(&state->taskx.ptr.pp_double[i][0], 1, &x->ptr.pp_double[i][0], 1, ae_v_len(0,m-1));
        state->tasky.ptr.p_double[i] = y->ptr.p_double[i];
    }
    ae_vector_set_length(&state->s, k, _state);
    ae_vector_set_length(&state->bndl, k, _state);
    ae_vector_set_length(&state->bndu, k, _state);
    for(i=0; i<=k-1; i++)
    {
        state->s.ptr.p_double[i] = 1.0;
        state->bndl.ptr.p_double[i] = _state->v_neginf;
        state->bndu.ptr.p_double[i] = _state->v_posinf;
    }
    state->optalgo = 0;
    state->prevnpt = -1;
    state->prevalgo = -1;
    state->nec = 0;
    state->nic = 0;
    minlmcreatev(k, n, &state->c0, diffstep, &state->optstate, _state);
    lsfit_lsfitclearrequestfields(state, _state);
    ae_vector_set_length(&state->rstate.ia, 6+1, _state);
    ae_vector_set_length(&state->rstate.ra, 10+1, _state);
    state->rstate.stage = -1;
}


/*************************************************************************
Weighted nonlinear least squares fitting using gradient only.

Nonlinear task min(F(c)) is solved, where

    F(c) = (w[0]*(f(c,x[0])-y[0]))^2 + ... + (w[n-1]*(f(c,x[n-1])-y[n-1]))^2,
    
    * N is a number of points,
    * M is a dimension of a space points belong to,
    * K is a dimension of a space of parameters being fitted,
    * w is an N-dimensional vector of weight coefficients,
    * x is a set of N points, each of them is an M-dimensional vector,
    * c is a K-dimensional vector of parameters being fitted
    
This subroutine uses only f(c,x[i]) and its gradient.
    
INPUT PARAMETERS:
    X       -   array[0..N-1,0..M-1], points (one row = one point)
    Y       -   array[0..N-1], function values.
    W       -   weights, array[0..N-1]
    C       -   array[0..K-1], initial approximation to the solution,
    N       -   number of points, N>1
    M       -   dimension of space
    K       -   number of parameters being fitted

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

See also:
    LSFitResults
    LSFitCreateFG (fitting without weights)
    LSFitCreateWFGH (fitting using Hessian)

IMPORTANT: the LSFIT optimizer  supports  parallel  model  evaluation  and
           parallel numerical  differentiation  ('callback  parallelism').
           This feature, which is present in commercial  ALGLIB  editions,
           greatly accelerates fits with large datasets  and/or  expensive
           target functions.
           
           Callback parallelism is usually beneficial when a  single  pass
           over  the   entire   dataset   requires   more   than   several
           milliseconds.
           
           See ALGLIB Reference Manual, 'Working with commercial  version'
           section,  and  comments  on  lsfitfit()   function   for   more
           information.
           
  -- ALGLIB --
     Copyright 17.08.2009 by Bochkanov Sergey
*************************************************************************/
void lsfitcreatewfg(/* Real    */ const ae_matrix* x,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* w,
     /* Real    */ const ae_vector* c,
     ae_int_t n,
     ae_int_t m,
     ae_int_t k,
     lsfitstate* state,
     ae_state *_state)
{
    ae_int_t i;

    _lsfitstate_clear(state);

    ae_assert(n>=1, "LSFitCreateWFG: N<1!", _state);
    ae_assert(m>=1, "LSFitCreateWFG: M<1!", _state);
    ae_assert(k>=1, "LSFitCreateWFG: K<1!", _state);
    ae_assert(c->cnt>=k, "LSFitCreateWFG: length(C)<K!", _state);
    ae_assert(isfinitevector(c, k, _state), "LSFitCreateWFG: C contains infinite or NaN values!", _state);
    ae_assert(y->cnt>=n, "LSFitCreateWFG: length(Y)<N!", _state);
    ae_assert(isfinitevector(y, n, _state), "LSFitCreateWFG: Y contains infinite or NaN values!", _state);
    ae_assert(w->cnt>=n, "LSFitCreateWFG: length(W)<N!", _state);
    ae_assert(isfinitevector(w, n, _state), "LSFitCreateWFG: W contains infinite or NaN values!", _state);
    ae_assert(x->rows>=n, "LSFitCreateWFG: rows(X)<N!", _state);
    ae_assert(x->cols>=m, "LSFitCreateWFG: cols(X)<M!", _state);
    ae_assert(apservisfinitematrix(x, n, m, _state), "LSFitCreateWFG: X contains infinite or NaN values!", _state);
    state->protocolversion = 1;
    state->teststep = (double)(0);
    state->diffstep = (double)(0);
    state->npoints = n;
    state->nweights = n;
    state->wkind = 1;
    state->m = m;
    state->k = k;
    lsfitsetcond(state, 0.0, 0, _state);
    lsfitsetstpmax(state, 0.0, _state);
    lsfitsetxrep(state, ae_false, _state);
    ae_matrix_set_length(&state->taskx, n, m, _state);
    ae_vector_set_length(&state->tasky, n, _state);
    ae_vector_set_length(&state->taskw, n, _state);
    ae_vector_set_length(&state->c, k, _state);
    ae_vector_set_length(&state->c0, k, _state);
    ae_vector_set_length(&state->c1, k, _state);
    ae_v_move(&state->c0.ptr.p_double[0], 1, &c->ptr.p_double[0], 1, ae_v_len(0,k-1));
    ae_v_move(&state->c1.ptr.p_double[0], 1, &c->ptr.p_double[0], 1, ae_v_len(0,k-1));
    ae_vector_set_length(&state->x, m, _state);
    ae_vector_set_length(&state->g, k, _state);
    ae_v_move(&state->taskw.ptr.p_double[0], 1, &w->ptr.p_double[0], 1, ae_v_len(0,n-1));
    for(i=0; i<=n-1; i++)
    {
        ae_v_move(&state->taskx.ptr.pp_double[i][0], 1, &x->ptr.pp_double[i][0], 1, ae_v_len(0,m-1));
        state->tasky.ptr.p_double[i] = y->ptr.p_double[i];
    }
    ae_vector_set_length(&state->s, k, _state);
    ae_vector_set_length(&state->bndl, k, _state);
    ae_vector_set_length(&state->bndu, k, _state);
    for(i=0; i<=k-1; i++)
    {
        state->s.ptr.p_double[i] = 1.0;
        state->bndl.ptr.p_double[i] = _state->v_neginf;
        state->bndu.ptr.p_double[i] = _state->v_posinf;
    }
    state->optalgo = 1;
    state->prevnpt = -1;
    state->prevalgo = -1;
    state->nec = 0;
    state->nic = 0;
    minlmcreatevj(k, n, &state->c0, &state->optstate, _state);
    lsfit_lsfitclearrequestfields(state, _state);
    ae_vector_set_length(&state->rstate.ia, 6+1, _state);
    ae_vector_set_length(&state->rstate.ra, 10+1, _state);
    state->rstate.stage = -1;
}


/*************************************************************************
Nonlinear least squares fitting using gradient only, without individual
weights.

Nonlinear task min(F(c)) is solved, where

    F(c) = ((f(c,x[0])-y[0]))^2 + ... + ((f(c,x[n-1])-y[n-1]))^2,

    * N is a number of points,
    * M is a dimension of a space points belong to,
    * K is a dimension of a space of parameters being fitted,
    * x is a set of N points, each of them is an M-dimensional vector,
    * c is a K-dimensional vector of parameters being fitted

This subroutine uses only f(c,x[i]) and its gradient.

INPUT PARAMETERS:
    X       -   array[0..N-1,0..M-1], points (one row = one point)
    Y       -   array[0..N-1], function values.
    C       -   array[0..K-1], initial approximation to the solution,
    N       -   number of points, N>1
    M       -   dimension of space
    K       -   number of parameters being fitted

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

IMPORTANT: the LSFIT optimizer  supports  parallel  model  evaluation  and
           parallel numerical  differentiation  ('callback  parallelism').
           This feature, which is present in commercial  ALGLIB  editions,
           greatly accelerates fits with large datasets  and/or  expensive
           target functions.
           
           Callback parallelism is usually beneficial when a  single  pass
           over  the   entire   dataset   requires   more   than   several
           milliseconds.
           
           See ALGLIB Reference Manual, 'Working with commercial  version'
           section,  and  comments  on  lsfitfit()   function   for   more
           information.
           
  -- ALGLIB --
     Copyright 17.08.2009 by Bochkanov Sergey
*************************************************************************/
void lsfitcreatefg(/* Real    */ const ae_matrix* x,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* c,
     ae_int_t n,
     ae_int_t m,
     ae_int_t k,
     lsfitstate* state,
     ae_state *_state)
{
    ae_int_t i;

    _lsfitstate_clear(state);

    ae_assert(n>=1, "LSFitCreateFG: N<1!", _state);
    ae_assert(m>=1, "LSFitCreateFG: M<1!", _state);
    ae_assert(k>=1, "LSFitCreateFG: K<1!", _state);
    ae_assert(c->cnt>=k, "LSFitCreateFG: length(C)<K!", _state);
    ae_assert(isfinitevector(c, k, _state), "LSFitCreateFG: C contains infinite or NaN values!", _state);
    ae_assert(y->cnt>=n, "LSFitCreateFG: length(Y)<N!", _state);
    ae_assert(isfinitevector(y, n, _state), "LSFitCreateFG: Y contains infinite or NaN values!", _state);
    ae_assert(x->rows>=n, "LSFitCreateFG: rows(X)<N!", _state);
    ae_assert(x->cols>=m, "LSFitCreateFG: cols(X)<M!", _state);
    ae_assert(apservisfinitematrix(x, n, m, _state), "LSFitCreateFG: X contains infinite or NaN values!", _state);
    ae_assert(x->rows>=n, "LSFitCreateFG: rows(X)<N!", _state);
    ae_assert(x->cols>=m, "LSFitCreateFG: cols(X)<M!", _state);
    ae_assert(apservisfinitematrix(x, n, m, _state), "LSFitCreateFG: X contains infinite or NaN values!", _state);
    state->protocolversion = 1;
    state->teststep = (double)(0);
    state->diffstep = (double)(0);
    state->npoints = n;
    state->wkind = 0;
    state->m = m;
    state->k = k;
    lsfitsetcond(state, 0.0, 0, _state);
    lsfitsetstpmax(state, 0.0, _state);
    lsfitsetxrep(state, ae_false, _state);
    ae_matrix_set_length(&state->taskx, n, m, _state);
    ae_vector_set_length(&state->tasky, n, _state);
    ae_vector_set_length(&state->c, k, _state);
    ae_vector_set_length(&state->c0, k, _state);
    ae_vector_set_length(&state->c1, k, _state);
    ae_v_move(&state->c0.ptr.p_double[0], 1, &c->ptr.p_double[0], 1, ae_v_len(0,k-1));
    ae_v_move(&state->c1.ptr.p_double[0], 1, &c->ptr.p_double[0], 1, ae_v_len(0,k-1));
    ae_vector_set_length(&state->x, m, _state);
    ae_vector_set_length(&state->g, k, _state);
    for(i=0; i<=n-1; i++)
    {
        ae_v_move(&state->taskx.ptr.pp_double[i][0], 1, &x->ptr.pp_double[i][0], 1, ae_v_len(0,m-1));
        state->tasky.ptr.p_double[i] = y->ptr.p_double[i];
    }
    ae_vector_set_length(&state->s, k, _state);
    ae_vector_set_length(&state->bndl, k, _state);
    ae_vector_set_length(&state->bndu, k, _state);
    for(i=0; i<=k-1; i++)
    {
        state->s.ptr.p_double[i] = 1.0;
        state->bndl.ptr.p_double[i] = _state->v_neginf;
        state->bndu.ptr.p_double[i] = _state->v_posinf;
    }
    state->optalgo = 1;
    state->prevnpt = -1;
    state->prevalgo = -1;
    state->nec = 0;
    state->nic = 0;
    minlmcreatevj(k, n, &state->c0, &state->optstate, _state);
    lsfit_lsfitclearrequestfields(state, _state);
    ae_vector_set_length(&state->rstate.ia, 6+1, _state);
    ae_vector_set_length(&state->rstate.ra, 10+1, _state);
    state->rstate.stage = -1;
}


/*************************************************************************
Stopping conditions for nonlinear least squares fitting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - ste pvector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by LSFitSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations   is    unlimited.   Only   Levenberg-Marquardt
                iterations  are  counted  (L-BFGS/CG  iterations  are  NOT
                counted because their cost is very low compared to that of
                LM).

NOTE

Passing EpsX=0  and  MaxIts=0  (simultaneously)  will  lead  to  automatic
stopping criterion selection (according to the scheme used by MINLM unit).


  -- ALGLIB --
     Copyright 17.08.2009 by Bochkanov Sergey
*************************************************************************/
void lsfitsetcond(lsfitstate* state,
     double epsx,
     ae_int_t maxits,
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsx, _state), "LSFitSetCond: EpsX is not finite!", _state);
    ae_assert(ae_fp_greater_eq(epsx,(double)(0)), "LSFitSetCond: negative EpsX!", _state);
    ae_assert(maxits>=0, "LSFitSetCond: negative MaxIts!", _state);
    state->epsx = epsx;
    state->maxits = maxits;
}


/*************************************************************************
This function sets maximum step length

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0,  if you don't
                want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  leads  to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

NOTE: non-zero StpMax leads to moderate  performance  degradation  because
intermediate  step  of  preconditioned L-BFGS optimization is incompatible
with limits on step size.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void lsfitsetstpmax(lsfitstate* state, double stpmax, ae_state *_state)
{


    ae_assert(ae_fp_greater_eq(stpmax,(double)(0)), "LSFitSetStpMax: StpMax<0!", _state);
    state->stpmax = stpmax;
}


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not
    
When reports are needed, State.C (current parameters) and State.F (current
value of fitting function) are reported.


  -- ALGLIB --
     Copyright 15.08.2010 by Bochkanov Sergey
*************************************************************************/
void lsfitsetxrep(lsfitstate* state, ae_bool needxrep, ae_state *_state)
{


    state->xrep = needxrep;
}


/*************************************************************************
This function sets scaling coefficients for underlying optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Generally, scale is NOT considered to be a form of preconditioner.  But LM
optimizer is unique in that it uses scaling matrix both  in  the  stopping
condition tests and as Marquardt damping factor.

Proper scaling is very important for the algorithm performance. It is less
important for the quality of results, but still has some influence (it  is
easier  to  converge  when  variables  are  properly  scaled, so premature
stopping is possible when very badly scalled variables are  combined  with
relaxed stopping conditions).

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void lsfitsetscale(lsfitstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(s->cnt>=state->k, "LSFitSetScale: Length(S)<K", _state);
    for(i=0; i<=state->k-1; i++)
    {
        ae_assert(ae_isfinite(s->ptr.p_double[i], _state), "LSFitSetScale: S contains infinite or NAN elements", _state);
        ae_assert(ae_fp_neq(s->ptr.p_double[i],(double)(0)), "LSFitSetScale: S contains infinite or NAN elements", _state);
        state->s.ptr.p_double[i] = ae_fabs(s->ptr.p_double[i], _state);
    }
}


/*************************************************************************
This function sets boundary constraints for underlying optimizer

Boundary constraints are inactive by default (after initial creation).
They are preserved until explicitly turned off with another SetBC() call.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[K].
                If some (all) variables are unbounded, you may specify
                very small number or -INF (latter is recommended because
                it will allow solver to use better algorithm).
    BndU    -   upper bounds, array[K].
                If some (all) variables are unbounded, you may specify
                very large number or +INF (latter is recommended because
                it will allow solver to use better algorithm).

NOTE 1: it is possible to specify BndL[i]=BndU[i]. In this case I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

NOTE 2: unlike other constrained optimization algorithms, this solver  has
following useful properties:
* bound constraints are always satisfied exactly
* function is evaluated only INSIDE area specified by bound constraints

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void lsfitsetbc(lsfitstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;


    k = state->k;
    ae_assert(bndl->cnt>=k, "LSFitSetBC: Length(BndL)<K", _state);
    ae_assert(bndu->cnt>=k, "LSFitSetBC: Length(BndU)<K", _state);
    for(i=0; i<=k-1; i++)
    {
        ae_assert(ae_isfinite(bndl->ptr.p_double[i], _state)||ae_isneginf(bndl->ptr.p_double[i], _state), "LSFitSetBC: BndL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(bndu->ptr.p_double[i], _state)||ae_isposinf(bndu->ptr.p_double[i], _state), "LSFitSetBC: BndU contains NAN or -INF", _state);
        if( ae_isfinite(bndl->ptr.p_double[i], _state)&&ae_isfinite(bndu->ptr.p_double[i], _state) )
        {
            ae_assert(ae_fp_less_eq(bndl->ptr.p_double[i],bndu->ptr.p_double[i]), "LSFitSetBC: BndL[i]>BndU[i]", _state);
        }
        state->bndl.ptr.p_double[i] = bndl->ptr.p_double[i];
        state->bndu.ptr.p_double[i] = bndu->ptr.p_double[i];
    }
}


/*************************************************************************
This function sets linear constraints for underlying optimizer

Linear constraints are inactive by default (after initial creation).
They are preserved until explicitly turned off with another SetLC() call.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    C       -   linear constraints, array[K,N+1].
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n+1]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n+1]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n+1]
    K       -   number of equality/inequality constraints, K>=0:
                * if given, only leading K elements of C/CT are used
                * if not given, automatically determined from sizes of C/CT

IMPORTANT: if you have linear constraints, it is strongly  recommended  to
           set scale of variables with lsfitsetscale(). QP solver which is
           used to calculate linearly constrained steps heavily relies  on
           good scaling of input problems.
           
NOTE: linear  (non-box)  constraints  are  satisfied only approximately  -
      there  always  exists some violation due  to  numerical  errors  and
      algorithmic limitations.

NOTE: general linear constraints  add  significant  overhead  to  solution
      process. Although solver performs roughly same amount of  iterations
      (when compared  with  similar  box-only  constrained  problem), each
      iteration   now    involves  solution  of  linearly  constrained  QP
      subproblem, which requires ~3-5 times more Cholesky  decompositions.
      Thus, if you can reformulate your problem in such way  this  it  has
      only box constraints, it may be beneficial to do so.

  -- ALGLIB --
     Copyright 29.04.2017 by Bochkanov Sergey
*************************************************************************/
void lsfitsetlc(lsfitstate* state,
     /* Real    */ const ae_matrix* c,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->k;
    
    /*
     * First, check for errors in the inputs
     */
    ae_assert(k>=0, "LSFitSetLC: K<0", _state);
    ae_assert(c->cols>=n+1||k==0, "LSFitSetLC: Cols(C)<N+1", _state);
    ae_assert(c->rows>=k, "LSFitSetLC: Rows(C)<K", _state);
    ae_assert(ct->cnt>=k, "LSFitSetLC: Length(CT)<K", _state);
    ae_assert(apservisfinitematrix(c, k, n+1, _state), "LSFitSetLC: C contains infinite or NaN values!", _state);
    
    /*
     * Handle zero K
     */
    if( k==0 )
    {
        state->nec = 0;
        state->nic = 0;
        return;
    }
    
    /*
     * Equality constraints are stored first, in the upper
     * NEC rows of State.CLEIC matrix. Inequality constraints
     * are stored in the next NIC rows.
     *
     * NOTE: we convert inequality constraints to the form
     * A*x<=b before copying them.
     */
    rmatrixsetlengthatleast(&state->cleic, k, n+1, _state);
    state->nec = 0;
    state->nic = 0;
    for(i=0; i<=k-1; i++)
    {
        if( ct->ptr.p_int[i]==0 )
        {
            ae_v_move(&state->cleic.ptr.pp_double[state->nec][0], 1, &c->ptr.pp_double[i][0], 1, ae_v_len(0,n));
            state->nec = state->nec+1;
        }
    }
    for(i=0; i<=k-1; i++)
    {
        if( ct->ptr.p_int[i]!=0 )
        {
            if( ct->ptr.p_int[i]>0 )
            {
                ae_v_moveneg(&state->cleic.ptr.pp_double[state->nec+state->nic][0], 1, &c->ptr.pp_double[i][0], 1, ae_v_len(0,n));
            }
            else
            {
                ae_v_move(&state->cleic.ptr.pp_double[state->nec+state->nic][0], 1, &c->ptr.pp_double[i][0], 1, ae_v_len(0,n));
            }
            state->nic = state->nic+1;
        }
    }
}


/*************************************************************************

CALLBACK PARALLELISM:

The  LSFIT  optimizer  supports  parallel  model  evaluation  and parallel
numerical differentiation ('callback parallelism'). This feature, which is
present in commercial ALGLIB editions, greatly accelerates fits with large
datasets and/or expensive target functions.

Callback parallelism is usually beneficial when a  single  pass  over  the
entire dataset requires more than several milliseconds. In this  case  the
job of computing model values at  dataset  points  can  be  split  between
multiple threads.

If you employ a numerical differentiation scheme, you can also parallelize
computation of different components of a numerical gradient. Generally, the
mode computationally demanding your problem is (many points, numerical differentiation,
expensive model), the more you can get for multithreading.

ALGLIB Reference Manual, 'Working with commercial  version' section,
describes how to activate callback parallelism for your programming language.

CALLBACK ARGUMENTS

This algorithm is somewhat unusual because  it  works  with  parameterized
function f(C,X), where X is  a  function  argument (we  have  many  points
which  are  characterized  by different  argument  values),  and  C  is  a
parameter to fit.

For example, if we want to do linear  fit  by  f(c0,c1,x) = c0*x+c1,  then
x will be argument, and {c0,c1} will be parameters.

It is important to understand that this algorithm finds   minimum  in  the
space of function PARAMETERS (not  arguments),  so  it  needs  derivatives
of f() with respect to C, not X.

In the example above it will need f=c0*x+c1 and {df/dc0,df/dc1} = {x,1}
instead of {df/dx} = {c0}.

  -- ALGLIB --
     Copyright 17.12.2023 by Bochkanov Sergey
*************************************************************************/
ae_bool lsfititeration(lsfitstate* state, ae_state *_state)
{
    double lx;
    double lf;
    double ld;
    double rx;
    double rf;
    double rd;
    ae_int_t n;
    ae_int_t m;
    ae_int_t k;
    double v;
    double vv;
    double v1;
    double v2;
    double relcnt;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j1;
    ae_int_t offs;
    ae_bool result;


    
    /*
     * Reverse communication preparations
     * I know it looks ugly, but it works the same way
     * anywhere from C++ to Python.
     *
     * This code initializes locals by:
     * * random values determined during code
     *   generation - on first subroutine call
     * * values from previous call - on subsequent calls
     */
    if( state->rstate.stage>=0 )
    {
        n = state->rstate.ia.ptr.p_int[0];
        m = state->rstate.ia.ptr.p_int[1];
        k = state->rstate.ia.ptr.p_int[2];
        i = state->rstate.ia.ptr.p_int[3];
        j = state->rstate.ia.ptr.p_int[4];
        j1 = state->rstate.ia.ptr.p_int[5];
        offs = state->rstate.ia.ptr.p_int[6];
        lx = state->rstate.ra.ptr.p_double[0];
        lf = state->rstate.ra.ptr.p_double[1];
        ld = state->rstate.ra.ptr.p_double[2];
        rx = state->rstate.ra.ptr.p_double[3];
        rf = state->rstate.ra.ptr.p_double[4];
        rd = state->rstate.ra.ptr.p_double[5];
        v = state->rstate.ra.ptr.p_double[6];
        vv = state->rstate.ra.ptr.p_double[7];
        v1 = state->rstate.ra.ptr.p_double[8];
        v2 = state->rstate.ra.ptr.p_double[9];
        relcnt = state->rstate.ra.ptr.p_double[10];
    }
    else
    {
        n = 359;
        m = -58;
        k = -919;
        i = -909;
        j = 81;
        j1 = 255;
        offs = 74;
        lx = -788.0;
        lf = 809.0;
        ld = 205.0;
        rx = -838.0;
        rf = 939.0;
        rd = -526.0;
        v = 763.0;
        vv = -541.0;
        v1 = -698.0;
        v2 = -900.0;
        relcnt = -318.0;
    }
    if( state->rstate.stage==0 )
    {
        goto lbl_0;
    }
    if( state->rstate.stage==1 )
    {
        goto lbl_1;
    }
    if( state->rstate.stage==2 )
    {
        goto lbl_2;
    }
    if( state->rstate.stage==3 )
    {
        goto lbl_3;
    }
    if( state->rstate.stage==4 )
    {
        goto lbl_4;
    }
    if( state->rstate.stage==5 )
    {
        goto lbl_5;
    }
    if( state->rstate.stage==6 )
    {
        goto lbl_6;
    }
    if( state->rstate.stage==7 )
    {
        goto lbl_7;
    }
    if( state->rstate.stage==8 )
    {
        goto lbl_8;
    }
    if( state->rstate.stage==9 )
    {
        goto lbl_9;
    }
    if( state->rstate.stage==10 )
    {
        goto lbl_10;
    }
    if( state->rstate.stage==11 )
    {
        goto lbl_11;
    }
    if( state->rstate.stage==12 )
    {
        goto lbl_12;
    }
    if( state->rstate.stage==13 )
    {
        goto lbl_13;
    }
    if( state->rstate.stage==14 )
    {
        goto lbl_14;
    }
    if( state->rstate.stage==15 )
    {
        goto lbl_15;
    }
    if( state->rstate.stage==16 )
    {
        goto lbl_16;
    }
    if( state->rstate.stage==17 )
    {
        goto lbl_17;
    }
    if( state->rstate.stage==18 )
    {
        goto lbl_18;
    }
    if( state->rstate.stage==19 )
    {
        goto lbl_19;
    }
    if( state->rstate.stage==20 )
    {
        goto lbl_20;
    }
    if( state->rstate.stage==21 )
    {
        goto lbl_21;
    }
    if( state->rstate.stage==22 )
    {
        goto lbl_22;
    }
    
    /*
     * Routine body
     */
    
    /*
     * Init
     */
    if( state->wkind==1 )
    {
        ae_assert(state->npoints==state->nweights, "LSFitFit: number of points is not equal to the number of weights", _state);
    }
    state->repvaridx = -1;
    n = state->npoints;
    m = state->m;
    k = state->k;
    ivectorsetlengthatleast(&state->tmpct, state->nec+state->nic, _state);
    for(i=0; i<=state->nec-1; i++)
    {
        state->tmpct.ptr.p_int[i] = 0;
    }
    for(i=0; i<=state->nic-1; i++)
    {
        state->tmpct.ptr.p_int[state->nec+i] = -1;
    }
    minlmsetcond(&state->optstate, state->epsx, state->maxits, _state);
    minlmsetstpmax(&state->optstate, state->stpmax, _state);
    minlmsetxrep(&state->optstate, state->xrep, _state);
    minlmsetscale(&state->optstate, &state->s, _state);
    minlmsetbc(&state->optstate, &state->bndl, &state->bndu, _state);
    minlmsetlc(&state->optstate, &state->cleic, &state->tmpct, state->nec+state->nic, _state);
    state->requesttype = 0;
    
    /*
     * Allocate temporaries, as mandated by the V2 protocol
     */
    if( state->protocolversion==2 )
    {
        rallocm(1, k, &state->tmpj1, _state);
        rallocv(1, &state->tmpf1, _state);
        rallocv(k, &state->tmpg1, _state);
        rallocv(k, &state->tmpx1, _state);
        rallocv(m, &state->tmpc1, _state);
    }
    
    /*
     *  Check that user-supplied gradient is correct
     */
    if( state->protocolversion==1 )
    {
        lsfit_lsfitclearrequestfields(state, _state);
    }
    if( !(ae_fp_greater(state->teststep,(double)(0))&&state->optalgo==1) )
    {
        goto lbl_23;
    }
    for(i=0; i<=k-1; i++)
    {
        state->c.ptr.p_double[i] = state->c0.ptr.p_double[i];
        if( ae_isfinite(state->bndl.ptr.p_double[i], _state) )
        {
            state->c.ptr.p_double[i] = ae_maxreal(state->c.ptr.p_double[i], state->bndl.ptr.p_double[i], _state);
        }
        if( ae_isfinite(state->bndu.ptr.p_double[i], _state) )
        {
            state->c.ptr.p_double[i] = ae_minreal(state->c.ptr.p_double[i], state->bndu.ptr.p_double[i], _state);
        }
    }
    i = 0;
lbl_25:
    if( i>k-1 )
    {
        goto lbl_27;
    }
    ae_assert(ae_fp_less_eq(state->bndl.ptr.p_double[i],state->c.ptr.p_double[i])&&ae_fp_less_eq(state->c.ptr.p_double[i],state->bndu.ptr.p_double[i]), "LSFitIteration: internal error(State.C is out of bounds)", _state);
    if( state->protocolversion!=2 )
    {
        goto lbl_28;
    }
    
    /*
     * Test using V2 protocol
     */
    rallocv(k+m, &state->querydata, _state);
    j = 0;
lbl_30:
    if( j>n-1 )
    {
        goto lbl_32;
    }
    
    /*
     * Query value at the left point
     */
    state->requesttype = 2;
    state->querysize = 1;
    state->queryfuncs = 1;
    state->queryvars = k;
    state->querydim = m;
    rcopyv(k, &state->c, &state->querydata, _state);
    state->querydata.ptr.p_double[i] = state->c.ptr.p_double[i]-state->teststep*state->s.ptr.p_double[i];
    if( ae_isfinite(state->bndl.ptr.p_double[i], _state)&&state->querydata.ptr.p_double[i]<state->bndl.ptr.p_double[i] )
    {
        state->querydata.ptr.p_double[i] = state->bndl.ptr.p_double[i];
    }
    lx = state->querydata.ptr.p_double[i];
    for(j1=0; j1<=m-1; j1++)
    {
        state->querydata.ptr.p_double[k+j1] = state->taskx.ptr.pp_double[j][j1];
    }
    rallocv(1, &state->replyfi, _state);
    rallocv(k, &state->replydj, _state);
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    lf = state->replyfi.ptr.p_double[0];
    ld = state->replydj.ptr.p_double[i];
    
    /*
     * Query value at the right point
     */
    state->requesttype = 2;
    state->querysize = 1;
    state->queryfuncs = 1;
    state->queryvars = k;
    state->querydim = m;
    rcopyv(k, &state->c, &state->querydata, _state);
    state->querydata.ptr.p_double[i] = state->c.ptr.p_double[i]+state->teststep*state->s.ptr.p_double[i];
    if( ae_isfinite(state->bndu.ptr.p_double[i], _state)&&state->querydata.ptr.p_double[i]>state->bndu.ptr.p_double[i] )
    {
        state->querydata.ptr.p_double[i] = state->bndu.ptr.p_double[i];
    }
    rx = state->querydata.ptr.p_double[i];
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    rf = state->replyfi.ptr.p_double[0];
    rd = state->replydj.ptr.p_double[i];
    
    /*
     * Query value at the middle, perform derivative check
     */
    state->requesttype = 2;
    state->querysize = 1;
    state->queryfuncs = 1;
    state->queryvars = k;
    state->querydim = m;
    rcopyv(k, &state->c, &state->querydata, _state);
    state->querydata.ptr.p_double[i] = (lx+rx)/(double)2;
    if( ae_isfinite(state->bndl.ptr.p_double[i], _state)&&state->querydata.ptr.p_double[i]<state->bndl.ptr.p_double[i] )
    {
        state->querydata.ptr.p_double[i] = state->bndl.ptr.p_double[i];
    }
    if( ae_isfinite(state->bndu.ptr.p_double[i], _state)&&state->querydata.ptr.p_double[i]>state->bndu.ptr.p_double[i] )
    {
        state->querydata.ptr.p_double[i] = state->bndu.ptr.p_double[i];
    }
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->needfg = ae_false;
    if( !derivativecheck(lf, ld, rf, rd, state->replyfi.ptr.p_double[0], state->replydj.ptr.p_double[i], rx-lx, _state) )
    {
        state->repvaridx = i;
        state->repterminationtype = -7;
        result = ae_false;
        return result;
    }
    j = j+1;
    goto lbl_30;
lbl_32:
    goto lbl_29;
lbl_28:
    ae_assert(state->protocolversion==1, "LSFIT: integrity check 8428 failed", _state);
    v = state->c.ptr.p_double[i];
    j = 0;
lbl_33:
    if( j>n-1 )
    {
        goto lbl_35;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->taskx.ptr.pp_double[j][0], 1, ae_v_len(0,m-1));
    state->c.ptr.p_double[i] = v-state->teststep*state->s.ptr.p_double[i];
    if( ae_isfinite(state->bndl.ptr.p_double[i], _state) )
    {
        state->c.ptr.p_double[i] = ae_maxreal(state->c.ptr.p_double[i], state->bndl.ptr.p_double[i], _state);
    }
    lx = state->c.ptr.p_double[i];
    state->needfg = ae_true;
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->needfg = ae_false;
    lf = state->f;
    ld = state->g.ptr.p_double[i];
    state->c.ptr.p_double[i] = v+state->teststep*state->s.ptr.p_double[i];
    if( ae_isfinite(state->bndu.ptr.p_double[i], _state) )
    {
        state->c.ptr.p_double[i] = ae_minreal(state->c.ptr.p_double[i], state->bndu.ptr.p_double[i], _state);
    }
    rx = state->c.ptr.p_double[i];
    state->needfg = ae_true;
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->needfg = ae_false;
    rf = state->f;
    rd = state->g.ptr.p_double[i];
    state->c.ptr.p_double[i] = (lx+rx)/(double)2;
    if( ae_isfinite(state->bndl.ptr.p_double[i], _state) )
    {
        state->c.ptr.p_double[i] = ae_maxreal(state->c.ptr.p_double[i], state->bndl.ptr.p_double[i], _state);
    }
    if( ae_isfinite(state->bndu.ptr.p_double[i], _state) )
    {
        state->c.ptr.p_double[i] = ae_minreal(state->c.ptr.p_double[i], state->bndu.ptr.p_double[i], _state);
    }
    state->needfg = ae_true;
    state->rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    state->needfg = ae_false;
    state->c.ptr.p_double[i] = v;
    if( !derivativecheck(lf, ld, rf, rd, state->f, state->g.ptr.p_double[i], rx-lx, _state) )
    {
        state->repvaridx = i;
        state->repterminationtype = -7;
        result = ae_false;
        return result;
    }
    j = j+1;
    goto lbl_33;
lbl_35:
lbl_29:
    i = i+1;
    goto lbl_25;
lbl_27:
lbl_23:
    
    /*
     * Fill WCur by weights:
     * * for WKind=0 unit weights are chosen
     * * for WKind=1 we use user-supplied weights stored in State.TaskW
     */
    rvectorsetlengthatleast(&state->wcur, n, _state);
    rallocv(n*k, &state->tmpwk, _state);
    for(i=0; i<=n-1; i++)
    {
        state->wcur.ptr.p_double[i] = 1.0;
        if( state->wkind==1 )
        {
            state->wcur.ptr.p_double[i] = state->taskw.ptr.p_double[i];
        }
        for(j=0; j<=k-1; j++)
        {
            state->tmpwk.ptr.p_double[i*k+j] = state->wcur.ptr.p_double[i];
        }
    }
    
    /*
     * Optimize
     */
    if( state->protocolversion==1 )
    {
        minlmsetprotocolv1(&state->optstate, _state);
    }
    else
    {
        ae_assert(state->protocolversion==2, "LSFIT: integrity check 2839 failed", _state);
        minlmsetprotocolv2(&state->optstate, _state);
    }
lbl_36:
    if( !minlmiteration(&state->optstate, _state) )
    {
        goto lbl_37;
    }
    if( state->protocolversion!=1 )
    {
        goto lbl_38;
    }
    if( !state->optstate.needfi )
    {
        goto lbl_40;
    }
    
    /*
     * calculate f[] = wi*(f(xi,c)-yi)
     */
    i = 0;
lbl_42:
    if( i>n-1 )
    {
        goto lbl_44;
    }
    ae_v_move(&state->c.ptr.p_double[0], 1, &state->optstate.x.ptr.p_double[0], 1, ae_v_len(0,k-1));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->taskx.ptr.pp_double[i][0], 1, ae_v_len(0,m-1));
    state->pointindex = i;
    lsfit_lsfitclearrequestfields(state, _state);
    state->needf = ae_true;
    state->rstate.stage = 6;
    goto lbl_rcomm;
lbl_6:
    state->needf = ae_false;
    vv = state->wcur.ptr.p_double[i];
    state->optstate.fi.ptr.p_double[i] = vv*(state->f-state->tasky.ptr.p_double[i]);
    i = i+1;
    goto lbl_42;
lbl_44:
    goto lbl_36;
lbl_40:
    if( !state->optstate.needf )
    {
        goto lbl_45;
    }
    
    /*
     * calculate F = sum (wi*(f(xi,c)-yi))^2
     */
    state->optstate.f = (double)(0);
    i = 0;
lbl_47:
    if( i>n-1 )
    {
        goto lbl_49;
    }
    ae_v_move(&state->c.ptr.p_double[0], 1, &state->optstate.x.ptr.p_double[0], 1, ae_v_len(0,k-1));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->taskx.ptr.pp_double[i][0], 1, ae_v_len(0,m-1));
    state->pointindex = i;
    lsfit_lsfitclearrequestfields(state, _state);
    state->needf = ae_true;
    state->rstate.stage = 7;
    goto lbl_rcomm;
lbl_7:
    state->needf = ae_false;
    vv = state->wcur.ptr.p_double[i];
    state->optstate.f = state->optstate.f+ae_sqr(vv*(state->f-state->tasky.ptr.p_double[i]), _state);
    i = i+1;
    goto lbl_47;
lbl_49:
    goto lbl_36;
lbl_45:
    if( !state->optstate.needfg )
    {
        goto lbl_50;
    }
    
    /*
     * calculate F/gradF
     */
    state->optstate.f = (double)(0);
    for(i=0; i<=k-1; i++)
    {
        state->optstate.g.ptr.p_double[i] = (double)(0);
    }
    i = 0;
lbl_52:
    if( i>n-1 )
    {
        goto lbl_54;
    }
    ae_v_move(&state->c.ptr.p_double[0], 1, &state->optstate.x.ptr.p_double[0], 1, ae_v_len(0,k-1));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->taskx.ptr.pp_double[i][0], 1, ae_v_len(0,m-1));
    state->pointindex = i;
    lsfit_lsfitclearrequestfields(state, _state);
    state->needfg = ae_true;
    state->rstate.stage = 8;
    goto lbl_rcomm;
lbl_8:
    state->needfg = ae_false;
    vv = state->wcur.ptr.p_double[i];
    state->optstate.f = state->optstate.f+ae_sqr(vv*(state->f-state->tasky.ptr.p_double[i]), _state);
    v = ae_sqr(vv, _state)*(double)2*(state->f-state->tasky.ptr.p_double[i]);
    ae_v_addd(&state->optstate.g.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,k-1), v);
    i = i+1;
    goto lbl_52;
lbl_54:
    goto lbl_36;
lbl_50:
    if( !state->optstate.needfij )
    {
        goto lbl_55;
    }
    
    /*
     * calculate Fi/jac(Fi)
     */
    i = 0;
lbl_57:
    if( i>n-1 )
    {
        goto lbl_59;
    }
    ae_v_move(&state->c.ptr.p_double[0], 1, &state->optstate.x.ptr.p_double[0], 1, ae_v_len(0,k-1));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->taskx.ptr.pp_double[i][0], 1, ae_v_len(0,m-1));
    state->pointindex = i;
    lsfit_lsfitclearrequestfields(state, _state);
    state->needfg = ae_true;
    state->rstate.stage = 9;
    goto lbl_rcomm;
lbl_9:
    state->needfg = ae_false;
    vv = state->wcur.ptr.p_double[i];
    state->optstate.fi.ptr.p_double[i] = vv*(state->f-state->tasky.ptr.p_double[i]);
    ae_v_moved(&state->optstate.j.ptr.pp_double[i][0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,k-1), vv);
    i = i+1;
    goto lbl_57;
lbl_59:
    goto lbl_36;
lbl_55:
    if( !state->optstate.xupdated )
    {
        goto lbl_60;
    }
    
    /*
     * Report new iteration
     */
    ae_v_move(&state->c.ptr.p_double[0], 1, &state->optstate.x.ptr.p_double[0], 1, ae_v_len(0,k-1));
    state->f = state->optstate.f;
    lsfit_lsfitclearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 10;
    goto lbl_rcomm;
lbl_10:
    state->xupdated = ae_false;
    goto lbl_36;
lbl_60:
lbl_38:
    if( state->protocolversion!=2 )
    {
        goto lbl_62;
    }
    if( state->optstate.requesttype!=-1 )
    {
        goto lbl_64;
    }
    
    /*
     * Report current point
     */
    state->requesttype = -1;
    state->queryvars = k;
    state->reportf = state->optstate.reportf;
    rcopyallocv(k, &state->optstate.reportx, &state->reportx, _state);
    state->rstate.stage = 11;
    goto lbl_rcomm;
lbl_11:
    goto lbl_36;
lbl_64:
    if( state->optstate.requesttype!=2 )
    {
        goto lbl_66;
    }
    
    /*
     * Request a batch of dense derivatives, repack request by MinLM (one query for N
     * functions) to the format used by LSFit (N single-function queries)
     */
    ae_assert(state->optstate.querysize==1, "LSFIT: integrity check 5248 failed", _state);
    ae_assert(state->optstate.queryfuncs==n, "LSFIT: integrity check 5249 failed", _state);
    ae_assert(state->optstate.queryvars==k, "LSFIT: integrity check 5250 failed", _state);
    ae_assert(state->optstate.querydim==0, "LSFIT: integrity check 5251 failed", _state);
    state->requesttype = 2;
    state->querysize = n;
    state->queryfuncs = 1;
    state->queryvars = k;
    state->querydim = m;
    rallocv(n*(k+m), &state->querydata, _state);
    offs = 0;
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=k-1; j++)
        {
            state->querydata.ptr.p_double[offs] = state->optstate.querydata.ptr.p_double[j];
            offs = offs+1;
        }
        for(j=0; j<=m-1; j++)
        {
            state->querydata.ptr.p_double[offs] = state->taskx.ptr.pp_double[i][j];
            offs = offs+1;
        }
    }
    rallocv(n, &state->replyfi, _state);
    rallocv(n*k, &state->replydj, _state);
    state->rstate.stage = 12;
    goto lbl_rcomm;
lbl_12:
    
    /*
     * Handle reply: replace f by weight*(f-y), scale gradient
     */
    state->optstate.requesttype = 0;
    rcopyv(n, &state->replyfi, &state->optstate.replyfi, _state);
    raddv(n, -1.0, &state->tasky, &state->optstate.replyfi, _state);
    rmergemulv(n, &state->wcur, &state->optstate.replyfi, _state);
    rcopyv(n*k, &state->replydj, &state->optstate.replydj, _state);
    rmergemulv(n*k, &state->tmpwk, &state->optstate.replydj, _state);
    goto lbl_36;
lbl_66:
    if( state->optstate.requesttype!=3 )
    {
        goto lbl_68;
    }
    
    /*
     * Numerical differentiation request: repack request by MinLM (one query for N
     * functions) to the format used by LSFit (N single-function queries)
     */
    ae_assert(state->optstate.querysize==1, "LSFIT: integrity check 3348 failed", _state);
    ae_assert(state->optstate.queryfuncs==n, "LSFIT: integrity check 3349 failed", _state);
    ae_assert(state->optstate.queryvars==k, "LSFIT: integrity check 3350 failed", _state);
    ae_assert(state->optstate.querydim==0, "LSFIT: integrity check 3351 failed", _state);
    ae_assert(state->optstate.queryformulasize>=2, "LSFIT: integrity check 3352 failed", _state);
    state->requesttype = 3;
    state->querysize = n;
    state->queryfuncs = 1;
    state->queryvars = k;
    state->querydim = m;
    state->queryformulasize = state->optstate.queryformulasize;
    rallocv(n*(k+m+k*2*state->queryformulasize), &state->querydata, _state);
    offs = 0;
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=k-1; j++)
        {
            state->querydata.ptr.p_double[offs] = state->optstate.querydata.ptr.p_double[j];
            offs = offs+1;
        }
        for(j=0; j<=m-1; j++)
        {
            state->querydata.ptr.p_double[offs] = state->taskx.ptr.pp_double[i][j];
            offs = offs+1;
        }
        for(j=0; j<=k*2*state->queryformulasize-1; j++)
        {
            state->querydata.ptr.p_double[offs] = state->optstate.querydata.ptr.p_double[k+j];
            offs = offs+1;
        }
    }
    rallocv(n, &state->replyfi, _state);
    rallocv(n*k, &state->replydj, _state);
    state->rstate.stage = 13;
    goto lbl_rcomm;
lbl_13:
    
    /*
     * Handle reply: replace f by weight*(f-y), scale gradient
     */
    state->optstate.requesttype = 0;
    rcopyv(n, &state->replyfi, &state->optstate.replyfi, _state);
    raddv(n, -1.0, &state->tasky, &state->optstate.replyfi, _state);
    rmergemulv(n, &state->wcur, &state->optstate.replyfi, _state);
    rcopyv(n*k, &state->replydj, &state->optstate.replydj, _state);
    rmergemulv(n*k, &state->tmpwk, &state->optstate.replydj, _state);
    goto lbl_36;
lbl_68:
    if( state->optstate.requesttype!=4 )
    {
        goto lbl_70;
    }
    
    /*
     * Request a batch of target values, repack request by MinLM (one query for N
     * functions) to the format used by LSFit (N single-function queries)
     */
    ae_assert(state->optstate.querysize==1, "LSFIT: integrity check 5248 failed", _state);
    ae_assert(state->optstate.queryfuncs==n, "LSFIT: integrity check 5249 failed", _state);
    ae_assert(state->optstate.queryvars==k, "LSFIT: integrity check 5250 failed", _state);
    ae_assert(state->optstate.querydim==0, "LSFIT: integrity check 5251 failed", _state);
    state->requesttype = 4;
    state->querysize = n;
    state->queryfuncs = 1;
    state->queryvars = k;
    state->querydim = m;
    rallocv(n*(k+m), &state->querydata, _state);
    offs = 0;
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=k-1; j++)
        {
            state->querydata.ptr.p_double[offs] = state->optstate.querydata.ptr.p_double[j];
            offs = offs+1;
        }
        for(j=0; j<=m-1; j++)
        {
            state->querydata.ptr.p_double[offs] = state->taskx.ptr.pp_double[i][j];
            offs = offs+1;
        }
    }
    rallocv(n, &state->replyfi, _state);
    state->rstate.stage = 14;
    goto lbl_rcomm;
lbl_14:
    state->optstate.requesttype = 0;
    rcopyv(n, &state->replyfi, &state->optstate.replyfi, _state);
    raddv(n, -1.0, &state->tasky, &state->optstate.replyfi, _state);
    rmergemulv(n, &state->wcur, &state->optstate.replyfi, _state);
    goto lbl_36;
lbl_70:
lbl_62:
    ae_assert(ae_false, "LSFIT: integrity check 2012 failed (unexpected request)", _state);
    goto lbl_36;
lbl_37:
    
    /*
     * Extract results
     *
     * NOTE: reverse communication protocol used by this unit does NOT
     *       allow us to reallocate State.C[] array. Thus, we extract
     *       results to the temporary variable in order to avoid possible
     *       reallocation.
     */
    minlmresults(&state->optstate, &state->c1, &state->optrep, _state);
    state->repterminationtype = state->optrep.terminationtype;
    state->repiterationscount = state->optrep.iterationscount;
    
    /*
     * calculate errors
     */
    if( state->repterminationtype<=0 )
    {
        goto lbl_72;
    }
    
    /*
     * Calculate RMS/Avg/Max/... errors
     */
    state->reprmserror = (double)(0);
    state->repwrmserror = (double)(0);
    state->repavgerror = (double)(0);
    state->repavgrelerror = (double)(0);
    state->repmaxerror = (double)(0);
    if( state->protocolversion!=1 )
    {
        goto lbl_74;
    }
    
    /*
     * Get target values using V1 protocol, load them to ReplyFi (it is not used anyway)
     */
    rallocv(n, &state->replyfi, _state);
    i = 0;
lbl_76:
    if( i>n-1 )
    {
        goto lbl_78;
    }
    ae_v_move(&state->c.ptr.p_double[0], 1, &state->c1.ptr.p_double[0], 1, ae_v_len(0,k-1));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->taskx.ptr.pp_double[i][0], 1, ae_v_len(0,m-1));
    state->pointindex = i;
    lsfit_lsfitclearrequestfields(state, _state);
    state->needf = ae_true;
    state->rstate.stage = 15;
    goto lbl_rcomm;
lbl_15:
    state->needf = ae_false;
    state->replyfi.ptr.p_double[i] = state->f;
    i = i+1;
    goto lbl_76;
lbl_78:
    goto lbl_75;
lbl_74:
    
    /*
     * Get target values using V2 protocol
     */
    ae_assert(state->protocolversion==2, "LSFIT: integrity check 7320 failed", _state);
    state->requesttype = 4;
    state->querysize = n;
    state->queryfuncs = 1;
    state->queryvars = k;
    state->querydim = m;
    rallocv(n*(k+m), &state->querydata, _state);
    offs = 0;
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=k-1; j++)
        {
            state->querydata.ptr.p_double[offs] = state->c1.ptr.p_double[j];
            offs = offs+1;
        }
        for(j=0; j<=m-1; j++)
        {
            state->querydata.ptr.p_double[offs] = state->taskx.ptr.pp_double[i][j];
            offs = offs+1;
        }
    }
    rallocv(n, &state->replyfi, _state);
    state->rstate.stage = 16;
    goto lbl_rcomm;
lbl_16:
lbl_75:
    relcnt = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        v = state->replyfi.ptr.p_double[i];
        vv = state->wcur.ptr.p_double[i];
        state->reprmserror = state->reprmserror+ae_sqr(v-state->tasky.ptr.p_double[i], _state);
        state->repwrmserror = state->repwrmserror+ae_sqr(vv*(v-state->tasky.ptr.p_double[i]), _state);
        state->repavgerror = state->repavgerror+ae_fabs(v-state->tasky.ptr.p_double[i], _state);
        if( ae_fp_neq(state->tasky.ptr.p_double[i],(double)(0)) )
        {
            state->repavgrelerror = state->repavgrelerror+ae_fabs(v-state->tasky.ptr.p_double[i], _state)/ae_fabs(state->tasky.ptr.p_double[i], _state);
            relcnt = relcnt+(double)1;
        }
        state->repmaxerror = ae_maxreal(state->repmaxerror, ae_fabs(v-state->tasky.ptr.p_double[i], _state), _state);
    }
    state->reprmserror = ae_sqrt(state->reprmserror/(double)n, _state);
    state->repwrmserror = ae_sqrt(state->repwrmserror/(double)n, _state);
    state->repavgerror = state->repavgerror/(double)n;
    if( ae_fp_neq(relcnt,(double)(0)) )
    {
        state->repavgrelerror = state->repavgrelerror/relcnt;
    }
    
    /*
     * Calculate covariance matrix
     */
    rmatrixsetlengthatleast(&state->tmpjac, n, k, _state);
    rvectorsetlengthatleast(&state->tmpf, n, _state);
    rvectorsetlengthatleast(&state->tmp, k, _state);
    if( ae_fp_less_eq(state->diffstep,(double)(0)) )
    {
        goto lbl_79;
    }
    
    /*
     * Compute Jacobian by means of numerical differentiation
     */
    if( state->protocolversion!=1 )
    {
        goto lbl_81;
    }
    
    /*
     * Use V1 protocol
     */
    lsfit_lsfitclearrequestfields(state, _state);
    state->needf = ae_true;
    i = 0;
lbl_83:
    if( i>n-1 )
    {
        goto lbl_85;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->taskx.ptr.pp_double[i][0], 1, ae_v_len(0,m-1));
    state->pointindex = i;
    state->rstate.stage = 17;
    goto lbl_rcomm;
lbl_17:
    state->tmpf.ptr.p_double[i] = state->f;
    j = 0;
lbl_86:
    if( j>k-1 )
    {
        goto lbl_88;
    }
    v = state->c.ptr.p_double[j];
    lx = v-state->diffstep*state->s.ptr.p_double[j];
    state->c.ptr.p_double[j] = lx;
    if( ae_isfinite(state->bndl.ptr.p_double[j], _state) )
    {
        state->c.ptr.p_double[j] = ae_maxreal(state->c.ptr.p_double[j], state->bndl.ptr.p_double[j], _state);
    }
    state->rstate.stage = 18;
    goto lbl_rcomm;
lbl_18:
    lf = state->f;
    rx = v+state->diffstep*state->s.ptr.p_double[j];
    state->c.ptr.p_double[j] = rx;
    if( ae_isfinite(state->bndu.ptr.p_double[j], _state) )
    {
        state->c.ptr.p_double[j] = ae_minreal(state->c.ptr.p_double[j], state->bndu.ptr.p_double[j], _state);
    }
    state->rstate.stage = 19;
    goto lbl_rcomm;
lbl_19:
    rf = state->f;
    state->c.ptr.p_double[j] = v;
    if( ae_fp_neq(rx,lx) )
    {
        state->tmpjac.ptr.pp_double[i][j] = (rf-lf)/(rx-lx);
    }
    else
    {
        state->tmpjac.ptr.pp_double[i][j] = (double)(0);
    }
    j = j+1;
    goto lbl_86;
lbl_88:
    i = i+1;
    goto lbl_83;
lbl_85:
    state->needf = ae_false;
    goto lbl_82;
lbl_81:
    
    /*
     * Use V2 protocol
     */
    state->requesttype = 3;
    state->querysize = n;
    state->queryfuncs = 1;
    state->queryvars = k;
    state->querydim = m;
    state->queryformulasize = 2;
    rallocv(n*(k+m+k*2*state->queryformulasize), &state->querydata, _state);
    offs = 0;
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=k-1; j++)
        {
            state->querydata.ptr.p_double[offs] = state->c1.ptr.p_double[j];
            offs = offs+1;
        }
        for(j=0; j<=m-1; j++)
        {
            state->querydata.ptr.p_double[offs] = state->taskx.ptr.pp_double[i][j];
            offs = offs+1;
        }
        for(j=0; j<=k-1; j++)
        {
            
            /*
             * We guard X[J] from leaving [BndL,BndU].
             * In case BndL=BndU, we assume that derivative in this direction is zero.
             */
            v = state->diffstep*state->s.ptr.p_double[j];
            v1 = state->c1.ptr.p_double[j]-v;
            if( ae_isfinite(state->bndl.ptr.p_double[j], _state)&&v1<state->bndl.ptr.p_double[j] )
            {
                v1 = state->bndl.ptr.p_double[j];
            }
            v2 = state->c1.ptr.p_double[j]+v;
            if( ae_isfinite(state->bndu.ptr.p_double[j], _state)&&v2>state->bndu.ptr.p_double[j] )
            {
                v2 = state->bndu.ptr.p_double[j];
            }
            v = (double)(0);
            if( v1<v2 )
            {
                v = (double)1/(v2-v1);
            }
            state->querydata.ptr.p_double[offs+0*2+0] = v1;
            state->querydata.ptr.p_double[offs+0*2+1] = -v;
            state->querydata.ptr.p_double[offs+1*2+0] = v2;
            state->querydata.ptr.p_double[offs+1*2+1] = v;
            offs = offs+2*state->queryformulasize;
        }
    }
    rallocv(n, &state->replyfi, _state);
    rallocv(n*k, &state->replydj, _state);
    state->rstate.stage = 20;
    goto lbl_rcomm;
lbl_20:
    rcopyv(n, &state->replyfi, &state->tmpf, _state);
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=k-1; j++)
        {
            state->tmpjac.ptr.pp_double[i][j] = state->replydj.ptr.p_double[i*k+j];
        }
    }
lbl_82:
    goto lbl_80;
lbl_79:
    
    /*
     * Jacobian is calculated with user-provided analytic gradient
     */
    if( state->protocolversion!=1 )
    {
        goto lbl_89;
    }
    
    /*
     * Use V1 protocol
     */
    lsfit_lsfitclearrequestfields(state, _state);
    state->needfg = ae_true;
    i = 0;
lbl_91:
    if( i>n-1 )
    {
        goto lbl_93;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->taskx.ptr.pp_double[i][0], 1, ae_v_len(0,m-1));
    state->pointindex = i;
    state->rstate.stage = 21;
    goto lbl_rcomm;
lbl_21:
    state->tmpf.ptr.p_double[i] = state->f;
    for(j=0; j<=k-1; j++)
    {
        state->tmpjac.ptr.pp_double[i][j] = state->g.ptr.p_double[j];
    }
    i = i+1;
    goto lbl_91;
lbl_93:
    state->needfg = ae_false;
    goto lbl_90;
lbl_89:
    
    /*
     * Use V2 protocol
     */
    ae_assert(state->protocolversion==2, "LSFIT: integrity check 7321 failed", _state);
    state->requesttype = 2;
    state->querysize = n;
    state->queryfuncs = 1;
    state->queryvars = k;
    state->querydim = m;
    rallocv(n*(k+m), &state->querydata, _state);
    offs = 0;
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=k-1; j++)
        {
            state->querydata.ptr.p_double[offs] = state->c1.ptr.p_double[j];
            offs = offs+1;
        }
        for(j=0; j<=m-1; j++)
        {
            state->querydata.ptr.p_double[offs] = state->taskx.ptr.pp_double[i][j];
            offs = offs+1;
        }
    }
    rallocv(n, &state->replyfi, _state);
    rallocv(n*k, &state->replydj, _state);
    state->rstate.stage = 22;
    goto lbl_rcomm;
lbl_22:
    rcopyv(n, &state->replyfi, &state->tmpf, _state);
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=k-1; j++)
        {
            state->tmpjac.ptr.pp_double[i][j] = state->replydj.ptr.p_double[i*k+j];
        }
    }
lbl_90:
lbl_80:
    for(i=0; i<=k-1; i++)
    {
        state->tmp.ptr.p_double[i] = 0.0;
    }
    lsfit_estimateerrors(&state->tmpjac, &state->tmpf, &state->tasky, &state->wcur, &state->tmp, &state->s, n, k, &state->rep, &state->tmpjacw, 0, _state);
lbl_72:
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = n;
    state->rstate.ia.ptr.p_int[1] = m;
    state->rstate.ia.ptr.p_int[2] = k;
    state->rstate.ia.ptr.p_int[3] = i;
    state->rstate.ia.ptr.p_int[4] = j;
    state->rstate.ia.ptr.p_int[5] = j1;
    state->rstate.ia.ptr.p_int[6] = offs;
    state->rstate.ra.ptr.p_double[0] = lx;
    state->rstate.ra.ptr.p_double[1] = lf;
    state->rstate.ra.ptr.p_double[2] = ld;
    state->rstate.ra.ptr.p_double[3] = rx;
    state->rstate.ra.ptr.p_double[4] = rf;
    state->rstate.ra.ptr.p_double[5] = rd;
    state->rstate.ra.ptr.p_double[6] = v;
    state->rstate.ra.ptr.p_double[7] = vv;
    state->rstate.ra.ptr.p_double[8] = v1;
    state->rstate.ra.ptr.p_double[9] = v2;
    state->rstate.ra.ptr.p_double[10] = relcnt;
    return result;
}


/*************************************************************************
Nonlinear least squares fitting results.

Called after return from LSFitFit().

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    C       -   array[K], solution
    Rep     -   optimization report. On success following fields are set:
                * TerminationType   completion code:
                    * -8    optimizer   detected  NAN/INF  in  the  target
                            function and/or gradient
                    * -7    gradient verification failed.
                            See LSFitSetGradientCheck() for more information.
                    * -3    inconsistent constraints
                    *  2    relative step is no more than EpsX.
                    *  5    MaxIts steps was taken
                    *  7    stopping conditions are too stringent,
                            further improvement is impossible
                * R2                non-adjusted coefficient of determination
                                    (non-weighted)
                * RMSError          rms error on the (X,Y).
                * AvgError          average error on the (X,Y).
                * AvgRelError       average relative error on the non-zero Y
                * MaxError          maximum error
                                    NON-WEIGHTED ERRORS ARE CALCULATED
                * WRMSError         weighted rms error on the (X,Y).
                
ERRORS IN PARAMETERS                
                
This  solver  also  calculates different kinds of errors in parameters and
fills corresponding fields of report:
* Rep.CovPar        covariance matrix for parameters, array[K,K].
* Rep.ErrPar        errors in parameters, array[K],
                    errpar = sqrt(diag(CovPar))
* Rep.ErrCurve      vector of fit errors - standard deviations of empirical
                    best-fit curve from "ideal" best-fit curve built  with
                    infinite number of samples, array[N].
                    errcurve = sqrt(diag(J*CovPar*J')),
                    where J is Jacobian matrix.
* Rep.Noise         vector of per-point estimates of noise, array[N]

IMPORTANT:  errors  in  parameters  are  calculated  without  taking  into
            account boundary/linear constraints! Presence  of  constraints
            changes distribution of errors, but there is no  easy  way  to
            account for constraints when you calculate covariance matrix.
            
NOTE:       noise in the data is estimated as follows:
            * for fitting without user-supplied  weights  all  points  are
              assumed to have same level of noise, which is estimated from
              the data
            * for fitting with user-supplied weights we assume that  noise
              level in I-th point is inversely proportional to Ith weight.
              Coefficient of proportionality is estimated from the data.
            
NOTE:       we apply small amount of regularization when we invert squared
            Jacobian and calculate covariance matrix. It  guarantees  that
            algorithm won't divide by zero  during  inversion,  but  skews
            error estimates a bit (fractional error is about 10^-9).
            
            However, we believe that this difference is insignificant  for
            all practical purposes except for the situation when you  want
            to compare ALGLIB results with "reference"  implementation  up
            to the last significant digit.
            
NOTE:       covariance matrix is estimated using  correction  for  degrees
            of freedom (covariances are divided by N-M instead of dividing
            by N).

  -- ALGLIB --
     Copyright 17.08.2009 by Bochkanov Sergey
*************************************************************************/
void lsfitresults(const lsfitstate* state,
     /* Real    */ ae_vector* c,
     lsfitreport* rep,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;

    ae_vector_clear(c);
    _lsfitreport_clear(rep);

    lsfit_clearreport(rep, _state);
    rep->terminationtype = state->repterminationtype;
    rep->varidx = state->repvaridx;
    if( rep->terminationtype>0 )
    {
        ae_vector_set_length(c, state->k, _state);
        ae_v_move(&c->ptr.p_double[0], 1, &state->c1.ptr.p_double[0], 1, ae_v_len(0,state->k-1));
        rep->rmserror = state->reprmserror;
        rep->wrmserror = state->repwrmserror;
        rep->avgerror = state->repavgerror;
        rep->avgrelerror = state->repavgrelerror;
        rep->maxerror = state->repmaxerror;
        rep->iterationscount = state->repiterationscount;
        ae_matrix_set_length(&rep->covpar, state->k, state->k, _state);
        ae_vector_set_length(&rep->errpar, state->k, _state);
        ae_vector_set_length(&rep->errcurve, state->npoints, _state);
        ae_vector_set_length(&rep->noise, state->npoints, _state);
        rep->r2 = state->rep.r2;
        for(i=0; i<=state->k-1; i++)
        {
            for(j=0; j<=state->k-1; j++)
            {
                rep->covpar.ptr.pp_double[i][j] = state->rep.covpar.ptr.pp_double[i][j];
            }
            rep->errpar.ptr.p_double[i] = state->rep.errpar.ptr.p_double[i];
        }
        for(i=0; i<=state->npoints-1; i++)
        {
            rep->errcurve.ptr.p_double[i] = state->rep.errcurve.ptr.p_double[i];
            rep->noise.ptr.p_double[i] = state->rep.noise.ptr.p_double[i];
        }
    }
}


/*************************************************************************
This  subroutine  turns  on  verification  of  the  user-supplied analytic
gradient:
* user calls this subroutine before fitting begins
* LSFitFit() is called
* prior to actual fitting, for  each  point  in  data  set  X_i  and  each
  component  of  parameters  being  fited C_j algorithm performs following
  steps:
  * two trial steps are made to C_j-TestStep*S[j] and C_j+TestStep*S[j],
    where C_j is j-th parameter and S[j] is a scale of j-th parameter
  * if needed, steps are bounded with respect to constraints on C[]
  * F(X_i|C) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point
  * in case difference between prediction and actual value is higher  than
    some predetermined threshold, algorithm stops with completion code -7;
    Rep.VarIdx is set to index of the parameter with incorrect derivative.
* after verification is over, algorithm proceeds to the actual optimization.

NOTE 1: verification needs N*K (points count * parameters count)  gradient
        evaluations. It is very costly and you should use it only for  low
        dimensional  problems,  when  you  want  to  be  sure  that you've
        correctly calculated analytic derivatives. You should not  use  it
        in the production code  (unless  you  want  to  check  derivatives
        provided by some third party).

NOTE 2: you  should  carefully  choose  TestStep. Value which is too large
        (so large that function behaviour is significantly non-cubic) will
        lead to false alarms. You may use  different  step  for  different
        parameters by means of setting scale with LSFitSetScale().

NOTE 3: this function may lead to false positives. In case it reports that
        I-th  derivative was calculated incorrectly, you may decrease test
        step  and  try  one  more  time  - maybe your function changes too
        sharply  and  your  step  is  too  large for such rapidly chanding
        function.

NOTE 4: this function works only for optimizers created with LSFitCreateWFG()
        or LSFitCreateFG() constructors.
        
INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification

  -- ALGLIB --
     Copyright 15.06.2012 by Bochkanov Sergey
*************************************************************************/
void lsfitsetgradientcheck(lsfitstate* state,
     double teststep,
     ae_state *_state)
{


    ae_assert(ae_isfinite(teststep, _state), "LSFitSetGradientCheck: TestStep contains NaN or Infinite", _state);
    ae_assert(ae_fp_greater_eq(teststep,(double)(0)), "LSFitSetGradientCheck: invalid argument TestStep(TestStep<0)", _state);
    state->teststep = teststep;
}


/*************************************************************************
Set V1 reverse communication protocol
*************************************************************************/
void lsfitsetprotocolv1(lsfitstate* state, ae_state *_state)
{


    state->protocolversion = 1;
}


/*************************************************************************
Set V2 reverse communication protocol
*************************************************************************/
void lsfitsetprotocolv2(lsfitstate* state, ae_state *_state)
{


    state->protocolversion = 2;
}


/*************************************************************************
This function analyzes section of curve for processing by RDP algorithm:
given set of points X,Y with indexes [I0,I1] it returns point with
worst deviation from linear model (non-parametric version which sees curve
as Y(x)).

Input parameters:
    X, Y        -   SORTED arrays.
    I0,I1       -   interval (boundaries included) to process
    Eps         -   desired precision
    
OUTPUT PARAMETERS:
    WorstIdx    -   index of worst point
    WorstError  -   error at worst point
    
NOTE: this function guarantees that it returns exactly zero for a section
      with less than 3 points.

  -- ALGLIB PROJECT --
     Copyright 02.10.2014 by Bochkanov Sergey
*************************************************************************/
static void lsfit_rdpanalyzesection(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t i0,
     ae_int_t i1,
     ae_int_t* worstidx,
     double* worsterror,
     ae_state *_state)
{
    ae_int_t i;
    double xleft;
    double xright;
    double vx;
    double ve;
    double a;
    double b;

    *worstidx = 0;
    *worsterror = 0.0;

    xleft = x->ptr.p_double[i0];
    xright = x->ptr.p_double[i1];
    if( i1-i0+1<3||ae_fp_eq(xright,xleft) )
    {
        *worstidx = i0;
        *worsterror = 0.0;
        return;
    }
    a = (y->ptr.p_double[i1]-y->ptr.p_double[i0])/(xright-xleft);
    b = (y->ptr.p_double[i0]*xright-y->ptr.p_double[i1]*xleft)/(xright-xleft);
    *worstidx = -1;
    *worsterror = (double)(0);
    for(i=i0+1; i<=i1-1; i++)
    {
        vx = x->ptr.p_double[i];
        ve = ae_fabs(a*vx+b-y->ptr.p_double[i], _state);
        if( (ae_fp_greater(vx,xleft)&&ae_fp_less(vx,xright))&&ae_fp_greater(ve,*worsterror) )
        {
            *worsterror = ve;
            *worstidx = i;
        }
    }
}


/*************************************************************************
Recursive splitting of interval [I0,I1] (right boundary included) with RDP
algorithm (non-parametric version which sees curve as Y(x)).

Input parameters:
    X, Y        -   SORTED arrays.
    I0,I1       -   interval (boundaries included) to process
    Eps         -   desired precision
    XOut,YOut   -   preallocated output arrays large enough to store result;
                    XOut[0..1], YOut[0..1] contain first and last points of
                    curve
    NOut        -   must contain 2 on input
    
OUTPUT PARAMETERS:
    XOut, YOut  -   curve generated by RDP algorithm, UNSORTED
    NOut        -   number of points in curve

  -- ALGLIB PROJECT --
     Copyright 02.10.2014 by Bochkanov Sergey
*************************************************************************/
static void lsfit_rdprecursive(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t i0,
     ae_int_t i1,
     double eps,
     /* Real    */ ae_vector* xout,
     /* Real    */ ae_vector* yout,
     ae_int_t* nout,
     ae_state *_state)
{
    ae_int_t worstidx;
    double worsterror;


    ae_assert(ae_fp_greater(eps,(double)(0)), "RDPRecursive: internal error, Eps<0", _state);
    lsfit_rdpanalyzesection(x, y, i0, i1, &worstidx, &worsterror, _state);
    if( ae_fp_less_eq(worsterror,eps) )
    {
        return;
    }
    xout->ptr.p_double[*nout] = x->ptr.p_double[worstidx];
    yout->ptr.p_double[*nout] = y->ptr.p_double[worstidx];
    *nout = *nout+1;
    if( worstidx-i0<i1-worstidx )
    {
        lsfit_rdprecursive(x, y, i0, worstidx, eps, xout, yout, nout, _state);
        lsfit_rdprecursive(x, y, worstidx, i1, eps, xout, yout, nout, _state);
    }
    else
    {
        lsfit_rdprecursive(x, y, worstidx, i1, eps, xout, yout, nout, _state);
        lsfit_rdprecursive(x, y, i0, worstidx, eps, xout, yout, nout, _state);
    }
}


/*************************************************************************
Internal 4PL/5PL fitting function.

Accepts X, Y and already initialized and prepared MinLMState structure.
On input P1 contains initial guess, on output it contains solution.  FLast
stores function value at P1.
*************************************************************************/
static void lsfit_logisticfitinternal(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     ae_bool is4pl,
     double lambdav,
     minlmstate* state,
     minlmreport* replm,
     /* Real    */ ae_vector* p1,
     double* flast,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double ta;
    double tb;
    double tc;
    double td;
    double tg;
    double vp0;
    double vp1;

    *flast = 0.0;

    minlmrestartfrom(state, p1, _state);
    while(minlmiteration(state, _state))
    {
        ta = state->x.ptr.p_double[0];
        tb = state->x.ptr.p_double[1];
        tc = state->x.ptr.p_double[2];
        td = state->x.ptr.p_double[3];
        tg = state->x.ptr.p_double[4];
        if( state->xupdated )
        {
            
            /*
             * Save best function value obtained so far.
             */
            *flast = state->f;
            continue;
        }
        if( state->needfi||state->needfij )
        {
            
            /*
             * Function vector and Jacobian
             */
            for(i=0; i<=n-1; i++)
            {
                ae_assert(ae_fp_greater_eq(x->ptr.p_double[i],(double)(0)), "LogisticFitInternal: integrity error", _state);
                
                /*
                 * Handle zero X
                 */
                if( ae_fp_eq(x->ptr.p_double[i],(double)(0)) )
                {
                    if( ae_fp_greater_eq(tb,(double)(0)) )
                    {
                        
                        /*
                         * Positive or zero TB, limit X^TB subject to X->+0 is equal to zero.
                         */
                        state->fi.ptr.p_double[i] = ta-y->ptr.p_double[i];
                        if( state->needfij )
                        {
                            state->j.ptr.pp_double[i][0] = (double)(1);
                            state->j.ptr.pp_double[i][1] = (double)(0);
                            state->j.ptr.pp_double[i][2] = (double)(0);
                            state->j.ptr.pp_double[i][3] = (double)(0);
                            state->j.ptr.pp_double[i][4] = (double)(0);
                        }
                    }
                    else
                    {
                        
                        /*
                         * Negative TB, limit X^TB subject to X->+0 is equal to +INF.
                         */
                        state->fi.ptr.p_double[i] = td-y->ptr.p_double[i];
                        if( state->needfij )
                        {
                            state->j.ptr.pp_double[i][0] = (double)(0);
                            state->j.ptr.pp_double[i][1] = (double)(0);
                            state->j.ptr.pp_double[i][2] = (double)(0);
                            state->j.ptr.pp_double[i][3] = (double)(1);
                            state->j.ptr.pp_double[i][4] = (double)(0);
                        }
                    }
                    continue;
                }
                
                /*
                 * Positive X.
                 * Prepare VP0/VP1, it may become infinite or nearly overflow in some rare cases,
                 * handle these cases
                 */
                vp0 = ae_pow(x->ptr.p_double[i]/tc, tb, _state);
                if( is4pl )
                {
                    vp1 = (double)1+vp0;
                }
                else
                {
                    vp1 = ae_pow((double)1+vp0, tg, _state);
                }
                if( (!ae_isfinite(vp1, _state)||ae_fp_greater(vp0,1.0E50))||ae_fp_greater(vp1,1.0E50) )
                {
                    
                    /*
                     * VP0/VP1 are not finite, assume that it is +INF or -INF
                     */
                    state->fi.ptr.p_double[i] = td-y->ptr.p_double[i];
                    if( state->needfij )
                    {
                        state->j.ptr.pp_double[i][0] = (double)(0);
                        state->j.ptr.pp_double[i][1] = (double)(0);
                        state->j.ptr.pp_double[i][2] = (double)(0);
                        state->j.ptr.pp_double[i][3] = (double)(1);
                        state->j.ptr.pp_double[i][4] = (double)(0);
                    }
                    continue;
                }
                
                /*
                 * VP0/VP1 are finite, normal processing
                 */
                if( is4pl )
                {
                    state->fi.ptr.p_double[i] = td+(ta-td)/vp1-y->ptr.p_double[i];
                    if( state->needfij )
                    {
                        state->j.ptr.pp_double[i][0] = (double)1/vp1;
                        state->j.ptr.pp_double[i][1] = -(ta-td)*vp0*ae_log(x->ptr.p_double[i]/tc, _state)/ae_sqr(vp1, _state);
                        state->j.ptr.pp_double[i][2] = (ta-td)*(tb/tc)*vp0/ae_sqr(vp1, _state);
                        state->j.ptr.pp_double[i][3] = (double)1-(double)1/vp1;
                        state->j.ptr.pp_double[i][4] = (double)(0);
                    }
                }
                else
                {
                    state->fi.ptr.p_double[i] = td+(ta-td)/vp1-y->ptr.p_double[i];
                    if( state->needfij )
                    {
                        state->j.ptr.pp_double[i][0] = (double)1/vp1;
                        state->j.ptr.pp_double[i][1] = (ta-td)*(-tg)*ae_pow((double)1+vp0, -tg-(double)1, _state)*vp0*ae_log(x->ptr.p_double[i]/tc, _state);
                        state->j.ptr.pp_double[i][2] = (ta-td)*(-tg)*ae_pow((double)1+vp0, -tg-(double)1, _state)*vp0*(-tb/tc);
                        state->j.ptr.pp_double[i][3] = (double)1-(double)1/vp1;
                        state->j.ptr.pp_double[i][4] = -(ta-td)/vp1*ae_log((double)1+vp0, _state);
                    }
                }
            }
            
            /*
             * Add regularizer
             */
            for(i=0; i<=4; i++)
            {
                state->fi.ptr.p_double[n+i] = lambdav*state->x.ptr.p_double[i];
                if( state->needfij )
                {
                    for(j=0; j<=4; j++)
                    {
                        state->j.ptr.pp_double[n+i][j] = 0.0;
                    }
                    state->j.ptr.pp_double[n+i][i] = lambdav;
                }
            }
            
            /*
             * Done
             */
            continue;
        }
        ae_assert(ae_false, "LogisticFitX: internal error", _state);
    }
    minlmresultsbuf(state, p1, replm, _state);
    ae_assert(replm->terminationtype>0, "LogisticFitX: internal error", _state);
}


/*************************************************************************
Calculate errors for 4PL/5PL fit.
Leaves other fields of Rep unchanged, so caller should properly initialize
it with ClearRep() call.

  -- ALGLIB PROJECT --
     Copyright 28.04.2017 by Bochkanov Sergey
*************************************************************************/
static void lsfit_logisticfit45errors(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     double a,
     double b,
     double c,
     double d,
     double g,
     lsfitreport* rep,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;
    double v;
    double rss;
    double tss;
    double meany;


    
    /*
     * Calculate errors
     */
    rep->rmserror = (double)(0);
    rep->avgerror = (double)(0);
    rep->avgrelerror = (double)(0);
    rep->maxerror = (double)(0);
    k = 0;
    rss = 0.0;
    tss = 0.0;
    meany = 0.0;
    for(i=0; i<=n-1; i++)
    {
        meany = meany+y->ptr.p_double[i];
    }
    meany = meany/(double)n;
    for(i=0; i<=n-1; i++)
    {
        
        /*
         * Calculate residual from regression
         */
        if( ae_fp_greater(x->ptr.p_double[i],(double)(0)) )
        {
            v = d+(a-d)/ae_pow(1.0+ae_pow(x->ptr.p_double[i]/c, b, _state), g, _state)-y->ptr.p_double[i];
        }
        else
        {
            if( ae_fp_greater_eq(b,(double)(0)) )
            {
                v = a-y->ptr.p_double[i];
            }
            else
            {
                v = d-y->ptr.p_double[i];
            }
        }
        
        /*
         * Update RSS (residual sum of squares) and TSS (total sum of squares)
         * which are used to calculate coefficient of determination.
         *
         * NOTE: we use formula R2 = 1-RSS/TSS because it has nice property of
         *       being equal to 0.0 if and only if model perfectly fits data.
         *
         *       When we fit nonlinear models, there are exist multiple ways of
         *       determining R2, each of them giving different results. Formula
         *       above is the most intuitive one.
         */
        rss = rss+v*v;
        tss = tss+ae_sqr(y->ptr.p_double[i]-meany, _state);
        
        /*
         * Update errors
         */
        rep->rmserror = rep->rmserror+ae_sqr(v, _state);
        rep->avgerror = rep->avgerror+ae_fabs(v, _state);
        if( ae_fp_neq(y->ptr.p_double[i],(double)(0)) )
        {
            rep->avgrelerror = rep->avgrelerror+ae_fabs(v/y->ptr.p_double[i], _state);
            k = k+1;
        }
        rep->maxerror = ae_maxreal(rep->maxerror, ae_fabs(v, _state), _state);
    }
    rep->rmserror = ae_sqrt(rep->rmserror/(double)n, _state);
    rep->avgerror = rep->avgerror/(double)n;
    if( k>0 )
    {
        rep->avgrelerror = rep->avgrelerror/(double)k;
    }
    rep->r2 = 1.0-rss/tss;
}


/*************************************************************************
Internal spline fitting subroutine

  -- ALGLIB PROJECT --
     Copyright 08.09.2009 by Bochkanov Sergey
*************************************************************************/
static void lsfit_spline1dfitinternal(ae_int_t st,
     /* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     /* Real    */ const ae_vector* _w,
     ae_int_t n,
     /* Real    */ const ae_vector* _xc,
     /* Real    */ const ae_vector* _yc,
     /* Integer */ const ae_vector* dc,
     ae_int_t k,
     ae_int_t m,
     spline1dinterpolant* s,
     spline1dfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector x;
    ae_vector y;
    ae_vector w;
    ae_vector xc;
    ae_vector yc;
    ae_matrix fmatrix;
    ae_matrix cmatrix;
    ae_vector y2;
    ae_vector w2;
    ae_vector sx;
    ae_vector sy;
    ae_vector sd;
    ae_vector tmp;
    ae_vector xoriginal;
    ae_vector yoriginal;
    lsfitreport lrep;
    double v0;
    double v1;
    double v2;
    double mx;
    spline1dinterpolant s2;
    ae_int_t i;
    ae_int_t j;
    ae_int_t relcnt;
    double xa;
    double xb;
    double sa;
    double sb;
    double bl;
    double br;
    double decay;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&y, 0, sizeof(y));
    memset(&w, 0, sizeof(w));
    memset(&xc, 0, sizeof(xc));
    memset(&yc, 0, sizeof(yc));
    memset(&fmatrix, 0, sizeof(fmatrix));
    memset(&cmatrix, 0, sizeof(cmatrix));
    memset(&y2, 0, sizeof(y2));
    memset(&w2, 0, sizeof(w2));
    memset(&sx, 0, sizeof(sx));
    memset(&sy, 0, sizeof(sy));
    memset(&sd, 0, sizeof(sd));
    memset(&tmp, 0, sizeof(tmp));
    memset(&xoriginal, 0, sizeof(xoriginal));
    memset(&yoriginal, 0, sizeof(yoriginal));
    memset(&lrep, 0, sizeof(lrep));
    memset(&s2, 0, sizeof(s2));
    ae_vector_init_copy(&x, _x, _state, ae_true);
    ae_vector_init_copy(&y, _y, _state, ae_true);
    ae_vector_init_copy(&w, _w, _state, ae_true);
    ae_vector_init_copy(&xc, _xc, _state, ae_true);
    ae_vector_init_copy(&yc, _yc, _state, ae_true);
    _spline1dinterpolant_clear(s);
    _spline1dfitreport_clear(rep);
    ae_matrix_init(&fmatrix, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&cmatrix, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&y2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&w2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&sx, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&sy, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&sd, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xoriginal, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&yoriginal, 0, DT_REAL, _state, ae_true);
    _lsfitreport_init(&lrep, _state, ae_true);
    _spline1dinterpolant_init(&s2, _state, ae_true);

    ae_assert(st==0||st==1, "Spline1DFit: internal error!", _state);
    if( st==0&&m<4 )
    {
        ae_assert(ae_false, "LSFIT: integrity check 5729 failed", _state);
        ae_frame_leave(_state);
        return;
    }
    if( st==1&&m<4 )
    {
        ae_assert(ae_false, "LSFIT: integrity check 6229 failed", _state);
        ae_frame_leave(_state);
        return;
    }
    if( (n<1||k<0)||k>=m )
    {
        ae_assert(ae_false, "LSFIT: integrity check 6729 failed", _state);
        ae_frame_leave(_state);
        return;
    }
    for(i=0; i<=k-1; i++)
    {
        if( dc->ptr.p_int[i]<0 )
        {
            ae_assert(ae_false, "LSFIT: integrity check 7329 failed", _state);
        }
        if( dc->ptr.p_int[i]>1 )
        {
            ae_assert(ae_false, "LSFIT: integrity check 7529 failed", _state);
        }
    }
    if( st==1&&m%2!=0 )
    {
        
        /*
         * Hermite fitter must have even number of basis functions
         */
        ae_assert(ae_false, "LSFIT: integrity check 8229 failed", _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * weight decay for correct handling of task which becomes
     * degenerate after constraints are applied
     */
    decay = (double)10000*ae_machineepsilon;
    
    /*
     * Scale X, Y, XC, YC
     */
    lsfitscalexy(&x, &y, &w, n, &xc, &yc, dc, k, &xa, &xb, &sa, &sb, &xoriginal, &yoriginal, _state);
    
    /*
     * allocate space, initialize:
     * * SX     -   grid for basis functions
     * * SY     -   values of basis functions at grid points
     * * FMatrix-   values of basis functions at X[]
     * * CMatrix-   values (derivatives) of basis functions at XC[]
     */
    ae_vector_set_length(&y2, n+m, _state);
    ae_vector_set_length(&w2, n+m, _state);
    ae_matrix_set_length(&fmatrix, n+m, m, _state);
    if( k>0 )
    {
        ae_matrix_set_length(&cmatrix, k, m+1, _state);
    }
    if( st==0 )
    {
        
        /*
         * allocate space for cubic spline
         */
        ae_vector_set_length(&sx, m-2, _state);
        ae_vector_set_length(&sy, m-2, _state);
        for(j=0; j<=m-2-1; j++)
        {
            sx.ptr.p_double[j] = (double)(2*j)/(double)(m-2-1)-(double)1;
        }
    }
    if( st==1 )
    {
        
        /*
         * allocate space for Hermite spline
         */
        ae_vector_set_length(&sx, m/2, _state);
        ae_vector_set_length(&sy, m/2, _state);
        ae_vector_set_length(&sd, m/2, _state);
        for(j=0; j<=m/2-1; j++)
        {
            sx.ptr.p_double[j] = (double)(2*j)/(double)(m/2-1)-(double)1;
        }
    }
    
    /*
     * Prepare design and constraints matrices:
     * * fill constraints matrix
     * * fill first N rows of design matrix with values
     * * fill next M rows of design matrix with regularizing term
     * * append M zeros to Y
     * * append M elements, mean(abs(W)) each, to W
     */
    for(j=0; j<=m-1; j++)
    {
        
        /*
         * prepare Jth basis function
         */
        if( st==0 )
        {
            
            /*
             * cubic spline basis
             */
            for(i=0; i<=m-2-1; i++)
            {
                sy.ptr.p_double[i] = (double)(0);
            }
            bl = (double)(0);
            br = (double)(0);
            if( j<m-2 )
            {
                sy.ptr.p_double[j] = (double)(1);
            }
            if( j==m-2 )
            {
                bl = (double)(1);
            }
            if( j==m-1 )
            {
                br = (double)(1);
            }
            spline1dbuildcubic(&sx, &sy, m-2, 1, bl, 1, br, &s2, _state);
        }
        if( st==1 )
        {
            
            /*
             * Hermite basis
             */
            for(i=0; i<=m/2-1; i++)
            {
                sy.ptr.p_double[i] = (double)(0);
                sd.ptr.p_double[i] = (double)(0);
            }
            if( j%2==0 )
            {
                sy.ptr.p_double[j/2] = (double)(1);
            }
            else
            {
                sd.ptr.p_double[j/2] = (double)(1);
            }
            spline1dbuildhermite(&sx, &sy, &sd, m/2, &s2, _state);
        }
        
        /*
         * values at X[], XC[]
         */
        for(i=0; i<=n-1; i++)
        {
            fmatrix.ptr.pp_double[i][j] = spline1dcalc(&s2, x.ptr.p_double[i], _state);
        }
        for(i=0; i<=k-1; i++)
        {
            ae_assert(dc->ptr.p_int[i]>=0&&dc->ptr.p_int[i]<=2, "Spline1DFit: internal error!", _state);
            spline1ddiff(&s2, xc.ptr.p_double[i], &v0, &v1, &v2, _state);
            if( dc->ptr.p_int[i]==0 )
            {
                cmatrix.ptr.pp_double[i][j] = v0;
            }
            if( dc->ptr.p_int[i]==1 )
            {
                cmatrix.ptr.pp_double[i][j] = v1;
            }
            if( dc->ptr.p_int[i]==2 )
            {
                cmatrix.ptr.pp_double[i][j] = v2;
            }
        }
    }
    for(i=0; i<=k-1; i++)
    {
        cmatrix.ptr.pp_double[i][m] = yc.ptr.p_double[i];
    }
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=m-1; j++)
        {
            if( i==j )
            {
                fmatrix.ptr.pp_double[n+i][j] = decay;
            }
            else
            {
                fmatrix.ptr.pp_double[n+i][j] = (double)(0);
            }
        }
    }
    ae_vector_set_length(&y2, n+m, _state);
    ae_vector_set_length(&w2, n+m, _state);
    ae_v_move(&y2.ptr.p_double[0], 1, &y.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&w2.ptr.p_double[0], 1, &w.ptr.p_double[0], 1, ae_v_len(0,n-1));
    mx = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        mx = mx+ae_fabs(w.ptr.p_double[i], _state);
    }
    mx = mx/(double)n;
    for(i=0; i<=m-1; i++)
    {
        y2.ptr.p_double[n+i] = (double)(0);
        w2.ptr.p_double[n+i] = mx;
    }
    
    /*
     * Solve constrained task
     */
    if( k>0 )
    {
        
        /*
         * solve using regularization
         */
        lsfitlinearwc(&y2, &w2, &fmatrix, &cmatrix, n+m, m, k, &tmp, &lrep, _state);
    }
    else
    {
        
        /*
         * no constraints, no regularization needed
         */
        lsfitlinearwc(&y, &w, &fmatrix, &cmatrix, n, m, k, &tmp, &lrep, _state);
    }
    rep->terminationtype = lrep.terminationtype;
    if( rep->terminationtype<0 )
    {
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Generate spline and scale it
     */
    if( st==0 )
    {
        
        /*
         * cubic spline basis
         */
        ae_v_move(&sy.ptr.p_double[0], 1, &tmp.ptr.p_double[0], 1, ae_v_len(0,m-2-1));
        spline1dbuildcubic(&sx, &sy, m-2, 1, tmp.ptr.p_double[m-2], 1, tmp.ptr.p_double[m-1], s, _state);
    }
    if( st==1 )
    {
        
        /*
         * Hermite basis
         */
        for(i=0; i<=m/2-1; i++)
        {
            sy.ptr.p_double[i] = tmp.ptr.p_double[2*i];
            sd.ptr.p_double[i] = tmp.ptr.p_double[2*i+1];
        }
        spline1dbuildhermite(&sx, &sy, &sd, m/2, s, _state);
    }
    spline1dlintransx(s, (double)2/(xb-xa), -(xa+xb)/(xb-xa), _state);
    spline1dlintransy(s, sb-sa, sa, _state);
    
    /*
     * Scale absolute errors obtained from LSFitLinearW.
     * Relative error should be calculated separately
     * (because of shifting/scaling of the task)
     */
    rep->taskrcond = lrep.taskrcond;
    rep->rmserror = lrep.rmserror*(sb-sa);
    rep->avgerror = lrep.avgerror*(sb-sa);
    rep->maxerror = lrep.maxerror*(sb-sa);
    rep->avgrelerror = (double)(0);
    relcnt = 0;
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_neq(yoriginal.ptr.p_double[i],(double)(0)) )
        {
            rep->avgrelerror = rep->avgrelerror+ae_fabs(spline1dcalc(s, xoriginal.ptr.p_double[i], _state)-yoriginal.ptr.p_double[i], _state)/ae_fabs(yoriginal.ptr.p_double[i], _state);
            relcnt = relcnt+1;
        }
    }
    if( relcnt!=0 )
    {
        rep->avgrelerror = rep->avgrelerror/(double)relcnt;
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Internal fitting subroutine
*************************************************************************/
static void lsfit_lsfitlinearinternal(/* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* w,
     /* Real    */ const ae_matrix* fmatrix,
     ae_int_t n,
     ae_int_t m,
     /* Real    */ ae_vector* c,
     lsfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    double threshold;
    ae_matrix ft;
    ae_matrix q;
    ae_matrix l;
    ae_matrix r;
    ae_vector b;
    ae_vector wmod;
    ae_vector tau;
    ae_vector nzeros;
    ae_vector s;
    ae_int_t i;
    ae_int_t j;
    double v;
    ae_vector sv;
    ae_matrix u;
    ae_matrix vt;
    ae_vector tmp;
    ae_vector utb;
    ae_vector sutb;
    ae_int_t relcnt;

    ae_frame_make(_state, &_frame_block);
    memset(&ft, 0, sizeof(ft));
    memset(&q, 0, sizeof(q));
    memset(&l, 0, sizeof(l));
    memset(&r, 0, sizeof(r));
    memset(&b, 0, sizeof(b));
    memset(&wmod, 0, sizeof(wmod));
    memset(&tau, 0, sizeof(tau));
    memset(&nzeros, 0, sizeof(nzeros));
    memset(&s, 0, sizeof(s));
    memset(&sv, 0, sizeof(sv));
    memset(&u, 0, sizeof(u));
    memset(&vt, 0, sizeof(vt));
    memset(&tmp, 0, sizeof(tmp));
    memset(&utb, 0, sizeof(utb));
    memset(&sutb, 0, sizeof(sutb));
    ae_vector_clear(c);
    _lsfitreport_clear(rep);
    ae_matrix_init(&ft, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&q, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&l, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&r, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&b, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&wmod, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tau, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&nzeros, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&s, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&sv, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&u, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&vt, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&utb, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&sutb, 0, DT_REAL, _state, ae_true);

    lsfit_clearreport(rep, _state);
    ae_assert(!(n<1||m<1), "LSFIT: integrity check 2508 failed", _state);
    rep->terminationtype = 1;
    threshold = ae_sqrt(ae_machineepsilon, _state);
    
    /*
     * Degenerate case, needs special handling
     */
    if( n<m )
    {
        
        /*
         * Create design matrix.
         */
        ae_matrix_set_length(&ft, n, m, _state);
        ae_vector_set_length(&b, n, _state);
        ae_vector_set_length(&wmod, n, _state);
        for(j=0; j<=n-1; j++)
        {
            v = w->ptr.p_double[j];
            ae_v_moved(&ft.ptr.pp_double[j][0], 1, &fmatrix->ptr.pp_double[j][0], 1, ae_v_len(0,m-1), v);
            b.ptr.p_double[j] = w->ptr.p_double[j]*y->ptr.p_double[j];
            wmod.ptr.p_double[j] = (double)(1);
        }
        
        /*
         * LQ decomposition and reduction to M=N
         */
        ae_vector_set_length(c, m, _state);
        for(i=0; i<=m-1; i++)
        {
            c->ptr.p_double[i] = (double)(0);
        }
        rep->taskrcond = (double)(0);
        rmatrixlq(&ft, n, m, &tau, _state);
        rmatrixlqunpackq(&ft, n, m, &tau, n, &q, _state);
        rmatrixlqunpackl(&ft, n, m, &l, _state);
        lsfit_lsfitlinearinternal(&b, &wmod, &l, n, n, &tmp, rep, _state);
        if( rep->terminationtype<=0 )
        {
            ae_frame_leave(_state);
            return;
        }
        for(i=0; i<=n-1; i++)
        {
            v = tmp.ptr.p_double[i];
            ae_v_addd(&c->ptr.p_double[0], 1, &q.ptr.pp_double[i][0], 1, ae_v_len(0,m-1), v);
        }
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * N>=M. Generate design matrix and reduce to N=M using
     * QR decomposition.
     */
    ae_matrix_set_length(&ft, n, m, _state);
    ae_vector_set_length(&b, n, _state);
    for(j=0; j<=n-1; j++)
    {
        v = w->ptr.p_double[j];
        ae_v_moved(&ft.ptr.pp_double[j][0], 1, &fmatrix->ptr.pp_double[j][0], 1, ae_v_len(0,m-1), v);
        b.ptr.p_double[j] = w->ptr.p_double[j]*y->ptr.p_double[j];
    }
    rmatrixqr(&ft, n, m, &tau, _state);
    rmatrixqrunpackq(&ft, n, m, &tau, m, &q, _state);
    rmatrixqrunpackr(&ft, n, m, &r, _state);
    ae_vector_set_length(&tmp, m, _state);
    for(i=0; i<=m-1; i++)
    {
        tmp.ptr.p_double[i] = (double)(0);
    }
    for(i=0; i<=n-1; i++)
    {
        v = b.ptr.p_double[i];
        ae_v_addd(&tmp.ptr.p_double[0], 1, &q.ptr.pp_double[i][0], 1, ae_v_len(0,m-1), v);
    }
    ae_vector_set_length(&b, m, _state);
    ae_v_move(&b.ptr.p_double[0], 1, &tmp.ptr.p_double[0], 1, ae_v_len(0,m-1));
    
    /*
     * R contains reduced MxM design upper triangular matrix,
     * B contains reduced Mx1 right part.
     *
     * Determine system condition number and decide
     * should we use triangular solver (faster) or
     * SVD-based solver (more stable).
     *
     * We can use LU-based RCond estimator for this task.
     */
    rep->taskrcond = rmatrixlurcondinf(&r, m, _state);
    if( ae_fp_greater(rep->taskrcond,threshold) )
    {
        
        /*
         * use QR-based solver
         */
        ae_vector_set_length(c, m, _state);
        c->ptr.p_double[m-1] = b.ptr.p_double[m-1]/r.ptr.pp_double[m-1][m-1];
        for(i=m-2; i>=0; i--)
        {
            v = ae_v_dotproduct(&r.ptr.pp_double[i][i+1], 1, &c->ptr.p_double[i+1], 1, ae_v_len(i+1,m-1));
            c->ptr.p_double[i] = (b.ptr.p_double[i]-v)/r.ptr.pp_double[i][i];
        }
    }
    else
    {
        
        /*
         * use SVD-based solver
         */
        if( !rmatrixsvd(&r, m, m, 1, 1, 2, &sv, &u, &vt, _state) )
        {
            ae_assert(ae_false, "LSFitLinearXX: critical failure - internal SVD solver failed", _state);
            ae_frame_leave(_state);
            return;
        }
        ae_vector_set_length(&utb, m, _state);
        ae_vector_set_length(&sutb, m, _state);
        for(i=0; i<=m-1; i++)
        {
            utb.ptr.p_double[i] = (double)(0);
        }
        for(i=0; i<=m-1; i++)
        {
            v = b.ptr.p_double[i];
            ae_v_addd(&utb.ptr.p_double[0], 1, &u.ptr.pp_double[i][0], 1, ae_v_len(0,m-1), v);
        }
        if( ae_fp_greater(sv.ptr.p_double[0],(double)(0)) )
        {
            rep->taskrcond = sv.ptr.p_double[m-1]/sv.ptr.p_double[0];
            for(i=0; i<=m-1; i++)
            {
                if( ae_fp_greater(sv.ptr.p_double[i],threshold*sv.ptr.p_double[0]) )
                {
                    sutb.ptr.p_double[i] = utb.ptr.p_double[i]/sv.ptr.p_double[i];
                }
                else
                {
                    sutb.ptr.p_double[i] = (double)(0);
                }
            }
        }
        else
        {
            rep->taskrcond = (double)(0);
            for(i=0; i<=m-1; i++)
            {
                sutb.ptr.p_double[i] = (double)(0);
            }
        }
        ae_vector_set_length(c, m, _state);
        for(i=0; i<=m-1; i++)
        {
            c->ptr.p_double[i] = (double)(0);
        }
        for(i=0; i<=m-1; i++)
        {
            v = sutb.ptr.p_double[i];
            ae_v_addd(&c->ptr.p_double[0], 1, &vt.ptr.pp_double[i][0], 1, ae_v_len(0,m-1), v);
        }
    }
    
    /*
     * calculate errors
     */
    rep->rmserror = (double)(0);
    rep->avgerror = (double)(0);
    rep->avgrelerror = (double)(0);
    rep->maxerror = (double)(0);
    relcnt = 0;
    for(i=0; i<=n-1; i++)
    {
        v = ae_v_dotproduct(&fmatrix->ptr.pp_double[i][0], 1, &c->ptr.p_double[0], 1, ae_v_len(0,m-1));
        rep->rmserror = rep->rmserror+ae_sqr(v-y->ptr.p_double[i], _state);
        rep->avgerror = rep->avgerror+ae_fabs(v-y->ptr.p_double[i], _state);
        if( ae_fp_neq(y->ptr.p_double[i],(double)(0)) )
        {
            rep->avgrelerror = rep->avgrelerror+ae_fabs(v-y->ptr.p_double[i], _state)/ae_fabs(y->ptr.p_double[i], _state);
            relcnt = relcnt+1;
        }
        rep->maxerror = ae_maxreal(rep->maxerror, ae_fabs(v-y->ptr.p_double[i], _state), _state);
    }
    rep->rmserror = ae_sqrt(rep->rmserror/(double)n, _state);
    rep->avgerror = rep->avgerror/(double)n;
    if( relcnt!=0 )
    {
        rep->avgrelerror = rep->avgrelerror/(double)relcnt;
    }
    ae_vector_set_length(&nzeros, n, _state);
    ae_vector_set_length(&s, m, _state);
    for(i=0; i<=m-1; i++)
    {
        s.ptr.p_double[i] = (double)(0);
    }
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=m-1; j++)
        {
            s.ptr.p_double[j] = s.ptr.p_double[j]+ae_sqr(fmatrix->ptr.pp_double[i][j], _state);
        }
        nzeros.ptr.p_double[i] = (double)(0);
    }
    for(i=0; i<=m-1; i++)
    {
        if( ae_fp_neq(s.ptr.p_double[i],(double)(0)) )
        {
            s.ptr.p_double[i] = ae_sqrt((double)1/s.ptr.p_double[i], _state);
        }
        else
        {
            s.ptr.p_double[i] = (double)(1);
        }
    }
    lsfit_estimateerrors(fmatrix, &nzeros, y, w, c, &s, n, m, rep, &r, 1, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Internal subroutine
*************************************************************************/
static void lsfit_lsfitclearrequestfields(lsfitstate* state,
     ae_state *_state)
{


    ae_assert(state->protocolversion==1, "LSFIT: unexpected protocol", _state);
    state->needf = ae_false;
    state->needfg = ae_false;
    state->xupdated = ae_false;
}


/*************************************************************************
Internal subroutine, calculates barycentric basis functions.
Used for efficient simultaneous calculation of N basis functions.

  -- ALGLIB --
     Copyright 17.08.2009 by Bochkanov Sergey
*************************************************************************/
static void lsfit_barycentriccalcbasis(const barycentricinterpolant* b,
     double t,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    double s2;
    double s;
    double v;
    ae_int_t i;
    ae_int_t j;


    
    /*
     * special case: N=1
     */
    if( b->n==1 )
    {
        y->ptr.p_double[0] = (double)(1);
        return;
    }
    
    /*
     * Here we assume that task is normalized, i.e.:
     * 1. abs(Y[i])<=1
     * 2. abs(W[i])<=1
     * 3. X[] is ordered
     *
     * First, we decide: should we use "safe" formula (guarded
     * against overflow) or fast one?
     */
    s = ae_fabs(t-b->x.ptr.p_double[0], _state);
    for(i=0; i<=b->n-1; i++)
    {
        v = b->x.ptr.p_double[i];
        if( ae_fp_eq(v,t) )
        {
            for(j=0; j<=b->n-1; j++)
            {
                y->ptr.p_double[j] = (double)(0);
            }
            y->ptr.p_double[i] = (double)(1);
            return;
        }
        v = ae_fabs(t-v, _state);
        if( ae_fp_less(v,s) )
        {
            s = v;
        }
    }
    s2 = (double)(0);
    for(i=0; i<=b->n-1; i++)
    {
        v = s/(t-b->x.ptr.p_double[i]);
        v = v*b->w.ptr.p_double[i];
        y->ptr.p_double[i] = v;
        s2 = s2+v;
    }
    v = (double)1/s2;
    ae_v_muld(&y->ptr.p_double[0], 1, ae_v_len(0,b->n-1), v);
}


/*************************************************************************
This is internal function for Chebyshev fitting.

It assumes that input data are normalized:
* X/XC belong to [-1,+1],
* mean(Y)=0, stddev(Y)=1.

It does not checks inputs for errors.

This function is used to fit general (shifted) Chebyshev models, power
basis models or barycentric models.

INPUT PARAMETERS:
    X   -   points, array[0..N-1].
    Y   -   function values, array[0..N-1].
    W   -   weights, array[0..N-1]
    N   -   number of points, N>0.
    XC  -   points where polynomial values/derivatives are constrained,
            array[0..K-1].
    YC  -   values of constraints, array[0..K-1]
    DC  -   array[0..K-1], types of constraints:
            * DC[i]=0   means that P(XC[i])=YC[i]
            * DC[i]=1   means that P'(XC[i])=YC[i]
    K   -   number of constraints, 0<=K<M.
            K=0 means no constraints (XC/YC/DC are not used in such cases)
    M   -   number of basis functions (= polynomial_degree + 1), M>=1

OUTPUT PARAMETERS:
    C   -   interpolant in Chebyshev form; [-1,+1] is used as base interval
    Rep -   report, same format as in LSFitLinearW() subroutine.
            Following fields are set:
            * TerminationType
            * RMSError      rms error on the (X,Y).
            * AvgError      average error on the (X,Y).
            * AvgRelError   average relative error on the non-zero Y
            * MaxError      maximum error
                            NON-WEIGHTED ERRORS ARE CALCULATED

IMPORTANT:
    this subroitine doesn't calculate task's condition number for K<>0.

  -- ALGLIB PROJECT --
     Copyright 10.12.2009 by Bochkanov Sergey
*************************************************************************/
static void lsfit_internalchebyshevfit(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* w,
     ae_int_t n,
     /* Real    */ const ae_vector* _xc,
     /* Real    */ const ae_vector* _yc,
     /* Integer */ const ae_vector* dc,
     ae_int_t k,
     ae_int_t m,
     /* Real    */ ae_vector* c,
     lsfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector xc;
    ae_vector yc;
    ae_vector y2;
    ae_vector w2;
    ae_vector tmp;
    ae_vector tmp2;
    ae_vector tmpdiff;
    ae_vector bx;
    ae_vector by;
    ae_vector bw;
    ae_matrix fmatrix;
    ae_matrix cmatrix;
    ae_int_t i;
    ae_int_t j;
    double mx;
    double decay;

    ae_frame_make(_state, &_frame_block);
    memset(&xc, 0, sizeof(xc));
    memset(&yc, 0, sizeof(yc));
    memset(&y2, 0, sizeof(y2));
    memset(&w2, 0, sizeof(w2));
    memset(&tmp, 0, sizeof(tmp));
    memset(&tmp2, 0, sizeof(tmp2));
    memset(&tmpdiff, 0, sizeof(tmpdiff));
    memset(&bx, 0, sizeof(bx));
    memset(&by, 0, sizeof(by));
    memset(&bw, 0, sizeof(bw));
    memset(&fmatrix, 0, sizeof(fmatrix));
    memset(&cmatrix, 0, sizeof(cmatrix));
    ae_vector_init_copy(&xc, _xc, _state, ae_true);
    ae_vector_init_copy(&yc, _yc, _state, ae_true);
    ae_vector_clear(c);
    _lsfitreport_clear(rep);
    ae_vector_init(&y2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&w2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmpdiff, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bx, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&by, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bw, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&fmatrix, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&cmatrix, 0, 0, DT_REAL, _state, ae_true);

    lsfit_clearreport(rep, _state);
    
    /*
     * weight decay for correct handling of task which becomes
     * degenerate after constraints are applied
     */
    decay = (double)10000*ae_machineepsilon;
    
    /*
     * allocate space, initialize/fill:
     * * FMatrix-   values of basis functions at X[]
     * * CMatrix-   values (derivatives) of basis functions at XC[]
     * * fill constraints matrix
     * * fill first N rows of design matrix with values
     * * fill next M rows of design matrix with regularizing term
     * * append M zeros to Y
     * * append M elements, mean(abs(W)) each, to W
     */
    ae_vector_set_length(&y2, n+m, _state);
    ae_vector_set_length(&w2, n+m, _state);
    ae_vector_set_length(&tmp, m, _state);
    ae_vector_set_length(&tmpdiff, m, _state);
    ae_matrix_set_length(&fmatrix, n+m, m, _state);
    if( k>0 )
    {
        ae_matrix_set_length(&cmatrix, k, m+1, _state);
    }
    
    /*
     * Fill design matrix, Y2, W2:
     * * first N rows with basis functions for original points
     * * next M rows with decay terms
     */
    for(i=0; i<=n-1; i++)
    {
        
        /*
         * prepare Ith row
         * use Tmp for calculations to avoid multidimensional arrays overhead
         */
        for(j=0; j<=m-1; j++)
        {
            if( j==0 )
            {
                tmp.ptr.p_double[j] = (double)(1);
            }
            else
            {
                if( j==1 )
                {
                    tmp.ptr.p_double[j] = x->ptr.p_double[i];
                }
                else
                {
                    tmp.ptr.p_double[j] = (double)2*x->ptr.p_double[i]*tmp.ptr.p_double[j-1]-tmp.ptr.p_double[j-2];
                }
            }
        }
        ae_v_move(&fmatrix.ptr.pp_double[i][0], 1, &tmp.ptr.p_double[0], 1, ae_v_len(0,m-1));
    }
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=m-1; j++)
        {
            if( i==j )
            {
                fmatrix.ptr.pp_double[n+i][j] = decay;
            }
            else
            {
                fmatrix.ptr.pp_double[n+i][j] = (double)(0);
            }
        }
    }
    ae_v_move(&y2.ptr.p_double[0], 1, &y->ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&w2.ptr.p_double[0], 1, &w->ptr.p_double[0], 1, ae_v_len(0,n-1));
    mx = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        mx = mx+ae_fabs(w->ptr.p_double[i], _state);
    }
    mx = mx/(double)n;
    for(i=0; i<=m-1; i++)
    {
        y2.ptr.p_double[n+i] = (double)(0);
        w2.ptr.p_double[n+i] = mx;
    }
    
    /*
     * fill constraints matrix
     */
    for(i=0; i<=k-1; i++)
    {
        
        /*
         * prepare Ith row
         * use Tmp for basis function values,
         * TmpDiff for basos function derivatives
         */
        for(j=0; j<=m-1; j++)
        {
            if( j==0 )
            {
                tmp.ptr.p_double[j] = (double)(1);
                tmpdiff.ptr.p_double[j] = (double)(0);
            }
            else
            {
                if( j==1 )
                {
                    tmp.ptr.p_double[j] = xc.ptr.p_double[i];
                    tmpdiff.ptr.p_double[j] = (double)(1);
                }
                else
                {
                    tmp.ptr.p_double[j] = (double)2*xc.ptr.p_double[i]*tmp.ptr.p_double[j-1]-tmp.ptr.p_double[j-2];
                    tmpdiff.ptr.p_double[j] = (double)2*(tmp.ptr.p_double[j-1]+xc.ptr.p_double[i]*tmpdiff.ptr.p_double[j-1])-tmpdiff.ptr.p_double[j-2];
                }
            }
        }
        if( dc->ptr.p_int[i]==0 )
        {
            ae_v_move(&cmatrix.ptr.pp_double[i][0], 1, &tmp.ptr.p_double[0], 1, ae_v_len(0,m-1));
        }
        if( dc->ptr.p_int[i]==1 )
        {
            ae_v_move(&cmatrix.ptr.pp_double[i][0], 1, &tmpdiff.ptr.p_double[0], 1, ae_v_len(0,m-1));
        }
        cmatrix.ptr.pp_double[i][m] = yc.ptr.p_double[i];
    }
    
    /*
     * Solve constrained task
     */
    if( k>0 )
    {
        
        /*
         * solve using regularization
         */
        lsfitlinearwc(&y2, &w2, &fmatrix, &cmatrix, n+m, m, k, c, rep, _state);
    }
    else
    {
        
        /*
         * no constraints, no regularization needed
         */
        lsfitlinearwc(y, w, &fmatrix, &cmatrix, n, m, 0, c, rep, _state);
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Internal Floater-Hormann fitting subroutine for fixed D
*************************************************************************/
static void lsfit_barycentricfitwcfixedd(/* Real    */ const ae_vector* _x,
     /* Real    */ const ae_vector* _y,
     /* Real    */ const ae_vector* _w,
     ae_int_t n,
     /* Real    */ const ae_vector* _xc,
     /* Real    */ const ae_vector* _yc,
     /* Integer */ const ae_vector* dc,
     ae_int_t k,
     ae_int_t m,
     ae_int_t d,
     ae_int_t* info,
     barycentricinterpolant* b,
     barycentricfitreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector x;
    ae_vector y;
    ae_vector w;
    ae_vector xc;
    ae_vector yc;
    ae_matrix fmatrix;
    ae_matrix cmatrix;
    ae_vector y2;
    ae_vector w2;
    ae_vector sx;
    ae_vector sy;
    ae_vector sbf;
    ae_vector xoriginal;
    ae_vector yoriginal;
    ae_vector tmp;
    lsfitreport lrep;
    double v0;
    double v1;
    double mx;
    barycentricinterpolant b2;
    ae_int_t i;
    ae_int_t j;
    ae_int_t relcnt;
    double xa;
    double xb;
    double sa;
    double sb;
    double decay;

    ae_frame_make(_state, &_frame_block);
    memset(&x, 0, sizeof(x));
    memset(&y, 0, sizeof(y));
    memset(&w, 0, sizeof(w));
    memset(&xc, 0, sizeof(xc));
    memset(&yc, 0, sizeof(yc));
    memset(&fmatrix, 0, sizeof(fmatrix));
    memset(&cmatrix, 0, sizeof(cmatrix));
    memset(&y2, 0, sizeof(y2));
    memset(&w2, 0, sizeof(w2));
    memset(&sx, 0, sizeof(sx));
    memset(&sy, 0, sizeof(sy));
    memset(&sbf, 0, sizeof(sbf));
    memset(&xoriginal, 0, sizeof(xoriginal));
    memset(&yoriginal, 0, sizeof(yoriginal));
    memset(&tmp, 0, sizeof(tmp));
    memset(&lrep, 0, sizeof(lrep));
    memset(&b2, 0, sizeof(b2));
    ae_vector_init_copy(&x, _x, _state, ae_true);
    ae_vector_init_copy(&y, _y, _state, ae_true);
    ae_vector_init_copy(&w, _w, _state, ae_true);
    ae_vector_init_copy(&xc, _xc, _state, ae_true);
    ae_vector_init_copy(&yc, _yc, _state, ae_true);
    *info = 0;
    _barycentricinterpolant_clear(b);
    _barycentricfitreport_clear(rep);
    ae_matrix_init(&fmatrix, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&cmatrix, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&y2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&w2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&sx, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&sy, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&sbf, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xoriginal, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&yoriginal, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp, 0, DT_REAL, _state, ae_true);
    _lsfitreport_init(&lrep, _state, ae_true);
    _barycentricinterpolant_init(&b2, _state, ae_true);

    if( ((n<1||m<2)||k<0)||k>=m )
    {
        *info = -1;
        ae_frame_leave(_state);
        return;
    }
    for(i=0; i<=k-1; i++)
    {
        *info = 0;
        if( dc->ptr.p_int[i]<0 )
        {
            *info = -1;
        }
        if( dc->ptr.p_int[i]>1 )
        {
            *info = -1;
        }
        if( *info<0 )
        {
            ae_frame_leave(_state);
            return;
        }
    }
    
    /*
     * weight decay for correct handling of task which becomes
     * degenerate after constraints are applied
     */
    decay = (double)10000*ae_machineepsilon;
    
    /*
     * Scale X, Y, XC, YC
     */
    lsfitscalexy(&x, &y, &w, n, &xc, &yc, dc, k, &xa, &xb, &sa, &sb, &xoriginal, &yoriginal, _state);
    
    /*
     * allocate space, initialize:
     * * FMatrix-   values of basis functions at X[]
     * * CMatrix-   values (derivatives) of basis functions at XC[]
     */
    ae_vector_set_length(&y2, n+m, _state);
    ae_vector_set_length(&w2, n+m, _state);
    ae_matrix_set_length(&fmatrix, n+m, m, _state);
    if( k>0 )
    {
        ae_matrix_set_length(&cmatrix, k, m+1, _state);
    }
    ae_vector_set_length(&y2, n+m, _state);
    ae_vector_set_length(&w2, n+m, _state);
    
    /*
     * Prepare design and constraints matrices:
     * * fill constraints matrix
     * * fill first N rows of design matrix with values
     * * fill next M rows of design matrix with regularizing term
     * * append M zeros to Y
     * * append M elements, mean(abs(W)) each, to W
     */
    ae_vector_set_length(&sx, m, _state);
    ae_vector_set_length(&sy, m, _state);
    ae_vector_set_length(&sbf, m, _state);
    for(j=0; j<=m-1; j++)
    {
        sx.ptr.p_double[j] = (double)(2*j)/(double)(m-1)-(double)1;
    }
    for(i=0; i<=m-1; i++)
    {
        sy.ptr.p_double[i] = (double)(1);
    }
    barycentricbuildfloaterhormann(&sx, &sy, m, d, &b2, _state);
    mx = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        lsfit_barycentriccalcbasis(&b2, x.ptr.p_double[i], &sbf, _state);
        ae_v_move(&fmatrix.ptr.pp_double[i][0], 1, &sbf.ptr.p_double[0], 1, ae_v_len(0,m-1));
        y2.ptr.p_double[i] = y.ptr.p_double[i];
        w2.ptr.p_double[i] = w.ptr.p_double[i];
        mx = mx+ae_fabs(w.ptr.p_double[i], _state)/(double)n;
    }
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=m-1; j++)
        {
            if( i==j )
            {
                fmatrix.ptr.pp_double[n+i][j] = decay;
            }
            else
            {
                fmatrix.ptr.pp_double[n+i][j] = (double)(0);
            }
        }
        y2.ptr.p_double[n+i] = (double)(0);
        w2.ptr.p_double[n+i] = mx;
    }
    if( k>0 )
    {
        for(j=0; j<=m-1; j++)
        {
            for(i=0; i<=m-1; i++)
            {
                sy.ptr.p_double[i] = (double)(0);
            }
            sy.ptr.p_double[j] = (double)(1);
            barycentricbuildfloaterhormann(&sx, &sy, m, d, &b2, _state);
            for(i=0; i<=k-1; i++)
            {
                ae_assert(dc->ptr.p_int[i]>=0&&dc->ptr.p_int[i]<=1, "BarycentricFit: internal error!", _state);
                barycentricdiff1(&b2, xc.ptr.p_double[i], &v0, &v1, _state);
                if( dc->ptr.p_int[i]==0 )
                {
                    cmatrix.ptr.pp_double[i][j] = v0;
                }
                if( dc->ptr.p_int[i]==1 )
                {
                    cmatrix.ptr.pp_double[i][j] = v1;
                }
            }
        }
        for(i=0; i<=k-1; i++)
        {
            cmatrix.ptr.pp_double[i][m] = yc.ptr.p_double[i];
        }
    }
    
    /*
     * Solve constrained task
     */
    if( k>0 )
    {
        
        /*
         * solve using regularization
         */
        lsfitlinearwc(&y2, &w2, &fmatrix, &cmatrix, n+m, m, k, &tmp, &lrep, _state);
    }
    else
    {
        
        /*
         * no constraints, no regularization needed
         */
        lsfitlinearwc(&y, &w, &fmatrix, &cmatrix, n, m, k, &tmp, &lrep, _state);
    }
    *info = lrep.terminationtype;
    if( *info<0 )
    {
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Generate interpolant and scale it
     */
    ae_v_move(&sy.ptr.p_double[0], 1, &tmp.ptr.p_double[0], 1, ae_v_len(0,m-1));
    barycentricbuildfloaterhormann(&sx, &sy, m, d, b, _state);
    barycentriclintransx(b, (double)2/(xb-xa), -(xa+xb)/(xb-xa), _state);
    barycentriclintransy(b, sb-sa, sa, _state);
    
    /*
     * Scale absolute errors obtained from LSFitLinearW.
     * Relative error should be calculated separately
     * (because of shifting/scaling of the task)
     */
    rep->taskrcond = lrep.taskrcond;
    rep->rmserror = lrep.rmserror*(sb-sa);
    rep->avgerror = lrep.avgerror*(sb-sa);
    rep->maxerror = lrep.maxerror*(sb-sa);
    rep->avgrelerror = (double)(0);
    relcnt = 0;
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_neq(yoriginal.ptr.p_double[i],(double)(0)) )
        {
            rep->avgrelerror = rep->avgrelerror+ae_fabs(barycentriccalc(b, xoriginal.ptr.p_double[i], _state)-yoriginal.ptr.p_double[i], _state)/ae_fabs(yoriginal.ptr.p_double[i], _state);
            relcnt = relcnt+1;
        }
    }
    if( relcnt!=0 )
    {
        rep->avgrelerror = rep->avgrelerror/(double)relcnt;
    }
    ae_frame_leave(_state);
}


static void lsfit_clearreport(lsfitreport* rep, ae_state *_state)
{


    rep->taskrcond = (double)(0);
    rep->iterationscount = 0;
    rep->varidx = -1;
    rep->rmserror = (double)(0);
    rep->avgerror = (double)(0);
    rep->avgrelerror = (double)(0);
    rep->maxerror = (double)(0);
    rep->wrmserror = (double)(0);
    rep->r2 = (double)(0);
    ae_matrix_set_length(&rep->covpar, 0, 0, _state);
    ae_vector_set_length(&rep->errpar, 0, _state);
    ae_vector_set_length(&rep->errcurve, 0, _state);
    ae_vector_set_length(&rep->noise, 0, _state);
}


/*************************************************************************
This internal function estimates covariance matrix and other error-related
information for linear/nonlinear least squares model.

It has a bit awkward interface, but it can be used  for  both  linear  and
nonlinear problems.

INPUT PARAMETERS:
    F1  -   array[0..N-1,0..K-1]:
            * for linear problems - matrix of function values
            * for nonlinear problems - Jacobian matrix
    F0  -   array[0..N-1]:
            * for linear problems - must be filled with zeros
            * for nonlinear problems - must store values of function being
              fitted
    Y   -   array[0..N-1]:
            * for linear and nonlinear problems - must store target values
    W   -   weights, array[0..N-1]:
            * for linear and nonlinear problems - weights
    X   -   array[0..K-1]:
            * for linear and nonlinear problems - current solution
    S   -   array[0..K-1]:
            * its components should be strictly positive
            * squared inverse of this diagonal matrix is used as damping
              factor for covariance matrix (linear and nonlinear problems)
            * for nonlinear problems, when scale of the variables is usually
              explicitly given by user, you may use scale vector for this
              parameter
            * for linear problems you may set this parameter to
              S=sqrt(1/diag(F'*F))
            * this parameter is automatically rescaled by this function,
              only relative magnitudes of its components (with respect to
              each other) matter.
    N   -   number of points, N>0.
    K   -   number of dimensions
    Rep -   structure which is used to store results
    Z   -   additional matrix which, depending on ZKind, may contain some
            information used to accelerate calculations - or just can be
            temporary buffer:
            * for ZKind=0       Z contains no information, just temporary
                                buffer which can be resized and used as needed
            * for ZKind=1       Z contains triangular matrix from QR
                                decomposition of W*F1. This matrix can be used
                                to speedup calculation of covariance matrix.
                                It should not be changed by algorithm.
    ZKind-  contents of Z

OUTPUT PARAMETERS:

* Rep.CovPar        covariance matrix for parameters, array[K,K].
* Rep.ErrPar        errors in parameters, array[K],
                    errpar = sqrt(diag(CovPar))
* Rep.ErrCurve      vector of fit errors - standard deviations of empirical
                    best-fit curve from "ideal" best-fit curve built  with
                    infinite number of samples, array[N].
                    errcurve = sqrt(diag(J*CovPar*J')),
                    where J is Jacobian matrix.
* Rep.Noise         vector of per-point estimates of noise, array[N]
* Rep.R2            coefficient of determination (non-weighted)

Other fields of Rep are not changed.

IMPORTANT:  errors  in  parameters  are  calculated  without  taking  into
            account boundary/linear constraints! Presence  of  constraints
            changes distribution of errors, but there is no  easy  way  to
            account for constraints when you calculate covariance matrix.
            
NOTE:       noise in the data is estimated as follows:
            * for fitting without user-supplied  weights  all  points  are
              assumed to have same level of noise, which is estimated from
              the data
            * for fitting with user-supplied weights we assume that  noise
              level in I-th point is inversely proportional to Ith weight.
              Coefficient of proportionality is estimated from the data.
            
NOTE:       we apply small amount of regularization when we invert squared
            Jacobian and calculate covariance matrix. It  guarantees  that
            algorithm won't divide by zero  during  inversion,  but  skews
            error estimates a bit (fractional error is about 10^-9).
            
            However, we believe that this difference is insignificant  for
            all practical purposes except for the situation when you  want
            to compare ALGLIB results with "reference"  implementation  up
            to the last significant digit.

  -- ALGLIB PROJECT --
     Copyright 10.12.2009 by Bochkanov Sergey
*************************************************************************/
static void lsfit_estimateerrors(/* Real    */ const ae_matrix* f1,
     /* Real    */ const ae_vector* f0,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* w,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* _s,
     ae_int_t n,
     ae_int_t k,
     lsfitreport* rep,
     /* Real    */ ae_matrix* z,
     ae_int_t zkind,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector s;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j1;
    double v;
    double noisec;
    matinvreport invrep;
    ae_int_t nzcnt;
    double avg;
    double rss;
    double tss;
    double sz;
    double ss;

    ae_frame_make(_state, &_frame_block);
    memset(&s, 0, sizeof(s));
    memset(&invrep, 0, sizeof(invrep));
    ae_vector_init_copy(&s, _s, _state, ae_true);
    _matinvreport_init(&invrep, _state, ae_true);

    
    /*
     * Compute NZCnt - count of non-zero weights
     */
    nzcnt = 0;
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_neq(w->ptr.p_double[i],(double)(0)) )
        {
            nzcnt = nzcnt+1;
        }
    }
    
    /*
     * Compute R2
     */
    if( nzcnt>0 )
    {
        avg = 0.0;
        for(i=0; i<=n-1; i++)
        {
            if( ae_fp_neq(w->ptr.p_double[i],(double)(0)) )
            {
                avg = avg+y->ptr.p_double[i];
            }
        }
        avg = avg/(double)nzcnt;
        rss = 0.0;
        tss = 0.0;
        for(i=0; i<=n-1; i++)
        {
            if( ae_fp_neq(w->ptr.p_double[i],(double)(0)) )
            {
                v = ae_v_dotproduct(&f1->ptr.pp_double[i][0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,k-1));
                v = v+f0->ptr.p_double[i];
                rss = rss+ae_sqr(v-y->ptr.p_double[i], _state);
                tss = tss+ae_sqr(y->ptr.p_double[i]-avg, _state);
            }
        }
        if( ae_fp_neq(tss,(double)(0)) )
        {
            rep->r2 = ae_maxreal(1.0-rss/tss, 0.0, _state);
        }
        else
        {
            rep->r2 = 1.0;
        }
    }
    else
    {
        rep->r2 = (double)(0);
    }
    
    /*
     * Compute estimate of proportionality between noise in the data and weights:
     *     NoiseC = mean(per-point-noise*per-point-weight)
     * Noise level (standard deviation) at each point is equal to NoiseC/W[I].
     */
    if( nzcnt>k )
    {
        noisec = 0.0;
        for(i=0; i<=n-1; i++)
        {
            if( ae_fp_neq(w->ptr.p_double[i],(double)(0)) )
            {
                v = ae_v_dotproduct(&f1->ptr.pp_double[i][0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,k-1));
                v = v+f0->ptr.p_double[i];
                noisec = noisec+ae_sqr((v-y->ptr.p_double[i])*w->ptr.p_double[i], _state);
            }
        }
        noisec = ae_sqrt(noisec/(double)(nzcnt-k), _state);
    }
    else
    {
        noisec = 0.0;
    }
    
    /*
     * Two branches on noise level:
     * * NoiseC>0   normal situation
     * * NoiseC=0   degenerate case CovPar is filled by zeros
     */
    rmatrixsetlengthatleast(&rep->covpar, k, k, _state);
    if( ae_fp_greater(noisec,(double)(0)) )
    {
        
        /*
         * Normal situation: non-zero noise level
         */
        ae_assert(zkind==0||zkind==1, "LSFit: internal error in EstimateErrors() function", _state);
        if( zkind==0 )
        {
            
            /*
             * Z contains no additional information which can be used to speed up
             * calculations. We have to calculate covariance matrix on our own:
             * * Compute scaled Jacobian N*J, where N[i,i]=WCur[I]/NoiseC, store in Z
             * * Compute Z'*Z, store in CovPar
             * * Apply moderate regularization to CovPar and compute matrix inverse.
             *   In case inverse failed, increase regularization parameter and try
             *   again.
             */
            rmatrixsetlengthatleast(z, n, k, _state);
            for(i=0; i<=n-1; i++)
            {
                v = w->ptr.p_double[i]/noisec;
                ae_v_moved(&z->ptr.pp_double[i][0], 1, &f1->ptr.pp_double[i][0], 1, ae_v_len(0,k-1), v);
            }
            
            /*
             * Convert S to automatically scaled damped matrix:
             * * calculate SZ - sum of diagonal elements of Z'*Z
             * * calculate SS - sum of diagonal elements of S^(-2)
             * * overwrite S by (SZ/SS)*S^(-2)
             * * now S has approximately same magnitude as giagonal of Z'*Z
             */
            sz = (double)(0);
            for(i=0; i<=n-1; i++)
            {
                for(j=0; j<=k-1; j++)
                {
                    sz = sz+z->ptr.pp_double[i][j]*z->ptr.pp_double[i][j];
                }
            }
            if( ae_fp_eq(sz,(double)(0)) )
            {
                sz = (double)(1);
            }
            ss = (double)(0);
            for(j=0; j<=k-1; j++)
            {
                ss = ss+(double)1/ae_sqr(s.ptr.p_double[j], _state);
            }
            for(j=0; j<=k-1; j++)
            {
                s.ptr.p_double[j] = sz/ss/ae_sqr(s.ptr.p_double[j], _state);
            }
            
            /*
             * Calculate damped inverse inv(Z'*Z+S).
             * We increase damping factor V until Z'*Z become well-conditioned.
             */
            v = 1.0E3*ae_machineepsilon;
            do
            {
                rmatrixsyrk(k, n, 1.0, z, 0, 0, 2, 0.0, &rep->covpar, 0, 0, ae_true, _state);
                for(i=0; i<=k-1; i++)
                {
                    rep->covpar.ptr.pp_double[i][i] = rep->covpar.ptr.pp_double[i][i]+v*s.ptr.p_double[i];
                }
                spdmatrixinverse(&rep->covpar, k, ae_true, &invrep, _state);
                v = (double)10*v;
            }
            while(invrep.terminationtype<=0);
            for(i=0; i<=k-1; i++)
            {
                for(j=i+1; j<=k-1; j++)
                {
                    rep->covpar.ptr.pp_double[j][i] = rep->covpar.ptr.pp_double[i][j];
                }
            }
        }
        if( zkind==1 )
        {
            
            /*
             * We can reuse additional information:
             * * Z contains R matrix from QR decomposition of W*F1 
             * * After multiplication by 1/NoiseC we get Z_mod = N*F1, where diag(N)=w[i]/NoiseC
             * * Such triangular Z_mod is a Cholesky factor from decomposition of J'*N'*N*J.
             *   Thus, we can calculate covariance matrix as inverse of the matrix given by
             *   its Cholesky decomposition. It allow us to avoid time-consuming calculation
             *   of J'*N'*N*J in CovPar - complexity is reduced from O(N*K^2) to O(K^3), which
             *   is quite good because K is usually orders of magnitude smaller than N.
             *
             * First, convert S to automatically scaled damped matrix:
             * * calculate SZ - sum of magnitudes of diagonal elements of Z/NoiseC
             * * calculate SS - sum of diagonal elements of S^(-1)
             * * overwrite S by (SZ/SS)*S^(-1)
             * * now S has approximately same magnitude as giagonal of Z'*Z
             */
            sz = (double)(0);
            for(j=0; j<=k-1; j++)
            {
                sz = sz+ae_fabs(z->ptr.pp_double[j][j]/noisec, _state);
            }
            if( ae_fp_eq(sz,(double)(0)) )
            {
                sz = (double)(1);
            }
            ss = (double)(0);
            for(j=0; j<=k-1; j++)
            {
                ss = ss+(double)1/s.ptr.p_double[j];
            }
            for(j=0; j<=k-1; j++)
            {
                s.ptr.p_double[j] = sz/ss/s.ptr.p_double[j];
            }
            
            /*
             * Calculate damped inverse of inv((Z+v*S)'*(Z+v*S))
             * We increase damping factor V until matrix become well-conditioned.
             */
            v = 1.0E3*ae_machineepsilon;
            do
            {
                for(i=0; i<=k-1; i++)
                {
                    for(j=i; j<=k-1; j++)
                    {
                        rep->covpar.ptr.pp_double[i][j] = z->ptr.pp_double[i][j]/noisec;
                    }
                    rep->covpar.ptr.pp_double[i][i] = rep->covpar.ptr.pp_double[i][i]+v*s.ptr.p_double[i];
                }
                spdmatrixcholeskyinverse(&rep->covpar, k, ae_true, &invrep, _state);
                v = (double)10*v;
            }
            while(invrep.terminationtype<=0);
            for(i=0; i<=k-1; i++)
            {
                for(j=i+1; j<=k-1; j++)
                {
                    rep->covpar.ptr.pp_double[j][i] = rep->covpar.ptr.pp_double[i][j];
                }
            }
        }
    }
    else
    {
        
        /*
         * Degenerate situation: zero noise level, covariance matrix is zero.
         */
        for(i=0; i<=k-1; i++)
        {
            for(j=0; j<=k-1; j++)
            {
                rep->covpar.ptr.pp_double[j][i] = (double)(0);
            }
        }
    }
    
    /*
     * Estimate erorrs in parameters, curve and per-point noise
     */
    rvectorsetlengthatleast(&rep->errpar, k, _state);
    rvectorsetlengthatleast(&rep->errcurve, n, _state);
    rvectorsetlengthatleast(&rep->noise, n, _state);
    for(i=0; i<=k-1; i++)
    {
        rep->errpar.ptr.p_double[i] = ae_sqrt(rep->covpar.ptr.pp_double[i][i], _state);
    }
    for(i=0; i<=n-1; i++)
    {
        
        /*
         * ErrCurve[I] is sqrt(P[i,i]) where P=J*CovPar*J'
         */
        v = 0.0;
        for(j=0; j<=k-1; j++)
        {
            for(j1=0; j1<=k-1; j1++)
            {
                v = v+f1->ptr.pp_double[i][j]*rep->covpar.ptr.pp_double[j][j1]*f1->ptr.pp_double[i][j1];
            }
        }
        rep->errcurve.ptr.p_double[i] = ae_sqrt(v, _state);
        
        /*
         * Noise[i] is filled using weights and current estimate of noise level
         */
        if( ae_fp_neq(w->ptr.p_double[i],(double)(0)) )
        {
            rep->noise.ptr.p_double[i] = noisec/w->ptr.p_double[i];
        }
        else
        {
            rep->noise.ptr.p_double[i] = (double)(0);
        }
    }
    ae_frame_leave(_state);
}


void _polynomialfitreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    polynomialfitreport *p = (polynomialfitreport*)_p;
    ae_touch_ptr((void*)p);
}


void _polynomialfitreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    polynomialfitreport       *dst = (polynomialfitreport*)_dst;
    const polynomialfitreport *src = (const polynomialfitreport*)_src;
    dst->terminationtype = src->terminationtype;
    dst->taskrcond = src->taskrcond;
    dst->rmserror = src->rmserror;
    dst->avgerror = src->avgerror;
    dst->avgrelerror = src->avgrelerror;
    dst->maxerror = src->maxerror;
}


void _polynomialfitreport_clear(void* _p)
{
    polynomialfitreport *p = (polynomialfitreport*)_p;
    ae_touch_ptr((void*)p);
}


void _polynomialfitreport_destroy(void* _p)
{
    polynomialfitreport *p = (polynomialfitreport*)_p;
    ae_touch_ptr((void*)p);
}


void _barycentricfitreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    barycentricfitreport *p = (barycentricfitreport*)_p;
    ae_touch_ptr((void*)p);
}


void _barycentricfitreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    barycentricfitreport       *dst = (barycentricfitreport*)_dst;
    const barycentricfitreport *src = (const barycentricfitreport*)_src;
    dst->terminationtype = src->terminationtype;
    dst->taskrcond = src->taskrcond;
    dst->dbest = src->dbest;
    dst->rmserror = src->rmserror;
    dst->avgerror = src->avgerror;
    dst->avgrelerror = src->avgrelerror;
    dst->maxerror = src->maxerror;
}


void _barycentricfitreport_clear(void* _p)
{
    barycentricfitreport *p = (barycentricfitreport*)_p;
    ae_touch_ptr((void*)p);
}


void _barycentricfitreport_destroy(void* _p)
{
    barycentricfitreport *p = (barycentricfitreport*)_p;
    ae_touch_ptr((void*)p);
}


void _lsfitreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    lsfitreport *p = (lsfitreport*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_init(&p->covpar, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->errpar, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->errcurve, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->noise, 0, DT_REAL, _state, make_automatic);
}


void _lsfitreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    lsfitreport       *dst = (lsfitreport*)_dst;
    const lsfitreport *src = (const lsfitreport*)_src;
    dst->terminationtype = src->terminationtype;
    dst->taskrcond = src->taskrcond;
    dst->iterationscount = src->iterationscount;
    dst->varidx = src->varidx;
    dst->rmserror = src->rmserror;
    dst->avgerror = src->avgerror;
    dst->avgrelerror = src->avgrelerror;
    dst->maxerror = src->maxerror;
    dst->wrmserror = src->wrmserror;
    ae_matrix_init_copy(&dst->covpar, &src->covpar, _state, make_automatic);
    ae_vector_init_copy(&dst->errpar, &src->errpar, _state, make_automatic);
    ae_vector_init_copy(&dst->errcurve, &src->errcurve, _state, make_automatic);
    ae_vector_init_copy(&dst->noise, &src->noise, _state, make_automatic);
    dst->r2 = src->r2;
}


void _lsfitreport_clear(void* _p)
{
    lsfitreport *p = (lsfitreport*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_clear(&p->covpar);
    ae_vector_clear(&p->errpar);
    ae_vector_clear(&p->errcurve);
    ae_vector_clear(&p->noise);
}


void _lsfitreport_destroy(void* _p)
{
    lsfitreport *p = (lsfitreport*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_destroy(&p->covpar);
    ae_vector_destroy(&p->errpar);
    ae_vector_destroy(&p->errcurve);
    ae_vector_destroy(&p->noise);
}


void _lsfitstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    lsfitstate *p = (lsfitstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->c0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->c1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->taskx, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tasky, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->taskw, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->cleic, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->c, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->g, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->reportx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->querydata, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->replyfi, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->replydj, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->replysj, _state, make_automatic);
    ae_vector_init(&p->tmpx1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpc1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpf1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpg1, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->tmpj1, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->wcur, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpwk, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpct, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->tmp, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpf, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->tmpjac, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->tmpjacw, 0, 0, DT_REAL, _state, make_automatic);
    _lsfitreport_init(&p->rep, _state, make_automatic);
    _minlmstate_init(&p->optstate, _state, make_automatic);
    _minlmreport_init(&p->optrep, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
}


void _lsfitstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    lsfitstate       *dst = (lsfitstate*)_dst;
    const lsfitstate *src = (const lsfitstate*)_src;
    dst->protocolversion = src->protocolversion;
    dst->optalgo = src->optalgo;
    dst->m = src->m;
    dst->k = src->k;
    dst->epsx = src->epsx;
    dst->maxits = src->maxits;
    dst->stpmax = src->stpmax;
    dst->xrep = src->xrep;
    ae_vector_init_copy(&dst->c0, &src->c0, _state, make_automatic);
    ae_vector_init_copy(&dst->c1, &src->c1, _state, make_automatic);
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic);
    ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic);
    ae_matrix_init_copy(&dst->taskx, &src->taskx, _state, make_automatic);
    ae_vector_init_copy(&dst->tasky, &src->tasky, _state, make_automatic);
    dst->npoints = src->npoints;
    ae_vector_init_copy(&dst->taskw, &src->taskw, _state, make_automatic);
    dst->nweights = src->nweights;
    dst->wkind = src->wkind;
    dst->wits = src->wits;
    dst->diffstep = src->diffstep;
    dst->teststep = src->teststep;
    ae_matrix_init_copy(&dst->cleic, &src->cleic, _state, make_automatic);
    dst->nec = src->nec;
    dst->nic = src->nic;
    dst->xupdated = src->xupdated;
    dst->needf = src->needf;
    dst->needfg = src->needfg;
    dst->pointindex = src->pointindex;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->c, &src->c, _state, make_automatic);
    dst->f = src->f;
    ae_vector_init_copy(&dst->g, &src->g, _state, make_automatic);
    dst->requesttype = src->requesttype;
    ae_vector_init_copy(&dst->reportx, &src->reportx, _state, make_automatic);
    dst->reportf = src->reportf;
    dst->querysize = src->querysize;
    dst->queryfuncs = src->queryfuncs;
    dst->queryvars = src->queryvars;
    dst->querydim = src->querydim;
    dst->queryformulasize = src->queryformulasize;
    ae_vector_init_copy(&dst->querydata, &src->querydata, _state, make_automatic);
    ae_vector_init_copy(&dst->replyfi, &src->replyfi, _state, make_automatic);
    ae_vector_init_copy(&dst->replydj, &src->replydj, _state, make_automatic);
    _sparsematrix_init_copy(&dst->replysj, &src->replysj, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpx1, &src->tmpx1, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpc1, &src->tmpc1, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpf1, &src->tmpf1, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpg1, &src->tmpg1, _state, make_automatic);
    ae_matrix_init_copy(&dst->tmpj1, &src->tmpj1, _state, make_automatic);
    ae_vector_init_copy(&dst->wcur, &src->wcur, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpwk, &src->tmpwk, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpct, &src->tmpct, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp, &src->tmp, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpf, &src->tmpf, _state, make_automatic);
    ae_matrix_init_copy(&dst->tmpjac, &src->tmpjac, _state, make_automatic);
    ae_matrix_init_copy(&dst->tmpjacw, &src->tmpjacw, _state, make_automatic);
    dst->tmpnoise = src->tmpnoise;
    dst->repiterationscount = src->repiterationscount;
    dst->repterminationtype = src->repterminationtype;
    dst->repvaridx = src->repvaridx;
    dst->reprmserror = src->reprmserror;
    dst->repavgerror = src->repavgerror;
    dst->repavgrelerror = src->repavgrelerror;
    dst->repmaxerror = src->repmaxerror;
    dst->repwrmserror = src->repwrmserror;
    _lsfitreport_init_copy(&dst->rep, &src->rep, _state, make_automatic);
    _minlmstate_init_copy(&dst->optstate, &src->optstate, _state, make_automatic);
    _minlmreport_init_copy(&dst->optrep, &src->optrep, _state, make_automatic);
    dst->prevnpt = src->prevnpt;
    dst->prevalgo = src->prevalgo;
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
}


void _lsfitstate_clear(void* _p)
{
    lsfitstate *p = (lsfitstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->c0);
    ae_vector_clear(&p->c1);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_matrix_clear(&p->taskx);
    ae_vector_clear(&p->tasky);
    ae_vector_clear(&p->taskw);
    ae_matrix_clear(&p->cleic);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->c);
    ae_vector_clear(&p->g);
    ae_vector_clear(&p->reportx);
    ae_vector_clear(&p->querydata);
    ae_vector_clear(&p->replyfi);
    ae_vector_clear(&p->replydj);
    _sparsematrix_clear(&p->replysj);
    ae_vector_clear(&p->tmpx1);
    ae_vector_clear(&p->tmpc1);
    ae_vector_clear(&p->tmpf1);
    ae_vector_clear(&p->tmpg1);
    ae_matrix_clear(&p->tmpj1);
    ae_vector_clear(&p->wcur);
    ae_vector_clear(&p->tmpwk);
    ae_vector_clear(&p->tmpct);
    ae_vector_clear(&p->tmp);
    ae_vector_clear(&p->tmpf);
    ae_matrix_clear(&p->tmpjac);
    ae_matrix_clear(&p->tmpjacw);
    _lsfitreport_clear(&p->rep);
    _minlmstate_clear(&p->optstate);
    _minlmreport_clear(&p->optrep);
    _rcommstate_clear(&p->rstate);
}


void _lsfitstate_destroy(void* _p)
{
    lsfitstate *p = (lsfitstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->c0);
    ae_vector_destroy(&p->c1);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_matrix_destroy(&p->taskx);
    ae_vector_destroy(&p->tasky);
    ae_vector_destroy(&p->taskw);
    ae_matrix_destroy(&p->cleic);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->c);
    ae_vector_destroy(&p->g);
    ae_vector_destroy(&p->reportx);
    ae_vector_destroy(&p->querydata);
    ae_vector_destroy(&p->replyfi);
    ae_vector_destroy(&p->replydj);
    _sparsematrix_destroy(&p->replysj);
    ae_vector_destroy(&p->tmpx1);
    ae_vector_destroy(&p->tmpc1);
    ae_vector_destroy(&p->tmpf1);
    ae_vector_destroy(&p->tmpg1);
    ae_matrix_destroy(&p->tmpj1);
    ae_vector_destroy(&p->wcur);
    ae_vector_destroy(&p->tmpwk);
    ae_vector_destroy(&p->tmpct);
    ae_vector_destroy(&p->tmp);
    ae_vector_destroy(&p->tmpf);
    ae_matrix_destroy(&p->tmpjac);
    ae_matrix_destroy(&p->tmpjacw);
    _lsfitreport_destroy(&p->rep);
    _minlmstate_destroy(&p->optstate);
    _minlmreport_destroy(&p->optrep);
    _rcommstate_destroy(&p->rstate);
}


/*$ End $*/
