/************************************************************************************
URSA -- Copyright (c) 2010-2020, Predrag Janicic

This file is part of URSA
 
URSA is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
 
URSA is WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
**************************************************************************************/

#include <iostream>
#include "URSA_SATinterpreter.hpp"
#include "ursa.tab.hpp"
#include "FormulaFactory.h"
#include "SATsolver.h"
#include "ArgoSATSolver.h"
#include "ClaspSATSolver.h"
#include "MiniSATSolver.h"


using namespace std;

typedef enum { eArgoSAT, eClasp, eMiniSAT } eSolvers;
eSolvers URSASolver;
unsigned int iAbstractNumberLength;
bool bQuiet;
bool bDimacsOnly;
bool bMapping;
bool bCoherentLogicProofExport;
Interpreter in;

unsigned int iVarCounter;

extern int yyparse ();

void ClearCommand(nodeType *p) {
    int i;

    if (!p) return;
    if(p->type==typeId) 
      free(p->id.i);

    if (p->type == typeOpr) 
      for (i = 0; i < p->opr.nops; i++) 
        ClearCommand(p->opr.op[i]);

    free(p);
}


// ----------------------------------------------------------------------------


int main(int argc, char** argv) {
    int i, len;
    cout << "************************************************" << endl;
    cout << "****  URSA Interpreter v4.00 (c) 2010-2020  ****" << endl;
    cout << "*** Predrag Janicic,  University of Belgrade ***" << endl;
    cout << "************************************************" << endl << endl;
    
    iAbstractNumberLength=8;
    bQuiet=false;
    bDimacsOnly=false;
    bCoherentLogicProofExport=false;
    bMapping=false;
    URSASolver = eClasp;

    for(i=1;i<argc;i++) {
      if(argv[i][0]=='-')
         switch(argv[i][1]) {
           case 'l':  if (sscanf(argv[i]+2,"%i",&len) == 1)  
                         iAbstractNumberLength = len;
                      else {
                         cout << "A number after the -l option expected." << endl << endl;
                         return false;
                      }
                      break;
           case 's':  {
                      char *p;
                      for (p=argv[i]; *p; p++ ) 
                        *p = tolower(*p);
                      if(!strcmp(argv[i]+2,"argosat"))
                        URSASolver = eArgoSAT;
                      if(!strcmp(argv[i]+2,"minisat"))
                        URSASolver = eMiniSAT;
                      break;
                      }
           case 'd':  bDimacsOnly=true; break;
           case 'q':  bQuiet=true; break;
           case 'c':  bCoherentLogicProofExport=true; break;
           case 'm':  bMapping=true; break;
           case 'h':  
                      cout << "Usage: ./ursa [OPTIONS] ..." << endl << endl;
                      cout << "Solves specified problems by reducing them to SAT." << endl << endl;
                      cout << "Options:" << endl;
                      cout << "-l - sets the number of bits that represent numbers (e.g., -l10; default value is 8)" << endl;
                      cout << "-d - DIMACS output only" << endl;
                      cout << "-q - quite mode (models are not printed out)" << endl;
                      cout << "-m - prints mapping between URSA variables and SAT variables" << endl;
                      cout << "-s - selects an underlying solvers (e.g., -sargosat, -sclasp, -sminisat; defaulf is clasp)" << endl << endl;
                      cout << "Example:" << endl;
                      cout << "./ursa -l10 < examples/CSP/queens.urs" << endl;
           default :  break;
         } 
    }

    iVarCounter=0;
    // yydebug=1;
    yyparse();

    map<const string, nodeType *, lstr >::iterator it;
    for(it=URSAprocedures.begin();it!=URSAprocedures.end();it++)
      ClearCommand(it->second);
    URSAprocedures.clear();

    return 0;
}


// ----------------------------------------------------------------------------


int ex(nodeType *p) {
   in.RecordCommand(p);
   return in.ExecuteCommand(p); 
}


// ----------------------------------------------------------------------------


bool Interpreter::IsNumberId(nodeType *p)  {
  return ((p->type==typeId) && (*(p->id.i)=='n'))||
         ((p->opr.oper=='@') && (*(p->opr.op[0]->id.i)=='n'));
}


// ----------------------------------------------------------------------------


bool Interpreter::IsBooleanId(nodeType *p)  {
  return ((p->type==typeId) && (*(p->id.i)=='b'))||
         ((p->opr.oper=='@') && (*(p->opr.op[0]->id.i)=='b'));
}


// ----------------------------------------------------------------------------


