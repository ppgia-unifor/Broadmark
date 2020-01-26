# -*- coding: utf-8 -*-
from __future__ import print_function, unicode_literals, with_statement, division, absolute_import
from os import listdir, makedirs
from os.path import isfile, join, exists
from subprocess import Popen, CREATE_NEW_CONSOLE, PIPE
from datetime import datetime
from shutil import rmtree
from bm_parse import process_results


base_json = r"""
{
    "m_inputScene": "@",
    "m_outputLog": "$",
    "m_algorithm": "%",
    "m_algorithm_prettyName": "~",
    "&": "£"
}
"""

def generate_jsons(batch_name, root_folder, scenes_folder, algorithms, additionalProp = "", aditionalPropValues = [""]):
    # finding all scene files
    print("Searching for scenes at '" + scenes_folder + "'...")
    scenes = [f for f in listdir(scenes_folder) if isfile(join(scenes_folder, f)) and f.endswith('.aabbs')]
    scenes = []
    for f in listdir(scenes_folder):
        if isfile(join(scenes_folder, f)):
            if f.endswith('.aabbs'):
                scenes.append(f)
            else:
                print(" -The file '" + f + "' is not a valid scene file (.aabbs) and will not be included")

    if len(scenes) == 0:
        print(" -No scenes were found at '" + scenes_folder + "'. Aborting execution..."); return None
    else:
        print(" -" + str(len(scenes)) + " scenes found!")
    
    # creating tests and results folders
    if batch_name != "":
        tests_folder = join(root_folder, batch_name)
        if (exists(tests_folder)):
            rmtree(tests_folder) # in case it already existed
    else:
        tests_folder = join(root_folder, "Tests " + datetime.now().strftime('%Y-%m-%d_%H-%M-%S' + '/'))

    results_folder = join(tests_folder, "Results/")
    makedirs(tests_folder)
    makedirs(results_folder)
    print("Test files (.json) will be stored at '" + tests_folder + "'")
    print("Results files (.json and .csv) will be stored at '" + results_folder + "'")
    
    # generating all jsons for scene-algorithm pairs
    print("Generating algorithm-scene pairs for the following algorithms: " + str(algorithms))
    count = 0
    for s in scenes:
        for a in algorithms:
            if additionalProp == "":
                task = s + "_" + a + ".json"
                data = base_json
                data = data.replace("@", join(scenes_folder, s))
                data = data.replace("$", join(results_folder, task + "_results.json"))
                data = data.replace("%", a)
                data = data.replace("~", a)
                data = data.replace("&", "NoAdditionalProperty")
                data = data.replace('"£"', '""')
                with open(join(tests_folder, task), 'w') as json:
                    json.write(data)
                    count = count + 1
            else:
                for value in aditionalPropValues:
                    alg_prettyName = a + "_" + additionalProp + "_" + str(value)
                    task = s + "_" + alg_prettyName + ".json"
                    data = base_json
                    data = data.replace("@", join(scenes_folder, s))
                    data = data.replace("$", join(results_folder, task + "_results.json"))
                    data = data.replace("%", a)
                    data = data.replace("~", alg_prettyName)
                    data = data.replace("&", additionalProp)
                    data = data.replace('"£"', str(value))
            
                    with open(join(tests_folder, task), 'w') as json:
                        json.write(data)
                        count = count + 1

    print(" -" + str(count) + " test files were generated!")
    print()
    return tests_folder, results_folder
    

def run_algorithms(tests_folder, results_folder, broadmark_bin):
    if not exists(broadmark_bin):
        print("The Broadmark executable could not be found, aborting..."); return None
    if not broadmark_bin.endswith(".exe"):
        print("The supplied Broadmark executable seems not to be an executable file (*.exe)..."); return None

    # display header
    print("Broadmark executable found!")
    print("Starting tests execution...")
    print()

    process = Popen([broadmark_bin])
    process.wait()
    
    # fetching all generated jsons and executing them
    tests = [f for f in listdir(tests_folder) if isfile(join(tests_folder, f))]
    count = 1
    for t in tests:
        print("Test " + str(count) + " of " + str(len(tests)))
        
        args = [broadmark_bin, join(tests_folder, t)]
        process = Popen(args)
        process.wait()
        if process.returncode != 0:
            print(" -An error ocurred when executing this test. If the assertion happened on 'scene.h', this is an indication of a faulty scene file.")
            print()
        else:
            process_results(results_folder)

        count = count + 1