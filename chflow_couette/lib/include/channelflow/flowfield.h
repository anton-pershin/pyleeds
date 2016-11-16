// flowfield.h: Class for N-dim Fourier x Chebyshev x Fourier expansions
// channelflow-1.3 www.channelflow.org
// copyright (C) 2001-2009 John F. Gibson, license declaration at end of file

#ifndef CHANNELFLOW_FLOWFIELD_H
#define CHANNELFLOW_FLOWFIELD_H

#include <fftw3.h>
#include "channelflow/mathdefs.h"
#include "channelflow/vector.h"
#include "channelflow/chebyshev.h"
#include "channelflow/basisfunc.h"
#include "channelflow/realprofile.h"
#include "channelflow/realprofileng.h"

#include <omp.h>

// A brief overview of FlowField (for more info see channelflow.ps in docs/).
//
// FlowField represents 3d vector fields with Fourier x Chebyshev x Fourier
// spectral expansions. I.e.
// u(x,y,z,i) =
//   sum{kx,kz,ny,i} uhat_{kx,kz,ny,i} T_ny(y) exp(2 pi I [kx x/Lx + kz z/Lz])
//
// FlowField represents such expansions with multidimensional arrays of
// spectral coefficients uhat_{mx,mz,my,i} or gridpoint data u(nx,ny,nz,i).
// (see docs for the relation between indices nx, mx, and kx).
//
// FlowFields have independent xz and y spectral transforms, therefore they
// can be in any one of four states: (xzstate, ystate) == (Physical, Physical),
// (Physical, Spectral), (Spectral, Physical), or (Spectral, Spectral).

// Most mathematical operations must be computed on when the FlowField is in
// (Spectral, Spectral) state. Some operations will automatically convert to
// the appropriate state to do the computation, then convert back to original
// state before returning. Not sure whether this is good --it's convenient but
// potentially leads to silent inefficiencies.

// The debugging libraries check that FlowFields are in the proper state,
// when it matters. Test your code for correct FlowField transforms and
// states by compiling and running with debug libs: "make foo.dx; ./foo.dx".

namespace channelflow {

class FieldSymmetry;

class FlowField {

public:
  FlowField();
  FlowField(int Nx, int Ny, int Nz, int Nd, Real Lx, Real Lz, Real a, Real b,
	    fieldstate xzstate=Spectral, fieldstate ystate=Spectral,
	    uint fftw_flags = FFTW_ESTIMATE);

  FlowField(int Nx, int Ny, int Nz, int Nd, int tensorOrder,
	    Real Lx, Real Lz, Real a, Real b,
	    fieldstate xzstate=Spectral, fieldstate ystate=Spectral,
	    uint fftw_flags = FFTW_ESTIMATE);

  FlowField(const FlowField& u);
  FlowField(const std::string& filebase); // opens filebase.h5 or filebase.ff
  FlowField(const std::string& filebase, int major, int minor, int update);
  ~FlowField();

  FlowField& operator = (const FlowField& u); // assign identical copy of U

  bool isNull(); // true if Nx=Ny=Nz=Nd=0

  // match geom params, set to zero
  void reconfig(const FlowField& u, uint fftw_flags = FFTW_ESTIMATE);
  void resize(int Nx, int Ny, int Nz, int Nd, Real Lx, Real Lz, Real a, Real b,
	      uint fftw_flags = FFTW_ESTIMATE);
  void rescale(Real Lx, Real Lz);
  void interpolate(const FlowField& u); // interpolate U onto this grid.

  void optimizeFFTW(uint fftw_flags = FFTW_MEASURE);
  // void call_fftw(); // debugging

  // Real    operator()(nx,ny,nz,i):  0<=nx<Nx, 0<=ny<Ny, 0<=nz<Nz, 0<=i<Nd
  // Complex cmplx(mx,my,mz,i):       0<=mx<Mx, 0<=my<My, 0<=mz<Mz, 0<=i<Nd
  //                           where    Mx==Nx    My==Ny  Mz==Nz_/2+1;

  // Element access methods for vector-valued fields
  inline Real&        operator()(int nx, int ny, int nz, int i);
  inline const Real&  operator()(int nx, int ny, int nz, int i) const;
  inline Complex&       cmplx(int mx, int my, int mz, int i);
  inline const Complex& cmplx(int mx, int my, int mz, int i) const;
  //inline Complex&       coeff(int mx, int my, int mz, int i);
  //inline const Complex& coeff(int mx, int my, int mz, int i) const;