int Interpreter::ExecuteCommand(nodeType *p) {
    if (!p) { return -1; }
    if(p->type!=typeOpr)
      return -1;

    switch(p->opr.oper) {
      case CALL:      ExecuteProcedure(p);
                      return 0;

      case LIST:      m_ST.printSym();
                      return 0;

      case CLEAR:     m_ST.Clear();
                      ClearProgram();
                      return 0;

      case HALT:      m_ST.Clear();
                      ClearProgram();
                      cout << "--> Ending session" << endl << endl;
                      exit(0);

      case FOR:       ExecuteCommand(p->opr.op[0]);
                      while(ReadBoolean(p->opr.op[1]).GetGroundValue()) {
                         ExecuteCommand(p->opr.op[3]); 
                         ExecuteCommand(p->opr.op[2]);  
                      }
                      return 0;

      case WHILE:     while(ReadBoolean(p->opr.op[0]).GetGroundValue()) 
                         ExecuteCommand(p->opr.op[1]); 
                      return 0;

      case IF:        if (ReadBoolean(p->opr.op[0]).GetGroundValue())
                          return ExecuteCommand(p->opr.op[1]);
                      else { 
                        if (p->opr.nops > 2)
                          return ExecuteCommand(p->opr.op[2]);
                        else 
                          return 0;
                      }

      case MINIMIZE:  { 
                        if (pOptimizationConstraint==NULL) 
                          setLimits(p, ReadNumber(p->opr.op[1]), ReadNumber(p->opr.op[2]), false); 
                        else {
                          if (pOptimizationConstraint!=p) {
                            cout << "Only the first optimization constraint is considered " << endl;
                            return 0;
                          }
                        }
                        Number n((unsigned int)nOptimalCandidate);
                        m_ST.letInt(sOptimizationVarName, n);
                      }
                      return 0;

      case MAXIMIZE:  { 
                        if (pOptimizationConstraint==NULL) 
                          setLimits(p, ReadNumber(p->opr.op[1]), ReadNumber(p->opr.op[2]), true); 
                        else {
                          if (pOptimizationConstraint!=p) {
                            cout << "Only the first optimization constraint is considered " << endl;
                            return 0;
                          }
                        }
                        Number n((unsigned int)nOptimalCandidate);
                        m_ST.letInt(sOptimizationVarName, n);
                      }
                      return 0;

      case ASSERT:    Solve(p->opr.op[0], false);
                      return 0;

      case ASSERTA:   Solve(p->opr.op[0], true);
                      return 0;

      case PRINT:     if (IsNumberId(p->opr.op[0])) 
                         ReadNumber(p->opr.op[0]).print('d');
                      else 
                         ReadBoolean(p->opr.op[0]).print();
                      return 0;

      case PRINTX:    ReadNumber(p->opr.op[0]).print('x');
                      return 0;

      case PRINTB:    ReadNumber(p->opr.op[0]).print('b');
                      return 0;

      case ';':       ExecuteCommand(p->opr.op[0]); 
                      return ExecuteCommand(p->opr.op[1]);

      case '=':       if (p->opr.nops == 2) {
                        if (IsNumberId(p->opr.op[0]))  
                           m_ST.letInt(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]));
                        else
                          m_ST.letBool(p->opr.op[0]->id.i, ReadBoolean(p->opr.op[1])); 
                      }
                      else if (p->opr.nops == 3) {
                        if (IsNumberId(p->opr.op[0])) 
                          m_ST.letIntEl(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]), ReadNumber(p->opr.op[2])); 
                        else
                          m_ST.letBoolEl(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]), ReadBoolean(p->opr.op[2])); 
                      } 
                      else {
                        if (IsNumberId(p->opr.op[0])) 
                          m_ST.letIntEl2(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]), ReadNumber(p->opr.op[2]), ReadNumber(p->opr.op[3])); 
                        else
                          m_ST.letBoolEl2(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]), ReadNumber(p->opr.op[2]), ReadBoolean(p->opr.op[3])); 
                      }
                      return 0;

      case PLUSPLUS:
                      { 
                        Number n((unsigned int)0); Number nleft((unsigned int)0); Number nright((unsigned int)0);
                        if (p->opr.nops == 1) 
                          nleft = m_ST.getIntValue(p->opr.op[0]->id.i,&iVarCounter); 
                        else if (p->opr.nops == 2) 
                          nleft = m_ST.getIntElValue(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]),&iVarCounter); 
                        else 
                          nleft = m_ST.getIntElValue2(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]), ReadNumber(p->opr.op[2]),&iVarCounter); 
                        nright=1;
                        n = nleft + nright; 
                        if (p->opr.nops == 1) 
                          m_ST.letInt(p->opr.op[0]->id.i, n); 
                        else if (p->opr.nops == 2) 
                          m_ST.letIntEl(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]), n); 
                        else 
                          m_ST.letIntEl2(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]), ReadNumber(p->opr.op[2]), n); 
                      }
                      return 0;

      case MINUSMINUS:
                      { 
                        Number n((unsigned int)0); Number nleft((unsigned int)0); Number nright((unsigned int)0);
                        if (p->opr.nops == 1) 
                          nleft = m_ST.getIntValue(p->opr.op[0]->id.i,&iVarCounter); 
                        else if (p->opr.nops == 2) 
                          nleft = m_ST.getIntElValue(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]),&iVarCounter); 
                        else 
                          nleft = m_ST.getIntElValue2(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]), ReadNumber(p->opr.op[2]),&iVarCounter); 
                        nright=1;
                        n = nleft - nright; 
                        if (p->opr.nops == 1) 
                          m_ST.letInt(p->opr.op[0]->id.i, n); 
                        else if (p->opr.nops == 2) 
                          m_ST.letIntEl(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]), n); 
                        else 
                          m_ST.letIntEl2(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]), ReadNumber(p->opr.op[2]), n); 
                      }
                      return 0;

      case PLUSEQ: 
      case MINUSEQ:
      case MULTEQ:
      case BITWISEANDEQ: 
      case BITWISEOREQ: 
      case BITWISEXOREQ:
      case LSHIFTEQ:  
      case RSHIFTEQ:    
                   {     
                      Number n((unsigned int)0); Number nleft((unsigned int)0);
                      Boolean b(false),bleft(false);
                      if (p->opr.nops == 2) 
                          nleft=m_ST.getIntValue(p->opr.op[0]->id.i,&iVarCounter); 
                      else if (p->opr.nops == 3) 
                          nleft = m_ST.getIntElValue(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]),&iVarCounter); 
                      else 
                          nleft = m_ST.getIntElValue2(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]), ReadNumber(p->opr.op[2]),&iVarCounter); 

                      switch(p->opr.oper) {
                        case PLUSEQ:   n = nleft + ReadNumber(p->opr.op[p->opr.nops-1]); break;
                        case MINUSEQ:  n = nleft - ReadNumber(p->opr.op[p->opr.nops-1]); break;
                        case MULTEQ:   n = nleft * ReadNumber(p->opr.op[p->opr.nops-1]); break;
//                      case DIVEQ:    n = nleft + ReadNumber(p->opr.op[p->opr.nops-1]); break;
                        case BITWISEANDEQ:  n = nleft & ReadNumber(p->opr.op[p->opr.nops-1]); break;
                        case BITWISEOREQ:   n = nleft | ReadNumber(p->opr.op[p->opr.nops-1]); break;
                        case BITWISEXOREQ:  n = nleft ^ ReadNumber(p->opr.op[p->opr.nops-1]); break;
                        case LSHIFTEQ: n = nleft << ReadNumber(p->opr.op[p->opr.nops-1]); break;
                        case RSHIFTEQ: n = nleft >> ReadNumber(p->opr.op[p->opr.nops-1]); break;
                      }

                      if (p->opr.nops == 2) 
                          m_ST.letInt(p->opr.op[0]->id.i, n); 
                      else if (p->opr.nops == 3) 
                          m_ST.letIntEl(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]), n); 
                      else 
                          m_ST.letIntEl2(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]), ReadNumber(p->opr.op[2]), n); 
              
                      return 0;
                   }

      case ANDEQ: 
      case OREQ: 
      case XOREQ:
                   {     
                      Number n((unsigned int)0); Number nleft((unsigned int)0);
                      Boolean b(false),bleft(false);
                      if (p->opr.nops == 2) 
                          bleft=m_ST.getBoolValue(p->opr.op[0]->id.i,&iVarCounter); 
                      else if (p->opr.nops == 3) 
                          bleft = m_ST.getBoolElValue(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]),&iVarCounter); 
                      else 
                          bleft = m_ST.getBoolElValue2(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]), ReadNumber(p->opr.op[2]),&iVarCounter); 

                      switch(p->opr.oper) {
                        case ANDEQ:   b = bleft & ReadBoolean(p->opr.op[p->opr.nops-1]); break;
                        case OREQ:    b = bleft | ReadBoolean(p->opr.op[p->opr.nops-1]); break;
                        case XOREQ:   b = bleft ^ ReadBoolean(p->opr.op[p->opr.nops-1]); break;
                      }

 	                    if (p->opr.nops == 2) 
                        m_ST.letBool(p->opr.op[0]->id.i, b); 
                      else if (p->opr.nops == 3) 
                        m_ST.letBoolEl(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]), b); 
     	                else 
                        m_ST.letBoolEl2(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]), ReadNumber(p->opr.op[2]), b); 
              
                      return 0;
                   }

       default: break;
    }

    return 0;
}


