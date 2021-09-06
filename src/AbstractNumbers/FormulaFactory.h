/************************************************************************************
URSA -- Copyright (c) 2010, Predrag Janicic

This file is part of URSA.
Authors: Milan Sesum and Predrag Janicic
 
URSA is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
 
URSA is WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
**************************************************************************************/

#ifndef __FORMULA_FACTORY_H
#define __FORMULA_FACTORY_H

#include <assert.h>
#include <vector>
#include <set>
#include <unordered_set>
#include "FormulaVector.h"

using namespace std;


////////////////////////////////////////////////////////////////////////////////
////	Class FormulaFactory
////////////////////////////////////////////////////////////////////////////////

class FormulaFactory {

private:

  FormulaFactory() {}

  class equal_to_Formula {
    public:
      bool operator() (const Formula* f1, const Formula* f2) const {
        return (*f1) == (*f2);
      }
  };

  class hash_Formula {
    public:
      size_t operator() (const Formula* f) const {
        return f->hashCode();
      }
  };

  typedef unordered_set<Formula*, hash_Formula, equal_to_Formula> FormulaPointerSet;
  FormulaPointerSet existingFormulas;
  static FormulaFactory *formulaFactory;


public:

  static FormulaFactory* Instance() {
    if (FormulaFactory::formulaFactory == 0)
      FormulaFactory::formulaFactory = new FormulaFactory();
    return FormulaFactory::formulaFactory;
  }

  ~FormulaFactory() {
     Clear();
  }

  void Clear() {
     existingFormulas.clear();
     formulaFactory = 0;
  }

  FormulaPointerSet::iterator begin(void) { return existingFormulas.begin(); }
  FormulaPointerSet::iterator end(void) { return existingFormulas.end(); }

  void print(void);
  Formula* Get(Formula *f);
  void Remove(Formula *f);
  void SetIds(int *varCount);
  bool GenerateCNF(FormulaVector constraint, vector<vector<int> >& conj, unsigned int* pMappedVarId, int* varCount);

private:
  bool IsClause(Formula* f);
  void GatherConjuncts(Formula* f, vector<Formula*> & args);

  Formula* GoThroughNegations(Formula* f, int* level);

  void TraverseNaryConnectives(Formula* f, FORMULA_TYPE type, vector<int> & args,vector<vector <int> > & conj_tmp);
  void TraverseFormulaTreeAndGenerateCNF(Formula* f,vector<vector <int> > & conj_tmp);
};

#endif
