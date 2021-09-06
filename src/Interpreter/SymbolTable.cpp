/************************************************************************************
URSA -- Copyright (c) 2010, Predrag Janicic

This file is part of URSA
 
URSA is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
 
URSA is WITHOUT ANY WARRANTY; without even the implied warranty of
GNU General Public License for more details.
**************************************************************************************/

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <assert.h>
#include <math.h>
#include "SymbolTable.hpp"

using namespace std;


string itos(unsigned int i)
{
   stringstream ss; 
   ss << i;
   return ss.str();
}


void SymbolTable::Clear() {
  map<const string, Number*, lstr>::iterator it;
  for ( it=SymInt.begin() ; it != SymInt.end(); it++ )  
    delete (*it).second;
  
  map<const string, Boolean*, lstr>::iterator itb;
  for ( itb=SymBool.begin() ; itb != SymBool.end(); itb++ )  
    delete (*itb).second;

  SymInt.clear();
  SymBool.clear();

  /*m_iVarCounter=0;*/
}


void SymbolTable::printSym()
{
  map<const string, Number*, lstr>::iterator it;
  for ( it=SymInt.begin() ; it != SymInt.end(); it++ )  {
    cout << (*it).first;
    (*it).second->PrintStatus();
//  (*it).second->PrintValue();
  }
  cout << endl;
  map<const string, Boolean*, lstr>::iterator itb;
  for ( itb=SymBool.begin() ; itb != SymBool.end(); itb++ )  {
    cout << (*itb).first;
    (*itb).second->PrintStatus();
//  (*itb).second->PrintValue();
  }
  cout << endl;
}


// ----------------------------------------------------------------------------


void SymbolTable::printIndependent(Valuation& model)
{
  map<const string, Number*, lstr>::iterator it;
  for ( it=SymInt.begin() ; it != SymInt.end(); it++ )  {
    if ((!(*it).second->IsGroundNumber()) && (*it).second->IsIndependent()) {   
      cout << (*it).first << "=" << getGroundNumber((*it).second->GetID(),model) << endl; // prints only truncated numbers
    }
  }
  map<const string, Boolean*, lstr>::iterator itb;
  for ( itb=SymBool.begin() ; itb != SymBool.end(); itb++ )  {
    if ((!(*itb).second->IsGroundBoolean()) && (*itb).second->IsIndependent()) {   
      cout << (*itb).first << "=";
      if(getGroundBoolean((*itb).second->GetID(),model)) 
        cout << "true" << endl;
      else
        cout << "false" << endl;
    }
  }
}


unsigned int SymbolTable::getGroundNumber(unsigned int iCounter,Valuation& model) {
  unsigned int n=0;
  for(unsigned int i=iCounter+1;i<=iCounter+iAbstractNumberLength;i++) {
    n = n << 1;
    n |= model.isTrue(Literals::fromInt(i));
  }
  return n;
}



bool SymbolTable::getGroundBoolean(unsigned int iCounter,Valuation& model) {
  return model.isTrue(Literals::fromInt(iCounter+1));
}


// ----------------------------------------------------------------------------

// These functions work with a reduced set of original variables


unsigned int SymbolTable::getGroundNumberMapped(unsigned int iCounter,Valuation& model, unsigned int* pMappedVarId) {
  unsigned int n=0;
  for(unsigned int i=iCounter+1;i<=iCounter+iAbstractNumberLength;i++) {
    n = n << 1;
    n |= model.isTrue(Literals::fromInt(pMappedVarId[i]));
  }
  return n;
}


void SymbolTable::printGroundNumberMapped(unsigned int iCounter,SATsolver* solver, unsigned int* pMappedVarId) {
    int pos;
    unsigned int nLen = (iAbstractNumberLength+7)/8;  
    unsigned char* pDigits = new unsigned char[nLen];
	for(unsigned int i=0;i<nLen;i++) 
      pDigits[i]=0;

    for(unsigned int i=iCounter+1;i<=iCounter+iAbstractNumberLength;i++) {
      pos = iCounter+iAbstractNumberLength-i;
      pDigits[pos/8] |= ((solver->isTrueVar(pMappedVarId[i])) << (pos % 8));
    }

    if(nLen<=sizeof(unsigned int)) { // print solutions in the decimal form if they can fit into int, otherwise print in the hex form
      unsigned int s=0; 
      for(int i=nLen-1;i>=0;i--)  
         s = s*256+pDigits[i]; 
   	  cout << s;
    }
    else {
	  cout << "0x";
      for(unsigned int i=nLen;i>0;i--) {
          cout << (char)((pDigits[i]>>4) >=10 ? 'A'+(pDigits[i]>>4)-10 : '0'+(pDigits[i]>>4));
          cout << (char)((pDigits[i]&15) >=10 ? 'A'+(pDigits[i]&15)-10 : '0'+(pDigits[i]&15)); 
	  }
    }
    delete [] pDigits;
}

