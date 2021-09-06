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

#include "FormulaFactory.h"
#include <cstdlib>
#include <limits.h>

////////////////////////////////////////////////////////////////////////////////
////	Class FormulaFactory
////////////////////////////////////////////////////////////////////////////////

FormulaFactory *FormulaFactory::formulaFactory = 0;


// ----------------------------------------------------------------------------


  void FormulaFactory::print(void) {

    FormulaPointerSet::iterator b = begin(), e = end(), i;
    int k = 0;
    for (i = b; i != e; i++, k++) {
      Formula *f = *i;
      if (f && f->GetRefCount() == 0) {
        cout << endl << k + 1 << ". RC = " << f->GetRefCount() << " " << f->GetType() << endl;
        f->print();
      }
    }
  }



// ----------------------------------------------------------------------------



  Formula* FormulaFactory::Get(Formula *f) {

    FormulaPointerSet::const_iterator i = existingFormulas.find(f);

    if ( i != existingFormulas.end() ) {
      if (*i != f) {
        Formula::DEL++;
      	delete f;
      }
      return *i;
    }
    else {
      existingFormulas.insert(f);
      return f;
    }
  }



// ----------------------------------------------------------------------------



  void FormulaFactory::Remove(Formula *f) {

    //		assert(f != 0);

    if (f->GetRefCount())
      return;

    //		assert(existingFormulas.find(f) != existingFormulas.end());
    if (existingFormulas.find(f) != existingFormulas.end())
      existingFormulas.erase(f);

    delete f;
    Formula::DEL++;
  }



// ----------------------------------------------------------------------------



  void FormulaFactory::SetIds(int *varCount) {

    FormulaPointerSet::iterator iter;
    for (iter = begin(); iter != end(); iter++)
      if ((*iter)->GetType() == VAR) {
        (*iter)->SetId(((FormulaVar*)(*iter))->GetOrdinal());
      }
      else if ((*iter)->GetType() != CONST) {
        (*iter)->SetId(++(*varCount));
      }
  }


// ----------------------------------------------------------------------------


  // f is either T or NT or does not includes nor T neither NT
  bool FormulaFactory::GenerateCNF(FormulaVector constraint, vector<vector<int> >& conj, unsigned int* pMappedVarId, int* varCount) {
    assert(constraint.getSize()==1);

    Formula* root=constraint.getFormulaFrom(0);

    if (root->GetType() == CONST) {
    	if (((FormulaConst*)root)->GetValue() == false) {
        // cerr << "The constraint is trivially false!" << endl;
        return false;
      }
    }

    vector<Formula*> top_conjuncts;
    GatherConjuncts(root,top_conjuncts);

    vector<int> disj,args;
    vector<vector<int> > conj_tmp;
    vector<Formula*>::iterator jt;
    for (jt = top_conjuncts.begin(); jt != top_conjuncts.end(); jt++) {
      if(IsClause(*jt)) {
        TraverseNaryConnectives(*jt,OR,args,conj_tmp);
        for (unsigned int i=0; i<args.size(); i++)  
          disj.push_back(args[i]);
        conj_tmp.push_back(disj);
        disj.clear();
        args.clear();
      }
      else {
        disj.push_back(((Formula*)*jt)->GetId());
        conj_tmp.push_back(disj);
        disj.clear();
        TraverseFormulaTreeAndGenerateCNF(((Formula*)*jt),conj_tmp);
      }
    }

    FormulaPointerSet::iterator it;
    unsigned int ReducedVars=0;
    for (it = FormulaFactory::Instance()->begin(); it != FormulaFactory::Instance()->end(); it++) {
      Formula *f = *it;
      if (f->IsDefined()) {
        pMappedVarId[f->GetId()]=++ReducedVars; 
        // cout << "var " << f->GetId() << " -> " << ReducedVars << " defined:" << f->IsDefined() << endl; 
      }
    }
    *varCount = ReducedVars;

    vector<int> clause; 
    int mappedVar;   
    for (vector<vector<int> >::iterator iter_c = conj_tmp.begin(); iter_c != conj_tmp.end(); iter_c++) {
      for (vector<int>::iterator iter_d = iter_c->begin(); iter_d != iter_c->end(); iter_d++) {
        mappedVar = ( *iter_d>0 ? pMappedVarId[*iter_d] : -pMappedVarId[-(*iter_d)]);
        clause.push_back(mappedVar);
      }
      conj.push_back(clause);
      clause.clear();
    }

    return true;
  }


