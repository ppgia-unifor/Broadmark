// SimulationGenerator - Startup.cs (2019)
// Copyright: Ygor Rebou�as Serpa


using System;
using System.Collections.Generic;
using UnityEngine;


/// <summary>
/// This script assembles everything needed to run a batch of tests
/// This script is meant to be used from the editor, to debug/develop
/// scenes and engines.
/// Also, the 'Wizard' script creates a startup object from GUI-provided
/// data to run simulations from a build in run-time.
/// </summary>
public class Startup : MonoBehaviour {
    public Wizard wizard;

    public string OutputRoot = "../Simulations/";
    public PhysicsEngineType Engine;
    public bool Record = false;
    public bool RecordMovie = false;
    public int Frames = 1000;
    public SimulationType[] Simulations = {
        SimulationType.FreeFall
    };
    public ObjectsType[] ObjectTypes = {
        ObjectsType.Cubes
    };
    public int[] Ns = {
        1000
    };
    public SimulationParameters Template;

    private void Start() {
        Simulation.OutputRoot = this.OutputRoot;
        Runner r = this.GetComponent<Runner>();

        List<SimulationParameters> list = new List<SimulationParameters>();
        foreach (int n in this.Ns) {
            foreach (SimulationType sim in this.Simulations) {
                foreach (ObjectsType type in this.ObjectTypes) {
                    SimulationParameters instance = this.Template.ShallowCopy();
                    instance.Engine = this.Engine;
                    instance.Frames = this.Frames;
                    instance.RecordSimulations = this.Record;
                    instance.RecordMovie = this.RecordMovie;
                    instance.Simulation = sim;
                    instance.ObjectsType = type;
                    instance.N = n;

                    list.Add(instance);
                }
            }
        }

        r.wizard = this.wizard;
        r.parameters = list.ToArray();

        r.enabled = true;
    }
}