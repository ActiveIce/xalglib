###########################################################################
# ALGLIB 3.18.0 (source code generated 2021-10-25)
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

setup(
    name         =   'xalglib',
    version      =   '3.18.0',
    description  =   'ALGLIB for Python: numerical library',
    author       =   'ALGLIB Project',
    url          =   'http://www.alglib.net/',
    license      =   "ALGLIB Personal and Academic Use License Agreement",
    packages     =   ['xalglib'],
    package_data =   {'xalglib': ['alglib318_32hpc.dll', 'alglib318_64hpc.dll', 'alglib318_64hpc.so']}
    )
