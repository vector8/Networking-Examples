using System.Runtime.InteropServices;

public static class CitaNetWrapper
{
    private const string DLL_NAME = "CitaNet";

    [DllImport(DLL_NAME)]
    public static extern void initialize(int port, string serverAddress);

    [DllImport(DLL_NAME)]
    public static extern void sendMsg(string msg);

    [DllImport(DLL_NAME)]
    public static extern bool hasReceived();

    [DllImport(DLL_NAME)]
    private static extern System.IntPtr getLastReceived();

    [DllImport(DLL_NAME)]
    public static extern bool hasError();

    [DllImport(DLL_NAME)]
    private static extern System.IntPtr getError();

    [DllImport(DLL_NAME)]
    public static extern void cleanUp();

    public static string getLastReceivedMessage()
    {
        return Marshal.PtrToStringAnsi(getLastReceived());
    }

    public static string getErrorMessage()
    {
        return Marshal.PtrToStringAnsi(getError());
    }
}