  // Element access methods for tensor-valued fields
  inline Real&        operator()(int nx, int ny, int nz, int i, int j);
  inline const Real&  operator()(int nx, int ny, int nz, int i, int j) const;
  inline Complex&       cmplx(int mx, int ny, int mz, int i, int j);
  inline const Complex& cmplx(int mx, int ny, int mz, int i, int j) const;
  //inline Complex&       coeff(int mx, int ny, int mz, int i, int j);
  //inline const Complex& coeff(int mx, int ny, int mz, int i, int j) const;

  FlowField operator[](int i) const; // extract ith component, inefficient


  //Real eval(Real x, Real y, Real z, int i) const;
  //Complex eval(int nx, Real y, int nz, int i) const;


  //void addProfile(const ChebyCoeff& profile, int i=0); // assume kx=kz=0
  //void addProfile(const ComplexChebyCoeff& profile, int mx, int mz, int i=0,
  //bool addconj=true);
  //void addProfile(const BasisFunc& profile, bool addconj=true);
  //void addProfile(const RealProfile& profile);

  ComplexChebyCoeff profile(int mx, int mz, int i) const;
  BasisFunc         profile(int mx, int mz) const;
//  ChebyCoeff    realprofile(int mx, int mz, int i, Sign s) const;
//  RealProfile   realProfile(int mx, int mz, Sign s) const;


  // Destructive transforms.
  //void realfft_xz();
  //void irealfft_xz();
  //void chebyfft_y();
  //void ichebyfft_y();

  void makeSpectral_xz();
  void makePhysical_xz();
  void makeSpectral_y();
  void makePhysical_y();
  void makeSpectral();
  void makePhysical();
  void makeState(fieldstate xzstate, fieldstate ystate);

  void setToZero();
  void perturb(Real magnitude, Real spectralDecay, bool meanflow=true);
  void addPerturbation(int kx, int kz, Real mag, Real spectralDecay);
  void addPerturbations(int kxmax, int kzmax, Real mag, Real spectralDecay, bool meanflow=true);
  void addPerturbations(Real magnitude, Real spectralDecay, bool meanflow=true);

  // Symmetry operations: (u,v,w)(x,y,z) -> ...
  //FlowField& translate(Real dx, Real dz);     // (u,v,w)(x+dx, y, z+dz)
  //FlowField& reflect();                       // (u,v,-w)(x, y, -z)
  //FlowField& rotate();                        // (-u,-v,w)(-x, -y, z)
  //FlowField& shiftReflect();                  // (u,v,-w)(x+Lx/2, y, -z)
  //FlowField& shiftRotate();                   // (-u,-v,w)(Lx/2-x, -y, Lz/2+z)

  FlowField& operator *=(const FieldSymmetry& s);
  FlowField& project(const FieldSymmetry& s);
  FlowField& project(const array<FieldSymmetry>& s);

  inline int numXmodes() const;   // should eliminate
  inline int numYmodes() const;
  inline int numZmodes() const;
  inline int numXgridpts() const; // should change to Nxmodes
  inline int numYgridpts() const;
  inline int numZgridpts() const;
  inline int vectorDim() const;   // should eliminate

  inline int Nx() const; // same as numXgridpts()
  inline int Ny() const; // same as numYgridpts()
  inline int Nz() const; // same as numZgridpts()
  inline int Nd() const; // same as vectorDim()
  inline int Mx() const; // same as numXmodes()
  inline int My() const; // same as numYmodes()
  inline int Mz() const; // same as numZmodes()

  inline int mx(int kx) const;  // where, in the array, is kx?
  inline int mz(int kz) const;  // should be mx,mz rather than nx,nz
  inline int kx(int mx) const;  // the wavenumber of the nxth array elem
  inline int kz(int mz) const;

  inline int kxmax() const;     // the largest value kx takes on
  inline int kzmax() const;
  inline int kxmin() const;     // the smallest value kx takes on
  inline int kzmin() const;
  inline int kxminDealiased() const; // |kx| > kxmaxDealiased is aliased mode
  inline int kxmaxDealiased() const; // |kx| > kxmaxDealiased is aliased mode
  inline int kzminDealiased() const; // |kx| > kxmaxDealiased is aliased mode
  inline int kzmaxDealiased() const;
  inline bool isAliased(int kx, int kz) const;


