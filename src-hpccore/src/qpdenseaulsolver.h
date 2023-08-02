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

#ifndef _qpdenseaulsolver_h
#define _qpdenseaulsolver_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "scodes.h"
#include "ablasmkl.h"
#include "ablasf.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "tsort.h"
#include "sparse.h"
#include "hblas.h"
#include "creflections.h"
#include "sblas.h"
#include "ablas.h"
#include "ortfac.h"
#include "blas.h"
#include "rotations.h"
#include "bdsvd.h"
#include "svd.h"
#include "dlu.h"
#include "sptrf.h"
#include "apstruct.h"
#include "amdordering.h"
#include "spchol.h"
#include "matgen.h"
#include "trfac.h"
#include "hsschur.h"
#include "basicstatops.h"
#include "evd.h"
#include "trlinsolve.h"
#include "safesolve.h"
#include "rcond.h"
#include "xblas.h"
#include "directdensesolvers.h"
#include "normestimator.h"
#include "linlsqr.h"
#include "linmin.h"
#include "optguardapi.h"
#include "matinv.h"
#include "optserv.h"
#include "fbls.h"
#include "minlbfgs.h"
#include "cqmodels.h"
#include "lpqpserv.h"
#include "snnls.h"
#include "sactivesets.h"
#include "qqpsolver.h"


/*$ Declarations $*/


/*************************************************************************
This object stores settings for DENSE-AUL solver.
It must be initialized with QPDENSEAULLoadDefaults().
After initialization you may change settings.
*************************************************************************/
typedef struct
{
    double epsx;
    ae_int_t outerits;
    double rho;
} qpdenseaulsettings;


/*************************************************************************
This object stores temporaries used by Dense-AUL solver.
*************************************************************************/
typedef struct
{
    ae_vector nulc;
    ae_matrix sclsfta;
    ae_vector sclsftb;
    ae_vector sclsfthasbndl;
    ae_vector sclsfthasbndu;
    ae_vector sclsftbndl;
    ae_vector sclsftbndu;
    ae_vector sclsftxc;
    ae_matrix sclsftcleic;
    ae_vector cidx;
    ae_vector cscales;
    ae_matrix exa;
    ae_vector exb;
    ae_vector exxc;
    ae_vector exbndl;
    ae_vector exbndu;
    ae_vector exscale;
    ae_vector exxorigin;
    qqpsettings qqpsettingsuser;
    qqpbuffers qqpbuf;
    ae_vector nulcest;
    ae_vector tmpg;
    ae_vector tmp0;
    ae_matrix tmp2;
    ae_vector modelg;
    ae_vector d;
    ae_vector deltax;
    convexquadraticmodel dummycqm;
    sparsematrix dummysparse;
    ae_matrix qrkkt;
    ae_vector qrrightpart;
    ae_vector qrtau;
    ae_vector qrsv0;
    ae_vector qrsvx1;
    ae_vector nicerr;
    ae_vector nicnact;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repncholesky;
    ae_int_t repnwrkchanges;
    ae_int_t repnwrk0;
    ae_int_t repnwrk1;
    ae_int_t repnwrkf;
    ae_int_t repnmv;
} qpdenseaulbuffers;


/*$ Body $*/


/*************************************************************************
This function initializes QPDENSEAULSettings structure with default settings.

Newly created structure MUST be initialized by default settings  -  or  by
copy of the already initialized structure.

  -- ALGLIB --
     Copyright 14.05.2011 by Bochkanov Sergey
*************************************************************************/
void qpdenseaulloaddefaults(ae_int_t nmain,
     qpdenseaulsettings* s,
     ae_state *_state);


/*************************************************************************
This function runs Dense-AUL solver; it returns after optimization process
was completed. Following QP problem is solved:

    min(0.5*(x-x_origin)'*A*(x-x_origin)+b'*(x-x_origin))
    
subject to combination of box and general linear dense/sparse constraints.

INPUT PARAMETERS:
    DenseA      -   for dense problems (AKind=0), A-term of CQM object
                    contains system matrix. Other terms are unspecified
                    and should not be referenced.
    SparseA     -   for sparse problems (AKind=1), CRS format
    AKind       -   sparse matrix format:
                    * 0 for dense matrix
                    * 1 for sparse matrix
    SparseUpper -   which triangle of SparseAC stores matrix  -  upper  or
                    lower one (for dense matrices this  parameter  is  not
                    actual).
    B           -   linear term, array[N]
    BndL        -   lower bound, array[N]
    BndU        -   upper bound, array[N]
    S           -   scale vector, array[NC]:
                    * I-th element contains scale of I-th variable,
                    * SC[I]>0
    XOrigin     -   origin term, array[NC]. Can be zero.
    N           -   number of variables in the  original  formulation  (no
                    slack variables).
    CLEIC       -   dense linear equality/inequality constraints. Equality
                    constraints come first.
    NEC, NIC    -   number of dense equality/inequality constraints.
    SCLEIC      -   sparse linear equality/inequality constraints. Equality
                    constraints come first.
    SNEC, SNIC  -   number of sparse equality/inequality constraints.
    RenormLC    -   whether constraints should be renormalized (recommended)
                    or used "as is".
    Settings    -   QPDENSEAULSettings object initialized by one of the initialization
                    functions.
    State       -   object which stores temporaries
    XS          -   initial point, array[NC]
    
    
OUTPUT PARAMETERS:
    XS          -   last point
    TerminationType-termination type:
                    *
                    *
                    *

  -- ALGLIB --
     Copyright 2017 by Bochkanov Sergey
*************************************************************************/
void qpdenseauloptimize(const convexquadraticmodel* a,
     const sparsematrix* sparsea,
     ae_int_t akind,
     ae_bool sparseaupper,
     /* Real    */ const ae_vector* b,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t nn,
     /* Real    */ const ae_matrix* cleic,
     ae_int_t dnec,
     ae_int_t dnic,
     const sparsematrix* scleic,
     ae_int_t snec,
     ae_int_t snic,
     ae_bool renormlc,
     const qpdenseaulsettings* settings,
     qpdenseaulbuffers* state,
     /* Real    */ ae_vector* xs,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     ae_int_t* terminationtype,
     ae_state *_state);
void _qpdenseaulsettings_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _qpdenseaulsettings_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _qpdenseaulsettings_clear(void* _p);
void _qpdenseaulsettings_destroy(void* _p);
void _qpdenseaulbuffers_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _qpdenseaulbuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _qpdenseaulbuffers_clear(void* _p);
void _qpdenseaulbuffers_destroy(void* _p);


/*$ End $*/
#endif

