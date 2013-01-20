#
# ipp setup file
# 
from distutils.core import Extension
from conf.ipp_globals import IPPINCLUDE
from conf.ipp_globals import IPPLIBDIR

module= Extension(
        'pyipp.ipps._ipps',
        ['src/lib/ipps/_ipps.c'],
        include_dirs= [IPPINCLUDE, 'src/include'],
        library_dirs= [IPPLIBDIR],
        libraries= ['ipps'],
        )

