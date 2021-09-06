/************************************************************************************
URSA -- Copyright (c) 2010, Predrag Janicic

This file is part of URSA
 
URSA is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
 
URSA is WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
**************************************************************************************/

#ifndef SATSOLVER_H_INCLUDED
#define SATSOLVER_H_INCLUDED

using namespace std;


class SATsolver {

public:
  SATsolver() {};
  virtual ~SATsolver() {};

  virtual void InitSolver(unsigned int numVars)=0;
  virtual void addClause(vector<int> &clause)=0;
  virtual bool solve()=0;
  virtual bool isTrueVar(unsigned int var)=0;
};

#endif

