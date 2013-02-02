/**
 * \file	_ipp.c
 * \brief	Python Bindings for Intel(R) Integrated Performance Primitives (IPP)
 * \author	Christian Staffa
 *
 * This file contains general functions from the ipp-function domain
 * 
 * Copyright (c) 2012-2013, Christian Staffa <www.haai.de>
 * Copyright (c) 2012, Riverbed Technology, Inc. <www.riverbed.com>
 * See LICENSE file
 */
#include <Python.h>
#include <structmember.h>
#include <ippcore.h>
#include <string.h>

static PyObject *IppError;

/**
 * \brief	translate a cpu type into a string
 */
const char *
_getIppCpuStringType(IppCpuType ct)
{
	switch (ct) {
		case ippCpuPP: return "Intel(R) Pentrium(R) processor"; break;
		case ippCpuPMX: return "Pentium(R) processor with MMX(TM) technology"; break;
		case ippCpuPPR: return "Pentium(R) Pro processor"; break;
		case ippCpuPII: return "Pentium(R) II processor"; break;
		case ippCpuPIII: return "Pentrium(R) III processor and Pentium(R) III Xeon(R) processor"; break;
		case ippCpuP4: return "Pentium(R) 4 processor and Intel(R) Xeon(R) processor"; break;
		case ippCpuP4HT: return "Pentium(R) 4 processor with HT Technology"; break;
		case ippCpuP4HT2: return "Pentium(R) 4 processor with Streaming SIMD Extension"; break;
		case ippCpuCentrino: return "Intel(R) Centrion(TM) mobile technology"; break;
		case ippCpuCoreSolo: return "Intel(R) Core(TM) Solo processor"; break;
		case ippCpuCoreDuo: return "Intel(R) Core(TM) Duo processor"; break;
		case ippCpuITP: return "Intel(R) Itanium(R) processor"; break;
		case ippCpuITP2: return "Intel(R) Intel(R) 2 processor"; break;
		case ippCpuEM64T: return "Intel(R) 64 Instruction Set Architecture (ISA)"; break;
		case ippCpuC2D: return "Intel(R) Core(TM) 2 Duo processor"; break;
		case ippCpuC2Q: return "Intel(R) Core(TM) 2 Quad processor"; break;
		case ippCpuPenryn: return "Intel(R) Core(TM) 2 processor with Intel(R) SSE4.1"; break;
		case ippCpuBonnell: return "Intel(R) Atom(TM) processor"; break;
		case ippCpuNehalem: return "Intel(R) Core(TM) i7 processor"; break;
		case ippCpuNext:
		case ippCpuSSE: return "Processor supports Streaming SIMD Extensions instruciton set"; break;
		case ippCpuSSE2: return "Processor supports Streaming SIMD Extensions 2 instruciton set"; break;
		case ippCpuSSE3: return "Processor supports Streaming SIMD Extensions 3 instruciton set"; break;
		case ippCpuSSSE3: return "Processor supports Supplemental Streaming SIMD Extension 3 instraction set"; break;
		case ippCpuSSE41: return "Processor supports Streaming SIMD Extensions 4.1 instraction set"; break;
		case ippCpuSSE42: return "Processor supports Streaming SIMD Extensions 4.2 instraction set"; break;
		case ippCpuAVX: return "Processor supports Advanced Vector Extensions instraction set"; break;
		case ippCpuAES: return "Processor supports AES New Instructions"; break;
		case ippCpuF16RND: return "Processor supports RDRRAND & Float16 instractions"; break;
		case ippCpuX8664: return "Processor supports 64 bit extension"; break;
		case ippCpuUnknown:
		default: return "Unkown Processor"; break;
	}
	return NULL;
}

/**
 * \brief	Translates a status code into a message
 */
static PyObject *
_getStatusString(PyObject *self, PyObject *args)
{
    const char *iss;
    IppStatus istatus;

    if (!PyArg_ParseTuple(args, "i", &istatus))
        goto error;
    iss= ippGetStatusString(istatus);
    return Py_BuildValue("s", iss);
error:
    return NULL;
}

/**
 * \brief	returns a processor type
 */
static PyObject *
_getCpuType(PyObject *self, PyObject *args)
{
	IppCpuType ct;
	
	ct= ippGetCpuType();
	return Py_BuildValue("i", ct);
}

