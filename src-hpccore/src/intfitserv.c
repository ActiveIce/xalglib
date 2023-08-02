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
#include "intfitserv.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Internal subroutine: automatic scaling for LLS tasks.
NEVER CALL IT DIRECTLY!

Maps abscissas to [-1,1], standartizes ordinates and correspondingly scales
constraints. It also scales weights so that max(W[i])=1

Transformations performed:
* X, XC         [XA,XB] => [-1,+1]
                transformation makes min(X)=-1, max(X)=+1

* Y             [SA,SB] => [0,1]
                transformation makes mean(Y)=0, stddev(Y)=1
                
* YC            transformed accordingly to SA, SB, DC[I]

  -- ALGLIB PROJECT --
     Copyright 08.09.2009 by Bochkanov Sergey
*************************************************************************/
void lsfitscalexy(/* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     /* Real    */ ae_vector* w,
     ae_int_t n,
     /* Real    */ ae_vector* xc,
     /* Real    */ ae_vector* yc,
     /* Integer */ const ae_vector* dc,
     ae_int_t k,
     double* xa,
     double* xb,
     double* sa,
     double* sb,
     /* Real    */ ae_vector* xoriginal,
     /* Real    */ ae_vector* yoriginal,
     ae_state *_state)
{
    double xmin;
    double xmax;
    ae_int_t i;
    double mx;

    *xa = 0.0;
    *xb = 0.0;
    *sa = 0.0;
    *sb = 0.0;
    ae_vector_clear(xoriginal);
    ae_vector_clear(yoriginal);

    ae_assert(n>=1, "LSFitScaleXY: incorrect N", _state);
    ae_assert(k>=0, "LSFitScaleXY: incorrect K", _state);
    xmin = x->ptr.p_double[0];
    xmax = x->ptr.p_double[0];
    for(i=1; i<=n-1; i++)
    {
        xmin = ae_minreal(xmin, x->ptr.p_double[i], _state);
        xmax = ae_maxreal(xmax, x->ptr.p_double[i], _state);
    }
    for(i=0; i<=k-1; i++)
    {
        xmin = ae_minreal(xmin, xc->ptr.p_double[i], _state);
        xmax = ae_maxreal(xmax, xc->ptr.p_double[i], _state);
    }
    if( ae_fp_eq(xmin,xmax) )
    {
        if( ae_fp_eq(xmin,(double)(0)) )
        {
            xmin = (double)(-1);
            xmax = (double)(1);
        }
        else
        {
            if( ae_fp_greater(xmin,(double)(0)) )
            {
                xmin = 0.5*xmin;
            }
            else
            {
                xmax = 0.5*xmax;
            }
        }
    }
    ae_vector_set_length(xoriginal, n, _state);
    ae_v_move(&xoriginal->ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,n-1));
    *xa = xmin;
    *xb = xmax;
    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = (double)2*(x->ptr.p_double[i]-0.5*(*xa+(*xb)))/(*xb-(*xa));
    }
    for(i=0; i<=k-1; i++)
    {
        ae_assert(dc->ptr.p_int[i]>=0, "LSFitScaleXY: internal error!", _state);
        xc->ptr.p_double[i] = (double)2*(xc->ptr.p_double[i]-0.5*(*xa+(*xb)))/(*xb-(*xa));
        yc->ptr.p_double[i] = yc->ptr.p_double[i]*ae_pow(0.5*(*xb-(*xa)), (double)(dc->ptr.p_int[i]), _state);
    }
    ae_vector_set_length(yoriginal, n, _state);
    ae_v_move(&yoriginal->ptr.p_double[0], 1, &y->ptr.p_double[0], 1, ae_v_len(0,n-1));
    *sa = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        *sa = *sa+y->ptr.p_double[i];
    }
    *sa = *sa/(double)n;
    *sb = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        *sb = *sb+ae_sqr(y->ptr.p_double[i]-(*sa), _state);
    }
    *sb = ae_sqrt(*sb/(double)n, _state)+(*sa);
    if( ae_fp_eq(*sb,*sa) )
    {
        *sb = (double)2*(*sa);
    }
    if( ae_fp_eq(*sb,*sa) )
    {
        *sb = *sa+(double)1;
    }
    for(i=0; i<=n-1; i++)
    {
        y->ptr.p_double[i] = (y->ptr.p_double[i]-(*sa))/(*sb-(*sa));
    }
    for(i=0; i<=k-1; i++)
    {
        if( dc->ptr.p_int[i]==0 )
        {
            yc->ptr.p_double[i] = (yc->ptr.p_double[i]-(*sa))/(*sb-(*sa));
        }
        else
        {
            yc->ptr.p_double[i] = yc->ptr.p_double[i]/(*sb-(*sa));
        }
    }
    mx = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        mx = ae_maxreal(mx, ae_fabs(w->ptr.p_double[i], _state), _state);
    }
    if( ae_fp_neq(mx,(double)(0)) )
    {
        for(i=0; i<=n-1; i++)
        {
            w->ptr.p_double[i] = w->ptr.p_double[i]/mx;
        }
    }
}