// ----------------------------------------------------------------------------


Number Interpreter::ReadNumber(nodeType *p) {
    if (!p) { Number n((unsigned int)0); return n; }   
    switch(p->type) {

    case typeIntConst:  { Number n(p->intConst.value); 
                        return n; 
                        }

    case typeId:        return m_ST.getIntValue(p->id.i,&iVarCounter);

    case typeOpr:
        switch(p->opr.oper) {
        case '@':       if(p->opr.nops==2) {
                          return m_ST.getIntElValue(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]),&iVarCounter); 
                        }
                        else 
                          return m_ST.getIntElValue2(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]), ReadNumber(p->opr.op[2]),&iVarCounter); 
        case UMINUS:    return ReadNumber(p->opr.op[0]).negate();
        case '+':       return ReadNumber(p->opr.op[0]) + ReadNumber(p->opr.op[1]);
        case '-':       return ReadNumber(p->opr.op[0]) - ReadNumber(p->opr.op[1]);
        case '*':       return ReadNumber(p->opr.op[0]) * ReadNumber(p->opr.op[1]);
//        case '/':       return ReadNumber(p->opr.op[0]) / ReadNumber(p->opr.op[1]);
        case '&':       return ReadNumber(p->opr.op[0]) & ReadNumber(p->opr.op[1]);
        case '|':       return ReadNumber(p->opr.op[0]) | ReadNumber(p->opr.op[1]);
        case '^':       return ReadNumber(p->opr.op[0]) ^ ReadNumber(p->opr.op[1]);
        case '~':       return ReadNumber(p->opr.op[0]).bitnegate();
        case LSHIFT:    return ReadNumber(p->opr.op[0]) << ReadNumber(p->opr.op[1]);
        case RSHIFT:    return ReadNumber(p->opr.op[0]) >> ReadNumber(p->opr.op[1]);
        case ITE:       return ReadNumber(p->opr.op[1]).ite(ReadBoolean(p->opr.op[0]),ReadNumber(p->opr.op[2]));
        case BOOL2NUM:  return ReadBoolean(p->opr.op[0]).Int();
        case SGN:       return ReadNumber(p->opr.op[0]).sgn();
        }
    default: break;
    }
    return (unsigned int)0;
}