/**
 * \brief	returns a current value of the time stamp counter (TSC) register
 */
static PyObject *
_getCpuClocks(PyObject *self, PyObject *args)
{
    Ipp64u icc;

    icc= ippGetCpuClocks();
    return Py_BuildValue("K", icc);
}

/**
 * \brief	estimates the processor operating freqency
 */
static PyObject *
_getCpuFreqMhz(PyObject *self, PyObject *args)
{
    int imhz;
    IppStatus istatus;

    istatus= ippGetCpuFreqMhz(&imhz);
    if (istatus != ippStsNoErr)
        goto error;
    return Py_BuildValue("i", imhz);
error:
    return NULL;
}

/**
 * \brief	convenient function for getting string representation
 */
void
_getCpuFeatureFlagString(Ipp64u CpuFeatureFlags, char *buf, int len)
{
	int before= 0;

	memset(buf, '\0', len);
	if (CpuFeatureFlags & ippCPUID_MMX) {
		if (before == 1) {
			strncat(buf, " ", 1); len+= 1;
		}
		strncat(buf, "MMX", sizeof("MMX")); len+= sizeof("MMX"); before= 1;
	}
	if (CpuFeatureFlags & ippCPUID_SSE) {
		if (before == 1) {
			strncat(buf, " ", 1); len+= 1;
		}
		strncat(buf, "SSE", sizeof("SSE")); len+= sizeof("SSE"); before= 1;
	}
	if (CpuFeatureFlags & ippCPUID_SSE2) {
		if (before == 1) {
			strncat(buf, " ", 1); len+= 1;
		}
		strncat(buf, "SSE2", sizeof("SSE2")); len+= sizeof("SSE2"); before=1;
	}
	if (CpuFeatureFlags & ippCPUID_SSE3) {
		if (before == 1) {
			strncat(buf, " ", 1); len+= 1;
		}
		strncat(buf, "SSE3", sizeof("SSE3")); len+= sizeof("SSE3"); before= 1;
	}
	if (CpuFeatureFlags & ippCPUID_SSSE3) {
		if (before == 1) {
			strncat(buf, " ", 1); len+= 1;
		}
		strncat(buf, "SSSE3", sizeof("SSSE3")); len+= sizeof("SSSE3"); before= 1;
	}
	if (CpuFeatureFlags & ippCPUID_MOVBE) {
		if (before == 1) {
			strncat(buf, " ", 1); len+= 1;
		}
		strncat(buf, "MOVBE", sizeof("MOVBE")); len+= sizeof("MOVBE"); before= 1;
	}
	if (CpuFeatureFlags & ippCPUID_SSE41) {
		if (before == 1) {
			strncat(buf, " ", 1); len+= 1;
		}
		strncat(buf, "SSE41", sizeof("SSE41")); len+= sizeof("SSE41"); before= 1;
	}
	if (CpuFeatureFlags & ippCPUID_SSE42) {
		if (before == 1) {
			strncat(buf, " ", 1); len+= 1;
		}
		strncat(buf, "SSE42", sizeof("SSE42")); len+= sizeof("SSE42"); before= 1;
	}
	if (CpuFeatureFlags & ippCPUID_AVX) {
		if (before == 1) {
			strncat(buf, " ", 1); len+= 1;
		}
		strncat(buf, "AVX", sizeof("AVX")); len+= sizeof("AVX"); before= 1;
	}
	if (CpuFeatureFlags & ippAVX_ENABLEDBYOS) {
		if (before == 1) {
			strncat(buf, " ", 1); len+= 1;
		}
		strncat(buf, "AVXOS", sizeof("AVXOS")); len+= sizeof("AVXOS"); before= 1;
	}
	if (CpuFeatureFlags & ippCPUID_AES) {
		if (before == 1) {
			strncat(buf, " ", 1); len+= 1;
		}
		strncat(buf, "AES", sizeof("AES")); len+= sizeof("AES"); before= 1;
	}
	if (CpuFeatureFlags & ippCPUID_CLMUL) {
		if (before == 1) {
			strncat(buf, " ", 1); len+= 1;
		}
		strncat(buf, "CLMUL", sizeof("CLMUL")); len+= sizeof("CLMUL"); before= 1;
	}
	buf[len]= '\0';
	return ;
}

/**
 * \brief	retrieves the processor features
 */
