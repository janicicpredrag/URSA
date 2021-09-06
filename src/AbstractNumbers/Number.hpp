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

#ifndef __NUMBER_H
#define __NUMBER_H

#include <FormulaVector.h>
#include <GroundNumber.hpp>

enum eVarType { eGroundNumber, eAbstractNumber };

typedef FormulaVector AbstractNumber; 

class Boolean;

class Number { 
friend class Boolean;

public:
  Number(char *s);  
  Number(unsigned int* iVarCounter);
  Number(unsigned int n);
  Number(GroundNumber n);
  Number(const Number& n);
  ~Number();

  Number& operator = (Number n);
 /* Number& operator = (GroundNumber n);*/
  void SetConstraint(unsigned int n);
  Number negate() const;
  Number bitnegate() const;

  Number operator << (const Number &n);
  Number operator >> (const Number &n);
  Number operator *  (const Number &n);
  Number operator +  (const Number &n);
  Number operator -  (const Number &n);
  Number operator ^  (const Number &n);
  Number operator |  (const Number &n);
  Number operator &  (const Number &n);
  Boolean operator <  (const Number &n);
  Boolean operator >  (const Number &n);
  Boolean operator <= (const Number &n);
  Boolean operator >= (const Number &n);  
  Boolean operator == (const Number &n);
  Boolean operator != (const Number &n);

  Number ite(const Boolean &b, const Number &n2);
  Number sgn();
  Boolean Bool() const;
  void print(char base) const;
  void PrintValue(char base) const;
  void PrintStatus() const;
  void TruncateToSize(unsigned int size);

  bool IsGroundNumber() const { return (m_VarType == eGroundNumber); }
  eVarType GetType() const { return m_VarType; }
  void SetType(eVarType Type) { m_VarType=Type; }

  unsigned int GetID() const { return m_nID; }
  bool IsIndependent() const { return (m_nID!=-1); }
  void SetIsDependent() { m_nID=-1; }

  GroundNumber GetGroundValue() const { return m_nN; }
  unsigned int GetGroundValueUnsigned() const { 
     return m_nN.GetGroundValueUnsigned();
  }
  AbstractNumber GetAbstractValue() const { return m_nW; }

  void SetAccessed(bool b) { bAccessed=b; }
  bool GetAccessed() const { return bAccessed; }

private: 

  GroundNumber m_nN;
  AbstractNumber m_nW;
  int m_nID;
  eVarType m_VarType;

  bool bAccessed; /* for dealing with arguments of procedures that are not used */
};


#endif
