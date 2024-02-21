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
#include "nearestneighbor.h"


/*$ Declarations $*/
static ae_int_t nearestneighbor_splitnodesize = 6;
static ae_int_t nearestneighbor_kdtreefirstversion = 0;
static ae_int_t nearestneighbor_tsqueryrnn(const kdtree* kdt,
     kdtreerequestbuffer* buf,
     /* Real    */ const ae_vector* x,
     double r,
     ae_bool selfmatch,
     ae_bool orderedbydist,
     ae_state *_state);
static void nearestneighbor_kdtreesplit(kdtree* kdt,
     ae_int_t i1,
     ae_int_t i2,
     ae_int_t d,
     double s,
     ae_int_t* i3,
     ae_state *_state);
static void nearestneighbor_kdtreegeneratetreerec(kdtree* kdt,
     ae_int_t* nodesoffs,
     ae_int_t* splitsoffs,
     ae_int_t i1,
     ae_int_t i2,
     ae_int_t maxleafsize,
     ae_state *_state);
static void nearestneighbor_kdtreequerynnrec(const kdtree* kdt,
     kdtreerequestbuffer* buf,
     ae_int_t offs,
     ae_state *_state);
static void nearestneighbor_kdtreequeryboxrec(const kdtree* kdt,
     kdtreerequestbuffer* buf,
     ae_int_t offs,
     ae_state *_state);
static void nearestneighbor_kdtreeinitbox(const kdtree* kdt,
     /* Real    */ const ae_vector* x,
     kdtreerequestbuffer* buf,
     ae_state *_state);
static void nearestneighbor_kdtreeallocdatasetindependent(kdtree* kdt,
     ae_int_t nx,
     ae_int_t ny,
     ae_state *_state);
static void nearestneighbor_kdtreeallocdatasetdependent(kdtree* kdt,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t ny,
     ae_state *_state);
