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

    def grab_namings(self, another_field):
        self.space.set_elements_names(another_field.space.elements_names)
        self.set_elements_names(another_field.elements_names)

    def set_uvw_naming(self):
        if len(self.elements) != 3:
            raise DimensionsDoNotMatch('UVW naming is possible only for a vector field with 3 elements')

        self.set_elements_names(['u', 'v', 'w'])

    def make_subfield(self, elems):
        indexes = self.convert_names_to_indexes_if_necessary(elems)
        subcontainer = self.make_subcontainer(indexes)
        space_ = Space(self.space.elements)
        space_.set_elements_names(self.space.elements_names)
        subfield = Field(subcontainer.elements, space_)
        subfield.set_elements_names(subcontainer.elements_names)
        return subfield

    def change_order(self, elems):
        indexes = self.space.convert_names_to_indexes_if_necessary(elems)
        self.elements[:] = [np.transpose(elem, indexes) for elem in self.elements]
        NamedAttributesContainer.change_order(self, indexes)
        self.space.change_order(indexes)
        self.update_attributed_elements()
        self.space.update_attributed_elements()

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
        #filtered_space.set_elements_names(self.space.elements_names)
        filtered_field = Field(filtered_raw_fields, filtered_space)
        #filtered_field.set_elements_names(self.elements_names)
        filtered_field.grab_namings(self)
        return filtered_field

    def map_to_equispaced_mesh(self, details_capacity_list):
        if self.space.dim() != 2:
            raise DimensionsDoNotMatch('Mapping to equispaced mesh is possible only for 2-dimensional space')

        new_coord_arrays = []
        indexes_mappings = [] # i -> mapping for ith coord
                              # mapping for ith coord: equispaced_array_index -> original_array_nearest_left_index
        for orig_coord_array, details_capacity in zip(self.space.elements, details_capacity_list):
            deltas = np.abs(orig_coord_array - np.roll(orig_coord_array, 1))
            max_delta = np.max(deltas[1:])
            min_delta = np.min(deltas[1:])
            equispaced_delta = min_delta + (1 - details_capacity) * (max_delta - min_delta)
            min_value = np.min(orig_coord_array)
            max_value = np.max(orig_coord_array)
            equispaced_number = (np.max(orig_coord_array) - np.min(orig_coord_array)) / equispaced_delta + 1
            equispaced_array = np.linspace(min_value, max_value, equispaced_number)
            new_coord_arrays.append(equispaced_array)
            indexes_mappings.append(map_differently_spaced_arrays(orig_coord_array, equispaced_array))

        # TODO: need to rewrite in matrix form
        new_x = new_coord_arrays[0]
        new_y = new_coord_arrays[1]
        x_indexes_mapping = indexes_mappings[0]
        y_indexes_mapping = indexes_mappings[1]
        new_elements = [np.zeros((new_x.shape[0], new_y.shape[0])) for i in range(len(self.elements))]
        for i in range(len(new_x)):
            x = new_x[i]
            x_l = self.space.elements[0][x_indexes_mapping[i]]
            if i != len(new_x) - 1:
                x_r = self.space.elements[0][x_indexes_mapping[i] + 1]
            for j in range(len(new_y)):
                y = new_y[j]
                y_l = self.space.elements[1][y_indexes_mapping[j]]
                if j != len(new_y) - 1:
                    y_r = self.space.elements[1][y_indexes_mapping[j] + 1]

                for u, new_u in zip(self.elements, new_elements):
                    if i == len(new_x) - 1 and j == len(new_y) - 1: # "corner" of domain
                        new_u[i, j] = u[x_indexes_mapping[i], y_indexes_mapping[j]]
                        continue
                    #print(new_x.shape)
                    #print(new_y.shape)
                    #print('Shape')
                    #print(u.shape)
                    #print('x_indexes_mapping')
                    #print(x_indexes_mapping)
                    #print('y_indexes_mapping')
                    #print(y_indexes_mapping)
                    #print('i = ' + str(i) + ', j = ' + str(j))

                    if i == len(new_x) - 1: # linear interpolation along the y-axis
                        new_u[i, j] = (y_r - y) / (y_r - y_l) * u[x_indexes_mapping[i], y_indexes_mapping[j]] \
                                    + (y - y_l) / (y_r - y_l) * u[x_indexes_mapping[i], y_indexes_mapping[j] + 1]
                    elif j == len(new_y) - 1: # linear interpolation along the x-axis
                        new_u[i, j] = (x_r - x) / (x_r - x_l) * u[x_indexes_mapping[i], y_indexes_mapping[j]] \
                                    + (x - x_l) / (x_r - x_l) * u[x_indexes_mapping[i] + 1, y_indexes_mapping[j]]
                    else: # bilinear interpolation
                        new_u[i, j] = (x_r - x) * (y_r - y) / (x_r - x_l) / (y_r - y_l) * u[x_indexes_mapping[i], y_indexes_mapping[j]] \
                                    + (x_r - x) * (y - y_l) / (x_r - x_l) / (y_r - y_l) * u[x_indexes_mapping[i], y_indexes_mapping[j] + 1] \
                                    + (x - x_l) * (y_r - y) / (x_r - x_l) / (y_r - y_l) * u[x_indexes_mapping[i] + 1, y_indexes_mapping[j]] \
                                    + (x - x_l) * (y - y_l) / (x_r - x_l) / (y_r - y_l) * u[x_indexes_mapping[i] + 1, y_indexes_mapping[j] + 1]

        equispaced_space = Space(new_coord_arrays)
        new_field = Field(new_elements, equispaced_space)
        new_field.grab_namings(self)
        return new_field

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
            val = np.sqrt(integrate_field(np.power(self.u, 2), self.space.x, self.space.y, self.space.z) / V)            
            L2_norms.append(LabeledValue(val, '||' + self.elements_names[i] + '||'))

        return L2_norms