static PyObject *
_getCpuFeatures(PyObject *self, PyObject *args)
{
	Ipp64u fm;
	char CpuFlagString[100];
	IppStatus istatus;

	istatus= ippGetCpuFeatures(&fm, NULL);
	if (istatus != ippStsNoErr)
		goto error;
	_getCpuFeatureFlagString(fm, CpuFlagString, sizeof(CpuFlagString));
	return Py_BuildValue("Ks", fm, CpuFlagString);
error:
	return NULL;
}

/**
 * \brief	returns a features mask for enabled processor features
 * url		
 */
static PyObject *
_getEnabledCpuFeatures(PyObject *self, PyObject *args)
{
	Ipp64u fm;
	char CpuFlagString[100];

	fm= ippGetEnabledCpuFeatures();
	_getCpuFeatureFlagString(fm, CpuFlagString, sizeof(CpuFlagString));
	return Py_BuildValue("Ks", fm, CpuFlagString);
}

/**
 * \brief	returns the number of cores for multi-core processors
 */
static PyObject *
_getNumCoresOnDie(PyObject *self, PyObject *args)
{
    int incod;

    incod= ippGetNumCoresOnDie();
    return Py_BuildValue("i", incod);
}

/**
 * \brief	returns maximum size of the L2 and L3 caches of the processor
 */
static PyObject *
_getMaxCacheSize(PyObject *self, PyObject *args)
{
    int isb;
    IppStatus istatus;

    istatus= ippGetMaxCacheSizeB(&isb);
    if (istatus != ippStsNoErr)
        goto ret;
    return Py_BuildValue("i", isb);
ret:
    return NULL;
}

/**
 * \brief	enables or disables flush-to-zero (FTZ) mode
 * url		
 */
static PyObject *
_setFlushToZero(PyObject *self, PyObject *args)
{
	IppStatus istatus;
	int val;
	unsigned int umask;

	if (!PyArg_ParseTuple(args, "ii", &val, &umask))
		goto error;
    if (umask= 0)
	    istatus= ippSetFlushToZero(val, NULL);
    else
        istatus= ippSetFlushToZero(val, &umask);
    if (istatus != ippStsNoErr)
        goto error;
    if (umask == 0)
	    return Py_BuildValue("ii", val, 0);
    else
        return Py_BuildValue("ii", val, umask);
error:
	return NULL;
}

/**
 * \brief	enables or disables denormals-are-zero (DAZ) mode
 * url		
 */
static PyObject *
_setDenormAreZeros(PyObject *self, PyObject *args)
{
	IppStatus istatus;
	int val;

	if (!PyArg_ParseTuple(args, "i", &val))
		goto error;
	istatus= ippSetDenormAreZeros(val);
	return Py_BuildValue("i", istatus);
error:
	return NULL;
}

/**
 * \brief	sets the number of threads in the multithreading environment
 */
static PyObject *
_setNumThreads(PyObject *self, PyObject *args)
{
    int inthr;
    IppStatus istatus;

	if (!PyArg_ParseTuple(args, "i", &inthr))
		goto error;
	istatus= ippSetNumThreads(inthr);
    if (istatus != ippStsNoErr)
        goto error;
    return Py_BuildValue("i", istatus);
error:
    return NULL;
}

/**
 * \brief	returns the number of existing threads in the multithreading environment
 */
static PyObject *
_getNumThreads(PyObject *self, PyObject *args)
{
    int inthr;
    IppStatus istatus;

    istatus= ippGetNumThreads(&inthr);
    if (istatus != ippStsNoErr)
        goto error;
    return Py_BuildValue("i", inthr);
error:
    return NULL;
}

/**
 * \brief	Binds OpenMP threads to OS processors
 * url
 */
static PyObject *
_setAffinity(PyObject *self, PyObject *args)
{
	IppStatus istatus;
	IppAffinityType iat;
	int offset;

	if (!PyArg_ParseTuple(args, "ii", &iat, &offset))
		goto error;
	istatus= ippSetAffinity(iat, offset);
	if (istatus != ippStsNoErr)
		goto error;
	return Py_BuildValue("i", istatus);
error:
	return NULL;
}

/**
 * \brief	holds the methods for the module
 */
