// SimulationGenerator - VisualizationWizard.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using UnityEngine;


public class VisualizationWizard : MonoBehaviour {
    private bool AllSet = false;
    public VisualizationManager manager;
    public GameObject[] ScenarioPrefabs;


    //[Header("Recording")]
    //public bool CorrectFreeFall = true;
    //public bool Record_AABBs = true;
    //public bool Record_Movie = true;

    //[Header("Tests")]
    //public string NumberOfObjects = "1, 2, 4, 8, 12, 16, 20, 24, 28, 32, 64, 96, 128";
    //public bool FreeFall = true;
    //public bool Brownian = true;
    //public bool Gravity = true;
    //public string Frames = "1000";

    //public bool Cubes = true;
    //public bool Assorted;


    //private void OnGUI() {
    //    if (this.manager.IsRunning) {
    //        return;
    //    }

    //    GUILayout.BeginArea(new Rect(
    //                            Screen.width / 2.0f - 150,
    //                            Screen.height / 2.0f - 150,
    //                            300,
    //                            800));

    //    GUILayout.BeginVertical();

    //    GUILayout.Label("System Options");
    //    this.Frames = GUILayout.TextField(this.Frames);
    //    this.CorrectFreeFall = GUILayout.Toggle(this.CorrectFreeFall, nameof(VisualizationWizard.CorrectFreeFall));
    //    this.Record_AABBs = GUILayout.Toggle(this.Record_AABBs, nameof(VisualizationWizard.Record_AABBs));
    //    this.Record_Movie = GUILayout.Toggle(this.Record_Movie, nameof(VisualizationWizard.Record_Movie));

    //    GUILayout.Space(10);
    //    GUILayout.Label("N of Objects (CSV)");
    //    this.NumberOfObjects = GUILayout.TextField(this.NumberOfObjects);

    //    GUILayout.Label("Scenarios");
    //    this.FreeFall = GUILayout.Toggle(this.FreeFall, nameof(Wizard.FreeFall));
    //    this.Brownian = GUILayout.Toggle(this.Brownian, nameof(Wizard.Brownian));
    //    this.Gravity = GUILayout.Toggle(this.Gravity, nameof(Wizard.Gravity));

    //    GUILayout.Label("Variants");
    //    this.Cubes = GUILayout.Toggle(this.Cubes, nameof(Wizard.Cubes));
    //    this.Assorted = GUILayout.Toggle(this.Assorted, nameof(Wizard.Assorted));

    //    GUILayout.Space(25);
    //    if (GUILayout.Button("Setar Parâmetros")) {
    //        this.ConfigureSimulationManager();
    //    }
    //    if (this.AllSet) {
    //        if (GUILayout.Button("Iniciar")) {
    //            this.manager.StartVisualizations();
    //        }
    //    }

    //    GUILayout.EndVertical();
    //    GUILayout.EndArea();
    //}

    //private void ConfigureSimulationManager() {
    //    try {
    //        this.manager.correctFreeFall = this.CorrectFreeFall;
    //        this.manager.Record_AABBs = this.Record_AABBs;
    //        this.manager.Record_Movie = this.Record_Movie;

    //        List<int> numberOfObjectsParsed = new List<int>();
    //        foreach (string s in this.NumberOfObjects.Split(new[] { ',', ' ' }, StringSplitOptions.RemoveEmptyEntries)) {
    //            Debug.Log(s);
    //            numberOfObjectsParsed.Add(int.Parse(s));
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

    //        this.manager.N_Frames = int.Parse(this.Frames);

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