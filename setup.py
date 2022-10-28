###########################################################################
# ALGLIB 3.19.0 (source code generated 2022-06-07)
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
    libnames   = ['alglib319_'+str(ctypes.sizeof(ctypes.c_void_p)*8)+'hpc'+'.dll']
else:
    libnames   = ['alglib319_'+str(ctypes.sizeof(ctypes.c_void_p)*8)+'hpc'+'.so']

setup(
    name         =   'xalglib',
    version      =   '3.19.0',
    description  =   'ALGLIB for Python: numerical library',
    author       =   'ALGLIB Project',
    url          =   'http://www.alglib.net/',
    license      =   "ALGLIB Personal and Academic Use License Agreement",
    packages     =   ['xalglib'],
    package_data =   {'xalglib': libnames}
    )
