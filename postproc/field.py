from __future__ import division
from math import *
import os
import re
import matplotlib
import matplotlib.pyplot as plt
import h5py
import numpy as np
from scipy.integrate import simps
from auxtools import NamedAttributesContainer, DimensionsDoNotMatch, LabeledValue

class Space(NamedAttributesContainer):
    def __init__(self, coords):
        #self.coords = list(coords)
        NamedAttributesContainer.__init__(self, coords, [])

    def dim(self):
        return len(self.elements)

    #def set_internal_coordinate_names(self, coords_names):
    #    if len(coords_names) != len(self.coords):
    #        raise DimensionsDoNotMatch('Number of coordinate names and dimension do not match')
    #    self._coords_names = coords_names
    #    for i in range(len(self.coords)):
    #        setattr(self, coords_names[i], self.coords[i])

    def set_xyz_naming(self):
        if self.dim() != 3:
            raise DimensionsDoNotMatch('XYZ naming is possible only for 3-dimensional space')

        self.set_elements_names(['x', 'y', 'z'])

    def make_subspace(self, indexes):
        subsontainer = self.make_subcontainer(indexes)
        subspace = Space(subsontainer.elements)
        subspace.set_elements_names(subsontainer.elements_names)
        return subspace

#class Field(object):
#    '''
#    Base class for field representation
#    '''
#    def __init__(self, u, v, w, x, y, z):
#        self.u = u
#        self.v = v
#        self.w = w
#        self.x = x
#        self.y = y
#        self.z = z
#
#    def L2_norms(self, normalize):
#        L2_norm_u = 0.
#        L2_norm_v = 0.
#        L2_norm_w = 0.
#
#        V = 1
#        if normalize:
#            V = abs(self.x[0] - self.x[len(self.x)-1]) * abs(self.y[0] - self.y[len(self.y)-1]) * abs(self.z[0] - self.z[len(self.z)-1])
#            #V = 4*np.pi * 2 * 2*np.pi
#        L2_norm_u = np.sqrt(-integrate_field(np.power(self.u, 2), self.x, self.y, self.z) / V)
#        L2_norm_v = np.sqrt(-integrate_field(np.power(self.v, 2), self.x, self.y, self.z) / V)
#        L2_norm_w = np.sqrt(-integrate_field(np.power(self.w, 2), self.x, self.y, self.z) / V)
#
#        return (L2_norm_u, L2_norm_v, L2_norm_w)

