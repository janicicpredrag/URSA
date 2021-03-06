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

#ifndef CLASP_UTIL_MISC_TYPES_H_INCLUDED
#define CLASP_UTIL_MISC_TYPES_H_INCLUDED

#ifdef _MSC_VER
#pragma warning (disable : 4311 4312)
#pragma once
#endif

#include <clasp/include/util/platform.h>
#include <utility>    // std::pair
#include <functional> // std::unary_function, std::binary_function
#include <algorithm>
#include <cassert>
#include <stdexcept>
/*!
 * \file 
 * Some utility types and functions not specific to clasp.
 */
namespace Clasp {
/*!
 * \defgroup misc Miscellaneous and Internal Stuff
 */
//@{

inline unsigned hashId(unsigned key) {  
	key = ~key + (key << 15);
	key ^= (key >> 11);
	key += (key << 3);
	key ^= (key >> 5);
	key += (key << 10);
	key ^= (key >> 16);
	return key;
}

// Computes n choose k.
inline uint64 choose(unsigned n, unsigned k) {
	if (k == 0) return 1;
	if (k > n) return 0;
	if (2 * k > n) { return choose(n, n-k);}
	uint64 res = n;
	for (unsigned i = 2 ; i <= k; ++i) {
		res *= (n + 1 - i);
		res /= i;
	}
	return res;
}


extern uint32 randSeed_g; /**< stores the seed of the RNG */

//! Sets the starting point for random-number generation.
/*!
 * The function sets the starting point for generating a series of pseudorandom integers. 
 * To reinitialize the generator, use 1 as the seed argument. Any other value for seed 
 * sets the generator to a random starting point. Calling rand() before any call to srand()
 * generates the same sequence as calling srand() with seed passed as 1.
 */
inline void srand(uint32 seed) { randSeed_g = seed; }

//! Generates a pseudorandom number
/*!
 * The rand function returns a pseudorandom integer in the range 0 to 32767 
 * Use the srand function to seed the pseudorandom-number generator before calling rand.
 *
 * \note This function is a replacement for the standard rand-function. It is provided
 * in order to get reproducible random numbers among different compilers.
 *
 * \attention The rand() function is not thread-safe. Calls to rand() from different
 * threads must be serialized.
 */
inline uint32 rand() {
	return( ((randSeed_g = randSeed_g * 214013L + 2531011L) >> 16) & 0x7fff );
}

//! random floating point number in the range [0, 1.0)
/*!
 * \attention Function is not thread-safe
 */
inline double drand() {
	return Clasp::rand()/static_cast<double>(0x8000u);
}

//! random number in the range [0, max)
/*!
 * \attention Function is not thread-safe
 */
inline unsigned irand(unsigned max) {
	return static_cast<unsigned>(drand() * max);
}

//! An unary operator function that calls p->destroy()
struct DestroyObject {
	template <class T>
	void operator()(T* p) const {
		p->destroy();
	}
};

//! An unary operator function that calls delete p
struct DeleteObject {
	template <class T>
	void operator()(T* p) const {
		delete p;
	}
};


//! Removes from the container c the first occurrence of a value v for which p(v) returns true
/*!
 * \pre C is a container that provides back() and pop_back()
 * \note Removal is implemented by replacing the element to be removed with 
 * the back()-element followed by a call to pop_back().
 */
template <class C, class P>
void remove_first_if(C& cont, const P& p) {
	for (typename C::iterator it = cont.begin(), end = cont.end(); it != end; ++it) {
		if (p(*it)) {
			*it = cont.back();
			cont.pop_back();
			return;
		}
	}
}

//! An unary operator function that simply returns its argument
template <class T>
struct identity : std::unary_function<T, T>{
	T&        operator()(T& x)      const { return x; }
	const T&  operator()(const T& x)  const { return x; }
};


//! An unary operator function that returns the first value of a std::pair
template <class P>
struct select1st : std::unary_function<P, typename P::first_type> {
	typename P::first_type& operator()(P& x) const {
		return x.first;
	}
	const typename P::first_type& operator()(const P& x) const {
		return x.first;
	}
};

//! An unary operator function that returns the second value of a std::pair
template <class P>
struct select2nd : std::unary_function<P, typename P::second_type> {
	typename P::second_type& operator()(P& x) const {
		return x.second;
	}
	const typename P::second_type& operator()(const P& x) const {
		return x.second;
	}
};

//! An unary operator function that returns Op1(Op2(x))
template <class OP1, class OP2>
struct compose_1 : public std::unary_function<
														typename OP2::argument_type, 
														typename OP1::result_type> {
	compose_1(const OP1& op1, const OP2& op2)
		: op1_(op1)
		, op2_(op2) {}
	
	typename OP1::result_type operator()(const typename OP2::argument_type& x) const {
		return op1_(op2_(x));
	}
protected:
	OP1 op1_;
	OP2 op2_;
};

/*!
 * A template helper function used to construct objects of type compose_1, 
 * where the component types are based on the data types passed as parameters.
 */
template <class OP1, class OP2>
inline compose_1<OP1, OP2> compose1(const OP1& op1, const OP2& op2) {
	return compose_1<OP1, OP2>(op1, op2);
}

//! An unary operator function that returns OP1(OP2(x), OP3(x))
template <class OP1, class OP2, class OP3>
struct compose_2_1 : public std::unary_function<
														typename OP2::argument_type, 
														typename OP1::result_type> {
	compose_2_1(const OP1& op1, const OP2& op2, const OP3& op3)
		: op1_(op1)
		, op2_(op2)
		, op3_(op3) {}
	
	typename OP1::result_type operator()(const typename OP2::argument_type& x) const {
		return op1_(op2_(x), op3_(x));
	}
protected:
	OP1 op1_;
	OP2 op2_;
	OP3 op3_;
};

/*!
 * A template helper function used to construct objects of type compose_2_1, 
 * where the component types are based on the data types passed as parameters.
 */
template <class OP1, class OP2, class OP3>
inline compose_2_1<OP1, OP2,OP3> compose2(const OP1& op1, const OP2& op2, const OP3& op3) {
	return compose_2_1<OP1, OP2, OP3>(op1, op2, op3);
}


//! A binary operator function that returns OP1(OP2(x), OP3(y))
template <class OP1, class OP2, class OP3>
struct compose_2_2 : public std::binary_function<
														typename OP2::argument_type, 
														typename OP3::argument_type,
														typename OP1::result_type> {
	compose_2_2(const OP1& op1, const OP2& op2, const OP3& op3)
		: op1_(op1)
		, op2_(op2)
		, op3_(op3) {}
	
	typename OP1::result_type operator()(const typename OP2::argument_type& x, const typename OP3::argument_type& y) const {
		return op1_(op2_(x), op3_(y));
	}
protected:
	OP1 op1_;
	OP2 op2_;
	OP3 op3_;
};

/*!
 * A template helper function used to construct objects of type compose_2_2, 
 * where the component types are based on the data types passed as parameters.
 */
template <class OP1, class OP2, class OP3>
inline compose_2_2<OP1, OP2,OP3> compose22(const OP1& op1, const OP2& op2, const OP3& op3) {
	return compose_2_2<OP1, OP2, OP3>(op1, op2, op3);
}

template <class T>
class SingleOwnerPtr {
public:
	explicit SingleOwnerPtr(T* ptr) : ptr_( ((uintp)ptr) | 1 ) {}
	~SingleOwnerPtr() {
		if ((ptr_ & 1) != 0) { 
			delete this->operator->(); 
		}
	}
	SingleOwnerPtr& operator=(T* ptr) {
		SingleOwnerPtr t(ptr);
		t.swap(*this);
		return *this;
	}
	T& operator*()  const { return *this->operator->(); }
	T* operator->() const {
		return (T*)(ptr_&~1u);
	}
	T*    release()   { ptr_ &= ~1u; return get();  }
	T*    get() const { return this->operator->();  }

