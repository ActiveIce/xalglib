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

#ifndef _nearestneighbor_h
#define _nearestneighbor_h

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
Buffer object which is used to perform nearest neighbor  requests  in  the
multithreaded mode (multiple threads working with same KD-tree object).

This object should be created with KDTreeCreateRequestBuffer().
*************************************************************************/
typedef struct
{
    ae_vector x;
    ae_vector boxmin;
    ae_vector boxmax;
    ae_int_t kneeded;
    double rneeded;
    ae_bool selfmatch;
    double approxf;
    ae_int_t kcur;
    ae_vector idx;
    ae_vector r;
    ae_vector buf;
    ae_vector curboxmin;
    ae_vector curboxmax;
    double curdist;
} kdtreerequestbuffer;


/*************************************************************************
KD-tree object.
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t nx;
    ae_int_t ny;
    ae_int_t normtype;
    ae_matrix xy;
    ae_vector tags;
    ae_vector boxmin;
    ae_vector boxmax;
    ae_vector nodes;
    ae_vector splits;
    kdtreerequestbuffer innerbuf;
    ae_int_t debugcounter;
} kdtree;


/*$ Body $*/


/*************************************************************************
KD-tree creation

This subroutine creates KD-tree from set of X-values and optional Y-values

INPUT PARAMETERS
    XY      -   dataset, array[0..N-1,0..NX+NY-1].
                one row corresponds to one point.
                first NX columns contain X-values, next NY (NY may be zero)
                columns may contain associated Y-values
    N       -   number of points, N>=0.
    NX      -   space dimension, NX>=1.
    NY      -   number of optional Y-values, NY>=0.
    NormType-   norm type:
                * 0 denotes infinity-norm
                * 1 denotes 1-norm
                * 2 denotes 2-norm (Euclidean norm)
                
OUTPUT PARAMETERS
    KDT     -   KD-tree
    
    
NOTES

1. KD-tree  creation  have O(N*logN) complexity and O(N*(2*NX+NY))  memory
   requirements.
2. Although KD-trees may be used with any combination of N  and  NX,  they
   are more efficient than brute-force search only when N >> 4^NX. So they
   are most useful in low-dimensional tasks (NX=2, NX=3). NX=1  is another
   inefficient case, because  simple  binary  search  (without  additional
   structures) is much more efficient in such tasks than KD-trees.

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreebuild(/* Real    */ const ae_matrix* xy,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t normtype,
     kdtree* kdt,
     ae_state *_state);


/*************************************************************************
KD-tree creation

This  subroutine  creates  KD-tree  from set of X-values, integer tags and
optional Y-values

INPUT PARAMETERS
    XY      -   dataset, array[0..N-1,0..NX+NY-1].
                one row corresponds to one point.
                first NX columns contain X-values, next NY (NY may be zero)
                columns may contain associated Y-values
    Tags    -   tags, array[0..N-1], contains integer tags associated
                with points.
    N       -   number of points, N>=0
    NX      -   space dimension, NX>=1.
    NY      -   number of optional Y-values, NY>=0.
    NormType-   norm type:
                * 0 denotes infinity-norm
                * 1 denotes 1-norm
                * 2 denotes 2-norm (Euclidean norm)

OUTPUT PARAMETERS
    KDT     -   KD-tree

NOTES

1. KD-tree  creation  have O(N*logN) complexity and O(N*(2*NX+NY))  memory
   requirements.
2. Although KD-trees may be used with any combination of N  and  NX,  they
   are more efficient than brute-force search only when N >> 4^NX. So they
   are most useful in low-dimensional tasks (NX=2, NX=3). NX=1  is another
   inefficient case, because  simple  binary  search  (without  additional
   structures) is much more efficient in such tasks than KD-trees.

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreebuildtagged(/* Real    */ const ae_matrix* xy,
     /* Integer */ const ae_vector* tags,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t normtype,
     kdtree* kdt,
     ae_state *_state);


/*************************************************************************
This function creates buffer  structure  which  can  be  used  to  perform
parallel KD-tree requests.

KD-tree subpackage provides two sets of request functions - ones which use
internal buffer of KD-tree object  (these  functions  are  single-threaded
because they use same buffer, which can not shared between  threads),  and
ones which use external buffer.

This function is used to initialize external buffer.

INPUT PARAMETERS
    KDT         -   KD-tree which is associated with newly created buffer

OUTPUT PARAMETERS
    Buf         -   external buffer.
    
    
IMPORTANT: KD-tree buffer should be used only with  KD-tree  object  which
           was used to initialize buffer. Any attempt to use buffer   with
           different object is dangerous - you  may  get  integrity  check
           failure (exception) because sizes of internal arrays do not fit
           to dimensions of KD-tree structure.

  -- ALGLIB --
     Copyright 18.03.2016 by Bochkanov Sergey
*************************************************************************/
void kdtreecreaterequestbuffer(const kdtree* kdt,
     kdtreerequestbuffer* buf,
     ae_state *_state);


