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

#include "Formula.h"
#include "FormulaFactory.h"

unsigned int Formula::NEW = 0, Formula::DEL = 0;

#include <unordered_set>
#include <functional>
using std::hash;

#define MAGIC1 255
#define MAGIC2 65535
#define MAGIC3 16777215

size_t compute_hash(enum FORMULA_TYPE type, bool val) { 
 // return std::hash<unsigned int>()((unsigned int)type) * MAGIC1 + std::hash<unsigned int>()((unsigned int)val) * MAGIC2;
  return ((unsigned int)type) * MAGIC1 + ((unsigned int)val) * MAGIC1;
} 

size_t compute_hash(enum FORMULA_TYPE type, unsigned int o) { 
//  return std::hash<unsigned int>()((unsigned int)type) * MAGIC1 + std::hash<unsigned int>()(o) * MAGIC2;
  return ((unsigned int)type) * MAGIC1 + o * MAGIC2;
} 

size_t compute_hash(enum FORMULA_TYPE type, Formula *f) { 
//  return std::hash<unsigned int>()((unsigned int)type) * MAGIC1 + std::hash<const char*>()((char*)f);
  return ((unsigned int)type) * MAGIC1 + f->hashCode() * MAGIC2;
} 

size_t compute_hash(enum FORMULA_TYPE type, Formula *f1, Formula *f2) { 
//  return std::hash<unsigned int>()((unsigned int)type) * MAGIC1 + std::hash<const char*>()((char*)f1) * MAGIC2 
 // + std::hash<const char*>()((char*)f2) * MAGIC2;
  return ((unsigned int)type) * MAGIC1 + f1->hashCode() * MAGIC3 + f2->hashCode() * MAGIC3;
}


////////////////////////////////////////////////////////////////////////////////
////	Class Formula
////////////////////////////////////////////////////////////////////////////////


Formula::Formula() : refCount(0), m_bDefined(false) {  }

Formula::Formula(FORMULA_TYPE t, size_t hV) : type(t), refCount(0), hashValue(hV), m_bDefined(false) {}

FORMULA_TYPE Formula::GetType(void) { return type; }

unsigned int Formula::GetId(void) { return id; }
void Formula::SetId(unsigned int id) { this->id = id; }

void Formula::IncRefCount(void) { ++refCount; }
void Formula::DecRefCount(void) { --refCount; }

unsigned int Formula::GetRefCount(void) { return refCount; }

size_t Formula::hashCode() const { return hashValue; }

bool Formula::operator == (const Formula& f) const {
  if (type != f.type)
    return false;
  return equals(f);
}


// assuming that f is in FormulaFactory
Formula* Formula::makeNot(Formula *f) {

  Formula *tmp;
  if (f->GetType() == CONST) {
    if (((FormulaConst*)f)->GetValue() == true)
      tmp = new FormulaNT();
    else
      tmp = new FormulaT();

    FormulaFactory::Instance()->Remove(f);
  }
  else
    tmp = new FormulaNot(f);

  Formula::NEW++;
  return FormulaFactory::Instance()->Get(tmp);
}


// assuming that f1 and f2 are in FormulaFactory
Formula* Formula::makeOr(Formula *f1, Formula *f2) {

  if (f1 == f2)
    return f1;
  else if (f1->GetType() == CONST) {
    if (((FormulaConst*)f1)->GetValue() == false) {
      FormulaFactory::Instance()->Remove(f1);
      return f2;
    }
    else {
      FormulaFactory::Instance()->Remove(f2);
      return f1; // true
    }
  }
  else if (f2->GetType() == CONST) {
    if (((FormulaConst*)f2)->GetValue() == false) {
      FormulaFactory::Instance()->Remove(f2);
      return f1;
    }
    else {
      FormulaFactory::Instance()->Remove(f1);
      return f2; // true
    }
  }
  else {
    Formula::NEW++;
    return FormulaFactory::Instance()->Get(new FormulaOr(f1, f2));
  }
}

