// SimulationGenerator - VisualizationManager.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using UnityEngine;


public class VisualizationManager : MonoBehaviour {
    //public bool IsRunning = false;

    //[Header("Configs")]
    //public GameObject cubePrefab;
    //public GameObject arenaPrefab;
    //public Material[] colors;
    //public GUISkin skin;
    //public bool correctFreeFall = true;
    //public bool Record_AABBs = true;
    //public bool Record_Movie = false;

    //[Header("Tests")]
    //public int[] NumberOfObjects;
    //public GameObject[] Scenarios;
    //public ObjectsType[] Types;
    //public int N_Frames;


    //public void StartVisualizations() {
    //    this.IsRunning = true;
    //    Camera.main.backgroundColor = Color.white;
    //    this.StartCoroutine(this.ProcessSimulations().GetEnumerator());
    //}

    //IEnumerable ProcessSimulations() {
    //    VisualizationScript visualization = null;
    //    string recordingFile = "";
    //    foreach (int numberOfObject in this.NumberOfObjects) {
    //        foreach (GameObject scenario in this.Scenarios) {
    //            foreach (ObjectsType objectsTypes in this.Types) {
    //                recordingFile = @"Recordings_Unity\";
    //                recordingFile += $"{scenario.name.Substring(7)}_{objectsTypes} {numberOfObject * 1000}obj {this.N_Frames}frames.bytes";

    //                visualization = this.gameObject.AddComponent<VisualizationScript>();
    //                visualization.cubePrefab = this.cubePrefab;
    //                visualization.arenaPrefab = this.arenaPrefab;
    //                visualization.colors = this.colors;
    //                visualization.skin = this.skin;
    //                visualization.correctFreeFall = this.correctFreeFall;
    //                visualization.recordAABBs = this.Record_AABBs;
    //                visualization.recordMovie = this.Record_Movie;
    //                visualization.recordingFile = recordingFile;

    //                while(!visualization.Finished) {
    //                    yield return new WaitForSeconds(1.0f);
    //                }
    //                Object.Destroy(visualization);
    //            }
    //        }
    //    }
    //    this.IsRunning = false;
    //    Camera.main.backgroundColor = Color.white * (80f / 255f);
    //    yield return null;
    //}
}