// ----------------------------------------------------------------------------


Boolean Interpreter::ReadBoolean(nodeType *p) {
    if (!p) { Boolean b(false); return b; } 
    switch(p->type) {
    case typeBoolConst: { Boolean b(p->boolConst.value); return b; };
    case typeId:        return m_ST.getBoolValue(p->id.i,&iVarCounter);
    case typeOpr:
        switch(p->opr.oper) {
        case '@':       if(p->opr.nops==2)
                          return m_ST.getBoolElValue(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]),&iVarCounter); 
                        else 
                          return m_ST.getBoolElValue2(p->opr.op[0]->id.i, ReadNumber(p->opr.op[1]), ReadNumber(p->opr.op[2]),&iVarCounter); 
        case LOGICALAND:return ReadBoolean(p->opr.op[0]) & ReadBoolean(p->opr.op[1]);
        case LOGICALOR: return ReadBoolean(p->opr.op[0]) | ReadBoolean(p->opr.op[1]);
        case LOGICALXOR:return ReadBoolean(p->opr.op[0]) ^ ReadBoolean(p->opr.op[1]);
        case '!':       return ReadBoolean(p->opr.op[0]).negate();
        case '<':       return ReadNumber(p->opr.op[0]) < ReadNumber(p->opr.op[1]);        
        case '>':       return ReadNumber(p->opr.op[0]) > ReadNumber(p->opr.op[1]);   
        case  LE:       return ReadNumber(p->opr.op[0]) <= ReadNumber(p->opr.op[1]);   
        case  GE:       return ReadNumber(p->opr.op[0]) >= ReadNumber(p->opr.op[1]);   
        case  EQ:       return ReadNumber(p->opr.op[0]) == ReadNumber(p->opr.op[1]);   
        case  NE:       return ReadNumber(p->opr.op[0]) != ReadNumber(p->opr.op[1]);   
        case ITE:       return ReadBoolean(p->opr.op[1]).ite(ReadBoolean(p->opr.op[0]),ReadBoolean(p->opr.op[2]));
        case NUM2BOOL:  return ReadNumber(p->opr.op[0]).Bool();
        }
    default: break;  
    }
    return false;
}  


// ----------------------------------------------------------------------------


bool Interpreter::Solve(nodeType *p, bool bAllSolutions) {
   if (pOptimizationConstraint) 
     SolveOptimizationProblem(p);
   else
     SolveConstraint(p, bAllSolutions);
   return true;
}



