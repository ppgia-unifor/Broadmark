// SimulationGenerator - Brownian.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using UnityEngine;
using Random = UnityEngine.Random;


public class Brownian : Simulation {
    public Brownian() : base(SimulationType.Brownian) { }

    [Header("Brownian")]
    [Tooltip("Number of frames needed to update all objects")]
    public float CyclePeriod = 100;
    public float ForceScale = 1.0f;
    protected int ObjectsPerFrame;
    protected int ObjectsUpdated;

    public override void Start(SimulationParameters parameters) {
        base.Start(parameters);

        this.ObjectsPerFrame = (int)Mathf.Ceil(this.Parameters.N / this.CyclePeriod);
        this.ObjectsUpdated = 0;
        this.Engine.SetGravity(Vector3.zero);
    }

    private Vector3 GetRandomSpeed() {
        return Random.insideUnitSphere * this.Parameters.MaxInitialVelocity * this.ForceScale;
    }

    public override void Step(int frame) {
        base.Step(frame);

        if (this.Parameters.Engine >= PhysicsEngineType.Viewer) {
            return;
        }

        for (int i = 0; i < this.ObjectsPerFrame; i++) {
            this.Bodies[this.ObjectsUpdated].SetSpeed(this.GetRandomSpeed());
            this.ObjectsUpdated = (this.ObjectsUpdated + 1) % this.Parameters.N;
        }

        #region Dealing with objects that left the World Box by teleporting them to the middle
        Bounds worldBounds = new Bounds(Vector3.zero, Vector3.one * this.Parameters.WorldSideLength);
        foreach (PhysicsBody body in this.Bodies) {
            if (!worldBounds.Contains(body.Bounds.center)) {
                body.SetPositionAndRotation(UnityEngine.Random.insideUnitSphere * this.Parameters.WorldSideLength, body.transform.rotation);
            }
        }
        #endregion
    }
}