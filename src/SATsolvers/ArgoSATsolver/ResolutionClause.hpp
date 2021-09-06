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
#ifndef __LEARNT_CLAUSE_H__
#define __LEARNT_CLAUSE_H__


#include <vector>
#include <algorithm>
#include <sstream>
#include <cassert>

#include "Variable.hpp"
#include "Literal.hpp"
#include "Clause.hpp"

namespace ArgoSat {
class Solver;

/**
 * Helper data structure used in firstUIP conflict analysis.
 * Class represents a clause that is resolved with other clauses in
 * order to compute firstUIP clause. Current decision level literals
 * are all going to be resolved during process and they are not explicitly
 * stored for efficiency reasons. Only lower decision level literals
 * that are going to be included in the resulting clause are stored.
 * @author Filip Maric
 */
class ResolutionClause {
 private:
  /**
   * List of variables that are currently in the clause.
   * This is just an optimization feature. Since we assume that
   * no conflicting literals are going to be added to the clause,
   * it is sufficent to check just if the clause contain the
   * variable in order to check if the clause contais the literal
   */
  std::vector<bool> _containsVariable;


  /**
   * Literals from lower decision levels (except literals from the decision 
   * 0 which are automatically removed). These literals are not
   * resolved during resolution process and they all participate
   * in the resulting clause.
   */
  std::vector<Literal> _lowerLevelLiterals;

#ifndef NDEBUG
  /**
   * Literals from the initial decision level (they are automaticaly removed 
   * from the clause)
   * We keep them only for debugging purposes.
   */
  std::vector<Literal> _initialLevelLiterals;
#endif

  /**
   * Number of literals from the current decision level. For efficiency
   * reasons, the literals are not explicitly stored, since they are
   * all going to be resolved away.
   */
  int _numCurrentLevelLiterals;

  /**
   * Literal whose opposite is last asserted literal from this clause.
   */
  Literal _lastLiteral;

  
  /**
   * Classic clause representation
   */
  Clause* _clause;

  /**
   * Sat solver which stores the trail M
   */
  const Solver& _solver;

 public:
  /**
   * Clause is initialized to the empty clause.
   * @param solver
   */
  ResolutionClause(const Solver& solver)
    : _solver(solver) {
    clear();
  }

  size_t size() {
    return _lowerLevelLiterals.size() + _numCurrentLevelLiterals;
  }

  void clear();

  void setNumberOfVariables(int n) {
    _containsVariable.resize(n);
  }
  

  /**
   * @return literals from lower decision levels
   */
  const std::vector<Literal>&  getLowerLevelLiterals() {
    return _lowerLevelLiterals;
  }

#ifndef NDEBUG
  /**
   * @return literals from decision level 0
   */
  const std::vector<Literal>& getInitialLevelLiterals() const {
    return _initialLevelLiterals;
  }
#endif

  /**
   * @return number of current decision level literals in the clause
   */
  int getNumberOfCurrentLevelLiterals() const {
    return _numCurrentLevelLiterals;
  }

  void setLastLiteral(Literal literal) {
    assert(containsVariable(Literals::variable(literal)));
    assert(level(Literals::opposite(literal)) == currentLevel());
    _lastLiteral = literal;
  }

  Clause* getClause() {
    if (_clause != 0)
      return _clause;

    assert(_lastLiteral != Literals::UNDEFINED);
    std::vector<Literal> literals = _lowerLevelLiterals;
	
    literals.insert(literals.begin(), _lastLiteral);

    _clause = new Clause(literals);

    return _clause;
  }

  /**
   * Starts the resolution process by adding all the literals from
   * the conflict clause.
   * @param conflictClause
   */
  void setConflictClause(Clause* conflictClause) {
    Clause::const_iterator i, 
      b = conflictClause->begin(), 
      e = conflictClause->end();
    for (i = b; i != e; i++)
      addLiteral(*i);
  }

  /**
   * Resolves this clause with the given clause, storing the result in
   * this clause. Resolution is done on the resolution literal which has
   * to be set using the setResolutionLiteral().
   * @param clause 
   */
  void resolve(const Clause* clause, Literal literal) {
    Clause::const_iterator i, b = clause->begin(), e = clause->end();
    for (i = b; i != e; i++) {
      if (*i != literal) {
	addLiteral(*i);
      }
    }
    removeCurrentLevelLiteral(Literals::opposite(literal));
  }

  /**
   *  UIP is reached if there is exactly one literal on the current decision
   *  level.
   */
  bool isUIP() const {
    return getNumberOfCurrentLevelLiterals() == 1;
  }

