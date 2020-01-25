# -*- coding: utf-8 -*-
from __future__ import print_function, unicode_literals, with_statement, division, absolute_import, absolute_import
from bm_run import generate_jsons, run_algorithms
from bm_parse import process_results



batch_run_name = "" # use this to name this testing session. If blank, a auto-generated timestamped name will be used
broadmark_bin = "D:/Bitbucket/broadmark/Broadmark/bin64/Broadmark_Release_x64.exe"
scenes_folder = "E:/Simulations/Recordings_AABBs/"
output_folder = "D:/Bitbucket/broadmark/Scripts/Tests/"
algorithms = ["KD", "CGAL", "GPU_Grid"]
#algorithms = ["SAP", "SAP_AVX_Parallel", "DBVT F", "DBVT D", "Tracy", "Tracy_Parallel", "CGAL", "Grid_3D", "Grid_3D_SAP", "Grid_3D_Parallel", "Grid_3D_SAP_Parallel", "KD", "GPU_Grid", "GPU_LBVH", "GPU_SAP"]
additionalProperty = ""
additionaPropertyValues = []




tests_folder = generate_jsons(batch_run_name, output_folder, scenes_folder, algorithms, additionalProperty, additionaPropertyValues)
results_folder = run_algorithms(tests_folder, broadmark_bin)

process_results(results_folder)