/*************************************************************************
K-NN query: K nearest neighbors

IMPORTANT: this function can not be used in multithreaded code because  it
           uses internal temporary buffer of kd-tree object, which can not
           be shared between multiple threads.  If  you  want  to  perform
           parallel requests, use function  which  uses  external  request
           buffer: KDTreeTsQueryKNN() ("Ts" stands for "thread-safe").

INPUT PARAMETERS
    KDT         -   KD-tree
    X           -   point, array[0..NX-1].
    K           -   number of neighbors to return, K>=1
    SelfMatch   -   whether self-matches are allowed:
                    * if True, nearest neighbor may be the point itself
                      (if it exists in original dataset)
                    * if False, then only points with non-zero distance
                      are returned
                    * if not given, considered True

RESULT
    number of actual neighbors found (either K or N, if K>N).

This  subroutine  performs  query  and  stores  its result in the internal
structures of the KD-tree. You can use  following  subroutines  to  obtain
these results:
* KDTreeQueryResultsX() to get X-values
* KDTreeQueryResultsXY() to get X- and Y-values
* KDTreeQueryResultsTags() to get tag values
* KDTreeQueryResultsDistances() to get distances

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
ae_int_t kdtreequeryknn(kdtree* kdt,
     /* Real    */ const ae_vector* x,
     ae_int_t k,
     ae_bool selfmatch,
     ae_state *_state);


/*************************************************************************
K-NN query: K nearest neighbors, using external thread-local buffer.

You can call this function from multiple threads for same kd-tree instance,
assuming that different instances of buffer object are passed to different
threads.

INPUT PARAMETERS
    KDT         -   kd-tree
    Buf         -   request buffer  object  created  for  this  particular
                    instance of kd-tree structure with kdtreecreaterequestbuffer()
                    function.
    X           -   point, array[0..NX-1].
    K           -   number of neighbors to return, K>=1
    SelfMatch   -   whether self-matches are allowed:
                    * if True, nearest neighbor may be the point itself
                      (if it exists in original dataset)
                    * if False, then only points with non-zero distance
                      are returned
                    * if not given, considered True

RESULT
    number of actual neighbors found (either K or N, if K>N).

This  subroutine  performs  query  and  stores  its result in the internal
structures  of  the  buffer object. You can use following  subroutines  to
obtain these results (pay attention to "buf" in their names):
* KDTreeTsQueryResultsX() to get X-values
* KDTreeTsQueryResultsXY() to get X- and Y-values
* KDTreeTsQueryResultsTags() to get tag values
* KDTreeTsQueryResultsDistances() to get distances
    
IMPORTANT: kd-tree buffer should be used only with  KD-tree  object  which
           was used to initialize buffer. Any attempt to use biffer   with
           different object is dangerous - you  may  get  integrity  check
           failure (exception) because sizes of internal arrays do not fit
           to dimensions of KD-tree structure.

  -- ALGLIB --
     Copyright 18.03.2016 by Bochkanov Sergey
*************************************************************************/
ae_int_t kdtreetsqueryknn(const kdtree* kdt,
     kdtreerequestbuffer* buf,
     /* Real    */ const ae_vector* x,
     ae_int_t k,
     ae_bool selfmatch,
     ae_state *_state);


/*************************************************************************
R-NN query: all points within R-sphere centered at X, ordered by  distance
between point and X (by ascending).

NOTE: it is also possible to perform undordered queries performed by means
      of kdtreequeryrnnu() and kdtreetsqueryrnnu() functions. Such queries
      are faster because we do not have to use heap structure for sorting.

IMPORTANT: this function can not be used in multithreaded code because  it
           uses internal temporary buffer of kd-tree object, which can not
           be shared between multiple threads.  If  you  want  to  perform
           parallel requests, use function  which  uses  external  request
           buffer: kdtreetsqueryrnn() ("Ts" stands for "thread-safe").

INPUT PARAMETERS
    KDT         -   KD-tree
    X           -   point, array[0..NX-1].
    R           -   radius of sphere (in corresponding norm), R>0
    SelfMatch   -   whether self-matches are allowed:
                    * if True, nearest neighbor may be the point itself
                      (if it exists in original dataset)
                    * if False, then only points with non-zero distance
                      are returned
                    * if not given, considered True

RESULT
    number of neighbors found, >=0

This  subroutine  performs  query  and  stores  its result in the internal
structures of the KD-tree. You can use  following  subroutines  to  obtain
actual results:
* KDTreeQueryResultsX() to get X-values
* KDTreeQueryResultsXY() to get X- and Y-values
* KDTreeQueryResultsTags() to get tag values
* KDTreeQueryResultsDistances() to get distances

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
ae_int_t kdtreequeryrnn(kdtree* kdt,
     /* Real    */ const ae_vector* x,
     double r,
     ae_bool selfmatch,
     ae_state *_state);


