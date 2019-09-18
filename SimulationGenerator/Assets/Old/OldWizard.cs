// SimulationGenerator - Wizard.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using UnityEngine;


public class OldWizard : MonoBehaviour {
    private bool AllSet = false;
    public GameObject CubePrefab;
    public GameObject[] ScenarioPrefabs;
    public GameObject SpherePrefab;


    //[Header("Simulation")]
    //public string Frames = "1000";
    //public string SolverIterations = "2048";
    //public string VelocityIterations = "256";
    //public string FixedTimestepDenominator = "30";
    //public string SubSteps = "16";

    //[Header("Objects")]
    //public bool useSpheres = false;
    //public string size = "1,0";
    //public string mass = "1,0";
    //public string density = "0,01";
    //public string InitialPositionScale = "0,90";
    //public string MaxInitialNormalizedVelocity = "0,7";
    //public string NormalizedGravity = "0,0981";
    //public bool UseCCD = true;

    //[Header("Recording")]
    //public bool Record_Unity = true;
    //public bool Record_AABBs = true;

    //[Header("Tests")]
    //public string NumberOfObjects = "1, 2, 4, 8, 12, 16, 20, 24, 28, 32, 64, 96, 128";
    //public bool FreeFall = true;
    //public bool Brownian = true;
    //public bool Gravity = true;

    //public bool Cubes = true;
    //public bool Assorted;


    //private void OnGUI() {
    //    if (this.manager.IsRunning) {
    //        return;
    //    }

    //    GUILayout.BeginArea(new Rect(
    //        Screen.width / 2.0f -150,
    //        0,
    //        300,
    //        800));

    //    GUILayout.BeginVertical();
    //    GUILayout.Label("Quality Options (Frames, Solver, Velocity, 1/Timestep, Substeps)");
    //    this.Frames = GUILayout.TextField(this.Frames);
    //    this.SolverIterations = GUILayout.TextField(this.SolverIterations);
    //    this.VelocityIterations = GUILayout.TextField(this.VelocityIterations);
    //    this.FixedTimestepDenominator = GUILayout.TextField(this.FixedTimestepDenominator);
    //    this.SubSteps = GUILayout.TextField(this.SubSteps);

    //    GUILayout.Label("Objects Options (Size, Mass, Density, PosScale, SpeedScale, Gravity)");
    //    this.useSpheres = GUILayout.Toggle(this.useSpheres, "Use Spheres");
    //    this.size = GUILayout.TextField(this.size);
    //    this.mass = GUILayout.TextField(this.mass);
    //    this.density = GUILayout.TextField(this.density);
    //    this.InitialPositionScale = GUILayout.TextField(this.InitialPositionScale);
    //    this.MaxInitialNormalizedVelocity = GUILayout.TextField(this.MaxInitialNormalizedVelocity);
    //    this.NormalizedGravity = GUILayout.TextField(this.NormalizedGravity);

    //    this.UseCCD = GUILayout.Toggle(this.UseCCD, nameof(Wizard.UseCCD));

    //    GUILayout.Label("System Options");
    //    GUILayout.BeginHorizontal();
    //    this.Record_Unity = GUILayout.Toggle(this.Record_Unity, nameof(Wizard.Record_Unity));
    //    this.Record_AABBs = GUILayout.Toggle(this.Record_AABBs, nameof(Wizard.Record_AABBs));
    //    GUILayout.EndHorizontal();

    //    GUILayout.Space(10);
    //    GUILayout.Label("N of Objects (CSV)");
    //    this.NumberOfObjects = GUILayout.TextField(this.NumberOfObjects);

    //    GUILayout.Label("Scenarios");
    //    GUILayout.BeginHorizontal();
    //    this.FreeFall = GUILayout.Toggle(this.FreeFall, nameof(Wizard.FreeFall));
    //    this.Brownian = GUILayout.Toggle(this.Brownian, nameof(Wizard.Brownian));
    //    this.Gravity = GUILayout.Toggle(this.Gravity, nameof(Wizard.Gravity));
    //    GUILayout.EndHorizontal();

    //    GUILayout.Label("Variants");
    //    GUILayout.BeginHorizontal();
    //    this.Cubes = GUILayout.Toggle(this.Cubes, nameof(Wizard.Cubes));
    //    this.Assorted = GUILayout.Toggle(this.Assorted, nameof(Wizard.Assorted));
    //    GUILayout.EndHorizontal();

    //    GUILayout.Space(25);
    //    if (GUILayout.Button("Setar Parâmetros")) {
    //        this.ConfigureSimulationManager();
    //    }
    //    if (this.AllSet) {
    //        if (GUILayout.Button("Iniciar")) {
    //            this.manager.StartSimulations();
    //        }
    //    }

    //    GUILayout.EndVertical();
    //    GUILayout.EndArea();
    //}

    //private void ConfigureSimulationManager() {
    //    try {
    //        this.manager.Frames = int.Parse(this.Frames);
    //        this.manager.SolverIterations = int.Parse(this.SolverIterations);
    //        this.manager.VelocityIterations = int.Parse(this.VelocityIterations);
    //        this.manager.FixedTimestep = 1.0f / int.Parse(this.FixedTimestepDenominator);
    //        this.manager.SubSteps = int.Parse(this.SubSteps);

    //        this.manager.CubePrefab = this.useSpheres ? this.SpherePrefab : this.CubePrefab;
    //        this.manager.size = float.Parse(this.size);
    //        this.manager.mass = float.Parse(this.mass);
    //        this.manager.density = float.Parse(this.density);
    //        this.manager.InitialPositionScale = float.Parse(this.InitialPositionScale);
    //        this.manager.MaxInitialNormalizedVelocity = float.Parse(this.MaxInitialNormalizedVelocity);
    //        this.manager.NormalizedGravity = float.Parse(this.NormalizedGravity);
    //        this.manager.UseCCD = this.UseCCD;

    //        this.manager.Record_AABBs = this.Record_AABBs;
    //        this.manager.Record_Unity = this.Record_Unity;
    //        this.manager.Record_Movie = false;

    //        List<int> numberOfObjectsParsed = new List<int>();
    //        foreach (string s in this.NumberOfObjects.Split(new [] {',', ' '}, StringSplitOptions.RemoveEmptyEntries)) {
    //            Debug.Log(s);
    //            numberOfObjectsParsed.Add(int.Parse(s) * 1000);
    //        }

    //        this.manager.NumberOfObjects = numberOfObjectsParsed.ToArray();

    //        List<GameObject> scenarios = new List<GameObject>();
    //        if (this.FreeFall) {
    //            scenarios.Add(this.ScenarioPrefabs[0]);
    //        }
    //        if (this.Brownian) {
    //            scenarios.Add(this.ScenarioPrefabs[1]);
    //        }
    //        if (this.Gravity) {
    //            scenarios.Add(this.ScenarioPrefabs[2]);
    //        }

    //        this.manager.Scenarios = scenarios.ToArray();

    //        List<ObjectsType> types = new List<ObjectsType>();
    //        if (this.Cubes) {
    //            types.Add(ObjectsType.Cubes);
    //        }
    //        if (this.Assorted) {
    //            types.Add(ObjectsType.Assorted);
    //        }

    //        this.manager.Types = types.ToArray();

    //        this.AllSet = true;
    //        Debug.Log("All Set, You can Start");
    //    } catch (Exception e) {
    //        Debug.LogError("Failed Processing Arguments: " + e.Message);
    //        Debug.LogError("Call StacK: " + e.StackTrace);
    //        this.AllSet = false;
    //    }
    //}
}