// SimulationGenerator - FreeFall.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;


public class FreeFall : Simulation {
    private Material defaultMaterial;
    private Material staticMaterial;

    public FreeFall() : base(SimulationType.FreeFall) { }

    public override void Start(SimulationParameters parameters) {
        base.Start(parameters);

        if (this.Parameters.FreeFall_PaintStaticObjects) {
            this.defaultMaterial = this.Bodies[0].Renderer.material;
            this.staticMaterial = new Material(this.defaultMaterial) {
                color = new Color(0, 237 / 255f, 177 / 255f) / 4
            };
        }
    }

    public override void Step(int frame) {
        base.Step(frame);

        if (this.Parameters.Engine >= PhysicsEngineType.Viewer) {
            return;
        }

        if (this.Parameters.FreeFall_PaintStaticObjects) {
            foreach (PhysicsBody body in this.Bodies) {
                body.Renderer.sharedMaterial = !body.GetSleepingState() ? this.defaultMaterial : this.staticMaterial;
            }
        }

        if (this.Parameters.FreeFall_ForceStaticObjectsToKinematic) {
            if (frame > 140) {
                foreach (PhysicsBody body in this.Bodies.Where(b => b.CanSleep() || b.GetSleepingState())) {
                    body.SetActivationState(false);
                    body.gameObject.isStatic = true;
                }
            }
        }
    }

    public override void OnGUI() {
        base.OnGUI();

        GUILayout.Label($"{"# of Static Objects".PadLeft(22)}: {this.Bodies.Count(x => x.GetSleepingState()):##0.0}");
    }
}