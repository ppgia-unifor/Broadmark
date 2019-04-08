// SimulationGenerator - PhysxEngine.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using UnityEngine;


public class PhysxEngine : PhysicsEngine {
    public override void Initialize(SimulationParameters parameters) {
        Physics.defaultSolverIterations = parameters.SolverIterations;
        Physics.defaultSolverVelocityIterations = parameters.VelocityIterations;
        Physics.gravity = Vector3.down * parameters.Gravity;
        Physics.autoSimulation = false;

        Physics.RebuildBroadphaseRegions(new Bounds(Vector3.zero, Vector3.one * parameters.WorldSideLength * 1.1f), 4);
    }

    public override void SetGravity(Vector3 gravity) {
        Physics.gravity = gravity;
    }

    public override PhysicsBody CreateBody(GameObject obj) {
        return obj.AddComponent<PhysxBody>();
    }

    public override void AddColliders(GameObject obj, params Bounds[] aabbs) {
        foreach (Bounds aabb in aabbs) {
            BoxCollider bc = obj.AddComponent<BoxCollider>();
            bc.center = aabb.center;
            bc.size = aabb.size;
        }
    }

    public override void Dispose() { }


    protected override void BeginStep() { }

    protected override void Substep(float step) {
        Physics.Simulate(step);
    }

    protected override void EndStep() { }
}