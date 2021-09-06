/******************************************************************************
 * Copyright (C) 2007-2009. Filip Maric, Predrag Janicic
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * This program is inspired by MiniSat solver (C) Een, Sorensson 2003-2006.
 ******************************************************************************/
#ifndef __SOLVER_H__
#define __SOLVER_H__

/**
   Example usage:

      Solver solver;
      solver.setOption...(...);
      solver.setNumberOfVariables(...);
      solver.addInitialClause(...);
      ...
      solver.addInitialClause(...);
      solver.solve();
      if (solver.isUnsatisfiable())
          ... report unsatisfiability ...
      else if (solver.isSatisfiable()) {
          model = solver.getModel();
          ... report satifiabilitiy ...
      }
 */

#include <vector>
#include <deque>
#include <fstream>

#include <Trail.hpp>
#include <ResolutionClause.hpp>

namespace ArgoSat {

  class LiteralSelectionStrategy;
  class RestartStrategy;
  class ForgetStrategy;
  class ForgetSelectionStrategy;

  class SolverListener;

  class Solver {
  public: Solver();
  public: ~Solver();
  public: void reset();
  public: static Solver* createFromCmdLine(int argc, char* argv[]);

    /****************************************************
     *      SOLVE
     *****************************************************/
  public: void solve();

  public: bool isSatisfiable() {
    return _satisfiable == TRUE;
  }
  public: bool isUnsatisfiable() {
    return _satisfiable == FALSE;
  }

  public: void resetSatisfiable() {
    _satisfiable = UNDEF;
  }

  private: ExtendedBoolean _satisfiable;

    /****************************************************
     *       MODEL
     ****************************************************/
  public: const Valuation& getModel() const {
    return _model;
  }
  public: bool verifyModel() const;

  private: void generateModel() {
    _model = _trail;
  }
  private: Valuation _model;


    /****************************************************
     *       VARIABLE SET
     ****************************************************/
  private: Variables _variables;

  public: const Variables& getVariables() const {
    return _variables;
  }

  public: Variable addVariable(bool useAsDecisionVariable = true);

  public: size_t getNumberOfVariables() const {
    return _variables.size();
  }
  public: void setNumberOfVariables(size_t n) {
    for (size_t i = getNumberOfVariables(); i < n; i++)
      addVariable();
  }

    /****************************************************
     *       ASSERTION TRAIL  -  M
     ****************************************************/
  private: void assertLiteral(Literal l, bool decision);
  private: Literal backtrackLiteral();
  private: void backtrackToLevel(unsigned level);

  private: Trail _trail;
  public: const Trail& getTrail() const {
    return _trail;
  }

    /****************************************************
     *      INITIAL CLAUSE SET  - F0
     ****************************************************/
  public: void addInitialClause(std::vector<Literal>& literals);
  public: size_t getNumberOfInitialClauses() const {
    return _initialClauses.size();
  }
  public: const std::vector<const Clause*>& getInitialClauses() const;

//      {
//    return reinterpret_cast<const std::vector<const Clause*>&>(_initialClauses);
//  }
  private: std::vector<Clause*> _initialClauses;


    /***************************************************
     *     CONFLICT CLAUSE DETECTION
     ***************************************************/
  private: bool isConflicting() const {
    return getConflictClause() != 0;
  }

  private: Clause* getConflictClause() const {
    return _conflictClause;
  }

  private: void setConflictClause(Clause* clause) {
    assert(clause == 0 || clause->isFalse(_trail));
    _conflictClause = clause;
  }
    Clause* _conflictClause;

    /*****************************************************
     *        DECIDE
     *****************************************************/
  private: void applyDecide();
  private: bool allVariablesAssigned();
  private: Literal selectDecisionLiteral();

  private: LiteralSelectionStrategy* _literalSelectionStrategy;
  public: void setLiteralSelectionStrategy(LiteralSelectionStrategy* literalSelectionStrategy);

    /*****************************************************
     *         UNIT PROPAGATE
     *****************************************************/
  private: void exhaustiveUnitPropagate();
  private: bool applyUnitPropagate();

    // Propagation graph
  private: const Clause* getReason(Literal literal) const;
  private: void setReason(Literal literal, const Clause* clause);
  private: std::vector<const Clause*> _reason;

    // Two watch literal scheme
  private: void setWatchLiteral1(Clause* clause, Literal literal);
  private: void setWatchLiteral2(Clause* clause, Literal literal);
  private: void setWatch1(Clause* clause, size_t position);
  private: void setWatch2(Clause* clause, size_t position);

  private: Literal getWatch1(const Clause* clause) const {
    return clause->watch1();
  }

  private: Literal getWatch2(const Clause* clause) const {
    return clause->watch2();
  }
  private: void removeWatch1(const Clause* clause);
  private: void removeWatch2(const Clause* clause);

  private: void swapWatches(Clause* clause) {
    clause->swapWatches();
  }
  private: int findNonWatchedUnfalsifiedLiteralPosition(Clause* clause, char watchNumber);

  private: void notifyWatches(Literal literal);
  private: std::vector< std::vector<Clause*> > _watchLists;

    /*****************************************************
     *        BACKJUMP
     *****************************************************/
  private: Literal findLastAssertedLiteral();

  private: void applyBackjump();

  private: void applyConflict();
  private: void applyExplainUIP();
  private: void applyExplain(Literal l);

  private: ResolutionClause _backjumpClause;
  private: Clause* _finalBackjumpClause;

    /*****************************************************
     *        SUBSUME                                     
     ******************************************************/
  private: void applySubsumption();
  private: void applyExpensiveSubsumption();
  private: bool allLiteralsAreExplainable(const Clause* clause, Literal literal);
  private: bool literalRedundant(Literal literal);
    
    /*****************************************************
     *        LEARN
     *****************************************************/
  private: void applyLearn(Clause* clause);

  private: std::vector<Clause*> _learntClauses;
  private: void addLearntClause(Clause* clause);
  public: bool isLearntClause(const Clause* clause) const;
  public: const std::vector<const Clause*>& getLearntClauses() const;
  public: size_t getNumberOfLearntClauses() const;
  public: size_t getTotalNumberOfClauses() const;

    /*****************************************************
     *        FORGET
     ****************************************************/
  private: void applyForget();
  private: void removeClause(const Clause* clause);
  private: bool isLockedByCurrentTrail(const Clause* clause) const;
  private: ForgetStrategy* _forgetStrategy;
  public: void setForgetStrategy(ForgetStrategy* forgetStrategy);
  private: ForgetSelectionStrategy* _forgetSelectionStrategy;
  public: void setForgetSelectionStrategy(ForgetSelectionStrategy* forgetSelectionStrategy);

    /*****************************************************
     *        SIMPLIFY
     ****************************************************/
  private: bool applySimplify();
  private: void forgetSatisfiedClauses(std::vector<Clause*>& clauses);
    
    /*****************************************************
     *        RESTART
     *****************************************************/
  private: void applyRestart();
  private: RestartStrategy* _restartStrategy;
  public: void setRestartStrategy(RestartStrategy* restartStrategy);

    /*****************************************************
     *        PURE LITERAL
     *****************************************************/
  private: bool applyPureLiteral();
  private: void exhaustivePureLiteral();
  private: bool _usePureLiteral;

    /*****************************************************
     *        LISTENERS
     *****************************************************/
  private: std::vector<SolverListener*> _listeners;
  public: void addListener(SolverListener* listener) const;
  public: void removeListener(SolverListener* listener) const;

  };
}//namespace ArgoSat


#endif
