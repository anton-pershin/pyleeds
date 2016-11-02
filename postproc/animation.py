import matplotlib
import matplotlib.pyplot as plt
import matplotlib.animation as anim
import matplotlib.gridspec as gridspec
from matplotlib.lines import Line2D
import types
from postproc.field import *

class QuickAnimation(anim.TimedAnimation):
    def __init__(self, path, time_units):
        self.path = path
        self.time_units = time_units
        self._drawn_L2_norms_u = []
        self._drawn_L2_norms_v = []
        self._grid_rows = 0
        self._grid_cols = 0

        anim.TimedAnimation.__init__(self, fig, interval=50, blit=False)

    def add_contour_animation(self, fields_2d, x, y, title, x_label, y_label):
        self._contours.append(ContourAnimationData(fields_2d, x, y, title, x_label, y_label))

    def add_plot_animation(self, x, y, title, x_label, y_label):
        self._plots.append(PlotAnimationData(x, y, title, x_label, y_label))

    def set_layout(self, rows, cols):
        self._grid_rows = rows
        self._grid_cols = cols

    def _draw_frame(self, framedata):
        time = framedata
        axes = self._fig.get_axes()
        for c in range(len(self._contours)):
            contours_data = self._contours[c]
            x, y = np.meshgrid(contours_data.x, contours_data.y, indexing='ij')
            axes[c].contourf(x, y, contours_data.fields[time], 50)
            axes[c].set_title(contours_data.title + ', T = ' + str(time))
    
        for p in range(len(self._plots)):
            self._plots[p].line.set_data(self._plots[p].x[:time], self._plots[p].y[:time])

        print('T = ' + str(time) + ' is drawn')

    def new_frame_seq(self):
        return iter(self.time_units)

    def _init_draw(self):
        for p in range(len(self._plots)):
            self._plots[p].line.set_data([], [])

        # TODO: figsize must be specified
        fig = plt.figure(figsize=(20,10))
        gs = gridspec.GridSpec(_grid_rows, _grid_cols)
        if len(self._contours) + len(self._plots) > self._grid_rows * self._grid_cols:
            raise BadAnimationLayout('The number of contours and plots is not consistent with the grid specification.')

        # TODO: join into one loop
        # at this moment, contours are on the left and plots are on the right
        time_units_number = 0
        if self._contours is not []:
            time_units_number = len(self._contours[0].fields)
        elif self._plots is not []:
            time_units_number = len(self._plots[0].x)
        else:
            raise EmptyAnimation('Neither contours nor plots are set')

        curr_row = 0
        curr_col = 0
        for c in range(len(self._contours)):
            if time_units_number != len(self._contours[c].fields):
                raise NotConsistentTimeUnits('Number of time units in ' + str(c) + 'th contour does not match with check value (1st contour/plot)')
            self._put_contour_into_figure(fig, gs, curr_row, curr_col, self._contours[c])
            curr_row += 1
            if curr_row == _grid_rows:
                curr_col += 1
                curr_row = 0

        for p in range(len(self._plots)):
            if time_units_number != len(self._contours[p].x):
                raise NotConsistentTimeUnits('Number of time units in ' + str(c) + 'th contour does not match with check value (1st contour/plot)')
            self._put_plot_into_figure(fig, gs, curr_row, curr_col, self._plots[p])
            curr_row += 1
            if curr_row == _grid_rows:
                curr_col += 1
                curr_row = 0

        fig.tight_layout()

    def _put_contour_into_figure(fig, grid_spec, row, col, contours_data):
        ax = fig.add_subplot(grid_spec[row, col])
        ax.set_xlabel(contours_data.x_label)
        ax.set_ylabel(contours_data.y_label)

    def _put_plot_into_figure(fig, grid_spec, row, col, plot_data):
        ax = fig.add_subplot(grid_spec[row, col])
        line = Line2D([], [], color='black', linewidth=2)
        plot_data.set_line(line)
        ax.add_line(line)
        ax.set_xlim(min(plot_data.x), max(plot_data.x))
        ax.set_ylim(min(plot_data.y), max(plot_data.y))
        ax.set_aspect('equal')
        ax.set_xlabel(plot_data.x_label)
        ax.set_ylabel(plot_data.y_label)
        ax.set_title(plot_data.title)

class BadAnimationLayout(Exception):
    pass

class NotConsistentTimeUnits(Exception):
    pass

class EmptyAnimation(Exception):
    pass

class ContourAnimationData:
    def __init__(self, fields_2d, x, y, title, x_label, y_label):
        self.fields = fields_2d.append(fields_2d)
        self.coords = (x, y)
        self.title = title
        self.x_label = x_label
        self.y_label = y_label

class PlotAnimationData:
    def __init__(self, x, y, title, x_label, y_label):
        self.x = x
        self.y = y
        self.title = title
        self.x_label = x_label
        self.y_label = y_label

    def set_line(self, line):
        self.line = line    