static void nearestneighbor_checkrequestbufferconsistency(const kdtree* kdt,
     const kdtreerequestbuffer* buf,
     ae_state *_state);


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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector tags;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&tags, 0, sizeof(tags));
    _kdtree_clear(kdt);
    ae_vector_init(&tags, 0, DT_INT, _state, ae_true);

    ae_assert(n>=0, "KDTreeBuild: N<0", _state);
    ae_assert(nx>=1, "KDTreeBuild: NX<1", _state);
    ae_assert(ny>=0, "KDTreeBuild: NY<0", _state);
    ae_assert(normtype>=0&&normtype<=2, "KDTreeBuild: incorrect NormType", _state);
    ae_assert(xy->rows>=n, "KDTreeBuild: rows(X)<N", _state);
    ae_assert(xy->cols>=nx+ny||n==0, "KDTreeBuild: cols(X)<NX+NY", _state);
    ae_assert(apservisfinitematrix(xy, n, nx+ny, _state), "KDTreeBuild: XY contains infinite or NaN values", _state);
    if( n>0 )
    {
        ae_vector_set_length(&tags, n, _state);
        for(i=0; i<=n-1; i++)
        {
            tags.ptr.p_int[i] = 0;
        }
    }
    kdtreebuildtagged(xy, &tags, n, nx, ny, normtype, kdt, _state);
    ae_frame_leave(_state);
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t nodesoffs;
    ae_int_t splitsoffs;

    _kdtree_clear(kdt);

    ae_assert(n>=0, "KDTreeBuildTagged: N<0", _state);
    ae_assert(nx>=1, "KDTreeBuildTagged: NX<1", _state);
    ae_assert(ny>=0, "KDTreeBuildTagged: NY<0", _state);
    ae_assert(normtype>=0&&normtype<=2, "KDTreeBuildTagged: incorrect NormType", _state);
    ae_assert(xy->rows>=n, "KDTreeBuildTagged: rows(X)<N", _state);
    ae_assert(xy->cols>=nx+ny||n==0, "KDTreeBuildTagged: cols(X)<NX+NY", _state);
    ae_assert(apservisfinitematrix(xy, n, nx+ny, _state), "KDTreeBuildTagged: XY contains infinite or NaN values", _state);
    
    /*
     * initialize
     */
    kdt->n = n;
    kdt->nx = nx;
    kdt->ny = ny;
    kdt->normtype = normtype;
    kdt->innerbuf.kcur = 0;
    
    /*
     * N=0 => quick exit
     */
    if( n==0 )
    {
        return;
    }
    
    /*
     * Allocate
     */
    nearestneighbor_kdtreeallocdatasetindependent(kdt, nx, ny, _state);
    nearestneighbor_kdtreeallocdatasetdependent(kdt, n, nx, ny, _state);
    kdtreecreaterequestbuffer(kdt, &kdt->innerbuf, _state);
    
    /*
     * Initial fill
     */
    for(i=0; i<=n-1; i++)
    {
        ae_v_move(&kdt->xy.ptr.pp_double[i][0], 1, &xy->ptr.pp_double[i][0], 1, ae_v_len(0,nx-1));
        ae_v_move(&kdt->xy.ptr.pp_double[i][nx], 1, &xy->ptr.pp_double[i][0], 1, ae_v_len(nx,2*nx+ny-1));
        kdt->tags.ptr.p_int[i] = tags->ptr.p_int[i];
    }
    
    /*
     * Determine bounding box
     */
    ae_v_move(&kdt->boxmin.ptr.p_double[0], 1, &kdt->xy.ptr.pp_double[0][0], 1, ae_v_len(0,nx-1));
    ae_v_move(&kdt->boxmax.ptr.p_double[0], 1, &kdt->xy.ptr.pp_double[0][0], 1, ae_v_len(0,nx-1));
    for(i=1; i<=n-1; i++)
    {
        for(j=0; j<=nx-1; j++)
        {
            kdt->boxmin.ptr.p_double[j] = ae_minreal(kdt->boxmin.ptr.p_double[j], kdt->xy.ptr.pp_double[i][j], _state);
            kdt->boxmax.ptr.p_double[j] = ae_maxreal(kdt->boxmax.ptr.p_double[j], kdt->xy.ptr.pp_double[i][j], _state);
        }
    }
    
    /*
     * Generate tree
     */
    nodesoffs = 0;
    splitsoffs = 0;
    ae_v_move(&kdt->innerbuf.curboxmin.ptr.p_double[0], 1, &kdt->boxmin.ptr.p_double[0], 1, ae_v_len(0,nx-1));
    ae_v_move(&kdt->innerbuf.curboxmax.ptr.p_double[0], 1, &kdt->boxmax.ptr.p_double[0], 1, ae_v_len(0,nx-1));
    nearestneighbor_kdtreegeneratetreerec(kdt, &nodesoffs, &splitsoffs, 0, n, 8, _state);
    ivectorresize(&kdt->nodes, nodesoffs, _state);
    rvectorresize(&kdt->splits, splitsoffs, _state);
}


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
     ae_state *_state)
{

    _kdtreerequestbuffer_clear(buf);

    ae_vector_set_length(&buf->x, kdt->nx, _state);
    ae_vector_set_length(&buf->boxmin, kdt->nx, _state);
    ae_vector_set_length(&buf->boxmax, kdt->nx, _state);
    ae_vector_set_length(&buf->idx, kdt->n, _state);
    ae_vector_set_length(&buf->r, kdt->n, _state);
    ae_vector_set_length(&buf->buf, ae_maxint(kdt->n, kdt->nx, _state), _state);
    ae_vector_set_length(&buf->curboxmin, kdt->nx, _state);
    ae_vector_set_length(&buf->curboxmax, kdt->nx, _state);
    buf->kcur = 0;
}


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
     ae_state *_state)
{
    ae_int_t result;


    ae_assert(k>=1, "KDTreeQueryKNN: K<1!", _state);
    ae_assert(x->cnt>=kdt->nx, "KDTreeQueryKNN: Length(X)<NX!", _state);
    ae_assert(isfinitevector(x, kdt->nx, _state), "KDTreeQueryKNN: X contains infinite or NaN values!", _state);
    result = kdtreetsqueryaknn(kdt, &kdt->innerbuf, x, k, selfmatch, 0.0, _state);
    return result;
}


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
     ae_state *_state)
{
    ae_int_t result;


    ae_assert(k>=1, "KDTreeTsQueryKNN: K<1!", _state);
    ae_assert(x->cnt>=kdt->nx, "KDTreeTsQueryKNN: Length(X)<NX!", _state);
    ae_assert(isfinitevector(x, kdt->nx, _state), "KDTreeTsQueryKNN: X contains infinite or NaN values!", _state);
    result = kdtreetsqueryaknn(kdt, buf, x, k, selfmatch, 0.0, _state);
    return result;
}


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
     ae_state *_state)
{
    ae_int_t result;


    ae_assert(ae_fp_greater(r,(double)(0)), "KDTreeQueryRNN: incorrect R!", _state);
    ae_assert(x->cnt>=kdt->nx, "KDTreeQueryRNN: Length(X)<NX!", _state);
    ae_assert(isfinitevector(x, kdt->nx, _state), "KDTreeQueryRNN: X contains infinite or NaN values!", _state);
    result = kdtreetsqueryrnn(kdt, &kdt->innerbuf, x, r, selfmatch, _state);
    return result;
}


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
     ae_state *_state)
{
    ae_int_t result;


    ae_assert(ae_fp_greater(r,(double)(0)), "KDTreeQueryRNNU: incorrect R!", _state);
    ae_assert(x->cnt>=kdt->nx, "KDTreeQueryRNNU: Length(X)<NX!", _state);
    ae_assert(isfinitevector(x, kdt->nx, _state), "KDTreeQueryRNNU: X contains infinite or NaN values!", _state);
    result = kdtreetsqueryrnnu(kdt, &kdt->innerbuf, x, r, selfmatch, _state);
    return result;
}


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
     ae_state *_state)
{
    ae_int_t result;


    ae_assert(ae_isfinite(r, _state)&&ae_fp_greater(r,(double)(0)), "KDTreeTsQueryRNN: incorrect R!", _state);
    ae_assert(x->cnt>=kdt->nx, "KDTreeTsQueryRNN: Length(X)<NX!", _state);
    ae_assert(isfinitevector(x, kdt->nx, _state), "KDTreeTsQueryRNN: X contains infinite or NaN values!", _state);
    result = nearestneighbor_tsqueryrnn(kdt, buf, x, r, selfmatch, ae_true, _state);
    return result;
}


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
     ae_state *_state)
{
    ae_int_t result;


    ae_assert(ae_isfinite(r, _state)&&ae_fp_greater(r,(double)(0)), "KDTreeTsQueryRNNU: incorrect R!", _state);
    ae_assert(x->cnt>=kdt->nx, "KDTreeTsQueryRNNU: Length(X)<NX!", _state);
    ae_assert(isfinitevector(x, kdt->nx, _state), "KDTreeTsQueryRNNU: X contains infinite or NaN values!", _state);
    result = nearestneighbor_tsqueryrnn(kdt, buf, x, r, selfmatch, ae_false, _state);
    return result;
}


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
     ae_state *_state)
{
    ae_int_t result;


    result = kdtreetsqueryaknn(kdt, &kdt->innerbuf, x, k, selfmatch, eps, _state);
    return result;
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t result;


    ae_assert(k>0, "KDTreeTsQueryAKNN: incorrect K!", _state);
    ae_assert(ae_fp_greater_eq(eps,(double)(0)), "KDTreeTsQueryAKNN: incorrect Eps!", _state);
    ae_assert(x->cnt>=kdt->nx, "KDTreeTsQueryAKNN: Length(X)<NX!", _state);
    ae_assert(isfinitevector(x, kdt->nx, _state), "KDTreeTsQueryAKNN: X contains infinite or NaN values!", _state);
    
    /*
     * Handle special case: KDT.N=0
     */
    if( kdt->n==0 )
    {
        buf->kcur = 0;
        result = 0;
        return result;
    }
    
    /*
     * Check consistency of request buffer
     */
    nearestneighbor_checkrequestbufferconsistency(kdt, buf, _state);
    
    /*
     * Prepare parameters
     */
    k = ae_minint(k, kdt->n, _state);
    buf->kneeded = k;
    buf->rneeded = (double)(0);
    buf->selfmatch = selfmatch;
    if( kdt->normtype==2 )
    {
        buf->approxf = (double)1/ae_sqr((double)1+eps, _state);
    }
    else
    {
        buf->approxf = (double)1/((double)1+eps);
    }
    buf->kcur = 0;
    
    /*
     * calculate distance from point to current bounding box
     */
    nearestneighbor_kdtreeinitbox(kdt, x, buf, _state);
    
    /*
     * call recursive search
     * results are returned as heap
     */
    nearestneighbor_kdtreequerynnrec(kdt, buf, 0, _state);
    
    /*
     * pop from heap to generate ordered representation
     *
     * last element is non pop'ed because it is already in
     * its place
     */
    result = buf->kcur;
    j = buf->kcur;
    for(i=buf->kcur; i>=2; i--)
    {
        tagheappopi(&buf->r, &buf->idx, &j, _state);
    }
    return result;
}


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
     ae_state *_state)
{
    ae_int_t result;


    result = kdtreetsquerybox(kdt, &kdt->innerbuf, boxmin, boxmax, _state);
    return result;
}


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
     ae_state *_state)
{
    ae_int_t j;
    ae_int_t result;


    ae_assert(boxmin->cnt>=kdt->nx, "KDTreeTsQueryBox: Length(BoxMin)<NX!", _state);
    ae_assert(boxmax->cnt>=kdt->nx, "KDTreeTsQueryBox: Length(BoxMax)<NX!", _state);
    ae_assert(isfinitevector(boxmin, kdt->nx, _state), "KDTreeTsQueryBox: BoxMin contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(boxmax, kdt->nx, _state), "KDTreeTsQueryBox: BoxMax contains infinite or NaN values!", _state);
    
    /*
     * Check consistency of request buffer
     */
    nearestneighbor_checkrequestbufferconsistency(kdt, buf, _state);
    
    /*
     * Quick exit for degenerate boxes
     */
    for(j=0; j<=kdt->nx-1; j++)
    {
        if( ae_fp_greater(boxmin->ptr.p_double[j],boxmax->ptr.p_double[j]) )
        {
            buf->kcur = 0;
            result = 0;
            return result;
        }
    }
    
    /*
     * Prepare parameters
     */
    for(j=0; j<=kdt->nx-1; j++)
    {
        buf->boxmin.ptr.p_double[j] = boxmin->ptr.p_double[j];
        buf->boxmax.ptr.p_double[j] = boxmax->ptr.p_double[j];
        buf->curboxmin.ptr.p_double[j] = boxmin->ptr.p_double[j];
        buf->curboxmax.ptr.p_double[j] = boxmax->ptr.p_double[j];
    }
    buf->kcur = 0;
    
    /*
     * call recursive search
     */
    nearestneighbor_kdtreequeryboxrec(kdt, buf, 0, _state);
    result = buf->kcur;
    return result;
}


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
     ae_state *_state)
{


    kdtreetsqueryresultsx(kdt, &kdt->innerbuf, x, _state);
}


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
     ae_state *_state)
{


    kdtreetsqueryresultsxy(kdt, &kdt->innerbuf, xy, _state);
}


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
     ae_state *_state)
{


    kdtreetsqueryresultstags(kdt, &kdt->innerbuf, tags, _state);
}


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
     ae_state *_state)
{


    kdtreetsqueryresultsdistances(kdt, &kdt->innerbuf, r, _state);
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;


    if( buf->kcur==0 )
    {
        return;
    }
    if( x->rows<buf->kcur||x->cols<kdt->nx )
    {
        ae_matrix_set_length(x, buf->kcur, kdt->nx, _state);
    }
    k = buf->kcur;
    for(i=0; i<=k-1; i++)
    {
        ae_v_move(&x->ptr.pp_double[i][0], 1, &kdt->xy.ptr.pp_double[buf->idx.ptr.p_int[i]][kdt->nx], 1, ae_v_len(0,kdt->nx-1));
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;


    if( buf->kcur==0 )
    {
        return;
    }
    if( xy->rows<buf->kcur||xy->cols<kdt->nx+kdt->ny )
    {
        ae_matrix_set_length(xy, buf->kcur, kdt->nx+kdt->ny, _state);
    }
    k = buf->kcur;
    for(i=0; i<=k-1; i++)
    {
        ae_v_move(&xy->ptr.pp_double[i][0], 1, &kdt->xy.ptr.pp_double[buf->idx.ptr.p_int[i]][kdt->nx], 1, ae_v_len(0,kdt->nx+kdt->ny-1));
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;


    if( buf->kcur==0 )
    {
        return;
    }
    if( tags->cnt<buf->kcur )
    {
        ae_vector_set_length(tags, buf->kcur, _state);
    }
    k = buf->kcur;
    for(i=0; i<=k-1; i++)
    {
        tags->ptr.p_int[i] = kdt->tags.ptr.p_int[buf->idx.ptr.p_int[i]];
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;


    if( buf->kcur==0 )
    {
        return;
    }
    if( r->cnt<buf->kcur )
    {
        ae_vector_set_length(r, buf->kcur, _state);
    }
    k = buf->kcur;
    
    /*
     * unload norms
     *
     * Abs() call is used to handle cases with negative norms
     * (generated during KFN requests)
     */
    if( kdt->normtype==0 )
    {
        for(i=0; i<=k-1; i++)
        {
            r->ptr.p_double[i] = ae_fabs(buf->r.ptr.p_double[i], _state);
        }
    }
    if( kdt->normtype==1 )
    {
        for(i=0; i<=k-1; i++)
        {
            r->ptr.p_double[i] = ae_fabs(buf->r.ptr.p_double[i], _state);
        }
    }
    if( kdt->normtype==2 )
    {
        for(i=0; i<=k-1; i++)
        {
            r->ptr.p_double[i] = ae_sqrt(ae_fabs(buf->r.ptr.p_double[i], _state), _state);
        }
    }
}


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
     ae_state *_state)
{

    ae_matrix_clear(x);

    kdtreequeryresultsx(kdt, x, _state);
}


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
     ae_state *_state)
{

    ae_matrix_clear(xy);

    kdtreequeryresultsxy(kdt, xy, _state);
}


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
     ae_state *_state)
{

    ae_vector_clear(tags);

    kdtreequeryresultstags(kdt, tags, _state);
}


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
     ae_state *_state)
{

    ae_vector_clear(r);

    kdtreequeryresultsdistances(kdt, r, _state);
}


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
     ae_state *_state)
{
    ae_int_t i;


    rvectorsetlengthatleast(boxmin, kdt->nx, _state);
    rvectorsetlengthatleast(boxmax, kdt->nx, _state);
    for(i=0; i<=kdt->nx-1; i++)
    {
        boxmin->ptr.p_double[i] = kdt->boxmin.ptr.p_double[i];
        boxmax->ptr.p_double[i] = kdt->boxmax.ptr.p_double[i];
    }
}


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
     ae_state *_state)
{

    *nodetype = 0;

    ae_assert(node>=0, "KDTreeExploreNodeType: incorrect node", _state);
    ae_assert(node<kdt->nodes.cnt, "KDTreeExploreNodeType: incorrect node", _state);
    if( kdt->nodes.ptr.p_int[node]>0 )
    {
        
        /*
         * Leaf node
         */
        *nodetype = 0;
        return;
    }
    if( kdt->nodes.ptr.p_int[node]==0 )
    {
        
        /*
         * Split node
         */
        *nodetype = 1;
        return;
    }
    ae_assert(ae_false, "KDTreeExploreNodeType: integrity check failure", _state);
}


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
     ae_state *_state)
{
    ae_int_t offs;
    ae_int_t i;
    ae_int_t j;

    *k = 0;

    ae_assert(node>=0, "KDTreeExploreLeaf: incorrect node index", _state);
    ae_assert(node+1<kdt->nodes.cnt, "KDTreeExploreLeaf: incorrect node index", _state);
    ae_assert(kdt->nodes.ptr.p_int[node]>0, "KDTreeExploreLeaf: incorrect node index", _state);
    *k = kdt->nodes.ptr.p_int[node];
    offs = kdt->nodes.ptr.p_int[node+1];
    ae_assert(offs>=0, "KDTreeExploreLeaf: integrity error", _state);
    ae_assert(offs+(*k)-1<kdt->xy.rows, "KDTreeExploreLeaf: integrity error", _state);
    rmatrixsetlengthatleast(xy, *k, kdt->nx+kdt->ny, _state);
    for(i=0; i<=*k-1; i++)
    {
        for(j=0; j<=kdt->nx+kdt->ny-1; j++)
        {
            xy->ptr.pp_double[i][j] = kdt->xy.ptr.pp_double[offs+i][kdt->nx+j];
        }
    }
}


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
     ae_state *_state)
{

    *d = 0;
    *s = 0.0;
    *nodele = 0;
    *nodege = 0;

    ae_assert(node>=0, "KDTreeExploreSplit: incorrect node index", _state);
    ae_assert(node+4<kdt->nodes.cnt, "KDTreeExploreSplit: incorrect node index", _state);
    ae_assert(kdt->nodes.ptr.p_int[node]==0, "KDTreeExploreSplit: incorrect node index", _state);
    *d = kdt->nodes.ptr.p_int[node+1];
    *s = kdt->splits.ptr.p_double[kdt->nodes.ptr.p_int[node+2]];
    *nodele = kdt->nodes.ptr.p_int[node+3];
    *nodege = kdt->nodes.ptr.p_int[node+4];
    ae_assert(*d>=0, "KDTreeExploreSplit: integrity failure", _state);
    ae_assert(*d<kdt->nx, "KDTreeExploreSplit: integrity failure", _state);
    ae_assert(ae_isfinite(*s, _state), "KDTreeExploreSplit: integrity failure", _state);
    ae_assert(*nodele>=0, "KDTreeExploreSplit: integrity failure", _state);
    ae_assert(*nodele<kdt->nodes.cnt, "KDTreeExploreSplit: integrity failure", _state);
    ae_assert(*nodege>=0, "KDTreeExploreSplit: integrity failure", _state);
    ae_assert(*nodege<kdt->nodes.cnt, "KDTreeExploreSplit: integrity failure", _state);
}


