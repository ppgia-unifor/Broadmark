// SimulationGenerator - SplitterEngine.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using System.IO;
using UnityEngine;


public class SplitterEngine : ViewerEngine {
    public override void Initialize(SimulationParameters parameters) {
        base.Initialize(parameters);

        parameters.N *= 8;
    }

    private Bounds currentBounds;
    private int currentSubBox;
    public override PhysicsBody CreateBody(GameObject obj) {
        NullBody body = obj.AddComponent<NullBody>();
        if (this.fs == null || this.br == null) {
            return body;
        }

        if (this.currentSubBox == 0) {
            this.currentBounds = this.br.ReadBounds();
        }

        Vector3 pos = body.transform.position * 2;
        Quaternion rot = body.transform.rotation;
        pos += rot * this.getOffset(this.currentBounds.extents, this.currentSubBox);

        body.SetBounds(this.currentBounds);
        body.SetPositionAndRotation(pos, body.transform.rotation);
        this.bodies.Add(body);
        this.currentSubBox = (this.currentSubBox + 1) % 8;

        return body;
    }

    protected override void BeginStep() {
        if (this.fs == null || this.br == null) {
            return;
        }

        Vector3 readPos = Vector3.zero;
        Quaternion readQuat = Quaternion.identity;
        for (int i = 0; i < this.bodies.Count; i++) {
            if (i % 8 == 0) {
                readPos = this.br.ReadVector3() * 2;
                readQuat = this.br.ReadQuaternion();
            }

            Vector3 extents = this.bodies[i].Renderer.bounds.extents;
            Vector3 pos = readPos + readQuat * this.getOffset(extents, i);
            this.bodies[i].SetPositionAndRotation(pos, readQuat);
        }
    }

    private Vector3 getFectors(int n) {
        n %= 8;

        switch (n) {
            case 0: return new Vector3(-1, -1, -1);
            case 1: return new Vector3(-1, -1, +1);
            case 2: return new Vector3(-1, +1, -1);
            case 3: return new Vector3(-1, +1, +1);
            case 4: return new Vector3(+1, -1, -1);
            case 5: return new Vector3(+1, -1, +1);
            case 6: return new Vector3(+1, +1, -1);
            case 7: return new Vector3(+1, +1, +1);
        }

        throw new Exception();
    }

    private Vector3 getOffset(Vector3 extents, int n) {
        extents.Scale(this.getFectors(n));
        return extents;
    }
}