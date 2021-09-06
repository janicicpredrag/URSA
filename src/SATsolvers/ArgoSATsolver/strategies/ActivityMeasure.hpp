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
#ifndef __ACTIVITY_H__
#define __ACTIVITY_H__

#define MAX_ACTIVITY (1e20)

class ActivityMeasure {
 public:
  ActivityMeasure(double bumpAmount, double decayFactor) 
    : _bumpAmount(bumpAmount),
    _decayFactor(decayFactor) {
  }

  void bump(double& activity) {
    activity += _bumpAmount;
  }

  void decay() {
    _bumpAmount *= _decayFactor;
  }

  void rescale(double& activity) {
    activity *= (1.0/MAX_ACTIVITY);
  }
    
  void rescaleBumpAmount() {
    _bumpAmount *= (1.0/MAX_ACTIVITY);
  }

  void setDecayFactor(float factor) {
    _decayFactor = factor;
  }

 private:
  double _bumpAmount;
  double _decayFactor;
};

#endif
