// chebyshev.h: Real- and Complex-valued Chebyshev expansion classes.
// channelflow-1.3 www.channelflow.org
// copyright (C) 2001-2009 John F. Gibson, license declaration at end of file

#ifndef CHANNELFLOW_CHEBYSHEV_H
#define CHANNELFLOW_CHEBYSHEV_H

#include <fftw3.h>
#include "channelflow/mathdefs.h"
#include "channelflow/vector.h"

namespace channelflow {

enum BC       {Free, Diri};
enum NormType {Uniform, Chebyshev}; // Uniform weighting in y or 1/sqrt(1-y^2)


void fftw_loadwisdom(const char* filename=0); //defaults to ~/.fftw_wisdom
void fftw_savewisdom(const char* filename=0);

Vector chebypoints(int N, Real a, Real b); // N   pts

Real chebyIP(int m, int n); // Integral_(-1)^1 Tm(y) Tn(y) dy
inline int cheby_c(int n) {return (n>0) ? 1 : ((n==0) ? 2 : 0);}

Real legendre(int n, Real x);    // Value of Legendre polynomial P_n(x)
Real chebyshev(int n, Real x);   // Value of Chebyshev polynomial T_n(x)

void gaussLegendreQuadrature(int N, Real a, Real b, Vector& x, Vector& w);

class ChebyTransform;

class ChebyCoeff : public Vector {
public:

  ChebyCoeff();   /// Null constructor
  ChebyCoeff(int N, Real a, Real b, fieldstate s=Spectral);
  ChebyCoeff(const Vector& v, Real a, Real b, fieldstate s=Spectral);
  ChebyCoeff(int N, const ChebyCoeff& g);   // copy first N elems.
  ChebyCoeff(const std::string& filebase);       // read ascii from file
  ~ChebyCoeff();

  void save(const std::string& filebase, fieldstate s=Physical) const;
  void binaryDump(std::ostream& os) const;
  void binaryLoad(std::istream& is);

  void reconfig(const ChebyCoeff& f);
  void randomize(Real magn, Real smoothness, BC aBC, BC bBC);
  void setBounds(Real a, Real b);
  void setState(fieldstate s);
  void setToZero();
  void fill(const ChebyCoeff& g);
  void interpolate(const ChebyCoeff& g); // *this is on subdomain of g.
  void reflect(const ChebyCoeff& g, parity p); // reflect g about u.a()

  Real eval_a() const;
  Real eval_b() const;
  Real eval(Real x) const;
  ChebyCoeff eval(const Vector& x) const;
  void eval(const Vector& x, ChebyCoeff& g) const;
  Real slope_a() const;
  Real slope_b() const;

  inline Real a() const;
  inline Real b() const;
  inline Real L() const;
  inline int N() const;
  inline int numModes() const;
  inline fieldstate state() const;
  Real mean() const;
  //Real energy() const;

  ChebyCoeff& operator*=(Real c);
  ChebyCoeff& operator+=(const ChebyCoeff& g);
  ChebyCoeff& operator-=(const ChebyCoeff& g);
  ChebyCoeff& operator*=(const ChebyCoeff& g); // dottimes, only for Physical

  // These transforms make temp ChebyTransforms thus are more expensive.
  void chebyfft();       // transform from Physcial to Spectral
  void ichebyfft();      // transform from Spectral to Physcial
  void makeSpectral();   // if Physical, transform to Spectral
  void makePhysical();   // if Spectral, transform to Physical
  void makeState(fieldstate s); // if state != s, transform to state s

  void chebyfft(const ChebyTransform& t);
  void ichebyfft(const ChebyTransform& t);
  void makeSpectral(const ChebyTransform& t);
  void makePhysical(const ChebyTransform& t);
  void makeState(fieldstate s, const ChebyTransform& t);

  bool congruent(const ChebyCoeff& g) const;

  friend void swap(ChebyCoeff& f, ChebyCoeff& g);

private:
  Real a_;             // lower bound of domain
  Real b_;             // upper bound of domain
  fieldstate state_;   // indicates Physical or Spectral state of object
  friend class ChebyTransform;
};

class ComplexChebyCoeff {
public:
  ComplexChebyCoeff();
  ComplexChebyCoeff(int N, Real a, Real b, fieldstate s);
  ComplexChebyCoeff(int N, const ComplexChebyCoeff& f); // copy first N elems.
  ComplexChebyCoeff(const ChebyCoeff& re, const ChebyCoeff& im);
  ComplexChebyCoeff(const std::string& filename); // read ascii from file
//   ComplexChebyCoeff(std::istream& is);          // read binary from stream

