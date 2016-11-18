# pyleeds
Codes supporting research.

## postproc
This is a python package devoted to the tools for post-processing of results obtained from channelflow code.

### Example
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

## chflow_couette
This is a cmake-project for a launch of calculations with channelflow library for the case of Couette flow. The main feature is that the resulting program is configurated via ini-file. Namely, domain size, discretization, number of time units to integrate, Reynolds number and initial fields are set in the ini-file and, therefore, there is no need to recompile a program when the changes are needed.

### Project's structure
/src -- source files;
/lib -- third-party libraries are located here, namely, channelflow's library and its header files;
/res -- additional files that accompany the main program are located here. At the moment, it is a configuration ini-file "settings.ini";
/bin -- this directory emerges when make-build is done and stores results of compilation. Binary program file and all files located in /res will be there.

### Configuration file
Configuration file "settings.ini" has an a bit modified ini-format. In essence, it is merely a set of "key=value" pairs grouped by sections:

```
[Definitions]
Nx = 64;
Ny = 33;
Nz = 512;
LxPrefactor = 4; // Lx = LxPrefactor * pi
a = -1.0;
b =  1.0;
LzPrefactor = 16; // Lz = LzPrefactor * pi
T = 200; // Number of time units to be calculated
Re = 400.0;

[Initial conditions]
U_file = data-couette/u90;
```

Sections are denoted by square brackets. If necessary, it is possible to add an inline comment and comments at a separate line.

Values in "key=value" pairs can have several forms:
- strings;
- any arithemtic format;
- boolean values: 0/1, true/false (and any upper/lower cases), t/f; 
- string-valued and real-valued arrays : arrayParam = (abc, def, ghi) or arrayParam = (3.8, 5, 1)

### Build
The project is built by the following commands executed in the project's directory:
```
$ mkdir build && cd build
$ cmake ..
$ make 
```

A separate directory "build" is commonly used in cmake-project to avoid contaminating of the main project's directory.