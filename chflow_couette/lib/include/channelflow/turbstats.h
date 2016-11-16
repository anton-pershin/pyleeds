// turbstats.h: a class for computing basic statistics of turbulent flows
// channelflow-1.3 www.channelflow.org
// copyright (C) 2001-2009 John F. Gibson, license declaration at end of file


#ifndef CHANNELFLOW_TURBSTATS_H
#define CHANNELFLOW_TURBSTATS_H

#include "channelflow/mathdefs.h"
#include "channelflow/chebyshev.h"
#include "channelflow/basisfunc.h"
#include "channelflow/flowfield.h"

namespace channelflow {

class TurbStats {
public:
  TurbStats();
  TurbStats(const std::string& filebase);
  TurbStats(const ChebyCoeff& Ubase, Real nu);

  void reset();
  void addData(FlowField& ubase, FlowField& tmp);
  void msave(const std::string& filebase, bool wallunits=false) const;

  // Terminology: utot = U + u = Ubase + ubase
  ChebyCoeff U() const;          // average of total flow
  ChebyCoeff dUdy() const;
  ChebyCoeff Ubase() const;      // base flow (e.g. parabola)
  ChebyCoeff ubase() const;      // mean fluctuation on base flow
  ChebyCoeff uu() const;
  ChebyCoeff uv() const;
  ChebyCoeff uw() const;
  ChebyCoeff vv() const;
  ChebyCoeff vw() const;
  ChebyCoeff ww() const;

  // wall unit stuff
  Real ustar() const;              // sqrt(nu <d/dy utot>)
  Real parabolicReynolds() const;  // h Uparab/nu, center vel of parab w = flux
  Real bulkReynolds() const;       // h Ubulk/nu
  Real centerlineReynolds() const; // h Ucenterline/nu
  Vector yplus() const;            // nu/ustar (y-a)

private:
  int count_;
  Real nu_;          // All ChebyCoeff quantities are sums for means, in utot
  ChebyCoeff Ubase_; // base flow (parabolas, etc).
  ChebyCoeff ubase_; // mean fluc above base flow: utot = Ubase + ubase
  ChebyCoeff U_;     // mean flow = avg(utot)
  ChebyCoeff uu_;    // sum_1^count utot utot
  ChebyCoeff uv_;
  ChebyCoeff uw_;
  ChebyCoeff vv_;
  ChebyCoeff vw_;
  ChebyCoeff ww_;
};


}

#endif // TURBSTATS

/* turbstats.h: a class for computing basic statistics of turbulent flows
 *
 * channelflow-1.3, www.channelflow.org
 *
 * Copyright (C) 2001-2009  John F. Gibson
 *
 * gibson@cns.physics.gatech.edu
 * Center for Nonlinear Sciences, School of Physics
 * Georgia Institute of Technology
 * Atlanta, GA 30332-0430
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, U
 */
