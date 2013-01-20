/**
 * \file	_ippch.c
 * \brief	String Processing Bindings for Intel IPP
 * \author	Christian Staffa
 *
 * Copyright (c) 2012, Christian Staffa <www.haai.de>
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

#define M_MASK  (1<<0)  /**< \def MULTIPLE flag mask */
#define S_MASK  (1<<1)  /**< \def SINGLE flag mask */
#define I_MASK  (1<<2)  /**< \def IGNORECASE flag mask */
#define X_MASK  (1<<3)  /**< \def VERBOSE flag mask */
#define G_MASK  (1<<4)  /**< \def GLOBAL flag mask */

static PyObject *IppchError;
static PyTypeObject IppRegExpStateObject_Type;

/**
 * \brief	IppRegExpStateObject
 */
typedef struct {
    PyObject_HEAD
    IppRegExpState *ires;           /**< Ipp Regexp State */
	IppRegExpMultiState *irems;		/**< Ipp Regexp Multi State */
    PyObject *attr_dict;            /**< attribute dictionary */
} IppRegExpStateObject;

/**
 * \brief	IppRegExpMultiStateObject
 */
typedef struct {
    PyObject_HEAD
	IppRegExpMultiState *irems;		/**< Ipp Regexp Multi State */
    PyObject *attr_dict;            /**< attribute dictionary */
} IppRegExpMultiStateObject;

/**
 * \brief	IppRegExpStateObject dealloc function
 */
static void
_dealloc_IppRegExpStateObject(PyObject *self)
{
    IppRegExpStateObject *o= (IppRegExpStateObject*)self;
    if (o->ires != (void*)0xcbcbcbcb && o->ires != NULL)
        ippsRegExpFree(o->ires);
	if (o->irems != (void*)0xcbcbcbcb && o->irems != NULL)
		ippsRegExpMultiFree(o->irems);
    Py_XDECREF(o->attr_dict);

    o->ob_type->tp_free((PyObject*)o);
}

/**
 * \brief   convert flags to Ipp char string
 * \return  Ipp options string
 *
 * significace  |128 | 64 | 32 || 16 |  8 |  4 |  2 |  1 |
 *              +----+----+----++----+----+----+----+----+
 * char(byte)   |XXXX|XXXX|XXXX||    |    |    |    |    |
 *              ---------------++----+----+----+----+----+
 * M_MASK       ---------------||  0 |  0 |  0 |  0 |  1 |
 * S_MASK       ---------------||  0 |  0 |  0 |  1 |  0 |
 * I_MASK       ---------------||  0 |  0 |  1 |  0 |  0 |
 * S_MASK       ---------------||  0 |  1 |  0 |  0 |  0 |
 * G_MASK       ---------------||  1 |  0 |  0 |  0 |  0 |
 */
char *
_getIppOptString(PyObject *flags, char *opts)
{
    int iflags= 0;
	
    if (!PyInt_Check(flags)) {
        PyErr_SetString(PyExc_TypeError, "wrong argument type");
        goto error;
    }
	iflags= PyInt_AsLong(flags);
	if (PyErr_Occurred())
		return NULL;

	if (iflags & M_MASK) strncat(opts, "m", 1);
    if (iflags & S_MASK) strncat(opts, "s", 1);
    if (iflags & I_MASK) strncat(opts, "i", 1);
    if (iflags & X_MASK) strncat(opts, "x", 1);
    if (iflags & G_MASK) strncat(opts, "g", 1);
    return opts;
error:
	return NULL;
}

/**
 * \brief	alloc function to create IppRegExpStateObject objects
 * \return	allocated IppRegExpStateObject
 */
static PyObject *
alloc_IppRegExpStateObject(PyTypeObject *type, Py_ssize_t nitems)
{
    PyObject *self;

    self= PyMem_Malloc(type->tp_basicsize);
    self= PyObject_Init(self, type);
	if (!self)
		return self;
	else
		return NULL;
}

/**
 * \brief	new function to create IppRegExpStateObject objects
 * \return	newley allocated and created IppRegExpStateObject
 *//*
static PyObject *
new_IppRegExpStateObject(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    IppRegExpStateObject *self;

    self= (IppRegExpStateObject *)type->tp_alloc(type,0);
    return (PyObject*)self;
}
*/

