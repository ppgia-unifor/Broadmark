// SimulationGenerator - Simulation.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering;
using Object = UnityEngine.Object;
using Random = UnityEngine.Random;


public class Simulation : IDisposable {
    public static string OutputRoot;
    public string Name => this.Type.ToString();
    public SimulationType Type { get; }
    public PhysicsEngine Engine { get; private set; }
    public Transform Scene { get; private set; }
    public List<PhysicsBody> Bodies { get; private set; }
    public SimulationParameters Parameters { get; private set; }
    public List<SimulationListener> Listeners { get; }


    public Simulation(SimulationType type) {
        this.Type = type;
        this.Listeners = new List<SimulationListener>();
    }

    public virtual void Start(SimulationParameters parameters) {
        this.Parameters = parameters;

        this.Engine = Factory.GetPhysicsEngine(this.Parameters.Engine);
        this.Engine.Initialize(this.Parameters);

        this.Bodies = new List<PhysicsBody>(this.Parameters.N);
        this.GenerateDefaultWorld();
        this.GenerateObjects(this.Parameters.ObjectsType);

        if (this.Parameters.RecordSimulations) {
            this.Listeners.Add(new AABBsRecorder());
            if (this.Parameters.Engine == PhysicsEngineType.PhysX) {
                this.Listeners.Add(new PositionRotationRecorder());
            }
        }

        if (this.Parameters.RecordMovie) {
            this.Listeners.Add(new MovieRecorder());
        }

        if (this.Parameters.N > 4000 && this.Parameters.Engine != PhysicsEngineType.Viewer) {
            foreach (PhysicsBody body in this.Bodies) {
                body.Renderer.lightProbeUsage = LightProbeUsage.Off;
                body.Renderer.reflectionProbeUsage = ReflectionProbeUsage.Off;
                body.Renderer.shadowCastingMode = ShadowCastingMode.Off;
                body.Renderer.receiveShadows = false;
            }
        }

        foreach (SimulationListener listener in this.Listeners) {
            listener.OnStart(this);
        }
    }

    public virtual void Step(int frame) {
        if (!this.Bodies.TrueForAll(b => b.GetSleepingState())) {
            this.Engine.Step(this.Parameters.FixedTimestep, this.Parameters.SubSteps);
        }

        foreach (SimulationListener listener in this.Listeners) {
            listener.OnStep(this);
        }
    }

    public virtual void Stop() {
        foreach (SimulationListener listener in this.Listeners) {
            listener.OnStop(this);
        }
    }

    public virtual void OnGUI() { }

    public virtual void Dispose() {
        foreach (PhysicsBody body in this.Bodies) {
            body.Dispose();
        }

        this.Bodies.Clear();
        this.Engine.Dispose();
        try {
            Object.Destroy(this.Scene.gameObject);
        } catch {
            // ignored
        }
    }


