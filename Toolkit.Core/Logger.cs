using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Toolkit.Core.Interop;

namespace Toolkit.Core
{
    public static class Logger
    {
        public static void Log(string message) => ManagedHost.LogMessage(message, ManagedHost.MessageLevel.Log);
        public static void Warn(string message) => ManagedHost.LogMessage(message, ManagedHost.MessageLevel.Warn);
        public static void Error(string message) => ManagedHost.LogMessage(message, ManagedHost.MessageLevel.Error);
    }
}
