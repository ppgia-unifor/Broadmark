// SimulationGenerator - PhysicsEngine.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using UnityEngine;


public abstract class PhysicsEngine : IDisposable {
    public abstract void Initialize(SimulationParameters parameters);
    public abstract void SetGravity(Vector3 gravity);
    public abstract PhysicsBody CreateBody(GameObject obj);
    public abstract void AddColliders(GameObject obj, params Bounds[] aabbs);
    public abstract void Dispose();

    public void Step(float step, int substeps) {
        this.BeginStep();

        substeps = Math.Max(1, substeps);
        for (int i = 0; i < substeps; i++) {
            this.Substep(step / substeps);
        }

        this.EndStep();
    }


    protected abstract void BeginStep();
    protected abstract void Substep(float step);
    protected abstract void EndStep();
}