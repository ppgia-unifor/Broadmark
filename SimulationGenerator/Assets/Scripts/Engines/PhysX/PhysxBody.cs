// SimulationGenerator - PhysxBody.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using UnityEngine;


public class PhysxBody : PhysicsBody {
    private Rigidbody body;

    public override void Setup(ObjectsType type, float mass) {
        this.body = this.gameObject.GetOrAddComponent<Rigidbody>();
        this.body.mass = mass;
        switch (type) {
            case ObjectsType.Cubes:
            case ObjectsType.Assorted:
                BoxCollider bc = this.gameObject.GetOrAddComponent<BoxCollider>();
                bc.center = Vector3.zero;
                bc.size = Vector3.one;
                break;
            case ObjectsType.Spheres:
                SphereCollider sc = this.gameObject.GetOrAddComponent<SphereCollider>();
                sc.center = Vector3.zero;
                sc.radius = 1;
                break;
            default: throw new ArgumentOutOfRangeException(nameof(type), type, null);
        }
    }

    public override void SetActivationState(bool state) {
        this.body.isKinematic = !state;
    }

    public override bool GetSleepingState() {
        return this.body.IsSleeping() || this.body.isKinematic;
    }

    public override bool CanSleep() {
        return this.body.velocity.magnitude < 0.5 && this.body.angularVelocity.magnitude < 3;
    }

    public override void SetSpeed(Vector3 speed) {
        this.body.velocity = speed;
    }

    public override void Dispose() { }
}