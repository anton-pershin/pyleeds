// poissonsovler.h: poisson equation solver
// channelflow-1.3 www.channelflow.org
// copyright (C) 2001-2009 John F. Gibson, license declaration at end of file


#ifndef CHANNELFLOW_POISSONSOLVER_H
#define CHANNELFLOW_POISSONSOLVER_H

#include "channelflow/mathdefs.h"
#include "channelflow/vector.h"
#include "channelflow/chebyshev.h"
#include "channelflow/flowfield.h"
#include "channelflow/helmholtz.h"
#include "channelflow/diffops.h"
#include "channelflow/dns.h"

namespace channelflow {

// Solve lapl u = f with dirichlet BCs. u is unknown, f is given.
class PoissonSolver {
public:
  PoissonSolver();
  PoissonSolver(const FlowField& u);
  PoissonSolver(int Nx, int Ny, int Nz, int Nd, Real Lx, Real Lz, Real a, Real b);

  PoissonSolver(const PoissonSolver& ps);
  PoissonSolver& operator=(const PoissonSolver& ps);
  ~PoissonSolver();

  // solve lapl u = f with boundary conditions u=0 or u=bc at y=a and y=b
  void solve(FlowField& u, const FlowField& f) const; // u=0 BC
  void solve(FlowField& u, const FlowField& f, const FlowField& bc) const;

  Real verify(const FlowField& u, const FlowField& f) const ;
  Real verify(const FlowField& u, const FlowField& f, const FlowField& bc) const;

  bool geomCongruent(const FlowField& u) const;
  bool congruent(const FlowField& u) const;

protected:
  int Mx_;      // number of X modes
  int My_;      // number of Chebyshev T(y) modes
  int Mz_;      // number of Z modes
  int Nd_;      // vector dimension
  Real Lx_;
  Real Lz_;
  Real a_;
  Real b_;

  HelmholtzSolver** helmholtz_; // 2d array of HelmHoltz solvers
};


// Solve lapl p = -div(nonl(u+U)) with BCs dp/dy = -nu d^2 (u+U)/dy^2 at y=a,b.
// The pressure p is unknown and u+U is a given velocity field.
// IMPORTANT NOTE: If nonl_method = Rotational, p is the modified pressure
// p + 1/2 u dot u. If you want the true pressure, set nonl_method to
// Convection, SkewSymmetric, Divergence, or Alternating.

class PressureSolver : public PoissonSolver {
public:
  PressureSolver();
  PressureSolver(int Nx, int Ny, int Nz, Real Lx, Real Lz, Real a, Real b,
		 const ChebyCoeff& U, Real nu, NonlinearMethod nonl_method);

  PressureSolver(const FlowField& u, Real nu, NonlinearMethod nonl_method);

  PressureSolver(const FlowField& u, const ChebyCoeff& U, Real nu,
		 NonlinearMethod nonl_method);

  ~PressureSolver();

  //PressureSolver(const PoissonSolver& ps);
  //PressureSolver& operator=(const PoissonSolver& ps);

  // solve lapl p = -div nonl(u+U) with BCs dp/dy = d^2 (u+U)/dy^2 at y=a,b
  void solve(FlowField& p, const FlowField& u);
  Real verify(const FlowField& p, const FlowField& u);

private:
  ChebyCoeff U_;
  ChebyTransform trans_;
  FlowField nonl_;
  FlowField tmp_;
  FlowField div_nonl_;
  Real nu_;
  NonlinearMethod nonl_method_;
};


// Normally I avoid derived classes, because it's hard to apprehend the
// data structure and functionality of the derived class, particluarly when
// the two classes reside in different files. In this case the classes are
// simple enough and can go in the same file, since they naturally fall in
// the same place in the software dependency graph.
// jfg Wed May 18 18:12:13 EDT 2005

} //namespace channelflow
#endif


/* poissonsovler.h: poisson equation solver
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
