// SimulationGenerator - NullBody.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using UnityEngine;


public class NullBody : PhysicsBody {
    public override void Setup(ObjectsType type, float mass) { }

    public override void SetActivationState(bool state) { }

    public override bool GetSleepingState() {
        return false;
    }

    public override bool CanSleep() {
        return false;
    }
    public override void SetSpeed(Vector3 speed) { }

    public override void Dispose() { }
}