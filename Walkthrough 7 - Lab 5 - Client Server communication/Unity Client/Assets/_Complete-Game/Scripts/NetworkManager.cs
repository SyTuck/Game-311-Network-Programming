﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;
using System.Runtime.InteropServices;
using System;
using System.Text;

using Assets._Complete_Game.Scripts.Responses;
using Assets._Complete_Game.Scripts.Requests;

public class NetworkManager : MonoBehaviour
{
    public static NetworkManager _instance = null;

    public bool updated;
    public float value1;
    public float value2;
    public string message1;



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

    public IEnumerator MakeGetRequest(string uri, string user = "")
    {
        Debug.Log("Get Request: " + uri);
        UnityWebRequest req = new UnityWebRequest(uri);
        req.SetRequestHeader("UserID", "1");
        if (user != "")
        {
            req.SetRequestHeader("UserName", user);
        }

        DownloadHandlerBuffer dH = new DownloadHandlerBuffer();
        req.downloadHandler = dH;

        yield return req.SendWebRequest();
        Debug.Log("Response Code: " + req.responseCode);
        Debug.Log(req.downloadHandler.text);

        if ((req.isNetworkError) || (req.responseCode > 299))
        {
            Debug.LogWarning("Error: " + req.error);
        }
        else
        {
            Debug.Log("Success: Response Received" + req.downloadHandler.text);
            PlayerDiedResponseData myJSON = JsonUtility.FromJson<PlayerDiedResponseData>(req.downloadHandler.text);
        }
    }

    public IEnumerator PostRequestExample(string uri, string initials)
    {
        Debug.Log("Post Request: " + uri);
        LoginRequestsData myData = new LoginRequestsData();
        myData.PlayerName = initials;
        string DataAsJSON = JsonUtility.ToJson(myData);
        byte[] bytes = Encoding.ASCII.GetBytes(DataAsJSON);

        UnityWebRequest req = new UnityWebRequest(uri);
        UploadHandlerRaw uH = new UploadHandlerRaw(bytes);
        req.uploadHandler = uH;
        req.SetRequestHeader("Content-Type", "application/json");
        req.SetRequestHeader("X-men", "Night Crawler");
        req.method = UnityWebRequest.kHttpVerbPOST;

        DownloadHandlerBuffer dH = new DownloadHandlerBuffer();
        req.downloadHandler = dH;

        yield return req.SendWebRequest();
        Debug.Log("Response Code: " + req.responseCode);

        if (req.isNetworkError)
        {
            Debug.LogWarning("Error: " + req.error);
        }
        else
        {
            Debug.Log("Response Received: " + req.downloadHandler.text);
        }
    }
    public IEnumerator PostRequestHealth(string uri, int apples, int sodas)
    {
        updated = false;
        Debug.Log("Post Request: " + uri);
        FoodEatenRequestsData myData = new FoodEatenRequestsData();
        myData.appleCount = apples;
        myData.popCount = sodas;
        string DataAsJSON = JsonUtility.ToJson(myData);
        byte[] bytes = Encoding.ASCII.GetBytes(DataAsJSON);

        UnityWebRequest req = new UnityWebRequest(uri);
        UploadHandlerRaw uH = new UploadHandlerRaw(bytes);
        req.uploadHandler = uH;
        req.SetRequestHeader("Content-Type", "application/json");
        req.SetRequestHeader("X-Eats", "Food");
        req.method = UnityWebRequest.kHttpVerbPOST;

        DownloadHandlerBuffer dH = new DownloadHandlerBuffer();
        req.downloadHandler = dH;

        yield return req.SendWebRequest();
        Debug.Log("Response Code: " + req.responseCode);

        if (req.isNetworkError)
        {
            Debug.LogWarning("Error: " + req.error);
        }
        else
        {
            Debug.Log("Response Received: " + req.downloadHandler.text);
            FoodEatenResponseData myDiet = JsonUtility.FromJson<FoodEatenResponseData>(req.downloadHandler.text);
            value1 = myDiet.appleValue;
            value2 = myDiet.popValue;
            message1 = myDiet.healthMessage;
            // GameObject.Find("Player").GetComponent<Player>().pointsPerFood = value1;
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