class Field(NamedAttributesContainer):
    '''
    Base class for field representation
    '''
    def __init__(self, elements, space):
        self.space = space
        NamedAttributesContainer.__init__(self, elements, [])

    def set_uvw_naming(self):
        if len(self.elements) != 3:
            raise DimensionsDoNotMatch('UVW naming is possible only for a vector field with 3 elements')

        self.set_elements_names(['u', 'v', 'w'])

    def make_subfield(self, elems):
        indexes = self.convert_names_to_indexes_if_necessary(elems)
        subcontainer = self.make_subcontainer(indexes)
        subfield = Field(subcontainer.elements, self.space)
        subfield.set_elements_names(subcontainer.elements_names)
        return subfield

    def change_order(self, elems):
        indexes = self.space.convert_names_to_indexes_if_necessary(elems)
        self.elements[:] = [np.transpose(elem, indexes) for elem in self.elements]
        NamedAttributesContainer.change_order(self, indexes)
        self.space.change_order(indexes)

    # TODO: must be generalized
    #def filter(self, coord, rule):
    def filter(self, coord, filtering_capacity):
        index = self.space.convert_names_to_indexes_if_necessary([coord])[0]
        spacing = int(1 / filtering_capacity)
        indexes_to_filter = list(range(1, self.space.elements[index].shape[0], spacing))
        filtered_coord_array = np.delete(self.space.elements[index], indexes_to_filter)
        filtered_coords = []
        for i in range(len(self.space.elements)):
            if i == index:
                filtered_coords.append(np.delete(self.space.elements[index], indexes_to_filter))
            else:
                filtered_coords.append(self.space.elements[i])

        filtered_raw_fields = [np.delete(raw_field, indexes_to_filter, axis=index) for raw_field in self.elements]
        filtered_space = Space(filtered_coords)
        filtered_space.set_elements_names(self.space.elements_names)
        filtered_field = Field(filtered_raw_fields, filtered_space)
        filtered_field.set_elements_names(self.elements_names)
        return filtered_field

    def average(self, elems, along):
        indexes = self.convert_names_to_indexes_if_necessary(elems)
        coord_index = self.space.convert_names_to_indexes_if_necessary([along])[0]
        averaged_subfield = self.make_subfield(elems)
        averaged_raw_fields = []
        for raw_field in averaged_subfield.elements:
            averaged_raw_fields.append(np.mean(raw_field, coord_index))

        averaged_subfield.elements = averaged_raw_fields
        all_indexes_expect_coord_index = range(coord_index) + range(coord_index + 1, len(averaged_subfield.space.elements))
        averaged_subfield.space = averaged_subfield.space.make_subspace(all_indexes_expect_coord_index)
        return averaged_subfield

    def L2_norms(self, normalize):
        V = 1
        if normalize:
            for i in range(len(self.space.elements)):
                coord = self.space.elements[i]
                V *= abs(coord[0] - coord[len(coord)-1])

        L2_norms = []
        for i in range(len(self.elements)):
            val = np.sqrt(-integrate_field(np.power(self.u, 2), self.space.x, self.space.y, self.space.z) / V)            
            L2_norms.append(LabeledValue(val, '||' + self.elements_names[i] + '||'))

        return L2_norms

def integrate_field(field, x, y, z):
    Nx = x.shape[0]
    Ny = y.shape[0]
    I_xy = np.zeros((Nx, Ny))
    I = np.zeros(Nx)
    for nx in range(Nx):
        for ny in range(Ny):
            I_xy[nx, ny] = np.trapz(field[nx, ny, :], z)

        I[nx] = np.trapz(I_xy[nx, :], y)

    return np.trapz(I, x)

def read_field(filename):
    f = h5py.File(filename, 'r')
    u_dataset = f['data']['u']
    u_numpy = u_dataset[0,:,:,:]
    v_numpy = u_dataset[1,:,:,:]
    w_numpy = u_dataset[2,:,:,:]

    x_dataset = f['geom']['x']
    y_dataset = f['geom']['y']
    z_dataset = f['geom']['z']
    x_numpy = x_dataset[:]
    y_numpy = y_dataset[:]
    z_numpy = z_dataset[:]
    
    space = Space([x_numpy, y_numpy, z_numpy])
    space.set_xyz_naming()
    field = Field([u_numpy, v_numpy, w_numpy], space)
    field.set_uvw_naming()
    return field

def read_fields(path, file_prefix, file_postfix, start_time = 0, end_time = None):
    files_list = os.listdir(path)
    found_files = []
    if end_time is None:
        end_time = len(files_list) # impossible to have more time units than number of files

    checker = range(start_time, end_time + 1)
    max_time_found = 0
    for file_ in files_list:
        match = re.match(file_prefix + '(?P<time>[0-9]+)' + file_postfix, file_)
        if match is not None:
            time = int(match.group('time'))
            if time >= start_time and time <= end_time:
                if time > max_time_found:
                    max_time_found = time
                checker.remove(time)
                found_files.append(match.string)

    end_time = max_time_found
    if checker != []:
        if checker.index(end_time + 1) != 0:
            raise BadFilesOrder('Time order based on files is broken. Probably, some of the files are missed')

    fields = []
    for t in range(start_time, end_time + 1):
        fields.append(read_field(path + '/' + file_prefix + str(t) + file_postfix))
    
    return fields

class BadFilesOrder(Exception):
    pass