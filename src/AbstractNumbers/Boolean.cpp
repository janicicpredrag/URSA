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

#include <Boolean.hpp>
#include <Number.hpp>

#define DEF_B_OP(OPERATOR) \
   Boolean Boolean::operator OPERATOR (const Boolean &b) { \
     Boolean m(false);                           \
     if(IsGroundBoolean())  {             \
       if(b.IsGroundBoolean())  {         \
         m.m_bB = (m_bB OPERATOR b.m_bB); \
         m.SetType(eGroundBoolean);       \
       }                                  \
       else  {                            \
         AbstractBoolean w;               \
         w = m_bB;                        \
         m.m_bW = (w OPERATOR b.m_bW);    \
         m.SetType(eAbstractBoolean);     \
       }                                  \
     }                                    \
     else  {                              \
       if(b.IsGroundBoolean()) {          \
         AbstractBoolean w;               \
         w = b.m_bB;                      \
         m.m_bW = (m_bW OPERATOR w);      \
         m.SetType(eAbstractBoolean);     \
       }                                  \
       else {                             \
         m.m_bW = m_bW OPERATOR b.m_bW;   \
         m.SetType(eAbstractBoolean);     \
       }                                  \
     }                                    \
     return m;                            \
   };

  Boolean::Boolean(unsigned int* iVarCounter) { 
    //cout << "1a" << endl;  
    m_nID= *iVarCounter; 
    m_bW.initDefined(*iVarCounter); 
    *iVarCounter += 1;
    SetType(eAbstractBoolean); 
  }    
  
  Boolean::Boolean(bool b) { 
    // cout << "2a " << b << endl; 
    m_nID=-1;
    m_bB=b; 
    SetType(eGroundBoolean); 
  }  

  Boolean::Boolean(const Boolean& b) { 
    // cout << "3a" << endl; 
    m_bB     =b.m_bB; 
    m_bW     =b.m_bW; 
    m_nID    =b.m_nID;
    m_VarType=b.m_VarType;
  }    

  Boolean::~Boolean() {}

  Boolean& Boolean::operator = (Boolean b) { 
    //cout << "4a" << endl; 
    m_bB     =b.m_bB; 
    m_bW     =b.m_bW; 
    m_nID    =b.m_nID;
    m_VarType=b.m_VarType;
    return *this;
  }

  void Boolean::SetConstraint(bool b) {
    m_bW.SetConstraint(b);
  }

  Boolean Boolean::negate() const {  
     Boolean m(false);
     if(IsGroundBoolean())  {
       if(m_bB)
         m.m_bB = false;
       else 
         m.m_bB = true;
       m.SetType(eGroundBoolean);
     }
     else  { 
       m.m_bW = ~m_bW;
       m.SetType(eAbstractBoolean);
     }
     return m;
  }

  DEF_B_OP(&)
  DEF_B_OP(|)
  DEF_B_OP(^)

  Boolean Boolean::ite(const Boolean &b, const Boolean &b2) const {
     if(b.IsGroundBoolean())               
       return (b.m_bB ? *this : b2);
     else {
         AbstractBoolean ab1; 
         if(IsGroundBoolean()) 
           ab1=m_bB;
         else
           ab1=m_bW;
         AbstractBoolean ab2; 
         if(b2.IsGroundBoolean()) 
           ab2=b2.m_bB;
         else
           ab2=b2.m_bW;
         Boolean m(false);                           
         m.m_bW=b.m_bW.ite(ab1,ab2); 
         m.SetType(eAbstractBoolean);
         return m;
      }
  }


  Number Boolean::Int() const {
     Number m((unsigned int)0);                           
     if(IsGroundBoolean())  {
       m.m_nN = GetGroundValue(); 
       m.SetType(eGroundNumber);
     }
     else {
       m.m_nW.setFormulaVectorAt(m_bW,m.m_nW.getSize()-1);
       m.SetType(eAbstractNumber);
     }
     return m;
  }


  void Boolean::print() const {  
    PrintStatus();
    PrintValue();
  }

  void Boolean::PrintValue() const {  
    cout << "--> value: " << endl;
    if(IsGroundBoolean())  {
      if(m_bB)
        cout << "T" << endl;
      else
        cout << "F" << endl;
    }
    else 
      m_bW.print();
  }

  void Boolean::PrintStatus() const {  
    if(IsGroundBoolean())  
      cout << "--> ground boolean" << endl;
    else {
      if(IsIndependent())
        cout << "--> abstract boolean - independent " << endl;
      else 
        cout << "--> abstract boolean - dependent "<< endl;
    }
  }




