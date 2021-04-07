using Microsoft.AspNetCore;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;
using System;
using System.IO;

namespace SynthWebsocketsServer
{

    public class Program
    {
        public static void Main(string[] args)
        {
            CreateWebHostBuilder(args).Build().Run();
        }

        public static IWebHostBuilder CreateWebHostBuilder(string[] args)
        {
            var config = new ConfigurationBuilder()
            .SetBasePath(Directory.GetCurrentDirectory())
            .Build();

            return WebHost.CreateDefaultBuilder(args)
                .ConfigureLogging(logging =>
                {
                    //logging.ClearProviders();
                    //logging.AddConsole();
                })
                .UseUrls("http://*:1235")
                .UseConfiguration(config)
                .UseStartup<Startup>();
        }
                
  
    }
}