/*************************************************************************
R-NN query: all points within R-sphere  centered  at  X,  no  ordering  by
distance as undicated by "U" suffix (faster that ordered query, for  large
queries - significantly faster).

IMPORTANT: this function can not be used in multithreaded code because  it
           uses internal temporary buffer of kd-tree object, which can not
           be shared between multiple threads.  If  you  want  to  perform
           parallel requests, use function  which  uses  external  request
           buffer: kdtreetsqueryrnn() ("Ts" stands for "thread-safe").

INPUT PARAMETERS
    KDT         -   KD-tree
    X           -   point, array[0..NX-1].
    R           -   radius of sphere (in corresponding norm), R>0
    SelfMatch   -   whether self-matches are allowed:
                    * if True, nearest neighbor may be the point itself
                      (if it exists in original dataset)
                    * if False, then only points with non-zero distance
                      are returned
                    * if not given, considered True

RESULT
    number of neighbors found, >=0

This  subroutine  performs  query  and  stores  its result in the internal
structures of the KD-tree. You can use  following  subroutines  to  obtain
actual results:
* KDTreeQueryResultsX() to get X-values
* KDTreeQueryResultsXY() to get X- and Y-values
* KDTreeQueryResultsTags() to get tag values
* KDTreeQueryResultsDistances() to get distances

As indicated by "U" suffix, this function returns unordered results.

  -- ALGLIB --
     Copyright 01.11.2018 by Bochkanov Sergey
*************************************************************************/
ae_int_t kdtreequeryrnnu(kdtree* kdt,
     /* Real    */ const ae_vector* x,
     double r,
     ae_bool selfmatch,
     ae_state *_state);


/*************************************************************************
R-NN query: all points within  R-sphere  centered  at  X,  using  external
thread-local buffer, sorted by distance between point and X (by ascending)

You can call this function from multiple threads for same kd-tree instance,
assuming that different instances of buffer object are passed to different
threads.

NOTE: it is also possible to perform undordered queries performed by means
      of kdtreequeryrnnu() and kdtreetsqueryrnnu() functions. Such queries
      are faster because we do not have to use heap structure for sorting.

INPUT PARAMETERS
    KDT         -   KD-tree
    Buf         -   request buffer  object  created  for  this  particular
                    instance of kd-tree structure with kdtreecreaterequestbuffer()
                    function.
    X           -   point, array[0..NX-1].
    R           -   radius of sphere (in corresponding norm), R>0
    SelfMatch   -   whether self-matches are allowed:
                    * if True, nearest neighbor may be the point itself
                      (if it exists in original dataset)
                    * if False, then only points with non-zero distance
                      are returned
                    * if not given, considered True

RESULT
    number of neighbors found, >=0

This  subroutine  performs  query  and  stores  its result in the internal
structures  of  the  buffer object. You can use following  subroutines  to
obtain these results (pay attention to "buf" in their names):
* KDTreeTsQueryResultsX() to get X-values
* KDTreeTsQueryResultsXY() to get X- and Y-values
* KDTreeTsQueryResultsTags() to get tag values
* KDTreeTsQueryResultsDistances() to get distances
    
IMPORTANT: kd-tree buffer should be used only with  KD-tree  object  which
           was used to initialize buffer. Any attempt to use biffer   with
           different object is dangerous - you  may  get  integrity  check
           failure (exception) because sizes of internal arrays do not fit
           to dimensions of KD-tree structure.

  -- ALGLIB --
     Copyright 18.03.2016 by Bochkanov Sergey
*************************************************************************/
ae_int_t kdtreetsqueryrnn(const kdtree* kdt,
     kdtreerequestbuffer* buf,
     /* Real    */ const ae_vector* x,
     double r,
     ae_bool selfmatch,
     ae_state *_state);


/*************************************************************************
R-NN query: all points within  R-sphere  centered  at  X,  using  external
thread-local buffer, no ordering by distance as undicated  by  "U"  suffix
(faster that ordered query, for large queries - significantly faster).

You can call this function from multiple threads for same kd-tree instance,
assuming that different instances of buffer object are passed to different
threads.

INPUT PARAMETERS
    KDT         -   KD-tree
    Buf         -   request buffer  object  created  for  this  particular
                    instance of kd-tree structure with kdtreecreaterequestbuffer()
                    function.
    X           -   point, array[0..NX-1].
    R           -   radius of sphere (in corresponding norm), R>0
    SelfMatch   -   whether self-matches are allowed:
                    * if True, nearest neighbor may be the point itself
                      (if it exists in original dataset)
                    * if False, then only points with non-zero distance
                      are returned
                    * if not given, considered True

RESULT
    number of neighbors found, >=0

This  subroutine  performs  query  and  stores  its result in the internal
structures  of  the  buffer object. You can use following  subroutines  to
obtain these results (pay attention to "buf" in their names):
* KDTreeTsQueryResultsX() to get X-values
* KDTreeTsQueryResultsXY() to get X- and Y-values
* KDTreeTsQueryResultsTags() to get tag values
* KDTreeTsQueryResultsDistances() to get distances

As indicated by "U" suffix, this function returns unordered results.
    
IMPORTANT: kd-tree buffer should be used only with  KD-tree  object  which
           was used to initialize buffer. Any attempt to use biffer   with
           different object is dangerous - you  may  get  integrity  check
           failure (exception) because sizes of internal arrays do not fit
           to dimensions of KD-tree structure.

  -- ALGLIB --
     Copyright 18.03.2016 by Bochkanov Sergey
*************************************************************************/
ae_int_t kdtreetsqueryrnnu(const kdtree* kdt,
     kdtreerequestbuffer* buf,
     /* Real    */ const ae_vector* x,
     double r,
     ae_bool selfmatch,
     ae_state *_state);


