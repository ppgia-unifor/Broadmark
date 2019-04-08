// SimulationGenerator - PositionRotationRecorder.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using UnityEngine;


public class PositionRotationRecorder : SimulationListener {
    private FileStream fs;
    private BinaryWriter bw;
    private string fileName;

    public override void OnStart(Simulation simulation) {
        this.fileName = $"{simulation}.posrot";

        string dir = Path.Combine(Simulation.OutputRoot, "Recordings_PositionRotation/");
        Directory.CreateDirectory(dir);
        this.fs = new FileStream(Path.Combine(dir, this.fileName), FileMode.OpenOrCreate);
        this.bw = new BinaryWriter(this.fs);

        this.bw.WriteCStyleString(simulation.Name, 128);
        this.bw.WriteCStyleString(simulation.Parameters.ObjectsType.ToString(), 128);
        this.bw.Write(simulation.Parameters.N);
        this.bw.Write(simulation.Parameters.Frames);
        this.bw.Write(0); // padding
        this.bw.Write(0);
        this.bw.Write(simulation.Scene.GetComponent<Renderer>().bounds);
        foreach (Bounds b in simulation.Bodies.Select(x => x.Bounds)) {
            this.bw.Write(b);
        }
    }

    public override void OnStep(Simulation simulation) {
        foreach (Transform t in simulation.Bodies.Select(x => x.transform)) {
            this.bw.Write(t.position);
            this.bw.Write(t.rotation);
        }
    }

    public override void OnStop(Simulation simulation) {
        this.bw.Flush();
        this.bw.Close();
    }
}