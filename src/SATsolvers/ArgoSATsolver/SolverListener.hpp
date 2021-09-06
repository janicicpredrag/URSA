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
#ifndef __SOLVER_LISTENER__H__
#define __SOLVER_LISTENER__H__

#include "Literal.hpp"
#include "Clause.hpp"

namespace ArgoSat {

class SolverListener {
 public:
  virtual ~SolverListener() {}
  virtual void init() {}
  virtual void onDecide(Literal /*l*/) {}
  virtual void onAssert(Literal /*l*/) {}
  virtual void onPropagate(Literal /*l*/, const Clause* /*clause*/) {}
  virtual void onBacktrack(Literal /*l*/) {}
  virtual void onConflict(const Clause* /*conflictClause*/) {}
  virtual void onExplain(Literal /*l*/, const Clause* /*clause*/) {}
  virtual void onLearn(const Clause* /*clause*/) {}
  virtual void onForget() {}
  virtual void onForgetClause(const Clause* /*clause*/) {}
  virtual void onRestart() {}
};

}//namespace ArgoSat
#endif