/*************************************************************************
K-NN query: approximate K nearest neighbors

IMPORTANT: this function can not be used in multithreaded code because  it
           uses internal temporary buffer of kd-tree object, which can not
           be shared between multiple threads.  If  you  want  to  perform
           parallel requests, use function  which  uses  external  request
           buffer: KDTreeTsQueryAKNN() ("Ts" stands for "thread-safe").

INPUT PARAMETERS
    KDT         -   KD-tree
    X           -   point, array[0..NX-1].
    K           -   number of neighbors to return, K>=1
    SelfMatch   -   whether self-matches are allowed:
                    * if True, nearest neighbor may be the point itself
                      (if it exists in original dataset)
                    * if False, then only points with non-zero distance
                      are returned
                    * if not given, considered True
    Eps         -   approximation factor, Eps>=0. eps-approximate  nearest
                    neighbor  is  a  neighbor  whose distance from X is at
                    most (1+eps) times distance of true nearest neighbor.

RESULT
    number of actual neighbors found (either K or N, if K>N).
    
NOTES
    significant performance gain may be achieved only when Eps  is  is  on
    the order of magnitude of 1 or larger.

This  subroutine  performs  query  and  stores  its result in the internal
structures of the KD-tree. You can use  following  subroutines  to  obtain
these results:
* KDTreeQueryResultsX() to get X-values
* KDTreeQueryResultsXY() to get X- and Y-values
* KDTreeQueryResultsTags() to get tag values
* KDTreeQueryResultsDistances() to get distances

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
ae_int_t kdtreequeryaknn(kdtree* kdt,
     /* Real    */ const ae_vector* x,
     ae_int_t k,
     ae_bool selfmatch,
     double eps,
     ae_state *_state);


/*************************************************************************
K-NN query: approximate K nearest neighbors, using thread-local buffer.

You can call this function from multiple threads for same kd-tree instance,
assuming that different instances of buffer object are passed to different
threads.

INPUT PARAMETERS
    KDT         -   KD-tree
    Buf         -   request buffer  object  created  for  this  particular
                    instance of kd-tree structure with kdtreecreaterequestbuffer()
                    function.
    X           -   point, array[0..NX-1].
    K           -   number of neighbors to return, K>=1
    SelfMatch   -   whether self-matches are allowed:
                    * if True, nearest neighbor may be the point itself
                      (if it exists in original dataset)
                    * if False, then only points with non-zero distance
                      are returned
                    * if not given, considered True
    Eps         -   approximation factor, Eps>=0. eps-approximate  nearest
                    neighbor  is  a  neighbor  whose distance from X is at
                    most (1+eps) times distance of true nearest neighbor.

RESULT
    number of actual neighbors found (either K or N, if K>N).
    
NOTES
    significant performance gain may be achieved only when Eps  is  is  on
    the order of magnitude of 1 or larger.

This  subroutine  performs  query  and  stores  its result in the internal
structures  of  the  buffer object. You can use following  subroutines  to
obtain these results (pay attention to "buf" in their names):
* KDTreeTsQueryResultsX() to get X-values
* KDTreeTsQueryResultsXY() to get X- and Y-values
* KDTreeTsQueryResultsTags() to get tag values
* KDTreeTsQueryResultsDistances() to get distances
    
IMPORTANT: kd-tree buffer should be used only with  KD-tree  object  which
           was used to initialize buffer. Any attempt to use biffer   with
           different object is dangerous - you  may  get  integrity  check
           failure (exception) because sizes of internal arrays do not fit
           to dimensions of KD-tree structure.

  -- ALGLIB --
     Copyright 18.03.2016 by Bochkanov Sergey
*************************************************************************/
ae_int_t kdtreetsqueryaknn(const kdtree* kdt,
     kdtreerequestbuffer* buf,
     /* Real    */ const ae_vector* x,
     ae_int_t k,
     ae_bool selfmatch,
     double eps,
     ae_state *_state);


