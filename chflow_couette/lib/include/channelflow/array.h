/* array.h: a templated 1d array class.
 * Channelflow-1.0
 *
 * Copyright (C) 2001-7  John F. Gibson  
 *  
 * Center for Nonlinear Science
 * School of Physics
 * Georgia Institute of Technology
 * Atlanta, GA 30332-0430
 * 404 385 2509 
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

#ifndef CHANNELFLOW_ARRAY_H
#define CHANNELFLOW_ARRAY_H

#include <assert.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

//#include "string.h"

#include "channelflow/mathdefs.h" // needed for binary IO functions

namespace channelflow {

typedef double Real;
const int REAL_OUTPUT_DIGITS = 17;

template <class T> 
class array {
public:
  array(int N=0);
  array(int N, const T& t);
  array(const array& a);
  //array(std::string& filename);
  ~array();

  bool operator==(const array& a);
  bool operator!=(const array& a);

  void resize(int N);
  void fill(const T& t);

  array& operator=(const array& a);
  inline T& operator[](int i);
  inline const T& operator[](int i) const;

  array subvector(int offset, int N) const;
  
  int N() const;
  int length() const;
  const T* pointer() const; // Efficiency overrules safety in thesis code.
  T* pointer();            

  // save and string& ctor form ascii io pair
  // read and write      form binary io pair
  void save(const std::string& filename) const; // inverse of array(file)
  void binaryDump(std::ostream& os) const;  
  void binaryLoad(std::istream& is);  

private:
  T* data_;
  int N_;
};

template <class T> std::ostream& operator<<(std::ostream&, const array<T>& a);

template <class T> 
inline T& array<T>::operator[](int i) {
  assert(i>=0 && i<N_);
  return data_[i];
}

template <class T> 
inline const T& array<T>::operator[](int i) const {
  assert(i>=0 && i<N_);
  return data_[i];
}

template <class T> 
array<T>::array(int N) :
  data_(new T[N]),
  N_(N)
{
  assert(data_ != 0);
  //for (int i=0; i<N_; ++i)
  //data_[i] = T();
}

template <class T> 
array<T>::array(int N, const T& t) :
  data_(new T[N]),
  N_(N)
{
  assert(data_ != 0);
  for (int i=0; i<N_; ++i)
    data_[i] = t;
}

template <class T> 
array<T>::array(const array& a) :
  data_(new T[a.N_]),
  N_(a.N_)
{
  assert(data_ != 0);
  T* dptr = data_;
  T* aptr = a.data_;
  for (int i=0; i<N_; ++i)
    *dptr++ = *aptr++;
}

template <class T> 
array<T>::~array() {
  //cout << "Vector dtor " << long(data_) << endl;
  delete[] data_;
  data_=0;
}

template <class T> 
void array<T>::resize(int N) {

  if (N != N_) {
    // Allocate new space
    T* newdata_ = new T[N];
    assert(newdata_ != 0);

    // Copy some/all of old data into new space
    int M = (N<N_) ? N : N_; // lesser of N, N_
    for(int i=0; i<M; ++i)
      newdata_[i] = data_[i];

    // Delete old space, reset pointer to new space, and update size.
    delete[] data_;
    data_ = newdata_;
    N_ = N;
  }
}
template <class T> 
void array<T>::fill(const T& t) {
  for(int i=0; i<N_; ++i)
    data_[i] = t;
}
 

template <class T> 
array<T> array<T>::subvector(int offset, int N) const
{
  array<T> subvec(N);
  for (int i=0; i<N; ++i)
    subvec[i] = data_[i+offset];
  return subvec;
}

template <class T> 
array<T>& array<T>::operator=(const array& a) {
  if (data_ != a.data_) {
    if (N_ != a.N_) {
      //cout << "delete in operator= on " << long(data_) << endl;
      delete[] data_;
      data_ = new T[a.N_];
      N_ = a.N_;
      assert(data_ != 0);
    }  
    T* dptr = data_;
    T* aptr = a.data_;
    for (int i=0; i<N_; ++i)
      *dptr++ = *aptr++;
  }
  return *this;
}

template <class T> 
bool array<T>::operator==(const array& a) {
  if (this == &a)
    return true;
  if (N_ != a.N_)
    return false;
  
  T* dptr = data_;
  T* aptr = a.data_;
  for (int n=0; n<N_; ++n)
    if (*dptr++ != *aptr++)
      return false;
  
  return true;
}

template <class T> 
bool array<T>::operator!=(const array& a) {return !(*this == a);}

template <class T> 
int array<T>::length() const {return N_;}

template <class T> 
int array<T>::N() const {return N_;}


template <class T> 
T* array<T>::pointer() {return data_;}

template <class T> 
const T* array<T>::pointer() const {return data_;}

template <class T> 
void array<T>::save(const std::string& filebase) const {
  std::ofstream os(filebase.c_str());
  os << std::scientific << std::setprecision(17);
    os << "% " << N_ << " 1\n";
  for (int i=0; i<N_; ++i) 
    os << data_[i] << '\n';
  os.close();
}

template <class T> 
std::ostream& operator<<(std::ostream& os, const array<T>& a) {
  int N = a.length();
  char seperator=(N<10) ? ' ' : '\n';
  os << std::setprecision(17);
  for (int i=0; i<N; ++i)
    os << a[i] << seperator;
  return os;
}

template <class T> 
void array<T>::binaryDump(std::ostream& os) const {
  write(os, N_);
  T* d = data_;
  T* end = data_ + N_;
  const int size = sizeof(T);
  while (d < end) 
    os.write((char*) d++, size);
}

template <class T> 
void array<T>::binaryLoad(std::istream& is) {
  int N;
  read(is, N);

  // Change length if necess.
  if (N != N_) {
    delete[] data_;
    data_ = new T[N_ = N];
  }

  // How can this be made to work with endianness and arbitrary types T?
  T* d = data_;
  T* end = data_ + N_;
  const int size = sizeof(T);
  while (d < end) 
    is.read((char*) d++, size); 
}

//typedef array<int> IntArray;
//typedef array<Real> RealArray;

}
#endif
