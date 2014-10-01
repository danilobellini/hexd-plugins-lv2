#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Created on 2014-10-01 00:56:57 BRT
# License is GPLv3, see COPYING.txt for more details.
# @author: Danilo de Jesus da Silva Bellini
"""
Simple polynomial interpolator with derivative constraints for the control
inputs.

This script uses the Sympy CAS for all symbolic computation (e.g. finding the
polynomial coefficients).

Resulting polynomial should:

- include (0, old)
- include (m, new)
- have zero derivative at t = 0
- have zero derivative at t = m

Where ``m = n - 1`` is the last time index (time given in "samples").
"""

from __future__ import print_function
from sympy import init_printing, symbols, Eq, pprint, solve, Symbol

# Initializization
init_printing(use_unicode=True)
a, b, c, d, t, m, old, delta = symbols("a b c d t m old delta")

print(__doc__)
print("Also, let's say that:")
pprint(Eq(delta, Symbol("new") - old))
print("\n")

# Polynomial definition
poly = a * t**3 + b * t**2 + c * t + d
print("The polynomial is:")
pprint(poly)
print("\n")

# Finds the coeffs
diff = poly.diff(t)

system = [Eq(poly.subs(t, 0), old),
          Eq(poly.subs(t, m), delta + old), # as delta = new - old
          diff.subs(t, 0),
          diff.subs(t, m)]

solution = solve(system, [a, b, c, d])

print("Solution:")
pprint(solution)
print("\n")

print("For a given value of t, the resulting gain is therefore:")
poly_s = (poly.subs(solution).collect(delta))
pprint(poly_s - poly_s.coeff(delta) * delta
              + poly_s.coeff(delta).factor() * delta)
