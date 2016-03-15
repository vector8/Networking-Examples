using System.Net;
using System.Collections.Generic;

namespace CitaNet
{
    class Message
    {
        public byte[] contents;
        public float timeOut;
        public IPEndPoint endpoint;
    }
}