/*************************************************************************
Box query: all points within user-specified box.

IMPORTANT: this function can not be used in multithreaded code because  it
           uses internal temporary buffer of kd-tree object, which can not
           be shared between multiple threads.  If  you  want  to  perform
           parallel requests, use function  which  uses  external  request
           buffer: KDTreeTsQueryBox() ("Ts" stands for "thread-safe").

INPUT PARAMETERS
    KDT         -   KD-tree
    BoxMin      -   lower bounds, array[0..NX-1].
    BoxMax      -   upper bounds, array[0..NX-1].


RESULT
    number of actual neighbors found (in [0,N]).

This  subroutine  performs  query  and  stores  its result in the internal
structures of the KD-tree. You can use  following  subroutines  to  obtain
these results:
* KDTreeQueryResultsX() to get X-values
* KDTreeQueryResultsXY() to get X- and Y-values
* KDTreeQueryResultsTags() to get tag values
* KDTreeQueryResultsDistances() returns zeros for this request
    
NOTE: this particular query returns unordered results, because there is no
      meaningful way of  ordering  points.  Furthermore,  no 'distance' is
      associated with points - it is either INSIDE  or OUTSIDE (so request
      for distances will return zeros).

  -- ALGLIB --
     Copyright 14.05.2016 by Bochkanov Sergey
*************************************************************************/
ae_int_t kdtreequerybox(kdtree* kdt,
     /* Real    */ const ae_vector* boxmin,
     /* Real    */ const ae_vector* boxmax,
     ae_state *_state);


/*************************************************************************
Box query: all points within user-specified box, using thread-local buffer.

You can call this function from multiple threads for same kd-tree instance,
assuming that different instances of buffer object are passed to different
threads.

INPUT PARAMETERS
    KDT         -   KD-tree
    Buf         -   request buffer  object  created  for  this  particular
                    instance of kd-tree structure with kdtreecreaterequestbuffer()
                    function.
    BoxMin      -   lower bounds, array[0..NX-1].
    BoxMax      -   upper bounds, array[0..NX-1].

RESULT
    number of actual neighbors found (in [0,N]).

This  subroutine  performs  query  and  stores  its result in the internal
structures  of  the  buffer object. You can use following  subroutines  to
obtain these results (pay attention to "ts" in their names):
* KDTreeTsQueryResultsX() to get X-values
* KDTreeTsQueryResultsXY() to get X- and Y-values
* KDTreeTsQueryResultsTags() to get tag values
* KDTreeTsQueryResultsDistances() returns zeros for this query
    
NOTE: this particular query returns unordered results, because there is no
      meaningful way of  ordering  points.  Furthermore,  no 'distance' is
      associated with points - it is either INSIDE  or OUTSIDE (so request
      for distances will return zeros).
    
IMPORTANT: kd-tree buffer should be used only with  KD-tree  object  which
           was used to initialize buffer. Any attempt to use biffer   with
           different object is dangerous - you  may  get  integrity  check
           failure (exception) because sizes of internal arrays do not fit
           to dimensions of KD-tree structure.

  -- ALGLIB --
     Copyright 14.05.2016 by Bochkanov Sergey
*************************************************************************/
ae_int_t kdtreetsquerybox(const kdtree* kdt,
     kdtreerequestbuffer* buf,
     /* Real    */ const ae_vector* boxmin,
     /* Real    */ const ae_vector* boxmax,
     ae_state *_state);


/*************************************************************************
X-values from last query.

This function retuns results stored in  the  internal  buffer  of  kd-tree
object. If you performed buffered requests (ones which  use  instances  of
kdtreerequestbuffer class), you  should  call  buffered  version  of  this
function - kdtreetsqueryresultsx().

INPUT PARAMETERS
    KDT     -   KD-tree
    X       -   possibly pre-allocated buffer. If X is too small to store
                result, it is resized. If size(X) is enough to store
                result, it is left unchanged.

OUTPUT PARAMETERS
    X       -   rows are filled with X-values

NOTES
1. points are ordered by distance from the query point (first = closest)
2. if  XY is larger than required to store result, only leading part  will
   be overwritten; trailing part will be left unchanged. So  if  on  input
   XY = [[A,B],[C,D]], and result is [1,2],  then  on  exit  we  will  get
   XY = [[1,2],[C,D]]. This is done purposely to increase performance;  if
   you want function  to  resize  array  according  to  result  size,  use
   function with same name and suffix 'I'.

SEE ALSO
* KDTreeQueryResultsXY()            X- and Y-values
* KDTreeQueryResultsTags()          tag values
* KDTreeQueryResultsDistances()     distances

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreequeryresultsx(const kdtree* kdt,
     /* Real    */ ae_matrix* x,
     ae_state *_state);


/*************************************************************************
X- and Y-values from last query

This function retuns results stored in  the  internal  buffer  of  kd-tree
object. If you performed buffered requests (ones which  use  instances  of
kdtreerequestbuffer class), you  should  call  buffered  version  of  this
function - kdtreetsqueryresultsxy().

INPUT PARAMETERS
    KDT     -   KD-tree
    XY      -   possibly pre-allocated buffer. If XY is too small to store
                result, it is resized. If size(XY) is enough to store
                result, it is left unchanged.

OUTPUT PARAMETERS
    XY      -   rows are filled with points: first NX columns with
                X-values, next NY columns - with Y-values.

NOTES
1. points are ordered by distance from the query point (first = closest)
2. if  XY is larger than required to store result, only leading part  will
   be overwritten; trailing part will be left unchanged. So  if  on  input
   XY = [[A,B],[C,D]], and result is [1,2],  then  on  exit  we  will  get
   XY = [[1,2],[C,D]]. This is done purposely to increase performance;  if
   you want function  to  resize  array  according  to  result  size,  use
   function with same name and suffix 'I'.

SEE ALSO
* KDTreeQueryResultsX()             X-values
* KDTreeQueryResultsTags()          tag values
* KDTreeQueryResultsDistances()     distances

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreequeryresultsxy(const kdtree* kdt,
     /* Real    */ ae_matrix* xy,
     ae_state *_state);