bool SymbolTable::getGroundBooleanMapped(unsigned int iCounter,Valuation& model, unsigned int* pMappedVarId) {
  return model.isTrue(Literals::fromInt(pMappedVarId[iCounter+1]));
}


// ----------------------------------------------------------------------------

// These functions work with a reduced set of original variables
void SymbolTable::printMapping()
{
  cout << endl << "-------------------------------------------------------------" << endl;
  cout << "Mapping between independent URSA variables and SAT variables:" << endl;
  map<const string, Number*, lstr>::iterator it;
  for ( it=SymInt.begin() ; it != SymInt.end(); it++ )  {
    if ((!(*it).second->IsGroundNumber()) && (*it).second->IsIndependent()) {   
      cout << (*it).first << "="; 
      unsigned int iCounter = (*it).second->GetID();
      for(unsigned int i=iCounter+1;i<=iCounter+iAbstractNumberLength;i++) 
        cout << "|" << i;
      cout << "|" << endl;
    }
  }

  map<const string, Boolean*, lstr>::iterator itb;
  for ( itb=SymBool.begin() ; itb != SymBool.end(); itb++ )  {
    if ((!(*itb).second->IsGroundBoolean()) && (*itb).second->IsIndependent()) {   
      cout << (*itb).first << "=";
      cout << "|" << (*itb).second->GetID()+1 << "|" << endl;
    }
  }
  cout << "-------------------------------------------------------------" << endl;
}


// ----------------------------------------------------------------------------

// These functions work with a reduced set of original variables
void SymbolTable::printIndependentMapped(SATsolver* solver, unsigned int* pMappedVarId)
{
  map<const string, Number*, lstr>::iterator it;
  for ( it=SymInt.begin() ; it != SymInt.end(); it++ )  {
    if ((!(*it).second->IsGroundNumber()) && (*it).second->IsIndependent()) {   
//      cout << (*it).first << "=" << getGroundNumberMapped((*it).second->GetID(),solver, pMappedVarId) << endl;
      cout << (*it).first << "="; 
      if(iAbstractNumberLength<=sizeof(unsigned int)) 
        cout << getGroundNumberMapped((*it).second->GetID(),solver, pMappedVarId) << ";" << endl;
      else {
        printGroundNumberMapped((*it).second->GetID(),solver, pMappedVarId);
        cout << ";" << endl;
      }
    }
  }
  map<const string, Boolean*, lstr>::iterator itb;
  for ( itb=SymBool.begin() ; itb != SymBool.end(); itb++ )  {
    if ((!(*itb).second->IsGroundBoolean()) && (*itb).second->IsIndependent()) {   
      cout << (*itb).first << "=";
      if(getGroundBooleanMapped((*itb).second->GetID(),solver, pMappedVarId)) 
        cout << "true;" << endl;
      else
        cout << "false;" << endl;
    }
  }
}


// ----------------------------------------------------------------------------

unsigned int SymbolTable::getGroundNumberMapped(unsigned int iCounter,SATsolver* solver, unsigned int* pMappedVarId) {
  unsigned int n=0;
  for(unsigned int i=iCounter+1;i<=iCounter+iAbstractNumberLength;i++) {
    n = n << 1;
    n |= solver->isTrueVar(pMappedVarId[i]);
  }
  return n;
}



bool SymbolTable::getGroundBooleanMapped(unsigned int iCounter,SATsolver* solver, unsigned int* pMappedVarId) {
  return solver->isTrueVar(pMappedVarId[iCounter+1]);
}




// -----------------------------------------------------



bool SymbolTable::DefinedIntVar(const string sVarName)
{  
   if( SymInt.find(sVarName) != SymInt.end() )  
     return true;
/*   if( SymBool.find(sVarName) != SymBool.end() )  
     return true;*/
   return false;
}


void SymbolTable::letInt(const string sVarName, const Number nValue)
{  
   if (!DefinedIntVar(sVarName)) { 
     SymInt[sVarName] = new Number(nValue);
   }
   else {
      *SymInt[sVarName] = nValue;
   }
   SetAccessedIntVar(sVarName, true);
   SymbolTable::SymInt[sVarName]->SetIsDependent();
} 



void SymbolTable::letIntEl(const string sVarName, const Number& nIndex, const Number& nValue)
{
   assert ( nIndex.IsGroundNumber() );

   string sName = sVarName + "[" + itos(nIndex.GetGroundValueUnsigned()) + "]";
   letInt(sName,nValue);
}