/**
 * \brief	init function to IppRegExpStateObject objects
 * \return	int
 */
int
init_IppRegExpStateObject(PyObject *self, PyObject *args, PyObject *kwds)
{
    IppRegExpStateObject *o= (IppRegExpStateObject *)self;

    if (o) {
        o->ires= NULL;
		o->irems= NULL;
        o->attr_dict= NULL;
    }	
    return 0;
}

/**
 * \brief	create a new IppRegExpStateObject object based on pattern pattern (:
 * \return	new IppRegExpStateObject of Type IppRegExpStateObject_Type
 */
static PyObject *
_create_IppRegExpStateObject(PyObject *pattern, PyObject *flags)
{
    char *pat, *p;
	char opts[6]= "\0";
    Py_ssize_t pat_len;
    int ieos= 0, numCaptGroups= 0, i= 0, statesize= 0;
    IppStatus istatus;
    PyObject *value, *groupindex;
	IppRegExpStateObject *ireso;

    ireso= PyObject_New(IppRegExpStateObject, &IppRegExpStateObject_Type);
    if (ireso == NULL)
        goto error;
    /*FIXME(haai): this should do the init function but it doesnt !?!*/
    ireso->ires= NULL;
    ireso->irems= NULL;
	ireso->attr_dict= NULL;
    if (!PyString_Check(pattern)) {
        PyErr_SetString(PyExc_TypeError, "wrong argument type!");
        goto error;
    }
    if (!PyInt_Check(flags)) {
        PyErr_SetString(PyExc_TypeError, "wrong argument type!");
        goto error;
    }

    PyString_AsStringAndSize(pattern, &pat, &pat_len);
	ippsRegExpGetSize(pat, &statesize);
	_getIppOptString(flags, opts);
    istatus= ippsRegExpInitAlloc(pat, NULL, &(ireso->ires), &ieos);
    if (istatus != ippStsNoErr) {
		value= Py_BuildValue("sisi",
                "ippstatus", istatus,
                "eoffset", ieos);
        PyErr_SetObject(IppchError, value);
        goto error;
    }
    p= pat;
	while (i++ < pat_len && *p) {
		if (*p == '(')
			numCaptGroups++; p++;
	}
	groupindex= PyDict_New();
	if (!groupindex) {
		goto error;
	}
	ireso->attr_dict= Py_BuildValue("{sisisNsOsi}",
			"statesize", statesize,
			"groups", numCaptGroups,
			"groupindex", groupindex,
			"pattern", pattern,
            "ippstatus", istatus
            );
	if (ireso->attr_dict == NULL) {
		Py_DECREF(groupindex); 
		goto error;
	}

	return (PyObject *)ireso;
error:
    Py_XDECREF(ireso);
    return NULL;
}

/**
 * \brief	create a new IppRegExpStateMultiObject
 * \return	new IppRegExpStateMultiObject of Type IppRegExpStateObject_Type
 */
