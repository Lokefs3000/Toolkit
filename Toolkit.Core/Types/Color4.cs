using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Toolkit.Core.Types
{
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct Color4
    {
        public float R;
        public float G;
        public float B;
        public float A;

        public Color4(float r, float g, float b, float a)
        {
            R = r;
            G = g;
            B = b;
            A = a;
        }
    }
}
