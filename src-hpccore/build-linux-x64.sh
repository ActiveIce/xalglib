#!/bin/bash
echo Building X-core - hpc

if [ ! -f "/opt/intel/oneapi/setvars.sh" ]; then
    echo "'/opt/intel/oneapi/setvars.sh' is not found"
    exit 1
fi

source /opt/intel/oneapi/setvars.sh intel64 || { echo "Unable to source compiler variables" ; exit 1 ; }
mkdir tmp                   || { echo "Unable to mkdir tmp" ; exit 1 ; }
cd tmp                      || { echo "Unable to cd tmp" ; exit 1 ; }
cp ../src/*.c .             || { echo "Error copying C files" ; exit 1 ; }
cp ../src/*.h .             || { echo "Error copying C files" ; exit 1 ; }

gcc -c -m64 -O3 -fno-tree-loop-distribute-patterns -fpic -shared -DX_FOR_LINUX -DAE_CPU=AE_INTEL -DAE_OS=AE_LINUX -DMKL_ILP64 -DAE_MKL -DAE_SMP -I{$MKLROOT}/include -I. -msse2       kernels_sse2.c    || { echo "Error compiling SSE2 kernels" ; exit 1 ; }
gcc -c -m64 -O3 -fno-tree-loop-distribute-patterns -fpic -shared -DX_FOR_LINUX -DAE_CPU=AE_INTEL -DAE_OS=AE_LINUX -DMKL_ILP64 -DAE_MKL -DAE_SMP -I{$MKLROOT}/include -I. -mavx2       kernels_avx2.c    || { echo "Error compiling AVX2 kernels" ; exit 1 ; }
gcc -c -m64 -O3 -fno-tree-loop-distribute-patterns -fpic -shared -DX_FOR_LINUX -DAE_CPU=AE_INTEL -DAE_OS=AE_LINUX -DMKL_ILP64 -DAE_MKL -DAE_SMP -I{$MKLROOT}/include -I. -mavx2 -mfma kernels_fma.c     || { echo "Error compiling FMA kernels"  ; exit 1 ; }
rm kernels_sse2.c                                   || { echo "Error cleaning up kernel source files" ; exit 1 ; }
rm kernels_avx2.c                                   || { echo "Error cleaning up kernel source files" ; exit 1 ; }
rm kernels_fma.c                                    || { echo "Error cleaning up kernel source files" ; exit 1 ; }

# compile with exp, pow and log being redirected to our own wrappers which improve GLIBC-related portability
gcc -o alglib401_64hpc.so -m64 -O3 -fno-tree-loop-distribute-patterns -fpic -shared -DX_FOR_LINUX -DAE_CPU=AE_INTEL -DAE_OS=AE_LINUX -DMKL_ILP64 -DAE_MKL -DAE_SMP -I{$MKLROOT}/include -I. *.c kernels_sse2.o kernels_avx2.o kernels_fma.o -Wl,--wrap=exp -Wl,--wrap=pow -Wl,--wrap=log -Wl,--start-group ${MKLROOT}/lib/intel64/libmkl_intel_ilp64.a ${MKLROOT}/lib/intel64/libmkl_sequential.a ${MKLROOT}/lib/intel64/libmkl_core.a -Wl,--end-group -lpthread -lm -ldl || { echo "compilation failure" ; exit 1 ; }
echo alglib401_64hpc.so is created
mv alglib401_64hpc.so ../alglib401_64hpc.so || { echo "Error copying SO file" ; exit 1 ; }
cd ..                       || { echo "Strange error" ; exit 1 ; }
echo DONE
