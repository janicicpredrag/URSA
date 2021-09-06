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
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <iostream>
using std::cerr;
using std::endl;

#include "Solver.hpp"

#include "LiteralSelectionStrategyVariablePolarity.hpp"
#include "VariableSelectionStrategyMinisat.hpp"
#include "RestartStrategyMinisat.hpp"
#include "ForgetStrategyMinisat.hpp"
#include "ForgetSelectionStrategyMinisat.hpp"

namespace ArgoSat {

  Solver::Solver() 
    : _satisfiable(UNDEF),
      _conflictClause(0),
      _backjumpClause(*this),
      _usePureLiteral(false) {
    _literalSelectionStrategy= new LiteralSelectionStrategyVariablePolarity
      (new MinisatVariableSelectionStrategy(*this), 
       new LiteralPolaritySelectionStrategyPolaritySaving(*this));
    _forgetStrategy = new MinisatForgetStrategy(*this); 
    _forgetSelectionStrategy = new MinisatForgetSelectionStrategy(*this);
    _restartStrategy = new MinisatRestartStrategy(*this);
  }

  Solver::~Solver() {
    std::vector<Clause*>::const_iterator it;
    for (it = _learntClauses.begin(); it != _learntClauses.end(); it++)
      removeClause(*it);
    
    std::vector<Clause*>::const_iterator jt;
    for (jt = _initialClauses.begin(); jt != _initialClauses.end(); jt++)
      removeClause(*jt);
    
    _variables.clear();

    delete _literalSelectionStrategy;
    delete _forgetStrategy;
    delete _forgetSelectionStrategy;
    delete _restartStrategy;
  }



  void Solver::reset() {
    std::vector<Clause*>::const_iterator it;
    for (it = _learntClauses.begin(); it != _learntClauses.end(); it++)
      removeClause(*it);
    
    std::vector<Clause*>::const_iterator jt;
    for (jt = _initialClauses.begin(); jt != _initialClauses.end(); jt++)
      removeClause(*jt);
    
    _variables.clear();
    _trail.clear();

    delete _literalSelectionStrategy;
    delete _forgetStrategy;
    delete _forgetSelectionStrategy;
    delete _restartStrategy;

    _satisfiable=UNDEF;
    _conflictClause=0;
 /*   _backjumpClause=*this;*/
    _usePureLiteral=false; 
    _literalSelectionStrategy= new LiteralSelectionStrategyVariablePolarity
      (new MinisatVariableSelectionStrategy(*this), 
       new LiteralPolaritySelectionStrategyPolaritySaving(*this));
    _forgetStrategy = new MinisatForgetStrategy(*this); 
    _forgetSelectionStrategy = new MinisatForgetSelectionStrategy(*this);
    _restartStrategy = new MinisatRestartStrategy(*this);
  }

  const std::vector<const Clause*>& Solver::getInitialClauses() const {
    return reinterpret_cast<const std::vector<const Clause*>&>(_initialClauses);
  }