// ----------------------------------------------------------------------------


void Interpreter::setLimits(nodeType *p, const Number& min, const Number& max, bool bMax) {
   assert ( min.IsGroundNumber() );
   assert ( max.IsGroundNumber() );
   sOptimizationVarName = p->opr.op[0]->id.i;
   nMin=min.GetGroundValueUnsigned();
   nMax=max.GetGroundValueUnsigned();
   bMaximize = bMax;
   nOptimalCandidate = bMax ? nMax : nMin;
   pOptimizationConstraint=p;
}


// ----------------------------------------------------------------------------


void Interpreter::SolveOptimizationProblem(nodeType *p) {
   CTimer Timer;
   Timer.StartMeasuringTime();

        unsigned best = 0;
        while(nMin <= nMax)  {
            cout << "Testing the value " << nMin << " for the variable " << sOptimizationVarName << " ..." << endl;
            if(SolveConstraint(p,false)) {
                best = bMaximize ? nMax : nMin;
                break;
            }
            else {
                if (bMaximize) 
                    nMax /= 2;
                else
                    nMin *= 2;
                m_ST.Clear();
                FormulaFactory::Instance()->Clear();
                nOptimalCandidate = bMaximize ? nMax : nMin;
                m_ST.letInt(sOptimizationVarName, nOptimalCandidate);
                vector<nodeType *>::iterator i;
                for(i=URSAprogram.begin();i+1!=URSAprogram.end();i++)  //execute until the assert command
                    ExecuteCommand(*i); 
            }
        }

        if (bMaximize) {
            nMin = best; nMax = best*2-1;
        }   
        else { 
            nMin = best/2+1; nMax = best;
        }   

        while(best && nMin <= nMax && ((nMin != best && !bMaximize)||(nMax != best && bMaximize)))  {
            m_ST.Clear();
            FormulaFactory::Instance()->Clear();
            nOptimalCandidate = (nMin+nMax)/2;
            m_ST.letInt(sOptimizationVarName, nOptimalCandidate);
            cout << "Testing the value " << nOptimalCandidate << " for the variable " << sOptimizationVarName << " ..." << endl;
            vector<nodeType *>::iterator i;
            for(i=URSAprogram.begin();i+1!=URSAprogram.end();i++)  //execute until the assert command
                ExecuteCommand(*i); 
            if(SolveConstraint(p,false)) {
                best = nOptimalCandidate;
                nMax = nOptimalCandidate-1;
            }
            else {
                nMin = nOptimalCandidate+1;
            }
        }
        cout << endl;
        if (best > 0) {
            cout << "Best found proof of the length " << best << endl;
            m_ST.Clear();
            FormulaFactory::Instance()->Clear();
            nOptimalCandidate = best;
            m_ST.letInt(sOptimizationVarName, best);
            vector<nodeType *>::iterator i;
            for(i=URSAprogram.begin();i+1!=URSAprogram.end();i++)  //execute until the assert command
                 ExecuteCommand(*i); 
        }
    

   /* binary search */
   while(1 && false) {
     cout << "Testing the value " << (nMin + nMax)/2 << " for the variable " << sOptimizationVarName << " ..." << endl;
     if(SolveConstraint(p,false)) {
       if(nMax<=nMin) {
         if (bMaximize)
           cout << "Maximal value for the variable " << sOptimizationVarName << " is " << (nMin + nMax)/2 << endl;
         else 
           cout << "Minimal value for the variable " << sOptimizationVarName << " is " << (nMin + nMax)/2 << endl;
         break; 
       }
       else {
         m_ST.Clear();
         FormulaFactory::Instance()->Clear();

         if (bMaximize)
           nMin = (nMin + nMax + 1)/2;
         else 
           nMax = (nMin + nMax)/2;
 
         Number n((unsigned int)((nMin+nMax)/2));
         m_ST.letInt(sOptimizationVarName, n);

         vector<nodeType *>::iterator i;
         for(i=URSAprogram.begin();i+1!=URSAprogram.end();i++)  //execute until the assert command
           ExecuteCommand(*i); 
       }
     }
     else {
       if(nMax<=nMin) {
         cout << "The constraint is not met for any admissible value for the variable " << sOptimizationVarName << endl;
         break;
       }

       m_ST.Clear();
       FormulaFactory::Instance()->Clear();

       if (bMaximize)
         nMax = (nMin + nMax)/2;
       else 
         nMin = (nMin + nMax + 1)/2;

       Number n((unsigned int)((nMin+nMax)/2));
       m_ST.letInt(sOptimizationVarName, n);

       vector<nodeType *>::iterator i;
       for(i=URSAprogram.begin();i+1!=URSAprogram.end();i++)  //execute until the assert command
         ExecuteCommand(*i); 
     }
   } 

   
   /* sequential search 
   while(1) {
     cout << "Testing the value " << (bMaximize ? nMax : nMin) << " for the variable " << sOptimizationVarName << " ..." << endl;
     if(SolveConstraint(p,false)) {
       if (bMaximize)
         cout << "Maximal value for the variable " << sOptimizationVarName << " is " << nMax << endl;
       else 
         cout << "Minimal value for the variable " << sOptimizationVarName << " is " << nMin << endl;
       break; 
     }
     else {
       if(nMax<=nMin) {
         cout << "The constraint is not met for any admissible value for the variable " << sOptimizationVarName << endl;
         break;
       }

       m_ST.Clear();
       FormulaFactory::Instance()->Clear();

       if (bMaximize) {
         nMax--;
         Number n((unsigned int)nMax);
         m_ST.letInt(sOptimizationVarName, n);
       }
       else {
         nMin++;
         Number n((unsigned int)nMin);
         m_ST.letInt(sOptimizationVarName, n);
       }

       vector<nodeType *>::iterator i;
       for(i=URSAprogram.begin();i+1!=URSAprogram.end();i++)  //execute until the assert command
         ExecuteCommand(*i); 
     }
   } */

   cout << "[Total time elapsed: " << Timer.ElapsedTime() << "]" << endl << endl;
}


