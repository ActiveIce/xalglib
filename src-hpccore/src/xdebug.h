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

#ifndef _xdebug_h
#define _xdebug_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"


/*$ Declarations $*/


/*************************************************************************
This is a debug class intended for testing ALGLIB interface generator.
Never use it in any real life project.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    ae_int_t i;
    ae_complex c;
    ae_vector a;
} xdebugrecord1;


/*$ Body $*/


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Creates and returns XDebugRecord1 structure:
* integer and complex fields of Rec1 are set to 1 and 1+i correspondingly
* array field of Rec1 is set to [2,3]

  -- ALGLIB --
     Copyright 27.05.2014 by Bochkanov Sergey
*************************************************************************/
void xdebuginitrecord1(xdebugrecord1* rec1, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Creates and returns XDebugRecord1 structure:
* integer and complex fields of Rec1 are set to 1 and 1+i correspondingly
* array field of Rec1 is set to [2,3]

  -- ALGLIB --
     Copyright 27.05.2014 by Bochkanov Sergey
*************************************************************************/
void xdebugupdaterecord1(xdebugrecord1* rec1, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Counts number of True values in the boolean 1D array.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
ae_int_t xdebugb1count(/* Boolean */ const ae_vector* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Replace all values in array by NOT(a[i]).
Array is passed using "shared" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugb1not(/* Boolean */ ae_vector* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Appends copy of array to itself.
Array is passed using "var" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugb1appendcopy(/* Boolean */ ae_vector* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Generate N-element array with even-numbered elements set to True.
Array is passed using "out" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugb1outeven(ae_int_t n,
     /* Boolean */ ae_vector* a,
     ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Returns sum of elements in the array.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
ae_int_t xdebugi1sum(/* Integer */ const ae_vector* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Replace all values in array by -A[I]
Array is passed using "shared" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugi1neg(/* Integer */ ae_vector* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Appends copy of array to itself.
Array is passed using "var" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugi1appendcopy(/* Integer */ ae_vector* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Generate N-element array with even-numbered A[I] set to I, and odd-numbered
ones set to 0.

Array is passed using "out" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugi1outeven(ae_int_t n,
     /* Integer */ ae_vector* a,
     ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Returns sum of elements in the array.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
double xdebugr1sum(/* Real    */ const ae_vector* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Returns sum of elements in the array.

Internally it creates a copy of the array.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
double xdebugr1internalcopyandsum(/* Real    */ const ae_vector* _a,
     ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Replace all values in array by -A[I]
Array is passed using "shared" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugr1neg(/* Real    */ ae_vector* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Appends copy of array to itself.
Array is passed using "var" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugr1appendcopy(/* Real    */ ae_vector* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Generate N-element array with even-numbered A[I] set to I*0.25,
and odd-numbered ones are set to 0.

Array is passed using "out" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugr1outeven(ae_int_t n,
     /* Real    */ ae_vector* a,
     ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Returns sum of elements in the array.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
ae_complex xdebugc1sum(/* Complex */ const ae_vector* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Replace all values in array by -A[I]
Array is passed using "shared" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugc1neg(/* Complex */ ae_vector* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Appends copy of array to itself.
Array is passed using "var" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugc1appendcopy(/* Complex */ ae_vector* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Generate N-element array with even-numbered A[K] set to (x,y) = (K*0.25, K*0.125)
and odd-numbered ones are set to 0.

Array is passed using "out" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugc1outeven(ae_int_t n,
     /* Complex */ ae_vector* a,
     ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Counts number of True values in the boolean 2D array.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
ae_int_t xdebugb2count(/* Boolean */ const ae_matrix* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Replace all values in array by NOT(a[i]).
Array is passed using "shared" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugb2not(/* Boolean */ ae_matrix* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Transposes array.
Array is passed using "var" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugb2transpose(/* Boolean */ ae_matrix* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Generate MxN matrix with elements set to "Sin(3*I+5*J)>0"
Array is passed using "out" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugb2outsin(ae_int_t m,
     ae_int_t n,
     /* Boolean */ ae_matrix* a,
     ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Returns sum of elements in the array.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
ae_int_t xdebugi2sum(/* Integer */ const ae_matrix* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Replace all values in array by -a[i,j]
Array is passed using "shared" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugi2neg(/* Integer */ ae_matrix* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Transposes array.
Array is passed using "var" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugi2transpose(/* Integer */ ae_matrix* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Generate MxN matrix with elements set to "Sign(Sin(3*I+5*J))"
Array is passed using "out" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugi2outsin(ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_matrix* a,
     ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Returns sum of elements in the array.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
double xdebugr2sum(/* Real    */ const ae_matrix* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Returns sum of elements in the array.

Internally it creates a copy of a.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
double xdebugr2internalcopyandsum(/* Real    */ const ae_matrix* _a,
     ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Replace all values in array by -a[i,j]
Array is passed using "shared" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugr2neg(/* Real    */ ae_matrix* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Transposes array.
Array is passed using "var" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugr2transpose(/* Real    */ ae_matrix* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Generate MxN matrix with elements set to "Sin(3*I+5*J)"
Array is passed using "out" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugr2outsin(ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* a,
     ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Returns sum of elements in the array.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
ae_complex xdebugc2sum(/* Complex */ const ae_matrix* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Replace all values in array by -a[i,j]
Array is passed using "shared" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugc2neg(/* Complex */ ae_matrix* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Transposes array.
Array is passed using "var" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugc2transpose(/* Complex */ ae_matrix* a, ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Generate MxN matrix with elements set to "Sin(3*I+5*J),Cos(3*I+5*J)"
Array is passed using "out" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugc2outsincos(ae_int_t m,
     ae_int_t n,
     /* Complex */ ae_matrix* a,
     ae_state *_state);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Returns sum of a[i,j]*(1+b[i,j]) such that c[i,j] is True

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
double xdebugmaskedbiasedproductsum(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_matrix* b,
     /* Boolean */ const ae_matrix* c,
     ae_state *_state);
void _xdebugrecord1_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _xdebugrecord1_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _xdebugrecord1_clear(void* _p);
void _xdebugrecord1_destroy(void* _p);


/*$ End $*/
#endif