  Solver* Solver::createFromCmdLine(int argc, char* argv[]) {
    Solver* solver = new Solver();
    int curr = 0;
    while(curr < argc) {
      if (argv[curr][0] == '-') {
	if (strcmp(argv[curr], "--literal_selection_strategy") == 0 ||
	    strcmp(argv[curr], "-l") == 0) {
	  LiteralSelectionStrategy* literalSelectionStrategy = 
	    LiteralSelectionStrategy::createFromCmdLine(*solver, argc, argv, ++curr);
	  solver->setLiteralSelectionStrategy(literalSelectionStrategy);
	} else if (strcmp(argv[curr], "--forget_strategy") == 0 ||
		   strcmp(argv[curr], "-f") == 0) {
	  ForgetStrategy* forgetStrategy = 
	    ForgetStrategy::createFromCmdLine(*solver, argc, argv, ++curr);
	  solver->setForgetStrategy(forgetStrategy);
	} else if (strcmp(argv[curr], "--forget_selection_strategy") == 0 ||
		   strcmp(argv[curr], "-fs") == 0) {
	  ForgetSelectionStrategy* forgetSelectionStrategy = 
	    ForgetSelectionStrategy::createFromCmdLine(*solver, argc, argv, ++curr);
	  solver->setForgetSelectionStrategy(forgetSelectionStrategy);
	} else if (strcmp(argv[curr], "--restart_strategy") == 0 ||
		   strcmp(argv[curr], "-r") == 0) {
	  RestartStrategy* restartStrategy = 
	    RestartStrategy::createFromCmdLine(*solver, argc, argv, ++curr);
	  solver->setRestartStrategy(restartStrategy);
	} else if (strcmp(argv[curr], "--variable_selection_strategy") == 0 ||
		   strcmp(argv[curr], "-v") == 0) {
	  VariableSelectionStrategy* variableSelectionStrategy = 
	    VariableSelectionStrategy::createFromCmdLine(*solver, argc, argv, ++curr);
	  LiteralSelectionStrategyVariablePolarity* ls = dynamic_cast<LiteralSelectionStrategyVariablePolarity*>(solver->_literalSelectionStrategy);
	  if (!ls)
	    throw std::string("Error setting variable selection strategy");
	  ls->setVariableSelectionStrategy(variableSelectionStrategy);
	} else if (strcmp(argv[curr], "--literal_polarity_selection_strategy") == 0 ||
		   strcmp(argv[curr], "-p") == 0) {
	  LiteralPolaritySelectionStrategy* literalPolaritySelectionStrategy = 
	    LiteralPolaritySelectionStrategy::createFromCmdLine(*solver, argc, argv, ++curr);
	  LiteralSelectionStrategyVariablePolarity* ls = dynamic_cast<LiteralSelectionStrategyVariablePolarity*>(solver->_literalSelectionStrategy);
	  if (!ls)
	    throw std::string("Error setting variable selection strategy");
	  ls->setLiteralPolaritySelectionStrategy(literalPolaritySelectionStrategy);
	} else if (strcmp(argv[curr], "--use_pure_literal") == 0 ||
		   strcmp(argv[curr], "-pl") == 0) {
	  solver->_usePureLiteral = true;
	  curr++;
	} else {
	  curr++;
	}
      } else 
	curr++;
    }
 
    return solver;
  }

  /****************************************************
   *      INITIAL CLAUSE SET  - F0
   ****************************************************/
  void Solver::addInitialClause(std::vector<Literal>& literals) {
    if (_satisfiable != FALSE)
      _satisfiable = UNDEF;

    //  cout << Clause(literals).toString() << endl;
    assert(_trail.currentLevel() == 0);
    Clause::removeDuplicateLiterals(literals);
    Clause::removeFalsifiedLiterals(literals, _trail);

    if (Clause::isTautology(literals)) {
      return;
    } else if (Clause::containsTrueLiteral(literals, _trail)) {
      return;
    } else if (literals.size() == 0) {
      _satisfiable = FALSE;
    } else if (literals.size() == 1) {
      assertLiteral(literals[0], false);
      exhaustiveUnitPropagate();
    } else {
      Clause* clause = new Clause(literals, true);
      assert(clause->isInitial());
      assert(!clause->containsDuplicateLiterals());
      _initialClauses.push_back(clause);
      setWatch1(clause, 0);
      setWatch2(clause, 1);
    }
  }


  /****************************************************
   *       VARIABLE SET
   ****************************************************/
  Variable Solver::addVariable(bool /*useAsDecisionVariable*/) {
    Variable var = _variables.newVariable();
    _trail.addVariable(var);
    _reason.push_back(0);
    _reason.push_back(0);
    _watchLists.push_back(std::vector<Clause*>());
    _watchLists.push_back(std::vector<Clause*>());
	
    return var;
  }