void SymbolTable::letIntEl2(const string sVarName, const Number& nIndex1, const Number& nIndex2, const Number& nValue)
{
   assert( nIndex1.IsGroundNumber() && nIndex2.IsGroundNumber() );

   string sName = sVarName + "[" + itos(nIndex1.GetGroundValueUnsigned()) + "][" + itos(nIndex2.GetGroundValueUnsigned()) + "]";
   letInt(sName,nValue);
}



const Number SymbolTable::getIntValue(const string sVarName, unsigned int* iVarCounter)
{
   if(!DefinedIntVar(sVarName))
   {
      Number* n = new Number(iVarCounter);
      n->SetType(eAbstractNumber);
      SymInt[sVarName]=n;
   }
   SetAccessedIntVar(sVarName, true);
   return *SymInt[sVarName];
}


const Number SymbolTable::getIntElValue(const string sVarName, const Number& nIndex, unsigned int* iVarCounter)
{
   string sName = sVarName + "[" + itos(nIndex.GetGroundValueUnsigned()) + "]";
   return getIntValue(sName, iVarCounter);
}

    
const Number SymbolTable::getIntElValue2(const string sVarName, const Number& nIndex1, const Number& nIndex2, unsigned int* iVarCounter)
{
   string sName = sVarName + "[" + itos(nIndex1.GetGroundValueUnsigned()) + "][" + itos(nIndex2.GetGroundValueUnsigned()) + "]";
   return getIntValue(sName, iVarCounter);
}



// -----------------------------------------------------


bool SymbolTable::DefinedBoolVar(const string sVarName)
{  
   if( SymBool.find(sVarName) != SymBool.end() )  
     return true;
   else
     return false;
}



void SymbolTable::letBool(const string sVarName, const Boolean bValue)
{
/*   if (!DefinedIntVar(sVarName))*/
   if (!DefinedBoolVar(sVarName))
      SymBool[sVarName] = new Boolean(bValue);
   else 
      *SymBool[sVarName] = bValue;
   SetAccessedBoolVar(sVarName, true);
   SymbolTable::SymBool[sVarName]->SetIsDependent();
} 


void SymbolTable::letBoolEl(const string sVarName, const Number& nIndex, const Boolean bValue)
{
   assert( nIndex.IsGroundNumber() );

   string sName = sVarName + "[" + itos(nIndex.GetGroundValueUnsigned()) + "]";
   letBool(sName,bValue);
}


void SymbolTable::letBoolEl2(const string sVarName, const Number& nIndex1, const Number& nIndex2, const Boolean bValue)
{
   assert( nIndex1.IsGroundNumber() && nIndex2.IsGroundNumber() );

   string sName = sVarName + "[" + itos(nIndex1.GetGroundValueUnsigned()) + "][" + itos(nIndex2.GetGroundValueUnsigned()) + "]";
   letBool(sName,bValue);
}



const Boolean SymbolTable::getBoolValue(const string sVarName, unsigned int* iVarCounter)
{
/*   if(!DefinedIntVar(sVarName))*/
   if (!DefinedBoolVar(sVarName))
   {
      Boolean* b = new Boolean(iVarCounter);
      b->SetType(eAbstractBoolean);
      SymBool[sVarName]=b;
   }
   SetAccessedBoolVar(sVarName, true);
   return *SymBool[sVarName];
}


const Boolean SymbolTable::getBoolElValue(const string sVarName, const Number& nIndex, unsigned int* iVarCounter)
{
   string sName = sVarName + "[" + itos(nIndex.GetGroundValueUnsigned()) + "]";
   return getBoolValue(sName,iVarCounter);
}


const Boolean SymbolTable::getBoolElValue2(const string sVarName, const Number& nIndex1, const Number& nIndex2, unsigned int* iVarCounter)
{
   string sName = sVarName + "[" + itos(nIndex1.GetGroundValueUnsigned()) + "][" + itos(nIndex2.GetGroundValueUnsigned()) + "]";
   return getBoolValue(sName, iVarCounter);
}

bool SymbolTable::SetAccessedIntVar(const string sVarName, bool bA)
{  
  if( SymInt.find(sVarName) != SymInt.end() )  {
    SymInt.find(sVarName)->second->SetAccessed(bA);
    return true;  
  }
  else 
    return false;
}

bool SymbolTable::GetAccessedIntVar(const string sVarName)
{  
  if( SymInt.find(sVarName) != SymInt.end() )  
    return SymInt.find(sVarName)->second->GetAccessed();
  else 
    return false;
}

bool SymbolTable::SetAccessedBoolVar(const string sVarName, bool bA)
{  
  if( SymBool.find(sVarName) != SymBool.end() )  {
    SymBool.find(sVarName)->second->SetAccessed(bA);
    return true;
  }
  else 
    return false;
}

