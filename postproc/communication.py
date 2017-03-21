import os
import os.path
import shutil
import paramiko
import subprocess
import shlex
from stat import S_ISDIR
from postproc.settings import RemoteHost

paramiko.util.log_to_file("paramiko.log")

# Decorator
def enable_sftp(func):
    def wrapped_func(self, *args, **kwds):
        self._init_sftp()
        return func(self, *args, **kwds)
    return wrapped_func

class Communication:
    def __init__(self, remote_host, username, password):
        if not isinstance(remote_host, RemoteHost):
            Exception('Only RemoteHost can be used to build Communication')
        self.remote_host = remote_host
        #self.ssh_server = ssh_server
        self.ssh_client = paramiko.SSHClient()
        self.sftp_client = None
        self.main_dir = '/nobackup/mmap/research'

        self.ssh_client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        self.ssh_client.connect(self.remote_host.ssh_host, username=username, password=password)

#    @classmethod
#    def fromsettings(cls, settings):
#        return cls(settings.ssh_server, settings.username, settings.password)
#
#    @classmethod
#    def create_with_remote(cls, remote_host):
#        comm = cls()
#        print('Enter authorization data for the host ' + remote_host.ssh_host)
#        username = raw_input('Username: ')
#        password = raw_input('Password: ')
#        comm.set_remote_host(remote_host, username, password)
#        return comm
#
#    def set_remote_host(self, remote_host, username, password):
#        self.remote_host = remote_host
#        self.ssh_server = ssh_server
#        self.ssh_client = paramiko.SSHClient()
#        self.ssh_client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
#        self.ssh_client.connect(self.ssh_server, username=username, password=password)
#        self.sftp_client = None

    def execute(self, command, is_remote=False, printing=True):
        if is_remote:
            if self.ssh_client is None:
                raise Exception('Remote host is not set')

            self._print_exec_msg(command, is_remote)
            stdin, stdout, stderr = self.ssh_client.exec_command(command)
            if printing:
                for line in stdout:
                    print('\t\t' + line.strip('\n'))
                for line in stderr:
                    print('\t\t' + line.strip('\n'))
        else:
            # use PIPEs to avoid breaking the child process when the parent process finishes
            # (works on Linux, solution for Windows is to add creationflags=0x00000010 instead of stdout, stderr, stdin)
            self._print_exec_msg(command, is_remote)
            args = shlex.split(command)
            pid = subprocess.Popen(args, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)

    def copy(self, from_, to_, mode='from_local'):
        # possible mode:
        # default -> from local path to remote path
        # 'all_remote' -> from remote path to remote path
        # 'all_local' -> from local path to local path
        # 'from_remote' -> from local path to local path
        if mode != 'all_local':
            if self.ssh_client is None:
                raise Exception('Remote host is not set')
            self._init_sftp()

        if mode == 'from_local':
            self._copy_from_local(from_, to_)
        elif mode == 'from_remote':
            self._copy_from_remote(from_, to_)
        elif mode == 'all_local':
            if os.path.isfile(from_):
                self._print_copy_msg(from_, to_)
                shutil.copy(from_, to_)
            else:
                self._print_copy_msg(from_, to_)
                shutil.copytree(from_, to_)
        elif mode == 'all_remote':
            self._print_copy_msg(self.remote_host.ssh_host + ':' + from_, self.remote_host.ssh_host + ':' + to_)
            self._mkdirp(to_)
            self.execute('cp -r %s %s' % (from_, to_), is_remote=True)
        else:
            raise Exception("Incorrect mode '%s'" % mode)

    @enable_sftp
    def listdir(self, path_on_remote):
        return self.sftp_client.listdir(path_on_remote)

    @enable_sftp
    def _chdir(self, path=None):
        self.sftp_client.chdir(path)

    @enable_sftp
    def _mkdir(self, path):
        self.sftp_client.mkdir(path)

    def _mkdirp(self, path):
        path_list = path.split('/')
        cur_dir = ''
        if (path_list[0] == '') or (path_list[0] == '~'): # path is absolute and relative to user's home dir => don't need to check obvious
            cur_dir = path_list.pop(0) + '/'
        start_creating = False # just to exclude unnecessary stat() calls when we catch non-existing dir
        for dir_ in path_list:
            if dir_ == '': # trailing slash or double slash, can skip
                continue
            cur_dir += dir_
            if start_creating or (not self._is_remote_dir(cur_dir)):
                self._mkdir(cur_dir)
                if not start_creating:
                    start_creating = True

            cur_dir += '/'

    @enable_sftp
    def _open(self, filename, mode='r'):
        return self.sftp_client.open(filename, mode)

    @enable_sftp
    def _get(self, remote_path, local_path):
        return self.sftp_client.get(remote_path, local_path)

    @enable_sftp
    def _put(self, local_path, remote_path):
        return self.sftp_client.put(local_path, remote_path)

    def _is_remote_dir(self, path):
        try:
            return S_ISDIR(self.sftp_client.stat(path).st_mode)
        except IOError:
            return False

    def _copy_from_local(self, from_, to_):
        if os.path.isfile(from_):
            self._mkdirp(to_)
            self._print_copy_msg(from_, self.remote_host.ssh_host + ':' + to_)
            self._put(from_, to_ + '/' + os.path.basename(from_))
        elif os.path.isdir(from_):
            new_path_on_remote = to_ + '/' + os.path.basename(from_)
            self._mkdir(new_path_on_remote)
            for dir_or_file in os.listdir(from_):
                self._copy_from_local(from_ + '/' + dir_or_file, new_path_on_remote)
        else:
            Exception("Path %s probably does not exist" % from_)

    def _copy_from_remote(self, from_, to_):
        if not self._is_remote_dir(from_):
            self._print_copy_msg(self.remote_host.ssh_host + ':' + from_, to_)
            self._get(from_, to_ + '/' + os.path.basename(from_))
        else:
            new_path_on_local = to_ + '/' + os.path.basename(from_)
            os.mkdir(new_path_on_local)
            for dir_or_file in self.sftp_client.listdir(from_):
                self._copy_from_remote(from_ + '/' + dir_or_file, new_path_on_local)

    def disconnect(self):
        if self.sftp_client is not None:
            self.sftp_client.close()
        self.ssh_client.close()

    def _init_sftp(self):
        if self.sftp_client is None:
            self.sftp_client = self.ssh_client.open_sftp()

    def _print_copy_msg(self, from_, to_):
        print('\tCopying %s to %s' % (from_, to_))

    def _print_exec_msg(self, cmd, is_remote):
        where = '@' + self.remote_host.ssh_host if is_remote else ''
        print('\tExecuting %s: %s' % (where, cmd))

class NotImplementedError(Exception):
    pass