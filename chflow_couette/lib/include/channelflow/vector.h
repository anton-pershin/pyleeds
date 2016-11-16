// vector.h: simple vector class for use with BandedTridiag
// channelflow-1.3 www.channelflow.org
// copyright (C) 2001-2009 John F. Gibson, license declaration at end of file

#ifndef CHANNELFLOW_VECTOR_H
#define CHANNELFLOW_VECTOR_H

#include "channelflow/mathdefs.h"

namespace channelflow {

class Vector {
public:
  Vector(int N=0);
  Vector(const Vector& a);
  Vector(const std::string& filename);
  virtual ~Vector();

  void resize(int N);
  void setToZero();
  virtual void randomize();

  Vector& operator=(const Vector& a);
  inline Real& operator[](int i);
  inline Real operator[](int i) const;
  inline Real& operator()(int i);
  inline Real operator()(int i) const;

  //Vector& operator-();
  Vector& operator*=(Real c);
  Vector& operator/=(Real c);
  Vector& operator+=(Real c);
  Vector& operator-=(Real c);
  Vector& operator+=(const Vector& c);
  Vector& operator-=(const Vector& c);
  Vector& dottimes(const Vector& c);
  Vector& dotdivide(const Vector& c);

  // Destructive transform methods. Vector must have proper length.
  Vector& abs();

  Vector subvector(int offset, int N) const;
  Vector modularSubvector(int offset, int N) const;

  inline int N() const;               // same as length()
  inline int length() const;          // length/dimension of vector
  inline const Real* pointer() const; // Efficiency overrules safety in thesis code.
  inline Real* pointer();

  // save and std::string& ctor form ascii io pair   in filebase.asc
  // read and write      form binary io pair
  void save(const std::string& filebase) const;
  //void read(const std::string& filebase);
  //void write(const std::string& filebase) const;
  //void read(std::istream& is);
  //void write(std::ostream& os) const;
  //void dump(std::ostream& os) const; // binary write, but doesn't preprend length

  friend void swap(Vector& f, Vector& g); // swap data w/o copying arrays

protected:
  Real* data_; // allocated and freed using FFTW routines for byte-alignment
  int N_;
};

// Same as Matlab u(uistart:uiskip:uiend) = v(vistart:viskip:viend)
void assign(Vector& u, int uistart, int uistride, int uiend,
	    Vector& v, int vistart, int vistride, int viend);

Vector operator*(Real c, const Vector& v);
Vector operator+(const Vector& u, const Vector& v);
Vector operator-(const Vector& u, const Vector& v);
Real operator*(const Vector& u, const Vector& v);
bool operator==(const Vector& u, const Vector& v);

Vector dottimes(const Vector& u, const Vector& v);
Vector dotdivide(const Vector& u, const Vector& v);

// Should change these to l2norm, l1Norm, linfNorm
Real L1Norm(const Vector& v);
Real L2Norm(const Vector& v);
Real L2Norm2(const Vector& v);
Real LinfNorm(const Vector& v);

Real L1Dist(const Vector& u, const Vector& v);
Real L2Dist(const Vector& u, const Vector& v);
Real L2Dist2(const Vector& u, const Vector& v);
Real LinfDist(const Vector& u, const Vector& v);

Real mean(const Vector& v);
int maxElemIndex(const Vector& v);
Vector vabs(const Vector& v);

int maxElemIndex(const Vector& v); // index of largest magnitude element

std::ostream& operator<<(std::ostream&, const Vector& a);

inline Real& Vector::operator[](int i) {
  assert(i>=0 && i<N_);
  return data_[i];
}

inline Real Vector::operator[](int i) const {
  assert(i>=0 && i<N_);
  return data_[i];
}
inline Real& Vector::operator()(int i) {
  assert(i>=0 && i<N_);
  return data_[i];
}

inline Real Vector::operator()(int i) const {
  assert(i>=0 && i<N_);
  return data_[i];
}

inline int Vector::N() const {return N_;}
inline int Vector::length() const {return N_;}
inline Real* Vector::pointer() {return data_;}
inline const Real* Vector::pointer() const {return data_;}

}
#endif


/* vector.h: simple vector class for use with BandedTridiag
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
