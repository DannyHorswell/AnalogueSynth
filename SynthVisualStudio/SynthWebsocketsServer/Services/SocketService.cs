using Microsoft.Extensions.Logging;
using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;

namespace SynthWebsocketsServer.Services
{
    public class SocketService : ISocketService
    {
        private readonly ILogger<SocketService> _logger;

        private TcpClient _tCPClient;
        private NetworkStream _networkStream;

        public SocketService(ILogger<SocketService> logger)
        {
            _logger = logger;

            _tCPClient = new TcpClient("127.0.0.1", 1234);
            _networkStream = _tCPClient.GetStream();
        }


        public void SendMessage(string message)
        {
            _logger.LogInformation($"Message received: {message}");

            var bytes = Encoding.Default.GetBytes(message);

            _networkStream.Write(bytes, 0, bytes.Length);
        }
    }
}