// ----------------------------------------------------------------------------


bool Interpreter::SolveConstraint(nodeType *p, bool bAllSolutions) {
  
  double dTime_generation, dTime_solving;
  double dTime_parsing = m_Timer.ElapsedTime();
  m_Timer.StartMeasuringTime();

  Boolean bConstraint(true);

  bool bMoreConstraints = true;
  while(bMoreConstraints)  {
    if(p->opr.oper==';') {
      bConstraint = bConstraint & ReadBoolean(p->opr.op[1]); 
      bMoreConstraints = (p->opr.oper==';');
      p = p->opr.op[0];
    }
    else {
      bConstraint = bConstraint & ReadBoolean(p);
      bMoreConstraints=false;
    }
  }

  // cout << "--> constraint: " << endl;
  // bConstraint.print();

   Formula* root=bConstraint.GetAbstractValue().getFormulaFrom(0);
   if (root->GetType() == CONST) {
       if (((FormulaConst*)root)->GetValue() == true) {
        cout << "yes" << endl;
        return true;
      }
      else
      {
        cout << "no" << endl;
        cout << "[Number of solutions: 0]" << endl;
        return false;
      }
   }

  if (bConstraint.IsGroundBoolean()) { 
      if (bConstraint.GetGroundValue()==true) {
      cout << "yes" << endl;
      return true;
    }
    else {
      cout << "no" << endl;
      cout << "[Number of solutions: 0]" << endl;
      return false;
    }
  }

  bConstraint.SetConstraint(true);
  int varCount = iVarCounter;

  FormulaFactory::Instance()->SetIds(&varCount);
  vector< vector<int> > conj;
  int numOfModels = 0;
   
  unsigned int* pMappedVarId = new unsigned int[varCount+1];

  if(bMapping) {
    m_ST.printMapping();
  }
 
  if (!FormulaFactory::Instance()->GenerateCNF(bConstraint.GetAbstractValue(), conj, pMappedVarId, &varCount))  {
    cout << endl << "No solutions found" << endl;
    dTime_generation = m_Timer.ElapsedTime();
    m_Timer.StartMeasuringTime();
  } 
  else
  { 
    if(bDimacsOnly) {
      dTime_generation = m_Timer.ElapsedTime();
      m_Timer.StartMeasuringTime();

      cout << "p cnf " << varCount << " " << conj.size() << " " << endl;
      for (vector<vector<int> >::iterator iter = conj.begin(); iter != conj.end(); iter++) {
        for (vector<int>::iterator it = iter->begin(); it != iter->end(); it++) 
          cout << *it << " ";                              
        cout << "0" << endl;
      }
    }
    else  {

      SATsolver *pSolver;
      if (URSASolver == eArgoSAT)
        pSolver = new ArgoSATsolver;
      else if (URSASolver == eMiniSAT)
        pSolver = new MiniSATsolver;
      else
        pSolver = new ClaspSATsolver;

      pSolver->InitSolver(varCount);
        
      for (vector<vector<int> >::iterator iter = conj.begin(); iter != conj.end(); iter++) 
        pSolver->addClause(*iter);

      dTime_generation = m_Timer.ElapsedTime();
      m_Timer.StartMeasuringTime();

      while(true) {
        if (!pSolver->solve()) {
          if(!numOfModels)
            cout << "No solutions found" << endl;
          break;
        }

        ++numOfModels;
        if (bAllSolutions)
          cout << "--> Solution " << numOfModels << endl;

        if (!bQuiet) 
          m_ST.printIndependentMapped(pSolver, pMappedVarId);

        if (bCoherentLogicProofExport) 
            m_ST.exportCoherentLogicProof2Txt(pSolver, pMappedVarId);

        cout << endl;

        if (!bAllSolutions)
          break;
      }
      delete pSolver;
    }
  }
 
  delete [] pMappedVarId;

  dTime_solving = m_Timer.ElapsedTime();
  cout << "[Formula generation: " << dTime_parsing << "s; conversion to CNF: "<< dTime_generation << "s; total: " << dTime_parsing+dTime_generation << "s]" << endl; 
  cout << "[Solving time: " << dTime_solving << "s]" << endl;
  cout << "[Formula size: " << varCount << " variables, " << conj.size() << " clauses]" << endl << endl;

  return (numOfModels>0);
}


