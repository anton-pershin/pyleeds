from __future__ import division
from math import *
import re
import matplotlib
import matplotlib.pyplot as plt
import h5py
import numpy as np
from scipy.integrate import simps

class Field:
    '''
    Base class for field representation
    '''
    def __init__(self, u, v, w, x, y, z):
        self.u = u
        self.v = v
        self.w = w
        self.x = x
        self.y = y
        self.z = z

    def L2_norms(self, normalize):
        L2_norm_u = 0.
        L2_norm_v = 0.
        L2_norm_w = 0.

        V = 1
        if normalize:
            V = abs(self.x[0] - self.x[len(self.x)-1]) * abs(self.y[0] - self.y[len(self.y)-1]) * abs(self.z[0] - self.z[len(self.z)-1])
            #V = 4*np.pi * 2 * 2*np.pi
        L2_norm_u = np.sqrt(-integrate_field(np.power(self.u, 2), self.x, self.y, self.z) / V)
        L2_norm_v = np.sqrt(-integrate_field(np.power(self.v, 2), self.x, self.y, self.z) / V)
        L2_norm_w = np.sqrt(-integrate_field(np.power(self.w, 2), self.x, self.y, self.z) / V)

        return (L2_norm_u, L2_norm_v, L2_norm_w)


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
    
    return Field(u_numpy, v_numpy, w_numpy, x_numpy, y_numpy, z_numpy)

def read_fields(path, file_prefix, file_postfix, start_time = 0, end_time = None):
    files_list = os.listdir(path)
    found_files = []
    if end_time is None:
        end_time = len(files_list) # impossible to have more time units than number of files

    checker = range(start_time, end_time + 1)
    max_time_found = 0
    for file_ in files_list:
        match = re.match('^' + file_prefix + '?P<time>([0-9]+)$' + file_postfix, file_)
        if match is not None:
            time = int(match.group('time'))
            if time >= start_time and time <= end_time:
                if time > max_time_found:
                    max_time_found = time
                checker.remove(time)
                found_files.append(match.string)

    end_time = max_time_found
    if checker is not []:
        if checker.index(end_time + 1) != 0:
            raise BadFilesOrder('Time order based on files is broken. Probably, some of the files are missed')

    fields = []
    for t in range(start_time, end_time + 1):
        fields.append(read_field(path + '/' + file_prefix + str(t) + file_postfix))
    
    return fields

def average_along(scalar_field, coord):
    axis = 0
    if coord is 'x':
        axis = 0
    elif coord is 'y':
        axis = 1
    elif coord is 'z':
        axis = 2

    return np.mean(scalar_field, axis)

class BadFilesOrder(Exception):
    pass