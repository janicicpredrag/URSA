# URSA

URSA (c) 2010

Version 4.0

URSA is a system for Uniform Reduction to SAT: i.e., for solving combinatorial
problem by reducing them to SATisfiability problem.

Author: 

Predrag Janicic, University of Belgrade

URL: http://www.matf.bg.ac.rs/~janicic/

## Licence

URSA is distributed under the GNU Public Licence, see file COPYING for details.

## Executables

The directory 'bin' stores a pre-built executable.

## Building from source code

URSA is written in C/C++ and was successfully built and run under Linux.

To build ursa:

> cd src

> make
	
If the build is successful, the executable output file 'ursa' resides in the src directory.

## Usage 

Usage: 

  ./ursa options

Options:

-l - sets the number of bits that represent numbers (e.g., -l10; the default value is 8)

-d - DIMACS output only

-q - quite mode (models are not printed out)

-s - selects an underlying solvers (e.g., -sargosat, -sclasp; the defaulf is clasp)

Example:

  ./ursa -l10 < examples/CSP/queens.urs

NOTA BENE: If URSA is used with clasp as an underlying SAT solver and if some propositional
variable is irrelevant for the asserted constraint, then its different values are not 
considered within the set of all models (so the set of models may not be as expected).