  void reconfig(const ComplexChebyCoeff& f);
  void resize(int N);
  void randomize(Real magn, Real smoothness, BC aBC, BC bBC);
  void setToZero();
  void setBounds(Real a, Real b);
  void setState(fieldstate s);
  void fill(const ComplexChebyCoeff& g); // set this[i]=v[i], rest=0
  void interpolate(const ComplexChebyCoeff& g); // set this[i]=v[i], rest=0
  void reflect(const ComplexChebyCoeff& g, parity p); // reflect v about u.a()

  Complex eval_a() const;
  Complex eval_b() const;
  Complex eval(Real x) const;
  Complex slope_a() const;
  Complex slope_b() const;

  Complex mean() const;
  //Real energy() const;

  inline Real a() const;
  inline Real b() const;
  inline Real L() const;
  inline int N() const;
  inline int length() const;
  inline int numModes() const;
  inline fieldstate state() const;

  inline Complex operator[](int n) const;
  inline void set(int n, Complex c);
  inline void add(int n, Complex c);
  inline void sub(int n, Complex c);

  ComplexChebyCoeff& operator += (const ComplexChebyCoeff& f);
  ComplexChebyCoeff& operator -= (const ComplexChebyCoeff& f);
  ComplexChebyCoeff& operator *= (Real c);
  ComplexChebyCoeff& operator *= (Complex c);   // gcc-2.95 can't handle this
  ComplexChebyCoeff& operator *= (const ComplexChebyCoeff& c); // dottimes

  void conjugate(); // destructive
  void save(const std::string& filebase, fieldstate s=Physical) const;
  void binaryDump(std::ostream& os) const;
  void binaryLoad(std::istream& is);

  bool congruent(const ComplexChebyCoeff& g) const;

  void chebyfft();
  void ichebyfft();
  void makeSpectral();
  void makePhysical();
  void makeState(fieldstate s);

  void chebyfft(const ChebyTransform& t);
  void ichebyfft(const ChebyTransform& t);
  void makeSpectral(const ChebyTransform& t);
  void makePhysical(const ChebyTransform& t);
  void makeState(fieldstate s, const ChebyTransform& t);

  friend void swap(ComplexChebyCoeff& f, ComplexChebyCoeff& g);

  ChebyCoeff re;
  ChebyCoeff im;
};


class ChebyTransform {
public:
  ChebyTransform(int N, uint fftw_flags = FFTW_ESTIMATE);
  ChebyTransform(const ChebyTransform& t);            // unimplemented
  ChebyTransform& operator=(const ChebyTransform& t); // unimplemented
  ~ChebyTransform();

  inline int N() const;
  inline int length() const;

