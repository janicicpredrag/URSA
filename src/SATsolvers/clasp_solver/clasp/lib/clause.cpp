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

#include <clasp/include/clause.h>
#include <clasp/include/solver.h>


namespace Clasp { namespace Detail {

struct GreaterLevel {
	GreaterLevel(const Solver& s) : solver_(s) {}
	bool operator()(const Literal& p1, const Literal& p2) const {
		assert(solver_.value(p1.var()) != value_free && solver_.value(p2.var()) != value_free);
		return solver_.level(p1.var()) > solver_.level(p2.var());
	}
private:
	GreaterLevel& operator=(const GreaterLevel&);
	const Solver& solver_;
};

}

/////////////////////////////////////////////////////////////////////////////////////////
// ClauseCreator
/////////////////////////////////////////////////////////////////////////////////////////
ClauseCreator::ClauseCreator(Solver* s) 
	: solver_(s)
	, type_(Constraint_t::native_constraint)
	, w2_(0)
	, sat_(0)
	, unit_(0) {
}

ClauseCreator& ClauseCreator::start(ConstraintType t) {
	assert(!solver_ || solver_->decisionLevel() == 0 || t != Constraint_t::native_constraint);
	literals_.clear();
	type_ = t;
	w2_   = t != Constraint_t::native_constraint;
	sat_  = 0;
	unit_ = 0;
	return *this;
}

ClauseCreator& ClauseCreator::startAsserting(ConstraintType t, const Literal& p) {
	literals_.assign(1, p);
	type_ = t;
	w2_   = 1;
	unit_ = 1;
	sat_  = 0;
	return *this;
}

ClauseCreator& ClauseCreator::add(const Literal& p) {
	assert(solver_); const Solver& s = *solver_;
	if (s.value(p.var()) == value_free || s.level(p.var()) > 0) {
		literals_.push_back(p);
		if (unit_ != 0 && s.level(p.var()) > s.level(literals_[w2_].var())) {
			// make sure w2_ points to the false lit assigned last
			w2_ = (uint32)literals_.size()-1;
		}
		else if (unit_ == 0 && type_ != Constraint_t::native_constraint && literals_.size()>1) {
			// make sure lits[0] and lits[w2_] are the valid watches
			uint32 lp = s.isFalse(p) ? s.level(p.var()) : uint32(-1);
			if (s.isFalse(literals_[0]) && lp > s.level(literals_[0].var())) {
				std::swap(literals_[0], literals_.back());
				lp = s.level(literals_.back().var());
			}
			if (s.isFalse(literals_[w2_]) && lp > s.level(literals_[w2_].var())) {
				w2_ = (uint32)literals_.size()-1;
			}
		}
	}
	else if (s.isTrue(p)) { sat_ = 1; }
	return *this;
}

void ClauseCreator::simplify() {
	assert(w2_ == 0 || literals_.size() == 1 || literals_[0] != literals_[w2_]);
	LitVec::iterator j = literals_.begin();
	for (LitVec::iterator i = j, end = literals_.end(); i != end; ++i) {
		if ( !solver_->seen(*i) ) {
			solver_->markSeen(*i);
			if (i != j) {
				if (*i == literals_[w2_]) {
					w2_ = static_cast<uint32>(j - literals_.begin());
				}
				*j = *i;
			}
			++j;
		}
		if (solver_->seen(~*i)) sat_ = 1;
	}
	literals_.erase(j, literals_.end());
	for (LitVec::iterator i = literals_.begin(), end = literals_.end(); i != end; ++i) {
		solver_->clearSeen(i->var());
	}
}

bool ClauseCreator::createClause(Solver& s, ConstraintType type, const LitVec& lits, uint32 sw, Constraint** out) {
	if (out) *out = 0;
	if (lits.empty()) {
		LitVec cfl(1, Literal());
		s.setConflict(cfl);
		return false;           // UNSAT on level 0
	}
	else if (type == Constraint_t::native_constraint && s.strategies().satPrePro.get()) {
		return s.strategies().satPrePro->addClause(lits);
	}
	s.strategies().heuristic->newConstraint(s, &lits[0], lits.size(), type);
	bool asserting = lits.size() == 1 || s.isFalse(lits[sw]);
	if (lits.size() < 4) {
		if (type != Constraint_t::native_constraint) updateLearnt(s.stats, lits.size(), type);
		if (lits.size() == 1) {
			return s.addUnary(lits[0]);
		}
		else if (lits.size() == 2) {
			return s.addBinary(lits[0], lits[1], asserting); 
		}
		return s.addTernary(lits[0], lits[1], lits[2], asserting);
	}
	else {                          // general clause
		Constraint* newCon;
		Literal first = lits[0];
		if (type == Constraint_t::native_constraint) {
			newCon = Clause::newClause(s, lits);
			s.add(newCon);
		}
		else {
			assert(sw != 0);
			Clause* c = Clause::newLearntClause(s, lits, type, sw);
			s.addLearnt(c);
			newCon = c;
		}
		if (out) *out = newCon;
		return !asserting || s.force(first, newCon);
	}
}

Literal& ClauseCreator::operator[](LitVec::size_type i) {
	assert( i < literals_.size() );
	return literals_[i];
}
LitVec::size_type ClauseCreator::size() const {
	return literals_.size();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Clause
/////////////////////////////////////////////////////////////////////////////////////////
Constraint* Clause::newClause(Solver& s, const LitVec& lits) {
	LitVec::size_type nl = 1 + lits.size(); // add ending sentinels
	void* mem = ::operator new( sizeof(Clause) + (nl*sizeof(Literal)) );
	return new (mem) Clause(s, lits, 0, Constraint_t::native_constraint, 0);
}

Clause* Clause::newLearntClause(Solver& s, const LitVec& lits, ConstraintType t, LitVec::size_type secondWatch) {
	assert(t != Constraint_t::native_constraint);
	LitVec::size_type nl = 1 + lits.size(); // add ending sentinels
	void* mem = ::operator new( sizeof(Clause) + (nl * sizeof(Literal)) );
	return new (mem) Clause(s, lits, secondWatch, t, 0);
}

Clause* Clause::newContractedClause(Solver& s, const LitVec& lits, LitVec::size_type sw, LitVec::size_type tailStart) {
	LitVec::size_type nl = 1 + lits.size(); // add ending sentinels
	void* mem = ::operator new( sizeof(Clause) + (nl * sizeof(Literal)) );
	return new (mem) Clause(s, lits, sw, Constraint_t::learnt_conflict, (uint32)tailStart);
}

Clause::Clause(Solver& s, const LitVec& theLits, LitVec::size_type sw, ConstraintType t, uint32 tail) {
	assert(int32(t) < 4);
	size_     = (uint32)theLits.size();
	type_     = t;
	last_     = (2<<1)+1;
	activity_ = (uint32)s.stats.restarts + 1;
	std::memcpy(begin(), &theLits[0], sizeof(Literal) * theLits.size());
	end()->asUint() = 1; // Ending Sentinel    - var 0 + watch-flag on	
	if (t == Constraint_t::native_constraint) {
		initWatches(s);
	}
	else {
		if (tail > 0) {
			assert(sw < tail);
			Literal* newEnd = begin()+tail;
			if (newEnd != end()) {
				*newEnd = ~*newEnd;
				newEnd->watch();
				size_ = newEnd - begin();
			}
		}
		else if (s.isFalse(theLits[sw]) && size_ >= s.strategies().compress()) {
			contract(s);
			sw = 1;
		}
		initWatches(s, 0, uint32(sw));
	}
}


void Clause::destroy() {
	void* mem = static_cast<Constraint*>(this);
	this->~Clause();
	::operator delete(mem);
}

void Clause::initWatches(Solver& s, uint32 fw, uint32 sw) {
	std::swap(lits_[0], lits_[fw]);
	std::swap(lits_[1], lits_[sw]);
	s.addWatch(~lits_[0], this, lits_[1]);
	s.addWatch(~lits_[1], this, lits_[0]);	
}

void Clause::initWatches(Solver& s) {
	if (s.strategies().randomWatches) {
		uint32 fw = irand(size_);
		uint32 sw;
		while ( (sw = irand(size_)) == fw) {/*intentionally empty*/}
		initWatches(s, fw, sw);
	}
	else {
		uint32 watch[2] = {0, (uint32)(size_-1)};
		uint32 count[2] = {s.numWatches(~(*this)[0]), s.numWatches(~(*this)[size_-1])};
		uint32 maxCount = count[0] < count[1];
		for (uint32 x = 1, end = size_-1; count[maxCount] > 0u && x != end; ++x) {
			uint32 cxw = s.numWatches(~(*this)[x]);
			if (cxw < count[maxCount]) {
				if (cxw < count[1-maxCount]) {
					watch[maxCount]   = watch[1-maxCount];
					count[maxCount]   = count[1-maxCount];
					watch[1-maxCount] = x;
					count[1-maxCount] = cxw;
				}
				else {
					watch[maxCount]   = x;
					count[maxCount]   = cxw;
				}
			}
		}
		initWatches(s, watch[0], watch[1]);
	}
}

void Clause::undoLevel(Solver& s) {
	Literal* r = end();
	*r = ~*r;           // restore original literal, implicitly resets the watch-flag!
	for (++r; !isSentinel(*r) && s.value(r->var()) == value_free; ++r);
	if (!isSentinel(*r)) {
		assert(s.level(r->var()) != 0 && "Contracted clause may not contain literals from level 0");
		*r = ~*r;         // Note: ~r is true!
		r->watch();       // create a new artificial ending sentinel
		s.addUndoWatch(s.level(r->var()), this);
	}
	size_ = r - begin();
}

// Note: asserted lit is always set to lits_[0] to make O(1) implementation of locked() 
// possible and to speed up isSatisfied.
Constraint::PropResult Clause::propagate(const Literal& p, uint32& data, Solver& s) {
	Literal falseLit = ~p;
	// make sure that we update lits_[1]
	if (lits_[0] == falseLit) {
		lits_[0] = lits_[1];
		lits_[1] = falseLit;
	}
	// check if clause is already satisfied 
	if (s.isTrue(lits_[0])) {
		data = lits_[0].asUint(); // store lits_[0] as block lit
		return PropResult(true, true);
	}
	lits_[1].asUint() = 1;      // terminate array with sentinel
	uint32 i = last_>>1;        // start where we left off
	int    d = ((last_&1)<<1)-1;// either +1 or -1
	assert(i > 1 && i <= size_);
	int ends = 0;               // number of array bounds seen - 2 means clause is active
	for (;;) {
		// search non-false literal;
		// sentinels guarantee termination
		while (s.isFalse(lits_[i])) {
			i += d;
		}
		if (!lits_[i].watched()) {// found a new watchable literal
			lits_[1] = lits_[i];    // swap old watch
			lits_[i] = falseLit;    // with new one
			last_    = (i<<1)+(d>0);// and remember where we stopped
			s.addWatch(~lits_[1], this, lits_[0]);
			return PropResult(true, false);
		}
		else if (++ends == 1) {   // halfway through, continue search but this
			d    *= -1;             // time, walk in the other
			i     = (last_>>1) + d; // direction
			assert(i >= 1 && i <= size_);
		}
		else {                    // both ends seen: unit, false, or sat
			lits_[1] = falseLit;    // restore watched lit
			return Constraint::PropResult(s.force(lits_[0], this), true);
		}
	}
}

void Clause::reason(const Literal& p, LitVec& rLits) {
	assert(p == lits_[0]); (void)p;
	++activity_;
	const Literal* e = end();
	for (Literal* r = begin()+1; r != e; ++r) {
		rLits.push_back(~*r);
	}
	if (!isSentinel(*e)) {    // clause is contracted - add remaining literals to reason
		rLits.push_back( *e );  // this one was already inverted in contract
		for (++e; !isSentinel(*e); ++e) {
			rLits.push_back(~*e);
		}
	}
}

bool Clause::simplify(Solver& s, bool reinit) {
	assert(s.decisionLevel() == 0);
	if (s.isTrue(lits_[0]) || s.isTrue(lits_[1])) {
		// clause is SAT
		Clause::removeWatches(s);
		return true;
	}
	uint32 j = 2;
	for (uint32 i = 2; !isSentinel(lits_[i]); ++i) {
		if (s.isTrue(lits_[i])) {
			Clause::removeWatches(s);
			return true;
		}
		else if (!s.isFalse(lits_[i])) {
			lits_[j++] = lits_[i];
		}
	}
	size_ = j;
	end()->asUint() = 1;
	last_ = (2<<1)+1;
	if (size_ < 4 || reinit) {
		Clause::removeWatches(s);
		if (size_==2) { return s.addBinary(*begin(), *(begin()+1), false); }
		if (size_==3) { return s.addTernary(*begin(), *(begin()+1), *(begin()+2),false); }
		std::random_shuffle(begin(), end(), irand);
		initWatches(s);
	}
	return false;
}

bool Clause::locked(const Solver& s) const {
	return s.isTrue(lits_[0]) && s.reason(lits_[0]) == this;
}

void Clause::removeWatches(Solver& s) {
	s.removeWatch(~lits_[0], this);
	s.removeWatch(~lits_[1], this);
	if (!isSentinel(*end())) {
		s.removeUndoWatch(s.level( end()->var() ), this );
		end()->asUint() = 1;
	}
}

bool Clause::isSatisfied(const Solver& s, LitVec& freeLits) const {
	uint32 li = last_>>1;
	if (li != size_ && s.isTrue(lits_[li])) {
		return true;
	}
	for (uint32 i = 0; i != size_; ++i) {
		if (s.isTrue(lits_[i])) {
			if (i > 1) {
				//  cache position of true lit
				last_ = (i<<1)+1;
			}
			return true;
		}
		else if (!s.isFalse(lits_[i])) {
			freeLits.push_back(lits_[i]);
		}
	}
	return false;
}

void Clause::contract(Solver& s) {
	assert(s.decisionLevel() > 0);
	// Pre: *begin() is the asserted literal, literals in [begin()+1, end()) are false
	// step 1: sort by decreasing decision level
	std::stable_sort(begin()+1, end(), Detail::GreaterLevel(s));

	// step 2: ignore level 0 literals - shouldn't be there in the first place
	Literal* newEnd = end();
	for (; s.level( (newEnd-1)->var() ) == 0; --newEnd);
	newEnd->asUint() = 1; // terminating sentinel
	size_ = newEnd - begin();

	// step 3: Determine the "active" part of the array.
	// The "active" part will contain only literals from the highest decision level.
	// Literals assigned earlier are temporarily removed from the clause.
	Literal xDL = s.decisionLevel() > 1 
		? s.decision(s.decisionLevel()-1)
		: Literal();
	newEnd      = std::lower_bound(begin()+2, end(), xDL, Detail::GreaterLevel(s));
	if (newEnd != end()) {
		// contract the clause by creating an artificial ending sentinel, i.e.
		// an out-of-bounds literal that is true and watched.
		*newEnd = ~*newEnd;
		newEnd->watch();
		s.addUndoWatch(s.level(newEnd->var()), this); 
		size_ = newEnd - begin();
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
// LoopFormula
/////////////////////////////////////////////////////////////////////////////////////////
LoopFormula::LoopFormula(Solver& s, uint32 size, Literal* bodyLits, uint32 numBodies, uint32 bodyToWatch) {
	activity_       = (uint32)s.stats.restarts + (size-numBodies);  
	end_            = numBodies + 2;
	size_           = end_+1;
	other_          = end_-1;
	lits_[0]        = Literal();  // Starting sentinel
	lits_[end_-1]   = Literal();  // Position of active atom
	lits_[end_]     = Literal();  // Ending sentinel - active part
	for (uint32 i = size_; i != size+3; ++i) {
		lits_[i] = Literal();
	}

	// copy bodies: S B1...Bn, watch one
	std::memcpy(lits_+1, bodyLits, numBodies * sizeof(Literal));
	s.addWatch(~lits_[1+bodyToWatch], this, ((1+bodyToWatch)<<1)+1);
	lits_[1+bodyToWatch].watch();
}

void LoopFormula::destroy() {
	void* mem = static_cast<Constraint*>(this);
	this->~LoopFormula();
	::operator delete(mem);
}


void LoopFormula::addAtom(Literal atom, Solver& s) {
	uint32 pos = size_++;
	assert(isSentinel(lits_[pos]));
	lits_[pos] = atom;
	lits_[pos].watch();
	s.addWatch( ~lits_[pos], this, (pos<<1)+0 );
	if (isSentinel(lits_[end_-1])) {
		lits_[end_-1] = lits_[pos];
	}
}

void LoopFormula::updateHeuristic(Solver& s) {
	Literal saved = lits_[end_-1];
	for (uint32 x = end_+1; x != size_; ++x) {
		lits_[end_-1] = lits_[x];
		s.strategies().heuristic->newConstraint(s, lits_+1, end_-1, Constraint_t::learnt_loop);
	}
	lits_[end_-1] = saved;
}

bool LoopFormula::watchable(const Solver& s, uint32 idx) {
	assert(!lits_[idx].watched());
	if (idx == end_-1) {
		for (uint32 x = end_+1; x != size_; ++x) {
			if (s.isFalse(lits_[x])) {
				lits_[idx] = lits_[x];
				return false;
			}
		}
	}
	return true;
}

bool LoopFormula::isTrue(const Solver& s, uint32 idx) {
	if (idx != end_-1) return s.isTrue(lits_[idx]);
	for (uint32 x = end_+1; x != size_; ++x) {
		if (!s.isTrue(lits_[x])) {
			lits_[end_-1] = lits_[x];
			return false;
		}
	}
	return true;
}

Constraint::PropResult LoopFormula::propagate(const Literal&, uint32& data, Solver& s) {
	if (isTrue(s, other_)) {          // ignore clause, as it is 
		return PropResult(true, true);  // already satisfied
	}
	uint32  pos   = data >> 1;
	uint32  idx   = pos;
	if (pos > end_) {
		// p is one of the atoms - move to active part
		lits_[end_-1] = lits_[pos];
		idx           = end_-1;
	}
	int     dir   = ((data & 1) << 1) - 1;
	int     bounds= 0;
	for (;;) {
		for (idx+=dir;s.isFalse(lits_[idx]);idx+=dir);  // search non-false literal - sentinels guarantee termination
		if (isSentinel(lits_[idx])) {             // Hit a bound,
			if (++bounds == 2) {                    // both ends seen, clause is unit, false, or sat
				if (other_ == end_-1) {
					uint32 x = end_+1;
					for (; x != size_ && s.force(lits_[x], this);  ++x);
					return Constraint::PropResult(x == size_, true);  
				}
				else {
					return Constraint::PropResult(s.force(lits_[other_], this), true);  
				}
			}
			idx   = std::min(pos, end_-1);          // halfway through, restart search, but
			dir   *= -1;                            // this time walk in the opposite direction.
			data  ^= 1;                             // Save new direction of watch
		}
		else if (!lits_[idx].watched() && watchable(s, idx)) { // found a new watchable literal
			if (pos > end_) {     // stop watching atoms
				lits_[end_-1].clearWatch();
				for (uint32 x = end_+1; x != size_; ++x) {
					if (x != pos) {
						s.removeWatch(~lits_[x], this);
						lits_[x].clearWatch();
					}
				}
			}
			lits_[pos].clearWatch();
			lits_[idx].watch();
			if (idx == end_-1) {  // start watching atoms
				for (uint32 x = end_+1; x != size_; ++x) {
					s.addWatch(~lits_[x], this, static_cast<uint32>(x << 1) + 0);
					lits_[x].watch();
				}
			}
			else {
				s.addWatch(~lits_[idx], this, static_cast<uint32>(idx << 1) + (dir==1));
			}
			return Constraint::PropResult(true, false);
		} 
		else if (lits_[idx].watched()) {          // Hit the other watched literal
			other_  = idx;                          // Store it in other_
		}
	}
}

// Body: all other bodies + active atom
// Atom: all bodies
void LoopFormula::reason(const Literal& p, LitVec& lits) {
	// all relevant bodies
	for (uint32 x = 1; x != end_-1; ++x) {
		if (lits_[x] != p) {
			lits.push_back(~lits_[x]);
		}
	}
	// if p is a body, add active atom
	if (other_ != end_-1) {
		lits.push_back(~lits_[end_-1]);
	}
	++activity_;
}

LitVec::size_type LoopFormula::size() const {
	return size_ - 3;
}

bool LoopFormula::locked(const Solver& s) const {
	if (other_ != end_-1) {
		return s.isTrue(lits_[other_]) && s.reason(lits_[other_]) == this;
	}
	for (uint32 x = end_+1; x != size_; ++x) {
		if (s.isTrue(lits_[x]) && s.reason(lits_[x]) == this) {
			return true;
		}
	}
	return false;
}

void LoopFormula::removeWatches(Solver& s) {
	for (uint32 x = 1; x != end_-1; ++x) {
		if (lits_[x].watched()) {
			s.removeWatch(~lits_[x], this);
			lits_[x].clearWatch();
		}
	}
	if (lits_[end_-1].watched()) {
		lits_[end_-1].clearWatch();
		for (uint32 x = end_+1; x != size_; ++x) {
			s.removeWatch(~lits_[x], this);
			lits_[x].clearWatch();
		}
	}
}

bool LoopFormula::isSatisfied(const Solver& s, LitVec& freeLits) const {
	if (other_ != end_-1 && s.isTrue(lits_[other_])) return true;
	for (uint32 x = 1; x != end_-1; ++x) {
		if (s.isTrue(lits_[x])) {
			other_ = x;
			return true;
		}
		else if (!s.isFalse(lits_[x])) { freeLits.push_back(lits_[x]); }
	}
	bool sat = true;
	for (uint32 x = end_+1; x != size_; ++x) {
		if (s.value(lits_[x].var()) == value_free) {
			freeLits.push_back(lits_[x]);
			sat = false;
		}
		else sat &= s.isTrue(lits_[x]);
	}
	return sat;
}

bool LoopFormula::simplify(Solver& s, bool) {
	assert(s.decisionLevel() == 0);
	typedef std::pair<uint32, uint32> WatchPos;
	bool      sat = false;          // is the constraint SAT?
	WatchPos  bodyWatches[2];       // old/new position of watched bodies
	uint32    bw  = 0;              // how many bodies are watched?
	uint32    j   = 1, i;
	// 1. simplify the set of bodies:
	// - search for a body that is true -> constraint is SAT
	// - remove all false bodies
	// - find the watched bodies
	for (i = 1; i != end_-1; ++i) {
		assert( !s.isFalse(lits_[i]) || !lits_[i].watched() ); // otherwise should be asserting 
		if (!s.isFalse(lits_[i])) {
			sat |= s.isTrue(lits_[i]);
			if (i != j) { lits_[j] = lits_[i]; }
			if (lits_[j].watched()) { bodyWatches[bw++] = WatchPos(i, j); }
			++j;
		}
	}
	uint32  newEnd    = j + 1;
	uint32  numBodies = j - 1;
	j += 2;
	// 2. simplify the set of atoms:
	// - remove all determined atoms
	// - remove/update watches if necessary
	for (i = end_ + 1; i != size_; ++i) {
		if (s.value(lits_[i].var()) == value_free) {
			if (i != j) { lits_[j] = lits_[i]; }
			if (lits_[j].watched()) {
				if (sat || numBodies <= 2) {
					s.removeWatch(~lits_[j], this);
					lits_[j].clearWatch();
				}
				else if (i != j) {
					Watch* w  = s.getWatch(~lits_[j], this);
					assert(w);
					w->data() = (j << 1) + 0;
				}
			}
			++j;
		}
		else if (lits_[i].watched()) {
			s.removeWatch(~lits_[i], this);
			lits_[i].clearWatch();
		}
	}
	size_         = j;
	end_          = newEnd;
	lits_[end_]   = Literal();
	lits_[end_-1] = lits_[end_+1];
	if (sat || numBodies < 3 || size_ == end_ + 1) {
		for (i = 0; i != bw; ++i) {
			s.removeWatch(~lits_[bodyWatches[i].second], this);
			lits_[bodyWatches[i].second].clearWatch();
		}
		if (sat || size_ == end_+1) { return true; }
		// replace constraint with short clauses
		ClauseCreator creator(&s);
		creator.start();
		for (i = 1; i != end_; ++i) { creator.add(lits_[i]); }
		for (i = end_+1; i != size_; ++i) {
			creator[creator.size()-1] = lits_[i];
			creator.end();
		}
		return true;
	}
	other_ = 1;
	for (i = 0; i != bw; ++i) {
		if (bodyWatches[i].first != bodyWatches[i].second) {
			Watch* w  = s.getWatch(~lits_[bodyWatches[i].second], this);
			assert(w);
			w->data() = (bodyWatches[i].second << 1) + (w->data()&1);
		}
	}
	return false;
}
}
