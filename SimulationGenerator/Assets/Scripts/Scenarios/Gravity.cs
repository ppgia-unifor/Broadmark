// SimulationGenerator - Gravity.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using UnityEngine;


public class Gravity : Simulation {
    public Gravity() : base(SimulationType.Gravity) { }

    public float GravityFrequency = 6.2f;
    public float GravityScale = 4;
    public float step;

    private Vector3 axis1;
    private Vector3 axis2;
    private Vector3 axis3;

    public override void Start(SimulationParameters parameters) {
        base.Start(parameters);

        this.axis1 = new Vector3(0, 0, 1);
        this.axis2 = new Vector3(0, 1, 0);
        this.axis3 = new Vector3(1, 0, 0);
        this.step = 360 * this.Parameters.FixedTimestep / this.GravityFrequency;

        this.Engine.SetGravity(this.axis3 * this.Parameters.Gravity * this.GravityScale);
    }

    public override void Step(int frame) {
        base.Step(frame);

        this.axis1 = new Vector3(0, 0, 1);
        this.axis2 = Quaternion.AngleAxis(this.step, this.axis1) * this.axis2;
        this.axis3 = Quaternion.AngleAxis(this.step, this.axis2) * this.axis3;

        this.Engine.SetGravity(this.axis3 * this.Parameters.Gravity * this.GravityScale);

        #region Dealing with objects that left the World Box by teleporting them to the middle
        Bounds worldBounds = new Bounds(Vector3.zero, Vector3.one * this.Parameters.WorldSideLength);
        foreach (PhysicsBody body in this.Bodies) {
            if (!worldBounds.Contains(body.Bounds.center)) {
                body.SetPositionAndRotation(UnityEngine.Random.insideUnitSphere * this.Parameters.WorldSideLength * 0.5f, body.transform.rotation);
            }
        } 
        #endregion
    }

    public override void OnGUI() {
        base.OnGUI();

        GUILayout.Label($"{"Gravity".PadLeft(22)}: {this.axis3 * this.GravityScale}");
        GUILayout.Label($"{"Axis 1".PadLeft(22)}: {this.axis1}");
        GUILayout.Label($"{"Axis 2".PadLeft(22)}: {this.axis2}");
        GUILayout.Label($"{"Axis 3".PadLeft(22)}: {this.axis3}");
    }
}