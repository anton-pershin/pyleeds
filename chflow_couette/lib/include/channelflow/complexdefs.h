// complexdefs.h: some small mathematical conveniences for complex numbers
// channelflow-1.3 www.channelflow.org
// copyright (C) 2001-2009 John F. Gibson, license declaration at end of file

#ifndef CHANNELFLOW_COMPLEXDEFS_H
#define CHANNELFLOW_COMPLEXDEFS_H

#include "channelflow/mathdefs.h"

// These functions are isolated in their own header file so they can be
// easily excluded, to avoid name clashes when working with other libraries
// (like Octave).

namespace channelflow {

inline Complex exp (const Complex& z) {
  return std::exp(Re(z)) * Complex(cos(Im(z)), sin(Im(z)));
}
inline Complex log (const Complex& z) {
  return Complex(std::log(abs(z)), arg(z));
}

}
// Use arg(z0 and abs(z) from <complex> rather than home-grown funcs
//inline Real norm (const Complex& z) {
//return pythag(Re(z), Im(z));
//}
//inline Real phase (const Complex& z) {
//  return atan2(Im(z), Re(z));
//}

#endif


/* complexdefs.h: some small mathematical conveniences for complex numbers
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
