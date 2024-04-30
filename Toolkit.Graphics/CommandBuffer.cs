using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Toolkit.Renderer
{
    public class CommandBuffer
    {
        public static readonly CommandBuffer Default = new CommandBuffer();

        internal CommandBuffer()
        {
            
        }

        public unsafe void Reset() => _resetCommandBuffer();

        private static unsafe delegate*<void> _resetCommandBuffer = null;
    }
}
