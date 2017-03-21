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

def plot_lines(x_list, y_list, layout=None, titles=None, labels=None, legend_loc='lower right', elongated=None, ylog=False):
    if layout is None: # assume horizontal
        layout = (1, len(x_list))
    if titles is None:
        titles = [None for _ in x_list]
    has_legend = True if labels is not None else False
    standard_length = 4
    x_length = layout[1] * standard_length
    y_length = layout[0] * standard_length
    if elongated == 'x':
        x_length *= 2
    elif elongated == 'y':
        y_length *= 2

    # We suppose that 10 is maximum vertical length, so descrease the sizes proportionally if it is too large
    if y_length > 10:
        y_length = 10

    fig, axes = plt.subplots(layout[0], layout[1], figsize=(x_length, y_length))

    if len(x_list) == 1:
        if has_legend:
            _plotting_func(axes, ylog)(x_list[0].values, y_list[0].values, label=labels[0])
        else:
            _plotting_func(axes, ylog)(x_list[0].values, y_list[0].values)
        _put_data_on_2d_axes(axes, x_list[0].label, y_list[0].label, has_legend=has_legend, legend_loc=legend_loc)
    else:
        if layout[0] == 1 or layout[1] == 1:
            for i in range(len(x_list)):
                if has_legend:
                    _plotting_func(axes[i], ylog)(x_list[i].values, y_list[i].values, label=labels[i])
                else:
                    _plotting_func(axes[i], ylog)(x_list[i].values, y_list[i].values)
                _put_data_on_2d_axes(axes[i], x_list[i].label, y_list[i].label, title=titles[i], has_legend=has_legend, legend_loc=legend_loc)
        else: 
            for row in range(layout[0]):
                for col in range(layout[1]):
                    i = row + col
                    if has_legend:
                        _plotting_func(axes[row, col], ylog)(x_list[i].values, y_list[i].values, label=labels[i])
                    else:
                        _plotting_func(axes[row, col], ylog)(x_list[i].values, y_list[i].values)
                    _put_data_on_2d_axes(axes[row, col], x_list[i].label, y_list[i].label, title=titles[i], has_legend=has_legend, legend_loc=legend_loc)
    return fig, axes

def plot_lines_on_one_plot(x_list, y_list, labels=[], lines_types=[], ylog=False, elongated=None, linewidth=2, legend_loc='lower right'):
    '''
    elongated keyword should be a string (either 'x' or 'y')
    '''
    standard_length = 4
    x_length = 2*standard_length if elongated == 'x' else standard_length
    y_length = 2*standard_length if elongated == 'y' else standard_length
    fig, ax = plt.subplots(1, 1, figsize=(x_length, y_length))
    has_labels = True if len(labels) != 0 else False
    if len(lines_types) == 0:
        lines_types = ['-' for _ in range(len(x_list))]
    for i in range(len(x_list)):
        if has_labels:
            _plotting_func(ax, ylog)(x_list[i].values, y_list[i].values, lines_types[i], linewidth=linewidth, label=labels[i])
        else:
            _plotting_func(ax, ylog)(x_list[i].values, y_list[i].values, lines_types[i], linewidth=linewidth)
    _put_data_on_2d_axes(ax, x_list[0].label, y_list[0].label, has_legend=has_labels, legend_loc=legend_loc)
    return fig, ax

def quive_field_2d(field_2d):
    coord1 = field_2d.space.elements[0]
    coord2 = field_2d.space.elements[1]
    x, y = np.meshgrid(coord1, coord2, indexing='ij') # ij-indexing guarantees correct order of indexes 
    fig, ax = plt.subplots()
    ax.quiver(x, y, field_2d.elements[0], field_2d.elements[1], length=0.15)
    put_labels_on_axes(ax, space)
    ax.set_aspect('equal')
    return fig, ax

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

    _build_fake_3d_box(ax, field.x, field.y, field.z)
    plt.show()

def _build_fake_3d_box(ax, x, y, z):
    # create cubic bounding box to simulate equal aspect ratio
    max_range = np.array([x.max() - x.min(), y.max() - y.min(), z.max() - z.min()]).max()
    x_box = 0.5*max_range*np.mgrid[-1:2:2,-1:2:2,-1:2:2][0].flatten() + 0.5*(x.max() + x.min())
    y_box = 0.5*max_range*np.mgrid[-1:2:2,-1:2:2,-1:2:2][1].flatten() + 0.5*(y.max() + y.min())
    z_box = 0.5*max_range*np.mgrid[-1:2:2,-1:2:2,-1:2:2][2].flatten() + 0.5*(z.max() + z.min())

    for xb, yb, zb in zip(x_box, y_box, z_box):
        ax.plot([xb], [yb], [zb], 'w')

def _plotting_func(ax, ylog):
    return ax.semilogy if ylog else ax.plot

def _put_labels_on_axes(ax, space):
    ax.set_xlabel(space.elements_names[0])
    ax.set_ylabel(space.elements_names[1])

def _put_data_on_2d_axes(ax, xlabel, ylabel, has_grid=True, has_legend=False, legend_loc='lower right', title=None):
    if title is not None:
        ax.set_title(title)
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    if has_grid:
        ax.grid()
    if has_legend:
        ax.legend(loc=legend_loc, fontsize='x-small')

if __name__ == '__main__':
    from test_fields import get_wave_field
    test_field = get_wave_field()
    #fig, ax = quive_field_2d(field_2d)
    fig, ax = plot_filled_contours(test_field)
    put_labels_on_axes(ax, test_field.space)
    plt.show()