    private void GenerateDefaultWorld() {
        // The world size is computed to match the objects density needed
        this.Scene = Object.Instantiate(this.Parameters.ArenaPrefab, Vector3.zero, Quaternion.identity).transform;
        this.Scene.localScale = Vector3.one * this.Parameters.WorldSideLength;

        Camera camera = Camera.allCameras[0];
        camera.transform.position = new Vector3(0.57f, 0.20f, 0.6f) * this.Parameters.WorldSideLength * 3.0f;
        camera.transform.LookAt(this.Scene.transform.position - Vector3.up * this.Parameters.WorldSideLength * 0.15f, Vector3.up);

        #region Collider Setup
        const float margin = 0.5f;
        const float halfMargin = margin / 2.0f;
        const float doubleMargin = margin * 2.0f;

        Bounds bc1 = new Bounds();
        Bounds bc2 = new Bounds();
        bc1.center = new Vector3(0.5f + halfMargin, 0, 0);
        bc2.center = new Vector3(-0.5f - halfMargin, 0, 0);
        bc1.size = new Vector3(margin, 1.0f, 1.0f);
        bc2.size = new Vector3(margin, 1.0f, 1.0f);

        Bounds bc3 = new Bounds();
        Bounds bc4 = new Bounds();
        bc3.center = new Vector3(0, 0.5f + halfMargin, 0);
        bc4.center = new Vector3(0, -0.5f - halfMargin, 0);
        bc3.size = new Vector3(1.0f, margin, 1.0f);
        bc4.size = new Vector3(1.0f, margin, 1.0f);

        Bounds bc5 = new Bounds();
        Bounds bc6 = new Bounds();
        bc5.center = new Vector3(0, 0, 0.5f + halfMargin);
        bc6.center = new Vector3(0, 0, -0.5f - halfMargin);
        bc5.size = new Vector3(1.0f, 1.0f, margin);
        bc6.size = new Vector3(1.0f, 1.0f, margin);

        bc1.size += new Vector3(0, 1, 1) * doubleMargin;
        bc2.size += new Vector3(0, 1, 1) * doubleMargin;
        bc3.size += new Vector3(0, 0, 1) * doubleMargin;
        bc4.size += new Vector3(0, 0, 1) * doubleMargin;
        bc5.size += Vector3.zero;
        bc6.size += Vector3.zero;
        #endregion

        this.Engine.AddColliders(this.Scene.gameObject, bc1, bc2, bc3, bc4, bc5, bc6);
    }

    private void GenerateObjects(ObjectsType type) {
        Vector3 center = this.Scene.GetComponent<Renderer>().bounds.center;
        for (int i = 0; i < this.Parameters.N; i++) {
            Vector3 scale;
            if (type == ObjectsType.Cubes || type == ObjectsType.Spheres) {
                scale = Vector3.one * this.Parameters.ObjectScale;
            } else {
                scale = this.GenerateAssortedSize(i);
            }

            GameObject cube = Object.Instantiate(type != ObjectsType.Spheres ? this.Parameters.CubePrefab : this.Parameters.SpherePrefab);
            cube.transform.position = Random.insideUnitSphere * this.Parameters.MaxInitialPosition + center;
            cube.transform.rotation = Quaternion.identity;
            cube.transform.localScale = scale;
            cube.transform.parent = this.Scene;

            PhysicsBody pb = this.Engine.CreateBody(cube);
            pb.Setup(ObjectsType.Cubes, this.Parameters.ObjectMass);
            pb.SetSpeed(Random.insideUnitSphere * this.Parameters.MaxInitialVelocity);

            this.Bodies.Add(pb);
        }
    }

    private Vector3 GenerateAssortedSize(int i) {
        const float min = 0.1f;
        const float max = 1.9f;

        Vector3 output;
        int shape = i % 80;
        if (shape < 4) { // Standard Cubes
            output = Vector3.one * (5 * min);
        } else if (shape < 8) {
            output = Vector3.one * max;
        } else if (shape < 11) { // Bars
            output = new Vector3(max * 8, min * 4, min * 4);
        } else if (shape < 12) {
            output = new Vector3(min * 4, max * 8, min * 4);
        } else if (shape < 13) {
            output = new Vector3(min * 4, min * 4, max * 8);
        } else if (shape < 14) { // Planks
            output = new Vector3(max * 1, max * 3, min * 1);
        } else if (shape < 15) {
            output = new Vector3(max * 1, min * 1, max * 3);
        } else if (shape < 16) {
            output = new Vector3(min * 1, max * 1, min * 3);
        } else if (shape < 17) { // X Squares
            output = new Vector3(max * 2, max * 2, min * 1);
        } else if (shape < 18) {
            output = new Vector3(max * 2, min * 1, max * 2);
        } else if (shape < 19) {
            output = new Vector3(min * 1, max * 2, max * 2);
        } else { // Random Cubes
            const float factor = 0.803f;

            output = new Vector3(Random.value, Random.value, Random.value);
            output *= max - min;
            output += Vector3.one * min;
            output *= factor;
        }

        return output * this.Parameters.ObjectScale;
    }

    public override string ToString() {
        return this.Parameters.ToString();
    }
}