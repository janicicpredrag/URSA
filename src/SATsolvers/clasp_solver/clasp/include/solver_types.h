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
#ifndef CLASP_SOLVER_TYPES_H_INCLUDED
#define CLASP_SOLVER_TYPES_H_INCLUDED
#ifdef _MSC_VER
#pragma once
#endif

#include <clasp/include/literal.h>
#include <clasp/include/constraint.h>
#include <clasp/include/util/misc_types.h>

#define MAINTAIN_JUMP_STATS 0           // alternative: 1

/*!
 * \file 
 * Contains some types used by a Solver
 */
namespace Clasp {

/*!
 * \addtogroup solver
 */
//@{

//! Possible types of a variable.
struct Var_t {
	enum Value { atom_var = 1, body_var = 2, atom_body_var = atom_var | body_var};
};
typedef Var_t::Value VarType;
inline bool isBody(VarType t) { return (static_cast<uint32>(t) & static_cast<uint32>(Var_t::body_var)) != 0; }
inline bool isAtom(VarType t) { return (static_cast<uint32>(t) & static_cast<uint32>(Var_t::atom_var)) != 0; }

///////////////////////////////////////////////////////////////////////////////
// Count a thing or two...
///////////////////////////////////////////////////////////////////////////////
//! A struct for aggregating some statistics
struct SolverStatistics {
	SolverStatistics() { std::memset(this, 0, sizeof(*this)); }
	uint64 models;    /**< Number of models found */
	uint64 conflicts; /**< Number of conflicts found */
	uint64 loops;     /**< Number of learnt loop-formulas */
	uint64 choices;   /**< Number of choices performed */
	uint64 restarts;  /**< Number of restarts */  
	
	uint64 lits[2];   /**< 0: conflict, 1: loop */
	uint32 native[3]; /**< 0: all, 1: binary, 2: ternary */
	uint32 learnt[3]; /**< 0: all, 1: binary, 2: ternary */
	uint32 deleted;   /**< number of constraints removed during nogood deletion */
#if MAINTAIN_JUMP_STATS == 1
	uint64  modLits;  /**< Number of decision literals in models */
	uint64  jumps;    /**< Number of backjumps (i.e. number of analyzed conflicts) */
	uint64  bJumps;   /**< Number of backjumps that were bounded */
	uint64  jumpSum;  /**< Number of levels that could be skipped w.r.t first-uip */
	uint64  boundSum; /**< Number of levels that could not be skipped because of backtrack-level*/
	uint32  maxJump;  /**< Longest possible backjump */
	uint32  maxJumpEx;/**< Longest executed backjump (< maxJump if longest jump was bounded) */
	uint32  maxBound; /**< Max difference between uip- and backtrack-level */
#endif
};

inline void updateLearnt(SolverStatistics& s, LitVec::size_type n, ConstraintType t) {
	assert(t != Constraint_t::native_constraint);
	++s.learnt[0];  
	s.lits[t-1] += n;
	s.loops += t == Constraint_t::learnt_loop;
	if (n > 1 && n < 4) {
		++s.learnt[n-1];
	}
}
#if MAINTAIN_JUMP_STATS == 0
inline void updateModels(SolverStatistics&, uint32) {}
inline void updateJumps(SolverStatistics&, uint32, uint32, uint32) {}
#else
inline void updateModels(SolverStatistics& s, uint32 n) {
	s.modLits += n;
}
inline void updateJumps(SolverStatistics& s, uint32 dl, uint32 uipLevel, uint32 bLevel) {
	++s.jumps;
	s.jumpSum += dl - uipLevel; 
	s.maxJump = std::max(s.maxJump, dl - uipLevel);
	if (uipLevel < bLevel) {
		++s.bJumps;
		s.boundSum += bLevel - uipLevel;
		s.maxJumpEx = std::max(s.maxJumpEx, dl - bLevel);
		s.maxBound  = std::max(s.maxBound, bLevel - uipLevel);
	}
	else {
		s.maxJumpEx = s.maxJump;
	}
}
#endif
///////////////////////////////////////////////////////////////////////////////
class Clause;
///////////////////////////////////////////////////////////////////////////////
// A watch is either a clause reference and a blocking literal or a 
// generic constraint reference and a "data-blob".
// In the first case, data_ is a literal and ref_ is either a literal (ternary clause)
// or a (compressed) pointer to a clause object.
// In the second case, ref_ is a (compressed) pointer to a constraint object
// and data is the "blob". 
// Note: Watch cannot distinguish whether it references a clause 
//       or a generic constraint! 
///////////////////////////////////////////////////////////////////////////////
//! Represents a watch in a Solver.
struct Watch {
	typedef PtrMap<void*> PtrMapper;
	Watch(Clause* a_clause, Literal block) 
		: ref_(PtrMapper::encode(a_clause))  { 
		block.clearWatch();
		data_ = block.asUint();
	}
	Watch(Literal x, Literal y) {
		x.watch();
		data_ = x.asUint();
		ref_  = y.asUint();
	}
	Watch(Constraint* a_con, uint32 data) 
		: data_(data)
		, ref_(PtrMapper::encode(a_con)) {
	}
	uint32&  data() { return data_; }
	