// assuming that f1 and f2 are in FormulaFactory
Formula* Formula::makeXor(Formula *f1, Formula *f2) {

  Formula *tmp;
  if (f1 == f2) {
    Formula::NEW++;
    FormulaFactory::Instance()->Remove(f1);
    return FormulaFactory::Instance()->Get(new FormulaNT());
  }
  else if (f1->GetType() == CONST) {
    if (((FormulaConst*)f1)->GetValue() == false)
      tmp = f2;
    else 
      tmp = makeNot(f2);

    FormulaFactory::Instance()->Remove(f1);
    return tmp;
  }
  else if (f2->GetType() == CONST) {
    if (((FormulaConst*)f2)->GetValue() == false)
      tmp = f1;
    else
      tmp = makeNot(f1);

    FormulaFactory::Instance()->Remove(f2);
    return tmp;
  }
  else {
    Formula::NEW++;
    return FormulaFactory::Instance()->Get(new FormulaXor(f1, f2));
  }
}


// assuming that f1 and f2 are in FormulaFactory
Formula* Formula::makeEquiv(Formula *f1, Formula *f2) {

  Formula *tmp;
  if (f1 == f2) {
    Formula::NEW++;
    FormulaFactory::Instance()->Remove(f1);
    return FormulaFactory::Instance()->Get(new FormulaT());
  }
  else if (f1->GetType() == CONST) {
    if (((FormulaConst*)f1)->GetValue() == false)
      tmp = makeNot(f2);
    else 
      tmp = f2;

    FormulaFactory::Instance()->Remove(f1);
    return tmp;
  }
  else if (f2->GetType() == CONST) {
    if (((FormulaConst*)f2)->GetValue() == false)
      tmp = makeNot(f1);
    else
      tmp = f1;

    FormulaFactory::Instance()->Remove(f2);
    return tmp;
  }
  else {
    Formula::NEW++;
    return FormulaFactory::Instance()->Get(new FormulaEquiv(f1, f2));
  }
}



// assuming that f1 and f2 are in FormulaFactory
Formula* Formula::makeAnd(Formula *f1, Formula *f2) {

  if (f1 == f2)
    return f1;
  else if (f1->GetType() == CONST) {
    if (((FormulaConst*)f1)->GetValue() == false) {
      FormulaFactory::Instance()->Remove(f2);
      return f1;
    }
    else {
      FormulaFactory::Instance()->Remove(f1);
      return f2;
    }
  }
  else if (f2->GetType() == CONST) {
    if (((FormulaConst*)f2)->GetValue() == false) {
      FormulaFactory::Instance()->Remove(f1);
      return f2;
    }
    else {
      FormulaFactory::Instance()->Remove(f2);
      return f1;
    }
  }
  else {
    Formula::NEW++;
    return FormulaFactory::Instance()->Get(new FormulaAnd(f1, f2));
  }
}


////////////////////////////////////////////////////////////////////////////////
////	Class FormulaConst
////////////////////////////////////////////////////////////////////////////////

FormulaConst::FormulaConst(bool val) : Formula(CONST, compute_hash(CONST, val)), value(val) {}

bool FormulaConst::GetValue(void) { return value; }

bool FormulaConst::equals(const Formula& f) const {
  return value == ((FormulaConst&)f).value;
}

void FormulaConst::print(void) { cout << value;}


////////////////////////////////////////////////////////////////////////////////
////	Class FormulaT
////////////////////////////////////////////////////////////////////////////////

FormulaT::FormulaT() : FormulaConst(true) {}

//void FormulaT::print(void) { cout << "1"; }


////////////////////////////////////////////////////////////////////////////////
////	Class FormulaNT
////////////////////////////////////////////////////////////////////////////////

FormulaNT::FormulaNT() : FormulaConst(false) {}

//void FormulaNT::print(void) { cout << "0"; }


////////////////////////////////////////////////////////////////////////////////
////	Class FormulaVar
////////////////////////////////////////////////////////////////////////////////

