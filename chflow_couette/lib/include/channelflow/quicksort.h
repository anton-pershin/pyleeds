// quicksort.h: classic quicksort algorithm
// channelflow-1.3 www.channelflow.org
// copyright (C) 2001-2009 John F. Gibson, license declaration at end of file

#ifndef QUICKSORT_H
#define QUICKSORT_H

#include <iomanip>
#include <iostream>
#include "channelflow/mathdefs.h"
#include "channelflow/array.h"

// increasing order: if rtn = quicksort(data) then
// data[rtn[i]] <= data[rtn[i+1]] <= ...
namespace channelflow {

template<class T> void swap(T& a, T& b);

// Public functions
template<class T> array<int> quicksort(T* data, int N);
template<class T> array<int> quicksort(const array<T>& data);
template<class T> void checksort(const array<T>& data,const array<int>& index);
template<class T> void reorder(const array<T>& data, const array<T>& index);

// Private functions
template<class T> int get_pivot(const T* data, int* index, int low, int hi);
template<class T> void q_sort(const T* data, int* index, int low, int hi);

//  return the index of the selected pivot value
template<class T> int get_pivot(const T* data, int* index, int low, int hi){
  return ((low + hi) / 2 );
}


template<class T> void swap(T& a, T& b) {
  T c = a;
  a = b;
  b = c;
}


// Quicksort a data range
template<class T> void q_sort(const T* data, int* index, int low, int hi) {
  int pivot_index;             // index in the data set of the pivot
  Real pivot_value;                // the value of the pivot element
  int left, right;

  // select the pivot element and remember its value
  pivot_index = get_pivot(data, index, low, hi);
  pivot_value = data[index[pivot_index]];

  // do the partitioning
  left = low; right = hi;
  do {

    // move left to the right bypassing elements already on the correct side
    while ((left <= hi) && (data[index[left]] < pivot_value))
      left++;

    // move right to the left bypassing elements already on the correct side
    while ((right >= low) && (pivot_value < data[index[right]]))
      right--;

    //  if the pointers are in the correct order then they are pointing to two
    //  items that are on the wrong side of the pivot value, swap them...

    if (left <= right) {
      swap(index[left], index[right]);
      left++;
      right--;
    }

  } while (left <= right);

  // now recurse on both partitions as long as they are large enough
  if (low < right)
    q_sort(data, index, low, right);
  if (left < hi)
    q_sort(data, index, left, hi);

}

template<class T> array<int> quicksort(const array<T>& data) {
  array<int> index(data.length());
  for (int i=0; i<data.length(); ++i)
    index[i] = i;
  q_sort(data.pointer(), index.pointer(), 0, data.length()-1);
  return index;
}
template<class T> array<int> quicksort(T* data, int N) {
  array<int> index(N);
  for (int i=0; i<N; ++i)
    index[i] = i;
  q_sort(data, index.pointer(), 0, N-1);
  return index;
}

template<class T> void checksort(const array<T>& data, const array<int>& index) {
int badcount=0;
  int N=data.length();
  std::cout << "Checking sort...\n";
  for (int i = 1; i<N; ++i) {
    if (data[index[i]] < data[index[i-1]]) {
      std::cout << "N=" << N << " i=" << i <<  " d[i-1]=" << data[index[i-1]] << " d[i]=" << data[index[i-1]] << '\n';
      ++badcount;
    }
  }
  //show_array(data, N);
  if (badcount==0)
    std::cout << "Good sort! Everything's in order.\n\n";
  else
    std::cout << "Bad  sort! Badcount = " << badcount << "\n\n";
}

} // end namespace channelflow

#endif

/* quicksort.h: classic quicksort algorithm
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
 *
 */
