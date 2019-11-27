using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;
using System.Runtime.InteropServices;
using System;

public class NetworkManager : MonoBehaviour
{
    public static NetworkManager _instance = null;
    
    public static NetworkManager Instance
    {
        get
        {
            if (_instance == null)
            {
                _instance = GameObject.FindObjectOfType<NetworkManager>();
                if (_instance == null)
                {
                    GameObject go = new GameObject("NetworkManager");
                    _instance = go.AddComponent<NetworkManager>();
                }
            }
            return _instance;
        }
    }

    private void Awake()
    {
        DontDestroyOnLoad(this.gameObject);
    }
    // Use this for initialization
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
    }

}