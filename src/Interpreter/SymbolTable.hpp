/************************************************************************************
URSA -- Copyright (c) 2010, Predrag Janicic

This file is part of URSA.
 
URSA is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
 
URSA is WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
**************************************************************************************/

#ifndef __SYMBOL_TABLE
#define __SYMBOL_TABLE

#include <map>
#include <string>
#include <iostream>
#include "Number.hpp"
#include "Boolean.hpp"
#include "Valuation.hpp"
#include "SATsolver.h"


using namespace std;
using namespace ArgoSat;


struct lstr
{
  bool operator()(const string s1, const string s2) const
  {
    return (s1<s2);
  }
};


class SymbolTable {

public:
  SymbolTable () { };
  ~SymbolTable () { Clear(); };

  void Clear(); 

  void printSym();
  void printIndependent(Valuation& model);
  unsigned int getGroundNumber(unsigned int iCounter,Valuation& model);
  bool getGroundBoolean(unsigned int iCounter,Valuation& model);

  void printIndependentMapped(Valuation& model, unsigned int* pMappedVarId);
  unsigned int getGroundNumberMapped(unsigned int iCounter,Valuation& model, unsigned int* pMappedVarId);
  bool getGroundBooleanMapped(unsigned int iCounter,Valuation& model, unsigned int* pMappedVarId);

  void printMapping();
  void printIndependentMapped(SATsolver* solver, unsigned int* pMappedVarId);
  unsigned int getGroundNumberMapped(unsigned int iCounter,SATsolver* solver, unsigned int* pMappedVarId);
  bool getGroundBooleanMapped(unsigned int iCounter,SATsolver* solver, unsigned int* pMappedVarId);
  void printGroundNumberMapped(unsigned int iCounter,SATsolver* solver, unsigned int* pMappedVarId);

  bool DefinedIntVar(const string sVarName);
  void letInt(const string sVarName, const Number nValue);
  void letIntEl(const string sVarName, const Number& nIndex, const Number& nValue);
  void letIntEl2(const string sVarName, const Number& nIndex1, const Number& nIndex2, const Number& nValue);
  const Number getIntValue(const string sVarName, unsigned int* iVarCounter);
  const Number getIntElValue(const string sVarName, const Number& nIndex, unsigned int* iVarCounter);
  const Number getIntElValue2(const string sVarName, const Number& nIndex1,const Number& nIndex2, unsigned int* iVarCounter);

  bool DefinedBoolVar(const string sVarName);
  void letBool(const string sVarName, const Boolean bValue);
  void letBoolEl(const string sVarName, const Number& nIndex, const Boolean bValue);
  void letBoolEl2(const string sVarName, const Number& nIndex1, const Number& nIndex2, const Boolean bValue);
  const Boolean getBoolValue(const string sVarName, unsigned int* iVarCounter);
  const Boolean getBoolElValue(const string sVarName, const Number& nIndex, unsigned int* iVarCounter);
  const Boolean getBoolElValue2(const string sVarName, const Number& nIndex1,const Number& nIndex2, unsigned int* iVarCounter);

  bool SetAccessedIntVar(const string sVarName, bool bA);
  bool GetAccessedIntVar(const string sVarName);
  bool SetAccessedBoolVar(const string sVarName, bool bA);
  bool GetAccessedBoolVar(const string sVarName);

  // Export of coherent proofs (given the input is a specification of a coherent proof encoded 
  int printGroundNumberMappedforLaTeX(unsigned int iCounter,SATsolver* solver, unsigned int* pMappedVarId);
  int readProofParameter(SATsolver* solver, unsigned int* pMappedVarId, string& s);
  bool readBoolProofParameter(SATsolver* solver, unsigned int* pMappedVarId, string& s);
  void exportCoherentLogicProof2Txt(SATsolver* solver, unsigned int* pMappedVarId);
  int arity(SATsolver* solver, unsigned int* pMappedVarId, int predicate);

private:

  map<const string, Number*, lstr > SymInt;
  map<const string, Boolean*, lstr > SymBool;

};


#endif
