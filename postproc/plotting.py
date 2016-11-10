from math import *
import matplotlib
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
from scipy.integrate import simps

def plot_filled_contours(field_2d):
    coord1 = field_2d.space.elements[0]
    coord2 = field_2d.space.elements[1]
    x, y = np.meshgrid(coord1, coord2, indexing='ij') # ij-indexing guarantees correct order of indexes 
                                                      # where the first index correponds to the x-coordinate
    raw_scalar_field = field_2d.elements[0]
    fig, ax = plt.subplots()
    p = ax.contourf(x, y, raw_scalar_field, 100, cmap=matplotlib.cm.jet, vmin=raw_scalar_field.min(), vmax=raw_scalar_field.max(), aspect='auto')
    ax.set_aspect('equal') # to make x,y axes scales correct
    cb = fig.colorbar(p, ax=ax, shrink=0.25) # "shrink" is used to make color bar small enough (it is very long if not used)
    ax.set_xlim(coord1.min(), coord1.max())
    ax.set_ylim(coord2.min(), coord2.max())
    return fig, ax

def plot_lines(x_list, y_list):
    figheight = 4 * len(x_list)
    fig, axes = plt.subplots(1, len(x_list), figsize=(figheight, 4))
    for i in range(len(x_list)):
        axes[i].plot(x_list[i].values, y_list[i].values)
        axes[i].set_xlabel(x_list[i].label)
        axes[i].set_ylabel(y_list[i].label)
    return fig, axes

def quive_plot_field(field):
    fig = plt.figure()
    ax = fig.gca(projection='3d')
    ax.set_aspect('equal')
    x, y, z = np.meshgrid(field.x, field.y, field.z, indexing='ij')
    ax.quiver(x, y, z, field.u, field.v, field.w, length=0.15, color='Tomato')
    ax.set_title('Test it')
    ax.set_xlabel('x')
    ax.set_ylabel('y')
    ax.set_zlabel('z')
    ax.view_init(elev=18, azim=30)
    ax.dist=8

    build_fake_3d_box(ax, field.x, field.y, field.z)
    plt.show()

def build_fake_3d_box(ax, x, y, z):
    # create cubic bounding box to simulate equal aspect ratio
    max_range = np.array([x.max() - x.min(), y.max() - y.min(), z.max() - z.min()]).max()
    x_box = 0.5*max_range*np.mgrid[-1:2:2,-1:2:2,-1:2:2][0].flatten() + 0.5*(x.max() + x.min())
    y_box = 0.5*max_range*np.mgrid[-1:2:2,-1:2:2,-1:2:2][1].flatten() + 0.5*(y.max() + y.min())
    z_box = 0.5*max_range*np.mgrid[-1:2:2,-1:2:2,-1:2:2][2].flatten() + 0.5*(z.max() + z.min())

    for xb, yb, zb in zip(x_box, y_box, z_box):
        ax.plot([xb], [yb], [zb], 'w')