/*************************************************************************
Tags from last query

This function retuns results stored in  the  internal  buffer  of  kd-tree
object. If you performed buffered requests (ones which  use  instances  of
kdtreerequestbuffer class), you  should  call  buffered  version  of  this
function - kdtreetsqueryresultstags().

INPUT PARAMETERS
    KDT     -   KD-tree
    Tags    -   possibly pre-allocated buffer. If X is too small to store
                result, it is resized. If size(X) is enough to store
                result, it is left unchanged.

OUTPUT PARAMETERS
    Tags    -   filled with tags associated with points,
                or, when no tags were supplied, with zeros

NOTES
1. points are ordered by distance from the query point (first = closest)
2. if  XY is larger than required to store result, only leading part  will
   be overwritten; trailing part will be left unchanged. So  if  on  input
   XY = [[A,B],[C,D]], and result is [1,2],  then  on  exit  we  will  get
   XY = [[1,2],[C,D]]. This is done purposely to increase performance;  if
   you want function  to  resize  array  according  to  result  size,  use
   function with same name and suffix 'I'.

SEE ALSO
* KDTreeQueryResultsX()             X-values
* KDTreeQueryResultsXY()            X- and Y-values
* KDTreeQueryResultsDistances()     distances

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreequeryresultstags(const kdtree* kdt,
     /* Integer */ ae_vector* tags,
     ae_state *_state);


/*************************************************************************
Distances from last query

This function retuns results stored in  the  internal  buffer  of  kd-tree
object. If you performed buffered requests (ones which  use  instances  of
kdtreerequestbuffer class), you  should  call  buffered  version  of  this
function - kdtreetsqueryresultsdistances().

INPUT PARAMETERS
    KDT     -   KD-tree
    R       -   possibly pre-allocated buffer. If X is too small to store
                result, it is resized. If size(X) is enough to store
                result, it is left unchanged.

OUTPUT PARAMETERS
    R       -   filled with distances (in corresponding norm)

NOTES
1. points are ordered by distance from the query point (first = closest)
2. if  XY is larger than required to store result, only leading part  will
   be overwritten; trailing part will be left unchanged. So  if  on  input
   XY = [[A,B],[C,D]], and result is [1,2],  then  on  exit  we  will  get
   XY = [[1,2],[C,D]]. This is done purposely to increase performance;  if
   you want function  to  resize  array  according  to  result  size,  use
   function with same name and suffix 'I'.

SEE ALSO
* KDTreeQueryResultsX()             X-values
* KDTreeQueryResultsXY()            X- and Y-values
* KDTreeQueryResultsTags()          tag values

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreequeryresultsdistances(const kdtree* kdt,
     /* Real    */ ae_vector* r,
     ae_state *_state);


/*************************************************************************
X-values from last query associated with kdtreerequestbuffer object.

INPUT PARAMETERS
    KDT     -   KD-tree
    Buf     -   request  buffer  object  created   for   this   particular
                instance of kd-tree structure.
    X       -   possibly pre-allocated buffer. If X is too small to store
                result, it is resized. If size(X) is enough to store
                result, it is left unchanged.

OUTPUT PARAMETERS
    X       -   rows are filled with X-values

NOTES
1. points are ordered by distance from the query point (first = closest)
2. if  XY is larger than required to store result, only leading part  will
   be overwritten; trailing part will be left unchanged. So  if  on  input
   XY = [[A,B],[C,D]], and result is [1,2],  then  on  exit  we  will  get
   XY = [[1,2],[C,D]]. This is done purposely to increase performance;  if
   you want function  to  resize  array  according  to  result  size,  use
   function with same name and suffix 'I'.

SEE ALSO
* KDTreeQueryResultsXY()            X- and Y-values
* KDTreeQueryResultsTags()          tag values
* KDTreeQueryResultsDistances()     distances

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreetsqueryresultsx(const kdtree* kdt,
     const kdtreerequestbuffer* buf,
     /* Real    */ ae_matrix* x,
     ae_state *_state);