/*************************************************************************
Serializer: allocation

  -- ALGLIB --
     Copyright 14.03.2011 by Bochkanov Sergey
*************************************************************************/
void kdtreealloc(ae_serializer* s, const kdtree* tree, ae_state *_state)
{


    
    /*
     * Header
     */
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    
    /*
     * Data
     */
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    ae_serializer_alloc_entry(s);
    allocrealmatrix(s, &tree->xy, -1, -1, _state);
    allocintegerarray(s, &tree->tags, -1, _state);
    allocrealarray(s, &tree->boxmin, -1, _state);
    allocrealarray(s, &tree->boxmax, -1, _state);
    allocintegerarray(s, &tree->nodes, -1, _state);
    allocrealarray(s, &tree->splits, -1, _state);
}


/*************************************************************************
Serializer: serialization

  -- ALGLIB --
     Copyright 14.03.2011 by Bochkanov Sergey
*************************************************************************/
void kdtreeserialize(ae_serializer* s,
     const kdtree* tree,
     ae_state *_state)
{


    
    /*
     * Header
     */
    ae_serializer_serialize_int(s, getkdtreeserializationcode(_state), _state);
    ae_serializer_serialize_int(s, nearestneighbor_kdtreefirstversion, _state);
    
    /*
     * Data
     */
    ae_serializer_serialize_int(s, tree->n, _state);
    ae_serializer_serialize_int(s, tree->nx, _state);
    ae_serializer_serialize_int(s, tree->ny, _state);
    ae_serializer_serialize_int(s, tree->normtype, _state);
    serializerealmatrix(s, &tree->xy, -1, -1, _state);
    serializeintegerarray(s, &tree->tags, -1, _state);
    serializerealarray(s, &tree->boxmin, -1, _state);
    serializerealarray(s, &tree->boxmax, -1, _state);
    serializeintegerarray(s, &tree->nodes, -1, _state);
    serializerealarray(s, &tree->splits, -1, _state);
}


