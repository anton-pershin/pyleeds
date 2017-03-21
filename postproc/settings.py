# It is plausible to create a collection of such settings. 

RESEARCH_REL_DIR = 'research'

class Host(object): # have to inherit object to make it new-style class (otherwise super() won't work)
    def __init__(self, chflow_absolute_path, research_absolute_path=RESEARCH_REL_DIR):
        self.research_abs_path = research_absolute_path 
        self.chflow_abs_path = chflow_absolute_path

    def get_path_to_program(self, program_name):
        return '/'.join((self.chflow_abs_path, 'programs', program_name))

    @classmethod
    def create_laptop(cls):
        return cls(chflow_absolute_path='/home/tony/projects/phd/channelflow_openmp/build')

class RemoteHost(Host):
    def __init__(self, ssh_host, chflow_absolute_path, research_absolute_path):
        self.ssh_host = ssh_host
        super(RemoteHost, self).__init__(chflow_absolute_path, research_absolute_path)

    @classmethod
    def create_arc2(cls):
        return cls('arc2.leeds.ac.uk', '/home/ufaserv1_f/mmap/arc/channelflow_openmp/build', '/nobackup/mmap/research')

    @classmethod
    def create_arc3(cls):
        return cls('arc3.leeds.ac.uk', '/home/ufaserv1_f/mmap/arc/channelflow_openmp/build', '/nobackup/mmap/research')