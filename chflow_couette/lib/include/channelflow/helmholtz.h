// helmholtz.h: solution of Helmholtz eqn in Chebyshev expansions
// channelflow-1.3 www.channelflow.org
// copyright (C) 2001-2009 John F. Gibson, license declaration at end of file

#ifndef CHANNELFLOW_HELMHOLTZ_H
#define CHANNELFLOW_HELMHOLTZ_H

#include "channelflow/mathdefs.h"
#include "channelflow/bandedtridiag.h"
#include "channelflow/chebyshev.h"


namespace channelflow {

// HelmholtzSolver solves the 1d Helmholtz eqn:  u'' - lambda u = f on [a,b]
// with Dirichlet or Neuman boundary conditions at a and
// with spectral chebyshev "tau" algorithm. u is unknown, f and lambda
// are known.  The linear algebra problem looks like A u_hat = B f_hat,
// where u_hat = chebyfft(u). Algorithm is from Canuto and Hussaini
// "Spectral Methods in Fluid Dynamics" section 5.1.2.
// For convenience I added a nu argument that defaults to 1.0
// Change it to solve nu u'' - lambda u = f

class HelmholtzSolver {
public:
  HelmholtzSolver();
  HelmholtzSolver(int numberModes, Real a, Real b, Real lambda, Real nu=1.0);

  // a and b are dirichlet BCs for u: ua = u(a), ub = u(b)
  void solve(ChebyCoeff& u, const ChebyCoeff& f, Real ua, Real ub) const;
  void verify(const ChebyCoeff& u, const ChebyCoeff& f, Real ua, Real ub, bool verbose=false) const;
  Real residual(const ChebyCoeff& u, const ChebyCoeff& f, Real ua, Real ub) const;

  // Solve nu u''(y) - lambda u(y) - mu = f(y), mean(u) = umean, u(-+1)=a,b
  // for u and mu, given nu, lambda, f, ua, ub, and umean.
  void solve(ChebyCoeff& u, Real& mu, const ChebyCoeff& f, Real umean, Real ua, Real ub) const;
  void verify(ChebyCoeff& u, Real& mu, const ChebyCoeff& f, Real umean, Real ua, Real ub) const;
  Real residual(const ChebyCoeff& u, Real mu, const ChebyCoeff& f, Real umean, Real ua, Real ub) const;

  Real lambda() const;
private:
  int N_;          // Canuto & Hussain's N, section 5.1.2
  int nModes_;     // N_+1
  int nEvenModes_; // number of even modes
  int nOddModes_;  // number of odd modes
  Real a_;
  Real b_;
  Real lambda_;
  Real nu_;
  inline int beta(int n) const;
  inline int c(int n) const;

  // Matrix form of Helmholtz eqn is A uhat = B fhat, even/odd modes decouple
  BandedTridiag Ae_; // LHS Tridiag for even modes: Ae uhat_even = Be fhat_even
  BandedTridiag Ao_; // LHS Tridiag for odd  modes: Ao uhat_odd  = Bo fhat_odd
  BandedTridiag Be_; // RHS Tridiag for even modes
  BandedTridiag Bo_; // RHS Tridiag for odd modes
};

// Canuto & Hussaini pg 67 and 130
//inline int HelmholtzSolver::beta(int n) const {return (n > N_-2) ? 0 : 1;}
inline int HelmholtzSolver::beta(int n) const {return (n > N_-2) ? 0 : 1;}
inline int HelmholtzSolver::c(int n) const {return (n==0 || n==N_) ? 2 : 1;}

} //namespace channelflow
#endif

/* helmholtz.h: Class for solution of Helmholtz eqn in Chebyshev expansion.
 *
 * channelflow-1.3, www.channelflow.org
 *
 * Copyright (C) 2001-2009  John F. Gibson
 *
 * John F. Gibson, gibson@cns.physics.gatech.edu
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
