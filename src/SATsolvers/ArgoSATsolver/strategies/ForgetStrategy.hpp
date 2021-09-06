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
#ifndef __FORGET_STRATEGY_H__
#define __FORGET_STRATEGY_H__

#include <cstring>
#include <cmath>


namespace ArgoSat {

  class Solver;

  class ForgetStrategy {
  public:
    virtual ~ForgetStrategy() {}
    virtual bool shouldForget() = 0;
    static ForgetStrategy* createFromCmdLine(Solver& solver, int argc, char* argv[], int curr);
  };

}//namespace ArgoSat

#endif