bool SymbolTable::GetAccessedBoolVar(const string sVarName)
{  
  if( SymBool.find(sVarName) != SymBool.end() )  
    return SymBool.find(sVarName)->second->GetAccessed();
  else 
    return false;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Support for SAT based proving in coherent logic


int SymbolTable::readProofParameter(SATsolver* solver, unsigned int* pMappedVarId, string& s)
{
      map<const string, Number*, lstr>::iterator it;
      for ( it=SymInt.begin() ; it != SymInt.end(); it++ )  {
        if ((!(*it).second->IsGroundNumber()) && (*it).second->IsIndependent()) {   
          if (s == (*it).first) {
             if(iAbstractNumberLength<=sizeof(unsigned int)) { 
                return getGroundNumberMapped((*it).second->GetID(),solver, pMappedVarId);
              }
              else
               return printGroundNumberMappedforLaTeX((*it).second->GetID(),solver, pMappedVarId);
          }   
        }
      }       
      if(!DefinedIntVar(s))
         return -1;
      const Number num = *SymInt[s];
      return num.GetGroundValueUnsigned();
}

// ----------------------------------------------------------------------------

bool SymbolTable::readBoolProofParameter(SATsolver* solver, unsigned int* pMappedVarId, string& s)
{
      map<const string, Boolean*, lstr>::iterator itb;
      for ( itb=SymBool.begin() ; itb != SymBool.end(); itb++ )  {
        if ((!(*itb).second->IsGroundBoolean()) && (*itb).second->IsIndependent()) {   
          if (s == (*itb).first) {
             if(iAbstractNumberLength<=sizeof(unsigned int)) { 
                return getGroundBooleanMapped((*itb).second->GetID(),solver, pMappedVarId);
              }
              else
               return printGroundNumberMappedforLaTeX((*itb).second->GetID(),solver, pMappedVarId);
          }   
        }
      }       
      if(!DefinedBoolVar(s))
         return -1;
      const Boolean b = *SymBool[s];
      return b.GetGroundValue();
}

// ----------------------------------------------------------------------------

int SymbolTable::printGroundNumberMappedforLaTeX(unsigned int iCounter,SATsolver* solver, unsigned int* pMappedVarId) {
    int value;
    int pos;
    unsigned int nLen = (iAbstractNumberLength+7)/8;  
    unsigned char* pDigits = new unsigned char[nLen];
	for(unsigned int i=0;i<nLen;i++) 
      pDigits[i]=0;

    for(unsigned int i=iCounter+1;i<=iCounter+iAbstractNumberLength;i++) {
      pos = iCounter+iAbstractNumberLength-i;
      pDigits[pos/8] |= ((solver->isTrueVar(pMappedVarId[i])) << (pos % 8));
    }

    if(nLen<=sizeof(unsigned int)) { // print solutions in the decimal form if they can fit into int, otherwise print in the hex form
      unsigned int s=0; 
      for(int i=nLen-1;i>=0;i--)  
         s = s*256+pDigits[i]; 
   	  value = s;
    }
    else {
	  cout << "0x";
      value = 0;
      for(unsigned int i=nLen;i>0;i--) {
          value += (char)((pDigits[i]>>4) >=10 ? 'A'+(pDigits[i]>>4)-10 : '0'+(pDigits[i]>>4));
          value +=  (char)((pDigits[i]&15) >=10 ? 'A'+(pDigits[i]&15)-10 : '0'+(pDigits[i]&15)); 
	  }
    }
    delete [] pDigits;
    return value;
}


int SymbolTable::arity(SATsolver* solver, unsigned int* pMappedVarId, int predicate)
{
    string s = "nArity[" + itos(predicate)+ "]";
    return readProofParameter(solver, pMappedVarId, s);
} 


string i2name(int i)
{
    if (i<26) 
        return string(1,'a'+i);
    else
        return "c"+itos(i);
}
 



void SymbolTable::exportCoherentLogicProof2Txt(SATsolver* solver, unsigned int* pMappedVarId)
{
enum StepKind { eAssumption, eNegIntro, eFirstCase, eSecondCase, eEQSub, eEQReflex, eEQSymm, eNegElim, eExcludedMiddle, eQEDbyCases, eQEDbyAssumption, eQEDbyEFQ, eQEDbyNegIntro, eNumberOfStepKinds };

  ofstream proofTxt;
  proofTxt.open ("sat-proof.txt");

  string s = "nMaxArg";
  int nMaxArg = readProofParameter(solver, pMappedVarId, s);

  unsigned int proofStep=0;
  for(;;) {
      int arg[2][20]; 
      string s = "nP[" + itos(proofStep)+"][0]";
      int predicate1 = readProofParameter(solver, pMappedVarId, s);
      if (predicate1 == -1) 
         break;
      for(int i=0;i<arity(solver,pMappedVarId,predicate1);i++) {
          s = "nA[" + itos(proofStep) +"][" + itos(i) + "]";
          arg[0][i] = readProofParameter(solver, pMappedVarId, s);
          arg[0][i] = arg[0][i] % 1000; // reduce spurious constants to <1000
      }

      int nesting;      
      s = "nNesting[" + itos(proofStep)+"]";
      nesting = readProofParameter(solver, pMappedVarId, s);
      if (nesting == -1) 
         break;

     // nesting = (int)(log(nesting)/log(2));
      proofTxt << setw(4) << right << proofStep << ". ";

      s = "nAxiomApplied[" + itos(proofStep)+"]";
      int axiom = readProofParameter(solver, pMappedVarId, s);

      if (axiom == -1) {
          proofTxt << setw(4) << right << nesting << "Unknown" << endl;
      }
      else if (axiom == eAssumption) {
          proofTxt << setw(4) << right << nesting << setw(4) << right << eAssumption << setw(4) << right << "0" << setw(5) << right << predicate1;
          for(int i=0;i<arity(solver,pMappedVarId,predicate1);i++) 
            proofTxt << setw(4) << right << arg[0][i];
          proofTxt << "   /*** Nesting: " << nesting << "; Step kind:" << eAssumption << " = Assumption; Branching: no; " << "p" << predicate1 << "(";
          for(int i=0;i<arity(solver,pMappedVarId,predicate1);i++) 
            proofTxt << i2name(arg[0][i]) << (i+1<arity(solver,pMappedVarId,predicate1) ? "," : "");
          proofTxt << ") ***/" << endl;
      }
      else if (axiom == eNegIntro) {
          proofTxt << setw(4) << right << nesting << setw(4) << right << eNegIntro << setw(4) << right << "0" << setw(5) << right << predicate1;
          for(int i=0;i<arity(solver,pMappedVarId,predicate1);i++) 
            proofTxt << setw(4) << right << arg[0][i];
          proofTxt << "   /*** Nesting: " << nesting << "; Step kind:" << eNegIntro << " = Neg Intro; Branching: no; " << "p" << predicate1 << "("; 
          for(int i=0;i<arity(solver,pMappedVarId,predicate1);i++) 
            proofTxt << i2name(arg[0][i]) << (i+1<arity(solver,pMappedVarId,predicate1) ? "," : "");
          proofTxt << ") ***/" << endl;
      }
      else if (axiom == eFirstCase) {
          proofTxt << setw(4) << right << nesting << setw(4) << right << eFirstCase << setw(4) << right << "0" << setw(5) << right << predicate1;
          for(int i=0;i<arity(solver,pMappedVarId,predicate1);i++) 
            proofTxt << setw(4) << right << arg[0][i];
          proofTxt << "   /*** Nesting: " << nesting << "; Step kind:" << eFirstCase << " = First case; Branching: no; " << "p" << predicate1 << "("; 
          for(int i=0;i<arity(solver,pMappedVarId,predicate1);i++) 
            proofTxt << i2name(arg[0][i]) << (i+1<arity(solver,pMappedVarId,predicate1) ? "," : "");
          proofTxt << ") ***/" << endl;
      }
      else if (axiom == eSecondCase) {
          proofTxt << setw(4) << right << nesting << setw(4) << right << eSecondCase << setw(4) << right << "0" << setw(5) << right << predicate1;
          for(int i=0;i<arity(solver,pMappedVarId,predicate1);i++) 
            proofTxt << setw(4) << right << arg[0][i];
          proofTxt << "   /*** Nesting: " << nesting << "; Step kind:" << eSecondCase << " = Second case; Branching: no; " << "p" << predicate1 << "("; 
          for(int i=0;i<arity(solver,pMappedVarId,predicate1);i++) 
            proofTxt << i2name(arg[0][i]) << (i+1<arity(solver,pMappedVarId,predicate1) ? "," : "");
          proofTxt << ") ***/" << endl;
      }
      else if (axiom == eQEDbyCases) {
          proofTxt << setw(4) << right << nesting << setw(4) << right  << eQEDbyCases;
          proofTxt << "   /*** Nesting: " << nesting << "; Step kind: QED by cases; ***/" << endl;
          if (nesting == 1) 
            break;
      }
      else if (axiom == eQEDbyAssumption) {
          proofTxt << setw(4) << right << nesting << setw(4) << right << eQEDbyAssumption;
          proofTxt << "   /*** Nesting: " << nesting << "; Step kind: QED by assumption; ***/" << endl;
          if (nesting == 1) 
            break;
      }
      else if (axiom == eQEDbyEFQ) {
          proofTxt << setw(4) << right << nesting << setw(4) << right << eQEDbyEFQ;
          proofTxt << "   /*** Nesting: " << nesting << "; Step kind: QED by EFQ; ***/" << endl;
          if (nesting == 1) 
            break;
      }
      else if (axiom == eQEDbyNegIntro) {
          proofTxt << setw(4) << right << nesting << setw(4) << right << eQEDbyNegIntro;
          proofTxt << "   /*** Nesting: " << nesting << "; Step kind: QED by NegIntro; ***/" << endl;
          if (nesting == 1) 
            break;
      }
      else if (axiom == eEQSub) {
          string sfrom1, sfrom2 = "";
          unsigned noPremises;
          int numberOfUnivVars;

          noPremises = 2;
          numberOfUnivVars = arity(solver,pMappedVarId,predicate1);

          for (unsigned i = 0; i<noPremises; i++) {  
             s = "nFrom[" + itos(proofStep)+"]["+ itos(i) +"]";
             int from = readProofParameter(solver, pMappedVarId, s);
             if (from == -1)
               assert(false);
             sfrom1 += itos(from) + " ";
             if (i > 0)
                sfrom2 += " and ";
             sfrom2 += "(" + itos(from) + ")";
          }

          proofTxt << setw(4) << right << nesting << setw(4) << right << axiom << setw(4) << right << "0" << setw(5) << right << predicate1;
          for(int i=0; i<arity(solver,pMappedVarId,predicate1); i++) 
             proofTxt << setw(4) << right <<  arg[0][i];
          proofTxt << "   /*** Nesting: " << nesting << "; Step kind:" << axiom << "=MP-axiom:" << axiom << "); Branching: no; " << "p" << predicate1 << "("; 
          for(int i=0;i<arity(solver,pMappedVarId,predicate1);i++) 
             proofTxt << i2name(arg[0][i]) << (i+1<arity(solver,pMappedVarId,predicate1) ? "," : "");
          proofTxt << ") ***/" << endl;

          proofTxt << setw(40) << right << " ";
          proofTxt << sfrom1 << "  /*** From steps: " << sfrom2 << " ***/" << endl;

          int inst[100];
          proofTxt << setw(40) << right << " ";
          for(int i=0; i<numberOfUnivVars; i++) {
              s = "nInst[" + itos(proofStep)+"][" + itos(i+1) + "]";
              inst[i] = readProofParameter(solver, pMappedVarId, s);
              proofTxt << inst[i] << " ";
          }
          s = "nInst[" + itos(proofStep)+"][" + itos(nMaxArg +2) + "]";
          proofTxt << readProofParameter(solver, pMappedVarId, s) << " ";

          proofTxt << "/*** Instantiation ***/" << endl;

      } 

      else if (axiom == eEQReflex) {
          proofTxt << setw(4) << right << nesting << setw(4) << right << axiom << setw(4) << right << "0" << setw(5) << right << predicate1;
          for(int i=0; i<arity(solver,pMappedVarId,predicate1); i++) 
             proofTxt << setw(4) << right <<  arg[0][i];
          proofTxt << "   /*** Nesting: " << nesting << "; Step kind:" << axiom << "=MP-axiom:" << axiom << "); Branching: no; " << "p" << predicate1 << "("; 
          for(int i=0;i<arity(solver,pMappedVarId,predicate1);i++) 
             proofTxt << i2name(arg[0][i]) << (i+1<arity(solver,pMappedVarId,predicate1) ? "," : "");
          proofTxt << ") ***/" << endl;

          proofTxt << setw(40) << right << " ";
          proofTxt << arg[0][0] << " ";  
          proofTxt << "/*** Instantiation ***/" << endl;
      } 

      else if (axiom == eEQSymm) {
          string sfrom1, sfrom2 = "";
          s = "nFrom[" + itos(proofStep)+"]["+ itos(0) +"]";
          int from = readProofParameter(solver, pMappedVarId, s);
          if (from == -1)
            assert(false);
          sfrom1 += itos(from) + " ";
          sfrom2 += "(" + itos(from) + ")";

          proofTxt << setw(4) << right << nesting << setw(4) << right << axiom << setw(4) << right << "0" << setw(5) << right << predicate1;
          for(int i=0; i<arity(solver,pMappedVarId,predicate1); i++) 
             proofTxt << setw(4) << right <<  arg[0][i];
          proofTxt << "   /*** Nesting: " << nesting << "; Step kind:" << axiom << "=MP-axiom:" << axiom << "); Branching: no; " << "p" << predicate1 << "("; 
          for(int i=0;i<arity(solver,pMappedVarId,predicate1);i++) 
             proofTxt << i2name(arg[0][i]) << (i+1<arity(solver,pMappedVarId,predicate1) ? "," : "");
          proofTxt << ") ***/" << endl;

          proofTxt << setw(40) << right << " ";
          proofTxt << sfrom1 << "  /*** From steps: " << sfrom2 << " ***/" << endl;

          proofTxt << setw(40) << right << " ";
          proofTxt << arg[0][0] << " ";
          proofTxt << arg[0][1] << " ";    
          proofTxt << "/*** Instantiation ***/" << endl;
      } 


      else if (axiom == eNegElim) {
          string sfrom1, sfrom2 = "";
          unsigned noPremises;
          int numberOfUnivVars;

          noPremises = 2;

          for (unsigned i = 0; i<noPremises; i++) {  
             s = "nFrom[" + itos(proofStep)+"]["+ itos(i) +"]";
             int from = readProofParameter(solver, pMappedVarId, s);
             if (from == -1)
               assert(false);

             string s = "nP[" + itos(from)+"][0]";
             int predicate_from = readProofParameter(solver, pMappedVarId, s);
             if (predicate_from == -1) 
                assert(false);
             numberOfUnivVars = arity(solver,pMappedVarId,predicate_from);

             sfrom1 += itos(from) + " ";
             if (i > 0)
                sfrom2 += " and ";
             sfrom2 += "(" + itos(from) + ")";
          }

          proofTxt << setw(4) << right << nesting << setw(4) << right << axiom << setw(4) << right << "0" << setw(5) << right << predicate1;
          for(int i=0; i<arity(solver,pMappedVarId,predicate1); i++) 
             proofTxt << setw(4) << right <<  arg[0][i];
          proofTxt << "   /*** Nesting: " << nesting << "; Step kind:" << axiom << "=MP-axiom:" << axiom << "); Branching: no; " << "p" << predicate1 << "("; 
          for(int i=0;i<arity(solver,pMappedVarId,predicate1);i++) 
             proofTxt << i2name(arg[0][i]) << (i+1<arity(solver,pMappedVarId,predicate1) ? "," : "");
          proofTxt << ") ***/" << endl;

          proofTxt << setw(40) << right << " ";
          proofTxt << sfrom1 << "  /*** From steps: " << sfrom2 << " ***/" << endl;

          proofTxt << setw(40) << right << " ";
          for(int i=0; i<numberOfUnivVars; i++) 
              proofTxt << i << " ";
          proofTxt << "/*** Instantiation ***/" << endl;
      } 
      else if (axiom == eExcludedMiddle) {
          string sfrom1, sfrom2 = "";
          int numberOfUnivVars = arity(solver,pMappedVarId,predicate1);

          s = "nP[" + itos(proofStep)+"][1]";
          int predicate2 = readProofParameter(solver, pMappedVarId, s);
          for(int i=0; i<arity(solver,pMappedVarId,predicate2); i++) {
              s = "nA[" + itos(proofStep) +"][" + itos(nMaxArg+i) + "]";
              arg[1][i] = readProofParameter(solver, pMappedVarId, s);
              arg[1][i] = arg[1][i] % 1000; // reduce spurious constants to <1000
          }
          proofTxt << setw(4) << right << nesting << setw(4) << right << " " << setw(4) << right << axiom << setw(4) << right << "1" << setw(5) << right << predicate1;
          for(int i=0; i<arity(solver,pMappedVarId,predicate1); i++) 
             proofTxt << setw(4) << right <<  arg[0][i];
          proofTxt << setw(5) << right << predicate2;
          for(int i=0;i<arity(solver,pMappedVarId,predicate2);i++) 
             proofTxt << setw(4) << right << arg[1][i];
          proofTxt << "   /*** Nesting: " << nesting << "; Step kind:" << axiom << " = MP-axiom:" << axiom << "; Branching: yes; " << "p" << predicate1 << "("; 
          for(int i=0;i<arity(solver,pMappedVarId,predicate1);i++) 
             proofTxt << i2name(arg[0][i]) << (i+1<arity(solver,pMappedVarId,predicate1) ? "," : "");
          proofTxt << ") or ";
          proofTxt << "p" << predicate2 << "(";
          for(int i=0;i<arity(solver,pMappedVarId,predicate2);i++) 
             proofTxt << i2name(arg[1][i]) << (i+1<arity(solver,pMappedVarId,predicate2) ? "," : "");
          proofTxt << ") ***/" << endl;

          proofTxt << setw(40) << right << " ";
          for(int i=0; i<numberOfUnivVars; i++) 
              proofTxt << i << " ";
          proofTxt << "/*** Instantiation ***/" << endl;
      } 
      else {
          string sfrom1, sfrom2 = "";

          unsigned noPremises;
          bool branching;
          int numberOfUnivVars;
          int numberOfExiVars;

          s = "nAxiomPremises[" + itos(axiom)+"]";
          noPremises = readProofParameter(solver, pMappedVarId, s);
          s = "bAxiomBranching[" + itos(axiom)+"]";
          branching = readBoolProofParameter(solver, pMappedVarId, s);
          s = "nAxiomUniVars[" + itos(axiom)+"]";     
          numberOfUnivVars = readProofParameter(solver, pMappedVarId, s);
          s = "nAxiomExiVars[" + itos(axiom)+"]";     
          numberOfExiVars = readProofParameter(solver, pMappedVarId, s);

          for (unsigned i = 0; i<noPremises; i++) {  
             s = "nFrom[" + itos(proofStep)+"]["+ itos(i) +"]";
             int from = readProofParameter(solver, pMappedVarId, s);
             if (from == -1)
               assert(false);
             if (from == 99) {
                noPremises = 0;
                sfrom1 += itos(from) + " ";             
                break;
             }
             sfrom1 += itos(from) + " ";
             if (i > 0)
                sfrom2 += " and ";
             sfrom2 += "(" + itos(from) + ")";
          }

          int inst[100];
          for(int i=0; i<numberOfUnivVars+numberOfExiVars; i++) {
              s = "nInst[" + itos(proofStep)+"][" + itos(i+1) + "]";
              inst[i] = readProofParameter(solver, pMappedVarId, s);
          }

          if (!branching) {
             proofTxt << setw(4) << right << nesting << setw(4) << right << axiom << setw(4) << right << "0" << setw(5) << right << predicate1;
             for(int i=0; i<arity(solver,pMappedVarId,predicate1); i++) 
                proofTxt << setw(4) << right <<  arg[0][i];
             proofTxt << "   /*** Nesting: " << nesting << "; Step kind:" << axiom << "=MP-axiom:" << axiom << "); Branching: no; " << "p" << predicate1 << "("; 
             for(int i=0;i<arity(solver,pMappedVarId,predicate1);i++) 
                proofTxt << i2name(arg[0][i]) << (i+1<arity(solver,pMappedVarId,predicate1) ? "," : "");
             proofTxt << ") ***/" << endl;
          }
          else {
              s = "nP[" + itos(proofStep)+"][1]";
              int predicate2 = readProofParameter(solver, pMappedVarId, s);
              for(int i=0; i<arity(solver,pMappedVarId,predicate2); i++) {
                  s = "nA[" + itos(proofStep) +"][" + itos(nMaxArg+i) + "]";
                  arg[1][i] = readProofParameter(solver, pMappedVarId, s);
                  arg[1][i] = arg[1][i] % 1000; // reduce spurious constants to <1000
              }
              proofTxt << setw(4) << right << nesting << setw(4) << right << " " << setw(4) << right << axiom << setw(4) << right << "1" << setw(5) << right << predicate1;
              for(int i=0; i<arity(solver,pMappedVarId,predicate1); i++) 
                 proofTxt << setw(4) << right <<  arg[0][i];
              proofTxt << setw(5) << right << predicate2;
              for(int i=0;i<arity(solver,pMappedVarId,predicate2);i++) 
                 proofTxt << setw(4) << right << arg[1][i];
              proofTxt << "   /*** Nesting: " << nesting << "; Step kind:" << axiom << " = MP-axiom:" << axiom << "; Branching: yes; " << "p" << predicate1 << "("; 
              for(int i=0;i<arity(solver,pMappedVarId,predicate1);i++) 
                 proofTxt << i2name(arg[0][i]) << (i+1<arity(solver,pMappedVarId,predicate1) ? "," : "");
              proofTxt << ") or ";
              proofTxt << "p" << predicate2 << "(";
              for(int i=0;i<arity(solver,pMappedVarId,predicate2);i++) 
                 proofTxt << i2name(arg[1][i]) << (i+1<arity(solver,pMappedVarId,predicate2) ? "," : "");
              proofTxt << ") ***/" << endl;
          }

          proofTxt << setw(40) << right << " ";
          proofTxt << sfrom1 << "  /*** From steps: " << sfrom2 << " ***/" << endl;

          proofTxt << setw(40) << right << " ";
          for(int i=0; i<numberOfUnivVars; i++) 
              proofTxt << inst[i] << " "; 
          for(int i=0; i<numberOfExiVars; i++) 
              proofTxt << inst[numberOfUnivVars+i] << " "; 
          proofTxt << "/*** Instantiation ***/" << endl;
      }
      proofStep++;
  }  

  proofTxt.close();

}