	// only useable if watch is a clause watch
	bool    isTernary()const { return (data_&1) != 0; }
	bool    isClause() const { return (data_&1) == 0; }
	Literal first()    const { return Literal::fromRep(data_); }
	Literal second()   const { assert(isTernary()); return Literal::fromRep(ref_); }
	Clause* clause()   const {
		assert(isClause());
		return static_cast<Clause*>(PtrMapper::decode(ref_));
	}
	bool operator==(const Clause* a_clause) const { 
		return !isTernary() && clause() == a_clause; 
	}
	// only usable if watch is a generic constraint watch
	Constraint* constraint() const {
		return static_cast<Constraint*>(PtrMapper::decode(ref_));
	}
	bool operator==(const Constraint* a_con) const { 
		return constraint() == a_con; 
	}
private:
	uint32 data_;
	uint32 ref_;
};

struct TernaryContains {
	TernaryContains(Literal a_lit) : p(a_lit) {}
	bool operator()(const Watch& w) const {
		return w.isTernary() && (w.first() == p || w.second() == p);
	}
	Literal p;
};

typedef uint8 ValueRep;           /**< Type of the three value-literals */
const ValueRep value_true   = 1;  /**< Value used for variables that are true */
const ValueRep value_false  = 2;  /**< Value used for variables that are false */
const ValueRep value_free   = 0;  /**< Value used for variables that are unassigned */

//! returns the value that makes the literal lit true.
/*!
 * \param lit the literal for which the true-value should be determined.
 * \return
 *   - value_true     iff lit is a positive literal
 *   - value_false    iff lit is a negative literal.
 *   .
 */
inline ValueRep trueValue(const Literal& lit) { return 1 + lit.sign(); }

//! returns the value that makes the literal lit false.
/*!
 * \param lit the literal for which the false-value should be determined.
 * \return
 *   - value_false      iff lit is a positive literal
 *   - value_true       iff lit is a negative literal.
 *   .
 */
inline ValueRep falseValue(const Literal& lit) { return 1 + !lit.sign(); }

//! Stores additional information about variables
class VarInfo {
public:
	VarInfo() {}
	void  reserve(uint32 maxSize) { info_.reserve(maxSize); }
	void  add(bool body) {
		info_.push_back( body ? BODY_FLAG : 0);
	}
	bool      frozen(Var v)     const { return (info_[v] & FROZEN_FLAG) != 0; }
	bool      eliminated(Var v) const { return (info_[v] & ELIM_FLAG) != 0;   }
	uint8     seen(Var v)       const { return info_[v] & SEEN_MASK; }
	ValueRep  prefValue(Var v)  const { return ValueRep((info_[v]>>2)&3u); }
	bool      body(Var v)       const { return (info_[v] & BODY_FLAG) != 0; }
	bool      eq(Var v)         const { return (info_[v] & EQ_FLAG) != 0; }
	void setFrozen(Var v, bool b)     { b ? info_[v] |= FROZEN_FLAG : info_[v] &= ~FROZEN_FLAG; }
	void setEliminated(Var v, bool b) { b ? info_[v] |= ELIM_FLAG   : info_[v] &= ~ELIM_FLAG;   }
	void setBody(Var v, bool b)       { b ? info_[v] |= BODY_FLAG   : info_[v] &= ~BODY_FLAG;   }
	void setEq(Var v, bool b)         { b ? info_[v] |= EQ_FLAG     : info_[v] &= ~EQ_FLAG;     }
	void setSeen(Var v, uint8 x)      { info_[v] |= x; }
	void clearSeen(Var v)             { info_[v] &= ~SEEN_MASK; }
	void setPrefValue(Var v, ValueRep val) {
		info_[v] &= ~PFVAL_MASK;
		info_[v] |= (val<<2);
	}
private:
	// Bit:   7     6   5   4   3   2  1  0
	//      frozen elim eq body pfVal  seen
	typedef PodVector<uint8>::type InfoVec;
	static const uint8 SEEN_MASK  = 3u;
	static const uint8 PFVAL_MASK = 12u;
	static const uint8 BODY_FLAG  = 1u<<4;
	static const uint8 EQ_FLAG    = 1u<<5;
	static const uint8 ELIM_FLAG  = 1u<<6;
	static const uint8 FROZEN_FLAG= 1u<<7;
	VarInfo(const VarInfo&);
	VarInfo& operator=(const VarInfo&);
	InfoVec info_;
};

//! Type used to store lookahead-information for one variable.
struct VarScore {
	VarScore() { clear(); }
	void clear() { reinterpret_cast<uint32&>(*this) = 0; }
	//! Mark literal p as dependent
	void setSeen( Literal p ) {
		seen_ |= uint32(p.sign()) + 1;
	}
	//! Is literal p dependent?
	bool seen(Literal p) const {
		return (seen_ & (uint32(p.sign())+1)) != 0;
	}
	//! Is this var dependent?
	bool seen() const { return seen_ != 0; }
	//! Mark literal p as tested during lookahead.
	void setTested( Literal p ) {
		tested_ |= uint32(p.sign()) + 1;
	}
	//! Was literal p tested during lookahead?
	bool tested(Literal p) const {
		return (tested_ & (uint32(p.sign())+1)) != 0;
	}
	//! Was some literal of this var tested?
	bool tested() const { return tested_ != 0; }
	//! Were both literals of this var tested?
	bool testedBoth() const { return tested_  == 3; }

