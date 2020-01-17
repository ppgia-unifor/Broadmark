

from os import listdir, getcwd
from os.path import isfile, isdir, join, basename, getmtime
from bm_plot import *
from bm_parse import process_results
from os.path import join, isfile


# You can set the results folder to use directly or let the system find the most recent one
results_folder = ''
if results_folder == '':
    root = getcwd() + '\\Tests\\'
    all_subdirs = [root + d + '\\Results\\' for d in listdir(root)]
    latest_subdir = max(all_subdirs, key=getmtime)
    results_folder = latest_subdir


plot_lines(results_folder)
plot_bars(results_folder)
plot_box(results_folder)