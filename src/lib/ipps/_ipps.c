/**
 * \file	_ipps.c
 * \brief	Supported Functions for the Signal Processing Libraries
 * \author	Christian Staffa
 *
 * Copyright (c) 2012-2013, Christian Staffa <www.haai.de>
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

static PyObject *IppsError;

static PyObject *
_getLibVersion(PyObject *self, PyObject *args)
{
	const IppLibraryVersion *ilv;

	ilv= ippsGetLibVersion();

    return Py_BuildValue("(sssdddd)",
			ilv->Name,
			ilv->Version,
			ilv->BuildDate,
			ilv->major,
			ilv->minor,
			ilv->majorBuild,
			ilv->build);
}

/**
 * \brief	holds the methods for the module
 */
static PyMethodDef Module_Methods[]= {
	{"_getLibVersion", _getLibVersion, METH_VARARGS,
		"Returns information aout the active version of Intell IPP signal processing software"},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

/**
 * \brief	_ippch extension module init function
 */
PyMODINIT_FUNC
init_ipps(void)
{
	PyObject *m;
    
	m= Py_InitModule("_ipps", Module_Methods);
	if (m == NULL)
		return;
	IppsError= PyErr_NewException("_ipps.error", NULL, NULL);
	Py_INCREF(IppsError);
	PyModule_AddObject(m, "_ippsError", IppsError);

    ippInit();

	return;
}
