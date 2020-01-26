# -*- coding: utf-8 -*-
from __future__ import print_function, unicode_literals, with_statement, division, absolute_import
from os import listdir
from os.path import isfile, join, basename
from pandas.io.json import json_normalize
import pandas as pd
import numpy as np
import json
import sys

# This scripts loads all json data generated at 'results_folder' and feeds it to
# pandas' dataframes. We do so in four layouts, for convenience:
#       Basic Table
#       Basic Table (described)
#       Multi-Index
#       Multi-Index (described)


# configurable parameters


def process_results(results_folder):
    # reads all test resulsts
    results = [f for f in listdir(results_folder) if isfile(join(results_folder, f)) and f.endswith('.json')]
    if len(results) == 0:
        return

    frame = pd.DataFrame() # keeps a master dataframe to store all algorithms
    for r in results:
        with (open(join(results_folder, r), "r")) as file:
            data = json.load(file)
    
            # we are interested on the scene's simple name, the algorithm and the n of objects
            scene_file = data["Settings"]["m_inputScene"]
            with open(scene_file, "rb") as f:
                # the scene's name and object types are encoded as c-style strings on the first 256 bytes of the file
                scene_name = f.read(128)
                objtype_name = f.read(128)
                if sys.version_info[0] < 3:
                    scene_name = ''.join([c for c in scene_name if c != b'\x00'])
                    objtype_name = ''.join([c for c in objtype_name if c != b'\x00'])
                else:
                    scene_name = ''.join([chr(c) for c in scene_name if bytes([c]) != b'\x00'])
                    objtype_name = ''.join([chr(c) for c in objtype_name if bytes([c]) != b'\x00'])
                
                scene = scene_name + ' ' + objtype_name

            algorithm = data["Settings"]["m_algorithm_prettyName"]
            n = data["Settings"]["m_numberOfObjects"]
    
            # loads the data into a dataframe and store the "Total" column
            df = json_normalize(data["Frames"])
            df = pd.DataFrame(data["Frames"])
            frame[(scene, n, algorithm)] = df["Total"]
    
    
    
    # The master dataframe ready for use
    # Columns: (scene, n, algorithm)
    # Rows: frame_number
    main_frame = frame
    main_frame.to_csv(join(results_folder, "main_frame.csv"), sep=str(';'))
    main_frame.to_excel(join(results_folder, "main_frame.xlsx"))
    main_frame.to_pickle(join(results_folder, "main_frame.pickle"))
    #print("main_frame written!")
    
    # The master dataframe in described form
    # Columns: (scene, n, algorithm)
    # Rows: frame_number
    described_frame = frame.describe()
    described_frame.to_csv(join(results_folder, "main_described_frame.csv"), sep=str(';'))
    described_frame.to_excel(join(results_folder, "main_described_frame.xlsx"))
    described_frame.to_pickle(join(results_folder, "main_described_frame.pickle"))
    #print("main_described_frame written!")
    
    
    # Creates a multi-indexed dataframe grouping algorithms by scene then by n
    # Columns: frame_number
    # Rows: Scene > N > Algorithm
    index = pd.MultiIndex.from_tuples(frame.columns)
    mi_frame = frame.transpose()
    mi_frame = mi_frame.set_index(index)

    labels = (mi_frame.columns.to_series() / 50).astype(int)
    mi_frame = mi_frame.groupby(labels, axis=1).mean()
    
    mi_frame.to_csv(join(results_folder, "multi_index_frame.csv"), sep=str(';'))
    mi_frame.to_excel(join(results_folder, "multi_index_frame.xlsx"))
    mi_frame.to_pickle(join(results_folder, "multi_index_frame.pickle"))
    #print("multi_index_frame written!")
    
    
    # Creates a multi-indexed dataframe of described data
    # Columns: count, mean, std, min, 25%, 50%, 75%, max
    # Rows: Scene > N > Algorithm
    index = pd.MultiIndex.from_tuples(frame.columns)
    dmi_frame = frame.describe().transpose()
    dmi_frame = dmi_frame.set_index(index)
    dmi_frame.to_csv(join(results_folder, "multi_index_described_frame.csv"), sep=str(';'))
    dmi_frame.to_excel(join(results_folder, "multi_index_described_frame.xlsx"))
    dmi_frame.to_pickle(join(results_folder, "multi_index_described_frame.pickle"))
    #print("multi_index_described_frame written!")

    lines_frame = dmi_frame["mean"].transpose()
    lines_frame = lines_frame.reset_index()
    lines_frame = pd.pivot_table(lines_frame, values="mean", index="level_1", columns="level_2")
    lines_frame.to_csv(join(results_folder, "lines_frame.csv"), sep=str(';'))
    lines_frame.to_excel(join(results_folder, "lines_frame.xlsx"))
    lines_frame.to_pickle(join(results_folder, "lines_frame.pickle"))
    #print("lines_frame written!")