	void swap(SingleOwnerPtr& o) {
		std::swap(ptr_, o.ptr_);
	}
private:
	SingleOwnerPtr(const SingleOwnerPtr&);
	SingleOwnerPtr& operator=(const SingleOwnerPtr&);
	uintp ptr_;
};


inline uint64 highDword(uint64 x) {
	const uint64 HIGH_MASK_U64 = ~uint64(0xFFFFFFFFu);
	return x & HIGH_MASK_U64;
}

template <class P, int ps = sizeof(P)>
struct PtrMap;

template <class P>
struct PtrMap<P*, 4> {
	static uint32 encode(P* p) {
		return reinterpret_cast<uint32>(p);
	}
	static P* decode(uint32 p) {
		return reinterpret_cast<P*>(p);
	}
};

template <class P>
struct PtrMap<P*, 8> {
	static uint32 encode(P* p) {
		uintp x = reinterpret_cast<uintp>(p);
		assert( (x & 7u) == 0 && "Bad pointer alignment" );
		return static_cast<uint32>(x) | getSegment(highDword(x));
	}
	static P* decode(uint32 p) {
		uint32 k = (p & 7u);
		uint64 x = segment_s[ k ] + (p&~7u);
		return reinterpret_cast<P*>(x);
	}
	static uint32 getSegment(uint64 high) {
		uint32 i;
		switch(0) {
			case 0 : if (segment_s[0] == 1 || segment_s[0] == high) { i = 0; break; }
			case 1 : if (segment_s[1] == 1 || segment_s[1] == high) { i = 1; break; }
			case 2 : if (segment_s[2] == 1 || segment_s[2] == high) { i = 2; break; }
			case 3 : if (segment_s[3] == 1 || segment_s[3] == high) { i = 3; break; }
			case 4 : if (segment_s[4] == 1 || segment_s[4] == high) { i = 4; break; }
			case 5 : if (segment_s[5] == 1 || segment_s[5] == high) { i = 5; break; }
			case 6 : if (segment_s[6] == 1 || segment_s[6] == high) { i = 6; break; }
			case 7 : if (segment_s[7] == 1 || segment_s[7] == high) { i = 7; break; }
			default: throw std::bad_alloc();
		}
		if (segment_s[i] != high) { segment_s[i] = high; }
		return i;
	}
	static uint64 segment_s[8];
};
template <class P>
uint64 PtrMap<P*, 8>::segment_s[8] = {1, 1, 1, 1, 1, 1, 1, 1};

template <class T>
class Ptr32 {
public:
	typedef T* value_type;
	typedef PtrMap<T*> PtrMapper;
	Ptr32(value_type p) : ptr_(PtrMapper::encode(p)) {}
	static Ptr32 fromRep(uint32 p) {
		return Ptr32(p,false);
	}
	T* get()       const throw() { 
		return static_cast<T*>(PtrMapper::decode(ptr_));
	}
	T& operator*() const throw() {
		return *get();
	}
	T* operator->() const throw() {
		return get();
	}
	uint32 encode() const { return ptr_; }
private:
	explicit Ptr32(uint32 p, bool) : ptr_(p) {}
	uint32 ptr_;
};

//@}
}

#endif