  inline Real Lx() const;
  inline Real Ly() const;
  inline Real Lz() const;
  inline Real a() const;
  inline Real b() const;
  inline Real x(int nx) const;  // the x coord of the nxth gridpoint
  inline Real y(int ny) const;
  inline Real z(int nz) const;

  Vector xgridpts() const;
  Vector ygridpts() const;
  Vector zgridpts() const;

  Complex Dx(int mx) const;        // spectral diff operator
  Complex Dz(int mz) const;        // spectral diff operator
  Complex Dx(int mx, int n) const; // spectral diff operator
  Complex Dz(int mz, int n) const; // spectral diff operator

  FlowField& operator *=(Real x);
  FlowField& operator *=(Complex x);
  FlowField& operator +=(const ChebyCoeff& U);        //  i.e. u(0,*,0,0) += U
  FlowField& operator -=(const ChebyCoeff& U);        //       u(0,*,0,0) -= U
  FlowField& operator +=(const ComplexChebyCoeff& U); // u.cmplx(0,*,0,0) += U
  FlowField& operator -=(const ComplexChebyCoeff& U); // u.cmplx(0,*,0,0) += U
  FlowField& operator +=(const BasisFunc& U);
  FlowField& operator -=(const BasisFunc& U);
  FlowField& operator +=(const RealProfile& U);
  FlowField& operator -=(const RealProfile& U);
  FlowField& operator +=(const FlowField& u);
  FlowField& operator -=(const FlowField& u);
  FlowField& operator +=(const RealProfileNG& U);
  FlowField& operator -=(const RealProfileNG& U);
  //FlowField& operator *=(const FlowField& u);

  bool geomCongruent(const FlowField& f) const;
  bool congruent(const FlowField& f) const;
  bool congruent(const BasisFunc& phi) const;
  //  bool congruent(const RealProfile& phi) const;
  bool congruent(const RealProfileNG& e) const;
  friend void swap(FlowField& f, FlowField& g); // exchange data of two congruent fields.

  // save methods add extension .asc or .ff ("flow field")
  void asciiSave(const std::string& filebase) const;
  void binarySave(const std::string& filebase) const;
  void hdf5Save(const std::string& filebase) const;

  // save to .h5 or .ff based on file extension, or if none, presence of HDF5 libs
  void save(const std::string& filebase) const;


  // save k-normal slice of ith component at nkth gridpoint (along k-direction)
  void saveSlice(int k, int i, int nk, const std::string& filebase,
		 int xstride=1, int ystride=1, int zstride=1) const;
  void saveProfile(int mx, int mz, const std::string& filebase) const;
  void saveProfile(int mx, int mz, const std::string& filebase,
		   const ChebyTransform& t) const;

  // save L2Norm(u(kx,kz)) to file, kxorder => order in kx,kz, drop last mode
  void saveSpectrum(const std::string& filebase, int i, int ny=-1,
		    bool kxorder=true, bool showpadding=false) const;
  void saveSpectrum(const std::string& filebase, bool kxorder=true,
		    bool showpadding=false) const;
  void saveDivSpectrum(const std::string& filebase, bool kxorder=true,
		       bool showpadding=false) const;

  void print() const;
  void dump() const;

  Real energy(bool normalize=true) const;
  Real energy(int mx, int mz, bool normalize=true) const;
  Real dudy_a() const;
  Real dudy_b() const;
  Real CFLfactor() const;
  Real CFLfactor(const ChebyCoeff& Ubase) const;

  void setState(fieldstate xz, fieldstate y);
  void assertState(fieldstate xz, fieldstate y) const;

  inline fieldstate xzstate() const;
  inline fieldstate ystate() const;

  void zeroPaddedModes();    // set padded modes to zero
  void setPadded(bool b);    // turn on padded flag
  bool padded() const;       // true implies that upper 1/3 modes are zero

private:
  int Nx_;     // number of X gridpoints and modes
  int Ny_;     // number of Y gridpoints and modes
  int Nz_;     // number of Z gridpoints
  int Nzpad_;  // 2*(Nz/2+1)
  int Nzpad2_; // number of Z modes == Nz/2+1.
  int Nd_;

  Real Lx_;
  Real Lz_;
  Real a_;
  Real b_;

