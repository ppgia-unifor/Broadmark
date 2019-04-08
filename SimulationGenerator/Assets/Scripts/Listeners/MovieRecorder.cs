// SimulationGenerator - RecorderMovie.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using System.IO;
using UnityEngine;


public class MovieRecorder : SimulationListener {
    private string dir;
    private int frame;
    private int interval = 5;

    public override void OnStart(Simulation simulation) {
        this.dir = Path.Combine(Simulation.OutputRoot, $"Recordings_Movies/{simulation} Movie");
        Directory.CreateDirectory(this.dir);
    }

    public override void OnStep(Simulation simulation) {
        if (this.interval > 0 && this.frame % this.interval == 0) {
            ScreenCapture.CaptureScreenshot($"{this.dir}/{this.frame:0000}.png", 1);
        }
        this.frame++;
    }

    public override void OnStop(Simulation simulation) { }
}