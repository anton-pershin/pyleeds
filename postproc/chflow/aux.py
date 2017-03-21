from postproc.field import read_field, write_field
from postproc.settings import *
from postproc.research import Task

import os

class TaskBuilder(object):
    def __init__(self, execution_host): # so we allow polymorphism by passing Host or RemoteHost
        self._execution_host = execution_host
        self._command = None
        self._is_global_command = False
        self._program = None
        self._inputs = []

    # Setters are needed because it will allow to generalize build_task method in the future 
    # (i.e to create a base class and create several builder like TaskWithOutputBuilder, GraphTaskBuilder and so on where there will be another setters)
    def set_command(self, command, is_global_command = False):
        self._command = command
        self._is_global_command = is_global_command

    def set_program(self, program):
        self._program = program

    def set_inputs(self, inputs):
        self._inputs += list(inputs)

    def build_task(self):
        remote_execution = isinstance(self._execution_host, RemoteHost)
        if remote_execution:
            program_move_mode = 'all_remote'
            inputs_move_mode = 'from_local'
        else:
            program_move_mode = 'all_local'
            inputs_move_mode = 'all_local'

        copies_list = []
        if self._program is not None:
            copies_list.append({
                'path' : self._program,
                'mode' : program_move_mode
            })
        
        print(self._inputs)
        for input_ in self._inputs:
            copies_list.append({'path' : input_, 'mode' : inputs_move_mode})

        return Task(self._execution_host, self._command, self._is_global_command, copies_list)

class SgeTaskBuilder(TaskBuilder):
    def __init__(self, execution_host):
        self._sge_script_lines = []
        self._script_name = 'sge_script.sh'
        if not isinstance(execution_host, RemoteHost):
            raise Exception('SGE can be used only on the remote machine')
        super(SgeTaskBuilder, self).__init__(execution_host)

    def set_command(self, command):
        # assume that commands regard programs in the same dir
        self._sge_script_lines.append('./' + command)

    def set_sge_properties(self, cores, time='48:00:00'):
        self._sge_script_lines = ['#$ -cwd -V\n', '#$ -l h_rt=%s\n' % time, '#$ -pe smp %i\n' % cores] \
                                    + self._sge_script_lines

    def build_task(self):
        if len(self._sge_script_lines) != 0:
            super(SgeTaskBuilder, self).set_command('qsub ' + self._script_name, is_global_command=True)

        task = super(SgeTaskBuilder, self).build_task()

        # NAME SHOULD BE CONSISTENT WITH TASK!
        tmp_dir = 'tmp'
        tmp_sge_script_path = tmp_dir + '/' + self._script_name
        if not os.path.isdir(tmp_dir):
            os.mkdir(tmp_dir)

        f = open(tmp_sge_script_path, 'w')
        f.writelines(self._sge_script_lines)
        task.copies_list.append({
                'path' : tmp_sge_script_path,
                'mode' : 'from_local'
            })

        return task

class PythonSgeGeneratorTaskBuilder(TaskBuilder):
    def __init__(self, execution_host):
        self._sge_script_default_lines = []
        self._sge_script_command_lines = []
        self._sge_script_names = []
        self._py_script_name = 'sge_generator.py'
        if not isinstance(execution_host, RemoteHost):
            raise Exception('SGE can be used only on the remote machine')
        super(PythonSgeGeneratorTaskBuilder, self).__init__(execution_host)

    def set_command(self, command, sid):
        # assume that commands regard programs in the same dir
        self._sge_script_command_lines.append('./' + command)
        self._sge_script_names.append('gensge_{}.sh'.format(sid))

    def set_sge_properties(self, cores, time='48:00:00'):
        self._sge_script_default_lines = ['#$ -cwd -V\\n', '#$ -l h_rt=%s\\n' % time, '#$ -pe smp %i\\n' % cores]

    def build_task(self):
        if len(self._sge_script_names) != 0:
            # nohup is necessary to prevent killing process when logging out
            # & is necessary to put the process in background
            super(PythonSgeGeneratorTaskBuilder, self).set_command('nohup python ' + self._py_script_name + ' >/dev/null 2>&1 &', is_global_command=True)
        task = super(PythonSgeGeneratorTaskBuilder, self).build_task()

        # NAME SHOULD BE CONSISTENT WITH TASK!
        tmp_dir = 'tmp'
        tmp_py_script_path = tmp_dir + '/' + self._py_script_name
        if not os.path.isdir(tmp_dir):
            os.mkdir(tmp_dir)

        f = open(tmp_py_script_path, 'w')
        f.write('import subprocess\n')
        f.write('import time\n\n')
        filenames_list_as_str = ''
        contents_list_as_str = ''
        sge_script_default_content = ''.join(self._sge_script_default_lines)
        for sge_script_name, sge_script_command_line in zip(self._sge_script_names, self._sge_script_command_lines):
            if filenames_list_as_str == '':
                filenames_list_as_str = '['
                contents_list_as_str = '['
            else:
                filenames_list_as_str += ', '
                contents_list_as_str += ', '
            filenames_list_as_str += "'" + sge_script_name + "'"
            sge_script_content = sge_script_default_content + sge_script_command_line + '\\n'
            contents_list_as_str += "'" + sge_script_content + "'"
        filenames_list_as_str += ']\n'
        contents_list_as_str += ']\n'
        f.write('sge_filenames = ' + filenames_list_as_str)
        f.write('sge_contents = ' + contents_list_as_str)
        f.write("log = open('sge_gen.log', 'w')\n")
        f.write('while len(sge_filenames) != 0:\n')
        f.write("\tstdout = subprocess.check_output(['qstat'])\n")
        f.write("\tlines = stdout.split('\\n')\n")
        f.write("\tlog.write('have ' + str(len(lines)) + ' lines\\n')\n")
        f.write('\tmax_lines_num = 12\n')
        f.write('\tif len(lines) < max_lines_num:\n')
        f.write('\t\tavail_sge_tasks = max_lines_num - len(lines)\n')
        f.write('\t\tif avail_sge_tasks > len(sge_filenames):\n')
        f.write('\t\t\tavail_sge_tasks = len(sge_filenames)\n')
        f.write("\t\tlog.write('can launch ' + str(avail_sge_tasks) + ' tasks\\n')\n")
        f.write('\t\tfor i in range(avail_sge_tasks):\n')
        f.write("\t\t\tsge_file = open(sge_filenames[i], 'w')\n")
        f.write("\t\t\tsge_file.write(sge_contents[i])\n")
        f.write("\t\t\tsge_file.close()\n")
        f.write("\t\t\tsubprocess.call(['qsub', sge_filenames[i]])\n")
        f.write("\t\tlog.write('delete tasks ' + str(sge_filenames[:avail_sge_tasks]) + '\\n')\n")
        f.write('\t\tdel sge_filenames[:avail_sge_tasks]\n')
        f.write('\t\tdel sge_contents[:avail_sge_tasks]\n')
        f.write('\ttime.sleep(600)\n')
        f.write('log.close()\n')
        f.close()
        
        task.copies_list.append({
                'path' : tmp_py_script_path,
                'mode' : 'from_local'
            })

        return task

class GraphTaskBuilder:
    def __init__(self, execution_host):
        pass

    def set_task(self, task):
        pass

    def build_task(self):
        pass