  /****************************************************
   *      SOLVE
   *****************************************************/
  void Solver::solve() {
    assert(_trail.currentLevel() == 0);

    if (_usePureLiteral)
      exhaustivePureLiteral();

    exhaustiveUnitPropagate();
  
    std::vector<SolverListener*>::const_iterator 
      lb = _listeners.begin(), le = _listeners.end(), li;
    for (li = lb; li != le; li++)
      (*li)->init();

    _backjumpClause.setNumberOfVariables(getNumberOfVariables());

    while(_satisfiable == UNDEF) {
      exhaustiveUnitPropagate();

      if (isConflicting()) {
	if (_trail.currentLevel() == 0) {
	  _satisfiable = FALSE;
	} else {
	  applyConflict();
	  applyExplainUIP();
	  applySubsumption();
	  applyBackjump();
	}
      } else {
	if (_restartStrategy->shouldRestart()) {
	  applyRestart();

	  if (_usePureLiteral)
	    exhaustivePureLiteral();
	  else		
	    applySimplify();

	}

	if (_forgetStrategy->shouldForget()) {
	  applyForget();
	}

	if (!allVariablesAssigned()) {
	  applyDecide();
	} else {
	  _satisfiable = TRUE;
	  generateModel();
	  backtrackToLevel(0);
	}
      }
    }
  }

  /****************************************************
   *       ASSERTION TRAIL  -  M
   ****************************************************/
  void Solver::assertLiteral(Literal l, bool decision) {
    _trail.push(l, decision);

    std::vector<SolverListener*>::const_iterator 
      lb = _listeners.begin(), le = _listeners.end(), li;
    for (li = lb; li != le; li++)
      (*li)->onAssert(l);

    notifyWatches(Literals::opposite(l));
  }

  bool Solver::allVariablesAssigned() {
    size_t nvars = getNumberOfVariables();
    for (Variable var = 0; var < nvars; var++)
      if (_trail.isUndefVariable(var) && _variables.isDecisionVariable(var)) {
	return false;
      }
    return true;
  }

  Literal Solver::backtrackLiteral() {
    assert(!_trail.empty());
    Literal l = _trail.pop();

    std::vector<SolverListener*>::const_iterator 
      lb = _listeners.begin(), le = _listeners.end(), li;
    for (li = lb; li != le; li++)
      (*li)->onBacktrack(l);

    return l;
  }

  void Solver::backtrackToLevel(unsigned level) {
    assert(_trail.currentLevel() >= level);
    while(_trail.currentLevel() > level)
      backtrackLiteral();
  }

  /*****************************************************
   *        MODEL
   *****************************************************/
  bool Solver::verifyModel() const {
    const Valuation& model = getModel();

    std::vector<const Clause*>::const_iterator it, 
      beg = getInitialClauses().begin(), en = getInitialClauses().end();
    for (it = beg; it != en; it++)
      if (!(*it)->isTrue(model)) {
	cerr << "Clause: " << (*it)->toString() << " is not true!" << endl;
	return false;
      }
    return true;
  }

  /*****************************************************
   *        DECIDE
   *****************************************************/
  void Solver::applyDecide() {
    Literal decisionLiteral = _literalSelectionStrategy->getLiteral();
    std::vector<SolverListener*>::const_iterator 
      lb = _listeners.begin(), le = _listeners.end(), li;
    for (li = lb; li != le; li++)
      (*li)->onDecide(decisionLiteral);

    setReason(decisionLiteral, 0);
    assertLiteral(decisionLiteral, true);
  }

  void Solver::setLiteralSelectionStrategy(LiteralSelectionStrategy* literalSelectionStrategy) {
    if (_literalSelectionStrategy)
      delete _literalSelectionStrategy;
    _literalSelectionStrategy = literalSelectionStrategy;
  }


  /*****************************************************
   *       UNIT PROPAGATE
   ****************************************************/
  void Solver::exhaustiveUnitPropagate() {
    bool ret;
    do {
      ret = applyUnitPropagate();
    } while (ret && !isConflicting());  
  }

