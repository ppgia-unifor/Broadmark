// SimulationGenerator - Factory.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;


public static class Factory {
    public static PhysicsEngine GetPhysicsEngine(PhysicsEngineType type = PhysicsEngineType.PhysX) {
        switch (type) {
            case PhysicsEngineType.PhysX: return new PhysxEngine();
            case PhysicsEngineType.Bullet: return new BulletEngine();
            case PhysicsEngineType.Viewer: return new ViewerEngine();
            case PhysicsEngineType.Splitter: return new SplitterEngine();
            default: throw new ArgumentOutOfRangeException(nameof(type), type, null);
        }
    }

    public static Simulation GetSimulation(SimulationType type) {
        switch (type) {
            case SimulationType.FreeFall: return new FreeFall();
            case SimulationType.Brownian: return new Brownian();
            case SimulationType.Gravity: return new Gravity();
            default: throw new ArgumentOutOfRangeException(nameof(type), type, null);
        }
    }
}