static PyObject *
_create_IppRegExpMultiStateObject(PyObject *patterns, PyObject *flags)
{
    char *pat, *p;
	char opts[6]= "\0";
    Py_ssize_t pat_len;
    int ieos= 0, numCaptGroups= 0, i= 0, j= 0, statesize= 0, ss= 0;
    int numpatterns;
    IppStatus istatus;
    PyObject *value, *tmpobj, *patternlist= NULL;
	IppRegExpStateObject *ireso;

    ireso= PyObject_New(IppRegExpStateObject, &IppRegExpStateObject_Type);
    if (ireso == NULL)
        goto error;
    ireso->ires= NULL;
    ireso->irems= NULL;
    ireso->attr_dict= NULL;
    if (!PyList_Check(patterns)) {
        PyErr_SetString(PyExc_TypeError, "wrong argument type!");
        goto error;
    }
    if (!PyInt_Check(flags)) {
        PyErr_SetString(PyExc_TypeError, "wrong argument type!");
        goto error;
    }
    numpatterns= PyList_Size(patterns);
	istatus= ippsRegExpMultiInitAlloc(&(ireso->irems), (Ipp32u)numpatterns);
	if (istatus != ippStsNoErr)
		goto error;		/* mem ireso->irems ! */
	ireso->attr_dict= PyDict_New(); /* mem attr_dict ! */
	if (ireso->attr_dict == NULL) {
		goto error;
	}
    patternlist= PyList_New(numpatterns);
	if (patternlist == NULL)
		goto error;
	ippsRegExpMultiGetSize(numpatterns, &ss);
	statesize+= ss;
	_getIppOptString(flags, opts);
	for (i= 0; i < numpatterns; ++i) {
		tmpobj= PyList_GetItem(patterns, i);
        Py_INCREF(tmpobj);
		PyString_AsStringAndSize(tmpobj, &pat, &pat_len);
		ippsRegExpGetSize(pat, &ss);
		statesize+= ss;
		istatus= ippsRegExpInitAlloc(pat, opts, &(ireso->ires), &ieos);
		if (istatus != ippStsNoErr) {
			value= Py_BuildValue("sisisi",
					"ippstatus", istatus,
					"idxerrpattern", i,
					"eoffset", ieos);
			PyErr_SetObject(IppchError, value);
			goto free;
		}
		istatus= ippsRegExpMultiAdd(ireso->ires, (i+1), ireso->irems);
        if (istatus != ippStsNoErr) {
		    value= Py_BuildValue("si",
                    "ippstatus", istatus);
            PyErr_SetObject(IppchError, value);
            goto free;
        }
    	p= pat;
        numCaptGroups= 0;
        j= 0;
		while (j++ < pat_len && *p) {
			if (*p == '(')
			numCaptGroups++; p++;
		}
		value= PyDict_New();
		PyDict_SetItemString(value, "pattern", tmpobj);
		PyDict_SetItemString(value, "groups", \
			Py_BuildValue("i", numCaptGroups));
		PyList_SetItem(patternlist, i, value);
	}
	PyDict_SetItemString(ireso->attr_dict, "patterns", patternlist);
	PyDict_SetItemString(ireso->attr_dict, "numpatterns", \
            Py_BuildValue("i", numpatterns));
	PyDict_SetItemString(ireso->attr_dict, "statesize", \
			Py_BuildValue("i", statesize));
	PyDict_SetItemString(ireso->attr_dict, "ippstatus", \
			Py_BuildValue("i", istatus));

	return (PyObject*)ireso;
free:
	Py_XDECREF(patternlist);
error:
	Py_XDECREF(ireso->attr_dict);
    Py_XDECREF(ireso);
    return NULL;
}

/**
 * \brief	IppRegExpStateObject method returning state size
 * \return	size of the IppRegExpState! (not IppRegExpStateObject)
 */
static PyObject *
get_state_size(PyObject *self, PyObject *args)
{
	PyObject *_size;
	IppRegExpStateObject *o= (IppRegExpStateObject*)self;
	if (o->ires == NULL) {
		return Py_BuildValue("i", 0);
	}
	_size= PyDict_GetItemString(o->attr_dict, "statesize");
	if (_size == NULL)
		return Py_BuildValue("i", 0);
	/* XXX(haai): borrowed reference?!? */
	return _size;
}

/**
 * \brief	search string with given multi regexp database
 * \return	
 */