void buildpriorterm(/* Real    */ ae_matrix* xy,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t modeltype,
     double priorval,
     /* Real    */ ae_matrix* v,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j0;
    ae_int_t j1;
    double rj;
    ae_matrix araw;
    ae_matrix amod;
    ae_matrix braw;
    ae_vector tmp0;
    double lambdareg;
    ae_int_t rfsits;

    ae_frame_make(_state, &_frame_block);
    memset(&araw, 0, sizeof(araw));
    memset(&amod, 0, sizeof(amod));
    memset(&braw, 0, sizeof(braw));
    memset(&tmp0, 0, sizeof(tmp0));
    ae_matrix_clear(v);
    ae_matrix_init(&araw, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&amod, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&braw, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>=0, "BuildPriorTerm: N<0", _state);
    ae_assert(nx>0, "BuildPriorTerm: NX<=0", _state);
    ae_assert(ny>0, "BuildPriorTerm: NY<=0", _state);
    ae_matrix_set_length(v, ny, nx+1, _state);
    for(i=0; i<=v->rows-1; i++)
    {
        for(j=0; j<=v->cols-1; j++)
        {
            v->ptr.pp_double[i][j] = (double)(0);
        }
    }
    if( n==0 )
    {
        if( modeltype==0 )
        {
            for(i=0; i<=ny-1; i++)
            {
                v->ptr.pp_double[i][nx] = priorval;
            }
            ae_frame_leave(_state);
            return;
        }
        if( modeltype==1 )
        {
            ae_frame_leave(_state);
            return;
        }
        if( modeltype==2 )
        {
            ae_frame_leave(_state);
            return;
        }
        if( modeltype==3 )
        {
            ae_frame_leave(_state);
            return;
        }
        ae_assert(ae_false, "BuildPriorTerm: unexpected model type", _state);
    }
    if( modeltype==0 )
    {
        for(i=0; i<=ny-1; i++)
        {
            v->ptr.pp_double[i][nx] = priorval;
        }
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=ny-1; j++)
            {
                xy->ptr.pp_double[i][nx+j] = xy->ptr.pp_double[i][nx+j]-priorval;
            }
        }
        ae_frame_leave(_state);
        return;
    }
    if( modeltype==2 )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=ny-1; j++)
            {
                v->ptr.pp_double[j][nx] = v->ptr.pp_double[j][nx]+xy->ptr.pp_double[i][nx+j];
            }
        }
        for(j=0; j<=ny-1; j++)
        {
            v->ptr.pp_double[j][nx] = v->ptr.pp_double[j][nx]/coalesce((double)(n), (double)(1), _state);
        }
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=ny-1; j++)
            {
                xy->ptr.pp_double[i][nx+j] = xy->ptr.pp_double[i][nx+j]-v->ptr.pp_double[j][nx];
            }
        }
        ae_frame_leave(_state);
        return;
    }
    if( modeltype==3 )
    {
        ae_frame_leave(_state);
        return;
    }
    ae_assert(modeltype==1, "BuildPriorTerm: unexpected model type", _state);
    lambdareg = 0.0;
    ae_matrix_set_length(&araw, nx+1, nx+1, _state);
    ae_matrix_set_length(&braw, nx+1, ny, _state);
    ae_vector_set_length(&tmp0, nx+1, _state);
    ae_matrix_set_length(&amod, nx+1, nx+1, _state);
    for(i=0; i<=nx; i++)
    {
        for(j=0; j<=nx; j++)
        {
            araw.ptr.pp_double[i][j] = (double)(0);
        }
    }
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            tmp0.ptr.p_double[j] = xy->ptr.pp_double[i][j];
        }
        tmp0.ptr.p_double[nx] = 1.0;
        for(j0=0; j0<=nx; j0++)
        {
            for(j1=0; j1<=nx; j1++)
            {
                araw.ptr.pp_double[j0][j1] = araw.ptr.pp_double[j0][j1]+tmp0.ptr.p_double[j0]*tmp0.ptr.p_double[j1];
            }
        }
    }
    for(rfsits=1; rfsits<=3; rfsits++)
    {
        for(i=0; i<=nx; i++)
        {
            for(j=0; j<=ny-1; j++)
            {
                braw.ptr.pp_double[i][j] = (double)(0);
            }
        }
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=nx-1; j++)
            {
                tmp0.ptr.p_double[j] = xy->ptr.pp_double[i][j];
            }
            tmp0.ptr.p_double[nx] = 1.0;
            for(j=0; j<=ny-1; j++)
            {
                rj = xy->ptr.pp_double[i][nx+j];
                for(j0=0; j0<=nx; j0++)
                {
                    rj = rj-tmp0.ptr.p_double[j0]*v->ptr.pp_double[j][j0];
                }
                for(j0=0; j0<=nx; j0++)
                {
                    braw.ptr.pp_double[j0][j] = braw.ptr.pp_double[j0][j]+rj*tmp0.ptr.p_double[j0];
                }
            }
        }
        for(;;)
        {
            for(i=0; i<=nx; i++)
            {
                for(j=0; j<=nx; j++)
                {
                    amod.ptr.pp_double[i][j] = araw.ptr.pp_double[i][j];
                }
                amod.ptr.pp_double[i][i] = amod.ptr.pp_double[i][i]+lambdareg*coalesce(amod.ptr.pp_double[i][i], (double)(1), _state);
            }
            if( spdmatrixcholesky(&amod, nx+1, ae_true, _state) )
            {
                break;
            }
            lambdareg = coalesce((double)10*lambdareg, 1.0E-12, _state);
        }
        rmatrixlefttrsm(nx+1, ny, &amod, 0, 0, ae_true, ae_false, 1, &braw, 0, 0, _state);
        rmatrixlefttrsm(nx+1, ny, &amod, 0, 0, ae_true, ae_false, 0, &braw, 0, 0, _state);
        for(i=0; i<=nx; i++)
        {
            for(j=0; j<=ny-1; j++)
            {
                v->ptr.pp_double[j][i] = v->ptr.pp_double[j][i]+braw.ptr.pp_double[i][j];
            }
        }
    }
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            tmp0.ptr.p_double[j] = xy->ptr.pp_double[i][j];
        }
        tmp0.ptr.p_double[nx] = 1.0;
        for(j=0; j<=ny-1; j++)
        {
            rj = 0.0;
            for(j0=0; j0<=nx; j0++)
            {
                rj = rj+tmp0.ptr.p_double[j0]*v->ptr.pp_double[j][j0];
            }
            xy->ptr.pp_double[i][nx+j] = xy->ptr.pp_double[i][nx+j]-rj;
        }
    }
    ae_frame_leave(_state);
}


