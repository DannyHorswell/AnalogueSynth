using Microsoft.AspNetCore.Mvc;
using System;
using System.Collections.Generic;
using System.Text;


namespace SynthWebsocketsServer.Controllers
{
    public class SynthControlController : Controller
    {
        [Route("")]
        public IActionResult Get()
        {
            return new OkResult();
        }
    }
}
