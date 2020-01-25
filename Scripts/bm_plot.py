# -*- coding: utf-8 -*-
from __future__ import print_function, unicode_literals, with_statement, division, absolute_import
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from bm_utils import get_iterable
from os.path import join
from six.moves import zip


def plot_bars(results_folder):
    # Reading the data
    frame = pd.read_pickle(join(results_folder, "multi_index_described_frame.pickle"))
    
    # Fetching scenes, number of objects and algorithms
    scenes = list(set(frame.index.get_level_values(0)))
    ns = sorted(list(set(frame.index.get_level_values(1))))
    algorithms = list(set(frame.index.get_level_values(2)))
    
    sns.set(style="white", context="talk")
    for scene in scenes:
        f, axes = plt.subplots(1, len(ns), figsize=(12, 5), sharex=True, sharey=True)
        for (n, a) in zip(ns, get_iterable(axes)):
            subframe = frame["mean"].transpose()[scene][n]#[wanted_algorithms]
            subframe = subframe.sort_values(ascending=False)
            x = list(subframe.index)
            y = list(subframe)
    
            plot = sns.barplot(x=x, y=y, palette="rocket", ax=a)
            plot = plot.set_title(scene)
            a.axhline(0, color="k", clip_on=False)
            a.set_ylabel(str(n) + " Objects")
            for tick in a.get_xticklabels():
                tick.set_rotation(90)
                
            
        sns.despine(bottom=True)
        plt.tight_layout(h_pad=2)
        plt.show()

def plot_box(results_folder):
    # Reading the data
    frame = pd.read_pickle(join(results_folder, "multi_index_frame.pickle"))
    
    # Fetching scenes, number of objects and algorithms
    scenes = list(set(frame.index.get_level_values(0)))
    ns = sorted(list(set(frame.index.get_level_values(1))))
    algorithms = list(set(frame.index.get_level_values(2)))
    
    subframe = frame.transpose()[scenes[0]][ns[2]]
    
    sns.set(style="ticks", palette="pastel")
    pal = sns.cubehelix_palette(len(algorithms), rot=-.5, dark=.3)
    sns.violinplot(data=subframe, palette=pal)
    
        
    sns.despine(offset=10, trim=True)
    plt.show()

def plot_lines(results_folder):
    frame = pd.read_pickle(join(results_folder, "multi_index_described_frame.pickle"))
    scenes = list(set(frame.index.get_level_values(0)))
    algorithms = list(set(frame.index.get_level_values(2)))
    
    sns.set(style="white", context="poster")
    f, axes = plt.subplots(1, len(scenes), figsize=(12, 8), sharex=True, sharey=True)
    for (s, a) in zip(scenes, get_iterable(axes)):
        subframe = frame["mean"].transpose()[s]
        subframe = subframe.reset_index()
        subframe.rename(columns={'level_0':'N (10³)', 'level_1':'Algorithm', 'mean':'mean time'}, inplace=True)
        subframe['N (10³)'] = subframe['N (10³)'] / 1000
    
        if len(algorithms) <= 6:
            plot = sns.lineplot(x='N (10³)', y='mean time', hue='Algorithm', style="Algorithm", palette="rocket", ax=a, markers=True, data=subframe)
        else:
            plot = sns.lineplot(x='N (10³)', y='mean time', hue='Algorithm', ax=a, markers=True, data=subframe)

        plot = plot.set_title(s)
        a.axhline(0, color="k", clip_on=True)
        a.set_xticks(np.arange(0, 1024+128, 128.0))
        a.set_ylim(0, 0.55)
        a.set_yticks(np.arange(0, 0.55, 0.05))
    
        for tick in a.get_xticklabels():
            tick.set_rotation(90)
            
        
    sns.despine(bottom=True)
    plt.tight_layout(h_pad=2)
    plt.show()

