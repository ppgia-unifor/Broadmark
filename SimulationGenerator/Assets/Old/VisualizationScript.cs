// SimulationGenerator - VisualizationScript.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using UnityEngine;


public class VisualizationScript : MonoBehaviour {
    //   public string recordingFile;
    //   public GameObject cubePrefab;
    //   public GameObject arenaPrefab;
    //   public Material[] colors;
    //   public GUISkin skin;
    //   public bool correctFreeFall = true;
    //   public bool recordAABBs = true;
    //   public bool recordMovie = false;

    //   [HideInInspector] public bool Finished;

    //   private FileStream stream;
    //   private BinaryReader buffer;
    //   private GameObject world;
    //   private GameObject[] cubes;
    //   private Vector3[] sizeBuffer;
    //   private Vector3[] posBuffer;
    //   private Quaternion[] rotBuffer;
    //   private List<Renderer> Renderers;
    //   private bool[] statics;
    //   private long[] timeToStatics;
    //   private Vector3[] oldPos;
    //   private Quaternion[] oldRot;
    //   private int statics_count;
    //   private Material staticsMaterial;
    //   private Vector3 worldMin;
    //   private Vector3 worldMax;
    //   private float worldSideLength;
    //   private long total;
    //   private long count;
    //   private long frame;
    //   private float beginMark = 0;

    //   private void Create() {
    //       float FixedTimestep = 1.0f / 60.0f;
    //       this.beginMark = Time.time;
    //       Time.timeScale = 1f;
    //       Time.fixedDeltaTime = FixedTimestep;
    //       Time.captureFramerate = (int)(1f / FixedTimestep);

    //       this.Finished = false;


    //       this.stream = new FileStream(this.recordingFile, FileMode.Open);
    //       this.buffer = new BinaryReader(this.stream);

    //       long n = this.buffer.ReadInt32();
    //       long t = this.stream.Length;
    //       long f = (t - 36 - n * 16) / (n * (16 + 16)); // tira a int e o header do começo, divide pelo # de bytes por frame
    //       long mb = t / (1024 * 1024);
    //       Debug.Log(n + " objects, " + mb + "mb and " + f + " frames");

    //       this.cubes = new GameObject[n];
    //       this.sizeBuffer = new Vector3[n];
    //       this.posBuffer = new Vector3[n];
    //       this.rotBuffer = new Quaternion[n];
    //       this.statics = new bool[n];
    //       this.timeToStatics = new long[n];
    //       this.oldPos = new Vector3[n];
    //       this.oldRot = new Quaternion[n];
    //       this.Renderers = new List<Renderer>();
    //       this.staticsMaterial = new Material(this.cubePrefab.GetComponent<Renderer>().sharedMaterial);
    //       this.staticsMaterial.color = Color.white * 0.25f;

    //       this.worldMin = new Vector3(this.buffer.ReadSingle(), this.buffer.ReadSingle(), this.buffer.ReadSingle());
    //       this.buffer.ReadSingle();
    //       this.worldMax = new Vector3(this.buffer.ReadSingle(), this.buffer.ReadSingle(), this.buffer.ReadSingle());
    //       this.buffer.ReadSingle();

    //       this.world = Object.Instantiate<GameObject>(this.arenaPrefab, Vector3.zero, Quaternion.identity);
    //       this.world.transform.position = (this.worldMax + this.worldMin) / 2;
    //       this.world.transform.localScale = (this.worldMax - this.worldMin);
    //       this.world.transform.parent = this.transform;
    //       this.worldSideLength = this.world.transform.localScale.x;

    //       Camera camera = Camera.main;
    //       Vector3 cameraPos = Vector3.one * this.worldSideLength;
    //       cameraPos.Scale(new Vector3(0.57f, 0.20f, 0.6f));
    //       cameraPos *= 3.0f;

    //       Vector3 lookat = this.world.transform.position;
    //       lookat -= Vector3.up * this.worldSideLength * 0.15f;

    //       camera.transform.position = cameraPos;
    //       camera.transform.LookAt(lookat, Vector3.up);

