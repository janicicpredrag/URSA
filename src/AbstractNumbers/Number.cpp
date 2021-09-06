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


#include <FormulaVector.h>
#include <Number.hpp>
#include <Boolean.hpp>

extern unsigned int iAbstractNumberLength;


#define DEF_OP(OPERATOR) \
   Number Number::operator OPERATOR (const Number &n) { \
     Number m((unsigned int)0);                           \
     if(IsGroundNumber())  {             \
       if(n.IsGroundNumber())  {         \
         m.m_nN = m_nN OPERATOR n.m_nN;  \
         m.SetType(eGroundNumber);       \
         m.TruncateToSize(iAbstractNumberLength); \
       }                                 \
       else  {                           \
         AbstractNumber w;               \
         w = m_nN;                       \
         m.m_nW = w OPERATOR n.m_nW;     \
         m.SetType(eAbstractNumber);     \
       }                                 \
     }                                   \
     else  {                             \
       if(n.IsGroundNumber()) {          \
         AbstractNumber w;               \
         w = n.m_nN;                     \
         m.m_nW = m_nW OPERATOR w;       \
         m.SetType(eAbstractNumber);     \
       }                                 \
       else {                            \
         m.m_nW = m_nW OPERATOR n.m_nW;  \
         m.SetType(eAbstractNumber);     \
       }                                 \
     }                                   \
     return m;                           \
   };

#define DEF_REL_OP(REL_OPERATOR) \
   Boolean Number::operator REL_OPERATOR (const Number &n) { \
     Boolean b(false);                   \
     if(IsGroundNumber())  {             \
       if(n.IsGroundNumber())  {         \
         b.m_bB = m_nN REL_OPERATOR n.m_nN;  \
         b.SetType(eGroundBoolean);      \
       }                                 \
       else  {                           \
         AbstractNumber w;               \
         w = m_nN;                       \
         b.m_bW = w REL_OPERATOR n.m_nW; \
         b.SetType(eAbstractBoolean);    \
       }                                 \
     }                                   \
     else  {                             \
         if(n.IsGroundNumber()) {        \
         AbstractNumber w;               \
         w = n.m_nN;\
         b.m_bW = m_nW REL_OPERATOR w;   \
         b.SetType(eAbstractBoolean);    \
       }                                 \
       else {                            \
         b.m_bW = m_nW REL_OPERATOR n.m_nW;  \
         b.SetType(eAbstractBoolean);    \
       }                                 \
     }                                   \
     return b;                           \
   };


  Number::Number(unsigned int* iVarCounter) { 
    m_nID= *iVarCounter; 
    m_nW.initDefined(*iVarCounter); 
    *iVarCounter += iAbstractNumberLength;
    SetType(eAbstractNumber); 
  }
  
  Number::Number(unsigned int n) { 
    m_nID=-1;
    m_nN = n;
    SetType(eGroundNumber);
    TruncateToSize(iAbstractNumberLength);  
  }  

  Number::Number(char *s) { 
    m_nID=-1;
    m_nN = s;
    SetType(eGroundNumber);
    TruncateToSize(iAbstractNumberLength);  
  }  

/*
  Number::Number(GroundNumber n) { 
    m_nID=-1;
    m_nN=n; 
cout << " ovde " << endl;  
  SetType(eGroundNumber);
    TruncateToSize(iAbstractNumberLength);  
  }  
*/

  Number::Number(const Number& n) { 
    m_nN     =n.m_nN; 
    m_nW     =n.m_nW; 
    m_nID    =n.m_nID;
    m_VarType=n.m_VarType;
  }    

  Number::~Number() {}

  Number& Number::operator = (Number n) { 
    m_nN     =n.m_nN; 
    m_nW     =n.m_nW; 
    m_nID    =n.m_nID;
    m_VarType=n.m_VarType;
    return *this;
  }

