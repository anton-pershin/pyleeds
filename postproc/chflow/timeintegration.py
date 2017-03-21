# What do I need?
# 1) Read file ReD.asc and return a function D(Re) from it.
# It is probably better to return an object which links corresponding D, Re and u-field together
# 2) Launch continuation -- both from scratch and proceeding previous continuation
# 3) All of these should be configurated such that I could use ssh and local ways of launching

# === LAUNCH ===
# Want: 
# if I start, then create a command for continuesoln like this one:
# ./continuesoln -eqb -contRe -R 173.31078118774471 -up TestEQ4pi32piRe173p31.h5
# if I restart from previous continuation, then create a command like this one:
# continuesoln -eqb -contRe --restart   ../contRe0/search-14 ../contRe0/search-15 ../contRe0/search-16

# === POSTPROCESSING RESULTS ===
# Want:
# being provided with filesystem (doesn't matter local or remote), get D(Re) function from results 
# and corresponding files. This is, in essence, parsing of ReD.asc 

from postproc.chflow.aux import *
from postproc.settings import *
from postproc.auxtools import parse_datafile

import os

class TimeIntegration:
    # It is a wrapper for Channelflow's program couette

    def __init__(self, execution_host):
        self.__execution_host = execution_host
        if isinstance(execution_host, RemoteHost):
            self.__task_builder = SgeTaskBuilder(execution_host)
        else:    
            self.__task_builder = TaskBuilder(execution_host)

    def create_task(self, Re, T_start, T_end, dt, dT_fields, dT_data, max_ke_threshold, init_field):
        self.__task_builder.set_command('couette -T0 {} -T1 {} -dt {} -R {} -dT {} -dPT {} -ke {} {}'.format(T_start, T_end, \
                                        dt, Re, dT_fields, dT_data, max_ke_threshold, os.path.basename(init_field)))
        self.__task_builder.set_program(self.__execution_host.get_path_to_program('couette'))
        self.__task_builder.set_inputs((init_field,))
        if isinstance(self.__task_builder, SgeTaskBuilder):
            self.__task_builder.set_sge_properties(16)
        return self.__task_builder.build_task()

    def create_plural_task(self, Re_list, T_start, T_end, dt_list, dT_fields, dT_data, max_ke_threshold, init_field):
        task_builder = PythonSgeGeneratorTaskBuilder(self.__execution_host)
        for Re, dt in zip(Re_list, dt_list):
            task_builder.set_command('couette -T0 {} -T1 {} -dt {} -R {} -dT {} -dPT {} -ke {} -o data-{} {}'.format(T_start, T_end, \
                                            dt, Re, dT_fields, dT_data, max_ke_threshold, Re, os.path.basename(init_field)), sid=str(Re))
        task_builder.set_program(self.__execution_host.get_path_to_program('couette'))
        task_builder.set_inputs((init_field,))
        task_builder.set_sge_properties(16, '24:00:00')
        return task_builder.build_task()

    def create_perturbed_field_task(self, Re, T_start, T_end, dt, seed, init_field, smoothness=0.4, magnitude=0.2):
        # both smoothness and magnitude are defined on [0, 1]
        # seed is a seed for randomizer
        pass

def harvest_integration_data(data_paths):
    data = []
    for path in data_paths:
        data.append(get_integration_data(path))
    return data

def get_integration_data(data_path):
    return parse_datafile(data_path + '/summary.txt', ['T', 'L2U', 'D', 'max_KE', 'LF', 'RF'], [float, float, float, float, float, float])