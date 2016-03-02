using UnityEngine;
using UnityEngine.UI;
using System.Collections;

public class UIController : MonoBehaviour
{
    public BoidNetworkManager networkMgr;
    public InputField serverAddressField;
    public Button startBtn;
    public Toggle asServer;

    public void asServerChecked(bool value)
    {
        serverAddressField.interactable = !value;

        startBtn.interactable = (value || serverAddressField.text.Length > 0);
    }

    public void startButtonPressed()
    {
        string address = (asServer.isOn ? "" : serverAddressField.text);
        networkMgr.initialize(8888, address, true);
        gameObject.SetActive(false);
    }

    public void onServerAddressFieldChanged(string value)
    {
        startBtn.interactable = (serverAddressField.text.Length > 0);
    }
}
