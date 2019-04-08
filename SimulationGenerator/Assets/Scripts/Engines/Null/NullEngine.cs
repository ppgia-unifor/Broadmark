// SimulationGenerator - NullEngine.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using System.IO;
using UnityEngine;


public class NullEngine : PhysicsEngine {
    public override void Initialize(SimulationParameters parameters) { }

    public override void SetGravity(Vector3 gravity) { }

    public override PhysicsBody CreateBody(GameObject obj) {
        return obj.GetOrAddComponent<NullBody>();
    }

    public override void AddColliders(GameObject obj, params Bounds[] aabbs) { }

    public override void Dispose() { }


    protected override void BeginStep() { }

    protected override void Substep(float step) { }

    protected override void EndStep() { }
}