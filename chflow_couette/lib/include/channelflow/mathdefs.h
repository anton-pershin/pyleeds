// mathdefs.h: some small mathematical conveniences.
// channelflow-1.3 www.channelflow.org
// copyright (C) 2001-2009 John F. Gibson, license declaration at end of file

#ifndef CHANNELFLOW_MATHDEFS_H
#define CHANNELFLOW_MATHDEFS_H

#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <complex>
#include <cstring>
#include <string>

namespace channelflow {

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef __CYGWIN__
#define NAN 0./0.
#endif
// Define DEBUG to true if NDEBUG isn't set or if it's set to false
#ifndef DEBUG
#ifndef NDEBUG
#define DEBUG 1
#else
#if NDEBUG
#define DEBUG 0
#else
#define DEBUG 1
#endif /* if NDEBUG */
#endif /* ifndef NDEBUG */
#endif /* ifndef DEBUG */



typedef double Real;
#ifndef octave_oct_cmplx_h
typedef std::complex<double> Complex;
#endif



//typedef unsigned int uint;

enum fieldstate { Physical, Spectral };
enum parity { Even, Odd };

const int REAL_DIGITS = 17;
const int REAL_IOWIDTH = 24;
//SWIG barfs on const Complex (0.0,1.0) for some reason
const Complex I = Complex(0.0, 1.0);
const Real pi = M_PI;

std::string i2s(int n);
std::string r2s(Real r);

void cferror(const std::string & message);
void cfpause();

inline int kronecker(int m, int n);
inline int  square(int x);
inline int  cube(int x);
inline Real square(Real x);
inline Real cube(Real x);
inline Real nr_sign(Real a, Real b);
inline void swap(int& a, int& b);
inline void swap(Real& a, Real& b);
inline void swap(Complex& a, Complex& b);

inline int Greater(int a, int b);
inline int lesser(int a, int b);
inline Real Greater(Real a, Real b);
inline Real lesser(Real a, Real b);

int iround(Real x);       // round to closest int
int intpow(int n, int p); // named this way to avoid real,int -> int,int casting
//Real pow(Real n, int p);
//Real pow(Real n, Real p);

Real fmod(Real x, Real m);   // return y s.t. 0<=y<m & y+N*m==x
Real pythag(Real a, Real b); // sqrt(a^2 + b^2) avoiding over/underflow
bool isPowerOfTwo(int n);

// Attempt to open filebase.ext then filebase, return successful filename
// eg string filename = ifstreamOpen(is, "foo", ".bin", ios::binary);
std::string ifstreamOpen(std::ifstream& is, const std::string& filebase,
			 const std::string& ext,
			 std::ios_base::openmode mode=std::ios::in);

// Ascii IO
void save(Real c, const std::string& filebase);
void load(Real& c, const std::string& filebase);
void save(Complex c, const std::string& filebase);
void load(Complex& c, const std::string& filebase);

// Binary IO
void write(std::ostream& os, int n);
void write(std::ostream& os, bool b);
void write(std::ostream& os, Real x);
void write(std::ostream& os, Complex z);
void write(std::ostream& os, fieldstate s);

void read(std::istream& is, int& n);
void read(std::istream& is, bool& b);
void read(std::istream& is, Real& x);
void read(std::istream& is, Complex& z);
void read(std::istream& os, fieldstate& s);

// Little-endian binary read for backwards compatibility
// void readLittleEndian(std::istream& is, int& n);
// void readLittleEndian(std::istream& is, Real& x);

inline Real Re(const Complex& z);   // Real part
inline Real Im(const Complex& z);   // Imag part
//inline Real abs(const Complex& z);// sqrt(a^2 + b^2), provided by std lib
inline Real abs2(const Complex& z); // a^2 + b^2 for a + b i

Real randomReal(Real a=0, Real b=1);  // uniform in [a, b]
Complex randomComplex();              // gaussian about zero

std::ostream & operator << (std::ostream & os, Complex z);

std::ostream & operator << (std::ostream & os, fieldstate f);
std::istream & operator >> (std::istream & is, fieldstate & f);

// Return channlflow version numbers, e.g. 0, 9, 20 for channelflow-0.9.20.
void channelflowVersion(int& major, int& minor, int& update);

// From foo.ext or foo return foo
std::string removeSuffix(const std::string& filename,
			 const std::string& extension);

// From foo.ext or foo return foo.ext
std::string appendSuffix(const std::string& filename,
			 const std::string& extension);

// Does filename have the given extension?
bool hasSuffix(const std::string& filename,
	       const std::string& extension);

bool isReadable(const std::string& filename);


inline int kronecker(int m, int n) {
  return (m==n) ? 1 : 0;
}

//inline Real abs(Real x) {return fabs(x);}
inline Real square(Real x) {
  return x * x;
}
inline Real cube(Real x) {
  return x * x * x;
}
inline Real nr_sign(Real a, Real b) {
  return (b >= 0.0) ? fabs (a) : -fabs (a);
}

inline int square(int x) {
  return x * x;
}
inline int cube(int x) {
  return x * x * x;
}
inline void swap(int& a, int& b) {
  int tmp = a;
  a = b;
  b = tmp;
}
inline void swap(Real& a, Real& b) {
  Real tmp = a;
  a = b;
  b = tmp;
}
inline void swap(Complex& a, Complex& b) {
  Complex tmp = a;
  a = b;
  b = tmp;
}

// Inline replacements for >? and <? GNUisms.
inline int Greater(int a, int b) {
  return (a > b) ? a : b;
}
inline int lesser(int a, int b) {
  return (a < b) ? a : b;
}
inline Real Greater(Real a, Real b) {
  return (a > b) ? a : b;
}
inline Real lesser(Real a, Real b) {
  return (a < b) ? a : b;
}
inline Real Re(const Complex& z) {
  return z.real();
}
inline Real Im(const Complex& z) {
  return z.imag();
}
inline Real abs2(const Complex& z) {
  return norm(z); // NOTE std lib norm is misnamed! Returns a^2+b^2 for a+bi.
}
//inline Complex conjugate(const Complex& z) {
//return Complex(Re(z), -Im(z));
//}

// Beware the standard-library norm(complex) function! It returns the
// squared norm!
//inline Real true_norm(const Complex& z) {
//  return pythag(Re(z), Im(z));
//}
//inline Real norm2(const Complex& z) {
//  return square(Re(z)) + square(Im(z));
//}
//#ifdef WIN32
//#undef assert
//#define assert(expr) (if(!expr) {cout << "Assertion failed. "<<endl; abort();})
//#endif

}
#endif				/* MATHDEFS_H */

/* mathdefs.h: some small mathematical conveniences.
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
