using System.Collections;
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

    public IEnumerator MakeGetRequest(string uri)
    {

        Debug.Log("Get Request: " + uri);
        UnityWebRequest req = new UnityWebRequest(uri);
        req.SetRequestHeader("UserID", "1");

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
            Debug.Log("Success: Response Received");
            PlayerDiedResponseData myJSON = JsonUtility.FromJson<PlayerDiedResponseData>(req.downloadHandler.text);
            Debug.Log(myJSON.UserID);
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
            Debug.Log("Respnonse Received: " + req.downloadHandler.text);
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