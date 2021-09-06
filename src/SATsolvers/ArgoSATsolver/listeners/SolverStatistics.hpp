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
#ifndef __SOLVER_STATISTICS_H__
#define __SOLVER_STATISTICS_H__
#include <cstdio>

#include <vector>

#include "SolverListener.hpp"

namespace ArgoSat {

class Solver;
class Clause;

class SolverStatistics : public SolverListener {
private:
  Solver& _solver;
  int _numberOfDecisions;
  int _numberOfConflicts;
  int _numberOfPropagations;
  int _numberOfRestarts;
	
public:
  SolverStatistics(Solver& solver);
  ~SolverStatistics();

  virtual void onDecide(Literal /*l*/) {
    _numberOfDecisions++;
  }
	
  virtual void onConflict(const Clause* /*clause*/) {
    _numberOfConflicts++;
  }

  virtual void onRestart() {
    _numberOfRestarts++;
    printStatistics();
  }

  virtual void onPropagate(Literal /*l*/, const Clause* /*clause*/) {
    _numberOfPropagations++;
  }

  int getNumberOfDecisions() const {
    return _numberOfDecisions;
  }

  int getNumberOfConflicts() const {
    return _numberOfConflicts;
  }

  int getNumberOfPropagations() const {
    return _numberOfPropagations;
  }

  int getNumberOfRestarts() const {
    return _numberOfRestarts;
  }

  static unsigned getNumberOfLiterals(const std::vector<const Clause*>& clauses);
  float averageNumberOfLiteralsPerInitialClause() const;
  float averageNumberOfLiteralsPerLearntClause() const;
  unsigned numberOfZeroLevelLiterals() const;

  void printReport() const;

  void printHeader() const;
  void printStatistics() const;
  void printFooter() const;

  void clear() {
    _numberOfDecisions = 0;
    _numberOfConflicts = 0;
    _numberOfRestarts = 0;
    _numberOfPropagations = 0;
  }
};

}//namespace ArgoSat

#endif