    //       // size buffer
    //       for (int j = 0; j < n; j++) {
    //           this.sizeBuffer[j] = new Vector3(this.buffer.ReadSingle(), this.buffer.ReadSingle(), this.buffer.ReadSingle());

    //           this.buffer.ReadSingle();
    //       }
    //       // pos and rot buffers
    //       for (int j = 0; j < n; j++) {
    //           this.posBuffer[j] = new Vector3(this.buffer.ReadSingle(), this.buffer.ReadSingle(), this.buffer.ReadSingle());
    //           this.buffer.ReadSingle();
    //       }
    //       for (int j = 0; j < n; j++) {
    //           this.rotBuffer[j] = new Quaternion(this.buffer.ReadSingle(), this.buffer.ReadSingle(), this.buffer.ReadSingle(), this.buffer.ReadSingle());
    //       }

    //       HashSet<int> hiddenObjects = new HashSet<int> {89, 3291, 3850, 1212, 1211, 2408};
    //       //HashSet<int> hiddenObjects = new HashSet<int> { 2491, 1768, 970, 2970, 92, 174, 3136, 651, 2809 };
    //       for (int i = 0; i < n; i++) {
    //           this.cubes[i] = (GameObject)Object.Instantiate(this.cubePrefab);

    //           this.cubes[i].transform.parent = this.transform;
    //           this.cubes[i].transform.localScale = this.sizeBuffer[i];
    //           this.cubes[i].transform.position = this.posBuffer[i];
    //           this.cubes[i].transform.rotation = this.rotBuffer[i];

    //           Renderer r = this.cubes[i].GetComponent<Renderer>();
    //           float mag = this.sizeBuffer[i].sqrMagnitude;

    //           if (mag > 230) { //bars
    //               r.material = this.colors[0];
    //               this.cubes[i].name = "Bar: " + i;
    //           } else if (mag > 34) { // planks
    //               r.material = this.colors[1];
    //               this.cubes[i].name = "Plank: " + i;
    //           } else if (mag > 14) { // squares
    //               r.material = this.colors[2];
    //               this.cubes[i].name = "Square: " + i;
    //           } else {
    //               r.material = this.colors[3];
    //               this.cubes[i].name = "Cube: " + i;
    //           }
    //           this.Renderers.Add(r);

    //           if (hiddenObjects.Contains(i)) {
    //               r.enabled = false;
    //           }
    //       }


    //       this.total = f;
    //       this.count = n;
    //       this.frame = 0;

    //       if (this.recordAABBs) {
    //           string file = "";
    //           file = Path.GetFileName(this.recordingFile);
    //           file = Path.ChangeExtension(file, "box");
    //           RecorderAABBs.instance.StartFile(file, (int)n, this.world.transform.GetChild(0));
    //           RecorderAABBs.instance.WriteHeader(this.Renderers);
    //       }
    //       if (this.recordMovie) {
    //           RecorderMovie.instance.StartMovie(true);
    //           if (this.recordMovie) {
    //               RecorderMovie.instance.RecordFrame();
    //           }
    //       }
    //       this.statics_count = 0;
    //   }

    //   struct V31 {
    //       public Vector3 v;
    //       public float pad;
    //   }
    //   void StepVisualization() {
    //       if (this.statics_count == this.count) {
    //           return;
    //       }

    //       Profiler.BeginSample("Step Visualization");
    //       Profiler.BeginSample("Lendo Buffer");
    //       var rawPos_bytes = this.buffer.ReadBytes((int)this.count * 16);
    //       var rawRot_bytes = this.buffer.ReadBytes((int)this.count * 16);

    //       Profiler.EndSample();
    //       Profiler.BeginSample("Convertendo");


    //       unsafe {
    //           Vector3 margin = 0.025f * Vector3.one;
    //           fixed (byte* pos_bytes = rawPos_bytes) {
    //               fixed (byte* rot_bytes = rawRot_bytes) {
    //                   V31* pos_data = (V31*)pos_bytes;
    //                   Quaternion* rot_data = (Quaternion*)rot_bytes;

