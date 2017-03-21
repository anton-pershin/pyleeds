import matplotlib
import matplotlib.pyplot as plt
import matplotlib.animation as anim
import matplotlib.gridspec as gridspec
from matplotlib.lines import Line2D
import types
from field import *

class BaseQuickAnimation(anim.TimedAnimation):
    def __init__(self, time_units):
        self.time_units = time_units
        self._contours = []
        self._plots = []
        self._streamlines = []
        self._quivers = []
        self._grid_rows = 0
        self._grid_cols = 0

        # TODO: figsize must be specified
        self._fig = plt.figure(figsize=(20,10))

        anim.TimedAnimation.__init__(self, self._fig, interval=50, blit=False)

    def add_contour_animation(self, fields_2d, x, y, title, x_label, y_label):
        self._contours.append(ScalarFieldAnimationData(fields_2d, x, y, title, x_label, y_label))

    def add_quiver_animation(self, u_fields_2d, v_fields_2d, x, y, title, x_label, y_label):
        self._quivers.append(QuiverAnimationData(u_fields_2d, v_fields_2d, x, y, title, x_label, y_label))

    def add_streamlines_animation(self, u_fields_2d, v_fields_2d, x, y, title, x_label, y_label):
        self._streamlines.append(QuiverAnimationData(u_fields_2d, v_fields_2d, x, y, title, x_label, y_label))

    def add_plot_animation(self, x, y, title, x_label, y_label):
        self._plots.append(PlotAnimationData(x, y, title, x_label, y_label))

    def set_layout(self, rows, cols):
        self._grid_rows = rows
        self._grid_cols = cols

    def _draw_frame(self, framedata):
        time = framedata
        axes = self._fig.get_axes()
        for c in range(len(self._contours)):
            # To update colorbar the following dirty hack is used.
            # We use method Colorbar.update_bruteforce which clears an axis,
            # so 
            # Yes, this is ugly, have to deal with it until we create contour_set only once and then update it for each frame
            # instead of creating contour_set for each frame via contourf
            #self._fig.clf()
            #axes[c].cla()
            contours_data = self._contours[c]
            x, y = np.meshgrid(contours_data.coords[0], contours_data.coords[1], indexing='ij')
            contour_set = axes[c].contourf(x, y, contours_data.fields[time], 50)
            axes[c].set_title(contours_data.title + ', T = ' + str(time))
            if contours_data.colorbar is None:
                contours_data.colorbar = self._fig.colorbar(contour_set, ax=axes[c])
            else:
                contours_data.colorbar.update_bruteforce(contour_set)

        for q in range(len(self._quivers)):
            #self._quivers[q].quiver_raw_data.set_UVC(self._quivers[q].u_fields[time], self._quivers[q].v_fields[time])
            quivers_data = self._quivers[q]
            x, y = np.meshgrid(quivers_data.coords[0], quivers_data.coords[1], indexing='ij')
            axes[len(self._contours) + q].cla()
            #axes[len(self._contours) + q].streamplot(x, y, quivers_data.u_fields[time], quivers_data.v_fields[time])
            axes[len(self._contours) + q].quiver(x, y, quivers_data.u_fields[time], quivers_data.v_fields[time])
            axes[len(self._contours) + q].set_title(quivers_data.title + ', T = ' + str(time))
            axes[len(self._contours) + q].set_xlim(quivers_data.coords[0].min(), quivers_data.coords[0].max())
            axes[len(self._contours) + q].set_ylim(quivers_data.coords[1].min(), quivers_data.coords[1].max())
            axes[len(self._contours) + q].set_xlabel(quivers_data.x_label)
            axes[len(self._contours) + q].set_ylabel(quivers_data.y_label)

        for s in range(len(self._streamlines)):
            quivers_data = self._streamlines[s]
            field_norm = np.sqrt(quivers_data.u_fields[time]**2 + quivers_data.v_fields[time]**2)
            axes[len(self._contours) + len(self._quivers) + s].cla()
            axes[len(self._contours) + len(self._quivers) + s].streamplot(quivers_data.coords[0], quivers_data.coords[1], np.transpose(quivers_data.u_fields[time]), np.transpose(quivers_data.v_fields[time]), linewidth=2, color=np.transpose(field_norm), cmap=plt.cm.inferno, density=2, arrowsize=2)
            axes[len(self._contours) + len(self._quivers) + s].set_title(quivers_data.title + ', T = ' + str(time))
            axes[len(self._contours) + len(self._quivers) + s].set_xlim(quivers_data.coords[0].min(), quivers_data.coords[0].max())
            axes[len(self._contours) + len(self._quivers) + s].set_ylim(quivers_data.coords[1].min(), quivers_data.coords[1].max())
            axes[len(self._contours) + len(self._quivers) + s].set_xlabel(quivers_data.x_label)
            axes[len(self._contours) + len(self._quivers) + s].set_ylabel(quivers_data.y_label)
    
        for p in range(len(self._plots)):
            self._plots[p].line.set_data(self._plots[p].x[:time], self._plots[p].y[:time])

        print('T = ' + str(time) + ' is drawn')

    def new_frame_seq(self):
        return iter(self.time_units)

    def _init_draw(self):
        for p in range(len(self._plots)):
            self._plots[p].set_line(Line2D([], []))

        gs = gridspec.GridSpec(self._grid_rows, self._grid_cols)
        if len(self._contours) + len(self._quivers) + len(self._plots) > self._grid_rows * self._grid_cols:
            raise BadAnimationLayout('The number of contours, quivers and plots is not consistent with the grid specification.')

        # TODO: join into one loop
        # at this moment, order is: contours, quivers, and then plots
        time_units_number = 0
        if self._contours != []:
            time_units_number = len(self._contours[0].fields)
        elif self._quivers != []:
            time_units_number = len(self._quivers[0].u_fields)
        elif self._streamlines != []:
            time_units_number = len(self._streamlines[0].u_fields)
        elif self._plots != []:
            time_units_number = len(self._plots[0].x)
        else:
            raise EmptyAnimation('No data to be animated')

        curr_row = 0
        curr_col = 0

        # TODO: generalize
        for c in range(len(self._contours)):
            if time_units_number != len(self._contours[c].fields):
                raise NotConsistentTimeUnits('Number of time units in ' + str(c) + 'th contour does not match with check value (1st contour)')
            self._put_contour_into_figure(gs, curr_row, curr_col, self._contours[c])
            curr_row += 1
            if curr_row == self._grid_rows:
                curr_col += 1
                curr_row = 0

        for q in range(len(self._quivers)):
            if time_units_number != len(self._quivers[q].u_fields):
                raise NotConsistentTimeUnits('Number of time units in ' + str(q) + 'th quiver does not match with check value (1st quiver)')
            self._put_quiver_into_figure(gs, curr_row, curr_col, self._quivers[q])
            curr_row += 1
            if curr_row == self._grid_rows:
                curr_col += 1
                curr_row = 0

        for s in range(len(self._streamlines)):
            if time_units_number != len(self._streamlines[s].u_fields):
                raise NotConsistentTimeUnits('Number of time units in ' + str(s) + 'th quiver does not match with check value (1st quiver)')
            self._put_quiver_into_figure(gs, curr_row, curr_col, self._streamlines[s])
            curr_row += 1
            if curr_row == self._grid_rows:
                curr_col += 1
                curr_row = 0

        for p in range(len(self._plots)):
            if time_units_number != len(self._plots[p].x):
                raise NotConsistentTimeUnits('Number of time units in ' + str(p) + 'th plot does not match with check value (1st plot)')
            self._put_plot_into_figure(gs, curr_row, curr_col, self._plots[p])
            curr_row += 1
            if curr_row == self._grid_rows:
                curr_col += 1
                curr_row = 0

        self._fig.tight_layout()

    def _put_contour_into_figure(self, grid_spec, row, col, contours_data):
        ax = self._fig.add_subplot(grid_spec[row, col])
        ax.set_xlabel(contours_data.x_label)
        ax.set_ylabel(contours_data.y_label)
        #axes[c].set_aspect('equal') # to make x,y axes scales correct
        ax.set_xlim(contours_data.coords[0].min(), contours_data.coords[0].max())
        ax.set_ylim(contours_data.coords[1].min(), contours_data.coords[1].max())

    def _put_quiver_into_figure(self, grid_spec, row, col, quivers_data):
        ax = self._fig.add_subplot(grid_spec[row, col])
        #x, y = np.meshgrid(quivers_data.coords[0], quivers_data.coords[1], indexing='ij')
        #q = ax.quiver(x, y, quivers_data.u_fields[0], quivers_data.v_fields[0])
        #q = ax.streamplot(x, y, quivers_data.u_fields[0], quivers_data.v_fields[0])
        #quivers_data.set_quiver_raw_data(q)
        ax.set_xlabel(quivers_data.x_label)
        ax.set_ylabel(quivers_data.y_label)
        #ax.set_xlim(quivers_data.coords[0].min(), quivers_data.coords[0].max())
        #ax.set_ylim(quivers_data.coords[1].min(), quivers_data.coords[1].max())

    def _put_plot_into_figure(self, grid_spec, row, col, plot_data):
        ax = self._fig.add_subplot(grid_spec[row, col])
        line = Line2D([], [], color='black', linewidth=2)
        plot_data.set_line(line)
        ax.add_line(line)
        ax.set_xlim(min(plot_data.x), 1.2*max(plot_data.x))
        ax.set_ylim(min(plot_data.y), 1.2*max(plot_data.y))
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