/*************************************************************************
Serializer: unserialization

  -- ALGLIB --
     Copyright 14.03.2011 by Bochkanov Sergey
*************************************************************************/
void kdtreeunserialize(ae_serializer* s, kdtree* tree, ae_state *_state)
{
    ae_int_t i0;
    ae_int_t i1;

    _kdtree_clear(tree);

    
    /*
     * check correctness of header
     */
    ae_serializer_unserialize_int(s, &i0, _state);
    ae_assert(i0==getkdtreeserializationcode(_state), "KDTreeUnserialize: stream header corrupted", _state);
    ae_serializer_unserialize_int(s, &i1, _state);
    ae_assert(i1==nearestneighbor_kdtreefirstversion, "KDTreeUnserialize: stream header corrupted", _state);
    
    /*
     * Unserialize data
     */
    ae_serializer_unserialize_int(s, &tree->n, _state);
    ae_serializer_unserialize_int(s, &tree->nx, _state);
    ae_serializer_unserialize_int(s, &tree->ny, _state);
    ae_serializer_unserialize_int(s, &tree->normtype, _state);
    unserializerealmatrix(s, &tree->xy, _state);
    unserializeintegerarray(s, &tree->tags, _state);
    unserializerealarray(s, &tree->boxmin, _state);
    unserializerealarray(s, &tree->boxmax, _state);
    unserializeintegerarray(s, &tree->nodes, _state);
    unserializerealarray(s, &tree->splits, _state);
    kdtreecreaterequestbuffer(tree, &tree->innerbuf, _state);
}


/*************************************************************************
This function returns an approximate cost (measured in CPU  cycles)  of  a
R-NN query with some fixed R.

A VERY CRUDE APPROXIMATION IS RETURNED, ONLY AN ORDER OF MAGNITUDE CORRECT

This approximation can be used in a multithreaded code which decides whether
it makes sense to activate multithreading or not.

Internally this function is implemented by performing some small (about 50)
amount of random queries and computing an average number of R-neighbors of
a node. This number is multiplied by log2(NPoints), and then  by  a  crude
estimate of how many CPU cycles is required to perform a split during kd-tree
search.

This function is deterministic, i.e. it always uses a fixed seed to initialize
its internal RNG.

IMPORTANT: this function can not be used in multithreaded code because  it
           uses internal temporary buffer of kd-tree object, which can not
           be shared between multiple threads.
           
           See kdtreeapproxrnnquerycost() for a thread-safe alternative.

INPUT PARAMETERS
    KDT         -   KD-tree
    R           -   radius of sphere (in corresponding norm), R>0

RESULT
    double precision number, approximate query cost

  -- ALGLIB --
     Copyright 24.11.2023 by Bochkanov Sergey
*************************************************************************/
double kdtreeapproxrnnquerycost(kdtree* kdt, double r, ae_state *_state)
{
    double result;


    result = kdtreetsapproxrnnquerycost(kdt, &kdt->innerbuf, r, _state);
    return result;
}


/*************************************************************************
This function returns an approximate cost (measured in CPU  cycles)  of  a
R-NN query with some fixed R.

A thread-safe version of kdtreeapproxrnnquerycost() using a request buffer.

  -- ALGLIB --
     Copyright 24.11.2023 by Bochkanov Sergey
*************************************************************************/
double kdtreetsapproxrnnquerycost(const kdtree* kdt,
     kdtreerequestbuffer* buf,
     double r,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t cnt;
    ae_int_t nx;
    double log2n;
    double avgrnn;
    hqrndstate rs;
    double result;

    ae_frame_make(_state, &_frame_block);
    memset(&rs, 0, sizeof(rs));
    _hqrndstate_init(&rs, _state, ae_true);

    ae_assert(ae_isfinite(r, _state)&&ae_fp_greater(r,(double)(0)), "KDTreeApproxRNNQueryCost: incorrect R!", _state);
    hqrndseed(46532, 66356, &rs, _state);
    nx = kdt->nx;
    cnt = ae_minint(50, kdt->n, _state);
    log2n = ae_log((double)(1+kdt->n), _state)/ae_log((double)(2), _state);
    avgrnn = (double)(0);
    rallocv(nx, &buf->xqc, _state);
    for(i=0; i<=cnt-1; i++)
    {
        k = hqrnduniformi(&rs, kdt->n, _state);
        for(j=0; j<=nx-1; j++)
        {
            buf->xqc.ptr.p_double[j] = kdt->xy.ptr.pp_double[k][nx+j];
        }
        avgrnn = avgrnn+(double)nearestneighbor_tsqueryrnn(kdt, buf, &buf->xqc, r, ae_true, ae_false, _state)/(double)cnt;
    }
    result = avgrnn*log2n*(double)15;
    ae_frame_leave(_state);
    return result;
}


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
static ae_int_t nearestneighbor_tsqueryrnn(const kdtree* kdt,
     kdtreerequestbuffer* buf,
     /* Real    */ const ae_vector* x,
     double r,
     ae_bool selfmatch,
     ae_bool orderedbydist,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t result;


    
    /*
     * Handle special case: KDT.N=0
     */
    if( kdt->n==0 )
    {
        buf->kcur = 0;
        result = 0;
        return result;
    }
    
    /*
     * Check consistency of request buffer
     */
    nearestneighbor_checkrequestbufferconsistency(kdt, buf, _state);
    
    /*
     * Prepare parameters
     */
    buf->kneeded = 0;
    if( kdt->normtype!=2 )
    {
        buf->rneeded = r;
    }
    else
    {
        buf->rneeded = ae_sqr(r, _state);
    }
    buf->selfmatch = selfmatch;
    buf->approxf = (double)(1);
    buf->kcur = 0;
    
    /*
     * calculate distance from point to current bounding box
     */
    nearestneighbor_kdtreeinitbox(kdt, x, buf, _state);
    
    /*
     * call recursive search
     * results are returned as heap
     */
    nearestneighbor_kdtreequerynnrec(kdt, buf, 0, _state);
    result = buf->kcur;
    
    /*
     * pop from heap to generate ordered representation
     *
     * last element is not pop'ed because it is already in
     * its place
     */
    if( orderedbydist )
    {
        j = buf->kcur;
        for(i=buf->kcur; i>=2; i--)
        {
            tagheappopi(&buf->r, &buf->idx, &j, _state);
        }
    }
    return result;
}


