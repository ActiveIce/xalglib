import xalglib
import math
import cmath
import sys
import random
import traceback

def doc_print_test(v, t, typecode, _threshold = 0):
    if typecode=="bool":
        if (not xalglib.is_bool(v)) or (not xalglib.is_bool(t)):
            return False
        return v==t
    if typecode=="int":
        if (not xalglib.is_int(v)) or (not xalglib.is_int(t)):
            return False
        return v==t
    if typecode=="real":
        if (not xalglib.is_real(v)) or (not xalglib.is_real(t)):
            return False
        if _threshold>=0:
            s = 1.0
        else:
            s = abs(t)
        threshold = abs(_threshold)
        return abs(v-t)/s<=threshold    
    if typecode=="complex":
        if (not xalglib.is_complex(v)) or (not xalglib.is_complex(t)):
            return False
        if _threshold>=0:
            s = 1.0
        else:
            s = abs(t)
        threshold = abs(_threshold)
        return abs(v-t)/s<=threshold
    
    if typecode=="bool_vector":
        if (not xalglib.is_bool_vector(t)) or (not xalglib.is_bool_vector(v)):
            return False
        if xalglib.safe_len("",v)!=xalglib.safe_len("",t):
            return False
        cnt = xalglib.safe_len("",t)
        for idx in range(cnt):
            if v[idx]!=t[idx]:
                return False
        return True
    
    if typecode=="bool_matrix":
        if (not xalglib.is_bool_matrix(t)) or (not xalglib.is_bool_matrix(v)):
            return False
        if xalglib.safe_rows("",v)!=xalglib.safe_rows("",t):
            return False
        if xalglib.safe_cols("",v)!=xalglib.safe_cols("",t):
            return False
        cols = xalglib.safe_cols("",t)
        rows = xalglib.safe_rows("",t)
        for idx0 in range(rows):
            for idx1 in range(cols):
                if v[idx0][idx1]!=t[idx0][idx1]:
                    return False
        return True
    
    if typecode=="int_vector":
        if (not xalglib.is_int_vector(t)) or (not xalglib.is_int_vector(v)):
            return False
        if xalglib.safe_len("",v)!=xalglib.safe_len("",t):
            return False
        cnt = xalglib.safe_len("",t)
        for idx in range(cnt):
            if v[idx]!=t[idx]:
                return False
        return True
    
    if typecode=="int_matrix":
        if (not xalglib.is_int_matrix(t)) or (not xalglib.is_int_matrix(v)):
            return False
        if xalglib.safe_rows("",v)!=xalglib.safe_rows("",t):
            return False
        if xalglib.safe_cols("",v)!=xalglib.safe_cols("",t):
            return False
        cols = xalglib.safe_cols("",t)
        rows = xalglib.safe_rows("",t)
        for idx0 in range(rows):
            for idx1 in range(cols):
                if v[idx0][idx1]!=t[idx0][idx1]:
                    return False
        return True
            
    if typecode=="real_vector":
        if (not xalglib.is_real_vector(t)) or (not xalglib.is_real_vector(v)):
            return False
        if xalglib.safe_len("",v)!=xalglib.safe_len("",t):
            return False
        cnt = xalglib.safe_len("",t)
        for idx in range(cnt):
            if _threshold>=0:
                s = 1.0
            else:
                s = abs(t[idx])
            threshold = abs(_threshold)
            if abs(v[idx]-t[idx])/s>threshold:
                return False
        return True
    
    if typecode=="real_matrix":
        if (not xalglib.is_real_matrix(t)) or (not xalglib.is_real_matrix(v)):
            return False
        if xalglib.safe_rows("",v)!=xalglib.safe_rows("",t):
            return False
        if xalglib.safe_cols("",v)!=xalglib.safe_cols("",t):
            return False
        cols = xalglib.safe_cols("",t)
        rows = xalglib.safe_rows("",t)
        for idx0 in range(rows):
            for idx1 in range(cols):
                if _threshold>=0:
                    s = 1.0
                else:
                    s = abs(t[idx0][idx1])
                threshold = abs(_threshold)
                if abs(v[idx0][idx1]-t[idx0][idx1])/s>threshold:
                    return False
        return True
            
    if typecode=="complex_vector":
        if (not xalglib.is_complex_vector(t)) or (not xalglib.is_complex_vector(v)):
            return False
        if xalglib.safe_len("",v)!=xalglib.safe_len("",t):
            return False
        cnt = xalglib.safe_len("",t)
        for idx in range(cnt):
            if _threshold>=0:
                s = 1.0
            else:
                s = abs(t[idx])
            threshold = abs(_threshold)
            if abs(v[idx]-t[idx])/s>threshold:
                return False
        return True
    
    if typecode=="complex_matrix":
        if (not xalglib.is_complex_matrix(t)) or (not xalglib.is_complex_matrix(v)):
            return False
        if xalglib.safe_rows("",v)!=xalglib.safe_rows("",t):
            return False
        if xalglib.safe_cols("",v)!=xalglib.safe_cols("",t):
            return False
        cols = xalglib.safe_cols("",t)
        rows = xalglib.safe_rows("",t)
        for idx0 in range(rows):
            for idx1 in range(cols):
                if _threshold>=0:
                    s = 1.0
                else:
                    s = abs(t[idx0][idx1])
                threshold = abs(_threshold)
                if abs(v[idx0][idx1]-t[idx0][idx1])/s>threshold:
                    return False
        return True
    raise ValueError("doc_print_test: unknown test value type")

def spoil_vec_by_adding_element(x):
    x.append(0)
    return

def spoil_vec_by_deleting_element(x):
    if len(x)!=0:
        x.pop()
    return

def spoil_mat_by_adding_row(x):
    cols = xalglib.safe_cols("internal error",x)
    if cols!=0:
        x.append([0]*cols)
    return

def spoil_mat_by_deleting_row(x):
    if len(x)!=0:
        x.pop()
    return

def spoil_mat_by_adding_col(x):
    for row in x:
        row.append(0)
    return

def spoil_mat_by_deleting_col(x):
    for row in x:
        row.pop()
    return

def spoil_vec_by_nan(x):
    lenx = xalglib.safe_len("internal error",x)
    if lenx!=0:
        i = random.randint(0,lenx-1)
        x[i] = float("nan")
    return

def spoil_vec_by_posinf(x):
    lenx = xalglib.safe_len("internal error",x)
    if lenx!=0:
        i = random.randint(0,lenx-1)
        x[i] = float("+inf")
    return

def spoil_vec_by_neginf(x):
    lenx = xalglib.safe_len("internal error",x)
    if lenx!=0:
        i = random.randint(0,lenx-1)
        x[i] = float("-inf")
    return

def spoil_mat_by_nan(x):
    rows = xalglib.safe_rows("internal error",x)
    cols = xalglib.safe_cols("internal error",x)
    if cols*rows!=0:
        x[random.randint(0,rows-1)][random.randint(0,cols-1)] = float("nan")
    return

def spoil_mat_by_posinf(x):
    rows = xalglib.safe_rows("internal error",x)
    cols = xalglib.safe_cols("internal error",x)
    if cols*rows!=0:
        x[random.randint(0,rows-1)][random.randint(0,cols-1)] = float("+inf")
    return

def spoil_mat_by_neginf(x):
    rows = xalglib.safe_rows("internal error",x)
    cols = xalglib.safe_cols("internal error",x)
    if cols*rows!=0:
        x[random.randint(0,rows-1)][random.randint(0,cols-1)] = float("-inf")
    return

def function1_func(x, param):
    #
    # this callback calculates f(x0,x1) = 100*(x0+3)^4 + (x1-3)^4
    #
    return 100*(x[0]+3)**4 + (x[1]-3)**4

def function1_grad(x, grad, param):
    #
    # this callback calculates f(x0,x1) = 100*(x0+3)^4 + (x1-3)^4
    # and its derivatives df/d0 and df/dx1
    #
    func = 100*(x[0]+3)**4 + (x[1]-3)**4
    grad[0] = 400*(x[0]+3)**3
    grad[1] = 4*(x[1]-3)**3
    return func

def function1_hess(x, grad, hess, param):
    #
    # this callback calculates f(x0,x1) = 100*(x0+3)^4 + (x1-3)^4
    # its derivatives df/d0 and df/dx1
    # and its Hessian.
    #
    func = 100*(x[0]+3)**4 + (x[1]-3)**4
    grad[0] = 400*(x[0]+3)**3
    grad[1] = 4*(x[1]-3)**3
    hess[0][0] = 1200*(x[0]+3)**2
    hess[0][1] = 0
    hess[1][0] = 0
    hess[1][1] = 12*(x[1]-3)**2
    return func

def function1_fvec(x, fi, param):
    #
    # this callback calculates
    # f0(x0,x1) = 100*(x0+3)^4,
    # f1(x0,x1) = (x1-3)^4
    #
    fi[0] = 10*(x[0]+3)**2
    fi[1] = (x[1]-3)**2
    return

def function1_jac(x, fi, jac, param):
    #
    # this callback calculates
    # f0(x0,x1) = 100*(x0+3)^4,
    # f1(x0,x1) = (x1-3)^4
    # and Jacobian matrix J = [dfi/dxj]
    #
    fi[0] = 10*(x[0]+3)**2
    fi[1] = (x[1]-3)**2
    jac[0][0] = 20*(x[0]+3)
    jac[0][1] = 0
    jac[1][0] = 0
    jac[1][1] = 2*(x[1]-3)
    return

def function2_func(x, param):
    #
    # this callback calculates f(x0,x1) = (x0^2+1)^2 + (x1-1)^2
    #
    return (x[0]*x[0]+1)**2 + (x[1]-1)**2

def function2_grad(x, grad, param):
    #
    # this callback calculates f(x0,x1) = (x0^2+1)^2 + (x1-1)^2
    # and its derivatives df/d0 and df/dx1
    #
    func = (x[0]*x[0]+1)**2 + (x[1]-1)**2
    grad[0] = 4*(x[0]*x[0]+1)*x[0]
    grad[1] = 2*(x[1]-1)
    return func

def function2_hess(x, grad, hess, param):
    #
    # this callback calculates f(x0,x1) = (x0^2+1)^2 + (x1-1)^2
    # its gradient and Hessian
    #
    func = (x[0]*x[0]+1)**2 + (x[1]-1)**2
    grad[0] = 4*(x[0]*x[0]+1)*x[0]
    grad[1] = 2*(x[1]-1)
    hess[0][0] = 12*x[0]*x[0]+4
    hess[0][1] = 0
    hess[1][0] = 0
    hess[1][1] = 2
    return func

def function2_fvec(x, fi, param):
    #
    # this callback calculates
    # f0(x0,x1) = x0^2+1
    # f1(x0,x1) = x1-1
    #
    fi[0] = x[0]*x[0]+1
    fi[1] = x[1]-1
    return

def function2_jac(x, fi, jac, param):
    #
    # this callback calculates
    # f0(x0,x1) = x0^2+1
    # f1(x0,x1) = x1-1
    # and Jacobian matrix J = [dfi/dxj]
    #
    fi[0] = x[0]*x[0]+1
    fi[1] = x[1]-1
    jac[0][0] = 2*x[0]
    jac[0][1] = 0
    jac[1][0] = 0
    jac[1][1] = 1
    return

def nlcfunc1_jac(x, fi, jac, param):
    #
    # this callback calculates
    #
    #     f0(x0,x1) = -x0+x1
    #     f1(x0,x1) = x0^2+x1^2-1
    #
    # and Jacobian matrix J = [dfi/dxj]
    #
    fi[0] = -x[0]+x[1]
    fi[1] = x[0]*x[0] + x[1]*x[1] - 1.0
    jac[0][0] = -1.0
    jac[0][1] = +1.0
    jac[1][0] = 2*x[0]
    jac[1][1] = 2*x[1]
    return

def nlcfunc2_jac(x, fi, jac, param):
    #
    # this callback calculates
    #
    #     f0(x0,x1,x2) = x0+x1
    #     f1(x0,x1,x2) = x2-exp(x0)
    #     f2(x0,x1,x2) = x0^2+x1^2-1
    #
    # and Jacobian matrix J = [dfi/dxj]
    #
    fi[0] = x[0]+x[1]
    fi[1] = x[2]-math.exp(x[0])
    fi[2] = x[0]*x[0] + x[1]*x[1] - 1.0
    jac[0][0] = 1.0
    jac[0][1] = 1.0
    jac[0][2] = 0.0
    jac[1][0] = -math.exp(x[0])
    jac[1][1] = 0.0
    jac[1][2] = 1.0
    jac[2][0] = 2*x[0]
    jac[2][1] = 2*x[1]
    jac[2][2] = 0.0
    return

def nlcfunc2_fvec(x, fi, param):
    #
    # this callback calculates
    #
    #     f0(x0,x1,x2) = x0+x1
    #     f1(x0,x1,x2) = x2-exp(x0)
    #     f2(x0,x1,x2) = x0^2+x1^2-1
    #
    fi[0] = x[0]+x[1]
    fi[1] = x[2]-math.exp(x[0])
    fi[2] = x[0]*x[0] + x[1]*x[1] - 1.0
    return

def nsfunc1_jac(x, fi, jac, param):
    #
    # this callback calculates
    #
    #     f0(x0,x1) = 2*|x0|+|x1|
    #
    # and Jacobian matrix J = [df0/dx0 df0/dx1]
    #
    fi[0] = 2*math.fabs(x[0])+math.fabs(x[1])
    jac[0][0] = 2*xalglib.xsign(x[0])
    jac[0][1] = xalglib.xsign(x[1])
    return

def nsfunc1_fvec(x, fi, param):
    #
    # this callback calculates
    #
    #     f0(x0,x1) = 2*|x0|+|x1|
    #
    fi[0] = 2*math.fabs(x[0])+math.fabs(x[1])
    return

def nsfunc2_jac(x, fi, jac, param):
    #
    # this callback calculates function vector
    #
    #     f0(x0,x1) = 2*|x0|+x1
    #     f1(x0,x1) = x0-1
    #     f2(x0,x1) = -x1-1
    #
    # and Jacobian matrix J
    #
    #         [ df0/dx0   df0/dx1 ]
    #     J = [ df1/dx0   df1/dx1 ]
    #         [ df2/dx0   df2/dx1 ]
    #
    fi[0] = 2*math.fabs(x[0])+math.fabs(x[1])
    jac[0][0] = 2*xalglib.xsign(x[0])
    jac[0][1] = xalglib.xsign(x[1])
    fi[1] = x[0]-1
    jac[1][0] = 1
    jac[1][1] = 0
    fi[2] = -x[1]-1
    jac[2][0] = 0
    jac[2][1] = -1
    return

def function_cx_1_func(c, x, param):
    #
    # this callback calculates f(c,x)=exp(-c0*sqr(x0))
    # where x is a position on X-axis and c is adjustable parameter
    #
    return math.exp(-c[0]*(x[0])**2)

def function_cx_1_grad(c, x, grad, param):
    #
    # this callback calculates f(c,x)=exp(-c0*sqr(x0)) and gradient G={df/dc[i]}
    # where x is a position on X-axis and c is adjustable parameter.
    # IMPORTANT: gradient is calculated with respect to C, not to X
    #
    func = math.exp(-c[0]*(x[0])**2)
    grad[0] = -((x[0])**2)*func
    return func

def function_cx_1_hess(c, x, grad, hess, param):
    #
    # this callback calculates f(c,x)=exp(-c0*sqr(x0)), gradient G={df/dc[i]} and Hessian H={d2f/(dc[i]*dc[j])}
    # where x is a position on X-axis and c is adjustable parameter.
    # IMPORTANT: gradient/Hessian are calculated with respect to C, not to X
    #
    func = math.exp(-c[0]*(x[0])**2)
    grad[0] = -((x[0])**2)*func
    hess[0][0] = ((x[0])**4)*func
    return func

def ode_function_1_diff(y, x, dy, param):
    #
    # this callback calculates f(y[],x)=-y[0]
    #
    dy[0] = -y[0]
    return

def int_function_1_func(x, xminusa, bminusx, param):
    #
    # this callback calculates f(x)=exp(x)
    #
    return math.exp(x)

def function_debt_func(c, x, param):
    #
    # this callback calculates f(c,x)=c[0]*(1+c[1]*(pow(x[0]-1999,c[2])-1))
    #
    return c[0]*(1+c[1]*((x[0]-1999)**c[2]-1))

def s1_grad(x, grad, param):
    #
    # this callback calculates f(x) = (1+x)^(-0.2) + (1-x)^(-0.3) + 1000*x and its gradient.
    #
    # function is trimmed when we calculate it near the singular points or outside of the [-1,+1].
    # Note that we do NOT calculate gradient in this case.
    #
    if (x[0]<=-0.999999999999) or (x[0]>=+0.999999999999):
        return 1.0E+300
    grad[0] = -0.2*math.pow(1+x[0],-1.2) +0.3*math.pow(1-x[0],-1.3) + 1000
    return math.pow(1+x[0],-0.2) + math.pow(1-x[0],-0.3) + 1000*x[0]

def multiobjective2_fvec(x, fi, param):
    #
    # this callback calculates the bi-objective target
    #
    #     f0(x0,x1) = x0^2 + (x1-1)^2
    #     f1(x0,x1) = (x0-1(^2 + x1^2
    #
    fi[0] = x[0]*x[0]+(x[1]-1)*(x[1]-1)
    fi[1] = (x[0]-1)*(x[0]-1)+x[1]*x[1]
    return
def multiobjective2_jac(x, fi, jac, param):
    #
    # this callback calculates the bi-objective target
    #
    #     f0(x0,x1) = x0^2 + (x1-1)^2
    #     f1(x0,x1) = (x0-1(^2 + x1^2
    #
    # and Jacobian matrix J = [dfi/dxj]
    #
    fi[0] = x[0]*x[0]+(x[1]-1)*(x[1]-1)
    fi[1] = (x[0]-1)*(x[0]-1)+x[1]*x[1]
    jac[0][0] = 2*x[0]
    jac[0][1] = 2*(x[1]-1)
    jac[1][0] = 2*(x[0]-1)
    jac[1][1] = 2*x[1]
    return
def multiobjective2constr_fvec(x, fi, param):
    #
    # this callback calculates the bi-objective target
    #
    #     f0(x0,x1) = x0^2 + (x1-1)^2
    #     f1(x0,x1) = (x0-1(^2 + x1^2
    #
    # nonlinear constraint function
    #
    #     f2(x0,x1) = x0^2 + x1^2
    #
    fi[0] = x[0]*x[0]+(x[1]-1)*(x[1]-1)
    fi[1] = (x[0]-1)*(x[0]-1)+x[1]*x[1]
    fi[2] = x[0]*x[0]+x[1]*x[1]
    return
def multiobjective2constr_jac(x, fi, jac, param):
    #
    # this callback calculates the bi-objective target
    #
    #     f0(x0,x1) = x0^2 + (x1-1)^2
    #     f1(x0,x1) = (x0-1(^2 + x1^2
    #
    # nonlinear constraint function
    #
    #     f2(x0,x1) = x0^2 + x1^2
    #
    # and Jacobian matrix J = [dfi/dxj]
    #
    fi[0] = x[0]*x[0]+(x[1]-1)*(x[1]-1)
    fi[1] = (x[0]-1)*(x[0]-1)+x[1]*x[1]
    fi[2] = x[0]*x[0]+x[1]*x[1]
    jac[0][0] = 2*x[0]
    jac[0][1] = 2*(x[1]-1)
    jac[1][0] = 2*(x[0]-1)
    jac[1][1] = 2*x[1]
    jac[2][0] = 2*x[0]
    jac[2][1] = 2*x[1]
    return

_TotalResult = True
sys.stdout.write("Python-ALGLIB communication tests. Please wait...\n")
try:
    #
    # TEST xdebug_t1
    #      Test initialization (out parameter) and update (shared parameter) for records
    #
    sys.stdout.write("0/162\n")
    _TestResult = True
    try:

        rec1 = xalglib.xdebuginitrecord1()
        _TestResult = _TestResult and doc_print_test(rec1.i, 1, "int")
        _TestResult = _TestResult and doc_print_test(rec1.c, 1+1j, "complex", 0.00005)
        _TestResult = _TestResult and doc_print_test(rec1.a, [2,3], "real_vector", 0.0005)

        xalglib.xdebugupdaterecord1(rec1)
        _TestResult = _TestResult and doc_print_test(rec1.i, 2, "int")
        _TestResult = _TestResult and doc_print_test(rec1.c, 3+4j, "complex", 0.00005)
        _TestResult = _TestResult and doc_print_test(rec1.a, [2,3,6], "real_vector", 0.0005)
    except (RuntimeError, ValueError):
        _TestResult = False
    except:
        raise
    if not _TestResult:
        sys.stdout.write("xdebug_t1                        FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST ablas_d_gemm
    #      Matrix multiplication (single-threaded)
    #
    _TestResult = True
    try:
        a = [[2,1],[1,3]]
        b = [[2,1],[0,1]]
        c = [[0,0],[0,0]]

        #
        # rmatrixgemm() function allows us to calculate matrix product C:=A*B or
        # to perform more general operation, C:=alpha*op1(A)*op2(B)+beta*C,
        # where A, B, C are rectangular matrices, op(X) can be X or X^T,
        # alpha and beta are scalars.
        #
        # This function:
        # * can apply transposition and/or multiplication by scalar to operands
        # * can use arbitrary part of matrices A/B (given by submatrix offset)
        # * can store result into arbitrary part of C
        # * for performance reasons requires C to be preallocated
        #
        # Parameters of this function are:
        # * M, N, K            -   sizes of op1(A) (which is MxK), op2(B) (which
        #                          is KxN) and C (which is MxN)
        # * Alpha              -   coefficient before A*B
        # * A, IA, JA          -   matrix A and offset of the submatrix
        # * OpTypeA            -   transformation type:
        #                          0 - no transformation
        #                          1 - transposition
        # * B, IB, JB          -   matrix B and offset of the submatrix
        # * OpTypeB            -   transformation type:
        #                          0 - no transformation
        #                          1 - transposition
        # * Beta               -   coefficient before C
        # * C, IC, JC          -   preallocated matrix C and offset of the submatrix
        #
        # Below we perform simple product C:=A*B (alpha=1, beta=0)
        #
        # IMPORTANT: this function works with preallocated C, which must be large
        #            enough to store multiplication result.
        #
        m = 2
        n = 2
        k = 2
        alpha = 1.0
        ia = 0
        ja = 0
        optypea = 0
        ib = 0
        jb = 0
        optypeb = 0
        beta = 0.0
        ic = 0
        jc = 0
        xalglib.rmatrixgemm(m, n, k, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc)
        _TestResult = _TestResult and doc_print_test(c, [[4,3],[2,4]], "real_matrix", 0.0001)

        #
        # Now we try to apply some simple transformation to operands: C:=A*B^T
        #
        optypeb = 1
        xalglib.rmatrixgemm(m, n, k, alpha, a, ia, ja, optypea, b, ib, jb, optypeb, beta, c, ic, jc)
        _TestResult = _TestResult and doc_print_test(c, [[5,1],[5,3]], "real_matrix", 0.0001)
    except (RuntimeError, ValueError):
        _TestResult = False
    except:
        raise
    if not _TestResult:
        sys.stdout.write("ablas_d_gemm                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST ablas_d_syrk
    #      Symmetric rank-K update (single-threaded)
    #
    _TestResult = True
    try:
        #
        # rmatrixsyrk() function allows us to calculate symmetric rank-K update
        # C := beta*C + alpha*A'*A, where C is square N*N matrix, A is square K*N
        # matrix, alpha and beta are scalars. It is also possible to update by
        # adding A*A' instead of A'*A.
        #
        # Parameters of this function are:
        # * N, K       -   matrix size
        # * Alpha      -   coefficient before A
        # * A, IA, JA  -   matrix and submatrix offsets
        # * OpTypeA    -   multiplication type:
        #                  * 0 - A*A^T is calculated
        #                  * 2 - A^T*A is calculated
        # * Beta       -   coefficient before C
        # * C, IC, JC  -   preallocated input/output matrix and submatrix offsets
        # * IsUpper    -   whether upper or lower triangle of C is updated;
        #                  this function updates only one half of C, leaving
        #                  other half unchanged (not referenced at all).
        #
        # Below we will show how to calculate simple product C:=A'*A
        #
        # NOTE: beta=0 and we do not use previous value of C, but still it
        #       MUST be preallocated.
        #
        n = 2
        k = 1
        alpha = 1.0
        ia = 0
        ja = 0
        optypea = 2
        beta = 0.0
        ic = 0
        jc = 0
        isupper = True
        a = [[1,2]]

        # preallocate space to store result
        c = [[0,0],[0,0]]

        # calculate product, store result into upper part of c
        xalglib.rmatrixsyrk(n, k, alpha, a, ia, ja, optypea, beta, c, ic, jc, isupper)

        # output result.
        # IMPORTANT: lower triangle of C was NOT updated!
        _TestResult = _TestResult and doc_print_test(c, [[1,2],[0,4]], "real_matrix", 0.0001)
    except (RuntimeError, ValueError):
        _TestResult = False
    except:
        raise
    if not _TestResult:
        sys.stdout.write("ablas_d_syrk                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST ablas_t_complex
    #      Basis test for complex matrix functions (correctness and presence of SMP support)
    #
    _TestResult = True
    try:

        # test cmatrixgemm()
        a = [[2j,1j],[1,3]]
        b = [[2,1],[0,1]]
        c = [[0,0],[0,0]]
        xalglib.cmatrixgemm(2, 2, 2, 1.0, a, 0, 0, 0, b, 0, 0, 0, 0.0, c, 0, 0)
        _TestResult = _TestResult and doc_print_test(c, [[4j,3j],[2,4]], "complex_matrix", 0.0001)
    except (RuntimeError, ValueError):
        _TestResult = False
    except:
        raise
    if not _TestResult:
        sys.stdout.write("ablas_t_complex                  FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST sparse_d_1
    #      Basic operations with sparse matrices
    #
    _TestResult = True
    for _spoil_scenario in range(-1,1):
        try:
            #
            # This example demonstrates creation/initialization of the sparse matrix
            # and matrix-vector multiplication.
            #
            # First, we have to create matrix and initialize it. Matrix is initially created
            # in the Hash-Table format, which allows convenient initialization. We can modify
            # Hash-Table matrix with sparseset() and sparseadd() functions.
            #
            # NOTE: Unlike CRS format, Hash-Table representation allows you to initialize
            # elements in the arbitrary order. You may see that we initialize a[0][0] first,
            # then move to the second row, and then move back to the first row.
            #
            s = xalglib.sparsecreate(2, 2)
            xalglib.sparseset(s, 0, 0, 2.0)
            xalglib.sparseset(s, 1, 1, 1.0)
            xalglib.sparseset(s, 0, 1, 1.0)

            xalglib.sparseadd(s, 1, 1, 4.0)

            #
            # Now S is equal to
            #   [ 2 1 ]
            #   [   5 ]
            # Lets check it by reading matrix contents with sparseget().
            # You may see that with sparseget() you may read both non-zero
            # and zero elements.
            #
            v = xalglib.sparseget(s, 0, 0)
            _TestResult = _TestResult and doc_print_test(v, 2.0000, "real", 0.005)
            v = xalglib.sparseget(s, 0, 1)
            _TestResult = _TestResult and doc_print_test(v, 1.0000, "real", 0.005)
            v = xalglib.sparseget(s, 1, 0)
            _TestResult = _TestResult and doc_print_test(v, 0.0000, "real", 0.005)
            v = xalglib.sparseget(s, 1, 1)
            _TestResult = _TestResult and doc_print_test(v, 5.0000, "real", 0.005)

            #
            # After successful creation we can use our matrix for linear operations.
            #
            # However, there is one more thing we MUST do before using S in linear
            # operations: we have to convert it from HashTable representation (used for
            # initialization and dynamic operations) to CRS format with sparseconverttocrs()
            # call. If you omit this call, ALGLIB will generate exception on the first
            # attempt to use S in linear operations. 
            #
            xalglib.sparseconverttocrs(s)

            #
            # Now S is in the CRS format and we are ready to do linear operations.
            # Lets calculate A*x for some x.
            #
            x = [1,-1]
            if _spoil_scenario==0:
                spoil_vec_by_deleting_element(x)
            y = []
            y = xalglib.sparsemv(s, x, y)
            _TestResult = _TestResult and doc_print_test(y, [1.000,-5.000], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("sparse_d_1                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST sparse_d_crs
    #      Advanced topic: creation in the CRS format.
    #
    _TestResult = True
    for _spoil_scenario in range(-1,2):
        try:
            #
            # This example demonstrates creation/initialization of the sparse matrix in the
            # CRS format.
            #
            # Hash-Table format used by default is very convenient (it allows easy
            # insertion of elements, automatic memory reallocation), but has
            # significant memory and performance overhead. Insertion of one element 
            # costs hundreds of CPU cycles, and memory consumption is several times
            # higher than that of CRS.
            #
            # When you work with really large matrices and when you can tell in 
            # advance how many elements EXACTLY you need, it can be beneficial to 
            # create matrix in the CRS format from the very beginning.
            #
            # If you want to create matrix in the CRS format, you should:
            # * use sparsecreatecrs() function
            # * know row sizes in advance (number of non-zero entries in the each row)
            # * initialize matrix with sparseset() - another function, sparseadd(), is not allowed
            # * initialize elements from left to right, from top to bottom, each
            #   element is initialized only once.
            #
            row_sizes = [2,2,2,1]
            if _spoil_scenario==0:
                spoil_vec_by_deleting_element(row_sizes)
            s = xalglib.sparsecreatecrs(4, 4, row_sizes)
            xalglib.sparseset(s, 0, 0, 2.0)
            xalglib.sparseset(s, 0, 1, 1.0)
            xalglib.sparseset(s, 1, 1, 4.0)
            xalglib.sparseset(s, 1, 2, 2.0)
            xalglib.sparseset(s, 2, 2, 3.0)
            xalglib.sparseset(s, 2, 3, 1.0)
            xalglib.sparseset(s, 3, 3, 9.0)

            #
            # Now S is equal to
            #   [ 2 1     ]
            #   [   4 2   ]
            #   [     3 1 ]
            #   [       9 ]
            #
            # We should point that we have initialized S elements from left to right,
            # from top to bottom. CRS representation does NOT allow you to do so in
            # the different order. Try to change order of the sparseset() calls above,
            # and you will see that your program generates exception.
            #
            # We can check it by reading matrix contents with sparseget().
            # However, you should remember that sparseget() is inefficient on
            # CRS matrices (it may have to pass through all elements of the row 
            # until it finds element you need).
            #
            v = xalglib.sparseget(s, 0, 0)
            _TestResult = _TestResult and doc_print_test(v, 2.0000, "real", 0.005)
            v = xalglib.sparseget(s, 2, 3)
            _TestResult = _TestResult and doc_print_test(v, 1.0000, "real", 0.005)

            # you may see that you can read zero elements (which are not stored) with sparseget()
            v = xalglib.sparseget(s, 3, 2)
            _TestResult = _TestResult and doc_print_test(v, 0.0000, "real", 0.005)

            #
            # After successful creation we can use our matrix for linear operations.
            # Lets calculate A*x for some x.
            #
            x = [1,-1,1,-1]
            if _spoil_scenario==1:
                spoil_vec_by_deleting_element(x)
            y = []
            y = xalglib.sparsemv(s, x, y)
            _TestResult = _TestResult and doc_print_test(y, [1.000,-2.000,2.000,-9], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("sparse_d_crs                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST solve_real
    #      Solving dense linear equations
    #
    _TestResult = True
    for _spoil_scenario in range(-1,43):
        try:
            #
            # This example demonstrates solution of a dense real linear system
            #

            #
            # First, solve A*x=b with a feature-rich rmatrixsolve() which supports iterative improvement
            # and condition number estimation
            #
            a = [[4,2],[-1,3]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(a)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(a)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(a)
            if _spoil_scenario==3:
                spoil_mat_by_adding_row(a)
            if _spoil_scenario==4:
                spoil_mat_by_adding_col(a)
            if _spoil_scenario==5:
                spoil_mat_by_deleting_row(a)
            if _spoil_scenario==6:
                spoil_mat_by_deleting_col(a)
            b = [8,5]
            if _spoil_scenario==7:
                spoil_vec_by_nan(b)
            if _spoil_scenario==8:
                spoil_vec_by_posinf(b)
            if _spoil_scenario==9:
                spoil_vec_by_neginf(b)
            if _spoil_scenario==10:
                spoil_vec_by_adding_element(b)
            if _spoil_scenario==11:
                spoil_vec_by_deleting_element(b)
            x, rep = xalglib.rmatrixsolve(a, b)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [1.0000, 2.0000], "real_vector", 0.00005)

            #
            # Then, solve C*x=d with rmatrixsolvefast() which has lower overhead
            #
            c = [[3,1],[2,4]]
            if _spoil_scenario==12:
                spoil_mat_by_nan(c)
            if _spoil_scenario==13:
                spoil_mat_by_posinf(c)
            if _spoil_scenario==14:
                spoil_mat_by_neginf(c)
            if _spoil_scenario==15:
                spoil_mat_by_adding_row(c)
            if _spoil_scenario==16:
                spoil_mat_by_adding_col(c)
            if _spoil_scenario==17:
                spoil_mat_by_deleting_row(c)
            if _spoil_scenario==18:
                spoil_mat_by_deleting_col(c)
            d = [2,-2]
            if _spoil_scenario==19:
                spoil_vec_by_nan(d)
            if _spoil_scenario==20:
                spoil_vec_by_posinf(d)
            if _spoil_scenario==21:
                spoil_vec_by_neginf(d)
            if _spoil_scenario==22:
                spoil_vec_by_adding_element(d)
            if _spoil_scenario==23:
                spoil_vec_by_deleting_element(d)
            xalglib.rmatrixsolvefast(c, d)
            _TestResult = _TestResult and doc_print_test(d, [1.0000, -1.0000], "real_vector", 0.00005)

            #
            # Sometimes you have LU decomposition of the system matrix readily
            # available. In such cases it is possible to save a lot of time by
            # passing precomputed LU factors to rmatrixlusolve(). The only
            # downside of such approach is that iterative refinement is unavailable
            # because original (unmodified) form of the system matrix is unknown
            # to ALGLIB.
            #
            # However, if you have BOTH original matrix and its LU decomposition,
            # it is possible to use rmatrixmixedsolve() which accepts both matrix
            # itself and its factors, and uses original matrix to refine solution
            # obtained with LU factors.
            #
            e = [[3,4],[2,4]]
            if _spoil_scenario==24:
                spoil_mat_by_nan(e)
            if _spoil_scenario==25:
                spoil_mat_by_posinf(e)
            if _spoil_scenario==26:
                spoil_mat_by_neginf(e)
            if _spoil_scenario==27:
                spoil_mat_by_adding_row(e)
            if _spoil_scenario==28:
                spoil_mat_by_adding_col(e)
            if _spoil_scenario==29:
                spoil_mat_by_deleting_row(e)
            if _spoil_scenario==30:
                spoil_mat_by_deleting_col(e)
            lue = [[3,4],[2,4]]
            if _spoil_scenario==31:
                spoil_mat_by_nan(lue)
            if _spoil_scenario==32:
                spoil_mat_by_posinf(lue)
            if _spoil_scenario==33:
                spoil_mat_by_neginf(lue)
            if _spoil_scenario==34:
                spoil_mat_by_adding_row(lue)
            if _spoil_scenario==35:
                spoil_mat_by_adding_col(lue)
            if _spoil_scenario==36:
                spoil_mat_by_deleting_row(lue)
            if _spoil_scenario==37:
                spoil_mat_by_deleting_col(lue)
            f = [2,0]
            if _spoil_scenario==38:
                spoil_vec_by_nan(f)
            if _spoil_scenario==39:
                spoil_vec_by_posinf(f)
            if _spoil_scenario==40:
                spoil_vec_by_neginf(f)
            if _spoil_scenario==41:
                spoil_vec_by_adding_element(f)
            if _spoil_scenario==42:
                spoil_vec_by_deleting_element(f)
            pivots = xalglib.rmatrixlu(lue)
            x, rep = xalglib.rmatrixlusolve(lue, pivots, f)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [2.0000, -1.0000], "real_vector", 0.00005)

            x, rep = xalglib.rmatrixmixedsolve(e, lue, pivots, f)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [2.0000, -1.0000], "real_vector", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("solve_real                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST solve_ls
    #      Solving dense linear equations in the least squares sense
    #
    _TestResult = True
    for _spoil_scenario in range(-1,6):
        try:
            a = [[4,2],[-1,3],[6,5]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(a)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(a)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(a)
            b = [8,5,16]
            if _spoil_scenario==3:
                spoil_vec_by_nan(b)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(b)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(b)
            x, rep = xalglib.rmatrixsolvels(a, b, 0.0)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [1.0000, 2.0000], "real_vector", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("solve_ls                         FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST solve_real_m
    #      Solving dense linear matrix equations
    #
    _TestResult = True
    for _spoil_scenario in range(-1,37):
        try:
            #
            # This example demonstrates solution of a dense real matrix system
            #

            #
            # First, solve A*X=B with a feature-rich rmatrixsolvem() which supports
            # iterative improvement and condition number estimation. Here A is
            # an N*N matrix, X is an N*M matrix, B is an N*M matrix.
            #
            a = [[4,2],[-1,3]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(a)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(a)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(a)
            if _spoil_scenario==3:
                spoil_mat_by_adding_row(a)
            if _spoil_scenario==4:
                spoil_mat_by_adding_col(a)
            if _spoil_scenario==5:
                spoil_mat_by_deleting_row(a)
            if _spoil_scenario==6:
                spoil_mat_by_deleting_col(a)
            b = [[8,10,4],[5,1,-1]]
            if _spoil_scenario==7:
                spoil_mat_by_nan(b)
            if _spoil_scenario==8:
                spoil_mat_by_posinf(b)
            if _spoil_scenario==9:
                spoil_mat_by_neginf(b)
            x, rep = xalglib.rmatrixsolvem(a, b, True)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [[1.0000, 2.0000,1.0000],[2.0000,1.0000,0.0000]], "real_matrix", 0.00005)

            #
            # Then, solve C*X=D with rmatrixsolvemfast() which has lower overhead
            # due to condition number estimation and iterative refinement parts
            # being dropped.
            #
            c = [[3,1],[2,4]]
            if _spoil_scenario==10:
                spoil_mat_by_nan(c)
            if _spoil_scenario==11:
                spoil_mat_by_posinf(c)
            if _spoil_scenario==12:
                spoil_mat_by_neginf(c)
            if _spoil_scenario==13:
                spoil_mat_by_adding_row(c)
            if _spoil_scenario==14:
                spoil_mat_by_adding_col(c)
            if _spoil_scenario==15:
                spoil_mat_by_deleting_row(c)
            if _spoil_scenario==16:
                spoil_mat_by_deleting_col(c)
            d = [[2,1],[-2,4]]
            if _spoil_scenario==17:
                spoil_mat_by_nan(d)
            if _spoil_scenario==18:
                spoil_mat_by_posinf(d)
            if _spoil_scenario==19:
                spoil_mat_by_neginf(d)
            xalglib.rmatrixsolvemfast(c, d)
            _TestResult = _TestResult and doc_print_test(d, [[1.0000,0.0000],[-1.0000,1.0000]], "real_matrix", 0.00005)

            #
            # Sometimes you have LU decomposition of the system matrix readily
            # available. In such cases it is possible to save a lot of time by
            # passing precomputed LU factors to rmatrixlusolve(). The only
            # downside of such approach is that iterative refinement is unavailable
            # because original (unmodified) form of the system matrix is unknown
            # to ALGLIB.
            #
            # However, if you have BOTH original matrix and its LU decomposition,
            # it is possible to use rmatrixmixedsolve() which accepts both matrix
            # itself and its factors, and uses original matrix to refine solution
            # obtained with LU factors.
            #
            e = [[3,4],[2,4]]
            if _spoil_scenario==20:
                spoil_mat_by_nan(e)
            if _spoil_scenario==21:
                spoil_mat_by_posinf(e)
            if _spoil_scenario==22:
                spoil_mat_by_neginf(e)
            if _spoil_scenario==23:
                spoil_mat_by_adding_row(e)
            if _spoil_scenario==24:
                spoil_mat_by_adding_col(e)
            if _spoil_scenario==25:
                spoil_mat_by_deleting_row(e)
            if _spoil_scenario==26:
                spoil_mat_by_deleting_col(e)
            lue = [[3,4],[2,4]]
            if _spoil_scenario==27:
                spoil_mat_by_nan(lue)
            if _spoil_scenario==28:
                spoil_mat_by_posinf(lue)
            if _spoil_scenario==29:
                spoil_mat_by_neginf(lue)
            if _spoil_scenario==30:
                spoil_mat_by_adding_row(lue)
            if _spoil_scenario==31:
                spoil_mat_by_adding_col(lue)
            if _spoil_scenario==32:
                spoil_mat_by_deleting_row(lue)
            if _spoil_scenario==33:
                spoil_mat_by_deleting_col(lue)
            f = [[2,5],[0,6]]
            if _spoil_scenario==34:
                spoil_mat_by_nan(f)
            if _spoil_scenario==35:
                spoil_mat_by_posinf(f)
            if _spoil_scenario==36:
                spoil_mat_by_neginf(f)
            pivots = xalglib.rmatrixlu(lue)
            x, rep = xalglib.rmatrixlusolvem(lue, pivots, f)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [[2.0000,-1.0000],[-1.0000,2.0000]], "real_matrix", 0.00005)

            x, rep = xalglib.rmatrixmixedsolvem(e, lue, pivots, f)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [[2.0000,-1.0000],[-1.0000,2.0000]], "real_matrix", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("solve_real_m                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST solve_complex
    #      Solving dense complex linear equations
    #
    _TestResult = True
    for _spoil_scenario in range(-1,43):
        try:
            #
            # This example demonstrates solution of a complex linear system
            #

            #
            # First, solve A*x=b with a feature-rich cmatrixsolve() which supports iterative improvement
            # and condition number estimation
            #
            a = [[-4,2j],[-1j,3]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(a)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(a)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(a)
            if _spoil_scenario==3:
                spoil_mat_by_adding_row(a)
            if _spoil_scenario==4:
                spoil_mat_by_adding_col(a)
            if _spoil_scenario==5:
                spoil_mat_by_deleting_row(a)
            if _spoil_scenario==6:
                spoil_mat_by_deleting_col(a)
            b = [8j,5]
            if _spoil_scenario==7:
                spoil_vec_by_nan(b)
            if _spoil_scenario==8:
                spoil_vec_by_posinf(b)
            if _spoil_scenario==9:
                spoil_vec_by_neginf(b)
            if _spoil_scenario==10:
                spoil_vec_by_adding_element(b)
            if _spoil_scenario==11:
                spoil_vec_by_deleting_element(b)
            x, rep = xalglib.cmatrixsolve(a, b)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [-1.0000j, 2.0000], "complex_vector", 0.00005)

            #
            # Then, solve C*x=d with cmatrixsolvefast() which has lower overhead
            #
            c = [[3j,1],[2j,4]]
            if _spoil_scenario==12:
                spoil_mat_by_nan(c)
            if _spoil_scenario==13:
                spoil_mat_by_posinf(c)
            if _spoil_scenario==14:
                spoil_mat_by_neginf(c)
            if _spoil_scenario==15:
                spoil_mat_by_adding_row(c)
            if _spoil_scenario==16:
                spoil_mat_by_adding_col(c)
            if _spoil_scenario==17:
                spoil_mat_by_deleting_row(c)
            if _spoil_scenario==18:
                spoil_mat_by_deleting_col(c)
            d = [2,-2]
            if _spoil_scenario==19:
                spoil_vec_by_nan(d)
            if _spoil_scenario==20:
                spoil_vec_by_posinf(d)
            if _spoil_scenario==21:
                spoil_vec_by_neginf(d)
            if _spoil_scenario==22:
                spoil_vec_by_adding_element(d)
            if _spoil_scenario==23:
                spoil_vec_by_deleting_element(d)
            xalglib.cmatrixsolvefast(c, d)
            _TestResult = _TestResult and doc_print_test(d, [-1.0000j, -1.0000], "complex_vector", 0.00005)

            #
            # Sometimes you have LU decomposition of the system matrix readily
            # available. In such cases it is possible to save a lot of time by
            # passing precomputed LU factors to cmatrixlusolve(). The only
            # downside of such approach is that iterative refinement is unavailable
            # because original (unmodified) form of the system matrix is unknown
            # to ALGLIB.
            #
            # However, if you have BOTH original matrix and its LU decomposition,
            # it is possible to use cmatrixmixedsolve() which accepts both matrix
            # itself and its factors, and uses original matrix to refine solution
            # obtained with LU factors.
            #
            e = [[-3,4j],[2j,4]]
            if _spoil_scenario==24:
                spoil_mat_by_nan(e)
            if _spoil_scenario==25:
                spoil_mat_by_posinf(e)
            if _spoil_scenario==26:
                spoil_mat_by_neginf(e)
            if _spoil_scenario==27:
                spoil_mat_by_adding_row(e)
            if _spoil_scenario==28:
                spoil_mat_by_adding_col(e)
            if _spoil_scenario==29:
                spoil_mat_by_deleting_row(e)
            if _spoil_scenario==30:
                spoil_mat_by_deleting_col(e)
            lue = [[-3,4j],[2j,4]]
            if _spoil_scenario==31:
                spoil_mat_by_nan(lue)
            if _spoil_scenario==32:
                spoil_mat_by_posinf(lue)
            if _spoil_scenario==33:
                spoil_mat_by_neginf(lue)
            if _spoil_scenario==34:
                spoil_mat_by_adding_row(lue)
            if _spoil_scenario==35:
                spoil_mat_by_adding_col(lue)
            if _spoil_scenario==36:
                spoil_mat_by_deleting_row(lue)
            if _spoil_scenario==37:
                spoil_mat_by_deleting_col(lue)
            f = [2j,0]
            if _spoil_scenario==38:
                spoil_vec_by_nan(f)
            if _spoil_scenario==39:
                spoil_vec_by_posinf(f)
            if _spoil_scenario==40:
                spoil_vec_by_neginf(f)
            if _spoil_scenario==41:
                spoil_vec_by_adding_element(f)
            if _spoil_scenario==42:
                spoil_vec_by_deleting_element(f)
            pivots = xalglib.cmatrixlu(lue)
            x, rep = xalglib.cmatrixlusolve(lue, pivots, f)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [-2.0000j, -1.0000], "complex_vector", 0.00005)

            x, rep = xalglib.cmatrixmixedsolve(e, lue, pivots, f)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [-2.0000j, -1.0000], "complex_vector", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("solve_complex                    FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST solve_complex_m
    #      Solving complex matrix equations
    #
    _TestResult = True
    for _spoil_scenario in range(-1,37):
        try:
            #
            # This example demonstrates solution of a dense complex matrix system
            #

            #
            # First, solve A*X=B with a feature-rich cmatrixsolvem() which supports
            # iterative improvement and condition number estimation. Here A is
            # an N*N matrix, X is an N*M matrix, B is an N*M matrix.
            #
            a = [[4j,-2],[-1,3j]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(a)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(a)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(a)
            if _spoil_scenario==3:
                spoil_mat_by_adding_row(a)
            if _spoil_scenario==4:
                spoil_mat_by_adding_col(a)
            if _spoil_scenario==5:
                spoil_mat_by_deleting_row(a)
            if _spoil_scenario==6:
                spoil_mat_by_deleting_col(a)
            b = [[8j,10j,4j],[5,1,-1]]
            if _spoil_scenario==7:
                spoil_mat_by_nan(b)
            if _spoil_scenario==8:
                spoil_mat_by_posinf(b)
            if _spoil_scenario==9:
                spoil_mat_by_neginf(b)
            x, rep = xalglib.cmatrixsolvem(a, b, True)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [[1.0000, 2.0000,1.0000],[-2.0000j,-1.0000j,0.0000]], "complex_matrix", 0.00005)

            #
            # Then, solve C*X=D with cmatrixsolvemfast() which has lower overhead
            # due to condition number estimation and iterative refinement parts
            # being dropped.
            #
            c = [[3,1],[2,4]]
            if _spoil_scenario==10:
                spoil_mat_by_nan(c)
            if _spoil_scenario==11:
                spoil_mat_by_posinf(c)
            if _spoil_scenario==12:
                spoil_mat_by_neginf(c)
            if _spoil_scenario==13:
                spoil_mat_by_adding_row(c)
            if _spoil_scenario==14:
                spoil_mat_by_adding_col(c)
            if _spoil_scenario==15:
                spoil_mat_by_deleting_row(c)
            if _spoil_scenario==16:
                spoil_mat_by_deleting_col(c)
            d = [[2,1],[-2,4]]
            if _spoil_scenario==17:
                spoil_mat_by_nan(d)
            if _spoil_scenario==18:
                spoil_mat_by_posinf(d)
            if _spoil_scenario==19:
                spoil_mat_by_neginf(d)
            xalglib.cmatrixsolvemfast(c, d)
            _TestResult = _TestResult and doc_print_test(d, [[1.0000,0.0000],[-1.0000,1.0000]], "complex_matrix", 0.00005)

            #
            # Sometimes you have LU decomposition of the system matrix readily
            # available. In such cases it is possible to save a lot of time by
            # passing precomputed LU factors to cmatrixlusolve(). The only
            # downside of such approach is that iterative refinement is unavailable
            # because original (unmodified) form of the system matrix is unknown
            # to ALGLIB.
            #
            # However, if you have BOTH original matrix and its LU decomposition,
            # it is possible to use cmatrixmixedsolve() which accepts both matrix
            # itself and its factors, and uses original matrix to refine solution
            # obtained with LU factors.
            #
            e = [[3,4],[2,4]]
            if _spoil_scenario==20:
                spoil_mat_by_nan(e)
            if _spoil_scenario==21:
                spoil_mat_by_posinf(e)
            if _spoil_scenario==22:
                spoil_mat_by_neginf(e)
            if _spoil_scenario==23:
                spoil_mat_by_adding_row(e)
            if _spoil_scenario==24:
                spoil_mat_by_adding_col(e)
            if _spoil_scenario==25:
                spoil_mat_by_deleting_row(e)
            if _spoil_scenario==26:
                spoil_mat_by_deleting_col(e)
            lue = [[3,4],[2,4]]
            if _spoil_scenario==27:
                spoil_mat_by_nan(lue)
            if _spoil_scenario==28:
                spoil_mat_by_posinf(lue)
            if _spoil_scenario==29:
                spoil_mat_by_neginf(lue)
            if _spoil_scenario==30:
                spoil_mat_by_adding_row(lue)
            if _spoil_scenario==31:
                spoil_mat_by_adding_col(lue)
            if _spoil_scenario==32:
                spoil_mat_by_deleting_row(lue)
            if _spoil_scenario==33:
                spoil_mat_by_deleting_col(lue)
            f = [[2,5],[0,6]]
            if _spoil_scenario==34:
                spoil_mat_by_nan(f)
            if _spoil_scenario==35:
                spoil_mat_by_posinf(f)
            if _spoil_scenario==36:
                spoil_mat_by_neginf(f)
            pivots = xalglib.cmatrixlu(lue)
            x, rep = xalglib.cmatrixlusolvem(lue, pivots, f)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [[2.0000,-1.0000],[-1.0000,2.0000]], "complex_matrix", 0.00005)

            x, rep = xalglib.cmatrixmixedsolvem(e, lue, pivots, f)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [[2.0000,-1.0000],[-1.0000,2.0000]], "complex_matrix", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("solve_complex_m                  FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST solve_spd
    #      Solving symmetric positive definite linear equations
    #
    _TestResult = True
    for _spoil_scenario in range(-1,18):
        try:
            #
            # This example demonstrates solution of a symmetric positive definite real system
            #
            isupper = True

            #
            # First, solve A*x=b with a feature-rich spdmatrixsolve() which supports iterative improvement
            # and condition number estimation
            #
            a = [[4,1],[1,4]]
            if _spoil_scenario==0:
                spoil_mat_by_adding_row(a)
            if _spoil_scenario==1:
                spoil_mat_by_adding_col(a)
            if _spoil_scenario==2:
                spoil_mat_by_deleting_row(a)
            if _spoil_scenario==3:
                spoil_mat_by_deleting_col(a)
            b = [6,9]
            if _spoil_scenario==4:
                spoil_vec_by_adding_element(b)
            if _spoil_scenario==5:
                spoil_vec_by_deleting_element(b)
            x, rep = xalglib.spdmatrixsolve(a, isupper, b)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [1.0000, 2.0000], "real_vector", 0.00005)

            #
            # Then, solve C*x=d with spdmatrixsolvefast() which has lower overhead
            #
            c = [[3,1],[1,3]]
            if _spoil_scenario==6:
                spoil_mat_by_adding_row(c)
            if _spoil_scenario==7:
                spoil_mat_by_adding_col(c)
            if _spoil_scenario==8:
                spoil_mat_by_deleting_row(c)
            if _spoil_scenario==9:
                spoil_mat_by_deleting_col(c)
            d = [2,-2]
            if _spoil_scenario==10:
                spoil_vec_by_adding_element(d)
            if _spoil_scenario==11:
                spoil_vec_by_deleting_element(d)
            xalglib.spdmatrixsolvefast(c, isupper, d)
            _TestResult = _TestResult and doc_print_test(d, [1.0000, -1.0000], "real_vector", 0.00005)

            #
            # Sometimes you have Cholesky decomposition of the system matrix readily
            # available. In such cases it is possible to save a lot of time by
            # passing precomputed Cholesky factor to spdmatrixcholeskysolve(). The only
            # downside of such approach is that iterative refinement is unavailable
            # because original (unmodified) form of the system matrix is unknown
            # to ALGLIB.
            #
            e = [[3,2],[2,3]]
            if _spoil_scenario==12:
                spoil_mat_by_adding_row(e)
            if _spoil_scenario==13:
                spoil_mat_by_adding_col(e)
            if _spoil_scenario==14:
                spoil_mat_by_deleting_row(e)
            if _spoil_scenario==15:
                spoil_mat_by_deleting_col(e)
            f = [4,1]
            if _spoil_scenario==16:
                spoil_vec_by_adding_element(f)
            if _spoil_scenario==17:
                spoil_vec_by_deleting_element(f)
            xalglib.spdmatrixcholesky(e, isupper)
            x, rep = xalglib.spdmatrixcholeskysolve(e, isupper, f)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [2.0000, -1.0000], "real_vector", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("solve_spd                        FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST solve_hpd
    #      Solving Hermitian positive definite linear equations
    #
    _TestResult = True
    for _spoil_scenario in range(-1,18):
        try:
            #
            # This example demonstrates solution of a Hermitian positive definite complex system
            #
            isupper = True

            #
            # First, solve A*x=b with a feature-rich hpdmatrixsolve() which supports iterative improvement
            # and condition number estimation
            #
            a = [[4,1j],[-1j,4]]
            if _spoil_scenario==0:
                spoil_mat_by_adding_row(a)
            if _spoil_scenario==1:
                spoil_mat_by_adding_col(a)
            if _spoil_scenario==2:
                spoil_mat_by_deleting_row(a)
            if _spoil_scenario==3:
                spoil_mat_by_deleting_col(a)
            b = [6,-9j]
            if _spoil_scenario==4:
                spoil_vec_by_adding_element(b)
            if _spoil_scenario==5:
                spoil_vec_by_deleting_element(b)
            x, rep = xalglib.hpdmatrixsolve(a, isupper, b)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [1.0000, -2.0000j], "complex_vector", 0.00005)

            #
            # Then, solve C*x=d with hpdmatrixsolvefast() which has lower overhead
            #
            c = [[3,-1j],[1j,3]]
            if _spoil_scenario==6:
                spoil_mat_by_adding_row(c)
            if _spoil_scenario==7:
                spoil_mat_by_adding_col(c)
            if _spoil_scenario==8:
                spoil_mat_by_deleting_row(c)
            if _spoil_scenario==9:
                spoil_mat_by_deleting_col(c)
            d = [-2j,-2]
            if _spoil_scenario==10:
                spoil_vec_by_adding_element(d)
            if _spoil_scenario==11:
                spoil_vec_by_deleting_element(d)
            xalglib.hpdmatrixsolvefast(c, isupper, d)
            _TestResult = _TestResult and doc_print_test(d, [-1.0000j, -1.0000], "complex_vector", 0.00005)

            #
            # Sometimes you have Cholesky decomposition of the system matrix readily
            # available. In such cases it is possible to save a lot of time by
            # passing precomputed Cholesky factor to hpdmatrixcholeskysolve(). The only
            # downside of such approach is that iterative refinement is unavailable
            # because original (unmodified) form of the system matrix is unknown
            # to ALGLIB.
            #
            e = [[3,2],[2,3]]
            if _spoil_scenario==12:
                spoil_mat_by_adding_row(e)
            if _spoil_scenario==13:
                spoil_mat_by_adding_col(e)
            if _spoil_scenario==14:
                spoil_mat_by_deleting_row(e)
            if _spoil_scenario==15:
                spoil_mat_by_deleting_col(e)
            f = [4,1]
            if _spoil_scenario==16:
                spoil_vec_by_adding_element(f)
            if _spoil_scenario==17:
                spoil_vec_by_deleting_element(f)
            xalglib.hpdmatrixcholesky(e, isupper)
            x, rep = xalglib.hpdmatrixcholeskysolve(e, isupper, f)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [2.0000, -1.0000], "complex_vector", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("solve_hpd                        FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST solve_real_tst
    #      .
    #
    _TestResult = True
    for _spoil_scenario in range(-1,36):
        try:
            a = [[4,2],[-1,3]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(a)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(a)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(a)
            if _spoil_scenario==3:
                spoil_mat_by_deleting_row(a)
            if _spoil_scenario==4:
                spoil_mat_by_deleting_col(a)
            b = [8,5]
            if _spoil_scenario==5:
                spoil_vec_by_nan(b)
            if _spoil_scenario==6:
                spoil_vec_by_posinf(b)
            if _spoil_scenario==7:
                spoil_vec_by_neginf(b)
            if _spoil_scenario==8:
                spoil_vec_by_deleting_element(b)
            x, rep = xalglib.rmatrixsolve(a, 2, b)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [1.0000, 2.0000], "real_vector", 0.00005)
            c = [[3,1],[2,4]]
            if _spoil_scenario==9:
                spoil_mat_by_nan(c)
            if _spoil_scenario==10:
                spoil_mat_by_posinf(c)
            if _spoil_scenario==11:
                spoil_mat_by_neginf(c)
            if _spoil_scenario==12:
                spoil_mat_by_deleting_row(c)
            if _spoil_scenario==13:
                spoil_mat_by_deleting_col(c)
            d = [2,-2]
            if _spoil_scenario==14:
                spoil_vec_by_nan(d)
            if _spoil_scenario==15:
                spoil_vec_by_posinf(d)
            if _spoil_scenario==16:
                spoil_vec_by_neginf(d)
            if _spoil_scenario==17:
                spoil_vec_by_deleting_element(d)
            xalglib.rmatrixsolvefast(c, 2, d)
            _TestResult = _TestResult and doc_print_test(d, [1.0000, -1.0000], "real_vector", 0.00005)

            e = [[3,4],[2,4]]
            if _spoil_scenario==18:
                spoil_mat_by_nan(e)
            if _spoil_scenario==19:
                spoil_mat_by_posinf(e)
            if _spoil_scenario==20:
                spoil_mat_by_neginf(e)
            if _spoil_scenario==21:
                spoil_mat_by_deleting_row(e)
            if _spoil_scenario==22:
                spoil_mat_by_deleting_col(e)
            lue = [[3,4],[2,4]]
            if _spoil_scenario==23:
                spoil_mat_by_nan(lue)
            if _spoil_scenario==24:
                spoil_mat_by_posinf(lue)
            if _spoil_scenario==25:
                spoil_mat_by_neginf(lue)
            if _spoil_scenario==26:
                spoil_mat_by_deleting_row(lue)
            if _spoil_scenario==27:
                spoil_mat_by_deleting_col(lue)
            f = [2,0]
            if _spoil_scenario==28:
                spoil_vec_by_nan(f)
            if _spoil_scenario==29:
                spoil_vec_by_posinf(f)
            if _spoil_scenario==30:
                spoil_vec_by_neginf(f)
            if _spoil_scenario==31:
                spoil_vec_by_deleting_element(f)
            pivots = xalglib.rmatrixlu(lue, 2, 2)
            x, rep = xalglib.rmatrixlusolve(lue, pivots, 2, f)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [2.0000, -1.0000], "real_vector", 0.00005)

            x, rep = xalglib.rmatrixmixedsolve(e, lue, pivots, 2, f)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [2.0000, -1.0000], "real_vector", 0.00005)

            f1 = [2,0]
            if _spoil_scenario==32:
                spoil_vec_by_nan(f1)
            if _spoil_scenario==33:
                spoil_vec_by_posinf(f1)
            if _spoil_scenario==34:
                spoil_vec_by_neginf(f1)
            if _spoil_scenario==35:
                spoil_vec_by_deleting_element(f1)
            xalglib.rmatrixlusolvefast(lue, pivots, 2, f1)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(f1, [2.0000, -1.0000], "real_vector", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("solve_real_tst                   FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST solve_real_m_test
    #      .
    #
    _TestResult = True
    for _spoil_scenario in range(-1,43):
        try:
            a = [[4,2],[-1,3]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(a)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(a)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(a)
            if _spoil_scenario==3:
                spoil_mat_by_deleting_row(a)
            if _spoil_scenario==4:
                spoil_mat_by_deleting_col(a)
            b = [[8,10,4],[5,1,-1]]
            if _spoil_scenario==5:
                spoil_mat_by_nan(b)
            if _spoil_scenario==6:
                spoil_mat_by_posinf(b)
            if _spoil_scenario==7:
                spoil_mat_by_neginf(b)
            if _spoil_scenario==8:
                spoil_mat_by_deleting_row(b)
            if _spoil_scenario==9:
                spoil_mat_by_deleting_col(b)
            x, rep = xalglib.rmatrixsolvem(a, 2, b, 3, True)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [[1.0000, 2.0000,1.0000],[2.0000,1.0000,0.0000]], "real_matrix", 0.00005)
            c = [[3,1],[2,4]]
            if _spoil_scenario==10:
                spoil_mat_by_nan(c)
            if _spoil_scenario==11:
                spoil_mat_by_posinf(c)
            if _spoil_scenario==12:
                spoil_mat_by_neginf(c)
            if _spoil_scenario==13:
                spoil_mat_by_deleting_row(c)
            if _spoil_scenario==14:
                spoil_mat_by_deleting_col(c)
            d = [[2,1],[-2,4]]
            if _spoil_scenario==15:
                spoil_mat_by_nan(d)
            if _spoil_scenario==16:
                spoil_mat_by_posinf(d)
            if _spoil_scenario==17:
                spoil_mat_by_neginf(d)
            if _spoil_scenario==18:
                spoil_mat_by_deleting_row(d)
            if _spoil_scenario==19:
                spoil_mat_by_deleting_col(d)
            xalglib.rmatrixsolvemfast(c, 2, d, 2)
            _TestResult = _TestResult and doc_print_test(d, [[1.0000,0.0000],[-1.0000,1.0000]], "real_matrix", 0.00005)

            e = [[3,4],[2,4]]
            if _spoil_scenario==20:
                spoil_mat_by_nan(e)
            if _spoil_scenario==21:
                spoil_mat_by_posinf(e)
            if _spoil_scenario==22:
                spoil_mat_by_neginf(e)
            if _spoil_scenario==23:
                spoil_mat_by_deleting_row(e)
            if _spoil_scenario==24:
                spoil_mat_by_deleting_col(e)
            lue = [[3,4],[2,4]]
            if _spoil_scenario==25:
                spoil_mat_by_nan(lue)
            if _spoil_scenario==26:
                spoil_mat_by_posinf(lue)
            if _spoil_scenario==27:
                spoil_mat_by_neginf(lue)
            if _spoil_scenario==28:
                spoil_mat_by_deleting_row(lue)
            if _spoil_scenario==29:
                spoil_mat_by_deleting_col(lue)
            f = [[2,5],[0,6]]
            if _spoil_scenario==30:
                spoil_mat_by_nan(f)
            if _spoil_scenario==31:
                spoil_mat_by_posinf(f)
            if _spoil_scenario==32:
                spoil_mat_by_neginf(f)
            if _spoil_scenario==33:
                spoil_mat_by_deleting_row(f)
            if _spoil_scenario==34:
                spoil_mat_by_deleting_col(f)
            pivots = xalglib.rmatrixlu(lue, 2, 2)
            x, rep = xalglib.rmatrixlusolvem(lue, pivots, 2, f, 2)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [[2.0000,-1.0000],[-1.0000,2.0000]], "real_matrix", 0.00005)

            x, rep = xalglib.rmatrixmixedsolvem(e, lue, pivots, 2, f, 2)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [[2.0000,-1.0000],[-1.0000,2.0000]], "real_matrix", 0.00005)

            f1 = [[2,5],[0,6]]
            if _spoil_scenario==35:
                spoil_mat_by_nan(f1)
            if _spoil_scenario==36:
                spoil_mat_by_posinf(f1)
            if _spoil_scenario==37:
                spoil_mat_by_neginf(f1)
            xalglib.rmatrixlusolvemfast(lue, pivots, f1)
            _TestResult = _TestResult and doc_print_test(f1, [[2.0000,-1.0000],[-1.0000,2.0000]], "real_matrix", 0.00005)

            f2 = [[2,5],[0,6]]
            if _spoil_scenario==38:
                spoil_mat_by_nan(f2)
            if _spoil_scenario==39:
                spoil_mat_by_posinf(f2)
            if _spoil_scenario==40:
                spoil_mat_by_neginf(f2)
            if _spoil_scenario==41:
                spoil_mat_by_deleting_row(f2)
            if _spoil_scenario==42:
                spoil_mat_by_deleting_col(f2)
            xalglib.rmatrixlusolvemfast(lue, pivots, 2, f2, 2)
            _TestResult = _TestResult and doc_print_test(f2, [[2.0000,-1.0000],[-1.0000,2.0000]], "real_matrix", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("solve_real_m_test                FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST lincg_d_1
    #      Solution of sparse linear systems with CG
    #
    _TestResult = True
    for _spoil_scenario in range(-1,4):
        try:
            #
            # This example illustrates solution of sparse linear systems with
            # conjugate gradient method.
            # 
            # Suppose that we have linear system A*x=b with sparse symmetric
            # positive definite A (represented by sparsematrix object)
            #         [ 5 1       ]
            #         [ 1 7 2     ]
            #     A = [   2 8 1   ]
            #         [     1 4 1 ]
            #         [       1 4 ]
            # and right part b
            #     [  7 ]
            #     [ 17 ]
            # b = [ 14 ]
            #     [ 10 ]
            #     [  6 ]
            # and we want to solve this system using sparse linear CG. In order
            # to do so, we have to create left part (sparsematrix object) and
            # right part (dense array).
            #
            # Initially, sparse matrix is created in the Hash-Table format,
            # which allows easy initialization, but do not allow matrix to be
            # used in the linear solvers. So after construction you should convert
            # sparse matrix to CRS format (one suited for linear operations).
            #
            # It is important to note that in our example we initialize full
            # matrix A, both lower and upper triangles. However, it is symmetric
            # and sparse solver needs just one half of the matrix. So you may
            # save about half of the space by filling only one of the triangles.
            #
            a = xalglib.sparsecreate(5, 5)
            xalglib.sparseset(a, 0, 0, 5.0)
            xalglib.sparseset(a, 0, 1, 1.0)
            xalglib.sparseset(a, 1, 0, 1.0)
            xalglib.sparseset(a, 1, 1, 7.0)
            xalglib.sparseset(a, 1, 2, 2.0)
            xalglib.sparseset(a, 2, 1, 2.0)
            xalglib.sparseset(a, 2, 2, 8.0)
            xalglib.sparseset(a, 2, 3, 1.0)
            xalglib.sparseset(a, 3, 2, 1.0)
            xalglib.sparseset(a, 3, 3, 4.0)
            xalglib.sparseset(a, 3, 4, 1.0)
            xalglib.sparseset(a, 4, 3, 1.0)
            xalglib.sparseset(a, 4, 4, 4.0)

            #
            # Now our matrix is fully initialized, but we have to do one more
            # step - convert it from Hash-Table format to CRS format (see
            # documentation on sparse matrices for more information about these
            # formats).
            #
            # If you omit this call, ALGLIB will generate exception on the first
            # attempt to use A in linear operations. 
            #
            xalglib.sparseconverttocrs(a)

            #
            # Initialization of the right part
            #
            b = [7,17,14,10,6]
            if _spoil_scenario==0:
                spoil_vec_by_nan(b)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(b)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(b)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(b)

            #
            # Now we have to create linear solver object and to use it for the
            # solution of the linear system.
            #
            # NOTE: lincgsolvesparse() accepts additional parameter which tells
            #       what triangle of the symmetric matrix should be used - upper
            #       or lower. Because we've filled both parts of the matrix, we
            #       can use any part - upper or lower.
            #
            s = xalglib.lincgcreate(5)
            xalglib.lincgsolvesparse(s, a, True, b)
            x, rep = xalglib.lincgresults(s)

            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(x, [1.000,2.000,1.000,2.000,1.000], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("lincg_d_1                        FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST linlsqr_d_1
    #      Solution of sparse linear systems with CG
    #
    _TestResult = True
    for _spoil_scenario in range(-1,4):
        try:
            #
            # This example illustrates solution of sparse linear least squares problem
            # with LSQR algorithm.
            # 
            # Suppose that we have least squares problem min|A*x-b| with sparse A
            # represented by sparsematrix object
            #         [ 1 1 ]
            #         [ 1 1 ]
            #     A = [ 2 1 ]
            #         [ 1   ]
            #         [   1 ]
            # and right part b
            #     [ 4 ]
            #     [ 2 ]
            # b = [ 4 ]
            #     [ 1 ]
            #     [ 2 ]
            # and we want to solve this system in the least squares sense using
            # LSQR algorithm. In order to do so, we have to create left part
            # (sparsematrix object) and right part (dense array).
            #
            # Initially, sparse matrix is created in the Hash-Table format,
            # which allows easy initialization, but do not allow matrix to be
            # used in the linear solvers. So after construction you should convert
            # sparse matrix to CRS format (one suited for linear operations).
            #
            a = xalglib.sparsecreate(5, 2)
            xalglib.sparseset(a, 0, 0, 1.0)
            xalglib.sparseset(a, 0, 1, 1.0)
            xalglib.sparseset(a, 1, 0, 1.0)
            xalglib.sparseset(a, 1, 1, 1.0)
            xalglib.sparseset(a, 2, 0, 2.0)
            xalglib.sparseset(a, 2, 1, 1.0)
            xalglib.sparseset(a, 3, 0, 1.0)
            xalglib.sparseset(a, 4, 1, 1.0)

            #
            # Now our matrix is fully initialized, but we have to do one more
            # step - convert it from Hash-Table format to CRS format (see
            # documentation on sparse matrices for more information about these
            # formats).
            #
            # If you omit this call, ALGLIB will generate exception on the first
            # attempt to use A in linear operations. 
            #
            xalglib.sparseconverttocrs(a)

            #
            # Initialization of the right part
            #
            b = [4,2,4,1,2]
            if _spoil_scenario==0:
                spoil_vec_by_nan(b)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(b)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(b)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(b)

            #
            # Now we have to create linear solver object and to use it for the
            # solution of the linear system.
            #
            s = xalglib.linlsqrcreate(5, 2)
            xalglib.linlsqrsolvesparse(s, a, b)
            x, rep = xalglib.linlsqrresults(s)

            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 4, "int")
            _TestResult = _TestResult and doc_print_test(x, [1.000,2.000], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("linlsqr_d_1                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST solvesks_d_1
    #      Solving low profile positive definite sparse systems with Skyline (SKS) solver
    #
    _TestResult = True
    for _spoil_scenario in range(-1,4):
        try:
            #
            # This example demonstrates creation/initialization of the sparse matrix
            # in the SKS (Skyline) storage format and solution using SKS-based direct
            # solver.
            #
            # NOTE: the SKS solver is intended for 'easy' tasks, i.e. low-profile positive
            #       definite systems (e.g. matrices with average bandwidth as low as 3),
            #       where it can avoid some overhead associated with more powerful supernodal
            #       Cholesky solver with AMD ordering.
            #
            #       It is recommended to use more powerful solvers for more difficult problems:
            #       * sparsespdsolve() for larger sparse positive definite systems
            #       * sparsesolve() for general (nonsymmetric) linear systems
            #
            # First, we have to create matrix and initialize it. Matrix is created
            # in the SKS format, using fixed bandwidth initialization function.
            # Several points should be noted:
            #
            # 1. SKS sparse storage format also allows variable bandwidth matrices;
            #    we just do not want to overcomplicate this example.
            #
            # 2. SKS format requires you to specify matrix geometry prior to
            #    initialization of its elements with sparseset(). If you specified
            #    bandwidth=1, you can not change your mind afterwards and call
            #    sparseset() for non-existent elements.
            # 
            # 3. Because SKS solver need just one triangle of SPD matrix, we can
            #    omit initialization of the lower triangle of our matrix.
            #
            n = 4
            bandwidth = 1
            s = xalglib.sparsecreatesksband(n, n, bandwidth)
            xalglib.sparseset(s, 0, 0, 2.0)
            xalglib.sparseset(s, 0, 1, 1.0)
            xalglib.sparseset(s, 1, 1, 3.0)
            xalglib.sparseset(s, 1, 2, 1.0)
            xalglib.sparseset(s, 2, 2, 3.0)
            xalglib.sparseset(s, 2, 3, 1.0)
            xalglib.sparseset(s, 3, 3, 2.0)

            #
            # Now we have symmetric positive definite 4x4 system width bandwidth=1:
            #
            #     [ 2 1     ]   [ x0]]   [  4 ]
            #     [ 1 3 1   ]   [ x1 ]   [ 10 ]
            #     [   1 3 1 ] * [ x2 ] = [ 15 ]
            #     [     1 2 ]   [ x3 ]   [ 11 ]
            #
            # After successful creation we can call SKS solver.
            #
            b = [4,10,15,11]
            if _spoil_scenario==0:
                spoil_vec_by_nan(b)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(b)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(b)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(b)
            isuppertriangle = True
            x, rep = xalglib.sparsespdsolvesks(s, isuppertriangle, b)
            _TestResult = _TestResult and doc_print_test(x, [1.0000, 2.0000, 3.0000, 4.0000], "real_vector", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("solvesks_d_1                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST sparse_solve_cholesky
    #      Solving positive definite sparse linear systems with the supernodal Cholesky solver
    #
    _TestResult = True
    for _spoil_scenario in range(-1,4):
        try:
            #
            # This example demonstrates creation/initialization of a sparse matrix and linear
            # system solution using Cholesky-based direct solver. This solver can handle any
            # problem sizes - from several tens of variables to millions of variables.
            #
            # First, we create a sparse matrix in the flexible hash table-based storage format,
            # initialize it and convert to the CRS format. Because the matrix is symmetric,
            # it is enough to specify only one triangle. The example below initializes the
            # lower one.
            #
            n = 4
            s = xalglib.sparsecreate(n, n, 0)
            xalglib.sparseset(s, 0, 0, 2.0)
            xalglib.sparseset(s, 1, 0, 1.0)
            xalglib.sparseset(s, 1, 1, 3.0)
            xalglib.sparseset(s, 2, 1, 1.0)
            xalglib.sparseset(s, 2, 2, 3.0)
            xalglib.sparseset(s, 3, 2, 1.0)
            xalglib.sparseset(s, 3, 3, 2.0)

            #
            # Now we have symmetric positive definite 4x4 system
            #
            #     [ 2 1     ]   [ x0]]   [  4 ]
            #     [ 1 3 1   ]   [ x1 ]   [ 10 ]
            #     [   1 3 1 ] * [ x2 ] = [ 15 ]
            #     [     1 2 ]   [ x3 ]   [ 11 ]
            #
            # Now, it is time to call the solver.
            #
            b = [4,10,15,11]
            if _spoil_scenario==0:
                spoil_vec_by_nan(b)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(b)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(b)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(b)
            isuppertriangle = False
            x, rep = xalglib.sparsespdsolve(s, isuppertriangle, b)
            _TestResult = _TestResult and doc_print_test(x, [1.0000, 2.0000, 3.0000, 4.0000], "real_vector", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("sparse_solve_cholesky            FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST sparse_solve
    #      Solving general sparse linear systems
    #
    _TestResult = True
    for _spoil_scenario in range(-1,4):
        try:
            #
            # This example demonstrates creation/initialization of a sparse matrix and linear
            # system solution using a direct solver. This solver can handle any problem sizes
            # - from several tens of variables to millions of variables.
            #
            # First, we create a sparse matrix in the flexible hash table-based storage format,
            # initialize it and convert to the CRS format.
            #
            n = 4
            s = xalglib.sparsecreate(n, n, 0)
            xalglib.sparseset(s, 0, 0, 2.0)
            xalglib.sparseset(s, 0, 1, 1.0)
            xalglib.sparseset(s, 1, 0, 1.0)
            xalglib.sparseset(s, 1, 1, 3.0)
            xalglib.sparseset(s, 1, 2, -1.0)
            xalglib.sparseset(s, 2, 2, 3.0)
            xalglib.sparseset(s, 2, 3, 1.0)
            xalglib.sparseset(s, 3, 2, 1.0)
            xalglib.sparseset(s, 3, 3, 2.0)

            #
            # Now we have symmetric positive definite 4x4 system
            #
            #     [ 2 1     ]   [ x0]]   [ 3 ]
            #     [ 1 3 -1  ]   [ x1 ]   [ 2 ]
            #     [     3 1 ] * [ x2 ] = [ 8 ]
            #     [     1 2 ]   [ x3 ]   [ 6 ]
            #
            # Now, it is time to call the solver. The sparsesolve() function supports several
            # solvers, our recommendation is to choose the default one. In the current version
            # it is a supernodal solver with static pivoting, followed by the iterative refinement.
            #
            b = [3,2,8,6]
            if _spoil_scenario==0:
                spoil_vec_by_nan(b)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(b)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(b)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(b)
            solvertype = 0
            x, rep = xalglib.sparsesolve(s, b, solvertype)
            _TestResult = _TestResult and doc_print_test(x, [1.0000, 1.0000, 2.0000, 2.0000], "real_vector", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("sparse_solve                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST matinv_d_r1
    #      Real matrix inverse
    #
    _TestResult = True
    for _spoil_scenario in range(-1,7):
        try:
            a = [[1,-1],[1,1]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(a)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(a)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(a)
            if _spoil_scenario==3:
                spoil_mat_by_adding_row(a)
            if _spoil_scenario==4:
                spoil_mat_by_adding_col(a)
            if _spoil_scenario==5:
                spoil_mat_by_deleting_row(a)
            if _spoil_scenario==6:
                spoil_mat_by_deleting_col(a)
            rep = xalglib.rmatrixinverse(a)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(a, [[0.5,0.5],[-0.5,0.5]], "real_matrix", 0.00005)
            _TestResult = _TestResult and doc_print_test(rep.r1, 0.5, "real", 0.00005)
            _TestResult = _TestResult and doc_print_test(rep.rinf, 0.5, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("matinv_d_r1                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST matinv_d_c1
    #      Complex matrix inverse
    #
    _TestResult = True
    for _spoil_scenario in range(-1,7):
        try:
            a = [[1j,-1],[1j,1]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(a)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(a)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(a)
            if _spoil_scenario==3:
                spoil_mat_by_adding_row(a)
            if _spoil_scenario==4:
                spoil_mat_by_adding_col(a)
            if _spoil_scenario==5:
                spoil_mat_by_deleting_row(a)
            if _spoil_scenario==6:
                spoil_mat_by_deleting_col(a)
            rep = xalglib.cmatrixinverse(a)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(a, [[-0.5j,-0.5j],[-0.5,0.5]], "complex_matrix", 0.00005)
            _TestResult = _TestResult and doc_print_test(rep.r1, 0.5, "real", 0.00005)
            _TestResult = _TestResult and doc_print_test(rep.rinf, 0.5, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("matinv_d_c1                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST matinv_d_spd1
    #      SPD matrix inverse
    #
    _TestResult = True
    for _spoil_scenario in range(-1,4):
        try:
            a = [[2,1],[1,2]]
            if _spoil_scenario==0:
                spoil_mat_by_adding_row(a)
            if _spoil_scenario==1:
                spoil_mat_by_adding_col(a)
            if _spoil_scenario==2:
                spoil_mat_by_deleting_row(a)
            if _spoil_scenario==3:
                spoil_mat_by_deleting_col(a)

            #
            # The matrix is given by its upper and lower triangles
            #
            #     [ 2 1 ]
            #     [ 1 2 ]
            #
            # However, spdmatrixinverse() accepts and modifies only one triangle - either
            # the upper or the lower one. The other triangle is left untouched. In this example
            # we modify the lower triangle. Thus, the inverse matrix is
            #
            #     [  2/3 -1/3 ]
            #     [ -1/3  2/3 ]
            #
            # but only lower triangle is returned, and the upper triangle is not modified:
            #
            #     [  2/3   1  ]
            #     [ -1/3  2/3 ]
            #
            #
            isupper = False
            rep = xalglib.spdmatrixinverse(a, isupper)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(a, [[0.666666,1],[-0.333333,0.666666]], "real_matrix", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("matinv_d_spd1                    FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST matinv_d_hpd1
    #      HPD matrix inverse
    #
    _TestResult = True
    for _spoil_scenario in range(-1,4):
        try:
            a = [[2,1],[1,2]]
            if _spoil_scenario==0:
                spoil_mat_by_adding_row(a)
            if _spoil_scenario==1:
                spoil_mat_by_adding_col(a)
            if _spoil_scenario==2:
                spoil_mat_by_deleting_row(a)
            if _spoil_scenario==3:
                spoil_mat_by_deleting_col(a)

            #
            # The matrix is given by its upper and lower triangles
            #
            #     [ 2 1 ]
            #     [ 1 2 ]
            #
            # However, hpdmatrixinverse() accepts and modifies only one triangle - either
            # the upper or the lower one. The other triangle is left untouched. In this example
            # we modify the lower triangle. Thus, the inverse matrix is
            #
            #     [  2/3 -1/3 ]
            #     [ -1/3  2/3 ]
            #
            # but only lower triangle is returned, and the upper triangle is not modified:
            #
            #     [  2/3   1  ]
            #     [ -1/3  2/3 ]
            #
            #
            isupper = False
            rep = xalglib.hpdmatrixinverse(a, isupper)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and doc_print_test(a, [[0.666666,1],[-0.333333,0.666666]], "complex_matrix", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("matinv_d_hpd1                    FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST matinv_t_r1
    #      Real matrix inverse: singular matrix
    #
    _TestResult = True
    try:
        a = [[1,-1],[-2,2]]
        rep = xalglib.rmatrixinverse(a)
        _TestResult = _TestResult and doc_print_test(rep.terminationtype, -3, "int")
        _TestResult = _TestResult and doc_print_test(rep.r1, 0.0, "real", 0.00005)
        _TestResult = _TestResult and doc_print_test(rep.rinf, 0.0, "real", 0.00005)
    except (RuntimeError, ValueError):
        _TestResult = False
    except:
        raise
    if not _TestResult:
        sys.stdout.write("matinv_t_r1                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST matinv_t_c1
    #      Complex matrix inverse: singular matrix
    #
    _TestResult = True
    try:
        a = [[1j,-1j],[-2,2]]
        rep = xalglib.cmatrixinverse(a)
        _TestResult = _TestResult and doc_print_test(rep.terminationtype, -3, "int")
        _TestResult = _TestResult and doc_print_test(rep.r1, 0.0, "real", 0.00005)
        _TestResult = _TestResult and doc_print_test(rep.rinf, 0.0, "real", 0.00005)
    except (RuntimeError, ValueError):
        _TestResult = False
    except:
        raise
    if not _TestResult:
        sys.stdout.write("matinv_t_c1                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minlbfgs_d_1
    #      Nonlinear optimization by L-BFGS
    #
    _TestResult = True
    for _spoil_scenario in range(-1,15):
        try:
            #
            # This example demonstrates minimization of
            #
            #     f(x,y) = 100*(x+3)^4+(y-3)^4
            #
            # using LBFGS method, with:
            # * initial point x=[0,0]
            # * unit scale being set for all variables (see minlbfgssetscale for more info)
            # * stopping criteria set to "terminate after short enough step"
            #
            # First, we create optimizer object and tune its properties.
            #
            # IMPORTANT: the  LBFGS  optimizer  supports  parallel parallel numerical
            #            differentiation  ('callback   parallelism').  This  feature,
            #            which  is  present  in  commercial  ALGLIB  editions greatly
            #            accelerates optimization with numerical  differentiation  of
            #            an expensive target functions.
            #
            #            Callback parallelism is usually  beneficial when computing a
            #            numerical gradient requires more than several  milliseconds.
            #            This particular  example,  of  course,  is  not  suited  for
            #            callback parallelism.
            #
            #            See ALGLIB Reference Manual, 'Working with commercial version'
            #            section,  and  comments  on  minlbfgsoptimize() function for
            #            more information.
            #
            x = [0,0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            epsg = 0
            if _spoil_scenario==6:
                epsg = float("nan")
            if _spoil_scenario==7:
                epsg = float("+inf")
            if _spoil_scenario==8:
                epsg = float("-inf")
            epsf = 0
            if _spoil_scenario==9:
                epsf = float("nan")
            if _spoil_scenario==10:
                epsf = float("+inf")
            if _spoil_scenario==11:
                epsf = float("-inf")
            epsx = 0.0000000001
            if _spoil_scenario==12:
                epsx = float("nan")
            if _spoil_scenario==13:
                epsx = float("+inf")
            if _spoil_scenario==14:
                epsx = float("-inf")
            maxits = 0
            state = xalglib.minlbfgscreate(1, x)
            xalglib.minlbfgssetcond(state, epsg, epsf, epsx, maxits)
            xalglib.minlbfgssetscale(state, s)

            #
            # Optimize and examine results.
            #
            xalglib.minlbfgsoptimize_g(state, function1_grad)
            x, rep = xalglib.minlbfgsresults(state)
            _TestResult = _TestResult and doc_print_test(x, [-3,3], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minlbfgs_d_1                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minlbfgs_d_2
    #      Nonlinear optimization with additional settings and restarts
    #
    _TestResult = True
    for _spoil_scenario in range(-1,21):
        try:
            #
            # This example demonstrates minimization of f(x,y) = 100*(x+3)^4+(y-3)^4
            # using LBFGS method.
            #
            # Several advanced techniques are demonstrated:
            # * upper limit on step size
            # * restart from new point
            #
            x = [0,0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            epsg = 0
            if _spoil_scenario==6:
                epsg = float("nan")
            if _spoil_scenario==7:
                epsg = float("+inf")
            if _spoil_scenario==8:
                epsg = float("-inf")
            epsf = 0
            if _spoil_scenario==9:
                epsf = float("nan")
            if _spoil_scenario==10:
                epsf = float("+inf")
            if _spoil_scenario==11:
                epsf = float("-inf")
            epsx = 0.0000000001
            if _spoil_scenario==12:
                epsx = float("nan")
            if _spoil_scenario==13:
                epsx = float("+inf")
            if _spoil_scenario==14:
                epsx = float("-inf")
            stpmax = 0.1
            if _spoil_scenario==15:
                stpmax = float("nan")
            if _spoil_scenario==16:
                stpmax = float("+inf")
            if _spoil_scenario==17:
                stpmax = float("-inf")
            maxits = 0

            # create and tune optimizer
            state = xalglib.minlbfgscreate(1, x)
            xalglib.minlbfgssetcond(state, epsg, epsf, epsx, maxits)
            xalglib.minlbfgssetstpmax(state, stpmax)
            xalglib.minlbfgssetscale(state, s)

            # Set up OptGuard integrity checker which catches errors
            # like nonsmooth targets or errors in the analytic gradient.
            #
            # OptGuard is essential at the early prototyping stages.
            #
            # NOTE: gradient verification needs 3*N additional function
            #       evaluations; DO NOT USE IT IN THE PRODUCTION CODE
            #       because it leads to unnecessary slowdown of your app.
            xalglib.minlbfgsoptguardsmoothness(state)
            xalglib.minlbfgsoptguardgradient(state, 0.001)

            # first run
            xalglib.minlbfgsoptimize_g(state, function1_grad)
            x, rep = xalglib.minlbfgsresults(state)
            _TestResult = _TestResult and doc_print_test(x, [-3,3], "real_vector", 0.005)

            # second run - algorithm is restarted
            x = [10,10]
            if _spoil_scenario==18:
                spoil_vec_by_nan(x)
            if _spoil_scenario==19:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==20:
                spoil_vec_by_neginf(x)
            xalglib.minlbfgsrestartfrom(state, x)
            xalglib.minlbfgsoptimize_g(state, function1_grad)
            x, rep = xalglib.minlbfgsresults(state)
            _TestResult = _TestResult and doc_print_test(x, [-3,3], "real_vector", 0.005)

            # check OptGuard integrity report. Why do we need it at all?
            # Well, try breaking the gradient by adding 1.0 to some
            # of its components - OptGuard should report it as error.
            # And it may also catch unintended errors too :)
            ogrep = xalglib.minlbfgsoptguardresults(state)
            _TestResult = _TestResult and doc_print_test(ogrep.badgradsuspected, False, "bool")
            _TestResult = _TestResult and doc_print_test(ogrep.nonc0suspected, False, "bool")
            _TestResult = _TestResult and doc_print_test(ogrep.nonc1suspected, False, "bool")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minlbfgs_d_2                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minlbfgs_numdiff
    #      Nonlinear optimization by L-BFGS with numerical differentiation
    #
    _TestResult = True
    for _spoil_scenario in range(-1,15):
        try:
            #
            # This example demonstrates minimization of
            #
            #     f(x,y) = 100*(x+3)^4+(y-3)^4
            #
            # using numerical differentiation to calculate gradient.
            #
            # IMPORTANT: the  LBFGS  optimizer  supports  parallel parallel numerical
            #            differentiation  ('callback   parallelism').  This  feature,
            #            which  is  present  in  commercial  ALGLIB  editions greatly
            #            accelerates optimization with numerical  differentiation  of
            #            an expensive target functions.
            #
            #            Callback parallelism is usually  beneficial when computing a
            #            numerical gradient requires more than several  milliseconds.
            #            This particular  example,  of  course,  is  not  suited  for
            #            callback parallelism.
            #
            #            See ALGLIB Reference Manual, 'Working with commercial version'
            #            section,  and  comments  on  minlbfgsoptimize() function for
            #            more information.
            #
            x = [0,0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            epsg = 0.0000000001
            if _spoil_scenario==3:
                epsg = float("nan")
            if _spoil_scenario==4:
                epsg = float("+inf")
            if _spoil_scenario==5:
                epsg = float("-inf")
            epsf = 0
            if _spoil_scenario==6:
                epsf = float("nan")
            if _spoil_scenario==7:
                epsf = float("+inf")
            if _spoil_scenario==8:
                epsf = float("-inf")
            epsx = 0
            if _spoil_scenario==9:
                epsx = float("nan")
            if _spoil_scenario==10:
                epsx = float("+inf")
            if _spoil_scenario==11:
                epsx = float("-inf")
            diffstep = 1.0e-6
            if _spoil_scenario==12:
                diffstep = float("nan")
            if _spoil_scenario==13:
                diffstep = float("+inf")
            if _spoil_scenario==14:
                diffstep = float("-inf")
            maxits = 0

            state = xalglib.minlbfgscreatef(1, x, diffstep)
            xalglib.minlbfgssetcond(state, epsg, epsf, epsx, maxits)
            xalglib.minlbfgsoptimize_f(state, function1_func)
            x, rep = xalglib.minlbfgsresults(state)

            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 4, "int")
            _TestResult = _TestResult and doc_print_test(x, [-3,3], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minlbfgs_numdiff                 FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minlbfgs_t_1
    #      Test buffered results which use shared convention for one of its parameters
    #
    _TestResult = True
    for _spoil_scenario in range(-1,15):
        try:
            x = [0,0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            epsg = 0
            if _spoil_scenario==6:
                epsg = float("nan")
            if _spoil_scenario==7:
                epsg = float("+inf")
            if _spoil_scenario==8:
                epsg = float("-inf")
            epsf = 0
            if _spoil_scenario==9:
                epsf = float("nan")
            if _spoil_scenario==10:
                epsf = float("+inf")
            if _spoil_scenario==11:
                epsf = float("-inf")
            epsx = 0.0000000001
            if _spoil_scenario==12:
                epsx = float("nan")
            if _spoil_scenario==13:
                epsx = float("+inf")
            if _spoil_scenario==14:
                epsx = float("-inf")
            maxits = 0
            state = xalglib.minlbfgscreate(1, x)
            xalglib.minlbfgssetcond(state, epsg, epsf, epsx, maxits)
            xalglib.minlbfgssetscale(state, s)
            xalglib.minlbfgsoptimize_g(state, function1_grad)
            x, rep = xalglib.minlbfgsresults(state)
            x = xalglib.minlbfgsresultsbuf(state, x, rep)
            _TestResult = _TestResult and doc_print_test(x, [-3,3], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minlbfgs_t_1                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minbleic_d_1
    #      Nonlinear optimization with bound constraints
    #
    _TestResult = True
    for _spoil_scenario in range(-1,20):
        try:
            #
            # This example demonstrates minimization of
            #
            #     f(x,y) = 100*(x+3)^4+(y-3)^4
            #
            # subject to box constraints
            #
            #     -1<=x<=+1, -1<=y<=+1
            #
            # using BLEIC optimizer with:
            # * initial point x=[0,0]
            # * unit scale being set for all variables (see minbleicsetscale for more info)
            # * stopping criteria set to "terminate after short enough step"
            # * OptGuard integrity check being used to check problem statement
            #   for some common errors like nonsmoothness or bad analytic gradient
            #
            # First, we create optimizer object and tune its properties:
            # * set box constraints
            # * set variable scales
            # * set stopping criteria
            #
            x = [0,0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            if _spoil_scenario==6:
                spoil_vec_by_deleting_element(s)
            bndl = [-1,-1]
            if _spoil_scenario==7:
                spoil_vec_by_nan(bndl)
            if _spoil_scenario==8:
                spoil_vec_by_deleting_element(bndl)
            bndu = [+1,+1]
            if _spoil_scenario==9:
                spoil_vec_by_nan(bndu)
            if _spoil_scenario==10:
                spoil_vec_by_deleting_element(bndu)
            epsg = 0
            if _spoil_scenario==11:
                epsg = float("nan")
            if _spoil_scenario==12:
                epsg = float("+inf")
            if _spoil_scenario==13:
                epsg = float("-inf")
            epsf = 0
            if _spoil_scenario==14:
                epsf = float("nan")
            if _spoil_scenario==15:
                epsf = float("+inf")
            if _spoil_scenario==16:
                epsf = float("-inf")
            epsx = 0.000001
            if _spoil_scenario==17:
                epsx = float("nan")
            if _spoil_scenario==18:
                epsx = float("+inf")
            if _spoil_scenario==19:
                epsx = float("-inf")
            maxits = 0
            state = xalglib.minbleiccreate(x)
            xalglib.minbleicsetbc(state, bndl, bndu)
            xalglib.minbleicsetscale(state, s)
            xalglib.minbleicsetcond(state, epsg, epsf, epsx, maxits)

            #
            # Then we activate OptGuard integrity checking.
            #
            # OptGuard monitor helps to catch common coding and problem statement
            # issues, like:
            # * discontinuity of the target function (C0 continuity violation)
            # * nonsmoothness of the target function (C1 continuity violation)
            # * erroneous analytic gradient, i.e. one inconsistent with actual
            #   change in the target/constraints
            #
            # OptGuard is essential for early prototyping stages because such
            # problems often result in premature termination of the optimizer
            # which is really hard to distinguish from the correct termination.
            #
            # IMPORTANT: GRADIENT VERIFICATION IS PERFORMED BY MEANS OF NUMERICAL
            #            DIFFERENTIATION. DO NOT USE IT IN PRODUCTION CODE!!!!!!!
            #
            #            Other OptGuard checks add moderate overhead, but anyway
            #            it is better to turn them off when they are not needed.
            #
            xalglib.minbleicoptguardsmoothness(state)
            xalglib.minbleicoptguardgradient(state, 0.001)

            #
            # Optimize and evaluate results
            #
            xalglib.minbleicoptimize_g(state, function1_grad)
            x, rep = xalglib.minbleicresults(state)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 4, "int")
            _TestResult = _TestResult and doc_print_test(x, [-1,1], "real_vector", 0.005)

            #
            # Check that OptGuard did not report errors
            #
            # NOTE: want to test OptGuard? Try breaking the gradient - say, add
            #       1.0 to some of its components.
            #
            ogrep = xalglib.minbleicoptguardresults(state)
            _TestResult = _TestResult and doc_print_test(ogrep.badgradsuspected, False, "bool")
            _TestResult = _TestResult and doc_print_test(ogrep.nonc0suspected, False, "bool")
            _TestResult = _TestResult and doc_print_test(ogrep.nonc1suspected, False, "bool")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minbleic_d_1                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minbleic_d_2
    #      Nonlinear optimization with linear inequality constraints
    #
    _TestResult = True
    for _spoil_scenario in range(-1,22):
        try:
            #
            # This example demonstrates minimization of
            #
            #     f(x,y) = 100*(x+3)^4+(y-3)^4
            #
            # subject to inequality constraints
            #
            # * x>=2 (posed as general linear constraint),
            # * x+y>=6
            #
            # using BLEIC optimizer with
            # * initial point x=[0,0]
            # * unit scale being set for all variables (see minbleicsetscale for more info)
            # * stopping criteria set to "terminate after short enough step"
            # * OptGuard integrity check being used to check problem statement
            #   for some common errors like nonsmoothness or bad analytic gradient
            #
            # First, we create optimizer object and tune its properties:
            # * set linear constraints
            # * set variable scales
            # * set stopping criteria
            #
            x = [5,5]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            if _spoil_scenario==6:
                spoil_vec_by_deleting_element(s)
            c = [[1,0,2],[1,1,6]]
            if _spoil_scenario==7:
                spoil_mat_by_nan(c)
            if _spoil_scenario==8:
                spoil_mat_by_posinf(c)
            if _spoil_scenario==9:
                spoil_mat_by_neginf(c)
            if _spoil_scenario==10:
                spoil_mat_by_deleting_row(c)
            if _spoil_scenario==11:
                spoil_mat_by_deleting_col(c)
            ct = [1,1]
            if _spoil_scenario==12:
                spoil_vec_by_deleting_element(ct)
            epsg = 0
            if _spoil_scenario==13:
                epsg = float("nan")
            if _spoil_scenario==14:
                epsg = float("+inf")
            if _spoil_scenario==15:
                epsg = float("-inf")
            epsf = 0
            if _spoil_scenario==16:
                epsf = float("nan")
            if _spoil_scenario==17:
                epsf = float("+inf")
            if _spoil_scenario==18:
                epsf = float("-inf")
            epsx = 0.000001
            if _spoil_scenario==19:
                epsx = float("nan")
            if _spoil_scenario==20:
                epsx = float("+inf")
            if _spoil_scenario==21:
                epsx = float("-inf")
            maxits = 0

            state = xalglib.minbleiccreate(x)
            xalglib.minbleicsetlc(state, c, ct)
            xalglib.minbleicsetscale(state, s)
            xalglib.minbleicsetcond(state, epsg, epsf, epsx, maxits)

            #
            # Then we activate OptGuard integrity checking.
            #
            # OptGuard monitor helps to catch common coding and problem statement
            # issues, like:
            # * discontinuity of the target function (C0 continuity violation)
            # * nonsmoothness of the target function (C1 continuity violation)
            # * erroneous analytic gradient, i.e. one inconsistent with actual
            #   change in the target/constraints
            #
            # OptGuard is essential for early prototyping stages because such
            # problems often result in premature termination of the optimizer
            # which is really hard to distinguish from the correct termination.
            #
            # IMPORTANT: GRADIENT VERIFICATION IS PERFORMED BY MEANS OF NUMERICAL
            #            DIFFERENTIATION. DO NOT USE IT IN PRODUCTION CODE!!!!!!!
            #
            #            Other OptGuard checks add moderate overhead, but anyway
            #            it is better to turn them off when they are not needed.
            #
            xalglib.minbleicoptguardsmoothness(state)
            xalglib.minbleicoptguardgradient(state, 0.001)

            #
            # Optimize and evaluate results
            #
            xalglib.minbleicoptimize_g(state, function1_grad)
            x, rep = xalglib.minbleicresults(state)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 4, "int")
            _TestResult = _TestResult and doc_print_test(x, [2,4], "real_vector", 0.005)

            #
            # Check that OptGuard did not report errors
            #
            # NOTE: want to test OptGuard? Try breaking the gradient - say, add
            #       1.0 to some of its components.
            #
            ogrep = xalglib.minbleicoptguardresults(state)
            _TestResult = _TestResult and doc_print_test(ogrep.badgradsuspected, False, "bool")
            _TestResult = _TestResult and doc_print_test(ogrep.nonc0suspected, False, "bool")
            _TestResult = _TestResult and doc_print_test(ogrep.nonc1suspected, False, "bool")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minbleic_d_2                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minbleic_numdiff
    #      Nonlinear optimization with bound constraints and numerical differentiation
    #
    _TestResult = True
    for _spoil_scenario in range(-1,23):
        try:
            #
            # This example demonstrates minimization of
            #
            #     f(x,y) = 100*(x+3)^4+(y-3)^4
            #
            # subject to box constraints
            #
            #     -1<=x<=+1, -1<=y<=+1
            #
            # using BLEIC optimizer with:
            # * numerical differentiation being used
            # * initial point x=[0,0]
            # * unit scale being set for all variables (see minbleicsetscale for more info)
            # * stopping criteria set to "terminate after short enough step"
            # * OptGuard integrity check being used to check problem statement
            #   for some common errors like nonsmoothness or bad analytic gradient
            #
            # First, we create optimizer object and tune its properties:
            # * set box constraints
            # * set variable scales
            # * set stopping criteria
            #
            x = [0,0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            if _spoil_scenario==6:
                spoil_vec_by_deleting_element(s)
            bndl = [-1,-1]
            if _spoil_scenario==7:
                spoil_vec_by_nan(bndl)
            if _spoil_scenario==8:
                spoil_vec_by_deleting_element(bndl)
            bndu = [+1,+1]
            if _spoil_scenario==9:
                spoil_vec_by_nan(bndu)
            if _spoil_scenario==10:
                spoil_vec_by_deleting_element(bndu)
            epsg = 0
            if _spoil_scenario==11:
                epsg = float("nan")
            if _spoil_scenario==12:
                epsg = float("+inf")
            if _spoil_scenario==13:
                epsg = float("-inf")
            epsf = 0
            if _spoil_scenario==14:
                epsf = float("nan")
            if _spoil_scenario==15:
                epsf = float("+inf")
            if _spoil_scenario==16:
                epsf = float("-inf")
            epsx = 0.000001
            if _spoil_scenario==17:
                epsx = float("nan")
            if _spoil_scenario==18:
                epsx = float("+inf")
            if _spoil_scenario==19:
                epsx = float("-inf")
            maxits = 0
            diffstep = 1.0e-6
            if _spoil_scenario==20:
                diffstep = float("nan")
            if _spoil_scenario==21:
                diffstep = float("+inf")
            if _spoil_scenario==22:
                diffstep = float("-inf")

            state = xalglib.minbleiccreatef(x, diffstep)
            xalglib.minbleicsetbc(state, bndl, bndu)
            xalglib.minbleicsetscale(state, s)
            xalglib.minbleicsetcond(state, epsg, epsf, epsx, maxits)

            #
            # Then we activate OptGuard integrity checking.
            #
            # Numerical differentiation always produces "correct" gradient
            # (with some truncation error, but unbiased). Thus, we just have
            # to check smoothness properties of the target: C0 and C1 continuity.
            #
            # Sometimes user accidentally tries to solve nonsmooth problems
            # with smooth optimizer. OptGuard helps to detect such situations
            # early, at the prototyping stage.
            #
            xalglib.minbleicoptguardsmoothness(state)

            #
            # Optimize and evaluate results
            #
            xalglib.minbleicoptimize_f(state, function1_func)
            x, rep = xalglib.minbleicresults(state)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 4, "int")
            _TestResult = _TestResult and doc_print_test(x, [-1,1], "real_vector", 0.005)

            #
            # Check that OptGuard did not report errors
            #
            # Want to challenge OptGuard? Try to make your problem
            # nonsmooth by replacing 100*(x+3)^4 by 100*|x+3| and
            # re-run optimizer.
            #
            ogrep = xalglib.minbleicoptguardresults(state)
            _TestResult = _TestResult and doc_print_test(ogrep.nonc0suspected, False, "bool")
            _TestResult = _TestResult and doc_print_test(ogrep.nonc1suspected, False, "bool")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minbleic_numdiff                 FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minqp_d_u1
    #      Unconstrained dense quadratic programming
    #
    _TestResult = True
    for _spoil_scenario in range(-1,10):
        try:
            #
            # This example demonstrates minimization of F(x0,x1) = x0^2 + x1^2 -6*x0 - 4*x1
            #
            # Exact solution is [x0,x1] = [3,2]
            #
            # IMPORTANT: this solver minimizes  following  function:
            #
            #     f(x) = 0.5*x'*A*x + b'*x.
            #
            # Note that quadratic term has 0.5 before it. So if you want to minimize
            # quadratic function, you should rewrite it in such way that quadratic term
            # is multiplied by 0.5 too.
            #
            # For example, our function is f(x)=x0^2+x1^2+..., but we rewrite it as 
            #
            #     f(x) = 0.5*(2*x0^2+2*x1^2) + .... 
            #
            # and pass diag(2,2) as quadratic term - NOT diag(1,1)!
            #
            a = [[2,0],[0,2]]
            if _spoil_scenario==0:
                spoil_mat_by_deleting_row(a)
            if _spoil_scenario==1:
                spoil_mat_by_deleting_col(a)
            b = [-6,-4]
            if _spoil_scenario==2:
                spoil_vec_by_nan(b)
            if _spoil_scenario==3:
                spoil_vec_by_posinf(b)
            if _spoil_scenario==4:
                spoil_vec_by_neginf(b)
            if _spoil_scenario==5:
                spoil_vec_by_deleting_element(b)
            s = [1,1]
            if _spoil_scenario==6:
                spoil_vec_by_nan(s)
            if _spoil_scenario==7:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==8:
                spoil_vec_by_neginf(s)
            if _spoil_scenario==9:
                spoil_vec_by_deleting_element(s)
            isupper = True

            # create the solver, set quadratic/linear terms
            state = xalglib.minqpcreate(2)
            xalglib.minqpsetquadraticterm(state, a, isupper)
            xalglib.minqpsetlinearterm(state, b)

            # Set the scale of the parameters.
            # It is strongly recommended that you set the scale of your variables.
            # Knowing their scales is essential for evaluation of stopping criteria
            # and for preconditioning of the algorithm steps.
            # You can find more information on scaling at http://www.alglib.net/optimization/scaling.php
            #
            # NOTE: for convex problems you may try using minqpsetscaleautodiag()
            #       which automatically determines variable scales.
            xalglib.minqpsetscale(state, s)

            #
            # Solve problem with the sparse interior-point method (sparse IPM) solver.
            #
            # This solver is intended for large-scale sparse problems with box and linear
            # constraints, but it will work on such a toy problem too.
            #
            # Default stopping criteria are used, Newton phase is active.
            #
            xalglib.minqpsetalgosparseipm(state, 0.0)
            xalglib.minqpoptimize(state)
            x, rep = xalglib.minqpresults(state)
            _TestResult = _TestResult and doc_print_test(x, [3,2], "real_vector", 0.005)

            #
            # Solve problem with dense IPM solver.
            #
            # This solver is optimized for problems with dense linear constraints and/or
            # dense quadratic term.
            #
            # Default stopping criteria are used.
            #
            xalglib.minqpsetalgodenseipm(state, 0.0)
            xalglib.minqpoptimize(state)
            x, rep = xalglib.minqpresults(state)
            _TestResult = _TestResult and doc_print_test(x, [3,2], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minqp_d_u1                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minqp_d_bc1
    #      Box constrained dense quadratic programming
    #
    _TestResult = True
    for _spoil_scenario in range(-1,14):
        try:
            #
            # This example demonstrates minimization of F(x0,x1) = x0^2 + x1^2 -6*x0 - 4*x1
            # subject to Box constraints 0<=x0<=2.5, 0<=x1<=2.5
            #
            # Exact solution is [x0,x1] = [2.5,2]
            #
            # IMPORTANT: this solver minimizes  following  function:
            #
            #     f(x) = 0.5*x'*A*x + b'*x.
            #
            # Note that quadratic term has 0.5 before it. So if you want to minimize
            # quadratic function, you should rewrite it in such way that quadratic term
            # is multiplied by 0.5 too.
            #
            # For example, our function is f(x)=x0^2+x1^2+..., but we rewrite it as 
            #
            #     f(x) = 0.5*(2*x0^2+2*x1^2) + ....
            #
            # and pass diag(2,2) as quadratic term - NOT diag(1,1)!
            #
            a = [[2,0],[0,2]]
            if _spoil_scenario==0:
                spoil_mat_by_deleting_row(a)
            if _spoil_scenario==1:
                spoil_mat_by_deleting_col(a)
            b = [-6,-4]
            if _spoil_scenario==2:
                spoil_vec_by_nan(b)
            if _spoil_scenario==3:
                spoil_vec_by_posinf(b)
            if _spoil_scenario==4:
                spoil_vec_by_neginf(b)
            if _spoil_scenario==5:
                spoil_vec_by_deleting_element(b)
            s = [1,1]
            if _spoil_scenario==6:
                spoil_vec_by_nan(s)
            if _spoil_scenario==7:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==8:
                spoil_vec_by_neginf(s)
            if _spoil_scenario==9:
                spoil_vec_by_deleting_element(s)
            bndl = [0.0,0.0]
            if _spoil_scenario==10:
                spoil_vec_by_nan(bndl)
            if _spoil_scenario==11:
                spoil_vec_by_deleting_element(bndl)
            bndu = [2.5,2.5]
            if _spoil_scenario==12:
                spoil_vec_by_nan(bndu)
            if _spoil_scenario==13:
                spoil_vec_by_deleting_element(bndu)
            isupper = True

            # create solver, set quadratic/linear terms
            state = xalglib.minqpcreate(2)
            xalglib.minqpsetquadraticterm(state, a, isupper)
            xalglib.minqpsetlinearterm(state, b)
            xalglib.minqpsetbc(state, bndl, bndu)

            # Set scale of the parameters.
            # It is strongly recommended that you set scale of your variables.
            # Knowing their scales is essential for evaluation of stopping criteria
            # and for preconditioning of the algorithm steps.
            # You can find more information on scaling at http://www.alglib.net/optimization/scaling.php
            #
            # NOTE: for convex problems you may try using minqpsetscaleautodiag()
            #       which automatically determines variable scales.
            xalglib.minqpsetscale(state, s)

            #
            # Solve problem with the sparse interior-point method (sparse IPM) solver.
            #
            # This solver is intended for large-scale sparse problems with box and linear
            # constraints, but it will work on such a toy problem too.
            #
            # Default stopping criteria are used, Newton phase is active.
            #
            xalglib.minqpsetalgosparseipm(state, 0.0)
            xalglib.minqpoptimize(state)
            x, rep = xalglib.minqpresults(state)
            _TestResult = _TestResult and doc_print_test(x, [2.5,2], "real_vector", 0.005)

            #
            # Solve problem with dense IPM solver.
            #
            # This solver is optimized for problems with dense linear constraints and/or
            # dense quadratic term.
            #
            # Default stopping criteria are used.
            #
            xalglib.minqpsetalgodenseipm(state, 0.0)
            xalglib.minqpoptimize(state)
            x, rep = xalglib.minqpresults(state)
            _TestResult = _TestResult and doc_print_test(x, [2.5,2], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minqp_d_bc1                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minqp_d_lc1
    #      Linearly constrained dense quadratic programming
    #
    _TestResult = True
    for _spoil_scenario in range(-1,13):
        try:
            #
            # This example demonstrates minimization of F(x0,x1) = x0^2 + x1^2 -6*x0 - 4*x1
            # subject to linear constraint x0+x1<=2
            #
            # Exact solution is [x0,x1] = [1.5,0.5]
            #
            # IMPORTANT: this solver minimizes  following  function:
            #
            #     f(x) = 0.5*x'*A*x + b'*x.
            #
            # Note that quadratic term has 0.5 before it. So if you want to minimize
            # quadratic function, you should rewrite it in such way that quadratic term
            # is multiplied by 0.5 too.
            #
            # For example, our function is f(x)=x0^2+x1^2+..., but we rewrite it as 
            #
            #     f(x) = 0.5*(2*x0^2+2*x1^2) + ....
            #
            # and pass diag(2,2) as quadratic term - NOT diag(1,1)!
            #
            a = [[2,0],[0,2]]
            if _spoil_scenario==0:
                spoil_mat_by_deleting_row(a)
            if _spoil_scenario==1:
                spoil_mat_by_deleting_col(a)
            b = [-6,-4]
            if _spoil_scenario==2:
                spoil_vec_by_nan(b)
            if _spoil_scenario==3:
                spoil_vec_by_posinf(b)
            if _spoil_scenario==4:
                spoil_vec_by_neginf(b)
            if _spoil_scenario==5:
                spoil_vec_by_deleting_element(b)
            s = [1,1]
            if _spoil_scenario==6:
                spoil_vec_by_nan(s)
            if _spoil_scenario==7:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==8:
                spoil_vec_by_neginf(s)
            if _spoil_scenario==9:
                spoil_vec_by_deleting_element(s)
            c = [[1.0,1.0,2.0]]
            if _spoil_scenario==10:
                spoil_mat_by_nan(c)
            if _spoil_scenario==11:
                spoil_mat_by_posinf(c)
            if _spoil_scenario==12:
                spoil_mat_by_neginf(c)
            ct = [-1]
            isupper = True

            # create solver, set quadratic/linear terms
            state = xalglib.minqpcreate(2)
            xalglib.minqpsetquadraticterm(state, a, isupper)
            xalglib.minqpsetlinearterm(state, b)
            xalglib.minqpsetlc(state, c, ct)

            # Set scale of the parameters.
            # It is strongly recommended that you set scale of your variables.
            # Knowing their scales is essential for evaluation of stopping criteria
            # and for preconditioning of the algorithm steps.
            # You can find more information on scaling at http://www.alglib.net/optimization/scaling.php
            #
            # NOTE: for convex problems you may try using minqpsetscaleautodiag()
            #       which automatically determines variable scales.
            xalglib.minqpsetscale(state, s)

            #
            # Solve problem with the sparse interior-point method (sparse IPM) solver.
            #
            # This solver is intended for large-scale sparse problems with box and linear
            # constraints, but it will work on such a toy problem too.
            #
            # Default stopping criteria are used, Newton phase is active.
            #
            xalglib.minqpsetalgosparseipm(state, 0.0)
            xalglib.minqpoptimize(state)
            x, rep = xalglib.minqpresults(state)
            _TestResult = _TestResult and doc_print_test(x, [1.5,0.5], "real_vector", 0.005)

            #
            # Solve problem with dense IPM solver.
            #
            # This solver is optimized for problems with dense linear constraints and/or
            # dense quadratic term.
            #
            # Default stopping criteria are used.
            #
            xalglib.minqpsetalgodenseipm(state, 0.0)
            xalglib.minqpoptimize(state)
            x, rep = xalglib.minqpresults(state)
            _TestResult = _TestResult and doc_print_test(x, [1.5,0.5], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minqp_d_lc1                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minqp_d_u2
    #      Unconstrained sparse quadratic programming
    #
    _TestResult = True
    for _spoil_scenario in range(-1,12):
        try:
            #
            # This example demonstrates minimization of F(x0,x1) = x0^2 + x1^2 -6*x0 - 4*x1,
            # with quadratic term given by sparse matrix structure.
            #
            # Exact solution is [x0,x1] = [3,2]
            #
            # We provide algorithm with starting point, although in this case
            # (dense matrix, no constraints) it can work without such information.
            #
            # IMPORTANT: this solver minimizes  following  function:
            #     f(x) = 0.5*x'*A*x + b'*x.
            # Note that quadratic term has 0.5 before it. So if you want to minimize
            # quadratic function, you should rewrite it in such way that quadratic term
            # is multiplied by 0.5 too.
            #
            # For example, our function is f(x)=x0^2+x1^2+..., but we rewrite it as 
            #     f(x) = 0.5*(2*x0^2+2*x1^2) + ....
            # and pass diag(2,2) as quadratic term - NOT diag(1,1)!
            #
            b = [-6,-4]
            if _spoil_scenario==0:
                spoil_vec_by_nan(b)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(b)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(b)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(b)
            x0 = [0,1]
            if _spoil_scenario==4:
                spoil_vec_by_nan(x0)
            if _spoil_scenario==5:
                spoil_vec_by_posinf(x0)
            if _spoil_scenario==6:
                spoil_vec_by_neginf(x0)
            if _spoil_scenario==7:
                spoil_vec_by_deleting_element(x0)
            s = [1,1]
            if _spoil_scenario==8:
                spoil_vec_by_nan(s)
            if _spoil_scenario==9:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==10:
                spoil_vec_by_neginf(s)
            if _spoil_scenario==11:
                spoil_vec_by_deleting_element(s)

            # initialize sparsematrix structure
            a = xalglib.sparsecreate(2, 2, 0)
            xalglib.sparseset(a, 0, 0, 2.0)
            xalglib.sparseset(a, 1, 1, 2.0)

            # create solver, set quadratic/linear terms
            state = xalglib.minqpcreate(2)
            xalglib.minqpsetquadratictermsparse(state, a, True)
            xalglib.minqpsetlinearterm(state, b)
            xalglib.minqpsetstartingpoint(state, x0)

            # Set scale of the parameters.
            # It is strongly recommended that you set scale of your variables.
            # Knowing their scales is essential for evaluation of stopping criteria
            # and for preconditioning of the algorithm steps.
            # You can find more information on scaling at http://www.alglib.net/optimization/scaling.php
            #
            # NOTE: for convex problems you may try using minqpsetscaleautodiag()
            #       which automatically determines variable scales.
            xalglib.minqpsetscale(state, s)

            #
            # Solve problem with the sparse interior-point method (sparse IPM) solver.
            #
            # This solver is intended for large-scale sparse problems with box and linear
            # constraints, but it will work on such a toy problem too.
            #
            # Default stopping criteria are used, Newton phase is active.
            #
            xalglib.minqpsetalgosparseipm(state, 0.0)
            xalglib.minqpoptimize(state)
            x, rep = xalglib.minqpresults(state)
            _TestResult = _TestResult and doc_print_test(x, [3,2], "real_vector", 0.005)

            #
            # Solve problem with dense IPM solver.
            #
            # This solver is optimized for problems with dense linear constraints and/or
            # dense quadratic term.
            #
            # Default stopping criteria are used.
            #
            xalglib.minqpsetalgodenseipm(state, 0.0)
            xalglib.minqpoptimize(state)
            x, rep = xalglib.minqpresults(state)
            _TestResult = _TestResult and doc_print_test(x, [3,2], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minqp_d_u2                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minqp_d_nonconvex
    #      Nonconvex quadratic programming
    #
    _TestResult = True
    for _spoil_scenario in range(-1,18):
        try:
            #
            # This example demonstrates minimization of nonconvex function
            #     F(x0,x1) = -(x0^2+x1^2)
            # subject to constraints x0,x1 in [1.0,2.0]
            # Exact solution is [x0,x1] = [2,2].
            #
            # Non-convex problems are harder to solve than convex ones, and they
            # may have more than one local minimum. However, ALGLIB solvers may deal
            # with such problems (although they do not guarantee convergence to
            # global minimum).
            #
            # IMPORTANT: this solver minimizes  following  function:
            #     f(x) = 0.5*x'*A*x + b'*x.
            # Note that quadratic term has 0.5 before it. So if you want to minimize
            # quadratic function, you should rewrite it in such way that quadratic term
            # is multiplied by 0.5 too.
            #
            # For example, our function is f(x)=-(x0^2+x1^2), but we rewrite it as 
            #     f(x) = 0.5*(-2*x0^2-2*x1^2)
            # and pass diag(-2,-2) as quadratic term - NOT diag(-1,-1)!
            #
            a = [[-2,0],[0,-2]]
            if _spoil_scenario==0:
                spoil_mat_by_deleting_row(a)
            if _spoil_scenario==1:
                spoil_mat_by_deleting_col(a)
            x0 = [1,1]
            if _spoil_scenario==2:
                spoil_vec_by_nan(x0)
            if _spoil_scenario==3:
                spoil_vec_by_posinf(x0)
            if _spoil_scenario==4:
                spoil_vec_by_neginf(x0)
            if _spoil_scenario==5:
                spoil_vec_by_deleting_element(x0)
            s = [1,1]
            if _spoil_scenario==6:
                spoil_vec_by_nan(s)
            if _spoil_scenario==7:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==8:
                spoil_vec_by_neginf(s)
            if _spoil_scenario==9:
                spoil_vec_by_deleting_element(s)
            bndl = [1.0,1.0]
            if _spoil_scenario==10:
                spoil_vec_by_nan(bndl)
            if _spoil_scenario==11:
                spoil_vec_by_deleting_element(bndl)
            bndu = [2.0,2.0]
            if _spoil_scenario==12:
                spoil_vec_by_nan(bndu)
            if _spoil_scenario==13:
                spoil_vec_by_deleting_element(bndu)
            isupper = True

            # create solver, set quadratic/linear terms, constraints
            state = xalglib.minqpcreate(2)
            xalglib.minqpsetquadraticterm(state, a, isupper)
            xalglib.minqpsetstartingpoint(state, x0)
            xalglib.minqpsetbc(state, bndl, bndu)

            # Set scale of the parameters.
            # It is strongly recommended that you set scale of your variables.
            # Knowing their scales is essential for evaluation of stopping criteria
            # and for preconditioning of the algorithm steps.
            # You can find more information on scaling at http://www.alglib.net/optimization/scaling.php
            #
            # NOTE: there also exists minqpsetscaleautodiag() function
            #       which automatically determines variable scales; however,
            #       it does NOT work for non-convex problems.
            xalglib.minqpsetscale(state, s)

            #
            # Solve problem with BLEIC-based QP solver.
            #
            # This solver is intended for problems with moderate (up to 50) number
            # of general linear constraints and unlimited number of box constraints.
            # It may solve non-convex problems as long as they are bounded from
            # below under constraints.
            #
            # Default stopping criteria are used.
            #
            xalglib.minqpsetalgobleic(state, 0.0, 0.0, 0.0, 0)
            xalglib.minqpoptimize(state)
            x, rep = xalglib.minqpresults(state)
            _TestResult = _TestResult and doc_print_test(x, [2,2], "real_vector", 0.005)

            #
            # Solve problem with DENSE-AUL solver.
            #
            # This solver is optimized for nonconvex problems with up to several thousands of
            # variables and large amount of general linear constraints. Problems with
            # less than 50 general linear constraints can be efficiently solved with
            # BLEIC, problems with box-only constraints can be solved with QuickQP.
            # However, DENSE-AUL will work in any (including unconstrained) case.
            #
            # Default stopping criteria are used.
            #
            xalglib.minqpsetalgodenseaul(state, 1.0e-9, 1.0e+4, 5)
            xalglib.minqpoptimize(state)
            x, rep = xalglib.minqpresults(state)
            _TestResult = _TestResult and doc_print_test(x, [2,2], "real_vector", 0.005)

            # Hmm... this problem is bounded from below (has solution) only under constraints.
            # What it we remove them?
            #
            # You may see that BLEIC algorithm detects unboundedness of the problem, 
            # -4 is returned as completion code. However, DENSE-AUL is unable to detect
            # such situation and it will cycle forever (we do not test it here).
            nobndl = [-float("inf"),-float("inf")]
            if _spoil_scenario==14:
                spoil_vec_by_nan(nobndl)
            if _spoil_scenario==15:
                spoil_vec_by_deleting_element(nobndl)
            nobndu = [float("inf"),float("inf")]
            if _spoil_scenario==16:
                spoil_vec_by_nan(nobndu)
            if _spoil_scenario==17:
                spoil_vec_by_deleting_element(nobndu)
            xalglib.minqpsetbc(state, nobndl, nobndu)
            xalglib.minqpsetalgobleic(state, 0.0, 0.0, 0.0, 0)
            xalglib.minqpoptimize(state)
            x, rep = xalglib.minqpresults(state)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, -4, "int")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minqp_d_nonconvex                FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minlm_d_v
    #      Nonlinear least squares optimization using function vector only
    #
    _TestResult = True
    for _spoil_scenario in range(-1,9):
        try:
            #
            # This example demonstrates minimization of F(x0,x1) = f0^2+f1^2, where 
            #
            #     f0(x0,x1) = 10*(x0+3)^2
            #     f1(x0,x1) = (x1-3)^2
            #
            # using "V" mode of the Levenberg-Marquardt optimizer (function values only,
            # no Jacobian information). The optimization algorithm uses function vector
            #
            #     f[] = {f1,f2}
            #
            # No other information (Jacobian, gradient, etc.) is needed.
            #
            # IMPORTANT: the  MINLM  optimizer  supports  parallel parallel numerical
            #            differentiation  ('callback   parallelism').  This  feature,
            #            which  is present  in  commercial  ALGLIB  editions, greatly
            #            accelerates optimization with numerical  differentiation  of
            #            an expensive target functions.
            #
            #            Callback parallelism is usually  beneficial when computing a
            #            numerical gradient requires more than several  milliseconds.
            #            This particular  example,  of  course,  is  not  suited  for
            #            callback parallelism.
            #
            #            If  you  solve  a  curve fitting problem, i.e. the  function
            #            vector is actually the same function computed  at  different
            #            points of a data points space, then it may be better to  use
            #            an LSFIT curve fitting solver, which offers more fine-grained
            #            parallelism due to knowledge of the  problem  structure.  In
            #            particular, it can accelerate both numerical differentiation
            #            and problems with user-supplied gradients.
            #
            #            See ALGLIB Reference Manual, 'Working with commercial version'
            #            section,  and  comments  on   minlmoptimize()  function  for
            #            more information.
            #
            x = [0,0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            epsx = 0.0000000001
            if _spoil_scenario==6:
                epsx = float("nan")
            if _spoil_scenario==7:
                epsx = float("+inf")
            if _spoil_scenario==8:
                epsx = float("-inf")
            maxits = 0

            #
            # Create optimizer, tell it to:
            # * use numerical differentiation with step equal to 0.0001
            # * use unit scale for all variables (s is a unit vector)
            # * stop after short enough step (less than epsx)
            #
            state = xalglib.minlmcreatev(2, x, 0.0001)
            xalglib.minlmsetcond(state, epsx, maxits)
            xalglib.minlmsetscale(state, s)

            #
            # Optimize
            #
            xalglib.minlmoptimize_v(state, function1_fvec)

            #
            # Test optimization results
            #
            # NOTE: because we use numerical differentiation, we do not
            #       verify Jacobian correctness - it is always "correct".
            #       However, if you switch to analytic gradient, consider
            #       checking it with OptGuard (see other examples).
            #
            x, rep = xalglib.minlmresults(state)
            _TestResult = _TestResult and doc_print_test(x, [-3,+3], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minlm_d_v                        FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minlm_d_vj
    #      Nonlinear least squares optimization using function vector and Jacobian
    #
    _TestResult = True
    for _spoil_scenario in range(-1,9):
        try:
            #
            # This example demonstrates minimization of F(x0,x1) = f0^2+f1^2, where 
            #
            #     f0(x0,x1) = 10*(x0+3)^2
            #     f1(x0,x1) = (x1-3)^2
            #
            # using "VJ" mode of the Levenberg-Marquardt optimizer.  The optimization
            # algorithm uses the  function  vector  f[] = {f1,f2}  and  the  Jacobian
            # matrix J = {dfi/dxj}, both of them provided by user.
            #
            # IMPORTANT: the   MINLM   optimizer  supports     parallel     numerical
            #            differentiation  ('callback   parallelism').  This  feature,
            #            which  is present  in  commercial  ALGLIB  editions, greatly
            #            accelerates optimization with numerical  differentiation  of
            #            an expensive target functions.
            #
            #            Callback parallelism is usually  beneficial when computing a
            #            numerical gradient requires more than several  milliseconds.
            #            This particular  example,  of  course,  is  not  suited  for
            #            callback parallelism.
            #
            #            If  you  solve  a  curve fitting problem, i.e. the  function
            #            vector is actually the same function computed  at  different
            #            points of a data points space, then it may be better to  use
            #            an LSFIT curve fitting solver, which offers more fine-grained
            #            parallelism due to knowledge of the  problem  structure.  In
            #            particular, it can accelerate both numerical differentiation
            #            and problems with user-supplied gradients.
            #
            #            See ALGLIB Reference Manual, 'Working with commercial version'
            #            section,  and  comments  on   minlmoptimize()  function  for
            #            more information.
            #
            x = [0,0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            epsx = 0.0000000001
            if _spoil_scenario==6:
                epsx = float("nan")
            if _spoil_scenario==7:
                epsx = float("+inf")
            if _spoil_scenario==8:
                epsx = float("-inf")
            maxits = 0

            #
            # Create optimizer, tell it to:
            # * use analytic gradient provided by user
            # * use unit scale for all variables (s is a unit vector)
            # * stop after short enough step (less than epsx)
            #
            state = xalglib.minlmcreatevj(2, x)
            xalglib.minlmsetcond(state, epsx, maxits)
            xalglib.minlmsetscale(state, s)

            #
            # Optimize
            #
            xalglib.minlmoptimize_vj(state, function1_fvec, function1_jac)

            #
            # Test optimization results
            #
            x, rep = xalglib.minlmresults(state)
            _TestResult = _TestResult and doc_print_test(x, [-3,+3], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minlm_d_vj                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minlm_d_vb
    #      Bound constrained nonlinear least squares optimization
    #
    _TestResult = True
    for _spoil_scenario in range(-1,13):
        try:
            #
            # This example demonstrates minimization of F(x0,x1) = f0^2+f1^2, where 
            #
            #     f0(x0,x1) = 10*(x0+3)^2
            #     f1(x0,x1) = (x1-3)^2
            #
            # with box constraints
            #
            #     -1 <= x0 <= +1
            #     -1 <= x1 <= +1
            #
            # using "V" mode of the Levenberg-Marquardt optimizer.  The  optimization
            # algorithm uses function  vector  f[] = {f1,f2}.  No  other  information
            # (Jacobian, gradient, etc.) is needed.
            #
            # IMPORTANT: the  MINLM  optimizer  supports  parallel parallel numerical
            #            differentiation  ('callback   parallelism').  This  feature,
            #            which  is present  in  commercial  ALGLIB  editions, greatly
            #            accelerates optimization with numerical  differentiation  of
            #            an expensive target functions.
            #
            #            Callback parallelism is usually  beneficial when computing a
            #            numerical gradient requires more than several  milliseconds.
            #            This particular  example,  of  course,  is  not  suited  for
            #            callback parallelism.
            #
            #            If  you  solve  a  curve fitting problem, i.e. the  function
            #            vector is actually the same function computed  at  different
            #            points of a data points space, then it may be better to  use
            #            an LSFIT curve fitting solver, which offers more fine-grained
            #            parallelism due to knowledge of the  problem  structure.  In
            #            particular, it can accelerate both numerical differentiation
            #            and problems with user-supplied gradients.
            #
            #            See ALGLIB Reference Manual, 'Working with commercial version'
            #            section,  and  comments  on   minlmoptimize()  function  for
            #            more information.
            #
            x = [0,0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            bndl = [-1,-1]
            if _spoil_scenario==6:
                spoil_vec_by_nan(bndl)
            if _spoil_scenario==7:
                spoil_vec_by_deleting_element(bndl)
            bndu = [+1,+1]
            if _spoil_scenario==8:
                spoil_vec_by_nan(bndu)
            if _spoil_scenario==9:
                spoil_vec_by_deleting_element(bndu)
            epsx = 0.0000000001
            if _spoil_scenario==10:
                epsx = float("nan")
            if _spoil_scenario==11:
                epsx = float("+inf")
            if _spoil_scenario==12:
                epsx = float("-inf")
            maxits = 0

            #
            # Create optimizer, tell it to:
            # * use numerical differentiation with step equal to 1.0
            # * use unit scale for all variables (s is a unit vector)
            # * stop after short enough step (less than epsx)
            # * set box constraints
            #
            state = xalglib.minlmcreatev(2, x, 0.0001)
            xalglib.minlmsetbc(state, bndl, bndu)
            xalglib.minlmsetcond(state, epsx, maxits)
            xalglib.minlmsetscale(state, s)

            #
            # Optimize
            #
            xalglib.minlmoptimize_v(state, function1_fvec)

            #
            # Test optimization results
            #
            # NOTE: because we use numerical differentiation, we do not
            #       verify Jacobian correctness - it is always "correct".
            #       However, if you switch to analytic gradient, consider
            #       checking it with OptGuard (see other examples).
            #
            x, rep = xalglib.minlmresults(state)
            _TestResult = _TestResult and doc_print_test(x, [-1,+1], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minlm_d_vb                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minlm_d_restarts
    #      Efficient restarts of LM optimizer
    #
    _TestResult = True
    for _spoil_scenario in range(-1,9):
        try:
            #
            # This example demonstrates minimization of F(x0,x1) = f0^2+f1^2, where 
            #
            #     f0(x0,x1) = 10*(x0+3)^2
            #     f1(x0,x1) = (x1-3)^2
            #
            # using several starting points and efficient restarts.
            #
            epsx = 0.0000000001
            if _spoil_scenario==0:
                epsx = float("nan")
            if _spoil_scenario==1:
                epsx = float("+inf")
            if _spoil_scenario==2:
                epsx = float("-inf")
            maxits = 0

            #
            # create optimizer using minlmcreatev()
            #
            x = [10,10]
            if _spoil_scenario==3:
                spoil_vec_by_nan(x)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(x)
            state = xalglib.minlmcreatev(2, x, 0.0001)
            xalglib.minlmsetcond(state, epsx, maxits)
            xalglib.minlmoptimize_v(state, function1_fvec)
            x, rep = xalglib.minlmresults(state)
            _TestResult = _TestResult and doc_print_test(x, [-3,+3], "real_vector", 0.005)

            #
            # restart optimizer using minlmrestartfrom()
            #
            # we can use different starting point, different function,
            # different stopping conditions, but the problem size
            # must remain unchanged.
            #
            x = [4,4]
            if _spoil_scenario==6:
                spoil_vec_by_nan(x)
            if _spoil_scenario==7:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==8:
                spoil_vec_by_neginf(x)
            xalglib.minlmrestartfrom(state, x)
            xalglib.minlmoptimize_v(state, function2_fvec)
            x, rep = xalglib.minlmresults(state)
            _TestResult = _TestResult and doc_print_test(x, [0,1], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minlm_d_restarts                 FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST mincg_d_1
    #      Nonlinear optimization by CG
    #
    _TestResult = True
    for _spoil_scenario in range(-1,15):
        try:
            #
            # This example demonstrates minimization of
            #
            #     f(x,y) = 100*(x+3)^4+(y-3)^4
            #
            # using nonlinear conjugate gradient method with:
            # * initial point x=[0,0]
            # * unit scale being set for all variables (see mincgsetscale for more info)
            # * stopping criteria set to "terminate after short enough step"
            # * OptGuard integrity check being used to check problem statement
            #   for some common errors like nonsmoothness or bad analytic gradient
            #
            # First, we create optimizer object and tune its properties
            #
            x = [0,0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            epsg = 0
            if _spoil_scenario==6:
                epsg = float("nan")
            if _spoil_scenario==7:
                epsg = float("+inf")
            if _spoil_scenario==8:
                epsg = float("-inf")
            epsf = 0
            if _spoil_scenario==9:
                epsf = float("nan")
            if _spoil_scenario==10:
                epsf = float("+inf")
            if _spoil_scenario==11:
                epsf = float("-inf")
            epsx = 0.0000000001
            if _spoil_scenario==12:
                epsx = float("nan")
            if _spoil_scenario==13:
                epsx = float("+inf")
            if _spoil_scenario==14:
                epsx = float("-inf")
            maxits = 0
            state = xalglib.mincgcreate(x)
            xalglib.mincgsetcond(state, epsg, epsf, epsx, maxits)
            xalglib.mincgsetscale(state, s)

            #
            # Activate OptGuard integrity checking.
            #
            # OptGuard monitor helps to catch common coding and problem statement
            # issues, like:
            # * discontinuity of the target function (C0 continuity violation)
            # * nonsmoothness of the target function (C1 continuity violation)
            # * erroneous analytic gradient, i.e. one inconsistent with actual
            #   change in the target/constraints
            #
            # OptGuard is essential for early prototyping stages because such
            # problems often result in premature termination of the optimizer
            # which is really hard to distinguish from the correct termination.
            #
            # IMPORTANT: GRADIENT VERIFICATION IS PERFORMED BY MEANS OF NUMERICAL
            #            DIFFERENTIATION. DO NOT USE IT IN PRODUCTION CODE!!!!!!!
            #
            #            Other OptGuard checks add moderate overhead, but anyway
            #            it is better to turn them off when they are not needed.
            #
            xalglib.mincgoptguardsmoothness(state)
            xalglib.mincgoptguardgradient(state, 0.001)

            #
            # Optimize and evaluate results
            #
            xalglib.mincgoptimize_g(state, function1_grad)
            x, rep = xalglib.mincgresults(state)
            _TestResult = _TestResult and doc_print_test(x, [-3,3], "real_vector", 0.005)

            #
            # Check that OptGuard did not report errors
            #
            # NOTE: want to test OptGuard? Try breaking the gradient - say, add
            #       1.0 to some of its components.
            #
            ogrep = xalglib.mincgoptguardresults(state)
            _TestResult = _TestResult and doc_print_test(ogrep.badgradsuspected, False, "bool")
            _TestResult = _TestResult and doc_print_test(ogrep.nonc0suspected, False, "bool")
            _TestResult = _TestResult and doc_print_test(ogrep.nonc1suspected, False, "bool")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("mincg_d_1                        FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST mincg_d_2
    #      Nonlinear optimization with additional settings and restarts
    #
    _TestResult = True
    for _spoil_scenario in range(-1,21):
        try:
            #
            # This example demonstrates minimization of f(x,y) = 100*(x+3)^4+(y-3)^4
            # with nonlinear conjugate gradient method.
            #
            # Several advanced techniques are demonstrated:
            # * upper limit on step size
            # * restart from new point
            #
            x = [0,0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            epsg = 0
            if _spoil_scenario==6:
                epsg = float("nan")
            if _spoil_scenario==7:
                epsg = float("+inf")
            if _spoil_scenario==8:
                epsg = float("-inf")
            epsf = 0
            if _spoil_scenario==9:
                epsf = float("nan")
            if _spoil_scenario==10:
                epsf = float("+inf")
            if _spoil_scenario==11:
                epsf = float("-inf")
            epsx = 0.0000000001
            if _spoil_scenario==12:
                epsx = float("nan")
            if _spoil_scenario==13:
                epsx = float("+inf")
            if _spoil_scenario==14:
                epsx = float("-inf")
            stpmax = 0.1
            if _spoil_scenario==15:
                stpmax = float("nan")
            if _spoil_scenario==16:
                stpmax = float("+inf")
            if _spoil_scenario==17:
                stpmax = float("-inf")
            maxits = 0

            # create and tune optimizer
            state = xalglib.mincgcreate(x)
            xalglib.mincgsetscale(state, s)
            xalglib.mincgsetcond(state, epsg, epsf, epsx, maxits)
            xalglib.mincgsetstpmax(state, stpmax)

            # Set up OptGuard integrity checker which catches errors
            # like nonsmooth targets or errors in the analytic gradient.
            #
            # OptGuard is essential at the early prototyping stages.
            #
            # NOTE: gradient verification needs 3*N additional function
            #       evaluations; DO NOT USE IT IN THE PRODUCTION CODE
            #       because it leads to unnecessary slowdown of your app.
            xalglib.mincgoptguardsmoothness(state)
            xalglib.mincgoptguardgradient(state, 0.001)

            # first run
            xalglib.mincgoptimize_g(state, function1_grad)
            x, rep = xalglib.mincgresults(state)
            _TestResult = _TestResult and doc_print_test(x, [-3,3], "real_vector", 0.005)

            # second run - algorithm is restarted with mincgrestartfrom()
            x = [10,10]
            if _spoil_scenario==18:
                spoil_vec_by_nan(x)
            if _spoil_scenario==19:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==20:
                spoil_vec_by_neginf(x)
            xalglib.mincgrestartfrom(state, x)
            xalglib.mincgoptimize_g(state, function1_grad)
            x, rep = xalglib.mincgresults(state)
            _TestResult = _TestResult and doc_print_test(x, [-3,3], "real_vector", 0.005)

            # check OptGuard integrity report. Why do we need it at all?
            # Well, try breaking the gradient by adding 1.0 to some
            # of its components - OptGuard should report it as error.
            # And it may also catch unintended errors too :)
            ogrep = xalglib.mincgoptguardresults(state)
            _TestResult = _TestResult and doc_print_test(ogrep.badgradsuspected, False, "bool")
            _TestResult = _TestResult and doc_print_test(ogrep.nonc0suspected, False, "bool")
            _TestResult = _TestResult and doc_print_test(ogrep.nonc1suspected, False, "bool")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("mincg_d_2                        FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST mincg_numdiff
    #      Nonlinear optimization by CG with numerical differentiation
    #
    _TestResult = True
    for _spoil_scenario in range(-1,18):
        try:
            #
            # This example demonstrates minimization of
            #
            #     f(x,y) = 100*(x+3)^4+(y-3)^4
            #
            # using numerical differentiation to calculate gradient.
            #
            # We also show how to use OptGuard integrity checker to catch common
            # problem statement errors like accidentally specifying nonsmooth target
            # function.
            #
            # First, we set up optimizer...
            #
            x = [0,0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            epsg = 0
            if _spoil_scenario==6:
                epsg = float("nan")
            if _spoil_scenario==7:
                epsg = float("+inf")
            if _spoil_scenario==8:
                epsg = float("-inf")
            epsf = 0
            if _spoil_scenario==9:
                epsf = float("nan")
            if _spoil_scenario==10:
                epsf = float("+inf")
            if _spoil_scenario==11:
                epsf = float("-inf")
            epsx = 0.0000000001
            if _spoil_scenario==12:
                epsx = float("nan")
            if _spoil_scenario==13:
                epsx = float("+inf")
            if _spoil_scenario==14:
                epsx = float("-inf")
            diffstep = 1.0e-6
            if _spoil_scenario==15:
                diffstep = float("nan")
            if _spoil_scenario==16:
                diffstep = float("+inf")
            if _spoil_scenario==17:
                diffstep = float("-inf")
            maxits = 0
            state = xalglib.mincgcreatef(x, diffstep)
            xalglib.mincgsetcond(state, epsg, epsf, epsx, maxits)
            xalglib.mincgsetscale(state, s)

            #
            # Then, we activate OptGuard integrity checking.
            #
            # Numerical differentiation always produces "correct" gradient
            # (with some truncation error, but unbiased). Thus, we just have
            # to check smoothness properties of the target: C0 and C1 continuity.
            #
            # Sometimes user accidentally tried to solve nonsmooth problems
            # with smooth optimizer. OptGuard helps to detect such situations
            # early, at the prototyping stage.
            #
            xalglib.mincgoptguardsmoothness(state)

            #
            # Now we are ready to run the optimization
            #
            xalglib.mincgoptimize_f(state, function1_func)
            x, rep = xalglib.mincgresults(state)
            _TestResult = _TestResult and doc_print_test(x, [-3,3], "real_vector", 0.005)

            #
            # ...and to check OptGuard integrity report.
            #
            # Want to challenge OptGuard? Try to make your problem
            # nonsmooth by replacing 100*(x+3)^4 by 100*|x+3| and
            # re-run optimizer.
            #
            ogrep = xalglib.mincgoptguardresults(state)
            _TestResult = _TestResult and doc_print_test(ogrep.nonc0suspected, False, "bool")
            _TestResult = _TestResult and doc_print_test(ogrep.nonc1suspected, False, "bool")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("mincg_numdiff                    FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST mindf_gdemo_auto
    #      Nonlinearly constrained differential evolution
    #
    _TestResult = True
    for _spoil_scenario in range(-1,18):
        try:
            #
            # This example demonstrates minimization of
            #
            #     f(x0,x1) = x0+x1
            #
            # subject to nonlinear constraints
            #
            #    x0^2 + x1^2 - 1 <= 0
            #    x2-exp(x0) = 0
            #
            x0 = [0,0,0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x0)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x0)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x0)
            s = [1,1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)

            #
            # Create optimizer object
            #
            state = xalglib.mindfcreate(x0)
            xalglib.mindfsetscale(state, s)

            #
            # Choose  one  of  nonlinear  programming  solvers  supported  by  MINDF
            # optimizer.
            #
            # This example shows how to use GDEMO (Generalized Differential Evolution,
            # MultiObjective) solver working in a single-objective mode. This solver
            # uses an adaptive choice of DE parameters (crossover, weight and strategy),
            # automatically choosing the most appropriate settings during the optimization.
            #
            # Thus, the only tunable parameters are iterations count and population size.
            # The latter one can be omitted, the solver will use a default size in this
            # case.
            #
            maxits = 200
            xalglib.mindfsetalgogdemo(state, maxits)

            #
            # Set nonlinear constraints.
            #
            # ALGLIB  supports  any  combination  of  box,  linear  and  nonlinear
            # constraints. This specific example uses only nonlinear ones.
            #
            # Since  version  4.01,  ALGLIB  supports  the  most  general  form of
            # nonlinear constraints: two-sided   constraints  NL<=C(x)<=NU,   with
            # elements being possibly infinite (means that this specific bound  is
            # ignored). It includes equality constraints,  upper/lower  inequality
            # constraints, range constraints. In particular, a pair of constraints
            #
            #        x2-exp(x0)       = 0
            #        x0^2 + x1^2 - 1 <= 0
            #
            # can be specified by passing NL=[0,-INF], NU=[0,0] to mindfsetnlc2().
            # Constraining functions themselves are passed as a part  of a problem
            # target vector (see below).
            #
            #
            # Unlike smooth optimizers like SQP which naturally include linear and
            # nonlinear constraints into the  algorithm,  derivative-free  methods
            # often need special strategies to deal with them, with each  strategy
            # having its own limitations:
            #
            # * an L2 penalty,  which  has  good  global  constraint   enforcement
            #   properties, but usually allows some moderate constraint violation
            #
            # * an L1 penalty, which has potential to enforce constraints exactly,
            #   but has somewhat weaker ability to move iterations from  far  away
            #   points closer to the feasible area. It also  has  somewhat  harder
            #   numerical properties, needing more iterations to converge.
            #
            # * a combined L1/L2 penalty, which is a good compromise
            #
            # The code below sets constraints bounds and tells the solver  to  use
            # a mixed L1/L2 penalized strategy.
            #
            # NOTE: box constraints require no special handling.
            #
            nl = [0,-float("inf")]
            if _spoil_scenario==6:
                spoil_vec_by_nan(nl)
            if _spoil_scenario==7:
                spoil_vec_by_adding_element(nl)
            if _spoil_scenario==8:
                spoil_vec_by_deleting_element(nl)
            nu = [0,0]
            if _spoil_scenario==9:
                spoil_vec_by_nan(nu)
            if _spoil_scenario==10:
                spoil_vec_by_adding_element(nu)
            if _spoil_scenario==11:
                spoil_vec_by_deleting_element(nu)
            rho1 = 5
            if _spoil_scenario==12:
                rho1 = float("nan")
            if _spoil_scenario==13:
                rho1 = float("+inf")
            if _spoil_scenario==14:
                rho1 = float("-inf")
            rho2 = 5
            if _spoil_scenario==15:
                rho2 = float("nan")
            if _spoil_scenario==16:
                rho2 = float("+inf")
            if _spoil_scenario==17:
                rho2 = float("-inf")
            xalglib.mindfsetnlc2(state, nl, nu)
            xalglib.mindfsetgdemopenalty(state, rho1, rho2)

            #
            # Optimize and test results.
            #
            # The optimizer object accepts vector function  with  its  first  component
            # being a target and subsequent components being nonlinear constraints.
            #
            # So, our vector function has the following form
            #
            #     {f0,f1,f2} = { x0+x1 , x2-exp(x0) , x0^2+x1^2-1 }
            #
            # with f0 being target function, f1 being equality constraint "f1=0",
            # f2 being inequality constraint "f2<=0".
            #
            xalglib.mindfoptimize_v(state, nlcfunc2_fvec)
            x1, rep = xalglib.mindfresults(state)
            _TestResult = _TestResult and doc_print_test(x1, [-0.70710,-0.70710,0.49306], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("mindf_gdemo_auto                 FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minlp_basic
    #      Basic linear programming example
    #
    _TestResult = True
    for _spoil_scenario in range(-1,15):
        try:
            #
            # This example demonstrates how to minimize
            #
            #     F(x0,x1) = -0.1*x0 - x1
            #
            # subject to box constraints
            #
            #     -1 <= x0,x1 <= +1 
            #
            # and general linear constraints
            #
            #     x0 - x1 >= -1
            #     x0 + x1 <=  1
            #
            # We use dual simplex solver provided by ALGLIB for this task. Box
            # constraints are specified by means of constraint vectors bndl and
            # bndu (we have bndl<=x<=bndu). General linear constraints are
            # specified as AL<=A*x<=AU, with AL/AU being 2x1 vectors and A being
            # 2x2 matrix.
            #
            # NOTE: some/all components of AL/AU can be +-INF, same applies to
            #       bndl/bndu. You can also have AL[I]=AU[i] (as well as
            #       BndL[i]=BndU[i]).
            #
            a = [[1,-1],[1,+1]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(a)
            if _spoil_scenario==1:
                spoil_mat_by_deleting_row(a)
            if _spoil_scenario==2:
                spoil_mat_by_deleting_col(a)
            al = [-1,-float("inf")]
            if _spoil_scenario==3:
                spoil_vec_by_nan(al)
            if _spoil_scenario==4:
                spoil_vec_by_deleting_element(al)
            au = [float("inf"),+1]
            if _spoil_scenario==5:
                spoil_vec_by_nan(au)
            if _spoil_scenario==6:
                spoil_vec_by_deleting_element(au)
            c = [-0.1,-1]
            if _spoil_scenario==7:
                spoil_vec_by_nan(c)
            if _spoil_scenario==8:
                spoil_vec_by_deleting_element(c)
            s = [1,1]
            if _spoil_scenario==9:
                spoil_vec_by_nan(s)
            if _spoil_scenario==10:
                spoil_vec_by_deleting_element(s)
            bndl = [-1,-1]
            if _spoil_scenario==11:
                spoil_vec_by_nan(bndl)
            if _spoil_scenario==12:
                spoil_vec_by_deleting_element(bndl)
            bndu = [+1,+1]
            if _spoil_scenario==13:
                spoil_vec_by_nan(bndu)
            if _spoil_scenario==14:
                spoil_vec_by_deleting_element(bndu)

            state = xalglib.minlpcreate(2)

            #
            # Set cost vector, box constraints, general linear constraints.
            #
            # Box constraints can be set in one call to minlpsetbc() or minlpsetbcall()
            # (latter sets same constraints for all variables and accepts two scalars
            # instead of two vectors).
            #
            # General linear constraints can be specified in several ways:
            # * minlpsetlc2dense() - accepts dense 2D array as input; sometimes this
            #   approach is more convenient, although less memory-efficient.
            # * minlpsetlc2() - accepts sparse matrix as input
            # * minlpaddlc2dense() - appends one row to the current set of constraints;
            #   row being appended is specified as dense vector
            # * minlpaddlc2() - appends one row to the current set of constraints;
            #   row being appended is specified as sparse set of elements
            # Independently from specific function being used, LP solver uses sparse
            # storage format for internal representation of constraints.
            #
            xalglib.minlpsetcost(state, c)
            xalglib.minlpsetbc(state, bndl, bndu)
            xalglib.minlpsetlc2dense(state, a, al, au, 2)

            #
            # Set scale of the parameters.
            #
            # It is strongly recommended that you set scale of your variables.
            # Knowing their scales is essential for evaluation of stopping criteria
            # and for preconditioning of the algorithm steps.
            # You can find more information on scaling at http://www.alglib.net/optimization/scaling.php
            #
            xalglib.minlpsetscale(state, s)

            # Solve
            xalglib.minlpoptimize(state)
            x, rep = xalglib.minlpresults(state)
            _TestResult = _TestResult and doc_print_test(x, [0,1], "real_vector", 0.0005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minlp_basic                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST nls_derivative_free
    #      Nonlinear least squares optimization using derivative-free algorithms
    #
    _TestResult = True
    for _spoil_scenario in range(-1,13):
        try:
            #
            # This example demonstrates minimization of F(x0,x1) = f0^2+f1^2, where 
            #
            #     f0(x0,x1) = 10*(x0+3)^2
            #     f1(x0,x1) = (x1-3)^2
            #
            # subject to box constraints
            #
            #     -1 <= x0 <= +1
            #     -1 <= x1 <= +1
            #
            # using DFO mode of the NLS optimizer.
            #
            # IMPORTANT: the  NLS  optimizer   supports   parallel  model  evaluation
            #            ('callback parallelism'). This feature, which  is present in
            #            commercial ALGLIB editions, greatly accelerates optimization
            #            when  using  a  solver  which  issues  batch  requests, i.e.
            #            multiple requests  for  target values, which can be computed
            #            independently by different threads.
            #
            #            Callback parallelism is usually  beneficial when  processing
            #            a  batch  request  requires  more than several milliseconds.
            #            This particular  example,  of  course,  is  not  suited  for
            #            callback parallelism.
            #
            #            It  also  requires  the  solver  which  issues  requests  in
            #            convenient batches, e.g. 2PS solver.
            #
            #            See ALGLIB Reference Manual, 'Working with commercial version'
            #            section,  and  comments  on  nlsoptimize() function for more
            #            information.
            #
            x = [0,0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            bndl = [-1,-1]
            if _spoil_scenario==6:
                spoil_vec_by_nan(bndl)
            if _spoil_scenario==7:
                spoil_vec_by_deleting_element(bndl)
            bndu = [+1,+1]
            if _spoil_scenario==8:
                spoil_vec_by_nan(bndu)
            if _spoil_scenario==9:
                spoil_vec_by_deleting_element(bndu)
            epsx = 0.0000001
            if _spoil_scenario==10:
                epsx = float("nan")
            if _spoil_scenario==11:
                epsx = float("+inf")
            if _spoil_scenario==12:
                epsx = float("-inf")
            maxits = 0

            #
            # Create optimizer, tell it to:
            # * use derivative-free mode
            # * use unit scale for all variables (s is a unit vector)
            # * stop after short enough step (less than epsx)
            #
            state = xalglib.nlscreatedfo(2, x)
            xalglib.nlssetcond(state, epsx, maxits)
            xalglib.nlssetscale(state, s)
            xalglib.nlssetbc(state, bndl, bndu)

            #
            # Choose a derivative-free nonlinear least squares algorithm. ALGLIB
            # supports the following solvers:
            #
            # * DFO-LSA  - a modified version of DFO-LS (Cartis, Fiala, Marteau,
            #   Roberts), with "A" standing for ALGLIB in order to distinguish it
            #   from the original version. This algorithm achieves the smallest
            #   function evaluations count, but has relatively high iteration
            #   overhead and no callback parallelism potential (it issues target
            #   evaluation requests one by one, so they can not be parallelized).
            #   Recommended for expensive targets with no parallelism support.
            #
            # * 2PS (two-point stencil) - an easily parallelized algorithm
            #   developed by ALGLIB Project. It needs about 3x-4x more target
            #   evaluations than DFO-LSA (the ratio has no strong dependence on
            #   the problem size), however it issues target evaluation requests
            #   in large batches, so they can be computed in parallel. Additionally
            #   it has low iteration overhead, so it can be better suited for
            #   problems with cheap targets that DFO-LSA.
            #
            # Both solvers demonstrate quadratic convergence similarly to the
            # Levenberg-Marquardt method.
            #
            # The summary is:
            # * expensive target, no parallelism => DFO-LSA 
            # * expensive target, parallel callbacks => 2PS
            # * inexpensive target => most likely 2PS, maybe DFO-LSA
            #
            # The code below sets the algorithm to be DFO-LSA, then switches
            # it to 2PS.
            #
            xalglib.nlssetalgodfolsa(state)
            xalglib.nlssetalgo2ps(state)

            #
            # Solve the problem.
            #
            # The code below does not use parallelism. If you want to activate
            # callback parallelism, use commercial edition of ALGLIB and pass
            # alglib::parallelcallbacks as an additional parameter to nlsoptimize().
            #
            # Callback parallelism is intended for expensive problems where one
            # batch (~N target evaluations) takes tens and hundreds of milliseconds
            # to compute.
            #
            xalglib.nlsoptimize_v(state, function1_fvec)

            #
            # Test optimization results
            #
            x, rep = xalglib.nlsresults(state)
            _TestResult = _TestResult and doc_print_test(x, [-1,+1], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("nls_derivative_free              FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minnlc_d_inequality
    #      Nonlinearly constrained optimization (inequality constraints)
    #
    _TestResult = True
    for _spoil_scenario in range(-1,15):
        try:
            #
            # This example demonstrates minimization of
            #
            #     f(x0,x1) = -x0+x1
            #
            # subject to box constraints
            #
            #    x0>=0, x1>=0
            #
            # and a nonlinear inequality constraint
            #
            #    x0^2 + x1^2 - 1 <= 0
            #
            # IMPORTANT: the   MINNLC   optimizer    supports    parallel   numerical
            #            differentiation  ('callback   parallelism').  This  feature,
            #            which  is present  in  commercial  ALGLIB  editions, greatly
            #            accelerates optimization with numerical  differentiation  of
            #            an expensive target functions.
            #
            #            Callback parallelism is usually  beneficial when computing a
            #            numerical gradient requires more than several  milliseconds.
            #            This particular  example,  of  course,  is  not  suited  for
            #            callback parallelism.
            #
            #            See ALGLIB Reference Manual, 'Working with commercial version'
            #            section,  and  comments  on   minlmoptimize()  function  for
            #            more information.
            #
            x0 = [0,0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x0)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x0)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x0)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            epsx = 0.000001
            if _spoil_scenario==6:
                epsx = float("nan")
            if _spoil_scenario==7:
                epsx = float("+inf")
            if _spoil_scenario==8:
                epsx = float("-inf")
            maxits = 0

            #
            # Create optimizer object and tune its settings:
            # * epsx=0.000001  stopping condition for inner iterations
            # * s=[1,1]        all variables have unit scale; it is important to
            #                  tell optimizer about scales of your variables - it
            #                  greatly accelerates convergence and helps to perform
            #                  some important integrity checks.
            #
            state = xalglib.minnlccreate(2, x0)
            xalglib.minnlcsetcond(state, epsx, maxits)
            xalglib.minnlcsetscale(state, s)

            #
            # Choose  one  of  nonlinear  programming  solvers  supported  by  MINNLC
            # optimizer.
            #
            # As of ALGLIB 4.01, the default (and recommended)  option  is to  use  a
            # large-scale filter-based SQP solver, which can utilize sparsity of  the
            # problem and uses a limited-memory BFGS update in order to  be  able  to
            # deal with thousands of variables.
            #
            # Other options include:
            # * SQP-BFGS (the same filter SQP solver relying on a dense BFGS  update,
            #   not intended for anything beyond 100 variables)
            # * AUL2 solver (a large-scale augmented  Lagrangian  solver for problems
            #   with  cheap  target  functions)
            # * SL1QP and SL1QP-BFGS legacy solvers which are similar to filter-based
            #   SQP/SQP-BFGS, but use a less  robust  L1  merit  function  to  handle
            #   constraints
            # * SLP a legacy  sequential  linear  programming  solver,  scales  badly
            #   beyond several tens of variables).
            #
            xalglib.minnlcsetalgosqp(state)

            #
            # Set constraints:
            #
            # 1. box constraints are passed with minnlcsetbc() call. The  solver also
            #    supports linear constraints with minnlcsetlc().
            #
            # 2. nonlinear constraints are more tricky - you can not "pack" a general
            #    nonlinear  function  into  a  double  precision  array.  That's  why
            #    minnlcsetnlc2() does not accept constraints itself - only constraint
            #    bounds are passed.
            #
            #    Since  version  4.01,  ALGLIB  supports  the  most  general  form of
            #    nonlinear constraints: two-sided   constraints  NL<=C(x)<=NU,   with
            #    elements being possibly infinite (means that this specific bound  is
            #    ignored). It includes equality constraints,  upper/lower  inequality
            #    constraints, range constraints. In particular, the constraint
            #
            #        x0^2 + x1^2 - 1 <= 0
            #
            #    can be specified by passing NL=[-INF], NU=[0] to minnlcsetnlc2().
            #
            #    Constraining functions themselves are passed as part  of  a  problem
            #    Jacobian (see below).
            #
            bndl = [0,0]
            bndu = [float("inf"),float("inf")]
            nl = [-float("inf")]
            if _spoil_scenario==9:
                spoil_vec_by_nan(nl)
            if _spoil_scenario==10:
                spoil_vec_by_adding_element(nl)
            if _spoil_scenario==11:
                spoil_vec_by_deleting_element(nl)
            nu = [0]
            if _spoil_scenario==12:
                spoil_vec_by_nan(nu)
            if _spoil_scenario==13:
                spoil_vec_by_adding_element(nu)
            if _spoil_scenario==14:
                spoil_vec_by_deleting_element(nu)
            xalglib.minnlcsetbc(state, bndl, bndu)
            xalglib.minnlcsetnlc2(state, nl, nu)

            #
            # Optimize and test results.
            #
            # Optimizer object accepts vector function and its Jacobian, with first
            # component (Jacobian row) being target function, and next components
            # (Jacobian rows) being nonlinear constraints.
            #
            # So, our vector function has form
            #
            #     {f0,f1} = { -x0+x1 , x0^2+x1^2-1 }
            #
            # with Jacobian
            #
            #         [  -1    +1  ]
            #     J = [            ]
            #         [ 2*x0  2*x1 ]
            #
            # with f0 being target function, f1 being constraining function. Number
            # of equality/inequality constraints is specified by minnlcsetnlc2().
            #
            xalglib.minnlcoptimize_j(state, nlcfunc1_jac)
            x1, rep = xalglib.minnlcresults(state)
            _TestResult = _TestResult and doc_print_test(x1, [1.0000,0.0000], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minnlc_d_inequality              FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minnlc_d_equality
    #      Nonlinearly constrained optimization (equality constraints)
    #
    _TestResult = True
    for _spoil_scenario in range(-1,15):
        try:
            #
            # This example demonstrates minimization of
            #
            #     f(x0,x1) = -x0+x1
            #
            # subject to nonlinear equality constraint
            #
            #    x0^2 + x1^2 - 1 = 0
            #
            # IMPORTANT: the   MINNLC   optimizer    supports    parallel   numerical
            #            differentiation  ('callback   parallelism').  This  feature,
            #            which  is present  in  commercial  ALGLIB  editions, greatly
            #            accelerates optimization with numerical  differentiation  of
            #            an expensive target functions.
            #
            #            Callback parallelism is usually  beneficial when computing a
            #            numerical gradient requires more than several  milliseconds.
            #            This particular  example,  of  course,  is  not  suited  for
            #            callback parallelism.
            #
            #            See ALGLIB Reference Manual, 'Working with commercial version'
            #            section,  and  comments  on   minlmoptimize()  function  for
            #            more information.
            #
            x0 = [1,1]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x0)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x0)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x0)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            epsx = 0.000001
            if _spoil_scenario==6:
                epsx = float("nan")
            if _spoil_scenario==7:
                epsx = float("+inf")
            if _spoil_scenario==8:
                epsx = float("-inf")
            maxits = 0

            #
            # Create optimizer object and tune its settings:
            # * epsx=0.000001  stopping condition for inner iterations
            # * s=[1,1]        all variables have unit scale
            #
            state = xalglib.minnlccreate(2, x0)
            xalglib.minnlcsetcond(state, epsx, maxits)
            xalglib.minnlcsetscale(state, s)

            #
            # Choose  one  of  nonlinear  programming  solvers  supported  by  MINNLC
            # optimizer.
            #
            # As of ALGLIB 4.01, the default (and recommended)  option  is to  use  a
            # large-scale filter-based SQP solver, which can utilize sparsity of  the
            # problem and uses a limited-memory BFGS update in order to  be  able  to
            # deal with thousands of variables.
            #
            # Other options include:
            # * SQP-BFGS (the same filter SQP solver relying on a dense BFGS  update,
            #   not intended for anything beyond 100 variables)
            # * AUL2 solver (a large-scale augmented  Lagrangian  solver for problems
            #   with  cheap  target  functions)
            # * SL1QP and SL1QP-BFGS legacy solvers which are similar to filter-based
            #   SQP/SQP-BFGS, but use a less  robust  L1  merit  function  to  handle
            #   constraints
            # * SLP a legacy  sequential  linear  programming  solver,  scales  badly
            #   beyond several tens of variables).
            #
            xalglib.minnlcsetalgosqp(state)

            #
            # Set constraints:
            #
            # Since  version  4.01,  ALGLIB  supports  the  most  general  form of
            # nonlinear constraints: two-sided   constraints  NL<=C(x)<=NU,   with
            # elements being possibly infinite (means that this specific bound  is
            # ignored). It includes equality constraints,  upper/lower  inequality
            # constraints, range constraints. In particular, the constraint
            #
            #        x0^2 + x1^2 - 1 = 0
            #
            # can be specified by passing NL=[0], NU=[0] to minnlcsetnlc2().
            #
            # Constraining functions themselves are passed as part  of  a  problem
            # Jacobian (see below).
            #
            nl = [0]
            if _spoil_scenario==9:
                spoil_vec_by_nan(nl)
            if _spoil_scenario==10:
                spoil_vec_by_adding_element(nl)
            if _spoil_scenario==11:
                spoil_vec_by_deleting_element(nl)
            nu = [0]
            if _spoil_scenario==12:
                spoil_vec_by_nan(nu)
            if _spoil_scenario==13:
                spoil_vec_by_adding_element(nu)
            if _spoil_scenario==14:
                spoil_vec_by_deleting_element(nu)
            xalglib.minnlcsetnlc2(state, nl, nu)

            #
            # Optimize and test results.
            #
            # Optimizer object accepts vector function and its Jacobian, with first
            # component (Jacobian row) being target function, and next components
            # (Jacobian rows) being nonlinear equality and inequality constraints.
            #
            # So, our vector function has form
            #
            #     {f0,f1} = { -x0+x1 , x0^2+x1^2-1 }
            #
            # with Jacobian
            #
            #         [  -1    +1  ]
            #     J = [            ]
            #         [ 2*x0  2*x1 ]
            #
            # with f0 being target function, f1 being constraining function. Number
            # of equality/inequality constraints is specified by minnlcsetnlc2().
            #
            xalglib.minnlcoptimize_j(state, nlcfunc1_jac)
            x1, rep = xalglib.minnlcresults(state)
            _TestResult = _TestResult and doc_print_test(x1, [0.70710,-0.70710], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minnlc_d_equality                FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minnlc_d_mixed
    #      Nonlinearly constrained optimization with mixed equality/inequality constraints
    #
    sys.stdout.write("50/162\n")
    _TestResult = True
    for _spoil_scenario in range(-1,15):
        try:
            #
            # This example demonstrates minimization of
            #
            #     f(x0,x1) = x0+x1
            #
            # subject to nonlinear inequality constraint
            #
            #    x0^2 + x1^2 - 1 <= 0
            #
            # and nonlinear equality constraint
            #
            #    x2-exp(x0) = 0
            #
            # IMPORTANT: the   MINNLC   optimizer    supports    parallel   numerical
            #            differentiation  ('callback   parallelism').  This  feature,
            #            which  is present  in  commercial  ALGLIB  editions, greatly
            #            accelerates optimization with numerical  differentiation  of
            #            an expensive target functions.
            #
            #            Callback parallelism is usually  beneficial when computing a
            #            numerical gradient requires more than several  milliseconds.
            #            This particular  example,  of  course,  is  not  suited  for
            #            callback parallelism.
            #
            #            See ALGLIB Reference Manual, 'Working with commercial version'
            #            section,  and  comments  on   minlmoptimize()  function  for
            #            more information.
            #
            x0 = [0,0,0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x0)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x0)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x0)
            s = [1,1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            epsx = 0.000001
            if _spoil_scenario==6:
                epsx = float("nan")
            if _spoil_scenario==7:
                epsx = float("+inf")
            if _spoil_scenario==8:
                epsx = float("-inf")
            maxits = 0

            #
            # Create optimizer object and tune its settings:
            # * epsx=0.000001  stopping condition for inner iterations
            # * s=[1,1]        all variables have unit scale
            # * upper limit on step length is specified (to avoid probing locations where exp() is large)
            #
            state = xalglib.minnlccreate(3, x0)
            xalglib.minnlcsetcond(state, epsx, maxits)
            xalglib.minnlcsetscale(state, s)
            xalglib.minnlcsetstpmax(state, 10.0)

            #
            # Choose  one  of  nonlinear  programming  solvers  supported  by  MINNLC
            # optimizer.
            #
            # As of ALGLIB 4.01, the default (and recommended)  option  is to  use  a
            # large-scale filter-based SQP solver, which can utilize sparsity of  the
            # problem and uses a limited-memory BFGS update in order to  be  able  to
            # deal with thousands of variables.
            #
            # Other options include:
            # * SQP-BFGS (the same filter SQP solver relying on a dense BFGS  update,
            #   not intended for anything beyond 100 variables)
            # * AUL2 solver (a large-scale augmented  Lagrangian  solver for problems
            #   with  cheap  target  functions)
            # * SL1QP and SL1QP-BFGS legacy solvers which are similar to filter-based
            #   SQP/SQP-BFGS, but use a less  robust  L1  merit  function  to  handle
            #   constraints
            # * SLP a legacy  sequential  linear  programming  solver,  scales  badly
            #   beyond several tens of variables).
            #
            xalglib.minnlcsetalgosqp(state)

            #
            # Set constraints:
            #
            # Since  version  4.01,  ALGLIB  supports  the  most  general  form of
            # nonlinear constraints: two-sided   constraints  NL<=C(x)<=NU,   with
            # elements being possibly infinite (means that this specific bound  is
            # ignored). It includes equality constraints,  upper/lower  inequality
            # constraints, range constraints. In particular, a pair of constraints
            #
            #        x2-exp(x0)       = 0
            #        x0^2 + x1^2 - 1 <= 0
            #
            # can be specified by passing NL=[0,-INF], NU=[0,0] to minnlcsetnlc2().
            #
            # Constraining functions themselves are passed as part  of  a  problem
            # Jacobian (see below).
            #
            nl = [0,-float("inf")]
            if _spoil_scenario==9:
                spoil_vec_by_nan(nl)
            if _spoil_scenario==10:
                spoil_vec_by_adding_element(nl)
            if _spoil_scenario==11:
                spoil_vec_by_deleting_element(nl)
            nu = [0,0]
            if _spoil_scenario==12:
                spoil_vec_by_nan(nu)
            if _spoil_scenario==13:
                spoil_vec_by_adding_element(nu)
            if _spoil_scenario==14:
                spoil_vec_by_deleting_element(nu)
            xalglib.minnlcsetnlc2(state, nl, nu)

            #
            # Optimize and test results.
            #
            # Optimizer object accepts vector function and its Jacobian, with first
            # component (Jacobian row) being target function, and next components
            # (Jacobian rows) being nonlinear equality and inequality constraints.
            #
            # So, our vector function has form
            #
            #     {f0,f1,f2} = { x0+x1 , x2-exp(x0) , x0^2+x1^2-1 }
            #
            # with Jacobian
            #
            #         [  +1      +1       0 ]
            #     J = [-exp(x0)  0        1 ]
            #         [ 2*x0    2*x1      0 ]
            #
            # with f0 being target function, f1 being equality constraint "f1=0",
            # f2 being inequality constraint "f2<=0".
            #
            xalglib.minnlcoptimize_j(state, nlcfunc2_jac)
            x1, rep = xalglib.minnlcresults(state)
            _TestResult = _TestResult and doc_print_test(x1, [-0.70710,-0.70710,0.49306], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minnlc_d_mixed                   FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minmo_biobjective
    #      Unconstrained biobjective optimization
    #
    _TestResult = True
    for _spoil_scenario in range(-1,4):
        try:
            #
            # This example demonstrates minimization of two targets
            #
            #     f0(x0,x1) = x0^2 + (x1-1)^2
            #     f1(x0,x1) = (x0-1(^2 + x1^2
            #
            # These targets are Euclidean distances to (0,1) and (1,0) respectively, thus solutions
            # to this problem occupy the straight line segment connecting these points. (Points
            # outside of the line are Pareto non-optimal because one can always decrease both distances
            # by moving closer to the line).
            #
            nvars = 2
            nobjectives = 2
            x0 = [0,0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x0)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x0)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x0)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(x0)
            frontsize = 10
            polishsolutions = True
            state = xalglib.minmocreate(nvars, nobjectives, x0)

            #
            # The solver is configured to compute 10 points approximating the Pareto front,
            # and to polish solutions (i.e. use an additional optimization phase that improves
            # accuracy on degenerate problems; not actually necessary for this simple example).
            #
            xalglib.minmosetalgonbi(state, frontsize, polishsolutions)

            #
            # Optimize and test results.
            #
            # The optimization is performed using analytic (user-provided) Jacobian matrix.
            # Use minmocreatef(), if you do not know analytic form of the Jacobian and want
            # ALGLIB to perform numerical differentiation.
            #
            # We requested 10 Pareto-optimal points and we expect solver to compute all points
            # (it is possible to return less if the solver was terminated)
            #
            xalglib.minmooptimize_j(state, multiobjective2_jac)
            paretofront, frontsize, rep = xalglib.minmoresults(state)
            _TestResult = _TestResult and doc_print_test(frontsize, 10, "int")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minmo_biobjective                FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minmo_biobjective_constr
    #      Nonlinearly constrained biobjective optimization
    #
    _TestResult = True
    for _spoil_scenario in range(-1,8):
        try:
            #
            # This example demonstrates minimization of two targets
            #
            #     f0(x0,x1) = x0^2 + (x1-1)^2
            #     f1(x0,x1) = (x0-1(^2 + x1^2
            #
            # subject to a nonlinear constraint
            #
            #     f2(x0,x1) = x0^2 + x1^2 >= 1
            #
            # These targets are Euclidean distances to (0,1) and (1,0) respectively, thus solutions to this
            # problem should occupy the straight line segment connecting these points. However, due to the
            # nonlinear constraint being present, Pareto front has another shape.
            #
            nvars = 2
            nobjectives = 2
            x0 = [0,0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x0)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x0)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x0)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(x0)
            frontsize = 10
            polishsolutions = True
            lowerbnd = [1]
            if _spoil_scenario==4:
                spoil_vec_by_nan(lowerbnd)
            if _spoil_scenario==5:
                spoil_vec_by_deleting_element(lowerbnd)
            upperbnd = [float("inf")]
            if _spoil_scenario==6:
                spoil_vec_by_nan(upperbnd)
            if _spoil_scenario==7:
                spoil_vec_by_deleting_element(upperbnd)
            state = xalglib.minmocreate(nvars, nobjectives, x0)
            xalglib.minmosetnlc2(state, lowerbnd, upperbnd, 1)

            #
            # The solver is configured to compute 10 points approximating the Pareto front,
            # and to polish solutions (i.e. use an additional optimization phase that improves
            # accuracy on degenerate problems; not actually necessary for this simple example).
            #
            xalglib.minmosetalgonbi(state, frontsize, polishsolutions)

            #
            # Optimize and test results.
            #
            # The optimization is performed using analytic (user-provided) Jacobian matrix.
            # Use minmocreatef(), if you do not know analytic form of the Jacobian and want
            # ALGLIB to perform numerical differentiation.
            #
            # We requested 10 Pareto-optimal points and we expect solver to compute all points
            # (it is possible to return less if the solver was terminated)
            #
            xalglib.minmooptimize_j(state, multiobjective2constr_jac)
            paretofront, frontsize, rep = xalglib.minmoresults(state)
            _TestResult = _TestResult and doc_print_test(frontsize, 10, "int")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minmo_biobjective_constr         FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minns_d_unconstrained
    #      Nonsmooth unconstrained optimization
    #
    _TestResult = True
    for _spoil_scenario in range(-1,15):
        try:
            #
            # This example demonstrates minimization of
            #
            #     f(x0,x1) = 2*|x0|+|x1|
            #
            # using nonsmooth nonlinear optimizer.
            #
            x0 = [1,1]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x0)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x0)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x0)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            epsx = 0.00001
            if _spoil_scenario==6:
                epsx = float("nan")
            if _spoil_scenario==7:
                epsx = float("+inf")
            if _spoil_scenario==8:
                epsx = float("-inf")
            radius = 0.1
            if _spoil_scenario==9:
                radius = float("nan")
            if _spoil_scenario==10:
                radius = float("+inf")
            if _spoil_scenario==11:
                radius = float("-inf")
            rho = 0.0
            if _spoil_scenario==12:
                rho = float("nan")
            if _spoil_scenario==13:
                rho = float("+inf")
            if _spoil_scenario==14:
                rho = float("-inf")
            maxits = 0

            #
            # Create optimizer object, choose AGS algorithm and tune its settings:
            # * radius=0.1     good initial value; will be automatically decreased later.
            # * rho=0.0        penalty coefficient for nonlinear constraints; can be zero
            #                  because we do not have such constraints
            # * epsx=0.000001  stopping conditions
            # * s=[1,1]        all variables have unit scale
            #
            state = xalglib.minnscreate(2, x0)
            xalglib.minnssetalgoags(state, radius, rho)
            xalglib.minnssetcond(state, epsx, maxits)
            xalglib.minnssetscale(state, s)

            #
            # Optimize and test results.
            #
            # Optimizer object accepts vector function and its Jacobian, with first
            # component (Jacobian row) being target function, and next components
            # (Jacobian rows) being nonlinear equality and inequality constraints
            # (box/linear ones are passed separately by means of minnssetbc() and
            # minnssetlc() calls).
            #
            # If you do not have nonlinear constraints (exactly our situation), then
            # you will have one-component function vector and 1xN Jacobian matrix.
            #
            # So, our vector function has form
            #
            #     {f0} = { 2*|x0|+|x1| }
            #
            # with Jacobian
            #
            #         [                       ]
            #     J = [ 2*sign(x0)   sign(x1) ]
            #         [                       ]
            #
            # NOTE: nonsmooth optimizer requires considerably more function
            #       evaluations than smooth solver - about 2N times more. Using
            #       numerical differentiation introduces additional (multiplicative)
            #       2N speedup.
            #
            #       It means that if smooth optimizer WITH user-supplied gradient
            #       needs 100 function evaluations to solve 50-dimensional problem,
            #       then AGS solver with user-supplied gradient will need about 10.000
            #       function evaluations, and with numerical gradient about 1.000.000
            #       function evaluations will be performed.
            #
            # NOTE: AGS solver used by us can handle nonsmooth and nonconvex
            #       optimization problems. It has convergence guarantees, i.e. it will
            #       converge to stationary point of the function after running for some
            #       time.
            #
            #       However, it is important to remember that "stationary point" is not
            #       equal to "solution". If your problem is convex, everything is OK.
            #       But nonconvex optimization problems may have "flat spots" - large
            #       areas where gradient is exactly zero, but function value is far away
            #       from optimal. Such areas are stationary points too, and optimizer
            #       may be trapped here.
            #
            #       "Flat spots" are nonsmooth equivalent of the saddle points, but with
            #       orders of magnitude worse properties - they may be quite large and
            #       hard to avoid. All nonsmooth optimizers are prone to this kind of the
            #       problem, because it is impossible to automatically distinguish "flat
            #       spot" from true solution.
            #
            #       This note is here to warn you that you should be very careful when
            #       you solve nonsmooth optimization problems. Visual inspection of
            #       results is essential.
            #
            xalglib.minnsoptimize_j(state, nsfunc1_jac)
            x1, rep = xalglib.minnsresults(state)
            _TestResult = _TestResult and doc_print_test(x1, [0.0000,0.0000], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minns_d_unconstrained            FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minns_d_diff
    #      Nonsmooth unconstrained optimization with numerical differentiation
    #
    _TestResult = True
    for _spoil_scenario in range(-1,18):
        try:
            #
            # This example demonstrates minimization of
            #
            #     f(x0,x1) = 2*|x0|+|x1|
            #
            # using nonsmooth nonlinear optimizer with numerical
            # differentiation provided by ALGLIB.
            #
            # NOTE: nonsmooth optimizer requires considerably more function
            #       evaluations than smooth solver - about 2N times more. Using
            #       numerical differentiation introduces additional (multiplicative)
            #       2N speedup.
            #
            #       It means that if smooth optimizer WITH user-supplied gradient
            #       needs 100 function evaluations to solve 50-dimensional problem,
            #       then AGS solver with user-supplied gradient will need about 10.000
            #       function evaluations, and with numerical gradient about 1.000.000
            #       function evaluations will be performed.
            #
            x0 = [1,1]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x0)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x0)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x0)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            epsx = 0.00001
            if _spoil_scenario==6:
                epsx = float("nan")
            if _spoil_scenario==7:
                epsx = float("+inf")
            if _spoil_scenario==8:
                epsx = float("-inf")
            diffstep = 0.000001
            if _spoil_scenario==9:
                diffstep = float("nan")
            if _spoil_scenario==10:
                diffstep = float("+inf")
            if _spoil_scenario==11:
                diffstep = float("-inf")
            radius = 0.1
            if _spoil_scenario==12:
                radius = float("nan")
            if _spoil_scenario==13:
                radius = float("+inf")
            if _spoil_scenario==14:
                radius = float("-inf")
            rho = 0.0
            if _spoil_scenario==15:
                rho = float("nan")
            if _spoil_scenario==16:
                rho = float("+inf")
            if _spoil_scenario==17:
                rho = float("-inf")
            maxits = 0

            #
            # Create optimizer object, choose AGS algorithm and tune its settings:
            # * radius=0.1     good initial value; will be automatically decreased later.
            # * rho=0.0        penalty coefficient for nonlinear constraints; can be zero
            #                  because we do not have such constraints
            # * epsx=0.000001  stopping conditions
            # * s=[1,1]        all variables have unit scale
            #
            state = xalglib.minnscreatef(2, x0, diffstep)
            xalglib.minnssetalgoags(state, radius, rho)
            xalglib.minnssetcond(state, epsx, maxits)
            xalglib.minnssetscale(state, s)

            #
            # Optimize and test results.
            #
            # Optimizer object accepts vector function, with first component
            # being target function, and next components being nonlinear equality
            # and inequality constraints (box/linear ones are passed separately
            # by means of minnssetbc() and minnssetlc() calls).
            #
            # If you do not have nonlinear constraints (exactly our situation), then
            # you will have one-component function vector.
            #
            # So, our vector function has form
            #
            #     {f0} = { 2*|x0|+|x1| }
            #
            xalglib.minnsoptimize_v(state, nsfunc1_fvec)
            x1, rep = xalglib.minnsresults(state)
            _TestResult = _TestResult and doc_print_test(x1, [0.0000,0.0000], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minns_d_diff                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minns_d_bc
    #      Nonsmooth box constrained optimization
    #
    _TestResult = True
    for _spoil_scenario in range(-1,17):
        try:
            #
            # This example demonstrates minimization of
            #
            #     f(x0,x1) = 2*|x0|+|x1|
            #
            # subject to box constraints
            #
            #        1 <= x0 < +INF
            #     -INF <= x1 < +INF
            #
            # using nonsmooth nonlinear optimizer.
            #
            x0 = [1,1]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x0)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x0)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x0)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            bndl = [1,-float("inf")]
            if _spoil_scenario==6:
                spoil_vec_by_nan(bndl)
            bndu = [float("inf"),float("inf")]
            if _spoil_scenario==7:
                spoil_vec_by_nan(bndu)
            epsx = 0.00001
            if _spoil_scenario==8:
                epsx = float("nan")
            if _spoil_scenario==9:
                epsx = float("+inf")
            if _spoil_scenario==10:
                epsx = float("-inf")
            radius = 0.1
            if _spoil_scenario==11:
                radius = float("nan")
            if _spoil_scenario==12:
                radius = float("+inf")
            if _spoil_scenario==13:
                radius = float("-inf")
            rho = 0.0
            if _spoil_scenario==14:
                rho = float("nan")
            if _spoil_scenario==15:
                rho = float("+inf")
            if _spoil_scenario==16:
                rho = float("-inf")
            maxits = 0

            #
            # Create optimizer object, choose AGS algorithm and tune its settings:
            # * radius=0.1     good initial value; will be automatically decreased later.
            # * rho=0.0        penalty coefficient for nonlinear constraints; can be zero
            #                  because we do not have such constraints
            # * epsx=0.000001  stopping conditions
            # * s=[1,1]        all variables have unit scale
            #
            state = xalglib.minnscreate(2, x0)
            xalglib.minnssetalgoags(state, radius, rho)
            xalglib.minnssetcond(state, epsx, maxits)
            xalglib.minnssetscale(state, s)

            #
            # Set box constraints.
            #
            # General linear constraints are set in similar way (see comments on
            # minnssetlc() function for more information).
            #
            # You may combine box, linear and nonlinear constraints in one optimization
            # problem.
            #
            xalglib.minnssetbc(state, bndl, bndu)

            #
            # Optimize and test results.
            #
            # Optimizer object accepts vector function and its Jacobian, with first
            # component (Jacobian row) being target function, and next components
            # (Jacobian rows) being nonlinear equality and inequality constraints
            # (box/linear ones are passed separately by means of minnssetbc() and
            # minnssetlc() calls).
            #
            # If you do not have nonlinear constraints (exactly our situation), then
            # you will have one-component function vector and 1xN Jacobian matrix.
            #
            # So, our vector function has form
            #
            #     {f0} = { 2*|x0|+|x1| }
            #
            # with Jacobian
            #
            #         [                       ]
            #     J = [ 2*sign(x0)   sign(x1) ]
            #         [                       ]
            #
            # NOTE: nonsmooth optimizer requires considerably more function
            #       evaluations than smooth solver - about 2N times more. Using
            #       numerical differentiation introduces additional (multiplicative)
            #       2N speedup.
            #
            #       It means that if smooth optimizer WITH user-supplied gradient
            #       needs 100 function evaluations to solve 50-dimensional problem,
            #       then AGS solver with user-supplied gradient will need about 10.000
            #       function evaluations, and with numerical gradient about 1.000.000
            #       function evaluations will be performed.
            #
            # NOTE: AGS solver used by us can handle nonsmooth and nonconvex
            #       optimization problems. It has convergence guarantees, i.e. it will
            #       converge to stationary point of the function after running for some
            #       time.
            #
            #       However, it is important to remember that "stationary point" is not
            #       equal to "solution". If your problem is convex, everything is OK.
            #       But nonconvex optimization problems may have "flat spots" - large
            #       areas where gradient is exactly zero, but function value is far away
            #       from optimal. Such areas are stationary points too, and optimizer
            #       may be trapped here.
            #
            #       "Flat spots" are nonsmooth equivalent of the saddle points, but with
            #       orders of magnitude worse properties - they may be quite large and
            #       hard to avoid. All nonsmooth optimizers are prone to this kind of the
            #       problem, because it is impossible to automatically distinguish "flat
            #       spot" from true solution.
            #
            #       This note is here to warn you that you should be very careful when
            #       you solve nonsmooth optimization problems. Visual inspection of
            #       results is essential.
            #
            #
            xalglib.minnsoptimize_j(state, nsfunc1_jac)
            x1, rep = xalglib.minnsresults(state)
            _TestResult = _TestResult and doc_print_test(x1, [1.0000,0.0000], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minns_d_bc                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minns_d_nlc
    #      Nonsmooth nonlinearly constrained optimization
    #
    _TestResult = True
    for _spoil_scenario in range(-1,15):
        try:
            #
            # This example demonstrates minimization of
            #
            #     f(x0,x1) = 2*|x0|+|x1|
            #
            # subject to combination of equality and inequality constraints
            #
            #      x0  =  1
            #      x1 >= -1
            #
            # using nonsmooth nonlinear optimizer. Although these constraints
            # are linear, we treat them as general nonlinear ones in order to
            # demonstrate nonlinearly constrained optimization setup.
            #
            x0 = [1,1]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x0)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x0)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x0)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            epsx = 0.00001
            if _spoil_scenario==6:
                epsx = float("nan")
            if _spoil_scenario==7:
                epsx = float("+inf")
            if _spoil_scenario==8:
                epsx = float("-inf")
            radius = 0.1
            if _spoil_scenario==9:
                radius = float("nan")
            if _spoil_scenario==10:
                radius = float("+inf")
            if _spoil_scenario==11:
                radius = float("-inf")
            rho = 50.0
            if _spoil_scenario==12:
                rho = float("nan")
            if _spoil_scenario==13:
                rho = float("+inf")
            if _spoil_scenario==14:
                rho = float("-inf")
            maxits = 0

            #
            # Create optimizer object, choose AGS algorithm and tune its settings:
            # * radius=0.1     good initial value; will be automatically decreased later.
            # * rho=50.0       penalty coefficient for nonlinear constraints. It is your
            #                  responsibility to choose good one - large enough that it
            #                  enforces constraints, but small enough in order to avoid
            #                  extreme slowdown due to ill-conditioning.
            # * epsx=0.000001  stopping conditions
            # * s=[1,1]        all variables have unit scale
            #
            state = xalglib.minnscreate(2, x0)
            xalglib.minnssetalgoags(state, radius, rho)
            xalglib.minnssetcond(state, epsx, maxits)
            xalglib.minnssetscale(state, s)

            #
            # Set general nonlinear constraints.
            #
            # This part is more tricky than working with box/linear constraints - you
            # can not "pack" general nonlinear function into double precision array.
            # That's why minnssetnlc() does not accept constraints itself - only
            # constraint COUNTS are passed: first parameter is number of equality
            # constraints, second one is number of inequality constraints.
            #
            # As for constraining functions - these functions are passed as part
            # of problem Jacobian (see below).
            #
            # NOTE: MinNS optimizer supports arbitrary combination of boundary, general
            #       linear and general nonlinear constraints. This example does not
            #       show how to work with general linear constraints, but you can
            #       easily find it in documentation on minnlcsetlc() function.
            #
            xalglib.minnssetnlc(state, 1, 1)

            #
            # Optimize and test results.
            #
            # Optimizer object accepts vector function and its Jacobian, with first
            # component (Jacobian row) being target function, and next components
            # (Jacobian rows) being nonlinear equality and inequality constraints
            # (box/linear ones are passed separately by means of minnssetbc() and
            # minnssetlc() calls).
            #
            # Nonlinear equality constraints have form Gi(x)=0, inequality ones
            # have form Hi(x)<=0, so we may have to "normalize" constraints prior
            # to passing them to optimizer (right side is zero, constraints are
            # sorted, multiplied by -1 when needed).
            #
            # So, our vector function has form
            #
            #     {f0,f1,f2} = { 2*|x0|+|x1|,  x0-1, -x1-1 }
            #
            # with Jacobian
            #
            #         [ 2*sign(x0)   sign(x1) ]
            #     J = [     1           0     ]
            #         [     0          -1     ]
            #
            # which means that we have optimization problem
            #
            #     min{f0} subject to f1=0, f2<=0
            #
            # which is essentially same as
            #
            #     min { 2*|x0|+|x1| } subject to x0=1, x1>=-1
            #
            # NOTE: AGS solver used by us can handle nonsmooth and nonconvex
            #       optimization problems. It has convergence guarantees, i.e. it will
            #       converge to stationary point of the function after running for some
            #       time.
            #
            #       However, it is important to remember that "stationary point" is not
            #       equal to "solution". If your problem is convex, everything is OK.
            #       But nonconvex optimization problems may have "flat spots" - large
            #       areas where gradient is exactly zero, but function value is far away
            #       from optimal. Such areas are stationary points too, and optimizer
            #       may be trapped here.
            #
            #       "Flat spots" are nonsmooth equivalent of the saddle points, but with
            #       orders of magnitude worse properties - they may be quite large and
            #       hard to avoid. All nonsmooth optimizers are prone to this kind of the
            #       problem, because it is impossible to automatically distinguish "flat
            #       spot" from true solution.
            #
            #       This note is here to warn you that you should be very careful when
            #       you solve nonsmooth optimization problems. Visual inspection of
            #       results is essential.
            #
            xalglib.minnsoptimize_j(state, nsfunc2_jac)
            x1, rep = xalglib.minnsresults(state)
            _TestResult = _TestResult and doc_print_test(x1, [1.0000,0.0000], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minns_d_nlc                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minbc_d_1
    #      Nonlinear optimization with box constraints
    #
    _TestResult = True
    for _spoil_scenario in range(-1,20):
        try:
            #
            # This example demonstrates minimization of
            #
            #     f(x,y) = 100*(x+3)^4+(y-3)^4
            #
            # subject to box constraints
            #
            #     -1<=x<=+1, -1<=y<=+1
            #
            # using MinBC optimizer with:
            # * initial point x=[0,0]
            # * unit scale being set for all variables (see minbcsetscale for more info)
            # * stopping criteria set to "terminate after short enough step"
            # * OptGuard integrity check being used to check problem statement
            #   for some common errors like nonsmoothness or bad analytic gradient
            #
            # First, we create optimizer object and tune its properties:
            # * set box constraints
            # * set variable scales
            # * set stopping criteria
            #
            x = [0,0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            if _spoil_scenario==6:
                spoil_vec_by_deleting_element(s)
            bndl = [-1,-1]
            if _spoil_scenario==7:
                spoil_vec_by_nan(bndl)
            if _spoil_scenario==8:
                spoil_vec_by_deleting_element(bndl)
            bndu = [+1,+1]
            if _spoil_scenario==9:
                spoil_vec_by_nan(bndu)
            if _spoil_scenario==10:
                spoil_vec_by_deleting_element(bndu)
            epsg = 0
            if _spoil_scenario==11:
                epsg = float("nan")
            if _spoil_scenario==12:
                epsg = float("+inf")
            if _spoil_scenario==13:
                epsg = float("-inf")
            epsf = 0
            if _spoil_scenario==14:
                epsf = float("nan")
            if _spoil_scenario==15:
                epsf = float("+inf")
            if _spoil_scenario==16:
                epsf = float("-inf")
            epsx = 0.000001
            if _spoil_scenario==17:
                epsx = float("nan")
            if _spoil_scenario==18:
                epsx = float("+inf")
            if _spoil_scenario==19:
                epsx = float("-inf")
            maxits = 0
            state = xalglib.minbccreate(x)
            xalglib.minbcsetbc(state, bndl, bndu)
            xalglib.minbcsetscale(state, s)
            xalglib.minbcsetcond(state, epsg, epsf, epsx, maxits)

            #
            # Then we activate OptGuard integrity checking.
            #
            # OptGuard monitor helps to catch common coding and problem statement
            # issues, like:
            # * discontinuity of the target function (C0 continuity violation)
            # * nonsmoothness of the target function (C1 continuity violation)
            # * erroneous analytic gradient, i.e. one inconsistent with actual
            #   change in the target/constraints
            #
            # OptGuard is essential for early prototyping stages because such
            # problems often result in premature termination of the optimizer
            # which is really hard to distinguish from the correct termination.
            #
            # IMPORTANT: GRADIENT VERIFICATION IS PERFORMED BY MEANS OF NUMERICAL
            #            DIFFERENTIATION. DO NOT USE IT IN PRODUCTION CODE!!!!!!!
            #
            #            Other OptGuard checks add moderate overhead, but anyway
            #            it is better to turn them off when they are not needed.
            #
            xalglib.minbcoptguardsmoothness(state)
            xalglib.minbcoptguardgradient(state, 0.001)

            #
            # Optimize and evaluate results
            #
            xalglib.minbcoptimize_g(state, function1_grad)
            x, rep = xalglib.minbcresults(state)
            _TestResult = _TestResult and doc_print_test(x, [-1,1], "real_vector", 0.005)

            #
            # Check that OptGuard did not report errors
            #
            # NOTE: want to test OptGuard? Try breaking the gradient - say, add
            #       1.0 to some of its components.
            #
            ogrep = xalglib.minbcoptguardresults(state)
            _TestResult = _TestResult and doc_print_test(ogrep.badgradsuspected, False, "bool")
            _TestResult = _TestResult and doc_print_test(ogrep.nonc0suspected, False, "bool")
            _TestResult = _TestResult and doc_print_test(ogrep.nonc1suspected, False, "bool")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minbc_d_1                        FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST minbc_numdiff
    #      Nonlinear optimization with bound constraints and numerical differentiation
    #
    _TestResult = True
    for _spoil_scenario in range(-1,23):
        try:
            #
            # This example demonstrates minimization of
            #
            #     f(x,y) = 100*(x+3)^4+(y-3)^4
            #
            # subject to box constraints
            #
            #    -1<=x<=+1, -1<=y<=+1
            #
            # using MinBC optimizer with:
            # * numerical differentiation being used
            # * initial point x=[0,0]
            # * unit scale being set for all variables (see minbcsetscale for more info)
            # * stopping criteria set to "terminate after short enough step"
            # * OptGuard integrity check being used to check problem statement
            #   for some common errors like nonsmoothness or bad analytic gradient
            #
            x = [0,0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            s = [1,1]
            if _spoil_scenario==3:
                spoil_vec_by_nan(s)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(s)
            if _spoil_scenario==6:
                spoil_vec_by_deleting_element(s)
            bndl = [-1,-1]
            if _spoil_scenario==7:
                spoil_vec_by_nan(bndl)
            if _spoil_scenario==8:
                spoil_vec_by_deleting_element(bndl)
            bndu = [+1,+1]
            if _spoil_scenario==9:
                spoil_vec_by_nan(bndu)
            if _spoil_scenario==10:
                spoil_vec_by_deleting_element(bndu)
            epsg = 0
            if _spoil_scenario==11:
                epsg = float("nan")
            if _spoil_scenario==12:
                epsg = float("+inf")
            if _spoil_scenario==13:
                epsg = float("-inf")
            epsf = 0
            if _spoil_scenario==14:
                epsf = float("nan")
            if _spoil_scenario==15:
                epsf = float("+inf")
            if _spoil_scenario==16:
                epsf = float("-inf")
            epsx = 0.000001
            if _spoil_scenario==17:
                epsx = float("nan")
            if _spoil_scenario==18:
                epsx = float("+inf")
            if _spoil_scenario==19:
                epsx = float("-inf")
            maxits = 0
            diffstep = 1.0e-6
            if _spoil_scenario==20:
                diffstep = float("nan")
            if _spoil_scenario==21:
                diffstep = float("+inf")
            if _spoil_scenario==22:
                diffstep = float("-inf")

            #
            # Now we are ready to actually optimize something:
            # * first we create optimizer
            # * we add boundary constraints
            # * we tune stopping conditions
            # * and, finally, optimize and obtain results...
            #
            state = xalglib.minbccreatef(x, diffstep)
            xalglib.minbcsetbc(state, bndl, bndu)
            xalglib.minbcsetscale(state, s)
            xalglib.minbcsetcond(state, epsg, epsf, epsx, maxits)

            #
            # Then we activate OptGuard integrity checking.
            #
            # Numerical differentiation always produces "correct" gradient
            # (with some truncation error, but unbiased). Thus, we just have
            # to check smoothness properties of the target: C0 and C1 continuity.
            #
            # Sometimes user accidentally tries to solve nonsmooth problems
            # with smooth optimizer. OptGuard helps to detect such situations
            # early, at the prototyping stage.
            #
            xalglib.minbcoptguardsmoothness(state)

            #
            # Optimize and evaluate results
            #
            xalglib.minbcoptimize_f(state, function1_func)
            x, rep = xalglib.minbcresults(state)
            _TestResult = _TestResult and doc_print_test(x, [-1,1], "real_vector", 0.005)

            #
            # Check that OptGuard did not report errors
            #
            # Want to challenge OptGuard? Try to make your problem
            # nonsmooth by replacing 100*(x+3)^4 by 100*|x+3| and
            # re-run optimizer.
            #
            ogrep = xalglib.minbcoptguardresults(state)
            _TestResult = _TestResult and doc_print_test(ogrep.nonc0suspected, False, "bool")
            _TestResult = _TestResult and doc_print_test(ogrep.nonc1suspected, False, "bool")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("minbc_numdiff                    FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST nneighbor_d_1
    #      Nearest neighbor search, KNN queries
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            a = [[0,0],[0,1],[1,0],[1,1]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(a)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(a)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(a)
            nx = 2
            ny = 0
            normtype = 2
            r = [[]]

            kdt = xalglib.kdtreebuild(a, nx, ny, normtype)

            x = [-1,0]
            k = xalglib.kdtreequeryknn(kdt, x, 1)
            _TestResult = _TestResult and doc_print_test(k, 1, "int")
            r = xalglib.kdtreequeryresultsx(kdt, r)
            _TestResult = _TestResult and doc_print_test(r, [[0,0]], "real_matrix", 0.05)

            x1 = [+0.9,0.1]
            k = xalglib.kdtreequeryknn(kdt, x1, 1)
            _TestResult = _TestResult and doc_print_test(k, 1, "int")
            r = xalglib.kdtreequeryresultsx(kdt, r)
            _TestResult = _TestResult and doc_print_test(r, [[1,0]], "real_matrix", 0.05)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("nneighbor_d_1                    FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST nneighbor_t_2
    #      Subsequent queries; buffered functions must use previously allocated storage (if large enough), so buffer may contain some info from previous call
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            a = [[0,0],[0,1],[1,0],[1,1]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(a)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(a)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(a)
            nx = 2
            ny = 0
            normtype = 2
            rx = [[]]
            kdt = xalglib.kdtreebuild(a, nx, ny, normtype)
            x = [+2,0]
            k = xalglib.kdtreequeryknn(kdt, x, 2, True)
            _TestResult = _TestResult and doc_print_test(k, 2, "int")
            rx = xalglib.kdtreequeryresultsx(kdt, rx)
            _TestResult = _TestResult and doc_print_test(rx, [[1,0],[1,1]], "real_matrix", 0.05)
            x = [-2,0]
            k = xalglib.kdtreequeryknn(kdt, x, 1, True)
            _TestResult = _TestResult and doc_print_test(k, 1, "int")
            rx = xalglib.kdtreequeryresultsx(kdt, rx)
            _TestResult = _TestResult and doc_print_test(rx, [[0,0],[1,1]], "real_matrix", 0.05)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("nneighbor_t_2                    FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST nneighbor_d_2
    #      Serialization of KD-trees
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            a = [[0,0],[0,1],[1,0],[1,1]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(a)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(a)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(a)
            nx = 2
            ny = 0
            normtype = 2
            r0 = [[]]
            r1 = [[]]

            #
            # Build tree and serialize it
            #
            kdt0 = xalglib.kdtreebuild(a, nx, ny, normtype)
            s = xalglib.kdtreeserialize(kdt0)
            kdt1 = xalglib.kdtreeunserialize(s)

            #
            # Compare results from KNN queries
            #
            x = [-1,0]
            xalglib.kdtreequeryknn(kdt0, x, 1)
            r0 = xalglib.kdtreequeryresultsx(kdt0, r0)
            xalglib.kdtreequeryknn(kdt1, x, 1)
            r1 = xalglib.kdtreequeryresultsx(kdt1, r1)
            _TestResult = _TestResult and doc_print_test(r0, [[0,0]], "real_matrix", 0.05)
            _TestResult = _TestResult and doc_print_test(r1, [[0,0]], "real_matrix", 0.05)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("nneighbor_d_2                    FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST odesolver_d1
    #      Solving y'=-y with ODE solver
    #
    _TestResult = True
    for _spoil_scenario in range(-1,13):
        try:
            y = [1]
            if _spoil_scenario==0:
                spoil_vec_by_nan(y)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(y)
            x = [0, 1, 2, 3]
            if _spoil_scenario==4:
                spoil_vec_by_nan(x)
            if _spoil_scenario==5:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==6:
                spoil_vec_by_neginf(x)
            eps = 0.00001
            if _spoil_scenario==7:
                eps = float("nan")
            if _spoil_scenario==8:
                eps = float("+inf")
            if _spoil_scenario==9:
                eps = float("-inf")
            h = 0
            if _spoil_scenario==10:
                h = float("nan")
            if _spoil_scenario==11:
                h = float("+inf")
            if _spoil_scenario==12:
                h = float("-inf")
            s = xalglib.odesolverrkck(y, x, eps, h)
            xalglib.odesolversolve(s, ode_function_1_diff);
            m, xtbl, ytbl, rep = xalglib.odesolverresults(s)
            _TestResult = _TestResult and doc_print_test(m, 4, "int")
            _TestResult = _TestResult and doc_print_test(xtbl, [0, 1, 2, 3], "real_vector", 0.005)
            _TestResult = _TestResult and doc_print_test(ytbl, [[1], [0.367], [0.135], [0.050]], "real_matrix", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("odesolver_d1                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST matdet_d_1
    #      Determinant calculation, real matrix, short form
    #
    _TestResult = True
    for _spoil_scenario in range(-1,7):
        try:
            b = [[1,2],[2,1]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(b)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(b)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(b)
            if _spoil_scenario==3:
                spoil_mat_by_adding_row(b)
            if _spoil_scenario==4:
                spoil_mat_by_adding_col(b)
            if _spoil_scenario==5:
                spoil_mat_by_deleting_row(b)
            if _spoil_scenario==6:
                spoil_mat_by_deleting_col(b)
            a = xalglib.rmatrixdet(b)
            _TestResult = _TestResult and doc_print_test(a, -3, "real", 0.0001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("matdet_d_1                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST matdet_d_2
    #      Determinant calculation, real matrix, full form
    #
    _TestResult = True
    for _spoil_scenario in range(-1,5):
        try:
            b = [[5,4],[4,5]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(b)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(b)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(b)
            if _spoil_scenario==3:
                spoil_mat_by_deleting_row(b)
            if _spoil_scenario==4:
                spoil_mat_by_deleting_col(b)
            a = xalglib.rmatrixdet(b, 2)
            _TestResult = _TestResult and doc_print_test(a, 9, "real", 0.0001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("matdet_d_2                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST matdet_d_3
    #      Determinant calculation, complex matrix, short form
    #
    _TestResult = True
    for _spoil_scenario in range(-1,7):
        try:
            b = [[1+1j,2],[2,1-1j]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(b)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(b)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(b)
            if _spoil_scenario==3:
                spoil_mat_by_adding_row(b)
            if _spoil_scenario==4:
                spoil_mat_by_adding_col(b)
            if _spoil_scenario==5:
                spoil_mat_by_deleting_row(b)
            if _spoil_scenario==6:
                spoil_mat_by_deleting_col(b)
            a = xalglib.cmatrixdet(b)
            _TestResult = _TestResult and doc_print_test(a, -2, "complex", 0.0001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("matdet_d_3                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST matdet_d_4
    #      Determinant calculation, complex matrix, full form
    #
    _TestResult = True
    for _spoil_scenario in range(-1,5):
        try:
            b = [[5j,4],[4j,5]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(b)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(b)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(b)
            if _spoil_scenario==3:
                spoil_mat_by_deleting_row(b)
            if _spoil_scenario==4:
                spoil_mat_by_deleting_col(b)
            a = xalglib.cmatrixdet(b, 2)
            _TestResult = _TestResult and doc_print_test(a, 9j, "complex", 0.0001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("matdet_d_4                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST matdet_d_5
    #      Determinant calculation, complex matrix with zero imaginary part, short form
    #
    _TestResult = True
    for _spoil_scenario in range(-1,7):
        try:
            b = [[9,1],[2,1]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(b)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(b)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(b)
            if _spoil_scenario==3:
                spoil_mat_by_adding_row(b)
            if _spoil_scenario==4:
                spoil_mat_by_adding_col(b)
            if _spoil_scenario==5:
                spoil_mat_by_deleting_row(b)
            if _spoil_scenario==6:
                spoil_mat_by_deleting_col(b)
            a = xalglib.cmatrixdet(b)
            _TestResult = _TestResult and doc_print_test(a, 7, "complex", 0.0001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("matdet_d_5                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST matdet_t_0
    #      Determinant calculation, real matrix, full form
    #
    _TestResult = True
    for _spoil_scenario in range(-1,5):
        try:
            b = [[3,4],[-4,3]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(b)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(b)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(b)
            if _spoil_scenario==3:
                spoil_mat_by_deleting_row(b)
            if _spoil_scenario==4:
                spoil_mat_by_deleting_col(b)
            a = xalglib.rmatrixdet(b, 2)
            _TestResult = _TestResult and doc_print_test(a, 25, "real", 0.0001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("matdet_t_0                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST matdet_t_1
    #      Determinant calculation, real matrix, LU, short form
    #
    _TestResult = True
    for _spoil_scenario in range(-1,9):
        try:
            b = [[1,2],[2,5]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(b)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(b)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(b)
            if _spoil_scenario==3:
                spoil_mat_by_adding_row(b)
            if _spoil_scenario==4:
                spoil_mat_by_adding_col(b)
            if _spoil_scenario==5:
                spoil_mat_by_deleting_row(b)
            if _spoil_scenario==6:
                spoil_mat_by_deleting_col(b)
            p = [1,1]
            if _spoil_scenario==7:
                spoil_vec_by_adding_element(p)
            if _spoil_scenario==8:
                spoil_vec_by_deleting_element(p)
            a = xalglib.rmatrixludet(b, p)
            _TestResult = _TestResult and doc_print_test(a, -5, "real", 0.0001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("matdet_t_1                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST matdet_t_2
    #      Determinant calculation, real matrix, LU, full form
    #
    _TestResult = True
    for _spoil_scenario in range(-1,6):
        try:
            b = [[5,4],[4,5]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(b)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(b)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(b)
            if _spoil_scenario==3:
                spoil_mat_by_deleting_row(b)
            if _spoil_scenario==4:
                spoil_mat_by_deleting_col(b)
            p = [0,1]
            if _spoil_scenario==5:
                spoil_vec_by_deleting_element(p)
            a = xalglib.rmatrixludet(b, p, 2)
            _TestResult = _TestResult and doc_print_test(a, 25, "real", 0.0001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("matdet_t_2                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST matdet_t_3
    #      Determinant calculation, complex matrix, full form
    #
    _TestResult = True
    for _spoil_scenario in range(-1,5):
        try:
            b = [[5j,4],[-4,5j]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(b)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(b)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(b)
            if _spoil_scenario==3:
                spoil_mat_by_deleting_row(b)
            if _spoil_scenario==4:
                spoil_mat_by_deleting_col(b)
            a = xalglib.cmatrixdet(b, 2)
            _TestResult = _TestResult and doc_print_test(a, -9, "complex", 0.0001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("matdet_t_3                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST matdet_t_4
    #      Determinant calculation, complex matrix, LU, short form
    #
    _TestResult = True
    for _spoil_scenario in range(-1,9):
        try:
            b = [[1,2],[2,5j]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(b)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(b)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(b)
            if _spoil_scenario==3:
                spoil_mat_by_adding_row(b)
            if _spoil_scenario==4:
                spoil_mat_by_adding_col(b)
            if _spoil_scenario==5:
                spoil_mat_by_deleting_row(b)
            if _spoil_scenario==6:
                spoil_mat_by_deleting_col(b)
            p = [1,1]
            if _spoil_scenario==7:
                spoil_vec_by_adding_element(p)
            if _spoil_scenario==8:
                spoil_vec_by_deleting_element(p)
            a = xalglib.cmatrixludet(b, p)
            _TestResult = _TestResult and doc_print_test(a, -5j, "complex", 0.0001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("matdet_t_4                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST matdet_t_5
    #      Determinant calculation, complex matrix, LU, full form
    #
    _TestResult = True
    for _spoil_scenario in range(-1,6):
        try:
            b = [[5,4j],[4,5]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(b)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(b)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(b)
            if _spoil_scenario==3:
                spoil_mat_by_deleting_row(b)
            if _spoil_scenario==4:
                spoil_mat_by_deleting_col(b)
            p = [0,1]
            if _spoil_scenario==5:
                spoil_vec_by_deleting_element(p)
            a = xalglib.cmatrixludet(b, p, 2)
            _TestResult = _TestResult and doc_print_test(a, 25, "complex", 0.0001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("matdet_t_5                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST autogk_d1
    #      Integrating f=exp(x) by adaptive integrator
    #
    _TestResult = True
    for _spoil_scenario in range(-1,6):
        try:
            #
            # This example demonstrates integration of f=exp(x) on [0,1]:
            # * first, autogkstate is initialized
            # * then we call integration function
            # * and finally we obtain results with autogkresults() call
            #
            a = 0
            if _spoil_scenario==0:
                a = float("nan")
            if _spoil_scenario==1:
                a = float("+inf")
            if _spoil_scenario==2:
                a = float("-inf")
            b = 1
            if _spoil_scenario==3:
                b = float("nan")
            if _spoil_scenario==4:
                b = float("+inf")
            if _spoil_scenario==5:
                b = float("-inf")

            s = xalglib.autogksmooth(a, b)
            xalglib.autogkintegrate(s, int_function_1_func);
            v, rep = xalglib.autogkresults(s)

            _TestResult = _TestResult and doc_print_test(v, 1.7182, "real", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("autogk_d1                        FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST basestat_d_base
    #      Basic functionality (moments, adev, median, percentile)
    #
    _TestResult = True
    for _spoil_scenario in range(-1,6):
        try:
            x = [0,1,4,9,16,25,36,49,64,81]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)

            #
            # Here we demonstrate calculation of sample moments
            # (mean, variance, skewness, kurtosis)
            #
            mean, variance, skewness, kurtosis = xalglib.samplemoments(x)
            _TestResult = _TestResult and doc_print_test(mean, 28.5, "real", 0.01)
            _TestResult = _TestResult and doc_print_test(variance, 801.1667, "real", 0.01)
            _TestResult = _TestResult and doc_print_test(skewness, 0.5751, "real", 0.01)
            _TestResult = _TestResult and doc_print_test(kurtosis, -1.2666, "real", 0.01)

            #
            # Average deviation
            #
            adev = xalglib.sampleadev(x)
            _TestResult = _TestResult and doc_print_test(adev, 23.2, "real", 0.01)

            #
            # Median and percentile
            #
            v = xalglib.samplemedian(x)
            _TestResult = _TestResult and doc_print_test(v, 20.5, "real", 0.01)
            p = 0.5
            if _spoil_scenario==3:
                p = float("nan")
            if _spoil_scenario==4:
                p = float("+inf")
            if _spoil_scenario==5:
                p = float("-inf")
            v = xalglib.samplepercentile(x, p)
            _TestResult = _TestResult and doc_print_test(v, 20.5, "real", 0.01)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("basestat_d_base                  FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST basestat_d_c2
    #      Correlation (covariance) between two random variables
    #
    _TestResult = True
    for _spoil_scenario in range(-1,10):
        try:
            #
            # We have two samples - x and y, and want to measure dependency between them
            #
            x = [0,1,4,9,16,25,36,49,64,81]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_adding_element(x)
            if _spoil_scenario==4:
                spoil_vec_by_deleting_element(x)
            y = [0,1,2,3,4,5,6,7,8,9]
            if _spoil_scenario==5:
                spoil_vec_by_nan(y)
            if _spoil_scenario==6:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==7:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==8:
                spoil_vec_by_adding_element(y)
            if _spoil_scenario==9:
                spoil_vec_by_deleting_element(y)

            #
            # Three dependency measures are calculated:
            # * covariation
            # * Pearson correlation
            # * Spearman rank correlation
            #
            v = xalglib.cov2(x, y)
            _TestResult = _TestResult and doc_print_test(v, 82.5, "real", 0.001)
            v = xalglib.pearsoncorr2(x, y)
            _TestResult = _TestResult and doc_print_test(v, 0.9627, "real", 0.001)
            v = xalglib.spearmancorr2(x, y)
            _TestResult = _TestResult and doc_print_test(v, 1.000, "real", 0.001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("basestat_d_c2                    FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST basestat_d_cm
    #      Correlation (covariance) between components of random vector
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # X is a sample matrix:
            # * I-th row corresponds to I-th observation
            # * J-th column corresponds to J-th variable
            #
            x = [[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(x)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(x)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(x)

            #
            # Three dependency measures are calculated:
            # * covariation
            # * Pearson correlation
            # * Spearman rank correlation
            #
            # Result is stored into C, with C[i,j] equal to correlation
            # (covariance) between I-th and J-th variables of X.
            #
            c = xalglib.covm(x)
            _TestResult = _TestResult and doc_print_test(c, [[1.80,0.60,-1.40],[0.60,0.70,-0.80],[-1.40,-0.80,14.70]], "real_matrix", 0.01)
            c = xalglib.pearsoncorrm(x)
            _TestResult = _TestResult and doc_print_test(c, [[1.000,0.535,-0.272],[0.535,1.000,-0.249],[-0.272,-0.249,1.000]], "real_matrix", 0.01)
            c = xalglib.spearmancorrm(x)
            _TestResult = _TestResult and doc_print_test(c, [[1.000,0.556,-0.306],[0.556,1.000,-0.750],[-0.306,-0.750,1.000]], "real_matrix", 0.01)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("basestat_d_cm                    FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST basestat_d_cm2
    #      Correlation (covariance) between two random vectors
    #
    _TestResult = True
    for _spoil_scenario in range(-1,6):
        try:
            #
            # X and Y are sample matrices:
            # * I-th row corresponds to I-th observation
            # * J-th column corresponds to J-th variable
            #
            x = [[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(x)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(x)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(x)
            y = [[2,3],[2,1],[-1,6],[-9,9],[7,1]]
            if _spoil_scenario==3:
                spoil_mat_by_nan(y)
            if _spoil_scenario==4:
                spoil_mat_by_posinf(y)
            if _spoil_scenario==5:
                spoil_mat_by_neginf(y)

            #
            # Three dependency measures are calculated:
            # * covariation
            # * Pearson correlation
            # * Spearman rank correlation
            #
            # Result is stored into C, with C[i,j] equal to correlation
            # (covariance) between I-th variable of X and J-th variable of Y.
            #
            c = xalglib.covm2(x, y)
            _TestResult = _TestResult and doc_print_test(c, [[4.100,-3.250],[2.450,-1.500],[13.450,-5.750]], "real_matrix", 0.01)
            c = xalglib.pearsoncorrm2(x, y)
            _TestResult = _TestResult and doc_print_test(c, [[0.519,-0.699],[0.497,-0.518],[0.596,-0.433]], "real_matrix", 0.01)
            c = xalglib.spearmancorrm2(x, y)
            _TestResult = _TestResult and doc_print_test(c, [[0.541,-0.649],[0.216,-0.433],[0.433,-0.135]], "real_matrix", 0.01)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("basestat_d_cm2                   FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST basestat_t_base
    #      Tests ability to detect errors in inputs
    #
    _TestResult = True
    for _spoil_scenario in range(-1,34):
        try:

            #
            # first, we test short form of functions
            #
            x1 = [0,1,4,9,16,25,36,49,64,81]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x1)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x1)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x1)
            mean, variance, skewness, kurtosis = xalglib.samplemoments(x1)
            x2 = [0,1,4,9,16,25,36,49,64,81]
            if _spoil_scenario==3:
                spoil_vec_by_nan(x2)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(x2)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(x2)
            adev = xalglib.sampleadev(x2)
            x3 = [0,1,4,9,16,25,36,49,64,81]
            if _spoil_scenario==6:
                spoil_vec_by_nan(x3)
            if _spoil_scenario==7:
                spoil_vec_by_posinf(x3)
            if _spoil_scenario==8:
                spoil_vec_by_neginf(x3)
            v = xalglib.samplemedian(x3)
            x4 = [0,1,4,9,16,25,36,49,64,81]
            if _spoil_scenario==9:
                spoil_vec_by_nan(x4)
            if _spoil_scenario==10:
                spoil_vec_by_posinf(x4)
            if _spoil_scenario==11:
                spoil_vec_by_neginf(x4)
            p = 0.5
            if _spoil_scenario==12:
                p = float("nan")
            if _spoil_scenario==13:
                p = float("+inf")
            if _spoil_scenario==14:
                p = float("-inf")
            v = xalglib.samplepercentile(x4, p)

            #
            # and then we test full form
            #
            x5 = [0,1,4,9,16,25,36,49,64,81]
            if _spoil_scenario==15:
                spoil_vec_by_nan(x5)
            if _spoil_scenario==16:
                spoil_vec_by_posinf(x5)
            if _spoil_scenario==17:
                spoil_vec_by_neginf(x5)
            if _spoil_scenario==18:
                spoil_vec_by_deleting_element(x5)
            mean, variance, skewness, kurtosis = xalglib.samplemoments(x5, 10)
            x6 = [0,1,4,9,16,25,36,49,64,81]
            if _spoil_scenario==19:
                spoil_vec_by_nan(x6)
            if _spoil_scenario==20:
                spoil_vec_by_posinf(x6)
            if _spoil_scenario==21:
                spoil_vec_by_neginf(x6)
            if _spoil_scenario==22:
                spoil_vec_by_deleting_element(x6)
            adev = xalglib.sampleadev(x6, 10)
            x7 = [0,1,4,9,16,25,36,49,64,81]
            if _spoil_scenario==23:
                spoil_vec_by_nan(x7)
            if _spoil_scenario==24:
                spoil_vec_by_posinf(x7)
            if _spoil_scenario==25:
                spoil_vec_by_neginf(x7)
            if _spoil_scenario==26:
                spoil_vec_by_deleting_element(x7)
            v = xalglib.samplemedian(x7, 10)
            x8 = [0,1,4,9,16,25,36,49,64,81]
            if _spoil_scenario==27:
                spoil_vec_by_nan(x8)
            if _spoil_scenario==28:
                spoil_vec_by_posinf(x8)
            if _spoil_scenario==29:
                spoil_vec_by_neginf(x8)
            if _spoil_scenario==30:
                spoil_vec_by_deleting_element(x8)
            p = 0.5
            if _spoil_scenario==31:
                p = float("nan")
            if _spoil_scenario==32:
                p = float("+inf")
            if _spoil_scenario==33:
                p = float("-inf")
            v = xalglib.samplepercentile(x8, 10, p)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("basestat_t_base                  FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST basestat_t_covcorr
    #      Tests ability to detect errors in inputs
    #
    _TestResult = True
    for _spoil_scenario in range(-1,126):
        try:

            #
            # 2-sample short-form cov/corr are tested
            #
            x1 = [0,1,4,9,16,25,36,49,64,81]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x1)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x1)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x1)
            if _spoil_scenario==3:
                spoil_vec_by_adding_element(x1)
            if _spoil_scenario==4:
                spoil_vec_by_deleting_element(x1)
            y1 = [0,1,2,3,4,5,6,7,8,9]
            if _spoil_scenario==5:
                spoil_vec_by_nan(y1)
            if _spoil_scenario==6:
                spoil_vec_by_posinf(y1)
            if _spoil_scenario==7:
                spoil_vec_by_neginf(y1)
            if _spoil_scenario==8:
                spoil_vec_by_adding_element(y1)
            if _spoil_scenario==9:
                spoil_vec_by_deleting_element(y1)
            v = xalglib.cov2(x1, y1)
            x2 = [0,1,4,9,16,25,36,49,64,81]
            if _spoil_scenario==10:
                spoil_vec_by_nan(x2)
            if _spoil_scenario==11:
                spoil_vec_by_posinf(x2)
            if _spoil_scenario==12:
                spoil_vec_by_neginf(x2)
            if _spoil_scenario==13:
                spoil_vec_by_adding_element(x2)
            if _spoil_scenario==14:
                spoil_vec_by_deleting_element(x2)
            y2 = [0,1,2,3,4,5,6,7,8,9]
            if _spoil_scenario==15:
                spoil_vec_by_nan(y2)
            if _spoil_scenario==16:
                spoil_vec_by_posinf(y2)
            if _spoil_scenario==17:
                spoil_vec_by_neginf(y2)
            if _spoil_scenario==18:
                spoil_vec_by_adding_element(y2)
            if _spoil_scenario==19:
                spoil_vec_by_deleting_element(y2)
            v = xalglib.pearsoncorr2(x2, y2)
            x3 = [0,1,4,9,16,25,36,49,64,81]
            if _spoil_scenario==20:
                spoil_vec_by_nan(x3)
            if _spoil_scenario==21:
                spoil_vec_by_posinf(x3)
            if _spoil_scenario==22:
                spoil_vec_by_neginf(x3)
            if _spoil_scenario==23:
                spoil_vec_by_adding_element(x3)
            if _spoil_scenario==24:
                spoil_vec_by_deleting_element(x3)
            y3 = [0,1,2,3,4,5,6,7,8,9]
            if _spoil_scenario==25:
                spoil_vec_by_nan(y3)
            if _spoil_scenario==26:
                spoil_vec_by_posinf(y3)
            if _spoil_scenario==27:
                spoil_vec_by_neginf(y3)
            if _spoil_scenario==28:
                spoil_vec_by_adding_element(y3)
            if _spoil_scenario==29:
                spoil_vec_by_deleting_element(y3)
            v = xalglib.spearmancorr2(x3, y3)

            #
            # 2-sample full-form cov/corr are tested
            #
            x1a = [0,1,4,9,16,25,36,49,64,81]
            if _spoil_scenario==30:
                spoil_vec_by_nan(x1a)
            if _spoil_scenario==31:
                spoil_vec_by_posinf(x1a)
            if _spoil_scenario==32:
                spoil_vec_by_neginf(x1a)
            if _spoil_scenario==33:
                spoil_vec_by_deleting_element(x1a)
            y1a = [0,1,2,3,4,5,6,7,8,9]
            if _spoil_scenario==34:
                spoil_vec_by_nan(y1a)
            if _spoil_scenario==35:
                spoil_vec_by_posinf(y1a)
            if _spoil_scenario==36:
                spoil_vec_by_neginf(y1a)
            if _spoil_scenario==37:
                spoil_vec_by_deleting_element(y1a)
            v = xalglib.cov2(x1a, y1a, 10)
            x2a = [0,1,4,9,16,25,36,49,64,81]
            if _spoil_scenario==38:
                spoil_vec_by_nan(x2a)
            if _spoil_scenario==39:
                spoil_vec_by_posinf(x2a)
            if _spoil_scenario==40:
                spoil_vec_by_neginf(x2a)
            if _spoil_scenario==41:
                spoil_vec_by_deleting_element(x2a)
            y2a = [0,1,2,3,4,5,6,7,8,9]
            if _spoil_scenario==42:
                spoil_vec_by_nan(y2a)
            if _spoil_scenario==43:
                spoil_vec_by_posinf(y2a)
            if _spoil_scenario==44:
                spoil_vec_by_neginf(y2a)
            if _spoil_scenario==45:
                spoil_vec_by_deleting_element(y2a)
            v = xalglib.pearsoncorr2(x2a, y2a, 10)
            x3a = [0,1,4,9,16,25,36,49,64,81]
            if _spoil_scenario==46:
                spoil_vec_by_nan(x3a)
            if _spoil_scenario==47:
                spoil_vec_by_posinf(x3a)
            if _spoil_scenario==48:
                spoil_vec_by_neginf(x3a)
            if _spoil_scenario==49:
                spoil_vec_by_deleting_element(x3a)
            y3a = [0,1,2,3,4,5,6,7,8,9]
            if _spoil_scenario==50:
                spoil_vec_by_nan(y3a)
            if _spoil_scenario==51:
                spoil_vec_by_posinf(y3a)
            if _spoil_scenario==52:
                spoil_vec_by_neginf(y3a)
            if _spoil_scenario==53:
                spoil_vec_by_deleting_element(y3a)
            v = xalglib.spearmancorr2(x3a, y3a, 10)

            #
            # vector short-form cov/corr are tested.
            #
            x4 = [[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]
            if _spoil_scenario==54:
                spoil_mat_by_nan(x4)
            if _spoil_scenario==55:
                spoil_mat_by_posinf(x4)
            if _spoil_scenario==56:
                spoil_mat_by_neginf(x4)
            c = xalglib.covm(x4)
            x5 = [[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]
            if _spoil_scenario==57:
                spoil_mat_by_nan(x5)
            if _spoil_scenario==58:
                spoil_mat_by_posinf(x5)
            if _spoil_scenario==59:
                spoil_mat_by_neginf(x5)
            c = xalglib.pearsoncorrm(x5)
            x6 = [[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]
            if _spoil_scenario==60:
                spoil_mat_by_nan(x6)
            if _spoil_scenario==61:
                spoil_mat_by_posinf(x6)
            if _spoil_scenario==62:
                spoil_mat_by_neginf(x6)
            c = xalglib.spearmancorrm(x6)

            #
            # vector full-form cov/corr are tested.
            #
            x7 = [[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]
            if _spoil_scenario==63:
                spoil_mat_by_nan(x7)
            if _spoil_scenario==64:
                spoil_mat_by_posinf(x7)
            if _spoil_scenario==65:
                spoil_mat_by_neginf(x7)
            if _spoil_scenario==66:
                spoil_mat_by_deleting_row(x7)
            if _spoil_scenario==67:
                spoil_mat_by_deleting_col(x7)
            c = xalglib.covm(x7, 5, 3)
            x8 = [[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]
            if _spoil_scenario==68:
                spoil_mat_by_nan(x8)
            if _spoil_scenario==69:
                spoil_mat_by_posinf(x8)
            if _spoil_scenario==70:
                spoil_mat_by_neginf(x8)
            if _spoil_scenario==71:
                spoil_mat_by_deleting_row(x8)
            if _spoil_scenario==72:
                spoil_mat_by_deleting_col(x8)
            c = xalglib.pearsoncorrm(x8, 5, 3)
            x9 = [[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]
            if _spoil_scenario==73:
                spoil_mat_by_nan(x9)
            if _spoil_scenario==74:
                spoil_mat_by_posinf(x9)
            if _spoil_scenario==75:
                spoil_mat_by_neginf(x9)
            if _spoil_scenario==76:
                spoil_mat_by_deleting_row(x9)
            if _spoil_scenario==77:
                spoil_mat_by_deleting_col(x9)
            c = xalglib.spearmancorrm(x9, 5, 3)

            #
            # cross-vector short-form cov/corr are tested.
            #
            x10 = [[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]
            if _spoil_scenario==78:
                spoil_mat_by_nan(x10)
            if _spoil_scenario==79:
                spoil_mat_by_posinf(x10)
            if _spoil_scenario==80:
                spoil_mat_by_neginf(x10)
            y10 = [[2,3],[2,1],[-1,6],[-9,9],[7,1]]
            if _spoil_scenario==81:
                spoil_mat_by_nan(y10)
            if _spoil_scenario==82:
                spoil_mat_by_posinf(y10)
            if _spoil_scenario==83:
                spoil_mat_by_neginf(y10)
            c = xalglib.covm2(x10, y10)
            x11 = [[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]
            if _spoil_scenario==84:
                spoil_mat_by_nan(x11)
            if _spoil_scenario==85:
                spoil_mat_by_posinf(x11)
            if _spoil_scenario==86:
                spoil_mat_by_neginf(x11)
            y11 = [[2,3],[2,1],[-1,6],[-9,9],[7,1]]
            if _spoil_scenario==87:
                spoil_mat_by_nan(y11)
            if _spoil_scenario==88:
                spoil_mat_by_posinf(y11)
            if _spoil_scenario==89:
                spoil_mat_by_neginf(y11)
            c = xalglib.pearsoncorrm2(x11, y11)
            x12 = [[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]
            if _spoil_scenario==90:
                spoil_mat_by_nan(x12)
            if _spoil_scenario==91:
                spoil_mat_by_posinf(x12)
            if _spoil_scenario==92:
                spoil_mat_by_neginf(x12)
            y12 = [[2,3],[2,1],[-1,6],[-9,9],[7,1]]
            if _spoil_scenario==93:
                spoil_mat_by_nan(y12)
            if _spoil_scenario==94:
                spoil_mat_by_posinf(y12)
            if _spoil_scenario==95:
                spoil_mat_by_neginf(y12)
            c = xalglib.spearmancorrm2(x12, y12)

            #
            # cross-vector full-form cov/corr are tested.
            #
            x13 = [[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]
            if _spoil_scenario==96:
                spoil_mat_by_nan(x13)
            if _spoil_scenario==97:
                spoil_mat_by_posinf(x13)
            if _spoil_scenario==98:
                spoil_mat_by_neginf(x13)
            if _spoil_scenario==99:
                spoil_mat_by_deleting_row(x13)
            if _spoil_scenario==100:
                spoil_mat_by_deleting_col(x13)
            y13 = [[2,3],[2,1],[-1,6],[-9,9],[7,1]]
            if _spoil_scenario==101:
                spoil_mat_by_nan(y13)
            if _spoil_scenario==102:
                spoil_mat_by_posinf(y13)
            if _spoil_scenario==103:
                spoil_mat_by_neginf(y13)
            if _spoil_scenario==104:
                spoil_mat_by_deleting_row(y13)
            if _spoil_scenario==105:
                spoil_mat_by_deleting_col(y13)
            c = xalglib.covm2(x13, y13, 5, 3, 2)
            x14 = [[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]
            if _spoil_scenario==106:
                spoil_mat_by_nan(x14)
            if _spoil_scenario==107:
                spoil_mat_by_posinf(x14)
            if _spoil_scenario==108:
                spoil_mat_by_neginf(x14)
            if _spoil_scenario==109:
                spoil_mat_by_deleting_row(x14)
            if _spoil_scenario==110:
                spoil_mat_by_deleting_col(x14)
            y14 = [[2,3],[2,1],[-1,6],[-9,9],[7,1]]
            if _spoil_scenario==111:
                spoil_mat_by_nan(y14)
            if _spoil_scenario==112:
                spoil_mat_by_posinf(y14)
            if _spoil_scenario==113:
                spoil_mat_by_neginf(y14)
            if _spoil_scenario==114:
                spoil_mat_by_deleting_row(y14)
            if _spoil_scenario==115:
                spoil_mat_by_deleting_col(y14)
            c = xalglib.pearsoncorrm2(x14, y14, 5, 3, 2)
            x15 = [[1,0,1],[1,1,0],[-1,1,0],[-2,-1,1],[-1,0,9]]
            if _spoil_scenario==116:
                spoil_mat_by_nan(x15)
            if _spoil_scenario==117:
                spoil_mat_by_posinf(x15)
            if _spoil_scenario==118:
                spoil_mat_by_neginf(x15)
            if _spoil_scenario==119:
                spoil_mat_by_deleting_row(x15)
            if _spoil_scenario==120:
                spoil_mat_by_deleting_col(x15)
            y15 = [[2,3],[2,1],[-1,6],[-9,9],[7,1]]
            if _spoil_scenario==121:
                spoil_mat_by_nan(y15)
            if _spoil_scenario==122:
                spoil_mat_by_posinf(y15)
            if _spoil_scenario==123:
                spoil_mat_by_neginf(y15)
            if _spoil_scenario==124:
                spoil_mat_by_deleting_row(y15)
            if _spoil_scenario==125:
                spoil_mat_by_deleting_col(y15)
            c = xalglib.spearmancorrm2(x15, y15, 5, 3, 2)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("basestat_t_covcorr               FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST idw_d_mstab
    #      Simple model built with IDW-MSTAB algorithm
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # This example illustrates basic concepts of the IDW models:
            # creation and evaluation.
            # 
            # Suppose that we have set of 2-dimensional points with associated
            # scalar function values, and we want to build an IDW model using
            # our data.
            # 
            # NOTE: we can work with N-dimensional models and vector-valued functions too :)
            # 
            # Typical sequence of steps is given below:
            # 1. we create IDW builder object
            # 2. we attach our dataset to the IDW builder and tune algorithm settings
            # 3. we generate IDW model
            # 4. we use IDW model instance (evaluate, serialize, etc.)
            #

            #
            # Step 1: IDW builder creation.
            #
            # We have to specify dimensionality of the space (2 or 3) and
            # dimensionality of the function (scalar or vector).
            #
            # New builder object is empty - it has not dataset and uses
            # default model construction settings
            #
            builder = xalglib.idwbuildercreate(2, 1)

            #
            # Step 2: dataset addition
            #
            # XY contains two points - x0=(-1,0) and x1=(+1,0) -
            # and two function values f(x0)=2, f(x1)=3.
            #
            xy = [[-1,0,2],[+1,0,3]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)
            xalglib.idwbuildersetpoints(builder, xy)

            #
            # Step 3: choose IDW algorithm and generate model
            #
            # We use modified stabilized IDW algorithm with following parameters:
            # * SRad - set to 5.0 (search radius must be large enough)
            #
            # IDW-MSTAB algorithm is a state-of-the-art implementation of IDW which
            # is competitive with RBFs and bicubic splines. See comments on the
            # idwbuildersetalgomstab() function for more information.
            #
            xalglib.idwbuildersetalgomstab(builder, 5.0)
            model, rep = xalglib.idwfit(builder)

            #
            # Step 4: model was built, evaluate its value
            #
            v = xalglib.idwcalc2(model, 1.0, 0.0)
            _TestResult = _TestResult and doc_print_test(v, 3.000, "real", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("idw_d_mstab                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST idw_d_serialize
    #      IDW model serialization/unserialization
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # This example shows how to serialize and unserialize IDW model.
            # 
            # Suppose that we have set of 2-dimensional points with associated
            # scalar function values, and we have built an IDW model using
            # our data.
            #
            # This model can be serialized to string or stream. ALGLIB supports
            # flexible (un)serialization, i.e. you can move serialized model
            # representation between different machines (32-bit or 64-bit),
            # different CPU architectures (x86/64, ARM) or even different
            # programming languages supported by ALGLIB (C#, C++, ...).
            #
            # Our first step is to build model, evaluate it at point (1,0),
            # and serialize it to string.
            #
            xy = [[-1,0,2],[+1,0,3]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)
            builder = xalglib.idwbuildercreate(2, 1)
            xalglib.idwbuildersetpoints(builder, xy)
            xalglib.idwbuildersetalgomstab(builder, 5.0)
            model, rep = xalglib.idwfit(builder)
            v = xalglib.idwcalc2(model, 1.0, 0.0)
            _TestResult = _TestResult and doc_print_test(v, 3.000, "real", 0.005)

            #
            # Serialization + unserialization to a different instance
            # of the model class.
            #
            s = xalglib.idwserialize(model)
            model2 = xalglib.idwunserialize(s)

            #
            # Evaluate unserialized model at the same point
            #
            v = xalglib.idwcalc2(model2, 1.0, 0.0)
            _TestResult = _TestResult and doc_print_test(v, 3.000, "real", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("idw_d_serialize                  FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST polint_d_calcdiff
    #      Interpolation and differentiation using barycentric representation
    #
    _TestResult = True
    for _spoil_scenario in range(-1,12):
        try:
            #
            # Here we demonstrate polynomial interpolation and differentiation
            # of y=x^2-x sampled at [0,1,2]. Barycentric representation of polynomial is used.
            #
            x = [0,1,2]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_adding_element(x)
            if _spoil_scenario==4:
                spoil_vec_by_deleting_element(x)
            y = [0,0,2]
            if _spoil_scenario==5:
                spoil_vec_by_nan(y)
            if _spoil_scenario==6:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==7:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==8:
                spoil_vec_by_adding_element(y)
            if _spoil_scenario==9:
                spoil_vec_by_deleting_element(y)
            t = -1
            if _spoil_scenario==10:
                t = float("+inf")
            if _spoil_scenario==11:
                t = float("-inf")

            # barycentric model is created
            p = xalglib.polynomialbuild(x, y)

            # barycentric interpolation is demonstrated
            v = xalglib.barycentriccalc(p, t)
            _TestResult = _TestResult and doc_print_test(v, 2.0, "real", 0.00005)

            # barycentric differentation is demonstrated
            v, dv = xalglib.barycentricdiff1(p, t)
            _TestResult = _TestResult and doc_print_test(v, 2.0, "real", 0.00005)
            _TestResult = _TestResult and doc_print_test(dv, -3.0, "real", 0.00005)

            # second derivatives with barycentric representation
            v, dv = xalglib.barycentricdiff1(p, t)
            _TestResult = _TestResult and doc_print_test(v, 2.0, "real", 0.00005)
            _TestResult = _TestResult and doc_print_test(dv, -3.0, "real", 0.00005)
            v, dv, d2v = xalglib.barycentricdiff2(p, t)
            _TestResult = _TestResult and doc_print_test(v, 2.0, "real", 0.00005)
            _TestResult = _TestResult and doc_print_test(dv, -3.0, "real", 0.00005)
            _TestResult = _TestResult and doc_print_test(d2v, 2.0, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("polint_d_calcdiff                FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST polint_d_conv
    #      Conversion between power basis and barycentric representation
    #
    _TestResult = True
    for _spoil_scenario in range(-1,5):
        try:
            #
            # Here we demonstrate conversion of y=x^2-x
            # between power basis and barycentric representation.
            #
            a = [0,-1,+1]
            if _spoil_scenario==0:
                spoil_vec_by_nan(a)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(a)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(a)
            t = 2
            if _spoil_scenario==3:
                t = float("+inf")
            if _spoil_scenario==4:
                t = float("-inf")

            #
            # a=[0,-1,+1] is decomposition of y=x^2-x in the power basis:
            #
            #     y = 0 - 1*x + 1*x^2
            #
            # We convert it to the barycentric form.
            #
            p = xalglib.polynomialpow2bar(a)

            # now we have barycentric interpolation; we can use it for interpolation
            v = xalglib.barycentriccalc(p, t)
            _TestResult = _TestResult and doc_print_test(v, 2.0, "real", 0.005)

            # we can also convert back from barycentric representation to power basis
            a2 = xalglib.polynomialbar2pow(p)
            _TestResult = _TestResult and doc_print_test(a2, [0,-1,+1], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("polint_d_conv                    FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST polint_d_spec
    #      Polynomial interpolation on special grids (equidistant, Chebyshev I/II)
    #
    _TestResult = True
    for _spoil_scenario in range(-1,11):
        try:
            #
            # Temporaries:
            # * values of y=x^2-x sampled at three special grids:
            #   * equdistant grid spanning [0,2],     x[i] = 2*i/(N-1), i=0..N-1
            #   * Chebyshev-I grid spanning [-1,+1],  x[i] = 1 + Cos(PI*(2*i+1)/(2*n)), i=0..N-1
            #   * Chebyshev-II grid spanning [-1,+1], x[i] = 1 + Cos(PI*i/(n-1)), i=0..N-1
            # * barycentric interpolants for these three grids
            # * vectors to store coefficients of quadratic representation
            #
            y_eqdist = [0,0,2]
            if _spoil_scenario==0:
                spoil_vec_by_nan(y_eqdist)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(y_eqdist)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(y_eqdist)
            y_cheb1 = [-0.116025,0.000000,1.616025]
            if _spoil_scenario==3:
                spoil_vec_by_nan(y_cheb1)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(y_cheb1)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(y_cheb1)
            y_cheb2 = [0,0,2]
            if _spoil_scenario==6:
                spoil_vec_by_nan(y_cheb2)
            if _spoil_scenario==7:
                spoil_vec_by_posinf(y_cheb2)
            if _spoil_scenario==8:
                spoil_vec_by_neginf(y_cheb2)

            #
            # First, we demonstrate construction of barycentric interpolants on
            # special grids. We unpack power representation to ensure that
            # interpolant was built correctly.
            #
            # In all three cases we should get same quadratic function.
            #
            p_eqdist = xalglib.polynomialbuildeqdist(0.0, 2.0, y_eqdist)
            a_eqdist = xalglib.polynomialbar2pow(p_eqdist)
            _TestResult = _TestResult and doc_print_test(a_eqdist, [0,-1,+1], "real_vector", 0.00005)

            p_cheb1 = xalglib.polynomialbuildcheb1(-1, +1, y_cheb1)
            a_cheb1 = xalglib.polynomialbar2pow(p_cheb1)
            _TestResult = _TestResult and doc_print_test(a_cheb1, [0,-1,+1], "real_vector", 0.00005)

            p_cheb2 = xalglib.polynomialbuildcheb2(-1, +1, y_cheb2)
            a_cheb2 = xalglib.polynomialbar2pow(p_cheb2)
            _TestResult = _TestResult and doc_print_test(a_cheb2, [0,-1,+1], "real_vector", 0.00005)

            #
            # Now we demonstrate polynomial interpolation without construction 
            # of the barycentricinterpolant structure.
            #
            # We calculate interpolant value at x=-2.
            # In all three cases we should get same f=6
            #
            t = -2
            if _spoil_scenario==9:
                t = float("+inf")
            if _spoil_scenario==10:
                t = float("-inf")
            v = xalglib.polynomialcalceqdist(0.0, 2.0, y_eqdist, t)
            _TestResult = _TestResult and doc_print_test(v, 6.0, "real", 0.00005)

            v = xalglib.polynomialcalccheb1(-1, +1, y_cheb1, t)
            _TestResult = _TestResult and doc_print_test(v, 6.0, "real", 0.00005)

            v = xalglib.polynomialcalccheb2(-1, +1, y_cheb2, t)
            _TestResult = _TestResult and doc_print_test(v, 6.0, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("polint_d_spec                    FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST polint_t_1
    #      Polynomial interpolation, full list of parameters.
    #
    _TestResult = True
    for _spoil_scenario in range(-1,10):
        try:
            x = [0,1,2]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(x)
            y = [0,0,2]
            if _spoil_scenario==4:
                spoil_vec_by_nan(y)
            if _spoil_scenario==5:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==6:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==7:
                spoil_vec_by_deleting_element(y)
            t = -1
            if _spoil_scenario==8:
                t = float("+inf")
            if _spoil_scenario==9:
                t = float("-inf")
            p = xalglib.polynomialbuild(x, y, 3)
            v = xalglib.barycentriccalc(p, t)
            _TestResult = _TestResult and doc_print_test(v, 2.0, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("polint_t_1                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST polint_t_2
    #      Polynomial interpolation, full list of parameters.
    #
    _TestResult = True
    for _spoil_scenario in range(-1,6):
        try:
            y = [0,0,2]
            if _spoil_scenario==0:
                spoil_vec_by_nan(y)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(y)
            t = -1
            if _spoil_scenario==4:
                t = float("+inf")
            if _spoil_scenario==5:
                t = float("-inf")
            p = xalglib.polynomialbuildeqdist(0.0, 2.0, y, 3)
            v = xalglib.barycentriccalc(p, t)
            _TestResult = _TestResult and doc_print_test(v, 2.0, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("polint_t_2                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST polint_t_3
    #      Polynomial interpolation, full list of parameters.
    #
    _TestResult = True
    for _spoil_scenario in range(-1,6):
        try:
            y = [-0.116025,0.000000,1.616025]
            if _spoil_scenario==0:
                spoil_vec_by_nan(y)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(y)
            t = -1
            if _spoil_scenario==4:
                t = float("+inf")
            if _spoil_scenario==5:
                t = float("-inf")
            p = xalglib.polynomialbuildcheb1(-1.0, +1.0, y, 3)
            v = xalglib.barycentriccalc(p, t)
            _TestResult = _TestResult and doc_print_test(v, 2.0, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("polint_t_3                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST polint_t_4
    #      Polynomial interpolation, full list of parameters.
    #
    _TestResult = True
    for _spoil_scenario in range(-1,12):
        try:
            y = [0,0,2]
            if _spoil_scenario==0:
                spoil_vec_by_nan(y)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(y)
            t = -2
            if _spoil_scenario==4:
                t = float("+inf")
            if _spoil_scenario==5:
                t = float("-inf")
            a = -1
            if _spoil_scenario==6:
                a = float("nan")
            if _spoil_scenario==7:
                a = float("+inf")
            if _spoil_scenario==8:
                a = float("-inf")
            b = +1
            if _spoil_scenario==9:
                b = float("nan")
            if _spoil_scenario==10:
                b = float("+inf")
            if _spoil_scenario==11:
                b = float("-inf")
            p = xalglib.polynomialbuildcheb2(a, b, y, 3)
            v = xalglib.barycentriccalc(p, t)
            _TestResult = _TestResult and doc_print_test(v, 6.0, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("polint_t_4                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST polint_t_5
    #      Polynomial interpolation, full list of parameters.
    #
    _TestResult = True
    for _spoil_scenario in range(-1,6):
        try:
            y = [0,0,2]
            if _spoil_scenario==0:
                spoil_vec_by_nan(y)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(y)
            t = -1
            if _spoil_scenario==4:
                t = float("+inf")
            if _spoil_scenario==5:
                t = float("-inf")
            v = xalglib.polynomialcalceqdist(0.0, 2.0, y, 3, t)
            _TestResult = _TestResult and doc_print_test(v, 2.0, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("polint_t_5                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST polint_t_6
    #      Polynomial interpolation, full list of parameters.
    #
    _TestResult = True
    for _spoil_scenario in range(-1,12):
        try:
            y = [-0.116025,0.000000,1.616025]
            if _spoil_scenario==0:
                spoil_vec_by_nan(y)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(y)
            t = -1
            if _spoil_scenario==4:
                t = float("+inf")
            if _spoil_scenario==5:
                t = float("-inf")
            a = -1
            if _spoil_scenario==6:
                a = float("nan")
            if _spoil_scenario==7:
                a = float("+inf")
            if _spoil_scenario==8:
                a = float("-inf")
            b = +1
            if _spoil_scenario==9:
                b = float("nan")
            if _spoil_scenario==10:
                b = float("+inf")
            if _spoil_scenario==11:
                b = float("-inf")
            v = xalglib.polynomialcalccheb1(a, b, y, 3, t)
            _TestResult = _TestResult and doc_print_test(v, 2.0, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("polint_t_6                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST polint_t_7
    #      Polynomial interpolation, full list of parameters.
    #
    _TestResult = True
    for _spoil_scenario in range(-1,12):
        try:
            y = [0,0,2]
            if _spoil_scenario==0:
                spoil_vec_by_nan(y)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(y)
            t = -2
            if _spoil_scenario==4:
                t = float("+inf")
            if _spoil_scenario==5:
                t = float("-inf")
            a = -1
            if _spoil_scenario==6:
                a = float("nan")
            if _spoil_scenario==7:
                a = float("+inf")
            if _spoil_scenario==8:
                a = float("-inf")
            b = +1
            if _spoil_scenario==9:
                b = float("nan")
            if _spoil_scenario==10:
                b = float("+inf")
            if _spoil_scenario==11:
                b = float("-inf")
            v = xalglib.polynomialcalccheb2(a, b, y, 3, t)
            _TestResult = _TestResult and doc_print_test(v, 6.0, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("polint_t_7                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST polint_t_8
    #      Polynomial interpolation: y=x^2-x, equidistant grid, barycentric form
    #
    _TestResult = True
    for _spoil_scenario in range(-1,5):
        try:
            y = [0,0,2]
            if _spoil_scenario==0:
                spoil_vec_by_nan(y)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(y)
            t = -1
            if _spoil_scenario==3:
                t = float("+inf")
            if _spoil_scenario==4:
                t = float("-inf")
            p = xalglib.polynomialbuildeqdist(0.0, 2.0, y)
            v = xalglib.barycentriccalc(p, t)
            _TestResult = _TestResult and doc_print_test(v, 2.0, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("polint_t_8                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST polint_t_9
    #      Polynomial interpolation: y=x^2-x, Chebyshev grid (first kind), barycentric form
    #
    _TestResult = True
    for _spoil_scenario in range(-1,11):
        try:
            y = [-0.116025,0.000000,1.616025]
            if _spoil_scenario==0:
                spoil_vec_by_nan(y)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(y)
            t = -1
            if _spoil_scenario==3:
                t = float("+inf")
            if _spoil_scenario==4:
                t = float("-inf")
            a = -1
            if _spoil_scenario==5:
                a = float("nan")
            if _spoil_scenario==6:
                a = float("+inf")
            if _spoil_scenario==7:
                a = float("-inf")
            b = +1
            if _spoil_scenario==8:
                b = float("nan")
            if _spoil_scenario==9:
                b = float("+inf")
            if _spoil_scenario==10:
                b = float("-inf")
            p = xalglib.polynomialbuildcheb1(a, b, y)
            v = xalglib.barycentriccalc(p, t)
            _TestResult = _TestResult and doc_print_test(v, 2.0, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("polint_t_9                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST polint_t_10
    #      Polynomial interpolation: y=x^2-x, Chebyshev grid (second kind), barycentric form
    #
    _TestResult = True
    for _spoil_scenario in range(-1,11):
        try:
            y = [0,0,2]
            if _spoil_scenario==0:
                spoil_vec_by_nan(y)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(y)
            t = -2
            if _spoil_scenario==3:
                t = float("+inf")
            if _spoil_scenario==4:
                t = float("-inf")
            a = -1
            if _spoil_scenario==5:
                a = float("nan")
            if _spoil_scenario==6:
                a = float("+inf")
            if _spoil_scenario==7:
                a = float("-inf")
            b = +1
            if _spoil_scenario==8:
                b = float("nan")
            if _spoil_scenario==9:
                b = float("+inf")
            if _spoil_scenario==10:
                b = float("-inf")
            p = xalglib.polynomialbuildcheb2(a, b, y)
            v = xalglib.barycentriccalc(p, t)
            _TestResult = _TestResult and doc_print_test(v, 6.0, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("polint_t_10                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST polint_t_11
    #      Polynomial interpolation: y=x^2-x, equidistant grid
    #
    _TestResult = True
    for _spoil_scenario in range(-1,5):
        try:
            y = [0,0,2]
            if _spoil_scenario==0:
                spoil_vec_by_nan(y)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(y)
            t = -1
            if _spoil_scenario==3:
                t = float("+inf")
            if _spoil_scenario==4:
                t = float("-inf")
            v = xalglib.polynomialcalceqdist(0.0, 2.0, y, t)
            _TestResult = _TestResult and doc_print_test(v, 2.0, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("polint_t_11                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST polint_t_12
    #      Polynomial interpolation: y=x^2-x, Chebyshev grid (first kind)
    #
    _TestResult = True
    for _spoil_scenario in range(-1,11):
        try:
            y = [-0.116025,0.000000,1.616025]
            if _spoil_scenario==0:
                spoil_vec_by_nan(y)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(y)
            t = -1
            if _spoil_scenario==3:
                t = float("+inf")
            if _spoil_scenario==4:
                t = float("-inf")
            a = -1
            if _spoil_scenario==5:
                a = float("nan")
            if _spoil_scenario==6:
                a = float("+inf")
            if _spoil_scenario==7:
                a = float("-inf")
            b = +1
            if _spoil_scenario==8:
                b = float("nan")
            if _spoil_scenario==9:
                b = float("+inf")
            if _spoil_scenario==10:
                b = float("-inf")
            v = xalglib.polynomialcalccheb1(a, b, y, t)
            _TestResult = _TestResult and doc_print_test(v, 2.0, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("polint_t_12                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST polint_t_13
    #      Polynomial interpolation: y=x^2-x, Chebyshev grid (second kind)
    #
    _TestResult = True
    for _spoil_scenario in range(-1,11):
        try:
            y = [0,0,2]
            if _spoil_scenario==0:
                spoil_vec_by_nan(y)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(y)
            t = -2
            if _spoil_scenario==3:
                t = float("+inf")
            if _spoil_scenario==4:
                t = float("-inf")
            a = -1
            if _spoil_scenario==5:
                a = float("nan")
            if _spoil_scenario==6:
                a = float("+inf")
            if _spoil_scenario==7:
                a = float("-inf")
            b = +1
            if _spoil_scenario==8:
                b = float("nan")
            if _spoil_scenario==9:
                b = float("+inf")
            if _spoil_scenario==10:
                b = float("-inf")
            v = xalglib.polynomialcalccheb2(a, b, y, t)
            _TestResult = _TestResult and doc_print_test(v, 6.0, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("polint_t_13                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST spline1d_d_linear
    #      Piecewise linear spline interpolation
    #
    _TestResult = True
    for _spoil_scenario in range(-1,12):
        try:
            #
            # We use piecewise linear spline to interpolate f(x)=x^2 sampled 
            # at 5 equidistant nodes on [-1,+1].
            #
            x = [-1.0,-0.5,0.0,+0.5,+1.0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_adding_element(x)
            if _spoil_scenario==4:
                spoil_vec_by_deleting_element(x)
            y = [+1.0,0.25,0.0,0.25,+1.0]
            if _spoil_scenario==5:
                spoil_vec_by_nan(y)
            if _spoil_scenario==6:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==7:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==8:
                spoil_vec_by_adding_element(y)
            if _spoil_scenario==9:
                spoil_vec_by_deleting_element(y)
            t = 0.25
            if _spoil_scenario==10:
                t = float("+inf")
            if _spoil_scenario==11:
                t = float("-inf")

            # build spline
            s = xalglib.spline1dbuildlinear(x, y)

            # calculate S(0.25) - it is quite different from 0.25^2=0.0625
            v = xalglib.spline1dcalc(s, t)
            _TestResult = _TestResult and doc_print_test(v, 0.125, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("spline1d_d_linear                FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST spline1d_d_cubic
    #      Cubic spline interpolation
    #
    sys.stdout.write("100/162\n")
    _TestResult = True
    for _spoil_scenario in range(-1,10):
        try:
            #
            # We use cubic spline to interpolate f(x)=x^2 sampled 
            # at 5 equidistant nodes on [-1,+1].
            #
            # First, we use default boundary conditions ("parabolically terminated
            # spline") because cubic spline built with such boundary conditions 
            # will exactly reproduce any quadratic f(x).
            #
            # Then we try to use natural boundary conditions
            #     d2S(-1)/dx^2 = 0.0
            #     d2S(+1)/dx^2 = 0.0
            # and see that such spline interpolated f(x) with small error.
            #
            x = [-1.0,-0.5,0.0,+0.5,+1.0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(x)
            y = [+1.0,0.25,0.0,0.25,+1.0]
            if _spoil_scenario==4:
                spoil_vec_by_nan(y)
            if _spoil_scenario==5:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==6:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==7:
                spoil_vec_by_deleting_element(y)
            t = 0.25
            if _spoil_scenario==8:
                t = float("+inf")
            if _spoil_scenario==9:
                t = float("-inf")
            natural_bound_type = 2
            #
            # Test exact boundary conditions: build S(x), calculare S(0.25)
            # (almost same as original function)
            #
            s = xalglib.spline1dbuildcubic(x, y)
            v = xalglib.spline1dcalc(s, t)
            _TestResult = _TestResult and doc_print_test(v, 0.0625, "real", 0.00001)

            #
            # Test natural boundary conditions: build S(x), calculare S(0.25)
            # (small interpolation error)
            #
            s = xalglib.spline1dbuildcubic(x, y, 5, natural_bound_type, 0.0, natural_bound_type, 0.0)
            v = xalglib.spline1dcalc(s, t)
            _TestResult = _TestResult and doc_print_test(v, 0.0580, "real", 0.0001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("spline1d_d_cubic                 FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST spline1d_d_monotone
    #      Monotone interpolation
    #
    _TestResult = True
    for _spoil_scenario in range(-1,10):
        try:
            #
            # Spline built witn spline1dbuildcubic() can be non-monotone even when
            # Y-values form monotone sequence. Say, for x=[0,1,2] and y=[0,1,1]
            # cubic spline will monotonically grow until x=1.5 and then start
            # decreasing.
            #
            # That's why ALGLIB provides special spline construction function
            # which builds spline which preserves monotonicity of the original
            # dataset.
            #
            # NOTE: in case original dataset is non-monotonic, ALGLIB splits it
            # into monotone subsequences and builds piecewise monotonic spline.
            #
            x = [0,1,2]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_adding_element(x)
            if _spoil_scenario==4:
                spoil_vec_by_deleting_element(x)
            y = [0,1,1]
            if _spoil_scenario==5:
                spoil_vec_by_nan(y)
            if _spoil_scenario==6:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==7:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==8:
                spoil_vec_by_adding_element(y)
            if _spoil_scenario==9:
                spoil_vec_by_deleting_element(y)

            # build spline
            s = xalglib.spline1dbuildmonotone(x, y)

            # calculate S at x = [-0.5, 0.0, 0.5, 1.0, 1.5, 2.0]
            # you may see that spline is really monotonic
            v = xalglib.spline1dcalc(s, -0.5)
            _TestResult = _TestResult and doc_print_test(v, 0.0000, "real", 0.00005)
            v = xalglib.spline1dcalc(s, 0.0)
            _TestResult = _TestResult and doc_print_test(v, 0.0000, "real", 0.00005)
            v = xalglib.spline1dcalc(s, +0.5)
            _TestResult = _TestResult and doc_print_test(v, 0.5000, "real", 0.00005)
            v = xalglib.spline1dcalc(s, 1.0)
            _TestResult = _TestResult and doc_print_test(v, 1.0000, "real", 0.00005)
            v = xalglib.spline1dcalc(s, 1.5)
            _TestResult = _TestResult and doc_print_test(v, 1.0000, "real", 0.00005)
            v = xalglib.spline1dcalc(s, 2.0)
            _TestResult = _TestResult and doc_print_test(v, 1.0000, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("spline1d_d_monotone              FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST spline1d_d_griddiff
    #      Differentiation on the grid using cubic splines
    #
    _TestResult = True
    for _spoil_scenario in range(-1,10):
        try:
            #
            # We use cubic spline to do grid differentiation, i.e. having
            # values of f(x)=x^2 sampled at 5 equidistant nodes on [-1,+1]
            # we calculate derivatives of cubic spline at nodes WITHOUT
            # CONSTRUCTION OF SPLINE OBJECT.
            #
            # There are efficient functions spline1dgriddiffcubic() and
            # spline1dgriddiff2cubic() for such calculations.
            #
            # We use default boundary conditions ("parabolically terminated
            # spline") because cubic spline built with such boundary conditions 
            # will exactly reproduce any quadratic f(x).
            #
            # Actually, we could use natural conditions, but we feel that 
            # spline which exactly reproduces f() will show us more 
            # understandable results.
            #
            x = [-1.0,-0.5,0.0,+0.5,+1.0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_adding_element(x)
            if _spoil_scenario==4:
                spoil_vec_by_deleting_element(x)
            y = [+1.0,0.25,0.0,0.25,+1.0]
            if _spoil_scenario==5:
                spoil_vec_by_nan(y)
            if _spoil_scenario==6:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==7:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==8:
                spoil_vec_by_adding_element(y)
            if _spoil_scenario==9:
                spoil_vec_by_deleting_element(y)

            #
            # We calculate first derivatives: they must be equal to 2*x
            #
            d1 = xalglib.spline1dgriddiffcubic(x, y)
            _TestResult = _TestResult and doc_print_test(d1, [-2.0, -1.0, 0.0, +1.0, +2.0], "real_vector", 0.0001)

            #
            # Now test griddiff2, which returns first AND second derivatives.
            # First derivative is 2*x, second is equal to 2.0
            #
            d1, d2 = xalglib.spline1dgriddiff2cubic(x, y)
            _TestResult = _TestResult and doc_print_test(d1, [-2.0, -1.0, 0.0, +1.0, +2.0], "real_vector", 0.0001)
            _TestResult = _TestResult and doc_print_test(d2, [ 2.0,  2.0, 2.0,  2.0,  2.0], "real_vector", 0.0001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("spline1d_d_griddiff              FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST spline1d_d_convdiff
    #      Resampling using cubic splines
    #
    _TestResult = True
    for _spoil_scenario in range(-1,11):
        try:
            #
            # We use cubic spline to do resampling, i.e. having
            # values of f(x)=x^2 sampled at 5 equidistant nodes on [-1,+1]
            # we calculate values/derivatives of cubic spline on 
            # another grid (equidistant with 9 nodes on [-1,+1])
            # WITHOUT CONSTRUCTION OF SPLINE OBJECT.
            #
            # There are efficient functions spline1dconvcubic(),
            # spline1dconvdiffcubic() and spline1dconvdiff2cubic() 
            # for such calculations.
            #
            # We use default boundary conditions ("parabolically terminated
            # spline") because cubic spline built with such boundary conditions 
            # will exactly reproduce any quadratic f(x).
            #
            # Actually, we could use natural conditions, but we feel that 
            # spline which exactly reproduces f() will show us more 
            # understandable results.
            #
            x_old = [-1.0,-0.5,0.0,+0.5,+1.0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x_old)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x_old)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x_old)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(x_old)
            y_old = [+1.0,0.25,0.0,0.25,+1.0]
            if _spoil_scenario==4:
                spoil_vec_by_nan(y_old)
            if _spoil_scenario==5:
                spoil_vec_by_posinf(y_old)
            if _spoil_scenario==6:
                spoil_vec_by_neginf(y_old)
            if _spoil_scenario==7:
                spoil_vec_by_deleting_element(y_old)
            x_new = [-1.00,-0.75,-0.50,-0.25,0.00,+0.25,+0.50,+0.75,+1.00]
            if _spoil_scenario==8:
                spoil_vec_by_nan(x_new)
            if _spoil_scenario==9:
                spoil_vec_by_posinf(x_new)
            if _spoil_scenario==10:
                spoil_vec_by_neginf(x_new)

            #
            # First, conversion without differentiation.
            #
            #
            y_new = xalglib.spline1dconvcubic(x_old, y_old, x_new)
            _TestResult = _TestResult and doc_print_test(y_new, [1.0000, 0.5625, 0.2500, 0.0625, 0.0000, 0.0625, 0.2500, 0.5625, 1.0000], "real_vector", 0.0001)

            #
            # Then, conversion with differentiation (first derivatives only)
            #
            #
            y_new, d1_new = xalglib.spline1dconvdiffcubic(x_old, y_old, x_new)
            _TestResult = _TestResult and doc_print_test(y_new, [1.0000, 0.5625, 0.2500, 0.0625, 0.0000, 0.0625, 0.2500, 0.5625, 1.0000], "real_vector", 0.0001)
            _TestResult = _TestResult and doc_print_test(d1_new, [-2.0, -1.5, -1.0, -0.5, 0.0, 0.5, 1.0, 1.5, 2.0], "real_vector", 0.0001)

            #
            # Finally, conversion with first and second derivatives
            #
            #
            y_new, d1_new, d2_new = xalglib.spline1dconvdiff2cubic(x_old, y_old, x_new)
            _TestResult = _TestResult and doc_print_test(y_new, [1.0000, 0.5625, 0.2500, 0.0625, 0.0000, 0.0625, 0.2500, 0.5625, 1.0000], "real_vector", 0.0001)
            _TestResult = _TestResult and doc_print_test(d1_new, [-2.0, -1.5, -1.0, -0.5, 0.0, 0.5, 1.0, 1.5, 2.0], "real_vector", 0.0001)
            _TestResult = _TestResult and doc_print_test(d2_new, [2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0], "real_vector", 0.0001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("spline1d_d_convdiff              FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST lsfit_d_nlf
    #      Nonlinear fitting using function value only
    #
    _TestResult = True
    for _spoil_scenario in range(-1,24):
        try:
            #
            # In this example we demonstrate exponential fitting by
            #
            #     f(x) = exp(-c*x^2)
            #
            # using numerical differentiation.
            #
            # IMPORTANT: the LSFIT optimizer supports parallel model  evaluation  and
            #            parallel numerical differentiation ('callback parallelism').
            #            This feature, which is present in commercial ALGLIB editions
            #            greatly  accelerates  fits  with   large   datasets   and/or
            #            expensive target functions.
            #
            #            Callback parallelism is usually  beneficial  when  a  single
            #            pass over the entire  dataset  requires  more  than  several
            #            milliseconds. This particular example,  of  course,  is  not
            #            suited for callback parallelism.
            #
            #            See ALGLIB Reference Manual, 'Working with commercial version'
            #            section,  and  comments  on  lsfitfit()  function  for  more
            #            information.
            #
            x = [[-1],[-0.8],[-0.6],[-0.4],[-0.2],[0],[0.2],[0.4],[0.6],[0.8],[1.0]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(x)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(x)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(x)
            if _spoil_scenario==3:
                spoil_mat_by_deleting_row(x)
            if _spoil_scenario==4:
                spoil_mat_by_deleting_col(x)
            y = [0.223130, 0.382893, 0.582748, 0.786628, 0.941765, 1.000000, 0.941765, 0.786628, 0.582748, 0.382893, 0.223130]
            if _spoil_scenario==5:
                spoil_vec_by_nan(y)
            if _spoil_scenario==6:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==7:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==8:
                spoil_vec_by_adding_element(y)
            if _spoil_scenario==9:
                spoil_vec_by_deleting_element(y)
            c = [0.3]
            if _spoil_scenario==10:
                spoil_vec_by_nan(c)
            if _spoil_scenario==11:
                spoil_vec_by_posinf(c)
            if _spoil_scenario==12:
                spoil_vec_by_neginf(c)
            epsx = 0.000001
            if _spoil_scenario==13:
                epsx = float("nan")
            if _spoil_scenario==14:
                epsx = float("+inf")
            if _spoil_scenario==15:
                epsx = float("-inf")
            maxits = 0
            diffstep = 0.0001
            if _spoil_scenario==16:
                diffstep = float("nan")
            if _spoil_scenario==17:
                diffstep = float("+inf")
            if _spoil_scenario==18:
                diffstep = float("-inf")

            #
            # Fitting without weights
            #
            state = xalglib.lsfitcreatef(x, y, c, diffstep)
            xalglib.lsfitsetcond(state, epsx, maxits)
            xalglib.lsfitfit_f(state, function_cx_1_func)
            c, rep = xalglib.lsfitresults(state)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 2, "int")
            _TestResult = _TestResult and doc_print_test(c, [1.5], "real_vector", 0.05)

            #
            # Fitting with weights
            # (you can change weights and see how it changes result)
            #
            w = [1,1,1,1,1,1,1,1,1,1,1]
            if _spoil_scenario==19:
                spoil_vec_by_nan(w)
            if _spoil_scenario==20:
                spoil_vec_by_posinf(w)
            if _spoil_scenario==21:
                spoil_vec_by_neginf(w)
            if _spoil_scenario==22:
                spoil_vec_by_adding_element(w)
            if _spoil_scenario==23:
                spoil_vec_by_deleting_element(w)
            state = xalglib.lsfitcreatewf(x, y, w, c, diffstep)
            xalglib.lsfitsetcond(state, epsx, maxits)
            xalglib.lsfitfit_f(state, function_cx_1_func)
            c, rep = xalglib.lsfitresults(state)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 2, "int")
            _TestResult = _TestResult and doc_print_test(c, [1.5], "real_vector", 0.05)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("lsfit_d_nlf                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST lsfit_d_nlfg
    #      Nonlinear fitting using gradient
    #
    _TestResult = True
    for _spoil_scenario in range(-1,21):
        try:
            #
            # In this example we demonstrate exponential fitting by
            #
            #     f(x) = exp(-c*x^2)
            #
            # using function value and gradient (with respect to c).
            #
            # IMPORTANT: the LSFIT optimizer supports parallel model  evaluation  and
            #            parallel numerical differentiation ('callback parallelism').
            #            This feature, which is present in commercial ALGLIB editions
            #            greatly  accelerates  fits  with   large   datasets   and/or
            #            expensive target functions.
            #
            #            Callback parallelism is usually  beneficial  when  a  single
            #            pass over the entire  dataset  requires  more  than  several
            #            milliseconds. This particular example,  of  course,  is  not
            #            suited for callback parallelism.
            #
            #            See ALGLIB Reference Manual, 'Working with commercial version'
            #            section,  and  comments  on  lsfitfit()  function  for  more
            #            information.
            #
            x = [[-1],[-0.8],[-0.6],[-0.4],[-0.2],[0],[0.2],[0.4],[0.6],[0.8],[1.0]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(x)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(x)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(x)
            if _spoil_scenario==3:
                spoil_mat_by_deleting_row(x)
            if _spoil_scenario==4:
                spoil_mat_by_deleting_col(x)
            y = [0.223130, 0.382893, 0.582748, 0.786628, 0.941765, 1.000000, 0.941765, 0.786628, 0.582748, 0.382893, 0.223130]
            if _spoil_scenario==5:
                spoil_vec_by_nan(y)
            if _spoil_scenario==6:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==7:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==8:
                spoil_vec_by_adding_element(y)
            if _spoil_scenario==9:
                spoil_vec_by_deleting_element(y)
            c = [0.3]
            if _spoil_scenario==10:
                spoil_vec_by_nan(c)
            if _spoil_scenario==11:
                spoil_vec_by_posinf(c)
            if _spoil_scenario==12:
                spoil_vec_by_neginf(c)
            epsx = 0.000001
            if _spoil_scenario==13:
                epsx = float("nan")
            if _spoil_scenario==14:
                epsx = float("+inf")
            if _spoil_scenario==15:
                epsx = float("-inf")
            maxits = 0

            #
            # Fitting without weights
            #
            state = xalglib.lsfitcreatefg(x, y, c)
            xalglib.lsfitsetcond(state, epsx, maxits)
            xalglib.lsfitfit_fg(state, function_cx_1_func, function_cx_1_grad)
            c, rep = xalglib.lsfitresults(state)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 2, "int")
            _TestResult = _TestResult and doc_print_test(c, [1.5], "real_vector", 0.05)

            #
            # Fitting with weights
            # (you can change weights and see how it changes result)
            #
            w = [1,1,1,1,1,1,1,1,1,1,1]
            if _spoil_scenario==16:
                spoil_vec_by_nan(w)
            if _spoil_scenario==17:
                spoil_vec_by_posinf(w)
            if _spoil_scenario==18:
                spoil_vec_by_neginf(w)
            if _spoil_scenario==19:
                spoil_vec_by_adding_element(w)
            if _spoil_scenario==20:
                spoil_vec_by_deleting_element(w)
            state = xalglib.lsfitcreatewfg(x, y, w, c)
            xalglib.lsfitsetcond(state, epsx, maxits)
            xalglib.lsfitfit_fg(state, function_cx_1_func, function_cx_1_grad)
            c, rep = xalglib.lsfitresults(state)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 2, "int")
            _TestResult = _TestResult and doc_print_test(c, [1.5], "real_vector", 0.05)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("lsfit_d_nlfg                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST lsfit_d_nlfb
    #      Bound contstrained nonlinear fitting using function value only
    #
    _TestResult = True
    for _spoil_scenario in range(-1,23):
        try:
            #
            # In this example we demonstrate exponential fitting by
            #
            #     f(x) = exp(-c*x^2)
            #
            # subject to box constraints
            #
            #     0.0 <= c <= 1.0
            #
            # using function value only. An unconstrained solution is c=1.5, but because of
            # constraints we should get c=1.0 (at the boundary).
            #
            # IMPORTANT: the LSFIT optimizer supports parallel model  evaluation  and
            #            parallel numerical differentiation ('callback parallelism').
            #            This feature, which is present in commercial ALGLIB editions
            #            greatly  accelerates  fits  with   large   datasets   and/or
            #            expensive target functions.
            #
            #            Callback parallelism is usually  beneficial  when  a  single
            #            pass over the entire  dataset  requires  more  than  several
            #            milliseconds. This particular example,  of  course,  is  not
            #            suited for callback parallelism.
            #
            #            See ALGLIB Reference Manual, 'Working with commercial version'
            #            section,  and  comments  on  lsfitfit()  function  for  more
            #            information.
            #
            x = [[-1],[-0.8],[-0.6],[-0.4],[-0.2],[0],[0.2],[0.4],[0.6],[0.8],[1.0]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(x)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(x)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(x)
            if _spoil_scenario==3:
                spoil_mat_by_deleting_row(x)
            if _spoil_scenario==4:
                spoil_mat_by_deleting_col(x)
            y = [0.223130, 0.382893, 0.582748, 0.786628, 0.941765, 1.000000, 0.941765, 0.786628, 0.582748, 0.382893, 0.223130]
            if _spoil_scenario==5:
                spoil_vec_by_nan(y)
            if _spoil_scenario==6:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==7:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==8:
                spoil_vec_by_adding_element(y)
            if _spoil_scenario==9:
                spoil_vec_by_deleting_element(y)
            c = [0.3]
            if _spoil_scenario==10:
                spoil_vec_by_nan(c)
            if _spoil_scenario==11:
                spoil_vec_by_posinf(c)
            if _spoil_scenario==12:
                spoil_vec_by_neginf(c)
            bndl = [0.0]
            if _spoil_scenario==13:
                spoil_vec_by_nan(bndl)
            if _spoil_scenario==14:
                spoil_vec_by_deleting_element(bndl)
            bndu = [1.0]
            if _spoil_scenario==15:
                spoil_vec_by_nan(bndu)
            if _spoil_scenario==16:
                spoil_vec_by_deleting_element(bndu)
            epsx = 0.000001
            if _spoil_scenario==17:
                epsx = float("nan")
            if _spoil_scenario==18:
                epsx = float("+inf")
            if _spoil_scenario==19:
                epsx = float("-inf")
            maxits = 0
            diffstep = 0.0001
            if _spoil_scenario==20:
                diffstep = float("nan")
            if _spoil_scenario==21:
                diffstep = float("+inf")
            if _spoil_scenario==22:
                diffstep = float("-inf")

            state = xalglib.lsfitcreatef(x, y, c, diffstep)
            xalglib.lsfitsetbc(state, bndl, bndu)
            xalglib.lsfitsetcond(state, epsx, maxits)
            xalglib.lsfitfit_f(state, function_cx_1_func)
            c, rep = xalglib.lsfitresults(state)
            _TestResult = _TestResult and doc_print_test(c, [1.0], "real_vector", 0.05)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("lsfit_d_nlfb                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST lsfit_d_nlscale
    #      Nonlinear fitting with custom scaling and bound constraints
    #
    _TestResult = True
    for _spoil_scenario in range(-1,27):
        try:
            #
            # In this example we demonstrate fitting by
            #
            #     f(x) = c[0]*(1+c[1]*((x-1999)^c[2]-1))
            #
            # subject to box constraints
            #
            #     -INF  < c[0] < +INF
            #      -10 <= c[1] <= +10
            #      0.1 <= c[2] <= 2.0
            #
            # The data we want to fit are time series of Japan national debt
            # collected from 2000 to 2008 measured in USD (dollars, not
            # millions of dollars).
            #
            # Our variables are:
            #     c[0] - debt value at initial moment (2000),
            #     c[1] - direction coefficient (growth or decrease),
            #     c[2] - curvature coefficient.
            # You may see that our variables are badly scaled - first one 
            # is order of 10^12, and next two are somewhere about 1 in 
            # magnitude. Such problem is difficult to solve without some
            # kind of scaling.
            # That is exactly where lsfitsetscale() function can be used.
            # We set scale of our variables to [1.0E12, 1, 1], which allows
            # us to easily solve this problem.
            #
            # You can try commenting out lsfitsetscale() call - and you will 
            # see that algorithm will fail to converge.
            #
            # IMPORTANT: the LSFIT optimizer supports parallel model  evaluation  and
            #            parallel numerical differentiation ('callback parallelism').
            #            This feature, which is present in commercial ALGLIB editions
            #            greatly  accelerates  fits  with   large   datasets   and/or
            #            expensive target functions.
            #
            #            Callback parallelism is usually  beneficial  when  a  single
            #            pass over the entire  dataset  requires  more  than  several
            #            milliseconds. This particular example,  of  course,  is  not
            #            suited for callback parallelism.
            #
            #            See ALGLIB Reference Manual, 'Working with commercial version'
            #            section,  and  comments  on  lsfitfit()  function  for  more
            #            information.
            #
            x = [[2000],[2001],[2002],[2003],[2004],[2005],[2006],[2007],[2008]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(x)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(x)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(x)
            if _spoil_scenario==3:
                spoil_mat_by_deleting_row(x)
            if _spoil_scenario==4:
                spoil_mat_by_deleting_col(x)
            y = [4323239600000.0, 4560913100000.0, 5564091500000.0, 6743189300000.0, 7284064600000.0, 7050129600000.0, 7092221500000.0, 8483907600000.0, 8625804400000.0]
            if _spoil_scenario==5:
                spoil_vec_by_nan(y)
            if _spoil_scenario==6:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==7:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==8:
                spoil_vec_by_adding_element(y)
            if _spoil_scenario==9:
                spoil_vec_by_deleting_element(y)
            c = [1.0e+13, 1, 1]
            if _spoil_scenario==10:
                spoil_vec_by_nan(c)
            if _spoil_scenario==11:
                spoil_vec_by_posinf(c)
            if _spoil_scenario==12:
                spoil_vec_by_neginf(c)
            epsx = 1.0e-5
            if _spoil_scenario==13:
                epsx = float("nan")
            if _spoil_scenario==14:
                epsx = float("+inf")
            if _spoil_scenario==15:
                epsx = float("-inf")
            bndl = [-float("inf"), -10, 0.1]
            if _spoil_scenario==16:
                spoil_vec_by_nan(bndl)
            if _spoil_scenario==17:
                spoil_vec_by_deleting_element(bndl)
            bndu = [float("inf"), +10, 2.0]
            if _spoil_scenario==18:
                spoil_vec_by_nan(bndu)
            if _spoil_scenario==19:
                spoil_vec_by_deleting_element(bndu)
            s = [1.0e+12, 1, 1]
            if _spoil_scenario==20:
                spoil_vec_by_nan(s)
            if _spoil_scenario==21:
                spoil_vec_by_posinf(s)
            if _spoil_scenario==22:
                spoil_vec_by_neginf(s)
            if _spoil_scenario==23:
                spoil_vec_by_deleting_element(s)
            maxits = 0
            diffstep = 1.0e-5
            if _spoil_scenario==24:
                diffstep = float("nan")
            if _spoil_scenario==25:
                diffstep = float("+inf")
            if _spoil_scenario==26:
                diffstep = float("-inf")

            state = xalglib.lsfitcreatef(x, y, c, diffstep)
            xalglib.lsfitsetcond(state, epsx, maxits)
            xalglib.lsfitsetbc(state, bndl, bndu)
            xalglib.lsfitsetscale(state, s)
            xalglib.lsfitfit_f(state, function_debt_func)
            c, rep = xalglib.lsfitresults(state)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 2, "int")
            _TestResult = _TestResult and doc_print_test(c, [4.142560e+12, 0.434240, 0.565376], "real_vector", -0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("lsfit_d_nlscale                  FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST lsfit_d_lin
    #      Unconstrained (general) linear least squares fitting with and without weights
    #
    _TestResult = True
    for _spoil_scenario in range(-1,13):
        try:
            #
            # In this example we demonstrate linear fitting by f(x|a) = a*exp(0.5*x).
            #
            # We have:
            # * y - vector of experimental data
            # * fmatrix -  matrix of basis functions calculated at sample points
            #              Actually, we have only one basis function F0 = exp(0.5*x).
            #
            fmatrix = [[0.606531],[0.670320],[0.740818],[0.818731],[0.904837],[1.000000],[1.105171],[1.221403],[1.349859],[1.491825],[1.648721]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(fmatrix)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(fmatrix)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(fmatrix)
            y = [1.133719, 1.306522, 1.504604, 1.554663, 1.884638, 2.072436, 2.257285, 2.534068, 2.622017, 2.897713, 3.219371]
            if _spoil_scenario==3:
                spoil_vec_by_nan(y)
            if _spoil_scenario==4:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==5:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==6:
                spoil_vec_by_adding_element(y)
            if _spoil_scenario==7:
                spoil_vec_by_deleting_element(y)

            #
            # Linear fitting without weights
            #
            c, rep = xalglib.lsfitlinear(y, fmatrix)
            _TestResult = _TestResult and doc_print_test(c, [1.98650], "real_vector", 0.00005)

            #
            # Linear fitting with individual weights.
            # Slightly different result is returned.
            #
            w = [1.414213, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
            if _spoil_scenario==8:
                spoil_vec_by_nan(w)
            if _spoil_scenario==9:
                spoil_vec_by_posinf(w)
            if _spoil_scenario==10:
                spoil_vec_by_neginf(w)
            if _spoil_scenario==11:
                spoil_vec_by_adding_element(w)
            if _spoil_scenario==12:
                spoil_vec_by_deleting_element(w)
            c, rep = xalglib.lsfitlinearw(y, w, fmatrix)
            _TestResult = _TestResult and doc_print_test(c, [1.983354], "real_vector", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("lsfit_d_lin                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST lsfit_d_linc
    #      Constrained (general) linear least squares fitting with and without weights
    #
    _TestResult = True
    for _spoil_scenario in range(-1,20):
        try:
            #
            # In this example we demonstrate linear fitting by f(x|a,b) = a*x+b
            # with simple constraint f(0)=0.
            #
            # We have:
            # * y - vector of experimental data
            # * fmatrix -  matrix of basis functions sampled at [0,1] with step 0.2:
            #                  [ 1.0   0.0 ]
            #                  [ 1.0   0.2 ]
            #                  [ 1.0   0.4 ]
            #                  [ 1.0   0.6 ]
            #                  [ 1.0   0.8 ]
            #                  [ 1.0   1.0 ]
            #              first column contains value of first basis function (constant term)
            #              second column contains second basis function (linear term)
            # * cmatrix -  matrix of linear constraints:
            #                  [ 1.0  0.0  0.0 ]
            #              first two columns contain coefficients before basis functions,
            #              last column contains desired value of their sum.
            #              So [1,0,0] means "1*constant_term + 0*linear_term = 0" 
            #
            y = [0.072436,0.246944,0.491263,0.522300,0.714064,0.921929]
            if _spoil_scenario==0:
                spoil_vec_by_nan(y)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==3:
                spoil_vec_by_adding_element(y)
            if _spoil_scenario==4:
                spoil_vec_by_deleting_element(y)
            fmatrix = [[1,0.0],[1,0.2],[1,0.4],[1,0.6],[1,0.8],[1,1.0]]
            if _spoil_scenario==5:
                spoil_mat_by_nan(fmatrix)
            if _spoil_scenario==6:
                spoil_mat_by_posinf(fmatrix)
            if _spoil_scenario==7:
                spoil_mat_by_neginf(fmatrix)
            if _spoil_scenario==8:
                spoil_mat_by_adding_row(fmatrix)
            if _spoil_scenario==9:
                spoil_mat_by_adding_col(fmatrix)
            if _spoil_scenario==10:
                spoil_mat_by_deleting_row(fmatrix)
            if _spoil_scenario==11:
                spoil_mat_by_deleting_col(fmatrix)
            cmatrix = [[1,0,0]]
            if _spoil_scenario==12:
                spoil_mat_by_nan(cmatrix)
            if _spoil_scenario==13:
                spoil_mat_by_posinf(cmatrix)
            if _spoil_scenario==14:
                spoil_mat_by_neginf(cmatrix)

            #
            # Constrained fitting without weights
            #
            c, rep = xalglib.lsfitlinearc(y, fmatrix, cmatrix)
            _TestResult = _TestResult and doc_print_test(c, [0,0.932933], "real_vector", 0.0005)

            #
            # Constrained fitting with individual weights
            #
            w = [1, 1.414213, 1, 1, 1, 1]
            if _spoil_scenario==15:
                spoil_vec_by_nan(w)
            if _spoil_scenario==16:
                spoil_vec_by_posinf(w)
            if _spoil_scenario==17:
                spoil_vec_by_neginf(w)
            if _spoil_scenario==18:
                spoil_vec_by_adding_element(w)
            if _spoil_scenario==19:
                spoil_vec_by_deleting_element(w)
            c, rep = xalglib.lsfitlinearwc(y, w, fmatrix, cmatrix)
            _TestResult = _TestResult and doc_print_test(c, [0,0.938322], "real_vector", 0.0005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("lsfit_d_linc                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST lsfit_d_pol
    #      Unconstrained polynomial fitting
    #
    _TestResult = True
    for _spoil_scenario in range(-1,20):
        try:
            #
            # This example demonstrates polynomial fitting.
            #
            # Fitting is done by two (M=2) functions from polynomial basis:
            #     f0 = 1
            #     f1 = x
            # Basically, it just a linear fit; more complex polynomials may be used
            # (e.g. parabolas with M=3, cubic with M=4), but even such simple fit allows
            # us to demonstrate polynomialfit() function in action.
            #
            # We have:
            # * x      set of abscissas
            # * y      experimental data
            #
            # Additionally we demonstrate weighted fitting, where second point has
            # more weight than other ones.
            #
            x = [0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_adding_element(x)
            if _spoil_scenario==4:
                spoil_vec_by_deleting_element(x)
            y = [0.00,0.05,0.26,0.32,0.33,0.43,0.60,0.60,0.77,0.98,1.02]
            if _spoil_scenario==5:
                spoil_vec_by_nan(y)
            if _spoil_scenario==6:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==7:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==8:
                spoil_vec_by_adding_element(y)
            if _spoil_scenario==9:
                spoil_vec_by_deleting_element(y)
            m = 2
            t = 2
            if _spoil_scenario==10:
                t = float("+inf")
            if _spoil_scenario==11:
                t = float("-inf")

            #
            # Fitting without individual weights
            #
            # NOTE: result is returned as barycentricinterpolant structure.
            #       if you want to get representation in the power basis,
            #       you can use barycentricbar2pow() function to convert
            #       from barycentric to power representation (see docs for 
            #       POLINT subpackage for more info).
            #
            p, rep = xalglib.polynomialfit(x, y, m)
            v = xalglib.barycentriccalc(p, t)
            _TestResult = _TestResult and doc_print_test(v, 2.011, "real", 0.002)

            #
            # Fitting with individual weights
            #
            # NOTE: slightly different result is returned
            #
            w = [1,1.414213562,1,1,1,1,1,1,1,1,1]
            if _spoil_scenario==12:
                spoil_vec_by_nan(w)
            if _spoil_scenario==13:
                spoil_vec_by_posinf(w)
            if _spoil_scenario==14:
                spoil_vec_by_neginf(w)
            if _spoil_scenario==15:
                spoil_vec_by_adding_element(w)
            if _spoil_scenario==16:
                spoil_vec_by_deleting_element(w)
            xc = []
            if _spoil_scenario==17:
                spoil_vec_by_adding_element(xc)
            yc = []
            if _spoil_scenario==18:
                spoil_vec_by_adding_element(yc)
            dc = []
            if _spoil_scenario==19:
                spoil_vec_by_adding_element(dc)
            p, rep = xalglib.polynomialfitwc(x, y, w, xc, yc, dc, m)
            v = xalglib.barycentriccalc(p, t)
            _TestResult = _TestResult and doc_print_test(v, 2.023, "real", 0.002)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("lsfit_d_pol                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST lsfit_d_polc
    #      Constrained polynomial fitting
    #
    _TestResult = True
    for _spoil_scenario in range(-1,29):
        try:
            #
            # This example demonstrates polynomial fitting.
            #
            # Fitting is done by two (M=2) functions from polynomial basis:
            #     f0 = 1
            #     f1 = x
            # with simple constraint on function value
            #     f(0) = 0
            # Basically, it just a linear fit; more complex polynomials may be used
            # (e.g. parabolas with M=3, cubic with M=4), but even such simple fit allows
            # us to demonstrate polynomialfit() function in action.
            #
            # We have:
            # * x      set of abscissas
            # * y      experimental data
            # * xc     points where constraints are placed
            # * yc     constraints on derivatives
            # * dc     derivative indices
            #          (0 means function itself, 1 means first derivative)
            #
            x = [1.0,1.0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_adding_element(x)
            if _spoil_scenario==4:
                spoil_vec_by_deleting_element(x)
            y = [0.9,1.1]
            if _spoil_scenario==5:
                spoil_vec_by_nan(y)
            if _spoil_scenario==6:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==7:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==8:
                spoil_vec_by_adding_element(y)
            if _spoil_scenario==9:
                spoil_vec_by_deleting_element(y)
            w = [1,1]
            if _spoil_scenario==10:
                spoil_vec_by_nan(w)
            if _spoil_scenario==11:
                spoil_vec_by_posinf(w)
            if _spoil_scenario==12:
                spoil_vec_by_neginf(w)
            if _spoil_scenario==13:
                spoil_vec_by_adding_element(w)
            if _spoil_scenario==14:
                spoil_vec_by_deleting_element(w)
            xc = [0]
            if _spoil_scenario==15:
                spoil_vec_by_nan(xc)
            if _spoil_scenario==16:
                spoil_vec_by_posinf(xc)
            if _spoil_scenario==17:
                spoil_vec_by_neginf(xc)
            if _spoil_scenario==18:
                spoil_vec_by_adding_element(xc)
            if _spoil_scenario==19:
                spoil_vec_by_deleting_element(xc)
            yc = [0]
            if _spoil_scenario==20:
                spoil_vec_by_nan(yc)
            if _spoil_scenario==21:
                spoil_vec_by_posinf(yc)
            if _spoil_scenario==22:
                spoil_vec_by_neginf(yc)
            if _spoil_scenario==23:
                spoil_vec_by_adding_element(yc)
            if _spoil_scenario==24:
                spoil_vec_by_deleting_element(yc)
            dc = [0]
            if _spoil_scenario==25:
                spoil_vec_by_adding_element(dc)
            if _spoil_scenario==26:
                spoil_vec_by_deleting_element(dc)
            t = 2
            if _spoil_scenario==27:
                t = float("+inf")
            if _spoil_scenario==28:
                t = float("-inf")
            m = 2

            p, rep = xalglib.polynomialfitwc(x, y, w, xc, yc, dc, m)
            v = xalglib.barycentriccalc(p, t)
            _TestResult = _TestResult and doc_print_test(v, 2.000, "real", 0.001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("lsfit_d_polc                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST lsfit_d_spline
    #      Unconstrained fitting by penalized regression spline
    #
    _TestResult = True
    for _spoil_scenario in range(-1,10):
        try:
            #
            # In this example we demonstrate penalized spline fitting of noisy data
            #
            # We have:
            # * x - abscissas
            # * y - vector of experimental data, straight line with small noise
            #
            x = [0.00,0.10,0.20,0.30,0.40,0.50,0.60,0.70,0.80,0.90]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_adding_element(x)
            if _spoil_scenario==4:
                spoil_vec_by_deleting_element(x)
            y = [0.10,0.00,0.30,0.40,0.30,0.40,0.62,0.68,0.75,0.95]
            if _spoil_scenario==5:
                spoil_vec_by_nan(y)
            if _spoil_scenario==6:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==7:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==8:
                spoil_vec_by_adding_element(y)
            if _spoil_scenario==9:
                spoil_vec_by_deleting_element(y)

            #
            # Fit with VERY small amount of smoothing (eps = 1.0E-9)
            # and large number of basis functions (M=50).
            #
            # With such small regularization penalized spline almost fully reproduces function values
            #
            s, rep = xalglib.spline1dfit(x, y, 50, 0.000000001)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            v = xalglib.spline1dcalc(s, 0.0)
            _TestResult = _TestResult and doc_print_test(v, 0.10, "real", 0.01)

            #
            # Fit with VERY large amount of smoothing eps=1000000
            # and large number of basis functions (M=50).
            #
            # With such regularization our spline should become close to the straight line fit.
            # We will compare its value in x=1.0 with results obtained from such fit.
            #
            s, rep = xalglib.spline1dfit(x, y, 50, 1000000)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            v = xalglib.spline1dcalc(s, 1.0)
            _TestResult = _TestResult and doc_print_test(v, 0.969, "real", 0.001)

            #
            # In real life applications you may need some moderate degree of fitting,
            # so we try to fit once more with eps=0.1.
            #
            s, rep = xalglib.spline1dfit(x, y, 50, 0.1)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("lsfit_d_spline                   FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST lsfit_t_polfit_1
    #      Polynomial fitting, full list of parameters.
    #
    _TestResult = True
    for _spoil_scenario in range(-1,10):
        try:
            x = [0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(x)
            y = [0.00,0.05,0.26,0.32,0.33,0.43,0.60,0.60,0.77,0.98,1.02]
            if _spoil_scenario==4:
                spoil_vec_by_nan(y)
            if _spoil_scenario==5:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==6:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==7:
                spoil_vec_by_deleting_element(y)
            m = 2
            t = 2
            if _spoil_scenario==8:
                t = float("+inf")
            if _spoil_scenario==9:
                t = float("-inf")
            p, rep = xalglib.polynomialfit(x, y, 11, m)
            v = xalglib.barycentriccalc(p, t)
            _TestResult = _TestResult and doc_print_test(v, 2.011, "real", 0.002)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("lsfit_t_polfit_1                 FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST lsfit_t_polfit_2
    #      Polynomial fitting, full list of parameters.
    #
    _TestResult = True
    for _spoil_scenario in range(-1,14):
        try:
            x = [0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(x)
            y = [0.00,0.05,0.26,0.32,0.33,0.43,0.60,0.60,0.77,0.98,1.02]
            if _spoil_scenario==4:
                spoil_vec_by_nan(y)
            if _spoil_scenario==5:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==6:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==7:
                spoil_vec_by_deleting_element(y)
            w = [1,1.414213562,1,1,1,1,1,1,1,1,1]
            if _spoil_scenario==8:
                spoil_vec_by_nan(w)
            if _spoil_scenario==9:
                spoil_vec_by_posinf(w)
            if _spoil_scenario==10:
                spoil_vec_by_neginf(w)
            if _spoil_scenario==11:
                spoil_vec_by_deleting_element(w)
            xc = []
            yc = []
            dc = []
            m = 2
            t = 2
            if _spoil_scenario==12:
                t = float("+inf")
            if _spoil_scenario==13:
                t = float("-inf")
            p, rep = xalglib.polynomialfitwc(x, y, w, 11, xc, yc, dc, 0, m)
            v = xalglib.barycentriccalc(p, t)
            _TestResult = _TestResult and doc_print_test(v, 2.023, "real", 0.002)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("lsfit_t_polfit_2                 FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST lsfit_t_polfit_3
    #      Polynomial fitting, full list of parameters.
    #
    _TestResult = True
    for _spoil_scenario in range(-1,23):
        try:
            x = [1.0,1.0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(x)
            y = [0.9,1.1]
            if _spoil_scenario==4:
                spoil_vec_by_nan(y)
            if _spoil_scenario==5:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==6:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==7:
                spoil_vec_by_deleting_element(y)
            w = [1,1]
            if _spoil_scenario==8:
                spoil_vec_by_nan(w)
            if _spoil_scenario==9:
                spoil_vec_by_posinf(w)
            if _spoil_scenario==10:
                spoil_vec_by_neginf(w)
            if _spoil_scenario==11:
                spoil_vec_by_deleting_element(w)
            xc = [0]
            if _spoil_scenario==12:
                spoil_vec_by_nan(xc)
            if _spoil_scenario==13:
                spoil_vec_by_posinf(xc)
            if _spoil_scenario==14:
                spoil_vec_by_neginf(xc)
            if _spoil_scenario==15:
                spoil_vec_by_deleting_element(xc)
            yc = [0]
            if _spoil_scenario==16:
                spoil_vec_by_nan(yc)
            if _spoil_scenario==17:
                spoil_vec_by_posinf(yc)
            if _spoil_scenario==18:
                spoil_vec_by_neginf(yc)
            if _spoil_scenario==19:
                spoil_vec_by_deleting_element(yc)
            dc = [0]
            if _spoil_scenario==20:
                spoil_vec_by_deleting_element(dc)
            m = 2
            t = 2
            if _spoil_scenario==21:
                t = float("+inf")
            if _spoil_scenario==22:
                t = float("-inf")
            p, rep = xalglib.polynomialfitwc(x, y, w, 2, xc, yc, dc, 1, m)
            v = xalglib.barycentriccalc(p, t)
            _TestResult = _TestResult and doc_print_test(v, 2.000, "real", 0.001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("lsfit_t_polfit_3                 FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST lsfit_t_4pl
    #      4-parameter logistic fitting
    #
    _TestResult = True
    for _spoil_scenario in range(-1,8):
        try:
            x = [1,2,3,4,5,6,7,8]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(x)
            y = [0.06313223,0.44552624,0.61838364,0.71385108,0.77345838,0.81383140,0.84280033,0.86449822]
            if _spoil_scenario==4:
                spoil_vec_by_nan(y)
            if _spoil_scenario==5:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==6:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==7:
                spoil_vec_by_deleting_element(y)
            n = 8

            #
            # Test logisticfit4() on carefully designed data with a priori known answer.
            #
            a, b, c, d, rep = xalglib.logisticfit4(x, y, n)
            _TestResult = _TestResult and doc_print_test(a, -1.000, "real", 0.01)
            _TestResult = _TestResult and doc_print_test(b, 1.200, "real", 0.01)
            _TestResult = _TestResult and doc_print_test(c, 0.900, "real", 0.01)
            _TestResult = _TestResult and doc_print_test(d, 1.000, "real", 0.01)

            #
            # Evaluate model at point x=0.5
            #
            v = xalglib.logisticcalc4(0.5, a, b, c, d)
            _TestResult = _TestResult and doc_print_test(v, -0.33874308, "real", 0.001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("lsfit_t_4pl                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST lsfit_t_5pl
    #      5-parameter logistic fitting
    #
    _TestResult = True
    for _spoil_scenario in range(-1,8):
        try:
            x = [1,2,3,4,5,6,7,8]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(x)
            y = [0.1949776139,0.5710060208,0.726002637,0.8060434158,0.8534547965,0.8842071579,0.9054773317,0.9209088299]
            if _spoil_scenario==4:
                spoil_vec_by_nan(y)
            if _spoil_scenario==5:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==6:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==7:
                spoil_vec_by_deleting_element(y)
            n = 8

            #
            # Test logisticfit5() on carefully designed data with a priori known answer.
            #
            a, b, c, d, g, rep = xalglib.logisticfit5(x, y, n)
            _TestResult = _TestResult and doc_print_test(a, -1.000, "real", 0.01)
            _TestResult = _TestResult and doc_print_test(b, 1.200, "real", 0.01)
            _TestResult = _TestResult and doc_print_test(c, 0.900, "real", 0.01)
            _TestResult = _TestResult and doc_print_test(d, 1.000, "real", 0.01)
            _TestResult = _TestResult and doc_print_test(g, 1.200, "real", 0.01)

            #
            # Evaluate model at point x=0.5
            #
            v = xalglib.logisticcalc5(0.5, a, b, c, d, g)
            _TestResult = _TestResult and doc_print_test(v, -0.2354656824, "real", 0.001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("lsfit_t_5pl                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST parametric_rdp
    #      Parametric Ramer-Douglas-Peucker approximation
    #
    _TestResult = True
    for _spoil_scenario in range(-1,7):
        try:
            #
            # We use RDP algorithm to approximate parametric 2D curve given by
            # locations in t=0,1,2,3 (see below), which form piecewise linear
            # trajectory through D-dimensional space (2-dimensional in our example).
            # 
            #     |
            #     |
            #     -     *     *     X2................X3
            #     |                .
            #     |               .
            #     -     *     *  .  *     *     *     *
            #     |             .
            #     |            .
            #     -     *     X1    *     *     *     *
            #     |      .....
            #     |  ....
            #     X0----|-----|-----|-----|-----|-----|---
            #
            npoints = 4
            ndimensions = 2
            x = [[0,0],[2,1],[3,3],[6,3]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(x)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(x)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(x)
            if _spoil_scenario==3:
                spoil_mat_by_deleting_row(x)
            if _spoil_scenario==4:
                spoil_mat_by_deleting_col(x)

            #
            # Approximation of parametric curve is performed by another parametric curve
            # with lesser amount of points. It allows to work with "compressed"
            # representation, which needs smaller amount of memory. Say, in our example
            # (we allow points with error smaller than 0.8) approximation will have
            # just two sequential sections connecting X0 with X2, and X2 with X3.
            # 
            #     |
            #     |
            #     -     *     *     X2................X3
            #     |               . 
            #     |             .  
            #     -     *     .     *     *     *     *
            #     |         .    
            #     |       .     
            #     -     .     X1    *     *     *     *
            #     |   .       
            #     | .    
            #     X0----|-----|-----|-----|-----|-----|---
            #
            #
            limitcnt = 0
            limiteps = 0.8
            if _spoil_scenario==5:
                limiteps = float("+inf")
            if _spoil_scenario==6:
                limiteps = float("-inf")
            y, idxy, nsections = xalglib.parametricrdpfixed(x, npoints, ndimensions, limitcnt, limiteps)
            _TestResult = _TestResult and doc_print_test(nsections, 2, "int")
            _TestResult = _TestResult and doc_print_test(idxy, [0,2,3], "int_vector")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("parametric_rdp                   FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST spline2d_bilinear
    #      Bilinear spline interpolation
    #
    _TestResult = True
    for _spoil_scenario in range(-1,16):
        try:
            #
            # We use bilinear spline to interpolate f(x,y)=x^2+2*y^2 sampled 
            # at (x,y) from [0.0, 0.5, 1.0] X [0.0, 1.0].
            #
            x = [0.0, 0.5, 1.0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(x)
            y = [0.0, 1.0]
            if _spoil_scenario==4:
                spoil_vec_by_nan(y)
            if _spoil_scenario==5:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==6:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==7:
                spoil_vec_by_deleting_element(y)
            f = [0.00,0.25,1.00,2.00,2.25,3.00]
            if _spoil_scenario==8:
                spoil_vec_by_nan(f)
            if _spoil_scenario==9:
                spoil_vec_by_posinf(f)
            if _spoil_scenario==10:
                spoil_vec_by_neginf(f)
            if _spoil_scenario==11:
                spoil_vec_by_deleting_element(f)
            vx = 0.25
            if _spoil_scenario==12:
                vx = float("+inf")
            if _spoil_scenario==13:
                vx = float("-inf")
            vy = 0.50
            if _spoil_scenario==14:
                vy = float("+inf")
            if _spoil_scenario==15:
                vy = float("-inf")

            # build spline
            s = xalglib.spline2dbuildbilinearv(x, 3, y, 2, f, 1)

            # calculate S(0.25,0.50)
            v = xalglib.spline2dcalc(s, vx, vy)
            _TestResult = _TestResult and doc_print_test(v, 1.1250, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("spline2d_bilinear                FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST spline2d_bicubic
    #      Bilinear spline interpolation
    #
    _TestResult = True
    for _spoil_scenario in range(-1,16):
        try:
            #
            # We use bilinear spline to interpolate f(x,y)=x^2+2*y^2 sampled 
            # at (x,y) from [0.0, 0.5, 1.0] X [0.0, 1.0].
            #
            x = [0.0, 0.5, 1.0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(x)
            y = [0.0, 1.0]
            if _spoil_scenario==4:
                spoil_vec_by_nan(y)
            if _spoil_scenario==5:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==6:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==7:
                spoil_vec_by_deleting_element(y)
            f = [0.00,0.25,1.00,2.00,2.25,3.00]
            if _spoil_scenario==8:
                spoil_vec_by_nan(f)
            if _spoil_scenario==9:
                spoil_vec_by_posinf(f)
            if _spoil_scenario==10:
                spoil_vec_by_neginf(f)
            if _spoil_scenario==11:
                spoil_vec_by_deleting_element(f)
            vx = 0.25
            if _spoil_scenario==12:
                vx = float("+inf")
            if _spoil_scenario==13:
                vx = float("-inf")
            vy = 0.50
            if _spoil_scenario==14:
                vy = float("+inf")
            if _spoil_scenario==15:
                vy = float("-inf")

            # build spline
            s = xalglib.spline2dbuildbicubicv(x, 3, y, 2, f, 1)

            # calculate S(0.25,0.50)
            v = xalglib.spline2dcalc(s, vx, vy)
            _TestResult = _TestResult and doc_print_test(v, 1.0625, "real", 0.00005)

            # calculate derivatives
            v, dx, dy = xalglib.spline2ddiff(s, vx, vy)
            _TestResult = _TestResult and doc_print_test(v, 1.0625, "real", 0.00005)
            _TestResult = _TestResult and doc_print_test(dx, 0.5000, "real", 0.00005)
            _TestResult = _TestResult and doc_print_test(dy, 2.0000, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("spline2d_bicubic                 FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST spline2d_fit_blocklls
    #      Fitting bicubic spline to irregular data
    #
    _TestResult = True
    for _spoil_scenario in range(-1,5):
        try:
            #
            # We use bicubic spline to reproduce f(x,y)=1/(1+x^2+2*y^2) sampled
            # at irregular points (x,y) from [-1,+1]*[-1,+1]
            #
            # We have 5 such points, located approximately at corners of the area
            # and its center -  but not exactly at the grid. Thus, we have to FIT
            # the spline, i.e. to solve least squares problem
            #
            xy = [[-0.987,-0.902,0.359],[0.948,-0.992,0.347],[-1.000,1.000,0.333],[1.000,0.973,0.339],[0.017,0.180,0.968]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)
            if _spoil_scenario==3:
                spoil_mat_by_deleting_row(xy)
            if _spoil_scenario==4:
                spoil_mat_by_deleting_col(xy)

            #
            # First step is to create spline2dbuilder object and set its properties:
            # * d=1 means that we create vector-valued spline with 1 component
            # * we specify dataset xy
            # * we rely on automatic selection of interpolation area
            # * we tell builder that we want to use 5x5 grid for an underlying spline
            # * we choose least squares solver named BlockLLS and configure it by
            #   telling that we want to apply zero nonlinearity penalty.
            #
            # NOTE: you can specify non-zero lambdav if you want to make your spline
            #       more "rigid", i.e. to penalize nonlinearity.
            #
            # NOTE: ALGLIB has two solvers which fit bicubic splines to irregular data,
            #       one of them is BlockLLS and another one is FastDDM. Former is
            #       intended for moderately sized grids (up to 512x512 nodes, although
            #       it may take up to few minutes); it is the most easy to use and
            #       control spline fitting function in the library. Latter, FastDDM,
            #       is intended for efficient solution of large-scale problems
            #       (up to 100.000.000 nodes). Both solvers can be parallelized, but
            #       FastDDM is much more efficient. See comments for more information.
            #
            d = 1
            lambdav = 0.000
            builder = xalglib.spline2dbuildercreate(d)
            xalglib.spline2dbuildersetpoints(builder, xy, 5)
            xalglib.spline2dbuildersetgrid(builder, 5, 5)
            xalglib.spline2dbuildersetalgoblocklls(builder, lambdav)

            #
            # Now we are ready to fit and evaluate our results
            #
            s, rep = xalglib.spline2dfit(builder)

            # evaluate results - function value at the grid is reproduced exactly
            v = xalglib.spline2dcalc(s, -1, 1)
            _TestResult = _TestResult and doc_print_test(v, 0.333000, "real", 0.005)

            # check maximum error - it must be nearly zero
            _TestResult = _TestResult and doc_print_test(rep.maxerror, 0.000, "real", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("spline2d_fit_blocklls            FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST spline2d_unpack
    #      Unpacking bilinear spline
    #
    _TestResult = True
    for _spoil_scenario in range(-1,12):
        try:
            #
            # We build bilinear spline for f(x,y)=x+2*y+3*xy for (x,y) in [0,1].
            # Then we demonstrate how to unpack it.
            #
            x = [0.0, 1.0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(x)
            y = [0.0, 1.0]
            if _spoil_scenario==4:
                spoil_vec_by_nan(y)
            if _spoil_scenario==5:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==6:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==7:
                spoil_vec_by_deleting_element(y)
            f = [0.00,1.00,2.00,6.00]
            if _spoil_scenario==8:
                spoil_vec_by_nan(f)
            if _spoil_scenario==9:
                spoil_vec_by_posinf(f)
            if _spoil_scenario==10:
                spoil_vec_by_neginf(f)
            if _spoil_scenario==11:
                spoil_vec_by_deleting_element(f)

            # build spline
            s = xalglib.spline2dbuildbilinearv(x, 2, y, 2, f, 1)

            # unpack and test
            m, n, d, c = xalglib.spline2dunpackv(s)
            _TestResult = _TestResult and doc_print_test(c, [[0, 1, 0, 1, 0,2,0,0, 1,3,0,0, 0,0,0,0, 0,0,0,0, 1]], "real_matrix", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("spline2d_unpack                  FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST spline2d_copytrans
    #      Copy and transform
    #
    _TestResult = True
    for _spoil_scenario in range(-1,16):
        try:
            #
            # We build bilinear spline for f(x,y)=x+2*y for (x,y) in [0,1].
            # Then we apply several transformations to this spline.
            #
            x = [0.0, 1.0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(x)
            y = [0.0, 1.0]
            if _spoil_scenario==4:
                spoil_vec_by_nan(y)
            if _spoil_scenario==5:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==6:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==7:
                spoil_vec_by_deleting_element(y)
            f = [0.00,1.00,2.00,3.00]
            if _spoil_scenario==8:
                spoil_vec_by_nan(f)
            if _spoil_scenario==9:
                spoil_vec_by_posinf(f)
            if _spoil_scenario==10:
                spoil_vec_by_neginf(f)
            if _spoil_scenario==11:
                spoil_vec_by_deleting_element(f)
            s = xalglib.spline2dbuildbilinearv(x, 2, y, 2, f, 1)

            # copy spline, apply transformation x:=2*xnew, y:=4*ynew
            # evaluate at (xnew,ynew) = (0.25,0.25) - should be same as (x,y)=(0.5,1.0)
            snew = xalglib.spline2dcopy(s)
            xalglib.spline2dlintransxy(snew, 2.0, 0.0, 4.0, 0.0)
            v = xalglib.spline2dcalc(snew, 0.25, 0.25)
            _TestResult = _TestResult and doc_print_test(v, 2.500, "real", 0.00005)

            # copy spline, apply transformation SNew:=2*S+3
            snew = xalglib.spline2dcopy(s)
            xalglib.spline2dlintransf(snew, 2.0, 3.0)
            v = xalglib.spline2dcalc(snew, 0.5, 1.0)
            _TestResult = _TestResult and doc_print_test(v, 8.000, "real", 0.00005)

            #
            # Same example, but for vector spline (f0,f1) = {x+2*y, 2*x+y}
            #
            f2 = [0.00,0.00, 1.00,2.00, 2.00,1.00, 3.00,3.00]
            if _spoil_scenario==12:
                spoil_vec_by_nan(f2)
            if _spoil_scenario==13:
                spoil_vec_by_posinf(f2)
            if _spoil_scenario==14:
                spoil_vec_by_neginf(f2)
            if _spoil_scenario==15:
                spoil_vec_by_deleting_element(f2)
            s = xalglib.spline2dbuildbilinearv(x, 2, y, 2, f2, 2)

            # copy spline, apply transformation x:=2*xnew, y:=4*ynew
            snew = xalglib.spline2dcopy(s)
            xalglib.spline2dlintransxy(snew, 2.0, 0.0, 4.0, 0.0)
            vr = xalglib.spline2dcalcv(snew, 0.25, 0.25)
            _TestResult = _TestResult and doc_print_test(vr, [2.500,2.000], "real_vector", 0.00005)

            # copy spline, apply transformation SNew:=2*S+3
            snew = xalglib.spline2dcopy(s)
            xalglib.spline2dlintransf(snew, 2.0, 3.0)
            vr = xalglib.spline2dcalcv(snew, 0.5, 1.0)
            _TestResult = _TestResult and doc_print_test(vr, [8.000,7.000], "real_vector", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("spline2d_copytrans               FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST spline2d_vector
    #      Copy and transform
    #
    _TestResult = True
    for _spoil_scenario in range(-1,12):
        try:
            #
            # We build bilinear vector-valued spline (f0,f1) = {x+2*y, 2*x+y}
            # Spline is built using function values at 2x2 grid: (x,y)=[0,1]*[0,1]
            # Then we perform evaluation at (x,y)=(0.1,0.3)
            #
            x = [0.0, 1.0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(x)
            y = [0.0, 1.0]
            if _spoil_scenario==4:
                spoil_vec_by_nan(y)
            if _spoil_scenario==5:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==6:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==7:
                spoil_vec_by_deleting_element(y)
            f = [0.00,0.00, 1.00,2.00, 2.00,1.00, 3.00,3.00]
            if _spoil_scenario==8:
                spoil_vec_by_nan(f)
            if _spoil_scenario==9:
                spoil_vec_by_posinf(f)
            if _spoil_scenario==10:
                spoil_vec_by_neginf(f)
            if _spoil_scenario==11:
                spoil_vec_by_deleting_element(f)
            s = xalglib.spline2dbuildbilinearv(x, 2, y, 2, f, 2)
            vr = xalglib.spline2dcalcv(s, 0.1, 0.3)
            _TestResult = _TestResult and doc_print_test(vr, [0.700,0.500], "real_vector", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("spline2d_vector                  FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST spline3d_trilinear
    #      Trilinear spline interpolation
    #
    _TestResult = True
    for _spoil_scenario in range(-1,22):
        try:
            #
            # We use trilinear spline to interpolate f(x,y,z)=x+xy+z sampled 
            # at (x,y,z) from [0.0, 1.0] X [0.0, 1.0] X [0.0, 1.0].
            #
            # We store x, y and z-values at local arrays with same names.
            # Function values are stored in the array F as follows:
            #     f[0]     (x,y,z) = (0,0,0)
            #     f[1]     (x,y,z) = (1,0,0)
            #     f[2]     (x,y,z) = (0,1,0)
            #     f[3]     (x,y,z) = (1,1,0)
            #     f[4]     (x,y,z) = (0,0,1)
            #     f[5]     (x,y,z) = (1,0,1)
            #     f[6]     (x,y,z) = (0,1,1)
            #     f[7]     (x,y,z) = (1,1,1)
            #
            x = [0.0, 1.0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(x)
            y = [0.0, 1.0]
            if _spoil_scenario==4:
                spoil_vec_by_nan(y)
            if _spoil_scenario==5:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==6:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==7:
                spoil_vec_by_deleting_element(y)
            z = [0.0, 1.0]
            if _spoil_scenario==8:
                spoil_vec_by_nan(z)
            if _spoil_scenario==9:
                spoil_vec_by_posinf(z)
            if _spoil_scenario==10:
                spoil_vec_by_neginf(z)
            if _spoil_scenario==11:
                spoil_vec_by_deleting_element(z)
            f = [0,1,0,2,1,2,1,3]
            if _spoil_scenario==12:
                spoil_vec_by_nan(f)
            if _spoil_scenario==13:
                spoil_vec_by_posinf(f)
            if _spoil_scenario==14:
                spoil_vec_by_neginf(f)
            if _spoil_scenario==15:
                spoil_vec_by_deleting_element(f)
            vx = 0.50
            if _spoil_scenario==16:
                vx = float("+inf")
            if _spoil_scenario==17:
                vx = float("-inf")
            vy = 0.50
            if _spoil_scenario==18:
                vy = float("+inf")
            if _spoil_scenario==19:
                vy = float("-inf")
            vz = 0.50
            if _spoil_scenario==20:
                vz = float("+inf")
            if _spoil_scenario==21:
                vz = float("-inf")

            # build spline
            s = xalglib.spline3dbuildtrilinearv(x, 2, y, 2, z, 2, f, 1)

            # calculate S(0.5,0.5,0.5)
            v = xalglib.spline3dcalc(s, vx, vy, vz)
            _TestResult = _TestResult and doc_print_test(v, 1.2500, "real", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("spline3d_trilinear               FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST spline3d_vector
    #      Vector-valued trilinear spline interpolation
    #
    _TestResult = True
    for _spoil_scenario in range(-1,22):
        try:
            #
            # We use trilinear vector-valued spline to interpolate {f0,f1}={x+xy+z,x+xy+yz+z}
            # sampled at (x,y,z) from [0.0, 1.0] X [0.0, 1.0] X [0.0, 1.0].
            #
            # We store x, y and z-values at local arrays with same names.
            # Function values are stored in the array F as follows:
            #     f[0]     f0, (x,y,z) = (0,0,0)
            #     f[1]     f1, (x,y,z) = (0,0,0)
            #     f[2]     f0, (x,y,z) = (1,0,0)
            #     f[3]     f1, (x,y,z) = (1,0,0)
            #     f[4]     f0, (x,y,z) = (0,1,0)
            #     f[5]     f1, (x,y,z) = (0,1,0)
            #     f[6]     f0, (x,y,z) = (1,1,0)
            #     f[7]     f1, (x,y,z) = (1,1,0)
            #     f[8]     f0, (x,y,z) = (0,0,1)
            #     f[9]     f1, (x,y,z) = (0,0,1)
            #     f[10]    f0, (x,y,z) = (1,0,1)
            #     f[11]    f1, (x,y,z) = (1,0,1)
            #     f[12]    f0, (x,y,z) = (0,1,1)
            #     f[13]    f1, (x,y,z) = (0,1,1)
            #     f[14]    f0, (x,y,z) = (1,1,1)
            #     f[15]    f1, (x,y,z) = (1,1,1)
            #
            x = [0.0, 1.0]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            if _spoil_scenario==3:
                spoil_vec_by_deleting_element(x)
            y = [0.0, 1.0]
            if _spoil_scenario==4:
                spoil_vec_by_nan(y)
            if _spoil_scenario==5:
                spoil_vec_by_posinf(y)
            if _spoil_scenario==6:
                spoil_vec_by_neginf(y)
            if _spoil_scenario==7:
                spoil_vec_by_deleting_element(y)
            z = [0.0, 1.0]
            if _spoil_scenario==8:
                spoil_vec_by_nan(z)
            if _spoil_scenario==9:
                spoil_vec_by_posinf(z)
            if _spoil_scenario==10:
                spoil_vec_by_neginf(z)
            if _spoil_scenario==11:
                spoil_vec_by_deleting_element(z)
            f = [0,0, 1,1, 0,0, 2,2, 1,1, 2,2, 1,2, 3,4]
            if _spoil_scenario==12:
                spoil_vec_by_nan(f)
            if _spoil_scenario==13:
                spoil_vec_by_posinf(f)
            if _spoil_scenario==14:
                spoil_vec_by_neginf(f)
            if _spoil_scenario==15:
                spoil_vec_by_deleting_element(f)
            vx = 0.50
            if _spoil_scenario==16:
                vx = float("+inf")
            if _spoil_scenario==17:
                vx = float("-inf")
            vy = 0.50
            if _spoil_scenario==18:
                vy = float("+inf")
            if _spoil_scenario==19:
                vy = float("-inf")
            vz = 0.50
            if _spoil_scenario==20:
                vz = float("+inf")
            if _spoil_scenario==21:
                vz = float("-inf")

            # build spline
            s = xalglib.spline3dbuildtrilinearv(x, 2, y, 2, z, 2, f, 2)

            # calculate S(0.5,0.5,0.5) - we have vector of values instead of single value
            v = xalglib.spline3dcalcv(s, vx, vy, vz)
            _TestResult = _TestResult and doc_print_test(v, [1.2500,1.5000], "real_vector", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("spline3d_vector                  FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST rbf_d_hrbf
    #      Simple model built with HRBF algorithm
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # This example illustrates basic concepts of the RBF models: creation, modification,
            # evaluation.
            # 
            # Suppose that we have set of 2-dimensional points with associated
            # scalar function values, and we want to build a RBF model using
            # our data.
            # 
            # NOTE: we can work with 3D models too :)
            # 
            # Typical sequence of steps is given below:
            # 1. we create RBF model object
            # 2. we attach our dataset to the RBF model and tune algorithm settings
            # 3. we rebuild RBF model using QNN algorithm on new data
            # 4. we use RBF model (evaluate, serialize, etc.)
            #

            #
            # Step 1: RBF model creation.
            #
            # We have to specify dimensionality of the space (2 or 3) and
            # dimensionality of the function (scalar or vector).
            #
            # New model is empty - it can be evaluated,
            # but we just get zero value at any point.
            #
            model = xalglib.rbfcreate(2, 1)

            v = xalglib.rbfcalc2(model, 0.0, 0.0)
            _TestResult = _TestResult and doc_print_test(v, 0.000, "real", 0.005)

            #
            # Step 2: we add dataset.
            #
            # XY contains two points - x0=(-1,0) and x1=(+1,0) -
            # and two function values f(x0)=2, f(x1)=3.
            #
            # We added points, but model was not rebuild yet.
            # If we call rbfcalc2(), we still will get 0.0 as result.
            #
            xy = [[-1,0,2],[+1,0,3]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)
            xalglib.rbfsetpoints(model, xy)

            v = xalglib.rbfcalc2(model, 0.0, 0.0)
            _TestResult = _TestResult and doc_print_test(v, 0.000, "real", 0.005)

            #
            # Step 3: rebuild model
            #
            # After we've configured model, we should rebuild it -
            # it will change coefficients stored internally in the
            # rbfmodel structure.
            #
            # We use hierarchical RBF algorithm with following parameters:
            # * RBase - set to 1.0
            # * NLayers - three layers are used (although such simple problem
            #   does not need more than 1 layer)
            # * LambdaReg - is set to zero value, no smoothing is required
            #
            xalglib.rbfsetalgohierarchical(model, 1.0, 3, 0.0)
            rep = xalglib.rbfbuildmodel(model)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")

            #
            # Step 4: model was built
            #
            # After call of rbfbuildmodel(), rbfcalc2() will return
            # value of the new model.
            #
            v = xalglib.rbfcalc2(model, 0.0, 0.0)
            _TestResult = _TestResult and doc_print_test(v, 2.500, "real", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("rbf_d_hrbf                       FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST rbf_d_vector
    #      Working with vector functions
    #
    _TestResult = True
    for _spoil_scenario in range(-1,6):
        try:
            #
            # Suppose that we have set of 2-dimensional points with associated VECTOR
            # function values, and we want to build a RBF model using our data.
            # 
            # Typical sequence of steps is given below:
            # 1. we create RBF model object
            # 2. we attach our dataset to the RBF model and tune algorithm settings
            # 3. we rebuild RBF model using new data
            # 4. we use RBF model (evaluate, serialize, etc.)
            #

            #
            # Step 1: RBF model creation.
            #
            # We have to specify dimensionality of the space (equal to 2) and
            # dimensionality of the function (2-dimensional vector function).
            #
            # New model is empty - it can be evaluated,
            # but we just get zero value at any point.
            #
            model = xalglib.rbfcreate(2, 2)

            x = [+1,+1]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            y = xalglib.rbfcalc(model, x)
            _TestResult = _TestResult and doc_print_test(y, [0.000,0.000], "real_vector", 0.005)

            #
            # Step 2: we add dataset.
            #
            # XY arrays containt four points:
            # * (x0,y0) = (+1,+1), f(x0,y0)=(0,-1)
            # * (x1,y1) = (+1,-1), f(x1,y1)=(-1,0)
            # * (x2,y2) = (-1,-1), f(x2,y2)=(0,+1)
            # * (x3,y3) = (-1,+1), f(x3,y3)=(+1,0)
            #
            xy = [[+1,+1,0,-1],[+1,-1,-1,0],[-1,-1,0,+1],[-1,+1,+1,0]]
            if _spoil_scenario==3:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==4:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==5:
                spoil_mat_by_neginf(xy)
            xalglib.rbfsetpoints(model, xy)

            # We added points, but model was not rebuild yet.
            # If we call rbfcalc(), we still will get 0.0 as result.
            y = xalglib.rbfcalc(model, x)
            _TestResult = _TestResult and doc_print_test(y, [0.000,0.000], "real_vector", 0.005)

            #
            # Step 3: rebuild model
            #
            # We use hierarchical RBF algorithm with following parameters:
            # * RBase - set to 1.0
            # * NLayers - three layers are used (although such simple problem
            #   does not need more than 1 layer)
            # * LambdaReg - is set to zero value, no smoothing is required
            #
            # After we've configured model, we should rebuild it -
            # it will change coefficients stored internally in the
            # rbfmodel structure.
            #
            xalglib.rbfsetalgohierarchical(model, 1.0, 3, 0.0)
            rep = xalglib.rbfbuildmodel(model)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")

            #
            # Step 4: model was built
            #
            # After call of rbfbuildmodel(), rbfcalc() will return
            # value of the new model.
            #
            y = xalglib.rbfcalc(model, x)
            _TestResult = _TestResult and doc_print_test(y, [0.000,-1.000], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("rbf_d_vector                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST rbf_d_polterm
    #      RBF models - working with polynomial term
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # This example show how to work with polynomial term
            # 
            # Suppose that we have set of 2-dimensional points with associated
            # scalar function values, and we want to build a RBF model using
            # our data.
            #
            # We use hierarchical RBF algorithm with following parameters:
            # * RBase - set to 1.0
            # * NLayers - three layers are used (although such simple problem
            #   does not need more than 1 layer)
            # * LambdaReg - is set to zero value, no smoothing is required
            #
            xy = [[-1,0,2],[+1,0,3]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)

            model = xalglib.rbfcreate(2, 1)
            xalglib.rbfsetpoints(model, xy)
            xalglib.rbfsetalgohierarchical(model, 1.0, 3, 0.0)

            #
            # By default, RBF model uses linear term. It means that model
            # looks like
            #     f(x,y) = SUM(RBF[i]) + a*x + b*y + c
            # where RBF[i] is I-th radial basis function and a*x+by+c is a
            # linear term. Having linear terms in a model gives us:
            # (1) improved extrapolation properties
            # (2) linearity of the model when data can be perfectly fitted
            #     by the linear function
            # (3) linear asymptotic behavior
            #
            # Our simple dataset can be modelled by the linear function
            #     f(x,y) = 0.5*x + 2.5
            # and rbfbuildmodel() with default settings should preserve this
            # linearity.
            #
            rep = xalglib.rbfbuildmodel(model)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            nx, ny, xwr, nc, c, modelversion = xalglib.rbfunpack(model)
            _TestResult = _TestResult and doc_print_test(c, [[0.500,0.000,2.500]], "real_matrix", 0.005)

            # asymptotic behavior of our function is linear
            v = xalglib.rbfcalc2(model, 1000.0, 0.0)
            _TestResult = _TestResult and doc_print_test(v, 502.50, "real", 0.05)

            #
            # Instead of linear term we can use constant term. In this case
            # we will get model which has form
            #     f(x,y) = SUM(RBF[i]) + c
            # where RBF[i] is I-th radial basis function and c is a constant,
            # which is equal to the average function value on the dataset.
            #
            # Because we've already attached dataset to the model the only
            # thing we have to do is to call rbfsetconstterm() and then
            # rebuild model with rbfbuildmodel().
            #
            xalglib.rbfsetconstterm(model)
            rep = xalglib.rbfbuildmodel(model)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            nx, ny, xwr, nc, c, modelversion = xalglib.rbfunpack(model)
            _TestResult = _TestResult and doc_print_test(c, [[0.000,0.000,2.500]], "real_matrix", 0.005)

            # asymptotic behavior of our function is constant
            v = xalglib.rbfcalc2(model, 1000.0, 0.0)
            _TestResult = _TestResult and doc_print_test(v, 2.500, "real", 0.005)

            #
            # Finally, we can use zero term. Just plain RBF without polynomial
            # part:
            #     f(x,y) = SUM(RBF[i])
            # where RBF[i] is I-th radial basis function.
            #
            xalglib.rbfsetzeroterm(model)
            rep = xalglib.rbfbuildmodel(model)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")
            nx, ny, xwr, nc, c, modelversion = xalglib.rbfunpack(model)
            _TestResult = _TestResult and doc_print_test(c, [[0.000,0.000,0.000]], "real_matrix", 0.005)

            # asymptotic behavior of our function is just zero constant
            v = xalglib.rbfcalc2(model, 1000.0, 0.0)
            _TestResult = _TestResult and doc_print_test(v, 0.000, "real", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("rbf_d_polterm                    FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST rbf_d_serialize
    #      Serialization/unserialization
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # This example show how to serialize and unserialize RBF model
            # 
            # Suppose that we have set of 2-dimensional points with associated
            # scalar function values, and we want to build a RBF model using
            # our data. Then we want to serialize it to string and to unserialize
            # from string, loading to another instance of RBF model.
            #
            # Here we assume that you already know how to create RBF models.
            #
            xy = [[-1,0,2],[+1,0,3]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)

            # model initialization
            model0 = xalglib.rbfcreate(2, 1)
            xalglib.rbfsetpoints(model0, xy)
            xalglib.rbfsetalgohierarchical(model0, 1.0, 3, 0.0)
            rep = xalglib.rbfbuildmodel(model0)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")

            #
            # Serialization - it looks easy,
            # but you should carefully read next section.
            #
            s = xalglib.rbfserialize(model0)
            model1 = xalglib.rbfunserialize(s)

            # both models return same value
            v = xalglib.rbfcalc2(model0, 0.0, 0.0)
            _TestResult = _TestResult and doc_print_test(v, 2.500, "real", 0.005)
            v = xalglib.rbfcalc2(model1, 0.0, 0.0)
            _TestResult = _TestResult and doc_print_test(v, 2.500, "real", 0.005)

            #
            # Previous section shows that model state is saved/restored during
            # serialization. However, some properties are NOT serialized.
            #
            # Serialization saves/restores RBF model, but it does NOT saves/restores
            # settings which were used to build current model. In particular, dataset
            # which was used to build model, is not preserved.
            #
            # What does it mean in for us?
            #
            # Do you remember this sequence: rbfcreate-rbfsetpoints-rbfbuildmodel?
            # First step creates model, second step adds dataset and tunes model
            # settings, third step builds model using current dataset and model
            # construction settings.
            #
            # If you call rbfbuildmodel() without calling rbfsetpoints() first, you
            # will get empty (zero) RBF model. In our example, model0 contains
            # dataset which was added by rbfsetpoints() call. However, model1 does
            # NOT contain dataset - because dataset is NOT serialized.
            #
            # This, if we call rbfbuildmodel(model0,rep), we will get same model,
            # which returns 2.5 at (x,y)=(0,0). However, after same call model1 will
            # return zero - because it contains RBF model (coefficients), but does NOT
            # contain dataset which was used to build this model.
            #
            # Basically, it means that:
            # * serialization of the RBF model preserves anything related to the model
            #   EVALUATION
            # * but it does NOT creates perfect copy of the original object.
            #
            rep = xalglib.rbfbuildmodel(model0)
            v = xalglib.rbfcalc2(model0, 0.0, 0.0)
            _TestResult = _TestResult and doc_print_test(v, 2.500, "real", 0.005)

            rep = xalglib.rbfbuildmodel(model1)
            v = xalglib.rbfcalc2(model1, 0.0, 0.0)
            _TestResult = _TestResult and doc_print_test(v, 0.000, "real", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("rbf_d_serialize                  FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST fft_complex_d1
    #      Complex FFT: simple example
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # first we demonstrate forward FFT:
            # [1i,1i,1i,1i] is converted to [4i, 0, 0, 0]
            #
            z = [1j,1j,1j,1j]
            if _spoil_scenario==0:
                spoil_vec_by_nan(z)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(z)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(z)
            xalglib.fftc1d(z)
            _TestResult = _TestResult and doc_print_test(z, [4j,0,0,0], "complex_vector", 0.0001)

            #
            # now we convert [4i, 0, 0, 0] back to [1i,1i,1i,1i]
            # with backward FFT
            #
            xalglib.fftc1dinv(z)
            _TestResult = _TestResult and doc_print_test(z, [1j,1j,1j,1j], "complex_vector", 0.0001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("fft_complex_d1                   FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST fft_complex_d2
    #      Complex FFT: advanced example
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # first we demonstrate forward FFT:
            # [0,1,0,1i] is converted to [1+1i, -1-1i, -1-1i, 1+1i]
            #
            z = [0,1,0,1j]
            if _spoil_scenario==0:
                spoil_vec_by_nan(z)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(z)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(z)
            xalglib.fftc1d(z)
            _TestResult = _TestResult and doc_print_test(z, [1+1j, -1-1j, -1-1j, 1+1j], "complex_vector", 0.0001)

            #
            # now we convert result back with backward FFT
            #
            xalglib.fftc1dinv(z)
            _TestResult = _TestResult and doc_print_test(z, [0,1,0,1j], "complex_vector", 0.0001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("fft_complex_d2                   FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST fft_real_d1
    #      Real FFT: simple example
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # first we demonstrate forward FFT:
            # [1,1,1,1] is converted to [4, 0, 0, 0]
            #
            x = [1,1,1,1]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            f = xalglib.fftr1d(x)
            _TestResult = _TestResult and doc_print_test(f, [4,0,0,0], "complex_vector", 0.0001)

            #
            # now we convert [4, 0, 0, 0] back to [1,1,1,1]
            # with backward FFT
            #
            x2 = xalglib.fftr1dinv(f)
            _TestResult = _TestResult and doc_print_test(x2, [1,1,1,1], "real_vector", 0.0001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("fft_real_d1                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST fft_real_d2
    #      Real FFT: advanced example
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # first we demonstrate forward FFT:
            # [1,2,3,4] is converted to [10, -2+2i, -2, -2-2i]
            #
            # note that output array is self-adjoint:
            # * f[0] = conj(f[0])
            # * f[1] = conj(f[3])
            # * f[2] = conj(f[2])
            #
            x = [1,2,3,4]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)
            f = xalglib.fftr1d(x)
            _TestResult = _TestResult and doc_print_test(f, [10, -2+2j, -2, -2-2j], "complex_vector", 0.0001)

            #
            # now we convert [10, -2+2i, -2, -2-2i] back to [1,2,3,4]
            #
            x2 = xalglib.fftr1dinv(f)
            _TestResult = _TestResult and doc_print_test(x2, [1,2,3,4], "real_vector", 0.0001)

            #
            # remember that F is self-adjoint? It means that we can pass just half
            # (slightly larger than half) of F to inverse real FFT and still get our result.
            #
            # I.e. instead [10, -2+2i, -2, -2-2i] we pass just [10, -2+2i, -2] and everything works!
            #
            # NOTE: in this case we should explicitly pass array length (which is 4) to ALGLIB;
            # if not, it will automatically use array length to determine FFT size and
            # will erroneously make half-length FFT.
            #
            f = [10, -2+2j, -2]
            x2 = xalglib.fftr1dinv(f, 4)
            _TestResult = _TestResult and doc_print_test(x2, [1,2,3,4], "real_vector", 0.0001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("fft_real_d2                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST fft_complex_e1
    #      error detection in backward FFT
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            z = [0,2,0,-2]
            if _spoil_scenario==0:
                spoil_vec_by_nan(z)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(z)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(z)
            xalglib.fftc1dinv(z)
            _TestResult = _TestResult and doc_print_test(z, [0,1j,0,-1j], "complex_vector", 0.0001)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("fft_complex_e1                   FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST clst_ahc
    #      Simple hierarchical clusterization with Euclidean distance function
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # The very simple clusterization example
            #
            # We have a set of points in 2D space:
            #     (P0,P1,P2,P3,P4) = ((1,1),(1,2),(4,1),(2,3),(4,1.5))
            #
            #  |
            #  |     P3
            #  |
            #  | P1          
            #  |             P4
            #  | P0          P2
            #  |-------------------------
            #
            # We want to perform Agglomerative Hierarchic Clusterization (AHC),
            # using complete linkage (default algorithm) and Euclidean distance
            # (default metric).
            #
            # In order to do that, we:
            # * create clusterizer with clusterizercreate()
            # * set points XY and metric (2=Euclidean) with clusterizersetpoints()
            # * run AHC algorithm with clusterizerrunahc
            #
            # You may see that clusterization itself is a minor part of the example,
            # most of which is dominated by comments :)
            #
            xy = [[1,1],[1,2],[4,1],[2,3],[4,1.5]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)

            s = xalglib.clusterizercreate()
            xalglib.clusterizersetpoints(s, xy, 2)
            rep = xalglib.clusterizerrunahc(s)

            #
            # Now we've built our clusterization tree. Rep.z contains information which
            # is required to build dendrogram. I-th row of rep.z represents one merge
            # operation, with first cluster to merge having index rep.z[I,0] and second
            # one having index rep.z[I,1]. Merge result has index NPoints+I.
            #
            # Clusters with indexes less than NPoints are single-point initial clusters,
            # while ones with indexes from NPoints to 2*NPoints-2 are multi-point
            # clusters created during merges.
            #
            # In our example, Z=[[2,4], [0,1], [3,6], [5,7]]
            #
            # It means that:
            # * first, we merge C2=(P2) and C4=(P4),    and create C5=(P2,P4)
            # * then, we merge  C2=(P0) and C1=(P1),    and create C6=(P0,P1)
            # * then, we merge  C3=(P3) and C6=(P0,P1), and create C7=(P0,P1,P3)
            # * finally, we merge C5 and C7 and create C8=(P0,P1,P2,P3,P4)
            #
            # Thus, we have following dendrogram:
            #  
            #      ------8-----
            #      |          |
            #      |      ----7----
            #      |      |       |
            #   ---5---   |    ---6---
            #   |     |   |    |     |
            #   P2   P4   P3   P0   P1
            #
            _TestResult = _TestResult and doc_print_test(rep.z, [[2,4],[0,1],[3,6],[5,7]], "int_matrix")

            #
            # We've built dendrogram above by reordering our dataset.
            #
            # Without such reordering it would be impossible to build dendrogram without
            # intersections. Luckily, ahcreport structure contains two additional fields
            # which help to build dendrogram from your data:
            # * rep.p, which contains permutation applied to dataset
            # * rep.pm, which contains another representation of merges 
            #
            # In our example we have:
            # * P=[3,4,0,2,1]
            # * PZ=[[0,0,1,1,0,0],[3,3,4,4,0,0],[2,2,3,4,0,1],[0,1,2,4,1,2]]
            #
            # Permutation array P tells us that P0 should be moved to position 3,
            # P1 moved to position 4, P2 moved to position 0 and so on:
            #
            #   (P0 P1 P2 P3 P4) => (P2 P4 P3 P0 P1)
            #
            # Merges array PZ tells us how to perform merges on the sorted dataset.
            # One row of PZ corresponds to one merge operations, with first pair of
            # elements denoting first of the clusters to merge (start index, end
            # index) and next pair of elements denoting second of the clusters to
            # merge. Clusters being merged are always adjacent, with first one on
            # the left and second one on the right.
            #
            # For example, first row of PZ tells us that clusters [0,0] and [1,1] are
            # merged (single-point clusters, with first one containing P2 and second
            # one containing P4). Third row of PZ tells us that we merge one single-
            # point cluster [2,2] with one two-point cluster [3,4].
            #
            # There are two more elements in each row of PZ. These are the helper
            # elements, which denote HEIGHT (not size) of left and right subdendrograms.
            # For example, according to PZ, first two merges are performed on clusterization
            # trees of height 0, while next two merges are performed on 0-1 and 1-2
            # pairs of trees correspondingly.
            #
            _TestResult = _TestResult and doc_print_test(rep.p, [3,4,0,2,1], "int_vector")
            _TestResult = _TestResult and doc_print_test(rep.pm, [[0,0,1,1,0,0],[3,3,4,4,0,0],[2,2,3,4,0,1],[0,1,2,4,1,2]], "int_matrix")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("clst_ahc                         FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST clst_kmeans
    #      Simple k-means clusterization
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # The very simple clusterization example
            #
            # We have a set of points in 2D space:
            #     (P0,P1,P2,P3,P4) = ((1,1),(1,2),(4,1),(2,3),(4,1.5))
            #
            #  |
            #  |     P3
            #  |
            #  | P1          
            #  |             P4
            #  | P0          P2
            #  |-------------------------
            #
            # We want to perform k-means++ clustering with K=2.
            #
            # In order to do that, we:
            # * create clusterizer with clusterizercreate()
            # * set points XY and metric (must be Euclidean, distype=2) with clusterizersetpoints()
            # * (optional) set number of restarts from random positions to 5
            # * run k-means algorithm with clusterizerrunkmeans()
            #
            # You may see that clusterization itself is a minor part of the example,
            # most of which is dominated by comments :)
            #
            xy = [[1,1],[1,2],[4,1],[2,3],[4,1.5]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)

            s = xalglib.clusterizercreate()
            xalglib.clusterizersetpoints(s, xy, 2)
            xalglib.clusterizersetkmeanslimits(s, 5, 0)
            rep = xalglib.clusterizerrunkmeans(s, 2)

            #
            # We've performed clusterization, and it succeeded (completion code is +1).
            #
            # Now first center is stored in the first row of rep.c, second one is stored
            # in the second row. rep.cidx can be used to determine which center is
            # closest to some specific point of the dataset.
            #
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, 1, "int")

            # We called clusterizersetpoints() with disttype=2 because k-means++
            # algorithm does NOT support metrics other than Euclidean. But what if we
            # try to use some other metric?
            #
            # We change metric type by calling clusterizersetpoints() one more time,
            # and try to run k-means algo again. It fails.
            #
            xalglib.clusterizersetpoints(s, xy, 0)
            rep = xalglib.clusterizerrunkmeans(s, 2)
            _TestResult = _TestResult and doc_print_test(rep.terminationtype, -5, "int")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("clst_kmeans                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST clst_linkage
    #      Clusterization with different linkage types
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # We have a set of points in 1D space:
            #     (P0,P1,P2,P3,P4) = (1, 3, 10, 16, 20)
            #
            # We want to perform Agglomerative Hierarchic Clusterization (AHC),
            # using either complete or single linkage and Euclidean distance
            # (default metric).
            #
            # First two steps merge P0/P1 and P3/P4 independently of the linkage type.
            # However, third step depends on linkage type being used:
            # * in case of complete linkage P2=10 is merged with [P0,P1]
            # * in case of single linkage P2=10 is merged with [P3,P4]
            #
            xy = [[1],[3],[10],[16],[20]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)

            s = xalglib.clusterizercreate()
            xalglib.clusterizersetpoints(s, xy, 2)

            # use complete linkage, reduce set down to 2 clusters.
            # print clusterization with clusterizergetkclusters(2).
            # P2 must belong to [P0,P1]
            xalglib.clusterizersetahcalgo(s, 0)
            rep = xalglib.clusterizerrunahc(s)
            cidx, cz = xalglib.clusterizergetkclusters(rep, 2)
            _TestResult = _TestResult and doc_print_test(cidx, [1,1,1,0,0], "int_vector")

            # use single linkage, reduce set down to 2 clusters.
            # print clusterization with clusterizergetkclusters(2).
            # P2 must belong to [P2,P3]
            xalglib.clusterizersetahcalgo(s, 1)
            rep = xalglib.clusterizerrunahc(s)
            cidx, cz = xalglib.clusterizergetkclusters(rep, 2)
            _TestResult = _TestResult and doc_print_test(cidx, [0,0,1,1,1], "int_vector")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("clst_linkage                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST clst_distance
    #      Clusterization with different metric types
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # We have three points in 4D space:
            #     (P0,P1,P2) = ((1, 2, 1, 2), (6, 7, 6, 7), (7, 6, 7, 6))
            #
            # We want to try clustering them with different distance functions.
            # Distance function is chosen when we add dataset to the clusterizer.
            # We can choose several distance types - Euclidean, city block, Chebyshev,
            # several correlation measures or user-supplied distance matrix.
            #
            # Here we'll try three distances: Euclidean, Pearson correlation,
            # user-supplied distance matrix. Different distance functions lead
            # to different choices being made by algorithm during clustering.
            #
            xy = [[1, 2, 1, 2], [6, 7, 6, 7], [7, 6, 7, 6]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)
            s = xalglib.clusterizercreate()

            # With Euclidean distance function (disttype=2) two closest points
            # are P1 and P2, thus:
            # * first, we merge P1 and P2 to form C3=[P1,P2]
            # * second, we merge P0 and C3 to form C4=[P0,P1,P2]
            disttype = 2
            xalglib.clusterizersetpoints(s, xy, disttype)
            rep = xalglib.clusterizerrunahc(s)
            _TestResult = _TestResult and doc_print_test(rep.z, [[1,2],[0,3]], "int_matrix")

            # With Pearson correlation distance function (disttype=10) situation
            # is different - distance between P0 and P1 is zero, thus:
            # * first, we merge P0 and P1 to form C3=[P0,P1]
            # * second, we merge P2 and C3 to form C4=[P0,P1,P2]
            disttype = 10
            xalglib.clusterizersetpoints(s, xy, disttype)
            rep = xalglib.clusterizerrunahc(s)
            _TestResult = _TestResult and doc_print_test(rep.z, [[0,1],[2,3]], "int_matrix")

            # Finally, we try clustering with user-supplied distance matrix:
            #     [ 0 3 1 ]
            # P = [ 3 0 3 ], where P[i,j] = dist(Pi,Pj)
            #     [ 1 3 0 ]
            #
            # * first, we merge P0 and P2 to form C3=[P0,P2]
            # * second, we merge P1 and C3 to form C4=[P0,P1,P2]
            d = [[0,3,1],[3,0,3],[1,3,0]]
            xalglib.clusterizersetdistances(s, d, True)
            rep = xalglib.clusterizerrunahc(s)
            _TestResult = _TestResult and doc_print_test(rep.z, [[0,2],[1,3]], "int_matrix")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("clst_distance                    FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST clst_kclusters
    #      Obtaining K top clusters from clusterization tree
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # We have a set of points in 2D space:
            #     (P0,P1,P2,P3,P4) = ((1,1),(1,2),(4,1),(2,3),(4,1.5))
            #
            #  |
            #  |     P3
            #  |
            #  | P1          
            #  |             P4
            #  | P0          P2
            #  |-------------------------
            #
            # We perform Agglomerative Hierarchic Clusterization (AHC) and we want
            # to get top K clusters from clusterization tree for different K.
            #
            xy = [[1,1],[1,2],[4,1],[2,3],[4,1.5]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)

            s = xalglib.clusterizercreate()
            xalglib.clusterizersetpoints(s, xy, 2)
            rep = xalglib.clusterizerrunahc(s)

            # with K=5, every points is assigned to its own cluster:
            # C0=P0, C1=P1 and so on...
            cidx, cz = xalglib.clusterizergetkclusters(rep, 5)
            _TestResult = _TestResult and doc_print_test(cidx, [0,1,2,3,4], "int_vector")

            # with K=1 we have one large cluster C0=[P0,P1,P2,P3,P4,P5]
            cidx, cz = xalglib.clusterizergetkclusters(rep, 1)
            _TestResult = _TestResult and doc_print_test(cidx, [0,0,0,0,0], "int_vector")

            # with K=3 we have three clusters C0=[P3], C1=[P2,P4], C2=[P0,P1]
            cidx, cz = xalglib.clusterizergetkclusters(rep, 3)
            _TestResult = _TestResult and doc_print_test(cidx, [2,2,1,0,1], "int_vector")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("clst_kclusters                   FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST randomforest_cls
    #      Simple classification with random forests
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # The very simple classification example: classify points (x,y) in 2D space
            # as ones with x>=0 and ones with x<0 (y is ignored, but our classifier
            # has to find out it).
            #
            # First, we have to create decision forest builder object, load dataset and
            # specify training settings. Our dataset is specified as matrix, which has
            # following format:
            #
            #     x0 y0 class0
            #     x1 y1 class1
            #     x2 y2 class2
            #     ....
            #
            # Here xi and yi can be any values (and in fact you can have any number of
            # independent variables), and classi MUST be integer number in [0,NClasses)
            # range. In our example we denote points with x>=0 as class #0, and
            # ones with negative xi as class #1.
            #
            # NOTE: if you want to solve regression problem, specify NClasses=1. In
            #       this case last column of xy can be any numeric value.
            #
            # For the sake of simplicity, our example includes only 4-point dataset.
            # However, random forests are able to cope with extremely large datasets
            # having millions of examples.
            #
            nvars = 2
            nclasses = 2
            npoints = 4
            xy = [[1,1,0],[1,-1,0],[-1,1,1],[-1,-1,1]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)

            builder = xalglib.dfbuildercreate()
            xalglib.dfbuildersetdataset(builder, xy, npoints, nvars, nclasses)

            # in our example we train decision forest using full sample - it allows us
            # to get zero classification error. However, in practical applications smaller
            # values are used: 50%, 25%, 5% or even less.
            xalglib.dfbuildersetsubsampleratio(builder, 1.0)

            # we train random forest with just one tree; again, in real life situations
            # you typically need from 50 to 500 trees.
            ntrees = 1
            forest, rep = xalglib.dfbuilderbuildrandomforest(builder, ntrees)

            # with such settings (100% of the training set is used) you can expect
            # zero classification error. Beautiful results, but remember - in real life
            # you do not need zero TRAINING SET error, you need good generalization.

            _TestResult = _TestResult and doc_print_test(rep.relclserror, 0.0000, "real", 0.00005)

            # now, let's perform some simple processing with dfprocess()
            x = [+1,0]
            y = []
            y = xalglib.dfprocess(forest, x, y)
            _TestResult = _TestResult and doc_print_test(y, [+1,0], "real_vector", 0.0005)

            # another option is to use dfprocess0() which returns just first component
            # of the output vector y. ideal for regression problems and binary classifiers.
            y0 = xalglib.dfprocess0(forest, x)
            _TestResult = _TestResult and doc_print_test(y0, 1.000, "real", 0.0005)

            # finally, you can use dfclassify() which returns most probable class index (i.e. argmax y[i]).
            i = xalglib.dfclassify(forest, x)
            _TestResult = _TestResult and doc_print_test(i, 0, "int")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("randomforest_cls                 FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST randomforest_reg
    #      Simple regression with decision forest
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # The very simple regression example: model f(x,y)=x+y
            #
            # First, we have to create DF builder object, load dataset and specify
            # training settings. Our dataset is specified as matrix, which has following
            # format:
            #
            #     x0 y0 f0
            #     x1 y1 f1
            #     x2 y2 f2
            #     ....
            #
            # Here xi and yi can be any values, and fi is a dependent function value.
            #
            # NOTE: you can also solve classification problems with DF models, see
            #       another example for this unit.
            #
            nvars = 2
            nclasses = 1
            npoints = 4
            xy = [[1,1,+2],[1,-1,0],[-1,1,0],[-1,-1,-2]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)

            builder = xalglib.dfbuildercreate()
            xalglib.dfbuildersetdataset(builder, xy, npoints, nvars, nclasses)

            # in our example we train decision forest using full sample - it allows us
            # to get zero classification error. However, in practical applications smaller
            # values are used: 50%, 25%, 5% or even less.
            xalglib.dfbuildersetsubsampleratio(builder, 1.0)

            # we train random forest with just one tree; again, in real life situations
            # you typically need from 50 to 500 trees.
            ntrees = 1
            model, rep = xalglib.dfbuilderbuildrandomforest(builder, ntrees)

            # with such settings (full sample is used) you can expect zero RMS error on the
            # training set. Beautiful results, but remember - in real life you do not
            # need zero TRAINING SET error, you need good generalization.

            _TestResult = _TestResult and doc_print_test(rep.rmserror, 0.0000, "real", 0.00005)

            # now, let's perform some simple processing with dfprocess()
            x = [+1,+1]
            y = []
            y = xalglib.dfprocess(model, x, y)
            _TestResult = _TestResult and doc_print_test(y, [+2], "real_vector", 0.0005)

            # another option is to use dfprocess0() which returns just first component
            # of the output vector y. ideal for regression problems and binary classifiers.
            y0 = xalglib.dfprocess0(model, x)
            _TestResult = _TestResult and doc_print_test(y0, 2.000, "real", 0.0005)

            # there also exist another convenience function, dfclassify(),
            # but it does not work for regression problems - it always returns -1.
            i = xalglib.dfclassify(model, x)
            _TestResult = _TestResult and doc_print_test(i, -1, "int")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("randomforest_reg                 FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST linreg_d_basic
    #      Linear regression used to build the very basic model and unpack coefficients
    #
    _TestResult = True
    try:
        #
        # In this example we demonstrate linear fitting by f(x|a) = a*exp(0.5*x).
        #
        # We have:
        # * xy - matrix of basic function values (exp(0.5*x)) and expected values
        #
        xy = [[0.606531,1.133719],[0.670320,1.306522],[0.740818,1.504604],[0.818731,1.554663],[0.904837,1.884638],[1.000000,2.072436],[1.105171,2.257285],[1.221403,2.534068],[1.349859,2.622017],[1.491825,2.897713],[1.648721,3.219371]]

        model, rep = xalglib.lrbuildz(xy, 11, 1)
        c, nvars = xalglib.lrunpack(model)
        _TestResult = _TestResult and doc_print_test(c, [1.98650,0.00000], "real_vector", 0.00005)
    except (RuntimeError, ValueError):
        _TestResult = False
    except:
        raise
    if not _TestResult:
        sys.stdout.write("linreg_d_basic                   FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST filters_d_sma
    #      SMA(k) filter
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # Here we demonstrate SMA(k) filtering for time series.
            #
            x = [5,6,7,8]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)

            #
            # Apply filter.
            # We should get [5, 5.5, 6.5, 7.5] as result
            #
            xalglib.filtersma(x, 2)
            _TestResult = _TestResult and doc_print_test(x, [5,5.5,6.5,7.5], "real_vector", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("filters_d_sma                    FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST filters_d_ema
    #      EMA(alpha) filter
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # Here we demonstrate EMA(0.5) filtering for time series.
            #
            x = [5,6,7,8]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)

            #
            # Apply filter.
            # We should get [5, 5.5, 6.25, 7.125] as result
            #
            xalglib.filterema(x, 0.5)
            _TestResult = _TestResult and doc_print_test(x, [5,5.5,6.25,7.125], "real_vector", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("filters_d_ema                    FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST filters_d_lrma
    #      LRMA(k) filter
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # Here we demonstrate LRMA(3) filtering for time series.
            #
            x = [7,8,8,9,12,12]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)

            #
            # Apply filter.
            # We should get [7.0000, 8.0000, 8.1667, 8.8333, 11.6667, 12.5000] as result
            #    
            xalglib.filterlrma(x, 3)
            _TestResult = _TestResult and doc_print_test(x, [7.0000,8.0000,8.1667,8.8333,11.6667,12.5000], "real_vector", 0.00005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("filters_d_lrma                   FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST ssa_d_basic
    #      Simple SSA analysis demo
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # Here we demonstrate SSA trend/noise separation for some toy problem:
            # small monotonically growing series X are analyzed with 3-tick window
            # and "top-K" version of SSA, which selects K largest singular vectors
            # for analysis, with K=1.
            #
            x = [0,0.5,1,1,1.5,2]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)

            #
            # First, we create SSA model, set its properties and add dataset.
            #
            # We use window with width=3 and configure model to use direct SSA
            # algorithm - one which runs exact O(N*W^2) analysis - to extract
            # one top singular vector. Well, it is toy problem :)
            #
            # NOTE: SSA model may store and analyze more than one sequence
            #       (say, different sequences may correspond to data collected
            #       from different devices)
            #
            s = xalglib.ssacreate()
            xalglib.ssasetwindow(s, 3)
            xalglib.ssaaddsequence(s, x)
            xalglib.ssasetalgotopkdirect(s, 1)

            #
            # Now we begin analysis. Internally SSA model stores everything it needs:
            # data, settings, solvers and so on. Right after first call to analysis-
            # related function it will analyze dataset, build basis and perform analysis.
            #
            # Subsequent calls to analysis functions will reuse previously computed
            # basis, unless you invalidate it by changing model settings (or dataset).
            #
            trend, noise = xalglib.ssaanalyzesequence(s, x)
            _TestResult = _TestResult and doc_print_test(trend, [0.3815,0.5582,0.7810,1.0794,1.5041,2.0105], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("ssa_d_basic                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST ssa_d_forecast
    #      Simple SSA forecasting demo
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # Here we demonstrate SSA forecasting on some toy problem with clearly
            # visible linear trend and small amount of noise.
            #
            x = [0.05,0.96,2.04,3.11,3.97,5.03,5.98,7.02,8.02]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x)

            #
            # First, we create SSA model, set its properties and add dataset.
            #
            # We use window with width=3 and configure model to use direct SSA
            # algorithm - one which runs exact O(N*W^2) analysis - to extract
            # two top singular vectors. Well, it is toy problem :)
            #
            # NOTE: SSA model may store and analyze more than one sequence
            #       (say, different sequences may correspond to data collected
            #       from different devices)
            #
            s = xalglib.ssacreate()
            xalglib.ssasetwindow(s, 3)
            xalglib.ssaaddsequence(s, x)
            xalglib.ssasetalgotopkdirect(s, 2)

            #
            # Now we begin analysis. Internally SSA model stores everything it needs:
            # data, settings, solvers and so on. Right after first call to analysis-
            # related function it will analyze dataset, build basis and perform analysis.
            #
            # Subsequent calls to analysis functions will reuse previously computed
            # basis, unless you invalidate it by changing model settings (or dataset).
            #
            # In this example we show how to use ssaforecastlast() function, which
            # predicts changed in the last sequence of the dataset. If you want to
            # perform prediction for some other sequence, use ssaforecastsequence().
            #
            trend = xalglib.ssaforecastlast(s, 3)

            #
            # Well, we expected it to be [9,10,11]. There exists some difference,
            # which can be explained by the artificial noise in the dataset.
            #
            _TestResult = _TestResult and doc_print_test(trend, [9.0005,9.9322,10.8051], "real_vector", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("ssa_d_forecast                   FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST ssa_d_realtime
    #      Real-time SSA algorithm with fast incremental updates
    #
    _TestResult = True
    for _spoil_scenario in range(-1,9):
        try:
            #
            # Suppose that you have a constant stream of incoming data, and you want
            # to regularly perform singular spectral analysis of this stream.
            #
            # One full run of direct algorithm costs O(N*Width^2) operations, so
            # the more points you have, the more it costs to rebuild basis from
            # scratch.
            # 
            # Luckily we have incremental SSA algorithm which can perform quick
            # updates of already computed basis in O(K*Width^2) ops, where K
            # is a number of singular vectors extracted. Usually it is orders of
            # magnitude faster than full update of the basis.
            #
            # In this example we start from some initial dataset x0. Then we
            # start appending elements one by one to the end of the last sequence.
            #
            # NOTE: direct algorithm also supports incremental updates, but
            #       with O(Width^3) cost. Typically K<<Width, so specialized
            #       incremental algorithm is still faster.
            #
            x0 = [0.009,0.976,1.999,2.984,3.977,5.002]
            if _spoil_scenario==0:
                spoil_vec_by_nan(x0)
            if _spoil_scenario==1:
                spoil_vec_by_posinf(x0)
            if _spoil_scenario==2:
                spoil_vec_by_neginf(x0)
            s1 = xalglib.ssacreate()
            xalglib.ssasetwindow(s1, 3)
            xalglib.ssaaddsequence(s1, x0)

            # set algorithm to the real-time version of top-K, K=2
            xalglib.ssasetalgotopkrealtime(s1, 2)

            # one more interesting feature of the incremental algorithm is "power-up" cycle.
            # even with incremental algorithm initial basis calculation costs O(N*Width^2) ops.
            # if such startup cost is too high for your real-time app, then you may divide
            # initial basis calculation across several model updates. It results in better
            # latency at the price of somewhat lesser precision during first few updates.
            xalglib.ssasetpoweruplength(s1, 3)

            # now, after we prepared everything, start to add incoming points one by one;
            # in the real life, of course, we will perform some work between subsequent update
            # (analyze something, predict, and so on).
            #
            # After each append we perform one iteration of the real-time solver. Usually
            # one iteration is more than enough to update basis. If you have REALLY tight
            # performance constraints, you may specify fractional amount of iterations,
            # which means that iteration is performed with required probability.
            updateits = 1.0
            if _spoil_scenario==3:
                updateits = float("nan")
            if _spoil_scenario==4:
                updateits = float("+inf")
            if _spoil_scenario==5:
                updateits = float("-inf")
            xalglib.ssaappendpointandupdate(s1, 5.951, updateits)
            a1, sv1, w, k = xalglib.ssagetbasis(s1)

            xalglib.ssaappendpointandupdate(s1, 7.074, updateits)
            a1, sv1, w, k = xalglib.ssagetbasis(s1)

            xalglib.ssaappendpointandupdate(s1, 7.925, updateits)
            a1, sv1, w, k = xalglib.ssagetbasis(s1)

            xalglib.ssaappendpointandupdate(s1, 8.992, updateits)
            a1, sv1, w, k = xalglib.ssagetbasis(s1)

            xalglib.ssaappendpointandupdate(s1, 9.942, updateits)
            a1, sv1, w, k = xalglib.ssagetbasis(s1)

            xalglib.ssaappendpointandupdate(s1, 11.051, updateits)
            a1, sv1, w, k = xalglib.ssagetbasis(s1)

            xalglib.ssaappendpointandupdate(s1, 11.965, updateits)
            a1, sv1, w, k = xalglib.ssagetbasis(s1)

            xalglib.ssaappendpointandupdate(s1, 13.047, updateits)
            a1, sv1, w, k = xalglib.ssagetbasis(s1)

            xalglib.ssaappendpointandupdate(s1, 13.970, updateits)
            a1, sv1, w, k = xalglib.ssagetbasis(s1)

            # Ok, we have our basis in a1[] and singular values at sv1[].
            # But is it good enough? Let's print it.
            _TestResult = _TestResult and doc_print_test(a1, [[0.510607,0.753611],[0.575201,0.058445],[0.639081,-0.654717]], "real_matrix", 0.0005)

            # Ok, two vectors with 3 components each.
            # But how to understand that is it really good basis?
            # Let's compare it with direct SSA algorithm on the entire sequence.
            x2 = [0.009,0.976,1.999,2.984,3.977,5.002,5.951,7.074,7.925,8.992,9.942,11.051,11.965,13.047,13.970]
            if _spoil_scenario==6:
                spoil_vec_by_nan(x2)
            if _spoil_scenario==7:
                spoil_vec_by_posinf(x2)
            if _spoil_scenario==8:
                spoil_vec_by_neginf(x2)
            s2 = xalglib.ssacreate()
            xalglib.ssasetwindow(s2, 3)
            xalglib.ssaaddsequence(s2, x2)
            xalglib.ssasetalgotopkdirect(s2, 2)
            a2, sv2, w, k = xalglib.ssagetbasis(s2)

            # it is exactly the same as one calculated with incremental approach!
            _TestResult = _TestResult and doc_print_test(a2, [[0.510607,0.753611],[0.575201,0.058445],[0.639081,-0.654717]], "real_matrix", 0.0005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("ssa_d_realtime                   FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST mcpd_simple1
    #      Simple unconstrained MCPD model (no entry/exit states)
    #
    sys.stdout.write("150/162\n")
    _TestResult = True
    for _spoil_scenario in range(-1,6):
        try:
            #
            # The very simple MCPD example
            #
            # We have a loan portfolio. Our loans can be in one of two states:
            # * normal loans ("good" ones)
            # * past due loans ("bad" ones)
            #
            # We assume that:
            # * loans can transition from any state to any other state. In 
            #   particular, past due loan can become "good" one at any moment 
            #   with same (fixed) probability. Not realistic, but it is toy example :)
            # * portfolio size does not change over time
            #
            # Thus, we have following model
            #     state_new = P*state_old
            # where
            #         ( p00  p01 )
            #     P = (          )
            #         ( p10  p11 )
            #
            # We want to model transitions between these two states using MCPD
            # approach (Markov Chains for Proportional/Population Data), i.e.
            # to restore hidden transition matrix P using actual portfolio data.
            # We have:
            # * poportional data, i.e. proportion of loans in the normal and past 
            #   due states (not portfolio size measured in some currency, although 
            #   it is possible to work with population data too)
            # * two tracks, i.e. two sequences which describe portfolio
            #   evolution from two different starting states: [1,0] (all loans 
            #   are "good") and [0.8,0.2] (only 80% of portfolio is in the "good"
            #   state)
            #
            track0 = [[1.00000,0.00000],[0.95000,0.05000],[0.92750,0.07250],[0.91738,0.08263],[0.91282,0.08718]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(track0)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(track0)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(track0)
            track1 = [[0.80000,0.20000],[0.86000,0.14000],[0.88700,0.11300],[0.89915,0.10085]]
            if _spoil_scenario==3:
                spoil_mat_by_nan(track1)
            if _spoil_scenario==4:
                spoil_mat_by_posinf(track1)
            if _spoil_scenario==5:
                spoil_mat_by_neginf(track1)

            s = xalglib.mcpdcreate(2)
            xalglib.mcpdaddtrack(s, track0)
            xalglib.mcpdaddtrack(s, track1)
            xalglib.mcpdsolve(s)
            p, rep = xalglib.mcpdresults(s)

            #
            # Hidden matrix P is equal to
            #         ( 0.95  0.50 )
            #         (            )
            #         ( 0.05  0.50 )
            # which means that "good" loans can become "bad" with 5% probability, 
            # while "bad" loans will return to good state with 50% probability.
            #
            _TestResult = _TestResult and doc_print_test(p, [[0.95,0.50],[0.05,0.50]], "real_matrix", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("mcpd_simple1                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST mcpd_simple2
    #      Simple MCPD model (no entry/exit states) with equality constraints
    #
    _TestResult = True
    for _spoil_scenario in range(-1,6):
        try:
            #
            # Simple MCPD example
            #
            # We have a loan portfolio. Our loans can be in one of three states:
            # * normal loans
            # * past due loans
            # * charged off loans
            #
            # We assume that:
            # * normal loan can stay normal or become past due (but not charged off)
            # * past due loan can stay past due, become normal or charged off
            # * charged off loan will stay charged off for the rest of eternity
            # * portfolio size does not change over time
            # Not realistic, but it is toy example :)
            #
            # Thus, we have following model
            #     state_new = P*state_old
            # where
            #         ( p00  p01    )
            #     P = ( p10  p11    )
            #         (      p21  1 )
            # i.e. four elements of P are known a priori.
            #
            # Although it is possible (given enough data) to In order to enforce 
            # this property we set equality constraints on these elements.
            #
            # We want to model transitions between these two states using MCPD
            # approach (Markov Chains for Proportional/Population Data), i.e.
            # to restore hidden transition matrix P using actual portfolio data.
            # We have:
            # * poportional data, i.e. proportion of loans in the current and past 
            #   due states (not portfolio size measured in some currency, although 
            #   it is possible to work with population data too)
            # * two tracks, i.e. two sequences which describe portfolio
            #   evolution from two different starting states: [1,0,0] (all loans 
            #   are "good") and [0.8,0.2,0.0] (only 80% of portfolio is in the "good"
            #   state)
            #
            track0 = [[1.000000,0.000000,0.000000],[0.950000,0.050000,0.000000],[0.927500,0.060000,0.012500],[0.911125,0.061375,0.027500],[0.896256,0.060900,0.042844]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(track0)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(track0)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(track0)
            track1 = [[0.800000,0.200000,0.000000],[0.860000,0.090000,0.050000],[0.862000,0.065500,0.072500],[0.851650,0.059475,0.088875],[0.838805,0.057451,0.103744]]
            if _spoil_scenario==3:
                spoil_mat_by_nan(track1)
            if _spoil_scenario==4:
                spoil_mat_by_posinf(track1)
            if _spoil_scenario==5:
                spoil_mat_by_neginf(track1)

            s = xalglib.mcpdcreate(3)
            xalglib.mcpdaddtrack(s, track0)
            xalglib.mcpdaddtrack(s, track1)
            xalglib.mcpdaddec(s, 0, 2, 0.0)
            xalglib.mcpdaddec(s, 1, 2, 0.0)
            xalglib.mcpdaddec(s, 2, 2, 1.0)
            xalglib.mcpdaddec(s, 2, 0, 0.0)
            xalglib.mcpdsolve(s)
            p, rep = xalglib.mcpdresults(s)

            #
            # Hidden matrix P is equal to
            #         ( 0.95 0.50      )
            #         ( 0.05 0.25      )
            #         (      0.25 1.00 ) 
            # which means that "good" loans can become past due with 5% probability, 
            # while past due loans will become charged off with 25% probability or
            # return back to normal state with 50% probability.
            #
            _TestResult = _TestResult and doc_print_test(p, [[0.95,0.50,0.00],[0.05,0.25,0.00],[0.00,0.25,1.00]], "real_matrix", 0.005)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("mcpd_simple2                     FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST knn_cls
    #      Simple classification with KNN model
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # The very simple classification example: classify points (x,y) in 2D space
            # as ones with x>=0 and ones with x<0 (y is ignored, but our classifier
            # has to find out it).
            #
            # First, we have to create KNN builder object, load dataset and specify
            # training settings. Our dataset is specified as matrix, which has following
            # format:
            #
            #     x0 y0 class0
            #     x1 y1 class1
            #     x2 y2 class2
            #     ....
            #
            # Here xi and yi can be any values (and in fact you can have any number of
            # independent variables), and classi MUST be integer number in [0,NClasses)
            # range. In our example we denote points with x>=0 as class #0, and
            # ones with negative xi as class #1.
            #
            # NOTE: if you want to solve regression problem, specify dataset in similar
            #       format, but with dependent variable(s) instead of class labels. You
            #       can have dataset with multiple dependent variables, by the way!
            #
            # For the sake of simplicity, our example includes only 4-point dataset and
            # really simple K=1 nearest neighbor search. Industrial problems typically
            # need larger values of K.
            #
            nvars = 2
            nclasses = 2
            npoints = 4
            xy = [[1,1,0],[1,-1,0],[-1,1,1],[-1,-1,1]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)

            builder = xalglib.knnbuildercreate()
            xalglib.knnbuildersetdatasetcls(builder, xy, npoints, nvars, nclasses)

            # we build KNN model with k=1 and eps=0 (exact k-nn search is performed)
            k = 1
            eps = 0
            model, rep = xalglib.knnbuilderbuildknnmodel(builder, k, eps)

            # with such settings (k=1 is used) you can expect zero classification
            # error on training set. Beautiful results, but remember - in real life
            # you do not need zero TRAINING SET error, you need good generalization.

            _TestResult = _TestResult and doc_print_test(rep.relclserror, 0.0000, "real", 0.00005)

            # now, let's perform some simple processing with knnprocess()
            x = [+1,0]
            y = []
            y = xalglib.knnprocess(model, x, y)
            _TestResult = _TestResult and doc_print_test(y, [+1,0], "real_vector", 0.0005)

            # another option is to use knnprocess0() which returns just first component
            # of the output vector y. ideal for regression problems and binary classifiers.
            y0 = xalglib.knnprocess0(model, x)
            _TestResult = _TestResult and doc_print_test(y0, 1.000, "real", 0.0005)

            # finally, you can use knnclassify() which returns most probable class index (i.e. argmax y[i]).
            i = xalglib.knnclassify(model, x)
            _TestResult = _TestResult and doc_print_test(i, 0, "int")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("knn_cls                          FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST knn_reg
    #      Simple classification with KNN model
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # The very simple regression example: model f(x,y)=x+y
            #
            # First, we have to create KNN builder object, load dataset and specify
            # training settings. Our dataset is specified as matrix, which has following
            # format:
            #
            #     x0 y0 f0
            #     x1 y1 f1
            #     x2 y2 f2
            #     ....
            #
            # Here xi and yi can be any values, and fi is a dependent function value.
            # By the way, with KNN algorithm you can even model functions with multiple
            # dependent variables!
            #
            # NOTE: you can also solve classification problems with KNN models, see
            #       another example for this unit.
            #
            # For the sake of simplicity, our example includes only 4-point dataset and
            # really simple K=1 nearest neighbor search. Industrial problems typically
            # need larger values of K.
            #
            nvars = 2
            nout = 1
            npoints = 4
            xy = [[1,1,+2],[1,-1,0],[-1,1,0],[-1,-1,-2]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)

            builder = xalglib.knnbuildercreate()
            xalglib.knnbuildersetdatasetreg(builder, xy, npoints, nvars, nout)

            # we build KNN model with k=1 and eps=0 (exact k-nn search is performed)
            k = 1
            eps = 0
            model, rep = xalglib.knnbuilderbuildknnmodel(builder, k, eps)

            # with such settings (k=1 is used) you can expect zero RMS error on the
            # training set. Beautiful results, but remember - in real life you do not
            # need zero TRAINING SET error, you need good generalization.

            _TestResult = _TestResult and doc_print_test(rep.rmserror, 0.0000, "real", 0.00005)

            # now, let's perform some simple processing with knnprocess()
            x = [+1,+1]
            y = []
            y = xalglib.knnprocess(model, x, y)
            _TestResult = _TestResult and doc_print_test(y, [+2], "real_vector", 0.0005)

            # another option is to use knnprocess0() which returns just first component
            # of the output vector y. ideal for regression problems and binary classifiers.
            y0 = xalglib.knnprocess0(model, x)
            _TestResult = _TestResult and doc_print_test(y0, 2.000, "real", 0.0005)

            # there also exist another convenience function, knnclassify(),
            # but it does not work for regression problems - it always returns -1.
            i = xalglib.knnclassify(model, x)
            _TestResult = _TestResult and doc_print_test(i, -1, "int")
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("knn_reg                          FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST nn_regr
    #      Regression problem with one output (2=>1)
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # The very simple example on neural network: network is trained to reproduce
            # small 2x2 multiplication table.
            #
            # NOTE: we use network with excessive amount of neurons, which guarantees
            #       almost exact reproduction of the training set. Generalization ability
            #       of such network is rather low, but we are not concerned with such
            #       questions in this basic demo.
            #

            #
            # Training set:
            # * one row corresponds to one record A*B=C in the multiplication table
            # * first two columns store A and B, last column stores C
            #
            # [1 * 1 = 1]
            # [1 * 2 = 2]
            # [2 * 1 = 2]
            # [2 * 2 = 4]
            #
            xy = [[1,1,1],[1,2,2],[2,1,2],[2,2,4]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)

            #
            # Network is created.
            # Trainer object is created.
            # Dataset is attached to trainer object.
            #
            trn = xalglib.mlpcreatetrainer(2, 1)
            network = xalglib.mlpcreate1(2, 5, 1)
            xalglib.mlpsetdataset(trn, xy, 4)

            #
            # Network is trained with 5 restarts from random positions
            #
            rep = xalglib.mlptrainnetwork(trn, network, 5)

            #
            # 2*2=?
            #
            x = [2,2]
            y = [0]
            y = xalglib.mlpprocess(network, x, y)
            _TestResult = _TestResult and doc_print_test(y, [4.000], "real_vector", 0.05)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("nn_regr                          FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST nn_regr_n
    #      Regression problem with multiple outputs (2=>2)
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # Network with 2 inputs and 2 outputs is trained to reproduce vector function:
            #     (x0,x1) => (x0+x1, x0*x1)
            #
            # Informally speaking, we want neural network to simultaneously calculate
            # both sum of two numbers and their product.
            #
            # NOTE: we use network with excessive amount of neurons, which guarantees
            #       almost exact reproduction of the training set. Generalization ability
            #       of such network is rather low, but we are not concerned with such
            #       questions in this basic demo.
            #

            #
            # Training set. One row corresponds to one record [A,B,A+B,A*B].
            #
            # [ 1   1  1+1  1*1 ]
            # [ 1   2  1+2  1*2 ]
            # [ 2   1  2+1  2*1 ]
            # [ 2   2  2+2  2*2 ]
            #
            xy = [[1,1,2,1],[1,2,3,2],[2,1,3,2],[2,2,4,4]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)

            #
            # Network is created.
            # Trainer object is created.
            # Dataset is attached to trainer object.
            #
            trn = xalglib.mlpcreatetrainer(2, 2)
            network = xalglib.mlpcreate1(2, 5, 2)
            xalglib.mlpsetdataset(trn, xy, 4)

            #
            # Network is trained with 5 restarts from random positions
            #
            rep = xalglib.mlptrainnetwork(trn, network, 5)

            #
            # 2+1=?
            # 2*1=?
            #
            x = [2,1]
            y = [0,0]
            y = xalglib.mlpprocess(network, x, y)
            _TestResult = _TestResult and doc_print_test(y, [3.000,2.000], "real_vector", 0.05)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("nn_regr_n                        FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST nn_cls2
    #      Binary classification problem
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # Suppose that we want to classify numbers as positive (class 0) and negative
            # (class 1). We have training set which includes several strictly positive
            # or negative numbers - and zero.
            #
            # The problem is that we are not sure how to classify zero, so from time to
            # time we mark it as positive or negative (with equal probability). Other
            # numbers are marked in pure deterministic setting. How will neural network
            # cope with such classification task?
            #
            # NOTE: we use network with excessive amount of neurons, which guarantees
            #       almost exact reproduction of the training set. Generalization ability
            #       of such network is rather low, but we are not concerned with such
            #       questions in this basic demo.
            #
            x = [0]
            y = [0,0]

            #
            # Training set. One row corresponds to one record [A => class(A)].
            #
            # Classes are denoted by numbers from 0 to 1, where 0 corresponds to positive
            # numbers and 1 to negative numbers.
            #
            # [ +1  0]
            # [ +2  0]
            # [ -1  1]
            # [ -2  1]
            # [  0  0]   !! sometimes we classify 0 as positive, sometimes as negative
            # [  0  1]   !!
            #
            xy = [[+1,0],[+2,0],[-1,1],[-2,1],[0,0],[0,1]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)

            #
            #
            # When we solve classification problems, everything is slightly different from
            # the regression ones:
            #
            # 1. Network is created. Because we solve classification problem, we use
            #    mlpcreatec1() function instead of mlpcreate1(). This function creates
            #    classifier network with SOFTMAX-normalized outputs. This network returns
            #    vector of class membership probabilities which are normalized to be
            #    non-negative and sum to 1.0
            #
            # 2. We use mlpcreatetrainercls() function instead of mlpcreatetrainer() to
            #    create trainer object. Trainer object process dataset and neural network
            #    slightly differently to account for specifics of the classification
            #    problems.
            #
            # 3. Dataset is attached to trainer object. Note that dataset format is slightly
            #    different from one used for regression.
            #
            trn = xalglib.mlpcreatetrainercls(1, 2)
            network = xalglib.mlpcreatec1(1, 5, 2)
            xalglib.mlpsetdataset(trn, xy, 6)

            #
            # Network is trained with 5 restarts from random positions
            #
            rep = xalglib.mlptrainnetwork(trn, network, 5)

            #
            # Test our neural network on strictly positive and strictly negative numbers.
            #
            # IMPORTANT! Classifier network returns class membership probabilities instead
            # of class indexes. Network returns two values (probabilities) instead of one
            # (class index).
            #
            # Thus, for +1 we expect to get [P0,P1] = [1,0], where P0 is probability that
            # number is positive (belongs to class 0), and P1 is probability that number
            # is negative (belongs to class 1).
            #
            # For -1 we expect to get [P0,P1] = [0,1]
            #
            # Following properties are guaranteed by network architecture:
            # * P0>=0, P1>=0   non-negativity
            # * P0+P1=1        normalization
            #
            x = [1]
            y = xalglib.mlpprocess(network, x, y)
            _TestResult = _TestResult and doc_print_test(y, [1.000,0.000], "real_vector", 0.05)
            x = [-1]
            y = xalglib.mlpprocess(network, x, y)
            _TestResult = _TestResult and doc_print_test(y, [0.000,1.000], "real_vector", 0.05)

            #
            # But what our network will return for 0, which is between classes 0 and 1?
            #
            # In our dataset it has two different marks assigned (class 0 AND class 1).
            # So network will return something average between class 0 and class 1:
            #     0 => [0.5, 0.5]
            #
            x = [0]
            y = xalglib.mlpprocess(network, x, y)
            _TestResult = _TestResult and doc_print_test(y, [0.500,0.500], "real_vector", 0.05)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("nn_cls2                          FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST nn_cls3
    #      Multiclass classification problem
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # Suppose that we want to classify numbers as positive (class 0) and negative
            # (class 1). We also have one more class for zero (class 2).
            #
            # NOTE: we use network with excessive amount of neurons, which guarantees
            #       almost exact reproduction of the training set. Generalization ability
            #       of such network is rather low, but we are not concerned with such
            #       questions in this basic demo.
            #
            x = [0]
            y = [0,0,0]

            #
            # Training set. One row corresponds to one record [A => class(A)].
            #
            # Classes are denoted by numbers from 0 to 2, where 0 corresponds to positive
            # numbers, 1 to negative numbers, 2 to zero
            #
            # [ +1  0]
            # [ +2  0]
            # [ -1  1]
            # [ -2  1]
            # [  0  2]
            #
            xy = [[+1,0],[+2,0],[-1,1],[-2,1],[0,2]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)

            #
            #
            # When we solve classification problems, everything is slightly different from
            # the regression ones:
            #
            # 1. Network is created. Because we solve classification problem, we use
            #    mlpcreatec1() function instead of mlpcreate1(). This function creates
            #    classifier network with SOFTMAX-normalized outputs. This network returns
            #    vector of class membership probabilities which are normalized to be
            #    non-negative and sum to 1.0
            #
            # 2. We use mlpcreatetrainercls() function instead of mlpcreatetrainer() to
            #    create trainer object. Trainer object process dataset and neural network
            #    slightly differently to account for specifics of the classification
            #    problems.
            #
            # 3. Dataset is attached to trainer object. Note that dataset format is slightly
            #    different from one used for regression.
            #
            trn = xalglib.mlpcreatetrainercls(1, 3)
            network = xalglib.mlpcreatec1(1, 5, 3)
            xalglib.mlpsetdataset(trn, xy, 5)

            #
            # Network is trained with 5 restarts from random positions
            #
            rep = xalglib.mlptrainnetwork(trn, network, 5)

            #
            # Test our neural network on strictly positive and strictly negative numbers.
            #
            # IMPORTANT! Classifier network returns class membership probabilities instead
            # of class indexes. Network returns three values (probabilities) instead of one
            # (class index).
            #
            # Thus, for +1 we expect to get [P0,P1,P2] = [1,0,0],
            # for -1 we expect to get [P0,P1,P2] = [0,1,0],
            # and for 0 we will get [P0,P1,P2] = [0,0,1].
            #
            # Following properties are guaranteed by network architecture:
            # * P0>=0, P1>=0, P2>=0    non-negativity
            # * P0+P1+P2=1             normalization
            #
            x = [1]
            y = xalglib.mlpprocess(network, x, y)
            _TestResult = _TestResult and doc_print_test(y, [1.000,0.000,0.000], "real_vector", 0.05)
            x = [-1]
            y = xalglib.mlpprocess(network, x, y)
            _TestResult = _TestResult and doc_print_test(y, [0.000,1.000,0.000], "real_vector", 0.05)
            x = [0]
            y = xalglib.mlpprocess(network, x, y)
            _TestResult = _TestResult and doc_print_test(y, [0.000,0.000,1.000], "real_vector", 0.05)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("nn_cls3                          FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST nn_trainerobject
    #      Advanced example on trainer object
    #
    _TestResult = True
    for _spoil_scenario in range(-1,6):
        try:
            #
            # Trainer object is used to train network. It stores dataset, training settings,
            # and other information which is NOT part of neural network. You should use
            # trainer object as follows:
            # (1) you create trainer object and specify task type (classification/regression)
            #     and number of inputs/outputs
            # (2) you add dataset to the trainer object
            # (3) you may change training settings (stopping criteria or weight decay)
            # (4) finally, you may train one or more networks
            #
            # You may interleave stages 2...4 and repeat them many times. Trainer object
            # remembers its internal state and can be used several times after its creation
            # and initialization.
            #

            #
            # Stage 1: object creation.
            #
            # We have to specify number of inputs and outputs. Trainer object can be used
            # only for problems with same number of inputs/outputs as was specified during
            # its creation.
            #
            # In case you want to train SOFTMAX-normalized network which solves classification
            # problems,  you  must  use  another  function  to  create  trainer  object:
            # mlpcreatetrainercls().
            #
            # Below we create trainer object which can be used to train regression networks
            # with 2 inputs and 1 output.
            #
            trn = xalglib.mlpcreatetrainer(2, 1)

            #
            # Stage 2: specification of the training set
            #
            # By default trainer object stores empty dataset. So to solve your non-empty problem
            # you have to set dataset by passing to trainer dense or sparse matrix.
            #
            # One row of the matrix corresponds to one record A*B=C in the multiplication table.
            # First two columns store A and B, last column stores C
            #
            #     [1 * 1 = 1]   [ 1 1 1 ]
            #     [1 * 2 = 2]   [ 1 2 2 ]
            #     [2 * 1 = 2] = [ 2 1 2 ]
            #     [2 * 2 = 4]   [ 2 2 4 ]
            #
            xy = [[1,1,1],[1,2,2],[2,1,2],[2,2,4]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)
            xalglib.mlpsetdataset(trn, xy, 4)

            #
            # Stage 3: modification of the training parameters.
            #
            # You may modify parameters like weights decay or stopping criteria:
            # * we set moderate weight decay
            # * we choose iterations limit as stopping condition (another condition - step size -
            #   is zero, which means than this condition is not active)
            #
            wstep = 0.000
            if _spoil_scenario==3:
                wstep = float("nan")
            if _spoil_scenario==4:
                wstep = float("+inf")
            if _spoil_scenario==5:
                wstep = float("-inf")
            maxits = 100
            xalglib.mlpsetdecay(trn, 0.01)
            xalglib.mlpsetcond(trn, wstep, maxits)

            #
            # Stage 4: training.
            #
            # We will train several networks with different architecture using same trainer object.
            # We may change training parameters or even dataset, so different networks are trained
            # differently. But in this simple example we will train all networks with same settings.
            #
            # We create and train three networks:
            # * network 1 has 2x1 architecture     (2 inputs, no hidden neurons, 1 output)
            # * network 2 has 2x5x1 architecture   (2 inputs, 5 hidden neurons, 1 output)
            # * network 3 has 2x5x5x1 architecture (2 inputs, two hidden layers, 1 output)
            #
            # NOTE: these networks solve regression problems. For classification problems you
            #       should use mlpcreatec0/c1/c2 to create neural networks which have SOFTMAX-
            #       normalized outputs.
            #

            net1 = xalglib.mlpcreate0(2, 1)
            net2 = xalglib.mlpcreate1(2, 5, 1)
            net3 = xalglib.mlpcreate2(2, 5, 5, 1)

            rep = xalglib.mlptrainnetwork(trn, net1, 5)
            rep = xalglib.mlptrainnetwork(trn, net2, 5)
            rep = xalglib.mlptrainnetwork(trn, net3, 5)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("nn_trainerobject                 FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST nn_crossvalidation
    #      Cross-validation
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # This example shows how to perform cross-validation with ALGLIB
            #

            #
            # Training set: f(x)=1/(x^2+1)
            # One row corresponds to one record [x,f(x)]
            #
            xy = [[-2.0,0.2],[-1.6,0.3],[-1.3,0.4],[-1,0.5],[-0.6,0.7],[-0.3,0.9],[0,1],[2.0,0.2],[1.6,0.3],[1.3,0.4],[1,0.5],[0.6,0.7],[0.3,0.9]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)

            #
            # Trainer object is created.
            # Dataset is attached to trainer object.
            #
            # NOTE: it is not good idea to perform cross-validation on sample
            #       as small as ours (13 examples). It is done for demonstration
            #       purposes only. Generalization error estimates won't be
            #       precise enough for practical purposes.
            #
            trn = xalglib.mlpcreatetrainer(1, 1)
            xalglib.mlpsetdataset(trn, xy, 13)

            #
            # The key property of the cross-validation is that it estimates
            # generalization properties of neural ARCHITECTURE. It does NOT
            # estimates generalization error of some specific network which
            # is passed to the k-fold CV routine.
            #
            # In our example we create 1x4x1 neural network and pass it to
            # CV routine without training it. Original state of the network
            # is not used for cross-validation - each round is restarted from
            # random initial state. Only geometry of network matters.
            #
            # We perform 5 restarts from different random positions for each
            # of the 10 cross-validation rounds.
            #
            network = xalglib.mlpcreate1(1, 4, 1)
            rep = xalglib.mlpkfoldcv(trn, network, 5, 10)

            #
            # Cross-validation routine stores estimates of the generalization
            # error to MLP report structure. You may examine its fields and
            # see estimates of different errors (RMS, CE, Avg).
            #
            # Because cross-validation is non-deterministic, in our manual we
            # can not say what values will be stored to rep after call to
            # mlpkfoldcv(). Every CV round will return slightly different
            # estimates.
            #
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("nn_crossvalidation               FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST nn_ensembles_es
    #      Early stopping ensembles
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # This example shows how to train early stopping ensebles.
            #

            #
            # Training set: f(x)=1/(x^2+1)
            # One row corresponds to one record [x,f(x)]
            #
            xy = [[-2.0,0.2],[-1.6,0.3],[-1.3,0.4],[-1,0.5],[-0.6,0.7],[-0.3,0.9],[0,1],[2.0,0.2],[1.6,0.3],[1.3,0.4],[1,0.5],[0.6,0.7],[0.3,0.9]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)

            #
            # Trainer object is created.
            # Dataset is attached to trainer object.
            #
            # NOTE: it is not good idea to use early stopping ensemble on sample
            #       as small as ours (13 examples). It is done for demonstration
            #       purposes only. Ensemble training algorithm won't find good
            #       solution on such small sample.
            #
            trn = xalglib.mlpcreatetrainer(1, 1)
            xalglib.mlpsetdataset(trn, xy, 13)

            #
            # Ensemble is created and trained. Each of 50 network is trained
            # with 5 restarts.
            #
            ensemble = xalglib.mlpecreate1(1, 4, 1, 50)
            rep = xalglib.mlptrainensemblees(trn, ensemble, 5)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("nn_ensembles_es                  FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    #
    # TEST nn_parallel
    #      Parallel training
    #
    _TestResult = True
    for _spoil_scenario in range(-1,3):
        try:
            #
            # This example shows how to use parallel functionality of ALGLIB.
            # We generate simple 1-dimensional regression problem and show how
            # to use parallel training, parallel cross-validation, parallel
            # training of neural ensembles.
            #
            # We assume that you already know how to use ALGLIB in serial mode
            # and concentrate on its parallel capabilities.
            #
            # NOTE: it is not good idea to use parallel features on sample as small
            #       as ours (13 examples). It is done only for demonstration purposes.
            #
            xy = [[-2.0,0.2],[-1.6,0.3],[-1.3,0.4],[-1,0.5],[-0.6,0.7],[-0.3,0.9],[0,1],[2.0,0.2],[1.6,0.3],[1.3,0.4],[1,0.5],[0.6,0.7],[0.3,0.9]]
            if _spoil_scenario==0:
                spoil_mat_by_nan(xy)
            if _spoil_scenario==1:
                spoil_mat_by_posinf(xy)
            if _spoil_scenario==2:
                spoil_mat_by_neginf(xy)
            trn = xalglib.mlpcreatetrainer(1, 1)
            xalglib.mlpsetdataset(trn, xy, 13)
            network = xalglib.mlpcreate1(1, 4, 1)
            ensemble = xalglib.mlpecreate1(1, 4, 1, 50)

            #
            # Below we demonstrate how to perform:
            # * parallel training of individual networks
            # * parallel cross-validation
            # * parallel training of neural ensembles
            #
            # In order to use multithreading, you have to:
            # 1) Install SMP edition of ALGLIB.
            # 2) This step is specific for C++ users: you should activate OS-specific
            #    capabilities of ALGLIB by defining AE_OS=AE_POSIX (for *nix systems)
            #    or AE_OS=AE_WINDOWS (for Windows systems).
            #    C# users do not have to perform this step because C# programs are
            #    portable across different systems without OS-specific tuning.
            # 3) Tell ALGLIB that you want it to use multithreading by means of
            #    setnworkers() call:
            #          * alglib::setnworkers(0)  = use all cores
            #          * alglib::setnworkers(-1) = leave one core unused
            #          * alglib::setnworkers(-2) = leave two cores unused
            #          * alglib::setnworkers(+2) = use 2 cores (even if you have more)
            #    During runtime ALGLIB will automatically determine whether it is
            #    feasible to start worker threads and split your task between cores.
            #
            xalglib.setnworkers(+2)

            #
            # First, we perform parallel training of individual network with 5
            # restarts from random positions. These 5 rounds of  training  are
            # executed in parallel manner,  with  best  network  chosen  after
            # training.
            #
            # ALGLIB can use additional way to speed up computations -  divide
            # dataset   into   smaller   subsets   and   process these subsets
            # simultaneously. It allows us  to  efficiently  parallelize  even
            # single training round. This operation is performed automatically
            # for large datasets, but our toy dataset is too small.
            #
            rep = xalglib.mlptrainnetwork(trn, network, 5)

            #
            # Then, we perform parallel 10-fold cross-validation, with 5 random
            # restarts per each CV round. I.e., 5*10=50  networks  are trained
            # in total. All these operations can be parallelized.
            #
            # NOTE: again, ALGLIB can parallelize  calculation   of   gradient
            #       over entire dataset - but our dataset is too small.
            #
            rep = xalglib.mlpkfoldcv(trn, network, 5, 10)

            #
            # Finally, we train early stopping ensemble of 50 neural networks,
            # each  of them is trained with 5 random restarts. I.e.,  5*50=250
            # networks aretrained in total.
            #
            rep = xalglib.mlptrainensemblees(trn, ensemble, 5)
            _TestResult = _TestResult and (_spoil_scenario==-1)
        except (RuntimeError, ValueError):
            _TestResult = _TestResult and (_spoil_scenario!=-1)
        except:
            raise
    if not _TestResult:
        sys.stdout.write("nn_parallel                      FAILED\n")
        sys.stdout.flush()
    _TotalResult = _TotalResult and _TestResult


    sys.stdout.write("162/162\n")
except Exception as e:
    sys.stdout.write("Unhandled exception was raised!\n")
    sys.stdout.write("MESSAGE: ")
    sys.stdout.write(str(e))
    sys.stdout.write("\n")
    traceback.print_exc()
    sys.exit(1)
if _TotalResult:
    sys.exit(0)
else:
    sys.exit(1)
