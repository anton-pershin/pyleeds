from __future__ import division
import matplotlib
import matplotlib.pyplot as plt
from datetime import *
import os
import os.path

class QuickReport:
    '''
    Class for automatization of making quick reports 
    '''

    TEX_FILE_HEADER = '''\documentclass[11pt]{article}
\usepackage{amsfonts,longtable}
\usepackage{amsmath}
\usepackage{amssymb}
\usepackage{amsthm}
\usepackage{esint}
\usepackage{mathdesign}
\usepackage{graphicx}
\usepackage{float}
\usepackage{bm}
\usepackage{hyperref}
\usepackage{epstopdf}
\usepackage[export]{adjustbox}
\\textwidth=16cm
\oddsidemargin=1cm
\\textheight=21cm
\\topmargin=-1cm
\setlength\parindent{0pt}
\setcounter{MaxMatrixCols}{20}
'''

    TEX_FILE_FOOTER = '\end{document}'

    def __init__(self, title):
        self.title = title
        self.blocks = []

    def add_section(self, section_name):
        self.blocks.append('\section{' + section_name + '}\n')

    def add_comment(self, comment):
        self.blocks.append(comment)

    def add_plot(self, fig, axes, caption, comment):
        if not os.path.exists('./doc'):
            os.mkdir('./doc')
            if not os.path.exists('./doc/images'):
                os.mkdir('./doc/images')

        block_number = len(self.blocks)
        block_pic_filename = str(block_number) + '.eps'
        block_pic_path = './doc/images/' + block_pic_filename
        block_pic_latex_path = 'images/' + block_pic_filename
        figure_label = 'fig:' + str(block_number)
        fig.savefig(block_pic_path)
        plt.clf()
        block_content = comment.replace('\\ref{@this@}', '\\ref{' + figure_label + '}') + '\n'
        block_content += '\\begin{figure}[H]\n'
        block_content += '\includegraphics[width=' + str(fig.get_figwidth()/10.) + '\\textwidth, center]{' + block_pic_latex_path + '}\n'
        block_content += '\caption{' + caption + '.}\label{' + figure_label + '}\n\end{figure}\n'

        self.blocks.append(block_content)

    def print_out(self):
        if not os.path.exists('./doc'):
            os.mkdir('./doc')

        texfile = open('./doc/quick_report_' + str(date.today()) + '.tex', 'w')
        texfile.write(QuickReport.TEX_FILE_HEADER)
        texfile.write('\\title{' + self.title + '}\n')
        texfile.write('''\\begin{document}
\maketitle
''')
        for block in self.blocks:
            texfile.write(block)

        texfile.write(QuickReport.TEX_FILE_FOOTER)
        texfile.close()