/*************************************************************************
X- and Y-values from last query associated with kdtreerequestbuffer object.

INPUT PARAMETERS
    KDT     -   KD-tree
    Buf     -   request  buffer  object  created   for   this   particular
                instance of kd-tree structure.
    XY      -   possibly pre-allocated buffer. If XY is too small to store
                result, it is resized. If size(XY) is enough to store
                result, it is left unchanged.

OUTPUT PARAMETERS
    XY      -   rows are filled with points: first NX columns with
                X-values, next NY columns - with Y-values.

NOTES
1. points are ordered by distance from the query point (first = closest)
2. if  XY is larger than required to store result, only leading part  will
   be overwritten; trailing part will be left unchanged. So  if  on  input
   XY = [[A,B],[C,D]], and result is [1,2],  then  on  exit  we  will  get
   XY = [[1,2],[C,D]]. This is done purposely to increase performance;  if
   you want function  to  resize  array  according  to  result  size,  use
   function with same name and suffix 'I'.

SEE ALSO
* KDTreeQueryResultsX()             X-values
* KDTreeQueryResultsTags()          tag values
* KDTreeQueryResultsDistances()     distances

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreetsqueryresultsxy(const kdtree* kdt,
     const kdtreerequestbuffer* buf,
     /* Real    */ ae_matrix* xy,
     ae_state *_state);


/*************************************************************************
Tags from last query associated with kdtreerequestbuffer object.

This function retuns results stored in  the  internal  buffer  of  kd-tree
object. If you performed buffered requests (ones which  use  instances  of
kdtreerequestbuffer class), you  should  call  buffered  version  of  this
function - KDTreeTsqueryresultstags().

INPUT PARAMETERS
    KDT     -   KD-tree
    Buf     -   request  buffer  object  created   for   this   particular
                instance of kd-tree structure.
    Tags    -   possibly pre-allocated buffer. If X is too small to store
                result, it is resized. If size(X) is enough to store
                result, it is left unchanged.

OUTPUT PARAMETERS
    Tags    -   filled with tags associated with points,
                or, when no tags were supplied, with zeros

NOTES
1. points are ordered by distance from the query point (first = closest)
2. if  XY is larger than required to store result, only leading part  will
   be overwritten; trailing part will be left unchanged. So  if  on  input
   XY = [[A,B],[C,D]], and result is [1,2],  then  on  exit  we  will  get
   XY = [[1,2],[C,D]]. This is done purposely to increase performance;  if
   you want function  to  resize  array  according  to  result  size,  use
   function with same name and suffix 'I'.

SEE ALSO
* KDTreeQueryResultsX()             X-values
* KDTreeQueryResultsXY()            X- and Y-values
* KDTreeQueryResultsDistances()     distances

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreetsqueryresultstags(const kdtree* kdt,
     const kdtreerequestbuffer* buf,
     /* Integer */ ae_vector* tags,
     ae_state *_state);


/*************************************************************************
Distances from last query associated with kdtreerequestbuffer object.

This function retuns results stored in  the  internal  buffer  of  kd-tree
object. If you performed buffered requests (ones which  use  instances  of
kdtreerequestbuffer class), you  should  call  buffered  version  of  this
function - KDTreeTsqueryresultsdistances().

INPUT PARAMETERS
    KDT     -   KD-tree
    Buf     -   request  buffer  object  created   for   this   particular
                instance of kd-tree structure.
    R       -   possibly pre-allocated buffer. If X is too small to store
                result, it is resized. If size(X) is enough to store
                result, it is left unchanged.

OUTPUT PARAMETERS
    R       -   filled with distances (in corresponding norm)

NOTES
1. points are ordered by distance from the query point (first = closest)
2. if  XY is larger than required to store result, only leading part  will
   be overwritten; trailing part will be left unchanged. So  if  on  input
   XY = [[A,B],[C,D]], and result is [1,2],  then  on  exit  we  will  get
   XY = [[1,2],[C,D]]. This is done purposely to increase performance;  if
   you want function  to  resize  array  according  to  result  size,  use
   function with same name and suffix 'I'.

SEE ALSO
* KDTreeQueryResultsX()             X-values
* KDTreeQueryResultsXY()            X- and Y-values
* KDTreeQueryResultsTags()          tag values

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreetsqueryresultsdistances(const kdtree* kdt,
     const kdtreerequestbuffer* buf,
     /* Real    */ ae_vector* r,
     ae_state *_state);


/*************************************************************************
X-values from last query; 'interactive' variant for languages like  Python
which   support    constructs   like  "X = KDTreeQueryResultsXI(KDT)"  and
interactive mode of interpreter.

This function allocates new array on each call,  so  it  is  significantly
slower than its 'non-interactive' counterpart, but it is  more  convenient
when you call it from command line.

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreequeryresultsxi(const kdtree* kdt,
     /* Real    */ ae_matrix* x,
     ae_state *_state);


/*************************************************************************
XY-values from last query; 'interactive' variant for languages like Python
which   support    constructs   like "XY = KDTreeQueryResultsXYI(KDT)" and
interactive mode of interpreter.

This function allocates new array on each call,  so  it  is  significantly
slower than its 'non-interactive' counterpart, but it is  more  convenient
when you call it from command line.

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreequeryresultsxyi(const kdtree* kdt,
     /* Real    */ ae_matrix* xy,
     ae_state *_state);


