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
#include "SolverStatistics.hpp"
#include "Solver.hpp"

namespace ArgoSat {
  SolverStatistics::SolverStatistics(Solver& solver) 
    : _solver(solver),
      _numberOfDecisions(0),
      _numberOfConflicts(0),
      _numberOfPropagations(0),
      _numberOfRestarts(0) {
    _solver.addListener(this);
  }

  SolverStatistics::~SolverStatistics() {
    _solver.removeListener(this);
  }


  unsigned SolverStatistics::getNumberOfLiterals(const std::vector<const Clause*>& clauses) {
    unsigned result = 0;
    std::vector<const Clause*>::const_iterator i;
    for (i = clauses.begin(); i != clauses.end(); i++) {
      result += (*i)->size();
    }
    return result;
  }

  float SolverStatistics::averageNumberOfLiteralsPerInitialClause() const {
    return ((float)getNumberOfLiterals(_solver.getInitialClauses())) / 
      ((float)(_solver.getInitialClauses().size()));
  }

  float SolverStatistics::averageNumberOfLiteralsPerLearntClause() const {
    return ((float)getNumberOfLiterals(_solver.getLearntClauses())) / 
      ((float)(_solver.getLearntClauses().size()));
  }


  unsigned SolverStatistics::numberOfZeroLevelLiterals() const {
    return _solver.getTrail().sizeUntilLevel(0);
  }

  void SolverStatistics::printHeader() const {
    printf("\n----------------------------------------------------------------------------------------------\n");
    printf("| R. | Decisions | Initial+Learnt Clauses (L/C)   | Conflicts | Propagations | 0. lvl / vars |\n");
    printf("----------------------------------------------------------------------------------------------\n");
  }

  void SolverStatistics::printStatistics() const {
    printf("| %2d | %9d |  %8lu + %9lu (%5.2f)  | %9d |  %10d  | %5d/%5lu   |\n", 
	   _numberOfRestarts,
	   _numberOfDecisions, 
	   _solver.getNumberOfInitialClauses(),
	   _solver.getNumberOfLearntClauses(),
	   averageNumberOfLiteralsPerLearntClause(),
	   _numberOfConflicts, 
	   _numberOfPropagations,
	   numberOfZeroLevelLiterals(),
	   _solver.getNumberOfVariables()
	   );
  }

  void SolverStatistics::printFooter() const {
    printf("----------------------------------------------------------------------------------------------\n");
  }

  void SolverStatistics::printReport() const {
    printHeader();
    printStatistics();
    printFooter();
  }

}//namespace ArgoSat