  bool Solver::applyUnitPropagate() {
    if (!_trail.hasUnitLiteral())
      return false;
  
    Literal l = _trail.assertNextUnitLiteral();

    std::vector<SolverListener*>::const_iterator 
      lb = _listeners.begin(), le = _listeners.end(), li;
    for (li = lb; li != le; li++) {
      (*li)->onPropagate(l, getReason(l));
      (*li)->onAssert(l);
    }
    notifyWatches(Literals::opposite(l));

    return true;
  } 

  void Solver::setWatch1(Clause* clause, size_t position) {
    Literal l = (*clause)[position];
    clause->setWatch1(position);
    _watchLists[l].push_back(clause);
  }

  void Solver::setWatch2(Clause* clause, size_t position) {
    Literal l = (*clause)[position];
    clause->setWatch2(position);
    _watchLists[l].push_back(clause);
  }

  void Solver::setWatchLiteral1(Clause* clause, Literal literal) {
    clause->setWatchLiteral1(literal);
    _watchLists[literal].push_back(clause);
  }

  void Solver::setWatchLiteral2(Clause* clause, Literal literal) {
    clause->setWatchLiteral2(literal);
    _watchLists[literal].push_back(clause);
  }

  void Solver::removeWatch1(const Clause* clause) {
    std::vector<Clause*>& watchList = _watchLists[clause->watch1()];
    std::vector<Clause*>::iterator it =
      std::find(watchList.begin(), watchList.end(), clause);
    if (it != watchList.end())
      watchList.erase(it);
  }

  void Solver::removeWatch2(const Clause* clause) {
    std::vector<Clause*>& watchList = _watchLists[clause->watch2()];
    std::vector<Clause*>::iterator it =
      std::find(watchList.begin(), watchList.end(), clause);
    if (it != watchList.end())
      watchList.erase(it);
  }

  void Solver::notifyWatches(Literal literal) {
    std::vector<Clause*>& watchList = _watchLists[literal];

    std::vector<Clause*>::const_iterator currentClause,
      beg = watchList.begin(), en = watchList.end();
    std::vector<Clause*>::iterator lastKeptClause = watchList.begin();
    for (currentClause = beg; currentClause != en; currentClause++) {
      Clause* clause = (*currentClause);

      // Assert: Watch literal is falsified
      // X F X X X X X X X    or   F X X X X X X X X X
      assert(_trail.isFalse(getWatch1(clause)) ||
	     _trail.isFalse(getWatch2(clause)));

    
      Literal watch1 = getWatch1(clause);
      // Assure that the false literal is watch 2
      if (watch1 == literal) {
	swapWatches(clause);
	watch1 = getWatch1(clause);
      }

      // Assert: X F X X X X X X X X
      assert(_trail.isFalse(getWatch2(clause)));

      // Clause is true, there is no need to change watch literals
      // Assert: T F X X X X X X X X
      if (_trail.isTrue(watch1)) {
	*lastKeptClause++ = clause;
	continue;
      }
    
      size_t position = Clause::NOT_FOUND;
      if (clause->size() > 2 && 
	  (position = clause->unfalsifiedNonWatchedLiteralPosition(_trail)) != Clause::NOT_FOUND) {
	assert(!_trail.isFalse((*clause)[position]));
	setWatch2(clause, position);
	continue;
      }

      assert(position == Clause::NOT_FOUND);
    
      if (_trail.isFalse(watch1)) {
	_conflictClause = clause;
	while (currentClause != en)
	  *lastKeptClause++ = *currentClause++;
	break;
      } else {
	setReason(watch1, clause);
	_trail.enqueueUnitLiteral(watch1);
      }
      *lastKeptClause++ = clause;
    }
  
    watchList.erase(lastKeptClause, watchList.end());
  }


  const Clause* Solver::getReason(Literal literal) const {
    return _reason[literal];
  }           
    
