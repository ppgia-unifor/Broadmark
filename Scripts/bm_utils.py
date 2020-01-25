# -*- coding: utf-8 -*-
from __future__ import print_function, unicode_literals, with_statement, division, absolute_import
import collections

def get_iterable(x):
    if isinstance(x, collections.Iterable):
        return x
    else:
        return (x,)