  bool padded_;    // flag, simplifies IO and norm calcs on dealiased fields

  Real* rdata_;    // stored with indices in order d, Ny, Nx, Nz
  Complex* cdata_; // Complex alias for rdata_ (cdata_ = (Complex*) rdata_).
  Real* scratch_;  // scratch space for y transforms.

  fieldstate xzstate_;
  fieldstate ystate_;

  fftw_plan xz_plan_;
  fftw_plan xz_iplan_;
  fftw_plan   y_plan_;
  //fftw_flags fftw_flags_;
  //fftw_plan  y_iplan_;

  inline int flatten(int nx, int ny, int nz, int i) const;
  inline int complex_flatten(int mx, int my, int mz, int i) const;
  inline int flatten(int nx, int ny, int nz, int i, int j) const;
  inline int complex_flatten(int mx, int my, int mz, int i, int j) const;
  void fftw_initialize(uint fftw_flags = FFTW_ESTIMATE);
};

void normalize(array<FlowField>& e);
void orthogonalize(array<FlowField>& e);
void orthonormalize(array<FlowField>& e);

// Quadratic interpolation/expolate of FlowField as function of parameter mu.
// Input arrays are length 3: un[0],un[1],un[2] at values mun[0], mun[1], mun[2].
// At any gridpoint, if difference is less than eps, use un[0] instead of interpolating
FlowField quadraticInterpolate(array<FlowField>& un, array<Real>& mun, Real mu, Real eps=1e-13);
FlowField polynomialInterpolate(array<FlowField>& un, array<Real>& mun, Real mu);


// Vector-valued access methods
inline int FlowField::flatten(int nx, int ny, int nz, int i) const {
  assert(nx>=0 && nx<Nx_);    assert(ny>=0 && ny<Ny_);
  assert(nz>=0 && nz<Nzpad_); assert(i>=0  && i<Nd_);
  return nz + Nzpad_*(nx + Nx_*(ny + Ny_*i));
}

inline int FlowField::complex_flatten(int mx, int my, int mz, int i) const {
  assert(mx>=0 && mx<Nx_);     assert(my>=0 && my<Ny_);
  assert(mz>=0 && mz<Nzpad2_); assert(i>=0  && i<Nd_);
  return (mz + Nzpad2_*(mx + Nx_*(my + Ny_*i)));
}

inline const Real& FlowField::operator()(int nx, int ny, int nz, int i) const {
  assert(xzstate_==Physical);
  return rdata_[flatten(nx,ny,nz,i)];
}
inline Real& FlowField::operator()(int nx, int ny, int nz, int i) {
  assert(xzstate_==Physical);
  return rdata_[flatten(nx,ny,nz,i)];
}
inline Complex& FlowField::cmplx(int mx, int my, int mz, int i) {
  assert(xzstate_==Spectral);
  return cdata_[complex_flatten(mx,my,mz,i)];
}
inline const Complex& FlowField::cmplx(int mx, int my, int mz, int i) const{
  assert(xzstate_==Spectral);
  return cdata_[complex_flatten(mx,my,mz,i)];
}

// Tensor-valued access methods
inline int FlowField::flatten(int nx, int ny, int nz, int i, int j) const {
  assert(nx>=0 && nx<Nx_);    assert(ny>=0 && ny<Ny_);
  assert(nz>=0 && nz<Nzpad_); assert(i>=0  && i<Nd_); assert(j>=0  && j<Nd_);
  return nz + Nzpad_*(nx + Nx_*(ny + Ny_*(i + Nd_*j)));
}

inline int FlowField::complex_flatten(int mx, int my, int mz, int i, int j) const {
  assert(mx>=0 && mx<Nx_);     assert(my>=0 && my<Ny_);
  assert(mz>=0 && mz<Nzpad2_); assert(i>=0  && i<Nd_);
  return (mz + Nzpad2_*(mx + Nx_*(my + Ny_*(i + Nd_*j))));
}

inline const Real& FlowField::operator()(int nx, int ny, int nz, int i, int j) const {
  assert(xzstate_==Physical);
  return rdata_[flatten(nx,ny,nz,i,j)];
}
inline Real& FlowField::operator()(int nx, int ny, int nz, int i, int j) {
  assert(xzstate_==Physical);
  return rdata_[flatten(nx,ny,nz,i,j)];
}
inline Complex& FlowField::cmplx(int mx, int my, int mz, int i, int j) {
  assert(xzstate_==Spectral);
  return cdata_[complex_flatten(mx,my,mz,i,j)];
}
inline const Complex& FlowField::cmplx(int mx, int my, int mz, int i, int j) const{
  assert(xzstate_==Spectral);
  return cdata_[complex_flatten(mx,my,mz,i,j)];
}

//inline Real& FlowField::rawData(int n) {return rdata_[n];}
//inline const Real& FlowField::rawData(int n) const{return rdata_[n];}
//inline int FlowField::rawDataLength() const {return Nx_*Ny_*Nz_*Nd_;}

inline Real FlowField::Lx() const {return Lx_;}
inline Real FlowField::Ly() const {return b_ - a_;}
inline Real FlowField::Lz() const {return Lz_;}
inline Real FlowField::a() const {return a_;}
inline Real FlowField::b() const {return b_;}

inline int FlowField::kx(int mx) const {
  //assert(xzstate_ == Spectral);
  assert(mx >= 0 && mx <Nx_);
  return (mx <= Nx_/2) ? mx : mx - Nx_;
}

inline int FlowField::kz(int mz) const {
  //assert(zstate_ == Spectral);
  assert(mz>= 0 && mz <Nzpad2_);
  return mz;
}

inline int FlowField::mx(int kx) const {
  //assert(xzstate_ == Spectral);
  assert(kx >= 1-Nx_/2 && kx <= Nx_/2);
  return (kx >= 0) ? kx : kx + Nx_;
}

inline int FlowField::mz(int kz) const {
  //assert(zstate_ == Spectral);
  assert(kz>= 0 && kz <Nzpad2_);
  return kz;
}

inline Real FlowField::x(int nx) const {
  //assert(xzstate_ == Physical);
  return nx*Lx_/Nx_;
}
inline Real FlowField::y(int ny) const {
  //assert(ystate_ == Physical);
  return 0.5*((b_+a_) + (b_-a_)*cos(pi*ny/(Ny_-1)));
}
inline Real FlowField::z(int nz) const {
  //assert(zstate_ == Physical);
  return nz*Lz_/Nz_;
}

inline int FlowField::numXmodes() const {return Nx_;}
inline int FlowField::numYmodes() const {return Ny_; }
inline int FlowField::numZmodes() const {return Nzpad2_;} // Nzpad2 = Nz/2+1

inline int FlowField::numXgridpts() const {return Nx_;}
inline int FlowField::numYgridpts() const {return Ny_; }
inline int FlowField::numZgridpts() const {return Nz_;}

inline int FlowField::Nx() const {return Nx_;}
inline int FlowField::Ny() const {return Ny_;}
inline int FlowField::Nz() const {return Nz_;}

inline int FlowField::Mx() const {return Nx_;}
inline int FlowField::My() const {return Ny_;}
inline int FlowField::Mz() const {return Nzpad2_;}

inline int FlowField::kxmax() const {return Nx_/2;}
inline int FlowField::kzmax() const {return Nz_/2;}
inline int FlowField::kxmin() const {return Nx_/2+1-Nx_;}
inline int FlowField::kzmin() const {return 0;}
inline int FlowField::kxmaxDealiased() const {return Nx_/3-1;}  // CHQZ06 p139
inline int FlowField::kzmaxDealiased() const {return Nz_/3-1;}  // CHQZ06 p139
inline int FlowField::kxminDealiased() const {return -(Nx_/3-1);}
inline int FlowField::kzminDealiased() const {return 0;}
inline bool FlowField::isAliased(int kx, int kz) const {
  return (abs(kx) > kxmaxDealiased() || abs(kz) > kzmaxDealiased()) ? true : false;
}
inline int FlowField::vectorDim() const {return Nd_;}
inline int FlowField::Nd() const {return Nd_;}

inline fieldstate FlowField::xzstate() const {return xzstate_;}
inline fieldstate FlowField::ystate() const {return ystate_;}

// helper func for zeroing highest-order mode under odd differentiation
// See Trefethen Spectral Methods in Matlab pg 19.
inline int zero_last_mode(int k, int kmax, int n) {
  return ((k == kmax) && (n%2 == 1)) ? 0 : 1;
}

} //namespace channelflow
#endif


/* flowfield.h: Class for N-dim Fourier x Chebyshev x Fourier expansions
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
