# pyleeds
Python scripts supporting research

## Example
The most common example of usage of postproc package. It includes loading channelflow data from .h5 files, creating animations, plots and making a quick report.
```python
from postproc.field import Field, read_fields
from postproc.plotting import plot_lines, plot_filled_contours
from postproc.auxtools import LabeledList
from postproc.report import QuickReport
from postproc.animation import QuickAnimation

print('Starting...')
print('Reading fields...')
fields = read_fields('data-couette-4-2-16-400', file_prefix='u', file_postfix='.h5', start_time=1)

print('Averaging and integrating...')
u_fields_yz = []
u_L2norms = LabeledList()
v_L2norms = LabeledList()
w_L2norms = LabeledList()
for field in fields:
    u_fields_yz.append(field.average(['u'], 'x'))
    L2norms = field.L2_norms(normalize=True)
    u_L2norms.append(L2norms[0])
    v_L2norms.append(L2norms[1])
    w_L2norms.append(L2norms[2])

print('Making animation...')
anim = QuickAnimation(range(len(fields)))
anim.set_layout(1, 2)
anim.add_contour_animation(u_fields_yz, 'U(y,z,t) averaged along the x-coordinate')
anim.add_plot_animation(u_L2norms, v_L2norms, 'Phase space')
anim.save('test_animation.mp4')

print('Making report...')
qr = QuickReport('Test of the template report builder')
qr.add_section('Norms')
fig, axes = plot_lines([u_L2norms, u_L2norms, v_L2norms], [v_L2norms, w_L2norms, w_L2norms])
comment_str = '''In the figure \\ref{@this@} we can see the norms corresponding to the turbulent state'''
qr.add_plot(fig, axes, 'Norms', comment_str)

qr.add_section('Contour plot')
comment_str = '''In the figure \\ref{@this@} we can see the contour plot of $u(y,z,t)$ corresponding to $T = 0$'''
fig, axes = plot_filled_contours(u_fields_yz)
qr.add_plot(fig, axes, 'Contour plot', comment_str)

qr.print_out()
```