void buildpriorterm1(/* Real    */ ae_vector* xy1,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t modeltype,
     double priorval,
     /* Real    */ ae_matrix* v,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t ew;
    double rj;
    ae_matrix araw;
    ae_matrix amod;
    ae_matrix braw;
    ae_vector tmp0;
    double lambdareg;
    ae_int_t rfsits;

    ae_frame_make(_state, &_frame_block);
    memset(&araw, 0, sizeof(araw));
    memset(&amod, 0, sizeof(amod));
    memset(&braw, 0, sizeof(braw));
    memset(&tmp0, 0, sizeof(tmp0));
    ae_matrix_clear(v);
    ae_matrix_init(&araw, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&amod, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&braw, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>=0, "BuildPriorTerm: N<0", _state);
    ae_assert(nx>0, "BuildPriorTerm: NX<=0", _state);
    ae_assert(ny>0, "BuildPriorTerm: NY<=0", _state);
    ew = nx+ny;
    ae_matrix_set_length(v, ny, nx+1, _state);
    for(i=0; i<=v->rows-1; i++)
    {
        for(j=0; j<=v->cols-1; j++)
        {
            v->ptr.pp_double[i][j] = (double)(0);
        }
    }
    if( n==0 )
    {
        if( modeltype==0 )
        {
            for(i=0; i<=ny-1; i++)
            {
                v->ptr.pp_double[i][nx] = priorval;
            }
            ae_frame_leave(_state);
            return;
        }
        if( modeltype==1 )
        {
            ae_frame_leave(_state);
            return;
        }
        if( modeltype==2 )
        {
            ae_frame_leave(_state);
            return;
        }
        if( modeltype==3 )
        {
            ae_frame_leave(_state);
            return;
        }
        ae_assert(ae_false, "BuildPriorTerm: unexpected model type", _state);
    }
    if( modeltype==0 )
    {
        for(i=0; i<=ny-1; i++)
        {
            v->ptr.pp_double[i][nx] = priorval;
        }
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=ny-1; j++)
            {
                xy1->ptr.p_double[i*ew+nx+j] = xy1->ptr.p_double[i*ew+nx+j]-priorval;
            }
        }
        ae_frame_leave(_state);
        return;
    }
    if( modeltype==2 )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=ny-1; j++)
            {
                v->ptr.pp_double[j][nx] = v->ptr.pp_double[j][nx]+xy1->ptr.p_double[i*ew+nx+j];
            }
        }
        for(j=0; j<=ny-1; j++)
        {
            v->ptr.pp_double[j][nx] = v->ptr.pp_double[j][nx]/coalesce((double)(n), (double)(1), _state);
        }
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=ny-1; j++)
            {
                xy1->ptr.p_double[i*ew+nx+j] = xy1->ptr.p_double[i*ew+nx+j]-v->ptr.pp_double[j][nx];
            }
        }
        ae_frame_leave(_state);
        return;
    }
    if( modeltype==3 )
    {
        ae_frame_leave(_state);
        return;
    }
    ae_assert(modeltype==1, "BuildPriorTerm: unexpected model type", _state);
    lambdareg = 0.0;
    ae_matrix_set_length(&araw, nx+1, nx+1, _state);
    ae_matrix_set_length(&braw, nx+1, ny, _state);
    ae_vector_set_length(&tmp0, nx+1, _state);
    ae_matrix_set_length(&amod, nx+1, nx+1, _state);
    for(i=0; i<=nx; i++)
    {
        for(j=0; j<=nx; j++)
        {
            araw.ptr.pp_double[i][j] = (double)(0);
        }
    }
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            tmp0.ptr.p_double[j] = xy1->ptr.p_double[i*ew+j];
        }
        tmp0.ptr.p_double[nx] = 1.0;
        for(j0=0; j0<=nx; j0++)
        {
            for(j1=0; j1<=nx; j1++)
            {
                araw.ptr.pp_double[j0][j1] = araw.ptr.pp_double[j0][j1]+tmp0.ptr.p_double[j0]*tmp0.ptr.p_double[j1];
            }
        }
    }
    for(rfsits=1; rfsits<=1; rfsits++)
    {
        for(i=0; i<=nx; i++)
        {
            for(j=0; j<=ny-1; j++)
            {
                braw.ptr.pp_double[i][j] = (double)(0);
            }
        }
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=nx-1; j++)
            {
                tmp0.ptr.p_double[j] = xy1->ptr.p_double[i*ew+j];
            }
            tmp0.ptr.p_double[nx] = 1.0;
            for(j=0; j<=ny-1; j++)
            {
                rj = xy1->ptr.p_double[i*ew+nx+j];
                for(j0=0; j0<=nx; j0++)
                {
                    rj = rj-tmp0.ptr.p_double[j0]*v->ptr.pp_double[j][j0];
                }
                for(j0=0; j0<=nx; j0++)
                {
                    braw.ptr.pp_double[j0][j] = braw.ptr.pp_double[j0][j]+rj*tmp0.ptr.p_double[j0];
                }
            }
        }
        for(;;)
        {
            for(i=0; i<=nx; i++)
            {
                for(j=0; j<=nx; j++)
                {
                    amod.ptr.pp_double[i][j] = araw.ptr.pp_double[i][j];
                }
                amod.ptr.pp_double[i][i] = amod.ptr.pp_double[i][i]+lambdareg*coalesce(amod.ptr.pp_double[i][i], (double)(1), _state);
            }
            if( spdmatrixcholesky(&amod, nx+1, ae_true, _state) )
            {
                break;
            }
            lambdareg = coalesce((double)10*lambdareg, 1.0E-12, _state);
        }
        rmatrixlefttrsm(nx+1, ny, &amod, 0, 0, ae_true, ae_false, 1, &braw, 0, 0, _state);
        rmatrixlefttrsm(nx+1, ny, &amod, 0, 0, ae_true, ae_false, 0, &braw, 0, 0, _state);
        for(i=0; i<=nx; i++)
        {
            for(j=0; j<=ny-1; j++)
            {
                v->ptr.pp_double[j][i] = v->ptr.pp_double[j][i]+braw.ptr.pp_double[i][j];
            }
        }
    }
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            tmp0.ptr.p_double[j] = xy1->ptr.p_double[i*ew+j];
        }
        tmp0.ptr.p_double[nx] = 1.0;
        for(j=0; j<=ny-1; j++)
        {
            rj = 0.0;
            for(j0=0; j0<=nx; j0++)
            {
                rj = rj+tmp0.ptr.p_double[j0]*v->ptr.pp_double[j][j0];
            }
            xy1->ptr.p_double[i*ew+nx+j] = xy1->ptr.p_double[i*ew+nx+j]-rj;
        }
    }
    ae_frame_leave(_state);
}


/*$ End $*/