class ScalarFieldAnimationData:
    def __init__(self, fields_2d, x, y, title, x_label, y_label):
        self.fields = fields_2d
        self.coords = (x, y)
        self.title = title
        self.x_label = x_label
        self.y_label = y_label
        self.colorbar = None

class QuiverAnimationData:
    def __init__(self, u_fields_2d, v_fields_2d, x, y, title, x_label, y_label):
        self.u_fields = u_fields_2d
        self.v_fields = v_fields_2d
        self.coords = (x, y)
        self.title = title
        self.x_label = x_label
        self.y_label = y_label

    def set_quiver_raw_data(self, q):
        self.quiver_raw_data = q

class PlotAnimationData:
    def __init__(self, x, y, title, x_label, y_label):
        self.x = x
        self.y = y
        self.title = title
        self.x_label = x_label
        self.y_label = y_label

    def set_line(self, line):
        self.line = line    

class QuickAnimation(BaseQuickAnimation):
    def __init__(self, time_units):
        BaseQuickAnimation.__init__(self, time_units)

    def add_contour_animation(self, fields, title):
        self._verify_fields(fields, 2, 1)
        raw_fields = [field.elements[0] for field in fields]
        coord1 = fields[0].space.elements[0]
        coord2 = fields[0].space.elements[1]
        coord1_name = fields[0].space.elements_names[0]
        coord2_name = fields[0].space.elements_names[1]
        BaseQuickAnimation.add_contour_animation(self, raw_fields, coord1, coord2, title, coord1_name, coord2_name)

    def add_quiver_animation(self, fields, title):
        self._verify_fields(fields, 2, 2)
        raw_fields1 = [field.elements[0] for field in fields]
        raw_fields2 = [field.elements[1] for field in fields]
        coord1 = fields[0].space.elements[0]
        coord2 = fields[0].space.elements[1]
        coord1_name = fields[0].space.elements_names[0]
        coord2_name = fields[0].space.elements_names[1]
        BaseQuickAnimation.add_quiver_animation(self, raw_fields1, raw_fields2, coord1, coord2, title, coord1_name, coord2_name)

    def add_streamlines_animation(self, fields, title):
        self._verify_fields(fields, 2, 2)
        raw_fields1 = [field.elements[0] for field in fields]
        raw_fields2 = [field.elements[1] for field in fields]
        coord1 = fields[0].space.elements[0]
        coord2 = fields[0].space.elements[1]
        coord1_name = fields[0].space.elements_names[0]
        coord2_name = fields[0].space.elements_names[1]
        BaseQuickAnimation.add_streamlines_animation(self, raw_fields1, raw_fields2, coord1, coord2, title, coord1_name, coord2_name)

    def add_plot_animation(self, x_labeled_list, y_labeled_list, title):
        BaseQuickAnimation.add_plot_animation(self, x_labeled_list.values, y_labeled_list.values, title, x_labeled_list.label, y_labeled_list.label)

    def save(self, filename):
        writer = anim.AVConvWriter(fps=10, bitrate=10000) # 400 kbits/s is 240p video
        BaseQuickAnimation.save(self, filename, writer=writer)

    def _verify_fields(self, fields, space_elem_required, field_elem_required):
        if fields[0].space.dim() != space_elem_required:
            raise IncorrectAnimationData('Only %d-dimensional spaces are eligible.' % space_elem_required)

        if len(fields[0].elements) != field_elem_required:
            raise IncorrectAnimationData('A vector field has to have exactly %d elements.' % field_elem_required)

class IncorrectAnimationData(Exception):
    pass

if __name__ == '__main__':
    from test_fields import get_time_dependent_wave_fields, get_time_dependent_circular_flow_fields
    #test_fields = get_time_dependent_wave_fields()
    #anim_ = QuickAnimation(range(len(test_fields)))
    #anim_.set_layout(1, 1)
    #anim_.add_contour_animation(test_fields, 'Test scalar field')
    #anim_.save('test_contour_animation.mp4')

    test_fields = get_time_dependent_circular_flow_fields()
    filtered_test_fields = []
    for field in test_fields:
        #filtered_test_fields.append(field.filter('x', 0.5).filter('y', 0.5))
        filtered_test_fields.append(field)

    anim_ = QuickAnimation(range(len(filtered_test_fields)))
    anim_.set_layout(1, 1)
    #anim_.add_quiver_animation(filtered_test_fields, 'Flow past a cylinder with varying circulation')
    #anim_.save('test_quiver_animation.mp4')
    anim_.add_streamlines_animation(filtered_test_fields, 'Flow past a cylinder with varying circulation')
    anim_.save('test_streamlines_animation.mp4')
