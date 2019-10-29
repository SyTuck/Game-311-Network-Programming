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

    private int sessionToke;

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

    public IEnumerator GetRandomRequest(string uri, Action callback)
    {
        Debug.Log("Get Random Request");

        UnityWebRequest req = new UnityWebRequest(uri);
        req.SetRequestHeader("UserID", "1");

        DownloadHandlerBuffer dH = new DownloadHandlerBuffer();
        req.downloadHandler = dH;

        yield return req.SendWebRequest();
        Debug.Log("GRR Response Code: " + req.responseCode);
        Debug.Log("GRR data" + req.downloadHandler.text);

        if (req.isNetworkError)
        {
            Debug.Log("GRR Error: " + req.error);
        }
        else
        {
            Debug.Log("GRR Success: Response Received");
            PlayerInitResponseData myJSON = JsonUtility.FromJson<PlayerInitResponseData>(req.downloadHandler.text);
            Debug.Log(myJSON.RandVal);
            callback();
        }
    }

    public IEnumerator MakeGetRequest(string uri)
    {
        UnityWebRequest req = new UnityWebRequest(uri);
        req.SetRequestHeader("UserID", "1");

        DownloadHandlerBuffer dH = new DownloadHandlerBuffer();
        req.downloadHandler = dH;

        yield return req.SendWebRequest();
        Debug.Log("Response Code: " + req.responseCode);
        Debug.Log(req.downloadHandler.text);

        if (req.isNetworkError)
        {
            Debug.Log("Error: " + req.error);
        }
        else
        {
            Debug.Log("MGR Success: Response Received");
            PlayerDiedResponseData myJSON = JsonUtility.FromJson<PlayerDiedResponseData>(req.downloadHandler.text);
            Debug.Log(myJSON.UserID);
        }
    }

    public IEnumerator PostRequestExample(string uri, string initials)
    {
        LoginRequestsData myData = new LoginRequestsData();
        myData.Name = initials;
        string DataAsJSON = JsonUtility.ToJson(myData);
        byte[] bytes = Encoding.ASCII.GetBytes(DataAsJSON);

        UnityWebRequest req = new UnityWebRequest(uri);
        UploadHandlerRaw uH = new UploadHandlerRaw(bytes);
        req.uploadHandler = uH;
        req.SetRequestHeader("Content-Type", "application/json");        
        req.method = UnityWebRequest.kHttpVerbPOST;

        DownloadHandlerBuffer dH = new DownloadHandlerBuffer();
        req.downloadHandler = dH;

        yield return req.SendWebRequest();
        Debug.Log("Response Code: " + req.responseCode);

        if (req.isNetworkError)
        {
            Debug.Log("Error: " + req.error);
        }
        else
        {
            Debug.Log("Post Respnonse Received: " + req.downloadHandler.text);
            PlayerLoginResponseData myJSON = JsonUtility.FromJson<PlayerLoginResponseData>(req.downloadHandler.text);
            Debug.Log("ResponseStr = \"" + myJSON.ResponseStr + "\" " + "SessionToken = " + myJSON.SessionToken);
            sessionToke = myJSON.SessionToken;
        }
    }
    public IEnumerator PostUpdateScore(string uri, string initials, int level)
    {
        UpdateScoreRequestData myData = new UpdateScoreRequestData();
        myData.Name = initials;
        myData.Score = level;
        string DataAsJSON = JsonUtility.ToJson(myData);
        byte[] bytes = Encoding.ASCII.GetBytes(DataAsJSON);

        UnityWebRequest req = new UnityWebRequest(uri);
        UploadHandlerRaw uH = new UploadHandlerRaw(bytes);
        req.uploadHandler = uH;
        req.SetRequestHeader("Content-Type", "application/json");
        req.SetRequestHeader("Token", sessionToke.ToString());
        req.method = UnityWebRequest.kHttpVerbPOST;

        DownloadHandlerBuffer dH = new DownloadHandlerBuffer();
        req.downloadHandler = dH;

        yield return req.SendWebRequest();
        Debug.Log("PUS Response Code: " + req.responseCode);

        if (req.isNetworkError)
        {
            Debug.Log("PUS Error: " + req.error);
        }
        else
        {
            Debug.Log("PUS Respnonse Received: " + req.downloadHandler.text);
 //           UpdateScoreResponseData myJSON = JsonUtility.FromJson<UpdateScoreResponseData>(req.downloadHandler.text);
//            Debug.Log("PUS Score message: \"" + myJSON.ScoreStatus);
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