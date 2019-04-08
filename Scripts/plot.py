

from os import listdir
from os.path import isfile, join, basename
from bm_plot import *
from bm_parse import process_results
from os.path import join, isfile

base_path = "D:/Bitbucket/broadmark/"
results_folder = base_path + "/Scripts/Tests/1_Artigo_Tool/3 Gravity/GPU + Parallel/"

process_results(results_folder)
#if not isfile(join(results_folder, "main_frame.pickle")):
#    process_results(results_folder)

#plot_lines(results_folder)
#plot_bars(results_folder)
#plot_box(results_folder)


# starting_folder = "D:/Bitbucket/broadmark/Scripts/Tests/1_Artigo_Tool/"
# def parse_folder(folder):
#     print(" -" + folder)
#     results = [d for d in listdir(folder) if not isfile(join(folder, d))]

#     if len(results) > 0:
#         for d in results:
#             d = join(folder, d)
#             parse_folder(d)
#     else:
#         try:
#             process_results(folder)
#         except:
#             print(" -Error processing this folder")


# parse_folder(starting_folder)