/*************************************************************************
Rearranges nodes [I1,I2) using partition in D-th dimension with S as threshold.
Returns split position I3: [I1,I3) and [I3,I2) are created as result.

This subroutine doesn't create tree structures, just rearranges nodes.
*************************************************************************/
static void nearestneighbor_kdtreesplit(kdtree* kdt,
     ae_int_t i1,
     ae_int_t i2,
     ae_int_t d,
     double s,
     ae_int_t* i3,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t ileft;
    ae_int_t iright;
    double v;

    *i3 = 0;

    ae_assert(kdt->n>0, "KDTreeSplit: internal error", _state);
    
    /*
     * split XY/Tags in two parts:
     * * [ILeft,IRight] is non-processed part of XY/Tags
     *
     * After cycle is done, we have Ileft=IRight. We deal with
     * this element separately.
     *
     * After this, [I1,ILeft) contains left part, and [ILeft,I2)
     * contains right part.
     */
    ileft = i1;
    iright = i2-1;
    while(ileft<iright)
    {
        if( kdt->xy.ptr.pp_double[ileft][d]<=s )
        {
            
            /*
             * XY[ILeft] is on its place.
             * Advance ILeft.
             */
            ileft = ileft+1;
        }
        else
        {
            
            /*
             * XY[ILeft,..] must be at IRight.
             * Swap and advance IRight.
             */
            for(i=0; i<=2*kdt->nx+kdt->ny-1; i++)
            {
                v = kdt->xy.ptr.pp_double[ileft][i];
                kdt->xy.ptr.pp_double[ileft][i] = kdt->xy.ptr.pp_double[iright][i];
                kdt->xy.ptr.pp_double[iright][i] = v;
            }
            j = kdt->tags.ptr.p_int[ileft];
            kdt->tags.ptr.p_int[ileft] = kdt->tags.ptr.p_int[iright];
            kdt->tags.ptr.p_int[iright] = j;
            iright = iright-1;
        }
    }
    if( kdt->xy.ptr.pp_double[ileft][d]<=s )
    {
        ileft = ileft+1;
    }
    else
    {
        iright = iright-1;
    }
    *i3 = ileft;
}


/*************************************************************************
Recursive kd-tree generation subroutine.

PARAMETERS
    KDT         tree
    NodesOffs   unused part of Nodes[] which must be filled by tree
    SplitsOffs  unused part of Splits[]
    I1, I2      points from [I1,I2) are processed
    
NodesOffs[] and SplitsOffs[] must be large enough.

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
static void nearestneighbor_kdtreegeneratetreerec(kdtree* kdt,
     ae_int_t* nodesoffs,
     ae_int_t* splitsoffs,
     ae_int_t i1,
     ae_int_t i2,
     ae_int_t maxleafsize,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nx;
    ae_int_t ny;
    ae_int_t i;
    ae_int_t j;
    ae_int_t oldoffs;
    ae_int_t i3;
    ae_int_t cntless;
    ae_int_t cntgreater;
    double minv;
    double maxv;
    ae_int_t minidx;
    ae_int_t maxidx;
    ae_int_t d;
    double ds;
    double s;
    double v;
    double v0;
    double v1;


    ae_assert(kdt->n>0, "KDTreeGenerateTreeRec: internal error", _state);
    ae_assert(i2>i1, "KDTreeGenerateTreeRec: internal error", _state);
    
    /*
     * Generate leaf if needed
     */
    if( i2-i1<=maxleafsize )
    {
        kdt->nodes.ptr.p_int[*nodesoffs+0] = i2-i1;
        kdt->nodes.ptr.p_int[*nodesoffs+1] = i1;
        *nodesoffs = *nodesoffs+2;
        return;
    }
    
    /*
     * Load values for easier access
     */
    nx = kdt->nx;
    ny = kdt->ny;
    
    /*
     * Select dimension to split:
     * * D is a dimension number
     * In case bounding box has zero size, we enforce creation of the leaf node.
     */
    d = 0;
    ds = kdt->innerbuf.curboxmax.ptr.p_double[0]-kdt->innerbuf.curboxmin.ptr.p_double[0];
    for(i=1; i<=nx-1; i++)
    {
        v = kdt->innerbuf.curboxmax.ptr.p_double[i]-kdt->innerbuf.curboxmin.ptr.p_double[i];
        if( v>ds )
        {
            ds = v;
            d = i;
        }
    }
    if( ae_fp_eq(ds,(double)(0)) )
    {
        kdt->nodes.ptr.p_int[*nodesoffs+0] = i2-i1;
        kdt->nodes.ptr.p_int[*nodesoffs+1] = i1;
        *nodesoffs = *nodesoffs+2;
        return;
    }
    
    /*
     * Select split position S using sliding midpoint rule,
     * rearrange points into [I1,I3) and [I3,I2).
     *
     * In case all points has same value of D-th component
     * (MinV=MaxV) we enforce D-th dimension of bounding
     * box to become exactly zero and repeat tree construction.
     */
    s = kdt->innerbuf.curboxmin.ptr.p_double[d]+0.5*ds;
    ae_v_move(&kdt->innerbuf.buf.ptr.p_double[0], 1, &kdt->xy.ptr.pp_double[i1][d], kdt->xy.stride, ae_v_len(0,i2-i1-1));
    n = i2-i1;
    cntless = 0;
    cntgreater = 0;
    minv = kdt->innerbuf.buf.ptr.p_double[0];
    maxv = kdt->innerbuf.buf.ptr.p_double[0];
    minidx = i1;
    maxidx = i1;
    for(i=0; i<=n-1; i++)
    {
        v = kdt->innerbuf.buf.ptr.p_double[i];
        if( v<minv )
        {
            minv = v;
            minidx = i1+i;
        }
        if( v>maxv )
        {
            maxv = v;
            maxidx = i1+i;
        }
        if( v<s )
        {
            cntless = cntless+1;
        }
        if( v>s )
        {
            cntgreater = cntgreater+1;
        }
    }
    if( minv==maxv )
    {
        
        /*
         * In case all points has same value of D-th component
         * (MinV=MaxV) we enforce D-th dimension of bounding
         * box to become exactly zero and repeat tree construction.
         */
        v0 = kdt->innerbuf.curboxmin.ptr.p_double[d];
        v1 = kdt->innerbuf.curboxmax.ptr.p_double[d];
        kdt->innerbuf.curboxmin.ptr.p_double[d] = minv;
        kdt->innerbuf.curboxmax.ptr.p_double[d] = maxv;
        nearestneighbor_kdtreegeneratetreerec(kdt, nodesoffs, splitsoffs, i1, i2, maxleafsize, _state);
        kdt->innerbuf.curboxmin.ptr.p_double[d] = v0;
        kdt->innerbuf.curboxmax.ptr.p_double[d] = v1;
        return;
    }
    if( cntless>0&&cntgreater>0 )
    {
        
        /*
         * normal midpoint split
         */
        nearestneighbor_kdtreesplit(kdt, i1, i2, d, s, &i3, _state);
    }
    else
    {
        
        /*
         * sliding midpoint
         */
        if( cntless==0 )
        {
            
            /*
             * 1. move split to MinV,
             * 2. place one point to the left bin (move to I1),
             *    others - to the right bin
             */
            s = minv;
            if( minidx!=i1 )
            {
                for(i=0; i<=2*nx+ny-1; i++)
                {
                    v = kdt->xy.ptr.pp_double[minidx][i];
                    kdt->xy.ptr.pp_double[minidx][i] = kdt->xy.ptr.pp_double[i1][i];
                    kdt->xy.ptr.pp_double[i1][i] = v;
                }
                j = kdt->tags.ptr.p_int[minidx];
                kdt->tags.ptr.p_int[minidx] = kdt->tags.ptr.p_int[i1];
                kdt->tags.ptr.p_int[i1] = j;
            }
            i3 = i1+1;
        }
        else
        {
            
            /*
             * 1. move split to MaxV,
             * 2. place one point to the right bin (move to I2-1),
             *    others - to the left bin
             */
            s = maxv;
            if( maxidx!=i2-1 )
            {
                for(i=0; i<=2*nx+ny-1; i++)
                {
                    v = kdt->xy.ptr.pp_double[maxidx][i];
                    kdt->xy.ptr.pp_double[maxidx][i] = kdt->xy.ptr.pp_double[i2-1][i];
                    kdt->xy.ptr.pp_double[i2-1][i] = v;
                }
                j = kdt->tags.ptr.p_int[maxidx];
                kdt->tags.ptr.p_int[maxidx] = kdt->tags.ptr.p_int[i2-1];
                kdt->tags.ptr.p_int[i2-1] = j;
            }
            i3 = i2-1;
        }
    }
    
    /*
     * Generate 'split' node
     */
    kdt->nodes.ptr.p_int[*nodesoffs+0] = 0;
    kdt->nodes.ptr.p_int[*nodesoffs+1] = d;
    kdt->nodes.ptr.p_int[*nodesoffs+2] = *splitsoffs;
    kdt->splits.ptr.p_double[*splitsoffs+0] = s;
    oldoffs = *nodesoffs;
    *nodesoffs = *nodesoffs+nearestneighbor_splitnodesize;
    *splitsoffs = *splitsoffs+1;
    
    /*
     * Recursive generation:
     * * update CurBox
     * * call subroutine
     * * restore CurBox
     */
    kdt->nodes.ptr.p_int[oldoffs+3] = *nodesoffs;
    v = kdt->innerbuf.curboxmax.ptr.p_double[d];
    kdt->innerbuf.curboxmax.ptr.p_double[d] = s;
    nearestneighbor_kdtreegeneratetreerec(kdt, nodesoffs, splitsoffs, i1, i3, maxleafsize, _state);
    kdt->innerbuf.curboxmax.ptr.p_double[d] = v;
    kdt->nodes.ptr.p_int[oldoffs+4] = *nodesoffs;
    v = kdt->innerbuf.curboxmin.ptr.p_double[d];
    kdt->innerbuf.curboxmin.ptr.p_double[d] = s;
    nearestneighbor_kdtreegeneratetreerec(kdt, nodesoffs, splitsoffs, i3, i2, maxleafsize, _state);
    kdt->innerbuf.curboxmin.ptr.p_double[d] = v;
    
    /*
     * Zero-fill unused portions of the node (avoid false warnings by Valgrind
     * about attempt to serialize uninitialized values)
     */
    ae_assert(nearestneighbor_splitnodesize==6, "KDTreeGenerateTreeRec: node size has unexpectedly changed", _state);
    kdt->nodes.ptr.p_int[oldoffs+5] = 0;
}


