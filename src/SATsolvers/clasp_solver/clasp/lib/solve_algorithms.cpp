// 
// Copyright (c) 2006-2007, Benjamin Kaufmann
// 
// This file is part of Clasp. See http://www.cs.uni-potsdam.de/clasp/ 
// 
// Clasp is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// Clasp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Clasp; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

/************************************************************************************
This file is modified version of the original. The modifications are
made so the clasp solver can return models on demand and for the 
purpose of the URSA system

URSA -- Copyright (c) 2010, Predrag Janicic
**************************************************************************************/


#include <clasp/include/solve_algorithms.h>
#include <clasp/include/solver.h>
#include <clasp/include/smodels_constraints.h>
#include <cmath>


using std::log;
namespace Clasp { 

Enumerator::Enumerator()  {}
Enumerator::~Enumerator() {}
Constraint::PropResult 
Enumerator::propagate(const Literal&, uint32&, Solver&) { return PropResult(true, false); }
void Enumerator::reason(const Literal&, LitVec&)    {}
ConstraintType Enumerator::type() const             { return Constraint_t::native_constraint; }

/////////////////////////////////////////////////////////////////////////////////////////
// SolveParams
/////////////////////////////////////////////////////////////////////////////////////////
SolveParams::SolveParams() 
	: reduceInc_(1.1)
	, restartInc_(1.5)
	, randFreq_(0.0)
	, enumerator_(0)
	, reduceBase_(0)
	, reduceMax_(uint32(-1))
	, restartBase_(100)
	, restartOuter_(0)
	, randRuns_(0), randConflicts_(0)
	, shuffleFirst_(0), shuffleNext_(0)
	, restartBounded_(false)
	, restartLocal_(false)
	, reduceOnRestart_(false) {
}

/////////////////////////////////////////////////////////////////////////////////////////
// Restarts
/////////////////////////////////////////////////////////////////////////////////////////
namespace {
// Implements clasp's configurable restart-strategies.
// Note: Currently all restart-strategies can be easily implemented using one class.
// In the future, as new restart strategies emerge, the class should be replaced with a strategy hierarchy
struct RestartStrategy {
public:
	RestartStrategy(uint32 base, double incA = 0.0, uint32 outer = 0) : incA_(incA), outer_(outer?outer:uint64(-1)), base_(base), idx_(0) {}
	uint64 next() {
		uint64 x;
		if      (base_ == 0)      x = uint64(-1);
		else if (incA_ == 0)      x = lubyR();
		else                      x = innerOuterR();
		++idx_;
		return x;
	}
private:
	uint64 arithR() const { return static_cast<uint64>(base_ * pow(incA_, (double)idx_)); }
	uint64 lubyR() const {
		uint32 k = idx_+1;
		while (k) {
			uint32 nk = static_cast<uint32>(log((double)k) / log(2.0)) + 1;
			if (k == ((1u << nk) - 1)) {
				return base_ * (1u << (nk-1));
			}
			k -= 1u << (nk-1);
			++k;
		}
		return base_;
	}
	uint64 innerOuterR() {
		uint64 x = arithR();
		if (x > outer_) {
			idx_    = 0;
			outer_  = static_cast<uint64>(outer_*1.5);
			return arithR();
		}
		return x;
	}
	double  incA_;
	uint64  outer_;
	uint32  base_;
	uint32  idx_;
};

}
/////////////////////////////////////////////////////////////////////////////////////////
// solve
/////////////////////////////////////////////////////////////////////////////////////////

//#define PRINT_SEARCH_PROGRESS

bool solve(Solver& s, const SolveParams& p,int *solve_state) {
 
	static  double maxLearnts;
	static double boundLearnts;
  static RestartStrategy rs(p.restartBase(), p.restartInc(), p.restartOuter());
  static ValueRep result;
  static uint32 randRuns;
  static double randFreq;
  static uint64 maxCfl;
  static uint32 shuffle;
  static int state=0;
  static bool r;

  switch(*solve_state) {

    case 0:
	    if (s.hasConflict()) return false;
	    maxLearnts   = p.reduceBase() != 0
                          ? p.reduceBase()
                          : std::min(s.numVars(), s.numConstraints()) / 3;
	    boundLearnts = p.reduceMax();
	    if (maxLearnts < s.numLearntConstraints()) {
		    maxLearnts = std::min(maxLearnts+s.numConstraints(), (double)std::numeric_limits<uint32>::max());
	    }
	    //RestartStrategy rs(p.restartBase(), p.restartInc(), p.restartOuter());
  /*	  ValueRep result = value_free;
	    uint32 randRuns = p.randRuns();
	    double randFreq = randRuns == 0 ? p.randomProbability() : 1.0;
	    uint64 maxCfl   = randRuns == 0 ? rs.next() : p.randConflicts();
	    uint32 shuffle  = p.shuffleBase();*/
	    result = value_free;
	    randRuns = p.randRuns();
	    randFreq = randRuns == 0 ? p.randomProbability() : 1.0;
	    maxCfl   = randRuns == 0 ? rs.next() : p.randConflicts();
	    shuffle  = p.shuffleBase();
      *solve_state=1;
      return true;

    case 1:
	    if (result == value_free) {
		    result = s.search(maxCfl, (uint32)maxLearnts, randFreq, p.restartLocal());
		    if (result == value_true) {
			    if (!p.enumerator()) 
            *solve_state=5;
          else {
            state=0;
            *solve_state=2;
          }
        } 
        else 
          *solve_state=4;
      }
      else 
        *solve_state=5;
      return true;

    case 2:    
    case 3:    
          {   
          r = !p.enumerator()->backtrackFromModel(s,&state);
          if (state==1)  {
             *solve_state=3; // loop
             return true;
          }

          if (r) {
            *solve_state=5;
            return true;
  		    }

			    result = value_free; // continue enumeration
			    randRuns = 0;        // but cancel remaining probings
			    randFreq = p.randomProbability();
			    if (!p.restartBounded() && !p.enumerator()->allowRestarts()) {
				    // Afert the first solution was found, we allow further restarts only if this
				    // is compatible with the enumerator used. 
				    maxCfl = static_cast<uint64>(-1);	
			    }
          *solve_state=1;
          return true;

         }

    case 4: 
        if (result == value_free) {  // restart search
			    if (randRuns == 0) {
				    maxCfl = rs.next();
				    if (p.reduceRestart()) { s.reduceLearnts(.33f); }
				    if (maxLearnts != (double)std::numeric_limits<uint32>::max() && maxLearnts < boundLearnts && (s.numLearntConstraints()+maxCfl) > maxLearnts) {
					    maxLearnts = std::min(maxLearnts*p.reduceInc(), (double)std::numeric_limits<uint32>::max());
				    }
				    if (++s.stats.restarts == shuffle) {
					    shuffle += p.shuffleNext();
					    s.shuffleOnNextSimplify();
				    }
			    }
			    else if (--randRuns == 0) {
				    maxCfl    = rs.next();
				    randFreq  = p.randomProbability();
			    } 
		    }
        *solve_state=1;
        return true;

   case 5:     

      	if (p.enumerator()) 
          p.enumerator()->endSearch(s);
	      result = s.decisionLevel() > s.rootLevel() ? value_true : value_false;
      	s.undoUntil(0);
        *solve_state=6;
	      return result == value_true;
   
    default: return true;
  }

}

bool solve(Solver& s, const LitVec& assumptions, const SolveParams& p) {
	if (s.hasConflict()) return false;
	if (!assumptions.empty() && !s.simplify()) {
		return false;
	}
	for (LitVec::size_type i = 0; i != assumptions.size(); ++i) {
		Literal p = assumptions[i];
		if (!s.isTrue(p) && (s.isFalse(p) || !(s.assume(p)&&s.stats.choices--) || !s.propagate())) {
			s.clearAssumptions();
			return false;
		}
	}
	s.setRootLevel(s.decisionLevel());
	bool ret = solve(s, p, 0);
	s.clearAssumptions();
	return ret;
}

}
