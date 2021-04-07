using System;
using System.Collections.Generic;
using System.Text;

namespace SynthWebsocketsServer.Services
{
    public interface ISocketService
    {
        void SendMessage(string message);
    }
}
