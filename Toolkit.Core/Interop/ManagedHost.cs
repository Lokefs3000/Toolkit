using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Toolkit.Core.Interop
{
    internal static class ManagedHost
    {
        private static unsafe delegate*<IntPtr, MessageLevel, void> _logMessage;

        [UnmanagedCallersOnly(EntryPoint = "ManagedHost_Initialize")]
        public static unsafe void Initialize(delegate*<IntPtr, MessageLevel, void> log)
        {
            _logMessage = log;

            Logger.Log("ManagedHost initialized!");
        }

        internal static unsafe void LogMessage(string message, MessageLevel level)
        {
            IntPtr str = Marshal.StringToCoTaskMemAuto(message);
            _logMessage(str, level);
            Marshal.FreeCoTaskMem(str);
        }

        internal enum MessageLevel
        {
            Log,
            Warn,
            Error
        }
    }
}
