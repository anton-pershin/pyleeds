// utilfuncs.h: an assortment of convenience functions for channelflow/programs
// channelflow-1.3 www.channelflow.org
// copyright (C) 2001-2009 John F. Gibson, license declaration at end of file

#ifndef CHANNELFLOW_UTILFUNCS_H
#define CHANNELFLOW_UTILFUNCS_H


#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <iomanip>
#include <string>
#include <iostream>
#include <vector>

#include "channelflow/mathdefs.h"
#include "channelflow/array.h"
#include "channelflow/flowfield.h"
#include "channelflow/dns.h"

namespace channelflow {

bool fileExists(const std::string& filename);

void save(const std::string& filebase, Real  T, const FieldSymmetry& tau);
void load(const std::string& filebase, Real& T, FieldSymmetry& tau);

// Neville algorithm quadratic inter/extrapolate based on f[0],f[1],f[2] and x[0],x[1],x[2]
Real quadraticInterpolate(const array<Real>& fn, const array<Real>& xn, Real x);
Real linearInterpolate(Real x0, Real f0, Real x1, Real f1, Real x);
bool isconst(array<Real> f, Real eps=1e-13);

// Neville algorithm (N-1)th order inter/extrapolate based on f[0],f[1],...,f[N-1] and x[0],x[1],...
Real polynomialInterpolate(const array<Real>& fn, const array<Real>& xn, Real x);

// secantSearch and bisectSearch:
// Find root of f(x) between a and b to tolerance feps from polynomial
// interpolant of {fn = f(xn)} I.e. rtn x st abs(f(x)) < feps and a <= x <= b.
Real secantSearch(Real a, Real b, array<Real>& fn, const array<Real>& xn,
		  Real feps=1e-14, int maxsteps=50);

Real bisectSearch(Real a, Real b, array<Real>& fn, const array<Real>& xn,
		  Real feps=1e-14, int maxsteps=50);


// Check divergence, dirichlet, normality, and maybe orthogonality of basis
void basischeck(const std::vector<RealProfile>& e, bool orthocheck=false);

// Make directory and set permissions to a+rx
void mkdir(const std::string& dirname);
void rename(const std::string& oldname, const std::string& newname);

// Append trailing / if necess
std::string pathfix(const std::string& path);

// Convert time to string in format convenient for filenames.
// Returns 1 or 1.000 or 1.250 as appropriate. Three digits
std::string t2s(Real t, bool decimals);

// Return false, approx, or true.
std::string fuzzyless(Real x, Real eps);

std::string pwd();

std::string clip(const std::string& filename, const std::string& ext);
std::string stub(const std::string& filename, const std::string& ext);

// Produce plots of various slices, etc.
void plotfield(const FlowField& u_, const std::string& outdir,
	       const std::string& label, int xstride=1, int ystride=1,
	       int zstride=1);

void plotspectra(const FlowField& u_, const std::string& outdir,
		 const std::string& label, bool showpadding=false);

// Produce plots of various slices, etc.
void plotxavg(const FlowField& u_, const std::string& outdir,
	      const std::string& label);

// Return a translation that maximizes the s-symmetry of u.
// (i.e. return tau // that minimizes L2Norm(s(tau(u)) - tau(u))).
FieldSymmetry optimizePhase(const FlowField& u, const FieldSymmetry& s,
			    int Nsteps=10, Real residual=1e-13, Real damp=1.0,
			    bool verbose=true, Real x0=0.0, Real z0=0.0);

// Return |P(u)|^2/|u|^2 where P(u) is projection onto
// sign ==  1 : s-symmetric subspace.
// sign == -1 : s-antisymmetric subspace.
Real PuFraction(const FlowField& u, const FieldSymmetry& s, int sign);


// A simple way to get flexible command-line args for chflow programs
// Scales as N^2 with # args, so don't use for hundreds of args.

void fixDiri(ChebyCoeff& f);
void fixDiriMean(ChebyCoeff& f);
void fixDiriNeum(ChebyCoeff& f);
void fixDiri(ComplexChebyCoeff& f);
void fixDiriMean(ComplexChebyCoeff& f);
void fixDiriNeum(ComplexChebyCoeff& f);

class ArgList {
public:
  typedef std::string str; // reduces line-length below

  ArgList();
  ArgList(int argc, char* argv[], const str& purpose);

  bool helpmode() const;
  bool errormode() const;
  int remaining() const;  // how many args have yet to be parsed

  // return true if the option is in the args list, false if not
  bool getflag(const str& shortopt, const str& longopt, const str& helpstr);

  bool getbool(const str& shortopt, const str& longopt, const str& helpstr);
  int  getint (const str& shortopt, const str& longopt, const str& helpstr);
  Real getreal(const str& shortopt, const str& longopt, const str& helpstr);
  str  getstr (const str& shortopt, const str& longopt, const str& helpstr);
  str  getpath(const str& shortopt, const str& longopt, const str& helpstr);

  bool getbool(const str& shortopt, const str& longopt, bool defalt, const str& helpstr);
  int  getint (const str& shortopt, const str& longopt, int defalt,  const str& helpstr);
  Real getreal(const str& shortopt, const str& longopt, Real defalt, const str& helpstr);
  str  getstr (const str& shortopt, const str& longopt, const str& defalt, const str& helpstr);
  str  getpath(const str& shortopt, const str& longopt, const str& defalt, const str& helpstr);

  // In the following position counts backwards from end of arglist, e.g
  // in "argv[0] arg3 arg2 arg1" the args are numbered as indicated.
  Real getreal(int position, const str& meaning, const str& helpstr);
  str  getpath(int position, const str& meaning, const str& helpstr);
  str  getstr (int position, const str& meaning, const str& helpstr);

  // Some higher-level get functions that parse ArgList and return TimeStep, DNSFlags, etc.
  //TimeStep gettimestep();
  //DNSFlags getdnsflags();
  //GMRESHookstepFlags getsolutionflags();

  void save(const str& outdir) const;  // save command-line to file <argv[0]>.args
  void save() const;  // save command-line to file <argv[0]>.args
  void check();       // check for unrecognized options and arguments

private:
  array<str> args_;
  array<bool> used_;
  bool helpmode_;
  bool errormode_;
  void printhelp(const str& sopt, const str& lopt, const str& type,
		 const str& defalt, const str& helpstr);
  void printhelp(int position, const str& name, const str& helpstr);
};

void field2vector(const FlowField& u, Vector& v);
void vector2field(const Vector& v, FlowField& u);

class FieldSeries {
public:
  FieldSeries();
  FieldSeries(int N); // Interpolate with (N-1)th-order polynomial

  void push(const FlowField& f, Real t);
  void interpolate(FlowField& f, Real t) const;
  bool full() const;

private:
  array<Real> t_;
  array<FlowField> f_;
  int emptiness_;  // init val N, decrement in push(), stacks are full at 0
};

} //namespace channelflow
#endif


/* utilfuncs.h: an assortment of convenience functions for channelflow/programs
 *
 * Channelflow-1.2
 *
 * Copyright (C) 2001-2008  John F. Gibson
 *
 * John F. Gibson
 * Center for Nonlinear Sciences
 * School of Physics
 * Georgia Institute of Technology
 * Atlanta, GA 30332-0430
 *
 * gibson@cns.physics.gatech.edu
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
