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

#ifndef __BOOLEAN_H
#define __BOOLEAN_H

#include <FormulaVector.h>


enum eBooleanVarType { eGroundBoolean, eAbstractBoolean };

typedef FormulaVector1 AbstractBoolean; 

class Number;

class Boolean { 
friend class Number;

public:
  Boolean(unsigned int* iVarCounter);   
  Boolean(bool b);  
  Boolean(const Boolean& b);    
  ~Boolean();
  Boolean& operator = (Boolean b);

  void SetConstraint(bool b);
  Boolean negate() const;

  Boolean operator & (const Boolean &b);
  Boolean operator | (const Boolean &b);
  Boolean operator ^ (const Boolean &b);

  Boolean ite(const Boolean &b, const Boolean &b2) const;
  Number Int() const;
  void print() const;
  void PrintValue() const;
  void PrintStatus() const;

  bool IsGroundBoolean() const { return (m_VarType == eGroundBoolean); }
  eBooleanVarType GetType() const { return m_VarType; }
  void SetType(eBooleanVarType Type) { m_VarType=Type; }

  unsigned int GetID() const { return m_nID; }
  bool IsIndependent() const { return (m_nID!=-1); }
  void SetIsDependent() { m_nID=-1; }

  bool GetGroundValue() const { return m_bB; }
  AbstractBoolean GetAbstractValue() const { return m_bW; }

  void SetAccessed(bool b) { bAccessed=b; }
  bool GetAccessed() const { return bAccessed; }

/*private: */
  bool m_bB;
  AbstractBoolean m_bW;
  int m_nID;
  eBooleanVarType m_VarType;
  bool bAccessed; /* for dealing with arguments of procedures that are not used */

};




#endif
