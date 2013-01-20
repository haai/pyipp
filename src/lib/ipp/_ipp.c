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
#include <ipp.h>
#include <string.h>
#ifdef DEBUG_IPP
#include <stdio.h>
#include <assert.h>
#endif

static PyObject *IppError;

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
 * \brief	gets and prints the cpu type string
 * \return	cpu identification string
 */
static PyObject *
_cpu_info(PyObject *self, PyObject *args)
{
	IppCpuType ct;
	
	ct= ippGetCpuType();
	return Py_BuildValue("s", _getIppCpuStringType(ct));
}


static PyObject *
_checkString(PyObject *self, PyObject *args)
{
    PyObject *string;
    int i, size;
    const char *s;
    PyArg_ParseTuple(args,"O",&string);
    s= PyString_AsString(string);
#ifdef DEBUG_IPP
    printf("string='%s'\n", s);
#endif
    /* FIXME(haai): strlen is no good on py strings! (py extending faq)*/
    size= strlen(s);
#ifdef DEBUG_IPP
    for (i= 0; i < size; ++i) {
        printf("%c", s[i]);
    }
#endif
    Py_RETURN_NONE;
}

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

static PyObject *
_getCpuFeatures(PyObject *self, PyObject *args)
{
	Ipp64u CpuFeatureFlags;
	char CpuFlagString[100];
	IppStatus istatus;

	istatus= ippGetCpuFeatures(&CpuFeatureFlags, NULL);
	if (istatus != ippStsNoErr) {
		if (istatus == ippStsNotSupportedCpu)
			PyErr_SetString(IppError, "Not Supported Cpu");
		goto error;
	}
	_getCpuFeatureFlagString(CpuFeatureFlags, CpuFlagString, sizeof(CpuFlagString));
	return Py_BuildValue("Ks", CpuFeatureFlags, CpuFlagString);
error:
	return NULL;
}

static PyObject *
_getEnabledCpuFeatures(PyObject *self, PyObject *args)
{
	Ipp64u CpuFeatureFlags;
	char CpuFlagString[100];

	CpuFeatureFlags= ippGetEnabledCpuFeatures();
	_getCpuFeatureFlagString(CpuFeatureFlags, CpuFlagString, sizeof(CpuFlagString));
	return Py_BuildValue("Ks", CpuFeatureFlags, CpuFlagString);
}

static PyObject *
_getLibVersion(PyObject *self, PyObject *args)
{
	const IppLibraryVersion *ilv;

	ilv= ippchGetLibVersion();
	return Py_BuildValue("ss", ilv->Name, ilv->Version);
}

static PyObject *
_getStatusString(PyObject *self, PyObject *args)
{
    const char *iss;
    IppStatus istatus;

    if (!PyArg_ParseTuple(args, "i", &istatus))
        goto ret;
    iss= ippGetStatusString(istatus);
    return Py_BuildValue("s", iss);
ret:
    return NULL;
}

static PyObject *
_getCpuClocks(PyObject *self, PyObject *args)
{
    Ipp64u icc;

    icc= ippGetCpuClocks();
    return Py_BuildValue("K", icc);
}

static PyObject *
_getCpuFreqMhz(PyObject *self, PyObject *args)
{
    int imhz;
    IppStatus istatus;

    istatus= ippGetCpuFreqMhz(&imhz);
    if (istatus != ippStsNoErr)
        goto ret;
    return Py_BuildValue("i", imhz);
ret:
    return NULL;
}

static PyObject *
_getNumCoresOnDie(PyObject *self, PyObject *args)
{
    int incod;

    incod= ippGetNumCoresOnDie();
    return Py_BuildValue("i", incod);
}

static PyObject *
_getMaxCacheSizeB(PyObject *self, PyObject *args)
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

static PyObject *
_getNumThreads(PyObject *self, PyObject *args)
{
    int inthr;
    IppStatus istatus;

    istatus= ippGetNumThreads(&inthr);
    if (istatus != ippStsNoErr)
        goto ret;
    return Py_BuildValue("i", inthr);
ret:
    return NULL;
}


/**
 * \brief	holds the methods for the module
 */
static PyMethodDef Module_Methods[]= {
	{"_cpu_info", _cpu_info, METH_VARARGS,
		"Execute cpu ipp lib function"},
	{"_getCpuFeatures", _getCpuFeatures, METH_VARARGS,
		"Get the CPU Features Flag Field"},
	{"_getEnabledCpuFeatures", _getEnabledCpuFeatures, METH_VARARGS,
		"Get the ENABLED CPU Features Flag Field"},
	{"_getLibVersion", _getLibVersion, METH_VARARGS,
		"Get the Library Version Information"},
    {"_getStatusString", _getStatusString, METH_VARARGS,
        "Translates a status code into a Ipp Status message"},
    {"_getCpuClocks", _getCpuClocks, METH_VARARGS,
        "Returns a current value of the time stamp counter TSC register"},
    {"_getCpuFreqMhz", _getCpuFreqMhz, METH_VARARGS,
        "Estimates the processor operating frequecy"},
    {"_getNumCoresOnDie", _getNumCoresOnDie, METH_VARARGS,
        "Returns the number of cores for multi-core processors"},
    {"_getMaxCacheSizeB", _getMaxCacheSizeB, METH_VARARGS,
        "Returns maximum size of the L2 and L3 caches of the processor"},
    {"_getNumThreads", _getNumThreads, METH_VARARGS,
        "Returns the number of existing threads in the multithreading env."},
    {"_checkString", _checkString, METH_VARARGS,
        "Check String"},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

/**
 * \brief	_ipp extension module init function
 */
PyMODINIT_FUNC
init_ipp(void)
{
	PyObject *m;
    
	m= Py_InitModule("_ipp", Module_Methods);
	if (m == NULL) {
		return;
	}
	IppError= PyErr_NewException("_ipp.error", NULL, NULL);
	Py_INCREF(IppError);
	PyModule_AddObject(m, "_IppError", IppError);

    ippInit();

	return;
}