/*************************************************************************
Recursive subroutine for NN queries.

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
static void nearestneighbor_kdtreequerynnrec(const kdtree* kdt,
     kdtreerequestbuffer* buf,
     ae_int_t offs,
     ae_state *_state)
{
    double ptdist;
    ae_int_t i;
    ae_int_t j;
    ae_int_t nx;
    ae_int_t i1;
    ae_int_t i2;
    ae_int_t d;
    double s;
    double v;
    double t1;
    ae_int_t childbestoffs;
    ae_int_t childworstoffs;
    ae_int_t childoffs;
    double prevdist;
    ae_bool todive;
    ae_bool bestisleft;
    ae_bool updatemin;


    ae_assert(kdt->n>0, "KDTreeQueryNNRec: internal error", _state);
    
    /*
     * Leaf node.
     * Process points.
     */
    if( kdt->nodes.ptr.p_int[offs]>0 )
    {
        i1 = kdt->nodes.ptr.p_int[offs+1];
        i2 = i1+kdt->nodes.ptr.p_int[offs];
        for(i=i1; i<=i2-1; i++)
        {
            
            /*
             * Calculate distance
             */
            ptdist = (double)(0);
            nx = kdt->nx;
            if( kdt->normtype==0 )
            {
                for(j=0; j<=nx-1; j++)
                {
                    ptdist = ae_maxreal(ptdist, ae_fabs(kdt->xy.ptr.pp_double[i][j]-buf->x.ptr.p_double[j], _state), _state);
                }
            }
            if( kdt->normtype==1 )
            {
                for(j=0; j<=nx-1; j++)
                {
                    ptdist = ptdist+ae_fabs(kdt->xy.ptr.pp_double[i][j]-buf->x.ptr.p_double[j], _state);
                }
            }
            if( kdt->normtype==2 )
            {
                for(j=0; j<=nx-1; j++)
                {
                    ptdist = ptdist+ae_sqr(kdt->xy.ptr.pp_double[i][j]-buf->x.ptr.p_double[j], _state);
                }
            }
            
            /*
             * Skip points with zero distance if self-matches are turned off
             */
            if( ptdist==(double)0&&!buf->selfmatch )
            {
                continue;
            }
            
            /*
             * We CAN'T process point if R-criterion isn't satisfied,
             * i.e. (RNeeded<>0) AND (PtDist>R).
             */
            if( buf->rneeded==(double)0||ptdist<=buf->rneeded )
            {
                
                /*
                 * R-criterion is satisfied, we must either:
                 * * replace worst point, if (KNeeded<>0) AND (KCur=KNeeded)
                 *   (or skip, if worst point is better)
                 * * add point without replacement otherwise
                 */
                if( buf->kcur<buf->kneeded||buf->kneeded==0 )
                {
                    
                    /*
                     * add current point to heap without replacement
                     */
                    tagheappushi(&buf->r, &buf->idx, &buf->kcur, ptdist, i, _state);
                }
                else
                {
                    
                    /*
                     * New points are added or not, depending on their distance.
                     * If added, they replace element at the top of the heap
                     */
                    if( ptdist<buf->r.ptr.p_double[0] )
                    {
                        if( buf->kneeded==1 )
                        {
                            buf->idx.ptr.p_int[0] = i;
                            buf->r.ptr.p_double[0] = ptdist;
                        }
                        else
                        {
                            tagheapreplacetopi(&buf->r, &buf->idx, buf->kneeded, ptdist, i, _state);
                        }
                    }
                }
            }
        }
        return;
    }
    
    /*
     * Simple split
     */
    if( kdt->nodes.ptr.p_int[offs]==0 )
    {
        
        /*
         * Load:
         * * D  dimension to split
         * * S  split position
         */
        d = kdt->nodes.ptr.p_int[offs+1];
        s = kdt->splits.ptr.p_double[kdt->nodes.ptr.p_int[offs+2]];
        
        /*
         * Calculate:
         * * ChildBestOffs      child box with best chances
         * * ChildWorstOffs     child box with worst chances
         */
        if( buf->x.ptr.p_double[d]<=s )
        {
            childbestoffs = kdt->nodes.ptr.p_int[offs+3];
            childworstoffs = kdt->nodes.ptr.p_int[offs+4];
            bestisleft = ae_true;
        }
        else
        {
            childbestoffs = kdt->nodes.ptr.p_int[offs+4];
            childworstoffs = kdt->nodes.ptr.p_int[offs+3];
            bestisleft = ae_false;
        }
        
        /*
         * Navigate through childs
         */
        for(i=0; i<=1; i++)
        {
            
            /*
             * Select child to process:
             * * ChildOffs      current child offset in Nodes[]
             * * UpdateMin      whether minimum or maximum value
             *                  of bounding box is changed on update
             */
            if( i==0 )
            {
                childoffs = childbestoffs;
                updatemin = !bestisleft;
            }
            else
            {
                updatemin = bestisleft;
                childoffs = childworstoffs;
            }
            
            /*
             * Update bounding box and current distance
             */
            if( updatemin )
            {
                prevdist = buf->curdist;
                t1 = buf->x.ptr.p_double[d];
                v = buf->curboxmin.ptr.p_double[d];
                if( t1<=s )
                {
                    if( kdt->normtype==0 )
                    {
                        buf->curdist = ae_maxreal(buf->curdist, s-t1, _state);
                    }
                    if( kdt->normtype==1 )
                    {
                        buf->curdist = buf->curdist-ae_maxreal(v-t1, (double)(0), _state)+s-t1;
                    }
                    if( kdt->normtype==2 )
                    {
                        buf->curdist = buf->curdist-ae_sqr(ae_maxreal(v-t1, (double)(0), _state), _state)+ae_sqr(s-t1, _state);
                    }
                }
                buf->curboxmin.ptr.p_double[d] = s;
            }
            else
            {
                prevdist = buf->curdist;
                t1 = buf->x.ptr.p_double[d];
                v = buf->curboxmax.ptr.p_double[d];
                if( t1>=s )
                {
                    if( kdt->normtype==0 )
                    {
                        buf->curdist = ae_maxreal(buf->curdist, t1-s, _state);
                    }
                    if( kdt->normtype==1 )
                    {
                        buf->curdist = buf->curdist-ae_maxreal(t1-v, (double)(0), _state)+t1-s;
                    }
                    if( kdt->normtype==2 )
                    {
                        buf->curdist = buf->curdist-ae_sqr(ae_maxreal(t1-v, (double)(0), _state), _state)+ae_sqr(t1-s, _state);
                    }
                }
                buf->curboxmax.ptr.p_double[d] = s;
            }
            
            /*
             * Decide: to dive into cell or not to dive
             */
            if( buf->rneeded!=(double)0&&buf->curdist>buf->rneeded )
            {
                todive = ae_false;
            }
            else
            {
                if( buf->kcur<buf->kneeded||buf->kneeded==0 )
                {
                    
                    /*
                     * KCur<KNeeded (i.e. not all points are found)
                     */
                    todive = ae_true;
                }
                else
                {
                    
                    /*
                     * KCur=KNeeded, decide to dive or not to dive
                     * using point position relative to bounding box.
                     */
                    todive = buf->curdist<=buf->r.ptr.p_double[0]*buf->approxf;
                }
            }
            if( todive )
            {
                nearestneighbor_kdtreequerynnrec(kdt, buf, childoffs, _state);
            }
            
            /*
             * Restore bounding box and distance
             */
            if( updatemin )
            {
                buf->curboxmin.ptr.p_double[d] = v;
            }
            else
            {
                buf->curboxmax.ptr.p_double[d] = v;
            }
            buf->curdist = prevdist;
        }
        return;
    }
}


