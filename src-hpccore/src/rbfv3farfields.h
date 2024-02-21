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

#ifndef _rbfv3farfields_h
#define _rbfv3farfields_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "scodes.h"
#include "apserv.h"
#include "ablasf.h"
#include "tsort.h"


/*$ Declarations $*/


/*************************************************************************
Biharmonic evaluator and its global temporaries
*************************************************************************/
typedef struct
{
    ae_int_t maxp;
    ae_int_t precomputedcount;
    ae_vector tdoublefactorial;
    ae_vector tfactorial;
    ae_vector tsqrtfactorial;
    ae_vector tpowminus1;
    ae_vector tpowi;
    ae_vector tpowminusi;
    ae_vector ynma;
    ae_vector pnma;
    ae_vector pnmb;
    ae_vector pmmc;
    ae_vector pmmcdiag;
    ae_vector mnma;
    ae_vector nnma;
    ae_vector inma;
} biharmonicevaluator;


/*************************************************************************
Panel (a set of closely located points) for a biharmonic evaluator
containing aggregated far field expansion functions N_ij and M_ij.

* C0, C1, C2 contain evaluation center
* RMax contains radius of the points cloud
* UseAtDistance contains minimum distance which allows to compute model
  value with required precision
* NY contains 'output' dimensions count
* P contains functions count
* MaxSumAbs contains maximum of column-wise sums of coefficient magnitudes
* tblN                  contains NY*PxP table of expansion functions N_nm,
                        with tblN[K*P*P+I*P+J] being N_nm(n=I,m=J) for K-th
                        output of the vector-valued model
* tblM                  contains PxP table of expansion functions M_nm,
                        with tblM[K*P*P+I*P+J] being M_nm(n=I,m=J) for K-th
                        output of the vector-valued model
* tblModN, tblModM      contain tables N and M with additional precomputed
                        multiplier merged in (which accelerates panel
                        evaluation)
* tblRModMN             contains tblModM and tblModN in a packed format:
                        * first P elements are  Real(tblModM[0..P-1])
                        *  next P elements are Image(tblModM[0..P-1])
                        *  next P elements are  Real(tblModN[0..P-1])
                        *  next P elements are Image(tblModN[0..P-1])
                        * next 4*P elements store subsequent P elements of
                          tblModM and tblModN and so on
*************************************************************************/
typedef struct
{
    double c0;
    double c1;
    double c2;
    double rmax;
    double useatdistance;
    ae_int_t ny;
    ae_int_t p;
    ae_int_t sizen;
    ae_int_t sizem;
    ae_int_t stride;
    ae_int_t sizeinner;
    ae_vector tbln;
    ae_vector tblm;
    ae_vector tblmodn;
    ae_vector tblmodm;
    ae_vector tblpowrmax;
    ae_vector tblrmodmn;
    double maxsumabs;
    ae_vector funcsphericaly;
    ae_vector tpowr;
} biharmonicpanel;


/*$ Body $*/


/*************************************************************************
Initialize precomputed table for a biharmonic evaluator

  -- ALGLIB --
     Copyright 26.08.2022 by Sergey Bochkanov
*************************************************************************/
void biharmonicevaluatorinit(biharmonicevaluator* eval,
     ae_int_t maxp,
     ae_state *_state);


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
     ae_state *_state);


/*************************************************************************
This function sets far field distance depending on desired accuracy.

INPUT PARAMETERS:
    Panel           -   panel with valid far field expansion
    Tol             -   desired tolerance

  -- ALGLIB --
     Copyright 20.11.2022 by Sergey Bochkanov
*************************************************************************/
void bhpanelsetprec(biharmonicpanel* panel, double tol, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);
void _biharmonicevaluator_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _biharmonicevaluator_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _biharmonicevaluator_clear(void* _p);
void _biharmonicevaluator_destroy(void* _p);
void _biharmonicpanel_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _biharmonicpanel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _biharmonicpanel_clear(void* _p);
void _biharmonicpanel_destroy(void* _p);


/*$ End $*/
#endif

