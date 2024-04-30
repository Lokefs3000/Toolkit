using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Toolkit.Core.Interop
{
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    internal struct InteropCall
    {
        public readonly IntPtr NamePtr;
        public readonly IntPtr FieldPtr;
        public readonly IntPtr FuncPtr;

        public string? Name => Marshal.PtrToStringAuto(NamePtr);
        public string? Field => Marshal.PtrToStringAuto(FieldPtr);
    }

    internal static class CallSystem
    {
        [UnmanagedCallersOnly(EntryPoint = "CallSystem_PushCalls")]
        public static void PushCalls(IntPtr calls, int size)
        {
            InteropCall[] interopCall;
            unsafe
            {
                Span<InteropCall> span = new Span<InteropCall>(calls.ToPointer(), size);
                interopCall = span.ToArray();
            }

            for (int i = 0; i < interopCall.Length; i++)
            {
                InteropCall call = interopCall[i];
                string? name = call.Name;

                if (name == null)
                {
                    Logger.Error("Interop call name is null!");
                    continue;
                }

                Type? type = AssemblyLoader.GetType(name, true, false);

                if (type == null)
                {
                    Logger.Error("Failed to get type for interop call!");
                    continue;
                }

                FieldInfo? field = type.GetFields(BindingFlags.NonPublic | BindingFlags.Static).FirstOrDefault(field => field.Name == call.Field);

                if (field == null)
                {
                    Logger.Error("Failed to find field for interop call!");
                    continue;
                }

                if (!field.FieldType.IsFunctionPointer)
                {
                    Logger.Error("Cannot set non-function pointer field!");
                    continue;
                }

                field.SetValue(null, call.FuncPtr);
            }
        }
    }
}