static PyObject *
searchMulti(PyObject *self, PyObject *args)
{
    PyObject *retval, *result, *source, *value, *tmp, *patterns, *pat;
    char *src;
    Py_ssize_t src_len;
    int iNumFind= 1, numMultiFind, i;
    IppStatus istatus= -1;
	IppRegExpMultiFind *iMultiFind, *p_iremf;
    IppRegExpStateObject *o;
    
    o= (IppRegExpStateObject*)self;
    if  (o->irems == NULL) {
        PyErr_SetString(IppchError, "No IppRegExpMultiState was created.");
        goto error;
    }
    if (!PyArg_ParseTuple(args, "O", &source))
        goto error;
    if (!PyString_Check(source)) {
        PyErr_SetString(PyExc_TypeError, "wrong argument type");
        goto error;
    }
    PyString_AsStringAndSize(source, &src, &src_len);

    tmp= PyDict_GetItemString(o->attr_dict, "numpatterns");
    numMultiFind= PyInt_AsLong(tmp);
    /*!!!*/
	iMultiFind= PyMem_Malloc(sizeof(IppRegExpMultiFind) * numMultiFind);
    if (iMultiFind == NULL) {
        PyErr_SetString(PyExc_MemoryError, "No Mem could be allocated");
        goto error;
    }
    patterns= PyDict_GetItemString(o->attr_dict, "patterns"); /*list*/
    Py_INCREF(patterns);
    for (i= 0; i < numMultiFind; ++i) {
        p_iremf= iMultiFind + i;
        pat= PyList_GetItem(patterns, i); /*dict*/
        tmp= PyDict_GetItemString(pat, "groups");
        iNumFind= PyLong_AsLong(tmp);
        ++iNumFind; /* !important! though groups can be 0 !!! */
        p_iremf->pFind= \
            (IppRegExpFind*)PyMem_Malloc(sizeof(IppRegExpFind) * (iNumFind));
        if (p_iremf->pFind == NULL) {
            Py_DECREF(patterns);
            goto free;
        }
        p_iremf->numMultiFind= iNumFind;
    }
    Py_DECREF(patterns);
    istatus= ippsRegExpMultiFind_8u((const Ipp8u*)src, (int)src_len, \
			iMultiFind, o->irems);
    if (istatus != ippStsNoErr) {
        value= Py_BuildValue("si", "IppRegExpMultiFind: Error Ipp Status", \
                istatus);
        PyErr_SetObject(IppchError, value);
        goto cleanup;
    }
    retval= PyList_New(numMultiFind);
    for (i= 0; i < numMultiFind; ++i) {
        p_iremf= iMultiFind + i;
        if (p_iremf->status == ippStsNoErr) {
            /* No Error and something is found! */
            if (p_iremf->numMultiFind > 0) {
                result= PyDict_New();
                PyDict_SetItemString(result, "numfind", \
                        Py_BuildValue("i", p_iremf->numMultiFind));
                PyList_SetItem(retval, i, \
                        Py_BuildValue("{sisisisO}", \
                        "patternid", p_iremf->regexpID, \
                        "ippstatus", p_iremf->status, \
                        "done", p_iremf->regexpDoneFlag, \
                        "result", result));
            }
            /* No Error but nothing found! */
            else {
                PyList_SetItem(retval, i, \
                        Py_BuildValue("{sisisisO}", \
                        "patternid", p_iremf->regexpID, \
                        "ippstatus", p_iremf->status, \
                        "done", p_iremf->regexpDoneFlag, \
                        "result", Py_None));
            }
        }
        else {
            /* Error occoured! */
            PyList_SetItem(retval, i, \
                    Py_BuildValue("{sisisi}", \
                    "patternid", p_iremf->regexpID, \
                    "ipperror", p_iremf->status, \
                    "done", p_iremf->regexpDoneFlag));
        }
    }
	for (i= 0; i < numMultiFind; ++i) {
		p_iremf= iMultiFind + i;
		if (p_iremf->pFind) PyMem_Free(p_iremf->pFind);
	}
    PyMem_Free(iMultiFind);
    return retval;
cleanup:
	for (i= 0; i < numMultiFind; ++i) {
		p_iremf= iMultiFind + i;
		if (p_iremf->pFind) PyMem_Free(p_iremf->pFind);
	}
free:
    PyMem_Free(iMultiFind);
error:
    return NULL;
}

/**
 * \brief	search string with given regexp
 * \return	
 */
