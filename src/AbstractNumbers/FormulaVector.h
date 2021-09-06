/************************************************************************************
URSA -- Copyright (c) 2010, Predrag Janicic

This file is part of URSA.
Authors: Predrag Janicic and Milan Sesum
 
URSA is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
 
URSA is WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
**************************************************************************************/

#ifndef __FORMULAVECTOR_H
#define __FORMULAVECTOR_H

#include <vector>
using namespace std;

#include "Formula.h"
#include "GroundNumber.hpp"

extern unsigned int iAbstractNumberLength;


class FormulaVector1;

////////////////////////////////////////////////////////////////////////////////
////	Class FormulaVector
////////////////////////////////////////////////////////////////////////////////

class FormulaVector {

public:

  FormulaVector();
  FormulaVector(unsigned int n);
  FormulaVector(unsigned int n, bool b);
  FormulaVector(const FormulaVector &fv);
  FormulaVector(const GroundNumber &n);

  ~FormulaVector();

  void init(unsigned int n);
  void initDefined(unsigned int n); /* original vars are not to be neglected, even if irrelevant */

  FormulaVector& operator = (unsigned long n);
  FormulaVector& operator = (const GroundNumber &n);
  FormulaVector& operator = (const unsigned char n[]);
  FormulaVector& operator = (const FormulaVector &fv);

  void setFormulaAt(unsigned int i, Formula *f);
  void setFormulaVectorAt(const FormulaVector &fv, unsigned int k);
  Formula* getFormulaFrom(unsigned int i) const;
  Formula* operator [] (unsigned int i) const;

  FormulaVector operator ~ () const;
  FormulaVector& operator &= (const FormulaVector &fv);
  FormulaVector& operator ^= (const FormulaVector &fv);
  FormulaVector operator << (unsigned int k) const;
  FormulaVector operator >> (unsigned int k) const;
  FormulaVector operator | (const FormulaVector &fv) const;
  FormulaVector operator ^ (const FormulaVector &fv) const;
  FormulaVector operator & (const FormulaVector &fv) const;
  FormulaVector operator & (const unsigned long c) const;

  FormulaVector& operator += (const FormulaVector &fv);
  FormulaVector& operator += (const unsigned long n);
  FormulaVector& operator -= (const FormulaVector &fv);
  FormulaVector operator * (const FormulaVector &fv);
  FormulaVector operator + (const FormulaVector &fv);
  FormulaVector operator - (const FormulaVector &fv);

  FormulaVector1 operator < (const FormulaVector &fv);
  FormulaVector1 operator > (const FormulaVector &fv);
  FormulaVector1 operator <= (const FormulaVector &fv);
  FormulaVector1 operator >= (const FormulaVector &fv);
  FormulaVector1 operator == (const FormulaVector &fv);
  FormulaVector1 operator != (const FormulaVector &fv);

  FormulaVector ite(const FormulaVector &fv1, const FormulaVector &fv2) const;
  FormulaVector sgn() const;
  FormulaVector1 sgn1() const;

  operator FormulaVector1();

  void print(void) const;

  void SetConstraint(unsigned int);

  Formula* doAnd(void);
  Formula* doOr(void);

  unsigned int getSize(void) const { return size; };

protected:
  Formula** bitArray;
  unsigned int size;

};


class FormulaVector1 : public FormulaVector {

public:
  FormulaVector1& operator = (unsigned long n);

  FormulaVector1() : FormulaVector(1) {}
  FormulaVector1& operator = (Formula *w);
};



#endif
