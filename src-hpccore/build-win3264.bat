@echo Building X-core (hpc)

@if not exist "..\vcvars32.bat" ( echo "../vcvars32.bat does not exist" & exit /B 1 )
@if not exist "..\vcvars64.bat" ( echo "../vcvars64.bat does not exist" & exit /B 1 )

@mkdir xcore || goto lbl_failure
@chdir xcore || goto lbl_failure

@copy "..\src\*.*"       . >NUL || goto lbl_failure

@call "..\..\vcvars32" || goto lbl_failure
@rc /fo alglib_info.res alglib_info.rc || goto lbl_failure

@call "..\..\vcvars32"                                                || goto lbl_failure
@cl /nologo /LD /Ox /wd4996 /we4020 /W3 /DX_FOR_WINDOWS /DAE_CPU=AE_INTEL /DAE_OS=AE_WINDOWS /DAE_MKL /DAE_SMP /I"%MKLROOT%/include" /I.            /c /Fokernels32_sse2.obj kernels_sse2.c || goto lbl_failure
@cl /nologo /LD /Ox /wd4996 /we4020 /W3 /DX_FOR_WINDOWS /DAE_CPU=AE_INTEL /DAE_OS=AE_WINDOWS /DAE_MKL /DAE_SMP /I"%MKLROOT%/include" /I. /arch:AVX  /c /Fokernels32_avx2.obj kernels_avx2.c || goto lbl_failure
@cl /nologo /LD /Ox /wd4996 /we4020 /W3 /DX_FOR_WINDOWS /DAE_CPU=AE_INTEL /DAE_OS=AE_WINDOWS /DAE_MKL /DAE_SMP /I"%MKLROOT%/include" /I. /arch:AVX2 /c /Fokernels32_fma.obj  kernels_fma.c  || goto lbl_failure
@call "..\..\vcvars64"                                                || goto lbl_failure
@cl /nologo /LD /Ox /wd4996 /we4020 /W3 /DX_FOR_WINDOWS /DAE_CPU=AE_INTEL /DAE_OS=AE_WINDOWS /DAE_MKL /DAE_SMP /DMKL_ILP64 /I"%MKLROOT%/include" /I.            /c /Fokernels64_sse2.obj kernels_sse2.c || goto lbl_failure
@cl /nologo /LD /Ox /wd4996 /we4020 /W3 /DX_FOR_WINDOWS /DAE_CPU=AE_INTEL /DAE_OS=AE_WINDOWS /DAE_MKL /DAE_SMP /DMKL_ILP64 /I"%MKLROOT%/include" /I. /arch:AVX  /c /Fokernels64_avx2.obj kernels_avx2.c || goto lbl_failure
@cl /nologo /LD /Ox /wd4996 /we4020 /W3 /DX_FOR_WINDOWS /DAE_CPU=AE_INTEL /DAE_OS=AE_WINDOWS /DAE_MKL /DAE_SMP /DMKL_ILP64 /I"%MKLROOT%/include" /I. /arch:AVX2 /c /Fokernels64_fma.obj  kernels_fma.c  || goto lbl_failure
@del /Q kernels_sse2.c                                                  || goto lbl_failure
@del /Q kernels_avx2.c                                                  || goto lbl_failure
@del /Q kernels_fma.c                                                   || goto lbl_failure

@call "..\..\vcvars32" || goto lbl_failure
@call "%mklvars%" ia32    || goto lbl_failure
@cvtres /machine:x86 /out:alglib32_info.obj alglib_info.res || goto lbl_failure
@cl /Fealglib400_32hpc.dll /nologo /LD /Ox /wd4996 /we4020 /W3 /DX_FOR_WINDOWS /DAE_CPU=AE_INTEL /DAE_OS=AE_WINDOWS /DAE_MKL /DAE_SMP /I"%MKLROOT%/include" /I. *.c alglib32_info.obj kernels32_sse2.obj kernels32_avx2.obj kernels32_fma.obj mkl_intel_c.lib mkl_sequential.lib mkl_core.lib /link /LIBPATH:"%MKLROOT%\lib\ia32" || goto lbl_failure
@echo alglib400_32hpc.dll was built
@move alglib400_32hpc.dll .. || goto lbl_failure
@del /Q alglib32_info.obj

@call "..\..\vcvars64" || goto lbl_failure
@call "%mklvars%" intel64 || goto lbl_failure
@cvtres /machine:x64 /out:alglib64_info.obj alglib_info.res >NUL
@cl /Fealglib400_64hpc.dll /nologo /LD /Ox /wd4996 /we4020 /W3 /DX_FOR_WINDOWS /DAE_CPU=AE_INTEL /DAE_OS=AE_WINDOWS /DAE_MKL /DAE_SMP /DMKL_ILP64 /I"%MKLROOT%/include" /I. *.c alglib64_info.obj kernels64_sse2.obj kernels64_avx2.obj kernels64_fma.obj mkl_intel_ilp64.lib mkl_sequential.lib mkl_core.lib /link /LIBPATH:"%MKLROOT%\lib\intel64" || goto lbl_failure
@echo alglib400_64hpc.dll was built
@move alglib400_64hpc.dll .. || goto lbl_failure
@del /Q alglib64_info.obj

@cd ..
@echo DONE
@exit /b 0

:lbl_failure
@echo ERROR: failure to build precompiled library
@exit /b 1
