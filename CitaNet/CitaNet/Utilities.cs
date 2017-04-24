using System.Net;
using System.Net.Sockets;
using System.Diagnostics;

namespace CitaNet
{
    enum MessageType
    {
        Ack = 0,
        Connect,
        Disconnect,
        Update
    }

    class UdpState
    {
        public IPEndPoint endpoint;
        public UdpClient client;
    }

    static class Time
    {
        private static Stopwatch watch;
        private static long lastTime = 0L;

        static Time()
        {
            watch = new Stopwatch();
            watch.Start();
        }

        public static long time
        {
            get
            {
                return watch.ElapsedMilliseconds;
            }
        }

        public static long deltaTime
        {
            get
            {
                long result = watch.ElapsedMilliseconds - lastTime;
                lastTime = watch.ElapsedMilliseconds;
                return result;
            }
        }
    }
}
