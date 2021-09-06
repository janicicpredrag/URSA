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

#include <assert.h>
#include "FormulaVector.h"
#include "FormulaFactory.h"


////////////////////////////////////////////////////////////////////////////////
////	Class FormulaVector
////////////////////////////////////////////////////////////////////////////////


FormulaVector::FormulaVector() {
  Formula *f = FormulaFactory::Instance()->Get(new FormulaNT());
  Formula::NEW++;
  size = iAbstractNumberLength;
  bitArray = new Formula* [size];
  for (unsigned int i = 0; i < size; i++) {
    bitArray[i] = f;
    f->IncRefCount();
  }
}


// -------------------------------------------------------------------


FormulaVector::FormulaVector(const FormulaVector &fv) {
  size = fv.getSize();
  bitArray = new Formula* [size];
  for (unsigned int i = 0; i < size; i++) {
    bitArray[i] = fv.bitArray[i];
    fv.bitArray[i]->IncRefCount();
  }
}


// -------------------------------------------------------------------


FormulaVector::FormulaVector(unsigned int n) {
  Formula *f = FormulaFactory::Instance()->Get(new FormulaNT());
  Formula::NEW++;
  size = n;

  bitArray = new Formula* [size];
  for (unsigned int i = 0; i < size; i++) {
    bitArray[i] = f;
    f->IncRefCount();
  }
}


// -------------------------------------------------------------------


FormulaVector::FormulaVector(const GroundNumber &n) {
  unsigned int i;
  Formula *NT = FormulaFactory::Instance()->Get(new FormulaNT());
  Formula::NEW++;
  Formula *T = FormulaFactory::Instance()->Get(new FormulaT());
  Formula::NEW++;

  for (i = 0; i < size; i++) {
    if (n.Bit(i))
      setFormulaAt(size - 1 - i, T);
    else
      setFormulaAt(size - 1 - i, NT);
 cout << i << endl;  
}

  FormulaFactory::Instance()->Remove(T);
  FormulaFactory::Instance()->Remove(NT);
}



// -------------------------------------------------------------------


FormulaVector::~FormulaVector() {
  for (unsigned int i = 0; i < size; i++) {
    bitArray[i]->DecRefCount();
    FormulaFactory::Instance()->Remove(bitArray[i]);
  }

  delete [] bitArray;
}


// -------------------------------------------------------------------


FormulaVector& FormulaVector::operator = (unsigned long n) {
  unsigned int i, j;
  Formula *NT = FormulaFactory::Instance()->Get(new FormulaNT());
  Formula::NEW++;
  Formula *T = FormulaFactory::Instance()->Get(new FormulaT());
  Formula::NEW++;

  for (i = 0, j = n; i < size; i++, j /= 2)
    if (j % 2)
      setFormulaAt(size - 1 - i, T);
    else
      setFormulaAt(size - 1 - i, NT);

  FormulaFactory::Instance()->Remove(T);
  FormulaFactory::Instance()->Remove(NT);

  return *this;
}


// -------------------------------------------------------------------


FormulaVector& FormulaVector::FormulaVector::operator = (const GroundNumber &n) {
  unsigned int i;
  Formula *NT = FormulaFactory::Instance()->Get(new FormulaNT());
  Formula::NEW++;	
  Formula *T = FormulaFactory::Instance()->Get(new FormulaT());
  Formula::NEW++;

  for (i = 0; i < size; i++) {
    if (n.Bit(i))
      setFormulaAt(size - 1 - i, T);
    else
      setFormulaAt(size - 1 - i, NT);
  }

  FormulaFactory::Instance()->Remove(T);
  FormulaFactory::Instance()->Remove(NT);

  return *this;
}

// -------------------------------------------------------------------



FormulaVector& FormulaVector::operator = (const FormulaVector &fv) {
  assert(size == fv.getSize());

  for (unsigned int i = 0; i < fv.getSize(); i++)
    setFormulaAt(i, fv.bitArray[i]);

  return *this;
}


// -------------------------------------------------------------------


void FormulaVector::setFormulaAt(unsigned int i, Formula *f) {

  Formula *tmp = bitArray[i];
  bitArray[i] = f;
  f->IncRefCount();
  tmp->DecRefCount();
  FormulaFactory::Instance()->Remove(tmp);
}


