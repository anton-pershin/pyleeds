// diffops.h: Differential operators for FlowFields
// channelflow-1.3 www.channelflow.org
// copyright (C) 2001-2009 John F. Gibson, license declaration at end of file

#ifndef CHANNELFLOW_DIFFOPS_H
#define CHANNELFLOW_DIFFOPS_H
#include <vector>
#include "channelflow/mathdefs.h"
#include "channelflow/vector.h"
#include "channelflow/chebyshev.h"
#include "channelflow/flowfield.h"


namespace channelflow {

// innerProduct(f,g) == Int f g*  dx dy dz   (/(Lx Ly Lz) if normalize==true)
// L2Norm2(f)  == Int ||f||^2  dx dy dz      (/(Lx Ly Lz) if normalize==true)
// bcNorm2(f)  == Int ||f||^2  dx dz at y=a,b   (/(Lx Lz) if normalize==true)
// divNorm2(f) == L2Norm2(div(u))
//
// L2Norm(f)    == sqrt(L2Norm2(f))
// bcNorm(f)    == sqrt(bcNorm2(f))
// L2Dist(f,g)  == sqrt(L2Dist2(f,g))
// L2Dist2(f,g) == L2Norm2(f-g)
// etc.

Real L1Norm(const FlowField& f, bool normalize=true);
Real L1Dist(const FlowField& f, const FlowField& g, bool normalize=true);
Real LinfNorm(const FlowField& f);
Real LinfDist(const FlowField& f, const FlowField& g);

Real L2Norm (const FlowField& f, bool normalize=true);
Real L2Norm2(const FlowField& f, bool normalize=true);
Real L2Dist (const FlowField& f, const FlowField& g, bool normalize=true);
Real L2Dist2(const FlowField& f, const FlowField& g, bool normalize=true);

Real chebyNorm (const FlowField& f, bool normalize=true);
Real chebyNorm2(const FlowField& f, bool normalize=true);
Real chebyDist (const FlowField& f, const FlowField& g, bool normalize=true);
Real chebyDist2(const FlowField& f, const FlowField& g, bool normalize=true);

Real bcNorm (const FlowField& f, bool normalize=true);
Real bcNorm2(const FlowField& f, bool normalize=true);
Real bcDist (const FlowField& f, const FlowField& g, bool normalize=true);
Real bcDist2(const FlowField& f, const FlowField& g, bool normalize=true);

Real divNorm (const FlowField& f, bool normalize=true);
Real divNorm2(const FlowField& f, bool normalize=true);
Real divDist (const FlowField& f, const FlowField& g, bool normalize=true);
Real divDist2(const FlowField& f, const FlowField& g, bool normalize=true);

// Restrict sums in norm computation to |kx|<=kxmax, |kz|<=kzmax,
Real L2Norm (const FlowField& f, int kxmax, int kzmax, bool normalize=true);
Real L2Norm2(const FlowField& f, int kxmax, int kzmax, bool normalize=true);
Real L2Dist (const FlowField& f, const FlowField& g, int kxmax, int kzmax,
	     bool normalize=true);
Real L2Dist2(const FlowField& f, const FlowField& g, int kxmax, int kzmax,
	     bool normalize=true);
Real L2InnerProduct(const FlowField& f, const FlowField& g,
		    int kxmax, int kzmax, bool normalize=true);

//Vector  vectorL1Dist(const FlowField& f, const FlowField& v);
//Complex innerProduct(const FlowField& f, const FlowField& v);
Real L2InnerProduct(const FlowField& f, const FlowField& g,
		    bool normalize=true);

Real dissipation(const FlowField& f);
Real forcing(const FlowField& f);

// ====================================================================
// Note: these were previously defaulted to normalize=false 2006-03-02
Complex L2InnerProduct(const FlowField& f, const BasisFunc& phi,
		       bool normalize=true);
//UNIMPLEMENTED
//Complex L2InnerProduct(const FlowField& f, const ComplexChebyCoeff& phi,
//		       bool normalize=true);

Real L2InnerProduct(const FlowField& f, const RealProfile& phi,
		    bool normalize=true);

inline Real L2IP(const FlowField& f, const FlowField& g, bool normalize=true) {
  return L2InnerProduct(f,g,normalize);
}

inline Real L2IP(const FlowField& f, const FlowField& g, int kxmax, int kzmax,
		 bool normalize=true) {
  return L2InnerProduct(f,g,kxmax, kzmax,normalize);
}

inline Complex L2IP(const FlowField& f, const BasisFunc& phi,
		    bool normalize=true) {
  return L2InnerProduct(f,phi,normalize);
}

//UNIMPLEMENTED
//inline Complex L2IP(const FlowField& f, const ComplexChebyCoeff& phi,
//		    bool normalize=true) {
//  return L2InnerProduct(f,phi,normalize);
//}

inline Real L2IP(const FlowField& f, const RealProfile& phi,
		 bool normalize=true) {
  return L2InnerProduct(f,phi,normalize);
}



// ====================================================================
Real L2InnerProduct(const RealProfileNG& e, const FlowField &u,bool normalize=true);

inline Real L2InnerProduct(const FlowField &u, const RealProfileNG& e, bool normalize=true)
{
  return L2InnerProduct(e, u, normalize);
}

inline Real L2IP(const RealProfileNG& e, const FlowField &u,bool normalize=true) { return L2InnerProduct(e,u,normalize); }
inline Real L2IP(const FlowField &u, const RealProfileNG& e, bool normalize=true) { return L2InnerProduct(e,u,normalize); }

// The following functions assume an orthonormal basis.
void field2coeff(const std::vector<RealProfile>& basis, const FlowField& u, array<Real>& a);
void coeff2field(const std::vector<RealProfile>& basis, const array<Real>& a, FlowField& u);

void field2coeff(const std::vector<BasisFunc>& basis, const FlowField& u, array<Complex>& a);
void coeff2field(const std::vector<BasisFunc>& basis, const array<Complex>& a, FlowField& u);

void field2coeff(const std::vector<RealProfileNG>& basis, const FlowField& u, std::vector<Real>& a);
void coeff2field(const std::vector<RealProfileNG>& basis, const std::vector<Real>& a, FlowField& u);

void swap(FlowField& f, FlowField& v); // exchange data of two flow fields.

// In general, these fucntions take input fields in any state and return
// output fields in Spectral,Spectral leaving inputs unchanged.
// But it's most efficient to send input fields as Spectral,Spectral.
// Input fields are logically constant, but they may be modified and
// unmodified (transforms and inverses) during the course of the computation.

//  lapl(f,laplf) computes laplf(nx,ny,nz,i) = d^2 f(nx,ny,nz,i)/dx_j^2
//  grad(f,gradf) computes
//                gradf(nx,ny,nz,i)        = df(nx,ny,nz,0)/dx_i for 1d f
//                gradf(nx,ny,nz,i3j(i,j)) = df(nx,ny,nz,i)/dx_j for 3d f


// Warning: these functions are convenient but inefficient due to
// construction and copying of returned FlowFields. Prefer the
// void-returning versions below for repeated calculations.
FlowField xdiff(const FlowField& f, int n=1);   // d^nf/dx^n
FlowField ydiff(const FlowField& f, int n=1);   // d^nf/dy^n
FlowField zdiff(const FlowField& f, int n=1);   // d^nf/dz^n
FlowField diff(const FlowField& f, int i, int n);           // d^nf/dx[i]^n
FlowField diff(const FlowField& f, int nx, int ny, int nz); // d^(nx+ny+nz)/(dx^nx dy^ny dz^nz) f

FlowField grad(const FlowField& f);   // grad f
FlowField lapl(const FlowField& f);   // lapl f
FlowField curl(const FlowField& f);   // del cross f
FlowField curl(const FlowField& f);   // del cross f
FlowField norm(const FlowField& f);   // ||f||
FlowField norm2(const FlowField& f);  // ||f||^2
FlowField div(const FlowField& f);    // del dot f

FlowField cross(const FlowField& f, const FlowField& g);
FlowField outer(const FlowField& f, const FlowField& g);//fi gj ei ej
FlowField dot(const FlowField& f, const FlowField& g);
FlowField energy(const FlowField& u);
FlowField energy(const FlowField& u, ChebyCoeff& U);


// These functions are preferred
void xdiff(const FlowField& f, FlowField& dfdx, int n=1);
void ydiff(const FlowField& f, FlowField& dfdy, int n=1);
void zdiff(const FlowField& f, FlowField& dfdz, int n=1);
void diff(const FlowField& f, FlowField& df, int i, int n);
void diff(const FlowField& f, FlowField& df, int nx, int ny, int nz);

void ydiffOld(const FlowField& f, FlowField& dfdy, int n=1);
void ydiffOnce(const FlowField& f, FlowField& dfdy);

void grad(const FlowField& f, FlowField& grad_f);   // grad f, 1d->3d or 3d->9d
void lapl(const FlowField& f, FlowField& lapl_f);   // lapl f
void curl(const FlowField& f, FlowField& curl_f);   // del cross f
void norm(const FlowField& f, FlowField& norm_f);   // ||f||
void norm2(const FlowField& f, FlowField& norm2_f); // ||f||^2
void div(const FlowField& f,  FlowField& divf);     // del dot f

void cross(const FlowField& f, const FlowField& g, FlowField& f_cross_g);
void outer(const FlowField& f, const FlowField& g, FlowField& fg);//fi gj ei ej
void dot(const FlowField& f, const FlowField& g, FlowField& f_dot_g);
void energy(const FlowField& u, FlowField& e);
void energy(const FlowField& u, const ChebyCoeff& U, FlowField& e);


// Different ways to calculate nonlinear term of Navier-Stokes equation.
// Let u' = u + U ex
// Rotational:  f = (curl u)  cross u + U du/dx + v dUdy ex, tmp = curl u
// Rotational2: f = (curl u') cross u'                       tmp = curl u'
// Convection:  f = u' dot grad u',                          tmp = grad u'
// Divergence:  f = div (u' u'),                             tmp = u' u'
// Skewsymm  :  f = 1/2 [u' dot (grad u') + div (u' u')],    tmp = grad u'
// Alternating:  alternate btwn divergence and convection
// Alternating_: alternate btwn convection and divergence
// Linearized:  f = u dot grad (U ex) + (U ex) dot grad u
//                = v dU/dy ex + U du/dx
//
// LinearizedAboutField:
//            utot = u + U = (u0+U) + (u-u0) = ubase + du
//              f = (curl ubase) cross ubase
//                  (curl ubase) cross du
//                  (curl du)    cross ubase
//                = (curl utot) cross (utot) - (curl du) cross (du)

// Note: these functions return f in Spectral, Spectral and u,U in entry states
// The functions are most efficient if u,U enter in Spectral, Spectral.
// If you want dealiased nonlinearities, call u.setAliasedModesToZero()
// beforehand and possibly f.setAliasedModesToZero() afterwards.

// Rotational f = (curl u) x u
// Convection f = u dot grad u
// Divergence f = div (u u)
// Skew-Symm  f = 1/2 [u dot grad u + div (u u)]
// Linearized f = dU/dy v ex + du/dx U
// Linearized f = (curl ubase)   x u + (curl u) x  ubase

void    rotationalNL(const FlowField& u, FlowField& f, FlowField& tmp);
void    convectionNL(const FlowField& u, FlowField& f, FlowField& tmp);
void    divergenceNL(const FlowField& u, FlowField& f, FlowField& tmp);
void skewsymmetricNL(const FlowField& u, FlowField& f, FlowField& tmp);
void    linearizedNL(const FlowField& u, const ChebyCoeff& U, FlowField& f);


/****************************************************************
void  linearAboutFieldNL(const FlowField& u, const FlowField& ubase,
			 const FlowField& ubtot, const FlowField& grad_ubtot,
			 FlowField& f, FlowField& tmp);
*******************************************/

void    linearAboutFieldNL(const FlowField& u, const FlowField& ubase,
			   const ChebyCoeff& U, FlowField& f,
			   FlowField& tmp, FlowField& tmp2);


void linearizedNL(const FlowField& u_, const FlowField& ubtot,
		  const FlowField& grad_ubtot, FlowField& f, FlowField& tmp);


//Unimplemented
//void matlabSave(FlowField& f, int i, std::string& filename);

void assignOrrSommField(FlowField& u, FlowField& P,
			Real t, Real Reynolds, Complex omega,
			const ComplexChebyCoeff& ueig,
			const ComplexChebyCoeff& veig,
			const ComplexChebyCoeff& peig);

void  dotgrad(const FlowField& u, const FlowField& v, FlowField& u_dotgrad_v,
	      FlowField& tmp);

FlowField dotgrad(const FlowField& u, const FlowField& v, FlowField& tmp);


void randomUprofile(ComplexChebyCoeff& u, Real mag, Real spectralDecay);
void randomVprofile(ComplexChebyCoeff& v, Real mag, Real spectralDecay);
void randomProfile(ComplexChebyCoeff& u, ComplexChebyCoeff& v, ComplexChebyCoeff& w,
		   int kx, int kz, Real Lx, Real Lz, Real mag, Real spectralDecay);

void chebyUprofile(ComplexChebyCoeff& u, int n, Real decay);
void chebyVprofile(ComplexChebyCoeff& v, int n, Real decay);
void chebyProfile(ComplexChebyCoeff& u, ComplexChebyCoeff& v, ComplexChebyCoeff& w,
		  int un, int vn, int kx, int kz, Real Lx, Real Lz, Real decay);


FlowField extractRolls(const FlowField &u);

}
#endif


/* diffops.h: Differential operators for FlowFields
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
