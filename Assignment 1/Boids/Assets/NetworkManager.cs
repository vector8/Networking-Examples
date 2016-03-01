using UnityEngine;
using System.Collections;

public class NetworkManager : MonoBehaviour
{
    public bool initialized { get; private set; }
    private bool hasInitializationError = false;

    void Start()
    {
        initialized = false;
        Application.runInBackground = true;
    }

    public void initialize(int port, string serverAddress)
    {
        NetworkingWrapper.initialize(port, serverAddress);
        if(NetworkingWrapper.hasError())
        {
            Debug.Log(NetworkingWrapper.getErrorMessage());
            hasInitializationError = true;
        }
        else
        {
            initialized = true;
        }
    }

    void OnApplicationQuit()
    {
        if(initialized || hasInitializationError)
        {
            NetworkingWrapper.cleanUp();
        }
    }
}
