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
#include <cstring>

#include <fstream>
#include <iterator>

#include "Auxiliary.hpp"
#include "LiteralPolaritySelectionStrategy.hpp"
#include "Solver.hpp"

namespace ArgoSat {

void LiteralPolaritySelectionStrategyPolaritySaving::init() {
   if (_inited)
	return;
   size_t numVars = _solver.getNumberOfVariables();
  _savedPolarity.resize(numVars);

  if (_useLiteralCountingInitialization) {
    LiteralOccurenceCounter counter(numVars);
    counter.count(_solver.getInitialClauses());
    for (Variable v = 0; v < numVars; v++) {
      Literal pos = Literals::literal(v, true);
      Literal neg = Literals::literal(v, false);
      if (counter.getCount(pos) > counter.getCount(neg))
	_savedPolarity[v] = true;
      else
	_savedPolarity[v] = false;	
    }
  } else if (_useFileInitialization != "") {
      std::ifstream istr(_useFileInitialization.c_str());
      std::istream_iterator<int> i(istr), end;
      while (i != end) {
	  if (*i > 0) {
	      _savedPolarity[*i - 1] = true;
	  } else if (*i < 0) {
	      _savedPolarity[-*i - 1] = false;
	  }
	  i++;
      }
      istr.close();
  } else {
    _savedPolarity.assign(numVars, false);
  }
      
  _inited = true;
}


LiteralPolaritySelectionStrategy* LiteralPolaritySelectionStrategy::createFromCmdLine(Solver& solver, int argc, char* argv[], int& curr) {
  if (curr >= argc) 
    throw std::string("LiteralPolaritySelectionStrategy specification expected");

  if (strcmp(argv[curr], "false") == 0)
    return LiteralPolaritySelectionStrategyAlwaysFalse::createFromCmdLine(solver, argc, argv, ++curr);
  if (strcmp(argv[curr], "true") == 0)
    return LiteralPolaritySelectionStrategyAlwaysTrue::createFromCmdLine(solver, argc, argv, ++curr);
  if (strcmp(argv[curr], "random") == 0)
    return LiteralPolaritySelectionStrategyRandom::createFromCmdLine(solver, argc, argv, ++curr);
  if (strcmp(argv[curr], "polarity_saving") == 0)
    return LiteralPolaritySelectionStrategyPolaritySaving::createFromCmdLine(solver, argc, argv, ++curr);

  throw std::string("Unknown LiteralPolaritySelectionStrategy: ") + argv[curr];
}

LiteralPolaritySelectionStrategy* LiteralPolaritySelectionStrategyAlwaysTrue::createFromCmdLine(Solver& /*solver*/, int /*argc*/, char** /*argv*/, int& /*curr*/) {
  return new LiteralPolaritySelectionStrategyAlwaysTrue();
}

LiteralPolaritySelectionStrategy* LiteralPolaritySelectionStrategyAlwaysFalse::createFromCmdLine(Solver& /*solver*/, int /*argc*/, char** /*argv*/, int& /*curr*/) {
  return new LiteralPolaritySelectionStrategyAlwaysFalse();
}

LiteralPolaritySelectionStrategy* LiteralPolaritySelectionStrategyPolaritySaving::createFromCmdLine(Solver& solver, int argc, char* argv[], int& curr) {
  bool useLiteralCountingInitialization = false;
  std::string useFileInitialization = "";
  if (curr < argc && strcmp(argv[curr], "init") == 0) {
      curr++;
      useLiteralCountingInitialization = true;
  } else if (curr < argc && strcmp(argv[curr], "file") == 0) {
      curr++;
      if (curr >= argc)
	  throw std::string("File name expected");
      else 
	  useFileInitialization = argv[curr++];
  }
  return new LiteralPolaritySelectionStrategyPolaritySaving(
		   solver, useLiteralCountingInitialization, useFileInitialization);
}

LiteralPolaritySelectionStrategy* LiteralPolaritySelectionStrategyRandom::createFromCmdLine(Solver& /*solver*/, int argc, char* argv[], int& curr) {
  if (curr >= argc || !isFloat(argv[curr]))
    throw std::string("LiteralPolaritySelectionStrategyRandom: percentage positive expected");

  double percentPositive = atof(argv[curr++]);
  return new LiteralPolaritySelectionStrategyRandom(percentPositive);
}


}//namespace ArgoSat
