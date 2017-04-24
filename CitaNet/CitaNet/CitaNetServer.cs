using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

namespace CitaNet
{
    class CitaNetServer
    {
        private Dictionary<string, CitaClient> clients = new Dictionary<string, CitaClient>();
        private Dictionary<int, Message> messagesAwaitingAck = new Dictionary<int, Message>();
        private IPEndPoint endpoint;
        private UdpClient server;
        private UdpState state = new UdpState();
        private bool running = true;

        static void Main(string[] args)
        {
            CitaNetServer s = new CitaNetServer(8888);
            s.run();
        }

        CitaNetServer(int port)
        {
            endpoint = new IPEndPoint(IPAddress.Any, port);
            server = new UdpClient(endpoint);

            state.client = server;
            state.endpoint = endpoint;
        }

        public void run()
        {
            server.BeginReceive(new AsyncCallback(receiveCallback), state);
            Console.WriteLine("CitaNet server started up, waiting for client connections...");

            while (running)
            {
                float elapsed = Time.deltaTime;

                foreach (Message m in messagesAwaitingAck.Values)
                {
                    m.timeOut -= elapsed;
                    if (m.timeOut <= 0)
                    {
                        m.timeOut = 1000;
                        sendMessage(m, false);
                    }
                }
            }
        }

        private void receiveCallback(IAsyncResult ar)
        {
            UdpClient client = ((UdpState)ar.AsyncState).client;
            IPEndPoint endpoint = ((UdpState)ar.AsyncState).endpoint;

            Byte[] receivedBytes = client.EndReceive(ar, ref endpoint);

            // do something with receivedBytes
            if (receivedBytes.Length > 0)
            {
                MessageType mType = (MessageType)receivedBytes[0];

                switch (mType)
                {
                    case MessageType.Ack:
                        {
                            int messageID = BitConverter.ToInt32(receivedBytes, 1);
                            
                            if(messagesAwaitingAck.ContainsKey(messageID))
                            {
                                messagesAwaitingAck.Remove(messageID);
                            }
                        }
                        break;
                    case MessageType.Connect:
                        if (clients.ContainsKey(endpoint.Address.ToString()))
                        {
                            Console.WriteLine("Warning: Client " + endpoint.Address.ToString() + " tried to connect, but it is already connected. Sending ack anyway.");
                            int messageID = BitConverter.ToInt32(receivedBytes, 1);
                            sendAck(endpoint, messageID);
                        }
                        else
                        {
                            LinkedList<byte> msgBytes = new LinkedList<byte>();
                            msgBytes.AddLast((byte)(MessageType.Connect));
                            foreach(byte b in endpoint.Address.GetAddressBytes())
                            {
                                msgBytes.AddLast(b);
                            }

                            Message m = new Message();
                            m.contents = new byte[msgBytes.Count];
                            msgBytes.CopyTo(m.contents, 0);

                            broadcastMessage(m);

                            CitaClient c = new CitaClient(endpoint);
                            clients.Add(endpoint.Address.ToString(), c);

                            Console.WriteLine("Client " + endpoint.Address.ToString() + " connected.");

                            // bytes 1-4 are message id
                            int messageID = BitConverter.ToInt32(receivedBytes, 1);
                            sendAck(endpoint, messageID);
                        }
                        break;
                    case MessageType.Disconnect:
                        if (!clients.ContainsKey(endpoint.Address.ToString()))
                        {
                            Console.WriteLine("Warning: Client at " + endpoint.Address.ToString() + " tried to disconnect from server, but was never connected.");
                            int messageID = BitConverter.ToInt32(receivedBytes, 1);
                            sendAck(endpoint, messageID);
                        }
                        else
                        {
                            clients.Remove(endpoint.Address.ToString());

                            int messageID = BitConverter.ToInt32(receivedBytes, 1);
                            sendAck(endpoint, messageID);
                        }
                        break;
                    case MessageType.Update:
                        break;
                    default:
                        break;
                }
            }

            server.BeginReceive(new AsyncCallback(receiveCallback), state);
        }

        private void sendAck(IPEndPoint endpoint, int messageID)
        {
            byte[] response = new byte[5];
            response[0] = (byte)(MessageType.Ack);
            Array.Copy(BitConverter.GetBytes(messageID), 0, response, 1, 4);
            server.SendAsync(response, response.Length, endpoint);
        }

        private void sendMessage(Message m, bool addToAckWaitingList)
        {
            server.SendAsync(m.contents, m.contents.Length, m.endpoint);
            
            if(addToAckWaitingList)
            {
                int messageID = BitConverter.ToInt32(m.contents, 1);
                messagesAwaitingAck.Add(messageID, m);
            }
        }

        // send message to all clients
        private void broadcastMessage(Message m)
        {
            foreach(CitaClient c in clients.Values)
            {
                server.SendAsync(m.contents, m.contents.Length, c.endpoint);

                int messageID = BitConverter.ToInt32(m.contents, 1);
                messagesAwaitingAck.Add(messageID, m);
            }
        }
    }
}
