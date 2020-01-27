# Scripts Folder

In this folder, there are the scripts used to batch-execute the Broadmark tool. Use the `run.py` script to run algorithms against scenes and use the `plot.py` script to generate plots of the generated results. Both scripts are simple and their configuration is straight-forward, just open them on your favorite text editor and change any of the variables if needed. 

These scripts were tested on both Python 2 and 3 and require numpy, pandas, openpyxl, matplotlib and seaborn to run, being these last two only needed for the plot.py script. You can install all dependencies using the following one-liner: `pip install numpy pandas, openpyxl matplotlib seaborn`.