// ----------------------------------------------------------------------------


  void FormulaFactory::GatherConjuncts(Formula* f, vector<Formula*> & args) {
    if(f->GetType()==AND) {
      GatherConjuncts(((FormulaAnd*)f)->GetLeftF(), args);
      GatherConjuncts(((FormulaAnd*)f)->GetRightF(), args);
    }
    else 
      args.push_back(f);
  }


// ----------------------------------------------------------------------------


  void FormulaFactory::TraverseNaryConnectives(Formula* f, FORMULA_TYPE type, vector<int> & args,vector<vector <int> > & conj_tmp) {
    if(f->GetType()==type && type==OR) {
      TraverseNaryConnectives(((FormulaOr*)f)->GetLeftF(), type, args, conj_tmp);
      TraverseNaryConnectives(((FormulaOr*)f)->GetRightF(), type, args, conj_tmp);
      return;
    }
 
    if(f->GetType()==type && type==AND) {
      TraverseNaryConnectives(((FormulaAnd*)f)->GetLeftF(), type, args, conj_tmp);
      TraverseNaryConnectives(((FormulaAnd*)f)->GetRightF(), type, args, conj_tmp);
      return;
    }

    if(f->GetType()==VAR) {
      f->SetDefined();
      args.push_back(f->GetId());
    }
    else if(f->GetType()==NOT) {
      int level;
      Formula *a = GoThroughNegations(f, &level);
      args.push_back(level % 2 ? -a->GetId() : a->GetId());
      TraverseFormulaTreeAndGenerateCNF(a,conj_tmp);
    }
    else {
      args.push_back(f->GetId());
      TraverseFormulaTreeAndGenerateCNF(f,conj_tmp);
    }

  }


// ----------------------------------------------------------------------------


  Formula* FormulaFactory::GoThroughNegations(Formula* f, int* level) {
    if(f->GetType()==NOT) {
      Formula *n=GoThroughNegations(((FormulaNot*)f)->GetF(), level);
      (*level)++;
      return n;
    }
    else {
      *level=0;
      return f;
    }
  }


// ----------------------------------------------------------------------------


  bool FormulaFactory::IsClause(Formula* f) {
    if(f->GetType()==OR || f->GetType()==NOT || f->GetType()==VAR || f->GetType()==CONST) 
      return true;
    else 
      return false;
  }


// ----------------------------------------------------------------------------


