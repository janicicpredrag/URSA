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
#ifndef __AUXILIARY_H__
#define __AUXILIARY_H__

#include <string>
#include <vector>
#include <cstdlib>

std::string toString(int i);
std::string toString(unsigned i);
std::string toString(unsigned short i);
std::string toString(double i);

bool isInt(const std::string& str);
bool isFloat(const std::string& str);

//generates a psuedo-random float from [0, 1)
float randFloat();

//generates a psuedo-random int from [0, n)
int randInt(int n);

//generates a pseudo-random bool
bool randBool();


#endif