/*************************************************************************
Tags  from  last  query;  'interactive' variant for languages like  Python
which  support  constructs  like "Tags = KDTreeQueryResultsTagsI(KDT)" and
interactive mode of interpreter.

This function allocates new array on each call,  so  it  is  significantly
slower than its 'non-interactive' counterpart, but it is  more  convenient
when you call it from command line.

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreequeryresultstagsi(const kdtree* kdt,
     /* Integer */ ae_vector* tags,
     ae_state *_state);


/*************************************************************************
Distances from last query; 'interactive' variant for languages like Python
which  support  constructs   like  "R = KDTreeQueryResultsDistancesI(KDT)"
and interactive mode of interpreter.

This function allocates new array on each call,  so  it  is  significantly
slower than its 'non-interactive' counterpart, but it is  more  convenient
when you call it from command line.

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreequeryresultsdistancesi(const kdtree* kdt,
     /* Real    */ ae_vector* r,
     ae_state *_state);


/*************************************************************************
It is informational function which returns bounding box for entire dataset.
This function is not visible to ALGLIB users, only ALGLIB itself  may  use
it.

This function assumes that output buffers are preallocated by caller.

  -- ALGLIB --
     Copyright 20.06.2016 by Bochkanov Sergey
*************************************************************************/
void kdtreeexplorebox(const kdtree* kdt,
     /* Real    */ ae_vector* boxmin,
     /* Real    */ ae_vector* boxmax,
     ae_state *_state);


/*************************************************************************
It is informational function which allows to get  information  about  node
type. Node index is given by integer value, with 0  corresponding  to root
node and other node indexes obtained via exploration.

You should not expect that serialization/unserialization will retain  node
indexes. You should keep in  mind  that  future  versions  of  ALGLIB  may
introduce new node types.

OUTPUT VALUES:
    NodeType    -   node type:
                    * 0 corresponds to leaf node, which can be explored by
                      kdtreeexploreleaf() function
                    * 1 corresponds to split node, which can  be  explored
                      by kdtreeexploresplit() function

  -- ALGLIB --
     Copyright 20.06.2016 by Bochkanov Sergey
*************************************************************************/
void kdtreeexplorenodetype(const kdtree* kdt,
     ae_int_t node,
     ae_int_t* nodetype,
     ae_state *_state);


/*************************************************************************
It is informational function which allows to get  information  about  leaf
node. Node index is given by integer value, with 0  corresponding  to root
node and other node indexes obtained via exploration.

You should not expect that serialization/unserialization will retain  node
indexes. You should keep in  mind  that  future  versions  of  ALGLIB  may
introduce new node types.

OUTPUT VALUES:
    XT      -   output buffer is reallocated (if too small) and filled by
                XY values
    K       -   number of rows in XY

  -- ALGLIB --
     Copyright 20.06.2016 by Bochkanov Sergey
*************************************************************************/
void kdtreeexploreleaf(const kdtree* kdt,
     ae_int_t node,
     /* Real    */ ae_matrix* xy,
     ae_int_t* k,
     ae_state *_state);


/*************************************************************************
It is informational function which allows to get  information  about split
node. Node index is given by integer value, with 0  corresponding  to root
node and other node indexes obtained via exploration.

You should not expect that serialization/unserialization will retain  node
indexes. You should keep in  mind  that  future  versions  of  ALGLIB  may
introduce new node types.

OUTPUT VALUES:
    XT      -   output buffer is reallocated (if too small) and filled by
                XY values
    K       -   number of rows in XY

    //      Nodes[idx+1]=dim    dimension to split
    //      Nodes[idx+2]=offs   offset of splitting point in Splits[]
    //      Nodes[idx+3]=left   position of left child in Nodes[]
    //      Nodes[idx+4]=right  position of right child in Nodes[]
    
  -- ALGLIB --
     Copyright 20.06.2016 by Bochkanov Sergey
*************************************************************************/
void kdtreeexploresplit(const kdtree* kdt,
     ae_int_t node,
     ae_int_t* d,
     double* s,
     ae_int_t* nodele,
     ae_int_t* nodege,
     ae_state *_state);


/*************************************************************************
Serializer: allocation

  -- ALGLIB --
     Copyright 14.03.2011 by Bochkanov Sergey
*************************************************************************/
void kdtreealloc(ae_serializer* s, const kdtree* tree, ae_state *_state);


/*************************************************************************
Serializer: serialization

  -- ALGLIB --
     Copyright 14.03.2011 by Bochkanov Sergey
*************************************************************************/
void kdtreeserialize(ae_serializer* s,
     const kdtree* tree,
     ae_state *_state);


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 14.03.2011 by Bochkanov Sergey
*************************************************************************/
void kdtreeunserialize(ae_serializer* s, kdtree* tree, ae_state *_state);
void _kdtreerequestbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _kdtreerequestbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _kdtreerequestbuffer_clear(void* _p);
void _kdtreerequestbuffer_destroy(void* _p);
void _kdtree_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _kdtree_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _kdtree_clear(void* _p);
void _kdtree_destroy(void* _p);


/*$ End $*/
#endif

