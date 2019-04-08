

from bm_run import generate_jsons, run_algorithms
from bm_parse import process_results


base_path = ""

batch_run_name = "Faltantes/" # use this to name this testing session. If blank, a auto-generated timestamped name will be used
root_folder = base_path + "D:/Bitbucket/broadmark/Scripts/Tests/"
scenes_folder = base_path + "E:/Simulations/Recordings_AABBs/"
algorithms = ["GPU_SAP"]
#algorithms = ["SAP", "SAP_AVX_Parallel", "DBVT F", "DBVT D", "Tracy", "Tracy_Parallel", "CGAL", "Grid_3D", "Grid_3D_SAP", "Grid_3D_Parallel", "Grid_3D_SAP_Parallel", "KD", "GPU_Grid", "GPU_LBVH", "GPU_SAP"]
additionalProperty = ""
additionaPropertyValues = []


broadmark_bin = base_path + "D:/Bitbucket/broadmark/Broadmark/bin64/Algorithms_Release_x64.exe"


tests_folder = generate_jsons(batch_run_name, root_folder, scenes_folder, algorithms, additionalProperty, additionaPropertyValues)
results_folder = run_algorithms(tests_folder, broadmark_bin)

process_results(results_folder)
