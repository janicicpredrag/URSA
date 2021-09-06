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
#ifndef __HEAP_H__
#define __HEAP_H__

#include <vector>
#include <cstdlib>
#include <iostream>
using std::cout;
using std::endl;

#include "Auxiliary.hpp"

template<class Type, class Comparator>
class Heap {
private:

  Comparator _gt;
  std::vector<Type> _heap;
  std::vector<size_t> _position_in_heap;

  static size_t parrent(size_t i) {
    return (i - 1) >> 1;
  }

  static size_t left_child(size_t i) {
    return (i << 1) | 1;
  }

  static size_t right_child(size_t i) {
    return (i + 1) << 1;
  }

  bool gt(Type x, Type y) {
    return _gt(x, y) || (!_gt(y, x) && x < y);
    //	return _gt(x, y) || (!_gt(y, x) && randBool());
  }

  void moveUp (size_t i) {
    Type x = _heap[i];
    while (i != 0) {
      if (gt(_heap[parrent(i)], x))
	break;

      size_t p = parrent(i);
      _heap[i] = _heap[p];
      _position_in_heap[_heap[i]] = i;
      i = p;
    }
    _heap[i] = x;
    _position_in_heap[_heap[i]] = i;

    assert(checkHeap(0));
  }

  void moveDown(size_t i) {
    Type x = _heap[i];
    size_t size = _heap.size(); 
    size_t left = left_child(i), 
      right = right_child(i);

    while(left < size) {
      size_t child;
      if (right >= size)
	child = left;
      else
	child = gt(_heap[left], _heap[right]) ? left : right;

      if (gt(x, _heap[child]))
	break;
	    
      _heap[i] = _heap[child];
      _position_in_heap[_heap[i]] = i;
      i = child;

      left = left_child(i);
      right = right_child(i);
    }
    _heap[i] = x;
    _position_in_heap[_heap[i]] = i;

    assert(checkHeap(0));
  }

  bool checkHeap(size_t i) {
    if (i >= size())
      return true;

    size_t left = left_child(i), 
      right = right_child(i);
    if (left < size() && _gt(_heap[left], _heap[i])) {
      cout << "Wrong: " << _heap[left] << "  " << _heap[i] << endl;
      return false;
    }
    if (right < size() && _gt(_heap[right], _heap[i])) {
      cout << "Wrong: " << _heap[right] << "  " << _heap[i] << endl;
      return false;
    }
    return checkHeap(left) && checkHeap(right);
	
  }

  static const unsigned UNDEFINED = (unsigned)(-1);

public:
  Heap() {
  }

  Heap(const Comparator& gt)
    : _gt(gt) {
  }
    
  // The heap is notified about x's existence, but
  // it's not inserted into it.
  void push_inactive(Type x) {
    _position_in_heap.push_back(UNDEFINED);
  }
    
  void push_heap(Type x) {
    _heap.push_back(x);
    _position_in_heap.push_back(size() - 1);

    moveUp(size() - 1); 
  }
 
  Type pop_heap() {
    assert(_heap.size() > 0);
    Type x = _heap[0];
    _heap[0] = _heap.back();
    _heap.pop_back();
    _position_in_heap[x] = UNDEFINED;
    if (!empty())
      moveDown(0);
    assert(!contains(x));
    return x;
  }

  void update_heap(Type x) {
    if (!contains(x))
      push_heap(x);
    else {
      moveUp(_position_in_heap[x]);
      moveDown(_position_in_heap[x]);
    }
  }

  void increase(Type x) {
    moveUp(_position_in_heap[x]);
  }

  void decrease(Type x) {
    moveDown(_position_in_heap[x]);
  }

  bool contains(Type x) {
    assert(x < _position_in_heap.size());
    return _position_in_heap[x] != UNDEFINED;
  }

  size_t size() {
    return _heap.size();
  }

  bool empty() {
    return size() == 0;
  }

  void print() const {
    cout << "_____________________" << endl;
    int i = 2;
    for (typename std::vector<Type>::const_iterator it = _heap.begin();
	 it !=  _heap.end();
	 it++) {
      _gt.print(*it);
      cout << (!(i & (i-1)) ? "\n" : " ");
      i++;
    }
    cout << endl << "_____________________" << endl;
  }

};

#endif
