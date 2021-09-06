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
 * This program is inspired by Picosat solver (C) Een, Sorensson 2003-2006.
 ******************************************************************************/
#include "RestartStrategyPicosat.hpp"
#include "Auxiliary.hpp"

namespace ArgoSat {

  RestartStrategy* PicosatRestartStrategy::createFromCmdLine(Solver& solver, int argc, char* argv[], int curr) {
    if (curr >= argc || !isInt(argv[curr]))
      return new PicosatRestartStrategy(solver);
    int r0 = atoi(argv[curr++]);

    if (curr >= argc || !isFloat(argv[curr]))
      throw std::string("PicosatRestartStrategy::Increment expected");
    double i = atof(argv[curr++]);

    return new PicosatRestartStrategy(solver, r0, i);
  }

}//namespace ArgoSat

