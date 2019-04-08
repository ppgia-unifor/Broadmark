// SimulationGenerator - PhysicsBody.cs (2019)
// Copyright: Ygor Rebou�as Serpa


using System;
using System.Collections.Generic;
using UnityEngine;


public abstract class PhysicsBody : MonoBehaviour, IDisposable {
    public Bounds Bounds => this.Renderer.bounds;
    public MeshRenderer Renderer { get; private set; }

    public abstract void Dispose();

    private void Awake() {
        this.Renderer = this.GetComponent<MeshRenderer>();
    }

    public abstract void Setup(ObjectsType type, float mass);
    public abstract void SetActivationState(bool state);
    public abstract bool GetSleepingState();
    public abstract bool CanSleep();
    public abstract void SetSpeed(Vector3 speed);

    public virtual void SetBounds(Bounds bounds) {
        this.transform.position = bounds.center;
        this.transform.rotation = Quaternion.identity;
        this.transform.localScale = Vector3.one;
        this.transform.localScale = new Vector3(
            bounds.size.x / this.transform.lossyScale.x,
            bounds.size.y / this.transform.lossyScale.y,
            bounds.size.z / this.transform.lossyScale.z);
    }

    public virtual void SetPositionAndRotation(Vector3 position, Quaternion rotation) {
        this.transform.position = position;
        this.transform.rotation = rotation;
    }
}