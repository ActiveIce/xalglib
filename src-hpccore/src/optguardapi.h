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

#ifndef _optguardapi_h
#define _optguardapi_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"


/*$ Declarations $*/


/*************************************************************************
This structure is used to store  OptGuard  report,  i.e.  report  on   the
properties of the nonlinear function being optimized with ALGLIB.

After you tell your optimizer to activate OptGuard  this technology starts
to silently monitor function values and gradients/Jacobians  being  passed
all around during your optimization session. Depending on specific set  of
checks enabled OptGuard may perform additional function evaluations  (say,
about 3*N evaluations if you want to check analytic gradient for errors).

Upon discovering that something strange happens  (function  values  and/or
gradient components change too sharply and/or unexpectedly) OptGuard  sets
one of the "suspicion  flags" (without interrupting optimization session).
After optimization is done, you can examine OptGuard report.

Following report fields can be set:
* nonc0suspected
* nonc1suspected
* badgradsuspected


=== WHAT CAN BE DETECTED WITH OptGuard INTEGRITY CHECKER =================

Following  types  of  errors  in your target function (constraints) can be
caught:
a) discontinuous functions ("non-C0" part of the report)
b) functions with discontinuous derivative ("non-C1" part of the report)
c) errors in the analytic gradient provided by user

These types of errors result in optimizer  stopping  well  before reaching
solution (most often - right after encountering discontinuity).

Type A errors are usually  coding  errors  during  implementation  of  the
target function. Most "normal" problems involve continuous functions,  and
anyway you can't reliably optimize discontinuous function.

Type B errors are either coding errors or (in case code itself is correct)
evidence of the fact  that  your  problem  is  an  "incorrect"  one.  Most
optimizers (except for ones provided by MINNS subpackage) do  not  support
nonsmooth problems.

Type C errors are coding errors which often prevent optimizer from  making
even one step  or result in optimizing stopping  too  early,  as  soon  as
actual descent direction becomes too different from one suggested by user-
supplied gradient.


=== WHAT IS REPORTED =====================================================

Following set of report fields deals with discontinuous  target functions,
ones not belonging to C0 continuity class:

* nonc0suspected - is a flag which is set upon discovering some indication
  of the discontinuity. If this flag is false, the rest of "non-C0" fields
  should be ignored
* nonc0fidx - is an index of the function (0 for  target  function,  1  or
  higher for nonlinear constraints) which is suspected of being "non-C0"
* nonc0lipshitzc - a Lipchitz constant for a function which was  suspected
  of being non-continuous.
* nonc0test0positive -  set  to  indicate  specific  test  which  detected
  continuity violation (test #0)

Following set of report fields deals with discontinuous gradient/Jacobian,
i.e. with functions violating C1 continuity:

* nonc1suspected - is a flag which is set upon discovering some indication
  of the discontinuity. If this flag is false, the rest of "non-C1" fields
  should be ignored
* nonc1fidx - is an index of the function (0 for  target  function,  1  or
  higher for nonlinear constraints) which is suspected of being "non-C1"
* nonc1lipshitzc - a Lipchitz constant for a function gradient  which  was
  suspected of being non-smooth.
* nonc1test0positive -  set  to  indicate  specific  test  which  detected
  continuity violation (test #0)
* nonc1test1positive -  set  to  indicate  specific  test  which  detected
  continuity violation (test #1)

Following set of report fields deals with errors in the gradient:
* badgradsuspected - is a flad which is set upon discovering an  error  in
  the analytic gradient supplied by user
* badgradfidx - index  of   the  function  with bad gradient (0 for target
  function, 1 or higher for nonlinear constraints)
* badgradvidx - index of the variable
* badgradxbase - location where Jacobian is tested
* following  matrices  store  user-supplied  Jacobian  and  its  numerical
  differentiation version (which is assumed to be  free  from  the  coding
  errors), both of them computed near the initial point:
  * badgraduser, an array[K,N], analytic Jacobian supplied by user
  * badgradnum,  an array[K,N], numeric  Jacobian computed by ALGLIB
  Here K is a total number of  nonlinear  functions  (target  +  nonlinear
  constraints), N is a variable number.
  The  element  of  badgraduser[] with index [badgradfidx,badgradvidx]  is
  assumed to be wrong.

More detailed error log can  be  obtained  from  optimizer  by  explicitly
requesting reports for tests C0.0, C1.0, C1.1.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    ae_bool nonc0suspected;
    ae_bool nonc0test0positive;
    ae_int_t nonc0fidx;
    double nonc0lipschitzc;
    ae_bool nonc1suspected;
    ae_bool nonc1test0positive;
    ae_bool nonc1test1positive;
    ae_int_t nonc1fidx;
    double nonc1lipschitzc;
    ae_bool badgradsuspected;
    ae_int_t badgradfidx;
    ae_int_t badgradvidx;
    ae_vector badgradxbase;
    ae_matrix badgraduser;
    ae_matrix badgradnum;
} optguardreport;


/*************************************************************************
This  structure  is  used  for  detailed   reporting  about  suspected  C0
continuity violation.

=== WHAT IS TESTED =======================================================

C0 test  studies  function  values (not gradient!)  obtained  during  line
searches and monitors estimate of the Lipschitz  constant.  Sudden  spikes
usually indicate that discontinuity was detected.


=== WHAT IS REPORTED =====================================================

Actually, report retrieval function returns TWO report structures:

* one for most suspicious point found so far (one with highest  change  in
  the function value), so called "strongest" report
* another one for most detailed line search (more function  evaluations  =
  easier to understand what's going on) which triggered  test #0 criteria,
  so called "longest" report

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* fidx - is an index of the function (0 for  target  function, 1 or higher
  for nonlinear constraints) which is suspected of being "non-C1"
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], f[] - arrays of length CNT which store step lengths and  function
  values at these points; f[i] is evaluated in x0+stp[i]*d.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.
* inneriter, outeriter - inner and outer iteration indexes (can be -1 if no
  iteration information was specified)

You can plot function values stored in stp[]  and  f[]  arrays  and  study
behavior of your function by your own eyes, just  to  be  sure  that  test
correctly reported C1 violation.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    ae_bool positive;
    ae_int_t fidx;
    ae_vector x0;
    ae_vector d;
    ae_int_t n;
    ae_vector stp;
    ae_vector f;
    ae_int_t cnt;
    ae_int_t stpidxa;
    ae_int_t stpidxb;
    ae_int_t inneriter;
    ae_int_t outeriter;
} optguardnonc0report;


/*************************************************************************
This  structure  is  used  for  detailed   reporting  about  suspected  C1
continuity violation as flagged by C1 test #0 (OptGuard  has several tests
for C1 continuity, this report is used by #0).

=== WHAT IS TESTED =======================================================

C1 test #0 studies function values (not gradient!)  obtained  during  line
searches and monitors behavior of directional  derivative  estimate.  This
test is less powerful than test #1, but it does  not  depend  on  gradient
values  and  thus  it  is  more  robust  against  artifacts  introduced by
numerical differentiation.


=== WHAT IS REPORTED =====================================================

Actually, report retrieval function returns TWO report structures:

* one for most suspicious point found so far (one with highest  change  in
  the directional derivative), so called "strongest" report
* another one for most detailed line search (more function  evaluations  =
  easier to understand what's going on) which triggered  test #0 criteria,
  so called "longest" report

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* fidx - is an index of the function (0 for  target  function, 1 or higher
  for nonlinear constraints) which is suspected of being "non-C1"
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], f[] - arrays of length CNT which store step lengths and  function
  values at these points; f[i] is evaluated in x0+stp[i]*d.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.
* inneriter, outeriter - inner and outer iteration indexes (can be -1 if no
  iteration information was specified)

You can plot function values stored in stp[]  and  f[]  arrays  and  study
behavior of your function by your own eyes, just  to  be  sure  that  test
correctly reported C1 violation.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    ae_bool positive;
    ae_int_t fidx;
    ae_vector x0;
    ae_vector d;
    ae_int_t n;
    ae_vector stp;
    ae_vector f;
    ae_int_t cnt;
    ae_int_t stpidxa;
    ae_int_t stpidxb;
    ae_int_t inneriter;
    ae_int_t outeriter;
} optguardnonc1test0report;


/*************************************************************************
This  structure  is  used  for  detailed   reporting  about  suspected  C1
continuity violation as flagged by C1 test #1 (OptGuard  has several tests
for C1 continuity, this report is used by #1).

=== WHAT IS TESTED =======================================================

C1 test #1 studies individual  components  of  the  gradient  as  recorded
during line searches. Upon discovering discontinuity in the gradient  this
test records specific component which was suspected (or  one  with  highest
indication of discontinuity if multiple components are suspected).

When precise analytic gradient is provided this test is more powerful than
test #0  which  works  with  function  values  and  ignores  user-provided
gradient.  However,  test  #0  becomes  more   powerful   when   numerical
differentiation is employed (in such cases test #1 detects  higher  levels
of numerical noise and becomes too conservative).

This test also tells specific components of the gradient which violate  C1
continuity, which makes it more informative than #0, which just tells that
continuity is violated.


=== WHAT IS REPORTED =====================================================

Actually, report retrieval function returns TWO report structures:

* one for most suspicious point found so far (one with highest  change  in
  the directional derivative), so called "strongest" report
* another one for most detailed line search (more function  evaluations  =
  easier to understand what's going on) which triggered  test #1 criteria,
  so called "longest" report

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* fidx - is an index of the function (0 for  target  function, 1 or higher
  for nonlinear constraints) which is suspected of being "non-C1"
* vidx - is an index of the variable in [0,N) with nonsmooth derivative
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], g[] - arrays of length CNT which store step lengths and  gradient
  values at these points; g[i] is evaluated in  x0+stp[i]*d  and  contains
  vidx-th component of the gradient.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.
* inneriter, outeriter - inner and outer iteration indexes (can be -1 if  no
  iteration information was specified)

You can plot function values stored in stp[]  and  g[]  arrays  and  study
behavior of your function by your own eyes, just  to  be  sure  that  test
correctly reported C1 violation.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    ae_bool positive;
    ae_int_t fidx;
    ae_int_t vidx;
    ae_vector x0;
    ae_vector d;
    ae_int_t n;
    ae_vector stp;
    ae_vector g;
    ae_int_t cnt;
    ae_int_t stpidxa;
    ae_int_t stpidxb;
    ae_int_t inneriter;
    ae_int_t outeriter;
} optguardnonc1test1report;


/*$ Body $*/


