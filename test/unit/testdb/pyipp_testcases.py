#!/usr/bin/env python
#
# pyipp unit test cases
#
import sys, os, getopt, re
from pyipp import ipp
import unittest

__IPPLOGVERBOSE__= 0

###############################################################################
# Logical test cases
#
class Logical(unittest.TestCase):
    def setUp(self):
        self.pattern= r'^(.*)$'
        self.flags= [ipp.M, ipp.S, ipp.I, ipp.X, ipp.G,
                ipp.M|ipp.S, ipp.M|ipp.I, ipp.M|ipp.X,
                ipp.M|ipp.G, ipp.S|ipp.I, ipp.S|ipp.X,
                ipp.S|ipp.G, ipp.I|ipp.X, ipp.I|ipp.G,
                ipp.X|ipp.G, ipp.M|ipp.S|ipp.I, ipp.M|ipp.S|ipp.X,
                ipp.M|ipp.S|ipp.G, ipp.M|ipp.I|ipp.X,
                ipp.M|ipp.I|ipp.G, ipp.M|ipp.X|ipp.G,
                ipp.M|ipp.S|ipp.I|ipp.X, ipp.M|ipp.S|ipp.I|ipp.G,
                ipp.M|ipp.S|ipp.I|ipp.X|ipp.G
                ]
        self.error= 0
        self.regexdb= '/test.unit/regexdb/'
        self.dbfiles= ['basic.dat', 'categorize.dat',
                'forcedassoc.dat', 'leftassoc.dat', 'nullsubexpr.dat',
                'repetition.dat', 'rightassoc.dat']

    def test_Compileable(self):
        self.error= 0
        try:
            re= ipp.compile(self.pattern)
        except Exception as e:
            print 'ERR='+str(e)
        self.assertEqual(re.ippstatus, 0)

    def test_CompileableWithAllFlags(self):
        for flag in self.flags:
            try:
                re= ipp.compile(self.pattern, flag)
            except Exception as e:
                self.error= 1
            if __IPPLOGVERBOSE__:
                print 'OK -> RET='+str(re.ippstatus)+\
                    '\tFLAGS='+str(flag)
        self.assertEqual(self.error, 0)

    def test_DetectableCpuType(self):
        retval= ipp.printCpuType()
        self.assertTrue(retval.__class__.__name__ == 'str')

    def test_BasicRegExpTests(self):
        self.error= 0
        _dirname= os.path.dirname(os.getcwd())
        print "" # haai: due to better visualisation! leave it untouched
        for dbfile in self.dbfiles:
            idx= 0
            error=0
            filename= _dirname+self.regexdb+dbfile
            f= open(filename, "r")
            lines= f.readlines()
            for line in lines:
                idx+= 1
                contents= line.split('\t')
                # TODO(haai): flag forwarding (:
                try:
                    re= ipp.search(contents[1], contents[2])
                    if re['ippstatus'] == 0 and __IPPLOGVERBOSE__ and \
                            re['numfind'] != 0:
                        print 'FOUND -> RET='+str(re['ippstatus'])+\
                            '\tIDX='+str(idx)+\
                            '\tRDB='+dbfile+\
                            '\tPAT='+contents[1]+\
                            '\tSRC='+contents[2]+\
                            '\tFIND='+str(re['numfind'])
                    if re['ippstatus'] == 0 and __IPPLOGVERBOSE__ and \
                            re['numfind'] ==0:
                        print '_NOT_ FOUND -> RET='+str(re['ippstatus'])+\
                            '\tIDX='+str(idx)+\
                            '\tRDB='+dbfile+\
                            '\tPAT='+contents[1]+\
                            '\tSRC='+contents[2]
                except Exception as e:
                    self.error= 1
                    print 'ERR='+str(e)+\
                            '\tIDX='+str(idx)+\
                            '\tRDB='+dbfile+\
                            '\tPAT='+contents[1]+\
                            '\tSRC='+contents[2]

        self.assertEqual(self.error, 0)


def createLogicalSuite():
    """returns the logical test suite"""
    tests= ['test_CompileableWithAllFlags',
            'test_DetectableCpuType',
            'test_BasicRegExpTests',
            'test_Compileable']
    return unittest.TestSuite(map(Logical, tests))

###############################################################################
# Infrastructural test cases
#
class Infrastructural(unittest.TestCase):
    def test_TestBehaviour(self):
        self.assertEqual(1,1)

def createInfrastructuralSuite():
    """returns the Infrastructural test suite"""
    tests= ['test_TestBehaviour']
    return unittest.TestSuite(map(Infrastructural, tests))


###############################################################################
# returns all test suites
#
def getAllTestSuites():
    retval= []
    retval.append(createLogicalSuite())
    retval.append(createInfrastructuralSuite())
    return retval