FormulaVar::FormulaVar(unsigned int o) : Formula(VAR, compute_hash(VAR,o)), ordinal(o) {}

void FormulaVar::print(void) { cout << " v" << ordinal << " "; }

unsigned int FormulaVar::GetOrdinal(void) { return ordinal; }

bool FormulaVar::equals(const Formula& f) const {
  return ordinal == ((FormulaVar&)f).ordinal;
}


////////////////////////////////////////////////////////////////////////////////
////	Class FormulaNot
////////////////////////////////////////////////////////////////////////////////

FormulaNot::FormulaNot(Formula *f) : Formula(NOT, compute_hash(NOT,f)), F(f) {
  f->IncRefCount();
}

FormulaNot::~FormulaNot() {

  F->DecRefCount();
  FormulaFactory::Instance()->Remove(F);
}

void FormulaNot::print(void) {

  if (F->GetType() == VAR) {
    cout << " ~";
    F->print();
  }
  else {
    cout << " ~(";
    F->print();
    cout << ")";
  }
}

Formula* FormulaNot::GetF(void) { return F; }

bool FormulaNot::equals(const Formula& f) const {
  return F == ((FormulaNot&)f).F;
}


////////////////////////////////////////////////////////////////////////////////
////	Class FormulaBinary
////////////////////////////////////////////////////////////////////////////////

FormulaBinary::FormulaBinary(Formula *f1, Formula *f2, FORMULA_TYPE type) : Formula(type, compute_hash(type,f1,f2)), leftF(f1), rightF(f2) {
  leftF->IncRefCount();
  rightF->IncRefCount();
}

FormulaBinary::~FormulaBinary() {

  leftF->DecRefCount();
  FormulaFactory::Instance()->Remove(leftF);
  rightF->DecRefCount();
  FormulaFactory::Instance()->Remove(rightF);
}

Formula* FormulaBinary::GetLeftF(void) { return leftF; }

Formula* FormulaBinary::GetRightF(void) { return rightF; }

bool FormulaBinary::equals(const Formula& f) const {
  return (leftF == ((FormulaBinary&)f).leftF && rightF == ((FormulaBinary&)f).rightF) || 
         (leftF == ((FormulaBinary&)f).rightF && rightF == ((FormulaBinary&)f).leftF);
}

void FormulaBinary::print(void) {

  if (leftF->GetType() == VAR)
    leftF->print();
  else {
    cout << " (";
    leftF->print();
    cout << ") ";
  }

  switch (type) {
  case AND :
    cout << " & ";
    break;
  case OR :
    cout << " | ";
    break;
  case XOR :
    cout << " ^ ";
    break;
  case EQUIV :
    cout << " <=> ";
    break;
  default: break;
  }

  if (rightF->GetType() == VAR)
    rightF->print();
  else {
    cout << " (";
    rightF->print();
    cout << ") ";
  }
}


////////////////////////////////////////////////////////////////////////////////
////	Class FormulaAnd
////////////////////////////////////////////////////////////////////////////////

FormulaAnd::FormulaAnd(Formula *f1, Formula *f2) : FormulaBinary(f1, f2, AND) {}


////////////////////////////////////////////////////////////////////////////////
////	Class FormulaOr
////////////////////////////////////////////////////////////////////////////////

FormulaOr::FormulaOr(Formula *f1, Formula *f2) : FormulaBinary(f1, f2, OR) {}


////////////////////////////////////////////////////////////////////////////////
////	Class FormulaXor
////////////////////////////////////////////////////////////////////////////////

FormulaXor::FormulaXor(Formula *f1, Formula *f2) : FormulaBinary(f1, f2, XOR) {}


////////////////////////////////////////////////////////////////////////////////
////	Class FormulaEquiv
////////////////////////////////////////////////////////////////////////////////

FormulaEquiv::FormulaEquiv(Formula *f1, Formula *f2) : FormulaBinary(f1, f2, EQUIV) {}





