# pyipp unit test script
import sys, os, unittest, re

# main function for the pyipp unit test cases
def doTest():
    retval= True
    print "//////[ pyipp unit tests ]//////"
    testsuites= []
    for f in os.listdir('test/unit/testdb'):
        if not re.match(r'^ts_.*\.py$', f):
            continue
        c= f.split('.')
        t= __import__("testdb.%s" % (c[0],),\
                globals(), locals(), ["testsuite"])
        print "imported testsuite %s" % (c[0],)
        testsuites.append(t.testsuite)
    for ts in testsuites:
        v= unittest.TextTestRunner(stream=sys.stdout,verbosity=2).run(ts)
        if v != True:
            retval= False
    print "//////[ pyipp unit test ... done (-; ]//////"
    return retval

if __name__ == '__main__':
    doTest()