  friend class ChebyCoeff;
  friend class ComplexChebyCoeff;

private:
  int N_;
  uint flags_;
  fftw_plan cosfftw_plan_;
};

ChebyCoeff operator*(Real c, const ChebyCoeff& g);
ChebyCoeff operator+(const ChebyCoeff& f, const ChebyCoeff& g);
ChebyCoeff operator-(const ChebyCoeff& f, const ChebyCoeff& g);
bool operator==(const ChebyCoeff& f, const ChebyCoeff& g);
bool operator!=(const ChebyCoeff& f, const ChebyCoeff& g);

void  diff(const ChebyCoeff& f, ChebyCoeff& df);
void diff2(const ChebyCoeff& f, ChebyCoeff& d2f);
void diff2(const ChebyCoeff& f, ChebyCoeff& d2f, ChebyCoeff& tmp);
void  diff(const ChebyCoeff& f, ChebyCoeff& df, int n);
ChebyCoeff  diff(const ChebyCoeff& f);
ChebyCoeff diff2(const ChebyCoeff& f);
ChebyCoeff  diff(const ChebyCoeff& f, int n);

// Integrate sets the arbitrary const of integration so that mean(u)==0.
void integrate(const ChebyCoeff& df, ChebyCoeff& f);
ChebyCoeff integrate(const ChebyCoeff& df);

void legendre(int n, ChebyCoeff& u, ChebyTransform& trans, bool normalize=false);
// ChebyCoeff chebyshev(int N, int n,ChebyTransform& trans, bool normalize=false);

//   normalize  ?   false                  :  true
// L2Norm2(f)   =      Int_a^b f^2 dy            1/(b-a) Int_a^b f^2 dy
// L2Dist2(f,g) =      Int_a^b (f-g)^2 dy        1/(b-a) Int_a^b (f-g)^2 dy
// L2Norm(f)    = sqrt(Int_a^b f^2 dy)      sqrt(1/(b-a) Int_a^b f^2 dy)
// L2Dist(f,g)  = sqrt(Int_a^b (f-g)^2 dy)  sqrt(1/(b-a) Int_a^b (f-g)^2 dy
// L2IP...(f,g) =      Int_a^b f g dy            1/(b-a) Int_a^b f g dy
//

Real L2Norm2(const ChebyCoeff& f, bool normalize=true);
Real L2Dist2(const ChebyCoeff& f, const ChebyCoeff& g, bool normalize=true);
Real L2Norm(const ChebyCoeff& f, bool normalize=true);
Real L2Dist(const ChebyCoeff& f, const ChebyCoeff& g, bool normalize=true);
Real L2InnerProduct(const ChebyCoeff& f, const ChebyCoeff& g,
		    bool normalize=true);

Real chebyNorm2(const ChebyCoeff& f, bool normalize=true);
Real chebyDist2(const ChebyCoeff& f, const ChebyCoeff& g, bool normalize=true);
Real chebyNorm(const ChebyCoeff& f, bool normalize=true);
Real chebyDist(const ChebyCoeff& f, const ChebyCoeff& g, bool normalize=true);
Real chebyInnerProduct(const ChebyCoeff& f, const ChebyCoeff& g,
		       bool normalize=true);

Real norm2(const ChebyCoeff& f, NormType n, bool normalize=true);
// Was undefined
/*Real dist2(const ChebyCoeff& f, NormType n, const ChebyCoeff& g,
	   bool normalize=true); */
Real norm(const ChebyCoeff& f, NormType n, bool normalize=true);
Real dist(const ChebyCoeff& f, const ChebyCoeff& g, NormType n,
	  bool normalize=true);
Real innerProduct(const ChebyCoeff& f, const ChebyCoeff& g, NormType n,
		    bool normalize=true);

Real L1Norm(const ChebyCoeff& f, bool normalize=true);
Real L1Dist(const ChebyCoeff& f, const ChebyCoeff& g, bool normalize=true);

Real LinfNorm(const ChebyCoeff& f);
Real LinfDist(const ChebyCoeff& f, const ChebyCoeff& g);


ComplexChebyCoeff operator*(Real c, const ComplexChebyCoeff& g);
ComplexChebyCoeff operator+(const ComplexChebyCoeff& f, const ComplexChebyCoeff& g);
ComplexChebyCoeff operator-(const ComplexChebyCoeff& f, const ComplexChebyCoeff& g);
bool operator==(const ComplexChebyCoeff& f, const ComplexChebyCoeff& g);
bool operator!=(const ComplexChebyCoeff& f, const ComplexChebyCoeff& g);

void  diff(const ComplexChebyCoeff& f, ComplexChebyCoeff& df);
void diff2(const ComplexChebyCoeff& f, ComplexChebyCoeff& d2f);
void diff2(const ComplexChebyCoeff& f, ComplexChebyCoeff& d2f,
	   ComplexChebyCoeff& tmp);
void  diff(const ComplexChebyCoeff& f, ComplexChebyCoeff& d2f, int n);

ComplexChebyCoeff  diff(const ComplexChebyCoeff& f);
ComplexChebyCoeff diff2(const ComplexChebyCoeff& f);
ComplexChebyCoeff  diff(const ComplexChebyCoeff& f, int n);

void integrate(const ComplexChebyCoeff& df, ComplexChebyCoeff& f);
ComplexChebyCoeff integrate(const ComplexChebyCoeff& df);

std::ostream& operator<<(std::ostream& os, const ComplexChebyCoeff& f);

inline ChebyCoeff& Re(ComplexChebyCoeff& f);
inline ChebyCoeff& Im(ComplexChebyCoeff& f);
inline const ChebyCoeff& Re(const ComplexChebyCoeff& f);
inline const ChebyCoeff& Im(const ComplexChebyCoeff& f);

// L1Norms are simple sums of |u_n| with no normalization whatsoever.
//Real L1Norm(const ComplexChebyCoeff& f);
//Real L1Dist(const ComplexChebyCoeff& f, const ComplexChebyCoeff& g);


//   normalize  ?           false                   true (c= 1/(b-a))
// L2Norm2(f)   =      Int_a^b f f* dy         1/(b-a) Int_a^b f f* dy
// L2Dist2(f,g) =      Int_a^b (f-g)(f-g)* dy  1/(b-a) Int_a^b (f-g)(f-g)* dy
// L2Norm(f)    = sqrt(Int_a^b f f* dy)         sqrt(c Int_a^b f f* dy)
// L2Dist(f,g)  = sqrt(Int_a^b (f-g)(f-g)* dy)  sqrt(c Int_a^b (f-g)(f-g)* dy
// L2IP...(f,g) =      Int_a^b f g dy          1/(b-a) Int_a^b f g* dy
//
Real L2Norm2(const ComplexChebyCoeff& f, bool normalize=true);
Real L2Dist2(const ComplexChebyCoeff& f, const ComplexChebyCoeff& g,
	    bool normalize=true);
Real L2Norm(const ComplexChebyCoeff& f, bool normalize=true);
Real L2Dist(const ComplexChebyCoeff& f, const ComplexChebyCoeff& g,
	    bool normalize=true);
Complex L2InnerProduct(const ComplexChebyCoeff& f, const ComplexChebyCoeff& g,
		       bool normalize=true);


Real chebyNorm2(const ComplexChebyCoeff& f, bool normalize=true);
Real chebyDist2(const ComplexChebyCoeff& f, const ComplexChebyCoeff& g,
		bool normalize=true);
Real chebyNorm(const ComplexChebyCoeff& f, bool normalize=true);
Real chebyDist(const ComplexChebyCoeff& f, const ComplexChebyCoeff& g,
	       bool normalize=true);
Complex chebyInnerProduct(const ComplexChebyCoeff& f, const ComplexChebyCoeff& g,
		       bool normalize=true);

Real norm2(const ComplexChebyCoeff& f, NormType n, bool normalize=true);
// Was Undefined
/*Real dist2(const ComplexChebyCoeff& f, NormType n, const ChebyCoeff& g,
	   bool normalize=true); */
Real norm(const ComplexChebyCoeff& f, NormType n, bool normalize=true);
Real dist(const ComplexChebyCoeff& f, const ComplexChebyCoeff& g, NormType n,
	  bool normalize=true);
Complex innerProduct(const ComplexChebyCoeff& f, const ComplexChebyCoeff& g,
		  NormType n,  bool normalize=true);

Real L1Norm(const ComplexChebyCoeff& f, bool normalize=true);
Real L1Dist(const ComplexChebyCoeff& f, const ComplexChebyCoeff& g, bool normalize=true);

Real LinfNorm(const ComplexChebyCoeff& f);
Real LinfDist(const ComplexChebyCoeff& f, const ComplexChebyCoeff& g);

// ComplexChebyCoeff chebyEval(const ComplexChebyCoeff& c, const Vector& x);

inline Real ChebyCoeff::a() const {return a_;}
inline Real ChebyCoeff::b() const {return b_;}
inline Real ChebyCoeff::L() const {return b_-a_;}
inline int ChebyCoeff::N() const {return N_;}
inline int ChebyCoeff::numModes() const {return N_;}
inline fieldstate ChebyCoeff::state() const {return state_;}

inline Real ComplexChebyCoeff::a() const {return re.a();}
inline Real ComplexChebyCoeff::b() const {return re.b();}
inline Real ComplexChebyCoeff::L() const {return re.L();}
inline int ComplexChebyCoeff::N() const {return re.N();}
inline int ComplexChebyCoeff::length() const {return re.length();}
inline int ComplexChebyCoeff::numModes() const {return re.numModes();}
inline fieldstate ComplexChebyCoeff::state() const {return re.state();}

inline ChebyCoeff& Re(ComplexChebyCoeff& f) {return f.re;}
inline ChebyCoeff& Im(ComplexChebyCoeff& f) {return f.im;}
inline const ChebyCoeff& Re(const ComplexChebyCoeff& f) {return f.re;}
inline const ChebyCoeff& Im(const ComplexChebyCoeff& f) {return f.im;}


inline Complex ComplexChebyCoeff::operator[](int i) const {
  return re[i] + I*im[i];
}
inline void ComplexChebyCoeff::set(int i, Complex c) {
  re[i] = Re(c);
  im[i] = Im(c);
}
inline void ComplexChebyCoeff::add(int i, Complex c) {
  re[i] += Re(c);
  im[i] += Im(c);
}
inline void ComplexChebyCoeff::sub(int i, Complex c) {
  re[i] -= Re(c);
  im[i] -= Im(c);
}

inline int ChebyTransform::N() const { return N_;}
inline int ChebyTransform::length() const { return N_;}

}

#endif /* CHEBYSHEV_H */

/* chebyshev.h: Real- and Complex-valued Chebyshev expansion classes.
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 */
