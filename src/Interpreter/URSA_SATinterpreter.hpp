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

#ifndef __INTERPRETER_H
#define __INTERPRETER_H

#include "ursa.h"
#include "Number.hpp"
#include "Boolean.hpp"
#include "SymbolTable.hpp"
#include "Timer.h"


map<const string, nodeType *, lstr > URSAprocedures; 


class Interpreter {
public:
   Interpreter () { m_Timer.StartMeasuringTime(); pOptimizationConstraint=NULL; };
   ~Interpreter () { m_ST.Clear(); ClearProgram(); };

   int ExecuteCommand(nodeType *p);
   void RecordCommand(nodeType *p);
   void RecordProcedure(nodeType *p);

private:
   vector<nodeType *> URSAprogram;

   bool ExecuteProcedure(nodeType *p);

   void ClearCommand(nodeType *p);
   void ClearProgram();
   bool Solve(nodeType *p, bool bAllSolutions);
   bool SolveConstraint(nodeType *p, bool bAllSolutions);
   void SolveOptimizationProblem(nodeType *p);

   void PrintProcedure();
   void PrintCommand(nodeType *p);
   void ExecuteCommandTree(nodeType *p);

   // for optimization problems 
   void setLimits(nodeType *p, const Number& min, const Number& max, bool bMax);
   unsigned int nMin, nMax, nOptimalCandidate;
   bool bMaximize; 
   string sOptimizationVarName;
   nodeType *pOptimizationConstraint;

   Number ReadNumber(nodeType *p);
   Boolean ReadBoolean(nodeType *p);
   bool IsNumberId(nodeType *p);
   bool IsBooleanId(nodeType *p);

   SymbolTable m_ST;

   CTimer m_Timer;

};


#endif


