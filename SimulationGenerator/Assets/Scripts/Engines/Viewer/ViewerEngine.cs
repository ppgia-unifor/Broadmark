// SimulationGenerator - ViewerEngine.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using System.IO;
using UnityEngine;


public class ViewerEngine : NullEngine {
    protected List<PhysicsBody> bodies;
    protected FileStream fs;
    protected BinaryReader br;

    public override void Initialize(SimulationParameters parameters) {
        string dir = Path.Combine(Simulation.OutputRoot, "Recordings_PositionRotation/");
        string fileName = Path.Combine(dir, $"{parameters}.posrot");

        if (!File.Exists(fileName)) {
            Debug.Log($"Could not find '{fileName}'");
            return;
        }

        this.fs = new FileStream(fileName, FileMode.Open);
        this.br = new BinaryReader(this.fs);

        char[] name = this.br.ReadChars(128);
        char[] objects = this.br.ReadChars(128);
        int n = this.br.ReadInt32();
        int f = this.br.ReadInt32();

        this.br.ReadInt32(); // padding 1
        this.br.ReadInt32(); // padding 2

        Bounds worldBounds = this.br.ReadBounds();

        Debug.Assert(name.FromCStyleString() == parameters.Simulation.ToString());
        Debug.Assert(objects.FromCStyleString() == parameters.ObjectsType.ToString());
        Debug.Assert(n == parameters.N);
        Debug.Assert(f == parameters.Frames);
        Debug.Assert(Math.Abs(worldBounds.size.x - parameters.WorldSideLength) < 0.01f);

        Debug.Log($"Sucessfully found '{parameters}'");
        this.bodies = new List<PhysicsBody>(n);
    }

    public override PhysicsBody CreateBody(GameObject obj) {
        PhysicsBody body = base.CreateBody(obj);
        if (this.fs == null || this.br == null) {
            return body;
        }

        body.SetBounds(this.br.ReadBounds());
        this.bodies.Add(body);

        return body;
    }

    protected override void BeginStep() {
        if (this.fs == null || this.br == null) {
            return;
        }

        foreach (PhysicsBody body in this.bodies) {
            body.SetPositionAndRotation(this.br.ReadVector3(), this.br.ReadQuaternion());
        }
    }

    public override void Dispose() {
        base.Dispose();

        this.br?.Dispose();
        this.br = null;
        this.fs?.Dispose();
        this.fs = null;
    }
}