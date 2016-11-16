// bandedtridiag.h: Banded tridiagonal matrix designed for Chebyshev Helomholtz
// channelflow-1.3, www.channelflow.org
// copyright (C) 2001-2009  John F. Gibson, full license at end of file

#ifndef CHANNELFLOW_BANDEDTRIDIAG_H
#define CHANNELFLOW_BANDEDTRIDIAG_H

#include "channelflow/mathdefs.h"
#include "channelflow/vector.h"

namespace channelflow
{

typedef double Real;

// Banded trigdiagonal matrix of form

// DxxxxxA
// xxx
//  xxx
//   xxx
//    xxx
//     xxx
//      xx
// row-major ordering. Pointers a_ and d_ point to elems A and D.

class BandedTridiag {
public:
  BandedTridiag();
  BandedTridiag(int M);
  BandedTridiag(const BandedTridiag& A);
  BandedTridiag(const std::string& filebase);
  ~BandedTridiag();

  BandedTridiag& operator=(const BandedTridiag& A);

  bool operator==(const BandedTridiag& A) const;
  inline int numrows() const;
  //inline int numcols() const;

  inline Real& band(int j);   // A[0,j];
  inline Real& diag(int i);   // A[i,i];
  inline Real& updiag(int i); // A[i,i+1]
  inline Real& lodiag(int i); // A[i,i-1];

  inline const Real& band(int i) const;
  inline const Real& diag(int i) const;
  inline const Real& updiag(int i) const;
  inline const Real& lodiag(int i) const;

  Real& elem(int i, int j);
  const Real& elem(int i, int j) const;
  void ULdecomp(); // no pivoting

  void ULsolve(Vector& b) const;
  void multiply(const Vector& x, Vector& b) const;
  void ULsolveStrided(Vector& b, int offset, int stride) const;
  void multiplyStrided(const Vector& x, Vector& b, int offset, int stride) const;

  void print() const;
  void ULprint() const;
  void test() const;
  void save(const std::string& filebase) const; // each row of output is i j Aij

private:
  int M_;     // # rows (square matrix)
  int Mbar_;  // M-1
  Real* a_;   // start of data array for elem storage
  Real* d_;   // address of first diagonal elem. d_ = a_ + Mbar_
  Real* invdiag_; // inverses of diagonal elements
  bool UL_;   // has UL decomp been done?

  int numNonzeros(int M) const;
  int numRows(int nnz) const;
};

inline Real& BandedTridiag::band(int j) {
  assert(j>=0 && j<M_);
  return a_[Mbar_-j];
}

inline Real& BandedTridiag::diag(int i) {
  assert(i>=0 && i<M_);
  return d_[3*i];
}

inline Real& BandedTridiag::updiag(int i) {
  assert(i>=0 && i<M_);
  return d_[3*i - 1];
}

inline Real& BandedTridiag::lodiag(int i) {
  assert(i>=0 && i<M_);
  return d_[3*i + 1];
}

inline const Real& BandedTridiag::band(int j) const {
  assert(j>=0 && j<M_);
  return a_[Mbar_-j];
}

inline const Real& BandedTridiag::diag(int i) const {
  assert(i>=0 && i<M_);
  return d_[3*i];
}

inline const Real& BandedTridiag::updiag(int i) const {
  assert(i>=0 && i<M_);
  return d_[3*i - 1];
}

inline const Real& BandedTridiag::lodiag(int i) const {
  assert(i>=0 && i<M_);
  return d_[3*i + 1];
}

} //namespace channelflow

#endif


/* bandedtridiag.h: Banded tridiagonal matrix designed for Chebyshev Helomholtz
 * channelflow-1.3, www.channelflow.org
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
