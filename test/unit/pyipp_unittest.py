#!/usr/bin/env python

# pyipp unit test script
import sys, os, unittest
import testdb.pyipp_testcases as pyipptc

#
# main function for the pyipp unit test cases (and re + re2 module)
#
if __name__ == '__main__':
    print "/"*3+"[ pyipp unit tests ]"+"/"*47
    print "Beginning Test Suites ...\n"
    print "/"*70

    print "/"*3+"pyipp testcases"+"/"*52
    suites= pyipptc.getAllTestSuites()
    for suite in suites:
        unittest.TextTestRunner(stream=sys.stdout,verbosity=2).run(suite)
        print "="*70+""

    print "/"*70
    print "pyipp unit test done ... (-;"