/*************************************************************************
Recursive subroutine for box queries.

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
static void nearestneighbor_kdtreequeryboxrec(const kdtree* kdt,
     kdtreerequestbuffer* buf,
     ae_int_t offs,
     ae_state *_state)
{
    ae_bool inbox;
    ae_int_t nx;
    ae_int_t i1;
    ae_int_t i2;
    ae_int_t i;
    ae_int_t j;
    ae_int_t d;
    double s;
    double v;


    ae_assert(kdt->n>0, "KDTreeQueryBoxRec: internal error", _state);
    nx = kdt->nx;
    
    /*
     * Check that intersection of query box with bounding box is non-empty.
     * This check is performed once for Offs=0 (tree root).
     */
    if( offs==0 )
    {
        for(j=0; j<=nx-1; j++)
        {
            if( buf->boxmin.ptr.p_double[j]>buf->curboxmax.ptr.p_double[j] )
            {
                return;
            }
            if( buf->boxmax.ptr.p_double[j]<buf->curboxmin.ptr.p_double[j] )
            {
                return;
            }
        }
    }
    
    /*
     * Leaf node.
     * Process points.
     */
    if( kdt->nodes.ptr.p_int[offs]>0 )
    {
        i1 = kdt->nodes.ptr.p_int[offs+1];
        i2 = i1+kdt->nodes.ptr.p_int[offs];
        for(i=i1; i<=i2-1; i++)
        {
            
            /*
             * Check whether point is in box or not
             */
            inbox = ae_true;
            for(j=0; j<=nx-1; j++)
            {
                inbox = inbox&&kdt->xy.ptr.pp_double[i][j]>=buf->boxmin.ptr.p_double[j];
                inbox = inbox&&kdt->xy.ptr.pp_double[i][j]<=buf->boxmax.ptr.p_double[j];
            }
            if( !inbox )
            {
                continue;
            }
            
            /*
             * Add point to unordered list
             */
            buf->r.ptr.p_double[buf->kcur] = 0.0;
            buf->idx.ptr.p_int[buf->kcur] = i;
            buf->kcur = buf->kcur+1;
        }
        return;
    }
    
    /*
     * Simple split
     */
    if( kdt->nodes.ptr.p_int[offs]==0 )
    {
        
        /*
         * Load:
         * * D  dimension to split
         * * S  split position
         */
        d = kdt->nodes.ptr.p_int[offs+1];
        s = kdt->splits.ptr.p_double[kdt->nodes.ptr.p_int[offs+2]];
        
        /*
         * Check lower split (S is upper bound of new bounding box)
         */
        if( s>=buf->boxmin.ptr.p_double[d] )
        {
            v = buf->curboxmax.ptr.p_double[d];
            buf->curboxmax.ptr.p_double[d] = s;
            nearestneighbor_kdtreequeryboxrec(kdt, buf, kdt->nodes.ptr.p_int[offs+3], _state);
            buf->curboxmax.ptr.p_double[d] = v;
        }
        
        /*
         * Check upper split (S is lower bound of new bounding box)
         */
        if( s<=buf->boxmax.ptr.p_double[d] )
        {
            v = buf->curboxmin.ptr.p_double[d];
            buf->curboxmin.ptr.p_double[d] = s;
            nearestneighbor_kdtreequeryboxrec(kdt, buf, kdt->nodes.ptr.p_int[offs+4], _state);
            buf->curboxmin.ptr.p_double[d] = v;
        }
        return;
    }
}


/*************************************************************************
Copies X[] to Buf.X[]
Loads distance from X[] to bounding box.
Initializes Buf.CurBox[].

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
static void nearestneighbor_kdtreeinitbox(const kdtree* kdt,
     /* Real    */ const ae_vector* x,
     kdtreerequestbuffer* buf,
     ae_state *_state)
{
    ae_int_t i;
    double vx;
    double vmin;
    double vmax;


    ae_assert(kdt->n>0, "KDTreeInitBox: internal error", _state);
    
    /*
     * calculate distance from point to current bounding box
     */
    buf->curdist = (double)(0);
    if( kdt->normtype==0 )
    {
        for(i=0; i<=kdt->nx-1; i++)
        {
            vx = x->ptr.p_double[i];
            vmin = kdt->boxmin.ptr.p_double[i];
            vmax = kdt->boxmax.ptr.p_double[i];
            buf->x.ptr.p_double[i] = vx;
            buf->curboxmin.ptr.p_double[i] = vmin;
            buf->curboxmax.ptr.p_double[i] = vmax;
            if( vx<vmin )
            {
                buf->curdist = ae_maxreal(buf->curdist, vmin-vx, _state);
            }
            else
            {
                if( vx>vmax )
                {
                    buf->curdist = ae_maxreal(buf->curdist, vx-vmax, _state);
                }
            }
        }
    }
    if( kdt->normtype==1 )
    {
        for(i=0; i<=kdt->nx-1; i++)
        {
            vx = x->ptr.p_double[i];
            vmin = kdt->boxmin.ptr.p_double[i];
            vmax = kdt->boxmax.ptr.p_double[i];
            buf->x.ptr.p_double[i] = vx;
            buf->curboxmin.ptr.p_double[i] = vmin;
            buf->curboxmax.ptr.p_double[i] = vmax;
            if( vx<vmin )
            {
                buf->curdist = buf->curdist+vmin-vx;
            }
            else
            {
                if( vx>vmax )
                {
                    buf->curdist = buf->curdist+vx-vmax;
                }
            }
        }
    }
    if( kdt->normtype==2 )
    {
        for(i=0; i<=kdt->nx-1; i++)
        {
            vx = x->ptr.p_double[i];
            vmin = kdt->boxmin.ptr.p_double[i];
            vmax = kdt->boxmax.ptr.p_double[i];
            buf->x.ptr.p_double[i] = vx;
            buf->curboxmin.ptr.p_double[i] = vmin;
            buf->curboxmax.ptr.p_double[i] = vmax;
            if( vx<vmin )
            {
                buf->curdist = buf->curdist+ae_sqr(vmin-vx, _state);
            }
            else
            {
                if( vx>vmax )
                {
                    buf->curdist = buf->curdist+ae_sqr(vx-vmax, _state);
                }
            }
        }
    }
}


