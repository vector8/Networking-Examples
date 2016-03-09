using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

public class BoidNetworkManager : MonoBehaviour
{
    public bool initialized { get; private set; }

    public BoidFlock flock;

    private int port;
    private string serverAddress;
    private bool retryInit = false;

    void Start()
    {
        initialized = false;
        Application.runInBackground = true;
    }

    void Update()
    {
        if (!initialized && retryInit)
        {
            // retry initialization
            initialize(port, serverAddress, retryInit);
        }
        else if (initialized)
        {
            if (hasNewMessage())
            {
                // parse the message here
                parseReceivedMessage();
            }
        }
    }

    private void checkErrors()
    {
        if (CitaNetWrapper.hasError())
        {
            Debug.Log(CitaNetWrapper.getErrorMessage());
        }
    }

    public void initialize(int port, string serverAddress, bool retry)
    {
        retryInit = retry;
        if (initialized)
        {
            // clean up in case we had already initialized
            CitaNetWrapper.cleanUp();
        }

        this.port = port;
        this.serverAddress = serverAddress;
        CitaNetWrapper.initialize(port, serverAddress);
        if (CitaNetWrapper.hasError())
        {
            Debug.Log(CitaNetWrapper.getErrorMessage());
            CitaNetWrapper.cleanUp();
        }
        else
        {
            initialized = true;
            flock.gameObject.SetActive(true);
        }
    }

    public bool hasNewMessage()
    {
        bool result = CitaNetWrapper.hasReceived();
        checkErrors();
        return result;
    }

    public void parseReceivedMessage()
    {
        string receivedMsg = CitaNetWrapper.getLastReceivedMessage();
        checkErrors();

        //print("Received: " + receivedMsg);

        string[] tokens = receivedMsg.Split('@');

        int flockCount = tokens.Length / 6;

        //print(tokens.Length.ToString() + " floats received, so " + flockCount + " boids.");

        //print("Last flock count: " + flock.remoteFlock.Count + " New flock count: " + flockCount);

        if (flockCount == flock.remoteFlock.Count)
        {
            // update existing boids
            for (int i = 0; i < flockCount; i++)
            {
                flock.remoteFlock[i].transform.position = new Vector3(float.Parse(tokens[i * 6]), float.Parse(tokens[i * 6 + 1]), float.Parse(tokens[i * 6 + 2]));
                flock.remoteFlock[i].velocity = new Vector3(float.Parse(tokens[i * 6 + 3]), float.Parse(tokens[i * 6 + 4]), float.Parse(tokens[i * 6 + 5]));
                flock.remoteFlock[i].id = i;
                // turn the boid toward their velocity
                flock.remoteFlock[i].transform.LookAt(flock.remoteFlock[i].transform.position + Vector3.Normalize(flock.remoteFlock[i].velocity));
            }
        }
        else
        {
            // clear remote flock and remake
            for (int i = 0; i < flock.remoteFlock.Count; i++)
            {
                Destroy(flock.remoteFlock[i].gameObject);
            }
            flock.remoteFlock = new List<Boid>();

            for (int i = 0; i < flockCount; i++)
            {
                GameObject go = GameObject.Instantiate<GameObject>(flock.remotePrefab);
                go.transform.position = new Vector3(float.Parse(tokens[i * 6]), float.Parse(tokens[i * 6 + 1]), float.Parse(tokens[i * 6 + 2]));
                go.transform.SetParent(flock.remoteParent);
                Boid b = go.GetComponent<Boid>();
                b.velocity = new Vector3(float.Parse(tokens[i * 6 + 3]), float.Parse(tokens[i * 6 + 4]), float.Parse(tokens[i * 6 + 5]));
                b.id = i;
                // turn the boid toward their velocity
                b.transform.LookAt(b.transform.position + Vector3.Normalize(b.velocity));
                flock.remoteFlock.Add(b);
            }
        }
    }

    public void sendFlockToRemote(List<Boid> flock)
    {
        string msgToSend = "";

        for (int i = 0; i < flock.Count; i++)
        {
            msgToSend += flock[i].transform.position.x.ToString() + "@" + flock[i].transform.position.y.ToString() + "@" + flock[i].transform.position.z.ToString() + "@";
            msgToSend += flock[i].velocity.x.ToString() + "@" + flock[i].velocity.y.ToString() + "@" + flock[i].velocity.z.ToString() + "@";
        }

        msgToSend.Substring(0, msgToSend.Length - 1);

        CitaNetWrapper.sendMsg(msgToSend);
        checkErrors();
    }

    void OnApplicationQuit()
    {
        if (initialized)
        {
            CitaNetWrapper.cleanUp();
        }
    }
}
