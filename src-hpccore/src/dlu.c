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
#include "dlu.h"


/*$ Declarations $*/
static void dlu_cmatrixlup2(/* Complex */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     /* Complex */ ae_vector* tmp,
     ae_state *_state);
static void dlu_rmatrixlup2(/* Real    */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     /* Real    */ ae_vector* tmp,
     ae_state *_state);
static void dlu_cmatrixplu2(/* Complex */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     /* Complex */ ae_vector* tmp,
     ae_state *_state);
static void dlu_rmatrixplu2(/* Real    */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     /* Real    */ ae_vector* tmp,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
Recurrent complex LU subroutine.
Never call it directly.

  -- ALGLIB routine --
     04.01.2010
     Bochkanov Sergey
*************************************************************************/
void cmatrixluprec(/* Complex */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     /* Complex */ ae_vector* tmp,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t m1;
    ae_int_t m2;


    if( ae_minint(m, n, _state)<=ablascomplexblocksize(a, _state) )
    {
        dlu_cmatrixlup2(a, offs, m, n, pivots, tmp, _state);
        return;
    }
    if( m>n )
    {
        cmatrixluprec(a, offs, n, n, pivots, tmp, _state);
        for(i=0; i<=n-1; i++)
        {
            ae_v_cmove(&tmp->ptr.p_complex[0], 1, &a->ptr.pp_complex[offs+n][offs+i], a->stride, "N", ae_v_len(0,m-n-1));
            ae_v_cmove(&a->ptr.pp_complex[offs+n][offs+i], a->stride, &a->ptr.pp_complex[offs+n][pivots->ptr.p_int[offs+i]], a->stride, "N", ae_v_len(offs+n,offs+m-1));
            ae_v_cmove(&a->ptr.pp_complex[offs+n][pivots->ptr.p_int[offs+i]], a->stride, &tmp->ptr.p_complex[0], 1, "N", ae_v_len(offs+n,offs+m-1));
        }
        cmatrixrighttrsm(m-n, n, a, offs, offs, ae_true, ae_true, 0, a, offs+n, offs, _state);
        return;
    }
    ablascomplexsplitlength(a, m, &m1, &m2, _state);
    cmatrixluprec(a, offs, m1, n, pivots, tmp, _state);
    if( m2>0 )
    {
        for(i=0; i<=m1-1; i++)
        {
            if( offs+i!=pivots->ptr.p_int[offs+i] )
            {
                ae_v_cmove(&tmp->ptr.p_complex[0], 1, &a->ptr.pp_complex[offs+m1][offs+i], a->stride, "N", ae_v_len(0,m2-1));
                ae_v_cmove(&a->ptr.pp_complex[offs+m1][offs+i], a->stride, &a->ptr.pp_complex[offs+m1][pivots->ptr.p_int[offs+i]], a->stride, "N", ae_v_len(offs+m1,offs+m-1));
                ae_v_cmove(&a->ptr.pp_complex[offs+m1][pivots->ptr.p_int[offs+i]], a->stride, &tmp->ptr.p_complex[0], 1, "N", ae_v_len(offs+m1,offs+m-1));
            }
        }
        cmatrixrighttrsm(m2, m1, a, offs, offs, ae_true, ae_true, 0, a, offs+m1, offs, _state);
        cmatrixgemm(m-m1, n-m1, m1, ae_complex_from_d(-1.0), a, offs+m1, offs, 0, a, offs, offs+m1, 0, ae_complex_from_d(1.0), a, offs+m1, offs+m1, _state);
        cmatrixluprec(a, offs+m1, m-m1, n-m1, pivots, tmp, _state);
        for(i=0; i<=m2-1; i++)
        {
            if( offs+m1+i!=pivots->ptr.p_int[offs+m1+i] )
            {
                ae_v_cmove(&tmp->ptr.p_complex[0], 1, &a->ptr.pp_complex[offs][offs+m1+i], a->stride, "N", ae_v_len(0,m1-1));
                ae_v_cmove(&a->ptr.pp_complex[offs][offs+m1+i], a->stride, &a->ptr.pp_complex[offs][pivots->ptr.p_int[offs+m1+i]], a->stride, "N", ae_v_len(offs,offs+m1-1));
                ae_v_cmove(&a->ptr.pp_complex[offs][pivots->ptr.p_int[offs+m1+i]], a->stride, &tmp->ptr.p_complex[0], 1, "N", ae_v_len(offs,offs+m1-1));
            }
        }
    }
}


/*************************************************************************
Recurrent real LU subroutine.
Never call it directly.

  -- ALGLIB routine --
     04.01.2010
     Bochkanov Sergey
*************************************************************************/
void rmatrixluprec(/* Real    */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     /* Real    */ ae_vector* tmp,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t m1;
    ae_int_t m2;


    if( ae_minint(m, n, _state)<=ablasblocksize(a, _state) )
    {
        dlu_rmatrixlup2(a, offs, m, n, pivots, tmp, _state);
        return;
    }
    if( m>n )
    {
        rmatrixluprec(a, offs, n, n, pivots, tmp, _state);
        for(i=0; i<=n-1; i++)
        {
            if( offs+i!=pivots->ptr.p_int[offs+i] )
            {
                ae_v_move(&tmp->ptr.p_double[0], 1, &a->ptr.pp_double[offs+n][offs+i], a->stride, ae_v_len(0,m-n-1));
                ae_v_move(&a->ptr.pp_double[offs+n][offs+i], a->stride, &a->ptr.pp_double[offs+n][pivots->ptr.p_int[offs+i]], a->stride, ae_v_len(offs+n,offs+m-1));
                ae_v_move(&a->ptr.pp_double[offs+n][pivots->ptr.p_int[offs+i]], a->stride, &tmp->ptr.p_double[0], 1, ae_v_len(offs+n,offs+m-1));
            }
        }
        rmatrixrighttrsm(m-n, n, a, offs, offs, ae_true, ae_true, 0, a, offs+n, offs, _state);
        return;
    }
    ablassplitlength(a, m, &m1, &m2, _state);
    rmatrixluprec(a, offs, m1, n, pivots, tmp, _state);
    if( m2>0 )
    {
        for(i=0; i<=m1-1; i++)
        {
            if( offs+i!=pivots->ptr.p_int[offs+i] )
            {
                ae_v_move(&tmp->ptr.p_double[0], 1, &a->ptr.pp_double[offs+m1][offs+i], a->stride, ae_v_len(0,m2-1));
                ae_v_move(&a->ptr.pp_double[offs+m1][offs+i], a->stride, &a->ptr.pp_double[offs+m1][pivots->ptr.p_int[offs+i]], a->stride, ae_v_len(offs+m1,offs+m-1));
                ae_v_move(&a->ptr.pp_double[offs+m1][pivots->ptr.p_int[offs+i]], a->stride, &tmp->ptr.p_double[0], 1, ae_v_len(offs+m1,offs+m-1));
            }
        }
        rmatrixrighttrsm(m2, m1, a, offs, offs, ae_true, ae_true, 0, a, offs+m1, offs, _state);
        rmatrixgemm(m-m1, n-m1, m1, -1.0, a, offs+m1, offs, 0, a, offs, offs+m1, 0, 1.0, a, offs+m1, offs+m1, _state);
        rmatrixluprec(a, offs+m1, m-m1, n-m1, pivots, tmp, _state);
        for(i=0; i<=m2-1; i++)
        {
            if( offs+m1+i!=pivots->ptr.p_int[offs+m1+i] )
            {
                ae_v_move(&tmp->ptr.p_double[0], 1, &a->ptr.pp_double[offs][offs+m1+i], a->stride, ae_v_len(0,m1-1));
                ae_v_move(&a->ptr.pp_double[offs][offs+m1+i], a->stride, &a->ptr.pp_double[offs][pivots->ptr.p_int[offs+m1+i]], a->stride, ae_v_len(offs,offs+m1-1));
                ae_v_move(&a->ptr.pp_double[offs][pivots->ptr.p_int[offs+m1+i]], a->stride, &tmp->ptr.p_double[0], 1, ae_v_len(offs,offs+m1-1));
            }
        }
    }
}


/*************************************************************************
Recurrent complex LU subroutine.
Never call it directly.

  -- ALGLIB routine --
     04.01.2010
     Bochkanov Sergey
*************************************************************************/
void cmatrixplurec(/* Complex */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     /* Complex */ ae_vector* tmp,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n1;
    ae_int_t n2;
    ae_int_t tsa;
    ae_int_t tsb;


    tsa = matrixtilesizea(_state)/2;
    tsb = matrixtilesizeb(_state);
    if( n<=tsa )
    {
        dlu_cmatrixplu2(a, offs, m, n, pivots, tmp, _state);
        return;
    }
    if( n>m )
    {
        cmatrixplurec(a, offs, m, m, pivots, tmp, _state);
        for(i=0; i<=m-1; i++)
        {
            ae_v_cmove(&tmp->ptr.p_complex[0], 1, &a->ptr.pp_complex[offs+i][offs+m], 1, "N", ae_v_len(0,n-m-1));
            ae_v_cmove(&a->ptr.pp_complex[offs+i][offs+m], 1, &a->ptr.pp_complex[pivots->ptr.p_int[offs+i]][offs+m], 1, "N", ae_v_len(offs+m,offs+n-1));
            ae_v_cmove(&a->ptr.pp_complex[pivots->ptr.p_int[offs+i]][offs+m], 1, &tmp->ptr.p_complex[0], 1, "N", ae_v_len(offs+m,offs+n-1));
        }
        cmatrixlefttrsm(m, n-m, a, offs, offs, ae_false, ae_true, 0, a, offs, offs+m, _state);
        return;
    }
    if( n>tsb )
    {
        n1 = tsb;
        n2 = n-n1;
    }
    else
    {
        tiledsplit(n, tsa, &n1, &n2, _state);
    }
    cmatrixplurec(a, offs, m, n1, pivots, tmp, _state);
    if( n2>0 )
    {
        for(i=0; i<=n1-1; i++)
        {
            if( offs+i!=pivots->ptr.p_int[offs+i] )
            {
                ae_v_cmove(&tmp->ptr.p_complex[0], 1, &a->ptr.pp_complex[offs+i][offs+n1], 1, "N", ae_v_len(0,n2-1));
                ae_v_cmove(&a->ptr.pp_complex[offs+i][offs+n1], 1, &a->ptr.pp_complex[pivots->ptr.p_int[offs+i]][offs+n1], 1, "N", ae_v_len(offs+n1,offs+n-1));
                ae_v_cmove(&a->ptr.pp_complex[pivots->ptr.p_int[offs+i]][offs+n1], 1, &tmp->ptr.p_complex[0], 1, "N", ae_v_len(offs+n1,offs+n-1));
            }
        }
        cmatrixlefttrsm(n1, n2, a, offs, offs, ae_false, ae_true, 0, a, offs, offs+n1, _state);
        cmatrixgemm(m-n1, n-n1, n1, ae_complex_from_d(-1.0), a, offs+n1, offs, 0, a, offs, offs+n1, 0, ae_complex_from_d(1.0), a, offs+n1, offs+n1, _state);
        cmatrixplurec(a, offs+n1, m-n1, n-n1, pivots, tmp, _state);
        for(i=0; i<=n2-1; i++)
        {
            if( offs+n1+i!=pivots->ptr.p_int[offs+n1+i] )
            {
                ae_v_cmove(&tmp->ptr.p_complex[0], 1, &a->ptr.pp_complex[offs+n1+i][offs], 1, "N", ae_v_len(0,n1-1));
                ae_v_cmove(&a->ptr.pp_complex[offs+n1+i][offs], 1, &a->ptr.pp_complex[pivots->ptr.p_int[offs+n1+i]][offs], 1, "N", ae_v_len(offs,offs+n1-1));
                ae_v_cmove(&a->ptr.pp_complex[pivots->ptr.p_int[offs+n1+i]][offs], 1, &tmp->ptr.p_complex[0], 1, "N", ae_v_len(offs,offs+n1-1));
            }
        }
    }
}


/*************************************************************************
Recurrent real LU subroutine.
Never call it directly.

  -- ALGLIB routine --
     04.01.2010
     Bochkanov Sergey
*************************************************************************/
void rmatrixplurec(/* Real    */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     /* Real    */ ae_vector* tmp,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n1;
    ae_int_t n2;
    ae_int_t tsa;
    ae_int_t tsb;


    tsa = matrixtilesizea(_state);
    tsb = matrixtilesizeb(_state);
    if( n<=tsb )
    {
        if( rmatrixplumkl(a, offs, m, n, pivots, _state) )
        {
            return;
        }
    }
    if( n<=tsa )
    {
        dlu_rmatrixplu2(a, offs, m, n, pivots, tmp, _state);
        return;
    }
    if( n>m )
    {
        rmatrixplurec(a, offs, m, m, pivots, tmp, _state);
        for(i=0; i<=m-1; i++)
        {
            ae_v_move(&tmp->ptr.p_double[0], 1, &a->ptr.pp_double[offs+i][offs+m], 1, ae_v_len(0,n-m-1));
            ae_v_move(&a->ptr.pp_double[offs+i][offs+m], 1, &a->ptr.pp_double[pivots->ptr.p_int[offs+i]][offs+m], 1, ae_v_len(offs+m,offs+n-1));
            ae_v_move(&a->ptr.pp_double[pivots->ptr.p_int[offs+i]][offs+m], 1, &tmp->ptr.p_double[0], 1, ae_v_len(offs+m,offs+n-1));
        }
        rmatrixlefttrsm(m, n-m, a, offs, offs, ae_false, ae_true, 0, a, offs, offs+m, _state);
        return;
    }
    if( n>tsb )
    {
        n1 = tsb;
        n2 = n-n1;
    }
    else
    {
        tiledsplit(n, tsa, &n1, &n2, _state);
    }
    rmatrixplurec(a, offs, m, n1, pivots, tmp, _state);
    if( n2>0 )
    {
        for(i=0; i<=n1-1; i++)
        {
            if( offs+i!=pivots->ptr.p_int[offs+i] )
            {
                ae_v_move(&tmp->ptr.p_double[0], 1, &a->ptr.pp_double[offs+i][offs+n1], 1, ae_v_len(0,n2-1));
                ae_v_move(&a->ptr.pp_double[offs+i][offs+n1], 1, &a->ptr.pp_double[pivots->ptr.p_int[offs+i]][offs+n1], 1, ae_v_len(offs+n1,offs+n-1));
                ae_v_move(&a->ptr.pp_double[pivots->ptr.p_int[offs+i]][offs+n1], 1, &tmp->ptr.p_double[0], 1, ae_v_len(offs+n1,offs+n-1));
            }
        }
        rmatrixlefttrsm(n1, n2, a, offs, offs, ae_false, ae_true, 0, a, offs, offs+n1, _state);
        rmatrixgemm(m-n1, n-n1, n1, -1.0, a, offs+n1, offs, 0, a, offs, offs+n1, 0, 1.0, a, offs+n1, offs+n1, _state);
        rmatrixplurec(a, offs+n1, m-n1, n-n1, pivots, tmp, _state);
        for(i=0; i<=n2-1; i++)
        {
            if( offs+n1+i!=pivots->ptr.p_int[offs+n1+i] )
            {
                ae_v_move(&tmp->ptr.p_double[0], 1, &a->ptr.pp_double[offs+n1+i][offs], 1, ae_v_len(0,n1-1));
                ae_v_move(&a->ptr.pp_double[offs+n1+i][offs], 1, &a->ptr.pp_double[pivots->ptr.p_int[offs+n1+i]][offs], 1, ae_v_len(offs,offs+n1-1));
                ae_v_move(&a->ptr.pp_double[pivots->ptr.p_int[offs+n1+i]][offs], 1, &tmp->ptr.p_double[0], 1, ae_v_len(offs,offs+n1-1));
            }
        }
    }
}


/*************************************************************************
Complex LUP kernel

  -- ALGLIB routine --
     10.01.2010
     Bochkanov Sergey
*************************************************************************/
static void dlu_cmatrixlup2(/* Complex */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     /* Complex */ ae_vector* tmp,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t jp;
    ae_complex s;


    if( m==0||n==0 )
    {
        return;
    }
    for(j=0; j<=ae_minint(m-1, n-1, _state); j++)
    {
        jp = j;
        for(i=j+1; i<=n-1; i++)
        {
            if( ae_fp_greater(ae_c_abs(a->ptr.pp_complex[offs+j][offs+i], _state),ae_c_abs(a->ptr.pp_complex[offs+j][offs+jp], _state)) )
            {
                jp = i;
            }
        }
        pivots->ptr.p_int[offs+j] = offs+jp;
        if( jp!=j )
        {
            ae_v_cmove(&tmp->ptr.p_complex[0], 1, &a->ptr.pp_complex[offs][offs+j], a->stride, "N", ae_v_len(0,m-1));
            ae_v_cmove(&a->ptr.pp_complex[offs][offs+j], a->stride, &a->ptr.pp_complex[offs][offs+jp], a->stride, "N", ae_v_len(offs,offs+m-1));
            ae_v_cmove(&a->ptr.pp_complex[offs][offs+jp], a->stride, &tmp->ptr.p_complex[0], 1, "N", ae_v_len(offs,offs+m-1));
        }
        if( ae_c_neq_d(a->ptr.pp_complex[offs+j][offs+j],(double)(0))&&j+1<=n-1 )
        {
            s = ae_c_d_div((double)(1),a->ptr.pp_complex[offs+j][offs+j]);
            ae_v_cmulc(&a->ptr.pp_complex[offs+j][offs+j+1], 1, ae_v_len(offs+j+1,offs+n-1), s);
        }
        if( j<ae_minint(m-1, n-1, _state) )
        {
            ae_v_cmove(&tmp->ptr.p_complex[0], 1, &a->ptr.pp_complex[offs+j+1][offs+j], a->stride, "N", ae_v_len(0,m-j-2));
            ae_v_cmoveneg(&tmp->ptr.p_complex[m], 1, &a->ptr.pp_complex[offs+j][offs+j+1], 1, "N", ae_v_len(m,m+n-j-2));
            cmatrixrank1(m-j-1, n-j-1, a, offs+j+1, offs+j+1, tmp, 0, tmp, m, _state);
        }
    }
}


/*************************************************************************
Real LUP kernel

  -- ALGLIB routine --
     10.01.2010
     Bochkanov Sergey
*************************************************************************/
static void dlu_rmatrixlup2(/* Real    */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     /* Real    */ ae_vector* tmp,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t jp;
    double s;


    if( m==0||n==0 )
    {
        return;
    }
    for(j=0; j<=ae_minint(m-1, n-1, _state); j++)
    {
        jp = j;
        for(i=j+1; i<=n-1; i++)
        {
            if( ae_fp_greater(ae_fabs(a->ptr.pp_double[offs+j][offs+i], _state),ae_fabs(a->ptr.pp_double[offs+j][offs+jp], _state)) )
            {
                jp = i;
            }
        }
        pivots->ptr.p_int[offs+j] = offs+jp;
        if( jp!=j )
        {
            ae_v_move(&tmp->ptr.p_double[0], 1, &a->ptr.pp_double[offs][offs+j], a->stride, ae_v_len(0,m-1));
            ae_v_move(&a->ptr.pp_double[offs][offs+j], a->stride, &a->ptr.pp_double[offs][offs+jp], a->stride, ae_v_len(offs,offs+m-1));
            ae_v_move(&a->ptr.pp_double[offs][offs+jp], a->stride, &tmp->ptr.p_double[0], 1, ae_v_len(offs,offs+m-1));
        }
        if( ae_fp_neq(a->ptr.pp_double[offs+j][offs+j],(double)(0))&&j+1<=n-1 )
        {
            s = (double)1/a->ptr.pp_double[offs+j][offs+j];
            ae_v_muld(&a->ptr.pp_double[offs+j][offs+j+1], 1, ae_v_len(offs+j+1,offs+n-1), s);
        }
        if( j<ae_minint(m-1, n-1, _state) )
        {
            ae_v_move(&tmp->ptr.p_double[0], 1, &a->ptr.pp_double[offs+j+1][offs+j], a->stride, ae_v_len(0,m-j-2));
            ae_v_moveneg(&tmp->ptr.p_double[m], 1, &a->ptr.pp_double[offs+j][offs+j+1], 1, ae_v_len(m,m+n-j-2));
            rmatrixrank1(m-j-1, n-j-1, a, offs+j+1, offs+j+1, tmp, 0, tmp, m, _state);
        }
    }
}


/*************************************************************************
Complex PLU kernel

  -- LAPACK routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     June 30, 1992
*************************************************************************/
static void dlu_cmatrixplu2(/* Complex */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     /* Complex */ ae_vector* tmp,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t jp;
    ae_complex s;


    if( m==0||n==0 )
    {
        return;
    }
    for(j=0; j<=ae_minint(m-1, n-1, _state); j++)
    {
        jp = j;
        for(i=j+1; i<=m-1; i++)
        {
            if( ae_fp_greater(ae_c_abs(a->ptr.pp_complex[offs+i][offs+j], _state),ae_c_abs(a->ptr.pp_complex[offs+jp][offs+j], _state)) )
            {
                jp = i;
            }
        }
        pivots->ptr.p_int[offs+j] = offs+jp;
        if( ae_c_neq_d(a->ptr.pp_complex[offs+jp][offs+j],(double)(0)) )
        {
            if( jp!=j )
            {
                for(i=0; i<=n-1; i++)
                {
                    s = a->ptr.pp_complex[offs+j][offs+i];
                    a->ptr.pp_complex[offs+j][offs+i] = a->ptr.pp_complex[offs+jp][offs+i];
                    a->ptr.pp_complex[offs+jp][offs+i] = s;
                }
            }
            if( j+1<=m-1 )
            {
                s = ae_c_d_div((double)(1),a->ptr.pp_complex[offs+j][offs+j]);
                ae_v_cmulc(&a->ptr.pp_complex[offs+j+1][offs+j], a->stride, ae_v_len(offs+j+1,offs+m-1), s);
            }
        }
        if( j<ae_minint(m, n, _state)-1 )
        {
            ae_v_cmove(&tmp->ptr.p_complex[0], 1, &a->ptr.pp_complex[offs+j+1][offs+j], a->stride, "N", ae_v_len(0,m-j-2));
            ae_v_cmoveneg(&tmp->ptr.p_complex[m], 1, &a->ptr.pp_complex[offs+j][offs+j+1], 1, "N", ae_v_len(m,m+n-j-2));
            cmatrixrank1(m-j-1, n-j-1, a, offs+j+1, offs+j+1, tmp, 0, tmp, m, _state);
        }
    }
}


/*************************************************************************
Real PLU kernel

  -- LAPACK routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     June 30, 1992
*************************************************************************/
static void dlu_rmatrixplu2(/* Real    */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     /* Real    */ ae_vector* tmp,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t jp;
    double s;


    if( m==0||n==0 )
    {
        return;
    }
    for(j=0; j<=ae_minint(m-1, n-1, _state); j++)
    {
        jp = j;
        for(i=j+1; i<=m-1; i++)
        {
            if( ae_fp_greater(ae_fabs(a->ptr.pp_double[offs+i][offs+j], _state),ae_fabs(a->ptr.pp_double[offs+jp][offs+j], _state)) )
            {
                jp = i;
            }
        }
        pivots->ptr.p_int[offs+j] = offs+jp;
        if( ae_fp_neq(a->ptr.pp_double[offs+jp][offs+j],(double)(0)) )
        {
            if( jp!=j )
            {
                for(i=0; i<=n-1; i++)
                {
                    s = a->ptr.pp_double[offs+j][offs+i];
                    a->ptr.pp_double[offs+j][offs+i] = a->ptr.pp_double[offs+jp][offs+i];
                    a->ptr.pp_double[offs+jp][offs+i] = s;
                }
            }
            if( j+1<=m-1 )
            {
                s = (double)1/a->ptr.pp_double[offs+j][offs+j];
                ae_v_muld(&a->ptr.pp_double[offs+j+1][offs+j], a->stride, ae_v_len(offs+j+1,offs+m-1), s);
            }
        }
        if( j<ae_minint(m, n, _state)-1 )
        {
            ae_v_move(&tmp->ptr.p_double[0], 1, &a->ptr.pp_double[offs+j+1][offs+j], a->stride, ae_v_len(0,m-j-2));
            ae_v_moveneg(&tmp->ptr.p_double[m], 1, &a->ptr.pp_double[offs+j][offs+j+1], 1, ae_v_len(m,m+n-j-2));
            rmatrixrank1(m-j-1, n-j-1, a, offs+j+1, offs+j+1, tmp, 0, tmp, m, _state);
        }
    }
}


/*$ End $*/
