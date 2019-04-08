// SimulationGenerator - Stopwatch.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using UnityEngine;


public delegate void MeasurableAction();

public static class Stopwatch {
    public static float MeasureAction(MeasurableAction action) {
        float start = Time.realtimeSinceStartup;
        action?.Invoke();

        return Time.realtimeSinceStartup - start;
    }
}