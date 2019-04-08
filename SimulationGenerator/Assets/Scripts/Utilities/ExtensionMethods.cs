// SimulationGenerator - ExtensionMethods.cs (2019)
// Copyright: Ygor Rebouças Serpa


using System;
using System.Collections.Generic;
using System.IO;
using UnityEngine;


public static class ExtensionMethods {
    public static void WriteCStyleString(this BinaryWriter bw, string s, int size, char paddingChar = '\0') {
        char[] chars = s.ToCharArray();
        for (int i = 0; i < Math.Min(chars.Length, size - 1); i++) {
            bw.Write(chars[i]);
        }

        for (int i = chars.Length; i < size - 1; i++) {
            bw.Write(paddingChar);
        }

        // Make sure that the last byte is a null terminator
        bw.Write('\0');
    }

    public static string FromCStyleString(this char[] chars) {
        int nullchar = Array.IndexOf(chars, '\0');
        return new string(chars, 0, nullchar);
    }

    public static T GetOrAddComponent<T>(this GameObject obj) where T : Component {
        T component = obj.GetComponent<T>();
        return component != null ? component : obj.AddComponent<T>();
    }

    public static void Write(this BinaryWriter bw, Vector3 v) {
        bw.Write(v.x);
        bw.Write(v.y);
        bw.Write(v.z);
        bw.Write(0.0f);
    }

    public static void Write(this BinaryWriter bw, Quaternion q) {
        bw.Write(q.x);
        bw.Write(q.y);
        bw.Write(q.z);
        bw.Write(q.w);
    }

    public static void Write(this BinaryWriter bw, Bounds b) {
        bw.Write(b.min);
        bw.Write(b.max);
    }

    public static Vector3 ReadVector3(this BinaryReader br) {
        return new Vector4(br.ReadSingle(), br.ReadSingle(), br.ReadSingle(), br.ReadSingle());
    }

    public static Quaternion ReadQuaternion(this BinaryReader br) {
        return new Quaternion(br.ReadSingle(), br.ReadSingle(), br.ReadSingle(), br.ReadSingle());
    }

    public static Bounds ReadBounds(this BinaryReader br) {
        return new Bounds {
            min = br.ReadVector3(),
            max = br.ReadVector3()
        };
    }
}