  void Solver::setReason(Literal literal, const Clause* clause) {
    _reason[literal] = clause;
  }       


  /*****************************************************
   *       CONFLICT
   ****************************************************/
  void Solver::applyConflict() {
    assert(isConflicting());

    std::vector<SolverListener*>::const_iterator 
      lb = _listeners.begin(), le = _listeners.end(), li;
    for (li = lb; li != le; li++)
      (*li)->onConflict(getConflictClause());


    _backjumpClause.clear();
    _backjumpClause.setConflictClause(getConflictClause());
  }


  /*****************************************************
   *       EXPLAIN
   ****************************************************/
  void Solver::applyExplain(Literal l) {
    assert(_backjumpClause.containsVariable(Literals::variable(l)));
    assert(_trail.isTrue(l));
    const Clause* reason = getReason(l);
    assert(reason != 0);

    _backjumpClause.resolve(reason, l);

    std::vector<SolverListener*>::const_iterator 
      lb = _listeners.begin(), le = _listeners.end(), li;
    for (li = lb; li != le; li++)
      (*li)->onExplain(l, reason);
  }

  void Solver::applyExplainUIP() {
    while (!_backjumpClause.isUIP()) {
      Literal l = findLastAssertedLiteral();
      applyExplain(l);
    }
  }

  Literal Solver::findLastAssertedLiteral() {
    while (!_backjumpClause.containsVariable(Literals::variable(_trail.top()))) {
      backtrackLiteral();
    }
    return _trail.top();
  }

  /*****************************************************
   *       SUBSUMPTION
   ****************************************************/
  void Solver::applySubsumption() {
    std::vector<Literal> literals =
      _backjumpClause.getLowerLevelLiterals();
    std::vector<Literal> markedLiterals;

    std::vector<Literal>::const_iterator i;
    for (i = literals.begin(); i != literals.end(); i++) {
      const Clause* reason = getReason(Literals::opposite(*i));
      if (reason != 0 && _backjumpClause.subsumes(reason)) {
	markedLiterals.push_back(*i);
      }
    }

    for (i = markedLiterals.begin(); i != markedLiterals.end(); i++) {
      _backjumpClause.removeLowerLevelLiteral(*i);
    }
  }

  void Solver::applyExpensiveSubsumption() {
    cout << _backjumpClause.toString() << endl;
    std::vector<Literal> literals =
      _backjumpClause.getLowerLevelLiterals();

    std::vector<Literal>::const_iterator i;
    for (i = literals.begin(); i != literals.end(); i++) {
      if (literalRedundant(*i)) {
	cout << "ISREDUNDANT: " << Literals::toString(*i) << endl;
	_backjumpClause.removeLowerLevelLiteral(*i);
      }
    }
  }

  bool Solver::allLiteralsAreExplainable(const Clause* clause, Literal literal) {
    Clause::const_iterator i, b = clause->begin(), e = clause->end();
    for (i = b; i != e; i++) {
      if (*i != literal) {
	if (getReason(*i) == 0)
	  return false;
      }
    }
    return true;
  }

  bool Solver::literalRedundant(Literal literal) {
    cout << "CHECK REDUNDANT: " << Literals::toString(literal) << endl;
    ResolutionClause backjumpClauseCopy = _backjumpClause;
    std::vector<Literal> stack;
    stack.push_back(literal);
    cout << "STACK PUSH: " << Literals::toString(literal) << endl;
    while (!stack.empty()) {
      Literal l = Literals::opposite(stack.back());
      stack.pop_back();
      const Clause* reason = getReason(l);
      if (reason == 0) {
	cout << "Decision: " << Literals::toString(l) << endl;
	return false;
      } else {
	cout << "Reason: " << reason->toString() << endl;
      }

      Clause::const_iterator i, b = reason->begin(), e = reason->end();
      for (i = b; i != e; i++) {
	if (!backjumpClauseCopy.containsVariable(Literals::variable(*i)) && 
	    _trail.level(Literals::opposite(*i)) > 0) {
	  stack.push_back(*i);
	  cout << "STACK PUSH: " << Literals::toString(*i) << endl;
	}
      }
      backjumpClauseCopy.resolve(reason, l);

    }
    return true;
  }



