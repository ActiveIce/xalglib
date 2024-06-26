###########################################################################
# ALGLIB 4.02.0 (source code generated 2024-05-31)
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

from setuptools import setup
import os
import sys
import ctypes
import shutil

#
# first, we need to copy shared libraries from core directory
#
if sys.platform=="win32" or sys.platform=="cygwin":
    #
    # we are running under windows
    #
    libnames   = ["alglib402_"+str(ctypes.sizeof(ctypes.c_void_p)*8)+"hpc"+".dll"]
    targetname =  "alglib402_"+str(ctypes.sizeof(ctypes.c_void_p)*8)+"hpc"+".dll"
    dirname    = "bin-windows-intel"
else:
    libnames   = ["alglib402_"+str(ctypes.sizeof(ctypes.c_void_p)*8)+"hpc"+".so"]
    targetname = "alglib402_"+str(ctypes.sizeof(ctypes.c_void_p)*8)+"hpc"+".so"
    dirname    = "bin-linux-intel"
libname = ""
for s in libnames:
    if os.path.exists(os.path.join(dirname,s)):
        libname = s
        break
if libname=="":
    sys.stdout.write("ALGLIB installer: unable to detect ALGLIB shared library\n")
    sys.exit(1)
shutil.copyfile(os.path.join(dirname,libname), targetname)

setup(
    name        =   'xalglib',
    version     =   '4.02.0',
    description =   'ALGLIB for Python: numerical library',
    author      =   'ALGLIB Project',
    url         =   'http://www.alglib.net/',
    license     =   "ALGLIB Personal and Academic Use License Agreement",
    py_modules  =   ['xalglib'],
    data_files  =   [('', [targetname])]
    )