    //                   for (int i = 0; i < this.count; i++) {
    //                       if (this.frame < 10 || !this.correctFreeFall) {
    //                           this.cubes[i].transform.position = pos_data[i].v;
    //                           this.cubes[i].transform.rotation = rot_data[i];
    //                           this.statics[i] = false;
    //                           this.timeToStatics[i] = 0;
    //                           this.oldPos[i] = this.cubes[i].transform.position;
    //                           this.oldRot[i] = this.cubes[i].transform.rotation;
    //                       } else {
    //                           if (this.statics[i]) {
    //                               continue;
    //                           }

    //                           Renderer r = this.cubes[i].GetComponent<Renderer>();
    //                           this.cubes[i].transform.position = this.oldPos[i];
    //                           this.cubes[i].transform.rotation = this.oldRot[i];
    //                           Bounds b1 = r.bounds;
    //                           this.cubes[i].transform.position = pos_data[i].v;
    //                           this.cubes[i].transform.rotation = rot_data[i];
    //                           Bounds b2 = r.bounds;

    //                           Vector3 aMin = b1.center - b1.extents - margin;
    //                           Vector3 aMax = b1.center + b1.extents + margin;
    //                           Vector3 bMin = b2.center - b2.extents;
    //                           Vector3 bMax = b2.center - b2.extents;
    //                           Vector3 t1 = aMin - bMin;
    //                           Vector3 t2 = bMax - aMax;
    //                           if (t1.x <= 0 && t1.y <= 0 && t1.z <= 0 &&
    //                               t2.x <= 0 && t2.y <= 0 && t2.z <= 0) {

    //                               if (this.frame == this.timeToStatics[i]) {
    //                                   this.statics[i] = true;
    //                                   this.statics_count++;
    //                                   r.material = this.staticsMaterial;
    //                               } else if (this.timeToStatics[i] == 0) {
    //                                   this.timeToStatics[i] = this.frame + 25;
    //                               }
    //                           } else {
    //                               this.timeToStatics[i] = 0;
    //                               this.oldPos[i] = this.cubes[i].transform.position;
    //                               this.oldRot[i] = this.cubes[i].transform.rotation;
    //                           }
    //                       }
    //                   }
    //               }
    //           }
    //       }
    //       Profiler.EndSample();
    //       Profiler.EndSample();
    //   }


    //void FixedUpdate () {
    //       if (this.Finished) {
    //           return;
    //       }

    //       if (this.cubes == null) {
    //           this.Create();
    //       } else {
    //           this.frame++;
    //           Debug.Log(this.frame + " ---- " + this.statics_count);
    //           if (this.frame < this.total) {
    //               this.StepVisualization();
    //               if (this.recordAABBs) {
    //                   RecorderAABBs.instance.WriteFrame(this.Renderers);
    //               }
    //               if (this.recordMovie) {
    //                   RecorderMovie.instance.RecordFrame();
    //               }
    //           } else {
    //               Debug.Log("Cleaning Up " + this.recordingFile + "... (T = " + Time.time + ")");
    //               for (int i = 0; i < this.count; i++) {
    //                   Object.Destroy(this.cubes[i]);
    //               }
    //               Object.Destroy(this.world);
    //               if (this.recordAABBs) {
    //                   RecorderAABBs.instance.WriteFrame(this.Renderers);
    //                   RecorderAABBs.instance.EndFile();
    //               }
    //               this.Finished = true;
    //           }
    //       }
    //   }

    //   private float now() {
    //       return Time.time - this.beginMark;
    //   }

    //   private void OnGUI() {
    //       GUI.skin = this.skin;

    //       GUILayout.BeginVertical();
    //       GUILayout.Label("            ");
    //       GUILayout.Label("  Scenario: " + this.recordingFile);
    //       GUILayout.Label("World Size: " + this.worldSideLength);
    //       GUILayout.Label("    Frames: " + this.total);
    //       GUILayout.Label("   Current: " + this.frame);
    //       GUILayout.Label("   Statics: " + this.statics_count);
    //   }
}