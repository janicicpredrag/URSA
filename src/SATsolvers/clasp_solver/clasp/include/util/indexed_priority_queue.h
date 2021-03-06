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
#ifndef BK_LIB_INDEXED_PRIORITY_QUEUE_H_INCLUDED
#define BK_LIB_INDEXED_PRIORITY_QUEUE_H_INCLUDED

#ifdef _MSC_VER
#pragma warning (disable : 4267)
#pragma once
#endif

#include <cstddef>
#include "pod_vector.h"
namespace bk_lib { namespace detail {

typedef std::size_t key_type;
const key_type noKey = static_cast<key_type>(-1);
inline key_type heap_root() { return 0; }
inline key_type heap_left(std::size_t i) { return (i<<1)+1; }
inline key_type heap_right(std::size_t i) { return (i+1)<<1; }
inline key_type heap_parent(std::size_t i) { return (i-1)>>1; }

}

// Note: Uses a Max-Heap!
template <
	class Cmp // sort-predicate - if Cmp(k1, k2) == true, n1 has higher priority than n2
>
class indexed_priority_queue {
public:
	typedef detail::key_type key_type;
	typedef pod_vector<key_type> index_container_type;
	typedef std::size_t     size_type;
	typedef Cmp             compare_type;
	
	explicit indexed_priority_queue( const compare_type& c = compare_type() );
	indexed_priority_queue(const indexed_priority_queue& other);
	
	indexed_priority_queue& operator=(const indexed_priority_queue& other) {
		indices_  = other.indices_;
		heap_     = other.heap_;
		compare_  = other.compare_;
	}

	const compare_type& key_compare() const {
		return compare_;
	}
	bool empty() const {
		return heap_.empty();
	}

	void push(key_type k) {
		assert( !is_in_queue(k) );
		if ((key_type)indices_.size() <= k) {
			indices_.resize(k+1, detail::noKey);
		}
		indices_[k] = (key_type)heap_.size();
		heap_.push_back(k);
		siftup(indices_[k]);
	}

	void pop() {
		assert(!empty());
		key_type x  = heap_[0];
		heap_[0]    = heap_.back();
		indices_[heap_[0]] = 0;
		indices_[x]   = detail::noKey;
		heap_.pop_back();
		if (heap_.size() > 1) {siftdown(0);}
	}

	void clear() {
		heap_.clear();
		indices_.clear();
	}

	size_type size( ) const {
		return heap_.size();
	}

	key_type top() const {
		assert(!empty());
		return heap_[0];
	}
	
	void update(key_type k) {
		if (!is_in_queue(k)) {
			push(k);
		}
		else {
			siftup(indices_[k]);
			siftdown(indices_[k]);
		}
	}
	// call if priority of k has increased
	void increase(key_type k) {
		assert(is_in_queue(k));
		siftup(indices_[k]);
	}
	// call if priority of k has decreased
	void decrease(key_type k) {
		assert(is_in_queue(k));
		siftdown(indices_[k]);
	}

	bool is_in_queue(key_type k) const {
		assert(valid_key(k));
		return k < (key_type)indices_.size() && indices_[k] != detail::noKey;
	}
	
	void remove(key_type k) {
		if (is_in_queue(k)) {
			key_type kInHeap  = indices_[k];
			heap_[kInHeap]    = heap_.back();
			indices_[heap_.back()]  = kInHeap;
			heap_.pop_back();
			indices_[k] = detail::noKey;
			if (heap_.size() > 1 && kInHeap != (key_type)heap_.size()) {
				siftup(kInHeap);
				siftdown(kInHeap);
			}
		}
	}
private:
	bool valid_key(key_type k) const {
		return k != detail::noKey;
	}
	index_container_type  indices_;
	index_container_type  heap_;
	compare_type          compare_;
	void siftup(key_type n) {
		using namespace detail;
		key_type x = heap_[n];
		key_type p = heap_parent(n);
		while (n != 0 && compare_(x, heap_[p])){
			heap_[n] = heap_[p];
			indices_[heap_[n]] = n;
			n = p;
			p = heap_parent(n);
		}
		heap_[n] = x;
		indices_[x] = n;
	}

	void siftdown(key_type n) {
		using namespace detail;
		key_type x = heap_[n];
		while (heap_left(n) < (key_type)heap_.size()){
			key_type child = smaller_child(n);
			if (!compare_(heap_[child], x)) {
				break;
			}
			heap_[n] = heap_[child];
			indices_[heap_[n]] = n;
			n = child;
		}
		heap_[n] = x;
		indices_[x] = n;
	}

	key_type smaller_child(size_type n) const {
		using namespace detail;
		return heap_right(n) < (key_type)heap_.size() && compare_(heap_[heap_right(n)], heap_[heap_left(n)])
			? heap_right(n)
			: heap_left(n);
	}
};

template <class C>
indexed_priority_queue<C>::indexed_priority_queue( const compare_type& c ) 
	: indices_()
	, heap_()
	, compare_(c) {
}

template <class C>
indexed_priority_queue<C>::indexed_priority_queue(const indexed_priority_queue& other)
	: indices_(other.indices_) 
	, heap_(other.heap_)
	, compare_(other.compare_) {
}

}
#endif