	//! Sets the score for literal p to value
	void setScore(Literal p, LitVec::size_type value) {
		if (value > (1U<<14)-1) value = (1U<<14)-1;
		if (p.sign()) nVal_ = uint32(value);
		else          pVal_ = uint32(value);
		setTested(p);
	}
	
	//! Returns the score for literal p.
	uint32 score(Literal p) const {
		return p.sign() ? nVal_ : pVal_;
	}
	
	//! Returns the scores of the two literals of a variable.
	/*!
	 * \param[out] mx the maximum score
	 * \param[out] mn the minimum score
	 */
	void score(uint32& mx, uint32& mn) const {
		if (nVal_ > pVal_) {
			mx = nVal_;
			mn = pVal_;
		}
		else {
			mx = pVal_;
			mn = nVal_;
		}
	}
	//! returns the sign of the literal that has the higher score.
	bool prefSign() const {
		return nVal_ > pVal_;
	}
private:
	uint32 pVal_  : 14;
	uint32 nVal_  : 14;
	uint32 seen_  : 2;
	uint32 tested_: 2;
};

typedef PodVector<VarScore>::type VarScores; /**< A vector of variable-scores */

//! Stores information about a literal that is implied on an earlier level than the current decision level.
struct ImpliedLiteral {
	ImpliedLiteral(Literal a_lit, uint32 a_level, const Antecedent& a_ante) 
		: lit(a_lit)
		, level(a_level)
		, ante(a_ante) {
	}
	Literal     lit;    /**< The implied literal */
	uint32      level;  /**< The earliest decision level on which lit is implied */
	Antecedent  ante;   /**< The reason why lit is implied on decision-level level */
};

//@}
}
#endif