void FormulaFactory::TraverseFormulaTreeAndGenerateCNF(Formula* f,vector<vector <int> > & conj_tmp) {

    // cout << "Traverse " << f->GetId() << " type: " << f->GetType() << " count : " << f->IsDefined() << endl;
    if (f->IsDefined())
      return;

    f->SetDefined();

    vector<int> disj,args;
    switch (f->GetType()) {
      case CONST: 
        break;

      case VAR: 
        break;

      case NOT :
        {
           assert(false);
           /*
           tmp = ((FormulaNot*)f)->GetF();
           disj.push_back(f->GetId());
           disj.push_back(tmp->GetId());
           conj_tmp.push_back(disj);
           disj.clear();
           
           disj.push_back(-f->GetId());
           disj.push_back(-tmp->GetId());
           conj_tmp.push_back(disj);
           disj.clear();
           */
           break;
         }
      
      case AND :
    	{
          TraverseNaryConnectives(f,AND,args,conj_tmp);          
          disj.push_back(f->GetId());
          for (unsigned int i=0; i<args.size(); i++)  
            disj.push_back(-args[i]);
          conj_tmp.push_back(disj);
          disj.clear();

          for (unsigned int i=0; i<args.size(); i++) {
            disj.push_back(-f->GetId());
            disj.push_back(args[i]);
            conj_tmp.push_back(disj);
            disj.clear();
          }
          args.clear();
          break;
        }
      
      case OR :
        {
          TraverseNaryConnectives(f,OR,args,conj_tmp);          
          disj.push_back(-f->GetId());
          for (unsigned int i=0; i<args.size(); i++) 
   	        disj.push_back(args[i]);
          conj_tmp.push_back(disj);
          disj.clear();

          for (unsigned int i=0; i<args.size(); i++) {
            disj.push_back(f->GetId());
            disj.push_back(-args[i]);
            conj_tmp.push_back(disj);
            disj.clear();
          }
          args.clear();
          break;
	}
     	      
      case XOR :
    	{
          int lFormulaID, rFormulaID, level;
          Formula *a;
          Formula *l = ((FormulaXor*)f)->GetLeftF(); 
          Formula *r = ((FormulaXor*)f)->GetRightF();

          a = GoThroughNegations(l, &level);
          lFormulaID = (level % 2 ? -a->GetId() : a->GetId());
          TraverseFormulaTreeAndGenerateCNF(a,conj_tmp);

          a = GoThroughNegations(r, &level);
          rFormulaID = (level % 2 ? -a->GetId() : a->GetId());
          TraverseFormulaTreeAndGenerateCNF(a,conj_tmp);

          disj.push_back(-f->GetId());
          disj.push_back(lFormulaID);
          disj.push_back(rFormulaID);
          conj_tmp.push_back(disj);
          disj.clear();

          disj.push_back(-f->GetId());
          disj.push_back(-lFormulaID);
          disj.push_back(-rFormulaID);
          conj_tmp.push_back(disj);
          disj.clear();

          disj.push_back(f->GetId());
          disj.push_back(-lFormulaID);
          disj.push_back(rFormulaID);
          conj_tmp.push_back(disj);
          disj.clear();

          disj.push_back(f->GetId());
          disj.push_back(lFormulaID);
          disj.push_back(-rFormulaID);
          conj_tmp.push_back(disj);
          disj.clear();

          break;
    	}

      case EQUIV :
        {
          int lFormulaID, rFormulaID, level;
          Formula *a;
          Formula *l = ((FormulaXor*)f)->GetLeftF(); 
          Formula *r = ((FormulaXor*)f)->GetRightF();

          a = GoThroughNegations(l, &level);
          lFormulaID = (level % 2 ? -a->GetId() : a->GetId());
          TraverseFormulaTreeAndGenerateCNF(a,conj_tmp);

          a = GoThroughNegations(r, &level);
          rFormulaID = (level % 2 ? -a->GetId() : a->GetId());
          TraverseFormulaTreeAndGenerateCNF(a,conj_tmp);

          disj.push_back(-f->GetId());
          disj.push_back(-lFormulaID);
          disj.push_back(rFormulaID);
          conj_tmp.push_back(disj);
          disj.clear();

          disj.push_back(-f->GetId());
          disj.push_back(lFormulaID);
          disj.push_back(-rFormulaID);
          conj_tmp.push_back(disj);
          disj.clear();

          disj.push_back(f->GetId());
          disj.push_back(lFormulaID);
          disj.push_back(rFormulaID);
          conj_tmp.push_back(disj);
          disj.clear();

          disj.push_back(f->GetId());
          disj.push_back(-lFormulaID);
          disj.push_back(-rFormulaID);
          conj_tmp.push_back(disj);
          disj.clear();

          break;
        }
  
     default: break;
   }
}