static PyObject *
search(PyObject *self, PyObject *args)
{
    PyObject *retval, *source, *numFind, *value;
    IppStatus istatus= -1;
    char *src;
    Py_ssize_t src_len;
    int iNumFind= 1;
    IppRegExpFind *iFind;
    IppRegExpStateObject *o;
    
    o= (IppRegExpStateObject*)self;
    if  (o->ires == NULL) {
        PyErr_SetString(IppchError, "no IppRegExpState was created.");
        goto error;
    }
    if (!PyArg_ParseTuple(args, "O", &source))
        goto error;
    if (!PyString_Check(source)) {
        PyErr_SetString(PyExc_TypeError, "wrong argument type");
        goto error;
    }
    PyString_AsStringAndSize(source, &src, &src_len);
    numFind= PyDict_GetItemString(o->attr_dict, "groups");
    if (numFind == NULL) {
        PyErr_SetString(PyExc_KeyError, "key 'groups' not found");
        goto error;
    }
    iNumFind+= PyInt_AsLong(numFind);
    if (iNumFind == -1) {
        if (PyErr_Occurred()) {
            goto error;
        }
    }
    iFind= (IppRegExpFind*)PyMem_Malloc(sizeof(IppRegExpFind) * iNumFind);
    if (iFind == NULL) {
        goto error;
    }
    istatus= ippsRegExpFind_8u((const Ipp8u*)src, (int)src_len, \
			o->ires, iFind, &iNumFind);
    if (istatus != ippStsNoErr) {
        value= Py_BuildValue("si", "IppRegExpFind: Error Ipp Status", istatus);
        PyErr_SetObject(IppchError, value);
        goto free;
    }
    PyMem_Free(iFind);
    if (iNumFind > 0) {
        retval= Py_BuildValue("{sisi}", \
                "numfind", iNumFind, \
                "ippstatus", istatus);
        return retval;
    }
    Py_RETURN_NONE;
    /*FIXME(haai): should return none again if no match (temp edited)
    else {
        retval= Py_BuildValue("{sisi}", \
                "found", Py_False,
                "ippstatus", istatus);
        return retval;
    }*/
free:
    PyMem_Free(iFind);
error:
    return NULL;
}

static PyObject *
setMatchLimit(PyObject *self, PyObject *args)
{
    PyObject *value;
    IppStatus istatus;
    unsigned int ilimit;
    IppRegExpStateObject *o;

    if (!PyArg_ParseTuple(args, "I", &ilimit))
        goto error;
    o= (IppRegExpStateObject *)self;
	if (o->ires == NULL) {
		PyErr_SetString(IppchError, "No IppRegExpState compiled");
		goto error;
	}
    istatus= ippsRegExpSetMatchLimit(ilimit, o->ires);
    if (istatus != ippStsNoErr) {
        value= Py_BuildValue("si",
                "ippsRegExpSetMatchLimit: Error Ipp Status:", istatus);
        PyErr_SetObject(PyExc_SystemError, value);
    }
    return Py_BuildValue("i", (int)istatus);
error:
    return NULL;
}

/**
 * \brief	IppRegExpStateObject Methods
 */
static PyMethodDef IppRegExpStateObject_Methods[]= {
	{"getStateSize", get_state_size, METH_VARARGS,
		"Get the actual IppRegExpState size"},
    {"search", search, METH_VARARGS,
        "Looks for occurences of the substring matching the specified regexp"},
	{"searchMulti", searchMulti, METH_VARARGS,
		"Looks for occurences of the substrings matching the specified regexes"},
    {"setMatchLimit", setMatchLimit, METH_VARARGS,
        "Set the value of the Match Stack Limit"},
	{NULL, NULL, 0, NULL} /* Sentinel */
};

/**
 * \brief	IppRegExpStateObject type definition
 */