/*
  Number& Number::operator = (GroundNumber n) { 
    m_nID=-1;
    m_nN=n; 
cout << " OVDE " << endl;  
  SetType(eGroundNumber);
    return *this;
  }
*/

  void Number::SetConstraint(unsigned int n) {
    m_nW.SetConstraint(n);
  }

  Number Number::negate() const {  
     Number m((unsigned int)0);
     if(IsGroundNumber())  {
       GroundNumber z(0); 
       m.m_nN = z-m_nN;
       m.SetType(eGroundNumber);
     }
     else  { 
       AbstractNumber w;
       w = (long unsigned int)0;
       m.m_nW = w-m_nW;
       m.SetType(eAbstractNumber);
     }
     return m;
  }


  Number Number::bitnegate() const {  
     Number m((unsigned int)0);
     if(IsGroundNumber())  {
       m.m_nN = m_nN.bitnegate();
       m.SetType(eGroundNumber);
     }
     else  { 
       AbstractNumber w;
       m.m_nW = ~m_nW;
       m.SetType(eAbstractNumber);
     }
     return m;
  }


  Number Number::operator << (const Number &n) { 
    Number m((unsigned int)0);                           
    //assert(n.IsGroundNumber());
    if(IsGroundNumber())  {             
      m.m_nN = (m_nN << n.m_nN);  
      m.SetType(eGroundNumber);       
    }                                   
    else  {                             
      m.m_nW = (m_nW << n.m_nN.GetGroundValueUnsigned());       
      m.SetType(eAbstractNumber);     
    }                                   
    return m;                           
  }


  Number Number::operator >> (const Number &n) { 
    Number m((unsigned int)0);                           
    //assert(n.IsGroundNumber());
    if(IsGroundNumber())  {             
      m.m_nN = (m_nN >> n.m_nN);  
      m.SetType(eGroundNumber);       
    }                                   
    else  {                             
      m.m_nW = (m_nW >> n.m_nN.GetGroundValueUnsigned());       
      m.SetType(eAbstractNumber);     
    }                                   
    return m;                           
  }


  DEF_OP(*)
  DEF_OP(+)
  DEF_OP(-)
  DEF_OP(^)
  DEF_OP(|)
  DEF_OP(&)

//  DEF_OP(/)

  DEF_REL_OP(<)
  DEF_REL_OP(>)
  DEF_REL_OP(<=)
  DEF_REL_OP(>=)
  DEF_REL_OP(==)
  DEF_REL_OP(!=)


  Number Number::ite(const Boolean &b, const Number &n2) {
     if(b.IsGroundBoolean()) {              
       return (b.m_bB ? *this : n2);
     }
     else {
         AbstractNumber an1; 
         if(IsGroundNumber()) 
           an1=m_nN;
         else
           an1=m_nW;
         AbstractNumber an2; 
         if(n2.IsGroundNumber()) 
           an2=n2.m_nN;
         else
           an2=n2.m_nW;
         Number m((unsigned int)0);                           
         m.m_nW=b.m_bW.ite(an1,an2); 
         m.SetType(eAbstractNumber);
         return m;
      }
  }


  Number Number::sgn() {
    Number m((unsigned int)0);                           
    if(IsGroundNumber())  {
      m.m_nN=(m_nN!=0); 
      m.SetType(eGroundNumber);
      return m;
    }
    else {
      m.m_nW=m_nW.sgn();
      m.SetType(eAbstractNumber);
      return m;
    }
  }



  Boolean Number::Bool() const {
     Boolean b(false);                           
     if(IsGroundNumber())  {
       b.m_bB = m_nN.Bool(); 
       b.SetType(eGroundBoolean);
     }
     else {
       b.m_bW = m_nW.sgn1();
       b.SetType(eAbstractBoolean);
     }
     return b;
  }



  void Number::print(char base) const {  
    PrintStatus();
    PrintValue(base);
  }

  void Number::PrintValue(char base) const {  
    cout << " --> value: ";
    if(IsGroundNumber())  {
      m_nN.PrintValue(base);
    }
    else {
      cout << endl;
      m_nW.print();
    }
  };


  void Number::PrintStatus() const {  
    if(IsGroundNumber())  
      cout << " --> ground number" << endl;
    else {
      if(IsIndependent())
        cout << " --> abstract number - independent " << endl;
      else 
        cout << " --> abstract number - dependent "<< endl;
    }
  }


  void Number::TruncateToSize(unsigned int /*size*/) {
    if(IsGroundNumber()) 
      m_nN.TruncateToSize();
  }