  /******************************************************
   *        BACKJUMP
   *******************************************************/
  void Solver::applyBackjump() {
    Literal l = findLastAssertedLiteral();
    Literal l_op = Literals::opposite(l);
    _backjumpClause.setLastLiteral(l_op);
    _conflictClause = 0;

    if (_backjumpClause.size() > 1) {
      Literal ll_op = _backjumpClause.getLastLowerLevelLiteral();
      Literal ll = Literals::opposite(ll_op);

      Clause* C = _backjumpClause.getClause();

      setWatchLiteral1(C, l_op);
      setWatchLiteral2(C, ll_op);
      setReason(l_op, C);

      applyLearn(C);

      unsigned level = _trail.level(ll);
      backtrackToLevel(level);
    } else {
      backtrackToLevel(0);
    }

    assertLiteral(l_op, false);
  }
   

  /*****************************************************
   *        LEARN
   *****************************************************/
  void Solver::applyLearn(Clause* clause) {
    assert (clause->size() > 1);
    addLearntClause(clause);

    std::vector<SolverListener*>::const_iterator 
      lb = _listeners.begin(), le = _listeners.end(), li;
    for (li = lb; li != le; li++)
      (*li)->onLearn(clause);
  }

  void Solver::addLearntClause(Clause* clause) {
    _learntClauses.push_back(clause);
  }

  bool Solver::isLearntClause(const Clause* clause) const {
    assert(clause->isInitial() || 
	   std::find(_learntClauses.begin(), _learntClauses.end(), clause) != 
	   _learntClauses.end());
    return !clause->isInitial();
  }

  const std::vector<const Clause*>& Solver::getLearntClauses() const {
    return reinterpret_cast<const std::vector<const Clause*>&>(_learntClauses);
  }

  size_t Solver::getNumberOfLearntClauses() const {
    return _learntClauses.size();
  }

  size_t Solver::getTotalNumberOfClauses() const {
    return getNumberOfInitialClauses() + getNumberOfLearntClauses();
  }



  /*****************************************************
   *        FORGET
   *****************************************************/
  void Solver::applyForget() {
    // Ensure that clauses themselves are not modified during forget operation
    std::vector<const Clause*>& learntClauses = 
      reinterpret_cast<std::vector<const Clause*>&>(_learntClauses);

    // Select locked clauses so that they can not be forgotten
    std::vector<const Clause*>::iterator firstUnlocked = learntClauses.begin(), i;
    for (i = learntClauses.begin(); i != learntClauses.end(); i++) {
      if (isLockedByCurrentTrail(*i)) {
	const Clause* tmp = *firstUnlocked;
	*firstUnlocked = *i;
	*i = tmp;
	firstUnlocked++;
      }
    }

    // Select unlocked clauses that should be forgotten
    std::vector<const Clause*>::iterator beginForget, endForget;
    _forgetSelectionStrategy->select(firstUnlocked, learntClauses.end(), 
				     beginForget, endForget);

    // forget selected unlocked clauses
    for (i = beginForget; i != endForget; i++) {
      assert(!isLockedByCurrentTrail(*i));
      removeClause(*i);
    }
    learntClauses.erase(beginForget, endForget);

    std::vector<SolverListener*>::const_iterator 
      lb = _listeners.begin(), le = _listeners.end(), li;
    for (li = lb; li != le; li++)
      (*li)->onForget();
  }

  void Solver::removeClause(const Clause* clause) {
    assert(clause->size() > 1);

    std::vector<SolverListener*>::const_iterator 
      lb = _listeners.begin(), le = _listeners.end(), li;
    for (li = lb; li != le; li++)
      (*li)->onForgetClause(clause);

    removeWatch1(clause);
    removeWatch2(clause);
    delete clause;
  }