static PyMethodDef Module_Methods[]= {
	{"_getCpuType", _getCpuType, METH_VARARGS,
		"Return a processor type"},
	{"_getCpuFeatures", _getCpuFeatures, METH_VARARGS,
		"Get the CPU Features Flag Field"},
	{"_getEnabledCpuFeatures", _getEnabledCpuFeatures, METH_VARARGS,
		"Get the ENABLED CPU Features Flag Field"},
    {"_getStatusString", _getStatusString, METH_VARARGS,
        "Translates a status code into a Ipp Status message"},
    {"_getCpuClocks", _getCpuClocks, METH_VARARGS,
        "Returns a current value of the time stamp counter TSC register"},
    {"_getCpuFreqMhz", _getCpuFreqMhz, METH_VARARGS,
        "Estimates the processor operating frequecy"},
    {"_getNumCoresOnDie", _getNumCoresOnDie, METH_VARARGS,
        "Returns the number of cores for multi-core processors"},
    {"_getMaxCacheSize", _getMaxCacheSize, METH_VARARGS,
        "Returns maximum size of the L2 and L3 caches of the processor"},
    {"_getNumThreads", _getNumThreads, METH_VARARGS,
        "Returns the number of existing threads in the multithreading environment"},
    {"_setNumThreads", _setNumThreads, METH_VARARGS,
        "Sets the number of threads in the multithreading environment"},
    {"_setAffinity", _setAffinity, METH_VARARGS,
        "Binds OpenMP threads to OS processors"},
    {"_setFlushToZero", _setFlushToZero, METH_VARARGS,
        "Enables or disables flush-to-zero (FTZ) mode"},
    {"_setDenormAreZeros", _setDenormAreZeros, METH_VARARGS,
        "Enables or disables denormals-are-zero (DAZ) mode"},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

/**
 * \brief	_ipp extension module init function
 */
PyMODINIT_FUNC
init_ipp(void)
{
	PyObject *m;
	PyObject *d;
    
	m= Py_InitModule("_ipp", Module_Methods);
	if (!m)
		return;
	d= PyModule_GetDict(m);
    /* add cpu type enums */
	PyDict_SetItemString(d, "ippCpuUnknown", PyInt_FromLong(ippCpuUnknown));
	PyDict_SetItemString(d, "ippCpuPP", PyInt_FromLong(ippCpuPP));
	PyDict_SetItemString(d, "ippCpuPMX", PyInt_FromLong(ippCpuPMX));
	PyDict_SetItemString(d, "ippCpuPPR", PyInt_FromLong(ippCpuPPR));
	PyDict_SetItemString(d, "ippCpuPII", PyInt_FromLong(ippCpuPII));
	PyDict_SetItemString(d, "ippCpuPIII", PyInt_FromLong(ippCpuPIII));
	PyDict_SetItemString(d, "ippCpuP4", PyInt_FromLong(ippCpuP4));
	PyDict_SetItemString(d, "ippCpuP4HT", PyInt_FromLong(ippCpuP4HT));
	PyDict_SetItemString(d, "ippCpuP4HT2", PyInt_FromLong(ippCpuP4HT2));
	PyDict_SetItemString(d, "ippCpuCentrino", PyInt_FromLong(ippCpuCentrino));
	PyDict_SetItemString(d, "ippCpuCoreSolo", PyInt_FromLong(ippCpuCoreSolo));
	PyDict_SetItemString(d, "ippCpuCoreDuo", PyInt_FromLong(ippCpuCoreDuo));
	PyDict_SetItemString(d, "ippCpuITP", PyInt_FromLong(ippCpuITP));
	PyDict_SetItemString(d, "ippCpuITP2", PyInt_FromLong(ippCpuITP2));
	PyDict_SetItemString(d, "ippCpuEM64T", PyInt_FromLong(ippCpuEM64T));
	PyDict_SetItemString(d, "ippCpuC2D", PyInt_FromLong(ippCpuC2D));
	PyDict_SetItemString(d, "ippCpuC2Q", PyInt_FromLong(ippCpuC2Q));
	PyDict_SetItemString(d, "ippCpuPenryn", PyInt_FromLong(ippCpuPenryn));
	PyDict_SetItemString(d, "ippCpuBonnell", PyInt_FromLong(ippCpuBonnell));
	PyDict_SetItemString(d, "ippCpuNehalem", PyInt_FromLong(ippCpuNehalem));
	PyDict_SetItemString(d, "ippCpuNext", PyInt_FromLong(ippCpuNext));
	PyDict_SetItemString(d, "ippCpuSSE", PyInt_FromLong(ippCpuSSE));
	PyDict_SetItemString(d, "ippCpuSSE2", PyInt_FromLong(ippCpuSSE2));
	PyDict_SetItemString(d, "ippCpuSSE3", PyInt_FromLong(ippCpuSSE3));
	PyDict_SetItemString(d, "ippCpuSSSE3", PyInt_FromLong(ippCpuSSSE3));
	PyDict_SetItemString(d, "ippCpuSSE41", PyInt_FromLong(ippCpuSSE41));
	PyDict_SetItemString(d, "ippCpuSSE42", PyInt_FromLong(ippCpuSSE42));
	PyDict_SetItemString(d, "ippCpuAVX", PyInt_FromLong(ippCpuAVX));
	PyDict_SetItemString(d, "ippCpuAES", PyInt_FromLong(ippCpuAES));
	PyDict_SetItemString(d, "ippCpuF16RND", PyInt_FromLong(ippCpuF16RND));
	PyDict_SetItemString(d, "ippCpuX8664", PyInt_FromLong(ippCpuX8664));
	/* add cpuid enums*/
	PyDict_SetItemString(d, "ippCPUID_MMX", PyInt_FromLong(ippCPUID_MMX));
	PyDict_SetItemString(d, "ippCPUID_SSE", PyInt_FromLong(ippCPUID_SSE));
	PyDict_SetItemString(d, "ippCPUID_SSE2", PyInt_FromLong(ippCPUID_SSE2));
	PyDict_SetItemString(d, "ippCPUID_SSE3", PyInt_FromLong(ippCPUID_SSE3));
	PyDict_SetItemString(d, "ippCPUID_SSSE3", PyInt_FromLong(ippCPUID_SSSE3));
	PyDict_SetItemString(d, "ippCPUID_MOVBE", PyInt_FromLong(ippCPUID_MOVBE));
	PyDict_SetItemString(d, "ippCPUID_SSE41", PyInt_FromLong(ippCPUID_SSE41));
	PyDict_SetItemString(d, "ippCPUID_SSE42", PyInt_FromLong(ippCPUID_SSE42));
	PyDict_SetItemString(d, "ippCPUID_AVX", PyInt_FromLong(ippCPUID_AVX));
	PyDict_SetItemString(d, "ippAVX_ENABLEDBYOS", PyInt_FromLong(ippAVX_ENABLEDBYOS));
	PyDict_SetItemString(d, "ippCPUID_AES", PyInt_FromLong(ippCPUID_AES));
	PyDict_SetItemString(d, "ippCPUID_CLMUL", PyInt_FromLong(ippCPUID_CLMUL));
	PyDict_SetItemString(d, "ippCPUID_ABB", PyInt_FromLong(ippCPUID_ABR));
	PyDict_SetItemString(d, "ippCPUID_RDRRAND", PyInt_FromLong(ippCPUID_RDRRAND));
	PyDict_SetItemString(d, "ippCPUID_F16C", PyInt_FromLong(ippCPUID_F16C));
	PyDict_SetItemString(d, "ippCPUID_GETINFO_A", PyLong_FromLongLong(ippCPUID_GETINFO_A));
    /* add affinity enums */
    PyDict_SetItemString(d, "ippAffinityCompactFineCore", PyLong_FromLong(ippAffinityCompactFineCore));
    PyDict_SetItemString(d, "ippAffinityCompactFineHT", PyLong_FromLong(ippAffinityCompactFineHT));
    PyDict_SetItemString(d, "ippAffinityAllEnabled", PyLong_FromLong(ippAffinityAllEnabled));
    PyDict_SetItemString(d, "ippAffinityRestore", PyLong_FromLong(ippAffinityRestore));
    PyDict_SetItemString(d, "ippTstAffinityCompactFineCore", PyLong_FromLong(ippTstAffinityCompactFineCore));
    PyDict_SetItemString(d, "ippTstAffinityCompactFineHT", PyLong_FromLong(ippTstAffinityCompactFineHT));
    /* add exception class */
	IppError= PyErr_NewException("_ipp.error", NULL, NULL);
	Py_INCREF(IppError);
	PyModule_AddObject(m, "_ippError", IppError);

    ippInit();

	return;
}