/*************************************************************************
This subroutine initializes "internal" OptGuard report,  i.e. one intended
for internal use by optimizers.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
void optguardinitinternal(optguardreport* rep,
     ae_int_t n,
     ae_int_t k,
     ae_state *_state);


/*************************************************************************
This subroutine exports report to user-readable representation (all arrays
are forced to have exactly same size as needed; unused arrays are  set  to
zero length).

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
void optguardexportreport(const optguardreport* srcrep,
     ae_int_t n,
     ae_int_t k,
     ae_bool badgradhasxj,
     optguardreport* dstrep,
     ae_state *_state);


/*************************************************************************
This subroutine exports report to user-readable representation (all arrays
are forced to have exactly same size as needed; unused arrays are  set  to
zero length).

NOTE: we assume that SrcRep contains scaled X0[] and  D[],  i.e.  explicit
      variable scaling was applied. We need to rescale them during export,
      that's why we need S[] parameter.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
void smoothnessmonitorexportc1test0report(const optguardnonc1test0report* srcrep,
     /* Real    */ const ae_vector* s,
     optguardnonc1test0report* dstrep,
     ae_state *_state);


/*************************************************************************
This subroutine exports report to user-readable representation (all arrays
are forced to have exactly same size as needed; unused arrays are  set  to
zero length).

NOTE: we assume that SrcRep contains scaled X0[], D[] and G[], i.e. explicit
      variable scaling was applied. We need to rescale them during export,
      that's why we need S[] parameter.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
void smoothnessmonitorexportc1test1report(const optguardnonc1test1report* srcrep,
     /* Real    */ const ae_vector* s,
     optguardnonc1test1report* dstrep,
     ae_state *_state);


/*************************************************************************
Returns True when all flags are clear. Intended for easy  coding  of  unit
tests.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
ae_bool optguardallclear(const optguardreport* rep, ae_state *_state);
void _optguardreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _optguardreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _optguardreport_clear(void* _p);
void _optguardreport_destroy(void* _p);
void _optguardnonc0report_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _optguardnonc0report_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _optguardnonc0report_clear(void* _p);
void _optguardnonc0report_destroy(void* _p);
void _optguardnonc1test0report_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _optguardnonc1test0report_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _optguardnonc1test0report_clear(void* _p);
void _optguardnonc1test0report_destroy(void* _p);
void _optguardnonc1test1report_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _optguardnonc1test1report_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _optguardnonc1test1report_clear(void* _p);
void _optguardnonc1test1report_destroy(void* _p);


/*$ End $*/
#endif