// ----------------------------------------------------------------------------


void Interpreter::RecordProcedure(nodeType *p) {
   // cout << "Procedure " << p->opr.op[0]->id.i << endl;
   URSAprocedures[p->opr.op[0]->id.i]=p;
}

// ----------------------------------------------------------------------------


void Interpreter::RecordCommand(nodeType *p) {
   URSAprogram.push_back(p);
}


// ----------------------------------------------------------------------------


void Interpreter::ClearCommand(nodeType *p) {
    int i;

    if (!p) return;
    if(p->type==typeId) 
      free(p->id.i);

    if (p->type == typeOpr) 
      for (i = 0; i < p->opr.nops; i++) 
        ClearCommand(p->opr.op[i]);

    free(p);
}


// ----------------------------------------------------------------------------


void Interpreter::ClearProgram() {
   vector<nodeType *>::iterator i;
   for(i=URSAprogram.begin();i!=URSAprogram.end();i++)
     ClearCommand(*i);
   URSAprogram.clear();

   pOptimizationConstraint=NULL;
}



// ----------------------------------------------------------------------------


void Interpreter::PrintProcedure() {
   map<const string, nodeType *, lstr >::iterator i;
   for(i=URSAprocedures.begin();i!=URSAprocedures.end();i++)
     PrintCommand(i->second);

}


// ----------------------------------------------------------------------------


void Interpreter::PrintCommand(nodeType *p) {
    int i;

    if (!p) return;
    if(p->type==typeId) 
      cout << p->id.i << " ";

    if(p->type==typeIntConst) 
      cout << p->intConst.value << " ";

    if(p->type==typeBoolConst) 
      cout << p->boolConst.value << " ";

    if (p->type == typeOpr) 
      for (i = 0; i < p->opr.nops; i++) 
        PrintCommand(p->opr.op[i]);
    cout << endl;
}



// ----------------------------------------------------------------------------


