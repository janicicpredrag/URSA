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
#include <clasp/include/solver.h>
#include <clasp/include/clause.h>
#include <clasp/include/util/misc_types.h>
#include <functional>
#include <stdexcept>

#ifdef _MSC_VER
#pragma warning (disable : 4355) // 'this' used in base member initializer list - intended & safe
#pragma warning (disable : 4702) // unreachable code - intended
#endif

namespace Clasp { 

uint32 randSeed_g = 1;

SatPreprocessor::~SatPreprocessor() {}
DecisionHeuristic::~DecisionHeuristic() {}
PostPropagator::PostPropagator() {}
PostPropagator::~PostPropagator() {}
void PostPropagator::reset() {}

/////////////////////////////////////////////////////////////////////////////////////////
// SelectFirst selection strategy
/////////////////////////////////////////////////////////////////////////////////////////
// selects the first free literal
Literal SelectFirst::doSelect(Solver& s) {
	for (Var i = 1; i <= s.numVars(); ++i) {
		if (s.value(i) == value_free) {
			return s.preferredLiteralByType(i);
		}
	}
	assert(!"SelectFirst::doSelect() - precondition violated!\n");
	return Literal();
}
/////////////////////////////////////////////////////////////////////////////////////////
// SelectRandom selection strategy
/////////////////////////////////////////////////////////////////////////////////////////
// Selects a random literal from all free literals.
class SelectRandom : public DecisionHeuristic {
public:
	SelectRandom() : randFreq_(1.0), pos_(0) {}
	void shuffle() {
		std::random_shuffle(vars_.begin(), vars_.end(), irand);
		pos_ = 0;
	}
	void randFreq(double d) { randFreq_ = d; }
	double randFreq() const { return randFreq_; }
	void endInit(const Solver& s) {
		vars_.clear();
		for (Var i = 1; i <= s.numVars(); ++i) {
			if (s.value( i ) == value_free) {
				vars_.push_back(i);
			}
		}
		pos_ = 0;
	}
private:
	Literal doSelect(Solver& s) {
		LitVec::size_type old = pos_;
		do {
			if (s.value(vars_[pos_]) == value_free) {
				Literal l = preferredLiteral(s, vars_[pos_]);
				return l != posLit(0)
					? l
					: s.preferredLiteralByType(vars_[pos_]);
			}
			if (++pos_ == vars_.size()) pos_ = 0;
		} while (old != pos_);
		assert(!"SelectRandom::doSelect() - precondition violated!\n");
		return Literal();
	}
	VarVec            vars_;
	double            randFreq_;
	VarVec::size_type pos_;
};
/////////////////////////////////////////////////////////////////////////////////////////
// SolverStrategies
/////////////////////////////////////////////////////////////////////////////////////////
SolverStrategies::SolverStrategies()
	: satPrePro()
	, heuristic(new SelectFirst) 
	, postProp(new NoPostPropagator)
	, search(use_learning)
	, cflMin(beame_minimization)
	, cflMinAntes(all_antes)
	, randomWatches(false)
	, saveProgress(false) 
	, compress_(250) {
}
PostPropagator* SolverStrategies::releasePostProp() {
	PostPropagator* p = postProp.release();
	postProp.reset(new NoPostPropagator);
	return p;
}
/////////////////////////////////////////////////////////////////////////////////////////
// Solver: Construction/Destruction/Setup
////////////////////////////////////////////////////////////////////////////////////////
Solver::Solver() 
	: strategy_()
	, randHeuristic_(0)
	, levConflicts_(0)
	, undoHead_(0)
	, front_(0)
	, binCons_(0)
	, ternCons_(0)
	, lastSimplify_(0)
	, rootLevel_(0)
	, btLevel_(0)
	, eliminated_(0)
	, shuffle_(false) {
	// every solver contains a special sentinel var that is always true
	Var sentVar = addVar( Var_t::atom_body_var );
	assign(sentVar, value_true);
	markSeen(sentVar);
	ConstraintDB* temp[100];
	// Pre-allocate 100 undo lists
	for (uint32 i = 0; i != 100; ++i) { temp[i] = allocUndo(); }
	for (uint32 i = 0; i != 100; ++i) { undoFree(temp[i]); }
}

Solver::~Solver() {
	freeMem();
}

void Solver::freeMem() {
	std::for_each( constraints_.begin(), constraints_.end(), DestroyObject());
	std::for_each( learnts_.begin(), learnts_.end(), DestroyObject() );
	constraints_.clear();
	learnts_.clear();
	PodVector<WL>::destruct(watches_);
	delete levConflicts_;
	delete randHeuristic_;
	// free undo lists
	// first those still in use
	for (TrailLevels::size_type i = 0; i != levels_.size(); ++i) {
		delete levels_[i].second;
	}
	// then those in the free list
	for (VecLayout* x = undoHead_; x; ) {
		VecLayout* t = x;
		x = x->last;
		t->last = t->start;
		delete (ConstraintDB*) t;
	}
}

void Solver::reset() {
	// hopefully, no one derived from this class...
	this->~Solver();
	new (this) Solver();
}
/////////////////////////////////////////////////////////////////////////////////////////
// Solver: Problem specification
////////////////////////////////////////////////////////////////////////////////////////
void Solver::startAddConstraints() {
	watches_.resize(assign_.size()<<1);
	trail_.reserve(numVars());
	strategy_.heuristic->startInit(*this);
}

bool Solver::endAddConstraints() {
	Antecedent::checkPlatformAssumptions();
	if (strategy_.satPrePro.get() != 0) {
		SolverStrategies::SatPrePro temp(strategy_.satPrePro.release());
		bool r = temp->preprocess();
		strategy_.satPrePro.reset(temp.release());
		if (!r) return false;
	}
	if (!simplify()) return false;
	stats.native[0] = numConstraints();
	stats.native[1] = numBinaryConstraints();
	stats.native[2] = numTernaryConstraints();
	strategy_.heuristic->endInit(*this);  
	if (randHeuristic_) randHeuristic_->endInit(*this);
	return true;
}

Var Solver::addVar(VarType t) {
	Var v = (uint32)assign_.size();
	reason_.push_back(Antecedent());
	assign_.push_back(0);
	extra_.add(t == Var_t::body_var);
	return v;
}

void Solver::eliminate(Var v, bool elim) {
	assert(validVar(v)); 
	if (elim && !eliminated(v)) {
		assert(value(v) == value_free && "Can not eliminate assigned var!\n");
		extra_.setEliminated(v, true);
		markSeen(v);
		// so that the var is ignored by heuristics
		assign(v, value_true);
		++eliminated_;
	}
	else if (!elim && eliminated(v)) {
		extra_.setEliminated(v, false);
		clearSeen(v);
		undo(v);
		--eliminated_;
		strategy_.heuristic->resurrect(v);
	}
}

bool Solver::addUnary(Literal p) {
	if (decisionLevel() != 0) {
		impliedLits_.push_back(ImpliedLiteral(p, 0, 0));
	}
	else {
		markSeen(p.var());
	}
	return force(p, 0);
}

bool Solver::addBinary(Literal p, Literal q, bool asserting) {
	assert(validWatch(~p) && validWatch(~q) && "ERROR: startAddConstraints not called!");
	++binCons_;
	watches_[(~p).index()].add(q);
	watches_[(~q).index()].add(p);
	return !asserting || force(p, ~q);
}

bool Solver::addTernary(Literal p, Literal q, Literal r, bool asserting) {
	assert(validWatch(~p) && validWatch(~q) && validWatch(~r) && "ERROR: startAddConstraints not called!");
	assert(p != q && q != r && "ERROR: ternary clause contains duplicate literal");
	++ternCons_;
	watches_[(~p).index()].addClause(Watch(q, r));
	watches_[(~q).index()].addClause(Watch(p, r));
	watches_[(~r).index()].addClause(Watch(p, q));
	return !asserting || force(p, Antecedent(~q, ~r));
}
/////////////////////////////////////////////////////////////////////////////////////////
// Solver: Watch management
////////////////////////////////////////////////////////////////////////////////////////
uint32 Solver::numWatches(Literal p) const {
	assert( validVar(p.var()) );
	if (!validWatch(p)) return 0;
	return (uint32)watches_[p.index()].size();
}
	
bool Solver::hasWatch(Literal p, Constraint* c) const {
	if (!validWatch(p) || !watches_[p.index()].constraints) return false;
	const WatchList& pList = *watches_[p.index()].constraints;
	return std::find(pList.begin(), pList.end(), c) != pList.end();		
}

bool Solver::hasWatch(Literal p, Clause* c) const {
	if (!validWatch(p)) return false;
	const WatchList& pList = watches_[p.index()].clauses;
	return std::find(pList.begin(), pList.end(), c) != pList.end();		
}


Watch* Solver::getWatch(Literal p, Constraint* c) const {
	if (!validWatch(p) || !watches_[p.index()].constraints) return 0;
	const WatchList& pList = *watches_[p.index()].constraints;
	WatchList::const_iterator it = std::find(pList.begin(), pList.end(), c);
	return it != pList.end()
		? &const_cast<Watch&>(*it)
		: 0;
}

void Solver::removeWatch(const Literal& p, Constraint* c) {
	assert(validWatch(p));
	if (watches_[p.index()].constraints) {
		WatchList& pList = *watches_[p.index()].constraints;
		WatchList::iterator it = std::find(pList.begin(), pList.end(), c);
		if (it != pList.end()) {
			pList.erase(it);
		}
	}
}
void Solver::removeWatch(const Literal& p, Clause* c) {
	assert(validWatch(p));
	WatchList& pList = watches_[p.index()].clauses;
	WatchList::iterator it = std::find(pList.begin(), pList.end(), c);
	if (it != pList.end()) {
		pList.erase(it);
	}
}

void Solver::removeUndoWatch(uint32 dl, Constraint* c) {
	assert(dl != 0 && dl <= decisionLevel() );
	if (levels_[dl-1].second) {
		ConstraintDB& uList = *levels_[dl-1].second;
		ConstraintDB::iterator it = std::find(uList.begin(), uList.end(), c);
		if (it != uList.end()) {
			*it = uList.back();
			uList.pop_back();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////////
// Solver: Basic DPLL-functions
////////////////////////////////////////////////////////////////////////////////////////
void Solver::initRandomHeuristic(double randFreq) {
	randFreq = std::min(1.0, std::max(0.0, randFreq));
	if (randFreq == 0.0) {
		delete randHeuristic_;
		randHeuristic_ = 0;
		return;
	}
	if (!randHeuristic_) {
		randHeuristic_ = new SelectRandom();
		randHeuristic_->endInit(*this);
	}
	static_cast<SelectRandom*>(randHeuristic_)->shuffle();
	static_cast<SelectRandom*>(randHeuristic_)->randFreq(randFreq);
}


// removes all satisfied binary and ternary clauses as well
// as all constraints for which Constraint::simplify returned true.
bool Solver::simplify() {
	if (decisionLevel() != 0) return true;
	if (trail_.empty()) strategy_.heuristic->simplify(*this, 0);
	
	// Loop because heuristic may apply failed-literal-detection and
	// therefore force some literals.
	while (lastSimplify_ < trail_.size() || shuffle_) {
		LitVec::size_type old = lastSimplify_;
		if (!propagate()) return false;   // Top-Level-conflict
		simplifySAT();                    // removes SAT-Constraints
		assert(lastSimplify_ == trail_.size());
		strategy_.heuristic->simplify(*this, old);    
	}
	return true;
}

void Solver::simplifySAT() {
	for (; lastSimplify_ < trail_.size(); ++lastSimplify_) {
		Literal p = trail_[lastSimplify_];
		markSeen(p.var());                    // ignore level 0 literals during conflict analysis
		simplifyShort(p);                     // remove satisfied binary- and ternary clauses
	}
	if (shuffle_) {
		std::random_shuffle(constraints_.begin(), constraints_.end(), irand);
		std::random_shuffle(learnts_.begin(), learnts_.end(), irand);
	}
	simplifyDB(constraints_);
	simplifyDB(learnts_);
	shuffle_ = false;
}

void Solver::simplifyDB(ConstraintDB& db) {
	ConstraintDB::size_type i, j, end = db.size();
	for (i = j = 0; i != end; ++i) {
		Constraint* c = db[i];
		if (c->simplify(*this, shuffle_)) { c->destroy(); }
		else                              { db[j++] = c;  }
	}
	db.erase(db.begin()+j, db.end());
}

// removes all binary clauses containing p - those are now SAT
// binary clauses containing ~p are unit and therefore likewise SAT. Those
// are removed when their second literal is processed.
// Note: Binary clauses containing p are those that watch ~p.
//
// Simplifies ternary clauses.
// Ternary clauses containing p are SAT and therefore removed.
// Ternary clauses containing ~p are now either binary or SAT. Those that
// are SAT are removed when the satisfied literal is processed. 
// All conditional binary-clauses are replaced with a real binary clause.
// Note: Ternary clauses containing p watch ~p. Those containing ~p watch p.
// Note: Those clauses are now either binary or satisfied.
void Solver::simplifyShort(Literal p) {
	WL& pList     = watches_[p.index()];
	WL& negPList  = watches_[(~p).index()];
	releaseVec( pList.binary ); // this list was already propagated
	binCons_    -= (uint32)negPList.binary.size();
	for (LitVec::size_type i = 0; i < negPList.binary.size(); ++i) {
		remove_first_if(watches_[(~negPList.binary[i]).index()].binary, std::bind2nd(std::equal_to<Literal>(), p));
	}
	releaseVec(negPList.binary);
	
	// remove every ternary clause containing p -> clause is satisfied
	WatchList& ptList = negPList.clauses;
	for (LitVec::size_type i = 0; i < ptList.size(); ++i) {
		if (ptList[i].isTernary()) {
			--ternCons_;
			remove_first_if(watches_[(~ptList[i].first()).index()].clauses, TernaryContains(p));
			remove_first_if(watches_[(~ptList[i].second()).index()].clauses, TernaryContains(p));
		}
	}
	releaseVec(ptList);
	// transform ternary clauses containing ~p to binary clause
	WatchList& npList = pList.clauses;
	for (LitVec::size_type i = 0; i < npList.size(); ++i) {
		if (npList[i].isTernary()) {
			const Literal& q = npList[i].first();
			const Literal& r = npList[i].second();
			if (value(q.var()) == value_free && value(r.var()) == value_free) {
				// clause is binary on dl 0
				--ternCons_;
				remove_first_if(watches_[(~q).index()].clauses, TernaryContains(~p));
				remove_first_if(watches_[(~r).index()].clauses, TernaryContains(~p));
				addBinary(q, r, false);
			}
			// else: clause is SAT and removed when the satisfied literal is processed
		}
	}
	releaseVec(npList);
	if (pList.constraints) {
		delete pList.constraints;	    // updated during propagation. 
		pList.constraints = 0;
	}
	if (negPList.constraints) {
		delete negPList.constraints; // ~p will never be true. List is no longer relevant.
		negPList.constraints = 0;
	}
}


bool Solver::force(const Literal& p, const Antecedent& c) {
	assert((!hasConflict() || isTrue(p)) && !eliminated(p.var()));
	const Var var = p.var();
	if (value(var) == value_free) {
		assign(var, trueValue(p));
		reason_[var] = c;
		trail_.push_back(p);
	}
	else if (value(var) == falseValue(p)) {   // conflict
		if (strategy_.search != SolverStrategies::no_learning && !c.isNull()) {
			c.reason(p, conflict_);
		}
		conflict_.push_back(~p);
		return false;
	}
	return true;
}

bool Solver::assume(const Literal& p) {
	assert( value(p.var()) == value_free );
	++stats.choices;
	levels_.push_back(LevelInfo((uint32)trail_.size(), 0));
	if (levConflicts_) levConflicts_->push_back( (uint32)stats.conflicts );
	return force(p, Antecedent());  // always true
}

bool Solver::propagate() {
	if (!unitPropagate() || !strategy_.postProp->propagate()) {
		front_ = trail_.size();
		strategy_.postProp->reset();
		return false;
	}
	assert(queueSize() == 0);
	return true;
}

bool Solver::unitPropagate() {
	assert(!hasConflict());
	Constraint::PropResult r;
	while (front_ < trail_.size()) {
		const Literal& p = trail_[front_++];
		uint32 idx = p.index();
		WL& wl = watches_[idx];
		LitVec::size_type i, j, bEnd = wl.binary.size(), clEnd = wl.clauses.size();
		// first, do binary BCP...    
		for (i = 0; i != bEnd; ++i) {
			if (!isTrue(wl.binary[i]) && !force(wl.binary[i], p)) {
				return false;
			}
		}
		// then, do clause BCP...
		j = 0;
		for (i = 0; i != clEnd;) {
			Watch& w  = wl.clauses[i];
			if (i != j) { wl.clauses[j] = w; }
			++i, ++j;
			if (!isTrue(w.first())) {
				if (!w.isTernary()) {
					r = w.clause()->Clause::propagate(p, w.data(), *this);
					assert(!w.isTernary());
					j -= !r.second;
					if (!r.first)  {
						copyDown(wl.clauses, i, j);
						return false;
					}
				}
				else if (isFalse(w.first()) && !force(w.second(), Antecedent(p, ~w.first()))) {
					copyDown(wl.clauses, i, j);
					return false;
				}
				else if (isFalse(w.second()) && !force(w.first(), Antecedent(p, ~w.second()))) {
					copyDown(wl.clauses, i, j);
					return false;
				}
			}
		}
		shrinkVecTo(wl.clauses, j);
		// and finally do general BCP
		if (wl.constraints) {
			j = 0, i = 0;
			WatchList& cons = *wl.constraints;
			for (LitVec::size_type end = cons.size(); i != end;) {
				Watch& w   = cons[i];
				if (i != j) { cons[j]= w; }
				++i;
				r = w.constraint()->propagate(p, w.data(), *this);
				j += r.second;
				if (!r.first)  {
					copyDown(cons, i, j);
					return false;
				}
			}
			shrinkVecTo(cons, j);
		}
	}
	return true;
}

bool Solver::failedLiteral(Var& var, VarScores& scores, VarType types, bool uniform, VarVec& deps) {
	assert(validVar(var));
	scores.resize( assign_.size() );
	deps.clear();
	Var oldVar = var;
	bool cfl = false;
	do {
		if ( (type(var) & types) != 0 && value(var) == value_free ) {
			Literal p = preferredLiteralByType(var);
			bool testBoth = uniform || type(var) == Var_t::atom_body_var;
			if (!scores[var].seen(p) && !lookahead(p, scores,deps, types, true)) {
				cfl = true;
				break;
			}
			if (testBoth && !scores[var].seen(~p) && !lookahead(~p, scores,deps, types, true)) {
				cfl = true;
				break;
			}
		}
		if (++var > numVars()) var = 0;
	} while (!cfl && var != oldVar);
	if (cfl) {
		scores[var].clear();
		for (VarVec::size_type i = 0; i < deps.size(); ++i) {
			scores[deps[i]].clear();
		}
		deps.clear();
	}   
	return cfl;
}

bool Solver::resolveConflict() {
	assert(hasConflict());
	++stats.conflicts;
	if (decisionLevel() > rootLevel_) {
		if (decisionLevel() != btLevel_ && strategy_.search != SolverStrategies::no_learning) {
			uint32 sw;
			uint32 uipLevel = analyzeConflict(sw);
			updateJumps(stats, decisionLevel(), uipLevel, btLevel_);
			undoUntil( uipLevel );
			bool ret = ClauseCreator::createClause(*this, Constraint_t::learnt_conflict, cc_, sw);
			if (uipLevel < btLevel_) {
				assert(decisionLevel() == btLevel_);
				// logically the asserting clause is unit on uipLevel but the backjumping
				// is bounded by btLevel_ thus the uip is asserted on that level. 
				// We store enough information so that the uip can be re-asserted once we backtrack below btLevel.
				impliedLits_.push_back( ImpliedLiteral( trail_.back(), uipLevel, reason(trail_.back()) ));
			}
			assert(ret);
			return ret;
		}
		else {
			return backtrack();
		}
	}
	return false;
}

bool Solver::backtrack() {
	if (decisionLevel() == rootLevel_) return false;
	Literal lastChoiceInverted = ~decision(decisionLevel());
	btLevel_ = decisionLevel() - 1;
	undoUntil(btLevel_);
	bool r = force(lastChoiceInverted, 0);
	ImpliedLits::size_type j = 0;
	for (ImpliedLits::size_type i = 0; i < impliedLits_.size(); ++i) {
		r = r && force(impliedLits_[i].lit, impliedLits_[i].ante);
		if (impliedLits_[i].level != btLevel_) {
			impliedLits_[j++] = impliedLits_[i];
		}
	}
	impliedLits_.erase(impliedLits_.begin()+j, impliedLits_.end());
	return r || backtrack();
}

void Solver::undoUntil(uint32 level) {
	assert(btLevel_ >= rootLevel_);
	level = std::max( level, btLevel_ );
	if (level >= decisionLevel()) return;
	conflict_.clear();
	strategy_.heuristic->undoUntil( *this, levels_[level].first);
	bool sp = false;
	do {
		undoLevel(sp);
		sp = strategy_.saveProgress;
	} while (decisionLevel() > level);
}

bool Solver::lookahead(Literal p, VarScores& scores, VarVec& deps, VarType types, bool addDeps) {
	LitVec::size_type old = trail_.size();
	bool ok = assume(p) && propagate();
	if (ok) {
		scores[p.var()].setScore(p, trail_.size() - old);
		if (addDeps) {
			for (; old < trail_.size(); ++old) {
				Var v = trail_[old].var();
				if ( (type(v) & types) != 0) {
					if (!scores[v].seen()) { deps.push_back(v); }
					scores[v].setSeen(trail_[old]);
				}
			}
		}
		undoUntil(decisionLevel()-1);
	}
	else {
		resolveConflict();
		--stats.conflicts;
	}
	--stats.choices;
	return ok;
}

uint32 Solver::estimateBCP(const Literal& p, int rd) const {
	assert(front_ == trail_.size());
	if (value(p.var()) != value_free) return 0;
	LitVec::size_type i = front_;
	Solver& self = const_cast<Solver&>(*this);
	self.assign(p.var(), trueValue(p));
	self.trail_.push_back(p);
	do {
		Literal x = trail_[i++];  
		const LitVec& xList = watches_[x.index()].binary;
		for (LitVec::size_type k = 0; k < xList.size(); ++k) {
			Literal y = xList[k];
			if (value(y.var()) == value_free) {
				self.assign(y.var(), trueValue(y));
				self.trail_.push_back(y);
			}
		}
	} while (i < trail_.size() && rd-- != 0);
	i = trail_.size() - front_;
	while (trail_.size() != front_) {
		self.undo(self.trail_.back().var());
		self.trail_.pop_back();
	}
	return (uint32)i;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Solver: Private helper functions
////////////////////////////////////////////////////////////////////////////////////////
// removes the current decision level
void Solver::undoLevel(bool sp) {
	assert(decisionLevel() != 0);
	LitVec::size_type numUndo = trail_.size() - levels_.back().first;
	assert(numUndo > 0 && "Decision level must not be empty!");
	if (!sp) {
		while (numUndo-- != 0) {
			undo(trail_.back().var());
			trail_.pop_back();
		}
	}
	else {
		while (numUndo-- != 0) {
			saveValueAndUndo(trail_.back());
			trail_.pop_back();
		}
	}
	if (levels_.back().second) {
		const ConstraintDB& undoList = *levels_.back().second;
		for (ConstraintDB::size_type i = 0, end = undoList.size(); i != end; ++i) {
			undoList[i]->undoLevel(*this);
		}
		undoFree(levels_.back().second);
	}
	levels_.pop_back();
	if (levConflicts_) levConflicts_->pop_back();
	front_ = trail_.size();
}

// Computes the First-UIP clause and stores it in cc_, where cc_[0] is the asserting literal (inverted UIP).
// Returns the dl on which cc_ is asserting and stores the position of a literal 
// from this level in secondWatch.
uint32 Solver::analyzeConflict(uint32& secondWatch) {
	// must be called here, because we unassign vars during analyzeConflict
	strategy_.heuristic->undoUntil( *this, levels_.back().first );
	uint32 onLevel  = 0;        // number of literals from the current DL in resolvent
	uint32 abstr    = 0;        // abstraction of DLs in cc_
	Literal p;                  // literal to be resolved out next
	cc_.assign(1, p);           // will later be replaced with asserting literal
	strategy_.heuristic->updateReason(*this, conflict_, p);
	for (;;) {
		for (LitVec::size_type i = 0; i != conflict_.size(); ++i) {
			Literal& q = conflict_[i];
			if (!seen(q.var())) {
				assert(isTrue(q) && "Invalid literal in reason set!");
				uint32 cl = level(q.var());
				assert(cl > 0 && "Simplify not called on Top-Level - seen-flag not set!");
				markSeen(q.var());
				if (cl == decisionLevel()) {
					++onLevel;
				}
				else {
					cc_.push_back(~q);
					abstr |= (1 << (cl & 31));
				}
			}
		}
		// search for the last assigned literal that needs to be analyzed...
		while (!seen(trail_.back().var())) {
			undo(trail_.back().var());
			trail_.pop_back();
		}
		p = trail_.back();
		clearSeen(p.var());
		conflict_.clear();
		if (--onLevel == 0) {
			break;
		}
		reason(p).reason(p, conflict_);
		strategy_.heuristic->updateReason(*this, conflict_, p);
	}
	cc_[0] = ~p; // store the 1-UIP
	assert( decisionLevel() == level(p.var()));
	minimizeConflictClause(abstr);
	// clear seen-flag of all literals that are not from the current dl
	// and determine position of literal from second highest DL, which is
	// the asserting level of the newly derived conflict clause.
	secondWatch        = 1;
	uint32 assertLevel = 0;
	for (LitVec::size_type i = 1; i != cc_.size(); ++i) {
		clearSeen(cc_[i].var());
		if (level(cc_[i].var()) > assertLevel) {
			assertLevel = level(cc_[i].var());
			secondWatch = (uint32)i;
		}
	}
	return assertLevel;
}

void Solver::minimizeConflictClause(uint32 abstr) {
	uint32 m = strategy_.cflMinAntes;
	LitVec::size_type t = trail_.size();
	// skip the asserting literal
	LitVec::size_type j = 1;
	for (LitVec::size_type i = 1; i != cc_.size(); ++i) { 
		Literal p = ~cc_[i];
		if (reason(p).isNull() || ((reason(p).type()+1) & m) == 0  || !minimizeLitRedundant(p, abstr)) {
			cc_[j++] = cc_[i];
		}
		// else: p is redundant and can be removed from cc_
		// it was added to trail_ so that we can clear its seen flag
	}
	cc_.erase(cc_.begin()+j, cc_.end());
	while (trail_.size() != t) {
		clearSeen(trail_.back().var());
		trail_.pop_back();
	}
}

bool Solver::minimizeLitRedundant(Literal p, uint32 abstr) {
	if (strategy_.cflMin == SolverStrategies::beame_minimization) {
		conflict_.clear(); reason(p).reason(p, conflict_);
		for (LitVec::size_type i = 0; i != conflict_.size(); ++i) {
			if (!seen(conflict_[i].var())) {
				return false;
			}
		}
		trail_.push_back(p);
		return true;
	}
	// else: een_minimization
	LitVec::size_type start = trail_.size();
	trail_.push_back(p);
	for (LitVec::size_type f = start; f != trail_.size(); ) {
		p = trail_[f++];
		conflict_.clear();
		reason(p).reason(p, conflict_);
		for (LitVec::size_type i = 0; i != conflict_.size(); ++i) {
			p = conflict_[i];
			if (!seen(p.var())) {
				if (!reason(p).isNull() && ((1<<(level(p.var())&31)) & abstr) != 0) {
					markSeen(p.var());
					trail_.push_back(p);
				}
				else {
					while (trail_.size() != start) {
						clearSeen(trail_.back().var());
						trail_.pop_back();
					}
					return false;
				}
			}
		}
	}
	return true;
}

// Selects next branching literal. Use user-supplied heuristic if rand() < randProp.
// Otherwise makes a random choice.
// Returns false if assignment is total.
bool Solver::decideNextBranch() {
	DecisionHeuristic* heu = strategy_.heuristic.get();
	if (randHeuristic_ && drand() < static_cast<SelectRandom*>(randHeuristic_)->randFreq()) {
		heu = randHeuristic_;
	}
	return heu->select(*this);
}

// Remove upto maxRem% of the learnt nogoods.
// Keep those that are locked or have a high activity.
void Solver::reduceLearnts(float maxRem) {
	uint32 oldS = numLearntConstraints();
	ConstraintDB::size_type i, j = 0;
	if (maxRem < 1.0f) {    
		LitVec::size_type remMax = static_cast<LitVec::size_type>(numLearntConstraints() * std::min(1.0f, std::max(0.0f, maxRem)));
		uint64 actSum = 0;
		for (i = 0; i != learnts_.size(); ++i) {
			actSum += static_cast<LearntConstraint*>(learnts_[i])->activity();
		}
		double actThresh = (actSum / (double) numLearntConstraints()) * 1.5;
		for (i = 0; i != learnts_.size(); ++i) {
			LearntConstraint* c = static_cast<LearntConstraint*>(learnts_[i]);
			if (remMax == 0 || c->locked(*this) || c->activity() > actThresh) {
				c->decreaseActivity();
				learnts_[j++] = c;
			}
			else {
				--remMax;
				c->removeWatches(*this);
				c->destroy();
			}
		}
	}
	else {
		// remove all nogoods that are not locked
		for (i = 0; i != learnts_.size(); ++i) {
			LearntConstraint* c = static_cast<LearntConstraint*>(learnts_[i]);
			if (c->locked(*this)) {
				c->decreaseActivity();
				learnts_[j++] = c;
			}
			else {
				c->removeWatches(*this);
				c->destroy();
			}
		}
	}
	learnts_.erase(learnts_.begin()+j, learnts_.end());
	stats.deleted += oldS - numLearntConstraints();
}
/////////////////////////////////////////////////////////////////////////////////////////
// The basic DPLL-like search-function
/////////////////////////////////////////////////////////////////////////////////////////
ValueRep Solver::search(uint64 maxConflicts, uint32 maxLearnts, double randProp, bool localR) {
	initRandomHeuristic(randProp);
	if (localR) {
		if (!levConflicts_) levConflicts_ = new VarVec();
		levConflicts_->assign(decisionLevel()+1, (uint32)stats.conflicts);
	}
	if (!simplify()) { return value_false; }
	do {
		while (!propagate()) {
			if (!resolveConflict() || (decisionLevel() == 0 && !simplify())) {
				return value_false;
			}
			if ((!localR && --maxConflicts == 0) ||
				(localR && (stats.conflicts - (*levConflicts_)[decisionLevel()]) > maxConflicts)) {
				undoUntil(0);
				return value_free;  
			}
		}
		if (numLearntConstraints()>maxLearnts) { reduceLearnts(.75f); }
	} while (decideNextBranch());
	assert(numFreeVars() == 0);
	++stats.models;
	updateModels(stats, decisionLevel());
	if (strategy_.satPrePro.get()) {
		strategy_.satPrePro->extendModel(assign_);
	}
	return value_true;
}
}