/*************************************************************************
This function allocates all dataset-independend array  fields  of  KDTree,
i.e.  such  array  fields  that  their dimensions do not depend on dataset
size.

This function do not sets KDT.NX or KDT.NY - it just allocates arrays

  -- ALGLIB --
     Copyright 14.03.2011 by Bochkanov Sergey
*************************************************************************/
static void nearestneighbor_kdtreeallocdatasetindependent(kdtree* kdt,
     ae_int_t nx,
     ae_int_t ny,
     ae_state *_state)
{


    ae_assert(kdt->n>0, "KDTreeAllocDatasetIndependent: internal error", _state);
    ae_vector_set_length(&kdt->boxmin, nx, _state);
    ae_vector_set_length(&kdt->boxmax, nx, _state);
}


/*************************************************************************
This function allocates all dataset-dependent array fields of KDTree, i.e.
such array fields that their dimensions depend on dataset size.

This function do not sets KDT.N, KDT.NX or KDT.NY -
it just allocates arrays.

  -- ALGLIB --
     Copyright 14.03.2011 by Bochkanov Sergey
*************************************************************************/
static void nearestneighbor_kdtreeallocdatasetdependent(kdtree* kdt,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t ny,
     ae_state *_state)
{


    ae_assert(n>0, "KDTreeAllocDatasetDependent: internal error", _state);
    ae_matrix_set_length(&kdt->xy, n, 2*nx+ny, _state);
    ae_vector_set_length(&kdt->tags, n, _state);
    ae_vector_set_length(&kdt->nodes, nearestneighbor_splitnodesize*2*n, _state);
    ae_vector_set_length(&kdt->splits, 2*n, _state);
}


/*************************************************************************
This  function   checks  consistency  of  request  buffer  structure  with
dimensions of kd-tree object.

  -- ALGLIB --
     Copyright 02.04.2016 by Bochkanov Sergey
*************************************************************************/
static void nearestneighbor_checkrequestbufferconsistency(const kdtree* kdt,
     const kdtreerequestbuffer* buf,
     ae_state *_state)
{


    ae_assert(buf->x.cnt>=kdt->nx, "KDTree: dimensions of kdtreerequestbuffer are inconsistent with kdtree structure", _state);
    ae_assert(buf->idx.cnt>=kdt->n, "KDTree: dimensions of kdtreerequestbuffer are inconsistent with kdtree structure", _state);
    ae_assert(buf->r.cnt>=kdt->n, "KDTree: dimensions of kdtreerequestbuffer are inconsistent with kdtree structure", _state);
    ae_assert(buf->buf.cnt>=ae_maxint(kdt->n, kdt->nx, _state), "KDTree: dimensions of kdtreerequestbuffer are inconsistent with kdtree structure", _state);
    ae_assert(buf->curboxmin.cnt>=kdt->nx, "KDTree: dimensions of kdtreerequestbuffer are inconsistent with kdtree structure", _state);
    ae_assert(buf->curboxmax.cnt>=kdt->nx, "KDTree: dimensions of kdtreerequestbuffer are inconsistent with kdtree structure", _state);
}


void _kdtreerequestbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    kdtreerequestbuffer *p = (kdtreerequestbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->boxmin, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->boxmax, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->idx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->r, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->buf, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->curboxmin, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->curboxmax, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xqc, 0, DT_REAL, _state, make_automatic);
}


void _kdtreerequestbuffer_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    kdtreerequestbuffer       *dst = (kdtreerequestbuffer*)_dst;
    const kdtreerequestbuffer *src = (const kdtreerequestbuffer*)_src;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->boxmin, &src->boxmin, _state, make_automatic);
    ae_vector_init_copy(&dst->boxmax, &src->boxmax, _state, make_automatic);
    dst->kneeded = src->kneeded;
    dst->rneeded = src->rneeded;
    dst->selfmatch = src->selfmatch;
    dst->approxf = src->approxf;
    dst->kcur = src->kcur;
    ae_vector_init_copy(&dst->idx, &src->idx, _state, make_automatic);
    ae_vector_init_copy(&dst->r, &src->r, _state, make_automatic);
    ae_vector_init_copy(&dst->buf, &src->buf, _state, make_automatic);
    ae_vector_init_copy(&dst->curboxmin, &src->curboxmin, _state, make_automatic);
    ae_vector_init_copy(&dst->curboxmax, &src->curboxmax, _state, make_automatic);
    dst->curdist = src->curdist;
    ae_vector_init_copy(&dst->xqc, &src->xqc, _state, make_automatic);
}


void _kdtreerequestbuffer_clear(void* _p)
{
    kdtreerequestbuffer *p = (kdtreerequestbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->boxmin);
    ae_vector_clear(&p->boxmax);
    ae_vector_clear(&p->idx);
    ae_vector_clear(&p->r);
    ae_vector_clear(&p->buf);
    ae_vector_clear(&p->curboxmin);
    ae_vector_clear(&p->curboxmax);
    ae_vector_clear(&p->xqc);
}


void _kdtreerequestbuffer_destroy(void* _p)
{
    kdtreerequestbuffer *p = (kdtreerequestbuffer*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->boxmin);
    ae_vector_destroy(&p->boxmax);
    ae_vector_destroy(&p->idx);
    ae_vector_destroy(&p->r);
    ae_vector_destroy(&p->buf);
    ae_vector_destroy(&p->curboxmin);
    ae_vector_destroy(&p->curboxmax);
    ae_vector_destroy(&p->xqc);
}


void _kdtree_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    kdtree *p = (kdtree*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_init(&p->xy, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tags, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->boxmin, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->boxmax, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nodes, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->splits, 0, DT_REAL, _state, make_automatic);
    _kdtreerequestbuffer_init(&p->innerbuf, _state, make_automatic);
}


void _kdtree_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    kdtree       *dst = (kdtree*)_dst;
    const kdtree *src = (const kdtree*)_src;
    dst->n = src->n;
    dst->nx = src->nx;
    dst->ny = src->ny;
    dst->normtype = src->normtype;
    ae_matrix_init_copy(&dst->xy, &src->xy, _state, make_automatic);
    ae_vector_init_copy(&dst->tags, &src->tags, _state, make_automatic);
    ae_vector_init_copy(&dst->boxmin, &src->boxmin, _state, make_automatic);
    ae_vector_init_copy(&dst->boxmax, &src->boxmax, _state, make_automatic);
    ae_vector_init_copy(&dst->nodes, &src->nodes, _state, make_automatic);
    ae_vector_init_copy(&dst->splits, &src->splits, _state, make_automatic);
    _kdtreerequestbuffer_init_copy(&dst->innerbuf, &src->innerbuf, _state, make_automatic);
    dst->debugcounter = src->debugcounter;
}


void _kdtree_clear(void* _p)
{
    kdtree *p = (kdtree*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_clear(&p->xy);
    ae_vector_clear(&p->tags);
    ae_vector_clear(&p->boxmin);
    ae_vector_clear(&p->boxmax);
    ae_vector_clear(&p->nodes);
    ae_vector_clear(&p->splits);
    _kdtreerequestbuffer_clear(&p->innerbuf);
}


void _kdtree_destroy(void* _p)
{
    kdtree *p = (kdtree*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_destroy(&p->xy);
    ae_vector_destroy(&p->tags);
    ae_vector_destroy(&p->boxmin);
    ae_vector_destroy(&p->boxmax);
    ae_vector_destroy(&p->nodes);
    ae_vector_destroy(&p->splits);
    _kdtreerequestbuffer_destroy(&p->innerbuf);
}


/*$ End $*/