  bool Solver::isLockedByCurrentTrail(const Clause* clause) const {
    return getReason(getWatch1(clause)) == clause;
  }

  void Solver::setForgetStrategy(ForgetStrategy* forgetStrategy) {
    if (_forgetStrategy)
      delete _forgetStrategy;
    _forgetStrategy = forgetStrategy;
  }

  void Solver::setForgetSelectionStrategy(ForgetSelectionStrategy* forgetSelectionStrategy) {
    if (_forgetSelectionStrategy)
      delete _forgetSelectionStrategy;
    _forgetSelectionStrategy = forgetSelectionStrategy;
  }

  /*****************************************************
   *        SIMPLIFY
   *****************************************************/
  bool Solver::applySimplify() {
    assert(_trail.currentLevel() == 0);

    size_t initialClausesBefore = getNumberOfInitialClauses();
    size_t learntClausesBefore  = getNumberOfLearntClauses();

    // forgetSatisfiedClauses(_initialClauses);
    forgetSatisfiedClauses(_learntClauses);

    if (initialClausesBefore < getNumberOfInitialClauses() ||
	learntClausesBefore < getNumberOfLearntClauses()) {
      return true;
    }
    return false;
  }

  void Solver::forgetSatisfiedClauses(std::vector<Clause*>& clauses) {
    std::vector<Clause*> newClauses;
    std::vector<Clause*>::const_iterator it, 
      beg = clauses.begin(), en = clauses.end();
    for (it = beg; it != en; it++)
      if ((*it)->isTrue(_trail) && !isLockedByCurrentTrail(*it)) {
	removeClause(*it);
      } else {
	newClauses.push_back(*it);
      }
    clauses = newClauses;
  }

  /*****************************************************
   *        RESTART
   *****************************************************/
  void Solver::applyRestart() {
    backtrackToLevel(0);

    std::vector<SolverListener*>::const_iterator 
      lb = _listeners.begin(), le = _listeners.end(), li;
    for (li = lb; li != le; li++)
      (*li)->onRestart();
  }

  void Solver::setRestartStrategy(RestartStrategy* restartStrategy) {
    if (_restartStrategy)
      delete _restartStrategy;
    _restartStrategy = restartStrategy;
  }

  /*****************************************************
   *        PURE LITERAL
   *****************************************************/
  bool Solver::applyPureLiteral() {
    assert(_trail.currentLevel() == 0);
    LiteralOccurenceCounter counter(getNumberOfVariables());
    counter.count(getInitialClauses());
    bool foundPureLiteral = false;
    for (Variable variable = 0; variable < getNumberOfVariables(); variable++) {
      Literal pos = Literals::literal(variable, true);
      Literal neg = Literals::literal(variable, false);
      if (counter.getCount(pos) == 0 && _trail.isUndef(neg)) {
	assertLiteral(neg, false);
	foundPureLiteral = true;
      } else if (counter.getCount(neg) == 0 && _trail.isUndef(pos)) {
	assertLiteral(pos, false);
	foundPureLiteral = true;
      }
    }
    return foundPureLiteral;
  }

  void Solver::exhaustivePureLiteral() {
    bool ret;
    do {
      ret = applyPureLiteral();
      ret |= applySimplify();
    } while (ret);
  }

  /*****************************************************
   *        LISTENERS
   *****************************************************/

  void Solver::addListener(SolverListener* listener) const {
    // This really doesn't change the solver state so we lie about const
    Solver* t = const_cast<Solver*>(this);
    t->_listeners.push_back(listener);
  }

  void Solver::removeListener(SolverListener* listener) const {
    // This really doesn't change the solver state so we lie about const
    Solver* t = const_cast<Solver*>(this);
    t->_listeners.erase(std::find(t->_listeners.begin(), t->_listeners.end(), listener));
  }

}//namespace ArgoSat
