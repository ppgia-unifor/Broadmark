// SimulationGenerator - PhysxBody.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using UnityEngine;
using BulletSharp;
using BulletSharp.Math;
using BulletUnity;
using Vector3 = UnityEngine.Vector3;
using Quaternion = UnityEngine.Quaternion;
using btQuaternion = BulletSharp.Math.Quaternion;
using btVector3 = BulletSharp.Math.Vector3;

public class BulletBody : PhysicsBody {
    public DiscreteDynamicsWorld world { get; set; }
    private RigidBody body;
    private Matrix worldTransform;

    private static BoxShape sharedBoxShape;
    private static SphereShape sharedSphereShape;
    public override void Setup(ObjectsType type, float mass) {
        CollisionShape shape;

        Vector3 scale = this.transform.localScale;
        Vector3 pos = this.transform.position;
        switch (type) {
            case ObjectsType.Cubes:
                if (BulletBody.sharedBoxShape == null) {
                    BulletBody.sharedBoxShape  = new BoxShape(scale.x, scale.y, scale.z);
                }

                shape = BulletBody.sharedBoxShape;
                break;
            case ObjectsType.Assorted:
                shape = new BoxShape(scale.x, scale.y, scale.z);
                break;
            case ObjectsType.Spheres:
                if (BulletBody.sharedSphereShape == null) {
                    BulletBody.sharedSphereShape = new SphereShape(1);
                }

                shape = BulletBody.sharedSphereShape;
                break;
            default: throw new ArgumentOutOfRangeException(nameof(type), type, null);
        }

        btVector3 localInertia;
        shape.CalculateLocalInertia(mass, out localInertia);
        RigidBodyConstructionInfo rbInfo = new RigidBodyConstructionInfo(mass, new BGameObjectMotionState(this.transform), shape, localInertia);

        this.body = new RigidBody(rbInfo);

        Matrix st = Matrix.Translation(new btVector3(pos.x, pos.y, pos.z));
        this.body.WorldTransform = st;
        this.world.AddRigidBody(this.body);
        rbInfo.Dispose();
    }

    public override void SetActivationState(bool state) {
        this.body.ForceActivationState(state ? ActivationState.ActiveTag : ActivationState.WantsDeactivation);
    }

    public override bool GetSleepingState() {
        return this.body.ActivationState != ActivationState.ActiveTag;
    }

    public override bool CanSleep() {
        return this.body.LinearVelocity.Length < 0.5 && this.body.AngularVelocity.Length < 3;
    }

    public override void SetSpeed(Vector3 speed) {
        this.body.LinearVelocity = new btVector3(speed.x, speed.y, speed.z);
    }

    public override void Dispose() {
        this.SetActivationState(false);
    }

    //private void Update() {
    //    this.body.GetWorldTransform(out this.worldTransform);
    //
    //    btVector3 pos = this.worldTransform.Origin;
    //    btQuaternion rot = this.worldTransform.Orientation;
    //    this.transform.position = new Vector3(pos.X, pos.Y, pos.Z);
    //    this.transform.rotation = new Quaternion(rot.X, rot.Y, rot.Z, rot.W);
    //}
}