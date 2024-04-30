using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Toolkit.Core.Interop
{
    [StructLayout(LayoutKind.Sequential)]
    internal struct NativeString : IDisposable
    {
        private IntPtr _string;
        private uint _disposed;

        public void Dispose()
        {
            if (_disposed == 0)
            {
                if (_string != IntPtr.Zero)
                {
                    Marshal.FreeCoTaskMem(_string);
                    _string = IntPtr.Zero;
                }

                _disposed = 1;
            }

            GC.SuppressFinalize(this);
        }

        public override string? ToString() => this;

        public static NativeString Null() => new NativeString() { _string = IntPtr.Zero };

        public static implicit operator NativeString(string? str) => new() { _string = Marshal.StringToCoTaskMemAuto(str) };
        public static implicit operator string?(NativeString str) => Marshal.PtrToStringAuto(str._string);
    }
}