def map_differently_spaced_arrays(orig_coord_array, new_coord_array):
    def search_for_next_left_index(array, start_index, value):
        left_index_ = start_index
        for i in range(start_index, len(array)):
            if array[i] > value:
                left_index_ = i - 1
                break
        return left_index_
    
    left_index = 0
    left_indexes_array = [0 for i in range(new_coord_array.shape[0])]
    # boundaries are also mapped exactly
    left_indexes_array[0] = 0
    left_indexes_array[-1] = len(orig_coord_array) - 1
    for i in range(1, len(new_coord_array) - 1):
        left_indexes_array[i] = search_for_next_left_index(orig_coord_array, left_index, new_coord_array[i])
    return left_indexes_array

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
    
    # Reverse order for the y-coordinate (chflow gives it from 1 to -1 instead of from -1 to 1)
    space = Space([x_numpy, y_numpy[::-1], z_numpy])
    space.set_xyz_naming()
    field = Field([u_numpy[:,::-1,:], v_numpy[:,::-1,:], w_numpy[:,::-1,:]], space)
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

if __name__ == '__main__':
    randomly_spaced_array = np.array([0., 0.1, 0.3, 0.32, 0.33, 0.5, 0.6, 0.62, 0.8, 1.])
    equispaced_array = np.linspace(0., 1., 20)
    mapping = map_differently_spaced_arrays(randomly_spaced_array, equispaced_array)
    correct_mapping = [0, 0, 1, 1, 1, 1, 2, 4, 4, 4, 5, 5, 7, 7, 7, 7, 8, 8, 8, 9]
    print('Randomly spaced array:')
    print(randomly_spaced_array)
    print('Equispaced array:')
    print(equispaced_array)
    print('Calculated mapping:')
    print(mapping)
    print('Correct mapping:')
    print(correct_mapping)
    print('Not matched indexes:')
    print([i for i,j in zip(mapping, correct_mapping) if i != j])

    from test_fields import get_randomly_spaced_wave_field
    from plotting import plot_filled_contours
    import matplotlib.pyplot as plt
    test_field = get_randomly_spaced_wave_field()
    equispaced_field = test_field.map_to_equispaced_mesh((1., 1.))
    plot_filled_contours(equispaced_field)
    #plot_filled_contours(test_field)
    plt.show()