#!/usr/bin/env python

# For further information please check the README
#
# NOTE: A legal Intel license and installed version of the
#       Intel IPP library bundle is needed.
#       Check README for details.
import sys, os, re
from distutils.core import setup, Extension

MAJOR_VERSION=0
MINOR_VERSION=1
BUILD_VERSION=0
DEBUG_IPP=0
EXTRA_COMPILE_ARGS= [
    '-Wall',
    #'-g'
    ]
DEFINE_MACROS= [('DEBUG_IPP', '%i' % DEBUG_IPP),('NDEBUG', '1')]
MODULES=[]

# authors and license information
f= open('LICENSE', 'r')
print f.read()

# process configuration for seperate modules
print "=== searching for registering modules ==="
for f in os.listdir('conf/modules'):
    if re.match(r'^.*\.pyc$', f) or re.match(r'^__init__.py$', f):
        continue
    c= f.split('.')
    m= __import__("conf.modules.%s" % (c[0],),\
            globals(), locals(), ["module"])
    print "imported module %s" % (c[0],), m
    print "registering %s" % m.module
    MODULES.append(m.module)
print "=== registering done ==="

# process common module globals
for m in MODULES:
    m.define_macros=DEFINE_MACROS
    m.extra_compile_args= EXTRA_COMPILE_ARGS

# python distutils setup file
setup(name='pyipp',
        version='%i.%i.%i' % (MAJOR_VERSION, MINOR_VERSION, BUILD_VERSION),
        packages=['pyipp', 'pyipp.ipp', 'pyipp.ipps'],
        package_dir={'': 'src'},
        #py_modules=['pyipp.ipp.ipp', 'pyipp.ipps.ippch'],
        description='pyipp - Intel IPP Python Bindings',
        long_description='''
            Python Bindings for the	Intel(R)
            Integrated Performance Primitives Library (IPP)
            NOTICE: A legal Intel license and installed
            version of Intels IPP library bundle is needed.
            Further information at:
            http://software.intel.com/en-us/articles/intel-ipp/
            ''',
        author='Christian Staffa',
        author_email='info@haai.de',
        url='https://github.com/haai/pyipp.git',
        platforms='linux',
        license='New-BSD',
        ext_modules=MODULES,
        )

