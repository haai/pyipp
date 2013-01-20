#!/usr/bin/env python

# pyipp - Intel IPP Binding Interfaces
#
# Copyright (c) 2012-2013, Christian Staffa <www.haai.de>
# Copyright (c) 2012, Riverbed Technology, Inc. <www.riverbed.com>

from pyipp.ipps import _ippch

# flags
M= MULTIPLE= 1
S= SINGLE= 2
I= IGNORECASE= 4
X= VERBOSE= 8
G= GLOBAL= 16

def compile(pattern, flags=0):
    """
    Compile a RE pattern string into a regexp object. Flags may be
    concatenated with | (i.e. M|S|X)
    Flags:
    M   Treat string as multiple lines
    S   Treat string as single line (.) match even
    I   Do case-insensitive pattern matching
    X   Extend patterns legibility by permitting whitespace and comments
    G   Global matching
    """
    return _ippch._compile(pattern, flags)

def compileMulti(patternlist, flags=0):
    """
    Compile a RE pattern list in regexp object. Flags may be
    concatenated with | (i.e. M|S|X)
    Flags:
    M   Treat string as multiple lines
    S   Treat string as single line (.) match even
    I   Do case-insensitive pattern matching
    X   Extend patterns legibility by permitting whitespace and comments
    G   Global matching
    """
    return _ippch._compileMulti(patternlist, flags)

def escape(string):
    """Return (a copy of) string with all non-alphanumerics backslashed."""
    pass

def match(string):
    """Return a corresponding match object instance, if 0 or more chars
    at beginning of <string> match the regex pattern string, or None
    if no match."""
    pass

def search(pattern, string, flags=0):
    """Scan through <string> for a location matching <pattern>,
    return a corresponding match object instance, or None if no match."""
    return _ippch._compile(pattern, flags)._search(string)

def split(pattern, string, maxsplit):
    """Split <string> by occurences of <pattern>. If capturing () are
    used in pattern, then occurences of patterns or subpatterns are also
    returned."""
    pass

def findall(pattern, string):
    """Return a list of non-overlapping matches in <pattern>, either a 
    list of groups or a list of tuples if the pattern has more than 1
    group."""
    pass

def sub(pattern, repl, string, count):
    """Return string obtained by replacing the (<count> first) leftmost
    non-overlapping occourrences of <pattern> (a string or a regexp object)
    in <string> by <repl>; <repl> can be a string or a fct called with
    a single match object argument, which must return the replacement
    string."""
    pass

def subn(pattern, repl, string, count):
    """Same as sub(), but returns a tuple (newString, numberOfSubsMade)."""
    pass