static PyTypeObject IppRegExpStateObject_Type= {
    PyObject_HEAD_INIT(NULL)
    0,                              /**< ob_size */
    "_ipp.IppRegExpStateObject",    /**< tp_name */
    sizeof(IppRegExpStateObject),   /**< tp_basicsize */
    0,                              /**< tp_itemsize */
    (destructor)_dealloc_IppRegExpStateObject, /**< tp_dealloc */
    0,                              /**< tp_print */
    0,                              /**< tp_getattr */
    0,                              /**< tp_setattr */
    0,                              /**< tp_compare */
    0,                              /**< tp_repr */
    0,                              /**< tp_as_number */
    0,                              /**< tp_as_sequence */
    0,                              /**< tp_as_mapping */
    0,                              /**< tp_hash */
    0,                              /**< tp_call */
    0,                              /**< tp_str */
    0,                              /**< tp_getattro */
    0,                              /**< tp_setattro */
    0,                              /**< tp_as_buffer */
    Py_TPFLAGS_DEFAULT,             /**< tp_flags */
    "IppRegExpStateObject objects", /**< tp_doc */
    0,                              /**< tp_traverse */
    0,                              /**< tp_clear */
    0,                              /**< tp_richcompare */
    0,                              /**< tp_weaklistoffset */
    0,                              /**< tp_iter */
    0,                              /**< tp_iternext */
    IppRegExpStateObject_Methods,   /**< tp_methods */
    0,                              /**< tp_members */
    0,                              /**< tp_getset */
    0,                              /**< tp_base */
    0,                              /**< tp_dict */
    0,                              /**< tp_descr_get */
    0,                              /**< tp_descr_set */
    offsetof(IppRegExpStateObject, attr_dict), /**< tp_dictoffset */
    (initproc)init_IppRegExpStateObject, /**< tp_init */
    (allocfunc)alloc_IppRegExpStateObject, /**< tp_alloc */
    0,                              /**< tp_new */
    0,                              /**< tp_free */
    0,                              /**< tp_is_gc */
    0,                              /**< tp_bases */
    0,                              /**< tp_mro */
    0,                              /**< tp_cache */
    0,                              /**< tp_subclasses */
    0,                              /**< tp_weaklist */
    0,                              /**< tp_del */
    0,                              /**< tp_version_tag */
};

/**
 * \brief	parses arguments and compiles regex state
 * \return	IppRegExpStateObject
 */
static PyObject *
_compile(PyObject *self, PyObject *args)
{
	PyObject *pattern;
    PyObject *flags;

	if (!PyArg_ParseTuple(args, "OO", &pattern, &flags))
		goto error;
    return _create_IppRegExpStateObject(pattern, flags);
error:
	return NULL;
}

/**
 * \brief	parses arguments and compiles regex multi state
 * \return	IppRegExpMultiStateObject
 */
static PyObject *
_compileMulti(PyObject *self, PyObject *args)
{
	PyObject *patterns;
    PyObject *flags;

	if (!PyArg_ParseTuple(args, "OO", &patterns, &flags))
		goto error;
    return _create_IppRegExpMultiStateObject(patterns, flags);
error:
	return NULL;
}

static PyObject *
_setMatchLimit(PyObject *self, PyObject *args)
{
    PyObject *o, *value;
    unsigned int ilimit;
    IppStatus istatus;
    IppRegExpStateObject *ireso;

    if (!PyArg_ParseTuple(args, "OI", &o, &ilimit))
        goto error;
    ireso= (IppRegExpStateObject *)o;
	if (ireso->ires == NULL)
		goto error;
    istatus= ippsRegExpSetMatchLimit(ilimit, ireso->ires);
    if (istatus != ippStsNoErr) {
        value= Py_BuildValue("si",
                "ippsRegExpSetMatchLimit: Error Ipp Status:", istatus);
        PyErr_SetObject(PyExc_SystemError, value);
    	goto error;
	}
    return Py_BuildValue("i", (int)istatus);
error:
	return NULL;
}

/**
 * \brief	holds the methods for the module
 */
static PyMethodDef Module_Methods[]= {
	{"_compile", _compile, METH_VARARGS,
        "Compile a RegExp Pattern to internal Structure"},
	{"_compileMulti", _compileMulti, METH_VARARGS,
		"Compile a Multi RegExp Pattern Structure"},
    {"_setMatchLimit", _setMatchLimit, METH_VARARGS,
        "Set the value of the Match Stack"},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

/**
 * \brief	_ippch extension module init function
 */
PyMODINIT_FUNC
init_ippch(void)
{
	PyObject *m;
    
    IppRegExpStateObject_Type.tp_new= PyType_GenericNew;
	if (PyType_Ready(&IppRegExpStateObject_Type) < 0)
		return;
	m= Py_InitModule("_ippch", Module_Methods);
	if (m == NULL)
		return;
	Py_INCREF(&IppRegExpStateObject_Type);
	PyModule_AddObject(m, "IppRegExpStateObject", 
			(PyObject*)&IppRegExpStateObject_Type);
	IppchError= PyErr_NewException("_ippch.error", NULL, NULL);
	Py_INCREF(IppchError);
	PyModule_AddObject(m, "_IppchError", IppchError);

    ippInit();

	return;
}