bool Interpreter::ExecuteProcedure(nodeType *p) {
   map<const string, nodeType *, lstr >::iterator it;
   it=URSAprocedures.find(p->opr.op[0]->id.i);
   if(it == URSAprocedures.end())  {
      cout << "Procedure " << p->opr.op[0]->id.i << "not defined" << endl; 
      return false;
   }      
//   cout << "Procedure " << p->opr.op[0]->id.i << " called " << endl; 

   // Copying arguments 
   nodeType *pargs=p->opr.op[1];
   nodeType *pdefargs=it->second->opr.op[1];
   Interpreter procInter;
   bool bMoreArgs = true;
   
   int actualargs=0;
   while(pargs->opr.oper==';')  {
     pargs = pargs->opr.op[0];
     actualargs++; 
   }  

   int defargs=0;
   while(pdefargs->opr.oper==';')  {
     pdefargs = pdefargs->opr.op[0];
     defargs++; 
   }  

   if(actualargs!=defargs) {
     cout << "Wrong number of arguments in the call of the procedure " << p->opr.op[0]->id.i << endl; 
     exit(1);
   }

   pargs=p->opr.op[1];
   pdefargs=it->second->opr.op[1];

   while(bMoreArgs)  {
     if(pargs->opr.oper==';') {
       if(IsNumberId(pdefargs->opr.op[1]))  {

         if (!IsNumberId(pargs->opr.op[1]) || m_ST.DefinedIntVar(pargs->opr.op[1]->id.i))
             procInter.m_ST.letInt(pdefargs->opr.op[1]->id.i, ReadNumber(pargs->opr.op[1]));
/*         else 
             procInter.m_ST.letInt(pdefargs->opr.op[1]->id.i, Number((unsigned int)0));*/
          procInter.m_ST.SetAccessedIntVar(pdefargs->opr.op[1]->id.i,false);
         
       }
       else {
         if (!IsBooleanId(pargs->opr.op[1]) || m_ST.DefinedBoolVar(pargs->opr.op[1]->id.i))
           procInter.m_ST.letBool(pdefargs->opr.op[1]->id.i, ReadBoolean(pargs->opr.op[1]));
/*         else 
           procInter.m_ST.letBool(pdefargs->opr.op[1]->id.i, Boolean(false));*/
         procInter.m_ST.SetAccessedBoolVar(pdefargs->opr.op[1]->id.i,false);
       }
       bMoreArgs = (pargs->opr.oper==';');
       pargs = pargs->opr.op[0];
       pdefargs = pdefargs->opr.op[0];
     }
     else {
       if(IsNumberId(pdefargs))  {

         if (!IsNumberId(pargs) || m_ST.DefinedIntVar(pargs->id.i)) 
           procInter.m_ST.letInt(pdefargs->id.i, ReadNumber(pargs));
/*         else 
           procInter.m_ST.letInt(pdefargs->id.i, Number((unsigned int)0));*/
         procInter.m_ST.SetAccessedIntVar(pdefargs->id.i,false);
       }
       else {
         if (!IsBooleanId(pargs) || m_ST.DefinedBoolVar(pargs->id.i))
           procInter.m_ST.letBool(pdefargs->id.i, ReadBoolean(pargs));
 /*        else
           procInter.m_ST.letBool(pdefargs->id.i, Boolean(false));*/
         procInter.m_ST.SetAccessedBoolVar(pdefargs->id.i,false);
       }

       bMoreArgs=false;
     }
   }

   // Executing procedure
   nodeType *pcode=it->second->opr.op[2];
   procInter.ExecuteCommandTree(pcode);

   // Copying arguments back
   pargs=p->opr.op[1];
   pdefargs=it->second->opr.op[1];

   bMoreArgs = true;
   while(bMoreArgs)  {
     if(pargs->opr.oper==';') {      
       if(IsNumberId(pargs->opr.op[1]))  {

         if (!procInter.m_ST.GetAccessedIntVar(pdefargs->opr.op[1]->id.i) && !m_ST.DefinedIntVar(pargs->opr.op[1]->id.i))
             procInter.m_ST.letInt(pdefargs->opr.op[1]->id.i, Number((unsigned int)0));

         m_ST.letInt(pargs->opr.op[1]->id.i, procInter.ReadNumber(pdefargs->opr.op[1]));
       }
       else 
         if(IsBooleanId(pargs->opr.op[1])) {

          if (!procInter.m_ST.GetAccessedBoolVar(pdefargs->opr.op[1]->id.i) && !m_ST.DefinedBoolVar(pargs->opr.op[1]->id.i))
             procInter.m_ST.letBool(pdefargs->opr.op[1]->id.i, Boolean(false));

          m_ST.letBool(pargs->opr.op[1]->id.i, procInter.ReadBoolean(pdefargs->opr.op[1]));

       }

       bMoreArgs = (pargs->opr.oper==';');
       pargs = pargs->opr.op[0];
       pdefargs = pdefargs->opr.op[0];
     }
     else {
       if(IsNumberId(pargs))  {

         if (!procInter.m_ST.GetAccessedIntVar(pdefargs->id.i) && !m_ST.DefinedIntVar(pargs->id.i))
             procInter.m_ST.letInt(pdefargs->id.i, Number((unsigned int)0));

         m_ST.letInt(pargs->id.i, procInter.ReadNumber(pdefargs));

       }
       else  
         if(IsBooleanId(pargs)) {

           if (!procInter.m_ST.GetAccessedBoolVar(pdefargs->id.i) && !m_ST.DefinedBoolVar(pargs->id.i))
              procInter.m_ST.letBool(pdefargs->id.i, Boolean(false));

            m_ST.letBool(pargs->id.i, procInter.ReadBoolean(pdefargs));
         }

       bMoreArgs=false;
     }
   }

   return true;
}


// ----------------------------------------------------------------------------


void Interpreter::ExecuteCommandTree(nodeType *p) {
     if(p->opr.oper==';') {
       ExecuteCommandTree(p->opr.op[0]);
       ExecuteCommand(p->opr.op[1]);
     }       
     else  
       ExecuteCommand(p);
}


// ----------------------------------------------------------------------------


int store_procedure(nodeType *p) {
   in.RecordProcedure(p);
   return 0; 
}



// ----------------------------------------------------------------------------



