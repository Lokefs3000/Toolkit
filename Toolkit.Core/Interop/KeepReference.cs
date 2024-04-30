using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Toolkit.Core.Interop
{
    internal class KeepReference
    {
        private static List<object> _keepReference = new List<object>();

        public static void Add(object obj)
        {
            _keepReference.Add(obj);
        }
    }
}
