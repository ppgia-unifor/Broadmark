// SimulationGenerator - PhysxEngine.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using BulletSharp;
using BulletUnity;
using UnityEngine;
using Vector3 = UnityEngine.Vector3;
using btVector3 = BulletSharp.Math.Vector3;


public class BulletEngine : PhysicsEngine {
    private SimulationParameters parameters;
    private DefaultCollisionConfiguration CollisionConf;
    private CollisionDispatcher Dispatcher;
    private DbvtBroadphase Broadphase;
    private DiscreteDynamicsWorld World;

    public override void Initialize(SimulationParameters parameters) {
        this.parameters = parameters;
        this.CollisionConf = new DefaultCollisionConfiguration();
        this.Dispatcher = new CollisionDispatcher(this.CollisionConf);
        this.Broadphase = new DbvtBroadphase {
            DeferredCollide = true
        };
        
        this.World = new DiscreteDynamicsWorld(this.Dispatcher, this.Broadphase, null, this.CollisionConf) {
            Gravity = new btVector3(0, parameters.Gravity, 0)
        };
        
        this.World.ConstraintSolver = new NncgConstraintSolver();
        this.World.SynchronizeAllMotionStates = true;
    }

    public override void SetGravity(Vector3 gravity) {
        btVector3 g = new btVector3(gravity.x, gravity.y, gravity.z);
        this.World.SetGravity(ref g);
    }

    public override PhysicsBody CreateBody(GameObject obj) {
        BulletBody bb = obj.AddComponent<BulletBody>();
        bb.world = this.World;
        return bb;
    }

    public override void AddColliders(GameObject obj, params Bounds[] aabbs) {
        BulletSharp.Math.Quaternion q = new BulletSharp.Math.Quaternion();
        foreach (Bounds aabb in aabbs) {
            Vector3 c = aabb.center * this.parameters.WorldSideLength;
            Vector3 e = aabb.extents * this.parameters.WorldSideLength;
            CollisionObject co = new CollisionObject {
                WorldTransform = BulletSharp.Math.Matrix.AffineTransformation(1f, q, new btVector3(c.x, c.y, c.z)),
                CollisionShape = new BoxShape(e.x, e.y, e.z)
            };

            this.World.AddCollisionObject(co);


            BoxCollider bc = obj.AddComponent<BoxCollider>();
            bc.center = aabb.center;
            bc.size = aabb.size;
        }
    }

    public override void Dispose() {
        if (this.World != null) {
            //remove/dispose constraints
            for (int i = this.World.NumConstraints - 1; i >= 0; i--) {
                TypedConstraint constraint = this.World.GetConstraint(i);
                this.World.RemoveConstraint(constraint);
                constraint.Dispose();
            }

            //remove the rigidbodies from the dynamics world and delete them
            for (int i = this.World.NumCollisionObjects - 1; i >= 0; i--) {
                CollisionObject obj = this.World.CollisionObjectArray[i];
                RigidBody body = obj as RigidBody;
                body?.MotionState?.Dispose();

                this.World.RemoveCollisionObject(obj);
                obj.Dispose();
            }
            
            this.World.Dispose();
            this.Broadphase.Dispose();
            this.Dispatcher.Dispose();
            this.CollisionConf.Dispose();
        }

        this.Broadphase?.Dispose();
        this.Dispatcher?.Dispose();
        this.CollisionConf?.Dispose();
    }


    protected override void BeginStep() { }

    protected override void Substep(float step) {
        this.World.StepSimulation(step, 0, step);
    }

    protected override void EndStep() { }
}