  static const unsigned NOT_FOUND = (unsigned)(-1);
  /**
   *  @return arbitrary literal asserted at last lower level
   */
  Literal getLastLowerLevelLiteral() const {
    assert(!_lowerLevelLiterals.empty());
    unsigned maxLevel = 0;
    Literal result = NOT_FOUND;
    std::vector<Literal>::const_iterator i, 
      b = _lowerLevelLiterals.begin(), 
      e = _lowerLevelLiterals.end();
    for (i = b; i != e; i++) {
      unsigned currLevel = levelVariable(Literals::variable(*i)); 
      if (maxLevel < currLevel) {
	maxLevel = currLevel;
	result = *i;
      }
    }
    assert(result != NOT_FOUND);
    return result;
  }

  /**
   * @return Maximal decision level of a literal (excluding current decision 
   * level). If there were no lower level literals in the clause, decision 
   * level 0 is returned.
   */
  unsigned getMaxLowerLevel() const {
    assert(isUIP());
    unsigned maxLevel = 
      _lowerLevelLiterals.empty() ? 0 : level(getLastLowerLevelLiteral());
    assert(maxLevel > 0 && maxLevel < currentLevel());
    return maxLevel;
  }	


  /**
   * Textual representation of the clause
   */
  std::string toString() const;
	
  /**
   * @param variable
   * @return true if the clause contains the given variable
   */
  bool containsVariable(Variable variable) const {
    return _containsVariable[variable];
  }

  /**
   * @return true if all variables from the given clause are contained in 
   * this resolution clause
   */
  bool subsumes(const Clause* clause) {
    Clause::const_iterator it,
      beg = clause->begin(), en = clause->end();
    for (it = beg; it != en; it++)
      if (!containsVariable(Literals::variable(*it))) {
	return false;
      }
    return true;
  }
    
 private: 
  /**
   * Adds literal to the clause
   * @param literal
   */
  void addLiteral(Literal literal) {
    //    logger << "Add literal: " << Literals::toString(literal) << endl;
    Variable variable = Literals::variable(literal);
    // Clause contains the literal iff it contains its variable
    if(containsVariable(variable))
      return;
	
    size_t decisionLevel = levelVariable(Literals::variable(literal));
    if (decisionLevel == currentLevel()) {
      // cout << "Current level: " << Literals::toString(literal) << endl;
      // No need to store current level literals, we just count them
      _numCurrentLevelLiterals++;
    } else if (decisionLevel == 0) {
#ifndef NDEBUG
      // cout << "Initial level: " << Literals::toString(literal) << endl;
      _initialLevelLiterals.push_back(literal);
#endif
    } else {
      // Add literals from previous levels except initial
      // cout << "Previous level: " << Literals::toString(literal) << endl;
      _lowerLevelLiterals.push_back(literal);
    }
		
    // Mark that the clause contains the given variable
    _containsVariable[Literals::variable(literal)] = true;
  }

 public:
  void removeCurrentLevelLiteral(Literal literal) {
    assert(levelVariable(Literals::variable(literal)) == currentLevel());
    _numCurrentLevelLiterals--;
    _containsVariable[Literals::variable(literal)] = false;
  }
  
  void removeLowerLevelLiteral(Literal literal) {
    _lowerLevelLiterals.erase(
			      std::find(_lowerLevelLiterals.begin(), 
					_lowerLevelLiterals.end(), literal));
    _containsVariable[Literals::variable(literal)] = false;
  }

#ifndef NDEBUG
  void removeInitialLevelLiteral(Literal literal) {
    _initialLevelLiterals.erase(
				std::find(_initialLevelLiterals.begin(), 
					  _initialLevelLiterals.end(), literal));
    _containsVariable[Literals::variable(literal)] = false;
  }
#endif

  void removeLiteral(Literal literal) {
    size_t decisionLevel = levelVariable(Literals::variable(literal));
    if (decisionLevel == currentLevel()) {
      removeCurrentLevelLiteral(literal);
    } else if (decisionLevel == 0) {
#ifndef NDEBUG
      removeInitialLevelLiteral(literal);
#endif
    } else {
      removeLowerLevelLiteral(literal);
    }
  }

 private:		
  std::string lowerLevelLiteralsToString() const {
    std::string result = "[";
    result += Literals::literalVectorToString(_lowerLevelLiterals);
#ifndef NDEBUG
    result += " | " ;
    result += Literals::literalVectorToString(_initialLevelLiterals);
#endif
    result += "]";
    return result;
  }

  /**
   * Gets decision level of a given literal
   */
  size_t level(Literal literal) const;

  /**
   * Gets decision level of a given variable
   */
  size_t levelVariable(Variable variable) const;

  /**
   * Gets the current decision level
   */
  size_t currentLevel() const;
};

}//namespace ArgoSat
#endif

