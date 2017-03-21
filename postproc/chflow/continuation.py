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

# TODO: should add commands for travelling waves (aka relative equilibrium)


from postproc.research import Task
from postproc.chflow.aux import SgeTaskBuilder
from postproc.auxtools import parse_datafile

import os
from operator import itemgetter
import numpy as np

class Continuation:
    # It is a wrapper for Channelflow's program continuesoln

    def __init__(self, execution_host):
        self.__execution_host = execution_host
        self.__task_builder = SgeTaskBuilder(execution_host)

    def create_task_to_find_equilibrium(self, init_guess, init_Re):
        self.__task_builder.set_command('findsoln -eqb -R ' + str(init_Re) + ' ' + os.path.basename(init_guess))
        self.__task_builder.set_program(self.__execution_host.get_path_to_program('findsoln'))
        self.__task_builder.set_inputs((init_guess,))
        self.__task_builder.set_sge_properties(cores=16)
        return self.__task_builder.build_task()

    def create_task_to_find_travelling_wave(self, init_guess, init_Re, phase_shift_coord, init_period):
        self.__task_builder.set_command('findsoln -eqb -' + phase_shift_coord + 'rel -R ' + str(init_Re) + \
                                        ' -T ' + phase_shift_coord + ' ' + os.path.basename(init_guess))
        self.__task_builder.set_program(self.__execution_host.get_path_to_program('findsoln'))
        self.__task_builder.set_inputs((init_guess,))
        self.__task_builder.set_sge_properties(cores=16)
        return self.__task_builder.build_task()

    def create_task_to_start_equilibrium(self, init_Re, init_field, additional_flags=''):
        self.__task_builder.set_command('continuesoln -eqb -contRe ' + additional_flags + ' -R ' + str(init_Re) + \
                                        os.path.basename(init_field))
        self._task_builder.set_program(self.__execution_host.get_path_to_program('continuesoln'))
        self._task_builder.set_inputs((init_field,))
        self.__task_builder.set_sge_properties(cores=16)
        return self.__task_builder.build_task()

    def create_task_to_start_periodic_orbit(self, init_Re, init_period, init_field, additional_flags=''):
        self.__task_builder.set_command('continuesoln -orb -contRe ' + additional_flags + ' -T ' + str(init_period) + \
                                        os.path.basename(init_field_file))
        self.__task_builder.set_program(self.__execution_host.get_path_to_program('continuesoln'))
        self.__task_builder.set_inputs((init_field,))
        self.__task_builder.set_sge_properties(cores=16)
        return self.__task_builder.build_task()
        
    def create_task_to_restart_equilibrium(self, three_successfully_continued_fields_dirs, additional_flags=''):
        self.__task_builder.set_command('continuesoln -eqb -contRe ' + additional_flags + ' --restart ' + \
                                        ' '.join([os.path.basename(dir_) for dir_ in three_successfully_continued_fields_dirs]))
        self.__task_builder.set_program(self.__execution_host.get_path_to_program('continuesoln'))
        self.__task_builder.set_inputs(three_successfully_continued_fields_dirs)
        self.__task_builder.set_sge_properties(cores=16)
        return self.__task_builder.build_task()

    def create_task_to_restart_travelling_wave(self, phase_shift_coord, three_successfully_continued_fields_dirs, additional_flags=''):
        self.__task_builder.set_command('continuesoln -eqb -' + phase_shift_coord + 'rel -contRe ' + additional_flags + \
                                        ' --restart ' + ' ' + ' '.join([os.path.basename(dir_) for dir_ in three_successfully_continued_fields_dirs]))
        self.__task_builder.set_program(self.__execution_host.get_path_to_program('continuesoln'))
        self.__task_builder.set_inputs(three_successfully_continued_fields_dirs)
        self.__task_builder.set_sge_properties(cores=16)
        return self.__task_builder.build_task()
        
    def create_task_to_restart_periodic_orbit(self, three_successfully_continued_fields_dirs):
        self.__task_builder.set_command('continuesoln -orb -contRe --restart ' + \
                                        ' '.join([os.path.basename(dir_) for dir_ in three_successfully_continued_fields_dirs]))
        self.__task_builder.set_program(self.__execution_host.get_path_to_program('continuesoln'))
        self.__task_builder.set_inputs(three_successfully_continued_fields_dirs)
        self.__task_builder.set_sge_properties(cores=16)
        return self.__task_builder.build_task()

def harvest_continuation_data(paths):
    # use upward D direction for sorting data
    # first, sort data in all paths with respect to growing D
    # second, sort paths with respect to growing D
    cont_data_list = [get_continuation_data(path) for path in paths]
    for cont_data in cont_data_list:
        indexes_and_D_sorted = sorted(enumerate(cont_data['D']), key=itemgetter(1))
        indexes = [pair[0] for pair in indexes_and_D_sorted]
        cont_data['D'] = [pair[1] for pair in indexes_and_D_sorted]
        cont_data['Re'] = [cont_data['Re'][i] for i in indexes]
        cont_data['U_files'] = [cont_data['U_files'][i] for i in indexes]
    cont_data_list.sort(key=lambda x: ['D'][0])
    total_cont_data = {
        'D' : [],
        'Re' : [],
        'U_files' : []
    }

    prev_D = 0.
    prev_Re = 0.
    for cont_data in cont_data_list:
        for D, Re, U_file in zip(cont_data['D'], cont_data['Re'], cont_data['U_files']):
            if np.abs(D - prev_D) > 10**(-8) or np.abs(Re - prev_Re) > 10**(-8):
                total_cont_data['D'].append(D)
                total_cont_data['Re'].append(Re)
                total_cont_data['U_files'].append(U_file)

    return total_cont_data

def get_continuation_data(path):
    return parse_datafile(path + '/ReD.asc', ['Re', 'D', 'U_files'], [float, float, str], [0, 1, 4])
#    Re_list = []
#    D_list = []
#    U_files_list = []
#    f = open(ReD_path, 'r') # if not found, expection will be raised anyway
#    lines = f.readlines()
#    for line in lines[1:]: # skip the first line
#        tmp = line.split()
#        Re_list.append(float(tmp[0])) # Re
#        D_list.append(float(tmp[1])) # D
#        U_files_list.append(tmp[4] + '/ubest.h5') # dir
#
#    cont_data = {
#        'D' : D_list,
#        'Re' : Re_list,
#        'U_files' : U_files_list,
#    }
#    return cont_data