// -------------------------------------------------------------------


Formula* FormulaVector::getFormulaFrom(unsigned int i) const {
  return bitArray[i];
}


// -------------------------------------------------------------------


Formula* FormulaVector::operator [] (unsigned int i) const {
  return bitArray[i];
}


// -------------------------------------------------------------------


void FormulaVector::init(unsigned int n) {
  for (unsigned int i = 0; i < size; i++) { 
    Formula *f = FormulaFactory::Instance()->Get(new FormulaVar(n + i + 1));
    setFormulaAt(i, f);
    Formula::NEW++;
  }
}

// -------------------------------------------------------------------

void FormulaVector::initDefined(unsigned int n) {
  for (unsigned int i = 0; i < size; i++) { 
    Formula *f = FormulaFactory::Instance()->Get(new FormulaVar(n + i + 1));
    setFormulaAt(i, f);
    f->SetDefined();
    Formula::NEW++;
  }
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator ~ () const {

  FormulaVector notFormulaVector(size);

  Formula *tmp;
  for (unsigned int i = 0; i < size; i++) {
    tmp = Formula::makeNot(bitArray[i]);
    notFormulaVector.setFormulaAt(i, tmp);
  }

  return notFormulaVector;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator << (unsigned int k) const {

  FormulaVector shlFormulaVector(size);

  assert(k >= 0);

  for (unsigned int i = k; i < size; i++)
    shlFormulaVector.setFormulaAt(i - k, bitArray[i]);

  Formula *f = FormulaFactory::Instance()->Get(new FormulaNT());
  Formula::NEW++;

  if (k>size)
    k=size;
  for (unsigned int i = 0; i < k; i++)
    shlFormulaVector.setFormulaAt(size - k + i, f);

  return shlFormulaVector;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator >> (unsigned int k) const {

  FormulaVector shrFormulaVector(size);

  // assert(k >= 0 && k < size);
  assert(k >= 0);

  if (k>size)
    k=size;

  Formula *f = FormulaFactory::Instance()->Get(new FormulaNT());
  Formula::NEW++;
  for (unsigned int i = 0; i < k; i++)
    shrFormulaVector.setFormulaAt(i, f);

  for (unsigned int i = k; i < size; i++)
    shrFormulaVector.setFormulaAt(i, bitArray[i - k]);

  return shrFormulaVector;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator | (const FormulaVector &fv) const {
  assert(size == fv.getSize());

  FormulaVector res(size);

  Formula *tmp;
  for (unsigned int i = 0; i < size; i++) {
    tmp = Formula::makeOr(bitArray[i], fv.bitArray[i]);
    res.setFormulaAt(i, tmp);
  }

  return res;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator ^ (const FormulaVector &fv) const {
  assert(size == fv.getSize());

  FormulaVector res(size);

  Formula *f;
  for (unsigned int i = 0; i < size; i++) {
    f = Formula::makeXor(bitArray[i], fv.bitArray[i]);
    res.setFormulaAt(i, f);
  }

  return res;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator & (const FormulaVector &fv) const {
  assert(size == fv.getSize());
  FormulaVector res(size);

  Formula *f;
  for (unsigned int i = 0; i < size; i++) {
    f = Formula::makeAnd(bitArray[i], fv.bitArray[i]);
    res.setFormulaAt(i, f);
  }
  return res;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator & (const unsigned long c) const {
  FormulaVector res(size);
  res = c;
  return res & *this;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator * (const FormulaVector &fv) {
  assert(size == fv.getSize());
  FormulaVector res(2 * size);

  for (unsigned int i = 0; i < size; i++) {
    Formula *f = Formula::makeAnd(bitArray[i], fv.bitArray[fv.getSize() - 1]);
    res.setFormulaAt(i + size, f);
  }

  FormulaVector tmp_p(2 * size);
  for (int j = size - 1; j >= 0; j--)
    tmp_p.setFormulaAt(size + j, bitArray[j]);

  for (int j = fv.getSize() - 2; j >= 0; j--) {

    FormulaVector tmp = tmp_p << (fv.getSize() - 1 - j);
    Formula *f;
    for (unsigned int i = 0; i < tmp.getSize(); i++) {
      f = Formula::makeAnd(tmp.bitArray[i], fv.bitArray[j]);
      tmp.setFormulaAt(i, f);
    }
    res += tmp;
  }

  FormulaVector result;
  for (unsigned int j = 0; j < size; j++)
    result.setFormulaAt(j, res.bitArray[size+j]);

  return result;
}


// -------------------------------------------------------------------


void FormulaVector::print(void) const {
  for (unsigned int i = 0; i < size; i++) {
    cout << i+1 << ". ";
    bitArray[i]->print();
    cout << endl;
  }
  cout << endl;
}


// -------------------------------------------------------------------


void FormulaVector::SetConstraint(unsigned int e_n_b) {

  Formula *tmp;
  for (int i = size - 1; i >= 0; i--, e_n_b /= 2) {
    if (e_n_b % 2 == 0) {
      tmp = Formula::makeNot(bitArray[i]);

      setFormulaAt(i, tmp);
    }
  }
}


// -------------------------------------------------------------------


FormulaVector& FormulaVector::operator &= (const FormulaVector &fv) {
  assert(size == fv.getSize());
  Formula *f;
  for (unsigned int i = 0; i < size; i++) {
    f = Formula::makeAnd(bitArray[i], fv.bitArray[i]);
    setFormulaAt(i, f);
  }

  return *this;
}


// -------------------------------------------------------------------


FormulaVector& FormulaVector::operator ^= (const FormulaVector &fv) {
  assert(size == fv.getSize());
  Formula *f;
  for (unsigned int i = 0; i < size; i++) {
    f = Formula::makeXor(bitArray[i], fv.bitArray[i]);
    setFormulaAt(i, f);
  }
  return *this;
}


// -------------------------------------------------------------------


FormulaVector& FormulaVector::operator += (const FormulaVector &fv) {
  assert(size == fv.getSize());

  Formula *c = FormulaFactory::Instance()->Get(new FormulaNT());
  Formula::NEW++;
  Formula *sumF;

  for(int i = size - 1; i >= 0; i--) {
    Formula *andF = NULL, *orF = NULL, *xorF = NULL;

    andF = Formula::makeAnd(bitArray[i], fv.bitArray[i]);
    orF = Formula::makeOr(bitArray[i], fv.bitArray[i]);
    xorF = Formula::makeXor(bitArray[i], fv.bitArray[i]);

    c->IncRefCount();
    sumF = Formula::makeXor(xorF, c);

    setFormulaAt(i, sumF);

    c->DecRefCount();
    c = Formula::makeOr(andF, Formula::makeAnd(c, orF));
  }

  FormulaFactory::Instance()->Remove(c);

  return *this;
}


// -------------------------------------------------------------------


FormulaVector& FormulaVector::operator += (const unsigned long l) {

  Formula *t = FormulaFactory::Instance()->Get(new FormulaT());
  Formula::NEW++;
  Formula *nt = FormulaFactory::Instance()->Get(new FormulaNT());
  Formula::NEW++;
  Formula *c = nt;
  Formula *sumF;
  unsigned long n = l;

  t->IncRefCount();
  nt->IncRefCount();

  for(int i = size - 1; i >= 0; i--, n >>= 1) {
    Formula *andF = NULL, *orF = NULL, *xorF = NULL;

    if (n & 1) {
      andF = bitArray[i];
      orF = t;
      xorF = Formula::makeNot(bitArray[i]);
    }
    else {
      andF = nt;
      orF = bitArray[i];
      xorF = bitArray[i];
    }

    c->IncRefCount();
    sumF = Formula::makeXor(xorF, c);

    setFormulaAt(i, sumF);

    c->DecRefCount();
    c = Formula::makeOr(andF, Formula::makeAnd(c, orF));
  }

  t->DecRefCount();
  nt->DecRefCount();

  FormulaFactory::Instance()->Remove(c);
  FormulaFactory::Instance()->Remove(t);
  FormulaFactory::Instance()->Remove(nt);

  return *this;
}


// -------------------------------------------------------------------


FormulaVector& FormulaVector::operator -= (const FormulaVector &fv) {
  assert(size == fv.getSize());
  FormulaVector minusFormulaVector(fv.getSize());

  Formula *c = FormulaFactory::Instance()->Get(new FormulaNT());
  c->IncRefCount();
  Formula::NEW++;
  Formula *difF;

  for(int i = size - 1; i >= 0; i--) {
    Formula *andF = NULL, *orF = NULL, *xorF = NULL;

    xorF = Formula::makeXor(bitArray[i], fv.bitArray[i]);

    difF = Formula::makeXor(xorF, c);

    setFormulaAt(i, difF);

    c->DecRefCount();

    Formula *notF = Formula::makeNot(bitArray[i]);
    notF->IncRefCount();

    andF = Formula::makeAnd(notF, fv.bitArray[i]);
    orF = Formula::makeOr(notF, fv.bitArray[i]);

    c = Formula::makeOr(andF, Formula::makeAnd(c, orF));
    c->IncRefCount();

    notF->DecRefCount();
    FormulaFactory::Instance()->Remove(notF);
  }

  c->DecRefCount();
  FormulaFactory::Instance()->Remove(c);

  return *this;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator + (const FormulaVector &fv) {
  assert(size == fv.getSize());
  FormulaVector plusFormulaVector(fv.getSize());

  Formula *c = FormulaFactory::Instance()->Get(new FormulaNT());
  Formula::NEW++;
  Formula *sumF;

  for(int i = size - 1; i >= 0; i--) {
    Formula *andF = NULL, *orF = NULL, *xorF = NULL;

    andF = Formula::makeAnd(bitArray[i], fv.bitArray[i]);
    orF = Formula::makeOr(bitArray[i], fv.bitArray[i]);
    xorF = Formula::makeXor(bitArray[i], fv.bitArray[i]);

    c->IncRefCount();
    sumF = Formula::makeXor(xorF, c);

    plusFormulaVector.setFormulaAt(i, sumF);

    c->DecRefCount();
    c = Formula::makeOr(andF, Formula::makeAnd(c, orF));
  }

  FormulaFactory::Instance()->Remove(c);

  return plusFormulaVector;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::operator - (const FormulaVector &fv) {
  assert(size == fv.getSize());
  FormulaVector minusFormulaVector(fv.getSize());

  Formula *c = FormulaFactory::Instance()->Get(new FormulaNT());
  c->IncRefCount();
  Formula::NEW++;
  Formula *difF;

  for(int i = size - 1; i >= 0; i--) {
    Formula *andF = NULL, *orF = NULL, *xorF = NULL;

    xorF = Formula::makeXor(bitArray[i], fv.bitArray[i]);

    difF = Formula::makeXor(xorF, c);

    minusFormulaVector.setFormulaAt(i, difF);

    c->DecRefCount();

    Formula *notF = Formula::makeNot(bitArray[i]);
    notF->IncRefCount();

    andF = Formula::makeAnd(notF, fv.bitArray[i]);
    orF = Formula::makeOr(notF, fv.bitArray[i]);

    c = Formula::makeOr(andF, Formula::makeAnd(c, orF));
    c->IncRefCount();

    notF->DecRefCount();
    FormulaFactory::Instance()->Remove(notF);
  }

  c->DecRefCount();
  FormulaFactory::Instance()->Remove(c);

  return minusFormulaVector;
}


// -------------------------------------------------------------------


Formula* FormulaVector::doAnd(void) {

  Formula *f = FormulaFactory::Instance()->Get(new FormulaT());
  Formula::NEW++;

  for (unsigned int i = 0; i < size; i++)
    f = Formula::makeAnd(f, bitArray[i]);

  return f;
}


// -------------------------------------------------------------------


Formula* FormulaVector::doOr(void) {

  Formula *f = FormulaFactory::Instance()->Get(new FormulaNT());
  Formula::NEW++;

  for (unsigned int i = 0; i < size; i++)
    f = Formula::makeOr(f, bitArray[i]);

  return f;
}


// -------------------------------------------------------------------


void FormulaVector::setFormulaVectorAt(const FormulaVector &fv, unsigned int k) {

  for (unsigned int i = 0; i < fv.getSize(); i++)
    setFormulaAt(k + i, fv.bitArray[i]);
}


// -------------------------------------------------------------------


FormulaVector1 FormulaVector::operator < (const FormulaVector &fv) {
  assert(size == fv.getSize());

  FormulaVector1 result, r1,r2;

  r2 = (*this)[size-1];
  r1 = fv[size-1];
  result = (r1 & ~r2);

  for(int i=size-2;i>=0;i--) {
    r2 = (*this)[i];
    r1 = fv[i];
    result = (r1 & ~r2) | (result & (r1==r2));
  }
 
  return result;
}


// -------------------------------------------------------------------


FormulaVector1 FormulaVector::operator > (const FormulaVector &fv) {
  assert(size == fv.getSize());
  FormulaVector1 result, r1,r2;

  r1 = (*this)[size-1];
  r2 = fv[size-1];
  result = (r1 & ~r2);

  for(int i=size-2;i>=0;i--) {
    r1 = (*this)[i];
    r2 = fv[i];
    result = (r1 & ~r2) | (result & (r1==r2));
  }
  return result;
}


// -------------------------------------------------------------------


FormulaVector1 FormulaVector::operator <= (const FormulaVector &fv) {
  return ~(*this>fv);
}


// -------------------------------------------------------------------


FormulaVector1 FormulaVector::operator >= (const FormulaVector &fv) {
  return ~(*this<fv);
}


// -------------------------------------------------------------------


FormulaVector1 FormulaVector::operator == (const FormulaVector &fv) {
  assert(size == fv.getSize());

  FormulaVector1 result, r;
  result = Formula::makeEquiv(bitArray[0], fv.bitArray[0]);
  for (unsigned int i = 1; i < size; i++) {
    r = Formula::makeEquiv(bitArray[i], fv.bitArray[i]);
    result = result & r;
  }

  return result;
}


// -------------------------------------------------------------------


FormulaVector1 FormulaVector::operator != (const FormulaVector &fv) {
  assert(size == fv.getSize());

  FormulaVector1 result, r;

  result = Formula::makeXor(bitArray[0], fv.bitArray[0]);
  for (unsigned int i = 1; i < size; i++) {
    r = Formula::makeXor(bitArray[i], fv.bitArray[i]);
    result = result | r;
  }

  return result;
}


// -------------------------------------------------------------------


FormulaVector1& FormulaVector1::operator = (unsigned long n) {
  Formula *NT = FormulaFactory::Instance()->Get(new FormulaNT());
  Formula::NEW++;
  Formula *T = FormulaFactory::Instance()->Get(new FormulaT());
  Formula::NEW++;

  if (n%2)
    setFormulaAt(size - 1, T);
  else
    setFormulaAt(size - 1, NT);

  FormulaFactory::Instance()->Remove(T);
  FormulaFactory::Instance()->Remove(NT);

  return *this;
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::ite(const FormulaVector &fv1, const FormulaVector &fv2) const {
  assert(size == 1);

  FormulaVector result(fv1.size);
  Formula *r1,*r2,*r3;
  for(unsigned i=0; i < fv1.size; i++) {
    if(fv1.bitArray[i]==fv2.bitArray[i])
      result.setFormulaAt(i,fv1.bitArray[i]);
    else {
      r1=Formula::makeNot(bitArray[0]);
      r2=Formula::makeOr(r1,fv1.bitArray[i]);
      r3=Formula::makeOr(bitArray[0],fv2.bitArray[i]);
      result.setFormulaAt(i,Formula::makeAnd(r2,r3));
    }
  }

  return result;  
}


// -------------------------------------------------------------------


FormulaVector FormulaVector::sgn() const {
  FormulaVector rf;
  FormulaVector1 result, r;
  result = bitArray[0];
  for (unsigned int i = 1; i < size; i++) {
    r = bitArray[i];
    result = result | r;
  }
  rf.setFormulaAt(size-1,result.bitArray[0]);
  return rf;
}


// -------------------------------------------------------------------


FormulaVector1 FormulaVector::sgn1() const {
  FormulaVector1 result, r;
  result = bitArray[0];
  for (unsigned int i = 1; i < size; i++) {
    r = bitArray[i];
    result = result | r;
  }
  return result;
}


// -------------------------------------------------------------------


FormulaVector::operator FormulaVector1() {
  assert(size==1);
  FormulaVector1 res;
  res = bitArray[0];
  return res;
}


// -------------------------------------------------------------------


FormulaVector1& FormulaVector1::operator = (Formula *f) {
  assert(size == 1);
  setFormulaAt(0, f);
  return *this;
}


// -------------------------------------------------------------------
