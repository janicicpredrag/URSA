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

#ifndef __FORMULA_H
#define __FORMULA_H

#include <iostream>
using namespace std;


// Legal formula types
enum FORMULA_TYPE {

  // Constants
  CONST,

  // Variables
  VAR,

  // Logical connectives
  NOT,
  AND,
  OR,
  XOR,
  EQUIV
};

////////////////////////////////////////////////////////////////////////////////
////	Class Formula
////////////////////////////////////////////////////////////////////////////////

class Formula {

public:

  virtual ~Formula() {}

  virtual void print(void) = 0;

  FORMULA_TYPE GetType(void);

  unsigned int GetId(void);
  void SetId(unsigned int id);

  void IncRefCount(void);
  void DecRefCount(void);

  unsigned int GetRefCount(void);

  virtual bool equals(const Formula& f) const = 0;
  bool operator == (const Formula& f) const;

  size_t hashCode() const;

  static unsigned int I, D, NEW, DEL;

  static Formula* makeNot(Formula*);
  static Formula* makeOr(Formula*, Formula*);
  static Formula* makeXor(Formula*, Formula*);
  static Formula* makeAnd(Formula*, Formula*);
  static Formula* makeEquiv(Formula*, Formula*);

  bool IsDefined() { return m_bDefined; }
  void SetDefined() { m_bDefined=true; }

protected:
  FORMULA_TYPE type;
  unsigned int refCount;
  unsigned int id;
  size_t hashValue;
  
  unsigned int m_bDefined;

  Formula();

  Formula(FORMULA_TYPE type, size_t hashValue);
};

////////////////////////////////////////////////////////////////////////////////
////	Class FormulaConst
////////////////////////////////////////////////////////////////////////////////

class FormulaConst : public Formula {

public:

  FormulaConst(bool value);
  bool GetValue(void);
  bool equals(const Formula& f) const;
  void print(void);

protected:

  bool value;
};

////////////////////////////////////////////////////////////////////////////////
////	Class FormulaT
////////////////////////////////////////////////////////////////////////////////

class FormulaT : public FormulaConst {

public:

  FormulaT();

};

////////////////////////////////////////////////////////////////////////////////
////	Class FormulaNT
////////////////////////////////////////////////////////////////////////////////

class FormulaNT : public FormulaConst {

public:

  FormulaNT();

};

////////////////////////////////////////////////////////////////////////////////
////	Class FormulaVar
////////////////////////////////////////////////////////////////////////////////

class FormulaVar : public Formula {

public:

  FormulaVar(unsigned int o);
  void print(void);
  unsigned int GetOrdinal(void);
  bool equals(const Formula& f) const;

private:

  unsigned int ordinal;
};

////////////////////////////////////////////////////////////////////////////////
////	Class FormulaNot
////////////////////////////////////////////////////////////////////////////////

class FormulaNot : public Formula {

public:

  FormulaNot(Formula *f);
  ~FormulaNot();
  void print(void);
  Formula* GetF(void);
  bool equals(const Formula& f) const;

private:

  Formula *F;
};

////////////////////////////////////////////////////////////////////////////////
////	Class FormulaBinary
////////////////////////////////////////////////////////////////////////////////

class FormulaBinary : public Formula {

public:

  FormulaBinary(Formula *f1, Formula *f2, FORMULA_TYPE type);
  virtual ~FormulaBinary();
  Formula* GetLeftF(void);
  Formula* GetRightF(void);
  bool equals(const Formula& f) const;
  void print(void);

protected:

  Formula *leftF, *rightF;
};

////////////////////////////////////////////////////////////////////////////////
////	Class FormulaAnd
////////////////////////////////////////////////////////////////////////////////

class FormulaAnd : public FormulaBinary {

public:

  FormulaAnd(Formula *f1, Formula *f2);
  ~FormulaAnd() {}

};

////////////////////////////////////////////////////////////////////////////////
////	Class FormulaOr
////////////////////////////////////////////////////////////////////////////////

class FormulaOr : public FormulaBinary {

public:

  FormulaOr(Formula *f1, Formula *f2);
  ~FormulaOr() {}

};

////////////////////////////////////////////////////////////////////////////////
////	Class FormulaXor
////////////////////////////////////////////////////////////////////////////////

class FormulaXor : public FormulaBinary {

public:

  FormulaXor(Formula *f1, Formula *f2);
  ~FormulaXor() {}

};


////////////////////////////////////////////////////////////////////////////////
////	Class FormulaEquiv
////////////////////////////////////////////////////////////////////////////////

class FormulaEquiv : public FormulaBinary {

public:

  FormulaEquiv(Formula *f1, Formula *f2);
  ~FormulaEquiv() {}

};

#endif
